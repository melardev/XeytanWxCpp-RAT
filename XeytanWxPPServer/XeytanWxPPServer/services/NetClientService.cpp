#include "NetClientService.h"
#include <wx/log.h>
#include "NetServerService.h"
#include "../../../shared/NetLib/packets/PacketPresentation.h"

NetClientService::NetClientService(NetServerService* server, wxSocketBase* clientSocket)
	: BaseNetClientService(clientSocket), server(server)
{
}


NetClientService::~NetClientService()
{
	wxPrintf(wxT("NetClientService destroyed"));
}

void NetClientService::onPacketReceived(const std::shared_ptr<Packet>& packet)
{
	if (packet->packetType == PacketType::Presentation)
	{
		const std::shared_ptr<PacketPresentation> packetPresentation =
			std::dynamic_pointer_cast<PacketPresentation>(packet);
		client.reset(new Client);
		client->clientId = clientSocket->GetSocket();
		client->pcName = packetPresentation->pcName;
		client->osName = packetPresentation->osName;
		client->username = packetPresentation->username;
	}
	server->onPacketReceived(client, packet);
}

#pragma once
#include "../../../shared/NetLib/services/BaseNetClientService.h"
#include <wx/sharedptr.h>
#include "../models/Client.h"

class NetServerService;

class NetClientService : public BaseNetClientService
{
public:
	
	NetClientService(NetServerService* server, wxSocketBase* clientSocket);
	~NetClientService();

	virtual void onPacketReceived(const std::shared_ptr<Packet>& packet) override;
	NetServerService* server;
	wxSharedPtr<Client> client;
};

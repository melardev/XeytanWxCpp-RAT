#pragma once
#include <wx/sharedptr.h>
#include <wx/wx.h>
#include "NetClientService.h"

#include "../concurrent/channels/double/NetAppEventDoubleQueueThreadChannel.h"
#include "../../../shared/ConcurrentLib/core/ThreadRunnableRunner.h"

WX_DECLARE_HASH_MAP(wxSOCKET_T, wxSharedPtr<NetClientService>, wxIntegerHash, wxIntegerEqual, Clients);

class NetServerService : public wxEvtHandler
{
public:
	NetServerService();
	~NetServerService();
	void setChannel(const wxSharedPtr<NetAppEventDoubleQueueThreadChannel>& channel);
	void runAsync();
	void OnServerEvent(wxSocketEvent& event);

	void onPacketReceived(const wxSharedPtr<Client>& client, const std::shared_ptr<Packet>& packet);

	static wxSharedPtr<NetServerService> createInstance();
	std::vector<wxSharedPtr<Client>> getClients();
	void listenAppEvents();
	wxSharedPtr<NetAppEventDoubleQueueThreadChannel> channel;
	wxThread* serverThread;
	wxIPV4address addr;
	wxSocketServer* serverSocket;

	Clients clients;
	bool running;
	ThreadRunnableRunner<NetServerService>* listenAppEventsThreads;
	wxMutex clientsMutex;

wxDECLARE_EVENT_TABLE();
};

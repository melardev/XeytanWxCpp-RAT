#pragma once
#include <wx/socket.h>
#include "../packets/packetdefs.h"
#include "../packets/Packet.h"

class BaseNetClientService : public wxEvtHandler
{
public:
	BaseNetClientService(wxSocketBase* clientSocket);
	~BaseNetClientService();
	void interactAsync();
	virtual void onPacketReceived(const std::shared_ptr<Packet>& packet) = 0;
	virtual void onConnectionSuccessful();
	void sendPacket(const std::shared_ptr<Packet>& packet);
	void readPacket(wxSocketBase* sock);
	void OnSocketEvent(wxSocketEvent& event);


	wxSocketBase* clientSocket;
	char receivingHeaderBuffer[PACKET_HEADER_LENGTH];
	bool headerOk;
	wxUint64 readenSoFar;
	char* payloadBuffer;
	wxUint64 packetSize;

	wxUint64 payloadAllocationSize;

wxDECLARE_EVENT_TABLE();
};

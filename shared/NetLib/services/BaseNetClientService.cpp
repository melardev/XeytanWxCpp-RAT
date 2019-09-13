#include "BaseNetClientService.h"
#include <wx/log.h>
#include <sstream>
#include <cereal/archives/binary.hpp>
#include "../packets/Packet.h"
#include <iomanip>
#include <fstream>


wxDECLARE_EVENT(ClientSocketId, wxSocketEvent);
wxDEFINE_EVENT(ClientSocketId, wxSocketEvent);

wxBEGIN_EVENT_TABLE(BaseNetClientService, wxEvtHandler)
		EVT_SOCKET(ClientSocketId, BaseNetClientService::OnSocketEvent)
wxEND_EVENT_TABLE()


BaseNetClientService::BaseNetClientService(wxSocketBase* clientSocket)
	: clientSocket(clientSocket), headerOk(false), readenSoFar(0)
{
}


BaseNetClientService::~BaseNetClientService()
{
}

void BaseNetClientService::interactAsync()
{
	clientSocket->SetEventHandler(*this, ClientSocketId);

	// clientSocket->SetNotify(wxSOCKET_INPUT_FLAG | wxSOCKET_LOST_FLAG);
	// Since we use this class for both server clients, and Client project
	// add wxSOCKET_CONNECTION_FLAG
	clientSocket->SetNotify(wxSOCKET_CONNECTION_FLAG | wxSOCKET_INPUT_FLAG | wxSOCKET_LOST_FLAG);
	clientSocket->Notify(true);
}


void BaseNetClientService::onConnectionSuccessful()
{
}

void BaseNetClientService::OnSocketEvent(wxSocketEvent& event)
{
	wxString s = "OnSocketEvent: ";
	wxSocketBase* sock = event.GetSocket();

	// m_text->AppendText(s);

	// Now we process the event
	switch (event.GetSocketEvent())
	{
	case wxSOCKET_CONNECTION:
		onConnectionSuccessful();
		break;
	case wxSOCKET_INPUT:
		{
			readPacket(sock);
			break;
		}
	case wxSOCKET_LOST:
		{
			// server->onClientConnectionClosed(sock->GetSocket());


			// wxLogMessage("Deleting socket.");

			break;
		}
	default: ;
	}

	// UpdateStatusBar();
}


void BaseNetClientService::readPacket(wxSocketBase* sock)
{
	wxPuts("readPacket()");
	// We disable input events, so that the test doesn't trigger
	// wxSocketEvent again.
	// sock->SetNotify(wxSOCKET_LOST_FLAG);

	// Which test are we going to run?
	if (!headerOk)
	{
		sock->Read(receivingHeaderBuffer + readenSoFar, PACKET_HEADER_LENGTH - readenSoFar);
		readenSoFar += sock->GetLastIOReadSize();
		// return if we don't have a full packet header
		if (readenSoFar < PACKET_HEADER_LENGTH)
		{
			// sock->SetNotify(wxSOCKET_LOST_FLAG | wxSOCKET_INPUT_FLAG);
			return;
		}

		headerOk = true;
		readenSoFar = 0;

		packetSize = *(wxUint64*)receivingHeaderBuffer;

		if (packetSize < 0 || packetSize > MAX_PACKET_SIZE)
		{
			// Invalid packetSize, reset
			headerOk = false;
			if (payloadAllocationSize > 0)
				memset(payloadBuffer, 0x00, payloadAllocationSize);

			// sock->SetNotify(wxSOCKET_LOST_FLAG | wxSOCKET_INPUT_FLAG);
			return;
		}
		else
		{
			// If we had previously allocated a char[] then reuse it or delete it
			if (payloadAllocationSize <= 0 || packetSize > payloadAllocationSize)
			{
				// delete previous buffer if allocated
				delete[] payloadBuffer;
				payloadBuffer = new char[packetSize];
				payloadAllocationSize = packetSize;
			}
		}
	}

	if (!sock->IsData())
	{
		// sock->SetNotify(wxSOCKET_LOST_FLAG | wxSOCKET_INPUT_FLAG);
		return;
	}
	sock->Read(payloadBuffer + readenSoFar, packetSize - readenSoFar);
	readenSoFar += sock->GetLastIOReadSize();
	if (readenSoFar < packetSize)
	{
		//  sock->SetNotify(wxSOCKET_LOST_FLAG | wxSOCKET_INPUT_FLAG);
		return;
	}

	std::shared_ptr<Packet> packet;
	std::string serializedStr;
	serializedStr.resize(packetSize);
	memcpy_s(&serializedStr[0], packetSize, payloadBuffer, packetSize);

	/* 
	// For debugging you may wanna dump the bytes to a file
	std::ofstream ofs("../../server_received.bin", std::ios::binary);
	ofs.write(serializedStr.data(), packetSize);
	ofs.close();
	*/

	{
		std::istringstream is(serializedStr);
		cereal::BinaryInputArchive iarchive(is);
		iarchive(packet);
	}

	headerOk = false;
	readenSoFar = 0;
	onPacketReceived(packet);

	// Enable input events again.
	// sock->SetNotify(wxSOCKET_LOST_FLAG | wxSOCKET_INPUT_FLAG);
}

void BaseNetClientService::sendPacket(const std::shared_ptr<Packet>& packet)
{
	wxPuts("sendPacket");
	
	/*
	 * IMO cereal is the easiest to use serialization cpp framework out there.
	 * But it is very fragile, if you try to serialize std::shared_ptr<Packet>
	 * You have to deserialize using std::shared_ptr<Packet>, if you serialize
	 * std::shared_ptr<PacketPresentation> deserialization 
	 * will not work with std::shared_ptr<Packet>, this is way any socket writing HAS
	 * to be done using this method which serializes everything as std::shared_ptr<Packet>.
	 * Then don't worry about polymorphism, cereal can detect that for you and call the serialize methods
	 * of the appropriate class/subclass.
	 * 
	 * I have tried to serialize uint64 0, then packet, then seek to the beginning of the
	 * oss and write the packet size using setw to guarantee header is always 8 bytes long
	 * that worked, but deserialization didn't, if you know how to make it work you canI have tried to serialize uint64 0, then packet, then seek to the beginning of the
	 * make a pull request, meanwhile I call Write twice. once for the size, other for payload.oss and write the packet size using setw to guarantee header is always 8 bytes long
	 * that worked, but deserialization didn't, if you know how to make it work you can
	 * make a pull request, meanwhile I call Write twice. once for the size, other for payload.
	 */
	std::ostringstream oss;
	{
		cereal::BinaryOutputArchive oarchive(oss);
		oarchive(packet);
	}

	// Another way of retrieving packet size, remember cursor must be at the end
	uint64_t serializedPacketSize = oss.tellp();
	std::string serializedStr = oss.str();

	const size_t serializedPacketLength = serializedStr.size();
	const void* serializedPacket = serializedStr.c_str();


	char packetHeader[PACKET_HEADER_LENGTH];
	memcpy_s(packetHeader, PACKET_HEADER_LENGTH, &serializedPacketSize, PACKET_HEADER_LENGTH);

	// For debugging we may want to dump binary content to file
	// std::ofstream ofs("../../client_send.bin", std::ios::binary);
	// ofs.write(serializedStr.data(), serializedPacketLength);
	// ofs.close();

	// Send packet length first
	clientSocket->Write(packetHeader, PACKET_HEADER_LENGTH);
	wxUint32 lastWriteSize = clientSocket->GetLastIOWriteSize();
	clientSocket->Write(serializedPacket, serializedPacketLength);
	lastWriteSize = clientSocket->GetLastIOWriteSize();
}

#pragma once
#include "../../shared/NetLib/services/BaseNetClientService.h"
#include "../../shared/NetLib/packets/PacketPresentation.h"
#include "../../shared/NetLib/models/FileInfo.h"
#include "../../shared/NetLib/packets/PacketProcess.h"

class Application;

class NetClientService : public BaseNetClientService
{
public:
	NetClientService(Application* application);
	~NetClientService();
	virtual void onPacketReceived(const std::shared_ptr<Packet>& packet) override;
	
	void onConnectionSuccessful() override;
	void start();
	void sendShellData(const wxString& data);
	void sendDirEntries(const wxString& path, const std::vector<FileInfo>& files);
	void sendProcesses(const std::vector<Process>& processes);

private:
	Application* application;
};


#pragma once
#include "concurrent/channels/double/UiAppEventDoubleQueueThreadChannel.h"
#include "../../shared/ConcurrentLib/core/ThreadRunnableRunner.h"
#include "../../shared/NetLib/packets/PacketProcess.h"

class IUiMediator
{
public:
	enum class View
	{
		Main,
		ReverseShell,
	};
	IUiMediator();
	~IUiMediator();
	void setUiChannel(const wxSharedPtr<UiAppEventDoubleQueueThreadChannel>& channel);
	bool handleInstruction(char* buffer);
	void mainLoop();
	wxSharedPtr<UiAppEventDoubleQueueThreadChannel> channel;
	ThreadRunnableRunner<IUiMediator>* appEventListenerThread;
	bool running;

	void printPrompt();
	void printClients(std::vector<wxSharedPtr<Client>>* vector);
	void printProcesses(const std::vector<Process>& processes);
	void listenAppEvents();



	wxSharedPtr<Client> activeClient;
	View currentView;
};

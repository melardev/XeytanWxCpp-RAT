#pragma once
#include <wx/wx.h>
#include "concurrent/channels/double/NetAppEventDoubleQueueThreadChannel.h"
#include "concurrent/channels/double/UiAppEventDoubleQueueThreadChannel.h"
#include "IUiMediator.h"
#include "../../shared/ConcurrentLib/core/ThreadRunnableRunner.h"
#include "services/NetServerService.h"

class ProcessService;

class Application : public wxApp
{
public:
	Application();
	~Application();
	bool OnInit() override;
	void run();
	void listenNetEvents();
	void listenUiEvents();
	bool running;

	wxSharedPtr<NetAppEventDoubleQueueThreadChannel> netChannel;
	wxSharedPtr<UiAppEventDoubleQueueThreadChannel> uiChannel;
	wxSharedPtr<IUiMediator> uiMediator;
	wxSharedPtr<NetServerService> netServerService;
	
	wxThread* netEventsListen;
	wxThread* uiEventsListen;
};

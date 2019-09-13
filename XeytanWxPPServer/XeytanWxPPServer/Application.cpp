#include "Application.h"
#include "concurrent/channels/double/NetAppEventDoubleQueueThreadChannel.h"
#include "concurrent/channels/double/UiAppEventDoubleQueueThreadChannel.h"
#include <wx/thread.h>
#include "../../shared/ConcurrentLib/core/ThreadRunnableRunner.h"


Application::Application()
{
}


Application::~Application()
{
}

bool Application::OnInit()
{
	running = true;
	netChannel = wxSharedPtr<NetAppEventDoubleQueueThreadChannel>(new NetAppEventDoubleQueueThreadChannel);
	uiChannel = wxSharedPtr<UiAppEventDoubleQueueThreadChannel>(new UiAppEventDoubleQueueThreadChannel);;

	uiMediator.reset(new IUiMediator);
	return true;
}

void Application::run()
{
	netEventsListen = new ThreadRunnableRunner<Application>(std::bind(&Application::listenNetEvents, this));
	uiEventsListen = new ThreadRunnableRunner<Application>(std::bind(&Application::listenUiEvents, this));

	std::_Binder<std::_Unforced, void( Application::*)(), Application*> handler1 = std::bind(
		&Application::listenUiEvents, this);


	uiEventsListen->Run();
	netEventsListen->Run();

	netServerService = NetServerService::createInstance();
	netServerService->setChannel(netChannel);
	netServerService->runAsync();

	uiMediator->setUiChannel(uiChannel);
	uiMediator->mainLoop();
}


void Application::listenNetEvents()
{
	while (running)
	{
		wxSharedPtr<AppEvent> netEvent = netChannel->takeFromNet();
		if (netEvent == nullptr)
		{
			std::cout << "NetEvent is null" << std::endl;
			continue;
		}

		Target target = netEvent->target;
		Subject subject = netEvent->subject;
		// wxSharedPtr<void> object = netEvent->object;
		Action action = netEvent->action;

		// Forward event to ui
		uiChannel->submitToUi(netEvent);
	}
}

void Application::listenUiEvents()
{
	while (running)
	{
		wxSharedPtr<AppEvent> uiEvent = uiChannel->takeFromUi();
		if (uiEvent == nullptr)
		{
			std::cout << "UiEvent is null" << std::endl;
			continue;
		}

		Target target = uiEvent->target;
		Subject subject = uiEvent->subject;
		// wxSharedPtr<void> object = uiEvent->object;
		Action action = uiEvent->action;

		
		// Forward event to net
		netChannel->submitToNet(uiEvent);
	}
}

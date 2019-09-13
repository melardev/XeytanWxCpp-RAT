#include "NetAppEventDoubleQueueThreadChannel.h"


NetAppEventDoubleQueueThreadChannel::NetAppEventDoubleQueueThreadChannel()
{
}

NetAppEventDoubleQueueThreadChannel::~NetAppEventDoubleQueueThreadChannel()
{
}

wxSharedPtr<AppEvent> NetAppEventDoubleQueueThreadChannel::takeFromApp()
{
	return takeFromRight();
}

wxSharedPtr<AppEvent> NetAppEventDoubleQueueThreadChannel::takeFromNet()
{
	return takeFromLeft();
}

void NetAppEventDoubleQueueThreadChannel::submitToApp(const wxSharedPtr<AppEvent>& event)
{
	submitToLeft(event);
}

void NetAppEventDoubleQueueThreadChannel::submitToNet(const wxSharedPtr<AppEvent>& event)
{
	submitToRight(event);
}

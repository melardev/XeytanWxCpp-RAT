#pragma once
#include "AppEventDoubleQueueThreadChannel.h"


class NetAppEventDoubleQueueThreadChannel : public AppEventDoubleQueueThreadChannel
{
public:
	NetAppEventDoubleQueueThreadChannel();
	~NetAppEventDoubleQueueThreadChannel();

	wxSharedPtr<AppEvent> takeFromApp();
	wxSharedPtr<AppEvent> takeFromNet();
	void submitToApp(const wxSharedPtr<AppEvent>& event);
	void submitToNet(const wxSharedPtr<AppEvent>& event);
};


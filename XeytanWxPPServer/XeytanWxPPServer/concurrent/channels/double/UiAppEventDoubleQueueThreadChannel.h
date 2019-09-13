#pragma once
#include "AppEventDoubleQueueThreadChannel.h"

class UiAppEventDoubleQueueThreadChannel : public AppEventDoubleQueueThreadChannel
{
public:
	UiAppEventDoubleQueueThreadChannel();
	~UiAppEventDoubleQueueThreadChannel();

	wxSharedPtr<AppEvent> takeFromApp();
	wxSharedPtr<AppEvent> takeFromUi();
	void submitToApp(const wxSharedPtr<AppEvent>& event);
	void submitToUi(const wxSharedPtr<AppEvent>& event);
};


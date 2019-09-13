#include "UiAppEventDoubleQueueThreadChannel.h"


UiAppEventDoubleQueueThreadChannel::UiAppEventDoubleQueueThreadChannel()
{
}

UiAppEventDoubleQueueThreadChannel::~UiAppEventDoubleQueueThreadChannel()
{
}

wxSharedPtr<AppEvent> UiAppEventDoubleQueueThreadChannel::takeFromApp()
{
	// The app posts on the left queue, so take from left
	return takeFromLeft();
}

wxSharedPtr<AppEvent> UiAppEventDoubleQueueThreadChannel::takeFromUi()
{
	// The ui posts on the left queue, so take from right
	return takeFromRight();
}

void UiAppEventDoubleQueueThreadChannel::submitToApp(const wxSharedPtr<AppEvent>& event)
{
	submitToRight(event);
}

void UiAppEventDoubleQueueThreadChannel::submitToUi(const wxSharedPtr<AppEvent>& event)
{
	submitToLeft(event);
}

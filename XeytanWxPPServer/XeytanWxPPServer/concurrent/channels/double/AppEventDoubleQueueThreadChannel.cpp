#include "AppEventDoubleQueueThreadChannel.h"
#include "../single/AppEventQueueThreadChannel.h"

class AppEvent;

AppEventDoubleQueueThreadChannel::AppEventDoubleQueueThreadChannel()
	: DoubleQueueThreadChannel<AppEvent>(wxSharedPtr<QueueThreadChannel<AppEvent>>(new AppEventQueueThreadChannel),
	                                     wxSharedPtr<QueueThreadChannel<AppEvent>>(new AppEventQueueThreadChannel))
{
}


AppEventDoubleQueueThreadChannel::~AppEventDoubleQueueThreadChannel()
{
}

#pragma once
#include "../../../../../shared/ConcurrentLib/channels/single/QueueThreadChannel.h"
#include "../../events/AppEvent.h"

class AppEventQueueThreadChannel : public QueueThreadChannel<AppEvent>
{
public:
	AppEventQueueThreadChannel();
	~AppEventQueueThreadChannel();
};

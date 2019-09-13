#pragma once
#include "../../../../../shared/ConcurrentLib/channels/double/DoubleQueueThreadChannel.h"
#include "../../events/AppEvent.h"

class AppEventDoubleQueueThreadChannel : public DoubleQueueThreadChannel<AppEvent>
{
public:
	AppEventDoubleQueueThreadChannel();
	~AppEventDoubleQueueThreadChannel();
};

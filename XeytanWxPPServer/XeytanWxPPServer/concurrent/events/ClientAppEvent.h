#pragma once
#include "AppEvent.h"
#include "../../models/Client.h"

class ClientAppEvent : public AppEvent
{
public:
	ClientAppEvent();
	~ClientAppEvent();


	ClientAppEvent(Subject subject, Action action, unsigned long long clientId);
	ClientAppEvent(Subject subject, Action action, const wxSharedPtr<Client>& client);
	ClientAppEvent(Subject subject, const wxSharedPtr<Client>& client);
	ClientAppEvent(Subject subject, Action action, const wxSharedPtr<Client>& client, const std::shared_ptr<void>& data);
	ClientAppEvent(Subject subject, const wxSharedPtr<Client>& client, const std::shared_ptr<void> data);

	wxSharedPtr<Client> getClient();
	wxSharedPtr<Client> client;
	unsigned long long clientId;
};

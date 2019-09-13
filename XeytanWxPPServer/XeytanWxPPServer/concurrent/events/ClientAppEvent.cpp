#include "ClientAppEvent.h"


ClientAppEvent::ClientAppEvent()
{
}


ClientAppEvent::~ClientAppEvent()
{
}

ClientAppEvent::ClientAppEvent(Subject subject, Action action, unsigned long long clientId)
	: AppEvent(Target::Client, subject, action), clientId(clientId)
{
}

ClientAppEvent::ClientAppEvent(Subject subject, Action action, const wxSharedPtr<Client>& client)
	: AppEvent(Target::Client, subject, action), client(client)
{
}

ClientAppEvent::ClientAppEvent(Subject subject, const wxSharedPtr<Client>& client)
	: AppEvent(Target::Client, subject), client(client), clientId(client->clientId)
{
}

ClientAppEvent::ClientAppEvent(Subject subject, Action action, const wxSharedPtr<Client>& client,
                               const std::shared_ptr<void>& data)
	: AppEvent(Target::Client, subject, action, data), client(client), clientId(client->clientId)
{
}

ClientAppEvent::ClientAppEvent(Subject subject, const wxSharedPtr<Client>& client, const std::shared_ptr<void> data)
	: AppEvent(Target::Client, subject, data), client(client)
{
}

wxSharedPtr<Client> ClientAppEvent::getClient()
{
	return client;
}

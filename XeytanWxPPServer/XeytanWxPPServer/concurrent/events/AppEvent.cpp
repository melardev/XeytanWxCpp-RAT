#include "AppEvent.h"
#include <iostream>

AppEvent::AppEvent()
{
}

AppEvent::AppEvent(Target target, Subject subject, Action action)
	: target(target), subject(subject), action(action)
{
}

AppEvent::AppEvent(Target target, Subject subject, Action action, const std::shared_ptr<void>& object)
	: target(target), subject(subject), action(action), object(object)
{
}

AppEvent::AppEvent(Target target, Subject subject,
                   const std::shared_ptr<void>& object)
	: target(target), subject(subject), action(Action::InvalidAction), object(object)
{
}

AppEvent::AppEvent(Target target, Subject subject, const wxString& description)
	: target(target), subject(subject), action(Action::InvalidAction), description(description)
{
}

AppEvent::AppEvent(Target target)
	: target(target)
{
}

AppEvent::~AppEvent()
{
	// std::cout << "AppEvent destructor called" << std::endl;
}

AppEvent::AppEvent(Target target, Subject subject)
	: target(target), subject(subject)
{
}

#pragma once
#include <wx/string.h>
#include <memory>
#include "../../models/Client.h"
#include <wx/sharedptr.h>

class wxString;

enum class Target
{
	InvalidTarget = -1,
	Application,
	Server,
	Client,
	Ui
};

enum class Subject
{
	InvalidSubject = -1,
	Interaction,
	// Used for console
	Connection,
	Desktop,
	Camera,
	Process,
	ReverseShell,
	FileSystem,
	Download,
	Information,
	PacketReceived,
	Error,
};

enum class Action
{
	InvalidAction = -1,
	ListAvailable,
	Start,
	Update,
	Fetch = Start,
	Push = Update,
	Pause,
	Stop
};


class AppEvent
{
public:
	AppEvent();

	AppEvent(Target target, Subject subject);

	AppEvent(Target target, Subject subject, Action action);
	AppEvent(Target target, Subject subject, Action action, const std::shared_ptr<void>& object);
	AppEvent(Target target, Subject subject, const std::shared_ptr<void>& object);

	AppEvent(Target target, Subject subject, const wxString& description);
	AppEvent(Target target);

	virtual ~AppEvent();
public:
	Target target;
	Subject subject;
	Action action;

	wxString description;

	std::shared_ptr<void> object;
};

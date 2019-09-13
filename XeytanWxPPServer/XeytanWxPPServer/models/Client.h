#pragma once
#include <wx/string.h>

class Client
{
public:
	Client();
	~Client();
	wxString pcName;
	wxString osName;
	wxString username;
	unsigned long long clientId;
};

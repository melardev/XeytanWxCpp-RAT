#pragma once
#include <wx/string.h>
class SystemService
{
public:
	SystemService();
	~SystemService();

	static wxString getOSName();
	static wxString getUsername();
	static wxString getPcName();

};


#include "SystemService.h"
#include "../appdefs.h"

#if defined(XEYTAN_WINDOWS)
#include <Windows.h>
#endif

SystemService::SystemService()
{
}


SystemService::~SystemService()
{
}

wxString SystemService::getOSName()
{
#ifdef XEYTAN_WINDOWS
	return "Windows";
#elif XEYTAN_LINUX
	return "Linux";
#endif
	return "Unknown";
}

wxString SystemService::getUsername()
{

#if defined(XEYTAN_WINDOWS)

	// wchar_t pcNameBuffer[255];
	// size_t pcNameLen = 255;
	// _wgetenv_s(&pcNameLen, pcNameBuffer, L"COMPUTERNAME");

	wchar_t userNameBuffer[255];
	DWORD userNameLen = 255;
	GetUserNameW(userNameBuffer, &userNameLen);
	return userNameBuffer;
#endif
}

wxString SystemService::getPcName()
{
	char computerName[255];
	gethostname(computerName, 255);
	return computerName;
}

#include "Application.h"
#include "appdefs.h"
#include <fstream>
#include "../../shared/NetLib/packets/PacketPresentation.h"
#include "concurrent/events/ClientAppEvent.h"
#include "../../shared/NetLib/packets/PacketProcess.h"

#ifdef XEYTAN_WINDOWS
#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "Rpcrt4.lib")
#pragma comment(lib, "ws2_32.lib")
#endif

IMPLEMENT_APP_NO_MAIN(Application);

#include <wx/dir.h>

void test()
{
	wxArrayString files;

	wxString file;

	wxDir dir("C:/");

	bool success = dir.GetFirst(&file);

	while (success)
	{
		success = dir.GetNext(&file);
		std::cout << file << std::endl;
	}
}

int main(int argc, char** argv)
{
	test();
	wxEntryStart(argc, argv);
	wxTheApp->CallOnInit();
	((Application*)wxTheApp)->run();

	wxTheApp->OnExit();
	wxEntryCleanup();
	return 0;
}

#include "Application.h"
#include "appdefs.h"

#ifdef XEYTAN_WINDOWS
#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "Rpcrt4.lib")
#pragma comment(lib, "ws2_32.lib")
#endif

wxIMPLEMENT_APP_NO_MAIN(Application);


void test()
{
	/*
	std::shared_ptr<PacketProcess> packet = std::make_shared<PacketProcess>();

	Process process;

	process.path = "C:/Windows/System32/calc.exe";
	process.pid = 3210;
	process.title = "Calculator";


	packet->processes.push_back(process);


	std::ostringstream oss;
	{
		cereal::BinaryOutputArchive oarchive(oss);
		oarchive(packet);
	}

	std::string serializedStr = oss.str();

	std::shared_ptr<PacketProcess> restored;
	{
		std::istringstream is(serializedStr);
		cereal::BinaryInputArchive iarchive(is);
		iarchive(restored);
	}
	*/
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

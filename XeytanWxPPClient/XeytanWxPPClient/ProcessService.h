#pragma once
#include <wx/app.h>
#include <wx/sharedptr.h>
#include "../../shared/ConcurrentLib/core/ThreadRunnableRunner.h"
#include <map>
#include "../../shared/NetLib/packets/PacketProcess.h"

class wxString;
class Application;
class RunnableThread;

class ProcessService : public wxEvtHandler
{
public:
	ProcessService(Application* application);

	static std::vector<Process> listProcesses();
#if defined(__WINDOWS__)
	static std::vector<Process> listProcessesWindows();
#endif
	static wxString getShellPath();
	// static processList getProcesses();
	void spawnShell();
	void writeToShell(const wxString& data);
	void stopShell();
	bool isShellActive();
	static std::map<std::string, std::string> getProcessEnvironment();
private:
	bool active;
	Application* application;
	wxProcess* process;
	long shellPid;
	RunnableThread* errorProcessReaderThread;
	RunnableThread* outputProcessReaderThread;
};

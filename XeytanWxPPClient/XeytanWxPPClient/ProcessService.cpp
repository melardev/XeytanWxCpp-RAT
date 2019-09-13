#include "ProcessService.h"
#include <wx/txtstrm.h>
#include <wx/process.h>
#include "../../shared/ConcurrentLib/core/ThreadRunnableRunner.h"
#include "Application.h"
#include "../../shared/ConcurrentLib/core/RunnableFuncThread.h"
#if defined(__WINDOWS__)

#include <Windows.h>
#include <TlHelp32.h>
#include <Psapi.h>

#pragma comment(lib, "psapi.lib")
#endif

ProcessService::ProcessService(Application* application)
	: application(application)
{
}


wxString ProcessService::getShellPath()
{
#if  defined(_WIN32) || defined(_WIN64)
	return "cmd";

#else

	if (wxFileExists("/bin/bash"))
		return "/bin/bash";
	else if (wxFileExists("/bin/sh"))
		return "/bin/sh";
	else
	{
		return "";
	}
#endif
}

void ProcessService::spawnShell()
{
	process = new wxProcess(this);
	process->Redirect();
	const wxString shellPath = getShellPath();
	shellPid = wxExecute(shellPath, wxEXEC_ASYNC | wxEXEC_HIDE_CONSOLE, process);


	// Process Output Stream
	outputProcessReaderThread = new RunnableThread([&]() -> void*
	{
		char buffer[4096];
		size_t bytesRead = 0;

		wxInputStream* in = process->GetInputStream();

		while ((bytesRead = in->Read(&buffer, sizeof(buffer)).LastRead()) > 0)
		{
			application->onProcOutput(wxString(buffer, 0, bytesRead));
		};
		return nullptr;
	});
	outputProcessReaderThread->Create();
	outputProcessReaderThread->Run();


	// Process Error Stream
	errorProcessReaderThread = new RunnableThread([&]() -> void*
	{
		char buffer[4096];
		size_t bytesRead = 0;
		wxInputStream* in = process->GetErrorStream();
		while ((bytesRead = in->Read(&buffer, sizeof(buffer)).LastRead()) > 0)
		{
			// application->OnProcOutput(wxString(buffer, 0, bytesRead));
		}

		return nullptr;
	});

	errorProcessReaderThread->Create();
	errorProcessReaderThread->Run();
}

void ProcessService::writeToShell(const wxString& command)
{
	wxOutputStream* processOutputStream = process->GetOutputStream();
	if (processOutputStream)
	{
		wxTextOutputStream textOutputStream(*processOutputStream);
		// I always prefer to use TextOutputStream.WriteString() but you can also use the low level wxOutputStream.WriteAll
		if (command.EndsWith(wxT('\n')))
			processOutputStream->WriteAll(command.c_str(), command.size());
		else
		{
			textOutputStream.WriteString(command + wxT('\n'));
			textOutputStream.Flush();
		}
	}
}

void ProcessService::stopShell()
{
	process->CloseOutput();
}

bool ProcessService::isShellActive()
{
	return active;
}

std::map<std::string, std::string> ProcessService::getProcessEnvironment()
{
	std::map<std::string, std::string> env;

	wxEnvVariableHashMap environment;
	wxGetEnvMap(&environment);

	for (auto it = environment.begin(); it != environment.end(); ++it)
	{
		env.insert({it->first, it->second});
	}

	return env;
}


std::vector<Process> ProcessService::listProcesses()
{
#ifdef __WINDOWS__
	return listProcessesWindows();
#else
	return std::vector<Process>();
#endif
}

#if defined(__WINDOWS__)

BOOL CALLBACK onEnumWindowProc(HWND hwnd, LPARAM lp)
{
	Process* processModel = (Process*)lp;
	processModel->hasUi = IsWindowVisible(hwnd);

	int bufferLength = GetWindowTextLengthW(hwnd);
	wchar_t* buffer = new wchar_t[bufferLength + 1];
	GetWindowTextW(hwnd, buffer, bufferLength + 1);
	processModel->title = buffer;
	delete[] buffer;
	return TRUE;
}

std::vector<Process> ProcessService::listProcessesWindows()
{
	std::vector<Process> processes;
	HANDLE handle = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	PROCESSENTRY32W pe;
	pe.dwSize = sizeof(PROCESSENTRY32W); // If not set the call Process32FirstW fails

	std::vector<Process>::const_iterator it = processes.cbegin();

	if (Process32First(handle, &pe))
	{
		do
		{
			Process process;
			process.pid = pe.th32ProcessID;

			process.path = pe.szExeFile;
			HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, pe.th32ProcessID);

			if (hProcess != NULL)
			{
				// We have successfully opened the process for query
				wchar_t filePath[MAX_PATH];

				if (GetModuleFileNameExW(hProcess, NULL, filePath, MAX_PATH))
				{
					process.path = filePath;
				}

				CloseHandle(hProcess);
			}

			EnumWindows(onEnumWindowProc, (LPARAM)&process);
			processes.push_back(process);
		}
		while (Process32NextW(handle, &pe));
	}

	return processes;
}
#endif

#pragma once
#include <wx/wx.h>
#include "NetClientService.h"

class ProcessService;

class Application : public wxApp
{
public:
	Application();
	~Application();
	void run();
	void onProcOutput(const wxString& data);
	void onInfoRequest();
	void onListDir(const wxString& path);
	void sendFsRoots();
	void sendListDir(const wxString& path);
	void onFsRequest(wxString& path);
	void onProcessRequest() const;
	wxSharedPtr<NetClientService> netClientService;
	bool OnInit() override;
	void onShellAction(ShellAction action, const wxString& data) const;

	wxSharedPtr<ProcessService> processService;
};


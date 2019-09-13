#pragma once
#include <vector>
#include <wx/wx.h>
#include "../../shared/NetLib/models/FileInfo.h"

class FileSystemService
{
public:
	FileSystemService();
	~FileSystemService();
	static const char* driveTypeIntToString(int type);


	static std::vector<FileInfo> retrieveRoots();

#ifdef __WINDOWS__
	static std::vector<FileInfo> retrieveRootsWindows();
#endif

	static void retrieveListDir(const wxString& path, std::vector<FileInfo>& dirEntries);
};

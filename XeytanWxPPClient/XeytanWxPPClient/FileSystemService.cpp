#include "FileSystemService.h"
#include <wx/dir.h>
#include <wx/filename.h>

FileSystemService::FileSystemService()
{
}

FileSystemService::~FileSystemService()
{
}

const char* FileSystemService::driveTypeIntToString(int type)
{
	switch (type)
	{
	case DRIVE_FIXED:
		return "fixed";
	case DRIVE_CDROM:
		return "cdrom";
	case DRIVE_REMOVABLE:
		return "removable";
	case DRIVE_UNKNOWN:
		return "unknown";
	case DRIVE_NO_ROOT_DIR:
		return "unmounted";
	case DRIVE_REMOTE:
		return "remote";
	case DRIVE_RAMDISK:
		return "ramdisk";
	default:
		return "?";
	}
}

std::wstring string2Wstring(const std::string& str)
{
#if defined(__WINDOWS__)
	int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
	std::wstring wstrTo(size_needed, 0);
	MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
	return wstrTo;
#elif defined(__LINUX__)

#endif
}

std::vector<FileInfo> FileSystemService::retrieveRootsWindows()
{
	std::vector<FileInfo> roots;
#if defined(__WINDOWS__)

	char driveLetters[255];

	char* currentLetter = driveLetters;

	char opts[20];

	char fsType[MAX_PATH + 1] = {0};
	DWORD pflags = 0;

	const int64_t numBytes = GetLogicalDriveStringsA(254, currentLetter);

	if (numBytes == 0)
	{
		return roots;
	}

	while (*currentLetter != 0)
	{
		FileInfo info;

		opts[0] = 0;
		fsType[0] = 0;


		const int driveType = GetDriveTypeA(currentLetter);

		const BOOL success = GetVolumeInformationA(
			currentLetter, NULL, _ARRAYSIZE(currentLetter),
			NULL, NULL, &pflags, fsType,
			_ARRAYSIZE(fsType));

		if (success)
		{
			if (pflags & FILE_READ_ONLY_VOLUME)
				strcat_s(opts, _countof(opts), "ro");
			else
				strcat_s(opts, _countof(opts), "rw");
			if (pflags & FILE_VOLUME_IS_COMPRESSED)
				strcat_s(opts, _countof(opts), ",compressed");
		}
		else
		{
			// We might get here in case of a floppy hard drive, in
			// which case the error is (21, "device not ready").
			// Let's pretend it didn't happen as we already have
			// the drive name and type ('removable').
			strcat_s(opts, _countof(opts), "");
		}

		if (strlen(opts) > 0)
			strcat_s(opts, _countof(opts), ",");

		strcat_s(opts, _countof(opts), driveTypeIntToString(driveType));

		info.filePath = string2Wstring(std::string(currentLetter) + " - " + opts);
		roots.push_back(info);

		currentLetter = strchr(currentLetter, 0) + 1;
	}

	return roots;

#endif
}


std::vector<FileInfo> FileSystemService::retrieveRoots()
{
#ifdef __WINDOWS__
	return retrieveRootsWindows();
#else
	return std::vector<FileInfo>();
#endif
}


void FileSystemService::retrieveListDir(const wxString& path, std::vector<FileInfo>& dirEntries)
{
	if (!wxDir::Exists(path))
		return;

	wxDir dir(path);

	wxString filename;
	bool success = dir.GetFirst(&filename);


	while (success)
	{
		FileInfo f = {};
		f.filePath = path;

		if (!path.EndsWith("/"))
			f.filePath += L"/";

		f.filePath += filename;

		if (wxFile::Exists(f.filePath))
		{
			wxFileName ifn(f.filePath);
			wxULongLong result = wxFileName::GetSize(f.filePath);
			f.size = result.ToULong();
			f.lastUpdateTime = ifn.GetModificationTime().GetTicks();
		}
		else
		{
			f.size = -1;
		}

		dirEntries.push_back(f);
		success = dir.GetNext(&filename);
	}
}

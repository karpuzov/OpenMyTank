
//-----------------------------------------------------------------------------

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <tchar.h>
#include <Shlobj.h>
#include <Shlwapi.h>
#include <Shellapi.h>

#include <string>
#include <fstream>

#pragma comment(lib, "Shlwapi.lib")

typedef  std::basic_string<TCHAR>  tstring;

//-----------------------------------------------------------------------------

bool GetAccountsDirectory(TCHAR path[])
{
	if (SUCCEEDED(::SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, 0, path)))
	{
		::PathAppend(path, TEXT("Macromedia\\Flash Player\\#SharedObjects\\"));

		TCHAR mask[MAX_PATH];
		::lstrcpy(mask, path);
		::PathAppend(mask, TEXT("*"));

		WIN32_FIND_DATA findFileData;

		CONST HANDLE hFind = ::FindFirstFile(mask, &findFileData);
		if (hFind != INVALID_HANDLE_VALUE)
		{
			do
			{
				if (findFileData.cFileName[0] != '.'
					&& findFileData.cFileName[1] != '.'
					&& findFileData.cFileName[2] != '\0')
				{
					::PathAppend(path, findFileData.cFileName);
					::PathAddBackslash(path);
					::FindClose(hFind);
					return true;
				}
			} while (::FindNextFile(hFind, &findFileData));
			::FindClose(hFind);
		}
	}
	return false;
}

bool RemoveFolder(tstring folderPath)
{
	folderPath += TEXT('\0');

	SHFILEOPSTRUCT operation;
	operation.hwnd = NULL;
	operation.wFunc = FO_DELETE;
	operation.pFrom = folderPath.c_str();
	operation.pTo = NULL;
	operation.fFlags = FOF_SILENT | FOF_NOCONFIRMATION | FOF_NOERRORUI | FOF_NOCONFIRMMKDIR;
	operation.fAnyOperationsAborted = FALSE;
	operation.hNameMappings = NULL;
	operation.lpszProgressTitle = NULL;

	return bool(0 == ::SHFileOperation(&operation));
}

tstring OemToTChar(const std::string& text)
{
	tstring result(text.begin(), text.end());
	::OemToChar(text.c_str(), &(result[0]));
	return result;
}

//-----------------------------------------------------------------------------

tstring GetAccountFromFile()
{
	std::ifstream file("accounts.cfg");
	std::string line;
	std::getline(file, line);
	if (!file.fail())
	{
		return OemToTChar(line);
	}
	return tstring();
}

bool UnSetup()
{
	TCHAR path[MAX_PATH];
	if (GetAccountsDirectory(path))
	{
		tstring p(path);

		RemoveFolder(p + tstring(TEXT("www.mytank.narod.ru")) + TEXT(".tankionline.com"));

		RemoveFolder(p + tstring(TEXT("mytank")) + TEXT(".tankionline.com"));

		const tstring account = GetAccountFromFile();
		if (!account.empty())
		{
			RemoveFolder(p + account + TEXT(".tankionline.com"));
		}
		return true;
	}
	return false;
}

//-----------------------------------------------------------------------------

int _tmain(int argc, _TCHAR* argv[])
{
	return !UnSetup();
}

//-----------------------------------------------------------------------------

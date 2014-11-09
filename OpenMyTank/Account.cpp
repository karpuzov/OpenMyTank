//-----------------------------------------------------------------------------

#include "Account.h"
#include "TankiOnline.h"

#include <windows.h>
#include <Shlwapi.h>
#include <Shlobj.h>

#include <fstream>
#include <vector>
#include <algorithm>

//-----------------------------------------------------------------------------

extern std::auto_ptr<TO::UrlManager> BattleUrl;

//-----------------------------------------------------------------------------

namespace Flash
{

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
                if (findFileData.cFileName[0] != '.' && findFileData.cFileName[1] != '.'
                        && findFileData.cFileName[2] != '\0')
                {
                    ::PathAppend(path, findFileData.cFileName);
                    ::PathAddBackslash(path);
                    ::FindClose(hFind);
                    return true;
                }
            }
            while (::FindNextFile(hFind, &findFileData));
            ::FindClose(hFind);
        }
    }
    return false;
}

//-----------------------------------------------------------------------------

bool IsAccountExists()
{
    TCHAR path[MAX_PATH];
    if (GetAccountsDirectory(path))
    {
        ::PathAppend(path, BattleUrl->getAccountCoockieName().c_str());

        return bool(FALSE != ::PathFileExists(path));
    }
    return false;
}

//-----------------------------------------------------------------------------

}// namespace Flash

//-----------------------------------------------------------------------------

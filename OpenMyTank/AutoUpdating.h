//-----------------------------------------------------------------------------

#pragma once

//-----------------------------------------------------------------------------

#include <windows.h>
#include <string>

//-----------------------------------------------------------------------------

template <typename stringType>
stringType GetThisFileName()
{
    TCHAR sourcePath[MAX_PATH];
    if (::GetModuleFileName(NULL, sourcePath, MAX_PATH) != 0)
    {
        return stringType(_tcsrchr(sourcePath, '\\') + 1, sourcePath + _tcslen(sourcePath));
    }
    return stringType();
}

void ForAutoupdate()
{
    const tstring myTankExe(TEXT("my_tank.exe"));
    const tstring newMyTankExe(TEXT("new_my_tank.exe"));
    if (GetThisFileName<const tstring>() == newMyTankExe)
    {
        ::CopyFile(newMyTankExe.c_str(), myTankExe.c_str(), FALSE);
    }
}

//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------

#include "file_utils.h"

#include <Shlobj.h>

//-----------------------------------------------------------------------------

static int WINAPI BrowseCallbackProc(HWND hwnd, UINT message, LPARAM, LPARAM lParam)
{
    if (message == BFFM_INITIALIZED)
    {
        ::SendMessage(hwnd, BFFM_SETSELECTION, TRUE, lParam);
    }
    return 0;
}

//-----------------------------------------------------------------------------

tstring GetFolder(const TCHAR title[], const TCHAR initialPath[], const HWND hwndOwner)
{
    TCHAR path[MAX_PATH] =
    { TEXT('\0') };
    bool result = false;

    LPMALLOC pMalloc;
    if (::SHGetMalloc(&pMalloc) == NOERROR)
    {
        BROWSEINFO bi =
        { 0 };

        bi.ulFlags = BIF_RETURNONLYFSDIRS;
        bi.hwndOwner = hwndOwner;
        bi.lpszTitle = title;
        //bi.lpfn      = NULL;
        bi.ulFlags |= BIF_STATUSTEXT;
        bi.lParam = LPARAM(path);
        if (initialPath != NULL)
        {
            bi.lpfn = BrowseCallbackProc;
            bi.lParam = (LPARAM)initialPath;
        }

        LPITEMIDLIST pidl = ::SHBrowseForFolder(&bi);
        if (pidl != NULL)
        {
            if (::SHGetPathFromIDList(pidl, path))
            {
                result = true;
            }
            pMalloc->Free(pidl);
        }
        if (bi.pidlRoot != NULL)
        {
            pMalloc->Free(&bi);
        }
        pMalloc->Release();
    }

    return tstring((result) ? (path) : (initialPath));
}

//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------

#pragma once

//-----------------------------------------------------------------------------

#include <windows.h>

#include <limits>
#include <sstream>

//-----------------------------------------------------------------------------

void MoveToCenterOfParentWindow(const HWND hwnd)
{
    RECT parentRect;
    if (!::GetWindowRect(::GetParent(hwnd), &parentRect))
    {
        parentRect.left = parentRect.top = 0;
        parentRect.right = ::GetSystemMetrics(SM_CXSCREEN);
        parentRect.bottom = ::GetSystemMetrics(SM_CYSCREEN);
    }
    // right and bottom on 1 great!

    RECT rc;
    ::GetWindowRect(hwnd, &rc);
    ::SetWindowPos(hwnd,
    NULL, parentRect.left + ((parentRect.right - parentRect.left) - (rc.right - rc.left)) / 2,
            parentRect.top + ((parentRect.bottom - parentRect.top) - (rc.bottom - rc.top)) / 2, 0,
            0,
            SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_NOSIZE);
}

//-----------------------------------------------------------------------------

tstring GetWindowString(const HWND editControl, int maxSymbolsWithoutZero = -1)
{
    if (maxSymbolsWithoutZero == -1)
    {
        maxSymbolsWithoutZero = ::GetWindowTextLength(editControl);
    }
    tstring result;
    if (maxSymbolsWithoutZero > 0)
    {
        TCHAR* text = new TCHAR[maxSymbolsWithoutZero + 1];
        ::GetWindowText(editControl, text, maxSymbolsWithoutZero + 1);
        result.assign(text);
        delete[] text;
    }
    return result;
}

//-----------------------------------------------------------------------------

unsigned GetEditNumber(const HWND editControl)
{
    std::basic_istringstream<TCHAR> stream(GetWindowString(editControl));
    unsigned number;
    stream >> number;
    return number;
}

//-----------------------------------------------------------------------------

void SetEditNumber(const HWND editControl, const unsigned number)
{
    std::basic_ostringstream<TCHAR> stream;
    stream << number;
    ::SetWindowText(editControl, stream.str().c_str());
}

//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------

#pragma once

//-----------------------------------------------------------------------------

#include <windows.h>

#include <string>

//-----------------------------------------------------------------------------

class ClipboardManager
{
public:

    ClipboardManager(const HWND window)
            : hwnd(window), nextHwnd(NULL), isHooked(false)
    {
    }

    ~ClipboardManager()
    {
        unhookChanges();
    }

    void hookChanges()
    {
        if (!isHooked)
        {
            isHooked = true;
            nextHwnd = ::SetClipboardViewer(hwnd);
        }
    }

    void unhookChanges()
    {
        if (isHooked)
        {
            isHooked = false;
            ::ChangeClipboardChain(hwnd, nextHwnd);
        }
    }

    LRESULT do_WM_CHANGECBCHAIN(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
        if ((HWND)wParam == nextHwnd)
        {
            nextHwnd = (HWND)lParam;
        }
        else if (nextHwnd != NULL)
        {
            ::SendMessage(nextHwnd, message, wParam, lParam);
        }
        return 0;
    }

    LRESULT do_WM_DRAWCLIPBOARD(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
        ::SendMessage(nextHwnd, message, wParam, lParam);
        return 0;
    }

    bool write(const std::basic_string<TCHAR>& stringToClipboard)
    {
        return write(stringToClipboard, hwnd);
    }

    std::basic_string<TCHAR> read() const
    {
        return read(hwnd);
    }

    static bool write(const std::basic_string<TCHAR>& stringToClipboard, const HWND hwnd)
    {
        bool success = false;
        if (::OpenClipboard(hwnd))
        {
            ::EmptyClipboard();

            const HGLOBAL memory = ::GlobalAlloc(GMEM_MOVEABLE,
                    (stringToClipboard.length() + 1) * sizeof(TCHAR));
            if (memory != NULL)
            {
                TCHAR* const newString = (TCHAR* const )::GlobalLock(memory);
                //std::copy(stringToClipboard.begin(), stringToClipboard.end(), newString); // warning showed!
                ::CopyMemory(newString, stringToClipboard.data(), (stringToClipboard.length() + 1) * sizeof(TCHAR));
                ::GlobalUnlock(memory);

                ::SetClipboardData(CF_UNICODETEXT, memory);

                success = true;
            }
            ::CloseClipboard();
        }
        return success;
    }

    static std::basic_string<TCHAR> read(const HWND hwnd)
    {
        std::basic_string<TCHAR> result;

        if (::IsClipboardFormatAvailable(CF_UNICODETEXT))
        {
            if (::OpenClipboard(hwnd))
            {
                const HANDLE memory = ::GetClipboardData(CF_UNICODETEXT);
                if (memory != NULL)
                {
                    const TCHAR* const text = (const TCHAR* const )::GlobalLock(memory);
                    if (text != NULL)
                    {
                        result.assign(text);
                        ::GlobalUnlock(memory);
                    }
                }
                ::CloseClipboard();
            }
        }

        return result;
    }

private:
    HWND hwnd;
    HWND nextHwnd;
    bool isHooked;
};

//-----------------------------------------------------------------------------

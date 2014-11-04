
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
    : Hwnd(window)
    , NextHwnd(NULL)
    , IsHooked(false)
  {
  }

  ~ClipboardManager()
  {
    UnhookChanges();
  }

  void HookChanges()
  {
    if (!IsHooked)
    {
      IsHooked = true;
      NextHwnd = ::SetClipboardViewer(Hwnd);
    }
  }

  void UnhookChanges()
  {
    if (IsHooked)
    {
      IsHooked = false;
      ::ChangeClipboardChain(Hwnd, NextHwnd);
    }
  }

  LRESULT Do_WM_CHANGECBCHAIN(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
  {
    if ((HWND)wParam == NextHwnd)
    {
      NextHwnd = (HWND)lParam;
    }
    else if (NextHwnd != NULL)
    {
      ::SendMessage(NextHwnd, message, wParam, lParam);
    }
    return 0;
  }

  LRESULT Do_WM_DRAWCLIPBOARD(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
  {
    ::SendMessage(NextHwnd, message, wParam, lParam);
    return 0;
  }

  bool Write(const std::basic_string<TCHAR>& stringToClipboard)
  {
    return Write(stringToClipboard, Hwnd);
  }

  std::basic_string<TCHAR> Read() const
  {
    return Read(Hwnd);
  }

  static bool Write(const std::basic_string<TCHAR>& stringToClipboard, const HWND hwnd)
  {
    bool success = false;
    if (::OpenClipboard(hwnd))
    {
      ::EmptyClipboard();

      const HGLOBAL memory = ::GlobalAlloc(GMEM_MOVEABLE, (stringToClipboard.length() + 1) * sizeof(TCHAR));
      if (memory != NULL)
      {
        TCHAR*const newString = (TCHAR*const)::GlobalLock(memory);
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

  static std::basic_string<TCHAR> Read(const HWND hwnd)
  {
    std::basic_string<TCHAR> result;

    if (::IsClipboardFormatAvailable(CF_UNICODETEXT))
    {
      if (::OpenClipboard(hwnd))
      {
        const HANDLE memory = ::GetClipboardData(CF_UNICODETEXT);
        if (memory != NULL)
        {
          const TCHAR*const text = (const TCHAR*const)::GlobalLock(memory);
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
  HWND  Hwnd;
  HWND  NextHwnd;
  bool  IsHooked;
};

//-----------------------------------------------------------------------------

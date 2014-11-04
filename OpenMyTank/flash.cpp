
//-----------------------------------------------------------------------------

#include "flash.h"
#include "web_browser.h"
#include "tanki_online.h"
#include "settings.h"
#include "chat_writer.h"

#include <atlbase.h>

#include <cassert>

//-----------------------------------------------------------------------------

extern HINSTANCE Instance;

ChatWriter* GlobalChatWriter = NULL;
SHORT GlobalSnapshotKey = 0;

//-----------------------------------------------------------------------------

static DWORD GetPixelColor(const HWND window, const int x, const int y)
{
  const HDC dc = ::GetDC(window);
  const DWORD color = ::GetPixel(dc, x, y);
  ::ReleaseDC(window, dc);
  return color;
}

//-----------------------------------------------------------------------------

namespace Flash
{

//-----------------------------------------------------------------------------

static CComModule ComModule;

static PlayerWindow* FlashPlayerObject = NULL;

//-----------------------------------------------------------------------------

PlayerWindow::PlayerWindow(CONST HWND parentWindow, CONST TCHAR url[])
  : AtlWindow(NULL)
  , FlashHwnd(NULL)
  , RemainTimerIterations(60 * 10)
  , CurrentChatWriter(NULL)
{
  assert(FlashPlayerObject == NULL);
  FlashPlayerObject = this;

  AtlWindow = ::CreateWindow(TEXT(ATLAXWIN_CLASS),
                             url,
                             WS_VISIBLE | WS_CHILD,
                             0, 0, 0, 0,
                             parentWindow, NULL, Instance, NULL);
  if (AtlWindow == NULL)
  {
    throw TEXT("Cannot create ActiveX window");
  }

  ::SetWindowPos(AtlWindow, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

  Browser.reset(new WebBrowser(AtlWindow));

  ::SetTimer(AtlWindow, GetInitialTimerId(), 100, (TIMERPROC)InitialTimerProc);

  GlobalSnapshotKey = GetSettings()->SnapshotKey;
}

//-----------------------------------------------------------------------------

PlayerWindow::~PlayerWindow()
{
  GlobalChatWriter = NULL;
  delete CurrentChatWriter;

  ReleaseAll();

  assert(FlashPlayerObject != NULL);
  FlashPlayerObject = NULL;
}

//-----------------------------------------------------------------------------

HWND
PlayerWindow::GetFlashHwnd() const
{
  if (FlashHwnd == NULL)
  {
    InitialTimerProc(NULL, 0, GetInitialTimerId(), 0);
  }
  return FlashHwnd;
}

//-----------------------------------------------------------------------------

void
PlayerWindow::MoveWindow(CONST RECT& rect)
{
  WindowSize.cx = rect.right - rect.left + 1;
  WindowSize.cy = rect.bottom - rect.top + 1;
  ::MoveWindow(AtlWindow, rect.left, rect.top, WindowSize.cx, WindowSize.cy, TRUE);
}

//-----------------------------------------------------------------------------

SIZE
PlayerWindow::GetSize() const
{
  return WindowSize;
}

//-----------------------------------------------------------------------------

tstring
PlayerWindow::GetBattleUrl() const
{
  assert(Browser.get() != NULL);
  return Browser->GetUrl();
}

//-----------------------------------------------------------------------------

void
PlayerWindow::ClickLeftPlayButton() const
{
  assert(GetFlashHwnd() != NULL);
  const POINTS points = TO::GetLeftPlayButtonClickPoint(WindowSize);
  const LPARAM lParam = (LPARAM)(MAKELPARAM(points.x, points.y));
  ::PostMessage(FlashHwnd, WM_LBUTTONDOWN, 0, lParam);
  ::PostMessage(FlashHwnd, WM_LBUTTONUP, 0, lParam);
}

//-----------------------------------------------------------------------------

void
PlayerWindow::ClickRightPlayButton() const
{
  assert(GetFlashHwnd() != NULL);
  const POINTS points = TO::GetRightPlayButtonClickPoint(WindowSize);
  const LPARAM lParam = (LPARAM)(MAKELPARAM(points.x, points.y));
  ::PostMessage(FlashHwnd, WM_LBUTTONDOWN, 0, lParam);
  ::PostMessage(FlashHwnd, WM_LBUTTONUP, 0, lParam);
}

//-----------------------------------------------------------------------------

bool
PlayerWindow::IsBattleHappens() const
{
  const DWORD color = GetPixelColor(GetFlashHwnd(), WindowSize.cx - 20, WindowSize.cy - 11);
  return bool(color == 0xFFFFFF);
}

//-----------------------------------------------------------------------------

void
PlayerWindow::ReleaseAll()
{
  ::KillTimer(AtlWindow, GetInitialTimerId());
  Browser.reset();
  if (AtlWindow != NULL)
  {
    ::DestroyWindow(AtlWindow);
    AtlWindow = NULL;
  }
}

//-----------------------------------------------------------------------------

UINT
PlayerWindow::GetInitialTimerId() const
{
  return (UINT)(this);
}

//-----------------------------------------------------------------------------

void CALLBACK
PlayerWindow::InitialTimerProc(HWND /*hwnd*/, UINT /*message*/, UINT thisPointer, DWORD /*time*/)
{
  PlayerWindow* object = reinterpret_cast<PlayerWindow*>(thisPointer);
  HWND hwnd = ::FindWindowEx(FlashPlayerObject->AtlWindow, NULL, NULL, NULL);
  if (hwnd != NULL)
  {
    hwnd = ::FindWindowEx(hwnd, NULL, NULL, NULL);
    if (hwnd != NULL)
    {
      hwnd = ::FindWindowEx(hwnd, NULL, NULL, NULL);
      if (hwnd != NULL)
      {
        const HWND parent = hwnd;
        hwnd = NULL;
        do
        {
          hwnd = ::FindWindowEx(parent, hwnd, NULL, NULL);
          if (hwnd != NULL)
          {
            RECT rc;
            ::GetClientRect(hwnd, &rc);
            if (rc.bottom == 1 || rc.right == 1)
            {
              object->RemainTimerIterations = 0;
              ::ShowWindow(hwnd, SW_HIDE);
            }
            else
            {
              object->FlashHwnd = hwnd;
              object->CurrentChatWriter = CreateChatWriterForWindow(hwnd);
              GlobalChatWriter = object->CurrentChatWriter;
            }
          }
        }
        while (hwnd != NULL);
        if (object->RemainTimerIterations <= 0)
        {
          ::KillTimer(FlashPlayerObject->AtlWindow, thisPointer);
        }
      }
    }
  }
  --object->RemainTimerIterations;
}

//-----------------------------------------------------------------------------

void
PlayerWindow::SendMessage(UINT message, WPARAM wParam, LPARAM lParam)
{
  if (GetFlashHwnd() != NULL)
  {
    ::SendMessage(GetFlashHwnd(), message, wParam, lParam);
  }
}

//-----------------------------------------------------------------------------

void
PlayerWindow::PostMessage(UINT message, WPARAM wParam, LPARAM lParam)
{
  if (GetFlashHwnd() != NULL)
  {
    ::PostMessage(GetFlashHwnd(), message, wParam, lParam);
  }
}

//-----------------------------------------------------------------------------

void
PlayerWindow::ActivateWindow(CONST HWND activateWindow) const
{
  if (GetFlashHwnd() != NULL)
  {
    ::SetForegroundWindow(GetFlashHwnd());
    ::SetActiveWindow(GetFlashHwnd());
    ::PostMessage(GetFlashHwnd(), WM_ACTIVATE, (WPARAM)WA_CLICKACTIVE, (LPARAM)activateWindow);
  }
}

//-----------------------------------------------------------------------------

} // namespace Flash

//-----------------------------------------------------------------------------

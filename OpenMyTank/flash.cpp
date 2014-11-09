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
        : atlWindow(NULL), flashHwnd(NULL), remainTimerIterations(60 * 10), currentChatWriter(NULL)
{
    assert(FlashPlayerObject == NULL);
    FlashPlayerObject = this;

    atlWindow = ::CreateWindow(TEXT(ATLAXWIN_CLASS),
            url,
            WS_VISIBLE | WS_CHILD,
            0, 0, 0, 0,
            parentWindow, NULL, Instance, NULL);
    if (atlWindow == NULL)
    {
        throw TEXT("Cannot create ActiveX window");
    }

    ::SetWindowPos(atlWindow, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

    browser.reset(new WebBrowser(atlWindow));

    ::SetTimer(atlWindow, getInitialTimerId(), 100, (TIMERPROC)initialTimerProc);

    GlobalSnapshotKey = GetSettings()->snapshotKey;
}

//-----------------------------------------------------------------------------

PlayerWindow::~PlayerWindow()
{
    GlobalChatWriter = NULL;
    delete currentChatWriter;

    releaseAll();

    assert(FlashPlayerObject != NULL);
    FlashPlayerObject = NULL;
}

//-----------------------------------------------------------------------------

HWND
PlayerWindow::getFlashHwnd() const
{
    if (flashHwnd == NULL)
    {
        initialTimerProc(NULL, 0, getInitialTimerId(), 0);
    }
    return flashHwnd;
}

//-----------------------------------------------------------------------------

void
PlayerWindow::moveWindow(CONST RECT& rect)
{
    windowSize.cx = rect.right - rect.left + 1;
    windowSize.cy = rect.bottom - rect.top + 1;
    ::MoveWindow(atlWindow, rect.left, rect.top, windowSize.cx, windowSize.cy, TRUE);
}

//-----------------------------------------------------------------------------

SIZE
PlayerWindow::getSize() const
{
    return windowSize;
}

//-----------------------------------------------------------------------------

tstring
PlayerWindow::getBattleUrl() const
{
    assert(browser.get() != NULL);
    return browser->getUrl();
}

//-----------------------------------------------------------------------------

void
PlayerWindow::clickLeftPlayButton() const
{
    assert(getFlashHwnd() != NULL);
    const POINTS points = TO::GetLeftPlayButtonClickPoint(windowSize);
    const LPARAM lParam = (LPARAM)(MAKELPARAM(points.x, points.y));
    ::PostMessage(flashHwnd, WM_LBUTTONDOWN, 0, lParam);
    ::PostMessage(flashHwnd, WM_LBUTTONUP, 0, lParam);
}

//-----------------------------------------------------------------------------

void
PlayerWindow::clickRightPlayButton() const
{
    assert(getFlashHwnd() != NULL);
    const POINTS points = TO::GetRightPlayButtonClickPoint(windowSize);
    const LPARAM lParam = (LPARAM)(MAKELPARAM(points.x, points.y));
    ::PostMessage(flashHwnd, WM_LBUTTONDOWN, 0, lParam);
    ::PostMessage(flashHwnd, WM_LBUTTONUP, 0, lParam);
}

//-----------------------------------------------------------------------------

bool
PlayerWindow::isBattleHappens() const
{
    const DWORD color = GetPixelColor(getFlashHwnd(), windowSize.cx - 20, windowSize.cy - 11);
    return bool(color == 0xFFFFFF);
}

//-----------------------------------------------------------------------------

void
PlayerWindow::releaseAll()
{
    ::KillTimer(atlWindow, getInitialTimerId());
    browser.reset();
    if (atlWindow != NULL)
    {
        ::DestroyWindow(atlWindow);
        atlWindow = NULL;
    }
}

//-----------------------------------------------------------------------------

UINT
PlayerWindow::getInitialTimerId() const
{
    return (UINT)(this);
}

//-----------------------------------------------------------------------------

void CALLBACK
PlayerWindow::initialTimerProc(HWND /*hwnd*/, UINT /*message*/, UINT thisPointer, DWORD /*time*/)
{
    PlayerWindow* object = reinterpret_cast<PlayerWindow*>(thisPointer);
    HWND hwnd = ::FindWindowEx(FlashPlayerObject->atlWindow, NULL, NULL, NULL);
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
                            object->remainTimerIterations = 0;
                            ::ShowWindow(hwnd, SW_HIDE);
                        }
                        else
                        {
                            object->flashHwnd = hwnd;
                            object->currentChatWriter = CreateChatWriterForWindow(hwnd);
                            GlobalChatWriter = object->currentChatWriter;
                        }
                    }
                }
                while (hwnd != NULL);
                if (object->remainTimerIterations <= 0)
                {
                    ::KillTimer(FlashPlayerObject->atlWindow, thisPointer);
                }
            }
        }
    }
    --object->remainTimerIterations;
}

//-----------------------------------------------------------------------------

void
PlayerWindow::sendMessage(UINT message, WPARAM wParam, LPARAM lParam)
{
    if (getFlashHwnd() != NULL)
    {
        ::SendMessage(getFlashHwnd(), message, wParam, lParam);
    }
}

//-----------------------------------------------------------------------------

void
PlayerWindow::postMessage(UINT message, WPARAM wParam, LPARAM lParam)
{
    if (getFlashHwnd() != NULL)
    {
        ::PostMessage(getFlashHwnd(), message, wParam, lParam);
    }
}

//-----------------------------------------------------------------------------

void
PlayerWindow::activateWindow(CONST HWND activateWindow) const
{
    if (getFlashHwnd() != NULL)
    {
        ::SetForegroundWindow(getFlashHwnd());
        ::SetActiveWindow(getFlashHwnd());
        ::PostMessage(getFlashHwnd(), WM_ACTIVATE, (WPARAM)WA_CLICKACTIVE, (LPARAM)activateWindow);
    }
}

//-----------------------------------------------------------------------------

}// namespace Flash

//-----------------------------------------------------------------------------

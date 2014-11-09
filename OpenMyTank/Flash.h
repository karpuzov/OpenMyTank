//-----------------------------------------------------------------------------

#pragma once

//-----------------------------------------------------------------------------

#include "tstring.h"

#include <Windows.h>

#include <memory>

//-----------------------------------------------------------------------------

class WebBrowser;
class ChatWriter;

//-----------------------------------------------------------------------------

namespace Flash
{

//-----------------------------------------------------------------------------

class PlayerWindow
{
public:

    PlayerWindow(CONST HWND parentWindow, CONST TCHAR url[]);
    ~PlayerWindow();

    HWND getFlashHwnd() const;

    void moveWindow(CONST RECT& rect);

    SIZE getSize() const;

    tstring getBattleUrl() const;

    void clickLeftPlayButton() const;
    void clickRightPlayButton() const;

    bool isBattleHappens() const;

private:

    std::auto_ptr<WebBrowser> browser;

    HWND atlWindow;
    HWND flashHwnd;
    int remainTimerIterations;

    SIZE windowSize;

    ChatWriter* currentChatWriter;

    void releaseAll();

    UINT getInitialTimerId() const;
    static void CALLBACK initialTimerProc(HWND hwnd, UINT message, UINT thisPointer, DWORD time);

    void sendMessage(UINT message, WPARAM wParam, LPARAM lParam);
    void postMessage(UINT message, WPARAM wParam, LPARAM lParam);

    void activateWindow(CONST HWND activateWindow = NULL) const;
};

//-----------------------------------------------------------------------------

}// namespace Flash

//-----------------------------------------------------------------------------

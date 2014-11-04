
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

  HWND GetFlashHwnd() const;

  void MoveWindow(CONST RECT& rect);

  SIZE GetSize() const;

  tstring GetBattleUrl() const;

  void ClickLeftPlayButton() const;
  void ClickRightPlayButton() const;

  bool IsBattleHappens() const;

private:

  std::auto_ptr<WebBrowser>  Browser;

  HWND AtlWindow;
  HWND FlashHwnd;
  int RemainTimerIterations;

  SIZE WindowSize;

  ChatWriter* CurrentChatWriter;

  void ReleaseAll();

  UINT GetInitialTimerId() const;
  static void CALLBACK InitialTimerProc(HWND hwnd, UINT message, UINT thisPointer, DWORD time);

  void SendMessage(UINT message, WPARAM wParam, LPARAM lParam);
  void PostMessage(UINT message, WPARAM wParam, LPARAM lParam);

  void ActivateWindow(CONST HWND activateWindow = NULL) const;
};

//-----------------------------------------------------------------------------

} // namespace Flash

//-----------------------------------------------------------------------------

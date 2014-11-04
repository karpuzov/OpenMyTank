
//-----------------------------------------------------------------------------

#pragma once

//-----------------------------------------------------------------------------

#include "tstring.h"

#include <Windows.h>

#include <map>

//-----------------------------------------------------------------------------

class ChatWriter
{
public:

  ChatWriter(const HWND window);

  void AddPhrase(const SHORT virtualKey, const tstring& phrase, const bool team = false);

  bool DispatchKeyboardMessage(const WPARAM wParam, const LPARAM lParam) const;

private:

  const HWND Hwnd;

  struct ChatString
  {
    tstring String;
    bool    IsTeam;
  };
  typedef  std::map<SHORT, ChatString>  KeyToStringMap;

  KeyToStringMap  KeyToChatString;

  static const UINT LParamForDownT;
  static const UINT LParamForUpT;
  static const UINT LParamForDownEnter;
  static const UINT LParamForUpEnter;
};

//-----------------------------------------------------------------------------

ChatWriter* CreateChatWriterForWindow(const HWND window);

//-----------------------------------------------------------------------------

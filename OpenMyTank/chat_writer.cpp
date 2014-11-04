
//-----------------------------------------------------------------------------

#include "chat_writer.h"

#include "settings.h"

//-----------------------------------------------------------------------------

const UINT ChatWriter::LParamForDownT = (::MapVirtualKey('T', MAPVK_VK_TO_VSC)) << 16;
const UINT ChatWriter::LParamForUpT = (::MapVirtualKey('T', MAPVK_VK_TO_VSC) + KF_UP) << 16;
const UINT ChatWriter::LParamForDownEnter = (::MapVirtualKey(VK_RETURN, MAPVK_VK_TO_VSC)) << 16;
const UINT ChatWriter::LParamForUpEnter = (::MapVirtualKey(VK_RETURN, MAPVK_VK_TO_VSC) + KF_UP) << 16;

//-----------------------------------------------------------------------------

ChatWriter::ChatWriter(const HWND window)
  : Hwnd(window)
{
}

//-----------------------------------------------------------------------------

void
ChatWriter::AddPhrase(const SHORT virtualKey, const tstring& phrase, const bool team)
{
  ChatString chatString;
  chatString.String = tstring(phrase);
  chatString.IsTeam = team;

  KeyToChatString[virtualKey] = chatString;
}

//-----------------------------------------------------------------------------

bool
ChatWriter::DispatchKeyboardMessage(const WPARAM wParam, const LPARAM /*lParam*/) const
{
  const KeyToStringMap::const_iterator it = KeyToChatString.find(wParam);
  if (it != KeyToChatString.end())
  {
    const ChatString& chatString = it->second;

    if (chatString.IsTeam)
    {
      ::PostMessage(Hwnd, WM_KEYDOWN, 'T', LParamForDownT);
      ::PostMessage(Hwnd, WM_KEYUP,   'T', LParamForUpT);
    }
    else
    {
      ::PostMessage(Hwnd, WM_KEYDOWN, VK_RETURN, LParamForDownEnter);
      ::PostMessage(Hwnd, WM_KEYUP,   VK_RETURN, LParamForUpEnter);
    }

    const tstring::const_iterator endCharIt = chatString.String.end();
    for (tstring::const_iterator charIt = chatString.String.begin();
         charIt != endCharIt;
         ++charIt)
    {
      ::PostMessage(Hwnd, WM_CHAR, *charIt, 0);
    }

    ::PostMessage(Hwnd, WM_KEYDOWN, VK_RETURN, LParamForDownEnter);
    ::PostMessage(Hwnd, WM_KEYUP,   VK_RETURN, LParamForUpEnter);

    return true;
  }
  return false;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

ChatWriter*
CreateChatWriterForWindow(const HWND window)
{
  ChatWriter* chatWriter = new ChatWriter(window);
  const Settings::ChatMessagesVector& messages = GetSettings()->ChatMessages;
  for (Settings::ChatMessagesVector::const_iterator it = messages.begin();
       it != messages.end();
       ++it)
  {
    chatWriter->AddPhrase(it->Key, it->Text, it->Team);
  }
  return chatWriter;
}

//-----------------------------------------------------------------------------

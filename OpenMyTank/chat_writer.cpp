//-----------------------------------------------------------------------------

#include "chat_writer.h"

#include "settings.h"

//-----------------------------------------------------------------------------

const UINT ChatWriter::LParamForDownT = (::MapVirtualKey('T', MAPVK_VK_TO_VSC)) << 16;
const UINT ChatWriter::LParamForUpT = (::MapVirtualKey('T', MAPVK_VK_TO_VSC) + KF_UP) << 16;
const UINT ChatWriter::LParamForDownEnter = (::MapVirtualKey(VK_RETURN, MAPVK_VK_TO_VSC)) << 16;
const UINT ChatWriter::LParamForUpEnter = (::MapVirtualKey(VK_RETURN, MAPVK_VK_TO_VSC) + KF_UP)
        << 16;

//-----------------------------------------------------------------------------

ChatWriter::ChatWriter(const HWND window)
        : hwnd(window)
{
}

//-----------------------------------------------------------------------------

void
ChatWriter::addPhrase(const SHORT virtualKey, const tstring& phrase, const bool team)
{
    ChatString chatString;
    chatString.string = tstring(phrase);
    chatString.isTeam = team;

    keyToChatString[virtualKey] = chatString;
}

//-----------------------------------------------------------------------------

bool
ChatWriter::dispatchKeyboardMessage(const WPARAM wParam, const LPARAM /*lParam*/) const
{
    const KeyToStringMap::const_iterator it = keyToChatString.find(wParam);
    if (it != keyToChatString.end())
    {
        const ChatString& chatString = it->second;

        if (chatString.isTeam)
        {
            ::PostMessage(hwnd, WM_KEYDOWN, 'T', LParamForDownT);
            ::PostMessage(hwnd, WM_KEYUP, 'T', LParamForUpT);
        }
        else
        {
            ::PostMessage(hwnd, WM_KEYDOWN, VK_RETURN, LParamForDownEnter);
            ::PostMessage(hwnd, WM_KEYUP, VK_RETURN, LParamForUpEnter);
        }

        const tstring::const_iterator endCharIt = chatString.string.end();
        for (tstring::const_iterator charIt = chatString.string.begin(); charIt != endCharIt;
                ++charIt)
        {
            ::PostMessage(hwnd, WM_CHAR, *charIt, 0);
        }

        ::PostMessage(hwnd, WM_KEYDOWN, VK_RETURN, LParamForDownEnter);
        ::PostMessage(hwnd, WM_KEYUP, VK_RETURN, LParamForUpEnter);

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
    const Settings::ChatMessagesVector& messages = GetSettings()->chatMessages;
    for (Settings::ChatMessagesVector::const_iterator it = messages.begin(); it != messages.end();
            ++it)
    {
        chatWriter->addPhrase(it->key, it->text, it->team);
    }
    return chatWriter;
}

//-----------------------------------------------------------------------------

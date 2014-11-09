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

    void addPhrase(const SHORT virtualKey, const tstring& phrase, const bool team = false);

    bool dispatchKeyboardMessage(const WPARAM wParam, const LPARAM lParam) const;

private:

    const HWND hwnd;

    struct ChatString
    {
        tstring string;
        bool isTeam;
    };
    typedef std::map<SHORT,ChatString> KeyToStringMap;

    KeyToStringMap keyToChatString;

    static const UINT LParamForDownT;
    static const UINT LParamForUpT;
    static const UINT LParamForDownEnter;
    static const UINT LParamForUpEnter;
};

//-----------------------------------------------------------------------------

ChatWriter* CreateChatWriterForWindow(const HWND window);

//-----------------------------------------------------------------------------

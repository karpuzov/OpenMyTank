//-----------------------------------------------------------------------------

#pragma once

//-----------------------------------------------------------------------------

#include <windows.h>

//-----------------------------------------------------------------------------

class KeyboardHook
{
public:

    KeyboardHook();
    ~KeyboardHook();

private:

    static HHOOK hook;

    static LRESULT CALLBACK hookProcedure(int code, WPARAM wParam, LPARAM lParam);
};

//-----------------------------------------------------------------------------

class MouseHook
{
public:

    MouseHook();
    ~MouseHook();

private:

    static HHOOK hook;

    static LRESULT CALLBACK hookProcedure(int code, WPARAM wParam, LPARAM lParam);
};

//-----------------------------------------------------------------------------

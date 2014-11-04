
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

  static HHOOK Hook;

  static LRESULT CALLBACK HookProcedure(int code, WPARAM wParam, LPARAM lParam);
};

//-----------------------------------------------------------------------------

class MouseHook
{
public:

  MouseHook();
  ~MouseHook();

private:

  static HHOOK Hook;

  static LRESULT CALLBACK HookProcedure(int code, WPARAM wParam, LPARAM lParam);
};

//-----------------------------------------------------------------------------

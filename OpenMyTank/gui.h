
//-----------------------------------------------------------------------------

#pragma once

//-----------------------------------------------------------------------------

#include "options.h"
#include "tstring.h"
#include "settings.h"
#include "resource.h"

#include <windows.h>
#include <Shellapi.h>

//-----------------------------------------------------------------------------

RECT MakeRECT(const int x, const int y, const int right, const int bottom);
POINT MakePOINT(const int x, const int y);
SIZE MakeSIZE(const int width, const int height);

RECT MakeRECT(const POINT& point, const SIZE& size);

//-----------------------------------------------------------------------------

namespace Gui
{

//-----------------------------------------------------------------------------

int GetMenuHeight();

DWORD GetMainWindowGwlStyle();

const TCHAR*const GetWindowClassName();

//-----------------------------------------------------------------------------

class Window
{
public:

  Window(const HWND parent, const RECT& rect);

  virtual ~Window();

  HWND GetHwnd() const;

  POINT GetPoint() const;
  SIZE GetSize() const;

  void SetRect(const RECT& rect);
  void Resize(const SIZE& newSize);
  void Move(const POINT& point);

protected:

  const HWND ParentHwnd;
  HWND Hwnd;

  SIZE Size;
  POINT Position;

  void Repaint();
  virtual void Draw(HDC hdc) const;

private:

  void UpdatePosition();
};

//-----------------------------------------------------------------------------

class Label : public Window
{
public:

  Label(const HWND parent,
        const RECT& rect,
        const TCHAR text[]);

  virtual ~Label();

  void SetTextColor(const COLORREF color);
  void SetBackgroundColor(const COLORREF color);

  void SetFont(const LOGFONT& logFont);

protected:

  virtual void Draw(HDC hdc) const;

protected: // usefull for MultilineLabel

  static LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

  static TCHAR* GetClassName()
  {
    return TEXT("OpenMyTank#Label");
  }

  static int RefCount;
  static HFONT DefaultFont;

  const tstring Caption;

  COLORREF TextColor;
  COLORREF BackgroundColor;
  HFONT Font;
};

//-----------------------------------------------------------------------------

class MultilineLabel : public Label
{
public:

  MultilineLabel(const HWND parent,
                 const RECT& rect,
                 const TCHAR text[]);

protected:

  virtual void Draw(HDC hdc) const;
};

//-----------------------------------------------------------------------------

class Button : public Window
{
public:

  Button(const HWND parent,
         const RECT& rect,
         const TCHAR text[],
         const DWORD id,
         bool checkboxLike = false);

  virtual ~Button();

  DWORD GetId() const;

  void SetEnabled(const bool enabled = true);

  bool GetChecked() const;
  void SetChecked(const bool checked = true);

  void Click();

protected:

  bool IsEnabled;
  bool IsInside;
  bool IsPressed;
  bool IsChecked;

  virtual void Draw(HDC hdc) const;

private:

  static LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

  static TCHAR* GetClassName()
  {
    return TEXT("OpenMyTank#Button");
  }

  static int RefCount;

  static HFONT Font;

  const tstring Caption;
  const DWORD Id;
  bool IsCheckbox;

  SIZE GetMinSize() const;
};

//-----------------------------------------------------------------------------

class WindowTitle
{
public:
  WindowTitle(CONST TCHAR* loginName)
  {
    LoginPart[0] = '\0';
    if (loginName[0] != '\0')
    {
      ::wsprintf(LoginPart, TEXT("[%s] "), loginName);
    }
    SetActiveServer();
  }

  CONST TCHAR* Get()
  {
    return Title;
  }

  CONST TCHAR* SetActiveServer(const int serverNumber = -1)
  {
    if (serverNumber == -1)
    {
      ::wsprintf(Title, TEXT("%s%s %s"), LoginPart, ProgramTitle, C2T(VersionNumber).c_str());
    }
    else
    {
      ::wsprintf(Title, TEXT("%s[%d] %s %s"), LoginPart, serverNumber, ProgramTitle, C2T(VersionNumber).c_str());
    }
    return Title;
  }

private:
  TCHAR LoginPart[32];
  TCHAR Title[512];
};

//-----------------------------------------------------------------------------

class AboutWindow
{
public:

  AboutWindow(CONST HWND parent);
  ~AboutWindow();

  void UpdateSize();

  static LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

private:

  CONST TCHAR* ClassName;
  HWND ParentWindow;

  HWND Window;

  Label*  ProgramNameStatic;
  Label*  VersionStatic;
  MultilineLabel*  NewsStatic;
  Button* SettingsButton;
  Button* HelpPageButton;
  Button* HomePageButton;
  Button* VkGroupPageButton;
  Button* NewCopyrightButton;
  Button* OldCopyrightButton;

  enum {Width = 1000, Height = 600};

  enum
  {
    SettingsButtonId,
    HelpPageButtonId,
    HomePageButtonId,
    VkGroupPageButtonId,
    NewCopyrightButtonId,
    OldCopyrightButtonId
  };
};

//-----------------------------------------------------------------------------

class ClickerWindow
{
public:

  ClickerWindow(const HWND parent, const RECT rect, Button* button);
  ~ClickerWindow();

  HWND GetHwnd() const;

  static LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

private:

  static int RefCount;

  const TCHAR* ClassName;
  Button* BuddyButton;

  HWND Window;

  Button* CancelButton;

  enum { CancelButtonId = 1 };
  enum { Margin = 5 };
};

//-----------------------------------------------------------------------------

} // namespace Gui

//-----------------------------------------------------------------------------

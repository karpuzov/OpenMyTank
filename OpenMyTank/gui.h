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

const TCHAR* const GetWindowClassName();

//-----------------------------------------------------------------------------

class Window
{
public:

    Window(const HWND parent, const RECT& rect);

    virtual ~Window();

    HWND getHwnd() const;

    POINT getPoint() const;
    SIZE getSize() const;

    void setRect(const RECT& rect);
    void resize(const SIZE& newSize);
    void move(const POINT& point);

protected:

    const HWND parentHwnd;
    HWND hwnd;

    SIZE size;
    POINT position;

    void repaint();
    virtual void draw(HDC hdc) const;

private:

    void updatePosition();
};

//-----------------------------------------------------------------------------

class Label
    : public Window
{
public:

    Label(const HWND parent, const RECT& rect, const TCHAR text[]);

    virtual ~Label();

    void setTextColor(const COLORREF color);
    void setBackgroundColor(const COLORREF color);

    void setFont(const LOGFONT& logFont);

protected:

    virtual void draw(HDC hdc) const;

protected:
    // usefull for MultilineLabel

    static LRESULT CALLBACK windowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

    static TCHAR* getClassName()
    {
        return TEXT("OpenMyTank#Label");
    }

    static int refCount;
    static HFONT defaultFont;

    const tstring caption;

    COLORREF textColor;
    COLORREF backgroundColor;
    HFONT font;
};

//-----------------------------------------------------------------------------

class MultilineLabel
    : public Label
{
public:

    MultilineLabel(const HWND parent, const RECT& rect, const TCHAR text[]);

protected:

    virtual void draw(HDC hdc) const;
};

//-----------------------------------------------------------------------------

class Button
    : public Window
{
public:

    Button(const HWND parent, const RECT& rect, const TCHAR text[], const DWORD id, bool checkboxLike = false);

    virtual ~Button();

    DWORD getId() const;

    void setEnabled(const bool enabled = true);

    bool getChecked() const;
    void setChecked(const bool checked = true);

    void click();

protected:

    bool isEnabled;
    bool isInside;
    bool isPressed;
    bool isChecked;

    virtual void draw(HDC hdc) const;

private:

    static LRESULT CALLBACK windowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

    static TCHAR* getClassName()
    {
        return TEXT("OpenMyTank#Button");
    }

    static int refCount;

    static HFONT font;

    const tstring caption;
    const DWORD id;
    bool isCheckbox;

    SIZE getMinSize() const;
};

//-----------------------------------------------------------------------------

class WindowTitle
{
public:
    WindowTitle(CONST TCHAR* loginName)
    {
        loginPart[0] = '\0';
        if (loginName[0] != '\0')
        {
            ::wsprintf(loginPart, TEXT("[%s] "), loginName);
        }
        setActiveServer();
    }

    CONST TCHAR* get()
    {
        return title;
    }

    CONST TCHAR* setActiveServer(const int serverNumber = -1)
    {
        if (serverNumber == -1)
        {
            ::wsprintf(title, TEXT("%s%s %s"), loginPart, ProgramTitle, C2T(VersionNumber).c_str());
        }
        else
        {
            ::wsprintf(title, TEXT("%s[%d] %s %s"), loginPart, serverNumber, ProgramTitle,
                    C2T(VersionNumber).c_str());
        }
        return title;
    }

private:
    TCHAR loginPart[32];
    TCHAR title[512];
};

//-----------------------------------------------------------------------------

class AboutWindow
{
public:

    AboutWindow(CONST HWND parent);
    ~AboutWindow();

    void updateSize();

    static LRESULT CALLBACK windowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

private:

    CONST TCHAR* className;
    HWND parentWindow;

    HWND window;

    Label* programNameStatic;
    Label* versionStatic;
    MultilineLabel* newsStatic;
    Button* settingsButton;
    Button* helpPageButton;
    Button* homePageButton;
    Button* vkGroupPageButton;
    Button* newCopyrightButton;
    Button* oldCopyrightButton;

    enum
    {
        Width = 1000, Height = 600
    };

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

    HWND getHwnd() const;

    static LRESULT CALLBACK windowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

private:

    static int refCount;

    const TCHAR* className;
    Button* buddyButton;

    HWND window;

    Button* cancelButton;

    enum
    {
        CancelButtonId = 1
    };
    enum
    {
        Margin = 5
    };
};

//-----------------------------------------------------------------------------

}// namespace Gui

//-----------------------------------------------------------------------------

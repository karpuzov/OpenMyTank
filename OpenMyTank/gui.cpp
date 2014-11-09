//-----------------------------------------------------------------------------

#include "gui.h"

#include <sstream>
#include <cassert>

//-----------------------------------------------------------------------------

extern HINSTANCE Instance;

//-----------------------------------------------------------------------------

RECT MakeRECT(const int x, const int y, const int right, const int bottom)
{
    RECT rect =
    { x, y, right, bottom };
    return rect;
}

POINT MakePOINT(const int x, const int y)
{
    POINT point =
    { x, y };
    return point;
}

SIZE MakeSIZE(const int width, const int height)
{
    SIZE size =
    { width, height };
    return size;
}

RECT MakeRECT(const POINT& point, const SIZE& size)
{
    RECT rect =
    { point.x, point.y, point.x + size.cx - 1, point.y + size.cy - 1 };
    return rect;
}

//-----------------------------------------------------------------------------

namespace Gui
{

//-----------------------------------------------------------------------------

int GetMenuHeight()
{
    return 30;
}

//-----------------------------------------------------------------------------

DWORD GetMainWindowGwlStyle()
{
    return (WS_OVERLAPPEDWINDOW | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS);
}

//-----------------------------------------------------------------------------

const TCHAR* const GetWindowClassName()
{
    static TCHAR MainWindowClass[] = TEXT("OpenMyTank");
    return MainWindowClass;
}

//-----------------------------------------------------------------------------
//  DoubleHdc
//-----------------------------------------------------------------------------

class DoubleHdc
{
public:

    DoubleHdc(HDC* hdc, const int width, const int height)
            : hdcPtr(hdc), ScreenHdc(*hdc), Width(width), Height(height)
    {
        MemoryHdc = ::CreateCompatibleDC(ScreenHdc);
        Bitmap = ::CreateCompatibleBitmap(ScreenHdc, Width, Height);
        OldBitmap = ::SelectObject(MemoryHdc, Bitmap);
        *hdcPtr = MemoryHdc;
    }

    ~DoubleHdc()
    {
        ::BitBlt(ScreenHdc, 0, 0, Width, Height, MemoryHdc, 0, 0, SRCCOPY);
        ::SelectObject(MemoryHdc, OldBitmap);
        ::DeleteObject(Bitmap);
        ::DeleteDC(MemoryHdc);
        *hdcPtr = ScreenHdc;
    }

private:

    HDC* const hdcPtr;
    const HDC ScreenHdc;
    const int Width;
    const int Height;

    HDC MemoryHdc;
    HBITMAP Bitmap;
    HANDLE OldBitmap;
};

//-----------------------------------------------------------------------------
//    Window
//-----------------------------------------------------------------------------

Window::Window(const HWND parent, const RECT& rect)
        : parentHwnd(parent)
{
    position.x = rect.left;
    position.y = rect.top;
    size.cx = rect.right - rect.left + 1;
    size.cy = rect.bottom - rect.top + 1;
}

//-----------------------------------------------------------------------------

Window::~Window()
{
}

//-----------------------------------------------------------------------------

HWND
Window::getHwnd() const
{
    return hwnd;
}

//-----------------------------------------------------------------------------

POINT
Window::getPoint() const
{
    return position;
}

//-----------------------------------------------------------------------------

SIZE
Window::getSize() const
{
    return size;
}

//-----------------------------------------------------------------------------

void
Window::setRect(const RECT& rect)
{
    position.x = rect.left;
    position.y = rect.top;
    size.cx = rect.right - rect.left + 1;
    size.cy = rect.bottom - rect.top + 1;
    updatePosition();
}

//-----------------------------------------------------------------------------

void
Window::resize(const SIZE& newSize)
{
    size = newSize;
    updatePosition();
}

//-----------------------------------------------------------------------------

void
Window::move(const POINT& point)
{
    position = point;
    updatePosition();
}

//-----------------------------------------------------------------------------

void
Window::repaint()
{
    ::InvalidateRect(hwnd, NULL, FALSE);
    ::UpdateWindow(hwnd);
}

//-----------------------------------------------------------------------------

void
Window::draw(HDC hdc) const
{
}

//-----------------------------------------------------------------------------

void
Window::updatePosition()
{
    ::MoveWindow(hwnd, position.x, position.y, size.cx, size.cy, TRUE);
}

//-----------------------------------------------------------------------------
//    Label
//-----------------------------------------------------------------------------

int Label::refCount = 0;
HFONT Label::defaultFont = NULL;

//-----------------------------------------------------------------------------

Label::Label(const HWND parent, const RECT& rect, const TCHAR text[])
        : Window(parent, rect), caption(text)
{
    if (refCount++ == 0)
    {
        WNDCLASSEX wcex;
        wcex.cbSize = sizeof(WNDCLASSEX);

        wcex.style = 0;
        wcex.lpfnWndProc = Label::windowProcedure;
        wcex.cbClsExtra = 0;
        wcex.cbWndExtra = 0;
        wcex.hInstance = Instance;
        wcex.hIcon = NULL;
        wcex.hCursor = ::LoadCursor(0, IDC_ARROW);
        wcex.hbrBackground = (HBRUSH)NULL;
        wcex.lpszMenuName = NULL;
        wcex.lpszClassName = getClassName();
        wcex.hIconSm = NULL;

        ::RegisterClassEx(&wcex);

        LOGFONT logFont =
        { 0 };
        defaultFont = ::CreateFontIndirect(&logFont);
    }

    font = defaultFont;

    hwnd = ::CreateWindow(getClassName(),
            text,
            WS_VISIBLE | WS_CHILD,
            position.x, position.y,
            size.cx, size.cy,
            parentHwnd,
            NULL,
            Instance,
            NULL);

    ::SetWindowLong(hwnd, GWLP_USERDATA, LONG(this));
}

//-----------------------------------------------------------------------------

Label::~Label()
{
    if (font != defaultFont)
    {
        ::DeleteObject(font);
    }
    ::DestroyWindow(hwnd);

    if (--refCount == 0)
    {
        ::DeleteObject(defaultFont);
        ::UnregisterClass(getClassName(), Instance);
    }
}

//-----------------------------------------------------------------------------

void
Label::setTextColor(const COLORREF color)
{
    textColor = color;
}

//-----------------------------------------------------------------------------

void
Label::setBackgroundColor(const COLORREF color)
{
    backgroundColor = color;
}

//-----------------------------------------------------------------------------

void
Label::setFont(const LOGFONT& logFont)
{
    if (font != defaultFont)
    {
        ::DeleteObject(font);
    }
    font = ::CreateFontIndirect(&logFont);
}

//-----------------------------------------------------------------------------

void
Label::draw(HDC hdc) const
{
    RECT rc;
    ::GetClientRect(hwnd, &rc);

    DoubleHdc buffer(&hdc, rc.right, rc.bottom);

    {
        const HBRUSH brush = ::CreateSolidBrush(backgroundColor);
        ::FillRect(hdc, &rc, brush);
        ::DeleteObject(brush);
    }
    {
        const HFONT oldFont = (HFONT)::SelectObject(hdc, font);
        {
            ::SetTextColor(hdc, textColor);
            ::SetBkColor(hdc, backgroundColor);
            ::DrawText(hdc, caption.c_str(), caption.length(), &rc,
                    DT_SINGLELINE | DT_NOCLIP | DT_CENTER | DT_VCENTER);
        }
        ::SelectObject(hdc, oldFont);
    }
}

//-----------------------------------------------------------------------------

LRESULT CALLBACK
Label::windowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
        case WM_ERASEBKGND:
        {
            return 1;
        }
        case WM_PAINT:
        {
            Label* const window = reinterpret_cast<Label*>(::GetWindowLong(hwnd, GWLP_USERDATA));
            PAINTSTRUCT ps;
            const HDC hdc = ::BeginPaint(hwnd, &ps);
            window->draw(hdc);
            ::EndPaint(hwnd, &ps);
            return 0;
        }
    }
    return ::DefWindowProc(hwnd, message, wParam, lParam);
}

//-----------------------------------------------------------------------------
//    MultilineLabel
//-----------------------------------------------------------------------------

MultilineLabel::MultilineLabel(const HWND parent, const RECT& rect, const TCHAR text[])
        : Label(parent, rect, text)
{
}

//-----------------------------------------------------------------------------

void
MultilineLabel::draw(HDC hdc) const
{
    RECT rc;
    ::GetClientRect(hwnd, &rc);

    DoubleHdc buffer(&hdc, rc.right, rc.bottom);

    {
        const HBRUSH brush = ::CreateSolidBrush(backgroundColor);
        ::FillRect(hdc, &rc, brush);
        ::DeleteObject(brush);
    }
    {
        const HFONT oldFont = (HFONT)::SelectObject(hdc, font);
        {
            ::SetTextColor(hdc, textColor);
            ::SetBkColor(hdc, backgroundColor);
            ::DrawText(hdc, caption.c_str(), caption.length(), &rc,
                    DT_WORDBREAK | DT_CENTER | DT_VCENTER);
        }
        ::SelectObject(hdc, oldFont);
    }
}

//-----------------------------------------------------------------------------
//    Button
//-----------------------------------------------------------------------------

int Gui::Button::refCount = 0;
HFONT Gui::Button::font = NULL;

//-----------------------------------------------------------------------------

Button::Button(
    const HWND parent, const RECT& rect, const TCHAR text[], const DWORD id, bool checkboxLike)
        : Window(parent, rect), caption(text), id(id), isCheckbox(checkboxLike), isEnabled(true),
            isInside(false), isPressed(false), isChecked(false)
{
    if (refCount++ == 0)
    {
        WNDCLASSEX wcex;
        wcex.cbSize = sizeof(WNDCLASSEX);

        wcex.style = 0;
        wcex.lpfnWndProc = Button::windowProcedure;
        wcex.cbClsExtra = 0;
        wcex.cbWndExtra = 0;
        wcex.hInstance = Instance;
        wcex.hIcon = NULL;
        wcex.hCursor = ::LoadCursor(0, IDC_ARROW);
        wcex.hbrBackground = (HBRUSH)NULL;
        wcex.lpszMenuName = NULL;
        wcex.lpszClassName = getClassName();
        wcex.hIconSm = NULL;

        ::RegisterClassEx(&wcex);

        LOGFONT logFont =
        { 0 };
        logFont.lfCharSet = RUSSIAN_CHARSET;
        logFont.lfHeight = 15;
        logFont.lfWeight = FW_NORMAL;
        ::lstrcpy(logFont.lfFaceName, TEXT("Arial"));
        assert(font == NULL);
        font = ::CreateFontIndirect(&logFont);
    }

    hwnd = ::CreateWindow(getClassName(),
            text,
            WS_VISIBLE | WS_CHILD,
            position.x, position.y,
            size.cx, size.cy,
            parentHwnd,
            (HMENU)(id),
            Instance,
            NULL);

    ::SetWindowLong(hwnd, GWLP_USERDATA, LONG(this));

    if (size.cx == 0)
    {
        resize(MakeSIZE(getMinSize().cx, getSize().cy));
    }
}

//-----------------------------------------------------------------------------

Button::~Button()
{
    ::DestroyWindow(hwnd);

    if (--refCount == 0)
    {
        assert(font != NULL);
        ::DeleteObject(font);
        font = NULL;

        ::UnregisterClass(getClassName(), Instance);
    }
}

//-----------------------------------------------------------------------------

DWORD
Button::getId() const
{
    return id;
}

//-----------------------------------------------------------------------------

void
Button::setEnabled(const bool enabled)
{
    isEnabled = enabled;
    repaint();
}

//-----------------------------------------------------------------------------

bool
Button::getChecked() const
{
    return isChecked;
}

//-----------------------------------------------------------------------------

void
Button::setChecked(const bool checked)
{
    isChecked = checked;
    repaint();
}

//-----------------------------------------------------------------------------

void
Button::click()
{
    if (isEnabled)
    {
        setChecked(!isChecked);
        ::PostMessage(parentHwnd, WM_COMMAND, WPARAM(id), LPARAM(this));
    }
}

//-----------------------------------------------------------------------------

void
Button::draw(HDC hdc) const
{
    RECT rc;
    ::GetClientRect(hwnd, &rc);

    DoubleHdc buffer(&hdc, rc.right, rc.bottom);

    COLORREF bgColor, textColor;
    if (!isEnabled)
    {
        bgColor = 0x575757;
        textColor = 0xb1b1b1;
    }
    else
    {
        if (isPressed && isInside)
        {
            bgColor = (isChecked) ? (0x2ce74f) : (0x185b1f);
        }
        else
        {
            bgColor = (isChecked) ? (0x31FF58) : (0x004A08);
        }
        textColor = (isChecked) ? (0x0E280E) : (0x30FB58);
    }

    // Background with border:
    const HBRUSH brush = ::CreateSolidBrush(bgColor);
    if (isEnabled && (isInside || isPressed))
    {
        const HPEN pen = ::CreatePen(PS_SOLID, 1, RGB(255, 255, 255));
        const HPEN oldPen = (HPEN)::SelectObject(hdc, pen);
        const HBRUSH oldBrush = (HBRUSH)::SelectObject(hdc, brush);
        ::Rectangle(hdc, 0, 0, rc.right, rc.bottom);
        ::SelectObject(hdc, oldBrush);
        ::SelectObject(hdc, oldPen);
        ::DeleteObject(pen);
    }
    else
    {
        ::FillRect(hdc, &rc, brush);
    }
    ::DeleteObject(brush);

    {
        const HFONT oldFont = (HFONT)::SelectObject(hdc, font);
        ::SetTextColor(hdc, textColor);
        ::SetBkColor(hdc, bgColor);
        ::DrawText(hdc, caption.c_str(), caption.length(), &rc,
                DT_SINGLELINE | DT_NOCLIP | DT_CENTER | DT_VCENTER);
        ::SelectObject(hdc, oldFont);
    }
}

//-----------------------------------------------------------------------------

LRESULT CALLBACK
Button::windowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
        case WM_ERASEBKGND:
        {
            return 1;
        }
        case WM_PAINT:
        {
            Button* const window = reinterpret_cast<Button*>(::GetWindowLong(hwnd, GWLP_USERDATA));
            PAINTSTRUCT ps;
            const HDC hdc = ::BeginPaint(hwnd, &ps);
            window->draw(hdc);
            ::EndPaint(hwnd, &ps);
            return 0;
        }
        case WM_MOUSEMOVE:
        {
            Button* const window = reinterpret_cast<Button*>(::GetWindowLong(hwnd, GWLP_USERDATA));
            if (window->isEnabled)
            {
                if (window->isPressed)
                {
                    RECT rc;
                    ::GetWindowRect(hwnd, &rc);
                    POINT pt =
                    { LOWORD(lParam), HIWORD(lParam) };
                    ::ClientToScreen(hwnd, &pt);
                    const bool newInside = bool(::PtInRect(&rc, pt) == TRUE);
                    if (window->isInside != newInside)
                    {
                        window->isInside = newInside;
                        window->repaint();
                    }
                }
                else
                {
                    if (!window->isInside)
                    {
                        window->isInside = true;

                        TRACKMOUSEEVENT m;
                        m.cbSize = sizeof(TRACKMOUSEEVENT);
                        m.hwndTrack = hwnd;
                        m.dwFlags = TME_LEAVE;
                        m.dwHoverTime = HOVER_DEFAULT;
                        ::TrackMouseEvent(&m);

                        window->repaint();
                    }
                }
            }
            return 0;
        }
        case WM_MOUSELEAVE:
        {
            Button* const window = reinterpret_cast<Button*>(::GetWindowLong(hwnd, GWLP_USERDATA));
            window->isInside = false;
            window->repaint();
            return 0;
        }
        case WM_LBUTTONDOWN:
        {
            Button* const window = reinterpret_cast<Button*>(::GetWindowLong(hwnd, GWLP_USERDATA));
            if (window->isEnabled)
            {
                window->isPressed = true;
                ::SetCapture(hwnd);
                window->repaint();
            }
            return 0;
        }
        case WM_LBUTTONUP:
        {
            Button* const window = reinterpret_cast<Button*>(::GetWindowLong(hwnd, GWLP_USERDATA));
            if (window->isEnabled)
            {
                if (window->isPressed && window->isInside)
                {
                    if (window->isCheckbox)
                    {
                        window->isChecked = !window->isChecked;
                    }
                    ::PostMessage(window->parentHwnd, WM_COMMAND, WPARAM(window->id), LPARAM(window));
                }
                window->isPressed = false;
                ::ReleaseCapture();
                window->repaint();
            }
            return 0;
        }
        case WM_CAPTURECHANGED:
        {
            Button* const window = reinterpret_cast<Button*>(::GetWindowLong(hwnd, GWLP_USERDATA));
            if (window->isPressed)
            {
                ::SetCapture(hwnd);
            }
            return 0;
        }
    }
    return ::DefWindowProc(hwnd, message, wParam, lParam);
}

//-----------------------------------------------------------------------------

SIZE
Button::getMinSize() const
{
    SIZE size;

    const HDC hdc = ::GetWindowDC(hwnd);
    LOGFONT logFont = { 0 };
    logFont.lfCharSet = RUSSIAN_CHARSET;
    logFont.lfHeight = 15;
    logFont.lfWeight = FW_NORMAL;
    ::lstrcpy(logFont.lfFaceName, TEXT("Arial"));
    const HFONT font = ::CreateFontIndirect(&logFont);
    const HFONT oldFont = (HFONT)::SelectObject(hdc, font);
    const tstring newText(TEXT(" ") + caption + TEXT(" "));
    ::GetTextExtentPoint(hdc, newText.c_str(), newText.length(), &size);
    ::SelectObject(hdc, oldFont);
    ::DeleteObject(font);
    ::ReleaseDC(hwnd, hdc);

    return size;
}

//-----------------------------------------------------------------------------
//    AboutWindow
//-----------------------------------------------------------------------------

AboutWindow::AboutWindow(CONST HWND parent)
    : className(TEXT("OpenMyTank#About")), parentWindow(parent), programNameStatic(NULL),
        versionStatic(NULL), newsStatic(NULL), settingsButton(NULL), helpPageButton(NULL),
        homePageButton(NULL), vkGroupPageButton(NULL), newCopyrightButton(NULL),
        oldCopyrightButton(NULL)
{
    WNDCLASSEX wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = 0;
    wcex.lpfnWndProc = AboutWindow::windowProcedure;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = Instance;
    wcex.hIcon = NULL;
    wcex.hCursor = ::LoadCursor(0, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)::CreateSolidBrush(0x0D220C);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = className;
    wcex.hIconSm = NULL;

    if (::RegisterClassEx(&wcex) != 0)
    {
        window = ::CreateWindow(className, TEXT(""),
                WS_VISIBLE | WS_CHILD | WS_CLIPCHILDREN,
                0, 0, 0, 0, parent, NULL, Instance, NULL);

        LOGFONT logFont =
        { 0 };
        logFont.lfWeight = FW_NORMAL;
        logFont.lfHeight = 40;

        TCHAR caption[256];

        programNameStatic = new Label(window, MakeRECT(MakePOINT(0, 50), MakeSIZE(Width, 40)),
                ProgramTitle);
        programNameStatic->setTextColor(0x00FF00);
        programNameStatic->setBackgroundColor(0x0D220C);
        programNameStatic->setFont(logFont);

        const tstring versionFormat(GetString(IDS_VERSION));
        ::wsprintf(caption, versionFormat.c_str(), C2T(VersionNumber).c_str(), C2T(VersionDate).c_str());
        versionStatic
			= new Label(window, MakeRECT(MakePOINT(0, 50 + 40 + 20), MakeSIZE(Width, 20)), caption);
        versionStatic->setTextColor(0x00AA00);
        versionStatic->setBackgroundColor(0x0D220C);
        logFont.lfHeight = 20;
        versionStatic->setFont(logFont);

        newsStatic = new MultilineLabel(window,
                MakeRECT(MakePOINT(100, 50 + 40 + 20 + 65), MakeSIZE(Width - 100 - 100, 75)),
                GetString(IDS_NEWS).c_str());
        newsStatic->setTextColor(0x00DDDD);
        newsStatic->setBackgroundColor(0x0D220C);
        logFont.lfHeight = 18;
        newsStatic->setFont(logFont);

        settingsButton = new Button(window,
                MakeRECT(MakePOINT(350, 260), MakeSIZE(Width - 350 * 2, 40)),
                GetString(IDS_SETTINGS_BUTTON).c_str(), SettingsButtonId);

        helpPageButton = new Button(window,
                MakeRECT(MakePOINT(300, Height - 260), MakeSIZE(Width - 300 * 2, 30)),
                GetString(IDS_HELP_PAGE_BUTTON).c_str(), HelpPageButtonId);

        homePageButton = new Button(window,
                MakeRECT(MakePOINT(345, Height - 190), MakeSIZE(Width - 345 * 2, 30)),
                ProgramHomePage, HomePageButtonId);

        const tstring vkGroupString(TEXT("OpenMyTank in VK.com"));
        vkGroupPageButton = new Button(window,
                MakeRECT(MakePOINT(345, Height - 150), MakeSIZE(Width - 345 * 2, 30)),
                vkGroupString.c_str(), VkGroupPageButtonId);

        const tstring newCopyrightString(
                TEXT("Copyright (C) 2014 OpenMyTank developers on GitHub"));
        newCopyrightButton = new Button(window,
                MakeRECT(MakePOINT(345, Height - 80), MakeSIZE(Width - 345 * 2, 25)),
                newCopyrightString.c_str(), NewCopyrightButtonId);

        const tstring copyrightString(TEXT("Copyright (C) 2012-2014 Karpuzov Valery"));
        oldCopyrightButton = new Button(window,
                MakeRECT(MakePOINT(380, Height - 50), MakeSIZE(Width - 380 * 2, 20)),
                copyrightString.c_str(), OldCopyrightButtonId);

        ::ShowWindow(window, SW_SHOW);
        ::UpdateWindow(window);
    }
}

//-----------------------------------------------------------------------------

AboutWindow::~AboutWindow()
{
    delete programNameStatic;
    delete versionStatic;
    delete newsStatic;
    delete settingsButton;
    delete helpPageButton;
    delete homePageButton;
    delete vkGroupPageButton;
    delete newCopyrightButton;
    delete oldCopyrightButton;
    ::DestroyWindow(window);
    ::UnregisterClass(className, Instance);
}

//-----------------------------------------------------------------------------

void
AboutWindow::updateSize()
{
    RECT rect;
    ::GetClientRect(parentWindow, &rect);
    ::MoveWindow(window, rect.left + (rect.right - 1000) / 2,
            1 + GetMenuHeight() + (rect.bottom - 630) / 2, Width, Height, TRUE);
}

//-----------------------------------------------------------------------------

LRESULT CALLBACK
AboutWindow::windowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    if (message == WM_COMMAND)
    {
        const WORD id = LOWORD(wParam);
        switch (id)
        {
            case SettingsButtonId:
                ChangeSettingsByUser();
                break;
            case HelpPageButtonId:
                ::ShellExecute(NULL, TEXT("open"), HelpPage, NULL, NULL, SW_SHOWNORMAL);
                break;
            case HomePageButtonId:
                ::ShellExecute(NULL, TEXT("open"), ProgramHomePage, NULL, NULL, SW_SHOWNORMAL);
                break;
            case VkGroupPageButtonId:
                ::ShellExecute(NULL, TEXT("open"), VkGroupPage, NULL, NULL, SW_SHOWNORMAL);
                break;
            case NewCopyrightButtonId:
                ::ShellExecute(NULL, TEXT("open"), NewCopyrightPage, NULL, NULL, SW_SHOWNORMAL);
                break;
            case OldCopyrightButtonId:
                ::ShellExecute(NULL, TEXT("open"), OldCopyrightPage, NULL, NULL, SW_SHOWNORMAL);
                break;
        }
    }
    return ::DefWindowProc(hwnd, message, wParam, lParam);
}

//-----------------------------------------------------------------------------
//    ClickerWindow
//-----------------------------------------------------------------------------

int Gui::ClickerWindow::refCount = 0;

//-----------------------------------------------------------------------------

ClickerWindow::ClickerWindow(const HWND parent, const RECT rect, Button* button)
    : className(TEXT("OpenMyTank#Clicker")), buddyButton(button), cancelButton(NULL)
{
    if (refCount++ == 0)
    {
        WNDCLASSEX wcex;
        wcex.cbSize = sizeof(WNDCLASSEX);
        wcex.style = 0;
        wcex.lpfnWndProc = ClickerWindow::windowProcedure;
        wcex.cbClsExtra = 0;
        wcex.cbWndExtra = 0;
        wcex.hInstance = Instance;
        wcex.hIcon = NULL;
        wcex.hCursor = ::LoadCursor(0, IDC_ARROW);
        wcex.hbrBackground = (HBRUSH)::CreateSolidBrush(0x0D220C);
        wcex.lpszMenuName = NULL;
        wcex.lpszClassName = className;
        wcex.hIconSm = NULL;
        ::RegisterClassEx(&wcex);
    }

    const int width = rect.right - rect.left;
    const int height = rect.bottom - rect.top;

    window = ::CreateWindow(className, TEXT(""),
            WS_VISIBLE | WS_CHILD | WS_CLIPCHILDREN,
            rect.left, rect.top, width, height, parent, NULL, Instance, NULL);

    ::SetWindowLong(window, GWLP_USERDATA, LONG(this));

    cancelButton = new Button(window,
            MakeRECT(MakePOINT(Margin, Margin), MakeSIZE(width - Margin * 2, height - Margin * 2)),
            GetString(IDS_CLICKING_BUTTON).c_str(), CancelButtonId);
    cancelButton->setChecked(true);

    ::ShowWindow(window, SW_SHOW);
    ::UpdateWindow(window);
}

//-----------------------------------------------------------------------------

ClickerWindow::~ClickerWindow()
{
    delete cancelButton;
    ::DestroyWindow(window);

    if (--refCount == 0)
    {
        ::UnregisterClass(className, Instance);
    }
}

//-----------------------------------------------------------------------------

HWND
ClickerWindow::getHwnd() const
{
    return window;
}

//-----------------------------------------------------------------------------

LRESULT CALLBACK
ClickerWindow::windowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    if (message == WM_SIZE)
    {
        const WORD width = LOWORD(lParam);
        const WORD height = HIWORD(lParam);
        ClickerWindow* const sender
            = reinterpret_cast<ClickerWindow*>(::GetWindowLong(hwnd, GWLP_USERDATA));
        if (sender != NULL)
        {
            sender->cancelButton->resize(MakeSIZE(width - Margin * 2, height - Margin * 2));
        }
        return 0;
    }
    if (message == WM_COMMAND)
    {
        assert(LOWORD(wParam) == CancelButtonId);
        ClickerWindow* const sender
            = reinterpret_cast<ClickerWindow*>(::GetWindowLong(hwnd, GWLP_USERDATA));
        if (sender != NULL)
        {
            sender->buddyButton->click();
        }
        return 0;
    }
    return ::DefWindowProc(hwnd, message, wParam, lParam);
}

//-----------------------------------------------------------------------------

}// namespace Gui

//-----------------------------------------------------------------------------

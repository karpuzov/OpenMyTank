
//-----------------------------------------------------------------------------

#include "gui.h"

#include <sstream>
#include <cassert>

//-----------------------------------------------------------------------------

extern HINSTANCE Instance;

//-----------------------------------------------------------------------------

RECT MakeRECT(const int x, const int y, const int right, const int bottom)
{
  RECT rect = {x, y, right, bottom};
  return rect;
}

POINT MakePOINT(const int x, const int y)
{
  POINT point = {x, y};
  return point;
}

SIZE MakeSIZE(const int width, const int height)
{
  SIZE size = {width, height};
  return size;
}

RECT MakeRECT(const POINT& point, const SIZE& size)
{
  RECT rect = {point.x, point.y, point.x + size.cx - 1, point.y + size.cy - 1};
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

const TCHAR*const GetWindowClassName()
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
    : hdcPtr(hdc)
    , ScreenHdc(*hdc)
    , Width(width)
    , Height(height)
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

  HDC*const hdcPtr;
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
  : ParentHwnd(parent)
{
  Position.x = rect.left;
  Position.y = rect.top;
  Size.cx = rect.right - rect.left + 1;
  Size.cy = rect.bottom - rect.top + 1;
}

//-----------------------------------------------------------------------------

Window::~Window()
{
}

//-----------------------------------------------------------------------------

HWND
Window::GetHwnd() const
{
  return Hwnd;
}

//-----------------------------------------------------------------------------

POINT
Window::GetPoint() const
{
  return Position;
}

//-----------------------------------------------------------------------------

SIZE
Window::GetSize() const
{
  return Size;
}

//-----------------------------------------------------------------------------

void
Window::SetRect(const RECT& rect)
{
  Position.x = rect.left;
  Position.y = rect.top;
  Size.cx = rect.right - rect.left + 1;
  Size.cy = rect.bottom - rect.top + 1;
  UpdatePosition();
}

//-----------------------------------------------------------------------------

void
Window::Resize(const SIZE& newSize)
{
  Size = newSize;
  UpdatePosition();
}

//-----------------------------------------------------------------------------

void
Window::Move(const POINT& point)
{
  Position = point;
  UpdatePosition();
}

//-----------------------------------------------------------------------------

void
Window::Repaint()
{
  ::InvalidateRect(Hwnd, NULL, FALSE);
  ::UpdateWindow(Hwnd);
}

//-----------------------------------------------------------------------------

void
Window::Draw(HDC hdc) const
{
}

//-----------------------------------------------------------------------------

void
Window::UpdatePosition()
{
  ::MoveWindow(Hwnd, Position.x, Position.y, Size.cx, Size.cy, TRUE);
}

//-----------------------------------------------------------------------------
//    Label
//-----------------------------------------------------------------------------

int Label::RefCount = 0;
HFONT Label::DefaultFont = NULL;

//-----------------------------------------------------------------------------

Label::Label(const HWND parent,
             const RECT& rect,
             const TCHAR text[])
 : Window(parent, rect)
 , Caption(text)
{
  if (RefCount++ == 0)
  {
    WNDCLASSEX wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style		= 0;
    wcex.lpfnWndProc	= Label::WindowProcedure;
    wcex.cbClsExtra	= 0;
    wcex.cbWndExtra	= 0;
    wcex.hInstance	= Instance;
    wcex.hIcon		= NULL;
    wcex.hCursor	= ::LoadCursor(0, IDC_ARROW);
    wcex.hbrBackground	= (HBRUSH)NULL;
    wcex.lpszMenuName	= NULL;
    wcex.lpszClassName	= GetClassName();
    wcex.hIconSm	= NULL;

    ::RegisterClassEx(&wcex);

    LOGFONT logFont = {0};
    DefaultFont = ::CreateFontIndirect(&logFont);
  }

  Font = DefaultFont;

  Hwnd = ::CreateWindow(GetClassName(),
    text,
    WS_VISIBLE | WS_CHILD,
    Position.x, Position.y,
    Size.cx, Size.cy,
    ParentHwnd,
    NULL,
    Instance,
    NULL);

  ::SetWindowLong(Hwnd, GWLP_USERDATA, LONG(this));
}

//-----------------------------------------------------------------------------

Label::~Label()
{
  if (Font != DefaultFont)
  {
    ::DeleteObject(Font);
  }
  ::DestroyWindow(Hwnd);

  if (--RefCount == 0)
  {
    ::DeleteObject(DefaultFont);
    ::UnregisterClass(GetClassName(), Instance);
  }
}

//-----------------------------------------------------------------------------

void
Label::SetTextColor(const COLORREF color)
{
  TextColor = color;
}

//-----------------------------------------------------------------------------

void
Label::SetBackgroundColor(const COLORREF color)
{
  BackgroundColor = color;
}

//-----------------------------------------------------------------------------

void
Label::SetFont(const LOGFONT& logFont)
{
  if (Font != DefaultFont)
  {
    ::DeleteObject(Font);
  }
  Font = ::CreateFontIndirect(&logFont);
}

//-----------------------------------------------------------------------------

void
Label::Draw(HDC hdc) const
{
  RECT rc;
  ::GetClientRect(Hwnd, &rc);

  DoubleHdc buffer(&hdc, rc.right, rc.bottom);

  {
    const HBRUSH brush = ::CreateSolidBrush(BackgroundColor);
    ::FillRect(hdc, &rc, brush);
    ::DeleteObject(brush);
  }
  {
    const HFONT oldFont = (HFONT)::SelectObject(hdc, Font);
    {
      ::SetTextColor(hdc, TextColor);
      ::SetBkColor(hdc, BackgroundColor);
      ::DrawText(hdc, Caption.c_str(), Caption.length(), &rc, DT_SINGLELINE | DT_NOCLIP | DT_CENTER | DT_VCENTER);
    }
    ::SelectObject(hdc, oldFont);
  }
}

//-----------------------------------------------------------------------------

LRESULT CALLBACK
Label::WindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  switch (message)
  {
  case WM_ERASEBKGND:
    {
      return 1;
    }
  case WM_PAINT:
    {
      Label*const window = reinterpret_cast<Label*>(::GetWindowLong(hwnd, GWLP_USERDATA));
      PAINTSTRUCT ps;
      const HDC hdc = ::BeginPaint(hwnd, &ps);
      window->Draw(hdc);
      ::EndPaint(hwnd, &ps);
      return 0;
    }
  }
  return ::DefWindowProc(hwnd, message, wParam, lParam);
}

//-----------------------------------------------------------------------------
//    MultilineLabel
//-----------------------------------------------------------------------------

MultilineLabel::MultilineLabel(const HWND parent,
                               const RECT& rect,
                               const TCHAR text[])
  : Label(parent, rect, text)
{
}

//-----------------------------------------------------------------------------

void
MultilineLabel::Draw(HDC hdc) const
{
  RECT rc;
  ::GetClientRect(Hwnd, &rc);

  DoubleHdc buffer(&hdc, rc.right, rc.bottom);

  {
    const HBRUSH brush = ::CreateSolidBrush(BackgroundColor);
    ::FillRect(hdc, &rc, brush);
    ::DeleteObject(brush);
  }
  {
    const HFONT oldFont = (HFONT)::SelectObject(hdc, Font);
    {
      ::SetTextColor(hdc, TextColor);
      ::SetBkColor(hdc, BackgroundColor);
      ::DrawText(hdc, Caption.c_str(), Caption.length(), &rc, DT_WORDBREAK | DT_CENTER | DT_VCENTER);
    }
    ::SelectObject(hdc, oldFont);
  }
}

//-----------------------------------------------------------------------------
//    Button
//-----------------------------------------------------------------------------

int Gui::Button::RefCount = 0;
HFONT Gui::Button::Font = NULL;

//-----------------------------------------------------------------------------

Button::Button(const HWND parent,
               const RECT& rect,
               const TCHAR text[],
               const DWORD id,
               bool checkboxLike)
  : Window(parent, rect)
  , Caption(text)
  , Id(id)
  , IsCheckbox(checkboxLike)
  , IsEnabled(true)
  , IsInside(false)
  , IsPressed(false)
  , IsChecked(false)
{
  if (RefCount++ == 0)
  {
    WNDCLASSEX wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style		= 0;
    wcex.lpfnWndProc	= Button::WindowProcedure;
    wcex.cbClsExtra	= 0;
    wcex.cbWndExtra	= 0;
    wcex.hInstance	= Instance;
    wcex.hIcon		= NULL;
    wcex.hCursor	= ::LoadCursor(0, IDC_ARROW);
    wcex.hbrBackground	= (HBRUSH)NULL;
    wcex.lpszMenuName	= NULL;
    wcex.lpszClassName	= GetClassName();
    wcex.hIconSm	= NULL;

    ::RegisterClassEx(&wcex);

    LOGFONT logFont = {0};
    logFont.lfCharSet = RUSSIAN_CHARSET;
    logFont.lfHeight = 15;
    logFont.lfWeight = FW_NORMAL;
    ::lstrcpy(logFont.lfFaceName, TEXT("Arial"));
    assert(Font == NULL);
    Font = ::CreateFontIndirect(&logFont);
  }

  Hwnd = ::CreateWindow(GetClassName(),
    text,
    WS_VISIBLE | WS_CHILD,
    Position.x, Position.y,
    Size.cx, Size.cy,
    ParentHwnd,
    (HMENU)(Id),
    Instance,
    NULL);

  ::SetWindowLong(Hwnd, GWLP_USERDATA, LONG(this));

  if (Size.cx == 0)
  {
    Resize(MakeSIZE(GetMinSize().cx, GetSize().cy));
  }
}

//-----------------------------------------------------------------------------

Button::~Button()
{
  ::DestroyWindow(Hwnd);

  if (--RefCount == 0)
  {
    assert(Font != NULL);
    ::DeleteObject(Font);
    Font = NULL;

    ::UnregisterClass(GetClassName(), Instance);
  }
}

//-----------------------------------------------------------------------------

DWORD
Button::GetId() const
{
  return Id;
}

//-----------------------------------------------------------------------------

void
Button::SetEnabled(const bool enabled)
{
  IsEnabled = enabled;
  Repaint();
}

//-----------------------------------------------------------------------------

bool
Button::GetChecked() const
{
  return IsChecked;
}

//-----------------------------------------------------------------------------

void
Button::SetChecked(const bool checked)
{
  IsChecked = checked;
  Repaint();
}

//-----------------------------------------------------------------------------

void
Button::Click()
{
  if (IsEnabled)
  {
    SetChecked(!IsChecked);
    ::PostMessage(ParentHwnd, WM_COMMAND, WPARAM(Id), LPARAM(this));
  }
}

//-----------------------------------------------------------------------------

void
Button::Draw(HDC hdc) const
{
  RECT rc;
  ::GetClientRect(Hwnd, &rc);

  DoubleHdc buffer(&hdc, rc.right, rc.bottom);

  COLORREF bgColor, textColor;
  if (!IsEnabled)
  {
    bgColor = 0x575757;
    textColor = 0xb1b1b1;
  }
  else
  {
    if (IsPressed && IsInside)
    {
      bgColor = (IsChecked) ? (0x2ce74f) : (0x185b1f);
    }
    else
    {
      bgColor = (IsChecked) ? (0x31FF58) : (0x004A08);
    }
    textColor = (IsChecked) ? (0x0E280E) : (0x30FB58);
  }

  // Background with border:
  const HBRUSH brush = ::CreateSolidBrush(bgColor);
  if (IsEnabled && (IsInside || IsPressed))
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
    const HFONT oldFont = (HFONT)::SelectObject(hdc, Font);
    ::SetTextColor(hdc, textColor);
    ::SetBkColor(hdc, bgColor);
    ::DrawText(hdc, Caption.c_str(), Caption.length(), &rc, DT_SINGLELINE | DT_NOCLIP | DT_CENTER | DT_VCENTER);
    ::SelectObject(hdc, oldFont);
  }
}

//-----------------------------------------------------------------------------

LRESULT CALLBACK
Button::WindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  switch (message)
  {
  case WM_ERASEBKGND:
    {
      return 1;
    }
  case WM_PAINT:
    {
      Button*const window = reinterpret_cast<Button*>(::GetWindowLong(hwnd, GWLP_USERDATA));
      PAINTSTRUCT ps;
      const HDC hdc = ::BeginPaint(hwnd, &ps);
      window->Draw(hdc);
      ::EndPaint(hwnd, &ps);
      return 0;
    }
  case WM_MOUSEMOVE:
    {
      Button*const window = reinterpret_cast<Button*>(::GetWindowLong(hwnd, GWLP_USERDATA));
      if (window->IsEnabled)
      {
        if (window->IsPressed)
        {
          RECT rc;
          ::GetWindowRect(hwnd, &rc);
          POINT pt = {LOWORD(lParam), HIWORD(lParam)};
          ::ClientToScreen(hwnd, &pt);
          const bool newInside = bool(::PtInRect(&rc, pt) == TRUE);
          if (window->IsInside != newInside)
          {
            window->IsInside = newInside;
            window->Repaint();
          }
        }
        else
        {
          if (!window->IsInside)
          {
            window->IsInside = true;

            TRACKMOUSEEVENT m;
            m.cbSize = sizeof(TRACKMOUSEEVENT);
            m.hwndTrack = hwnd;
            m.dwFlags = TME_LEAVE;
            m.dwHoverTime = HOVER_DEFAULT;
            ::TrackMouseEvent(&m);

            window->Repaint();
          }
        }
      }
      return 0;
    }
  case WM_MOUSELEAVE:
    {
      Button*const window = reinterpret_cast<Button*>(::GetWindowLong(hwnd, GWLP_USERDATA));
      window->IsInside = false;
      window->Repaint();
      return 0;
    }
  case WM_LBUTTONDOWN:
    {
      Button*const window = reinterpret_cast<Button*>(::GetWindowLong(hwnd, GWLP_USERDATA));
      if (window->IsEnabled)
      {
        window->IsPressed = true;
        ::SetCapture(hwnd);
        window->Repaint();
      }
      return 0;
    }
  case WM_LBUTTONUP:
    {
      Button*const window = reinterpret_cast<Button*>(::GetWindowLong(hwnd, GWLP_USERDATA));
      if (window->IsEnabled)
      {
        if (window->IsPressed && window->IsInside)
        {
          if (window->IsCheckbox)
          {
            window->IsChecked = !window->IsChecked;
          }
          ::PostMessage(window->ParentHwnd, WM_COMMAND, WPARAM(window->Id), LPARAM(window));
        }
        window->IsPressed = false;
        ::ReleaseCapture();
        window->Repaint();
      }
      return 0;
    }
  case WM_CAPTURECHANGED:
    {
      Button*const window = reinterpret_cast<Button*>(::GetWindowLong(hwnd, GWLP_USERDATA));
      if (window->IsPressed)
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
Button::GetMinSize() const
{
  SIZE size;

  const HDC hdc = ::GetWindowDC(Hwnd);
  LOGFONT logFont = {0};
  logFont.lfCharSet = RUSSIAN_CHARSET;
  logFont.lfHeight = 15;
  logFont.lfWeight = FW_NORMAL;
  ::lstrcpy(logFont.lfFaceName, TEXT("Arial"));
  const HFONT font = ::CreateFontIndirect(&logFont);
  const HFONT oldFont = (HFONT)::SelectObject(hdc, font);
  const tstring newText(TEXT(" ") + Caption + TEXT(" "));
  ::GetTextExtentPoint(hdc, newText.c_str(), newText.length(), &size);
  ::SelectObject(hdc, oldFont);
  ::DeleteObject(font);
  ::ReleaseDC(Hwnd, hdc);

  return size;
}

//-----------------------------------------------------------------------------
//    AboutWindow
//-----------------------------------------------------------------------------

AboutWindow::AboutWindow(CONST HWND parent)
  : ClassName(TEXT("OpenMyTank#About"))
  , ParentWindow(parent)
  , ProgramNameStatic(NULL)
  , VersionStatic(NULL)
  , NewsStatic(NULL)
  , SettingsButton(NULL)
  , HelpPageButton(NULL)
  , HomePageButton(NULL)
  , VkGroupPageButton(NULL)
  , NewCopyrightButton(NULL)
  , OldCopyrightButton(NULL)
{
  WNDCLASSEX wcex;
  wcex.cbSize = sizeof(WNDCLASSEX);

  wcex.style         = 0;
  wcex.lpfnWndProc   = AboutWindow::WindowProcedure;
  wcex.cbClsExtra    = 0;
  wcex.cbWndExtra    = 0;
  wcex.hInstance     = Instance;
  wcex.hIcon         = NULL;
  wcex.hCursor       = ::LoadCursor(0, IDC_ARROW);
  wcex.hbrBackground = (HBRUSH)::CreateSolidBrush(0x0D220C);
  wcex.lpszMenuName  = NULL;
  wcex.lpszClassName = ClassName;
  wcex.hIconSm	= NULL;

  if (::RegisterClassEx(&wcex) != 0)
  {
    Window = ::CreateWindow(ClassName, TEXT(""),
      WS_VISIBLE | WS_CHILD | WS_CLIPCHILDREN,
      0, 0, 0, 0, parent, NULL, Instance, NULL);

    LOGFONT logFont = {0};
    logFont.lfWeight = FW_NORMAL;
    logFont.lfHeight = 40;

    TCHAR caption[256];

    ProgramNameStatic = new Label(Window, MakeRECT(MakePOINT(0, 50), MakeSIZE(Width, 40)), ProgramTitle);
    ProgramNameStatic->SetTextColor(0x00FF00);
    ProgramNameStatic->SetBackgroundColor(0x0D220C);
    ProgramNameStatic->SetFont(logFont);

    const tstring versionFormat(GetString(IDS_VERSION));
    ::wsprintf(caption, versionFormat.c_str(), C2T(VersionNumber).c_str(), C2T(VersionDate).c_str());
    VersionStatic = new Label(Window, MakeRECT(MakePOINT(0, 50+40+20), MakeSIZE(Width, 20)), caption);
    VersionStatic->SetTextColor(0x00AA00);
    VersionStatic->SetBackgroundColor(0x0D220C);
    logFont.lfHeight = 20;
    VersionStatic->SetFont(logFont);

    NewsStatic = new MultilineLabel(Window, MakeRECT(MakePOINT(100, 50+40+20 +65), MakeSIZE(Width-100-100, 75)), GetString(IDS_NEWS).c_str());
    NewsStatic->SetTextColor(0x00DDDD);
    NewsStatic->SetBackgroundColor(0x0D220C);
    logFont.lfHeight = 18;
    NewsStatic->SetFont(logFont);

    SettingsButton = new Button(Window, MakeRECT(MakePOINT(350, 260), MakeSIZE(Width - 350 * 2, 40)), GetString(IDS_SETTINGS_BUTTON).c_str(), SettingsButtonId);

    HelpPageButton = new Button(Window, MakeRECT(MakePOINT(300, Height - 260), MakeSIZE(Width - 300 * 2, 30)), GetString(IDS_HELP_PAGE_BUTTON).c_str(), HelpPageButtonId);

	HomePageButton = new Button(Window, MakeRECT(MakePOINT(345, Height - 190), MakeSIZE(Width - 345 * 2, 30)), ProgramHomePage, HomePageButtonId);

    const tstring vkGroupString(TEXT("OpenMyTank in VK.com"));
	VkGroupPageButton = new Button(Window, MakeRECT(MakePOINT(345, Height - 150), MakeSIZE(Width - 345 * 2, 30)), vkGroupString.c_str(), VkGroupPageButtonId);

    const tstring newCopyrightString(TEXT("Copyright (C) 2014 OpenMyTank developers on GitHub"));
    NewCopyrightButton = new Button(Window, MakeRECT(MakePOINT(345, Height - 80), MakeSIZE(Width - 345 * 2, 25)), newCopyrightString.c_str(), NewCopyrightButtonId);

    const tstring copyrightString(TEXT("Copyright (C) 2012-2014 Karpuzov Valery"));
    OldCopyrightButton = new Button(Window, MakeRECT(MakePOINT(380, Height - 50), MakeSIZE(Width - 380 * 2, 20)), copyrightString.c_str(), OldCopyrightButtonId);

    ::ShowWindow(Window, SW_SHOW);
    ::UpdateWindow(Window);
  }
}

//-----------------------------------------------------------------------------

AboutWindow::~AboutWindow()
{
  delete ProgramNameStatic;
  delete VersionStatic;
  delete NewsStatic;
  delete SettingsButton;
  delete HelpPageButton;
  delete HomePageButton;
  delete VkGroupPageButton;
  delete NewCopyrightButton;
  delete OldCopyrightButton;
  ::DestroyWindow(Window);
  ::UnregisterClass(ClassName, Instance);
}

//-----------------------------------------------------------------------------

void
AboutWindow::UpdateSize()
{
  RECT rect;
  ::GetClientRect(ParentWindow, &rect);
  ::MoveWindow(Window,
               rect.left + (rect.right - 1000) / 2, 1 + GetMenuHeight() + (rect.bottom - 630) / 2,
               Width, Height, TRUE);
}

//-----------------------------------------------------------------------------

LRESULT CALLBACK
AboutWindow::WindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
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

int Gui::ClickerWindow::RefCount = 0;

//-----------------------------------------------------------------------------

ClickerWindow::ClickerWindow(const HWND parent, const RECT rect, Button* button)
  : ClassName(TEXT("OpenMyTank#Clicker"))
  , BuddyButton(button)
  , CancelButton(NULL)
{
  if (RefCount++ == 0)
  {
    WNDCLASSEX wcex;
    wcex.cbSize        = sizeof(WNDCLASSEX);
    wcex.style         = 0;
    wcex.lpfnWndProc   = ClickerWindow::WindowProcedure;
    wcex.cbClsExtra    = 0;
    wcex.cbWndExtra    = 0;
    wcex.hInstance     = Instance;
    wcex.hIcon         = NULL;
    wcex.hCursor       = ::LoadCursor(0, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)::CreateSolidBrush(0x0D220C);
    wcex.lpszMenuName  = NULL;
    wcex.lpszClassName = ClassName;
    wcex.hIconSm       = NULL;
    ::RegisterClassEx(&wcex);
  }

  const int width = rect.right - rect.left;
  const int height = rect.bottom - rect.top;

  Window = ::CreateWindow(ClassName, TEXT(""),
    WS_VISIBLE | WS_CHILD | WS_CLIPCHILDREN,
    rect.left, rect.top, width, height, parent, NULL, Instance, NULL);
  
  ::SetWindowLong(Window, GWLP_USERDATA, LONG(this));

  CancelButton = new Button(Window,
                            MakeRECT(MakePOINT(Margin, Margin), MakeSIZE(width - Margin*2, height - Margin*2)),
                            GetString(IDS_CLICKING_BUTTON).c_str(),
                            CancelButtonId);
  CancelButton->SetChecked(true);

  ::ShowWindow(Window, SW_SHOW);
  ::UpdateWindow(Window);
}

//-----------------------------------------------------------------------------

ClickerWindow::~ClickerWindow()
{
  delete CancelButton;
  ::DestroyWindow(Window);

  if (--RefCount == 0)
  {
    ::UnregisterClass(ClassName, Instance);
  }
}

//-----------------------------------------------------------------------------

HWND
ClickerWindow::GetHwnd() const
{
  return Window;
}

//-----------------------------------------------------------------------------

LRESULT CALLBACK
ClickerWindow::WindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  if (message == WM_SIZE)
  {
    const WORD width = LOWORD(lParam); 
    const WORD height = HIWORD(lParam);
    ClickerWindow*const sender = reinterpret_cast<ClickerWindow*>(::GetWindowLong(hwnd, GWLP_USERDATA));
    if (sender != NULL)
    {
      sender->CancelButton->Resize(MakeSIZE(width - Margin * 2, height - Margin * 2));
    }
    return 0;
  }
  if (message == WM_COMMAND)
  {
    assert(LOWORD(wParam) == CancelButtonId);
    ClickerWindow*const sender = reinterpret_cast<ClickerWindow*>(::GetWindowLong(hwnd, GWLP_USERDATA));
    if (sender != NULL)
    {
      sender->BuddyButton->Click();
    }
    return 0;
  }
  return ::DefWindowProc(hwnd, message, wParam, lParam);
}

//-----------------------------------------------------------------------------

} // namespace Gui

//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------

#pragma once

//-----------------------------------------------------------------------------

#include "thread.h"

#include <windows.h>

//-----------------------------------------------------------------------------

namespace Gdiplus
{
class EncoderParameters;
}

//-----------------------------------------------------------------------------

class Screenshoter
{
public:

  Screenshoter(const WCHAR* path,
               const TCHAR* dateFormat,
               const TCHAR* timeFormat,
               const WCHAR* format,
               const LONG jpegQuality,
               const bool beepOnSuccess);
  ~Screenshoter();

  bool SaveScreen(const HWND window);

private:

  const WCHAR*const Path;
  const TCHAR*const DateFormat;
  const TCHAR*const TimeFormat;
  const WCHAR*const Format;

  ULONG_PTR GdiplusToken;

  WCHAR FileName[MAX_PATH];
  WCHAR* DateStart;

  const LONG JpegQuality;
  Gdiplus::EncoderParameters* JpegParameters;

  const bool BeepOnSuccess;

  WCHAR*const MakeFileName();
};

//-----------------------------------------------------------------------------

class ScreenshoterThread : public Thread
{
public:

  ScreenshoterThread(const HWND window,
                     const WCHAR* path,
                     const TCHAR* dateFormat,
                     const TCHAR* timeFormat,
                     const WCHAR* format,
                     const LONG jpegQuality,
                     const bool beepOnSuccess,
                     const HWND parentWindow)
    : Thread(parentWindow)
    , Window(window)
    , Path(path)
    , DateFormat(dateFormat)
    , TimeFormat(timeFormat)
    , Format(format)
    , JpegQuality(jpegQuality)
    , BeepOnSuccess(beepOnSuccess)
  {
  }

  void Run()
  {
    Screenshoter(Path, DateFormat, TimeFormat, Format, JpegQuality, BeepOnSuccess)
        .SaveScreen(Window);
  }

private:

  const HWND Window;

  const WCHAR*const Path;
  const TCHAR*const DateFormat;
  const TCHAR*const TimeFormat;
  const WCHAR*const Format;
  const LONG JpegQuality;
  const bool BeepOnSuccess;

};

//-----------------------------------------------------------------------------

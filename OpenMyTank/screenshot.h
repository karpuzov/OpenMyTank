//-----------------------------------------------------------------------------

#pragma once

//-----------------------------------------------------------------------------

#include "thread.h"

#include <windows.h>

//-----------------------------------------------------------------------------

namespace Gdiplus { class EncoderParameters; }

//-----------------------------------------------------------------------------

class Screenshoter
{
public:

    Screenshoter(
        const WCHAR* path, const TCHAR* dateFormat, const TCHAR* timeFormat, const WCHAR* format,
        const LONG jpegQuality, const bool beepOnSuccess);
    ~Screenshoter();

    bool saveScreen(const HWND window);

private:

    const WCHAR* const path;
    const TCHAR* const dateFormat;
    const TCHAR* const timeFormat;
    const WCHAR* const format;

    ULONG_PTR gdiplusToken;

    WCHAR fileName[MAX_PATH];
    WCHAR* dateStart;

    const LONG jpegQuality;
    Gdiplus::EncoderParameters* jpegParameters;

    const bool beepOnSuccess;

    WCHAR* const makeFileName();
};

//-----------------------------------------------------------------------------

class ScreenshoterThread : public Thread
{
public:

    ScreenshoterThread(
        const HWND window, const WCHAR* path, const TCHAR* dateFormat, const TCHAR* timeFormat,
        const WCHAR* format, const LONG jpegQuality, const bool beepOnSuccess,
        const HWND parentWindow)
            : Thread(parentWindow), window(window), path(path), dateFormat(dateFormat)
            , timeFormat(timeFormat), format(format), jpegQuality(jpegQuality)
            , beepOnSuccess(beepOnSuccess)
    {
    }

    void run()
    {
        Screenshoter(path, dateFormat, timeFormat, format, jpegQuality, beepOnSuccess).saveScreen(
                window);
    }

private:

    const HWND window;

    const WCHAR* const path;
    const TCHAR* const dateFormat;
    const TCHAR* const timeFormat;
    const WCHAR* const format;
    const LONG jpegQuality;
    const bool beepOnSuccess;

};

//-----------------------------------------------------------------------------

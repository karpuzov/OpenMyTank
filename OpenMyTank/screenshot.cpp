//-----------------------------------------------------------------------------

#include "screenshot.h"
#include "settings.h"
#include "resource.h"

#include <gdiplus.h>
#include <Shlwapi.h>
#include <mmsystem.h>

#pragma comment(lib, "gdiplus")
#pragma comment(lib, "winmm")

//-----------------------------------------------------------------------------

bool GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
{
    using namespace Gdiplus;

    bool ok = false;

    UINT num = 0;          // number of image encoders
    UINT size = 0;         // size of the image encoder array in bytes
    GetImageEncodersSize(&num, &size);
    if (size != 0)
    {
        ImageCodecInfo* pImageCodecInfo = (ImageCodecInfo*)(malloc(size));
        if (pImageCodecInfo != NULL)
        {
            GetImageEncoders(num, size, pImageCodecInfo);

            for (UINT j = 0; j < num && !ok; ++j)
            {
                if (wcscmp(pImageCodecInfo[j].MimeType, format) == 0)
                {
                    *pClsid = pImageCodecInfo[j].Clsid;
                    ok = true;
                }
            }

            free(pImageCodecInfo);
        }
    }

    return ok;
}

//-----------------------------------------------------------------------------

const CLSID* GetConstClsid(const WCHAR* format)
{
    static CLSID clsid;
    static std::basic_string<WCHAR> lastFormatString;

    std::basic_string<WCHAR> formatString(TEXT("image/"));
    formatString.append(format);

    if (lastFormatString != formatString)
    {
        GetEncoderClsid(formatString.c_str(), &clsid);
        lastFormatString = formatString;
    }

    return &clsid;
}

//-----------------------------------------------------------------------------

Screenshoter::Screenshoter(
    const WCHAR* path, const TCHAR* dateFormat, const TCHAR* timeFormat, const WCHAR* format,
    const LONG jpegQuality, const bool beepOnSuccess)
        : path(path), dateFormat(dateFormat), timeFormat(timeFormat), format(format),
            jpegQuality(jpegQuality), jpegParameters(NULL), beepOnSuccess(beepOnSuccess)
{
    Gdiplus::GdiplusStartupInput gdiplusStartupInput;
    Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

    wcscpy_s(fileName, sizeof(fileName) / sizeof(fileName[0]), path);
    ::PathAddBackslash(fileName);
    dateStart = fileName + wcslen(fileName);

    if (format[0] == L'j' && format[1] == L'p') // "jpg" or "jpeg"
    {
        jpegParameters = new Gdiplus::EncoderParameters;
        jpegParameters->Count = 1;
        jpegParameters->Parameter[0].Guid = Gdiplus::EncoderQuality;
        jpegParameters->Parameter[0].Type = Gdiplus::EncoderParameterValueTypeLong;
        jpegParameters->Parameter[0].NumberOfValues = 1;
        jpegParameters->Parameter[0].Value = (VOID*)(&jpegQuality);
    }
}

//-----------------------------------------------------------------------------

Screenshoter::~Screenshoter()
{
    Gdiplus::GdiplusShutdown(gdiplusToken);

    delete jpegParameters;
}

//-----------------------------------------------------------------------------

bool
Screenshoter::saveScreen(const HWND window)
{
    bool ok = false;

    const HDC dc = ::GetDC(window);
    RECT rect;
    if (::GetWindowRect(window, &rect))
    {
        const int width = rect.right - rect.left;
        const int height = rect.bottom - rect.top;

        const HDC memDc = ::CreateCompatibleDC(dc);
        const HBITMAP memBitmap = ::CreateCompatibleBitmap(dc, width, height);
        const HBITMAP oldBitmap = (HBITMAP)::SelectObject(memDc, memBitmap);

        ::BitBlt(memDc, 0, 0, width, height, dc, 0, 0, SRCCOPY);

        {
            ::CreateDirectory(path, NULL);

            Gdiplus::Bitmap bitmap(memBitmap, NULL);
            if (Gdiplus::Ok == bitmap.Save(makeFileName(), GetConstClsid(format), jpegParameters))
            {
                ok = true;
            }
        }

        ::SelectObject(memDc, oldBitmap);
        ::DeleteObject(memDc);
        ::DeleteObject(memBitmap);
    }
    ::ReleaseDC(window, dc);

    if (beepOnSuccess && ok)
    {
        ::PlaySound(MAKEINTRESOURCE(IDR_BEEPWAVE), NULL, SND_RESOURCE | SND_SYNC);
    }
    return ok;
}

//-----------------------------------------------------------------------------

WCHAR* const
Screenshoter::makeFileName()
{
    WCHAR* string = dateStart;

    string += ::GetDateFormat(LOCALE_USER_DEFAULT, 0,
    NULL, dateFormat, string,
    MAX_PATH - (string - fileName));
    --string;

    string += ::GetTimeFormat(LOCALE_USER_DEFAULT, 0,
    NULL, timeFormat, string,
    MAX_PATH - (string - fileName));
    --string;

    wcscpy_s(string, MAX_PATH - (string - fileName), format);

    return fileName;
}

//-----------------------------------------------------------------------------

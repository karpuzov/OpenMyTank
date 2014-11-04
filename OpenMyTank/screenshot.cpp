
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

  UINT  num = 0;          // number of image encoders
  UINT  size = 0;         // size of the image encoder array in bytes
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

Screenshoter::Screenshoter(const WCHAR* path,
                           const TCHAR* dateFormat,
                           const TCHAR* timeFormat,
                           const WCHAR* format,
                           const LONG jpegQuality,
                           const bool beepOnSuccess)
  : Path(path)
  , DateFormat(dateFormat)
  , TimeFormat(timeFormat)
  , Format(format)
  , JpegQuality(jpegQuality)
  , JpegParameters(NULL)
  , BeepOnSuccess(beepOnSuccess)
{
  Gdiplus::GdiplusStartupInput gdiplusStartupInput;
  Gdiplus::GdiplusStartup(&GdiplusToken, &gdiplusStartupInput, NULL);

  wcscpy_s(FileName, sizeof(FileName) / sizeof(FileName[0]), Path);
  ::PathAddBackslash(FileName);
  DateStart = FileName + wcslen(FileName);

  if (format[0] == L'j' && format[1] == L'p') // "jpg" or "jpeg"
  {
    JpegParameters = new Gdiplus::EncoderParameters;
    JpegParameters->Count = 1;
    JpegParameters->Parameter[0].Guid = Gdiplus::EncoderQuality;
    JpegParameters->Parameter[0].Type = Gdiplus::EncoderParameterValueTypeLong;
    JpegParameters->Parameter[0].NumberOfValues = 1;
    JpegParameters->Parameter[0].Value = (VOID*)(&JpegQuality);
  }
}

//-----------------------------------------------------------------------------

Screenshoter::~Screenshoter()
{
  Gdiplus::GdiplusShutdown(GdiplusToken);

  delete JpegParameters;
}

//-----------------------------------------------------------------------------

bool
Screenshoter::SaveScreen(const HWND window)
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
      ::CreateDirectory(Path, NULL);

      Gdiplus::Bitmap bitmap(memBitmap, NULL);
      if (Gdiplus::Ok == bitmap.Save(MakeFileName(), GetConstClsid(Format), JpegParameters))
      {
        ok = true;
      }
    }

    ::SelectObject(memDc, oldBitmap);
    ::DeleteObject(memDc);
    ::DeleteObject(memBitmap);
  }
  ::ReleaseDC(window, dc);

  if (BeepOnSuccess && ok)
  {
    ::PlaySound(MAKEINTRESOURCE(IDR_BEEPWAVE), NULL, SND_RESOURCE | SND_SYNC);
  }
  return ok;
}

//-----------------------------------------------------------------------------

WCHAR*const
Screenshoter::MakeFileName()
{
  WCHAR* string = DateStart;

  string += ::GetDateFormat(LOCALE_USER_DEFAULT,
                            0,
                            NULL,
                            DateFormat,
                            string,
                            MAX_PATH - (string - FileName));
  --string;

  string += ::GetTimeFormat(LOCALE_USER_DEFAULT,
                            0,
                            NULL,
                            TimeFormat,
                            string,
                            MAX_PATH - (string - FileName));
  --string;

  wcscpy_s(string, MAX_PATH - (string - FileName), Format);

  return FileName;
}

//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------

#include "tstring.h"

#include <windows.h>

//-----------------------------------------------------------------------------

tstring C2T(const std::string& text)
{
  return tstring(text.begin(), text.end());
}

//-----------------------------------------------------------------------------

std::string T2C(const tstring& text)
{
  return std::string(text.begin(), text.end());
}

//-----------------------------------------------------------------------------

tstring Ansi2Tchar(const std::string& ansiString) // like atl 'A2T'
{
  const char br[] = "\r\n";
  TCHAR tbr[2 + 1];
  ::OemToChar(br, tbr);

  tstring ttext(ansiString.begin(), ansiString.end());
  ::OemToChar(ansiString.c_str(), &(ttext[0]));

  std::size_t pos = 0;
  while ((pos = ttext.find(tbr, pos)) != std::string::npos)
  {
    ttext.replace(pos, 2, TEXT("\r\n"));
  }

  return ttext;
}

//-----------------------------------------------------------------------------

std::string TCharToOem(const tstring& tText)
{
  std::string result(tText.begin(), tText.end());
  ::CharToOem(tText.c_str(), &(result[0]));
  return result;
}

//-----------------------------------------------------------------------------

tstring OemToTChar(const std::string& text)
{
  tstring result(text.begin(), text.end());
  ::OemToChar(text.c_str(), &(result[0]));
  return result;
}

//-----------------------------------------------------------------------------

extern HINSTANCE Instance;

tstring GetString(const int stringId)
{
  static TCHAR sz[255 + 1];
  ::LoadString(Instance, stringId, sz, sizeof(sz)/sizeof(*sz));
  return tstring(sz);
}

//-----------------------------------------------------------------------------

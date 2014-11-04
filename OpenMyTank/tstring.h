
//-----------------------------------------------------------------------------

#pragma once

//-----------------------------------------------------------------------------

#include <tchar.h>

#include <string>

//-----------------------------------------------------------------------------

typedef  std::basic_string<TCHAR>  tstring;

tstring C2T(const std::string& text);
std::string T2C(const tstring& text);

tstring Ansi2Tchar(const std::string& ansiString); // like atl 'A2T'

std::string TCharToOem(const tstring& tText);
tstring OemToTChar(const std::string& text);

//template <typename outputStringType, typename inputStringType>
//outputStringType To(const inputStringType& inputString)
//{
//  return outputStringType(inputString.begin(), inputString.end());
//}

//-----------------------------------------------------------------------------

// Load localized string from resources
tstring GetString(const int stringId);

//-----------------------------------------------------------------------------

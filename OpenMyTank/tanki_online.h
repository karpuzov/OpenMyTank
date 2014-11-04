
//-----------------------------------------------------------------------------

#pragma once

//-----------------------------------------------------------------------------

#include "tstring.h"

#include <windows.h>

//-----------------------------------------------------------------------------

namespace TO
{

//-----------------------------------------------------------------------------

class UrlManager
{
public:

  UrlManager(CONST TCHAR*CONST commandLineArgument);

  tstring GetServerLink(const unsigned serverNumber) const;

  tstring GetAccountCoockieName() const;

  bool HasAccount() const;

  tstring ToMyUrl(const tstring& publicUrl) const;
  tstring ToPublicUrl(const tstring& myUrl) const;

  static unsigned ExtractServerNumber(const tstring& url);

  static bool IsValidUrl(const tstring& url);

  static tstring SetMyLanguage(const tstring& url);

private:

  tstring FrontPart;
  const tstring MiddlePart;

  tstring MySlot;
  tstring MySlotPoint;

  tstring AccountCoockieName;

  static const tstring TankiOnlineServer;
};

//-----------------------------------------------------------------------------

int GetDelayForClicking();

SIZE MinimumPlayerSize();

unsigned GetNumberOfServers();

::POINTS GetRightPlayButtonClickPoint(CONST ::SIZE& size);
::POINTS GetLeftPlayButtonClickPoint(CONST ::SIZE& size);

::RECT GetRightClickerWindowRect(CONST ::SIZE& size);
::RECT GetLeftClickerWindowRect(CONST ::SIZE& size);

//-----------------------------------------------------------------------------

} // namespace TO

//-----------------------------------------------------------------------------

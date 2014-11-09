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

    tstring getServerLink(const unsigned serverNumber) const;

    tstring getAccountCoockieName() const;

    bool hasAccount() const;

    tstring toMyUrl(const tstring& publicUrl) const;
    tstring toPublicUrl(const tstring& myUrl) const;

    static unsigned extractServerNumber(const tstring& url);

    static bool isValidUrl(const tstring& url);

    static tstring setMyLanguage(const tstring& url);

private:

    tstring frontPart;
    const tstring middlePart;

    tstring mySlot;
    tstring mySlotPoint;

    tstring accountCoockieName;

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

}// namespace TO

//-----------------------------------------------------------------------------

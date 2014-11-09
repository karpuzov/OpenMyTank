//-----------------------------------------------------------------------------

#include "TankiOnline.h"
#include "Options.h"
#include "Settings.h"
#include "Account.h"

#include <cctype>
#include <cassert>
#include <sstream>

//-----------------------------------------------------------------------------

namespace TO
{

//-----------------------------------------------------------------------------
//    UrlManager
//-----------------------------------------------------------------------------

// Old variant: tankionline.com/battle-(*loc)(*N).html
// New variant: tankionline.com/battle-(*loc).html#/server=(*Loc+*N)

const tstring UrlManager::TankiOnlineServer(TEXT("tankionline.com/battle-"));

//-----------------------------------------------------------------------------

UrlManager::UrlManager(CONST TCHAR*CONST commandLineArgument)
        : frontPart(TEXT("http://")), middlePart(TEXT(".html#/server="))
{
    if (commandLineArgument[0] == '\0')
    {
        mySlot.assign(GetSettings()->account);
    }
    else
    {
        mySlot.assign(commandLineArgument);
    }
    mySlotPoint = mySlot + TEXT(".");

    frontPart.append(mySlot + TEXT(".") + TankiOnlineServer);

    accountCoockieName = mySlot + TEXT(".tankionline.com");
}

//-----------------------------------------------------------------------------

tstring
UrlManager::getServerLink(const unsigned serverNumber) const
{
    assert(serverNumber > 0);
    std::basic_ostringstream<tstring::value_type> out;
    out << frontPart << C2T(GetSettings()->language) << middlePart << TEXT("RU") << serverNumber;
    return out.str();
}

//-----------------------------------------------------------------------------

tstring
UrlManager::getAccountCoockieName() const
{
    return accountCoockieName;
}

//-----------------------------------------------------------------------------

tstring
UrlManager::toMyUrl(const tstring& publicUrl) const
{
    if (isValidUrl(publicUrl))
    {
        // Add "Account" part
        const std::string::size_type pos = publicUrl.find(mySlotPoint);
        if (pos == std::string::npos)
        {
            const std::string::size_type pos2 = publicUrl.find(TankiOnlineServer);
            assert(pos2 != std::string::npos);
            return setMyLanguage(tstring(publicUrl).insert(pos2, mySlotPoint)); // ref was not accepted
        }
    }
    return publicUrl;
}

//-----------------------------------------------------------------------------

tstring
UrlManager::toPublicUrl(const tstring& url) const
{
    if (isValidUrl(url))
    {
        // Remove "Account" part
        const std::string::size_type pos = url.find(mySlotPoint);
        if (pos != std::string::npos)
        {
            return setMyLanguage(tstring(url).erase(pos, mySlotPoint.length()));
        }
    }
    return url;
}

//-----------------------------------------------------------------------------

unsigned
UrlManager::extractServerNumber(const tstring& url)
{
    if (isValidUrl(url))
    {
        const std::string::size_type pos = url.find(TEXT("server="));
        assert(pos != std::string::npos);

        const TCHAR figures[] = TEXT("0123456789");
        const std::string::size_type begin = url.find_first_of(figures, pos);
        assert(begin != std::string::npos);

        const std::string::size_type end = url.find_first_not_of(figures, begin);
        tstring numbersPart;
        if (end == std::string::npos)
        {
            numbersPart.assign(url.begin() + begin, url.end());
        }
        else
        {
            numbersPart.assign(url.begin() + begin, url.begin() + end);
        }

        unsigned number;
        std::basic_istringstream<tstring::value_type> in(numbersPart);
        in >> number;
        return number;
    }
    return 0;
}

//-----------------------------------------------------------------------------

bool
UrlManager::isValidUrl(const tstring& url)
{
    // or: start from "http://"
    std::string::size_type pos = url.find(TEXT("http://")); // TODO: compare
    if (pos == 0)
    {
        // contains "tankionline.com/battle-"
        pos = url.find(TankiOnlineServer, 0);
        if (pos != std::string::npos)
        {
            // contains ".html"
            return bool(url.find(TEXT(".html"), pos) != std::string::npos);
        }
    }
    return false;
}

//-----------------------------------------------------------------------------

tstring
UrlManager::setMyLanguage(const tstring& url)
{
    if (isValidUrl(url))
    {
        const std::string::size_type pos = url.find(TankiOnlineServer);
        if (pos != std::string::npos)
        {
            const tstring newLanguage(C2T(GetSettings()->language));
            return tstring(url).replace(pos + TankiOnlineServer.length(), newLanguage.length(),
                    newLanguage);
        }
    }
    return url;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

SIZE
MinimumPlayerSize()
{
    CONST SIZE size = { 1000, 600 };
    return size;
}

//-----------------------------------------------------------------------------

unsigned GetNumberOfServers()
{
    return GetSettings()->serverButtonsCount;
}

//-----------------------------------------------------------------------------

::POINTS GetRightPlayButtonClickPoint(CONST ::SIZE& size)
{
    CONST ::POINTS point =
    { SHORT(size.cx - 80), SHORT(size.cy - 40) };
    return point;
}

//-----------------------------------------------------------------------------

::POINTS GetLeftPlayButtonClickPoint(CONST ::SIZE& size)
{
    CONST ::POINTS point =
    { SHORT(size.cx / 3 * 2 + 80), SHORT(size.cy - 40) };
    return point;
}

//-----------------------------------------------------------------------------

::RECT GetRightClickerWindowRect(CONST ::SIZE& size)
{
    const int width = min(118, 99 + (size.cx - 1000 + 4) / 6);
    const int right = ((size.cx / 3) * 3 + 1) - 18;
    const int top = size.cy - 55;
    const ::RECT rc =
    { right - width, top, right, top + 39 };
    return rc;
}

//-----------------------------------------------------------------------------

::RECT GetLeftClickerWindowRect(CONST ::SIZE& size)
{
    const int width = min(118, 99 + (size.cx - 1000 + 4) / 6);
    const int left = size.cx / 3 * 2 + 17;
    const int top = size.cy - 55;
    const ::RECT rc =
    { left, top, left + width, top + 39 };
    return rc;
}

//-----------------------------------------------------------------------------

}// namespace TO

//-----------------------------------------------------------------------------

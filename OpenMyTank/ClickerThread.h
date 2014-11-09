//-----------------------------------------------------------------------------

#pragma once

//-----------------------------------------------------------------------------

#include "Thread.h"
#include "Flash.h"

//-----------------------------------------------------------------------------

extern std::auto_ptr<Flash::PlayerWindow> FlashPlayer;
extern volatile bool BattleIsStarted;

//-----------------------------------------------------------------------------

class ClickerThread : public Thread
{
public:

    ClickerThread(const HWND parentHwnd)
            : Thread(parentHwnd), leftClicking(false), rightClicking(false)
    {
    }

    ~ClickerThread()
    {
    }

    void setLeftClicker(const bool enabled)
    {
        leftClicking = enabled;
    }

    void setRightClicker(const bool enabled)
    {
        rightClicking = enabled;
    }

    bool isBusy() const
    {
        return bool(leftClicking || rightClicking);
    }

    void run()
    {
        BattleIsStarted = false;

        while (runned && (FlashPlayer->getFlashHwnd() == NULL))
        {
            ::Sleep(50);
        }

        while (runned && !(leftClicking || rightClicking))
        {
            ::Sleep(50);
        }

        int i = 0;
        while (runned)
        {
            if (leftClicking)
            {
                FlashPlayer->clickLeftPlayButton();
            }
            if (rightClicking)
            {
                FlashPlayer->clickRightPlayButton();
            }

            if (i++ % 1000 == 0)
            {
                if (FlashPlayer->isBattleHappens())
                {
                    BattleIsStarted = true;
                    break;
                }
            }

            ::Sleep(GetSettings()->clickersDelay);
        }
    }

private:

    bool leftClicking;
    bool rightClicking;
};

//-----------------------------------------------------------------------------

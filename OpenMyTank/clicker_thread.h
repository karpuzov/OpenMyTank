
//-----------------------------------------------------------------------------

#pragma once

//-----------------------------------------------------------------------------

#include "thread.h"
#include "flash.h"

//-----------------------------------------------------------------------------

extern std::auto_ptr<Flash::PlayerWindow> FlashPlayer;
extern volatile bool BattleIsStarted;

//-----------------------------------------------------------------------------

class ClickerThread : public Thread
{
public:

  ClickerThread(const HWND parentHwnd)
    : Thread(parentHwnd)
    , LeftClicking(false)
    , RightClicking(false)
  {
  }

  ~ClickerThread()
  {
  }

  void SetLeftClicker(const bool enabled)
  {
    LeftClicking = enabled;
  }

  void SetRightClicker(const bool enabled)
  {
    RightClicking = enabled;
  }

  bool IsBusy() const
  {
    return bool(LeftClicking || RightClicking);
  }

  void Run()
  {
    BattleIsStarted = false;

    while (Runned && (FlashPlayer->GetFlashHwnd() == NULL))
    {
      ::Sleep(50);
    }

    while (Runned && !(LeftClicking || RightClicking))
    {
      ::Sleep(50);
    }

    int i = 0;
    while (Runned)
    {
      if (LeftClicking)
      {
        FlashPlayer->ClickLeftPlayButton();
      }
      if (RightClicking)
      {
        FlashPlayer->ClickRightPlayButton();
      }

      if (i++ % 1000 == 0)
      {
        if (FlashPlayer->IsBattleHappens())
        {
          BattleIsStarted = true;
          break;
        }
      }

      ::Sleep(GetSettings()->ClickersDelay);
    }
  }

private:

  bool LeftClicking;
  bool RightClicking;
};

//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------

#pragma once

//-----------------------------------------------------------------------------

#include <windows.h>

//-----------------------------------------------------------------------------

class Thread
{
public:

  enum { WM_THREADFINISH = WM_USER + 222 };

  Thread(const HWND parent = NULL);  // to this window will be sended WM_THREADFINISH
                                     // after returned from Run()
  virtual ~Thread();

  virtual void Run() = 0;

  void Start();

  void Exec();

  bool Exit(const DWORD msecs = INFINITE);

  DWORD GetThreadId() const;

protected:
  volatile bool Runned;

private:
  const HWND ParentHwnd;
  HANDLE ThreadHandle;
  DWORD  ThreadId;

  static unsigned __stdcall ThreadLoop(void* thisThread); // for modifying ThreadId value
};

//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------

#include "thread.h"

#include <process.h>

//-----------------------------------------------------------------------------

Thread::Thread(const HWND parent)
  : ParentHwnd(parent)
  , Runned(false)
  , ThreadId(0)
{
  ThreadHandle = (HANDLE)::_beginthreadex(NULL,
                                          0,
                                          Thread::ThreadLoop,
                                          static_cast<void*>(this),
                                          CREATE_SUSPENDED,
                                          reinterpret_cast<unsigned int*>(&ThreadId));
  if (ThreadHandle == 0)
  {
    //throw;
  }
}

//-----------------------------------------------------------------------------

Thread::~Thread()
{
  Exit();
  ::CloseHandle(ThreadHandle);
}

//-----------------------------------------------------------------------------

void
Thread::Start()
{
  if (!Runned)
  {
    Runned = true;
    ::ResumeThread(ThreadHandle);
  }
}

//-----------------------------------------------------------------------------

void
Thread::Exec()
{
  MSG msg;
  while (Runned && ::GetMessage(&msg, NULL, 0, 0))
  {
    ::TranslateMessage(&msg);
    ::DispatchMessage(&msg);
  }
}

//-----------------------------------------------------------------------------

bool
Thread::Exit(const DWORD msecs)
{
  if (Runned)
  {
    Runned = false;
    ::PostThreadMessage(this->GetThreadId(), WM_QUIT, 0, 0);
    return bool(::WaitForSingleObject(ThreadHandle, msecs) == WAIT_OBJECT_0);
  }
  return true;
}

//-----------------------------------------------------------------------------

DWORD
Thread::GetThreadId() const
{
  return ThreadId;
}

//-----------------------------------------------------------------------------

unsigned __stdcall
Thread::ThreadLoop(void* thisThread)
{
  //MSG msg;  PeekMessage(&msg, NULL, WM_USER, WM_USER, PM_NOREMOVE); - creating message loop?

  Thread*const threadObject = static_cast<Thread*>(thisThread);
  threadObject->Run();

  if (threadObject->ParentHwnd != NULL)
  {
    ::PostMessage(threadObject->ParentHwnd, Thread::WM_THREADFINISH, (WPARAM)(threadObject), (LPARAM)(threadObject->ThreadId));
  }

  ::_endthreadex(0);
  return 0;
}

//-----------------------------------------------------------------------------

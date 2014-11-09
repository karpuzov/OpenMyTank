//-----------------------------------------------------------------------------

#include "thread.h"

#include <process.h>

//-----------------------------------------------------------------------------

Thread::Thread(const HWND parent)
        : parentHwnd(parent), runned(false), threadId(0)
{
    threadHandle = (HANDLE)::_beginthreadex(NULL, 0, Thread::threadLoop, static_cast<void*>(this),
    CREATE_SUSPENDED, reinterpret_cast<unsigned int*>(&threadId));
    if (threadHandle == 0)
    {
        //throw;
    }
}

//-----------------------------------------------------------------------------

Thread::~Thread()
{
    exit();
    ::CloseHandle(threadHandle);
}

//-----------------------------------------------------------------------------

void
Thread::start()
{
    if (!runned)
    {
        runned = true;
        ::ResumeThread(threadHandle);
    }
}

//-----------------------------------------------------------------------------

void
Thread::exec()
{
    MSG msg;
    while (runned && ::GetMessage(&msg, NULL, 0, 0))
    {
        ::TranslateMessage(&msg);
        ::DispatchMessage(&msg);
    }
}

//-----------------------------------------------------------------------------

bool
Thread::exit(const DWORD msecs)
{
    if (runned)
    {
        runned = false;
        ::PostThreadMessage(this->getThreadId(), WM_QUIT, 0, 0);
        return bool(::WaitForSingleObject(threadHandle, msecs) == WAIT_OBJECT_0);
    }
    return true;
}

//-----------------------------------------------------------------------------

DWORD
Thread::getThreadId() const
{
    return threadId;
}

//-----------------------------------------------------------------------------

unsigned __stdcall
Thread::threadLoop(void* thisThread)
{
    //MSG msg;  PeekMessage(&msg, NULL, WM_USER, WM_USER, PM_NOREMOVE); - creating message loop?

    Thread* const threadObject = static_cast<Thread*>(thisThread);
    threadObject->run();

    if (threadObject->parentHwnd != NULL)
    {
        ::PostMessage(threadObject->parentHwnd, Thread::WM_THREADFINISH, (WPARAM)(threadObject), (LPARAM)(threadObject->threadId));
    }

    ::_endthreadex(0);
    return 0;
}

//-----------------------------------------------------------------------------

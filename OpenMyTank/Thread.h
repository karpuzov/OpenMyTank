//-----------------------------------------------------------------------------

#pragma once

//-----------------------------------------------------------------------------

#include <windows.h>

//-----------------------------------------------------------------------------

class Thread
{
public:

    enum
    {
        WM_THREADFINISH = WM_USER + 222
    };

    Thread(const HWND parent = NULL);  // to this window will be sended WM_THREADFINISH
                                       // after returned from Run()
    virtual ~Thread();

    virtual void run() = 0;

    void start();

    void exec();

    bool exit(const DWORD msecs = INFINITE);

    DWORD getThreadId() const;

protected:
    volatile bool runned;

private:
    const HWND parentHwnd;
    HANDLE threadHandle;
    DWORD threadId;

    static unsigned __stdcall threadLoop(void* thisThread); // for modifying ThreadId value
};

//-----------------------------------------------------------------------------

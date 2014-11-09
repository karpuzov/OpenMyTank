//-----------------------------------------------------------------------------

#pragma once

//-----------------------------------------------------------------------------

#include <atlbase.h>
#include <atlwin.h>
#include <comdef.h>
#include <exdispid.h> 

#include <string>
#include <new>
#include <cassert>

//-----------------------------------------------------------------------------

extern HWND MainWindow;

//-----------------------------------------------------------------------------

// See http://www.antillia.com/sol9.2.0/classes/BrowserEvent.html

class BrowserEvent : public IDispatch
{
public:

    enum
    {
        WM_BROWSEREVENT = WM_USER + 51
    };

    BrowserEvent()
    {
    }

    virtual ~BrowserEvent()
    {
    }

    STDMETHODIMP_(ULONG) AddRef()
    {
        return 1;
    }

    STDMETHODIMP_(ULONG) Release()
    {
        return 1;
    }

    STDMETHODIMP QueryInterface(REFIID riid, void** ppv)
    {
        if (riid == IID_IUnknown)
        {
            *ppv = (IUnknown*)this;
            return S_OK;
        }
        if (riid == IID_IDispatch)
        {
            *ppv = (IDispatch*)this;
            return S_OK;
        }
        *ppv = NULL;
        return E_NOINTERFACE;
    }

    STDMETHODIMP GetTypeInfoCount(UINT* pCountTypeInfo)
    {
        return S_OK;
    }

    STDMETHODIMP GetTypeInfo(UINT iTypeInfo, LCID lcid, ITypeInfo** ppITypeInfo)
    {
        return S_OK;
    }

    STDMETHODIMP GetIDsOfNames(REFIID riid,
            LPOLESTR* rgszNames,
            UINT cNames,
            LCID lcid,
            DISPID* rgDispId)
    {
        return S_OK;
    }

    STDMETHODIMP Invoke(DISPID dispidMember,
            REFIID riid, LCID lcid,
            WORD wFlags,
            DISPPARAMS* dispParams,
            VARIANT* pvarResult,
            EXCEPINFO* pExcepInfo,
            UINT* puArgErr)
    {
        if (dispParams == NULL)
        {
            return E_INVALIDARG;
        }

        switch (dispidMember)
        {
            case DISPID_BEFORENAVIGATE2:
            {
                const BSTR burl = dispParams->rgvarg[5].pvarVal->bstrVal;
                notifyWindow(dispidMember, burl);
                break;
            }
            case DISPID_NAVIGATECOMPLETE2:
            case DISPID_DOCUMENTCOMPLETE:
            {
                const BSTR burl = dispParams->rgvarg[0].pvarVal->bstrVal;
                notifyWindow(dispidMember, burl);
                break;
            }
            case DISPID_NAVIGATEERROR:
            {
                const BSTR burl = dispParams->rgvarg[3].pvarVal->bstrVal;
                notifyWindow(dispidMember, burl);
                break;
            }
        }
        return S_OK;
    }

private:

    void notifyWindow(const DISPID eventType, const BSTR url) const
    {
        ::PostMessage(MainWindow, WM_BROWSEREVENT, (WPARAM)eventType, (LPARAM)new tstring(url));
    }
};

//-----------------------------------------------------------------------------

class WebBrowser
{
public:

    WebBrowser(const HWND hwnd)
            : ActiveX(NULL)
    {
        IUnknown* unknown;
        if (S_OK == AtlAxGetControl(hwnd, &unknown))
        {
            if (S_OK == unknown->QueryInterface(__uuidof(IWebBrowser), (void**)&ActiveX))
            {
                enableEventsCather();
                return;
            }
        }
        throw TEXT("Cannot query IWebBrowser interface");
    }

    ~WebBrowser()
    {
        disableEventsCather();
        if (ActiveX != NULL)
        {
            ActiveX->Release();
        }
    }

    std::basic_string<TCHAR> getUrl() const
    {
        _bstr_t bstr;
        if (S_OK == ActiveX->get_LocationURL(bstr.GetAddress()))
        {
            return std::basic_string<TCHAR>(bstr);
        }
        return std::basic_string<TCHAR>();
    }

    //bool SetUrl(const std::basic_string<TCHAR>& url)
    //{
    //  _bstr_t bstr(url.c_str());
    //  return bool(S_OK == ActiveX->Navigate(bstr.GetBSTR(), NULL, NULL, NULL, NULL));
    //}

private:

    IWebBrowser* ActiveX;

    bool enableEventsCather()
    {
        return setEventsCatherMode(true);
    }

    bool disableEventsCather()
    {
        return setEventsCatherMode(false);
    }

    bool setEventsCatherMode(const bool on)
    {
        // See http://www.wasm.ru/forum/viewtopic.php?pid=396856
        // See http://www.antillia.com/sol9.2.0/classes/BrowserEvent.html

        static BrowserEvent* EventHandler = NULL;
        assert(on == bool(EventHandler == NULL));

        CComQIPtr<IConnectionPointContainer> connectionPointContainer(ActiveX);
        CComPtr<IConnectionPoint> connectionPoint;
        if (S_OK == connectionPointContainer->FindConnectionPoint(DIID_DWebBrowserEvents2, &connectionPoint))
        {
            static DWORD cookie;
            if (on)
            {
                EventHandler = new (std::nothrow) BrowserEvent;
                if (EventHandler != NULL)
                {
                    return bool(S_OK == connectionPoint->Advise(dynamic_cast<IDispatch*>(EventHandler), &cookie));
                }
            }
            else
            {
                const bool ok = bool(S_OK == connectionPoint->Unadvise(cookie));
                delete EventHandler;
                EventHandler = NULL;
                return ok;
            }
        }
        return false;
    }

};

//-----------------------------------------------------------------------------

#include <cstdio>
#include <new>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <lplugfwk.h>

#include "../app/events.h"

class CPlugin
{
public:
    void OnEvent(int const nEvent, unsigned long const ulData, void* const lpData)
    {
        switch(nEvent)
        {
        case PLUGINFWK_EVENT_STOP:
            puts("Plugin::OnStop");
            // do not forget to release and unregister interfaces
            delete this;
            break;
        case PLUGINFWK_EVENT_INTERFACE:
            {
                IPlugFwkUnknown* lpUnk = (IPlugFwkUnknown*)lpData;

                // if we wanted to keep the interface, we'd this:
                // lpUnk->Acquire(lpUnk, &lpCopy);

                IPlugFwkInterface* lpIf = NULL;

                // we are explicitly getting IPlugFwkInterface, since
                // the default interface may change in the future, for
                // example to IPlugFwkInterface2, so lpUnk must be
                // treated as IPlugFwkUnknown only.
                if(lpUnk->QueryInterface(lpUnk, &GUID_IPlugFwkInterface, (IPlugFwkUnknown**)&lpIf))
                {
                    // if we wanted to register our own interfaces:
                    // lpIf->RegisterInterface(&GUID_IPlugInterface, &lpPlugInterface);
                    lpIf->Release(&lpIf);
                }

                IAppInterface* lpAppIf = NULL;

                if(lpUnk->QueryInterface(lpUnk, &GUID_IAppInterface, (IPlugFwkUnknown**)&lpAppIf))
                {
                    lpAppIf->HelloApp(lpAppIf, "Hello world from plugin!");
                    lpAppIf->Release(&lpAppIf);
                }
            }
            break;
        case APP_EVENT_FOO:
            // Do Foo
            puts("Plugin::OnFoo");
            break;
        case APP_EVENT_BAR:
            // Do Bar
            puts("Plugin::OnBar");
            break;
        }
    }
};

bool __stdcall PlugFwkOnEvent(int const nEvent, unsigned long const ulData, void* const lpData, void* const lpContext)
{
    if(nEvent==PLUGINFWK_EVENT_START)
    {
        CPlugin* lpThis = new CPlugin;
        void** lppOut = (void**)lpData;

        puts("Plugin::OnStart");

        lppOut[0] = lpThis;
        return lpThis!=NULL;
    }

    CPlugin* const lpThis = (CPlugin*)lpContext;
    lpThis->OnEvent(nEvent, ulData, lpData);
    return false;
}

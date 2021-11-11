#include <cstdio>
#include <new>

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
            delete this;
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

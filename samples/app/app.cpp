#include <cstdio>
#include <cstdlib>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <lplugfwk.h>

#include "events.h"

static void __stdcall AppIAcquire(IAppInterface* const lpThis, IAppInterface** const lppOut);
static void __stdcall AppIRelease(IAppInterface** const lppThis);
static bool __stdcall AppIQueryInterface(IAppInterface* const lpThis, GUID const* const lpGuid, IPlugFwkUnknown** const lppOut);
static void __stdcall AppIHelloApp(IAppInterface* const lpThis, char const* const lpszText);

static IAppInterface const l_AppIfTemplate =
{
    /* IPlugFwkUnknown */
    &AppIAcquire,
    &AppIRelease,
    &AppIQueryInterface,
    /* IPlugFwkAppIf */
    &AppIHelloApp,
};

static void __stdcall AppIAcquire(IAppInterface* const lpThis, IAppInterface** const lppOut)
{
    // We do not carry any dynamic data, so do not care about
    // reference-counting for this sample. In a real application,
    // all interfaces should be reference-counted and dynamically
    // allocated, as presented in the framework.
    lppOut[0] = lpThis;
}

static void __stdcall AppIRelease(IAppInterface** const lppThis)
{
    lppThis[0] = NULL;
}

static bool __stdcall AppIQueryInterface(IAppInterface* const lpThis, GUID const* const lpGuid, IPlugFwkUnknown** const lppOut)
{
    if(IsEqualGUID(lpGuid[0], GUID_IAppInterface))
    {
        lppOut[0] = (IPlugFwkUnknown*)&l_AppIfTemplate;
        return true;
    }

    lppOut[0] = NULL;
    return false;
}

static void __stdcall AppIHelloApp(IAppInterface* const lpThis, char const* const lpszText)
{
    printf("Hello from Plug-in to App: %s\n", lpszText);
}

int __cdecl main(int nArgc, char** lppszArgv)
{
    PluginFwkRegisterInterface(&GUID_IAppInterface, (IPlugFwkUnknown*)&l_AppIfTemplate);

    if(PluginFwkInit("..\\plugin"))
    {
        // do something
        puts("Do something...");

        // notify of foo
        PluginFwkEvent(APP_EVENT_FOO, 0, NULL);

        // do some more thing
        puts("Do some more thing...");

        // notify of bar
        PluginFwkEvent(APP_EVENT_BAR, 0, NULL);

        // finish up
        puts("Finishing...");

        // unload everything
        PluginFwkQuit();
    }

    PluginFwkUnregisterInterface(&GUID_IAppInterface);
    return 0;
}

#include <cstdio>
#include <cstdlib>

#include <lplugfwk.h>

#include "events.h"

int __cdecl main(int nArgc, char** lppszArgv)
{
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

    return 0;
}

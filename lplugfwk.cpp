#include <cstring>
#include <map>
#include <string>
#include <vector>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "lplugfwk.h"

typedef bool (__stdcall* LPFNPLUGINFWKONEVENT)
(
    int const nEvent,
    unsigned long const ulData,
    void* const lpData,
    void* const lpContext
);
struct PLUGININFO
{
    HMODULE hDll;
    LPFNPLUGINFWKONEVENT lpfnOnEvent;
    void* lpContext;
};
typedef std::vector< PLUGININFO > PLUGINFOVEC;
typedef PLUGINFOVEC::const_iterator PLUGINFOITER;

struct INTERFACESTATE
{
    unsigned long ulLocks;
};
class LessGuid
{
public:
    bool operator()(GUID const& guidOne, GUID const& guidTwo) const
    {
        return memcmp(&guidOne, &guidTwo, sizeof(guidOne))<0;
    }
};
typedef std::map< GUID, IPlugFwkUnknown*, LessGuid > INTERFACEINFOMAP;
typedef std::pair< GUID, IPlugFwkUnknown* > INTERFACEINFOPAIR;

static PLUGINFOVEC l_aPlugInfoVec;
static INTERFACEINFOMAP l_aInterfaceInfoMap;

static void __stdcall PluginFwkIAcquire(IPlugFwkUnknown* const lpThis, IPlugFwkUnknown** const lppOut);
static void __stdcall PluginFwkIRelease(IPlugFwkUnknown** const lpThis);
static bool __stdcall PluginFwkIQueryInterface(IPlugFwkUnknown* const lpThis, GUID const* const lpGuid, IPlugFwkUnknown** const lppOut);
static bool __stdcall PluginFwkIUnregisterInterface(IPlugFwkInterface* const lpThis, GUID const* const lpGuid);
static bool __stdcall PluginFwkIRegisterInterface(IPlugFwkInterface* const lpThis, GUID const* const lpGuid, IPlugFwkUnknown* const lpInterface);

static IPlugFwkUnknown const l_UnknownTemplate =
{
    /* IPlugFwkUnknown */
    &PluginFwkIAcquire,
    &PluginFwkIRelease,
    &PluginFwkIQueryInterface,
};

static IPlugFwkInterface const l_InterfaceTemplate =
{
    /* IPlugFwkUnknown */
    (void (__stdcall*)(IPlugFwkInterface* const, IPlugFwkInterface** const))&PluginFwkIAcquire,
    (void (__stdcall*)(IPlugFwkInterface** const))&PluginFwkIRelease,
    (bool (__stdcall*)(IPlugFwkInterface* const, GUID const* const, IPlugFwkUnknown** const))&PluginFwkIQueryInterface,
    /* IPlugFwkInterface */
    &PluginFwkIUnregisterInterface,
    &PluginFwkIRegisterInterface,
};

static INTERFACESTATE* PluginFwkPGetState(IPlugFwkUnknown* const lpThis)
{
    return &((INTERFACESTATE*)lpThis)[-1];
}

static void __stdcall PluginFwkIAcquire(IPlugFwkUnknown* const lpThis, IPlugFwkUnknown** const lppOut)
{
    INTERFACESTATE* const lpState = PluginFwkPGetState(lpThis);

    InterlockedIncrement((LONG*)&lpState->ulLocks);
    lppOut[0] = lpThis;
}

static void __stdcall PluginFwkIRelease(IPlugFwkUnknown** const lppThis)
{
    IPlugFwkUnknown* const lpThis = lppThis[0];
    INTERFACESTATE* const lpState = PluginFwkPGetState(lpThis);

    lppThis[0] = NULL;

    if(!InterlockedDecrement((LONG*)&lpState->ulLocks))
    {
        free(lpThis);
    }
}

static bool __stdcall PluginFwkIQueryInterface(IPlugFwkUnknown* const lpThis, GUID const* const lpGuid, IPlugFwkUnknown** const lppOut)
{
    INTERFACEINFOMAP::const_iterator const itIf = l_aInterfaceInfoMap.find(lpGuid[0]);

    if(itIf==l_aInterfaceInfoMap.end())
    {
        return false;
    }

    IPlugFwkUnknown* const lpIf = itIf->second;

    lpIf->Acquire(lpIf, lppOut);
    return true;
}

static bool __stdcall PluginFwkIUnregisterInterface(IPlugFwkInterface* const lpThis, GUID const* const lpGuid)
{
    return PluginFwkUnregisterInterface(lpGuid);
}

static bool __stdcall PluginFwkIRegisterInterface(IPlugFwkInterface* const lpThis, GUID const* const lpGuid, IPlugFwkUnknown* const lpInterface)
{
    return PluginFwkRegisterInterface(lpGuid, lpInterface);
}

static bool PluginFwkPLoad(char const* const lpszPluginFileName)
{
    PLUGININFO Pi = { 0 };

    Pi.hDll = LoadLibraryA(lpszPluginFileName);

    if(Pi.hDll!=NULL)
    {
        Pi.lpfnOnEvent = (LPFNPLUGINFWKONEVENT)GetProcAddress(Pi.hDll, "PlugFwkOnEvent");

        if(Pi.lpfnOnEvent!=NULL)
        {
            if(Pi.lpfnOnEvent(PLUGINFWK_EVENT_START, 0, &Pi.lpContext, NULL))
            {
                l_aPlugInfoVec.push_back(Pi);
                return true;
            }
        }

        FreeLibrary(Pi.hDll);
    }

    return false;
}

bool PluginFwkInit(char const* const lpszPluginDirectory)
{
    bool bSuccess = false;
    std::string const sPluginDirectory = lpszPluginDirectory;
    WIN32_FIND_DATAA Wfd = { 0 };
    HANDLE hFind = FindFirstFileA((sPluginDirectory + "\\*.plugin").c_str(), &Wfd);

    if(hFind!=INVALID_HANDLE_VALUE)
    {
        do
        {
            if(!PluginFwkPLoad((sPluginDirectory + "\\" + Wfd.cFileName).c_str()))
            {
                break;
            }
        }
        while(FindNextFile(hFind, &Wfd));

        if(GetLastError()==ERROR_NO_MORE_FILES)
        {
            bSuccess = true;
        }

        FindClose(hFind);
    }
    else
    if(GetLastError()==ERROR_FILE_NOT_FOUND)
    {
        bSuccess = true;
    }

    if(bSuccess)
    {
        // C-style class
        INTERFACESTATE* const lpState = static_cast< INTERFACESTATE* >(malloc(sizeof(INTERFACESTATE)+sizeof(IPlugFwkInterface)));
        IPlugFwkInterface* lpInterface = (IPlugFwkInterface*)&lpState[1];

        // manual construction
        lpState->ulLocks = 1;
        memcpy(lpInterface, &l_InterfaceTemplate, sizeof(l_InterfaceTemplate));

        // register as IPlugFwkInterface to allow proper QueryInterface
        PluginFwkRegisterInterface(&GUID_IPlugFwkInterface, (IPlugFwkUnknown*)lpInterface);

        // distribute to all plug-ins
        PluginFwkEvent(PLUGINFWK_EVENT_INTERFACE, 0, lpInterface);

        // drop our reference
        lpInterface->Release(&lpInterface);
    }
    else
    {
        PluginFwkQuit();
    }

    return bSuccess;
}

void PluginFwkEvent(int const nEvent, unsigned long const ulData, void* const lpData)
{
    for(PLUGINFOITER itPi = l_aPlugInfoVec.begin(); itPi!=l_aPlugInfoVec.end(); itPi++)
    {
        itPi->lpfnOnEvent(nEvent, ulData, lpData, itPi->lpContext);
    }
}

bool PluginFwkUnregisterInterface(GUID const* const lpGuid)
{
    INTERFACEINFOMAP::iterator const itIf = l_aInterfaceInfoMap.find(lpGuid[0]);

    if(itIf==l_aInterfaceInfoMap.end())
    {
        return false;
    }

    IPlugFwkUnknown* lpIf = itIf->second;

    lpIf->Release(&lpIf);  // release reference
    l_aInterfaceInfoMap.erase(itIf);
    return true;
}

bool PluginFwkRegisterInterface(GUID const* const lpGuid, IPlugFwkUnknown* const lpInterface)
{
    INTERFACEINFOMAP::const_iterator const itIf = l_aInterfaceInfoMap.find(lpGuid[0]);

    if(itIf!=l_aInterfaceInfoMap.end())
    {
        return false;
    }

    IPlugFwkUnknown* lpCopy = NULL;

    lpInterface->Acquire(lpInterface, &lpCopy);  // keep a reference while this interface is registered
    l_aInterfaceInfoMap.insert(INTERFACEINFOPAIR(lpGuid[0], lpCopy));
    return true;
}

void PluginFwkQuit(void)
{
    PluginFwkEvent(PLUGINFWK_EVENT_STOP, 0, NULL);

    for(PLUGINFOITER itPi = l_aPlugInfoVec.begin(); itPi!=l_aPlugInfoVec.end(); itPi++)
    {
        FreeLibrary(itPi->hDll);
    }
    l_aPlugInfoVec.clear();

    PluginFwkUnregisterInterface(&GUID_IPlugFwkInterface);
}

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

static PLUGINFOVEC l_aPlugInfoVec;

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

    if(!bSuccess)
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

void PluginFwkQuit(void)
{
    PluginFwkEvent(PLUGINFWK_EVENT_STOP, 0, NULL);

    for(PLUGINFOITER itPi = l_aPlugInfoVec.begin(); itPi!=l_aPlugInfoVec.end(); itPi++)
    {
        FreeLibrary(itPi->hDll);
    }
    l_aPlugInfoVec.clear();
}

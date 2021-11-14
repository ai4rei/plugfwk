#ifndef LPLUGFWK_H
#define LPLUGFWK_H

#ifdef __cplusplus
extern "C"
{
#endif  /* __cplusplus */

enum PLUGINFWK_EVENT
{
    /**
        @brief  Received before unloading the plugin.
        @param  ulData
                Unused.
        @param  lpData
                Unused.
        @return Ignored.
        @note   Any registered interfaces should be unregistered,
                and any acquired interfaces should be released at
                this point.
        @note   This event shall not be posted by the application.
    */
    PLUGINFWK_EVENT_STOP,

    /**
        @brief  Received after loading the plugin.
        @param  ulData
                Unused.
        @param  lpData
                Pointer to a void* variable that will receive the
                plugin's context data to be attached to every event.
        @return true
                Plugin will stay loaded and receive events.
        @return false
                Plugin will be immediately unloaded (no STOP event).
        @note   This event shall not be posted by the application.
    */
    PLUGINFWK_EVENT_START,

    /**
        @brief  Provides the plug-in with an interface to query
                interfaces of the application or other plug-ins.
        @param  ulData
                Unused.
        @param  lpData
                Pointer to a IPlugFwkUnknown interface. Use Acquire
                if you want to keep a reference to this interface,
                after the event handler returns. After you are done
                with using the interface, call Release.
        @return Ignored.
        @note   This is a good place to register your plug-in's
                interfaces.
        @note   This event shall not be posted by the application.
    */
    PLUGINFWK_EVENT_INTERFACE,

    /**
        @brief  Custom events defined by the application start here.
    */
    PLUGINFWK_EVENT_CUSTOM_BEGIN = 100,
};

typedef struct IPlugFwkUnknown IPlugFwkUnknown;
struct IPlugFwkUnknown
{
    /* IPlugFwkUnknown */
    void (__stdcall* Acquire)(IPlugFwkUnknown* const lpThis, IPlugFwkUnknown** const lppOut);
    void (__stdcall* Release)(IPlugFwkUnknown** const lppThis);
    bool (__stdcall* QueryInterface)(IPlugFwkUnknown* const lpThis, GUID const* const lpGuid, IPlugFwkUnknown** const lppOut);
};

typedef struct IPlugFwkInterface IPlugFwkInterface;
struct IPlugFwkInterface/* : public IPlugFwkUnknown */
{
    /* IPlugFwkUnknown */
    void (__stdcall* Acquire)(IPlugFwkInterface* const lpThis, IPlugFwkInterface** const lppOut);
    void (__stdcall* Release)(IPlugFwkInterface** const lppThis);
    bool (__stdcall* QueryInterface)(IPlugFwkInterface* const lpThis, GUID const* const lpGuid, IPlugFwkUnknown** const lppOut);
    /* IPlugFwkInterface */
    bool (__stdcall* UnregisterInterface)(IPlugFwkInterface* const lpThis, GUID const* const lpGuid);
    bool (__stdcall* RegisterInterface)(IPlugFwkInterface* const lpThis, GUID const* const lpGuid, IPlugFwkUnknown* const lpInterface);
};

// {CA067661-4589-11EC-A78C-74DA38C22B45}
static GUID const GUID_IPlugFwkInterface =
{ 0xCA067661, 0x4589, 0x11EC, { 0xA7, 0x8C, 0x74, 0xDA, 0x38, 0xC2, 0x2B, 0x45 } };

/**
    @brief  Prototype of the OnEvent function to be exported by the
            plugin.
*/
bool __stdcall PlugFwkOnEvent(int const nEvent, unsigned long const ulData, void* const lpData, void* const lpContext);

/**
    @brief  Loads and initializes all plugins.
    @param  lpszPluginDirectory
            Path in which plugins are searched.
    @return Whether all plugins loaded successfully.
*/
bool PluginFwkInit(char const* const lpszPluginDirectory);

/**
    @brief  Dispatches an event to all plugins.
    @param  nEvent
            Application-defined event id starting at
            PLUGINFWK_EVENT_CUSTOM_BEGIN.
    @param  ulData
            Optional integer value to pass with the event.
    @param  lpData
            Optional pointer value to pass with the event.
*/
void PluginFwkEvent(int const nEvent, unsigned long const ulData, void* const lpData);

/**
    @brief  Allows the application to unregister an interface.
    @param  lpGuid
            GUID of the interface to unregister.
    @return Whether or not an interface was unregistered.
    @note   This interface intentionally allows removal if any
            interface, not just the application's own interfaces, for
            example for removing incompatible interfaces of old
            plug-ins.
*/
bool PluginFwkUnregisterInterface(GUID const* const lpGuid);

/**
    @brief  Allows the application to register an interface for
            plug-ins to use.
    @param  lpGuid
            GUID of the interface to register. This GUID must be
            unique for every interface and for every version of said
            interface.
    @param  lpInterface
            Pointer to an interface to register, that inherits from
            IPlugFwkUnknown (in C terms: implements methods of
            IPlugFwkUnknown before their own).
    @note   Once interface is assigned a GUID, it shall not change.
            Any change should cause a new version of that interface
            to be made and its GUID regenerated.
*/
bool PluginFwkRegisterInterface(GUID const* const lpGuid, IPlugFwkUnknown* const lpInterface);

/**
    @brief  Uninitializes and unloads all plugins.
*/
void PluginFwkQuit(void);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif  /* LPLUGFWK_H */

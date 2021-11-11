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
        @brief  Custom events defined by the application start here.
    */
    PLUGINFWK_EVENT_CUSTOM_BEGIN,
};

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
    @brief  Uninitializes and unloads all plugins.
*/
void PluginFwkQuit(void);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif  /* LPLUGFWK_H */

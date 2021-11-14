#ifndef APP_EVENTS_H
#define APP_EVENTS_H

#include <lplugfwk.h>

#ifdef __cplusplus
extern "C"
{
#endif  /* __cplusplus */

#define APP_EVENT_FOO (PLUGINFWK_EVENT_CUSTOM_BEGIN+0)
#define APP_EVENT_BAR (PLUGINFWK_EVENT_CUSTOM_BEGIN+1)

typedef struct IAppInterface IAppInterface;
struct IAppInterface/* : public IPlugFwkUnknown */
{
    /* IPlugFwkUnknown */
    void (__stdcall* Acquire)(IAppInterface* const lpThis, IAppInterface** const lppOut);
    void (__stdcall* Release)(IAppInterface** const lppThis);
    bool (__stdcall* QueryInterface)(IAppInterface* const lpThis, GUID const* const lpGuid, IPlugFwkUnknown** const lppOut);
    /* IAppInterface */
    void (__stdcall* HelloApp)(IAppInterface* const lpThis, char const* const lpszText);
};
// {CA067660-4589-11EC-A78C-74DA38C22B45}
static GUID const GUID_IAppInterface =
{ 0xCA067660, 0x4589, 0x11EC, { 0xA7, 0x8C, 0x74, 0xDA, 0x38, 0xC2, 0x2B, 0x45 } };

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif  /* APP_EVENTS_H */

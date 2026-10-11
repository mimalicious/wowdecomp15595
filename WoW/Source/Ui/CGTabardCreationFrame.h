#ifndef WOW_SOURCE_UI_CGTABARDCREATIONFRAME_H
#define WOW_SOURCE_UI_CGTABARDCREATIONFRAME_H

#include <cstdint>
#include "WoW/Source/Object/WGUID.h"

struct lua_State;

void TabardCreationRegisterScriptFunctions();
void TabardCreationUnregisterScriptFunctions();

class CGTabardCreationFrame {
    public:
    static void Close();
    static void LeaveWorld();
    static void Open(const WOWGUID& vendor);
    static void EnterWorld();
    static WOWGUID m_vendor;
};

uint32_t GuildGetTabardCost();
bool GuildGetGuildTabard(WOWGUID requester, WOWGUID guild, void (*callback)(const uint64_t& key, const WOWGUID& guid, void* param, bool success), void* param, int32_t& emblemStyle, int32_t& emblemColor, int32_t& borderStyle, int32_t& borderColor, int32_t& background);

#endif

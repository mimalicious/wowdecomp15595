#ifndef WOW_SOURCE_UI_CONTAINERFRAME_H
#define WOW_SOURCE_UI_CONTAINERFRAME_H

#include <cstdint>
#include "WoW/Source/Object/WGUID.h"

struct lua_State;

class CGBag_C;
class CGItem_C;

class CGContainerInfo {
    public:
    static WOWGUID m_containers[11];
    static char* m_searchText;
    static char* m_searchTextParsed;

    static void UpdateCooldowns();
    static WOWGUID GetContainer(int32_t index);
    static void SetItemSearch(const char* search);
    static void UpdateContents(WOWGUID guid);
    static void OpenContainer(WOWGUID guid);
    static void UpdateItem(WOWGUID guid);
    static bool GetLuaBagAndSlot(lua_State* L, CGBag_C*& bag, int32_t& slot, int32_t& bank);
    static void UpdateContainers();
    static void EnterWorld();
    static void LeaveWorld();
    static bool IsItemFiltered(const CGItem_C* item);
};

void ContainerRegisterScriptFunctions();
void ContainerUnregisterScriptFunctions();

int32_t InvUpdateHandler(WOWGUID guid, uint32_t fieldOffset, uint32_t fieldSize, const void* oldValue, void* param);
int32_t UpdateInvContents(WOWGUID guid, uint32_t fieldOffset, uint32_t fieldSize, const void* oldValue, void* param);
int32_t UpdateContainerContents(WOWGUID guid, uint32_t fieldOffset, uint32_t fieldSize, const void* oldValue, void* param);

#endif

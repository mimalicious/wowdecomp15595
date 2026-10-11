#include "ItemTextFrame.h"
#include <cstdint>
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"
#include "Engine/Source/lua-5.1/src/lua.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/Object/ObjectClient/Object_C.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/DBClient/DBCacheInstances.h"
#include "WoW/Source/DB/Rec/PageTextMaterialRec.h"
#include "WoW/Source/Game/GameClient/NameCache.h"
#include "WoW/Source/Object/Item.h"
#include "WoW/Source/Object/ItemStats.h"
#include "WoW/Source/Object/ObjectClient/GameObject_C.h"
#include "WoW/Source/Object/ObjectClient/Item_C.h"

// OFFSET: 0x009DDF20/0x140773FF0
static int32_t Script_ItemTextGetPage(lua_State* L) {
    lua_pushnumber(L, CGItemText::m_currentPage + 1);
    return 1;
}

// OFFSET: 0x009DDFD0/0x1407740D0
static int32_t Script_ItemTextGetItem(lua_State* L) {
    const char* name = NULL;
    CGObject_C* object = ClntObjMgrObjectPtr(CGItemText::m_itemGUID, TYPEMASK_OBJECT, __FILE__, __LINE__);

    if (object) {
        name = object->GetObjectName();
    }

    lua_pushstring(L, name);
    return 1;
}

// OFFSET: 0x009DDF60/0x140774020
static int32_t Script_ItemTextGetText(lua_State* L) {
    lua_pushstring(L, CGItemText::m_text);
    return 1;
}

// PROVEN: ed2b2a8b IDENTICAL/NONE
// OFFSET: 0x009DE020/0x140774130
static int32_t Script_ItemTextHasNextPage(lua_State* L) {
    if (CGItemText::m_hasItemText || !CGItemText::m_pages.m_data[CGItemText::m_currentPage + 1]) {
        lua_pushnil(L);
        return 1;
    }

    lua_pushnumber(L, 1.0);
    return 1;
}

// OFFSET: 0x009DE830/0x140774A20
static int32_t Script_ItemTextGetCreator(lua_State* L) {
    CGItem_C* item = static_cast<CGItem_C*>(ClntObjMgrObjectPtr(CGItemText::m_itemGUID, TYPEMASK_ITEM, __FILE__, __LINE__));
    NameCache* name;

    if (item && item->m_item->m_itemFieldCreator && (name = g_nameDBCache.GetRecord(item->m_item->m_itemFieldCreator.m_guid, item->m_obj->m_guid, CGItemText::ItemTextCallback, NULL, false))) {
        lua_pushstring(L, name->m_name);
        return 1;
    }

    lua_pushnil(L);
    return 1;
}

// OFFSET: 0x009DE3E0/0x140774590
static int32_t Script_ItemTextGetMaterial(lua_State* L) {
    CGObject_C* object = ClntObjMgrObjectPtr(CGItemText::m_itemGUID, TYPEMASK_OBJECT, __FILE__, __LINE__);

    if (object) {
        int32_t material;

        if (object->m_obj->m_type & TYPEMASK_ITEM) {
            WOWGUID none = WOWGUID();
            ItemStats_C* stats = g_itemDBCache.GetRecord(object->m_obj->m_entryID, none, NULL, NULL, false);

            if (!stats) {
                lua_pushnil(L);
                return 1;
            }

            material = stats->m_pageMaterial;
        } else if (object->m_obj->m_type & TYPEMASK_GAMEOBJECT) {
            material = static_cast<CGGameObject_C*>(object)->GetPageTextMaterial();
        } else {
            lua_pushnil(L);
            return 1;
        }

        if (material > 0) {
            PageTextMaterialRec* rec = g_pageTextMaterialDB.GetRecord(material);

            if (rec) {
                lua_pushstring(L, rec->m_name);
                return 1;
            }
        }
    }

    lua_pushnil(L);
    return 1;
}

// OFFSET: 0x009DE4C0/0x140774680
static int32_t Script_ItemTextPrevPage(lua_State* L) {
    if (CGItemText::m_itemGUID && !CGItemText::m_hasItemText && CGItemText::m_currentPage) {
        CGItemText::m_currentPage--;
        CGItemText::DisplayText(CGItemText::m_itemGUID, 1);
    }

    return 0;
}

// PROVEN: bc4af6ca IDENTICAL/NONE
// OFFSET: 0x009DE500/0x1407746D0
static int32_t Script_ItemTextNextPage(lua_State* L) {
    if (CGItemText::m_itemGUID && !CGItemText::m_hasItemText && CGItemText::m_pages.m_data[CGItemText::m_currentPage + 1] > 0) {
        CGItemText::m_currentPage++;
        CGItemText::DisplayText(CGItemText::m_itemGUID, 1);
    }

    return 0;
}

// PROVEN: 2de55c44 IDENTICAL/NONE
// OFFSET: 0x009DE7B0/0x1407749E0
static int32_t Script_CloseItemText(lua_State* L) {
    WOWGUID none = WOWGUID();
    CGItemText::SetItem(none, 0);
    return 0;
}

// STATIC: 0x00D165C0
static FrameScript_Method s_ScriptFunctions[] = {
    { "ItemTextGetItem", &Script_ItemTextGetItem },
    { "ItemTextGetCreator", &Script_ItemTextGetCreator },
    { "ItemTextGetMaterial", &Script_ItemTextGetMaterial },
    { "ItemTextGetPage", &Script_ItemTextGetPage },
    { "ItemTextGetText", &Script_ItemTextGetText },
    { "ItemTextHasNextPage", &Script_ItemTextHasNextPage },
    { "ItemTextPrevPage", &Script_ItemTextPrevPage },
    { "ItemTextNextPage", &Script_ItemTextNextPage },
    { "CloseItemText", &Script_CloseItemText },
};

// PROVEN: 3ce033f1 IDENTICAL/NONE
// OFFSET: 0x009DDF80/0x140774040
void ItemTextRegisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_RegisterFunction(s_ScriptFunctions[i].m_name, s_ScriptFunctions[i].m_method);
    }
}

// PROVEN: 566df76a IDENTICAL/NONE
// OFFSET: 0x009DDFB0/0x140774090
void ItemTextUnregisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_UnregisterFunction(s_ScriptFunctions[i].m_name);
    }
}

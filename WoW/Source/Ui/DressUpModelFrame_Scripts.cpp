#include "DressUpModelFrame.h"
#include <cstdint>
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"
#include "Engine/Source/lua-5.1/src/lua.h"
#include "Engine/Source/Storm/H/storm.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/SpellRec.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/UIUtil/Tooltip.h"
#include "WoW/Source/Ui/GameUI.h"

// OFFSET: #0x00597CE0
void CGDressUpModelFrame::DestroyScriptMetaTable() {
    lua_State* L = FrameScript_GetContext();
    luaL_unref(L, LUA_REGISTRYINDEX, CGDressUpModelFrame::s_metatable);
    CGDressUpModelFrame::s_metatable = LUA_NOREF;
}

// OFFSET: #0x00597F20
void CGDressUpModelFrame::CreateScriptMetaTable() {
    lua_State* L = FrameScript_GetContext();
    CGDressUpModelFrame::s_metatable = FrameScript_Object::CreateScriptMetaTable(L, &CGDressUpModelFrame::RegisterScriptMethods);
}

// OFFSET: 0x00A04D20/0x1407A4440
static int32_t Script_Undress(lua_State* L) {
    CGDressUpModelFrame* frame = static_cast<CGDressUpModelFrame*>(FrameScript_GetObjectThis(L, CGDressUpModelFrame::GetObjectType()));
    frame->Undress();
    return 0;
}

// OFFSET: 0x00A04D60/0x1407A4480
static int32_t Script_Dress(lua_State* L) {
    CGDressUpModelFrame* frame = static_cast<CGDressUpModelFrame*>(FrameScript_GetObjectThis(L, CGDressUpModelFrame::GetObjectType()));
    frame->RefreshUnit(frame->m_unitGuid);
    return 0;
}

// OFFSET: 0x00A05880/0x1407A5260
static int32_t Script_TryOn(lua_State* L) {
    CGDressUpModelFrame* frame = static_cast<CGDressUpModelFrame*>(FrameScript_GetObjectThis(L, CGDressUpModelFrame::GetObjectType()));

    if (lua_isnumber(L, 2)) {
        frame->TryOn(static_cast<int32_t>(lua_tonumber(L, 2)), NULL, -1);
        return 0;
    }

    if (lua_isstring(L, 2)) {
        const char* link = lua_tolstring(L, 2, NULL);

        if (SStrStr(link, "item:")) {
            ItemLinkInfo info;
            info.Clear();
            info.GetItemIdFromLink(link);
            frame->TryOn(info.m_itemId, info.m_enchants, -1);
            return 0;
        }

        if (SStrStr(link, "enchant:")) {
            const char* enchant = SStrStr(link, "enchant:");

            if (enchant) {
                int32_t spellId = SStrToInt(enchant + 8);

                if (spellId > 0) {
                    SpellRec spell;

                    if (g_spellDB.GetRecord(spellId, &spell) && (spell.m_attributes & 0x20) && spell.m_effectItemType[0]) {
                        frame->TryOn(spell.m_effectItemType[0], NULL, -1);
                        return 0;
                    }
                }
            }
        } else {
            CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();
            CGItem_C* item = player->m_bag.FindItemByFullName(lua_tolstring(L, 2, NULL), 0);

            if (item) {
                frame->TryOnItem(item, -1);
                return 0;
            }
        }
    }

    CGGameUI::DisplayError(ERR_NOT_EQUIPPABLE);
    return 0;
}

// STATIC: #0x00ACF550
static FrameScript_Method s_ScriptMethods[] = {
    { "Undress", &Script_Undress },
    { "Dress", &Script_Dress },
    { "TryOn", &Script_TryOn },
};

// OFFSET: %0x00945150
void CGDressUpModelFrame::RegisterScriptMethods(lua_State* L) {
    CGCharacterModelBase::RegisterScriptMethods(L);
    FrameScript_Object::FillScriptMethodTable(L, s_ScriptMethods, sizeof(s_ScriptMethods) / sizeof(s_ScriptMethods[0]));
}

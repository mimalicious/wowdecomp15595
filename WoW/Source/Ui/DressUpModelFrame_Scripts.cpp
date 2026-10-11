#include "DressUpModelFrame.h"
#include <cstdint>
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"
#include "Engine/Source/lua-5.1/src/lua.h"
#include "Engine/Source/Storm/H/storm.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/SpellEffectRec.h"
#include "WoW/Source/DB/Rec/SpellRec.h"
#include "WoW/Source/Object/ObjectClient/Item_C.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/UIUtil/Tooltip.h"
#include "WoW/Source/Ui/GameUI.h"

// PROVEN: 2fa1cfe7 IDENTICAL/NONE
// OFFSET: 0x00A04B60/0x1407A4200
void CGDressUpModelFrame::DestroyScriptMetaTable() {
    lua_State* L = FrameScript_GetContext();
    luaL_unref(L, LUA_REGISTRYINDEX, CGDressUpModelFrame::s_metatable);
    CGDressUpModelFrame::s_metatable = LUA_NOREF;
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

    if (frame->m_unitGuid) {
        frame->RefreshUnit(frame->m_unitGuid);
    }

    if (frame->m_model) {
        frame->ConfigureCamera();
    }

    return 0;
}

// OFFSET: 0x00A04DA0/0x1407A44C0
static int32_t Script_UndressSlot(lua_State* L) {
    CGDressUpModelFrame* frame = static_cast<CGDressUpModelFrame*>(FrameScript_GetObjectThis(L, CGDressUpModelFrame::GetObjectType()));

    if (!lua_isnumber(L, 2)) {
        const char* name = frame->GetName();

        if (!name) {
            name = "<unnamed>";
        }

        luaL_error(L, "Usage: %s:UndressSlot(slotIndex)", name);
        return 0;
    }

    frame->UndressSlot(lua_tointeger(L, 2) - 1);
    return 0;
}

// PROVEN: 35eeee73 IDENTICAL/NONE
// OFFSET: 0x00A04E20/0x1407A4560
void CGDressUpModelFrame::CreateScriptMetaTable() {
    lua_State* L = FrameScript_GetContext();
    CGDressUpModelFrame::s_metatable = FrameScript_Object::CreateScriptMetaTable(L, &CGDressUpModelFrame::RegisterScriptMethods);
}

// PROVEN: 628507f9 IDENTICAL/NONE
// OFFSET: 0x00A05880/0x1407A5260
static int32_t Script_TryOn(lua_State* L) {
    CGDressUpModelFrame* frame = static_cast<CGDressUpModelFrame*>(FrameScript_GetObjectThis(L, CGDressUpModelFrame::GetObjectType()));
    int32_t hand = -1;

    if (lua_isstring(L, 3)) {
        const char* slot = lua_tolstring(L, 3, NULL);

        if (SStrStr(slot, "mainhand")) {
            hand = 15;
        } else if (SStrStr(slot, "offhand")) {
            hand = 16;
        }
    }

    if (lua_isnumber(L, 2)) {
        frame->TryOnWithVisual(static_cast<int32_t>(lua_tonumber(L, 2)), 0, hand);
        return 0;
    }

    if (lua_isstring(L, 2)) {
        const char* link = lua_tolstring(L, 2, NULL);

        if (SStrStr(link, "item:")) {
            TooltipExtendedItemInfo info;
            info.Clear();
            info.GetItemIdFromLink(link);
            frame->TryOn(info.m_itemId, info.m_enchants, hand);
            return 0;
        }

        if (SStrStr(link, "enchant:")) {
            const char* enchant = SStrStr(link, "enchant:");

            if (enchant) {
                int32_t spellId = SStrToInt(enchant + 8);

                if (spellId > 0) {
                    SpellRec* spell;
                    const SpellEffectRec* effect;

                    if ((spell = g_spellDB.GetRecord(spellId)) && (spell->m_attributes & 0x20) && (effect = SpellGetEffect(spell->m_id, 0)) && effect->m_effectItemType) {
                        frame->TryOn(effect->m_effectItemType, NULL, -1);
                        return 0;
                    }
                }
            }
        } else {
            CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();
            CGItem_C* item = player->m_bag.FindItemByFullName(lua_tolstring(L, 2, NULL), 0);

            if (item) {
                frame->TryOnItem(item, hand);
                return 0;
            }
        }
    }

    CGGameUI::DisplayError(ERR_NOT_EQUIPPABLE);
    return 0;
}

// STATIC: 0x00D176E8
static FrameScript_Method s_ScriptMethods[] = {
    { "Undress", &Script_Undress },
    { "Dress", &Script_Dress },
    { "TryOn", &Script_TryOn },
    { "UndressSlot", &Script_UndressSlot },
};

// PROVEN: db121a66 IDENTICAL/NONE
// OFFSET: 0x00A04B40/0x1407A41D0
void CGDressUpModelFrame::RegisterScriptMethods(lua_State* L) {
    CGCharacterModelBase::RegisterScriptMethods(L);
    FrameScript_Object::FillScriptMethodTable(L, s_ScriptMethods, sizeof(s_ScriptMethods) / sizeof(s_ScriptMethods[0]));
}

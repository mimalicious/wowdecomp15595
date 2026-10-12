#include "GlyphInfo.h"
#include "WoW/Source/DB/Rec/SpellEffectRec.h"
#include <cstdint>
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"
#include "Engine/Source/Base/CDataStore.h"
#include "WoW/Source/Net/Types.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "Engine/Source/FrameXML/LoadXML.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/GlyphPropertiesRec.h"
#include "WoW/Source/DB/Rec/GlyphSlotRec.h"
#include "WoW/Source/DB/Rec/SpellIconRec.h"
#include "WoW/Source/DB/Rec/SpellRec.h"
#include "WoW/Source/Magic/MagicClient/Spell_C.h"
#include "WoW/Source/Object/Player.h"
#include "WoW/Source/Ui/TalentInfo.h"
#include "WoW/Source/UIUtil/Tooltip.h"
#include "Engine/Source/Storm/H/storm.h"
#include "WoW/Source/DB/Rec/ItemRecSparse.h"
#include "WoW/Source/DB/Rec/ItemRec.h"
#include "WoW/Source/DB/Rec/SpellReagentsRec.h"
#include "WoW/Source/Object/ObjectClient/Bag_C.h"
#include "WoW/Source/Object/ObjectClient/Item_C.h"

// OFFSET: 0x009EEB00/0x1407881E0
static int32_t Script_GetNumGlyphSockets(lua_State* L) {
    lua_pushnumber(L, 9.0);
    return 1;
}

// OFFSET: 0x009EEB30/0x140788200
static int32_t Script_SetGlyphFilter(lua_State* L) {
    if (lua_type(L, 1) != LUA_TBOOLEAN && lua_type(L, 2) != LUA_TBOOLEAN && !lua_isnumber(L, 3)) {
        luaL_error(L, "Usage: SetFilter(knowChecked, unKnowChecked, index)");
        return 0;
    }

    uint32_t filter = (lua_toboolean(L, 1) ? 8 : 0) | (lua_toboolean(L, 2) ? 0x10 : 0);
    uint32_t types[4] = { 1, 2, 4, 7 };
    uint32_t index = static_cast<uint32_t>(lua_tonumber(L, 3)) - 1;

    if (index < 4) {
        filter |= types[index];
    }

    CGGlyphInfo::s_filter = filter;
    CGGlyphInfo::s_numFiltered = -1;
    return 0;
}

// OFFSET: 0x009EEC10/0x1407882E0
static int32_t Script_ToggleGlyphFilter(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: ToggleGlyphFilter(filter)");
        return 0;
    }

    uint32_t filter = lua_tointeger(L, 1);
    CGGlyphInfo::s_numFiltered = -1;

    if (filter & CGGlyphInfo::s_filter) {
        CGGlyphInfo::s_filter &= ~filter;
    } else {
        CGGlyphInfo::s_filter |= filter;
    }

    return 0;
}

// OFFSET: 0x009EEC70/0x140788350
static int32_t Script_IsGlyphFlagSet(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: IsGlyphFlagSet(flag)");
        return 0;
    }

    uint32_t flag = lua_tointeger(L, 1);
    lua_pushboolean(L, flag & CGGlyphInfo::s_filter);
    return 1;
}

// OFFSET: 0x009EF290/0x140788B50
static int32_t Script_GetGlyphInfo(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetGlyphInfo(index)");
        return 0;
    }

    uint32_t index = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1;

    if (index >= CGGlyphInfo::s_glyphs.Count()) {
        return 0;
    }

    GlyphItem* glyph = CGGlyphInfo::s_glyphs[index];

    if (!glyph) {
        return 0;
    }

    if (glyph->m_spellID == -1) {
        lua_pushstring(L, "header");
        lua_pushinteger(L, glyph->m_type + 1);
        lua_pushinteger(L, 1 << glyph->m_type);
        return 3;
    }

    SpellRec* spell = g_spellDB.GetRecord(glyph->m_spellID);

    if (!spell) {
        return 0;
    }

    lua_pushstring(L, spell->m_name);
    lua_pushinteger(L, glyph->m_type + 1);
    lua_pushboolean(L, glyph->m_known);

    uint32_t count;
    const TSGrowableArray<const SpellEffectRec*>* effects = SpellGetEffects(spell->m_id, count);
    int32_t glyphId = -1;
    SpellIconRec* icon = NULL;

    for (uint32_t i = 0; i < count; i++) {
        if (effects->m_data[i] && effects->m_data[i]->m_effect == 74) {
            glyphId = effects->m_data[i]->m_effectMiscValue;
            GlyphPropertiesRec* properties = g_glyphPropertiesDB.GetRecord(glyphId);

            if (properties) {
                SpellRec* glyphSpell = g_spellDB.GetRecord(properties->m_spellID);

                if (glyphSpell && (icon = g_spellIconDB.GetRecord(glyphSpell->m_spellIconID)) != NULL) {
                    break;
                }
            }
        }
    }

    if (icon || (icon = g_spellIconDB.GetRecord(spell->m_spellIconID)) != NULL) {
        lua_pushstring(L, icon->m_textureFilename);
    } else {
        lua_pushnil(L);
    }

    if (glyphId == -1) {
        lua_pushnil(L);
        lua_pushnil(L);
    } else {
        lua_pushinteger(L, glyphId);
        lua_pushstring(L, CGTooltip::GetGlyphHyperlinkString(glyphId));
    }

    return 6;
}

// OFFSET: 0x009EF490/0x140788E50
static int32_t Script_SetGlyphNameFilter(lua_State* L) {
    CGGlyphInfo::SetNameFilter(lua_tostring(L, 1));
    return 0;
}

// OFFSET: 0x009EF4B0/0x140788E70
static int32_t Script_GetSelectedGlyphSpellIndex(lua_State* L) {
    int32_t i;

    for (i = CGGlyphInfo::s_glyphs.Count() - 1; i >= 0; i--) {
        int32_t spellID = CGGlyphInfo::s_glyphs[i]->m_spellID;

        if (spellID != -1 && spellID == CGGlyphInfo::s_selectedGlyph) {
            break;
        }
    }

    if (i >= 0) {
        lua_pushinteger(L, i + 1);
        return 1;
    }

    return 0;
}

// OFFSET: 0x009EF510/0x140788ED0
static int32_t Script_CastGlyphByName(lua_State* L) {
    if (!lua_isstring(L, 1) || !lua_isnumber(L, 2)) {
        luaL_error(L, "Usage: CastGlyphByName(name, slot)");
        return 0;
    }

    int32_t spellID = CGGlyphInfo::GetGlyphSpellIDByName(lua_tostring(L, 1));
    uint32_t slot = static_cast<uint32_t>(lua_tonumber(L, 2)) - 1;
    CGTalentGroup_C* group = TalentInfoGetTalentGroup(TalentInfoGetActiveTalentGroup(0, 0), 0, 0);

    if (slot < 9 && group && spellID) {
        if (g_glyphPropertiesDB.GetRecord(group->GetGlyph(slot))) {
            Spell_C_ClickSpell(CGGlyphInfo::s_clearGlyphSpell, NULL, WOWGUID(), 0, 0);
            Spell_C_HandleGlyphClick(slot);
        }

        Spell_C_ClickSpell(spellID, NULL, WOWGUID(), 0, 0);
        Spell_C_HandleGlyphClick(slot);
    }

    return 0;
}

// OFFSET: 0x009EF610/0x140788FF0
static int32_t Script_CastGlyphByID(lua_State* L) {
    if (!lua_isstring(L, 1) || !lua_isnumber(L, 2)) {
        luaL_error(L, "Usage: CastGlyphByID(spellID, slot)");
        return 0;
    }

    int32_t spellID = static_cast<int32_t>(lua_tonumber(L, 1));
    uint32_t slot = static_cast<uint32_t>(lua_tonumber(L, 2)) - 1;
    CGTalentGroup_C* group = TalentInfoGetTalentGroup(TalentInfoGetActiveTalentGroup(0, 0), 0, 0);

    if (slot < 9 && group && spellID && CGGlyphInfo::IsGlyphKnown(spellID)) {
        if (g_glyphPropertiesDB.GetRecord(group->GetGlyph(slot))) {
            Spell_C_ClickSpell(CGGlyphInfo::s_clearGlyphSpell, NULL, WOWGUID(), 0, 0);
            Spell_C_HandleGlyphClick(slot);
        }

        Spell_C_ClickSpell(spellID, NULL, WOWGUID(), 0, 0);
        Spell_C_HandleGlyphClick(slot);
    }

    return 0;
}

// OFFSET: 0x009EF8C0/0x140789390
static int32_t Script_GetGlyphSocketInfo(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetGlyphSocketInfo(index[, talentGroup])");
        return 0;
    }

    uint32_t index = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1;
    CGTalentGroup_C* group = TalentInfoGetTalentGroup(TalentInfoGetScriptGroupIndex(L, 0, 0, 2), 0, 0);
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (index >= 6 || !group || !player) {
        return 0;
    }

    if (player->m_player->m_playerGlyphsEnabled & (1 << index)) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    GlyphSlotRec* slot = g_glyphSlotDB.GetRecord(player->m_player->m_playerFieldGlyphSlots1[index]);

    if (slot) {
        lua_pushnumber(L, slot->m_type + 1);
    } else {
        lua_pushnil(L);
    }

    GlyphPropertiesRec* glyph = g_glyphPropertiesDB.GetRecord(group->GetGlyph(index));

    if (glyph) {
        lua_pushnumber(L, glyph->m_spellID);
        SpellIconRec* icon = g_spellIconDB.GetRecord(glyph->m_spellIconID);

        if (icon) {
            lua_pushstring(L, icon->m_textureFilename);
        } else {
            lua_pushnil(L);
        }
    } else {
        lua_pushnil(L);
        lua_pushnil(L);
    }

    return 4;
}

// OFFSET: 0x009EFAA0/0x140789600
static int32_t Script_GlyphMatchesSocket(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GlyphMatchesSocket(socketIndex)");
        return 0;
    }

    uint32_t index = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1;
    CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();

    if (!player) {
        lua_pushnil(L);
        return 1;
    }

    SpellRec* spell;
    PendingSpellCast* cast = Spell_C_GetTargetingSpellCast();

    if (!cast || !(spell = g_spellDB.GetRecord(cast->m_data.m_spellId))) {
        return 0;
    }

    uint32_t count;
    const TSGrowableArray<const SpellEffectRec*>* effects = SpellGetEffects(spell->m_id, count);

    for (uint32_t i = 0; i < count; i++) {
        const SpellEffectRec* effect = effects->m_data[i];

        if (!effect || effect->m_effect != 74) {
            continue;
        }

        if (!effect->m_effectMiscValue) {
            lua_pushnumber(L, 1.0);
            return 1;
        }

        GlyphPropertiesRec* glyph = g_glyphPropertiesDB.GetRecord(effect->m_effectMiscValue);
        GlyphSlotRec* slot = g_glyphSlotDB.GetRecord(player->m_player->m_playerFieldGlyphSlots1[index]);

        if (!glyph || !slot) {
            continue;
        }

        if (slot->m_type != glyph->m_glyphSlotFlags) {
            return 0;
        }

        lua_pushnumber(L, 1.0);
        return 1;
    }

    return 0;
}

// OFFSET: 0x009EFC30/0x140789800
static int32_t Script_PlaceGlyphInSocket(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: PlaceGlyphInSocket(index)");
        return 0;
    }

    uint32_t index = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1;
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (index >= 6 || !player) {
        return 0;
    }

    if (Spell_C_HandleGlyphClick(index)) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: 0x009EFDC0/0x1407899B0
static int32_t Script_RemoveGlyphFromSocket(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: RemoveGlyphFromSocket(index)");
        return 0;
    }

    uint32_t index = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1;
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (index < 9 && player && player->m_player->m_playerFieldGlyphSlots1[index]) {
        Spell_C_ClickSpell(CGGlyphInfo::s_clearGlyphSpell, NULL, WOWGUID::Make(0), 0, 0);
        Spell_C_HandleGlyphClick(index);
        CGGlyphInfo::s_selectedGlyph = -1;
        FrameScript_SignalEvent(SCRIPT_EVENT_USE_GLYPH, NULL);
    }

    return 0;
}

// OFFSET: 0x009EFE90/0x140789A70
static int32_t Script_GetGlyphLink(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetGlyphLink(index[, talentGroup])");
        return 0;
    }

    uint32_t index = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1;
    CGTalentGroup_C* group = TalentInfoGetTalentGroup(TalentInfoGetScriptGroupIndex(L, 0, 0, 2), 0, 0);
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (index >= 9 || !player || !group) {
        return 0;
    }

    lua_pushstring(L, CGTooltip::GetGlyphHyperlinkString(group->GetGlyph(index)));
    return 1;
}

// OFFSET: 0x009EFF60/0x140789B50
static int32_t Script_GetNumGlyphs(lua_State* L) {
    if (CGGlyphInfo::s_numFiltered == -1) {
        CGGlyphInfo::FilterGlyphs();
    }

    lua_pushinteger(L, CGGlyphInfo::s_numFiltered);
    return 1;
}

// OFFSET: 0x009EFF90/0x140789B90
static int32_t Script_CastGlyph(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: CastGlyph(index)");
        return 0;
    }

    CGGlyphInfo::CastGlyph(static_cast<uint32_t>(lua_tonumber(L, 1)) - 1);
    return 0;
}

// OFFSET: 0x009F0360/0x14078A0D0
static int32_t Script_GetGlyphClearInfo(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));
    int32_t clearSpell = CGGlyphInfo::s_clearGlyphSpell;

    if (!player && clearSpell > 0) {
        return 0;
    }

    SpellRec* spell = g_spellDB.GetRecord(clearSpell);

    if (!spell || !spell->m_spellReagentsID) {
        return 0;
    }

    SpellReagentsRec* reagents = g_spellReagentsDB.GetRecord(spell->m_spellReagentsID);

    if (!reagents) {
        return 0;
    }

    uint32_t i;
    ItemRecSparse* item;
    ItemRec* itemRec;

    for (i = 0; ; i++) {
        if (reagents->m_reagent[i] > 0) {
            WOWGUID none;
            item = g_itemSparseDB.GetRecord(reagents->m_reagent[i], none, ClearGlyphReagentCallback, NULL, false);

            if (item && (itemRec = item->GetNonSparse()) != NULL) {
                break;
            }
        }

        if (i + 1 >= 8) {
            return 0;
        }
    }

    lua_pushstring(L, item->GetName(0));
    lua_pushinteger(L, player->GetBag()->GetItemTypeCount(reagents->m_reagent[i], 0));

    const char* path = ClientDBStringLookup(3);
    char texture[260];
    SStrPrintf(texture, sizeof(texture), "%s%s%s", path, *path ? "\\" : "", CGItem_C::GetInventoryArt(itemRec->m_displayInfoID));
    lua_pushstring(L, texture);
    lua_pushinteger(L, clearSpell);
    lua_pushinteger(L, reagents->m_reagentCount[i]);
    return 5;
}

// STATIC: 0x00D16D48/0x140B3D060
static FrameScript_Method s_ScriptFunctions[] = {
    { "GetNumGlyphSockets", &Script_GetNumGlyphSockets },
    { "GetGlyphSocketInfo", &Script_GetGlyphSocketInfo },
    { "GlyphMatchesSocket", &Script_GlyphMatchesSocket },
    { "PlaceGlyphInSocket", &Script_PlaceGlyphInSocket },
    { "RemoveGlyphFromSocket", &Script_RemoveGlyphFromSocket },
    { "GetGlyphLink", &Script_GetGlyphLink },
    { "GetNumGlyphs", &Script_GetNumGlyphs },
    { "GetGlyphInfo", &Script_GetGlyphInfo },
    { "CastGlyph", &Script_CastGlyph },
    { "SetGlyphFilter", &Script_SetGlyphFilter },
    { "ToggleGlyphFilter", &Script_ToggleGlyphFilter },
    { "IsGlyphFlagSet", &Script_IsGlyphFlagSet },
    { "SetGlyphNameFilter", &Script_SetGlyphNameFilter },
    { "GetGlyphClearInfo", &Script_GetGlyphClearInfo },
    { "GetSelectedGlyphSpellIndex", &Script_GetSelectedGlyphSpellIndex },
    { "CastGlyphByName", &Script_CastGlyphByName },
    { "CastGlyphByID", &Script_CastGlyphByID },
};

// OFFSET: 0x009EECD0/0x1407883C0
void GlyphInfoRegisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_RegisterFunction(s_ScriptFunctions[i].m_name, s_ScriptFunctions[i].m_method);
    }
}

// OFFSET: 0x009EED00/0x140788410
void GlyphInfoUnregisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_UnregisterFunction(s_ScriptFunctions[i].m_name);
    }
}

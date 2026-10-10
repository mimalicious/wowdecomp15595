#include "Tooltip.h"
#include <cstdint>
#include <cstdio>
#include <cstring>
#include "Engine/Source/Base/Coordinate.h"
#include "Engine/Source/ConsoleBase/ConsoleVar.h"
#include "Engine/Source/Frame/CSimpleRender.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"
#include "Engine/Source/FrameXML/LoadXML.h"
#include "Engine/Source/Os/TimeManager.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/Tempest/CAaRect.h"
#include "Engine/Source/Tempest/CImVector.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"
#include "Engine/Source/lua-5.1/src/lua.h"
#include "WoW/Common/WowTime.h"
#include "WoW/Source/DB/DBClient/DBCacheInstances.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/CurrencyTypesRec.h"
#include "WoW/Source/DB/Rec/ItemExtendedCostRec.h"
#include "WoW/Source/DB/Rec/ItemRec.h"
#include "WoW/Source/DB/Rec/ItemRecSparse.h"
#include "WoW/Source/DB/Rec/LfgDungeonsRec.h"
#include "WoW/Source/DB/Rec/SpellEffectRec.h"
#include "WoW/Source/DB/Rec/SpellItemEnchantmentRec.h"
#include "WoW/Source/DB/Rec/SpellRec.h"
#include "WoW/Source/DB/Rec/SpellReagentsRec.h"
#include "WoW/Source/DB/Rec/TalentRec.h"
#include "WoW/Source/Magic/MagicClient/Spell_C.h"
#include "WoW/Source/Object/Item.h"
#include "WoW/Source/Object/ObjectClient/Bag_C.h"
#include "WoW/Source/Object/ObjectClient/Container_C.h"
#include "WoW/Source/Object/ObjectClient/Item_C.h"
#include "WoW/Source/Object/ObjectClient/LootRoll.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/Object/ObjectClient/UnitCombatLog_C.h"
#include "WoW/Source/Object/ObjectClient/VoidStorage_C.h"
#include "WoW/Source/Object/ObjectClient/VoidTransfer.h"
#include "WoW/Source/Object/QuestCache.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "Engine/Source/Event/FrameTime.h"
#include "WoW/Source/UIUtil/Types.h"
#include "WoW/Source/Ui/ActionBarFrame.h"
#include "WoW/Source/Ui/AuctionHouse.h"
#include "WoW/Source/Ui/ChatFrame.h"
#include "WoW/Source/Ui/ClassTrainerFrame.h"
#include "WoW/Source/Ui/ContainerFrame.h"
#include "WoW/Source/Ui/CurrencyTypes.h"
#include "WoW/Source/Ui/DanceStudio.h"
#include "WoW/Source/Ui/EquipmentManager.h"
#include "WoW/Source/Ui/GameUI.h"
#include "WoW/Source/Ui/GuildBankFrame.h"
#include "WoW/Source/Ui/ItemSocketInfo.h"
#include "WoW/Source/Ui/LFGInfo.h"
#include "WoW/Source/Ui/LootFrame.h"
#include "WoW/Source/Ui/MailInfo.h"
#include "WoW/Source/Ui/MerchantFrame.h"
#include "WoW/Source/Ui/PaperDollInfoFrame.h"
#include "WoW/Source/Ui/PetInfo.h"
#include "WoW/Source/Ui/QuestFrame.h"
#include "WoW/Source/Ui/QuestLog.h"
#include "WoW/Source/Ui/RaidInfo.h"
#include "WoW/Source/Ui/Reforge.h"
#include "WoW/Source/Ui/ScriptEvents.h"
#include "WoW/Source/Ui/SpellBookFrame.h"
#include "WoW/Source/Ui/TalentInfo.h"
#include "WoW/Source/Ui/TotemInfo.h"
#include "WoW/Source/Ui/TradeFrame.h"
#include "WoW/Source/Ui/TradeSkillFrame.h"
#include "WoW/Source/Ui/Transmogrify.h"

// OFFSET: 0x004648B0
static int32_t ReadTexCoordArgs(lua_State* L, int32_t index, CAaRect& coords) {
    coords.minY = static_cast<float>(lua_tonumber(L, index));
    coords.maxY = static_cast<float>(lua_tonumber(L, index + 1));
    coords.minX = static_cast<float>(lua_tonumber(L, index + 2));
    coords.maxX = static_cast<float>(lua_tonumber(L, index + 3));
    return index + 4;
}

// OFFSET: 0x00467C20/0x14007BAF0
static int32_t Script_SetMinimumWidth(lua_State* L) {
    CGTooltip* tooltip = static_cast<CGTooltip*>(FrameScript_GetObjectThis(L, CGTooltip::GetObjectType()));

    if (!lua_isnumber(L, 2)) {
        const char* name = tooltip->GetName();
        luaL_error(L, "Usage: %s:SetMinimumWidth(width [,force])", name ? name : "<unnamed>");
        return 0;
    }

    int32_t force = StringToBOOL(L, 3, 0);
    tooltip->m_minimumWidth = static_cast<float>(lua_tonumber(L, 2));
    tooltip->m_minimumWidthForce = force;
    return 0;
}

// OFFSET: 0x00467CB0/0x14007BBB0
static int32_t Script_GetMinimumWidth(lua_State* L) {
    CGTooltip* tooltip = static_cast<CGTooltip*>(FrameScript_GetObjectThis(L, CGTooltip::GetObjectType()));
    lua_pushnumber(L, tooltip->m_minimumWidth);

    if (tooltip->m_minimumWidthForce) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 2;
}

// OFFSET: 0x00467D30/0x14007BC40
static int32_t Script_SetPadding(lua_State* L) {
    CGTooltip* tooltip = static_cast<CGTooltip*>(FrameScript_GetObjectThis(L, CGTooltip::GetObjectType()));
    float padding = static_cast<float>(lua_tonumber(L, 2));
    tooltip->m_padding = NDCToDDCWidth(padding / (CoordinateGetAspectCompensation() * 1024.0f));
    return 0;
}

// OFFSET: 0x00467D90/0x14007BCC0
static int32_t Script_GetPadding(lua_State* L) {
    CGTooltip* tooltip = static_cast<CGTooltip*>(FrameScript_GetObjectThis(L, CGTooltip::GetObjectType()));
    lua_pushnumber(L, tooltip->m_padding);
    return 1;
}

// OFFSET: 0x00467DE0/0x14007BD10
static int32_t Script_IsOwned(lua_State* L) {
    CGTooltip* tooltip = static_cast<CGTooltip*>(FrameScript_GetObjectThis(L, CGTooltip::GetObjectType()));

    if (lua_type(L, 2) != LUA_TTABLE) {
        const char* name = tooltip->GetName();
        luaL_error(L, "Usage: %s:IsOwned(frame)", name ? name : "<unnamed>");
        return 0;
    }

    lua_rawgeti(L, 2, 0);
    CSimpleFrame* frame = static_cast<CSimpleFrame*>(lua_touserdata(L, -1));
    lua_settop(L, -2);

    if (!frame) {
        const char* name = tooltip->GetName();
        luaL_error(L, "%s:IsOwned(): Couldn't find 'this' in frame object", name ? name : "<unnamed>");
        return 0;
    }

    if (!frame->IsA(CSimpleFrame::GetObjectType())) {
        const char* name = tooltip->GetName();
        luaL_error(L, "%s:IsOwned(): Wrong object type, expected frame", name ? name : "<unnamed>");
        return 0;
    }

    if (tooltip->m_owner == frame) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: 0x00467F20/0x14007BEB0
static int32_t Script_GetOwner(lua_State* L) {
    CGTooltip* tooltip = static_cast<CGTooltip*>(FrameScript_GetObjectThis(L, CGTooltip::GetObjectType()));
    CSimpleFrame* owner = tooltip->m_owner;

    if (!owner) {
        lua_pushnil(L);
        return 1;
    }

    if (!owner->m_luaRegistered) {
        owner->RegisterScriptObject(NULL);
    }

    lua_rawgeti(L, LUA_REGISTRYINDEX, owner->m_luaObjectRef);
    return 1;
}

// OFFSET: 0x00467F90/0x14007BF40
static int32_t Script_SetAnchorType(lua_State* L) {
    CGTooltip* tooltip = static_cast<CGTooltip*>(FrameScript_GetObjectThis(L, CGTooltip::GetObjectType()));

    if (!lua_isstring(L, 2)) {
        const char* name = tooltip->GetName();
        luaL_error(L, "Usage: %s:SetAnchorType( anchorType [,Xoffset] [,Yoffset] )", name ? name : "<unnamed>");
        return 0;
    }

    TOOLTIP_ANCHORPOINT anchor = ANCHOR_LEFT;

    if (lua_isstring(L, 2)) {
        const char* point = lua_tolstring(L, 2, NULL);

        if (_stricmp(point, "ANCHOR_LEFT")) {
            if (!_stricmp(point, "ANCHOR_RIGHT")) {
                anchor = ANCHOR_RIGHT;
            } else if (!_stricmp(point, "ANCHOR_BOTTOMLEFT")) {
                anchor = ANCHOR_BOTTOMLEFT;
            } else if (!_stricmp(point, "ANCHOR_BOTTOM")) {
                anchor = ANCHOR_BOTTOM;
            } else if (!_stricmp(point, "ANCHOR_BOTTOMRIGHT")) {
                anchor = ANCHOR_BOTTOMRIGHT;
            } else if (!_stricmp(point, "ANCHOR_TOPLEFT")) {
                anchor = ANCHOR_TOPLEFT;
            } else if (!_stricmp(point, "ANCHOR_TOP")) {
                anchor = ANCHOR_TOP;
            } else if (!_stricmp(point, "ANCHOR_TOPRIGHT")) {
                anchor = ANCHOR_TOPRIGHT;
            } else if (!_stricmp(point, "ANCHOR_CURSOR")) {
                anchor = ANCHOR_CURSOR;
            } else if (!_stricmp(point, "ANCHOR_NONE")) {
                anchor = ANCHOR_NONE;
            } else if (!_stricmp(point, "ANCHOR_PRESERVE")) {
                anchor = ANCHOR_PRESERVE;
            } else if (!_stricmp(point, "ANCHOR_CURSOR_RIGHT")) {
                anchor = ANCHOR_CURSOR_RIGHT;
            }
        }
    }

    float offsetX = 0.0f;
    float offsetY = 0.0f;

    if (lua_isnumber(L, 3)) {
        float value = static_cast<float>(lua_tonumber(L, 3));
        offsetX = NDCToDDCWidth(value / (CoordinateGetAspectCompensation() * 1024.0f));
    }

    if (lua_isnumber(L, 4)) {
        float value = static_cast<float>(lua_tonumber(L, 4));
        offsetY = NDCToDDCWidth(value / (CoordinateGetAspectCompensation() * 1024.0f));
    }

    tooltip->m_offsetX = offsetX;
    tooltip->m_offsetY = offsetY;

    if (tooltip->m_owner) {
        tooltip->m_anchorType = anchor;
    }

    tooltip->ResetPosition(1);
    return 0;
}

// OFFSET: 0x00468200/0x14007C200
static int32_t Script_GetAnchorType(lua_State* L) {
    CGTooltip* tooltip = static_cast<CGTooltip*>(FrameScript_GetObjectThis(L, CGTooltip::GetObjectType()));

    switch (tooltip->m_anchorType) {
        case ANCHOR_LEFT:
            lua_pushstring(L, "ANCHOR_LEFT");
            return 1;
        case ANCHOR_RIGHT:
            lua_pushstring(L, "ANCHOR_RIGHT");
            return 1;
        case ANCHOR_BOTTOMLEFT:
            lua_pushstring(L, "ANCHOR_BOTTOMLEFT");
            return 1;
        case ANCHOR_BOTTOM:
            lua_pushstring(L, "ANCHOR_BOTTOM");
            return 1;
        case ANCHOR_BOTTOMRIGHT:
            lua_pushstring(L, "ANCHOR_BOTTOMRIGHT");
            return 1;
        case ANCHOR_TOPLEFT:
            lua_pushstring(L, "ANCHOR_TOPLEFT");
            return 1;
        case ANCHOR_TOP:
            lua_pushstring(L, "ANCHOR_TOP");
            return 1;
        case ANCHOR_TOPRIGHT:
            lua_pushstring(L, "ANCHOR_TOPRIGHT");
            return 1;
        case ANCHOR_CURSOR:
            lua_pushstring(L, "ANCHOR_CURSOR");
            return 1;
        case ANCHOR_PRESERVE:
            lua_pushstring(L, "ANCHOR_PRESERVE");
            return 1;
        case ANCHOR_CURSOR_RIGHT:
            lua_pushstring(L, "ANCHOR_CURSOR_RIGHT");
            return 1;
        default:
            lua_pushstring(L, "ANCHOR_NONE");
            return 1;
    }
}

// OFFSET: 0x00468380/0x14007C3C0
static int32_t Script_ClearLines(lua_State* L) {
    CGTooltip* tooltip = static_cast<CGTooltip*>(FrameScript_GetObjectThis(L, CGTooltip::GetObjectType()));
    tooltip->ClearTooltip();
    return 0;
}

// OFFSET: 0x004683C0/0x14007C400
static int32_t Script_AddTexture(lua_State* L) {
    CGTooltip* tooltip = static_cast<CGTooltip*>(FrameScript_GetObjectThis(L, CGTooltip::GetObjectType()));
    const char* texture = NULL;

    if (lua_isstring(L, 2)) {
        texture = lua_tolstring(L, 2, NULL);
    }

    if (lua_isnumber(L, 3)) {
        if (!lua_isnumber(L, 4) || !lua_isnumber(L, 5) || !lua_isnumber(L, 6)) {
            const char* name = tooltip->GetName();
            luaL_error(L, "Usage: %s:AddTexture(\"filename\" [, minx, maxx, miny, maxy])", name ? name : "<unnamed>");
            return 0;
        }

        CAaRect coords;
        coords.minX = 0.0f;
        coords.minY = 0.0f;
        coords.maxX = 0.0f;
        coords.maxY = 0.0f;
        ReadTexCoordArgs(L, 3, coords);
        CImVector color(0xFFFFFFFF);
        tooltip->AddTexture(texture, coords, color);
        return 0;
    }

    CAaRect coords;
    coords.minX = 0.0f;
    coords.minY = 0.0f;
    coords.maxX = 1.0f;
    coords.maxY = 1.0f;
    CImVector color(0xFFFFFFFF);
    tooltip->AddTexture(texture, coords, color);
    return 0;
}

// OFFSET: 0x00468500/0x14007C5B0
static int32_t Script_FadeOut(lua_State* L) {
    CGTooltip* tooltip = static_cast<CGTooltip*>(FrameScript_GetObjectThis(L, CGTooltip::GetObjectType()));
    tooltip->FadeOut();
    return 0;
}

// OFFSET: 0x00468580/0x14007C630
static int32_t Script_NumLines(lua_State* L) {
    CGTooltip* tooltip = static_cast<CGTooltip*>(FrameScript_GetObjectThis(L, CGTooltip::GetObjectType()));
    lua_pushnumber(L, tooltip->m_numLines);
    return 1;
}

// OFFSET: 0x004685E0/0x14007C680
static int32_t Script_IsUnit(lua_State* L) {
    CGTooltip* tooltip = static_cast<CGTooltip*>(FrameScript_GetObjectThis(L, CGTooltip::GetObjectType()));

    if (!lua_isstring(L, 2)) {
        luaL_error(L, "Usage: %s:IsUnit(\"unit\")");
        return 0;
    }

    WOWGUID guid = Script_GetGUIDFromName(lua_tolstring(L, 2, NULL));

    if (guid.m_guid && guid == tooltip->m_unitGuid) {
        lua_pushnumber(L, 1.0);
        return 1;
    }

    lua_pushnil(L);
    return 1;
}

// OFFSET: 0x00468690/0x14007C740
static int32_t Script_GetUnit(lua_State* L) {
    CGTooltip* tooltip = static_cast<CGTooltip*>(FrameScript_GetObjectThis(L, CGTooltip::GetObjectType()));
    WOWGUID guid = tooltip->m_unitGuid;
    const char* token = Script_GetTokenFromGUID(&guid);
    const char* name = NULL;

    if (token && Script_GetNameFromToken(token, &name, true, false)) {
        lua_pushstring(L, name);
        lua_pushstring(L, token);
        return 2;
    }

    if (CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(tooltip->m_unitGuid, TYPEMASK_UNIT, __FILE__, __LINE__))) {
        lua_pushstring(L, unit->GetUnitName(0, 1));
        lua_pushstring(L, token);
        return 2;
    }

    return 0;
}

// OFFSET: 0x00468760/0x14007C830
static int32_t Script_GetSpell(lua_State* L) {
    CGTooltip* tooltip = static_cast<CGTooltip*>(FrameScript_GetObjectThis(L, CGTooltip::GetObjectType()));
    int32_t count = 0;

    if (SpellRec* spell = g_spellDB.GetRecord(tooltip->m_spellId)) {
        lua_pushstring(L, spell->m_name);
        lua_pushstring(L, spell->m_nameSubtext);
        lua_pushnumber(L, tooltip->m_spellId);
        count = 3;
    }

    if (SpellRec* spell = g_spellDB.GetRecord(tooltip->m_nextRankSpellId)) {
        lua_pushstring(L, spell->m_name);
        lua_pushstring(L, spell->m_nameSubtext);
        lua_pushnumber(L, tooltip->m_nextRankSpellId);
        count += 3;
    }

    return count;
}

// OFFSET: 0x00468860/0x14007C9A0
static int32_t FindItemByGUID(const CGItem_C* item, void* param) {
    return item->m_obj->m_guid == *static_cast<WOWGUID*>(param);
}

// OFFSET: 0x00468890
void CGTooltip::CreateScriptMetaTable() {
    lua_State* L = FrameScript_GetContext();
    CGTooltip::s_metatable = FrameScript_Object::CreateScriptMetaTable(L, &CGTooltip::RegisterScriptMethods);
}

// OFFSET: 0x00469B60/0x14007DEF0
static int32_t Script_SetOwner(lua_State* L) {
    CGTooltip* tooltip = static_cast<CGTooltip*>(FrameScript_GetObjectThis(L, CGTooltip::GetObjectType()));
    tooltip->Hide();

    if (lua_type(L, 2) != LUA_TTABLE) {
        const char* name = tooltip->GetName();
        luaL_error(L, "Usage: %s:SetOwner(frame)", name ? name : "<unnamed>");
        return 0;
    }

    lua_rawgeti(L, 2, 0);
    CSimpleFrame* owner = static_cast<CSimpleFrame*>(lua_touserdata(L, -1));
    lua_settop(L, -2);

    if (!owner) {
        const char* name = tooltip->GetName();
        luaL_error(L, "%s:SetOwner(): Couldn't find 'this' in frame object", name ? name : "<unnamed>");
        return 0;
    }

    if (!owner->IsA(CSimpleFrame::GetObjectType())) {
        const char* name = tooltip->GetName();
        luaL_error(L, "%s:SetOwner(): Wrong object type, expected frame", name ? name : "<unnamed>");
        return 0;
    }

    if (owner == tooltip) {
        const char* name = tooltip->GetName();
        luaL_error(L, "%s:SetOwner(): Can't set owner to self", name ? name : "<unnamed>");
        return 0;
    }

    TOOLTIP_ANCHORPOINT anchor = ANCHOR_LEFT;

    if (lua_isstring(L, 3)) {
        const char* point = lua_tolstring(L, 3, NULL);

        if (_stricmp(point, "ANCHOR_LEFT")) {
            if (!_stricmp(point, "ANCHOR_RIGHT")) {
                anchor = ANCHOR_RIGHT;
            } else if (!_stricmp(point, "ANCHOR_BOTTOMLEFT")) {
                anchor = ANCHOR_BOTTOMLEFT;
            } else if (!_stricmp(point, "ANCHOR_BOTTOM")) {
                anchor = ANCHOR_BOTTOM;
            } else if (!_stricmp(point, "ANCHOR_BOTTOMRIGHT")) {
                anchor = ANCHOR_BOTTOMRIGHT;
            } else if (!_stricmp(point, "ANCHOR_TOPLEFT")) {
                anchor = ANCHOR_TOPLEFT;
            } else if (!_stricmp(point, "ANCHOR_TOP")) {
                anchor = ANCHOR_TOP;
            } else if (!_stricmp(point, "ANCHOR_TOPRIGHT")) {
                anchor = ANCHOR_TOPRIGHT;
            } else if (!_stricmp(point, "ANCHOR_CURSOR")) {
                anchor = ANCHOR_CURSOR;
            } else if (!_stricmp(point, "ANCHOR_NONE")) {
                anchor = ANCHOR_NONE;
            } else if (!_stricmp(point, "ANCHOR_PRESERVE")) {
                anchor = ANCHOR_PRESERVE;
            } else if (!_stricmp(point, "ANCHOR_CURSOR_RIGHT")) {
                anchor = ANCHOR_CURSOR_RIGHT;
            }
        }
    }

    float offsetX = 0.0f;

    if (lua_isnumber(L, 4)) {
        float value = static_cast<float>(lua_tonumber(L, 4));
        offsetX = NDCToDDCWidth(value / (CoordinateGetAspectCompensation() * 1024.0f));
    }

    float offsetY;

    if (lua_isnumber(L, 5)) {
        float value = static_cast<float>(lua_tonumber(L, 5));
        offsetY = NDCToDDCWidth(value / (CoordinateGetAspectCompensation() * 1024.0f));
    } else {
        offsetY = 0.0f;
    }

    tooltip->SetOwner(owner, anchor, offsetX, offsetY);
    return 0;
}

// OFFSET: 0x00469E90/0x14007E2B0
static int32_t Script_AppendText(lua_State* L) {
    CGTooltip* tooltip = static_cast<CGTooltip*>(FrameScript_GetObjectThis(L, CGTooltip::GetObjectType()));

    if (!lua_isstring(L, 2)) {
        const char* name = tooltip->GetName();
        luaL_error(L, "Usage: %s:AppendText(\"text\")", name ? name : "<unnamed>");
        return 0;
    }

    tooltip->AppendText(lua_tolstring(L, 2, NULL));
    return 0;
}

// OFFSET: 0x00469F10/0x14007E350
static int32_t Script_IsEquippedItem(lua_State* L) {
    CGTooltip* tooltip = static_cast<CGTooltip*>(FrameScript_GetObjectThis(L, CGTooltip::GetObjectType()));
    CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();
    WOWGUID guid = tooltip->m_itemGuid;

    if (player && guid.m_guid && player->m_bag.FindItem(FindItemByGUID, &guid, 1)) {
        lua_pushnumber(L, 1.0);
        return 1;
    }

    lua_pushnil(L);
    return 1;
}

// OFFSET: 0x0046AFF0/0x14007F900
static int32_t Script_AddFontStrings(lua_State* L) {
    CGTooltip* tooltip = static_cast<CGTooltip*>(FrameScript_GetObjectThis(L, CGTooltip::GetObjectType()));
    CSimpleFontString* strings[2];

    for (int32_t i = 2; i - 2 < 2; i++) {
        if (lua_type(L, i) != LUA_TTABLE) {
            const char* name = tooltip->GetName();
            luaL_error(L, "Usage: %s:AddFontStrings(leftstring, rightstring)", name ? name : "<unnamed>");
            continue;
        }

        lua_rawgeti(L, i, 0);
        CSimpleFontString* string = static_cast<CSimpleFontString*>(lua_touserdata(L, -1));
        lua_settop(L, -2);

        if (!string) {
            const char* name = tooltip->GetName();
            luaL_error(L, "%s:AddFontStrings(): Couldn't find 'this' in fontstring", name ? name : "<unnamed>");
            return 0;
        }

        if (!string->IsA(CSimpleFontString::GetObjectType())) {
            const char* name = tooltip->GetName();
            luaL_error(L, "%s:AddFontStrings(): Wrong object type, expected fontstring", name ? name : "<unnamed>");
            return 0;
        }

        strings[i - 2] = string;
    }

    tooltip->AddFontStrings(strings[0], strings[1]);
    return 0;
}

// OFFSET: 0x0046B140/0x14007FA70
static int32_t Script_AddLine(lua_State* L) {
    CGTooltip* tooltip = static_cast<CGTooltip*>(FrameScript_GetObjectThis(L, CGTooltip::GetObjectType()));
    const char* text = NULL;
    CImVector color = s_tooltipTitleColor;

    if (lua_isstring(L, 2)) {
        text = lua_tolstring(L, 2, NULL);
    }

    if (lua_isnumber(L, 3)) {
        FrameScript_GetColorNoAlpha(L, 3, color);
    }

    int32_t wrap = StringToBOOL(L, 6, 0);
    tooltip->AddLine(text, NULL, color, color, wrap);
    return 0;
}

// OFFSET: 0x0046B1E0/0x14007FB40
static int32_t Script_AddDoubleLine(lua_State* L) {
    CGTooltip* tooltip = static_cast<CGTooltip*>(FrameScript_GetObjectThis(L, CGTooltip::GetObjectType()));
    const char* left = NULL;
    const char* right = NULL;
    CImVector leftColor = s_tooltipTitleColor;
    CImVector rightColor = s_tooltipTitleColor;

    if (lua_isstring(L, 2)) {
        left = lua_tolstring(L, 2, NULL);
    }

    if (lua_isstring(L, 3)) {
        right = lua_tolstring(L, 3, NULL);
    }

    if (lua_isnumber(L, 4)) {
        FrameScript_GetColorNoAlpha(L, 4, leftColor);
    }

    if (lua_isnumber(L, 7)) {
        FrameScript_GetColorNoAlpha(L, 7, rightColor);
    }

    int32_t wrap = StringToBOOL(L, 10, 0);
    tooltip->AddLine(left, right, leftColor, rightColor, wrap);
    return 0;
}

// OFFSET: 0x0046B2D0/0x14007FC50
static int32_t Script_SetText(lua_State* L) {
    CGTooltip* tooltip = static_cast<CGTooltip*>(FrameScript_GetObjectThis(L, CGTooltip::GetObjectType()));

    if (!lua_isstring(L, 2)) {
        const char* name = tooltip->GetName();
        luaL_error(L, "Usage: %s:SetText(\"text\" [, color])", name ? name : "<unnamed>");
        return 0;
    }

    const char* text = lua_tolstring(L, 2, NULL);
    CImVector color = s_tooltipTitleColor;

    if (lua_isnumber(L, 3)) {
        FrameScript_GetColor(L, 3, color);
    }

    int32_t wrap = StringToBOOL(L, 7, 0);
    tooltip->ClearTooltip();
    tooltip->AddLine(text, NULL, color, color, wrap);
    tooltip->Show();
    return 0;
}

// OFFSET: 0x0046B3B0
static int32_t Script_SetTotem(lua_State* L) {
    CGTooltip* tooltip = static_cast<CGTooltip*>(FrameScript_GetObjectThis(L, CGTooltip::GetObjectType()));

    if (!lua_isnumber(L, 2)) {
        const char* name = tooltip->GetName();
        luaL_error(L, "Usage: %s:SetTotem(slot)", name ? name : "<unnamed>");
        return 0;
    }

    int32_t slot = static_cast<int32_t>(lua_tonumber(L, 2));
    TotemInfo* totem = CGGameUI::GetTotemInfo(static_cast<uint32_t>(slot - 1));

    if (!totem || !totem->m_guid.m_guid) {
        return 0;
    }

    tooltip->ClearTooltip();
    tooltip->AddLine(totem->m_name, NULL, s_tooltipTitleColor, s_tooltipTitleColor, 0);
    char text[1024];
    CGTooltip::GetDurationString(text, sizeof(text), totem->GetSpawnTimeLeft(), "SPELL_TIME_REMAINING", NULL, 1, false);
    tooltip->AddLine(text, NULL, s_tooltipTextColor, s_tooltipTextColor, 0);
    tooltip->Show();
    return 0;
}

// OFFSET: 0x0046DAF0/0x140083440
static int32_t Script_SetGlyph(lua_State* L) {
    CGTooltip* tooltip = static_cast<CGTooltip*>(FrameScript_GetObjectThis(L, CGTooltip::GetObjectType()));

    if (!lua_isnumber(L, 2)) {
        const char* name = tooltip->GetName();
        luaL_error(L, "Usage: %s:SetGlyph(glyphSlot[, groupIndex])", name ? name : "<unnamed>");
        return 0;
    }

    CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();
    uint32_t groupIndex = TalentInfoGetScriptGroupIndex(L, 0, 0, 3);
    CGTalentGroup_C* group = TalentInfoGetTalentGroup(groupIndex, 0, 0);

    if (player && group) {
        int32_t slot = static_cast<int32_t>(lua_tonumber(L, 2));
        tooltip->SetGlyph(group->GetGlyph(slot - 1), 0, 0);
    }

    return 0;
}

// OFFSET: 0x0046DBE0/0x140083560
static int32_t Script_SetGlyphByID(lua_State* L) {
    CGTooltip* tooltip = static_cast<CGTooltip*>(FrameScript_GetObjectThis(L, CGTooltip::GetObjectType()));

    if (!lua_isnumber(L, 2)) {
        const char* name = tooltip->GetName();
        luaL_error(L, "Usage: %s:SetGlyphByID(glyphID)", name ? name : "<unnamed>");
        return 0;
    }

    tooltip->SetGlyph(lua_tointeger(L, 2), 1, 0);
    return 0;
}

// OFFSET: 0x0046DC60/0x140083610
static int32_t Script_SetLootCurrency(lua_State* L) {
    CGTooltip* tooltip = static_cast<CGTooltip*>(FrameScript_GetObjectThis(L, CGTooltip::GetObjectType()));
    int32_t currencyId;
    CurrencyTypesRec* currency;

    if (!lua_isnumber(L, 2) || (currencyId = CGLootInfo::GetLootCurrency(static_cast<uint32_t>(static_cast<int64_t>(lua_tonumber(L, 2) - 1.0)))) == 0 || (currency = g_currencyTypesDB.GetRecord(currencyId)) == NULL) {
        luaL_error(L, "Invalid loot slot in SetLootCurrency");
        return 0;
    }

    tooltip->SetCurrency(currency, WOWGUID());
    return 0;
}

// OFFSET: 0x0046DD20/0x1400836E0
static int32_t Script_SetQuestCurrency(lua_State* L) {
    CGTooltip* tooltip = static_cast<CGTooltip*>(FrameScript_GetObjectThis(L, CGTooltip::GetObjectType()));
    const char* type;
    uint32_t index;
    int32_t currencyId;

    if (!lua_isstring(L, 2) || !lua_isnumber(L, 3) || (type = lua_tolstring(L, 2, NULL), index = static_cast<int32_t>(lua_tonumber(L, 3)) - 1, currencyId = CGQuestInfo::GetQuestCurrencyID(type, index), currencyId <= 0)) {
        luaL_error(L, "Invalid quest item in SetQuestCurrency(\"type\", index)");
        return 0;
    }

    CurrencyTypesRec* currency = g_currencyTypesDB.GetRecord(currencyId);

    if (!currency) {
        return 0;
    }

    char name[256];
    char texture[260];
    uint32_t count;
    CGQuestInfo::GetQuestCurrencyInfo(type, index, name, sizeof(name), texture, sizeof(texture), count);
    tooltip->m_itemLinkInfo.Clear();
    tooltip->m_itemLinkInfo.m_stackCount = count;
    tooltip->SetCurrency(currency, WOWGUID());
    return 0;
}

// OFFSET: 0x0046DE30/0x140083850
static int32_t Script_SetQuestLogCurrency(lua_State* L) {
    int32_t currencyId = 0;
    CGTooltip* tooltip = static_cast<CGTooltip*>(FrameScript_GetObjectThis(L, CGTooltip::GetObjectType()));

    if (!lua_isstring(L, 2) || !lua_isnumber(L, 3)) {
        luaL_error(L, "Invalid quest item in SetQuestLogCurrency(\"type\", index)");
        return 0;
    }

    const char* type = lua_tolstring(L, 2, NULL);
    int32_t index = static_cast<int32_t>(lua_tonumber(L, 3)) - 1;
    uint32_t count = 0;
    WOWGUID requester = WOWGUID();
    QuestCache* quest = g_questDBCache.GetRecord(CGQuestLog::m_selectedQuest, requester, NULL, NULL, false);

    if (!quest || index < 0) {
        return 0;
    }

    if (!_stricmp(type, "reward") && static_cast<uint32_t>(index) < 4) {
        currencyId = quest->m_rewardCurrencyId[index];
        count = quest->m_rewardCurrencyCount[index];
    } else if (!_stricmp(type, "required") && static_cast<uint32_t>(index) < 4) {
        currencyId = quest->m_requiredCurrencyId[index];
        count = quest->m_requiredCurrencyCount[index];
    }

    if (CurrencyTypesRec* currency = g_currencyTypesDB.GetRecord(currencyId)) {
        tooltip->m_itemLinkInfo.Clear();
        tooltip->m_itemLinkInfo.m_stackCount = count;
        tooltip->SetCurrency(currency, WOWGUID());
    }

    return 0;
}

// OFFSET: 0x0046E330/0x140084010
static int32_t Script_SetUnitAura(lua_State* L) {
    CGTooltip* tooltip = static_cast<CGTooltip*>(FrameScript_GetObjectThis(L, CGTooltip::GetObjectType()));
    ScriptAuraInfo info;

    if (!Script_GetUnitAuraInfo(L, 2, &info)) {
        const char* name = tooltip->GetName();
        luaL_error(L, "Usage: %s:SetUnitAura(\"unit\", [index] or [\"name\", \"rank\"][, \"filter\"])", name ? name : "<unnamed>");
        return 0;
    }

    return SetTooltipUnitAura(tooltip, &info);
}

// OFFSET: 0x0046E3B0/0x1400840B0
static int32_t Script_SetUnitBuff(lua_State* L) {
    CGTooltip* tooltip = static_cast<CGTooltip*>(FrameScript_GetObjectThis(L, CGTooltip::GetObjectType()));
    ScriptAuraInfo info;

    if (!Script_GetUnitAuraInfo(L, 2, &info)) {
        const char* name = tooltip->GetName();
        luaL_error(L, "Usage: %s:SetUnitBuff(\"unit\", [index] or [\"name\", \"rank\"][, \"filter\"])", name ? name : "<unnamed>");
        return 0;
    }

    info.m_filter = (info.m_filter & 0xFC) | 1;
    return SetTooltipUnitAura(tooltip, &info);
}

// OFFSET: 0x0046E440/0x140084150
static int32_t Script_SetUnitDebuff(lua_State* L) {
    CGTooltip* tooltip = static_cast<CGTooltip*>(FrameScript_GetObjectThis(L, CGTooltip::GetObjectType()));
    ScriptAuraInfo info;

    if (!Script_GetUnitAuraInfo(L, 2, &info)) {
        const char* name = tooltip->GetName();
        luaL_error(L, "Usage: %s:SetUnitDebuff(\"unit\", [index] or [\"name\", \"rank\"][, \"filter\"])", name ? name : "<unnamed>");
        return 0;
    }

    info.m_filter = (info.m_filter & 0xFC) | 2;
    return SetTooltipUnitAura(tooltip, &info);
}

// OFFSET: 0x0046E4D0/0x1400841F0
static int32_t Script_SetCurrencyToken(lua_State* L) {
    CGTooltip* tooltip = static_cast<CGTooltip*>(FrameScript_GetObjectThis(L, CGTooltip::GetObjectType()));

    if (!lua_isnumber(L, 2)) {
        const char* name = tooltip->GetName();
        luaL_error(L, "Usage: %s:SetCurrencyToken(index)", name ? name : "<unnamed>");
        return 0;
    }

    CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();
    uint32_t index = static_cast<uint32_t>(static_cast<int64_t>(lua_tonumber(L, 2))) - 1;
    CGCurrencyTypes::CurrencyListInfo* info = CGCurrencyTypes::GetDisplayListInfo(index);
    const CurrencyTypesRec* currency = CGCurrencyTypes::GetCurrencyRec(index);

    if (tooltip && player && info && !info->m_isHeader && currency) {
        tooltip->SetCurrency(currency, WOWGUID());
    }

    return 0;
}

// OFFSET: 0x0046E5C0/0x140084300
static int32_t Script_SetBackpackToken(lua_State* L) {
    CGTooltip* tooltip = static_cast<CGTooltip*>(FrameScript_GetObjectThis(L, CGTooltip::GetObjectType()));

    if (!lua_isnumber(L, 2)) {
        const char* name = tooltip->GetName();
        luaL_error(L, "Usage: %s:SetBackpackToken(index)", name ? name : "<unnamed>");
        return 0;
    }

    CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();
    int32_t index = static_cast<int32_t>(static_cast<int64_t>(lua_tonumber(L, 2)));
    const CurrencyTypesRec* currency = CGCurrencyTypes::GetBackpackCurrencyInfo(index - 1);

    if (tooltip && player && currency) {
        tooltip->SetCurrency(currency, WOWGUID());
    }

    return 0;
}

// OFFSET: 0x0046E690/0x1400843F0
static int32_t Script_SetEquipmentSet(lua_State* L) {
    CGTooltip* tooltip = static_cast<CGTooltip*>(FrameScript_GetObjectThis(L, CGTooltip::GetObjectType()));

    if (!lua_isstring(L, 2)) {
        const char* name = tooltip->GetName();
        luaL_error(L, "Usage: %s:SetEquipmentSet(\"setName\")", name ? name : "<unnamed>");
        return 0;
    }

    if (EQUIPMENT_SET* set = CGEquipmentManager::GetSetByName(lua_tolstring(L, 2, NULL))) {
        tooltip->SetEquipmentSet(set->m_id);
    }

    return 0;
}

// OFFSET: 0x0046E720/0x1400844A0
static int32_t Script_SetFrameStack(lua_State* L) {
    CGTooltip* tooltip = static_cast<CGTooltip*>(FrameScript_GetObjectThis(L, CGTooltip::GetObjectType()));

    if (lua_toboolean(L, 2)) {
        tooltip->SetFrameStack(true);
    } else {
        tooltip->SetFrameStack(false);
    }

    return 0;
}

// OFFSET: 0x0046E780/0x140084510
static int32_t Script_SetInstanceLockEncountersComplete(lua_State* L) {
    CGTooltip* tooltip = static_cast<CGTooltip*>(FrameScript_GetObjectThis(L, CGTooltip::GetObjectType()));

    if (!lua_isnumber(L, 2)) {
        luaL_error(L, "Usage: Tooltip:SetInstanceLockEncountersComplete(index)");
        return 0;
    }

    uint32_t index = lua_tointeger(L, 2) - 1;

    if (index < s_savedInstances.m_count) {
        if (RaidInstanceInfo* instance = &s_savedInstances.m_data[index]) {
            tooltip->SetInstanceLockCompletedEncounters(ClntObjMgrGetActivePlayer(), instance->m_mapID, instance->m_difficulty + 2 * instance->m_heroic, instance->m_completedMask);
        }
    }

    return 0;
}

// OFFSET: 0x0046E820/0x1400845E0
static int32_t Script_SetCurrencyByID(lua_State* L) {
    CGTooltip* tooltip = static_cast<CGTooltip*>(FrameScript_GetObjectThis(L, CGTooltip::GetObjectType()));

    if (!lua_isnumber(L, 2)) {
        const char* name = tooltip->GetName();
        luaL_error(L, "Usage: %s:SetCurrencyByID(id)", name ? name : "<unnamed>");
        return 0;
    }

    CurrencyTypesRec* currency = g_currencyTypesDB.GetRecord(static_cast<int32_t>(static_cast<int64_t>(lua_tonumber(L, 2))));

    if (tooltip && currency) {
        tooltip->SetCurrency(currency, WOWGUID());
    }

    return 0;
}

// OFFSET: 0x0046F0C0/0x1400850A0
static void SetItemSpellCharges(int32_t itemId, TooltipExtendedItemInfo* info) {
    WOWGUID requester = WOWGUID();
    ItemRecSparse* item = g_itemSparseDB.GetRecord(itemId, requester, NULL, NULL, false);

    if (!item) {
        return;
    }

    for (uint32_t i = 0; i < 5; i++) {
        if (item->m_spellID[i] && !item->m_spellTrigger[i]) {
            info->m_spellCharges = item->m_spellCharges[i];
            return;
        }
    }
}

// OFFSET: 0x004730B0/0x14008B0A0
static int32_t Script_SetShapeshift(lua_State* L) {
    CGTooltip* tooltip = static_cast<CGTooltip*>(FrameScript_GetObjectThis(L, CGTooltip::GetObjectType()));

    if (!lua_isnumber(L, 2)) {
        const char* name = tooltip->GetName();
        luaL_error(L, "Usage: %s:SetShapeshift(slot)", name ? name : "<unnamed>");
        lua_pushnil(L);
        return 1;
    }

    int32_t slot = static_cast<int32_t>(static_cast<int64_t>(lua_tonumber(L, 2)));

    if (static_cast<uint32_t>(slot - 1) < s_shapeshiftSpells.m_count) {
        int32_t spellId = s_shapeshiftSpells.m_data[slot - 1].m_spellId;
        uint32_t start = 0;
        uint32_t duration = 0;
        uint32_t enable = 0;
        Spell_C_GetSpellCooldown(spellId, 0, &start, &duration, &enable, NULL);
        int32_t brief = 1;
        CVar* uberTooltips = CVar::Lookup("UberTooltips");

        if (uberTooltips && uberTooltips->m_intValue) {
            brief = 0;
        }

        int32_t result;

        if (enable && duration && start) {
            result = tooltip->SetSpell(spellId, brief, duration + start - FrameTime::GetCurTimeMs(), 0, 1, 0, 0, NULL, NULL, 0, 0, -1, -1, 0, 0, 0, 0);
        } else {
            result = tooltip->SetSpell(spellId, brief, 0, 0, 1, 0, 0, NULL, NULL, 0, 0, -1, -1, 0, 0, 0, 0);
        }

        if (result) {
            lua_pushnumber(L, 1.0);
            return 1;
        }
    }

    lua_pushnil(L);
    return 1;
}

// OFFSET: 0x00473230/0x14008B310
static int32_t Script_SetPossession(lua_State* L) {
    CGTooltip* tooltip = static_cast<CGTooltip*>(FrameScript_GetObjectThis(L, CGTooltip::GetObjectType()));

    if (!lua_isnumber(L, 2)) {
        const char* name = tooltip->GetName();
        luaL_error(L, "Usage: %s:SetPossession(slot)", name ? name : "<unnamed>");
        lua_pushnil(L);
        return 1;
    }

    int32_t slot = static_cast<int32_t>(static_cast<int64_t>(lua_tonumber(L, 2)));

    if (slot == 1) {
        uint32_t enable = 0;
        uint32_t duration = 0;
        uint32_t start = 0;
        Spell_C_GetSpellCooldown(CGPetInfo::m_possessSpell, 0, &start, &duration, &enable, NULL);
        int32_t brief = 1;
        CVar* uberTooltips = CVar::Lookup("UberTooltips");

        if (uberTooltips && uberTooltips->m_intValue) {
            brief = 0;
        }

        int32_t result;

        if (enable && duration && start) {
            result = tooltip->SetSpell(CGPetInfo::m_possessSpell, brief, duration + start - FrameTime::GetCurTimeMs(), 0, 1, 0, 0, NULL, NULL, 0, 0, -1, -1, 0, 0, 0, 0);
        } else {
            result = tooltip->SetSpell(CGPetInfo::m_possessSpell, brief, 0, 0, 1, 0, 0, NULL, NULL, 0, 0, -1, -1, 0, 0, 0, 0);
        }

        if (result) {
            lua_pushnumber(L, 1.0);
            return 1;
        }
    }

    lua_pushnil(L);
    return 1;
}

// OFFSET: 0x004733C0/0x14008B590
static int32_t Script_SetSpellByID(lua_State* L) {
    CGTooltip* tooltip = static_cast<CGTooltip*>(FrameScript_GetObjectThis(L, CGTooltip::GetObjectType()));
    int32_t spellId;

    if (!lua_isnumber(L, 2) || (spellId = lua_tointeger(L, 2), spellId < 0)) {
        const char* name = tooltip->GetName();
        luaL_error(L, "Invalid spell ID in %s:SetSpellByID", name ? name : "<unnamed>");
        return 0;
    }

    int32_t isPet = StringToBOOL(L, 3, 0);
    int32_t showSubtext = StringToBOOL(L, 4, 0);
    uint32_t duration = 0;
    uint32_t start = 0;
    uint32_t enable = 0;
    Spell_C_GetSpellCooldown(spellId, isPet, &start, &duration, &enable, NULL);
    int32_t result;

    if (enable && duration && start) {
        result = tooltip->SetSpell(spellId, 0, duration + start - FrameTime::GetCurTimeMs(), isPet, showSubtext, 0, 0, NULL, NULL, 0, 0, -1, -1, 0, 0, 0, 0);
    } else {
        result = tooltip->SetSpell(spellId, 0, 0, isPet, showSubtext, 0, 0, NULL, NULL, 0, 0, -1, -1, 0, 0, 0, 0);
    }

    if (result) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: 0x00473530/0x14008B820
static int32_t Script_AddSpellByID(lua_State* L) {
    CGTooltip* tooltip = static_cast<CGTooltip*>(FrameScript_GetObjectThis(L, CGTooltip::GetObjectType()));
    int32_t spellId;

    if (!lua_isnumber(L, 2) || (spellId = lua_tointeger(L, 2), spellId < 0)) {
        const char* name = tooltip->GetName();
        luaL_error(L, "Invalid spell ID in %s:SetSpellByID", name ? name : "<unnamed>");
        return 0;
    }

    int32_t isPet = StringToBOOL(L, 3, 0);
    int32_t showSubtext = StringToBOOL(L, 4, 0);
    uint32_t duration = 0;
    uint32_t start = 0;
    uint32_t enable = 0;
    Spell_C_GetSpellCooldown(spellId, isPet, &start, &duration, &enable, NULL);
    int32_t result;

    if (enable && duration && start) {
        result = tooltip->SetSpell(spellId, 0, duration + start - FrameTime::GetCurTimeMs(), isPet, showSubtext, 0, 0, NULL, NULL, 1, 0, -1, -1, 0, 0, 0, 0);
    } else {
        result = tooltip->SetSpell(spellId, 0, 0, isPet, showSubtext, 0, 0, NULL, NULL, 1, 0, -1, -1, 0, 0, 0, 0);
    }

    if (result) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: 0x004736A0/0x14008BAB0
static int32_t Script_SetUnit(lua_State* L) {
    CGTooltip* tooltip = static_cast<CGTooltip*>(FrameScript_GetObjectThis(L, CGTooltip::GetObjectType()));

    if (!lua_isstring(L, 2)) {
        const char* name = tooltip->GetName();
        luaL_error(L, "Usage: %s:SetUnit(\"unit\"[, hideStatus])", name ? name : "<unnamed>");
        return 0;
    }

    WOWGUID guid = Script_GetGUIDFromName(lua_tolstring(L, 2, NULL));
    int32_t hideStatus = StringToBOOL(L, 3, 0);

    if (guid.m_guid && guid.m_guid != 0xFFFFFFFFFFFFFFFEULL && tooltip->SetUnit(guid, 1, WOWGUID(), hideStatus)) {
        lua_pushnumber(L, 1.0);
        return 1;
    }

    lua_pushnil(L);
    return 1;
}

// OFFSET: 0x00473790/0x14008BBD0
static int32_t Script_SetQuestRewardSpell(lua_State* L) {
    CGTooltip* tooltip = static_cast<CGTooltip*>(FrameScript_GetObjectThis(L, CGTooltip::GetObjectType()));

    if (CGQuestInfo::m_rewards.m_spell) {
        tooltip->SetSpell(CGQuestInfo::m_rewards.m_spell, 0, 0, 0, 0, 0, 0, NULL, NULL, 0, 0, -1, -1, 0, 0, 0, 0);
    }

    return 0;
}

// OFFSET: 0x004737F0/0x14008BC70
static int32_t Script_SetQuestLogRewardSpell(lua_State* L) {
    CGTooltip* tooltip = static_cast<CGTooltip*>(FrameScript_GetObjectThis(L, CGTooltip::GetObjectType()));
    WOWGUID requester = WOWGUID();

    if (QuestCache* quest = g_questDBCache.GetRecord(CGQuestLog::m_selectedQuest, requester, NULL, NULL, false)) {
        if (int32_t spellId = quest->m_rewardSpellId) {
            tooltip->SetSpell(spellId, 0, 0, 0, 0, 0, 0, NULL, NULL, 0, 0, -1, -1, 0, 0, 0, 0);
        }
    }

    return 0;
}

// OFFSET: 0x00473880/0x14008BD50
static int32_t Script_GetItem(lua_State* L) {
    CGTooltip* tooltip = static_cast<CGTooltip*>(FrameScript_GetObjectThis(L, CGTooltip::GetObjectType()));

    if (!tooltip->m_itemId) {
        return 0;
    }

    CGItem_C* item = static_cast<CGItem_C*>(ClntObjMgrObjectPtr(tooltip->m_itemGuid, TYPEMASK_ITEM, __FILE__, __LINE__));
    TooltipExtendedItemInfo* info = tooltip->m_itemUseLinkInfo ? &tooltip->m_itemLinkInfo : NULL;
    int32_t randomPropertiesId = 0;
    bool wrapped = (item && (item->m_item->m_itemFieldFlags & 0x8)) || (info && info->m_wrapped);

    if (!wrapped) {
        if (info && info->m_randomPropertiesId) {
            randomPropertiesId = info->m_randomPropertiesId;
        } else if (item) {
            randomPropertiesId = item->m_item->m_itemFieldRandomPropertiesId;
        }
    }

    char name[1024];
    CGItem_C::BuildItemName(name, sizeof(name), tooltip->m_itemId, randomPropertiesId);
    lua_pushstring(L, name);

    if (item) {
        lua_pushstring(L, CGTooltip::GetItemHyperlinkString(item));
        return 2;
    }

    WOWGUID requester = WOWGUID();
    ItemRecSparse* rec = g_itemSparseDB.GetRecord(tooltip->m_itemId, requester, NULL, NULL, false);
    const char* link;

    if (!info || wrapped) {
        int32_t gems[3] = { 0, 0, 0 };
        link = CGTooltip::GetItemHyperlinkString(tooltip->m_itemId, rec ? rec->m_quality : 1, 0, gems, 0, 0, 0);
    } else {
        link = CGTooltip::GetItemHyperlinkString(tooltip->m_itemId, rec ? rec->m_quality : 1, info->m_enchants[0], &info->m_enchants[2], info->m_randomPropertiesId, info->m_propertySeed, info->m_enchants[8]);
    }

    lua_pushstring(L, link);
    return 2;
}

// OFFSET: 0x0047C5E0/0x140099500
static int32_t Script_SetHyperlink(lua_State* L) {
    CGTooltip* tooltip = static_cast<CGTooltip*>(FrameScript_GetObjectThis(L, CGTooltip::GetObjectType()));

    if (!lua_isstring(L, 2)) {
        const char* name = tooltip->GetName();
        luaL_error(L, "Usage: %s:SetHyperlink(link)", name ? name : "<unnamed>");
        return 0;
    }

    const char* link = lua_tolstring(L, 2, NULL);
    const char* cursor;

    if ((cursor = strstr(link, "item:")) != NULL) {
        int32_t itemId = SStrToInt(cursor + 5);

        if (tooltip->m_shown && itemId == tooltip->m_itemId) {
            tooltip->Hide();
            return 0;
        }

        if (itemId > 0) {
            tooltip->m_itemLinkInfo.Clear();
            tooltip->m_itemLinkInfo.GetItemIdFromLink(link);
            WOWGUID itemGuid = WOWGUID();
            WOWGUID requester = WOWGUID();
            tooltip->SetItem(itemId, requester, itemGuid, 0, 0, 1, 0, WOWGUID(), 0, NULL, 1, &tooltip->m_itemLinkInfo, 0, 0, 0, NULL);
            return 0;
        }

        return 0;
    }

    if ((cursor = strstr(link, "currency:")) != NULL) {
        int32_t currencyId = SStrToInt(cursor + 9);

        if (tooltip->m_shown && currencyId == tooltip->m_currencyId) {
            tooltip->Hide();
            return 0;
        }

        if (currencyId > 0) {
            if (CurrencyTypesRec* currency = g_currencyTypesDB.GetRecord(currencyId)) {
                tooltip->SetCurrency(currency, WOWGUID());
                return 0;
            }
        }

        return 0;
    }

    if ((cursor = strstr(link, "enchant:")) != NULL) {
        int32_t spellId = SStrToInt(cursor + 8);

        if (tooltip->m_shown && spellId == tooltip->m_spellId) {
            tooltip->Hide();
            return 0;
        }

        SpellRec* spell = g_spellDB.GetRecord(spellId);

        if (spell && (spell->m_attributes & 0x20)) {
            tooltip->SetSpell(spellId, 0, 0, 0, 0, 0, 0, NULL, NULL, 0, 0, -1, -1, 0, 0, 0, 0);
            return 0;
        }

        return 0;
    }

    if ((cursor = strstr(link, "dance:")) != NULL) {
        CGDanceStudio::LoadDance(SStrToInt(cursor + 6));
        return 0;
    }

    if ((cursor = strstr(link, "spell:")) != NULL) {
        int32_t spellId = SStrToInt(cursor + 6);

        if (tooltip->m_shown && spellId == tooltip->m_spellId) {
            tooltip->Hide();
            return 0;
        }

        tooltip->SetSpell(spellId, 0, 0, 0, 0, 0, 1, NULL, NULL, 0, 0, -1, -1, 0, 0, 0, 0);
        return 0;
    }

    if ((cursor = strstr(link, "unit:")) != NULL) {
        WOWGUID guid = StringToGUID(cursor + 5);

        if (tooltip->m_shown && guid == tooltip->m_unitGuid) {
            tooltip->Hide();
            return 0;
        }

        tooltip->SetUnit(guid, 0, WOWGUID(), 0);
        return 0;
    }

    if ((cursor = strstr(link, "quest:")) != NULL) {
        int32_t questId = SStrToInt(cursor + 6);

        if (tooltip->m_shown && questId == tooltip->m_questId) {
            tooltip->Hide();
            return 0;
        }

        tooltip->SetQuest(questId);
        return 0;
    }

    if ((cursor = strstr(link, "talent:")) != NULL) {
        cursor += 7;
        TalentRec* talent = g_talentDB.GetRecord(SStrToInt(cursor));

        if (!talent) {
            return 0;
        }

        int32_t rank = -1;
        const char* field = strchr(cursor, ':');

        if (field) {
            rank = SStrToInt(field + 1);
        }

        tooltip->SetTalent(talent, NULL, 0, 1, rank, 0, 0);
        return 0;
    }

    if ((cursor = strstr(link, "trade:")) != NULL) {
        CGTradeSkillInfo::SetLink(cursor + 6);
        return 0;
    }

    if ((cursor = strstr(link, "achievement:")) != NULL) {
        cursor += 12;
        int32_t achievementId = SStrToInt(cursor);

        if (tooltip->m_shown && achievementId == tooltip->m_achievementId) {
            tooltip->Hide();
            return 0;
        }

        WOWGUID guid = WOWGUID();
        WowTime time;
        int32_t completed = 0;
        uint32_t criteria1 = 0;
        uint32_t criteria2 = 0;
        uint32_t criteria3 = 0;
        uint32_t criteria4 = 0;
        const char* field = strchr(cursor, ':');

        if (field) {
            field++;
            sscanf(field, "%llX:", &guid.m_guid);
        }

        if ((field = strchr(field, ':')) != NULL) {
            completed = SStrToInt(++field);
        }

        if ((field = strchr(field, ':')) != NULL) {
            time.m_month = SStrToInt(++field);
        }

        if ((field = strchr(field, ':')) != NULL) {
            time.m_monthDay = SStrToInt(++field);
        }

        if ((field = strchr(field, ':')) != NULL) {
            time.m_year = SStrToInt(++field);
        }

        if ((field = strchr(field, ':')) != NULL) {
            criteria1 = SStrToUnsigned(++field);
        }

        if ((field = strchr(field, ':')) != NULL) {
            criteria2 = SStrToUnsigned(++field);
        }

        if ((field = strchr(field, ':')) != NULL) {
            criteria3 = SStrToUnsigned(++field);
        }

        if ((field = strchr(field, ':')) != NULL) {
            criteria4 = SStrToUnsigned(field + 1);
        }

        tooltip->SetAchievement(achievementId, guid, completed != 0, time, criteria1, criteria2, criteria3, criteria4);
        return 0;
    }

    uint32_t difficulty = 0;

    if ((cursor = strstr(link, "glyph:")) != NULL) {
        tooltip->SetGlyph(SStrToInt(cursor + 6), 1, 0);
        return 0;
    }

    if ((cursor = strstr(link, "instancelock:")) == NULL) {
        luaL_error(L, "%s:SetHyperlink(): Unknown link type", tooltip->GetDisplayName());
        return 0;
    }

    cursor += 13;
    WOWGUID guid = StringToGUID(cursor);
    int32_t mapId = 0;
    uint32_t encounterMask = 0;
    const char* field = strchr(cursor, ':');

    if (field) {
        field++;
        mapId = SStrToInt(field);
    }

    if ((field = strchr(field, ':')) != NULL) {
        field++;
        difficulty = SStrToInt(field);
    }

    if ((field = strchr(field, ':')) != NULL) {
        encounterMask = SStrToInt(field + 1);
    }

    tooltip->SetInstanceLockCompletedEncounters(guid, mapId, difficulty, encounterMask);
    return 0;
}

// OFFSET: 0x0047CC50/0x140099D80
static int32_t Script_SetPetAction(lua_State* L) {
    CGTooltip* tooltip = static_cast<CGTooltip*>(FrameScript_GetObjectThis(L, CGTooltip::GetObjectType()));

    if (!lua_isnumber(L, 2)) {
        const char* name = tooltip->GetName();
        luaL_error(L, "Usage: %s:SetPetAction(slot)", name ? name : "<unnamed>");
        lua_pushnil(L);
        return 1;
    }

    uint32_t slot = static_cast<int32_t>(lua_tonumber(L, 2)) - 1;
    PetAction* action;

    if (CGPetInfo::GetPet(0) && slot < 10 && (action = &CGPetInfo::m_actions[slot]) != NULL && action->m_data && tooltip->SetPetAction(CGPetInfo::m_actions[slot])) {
        lua_pushnumber(L, 1.0);
        return 1;
    }

    lua_pushnil(L);
    return 1;
}

// OFFSET: 0x0047CD20/0x140099E70
static int32_t Script_SetSpellBookItem(lua_State* L) {
    CGTooltip* tooltip = static_cast<CGTooltip*>(FrameScript_GetObjectThis(L, CGTooltip::GetObjectType()));
    uint32_t slot;

    if (!lua_isnumber(L, 2) || !lua_isstring(L, 3) || (slot = static_cast<int32_t>(lua_tonumber(L, 2) - 1.0), slot > 0x3FF)) {
        const char* name = tooltip->GetName();
        luaL_error(L, "Invalid spell slot in %s:SetSpell", name ? name : "<unnamed>");
        return 0;
    }

    int32_t petSpell = 0;

    if (!_stricmp("pet", lua_tolstring(L, 3, NULL))) {
        petSpell = 1;
    }

    SpellBookItem* item = CGSpellBook::GetSpellBookItem(slot, petSpell);
    int32_t result;

    if (!item) {
        lua_pushnil(L);
        return 1;
    }

    switch (item->m_type) {
        case 1:
        case 2: {
            if (item->m_id <= 0) {
                lua_pushnil(L);
                return 1;
            }

            int32_t spellId = CGSpellBook::GetOverrideSpell(item->m_id);
            uint32_t enable = 0;
            uint32_t duration = 0;
            uint32_t start = 0;
            Spell_C_GetSpellCooldown(spellId, petSpell, &start, &duration, &enable, NULL);

            if (enable && duration && start) {
                result = tooltip->SetSpell(spellId, 0, duration + start - FrameTime::GetCurTimeMs(), petSpell, 0, 0, 0, NULL, NULL, 0, 0, -1, -1, 0, 0, 0, 0);
            } else {
                result = tooltip->SetSpell(spellId, 0, 0, petSpell, 0, 0, 0, NULL, NULL, 0, 0, -1, -1, 0, 0, 0, 0);
            }

            break;
        }
        case 3: {
            if (!item->m_id) {
                lua_pushnil(L);
                return 1;
            }

            PetAction action;
            action.m_data = item->m_id;
            result = tooltip->SetPetAction(action);
            break;
        }
        case 4:
            tooltip->SetSpellFlyout(item->m_id, true);
            lua_pushnil(L);
            return 1;
        default:
            lua_pushnil(L);
            return 1;
    }

    if (result) {
        lua_pushnumber(L, 1.0);
        return 1;
    }

    lua_pushnil(L);
    return 1;
}

// OFFSET: 0x0047CF20/0x14009A180
static int32_t Script_SetInventoryItem(lua_State* L) {
    CGTooltip* tooltip = static_cast<CGTooltip*>(FrameScript_GetObjectThis(L, CGTooltip::GetObjectType()));

    if (!lua_isstring(L, 2)) {
        const char* name = tooltip->GetName();
        luaL_error(L, "Usage: %s:SetInventoryItem(unit, slot [, nameOnly])", name ? name : "<unnamed>");
        return 0;
    }

    int32_t slot;

    if (!lua_isnumber(L, 3) || (slot = static_cast<int32_t>(lua_tonumber(L, 3) - 1.0), static_cast<uint32_t>(slot) >= 23) && (slot < 39 || slot > 66) && static_cast<uint32_t>(slot - 67) > 6) {
        luaL_error(L, "Invalid inventory slot in SetInventoryItem");
        return 0;
    }

    int32_t nameOnly = StringToBOOL(L, 4, 0);
    CGPlayer_C* unit = static_cast<CGPlayer_C*>(Script_GetUnitFromName(lua_tolstring(L, 2, NULL)));

    if (!CGCharacterInfo::CanInspect(unit)) {
        lua_pushnil(L);
        lua_pushnil(L);
        lua_pushnumber(L, 0.0);
        return 3;
    }

    CGItem_C* item = unit->m_bag.GetItemPointer(slot);

    if (!item) {
        InspectItem* inspected = unit->GetInspectedItem(slot);

        if (!inspected || !inspected->m_itemId) {
            lua_pushnil(L);
            lua_pushnil(L);
            lua_pushnumber(L, 0.0);
            return 3;
        }

        WOWGUID query = WOWGUID();
        g_itemSparseDB.GetRecord(inspected->m_itemId, query, TooltipItemCallback, tooltip, false);
        TooltipExtendedItemInfo& info = tooltip->m_itemLinkInfo;
        info.Clear();
        info.m_randomPropertiesId = inspected->m_randomPropertiesId;

        for (uint32_t i = 0; i < 15; i++) {
            info.m_enchants[i] = inspected->m_enchants[i];
            info.m_enchantDurations[i] = 0;
            info.m_enchantCharges[i] = 0;
        }

        info.m_propertySeed = inspected->m_propertySeed;
        info.m_creator = inspected->m_creator;
        WOWGUID itemGuid = WOWGUID();
        WOWGUID requester = unit->m_obj->m_guid;
        tooltip->SetItem(inspected->m_itemId, requester, itemGuid, nameOnly, 0, 1, 0, unit->m_obj->m_guid, 0, NULL, 0, NULL, 0, 0, 0, NULL);
        lua_pushnumber(L, 1.0);
        lua_pushnil(L);
        lua_pushnumber(L, 0.0);
        return 3;
    }

    int32_t result;

    if (!item->GetUsableSpellCharges() || item->GetCurrentSpellCharges()) {
        uint32_t start = 0;
        uint32_t duration = 0;
        uint32_t enable = 0;
        Spell_C_GetItemCooldown(item, &start, &duration, &enable);

        if (enable && start && duration) {
            tooltip->m_itemLinkInfo.Clear();
            tooltip->m_itemLinkInfo.m_cooldown = start + duration - FrameTime::GetCurTimeMs();
            tooltip->m_itemLinkInfo.m_spellCharges = item->GetCurrentSpellCharges();
            WOWGUID itemGuid = item->m_obj->m_guid;
            WOWGUID requester = unit->m_obj->m_guid;
            result = tooltip->SetItem(item->m_obj->m_entryID, requester, itemGuid, nameOnly, 0, 1, 0, unit->m_obj->m_guid, 0, NULL, 0, NULL, 0, 0, 0, NULL);
        } else {
            WOWGUID itemGuid = item->m_obj->m_guid;
            WOWGUID requester = unit->m_obj->m_guid;
            result = tooltip->SetItem(item->m_obj->m_entryID, requester, itemGuid, nameOnly, 0, 0, 0, unit->m_obj->m_guid, 0, NULL, 0, NULL, 0, 0, 0, NULL);
        }
    } else {
        WOWGUID itemGuid = item->m_obj->m_guid;
        WOWGUID requester = unit->m_obj->m_guid;
        result = tooltip->SetItem(item->m_obj->m_entryID, requester, itemGuid, nameOnly, 0, 0, 0, unit->m_obj->m_guid, 0, NULL, 0, NULL, 0, 0, 0, NULL);
    }

    lua_pushnumber(L, 1.0);

    if (result) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    lua_pushnumber(L, static_cast<uint32_t>(CGMerchantInfo::GetRepairCost(item)));
    return 3;
}

// OFFSET: 0x0047D330/0x14009A6F0
static int32_t Script_SetLootItem(lua_State* L) {
    CGTooltip* tooltip = static_cast<CGTooltip*>(FrameScript_GetObjectThis(L, CGTooltip::GetObjectType()));

    if (!lua_isnumber(L, 2)) {
        luaL_error(L, "Invalid loot slot in SetLootItem");
        return 0;
    }

    uint32_t slot = static_cast<uint32_t>(static_cast<int64_t>(lua_tonumber(L, 2) - 1.0));
    int32_t itemId = CGLootInfo::GetLootItem(slot);

    if (itemId > 0) {
        TooltipExtendedItemInfo* info = &tooltip->m_itemLinkInfo;
        info->Clear();
        info->m_propertySeed = CGLootInfo::GetLootSeed(slot);
        info->m_randomPropertiesId = CGLootInfo::GetLootRandomPropertiesID(slot);
        info->m_stackCount = CGLootInfo::GetLootQuantity(slot);
        SetItemSpellCharges(itemId, info);
        WOWGUID itemGuid = WOWGUID();
        tooltip->SetItem(itemId, CGLootInfo::m_lootObj, itemGuid, 0, 0, 1, 0, WOWGUID(), 0, NULL, 0, NULL, 0, 0, 0, NULL);
    }

    return 0;
}

// OFFSET: 0x0047D440/0x14009A870
static int32_t Script_SetQuestItem(lua_State* L) {
    CGTooltip* tooltip = static_cast<CGTooltip*>(FrameScript_GetObjectThis(L, CGTooltip::GetObjectType()));
    const char* type;
    uint32_t index;
    int32_t itemId;

    if (!lua_isstring(L, 2) || !lua_isnumber(L, 3) || (type = lua_tolstring(L, 2, NULL), index = static_cast<int32_t>(lua_tonumber(L, 3)) - 1, itemId = CGQuestInfo::GetQuestItemID(type, index), itemId <= 0)) {
        luaL_error(L, "Invalid quest item in SetQuestItem(\"type\", index)");
        return 0;
    }

    char name[256];
    char texture[260];
    uint32_t count;
    int32_t quality;
    int32_t usable;
    CGQuestInfo::GetQuestItemInfo(type, index, name, sizeof(name), texture, sizeof(texture), count, quality, usable);
    TooltipExtendedItemInfo* info = &tooltip->m_itemLinkInfo;
    info->Clear();
    info->m_stackCount = count;
    SetItemSpellCharges(itemId, info);
    WOWGUID itemGuid = WOWGUID();
    tooltip->SetItem(itemId, CGQuestInfo::m_npc, itemGuid, 0, 0, 1, 0, WOWGUID(), 0, NULL, 0, NULL, 0, 0, 0, NULL);
    return 0;
}

// OFFSET: 0x0047D570/0x14009AA40
static int32_t Script_SetQuestLogItem(lua_State* L) {
    CGTooltip* tooltip = static_cast<CGTooltip*>(FrameScript_GetObjectThis(L, CGTooltip::GetObjectType()));
    const char* type;
    int32_t index;
    int32_t itemId;

    if (!lua_isstring(L, 2) || !lua_isnumber(L, 3) || (type = lua_tolstring(L, 2, NULL), index = static_cast<int32_t>(lua_tonumber(L, 3)) - 1, itemId = CGQuestLog::GetQuestItemID(type, index), itemId <= 0)) {
        luaL_error(L, "Invalid quest item in SetQuestLogItem(\"type\", index)");
        return 0;
    }

    TooltipExtendedItemInfo* info = &tooltip->m_itemLinkInfo;
    info->Clear();
    WOWGUID requester = WOWGUID();
    QuestCache* quest = g_questDBCache.GetRecord(CGQuestLog::m_selectedQuest, requester, NULL, NULL, false);

    if (quest && index >= 0) {
        if (!_stricmp(type, "reward") && static_cast<uint32_t>(index) < 4) {
            info->m_stackCount = quest->m_rewardItemCount[index];
        } else if (!_stricmp(type, "choice") && static_cast<uint32_t>(index) < 6) {
            info->m_stackCount = quest->m_rewardChoiceItemCount[index];
        }
    }

    SetItemSpellCharges(itemId, info);
    WOWGUID itemGuid = WOWGUID();
    WOWGUID owner = WOWGUID();
    tooltip->SetItem(itemId, owner, itemGuid, 0, 0, 1, 0, WOWGUID(), 0, NULL, 0, NULL, 0, 0, 0, NULL);
    return 0;
}

// OFFSET: 0x0047D6E0/0x14009AC70
static int32_t Script_SetTrainerService(lua_State* L) {
    CGTooltip* tooltip = static_cast<CGTooltip*>(FrameScript_GetObjectThis(L, CGTooltip::GetObjectType()));
    TrainerServiceInfo* service;
    const TSGrowableArray<const SpellEffectRec*>* effects;
    uint32_t count;

    if (!lua_isnumber(L, 2) || (service = CGClassTrainer::GetService(static_cast<int32_t>(lua_tonumber(L, 2)) - 1)) == NULL || (effects = SpellGetEffects(service->m_spellID, count), !count)) {
        luaL_error(L, "Invalid trainer service in SetTrainerService(index)");
        return 0;
    }

    for (uint32_t i = 0; i < count; i++) {
        const SpellEffectRec* effect = effects->m_data[i];

        if (!effect) {
            continue;
        }

        SpellRec* learned;

        if ((effect->m_effect != 36 && effect->m_effect != 57) || (learned = g_spellDB.GetRecord(effect->m_effectTriggerSpell)) == NULL) {
            continue;
        }

        const SpellEffectRec* first = SpellGetEffect(learned->m_id, 0);
        int32_t itemType;

        if ((learned->m_attributes & 0x20) && first && (first->m_effect == 24 || first->m_effect == 157 || first->m_effect == 59) && (itemType = first->m_effectItemType) != 0) {
            WOWGUID requester;
            requester.m_guid = static_cast<uint64_t>(static_cast<int64_t>(learned->m_id)) | 0x1FE0000000000000ULL;
            WOWGUID itemGuid = WOWGUID();
            tooltip->SetItem(itemType, requester, itemGuid, 0, 0, 0, 0, WOWGUID(), 0, NULL, 0, NULL, 0, 0, 1, NULL);
            return 0;
        }

        tooltip->SetSpell(learned->m_id, 0, 0, effects->m_data[i]->m_effect == 57, 0, 0, 0, NULL, NULL, 0, 0, -1, -1, 0, 0, 0, 0);
        return 0;
    }

    tooltip->SetSpell(service->m_spellID, 0, 0, 0, 0, 0, 0, NULL, NULL, 0, 0, -1, -1, 0, 0, 0, 0);
    return 0;
}

// OFFSET: 0x0047D8B0/0x14009AF70
static int32_t Script_SetTradeSkillItem(lua_State* L) {
    CGTooltip* tooltip = static_cast<CGTooltip*>(FrameScript_GetObjectThis(L, CGTooltip::GetObjectType()));
    int32_t index;
    TradeSkillInfo* recipe;
    SpellRec* spell;

    if (!lua_isnumber(L, 2) || (index = static_cast<int32_t>(lua_tonumber(L, 2)) - 1, static_cast<uint32_t>(index) >= CGTradeSkillInfo::m_numDisplayEntries) || (recipe = CGTradeSkillInfo::m_displayList.m_data[index]) == NULL || (spell = g_spellDB.GetRecord(recipe->m_spellId)) == NULL) {
        luaL_error(L, "Invalid trade skill item in SetTradeSkillItem(index [,reagent])");
        return 0;
    }

    if (!lua_isnumber(L, 3)) {
        const SpellEffectRec* effect = SpellGetEffect(spell->m_id, 0);

        if (effect && (effect->m_effect == 24 || effect->m_effect == 157 || effect->m_effect == 59) && effect->m_effectItemType) {
            int32_t minPoints;
            int32_t maxPoints;
            Spell_C_GetMinMaxPoints(spell, 0, &minPoints, &maxPoints, 0, 0, 0, 0);
            TooltipExtendedItemInfo* info = &tooltip->m_itemLinkInfo;
            info->Clear();
            info->m_stackCount = minPoints;
            info->m_maxStackCount = maxPoints;

            if (effect->m_effect == 24) {
                info->m_spellCharges = CGItem_C::NumBaseCharges(effect->m_effectItemType);
            }

            WOWGUID requester;
            requester.m_guid = static_cast<uint64_t>(static_cast<int64_t>(recipe->m_spellId)) | 0x1FE0000000000000ULL;
            WOWGUID itemGuid = WOWGUID();
            tooltip->SetItem(effect->m_effectItemType, requester, itemGuid, 0, 0, 0, 0, WOWGUID(), 0, NULL, 0, NULL, 0, 0, 0, NULL);
            return 0;
        }

        tooltip->SetSpell(recipe->m_spellId, 0, 0, 0, 0, 0, 0, NULL, NULL, 0, 0, -1, -1, 0, 0, 0, 0);
        return 0;
    }

    SpellReagentsRec* reagents;

    if (!spell->m_spellReagentsID || (reagents = g_spellReagentsDB.GetRecord(spell->m_spellReagentsID)) == NULL) {
        luaL_error(L, "Invalid trade skill item in SetTradeSkillItem(index [,reagent])");
        return 0;
    }

    int32_t reagent = static_cast<int32_t>(lua_tonumber(L, 3));
    int32_t found = 0;
    uint32_t i;

    for (i = 0; ; i++) {
        if (reagents->m_reagent[i] && ++found == reagent) {
            break;
        }

        if (i + 1 >= 8) {
            luaL_error(L, "Invalid trade skill item in SetTradeSkillItem(index [,reagent])");
            return 0;
        }
    }

    tooltip->m_itemLinkInfo.Clear();
    tooltip->m_itemLinkInfo.m_stackCount = reagents->m_reagentCount[i];
    WOWGUID requester;
    requester.m_guid = static_cast<uint64_t>(static_cast<int64_t>(recipe->m_spellId)) | 0x1FE0000000000000ULL;
    WOWGUID itemGuid = WOWGUID();
    tooltip->SetItem(reagents->m_reagent[i], requester, itemGuid, 0, 0, 0, 0, WOWGUID(), 0, NULL, 0, NULL, 0, 0, 0, NULL);
    return 0;
}

// OFFSET: 0x0047DB30/0x14009B310
static int32_t Script_SetMerchantItem(lua_State* L) {
    CGTooltip* tooltip = static_cast<CGTooltip*>(FrameScript_GetObjectThis(L, CGTooltip::GetObjectType()));

    if (!lua_isnumber(L, 2)) {
        luaL_error(L, "Invalid merchant slot in SetMerchantItem");
        return 0;
    }

    int32_t index = static_cast<int32_t>(static_cast<int64_t>(lua_tonumber(L, 2) - 1.0));

    if (index < 0 || index >= CGMerchantInfo::m_numMerchantItems) {
        return 0;
    }

    WOWGUID merchant = CGMerchantInfo::m_merchant;

    if (!merchant.m_guid) {
        return 0;
    }

    VendorItem* item = &CGMerchantInfo::m_items[index];

    if (!item) {
        return 0;
    }

    if (item->m_type == VENDOR_ITEM_TYPE_ITEM && item->m_itemID > 0) {
        WOWGUID itemGuid = WOWGUID();
        WOWGUID requester = CGMerchantInfo::m_merchant;
        tooltip->SetItem(item->m_itemID, requester, itemGuid, 0, 0, 0, 0, WOWGUID(), 0, NULL, 0, NULL, 0, 0, 1, NULL);
        return 0;
    }

    if (item->m_type == VENDOR_ITEM_TYPE_CURRENCY && item->m_itemID) {
        if (CurrencyTypesRec* currency = g_currencyTypesDB.GetRecord(item->m_itemID)) {
            tooltip->SetCurrency(currency, merchant);
        }
    }

    return 0;
}

// OFFSET: 0x0047DC60/0x14009B4B0
static int32_t Script_SetMerchantCostItem(lua_State* L) {
    int32_t found = 0;
    CGTooltip* tooltip = static_cast<CGTooltip*>(FrameScript_GetObjectThis(L, CGTooltip::GetObjectType()));

    if (!lua_isnumber(L, 2) || !lua_isnumber(L, 3)) {
        luaL_error(L, "Invalid merchant slot in SetMerchantCostItem");
        return 0;
    }

    int32_t index = static_cast<int32_t>(static_cast<int64_t>(lua_tonumber(L, 2) - 1.0));
    int32_t costIndex = static_cast<int32_t>(static_cast<int64_t>(lua_tonumber(L, 3) - 1.0));

    if (index < 0 || index >= CGMerchantInfo::m_numMerchantItems || !CGMerchantInfo::m_merchant.m_guid) {
        return 0;
    }

    VendorItem* item = &CGMerchantInfo::m_items[index];

    if (!item || !item->m_itemID) {
        return 0;
    }

    WOWGUID requester = WOWGUID();
    ItemExtendedCostRec* cost = g_itemExtendedCostDB.GetRecord(item->m_extendedCostID, requester, NULL, NULL, false);

    if (!cost) {
        return 0;
    }

    for (uint32_t i = 0; i < 5; i++) {
        if (!cost->m_itemID[i]) {
            continue;
        }

        WOWGUID query = WOWGUID();
        ItemRecSparse* sparse = g_itemSparseDB.GetRecord(cost->m_itemID[i], query, NULL, NULL, false);

        if (!sparse) {
            continue;
        }

        ItemRec* rec = sparse->GetNonSparse();

        if (!rec || rec->m_classID == 10) {
            continue;
        }

        if (found == costIndex) {
            WOWGUID merchant = CGMerchantInfo::m_merchant;
            WOWGUID itemGuid = WOWGUID();
            tooltip->SetItem(cost->m_itemID[i], merchant, itemGuid, 0, 0, 0, 0, WOWGUID(), 0, NULL, 0, NULL, 0, 0, 1, NULL);
            return 0;
        }

        found++;
    }

    for (uint32_t i = 0; i < 5; i++) {
        if (!cost->m_currencyID[i]) {
            continue;
        }

        if (found != costIndex) {
            found++;
            continue;
        }

        tooltip->SetCurrency(g_currencyTypesDB.GetRecord(cost->m_currencyID[i]), WOWGUID());
        return 0;
    }

    return 0;
}

// OFFSET: 0x0047DEA0/0x14009B770
static int32_t Script_SetTradePlayerItem(lua_State* L) {
    CGTooltip* tooltip = static_cast<CGTooltip*>(FrameScript_GetObjectThis(L, CGTooltip::GetObjectType()));

    if (!lua_isnumber(L, 2)) {
        luaL_error(L, "Invalid trade slot in SetTradePlayerItem");
        return 0;
    }

    uint32_t slot = static_cast<uint32_t>(static_cast<int64_t>(lua_tonumber(L, 2) - 1.0));
    WOWGUID itemGuid = slot > 6 ? WOWGUID() : CGTradeInfo::m_playerTrade.m_items[slot].m_item;
    CGItem_C* item = static_cast<CGItem_C*>(ClntObjMgrObjectPtr(itemGuid, TYPEMASK_ITEM, __FILE__, __LINE__));

    if (!item) {
        return 0;
    }

    TooltipExtendedItemInfo* info = &tooltip->m_itemLinkInfo;
    info->Clear();
    memset(info->m_enchants, 0, sizeof(info->m_enchants));
    memset(info->m_enchantDurations, 0, sizeof(info->m_enchantDurations));
    memset(info->m_enchantCharges, 0, sizeof(info->m_enchantCharges));
    info->m_enchants[8] = (item->GetItemStaticFlags(0) & 0x2000) ? 0 : item->m_item->m_itemFieldEnchantment[8].m_id;
    info->m_cooldown = 0;
    info->m_creator = item->m_item->m_itemFieldCreator;
    info->m_wrapped = (item->m_item->m_itemFieldFlags >> 3) & 1;
    info->m_giftCreator = item->m_item->m_itemFieldGiftcreator;
    int32_t useSlot = item->GetOnUseSlot();

    if (useSlot >= 0) {
        info->m_spellCharges = item->m_item->m_itemFieldSpellCharges[useSlot];
    }

    info->m_proposedEnchant = slot != 6 ? 0 : CGTradeInfo::m_targetTrade.m_proposedEnchant;
    WOWGUID guid = item->m_obj->m_guid;
    WOWGUID requester = item->m_obj->m_guid;
    tooltip->SetItem(item->m_obj->m_entryID, requester, guid, 0, 0, 1, 0, WOWGUID(), 0, NULL, useSlot < 0, NULL, 0, 0, 0, NULL);
    return 0;
}

// OFFSET: 0x0047E0A0/0x14009BA00
static int32_t Script_SetTradeTargetItem(lua_State* L) {
    CGTooltip* tooltip = static_cast<CGTooltip*>(FrameScript_GetObjectThis(L, CGTooltip::GetObjectType()));

    if (!lua_isnumber(L, 2)) {
        luaL_error(L, "Invalid trade slot in SetTradeTargetItem");
        return 0;
    }

    uint32_t slot = static_cast<uint32_t>(static_cast<int64_t>(lua_tonumber(L, 2) - 1.0));

    if (slot > 6) {
        return 0;
    }

    TradeTargetItem* trade = &CGTradeInfo::m_targetTrade.m_items[slot];
    int32_t itemId = trade->m_itemID;

    if (itemId <= 0) {
        return 0;
    }

    TooltipExtendedItemInfo* info = &tooltip->m_itemLinkInfo;
    info->Clear();
    memset(info->m_enchants, 0, sizeof(info->m_enchants));
    memset(info->m_enchantDurations, 0, sizeof(info->m_enchantDurations));
    memset(info->m_enchantCharges, 0, sizeof(info->m_enchantCharges));
    info->m_enchants[0] = trade->m_enchant;
    info->m_enchants[8] = trade->m_reforgeEnchant;
    info->m_enchants[7] = trade->m_engineeringEnchant;

    for (uint32_t i = 0; i < 3; i++) {
        info->m_enchants[2 + i] = slot < 7 && i < 3 ? CGTradeInfo::m_targetTrade.m_items[slot].m_socketEnchants[i] : 0;
        info->m_enchantDurations[2 + i] = 0;
        info->m_enchantCharges[2 + i] = 0;
    }

    info->m_proposedEnchant = 0;
    info->m_cooldown = 0;
    info->m_creator = trade->m_creator;
    info->m_wrapped = trade->m_wrapped;
    info->m_lockId = trade->m_locked;
    info->m_giftCreator = trade->m_giftCreator;
    info->m_propertySeed = trade->m_suffixFactor;
    info->m_randomPropertiesId = trade->m_randomPropertyID;
    info->m_spellCharges = trade->m_spellCharges;
    info->m_proposedEnchant = slot != 6 ? 0 : CGTradeInfo::m_playerTrade.m_proposedEnchant;
    info->m_maxDurability = trade->m_maxDurability;
    info->m_durability = trade->m_durability;
    info->m_stackCount = trade->m_count;
    WOWGUID requester = CGTradeInfo::m_tradingPlayer;
    WOWGUID itemGuid = WOWGUID();
    tooltip->SetItem(itemId, requester, itemGuid, 0, 0, 1, 0, WOWGUID(), 0, NULL, 0, NULL, 0, trade->m_locked == 0, 0, NULL);
    return 0;
}

// OFFSET: 0x0047E2E0/0x14009BCE0
static int32_t Script_SetBagItem(lua_State* L) {
    CGTooltip* tooltip = static_cast<CGTooltip*>(FrameScript_GetObjectThis(L, CGTooltip::GetObjectType()));
    CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();

    if (!player || !lua_isnumber(L, 2) || !lua_isnumber(L, 3)) {
        return 0;
    }

    int32_t bag = static_cast<int32_t>(lua_tonumber(L, 2));
    CGBag_C* container;

    if (bag) {
        if (static_cast<uint32_t>(bag - 1) > 10) {
            return 0;
        }

        CGContainer_C* object = static_cast<CGContainer_C*>(ClntObjMgrObjectPtr(CGContainerInfo::GetContainer(bag - 1), TYPEMASK_CONTAINER, __FILE__, __LINE__));

        if (!object) {
            return 0;
        }

        container = object->GetBag();
    } else {
        container = &player->m_bag;
    }

    if (!container) {
        return 0;
    }

    int32_t slot = static_cast<int32_t>(lua_tonumber(L, 3)) - 1;

    if (container == &player->m_bag) {
        slot += 23;
    }

    CGItem_C* item;

    if (slot < 0 || slot >= static_cast<int32_t>(container->m_numSlots) || (item = container->GetItemPointer(slot)) == NULL) {
        lua_pushnil(L);
        return 1;
    }

    int32_t result;

    if (item->GetUsableSpellCharges() && !item->GetCurrentSpellCharges()) {
        WOWGUID itemGuid = item->m_obj->m_guid;
        WOWGUID requester = item->m_obj->m_guid;
        result = tooltip->SetItem(item->m_obj->m_entryID, requester, itemGuid, 0, 0, 0, 0, WOWGUID(), 0, NULL, 0, NULL, 0, 0, 0, NULL);
    } else {
        uint32_t start = 0;
        uint32_t duration = 0;
        uint32_t enable = 0;
        Spell_C_GetItemCooldown(item, &start, &duration, &enable);

        if (!enable || !start || !duration) {
            WOWGUID itemGuid = item->m_obj->m_guid;
            WOWGUID requester = item->m_obj->m_guid;
            result = tooltip->SetItem(item->m_obj->m_entryID, requester, itemGuid, 0, 0, 0, 0, WOWGUID(), 0, NULL, 0, NULL, 0, 0, 0, NULL);
        } else {
            TooltipExtendedItemInfo* info = &tooltip->m_itemLinkInfo;
            info->Clear();

            for (uint32_t i = 0; i < 15; i++) {
                info->m_enchants[i] = 0;
                info->m_enchantDurations[i] = 0;
                info->m_enchantCharges[i] = 0;
            }

            info->m_cooldown = start + duration - FrameTime::GetCurTimeMs();
            info->m_proposedEnchant = 0;
            info->m_creator = item->m_item->m_itemFieldCreator;
            info->m_wrapped = (item->m_item->m_itemFieldFlags >> 3) & 1;
            info->m_giftCreator = item->m_item->m_itemFieldGiftcreator;
            info->m_spellCharges = item->GetCurrentSpellCharges();
            WOWGUID itemGuid = item->m_obj->m_guid;
            WOWGUID requester = item->m_obj->m_guid;
            result = tooltip->SetItem(item->m_obj->m_entryID, requester, itemGuid, 0, 0, 1, 0, WOWGUID(), 0, NULL, 0, NULL, 0, 0, 0, NULL);
        }
    }

    int32_t repairCost = CGMerchantInfo::GetRepairCost(item);

    if (result) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    lua_pushnumber(L, static_cast<uint32_t>(repairCost));
    return 2;
}

// OFFSET: 0x0047E610/0x14009C120
static int32_t Script_SetInventoryItemByID(lua_State* L) {
    CGTooltip* tooltip = static_cast<CGTooltip*>(FrameScript_GetObjectThis(L, CGTooltip::GetObjectType()));
    CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();

    if (!lua_isnumber(L, 2)) {
        luaL_error(L, "Usage: SetInventoryItemByID(itemID)");
        return 0;
    }

    CGItem_C* item = player->m_bag.FindItemOfType(static_cast<int32_t>(lua_tonumber(L, 2)), 0);

    if (!item) {
        lua_pushnil(L);
        return 1;
    }

    int32_t result;

    if (!item->GetUsableSpellCharges() || item->GetCurrentSpellCharges()) {
        uint32_t start = 0;
        uint32_t duration = 0;
        uint32_t enable = 0;
        Spell_C_GetItemCooldown(item, &start, &duration, &enable);

        if (enable && start && duration) {
            TooltipExtendedItemInfo* info = &tooltip->m_itemLinkInfo;
            info->Clear();

            for (uint32_t i = 0; i < 15; i++) {
                info->m_enchants[i] = 0;
                info->m_enchantDurations[i] = 0;
                info->m_enchantCharges[i] = 0;
            }

            info->m_cooldown = start + duration - FrameTime::GetCurTimeMs();
            info->m_proposedEnchant = 0;
            info->m_creator = item->m_item->m_itemFieldCreator;
            info->m_wrapped = (item->m_item->m_itemFieldFlags >> 3) & 1;
            info->m_giftCreator = item->m_item->m_itemFieldGiftcreator;
            info->m_spellCharges = item->GetCurrentSpellCharges();
            WOWGUID itemGuid = item->m_obj->m_guid;
            WOWGUID requester = item->m_obj->m_guid;
            result = tooltip->SetItem(item->m_obj->m_entryID, requester, itemGuid, 0, 0, 1, 0, WOWGUID(), 0, NULL, 0, NULL, 0, 0, 0, NULL);
        } else {
            WOWGUID itemGuid = item->m_obj->m_guid;
            WOWGUID requester = item->m_obj->m_guid;
            result = tooltip->SetItem(item->m_obj->m_entryID, requester, itemGuid, 0, 0, 0, 0, WOWGUID(), 0, NULL, 0, NULL, 0, 0, 0, NULL);
        }
    } else {
        WOWGUID itemGuid = item->m_obj->m_guid;
        WOWGUID requester = item->m_obj->m_guid;
        result = tooltip->SetItem(item->m_obj->m_entryID, requester, itemGuid, 0, 0, 0, 0, WOWGUID(), 0, NULL, 0, NULL, 0, 0, 0, NULL);
    }

    if (result) {
        lua_pushnumber(L, 1.0);
        return 1;
    }

    lua_pushnil(L);
    return 1;
}

// OFFSET: 0x0047E8A0/0x14009C4A0
static int32_t Script_SetTalent(lua_State* L) {
    CGTooltip* tooltip = static_cast<CGTooltip*>(FrameScript_GetObjectThis(L, CGTooltip::GetObjectType()));

    if (!lua_isnumber(L, 2) || !lua_isnumber(L, 3)) {
        const char* name = tooltip->GetName();
        luaL_error(L, "Usage: %s:SetTalent(tabIndex, talentIndex[, isInspect[, isPet[, groupIndex[, isPreview]]]])", name ? name : "<unnamed>");
        return 0;
    }

    if (!ClntObjMgrGetActivePlayerObj()) {
        return 0;
    }

    uint32_t tabIndex = static_cast<uint32_t>(static_cast<int64_t>(lua_tonumber(L, 2))) - 1;
    uint32_t talentIndex = static_cast<uint32_t>(static_cast<int64_t>(lua_tonumber(L, 3))) - 1;
    int32_t isInspect = StringToBOOL(L, 4, 0);
    int32_t isPet = StringToBOOL(L, 5, 0);
    uint32_t groupIndex = TalentInfoGetScriptGroupIndex(L, isInspect, isPet, 6);
    int32_t isPreview = StringToBOOL(L, 7, 0);
    TalentTabInfo* tab = CGTalentInfo::GetTab(tabIndex, isInspect, isPet);

    if (tab && talentIndex < tab->m_numTalents) {
        const TalentRec* talent = &tab->m_talents[talentIndex];
        tooltip->SetTalent(talent, TalentInfoGetTalentGroup(groupIndex, isInspect, isPet), isInspect, 0, -1, isPet, isPreview);
    }

    return 0;
}

// OFFSET: 0x0047EA10/0x14009C660
static int32_t Script_SetSendMailItem(lua_State* L) {
    CGTooltip* tooltip = static_cast<CGTooltip*>(FrameScript_GetObjectThis(L, CGTooltip::GetObjectType()));
    uint32_t index;

    if (lua_isnumber(L, 2)) {
        index = static_cast<uint32_t>(static_cast<int64_t>(lua_tonumber(L, 2) - 1.0));

        if (index >= 16) {
            lua_pushnil(L);
            return 1;
        }
    } else {
        index = 0;
    }

    WOWGUID guid = CGMailInfo::m_sendItems[index].m_guid;

    if (guid.m_guid) {
        if (CGItem_C* item = static_cast<CGItem_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_ITEM, __FILE__, __LINE__))) {
            if (tooltip->SetItem(item->m_obj->m_entryID, guid, guid, 0, 0, 0, 0, WOWGUID(), 0, NULL, 0, NULL, 0, 0, 0, NULL)) {
                lua_pushnumber(L, 1.0);
                return 1;
            }
        }
    }

    lua_pushnil(L);
    return 1;
}

// OFFSET: 0x0047EB40/0x14009C7B0
static int32_t Script_SetInboxItem(lua_State* L) {
    CGTooltip* tooltip = static_cast<CGTooltip*>(FrameScript_GetObjectThis(L, CGTooltip::GetObjectType()));

    if (!lua_isnumber(L, 2)) {
        const char* name = tooltip->GetName();
        luaL_error(L, "Usage: %s:SetInboxItem(messageIndex, attachmentIndex)", name ? name : "<unnamed>");
        return 0;
    }

    uint32_t index = static_cast<uint32_t>(static_cast<int64_t>(lua_tonumber(L, 2)));

    if (index - 1 >= CGMailInfo::m_numInboxItems) {
        lua_pushnil(L);
        return 1;
    }

    MailboxMessageInfo* mail = CGMailInfo::m_inbox.m_data[index - 1];

    if (!mail) {
        lua_pushnil(L);
        return 1;
    }

    MailAttachmentInfo* attachment;
    uint32_t attachmentIndex;

    if (lua_isnumber(L, 3) && (attachmentIndex = static_cast<uint32_t>(static_cast<int64_t>(lua_tonumber(L, 3)))) != 0) {
        if (attachmentIndex - 1 >= 16) {
            lua_pushnil(L);
            return 1;
        }

        attachment = &mail->m_attachments[attachmentIndex - 1];
    } else {
        attachment = CGMailInfo::GetFirstAttachment(mail);
    }

    if (attachment && attachment->m_itemID) {
        TooltipExtendedItemInfo* info = &tooltip->m_itemLinkInfo;
        info->Clear();

        for (uint32_t i = 0; i < 15; i++) {
            if (i >= 10) {
                info->m_enchants[i] = 0;
                info->m_enchantDurations[i] = 0;
                info->m_enchantCharges[i] = 0;
            } else {
                info->m_enchants[i] = attachment->m_enchantIDs[i];
                info->m_enchantDurations[i] = attachment->m_enchantDurations[i];
                info->m_enchantCharges[i] = attachment->m_enchantCharges[i];
            }
        }

        info->m_cooldown = 0;
        info->m_proposedEnchant = 0;
        info->m_creator = WOWGUID();
        info->m_wrapped = 0;
        info->m_giftCreator = WOWGUID();
        info->m_spellCharges = attachment->m_spellCharges;
        info->m_propertySeed = attachment->m_suffixFactor;
        info->m_randomPropertiesId = attachment->m_randomPropertiesID;
        info->m_maxDurability = attachment->m_maxDurability;
        info->m_durability = attachment->m_durability;
        info->m_stackCount = attachment->m_count;
        WOWGUID itemGuid = WOWGUID();
        WOWGUID requester = WOWGUID();

        if (tooltip->SetItem(attachment->m_itemID, requester, itemGuid, 0, 0, 1, 0, WOWGUID(), 0, NULL, 0, NULL, 0, attachment->unk_009C, 0, NULL)) {
            lua_pushnumber(L, 1.0);
            return 1;
        }
    }

    lua_pushnil(L);
    return 1;
}

// OFFSET: 0x0047ED80/0x14009CA20
static int32_t Script_SetAuctionSellItem(lua_State* L) {
    CGTooltip* tooltip = static_cast<CGTooltip*>(FrameScript_GetObjectThis(L, CGTooltip::GetObjectType()));
    WOWGUID guid = CGAuctionHouse::m_sellItem;
    CGItem_C* item;

    if (CGAuctionHouse::m_sellItem.m_guid && (item = static_cast<CGItem_C*>(ClntObjMgrObjectPtr(CGAuctionHouse::m_sellItem, TYPEMASK_ITEM, __FILE__, __LINE__))) != NULL && tooltip->SetItem(item->m_obj->m_entryID, guid, guid, 0, 0, 0, 0, WOWGUID(), 0, NULL, 0, NULL, 0, 0, 0, NULL)) {
        lua_pushnumber(L, 1.0);
        return 1;
    }

    lua_pushnil(L);
    return 1;
}

// OFFSET: 0x0047EE50/0x14009CB20
static int32_t Script_SetAuctionItem(lua_State* L) {
    CGTooltip* tooltip = static_cast<CGTooltip*>(FrameScript_GetObjectThis(L, CGTooltip::GetObjectType()));

    if (!lua_isstring(L, 2) || !lua_isnumber(L, 3)) {
        const char* name = tooltip->GetName();
        luaL_error(L, "Usage: %s:SetAuctionItem(\"type\", index)", name ? name : "<unnamed>");
        return 0;
    }

    const char* type = lua_tolstring(L, 2, NULL);
    uint32_t index = static_cast<uint32_t>(static_cast<int64_t>(lua_tonumber(L, 3))) - 1;
    AuctionItemInfo* auction;

    if (!_stricmp(type, "owner")) {
        if (index >= CGAuctionHouse::m_ownerList.m_count) {
            lua_pushnil(L);
            return 1;
        }

        auction = &CGAuctionHouse::m_ownerList.m_data[index];
    } else if (!_stricmp(type, "bidder")) {
        auction = GetBidderAuction(index);
    } else if (!_stricmp(type, "list")) {
        auction = GetListAuction(index);
    } else {
        lua_pushnil(L);
        return 1;
    }

    if (auction && auction->m_itemID) {
        TooltipExtendedItemInfo* info = &tooltip->m_itemLinkInfo;
        info->Clear();

        for (uint32_t i = 0; i < 15; i++) {
            if (i >= 10) {
                info->m_enchants[i] = 0;
                info->m_enchantDurations[i] = 0;
                info->m_enchantCharges[i] = 0;
            } else {
                info->m_enchants[i] = auction->m_enchantID[i];
                info->m_enchantDurations[i] = auction->m_enchantDuration[i];
                info->m_enchantCharges[i] = auction->m_enchantCharges[i];
            }
        }

        info->m_cooldown = 0;
        info->m_proposedEnchant = 0;
        info->m_creator = WOWGUID();
        info->m_wrapped = 0;
        info->m_giftCreator = WOWGUID();
        info->m_spellCharges = auction->m_spellCharges;
        info->m_propertySeed = auction->m_suffixFactor;
        info->m_randomPropertiesId = auction->m_randomPropertyID;
        info->m_stackCount = auction->m_count;
        WOWGUID query = WOWGUID();
        ItemRecSparse* rec = g_itemSparseDB.GetRecord(auction->m_itemID, query, NULL, NULL, false);
        info->m_lockId = (!rec || (auction->m_flags & 0x4)) ? 0 : rec->m_lockID;
        WOWGUID itemGuid = WOWGUID();
        WOWGUID requester = WOWGUID();

        if (tooltip->SetItem(auction->m_itemID, requester, itemGuid, 0, 0, 1, 0, WOWGUID(), 0, NULL, 0, NULL, 0, 0, 1, NULL)) {
            lua_pushnumber(L, 1.0);
            return 1;
        }
    }

    lua_pushnil(L);
    return 1;
}

// OFFSET: 0x0047F0D0/0x14009CE40
static int32_t Script_SetBuybackItem(lua_State* L) {
    CGTooltip* tooltip = static_cast<CGTooltip*>(FrameScript_GetObjectThis(L, CGTooltip::GetObjectType()));

    if (!lua_isnumber(L, 2)) {
        const char* name = tooltip->GetName();
        luaL_error(L, "Usage: %s:SetBuybackItem(slot)", name ? name : "<unnamed>");
        return 0;
    }

    CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();
    uint32_t index = static_cast<uint32_t>(static_cast<int64_t>(lua_tonumber(L, 2)));
    uint32_t slot = index - 1 >= CGMerchantInfo::m_numBuybackItems ? 0 : CGMerchantInfo::m_buybackSlots[index - 1];

    if (player && slot && slot < player->m_bag.m_numSlots && player->m_bag.m_slots[slot].m_guid) {
        if (CGItem_C* item = player->m_bag.GetItemPointer(slot)) {
            WOWGUID itemGuid = item->m_obj->m_guid;
            WOWGUID requester = player->m_obj->m_guid;
            tooltip->SetItem(item->m_obj->m_entryID, requester, itemGuid, 0, 0, 0, 0, WOWGUID(), 0, NULL, 0, NULL, 0, 0, 0, NULL);
        }
    }

    return 0;
}

// OFFSET: 0x0047F210/0x14009CFE0
static int32_t Script_SetLootRollItem(lua_State* L) {
    CGTooltip* tooltip = static_cast<CGTooltip*>(FrameScript_GetObjectThis(L, CGTooltip::GetObjectType()));

    if (!lua_isnumber(L, 2)) {
        const char* name = tooltip->GetName();
        luaL_error(L, "Usage: %s:SetLootRollItem(id)", name ? name : "<unnamed>");
        return 0;
    }

    PendingLootRoll* roll = FindRoll(static_cast<int32_t>(static_cast<int64_t>(lua_tonumber(L, 2))));

    if (roll) {
        TooltipExtendedItemInfo* info = &tooltip->m_itemLinkInfo;
        info->Clear();
        info->m_propertySeed = roll->m_randomSuffix;
        info->m_randomPropertiesId = roll->m_randomPropertyId;
        WOWGUID itemGuid = WOWGUID();
        WOWGUID requester = WOWGUID();
        info->m_stackCount = roll->m_count;
        tooltip->SetItem(roll->m_itemId, requester, itemGuid, 0, 0, 1, 0, WOWGUID(), 0, NULL, 0, NULL, 0, 0, 0, NULL);
    }

    return 0;
}

// OFFSET: 0x0047F310/0x14009D140
static int32_t Script_SetSocketedItem(lua_State* L) {
    CGTooltip* tooltip = static_cast<CGTooltip*>(FrameScript_GetObjectThis(L, CGTooltip::GetObjectType()));

    if (CGItem_C* item = static_cast<CGItem_C*>(ClntObjMgrObjectPtr(CGItemSocketInfo::m_socketItem, TYPEMASK_ITEM, __FILE__, __LINE__))) {
        WOWGUID itemGuid = item->m_obj->m_guid;
        WOWGUID requester = WOWGUID();
        tooltip->SetItem(item->m_obj->m_entryID, requester, itemGuid, 0, 0, 0, 0, WOWGUID(), 1, NULL, 0, NULL, 0, 0, 0, NULL);
    }

    return 0;
}

// OFFSET: 0x0047F3C0/0x14009D210
static int32_t Script_SetTransmogrifyItem(lua_State* L) {
    CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();

    if (!player) {
        return 0;
    }

    CGTooltip* tooltip = static_cast<CGTooltip*>(FrameScript_GetObjectThis(L, CGTooltip::GetObjectType()));

    if (!lua_isnumber(L, 2)) {
        const char* name = tooltip->GetName();
        luaL_error(L, "Usage: %s:SetTransmogrifyItem(slot)", name ? name : "<unnamed>");
        return 0;
    }

    uint32_t slot = static_cast<uint32_t>(static_cast<int64_t>(lua_tonumber(L, 2)));
    TransmogrifyPendingItem* pending = CGTransmogrify::GetPendingItem(slot - 1);
    CGItem_C* item = player->m_bag.GetItemPointer(slot - 1);

    if (pending && item) {
        WOWGUID itemGuid = item->m_obj->m_guid;
        WOWGUID requester = WOWGUID();
        tooltip->SetItem(item->m_obj->m_entryID, requester, itemGuid, 0, 0, 0, 0, WOWGUID(), 0, NULL, 0, NULL, 0, 0, 0, &pending->m_item);
    }

    return 0;
}

// OFFSET: 0x0047F4E0/0x14009D380
static int32_t Script_SetSocketGem(lua_State* L) {
    CGTooltip* tooltip = static_cast<CGTooltip*>(FrameScript_GetObjectThis(L, CGTooltip::GetObjectType()));

    if (!lua_isnumber(L, 2)) {
        const char* name = tooltip->GetName();
        luaL_error(L, "Usage: %s:SetSocketGem(index)", name ? name : "<unnamed>");
        return 0;
    }

    uint32_t index = static_cast<uint32_t>(static_cast<int64_t>(lua_tonumber(L, 2))) - 1;
    WOWGUID gem = index >= 3 ? WOWGUID() : CGItemSocketInfo::m_gemItems[index];

    if (CGItem_C* item = static_cast<CGItem_C*>(ClntObjMgrObjectPtr(gem, TYPEMASK_ITEM, __FILE__, __LINE__))) {
        WOWGUID itemGuid = item->m_obj->m_guid;
        WOWGUID requester = WOWGUID();
        tooltip->SetItem(item->m_obj->m_entryID, requester, itemGuid, 0, 0, 0, 0, WOWGUID(), 0, NULL, 0, NULL, 0, 0, 0, NULL);
    }

    return 0;
}

// OFFSET: 0x0047F600/0x14009D4D0
static int32_t Script_SetExistingSocketGem(lua_State* L) {
    CGTooltip* tooltip = static_cast<CGTooltip*>(FrameScript_GetObjectThis(L, CGTooltip::GetObjectType()));

    if (!lua_isnumber(L, 2)) {
        const char* name = tooltip->GetName();
        luaL_error(L, "Usage: %s:SetSocketGem(index, [toDestroy])", name ? name : "<unnamed>");
        return 0;
    }

    uint32_t index = static_cast<uint32_t>(static_cast<int64_t>(lua_tonumber(L, 2))) - 1;
    int32_t toDestroy = StringToBOOL(L, 3, 0);
    CGItem_C* item = static_cast<CGItem_C*>(ClntObjMgrObjectPtr(CGItemSocketInfo::m_socketItem, TYPEMASK_ITEM, __FILE__, __LINE__));

    if (item && index < 3) {
        if (SpellItemEnchantmentRec* enchant = g_spellItemEnchantmentDB.GetRecord(item->GetEnchantmentId(index + 2))) {
            if (int32_t gemId = enchant->m_srcItemID) {
                WOWGUID itemGuid = WOWGUID();
                WOWGUID requester = WOWGUID();
                tooltip->SetItem(gemId, requester, itemGuid, 0, toDestroy, 0, 0, WOWGUID(), 0, NULL, 0, NULL, 0, 0, 0, NULL);
            }
        }
    }

    return 0;
}

// OFFSET: 0x0047F730/0x14009D680
static int32_t Script_SetGuildBankItem(lua_State* L) {
    CGTooltip* tooltip = static_cast<CGTooltip*>(FrameScript_GetObjectThis(L, CGTooltip::GetObjectType()));

    if (!lua_isnumber(L, 2) || !lua_isnumber(L, 3)) {
        luaL_error(L, "Invalid tab or slot in SetGuildBankItem");
        return 0;
    }

    uint32_t tab = static_cast<uint32_t>(static_cast<int64_t>(lua_tonumber(L, 2) - 1.0));
    uint32_t slot = static_cast<uint32_t>(static_cast<int64_t>(lua_tonumber(L, 3) - 1.0));
    GuildBankSlotInfo* bankItem = CGGuildBankInfo::GetGuildBankItem(tab, slot);

    if (!bankItem) {
        return 0;
    }

    TooltipExtendedItemInfo* info = &tooltip->m_itemLinkInfo;
    info->Clear();
    info->m_enchants[0] = bankItem->m_enchant;
    info->m_enchants[8] = bankItem->m_reforgeEnchant;
    info->m_enchants[7] = bankItem->m_engineeringEnchant;
    info->m_enchants[2] = bankItem->m_enchants[0];
    info->m_enchants[3] = bankItem->m_enchants[1];
    info->m_enchants[4] = bankItem->m_enchants[2];
    info->m_propertySeed = bankItem->m_suffixFactor;
    info->m_randomPropertiesId = bankItem->m_randomPropertyID;
    info->m_spellCharges = bankItem->m_spellCharges;
    info->m_stackCount = bankItem->m_count;

    if (CGPlayer_C* player = ClntObjMgrGetActivePlayerObj()) {
        WOWGUID itemGuid = player->m_obj->m_guid;
        WOWGUID requester = WOWGUID();
        tooltip->SetItem(bankItem->m_itemID, requester, itemGuid, 0, 0, 1, 0, WOWGUID(), 0, NULL, 0, NULL, 0, bankItem->m_flags & 0x4, 0, NULL);
    }

    return 0;
}

// OFFSET: 0x0047F8C0/0x14009D870
static int32_t Script_SetQuestLogSpecialItem(lua_State* L) {
    CGTooltip* tooltip = static_cast<CGTooltip*>(FrameScript_GetObjectThis(L, CGTooltip::GetObjectType()));

    if (!lua_isnumber(L, 2)) {
        const char* name = tooltip->GetName();
        luaL_error(L, "Usage: %s:SetQuestLogSpecialItem(index)", name ? name : "<unnamed>");
        return 0;
    }

    uint32_t index = static_cast<uint32_t>(static_cast<int64_t>(lua_tonumber(L, 2)));

    if (CGItem_C* item = CGQuestLog::GetQuestSpecialItem(index - 1)) {
        WOWGUID itemGuid = item->m_obj->m_guid;
        WOWGUID requester = ClntObjMgrGetActivePlayer();
        tooltip->SetItem(item->m_obj->m_entryID, requester, itemGuid, 0, 0, 0, 0, WOWGUID(), 0, NULL, 0, NULL, 0, 0, 0, NULL);
    }

    return 0;
}

// OFFSET: 0x0047F9B0/0x14009D9A0
static void SetLFGRewardItem(CGTooltip* tooltip, const LFGReward* reward) {
    if (!reward) {
        return;
    }

    if (reward->m_isCurrency) {
        if (CurrencyTypesRec* currency = g_currencyTypesDB.GetRecord(reward->m_id)) {
            tooltip->SetCurrency(currency, WOWGUID());
        }
    } else {
        tooltip->m_itemLinkInfo.Clear();
        tooltip->m_itemLinkInfo.m_stackCount = reward->m_quantity;
        WOWGUID itemGuid = WOWGUID();
        WOWGUID requester = WOWGUID();
        tooltip->SetItem(reward->m_id, requester, itemGuid, 0, 0, 1, 0, WOWGUID(), 0, NULL, 0, NULL, 0, 0, 0, NULL);
    }
}

// OFFSET: 0x0047FA40/0x14009DAB0
static int32_t Script_SetLFGDungeonReward(lua_State* L) {
    CGTooltip* tooltip = static_cast<CGTooltip*>(FrameScript_GetObjectThis(L, CGTooltip::GetObjectType()));

    if (!lua_isnumber(L, 2) || !lua_isnumber(L, 3)) {
        luaL_error(L, "Usage: Tooltip:SetLFGDungeonReward(dungeonID, lootIndex)");
        return 0;
    }

    int32_t dungeonId = lua_tointeger(L, 2);
    uint32_t index = lua_tointeger(L, 3) - 1;

    if (LfgDungeonsRec* dungeon = g_lfgDungeonsDB.GetRecord(dungeonId)) {
        SetLFGRewardItem(tooltip, CGLookingForGroup::GetDungeonReward((dungeon->m_id & 0xFFFFFF) | (dungeon->m_typeID << 24), index));
    }

    return 0;
}

// OFFSET: 0x0047FB00/0x14009DBC0
static int32_t Script_SetLFGCompletionReward(lua_State* L) {
    CGTooltip* tooltip = static_cast<CGTooltip*>(FrameScript_GetObjectThis(L, CGTooltip::GetObjectType()));

    if (!lua_isnumber(L, 2)) {
        luaL_error(L, "Usage: Tooltip:SetLFGCompletionReward(lootIndex)");
        return 0;
    }

    uint32_t index = lua_tointeger(L, 2) - 1;

    if (index < CGLookingForGroup::m_completionRewards.m_count) {
        SetLFGRewardItem(tooltip, &CGLookingForGroup::m_completionRewards.m_data[index]);
    }

    return 0;
}

// OFFSET: 0x0047FB80/0x14009DC60
static int32_t Script_SetLFGDungeonShortageReward(lua_State* L) {
    CGTooltip* tooltip = static_cast<CGTooltip*>(FrameScript_GetObjectThis(L, CGTooltip::GetObjectType()));

    if (!lua_isnumber(L, 2) || !lua_isnumber(L, 3) || !lua_isnumber(L, 4)) {
        luaL_error(L, "Usage: Tooltip:SetLFGDungeonReward(dungeonID, shortageSeverity, lootIndex)");
        return 0;
    }

    int32_t dungeonId = lua_tointeger(L, 2);
    uint32_t severity = lua_tointeger(L, 3) - 1;

    if (severity > 2) {
        return 0;
    }

    uint32_t index = lua_tointeger(L, 4) - 1;
    LfgDungeonsRec* dungeon = g_lfgDungeonsDB.GetRecord(dungeonId);

    if (!dungeon) {
        return 0;
    }

    LFGDungeonRewardInfo* rewards = CGLookingForGroup::GetDungeonRewardInfo((dungeonId & 0xFFFFFF) | (dungeon->m_typeID << 24));

    if (rewards && index < rewards->m_shortages[severity].m_rewards.m_count) {
        SetLFGRewardItem(tooltip, &rewards->m_shortages[severity].m_rewards.m_data[index]);
    }

    return 0;
}

// OFFSET: 0x0047FC80/0x14009DDC0
static int32_t Script_SetReforgeItem(lua_State* L) {
    CGTooltip* tooltip = static_cast<CGTooltip*>(FrameScript_GetObjectThis(L, CGTooltip::GetObjectType()));

    if (CGItem_C* item = static_cast<CGItem_C*>(ClntObjMgrObjectPtr(CGReforge::m_reforgeItem, TYPEMASK_ITEM, __FILE__, __LINE__))) {
        WOWGUID itemGuid = item->m_obj->m_guid;
        WOWGUID requester = item->m_obj->m_guid;
        tooltip->SetItem(item->m_obj->m_entryID, requester, itemGuid, 0, 0, 0, 0, WOWGUID(), 0, NULL, 0, NULL, 0, 0, 0, NULL);
    }

    return 0;
}

// OFFSET: 0x0047FD20/0x14009DE90
static int32_t Script_SetItemByID(lua_State* L) {
    CGTooltip* tooltip = static_cast<CGTooltip*>(FrameScript_GetObjectThis(L, CGTooltip::GetObjectType()));

    if (!lua_isnumber(L, 2)) {
        const char* name = tooltip->GetName();
        luaL_error(L, "Usage: %s:SetItemByID(ID)", name ? name : "<unnamed>");
        return 0;
    }

    double itemId = lua_tonumber(L, 2);
    WOWGUID itemGuid = WOWGUID();
    WOWGUID requester = WOWGUID();
    tooltip->SetItem(static_cast<int32_t>(static_cast<int64_t>(itemId)), requester, itemGuid, 0, 0, 0, 0, WOWGUID(), 0, NULL, 0, NULL, 0, 0, 0, NULL);
    return 0;
}

// OFFSET: 0x0047FDF0/0x14009DFA0
static int32_t Script_SetVoidItem(lua_State* L) {
    CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();

    if (!player) {
        return 0;
    }

    CGVoidStorage_C* storage = player->GetVoidStorage();

    if (!storage || (storage->m_state != 2 && storage->m_state != 4 && storage->m_state != 3)) {
        return 0;
    }

    CGTooltip* tooltip = static_cast<CGTooltip*>(FrameScript_GetObjectThis(L, CGTooltip::GetObjectType()));

    if (!lua_isnumber(L, 2)) {
        const char* name = tooltip->GetName();
        luaL_error(L, "Usage: %s:SetVoidItem(slot)", name ? name : "<unnamed>");
        return 0;
    }

    uint32_t slot = static_cast<uint32_t>(static_cast<int64_t>(lua_tonumber(L, 2) - 1.0));
    VoidItem_C* item = storage->GetItemBySlot(slot);

    if (item) {
        TooltipExtendedItemInfo* info = &tooltip->m_itemLinkInfo;
        info->Clear();
        info->m_itemId = item->m_entryID;
        info->m_creator = item->m_creator;
        info->m_randomPropertiesId = item->m_randomPropertiesId;
        WOWGUID itemGuid = WOWGUID();
        info->m_propertySeed = item->m_propertySeed;
        info->m_soulbound = 1;
        tooltip->SetItem(item->m_entryID, item->m_guid, itemGuid, 0, 0, 1, 0, WOWGUID(), 0, NULL, 0, NULL, 0, 0, 0, NULL);
    }

    return 0;
}

// OFFSET: 0x0047FF50/0x14009E170
static int32_t Script_SetVoidDepositItem(lua_State* L) {
    CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();

    if (!player) {
        return 0;
    }

    CGVoidStorage_C* storage = player->GetVoidStorage();

    if (!storage || (storage->m_state != 2 && storage->m_state != 4 && storage->m_state != 3)) {
        return 0;
    }

    CGTooltip* tooltip = static_cast<CGTooltip*>(FrameScript_GetObjectThis(L, CGTooltip::GetObjectType()));

    if (!lua_isnumber(L, 2)) {
        const char* name = tooltip->GetName();
        luaL_error(L, "Usage: %s:SetVoidItem(slot)", name ? name : "<unnamed>");
        return 0;
    }

    uint32_t slot = static_cast<uint32_t>(static_cast<int64_t>(lua_tonumber(L, 2) - 1.0));

    if (CGVoidTransfer* transfer = CGVoidTransfer::Get(false)) {
        WOWGUID guid = transfer->GetDepositItem(slot);

        if (guid.m_guid) {
            if (CGItem_C* item = static_cast<CGItem_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_ITEM, __FILE__, __LINE__))) {
                tooltip->SetItem(item->m_obj->m_entryID, guid, guid, 0, 0, 0, 0, WOWGUID(), 0, NULL, 0, NULL, 0, 0, 0, NULL);
            }
        }
    }

    return 0;
}

// OFFSET: 0x004800A0/0x14009E320
static int32_t Script_SetVoidWithdrawalItem(lua_State* L) {
    CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();

    if (!player) {
        return 0;
    }

    CGVoidStorage_C* storage = player->GetVoidStorage();

    if (!storage || (storage->m_state != 2 && storage->m_state != 4 && storage->m_state != 3)) {
        return 0;
    }

    CGTooltip* tooltip = static_cast<CGTooltip*>(FrameScript_GetObjectThis(L, CGTooltip::GetObjectType()));

    if (!lua_isnumber(L, 2)) {
        const char* name = tooltip->GetName();
        luaL_error(L, "Usage: %s:SetVoidItem(slot)", name ? name : "<unnamed>");
        return 0;
    }

    uint32_t slot = static_cast<uint32_t>(static_cast<int64_t>(lua_tonumber(L, 2) - 1.0));

    if (CGVoidTransfer* transfer = CGVoidTransfer::Get(false)) {
        WOWGUID guid = transfer->GetWithdrawItem(slot);

        if (guid.m_guid) {
            if (VoidItem_C* item = storage->GetItem(guid)) {
                TooltipExtendedItemInfo* info = &tooltip->m_itemLinkInfo;
                info->Clear();
                info->m_itemId = item->m_entryID;
                info->m_creator = item->m_creator;
                info->m_randomPropertiesId = item->m_randomPropertiesId;
                info->m_propertySeed = item->m_propertySeed;
                info->m_soulbound = 1;
                WOWGUID itemGuid = WOWGUID();
                tooltip->SetItem(item->m_entryID, item->m_guid, itemGuid, 0, 0, 1, 0, WOWGUID(), 0, NULL, 0, NULL, 0, 0, 0, NULL);
            }
        }
    }

    return 0;
}

// OFFSET: 0x00480220/0x14009E520
static int32_t Script_SetMissingLootItem(lua_State* L) {
    CGTooltip* tooltip = static_cast<CGTooltip*>(FrameScript_GetObjectThis(L, CGTooltip::GetObjectType()));

    if (!lua_isnumber(L, 2)) {
        luaL_error(L, "Invalid loot index in SetMissingLootItem");
        return 0;
    }

    uint32_t index = static_cast<uint32_t>(static_cast<int64_t>(lua_tonumber(L, 2) - 1.0));

    if (index >= CGLootInfo::m_missingItems.m_count) {
        return 0;
    }

    MissingLootItem* missing = &CGLootInfo::m_missingItems.m_data[index];

    if (!missing) {
        return 0;
    }

    TooltipExtendedItemInfo* info = &tooltip->m_itemLinkInfo;
    info->Clear();
    info->m_propertySeed = missing->m_seed;
    info->m_randomPropertiesId = missing->m_randomPropertiesId;
    info->m_stackCount = missing->m_quantity;
    SetItemSpellCharges(missing->m_itemId, info);
    WOWGUID itemGuid = WOWGUID();
    WOWGUID requester = WOWGUID();
    tooltip->SetItem(missing->m_itemId, requester, itemGuid, 0, 0, 1, 0, WOWGUID(), 0, NULL, 0, NULL, 0, 0, 0, NULL);
    return 0;
}

// OFFSET: 0x004806E0/0x14009EB30
static int32_t Script_SetAction(lua_State* L) {
    CGTooltip* tooltip = static_cast<CGTooltip*>(FrameScript_GetObjectThis(L, CGTooltip::GetObjectType()));

    if (!lua_isnumber(L, 2)) {
        const char* name = tooltip->GetName();
        luaL_error(L, "Usage: %s:SetAction(slot)", name ? name : "<unnamed>");
        lua_pushnil(L);
        return 1;
    }

    uint32_t slot = static_cast<int32_t>(lua_tonumber(L, 2)) - 1;
    uint32_t start;
    uint32_t duration;
    uint32_t enable;
    CGActionBar::GetCooldown(slot, start, duration, enable);
    tooltip->ClearTooltip();
    uint32_t action = CGActionBar::m_slotActions[slot];
    int32_t result;

    if (action) {
        if ((action & 0xF0000000) == 0x80000000) {
            int32_t itemId = CGActionBar::GetItem(slot);
            CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();
            WOWGUID query = WOWGUID();
            ItemRecSparse* rec = g_itemSparseDB.GetRecord(itemId, query, NULL, NULL, false);
            int32_t charges = CGItem_C::NumBaseCharges(itemId);

            if (!charges) {
                if (CGItem_C* enchantItem = CGActionBar::FindUseEnchantItemPtr(itemId)) {
                    charges = enchantItem->GetUsableSpellCharges();
                }
            }

            CGItem_C* item = NULL;

            if (player && rec) {
                if (rec->m_inventoryType) {
                    uint32_t flags = 129;

                    if (charges != -1 && charges) {
                        flags = 161;
                    }

                    item = player->m_bag.FindItemOfType(itemId, flags);

                    if (!item) {
                        if (flags & 0x20) {
                            item = player->m_bag.FindItemOfType(itemId, 32);
                        }

                        if (!item) {
                            item = player->m_bag.FindItemOfType(itemId, 128);
                        }
                    }
                } else {
                    uint32_t flags = 256;

                    if (charges != -1 && charges) {
                        flags = 288;
                    }

                    item = player->m_bag.FindItemOfType(itemId, flags);

                    if (!item && (flags & 0x20)) {
                        item = player->m_bag.FindItemOfType(itemId, flags & ~0x20);
                    }
                }
            }

            if (item) {
                TooltipExtendedItemInfo* info = &tooltip->m_itemLinkInfo;
                info->Clear();
                info->m_cooldown = (enable && start && duration) ? duration + start - FrameTime::GetCurTimeMs() : 0;

                if (charges != -1 && charges) {
                    info->m_spellCharges = item->GetCurrentSpellCharges();
                }

                WOWGUID itemGuid = item->m_obj->m_guid;
                WOWGUID requester = item->m_obj->m_guid;
                result = tooltip->SetItem(item->m_obj->m_entryID, requester, itemGuid, 0, 0, 1, 0, WOWGUID(), 0, NULL, 0, NULL, 0, 0, 0, NULL);
            } else {
                WOWGUID itemGuid = WOWGUID();
                WOWGUID requester = WOWGUID();
                result = tooltip->SetItem(itemId, requester, itemGuid, 0, 0, 0, 0, WOWGUID(), 0, NULL, 0, NULL, 0, 0, 0, NULL);
            }

            if (result) {
                lua_pushnumber(L, 1.0);
                return 1;
            }

            lua_pushnil(L);
            return 1;
        }

        if (!(action & 0xF0000000)) {
            int32_t brief = 1;
            CVar* uberTooltips = CVar::Lookup("UberTooltips");

            if (uberTooltips && uberTooltips->m_intValue) {
                brief = 0;
            }

            int32_t petSpell;
            int32_t baseSpell = 0;
            int32_t noCastTime = 0;
            int32_t noCost = 0;
            int32_t spellId = CGActionBar::GetSpell(slot, petSpell, &baseSpell, &noCastTime, &noCost, NULL);

            if (enable && start && duration) {
                result = tooltip->SetSpell(spellId, brief, duration + start - FrameTime::GetCurTimeMs(), 0, 1, 0, 0, NULL, NULL, 0, 0, -1, -1, 0, 0, noCastTime, noCost);
            } else {
                result = tooltip->SetSpell(spellId, brief, 0, 0, 1, 0, 0, NULL, NULL, 0, 0, -1, -1, 0, 0, noCastTime, noCost);
            }

            if (result) {
                lua_pushnumber(L, 1.0);
                return 1;
            }

            lua_pushnil(L);
            return 1;
        }
    }

    if (CGActionBar::IsMacro(slot)) {
        result = tooltip->SetMacro(CGActionBar::GetMacroID(slot));
    } else if (CGActionBar::IsEquipmentSet(slot)) {
        result = tooltip->SetEquipmentSet(CGActionBar::GetEquipmentSetID(slot));
    } else if (CGActionBar::IsPetAction(slot)) {
        PetAction* petAction = GetPetAction(CGActionBar::GetSlotSpellAction(slot));

        if (!petAction) {
            lua_pushnil(L);
            return 1;
        }

        result = tooltip->SetPetAction(*petAction);
    } else {
        if (CGActionBar::IsFlyout(slot)) {
            CVar* uberTooltips = CVar::Lookup("UberTooltips");
            bool showDescription = false;

            if (uberTooltips && uberTooltips->m_intValue) {
                showDescription = true;
            }

            tooltip->SetSpellFlyout(CGActionBar::GetFlyout(slot), showDescription);
        }

        lua_pushnil(L);
        return 1;
    }

    if (result) {
        lua_pushnumber(L, 1.0);
        return 1;
    }

    lua_pushnil(L);
    return 1;
}

// OFFSET: 0x00481320/0x14009FDA0
static int32_t Script_SetHyperlinkCompareItem(lua_State* L) {
    CGTooltip* tooltip = static_cast<CGTooltip*>(FrameScript_GetObjectThis(L, CGTooltip::GetObjectType()));

    if (!lua_isstring(L, 2)) {
        const char* name = tooltip->GetName();
        luaL_error(L, "Usage: %s:SetHyperlinkCompareItem(\"hyperlink\" [, offset, shiftButton])", name ? name : "<unnamed>");
        return 0;
    }

    const char* link = lua_tolstring(L, 2, NULL);
    TooltipExtendedItemInfo info;
    info.Clear();

    if (ClntObjMgrGetActivePlayerObj() && info.GetItemIdFromLink(link)) {
        int32_t offset = 0;
        int32_t shift = StringToBOOL(L, 4, 0);

        if (lua_isnumber(L, 3)) {
            offset = static_cast<int32_t>(static_cast<int64_t>(lua_tonumber(L, 3))) - 1;
        }

        if (lua_type(L, 5) == LUA_TTABLE) {
            lua_rawgeti(L, 5, 0);
            CGTooltip* compare = static_cast<CGTooltip*>(lua_touserdata(L, -1));

            if (compare->IsA(CGTooltip::GetObjectType()) && SetCompareItem(tooltip, info.m_itemId, offset, shift, compare)) {
                lua_pushnumber(L, 1.0);
                return 1;
            }
        }

        if (SetCompareItem(tooltip, info.m_itemId, offset, shift, NULL)) {
            lua_pushnumber(L, 1.0);
            return 1;
        }
    }

    lua_pushnil(L);
    return 1;
}

// OFFSET: 0x00465B00
void CGTooltip::DestroyScriptMetaTable() {
    lua_State* L = FrameScript_GetContext();
    luaL_unref(L, LUA_REGISTRYINDEX, CGTooltip::s_metatable);
    CGTooltip::s_metatable = LUA_NOREF;
}

// STATIC: 0x00C9A008
static FrameScript_Method s_ScriptMethods[] = {
    { "AddFontStrings", &Script_AddFontStrings },
    { "SetMinimumWidth", &Script_SetMinimumWidth },
    { "GetMinimumWidth", &Script_GetMinimumWidth },
    { "SetPadding", &Script_SetPadding },
    { "GetPadding", &Script_GetPadding },
    { "IsOwned", &Script_IsOwned },
    { "GetOwner", &Script_GetOwner },
    { "SetOwner", &Script_SetOwner },
    { "GetAnchorType", &Script_GetAnchorType },
    { "SetAnchorType", &Script_SetAnchorType },
    { "ClearLines", &Script_ClearLines },
    { "AddLine", &Script_AddLine },
    { "AddDoubleLine", &Script_AddDoubleLine },
    { "AddTexture", &Script_AddTexture },
    { "SetText", &Script_SetText },
    { "AppendText", &Script_AppendText },
    { "FadeOut", &Script_FadeOut },
    { "SetHyperlink", &Script_SetHyperlink },
    { "SetAction", &Script_SetAction },
    { "SetPetAction", &Script_SetPetAction },
    { "SetShapeshift", &Script_SetShapeshift },
    { "SetPossession", &Script_SetPossession },
    { "SetSpellBookItem", &Script_SetSpellBookItem },
    { "SetSpellByID", &Script_SetSpellByID },
    { "AddSpellByID", &Script_AddSpellByID },
    { "SetGlyph", &Script_SetGlyph },
    { "SetGlyphByID", &Script_SetGlyphByID },
    { "SetInventoryItem", &Script_SetInventoryItem },
    { "SetLootItem", &Script_SetLootItem },
    { "SetLootCurrency", &Script_SetLootCurrency },
    { "SetQuestItem", &Script_SetQuestItem },
    { "SetQuestLogItem", &Script_SetQuestLogItem },
    { "SetQuestCurrency", &Script_SetQuestCurrency },
    { "SetQuestLogCurrency", &Script_SetQuestLogCurrency },
    { "SetTrainerService", &Script_SetTrainerService },
    { "SetTradeSkillItem", &Script_SetTradeSkillItem },
    { "SetMerchantItem", &Script_SetMerchantItem },
    { "SetMerchantCostItem", &Script_SetMerchantCostItem },
    { "SetMissingLootItem", &Script_SetMissingLootItem },
    { "SetTradePlayerItem", &Script_SetTradePlayerItem },
    { "SetTradeTargetItem", &Script_SetTradeTargetItem },
    { "SetBagItem", &Script_SetBagItem },
    { "SetInventoryItemByID", &Script_SetInventoryItemByID },
    { "SetUnit", &Script_SetUnit },
    { "SetUnitBuff", &Script_SetUnitBuff },
    { "SetUnitDebuff", &Script_SetUnitDebuff },
    { "SetUnitAura", &Script_SetUnitAura },
    { "SetTalent", &Script_SetTalent },
    { "SetSendMailItem", &Script_SetSendMailItem },
    { "SetInboxItem", &Script_SetInboxItem },
    { "SetAuctionSellItem", &Script_SetAuctionSellItem },
    { "SetAuctionItem", &Script_SetAuctionItem },
    { "NumLines", &Script_NumLines },
    { "SetQuestRewardSpell", &Script_SetQuestRewardSpell },
    { "SetQuestLogRewardSpell", &Script_SetQuestLogRewardSpell },
    { "SetHyperlinkCompareItem", &Script_SetHyperlinkCompareItem },
    { "SetBuybackItem", &Script_SetBuybackItem },
    { "SetLootRollItem", &Script_SetLootRollItem },
    { "SetSocketedItem", &Script_SetSocketedItem },
    { "SetSocketGem", &Script_SetSocketGem },
    { "SetExistingSocketGem", &Script_SetExistingSocketGem },
    { "SetGuildBankItem", &Script_SetGuildBankItem },
    { "IsUnit", &Script_IsUnit },
    { "GetUnit", &Script_GetUnit },
    { "GetItem", &Script_GetItem },
    { "GetSpell", &Script_GetSpell },
    { "SetTotem", &Script_SetTotem },
    { "SetCurrencyToken", &Script_SetCurrencyToken },
    { "SetBackpackToken", &Script_SetBackpackToken },
    { "IsEquippedItem", &Script_IsEquippedItem },
    { "SetQuestLogSpecialItem", &Script_SetQuestLogSpecialItem },
    { "SetEquipmentSet", &Script_SetEquipmentSet },
    { "SetFrameStack", &Script_SetFrameStack },
    { "SetLFGDungeonReward", &Script_SetLFGDungeonReward },
    { "SetLFGCompletionReward", &Script_SetLFGCompletionReward },
    { "SetLFGDungeonShortageReward", &Script_SetLFGDungeonShortageReward },
    { "SetReforgeItem", &Script_SetReforgeItem },
    { "SetInstanceLockEncountersComplete", &Script_SetInstanceLockEncountersComplete },
    { "SetCurrencyByID", &Script_SetCurrencyByID },
    { "SetItemByID", &Script_SetItemByID },
    { "SetTransmogrifyItem", &Script_SetTransmogrifyItem },
    { "SetVoidItem", &Script_SetVoidItem },
    { "SetVoidDepositItem", &Script_SetVoidDepositItem },
    { "SetVoidWithdrawalItem", &Script_SetVoidWithdrawalItem },
};

// OFFSET: 0x00465AE0
void CGTooltip::RegisterScriptMethods(lua_State* L) {
    CSimpleFrame::RegisterScriptMethods(L);
    FrameScript_Object::FillScriptMethodTable(L, s_ScriptMethods, sizeof(s_ScriptMethods) / sizeof(s_ScriptMethods[0]));
}

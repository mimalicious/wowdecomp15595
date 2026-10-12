#include "UnitCombatLog_C.h"
#include <cstring>
#include <ctime>
#include "Engine/Source/Base/ConvertUTF.h"
#include "Engine/Source/ConsoleBase/ConsoleVar.h"
#include "Engine/Source/Event/FrameTime.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameXML/LoadXML.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/lua-5.1/src/lua.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"
#include "WoW/Source/DB/DBClient/CreatureStats_C.h"
#include "WoW/Source/DB/DBClient/DBCacheInstances.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/DBClient/GameObjectStats_C.h"
#include "WoW/Source/DB/Rec/ItemRecSparse.h"
#include "WoW/Source/DB/Rec/SpellAuraOptionsRec.h"
#include "WoW/Source/DB/Rec/SpellCategoriesRec.h"
#include "WoW/Source/DB/Rec/SpellDurationRec.h"
#include "WoW/Source/DB/Rec/SpellEffectRec.h"
#include "WoW/Source/DB/Rec/SpellItemEnchantmentRec.h"
#include "WoW/Source/DB/Rec/SpellMechanicRec.h"
#include "WoW/Source/DB/Rec/SpellRec.h"
#include "WoW/Source/DB/Rec/SummonPropertiesRec.h"
#include "WoW/Source/Game/GameClient/NameCache.h"
#include "WoW/Source/Game/GameClient/PetNameCache.h"
#include "WoW/Source/Game/GameClient/PlayerName.h"
#include "WoW/Source/Game/ValidateName.h"
#include "WoW/Source/Magic/MagicClient/Spell_C.h"
#include "WoW/Source/Object/ObjectClient/AuraData.h"
#include "WoW/Source/Object/ObjectClient/GameObject_C.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/Object/ObjectClient/UnitCombat_C.h"
#include "WoW/Source/Object/ObjectClient/Unit_C.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/Ui/ChatFrame.h"
#include "WoW/Source/Ui/GameUI.h"
#include "WoW/Source/Ui/PartyFrame.h"
#include "WoW/Source/Ui/RaidInfo.h"
#include "WoW/Source/Ui/ScriptEvents.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"

// STATIC: 0x00CA7818/0x140B47300
static const char* s_combatLogEventNames[50] = {
    "ENVIRONMENTAL_DAMAGE",
    "SWING_DAMAGE",
    "SWING_MISSED",
    "RANGE_DAMAGE",
    "RANGE_MISSED",
    "SPELL_CAST_START",
    "SPELL_CAST_SUCCESS",
    "SPELL_CAST_FAILED",
    "SPELL_MISSED",
    "SPELL_DAMAGE",
    "SPELL_HEAL",
    "SPELL_ENERGIZE",
    "SPELL_DRAIN",
    "SPELL_LEECH",
    "SPELL_INSTAKILL",
    "SPELL_SUMMON",
    "SPELL_CREATE",
    "SPELL_INTERRUPT",
    "SPELL_EXTRA_ATTACKS",
    "SPELL_DURABILITY_DAMAGE",
    "SPELL_DURABILITY_DAMAGE_ALL",
    "SPELL_AURA_APPLIED",
    "SPELL_AURA_APPLIED_DOSE",
    "SPELL_AURA_REMOVED_DOSE",
    "SPELL_AURA_REMOVED",
    "SPELL_AURA_REFRESH",
    "SPELL_DISPEL",
    "SPELL_STOLEN",
    "SPELL_AURA_BROKEN",
    "SPELL_AURA_BROKEN_SPELL",
    "DAMAGE_AURA_BROKEN",
    "ENCHANT_APPLIED",
    "ENCHANT_REMOVED",
    "SPELL_PERIODIC_MISSED",
    "SPELL_PERIODIC_DAMAGE",
    "SPELL_PERIODIC_HEAL",
    "SPELL_PERIODIC_ENERGIZE",
    "SPELL_PERIODIC_DRAIN",
    "SPELL_PERIODIC_LEECH",
    "SPELL_DISPEL_FAILED",
    "DAMAGE_SHIELD",
    "DAMAGE_SHIELD_MISSED",
    "DAMAGE_SPLIT",
    "PARTY_KILL",
    "UNIT_DIED",
    "UNIT_DESTROYED",
    "SPELL_RESURRECT",
    "SPELL_BUILDING_DAMAGE",
    "SPELL_BUILDING_HEAL",
    "UNIT_DISSIPATES"
};

// STATIC: 0x00CA78E0/0x140B47490
static const char* s_combatTextTypes[48] = {
    "INTERRUPT",
    "DAMAGE_CRIT",
    "DAMAGE",
    "MISS",
    "DODGE",
    "PARRY",
    "EVADE",
    "IMMUNE",
    "DEFLECT",
    "REFLECT",
    "RESIST",
    "BLOCK",
    "ABSORB",
    "SPELL_DAMAGE_CRIT",
    "SPELL_DAMAGE",
    "SPELL_MISS",
    "SPELL_DODGE",
    "SPELL_PARRY",
    "SPELL_EVADE",
    "SPELL_IMMUNE",
    "SPELL_DEFLECT",
    "SPELL_REFLECT",
    "SPELL_RESIST",
    "SPELL_BLOCK",
    "SPELL_ABSORB",
    "ENCHANTMENT_REMOVED",
    "ENCHANTMENT_ADDED",
    "PERIODIC_HEAL",
    "ENERGIZE",
    "PERIODIC_ENERGIZE",
    "SPELL_CAST",
    "SPELL_AURA_END",
    "SPELL_AURA_END_HARMFUL",
    "SPELL_AURA_START",
    "SPELL_AURA_START_HARMFUL",
    "SPELL_ACTIVE",
    "FACTION",
    "HEAL_CRIT",
    "HEAL",
    "DAMAGE_SHIELD",
    "SPELL_DISPELLED",
    "EXTRA_ATTACKS",
    "SPLIT_DAMAGE",
    "HONOR_GAINED",
    "PERIODIC_HEAL_ABSORB",
    "HEAL_CRIT_ABSORB",
    "HEAL_ABSORB",
    "ARENA_POINTS_GAINED"
};

// STATIC: 0x00CA79F8/0x140B476C0
static const uint32_t s_mechanicTextColor = 0xFFFFDE00;

// STATIC: 0x00CA7A04/0x140B476D0
static TSList<InvalidatedName, TSGetLink<InvalidatedName> > s_invalidatedNames;

// STATIC: 0x00CA7A10/0x140B476E8
static TSList<PendingCombatLogData, TSGetLink<PendingCombatLogData> > s_pendingData;

// STATIC: 0x00CA7A1C/0x140B47700
static TSList<PendingCombatLogData, TSGetLink<PendingCombatLogData> > s_freePendingData;

// STATIC: 0x00CA7A28/0x140B47718
TSList<CombatLogFilter, TSGetLink<CombatLogFilter> > s_filters;

// STATIC: 0x00CA7A34/0x140B47730
TSList<CombatLogEntry, TSGetLink<CombatLogEntry> > s_entries;

// STATIC: 0x00CA7A40/0x140B47748
static TSList<CombatLogEntry, TSGetLink<CombatLogEntry> > s_freeEntries;

// STATIC: 0x00DD82E0/0x140CBEA50
WOWGUID s_combatTextActiveUnit;

// STATIC: 0x00DD82E8/0x140CBEA58
static uint32_t s_startTimeMs;

// STATIC: 0x00DD82EC
static uint32_t s_startTime;

// STATIC: 0x00DD82F0/0x140CBEA68
CombatLogEntry* s_currentEntry;

// STATIC: 0x00DD82F4/0x140CBEA70
CombatLogEntry* s_pendingEntries;

// STATIC: 0x00DD8560
static char s_unknownObjectName[256];

// STATIC: 0x00CF0CA4/0x140B91550
const char* g_spellMissStrings[12] = { "NONE", "MISS", "RESIST", "DODGE", "PARRY", "BLOCK", "EVADE", "IMMUNE", "IMMUNE", "DEFLECT", "ABSORB", "REFLECT" };

// STATIC: 0x00C9CBD4/0x140B36B38
static const char* s_environmentalDamageTypes[6] = { "FATIGUE", "DROWNING", "FALLING", "LAVA", "SLIME", "FIRE" };

// INLINED
inline CombatLogEntry::CombatLogEntry() {
    this->m_infoFlags = 0;
    this->Clear();
}

// INLINED
inline CombatLogEntry::~CombatLogEntry() {
    if (this->m_infoFlags & 0x2) {
        SMemFree(this->m_info.m_text, __FILE__, __LINE__, 0);
    }

    this->CancelPendingCallbacks();
}

// INLINED
inline InvalidatedName::InvalidatedName() : m_name(NULL) {
}

// INLINED
inline InvalidatedName::~InvalidatedName() {
    SMemFree(this->m_name, __FILE__, __LINE__, 0);
}

// OFFSET: 0x005F34F0/0x140279050
uint32_t GetCombatLogObjectRaidFlags(const WOWGUID& guid) {
    if (!guid) {
        return 0x80000000;
    }

    uint32_t flags = 0;

    if (CGRaidInfo::m_numMembers || CGPartyInfo::NumMembers()) {
        uint32_t index = GetRaidTargetIndexFromGuid(guid);

        if (index < 28) {
            return 1 << index;
        }
    }

    return flags;
}

// OFFSET: 0x005F3540/0x1402764B0
char* GUIDToString(WOWGUID guid, char* buffer) {
    buffer[0] = '0';
    buffer[1] = 'x';
    buffer[18] = '\0';
    char* cursor = buffer + 17;
    uint64_t value = guid.m_guid;

    for (int32_t i = 16; i; i--) {
        uint8_t digit = value & 0xF;
        *cursor-- = digit >= 10 ? digit + 'A' - 10 : digit + '0';
        value >>= 4;
    }

    return buffer;
}

// OFFSET: 0x005F3590/0x140276500
WOWGUID StringToGUID(const char* string) {
    const char* cursor = string;
    uint64_t value = 0;

    if (!SStrCmpI(string, "0x", 2)) {
        cursor = string + 2;
    }

    for (int32_t i = 16; i; i--) {
        char c = *cursor++;
        int32_t digit;

        if (static_cast<uint8_t>(c - '0') <= 9) {
            digit = c - '0';
        } else if (static_cast<uint8_t>(c - 'a') <= 5) {
            digit = c - 'a' + 10;
        } else if (static_cast<uint8_t>(c - 'A') <= 5) {
            digit = c - 'A' + 10;
        } else {
            break;
        }

        value = (value << 4) | digit;
    }

    return WOWGUID::Make(value);
}

// OFFSET: 0x005F3620
bool CombatLogFlagsMatch(uint32_t flags, uint32_t mask, uint32_t raidFlags, uint32_t raidMask) {
    uint32_t matched = flags & mask;

    if ((matched & 0xFFFF0000) || (raidFlags & raidMask)) {
        return true;
    }

    if ((matched & 0xF) && (matched & 0xF0) && (matched & 0x300)) {
        return (matched & 0xFC00) != 0;
    }

    return false;
}

// OFFSET: 0x005F3660/0x1402765A0
void CombatLogEntry::SetFailReason(const char* reason) {
    this->m_infoFlags |= 0x2;
    this->m_info.m_text = SStrDupA(reason, __FILE__, __LINE__);
}

// OFFSET: 0x005F3690
void CombatLogEntry::SetAuraValues(const int32_t* values) {
    this->m_infoFlags |= 0x10000;
    this->m_info.m_values[3] = values[0];
    this->m_info.m_values[4] = values[1];
    this->m_info.m_values[5] = values[2];
}

// OFFSET: 0x005F36C0/0x1402765E0
void __stdcall CheckToSanitizeUnitLowGuid(WOWGUID* guid) {
    uint32_t high = guid->m_guid >> 32;

    if ((high & 0xF0F00000) != 0xF1300000 && (high & 0xF0F00000) != 0xF1500000) {
        return;
    }

    CreatureStats_C* stats = g_creatureDBCache.GetRecord(high & 0xFFFFF, *guid, NULL, NULL, false);

    if (stats && (stats->m_typeFlags[0] & 0x4000)) {
        guid->m_guid &= 0xFFFFFFFFFF000000ULL;
    }
}

// OFFSET: 0x005F3730
static void PackHex(char*& cursor, uint32_t value, uint32_t& remaining) {
    if (remaining > 1) {
        *cursor++ = ',';
        remaining--;
    }

    if (remaining < 10) {
        return;
    }

    *cursor++ = '0';
    *cursor++ = 'x';
    remaining -= 2;

    if (!value) {
        *cursor++ = '0';
        remaining--;
        return;
    }

    uint32_t mask = 0xF0000000;
    int32_t digits = 8;

    while (!(mask & value)) {
        mask >>= 4;
        digits--;
    }

    cursor += digits;

    do {
        cursor--;
        uint8_t digit = value & 0xF;
        *cursor = digit >= 10 ? digit + 'a' - 10 : digit + '0';
        value >>= 4;
    } while (value);

    cursor += digits;
    remaining -= digits;
}

// OFFSET: 0x005F37B0
static void PackString(char*& cursor, uint32_t& remaining, const char* text) {
    if (remaining > 1) {
        *cursor++ = ',';
        remaining--;
    }

    for (const char* c = text; *c && remaining; c++) {
        *cursor++ = *c;
        remaining--;
    }
}

// OFFSET: 0x005F37E0
static void PackQuotedString(char*& cursor, const char* text, uint32_t& remaining) {
    if (remaining > 1) {
        *cursor++ = ',';

        if (--remaining > 1) {
            *cursor++ = '"';
            remaining--;
        }
    }

    for (const char* c = text; *c && remaining; c++) {
        if ((*c == '"' || *c == '\\') && remaining > 1) {
            *cursor++ = '\\';
            remaining--;
        }

        *cursor++ = *c;
        remaining--;
    }

    if (remaining > 1) {
        *cursor++ = '"';
        remaining--;
    }
}

// OFFSET: 0x005F3850
static void PackBool(uint32_t& remaining, char*& cursor, int32_t value) {
    if (value) {
        if (remaining > 1) {
            *cursor++ = ',';
            remaining--;
        }

        uint32_t length = SStrPrintf(cursor, remaining, "%d", 1);

        if (length && length < remaining) {
            cursor += length;
            remaining -= length;
        }
    } else {
        if (remaining > 1) {
            *cursor++ = ',';
            remaining--;
        }

        for (const char* c = "nil"; *c && remaining; c++) {
            *cursor++ = *c;
            remaining--;
        }
    }
}

// OFFSET: 0x005F38C0
bool IsCombatTextActiveUnit(WOWGUID guid) {
    return s_combatTextActiveUnit == guid;
}

// OFFSET: 0x005F38F0/0x140276660
void SendToCombatText(int32_t type, const char* text, int32_t amount) {
    FrameScript_SignalEvent(SCRIPT_EVENT_COMBAT_TEXT_UPDATE, "%s%s%d", s_combatTextTypes[type], text, amount);
}

// OFFSET: 0x005F3920
void SendToCombatText(int32_t type, const char* text, int32_t amount, int32_t overflow) {
    FrameScript_SignalEvent(SCRIPT_EVENT_COMBAT_TEXT_UPDATE, "%s%s%d%d", s_combatTextTypes[type], text, amount, overflow);
}

// OFFSET: 0x005F3950
void SendToCombatText(int32_t type, int32_t amount, int32_t overflow) {
    FrameScript_SignalEvent(SCRIPT_EVENT_COMBAT_TEXT_UPDATE, "%s%d%d", s_combatTextTypes[type], amount, overflow);
}

// OFFSET: 0x005F3980
void SendToCombatText(int32_t type, int32_t amount) {
    FrameScript_SignalEvent(SCRIPT_EVENT_COMBAT_TEXT_UPDATE, "%s%d", s_combatTextTypes[type], amount);
}

// OFFSET: 0x005F39B0/0x1402766A0
void SendToCombatText(int32_t type, float amount) {
    FrameScript_SignalEvent(SCRIPT_EVENT_COMBAT_TEXT_UPDATE, "%s%f", s_combatTextTypes[type], amount);
}

// OFFSET: 0x005F39E0/0x1402766D0
void SendToCombatText(int32_t type, const char* text) {
    FrameScript_SignalEvent(SCRIPT_EVENT_COMBAT_TEXT_UPDATE, "%s%s", s_combatTextTypes[type], text);
}

// OFFSET: 0x005F3A10
void SendToCombatText(int32_t type) {
    FrameScript_SignalEvent(SCRIPT_EVENT_COMBAT_TEXT_UPDATE, "%s", s_combatTextTypes[type]);
}

// OFFSET: 0x005F3BC0/0x1402785A0
void UnitCombatLogProcGo(const WOWGUID& casterGuid, const SpellRec* spell) {
    if (s_combatTextActiveUnit == casterGuid && (spell->m_attributesExF & 0x40)) {
        FrameScript_SignalEvent(SCRIPT_EVENT_COMBAT_TEXT_UPDATE, "%s%s", s_combatTextTypes[30], static_cast<const char*>(spell->m_name));
    }
}

// OFFSET: 0x005F3DD0/0x140276AE0
void CombatLogEntry::Clear() {
    this->m_event = -1;
    this->m_pendingMask = 0;
    this->m_sourceGuid = WOWGUID();
    this->m_sourceName = NULL;
    this->m_sourceRealm = NULL;
    this->m_sourceFlags = 0x80000000;
    this->m_sourceRaidFlags = 0x80000000;
    this->m_destGuid = WOWGUID();
    this->m_destName = NULL;
    this->m_destRealm = NULL;
    this->m_destFlags = 0x80000000;
    this->m_destRaidFlags = 0x80000000;
    this->m_spellId = 0;
    this->m_spellName.Copy(static_cast<const char*>(NULL));

    if (this->m_infoFlags & 0x2) {
        SMemFree(this->m_info.m_text, __FILE__, __LINE__, 0);
    }

    this->m_infoFlags = 0;
    this->m_hideCaster = true;
}

// OFFSET: 0x005F3E40
void CombatLogEntry::SetMissInfo(int32_t missType, int32_t amount, bool offHand) {
    this->m_infoFlags |= 0x4;
    this->m_info.m_bytes[4] = offHand;
    this->m_info.m_values[0] = missType;

    switch (missType) {
        case 2:
            this->m_infoFlags |= 0x4000;
            this->m_info.m_values[5] = amount;
            break;

        case 5:
            this->m_infoFlags |= 0x8000;
            this->m_info.m_values[6] = amount;
            break;

        case 10:
            this->m_infoFlags |= 0x2000;
            this->m_info.m_values[4] = amount;
            break;
    }
}

// OFFSET: 0x005F3EA0/0x140276B80
int32_t CombatLogEntry::PushEvent(lua_State* L) {
    double timestamp = static_cast<double>(this->m_timestamp - s_startTimeMs) * 0.001 + static_cast<double>(s_startTime);
    char name[308];
    char guid[20];

    lua_checkstack(L, 8);
    lua_pushnumber(L, timestamp);
    lua_pushstring(L, s_combatLogEventNames[this->m_event]);
    lua_pushboolean(L, this->m_hideCaster);
    lua_pushstring(L, GUIDToString(this->m_sourceGuid, guid));

    if (this->m_sourceName && this->m_sourceRealm) {
        SStrPrintf(name, 305, "%s-%s", this->m_sourceName, this->m_sourceRealm);
        lua_pushstring(L, name);
    } else if (this->m_sourceName && *this->m_sourceName) {
        lua_pushstring(L, this->m_sourceName);
    } else {
        lua_pushnil(L);
    }

    lua_pushinteger(L, this->m_sourceFlags);
    lua_pushinteger(L, this->m_sourceRaidFlags);
    lua_pushstring(L, GUIDToString(this->m_destGuid, guid));

    if (this->m_destName && this->m_destRealm) {
        SStrPrintf(name, 305, "%s-%s", this->m_destName, this->m_destRealm);
        lua_pushstring(L, name);
    } else if (this->m_destName && *this->m_destName) {
        lua_pushstring(L, this->m_destName);
    } else {
        lua_pushnil(L);
    }

    lua_pushinteger(L, this->m_destFlags);
    lua_pushinteger(L, this->m_destRaidFlags);
    int32_t count = 11;

    if (this->m_spellId) {
        lua_checkstack(L, 3);
        lua_pushinteger(L, this->m_spellId);
        const SpellRec* spell = g_spellDB.GetRecord(this->m_spellId);

        if (spell) {
            lua_pushstring(L, spell->m_name);
            lua_pushinteger(L, spell->m_schoolMask);
        } else {
            lua_pushnil(L);
            lua_pushinteger(L, 0);
        }

        count = 14;
    }

    if (this->m_infoFlags & 0x1) {
        lua_checkstack(L, 1);
        lua_pushinteger(L, this->m_info.m_values[0]);
        count++;
    }

    if (this->m_infoFlags & 0x2) {
        lua_checkstack(L, 1);
        lua_pushstring(L, this->m_info.m_text);
        count++;
    }

    if (this->m_infoFlags & 0x4) {
        lua_checkstack(L, 2);
        lua_pushstring(L, g_spellMissStrings[this->m_info.m_values[0]]);

        if (this->m_info.m_bytes[4]) {
            lua_pushnumber(L, 1.0);
        } else {
            lua_pushnil(L);
        }

        count += 2;
    }

    if (this->m_infoFlags & 0x8) {
        lua_checkstack(L, 1);
        lua_pushstring(L, s_environmentalDamageTypes[this->m_info.m_values[0]]);
        count++;
    }

    if (this->m_infoFlags & 0x10) {
        lua_checkstack(L, 10);
        lua_pushnumber(L, static_cast<double>(this->m_info.m_values[1]));
        lua_pushnumber(L, static_cast<double>(this->m_info.m_values[2]));
        lua_pushinteger(L, this->m_info.m_values[3]);

        if (this->m_info.m_values[5]) {
            lua_pushnumber(L, static_cast<double>(this->m_info.m_values[5]));
        } else {
            lua_pushnil(L);
        }

        if (this->m_info.m_values[6]) {
            lua_pushnumber(L, static_cast<double>(this->m_info.m_values[6]));
        } else {
            lua_pushnil(L);
        }

        if (this->m_info.m_values[4]) {
            lua_pushnumber(L, static_cast<double>(this->m_info.m_values[4]));
        } else {
            lua_pushnil(L);
        }

        if (this->m_critical) {
            lua_pushnumber(L, 1.0);
        } else {
            lua_pushnil(L);
        }

        if (this->m_glancing) {
            lua_pushnumber(L, 1.0);
        } else {
            lua_pushnil(L);
        }

        if (this->m_crushing) {
            lua_pushnumber(L, 1.0);
        } else {
            lua_pushnil(L);
        }

        if (this->m_isOffHand) {
            lua_pushnumber(L, 1.0);
        } else {
            lua_pushnil(L);
        }

        count += 10;
    }

    if (this->m_infoFlags & 0x2000) {
        lua_checkstack(L, 1);
        lua_pushinteger(L, this->m_info.m_values[4]);
        count++;
    }

    if (this->m_infoFlags & 0x4000) {
        lua_checkstack(L, 1);
        lua_pushinteger(L, this->m_info.m_values[5]);
        count++;
    }

    if (this->m_infoFlags & 0x8000) {
        lua_checkstack(L, 1);
        lua_pushinteger(L, this->m_info.m_values[6]);
        count++;
    }

    if (this->m_infoFlags & 0x20) {
        lua_checkstack(L, 4);
        lua_pushnumber(L, static_cast<double>(this->m_info.m_values[0]));
        lua_pushnumber(L, static_cast<double>(this->m_info.m_values[1]));
        lua_pushnumber(L, static_cast<double>(this->m_info.m_values[2]));

        if (this->m_info.m_bytes[12]) {
            lua_pushnumber(L, 1.0);
        } else {
            lua_pushnil(L);
        }

        count += 4;
    }

    if (this->m_infoFlags & 0x40) {
        lua_checkstack(L, 2);
        lua_pushnumber(L, static_cast<double>(this->m_info.m_values[0]));
        lua_pushnumber(L, static_cast<double>(this->m_info.m_values[1]));

        if (this->m_info.m_values[2] < 0) {
            lua_pushnil(L);
        } else {
            lua_pushnumber(L, static_cast<double>(this->m_info.m_values[2]));
        }

        count += 3;
    }

    if (this->m_infoFlags & 0x80) {
        lua_checkstack(L, 3);
        lua_pushnumber(L, static_cast<double>(this->m_info.m_values[0]));
        lua_pushnumber(L, static_cast<double>(this->m_info.m_values[1]));

        if (this->m_info.m_values[2]) {
            lua_pushnumber(L, static_cast<double>(this->m_info.m_values[2]));
        } else {
            lua_pushnil(L);
        }

        if (this->m_info.m_values[3] < 0) {
            lua_pushnil(L);
        } else {
            lua_pushnumber(L, static_cast<double>(this->m_info.m_values[3]));
        }

        count += 4;
    }

    if (this->m_infoFlags & 0x100) {
        lua_checkstack(L, 2);
        lua_pushinteger(L, this->m_info.m_values[0]);
        const SpellRec* spell = g_spellDB.GetRecord(this->m_info.m_values[0]);

        if (spell) {
            lua_pushstring(L, spell->m_name);
            lua_pushinteger(L, spell->m_schoolMask);
        } else {
            lua_pushnil(L);
            lua_pushnumber(L, 0.0);
        }

        count += 3;
    }

    if (this->m_infoFlags & 0x1000) {
        lua_checkstack(L, 1);
        lua_pushstring(L, this->m_info.m_names[2]);
        count++;
    }

    if (this->m_infoFlags & 0x200) {
        lua_checkstack(L, 2);
        lua_pushinteger(L, this->m_info.m_values[0]);
        lua_pushstring(L, this->m_info.m_names[1]);
        count += 2;
    }

    if (this->m_infoFlags & 0x400) {
        lua_checkstack(L, 1);
        lua_pushstring(L, this->m_info.m_bytes[4] ? "DEBUFF" : "BUFF");
        count++;
    }

    if (this->m_infoFlags & 0x10000) {
        lua_checkstack(L, 3);

        for (uint32_t i = 0; i < 3; i++) {
            lua_pushinteger(L, this->m_info.m_values[3 + i]);
        }

        count += 3;
    }

    if (this->m_infoFlags & 0x800) {
        lua_checkstack(L, 1);
        lua_pushinteger(L, this->m_info.m_values[2]);
        count++;
    }

    return count;
}

// OFFSET: 0x005F4590/0x1402773A0
void CombatLogEntry::PackEvent(char* buffer, uint32_t size) {
    char name[308];
    SStrCopy(buffer, s_combatLogEventNames[this->m_event], size);
    uint32_t length = SStrLen(s_combatLogEventNames[this->m_event]);
    char* cursor = buffer + length;
    uint32_t remaining = size - length;

    if (remaining > 1) {
        *cursor++ = ',';
        remaining--;
    }

    if (remaining > 18) {
        GUIDToString(this->m_sourceGuid, cursor);
        cursor += 18;
        remaining -= 18;
    }

    if (this->m_sourceName) {
        if (this->m_sourceRealm) {
            SStrPrintf(name, 305, "%s-%s", this->m_sourceName, this->m_sourceRealm);
            PackQuotedString(cursor, name, remaining);
        } else {
            PackQuotedString(cursor, this->m_sourceName, remaining);
        }
    } else {
        PackString(cursor, remaining, "nil");
    }

    PackHex(cursor, this->m_sourceFlags, remaining);
    PackHex(cursor, this->m_sourceRaidFlags, remaining);

    if (remaining > 1) {
        *cursor++ = ',';
        remaining--;
    }

    if (remaining > 18) {
        GUIDToString(this->m_destGuid, cursor);
        cursor += 18;
        remaining -= 18;
    }

    if (this->m_destName) {
        if (this->m_destRealm) {
            SStrPrintf(name, 305, "%s-%s", this->m_destName, this->m_destRealm);
            PackQuotedString(cursor, name, remaining);
        } else {
            PackQuotedString(cursor, this->m_destName, remaining);
        }
    } else {
        PackString(cursor, remaining, "nil");
    }

    PackHex(cursor, this->m_destFlags, remaining);
    PackHex(cursor, this->m_destRaidFlags, remaining);

    if (this->m_spellId) {
        if (remaining > 1) {
            *cursor++ = ',';
            remaining--;
        }

        uint32_t written = SStrPrintf(cursor, remaining, "%d", this->m_spellId);

        if (written && written < remaining) {
            cursor += written;
            remaining -= written;
        }

        const SpellRec* spell = g_spellDB.GetRecord(this->m_spellId);

        if (spell) {
            PackQuotedString(cursor, spell->m_name, remaining);
            PackHex(cursor, spell->m_schoolMask, remaining);
        } else {
            PackString(cursor, remaining, "nil");

            if (remaining > 1) {
                *cursor++ = ',';
                remaining--;
            }

            if (remaining >= 10) {
                *cursor++ = '0';
                *cursor++ = 'x';
                *cursor++ = '0';
                remaining -= 3;
            }
        }
    }

    const int32_t* values = this->m_info.m_values;

    if (this->m_infoFlags & 0x1) {
        if (remaining > 1) {
            *cursor++ = ',';
            remaining--;
        }

        uint32_t written = SStrPrintf(cursor, remaining, "%d", values[0]);

        if (written && written < remaining) {
            cursor += written;
            remaining -= written;
        }
    }

    if (this->m_infoFlags & 0x2) {
        PackQuotedString(cursor, this->m_info.m_text, remaining);
    }

    if (this->m_infoFlags & 0x4) {
        PackString(cursor, remaining, g_spellMissStrings[values[0]]);
    }

    if (this->m_infoFlags & 0x8) {
        PackString(cursor, remaining, s_environmentalDamageTypes[values[0]]);
    }

    if (this->m_infoFlags & 0x10) {
        const int32_t damage[6] = { values[1], values[2], values[3], values[5], values[6], values[4] };

        for (uint32_t i = 0; i < 6; i++) {
            if (remaining > 1) {
                *cursor++ = ',';
                remaining--;
            }

            uint32_t written = SStrPrintf(cursor, remaining, "%d", damage[i]);

            if (written && written < remaining) {
                cursor += written;
                remaining -= written;
            }
        }

        PackBool(remaining, cursor, this->m_critical);
        PackBool(remaining, cursor, this->m_glancing);
        PackBool(remaining, cursor, this->m_crushing);
    }

    const uint32_t singleFlags[3] = { 0x2000, 0x4000, 0x8000 };
    const int32_t singleValues[3] = { values[4], values[5], values[6] };

    for (uint32_t i = 0; i < 3; i++) {
        if (!(this->m_infoFlags & singleFlags[i])) {
            continue;
        }

        if (remaining > 1) {
            *cursor++ = ',';
            remaining--;
        }

        uint32_t written = SStrPrintf(cursor, remaining, "%d", singleValues[i]);

        if (written && written < remaining) {
            cursor += written;
            remaining -= written;
        }
    }

    if (this->m_infoFlags & 0x20) {
        for (uint32_t i = 0; i < 3; i++) {
            if (remaining > 1) {
                *cursor++ = ',';
                remaining--;
            }

            uint32_t written = SStrPrintf(cursor, remaining, "%d", values[i]);

            if (written && written < remaining) {
                cursor += written;
                remaining -= written;
            }
        }

        PackBool(remaining, cursor, this->m_info.m_bytes[12]);
    }

    if (this->m_infoFlags & 0x40) {
        for (uint32_t i = 0; i < 2; i++) {
            if (remaining > 1) {
                *cursor++ = ',';
                remaining--;
            }

            uint32_t written = SStrPrintf(cursor, remaining, "%d", values[i]);

            if (written && written < remaining) {
                cursor += written;
                remaining -= written;
            }
        }
    }

    if (this->m_infoFlags & 0x80) {
        for (uint32_t i = 0; i < 3; i++) {
            if (remaining > 1) {
                *cursor++ = ',';
                remaining--;
            }

            uint32_t written = SStrPrintf(cursor, remaining, "%d", values[i]);

            if (written && written < remaining) {
                cursor += written;
                remaining -= written;
            }
        }
    }

    if (this->m_infoFlags & 0x100) {
        if (remaining > 1) {
            *cursor++ = ',';
            remaining--;
        }

        uint32_t written = SStrPrintf(cursor, remaining, "%d", values[0]);

        if (written && written < remaining) {
            cursor += written;
            remaining -= written;
        }

        const SpellRec* spell = g_spellDB.GetRecord(values[0]);
        int32_t school;

        if (spell) {
            PackQuotedString(cursor, spell->m_name, remaining);
            school = spell->m_schoolMask;
        } else {
            PackString(cursor, remaining, "nil");
            school = 0;
        }

        if (remaining > 1) {
            *cursor++ = ',';
            remaining--;
        }

        written = SStrPrintf(cursor, remaining, "%d", school);

        if (written && written < remaining) {
            cursor += written;
            remaining -= written;
        }
    }

    if (this->m_infoFlags & 0x1000) {
        PackQuotedString(cursor, this->m_info.m_names[2], remaining);
    }

    if (this->m_infoFlags & 0x200) {
        if (remaining > 1) {
            *cursor++ = ',';
            remaining--;
        }

        uint32_t written = SStrPrintf(cursor, remaining, "%d", values[0]);

        if (written && written < remaining) {
            cursor += written;
            remaining -= written;
        }

        PackQuotedString(cursor, this->m_info.m_names[1], remaining);
    }

    if (this->m_infoFlags & 0x400) {
        const char* type = this->m_info.m_bytes[4] ? "DEBUFF" : "BUFF";

        if (remaining > 1) {
            *cursor++ = ',';
            remaining--;
        }

        for (const char* c = type; *c && remaining; c++, remaining--) {
            *cursor++ = *c;
        }
    }

    if (this->m_infoFlags & 0x10000) {
        for (uint32_t i = 0; i < 3; i++) {
            if (remaining > 1) {
                *cursor++ = ',';
                remaining--;
            }

            uint32_t written = SStrPrintf(cursor, remaining, "%d", values[3 + i]);

            if (written && written < remaining) {
                cursor += written;
                remaining -= written;
            }
        }
    }

    if (this->m_infoFlags & 0x800) {
        if (remaining > 1) {
            *cursor++ = ',';
            remaining--;
        }

        uint32_t written = SStrPrintf(cursor, remaining, "%d", values[2]);

        if (written && written < remaining) {
            cursor += written;
        }
    }

    *cursor = '\0';
}

// OFFSET: 0x005F4F60/0x1402784E0
bool SpellHasLearnSpellEffect(const SpellRec* spell) {
    uint32_t count;
    const TSGrowableArray<const SpellEffectRec*>* effects = SpellGetEffects(spell->m_id, count);

    for (uint32_t i = 0; i < count; i++) {
        if (effects->m_data[i] && effects->m_data[i]->m_effect == 36) {
            return true;
        }
    }

    return false;
}

// OFFSET: 0x005F4FB0/0x140278490
bool SpellHasOpenLockEffect(const SpellRec* spell) {
    uint32_t count;
    const TSGrowableArray<const SpellEffectRec*>* effects = SpellGetEffects(spell->m_id, count);

    for (uint32_t i = 0; i < count; i++) {
        if (effects->m_data[i] && effects->m_data[i]->m_effect == 33) {
            return true;
        }
    }

    return false;
}

// OFFSET: 0x005F5040
static bool ShouldDisplayMechanic(const SpellRec* spell, const WOWGUID& casterGuid) {
    if (!(spell->m_attributesExF & 0x200000)) {
        return false;
    }

    CVar* all = CVar::Lookup("fctAllSpellMechanics");

    if (all && all->m_intValue > 0) {
        return true;
    }

    CVar* mechanics = CVar::Lookup("fctSpellMechanics");

    if (mechanics && mechanics->m_intValue) {
        CVar* other = CVar::Lookup("fctSpellMechanicsOther");

        if (IsCombatTextActiveUnit(casterGuid) || (other && other->m_intValue)) {
            return true;
        }
    }

    return false;
}

// OFFSET: 0x005F50B0/0x1402785F0
static void DisplayMechanicEffect(const WOWGUID& casterGuid, const WOWGUID& unitGuid, const SpellRec* spell, const SpellMechanicRec* mechanic) {
    if (!ShouldDisplayMechanic(spell, casterGuid)) {
        return;
    }

    char text[148];
    uint16_t wide[64];
    char stateName[64];
    ConvertUTF8toUTF16(wide, 64, mechanic->m_stateName, 0x7FFFFFFF, NULL, NULL);
    wide[0] = ToUpper(wide[0]);
    ConvertUTF16toUTF8(stateName, 64, wide, 0x7FFFFFFF, NULL, NULL);
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(unitGuid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (s_combatTextActiveUnit == casterGuid) {
        SStrCopy(text, stateName, sizeof(text));
    } else {
        SStrPrintf(text, sizeof(text), "%s (%s)", stateName, GetCombatLogObjectName(casterGuid));
    }

    if (unit) {
        PlayerNameCreateText(unit->m_nameDesc, 0, text, reinterpret_cast<const CImVector*>(&s_mechanicTextColor), NULL);
    }
}

// OFFSET: 0x005F51E0/0x1402787C0
void UnitCombatLogSpellMechanic(const WOWGUID& unitGuid, const AuraData* aura, const SpellRec* spell, uint32_t effectMask) {
    if ((spell->m_attributes & 0x180) || !*spell->m_name || SpellHasLearnSpellEffect(spell) || (spell->m_attributesExD & 0x1) || SpellHasOpenLockEffect(spell)) {
        return;
    }

    uint32_t count;
    const TSGrowableArray<const SpellEffectRec*>* effects = SpellGetEffects(spell->m_id, count);

    for (int32_t i = 0; i < static_cast<int32_t>(count); i++) {
        const SpellEffectRec* effect = effects->m_data[i];

        if (!effect || !effect->m_effectMechanic || !((1 << i) & effectMask)) {
            continue;
        }

        bool duplicate = false;

        for (int32_t j = i - 1; j >= 0; j--) {
            if (effects->m_data[j] && effects->m_data[i]->m_effectMechanic == effects->m_data[j]->m_effectMechanic) {
                duplicate = true;
            }
        }

        if (duplicate) {
            continue;
        }

        SpellMechanicRec* mechanic = g_spellMechanicDB.GetRecord(effect->m_effectMechanic);

        if (mechanic && mechanic->m_id != 26) {
            DisplayMechanicEffect(aura->m_creator, unitGuid, spell, mechanic);
        }
    }

    if (!spell->m_spellCategoriesID) {
        return;
    }

    SpellCategoriesRec* categories = g_spellCategoriesDB.GetRecord(spell->m_spellCategoriesID);

    if (!categories || !categories->m_mechanic) {
        return;
    }

    bool duplicate = false;

    for (int32_t i = 0; i < static_cast<int32_t>(count); i++) {
        if (effects->m_data[i] && effects->m_data[i]->m_effectMechanic == categories->m_mechanic) {
            duplicate = true;
        }
    }

    if (duplicate) {
        return;
    }

    SpellMechanicRec* mechanic = g_spellMechanicDB.GetRecord(categories->m_mechanic);

    if (mechanic && mechanic->m_id != 26) {
        DisplayMechanicEffect(aura->m_creator, unitGuid, spell, mechanic);
    }
}

// OFFSET: 0x005F5390/0x140278A40
uint32_t GetCombatLogObjectFlags(const WOWGUID& guid) {
    if (!guid) {
        return 0x80000000;
    }

    bool inRaid = CGRaidInfo::m_numMembers != 0;
    bool inGroup = inRaid || CGPartyInfo::NumMembers();
    WOWGUID owner = guid;
    CGObject_C* object = static_cast<CGObject_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_OBJECT, __FILE__, __LINE__));
    CGUnit_C* unit = NULL;

    if (object) {
        if (object->m_obj->m_type & TYPEMASK_UNIT) {
            unit = static_cast<CGUnit_C*>(object);
            const WOWGUID& controller = unit->m_unit->m_unitFieldCharmedby ? unit->m_unit->m_unitFieldCharmedby : unit->m_unit->m_unitFieldCreatedby;

            if (controller) {
                owner = controller;
            }
        } else if (object->m_obj->m_type & TYPEMASK_GAMEOBJECT) {
            owner = static_cast<CGGameObject_C*>(object)->m_gameObject->m_createdBy;
        }
    }

    uint32_t ownerHigh = owner.m_guid >> 32;

    if (owner != guid && ((ownerHigh & 0xF0000000) || !((ownerHigh & 0xF07FFFFF) || static_cast<uint32_t>(owner.m_guid)))) {
        CGUnit_C* ownerUnit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(owner, TYPEMASK_UNIT, __FILE__, __LINE__));

        if (ownerUnit) {
            const WOWGUID& controller = ownerUnit->m_unit->m_unitFieldCharmedby ? ownerUnit->m_unit->m_unitFieldCharmedby : ownerUnit->m_unit->m_unitFieldCreatedby;

            if (controller) {
                owner = *ownerUnit->GetCharmerOrCreatorGuid();
            }
        }
    }

    uint32_t high = guid.m_guid >> 32;
    bool isPlayer = !(high & 0xF0000000) && ((high & 0xF07FFFFF) || static_cast<uint32_t>(guid.m_guid));
    uint32_t flags;

    if (isPlayer || (high & 0xF0F00000) == 0xF1300000 || (high & 0xF0F00000) == 0xF1500000 || (high & 0xF0F00000) == 0xF1400000) {
        if (owner == guid) {
            if (!isPlayer) {
                flags = (high & 0xF0F00000) == 0xF1400000 ? 0x1000 : 0x800;
            } else {
                flags = 0x400;
            }
        } else {
            const WOWGUID& master = unit->m_unit->m_unitFieldCharmedby ? unit->m_unit->m_unitFieldCharmedby : unit->m_unit->m_unitFieldSummonedby;
            flags = master ? 0x1000 : 0x2000;
        }
    } else {
        flags = 0x4000;
    }

    ownerHigh = owner.m_guid >> 32;

    if ((ownerHigh & 0xF0000000) || !((ownerHigh & 0xF07FFFFF) || static_cast<uint32_t>(owner.m_guid))) {
        flags |= 0x200;
    } else {
        flags |= 0x100;
    }

    if (owner == ClntObjMgrGetActivePlayer()) {
        flags |= 0x11;
    } else {
        bool hostile = false;

        if (unit) {
            CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

            if (player) {
                int32_t reaction = unit->UnitReaction(player);

                if (reaction >= 4) {
                    flags |= 0x10;
                } else if (reaction <= 1) {
                    flags |= 0x40;
                    hostile = true;
                }
            }
        }

        if (!hostile && inGroup) {
            if (CGPartyInfo::IsMember(owner)) {
                flags |= 0x12;
            } else if (inRaid && IsGuidInMyRaid(owner)) {
                flags |= 0x14;
            }
        }

        if (!(flags & 0xF)) {
            flags |= 0x8;
        }

        if (!(flags & 0xF0)) {
            flags |= 0x20;
        }
    }

    if (guid == CGGameUI::m_lockedTarget) {
        flags |= 0x10000;
    }

    if (guid == CGGameUI::m_focusTarget) {
        flags |= 0x20000;
    }

    if ((flags & 0x400) && ((flags & 0x6) || (owner == ClntObjMgrGetActivePlayer() && inGroup))) {
        uint8_t assignments = inRaid ? CGRaidInfo::GetPartyAssignments(guid) : CGPartyInfo::GetPartyAssignments(guid);

        if (assignments & 0x2) {
            return flags | 0x40000;
        }

        if (assignments & 0x4) {
            return flags | 0x80000;
        }
    }

    return flags;
}

// OFFSET: 0x005F5710/0x140278D90
bool CombatLogFilter::SetFilterData(const char* events, const WOWGUID& sourceGuid, uint32_t sourceFlags, uint32_t sourceRaidFlags, const WOWGUID& destGuid, uint32_t destFlags, uint32_t destRaidFlags, int32_t spellId, const char* spellName) {
    if (events) {
        if (this->m_events.m_count) {
            memset(this->m_events.m_bits.m_data, 0, sizeof(uint32_t) * this->m_events.m_bits.m_count);
        }

        char token[128];

        do {
            SStrTokenize(&events, token, sizeof(token), " ,", NULL);

            if (!token[0]) {
                break;
            }

            for (uint32_t i = 0; i < 50; i++) {
                if (!SStrCmpI(token, s_combatLogEventNames[i], STORM_MAX_STR)) {
                    this->m_events.m_bits.m_data[i >> 5] |= 1 << (i & 0x1F);
                    break;
                }
            }
        } while (token[0] && *events);
    } else if (this->m_events.m_count) {
        memset(this->m_events.m_bits.m_data, 0xFF, sizeof(uint32_t) * this->m_events.m_bits.m_count);
    }

    if (sourceGuid) {
        this->m_sourceGuid = sourceGuid;
        this->m_sourceFlags = 0;
        this->m_sourceRaidFlags = 0;
    } else {
        this->m_sourceGuid = WOWGUID();
        this->m_sourceFlags = sourceFlags;
        this->m_sourceRaidFlags = sourceRaidFlags;
    }

    if (destGuid) {
        this->m_destGuid = destGuid;
        this->m_destFlags = 0;
        this->m_destRaidFlags = 0;
    } else {
        this->m_destGuid = WOWGUID();
        this->m_destFlags = destFlags;
        this->m_destRaidFlags = destRaidFlags;
    }

    if (spellId) {
        this->m_spellId = spellId;
        this->m_spellName.Copy(static_cast<const char*>(NULL));
        return true;
    }

    this->m_spellId = 0;
    this->m_spellName.Copy(spellName);
    return true;
}

// OFFSET: 0x005F5870/0x140278F50
bool CombatLogFilter::MatchesFilter(int32_t event, const WOWGUID& sourceGuid, uint32_t sourceFlags, uint32_t sourceRaidFlags, const WOWGUID& destGuid, uint32_t destFlags, uint32_t destRaidFlags, int32_t spellId, const char* spellName) {
    if (!((1 << (event & 0x1F)) & this->m_events.m_bits.m_data[static_cast<uint32_t>(event) >> 5])) {
        return false;
    }

    if (this->m_sourceGuid) {
        if (sourceGuid != this->m_sourceGuid) {
            return false;
        }
    } else {
        uint32_t matched = sourceFlags & this->m_sourceFlags;

        if (!(matched & 0xFFFF0000) && !(sourceRaidFlags & this->m_sourceRaidFlags) && (!(matched & 0xF) || !(matched & 0xF0) || !(matched & 0x300) || !(matched & 0xFC00))) {
            return false;
        }
    }

    if (this->m_destGuid) {
        if (destGuid != this->m_destGuid) {
            return false;
        }
    } else if (!CombatLogFlagsMatch(this->m_destFlags, destFlags, this->m_destRaidFlags, destRaidFlags)) {
        return false;
    }

    if (this->m_spellId) {
        return spellId == this->m_spellId;
    }

    const char* name = this->m_spellName.m_str;
    return !name || (spellName && !SStrCmpUTF8I(spellName, name, 0x7FFFFFFF));
}

// OFFSET: 0x005F5970/0x1402790D0
bool CombatLogEntry::MatchesFilter() {
    CombatLogFilter* filter = s_filters.Head();

    if (!filter) {
        return true;
    }

    for (; filter; filter = s_filters.Next(filter)) {
        if (filter->MatchesFilter(this->m_event, this->m_sourceGuid, this->m_sourceFlags, this->m_sourceRaidFlags, this->m_destGuid, this->m_destFlags, this->m_destRaidFlags, this->m_spellId, this->m_spellName.m_str)) {
            return true;
        }
    }

    return false;
}

// OFFSET: 0x005F59D0/0x140279170
void SendCombatLogEvents() {
    char buffer[4099];
    bool logging = CGChat::IsChatLogging(1) != 0;
    lua_State* L = FrameScript_GetContext();
    lua_pushstring(L, g_scriptEvents[SCRIPT_EVENT_COMBAT_LOG_EVENT]);

    for (CombatLogEntry* entry = s_pendingEntries;;) {
        int32_t count = entry->PushEvent(L);

        if (s_pendingEntries->MatchesFilter()) {
            FrameScript_SignalEvent(SCRIPT_EVENT_COMBAT_LOG_EVENT, L, count + 1);
        }

        TSList<EVENTLISTENERNODE>* listeners = FrameScript_GetEventListeners(g_scriptEvents[SCRIPT_EVENT_COMBAT_LOG_EVENT_UNFILTERED]);

        if (!listeners->Head()) {
            lua_settop(L, -1 - count);
        } else {
            lua_pushstring(L, g_scriptEvents[SCRIPT_EVENT_COMBAT_LOG_EVENT_UNFILTERED]);
            lua_insert(L, -count - 1);
            FrameScript_SignalEvent(SCRIPT_EVENT_COMBAT_LOG_EVENT_UNFILTERED, L, count + 1);
            lua_settop(L, -count - 2);
        }

        if (logging) {
            s_pendingEntries->PackEvent(buffer, 4096);
            CGChat::AddCombatMessage(buffer);
        }

        entry = s_entries.Next(s_pendingEntries);

        if (!entry) {
            break;
        }

        s_pendingEntries = entry;

        if (entry->m_pendingMask) {
            lua_settop(L, -2);
            return;
        }
    }

    s_pendingEntries = NULL;
    lua_settop(L, -2);
}

// OFFSET: 0x005F5E60/0x140279680
PendingCombatLogData* GetPendingCombatLogData(CombatLogEntry* entry, const WOWGUID& guid, uint32_t mask) {
    PendingCombatLogData* data = s_freePendingData.Head();

    if (!data) {
        data = s_freePendingData.NewNode(2, 0, 0);
    }

    data->m_entry = entry;
    data->m_guid = guid;
    data->m_id = 0;
    data->m_type = 0;
    data->m_mask = mask;
    return data;
}

// OFFSET: 0x005F5F10/0x140279790
void UnitCombatLogInvalidateName(const char* name) {
    InvalidatedName* invalidated = s_invalidatedNames.NewNode(2, 0, 0);
    invalidated->m_name = SStrDupA(name, __FILE__, __LINE__);

    for (CombatLogEntry* entry = s_entries.Head(); entry; entry = s_entries.Next(entry)) {
        if (entry->m_sourceName == name) {
            entry->m_sourceName = invalidated->m_name;
        }

        if (entry->m_destName == name) {
            entry->m_destName = invalidated->m_name;
        }
    }
}

// OFFSET: 0x005F61F0/0x140279B10
CombatLogFilter::CombatLogFilter() {
    this->m_events.m_count = 50;
    this->m_events.m_bits.SetCount(((50 - 1) >> 5) + 1);
}

// OFFSET: 0x005F6250/0x140279BA0
const char* CombatLogEntry::ResolveName(PendingCombatLogData* data, const char** realm, bool pending) {
    void (*callback)(int32_t, const WOWGUID&, void*, bool) = pending ? CombatLogEntry::PendingCombatLogResolved : NULL;
    const char* name = NULL;

    switch (data->m_type) {
        case 1: {
            NameCache* cache = g_nameDBCache.GetRecord(data->m_guid.m_guid, data->m_guid, callback, data, false);

            if (cache) {
                if (cache->m_realmName[0] && realm) {
                    *realm = cache->m_realmName;
                }

                name = cache->m_name;
            }

            break;
        }

        case 2: {
            PetNameCache* pet = g_petNameCache.GetRecord(data->m_id, data->m_guid, callback, data, false);

            if (!pet) {
                break;
            }

            CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(data->m_guid, TYPEMASK_UNIT, __FILE__, __LINE__));

            if (unit && pet->m_timestamp != unit->m_unit->m_unitFieldPetNameTimestamp) {
                UnitCombatLogInvalidateName(pet->m_name);
                g_petNameCache.Invalidate(data->m_id);
                g_petNameCache.GetRecord(data->m_id, data->m_guid, callback, data, false);
                break;
            }

            name = pet->m_name;
            break;
        }

        case 3: {
            CreatureStats_C* stats = g_creatureDBCache.GetRecord(data->m_id, data->m_guid, callback, data, false);

            if (!stats) {
                break;
            }

            name = stats->GetName(0);
            CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(data->m_guid, TYPEMASK_UNIT, __FILE__, __LINE__));

            if (unit && unit->m_unit->m_unitFieldBytes0[2] == 1 && stats->GetNameAlt(0)) {
                name = stats->GetNameAlt(0);
            }

            break;
        }

        case 4: {
            GameObjectStats_C* stats = g_gameObjectDBCache.GetRecord(data->m_id, data->m_guid, callback, data, false);

            if (stats) {
                name = stats->GetName(0);
            }

            break;
        }

        case 5: {
            ItemRecSparse* stats = g_itemSparseDB.GetRecord(data->m_id, data->m_guid, callback, data, false);

            if (stats) {
                name = stats->GetName(0);
            }

            break;
        }
    }

    if (name) {
        return name;
    }

    if (!s_unknownObjectName[0]) {
        SStrCopy(s_unknownObjectName, FrameScript_GetText("UNKNOWNOBJECT", -1, GENDER_NOT_APPLICABLE), sizeof(s_unknownObjectName));
    }

    if (pending) {
        this->m_pendingMask |= data->m_mask;
        s_pendingData.LinkToTail(data);
    }

    return s_unknownObjectName;
}

// OFFSET: 0x005F64E0/0x140279F00
void CombatLogEntry::PendingCombatLogResolved(int32_t id, const WOWGUID& guid, void* param, bool success) {
    PendingCombatLogData* data = static_cast<PendingCombatLogData*>(param);
    CombatLogEntry* entry = data->m_entry;

    if (success) {
        const char* realm = NULL;
        const char* name = entry->ResolveName(data, &realm, false);

        switch (data->m_mask) {
            case 1:
                entry->m_sourceName = name;
                entry->m_sourceRealm = realm;
                break;

            case 2:
                entry->m_destName = name;
                entry->m_destRealm = realm;
                break;

            case 4:
                entry->m_info.m_names[1] = name;
                break;
        }
    }

    entry->m_pendingMask &= ~data->m_mask;

    if (!entry->m_pendingMask) {
        CheckToSanitizeUnitLowGuid(&entry->m_sourceGuid);
        CheckToSanitizeUnitLowGuid(&entry->m_destGuid);

        if (s_pendingEntries == entry) {
            SendCombatLogEvents();
        }
    }

    s_freePendingData.LinkToTail(data);
}

// OFFSET: 0x005F67F0/0x14027A2D0
const char* CombatLogEntry::GetObjectName(const WOWGUID& guid, uint32_t mask, const char** realm) {
    PendingCombatLogData* data = GetPendingCombatLogData(this, guid, mask);
    uint32_t high = guid.m_guid >> 32;

    if (!(high & 0xF0000000) && ((high & 0xF07FFFFF) || static_cast<uint32_t>(guid.m_guid))) {
        data->m_type = 1;
        return this->ResolveName(data, realm, true);
    }

    uint32_t type = high & 0xF0F00000;

    if (type == 0xF1300000 || type == 0xF1500000) {
        CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));

        if (unit) {
            uint32_t petNumber = unit->m_unit->m_unitFieldPetnumber;

            if (petNumber) {
                data->m_id = petNumber;
                data->m_type = 2;
                return this->ResolveName(data, realm, true);
            }

            data->m_type = 3;
            data->m_id = unit->m_obj->m_entryID;
            return this->ResolveName(data, realm, true);
        }

        data->m_type = 3;
        data->m_id = high & 0xFFFFF;
        return this->ResolveName(data, realm, true);
    }

    if (type == 0xF1400000) {
        data->m_id = (guid.m_guid >> 24) & 0xFFFFFFF;
        data->m_type = 2;
        return this->ResolveName(data, realm, true);
    }

    if (type == 0xF1100000) {
        data->m_type = 4;
        data->m_id = high & 0xFFFFF;
        return this->ResolveName(data, realm, true);
    }

    if (!s_unknownObjectName[0]) {
        SStrCopy(s_unknownObjectName, FrameScript_GetText("UNKNOWNOBJECT", -1, GENDER_NOT_APPLICABLE), sizeof(s_unknownObjectName));
    }

    return s_unknownObjectName;
}

// OFFSET: 0x005F6980/0x14027A490
void CombatLogEntry::CancelPendingCallbacks() {
    PendingCombatLogData* data = s_pendingData.Head();

    while (data) {
        PendingCombatLogData* next = s_pendingData.Next(data);

        if (data->m_entry == this) {
            switch (data->m_type) {
                case 1:
                    g_nameDBCache.CancelCallback(data->m_guid.m_guid, CombatLogEntry::PendingCombatLogResolved, data);
                    break;

                case 2:
                    g_petNameCache.CancelCallback(data->m_id, CombatLogEntry::PendingCombatLogResolved, data);
                    break;

                case 3:
                    g_creatureDBCache.CancelCallback(data->m_id, CombatLogEntry::PendingCombatLogResolved, data);
                    break;

                case 4:
                    g_gameObjectDBCache.CancelCallback(data->m_id, CombatLogEntry::PendingCombatLogResolved, data);
                    break;

                case 5:
                    g_itemSparseDB.CancelCallback(data->m_id, CombatLogEntry::PendingCombatLogResolved, data);
                    break;
            }

            s_pendingData.DeleteNode(data);
        }

        data = next;
    }
}

// OFFSET: 0x005F6AA0/0x14027A610
void CombatLogEntry::SetSource(const WOWGUID& guid) {
    this->m_sourceGuid = guid;
    this->m_sourceName = this->GetObjectName(guid, 1, &this->m_sourceRealm);
    this->m_sourceFlags = GetCombatLogObjectFlags(guid);
    this->m_sourceRaidFlags = GetCombatLogObjectRaidFlags(guid);
    this->m_hideCaster = false;
}

// OFFSET: 0x005F6AF0/0x14027A6A0
void CombatLogEntry::SetExecuteItem(int32_t itemId) {
    this->m_infoFlags |= 0x200;
    this->m_info.m_values[0] = itemId;
    PendingCombatLogData* data = GetPendingCombatLogData(this, WOWGUID(), 4);
    data->m_id = itemId;
    data->m_type = 5;
    this->m_info.m_names[1] = this->ResolveName(data, NULL, true);
}

// OFFSET: 0x005F6B50/0x14027A710
void RecycleCombatLogEntry(CombatLogEntry* entry) {
    if (s_pendingEntries == entry) {
        s_pendingEntries = s_entries.Next(entry);
    }

    if (s_currentEntry == entry) {
        s_currentEntry = s_entries.Next(entry);
    }

    entry->CancelPendingCallbacks();
    s_freeEntries.LinkToTail(entry);
}

// OFFSET: 0x005F6BC0/0x14027A7F0
CombatLogEntry* GetCombatLogEntry(const WOWGUID& source, const WOWGUID& dest, int32_t event, const SpellRec* spell) {
    CombatLogEntry* entry = s_freeEntries.Head();

    if (!entry) {
        CombatLogEntry* oldest = s_entries.Head();

        if (oldest && static_cast<int32_t>(FrameTime::GetCurTimeMs() - 1000 * g_combatLogRetentionTimeCVar->m_intValue - oldest->m_timestamp) >= 0) {
            RecycleCombatLogEntry(oldest);
            entry = oldest;
        } else {
            entry = s_freeEntries.NewNode(2, 0, 0);
        }
    }

    entry->Clear();
    entry->m_event = event;

    if (source) {
        if (spell && (spell->m_attributesExD & 0x2000)) {
            entry->m_sourceFlags = GetCombatLogObjectFlags(source);
            entry->m_sourceRaidFlags = GetCombatLogObjectRaidFlags(source);
        } else {
            entry->SetSource(source);
        }
    }

    if (dest) {
        entry->m_destGuid = dest;
        entry->m_destName = entry->GetObjectName(dest, 2, &entry->m_destRealm);
        entry->m_destFlags = GetCombatLogObjectFlags(dest);
        entry->m_destRaidFlags = GetCombatLogObjectRaidFlags(dest);
    }

    if (spell) {
        entry->m_spellId = spell->m_id;
        entry->m_spellName.Copy(spell->m_name);
    }

    return entry;
}

// OFFSET: 0x005F6CB0/0x14027A9F0
void QueueCombatLogEntry(CombatLogEntry* entry, uint32_t timeOffset) {
    entry->m_timestamp = FrameTime::GetCurTimeMs() - timeOffset;
    s_entries.LinkToTail(entry);

    if (!s_pendingEntries) {
        s_pendingEntries = entry;

        if (entry->m_pendingMask) {
            return;
        }

        CheckToSanitizeUnitLowGuid(&entry->m_sourceGuid);
        CheckToSanitizeUnitLowGuid(&entry->m_destGuid);
        SendCombatLogEvents();
        return;
    }

    if (s_pendingEntries->m_pendingMask && static_cast<int32_t>(FrameTime::GetCurTimeMs() - 3000 - s_pendingEntries->m_timestamp) >= 0) {
        CombatLogEntry* pending = s_pendingEntries;
        pending->CancelPendingCallbacks();
        pending->m_pendingMask = 0;
        CheckToSanitizeUnitLowGuid(&s_pendingEntries->m_sourceGuid);
        CheckToSanitizeUnitLowGuid(&s_pendingEntries->m_destGuid);
        SendCombatLogEvents();
    }
}

// OFFSET: 0x005F6D40/0x14027AB00
static void SendMissEvent(const SpellRec* spell, const WOWGUID& source, const WOWGUID& dest, int32_t event, int32_t missType, int32_t amount, int32_t timeOffset, bool offHand) {
    CombatLogEntry* entry = GetCombatLogEntry(source, dest, event, spell);
    entry->SetMissInfo(missType, amount, offHand);

    if (s_combatTextActiveUnit == dest) {
        bool isSpell = event == 8;

        switch (missType) {
            case 1:
                SendToCombatText(isSpell ? 15 : 3);
                break;

            case 2:
                SendToCombatText(isSpell ? 22 : 10);
                break;

            case 3:
                SendToCombatText(isSpell ? 16 : 4);
                break;

            case 4:
                SendToCombatText(isSpell ? 17 : 5);
                break;

            case 5:
                SendToCombatText(isSpell ? 23 : 11);
                break;

            case 6:
                SendToCombatText(isSpell ? 18 : 6);
                break;

            case 7:
            case 8:
                SendToCombatText(isSpell ? 19 : 7);
                break;

            case 9:
                SendToCombatText(isSpell ? 20 : 8);
                break;

            case 10:
                SendToCombatText(isSpell ? 24 : 12);
                break;

            case 11:
                SendToCombatText(isSpell ? 21 : 9);
                break;
        }
    }

    QueueCombatLogEntry(entry, timeOffset);
}

// OFFSET: 0x005F6E60/0x14027AD90
static void SendDamageEvent(const WOWGUID& dest, int32_t event, const WOWGUID& source, const SpellRec* spell, int32_t damage, int32_t overkill, int32_t school, int32_t resisted, int32_t blocked, int32_t absorbed, bool critical, bool glancing, bool crushing, int32_t timeOffset, bool offHand) {
    CGUnit_C* unit;

    if (event != 1 && (unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(dest, TYPEMASK_UNIT, __FILE__, __LINE__))) != NULL) {
        unit->ModifyPredictedHealth(-damage);
    }

    CombatLogEntry* entry = GetCombatLogEntry(source, dest, event, spell);
    entry->m_info.m_values[2] = overkill;
    entry->m_info.m_values[3] = school;
    entry->m_info.m_values[5] = resisted;
    entry->m_info.m_values[6] = blocked;
    entry->m_info.m_values[4] = absorbed;
    entry->m_infoFlags |= 0x10;
    entry->m_critical = critical;
    entry->m_glancing = glancing;
    entry->m_crushing = crushing;
    entry->m_isOffHand = offHand;
    entry->m_info.m_values[1] = damage;
    QueueCombatLogEntry(entry, timeOffset);

    if (s_combatTextActiveUnit != dest) {
        return;
    }

    if (resisted > 0) {
        SendToCombatText(event == 9 ? 22 : 10, damage, resisted);
    } else if (blocked) {
        SendToCombatText(event == 9 ? 23 : 11, damage, blocked);
    } else if (absorbed) {
        SendToCombatText(event == 9 ? 24 : 12, damage, absorbed);
    } else if (critical) {
        SendToCombatText(event == 9 ? 13 : 1, damage);
    } else if (event == 40) {
        SendToCombatText(39, damage);
    } else {
        SendToCombatText(event == 9 ? 14 : 2, damage);
    }
}

// OFFSET: 0x005F7010/0x14027AFF0
static void SendHealEvent(const SpellRec* spell, const WOWGUID& source, int32_t event, const WOWGUID& dest, int32_t amount, int32_t overheal, int32_t absorbed, bool critical, int32_t timeOffset) {
    CombatLogEntry* entry = GetCombatLogEntry(source, dest, event, spell);
    entry->m_infoFlags |= 0x20;
    entry->m_info.m_values[2] = absorbed;
    entry->m_info.m_values[1] = overheal;
    entry->m_info.m_values[0] = amount;
    entry->m_info.m_bytes[12] = critical;
    QueueCombatLogEntry(entry, timeOffset);

    if (event == 35) {
        SpellDurationRec* duration = g_spellDurationDB.GetRecord(spell->m_durationIndex);

        if (duration) {
            int32_t maxDuration = duration->m_maxDuration;
            int32_t length = duration->m_duration + duration->m_durationPerLevel * Spell_C_GetSpellLevel(spell, 0, 0);

            if (length < maxDuration) {
                maxDuration = duration->m_duration + duration->m_durationPerLevel * Spell_C_GetSpellLevel(spell, 0, 0);
            }

            if (maxDuration > 30000) {
                return;
            }
        }
    }

    if (s_combatTextActiveUnit == dest) {
        if (absorbed) {
            if (event == 35) {
                FrameScript_SignalEvent(SCRIPT_EVENT_COMBAT_TEXT_UPDATE, "%s%s%d%d", s_combatTextTypes[44], GetCombatLogObjectName(source), amount, absorbed);
            } else {
                SendToCombatText(critical ? 45 : 46, GetCombatLogObjectName(source), amount, absorbed);
            }
        } else if (event == 35) {
            FrameScript_SignalEvent(SCRIPT_EVENT_COMBAT_TEXT_UPDATE, "%s%s%d", s_combatTextTypes[27], GetCombatLogObjectName(source), amount);
        } else {
            SendToCombatText(critical ? 37 : 38, GetCombatLogObjectName(source), amount);
        }

        return;
    }

    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(dest, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (unit) {
        unit->AddWorldHealingText(source, amount, spell, critical, 0);
    }
}

// OFFSET: 0x005F71D0/0x14027B230
static void SendEnergizeEvent(const WOWGUID& dest, int32_t powerType, int32_t event, const WOWGUID& source, const SpellRec* spell, int32_t amount, int32_t timeOffset) {
    int32_t alternatePower = -1;
    int32_t displayAmount = amount / static_cast<int32_t>(Spell_C_GetPowerDisplayMod(powerType));

    if (powerType == 10) {
        CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(dest, TYPEMASK_PLAYER, __FILE__, __LINE__));

        if (player) {
            alternatePower = player->m_powerBarID;
        }
    }

    CombatLogEntry* entry = GetCombatLogEntry(source, dest, event, spell);
    entry->m_infoFlags |= 0x40;
    entry->m_info.m_values[0] = displayAmount;
    entry->m_info.m_values[1] = powerType;
    entry->m_info.m_values[2] = alternatePower;
    QueueCombatLogEntry(entry, timeOffset);

    if (s_combatTextActiveUnit == dest) {
        FrameScript_SignalEvent(SCRIPT_EVENT_COMBAT_TEXT_UPDATE, "%s%d%s", s_combatTextTypes[(event == 36) + 28], displayAmount, Spell_C_GetPowerTypeToken(powerType));
    }
}

// OFFSET: 0x005F72A0/0x14027B340
static void AddPowerDrainEntry(const WOWGUID& dest, int32_t event, const WOWGUID& source, const SpellRec* spell, int32_t amount, int32_t powerType, int32_t extraAmount, int32_t timeOffset) {
    uint32_t mod = Spell_C_GetPowerDisplayMod(powerType);
    int32_t displayAmount = amount / static_cast<int32_t>(mod);
    int32_t alternatePower = -1;
    int32_t displayExtra = extraAmount / static_cast<int32_t>(mod);

    if (powerType == 10) {
        CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(dest, TYPEMASK_PLAYER, __FILE__, __LINE__));

        if (player) {
            alternatePower = player->m_powerBarID;
        }
    }

    CombatLogEntry* entry = GetCombatLogEntry(source, dest, event, spell);
    entry->m_infoFlags |= 0x80;
    entry->m_info.m_values[2] = displayExtra;
    entry->m_info.m_values[0] = displayAmount;
    entry->m_info.m_values[1] = powerType;
    entry->m_info.m_values[3] = alternatePower;
    QueueCombatLogEntry(entry, timeOffset);

    if (s_combatTextActiveUnit == dest && displayAmount) {
        FrameScript_SignalEvent(SCRIPT_EVENT_COMBAT_TEXT_UPDATE, "%s%d%s", s_combatTextTypes[28], displayAmount, Spell_C_GetPowerTypeToken(powerType));
    }

    if (s_combatTextActiveUnit == source && displayExtra) {
        FrameScript_SignalEvent(SCRIPT_EVENT_COMBAT_TEXT_UPDATE, "%s%d%s", s_combatTextTypes[(event == 38) + 28], displayExtra, Spell_C_GetPowerTypeToken(powerType));
    }
}

// OFFSET: 0x005F73D0/0x14027B4B0
static int32_t Packet_SMSG_PARTY_KILL_LOG(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    uint64_t value = 0;
    msg->Get(value);
    WOWGUID source = WOWGUID::Make(value);
    value = 0;
    msg->Get(value);
    WOWGUID dest = WOWGUID::Make(value);
    CombatLogEntry* entry = GetCombatLogEntry(source, dest, 43, NULL);
    QueueCombatLogEntry(entry, 0);
    return 1;
}

// OFFSET: 0x005F7440/0x14027B530
static int32_t Packet_SMSG_PROC_RESIST(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    uint64_t value = 0;
    msg->Get(value);
    WOWGUID source = WOWGUID::Make(value);
    value = 0;
    msg->Get(value);
    WOWGUID dest = WOWGUID::Make(value);
    uint32_t spellId = 0;
    msg->Get(spellId);
    uint8_t debug = 0;
    msg->Get(debug);
    const SpellRec* spell = g_spellDB.GetRecord(spellId);

    if (!spell || (spell->m_attributes & 0x180) || !*spell->m_name) {
        return 1;
    }

    CombatLogEntry* entry = GetCombatLogEntry(source, dest, 8, spell);
    entry->m_infoFlags |= 0x4004;
    entry->m_info.m_values[0] = 2;
    entry->m_info.m_bytes[4] = 0;
    entry->m_info.m_values[5] = 0;

    if (s_combatTextActiveUnit == dest) {
        SendToCombatText(22);
    }

    QueueCombatLogEntry(entry, 0);
    return 1;
}

// OFFSET: 0x005F7540
static int32_t Packet_SMSG_DISPEL_FAILED(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    uint64_t value = 0;
    msg->Get(value);
    WOWGUID source = WOWGUID::Make(value);
    value = 0;
    msg->Get(value);
    WOWGUID dest = WOWGUID::Make(value);
    uint32_t spellId = 0;
    msg->Get(spellId);
    const SpellRec* spell = g_spellDB.GetRecord(spellId);

    if (!spell || (spell->m_attributes & 0x180) || !*spell->m_name || msg->IsRead()) {
        return 1;
    }

    do {
        uint32_t dispelledId = 0;
        msg->Get(dispelledId);
        const SpellRec* dispelled = g_spellDB.GetRecord(dispelledId);

        if (dispelled && !(dispelled->m_attributes & 0x180) && *dispelled->m_name) {
            CombatLogEntry* entry = GetCombatLogEntry(source, dest, 39, spell);
            entry->m_infoFlags |= 0x100;
            entry->m_info.m_values[0] = dispelledId;
            QueueCombatLogEntry(entry, 0);
        }
    } while (!msg->IsRead());

    return 1;
}

// OFFSET: 0x005F7680/0x14027B800
static int32_t Packet_SMSG_ENCHANTMENT_LOG(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    SmartGUID target;
    SmartGUID caster;
    *msg >> target;
    *msg >> caster;
    uint32_t itemId;
    uint32_t enchantId;
    msg->Get(itemId);
    msg->Get(enchantId);
    SpellItemEnchantmentRec* enchant = g_spellItemEnchantmentDB.GetRecord(enchantId);

    if (!enchant || (enchant->m_flags & 0x2)) {
        return 1;
    }

    WOWGUID source = caster;
    CombatLogEntry* entry = GetCombatLogEntry(source, target, source ? 31 : 32, NULL);
    entry->m_infoFlags |= 0x1000;
    entry->m_info.m_names[2] = enchant->m_name;
    entry->SetExecuteItem(itemId);
    QueueCombatLogEntry(entry, 0);
    return 1;
}

// OFFSET: 0x005F7790/0x14027B950
void UnitCombatLog(const WOWGUID& guid, int32_t type, int32_t damage, int32_t absorbed, int32_t resisted) {
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (unit) {
        unit->ModifyPredictedHealth(-damage);
        unit->TriggerNamePlateDamageFeedback();
    }

    CombatLogEntry* entry = GetCombatLogEntry(WOWGUID(), guid, 0, NULL);
    entry->m_infoFlags |= 0x8;
    entry->m_info.m_values[0] = type;
    entry->m_critical = 0;
    entry->m_glancing = 0;
    entry->m_crushing = 0;
    entry->m_isOffHand = 0;
    entry->m_infoFlags |= 0x10;
    entry->m_info.m_values[3] = 1 << s_environmentalDamageSchools[type];
    entry->m_info.m_values[1] = damage;
    entry->m_info.m_values[2] = 0;
    entry->m_info.m_values[5] = resisted;
    entry->m_info.m_values[6] = 0;
    entry->m_info.m_values[4] = absorbed;
    QueueCombatLogEntry(entry, 0);

    if (s_combatTextActiveUnit != guid) {
        return;
    }

    if (resisted) {
        SendToCombatText(10, damage, resisted);
    } else if (absorbed) {
        SendToCombatText(12, damage, absorbed);
    } else {
        SendToCombatText(2, damage);
    }
}

// OFFSET: 0x005F78B0
void UnitCombatLog(const ATTACKROUNDINFO& info) {
    CGUnit_C* attacker;

    if ((info.m_hitInfo & 0x800000) && (attacker = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(info.m_attacker, TYPEMASK_UNIT, __FILE__, __LINE__))) != NULL) {
        attacker->ModifyPredictedPower(1, info.m_rageGained);
    }

    if (!ShouldShowFeedback(info)) {
        return;
    }

    uint32_t state = info.m_victimState;
    uint32_t hitInfo = info.m_hitInfo;
    bool offHand = (hitInfo & 0x4) != 0;

    if ((state == 1 || state == 5) && info.m_damage) {
        int32_t resisted = 0;
        int32_t absorbed = 0;

        for (uint32_t i = 0; i < 2 && info.m_schoolMask[i]; i++) {
            resisted += info.m_resisted[i];
            absorbed += info.m_absorbed[i];
        }

        WOWGUID attackerGuid = info.m_attacker;
        WOWGUID targetGuid = info.m_target;
        int32_t blocked = info.m_blocked;
        bool critical = (hitInfo & 0x200) != 0;
        CombatLogEntry* entry = GetCombatLogEntry(attackerGuid, targetGuid, 1, NULL);
        entry->m_info.m_values[2] = info.m_overkill;
        entry->m_info.m_values[3] = info.m_schoolMask[0];
        entry->m_info.m_values[4] = absorbed;
        entry->m_infoFlags |= 0x10;
        entry->m_critical = critical;
        entry->m_glancing = (hitInfo & 0x10000) != 0;
        entry->m_crushing = (hitInfo & 0x20000) != 0;
        entry->m_isOffHand = offHand;
        entry->m_info.m_values[1] = info.m_damage;
        entry->m_info.m_values[5] = resisted;
        entry->m_info.m_values[6] = blocked;
        QueueCombatLogEntry(entry, 0);

        if (s_combatTextActiveUnit != targetGuid) {
            return;
        }

        if (resisted > 0) {
            FrameScript_SignalEvent(SCRIPT_EVENT_COMBAT_TEXT_UPDATE, "%s%d%d", s_combatTextTypes[10], info.m_damage, resisted);
        } else if (blocked) {
            SendToCombatText(11, info.m_damage, blocked);
        } else if (absorbed) {
            SendToCombatText(12, info.m_damage, absorbed);
        } else if (critical) {
            SendToCombatText(1, info.m_damage);
        } else {
            SendToCombatText(2, info.m_damage);
        }

        return;
    }

    WOWGUID attackerGuid = info.m_attacker;
    WOWGUID targetGuid = info.m_target;

    if (hitInfo & 0x10) {
        CombatLogEntry* entry = GetCombatLogEntry(attackerGuid, targetGuid, 2, NULL);
        entry->m_infoFlags |= 0x4;
        entry->m_info.m_values[0] = 1;
        entry->m_info.m_bytes[4] = offHand;

        if (s_combatTextActiveUnit == targetGuid) {
            SendToCombatText(3);
        }

        QueueCombatLogEntry(entry, 0);
        return;
    }

    if (hitInfo & 0x20) {
        int32_t absorbed = 0;

        for (uint32_t i = 0; i < 2 && info.m_schoolMask[i]; i++) {
            absorbed += info.m_absorbed[i];
        }

        CombatLogEntry* entry = GetCombatLogEntry(attackerGuid, targetGuid, 2, NULL);
        entry->m_infoFlags |= 0x2004;
        entry->m_info.m_values[0] = 10;
        entry->m_info.m_bytes[4] = offHand;
        entry->m_info.m_values[4] = absorbed;

        if (s_combatTextActiveUnit == targetGuid) {
            SendToCombatText(12);
        }

        QueueCombatLogEntry(entry, 0);
        return;
    }

    if (hitInfo & 0x80) {
        int32_t resisted = 0;

        for (uint32_t i = 0; i < 2 && info.m_schoolMask[i]; i++) {
            resisted += info.m_resisted[i];
        }

        CombatLogEntry* entry = GetCombatLogEntry(attackerGuid, targetGuid, 2, NULL);
        entry->m_infoFlags |= 0x4004;
        entry->m_info.m_values[0] = 2;
        entry->m_info.m_bytes[4] = offHand;
        entry->m_info.m_values[5] = resisted;

        if (s_combatTextActiveUnit == targetGuid) {
            SendToCombatText(10);
        }

        QueueCombatLogEntry(entry, 0);
        return;
    }

    int32_t missType;
    int32_t textType;

    switch (state) {
        case 2:
            missType = 3;
            textType = 4;
            break;

        case 3:
            missType = 4;
            textType = 5;
            break;

        case 5: {
            int32_t blocked = info.m_blocked;
            CombatLogEntry* entry = GetCombatLogEntry(attackerGuid, targetGuid, 2, NULL);
            entry->m_infoFlags |= 0x8004;
            entry->m_info.m_values[0] = 5;
            entry->m_info.m_bytes[4] = offHand;
            entry->m_info.m_values[6] = blocked;

            if (s_combatTextActiveUnit == targetGuid) {
                SendToCombatText(11);
            }

            QueueCombatLogEntry(entry, 0);
            return;
        }

        case 6:
            missType = 6;
            textType = 6;
            break;

        case 7:
            missType = 7;
            textType = 7;
            break;

        case 8:
            missType = 9;
            textType = 8;
            break;

        default:
            return;
    }

    CombatLogEntry* entry = GetCombatLogEntry(attackerGuid, targetGuid, 2, NULL);
    entry->m_infoFlags |= 0x4;
    entry->m_info.m_values[0] = missType;
    entry->m_info.m_bytes[4] = offHand;

    if (s_combatTextActiveUnit == targetGuid) {
        SendToCombatText(textType);
    }

    QueueCombatLogEntry(entry, 0);
}

// OFFSET: 0x005F7F10/0x14027C2C0
void UnitCombatLogCastStart(const WOWGUID& caster, const WOWGUID& target, const SpellRec* spell) {
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(caster, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (!unit) {
        return;
    }

    if (unit->m_unit->m_unitFieldFlags2 & 0x2000000) {
        CGUnit_C* targetUnit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(target, TYPEMASK_UNIT, __FILE__, __LINE__));

        if (!targetUnit || !(targetUnit->m_unit->m_unitFieldFlags & 0x8)) {
            return;
        }
    }

    if (!spell || (spell->m_attributes & 0x180) || !*spell->m_name || SpellHasLearnSpellEffect(spell) || (spell->m_attributesExD & 0x1)) {
        return;
    }

    if (unit->GetSpellCastingTime(spell)) {
        CombatLogEntry* entry = GetCombatLogEntry(caster, WOWGUID(), 5, spell);
        QueueCombatLogEntry(entry, 0);
    }
}

// OFFSET: 0x005F7FF0/0x14027C3B0
void UnitCombatLogCastGo(const WOWGUID& caster, const WOWGUID& target, const SpellRec* spell) {
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(caster, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (!unit) {
        return;
    }

    if (unit->m_unit->m_unitFieldFlags2 & 0x2000000) {
        CGUnit_C* targetUnit;

        if (!target || (targetUnit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(target, TYPEMASK_UNIT, __FILE__, __LINE__))) == NULL || !(targetUnit->m_unit->m_unitFieldFlags & 0x8)) {
            return;
        }
    }

    if (!spell || (spell->m_attributes & 0x180) || !*spell->m_name || SpellHasLearnSpellEffect(spell) || (spell->m_attributesExD & 0x1) || SpellHasOpenLockEffect(spell)) {
        return;
    }

    if (!unit->GetSpellCastingTime(spell)) {
        CombatLogEntry* entry = GetCombatLogEntry(caster, target, 6, spell);
        QueueCombatLogEntry(entry, 0);
    }

    if (IsCombatTextActiveUnit(caster) && (spell->m_attributesExF & 0x40)) {
        SendToCombatText(30, spell->m_name);
    }
}

// OFFSET: 0x005F8110/0x14027C500
void UnitCombatLogSpellFail(const WOWGUID& caster, const SpellRec* spell, const char* reason) {
    if (!reason || !*reason || !spell || (spell->m_attributes & 0x80) || !*spell->m_name) {
        return;
    }

    CombatLogEntry* entry = GetCombatLogEntry(caster, WOWGUID(), 7, spell);
    entry->SetFailReason(reason);
    QueueCombatLogEntry(entry, 0);
}

// OFFSET: 0x005F8170/0x14027C580
void UnitCombatLogSpellMissed(const WOWGUID& casterGuid, const WOWGUID& targetGuid, const SpellRec* spell, uint32_t missType, bool damageShield, bool periodic, int32_t timeOffset) {
    if (!missType || !spell || (spell->m_attributes & 0x180) || !*spell->m_name) {
        return;
    }

    CGUnit_C* caster = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(casterGuid, TYPEMASK_UNIT, __FILE__, __LINE__));
    CGUnit_C* target = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(targetGuid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (caster && (caster->m_unit->m_unitFieldFlags2 & 0x2000000) && target && !(target->m_unit->m_unitFieldFlags & 0x8)) {
        return;
    }

    int32_t event;

    if (periodic) {
        event = 33;
    } else if (damageShield) {
        event = 41;
    } else {
        event = (spell->m_attributesExC & 0x8000) ? 4 : 8;
    }

    SendMissEvent(spell, casterGuid, targetGuid, event, missType, 0, timeOffset, false);
}

// OFFSET: 0x005F8260/0x14027C6C0
void UnitCombatLogSpellDamage(const WOWGUID& source, const WOWGUID& dest, const SpellRec* spell, int32_t damage, int32_t overkill, int32_t school, int32_t absorbed, int32_t resisted, int32_t blocked, int32_t hitInfo, bool periodic, int32_t timeOffset) {
    if (!spell || (spell->m_attributes & 0x180) || !*spell->m_name) {
        return;
    }

    if (damage) {
        CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(dest, TYPEMASK_UNIT, __FILE__, __LINE__));

        if (unit) {
            unit->TriggerNamePlateDamageFeedback();
        }

        int32_t event = periodic ? 34 : ((spell->m_attributesExC & 0x8000) ? 3 : 9);
        SendDamageEvent(dest, event, source, spell, damage, overkill, school, resisted, blocked, absorbed, (hitInfo & 0x2) != 0, false, false, timeOffset, false);
        return;
    }

    int32_t event = periodic ? 33 : ((spell->m_attributesExC & 0x8000) ? 4 : 8);
    CombatLogEntry* entry;

    if (absorbed) {
        entry = GetCombatLogEntry(source, dest, event, spell);
        entry->m_infoFlags |= 0x2004;
        entry->m_info.m_values[0] = 10;
        entry->m_info.m_bytes[4] = 0;
        entry->m_info.m_values[4] = absorbed;

        if (s_combatTextActiveUnit == dest) {
            SendToCombatText(event == 8 ? 24 : 12);
        }
    } else if (blocked) {
        entry = GetCombatLogEntry(source, dest, event, spell);
        entry->m_infoFlags |= 0x8004;
        entry->m_info.m_values[0] = 5;
        entry->m_info.m_bytes[4] = 0;
        entry->m_info.m_values[6] = blocked;

        if (s_combatTextActiveUnit == dest) {
            SendToCombatText(event == 8 ? 23 : 11);
        }
    } else {
        if (!resisted) {
            return;
        }

        entry = GetCombatLogEntry(source, dest, event, spell);
        entry->m_infoFlags |= 0x4004;
        entry->m_info.m_values[0] = 2;
        entry->m_info.m_bytes[4] = 0;
        entry->m_info.m_values[5] = resisted;

        if (s_combatTextActiveUnit == dest) {
            SendToCombatText(event == 8 ? 22 : 10);
        }
    }

    QueueCombatLogEntry(entry, 0);
}

// OFFSET: 0x005F8460
void UnitCombatLogHeal(const WOWGUID& casterGuid, const WOWGUID& targetGuid, const SpellRec* spell, int32_t amount, int32_t overheal, int32_t absorbed, bool crit, bool periodic, int32_t timeOffset) {
    if (!amount && !overheal && !absorbed) {
        return;
    }

    CGUnit_C* target = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(targetGuid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (target) {
        target->SubtractChanneledHealPrediction(spell->m_id, casterGuid, absorbed + amount + overheal);
        target->ModifyPredictedHealth(amount);
    }

    if (spell && !(spell->m_attributes & 0x180) && *spell->m_name && (!(spell->m_attributesExJ & 0x8) || overheal != amount)) {
        SendHealEvent(spell, casterGuid, periodic ? 35 : 10, targetGuid, amount, overheal, absorbed, crit, timeOffset);
    }
}

// OFFSET: 0x005F8520/0x14027CAA0
void UnitCombatLogEnergize(const WOWGUID& casterGuid, const WOWGUID& targetGuid, const SpellRec* spell, int32_t powerType, int32_t amount, bool periodic, int32_t timeOffset) {
    if (!amount) {
        return;
    }

    CGUnit_C* target = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(targetGuid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (target) {
        target->ModifyPredictedPower(powerType, amount);
    }

    if (spell && !(spell->m_attributes & 0x180) && *spell->m_name) {
        SendEnergizeEvent(targetGuid, powerType, periodic ? 36 : 11, casterGuid, spell, amount, timeOffset);
    }
}

// OFFSET: 0x005F85A0/0x14027CB70
void UnitCombatLogPowerDrain(const WOWGUID& casterGuid, const WOWGUID& targetGuid, const SpellRec* spell, int32_t powerType, int32_t amount, float multiplier, bool periodic, int32_t timeOffset, bool leech) {
    if (!amount) {
        return;
    }

    CGUnit_C* target = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(targetGuid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (target) {
        target->ModifyPredictedPower(powerType, -amount);
    }

    if (!spell || (spell->m_attributes & 0x180) || !*spell->m_name) {
        return;
    }

    int32_t extra;

    if (leech) {
        extra = static_cast<int32_t>(static_cast<float>(amount) * multiplier);

        if (extra) {
            AddPowerDrainEntry(targetGuid, periodic ? 38 : 13, casterGuid, spell, amount, powerType, extra, timeOffset);
            return;
        }
    } else {
        extra = 0;
    }

    AddPowerDrainEntry(targetGuid, periodic ? 37 : 12, casterGuid, spell, amount, powerType, extra, timeOffset);
}

// OFFSET: 0x005F8660/0x14027CC80
void UnitCombatLogInstakill(const WOWGUID& casterGuid, const WOWGUID& targetGuid, const SpellRec* spell) {
    if (spell && !(spell->m_attributes & 0x180) && *spell->m_name) {
        QueueCombatLogEntry(GetCombatLogEntry(casterGuid, targetGuid, 14, spell), 0);
    }
}

// OFFSET: 0x005F86A0/0x14027CCD0
void UnitCombatLogSummon(const WOWGUID& casterGuid, const WOWGUID& targetGuid, const SpellRec* spell) {
    if (!spell || (spell->m_attributes & 0x180) || !*spell->m_name || (spell->m_attributesExJ & 0x40)) {
        return;
    }

    uint32_t high = targetGuid.m_guid >> 32;
    int32_t event = ((high & 0xF0F00000) == 0xF1100000 || ((high & 0xF0000000) == 0x10000000 && (high & 0xFF00000) == 0xFC00000)) ? 16 : 15;
    QueueCombatLogEntry(GetCombatLogEntry(casterGuid, targetGuid, event, spell), 0);
}

// OFFSET: 0x005F8740
void UnitCombatLogResurrect(const WOWGUID& casterGuid, const WOWGUID& targetGuid, const SpellRec* spell) {
    if (spell && !(spell->m_attributes & 0x180) && *spell->m_name) {
        QueueCombatLogEntry(GetCombatLogEntry(casterGuid, targetGuid, 46, spell), 0);
    }
}

// OFFSET: 0x005F8780/0x14027CDF0
void UnitCombatLogInterrupt(const WOWGUID& casterGuid, const WOWGUID& targetGuid, const SpellRec* spell, const SpellRec* interrupted) {
    if (!spell || (spell->m_attributes & 0x180) || !*spell->m_name || !interrupted || (interrupted->m_attributes & 0x180) || !*interrupted->m_name) {
        return;
    }

    CombatLogEntry* entry = GetCombatLogEntry(casterGuid, targetGuid, 17, spell);
    entry->m_infoFlags |= 0x100;
    entry->m_info.m_values[0] = interrupted->m_id;
    QueueCombatLogEntry(entry, 0);

    if (IsCombatTextActiveUnit(targetGuid)) {
        SendToCombatText(0, interrupted->m_name);
    }

    SpellMechanicRec* mechanic = g_spellMechanicDB.GetRecord(26);

    if (mechanic) {
        DisplayMechanicEffect(casterGuid, targetGuid, spell, mechanic);
    }
}

// OFFSET: 0x005F8830/0x14027CF40
void UnitCombatLogExtraAttacks(const WOWGUID& casterGuid, const WOWGUID& targetGuid, const SpellRec* spell, int32_t count, int32_t timeOffset) {
    if (!spell || (spell->m_attributes & 0x180) || !*spell->m_name || !count) {
        return;
    }

    CombatLogEntry* entry = GetCombatLogEntry(casterGuid, targetGuid, 18, spell);
    entry->m_infoFlags |= 0x1;
    entry->m_info.m_values[0] = count;
    QueueCombatLogEntry(entry, timeOffset);

    if (s_combatTextActiveUnit == targetGuid) {
        SendToCombatText(41, count);
    }
}

// OFFSET: 0x005F88A0/0x14027CFE0
void UnitCombatLogDurabilityDamage(const WOWGUID& casterGuid, const WOWGUID& targetGuid, const SpellRec* spell, int32_t itemId) {
    if (!spell || (spell->m_attributes & 0x180) || !*spell->m_name) {
        return;
    }

    CombatLogEntry* entry = GetCombatLogEntry(casterGuid, targetGuid, 19, spell);
    entry->SetExecuteItem(itemId);
    QueueCombatLogEntry(entry, 0);
}

// OFFSET: 0x005F88F0
void UnitCombatLogDurabilityDamageAll(const WOWGUID& casterGuid, const WOWGUID& targetGuid, const SpellRec* spell) {
    if (spell && !(spell->m_attributes & 0x180) && *spell->m_name) {
        QueueCombatLogEntry(GetCombatLogEntry(casterGuid, targetGuid, 20, spell), 0);
    }
}

// OFFSET: 0x005F8930/0x14027D0A0
void UnitCombatLogAuraAddedOrRemoved(CGUnit_C* unit, AuraData* aura, const SpellRec* spell, bool added) {
    CGUnit_C* caster = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(aura->m_creator, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (caster && (caster->m_unit->m_unitFieldFlags2 & 0x2000000) && unit && !(unit->m_unit->m_unitFieldFlags & 0x8)) {
        return;
    }

    if (unit->m_unit->m_unitFieldHealth <= 0 && !added) {
        return;
    }

    if (!spell || !spell->m_id || (spell->m_attributes & 0x180) || !*spell->m_name || ((spell->m_attributes & 0x40) && !(spell->m_attributesExI & 0x20000000)) || (spell->m_attributesExF & 0x400)) {
        return;
    }

    WOWGUID unitGuid = unit->m_obj->m_guid;
    bool harmful = (aura->m_flags & 0x80) != 0;
    CombatLogEntry* entry = GetCombatLogEntry(aura->m_creator, unitGuid, added ? 21 : 24, spell);
    entry->m_infoFlags |= 0x400;
    entry->m_info.m_bytes[4] = harmful;

    if ((spell->m_attributesExH & 0x1000) && (SpellHasAuraEffect(spell, 69) || SpellHasAuraEffect(spell, 301))) {
        entry->SetAuraValues(aura->m_effectAmounts);
    }

    QueueCombatLogEntry(entry, 0);

    if (added) {
        WOWGUID guid = unit->m_obj->m_guid;
        UnitCombatLogSpellMechanic(guid, aura, spell, aura->m_flags);
    }

    if (!IsCombatTextActiveUnit(unit->m_obj->m_guid)) {
        return;
    }

    if (added) {
        if (harmful) {
            SendToCombatText(34, spell->m_name);
        } else if (!(spell->m_attributesExF & 0x40)) {
            SendToCombatText(33, spell->m_name);
        }
    } else if (harmful) {
        SendToCombatText(32, spell->m_name);
    } else {
        SendToCombatText(31, spell->m_name);
    }
}

// OFFSET: 0x005F8B20
void UnitCombatLogAuraModifiedApplications(CGUnit_C* unit, WOWGUID& caster, const SpellRec* spell, int32_t oldStack, int32_t newStack, bool harmful) {
    if (!spell || (spell->m_attributes & 0x180) || !*spell->m_name || (spell->m_attributesExF & 0x400) || !spell->m_spellAuraOptionsID) {
        return;
    }

    SpellAuraOptionsRec* options = g_spellAuraOptionsDB.GetRecord(spell->m_spellAuraOptionsID);

    if (options && options->m_cumulativeAura <= 1) {
        return;
    }

    CGUnit_C* casterUnit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(caster, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (casterUnit && (casterUnit->m_unit->m_unitFieldFlags2 & 0x2000000) && !(unit->m_unit->m_unitFieldFlags & 0x8)) {
        return;
    }

    WOWGUID unitGuid = unit->m_obj->m_guid;
    CombatLogEntry* entry = GetCombatLogEntry(caster, unitGuid, (newStack <= oldStack) + 22, spell);
    entry->m_infoFlags |= 0xC00;
    entry->m_info.m_bytes[4] = harmful;
    entry->m_info.m_values[2] = newStack;
    QueueCombatLogEntry(entry, 0);
}

// OFFSET: 0x005F8C20
void UnitCombatLogDispelled(const WOWGUID& casterGuid, const WOWGUID& targetGuid, const SpellRec* spell, const SpellRec* dispelled, bool harmful, bool stolen, int32_t timeOffset) {
    if (!spell || (spell->m_attributes & 0x180) || !*spell->m_name || !dispelled || (dispelled->m_attributes & 0x180) || !*dispelled->m_name) {
        return;
    }

    CombatLogEntry* entry = GetCombatLogEntry(casterGuid, targetGuid, stolen + 26, spell);
    entry->m_infoFlags |= 0x500;
    entry->m_info.m_values[0] = dispelled->m_id;
    entry->m_info.m_bytes[4] = harmful;
    QueueCombatLogEntry(entry, timeOffset);

    if (IsCombatTextActiveUnit(targetGuid)) {
        SendToCombatText(40, dispelled->m_name);
    }
}

// OFFSET: 0x005F8CB0
void UnitCombatLogAuraBroken(const WOWGUID& casterGuid, const WOWGUID& targetGuid, const SpellRec* spell, const SpellRec* breaker, bool harmful) {
    if (!spell || (spell->m_attributes & 0x180) || !*spell->m_name || !breaker || (breaker->m_attributes & 0x180) || !*breaker->m_name) {
        return;
    }

    WOWGUID source = (spell->m_attributesExD & 0x2000) ? WOWGUID() : casterGuid;
    CombatLogEntry* entry = GetCombatLogEntry(source, targetGuid, 29, breaker);

    if (spell->m_id) {
        entry->m_infoFlags |= 0x100;
        entry->m_info.m_values[0] = spell->m_id;
    }

    entry->m_infoFlags |= 0x400;
    entry->m_info.m_bytes[4] = harmful;
    QueueCombatLogEntry(entry, 0);

    if (IsCombatTextActiveUnit(targetGuid)) {
        SendToCombatText(40, breaker->m_name);
    }
}

// OFFSET: 0x005F8D80/0x14027D660
void UnitCombatLogAuraRefreshed(CGUnit_C* unit, AuraData* aura, const SpellRec* spell) {
    if (!spell || (spell->m_attributes & 0x180) || !*spell->m_name || ((spell->m_attributes & 0x40) && !(spell->m_attributesExI & 0x20000000)) || (spell->m_attributesExF & 0x400) || (spell->m_attributesExI & 0x8)) {
        return;
    }

    CGUnit_C* caster = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(aura->m_creator, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (caster && (caster->m_unit->m_unitFieldFlags2 & 0x2000000) && (!unit || !(unit->m_unit->m_unitFieldFlags & 0x8))) {
        return;
    }

    WOWGUID unitGuid = unit->m_obj->m_guid;
    CombatLogEntry* entry = GetCombatLogEntry(aura->m_creator, unitGuid, 25, spell);
    entry->m_infoFlags |= 0x400;
    entry->m_info.m_bytes[4] = (aura->m_flags & 0x80) != 0;

    if ((spell->m_attributesExH & 0x1000) && (SpellHasAuraEffect(spell, 69) || SpellHasAuraEffect(spell, 301))) {
        entry->SetAuraValues(aura->m_effectAmounts);
    }

    QueueCombatLogEntry(entry, 0);
    WOWGUID guid = unit->m_obj->m_guid;
    UnitCombatLogSpellMechanic(guid, aura, spell, aura->m_flags);
}

// OFFSET: 0x005F8ED0/0x14027D800
void UnitCombatLogDamageShield(const WOWGUID& casterGuid, const WOWGUID& targetGuid, const SpellRec* spell, int32_t damage, int32_t overkill, int32_t school, int32_t absorbed) {
    if (!spell || (spell->m_attributes & 0x180) || !*spell->m_name) {
        return;
    }

    CGUnit_C* target = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(targetGuid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (target) {
        target->ModifyPredictedHealth(-damage);
    }

    CombatLogEntry* entry = GetCombatLogEntry(casterGuid, targetGuid, 40, spell);
    entry->m_infoFlags |= 0x10;
    entry->m_critical = 0;
    entry->m_glancing = 0;
    entry->m_crushing = 0;
    entry->m_isOffHand = 0;
    entry->m_info.m_values[3] = school;
    entry->m_info.m_values[1] = damage;
    entry->m_info.m_values[2] = overkill;
    entry->m_info.m_values[5] = 0;
    entry->m_info.m_values[6] = 0;
    entry->m_info.m_values[4] = absorbed;
    QueueCombatLogEntry(entry, 0);

    if (s_combatTextActiveUnit == targetGuid) {
        if (absorbed) {
            SendToCombatText(12, damage, absorbed);
        } else {
            SendToCombatText(39, damage);
        }
    }
}

// OFFSET: 0x005F8FA0/0x14027D940
void UnitCombatLogSplitDamage(const WOWGUID& source, const WOWGUID& dest, const SpellRec* spell, int32_t damage, int32_t overkill, int32_t school, int32_t absorbed, int32_t resisted, int32_t blocked, int32_t hitInfo, int32_t timeOffset) {
    if (!spell || (spell->m_attributes & 0x180) || !*spell->m_name) {
        return;
    }

    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(dest, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (unit) {
        unit->ModifyPredictedHealth(-damage);
    }

    CombatLogEntry* entry = GetCombatLogEntry(source, dest, 42, spell);
    entry->m_infoFlags |= 0x10;
    entry->m_info.m_values[2] = overkill;
    entry->m_info.m_values[3] = school;
    entry->m_info.m_values[6] = blocked;
    entry->m_info.m_values[4] = absorbed;
    entry->m_critical = (hitInfo & 0x2) != 0;
    entry->m_glancing = 0;
    entry->m_crushing = 0;
    entry->m_isOffHand = 0;
    entry->m_info.m_values[1] = damage;
    entry->m_info.m_values[5] = resisted;
    QueueCombatLogEntry(entry, timeOffset);

    if (s_combatTextActiveUnit == dest) {
        if (resisted > 0) {
            FrameScript_SignalEvent(SCRIPT_EVENT_COMBAT_TEXT_UPDATE, "%s%d%d", s_combatTextTypes[10], damage, resisted);
        } else if (blocked) {
            SendToCombatText(11, damage, blocked);
        } else if (absorbed) {
            SendToCombatText(12, damage, absorbed);
        } else if (hitInfo & 0x2) {
            SendToCombatText(1, damage);
        } else {
            SendToCombatText(2, damage);
        }
    }

    if (s_combatTextActiveUnit == dest) {
        FrameScript_SignalEvent(SCRIPT_EVENT_COMBAT_TEXT_UPDATE, "%s%d", s_combatTextTypes[42], damage);
    }
}

// OFFSET: 0x005F9100/0x14027DB60
void UnitCombatLogUnitDead(CGUnit_C* unit) {
    if (unit->m_unit->m_unitFieldFlags2 & 0x2000000) {
        return;
    }

    int32_t event = 44;
    CreatureStats_C* stats = g_creatureDBCache.GetRecord(unit->m_obj->m_entryID, WOWGUID(), NULL, NULL, false);

    if (stats && stats->m_type == 13) {
        event = 49;
    } else {
        const SpellRec* spell = g_spellDB.GetRecord(unit->m_unit->m_unitCreatedBySpell);

        if (spell) {
            const SpellEffectRec* effect = SpellGetEffect(spell->m_id, 0);

            if (effect && effect->m_effect == 28) {
                SummonPropertiesRec* properties = g_summonPropertiesDB.GetRecord(effect->m_effectMiscValueB);

                if (properties && static_cast<uint32_t>(properties->m_slot - 1) <= 3) {
                    event = 45;
                }
            }
        }
    }

    WOWGUID guid = unit->m_obj->m_guid;
    QueueCombatLogEntry(GetCombatLogEntry(WOWGUID(), guid, event, NULL), 0);
}

// OFFSET: 0x005F91F0
void UnitCombatLogBuildingDamage(const WOWGUID& caster, const WOWGUID& owner, const WOWGUID& building, int32_t damage, const SpellRec* spell) {
    if (!spell || (spell->m_attributes & 0x180) || !*spell->m_name) {
        return;
    }

    CombatLogEntry* entry = GetCombatLogEntry(caster, building, 47, spell);
    int32_t school = spell->m_schoolMask;
    entry->m_infoFlags |= 0x10;
    entry->m_critical = 0;
    entry->m_glancing = 0;
    entry->m_crushing = 0;
    entry->m_isOffHand = 0;
    entry->m_info.m_values[1] = damage;
    entry->m_info.m_values[2] = 0;
    entry->m_info.m_values[3] = school;
    entry->m_info.m_values[5] = 0;
    entry->m_info.m_values[6] = 0;
    entry->m_info.m_values[4] = 0;
    QueueCombatLogEntry(entry, 0);

    if (owner == ClntObjMgrGetActivePlayer()) {
        CGGameObject_C* gameObject = static_cast<CGGameObject_C*>(ClntObjMgrObjectPtr(building, TYPEMASK_GAMEOBJECT, __FILE__, __LINE__));

        if (gameObject) {
            gameObject->AddWorldDamageText(damage);
        }
    }
}

// OFFSET: 0x005F92A0
void UnitCombatLogBuildingHeal(const WOWGUID& caster, const WOWGUID& owner, const WOWGUID& building, int32_t amount, const SpellRec* spell) {
    if (!spell || (spell->m_attributes & 0x180) || !*spell->m_name) {
        return;
    }

    CombatLogEntry* entry = GetCombatLogEntry(caster, building, 48, spell);
    entry->m_infoFlags |= 0x20;
    entry->m_info.m_values[0] = amount;
    entry->m_info.m_values[1] = 0;
    entry->m_info.m_values[2] = 0;
    entry->m_info.m_bytes[12] = 0;
    QueueCombatLogEntry(entry, 0);

    if (owner == ClntObjMgrGetActivePlayer()) {
        CGGameObject_C* gameObject = static_cast<CGGameObject_C*>(ClntObjMgrObjectPtr(building, TYPEMASK_GAMEOBJECT, __FILE__, __LINE__));

        if (gameObject) {
            gameObject->AddWorldHealingText(amount, spell);
        }
    }
}

// OFFSET: 0x00558AA0/0x1401B5E00
int32_t CombatLog::HandleSpellLogMiss(CDataStore* msg, int32_t timeOffset) {
    uint32_t spellId = 0;
    msg->Get(spellId);
    uint64_t value = 0;
    msg->Get(value);
    WOWGUID caster = WOWGUID::Make(value);
    uint8_t debug = 0;
    msg->Get(debug);
    bool isDebug = debug != 0;
    uint32_t count = 0;
    msg->Get(count);
    const SpellRec* spell = g_spellDB.GetRecord(spellId);

    for (; count; count--) {
        value = 0;
        msg->Get(value);
        WOWGUID target = WOWGUID::Make(value);
        uint8_t missType = 0;
        msg->Get(missType);

        if (isDebug) {
            float roll = 0.0f;
            msg->Get(roll);
            float chance = 0.0f;
            msg->Get(chance);
        } else if (spell && !(spell->m_attributes & 0x100)) {
            CGGameUI::ShowSpellMissFeedback(target, missType);
            UnitCombatLogSpellMissed(caster, target, spell, missType, true, false, timeOffset);
            CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(target, TYPEMASK_UNIT, __FILE__, __LINE__));

            if (unit) {
                unit->AddWorldText(caster, missType, spell, false, 0);
            }
        }
    }

    return 1;
}

// OFFSET: 0x00558C30
int32_t CombatLog::HandleSpellLogExecute(CDataStore* msg, int32_t timeOffset) {
    SmartGUID caster;
    *msg >> caster;
    uint32_t spellId = 0;
    msg->Get(spellId);
    int32_t effectCount = 0;
    msg->Get(reinterpret_cast<uint32_t&>(effectCount));
    const SpellRec* spell = g_spellDB.GetRecord(spellId);

    if (!spell) {
        msg->m_read = msg->m_size;
        return 1;
    }

    for (int32_t i = effectCount; i > 0; i--) {
        uint32_t effect = 0;
        msg->Get(effect);
        int32_t targetCount = 0;
        msg->Get(reinterpret_cast<uint32_t&>(targetCount));

        for (int32_t j = targetCount; j > 0; j--) {
            switch (effect) {
                case 8:
                case 62: {
                    SmartGUID target;
                    *msg >> target;
                    uint32_t amount = 0;
                    msg->Get(amount);
                    uint32_t powerType = 0;
                    msg->Get(powerType);
                    float multiplier = 0.0f;
                    msg->Get(multiplier);
                    UnitCombatLogPowerDrain(caster, target, spell, powerType, amount, multiplier, (spell->m_attributesExC & 0x2000000) != 0, timeOffset, effect == 8);
                    break;
                }

                case 18:
                case 113:
                case 172: {
                    SmartGUID target;
                    *msg >> target;
                    UnitCombatLogResurrect(caster, target, spell);
                    break;
                }

                case 19: {
                    SmartGUID target;
                    *msg >> target;
                    uint32_t attacks = 0;
                    msg->Get(attacks);
                    UnitCombatLogExtraAttacks(caster, target, spell, attacks, timeOffset);
                    break;
                }

                case 24:
                case 59:
                case 157: {
                    uint32_t itemId = 0;
                    msg->Get(itemId);
                    CGChat::LogTradeSkillCreate(caster, itemId);
                    break;
                }

                case 28:
                case 50:
                case 56:
                case 76:
                case 81:
                case 83:
                case 104:
                case 171: {
                    SmartGUID target;
                    *msg >> target;
                    UnitCombatLogSummon(caster, target, spell);
                    break;
                }

                case 33: {
                    SmartGUID target;
                    *msg >> target;
                    CGChat::LogOpenLock(caster, target, spell);
                    break;
                }

                case 101: {
                    uint32_t itemId = 0;
                    msg->Get(itemId);
                    CGChat::LogFeedPet(caster, itemId);
                    break;
                }

                case 102: {
                    SmartGUID target;
                    *msg >> target;
                    CGChat::LogDismissPet(caster, target);
                    break;
                }

                case 111: {
                    SmartGUID target;
                    *msg >> target;
                    int32_t itemId = 0;
                    msg->Get(reinterpret_cast<uint32_t&>(itemId));
                    int32_t slot = 0;
                    msg->Get(reinterpret_cast<uint32_t&>(slot));

                    if (itemId == -1 && slot == -1) {
                        UnitCombatLogDurabilityDamageAll(caster, target, spell);
                    } else {
                        UnitCombatLogDurabilityDamage(caster, target, spell, itemId);
                    }

                    break;
                }
            }
        }
    }

    return 1;
}

// OFFSET: 0x00559290/0x1401B67C0
int32_t CombatLog::HandleSpellNonMeleeDamageLog(CDataStore* msg, int32_t timeOffset) {
    SmartGUID target;
    *msg >> target;
    SmartGUID caster;
    *msg >> caster;
    uint32_t spellId = 0;
    msg->Get(spellId);
    int32_t damage = 0;
    msg->Get(reinterpret_cast<uint32_t&>(damage));
    int32_t overkill = 0;
    msg->Get(reinterpret_cast<uint32_t&>(overkill));
    uint8_t school = 0;
    msg->Get(school);
    int32_t absorbed = 0;
    msg->Get(reinterpret_cast<uint32_t&>(absorbed));
    int32_t resisted = 0;
    msg->Get(reinterpret_cast<uint32_t&>(resisted));
    uint8_t periodic = 0;
    msg->Get(periodic);
    uint8_t unused = 0;
    msg->Get(unused);
    int32_t blocked = 0;
    msg->Get(reinterpret_cast<uint32_t&>(blocked));
    uint32_t hitInfo = 0;
    msg->Get(hitInfo);
    uint8_t debug = 0;
    msg->Get(debug);
    bool critical = (hitInfo & 0x2) != 0;
    uint32_t flags = (hitInfo & 0x2) ? 0x200 : 0;

    if (absorbed) {
        flags |= 0x20;
    }

    if (blocked) {
        flags |= 0x2000;
    }

    if (resisted) {
        flags |= 0x80;
    }

    const SpellRec* spell = g_spellDB.GetRecord(spellId);

    if (hitInfo & 0x8) {
        UnitCombatLogSplitDamage(caster, target, spell, damage, overkill, school, absorbed, resisted, blocked, hitInfo, timeOffset);

        if (spell && !(spell->m_attributes & 0x100)) {
            CGGameUI::ShowCombatFeedback(target, damage, school, flags);
        }

        return 1;
    }

    WOWGUID source = (hitInfo & 0x10) ? target : caster;
    UnitCombatLogSpellDamage(source, target, spell, damage, overkill, school, absorbed, resisted, blocked, hitInfo, periodic != 0, timeOffset);

    if (spell && !(spell->m_attributes & 0x100)) {
        CGGameUI::ShowCombatFeedback(target, damage, school, flags);
        CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(target, TYPEMASK_UNIT, __FILE__, __LINE__));

        if (unit) {
            if (damage) {
                unit->AddWorldDamageText(caster, damage, spell, critical, false, false);
            } else if (absorbed) {
                unit->AddWorldText(caster, 10, spell, false, 0);
            } else if (blocked) {
                unit->AddWorldText(caster, 5, spell, false, 0);
            } else if (resisted) {
                unit->AddWorldText(caster, 2, spell, false, 0);
            }
        }
    }

    if (debug) {
        float value = 0.0f;

        if (hitInfo & 0x1) {
            msg->Get(value);
            msg->Get(value);
        }

        if (hitInfo & 0x4) {
            msg->Get(value);
            msg->Get(value);
        }

        if (hitInfo & 0x20) {
            for (uint32_t i = 0; i < 6; i++) {
                msg->Get(value);
            }
        }
    }

    return 1;
}

// OFFSET: 0x00559670/0x1401B6CF0
int32_t CombatLog::HandleSpellHealLog(CDataStore* msg, int32_t timeOffset) {
    SmartGUID target;
    *msg >> target;
    SmartGUID caster;
    *msg >> caster;
    uint32_t spellId = 0;
    msg->Get(spellId);
    int32_t amount = 0;
    msg->Get(reinterpret_cast<uint32_t&>(amount));
    int32_t overheal = 0;
    msg->Get(reinterpret_cast<uint32_t&>(overheal));
    int32_t absorbed = 0;
    msg->Get(reinterpret_cast<uint32_t&>(absorbed));
    uint8_t value = 0;
    msg->Get(value);
    bool critical = value != 0;
    value = 0;
    msg->Get(value);
    const SpellRec* spell = g_spellDB.GetRecord(spellId);

    if (!spell) {
        return 1;
    }

    if (!(spell->m_attributes & 0x100) && (!(spell->m_attributesExJ & 0x8) || overheal != amount)) {
        CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(target, TYPEMASK_UNIT, __FILE__, __LINE__));

        if (unit && !amount && absorbed) {
            unit->AddWorldText(caster, 10, spell, true, 1);
        }

        CGGameUI::ShowHealingFeedback(target, amount, critical);
    }

    UnitCombatLogHeal(caster, target, spell, amount, overheal, absorbed, critical, (spell->m_attributesExC & 0x2000000) != 0, timeOffset);
    return 1;
}

// OFFSET: 0x00559810
int32_t CombatLog::HandleSpellLogEnergizeLog(CDataStore* msg, int32_t timeOffset) {
    SmartGUID target;
    *msg >> target;
    SmartGUID caster;
    *msg >> caster;
    uint32_t spellId = 0;
    msg->Get(spellId);
    uint32_t powerType = 0;
    msg->Get(powerType);
    int32_t amount = 0;
    msg->Get(reinterpret_cast<uint32_t&>(amount));
    const SpellRec* spell = g_spellDB.GetRecord(spellId);

    if (!spell) {
        return 1;
    }

    if (!(spell->m_attributes & 0x100)) {
        CGGameUI::ShowEnergizeFeedback(target, powerType, amount);
    }

    UnitCombatLogEnergize(caster, target, spell, powerType, amount, (spell->m_attributesExC & 0x2000000) != 0, timeOffset);
    return 1;
}

// OFFSET: 0x00559920
int32_t CombatLog::HandleSpellRemoveLog(NETMESSAGE msgId, CDataStore* msg, int32_t timeOffset) {
    uint32_t spellId = 0;
    uint8_t debug = 0;
    SmartGUID target;
    *msg >> target;
    SmartGUID caster;
    *msg >> caster;
    msg->Get(spellId);
    msg->Get(debug);
    uint32_t count = 0;
    msg->Get(count);

    while (count) {
        count--;
        uint32_t removedId;
        msg->Get(removedId);
        uint8_t harmful;
        msg->Get(harmful);
        const SpellRec* spell = g_spellDB.GetRecord(spellId);
        const SpellRec* removed = g_spellDB.GetRecord(removedId);

        if (msgId == SMSG_SPELL_BREAK_LOG) {
            UnitCombatLogAuraBroken(caster, target, spell, removed, harmful != 0);
        } else {
            UnitCombatLogDispelled(caster, target, spell, removed, harmful != 0, msgId == SMSG_SPELL_STEAL_LOG, timeOffset);
        }
    }

    return 1;
}

// OFFSET: 0x005F9340
int32_t CombatLog::HandlePeriodicAuraLog(CDataStore* msg, int32_t timeOffset) {
    SmartGUID target;
    *msg >> target;
    SmartGUID caster;
    *msg >> caster;
    uint32_t spellId = 0;
    msg->Get(spellId);
    int32_t count = 0;
    msg->Get(reinterpret_cast<uint32_t&>(count));
    const SpellRec* spell = g_spellDB.GetRecord(spellId);
    CGUnit_C* unit;

    if (!spell || (unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(target, TYPEMASK_UNIT, __FILE__, __LINE__))) == NULL) {
        msg->m_read = msg->m_size;
        return 1;
    }

    for (int32_t i = 0; i < count; i++) {
        uint32_t auraType = 0;
        msg->Get(auraType);

        switch (auraType) {
            case 3:
            case 89: {
                int32_t damage = 0;
                msg->Get(reinterpret_cast<uint32_t&>(damage));
                int32_t overkill = 0;
                msg->Get(reinterpret_cast<uint32_t&>(overkill));
                uint32_t school = 0;
                msg->Get(school);
                int32_t absorbed = 0;
                msg->Get(reinterpret_cast<uint32_t&>(absorbed));
                int32_t resisted = 0;
                msg->Get(reinterpret_cast<uint32_t&>(resisted));
                uint8_t critical = 0;
                msg->Get(critical);
                int32_t hitInfo = 0;

                if (!(spell->m_attributes & 0x100)) {
                    if (damage) {
                        unit->AddWorldDamageText(caster, damage, spell, critical != 0, true, false);
                    } else if (absorbed) {
                        unit->AddWorldText(caster, 10, spell, true, 0);
                    } else if (resisted) {
                        unit->AddWorldText(caster, 2, spell, true, 0);
                    }

                    uint32_t flags = absorbed ? 0x20 : 0;

                    if (resisted) {
                        flags |= 0x80;
                    }

                    if (critical) {
                        flags |= 0x200;
                        hitInfo = 2;
                    }

                    CGGameUI::ShowCombatFeedback(target, damage, school, flags);
                }

                UnitCombatLogSpellDamage(caster, target, spell, damage, overkill, school, absorbed, resisted, 0, hitInfo, true, timeOffset);
                break;
            }

            case 8:
            case 20: {
                int32_t amount = 0;
                msg->Get(reinterpret_cast<uint32_t&>(amount));
                int32_t overheal = 0;
                msg->Get(reinterpret_cast<uint32_t&>(overheal));
                int32_t absorbed = 0;
                msg->Get(reinterpret_cast<uint32_t&>(absorbed));
                uint8_t critical = 0;
                msg->Get(critical);

                if (!(spell->m_attributes & 0x100) && (!(spell->m_attributesExJ & 0x8) || overheal != amount)) {
                    if (!amount && absorbed) {
                        unit->AddWorldText(caster, 10, spell, true, 1);
                    }

                    CGGameUI::ShowHealingFeedback(target, amount, critical != 0);
                }

                UnitCombatLogHeal(caster, target, spell, amount, overheal, absorbed, critical != 0, true, timeOffset);

                if (spell->m_attributesExH & 0x1000000) {
                    Script_SendUnitSignal(unit->m_obj->m_guid, SCRIPT_EVENT_UNIT_HEAL_PREDICTION);
                }

                break;
            }

            case 21:
            case 24: {
                uint32_t powerType = 0;
                msg->Get(powerType);
                int32_t amount = 0;
                msg->Get(reinterpret_cast<uint32_t&>(amount));

                if (!(spell->m_attributes & 0x100)) {
                    CGGameUI::ShowEnergizeFeedback(target, powerType, amount);
                }

                UnitCombatLogEnergize(caster, target, spell, powerType, amount, true, timeOffset);
                break;
            }

            case 64: {
                uint32_t powerType = 0;
                msg->Get(powerType);
                int32_t amount = 0;
                msg->Get(reinterpret_cast<uint32_t&>(amount));
                float multiplier = 0.0f;
                msg->Get(multiplier);

                if (amount) {
                    CGUnit_C* drained = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(target, TYPEMASK_UNIT, __FILE__, __LINE__));

                    if (drained) {
                        drained->ModifyPredictedPower(powerType, -amount);
                    }

                    if (!(spell->m_attributes & 0x180) && *spell->m_name) {
                        int32_t extra = static_cast<int32_t>(static_cast<float>(amount) * multiplier);
                        AddPowerDrainEntry(target, (extra != 0) + 37, caster, spell, amount, powerType, extra, timeOffset);
                    }
                }

                break;
            }

            default:
                msg->m_read = msg->m_size;
                return 1;
        }
    }

    return 1;
}

// OFFSET: 0x005F98C0
static int32_t Packet_SMSG_SPELL_PERIODIC_AURA_LOG(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    return CombatLog::HandlePeriodicAuraLog(msg, 0);
}

// OFFSET: 0x005F98E0/0x14027E5D0
static int32_t Packet_SMSG_DESTRUCTIBLE_BUILDING_DAMAGE(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    SmartGUID building;
    SmartGUID caster;
    SmartGUID owner;
    *msg >> building;
    *msg >> caster;
    *msg >> owner;
    int32_t damage;
    msg->Get(reinterpret_cast<uint32_t&>(damage));
    uint32_t spellId;
    msg->Get(spellId);
    const SpellRec* spell = g_spellDB.GetRecord(spellId);

    if (damage <= 0) {
        UnitCombatLogBuildingHeal(caster, owner, building, -damage, spell);
    } else {
        UnitCombatLogBuildingDamage(caster, owner, building, damage, spell);
    }

    return 1;
}

// OFFSET: 0x005F9A80/0x14027E910
void UnitCombatLogReset() {
    s_entries.Clear();
    s_freeEntries.Clear();
    s_pendingEntries = NULL;
    s_currentEntry = NULL;
    s_filters.Clear();
    s_pendingData.Clear();
    s_freePendingData.Clear();
    s_invalidatedNames.Clear();
    s_unknownObjectName[0] = '\0';
}

// OFFSET: 0x005F9AE0
void UnitCombatLogInitialize() {
    UnitCombatLogReset();
    s_startTime = static_cast<uint32_t>(time(NULL));
    s_startTimeMs = FrameTime::GetCurTimeMs();
    ClientServices::SetMessageHandler(SMSG_SPELL_PERIODIC_AURA_LOG, Packet_SMSG_SPELL_PERIODIC_AURA_LOG, NULL);
    ClientServices::SetMessageHandler(SMSG_ENCHANTMENT_LOG, Packet_SMSG_ENCHANTMENT_LOG, NULL);
    ClientServices::SetMessageHandler(SMSG_PARTY_KILL_LOG, Packet_SMSG_PARTY_KILL_LOG, NULL);
    ClientServices::SetMessageHandler(SMSG_PROC_RESIST, Packet_SMSG_PROC_RESIST, NULL);
    ClientServices::SetMessageHandler(SMSG_DISPEL_FAILED, Packet_SMSG_DISPEL_FAILED, NULL);
    s_combatTextActiveUnit = WOWGUID();
    ClientServices::SetMessageHandler(SMSG_DESTRUCTIBLE_BUILDING_DAMAGE, Packet_SMSG_DESTRUCTIBLE_BUILDING_DAMAGE, NULL);
}

// OFFSET: 0x005F9BA0/0x14027EA60
void UnitCombatLogShutdown() {
    UnitCombatLogReset();
    ClientServices::ClearMessageHandler(SMSG_SPELL_PERIODIC_AURA_LOG);
    ClientServices::ClearMessageHandler(SMSG_ENCHANTMENT_LOG);
    ClientServices::ClearMessageHandler(SMSG_PARTY_KILL_LOG);
    ClientServices::ClearMessageHandler(SMSG_PROC_RESIST);
    ClientServices::ClearMessageHandler(SMSG_DISPEL_FAILED);
    ClientServices::ClearMessageHandler(SMSG_DESTRUCTIBLE_BUILDING_DAMAGE);
}

// OFFSET: 0x005FF760/0x140283870
bool ShouldShowFeedback(const ATTACKROUNDINFO& info) {
    return !info.m_meleeSpellId || info.m_victimState == 1;
}

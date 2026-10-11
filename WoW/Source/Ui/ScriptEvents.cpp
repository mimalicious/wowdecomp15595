#include <cstdint>
#include "Engine/Source/Storm/H/stpl.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "WoW/Source/Ui/ScriptEvents.h"
#include "WoW/Source/Object/ObjectClient/Unit_C.h"
#include "WoW/Source/Object/Types.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/Ui/BattlefieldInfo.h"
#include "WoW/Source/Ui/GameUI.h"
#include "WoW/Source/Ui/PartyFrame.h"
#include "WoW/Source/Ui/RaidInfo.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/lua-5.1/src/lua.h"
#include "WoW/Source/DB/DBClient/DBCacheInstances.h"
#include "WoW/Source/Game/GameClient/NameCache.h"
#include "WoW/Source/Game/GameClient/PetNameCache.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/Ui/CommentatorFrame.h"
#include "WoW/Source/Ui/InstanceEncounter_C.h"
#include "WoW/Source/Ui/QuestFrame.h"
#include "WoW/Source/DB/Rec/SpellRec.h"
#include "WoW/Source/Ui/SpellBookFrame.h"
#include "WoW/Source/DB/Rec/SpellDispelTypeRec.h"
#include "WoW/Source/DB/Rec/SpellIconRec.h"
#include "WoW/Source/Object/ObjectClient/AuraData.h"
#include "WoW/Source/DB/DBClient/DBClient.h"

// STATIC: 0x00D39228
const char* g_scriptEvents[NUM_SCRIPT_EVENTS];

// STATIC: 0x00D39F38
static TSGrowableArray<UniqueSignal> s_uniqueSignals;

// STATIC: #0x00C25A28
static char s_nameWithRealm[305];

// STATIC: #0x00C25BA0
static char s_tokenBuffers[8][32];

// STATIC: 0x00C99BD8
static char* s_tokenIDs[8] = { s_tokenBuffers[0], s_tokenBuffers[1], s_tokenBuffers[2], s_tokenBuffers[3], s_tokenBuffers[4], s_tokenBuffers[5], s_tokenBuffers[6], s_tokenBuffers[7] };

// STATIC: 0x00D3A0D4
static char s_auraToken[32];

// STATIC: 0x00D3A0B4
static char s_tokenFromGUID[32];

// OFFSET: 0x0044C420/0x140059B00
void ScriptEventsInitialize() {
#define SCRIPT_EVENT_NAME(name, index) g_scriptEvents[SCRIPT_EVENT_##name] = #name;
#define SCRIPT_EVENT_ALIAS_NAME(name, string, index) g_scriptEvents[SCRIPT_EVENT_##name] = string;
    SCRIPT_EVENTS(SCRIPT_EVENT_NAME, SCRIPT_EVENT_ALIAS_NAME)
#undef SCRIPT_EVENT_ALIAS_NAME
#undef SCRIPT_EVENT_NAME
}

// OFFSET: #0x006081B0
static const WOWGUID* GetCurrentQuestGiverGUID() {
    if (CGQuestInfo::m_npcInform != WOWGUID()) {
        return &CGQuestInfo::m_npcInform;
    }

    return &CGQuestInfo::m_npc;
}

// OFFSET: 0x0044F2E0/0x14005D740
static bool ParseTrailingTokens(const char* token, WOWGUID& guid, CGPlayer_C* player) {
    while (*token) {
        if (*token == '-') {
            token++;
        }

        if (!SStrCmpI(token, "target", 6)) {
            token += 6;

            if (guid == ClntObjMgrGetActivePlayer()) {
                guid = CGGameUI::m_lockedTarget;
            } else {
                CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));
                guid = unit ? unit->m_unit->m_unitFieldTarget : WOWGUID();
            }
        } else if (!SStrCmpI(token, "pet", 3) && (guid == ClntObjMgrGetActivePlayer() || CGGameUI::IsPartyMember(guid) || (player && (player->m_player->m_playerFlags & 0x80000)))) {
            token += 3;
            CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));

            if (unit) {
                guid = unit->m_unit->m_unitFieldCharm != WOWGUID() ? unit->m_unit->m_unitFieldCharm : unit->m_unit->m_unitFieldSummon;
            } else {
                guid = WOWGUID();
            }
        } else {
            guid = WOWGUID();
            return false;
        }
    }

    return true;
}

// OFFSET: 0x0044F430/0x14005D8A0
bool Script_GetGUIDFromToken(const char* token, WOWGUID& guid, bool defaultToTarget, bool withRealm) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!token || !*token) {
        if (!defaultToTarget) {
            return false;
        }

        guid = CGGameUI::m_lockedTarget;
        return true;
    }

    const char* name = token;
    guid = WOWGUID();

    if (!SStrCmpI(token, "player", 6)) {
        token += 6;
        guid = player ? player->m_obj->m_guid : WOWGUID();
    } else if (!SStrCmpI(token, "vehicle", 7)) {
        token += 7;
        guid = player && player->IsVehiclePassengerSeated() ? WOWGUID::Make(player->GetTransportGUID()) : WOWGUID();
    } else if (!SStrCmpI(token, "pet", 3)) {
        token += 3;

        if (player) {
            guid = player->m_unit->m_unitFieldCharm != WOWGUID() ? player->m_unit->m_unitFieldCharm : player->m_unit->m_unitFieldSummon;
        } else {
            guid = WOWGUID();
        }
    } else if (!SStrCmpI(token, "target", 6)) {
        token += 6;
        guid = CGGameUI::m_lockedTarget;
    } else if (!SStrCmpI(token, "partypet", 8)) {
        token += 8;
        guid = CGGameUI::GetPartyMemberPet(SStrToUnsigned(&token) - 1);
    } else if (!SStrCmpI(token, "party", 5)) {
        token += 5;
        guid = CGGameUI::GetPartyMember(SStrToUnsigned(&token) - 1);
    } else if (!SStrCmpI(token, "raidpet", 7)) {
        token += 7;
        guid = CGGameUI::GetRaidMemberPet(SStrToUnsigned(&token) - 1);
    } else if (!SStrCmpI(token, "raid", 4)) {
        token += 4;
        guid = GetRaidMemberGUID(SStrToUnsigned(&token) - 1);
    } else if (!SStrCmpI(token, "boss", 4)) {
        token += 4;
        guid = CGInstanceEncounter_C::GetBoss(SStrToUnsigned(&token) - 1);
    } else if (!SStrCmpI(token, "arenapet", 8)) {
        token += 8;
        guid = CGBattlefieldInfo::GetArenaOpponentPet(SStrToUnsigned(&token) - 1);
    } else if (!SStrCmpI(token, "arena", 5)) {
        token += 5;
        guid = CGBattlefieldInfo::GetArenaOpponent(SStrToUnsigned(&token) - 1);
    } else if (!SStrCmpI(token, "commentator", 11)) {
        token += 11;
        guid.m_guid = CGCommentator::s_Commentator.GetArenaMember(SStrToUnsigned(&token) - 1);
    } else if (!SStrCmpI(token, "mouseover", 9)) {
        token += 9;
        guid = CGGameUI::m_currentObjectTrack;

        if (!ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__) && !CGGameUI::IsRaidMemberOrPet(guid)) {
            guid = WOWGUID();
        }
    } else if (!SStrCmpI(token, "focus", 5)) {
        token += 5;
        guid = CGGameUI::m_focusTarget;
    } else if (!SStrCmpI(token, "npc", STORM_MAX_STR)) {
        token += 3;
        guid = CGGameUI::m_interactTarget;
    } else if (!SStrCmpI(token, "questnpc", STORM_MAX_STR)) {
        token += 8;
        guid = *GetCurrentQuestGiverGUID();
    } else if (!SStrCmpI(token, "none", STORM_MAX_STR)) {
        token += 4;
        guid.m_guid = 0xFFFFFFFFFFFFFFFFULL;
    }

    if ((guid != WOWGUID() || !*token) && ParseTrailingTokens(token, guid, player)) {
        if (guid == WOWGUID()) {
            guid.m_guid = 0xFFFFFFFFFFFFFFFEULL;
        }

        return true;
    }

    if (guid != WOWGUID() || !Script_GetGUIDFromString(&name, guid, withRealm) || !ParseTrailingTokens(name, guid, player)) {
        return false;
    }

    if (guid == WOWGUID()) {
        guid.m_guid = 0xFFFFFFFFFFFFFFFEULL;
    }

    return true;
}

// OFFSET: 0x00450AA0/0x14005EEC0
WOWGUID Script_GetGUIDFromName(const char* name) {
    WOWGUID guid = WOWGUID::Make(0);
    Script_GetGUIDFromToken(name, guid, false, false);
    return guid;
}

// OFFSET: 0x00450AD0/0x14005EEF0
CGUnit_C* Script_GetUnitFromName(const char* token) {
    WOWGUID guid;

    if (!Script_GetGUIDFromToken(token, guid, false, false)) {
        return NULL;
    }

    return static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));
}

// OFFSET: #0x006143F0
void Script_SendUniqueSignal(const WOWGUID& guid, int32_t signal) {
    for (uint32_t i = 0; i < s_uniqueSignals.Count(); i++) {
        UniqueSignal& entry = s_uniqueSignals[i];

        if (entry.m_guid == guid && entry.m_signal == signal) {
            return;
        }
    }

    UniqueSignal* entry = s_uniqueSignals.New();
    entry->m_guid = guid;
    entry->m_signal = signal;
}

// OFFSET: #0x006144D0
void Script_SendUniqueSignalToAllUnits(int32_t signal) {
    CGUnit_C* player = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player) {
        WOWGUID guid = player->m_obj->m_guid;
        Script_SendUniqueSignal(guid, signal);

        const WOWGUID& pet = player->m_unit->m_unitFieldCharm ? player->m_unit->m_unitFieldCharm : player->m_unit->m_unitFieldSummon;

        if (pet) {
            const WOWGUID& target = player->m_unit->m_unitFieldCharm ? player->m_unit->m_unitFieldCharm : player->m_unit->m_unitFieldSummon;
            Script_SendUniqueSignal(target, signal);
        }
    }

    for (uint32_t i = 0; i < CGPartyInfo::NumMembers(); i++) {
        if (CGGameUI::GetPartyMember(i)) {
            WOWGUID member = CGGameUI::GetPartyMember(i);
            Script_SendUniqueSignal(member, signal);

            if (CGGameUI::GetPartyMemberPet(i)) {
                WOWGUID pet = CGGameUI::GetPartyMemberPet(i);
                Script_SendUniqueSignal(pet, signal);
            }
        }
    }

    for (uint32_t i = 0; i < CGRaidInfo::m_numMembers; i++) {
        if (GetRaidMemberGUID(i)) {
            WOWGUID member = GetRaidMemberGUID(i);
            Script_SendUniqueSignal(member, signal);

            if (CGGameUI::GetRaidMemberPet(i)) {
                WOWGUID pet = CGGameUI::GetRaidMemberPet(i);
                Script_SendUniqueSignal(pet, signal);
            }
        }
    }

    for (uint32_t i = 0; i < GetArenaPetGUIDCount(); i++) {
        if (i >= 5) {
            continue;
        }

        if (CGBattlefieldInfo::m_arenaOpponents[i]) {
            WOWGUID opponent = CGBattlefieldInfo::m_arenaOpponents[i];
            Script_SendUniqueSignal(opponent, signal);
        }

        CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(CGBattlefieldInfo::m_arenaOpponents[i], TYPEMASK_UNIT, __FILE__, __LINE__));
        WOWGUID pet;

        if (unit) {
            const WOWGUID& unitPet = unit->m_unit->m_unitFieldCharm ? unit->m_unit->m_unitFieldCharm : unit->m_unit->m_unitFieldSummon;
            pet = unitPet;
        } else {
            pet = CGBattlefieldInfo::m_arenaOpponentPets[i][0];
        }

        if (pet) {
            unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(CGBattlefieldInfo::m_arenaOpponents[i], TYPEMASK_UNIT, __FILE__, __LINE__));
            WOWGUID petGuid;

            if (unit) {
                const WOWGUID& unitPet = unit->m_unit->m_unitFieldCharm ? unit->m_unit->m_unitFieldCharm : unit->m_unit->m_unitFieldSummon;
                petGuid = unitPet;
            } else {
                petGuid = CGBattlefieldInfo::m_arenaOpponentPets[i][0];
            }

            Script_SendUniqueSignal(petGuid, signal);
        }
    }

    if (CGGameUI::m_lockedTarget) {
        Script_SendUniqueSignal(CGGameUI::m_lockedTarget, signal);
    }

    if (CGGameUI::m_focusTarget) {
        Script_SendUniqueSignal(CGGameUI::m_focusTarget, signal);
    }

    if (CGGameUI::m_interactTarget) {
        Script_SendUniqueSignal(CGGameUI::m_interactTarget, signal);
    }
}

// PROVEN: 0db9253a IDENTICAL/NONE
// OFFSET: 0x0045AB10/0x14006B2D0
void Script_SendUniqueSignal(int32_t signal) {
    WOWGUID guid = WOWGUID();
    Script_SendUniqueSignal(guid, signal);
}

// PROVEN: def28bb6 IDENTICAL/NONE
// OFFSET: 0x004598F0/0x1400698E0
void Script_SendUniqueSignals() {
    uint32_t count = s_uniqueSignals.Count();

    for (uint32_t i = 0; i < count; i++) {
        UniqueSignal& entry = s_uniqueSignals[i];

        if (entry.m_guid) {
            Script_SendUnitSignal(entry.m_guid, entry.m_signal);
        } else {
            FrameScript_SignalEvent(entry.m_signal, NULL);
        }
    }

    s_uniqueSignals.m_count = 0;
}

// PROVEN: 9dc2d163 IDENTICAL/NONE
// OFFSET: 0x00450B20
static int32_t UnitFieldChangedHandler(WOWGUID guid, uint32_t fieldOffset, uint32_t fieldSize, const void* oldValue, void* param) {
    Script_SendUnitSignal(guid, fieldOffset >> 2);
    return 1;
}

// OFFSET: 0x00450B40
static int32_t PlayerInventoryChangedHandler(WOWGUID guid, uint32_t fieldOffset, uint32_t fieldSize, const void* oldValue, void* param) {
    Script_SendUnitSignal(guid, SCRIPT_EVENT_UNIT_INVENTORY_CHANGED);
    return 1;
}

// OFFSET: 0x00450B60
static int32_t PlayerQuestLogChangedHandler(WOWGUID guid, uint32_t fieldOffset, uint32_t fieldSize, const void* oldValue, void* param) {
    Script_SendUnitSignal(guid, SCRIPT_EVENT_UNIT_QUEST_LOG_CHANGED);
    return 1;
}

// OFFSET: 0x004592C0/0x1400692F0
void ScriptEventsRegisterUnit(CGUnit_C* unit) {
    for (uint32_t i = 0; i < sizeof(unit->m_unitMirrorLists) / sizeof(unit->m_unitMirrorLists[0]); i++) {
        if (!g_scriptEvents[i]) {
            continue;
        }

        uint32_t size;

        switch (i) {
            case 0:
            case 2:
            case 6:
            case 12:
                size = sizeof(WOWGUID);
                break;

            case 125:
            case 132:
                size = 28;
                break;

            default:
                size = sizeof(uint32_t);
                break;
        }

        ClntObjMgrSetObjMirrorHandler(unit->m_obj->m_guid, ID_UNIT, i * sizeof(uint32_t), size, UnitFieldChangedHandler, NULL, static_cast<HANDLER_PRIORITY>(0), 0);
    }

    if (unit->m_obj->m_type & TYPEMASK_PLAYER) {
        ClntObjMgrSetObjMirrorHandler(unit->m_obj->m_guid, ID_PLAYER, 0x21C, 0x98, PlayerInventoryChangedHandler, NULL, static_cast<HANDLER_PRIORITY>(0), 0);
        ClntObjMgrSetObjMirrorHandler(unit->m_obj->m_guid, ID_PLAYER, 0x28, 0x1F4, PlayerQuestLogChangedHandler, NULL, static_cast<HANDLER_PRIORITY>(0), 0);
    }
}

// OFFSET: 0x00459420/0x1400694C0
void ScriptEventsUnregisterUnit(CGUnit_C* unit) {
    for (uint32_t i = 0; i < sizeof(unit->m_unitMirrorLists) / sizeof(unit->m_unitMirrorLists[0]); i++) {
        if (g_scriptEvents[i]) {
            ClntObjMgrUnsetObjMirrorHandler(unit->m_obj->m_guid, ID_UNIT, i * sizeof(uint32_t), UnitFieldChangedHandler, NULL);
        }
    }

    if (unit->m_obj->m_type & TYPEMASK_PLAYER) {
        ClntObjMgrUnsetObjMirrorHandler(unit->m_obj->m_guid, ID_PLAYER, 0x21C, PlayerInventoryChangedHandler, NULL);
        ClntObjMgrUnsetObjMirrorHandler(unit->m_obj->m_guid, ID_PLAYER, 0x28, PlayerQuestLogChangedHandler, NULL);
    }
}

// PROVEN: 64f52559 IDENTICAL/NONE
// OFFSET: 0x0044BE10/0x140059410
void UnitNameQueryCallback(int32_t id, const WOWGUID& guid, void* param, bool success) {
    if (success) {
        CGGameUI::UnitNameUpdate(guid);
    }
}

// OFFSET: %0x0045AD50
WOWGUID GetGuidByKeyword(const char* token) {
    WOWGUID guid = WOWGUID();
    Script_GetGUIDFromToken(token, guid, false, false);
    return guid;
}

// OFFSET: 0x0044F8B0
bool GetUnitByKeyword(const char* token, CGUnit_C** unit, bool defaultToTarget) {
    WOWGUID guid;

    if (!Script_GetGUIDFromToken(token, guid, defaultToTarget, false)) {
        return false;
    }

    *unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));
    return true;
}

// OFFSET: 0x0044E490/0x14005C6A0
bool Script_GetNameFromToken(const char* token, const char** name, bool defaultToTarget, bool withRealm) {
    WOWGUID guid = WOWGUID();

    if (!Script_GetGUIDFromToken(token, guid, defaultToTarget, false)) {
        return false;
    }

    if (guid.m_guid == 0xFFFFFFFFFFFFFFFEULL) {
        return true;
    }

    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (unit) {
        if (!withRealm) {
            *name = unit->GetUnitName(NULL, 1);
            return true;
        }

        const char* realm = NULL;
        const char* unitName = unit->GetUnitName(&realm, 1);

        if (!unitName || !realm) {
            *name = unitName;
            return true;
        }

        SStrPrintf(s_nameWithRealm, sizeof(s_nameWithRealm), "%s-%s", unitName, realm);
        *name = s_nameWithRealm;
        return true;
    }

    if (!(guid.m_guid & 0xF000000000000000ULL) && (guid.m_guid & 0xF07FFFFFFFFFFFFFULL)) {
        WOWGUID requester = WOWGUID();
        NameCache* record = g_nameDBCache.GetRecord(guid.m_guid, requester, NULL, NULL, false);

        if (!record) {
            return true;
        }

        if (!withRealm || !record->m_realmName[0]) {
            *name = record->m_name;
            return true;
        }

        SStrPrintf(s_nameWithRealm, sizeof(s_nameWithRealm), "%s-%s", static_cast<const char*>(record->m_name), record->m_realmName);
        *name = s_nameWithRealm;
        return true;
    }

    if ((guid.m_guid & 0xF0F0000000000000ULL) != 0xF040000000000000ULL) {
        return true;
    }

    PetNameCache* pet = g_petNameCache.GetRecord(static_cast<int32_t>((guid.m_guid >> 24) & 0xFFFFFFF), guid, NULL, NULL, false);

    if (pet) {
        *name = pet->m_name;
    }

    return true;
}

// OFFSET: 0x0044EE40/0x14005D160
bool Script_GetGUIDFromString(const char** name, WOWGUID& guid, bool forceRealm) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));
    char buffer[49];
    char token[10];
    uint32_t length = 0;
    bool withRealm = false;

    while (**name && length < 48) {
        if (**name == '-') {
            if (!SStrCmpI(*name, "-target", STORM_MAX_STR) || !SStrCmpI(*name, "-pet", STORM_MAX_STR) || !SStrCmpI(*name, "-target-", 8) || !SStrCmpI(*name, "-pet-", 5)) {
                break;
            }

            withRealm = true;
        }

        buffer[length++] = *(*name)++;
    }

    buffer[length] = '\0';
    const char* unitName = NULL;

    if (Script_GetNameFromToken("player", &unitName, false, withRealm) && unitName && !SStrCmpUTF8I(buffer, unitName, STORM_MAX_STR)) {
        guid = player ? player->m_obj->m_guid : WOWGUID();
    } else if (CGRaidInfo::m_numMembers) {
        for (uint32_t i = 0; i < CGRaidInfo::m_numMembers; i++) {
            SStrPrintf(token, sizeof(token), "raid%d", i + 1);

            if (Script_GetNameFromToken(token, &unitName, false, withRealm) && unitName && !SStrCmpUTF8I(buffer, unitName, STORM_MAX_STR)) {
                guid = GetRaidMemberGUID(i);
                break;
            }
        }
    } else if (CGPartyInfo::NumMembers()) {
        for (uint32_t i = 0; i < CGPartyInfo::NumMembers(); i++) {
            SStrPrintf(token, sizeof(token), "party%d", i + 1);

            if (Script_GetNameFromToken(token, &unitName, false, withRealm) && unitName && !SStrCmpUTF8I(buffer, unitName, STORM_MAX_STR)) {
                guid = CGGameUI::GetPartyMember(i);
                break;
            }
        }
    } else {
        for (uint32_t i = 0; i < GetArenaPetGUIDCount(); i++) {
            SStrPrintf(token, sizeof(token), "arena%d", i + 1);

            if (Script_GetNameFromToken(token, &unitName, false, withRealm) && unitName && !SStrCmpUTF8I(buffer, unitName, STORM_MAX_STR)) {
                guid = CGBattlefieldInfo::GetArenaOpponent(i);
                break;
            }
        }
    }

    if (guid != WOWGUID()) {
        return true;
    }

    if (Script_GetNameFromToken("pet", &unitName, false, withRealm) && unitName && !SStrCmpUTF8I(buffer, unitName, STORM_MAX_STR)) {
        if (player) {
            guid = player->m_unit->m_unitFieldCharm != WOWGUID() ? player->m_unit->m_unitFieldCharm : player->m_unit->m_unitFieldSummon;
        } else {
            guid = WOWGUID();
        }
    } else if (CGRaidInfo::m_numMembers) {
        for (uint32_t i = 0; i < CGRaidInfo::m_numMembers; i++) {
            SStrPrintf(token, sizeof(token), "raidpet%d", i + 1);

            if (Script_GetNameFromToken(token, &unitName, false, withRealm) && unitName && !SStrCmpUTF8I(buffer, unitName, STORM_MAX_STR)) {
                guid = CGGameUI::GetRaidMemberPet(i);
                break;
            }
        }
    } else if (CGPartyInfo::NumMembers()) {
        for (uint32_t i = 0; i < CGPartyInfo::NumMembers(); i++) {
            SStrPrintf(token, sizeof(token), "partypet%d", i + 1);

            if (Script_GetNameFromToken(token, &unitName, false, withRealm) && unitName && !SStrCmpUTF8I(buffer, unitName, STORM_MAX_STR)) {
                guid = CGGameUI::GetPartyMemberPet(i);
                break;
            }
        }
    } else {
        for (uint32_t i = 0; i < GetArenaPetGUIDCount(); i++) {
            SStrPrintf(token, sizeof(token), "arenapet%d", i + 1);

            if (Script_GetNameFromToken(token, &unitName, false, withRealm) && unitName && !SStrCmpUTF8I(buffer, unitName, STORM_MAX_STR)) {
                guid = CGBattlefieldInfo::GetArenaOpponentPet(i);
                break;
            }
        }
    }

    return guid != WOWGUID();
}

// OFFSET: 0x0044F900
const char* Script_GetTokenFromGUID(const WOWGUID* guid) {
    if (!*guid) {
        return NULL;
    }

    CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();

    if (!player) {
        return NULL;
    }

    if (*guid == player->m_obj->m_guid) {
        return "player";
    }

    if (player->IsVehiclePassengerSeated() && guid->m_guid == player->GetTransportGUID()) {
        return "vehicle";
    }

    if (*guid == (player->m_unit->m_unitFieldCharm.m_guid ? player->m_unit->m_unitFieldCharm : player->m_unit->m_unitFieldSummon)) {
        return "pet";
    }

    for (uint32_t i = 0; i < CGPartyInfo::NumMembers(); i++) {
        if (*guid == CGGameUI::GetPartyMember(i)) {
            SStrPrintf(s_tokenFromGUID, sizeof(s_tokenFromGUID), "party%d", i + 1);
            return s_tokenFromGUID;
        }

        if (*guid == CGGameUI::GetPartyMemberPet(i)) {
            SStrPrintf(s_tokenFromGUID, sizeof(s_tokenFromGUID), "partypet%d", i + 1);
            return s_tokenFromGUID;
        }
    }

    for (uint32_t i = 0; i < CGRaidInfo::m_numMembers; i++) {
        if (*guid == GetRaidMemberGUID(i)) {
            SStrPrintf(s_tokenFromGUID, sizeof(s_tokenFromGUID), "raid%d", i + 1);
            return s_tokenFromGUID;
        }

        if (*guid == CGGameUI::GetRaidMemberPet(i)) {
            SStrPrintf(s_tokenFromGUID, sizeof(s_tokenFromGUID), "raidpet%d", i + 1);
            return s_tokenFromGUID;
        }
    }

    for (uint32_t i = 0; i < GetArenaPetGUIDCount(); i++) {
        if (*guid == CGBattlefieldInfo::GetArenaOpponent(i)) {
            SStrPrintf(s_tokenFromGUID, sizeof(s_tokenFromGUID), "arena%d", i + 1);
            return s_tokenFromGUID;
        }

        if (*guid == CGBattlefieldInfo::GetArenaOpponentPet(i)) {
            SStrPrintf(s_tokenFromGUID, sizeof(s_tokenFromGUID), "arenapet%d", i + 1);
            return s_tokenFromGUID;
        }
    }

    for (uint32_t i = 0; i < CGInstanceEncounter_C::GetNumBossSlots(); i++) {
        if (*guid == CGInstanceEncounter_C::GetBoss(i)) {
            SStrPrintf(s_tokenFromGUID, sizeof(s_tokenFromGUID), "boss%d", i + 1);
            return s_tokenFromGUID;
        }
    }

    if (*guid == CGGameUI::m_lockedTarget) {
        return "target";
    }

    if (*guid == CGGameUI::m_focusTarget) {
        return "focus";
    }

    if (*guid == CGGameUI::m_interactTarget) {
        return "npc";
    }

    if (*guid == CGGameUI::m_currentObjectTrack) {
        return "mouseover";
    }

    return NULL;
}

// OFFSET: 0x0044FC50
const char* Script_GetAuraTokenFromGUID(const WOWGUID* guid) {
    if (!*guid) {
        return NULL;
    }

    CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();

    if (!player) {
        return NULL;
    }

    if (*guid == player->m_obj->m_guid) {
        return "player";
    }

    if (player->IsVehiclePassengerSeated() && guid->m_guid == player->GetTransportGUID()) {
        return "vehicle";
    }

    if (*guid == (player->m_unit->m_unitFieldCharm ? player->m_unit->m_unitFieldCharm : player->m_unit->m_unitFieldSummon)) {
        return "pet";
    }

    for (uint32_t i = 0; i < CGPartyInfo::NumMembers(); i++) {
        if (*guid == CGGameUI::GetPartyMember(i)) {
            SStrPrintf(s_auraToken, sizeof(s_auraToken), "party%d", i + 1);
            return s_auraToken;
        }

        if (*guid == CGGameUI::GetPartyMemberPet(i)) {
            SStrPrintf(s_auraToken, sizeof(s_auraToken), "partypet%d", i + 1);
            return s_auraToken;
        }
    }

    for (uint32_t i = 0; i < CGRaidInfo::m_numMembers; i++) {
        if (*guid == GetRaidMemberGUID(i)) {
            SStrPrintf(s_auraToken, sizeof(s_auraToken), "raid%d", i + 1);
            return s_auraToken;
        }

        if (*guid == CGGameUI::GetRaidMemberPet(i)) {
            SStrPrintf(s_auraToken, sizeof(s_auraToken), "raidpet%d", i + 1);
            return s_auraToken;
        }
    }

    for (uint32_t i = 0; i < GetArenaPetGUIDCount(); i++) {
        if (*guid == CGBattlefieldInfo::GetArenaOpponent(i)) {
            SStrPrintf(s_auraToken, sizeof(s_auraToken), "arena%d", i + 1);
            return s_auraToken;
        }

        if (*guid == CGBattlefieldInfo::GetArenaOpponentPet(i)) {
            SStrPrintf(s_auraToken, sizeof(s_auraToken), "arenapet%d", i + 1);
            return s_auraToken;
        }
    }

    if (*guid == CGGameUI::m_lockedTarget) {
        return "target";
    }

    if (*guid == CGGameUI::m_focusTarget) {
        return "focus";
    }

    if (*guid == CGGameUI::m_interactTarget) {
        return "npc";
    }

    if (*guid == CGGameUI::m_currentObjectTrack) {
        return "mouseover";
    }

    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(*guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (!unit) {
        return NULL;
    }

    WOWGUID owner = unit->m_unit->m_unitFieldCharmedby ? unit->m_unit->m_unitFieldCharmedby : unit->m_unit->m_unitFieldCreatedby;
    WOWGUID charm = unit->m_unit->m_unitFieldCharm ? unit->m_unit->m_unitFieldCharm : unit->m_unit->m_unitFieldSummon;

    if (owner) {
        if (owner == player->m_obj->m_guid) {
            return "player";
        }
    } else if (!charm) {
        return NULL;
    }

    if (charm && charm == player->m_obj->m_guid) {
        return "player";
    }

    if (player->IsVehiclePassengerSeated()) {
        if (owner && owner.m_guid == player->GetTransportGUID()) {
            return "vehicle";
        }

        if (charm && charm.m_guid == player->GetTransportGUID()) {
            return "vehicle";
        }
    }

    WOWGUID playerPet = player->m_unit->m_unitFieldCharm ? player->m_unit->m_unitFieldCharm : player->m_unit->m_unitFieldSummon;

    if (owner && owner == playerPet) {
        return "pet";
    }

    if (charm && charm == playerPet) {
        return "pet";
    }

    for (uint32_t i = 0; i < CGPartyInfo::NumMembers(); i++) {
        WOWGUID member = CGGameUI::GetPartyMember(i);

        if ((owner && owner == member) || (charm && charm == member)) {
            SStrPrintf(s_auraToken, sizeof(s_auraToken), "party%d", i + 1);
            return s_auraToken;
        }

        WOWGUID memberPet = CGGameUI::GetPartyMemberPet(i);

        if ((owner && owner == memberPet) || (charm && charm == memberPet)) {
            SStrPrintf(s_auraToken, sizeof(s_auraToken), "partypet%d", i + 1);
            return s_auraToken;
        }
    }

    for (uint32_t i = 0; i < CGRaidInfo::m_numMembers; i++) {
        WOWGUID member = GetRaidMemberGUID(i);

        if ((owner && owner == member) || (charm && charm == member)) {
            SStrPrintf(s_auraToken, sizeof(s_auraToken), "raid%d", i + 1);
            return s_auraToken;
        }

        WOWGUID memberPet = CGGameUI::GetRaidMemberPet(i);

        if ((owner && owner == memberPet) || (charm && charm == memberPet)) {
            SStrPrintf(s_auraToken, sizeof(s_auraToken), "raidpet%d", i + 1);
            return s_auraToken;
        }
    }

    for (uint32_t i = 0; i < GetArenaPetGUIDCount(); i++) {
        WOWGUID opponent = CGBattlefieldInfo::GetArenaOpponent(i);

        if ((owner && owner == opponent) || (charm && charm == opponent)) {
            SStrPrintf(s_auraToken, sizeof(s_auraToken), "arena%d", i + 1);
            return s_auraToken;
        }

        WOWGUID opponentPet = CGBattlefieldInfo::GetArenaOpponentPet(i);

        if ((owner && owner == opponentPet) || (charm && charm == opponentPet)) {
            SStrPrintf(s_auraToken, sizeof(s_auraToken), "arenapet%d", i + 1);
            return s_auraToken;
        }
    }

    if ((owner && owner == CGGameUI::m_lockedTarget) || (charm && charm == CGGameUI::m_lockedTarget)) {
        return "target";
    }

    if ((owner && owner == CGGameUI::m_focusTarget) || (charm && charm == CGGameUI::m_focusTarget)) {
        return "focus";
    }

    if ((owner && owner == CGGameUI::m_interactTarget) || (charm && charm == CGGameUI::m_interactTarget)) {
        return "npc";
    }

    if ((owner && owner == CGGameUI::m_currentObjectTrack) || (charm && charm == CGGameUI::m_currentObjectTrack)) {
        return "mouseover";
    }

    return NULL;
}

// OFFSET: 0x0045A130
int32_t PushUnitAura(lua_State* L, ScriptAuraInfo* info) {
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(info->m_guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (unit && (unit->m_auraEffectMask[36] & 0x80) && ClntObjMgrGetActivePlayer() && !unit->IsFriendly(static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__)))) {
        return 0;
    }

    lua_checkstack(L, 16);

    if (!unit || (unit->m_modelFlags & 0x10000)) {
        uint32_t count;
        const TSGrowableArray<PartyMemberAuraStates>* auras;

        if (PartyMemberState* state = CGGameUI::GetPartyMemberState(info->m_guid)) {
            if (!(state->m_flags & 0x1)) {
                return 0;
            }

            count = state->m_numAuras;
            auras = &state->m_auras;
        } else if (PartyMemberPetState* petState = CGGameUI::GetPartyMemberPetState(info->m_guid)) {
            count = petState->m_numAuras;
            auras = &petState->m_auras;
        } else {
            return 0;
        }

        if (!auras || !auras->m_count) {
            return 0;
        }

        bool helpful = (info->m_filter & 0x1) != 0;
        bool raid = (info->m_filter & 0x8) != 0;
        int32_t found = 0;

        for (uint32_t i = 0; i < count; i++) {
            const PartyMemberAuraStates* entry = &auras->m_data[i];

            if (!entry->m_spellId) {
                break;
            }

            if (helpful != !(entry->m_flags & 0x80)) {
                continue;
            }

            SpellRec* spell = g_spellDB.GetRecord(entry->m_spellId);

            if (!spell) {
                continue;
            }

            WOWGUID caster = WOWGUID::Make(0);

            if (!CGSpellBook::CanShowAura(spell, caster, (entry->m_flags & 0x8) != 0)) {
                continue;
            }

            int32_t dispelType = 0;

            if (spell->m_spellCategoriesID) {
                SpellCategoriesRec* categories = g_spellCategoriesDB.GetRecord(spell->m_spellCategoriesID);

                if (categories) {
                    dispelType = categories->m_dispelType;
                }
            }

            if (raid && !(helpful ? CGSpellBook::FindRaidBuff(entry->m_spellId) : CGSpellBook::FindRaidDispel(dispelType))) {
                continue;
            }

            if ((entry->m_flags & 0x10) ? (info->m_filter & 0x20) : (info->m_filter & 0x10)) {
                continue;
            }

            if ((info->m_index >= 0 && info->m_index == found) || (info->m_name && !SStrCmpUTF8I(info->m_name, spell->m_name, 0x7FFFFFFF) && (!info->m_rank || !SStrCmpUTF8I(info->m_rank, spell->m_nameSubtext, 0x7FFFFFFF)))) {
                const char* dispelName = NULL;

                if (dispelType) {
                    SpellDispelTypeRec* dispel = g_spellDispelTypeDB.GetRecord(dispelType);

                    if (dispel && dispel->m_immunityPossible) {
                        dispelName = dispel->m_internalName;
                    }
                }

                SpellIconRec* icon = g_spellIconDB.GetRecord(spell->m_spellIconID);
                lua_pushstring(L, spell->m_name);
                lua_pushstring(L, spell->m_nameSubtext);
                lua_pushstring(L, icon ? static_cast<const char*>(icon->m_textureFilename) : NULL);
                lua_pushnumber(L, 1.0);
                lua_pushstring(L, dispelName);
                lua_pushnumber(L, 0.0);
                lua_pushnumber(L, 0.0);
                lua_pushnil(L);
                lua_pushnil(L);

                if ((spell->m_attributes & 0x4000000) || !(spell->m_attributesExG & 0x10000000)) {
                    lua_pushnil(L);
                } else {
                    lua_pushnumber(L, 1.0);
                }

                lua_pushnumber(L, spell->m_id);
                WOWGUID none = WOWGUID::Make(0);
                lua_pushboolean(L, CGSpellBook::CanApplyAura(spell->m_id, &none));

                if (spell->m_attributesExH & 0x40000000) {
                    lua_pushnumber(L, 1.0);
                } else {
                    lua_pushnil(L);
                }

                for (uint32_t j = 0; j < 3; j++) {
                    lua_pushnumber(L, auras->m_data[i].m_effectAmounts[j]);
                }

                lua_pushboolean(L, 0);
                return 17;
            }

            found++;
        }

        return 0;
    }

    int32_t slot = unit->GetSortedAuraSlot(info->m_index, info->m_name, info->m_rank, info->m_filter);

    if (slot < 0) {
        return 0;
    }

    SpellRec* spell = g_spellDB.GetRecord(unit->GetAuraSpellId(slot));

    if (!spell) {
        return 0;
    }

    AuraData* aura = unit->GetAura(slot);
    const char* dispelName = NULL;

    if (spell->m_spellCategoriesID) {
        SpellCategoriesRec* categories = g_spellCategoriesDB.GetRecord(spell->m_spellCategoriesID);

        if (categories && categories->m_dispelType) {
            SpellDispelTypeRec* dispel = g_spellDispelTypeDB.GetRecord(categories->m_dispelType);

            if (dispel && dispel->m_immunityPossible) {
                dispelName = dispel->m_internalName;
            }
        }
    }

    SpellIconRec* icon = g_spellIconDB.GetRecord(spell->m_spellIconID);
    lua_pushstring(L, spell->m_name);
    lua_pushstring(L, spell->m_nameSubtext);
    lua_pushstring(L, icon ? static_cast<const char*>(icon->m_textureFilename) : NULL);
    lua_pushnumber(L, aura->m_stackCount);
    lua_pushstring(L, dispelName);

    if (aura->m_duration <= 0) {
        lua_pushnumber(L, 0.0);
        lua_pushnumber(L, 0.0);
    } else {
        lua_pushnumber(L, aura->m_duration * 0.001);
        lua_pushnumber(L, static_cast<uint32_t>(aura->m_endTime) * 0.001);
    }

    lua_pushstring(L, Script_GetAuraTokenFromGUID(&aura->m_creator));

    if (CGSpellBook::CanStealAura(unit, aura, spell)) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    if ((spell->m_attributes & 0x4000000) || !(spell->m_attributesExG & 0x10000000)) {
        lua_pushnil(L);
    } else {
        lua_pushnumber(L, 1.0);
    }

    lua_pushnumber(L, spell->m_id);
    lua_pushboolean(L, CGSpellBook::CanApplyAura(aura));

    if (spell->m_attributesExH & 0x40000000) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    for (uint32_t j = 0; j < 3; j++) {
        lua_pushnumber(L, aura->m_effectAmounts[j]);
    }

    uint32_t high = static_cast<uint32_t>(aura->m_creator.m_guid >> 32);

    if ((high & 0xF0000000) || !((high & 0xF07FFFFF) | static_cast<uint32_t>(aura->m_creator.m_guid))) {
        lua_pushboolean(L, 0);
    } else {
        lua_pushboolean(L, 1);
    }

    return 17;
}

// OFFSET: 0x00450380
char** Script_GetTokenIDsFromGUID(const WOWGUID& guid, int32_t* count) {
    *count = 0;

    if (guid == WOWGUID()) {
        return NULL;
    }

    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return NULL;
    }

    if (guid == player->m_obj->m_guid) {
        SStrCopy(s_tokenIDs[(*count)++], "player", sizeof(s_tokenBuffers[0]));
    }

    if (player->IsVehiclePassengerSeated() && guid.m_guid == player->GetTransportGUID()) {
        SStrCopy(s_tokenIDs[(*count)++], "vehicle", sizeof(s_tokenBuffers[0]));
    }

    if (guid == (player->m_unit->m_unitFieldCharm != WOWGUID() ? player->m_unit->m_unitFieldCharm : player->m_unit->m_unitFieldSummon)) {
        SStrCopy(s_tokenIDs[(*count)++], "pet", sizeof(s_tokenBuffers[0]));
    }

    for (uint32_t i = 0; i < CGPartyInfo::NumMembers(); i++) {
        if (guid == CGGameUI::GetPartyMember(i)) {
            SStrPrintf(s_tokenIDs[(*count)++], sizeof(s_tokenBuffers[0]), "party%d", i + 1);
            break;
        }

        if (guid == CGGameUI::GetPartyMemberPet(i)) {
            SStrPrintf(s_tokenIDs[(*count)++], sizeof(s_tokenBuffers[0]), "partypet%d", i + 1);
            break;
        }
    }

    for (uint32_t i = 0; i < CGRaidInfo::m_numMembers; i++) {
        if (guid == GetRaidMemberGUID(i)) {
            SStrPrintf(s_tokenIDs[(*count)++], sizeof(s_tokenBuffers[0]), "raid%d", i + 1);
            break;
        }

        if (guid == CGGameUI::GetRaidMemberPet(i)) {
            SStrPrintf(s_tokenIDs[(*count)++], sizeof(s_tokenBuffers[0]), "raidpet%d", i + 1);
            break;
        }
    }

    for (uint32_t i = 0; i < GetArenaPetGUIDCount(); i++) {
        if (guid == CGBattlefieldInfo::GetArenaOpponent(i)) {
            SStrPrintf(s_tokenIDs[(*count)++], sizeof(s_tokenBuffers[0]), "arena%d", i + 1);
            break;
        }

        if (guid == CGBattlefieldInfo::GetArenaOpponentPet(i)) {
            SStrPrintf(s_tokenIDs[(*count)++], sizeof(s_tokenBuffers[0]), "arenapet%d", i + 1);
            break;
        }
    }

    for (uint32_t i = 0; i < CGInstanceEncounter_C::GetNumBossSlots(); i++) {
        if (guid == CGInstanceEncounter_C::GetBoss(i)) {
            SStrPrintf(s_tokenIDs[(*count)++], sizeof(s_tokenBuffers[0]), "boss%d", i + 1);
            break;
        }
    }

    if (guid == CGGameUI::m_lockedTarget) {
        SStrCopy(s_tokenIDs[(*count)++], "target", sizeof(s_tokenBuffers[0]));
    }

    if (guid == CGGameUI::m_focusTarget) {
        SStrCopy(s_tokenIDs[(*count)++], "focus", sizeof(s_tokenBuffers[0]));
    }

    if (guid == CGGameUI::m_interactTarget) {
        SStrCopy(s_tokenIDs[(*count)++], "npc", sizeof(s_tokenBuffers[0]));
    }

    return s_tokenIDs;
}

// PROVEN: 3e530f3f IDENTICAL/NONE
// OFFSET: 0x00450760/0x14005EA40
void Script_SendUnitSignal(const WOWGUID& guid, int32_t signal) {
    int32_t count;
    char** tokens = Script_GetTokenIDsFromGUID(guid, &count);

    for (int32_t i = 0; i < count; i++) {
        FrameScript_SignalEvent(signal, "%s", tokens[i]);
    }
}

// OFFSET: 0x004507B0/0x14005EAA0
void Script_SendUnitSignal(const WOWGUID& guid, int32_t signal, const char* format, ...) {
    va_list args;
    va_start(args, format);

    int32_t count;
    char** tokens = Script_GetTokenIDsFromGUID(guid, &count);

    for (int32_t i = 0; i < count; i++) {
        if (!FrameScript_ShouldSignalEvent(signal)) {
            continue;
        }

        lua_State* L = FrameScript_GetContext();
        const char* taint = lua_tainted;

        if (++lua_taintexpected && !lua_taintedclosure) {
            lua_tainted = NULL;
        }

        FrameScript_PushEventName(signal);
        lua_pushstring(L, tokens[i]);
        int32_t argCount = 2;

        if (format) {
            argCount = vFrameScript_PushEventArgs(format, args) + 2;
        }

        FrameScript_SignalEvent(signal, L, argCount);
        lua_settop(L, -1 - argCount);

        if (lua_taintexpected && !lua_taintedclosure) {
            lua_tainted = taint;
        }

        if (--lua_taintexpected <= 0) {
            lua_taintexpected = 0;
        }
    }
}

// OFFSET: 0x004508B0
void Script_SendUnitSpellSignal(const WOWGUID& guid, uint8_t castCount, const SpellRec* spell, int32_t signal) {
    if (!spell) {
        Script_SendUnitSignal(guid, signal);
        return;
    }

    int32_t count;
    char** tokens = Script_GetTokenIDsFromGUID(guid, &count);

    for (int32_t i = 0; i < count; i++) {
        FrameScript_SignalEvent(signal, "%s%s%s%d%d", tokens[i], static_cast<const char*>(spell->m_name), static_cast<const char*>(spell->m_nameSubtext), castCount, spell->m_id);
    }
}

// OFFSET: 0x0044BF30
static int32_t Script_GetAuraInfo(lua_State* L, int32_t index, ScriptAuraInfo* info) {
    if (!lua_isnumber(L, index) && !lua_isstring(L, index)) {
        return 0;
    }

    int32_t filterIndex;

    if (lua_isnumber(L, index)) {
        info->m_index = static_cast<int32_t>(lua_tointeger(L, index)) - 1;
        filterIndex = index + 1;
        info->m_name = NULL;
        info->m_rank = NULL;
    } else {
        info->m_index = -1;
        info->m_name = lua_tostring(L, index);
        info->m_rank = lua_tostring(L, index + 1);
        filterIndex = index + 2;
    }

    info->m_filter = 1;

    if (lua_isstring(L, filterIndex)) {
        const char* filter = lua_tostring(L, filterIndex);
        char token[32];

        do {
            SStrTokenize(&filter, token, sizeof(token), " |", NULL);

            if (!token[0]) {
                break;
            }

            if (!SStrCmpI(token, "HELPFUL", STORM_MAX_STR)) {
                info->m_filter = (info->m_filter & ~3) | 1;
            } else if (!SStrCmpI(token, "HARMFUL", STORM_MAX_STR)) {
                info->m_filter = (info->m_filter & ~3) | 2;
            } else if (!SStrCmpI(token, "PLAYER", STORM_MAX_STR)) {
                info->m_filter |= 4;
            } else if (!SStrCmpI(token, "RAID", STORM_MAX_STR)) {
                info->m_filter |= 8;
            } else if (!SStrCmpI(token, "CANCELABLE", STORM_MAX_STR)) {
                info->m_filter |= 0x10;
            } else if (!SStrCmpI(token, "NOT_CANCELABLE", STORM_MAX_STR)) {
                info->m_filter |= 0x20;
            }
        } while (token[0] && *filter);
    }

    return 1;
}

// OFFSET: 0x00455840
int32_t Script_GetUnitAuraInfo(lua_State* L, int32_t index, ScriptAuraInfo* info) {
    if (!lua_isstring(L, index) || (!lua_isnumber(L, index + 1) && !lua_isstring(L, index + 1))) {
        return 0;
    }

    WOWGUID guid;
    Script_GetGUIDFromToken(lua_tostring(L, index), guid, false, false);
    info->m_guid = guid;
    return Script_GetAuraInfo(L, index + 1, info);
}

// OFFSET: #0x006147B0
void ClearUniqueSignals() {
    s_uniqueSignals.m_count = 0;
}

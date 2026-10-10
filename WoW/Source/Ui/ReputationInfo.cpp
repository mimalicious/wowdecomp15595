#include "ReputationInfo.h"
#include "Engine/Source/Base/CDataStore.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"
#include <cstdlib>
#include <malloc.h>
#include "Engine/Source/ConsoleBase/ConsoleVar.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/Storm/H/storm.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/FactionRec.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/Ui/ChatFrame.h"
#include "WoW/Source/Ui/Types.h"
#include <new>
#include "WoW/Source/Object/ObjectClient/Effect_C.h"
#include "WoW/Source/Ui/QuestLog.h"
#include "WoW/Source/Ui/Tutorial.h"
#include <cstring>
#include "WoW/Source/Object/Unit.h"
#include "WoW/Source/Object/Player.h"
#include "WoW/Source/Ui/GameUI.h"

// STATIC: 0x00F4BA28
FactionStanding CGReputationInfo::m_factionStandings[256];

// STATIC: 0x00F4CA28
FactionHeader* CGReputationInfo::m_headers[128];

// STATIC: 0x00F4CC28
uint32_t CGReputationInfo::m_numHeaders;

// STATIC: 0x00F4CC2C
uint32_t CGReputationInfo::m_numFactions;

// STATIC: 0x00F4CC30
uint32_t CGReputationInfo::m_numShownFactions;

// STATIC: 0x00F4CC38
TSFixedArray<FactionListEntry*> CGReputationInfo::m_factionList;

// STATIC: 0x00F4CC44
TSGrowableArray<ForcedReaction> CGReputationInfo::m_forcedReactions;

// STATIC: 0x00F4CC54
TSHashTable<FACTION, HASHKEY_NONE> CGReputationInfo::m_factions;

// STATIC: 0x00D151A0
uint32_t CGReputationInfo::m_collapsedHeaders = 0xFFFFFFFF;

// STATIC: 0x00F4CC34
int32_t CGReputationInfo::m_selectedFaction;

// STATIC: 0x00B9E614
const int32_t CGReputationInfo::m_reactionThresholds[9] = { -42000, -6000, -3000, 0, 3000, 9000, 21000, 42000, 43000 };

// OFFSET: 0x009A59B0
static int32_t GetHeaderIndex(int32_t factionId) {
    for (uint32_t i = 0; i < CGReputationInfo::m_numHeaders; i++) {
        if (CGReputationInfo::m_headers[i]->m_factionId == factionId) {
            return i;
        }
    }

    return -1;
}

// OFFSET: 0x009A59E0/0x14072BDC0
static int32_t GetReactionFromStanding(int32_t standing) {
    if (standing >= 42000) {
        return 7;
    }

    if (standing >= 21000) {
        return 6;
    }

    if (standing >= 9000) {
        return 5;
    }

    if (standing >= 3000) {
        return 4;
    }

    if (standing >= 0) {
        return 3;
    }

    if (standing >= -3000) {
        return 2;
    }

    return standing >= -6000;
}

// OFFSET: 0x009A5A50/0x14072BD90
static bool IsHeaderDescendant(int32_t headerIndex, int32_t ancestor) {
    if (ancestor == -1) {
        return false;
    }

    while (ancestor) {
        int32_t parent = CGReputationInfo::m_headers[headerIndex]->m_parent;

        if (parent == ancestor) {
            return true;
        }

        if (!parent || parent == -1) {
            return false;
        }

        headerIndex = GetHeaderIndex(parent);
    }

    return false;
}

// OFFSET: 0x009A5D10
int32_t CGReputationInfo::GetSelectedFactionIndex() {
    for (uint32_t i = 0; i < CGReputationInfo::m_numShownFactions; i++) {
        if (CGReputationInfo::m_factionList.m_data[i]->m_factionId == CGReputationInfo::m_selectedFaction) {
            return i;
        }
    }

    return -1;
}

// OFFSET: 0x009A5D50
static int QSortFactions(const void* a, const void* b) {
    const FactionListEntry* left = *static_cast<FactionListEntry* const*>(a);
    const FactionListEntry* right = *static_cast<FactionListEntry* const*>(b);
    uint32_t leftHeader = left->m_headerIndex;
    uint32_t rightHeader = right->m_headerIndex;

    if (leftHeader == rightHeader) {
        if (left->m_isHeader || right->m_isHeader) {
            return 2 * (left->m_isHeader == 0) - 1;
        }

        FactionRec* leftRec = g_factionDB.GetRecord(left->m_factionId);
        FactionRec* rightRec = g_factionDB.GetRecord(right->m_factionId);

        if (leftRec && rightRec) {
            return SStrSortUTF8I(leftRec->m_name, rightRec->m_name, STORM_MAX_STR);
        }

        return 0;
    }

    FactionHeader* leftParent = leftHeader > 128 ? NULL : CGReputationInfo::m_headers[leftHeader];
    bool leftHidden = !left->m_isHeader && leftHeader < 128 && !(CGReputationInfo::m_collapsedHeaders & (1 << leftHeader));

    if (leftParent && !leftHidden) {
        leftHidden = leftParent->m_collapsed || (left->m_isHeader && (!leftParent->m_factionId || leftParent->m_factionId == -1) && !leftParent->m_count);
    }

    FactionHeader* rightParent = rightHeader > 128 ? NULL : CGReputationInfo::m_headers[rightHeader];
    bool rightHidden = !right->m_isHeader && rightHeader < 128 && !(CGReputationInfo::m_collapsedHeaders & (1 << rightHeader));

    if (rightParent && !rightHidden) {
        rightHidden = rightParent->m_collapsed || (right->m_isHeader && (!rightParent->m_factionId || rightParent->m_factionId == -1) && !rightParent->m_count);
    }

    if (leftHidden) {
        return !rightHidden;
    }

    if (rightHidden) {
        return -1;
    }

    return 2 * (static_cast<int32_t>(leftHeader) >= static_cast<int32_t>(rightHeader)) - 1;
}

// OFFSET: 0x009A5F10/0x14072C430
bool CGReputationInfo::IsAtWar(int32_t faction) {
    FactionRec* rec = g_factionDB.GetRecord(faction);

    if (rec && static_cast<uint32_t>(rec->m_reputationIndex) <= 255) {
        return (CGReputationInfo::m_factionStandings[rec->m_reputationIndex].m_flags >> 1) & 1;
    }

    return false;
}

// OFFSET: 0x009A5F60
static bool IsFactionPeaceForced(int32_t faction) {
    FactionRec* rec = g_factionDB.GetRecord(faction);

    if (rec && static_cast<uint32_t>(rec->m_reputationIndex) <= 255) {
        return (CGReputationInfo::m_factionStandings[rec->m_reputationIndex].m_flags >> 4) & 1;
    }

    return false;
}

// OFFSET: 0x009A5FC0/0x14072C480
bool CGReputationInfo::IsFactionInactive(uint32_t index) {
    int32_t factionId = index <= CGReputationInfo::m_numFactions ? CGReputationInfo::m_factionList.m_data[index]->m_factionId : 0;
    FactionRec* faction = g_factionDB.GetRecord(factionId);

    if (faction && static_cast<uint32_t>(faction->m_reputationIndex) <= 255) {
        return (CGReputationInfo::m_factionStandings[faction->m_reputationIndex].m_flags >> 5) & 1;
    }

    return false;
}

// OFFSET: 0x009A6020/0x14072C4F0
int32_t GetRepListRepValue(int32_t faction) {
    FactionRec* rec = g_factionDB.GetRecord(faction);

    if (!rec || static_cast<uint32_t>(rec->m_reputationIndex) > 255) {
        return 0;
    }

    return CGReputationInfo::m_factionStandings[rec->m_reputationIndex].m_base + CGReputationInfo::m_factionStandings[rec->m_reputationIndex].m_standing;
}

// OFFSET: 0x009A6070/0x14072C540
int32_t CGReputationInfo::GetFactionStandingReaction(int32_t faction) {
    FactionRec* rec = g_factionDB.GetRecord(faction);

    if (!rec || static_cast<uint32_t>(rec->m_reputationIndex) > 255) {
        return GetReactionFromStanding(0);
    }

    return GetReactionFromStanding(CGReputationInfo::m_factionStandings[rec->m_reputationIndex].m_base + CGReputationInfo::m_factionStandings[rec->m_reputationIndex].m_standing);
}

// PROVEN: 4466791a IDENTICAL/NONE
// OFFSET: 0x009A60D0/0x14072C5F0
bool CGReputationInfo::GetForcedReaction(int32_t faction, int32_t* reaction) {
    for (uint32_t i = 0; i < CGReputationInfo::m_forcedReactions.m_count; i++) {
        if (faction == CGReputationInfo::m_forcedReactions.m_data[i].m_factionId) {
            *reaction = CGReputationInfo::m_forcedReactions.m_data[i].m_reaction;
            return true;
        }
    }

    return false;
}

// OFFSET: 0x009A6110
static bool IsChildFaction(int32_t faction) {
    for (uint32_t i = 0; i < CGReputationInfo::m_numShownFactions; i++) {
        FactionListEntry* entry = CGReputationInfo::m_factionList.m_data[i];

        if (entry->m_factionId == faction && static_cast<int32_t>(entry->m_headerIndex) >= 0) {
            return CGReputationInfo::m_headers[entry->m_headerIndex]->m_parent != 0;
        }
    }

    return false;
}

// OFFSET: 0x009A6270
int32_t Packet_SMSG_SET_FACTION_ATWAR(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    uint32_t repListId;
    uint8_t flags;
    msg->Get(repListId);
    msg->Get(flags);

    uint8_t current = CGReputationInfo::m_factionStandings[repListId].m_flags;
    CGReputationInfo::m_factionStandings[repListId].m_flags = (flags & 0x2) ? (current | 0x2) : (current & ~0x2);
    FrameScript_SignalEvent(SCRIPT_EVENT_UPDATE_FACTION, NULL);

    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player) {
        player->UpdateReaction(false);
    }

    return 1;
}

// OFFSET: 0x009A62F0
uint32_t CGReputationInfo::AddHeader(int32_t factionId, int32_t parent) {
    uint32_t index;

    for (index = 0; index < CGReputationInfo::m_numHeaders; index++) {
        if (CGReputationInfo::m_headers[index]->m_factionId == factionId) {
            break;
        }
    }

    if (index != CGReputationInfo::m_numHeaders) {
        return index;
    }

    CGReputationInfo::m_headers[CGReputationInfo::m_numHeaders]->m_factionId = factionId;
    CGReputationInfo::m_headers[CGReputationInfo::m_numHeaders]->m_collapsed = 0;

    if (parent) {
        for (uint32_t i = 0; i < CGReputationInfo::m_numHeaders; i++) {
            FactionHeader* header = CGReputationInfo::m_headers[i];

            if (header->m_factionId == parent) {
                if (header->m_collapsed || (i < 128 && !(CGReputationInfo::m_collapsedHeaders & (1 << i)))) {
                    CGReputationInfo::m_headers[CGReputationInfo::m_numHeaders]->m_collapsed = 1;
                }

                break;
            }
        }
    }

    CGReputationInfo::m_headers[CGReputationInfo::m_numHeaders]->m_parent = parent;
    CGReputationInfo::m_numHeaders++;
    uint32_t bit = 1 << index;
    CGReputationInfo::m_collapsedHeaders |= bit;

    CVarIntListWrapper list;
    list.m_cvar = CVar::Lookup("reputationsCollapsed");
    list.m_pos = 2;
    list.m_state = 0;
    list.GetNextValue();

    for (uint32_t value = list.GetNextValue(); value; value = list.GetNextValue()) {
        if (value != static_cast<uint32_t>(factionId)) {
            continue;
        }

        CGReputationInfo::m_collapsedHeaders &= ~bit;

        for (uint32_t i = 0; i < CGReputationInfo::m_numHeaders; i++) {
            if (factionId == -1 || !factionId) {
                continue;
            }

            FactionHeader* header = CGReputationInfo::m_headers[i];
            int32_t headerParent = header->m_parent;

            if (headerParent == factionId) {
                header->m_collapsed = 1;
            } else if (headerParent && headerParent != -1 && IsHeaderDescendant(GetHeaderIndex(headerParent), factionId)) {
                header->m_collapsed = 1;
            }
        }

        break;
    }

    if (list.m_state != 1) {
        char value[256];
        value[0] = 'v';
        value[1] = 1;
        value[list.m_pos] = '\0';
        list.m_cvar->Set(value, true, false, false, true);
    }

    return index;
}

// OFFSET: 0x009A64E0
void CGReputationInfo::SetAtWar(int32_t faction, bool atWar) {
    uint32_t header;

    for (header = 0; header < CGReputationInfo::m_numHeaders; header++) {
        if (CGReputationInfo::m_headers[header]->m_factionId == faction) {
            break;
        }
    }

    if (header < CGReputationInfo::m_numHeaders && static_cast<int32_t>(header) > 0) {
        return;
    }

    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));
    FactionRec* rec;

    if ((!atWar && GetRepListRepValue(faction) < -3000) || !(rec = g_factionDB.GetRecord(faction))) {
        return;
    }

    uint32_t index = rec->m_reputationIndex;

    if (index > 255) {
        return;
    }

    if (!atWar && player && (player->m_unit->m_unitFieldFlags & 0x80000)) {
        CGGameUI::DisplayError(ERR_NOT_IN_COMBAT);
        return;
    }

    uint8_t flags = CGReputationInfo::m_factionStandings[index].m_flags;
    CGReputationInfo::m_factionStandings[index].m_flags = (!atWar || (flags & 0x10)) ? flags & ~2 : flags | 2;
    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_SET_FACTION_AT_WAR));
    msg.Put(index);
    msg.Put(static_cast<uint8_t>(atWar != 0));
    msg.Finalize();
    ClientServices::Send2(&msg);

    if (player) {
        player->UpdateReaction(false);
    }
}

// OFFSET: 0x009A6670
void CGReputationInfo::SetWatchedFaction(int32_t faction) {
    FactionRec* rec = g_factionDB.GetRecord(faction);
    int32_t index = rec ? rec->m_reputationIndex : -1;
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player && static_cast<uint32_t>(index) != player->m_player->m_playerFieldWatchedFactionIndex) {
        CDataStore msg;
        msg.Put(static_cast<uint32_t>(CMSG_SET_WATCHED_FACTION));
        msg.Put(static_cast<uint32_t>(index));
        msg.Finalize();
        ClientServices::Send2(&msg);
    }
}

// OFFSET: 0x009A6740/0x14072CD90
static int32_t IsPlayerWatchingFaction(int32_t faction) {
    FactionRec* rec = g_factionDB.GetRecord(faction);
    int32_t index = rec ? rec->m_reputationIndex : -1;
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));
    return player && player->m_player->m_playerFieldWatchedFactionIndex == static_cast<uint32_t>(index);
}

// OFFSET: 0x009A67B0/0x14072CE10
static void SaveCollapsedHeaders() {
    CVarIntListWrapper list;
    list.m_cvar = CVar::Lookup("reputationsCollapsed");
    list.m_pos = 2;
    list.m_state = 0;
    list.AddNextValue(1);

    for (uint32_t i = 0; i < CGReputationInfo::m_numHeaders; i++) {
        if (i < 128 && !(CGReputationInfo::m_collapsedHeaders & (1 << i)) && CGReputationInfo::m_headers[i]->m_factionId != -1) {
            list.AddNextValue(CGReputationInfo::m_headers[i]->m_factionId);
        }
    }

    list.m_buffer[0] = 'v';
    list.m_buffer[1] = 1;
    list.m_buffer[list.m_pos] = 0;
    list.m_cvar->Set(list.m_buffer, true, false, false, true);
}

// OFFSET: 0x009A6880/0x14072CF10
static bool FactionHasReputationForPlayer(const FactionRec* faction) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return false;
    }

    uint8_t classId = player->m_unit->m_unitFieldBytes0[1];
    uint8_t race = player->m_unit->m_unitFieldBytes0[0];

    for (uint32_t i = 0; i < 4; i++) {
        int32_t raceMask = faction->m_reputationRaceMask[i];
        int32_t classMask = faction->m_reputationClassMask[i];

        if (raceMask) {
            if (!((1 << (race - 1)) & raceMask)) {
                continue;
            }

            if (!classMask) {
                return static_cast<int8_t>(faction->m_reputationFlags[i]) < 0 && faction->m_reputationIndex >= 0;
            }
        } else if (!classMask) {
            continue;
        }

        if ((1 << (classId - 1)) & classMask) {
            return static_cast<int8_t>(faction->m_reputationFlags[i]) < 0 && faction->m_reputationIndex >= 0;
        }
    }

    return false;
}

// OFFSET: 0x009A6940/0x14072CFF0
int32_t GetFactionInfoHelper(lua_State* L, int32_t faction) {
    FactionRec* rec = g_factionDB.GetRecord(faction);

    if (!rec) {
        uint32_t index = 0;

        while (index < CGReputationInfo::m_numHeaders && CGReputationInfo::m_headers[index]->m_factionId != faction) {
            index++;
        }

        if (index < CGReputationInfo::m_numHeaders && index < 128) {
            const char* text = NULL;

            if (CGReputationInfo::m_headers[index]->m_factionId == -1) {
                text = FrameScript_GetText("FACTION_INACTIVE", -1, GENDER_NOT_APPLICABLE);
            } else if (!CGReputationInfo::m_headers[index]->m_factionId) {
                text = FrameScript_GetText("FACTION_OTHER", -1, GENDER_NOT_APPLICABLE);
            }

            if (text) {
                lua_pushstring(L, text);
                lua_pushstring(L, "");
                lua_pushnumber(L, 1.0);
                lua_pushnumber(L, 0.0);
                lua_pushnumber(L, 0.0);
                lua_pushnumber(L, 0.0);
                lua_pushnil(L);
                lua_pushnil(L);
                lua_pushnumber(L, 1.0);

                if (CGReputationInfo::m_collapsedHeaders & (1 << index)) {
                    lua_pushnil(L);
                } else {
                    lua_pushnumber(L, 1.0);
                }

                lua_pushnil(L);
                lua_pushnil(L);
                lua_pushnil(L);
                return 13;
            }
        }

        lua_pushnil(L);
        lua_pushnil(L);
        lua_pushnumber(L, 1.0);
        lua_pushnumber(L, 0.0);
        lua_pushnumber(L, 0.0);
        lua_pushnumber(L, 0.0);
        lua_pushnil(L);
        lua_pushnil(L);
        lua_pushnil(L);
        lua_pushnil(L);
        lua_pushnil(L);
        lua_pushnil(L);
        lua_pushnil(L);
        return 13;
    }

    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));
    const char* name;

    if (!player || !(name = player->GetPlayerGuildNameText()) || !(rec->m_reputationFlags[0] & 0x200)) {
        name = rec->m_name;
    }

    lua_pushstring(L, name);
    lua_pushstring(L, rec->m_description);
    int32_t reaction = CGReputationInfo::GetFactionStandingReaction(faction);
    lua_pushnumber(L, reaction + 1);
    int32_t max = CGReputationInfo::m_reactionThresholds[reaction + 1];
    int32_t value = GetRepListRepValue(faction);
    lua_pushnumber(L, CGReputationInfo::m_reactionThresholds[reaction]);
    lua_pushnumber(L, max);
    lua_pushnumber(L, value);

    if (CGReputationInfo::IsAtWar(faction)) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    if (value < -3000 || IsFactionPeaceForced(faction)) {
        lua_pushnil(L);
    } else {
        lua_pushnumber(L, 1.0);
    }

    uint32_t index = 0;

    while (index < CGReputationInfo::m_numHeaders && CGReputationInfo::m_headers[index]->m_factionId != faction) {
        index++;
    }

    if (index < CGReputationInfo::m_numHeaders) {
        lua_pushnumber(L, 1.0);

        if (index >= 128 || (CGReputationInfo::m_collapsedHeaders & (1 << index))) {
            lua_pushnil(L);
        } else {
            lua_pushnumber(L, 1.0);
        }

        if (FactionHasReputationForPlayer(rec)) {
            lua_pushnumber(L, 1.0);
        } else {
            lua_pushnil(L);
        }
    } else {
        lua_pushnil(L);
        lua_pushnil(L);
        lua_pushnil(L);
    }

    if (IsPlayerWatchingFaction(faction)) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    if (IsChildFaction(faction)) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 13;
}

// OFFSET: 0x009A71C0
void CGReputationInfo::ShutdownGame() {
    CGReputationInfo::m_numFactions = 0;
    CGReputationInfo::m_numShownFactions = 0;
    CGReputationInfo::m_numHeaders = 0;
    CGReputationInfo::m_selectedFaction = 0;
    ClientServices::ClearMessageHandler(SMSG_INITIALIZE_FACTIONS);
    ClientServices::ClearMessageHandler(SMSG_SET_FACTION_AT_WAR);
    ClientServices::ClearMessageHandler(SMSG_SET_FACTION_VISIBLE);
    ClientServices::ClearMessageHandler(SMSG_SET_FACTION_NOT_VISIBLE);
    ClientServices::ClearMessageHandler(SMSG_SET_FACTION_STANDING);
    ClientServices::ClearMessageHandler(SMSG_SET_FORCED_REACTIONS);

    for (uint32_t i = 0; i < CGReputationInfo::m_factionList.m_count; i++) {
        if (CGReputationInfo::m_factionList.m_data[i]) {
            SMemFree(CGReputationInfo::m_factionList.m_data[i], "delete", -1, 0);
        }

        CGReputationInfo::m_factionList.m_data[i] = NULL;
    }

    for (uint32_t i = 0; i < 128; i++) {
        if (CGReputationInfo::m_headers[i]) {
            SMemFree(CGReputationInfo::m_headers[i], "delete", -1, 0);
        }

        CGReputationInfo::m_headers[i] = NULL;
    }

    CGReputationInfo::m_forcedReactions.m_count = 0;
    memset(CGReputationInfo::m_factionStandings, 0, sizeof(CGReputationInfo::m_factionStandings));
}

// OFFSET: 0x009A72D0
int32_t Packet_SMSG_SET_FORCED_REACTIONS(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    uint32_t count;
    msg->Get(count);
    CGReputationInfo::m_forcedReactions.SetCount(count);

    for (uint32_t i = 0; i < count; i++) {
        msg->Get(reinterpret_cast<uint32_t&>(CGReputationInfo::m_forcedReactions.m_data[i].m_factionId));
        msg->Get(reinterpret_cast<uint32_t&>(CGReputationInfo::m_forcedReactions.m_data[i].m_reaction));
    }

    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player) {
        player->UpdateReaction(false);
    }

    return 1;
}

// OFFSET: 0x009A73A0
uint32_t CGReputationInfo::AddFactionHeader(int32_t factionId) {
    FactionRec* faction = g_factionDB.GetRecord(factionId);
    int32_t parent;

    if (faction) {
        parent = faction->m_parentFactionID;

        if (parent) {
            CGReputationInfo::AddFactionHeader(parent);
        }
    } else {
        parent = 0;
    }

    uint32_t i;

    for (i = 0; i < CGReputationInfo::m_numFactions; i++) {
        FactionListEntry* entry = CGReputationInfo::m_factionList.m_data[i];

        if (entry->m_isHeader && entry->m_factionId == factionId) {
            break;
        }
    }

    if (i != CGReputationInfo::m_numFactions) {
        return i;
    }

    CGReputationInfo::m_factionList.m_data[CGReputationInfo::m_numFactions + 1]->m_factionId = factionId;
    FactionRec* rec = g_factionDB.GetRecord(factionId);
    CGReputationInfo::m_factionList.m_data[CGReputationInfo::m_numFactions + 1]->m_repListId = rec ? rec->m_reputationIndex : -1;
    CGReputationInfo::m_factionList.m_data[CGReputationInfo::m_numFactions + 1]->m_isHeader = 1;
    CGReputationInfo::m_factionList.m_data[++CGReputationInfo::m_numFactions]->m_parent = factionId;
    return CGReputationInfo::AddHeader(factionId, parent);
}

// OFFSET: 0x009A74A0
int32_t CGReputationInfo::AddVisibleFaction(uint32_t repListId, int32_t announce) {
    for (uint32_t i = 0; i < CGReputationInfo::m_numFactions; i++) {
        if (static_cast<uint32_t>(CGReputationInfo::m_factionList.m_data[i]->m_repListId) == repListId) {
            return 0;
        }
    }

    FactionListEntry* entry = CGReputationInfo::m_factionList.m_data[CGReputationInfo::m_numFactions];
    entry->m_repListId = repListId;
    entry->m_factionId = repListId > 255 ? 0 : CGReputationInfo::m_factionStandings[repListId].m_factionId;
    entry->m_isHeader = (CGReputationInfo::m_factionStandings[repListId].m_flags >> 3) & 1;

    FactionRec* faction = g_factionDB.GetRecord(entry->m_factionId);
    int32_t parent = faction ? faction->m_parentFactionID : 0;

    if (entry->m_isHeader) {
        CGReputationInfo::AddHeader(entry->m_factionId, parent);
        CGReputationInfo::m_factionList.m_data[CGReputationInfo::m_numFactions]->m_parent = CGReputationInfo::m_factionList.m_data[CGReputationInfo::m_numFactions]->m_factionId;
    } else {
        entry->m_parent = parent;
    }

    if (!CGReputationInfo::m_factionList.m_data[CGReputationInfo::m_numFactions]->m_isHeader || parent >= 0) {
        CGReputationInfo::AddFactionHeader(parent);
    }

    if (faction && announce) {
        int32_t id = faction->m_id;
        CGChat::LogReactionChanged(id, CGReputationInfo::GetFactionStandingReaction(id));
    }

    CGReputationInfo::m_numFactions++;
    return 1;
}

// OFFSET: 0x009A75E0
static bool FactionSortIsHidden(const FactionHeader* header) {
    FactionRec* faction = g_factionDB.GetRecord(header->m_factionId);

    if (!faction) {
        return header->m_count == 0;
    }

    return !FactionHasReputationForPlayer(faction) && !header->m_count;
}

// OFFSET: 0x009A7640
static int QSortFactionHeaders(const void* a, const void* b) {
    FactionHeader* left = *static_cast<FactionHeader* const*>(a);
    FactionHeader* right = *static_cast<FactionHeader* const*>(b);
    bool leftHidden = FactionSortIsHidden(left);
    bool rightHidden = FactionSortIsHidden(right);

    if (leftHidden) {
        return !rightHidden;
    }

    if (rightHidden) {
        return -1;
    }

    int32_t leftParent = left->m_parent;

    if (leftParent && right->m_factionId && leftParent == right->m_factionId) {
        return 1;
    }

    int32_t rightParent = right->m_parent;

    if (rightParent && left->m_factionId && rightParent == left->m_factionId) {
        return -1;
    }

    if (leftParent != rightParent) {
        while (leftParent) {
            int32_t index = GetHeaderIndex(leftParent);
            left = index >= 0 && index <= 128 ? CGReputationInfo::m_headers[index] : NULL;
            leftParent = left->m_parent;
        }

        while (rightParent) {
            int32_t index = GetHeaderIndex(rightParent);
            right = index >= 0 && index <= 128 ? CGReputationInfo::m_headers[index] : NULL;
            rightParent = right->m_parent;
        }
    }

    FactionRec* leftRec = g_factionDB.GetRecord(left->m_factionId);
    FactionRec* rightRec = g_factionDB.GetRecord(right->m_factionId);

    if (!leftRec) {
        if (!rightRec) {
            return 2 * (left->m_factionId < right->m_factionId) - 1;
        }

        return 2 * (leftRec == NULL) - 1;
    }

    if (!rightRec) {
        return 2 * (leftRec == NULL) - 1;
    }

    if (leftRec->m_id == 1169) {
        return -1;
    }

    if (rightRec->m_id == 1169) {
        return 1;
    }

    if (leftRec->m_sortOrder != rightRec->m_sortOrder) {
        return rightRec->m_sortOrder - leftRec->m_sortOrder;
    }

    return SStrSortUTF8I(leftRec->m_name, rightRec->m_name, STORM_MAX_STR);
}

// OFFSET: 0x009A77E0/0x14072E2F0
void CGReputationInfo::SortFactions() {
    for (uint32_t h = 0; h < CGReputationInfo::m_numHeaders; h++) {
        FactionHeader* header = CGReputationInfo::m_headers[h];
        header->m_count = 0;

        for (uint32_t i = 0; i < CGReputationInfo::m_numFactions; i++) {
            FactionListEntry* entry = CGReputationInfo::m_factionList.m_data[i];
            FactionRec* faction = g_factionDB.GetRecord(entry->m_factionId);
            int32_t parent = faction && static_cast<uint32_t>(faction->m_reputationIndex) <= 255 && (CGReputationInfo::m_factionStandings[faction->m_reputationIndex].m_flags & 0x20) ? -1 : entry->m_parent;

            if (parent == header->m_factionId && !entry->m_isHeader) {
                header->m_count++;
            }
        }
    }

    for (uint32_t i = 0; i < CGReputationInfo::m_numHeaders; i++) {
        for (uint32_t j = 1; j < CGReputationInfo::m_numHeaders; j++) {
            if (i == j) {
                continue;
            }

            FactionHeader* child = CGReputationInfo::m_headers[j];

            if (child->m_parent && CGReputationInfo::m_headers[i]->m_factionId == child->m_parent) {
                CGReputationInfo::m_headers[i]->m_count += child->m_count;
                FactionRec* faction = g_factionDB.GetRecord(CGReputationInfo::m_headers[j]->m_factionId);

                if (FactionHasReputationForPlayer(faction) && CGReputationInfo::m_headers[j]->m_parent != -1) {
                    CGReputationInfo::m_headers[i]->m_count++;
                }
            }
        }
    }

    uint32_t numHeaders = CGReputationInfo::m_numHeaders;
    int32_t* expanded = static_cast<int32_t*>(alloca(sizeof(int32_t) * numHeaders));

    for (uint32_t i = 0; i < CGReputationInfo::m_numHeaders; i++) {
        expanded[i] = CGReputationInfo::m_headers[i]->m_factionId;

        if (i >= 128 || (CGReputationInfo::m_collapsedHeaders & (1 << i))) {
            expanded[i] = -2;
        }
    }

    qsort(CGReputationInfo::m_headers, CGReputationInfo::m_numHeaders, sizeof(CGReputationInfo::m_headers[0]), QSortFactionHeaders);
    uint32_t sortedHeaders = CGReputationInfo::m_numHeaders;
    CGReputationInfo::m_collapsedHeaders = 0xFFFFFFFF;

    for (uint32_t i = 0; i < numHeaders; i++) {
        if (expanded[i] == -2) {
            continue;
        }

        for (uint32_t k = 0; k < CGReputationInfo::m_numHeaders; k++) {
            if (expanded[i] == CGReputationInfo::m_headers[k]->m_factionId) {
                CGReputationInfo::m_collapsedHeaders &= ~(1 << k);
            }
        }
    }

    CGReputationInfo::m_numShownFactions = CGReputationInfo::m_numFactions;

    for (uint32_t i = 0; i < CGReputationInfo::m_numFactions; i++) {
        uint32_t h;

        for (h = 0; h < sortedHeaders; h++) {
            int32_t parent = CGReputationInfo::IsFactionInactive(i) ? -1 : CGReputationInfo::m_factionList.m_data[i]->m_parent;

            if (CGReputationInfo::m_headers[h]->m_factionId == parent) {
                break;
            }
        }

        if (h < sortedHeaders) {
            FactionListEntry* entry = CGReputationInfo::m_factionList.m_data[i];
            entry->m_headerIndex = h;

            if (!entry->m_isHeader && ((h < 128 && !(CGReputationInfo::m_collapsedHeaders & (1 << h))) || CGReputationInfo::m_headers[h]->m_collapsed)) {
                CGReputationInfo::m_numShownFactions--;
            }

            if (CGReputationInfo::m_factionList.m_data[i]->m_isHeader) {
                FactionHeader* header = CGReputationInfo::m_headers[h];
                FactionRec* faction = g_factionDB.GetRecord(header->m_factionId);

                if ((faction ? !FactionHasReputationForPlayer(faction) && !header->m_count : !header->m_count) || CGReputationInfo::m_headers[h]->m_collapsed) {
                    CGReputationInfo::m_numShownFactions--;
                }
            }
        }

        sortedHeaders = CGReputationInfo::m_numHeaders;
    }

    qsort(CGReputationInfo::m_factionList.m_data, CGReputationInfo::m_numFactions, sizeof(CGReputationInfo::m_factionList.m_data[0]), QSortFactions);
    FrameScript_SignalEvent(SCRIPT_EVENT_UPDATE_FACTION, NULL);
}

// OFFSET: 0x009A7B70/0x14072E730
void CGReputationInfo::SetFactionInactive(int32_t faction, bool inactive) {
    uint32_t header;

    for (header = 0; header < CGReputationInfo::m_numHeaders; header++) {
        if (CGReputationInfo::m_headers[header]->m_factionId == faction) {
            break;
        }
    }

    if (header < CGReputationInfo::m_numHeaders && static_cast<int32_t>(header) > 0) {
        return;
    }

    FactionRec* rec = g_factionDB.GetRecord(faction);

    if (!rec) {
        return;
    }

    uint32_t index = rec->m_reputationIndex;

    if (index > 255) {
        return;
    }

    if (inactive) {
        CGReputationInfo::m_factionStandings[index].m_flags |= 0x20;
    } else {
        CGReputationInfo::m_factionStandings[index].m_flags &= ~0x20;
    }

    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_SET_FACTION_INACTIVE));
    msg.Put(index);
    msg.Put(static_cast<uint8_t>(inactive != 0));
    msg.Finalize();
    ClientServices::Send2(&msg);
    CGReputationInfo::SortFactions();
}

// OFFSET: 0x009A7C90/0x14072E880
void CGReputationInfo::CollapseHeader(uint32_t index, int32_t collapse) {
    if (index >= CGReputationInfo::m_numFactions || !CGReputationInfo::m_factionList.m_data[index]->m_isHeader) {
        CGReputationInfo::m_collapsedHeaders = (collapse != 0) - 1;
        CGReputationInfo::SortFactions();
        SaveCollapsedHeaders();
        return;
    }

    FactionListEntry* entry = CGReputationInfo::m_factionList.m_data[index];
    uint32_t h;

    for (h = 0; h < CGReputationInfo::m_numHeaders; h++) {
        if (CGReputationInfo::m_headers[h]->m_factionId == entry->m_parent) {
            break;
        }
    }

    if (h == CGReputationInfo::m_numHeaders) {
        return;
    }

    if (collapse) {
        CGReputationInfo::m_collapsedHeaders &= ~(1 << h);

        for (uint32_t i = 0; i < CGReputationInfo::m_numHeaders; i++) {
            if (IsHeaderDescendant(i, CGReputationInfo::m_headers[h]->m_factionId)) {
                CGReputationInfo::m_headers[i]->m_collapsed = 1;
            }
        }
    } else {
        CGReputationInfo::m_collapsedHeaders |= 1 << h;

        for (uint32_t i = 0; i < CGReputationInfo::m_numHeaders; i++) {
            if (IsHeaderDescendant(i, CGReputationInfo::m_headers[h]->m_factionId)) {
                CGReputationInfo::m_headers[i]->m_collapsed = 0;
            }
        }
    }

    CGReputationInfo::SortFactions();
    SaveCollapsedHeaders();
}

// OFFSET: 0x009A7F90
int32_t Packet_SMSG_SET_FACTION_STANDING(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    bool sort = false;
    bool updateQuests = false;
    bool updateReaction = false;
    bool newlyVisible = false;
    bool improved = false;
    float bonus;
    uint8_t showEffect;
    uint32_t count;
    msg->Get(bonus);
    msg->Get(showEffect);
    msg->Get(count);

    for (uint32_t i = 0; i < count; i++) {
        uint32_t repListId;
        int32_t standing;
        msg->Get(repListId);
        msg->Get(reinterpret_cast<uint32_t&>(standing));
        uint8_t flags = CGReputationInfo::m_factionStandings[repListId].m_flags;
        int32_t logMode = 0;

        if (flags & 0x40) {
            logMode = (flags & 0x4) ? 2 : 1;
        } else if (i == 0 && !(flags & 0x4)) {
            logMode = 1;
        }

        int32_t factionId = repListId <= 255 ? CGReputationInfo::m_factionStandings[repListId].m_factionId : 0;
        int32_t oldReaction = CGReputationInfo::GetFactionStandingReaction(factionId);
        int32_t oldStanding = CGReputationInfo::m_factionStandings[repListId].m_standing;

        if (standing != oldStanding) {
            CGReputationInfo::m_factionStandings[repListId].m_standing = standing;

            if (logMode) {
                CGChat::LogFactionChanged(repListId <= 255 ? CGReputationInfo::m_factionStandings[repListId].m_factionId : 0, standing - oldStanding, logMode == 2, bonus);
            }
        }

        int32_t newReaction = CGReputationInfo::GetFactionStandingReaction(factionId);

        if (!(flags & 0x5)) {
            flags |= 0x1;

            if (CGReputationInfo::AddVisibleFaction(repListId, 1)) {
                newlyVisible = true;
            }
        }

        if (newReaction < 2) {
            flags |= 0x2;
        } else if (newReaction > oldReaction) {
            flags &= ~0x2;
        }

        uint8_t& storedFlags = CGReputationInfo::m_factionStandings[repListId].m_flags;

        if (flags != storedFlags) {
            if ((flags ^ storedFlags) & 0x2) {
                updateReaction = true;
            }

            storedFlags = flags;
            sort = true;
        }

        if (newReaction != oldReaction) {
            if (!(flags & 0x4) && (!(flags & 0x8) || (flags & 0x80))) {
                if (!newlyVisible) {
                    CGChat::LogReactionChanged(factionId, newReaction);
                }

                if (newReaction > oldReaction) {
                    improved = true;
                }
            }

            updateQuests = true;
            updateReaction = true;
        }
    }

    if (sort) {
        CGReputationInfo::SortFactions();
    }

    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (updateQuests) {
        CGQuestLog::Update(0);
    }

    if (updateReaction && player) {
        player->UpdateReaction(false);
    }

    if (!improved || !showEffect || player) {
        if (improved && showEffect) {
            CEffect* effect = CEffect::Create();
            WOWGUID guid = player->m_obj->m_guid;
            effect->AddSpecialEffect(9, guid, CGObject_C::EffectOnceHandler);
            effect->m_flags |= 0x20;
            effect->Release();
        }

        if (player && static_cast<int32_t>(player->m_unit->m_unitFieldLevel) >= 10) {
            CGTutorial::TriggerTutorial(15);
        }
    }

    FrameScript_SignalEvent(SCRIPT_EVENT_UPDATE_FACTION, NULL);
    return 1;
}

// OFFSET: 0x009A8350
TSGrowableArray<const FactionRec*>* CGReputationInfo::GetFactionChildren(int32_t faction) {
    FACTION* node = CGReputationInfo::m_factions.Ptr(faction, HASHKEY_NONE());
    return node ? &node->m_children : NULL;
}

// OFFSET: 0x009A84F0
void CGReputationInfo::LeaveWorld() {
    CGReputationInfo::m_forcedReactions.m_count = 0;
    CGReputationInfo::m_factions.InternalClear(0);
}

// OFFSET: 0x009A8BD0/0x14072FB30
void CGReputationInfo::EnterWorld() {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return;
    }

    CGReputationInfo::m_factions.InternalClear(0);
    uint8_t race = player->m_unit->m_unitFieldBytes0[0];
    uint8_t classId = player->m_unit->m_unitFieldBytes0[1];
    CGReputationInfo::m_collapsedHeaders = 0xFFFFFFFF;
    CGReputationInfo::m_numFactions = 0;
    CGReputationInfo::m_numShownFactions = 0;
    CGReputationInfo::m_numHeaders = 0;

    for (int32_t i = g_factionDB.m_numRecords; i;) {
        i--;
        FactionRec* faction = g_factionDB.GetRecordByIndex(i);

        if (static_cast<uint32_t>(faction->m_reputationIndex) > 255) {
            continue;
        }

        if (faction->m_parentFactionID) {
            FACTION* node = CGReputationInfo::m_factions.Ptr(faction->m_parentFactionID, HASHKEY_NONE());

            if (!node) {
                node = CGReputationInfo::m_factions.New(faction->m_parentFactionID, HASHKEY_NONE(), 0, 0);
            }

            const FactionRec* child = faction;
            node->m_children.New(child);
        }

        CGReputationInfo::m_factionStandings[faction->m_reputationIndex].m_factionId = faction->m_id;

        for (uint32_t k = 0; k < 4; k++) {
            int32_t raceMask = faction->m_reputationRaceMask[k];
            int32_t classMask = faction->m_reputationClassMask[k];

            if (raceMask) {
                if (!((1 << (race - 1)) & raceMask)) {
                    continue;
                }

                if (classMask && !((1 << (classId - 1)) & classMask)) {
                    continue;
                }
            } else if (!classMask || !((1 << (classId - 1)) & classMask)) {
                continue;
            }

            CGReputationInfo::m_factionStandings[faction->m_reputationIndex].m_base = faction->m_reputationBase[k];

            if (CGReputationInfo::m_factionStandings[faction->m_reputationIndex].m_flags & 0x1) {
                CGReputationInfo::AddVisibleFaction(faction->m_reputationIndex, 0);
            }
        }
    }

    CGReputationInfo::AddHeader(-1, 0);
    CGReputationInfo::m_factionList.m_data[CGReputationInfo::m_numFactions]->m_repListId = -1;
    CGReputationInfo::m_factionList.m_data[CGReputationInfo::m_numFactions]->m_factionId = -1;
    CGReputationInfo::m_factionList.m_data[CGReputationInfo::m_numFactions]->m_isHeader = 1;
    CGReputationInfo::m_factionList.m_data[CGReputationInfo::m_numFactions++]->m_parent = -1;
    CGReputationInfo::CollapseHeader(CGReputationInfo::m_numFactions - 1, 1);
    CGReputationInfo::SortFactions();
}

// OFFSET: 0x009A8E50
int32_t Packet_SMSG_INITIALIZE_FACTIONS(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    int32_t count;
    msg->Get(reinterpret_cast<uint32_t&>(count));

    for (int32_t i = 0; i < count; i++) {
        uint8_t flags;
        msg->Get(flags);
        CGReputationInfo::m_factionStandings[i].m_flags = flags;

        int32_t standing;
        msg->Get(reinterpret_cast<uint32_t&>(standing));

        if (standing != CGReputationInfo::m_factionStandings[i].m_standing) {
            CGReputationInfo::m_factionStandings[i].m_standing = standing;
        }
    }

    CGReputationInfo::EnterWorld();
    FrameScript_SignalEvent(SCRIPT_EVENT_UPDATE_FACTION, NULL);
    return 1;
}

// OFFSET: 0x009A8ED0
int32_t Packet_SMSG_SET_FACTION_VISIBLE(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    uint32_t repListId;
    msg->Get(repListId);

    if (msgId == SMSG_SET_FACTION_VISIBLE) {
        uint8_t flags = CGReputationInfo::m_factionStandings[repListId].m_flags;

        if (!(flags & 0x1)) {
            CGReputationInfo::AddVisibleFaction(repListId, 1);
            CGReputationInfo::m_factionStandings[repListId].m_flags = flags | 0x1;
            CGReputationInfo::SortFactions();
        }

        return 1;
    }

    if (msgId != SMSG_SET_FACTION_NOT_VISIBLE) {
        return 1;
    }

    uint8_t flags = CGReputationInfo::m_factionStandings[repListId].m_flags;

    if (!(flags & 0x1)) {
        return 1;
    }

    CGReputationInfo::m_factionStandings[repListId].m_flags = flags & ~0x1;
    CGReputationInfo::m_forcedReactions.m_count = 0;
    CGReputationInfo::m_factions.InternalClear(0);
    CGReputationInfo::EnterWorld();
    CGReputationInfo::SortFactions();
    return 1;
}

// OFFSET: 0x009A8FA0/0x1407300A0
void CGReputationInfo::InitializeGame() {
    ClientServices::SetMessageHandler(SMSG_INITIALIZE_FACTIONS, Packet_SMSG_INITIALIZE_FACTIONS, NULL);
    ClientServices::SetMessageHandler(SMSG_SET_FACTION_AT_WAR, Packet_SMSG_SET_FACTION_ATWAR, NULL);
    ClientServices::SetMessageHandler(SMSG_SET_FACTION_VISIBLE, Packet_SMSG_SET_FACTION_VISIBLE, NULL);
    ClientServices::SetMessageHandler(SMSG_SET_FACTION_NOT_VISIBLE, Packet_SMSG_SET_FACTION_VISIBLE, NULL);
    ClientServices::SetMessageHandler(SMSG_SET_FACTION_STANDING, Packet_SMSG_SET_FACTION_STANDING, NULL);
    ClientServices::SetMessageHandler(SMSG_SET_FORCED_REACTIONS, Packet_SMSG_SET_FORCED_REACTIONS, NULL);
    CGReputationInfo::m_factionList.SetCount(385);

    for (uint32_t i = 0; i < CGReputationInfo::m_factionList.m_count; i++) {
        CGReputationInfo::m_factionList.m_data[i] = static_cast<FactionListEntry*>(SMemAlloc(sizeof(FactionListEntry), __FILE__, __LINE__, 0));
    }

    for (uint32_t i = 0; i < 128; i++) {
        CGReputationInfo::m_headers[i] = static_cast<FactionHeader*>(SMemAlloc(sizeof(FactionHeader), __FILE__, __LINE__, 0));
    }
}

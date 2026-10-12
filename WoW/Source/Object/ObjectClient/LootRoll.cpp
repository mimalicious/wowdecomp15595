#include "LootRoll.h"
#include <cstring>
#include "Engine/Source/Base/CDataStore.h"
#include "Engine/Source/ConsoleBase/ConsoleVar.h"
#include "Engine/Source/Event/FrameTime.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/Storm/H/storm.h"
#include "WoW/Source/DB/DBClient/DBCacheInstances.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/ItemRecSparse.h"
#include "WoW/Source/Game/GameClient/NameCache.h"
#include "WoW/Source/Net/Types.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/UIUtil/Tooltip.h"
#include "WoW/Source/Ui/ChatFrame.h"
#include "WoW/Source/Ui/GameUI.h"
#include "WoW/Source/Ui/LootFrame.h"
#include "WoW/Source/Ui/RaidInfo.h"
#include "WoW/Source/Ui/Types.h"
#include "WoW/Source/WowServices/WDataStore.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"

// STATIC: 0x00CA57BC/0x140B440B0
TSExplicitList<PendingLootRoll, offsetof(PendingLootRoll, m_link)> s_pendingLootRolls;

// STATIC: 0x00DD5268/0x140CBB4F0
WOWGUID s_masterLootCandidates[40];

// STATIC: 0x00DD53A8/0x140CBB630
static int32_t s_nextLootRollId;

// STATIC: 0x00DD53AC/0x140CBB634
uint32_t s_numShownLootRolls;

// OFFSET: 0x0057B340
static void LootRollShow(PendingLootRoll* roll) {
    if (s_numShownLootRolls >= 4) {
        roll->m_showPending = 1;
        return;
    }

    s_numShownLootRolls++;
    roll->m_shown = 1;
    FrameScript_SignalEvent(SCRIPT_EVENT_START_LOOT_ROLL, "%d%d", roll->m_id, roll->m_rollTime);
}

// OFFSET: 0x0057B380/0x1401E0EA0
int32_t LootRollMasterList(CDataStore* msg) {
    memset(s_masterLootCandidates, 0, sizeof(s_masterLootCandidates));
    uint8_t count;
    msg->Get(count);
    bool raid = CGRaidInfo::m_numMembers != 0;

    for (uint32_t i = 0; i < count; i++) {
        WOWGUID guid;
        msg->Get(guid.m_guid);

        if (!raid) {
            s_masterLootCandidates[i] = guid;
            continue;
        }

        CGRaidMember* member = CGRaidInfo::FindMemberByGuid(guid);

        if (!member) {
            continue;
        }

        uint32_t first = 5 * member->m_subgroup;
        uint32_t last = first + 5;

        for (uint32_t j = first; j < last && j < 40; j++) {
            if (!s_masterLootCandidates[j]) {
                s_masterLootCandidates[j] = guid;
                break;
            }
        }
    }

    return 1;
}

// OFFSET: 0x0057B460/0x1401E0FB0
WOWGUID GetMasterLootCandidate(uint32_t index) {
    if (index < 40) {
        return s_masterLootCandidates[index];
    }

    return WOWGUID();
}

// OFFSET: 0x0057B490/0x1401E0FD0
static void LootRollCalculateReasons(PendingLootRoll* roll, const ItemRecSparse* rec) {
    roll->m_reasonPass = 0;
    bool invalid = false;

    if (CGPlayer_C* player = ClntObjMgrGetActivePlayerObj()) {
        invalid = !player->m_bag.ItemCountValid(roll->m_itemId, rec, roll->m_count, false);
    }

    uint8_t mask = roll->m_voteMask;

    if (mask & 0x2) {
        roll->m_reasonNeed = 0;
    } else if (invalid) {
        roll->m_reasonNeed = 2;
    } else {
        roll->m_reasonNeed = static_cast<uint8_t>(((rec->m_flags[1] >> 6) & 0x4) | 0x1);
    }

    roll->m_reasonGreed = ~(mask >> 1) & 0x2;

    if (mask & 0x8) {
        roll->m_reasonDisenchant = 0;
    } else {
        roll->m_reasonDisenchant = (rec->GetDisenchantSkillRequired() >= 0) + 3;
    }
}

// OFFSET: 0x0057B530
static PendingLootRoll* FindRollBySlot(WOWGUID lootGuid, uint32_t slot) {
    for (PendingLootRoll* roll = s_pendingLootRolls.Head(); roll; roll = roll->Next()) {
        if (roll->m_lootGuid == lootGuid && roll->m_slot == slot) {
            return roll;
        }
    }

    return NULL;
}

// OFFSET: 0x0057B580/0x1401E1090
PendingLootRoll* FindRoll(int32_t id) {
    for (PendingLootRoll* roll = s_pendingLootRolls.Head(); roll; roll = roll->Next()) {
        if (roll->m_id == id) {
            return roll;
        }
    }

    return NULL;
}

// OFFSET: 0x0057B5B0/0x1401E10C0
void LootRollShowPending() {
    if (s_numShownLootRolls >= 4) {
        return;
    }

    for (PendingLootRoll* roll = s_pendingLootRolls.Head(); roll; ) {
        PendingLootRoll* next = roll->Next();

        if (roll->m_showPending) {
            s_numShownLootRolls++;
            roll->m_showPending = 0;
            roll->m_shown = 1;
            FrameScript_SignalEvent(SCRIPT_EVENT_START_LOOT_ROLL, "%d%d", roll->m_id, roll->m_rollTime);

            if (s_numShownLootRolls >= 4) {
                break;
            }
        }

        roll = next;
    }
}

// PROVEN: 840baf62 IDENTICAL/DIFFERENT
// OFFSET: 0x0057B620/0x1401E1140
static void AllPassedStats(const ItemRecSparse* rec, PendingLootRoll* roll) {
    const char* link = CGTooltip::GetItemHyperlinkString(roll->m_itemId, rec->m_quality, 0, roll->m_randomPropertyId, roll->m_randomSuffix, 0);
    char text[3000];
    SStrPrintf(text, sizeof(text), FrameScript_GetText("LOOT_ROLL_ALL_PASSED", -1, GENDER_NOT_APPLICABLE), link);
    CGChat::AddChatMessage(text, CHAT_MSG_LOOT, NULL);

    if (!roll->m_rolled) {
        FrameScript_SignalEvent(SCRIPT_EVENT_CANCEL_LOOT_ROLL, "%d", roll->m_id);

        if (roll->m_shown) {
            s_numShownLootRolls--;
        }
    }

    delete roll;
    LootRollShowPending();
}

// OFFSET: 0x0057B700/0x1401E1250
static void WonStatsAndName(const NameCache* name, const ItemRecSparse* rec, PendingLootRoll* roll) {
    char text[3000];

    if (roll->m_rollerGuid == ClntObjMgrGetActivePlayer()) {
        if (g_showLootSpamCVar->m_intValue) {
            SStrPrintf(text, sizeof(text), FrameScript_GetText("LOOT_ROLL_YOU_WON", -1, GENDER_NOT_APPLICABLE), CGTooltip::GetItemHyperlinkString(roll->m_itemId, rec->m_quality, 0, roll->m_randomPropertyId, roll->m_randomSuffix, 0));
        } else if (roll->m_rollType == 1) {
            const char* link = CGTooltip::GetItemHyperlinkString(roll->m_itemId, rec->m_quality, 0, roll->m_randomPropertyId, roll->m_randomSuffix, 0);
            SStrPrintf(text, sizeof(text), FrameScript_GetText("LOOT_ROLL_YOU_WON_NO_SPAM_NEED", -1, GENDER_NOT_APPLICABLE), roll->m_rollNumber, link);
        } else if (roll->m_rollType == 2) {
            const char* link = CGTooltip::GetItemHyperlinkString(roll->m_itemId, rec->m_quality, 0, roll->m_randomPropertyId, roll->m_randomSuffix, 0);
            SStrPrintf(text, sizeof(text), FrameScript_GetText("LOOT_ROLL_YOU_WON_NO_SPAM_GREED", -1, GENDER_NOT_APPLICABLE), roll->m_rollNumber, link);
        } else if (roll->m_rollType == 3) {
            const char* link = CGTooltip::GetItemHyperlinkString(roll->m_itemId, rec->m_quality, 0, roll->m_randomPropertyId, roll->m_randomSuffix, 0);
            SStrPrintf(text, sizeof(text), FrameScript_GetText("LOOT_ROLL_YOU_WON_NO_SPAM_DE", -1, GENDER_NOT_APPLICABLE), roll->m_rollNumber, link);
        }
    } else if (g_showLootSpamCVar->m_intValue) {
        const char* link = CGTooltip::GetItemHyperlinkString(roll->m_itemId, rec->m_quality, 0, roll->m_randomPropertyId, roll->m_randomSuffix, 0);
        SStrPrintf(text, sizeof(text), FrameScript_GetText("LOOT_ROLL_WON", -1, GENDER_NOT_APPLICABLE), static_cast<const char*>(name->m_name), link);
    } else if (roll->m_rollType == 1) {
        const char* link = CGTooltip::GetItemHyperlinkString(roll->m_itemId, rec->m_quality, 0, roll->m_randomPropertyId, roll->m_randomSuffix, 0);
        SStrPrintf(text, sizeof(text), FrameScript_GetText("LOOT_ROLL_WON_NO_SPAM_NEED", -1, GENDER_NOT_APPLICABLE), static_cast<const char*>(name->m_name), roll->m_rollNumber, link);
    } else if (roll->m_rollType == 2) {
        const char* link = CGTooltip::GetItemHyperlinkString(roll->m_itemId, rec->m_quality, 0, roll->m_randomPropertyId, roll->m_randomSuffix, 0);
        SStrPrintf(text, sizeof(text), FrameScript_GetText("LOOT_ROLL_WON_NO_SPAM_GREED", -1, GENDER_NOT_APPLICABLE), static_cast<const char*>(name->m_name), roll->m_rollNumber, link);
    } else if (roll->m_rollType == 3) {
        const char* link = CGTooltip::GetItemHyperlinkString(roll->m_itemId, rec->m_quality, 0, roll->m_randomPropertyId, roll->m_randomSuffix, 0);
        SStrPrintf(text, sizeof(text), FrameScript_GetText("LOOT_ROLL_WON_NO_SPAM_DE", -1, GENDER_NOT_APPLICABLE), static_cast<const char*>(name->m_name), roll->m_rollNumber, link);
    }

    CGChat::AddChatMessage(text, CHAT_MSG_LOOT, NULL);

    if (!roll->m_rolled) {
        FrameScript_SignalEvent(SCRIPT_EVENT_CANCEL_LOOT_ROLL, "%d", roll->m_id);

        if (roll->m_shown) {
            s_numShownLootRolls--;
        }
    }

    delete roll;
    LootRollShowPending();
}

// OFFSET: 0x0057B9B0/0x1401E1540
static void RolledStatsAndName(const NameCache* name, const ItemRecSparse* rec, PendingLootRoll* roll) {
    if (!g_showLootSpamCVar->m_intValue && (roll->m_rollerGuid != ClntObjMgrGetActivePlayer() || !roll->m_autoPass)) {
        delete roll;
        return;
    }

    char text[3000];
    uint32_t rollType = roll->m_rollType;
    int32_t rollNumber = roll->m_rollNumber;

    if (roll->m_rollerGuid == ClntObjMgrGetActivePlayer()) {
        if (rollType == 2 && rollNumber < 0) {
            SStrPrintf(text, sizeof(text), FrameScript_GetText("LOOT_ROLL_GREED_SELF", -1, GENDER_NOT_APPLICABLE), CGTooltip::GetItemHyperlinkString(roll->m_itemId, rec->m_quality, 0, roll->m_randomPropertyId, roll->m_randomSuffix, 0));
        } else if (rollType == 3 && rollNumber < 0) {
            SStrPrintf(text, sizeof(text), FrameScript_GetText("LOOT_ROLL_DISENCHANT_SELF", -1, GENDER_NOT_APPLICABLE), CGTooltip::GetItemHyperlinkString(roll->m_itemId, rec->m_quality, 0, roll->m_randomPropertyId, roll->m_randomSuffix, 0));
        } else if (rollNumber < 0) {
            const char* format = roll->m_autoPass ? FrameScript_GetText("LOOT_ROLL_PASSED_SELF_AUTO", -1, GENDER_NOT_APPLICABLE) : FrameScript_GetText("LOOT_ROLL_PASSED_SELF", -1, GENDER_NOT_APPLICABLE);
            SStrPrintf(text, sizeof(text), format, CGTooltip::GetItemHyperlinkString(roll->m_itemId, rec->m_quality, 0, roll->m_randomPropertyId, roll->m_randomSuffix, 0));
        } else if (!rollType) {
            SStrPrintf(text, sizeof(text), FrameScript_GetText("LOOT_ROLL_NEED_SELF", -1, GENDER_NOT_APPLICABLE), CGTooltip::GetItemHyperlinkString(roll->m_itemId, rec->m_quality, 0, roll->m_randomPropertyId, roll->m_randomSuffix, 0));
        } else if (rollType == 2) {
            const char* link = CGTooltip::GetItemHyperlinkString(roll->m_itemId, rec->m_quality, 0, roll->m_randomPropertyId, roll->m_randomSuffix, 0);
            SStrPrintf(text, sizeof(text), FrameScript_GetText("LOOT_ROLL_ROLLED_GREED", -1, GENDER_NOT_APPLICABLE), roll->m_rollNumber, link, static_cast<const char*>(name->m_name));
        } else if (rollType == 3) {
            const char* link = CGTooltip::GetItemHyperlinkString(roll->m_itemId, rec->m_quality, 0, roll->m_randomPropertyId, roll->m_randomSuffix, 0);
            SStrPrintf(text, sizeof(text), FrameScript_GetText("LOOT_ROLL_ROLLED_DE", -1, GENDER_NOT_APPLICABLE), roll->m_rollNumber, link, static_cast<const char*>(name->m_name));
        } else if (rollType == 1 && rollNumber > 100) {
            const char* link = CGTooltip::GetItemHyperlinkString(roll->m_itemId, rec->m_quality, 0, roll->m_randomPropertyId, roll->m_randomSuffix, 0);
            SStrPrintf(text, sizeof(text), FrameScript_GetText("LOOT_ROLL_ROLLED_NEED_ROLE_BONUS", -1, GENDER_NOT_APPLICABLE), roll->m_rollNumber, link, static_cast<const char*>(name->m_name));
        } else {
            const char* link = CGTooltip::GetItemHyperlinkString(roll->m_itemId, rec->m_quality, 0, roll->m_randomPropertyId, roll->m_randomSuffix, 0);
            SStrPrintf(text, sizeof(text), FrameScript_GetText("LOOT_ROLL_ROLLED_NEED", -1, GENDER_NOT_APPLICABLE), roll->m_rollNumber, link, static_cast<const char*>(name->m_name));
        }
    } else if (rollType == 2 && rollNumber < 0) {
        const char* link = CGTooltip::GetItemHyperlinkString(roll->m_itemId, rec->m_quality, 0, roll->m_randomPropertyId, roll->m_randomSuffix, 0);
        SStrPrintf(text, sizeof(text), FrameScript_GetText("LOOT_ROLL_GREED", -1, GENDER_NOT_APPLICABLE), static_cast<const char*>(name->m_name), link);
    } else if (rollType == 3 && rollNumber < 0) {
        const char* link = CGTooltip::GetItemHyperlinkString(roll->m_itemId, rec->m_quality, 0, roll->m_randomPropertyId, roll->m_randomSuffix, 0);
        SStrPrintf(text, sizeof(text), FrameScript_GetText("LOOT_ROLL_DISENCHANT", -1, GENDER_NOT_APPLICABLE), static_cast<const char*>(name->m_name), link);
    } else if (rollNumber >= 0) {
        if (!rollType) {
            const char* link = CGTooltip::GetItemHyperlinkString(roll->m_itemId, rec->m_quality, 0, roll->m_randomPropertyId, roll->m_randomSuffix, 0);
            SStrPrintf(text, sizeof(text), FrameScript_GetText("LOOT_ROLL_NEED", -1, GENDER_NOT_APPLICABLE), static_cast<const char*>(name->m_name), link);
        } else if (rollType == 2) {
            const char* link = CGTooltip::GetItemHyperlinkString(roll->m_itemId, rec->m_quality, 0, roll->m_randomPropertyId, roll->m_randomSuffix, 0);
            SStrPrintf(text, sizeof(text), FrameScript_GetText("LOOT_ROLL_ROLLED_GREED", -1, GENDER_NOT_APPLICABLE), roll->m_rollNumber, link, static_cast<const char*>(name->m_name));
        } else if (rollType == 3) {
            const char* link = CGTooltip::GetItemHyperlinkString(roll->m_itemId, rec->m_quality, 0, roll->m_randomPropertyId, roll->m_randomSuffix, 0);
            SStrPrintf(text, sizeof(text), FrameScript_GetText("LOOT_ROLL_ROLLED_DE", -1, GENDER_NOT_APPLICABLE), roll->m_rollNumber, link, static_cast<const char*>(name->m_name));
        } else if (rollType == 1 && rollNumber > 100) {
            const char* link = CGTooltip::GetItemHyperlinkString(roll->m_itemId, rec->m_quality, 0, roll->m_randomPropertyId, roll->m_randomSuffix, 0);
            SStrPrintf(text, sizeof(text), FrameScript_GetText("LOOT_ROLL_ROLLED_NEED_ROLE_BONUS", -1, GENDER_NOT_APPLICABLE), roll->m_rollNumber, link, static_cast<const char*>(name->m_name));
        } else {
            const char* link = CGTooltip::GetItemHyperlinkString(roll->m_itemId, rec->m_quality, 0, roll->m_randomPropertyId, roll->m_randomSuffix, 0);
            SStrPrintf(text, sizeof(text), FrameScript_GetText("LOOT_ROLL_ROLLED_NEED", -1, GENDER_NOT_APPLICABLE), roll->m_rollNumber, link, static_cast<const char*>(name->m_name));
        }
    } else {
        const char* format = roll->m_autoPass ? FrameScript_GetText("LOOT_ROLL_PASSED_AUTO", -1, static_cast<FRAMESCRIPT_GENDER>((name->m_sex != 0) + 2)) : FrameScript_GetText("LOOT_ROLL_PASSED", -1, GENDER_NOT_APPLICABLE);
        SStrPrintf(text, sizeof(text), format, static_cast<const char*>(name->m_name), CGTooltip::GetItemHyperlinkString(roll->m_itemId, rec->m_quality, 0, roll->m_randomPropertyId, roll->m_randomSuffix, 0));
    }

    CGChat::AddChatMessage(text, CHAT_MSG_LOOT, NULL);
    delete roll;
}

// OFFSET: 0x0057BF50
void LootRollShutdown() {
    s_pendingLootRolls.Clear();
    s_numShownLootRolls = 0;
}

// OFFSET: 0x0057BF70/0x1401E1D20
void CloseAllLootRolls() {
    for (PendingLootRoll* roll = s_pendingLootRolls.Head(); roll; ) {
        PendingLootRoll* next = roll->Next();

        if (!roll->m_rolled) {
            WDataStore msg;
            msg.Put(static_cast<uint32_t>(CMSG_LOOT_ROLL));
            msg.Put(roll->m_lootGuid.m_guid);
            msg.Put(roll->m_slot);
            msg.Put(static_cast<uint8_t>(0));
            msg.Finalize();
            ClientServices::Send2(&msg);

            if (!roll->m_showPending) {
                FrameScript_SignalEvent(SCRIPT_EVENT_CANCEL_LOOT_ROLL, "%d", roll->m_id);
            }
        }

        roll = next;
    }

    s_pendingLootRolls.Clear();
    s_numShownLootRolls = 0;
}

// OFFSET: 0x0057C0A0/0x1401E1F10
static void StartCallback(int32_t id, const WOWGUID& guid, void* param, bool success) {
    PendingLootRoll* roll = static_cast<PendingLootRoll*>(param);

    if (success) {
        WOWGUID requester = WOWGUID();
        ItemRecSparse* rec = g_itemSparseDB.GetRecord(id, requester, NULL, NULL, false);
        LootRollCalculateReasons(roll, rec);
        LootRollShow(roll);
    } else if (roll) {
        delete roll;
    }
}

// OFFSET: 0x0057C180/0x1401E2000
int32_t LootRollStart(CDataStore* msg) {
    WOWGUID lootGuid;
    int32_t mapId;
    msg->Get(lootGuid.m_guid);
    msg->Get(reinterpret_cast<uint32_t&>(mapId));

    if (mapId != CGGameUI::m_iCurrentMapID) {
        msg->m_read = msg->m_size;
        return 1;
    }

    PendingLootRoll* roll = s_pendingLootRolls.NewNode(2, 0, 0);
    roll->m_lootGuid = lootGuid;
    roll->m_id = s_nextLootRollId++;
    roll->unk_0030 = 0;
    roll->m_showPending = 0;
    roll->m_rolled = 0;
    roll->m_shown = 0;
    roll->unk_005C = 3;
    msg->Get(roll->m_slot);
    msg->Get(roll->m_itemId);
    msg->Get(roll->m_randomSuffix);
    msg->Get(roll->m_randomPropertyId);
    msg->Get(roll->m_count);
    msg->Get(roll->m_rollTime);
    msg->Get(roll->m_voteMask);
    uint8_t value = 0;
    msg->Get(value);
    roll->unk_005C = value;
    roll->m_expireTime = roll->m_rollTime + FrameTime::GetCurTimeMs();
    roll->m_rollType = 0;

    if (ItemRecSparse* rec = g_itemSparseDB.GetRecord(roll->m_itemId, roll->m_lootGuid, &StartCallback, roll, false)) {
        LootRollCalculateReasons(roll, rec);
        LootRollShow(roll);
    }

    return 1;
}

// OFFSET: 0x0057C320/0x1401E2240
static void PassedCallback(int32_t id, const WOWGUID& guid, void* param, bool success) {
    PendingLootRoll* roll = static_cast<PendingLootRoll*>(param);

    if (success) {
        WOWGUID requester = WOWGUID();
        AllPassedStats(g_itemSparseDB.GetRecord(id, requester, NULL, NULL, false), roll);
    } else if (roll) {
        delete roll;
    }
}

// OFFSET: 0x0057C3C0/0x1401E22F0
int32_t LootRollAllPassed(CDataStore* msg) {
    WOWGUID lootGuid;
    uint32_t slot;
    uint32_t itemId;
    uint32_t randomSuffix;
    uint32_t randomPropertyId;
    msg->Get(lootGuid.m_guid);
    msg->Get(slot);
    msg->Get(itemId);
    msg->Get(randomSuffix);
    msg->Get(randomPropertyId);
    PendingLootRoll* roll = FindRollBySlot(lootGuid, slot);

    if (!roll) {
        roll = new (__FILE__, __LINE__) PendingLootRoll;
        roll->m_lootGuid = lootGuid;
        roll->m_slot = slot;
        roll->m_itemId = itemId;
        roll->m_id = -1;
        roll->m_randomPropertyId = randomPropertyId;
        roll->m_randomSuffix = randomSuffix;
        roll->m_rolled = 1;
        roll->m_rollType = 0;
    }

    if (ItemRecSparse* rec = g_itemSparseDB.GetRecord(itemId, lootGuid, &PassedCallback, roll, false)) {
        AllPassedStats(rec, roll);
    }

    CGLootInfo::ClearRollPending(lootGuid, slot);
    return 1;
}

static void WonCallback(int32_t id, const WOWGUID& guid, void* param, bool success);

// OFFSET: 0x0057C4C0
static void WonNameCallback(int32_t id, const WOWGUID& guid, void* param, bool success) {
    PendingLootRoll* roll = static_cast<PendingLootRoll*>(param);

    if (success) {
        WOWGUID requester = WOWGUID();
        NameCache* name = g_nameDBCache.GetRecord(guid.m_guid, requester, NULL, NULL, false);

        if (ItemRecSparse* rec = g_itemSparseDB.GetRecord(roll->m_itemId, guid, &WonCallback, roll, false)) {
            WonStatsAndName(name, rec, roll);
        }
    } else if (roll) {
        delete roll;
    }
}

// OFFSET: 0x0057C580
static void WonCallback(int32_t id, const WOWGUID& guid, void* param, bool success) {
    PendingLootRoll* roll = static_cast<PendingLootRoll*>(param);

    if (success) {
        WOWGUID requester = WOWGUID();
        ItemRecSparse* rec = g_itemSparseDB.GetRecord(id, requester, NULL, NULL, false);

        if (NameCache* name = g_nameDBCache.GetRecord(roll->m_rollerGuid.m_guid, roll->m_rollerGuid, &WonNameCallback, roll, false)) {
            WonStatsAndName(name, rec, roll);
        }
    } else if (roll) {
        delete roll;
    }
}

// OFFSET: 0x0057C630/0x1401E2620
int32_t LootRollWon(CDataStore* msg) {
    WOWGUID lootGuid;
    uint32_t slot;
    uint32_t itemId;
    uint32_t randomSuffix;
    uint32_t randomPropertyId;
    WOWGUID winner;
    uint32_t rollNumber;
    uint8_t rollType;
    msg->Get(lootGuid.m_guid);
    msg->Get(slot);
    msg->Get(itemId);
    msg->Get(randomSuffix);
    msg->Get(randomPropertyId);
    msg->Get(winner.m_guid);
    msg->Get(rollNumber);
    msg->Get(rollType);
    PendingLootRoll* roll = FindRollBySlot(lootGuid, slot);

    if (!roll) {
        roll = new (__FILE__, __LINE__) PendingLootRoll;
        roll->m_lootGuid = lootGuid;
        roll->m_slot = slot;
        roll->m_itemId = itemId;
        roll->m_id = -1;
        roll->m_randomPropertyId = randomPropertyId;
        roll->m_randomSuffix = randomSuffix;
        roll->m_rolled = 1;
        roll->m_rollType = 0;
    }

    roll->m_rollNumber = rollNumber;
    roll->m_rollType = rollType;
    roll->m_rollerGuid = winner;

    if (ItemRecSparse* rec = g_itemSparseDB.GetRecord(itemId, lootGuid, &WonCallback, roll, false)) {
        if (NameCache* name = g_nameDBCache.GetRecord(roll->m_rollerGuid.m_guid, roll->m_rollerGuid, &WonNameCallback, roll, false)) {
            WonStatsAndName(name, rec, roll);
        }
    }

    CGLootInfo::ClearRollPending(lootGuid, slot);
    return 1;
}

// OFFSET: 0x0057C790/0x1401E27E0
void LootRoll(int32_t id, int32_t rollType, bool confirmed) {
    for (PendingLootRoll* roll = s_pendingLootRolls.Head(); roll; roll = s_pendingLootRolls.Next(roll)) {
        if (roll->m_id != id) {
            continue;
        }

        if (roll->m_rolled) {
            return;
        }

        if (rollType && !confirmed) {
            WOWGUID requester = WOWGUID();

            if (ItemRecSparse* rec = g_itemSparseDB.GetRecord(roll->m_itemId, requester, NULL, NULL, false)) {
                if (rollType == 3) {
                    if (rec->m_quality >= 3) {
                        FrameScript_SignalEvent(SCRIPT_EVENT_CONFIRM_DISENCHANT_ROLL, "%d%d", roll->m_id, 3);
                        return;
                    }
                } else {
                    int32_t bonding = rec->m_bonding;

                    if (bonding == 1) {
                        FrameScript_SignalEvent(SCRIPT_EVENT_CONFIRM_LOOT_ROLL, "%d%d%s", roll->m_id, rollType, FrameScript_GetText("LOOT_NO_DROP", -1, GENDER_NOT_APPLICABLE));
                        return;
                    }

                    if (bonding == 2 && roll->unk_005C == 4 && rollType == 1) {
                        FrameScript_SignalEvent(SCRIPT_EVENT_CONFIRM_LOOT_ROLL, "%d%d%s", roll->m_id, rollType, FrameScript_GetText("LOOT_NO_DROP_ROLL_NEED", -1, GENDER_NOT_APPLICABLE));
                        return;
                    }
                }
            }
        }

        if (static_cast<uint8_t>(1 << rollType) & roll->m_voteMask) {
            WDataStore msg;
            msg.Put(static_cast<uint32_t>(CMSG_LOOT_ROLL));
            msg.Put(roll->m_lootGuid.m_guid);
            msg.Put(roll->m_slot);
            msg.Put(static_cast<uint8_t>(rollType));
            msg.Finalize();
            ClientServices::Send2(&msg);
            roll->unk_0030 = 1;
            FrameScript_SignalEvent(SCRIPT_EVENT_CANCEL_LOOT_ROLL, "%d", roll->m_id);
            bool shown = roll->m_shown != 0;
            roll->m_rolled = 1;

            if (shown) {
                s_numShownLootRolls--;
            }

            LootRollShowPending();
        }

        return;
    }
}

// OFFSET: 0x0057C990
static void RolledNameCallback(int32_t id, const WOWGUID& guid, void* param, bool success) {
    PendingLootRoll* roll = static_cast<PendingLootRoll*>(param);

    if (success) {
        WOWGUID requester = WOWGUID();
        NameCache* name = g_nameDBCache.GetRecord(guid.m_guid, requester, NULL, NULL, false);
        WOWGUID itemRequester = WOWGUID();
        ItemRecSparse* rec = g_itemSparseDB.GetRecord(roll->m_itemId, itemRequester, NULL, NULL, false);
        RolledStatsAndName(name, rec, roll);
    } else if (roll) {
        delete roll;
    }
}

// OFFSET: 0x0057CA40
static void RolledCallback(int32_t id, const WOWGUID& guid, void* param, bool success) {
    PendingLootRoll* roll = static_cast<PendingLootRoll*>(param);

    if (success) {
        WOWGUID requester = WOWGUID();
        ItemRecSparse* rec = g_itemSparseDB.GetRecord(id, requester, NULL, NULL, false);

        if (NameCache* name = g_nameDBCache.GetRecord(roll->m_rollerGuid.m_guid, roll->m_rollerGuid, &RolledNameCallback, roll, false)) {
            RolledStatsAndName(name, rec, roll);
        }
    } else if (roll) {
        delete roll;
    }
}

// OFFSET: 0x0057CB00/0x1401E2C60
int32_t LootRollRolled(CDataStore* msg) {
    PendingLootRoll* roll = new (__FILE__, __LINE__) PendingLootRoll;
    msg->Get(roll->m_lootGuid.m_guid);
    msg->Get(roll->m_slot);
    msg->Get(roll->m_rollerGuid.m_guid);
    msg->Get(roll->m_itemId);
    msg->Get(roll->m_randomSuffix);
    msg->Get(roll->m_randomPropertyId);
    uint32_t rollNumber = 0;
    msg->Get(rollNumber);
    roll->m_rollNumber = rollNumber;
    uint8_t value = 0;
    msg->Get(value);
    roll->m_rollType = value;
    value = 0;
    msg->Get(value);
    roll->m_autoPass = value != 0;

    if (ItemRecSparse* rec = g_itemSparseDB.GetRecord(roll->m_itemId, roll->m_lootGuid, &RolledCallback, roll, false)) {
        if (NameCache* name = g_nameDBCache.GetRecord(roll->m_rollerGuid.m_guid, roll->m_rollerGuid, &RolledNameCallback, roll, false)) {
            RolledStatsAndName(name, rec, roll);
        }
    }

    return 1;
}

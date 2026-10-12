#include "QuestFrame.h"
#include <cstdlib>
#include <cstring>
#include "Engine/Source/Base/CDataStore.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/Storm/H/storm.h"
#include "WoW/Source/DB/DBClient/DBCacheInstances.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/CurrencyTypesRec.h"
#include "WoW/Source/DB/Rec/FactionRec.h"
#include "WoW/Source/DB/Rec/ItemRec.h"
#include "WoW/Source/DB/Rec/ItemRecSparse.h"
#include "WoW/Source/DB/Rec/QuestFactionRewardRec.h"
#include "WoW/Source/Game/GameClient/NameCache.h"
#include "WoW/Source/Net/Types.h"
#include "WoW/Source/Object/ObjectClient/Item_C.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/Object/ObjectClient/Unit_C.h"
#include "WoW/Source/Object/Player.h"
#include "WoW/Source/Object/QuestCache.h"
#include "WoW/Source/Object/Unit.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/Ui/ChatFrame.h"
#include "WoW/Source/Ui/CurrencyTypes.h"
#include "WoW/Source/Ui/GameUI.h"
#include "WoW/Source/Ui/GossipInfo.h"
#include "WoW/Source/Ui/QuestLog.h"
#include "WoW/Source/Ui/ReputationInfo.h"
#include "WoW/Source/Ui/Tutorial.h"
#include "WoW/Source/Ui/Types.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"

// STATIC: 0x00F36DF0/0x140E2A3B8
QuestRequest CGQuestInfo::m_questRequest;

// STATIC: 0x00F36E58/0x140E2A400
QuestItemSlot CGQuestInfo::m_questItems[6];

// STATIC: 0x00F36F30/0x140E2A500
char CGQuestInfo::m_portraitTurnInName[256];

// STATIC: 0x00F37030/0x140E2A600
char CGQuestInfo::m_portraitTurnInText[1024];

// STATIC: 0x00F37430/0x140E2AA00
char CGQuestInfo::m_portraitGiverName[256];

// STATIC: 0x00F37530/0x140E2AB00
char CGQuestInfo::m_portraitGiverText[1024];

// STATIC: 0x00F37930/0x140E2A3D8
int32_t CGQuestInfo::m_rewardFactionAmounts[10];

// STATIC: 0x00F37958
int32_t CGQuestInfo::m_rewardFactionList[10];

// STATIC: 0x00F37980/0x140E2AF00
uint32_t CGQuestInfo::m_numRewardFactions;

// STATIC: 0x00F37988/0x140E2AF10
QuestGiverQuest CGQuestInfo::m_inProgressQuests[32];

// STATIC: 0x00F3BC08/0x140E2F190
QuestGiverQuest CGQuestInfo::m_availableQuests[32];

// STATIC: 0x00F3FE88/0x140E334EC
QuestRewards_C CGQuestInfo::m_rewards;

// STATIC: 0x00F3FF98/0x140E33520
char CGQuestInfo::m_rewardText[3000];

// STATIC: 0x00F40B50/0x140E340E0
char CGQuestInfo::m_progressText[3000];

// STATIC: 0x00F41708/0x140E34CA0
char CGQuestInfo::m_questLogText[3000];

// STATIC: 0x00F422C0/0x140E35860
char CGQuestInfo::m_questText[3000];

// STATIC: 0x00F42E78/0x140E36420
char CGQuestInfo::m_greetingText[2048];

// STATIC: 0x00F43678/0x140E36C20
char CGQuestInfo::m_questTitle[512];

// STATIC: 0x00F43878/0x140E36E20
WOWGUID CGQuestInfo::m_npc;

// STATIC: 0x00F43880/0x140E36E28
WOWGUID CGQuestInfo::m_npcInform;

// STATIC: 0x00F43888/0x140E36E30
int32_t CGQuestInfo::m_state;

// STATIC: 0x00F4388C/0x140E36E34
int32_t CGQuestInfo::m_currentQuest;

// STATIC: 0x00F43890/0x140E36E38
int32_t CGQuestInfo::m_canComplete;

// STATIC: 0x00F43894/0x140E36E3C
int32_t CGQuestInfo::m_autoLaunched;

// STATIC: 0x00F43898/0x140E36E40
int32_t CGQuestInfo::m_startQuestCheat;

// STATIC: 0x00F4389C/0x140E36E44
int32_t CGQuestInfo::m_fromAreaTrigger;

// STATIC: 0x00F438A0/0x140E36E48
int32_t CGQuestInfo::m_lastChosenItem;

// STATIC: 0x00F438A4/0x140E36E4C
uint32_t CGQuestInfo::m_numQuests;

// STATIC: 0x00F438A8/0x140E36E50
uint32_t CGQuestInfo::m_numInProgress;

// STATIC: 0x00F438AC/0x140E36E54
int32_t CGQuestInfo::m_requiredMoney;

// STATIC: 0x00F438B0/0x140E36E58
int32_t CGQuestInfo::m_criteriaSpell;

// STATIC: 0x00F438B4
int32_t CGQuestInfo::m_pendingQuest;

// STATIC: 0x00F438B8/0x140E36E60
int32_t CGQuestInfo::m_responseSent;

// STATIC: 0x00F438BC/0x140E36E64
int32_t CGQuestInfo::m_suggestedGroupNum;

// STATIC: 0x00F438C0/0x140E36E68
uint32_t CGQuestInfo::m_questFlags;

// STATIC: 0x00F438C4/0x140E36E6C
int32_t CGQuestInfo::m_portraitGiver;

// STATIC: 0x00F438C8/0x140E36E70
int32_t CGQuestInfo::m_portraitTurnIn;

// STATIC: 0x00F438CC/0x140E36E74
int32_t CGQuestInfo::m_autoAcceptSound;

// STATIC: 0x00F438D0/0x140E36E80
static char s_currencyTexture[260];

// STATIC: 0x00F439D8/0x140E36F90
static char s_questItemTexture[260];

// STATIC: 0x00D126B0
static const uint32_t s_rewardFactionFlagBits[5] = { 0x1, 0x2, 0x4, 0x8, 0x10 };

// OFFSET: 0x00944110/0x1406B0F10
void CGQuestInfo::ClearState() {
    for (uint32_t i = 0; i < 32; i++) {
        CGQuestInfo::m_availableQuests[i].m_questID = 0;
        CGQuestInfo::m_availableQuests[i].m_level = 0;
        CGQuestInfo::m_availableQuests[i].m_flags = 0;
        CGQuestInfo::m_availableQuests[i].m_title[0] = 0;
        CGQuestInfo::m_availableQuests[i].m_complete = 0;
        CGQuestInfo::m_availableQuests[i].m_repeatable = 0;
        CGQuestInfo::m_inProgressQuests[i].m_questID = 0;
        CGQuestInfo::m_inProgressQuests[i].m_level = 0;
        CGQuestInfo::m_inProgressQuests[i].m_flags = 0;
        CGQuestInfo::m_inProgressQuests[i].m_title[0] = 0;
        CGQuestInfo::m_inProgressQuests[i].m_complete = 0;
        CGQuestInfo::m_inProgressQuests[i].m_repeatable = 0;
    }

    CGQuestInfo::m_numQuests = 0;
    CGQuestInfo::m_numInProgress = 0;
    CGQuestInfo::m_criteriaSpell = 0;
    CGQuestInfo::m_greetingText[0] = 0;
    CGQuestInfo::m_questTitle[0] = 0;
    CGQuestInfo::m_questText[0] = 0;
    CGQuestInfo::m_questLogText[0] = 0;
    CGQuestInfo::m_progressText[0] = 0;
    CGQuestInfo::m_rewardText[0] = 0;
    CGQuestInfo::m_suggestedGroupNum = 0;
    CGQuestInfo::m_autoLaunched = 0;
    CGQuestInfo::m_questFlags = 0;
    CGQuestInfo::m_portraitGiver = 0;
    CGQuestInfo::m_portraitGiverText[0] = 0;
    CGQuestInfo::m_portraitGiverName[0] = 0;
    CGQuestInfo::m_portraitTurnIn = 0;
    CGQuestInfo::m_portraitTurnInText[0] = 0;
    CGQuestInfo::m_portraitTurnInName[0] = 0;
    CGQuestInfo::m_autoAcceptSound = 0;
}

// OFFSET: 0x009441F0/0x1406B1000
void CGQuestInfo::ClearQuestItems() {
    for (int32_t i = 0; i < 6; i++) {
        CGQuestInfo::m_questItems[i].m_reward.m_itemID = 0;
        CGQuestInfo::m_questItems[i].m_reward.m_displayID = 0;
        CGQuestInfo::m_questItems[i].m_reward.m_count = 0;
        CGQuestInfo::m_questItems[i].m_choice.m_itemID = 0;
        CGQuestInfo::m_questItems[i].m_choice.m_displayID = 0;
        CGQuestInfo::m_questItems[i].m_choice.m_count = 0;
        CGQuestInfo::m_questItems[i].m_required.m_itemID = 0;
        CGQuestInfo::m_questItems[i].m_required.m_displayID = 0;
        CGQuestInfo::m_questItems[i].m_required.m_count = 0;
    }
}

// OFFSET: 0x00944310/0x1406B1150
void CGQuestInfo::EnterWorld() {
    CGQuestInfo::m_npc = WOWGUID::Make(0);
    CGQuestInfo::m_npcInform = WOWGUID::Make(0);
    CGQuestInfo::m_state = 0;
    CGQuestInfo::m_currentQuest = 0;
    CGQuestInfo::m_canComplete = 0;
    memset(&CGQuestInfo::m_questRequest, 0, sizeof(CGQuestInfo::m_questRequest));
    CGQuestInfo::m_autoLaunched = 0;
    CGQuestInfo::m_requiredMoney = 0;
    CGQuestInfo::m_lastChosenItem = 0;
    CGQuestInfo::m_questFlags = 0;
    CGQuestInfo::m_portraitGiver = 0;
    CGQuestInfo::m_portraitTurnIn = 0;
    CGQuestInfo::ClearState();
    CGQuestInfo::ClearQuestItems();
}

// OFFSET: 0x00944380/0x1406B11C0
void CGQuestInfo::EndQuestList() {
    FrameScript_SignalEvent(SCRIPT_EVENT_QUEST_GREETING, NULL);
}

// OFFSET: 0x00944390/0x1406B11D0
void CGQuestInfo::SetPortraits(int32_t giver, const char* giverText, const char* giverName, int32_t turnIn, const char* turnInText, const char* turnInName) {
    CGQuestInfo::m_portraitGiver = giver;

    if (giverText) {
        SStrCopy(CGQuestInfo::m_portraitGiverText, giverText, sizeof(CGQuestInfo::m_portraitGiverText));
    } else {
        CGQuestInfo::m_portraitGiverText[0] = 0;
    }

    if (giverName) {
        SStrCopy(CGQuestInfo::m_portraitGiverName, giverName, sizeof(CGQuestInfo::m_portraitGiverName));
    } else {
        CGQuestInfo::m_portraitGiverName[0] = 0;
    }

    CGQuestInfo::m_portraitTurnIn = turnIn;

    if (turnInText) {
        SStrCopy(CGQuestInfo::m_portraitTurnInText, turnInText, sizeof(CGQuestInfo::m_portraitTurnInText));
    } else {
        CGQuestInfo::m_portraitTurnInText[0] = 0;
    }

    if (turnInName) {
        SStrCopy(CGQuestInfo::m_portraitTurnInName, turnInName, sizeof(CGQuestInfo::m_portraitTurnInName));
    } else {
        CGQuestInfo::m_portraitTurnInName[0] = 0;
    }
}

// OFFSET: 0x00944480/0x1406B1310
void CGQuestInfo::SetRewardFactionInfo(uint32_t index, int32_t faction, int32_t value, int32_t override) {
    if (index <= 4) {
        CGQuestInfo::m_rewards.m_factionIDs[index] = faction;
        CGQuestInfo::m_rewards.m_factionValues[index] = value;
        CGQuestInfo::m_rewards.m_factionOverrides[index] = override;
    }
}

// OFFSET: 0x009444B0/0x1406B1340
void CGQuestInfo::SetRewardFactionFlags(uint32_t flags) {
    CGQuestInfo::m_rewards.m_factionFlags = flags;
}

// OFFSET: 0x009444C0
int32_t CGQuestInfo::GetNumQuestChoices() {
    int32_t count = 0;

    for (uint32_t i = 0; i < 6; i++) {
        if (!CGQuestInfo::m_questItems[i].m_choice.m_itemID) {
            break;
        }

        count++;
    }

    return count;
}

// OFFSET: 0x009444E0/0x1406B1350
int32_t CGQuestInfo::GetQuestItemID(const char* type, uint32_t index) {
    if (index > 6) {
        return 0;
    }

    if (!_stricmp(type, "reward")) {
        return CGQuestInfo::m_questItems[index].m_reward.m_itemID;
    }

    if (!_stricmp(type, "choice")) {
        return CGQuestInfo::m_questItems[index].m_choice.m_itemID;
    }

    if (!_stricmp(type, "required")) {
        return CGQuestInfo::m_questItems[index].m_required.m_itemID;
    }

    return 0;
}

// OFFSET: 0x00944560/0x1406B1400
int32_t CGQuestInfo::GetQuestCurrencyID(const char* type, uint32_t index) {
    if (!_stricmp(type, "reward")) {
        if (index < 4) {
            return CGQuestInfo::m_rewards.m_currencyIDs[index];
        }
    } else if (!_stricmp(type, "required") && index < 4) {
        return CGQuestInfo::m_questRequest.m_currencies[index].m_currencyID;
    }

    return 0;
}

// OFFSET: 0x009445C0/0x1406B1480
void CGQuestInfo::ConfirmAcceptQuest(int32_t questId, const char* title, const WOWGUID* guid) {
    CGQuestInfo::m_pendingQuest = questId;
    WOWGUID requester = WOWGUID();

    if (NameCache* name = g_nameDBCache.GetRecord(guid->m_guid, requester, NULL, NULL, false)) {
        FrameScript_SignalEvent(SCRIPT_EVENT_QUEST_ACCEPT_CONFIRM, "%s%s", static_cast<const char*>(name->m_name), title);
    }
}

// OFFSET: 0x00944620/0x1406B14E0
bool CGQuestInfo::IsQuestCompleteAt(uint32_t index) {
    if (!(index >= CGQuestInfo::m_numInProgress)) {
        uint32_t logIndex = CGQuestLog::GetQuestIndex(CGQuestInfo::m_inProgressQuests[index].m_questID) - 1;
        return CGQuestLog::IsQuestComplete(logIndex) || CGQuestLog::HasNoObjectives(logIndex);
    }

    return false;
}

// OFFSET: 0x00945070/0x1406B1E60
void CGQuestInfo::SetState(WOWGUID npc, WOWGUID inform, int32_t state, const char* text, int32_t questId, int32_t startCheat, int32_t fromAreaTrigger, int32_t criteriaSpell) {
    CGObject_C* object = ClntObjMgrObjectPtr(npc, TYPEMASK_OBJECT, __FILE__, __LINE__);

    if (!object || !(object->m_obj->m_type & (TYPEMASK_ITEM | TYPEMASK_UNIT | TYPEMASK_GAMEOBJECT))) {
        return;
    }

    if (!state) {
        CGQuestInfo::ClearState();
    }

    char buffer[3000];
    WOWGUID player = ClntObjMgrGetActivePlayer();
    QuestParserParseText(text, buffer, sizeof(buffer), player, NULL, 0);

    if (!buffer[0]) {
        SStrCopy(buffer, " ", sizeof(buffer));
    }

    switch (state) {
        case 0:
            SStrCopy(CGQuestInfo::m_greetingText, buffer, sizeof(CGQuestInfo::m_greetingText));
            break;

        case 1:
            SStrCopy(CGQuestInfo::m_questText, buffer, sizeof(CGQuestInfo::m_questText));
            break;

        case 2:
            SStrCopy(CGQuestInfo::m_progressText, buffer, sizeof(CGQuestInfo::m_progressText));
            break;

        case 3:
            SStrCopy(CGQuestInfo::m_rewardText, buffer, sizeof(CGQuestInfo::m_rewardText));
            break;

        default:
            break;
    }

    CGQuestInfo::m_state = state;
    CGQuestInfo::m_currentQuest = questId;
    CGQuestInfo::m_fromAreaTrigger = fromAreaTrigger;
    CGQuestInfo::m_startQuestCheat = startCheat;
    CGQuestInfo::m_criteriaSpell = criteriaSpell;

    if (!(object->m_obj->m_type & TYPEMASK_GAMEOBJECT) || npc != CGGameUI::m_interactTarget) {
        CGGameUI::SetInteractTarget(npc);
    }

    CGQuestInfo::m_npc = npc;
    CGQuestInfo::m_npcInform = inform;
    CGQuestInfo::m_lastChosenItem = 0;
    CGQuestInfo::m_responseSent = 0;
}

// OFFSET: 0x00945270/0x1406B20F0
void CGQuestInfo::SetLogDescription(const char* text) {
    if (!text || !*text) {
        CGQuestInfo::m_questLogText[0] = 0;
        return;
    }

    char buffer[3000];
    WOWGUID player = ClntObjMgrGetActivePlayer();
    QuestParserParseText(text, buffer, sizeof(buffer), player, NULL, 0);
    SStrCopy(CGQuestInfo::m_questLogText, buffer, sizeof(CGQuestInfo::m_questLogText));
}

// OFFSET: 0x009452F0/0x1406B21B0
void CGQuestInfo::AddQuest(int32_t questId, const char* title, int32_t level, int32_t flags, int32_t complete, int32_t repeatable) {
    QuestGiverQuest& quest = CGQuestInfo::m_availableQuests[CGQuestInfo::m_numQuests];
    quest.m_questID = questId;
    quest.m_level = level;
    quest.m_flags = flags;

    if (title) {
        SStrCopy(quest.m_title, title, sizeof(quest.m_title));
    }

    CGQuestInfo::m_numQuests++;
    quest.m_complete = complete;
    quest.m_repeatable = repeatable;
}

// OFFSET: 0x00945370/0x1406B2260
void CGQuestInfo::AddQuestInProgress(int32_t questId, const char* title, int32_t level, int32_t flags, int32_t repeatable) {
    QuestGiverQuest& quest = CGQuestInfo::m_inProgressQuests[CGQuestInfo::m_numInProgress];
    quest.m_questID = questId;
    quest.m_level = level;
    quest.m_flags = flags;

    if (title) {
        SStrCopy(quest.m_title, title, sizeof(quest.m_title));
    }

    CGQuestInfo::m_numInProgress++;
    quest.m_complete = 0;
    quest.m_repeatable = repeatable;
}

// OFFSET: 0x009453F0/0x1406B2310
void CGQuestInfo::AddItemRequest(const char* title, const QuestRequest& request, int32_t requiredMoney, int32_t canComplete, int32_t autoLaunched, int32_t suggestedGroupNum, uint32_t questFlags) {
    for (uint32_t i = 0; i < 6; i++) {
        QuestItemInfo& required = CGQuestInfo::m_questItems[i].m_required;

        if (request.m_items[i].m_itemID) {
            required.m_itemID = request.m_items[i].m_itemID;
            required.m_displayID = request.m_items[i].m_displayID;
            required.m_count = request.m_items[i].m_count;
        } else {
            required.m_itemID = 0;
            required.m_displayID = 0;
            required.m_count = 1;
        }
    }

    CGQuestInfo::m_questRequest = request;

    if (title) {
        SStrCopy(CGQuestInfo::m_questTitle, *title ? title : " ", sizeof(CGQuestInfo::m_questTitle));
    }

    CGQuestInfo::m_requiredMoney = requiredMoney;
    CGQuestInfo::m_canComplete = canComplete;
    CGQuestInfo::m_autoLaunched = autoLaunched;
    CGQuestInfo::m_suggestedGroupNum = suggestedGroupNum;
    CGQuestInfo::m_questFlags = questFlags;
    FrameScript_SignalEvent(SCRIPT_EVENT_QUEST_PROGRESS, NULL);
}

// OFFSET: 0x009455D0/0x1406B2440
void CGQuestInfo::ProcessFactionRewardAdjust(int32_t factionId, int32_t amount, int32_t upperRank, int32_t lowerRank, int32_t source, int32_t noSpillover) {
    FactionRec* faction = g_factionDB.GetRecord(factionId);

    if (!faction) {
        return;
    }

    int32_t current = GetRepListRepValue(factionId);
    int32_t result = current + amount;

    if (amount <= 0) {
        if (lowerRank >= 0) {
            int32_t limit = 100 * CGReputationInfo::m_reactionThresholds[lowerRank];
            int32_t floor = current >= limit ? limit : current;

            if (result <= floor) {
                if (current < limit) {
                    return;
                }

                result = limit;
            }
        }
    } else if (upperRank >= 0) {
        int32_t limit = 100 * CGReputationInfo::m_reactionThresholds[upperRank + 1] - 100;
        int32_t ceiling = current <= limit ? limit : current;

        if (result >= ceiling) {
            if (current > limit) {
                return;
            }

            result = limit;
        }
    }

    if (result == current || CGQuestInfo::m_numRewardFactions >= 10) {
        return;
    }

    CGQuestInfo::m_rewardFactionList[CGQuestInfo::m_numRewardFactions] = factionId;
    CGQuestInfo::m_rewardFactionAmounts[CGQuestInfo::m_numRewardFactions] = amount;
    CGQuestInfo::m_numRewardFactions++;

    if (noSpillover) {
        return;
    }

    int32_t parent = faction->m_parentFactionID;

    if (parent && parent != source) {
        int32_t spill = static_cast<int32_t>(static_cast<float>(amount) * faction->m_parentFactionMod[1]);

        if (spill) {
            CGQuestInfo::ProcessFactionRewardAdjust(parent, spill, faction->m_parentFactionCap[1], -1, faction->m_id, 0);
        }
    }

    TSGrowableArray<const FactionRec*>* children = CGReputationInfo::GetFactionChildren(faction->m_id);

    if (!children) {
        return;
    }

    for (uint32_t i = children->m_count; i; i--) {
        const FactionRec* child = children->m_data[i - 1];

        if (child->m_id == source) {
            continue;
        }

        int32_t spill = static_cast<int32_t>(static_cast<float>(amount) * child->m_parentFactionMod[0]);

        if (spill) {
            CGQuestInfo::ProcessFactionRewardAdjust(child->m_id, spill, child->m_parentFactionCap[0], -1, faction->m_id, 0);
        }
    }
}

// OFFSET: 0x00945780/0x1406B2600
int32_t CGQuestInfo::GetQuestCurrencyInfo(const char* type, uint32_t index, char* name, uint32_t nameSize, char* texture, uint32_t textureSize, uint32_t& count) {
    int32_t currencyId = 0;

    if (!_stricmp(type, "reward")) {
        if (index >= 4) {
            return 0;
        }

        currencyId = CGQuestInfo::m_rewards.m_currencyIDs[index];
        count = CGQuestInfo::m_rewards.m_currencyCounts[index];
    } else if (!_stricmp(type, "required")) {
        if (index >= 4) {
            return 0;
        }

        currencyId = CGQuestInfo::m_questRequest.m_currencies[index].m_currencyID;
        count = CGQuestInfo::m_questRequest.m_currencies[index].m_count;
    }

    CurrencyTypesRec* rec = g_currencyTypesDB.GetRecord(currencyId);

    if (!rec) {
        return 0;
    }

    SStrCopy(name, rec->m_name, nameSize);
    const char* path = ClientDBStringLookup(3);
    SStrPrintf(s_currencyTexture, sizeof(s_currencyTexture), "%s%s%s", path, *path ? "\\" : "", CGCurrencyTypes::GetCurrencyIcon(rec));
    SStrCopy(texture, s_currencyTexture, textureSize);
    return 1;
}

// OFFSET: 0x00945CB0/0x1406B2CA0
void CGQuestInfo::QuestGiverFinished(int32_t keepOpen, int32_t questId, int32_t notify, int32_t talkToGossip) {
    if (!CGQuestInfo::m_npc) {
        return;
    }

    if (notify && ClntObjMgrObjectPtr(CGQuestInfo::m_npc, TYPEMASK_PLAYER, __FILE__, __LINE__)) {
        CDataStore msg;
        msg.Put(static_cast<uint32_t>(MSG_QUEST_PUSH_RESULT));
        msg.Put(CGQuestInfo::m_npcInform ? CGQuestInfo::m_npcInform.m_guid : CGQuestInfo::m_npc.m_guid);
        msg.Put(static_cast<uint32_t>(questId));
        msg.Put(static_cast<uint8_t>(3));
        msg.Finalize();
        ClientServices::Send2(&msg);
    }

    if (keepOpen) {
        return;
    }

    WOWGUID npc = CGQuestInfo::m_npc;
    CGQuestInfo::m_npc = WOWGUID();
    CGQuestInfo::m_npcInform = WOWGUID();

    if (!CGGossipInfo::m_gossip) {
        CGGameUI::ClearInteractTarget(npc, 0);
    }

    FrameScript_SignalEvent(SCRIPT_EVENT_QUEST_FINISHED, NULL);

    if (talkToGossip && ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__)) {
        CGPlayer_C::TalkToGossip(npc);
    }
}

// OFFSET: 0x00945E20/0x1406B2E40
int32_t CGQuestInfo::IsCompletable() {
    if (!CGQuestInfo::m_canComplete) {
        return 0;
    }

    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return 0;
    }

    for (uint32_t i = 0; i < 6; i++) {
        int32_t itemId = CGQuestInfo::m_questItems[i].m_required.m_itemID;

        if (!itemId) {
            return 1;
        }

        if (player->m_bag.GetItemTypeCount(itemId, 8) < CGQuestInfo::m_questItems[i].m_required.m_count) {
            return 0;
        }
    }

    return 1;
}

// OFFSET: 0x00945E90/0x1406B2F00
void CGQuestInfo::QueryQuest(uint32_t index) {
    if (index > CGQuestInfo::m_numQuests || CGQuestInfo::m_responseSent) {
        return;
    }

    if (!ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__)) {
        return;
    }

    if (CGQuestInfo::m_availableQuests[index].m_complete) {
        CGPlayer_C::CompleteQuest(CGQuestInfo::m_npc, CGQuestInfo::m_availableQuests[index].m_questID, 0);
    } else {
        CGPlayer_C::QueryQuest(CGQuestInfo::m_npc, CGQuestInfo::m_availableQuests[index].m_questID);
    }

    CGQuestInfo::m_responseSent = 1;
}

// OFFSET: 0x00945F20/0x1406B2FA0
void CGQuestInfo::CompleteQuest(uint32_t index) {
    if (index > CGQuestInfo::m_numInProgress || CGQuestInfo::m_responseSent) {
        return;
    }

    if (!ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__)) {
        return;
    }

    CGTutorial::TriggerTutorial(33);
    CGPlayer_C::CompleteQuest(CGQuestInfo::m_npc, CGQuestInfo::m_inProgressQuests[index].m_questID, 0);
    CGQuestInfo::m_responseSent = 1;
}

// OFFSET: 0x00945F90/0x1406B3030
void CGQuestInfo::AcceptQuest() {
    if (CGQuestInfo::m_state != 1) {
        return;
    }

    if (!ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__) || CGQuestInfo::m_responseSent) {
        return;
    }

    CGPlayer_C::AcceptQuest(CGQuestInfo::m_npcInform ? CGQuestInfo::m_npcInform : CGQuestInfo::m_npc, CGQuestInfo::m_currentQuest, CGQuestInfo::m_startQuestCheat);
    CGQuestInfo::m_startQuestCheat = 0;
    CGQuestInfo::m_responseSent = 1;
    CGQuestInfo::QuestGiverFinished(0, 0, 0, (CGQuestInfo::m_questFlags >> 29) & 1);
}

// OFFSET: 0x00946030/0x1406B3150
void CGQuestInfo::DeclineQuest() {
    if (CGQuestInfo::m_responseSent) {
        return;
    }

    CGObject_C* object = ClntObjMgrObjectPtr(CGQuestInfo::m_npc, TYPEMASK_OBJECT, __FILE__, __LINE__);

    if (!object) {
        return;
    }

    if ((object->m_obj->m_type & TYPEMASK_UNIT) && (static_cast<CGUnit_C*>(object)->m_unit->m_unitNpcFlags & 1) && ClntObjMgrGetActivePlayerObj()) {
        CGPlayer_C::TalkToGossip(CGQuestInfo::m_npc);
        CGQuestInfo::m_responseSent = 1;
        return;
    }

    uint32_t type = object->m_obj->m_type;

    if ((type & (TYPEMASK_ITEM | TYPEMASK_PLAYER))) {
        CGQuestInfo::QuestGiverFinished(0, CGQuestInfo::m_currentQuest, 1, 0);
        return;
    }

    if (CGQuestInfo::m_autoLaunched) {
        CGQuestInfo::QuestGiverFinished(0, CGQuestInfo::m_currentQuest, 1, 0);
        return;
    }

    if (type & TYPEMASK_GAMEOBJECT) {
        object->OnRightClick();
    } else {
        CDataStore msg;
        msg.Put(static_cast<uint32_t>(CMSG_QUEST_GIVER_HELLO));
        msg.Put(CGQuestInfo::m_npc.m_guid);
        msg.Finalize();
        ClientServices::Send2(&msg);
    }

    CGQuestInfo::m_responseSent = 1;
}

// OFFSET: 0x00946170/0x1406B3300
void CGQuestInfo::GiveQuestItems() {
    if (CGQuestInfo::m_state != 2 || CGQuestInfo::m_responseSent) {
        return;
    }

    if (!ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__)) {
        return;
    }

    CGTutorial::TriggerTutorial(33);
    CGPlayer_C::GiveQuestItems(CGQuestInfo::m_npc, CGQuestInfo::m_currentQuest);
    CGQuestInfo::m_responseSent = 1;
}

// OFFSET: 0x009461D0/0x1406B3380
int32_t CGQuestInfo::GetReward(int32_t choice) {
    if (CGQuestInfo::m_state != 3) {
        return 1;
    }

    if (!ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__) || CGQuestInfo::m_responseSent) {
        return 1;
    }

    int32_t numChoices = CGQuestInfo::GetNumQuestChoices();

    if (numChoices > 0) {
        if (choice >= numChoices || choice < 0) {
            return 0;
        }

        CGQuestInfo::m_lastChosenItem = CGQuestInfo::m_questItems[choice].m_choice.m_itemID;
    }

    CGTutorial::TriggerTutorial(33);
    CGPlayer_C::GetQuestReward(CGQuestInfo::m_npc, CGQuestInfo::m_currentQuest, choice <= 0 ? 0 : choice);
    CGQuestInfo::m_responseSent = 1;
    return 1;
}

// OFFSET: 0x00946280/0x1406B3480
bool CGQuestInfo::IsTrivialQuest(int32_t level) {
    if (level != -1) {
        int32_t range = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__))->GetQuestGreenRange();
        return level + range < static_cast<int32_t>(static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__))->m_unit->m_unitFieldLevel);
    }

    return false;
}

// OFFSET: 0x00946310/0x1406B3540
void CGQuestInfo::ProcessFactionRewards() {
    CGQuestInfo::m_numRewardFactions = 0;

    for (uint32_t i = 0; i < 10; i++) {
        CGQuestInfo::m_rewardFactionList[i] = 0;
    }

    for (uint32_t i = 0; i < 10; i++) {
        CGQuestInfo::m_rewardFactionAmounts[i] = 0;
    }

    for (uint32_t i = 0; i < 5; i++) {
        FactionRec* faction = g_factionDB.GetRecord(CGQuestInfo::m_rewards.m_factionIDs[i]);

        if (!faction) {
            continue;
        }

        int32_t value = CGQuestInfo::m_rewards.m_factionValues[i];
        QuestFactionRewardRec* reward = g_questFactionRewardDB.GetRecord((value < 0) + 1);
        uint32_t index = abs(value);

        if (!reward || index >= 10 || !reward->m_difficulty[index]) {
            continue;
        }

        int32_t amount = CGQuestInfo::m_rewards.m_factionOverrides[i];

        if (!amount) {
            amount = 100 * reward->m_difficulty[index];
        }

        CGQuestInfo::ProcessFactionRewardAdjust(faction->m_id, amount, -1, -1, 0, (CGQuestInfo::m_rewards.m_factionFlags & s_rewardFactionFlagBits[i]) != 0);
    }
}

// OFFSET: 0x00946BC0/0x1406B3DE0
void CGQuestInfo::LeaveWorld() {
    CGQuestInfo::QuestGiverFinished(0, CGQuestInfo::m_currentQuest, 1, 0);
}

// OFFSET: 0x00946BE0/0x1406B3E00
void CGQuestInfo::AddReward(const char* title, const QuestRewards_C& rewards, int32_t autoLaunched, int32_t suggestedGroupNum, uint32_t questFlags) {
    CGQuestInfo::m_rewards = rewards;
    CGQuestInfo::ClearQuestItems();
    int32_t i = 0;

    for (; i < CGQuestInfo::m_rewards.m_numChoiceItems; i++) {
        CGQuestInfo::m_questItems[i].m_choice.m_itemID = CGQuestInfo::m_rewards.m_choiceItemIDs[i];
        CGQuestInfo::m_questItems[i].m_choice.m_displayID = CGQuestInfo::m_rewards.m_choiceItemDisplayIDs[i];
        CGQuestInfo::m_questItems[i].m_choice.m_count = CGQuestInfo::m_rewards.m_choiceItemCounts[i];
    }

    for (; i < 6; i++) {
        CGQuestInfo::m_questItems[i].m_choice.m_itemID = 0;
        CGQuestInfo::m_questItems[i].m_choice.m_displayID = 0;
        CGQuestInfo::m_questItems[i].m_choice.m_count = 1;
    }

    i = 0;

    for (; i < CGQuestInfo::m_rewards.m_numRewardItems; i++) {
        CGQuestInfo::m_questItems[i].m_reward.m_itemID = CGQuestInfo::m_rewards.m_rewardItemIDs[i];
        CGQuestInfo::m_questItems[i].m_reward.m_displayID = CGQuestInfo::m_rewards.m_rewardItemDisplayIDs[i];
        CGQuestInfo::m_questItems[i].m_reward.m_count = CGQuestInfo::m_rewards.m_rewardItemCounts[i];
    }

    for (; i < 6; i++) {
        CGQuestInfo::m_questItems[i].m_reward.m_itemID = 0;
        CGQuestInfo::m_questItems[i].m_reward.m_displayID = 0;
        CGQuestInfo::m_questItems[i].m_reward.m_count = 1;
    }

    if (title) {
        SStrCopy(CGQuestInfo::m_questTitle, *title ? title : " ", sizeof(CGQuestInfo::m_questTitle));
    }

    CGQuestInfo::m_autoLaunched = autoLaunched;

    if (rewards.m_money < 0) {
        CGQuestInfo::m_rewards.m_money = 0;
        CGQuestInfo::m_requiredMoney = -rewards.m_money;
    } else {
        CGQuestInfo::m_requiredMoney = 0;
    }

    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player && static_cast<int32_t>(player->m_unit->m_unitFieldLevel) >= static_cast<int32_t>(player->m_player->m_playerFieldMaxLevel)) {
        CGQuestInfo::m_rewards.m_xp = 0;
    }

    CGQuestInfo::ProcessFactionRewards();
    CGQuestInfo::m_suggestedGroupNum = suggestedGroupNum;
    CGQuestInfo::m_questFlags = questFlags;
}

// OFFSET: 0x00946E70/0x1406B4190
static void QuestItemStatsCallback(int32_t id, const WOWGUID& guid, void* param, bool success) {
    if (!success) {
        return;
    }

    WOWGUID requester;
    requester.m_guid = 0;

    if (g_itemSparseDB.GetRecord(id, requester, NULL, NULL, false)) {
        FrameScript_SignalEvent(SCRIPT_EVENT_QUEST_ITEM_UPDATE, NULL);
    }
}

// OFFSET: 0x00946EC0/0x1406B41E0
void CGQuestInfo::ShowOfferFromQuest(CGPlayer_C* player, WOWGUID npc, int32_t questId) {
    WOWGUID requester = WOWGUID();
    QuestCache* quest = g_questDBCache.GetRecord(questId, requester, NULL, NULL, false);

    if (!quest) {
        return;
    }

    QuestRewards_C rewards;
    int32_t numRewards = 0;
    int32_t numChoices = 0;

    for (uint32_t i = 0; i < 4; i++) {
        if (quest->m_rewardItem[i]) {
            numRewards++;
        }
    }

    for (uint32_t i = 0; i < 6; i++) {
        if (quest->m_rewardChoiceItem[i]) {
            numChoices++;
        }
    }

    rewards.m_arenaPoints = quest->m_rewardArenaPoints;

    for (uint32_t i = 0; i < 6; i++) {
        rewards.m_choiceItemIDs[i] = quest->m_rewardChoiceItem[i];
    }

    rewards.m_numChoiceItems = numChoices;
    WOWGUID itemRequester = WOWGUID();

    for (int32_t i = 0; i < numChoices; i++) {
        ItemRec* item = g_itemDB.GetRecord(quest->m_rewardChoiceItem[i], itemRequester, NULL, NULL, false);
        if (item) {
            rewards.m_choiceItemDisplayIDs[i] = item->m_displayInfoID;
        } else {
            rewards.m_choiceItemDisplayIDs[i] = 0;
        }
    }

    for (uint32_t i = 0; i < 6; i++) {
        rewards.m_choiceItemCounts[i] = quest->m_rewardChoiceItemCount[i];
    }

    for (uint32_t i = 0; i < 4; i++) {
        rewards.m_currencyIDs[i] = quest->m_rewardCurrencyId[i];
        rewards.m_currencyCounts[i] = quest->m_rewardCurrencyCount[i];
    }

    rewards.m_factionFlags = quest->m_rewardReputationMask;

    for (uint32_t i = 0; i < 5; i++) {
        rewards.m_factionIDs[i] = quest->m_rewardReputationFaction[i];
        rewards.m_factionOverrides[i] = quest->m_rewardReputationOverride[i];
        rewards.m_factionValues[i] = quest->m_factionRewardId[i];
    }

    rewards.m_honor = quest->m_rewardHonor;
    rewards.m_numRewardItems = numRewards;

    for (uint32_t i = 0; i < 4; i++) {
        rewards.m_rewardItemDisplayIDs[i] = quest->m_rewardItem[i];
        rewards.m_rewardItemIDs[i] = quest->m_rewardItem[i];
        rewards.m_rewardItemCounts[i] = quest->m_rewardItemCount[i];
    }

    rewards.m_money = quest->m_rewardMoney;
    rewards.m_spellCast = quest->m_effectOnPlayer;
    rewards.m_spell = quest->m_rewardSpellId;
    rewards.m_talents = quest->m_rewardTalentPoints;
    rewards.m_title = quest->m_rewardTitleId;
    rewards.m_honorMultiplier = quest->m_rewardHonorBonus;
    rewards.m_xp = CalcQuestExp(quest->m_id, player->m_unit->m_unitFieldLevel);
    rewards.m_skillLine = quest->m_rewardSkillId;
    rewards.m_skillPoints = quest->m_rewardSkillPoints;
    CGQuestInfo::SetState(npc, WOWGUID::Make(0), 1, quest->m_description, questId, 0, 0, 0);
    CGQuestInfo::SetLogDescription(quest->m_objText);
    CGQuestInfo::AddReward(quest->m_name, rewards, 0, quest->m_suggestedPlayers, quest->m_flags);
    CGQuestInfo::SetPortraits(quest->m_questGiverPortrait, quest->m_questGiverTextWindow, quest->m_questGiverTargetName, quest->m_questTurnInPortrait, quest->m_questTurnTextWindow, quest->m_questTurnTargetName);
    FrameScript_SignalEvent(SCRIPT_EVENT_QUEST_DETAIL, NULL);
}

// OFFSET: 0x00947290/0x1406B4740
int32_t CGQuestInfo::GetQuestItemInfo(const char* type, uint32_t index, char* name, uint32_t nameSize, char* texture, uint32_t textureSize, uint32_t& count, int32_t& quality, int32_t& usable) {
    *name = 0;
    *texture = 0;
    count = 1;
    quality = 0;
    usable = 1;

    if (index > 6) {
        return 0;
    }

    int32_t itemId;
    int32_t displayId;

    if (!_stricmp(type, "reward")) {
        itemId = CGQuestInfo::m_questItems[index].m_reward.m_itemID;
        displayId = CGQuestInfo::m_questItems[index].m_reward.m_displayID;
        count = CGQuestInfo::m_questItems[index].m_reward.m_count;
    } else if (!_stricmp(type, "choice")) {
        itemId = CGQuestInfo::m_questItems[index].m_choice.m_itemID;
        displayId = CGQuestInfo::m_questItems[index].m_choice.m_displayID;
        count = CGQuestInfo::m_questItems[index].m_choice.m_count;
    } else if (!_stricmp(type, "required")) {
        itemId = CGQuestInfo::m_questItems[index].m_required.m_itemID;
        displayId = CGQuestInfo::m_questItems[index].m_required.m_displayID;
        count = CGQuestInfo::m_questItems[index].m_required.m_count;
    } else {
        return 0;
    }

    ItemRecSparse* rec = g_itemSparseDB.GetRecord(itemId, CGQuestInfo::m_npc, &QuestItemStatsCallback, NULL, false);

    if (rec) {
        SStrCopy(name, rec->GetName(0), nameSize);
        if (rec->m_inventoryType) {
            quality = rec->m_quality;
        } else {
            quality = -1;
        }
    }

    const char* path = ClientDBStringLookup(3);
    SStrPrintf(s_questItemTexture, sizeof(s_questItemTexture), "%s%s%s", path, *path ? "\\" : "", CGItem_C::GetInventoryArt(displayId));
    SStrCopy(texture, s_questItemTexture, textureSize);
    int32_t error;

    if (CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__))) {
        if (rec && !player->CanUseItem(rec, error)) {
            usable = 0;
        }
    }

    return 1;
}

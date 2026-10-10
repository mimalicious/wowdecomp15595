#ifndef WOW_SOURCE_UI_QUESTFRAME_H
#define WOW_SOURCE_UI_QUESTFRAME_H

#include <cstddef>
#include <cstdint>
#include "WoW/Source/Object/WGUID.h"

struct QuestItemInfo {
    /* 0000 */ int32_t m_itemID;
    /* 0004 */ int32_t m_displayID;
    /* 0008 */ int32_t m_count;
};

struct QuestItemSlot {
    /* 0000 */ QuestItemInfo m_reward;
    /* 000C */ QuestItemInfo m_choice;
    /* 0018 */ QuestItemInfo m_required;
};

struct QuestGiverQuest {
    /* 0000 */ int32_t m_questID;
    /* 0004 */ int32_t m_level;
    /* 0008 */ int32_t m_flags;
    /* 000C */ char m_title[512];
    /* 020C */ int32_t m_complete;
    /* 0210 */ int32_t m_repeatable;
};

struct QuestRequestItem {
    /* 0000 */ int32_t m_itemID;
    /* 0004 */ int32_t m_count;
    /* 0008 */ int32_t m_displayID;
};

struct QuestRequestCurrency {
    /* 0000 */ int32_t m_currencyID;
    /* 0004 */ int32_t m_count;
};

struct QuestRequest {
    /* 0000 */ QuestRequestItem m_items[6];
    /* 0048 */ QuestRequestCurrency m_currencies[4];
};

struct QuestRewards_C {
    /* 0000 */ int32_t m_numChoiceItems;
    /* 0004 */ int32_t m_choiceItemIDs[6];
    /* 001C */ int32_t m_choiceItemCounts[6];
    /* 0034 */ int32_t m_choiceItemDisplayIDs[6];
    /* 004C */ int32_t m_numRewardItems;
    /* 0050 */ int32_t m_rewardItemIDs[4];
    /* 0060 */ int32_t m_rewardItemCounts[4];
    /* 0070 */ int32_t m_rewardItemDisplayIDs[4];
    /* 0080 */ int32_t m_money;
    /* 0084 */ int32_t m_xp;
    /* 0088 */ int32_t m_title;
    /* 008C */ int32_t m_honor;
    /* 0090 */ float m_honorMultiplier;
    /* 0094 */ int32_t m_talents;
    /* 0098 */ int32_t m_arenaPoints;
    /* 009C */ uint32_t m_factionFlags;
    /* 00A0 */ int32_t m_factionIDs[5];
    /* 00B4 */ int32_t m_factionValues[5];
    /* 00C8 */ int32_t m_factionOverrides[5];
    /* 00DC */ int32_t m_spell;
    /* 00E0 */ int32_t m_spellCast;
    /* 00E4 */ int32_t m_currencyIDs[4];
    /* 00F4 */ int32_t m_currencyCounts[4];
    /* 0104 */ int32_t m_skillLine;
    /* 0108 */ int32_t m_skillPoints;
};

class CGPlayer_C;

class CGQuestInfo {
    public:
    static QuestRequest m_questRequest;
    static QuestItemSlot m_questItems[6];
    static char m_portraitTurnInName[256];
    static char m_portraitTurnInText[1024];
    static char m_portraitGiverName[256];
    static char m_portraitGiverText[1024];
    static int32_t m_rewardFactionAmounts[10];
    static int32_t m_rewardFactionList[10];
    static uint32_t m_numRewardFactions;
    static QuestGiverQuest m_inProgressQuests[32];
    static QuestGiverQuest m_availableQuests[32];
    static QuestRewards_C m_rewards;
    static char m_rewardText[3000];
    static char m_progressText[3000];
    static char m_questLogText[3000];
    static char m_questText[3000];
    static char m_greetingText[2048];
    static char m_questTitle[512];
    static WOWGUID m_npc;
    static WOWGUID m_npcInform;
    static int32_t m_state;
    static int32_t m_currentQuest;
    static int32_t m_canComplete;
    static int32_t m_autoLaunched;
    static int32_t m_startQuestCheat;
    static int32_t m_fromAreaTrigger;
    static int32_t m_lastChosenItem;
    static uint32_t m_numQuests;
    static uint32_t m_numInProgress;
    static int32_t m_requiredMoney;
    static int32_t m_criteriaSpell;
    static int32_t m_pendingQuest;
    static int32_t m_responseSent;
    static int32_t m_suggestedGroupNum;
    static uint32_t m_questFlags;
    static int32_t m_portraitGiver;
    static int32_t m_portraitTurnIn;
    static int32_t m_autoAcceptSound;

    static void ClearState();
    static void ClearQuestItems();
    static void EnterWorld();
    static void EndQuestList();
    static void SetPortraits(int32_t giver, const char* giverText, const char* giverName, int32_t turnIn, const char* turnInText, const char* turnInName);
    static void SetRewardFactionInfo(uint32_t index, int32_t faction, int32_t value, int32_t override);
    static void SetRewardFactionFlags(uint32_t flags);
    static int32_t GetNumQuestChoices();
    static int32_t GetQuestItemID(const char* type, uint32_t index);
    static int32_t GetQuestCurrencyID(const char* type, uint32_t index);
    static void ConfirmAcceptQuest(int32_t questId, const char* title, const WOWGUID* guid);
    static bool IsQuestCompleteAt(uint32_t index);
    static void SetState(WOWGUID npc, WOWGUID inform, int32_t state, const char* text, int32_t questId, int32_t startCheat, int32_t fromAreaTrigger, int32_t criteriaSpell);
    static void SetLogDescription(const char* text);
    static void AddQuest(int32_t questId, const char* title, int32_t level, int32_t flags, int32_t complete, int32_t repeatable);
    static void AddQuestInProgress(int32_t questId, const char* title, int32_t level, int32_t flags, int32_t repeatable);
    static void AddItemRequest(const char* title, const QuestRequest& request, int32_t requiredMoney, int32_t canComplete, int32_t autoLaunched, int32_t suggestedGroupNum, uint32_t questFlags);
    static void ProcessFactionRewardAdjust(int32_t factionId, int32_t amount, int32_t upperRank, int32_t lowerRank, int32_t source, int32_t noSpillover);
    static int32_t GetQuestCurrencyInfo(const char* type, uint32_t index, char* name, uint32_t nameSize, char* texture, uint32_t textureSize, uint32_t& count);
    static void QuestGiverFinished(int32_t keepOpen, int32_t questId, int32_t notify, int32_t talkToGossip);
    static int32_t IsCompletable();
    static void QueryQuest(uint32_t index);
    static void CompleteQuest(uint32_t index);
    static void AcceptQuest();
    static void DeclineQuest();
    static void GiveQuestItems();
    static int32_t GetReward(int32_t choice);
    static bool IsTrivialQuest(int32_t level);
    static void ProcessFactionRewards();
    static void LeaveWorld();
    static void AddReward(const char* title, const QuestRewards_C& rewards, int32_t autoLaunched, int32_t suggestedGroupNum, uint32_t questFlags);
    static void ShowOfferFromQuest(CGPlayer_C* player, WOWGUID npc, int32_t questId);
    static int32_t GetQuestItemInfo(const char* type, uint32_t index, char* name, uint32_t nameSize, char* texture, uint32_t textureSize, uint32_t& count, int32_t& quality, int32_t& usable);
};

struct lua_State;

void QuestInfoRegisterScriptFunctions();
void QuestInfoUnregisterScriptFunctions();

#endif

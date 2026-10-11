#ifndef WOW_SOURCE_OBJECT_QUESTCACHE_H
#define WOW_SOURCE_OBJECT_QUESTCACHE_H

#include <cstddef>
#include <cstdint>

class CDataStore;

class QuestCache {
    public:
    static const uint32_t s_cacheVersion = 3;

    /* 0000 */ int32_t m_id;
    /* 0004 */ uint32_t m_method;
    /* 0008 */ uint32_t m_level;
    /* 000C */ uint32_t m_requiredLevel;
    /* 0010 */ uint32_t m_areaIdOrSortId;
    /* 0014 */ uint32_t m_infoId;
    /* 0018 */ uint32_t m_suggestedPlayers;
    /* 001C */ uint32_t m_friendlyFactionId;
    /* 0020 */ uint32_t m_friendlyFactionAmount;
    /* 0024 */ uint32_t m_hostileFactionId;
    /* 0028 */ uint32_t m_hostileFactionAmount;
    /* 002C */ uint32_t m_nextQuestId;
    /* 0030 */ uint32_t m_xPId;
    /* 0034 */ int32_t m_rewardMoney;
    /* 0038 */ uint32_t m_rewardMoneyInsteadOfXp;
    /* 003C */ uint32_t m_rewardSpellId;
    /* 0040 */ uint32_t m_effectOnPlayer;
    /* 0044 */ uint32_t m_rewardHonor;
    /* 0048 */ float m_rewardHonorBonus;
    /* 004C */ uint32_t m_startingItemId;
    /* 0050 */ uint32_t m_flags;
    /* 0054 */ uint32_t m_minimapTargetMark;
    /* 0058 */ uint32_t m_rewardItem [4];
    /* 0068 */ uint32_t m_rewardItemCount [4];
    /* 0078 */ uint32_t m_rewardChoiceItem [6];
    /* 0090 */ uint32_t m_rewardChoiceItemCount [6];
    /* 00A8 */ uint32_t m_pointMapId;
    /* 00AC */ float m_pointX;
    /* 00B0 */ float m_pointY;
    /* 00B4 */ uint32_t m_pointOptional;
    /* 00B8 */ char m_name [512];
    /* 02B8 */ char m_objText [3000];
    /* 0E70 */ char m_description [3000];
    /* 1A28 */ char m_endText [512];
    /* 1C28 */ uint32_t m_objectiveId [4];
    /* 1C38 */ uint32_t m_objectiveRequiredCount [4];
    /* 1C48 */ uint32_t m_collectItemId [6];
    /* 1C60 */ uint32_t m_collectItemCount [6];
    /* 1C78 */ uint32_t m_intermediateItemId [4];
    /* 1C88 */ uint32_t m_intermediateItemCount [4];
    /* 1C98 */ uint32_t m_requiredSpell;
    /* 1C9C */ char m_objectiveText [4][256];
    /* 209C */ uint32_t m_rewardTitleId;
    /* 20A0 */ uint32_t m_requiredPlayersKilled;
    /* 20A4 */ uint32_t m_rewardTalentPoints;
    /* 20A8 */ uint32_t m_rewardArenaPoints;
    /* 20AC */ uint32_t m_rewardSkillId;
    /* 20B0 */ uint32_t m_rewardSkillPoints;
    /* 20B4 */ uint32_t m_questGiverPortrait;
    /* 20B8 */ uint32_t m_questTurnInPortrait;
    /* 20BC */ char m_questGiverTextWindow [1024];
    /* 24BC */ char m_questGiverTargetName [256];
    /* 25BC */ char m_questTurnTextWindow [1024];
    /* 29BC */ char m_questTurnTargetName [256];
    /* 2ABC */ char m_completionText [2048];
    /* 32BC */ uint32_t m_rewardReputationFaction [5];
    /* 32D0 */ uint32_t m_factionRewardId [5];
    /* 32E4 */ int32_t m_rewardReputationOverride [5];
    /* 32F8 */ uint32_t m_rewardReputationMask;
    /* 32FC */ uint32_t m_rewardCurrencyId [4];
    /* 330C */ uint32_t m_rewardCurrencyCount [4];
    /* 331C */ uint32_t m_requiredCurrencyId [4];
    /* 332C */ uint32_t m_requiredCurrencyCount [4];
    /* 333C */ uint32_t m_soundAccept;
    /* 3340 */ uint32_t m_soundTurnIn;

    QuestCache();
    QuestCache& operator=(const QuestCache& source);
    void Pack(CDataStore* msg);
    void Unpack(CDataStore* msg);
};

#endif

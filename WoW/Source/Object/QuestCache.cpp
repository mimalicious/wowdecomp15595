#include "QuestCache.h"
#include "Engine/Source/Base/CDataStore.h"
#include "Engine/Source/Storm/H/storm.h"

// OFFSET: 0x00496800/0x1400BCA10
QuestCache::QuestCache() {
    this->m_id = 0;
    this->m_method = 0;
    this->m_level = 0;
    this->m_requiredLevel = 0;
    this->m_areaIdOrSortId = 0;
    this->m_infoId = 0;
    this->m_suggestedPlayers = 0;
    this->m_friendlyFactionId = 0;
    this->m_friendlyFactionAmount = 0;
    this->m_hostileFactionId = 0;
    this->m_hostileFactionAmount = 0;
    this->m_nextQuestId = 0;
    this->m_xPId = 0;
    this->m_rewardMoney = 0;
    this->m_rewardMoneyInsteadOfXp = 0;
    this->m_rewardSpellId = 0;
    this->m_effectOnPlayer = 0;
    this->m_rewardHonor = 0;
    this->m_rewardHonorBonus = 0.0f;
    this->m_startingItemId = 0;
    this->m_flags = 0;
    this->m_minimapTargetMark = 0;
    this->m_rewardTitleId = 0;
    this->m_requiredPlayersKilled = 0;
    this->m_rewardTalentPoints = 0;
    this->m_rewardArenaPoints = 0;
    this->m_rewardSkillId = 0;
    this->m_rewardSkillPoints = 0;
    this->m_questGiverPortrait = 0;
    this->m_questTurnInPortrait = 0;

    for (uint32_t i = 0; i < 5; i++) {
        this->m_rewardReputationFaction[i] = 0;
    }

    for (uint32_t i = 0; i < 5; i++) {
        this->m_factionRewardId[i] = 0;
    }

    for (uint32_t i = 0; i < 5; i++) {
        this->m_rewardReputationOverride[i] = 0;
    }

    this->m_rewardReputationMask = 0;

    for (uint32_t i = 0; i < 4; i++) {
        this->m_rewardItem[i] = 0;
    }

    for (uint32_t i = 0; i < 4; i++) {
        this->m_rewardItemCount[i] = 1;
    }

    for (uint32_t i = 0; i < 6; i++) {
        this->m_rewardChoiceItem[i] = 0;
        this->m_rewardChoiceItemCount[i] = 1;
    }

    this->m_pointMapId = 0;
    this->m_pointX = 0.0f;
    this->m_pointY = 0.0f;
    this->m_pointOptional = 0;
    this->m_name[0] = '\0';
    this->m_objText[0] = '\0';
    this->m_description[0] = '\0';
    this->m_endText[0] = '\0';
    this->m_completionText[0] = '\0';
    this->m_questGiverTextWindow[0] = '\0';
    this->m_questGiverTargetName[0] = '\0';
    this->m_questTurnTextWindow[0] = '\0';
    this->m_questTurnTargetName[0] = '\0';

    for (uint32_t i = 0; i < 4; i++) {
        this->m_objectiveId[i] = 0;
        this->m_objectiveRequiredCount[i] = 0;
        this->m_objectiveText[i][0] = '\0';
    }

    for (uint32_t i = 0; i < 4; i++) {
        this->m_intermediateItemId[i] = 0;
        this->m_intermediateItemCount[i] = 0;
    }

    for (uint32_t i = 0; i < 6; i++) {
        this->m_collectItemId[i] = 0;
        this->m_collectItemCount[i] = 0;
    }

    this->m_requiredSpell = 0;
    this->m_soundAccept = 0;
    this->m_soundTurnIn = 0;
}

// PROVEN: 9c0c0b16 IDENTICAL/DIFFERENT
// OFFSET: 0x0049B500/0x1400C72C0
QuestCache& QuestCache::operator=(const QuestCache& source) {
    this->m_id = source.m_id;
    this->m_method = source.m_method;
    this->m_level = source.m_level;
    this->m_requiredLevel = source.m_requiredLevel;
    this->m_areaIdOrSortId = source.m_areaIdOrSortId;
    this->m_infoId = source.m_infoId;
    this->m_suggestedPlayers = source.m_suggestedPlayers;
    this->m_friendlyFactionId = source.m_friendlyFactionId;
    this->m_friendlyFactionAmount = source.m_friendlyFactionAmount;
    this->m_hostileFactionId = source.m_hostileFactionId;
    this->m_hostileFactionAmount = source.m_hostileFactionAmount;
    this->m_nextQuestId = source.m_nextQuestId;
    this->m_xPId = source.m_xPId;
    this->m_rewardMoney = source.m_rewardMoney;
    this->m_rewardMoneyInsteadOfXp = source.m_rewardMoneyInsteadOfXp;
    this->m_rewardSpellId = source.m_rewardSpellId;
    this->m_effectOnPlayer = source.m_effectOnPlayer;
    this->m_rewardHonor = source.m_rewardHonor;
    this->m_rewardHonorBonus = source.m_rewardHonorBonus;
    this->m_startingItemId = source.m_startingItemId;
    this->m_flags = source.m_flags;
    this->m_minimapTargetMark = source.m_minimapTargetMark;
    this->m_rewardTitleId = source.m_rewardTitleId;
    this->m_requiredPlayersKilled = source.m_requiredPlayersKilled;
    this->m_rewardTalentPoints = source.m_rewardTalentPoints;
    this->m_rewardArenaPoints = source.m_rewardArenaPoints;
    this->m_rewardSkillId = source.m_rewardSkillId;
    this->m_rewardSkillPoints = source.m_rewardSkillPoints;
    this->m_rewardReputationMask = source.m_rewardReputationMask;
    this->m_questGiverPortrait = source.m_questGiverPortrait;
    this->m_questTurnInPortrait = source.m_questTurnInPortrait;

    for (uint32_t i = 0; i < 5; i++) {
        this->m_rewardReputationFaction[i] = source.m_rewardReputationFaction[i];
    }

    for (uint32_t i = 0; i < 5; i++) {
        this->m_factionRewardId[i] = source.m_factionRewardId[i];
    }

    for (uint32_t i = 0; i < 5; i++) {
        this->m_rewardReputationOverride[i] = source.m_rewardReputationOverride[i];
    }

    for (uint32_t i = 0; i < 4; i++) {
        this->m_rewardItem[i] = source.m_rewardItem[i];
        this->m_rewardItemCount[i] = source.m_rewardItemCount[i];
    }

    for (uint32_t i = 0; i < 6; i++) {
        this->m_rewardChoiceItem[i] = source.m_rewardChoiceItem[i];
        this->m_rewardChoiceItemCount[i] = source.m_rewardChoiceItemCount[i];
    }

    this->m_pointMapId = source.m_pointMapId;
    this->m_pointX = source.m_pointX;
    this->m_pointY = source.m_pointY;
    this->m_pointOptional = source.m_pointOptional;
    SStrCopy(this->m_name, source.m_name, sizeof(this->m_name));
    SStrCopy(this->m_objText, source.m_objText, sizeof(this->m_objText));
    SStrCopy(this->m_description, source.m_description, sizeof(this->m_description));
    SStrCopy(this->m_endText, source.m_endText, sizeof(this->m_endText));
    SStrCopy(this->m_completionText, source.m_completionText, sizeof(this->m_completionText));
    SStrCopy(this->m_questGiverTextWindow, source.m_questGiverTextWindow, sizeof(this->m_questGiverTextWindow));
    SStrCopy(this->m_questGiverTargetName, source.m_questGiverTargetName, sizeof(this->m_questGiverTargetName));
    SStrCopy(this->m_questTurnTextWindow, source.m_questTurnTextWindow, sizeof(this->m_questTurnTextWindow));
    SStrCopy(this->m_questTurnTargetName, source.m_questTurnTargetName, sizeof(this->m_questTurnTargetName));

    for (uint32_t i = 0; i < 4; i++) {
        this->m_objectiveId[i] = source.m_objectiveId[i];
        this->m_objectiveRequiredCount[i] = source.m_objectiveRequiredCount[i];
        this->m_intermediateItemId[i] = source.m_intermediateItemId[i];
        this->m_intermediateItemCount[i] = source.m_intermediateItemCount[i];
        SStrCopy(this->m_objectiveText[i], source.m_objectiveText[i], sizeof(this->m_objectiveText[i]));
    }

    for (uint32_t i = 0; i < 6; i++) {
        this->m_collectItemId[i] = source.m_collectItemId[i];
        this->m_collectItemCount[i] = source.m_collectItemCount[i];
    }

    this->m_requiredSpell = source.m_requiredSpell;

    for (uint32_t i = 0; i < 4; i++) {
        this->m_rewardCurrencyId[i] = source.m_rewardCurrencyId[i];
    }

    for (uint32_t i = 0; i < 4; i++) {
        this->m_rewardCurrencyCount[i] = source.m_rewardCurrencyCount[i];
    }

    for (uint32_t i = 0; i < 4; i++) {
        this->m_requiredCurrencyId[i] = source.m_requiredCurrencyId[i];
    }

    for (uint32_t i = 0; i < 4; i++) {
        this->m_requiredCurrencyCount[i] = source.m_requiredCurrencyCount[i];
    }

    this->m_soundAccept = source.m_soundAccept;
    this->m_soundTurnIn = source.m_soundTurnIn;

    return *this;
}

// PROVEN: 7e8b7f15 IDENTICAL/DIFFERENT
// OFFSET: 0x004EAF00/0x14012DF40
void QuestCache::Pack(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(this->m_id));
    msg->Put(this->m_method);
    msg->Put(this->m_level);
    msg->Put(this->m_requiredLevel);
    msg->Put(this->m_areaIdOrSortId);
    msg->Put(this->m_infoId);
    msg->Put(this->m_suggestedPlayers);
    msg->Put(this->m_friendlyFactionId);
    msg->Put(this->m_friendlyFactionAmount);
    msg->Put(this->m_hostileFactionId);
    msg->Put(this->m_hostileFactionAmount);
    msg->Put(this->m_nextQuestId);
    msg->Put(this->m_xPId);
    msg->Put(static_cast<uint32_t>(this->m_rewardMoney));
    msg->Put(this->m_rewardMoneyInsteadOfXp);
    msg->Put(this->m_rewardSpellId);
    msg->Put(this->m_effectOnPlayer);
    msg->Put(this->m_rewardHonor);
    msg->Put(this->m_rewardHonorBonus);
    msg->Put(this->m_startingItemId);
    msg->Put(this->m_flags);
    msg->Put(this->m_minimapTargetMark);
    msg->Put(this->m_rewardTitleId);
    msg->Put(this->m_requiredPlayersKilled);
    msg->Put(this->m_rewardTalentPoints);
    msg->Put(this->m_rewardArenaPoints);
    msg->Put(this->m_rewardSkillId);
    msg->Put(this->m_rewardSkillPoints);
    msg->Put(this->m_rewardReputationMask);
    msg->Put(this->m_questGiverPortrait);
    msg->Put(this->m_questTurnInPortrait);

    uint32_t count = 0;

    for (uint32_t i = 0; i < 4; i++) {
        if (this->m_rewardItem[i]) {
            msg->Put(this->m_rewardItem[i]);
            msg->Put(this->m_rewardItemCount[i]);
            count++;
        }
    }

    for (; count < 4; count++) {
        msg->Put(static_cast<uint32_t>(0));
        msg->Put(static_cast<uint32_t>(0));
    }

    count = 0;

    for (uint32_t i = 0; i < 6; i++) {
        if (this->m_rewardChoiceItem[i]) {
            msg->Put(this->m_rewardChoiceItem[i]);
            msg->Put(this->m_rewardChoiceItemCount[i]);
            count++;
        }
    }

    for (; count < 6; count++) {
        msg->Put(static_cast<uint32_t>(0));
        msg->Put(static_cast<uint32_t>(0));
    }

    for (uint32_t i = 0; i < 5; i++) {
        msg->Put(this->m_rewardReputationFaction[i]);
    }

    for (uint32_t i = 0; i < 5; i++) {
        msg->Put(this->m_factionRewardId[i]);
    }

    for (uint32_t i = 0; i < 5; i++) {
        msg->Put(static_cast<uint32_t>(this->m_rewardReputationOverride[i]));
    }

    msg->Put(this->m_pointMapId);
    msg->Put(this->m_pointX);
    msg->Put(this->m_pointY);
    msg->Put(this->m_pointOptional);
    msg->PutString(this->m_name);
    msg->PutString(this->m_objText);
    msg->PutString(this->m_description);
    msg->PutString(this->m_endText);
    msg->PutString(this->m_completionText);

    for (uint32_t i = 0; i < 4; i++) {
        msg->Put(this->m_objectiveId[i]);
        msg->Put(this->m_objectiveRequiredCount[i]);
        msg->Put(this->m_intermediateItemId[i]);
        msg->Put(this->m_intermediateItemCount[i]);
    }

    for (uint32_t i = 0; i < 6; i++) {
        msg->Put(this->m_collectItemId[i]);
        msg->Put(this->m_collectItemCount[i]);
    }

    msg->Put(this->m_requiredSpell);

    for (uint32_t i = 0; i < 4; i++) {
        msg->PutString(this->m_objectiveText[i]);
    }

    for (uint32_t i = 0; i < 4; i++) {
        msg->Put(this->m_rewardCurrencyId[i]);
        msg->Put(this->m_rewardCurrencyCount[i]);
    }

    for (uint32_t i = 0; i < 4; i++) {
        msg->Put(this->m_requiredCurrencyId[i]);
        msg->Put(this->m_requiredCurrencyCount[i]);
    }

    msg->PutString(this->m_questGiverTextWindow);
    msg->PutString(this->m_questGiverTargetName);
    msg->PutString(this->m_questTurnTextWindow);
    msg->PutString(this->m_questTurnTargetName);
    msg->Put(this->m_soundAccept);
    msg->Put(this->m_soundTurnIn);
}

// PROVEN: 0b09a536 IDENTICAL/DIFFERENT
// OFFSET: 0x004EB380/0x14012E3E0
void QuestCache::Unpack(CDataStore* msg) {
    msg->Get(reinterpret_cast<uint32_t&>(this->m_id));
    msg->Get(this->m_method);
    msg->Get(this->m_level);
    msg->Get(this->m_requiredLevel);
    msg->Get(this->m_areaIdOrSortId);
    msg->Get(this->m_infoId);
    msg->Get(this->m_suggestedPlayers);
    msg->Get(this->m_friendlyFactionId);
    msg->Get(this->m_friendlyFactionAmount);
    msg->Get(this->m_hostileFactionId);
    msg->Get(this->m_hostileFactionAmount);
    msg->Get(this->m_nextQuestId);
    msg->Get(this->m_xPId);
    msg->Get(reinterpret_cast<uint32_t&>(this->m_rewardMoney));
    msg->Get(this->m_rewardMoneyInsteadOfXp);
    msg->Get(this->m_rewardSpellId);
    msg->Get(this->m_effectOnPlayer);
    msg->Get(this->m_rewardHonor);
    msg->Get(this->m_rewardHonorBonus);
    msg->Get(this->m_startingItemId);
    msg->Get(this->m_flags);
    msg->Get(this->m_minimapTargetMark);
    msg->Get(this->m_rewardTitleId);
    msg->Get(this->m_requiredPlayersKilled);
    msg->Get(this->m_rewardTalentPoints);
    msg->Get(this->m_rewardArenaPoints);
    msg->Get(this->m_rewardSkillId);
    msg->Get(this->m_rewardSkillPoints);
    msg->Get(this->m_rewardReputationMask);
    msg->Get(this->m_questGiverPortrait);
    msg->Get(this->m_questTurnInPortrait);

    for (uint32_t i = 0; i < 4; i++) {
        msg->Get(this->m_rewardItem[i]);
        msg->Get(this->m_rewardItemCount[i]);
    }

    for (uint32_t i = 0; i < 6; i++) {
        msg->Get(this->m_rewardChoiceItem[i]);
        msg->Get(this->m_rewardChoiceItemCount[i]);
    }

    for (uint32_t i = 0; i < 5; i++) {
        msg->Get(this->m_rewardReputationFaction[i]);
    }

    for (uint32_t i = 0; i < 5; i++) {
        msg->Get(this->m_factionRewardId[i]);
    }

    for (uint32_t i = 0; i < 5; i++) {
        msg->Get(reinterpret_cast<uint32_t&>(this->m_rewardReputationOverride[i]));
    }

    msg->Get(this->m_pointMapId);
    msg->Get(this->m_pointX);
    msg->Get(this->m_pointY);
    msg->Get(this->m_pointOptional);
    msg->GetString(this->m_name, sizeof(this->m_name));
    msg->GetString(this->m_objText, sizeof(this->m_objText));
    msg->GetString(this->m_description, sizeof(this->m_description));
    msg->GetString(this->m_endText, sizeof(this->m_endText));
    msg->GetString(this->m_completionText, 0x7FFFFFFF);

    for (uint32_t i = 0; i < 4; i++) {
        msg->Get(this->m_objectiveId[i]);
        msg->Get(this->m_objectiveRequiredCount[i]);
        msg->Get(this->m_intermediateItemId[i]);
        msg->Get(this->m_intermediateItemCount[i]);
    }

    for (uint32_t i = 0; i < 6; i++) {
        msg->Get(this->m_collectItemId[i]);
        msg->Get(this->m_collectItemCount[i]);
    }

    msg->Get(this->m_requiredSpell);

    for (uint32_t i = 0; i < 4; i++) {
        msg->GetString(this->m_objectiveText[i], 0x7FFFFFFF);
    }

    for (uint32_t i = 0; i < 4; i++) {
        msg->Get(this->m_rewardCurrencyId[i]);
        msg->Get(this->m_rewardCurrencyCount[i]);
    }

    for (uint32_t i = 0; i < 4; i++) {
        msg->Get(this->m_requiredCurrencyId[i]);
        msg->Get(this->m_requiredCurrencyCount[i]);
    }

    msg->GetString(this->m_questGiverTextWindow, 0x7FFFFFFF);
    msg->GetString(this->m_questGiverTargetName, 0x7FFFFFFF);
    msg->GetString(this->m_questTurnTextWindow, 0x7FFFFFFF);
    msg->GetString(this->m_questTurnTargetName, 0x7FFFFFFF);
    msg->Get(this->m_soundAccept);
    msg->Get(this->m_soundTurnIn);
}

#include "LFGListParty.h"
#include <cstring>
#include "Engine/Source/Storm/H/storm.h"

// OFFSET: 0x0097F9E0
LFGListPlayer* LFGListParty::GetMember(uint32_t index) const {
    if (index + 1 < this->m_members.m_count) {
        return this->m_members.m_data[index + 1];
    }

    return NULL;
}

// OFFSET: 0x00984190/0x1406FF190
void LFGListParty::AddPlayer(LFGListPlayer* player) {
    if (player->m_partyIndex >= 0) {
        return;
    }

    uint32_t count = this->m_members.Count();
    this->m_members.SetCount(count + 1);

    if (player->m_leader) {
        for (int32_t i = count; i > 0; i--) {
            this->m_members.m_data[i] = this->m_members.m_data[i - 1];
            this->m_members.m_data[i]->m_partyIndex = i;
        }

        this->m_members.m_data[0] = player;
        player->m_partyIndex = 0;
    } else {
        this->m_members.m_data[count] = player;
        player->m_partyIndex = count;
    }
}

// OFFSET: 0x00984220
void LFGListParty::RemovePlayer(LFGListPlayer* player) {
    int32_t index = player->m_partyIndex;

    if (index < 0) {
        return;
    }

    uint32_t count = this->m_members.Count();

    for (; index < static_cast<int32_t>(count - 1); index++) {
        this->m_members.m_data[index] = this->m_members.m_data[index + 1];
        this->m_members.m_data[index]->m_partyIndex = index;
    }

    this->m_members.SetCount(count - 1);
    player->m_partyIndex = -1;
}

// OFFSET: 0x00986580/0x140701B10
LFGListParty::LFGListParty(const LFGListParty& source) {
    this->m_index = -1;
    SStrCopy(this->m_comment, source.m_comment, sizeof(this->m_comment));
    this->m_roles[0] = source.m_roles[0];
    this->m_roles[1] = source.m_roles[1];
    this->m_roles[2] = source.m_roles[2];
    this->unk_0128 = source.unk_0128;
    this->m_encounterMask = source.m_encounterMask;
}

// OFFSET: 0x00986640/0x140701BD0
LFGListPlayer::LFGListPlayer(const LFGListPlayer& source) {
    this->m_index = source.m_index;
    this->m_partyIndex = source.m_partyIndex;
    this->m_party = source.m_party;
    this->m_level = source.m_level;
    this->m_class = source.m_class;
    this->m_race = source.m_race;
    memcpy(this->m_talents, source.m_talents, sizeof(this->m_talents));
    this->m_armor = source.m_armor;
    this->m_spellDamage = source.m_spellDamage;
    this->m_spellHeal = source.m_spellHeal;
    this->m_critMelee = source.m_critMelee;
    this->m_critRanged = source.m_critRanged;
    this->m_critSpell = source.m_critSpell;
    this->m_mp5 = source.m_mp5;
    this->m_mp5Combat = source.m_mp5Combat;
    this->m_attackPower = source.m_attackPower;
    this->m_agility = source.m_agility;
    this->m_maxHealth = source.m_maxHealth;
    this->m_maxMana = source.m_maxMana;
    this->unk_0064 = source.unk_0064;
    this->m_gearRating = source.m_gearRating;
    this->m_avgItemLevel = source.m_avgItemLevel;
    this->m_defenseSkill = source.m_defenseSkill;
    this->m_dodge = source.m_dodge;
    this->m_block = source.m_block;
    this->m_parry = source.m_parry;
    this->m_haste = source.m_haste;
    this->m_expertise = source.m_expertise;
    this->m_areaID = source.m_areaID;
    this->unk_008C = source.unk_008C;
    this->m_leader = source.m_leader;
    this->m_roles = source.m_roles;
    memcpy(this->m_comment, source.m_comment, sizeof(this->m_comment));
    this->unk_0190 = source.unk_0190;
    this->m_encounterMask = source.m_encounterMask;
}

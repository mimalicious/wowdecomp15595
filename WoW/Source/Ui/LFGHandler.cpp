#include "LFGHandler.h"
#include "Engine/Source/Storm/H/storm.h"
#include "WoW/Source/JamClientAutoCode/JamClient.h"
#include "WoW/Source/Ui/LFGLIST.h"

// OFFSET: 0x00980050
void LFGHandler::ReadGroupInfo(const JamLfgSearchResultParty* source, LFGListParty* party) {
    if (source->unk_0008 & 0x2) {
        SStrCopy(party->m_comment, source->m_comment, sizeof(party->m_comment));
    }

    if (source->unk_0008 & 0x10) {
        party->m_roles[0] = source->unk_011C[0];
        party->m_roles[1] = source->unk_011C[1];
        party->m_roles[2] = source->unk_011C[2];
    }

    if (source->unk_0008 & 0x80) {
        party->unk_0128 = *reinterpret_cast<const uint64_t*>(source->m_leaderGUID);
        party->m_encounterMask = source->unk_0118;
    }
}

// OFFSET: 0x00986A30
void LFGHandler::ReadPlayerInfo(const JamLfgSearchResultPlayer* source, LFGLIST* list, LFGListPlayer* player) {
    if (source->unk_0008 & 0x1) {
        player->m_level = source->unk_000C;
        player->m_class = source->unk_000D;
        player->m_race = source->unk_000E;
        player->m_talents[0] = source->unk_018C;
        player->m_talents[1] = source->unk_018D;
        player->m_talents[2] = source->unk_018E;
        player->m_armor = source->unk_0010;
        player->m_spellDamage = source->unk_0014;
        player->m_spellHeal = source->unk_0018;
        player->m_critMelee = source->unk_001C;
        player->m_critRanged = source->unk_0020;
        player->m_critSpell = source->unk_0024;
        player->m_mp5 = source->unk_0028;
        player->m_mp5Combat = source->unk_002C;
        player->m_attackPower = source->unk_0030;
        player->m_agility = source->unk_0034;
        player->m_maxHealth = source->unk_0038;
        player->m_maxMana = source->unk_003C;
        player->unk_0064 = source->unk_0040;
        player->m_gearRating = source->unk_0044;
        player->m_avgItemLevel = source->unk_0048;
        player->m_defenseSkill = source->unk_004C;
        player->m_dodge = source->unk_0050;
        player->m_block = source->unk_0054;
        player->m_parry = source->unk_0058;
        player->m_haste = source->unk_005C;
        player->m_expertise = source->unk_0060;
    }

    if (source->unk_0008 & 0x2) {
        SStrCopy(player->m_comment, source->m_comment, sizeof(player->m_comment));
    }

    if (source->unk_0008 & 0x4) {
        uint8_t leader = source->unk_0164;
        uint8_t wasLeader = player->m_leader;
        player->m_leader = leader;

        if ((wasLeader != 0) != (leader != 0) && player->m_party) {
            player->m_party->RemovePlayer(player);
            player->m_party->AddPlayer(player);
        }
    }

    if (source->unk_0008 & 0x8) {
        uint64_t current = player->m_party ? player->m_party->m_key.m_guid.m_guid : 0;
        uint64_t guid = *reinterpret_cast<const uint64_t*>(source->m_partyGUID);

        if (current != guid) {
            list->RemovePlayer(player);
            player->m_party = guid ? list->m_parties.Ptr(static_cast<uint32_t>(guid), CHashKeyGUID(guid)) : NULL;
            list->AddPlayer(player);
        }
    }

    if (source->unk_0008 & 0x10) {
        player->m_roles = source->unk_0170;
    }

    if (source->unk_0008 & 0x20) {
        player->m_areaID = source->unk_0174;
    }

    if (source->unk_0008 & 0x40) {
        player->unk_008C = source->unk_0178;
    }

    if (source->unk_0008 & 0x80) {
        player->unk_0190 = *reinterpret_cast<const uint64_t*>(source->m_guildGUID);
        player->m_encounterMask = source->unk_0188;
    }
}

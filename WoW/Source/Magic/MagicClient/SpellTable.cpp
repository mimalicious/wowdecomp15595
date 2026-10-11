#include "SpellTable.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/SkillLineAbilityRec.h"
#include "WoW/Source/DB/Rec/SkillLineAbilitySortedSpellRec.h"
#include "WoW/Source/DB/Rec/SkillLineRec.h"
#include "WoW/Source/DB/Rec/SkillRaceClassInfoRec.h"
#include "WoW/Source/DB/Rec/SkillTiersRec.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/Object/Types.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"

// STATIC: 0x00ED67C4
static bool s_skillLineLinkInfoDirty;

// STATIC: 0x00ED67C8
static TSHashTable<RACECLASSINFO, HASHKEY_NONE> s_raceClassInfo;

// STATIC: 0x00ED67F4
static TSGrowableArray<SkillLineLinkInfo> s_skillLineLinkInfo;

// STATIC: 0x00ED6804
static bool s_destroyed;

// OFFSET: 0x008C0420/0x14060A220
bool SpellTableMatchRaceClass(int32_t race, int32_t classId, int32_t raceMask, int32_t classMask, int32_t excludeRace, int32_t excludeClass) {
    if (excludeRace) {
        raceMask = ~raceMask;
    }

    if (excludeClass) {
        classMask = ~classMask;
    }

    return (!raceMask || ((1 << (static_cast<int8_t>(race) - 1)) & raceMask)) && (!classMask || ((1 << (static_cast<int8_t>(classId) - 1)) & classMask));
}

// OFFSET: 0x008C0520/0x14060A350
SkillLineAbilityRec* SpellTableLookupPetAbility(int32_t skillLine, int32_t spellId) {
    for (int32_t i = 0; i < g_skillLineAbilityDB.m_numRecords; i++) {
        SkillLineAbilityRec* ability = g_skillLineAbilityDB.GetRecordByIndex(i);

        if (ability->m_skillLine == skillLine && ability->m_spell == spellId) {
            return ability;
        }
    }

    return NULL;
}

// OFFSET: 0x008C0570/0x14060A3A0
bool SpellTableGetSkillLineLinkInfo(int32_t skillLine, const SkillLineAbilityRec*& ability, uint32_t& count) {
    for (uint32_t i = 0; i < s_skillLineLinkInfo.Count(); i++) {
        if (s_skillLineLinkInfo.m_data[i].m_ability->m_skillLine == skillLine) {
            ability = s_skillLineLinkInfo.m_data[i].m_ability;
            count = s_skillLineLinkInfo.m_data[i].m_count;
            return true;
        }
    }

    return false;
}

// OFFSET: 0x008C05C0/0x14060A490
static void SpellTableBuildSkillLineLinkInfo() {
    SkillLineLinkInfo* link = NULL;
    int32_t lastSkillLine = -1;

    for (int32_t i = 0; i < g_skillLineAbilityDB.m_numRecords; i++) {
        SkillLineAbilityRec* ability = g_skillLineAbilityDB.GetRecordByIndex(i);

        if (lastSkillLine == ability->m_skillLine) {
            if (link) {
                link->m_count++;
            }

            continue;
        }

        SkillLineRec* skillLine = g_skillLineDB.GetRecord(ability->m_skillLine);

        if (skillLine && skillLine->m_canLink) {
            link = s_skillLineLinkInfo.New();
            link->m_ability = ability;
            link->m_count = 1;
            lastSkillLine = ability->m_skillLine;
        } else {
            link = NULL;
        }
    }

    s_skillLineLinkInfoDirty = false;
}

// OFFSET: 0x008C06E0/0x14060A580
const SkillRaceClassInfoRec* SpellTableLookupInfo(int32_t race, int32_t classId, int32_t skillId) {
    HASHKEY_NONE key;
    RACECLASSINFO* info = s_raceClassInfo.Ptr(skillId, key);

    if (!info) {
        return NULL;
    }

    const SkillRaceClassInfoRec* rec = info->m_info;

    for (int32_t i = 0; i < info->m_count; i++, rec++) {
        if ((!rec->m_raceMask || ((1 << (static_cast<int8_t>(race) - 1)) & rec->m_raceMask)) && (!rec->m_classMask || ((1 << (static_cast<int8_t>(classId) - 1)) & rec->m_classMask))) {
            return rec;
        }
    }

    return NULL;
}

// OFFSET: 0x008C0750/0x14060A640
SkillLineAbilityRec* SpellTableLookupAbility(int32_t race, int32_t classId, int32_t spellId) {
    if (s_skillLineLinkInfoDirty) {
        SpellTableBuildSkillLineLinkInfo();
    }

    if (!ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__)) {
        return NULL;
    }

    uint32_t low = 0;
    uint32_t high = g_skillLineAbilityDB.m_numRecords;

    do {
        if (low >= static_cast<uint32_t>(g_skillLineAbilityDB.m_numRecords)) {
            break;
        }

        int32_t mid = static_cast<int32_t>((high + low) >> 1);
        SkillLineAbilitySortedSpellRec* sorted = g_skillLineAbilitySortedSpellDB.GetRecordByIndex(mid);

        if (!sorted) {
            break;
        }

        if (spellId == sorted->m_spell) {
            while (mid > 0) {
                if (g_skillLineAbilitySortedSpellDB.GetRecordByIndex(mid - 1)->m_spell != spellId) {
                    break;
                }

                mid--;
            }

            for (int32_t i = mid; i < g_skillLineAbilityDB.m_numRecords; i++) {
                if (g_skillLineAbilitySortedSpellDB.GetRecordByIndex(i)->m_spell != spellId) {
                    break;
                }

                SkillLineAbilityRec* ability = g_skillLineAbilityDB.GetRecord(g_skillLineAbilitySortedSpellDB.GetRecordByIndex(i)->m_id);

                if (ability->m_spell == spellId && SpellTableLookupInfo(race, classId, ability->m_skillLine) && SpellTableMatchRaceClass(race, classId, ability->m_raceMask, ability->m_classMask, ability->m_excludeRace, ability->m_excludeClass)) {
                    return ability;
                }
            }

            return NULL;
        }

        if (spellId >= sorted->m_spell) {
            low = mid + 1;
        } else {
            high = mid - 1;
        }
    } while (low <= high);

    return NULL;
}

// OFFSET: 0x008C0A60
void SpellTableDestroy() {
    s_destroyed = true;
    s_raceClassInfo.Clear();
    s_skillLineLinkInfo.m_count = 0;
}

// OFFSET: 0x008C1110/0x14060B350
void SpellTableInitialize() {
    RACECLASSINFO* raceClassInfo = NULL;
    int32_t lastSkill = -1;

    for (int32_t i = 0; i < g_skillRaceClassInfoDB.m_numRecords; i++) {
        SkillRaceClassInfoRec* rec = g_skillRaceClassInfoDB.GetRecordByIndex(i);

        if (rec->m_skillID == lastSkill) {
            raceClassInfo->m_count++;
            continue;
        }

        HASHKEY_NONE key;
        raceClassInfo = s_raceClassInfo.New(rec->m_skillID, key, 0, 0);
        raceClassInfo->m_info = rec;
        raceClassInfo->m_count = 1;
        lastSkill = rec->m_skillID;
    }

    s_skillLineLinkInfoDirty = true;
}


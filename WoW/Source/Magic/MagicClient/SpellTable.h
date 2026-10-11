#ifndef WOW_SOURCE_MAGIC_MAGICCLIENT_SPELLTABLE_H
#define WOW_SOURCE_MAGIC_MAGICCLIENT_SPELLTABLE_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Storm/H/stpl.h"

class SkillLineAbilityRec;
class SkillRaceClassInfoRec;

struct RACECLASSINFO : public TSHashObject<RACECLASSINFO, HASHKEY_NONE> {
    /* 0018 */ const SkillRaceClassInfoRec* m_info;
    /* 001C */ int32_t m_count;
};

struct SkillLineLinkInfo {
    /* 0000 */ const SkillLineAbilityRec* m_ability;
    /* 0004 */ uint32_t m_count;
};

void SpellTableInitialize();
void SpellTableDestroy();
bool SpellTableMatchRaceClass(int32_t race, int32_t classId, int32_t raceMask, int32_t classMask, int32_t excludeRace, int32_t excludeClass);
SkillLineAbilityRec* SpellTableLookupPetAbility(int32_t skillLine, int32_t spellId);
bool SpellTableGetSkillLineLinkInfo(int32_t skillLine, const SkillLineAbilityRec*& ability, uint32_t& count);
const SkillRaceClassInfoRec* SpellTableLookupInfo(int32_t race, int32_t classId, int32_t skillId);
SkillLineAbilityRec* SpellTableLookupAbility(int32_t race, int32_t classId, int32_t spellId);

#endif

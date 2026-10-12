#include "WoW/Source/DB/Rec/SkillLineAbilityRec.h"

// STATIC: 0x00C2254C/0x140A17420
static const int32_t s_fieldOffsets[14] = { 0, 4, 8, 12, 16, 20, 24, 28, 32, 36, 40, 44, 48, 52 };

// STATIC: 0x00C22584/0x140A17458
static const int32_t s_fieldSizes[14] = { 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4 };

// STATIC: 0x00C225BC/0x140A17490
static const int32_t s_fieldTypes[14] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

// STATIC: 0x00C225F4/0x140A174C8
const DBCMeta SkillLineAbilityRec::s_meta = { "DBFilesClient\\SkillLineAbility.dbc", 14, 56, SkillLineAbilityRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// OFFSET: 0x00401000/0x1407D7E80
void SkillLineAbilityRec::FixupStrings(void* record, const char* strings) {
}

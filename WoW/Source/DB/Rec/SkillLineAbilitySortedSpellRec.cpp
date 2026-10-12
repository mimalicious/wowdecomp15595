#include "WoW/Source/DB/Rec/SkillLineAbilitySortedSpellRec.h"

// STATIC: 0x00C224F0/0x140A173A8
static const int32_t s_fieldOffsets[2] = { 0, 4 };

// STATIC: 0x00C224F8/0x140A173B0
static const int32_t s_fieldSizes[2] = { 4, 4 };

// STATIC: 0x00C22500/0x140A173B8
static const int32_t s_fieldTypes[2] = { 0, 0 };

// STATIC: 0x00C22508/0x140A173C0
const DBCMeta SkillLineAbilitySortedSpellRec::s_meta = { "DBFilesClient\\SkillLineAbilitySortedSpell.dbc", 2, 8, SkillLineAbilitySortedSpellRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// OFFSET: 0x00401000/0x1407D7E80
void SkillLineAbilitySortedSpellRec::FixupStrings(void* record, const char* strings) {
}

#include "WoW/Source/DB/Rec/SkillTiersRec.h"

// STATIC: 0x00C222D0/0x140A170F0
static const int32_t s_fieldOffsets[3] = { 0, 4, 68 };

// STATIC: 0x00C222DC/0x140A17100
static const int32_t s_fieldSizes[3] = { 4, 64, 64 };

// STATIC: 0x00C222E8/0x140A17110
static const int32_t s_fieldTypes[3] = { 0, 0, 0 };

// STATIC: 0x00C222F4/0x140A17120
const DBCMeta SkillTiersRec::s_meta = { "DBFilesClient\\SkillTiers.dbc", 3, 132, SkillTiersRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// OFFSET: 0x00401000/0x1407D7E80
void SkillTiersRec::FixupStrings(void* record, const char* strings) {
}

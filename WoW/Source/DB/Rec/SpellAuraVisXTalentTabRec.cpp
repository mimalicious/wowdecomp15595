#include "WoW/Source/DB/Rec/SpellAuraVisXTalentTabRec.h"

// STATIC: 0x00C21F38/0x140A16C78
static const int32_t s_fieldOffsets[3] = { 0, 4, 8 };

// STATIC: 0x00C21F44/0x140A16C88
static const int32_t s_fieldSizes[3] = { 4, 4, 4 };

// STATIC: 0x00C21F50/0x140A16C98
static const int32_t s_fieldTypes[3] = { 0, 0, 0 };

// STATIC: 0x00C21F5C/0x140A16CA8
const DBCMeta SpellAuraVisXTalentTabRec::s_meta = { "DBFilesClient\\SpellAuraVisXTalentTab.dbc", 3, 12, SpellAuraVisXTalentTabRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// OFFSET: 0x00401000/0x1407D7E80
void SpellAuraVisXTalentTabRec::FixupStrings(void* record, const char* strings) {
}

#include "WoW/Source/DB/Rec/SpellCooldownsRec.h"

// STATIC: 0x00C21998/0x140A165F0
static const int32_t s_fieldOffsets[4] = { 0, 4, 8, 12 };

// STATIC: 0x00C219A8/0x140A16600
static const int32_t s_fieldSizes[4] = { 4, 4, 4, 4 };

// STATIC: 0x00C219B8/0x140A16610
static const int32_t s_fieldTypes[4] = { 0, 0, 0, 0 };

// STATIC: 0x00C219C8/0x140A16620
const DBCMeta SpellCooldownsRec::s_meta = { "DBFilesClient\\SpellCooldowns.dbc", 4, 16, SpellCooldownsRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// OFFSET: 0x00401000/0x1407D7E80
void SpellCooldownsRec::FixupStrings(void* record, const char* strings) {
}

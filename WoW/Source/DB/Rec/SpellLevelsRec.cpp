#include "WoW/Source/DB/Rec/SpellLevelsRec.h"

// STATIC: 0x00C21058/0x140A15AF0
static const int32_t s_fieldOffsets[4] = { 0, 4, 8, 12 };

// STATIC: 0x00C21068/0x140A15B00
static const int32_t s_fieldSizes[4] = { 4, 4, 4, 4 };

// STATIC: 0x00C21078/0x140A15B10
static const int32_t s_fieldTypes[4] = { 0, 0, 0, 0 };

// STATIC: 0x00C21088/0x140A15B20
const DBCMeta SpellLevelsRec::s_meta = { "DBFilesClient\\SpellLevels.dbc", 4, 16, SpellLevelsRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// OFFSET: 0x00401000/0x1407D7E80
void SpellLevelsRec::FixupStrings(void* record, const char* strings) {
}

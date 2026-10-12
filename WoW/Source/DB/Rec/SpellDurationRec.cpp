#include "WoW/Source/DB/Rec/SpellDurationRec.h"

// STATIC: 0x00C21570/0x140A16130
static const int32_t s_fieldOffsets[4] = { 0, 4, 8, 12 };

// STATIC: 0x00C21580/0x140A16140
static const int32_t s_fieldSizes[4] = { 4, 4, 4, 4 };

// STATIC: 0x00C21590/0x140A16150
static const int32_t s_fieldTypes[4] = { 0, 0, 0, 0 };

// STATIC: 0x00C215A0/0x140A16160
const DBCMeta SpellDurationRec::s_meta = { "DBFilesClient\\SpellDuration.dbc", 4, 16, SpellDurationRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// OFFSET: 0x00401000/0x1407D7E80
void SpellDurationRec::FixupStrings(void* record, const char* strings) {
}

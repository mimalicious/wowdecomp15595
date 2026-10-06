#include "WoW/Source/DB/Rec/gtCombatRatingsRec.h"

// STATIC: 0x00C24FC4/0x140A1AA00
static const int32_t s_fieldOffsets[1] = { 4 };

// STATIC: 0x00C24FC8/0x140A1AA04
static const int32_t s_fieldSizes[1] = { 4 };

// STATIC: 0x00C24FCC/0x140A1AA08
static const int32_t s_fieldTypes[1] = { 3 };

// STATIC: 0x00C24FD0/0x140A1AA10
const DBCMeta gtCombatRatingsRec::s_meta = { "DBFilesClient\\gtCombatRatings.dbc", 1, 8, gtCombatRatingsRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// PROVEN: 13741388 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void gtCombatRatingsRec::FixupStrings(void* record, const char* strings) {
}

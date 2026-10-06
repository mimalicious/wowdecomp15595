#include "WoW/Source/DB/Rec/CreatureSpellDataRec.h"

// STATIC: 0x00C25FD8/0x140A1BDF8
static const int32_t s_fieldOffsets[3] = { 0, 4, 20 };

// STATIC: 0x00C25FE4/0x140A1BE08
static const int32_t s_fieldSizes[3] = { 4, 16, 16 };

// STATIC: 0x00C25FF0/0x140A1BE18
static const int32_t s_fieldTypes[3] = { 0, 0, 0 };

// STATIC: 0x00C25FFC/0x140A1BE28
const DBCMeta CreatureSpellDataRec::s_meta = { "DBFilesClient\\CreatureSpellData.dbc", 3, 36, CreatureSpellDataRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// PROVEN: 86e4a389 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void CreatureSpellDataRec::FixupStrings(void* record, const char* strings) {
}

#include "WoW/Source/DB/Rec/gtChanceToMeleeCritRec.h"

// STATIC: 0x00C24F74/0x140A1A990
static const int32_t s_fieldOffsets[1] = { 4 };

// STATIC: 0x00C24F78/0x140A1A994
static const int32_t s_fieldSizes[1] = { 4 };

// STATIC: 0x00C24F7C/0x140A1A998
static const int32_t s_fieldTypes[1] = { 3 };

// STATIC: 0x00C24F80/0x140A1A9A0
const DBCMeta gtChanceToMeleeCritRec::s_meta = { "DBFilesClient\\gtChanceToMeleeCrit.dbc", 1, 8, gtChanceToMeleeCritRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// OFFSET: 0x00401000/0x1407D7E80
void gtChanceToMeleeCritRec::FixupStrings(void* record, const char* strings) {
}

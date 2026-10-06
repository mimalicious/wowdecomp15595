#include "WoW/Source/DB/Rec/gtChanceToMeleeCritBaseRec.h"

// STATIC: 0x00C24F20/0x140A1A920
static const int32_t s_fieldOffsets[1] = { 4 };

// STATIC: 0x00C24F24/0x140A1A924
static const int32_t s_fieldSizes[1] = { 4 };

// STATIC: 0x00C24F28/0x140A1A928
static const int32_t s_fieldTypes[1] = { 3 };

// STATIC: 0x00C24F2C/0x140A1A930
const DBCMeta gtChanceToMeleeCritBaseRec::s_meta = { "DBFilesClient\\gtChanceToMeleeCritBase.dbc", 1, 8, gtChanceToMeleeCritBaseRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// PROVEN: 2734f430 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void gtChanceToMeleeCritBaseRec::FixupStrings(void* record, const char* strings) {
}

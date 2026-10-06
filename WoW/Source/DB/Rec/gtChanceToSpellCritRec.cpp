#include "WoW/Source/DB/Rec/gtChanceToSpellCritRec.h"

// STATIC: 0x00C24EC8/0x140A1A8A8
static const int32_t s_fieldOffsets[1] = { 4 };

// STATIC: 0x00C24ECC/0x140A1A8AC
static const int32_t s_fieldSizes[1] = { 4 };

// STATIC: 0x00C24ED0/0x140A1A8B0
static const int32_t s_fieldTypes[1] = { 3 };

// STATIC: 0x00C24ED4/0x140A1A8B8
const DBCMeta gtChanceToSpellCritRec::s_meta = { "DBFilesClient\\gtChanceToSpellCrit.dbc", 1, 8, gtChanceToSpellCritRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// PROVEN: 6340825f IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void gtChanceToSpellCritRec::FixupStrings(void* record, const char* strings) {
}

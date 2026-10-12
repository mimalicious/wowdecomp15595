#include "WoW/Source/DB/Rec/gtChanceToSpellCritBaseRec.h"

// STATIC: 0x00C24E74/0x140A1A838
static const int32_t s_fieldOffsets[1] = { 4 };

// STATIC: 0x00C24E78/0x140A1A83C
static const int32_t s_fieldSizes[1] = { 4 };

// STATIC: 0x00C24E7C/0x140A1A840
static const int32_t s_fieldTypes[1] = { 3 };

// STATIC: 0x00C24E80/0x140A1A848
const DBCMeta gtChanceToSpellCritBaseRec::s_meta = { "DBFilesClient\\gtChanceToSpellCritBase.dbc", 1, 8, gtChanceToSpellCritBaseRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// OFFSET: 0x00401000/0x1407D7E80
void gtChanceToSpellCritBaseRec::FixupStrings(void* record, const char* strings) {
}

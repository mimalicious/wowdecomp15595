#include "WoW/Source/DB/Rec/ArmorLocationRec.h"

// STATIC: 0x00C269D0/0x140A1CA10
static const int32_t s_fieldOffsets[6] = { 0, 4, 8, 12, 16, 20 };

// STATIC: 0x00C269E8/0x140A1CA28
static const int32_t s_fieldSizes[6] = { 4, 4, 4, 4, 4, 4 };

// STATIC: 0x00C26A00/0x140A1CA40
static const int32_t s_fieldTypes[6] = { 0, 3, 3, 3, 3, 3 };

// STATIC: 0x00C26A18/0x140A1CA58
const DBCMeta ArmorLocationRec::s_meta = { "DBFilesClient\\ArmorLocation.dbc", 6, 24, ArmorLocationRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// PROVEN: 708698ed IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void ArmorLocationRec::FixupStrings(void* record, const char* strings) {
}

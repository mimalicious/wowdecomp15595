#include "WoW/Source/DB/Rec/gtOCTBaseMPByClassRec.h"

// STATIC: 0x00C24D74/0x140A1A6E0
static const int32_t s_fieldOffsets[1] = { 4 };

// STATIC: 0x00C24D78/0x140A1A6E4
static const int32_t s_fieldSizes[1] = { 4 };

// STATIC: 0x00C24D7C/0x140A1A6E8
static const int32_t s_fieldTypes[1] = { 3 };

// STATIC: 0x00C24D80/0x140A1A6F0
const DBCMeta gtOCTBaseMPByClassRec::s_meta = { "DBFilesClient\\gtOCTBaseMPByClass.dbc", 1, 8, gtOCTBaseMPByClassRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// PROVEN: e2346dff IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void gtOCTBaseMPByClassRec::FixupStrings(void* record, const char* strings) {
}

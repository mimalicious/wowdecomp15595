#include "WoW/Source/DB/Rec/gtOCTBaseHPByClassRec.h"

// STATIC: 0x00C24DC8/0x140A1A750
static const int32_t s_fieldOffsets[1] = { 4 };

// STATIC: 0x00C24DCC/0x140A1A754
static const int32_t s_fieldSizes[1] = { 4 };

// STATIC: 0x00C24DD0/0x140A1A758
static const int32_t s_fieldTypes[1] = { 3 };

// STATIC: 0x00C24DD4/0x140A1A760
const DBCMeta gtOCTBaseHPByClassRec::s_meta = { "DBFilesClient\\gtOCTBaseHPByClass.dbc", 1, 8, gtOCTBaseHPByClassRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// OFFSET: 0x00401000/0x1407D7E80
void gtOCTBaseHPByClassRec::FixupStrings(void* record, const char* strings) {
}

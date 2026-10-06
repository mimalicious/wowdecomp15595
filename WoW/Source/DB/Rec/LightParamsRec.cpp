#include "WoW/Source/DB/Rec/LightParamsRec.h"

// STATIC: 0x00C28374/0x140A1EB28
static const int32_t s_fieldOffsets[10] = { 0, 4, 8, 12, 16, 20, 24, 28, 32, 36 };

// STATIC: 0x00C2839C/0x140A1EB50
static const int32_t s_fieldSizes[10] = { 4, 4, 4, 4, 4, 4, 4, 4, 4, 4 };

// STATIC: 0x00C283C4/0x140A1EB78
static const int32_t s_fieldTypes[10] = { 0, 0, 0, 0, 3, 3, 3, 3, 3, 0 };

// STATIC: 0x00C283EC/0x140A1EBA0
const DBCMeta LightParamsRec::s_meta = { "DBFilesClient\\LightParams.dbc", 10, 40, LightParamsRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// PROVEN: ff8f362a IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void LightParamsRec::FixupStrings(void* record, const char* strings) {
}

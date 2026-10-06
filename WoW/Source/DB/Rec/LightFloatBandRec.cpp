#include "WoW/Source/DB/Rec/LightFloatBandRec.h"

// STATIC: 0x00C27FFC/0x140A1E6F8
static const int32_t s_fieldOffsets[4] = { 0, 4, 8, 72 };

// STATIC: 0x00C2800C/0x140A1E708
static const int32_t s_fieldSizes[4] = { 4, 4, 64, 64 };

// STATIC: 0x00C2801C/0x140A1E718
static const int32_t s_fieldTypes[4] = { 0, 0, 0, 3 };

// STATIC: 0x00C2802C/0x140A1E728
const DBCMeta LightFloatBandRec::s_meta = { "DBFilesClient\\LightFloatBand.dbc", 4, 136, LightFloatBandRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// PROVEN: 6e037d49 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void LightFloatBandRec::FixupStrings(void* record, const char* strings) {
}

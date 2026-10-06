#include "WoW/Source/DB/Rec/LiquidObjectRec.h"

// STATIC: 0x00C27EA4/0x140A1E538
static const int32_t s_fieldOffsets[6] = { 0, 4, 8, 12, 16, 20 };

// STATIC: 0x00C27EBC/0x140A1E550
static const int32_t s_fieldSizes[6] = { 4, 4, 4, 4, 4, 4 };

// STATIC: 0x00C27ED4/0x140A1E568
static const int32_t s_fieldTypes[6] = { 0, 3, 3, 0, 0, 0 };

// STATIC: 0x00C27EEC/0x140A1E580
const DBCMeta LiquidObjectRec::s_meta = { "DBFilesClient\\LiquidObject.dbc", 6, 24, LiquidObjectRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// PROVEN: eefa6274 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void LiquidObjectRec::FixupStrings(void* record, const char* strings) {
}

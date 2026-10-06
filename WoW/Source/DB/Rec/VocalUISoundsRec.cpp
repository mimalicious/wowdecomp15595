#include "WoW/Source/DB/Rec/VocalUISoundsRec.h"

// STATIC: 0x00C1F634/0x140A13BF0
static const int32_t s_fieldOffsets[5] = { 0, 4, 8, 12, 20 };

// STATIC: 0x00C1F648/0x140A13C08
static const int32_t s_fieldSizes[5] = { 4, 4, 4, 8, 8 };

// STATIC: 0x00C1F65C/0x140A13C20
static const int32_t s_fieldTypes[5] = { 0, 0, 0, 0, 0 };

// STATIC: 0x00C1F670/0x140A13C38
const DBCMeta VocalUISoundsRec::s_meta = { "DBFilesClient\\VocalUISounds.dbc", 5, 28, VocalUISoundsRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// PROVEN: 4871e926 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void VocalUISoundsRec::FixupStrings(void* record, const char* strings) {
}

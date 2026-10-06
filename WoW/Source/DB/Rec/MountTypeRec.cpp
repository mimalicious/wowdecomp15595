#include "WoW/Source/DB/Rec/MountTypeRec.h"

// STATIC: 0x00C232C4/0x140A184D8
static const int32_t s_fieldOffsets[2] = { 0, 4 };

// STATIC: 0x00C232CC/0x140A184E0
static const int32_t s_fieldSizes[2] = { 4, 96 };

// STATIC: 0x00C232D4/0x140A184E8
static const int32_t s_fieldTypes[2] = { 0, 0 };

// STATIC: 0x00C232DC/0x140A184F0
const DBCMeta MountTypeRec::s_meta = { "DBFilesClient\\MountType.dbc", 2, 100, MountTypeRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// PROVEN: 64849ff8 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void MountTypeRec::FixupStrings(void* record, const char* strings) {
}

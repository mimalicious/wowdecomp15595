#include "WoW/Source/DB/Rec/WeaponSwingSounds2Rec.h"

// STATIC: 0x00C1F4A8/0x140A13A00
static const int32_t s_fieldOffsets[4] = { 0, 4, 8, 12 };

// STATIC: 0x00C1F4B8/0x140A13A10
static const int32_t s_fieldSizes[4] = { 4, 4, 4, 4 };

// STATIC: 0x00C1F4C8/0x140A13A20
static const int32_t s_fieldTypes[4] = { 0, 0, 0, 0 };

// STATIC: 0x00C1F4D8/0x140A13A30
const DBCMeta WeaponSwingSounds2Rec::s_meta = { "DBFilesClient\\WeaponSwingSounds2.dbc", 4, 16, WeaponSwingSounds2Rec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// OFFSET: 0x00401000/0x1407D7E80
void WeaponSwingSounds2Rec::FixupStrings(void* record, const char* strings) {
}

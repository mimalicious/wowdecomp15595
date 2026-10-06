#include "WoW/Source/DB/Rec/WeaponImpactSoundsRec.h"

// STATIC: 0x00C1F520/0x140A13A90
static const int32_t s_fieldOffsets[5] = { 0, 4, 8, 12, 52 };

// STATIC: 0x00C1F534/0x140A13AA8
static const int32_t s_fieldSizes[5] = { 4, 4, 4, 40, 40 };

// STATIC: 0x00C1F548/0x140A13AC0
static const int32_t s_fieldTypes[5] = { 0, 0, 0, 0, 0 };

// STATIC: 0x00C1F55C/0x140A13AD8
const DBCMeta WeaponImpactSoundsRec::s_meta = { "DBFilesClient\\WeaponImpactSounds.dbc", 5, 92, WeaponImpactSoundsRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// PROVEN: 67adb663 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void WeaponImpactSoundsRec::FixupStrings(void* record, const char* strings) {
}

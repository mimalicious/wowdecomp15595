#include "WoW/Source/DB/Rec/VehicleSeatRec.h"

// STATIC: 0x00C1F790/0x140A13DB0
static const int32_t s_fieldOffsets[62] = { 0, 4, 8, 12, 24, 28, 32, 36, 40, 44, 48, 52, 56, 60, 64, 68, 72, 76, 80, 84, 88, 92, 96, 100, 104, 108, 112, 116, 120, 124, 128, 132, 136, 140, 144, 148, 152, 156, 160, 164, 168, 172, 176, 180, 184, 188, 192, 196, 200, 212, 216, 220, 224, 228, 232, 236, 240, 244, 248, 252, 256, 260 };

// STATIC: 0x00C1F888/0x140A13EE0
static const int32_t s_fieldSizes[62] = { 4, 4, 4, 12, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 12, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4 };

// STATIC: 0x00C1F980/0x140A13FE0
static const int32_t s_fieldTypes[62] = { 0, 0, 0, 3, 3, 3, 3, 3, 3, 3, 3, 0, 0, 0, 0, 0, 0, 3, 3, 3, 3, 3, 3, 3, 0, 0, 0, 3, 3, 3, 0, 0, 0, 0, 0, 0, 0, 3, 3, 0, 0, 0, 0, 0, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0 };

// STATIC: 0x00C1FA78/0x140A13EA8
const DBCMeta VehicleSeatRec::s_meta = { "DBFilesClient\\VehicleSeat.dbc", 62, 264, VehicleSeatRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// PROVEN: 5b7a1cc4 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void VehicleSeatRec::FixupStrings(void* record, const char* strings) {
}

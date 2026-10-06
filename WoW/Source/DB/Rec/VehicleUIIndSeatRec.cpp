#include "WoW/Source/DB/Rec/VehicleUIIndSeatRec.h"

// STATIC: 0x00C1F6B4/0x140A13C98
static const int32_t s_fieldOffsets[5] = { 0, 4, 8, 12, 16 };

// STATIC: 0x00C1F6C8/0x140A13CB0
static const int32_t s_fieldSizes[5] = { 4, 4, 4, 4, 4 };

// STATIC: 0x00C1F6DC/0x140A13CC8
static const int32_t s_fieldTypes[5] = { 0, 0, 0, 3, 3 };

// STATIC: 0x00C1F6F0/0x140A13CE0
const DBCMeta VehicleUIIndSeatRec::s_meta = { "DBFilesClient\\VehicleUIIndSeat.dbc", 5, 20, VehicleUIIndSeatRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// PROVEN: 80d4e2ed IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void VehicleUIIndSeatRec::FixupStrings(void* record, const char* strings) {
}

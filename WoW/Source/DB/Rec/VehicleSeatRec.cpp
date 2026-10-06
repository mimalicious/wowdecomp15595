#include "WoW/Source/DB/Rec/VehicleSeatRec.h"

// STATIC: 0x00C1FA78/0x140A13EA8
const DBCMeta VehicleSeatRec::s_meta = { "DBFilesClient\\VehicleSeat.dbc", 62, 264, VehicleSeatRec::FixupStrings, 0 };

// PROVEN: 5b7a1cc4 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void VehicleSeatRec::FixupStrings(void* record, const char* strings) {
}

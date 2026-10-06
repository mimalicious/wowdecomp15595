#include "WoW/Source/DB/Rec/VehicleUIIndSeatRec.h"

// STATIC: %0x00B446C8
const DBCMeta VehicleUIIndSeatRec::s_meta = { "DBFilesClient\\VehicleUIIndSeat.dbc", 5, 20, VehicleUIIndSeatRec::FixupStrings, 0 };

// PROVEN: 80d4e2ed IDENTICAL/NONE
// OFFSET: 0x00401000
void VehicleUIIndSeatRec::FixupStrings(void* record, const char* strings) {
}

#if defined(WOWDECOMP_ARCH_64)
void VehicleUIIndSeatRec::ConvertRow(VehicleUIIndSeatRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    memcpy(&rec->m_vehicleUiindicatorID, &row[0x0004], sizeof(rec->m_vehicleUiindicatorID));
    memcpy(&rec->m_virtualSeatIndex, &row[0x0008], sizeof(rec->m_virtualSeatIndex));
    memcpy(&rec->m_xpos, &row[0x000C], sizeof(rec->m_xpos));
    memcpy(&rec->m_ypos, &row[0x0010], sizeof(rec->m_ypos));
}
#endif

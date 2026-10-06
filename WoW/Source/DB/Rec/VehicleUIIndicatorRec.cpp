#include "WoW/Source/DB/Rec/VehicleUIIndicatorRec.h"

// STATIC: %0x00B44704
const DBCMeta VehicleUIIndicatorRec::s_meta = { "DBFilesClient\\VehicleUIIndicator.dbc", 2, 8, VehicleUIIndicatorRec::FixupStrings, 0 };

// OFFSET: %0x0097C040
void VehicleUIIndicatorRec::FixupStrings(void* record, const char* strings) {
    VehicleUIIndicatorRec* rec = static_cast<VehicleUIIndicatorRec*>(record);
    rec->m_backgroundTexture = &strings[reinterpret_cast<uintptr_t>(rec->m_backgroundTexture)];
}

#if defined(WOWDECOMP_ARCH_64)
void VehicleUIIndicatorRec::ConvertRow(VehicleUIIndicatorRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    rec->m_backgroundTexture = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0004])];
}
#endif

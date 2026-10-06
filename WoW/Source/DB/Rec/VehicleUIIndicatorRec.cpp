#include "WoW/Source/DB/Rec/VehicleUIIndicatorRec.h"

// STATIC: 0x00C1F750/0x140A13D58
const DBCMeta VehicleUIIndicatorRec::s_meta = { "DBFilesClient\\VehicleUIIndicator.dbc", 2, 8, VehicleUIIndicatorRec::FixupStrings, 0 };

// PROVEN: 1ec30822 IDENTICAL/NONE
// OFFSET: 0x00A2EBA0/0x1407D8330
void VehicleUIIndicatorRec::FixupStrings(void* record, const char* strings) {
    VehicleUIIndicatorRec* rec = static_cast<VehicleUIIndicatorRec*>(record);
    DBCFixupString(rec->m_backgroundTexture, strings);
}

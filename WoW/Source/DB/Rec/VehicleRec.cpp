#include "WoW/Source/DB/Rec/VehicleRec.h"

// STATIC: 0x00C1FC1C/0x140A14278
const DBCMeta VehicleRec::s_meta = { "DBFilesClient\\Vehicle.dbc", 29, 160, VehicleRec::FixupStrings, 0 };

// PROVEN: b18ec0da IDENTICAL/NONE
// OFFSET: 0x00A2E930/0x1407D7CB0
void VehicleRec::FixupStrings(void* record, const char* strings) {
    VehicleRec* rec = static_cast<VehicleRec*>(record);
    DBCFixupString(rec->m_msslTrgtArcTexture, strings);
    DBCFixupString(rec->m_msslTrgtImpactTexture, strings);
    DBCFixupString(rec->m_msslTrgtImpactModel[0], strings);
    DBCFixupString(rec->m_msslTrgtImpactModel[1], strings);
}

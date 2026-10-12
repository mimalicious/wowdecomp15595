#include "WoW/Source/DB/Rec/VehicleUIIndicatorRec.h"

// STATIC: 0x00C1F738/0x140A13D40
static const int32_t s_fieldOffsets[2] = { 0, 4 };

// STATIC: 0x00C1F740/0x140A13D48
static const int32_t s_fieldSizes[2] = { 4, 4 };

// STATIC: 0x00C1F748/0x140A13D50
static const int32_t s_fieldTypes[2] = { 0, 2 };

// STATIC: 0x00C1F750/0x140A13D58
const DBCMeta VehicleUIIndicatorRec::s_meta = { "DBFilesClient\\VehicleUIIndicator.dbc", 2, 8, VehicleUIIndicatorRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// PROVEN: 1ec30822 IDENTICAL/DIFFERENT
// OFFSET: 0x00A2EBA0/0x1407D8330
void VehicleUIIndicatorRec::FixupStrings(void* record, const char* strings) {
    VehicleUIIndicatorRec* rec = static_cast<VehicleUIIndicatorRec*>(record);
    DBCFixupString(rec->m_backgroundTexture, strings);
}

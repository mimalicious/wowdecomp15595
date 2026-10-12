#include "WoW/Source/DB/Rec/VehicleRec.h"

// STATIC: 0x00C1FAB8/0x140A14100
static const int32_t s_fieldOffsets[29] = { 0, 4, 8, 12, 16, 20, 24, 56, 60, 64, 68, 72, 76, 80, 84, 88, 92, 96, 100, 104, 108, 116, 120, 124, 132, 136, 140, 144, 148 };

// STATIC: 0x00C1FB30/0x140A14180
static const int32_t s_fieldSizes[29] = { 4, 4, 4, 4, 4, 4, 32, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 8, 4, 4, 8, 4, 4, 4, 4, 12 };

// STATIC: 0x00C1FBA8/0x140A14200
static const int32_t s_fieldTypes[29] = { 0, 0, 3, 3, 3, 3, 0, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2, 2, 2, 3, 0, 3, 0, 0 };

// STATIC: 0x00C1FC1C/0x140A14278
const DBCMeta VehicleRec::s_meta = { "DBFilesClient\\Vehicle.dbc", 29, 160, VehicleRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// PROVEN: b18ec0da IDENTICAL/DIFFERENT
// OFFSET: 0x00A2E930/0x1407D7CB0
void VehicleRec::FixupStrings(void* record, const char* strings) {
    VehicleRec* rec = static_cast<VehicleRec*>(record);
    DBCFixupString(rec->m_msslTrgtArcTexture, strings);
    DBCFixupString(rec->m_msslTrgtImpactTexture, strings);
    DBCFixupString(rec->m_msslTrgtImpactModel[0], strings);
    DBCFixupString(rec->m_msslTrgtImpactModel[1], strings);
}

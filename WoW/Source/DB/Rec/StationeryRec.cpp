#include "WoW/Source/DB/Rec/StationeryRec.h"

// STATIC: 0x00C2052C/0x140A14D90
static const int32_t s_fieldOffsets[4] = { 0, 4, 8, 12 };

// STATIC: 0x00C2053C/0x140A14DA0
static const int32_t s_fieldSizes[4] = { 4, 4, 4, 4 };

// STATIC: 0x00C2054C/0x140A14DB0
static const int32_t s_fieldTypes[4] = { 0, 0, 2, 0 };

// STATIC: 0x00C2055C/0x140A14DC0
const DBCMeta StationeryRec::s_meta = { "DBFilesClient\\Stationery.dbc", 4, 16, StationeryRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// OFFSET: 0x00A2ECA0/0x1407D8230
void StationeryRec::FixupStrings(void* record, const char* strings) {
    StationeryRec* rec = static_cast<StationeryRec*>(record);
    DBCFixupString(rec->m_texture, strings);
}

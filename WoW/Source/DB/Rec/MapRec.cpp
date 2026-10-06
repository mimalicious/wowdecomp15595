#include "WoW/Source/DB/Rec/MapRec.h"

// STATIC: 0x00C28248/0x140A1E9E0
static const int32_t s_fieldOffsets[19] = { 0, 4, 8, 12, 16, 20, 24, 28, 32, 36, 40, 44, 48, 52, 60, 64, 68, 72, 76 };

// STATIC: 0x00C28298/0x140A1EA30
static const int32_t s_fieldSizes[19] = { 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 8, 4, 4, 4, 4, 4 };

// STATIC: 0x00C282E8/0x140A1EA80
static const int32_t s_fieldTypes[19] = { 0, 2, 0, 0, 0, 0, 2, 0, 2, 2, 0, 3, 0, 3, 0, 0, 0, 0, 0 };

// STATIC: 0x00C28334/0x140A1EAD0
const DBCMeta MapRec::s_meta = { "DBFilesClient\\Map.dbc", 19, 80, MapRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// PROVEN: 1e9f9ca0 IDENTICAL/NONE
// OFFSET: 0x00A3B750/0x1407E8630
void MapRec::FixupStrings(void* record, const char* strings) {
    MapRec* rec = static_cast<MapRec*>(record);
    DBCFixupString(rec->m_directory, strings);
    DBCFixupString(rec->m_mapName, strings);
    DBCFixupString(rec->m_mapDescription0, strings);
    DBCFixupString(rec->m_mapDescription1, strings);
}

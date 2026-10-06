#include "WoW/Source/DB/Rec/MapRec.h"

// STATIC: 0x00C28334/0x140A1EAD0
const DBCMeta MapRec::s_meta = { "DBFilesClient\\Map.dbc", 19, 80, MapRec::FixupStrings, 0 };

// PROVEN: 1e9f9ca0 IDENTICAL/NONE
// OFFSET: 0x00A3B750/0x1407E8630
void MapRec::FixupStrings(void* record, const char* strings) {
    MapRec* rec = static_cast<MapRec*>(record);
    DBCFixupString(rec->m_directory, strings);
    DBCFixupString(rec->m_mapName, strings);
    DBCFixupString(rec->m_mapDescription0, strings);
    DBCFixupString(rec->m_mapDescription1, strings);
}

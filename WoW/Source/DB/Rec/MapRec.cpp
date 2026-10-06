#include "WoW/Source/DB/Rec/MapRec.h"

// STATIC: %0x00B4391C
const DBCMeta MapRec::s_meta = { "DBFilesClient\\Map.dbc", 19, 80, MapRec::FixupStrings, 0 };

// OFFSET: %0x0097BFC0
void MapRec::FixupStrings(void* record, const char* strings) {
    MapRec* rec = static_cast<MapRec*>(record);
    rec->m_directory = &strings[reinterpret_cast<uintptr_t>(rec->m_directory)];
    rec->m_mapName = &strings[reinterpret_cast<uintptr_t>(rec->m_mapName)];
    rec->m_mapDescription0 = &strings[reinterpret_cast<uintptr_t>(rec->m_mapDescription0)];
    rec->m_mapDescription1 = &strings[reinterpret_cast<uintptr_t>(rec->m_mapDescription1)];
}

#if defined(WOWDECOMP_ARCH_64)
void MapRec::ConvertRow(MapRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    rec->m_directory = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0004])];
    memcpy(&rec->m_instanceType, &row[0x0008], sizeof(rec->m_instanceType));
    memcpy(&rec->m_flags, &row[0x000C], sizeof(rec->m_flags));
    memcpy(&rec->m_pvp, &row[0x0010], sizeof(rec->m_pvp));
    memcpy(&rec->unk_0014, &row[0x0014], sizeof(rec->unk_0014));
    rec->m_mapName = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0018])];
    memcpy(&rec->m_areaTableID, &row[0x001C], sizeof(rec->m_areaTableID));
    rec->m_mapDescription0 = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0020])];
    rec->m_mapDescription1 = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0024])];
    memcpy(&rec->m_loadingScreenID, &row[0x0028], sizeof(rec->m_loadingScreenID));
    memcpy(&rec->m_minimapIconScale, &row[0x002C], sizeof(rec->m_minimapIconScale));
    memcpy(&rec->m_corpseMapID, &row[0x0030], sizeof(rec->m_corpseMapID));
    memcpy(rec->m_corpse, &row[0x0034], sizeof(rec->m_corpse));
    memcpy(&rec->m_timeOfDayOverride, &row[0x003C], sizeof(rec->m_timeOfDayOverride));
    memcpy(&rec->m_expansionID, &row[0x0040], sizeof(rec->m_expansionID));
    memcpy(&rec->m_raidOffset, &row[0x0044], sizeof(rec->m_raidOffset));
    memcpy(&rec->m_maxPlayers, &row[0x0048], sizeof(rec->m_maxPlayers));
    memcpy(&rec->unk_004C, &row[0x004C], sizeof(rec->unk_004C));
}
#endif

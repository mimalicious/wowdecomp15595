#include "WoW/Source/DB/Rec/WorldChunkSoundsRec.h"

// STATIC: %0x00B442F0
const DBCMeta WorldChunkSoundsRec::s_meta = { "DBFilesClient\\WorldChunkSounds.dbc", 9, 40, WorldChunkSoundsRec::FixupStrings, 0 };

// OFFSET: %0x00401000
void WorldChunkSoundsRec::FixupStrings(void* record, const char* strings) {
}

#if defined(WOWDECOMP_ARCH_64)
void WorldChunkSoundsRec::ConvertRow(WorldChunkSoundsRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_generatedID, &row[0x0000], sizeof(rec->m_generatedID));
    memcpy(&rec->m_id, &row[0x0004], sizeof(rec->m_id));
    memcpy(&rec->m_chunkX, &row[0x0008], sizeof(rec->m_chunkX));
    memcpy(&rec->m_chunkY, &row[0x000C], sizeof(rec->m_chunkY));
    memcpy(&rec->m_subchunkX, &row[0x0010], sizeof(rec->m_subchunkX));
    memcpy(&rec->m_subchunkY, &row[0x0014], sizeof(rec->m_subchunkY));
    memcpy(&rec->m_zoneIntroMusicID, &row[0x0018], sizeof(rec->m_zoneIntroMusicID));
    memcpy(&rec->m_zoneMusicID, &row[0x001C], sizeof(rec->m_zoneMusicID));
    memcpy(&rec->m_soundAmbienceID, &row[0x0020], sizeof(rec->m_soundAmbienceID));
    memcpy(&rec->m_soundProviderPreferencesID, &row[0x0024], sizeof(rec->m_soundProviderPreferencesID));
}
#endif

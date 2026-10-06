#include "WoW/Source/DB/Rec/WorldStateZoneSoundsRec.h"

// STATIC: %0x00B4432C
const DBCMeta WorldStateZoneSoundsRec::s_meta = { "DBFilesClient\\WorldStateZoneSounds.dbc", 8, 36, WorldStateZoneSoundsRec::FixupStrings, 0 };

// PROVEN: 5d3d235c IDENTICAL/NONE
// OFFSET: 0x00401000
void WorldStateZoneSoundsRec::FixupStrings(void* record, const char* strings) {
}

#if defined(WOWDECOMP_ARCH_64)
void WorldStateZoneSoundsRec::ConvertRow(WorldStateZoneSoundsRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    memcpy(&rec->m_worldStateID, &row[0x0004], sizeof(rec->m_worldStateID));
    memcpy(&rec->m_worldStateValue, &row[0x0008], sizeof(rec->m_worldStateValue));
    memcpy(&rec->m_areaID, &row[0x000C], sizeof(rec->m_areaID));
    memcpy(&rec->m_wmoareaID, &row[0x0010], sizeof(rec->m_wmoareaID));
    memcpy(&rec->m_zoneIntroMusicID, &row[0x0014], sizeof(rec->m_zoneIntroMusicID));
    memcpy(&rec->m_zoneMusicID, &row[0x0018], sizeof(rec->m_zoneMusicID));
    memcpy(&rec->m_soundAmbienceID, &row[0x001C], sizeof(rec->m_soundAmbienceID));
    memcpy(&rec->m_soundProviderPreferencesID, &row[0x0020], sizeof(rec->m_soundProviderPreferencesID));
}
#endif

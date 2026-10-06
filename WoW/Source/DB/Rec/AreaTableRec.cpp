#include "WoW/Source/DB/Rec/AreaTableRec.h"

// STATIC: %0x00B44084
const DBCMeta AreaTableRec::s_meta = { "DBFilesClient\\AreaTable.dbc", 22, 100, AreaTableRec::FixupStrings, 0 };

// OFFSET: %0x0097C110
void AreaTableRec::FixupStrings(void* record, const char* strings) {
    AreaTableRec* rec = static_cast<AreaTableRec*>(record);
    rec->m_areaName = &strings[reinterpret_cast<uintptr_t>(rec->m_areaName)];
}

#if defined(WOWDECOMP_ARCH_64)
void AreaTableRec::ConvertRow(AreaTableRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    memcpy(&rec->m_continentID, &row[0x0004], sizeof(rec->m_continentID));
    memcpy(&rec->m_parentAreaID, &row[0x0008], sizeof(rec->m_parentAreaID));
    memcpy(&rec->m_areaBit, &row[0x000C], sizeof(rec->m_areaBit));
    memcpy(&rec->m_flags, &row[0x0010], sizeof(rec->m_flags));
    memcpy(&rec->m_soundProviderPref, &row[0x0014], sizeof(rec->m_soundProviderPref));
    memcpy(&rec->m_soundProviderPrefUnderwater, &row[0x0018], sizeof(rec->m_soundProviderPrefUnderwater));
    memcpy(&rec->m_ambienceID, &row[0x001C], sizeof(rec->m_ambienceID));
    memcpy(&rec->m_zoneMusic, &row[0x0020], sizeof(rec->m_zoneMusic));
    memcpy(&rec->m_introSound, &row[0x0024], sizeof(rec->m_introSound));
    memcpy(&rec->m_explorationLevel, &row[0x0028], sizeof(rec->m_explorationLevel));
    rec->m_areaName = &strings[*reinterpret_cast<const uint32_t*>(&row[0x002C])];
    memcpy(&rec->unk_0030, &row[0x0030], sizeof(rec->unk_0030));
    memcpy(&rec->unk_0034, &row[0x0034], sizeof(rec->unk_0034));
    memcpy(&rec->unk_0038, &row[0x0038], sizeof(rec->unk_0038));
    memcpy(&rec->unk_003C, &row[0x003C], sizeof(rec->unk_003C));
    memcpy(&rec->unk_0040, &row[0x0040], sizeof(rec->unk_0040));
    memcpy(&rec->unk_0044, &row[0x0044], sizeof(rec->unk_0044));
    memcpy(&rec->unk_0048, &row[0x0048], sizeof(rec->unk_0048));
    memcpy(&rec->unk_004C, &row[0x004C], sizeof(rec->unk_004C));
    memcpy(&rec->unk_0050, &row[0x0050], sizeof(rec->unk_0050));
    memcpy(&rec->unk_0054, &row[0x0054], sizeof(rec->unk_0054));
    memcpy(&rec->unk_0058, &row[0x0058], sizeof(rec->unk_0058));
    memcpy(&rec->unk_005C, &row[0x005C], sizeof(rec->unk_005C));
    memcpy(&rec->unk_0060, &row[0x0060], sizeof(rec->unk_0060));
}
#endif

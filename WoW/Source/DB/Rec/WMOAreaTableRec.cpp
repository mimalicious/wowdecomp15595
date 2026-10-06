#include "WoW/Source/DB/Rec/WMOAreaTableRec.h"

// STATIC: %0x00B4465C
const DBCMeta WMOAreaTableRec::s_meta = { "DBFilesClient\\WMOAreaTable.dbc", 15, 60, WMOAreaTableRec::FixupStrings, 0 };

// OFFSET: %0x0097C110
void WMOAreaTableRec::FixupStrings(void* record, const char* strings) {
    WMOAreaTableRec* rec = static_cast<WMOAreaTableRec*>(record);
    rec->m_areaName = &strings[reinterpret_cast<uintptr_t>(rec->m_areaName)];
}

#if defined(WOWDECOMP_ARCH_64)
void WMOAreaTableRec::ConvertRow(WMOAreaTableRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    memcpy(&rec->m_wmoid, &row[0x0004], sizeof(rec->m_wmoid));
    memcpy(&rec->m_nameSetID, &row[0x0008], sizeof(rec->m_nameSetID));
    memcpy(&rec->m_wmogroupID, &row[0x000C], sizeof(rec->m_wmogroupID));
    memcpy(&rec->m_soundProviderPref, &row[0x0010], sizeof(rec->m_soundProviderPref));
    memcpy(&rec->m_soundProviderPrefUnderwater, &row[0x0014], sizeof(rec->m_soundProviderPrefUnderwater));
    memcpy(&rec->m_ambienceID, &row[0x0018], sizeof(rec->m_ambienceID));
    memcpy(&rec->m_zoneMusic, &row[0x001C], sizeof(rec->m_zoneMusic));
    memcpy(&rec->m_introSound, &row[0x0020], sizeof(rec->m_introSound));
    memcpy(&rec->m_flags, &row[0x0024], sizeof(rec->m_flags));
    memcpy(&rec->m_areaTableID, &row[0x0028], sizeof(rec->m_areaTableID));
    rec->m_areaName = &strings[*reinterpret_cast<const uint32_t*>(&row[0x002C])];
    memcpy(&rec->unk_0030, &row[0x0030], sizeof(rec->unk_0030));
    memcpy(&rec->unk_0034, &row[0x0034], sizeof(rec->unk_0034));
    memcpy(&rec->unk_0038, &row[0x0038], sizeof(rec->unk_0038));
}
#endif

#include "WoW/Source/DB/Rec/AreaPOIRec.h"

// STATIC: %0x00B47278
const DBCMeta AreaPOIRec::s_meta = { "DBFilesClient\\AreaPOI.dbc", 13, 88, AreaPOIRec::FixupStrings, 0 };

// OFFSET: %0x0097C490
void AreaPOIRec::FixupStrings(void* record, const char* strings) {
    AreaPOIRec* rec = static_cast<AreaPOIRec*>(record);
    rec->m_name = &strings[reinterpret_cast<uintptr_t>(rec->m_name)];
    rec->m_description = &strings[reinterpret_cast<uintptr_t>(rec->m_description)];
}

#if defined(WOWDECOMP_ARCH_64)
void AreaPOIRec::ConvertRow(AreaPOIRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    memcpy(&rec->m_importance, &row[0x0004], sizeof(rec->m_importance));
    memcpy(rec->m_icon, &row[0x0008], sizeof(rec->m_icon));
    memcpy(&rec->m_factionID, &row[0x002C], sizeof(rec->m_factionID));
    memcpy(rec->m_pos, &row[0x0030], sizeof(rec->m_pos));
    memcpy(&rec->m_continentID, &row[0x0038], sizeof(rec->m_continentID));
    memcpy(&rec->m_flags, &row[0x003C], sizeof(rec->m_flags));
    memcpy(&rec->m_areaID, &row[0x0040], sizeof(rec->m_areaID));
    rec->m_name = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0044])];
    rec->m_description = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0048])];
    memcpy(&rec->m_worldStateID, &row[0x004C], sizeof(rec->m_worldStateID));
    memcpy(&rec->unk_0050, &row[0x0050], sizeof(rec->unk_0050));
    memcpy(&rec->unk_0054, &row[0x0054], sizeof(rec->unk_0054));
}
#endif

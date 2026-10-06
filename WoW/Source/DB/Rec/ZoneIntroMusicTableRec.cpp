#include "WoW/Source/DB/Rec/ZoneIntroMusicTableRec.h"

// STATIC: %0x00B44400
const DBCMeta ZoneIntroMusicTableRec::s_meta = { "DBFilesClient\\ZoneIntroMusicTable.dbc", 5, 20, ZoneIntroMusicTableRec::FixupStrings, 0 };

// OFFSET: %0x0097C040
void ZoneIntroMusicTableRec::FixupStrings(void* record, const char* strings) {
    ZoneIntroMusicTableRec* rec = static_cast<ZoneIntroMusicTableRec*>(record);
    rec->m_name = &strings[reinterpret_cast<uintptr_t>(rec->m_name)];
}

#if defined(WOWDECOMP_ARCH_64)
void ZoneIntroMusicTableRec::ConvertRow(ZoneIntroMusicTableRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    rec->m_name = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0004])];
    memcpy(&rec->m_soundID, &row[0x0008], sizeof(rec->m_soundID));
    memcpy(&rec->m_priority, &row[0x000C], sizeof(rec->m_priority));
    memcpy(&rec->m_minDelayMinutes, &row[0x0010], sizeof(rec->m_minDelayMinutes));
}
#endif

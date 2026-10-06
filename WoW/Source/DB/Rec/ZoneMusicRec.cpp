#include "WoW/Source/DB/Rec/ZoneMusicRec.h"

// STATIC: %0x00B4435C
const DBCMeta ZoneMusicRec::s_meta = { "DBFilesClient\\ZoneMusic.dbc", 5, 32, ZoneMusicRec::FixupStrings, 0 };

// OFFSET: %0x0097C040
void ZoneMusicRec::FixupStrings(void* record, const char* strings) {
    ZoneMusicRec* rec = static_cast<ZoneMusicRec*>(record);
    rec->m_setName = &strings[reinterpret_cast<uintptr_t>(rec->m_setName)];
}

#if defined(WOWDECOMP_ARCH_64)
void ZoneMusicRec::ConvertRow(ZoneMusicRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    rec->m_setName = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0004])];
    memcpy(rec->m_silenceIntervalMin, &row[0x0008], sizeof(rec->m_silenceIntervalMin));
    memcpy(rec->m_silenceIntervalMax, &row[0x0010], sizeof(rec->m_silenceIntervalMax));
    memcpy(rec->m_sounds, &row[0x0018], sizeof(rec->m_sounds));
}
#endif

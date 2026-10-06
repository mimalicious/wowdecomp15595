#include "WoW/Source/DB/Rec/MovieRec.h"

// STATIC: %0x00B438F0
const DBCMeta MovieRec::s_meta = { "DBFilesClient\\Movie.dbc", 4, 16, MovieRec::FixupStrings, 0 };

// OFFSET: %0x0097C040
void MovieRec::FixupStrings(void* record, const char* strings) {
    MovieRec* rec = static_cast<MovieRec*>(record);
    rec->m_filename = &strings[reinterpret_cast<uintptr_t>(rec->m_filename)];
}

#if defined(WOWDECOMP_ARCH_64)
void MovieRec::ConvertRow(MovieRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    rec->m_filename = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0004])];
    memcpy(&rec->m_volume, &row[0x0008], sizeof(rec->m_volume));
    memcpy(&rec->m_keyID, &row[0x000C], sizeof(rec->m_keyID));
}
#endif

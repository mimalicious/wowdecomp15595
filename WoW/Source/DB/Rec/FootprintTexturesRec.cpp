#include "WoW/Source/DB/Rec/FootprintTexturesRec.h"

// STATIC: %0x00B46A94
const DBCMeta FootprintTexturesRec::s_meta = { "DBFilesClient\\FootprintTextures.dbc", 2, 8, FootprintTexturesRec::FixupStrings, 0 };

// OFFSET: %0x0097C040
void FootprintTexturesRec::FixupStrings(void* record, const char* strings) {
    FootprintTexturesRec* rec = static_cast<FootprintTexturesRec*>(record);
    rec->m_footstepFilename = &strings[reinterpret_cast<uintptr_t>(rec->m_footstepFilename)];
}

#if defined(WOWDECOMP_ARCH_64)
void FootprintTexturesRec::ConvertRow(FootprintTexturesRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    rec->m_footstepFilename = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0004])];
}
#endif

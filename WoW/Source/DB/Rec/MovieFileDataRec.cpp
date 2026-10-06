#include "WoW/Source/DB/Rec/MovieFileDataRec.h"

// STATIC: %0x00B438C4
const DBCMeta MovieFileDataRec::s_meta = { "DBFilesClient\\MovieFileData.dbc", 2, 8, MovieFileDataRec::FixupStrings, 0 };

// PROVEN: 9faf1e1c IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void MovieFileDataRec::FixupStrings(void* record, const char* strings) {
}

#if defined(WOWDECOMP_ARCH_64)
void MovieFileDataRec::ConvertRow(MovieFileDataRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    memcpy(&rec->m_resolution, &row[0x0004], sizeof(rec->m_resolution));
}
#endif

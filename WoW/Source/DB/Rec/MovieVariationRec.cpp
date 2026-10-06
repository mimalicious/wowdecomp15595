#include "WoW/Source/DB/Rec/MovieVariationRec.h"

// STATIC: %0x00B43890
const DBCMeta MovieVariationRec::s_meta = { "DBFilesClient\\MovieVariation.dbc", 3, 12, MovieVariationRec::FixupStrings, 0 };

// PROVEN: bbc864c4 IDENTICAL/NONE
// OFFSET: 0x00401000
void MovieVariationRec::FixupStrings(void* record, const char* strings) {
}

#if defined(WOWDECOMP_ARCH_64)
void MovieVariationRec::ConvertRow(MovieVariationRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    memcpy(&rec->m_movieID, &row[0x0004], sizeof(rec->m_movieID));
    memcpy(&rec->m_fileDataID, &row[0x0008], sizeof(rec->m_fileDataID));
}
#endif

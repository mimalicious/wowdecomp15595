#include "WoW/Source/DB/Rec/CharHairGeosetsRec.h"

// STATIC: %0x00B43F80
const DBCMeta CharHairGeosetsRec::s_meta = { "DBFilesClient\\CharHairGeosets.dbc", 6, 24, CharHairGeosetsRec::FixupStrings, 0 };

// PROVEN: a21b8b23 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void CharHairGeosetsRec::FixupStrings(void* record, const char* strings) {
}

#if defined(WOWDECOMP_ARCH_64)
void CharHairGeosetsRec::ConvertRow(CharHairGeosetsRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    memcpy(&rec->m_raceID, &row[0x0004], sizeof(rec->m_raceID));
    memcpy(&rec->m_sexID, &row[0x0008], sizeof(rec->m_sexID));
    memcpy(&rec->m_variationID, &row[0x000C], sizeof(rec->m_variationID));
    memcpy(&rec->m_geosetID, &row[0x0010], sizeof(rec->m_geosetID));
    memcpy(&rec->m_showscalp, &row[0x0014], sizeof(rec->m_showscalp));
}
#endif

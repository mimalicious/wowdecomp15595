#include "WoW/Source/DB/Rec/CharacterFacialHairStylesRec.h"

// STATIC: %0x00B43EDC
const DBCMeta CharacterFacialHairStylesRec::s_meta = { "DBFilesClient\\CharacterFacialHairStyles.dbc", 4, 36, CharacterFacialHairStylesRec::FixupStrings, 0 };

// OFFSET: %0x00401000
void CharacterFacialHairStylesRec::FixupStrings(void* record, const char* strings) {
}

#if defined(WOWDECOMP_ARCH_64)
void CharacterFacialHairStylesRec::ConvertRow(CharacterFacialHairStylesRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    memcpy(&rec->m_raceID, &row[0x0004], sizeof(rec->m_raceID));
    memcpy(&rec->m_sexID, &row[0x0008], sizeof(rec->m_sexID));
    memcpy(&rec->m_variationID, &row[0x000C], sizeof(rec->m_variationID));
    memcpy(rec->m_geoset, &row[0x0010], sizeof(rec->m_geoset));
}
#endif

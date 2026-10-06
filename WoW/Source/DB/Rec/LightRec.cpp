#include "WoW/Source/DB/Rec/LightRec.h"

// STATIC: %0x00B439E8
const DBCMeta LightRec::s_meta = { "DBFilesClient\\Light.dbc", 6, 60, LightRec::FixupStrings, 0 };

// PROVEN: 12f22067 IDENTICAL/NONE
// OFFSET: 0x00401000
void LightRec::FixupStrings(void* record, const char* strings) {
}

#if defined(WOWDECOMP_ARCH_64)
void LightRec::ConvertRow(LightRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    memcpy(&rec->m_continentID, &row[0x0004], sizeof(rec->m_continentID));
    memcpy(rec->m_gameCoords, &row[0x0008], sizeof(rec->m_gameCoords));
    memcpy(&rec->m_gameFalloffStart, &row[0x0014], sizeof(rec->m_gameFalloffStart));
    memcpy(&rec->m_gameFalloffEnd, &row[0x0018], sizeof(rec->m_gameFalloffEnd));
    memcpy(rec->m_lightParamsID, &row[0x001C], sizeof(rec->m_lightParamsID));
}
#endif

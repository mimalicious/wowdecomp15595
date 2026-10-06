#include "WoW/Source/DB/Rec/LightParamsRec.h"

// STATIC: %0x00B43988
const DBCMeta LightParamsRec::s_meta = { "DBFilesClient\\LightParams.dbc", 10, 40, LightParamsRec::FixupStrings, 0 };

// PROVEN: ff8f362a IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void LightParamsRec::FixupStrings(void* record, const char* strings) {
}

#if defined(WOWDECOMP_ARCH_64)
void LightParamsRec::ConvertRow(LightParamsRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    memcpy(&rec->m_highlightSky, &row[0x0004], sizeof(rec->m_highlightSky));
    memcpy(&rec->m_lightSkyboxID, &row[0x0008], sizeof(rec->m_lightSkyboxID));
    memcpy(&rec->m_cloudTypeID, &row[0x000C], sizeof(rec->m_cloudTypeID));
    memcpy(&rec->m_glow, &row[0x0010], sizeof(rec->m_glow));
    memcpy(&rec->m_waterShallowAlpha, &row[0x0014], sizeof(rec->m_waterShallowAlpha));
    memcpy(&rec->m_waterDeepAlpha, &row[0x0018], sizeof(rec->m_waterDeepAlpha));
    memcpy(&rec->m_oceanShallowAlpha, &row[0x001C], sizeof(rec->m_oceanShallowAlpha));
    memcpy(&rec->m_oceanDeepAlpha, &row[0x0020], sizeof(rec->m_oceanDeepAlpha));
    memcpy(&rec->unk_0024, &row[0x0024], sizeof(rec->unk_0024));
}
#endif

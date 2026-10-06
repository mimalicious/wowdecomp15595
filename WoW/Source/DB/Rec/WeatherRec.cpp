#include "WoW/Source/DB/Rec/WeatherRec.h"

// STATIC: %0x00B44578
const DBCMeta WeatherRec::s_meta = { "DBFilesClient\\Weather.dbc", 6, 32, WeatherRec::FixupStrings, 0 };

// OFFSET: %0x0097C140
void WeatherRec::FixupStrings(void* record, const char* strings) {
    WeatherRec* rec = static_cast<WeatherRec*>(record);
    rec->m_effectTexture = &strings[reinterpret_cast<uintptr_t>(rec->m_effectTexture)];
}

#if defined(WOWDECOMP_ARCH_64)
void WeatherRec::ConvertRow(WeatherRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    memcpy(&rec->m_ambienceID, &row[0x0004], sizeof(rec->m_ambienceID));
    memcpy(&rec->m_effectType, &row[0x0008], sizeof(rec->m_effectType));
    memcpy(&rec->m_transitionSkyBox, &row[0x000C], sizeof(rec->m_transitionSkyBox));
    memcpy(rec->m_effectColor, &row[0x0010], sizeof(rec->m_effectColor));
    rec->m_effectTexture = &strings[*reinterpret_cast<const uint32_t*>(&row[0x001C])];
}
#endif

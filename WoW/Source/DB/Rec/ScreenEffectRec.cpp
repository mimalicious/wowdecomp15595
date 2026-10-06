#include "WoW/Source/DB/Rec/ScreenEffectRec.h"

// STATIC: %0x00B45688
const DBCMeta ScreenEffectRec::s_meta = { "DBFilesClient\\ScreenEffect.dbc", 7, 40, ScreenEffectRec::FixupStrings, 0 };

// OFFSET: %0x0097C040
void ScreenEffectRec::FixupStrings(void* record, const char* strings) {
    ScreenEffectRec* rec = static_cast<ScreenEffectRec*>(record);
    rec->m_name = &strings[reinterpret_cast<uintptr_t>(rec->m_name)];
}

#if defined(WOWDECOMP_ARCH_64)
void ScreenEffectRec::ConvertRow(ScreenEffectRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    rec->m_name = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0004])];
    memcpy(&rec->m_effect, &row[0x0008], sizeof(rec->m_effect));
    memcpy(rec->m_param, &row[0x000C], sizeof(rec->m_param));
    memcpy(&rec->m_lightParamsID, &row[0x001C], sizeof(rec->m_lightParamsID));
    memcpy(&rec->m_soundAmbienceID, &row[0x0020], sizeof(rec->m_soundAmbienceID));
    memcpy(&rec->m_zoneMusicID, &row[0x0024], sizeof(rec->m_zoneMusicID));
}
#endif

#include "WoW/Source/DB/Rec/LightSkyboxRec.h"

// STATIC: %0x00B45D5C
const DBCMeta LightSkyboxRec::s_meta = { "DBFilesClient\\LightSkybox.dbc", 3, 12, LightSkyboxRec::FixupStrings, 0 };

// OFFSET: %0x0097C040
void LightSkyboxRec::FixupStrings(void* record, const char* strings) {
    LightSkyboxRec* rec = static_cast<LightSkyboxRec*>(record);
    rec->m_name = &strings[reinterpret_cast<uintptr_t>(rec->m_name)];
}

#if defined(WOWDECOMP_ARCH_64)
void LightSkyboxRec::ConvertRow(LightSkyboxRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    rec->m_name = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0004])];
    memcpy(&rec->m_flags, &row[0x0008], sizeof(rec->m_flags));
}
#endif

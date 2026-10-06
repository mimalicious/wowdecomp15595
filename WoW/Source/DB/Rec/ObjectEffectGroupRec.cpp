#include "WoW/Source/DB/Rec/ObjectEffectGroupRec.h"

// STATIC: %0x00B43788
const DBCMeta ObjectEffectGroupRec::s_meta = { "DBFilesClient\\ObjectEffectGroup.dbc", 2, 8, ObjectEffectGroupRec::FixupStrings, 0 };

// OFFSET: %0x0097C040
void ObjectEffectGroupRec::FixupStrings(void* record, const char* strings) {
    ObjectEffectGroupRec* rec = static_cast<ObjectEffectGroupRec*>(record);
    rec->m_name = &strings[reinterpret_cast<uintptr_t>(rec->m_name)];
}

#if defined(WOWDECOMP_ARCH_64)
void ObjectEffectGroupRec::ConvertRow(ObjectEffectGroupRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    rec->m_name = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0004])];
}
#endif

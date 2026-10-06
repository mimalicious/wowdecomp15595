#include "WoW/Source/DB/Rec/ObjectEffectRec.h"

// STATIC: %0x00B437BC
const DBCMeta ObjectEffectRec::s_meta = { "DBFilesClient\\ObjectEffect.dbc", 10, 48, ObjectEffectRec::FixupStrings, 0 };

// OFFSET: %0x0097C040
void ObjectEffectRec::FixupStrings(void* record, const char* strings) {
    ObjectEffectRec* rec = static_cast<ObjectEffectRec*>(record);
    rec->m_name = &strings[reinterpret_cast<uintptr_t>(rec->m_name)];
}

#if defined(WOWDECOMP_ARCH_64)
void ObjectEffectRec::ConvertRow(ObjectEffectRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    rec->m_name = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0004])];
    memcpy(&rec->m_objectEffectGroupID, &row[0x0008], sizeof(rec->m_objectEffectGroupID));
    memcpy(&rec->m_triggerType, &row[0x000C], sizeof(rec->m_triggerType));
    memcpy(&rec->m_eventType, &row[0x0010], sizeof(rec->m_eventType));
    memcpy(&rec->m_effectRecType, &row[0x0014], sizeof(rec->m_effectRecType));
    memcpy(&rec->m_effectRecID, &row[0x0018], sizeof(rec->m_effectRecID));
    memcpy(&rec->m_attachment, &row[0x001C], sizeof(rec->m_attachment));
    memcpy(rec->m_offset, &row[0x0020], sizeof(rec->m_offset));
    memcpy(&rec->m_objectEffectModifierID, &row[0x002C], sizeof(rec->m_objectEffectModifierID));
}
#endif

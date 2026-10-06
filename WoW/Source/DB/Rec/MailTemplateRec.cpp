#include "WoW/Source/DB/Rec/MailTemplateRec.h"

// STATIC: %0x00B45C8C
const DBCMeta MailTemplateRec::s_meta = { "DBFilesClient\\MailTemplate.dbc", 3, 12, MailTemplateRec::FixupStrings, 0 };

// OFFSET: %0x0097C1F0
void MailTemplateRec::FixupStrings(void* record, const char* strings) {
    MailTemplateRec* rec = static_cast<MailTemplateRec*>(record);
    rec->m_subject = &strings[reinterpret_cast<uintptr_t>(rec->m_subject)];
    rec->m_body = &strings[reinterpret_cast<uintptr_t>(rec->m_body)];
}

#if defined(WOWDECOMP_ARCH_64)
void MailTemplateRec::ConvertRow(MailTemplateRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    rec->m_subject = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0004])];
    rec->m_body = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0008])];
}
#endif

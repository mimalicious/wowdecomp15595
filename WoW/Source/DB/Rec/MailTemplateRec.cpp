#include "WoW/Source/DB/Rec/MailTemplateRec.h"

// STATIC: 0x00C234F0/0x140A18790
const DBCMeta MailTemplateRec::s_meta = { "DBFilesClient\\MailTemplate.dbc", 3, 12, MailTemplateRec::FixupStrings, 0 };

// PROVEN: c6b08f42 IDENTICAL/NONE
// OFFSET: 0x00A2E9B0/0x1407D8110
void MailTemplateRec::FixupStrings(void* record, const char* strings) {
    MailTemplateRec* rec = static_cast<MailTemplateRec*>(record);
    DBCFixupString(rec->m_subject, strings);
    DBCFixupString(rec->m_body, strings);
}

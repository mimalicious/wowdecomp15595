#include "WoW/Source/DB/Rec/MailTemplateRec.h"

// STATIC: 0x00C234CC/0x140A18760
static const int32_t s_fieldOffsets[3] = { 0, 4, 8 };

// STATIC: 0x00C234D8/0x140A18770
static const int32_t s_fieldSizes[3] = { 4, 4, 4 };

// STATIC: 0x00C234E4/0x140A18780
static const int32_t s_fieldTypes[3] = { 0, 2, 2 };

// STATIC: 0x00C234F0/0x140A18790
const DBCMeta MailTemplateRec::s_meta = { "DBFilesClient\\MailTemplate.dbc", 3, 12, MailTemplateRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// OFFSET: 0x00A2E9B0/0x1407D8110
void MailTemplateRec::FixupStrings(void* record, const char* strings) {
    MailTemplateRec* rec = static_cast<MailTemplateRec*>(record);
    DBCFixupString(rec->m_subject, strings);
    DBCFixupString(rec->m_body, strings);
}

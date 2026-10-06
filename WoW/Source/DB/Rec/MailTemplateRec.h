#ifndef WOW_SOURCE_DB_REC_MAILTEMPLATEREC_H
#define WOW_SOURCE_DB_REC_MAILTEMPLATEREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class MailTemplateRec {
    public:
    static const DBCMeta s_meta;

    static void FixupStrings(void* record, const char* strings);

    /* 0000 */ int32_t m_id;
    /* 0004 */ DBCString m_subject;
    /* 0008 */ DBCString m_body;
};

#endif

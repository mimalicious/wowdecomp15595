#ifndef WOW_SOURCE_DB_REC_FILEDATAREC_H
#define WOW_SOURCE_DB_REC_FILEDATAREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class FileDataRec {
    public:
    static const DBCMeta s_meta;

    /* 0000 */ int32_t m_id;
    /* 0004 */ DBCString m_filename;
    /* 0008 */ DBCString m_filepath;

    static void FixupStrings(void* record, const char* strings);
};

#endif

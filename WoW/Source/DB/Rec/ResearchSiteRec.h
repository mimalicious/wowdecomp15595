#ifndef WOW_SOURCE_DB_REC_RESEARCHSITEREC_H
#define WOW_SOURCE_DB_REC_RESEARCHSITEREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class ResearchSiteRec {
    public:
    static const DBCMeta s_meta;

    static void FixupStrings(void* record, const char* strings);

    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_mapID;
    /* 0008 */ int32_t m_questPOIBlobID;
    /* 000C */ DBCString m_name;
    /* 0010 */ int32_t m_areaPOIIconEnum;
};

#endif

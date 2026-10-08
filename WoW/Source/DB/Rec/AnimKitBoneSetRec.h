#ifndef WOW_SOURCE_DB_REC_ANIMKITBONESETREC_H
#define WOW_SOURCE_DB_REC_ANIMKITBONESETREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class AnimKitBoneSetRec {
    public:
    static const DBCMeta s_meta;

    static void FixupStrings(void* record, const char* strings);

    /* 0000 */ int32_t m_id;
    /* 0004 */ DBCString m_name;
    /* 0008 */ int32_t m_boneDataID;
    /* 000C */ int32_t m_parentAnimKitBoneSetID;
    /* 0010 */ int32_t m_extraBoneCount;
    /* 0014 */ int32_t m_altAnimKitBoneSetID;
};

#endif

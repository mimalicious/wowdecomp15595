#ifndef WOW_SOURCE_DB_REC_OBJECTEFFECTPACKAGEELEMREC_H
#define WOW_SOURCE_DB_REC_OBJECTEFFECTPACKAGEELEMREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class ObjectEffectPackageElemRec {
    public:
    static const DBCMeta s_meta;

    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_objectEffectPackageID;
    /* 0008 */ int32_t m_objectEffectGroupID;
    /* 000C */ int32_t m_stateType;

    static void FixupStrings(void* record, const char* strings);
#if defined(WOWDECOMP_ARCH_64)
    static void ConvertRow(ObjectEffectPackageElemRec* rec, const uint8_t* row, const char* strings);
#endif
};

#endif

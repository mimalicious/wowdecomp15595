#ifndef WOW_SOURCE_DB_REC_OBJECTEFFECTMODIFIERREC_H
#define WOW_SOURCE_DB_REC_OBJECTEFFECTMODIFIERREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class ObjectEffectModifierRec {
    public:
    static const DBCMeta s_meta;

    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_inputType;
    /* 0008 */ int32_t m_mapType;
    /* 000C */ int32_t m_outputType;
    /* 0010 */ float m_param [4];

    static void FixupStrings(void* record, const char* strings);
};

#endif

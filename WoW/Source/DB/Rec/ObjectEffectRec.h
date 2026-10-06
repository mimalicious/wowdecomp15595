#ifndef WOW_SOURCE_DB_REC_OBJECTEFFECTREC_H
#define WOW_SOURCE_DB_REC_OBJECTEFFECTREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class ObjectEffectRec {
    public:
    static const DBCMeta s_meta;

    /* 0000 */ int32_t m_id;
    /* 0004 */ DBCString m_name;
    /* 0008 */ int32_t m_objectEffectGroupID;
    /* 000C */ int32_t m_triggerType;
    /* 0010 */ int32_t m_eventType;
    /* 0014 */ int32_t m_effectRecType;
    /* 0018 */ int32_t m_effectRecID;
    /* 001C */ int32_t m_attachment;
    /* 0020 */ float m_offset [3];
    /* 002C */ int32_t m_objectEffectModifierID;

    static void FixupStrings(void* record, const char* strings);
};

#endif

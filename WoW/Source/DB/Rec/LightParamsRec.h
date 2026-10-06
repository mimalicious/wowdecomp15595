#ifndef WOW_SOURCE_DB_REC_LIGHTPARAMSREC_H
#define WOW_SOURCE_DB_REC_LIGHTPARAMSREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class LightParamsRec {
    public:
    static const DBCMeta s_meta;

    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_highlightSky;
    /* 0008 */ int32_t m_lightSkyboxID;
    /* 000C */ int32_t m_cloudTypeID;
    /* 0010 */ float m_glow;
    /* 0014 */ float m_waterShallowAlpha;
    /* 0018 */ float m_waterDeepAlpha;
    /* 001C */ float m_oceanShallowAlpha;
    /* 0020 */ float m_oceanDeepAlpha;
    /* 0024 */ int32_t unk_0024;

    static void FixupStrings(void* record, const char* strings);
#if defined(WOWDECOMP_ARCH_64)
    static void ConvertRow(LightParamsRec* rec, const uint8_t* row, const char* strings);
#endif
};

#endif

#ifndef WOW_SOURCE_DB_REC_SCREENEFFECTREC_H
#define WOW_SOURCE_DB_REC_SCREENEFFECTREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class ScreenEffectRec {
    public:
    static const DBCMeta s_meta;

    /* 0000 */ int32_t m_id;
    /* 0004 */ const char* m_name;
    /* 0008 */ int32_t m_effect;
    /* 000C */ int32_t m_param [4];
    /* 001C */ int32_t m_lightParamsID;
    /* 0020 */ int32_t m_soundAmbienceID;
    /* 0024 */ int32_t m_zoneMusicID;

    static void FixupStrings(void* record, const char* strings);
#if defined(WOWDECOMP_ARCH_64)
    static void ConvertRow(ScreenEffectRec* rec, const uint8_t* row, const char* strings);
#endif
};

#endif

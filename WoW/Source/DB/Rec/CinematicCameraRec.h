#ifndef WOW_SOURCE_DB_REC_CINEMATICCAMERAREC_H
#define WOW_SOURCE_DB_REC_CINEMATICCAMERAREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class CinematicCameraRec {
    public:
    static const DBCMeta s_meta;

    /* 0000 */ int32_t m_id;
    /* 0004 */ DBCString m_model;
    /* 0008 */ int32_t m_soundID;
    /* 000C */ float m_origin [3];
    /* 0018 */ float m_originFacing;

    static void FixupStrings(void* record, const char* strings);
};

#endif

#ifndef WOW_SOURCE_DB_REC_CAMERAMODEREC_H
#define WOW_SOURCE_DB_REC_CAMERAMODEREC_H

#include <cstdint>
#include "Engine/Source/Tempest/C3Vector.h"
#include "WoW/Source/DB/WowClientDB.h"

class CameraModeRec {
    public:
    static const DBCMeta s_meta;

    static void FixupStrings(void* record, const char* strings);

    /* 0000 */ int32_t m_id;
    /* 0004 */ DBCString m_name;
    /* 0008 */ int32_t m_type;
    /* 000C */ int32_t m_flags;
    /* 0010 */ C3Vector m_positionOffset;
    /* 001C */ C3Vector m_targetOffset;
    /* 0028 */ float m_positionSmoothing;
    /* 002C */ float m_rotationSmoothing;
    /* 0030 */ float m_fieldOfView;
    /* 0034 */ int32_t m_lockedPositionOffsetBase;
    /* 0038 */ int32_t m_lockedPositionOffsetDirection;
    /* 003C */ int32_t m_lockedTargetOffsetBase;
    /* 0040 */ int32_t m_lockedTargetOffsetDirection;
};

#endif

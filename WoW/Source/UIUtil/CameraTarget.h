#ifndef WOW_SOURCE_UIUTIL_CAMERATARGET_H
#define WOW_SOURCE_UIUTIL_CAMERATARGET_H

#include <cstdint>
#include "Engine/Source/Tempest/C44Matrix.h"
#include "WoW/Source/Object/WGUID.h"

class CCameraTarget {
    public:
    enum TARGET_TYPE {
        TARGET_SELF = 0,
        TARGET_UNIT_TARGET = 1,
        TARGET_CHANNEL_OBJECT = 2,
        TARGET_VEHICLE = 3,
    };

    /* 0000 */ TARGET_TYPE m_types[4];
    /* 0010 */ WOWGUID m_targetGUID;
    /* 0018 */ WOWGUID m_currentTargetGUID;
    /* 0020 */ C44Matrix m_matrix;
    /* 0060 */ int32_t m_valid;

    CCameraTarget();
    void UpdateInternal();
    WOWGUID CalcTargetGUID(WOWGUID guid);
    void SetObject(const C44Matrix& matrix, WOWGUID guid, TARGET_TYPE* types, int32_t count);
    void UpdateTarget(WOWGUID guid);
};

#endif

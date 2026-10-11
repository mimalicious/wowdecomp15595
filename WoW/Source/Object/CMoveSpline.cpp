#include "Engine/Source/Base/CDataStore.h"
#include "Engine/Source/Os/TimeManager.h"
#include "WoW/Source/Object/CMoveSpline.h"
#include "WoW/Source/Object/CClientMoveUpdate.h"
#include "WoW/Source/Object/Types.h"

// INLINED
CMoveSpline::CMoveSpline() {
    this->m_finalDestination.x = 0.0f;
    this->m_finalDestination.y = 0.0f;
    this->m_finalDestination.z = 0.0f;
}

// OFFSET: 0x005E70E0/0x140267B90
CMoveSpline::~CMoveSpline() {
}

// PROVEN: dd44802f IDENTICAL/NONE
// OFFSET: 0x004010A0
CDataStore& operator>>(CDataStore& msg, C3Vector& vector) {
    msg.Get(vector.x);
    msg.Get(vector.y);
    msg.Get(vector.z);
    return msg;
}

// INLINED
CDataStore& operator>>(CDataStore& msg, CMoveSpline& spline) {
    msg.Get(spline.m_flags);

    if (spline.m_flags & SPLINEFLAG_FINAL_ANGLE) {
        msg.Get(spline.m_face.m_angle);
    } else if (spline.m_flags & SPLINEFLAG_FINAL_TARGET) {
        msg.Get(*reinterpret_cast<uint64_t*>(&spline.m_face.m_target));
    } else if (spline.m_flags & SPLINEFLAG_FINAL_POINT) {
        msg >> *reinterpret_cast<C3Vector*>(spline.m_face.m_point);
    }

    uint32_t now = OsGetAsyncTimeMs();
    uint32_t timePassed;
    msg.Get(timePassed);
    spline.m_timePassed = now - timePassed;

    msg.Get(spline.m_duration);
    msg.Get(spline.m_id);
    msg.Get(spline.m_durationMod);
    msg.Get(spline.m_durationModNext);
    msg.Get(spline.m_verticalAcceleration);
    msg.Get(spline.m_effectStartTime);
    msg >> spline.m_spline;
    msg.Get(spline.m_finalDestination.x);
    msg.Get(spline.m_finalDestination.y);
    msg.Get(spline.m_finalDestination.z);
    return msg;
}

// OFFSET: 0x005FF7B0
CMoveSpline& CMoveSpline::CopyFrom(const CMoveSpline& source) {
    this->unk_0008 = source.unk_0008;
    this->m_face = source.m_face;
    this->unk_001C = source.unk_001C;
    this->m_flags = source.m_flags;
    this->m_timePassed = source.m_timePassed;
    this->m_start = source.m_start;
    this->m_duration = source.m_duration;
    this->m_id = source.m_id;
    this->m_spline = source.m_spline;
    this->m_finalDestination = source.m_finalDestination;
    this->m_durationMod = source.m_durationMod;
    this->m_durationModNext = source.m_durationModNext;
    this->m_verticalAcceleration = source.m_verticalAcceleration;
    this->m_effectStartTime = source.m_effectStartTime;
    return *this;
}

// OFFSET: 0x00A291D0
CMoveSpline& CMoveSpline::InitFromClientSpline(const CClientMoveSpline& source) {
    this->m_id = source.m_id;
    this->m_finalDestination = source.m_endPoint;

    if (!source.m_hasData) {
        this->m_duration = 0;
        this->m_spline.SetPoints(NULL, 0);
        return *this;
    }

    const CClientMoveSplineData& data = source.m_data;
    this->m_flags = data.m_flags;

    switch (data.m_faceType) {
        case 0:
            this->m_flags |= SPLINEFLAG_FINAL_ANGLE;
            this->m_face.m_angle = data.m_facingAngle;
            break;
        case 1:
            this->m_face.m_point[0] = data.m_facingPoint.x;
            this->m_face.m_point[1] = data.m_facingPoint.y;
            this->m_flags |= SPLINEFLAG_FINAL_POINT;
            this->m_face.m_point[2] = data.m_facingPoint.z;
            break;
        case 2:
            this->m_face.m_target = data.m_facingTarget;
            this->m_flags |= SPLINEFLAG_FINAL_TARGET;
            break;
    }

    this->m_timePassed = OsGetAsyncTimeMs() - data.m_timePassed;
    this->m_duration = data.m_duration;
    this->m_durationMod = data.m_durationMod;
    this->m_durationModNext = data.m_durationModNext;

    if (data.m_hasVerticalAcceleration) {
        this->m_flags |= SPLINEFLAG_PARABOLIC;
        this->m_verticalAcceleration = data.m_verticalAcceleration;
    } else {
        this->m_verticalAcceleration = 0.0f;
    }

    if (data.m_hasEffectStartTime) {
        this->m_effectStartTime = data.m_effectStartTime;

        if (!(this->m_flags & SPLINEFLAG_PARABOLIC)) {
            this->m_flags |= SPLINEFLAG_ANIMATION;
        }
    } else {
        this->m_effectStartTime = 0;
    }

    this->m_spline.SetPoints(data.m_points.m_data, data.m_points.m_count);
    this->m_spline.m_splineMode = data.m_mode;
    return *this;
}

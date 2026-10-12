#ifndef WOW_SOURCE_UIUTIL_CAMERATRACK_H
#define WOW_SOURCE_UIUTIL_CAMERATRACK_H

#include <cstdint>
#include "Engine/Source/Storm/H/stpl.h"
#include "Engine/Source/Tempest/C3Vector.h"
#include "WoW/Source/Object/WGUID.h"
#include "WoW/Source/UIUtil/CameraShot.h"

class CCameraMode;
class CCameraModeInstance;

class CCameraTransition {
    public:
    /* 0000 */ uint32_t m_flags;
    /* 0004 */ float m_duration;
    /* 0008 */ float m_delay;
    /* 000C */ float m_easeIn;
    /* 0010 */ float m_easeOut;
    /* 0014 */ uint32_t unk_0014;
    /* 0018 */ uint32_t unk_0018;

    static float CalcBezier(float t, float easeIn, float easeOut);

    CCameraTransition();
    int32_t IsComplete(float time) const;
    float CalcLerpFactor(float time) const;
};

class CCameraTrackBlendData {
    public:
    /* 0000 */ float m_distance;
    /* 0004 */ float m_yaw;
    /* 0008 */ float m_pitch;
    /* 000C */ C3Vector m_position;
    /* 0018 */ C3Vector m_target;
    /* 0024 */ float m_fov;
    /* 0028 */ float m_roll;
    /* 002C */ float m_fade;

    CCameraTrackBlendData();
    void Init(const CCameraShot& shot);
    void CalcDeltas(const CCameraShot& from, const CCameraShot& to);
    CCameraShot ApplyBlend(const CCameraShot& shot, float factor, int32_t flags) const;
};

class CCameraTrackNode {
    public:
    /* 0000 */ const CCameraTransition* m_transition;
    /* 0004 */ CCameraModeInstance* m_instance;
    /* 0008 */ CCameraTrackNode* m_next;
    /* 000C */ CCameraTrackBlendData m_blend;
    /* 003C */ CCameraShot m_shot;

    CCameraTrackNode();
    void Init(const CCameraTransition* transition, CCameraModeInstance* instance, CCameraTrackNode* prev);
    CCameraShot Update(float elapsed);
};

class CCameraTrack {
    public:
    enum DEFERRED_ACTION {
        DEFERRED_NONE = 0,
        DEFERRED_SET_CAMERA = 1,
        DEFERRED_RESET_CAMERAS = 2,
        DEFERRED_CLEAR_CAMERA = 3,
    };

    struct DeferredCameraAction {
        /* 0000 */ DEFERRED_ACTION m_action;
        /* 0004 */ const CCameraModeInstance* m_instance;
        /* 0008 */ const CCameraMode* m_mode;
        /* 0010 */ WOWGUID m_guid;

        DeferredCameraAction();
    };

    /* 0000 */ CCameraShot m_shot;
    /* 0024 */ CCameraTrackNode* m_head;
    /* 0028 */ TSGrowableArray<DeferredCameraAction> m_deferredActions;

    CCameraTrack();
    ~CCameraTrack();
    int32_t IsCompletelyLegacy() const;
    const CCameraMode* GetCurrentCamera() const;
    void SetCurrentCameraInternal(const CCameraModeInstance* instance, const CCameraMode* mode, WOWGUID guid);
    void ClearCamerasInternal(CCameraTrackNode* keep);
    void ResetCamerasInternal(WOWGUID guid);
    void ResetCameras(WOWGUID guid);
    void SetCurrentCamera(const CCameraModeInstance* instance, const CCameraMode* mode, WOWGUID guid);
    void ProcessDeferredActions();
    void Update(float elapsed);
};

#include "WoW/Source/UIUtil/CameraTrack.inl"

#endif

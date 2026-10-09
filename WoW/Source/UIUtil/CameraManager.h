#ifndef WOW_SOURCE_UIUTIL_CAMERAMANAGER_H
#define WOW_SOURCE_UIUTIL_CAMERAMANAGER_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Storm/H/stpl.h"
#include "Engine/Source/Tempest/C3Vector.h"
#include "Engine/Source/Tempest/C44Matrix.h"
#include "WoW/Source/DB/Rec/CameraModeRec.h"
#include "WoW/Source/Object/WGUID.h"
#include "WoW/Source/StaticSingleton.h"
#include "WoW/Source/UIUtil/CameraShot.h"
#include "WoW/Source/UIUtil/CameraTrack.h"

class CRect;
class CCameraMode;
class CCameraModeInstance;
class CCameraTrack;
class CCameraTransition;
struct CVar;

struct CCameraModeBuffer {
    /* 0000 */ uint32_t m_data[2];
};

class CCameraModeEntry : public TSHashObject<CCameraModeEntry, HASHKEY_NONE> {
    public:
    /* 0018 */ CCameraMode* m_mode;

    ~CCameraModeEntry();
};

class CCameraManager {
    public:
    struct CameraShake : public TSLinkedNode<CameraShake> {
        /* 0008 */ int32_t m_type;
        /* 000C */ int32_t m_dir;
        /* 0010 */ float m_amplitude;
        /* 0014 */ float m_frequency;
        /* 0018 */ float m_duration;
        /* 001C */ float m_timeOffset;
        /* 0020 */ float m_damping;
        /* 0024 */ C3Vector m_position;
        /* 0030 */ uint32_t m_startTime;
        /* 0034 */ uint32_t m_flags;
    };

    static CVar* s_cameraSystemCVar;
    static uint32_t ANIMATED_CAMERA_ID;
    static CameraModeRec s_animatedCameraRec;
    static TSHashTable<CCameraModeEntry, HASHKEY_NONE> s_cameraModes;
    static TSGrowableArray<CCameraTransition> s_transitions;

    static void __stdcall AddShakeOffset(C3Vector& offset, float facing, float amplitude, float time, float frequency, int32_t type, float damping, int32_t dir);

    /* 0000 */ float m_aspect;
    /* 0004 */ CCameraShot m_shot;
    /* 0028 */ C44Matrix m_renderMatrix;
    /* 0068 */ CCameraTrack* m_track;
    /* 006C */ int32_t m_legacyActive;
    /* 0070 */ TSList<CameraShake, TSGetLink<CameraShake>> m_shakes;

    CCameraManager();
    ~CCameraManager();
    float GetYaw();
    int32_t IsCompletelyLegacy();
    const CCameraMode* GetGameCamera();
    void ResetCameras(WOWGUID guid);
    CCameraMode* CreateCameraMode(CCameraModeBuffer* buffer, const CameraModeRec* rec);
    int32_t IsCameraSystemEnabled();
    void SetScreenAspect(const CRect& rect);
    const CCameraTransition* FindTransition(const CCameraMode* from, const CCameraMode* to);
    void ApplyWorldProjection(const CRect& rect);
    void UpdateLegacyState();
    C3Vector CalcShakeOffset(const C3Vector& position, float facing, int32_t requireFlag2, int32_t requireFlag1);
    void ApplyShakes(CCameraShot& shot);
    void InitCameraTransitions();
    void DestroyCameraModes();
    void SetGameCamera(const CCameraModeInstance* instance, uint32_t id, WOWGUID guid);
    void SetGameCamera(uint32_t id, WOWGUID guid);
    void AddShake(const C3Vector& position, int32_t type, int32_t dir, float amplitude, float frequency, float duration, float timeOffset, float damping, uint32_t flags);
    void AddShakeById(int32_t shakeId, const C3Vector& position);
    void ClearShakes();
    void PruneShakes();
    void Update(float elapsed);
    void InitCameraModeFromRecord(const CameraModeRec* rec);
    void InitCameraModes();
};

#endif

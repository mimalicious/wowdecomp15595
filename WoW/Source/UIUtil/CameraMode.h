#ifndef WOW_SOURCE_UIUTIL_CAMERAMODE_H
#define WOW_SOURCE_UIUTIL_CAMERAMODE_H

#include <cstdint>
#include "Engine/Source/Tempest/C3Vector.h"
#include "Engine/Source/Tempest/C44Matrix.h"
#include "WoW/Source/Object/WGUID.h"
#include "WoW/Source/UIUtil/CameraShot.h"
#include "WoW/Source/UIUtil/CameraTarget.h"

class CameraModeRec;
class CCameraMode;
class CM2Model;

enum CAMERA_MODE_TYPE {
    CAMERA_MODE_LEGACY = 1,
    CAMERA_MODE_LOCKED = 2,
    CAMERA_MODE_ANIMATED = 3,
};

enum CAMERA_MODE_LOCKED_OFFSET_BASE {
    CAMERA_MODE_LOCKED_OFFSET_BASE_TARGET = 0,
    CAMERA_MODE_LOCKED_OFFSET_BASE_SECOND_TARGET = 1,
    CAMERA_MODE_LOCKED_OFFSET_BASE_WORLD = 2,
};

enum CAMERA_MODE_LOCKED_OFFSET_DIRECTION {
    CAMERA_MODE_LOCKED_OFFSET_DIRECTION_TARGET = 0,
    CAMERA_MODE_LOCKED_OFFSET_DIRECTION_SECOND_TARGET = 1,
    CAMERA_MODE_LOCKED_OFFSET_DIRECTION_TO_SECOND_TARGET = 2,
    CAMERA_MODE_LOCKED_OFFSET_DIRECTION_FROM_SECOND_TARGET = 3,
};

class CCameraModeInstance {
    public:
    /* 0000 */ CCameraShot m_shot;
    /* 0028 */ WOWGUID m_guid;
    /* 0030 */ const CCameraMode* m_mode;
    /* 0034 */ float m_time;

    CCameraModeInstance();
    void Init(const CCameraModeInstance& instance);
    void Destroy();
    void Update(float elapsed);
    int32_t IsTracking(WOWGUID guid) const;
};

class CCameraModeInstanceDefault : public CCameraModeInstance {
};

class CCameraModeInstanceAnimated : public CCameraModeInstance {
    public:
    /* 0038 */ int32_t m_cameraIndex : 7;
    /* 0038 */ int32_t m_active : 1;

    CCameraModeInstanceAnimated();
};

class CCameraModeInstanceLocked : public CCameraModeInstance {
    public:
    /* 0038 */ CCameraTarget m_target;
    /* 00A0 */ CCameraTarget m_secondTarget;
    /* 0108 */ uint32_t m_resetRequested : 1;
    /* 0108 */ uint32_t m_twoTargets : 1;

    CCameraModeInstanceLocked();
};

class CCameraMode {
    public:
    /* 0004 */ const CameraModeRec* m_rec;

    CCameraMode(const CameraModeRec* rec);
    uint32_t GetID() const;

    virtual CCameraModeInstance* AllocInstance(const CCameraModeInstance& instance) const;
    virtual void FreeInstance(CCameraModeInstance* instance) const;
    virtual ~CCameraMode();
    virtual int32_t GetType() const = 0;
    virtual void InitInstance(CCameraModeInstance* instance) const = 0;
    virtual void UpdateInstance(CCameraModeInstance* instance, float elapsed) const = 0;
    virtual void DestroyInstance(CCameraModeInstance* instance) const = 0;
    virtual int32_t AllowDuplicate(const CCameraModeInstance* current, const CCameraModeInstance* instance) const;
    virtual int32_t IsTracking(const CCameraModeInstance* instance, WOWGUID guid) const;
};

class CCameraModeLegacy : public CCameraMode {
    public:
    static int32_t s_suspended;

    static void CalcShotFromLegacyCamera(CCameraShot& shot);

    CCameraModeLegacy(const CameraModeRec* rec);
    void UpdateFromLegacyCamera(CCameraModeInstance* instance) const;

    virtual int32_t GetType() const;
    virtual void InitInstance(CCameraModeInstance* instance) const;
    virtual void UpdateInstance(CCameraModeInstance* instance, float elapsed) const;
    virtual void DestroyInstance(CCameraModeInstance* instance) const;
    virtual int32_t IsTracking(const CCameraModeInstance* instance, WOWGUID guid) const;
};

class CCameraModeAnimated : public CCameraMode {
    public:
    static int32_t s_cameraIndex;

    int32_t CalcShotFromModel(int32_t index, CM2Model* model, CCameraShot& shot) const;

    CCameraModeAnimated(const CameraModeRec* rec);
    void UpdateInternal(CCameraModeInstance* instance) const;

    virtual CCameraModeInstance* AllocInstance(const CCameraModeInstance& instance) const;
    virtual void FreeInstance(CCameraModeInstance* instance) const;
    virtual int32_t GetType() const;
    virtual void InitInstance(CCameraModeInstance* instance) const;
    virtual void UpdateInstance(CCameraModeInstance* instance, float elapsed) const;
    virtual void DestroyInstance(CCameraModeInstance* instance) const;
    virtual int32_t AllowDuplicate(const CCameraModeInstance* current, const CCameraModeInstance* instance) const;
};

class CCameraModeLocked : public CCameraMode {
    public:
    C3Vector CalcOffsetTwoTargets(const C3Vector& offset, const C44Matrix& target, const C44Matrix& secondTarget, CAMERA_MODE_LOCKED_OFFSET_BASE base, CAMERA_MODE_LOCKED_OFFSET_DIRECTION direction, int32_t flatten) const;

    CCameraModeLocked(const CameraModeRec* rec);
    void CalcTargetCameraShot(CCameraModeInstance* instance, CCameraShot& shot) const;

    virtual CCameraModeInstance* AllocInstance(const CCameraModeInstance& instance) const;
    virtual void FreeInstance(CCameraModeInstance* instance) const;
    virtual int32_t GetType() const;
    virtual void InitInstance(CCameraModeInstance* instance) const;
    virtual void UpdateInstance(CCameraModeInstance* instance, float elapsed) const;
    virtual void DestroyInstance(CCameraModeInstance* instance) const;
    virtual int32_t IsTracking(const CCameraModeInstance* instance, WOWGUID guid) const;
};

#include "WoW/Source/UIUtil/CameraMode.inl"

#endif

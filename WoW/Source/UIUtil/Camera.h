#ifndef WOW_SOURCE_UIUTIL_CAMERA_H
#define WOW_SOURCE_UIUTIL_CAMERA_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Tempest/C3Vector.h"
#include "Engine/Source/Tempest/C33Matrix.h"
#include "Engine/Source/Tempest/C34Matrix.h"
#include "Engine/Source/Storm/H/stpl.h"
#include "Engine/Source/Base/Handle.h"
#include "WoW/Source/Object/WGUID.h"
#include "WoW/Source/UIUtil/CSimpleCamera.h"
#include "Engine/Source/Services/CameraInt.h"

class CGObject_C;
class CGUnit_C;
class CM2Model;
class CRect;
class CVehicleCamera_C;
struct CVar;

class CAMERA_DEFAULTS {
    public:
    /* 0000 */ const char* m_values [3];
};

class CAMERA_SMOOTH {
    public:
    /* 0000 */ uint32_t m_startTimeMs;
    /* 0004 */ float m_rate;
    /* 0008 */ float m_target;
    /* 000C */ float m_startValue;
    /* 0010 */ float m_param3;
    /* 0014 */ float m_param4;
};

class CCamera;
class CGInputControl;

class CGCamera : public CSimpleCamera {
    public:
    int32_t GetCameraDistance(float& distance, const C3Vector& position, const C3Vector& target, uint32_t flags);
    void ResetView(int32_t view);
    void SetView(int32_t view, int32_t blendStyle, int32_t transition);
    void BumpCameraPositionFromLiquid(C3Vector& target, float distance, C3Vector& position);
    int32_t CanSmoothTarget();
    int32_t CanSmoothYaw(float yawMin, float yawMax);
    uint32_t CollideCameraWithWorld(const C3Vector& target, float& distance, float& height, const C3Vector& offset, float liquid, float& extent);
    void RunBobs(C3Vector& offset);
    void UpdateMotion(uint32_t time);
    void UpdateTargetSmoothing(CGObject_C* target, uint32_t time);
    void UpdateUncontrolledState(int32_t uncontrolled);
    void UpdateVehiclePitchOffset(uint32_t time);
    void UpdateVehicleYawOffset(uint32_t time);
    void CalcModelCamera(uint32_t time);
    void UpdateFromModelCamera();
    void CalcTargetCamera(CGObject_C* target, uint32_t time);
    void SyncFreeLookFacing();
    void SyncFreeLookPitch(float pitch, uint32_t time, int32_t force);
    void UpdateFreeLookFacing(float x, float y, float* a3);
    void SetTelescope(int32_t telescope);
    static int32_t UpdateCallback(const void* data, void* param);
    static int32_t CCommand_PitchLimit(const char* command, const char* arguments);
    static uint32_t GetCollisionQueryFlags();
    static float GetDefaultZoomMaxDistance();
    float GetDefaultZoomMinDistance();
    void SetPositionAndTargetWithRoll(const C3Vector& position, const C3Vector& target, float roll);
    void CommentatorSetViewAndPosition(const C3Vector& position, float facing, float pitch);
    void BarberShopSetView(float distance, float facing, float pitch);
    int32_t FinishLoadingModel();

    /* 003C */ CM2Model* m_model;
    /* 0040 */ uint32_t m_someTime;
    /* 0044 */ CCamera* m_modelCamera;
    /* 0048 */ C34Matrix m_modelMatrix;
    /* 0078 */ WOWGUID m_targetGUID;
    /* 0080 */ uint32_t m_state;
    /* 0084 */ uint32_t m_flags;
    /* 0088 */ WOWGUID m_relativeTo;
    /* 0090 */ float m_terrainTilt;
    /* 0094 */ int32_t m_ignoreFacingRefs;
    /* 0098 */ int32_t m_groundTiltRefs;
    /* 009C */ uint32_t m_viewIndex;
    /* 00A0 */ float m_views [8][3];
    /* 0100 */ float m_distance;
    /* 0104 */ float m_yaw;
    /* 0108 */ float m_pitch;
    /* 010C */ float m_roll;
    /* 0110 */ float m_height;
    /* 0114 */ float m_yawOffset;
    /* 0118 */ float m_targetOffset;
    /* 011C */ float m_groundTilt;
    /* 0120 */ float m_targetFov;
    /* 0124 */ float m_flyingMountHeight;
    /* 0128 */ C3Vector m_bobOffset;
    /* 0134 */ float m_targetHeightNear;
    /* 0138 */ float m_targetHeightFar;
    /* 013C */ float m_targetHeightSwim;
    /* 0140 */ float m_mountHeight;
    /* 0144 */ uint32_t m_heightChangedTimeMs;
    /* 0148 */ uint8_t m_fade;
    /* 014C */ uint32_t m_motionFlags;
    /* 0150 */ uint32_t m_motionStartTime [6];
    /* 0168 */ uint32_t m_motionStopTime [6];
    /* 0180 */ uint32_t m_motionEndTime [6];
    /* 0198 */ float m_motionSpeed [6];
    /* 01B0 */ C3Vector m_targetPosition;
    /* 01BC */ float unk_01BC;
    /* 01C0 */ float m_targetFacing;
    /* 01C4 */ uint32_t m_terrainTiltTime;
    /* 01C8 */ float unk_01C8;
    /* 01CC */ CAMERA_SMOOTH m_smoothDistance;
    /* 01E4 */ CAMERA_SMOOTH m_smoothGroundTilt;
    /* 01FC */ CAMERA_SMOOTH m_smoothHeight;
    /* 0214 */ CAMERA_SMOOTH m_smoothPitch;
    /* 022C */ CAMERA_SMOOTH m_smoothTargetOffset;
    /* 0244 */ CAMERA_SMOOTH m_smoothYaw;
    /* 025C */ CAMERA_SMOOTH m_smoothFoV;
    /* 0274 */ uint32_t m_bobTime;
    /* 0278 */ float m_bobDuration;
    /* 027C */ C3Vector m_bobStart;
    /* 0288 */ uint32_t unk_0288;
    /* 028C */ int32_t m_underwater;
    /* 0290 */ uint32_t m_wasFlying;
    /* 0294 */ CAMERA_SMOOTH m_smoothFlyingHeight;
    /* 02AC */ float m_collideExtent;
    /* 02B0 */ uint32_t m_vehicleZoomEnabled;
    /* 02B4 */ float m_overrideDistanceMin;
    /* 02B8 */ float m_overrideDistanceMax;
    /* 02BC */ float m_vehicleExitZoomDistance;
    /* 02C0 */ uint32_t m_vehicleYawStartTime;
    /* 02C4 */ uint32_t m_vehicleYawEndTime;
    /* 02C8 */ float m_vehicleYawFrom;
    /* 02CC */ float m_vehicleYawTo;
    /* 02D0 */ float m_vehicleYawOffset;
    /* 02D4 */ uint32_t m_vehiclePitchStartTime;
    /* 02D8 */ uint32_t m_vehiclePitchEndTime;
    /* 02DC */ float m_vehiclePitchFrom;
    /* 02E0 */ float m_vehiclePitchTo;
    /* 02E4 */ float m_vehiclePitchOffset;
    /* 02E8 */ uint32_t m_vehicleTrackingStyle;
    /* 02EC */ uint32_t m_prevViewIndex;
    /* 02F0 */ float m_prevDistance;
    /* 02F4 */ float unk_02F4;
    /* 02F8 */ float unk_02F8;
    /* 02FC */ CVehicleCamera_C* m_vehicleCamera;

    float FOV() const override;
    C3Vector Forward() const override;
    C3Vector Right() const override;
    C3Vector Up() const override;

    CGCamera();
    ~CGCamera();
    void BeginBobbingSmooth();
    void CalcTerrainTilt(CGObject_C* target, uint32_t time);
    void CameraTeleported(float oldFacing);
    uint32_t CanSmoothTargetFacing(CGObject_C* target);
    void CancelSmoothPitch();
    void CancelSmoothTargetOffset();
    void CancelSmoothYaw();
    void CheckUnderwater();
    void ClampPitchAndNormalize();
    void ClampPitchToLimits(float delta);
    void ClearModelCamera();
    int32_t SetModelCamera(const char* fileName, const C3Vector* position, float facing, void (*callback)(CM2Model* model, uint32_t boneId, uint32_t sequenceId, int32_t interrupted, uint32_t time, uint64_t param), uint64_t param);
    void ClearVehicleExitZoomDistance();
    void CreateViewFromCamera(uint32_t view);
    void DecIgnoreFacing();
    void DisableFreeLook(int32_t keepSmoothing);
    void EnableFreeLook();
    int32_t FinishLoadingTarget(CGObject_C* target);
    C3Vector GetCameraPosition(const C3Vector& target, float distance, const C3Vector& offset);
    float GetChaseFacing(CGUnit_C* unit) const;
    C3Vector GetSafeWorldPos(CGObject_C* target) const;
    int32_t HasModelCamera();
    int32_t HasTargetOffset();
    void IncIgnoreFacing();
    void InitializeView();
    uint32_t IsBobbingAllowed(CGObject_C* target);
    int32_t IsCustomViewSmoothingActive();
    int32_t IsFirstPersonView();
    int32_t IsViewDefault(uint32_t view);
    void LoadCameraSettings();
    void MakeRelativeTo(WOWGUID guid);
    C33Matrix ParentToWorld() const;
    void PerformTerrainTilt(CGObject_C* target, uint32_t time, int32_t immediate);
    void PickVehicleCamera();
    void OnVehicleCameraDestroyed();
    void ResetModelCamera();
    void RotateOffsetByRoll(C3Vector& offset);
    void SaveCameraSettings();
    void SaveViewToCVars(uint32_t view, float distance, float pitch, float yaw);
    int32_t SetDesiredFoVOffset(float offset, float delay, float scale, uint32_t time);
    int32_t SetDesiredGroundOffset(float offset, float delay, float scale, uint32_t time);
    int32_t SetDesiredPitchAngle(float pitch, float delay, float scale, uint32_t time);
    int32_t SetDesiredTargetOffset(float offset, float delay, float scale, uint32_t time);
    void SetDesiredVehiclePitchOffset(float offset, uint32_t startTime, uint32_t endTime);
    void SetDesiredVehicleYawOffset(float offset, uint32_t startTime, uint32_t endTime);
    int32_t SetDesiredYawAngle(float yaw, float delay, float scale, uint32_t time);
    void SetModeFreeLook();
    void SetModeNormal();
    void SetTarget(CGObject_C* target, int32_t freeLook);
    void SetVehicleExitZoomDistance(float distance);
    void SetVehicleSeatZoomMaxDistance(float distance);
    void SetVehicleSeatZoomMaxDistanceToDefault();
    void SetVehicleSeatZoomMinDistance(float distance);
    void SetVehicleSeatZoomMinDistanceToDefault();
    void SetVehicleTrackingStyle(uint32_t style);
    void SetVehicleZoomEnabled(int32_t enabled, float duration, int32_t animate);
    int32_t ShouldSmoothPitch(float minPitch, float maxPitch);
    int32_t SmoothSetDistance(float distance, float delay, float scale, uint32_t time);
    int32_t SmoothSetFlyingMountHeight(float height, float delay, float scale, uint32_t time);
    int32_t SmoothSetHeight(float height, float delay, float scale, uint32_t time);
    float SmoothTargetFacing(float facing, int32_t moved);
    void StartMotion(uint32_t motion, uint32_t time, uint32_t duration, float speed);
    int32_t StartSmoothDistance(float distance, float rate, uint32_t startTime);
    int32_t StartSmoothFlyingMountHeight(float height, float rate, uint32_t startTime);
    int32_t StartSmoothFoV(float offset, float rate, uint32_t startTime);
    int32_t StartSmoothGroundTilt(float offset, float rate, uint32_t startTime);
    int32_t StartSmoothHeight(float height, float rate, uint32_t startTime);
    int32_t StartSmoothPitch(float pitch, float rate, uint32_t startTime);
    int32_t StartSmoothTargetOffset(float offset, float rate, uint32_t startTime);
    int32_t StartSmoothYaw(float yaw, float rate, uint32_t startTime);
    void UpdateAutoInteractState(int32_t enabled);
    void UpdateBobbingState(int32_t moving);
    void SmoothFreeLook(CGInputControl* input, int32_t idle);
    void UpdateTrackingState(int32_t tracking);
    int32_t UpdateInebriation(float drunkness, int32_t immediate);
    float UpdateLiquidSurfaceStatus(CGObject_C* target);
    void UpdateMountHeightOrOffset(CGUnit_C* unit);
    void UpdateTargetFacing(CGObject_C* target, float& yaw, float& pitch, float& roll);
    void UpdateTargetHeight(CGObject_C* target, uint32_t time);
    void UpdateVehicleTarget(uint32_t time);
    void UpdateYaw(float delta);
    void ZoomIn(float distance, uint32_t time, float duration);
    void ZoomOut(float distance, uint32_t time, float duration);
    void ZoomToDistance(float distance, uint32_t time, float duration, int32_t lock);
};

extern float MAX_PITCH_ANGLE;
extern float MIN_PITCH_ANGLE;
extern int32_t s_cameraViewAngular[3];
extern CAMERA_DEFAULTS s_cameraViewDefaults[8];
extern const float s_terrainTiltTable[10][2];
extern float s_targetHeightOffset;
extern float s_cameraBobFrequency;
extern float s_cameraBobVerticalAmplitude;
extern float s_cameraBobHorizontalAmplitude;

class CameraCVarRef {
    public:
    static CameraCVarRef* s_head;
    static uint32_t s_count;

    /* 0000 */ CVar* m_cvar;
    /* 0004 */ CameraCVarRef* m_next;

    CameraCVarRef();
    CameraCVarRef& operator=(CVar* cvar);
    CVar* operator->() const;
    operator CVar*() const;
};

extern CameraCVarRef s_cvCameraView;
extern CameraCVarRef s_cvCameraViewData[8][3];
extern CameraCVarRef s_cameraSmooth;
extern CameraCVarRef s_cvCameraSmoothStyle;
extern CameraCVarRef s_cvCameraTerrainTilt;
extern CameraCVarRef s_cvCameraTerrainTiltState[5][10][3];
extern CameraCVarRef s_cvCameraTerrainTiltTimeMin;
extern CameraCVarRef s_cvCameraTerrainTiltTimeMax;
extern CameraCVarRef s_cvCameraGroundSmoothSpeed;
extern CameraCVarRef s_cvCameraFoVSmoothSpeed;
extern CameraCVarRef s_cvCameraHeightSmoothSpeed;
extern CameraCVarRef s_cvCameraFlyingMountHeightSmoothSpeed;
extern CameraCVarRef s_cvCameraHeightIgnoreStandState;
extern CameraCVarRef s_cvCameraSavedDistance;
extern CameraCVarRef s_cvCameraSavedVehicleDistance;
extern CameraCVarRef s_cvCameraSavedPitch;
extern CameraCVarRef s_cvCameraPivot;
extern CameraCVarRef s_cvCameraSmoothPitch;
extern CameraCVarRef s_cvCameraBobbing;
extern CameraCVarRef s_cvCameraBobbingSmoothSpeed;
extern CameraCVarRef s_cvCameraCustomViewSmoothing;
extern CameraCVarRef s_cvCameraDistanceMoveSpeed;
extern CameraCVarRef s_cvCameraDistanceSmoothSpeed;
extern CameraCVarRef s_cvCameraPitchSmoothSpeed;
extern CameraCVarRef s_cvCameraTargetSmoothSpeed;
extern CameraCVarRef s_cvCameraYawSmoothSpeed;
extern CameraCVarRef s_cameraWaterCollision;
extern CameraCVarRef s_cvCameraSmoothTimeMax;
extern CameraCVarRef s_cvCameraSmoothTimeMin;
extern CameraCVarRef s_cvCameraYawSmoothMax;
extern CameraCVarRef s_cvCameraYawSmoothMin;
extern CameraCVarRef s_cvCameraPitchSmoothMax;
extern CameraCVarRef s_cvCameraPitchSmoothMin;
extern CameraCVarRef s_cvCameraDistanceMaxFactor;
extern CameraCVarRef s_cvCameraDistanceMax;
extern CameraCVarRef s_cvCameraSubmergeFinalPitch;
extern CameraCVarRef s_cvCameraSurfaceFinalPitch;
extern CameraCVarRef s_cvCameraSubmergePitch;
extern CameraCVarRef s_cvCameraSurfacePitch;
extern CameraCVarRef s_cvCameraDive;
extern CameraCVarRef s_cvCameraPivotDYMin;
extern CameraCVarRef s_cvCameraPivotDXMax;
extern CameraCVarRef s_cvCameraSmoothViewData[5][3][2];
extern CameraCVarRef s_cvCameraSmoothState[5][7][2];
extern CameraCVarRef s_cvCameraSmoothYaw;
extern CameraCVarRef s_cvCameraSmoothTrackingStyle;
extern CameraCVarRef s_cvCameraViewBlendStyle;
extern CameraCVarRef s_cvCameraYawMoveSpeed;
extern CameraCVarRef s_cvCameraPitchMoveSpeed;
extern CameraCVarRef s_cvMouseInvertPitch;
extern CameraCVarRef s_cvMouseInvertYaw;

void CameraRegisterCVars();

void CameraRegisterScriptFunctions();
void CameraUnregisterScriptFunctions();

float CameraEaseInOut(float t);

#include "WoW/Source/UIUtil/Camera.inl"

#endif

#include <cmath>
#include <cstdint>
#include <new>
#include "Engine/Source/BlizzardCore/SFile2-Core.h"
#include "Engine/Source/ConsoleBase/ConsoleVar.h"
#include "Engine/Source/Event/CFocusEvent.h"
#include "Engine/Source/Event/CMouseEvent.h"
#include "Engine/Source/Frame/CSimpleTop.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameXML/XMLTree.h"
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "Engine/Source/Os/Joystick.h"
#include "Engine/Source/Os/TimeManager.h"
#include "Engine/Source/SteelseriesMouse/WowMouse.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/Tempest/C2Vector.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/SpellRec.h"
#include "WoW/Source/DB/Rec/VehicleRec.h"
#include "WoW/Source/DB/Rec/VehicleSeatRec.h"
#include "WoW/Source/Magic/MagicClient/Spell_C.h"
#include "WoW/Source/Object/CMoveSpline.h"
#include "WoW/Source/Object/Movement.h"
#include "WoW/Source/Object/ObjectClient/Movement_C.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/Object/ObjectClient/Unit_C.h"
#include "WoW/Source/Object/ObjectClient/Vehicle_C.h"
#include "WoW/Source/Object/ObjectClient/VehicleCamera_C.h"
#include "WoW/Source/Object/ObjectClient/VehiclePassenger_C.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/Ui/CommentatorFrame.h"
#include "WoW/Source/Ui/GameUI.h"
#include "WoW/Source/Ui/ScriptEvents.h"
#include "WoW/Source/Ui/UIBindings.h"
#include "WoW/Source/Ui/WorldFrame.h"
#include "WoW/Source/UIUtil/Camera.h"
#include "WoW/Source/UIUtil/InputControl.h"

// STATIC: 0x00C992F8
uint32_t s_joystick = 0xFFFFFFFF;

// STATIC: 0x00C992FC
int32_t s_joystickMouseEnabled = 1;

// STATIC: 0x00D39204
CGInputControl* CGInputControl::s_inputControl;

// STATIC: #0x00C24958
float s_vehicleAimNormPower;

// PROVEN: 82e97808 IDENTICAL/NONE
// OFFSET: 0x00447970/0x1400541C0
CGInputControl* CGInputControl::GetActive() {
    return CGInputControl::s_inputControl;
}

// OFFSET: #0x005F9670
void CGInputControl::UpdateJoystickMouseMode() {
    int32_t enable = s_joystickMouseEnabled && !s_cvCinematicJoystick->m_intValue && (this->m_joystickMouseFlags & 0x2) && (this->m_mouseModeFlags & 0x1);

    if (enable == (this->m_joystickMouseFlags & 0x1)) {
        return;
    }

    if (enable) {
        this->m_joystickMouseFlags |= 0x1;
        OsJoystickSetMouseMode(s_joystick, enable);
    } else {
        this->m_joystickMouseFlags &= ~0x1;
        OsJoystickSetMouseMode(s_joystick, 0);
    }
}

// OFFSET: 0x00447A20
void CGInputControl::SetMouseCVarHandler(bool enable) {
    if (!this->m_wowMouse) {
        if (enable) {
            this->m_wowMouse = new (SMemAlloc(sizeof(WowMouse), __FILE__, __LINE__, 0)) WowMouse();
        }
    } else if (!enable) {
        delete this->m_wowMouse;
        this->m_wowMouse = NULL;
    }
}

// OFFSET: #0x005FD1D0
CGInputControl::CGInputControl() : m_flags(0), m_lookStartTime(0), m_pendingDefaultAction(PENDING_ACTION_NONE), m_facingOverrideActive(0), m_facingOverride(0.0f), m_vehicleAimValid(0), m_vehicleAim(0.0f), m_mouseModeFlags(3), m_forceCursorOn(0), m_joystickMouseFlags(3), m_joystickLookX(0.0f), m_joystickLookY(0.0f), m_wowMouse(NULL) {
    this->m_time = OsGetAsyncTimeMs();
}

// OFFSET: #0x005FD2C0
void InputControlInitialize() {
    CVar::Register("Joystick", "enable joystick control", 0, "0", JoystickCallback, 5, false, NULL, false);
    s_cvCinematicJoystick = CVar::Register("CinematicJoystick", "enable cinematic joystick control", 0, "0", NULL, 5, false, NULL, false);
    CGInputControl::s_inputControl = new (SMemAlloc(sizeof(CGInputControl), __FILE__, __LINE__, 0)) CGInputControl();
    s_cvEnableWowMouse = CVar::Register("enableWowMouse", "Enable Steelseries World of Warcraft Mouse", 1, "0", WowMouseCVarCallback, 5, false, NULL, false);
}

// OFFSET: 0x00448BA0
void CGInputControl::UpdateMouseMode(int32_t force) {
    uint32_t flags = this->m_mouseModeFlags;
    uint32_t cursorOn;

    if (flags & 0x20) {
        cursorOn = 1;
    } else if (flags & 0x40) {
        cursorOn = 0;
    } else if (s_cvCinematicJoystick->m_intValue || (flags & 0x10) || this->m_forceCursorOn) {
        cursorOn = 1;
    } else {
        cursorOn = flags & 0x6;
    }

    if (force || cursorOn != (flags & 0x1)) {
        if (cursorOn) {
            this->m_mouseModeFlags = flags | 0x1;

            if (!(this->m_flags & 0x3)) {
                g_theGxDevicePtr->CursorSetVisible(1);
            }
        } else {
            this->m_mouseModeFlags = flags & ~0x1;
            g_theGxDevicePtr->CursorSetVisible(0);
        }
    }

    this->UpdateJoystickMouseMode();
}

// INLINED
CGInputControl::~CGInputControl() {
    if (this->m_wowMouse) {
        delete this->m_wowMouse;
    }

    this->m_wowMouse = NULL;
}

// OFFSET: 0x0044B900
void CGInputControl::Destroy() {
    if (CGInputControl::s_inputControl) {
        delete CGInputControl::s_inputControl;
    }

    CGInputControl::s_inputControl = NULL;
}

// OFFSET: #0x005FD430
void InputControlDestroy() {
    CGInputControl::Destroy();
}

// OFFSET: 0x00447980/0x1400541D0
void CGInputControl::UpdateHalted() {
    this->m_flags &= ~0xF0000;
}

// PROVEN: 1e1d90d0 IDENTICAL/NONE
// OFFSET: 0x00447990/0x1400541E0
void CGInputControl::UpdateMoveStopped() {
    this->m_flags &= ~0x11000;
}

// PROVEN: ad9f3291 IDENTICAL/NONE
// OFFSET: 0x004479A0
int32_t CGInputControl::IsMouseDrag(uint32_t time) const {
    uint32_t elapsed = time - this->m_lookStartTime;

    if (static_cast<int32_t>(elapsed - 800) >= 0) {
        return 1;
    }

    if (this->m_dragAccumX >= 8.0f || this->m_dragAccumY >= 8.0f) {
        return static_cast<int32_t>(elapsed - 200) >= 0;
    }

    return 0;
}

// PROVEN: c7ff3e4f IDENTICAL/NONE
// OFFSET: 0x00447A10/0x140054220
float CGInputControl::GetUIVehiclePower() {
    return s_vehicleAimNormPower;
}

// PROVEN: ef6fb063 IDENTICAL/NONE
// OFFSET: 0x00448D30/0x140055850
int32_t CGInputControl::WowMouseEnabled() {
    return s_cvEnableWowMouse && s_cvEnableWowMouse->m_intValue;
}

// OFFSET: 0x00447A80
int32_t CGInputControl::WowMouseConnected() {
    return this->m_wowMouse && this->m_wowMouse->IsConnected();
}

// OFFSET: 0x00447AA0
bool CGInputControl::DetectWowMouse() {
    s_cvEnableWowMouse->Set("1", true, false, false, true);

    if (this->m_wowMouse) {
        bool connected = this->m_wowMouse->IsConnected();

        if (connected) {
            return connected;
        }
    }

    FrameScript_SignalEvent(SCRIPT_EVENT_WOW_MOUSE_NOT_FOUND, NULL);
    return s_cvEnableWowMouse->Set("0", true, false, false, true);
}

// PROVEN: a91e3341 IDENTICAL/NONE
// OFFSET: 0x0043BD20
int32_t CGInputControl::IsMouseDragging() const {
    uint32_t flags = this->m_flags;

    return (flags & 0x300) && !(flags & 0x2000001);
}

// PROVEN: 89b7ef7c IDENTICAL/NONE
// OFFSET: 0x0043BD40
int32_t CGInputControl::IsIdle() const {
    uint32_t flags = this->m_flags;

    return !(flags & 0x1030) && !(flags & 0xC0) && (!(flags & 0x2000001) || !(flags & 0x300)) && (!(flags & 0x300) || (flags & 0x2000001)) && !(flags & 0x1E00000);
}

// OFFSET: 0x00447B70
XMLTree* LoadJoystickConfig(XMLNode*& config) {
    config = NULL;

    if (s_joystick == 0xFFFFFFFF) {
        return NULL;
    }

    char joystickName[1024];
    OsJoystickGetName(s_joystick, joystickName, sizeof(joystickName));

    void* buffer;
    uint32_t bytes;

    if (!SFile::ReadFile(NULL, "Interface\\Joystick\\JoystickConfig.xml", &buffer, &bytes, 0, 1)) {
        return NULL;
    }

    XMLTree* tree = XMLTree_Load(static_cast<const char*>(buffer), bytes);
    SMemFree(buffer, __FILE__, __LINE__, 0);

    if (!tree) {
        return NULL;
    }

    for (XMLNode* joystick = XMLTree_GetRoot(tree)->m_child; joystick; joystick = joystick->m_next) {
        if (SStrCmpI(joystick->m_name.m_str, "Joystick", STORM_MAX_STR)) {
            continue;
        }

        const char* platform = joystick->GetAttributeByName("platform");

        if (platform && SStrCmpI(platform, "windows", STORM_MAX_STR)) {
            continue;
        }

        for (XMLNode* child = joystick->m_child; child; child = child->m_next) {
            if (SStrCmpI(child->m_name.m_str, "name", STORM_MAX_STR)) {
                continue;
            }

            const char* name = child->GetAttributeByName("name");

            if (name && !SStrCmpI(name, joystickName, STORM_MAX_STR)) {
                config = joystick;
                return tree;
            }
        }
    }

    XMLTree_Free(tree);

    return NULL;
}

// OFFSET: 0x00448000
CGUnit_C* GetVehicleUnitPtr() {
    CGUnit_C* mover = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(CGUnit_C::s_activeMover, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (!mover) {
        return NULL;
    }

    if (!mover->m_vehiclePassenger || mover->m_vehiclePassenger->m_state != 3) {
        return mover;
    }

    CGUnit_C* vehicle = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(WOWGUID::Make(mover->GetTransportGUID()), TYPEMASK_UNIT, __FILE__, __LINE__));

    if (!vehicle || !vehicle->m_vehicle) {
        return mover;
    }

    VehicleSeatRec* seat = vehicle->m_vehicle->GetVehicleSeatRec(mover->m_movementData.m_transportSeat);

    if (!seat || !(seat->m_flags & 0x800)) {
        return mover;
    }

    return vehicle;
}

// OFFSET: 0x00448370/0x140054CE0
int32_t __stdcall CGInputControl::CanControl(CGUnit_C* unit) {
    if (!unit || unit->m_unit->m_unitFieldHealth <= 0) {
        return 0;
    }

    CMoveSpline* spline = unit->m_movement->m_spline;

    if (spline && !(spline->m_flags & 0x400)) {
        return 0;
    }

    return !unit->AnimSuppressesMovement() && (!unit->IsActivePlayer() || !(static_cast<CGPlayer_C*>(unit)->m_combatModeFlags & 0x1)) && !unit->IsVehicleTransitionPreventingAction();
}

// OFFSET: 0x004489B0
int32_t CGInputControl::CameraCanTurnPlayer() const {
    CGUnit_C* mover = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(CGUnit_C::s_activeMover, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (!mover) {
        return 0;
    }

    CGCamera* camera = CGWorldFrame::GetActiveCamera();

    if (!(((mover->m_obj->m_type >> 4) & 1) && static_cast<CGPlayer_C*>(mover)->IsCommentatorUberOrInArena())) {
        if (mover->m_unit->m_unitFieldHealth <= 0) {
            return 0;
        }

        CMoveSpline* spline = mover->m_movement->m_spline;

        if (spline && !(spline->m_flags & 0x400)) {
            return 0;
        }

        if ((mover->m_unit->m_unitFieldFlags & 0x40000) || mover->IsVehicleTransitionPreventingAction() || mover->GetClientStandState()) {
            return 0;
        }

        if ((camera->m_targetGUID) != mover->m_obj->m_guid) {
            return 0;
        }
    }

    if (!(camera->m_state & 0x1)) {
        return 0;
    }

    return (this->m_flags & 0x2000001) != 0;
}

// OFFSET: 0x00448C30
void CGInputControl::SendUIVehicleAngleUpdate(float angle) {
    CGUnit_C* mover = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(CGUnit_C::s_activeMover, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (!mover) {
        return;
    }

    VehicleRec* rec = mover->m_vehicle ? mover->m_vehicle->m_vehicleRec : NULL;

    if (!rec) {
        return;
    }

    float pitchMin;
    float pitchMax;

    if (rec->m_flags & 0x40) {
        pitchMin = rec->m_pitchMin;
        pitchMax = rec->m_pitchMax;
    } else {
        pitchMin = -1.5707964f;
        pitchMax = 1.5707964f;
    }

    float range = pitchMax - pitchMin;
    float normalized = fabsf(range) <= 0.000099999997f ? 0.0f : (angle - pitchMin) / range;

    FrameScript_SignalEvent(SCRIPT_EVENT_VEHICLE_ANGLE_UPDATE, "%f%f", static_cast<double>(normalized), static_cast<double>(angle));
}

// OFFSET: 0x00449910
int32_t CGInputControl::IsVehicleSeatFlagSet(uint32_t flags) const {
    CGUnit_C* mover = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(CGUnit_C::s_activeMover, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (!mover) {
        return 0;
    }

    CGUnit_C* player = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player || !player->m_vehiclePassenger || player->m_vehiclePassenger->m_state != 3) {
        return 0;
    }

    if (player->GetTransportGUID() != mover->m_obj->m_guid.m_guid && player->m_obj->m_guid != mover->m_obj->m_guid) {
        return 0;
    }

    CGUnit_C* vehicle = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(WOWGUID::Make(player->GetTransportGUID()), TYPEMASK_UNIT, __FILE__, __LINE__));

    if (!vehicle) {
        return 0;
    }

    VehicleSeatRec* seat = vehicle->GetVehicleSeatRec(player->m_movementData.m_transportSeat);

    return seat && (seat->m_flags & flags);
}

// OFFSET: 0x0044B970
void CGInputControl::SetMouselookOverrideBinding(const char* key, const char* binding) {
    if (!binding) {
        MOUSELOOKBINDING* entry = this->m_mouseLookBindings.Ptr(key);

        if (entry) {
            this->m_mouseLookBindings.Delete(entry);
        }

        return;
    }

    MOUSELOOKBINDING* entry = this->m_mouseLookBindings.Ptr(key);

    if (entry) {
        if (SStrCmpI(entry->m_binding, binding, STORM_MAX_STR)) {
            SMemFree(entry->m_binding, __FILE__, __LINE__, 0);
            entry->m_binding = SStrDupA(binding, __FILE__, __LINE__);
        }
    } else {
        entry = this->m_mouseLookBindings.New(key, 0, 0);
        entry->m_binding = SStrDupA(binding, __FILE__, __LINE__);
    }
}

// OFFSET: 0x004483E0
int32_t __stdcall CGInputControl::CanTurn(CGUnit_C* unit) {
    return CGInputControl::CanControl(unit) && !(unit->m_unit->m_unitFieldFlags & 0x40000) && !unit->IsVehiclePreventingTurning();
}

// OFFSET: 0x00448430
int32_t CGInputControl::CanSyncFreeLookFacing(CGUnit_C* unit) const {
    return CGInputControl::CanControl(unit) && !(unit->m_unit->m_unitFieldFlags & 0x40000) && !unit->IsVehiclePreventingTurning() && (this->m_flags & 0x2000001) && !unit->GetClientStandState();
}

// OFFSET: #0x005FA420
int32_t CGInputControl::IsMouseDragMoving() const {
    int32_t result = this->IsMouseDrag(OsGetAsyncTimeMs());

    if (result) {
        return g_theGxDevicePtr->m_frameCount - this->m_lastMouseMoveFrame < 2;
    }

    return result;
}

// OFFSET: 0x00448AA0
int32_t CGInputControl::CameraCanPitchPlayer() {
    CGUnit_C* mover = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(CGUnit_C::s_activeMover, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (!mover) {
        return 0;
    }

    CGCamera* camera = CGWorldFrame::GetActiveCamera();

    if (((mover->m_obj->m_type >> 4) & 1) && static_cast<CGPlayer_C*>(mover)->IsCommentatorUberOrInArena()) {
        return 1;
    }

    if (mover->m_unit->m_unitFieldHealth <= 0) {
        return 0;
    }

    CMoveSpline* spline = mover->m_movement->m_spline;

    if (spline && !(spline->m_flags & 0x400)) {
        return 0;
    }

    if (mover->IsVehicleTransitionPreventingAction() || (mover->m_unit->m_unitFieldFlags & 0x40000) || mover->GetClientStandState()) {
        return 0;
    }

    CMovement_C* movement = mover->m_movement;

    if ((movement->m_flags2 & 0x20) && !(movement->m_flags & 0x2200000)) {
        VehicleRec* rec = mover->GetVehicleRec();

        if (!rec || !(rec->m_flags & 0x40000)) {
            return 0;
        }
    }

    if ((camera->m_targetGUID) != mover->m_obj->m_guid) {
        return 0;
    }

    return 1;
}

// OFFSET: [0x00449030]
void CGInputControl::EnterWorld() {
    uint32_t time = OsGetAsyncTimeMs();
    this->UnsetControlBit(0xFFFFFFFF, time, 0);
    this->m_time = time;
    this->m_joystickLookX = 0.0f;
    this->m_joystickLookY = 0.0f;
}

// OFFSET: 0x00449060
int32_t __stdcall CGInputControl::CanMove(CGUnit_C* unit) {
    if (!CGInputControl::CanControl(unit)) {
        return 0;
    }

    return unit->m_unit->m_unitFieldHealth > 0 && !(unit->m_movement->m_flags & 0x100A00) && unit->m_unit->m_unitFieldBytes1[0] != 7 && !unit->HasVehicleTransport();
}

// OFFSET: #0x00568560
void __stdcall ScaleMouseDeltaByFov(float* dx, float* dy) {
    if (!CGWorldFrame::s_currentWorldFrame) {
        return;
    }

    CGCamera* camera = CGWorldFrame::GetActiveCamera();

    if (!camera) {
        return;
    }

    float fovScale = camera->FOV() * 0.81851113f;
    float clamped = 1.0f;

    if (fovScale < 1.0f) {
        clamped = 0.050000001f;

        if (fovScale > 0.050000001f) {
            clamped = fovScale;
        }
    }

    float scale = 0.050000001f + clamped * 0.94999999f;
    *dx = *dx * scale;
    *dy = scale * *dy;
}

// OFFSET: 0x00449850
void CGInputControl::VehicleAimChange(uint32_t time, float delta) {
    CGUnit_C* mover = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(CGUnit_C::s_activeMover, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (!mover) {
        return;
    }

    float aim = this->m_vehicleAimValid ? this->m_vehicleAim : mover->GetPitch();
    this->SetPlayerPitch(mover, time, aim + delta);
}

// OFFSET: 0x004498C0
void CGInputControl::OnPitchToAngleStop() {
    this->m_vehicleAimValid = 0;

    CGUnit_C* mover = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(CGUnit_C::s_activeMover, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (mover) {
        CGInputControl::SendUIVehicleAngleUpdate(mover->GetPitch());
    }
}

// OFFSET: 0x00448F10
void CGInputControl::OnUpdate(float elapsed) {
    CGUnit_C* mover = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(CGUnit_C::s_activeMover, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (!mover) {
        return;
    }

    if (s_joystick != 0xFFFFFFFF && (this->m_flags & 0x6000000)) {
        CGCamera* camera = CGWorldFrame::GetActiveCamera();

        if (camera) {
            // STATIC: #0x00C24964
            static C2Vector s_joystickLookScale = C2Vector(800.0f, 600.0f);

            camera->UpdateFreeLookFacing(this->m_joystickLookX * s_joystickLookScale.x * elapsed, this->m_joystickLookY * s_joystickLookScale.y * elapsed, NULL);

            if (this->CanSyncFreeLookFacing(mover)) {
                camera->SyncFreeLookFacing();
            }
        }
    }

    if (mover->m_movement->m_flags & 0xC0) {
        CGInputControl::SendUIVehicleAngleUpdate(mover->GetPitch());
    }
}

// OFFSET: 0x00449E50
void CGInputControl::OnMouseMoveRel(const CMouseEvent& evt) {
    if (!this->m_flags) {
        return;
    }

    uint32_t frame = g_theGxDevicePtr->m_frameCount;
    float x = evt.m_data.m_mousePosition.x;
    float y = evt.m_data.m_mousePosition.y;
    CGUnit_C* mover = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(CGUnit_C::s_activeMover, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (mover && (mover->m_obj->m_type & TYPEMASK_PLAYER) && static_cast<CGPlayer_C*>(mover)->IsCommentatorUberOrInArena()) {
        ScaleMouseDeltaByFov(&x, &y);
    }

    this->m_lastMouseMoveFrame = frame;
    int32_t allowPitch = 1;
    this->m_dragAccumX = std::fabs(x) + this->m_dragAccumX;
    this->m_dragAccumY = std::fabs(y) + this->m_dragAccumY;
    int32_t canSync = this->CanSyncFreeLookFacing(mover);

    if (canSync && mover && mover->m_vehicle && mover->m_vehicle->m_vehicleRec && mover->m_vehicle->IsSuppressingCameraPitchWhileMouseAiming()) {
        allowPitch = 0;
    }

    CGCamera* camera = CGWorldFrame::GetActiveCamera();

    if (allowPitch) {
        camera->UpdateFreeLookFacing(x, y, NULL);
    } else {
        float pitchDelta;
        camera->UpdateFreeLookFacing(x, y, &pitchDelta);

        if (CGInputControl::CameraCanPitchPlayer()) {
            float aim = this->m_vehicleAimValid ? this->m_vehicleAim : mover->GetPitch();
            this->SetPlayerPitch(mover, OsGetAsyncTimeMs(), aim - pitchDelta);
            this->m_flags &= ~0x80000;
        }
    }

    if (canSync) {
        camera->SyncFreeLookFacing();
    }
}

// OFFSET: 0x00449620
void CGInputControl::CameraTurnPlayer(uint32_t time, float facing) {
    if (!this->CameraCanTurnPlayer()) {
        return;
    }

    CGUnit_C* mover = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(CGUnit_C::s_activeMover, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (!mover) {
        return;
    }

    if ((mover->m_obj->m_type & TYPEMASK_PLAYER) && static_cast<CGPlayer_C*>(mover)->IsCommentatorUberOrInArena()) {
        CGCommentator::s_Commentator.m_facing = facing;
        return;
    }

    if (mover->m_vehicleCamera) {
        mover->ConvertVehicleCameraRelativeFacingToTransportRelativeFacing(facing);
    }

    if (mover->m_movement->m_flags2 & 0x8) {
        if (!this->m_facingOverrideActive || this->m_facingOverride != facing) {
            mover->OnTurnToAngleLocal(time, facing);
            this->m_facingOverride = facing;
        }

        if (!this->m_facingOverrideActive) {
            CGWorldFrame::GetActiveCamera()->IncIgnoreFacing();
            this->m_flags &= ~0x40000;
            this->m_facingOverrideActive = 1;
            return;
        }
    } else {
        mover->OnSetRawFacingLocal(time, facing);
        CVehiclePassenger_C* passenger = mover->m_vehiclePassenger;

        if (passenger && mover->m_vehicleCamera && mover->m_vehicleCamera->IsTransitioningFacing()) {
            passenger->LockWorldFacingNextFrame(mover->CGUnit::GetFacing(facing));
        }
    }

    this->m_flags &= ~0x40000;
}

// OFFSET: 0x00449760
void CGInputControl::SetPlayerPitch(CGUnit_C* unit, uint32_t time, float pitch) {
    VehicleRec* rec = unit->m_vehicle ? unit->m_vehicle->m_vehicleRec : NULL;

    if (rec) {
        float min;
        float max;

        if (rec->m_flags & 0x40) {
            min = rec->m_pitchMin;
            max = rec->m_pitchMax;
        } else {
            min = -1.5707964f;
            max = 1.5707964f;
        }

        if (pitch < min) {
            pitch = min;
        } else if (max <= pitch) {
            pitch = max;
        }
    }

    if (unit->m_movement->m_flags2 & 0x10) {
        if (!this->m_vehicleAimValid || this->m_vehicleAim != pitch) {
            unit->OnPitchToAngleLocal(time, pitch);
            this->m_vehicleAim = pitch;
        }

        this->m_vehicleAimValid = 1;
    } else {
        unit->OnSetPitchLocal(time, pitch);

        if (rec && (rec->m_flags & 0x400)) {
            CGInputControl::SendUIVehicleAngleUpdate(pitch);
        }
    }

    CGUnit_C::PitchMissileTrajectoryTargetingNextFrame();
}

// OFFSET: 0x0044A270
void CGInputControl::CameraPitchPlayer(uint32_t time, float pitch) {
    if (!CGInputControl::CameraCanPitchPlayer()) {
        return;
    }

    CGUnit_C* mover = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(CGUnit_C::s_activeMover, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (!mover) {
        return;
    }

    if ((mover->m_obj->m_type & TYPEMASK_PLAYER) && static_cast<CGPlayer_C*>(mover)->IsCommentatorUberOrInArena()) {
        CGCommentator::s_Commentator.m_pitch = pitch;
        return;
    }

    float value = -pitch;

    if (mover->m_vehicle && mover->m_vehicle->m_vehicleRec) {
        value = mover->m_vehicle->m_vehicleRec->m_mouseLookOffsetPitch - pitch;
    }

    this->SetPlayerPitch(mover, time, value);
    this->m_flags &= ~0x80000;
}

// PROVEN: aacc7a9c IDENTICAL/NONE
// OFFSET: 0x004479F0/0x1400541F0
void CGInputControl::OnTurnToAngleStop() {
    if (this->m_facingOverrideActive) {
        this->m_facingOverrideActive = 0;
        CGWorldFrame::GetActiveCamera()->DecIgnoreFacing();
    }
}

// OFFSET: 0x00448DB0/0x140055940
void CheckToCancelCurrentChannelSpell() {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player || player->m_obj->m_guid != CGUnit_C::s_activeMover) {
        return;
    }

    Spell_C_CancelCastingSpellByMovement(player);
    int32_t channelSpell = player->m_unit->m_unitChannelSpell;

    if (!channelSpell) {
        return;
    }

    SpellRec* spell = g_spellDB.GetRecord(channelSpell);

    if (!spell || !spell->m_spellInterruptsID) {
        return;
    }

    SpellInterruptsRec* interrupts = g_spellInterruptsDB.GetRecord(spell->m_spellInterruptsID);

    if (interrupts && (interrupts->m_channelInterruptFlags & 0x8) && !player->HasAuraMatchingSpellClass(330, spell)) {
        Spell_C_CancelChannelSpell(channelSpell);
    }
}

// OFFSET: 0x004484A0
int32_t CGInputControl::SetControlBit(uint32_t bit, uint32_t time) {
    if (bit & this->m_flags) {
        return 0;
    }

    CGCamera* camera = CGWorldFrame::GetActiveCamera();
    uint32_t flags = this->m_flags;
    int32_t wasIdle = !(flags & 0x1030) && !(flags & 0xC0) && (!(flags & 0x2000001) || !(flags & 0x300)) && (!(flags & 0x300) || (flags & 0x2000001)) && !(flags & 0x1E00000);
    uint32_t wasLooking = flags & 0x6000003;
    uint32_t wasMouseDown = flags & 0x3;
    int32_t wasBothButtons = (flags & 0x1) && (flags & 0x2);
    uint32_t wasRightLook = flags & 0x4000002;
    uint32_t wasLeftLook = flags & 0x2000001;
    int32_t wasMouseTurn = (flags & 0x2000001) && (flags & 0x300);

    this->m_flags = flags | bit;

    if (!wasLooking && (this->m_flags & 0x6000003)) {
        this->m_dragAccumX = 0.0f;
        this->m_dragAccumY = 0.0f;
        this->m_lookStartTime = time;
        camera->EnableFreeLook();
    }

    if (!wasMouseDown && (this->m_flags & 0x3)) {
        CGGameUI::OnMouseModeRelative();
    }

    if (!wasBothButtons && (this->m_flags & 0x1) && (this->m_flags & 0x2)) {
        CGUnit_C* mover = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(CGUnit_C::s_activeMover, TYPEMASK_UNIT, __FILE__, __LINE__));

        if (this->CanSyncFreeLookFacing(mover)) {
            camera->SyncFreeLookFacing();
            this->m_freeLookFacingSynced = 1;
        }
    } else if (!(this->m_flags & 0x1) || !(this->m_flags & 0x2)) {
        this->m_freeLookFacingSynced = 0;
    }

    if (bit & 0x1E00000) {
        camera->UpdateTrackingState(this->m_flags & 0x1E00000);
    }

    if ((bit & 0xA010F0) || (!wasBothButtons && (this->m_flags & 0x1) && (this->m_flags & 0x2)) || (!wasMouseTurn && (this->m_flags & 0x2000001) && (this->m_flags & 0x300))) {
        uint32_t newFlags = this->m_flags;
        camera->UpdateBobbingState((newFlags & 0xA010F0) || ((newFlags & 0x1) && (newFlags & 0x2)) || ((newFlags & 0x2000001) && (newFlags & 0x300)));
    }

    if (this->m_facingOverrideActive && (bit & 0x300)) {
        this->OnTurnToAngleStop();
    }

    if ((bit & 0x13F0) || (!wasRightLook && (this->m_flags & 0x4000002)) || (!wasLeftLook && (this->m_flags & 0x2000001)) || (!wasMouseTurn && (this->m_flags & 0x2000001) && (this->m_flags & 0x300))) {
        camera->SmoothFreeLook(this, !wasIdle && this->IsIdle());
    }

    if (bit & 0x30) {
        this->m_flags &= ~0x1000u;
    }

    if (!wasBothButtons && (this->m_flags & 0x1) && (this->m_flags & 0x2)) {
        this->m_flags &= ~0x1000u;
    }

    if ((bit & 0x3) && (this->m_flags & 0x3) != bit) {
        this->m_pendingDefaultAction = PENDING_ACTION_NONE;
    }

    return 1;
}

// OFFSET: 0x00448750
int32_t CGInputControl::UnsetControlBit(uint32_t bit, uint32_t time, int32_t keepSmoothing) {
    if (!(bit & this->m_flags)) {
        return 0;
    }

    CGCamera* camera = CGWorldFrame::GetActiveCamera();
    uint32_t flags = this->m_flags;
    uint32_t wasLooking = flags & 0x6000003;
    uint32_t wasMouseDown = flags & 0x3;
    int32_t wasIdle = !(flags & 0x1030) && !(flags & 0xC0) && (!(flags & 0x2000001) || !(flags & 0x300)) && (!(flags & 0x300) || (flags & 0x2000001)) && !(flags & 0x1E00000);
    int32_t wasBothButtons = (flags & 0x1) && (flags & 0x2);
    uint32_t wasRightLook = flags & 0x4000002;
    uint32_t wasLeftLook = flags & 0x2000001;
    int32_t wasMouseTurn = (flags & 0x2000001) && (flags & 0x300);

    this->m_flags = flags & ~bit;

    if (wasLooking && !(this->m_flags & 0x6000003)) {
        camera->DisableFreeLook(keepSmoothing);

        if (this->m_facingOverrideActive && (this->m_flags & 0x300)) {
            this->OnTurnToAngleStop();
        }
    }

    if (wasMouseDown && !(this->m_flags & 0x3)) {
        CGGameUI::OnMouseModeNormal();
    }

    if (bit & 0x1E00000) {
        camera->UpdateTrackingState(this->m_flags & 0x1E00000);
    }

    if ((bit & 0xA010F0) || (wasBothButtons && (!(this->m_flags & 0x1) || !(this->m_flags & 0x2))) || (wasMouseTurn && (!(this->m_flags & 0x2000001) || !(this->m_flags & 0x300)))) {
        uint32_t newFlags = this->m_flags;
        camera->UpdateBobbingState((newFlags & 0xA010F0) || ((newFlags & 0x1) && (newFlags & 0x2)) || ((newFlags & 0x2000001) && (newFlags & 0x300)));
    }

    if ((bit & 0x13F0) || (wasRightLook && !(this->m_flags & 0x4000002)) || (wasLeftLook && !(this->m_flags & 0x2000001)) || (wasMouseTurn && (!(this->m_flags & 0x2000001) || !(this->m_flags & 0x300)))) {
        camera->SmoothFreeLook(this, !wasIdle && this->IsIdle());
    }

    if (wasMouseDown && !(this->m_flags & 0x3)) {
        if (!this->IsMouseDrag(time)) {
            if (this->m_pendingDefaultAction == PENDING_ACTION_LEFT) {
                CGWorldFrame::s_currentWorldFrame->PerformDefaultAction(MOUSE_BUTTON_LEFT);
            } else if (this->m_pendingDefaultAction == PENDING_ACTION_RIGHT) {
                CGWorldFrame::s_currentWorldFrame->PerformDefaultAction(MOUSE_BUTTON_RIGHT);
            }
        }

        this->m_pendingDefaultAction = PENDING_ACTION_NONE;
    }

    return 1;
}

// OFFSET: [0x00449030]
void CGInputControl::Reset() {
    uint32_t time = OsGetAsyncTimeMs();
    this->UnsetControlBit(0xFFFFFFFF, time, 0);
    this->m_time = time;
    this->m_joystickLookX = 0.0f;
    this->m_joystickLookY = 0.0f;
}

// OFFSET: 0x004490B0
void CGInputControl::AscendDescendPlayer(uint32_t time, CGUnit_C* unit) {
    CMovement_C* movement = unit->m_movement;
    uint32_t moveFlags = movement->m_flags;

    if (!(moveFlags & 0x2200000)) {
        this->m_flags &= ~0x100000u;
        return;
    }

    uint32_t flags = this->m_flags;
    int32_t direction = (flags & 0x2000) != 0;

    if (flags & 0x4000) {
        direction--;
    }

    if (movement->m_flags2 & 0x10) {
        if (direction > 0) {
            if (!(flags & 0x100000) || (moveFlags & 0x80)) {
                unit->OnPitchStartLocal(time, 1);
                this->m_flags |= 0x100000;
            }
        } else if (direction < 0) {
            if (!(flags & 0x100000) || (moveFlags & 0x40)) {
                unit->OnPitchStartLocal(time, 0);
                this->m_flags |= 0x100000;
            }
        } else if (flags & 0x100000) {
            unit->OnPitchStopLocal(time);
            this->m_flags &= ~0x100000u;
            CGInputControl::SendUIVehicleAngleUpdate(unit->GetPitch());
        }

        return;
    }

    if (!direction) {
        if (flags & 0x100000) {
            unit->OnAscendDescendStopLocal(time);
            this->m_flags &= ~0x100000u;
        }

        return;
    }

    if (ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__)) {
        CGPlayer_C::ClearAFK(0);
    }

    if (direction > 0) {
        if (!(this->m_flags & 0x100000) || (unit->m_movement->m_flags & 0x800000)) {
            unit->OnAscendDescendStartLocal(time, 1);
            this->m_flags |= 0x100000;
        }
    } else {
        if (!(this->m_flags & 0x100000) || (unit->m_movement->m_flags & 0x400000)) {
            unit->OnAscendDescendStartLocal(time, 0);
            this->m_flags |= 0x100000;
        }
    }
}

// OFFSET: 0x00449240
void CGInputControl::MovePlayer(uint32_t time, CGUnit_C* unit) {
    int32_t direction = 0;

    if ((unit->m_unit->m_unitFieldFlags2 & 0x40) || unit->IsVehicleCurrentlyUnstoppable()) {
        direction = 1;
    } else {
        uint32_t flags = this->m_flags;

        if (flags & 0x1000) {
            direction = 1;
        }

        if (flags & 0x10) {
            direction++;
        }

        if ((flags & 0x1) && (flags & 0x2)) {
            direction++;
        }

        if (flags & 0x20) {
            direction--;
        }

        if (!direction) {
            if (flags & 0x10000) {
                unit->OnMoveStopLocal(time);
                this->m_flags &= ~0x10000u;
            }

            return;
        }
    }

    if (unit->GetClientStandState()) {
        unit->ChangeStandState(0);
        CGCamera* camera = CGWorldFrame::GetActiveCamera();

        if (this->CanSyncFreeLookFacing(unit)) {
            camera->SyncFreeLookFacing();
        }
    }

    if (ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__)) {
        CGPlayer_C::ClearAFK(0);
    }

    if (direction > 0) {
        if (!(this->m_flags & 0x10000) || (unit->m_movement->m_flags & 0x2)) {
            unit->OnMoveStartLocal(time, 1);
            this->m_flags |= 0x10000;
        }
    } else {
        if (!(this->m_flags & 0x10000) || (unit->m_movement->m_flags & 0x1)) {
            unit->OnMoveStartLocal(time, 0);
            this->m_flags |= 0x10000;
        }
    }
}

// OFFSET: 0x00449380
void CGInputControl::StrafePlayer(uint32_t time, CGUnit_C* unit) {
    uint32_t flags = this->m_flags;
    int32_t direction = (flags & 0x40) != 0;

    if ((flags & 0x2000001) && (flags & 0x100)) {
        direction++;
    }

    if (flags & 0x80) {
        direction--;
    }

    if ((flags & 0x2000001) && (flags & 0x200)) {
        direction--;
    }

    if (!direction) {
        if (flags & 0x20000) {
            unit->OnStrafeStopLocal(time);
            this->m_flags &= ~0x20000u;
        }

        return;
    }

    if (unit->GetClientStandState()) {
        unit->ChangeStandState(0);
    }

    if (ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__)) {
        CGPlayer_C::ClearAFK(0);
    }

    if (direction > 0) {
        if (!(this->m_flags & 0x20000)) {
            unit->OnStrafeStartLocal(time, 1);
            this->m_flags |= 0x20000;
        }
    } else {
        if (!(this->m_flags & 0x20000)) {
            unit->OnStrafeStartLocal(time, 0);
            this->m_flags |= 0x20000;
        }
    }
}

// OFFSET: 0x00449470
void CGInputControl::TurnPlayer(uint32_t time, CGUnit_C* unit) {
    uint32_t flags = this->m_flags;
    int32_t direction = (flags & 0x100) != 0;

    if (flags & 0x200) {
        direction--;
    }

    if ((!(flags & 0x2000001) || unit->IsStrafingDisabled()) && direction) {
        if (unit->GetClientStandState()) {
            unit->ChangeStandState(0);
        }

        if (ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__)) {
            CGPlayer_C::ClearAFK(0);
        }

        if (direction > 0) {
            if (!(this->m_flags & 0x40000)) {
                unit->OnTurnStartLocal(time, 1);
                this->m_flags |= 0x40000;
            }
        } else {
            if (!(this->m_flags & 0x40000)) {
                unit->OnTurnStartLocal(time, 0);
                this->m_flags |= 0x40000;
            }
        }
    } else if (this->m_flags & 0x40000) {
        unit->OnTurnStopLocal(time);
        this->m_flags &= ~0x40000u;
    }
}

// OFFSET: 0x00449560
void CGInputControl::PitchPlayer(uint32_t time, CGUnit_C* unit) {
    uint32_t flags = this->m_flags;

    if (flags & 0x2000001) {
        return;
    }

    int32_t direction = (flags & 0x400) != 0;

    if (flags & 0x800) {
        direction--;
    }

    if (direction > 0) {
        if (!(flags & 0x80000)) {
            unit->OnPitchStartLocal(time, 1);
            this->m_flags |= 0x80000;
        }
    } else if (direction < 0) {
        if (!(flags & 0x80000)) {
            unit->OnPitchStartLocal(time, 0);
            this->m_flags |= 0x80000;
        }
    } else if (flags & 0x80000) {
        unit->OnPitchStopLocal(time);
        this->m_flags &= ~0x80000u;
        CGInputControl::SendUIVehicleAngleUpdate(unit->GetPitch());
    }
}

// OFFSET: 0x00449FD0/0x140056DE0
void CGInputControl::UpdatePlayer(uint32_t time, int32_t a2) {
    CGUnit_C* mover = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(CGUnit_C::s_activeMover, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (!mover) {
        return;
    }

    if ((mover->m_obj->m_type & TYPEMASK_PLAYER) && static_cast<CGPlayer_C*>(mover)->IsCommentatorUberOrInArena()) {
        CGCommentator::s_Commentator.UpdateCameraVelocity(this->m_flags, a2);
        return;
    }

    uint32_t lastUpdateTime;

    if (MovementGetLastUpdateTime(&lastUpdateTime) && static_cast<int32_t>(time - lastUpdateTime) < 0) {
        time = lastUpdateTime;
    }

    bool canMove = a2 && CGInputControl::CanMove(mover);
    bool canTurn = a2 && CGInputControl::CanTurn(mover);

    if (canMove) {
        this->MovePlayer(time, mover);
        this->StrafePlayer(time, mover);
        this->AscendDescendPlayer(time, mover);
    } else {
        if (!canTurn) {
            mover->ClearTrackingTarget(false, true);
        }

        if (this->m_flags & 0x10000) {
            if (!mover->m_movement->IsOnSpline()) {
                mover->OnMoveStopLocal(time);
            }

            this->m_flags &= ~0x10000u;
        }

        if (this->m_flags & 0x20000) {
            if (!mover->m_movement->IsOnSpline()) {
                mover->OnStrafeStopLocal(time);
            }

            this->m_flags &= ~0x20000u;
        }

        if (this->m_flags & 0x1000) {
            this->m_flags &= ~0x1000u;
        }

        if (this->m_flags & 0x100000) {
            if (!mover->m_movement->IsOnSpline()) {
                mover->OnAscendDescendStopLocal(time);
            }

            this->m_flags &= ~0x100000u;
        }
    }

    if (canTurn) {
        this->TurnPlayer(time, mover);

        if ((mover->m_movement->m_flags & 0x2200000) || (mover->m_movement->m_flags2 & 0x20)) {
            this->PitchPlayer(time, mover);
        }
    } else {
        if (this->m_flags & 0x40000) {
            if (!mover->m_movement->IsOnSpline()) {
                mover->OnTurnStopLocal(time);
            }

            this->m_flags &= ~0x40000u;
        }

        if (this->m_flags & 0x80000) {
            if (!mover->m_movement->IsOnSpline() && (mover->m_movement->m_flags & 0x2200000)) {
                mover->OnPitchStopLocal(time);
            }

            this->m_flags &= ~0x80000u;
        }
    }
}

// PROVEN: b2d85fc5 IDENTICAL/NONE
// OFFSET: 0x0044A210/0x140057060
void CGInputControl::ToggleControlBit(uint32_t bit, int32_t enable, uint32_t time, int32_t a5) {
    int32_t changed = enable ? this->SetControlBit(bit, time) : this->UnsetControlBit(bit, time, a5);

    if (!changed) {
        return;
    }

    if (enable && ((bit & 0x10F0) || ((bit & 0x3) && (this->m_flags & 0x1) && (this->m_flags & 0x2)))) {
        CheckToCancelCurrentChannelSpell();
    }

    this->UpdatePlayer(time, 1);
}

// PROVEN: c7904511 IDENTICAL/NONE
// OFFSET: 0x0044A330/0x1400571C0
void CGInputControl::StopMovementAndTurning(uint32_t time) {
    uint32_t bits = this->m_flags & 0x1330;
    this->ToggleControlBit(bits, 0, time, 0);
    this->UpdatePlayer(time, 1);
    this->ToggleControlBit(bits, 1, time, 0);
}

// PROVEN: 06a904b6 IDENTICAL/NONE
// OFFSET: 0x004478E0/0x140054100
uint32_t GetJoystick() {
    return s_joystick;
}

// OFFSET: 0x0044A560
void DescendStart() {
    CGInputControl* inputControl = CGInputControl::s_inputControl;

    if (CGGameUI::CanPerformAction(0)) {
        uint32_t time = CSimpleTop::s_eventTime;

        if (inputControl->SetControlBit(0x4000, time)) {
            inputControl->UpdatePlayer(time, 1);
        }
    }

    CheckToCancelCurrentChannelSpell();
}

// OFFSET: 0x0044ADD0
void CGInputControl::OnFocusChange(const CFocusEvent& event) {
    if (!event.m_data.m_focus) {
        this->m_flags &= 0xFFFFF00F;
        this->UpdatePlayer(OsGetAsyncTimeMs(), 1);
    }

    if (event.m_data.m_focus) {
        this->m_mouseModeFlags &= ~0x20u;
    } else {
        this->m_mouseModeFlags |= 0x20u;
    }

    this->UpdateMouseMode(0);
}

// OFFSET: 0x0044AE30
void CGInputControl::SetMouselookOverrideBindings() {
    CGUIBindings* bindings = CGUIBindings::s_bindings;

    for (MOUSELOOKBINDING* entry = this->m_mouseLookBindings.m_fulllist.Head(); entry; entry = this->m_mouseLookBindings.Next(entry)) {
        bindings->SetOverrideBinding(this, 1, entry->m_key.m_str, entry->m_binding, false);
    }
}

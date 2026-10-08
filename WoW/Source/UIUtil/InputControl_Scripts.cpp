#include "InputControl.h"
#include <cstdint>
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"
#include "Engine/Source/Frame/CSimpleTop.h"
#include "Engine/Source/FrameXML/LoadXML.h"
#include "WoW/Source/Object/Movement.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/Object/ObjectClient/Unit_C.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/Ui/GameUI.h"
#include "WoW/Source/Ui/UIBindings.h"
#include "WoW/Source/Ui/WorldFrame.h"
#include "WoW/Source/Object/ObjectClient/Vehicle_C.h"
#include "WoW/Source/DB/Rec/VehicleRec.h"
#include <cmath>
#include "Engine/Source/lua-5.1/src/lua.h"
#include "WoW/Source/Ui/Types.h"
#include "WoW/Source/UIUtil/Types.h"

// OFFSET: 0x0044A3A0/0x140057260
static int32_t Script_JumpOrAscendStart(lua_State* L) {
    CGUnit_C* mover = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(CGUnit_C::s_activeMover, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (!mover) {
        return 0;
    }

    CGInputControl* input = CGInputControl::s_inputControl;

    if (!CGGameUI::CanPerformAction(0)) {
        return 0;
    }

    input->ToggleControlBit(0x2000, 1, CSimpleTop::s_eventTime, 0);

    if (mover->m_movement->m_flags & 0x2200000) {
        return 0;
    }

    CheckToCancelCurrentChannelSpell();

    if (mover->m_unit->m_unitFieldHealth <= 0 || mover->IsOnSpline() || mover->IsAlteredFormTransitionPreventingMovement() || !mover->CanMove() || mover->AnimSuppressesMovement()) {
        return 0;
    }

    uint32_t time = CSimpleTop::s_eventTime;

    if (mover->GetClientStandState()) {
        mover->ChangeStandState(0);
    }

    if (ClntObjMgrGetActivePlayerObj()) {
        CGPlayer_C::ClearAFK(0);
    }

    if (mover->GetCanFly()) {
        mover->OnFlightLocal(time, 1);
    } else {
        mover->TryJumpOrAscend(time);
    }

    return 0;
}

// OFFSET: 0x0044A510/0x140057430
static int32_t Script_AscendStop(lua_State* L) {
    CGInputControl* input = CGInputControl::s_inputControl;

    if (!CGGameUI::CanPerformAction(0)) {
        return 0;
    }

    input->ToggleControlBit(0x2000, 0, CSimpleTop::s_eventTime, 0);
    return 0;
}

// OFFSET: 0x0044A5B0/0x1400574F0
static int32_t Script_DescendStop(lua_State* L) {
    CGInputControl* input = CGInputControl::s_inputControl;

    if (!CGGameUI::CanPerformAction(0)) {
        return 0;
    }

    input->ToggleControlBit(0x4000, 0, CSimpleTop::s_eventTime, 0);
    return 0;
}

// OFFSET: 0x00448E70/0x140055A30
static int32_t Script_ToggleRun(lua_State* L) {
    if (!CGGameUI::CanPerformAction(0)) {
        return 0;
    }

    CGUnit_C* mover = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(CGUnit_C::s_activeMover, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (!mover) {
        return 0;
    }

    uint32_t time = CSimpleTop::s_eventTime;

    if (mover->m_unit->m_unitFieldHealth > 0 && !mover->m_movement->IsOnSpline() && !mover->IsAlteredFormTransitionPreventingMovement() && mover->CanMove()) {
        mover->ToggleRunModeLocal(time);
    }

    return 0;
}

// OFFSET: 0x0044A600/0x140057550
static int32_t Script_ToggleAutoRun(lua_State* L) {
    CGInputControl* input = CGInputControl::s_inputControl;
    int32_t enable = !(input->m_flags & 0x1000);

    if (!CGGameUI::CanPerformAction(0)) {
        return 0;
    }

    input->ToggleControlBit(0x1000, enable, CSimpleTop::s_eventTime, 0);
    return 0;
}

// OFFSET: 0x0044A670/0x1400575F0
static int32_t Script_MoveForwardStart(lua_State* L) {
    CGInputControl* input = CGInputControl::s_inputControl;

    if (!CGGameUI::CanPerformAction(0)) {
        return 0;
    }

    input->ToggleControlBit(0x10, 1, CSimpleTop::s_eventTime, 0);
    return 0;
}

// OFFSET: 0x0044A6C0/0x140057660
static int32_t Script_MoveForwardStop(lua_State* L) {
    CGInputControl* input = CGInputControl::s_inputControl;

    if (!CGGameUI::CanPerformAction(0)) {
        return 0;
    }

    input->ToggleControlBit(0x10, 0, CSimpleTop::s_eventTime, 0);
    return 0;
}

// OFFSET: 0x0044A700/0x1400576C0
static int32_t Script_MoveBackwardStart(lua_State* L) {
    CGInputControl* input = CGInputControl::s_inputControl;

    if (!CGGameUI::CanPerformAction(0)) {
        return 0;
    }

    input->ToggleControlBit(0x20, 1, CSimpleTop::s_eventTime, 0);
    return 0;
}

// OFFSET: 0x0044A750/0x140057730
static int32_t Script_MoveBackwardStop(lua_State* L) {
    CGInputControl* input = CGInputControl::s_inputControl;

    if (!CGGameUI::CanPerformAction(0)) {
        return 0;
    }

    input->ToggleControlBit(0x20, 0, CSimpleTop::s_eventTime, 0);
    return 0;
}

// OFFSET: 0x0044A790/0x140057790
static int32_t Script_TurnLeftStart(lua_State* L) {
    CGInputControl* input = CGInputControl::s_inputControl;

    if (!CGGameUI::CanPerformAction(0)) {
        return 0;
    }

    input->ToggleControlBit(0x100, 1, CSimpleTop::s_eventTime, 0);
    return 0;
}

// OFFSET: 0x0044A7D0/0x1400577F0
static int32_t Script_TurnLeftStop(lua_State* L) {
    CGInputControl* input = CGInputControl::s_inputControl;

    if (!CGGameUI::CanPerformAction(0)) {
        return 0;
    }

    input->ToggleControlBit(0x100, 0, CSimpleTop::s_eventTime, 0);
    return 0;
}

// OFFSET: 0x0044A820/0x140057850
static int32_t Script_TurnRightStart(lua_State* L) {
    CGInputControl* input = CGInputControl::s_inputControl;

    if (!CGGameUI::CanPerformAction(0)) {
        return 0;
    }

    input->ToggleControlBit(0x200, 1, CSimpleTop::s_eventTime, 0);
    return 0;
}

// OFFSET: 0x0044A860/0x1400578B0
static int32_t Script_TurnRightStop(lua_State* L) {
    CGInputControl* input = CGInputControl::s_inputControl;

    if (!CGGameUI::CanPerformAction(0)) {
        return 0;
    }

    input->ToggleControlBit(0x200, 0, CSimpleTop::s_eventTime, 0);
    return 0;
}

// OFFSET: 0x0044A8B0/0x140057910
static int32_t Script_StrafeLeftStart(lua_State* L) {
    CGInputControl* input = CGInputControl::s_inputControl;

    if (!CGGameUI::CanPerformAction(0)) {
        return 0;
    }

    input->ToggleControlBit(0x40, 1, CSimpleTop::s_eventTime, 0);
    return 0;
}

// OFFSET: 0x0044A900/0x140057980
static int32_t Script_StrafeLeftStop(lua_State* L) {
    CGInputControl* input = CGInputControl::s_inputControl;

    if (!CGGameUI::CanPerformAction(0)) {
        return 0;
    }

    input->ToggleControlBit(0x40, 0, CSimpleTop::s_eventTime, 0);
    return 0;
}

// OFFSET: 0x0044A940/0x1400579E0
static int32_t Script_StrafeRightStart(lua_State* L) {
    CGInputControl* input = CGInputControl::s_inputControl;

    if (!CGGameUI::CanPerformAction(0)) {
        return 0;
    }

    input->ToggleControlBit(0x80, 1, CSimpleTop::s_eventTime, 0);
    return 0;
}

// OFFSET: 0x0044A990/0x140057A50
static int32_t Script_StrafeRightStop(lua_State* L) {
    CGInputControl* input = CGInputControl::s_inputControl;

    if (!CGGameUI::CanPerformAction(0)) {
        return 0;
    }

    input->ToggleControlBit(0x80, 0, CSimpleTop::s_eventTime, 0);
    return 0;
}

// OFFSET: 0x0044A9E0/0x140057F70
static int32_t Script_VehicleAimUpStop(lua_State* L) {
    CGInputControl* input = CGInputControl::s_inputControl;

    if (!CGGameUI::CanPerformAction(0)) {
        return 0;
    }

    input->ToggleControlBit(0x400, 0, CSimpleTop::s_eventTime, 0);
    return 0;
}

// OFFSET: 0x0044AD80/0x140057B10
static int32_t Script_VehicleAimDownStop(lua_State* L) {
    CGInputControl* input = CGInputControl::s_inputControl;

    if (!CGGameUI::CanPerformAction(0)) {
        return 0;
    }

    input->ToggleControlBit(0x800, 0, CSimpleTop::s_eventTime, 0);
    return 0;
}

// OFFSET: 0x0044AD40/0x140057F10
static int32_t Script_VehicleAimUpStart(lua_State* L) {
    CGInputControl* input = CGInputControl::s_inputControl;

    if (!CGGameUI::CanPerformAction(0)) {
        return 0;
    }

    input->ToggleControlBit(0x400, 1, CSimpleTop::s_eventTime, 0);
    return 0;
}

// OFFSET: 0x0044AA30/0x140057AB0
static int32_t Script_VehicleAimDownStart(lua_State* L) {
    CGInputControl* input = CGInputControl::s_inputControl;

    if (!CGGameUI::CanPerformAction(0)) {
        return 0;
    }

    input->ToggleControlBit(0x800, 1, CSimpleTop::s_eventTime, 0);
    return 0;
}

// OFFSET: 0x0044AA70/0x140057B70
static int32_t Script_TurnOrActionStart(lua_State* L) {
    CGInputControl* input = CGInputControl::s_inputControl;

    if (CGGameUI::CanPerformAction(0)) {
        uint32_t time = CSimpleTop::s_eventTime;

        if (!(input->m_flags & 0x3)) {
            input->m_pendingDefaultAction = PENDING_ACTION_RIGHT;
            CGWorldFrame::s_currentWorldFrame->SetupDefaultAction();
        }

        if (input->SetControlBit(1, time)) {
            if ((input->m_flags & 0x1) && (input->m_flags & 0x2)) {
                CheckToCancelCurrentChannelSpell();
            }

            input->UpdatePlayer(time, 1);
        }
    }

    return 0;
}

// OFFSET: 0x0044AAE0/0x140057BF0
static int32_t Script_TurnOrActionStop(lua_State* L) {
    CGInputControl* input = CGInputControl::s_inputControl;

    if (CGGameUI::CanPerformAction(0)) {
        uint32_t time = CSimpleTop::s_eventTime;

        if (input->UnsetControlBit(1, time, 0)) {
            input->UpdatePlayer(time, 1);
        }
    }

    return 0;
}

// OFFSET: 0x0044AB20/0x140057C50
static int32_t Script_CameraOrSelectOrMoveStart(lua_State* L) {
    CGInputControl* input = CGInputControl::s_inputControl;

    if (CGGameUI::CanPerformAction(0)) {
        uint32_t time = CSimpleTop::s_eventTime;

        if (!(input->m_flags & 0x3)) {
            input->m_pendingDefaultAction = PENDING_ACTION_LEFT;
            CGWorldFrame::s_currentWorldFrame->SetupDefaultAction();
        }

        if (input->SetControlBit(2, time)) {
            if ((input->m_flags & 0x1) && (input->m_flags & 0x2)) {
                CheckToCancelCurrentChannelSpell();
            }

            input->UpdatePlayer(time, 1);
        }
    }

    return 0;
}

// OFFSET: 0x0044AB90/0x140057CD0
static int32_t Script_CameraOrSelectOrMoveStop(lua_State* L) {
    CGInputControl* input = CGInputControl::s_inputControl;

    if (CGGameUI::CanPerformAction(0)) {
        uint32_t time = CSimpleTop::s_eventTime;

        if (input->UnsetControlBit(2, time, StringToBOOL(L, 1, 0))) {
            input->UpdatePlayer(time, 1);
        }
    }

    return 0;
}

// PROVEN: 9f989108 IDENTICAL/NONE
// OFFSET: 0x0044ACF0/0x140057EA0
static int32_t Script_MouselookStop(lua_State* L) {
    CGInputControl* input = CGInputControl::s_inputControl;
    uint32_t time = CSimpleTop::s_eventTime;
    input->m_pendingDefaultAction = PENDING_ACTION_NONE;

    if (input->UnsetControlBit(1, time, 0)) {
        input->UpdatePlayer(time, 1);
    }

    CGUIBindings::s_bindings->ClearOverrideBindings(input, false);
    return 0;
}

// PROVEN: 8447b888 IDENTICAL/NONE
// OFFSET: 0x0044B000/0x140058290
static int32_t Script_MouselookStart(lua_State* L) {
    CGInputControl* input = CGInputControl::s_inputControl;
    uint32_t time = CSimpleTop::s_eventTime;
    input->m_pendingDefaultAction = PENDING_ACTION_NONE;

    if (input->SetControlBit(1, time)) {
        if ((input->m_flags & 0x1) && (input->m_flags & 0x2)) {
            CheckToCancelCurrentChannelSpell();
        }

        input->UpdatePlayer(time, 1);
    }

    input->SetMouselookOverrideBindings();
    return 0;
}

// OFFSET: 0x004478F0/0x140054110
static int32_t Script_VehicleAimGetNormPower(lua_State* L) {
    lua_pushnumber(L, s_vehicleAimNormPower);
    return 1;
}

// PROVEN: acace478 IDENTICAL/NONE
// OFFSET: 0x004480B0/0x140054A00
static int32_t Script_IsMouselooking(lua_State* L) {
    if (CGInputControl::s_inputControl->m_flags & 0x2000001) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: 0x004480F0/0x140054A40
static int32_t Script_VehicleAimGetAngle(lua_State* L) {
    CGUnit_C* vehicle = GetVehicleUnitPtr();

    if (vehicle) {
        lua_pushnumber(L, vehicle->GetPitch());
    } else {
        lua_pushnumber(L, 0.0);
    }

    return 1;
}

// OFFSET: 0x00449D60/0x140056B20
static int32_t Script_IsUsingVehicleControls(lua_State* L) {
    if (CGInputControl::s_inputControl->IsVehicleSeatFlagSet(0x800000)) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: 0x00449DB0/0x140056B70
static int32_t Script_CanExitVehicle(lua_State* L) {
    if (CGInputControl::s_inputControl->IsVehicleSeatFlagSet(0x2000000)) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: 0x00449E00/0x140056BC0
static int32_t Script_CanSwitchVehicleSeats(lua_State* L) {
    if (CGInputControl::s_inputControl->IsVehicleSeatFlagSet(0x4000000)) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: 0x00448210/0x140054B60
static int32_t Script_VehicleAimSetNormPower(lua_State* L) {
    if (!GetVehicleUnitPtr() || !lua_isnumber(L, 1)) {
        return 0;
    }

    double power = lua_tonumber(L, 1);
    s_vehicleAimNormPower = static_cast<float>(power);

    if (power < 0.0) {
        s_vehicleAimNormPower = 0.0f;
    } else if (power >= 1.0) {
        s_vehicleAimNormPower = 1.0f;
    }

    return 0;
}

// OFFSET: 0x0044AC90/0x140057E20
static int32_t Script_MoveAndSteerStop(lua_State* L) {
    CGInputControl* inputControl = CGInputControl::s_inputControl;

    if (CGGameUI::CanPerformAction(0)) {
        uint32_t time = CSimpleTop::s_eventTime;

        if (inputControl->UnsetControlBit(2, time, 0)) {
            inputControl->UpdatePlayer(time, 1);
        }

        if (inputControl->UnsetControlBit(1, time, 0)) {
            inputControl->UpdatePlayer(time, 1);
        }
    }

    return 0;
}

// OFFSET: 0x00448280/0x140054BE0
static int32_t Script_IsVehicleAimAngleAdjustable(lua_State* L) {
    CGUnit_C* unit = GetVehicleUnitPtr();
    VehicleRec* rec = unit && unit->m_vehicle ? unit->m_vehicle->m_vehicleRec : NULL;

    if (rec && (rec->m_flags & 0x400)) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: 0x004482F0/0x140054C50
static int32_t Script_IsVehicleAimPowerAdjustable(lua_State* L) {
    CGUnit_C* unit = GetVehicleUnitPtr();
    VehicleRec* rec = unit && unit->m_vehicle ? unit->m_vehicle->m_vehicleRec : NULL;

    if (rec && (rec->m_flags & 0x800)) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: 0x0044BA80/0x140059170
static int32_t Script_SetMouselookOverrideBinding(lua_State* L) {
    CGInputControl* inputControl = CGInputControl::s_inputControl;

    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: SetMouselookOverrideBinding(\"key\", \"binding\" or nil)");
        return 0;
    }

    if (CGGameUI::CanPerformAction(13)) {
        inputControl->SetMouselookOverrideBinding(lua_tolstring(L, 1, NULL), lua_tolstring(L, 2, NULL));
    }

    return 0;
}

// OFFSET: 0x00448140/0x140054A90
static int32_t Script_VehicleAimGetNormAngle(lua_State* L) {
    CGUnit_C* unit = GetVehicleUnitPtr();

    if (!unit) {
        lua_pushnumber(L, 0.0);
        return 1;
    }

    VehicleRec* rec = unit->m_vehicle ? unit->m_vehicle->m_vehicleRec : NULL;
    float pitchMin;
    float pitchMax;

    if (rec && (rec->m_flags & 0x40)) {
        pitchMin = rec->m_pitchMin;
        pitchMax = rec->m_pitchMax;
    } else {
        pitchMin = -1.5707964f;
        pitchMax = 1.5707964f;
    }

    float range = pitchMax - pitchMin;
    float angle = fabs(range) <= 0.000099999997f ? 0.0f : (unit->GetPitch() - pitchMin) / range;
    lua_pushnumber(L, angle);
    return 1;
}

// OFFSET: 0x00449C00/0x140056980
static int32_t Script_VehicleAimRequestAngle(lua_State* L) {
    CGInputControl* inputControl = CGInputControl::s_inputControl;
    uint32_t time = CSimpleTop::s_eventTime;
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(CGUnit_C::s_activeMover, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (unit) {
        VehicleRec* rec = unit->m_vehicle ? unit->m_vehicle->m_vehicleRec : NULL;

        if (rec && (rec->m_flags & 0x400) && lua_isnumber(L, 1)) {
            inputControl->SetPlayerPitch(unit, time, static_cast<float>(lua_tonumber(L, 1)));
        }
    }

    return 0;
}

// OFFSET: 0x00449CA0/0x140056A40
static int32_t Script_VehicleAimRequestNormAngle(lua_State* L) {
    CGInputControl* inputControl = CGInputControl::s_inputControl;
    uint32_t time = CSimpleTop::s_eventTime;
    CGUnit_C* unit = GetVehicleUnitPtr();

    if (!unit) {
        return 0;
    }

    VehicleRec* rec = unit->m_vehicle ? unit->m_vehicle->m_vehicleRec : NULL;

    if (!rec || !(rec->m_flags & 0x400)) {
        return 0;
    }

    if (lua_isnumber(L, 1)) {
        double value = lua_tonumber(L, 1);
        float pitchMin;
        float pitchMax;

        if (rec->m_flags & 0x40) {
            pitchMin = rec->m_pitchMin;
            pitchMax = rec->m_pitchMax;
        } else {
            pitchMin = -1.5707964f;
            pitchMax = 1.5707964f;
        }

        inputControl->SetPlayerPitch(unit, time, static_cast<float>(value * (pitchMax - pitchMin) + pitchMin));
    }

    return 0;
}

// PROVEN: 6d45b25d IDENTICAL/NONE
// OFFSET: 0x00448360/0x140054CC0
static int32_t Script_DetectWowMouse(lua_State* L) {
    CGInputControl::s_inputControl->DetectWowMouse();
    return 0;
}

// OFFSET: 0x00449A10/0x140056720
static int32_t Script_VehicleExit(lua_State* L) {
    CGUnit_C* mover = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(CGUnit_C::s_activeMover, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (!mover) {
        return 0;
    }

    if (!CGInputControl::s_inputControl->IsVehicleSeatFlagSet(0x2000000)) {
        CGGameUI::DisplayError(ERR_SPELL_FAILED_S, FrameScript_GetText("SPELL_FAILED_CANT_DO_THAT_RIGHT_NOW", -1, GENDER_NOT_APPLICABLE));
        return 0;
    }

    mover->VehicleExit();
    return 0;
}

// OFFSET: 0x00449AA0/0x1400567E0
static int32_t Script_VehiclePrevSeat(lua_State* L) {
    CGUnit_C* mover = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(CGUnit_C::s_activeMover, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (mover && CGInputControl::s_inputControl->IsVehicleSeatFlagSet(0x4000000)) {
        mover->VehiclePrevSeat();
    }

    return 0;
}

// OFFSET: 0x00449AF0/0x140056840
static int32_t Script_VehicleNextSeat(lua_State* L) {
    CGUnit_C* mover = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(CGUnit_C::s_activeMover, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (mover && CGInputControl::s_inputControl->IsVehicleSeatFlagSet(0x4000000)) {
        mover->VehicleNextSeat();
    }

    return 0;
}

// OFFSET: 0x00449B40/0x1400568A0
static int32_t Script_VehicleAimIncrement(lua_State* L) {
    uint32_t time = CSimpleTop::s_eventTime;
    CGInputControl* input = CGInputControl::s_inputControl;

    if (lua_isnumber(L, 1)) {
        input->VehicleAimChange(time, static_cast<float>(lua_tonumber(L, 1)));
    } else {
        input->VehicleAimChange(time, 0.1f);
    }

    return 0;
}

// OFFSET: 0x00449BA0/0x140056910
static int32_t Script_VehicleAimDecrement(lua_State* L) {
    uint32_t time = CSimpleTop::s_eventTime;
    CGInputControl* input = CGInputControl::s_inputControl;
    double delta = lua_isnumber(L, 1) ? lua_tonumber(L, 1) : 0.1;
    input->VehicleAimChange(time, static_cast<float>(-delta));
    return 0;
}

// PROVEN: 42d39f95 IDENTICAL/NONE
// OFFSET: 0x0044ABE0/0x140057D50
static int32_t Script_MoveAndSteerStart(lua_State* L) {
    CGInputControl* input = CGInputControl::s_inputControl;

    if (!CGGameUI::CanPerformAction(0)) {
        return 0;
    }

    uint32_t time = CSimpleTop::s_eventTime;

    if (!(input->m_flags & 3)) {
        input->m_pendingDefaultAction = PENDING_ACTION_LEFT;
        CGWorldFrame::s_currentWorldFrame->SetupDefaultAction();
    }

    if (input->SetControlBit(2, time)) {
        if ((input->m_flags & 1) && (input->m_flags & 2)) {
            CheckToCancelCurrentChannelSpell();
        }

        input->UpdatePlayer(time, 1);
    }

    if (!(input->m_flags & 3)) {
        input->m_pendingDefaultAction = PENDING_ACTION_RIGHT;
        CGWorldFrame::s_currentWorldFrame->SetupDefaultAction();
    }

    if (input->SetControlBit(1, time)) {
        if ((input->m_flags & 1) && (input->m_flags & 2)) {
            CheckToCancelCurrentChannelSpell();
        }

        input->UpdatePlayer(time, 1);
    }

    return 0;
}

// STATIC: #0x00AD1938
static FrameScript_Method s_ScriptFunctions[] = {
    { "JumpOrAscendStart", &Script_JumpOrAscendStart },
    { "AscendStop", &Script_AscendStop },
    { "DescendStop", &Script_DescendStop },
    { "ToggleRun", &Script_ToggleRun },
    { "ToggleAutoRun", &Script_ToggleAutoRun },
    { "MoveForwardStart", &Script_MoveForwardStart },
    { "MoveForwardStop", &Script_MoveForwardStop },
    { "MoveBackwardStart", &Script_MoveBackwardStart },
    { "MoveBackwardStop", &Script_MoveBackwardStop },
    { "TurnLeftStart", &Script_TurnLeftStart },
    { "TurnLeftStop", &Script_TurnLeftStop },
    { "TurnRightStart", &Script_TurnRightStart },
    { "TurnRightStop", &Script_TurnRightStop },
    { "StrafeLeftStart", &Script_StrafeLeftStart },
    { "StrafeLeftStop", &Script_StrafeLeftStop },
    { "StrafeRightStart", &Script_StrafeRightStart },
    { "StrafeRightStop", &Script_StrafeRightStop },
    { "PitchUpStart", &Script_VehicleAimUpStart },
    { "PitchUpStop", &Script_VehicleAimUpStop },
    { "PitchDownStart", &Script_VehicleAimDownStart },
    { "PitchDownStop", &Script_VehicleAimDownStop },
    { "TurnOrActionStart", &Script_TurnOrActionStart },
    { "TurnOrActionStop", &Script_TurnOrActionStop },
    { "CameraOrSelectOrMoveStart", &Script_CameraOrSelectOrMoveStart },
    { "CameraOrSelectOrMoveStop", &Script_CameraOrSelectOrMoveStop },
    { "MoveAndSteerStart", &Script_MoveAndSteerStart },
    { "MoveAndSteerStop", &Script_MoveAndSteerStop },
    { "SetMouselookOverrideBinding", &Script_SetMouselookOverrideBinding },
    { "MouselookStart", &Script_MouselookStart },
    { "MouselookStop", &Script_MouselookStop },
    { "IsMouselooking", &Script_IsMouselooking },
    { "VehicleExit", &Script_VehicleExit },
    { "VehiclePrevSeat", &Script_VehiclePrevSeat },
    { "VehicleNextSeat", &Script_VehicleNextSeat },
    { "VehicleAimUpStart", &Script_VehicleAimUpStart },
    { "VehicleAimUpStop", &Script_VehicleAimUpStop },
    { "VehicleAimDownStart", &Script_VehicleAimDownStart },
    { "VehicleAimDownStop", &Script_VehicleAimDownStop },
    { "VehicleAimIncrement", &Script_VehicleAimIncrement },
    { "VehicleAimDecrement", &Script_VehicleAimDecrement },
    { "VehicleAimRequestAngle", &Script_VehicleAimRequestAngle },
    { "VehicleAimGetAngle", &Script_VehicleAimGetAngle },
    { "VehicleAimRequestNormAngle", &Script_VehicleAimRequestNormAngle },
    { "VehicleAimGetNormAngle", &Script_VehicleAimGetNormAngle },
    { "VehicleAimSetNormPower", &Script_VehicleAimSetNormPower },
    { "VehicleAimGetNormPower", &Script_VehicleAimGetNormPower },
    { "IsUsingVehicleControls", &Script_IsUsingVehicleControls },
    { "CanExitVehicle", &Script_CanExitVehicle },
    { "CanSwitchVehicleSeats", &Script_CanSwitchVehicleSeats },
    { "IsVehicleAimAngleAdjustable", &Script_IsVehicleAimAngleAdjustable },
    { "IsVehicleAimPowerAdjustable", &Script_IsVehicleAimPowerAdjustable },
    { "DetectWowMouse", &Script_DetectWowMouse },
};

// PROVEN: d152bd3f IDENTICAL/NONE
// OFFSET: 0x00447920/0x140054130
void InputControlRegisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_RegisterFunction(s_ScriptFunctions[i].m_name, s_ScriptFunctions[i].m_method);
    }
}

// PROVEN: ccaeca04 IDENTICAL/NONE
// OFFSET: 0x00447950/0x140054180
void InputControlUnregisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_UnregisterFunction(s_ScriptFunctions[i].m_name);
    }
}

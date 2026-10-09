#include "Engine/Source/ConsoleBase/ConsoleVar.h"
#include "Engine/Source/ConsoleBase/IConsole.h"
#include "Engine/Source/Storm/H/storm.h"
#include "WoW/Source/UIUtil/Camera.h"

// STATIC: 0x00D38770
CameraCVarRef* CameraCVarRef::s_head;

// STATIC: 0x00D3876C
uint32_t CameraCVarRef::s_count;

// STATIC: 0x00D39180
CameraCVarRef s_cvCameraSmoothTimeMax;

// STATIC: 0x00D39178
CameraCVarRef s_cvCameraSmoothTimeMin;

// STATIC: 0x00D39170
CameraCVarRef s_cvCameraYawSmoothMax;

// STATIC: 0x00D39168
CameraCVarRef s_cvCameraYawSmoothMin;

// STATIC: 0x00D39160
CameraCVarRef s_cvCameraPitchSmoothMax;

// STATIC: 0x00D39158
CameraCVarRef s_cvCameraPitchSmoothMin;

// STATIC: 0x00D39150
CameraCVarRef s_cvCameraDistanceMaxFactor;

// STATIC: 0x00D39148
CameraCVarRef s_cvCameraDistanceMax;

// STATIC: 0x00D39140
CameraCVarRef s_cvCameraSubmergeFinalPitch;

// STATIC: 0x00D39138
CameraCVarRef s_cvCameraSurfaceFinalPitch;

// STATIC: 0x00D39130
CameraCVarRef s_cvCameraSubmergePitch;

// STATIC: 0x00D39128
CameraCVarRef s_cvCameraSurfacePitch;

// STATIC: 0x00D39120
CameraCVarRef s_cvCameraDive;

// STATIC: 0x00D39118
CameraCVarRef s_cvCameraPivotDYMin;

// STATIC: 0x00D39110
CameraCVarRef s_cvCameraPivotDXMax;

// STATIC: 0x00D39108
CameraCVarRef s_cvCameraPivot;

// STATIC: 0x00D39100
CameraCVarRef s_cvCameraHeightIgnoreStandState;

// STATIC: 0x00D390F8
CameraCVarRef s_cameraWaterCollision;

// STATIC: 0x00D390F0
CameraCVarRef s_cvCameraTerrainTiltTimeMax;

// STATIC: 0x00D390E8
CameraCVarRef s_cvCameraTerrainTiltTimeMin;

// STATIC: 0x00D38C38
CameraCVarRef s_cvCameraTerrainTiltState[5][10][3];

// STATIC: 0x00D38C30
CameraCVarRef s_cvCameraTerrainTilt;

// STATIC: 0x00D38908
CameraCVarRef s_cvCameraSmoothStyle;

// STATIC: 0x00D38B40
CameraCVarRef s_cvCameraSmoothViewData[5][3][2];

// STATIC: 0x00D38910
CameraCVarRef s_cvCameraSmoothState[5][7][2];

// STATIC: 0x00D38900
CameraCVarRef s_cvCameraSmoothYaw;

// STATIC: 0x00D388F8
CameraCVarRef s_cvCameraSmoothTrackingStyle;

// STATIC: 0x00D388F0
CameraCVarRef s_cvCameraSmoothPitch;

// STATIC: 0x00D388E8
CameraCVarRef s_cameraSmooth;

// STATIC: 0x00D388E0
CameraCVarRef s_cvCameraCustomViewSmoothing;

// STATIC: 0x00D38818
CameraCVarRef s_cvCameraView;

// STATIC: 0x00D38820
CameraCVarRef s_cvCameraViewData[8][3];

// STATIC: 0x00D38810
CameraCVarRef s_cvCameraViewBlendStyle;

// STATIC: 0x00D38808
CameraCVarRef s_cvCameraFlyingMountHeightSmoothSpeed;

// STATIC: 0x00D38800
CameraCVarRef s_cvCameraYawSmoothSpeed;

// STATIC: 0x00D387F8
CameraCVarRef s_cvCameraTargetSmoothSpeed;

// STATIC: 0x00D387F0
CameraCVarRef s_cvCameraPitchSmoothSpeed;

// STATIC: 0x00D387E8
CameraCVarRef s_cvCameraHeightSmoothSpeed;

// STATIC: 0x00D387E0
CameraCVarRef s_cvCameraGroundSmoothSpeed;

// STATIC: 0x00D387D8
CameraCVarRef s_cvCameraFoVSmoothSpeed;

// STATIC: 0x00D387D0
CameraCVarRef s_cvCameraDistanceSmoothSpeed;

// STATIC: 0x00D387C8
CameraCVarRef s_cvCameraBobbingSmoothSpeed;

// STATIC: 0x00D387C0
CameraCVarRef s_cvCameraYawMoveSpeed;

// STATIC: 0x00D387B0
CameraCVarRef s_cvCameraPitchMoveSpeed;

// STATIC: 0x00D387A8
CameraCVarRef s_cvCameraDistanceMoveSpeed;

// STATIC: 0x00D38760
float s_cameraBobFrequency;

// STATIC: 0x00D38764
float s_cameraBobVerticalAmplitude;

// STATIC: 0x00D38768
float s_cameraBobHorizontalAmplitude;

// STATIC: 0x00D387A0
CameraCVarRef s_cvCameraBobbing;

// STATIC: 0x00D38798
CameraCVarRef s_cvMouseInvertPitch;

// STATIC: 0x00D38790
CameraCVarRef s_cvMouseInvertYaw;

// STATIC: 0x00D38788
CameraCVarRef s_cvCameraSavedPitch;

// STATIC: 0x00D38780
CameraCVarRef s_cvCameraSavedVehicleDistance;

// STATIC: 0x00D38778
CameraCVarRef s_cvCameraSavedDistance;

// STATIC: 0x00C98C60
static const char* s_cameraViewNames[8] = { "", "A", "B", "C", "D", "E", "Com", "Barber Shop" };

// STATIC: 0x00C98C80
static const char* s_cameraViewDataNames[3] = { "Distance", "Pitch", "Yaw" };

// STATIC: 0x00C98C98
static const char* s_defaultCameraViewBlendStyle = "1";

// STATIC: 0x00C98C9C
static const char* s_defaultCameraView = "2";

// STATIC: 0x00C98D08
static const char* s_cameraSmoothStyleNames[5] = { "Never", "Smart", "Always", "Spline", "Smarter" };

// STATIC: 0x00C98D00
static const char* s_cameraSmoothParamNames[2] = { "Delay", "Factor" };

// STATIC: 0x00C98D1C
static const char* s_cameraSmoothStateNames[7] = { "Idle", "Stop", "Track", "Move", "Strafe", "Turn", "Fear" };

// STATIC: 0x00C98D38
static const char* s_defaultCameraSmoothStyle = "4";

// STATIC: 0x00C98D48
static const char* s_cameraSmoothStateDefaults[5][7][2] = {
    {
        { "0.0", "0.0" },
        { "0.0", "0.0" },
        { "0.0", "0.0" },
        { "0.0", "0.0" },
        { "0.0", "0.0" },
        { "0.0", "0.0" },
        { "0.0", "0.0" },
    },
    {
        { "0.0", "0.0" },
        { "0.0", "0.0" },
        { "0.4", "10.0" },
        { "0.0", "1.0" },
        { "0.0", "1.0" },
        { "0.0", "1.0" },
        { "0.4", "10.0" },
    },
    {
        { "0.0", "1.0" },
        { "0.0", "1.0" },
        { "0.0", "1.0" },
        { "0.0", "1.0" },
        { "0.0", "1.0" },
        { "0.0", "1.0" },
        { "0.0", "1.0" },
    },
    {
        { "0.0", "4.0" },
        { "0.0", "4.0" },
        { "0.0", "4.0" },
        { "0.0", "1.0" },
        { "0.0", "1.0" },
        { "0.0", "1.0" },
        { "0.0", "4.0" },
    },
    {
        { "0.0", "0.0" },
        { "0.0", "0.0" },
        { "0.4", "10.0" },
        { "0.0", "1.0" },
        { "0.0", "1.0" },
        { "0.0", "1.0" },
        { "0.4", "10.0" },
    },
};

// STATIC: 0x00C98E60
static const char* s_cameraSmoothViewDataDefaults[5][3][2] = {
    {
        { "0.0", "0.0" },
        { "0.0", "0.0" },
        { "0.0", "0.0" },
    },
    {
        { "0.0", "0.0" },
        { "0.0", "0.0" },
        { "0.0", "1.0" },
    },
    {
        { "0.0", "0.0" },
        { "0.0", "1.0" },
        { "0.0", "1.0" },
    },
    {
        { "0.0", "0.0" },
        { "0.0", "1.0" },
        { "0.0", "1.0" },
    },
    {
        { "0.0", "0.0" },
        { "0.0", "1.0" },
        { "0.0", "1.0" },
    },
};

// STATIC: 0x00C98ED8
static const char* s_cameraTerrainTiltStateNames[10] = { "Fall", "Fear", "Idle", "Jump", "Move", "Strafe", "Swim", "Taxi", "Track", "Turn" };

// STATIC: 0x00C98D3C
static const char* s_cameraTerrainTiltParamNames[3] = { "Absorb", "Delay", "Factor" };

// STATIC: 0x00C98F00
static const char* s_cameraTerrainTiltStateDefaults[5][10][3] = {
    {
        { "0.0", "0.0", "-1.0" },
        { "0.0", "0.0", "-1.0" },
        { "0.0", "0.0", "-1.0" },
        { "0.0", "0.0", "-1.0" },
        { "0.0", "0.0", "-1.0" },
        { "0.0", "0.0", "-1.0" },
        { "0.0", "0.0", "-1.0" },
        { "0.0", "0.0", "-1.0" },
        { "0.0", "0.0", "-1.0" },
        { "0.0", "0.0", "-1.0" },
    },
    {
        { "1.0", "0.0", "0.75" },
        { "1.0", "0.0", "1.0" },
        { "0.0", "0.0", "-1.0" },
        { "0.0", "0.0", "-1.0" },
        { "1.0", "0.0", "1.0" },
        { "1.0", "0.0", "1.0" },
        { "0.0", "0.0", "1.0" },
        { "0.0", "0.0", "1.0" },
        { "1.0", "0.0", "1.0" },
        { "1.0", "0.0", "1.0" },
    },
    {
        { "1.0", "0.0", "0.75" },
        { "1.0", "0.0", "1.0" },
        { "1.0", "0.0", "1.0" },
        { "0.0", "0.0", "-1.0" },
        { "1.0", "0.0", "1.0" },
        { "1.0", "0.0", "1.0" },
        { "0.0", "0.0", "1.0" },
        { "0.0", "0.0", "1.0" },
        { "1.0", "0.0", "1.0" },
        { "1.0", "0.0", "1.0" },
    },
    {
        { "1.0", "0.0", "0.75" },
        { "1.0", "0.0", "1.0" },
        { "0.0", "0.0", "-1.0" },
        { "0.0", "0.0", "-1.0" },
        { "1.0", "0.0", "1.0" },
        { "1.0", "0.0", "1.0" },
        { "0.0", "0.0", "1.0" },
        { "0.0", "0.0", "1.0" },
        { "1.0", "0.0", "1.0" },
        { "1.0", "0.0", "1.0" },
    },
    {
        { "1.0", "0.0", "0.75" },
        { "1.0", "0.0", "1.0" },
        { "0.0", "0.0", "-1.0" },
        { "0.0", "0.0", "-1.0" },
        { "1.0", "0.0", "1.0" },
        { "1.0", "0.0", "1.0" },
        { "0.0", "0.0", "1.0" },
        { "0.0", "0.0", "1.0" },
        { "1.0", "0.0", "1.0" },
        { "1.0", "0.0", "1.0" },
    },
};

// PROVEN: 669754d9 IDENTICAL/NONE
// OFFSET: 0x0043A5E0
static bool ValidateCameraView(CVar* var, const char* oldValue, const char* value, void* arg) {
    float view = SStrToFloat(value);

    if (view >= 0.0f && view <= 7.0f) {
        return true;
    }

    ConsoleWriteA("Value out of range (%f - %f)\n", DEFAULT_COLOR, 0.0, 7.0);
    return false;
}

// PROVEN: 45d370ca IDENTICAL/NONE
// OFFSET: 0x0043A640
static bool ValidateCameraDistance(CVar* var, const char* oldValue, const char* value, void* arg) {
    float distance = SStrToFloat(value);

    if (distance >= 0.0f && distance <= 50.0f) {
        return true;
    }

    ConsoleWriteA("Value out of range (%f - %f)\n", DEFAULT_COLOR, 0.0, 50.0);
    return false;
}

// PROVEN: d4087f33 IDENTICAL/NONE
// OFFSET: 0x0043A6A0
static bool ValidateCameraPitch(CVar* var, const char* oldValue, const char* value, void* arg) {
    float minPitch = MIN_PITCH_ANGLE * 57.29578f;
    float maxPitch = MAX_PITCH_ANGLE * 57.29578f;
    float pitch = SStrToFloat(value);

    if (pitch >= minPitch && pitch <= maxPitch) {
        return true;
    }

    ConsoleWriteA("Value out of range (%f - %f)\n", DEFAULT_COLOR, minPitch, maxPitch);
    return false;
}

// PROVEN: e44386eb IDENTICAL/NONE
// OFFSET: 0x0043A720
static bool ValidateCameraTime(CVar* var, const char* oldValue, const char* value, void* arg) {
    float time = SStrToFloat(value);

    if (time >= 0.001f && time <= 300.0f) {
        return true;
    }

    ConsoleWriteA("Value out of range (%f - %f)\n", DEFAULT_COLOR, 0.001f, 300.0f);
    return false;
}

// OFFSET: 0x0043A780
static bool ValidateCameraYaw(CVar* var, const char* oldValue, const char* value, void* arg) {
    float yaw = SStrToFloat(value);

    if (yaw >= 0.0f && yaw <= 360.0f) {
        return true;
    }

    ConsoleWriteA("Value out of range (%f - %f)\n", DEFAULT_COLOR, 0.0, 360.0);
    return false;
}

// OFFSET: 0x0043A7E0
static bool ValidateCameraAngularSpeed(CVar* var, const char* oldValue, const char* value, void* arg) {
    float speed = SStrToFloat(value);

    if (speed >= 0.1f && speed <= 360.0f) {
        return true;
    }

    ConsoleWriteA("Value out of range (%f - %f)\n", DEFAULT_COLOR, 0.1f, 360.0f);
    return false;
}

// PROVEN: 09493d2b IDENTICAL/NONE
// OFFSET: 0x0043A840/0x1400456E0
static bool ValidateCameraLinearSpeed(CVar* var, const char* oldValue, const char* value, void* arg) {
    float speed = SStrToFloat(value);

    if (speed >= 0.0027777778f && speed <= 50.0f) {
        return true;
    }

    ConsoleWriteA("Value out of range (%f - %f)\n", DEFAULT_COLOR, 0.0027777778f, 50.0f);
    return false;
}

// PROVEN: 30fce973 IDENTICAL/NONE
// OFFSET: 0x0043A8A0
static bool ValidateCameraSmoothStyle(CVar* var, const char* oldValue, const char* value, void* arg) {
    float style = SStrToFloat(value);

    if (style >= 0.0f && style <= 5.0f) {
        return true;
    }

    ConsoleWriteA("Value out of range (%f - %f)\n", DEFAULT_COLOR, 0.0, 5.0);
    return false;
}

// STATIC: 0x00C99160
static bool (*s_cameraViewDataValidators[3])(CVar*, const char*, const char*, void*) = { ValidateCameraDistance, ValidateCameraPitch, ValidateCameraYaw };

// OFFSET: 0x0043C0B0/0x140046AE0
void CameraRegisterCVars() {
    s_cvCameraSavedDistance = CVar::Register("cameraSavedDistance", NULL, 0x20, s_cameraViewDefaults[SStrToInt(s_defaultCameraView)].m_values[0], NULL, CATEGORY_DEFAULT, false, NULL, false);
    s_cvCameraSavedVehicleDistance = CVar::Register("cameraSavedVehicleDistance", NULL, 0x20, "-1.0", NULL, CATEGORY_DEFAULT, false, NULL, false);
    s_cvCameraSavedPitch = CVar::Register("cameraSavedPitch", NULL, 0x20, s_cameraViewDefaults[SStrToInt(s_defaultCameraView)].m_values[1], NULL, CATEGORY_DEFAULT, false, NULL, false);
    s_cvMouseInvertYaw = CVar::Register("mouseInvertYaw", NULL, 0x10, "0", NULL, CATEGORY_DEFAULT, false, NULL, false);
    s_cvMouseInvertPitch = CVar::Register("mouseInvertPitch", NULL, 0x10, "0", NULL, CATEGORY_DEFAULT, false, NULL, false);
    s_cvCameraBobbing = CVar::Register("cameraBobbing", NULL, 0x0, "0", NULL, CATEGORY_DEFAULT, false, NULL, false);
    s_cameraBobHorizontalAmplitude = 2.0f;
    s_cameraBobVerticalAmplitude = 2.0f;
    s_cameraBobFrequency = 0.8f;
    s_cvCameraDistanceMoveSpeed = CVar::Register("cameraDistanceMoveSpeed", NULL, 0x10, "8.33", ValidateCameraLinearSpeed, CATEGORY_DEFAULT, false, NULL, false);
    s_cvCameraPitchMoveSpeed = CVar::Register("cameraPitchMoveSpeed", NULL, 0x10, "90", ValidateCameraAngularSpeed, CATEGORY_DEFAULT, false, NULL, false);
    s_cvCameraYawMoveSpeed = CVar::Register("cameraYawMoveSpeed", NULL, 0x10, "180", ValidateCameraAngularSpeed, CATEGORY_DEFAULT, false, NULL, false);
    s_cvCameraBobbingSmoothSpeed = CVar::Register("cameraBobbingSmoothSpeed", NULL, 0x10, "0.8", ValidateCameraLinearSpeed, CATEGORY_DEFAULT, false, NULL, false);
    s_cvCameraFoVSmoothSpeed = CVar::Register("cameraFoVSmoothSpeed", NULL, 0x10, "0.5", ValidateCameraAngularSpeed, CATEGORY_DEFAULT, false, NULL, false);
    s_cvCameraDistanceSmoothSpeed = CVar::Register("cameraDistanceSmoothSpeed", NULL, 0x10, "8.33", ValidateCameraLinearSpeed, CATEGORY_DEFAULT, false, NULL, false);
    s_cvCameraGroundSmoothSpeed = CVar::Register("cameraGroundSmoothSpeed", NULL, 0x10, "7.5", ValidateCameraAngularSpeed, CATEGORY_DEFAULT, false, NULL, false);
    s_cvCameraHeightSmoothSpeed = CVar::Register("cameraHeightSmoothSpeed", NULL, 0x10, "1.2", ValidateCameraLinearSpeed, CATEGORY_DEFAULT, false, NULL, false);
    s_cvCameraPitchSmoothSpeed = CVar::Register("cameraPitchSmoothSpeed", NULL, 0x10, "45", ValidateCameraAngularSpeed, CATEGORY_DEFAULT, false, NULL, false);
    s_cvCameraTargetSmoothSpeed = CVar::Register("cameraTargetSmoothSpeed", NULL, 0x10, "90", ValidateCameraAngularSpeed, CATEGORY_DEFAULT, false, NULL, false);
    s_cvCameraYawSmoothSpeed = CVar::Register("cameraYawSmoothSpeed", NULL, 0x10, "180", ValidateCameraAngularSpeed, CATEGORY_DEFAULT, false, NULL, false);
    s_cvCameraFlyingMountHeightSmoothSpeed = CVar::Register("cameraFlyingMountHeightSmoothSpeed", NULL, 0x10, "2.0", ValidateCameraLinearSpeed, CATEGORY_DEFAULT, false, NULL, false);
    s_cvCameraViewBlendStyle = CVar::Register("cameraViewBlendStyle", NULL, 0x10, s_defaultCameraViewBlendStyle, NULL, CATEGORY_DEFAULT, false, NULL, false);
    s_cvCameraView = CVar::Register("cameraView", NULL, 0x10, s_defaultCameraView, ValidateCameraView, CATEGORY_DEFAULT, false, NULL, false);

    char name[64];

    for (uint32_t view = 0; view < 8; view++) {
        for (uint32_t i = 0; i < 3; i++) {
            name[0] = '\0';
            SStrPack(name, "camera", sizeof(name));
            SStrPack(name, s_cameraViewDataNames[i], sizeof(name));
            SStrPack(name, s_cameraViewNames[view], sizeof(name));
            s_cvCameraViewData[view][i] = CVar::Register(name, NULL, 0x50, s_cameraViewDefaults[view].m_values[i], s_cameraViewDataValidators[i], CATEGORY_DEFAULT, false, NULL, false);
        }
    }

    s_cameraSmooth = CVar::Register("camerasmooth", NULL, 0x10, "1", NULL, CATEGORY_DEFAULT, false, NULL, false);
    s_cvCameraSmoothPitch = CVar::Register("cameraSmoothPitch", NULL, 0x10, "1", NULL, CATEGORY_DEFAULT, false, NULL, false);
    s_cvCameraSmoothYaw = CVar::Register("cameraSmoothYaw", NULL, 0x10, "1", NULL, CATEGORY_DEFAULT, false, NULL, false);
    s_cvCameraSmoothStyle = CVar::Register("cameraSmoothStyle", NULL, 0x10, s_defaultCameraSmoothStyle, ValidateCameraSmoothStyle, CATEGORY_DEFAULT, false, NULL, false);
    s_cvCameraSmoothTrackingStyle = CVar::Register("cameraSmoothTrackingStyle", NULL, 0x10, s_defaultCameraSmoothStyle, ValidateCameraSmoothStyle, CATEGORY_DEFAULT, false, NULL, false);
    s_cvCameraCustomViewSmoothing = CVar::Register("cameraCustomViewSmoothing", NULL, 0x10, "0", NULL, CATEGORY_DEFAULT, false, NULL, false);

    for (uint32_t style = 0; style < 5; style++) {
        for (uint32_t state = 0; state < 7; state++) {
            for (uint32_t i = 0; i < 2; i++) {
                name[0] = '\0';
                SStrPack(name, "cameraSmooth", sizeof(name));
                SStrPack(name, s_cameraSmoothStyleNames[style], sizeof(name));
                SStrPack(name, s_cameraSmoothStateNames[state], sizeof(name));
                SStrPack(name, s_cameraSmoothParamNames[i], sizeof(name));
                s_cvCameraSmoothState[style][state][i] = CVar::Register(name, NULL, 0x10, s_cameraSmoothStateDefaults[style][state][i], NULL, CATEGORY_DEFAULT, false, NULL, false);
            }
        }

        for (uint32_t data = 0; data < 3; data++) {
            for (uint32_t i = 0; i < 2; i++) {
                name[0] = '\0';
                SStrPack(name, "cameraSmoothViewData", sizeof(name));
                SStrPack(name, s_cameraSmoothStyleNames[style], sizeof(name));
                SStrPack(name, s_cameraViewDataNames[data], sizeof(name));
                SStrPack(name, s_cameraSmoothParamNames[i], sizeof(name));
                s_cvCameraSmoothViewData[style][data][i] = CVar::Register(name, NULL, 0x10, s_cameraSmoothViewDataDefaults[style][data][i], NULL, CATEGORY_DEFAULT, false, NULL, false);
            }
        }

        for (uint32_t state = 0; state < 10; state++) {
            for (uint32_t i = 0; i < 3; i++) {
                name[0] = '\0';
                SStrPack(name, "cameraTerrainTilt", sizeof(name));
                SStrPack(name, s_cameraSmoothStyleNames[style], sizeof(name));
                SStrPack(name, s_cameraTerrainTiltStateNames[state], sizeof(name));
                SStrPack(name, s_cameraTerrainTiltParamNames[i], sizeof(name));
                s_cvCameraTerrainTiltState[style][state][i] = CVar::Register(name, NULL, 0x10, s_cameraTerrainTiltStateDefaults[style][state][i], NULL, CATEGORY_DEFAULT, false, NULL, false);
            }
        }
    }

    s_cvCameraTerrainTilt = CVar::Register("cameraTerrainTilt", NULL, 0x10, "0", NULL, CATEGORY_DEFAULT, false, NULL, false);
    s_cvCameraTerrainTiltTimeMin = CVar::Register("cameraTerrainTiltTimeMin", NULL, 0x10, "3.0", ValidateCameraTime, CATEGORY_DEFAULT, false, NULL, false);
    s_cvCameraTerrainTiltTimeMax = CVar::Register("cameraTerrainTiltTimeMax", NULL, 0x10, "10.0", ValidateCameraTime, CATEGORY_DEFAULT, false, NULL, false);
    s_cameraWaterCollision = CVar::Register("cameraWaterCollision", NULL, 0x10, "1", NULL, CATEGORY_DEFAULT, false, NULL, false);
    s_cvCameraHeightIgnoreStandState = CVar::Register("cameraHeightIgnoreStandState", NULL, 0x10, "0", NULL, CATEGORY_DEFAULT, false, NULL, false);
    s_cvCameraPivot = CVar::Register("cameraPivot", NULL, 0x10, "1", NULL, CATEGORY_DEFAULT, false, NULL, false);
    s_cvCameraPivotDXMax = CVar::Register("cameraPivotDXMax", NULL, 0x10, "0.05", NULL, CATEGORY_DEFAULT, false, NULL, false);
    s_cvCameraPivotDYMin = CVar::Register("cameraPivotDYMin", NULL, 0x10, "0.00", NULL, CATEGORY_DEFAULT, false, NULL, false);
    s_cvCameraDive = CVar::Register("cameraDive", NULL, 0x10, "1", NULL, CATEGORY_DEFAULT, false, NULL, false);
    s_cvCameraSurfacePitch = CVar::Register("cameraSurfacePitch", NULL, 0x10, "0.0", ValidateCameraPitch, CATEGORY_DEFAULT, false, NULL, false);
    s_cvCameraSubmergePitch = CVar::Register("cameraSubmergePitch", NULL, 0x10, "18.0", ValidateCameraPitch, CATEGORY_DEFAULT, false, NULL, false);
    s_cvCameraSurfaceFinalPitch = CVar::Register("cameraSurfaceFinalPitch", NULL, 0x10, "5.0", ValidateCameraPitch, CATEGORY_DEFAULT, false, NULL, false);
    s_cvCameraSubmergeFinalPitch = CVar::Register("cameraSubmergeFinalPitch", NULL, 0x10, "5.0", ValidateCameraPitch, CATEGORY_DEFAULT, false, NULL, false);
    s_cvCameraDistanceMax = CVar::Register("cameraDistanceMax", NULL, 0x10, "15.0", ValidateCameraDistance, CATEGORY_DEFAULT, false, NULL, false);
    s_cvCameraDistanceMaxFactor = CVar::Register("cameraDistanceMaxFactor", NULL, 0x10, "1.0", NULL, CATEGORY_DEFAULT, false, NULL, false);
    s_cvCameraPitchSmoothMin = CVar::Register("cameraPitchSmoothMin", NULL, 0x10, "0.0", ValidateCameraPitch, CATEGORY_DEFAULT, false, NULL, false);
    s_cvCameraPitchSmoothMax = CVar::Register("cameraPitchSmoothMax", NULL, 0x10, "30.0", ValidateCameraPitch, CATEGORY_DEFAULT, false, NULL, false);
    s_cvCameraYawSmoothMin = CVar::Register("cameraYawSmoothMin", NULL, 0x10, "0.0", ValidateCameraYaw, CATEGORY_DEFAULT, false, NULL, false);
    s_cvCameraYawSmoothMax = CVar::Register("cameraYawSmoothMax", NULL, 0x10, "0.0", ValidateCameraYaw, CATEGORY_DEFAULT, false, NULL, false);
    s_cvCameraSmoothTimeMin = CVar::Register("cameraSmoothTimeMin", NULL, 0x10, "0.1", ValidateCameraTime, CATEGORY_DEFAULT, false, NULL, false);
    s_cvCameraSmoothTimeMax = CVar::Register("cameraSmoothTimeMax", NULL, 0x10, "2.0", ValidateCameraTime, CATEGORY_DEFAULT, false, NULL, false);
}

#include "MovementShared.h"
#include <cmath>
#include <cstdio>
#include <malloc.h>
#include "Engine/Source/Os/TimeManager.h"
#include "Engine/Source/Tempest/C33Matrix.h"
#include "Engine/Source/Tempest/C3Spline.h"
#include "Engine/Source/Tempest/C44Matrix.h"
#include "Engine/Source/Tempest/CMath.h"
#include "WoW/Source/Net/Types.h"
#include "WoW/Source/Object/CMovementGlobals.h"
#include "WoW/Source/Object/CMovementStatus.h"
#include "WoW/Source/Object/CMoveSpline.h"
#include "WoW/Source/Object/ObjectClient/Movement_C.h"
#include "WoW/Source/Object/Types.h"

// STATIC: 0x00D18C40
float CMovementShared::s_terminalVelocity = 60.148003f;

// STATIC: 0x00D18C44
float CMovementShared::s_safeFallTerminalVelocity = 7.0f;

// STATIC: #0x00B2D9F0
float CMovementShared::s_twoPi = 6.2831855f;

// OFFSET: 0x00572130
int32_t CMovementShared::IsGravityDisabled() const {
    CMoveSpline* spline = this->m_spline;

    if (spline) {
        if (!(spline->m_flags & SPLINEFLAG_NO_SPLINE) && (spline->m_flags & SPLINEFLAG_FALLING)) {
            return 0;
        }

        if (!(spline->m_flags & SPLINEFLAG_NO_SPLINE) && (spline->m_flags & SPLINEFLAG_FLYING)) {
            return 1;
        }
    }

    return (this->m_flags & (MOVEMENTFLAG_DISABLE_COLLISION | MOVEMENTFLAG_DISABLE_GRAVITY)) != 0;
}

// OFFSET: 0x00A1FD20/0x1407C5DC0
float __stdcall CMovementShared::CalcFallDistance(float time, int32_t safeFall, float velocity) {
    float terminal = safeFall ? s_safeFallTerminalVelocity : s_terminalVelocity;

    if (velocity > terminal) {
        velocity = terminal;
    }

    if (19.291105f * time + velocity > terminal) {
        float accelTime = (terminal - velocity) * 0.051837362f;
        return terminal * (time - accelTime) + (velocity + 9.6455526f * accelTime) * accelTime;
    }

    return time * (velocity + time * 9.6455526f);
}

// OFFSET: 0x00A1FFB0/0x1407C6070
float CMovementShared::GetDistanceFallen() const {
    CMoveSpline* spline = this->m_spline;
    float distance;

    if (spline && (spline->m_flags & SPLINEFLAG_FALLING)) {
        float time = (spline->m_duration - spline->m_effectStartTime) * 0.001f;
        return CalcFallDistance(time, 0, 0.0f);
    }

    if (spline && (spline->m_flags & SPLINEFLAG_PARABOLIC)) {
        if (!spline->m_duration) {
            return 0.0f;
        }

        C3Vector start(0.0f, 0.0f, 0.0f);
        spline->m_spline.Pos(0.0f, start, C3Spline::EVAL_ARCLENGTH);
        float progress = static_cast<int32_t>(this->m_spline->m_start) * 0.001f / (this->m_spline->m_duration * 0.001f);
        C3Vector current(0.0f, 0.0f, 0.0f);
        this->m_spline->m_spline.Pos(progress, current, C3Spline::EVAL_ARCLENGTH);
        distance = start.z - current.z;
    } else {
        distance = this->m_fallStartZ - this->m_position.z;

        if (this->m_fallVelocity < -0.00000023841858f) {
            distance = distance + this->m_fallVelocity * this->m_fallVelocity * 0.025918681f;
        }
    }

    if (distance < 0.0f) {
        return 0.0f;
    }

    return distance;
}

// PROVEN: e4ac0b22 IDENTICAL/NONE
// OFFSET: 0x00A20210/0x1407C6290
int32_t CMovementShared::UpgradePendingRoot() {
    uint32_t flags = this->m_flags;

    if (!(flags & MOVEMENTFLAG_PENDING_ROOT)) {
        return 0;
    }

    this->m_flags = (flags & 0xFF901B00) | MOVEMENTFLAG_ROOT;
    return 1;
}

// PROVEN: 4afe9a90 IDENTICAL/NONE
// OFFSET: 0x00A20230
void CMovementShared::DowngradeRootToPending() {
    if (this->m_flags & MOVEMENTFLAG_ROOT) {
        this->m_flags = (this->m_flags & ~(MOVEMENTFLAG_ROOT | MOVEMENTFLAG_PENDING_ROOT)) | MOVEMENTFLAG_PENDING_ROOT;
    }
}

// PROVEN: d849e60d IDENTICAL/NONE
// OFFSET: 0x00A20610/0x1407C6650
void CMovementShared::StopAllLogging() {
    CMovementGlobals* globals = MovementGetGlobals();

    if (globals && globals->m_movementLog) {
        fclose(globals->m_movementLog);
        globals->m_movementLog = NULL;
    }
}

// OFFSET: 0x00A1FDF0
float CMovementShared::TimeToFallDistance(float distance, int32_t safeFall) {
    float terminal = safeFall ? s_safeFallTerminalVelocity : s_terminalVelocity;
    float accelTime = 0.051837362f * terminal;
    float accelDistance = accelTime * terminal * 0.5f;

    if (distance < accelDistance) {
        if (0.0f < distance) {
            return sqrt(distance * 0.10367472f);
        }

        return 0.0f;
    }

    return (distance - accelDistance) / terminal + accelTime;
}

// OFFSET: 0x00A20E50
C3Vector CMovementShared::ComputeSplineNormal(uint32_t duration, const C44Matrix& frame) const {
    float progress = 1.0f;
    C3Vector point(0.0f, 0.0f, 0.0f);

    if (duration) {
        progress = static_cast<float>(static_cast<int32_t>(this->m_spline->m_start + 1000)) / static_cast<float>(duration);
    }

    if ((this->m_spline->m_flags & SPLINEFLAG_CYCLIC) && progress > 1.0f) {
        progress = progress - 1.0f;
    }

    this->m_spline->m_spline.Pos(fclamp(progress, 0.0f, 1.0f), point, C3Spline::EVAL_ARCLENGTH);
    C2Vector direction(this->m_moveDir);
    C3Vector world = CPassenger::GetPosition(this->m_position);
    C3Vector delta(point.x - world.x, point.y - world.y, point.z - world.z);
    C2Vector target(delta);
    direction.NormalizeSafe();
    target.NormalizeSafe();
    float cosAngle = direction.y * target.y + target.x * direction.x;
    ClampInPlace(cosAngle, -1.0f, 1.0f);
    double angle = target.y * direction.x - direction.y * target.x < 0.0f ? acos(static_cast<double>(cosAngle)) : -acos(static_cast<double>(cosAngle));
    C33Matrix rotation = C33Matrix::Rotation(fclamp(static_cast<float>(angle + angle), -1.5707964f, 1.5707964f), this->m_moveDir, false);
    return *reinterpret_cast<const C3Vector*>(&frame.c0) * rotation;
}

// OFFSET: 0x00A26190
void CMovementShared::Root() {
    this->m_flags |= MOVEMENTFLAG_ROOT;
    this->StopFalling();
    this->m_flags &= 0xFF901F00;
    this->CalcCurrentSpeed(0);
}

// OFFSET: 0x00A26540
CMovementShared::CMovementShared(const WOWGUID& transportGuid, const C3Vector& position, float facing, const WOWGUID& ownerGuid) : CPassenger(transportGuid, position, ownerGuid), m_anchorPos(position) {
    this->m_flags = 0;
    this->m_flags2 = 0;
    this->m_transportSeat = 0xFF;
    this->m_anchorFacing = facing;
    this->m_anchorPitch = 0.0f;
    this->m_moveDir.x = 0.0f;
    this->m_moveDir.y = 0.0f;
    this->m_moveDir.z = 0.0f;
    this->m_moveDir2D.x = 0.0f;
    this->m_moveDir2D.y = 0.0f;
    this->m_fallTimeMs = 0;
    this->m_pitchSin = 0.0f;
    this->m_pitchCos = 1.0f;
    this->unk_002C |= 1;
    this->m_fallStartZ = position.z;
    this->m_spline = NULL;
    this->m_currentSpeed = 0.0f;
    this->m_walkSpeed = 0.0f;
    this->m_runSpeed = 0.0f;
    this->m_runBackSpeed = 0.0f;
    this->m_swimSpeed = 0.0f;
    this->m_swimBackSpeed = 0.0f;
    this->m_flightSpeed = 0.0f;
    this->m_flightBackSpeed = 0.0f;
    this->m_turnRate = 0.0f;
    this->m_pitchRate = 0.0f;
    this->m_fallVelocity = 0.0f;
    this->m_facing = facing;
    this->m_statusTimeMs = OsGetAsyncTimeMs();
    this->m_hoverHeight = 1.0f;
}

// PROVEN: b228fe3b IDENTICAL/NONE
// OFFSET: 0x00A1FBD0
void CMovementShared::CalcCurrentSpeed(int32_t force) {
    if (!(this->m_flags & MOVEMENTFLAG_FALLING) || force) {
        this->m_currentSpeed = this->GetBaseSpeed(force);
    }
}

// OFFSET: 0x00A1FAD0
void CMovementShared::CalcDirection() {
    float facing = this->m_anchorFacing;
    C2Vector dir2D;
    dir2D.x = cos(facing);
    dir2D.y = sin(facing);
    this->m_moveDir2D = dir2D;

    if ((this->m_flags & (MOVEMENTFLAG_SWIMMING | MOVEMENTFLAG_FLYING)) && fabs(this->m_anchorPitch) >= 0.00000095367432f) {
        float pitch = this->m_anchorPitch;
        this->m_pitchCos = cos(pitch);
        this->m_pitchSin = sin(pitch);
        this->m_moveDir.x = this->m_pitchCos * dir2D.x;
        this->m_moveDir.y = this->m_pitchCos * dir2D.y;
        this->m_moveDir.z = this->m_pitchSin;
    } else {
        this->m_moveDir.x = this->m_moveDir2D.x;
        this->m_moveDir.y = this->m_moveDir2D.y;
        this->m_pitchSin = 0.0f;
        this->m_moveDir.z = 0.0f;
        this->m_pitchCos = 1.0f;
    }
}

// OFFSET: 0x00A204A0
void CMovementShared::ComputeSplineDurationModifier(float progress) {
    CMoveSpline* spline = this->m_spline;

    if (!spline) {
        return;
    }

    if (!spline->m_duration || spline->m_duration * spline->m_durationMod < 0.01f) {
        spline->m_durationModNext = 1.0f;
        return;
    }

    float delta = progress - (int32_t)spline->m_start / (spline->m_duration * spline->m_durationMod);

    if (0.5f < delta) {
        delta = delta - 1.0f;
    }

    if (-0.5f > delta) {
        delta = delta + 1.0f;
    }

    spline = this->m_spline;
    int64_t offset = (int64_t)(delta * spline->m_duration);
    spline->m_durationModNext = (float)(spline->m_duration - (uint32_t)offset) / spline->m_duration;

    spline = this->m_spline;
    float modifier = spline->m_durationModNext;

    if (modifier < 0.5f) {
        spline->m_durationModNext = 0.5f;
    } else if (modifier >= 2.0f) {
        spline->m_durationModNext = 2.0f;
    } else {
        spline->m_durationModNext = modifier;
    }
}

// OFFSET: 0x00A21110
void CMovementShared::CalcDirection(int32_t force) {
    if ((this->m_flags & MOVEMENTFLAG_FALLING) && !force) {
        return;
    }

    this->CalcDirection();
    uint32_t flags = this->m_flags;

    if (flags & (MOVEMENTFLAG_FORWARD | MOVEMENTFLAG_BACKWARD)) {
        if (flags & (MOVEMENTFLAG_STRAFE_LEFT | MOVEMENTFLAG_STRAFE_RIGHT)) {
            float forward2DX = this->m_moveDir2D.x;
            float forward2DY = this->m_moveDir2D.y;
            float forwardX = this->m_moveDir.x;
            float forwardY = this->m_moveDir.y;
            float forwardZ = this->m_moveDir.z;

            if (flags & MOVEMENTFLAG_BACKWARD) {
                forward2DX = -forward2DX;
                forward2DY = -forward2DY;
                forwardX = -forwardX;
                forwardY = -forwardY;
                forwardZ = -forwardZ;
            }

            float swap = this->m_moveDir2D.x;
            this->m_moveDir2D.x = this->m_moveDir2D.y;
            this->m_moveDir2D.y = swap;

            if (this->m_flags & MOVEMENTFLAG_STRAFE_LEFT) {
                this->m_moveDir2D.x = -this->m_moveDir2D.x;
            } else {
                this->m_moveDir2D.y = -this->m_moveDir2D.y;
            }

            this->m_moveDir.x = this->m_moveDir2D.x + forwardX;
            this->m_moveDir.y = this->m_moveDir2D.y + forwardY;
            this->m_moveDir.z = forwardZ;
            this->m_moveDir2D.x = this->m_moveDir2D.x + forward2DX;
            this->m_moveDir2D.y = this->m_moveDir2D.y + forward2DY;
            this->m_moveDir.x = this->m_moveDir.x * 0.70710677f;
            this->m_moveDir.y = this->m_moveDir.y * 0.70710677f;
            this->m_moveDir.z = this->m_moveDir.z * 0.70710677f;
            this->m_moveDir2D.x = this->m_moveDir2D.x * 0.70710677f;
            this->m_moveDir2D.y = 0.70710677f * this->m_moveDir2D.y;
            return;
        }
    } else if (flags & (MOVEMENTFLAG_STRAFE_LEFT | MOVEMENTFLAG_STRAFE_RIGHT)) {
        float swap = this->m_moveDir2D.x;
        this->m_moveDir2D.x = this->m_moveDir2D.y;
        this->m_moveDir2D.y = swap;

        if (this->m_flags & MOVEMENTFLAG_STRAFE_LEFT) {
            this->m_moveDir2D.x = -this->m_moveDir2D.x;
        } else {
            this->m_moveDir2D.y = -this->m_moveDir2D.y;
        }

        this->m_moveDir.x = this->m_moveDir2D.x;
        this->m_moveDir.y = this->m_moveDir2D.y;
        this->m_moveDir.z = 0.0f;
        return;
    }

    if (flags & MOVEMENTFLAG_BACKWARD) {
        this->m_moveDir = C3Vector(-this->m_moveDir.x, -this->m_moveDir.y, -this->m_moveDir.z);
        float x = -this->m_moveDir2D.x;
        float y = -this->m_moveDir2D.y;
        this->m_moveDir2D.x = x;
        this->m_moveDir2D.y = y;
    }
}

// PROVEN: 2c482da5 IDENTICAL/NONE
// OFFSET: 0x00A1FDB0
float CMovementShared::CalcFallStartElevation(uint32_t time) const {
    return CalcFallDistance(time * 0.001f, this->m_flags & MOVEMENTFLAG_SAFE_FALL, this->m_fallVelocity) + this->m_position.z;
}

// OFFSET: 0x00A20D40
float CMovementShared::CalcSplineHeight(uint32_t time, float z) const {
    if (!time) {
        return z;
    }

    CMoveSpline* spline = this->m_spline;

    if (time >= spline->m_duration) {
        return z;
    }

    if (spline->m_flags & SPLINEFLAG_PARABOLIC) {
        if (time <= spline->m_effectStartTime) {
            return z;
        }

        float start = spline->m_effectStartTime * 0.001f;
        float elapsed = time * 0.001f - start;
        return (0.001f * this->m_spline->m_duration - start) * this->m_spline->m_verticalAcceleration * 0.5f * elapsed - 0.5f * (elapsed * (this->m_spline->m_verticalAcceleration * elapsed)) + z;
    }

    if (!(spline->m_flags & SPLINEFLAG_FALLING)) {
        return z;
    }

    float height = this->m_fallStartZ - CalcFallDistance(time * 0.001f, spline->m_flags & SPLINEFLAG_FALLING_SLOW, 0.0f);

    if (height <= spline->m_finalDestination.z) {
        return spline->m_finalDestination.z;
    }

    return height;
}

// OFFSET: 0x00A1FE80
float CMovementShared::CalcTimeFallen(float distance, int32_t upward) const {
    float terminal = (this->m_flags & MOVEMENTFLAG_SAFE_FALL) ? s_safeFallTerminalVelocity : s_terminalVelocity;
    float velocity = this->m_fallVelocity;

    if (velocity > terminal) {
        velocity = terminal;
    }

    if (fabs(velocity) < 0.00000023841858f) {
        return TimeToFallDistance(distance, this->m_flags & MOVEMENTFLAG_SAFE_FALL);
    }

    float discriminant = velocity * velocity + 38.582211f * distance;
    float root = 0.0f;

    if (discriminant > 0.0f) {
        root = sqrt(discriminant);
    }

    float timeUp = (-velocity - root) * 0.051837362f;
    float timeDown = (root - velocity) * 0.051837362f;
    float accelTime = 0.051837362f * (terminal - velocity);
    float result;

    if (timeDown <= accelTime) {
        result = timeDown;
    } else {
        result = (distance - (velocity + accelTime * 9.6455526f) * accelTime) / terminal + accelTime;
    }

    if (upward) {
        result = 0.0f;

        if (timeUp >= 0.0f) {
            return timeUp;
        }
    }

    return result;
}

// OFFSET: 0x00605F60
int32_t CMovementShared::CanCollideWhileFlying() const {
    if (this->IsSplineFlyerOrFlyingOrSwimming()) {
        return 1;
    }

    CMoveSpline* spline = this->m_spline;

    if ((this->m_flags2 & MOVEMENTFLAG2_JUMP_SPLINE_IN_AIR) && spline && !(spline->m_flags & SPLINEFLAG_NO_SPLINE) && (spline->m_flags & SPLINEFLAG_PARABOLIC)) {
        return 1;
    }

    spline = this->m_spline;

    if (!spline) {
        return 0;
    }

    if ((spline->m_flags & SPLINEFLAG_NO_SPLINE) || !(spline->m_flags & SPLINEFLAG_FALLING)) {
        return 0;
    }

    return 1;
}

// OFFSET: 0x00A22B80
void CMovementShared::ForceStopStrafe() {
    this->m_flags &= ~(MOVEMENTFLAG_STRAFE_LEFT | MOVEMENTFLAG_STRAFE_RIGHT | MOVEMENTFLAG_PENDING_STRAFE_STOP);
    this->m_currentSpeed = this->GetBaseSpeed(0);
    this->UpdateAnchors(0);
}

// OFFSET: 0x00A258B0
int32_t CMovementShared::ChangeFlightBackSpeed(float speed) {
    if (fabs(speed - this->m_flightBackSpeed) < 0.00000023841858f) {
        return 0;
    }

    this->m_flightBackSpeed = speed;
    this->UpdateAnchors(0);
    this->CalcCurrentSpeed(0);
    return 1;
}

// OFFSET: 0x00A255B0
int32_t CMovementShared::ChangeFlightSpeed(float speed) {
    if (fabs(speed - this->m_flightSpeed) < 0.00000023841858f) {
        return 0;
    }

    this->m_flightSpeed = speed;
    this->UpdateAnchors(0);
    this->CalcCurrentSpeed(0);
    return 1;
}

// OFFSET: 0x00A243D0
int32_t CMovementShared::ChangePitchRate(float rate) {
    if (fabs(rate - this->m_pitchRate) < 0.00000023841858f) {
        return 0;
    }

    this->m_pitchRate = rate;
    this->UpdateAnchors(0);
    return 1;
}

// OFFSET: 0x00A249B0
int32_t CMovementShared::ChangeRunBackSpeed(float speed) {
    if (fabs(speed - this->m_runBackSpeed) < 0.00000023841858f) {
        return 0;
    }

    this->m_runBackSpeed = speed;
    this->UpdateAnchors(0);
    this->CalcCurrentSpeed(0);
    return 1;
}

// OFFSET: 0x00A246B0
int32_t CMovementShared::ChangeRunSpeed(float speed) {
    if (fabs(speed - this->m_runSpeed) < 0.00000023841858f) {
        return 0;
    }

    this->m_runSpeed = speed;
    this->UpdateAnchors(0);
    this->CalcCurrentSpeed(0);
    return 1;
}

// OFFSET: 0x00A252B0
int32_t CMovementShared::ChangeSwimBackSpeed(float speed) {
    if (fabs(speed - this->m_swimBackSpeed) < 0.00000023841858f) {
        return 0;
    }

    this->m_swimBackSpeed = speed;
    this->UpdateAnchors(0);
    this->CalcCurrentSpeed(0);
    return 1;
}

// OFFSET: 0x00A24FB0
int32_t CMovementShared::ChangeSwimSpeed(float speed) {
    if (fabs(speed - this->m_swimSpeed) < 0.00000023841858f) {
        return 0;
    }

    this->m_swimSpeed = speed;
    this->UpdateAnchors(0);
    this->CalcCurrentSpeed(0);
    return 1;
}

// OFFSET: 0x00A240F0
int32_t CMovementShared::ChangeTurnRate(float rate) {
    if (fabs(rate - this->m_turnRate) < 0.00000023841858f) {
        return 0;
    }

    this->m_turnRate = rate;
    this->UpdateAnchors(0);
    return 1;
}

// OFFSET: 0x00A24CB0
int32_t CMovementShared::ChangeWalkSpeed(float speed) {
    if (fabs(speed - this->m_walkSpeed) < 0.00000023841858f) {
        return 0;
    }

    this->m_walkSpeed = speed;
    this->UpdateAnchors(0);
    this->CalcCurrentSpeed(0);
    return 1;
}

// PROVEN: 2ced7c66 IDENTICAL/NONE
// OFFSET: 0x00A25E90
int32_t CMovementShared::EnableCollision(int32_t enable) {
    if (enable) {
        this->m_flags &= ~MOVEMENTFLAG_DISABLE_COLLISION;
    } else {
        this->m_flags |= MOVEMENTFLAG_DISABLE_COLLISION;
    }

    return 0;
}

// OFFSET: 0x00A25EC0
int32_t CMovementShared::EnableGravity(int32_t enable) {
    uint32_t flags = this->m_flags;
    uint32_t newFlags = enable ? flags & ~MOVEMENTFLAG_DISABLE_GRAVITY : flags | MOVEMENTFLAG_DISABLE_GRAVITY;
    this->m_flags = newFlags;

    if (flags == newFlags) {
        return 0;
    }

    this->UpdateAnchors(0);
    return 1;
}

// PROVEN: 9481efa7 IDENTICAL/NONE
// OFFSET: 0x00A203E0
void CMovementShared::FeatherFall(int32_t enable) {
    if (enable) {
        this->m_flags |= MOVEMENTFLAG_SAFE_FALL;
    } else {
        this->m_flags &= ~MOVEMENTFLAG_SAFE_FALL;
    }
}

// OFFSET: 0x00A1F8C0/0x1407C5AA0
float CMovementShared::GetBaseSpeed(int32_t walk) const {
    uint32_t flags = this->m_flags;

    if (!(flags & (MOVEMENTFLAG_FORWARD | MOVEMENTFLAG_BACKWARD | MOVEMENTFLAG_STRAFE_LEFT | MOVEMENTFLAG_STRAFE_RIGHT | MOVEMENTFLAG_ASCENDING | MOVEMENTFLAG_DESCENDING))) {
        return 0.0f;
    }

    CMoveSpline* spline = this->m_spline;

    if (spline && !(spline->m_flags & SPLINEFLAG_NO_SPLINE)) {
        if (!spline->m_duration) {
            return 0.0f;
        }

        return this->m_spline->m_spline.m_length / spline->m_duration * 1000.0f;
    }

    if (flags & MOVEMENTFLAG_FLYING) {
        if ((flags & MOVEMENTFLAG_BACKWARD) && this->m_flightSpeed >= this->m_flightBackSpeed) {
            return this->m_flightBackSpeed;
        }

        return this->m_flightSpeed;
    }

    if (flags & MOVEMENTFLAG_SWIMMING) {
        if ((flags & MOVEMENTFLAG_BACKWARD) && this->m_swimSpeed >= this->m_swimBackSpeed) {
            return this->m_swimBackSpeed;
        }

        return this->m_swimSpeed;
    }

    if ((flags & MOVEMENTFLAG_WALKING) || walk) {
        if (this->m_runSpeed > this->m_walkSpeed) {
            return this->m_walkSpeed;
        }
    } else if ((flags & MOVEMENTFLAG_BACKWARD) && this->m_runSpeed >= this->m_runBackSpeed) {
        return this->m_runBackSpeed;
    }

    return this->m_runSpeed;
}

// OFFSET: 0x00A1FC00
float CMovementShared::GetFeatherFallVelocity() const {
    float terminal = (this->m_flags & MOVEMENTFLAG_SAFE_FALL) ? s_safeFallTerminalVelocity : s_terminalVelocity;
    float velocity = this->m_fallVelocity;

    if (velocity > terminal) {
        velocity = terminal;
    }

    velocity = velocity + this->m_fallTimeMs * 0.001f * 19.291105f;

    if (velocity > terminal) {
        return terminal;
    }

    return velocity;
}

// PROVEN: 25a8f394 IDENTICAL/NONE
// OFFSET: 0x00A1F890
float CMovementShared::GetModifiedSplineDuration() const {
    return this->m_spline->m_duration * this->m_spline->m_durationMod;
}

// PROVEN: b5ee02dd IDENTICAL/NONE
// OFFSET: 0x00A20400
void CMovementShared::Hover(int32_t enable) {
    if (enable) {
        this->m_flags = this->m_flags & ~MOVEMENTFLAG_SPLINE_ELEVATION | MOVEMENTFLAG_HOVER;
    } else {
        this->m_flags &= ~MOVEMENTFLAG_HOVER;
    }
}

// OFFSET: #0x00723350
int32_t CMovementShared::IsFalling() const {
    uint32_t flags = this->m_flags;

    if ((flags & MOVEMENTFLAG_FALLING) && ((flags & MOVEMENTFLAG_FALLING_FAR) || 0.0f != this->m_fallVelocity)) {
        return 1;
    }

    CMoveSpline* spline = this->m_spline;

    if (spline && !(spline->m_flags & SPLINEFLAG_NO_SPLINE) && (spline->m_flags & SPLINEFLAG_FALLING)) {
        return 1;
    }

    if (!(this->m_flags2 & MOVEMENTFLAG2_JUMP_SPLINE_IN_AIR)) {
        return 0;
    }

    if (!spline) {
        return 0;
    }

    if ((spline->m_flags & SPLINEFLAG_NO_SPLINE) || !(spline->m_flags & SPLINEFLAG_PARABOLIC)) {
        return 0;
    }

    return 1;
}

// OFFSET: 0x00A1FCC0
int32_t CMovementShared::IsJumpingUp() const {
    return (this->m_flags & MOVEMENTFLAG_FALLING) && 0.0f != this->m_fallVelocity && this->m_fallVelocity * -0.051837362f > this->m_fallTimeMs * 0.001f;
}

// OFFSET: %0x00581240
int32_t CMovementShared::IsOnFallingSpline() const {
    CMoveSpline* spline = this->m_spline;

    if (spline && !(spline->m_flags & SPLINEFLAG_NO_SPLINE) && (spline->m_flags & SPLINEFLAG_FALLING)) {
        return 1;
    }

    return 0;
}

// OFFSET: 0x0043B7C0
int32_t CMovementShared::IsOnFlyingSpline() const {
    CMoveSpline* spline = this->m_spline;

    if (spline && !(spline->m_flags & SPLINEFLAG_NO_SPLINE) && (spline->m_flags & SPLINEFLAG_FLYING)) {
        return 1;
    }

    return 0;
}

// OFFSET: %0x00581270
int32_t CMovementShared::IsOnParabolicSpline() const {
    CMoveSpline* spline = this->m_spline;

    if (spline && !(spline->m_flags & SPLINEFLAG_NO_SPLINE) && (spline->m_flags & SPLINEFLAG_PARABOLIC)) {
        return 1;
    }

    return 0;
}

// PROVEN: 05c18660 IDENTICAL/NONE
// OFFSET: [0x0043B770]
int32_t CMovementShared::HasFallVelocity() const {
    return (this->m_flags & MOVEMENTFLAG_FALLING) && this->m_fallVelocity != 0.0f;
}

// PROVEN: b1ccd881 IDENTICAL/NONE
// OFFSET: 0x0043B7A0
int32_t CMovementShared::IsOnSpline() const {
    return this->m_spline && !(this->m_spline->m_flags & SPLINEFLAG_NO_SPLINE);
}

// OFFSET: #0x0075EDA0
int32_t CMovementShared::IsSplineFlyerOrFlyingOrSwimming() const {
    if (this->IsGravityDisabled()) {
        return 1;
    }

    return (this->m_flags & (MOVEMENTFLAG_SWIMMING | MOVEMENTFLAG_FLYING)) != 0;
}

// OFFSET: #0x0071C660
int32_t CMovementShared::IsSplineFlyerOrSwimmingOrSafeFalling() const {
    if (this->IsGravityDisabled()) {
        return 1;
    }

    return (this->m_flags & (MOVEMENTFLAG_SWIMMING | MOVEMENTFLAG_SAFE_FALL)) != 0;
}

// OFFSET: #0x0071C720
int32_t CMovementShared::IsSplineFlyerOrHoveringFlyingOrSwimming() const {
    if (this->IsGravityDisabled()) {
        return 1;
    }

    return (this->m_flags & (MOVEMENTFLAG_SWIMMING | MOVEMENTFLAG_FLYING | MOVEMENTFLAG_HOVER)) != 0;
}

// OFFSET: 0x00A1FA50
void CMovementShared::PlotAscendDescend(float dt, C3Vector* move) const {
    float vertical = 0.70710677f;
    float y = this->m_moveDir2D.y * 0.70710677f;

    if (!(this->m_flags & MOVEMENTFLAG_ASCENDING)) {
        vertical = -0.70710677f;
    }

    float speed = this->m_currentSpeed;
    *move = C3Vector(this->m_moveDir2D.x * 0.70710677f * dt * speed, y * dt * speed, speed * (vertical * dt));
}

// PROVEN: f1311031 IDENTICAL/NONE
// OFFSET: 0x00A20640
float CMovementShared::PlotFacing(float dt) const {
    uint32_t flags = this->m_flags;
    float rate;

    if (flags & MOVEMENTFLAG_TURN_LEFT) {
        rate = this->m_turnRate;
    } else if (flags & MOVEMENTFLAG_TURN_RIGHT) {
        rate = -this->m_turnRate;
    } else {
        rate = 0.0f;
    }

    if ((flags & (MOVEMENTFLAG_FORWARD | MOVEMENTFLAG_BACKWARD | MOVEMENTFLAG_STRAFE_LEFT | MOVEMENTFLAG_STRAFE_RIGHT | MOVEMENTFLAG_FALLING | MOVEMENTFLAG_ASCENDING | MOVEMENTFLAG_DESCENDING)) && !(this->m_flags2 & MOVEMENTFLAG2_FULL_SPEED_TURNING)) {
        rate = rate * 0.75f;
    }

    return CMath::normalizeangle0to2pi_(rate * dt + this->m_anchorFacing);
}

// OFFSET: 0x00A20960
void CMovementShared::PlotHorzCircularPosition(float dt, C3Vector* move) const {
    uint32_t flags = this->m_flags;
    float speed = this->m_currentSpeed;
    float rate;

    if (flags & MOVEMENTFLAG_TURN_LEFT) {
        rate = this->m_turnRate;
    } else if (flags & MOVEMENTFLAG_TURN_RIGHT) {
        rate = -this->m_turnRate;
    } else {
        rate = 0.0f;
    }

    if ((flags & (MOVEMENTFLAG_FORWARD | MOVEMENTFLAG_BACKWARD | MOVEMENTFLAG_STRAFE_LEFT | MOVEMENTFLAG_STRAFE_RIGHT | MOVEMENTFLAG_FALLING | MOVEMENTFLAG_ASCENDING | MOVEMENTFLAG_DESCENDING)) && !(this->m_flags2 & MOVEMENTFLAG2_FULL_SPEED_TURNING)) {
        rate = rate * 0.75f;
    }

    float radius = this->m_currentSpeed / rate;
    float angle = rate * dt;
    float c = cos(angle);
    float s = sin(angle);
    float forward = s * radius;
    float side = radius - c * radius;
    float x = this->m_moveDir2D.x * forward - this->m_moveDir2D.y * side;

    if (this->m_flags & MOVEMENTFLAG_ASCENDING) {
        move->x = x * 0.70710677f;
        move->y = (forward * this->m_moveDir2D.y + side * this->m_moveDir2D.x) * 0.70710677f;
        move->z = 0.70710677f * (speed * dt);
    } else if (this->m_flags & MOVEMENTFLAG_DESCENDING) {
        move->x = x * 0.70710677f;
        move->y = (forward * this->m_moveDir2D.y + side * this->m_moveDir2D.x) * 0.70710677f;
        move->z = speed * dt * -0.70710677f;
    } else if (this->m_flags & (MOVEMENTFLAG_FORWARD | MOVEMENTFLAG_BACKWARD)) {
        move->x = x * this->m_pitchCos;
        move->y = (forward * this->m_moveDir2D.y + side * this->m_moveDir2D.x) * this->m_pitchCos;
        move->z = this->m_pitchSin * speed * dt;
    } else {
        move->x = x;
        move->y = forward * this->m_moveDir2D.y + side * this->m_moveDir2D.x;
        move->z = 0.0f;
    }
}

// PROVEN: 2d831ff5 IDENTICAL/NONE
// OFFSET: 0x00A206A0
float CMovementShared::PlotPitch(float dt) const {
    uint32_t flags = this->m_flags;
    float rate;

    if (flags & MOVEMENTFLAG_PITCH_UP) {
        rate = this->m_pitchRate;
    } else if (flags & MOVEMENTFLAG_PITCH_DOWN) {
        rate = -this->m_pitchRate;
    } else {
        rate = 0.0f;
    }

    if ((flags & (MOVEMENTFLAG_FORWARD | MOVEMENTFLAG_BACKWARD | MOVEMENTFLAG_STRAFE_LEFT | MOVEMENTFLAG_STRAFE_RIGHT | MOVEMENTFLAG_FALLING | MOVEMENTFLAG_ASCENDING | MOVEMENTFLAG_DESCENDING)) && !(this->m_flags2 & MOVEMENTFLAG2_FULL_SPEED_PITCHING)) {
        rate = rate * 0.75f;
    }

    return CMath::normalizeAngleNegPiToPi_(rate * dt + this->m_anchorPitch);
}

// OFFSET: 0x00A20700
void CMovementShared::PlotSpiralPosition(float dt, C3Vector* move) const {
    uint32_t flags = this->m_flags;
    float speed = this->m_currentSpeed;
    float turnRate;

    if (flags & MOVEMENTFLAG_TURN_LEFT) {
        turnRate = this->m_turnRate;
    } else if (flags & MOVEMENTFLAG_TURN_RIGHT) {
        turnRate = -this->m_turnRate;
    } else {
        turnRate = 0.0f;
    }

    if ((flags & (MOVEMENTFLAG_FORWARD | MOVEMENTFLAG_BACKWARD | MOVEMENTFLAG_STRAFE_LEFT | MOVEMENTFLAG_STRAFE_RIGHT | MOVEMENTFLAG_FALLING | MOVEMENTFLAG_ASCENDING | MOVEMENTFLAG_DESCENDING)) && !(this->m_flags2 & MOVEMENTFLAG2_FULL_SPEED_TURNING)) {
        turnRate = turnRate * 0.75f;
    }

    float pitchRate;

    if (flags & MOVEMENTFLAG_PITCH_UP) {
        pitchRate = this->m_pitchRate;
    } else if (flags & MOVEMENTFLAG_PITCH_DOWN) {
        pitchRate = -this->m_pitchRate;
    } else {
        pitchRate = 0.0f;
    }

    if ((flags & (MOVEMENTFLAG_FORWARD | MOVEMENTFLAG_BACKWARD | MOVEMENTFLAG_STRAFE_LEFT | MOVEMENTFLAG_STRAFE_RIGHT | MOVEMENTFLAG_FALLING | MOVEMENTFLAG_ASCENDING | MOVEMENTFLAG_DESCENDING)) && !(this->m_flags2 & MOVEMENTFLAG2_FULL_SPEED_PITCHING)) {
        pitchRate = 0.75f * pitchRate;
    }

    float turnRadius = speed * 0.70710677f / turnRate;
    float pitchRadius = speed * 0.70710677f / pitchRate;
    float turnAngle = turnRate * dt;
    float pitchAngle = dt * pitchRate;
    float turnCos = cos(turnAngle);
    float turnSin = sin(turnAngle);
    float pitchCos = cos(pitchAngle);
    float pitchSin = sin(pitchAngle);
    float forward = turnSin * turnRadius;
    float side = turnRadius - turnCos * turnRadius;
    move->x = this->m_moveDir2D.x * forward - this->m_moveDir2D.y * side;
    move->y = forward * this->m_moveDir2D.y + side * this->m_moveDir2D.x;
    move->z = (pitchRadius - pitchCos * pitchRadius) * this->m_pitchCos + pitchRadius * pitchSin * this->m_pitchSin;
}

// OFFSET: 0x00A1F9E0
void CMovementShared::PlotStraight(float dt, C3Vector* move) const {
    float speed = this->m_currentSpeed;
    *move = C3Vector(this->m_moveDir.x * dt * speed, this->m_moveDir.y * dt * speed, dt * this->m_moveDir.z * speed);
}

// OFFSET: 0x00A20B60
uint32_t CMovementShared::PlotUnitMovement(uint32_t time, C3Vector* move, float* facing, float* pitch) const {
    if (!time) {
        return this->m_flags & (MOVEMENTFLAG_FORWARD | MOVEMENTFLAG_BACKWARD | MOVEMENTFLAG_STRAFE_LEFT | MOVEMENTFLAG_STRAFE_RIGHT | MOVEMENTFLAG_FALLING | MOVEMENTFLAG_ASCENDING | MOVEMENTFLAG_DESCENDING);
    }

    float dt = time * 0.001f;
    uint32_t mode = 0;

    if (this->m_flags & (MOVEMENTFLAG_TURN_LEFT | MOVEMENTFLAG_TURN_RIGHT)) {
        if (facing) {
            *facing = this->PlotFacing(dt);
        }

        if (!(this->m_flags & MOVEMENTFLAG_FALLING)) {
            mode = 2;
        }
    }

    if (this->m_flags & (MOVEMENTFLAG_PITCH_UP | MOVEMENTFLAG_PITCH_DOWN)) {
        if (pitch) {
            *pitch = this->PlotPitch(dt);
        }

        if (!(this->m_flags & (MOVEMENTFLAG_ASCENDING | MOVEMENTFLAG_DESCENDING))) {
            mode |= 8;
        }
    }

    uint32_t flags = this->m_flags;

    if (flags & (MOVEMENTFLAG_ASCENDING | MOVEMENTFLAG_DESCENDING)) {
        mode |= 0x10;
    }

    if (flags & (MOVEMENTFLAG_FORWARD | MOVEMENTFLAG_BACKWARD)) {
        mode |= 1;
    }

    if (flags & (MOVEMENTFLAG_STRAFE_LEFT | MOVEMENTFLAG_STRAFE_RIGHT)) {
        mode |= 4;
    }

    switch (mode) {
        case 1:
        case 4:
        case 5:
        case 12: {
            this->PlotStraight(dt, move);
            break;
        }

        case 3:
        case 6:
        case 7:
        case 14:
        case 19:
        case 22:
        case 23: {
            this->PlotHorzCircularPosition(dt, move);
            break;
        }

        case 9:
        case 13: {
            this->PlotVertCircularPosition(dt, move);
            break;
        }

        case 11:
        case 15: {
            this->PlotSpiralPosition(dt, move);
            break;
        }

        case 16:
        case 18: {
            float z = dt * this->m_currentSpeed;

            if (!(flags & MOVEMENTFLAG_ASCENDING)) {
                z = -z;
            }

            move->x = 0.0f;
            move->y = 0.0f;
            move->z = z;
            break;
        }

        case 17:
        case 20:
        case 21: {
            this->PlotAscendDescend(dt, move);
            break;
        }
    }

    return this->m_flags & (MOVEMENTFLAG_FORWARD | MOVEMENTFLAG_BACKWARD | MOVEMENTFLAG_STRAFE_LEFT | MOVEMENTFLAG_STRAFE_RIGHT | MOVEMENTFLAG_FALLING | MOVEMENTFLAG_ASCENDING | MOVEMENTFLAG_DESCENDING);
}

// OFFSET: 0x00A20870
void CMovementShared::PlotVertCircularPosition(float dt, C3Vector* move) const {
    uint32_t flags = this->m_flags;
    float rate;

    if (flags & MOVEMENTFLAG_PITCH_UP) {
        rate = this->m_pitchRate;
    } else if (flags & MOVEMENTFLAG_PITCH_DOWN) {
        rate = -this->m_pitchRate;
    } else {
        rate = 0.0f;
    }

    if ((flags & (MOVEMENTFLAG_FORWARD | MOVEMENTFLAG_BACKWARD | MOVEMENTFLAG_STRAFE_LEFT | MOVEMENTFLAG_STRAFE_RIGHT | MOVEMENTFLAG_FALLING | MOVEMENTFLAG_ASCENDING | MOVEMENTFLAG_DESCENDING)) && !(this->m_flags2 & MOVEMENTFLAG2_FULL_SPEED_PITCHING)) {
        rate = rate * 0.75f;
    }

    float radius = this->m_currentSpeed / rate;
    float angle = rate * dt;
    float c = cos(angle);
    float s = sin(angle);
    float forward = s * radius;
    float up = radius - c * radius;
    move->x = (forward * this->m_pitchCos - up * this->m_pitchSin) * this->m_moveDir2D.x;
    move->y = (forward * this->m_pitchCos - up * this->m_pitchSin) * this->m_moveDir2D.y;
    move->z = up * this->m_pitchCos + forward * this->m_pitchSin;
}

// PROVEN: 1abd4d80 IDENTICAL/NONE
// OFFSET: 0x00A203B0
void CMovementShared::PostFeatherFall() {
    if (this->m_flags & MOVEMENTFLAG_FALLING) {
        this->m_fallVelocity = this->GetFeatherFallVelocity();
        this->m_fallTimeMs = 0;
        this->m_fallStartZ = this->m_position.z;
    }
}

// OFFSET: 0x00A20180
float CMovementShared::RelDistanceFallen(uint32_t time, float z) const {
    uint32_t flags = this->m_flags;
    float velocity = this->m_fallVelocity;

    if (flags & MOVEMENTFLAG_FALLING) {
        return CalcFallDistance(time * 0.001f, flags & MOVEMENTFLAG_SAFE_FALL, velocity) + z - this->m_fallStartZ;
    }

    return CalcFallDistance(time * 0.001f, flags & MOVEMENTFLAG_SAFE_FALL, velocity);
}

// PROVEN: e6d9576d IDENTICAL/NONE
// OFFSET: 0x00A20350
void CMovementShared::UpdateSplineID(uint32_t splineId) {
    this->m_spline->m_id = splineId;
    this->m_spline->m_spline.SetPoints(NULL, 0);
    this->m_spline->m_duration = 0;
}

// PROVEN: 6bf43fc3 IDENTICAL/NONE
// OFFSET: 0x00A205F0
void CMovementShared::ProcessFallReset() {
    this->m_flags |= MOVEMENTFLAG_FALLING_FAR;
    this->m_fallStartZ = this->m_position.z;
    this->m_fallVelocity = 0.0f;
    this->m_fallTimeMs = 0;
}

// OFFSET: 0x00A25BB0
void CMovementShared::SetRunMode(int32_t run) {
    if (run) {
        this->m_flags &= ~MOVEMENTFLAG_WALKING;
    } else {
        this->m_flags |= MOVEMENTFLAG_WALKING;
    }

    this->UpdateAnchors(0);
    this->CalcCurrentSpeed(0);
}

// PROVEN: ede5cbab IDENTICAL/NONE
// OFFSET: 0x00A20330/0x1407C63D0
void CMovementShared::MarkAwaitingLoad() {
    this->m_flags2 |= MOVEMENTFLAG2_AWAITING_LOAD;
}

// PROVEN: f0ecfad8 IDENTICAL/NONE
// OFFSET: 0x00A20450
void CMovementShared::SetAnimTierInTrans(int32_t enable) {
    if (enable) {
        this->m_flags2 |= MOVEMENTFLAG2_ANIM_TIER_IN_TRANS;
    } else {
        this->m_flags2 &= ~MOVEMENTFLAG2_ANIM_TIER_IN_TRANS;
    }
}

// PROVEN: feb66b6e IDENTICAL/NONE
// OFFSET: 0x00A20430/0x1407CD300
void CMovementShared::SetJumpSplineInAir(int32_t enable) {
    if (enable) {
        this->m_flags2 |= MOVEMENTFLAG2_JUMP_SPLINE_IN_AIR;
    } else {
        this->m_flags2 &= ~MOVEMENTFLAG2_JUMP_SPLINE_IN_AIR;
    }
}

// PROVEN: 251bd58a IDENTICAL/NONE
// OFFSET: 0x00A20480
void CMovementShared::SetVehicleExitVoluntary(int32_t enable) {
    if (enable) {
        this->m_flags2 |= MOVEMENTFLAG2_VEHICLE_EXIT_VOLUNTARY;
    } else {
        this->m_flags2 &= ~MOVEMENTFLAG2_VEHICLE_EXIT_VOLUNTARY;
    }
}

// OFFSET: 0x00A22A10
int32_t CMovementShared::StartMove(int32_t forward) {
    this->m_flags &= ~(MOVEMENTFLAG_PENDING_STOP | MOVEMENTFLAG_PENDING_FORWARD | MOVEMENTFLAG_PENDING_BACKWARD);
    uint32_t flags = this->m_flags;
    int32_t falling = 0;

    if (flags & MOVEMENTFLAG_FALLING) {
        if (flags & (MOVEMENTFLAG_FORWARD | MOVEMENTFLAG_BACKWARD | MOVEMENTFLAG_STRAFE_LEFT | MOVEMENTFLAG_STRAFE_RIGHT)) {
            if (forward) {
                if (!(flags & MOVEMENTFLAG_FORWARD)) {
                    this->m_flags = flags | MOVEMENTFLAG_PENDING_FORWARD;
                    return 0;
                }
            } else if (!(flags & MOVEMENTFLAG_BACKWARD)) {
                this->m_flags = flags | MOVEMENTFLAG_PENDING_BACKWARD;
            }

            return 0;
        }

        falling = 1;
    }

    if (forward) {
        this->m_flags = (flags & ~(MOVEMENTFLAG_FORWARD | MOVEMENTFLAG_BACKWARD)) | MOVEMENTFLAG_FORWARD;
    } else {
        this->m_flags = (flags & ~(MOVEMENTFLAG_FORWARD | MOVEMENTFLAG_BACKWARD)) | MOVEMENTFLAG_BACKWARD;
    }

    this->UpdateAnchors(falling);
    this->CalcCurrentSpeed(falling);
    return 1;
}

// OFFSET: 0x00A233E0
void CMovementShared::StartPitch(int32_t up) {
    uint32_t flags = this->m_flags;

    if (!(flags & (MOVEMENTFLAG_SWIMMING | MOVEMENTFLAG_FLYING)) && !(this->m_flags2 & MOVEMENTFLAG2_ALWAYS_ALLOW_PITCHING)) {
        return;
    }

    this->m_flags = flags & ~(MOVEMENTFLAG_PITCH_UP | MOVEMENTFLAG_PITCH_DOWN) | (up ? MOVEMENTFLAG_PITCH_UP : MOVEMENTFLAG_PITCH_DOWN);
    this->m_flags2 &= ~MOVEMENTFLAG2_INTERPOLATED_PITCHING;
    this->UpdateAnchors(0);
}

// OFFSET: 0x00A22E50
void CMovementShared::StartTurn(int32_t left) {
    if (left) {
        this->m_flags = this->m_flags & ~(MOVEMENTFLAG_TURN_LEFT | MOVEMENTFLAG_TURN_RIGHT) | MOVEMENTFLAG_TURN_LEFT;
    } else {
        this->m_flags = this->m_flags & ~(MOVEMENTFLAG_TURN_LEFT | MOVEMENTFLAG_TURN_RIGHT) | MOVEMENTFLAG_TURN_RIGHT;
    }

    this->m_flags2 &= ~MOVEMENTFLAG2_INTERPOLATED_TURNING;
    this->UpdateAnchors(0);
}

// OFFSET: 0x00A23D10
int32_t CMovementShared::StopAscensionDescension() {
    uint32_t flags = this->m_flags;

    if (!(flags & (MOVEMENTFLAG_SWIMMING | MOVEMENTFLAG_FLYING))) {
        return 0;
    }

    this->m_flags = flags & ~(MOVEMENTFLAG_ASCENDING | MOVEMENTFLAG_DESCENDING);
    this->CalcCurrentSpeed(0);
    this->UpdateAnchors(0);
    return 1;
}

// OFFSET: 0x00A21510/0x1407C7680
void CMovementShared::BeginFalling(float velocity) {
    this->DowngradeRootToPending();
    this->UpdateAnchors(0);
    this->m_flags &= ~(MOVEMENTFLAG_ASCENDING | MOVEMENTFLAG_DESCENDING | MOVEMENTFLAG_SPLINE_ELEVATION);

    if (!(this->m_flags2 & MOVEMENTFLAG2_ALWAYS_ALLOW_PITCHING)) {
        this->m_flags &= ~(MOVEMENTFLAG_PITCH_UP | MOVEMENTFLAG_PITCH_DOWN);
    }

    this->CalcCurrentSpeed(0);
    this->m_fallStartZ = this->m_position.z;
    this->m_flags = (this->m_flags & ~(MOVEMENTFLAG_FALLING | MOVEMENTFLAG_SWIMMING | MOVEMENTFLAG_FLYING)) | MOVEMENTFLAG_FALLING;
    this->m_fallTimeMs = 0;
    this->m_fallVelocity = velocity;
}

// OFFSET: 0x00A21840/0x1407C7A00
void CMovementShared::StopFalling() {
    uint32_t flags = this->m_flags;
    uint32_t falling = flags & MOVEMENTFLAG_FALLING;

    if (falling) {
        this->m_flags = flags & ~(MOVEMENTFLAG_FALLING | MOVEMENTFLAG_FALLING_FAR);
    }

    if (!this->UpgradePendingRoot() && !falling) {
        return;
    }

    this->UpdateAnchors(0);
    this->CalcCurrentSpeed(0);
}

// OFFSET: 0x00A21B40
void CMovementShared::StopFallingAlwaysAnchor() {
    if (this->m_flags & MOVEMENTFLAG_FALLING) {
        this->m_flags &= ~(MOVEMENTFLAG_FALLING | MOVEMENTFLAG_FALLING_FAR);
    }

    this->UpgradePendingRoot();

    this->UpdateAnchors(0);
    this->CalcCurrentSpeed(0);
}

// OFFSET: 0x00A236C0
int32_t CMovementShared::StopPitch() {
    uint32_t flags = this->m_flags;

    if (!(flags & (MOVEMENTFLAG_PITCH_UP | MOVEMENTFLAG_PITCH_DOWN))) {
        return 0;
    }

    this->m_flags = flags & ~(MOVEMENTFLAG_PITCH_UP | MOVEMENTFLAG_PITCH_DOWN);
    this->UpdateAnchors(0);
    return 1;
}

// OFFSET: 0x00A23120
int32_t CMovementShared::StopTurn() {
    uint32_t flags = this->m_flags;

    if (!(flags & (MOVEMENTFLAG_TURN_LEFT | MOVEMENTFLAG_TURN_RIGHT))) {
        return 0;
    }

    this->m_flags = flags & ~(MOVEMENTFLAG_TURN_LEFT | MOVEMENTFLAG_TURN_RIGHT);
    this->UpdateAnchors(0);
    return 1;
}

// OFFSET: 0x00A1FC80
float CMovementShared::TimeToJumpPeak() const {
    if (this->m_flags & MOVEMENTFLAG_FALLING) {
        return this->m_fallVelocity * -0.051837362f;
    }

    return 0.0f;
}

// PROVEN: 4bcdae57 IDENTICAL/NONE
// OFFSET: 0x00A214C0
void CMovementShared::UpdateAnchors(int32_t force) {
    this->m_anchorFacing = this->m_facing;
    this->m_anchorPos = this->m_position;
    this->m_anchorPitch = this->m_pitch;
    this->m_anchorElapsedMs = 0;
    this->CalcDirection(force);
    MovementNotifyZoneMgr(*this->m_ownerGuid);
}

// OFFSET: 0x00A202A0
void CMovementShared::UpdateBaseStatus(const CMovementStatus& status) {
    this->m_position = status.m_position;
    this->m_anchorPos = status.m_position;
    float facing = CMath::normalizeangle0to2pi_(status.m_facing);
    this->m_facing = facing;
    this->m_anchorFacing = facing;
    float pitch = CMath::normalizeAngleNegPiToPi_(status.m_pitch);
    this->m_pitch = pitch;
    this->m_anchorPitch = pitch;
    this->m_anchorElapsedMs = 0;
    this->m_stepUpStartZ = status.m_splineElevation;
    this->CalcCurrentSpeed(0);
}

// OFFSET: 0x00603860
void CMovementShared::UpdateFallingFar() {
    uint32_t flags = this->m_flags;

    if (flags & (MOVEMENTFLAG_FALLING_FAR | MOVEMENTFLAG_SAFE_FALL)) {
        return;
    }

    if ((flags & MOVEMENTFLAG_FALLING) && 0.0f != this->m_fallVelocity) {
        if (this->m_fallStartZ - 0.11111111f >= this->m_position.z) {
            this->m_flags = flags | MOVEMENTFLAG_FALLING_FAR;
        }
    } else if (this->m_fallTimeMs >= 500) {
        this->m_flags = flags | MOVEMENTFLAG_FALLING_FAR;
    }
}

// OFFSET: 0x00A20390
void CMovementShared::WalkOnWater(int32_t enable) {
    if (enable) {
        this->m_flags |= MOVEMENTFLAG_WATERWALKING;
    } else {
        this->m_flags &= ~MOVEMENTFLAG_WATERWALKING;
    }
}

// PROVEN: 8271adee IDENTICAL/NONE
// OFFSET: 0x0056E040
C3Vector CMovementShared::GetPosition() const {
    return CPassenger::GetPosition(this->m_position);
}

// PROVEN: f4b9308e IDENTICAL/NONE
// OFFSET: 0x00A20D20/0x1407C6DD0
uint32_t CMovementShared::PlotUnitMovement(uint32_t time, C3Vector* move) {
    return this->PlotUnitMovement(time, move, &this->m_facing, &this->m_pitch);
}

// PROVEN: c32470d7 IDENTICAL/NONE
// OFFSET: 0x00A201F0
float CMovementShared::RelDistanceFallen(uint32_t time) const {
    return this->RelDistanceFallen(time, this->m_position.z);
}

// OFFSET: 0x00A24050
int32_t CMovementShared::SetPitch(float pitch) {
    if (fabs(pitch - this->m_pitch) < 9.5367432e-7f) {
        this->m_flags &= ~(MOVEMENTFLAG_PITCH_UP | MOVEMENTFLAG_PITCH_DOWN);
        return 0;
    }

    pitch = CMath::normalizeAngleNegPiToPi_(pitch);
    uint32_t flags = this->m_flags;
    this->m_pitch = pitch;

    if (flags & (MOVEMENTFLAG_SWIMMING | MOVEMENTFLAG_FLYING)) {
        this->UpdateAnchors(0);
        this->m_flags &= ~(MOVEMENTFLAG_PITCH_UP | MOVEMENTFLAG_PITCH_DOWN);
        return 0;
    }

    if (!(flags & MOVEMENTFLAG_WATERWALKING) || (flags & 0x60080F) || (this->m_flags2 & MOVEMENTFLAG2_UNK_0100) || pitch > -0.6457718f) {
        this->m_flags &= ~(MOVEMENTFLAG_PITCH_UP | MOVEMENTFLAG_PITCH_DOWN);
        return 0;
    }

    this->m_flags &= ~(MOVEMENTFLAG_PITCH_UP | MOVEMENTFLAG_PITCH_DOWN);
    return 1;
}

// OFFSET: 0x00A23FF0
void CMovementShared::SetRawFacing(float facing) {
    if (!(fabs(facing - this->m_facing) < 9.5367432e-7f)) {
        int32_t falling = this->m_flags & MOVEMENTFLAG_FALLING;
        this->m_facing = CMath::normalizeangle0to2pi_(facing);

        if (!falling) {
            this->UpdateAnchors(0);
        }
    }

    this->m_flags &= ~(MOVEMENTFLAG_TURN_LEFT | MOVEMENTFLAG_TURN_RIGHT);
}

// OFFSET: 0x00A23CB0
int32_t CMovementShared::StartAscensionDescension(int32_t ascend) {
    uint32_t flags = this->m_flags;

    if (!(flags & (MOVEMENTFLAG_SWIMMING | MOVEMENTFLAG_FLYING)) || (this->m_flags2 & MOVEMENTFLAG2_NO_STRAFE)) {
        return 0;
    }

    this->m_flags = flags | (ascend ? MOVEMENTFLAG_ASCENDING : MOVEMENTFLAG_DESCENDING);
    this->UpdateAnchors(0);
    this->CalcCurrentSpeed(0);
    return 1;
}

// PROVEN: 63b11f7f IDENTICAL/NONE
// OFFSET: 0x00A23C60
int32_t CMovementShared::StartFlight() {
    uint32_t flags = this->m_flags;

    if (!(flags & MOVEMENTFLAG_CAN_FLY) || (flags & MOVEMENTFLAG_PENDING_ROOT)) {
        return 0;
    }

    this->m_flags = (flags & ~(MOVEMENTFLAG_FALLING | MOVEMENTFLAG_FALLING_FAR | MOVEMENTFLAG_SWIMMING | MOVEMENTFLAG_FLYING | MOVEMENTFLAG_SPLINE_ELEVATION)) | MOVEMENTFLAG_FLYING;
    this->UpdateAnchors(0);
    this->CalcCurrentSpeed(0);
    return 1;
}

// OFFSET: %0x0095DBA0
int32_t CMovementShared::StopStrafe(uint32_t moveFlags) {
    if (!(moveFlags & (MOVEMENTFLAG_STRAFE_LEFT | MOVEMENTFLAG_STRAFE_RIGHT)) || (this->m_flags & MOVEMENTFLAG_FALLING)) {
        return 0;
    }

    this->ForceStopStrafe();
    return 1;
}

// OFFSET: 0x00A22AE0
int32_t CMovementShared::StartStrafe(int32_t left) {
    this->m_flags &= ~(MOVEMENTFLAG_PENDING_STRAFE_STOP | MOVEMENTFLAG_PENDING_STRAFE_LEFT | MOVEMENTFLAG_PENDING_STRAFE_RIGHT);
    uint32_t flags = this->m_flags;

    if (this->m_flags2 & MOVEMENTFLAG2_NO_STRAFE) {
        return 0;
    }

    int32_t force = 0;

    if (flags & MOVEMENTFLAG_FALLING) {
        if (flags & (MOVEMENTFLAG_FORWARD | MOVEMENTFLAG_BACKWARD | MOVEMENTFLAG_STRAFE_LEFT | MOVEMENTFLAG_STRAFE_RIGHT)) {
            if (left) {
                if (!(flags & MOVEMENTFLAG_STRAFE_LEFT)) {
                    this->m_flags = flags | MOVEMENTFLAG_PENDING_STRAFE_LEFT;
                    return 0;
                }
            } else if (!(flags & MOVEMENTFLAG_STRAFE_RIGHT)) {
                this->m_flags = flags | MOVEMENTFLAG_PENDING_STRAFE_RIGHT;
            }

            return 0;
        }

        force = 1;
    }

    this->m_flags = (flags & ~(MOVEMENTFLAG_STRAFE_LEFT | MOVEMENTFLAG_STRAFE_RIGHT)) | (left ? MOVEMENTFLAG_STRAFE_LEFT : MOVEMENTFLAG_STRAFE_RIGHT);
    this->UpdateAnchors(force);
    this->CalcCurrentSpeed(force);
    return 1;
}

// OFFSET: 0x00A21390/0x1407C74C0
C3Vector CMovementShared::GetDirection() const {
    WOWGUID transport = *(const WOWGUID*)&this->m_transportGuid;

    if (!transport) {
        return this->m_moveDir;
    }

    C44Matrix matrix;
    MovementGetTransportMtxX(transport, &matrix, *this->m_ownerGuid, __FILE__, __LINE__);

    return C3Vector(
        this->m_moveDir.x * matrix.a0 + matrix.c0 * this->m_moveDir.z + matrix.b0 * this->m_moveDir.y,
        this->m_moveDir.x * matrix.a1 + matrix.c1 * this->m_moveDir.z + matrix.b1 * this->m_moveDir.y,
        this->m_moveDir.x * matrix.a2 + matrix.c2 * this->m_moveDir.z + matrix.b2 * this->m_moveDir.y);
}

// OFFSET: 0x00A23980
void CMovementShared::StartSwim() {
    this->m_flags = (this->m_flags & ~(MOVEMENTFLAG_SWIMMING | MOVEMENTFLAG_FLYING | MOVEMENTFLAG_SPLINE_ELEVATION)) | MOVEMENTFLAG_SWIMMING;
    this->UpdateAnchors(0);
    this->StopFalling();
    this->CalcCurrentSpeed(0);
}

// OFFSET: %0x00962AF0
int32_t CMovementShared::StopMove(uint32_t moveFlags) {
    if (!(moveFlags & (MOVEMENTFLAG_FORWARD | MOVEMENTFLAG_BACKWARD)) || (this->m_flags & MOVEMENTFLAG_FALLING)) {
        return 0;
    }

    this->ForceStopMove(1);
    return 1;
}

// PROVEN: c3f342cc IDENTICAL/NONE
// OFFSET: 0x00A21E30/0x1407CD4A0
void CMovementShared::RemoveSpline() {
    this->m_flags2 &= ~MOVEMENTFLAG2_JUMP_SPLINE_IN_AIR;

    if (!this->m_spline) {
        return;
    }

    MovementDelSpline(this->m_spline);
    uint32_t falling = this->m_flags & MOVEMENTFLAG_FALLING;
    this->m_spline = NULL;

    if (!falling) {
        this->m_currentSpeed = this->GetBaseSpeed(0);
    }
}

// OFFSET: 0x00A226F0
void CMovementShared::Teleport(const C3Vector& position, float facing, int32_t removeSpline) {
    this->m_position = position;
    this->m_facing = facing;
    this->m_pitch = 0.0f;
    this->StopFalling();
    this->m_anchorFacing = this->m_facing;
    this->m_flags &= 0xFC801F00;
    this->m_anchorPitch = this->m_pitch;
    this->m_anchorPos = this->m_position;
    this->m_anchorElapsedMs = 0;
    this->CalcDirection(0);
    MovementNotifyZoneMgr(*this->m_ownerGuid);

    if (!(this->m_flags & MOVEMENTFLAG_FALLING)) {
        this->m_currentSpeed = this->GetBaseSpeed(0);
    }

    if (this->m_spline) {
        if (removeSpline) {
            this->RemoveSpline();
        } else {
            this->m_spline->m_flags |= SPLINEFLAG_NO_SPLINE;
        }
    }
}

// OFFSET: 0x00A21E80/0x1407C8090
void CMovementShared::GetMoveStatus(uint32_t gameTime, int32_t changeTransport, int32_t includeFall, CMovementStatus* status) const {
    CMovementGlobals* globals = MovementGetGlobals();
    status->m_gameTime = gameTime;
    status->m_moveFlags = this->m_flags;
    status->m_moveExtraFlags = this->m_flags2;

    if (this->m_transportGuid || changeTransport) {
        status->m_hasTransport = true;
        status->m_transport.m_guid = this->m_transportGuid;
        status->m_transport.m_position = this->m_position;
        status->m_transport.m_facing = this->m_facing;
        status->m_transport.m_seat = this->m_transportSeat;
        status->m_transport.m_time = globals->m_transportTime;

        if (globals->m_hasTransportTime2) {
            status->m_transport.m_time2 = globals->m_transportTime2;
            status->m_transport.m_hasTime2 = true;
            globals->m_hasTransportTime2 = 0;
        }
    }

    status->m_position = CPassenger::GetPosition(this->m_position);
    status->m_facing = this->GetFacing(this->m_facing);

    if ((this->m_flags & (MOVEMENTFLAG_SWIMMING | MOVEMENTFLAG_FLYING)) || (this->m_flags2 & MOVEMENTFLAG2_ALWAYS_ALLOW_PITCHING)) {
        status->m_pitch = this->m_pitch;
    }

    if ((this->m_flags & MOVEMENTFLAG_FALLING) || this->m_fallTimeMs || includeFall) {
        status->m_hasFall = true;
        status->m_fall.m_time = this->m_fallTimeMs;
        status->m_fall.m_zSpeed = this->m_fallVelocity;

        if (this->m_flags & MOVEMENTFLAG_FALLING) {
            status->m_fall.m_hasDirection = true;
            status->m_fall.m_direction.m_sinAngle = this->m_moveDir2D.x;
            status->m_fall.m_direction.m_cosAngle = this->m_moveDir2D.y;
            status->m_fall.m_direction.m_xySpeed = this->m_currentSpeed;
        }
    }

    if (this->m_flags & MOVEMENTFLAG_SPLINE_ELEVATION) {
        status->m_splineElevation = this->m_stepUpStartZ;
    }

    status->m_hasSpline = this->m_spline != NULL;
}

// OFFSET: 0x00A21FD0
void CMovementShared::UpdateFallState(const CMovementStatus& status) {
    const CMovementStatus::FallInfo* fall = status.m_hasFall ? &status.m_fall : NULL;
    this->m_fallTimeMs = fall->m_time;
    this->m_fallVelocity = fall->m_zSpeed;
    this->m_fallStartZ = CalcFallDistance(fall->m_time * 0.001f, this->m_flags & MOVEMENTFLAG_SAFE_FALL, fall->m_zSpeed) + this->m_position.z;

    if (fall->m_hasDirection) {
        this->m_moveDir2D.x = fall->m_direction.m_sinAngle;
        this->m_moveDir2D.y = fall->m_direction.m_cosAngle;
        this->m_currentSpeed = fall->m_direction.m_xySpeed;
    } else {
        this->m_moveDir2D.x = 0.0f;
        this->m_moveDir2D.y = 0.0f;
        this->m_currentSpeed = 0.0f;
    }

    this->m_moveDir.x = this->m_moveDir2D.x;
    this->m_moveDir.y = this->m_moveDir2D.y;
    this->m_moveDir.z = 0.0f;
}

// OFFSET: 0x00A220A0
void CMovementShared::TransformSpline(const C44Matrix& matrix, float facing) {
    uint32_t splineFlags = this->m_spline->m_flags;

    if (splineFlags & SPLINEFLAG_FINAL_POINT) {
        *reinterpret_cast<C3Vector*>(this->m_spline->m_face.m_point) *= matrix;
    } else if (splineFlags & SPLINEFLAG_FINAL_ANGLE) {
        this->m_spline->m_face.m_angle = CMath::normalizeangle0to2pi_(this->m_spline->m_face.m_angle + facing);
    }

    TSStackArray<C3Vector> points(static_cast<C3Vector*>(alloca(sizeof(C3Vector) * this->m_spline->m_spline.m_pointCount)), this->m_spline->m_spline.m_pointCount, 1);

    for (uint32_t i = 0; i < points.m_count; i++) {
        points.m_data[i] = this->m_spline->m_spline.Point(i) * matrix;
    }

    this->m_spline->m_spline.SetPoints(points.m_data, points.m_count);
    this->m_spline->m_finalDestination *= matrix;
}

// OFFSET: 0x00A22340
void CMovementShared::TransportTransform(const C44Matrix& matrix, float facing, const C3Vector& oldPosition) {
    uint32_t flags = this->m_flags;

    if (flags & MOVEMENTFLAG_FALLING) {
        this->m_fallStartZ = CalcFallDistance(this->m_fallTimeMs * 0.001f, flags & MOVEMENTFLAG_SAFE_FALL, this->m_fallVelocity) + this->m_position.z;
    }

    if (flags & MOVEMENTFLAG_SPLINE_ELEVATION) {
        this->m_stepUpStartZ = matrix.b2 * oldPosition.y + matrix.a2 * oldPosition.x + matrix.c2 * this->m_stepUpStartZ + matrix.d2;
    }

    this->m_anchorPos *= matrix;
    this->m_anchorFacing = CMath::normalizeangle0to2pi_(facing + this->m_anchorFacing);

    C3Vector direction;
    direction.x = matrix.b0 * this->m_moveDir.y + matrix.c0 * this->m_moveDir.z + matrix.a0 * this->m_moveDir.x;
    direction.y = matrix.b1 * this->m_moveDir.y + matrix.c1 * this->m_moveDir.z + matrix.a1 * this->m_moveDir.x;
    direction.z = matrix.c2 * this->m_moveDir.z + matrix.a2 * this->m_moveDir.x + matrix.b2 * this->m_moveDir.y;
    this->m_moveDir = direction;

    this->m_moveDir2D = C2Vector(this->m_moveDir);
    this->m_moveDir2D.NormalizeSafe();

    if (this->m_spline) {
        this->TransformSpline(matrix, facing);
    }
}

// OFFSET: 0x00A22590
float CMovementShared::AddToTransport(WOWGUID transport, int32_t seat, C44Matrix* inverse, C44Matrix* transportMatrix) {
    C3Vector oldPosition = this->m_position;
    float facing = CPassenger::AddToTransport(transport, inverse, transportMatrix);
    this->TransportTransform(*inverse, facing, oldPosition);
    MovementAddToTransport(this, transport, seat, 0, 0);
    return facing;
}

// OFFSET: 0x00A22600/0x1407C88A0
WOWGUID CMovementShared::UpdateTransportStatus(const CMovementStatus& status) {
    if (!status.m_hasTransport) {
        return WOWGUID();
    }

    WOWGUID transport = status.m_transport.m_guid;

    if (transport) {
        if (!MovementObjIsTransport(transport)) {
            return WOWGUID();
        }

        this->m_position = status.m_transport.m_position;
        this->m_anchorPos = status.m_transport.m_position;
        this->m_facing = status.m_transport.m_facing;
        this->m_anchorFacing = status.m_transport.m_facing;
        this->m_fallStartZ = CalcFallDistance(this->m_fallTimeMs * 0.001f, this->m_flags & MOVEMENTFLAG_SAFE_FALL, this->m_fallVelocity) + this->m_position.z;
        this->m_stepUpStartZ = status.m_position.z - status.m_transport.m_position.z;
        this->m_transportSeat = status.m_transport.m_seat;
    }

    return transport;
}

// OFFSET: 0x00A261C0
int32_t CMovementShared::IsFallingSwimmingFlying(uint32_t ignoreFlags) const {
    if (this->IsGravityDisabled()) {
        return 1;
    }

    return (~ignoreFlags & this->m_flags & (MOVEMENTFLAG_UNK_40000000 | MOVEMENTFLAG_FLYING | MOVEMENTFLAG_SWIMMING | MOVEMENTFLAG_ROOT)) != 0;
}

// OFFSET: 0x00A26220
void CMovementShared::Disable() {
    this->RemoveSpline();
    this->m_globalUnitsLink.Unlink();
    this->m_flags &= 0xFF9FF700;
    this->m_flags2 &= ~MOVEMENTFLAG2_AWAITING_LOAD;
}

// OFFSET: 0x00A262B0
void CMovementShared::Suspend() {
    this->Disable();
    WOWGUID transport = this->m_link.m_next ? this->m_transportGuid : WOWGUID();
    this->m_link.Unlink();
    this->m_transportGuid = WOWGUID();

    if (transport) {
        MovementNotifyTransportUnitRemoved(*this->m_ownerGuid, transport, this->m_transportSeat);
    }
}

// OFFSET: 0x00A26300
int32_t CMovementShared::StartFalling(float velocity) {
    if (this->m_flags & (MOVEMENTFLAG_UNK_40000000 | MOVEMENTFLAG_ROOT)) {
        return 0;
    }

    if (this->m_spline && (this->m_spline->m_flags & (SPLINEFLAG_FALLING | SPLINEFLAG_PARABOLIC))) {
        return 0;
    }

    this->BeginFalling(velocity);
    return 1;
}

// OFFSET: 0x00A26340
int32_t CMovementShared::StartFallingIgnoring(uint32_t ignoreFlags, float velocity) {
    if (~ignoreFlags & this->m_flags & (MOVEMENTFLAG_UNK_40000000 | MOVEMENTFLAG_ROOT)) {
        return 0;
    }

    if (this->m_spline && (this->m_spline->m_flags & (SPLINEFLAG_FALLING | SPLINEFLAG_PARABOLIC))) {
        return 0;
    }

    this->BeginFalling(velocity);
    return 1;
}

// OFFSET: 0x00A26390
int32_t CMovementShared::Jump(int32_t checkHover) {
    if (checkHover && (this->m_flags & MOVEMENTFLAG_HOVER) && !(this->m_flags & MOVEMENTFLAG_SWIMMING)) {
        return 0;
    }

    if (this->m_spline && !(this->m_spline->m_flags & SPLINEFLAG_NO_SPLINE) && (this->m_spline->m_flags & SPLINEFLAG_FLYING)) {
        return 0;
    }

    uint32_t flags = this->m_flags;

    if ((flags & (MOVEMENTFLAG_ROOT | MOVEMENTFLAG_FALLING | MOVEMENTFLAG_FLYING)) || (!(flags & MOVEMENTFLAG_DISABLE_COLLISION) && this->IsGravityDisabled())) {
        return 0;
    }

    if (this->m_flags2 & MOVEMENTFLAG2_NO_JUMPING) {
        return 0;
    }

    this->StartFalling((flags & MOVEMENTFLAG_SWIMMING) ? -9.0967484f : -7.9555473f);
    return 1;
}

// OFFSET: 0x00A26430/0x1407CD440
void CMovementShared::DowngradeRootIfFalling() {
    if (!this->IsFallingSwimmingFlying(MOVEMENTFLAG_ROOT)) {
        this->DowngradeRootToPending();
    }
}

// OFFSET: 0x00A26460
float CMovementShared::RemoveFromTransportFinish(const C44Matrix& matrix, float facing, const C3Vector& oldPosition) {
    this->TransportTransform(matrix, facing, oldPosition);
    this->m_link.Unlink();
    MovementNotifyTransportUnitRemoved(*this->m_ownerGuid, this->m_transportGuid, this->m_transportSeat);
    this->m_flags2 &= ~MOVEMENTFLAG2_AWAITING_LOAD;
    return facing;
}

// OFFSET: 0x00A264F0
void CMovementShared::SetFacing(float facing) {
    WOWGUID transport = this->m_transportGuid;
    float transportFacing = transport ? MovementGetTransportFacing(transport) : 0.0f;
    this->SetRawFacing(facing - transportFacing);
}

// OFFSET: 0x00A26650/0x1407CDAF0
int32_t CMovementShared::TryStartFalling() {
    CMoveSpline* spline = this->m_spline;

    if (!spline || (spline->m_flags & SPLINEFLAG_NO_SPLINE) || !(spline->m_flags & SPLINEFLAG_FALLING)) {
        if (spline && !(spline->m_flags & SPLINEFLAG_NO_SPLINE) && (spline->m_flags & SPLINEFLAG_FLYING)) {
            return 0;
        }

        if (this->m_flags & (MOVEMENTFLAG_DISABLE_COLLISION | MOVEMENTFLAG_DISABLE_GRAVITY)) {
            return 0;
        }
    }

    uint32_t flags = this->m_flags;

    if (!(flags & (MOVEMENTFLAG_UNK_40000000 | MOVEMENTFLAG_FLYING | MOVEMENTFLAG_SWIMMING | MOVEMENTFLAG_FALLING | MOVEMENTFLAG_ROOT)) && !(flags & (MOVEMENTFLAG_UNK_40000000 | MOVEMENTFLAG_ROOT)) && (!spline || !(spline->m_flags & (SPLINEFLAG_FALLING | SPLINEFLAG_PARABOLIC)))) {
        this->StartFalling(0.0f);
        return 1;
    }

    return 0;
}

// PROVEN: be94fad9 IDENTICAL/NONE
// OFFSET: 0x00A266C0/0x1407CDC10
float CMovementShared::RemoveFromTransport(C44Matrix* matrix) {
    C3Vector oldPosition = this->m_position;
    float facing = CPassenger::RemoveFromTransport(matrix);
    return this->RemoveFromTransportFinish(*matrix, facing, oldPosition);
}

// PROVEN: 9c598c81 IDENTICAL/NONE
// OFFSET: 0x00A20250
void CMovementShared::AddSpline(const C3Vector& position) {
    if (!this->m_spline) {
        this->m_spline = MovementNewSpline();
    }

    this->m_spline->m_flags = 0;
    this->m_spline->m_finalDestination = position;
}

// OFFSET: 0x00572080
void CMovementShared::ResetFallOnControlLoss() {
    if (this->m_flags & MOVEMENTFLAG_PENDING_ROOT) {
        this->Root();
    }
}

// PROVEN: 005a6019 IDENTICAL/NONE
// OFFSET: 0x00A20340/0x1407C63E0
void CMovementShared::UnMarkAwaitingLoad() {
    this->m_flags2 &= ~MOVEMENTFLAG2_AWAITING_LOAD;
}

// OFFSET: 0x00A26700
void CMovementShared::ForceStopMove(int32_t tryFall) {
    this->m_flags &= ~(MOVEMENTFLAG_FORWARD | MOVEMENTFLAG_BACKWARD | MOVEMENTFLAG_PENDING_STOP);
    this->m_currentSpeed = this->GetBaseSpeed(0);
    this->UpdateAnchors(0);

    if (this->m_spline && !(this->m_spline->m_flags & SPLINEFLAG_NO_SPLINE)) {
        this->m_spline->m_flags |= SPLINEFLAG_NO_SPLINE;

        if (tryFall && this->m_spline && (this->m_spline->m_flags & SPLINEFLAG_FLYING)) {
            this->TryStartFalling();
        }
    }
}

// PROVEN: f1105040 IDENTICAL/NONE
// OFFSET: 0x00A26A00
int32_t CMovementShared::StopStrafe() {
    uint32_t flags = this->m_flags;

    if (!(flags & (MOVEMENTFLAG_STRAFE_LEFT | MOVEMENTFLAG_STRAFE_RIGHT))) {
        if (flags & (MOVEMENTFLAG_PENDING_STRAFE_LEFT | MOVEMENTFLAG_PENDING_STRAFE_RIGHT)) {
            this->m_flags = flags & ~(MOVEMENTFLAG_PENDING_STRAFE_LEFT | MOVEMENTFLAG_PENDING_STRAFE_RIGHT);
        }

        return 0;
    }

    if (flags & MOVEMENTFLAG_SPLINE_ELEVATION) {
        this->m_flags = flags & ~MOVEMENTFLAG_SPLINE_ELEVATION;
        this->TryStartFalling();
    }

    if (this->m_flags & MOVEMENTFLAG_FALLING) {
        this->m_flags = (this->m_flags & ~(MOVEMENTFLAG_PENDING_STRAFE_STOP | MOVEMENTFLAG_PENDING_STRAFE_LEFT | MOVEMENTFLAG_PENDING_STRAFE_RIGHT)) | MOVEMENTFLAG_PENDING_STRAFE_STOP;
        return 0;
    }

    this->ForceStopStrafe();
    return 1;
}

// OFFSET: 0x00A26A60
void CMovementShared::StopSwim() {
    uint16_t flags2 = this->m_flags2;
    this->m_flags &= ~(MOVEMENTFLAG_ASCENDING | MOVEMENTFLAG_DESCENDING);

    if (!(flags2 & MOVEMENTFLAG2_ALWAYS_ALLOW_PITCHING)) {
        this->m_flags &= ~(MOVEMENTFLAG_PITCH_UP | MOVEMENTFLAG_PITCH_DOWN);
        this->m_anchorPitch = 0.0f;
        this->m_pitch = 0.0f;
    }

    if (!(flags2 & MOVEMENTFLAG2_UNK_0800) && !this->IsFallingSwimmingFlying(MOVEMENTFLAG_SWIMMING) && !(this->m_flags & MOVEMENTFLAG_FALLING)) {
        this->StartFallingIgnoring(MOVEMENTFLAG_SWIMMING, 0.0f);
    }

    this->m_flags &= ~MOVEMENTFLAG_SWIMMING;
    this->CalcCurrentSpeed(0);
    this->UpdateAnchors(0);
}

// OFFSET: 0x00A26D90
void CMovementShared::StopFlight() {
    this->m_flags &= ~(MOVEMENTFLAG_PITCH_UP | MOVEMENTFLAG_PITCH_DOWN | MOVEMENTFLAG_ASCENDING | MOVEMENTFLAG_DESCENDING);
    this->m_anchorPitch = 0.0f;
    this->m_pitch = 0.0f;

    if (!(this->m_flags2 & MOVEMENTFLAG2_UNK_0800) && !this->IsFallingSwimmingFlying(MOVEMENTFLAG_FLYING | MOVEMENTFLAG_ROOT) && !(this->m_flags & MOVEMENTFLAG_FALLING)) {
        this->StartFallingIgnoring(MOVEMENTFLAG_FLYING | MOVEMENTFLAG_ROOT, 0.0f);
    }

    this->m_flags &= ~MOVEMENTFLAG_FLYING;
    this->CalcCurrentSpeed(0);
    this->UpdateAnchors(0);
}

// PROVEN: 68264aae IDENTICAL/NONE
// OFFSET: 0x00A270B0
void CMovementShared::TryRoot() {
    uint32_t flags = this->m_flags;

    if (flags & (MOVEMENTFLAG_PENDING_ROOT | MOVEMENTFLAG_ROOT)) {
        return;
    }

    if (this->m_spline) {
        this->m_flags = flags | MOVEMENTFLAG_ROOT;
    } else if (flags & MOVEMENTFLAG_FALLING) {
        this->ForceStopStrafe();
        this->ForceStopMove(1);
        this->m_flags |= MOVEMENTFLAG_PENDING_ROOT;
    } else {
        this->Root();
    }
}

// PROVEN: fa7136c8 IDENTICAL/NONE
// OFFSET: 0x00A27100
void CMovementShared::UnRoot(int32_t tryFall) {
    this->m_flags &= ~(MOVEMENTFLAG_ROOT | MOVEMENTFLAG_PENDING_ROOT);

    if (tryFall) {
        this->TryStartFalling();
    }
}

// PROVEN: 231507a8 IDENTICAL/NONE
// OFFSET: 0x00A27120
void CMovementShared::OnSpline(uint32_t timePassed, const C3Vector* points, uint32_t pointCount, uint32_t duration, uint32_t splineFlags, uint32_t splineId) {
    this->m_spline->m_flags = splineFlags;
    this->m_spline->m_timePassed = timePassed;
    this->m_spline->m_start = 0;
    this->m_spline->m_duration = duration;
    this->m_spline->m_spline.m_splineMode = (this->m_spline->m_flags >> 11) & 1;
    this->m_spline->m_spline.SetPoints(points, pointCount);
    this->m_spline->m_id = splineId;
    this->m_spline->m_durationModNext = 1.0f;
    this->m_spline->m_durationMod = 1.0f;
    this->StopFalling();
    this->StartMove(!(this->m_spline->m_flags & SPLINEFLAG_BACKWARD));

    if (this->m_spline->m_flags & SPLINEFLAG_FALLING) {
        if (this->m_flags & MOVEMENTFLAG_FLYING) {
            this->StopFlight();
        }

        this->m_fallStartZ = this->m_position.z;
    }
}

// PROVEN: f87dcef5 IDENTICAL/NONE
// OFFSET: 0x00A27200
int32_t CMovementShared::StopMove() {
    uint32_t flags = this->m_flags;

    if (!(flags & (MOVEMENTFLAG_FORWARD | MOVEMENTFLAG_BACKWARD))) {
        if (flags & (MOVEMENTFLAG_PENDING_FORWARD | MOVEMENTFLAG_PENDING_BACKWARD)) {
            this->m_flags = flags & ~(MOVEMENTFLAG_PENDING_FORWARD | MOVEMENTFLAG_PENDING_BACKWARD);
        }

        return 0;
    }

    if (flags & MOVEMENTFLAG_SPLINE_ELEVATION) {
        this->m_flags = flags & ~MOVEMENTFLAG_SPLINE_ELEVATION;
        this->TryStartFalling();
    }

    if (this->m_flags & MOVEMENTFLAG_FALLING) {
        this->m_flags = (this->m_flags & ~(MOVEMENTFLAG_PENDING_STOP | MOVEMENTFLAG_PENDING_FORWARD | MOVEMENTFLAG_PENDING_BACKWARD)) | MOVEMENTFLAG_PENDING_STOP;
        return 0;
    }

    this->ForceStopMove(1);
    return 1;
}

// PROVEN: 07658f82 IDENTICAL/NONE
// OFFSET: 0x00A27260
void CMovementShared::ConvertCurrentSplineToLoopingSpline(uint32_t timePassed) {
    CMoveSpline* spline = this->m_spline;

    if (!spline) {
        return;
    }

    uint32_t count = spline->m_spline.m_pointCount;

    if (count < 4) {
        return;
    }

    C3Vector* points = static_cast<C3Vector*>(alloca(sizeof(C3Vector) * count));

    if (!points) {
        return;
    }

    this->m_spline = MovementNewSpline();

    if (!this->m_spline) {
        this->m_spline = spline;
        return;
    }

    spline->m_spline.GetPoints(points, count);
    points[1] = points[count - 3];
    int32_t looping = spline->m_spline.GetSplineMode() == 2;
    this->OnSpline(timePassed, &points[1], count - 1, spline->m_duration, spline->m_flags & ~(SPLINEFLAG_FINAL_POINT | SPLINEFLAG_FINAL_TARGET | SPLINEFLAG_FINAL_ANGLE), spline->m_id);
    this->m_spline->m_effectStartTime = 0;
    this->m_spline->m_verticalAcceleration = 0.0f;

    if (looping) {
        this->m_spline->m_spline.m_splineMode = 2;
    }

    MovementDelSpline(spline);
}

// OFFSET: 0x00A27380
uint32_t CMovementShared::PlotUnitSplineMovement(uint32_t time, C3Vector* position, C3Vector* normal) {
    *position = this->m_position;

    if (!(this->m_flags & (MOVEMENTFLAG_FORWARD | MOVEMENTFLAG_BACKWARD))) {
        return this->m_flags & 0x60080F;
    }

    if (this->m_spline->m_flags & SPLINEFLAG_FROZEN) {
        return 1;
    }

    this->m_spline->m_start = time - this->m_spline->m_timePassed;
    CMoveSpline* spline = this->m_spline;
    uint32_t duration = RoundToInt64(this->GetModifiedSplineDuration());
    float t;

    if (!duration) {
        spline->m_flags |= SPLINEFLAG_DONE;
        t = 1.0f;
    } else if (static_cast<int32_t>(spline->m_start) < 0) {
        t = 0.0f;
    } else if (spline->m_start < duration) {
        t = static_cast<float>(static_cast<int32_t>(spline->m_start)) / static_cast<float>(duration);
    } else if (!(spline->m_flags & SPLINEFLAG_CYCLIC)) {
        spline->m_flags |= SPLINEFLAG_DONE;
        t = 1.0f;
    } else {
        spline->m_start -= duration;
        this->m_spline->m_timePassed = time - this->m_spline->m_start;

        if (this->m_spline->m_flags & SPLINEFLAG_ENTER_CYCLE) {
            this->ConvertCurrentSplineToLoopingSpline(this->m_spline->m_timePassed);
            this->m_spline->m_flags &= ~SPLINEFLAG_ENTER_CYCLE;
        }

        this->m_spline->m_durationMod = this->m_spline->m_durationModNext;
        this->m_spline->m_durationModNext = 1.0f;
        duration = RoundToInt64(this->GetModifiedSplineDuration());

        if (duration) {
            t = static_cast<float>(static_cast<int32_t>(this->m_spline->m_start)) / static_cast<float>(duration);
        } else {
            t = 1.0f;
        }
    }

    C44Matrix frame(this->m_moveDir.x, this->m_moveDir.y, this->m_moveDir.z, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
    this->m_spline->m_spline.Frame(t, frame, C3Spline::EVAL_ARCLENGTH);
    spline = this->m_spline;

    if (!(spline->m_flags & (SPLINEFLAG_ORIENTATION_FIXED | SPLINEFLAG_FALLING | SPLINEFLAG_UNK_0008)) && frame.a0 * frame.a0 + frame.a1 * frame.a1 > 0.0018490001f) {
        this->m_facing = static_cast<float>(atan2(static_cast<double>(frame.a1), static_cast<double>(frame.a0)));
    }

    if (spline->m_flags & SPLINEFLAG_BACKWARD) {
        this->m_facing = this->m_facing - 3.1415927f;
    }

    this->m_facing = CMath::normalizeangle0to2pi_(this->m_facing);
    this->m_moveDir = *reinterpret_cast<C3Vector*>(&frame.a0);

    if (fabs(this->m_moveDir.z * this->m_moveDir.z + this->m_moveDir.y * this->m_moveDir.y + this->m_moveDir.x * this->m_moveDir.x) < 0.00000095367432f) {
        this->m_moveDir.x = 1.0f;
    }

    if (this->m_flags & (MOVEMENTFLAG_SWIMMING | MOVEMENTFLAG_FLYING)) {
        this->m_pitch = static_cast<float>(asin(static_cast<double>(frame.a2)));
    } else if ((this->m_spline->m_flags & (SPLINEFLAG_FLYING | SPLINEFLAG_CATMULLROM)) == (SPLINEFLAG_FLYING | SPLINEFLAG_CATMULLROM) && normal) {
        *normal = this->ComputeSplineNormal(duration, frame);
    }

    *position = *reinterpret_cast<C3Vector*>(&frame.d0);
    spline = this->m_spline;
    uint32_t flags = spline->m_flags;

    if (flags & SPLINEFLAG_ANIMATION) {
        if (!(this->m_flags2 & MOVEMENTFLAG2_ANIM_TIER_IN_TRANS) && spline->m_start > spline->m_effectStartTime) {
            this->m_flags2 |= MOVEMENTFLAG2_ANIM_TIER_IN_TRANS;
            return 1;
        }
    } else if (flags & SPLINEFLAG_PARABOLIC) {
        if (spline->m_start > spline->m_effectStartTime && !(this->m_flags2 & MOVEMENTFLAG2_JUMP_SPLINE_IN_AIR)) {
            this->m_fallStartZ = this->m_position.z;
            this->m_flags2 |= MOVEMENTFLAG2_JUMP_SPLINE_IN_AIR;
        }

        position->z = this->CalcSplineHeight(this->m_spline->m_start, position->z);

        if (position->z > this->m_fallStartZ) {
            this->m_fallStartZ = this->m_position.z;
            return 1;
        }
    } else if (flags & SPLINEFLAG_FALLING) {
        float z = this->CalcSplineHeight(spline->m_start, position->z);
        position->z = z;
        CMoveSpline* falling = this->m_spline;

        if (fequal(z, falling->m_finalDestination.z)) {
            int32_t fallTime = static_cast<int32_t>(TimeToFallDistance(this->m_fallStartZ - z, falling->m_flags & SPLINEFLAG_FALLING_SLOW) * 1000.0f);

            if (static_cast<int32_t>(falling->m_start) > fallTime) {
                falling->m_start = fallTime;
            }

            this->m_spline->m_flags |= SPLINEFLAG_DONE;
        }
    }

    return 1;
}

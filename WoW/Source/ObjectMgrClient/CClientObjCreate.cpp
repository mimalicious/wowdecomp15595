#include "WoW/Source/ObjectMgrClient/CClientObjCreate.h"
#include "Engine/Source/Base/CDataStore.h"
#include "WoW/Source/JamClientAutoCode/JamClientMessage.h"

// OFFSET: 0x00699EC0
CClientObjCreate::CClientObjCreate() {
    this->m_hasMovement = false;
    this->m_hasGOTransport = false;
    this->m_hasStationaryPosition = false;
    this->m_hasTarget = false;
    this->m_hasTransportTime = false;
    this->m_hasVehicle = false;
    this->m_hasAnimKits = false;
    this->m_animKits[0] = 0;
    this->m_animKits[1] = 0;
    this->m_animKits[2] = 0;
    this->m_hasRotation = false;
    this->m_hasAreaTrigger = false;
    this->m_isActivePlayer = false;
    this->unk_0001 = false;
    this->unk_0002 = false;
    this->unk_0003 = false;
    this->unk_0004 = false;
}

// OFFSET: 0x00699F90/0x1403464F0
CClientObjCreate::~CClientObjCreate() {
}

static void GetGuidByte(CDataStore* msg, uint8_t& value) {
    if (value) {
        uint8_t byte = 0;
        msg->Get(byte);
        value ^= byte;
    }
}

// OFFSET: 0x00698320
void CClientObjCreate::Get(CDataStore* msg, bool a2) {
    CMovementStatus& status = this->m_move.m_status;
    CClientMoveSplineData& spline = this->m_move.m_spline.m_data;
    uint8_t* guid = reinterpret_cast<uint8_t*>(&status.m_guid.m_guid);
    uint8_t* transportGuid = reinterpret_cast<uint8_t*>(&status.m_transport.m_guid.m_guid);
    uint8_t* facingGuid = reinterpret_cast<uint8_t*>(&spline.m_facingTarget.m_guid);
    uint8_t* goTransportGuid = reinterpret_cast<uint8_t*>(&this->m_goTransport.m_guid.m_guid);
    uint8_t* targetGuid = reinterpret_cast<uint8_t*>(&this->m_target.m_guid);

    JamBitReader bits(msg);
    this->unk_0003 = bits.ReadBit();
    this->unk_0004 = bits.ReadBit();
    this->m_hasRotation = bits.ReadBit();
    this->m_hasAnimKits = bits.ReadBit();
    this->m_hasTarget = bits.ReadBit();
    this->m_isActivePlayer = bits.ReadBit();
    this->m_hasVehicle = bits.ReadBit();
    this->m_hasMovement = bits.ReadBit();
    this->m_stopFrames.SetCount(bits.ReadBits<24>());
    this->unk_0001 = bits.ReadBit();
    this->m_hasGOTransport = bits.ReadBit();
    this->m_hasStationaryPosition = bits.ReadBit();
    this->m_hasAreaTrigger = bits.ReadBit();
    this->unk_0002 = bits.ReadBit();
    this->m_hasTransportTime = bits.ReadBit();

    if (this->m_hasMovement) {
        status.m_moveFlags = !bits.ReadBit();
        status.m_facing = bits.ReadBit() ? 0.0f : 1.0f;
        guid[7] = bits.ReadBit();
        guid[3] = bits.ReadBit();
        guid[2] = bits.ReadBit();

        if (status.m_moveFlags) {
            status.m_moveFlags = bits.ReadBits<30>();
        }

        status.m_hasSpline = bits.ReadBit();
        status.m_pitch = bits.ReadBit() ? 0.0f : 1.0f;
        this->m_move.m_hasSpline = bits.ReadBit();
        status.m_hasFall = bits.ReadBit();
        status.m_splineElevation = bits.ReadBit() ? 0.0f : 1.0f;
        guid[5] = bits.ReadBit();
        status.m_hasTransport = bits.ReadBit();
        status.m_gameTime = !bits.ReadBit();

        if (status.m_hasTransport) {
            transportGuid[1] = bits.ReadBit();
            status.m_transport.m_hasTime2 = bits.ReadBit();
            transportGuid[4] = bits.ReadBit();
            transportGuid[0] = bits.ReadBit();
            transportGuid[6] = bits.ReadBit();
            status.m_transport.m_hasVehicleId = bits.ReadBit();
            transportGuid[7] = bits.ReadBit();
            transportGuid[5] = bits.ReadBit();
            transportGuid[3] = bits.ReadBit();
            transportGuid[2] = bits.ReadBit();
        }

        guid[4] = bits.ReadBit();

        if (this->m_move.m_hasSpline) {
            this->m_move.m_spline.m_hasData = bits.ReadBit();

            if (this->m_move.m_spline.m_hasData) {
                spline.m_mode = bits.ReadBits<2>();
                spline.m_hasEffectStartTime = bits.ReadBit();
                spline.m_points.SetCount(bits.ReadBits<22>());
                spline.m_faceType = bits.ReadBits<2>();

                if (spline.m_faceType == 2) {
                    facingGuid[4] = bits.ReadBit();
                    facingGuid[3] = bits.ReadBit();
                    facingGuid[7] = bits.ReadBit();
                    facingGuid[2] = bits.ReadBit();
                    facingGuid[6] = bits.ReadBit();
                    facingGuid[1] = bits.ReadBit();
                    facingGuid[0] = bits.ReadBit();
                    facingGuid[5] = bits.ReadBit();
                }

                spline.m_hasVerticalAcceleration = bits.ReadBit();
                spline.m_flags = bits.ReadBits<25>();
            }
        }

        guid[6] = bits.ReadBit();

        if (status.m_hasFall) {
            status.m_fall.m_hasDirection = bits.ReadBit();
        }

        guid[0] = bits.ReadBit();
        guid[1] = bits.ReadBit();
        status.m_heightChangeFailed = bits.ReadBit();
        status.m_moveExtraFlags = !bits.ReadBit();

        if (status.m_moveExtraFlags) {
            status.m_moveExtraFlags = bits.ReadBits<12>();
        }
    }

    if (this->m_hasGOTransport) {
        goTransportGuid[5] = bits.ReadBit();
        this->m_goTransport.m_hasVehicleId = bits.ReadBit();
        goTransportGuid[0] = bits.ReadBit();
        goTransportGuid[3] = bits.ReadBit();
        goTransportGuid[6] = bits.ReadBit();
        goTransportGuid[1] = bits.ReadBit();
        goTransportGuid[4] = bits.ReadBit();
        goTransportGuid[2] = bits.ReadBit();
        this->m_goTransport.m_hasTime2 = bits.ReadBit();
        goTransportGuid[7] = bits.ReadBit();
    }

    if (this->m_hasTarget) {
        targetGuid[2] = bits.ReadBit();
        targetGuid[7] = bits.ReadBit();
        targetGuid[0] = bits.ReadBit();
        targetGuid[4] = bits.ReadBit();
        targetGuid[5] = bits.ReadBit();
        targetGuid[6] = bits.ReadBit();
        targetGuid[1] = bits.ReadBit();
        targetGuid[3] = bits.ReadBit();
    }

    if (this->m_hasAnimKits) {
        this->m_animKits[0] = !bits.ReadBit();
        this->m_animKits[1] = !bits.ReadBit();
        this->m_animKits[2] = !bits.ReadBit();
    }

    for (uint32_t i = 0; i < this->m_stopFrames.Count(); i++) {
        uint32_t value = 0;
        msg->Get(value);
        this->m_stopFrames[i] = value;
    }

    if (this->m_hasMovement) {
        GetGuidByte(msg, guid[4]);
        msg->Get(this->m_move.m_runBackSpeed);

        if (status.m_hasFall) {
            if (status.m_fall.m_hasDirection) {
                msg->Get(status.m_fall.m_direction.m_xySpeed);
                msg->Get(status.m_fall.m_direction.m_sinAngle);
                msg->Get(status.m_fall.m_direction.m_cosAngle);
            }

            msg->Get(status.m_fall.m_time);
            msg->Get(status.m_fall.m_zSpeed);
        }

        msg->Get(this->m_move.m_swimBackSpeed);

        if (status.m_splineElevation != 0.0f) {
            msg->Get(status.m_splineElevation);
        }

        if (this->m_move.m_hasSpline) {
            if (this->m_move.m_spline.m_hasData) {
                if (spline.m_hasVerticalAcceleration) {
                    msg->Get(spline.m_verticalAcceleration);
                }

                msg->Get(spline.m_timePassed);

                if (spline.m_faceType == 0) {
                    msg->Get(spline.m_facingAngle);
                }

                if (spline.m_faceType == 2) {
                    GetGuidByte(msg, facingGuid[5]);
                    GetGuidByte(msg, facingGuid[3]);
                    GetGuidByte(msg, facingGuid[7]);
                    GetGuidByte(msg, facingGuid[1]);
                    GetGuidByte(msg, facingGuid[6]);
                    GetGuidByte(msg, facingGuid[4]);
                    GetGuidByte(msg, facingGuid[2]);
                    GetGuidByte(msg, facingGuid[0]);
                }

                for (uint32_t i = 0; i < spline.m_points.Count(); i++) {
                    C3Vector& point = spline.m_points[i];
                    msg->Get(point.z);
                    msg->Get(point.x);
                    msg->Get(point.y);
                }

                if (spline.m_faceType == 1) {
                    msg->Get(spline.m_facingPoint.x);
                    msg->Get(spline.m_facingPoint.z);
                    msg->Get(spline.m_facingPoint.y);
                }

                msg->Get(spline.m_durationModNext);
                msg->Get(spline.m_duration);

                if (spline.m_hasEffectStartTime) {
                    msg->Get(spline.m_effectStartTime);
                }

                msg->Get(spline.m_durationMod);
            }

            msg->Get(this->m_move.m_spline.m_endPoint.z);
            msg->Get(this->m_move.m_spline.m_endPoint.x);
            msg->Get(this->m_move.m_spline.m_endPoint.y);
            msg->Get(this->m_move.m_spline.m_id);
        }

        msg->Get(status.m_position.z);
        GetGuidByte(msg, guid[5]);

        if (status.m_hasTransport) {
            GetGuidByte(msg, transportGuid[5]);
            GetGuidByte(msg, transportGuid[7]);
            msg->Get(status.m_transport.m_time);
            msg->Get(status.m_transport.m_facing);

            if (status.m_transport.m_hasTime2) {
                msg->Get(status.m_transport.m_time2);
            }

            msg->Get(status.m_transport.m_position.y);
            msg->Get(status.m_transport.m_position.x);
            GetGuidByte(msg, transportGuid[3]);
            msg->Get(status.m_transport.m_position.z);
            GetGuidByte(msg, transportGuid[0]);

            if (status.m_transport.m_hasVehicleId) {
                msg->Get(status.m_transport.m_vehicleId);
            }

            msg->Get(status.m_transport.m_seat);
            GetGuidByte(msg, transportGuid[1]);
            GetGuidByte(msg, transportGuid[6]);
            GetGuidByte(msg, transportGuid[2]);
            GetGuidByte(msg, transportGuid[4]);
        }

        msg->Get(status.m_position.x);
        msg->Get(this->m_move.m_pitchRate);
        GetGuidByte(msg, guid[3]);
        GetGuidByte(msg, guid[0]);
        msg->Get(this->m_move.m_swimSpeed);
        msg->Get(status.m_position.y);
        GetGuidByte(msg, guid[7]);
        GetGuidByte(msg, guid[1]);
        GetGuidByte(msg, guid[2]);
        msg->Get(this->m_move.m_walkSpeed);

        if (status.m_gameTime) {
            msg->Get(status.m_gameTime);
        }

        msg->Get(this->m_move.m_turnRate);
        GetGuidByte(msg, guid[6]);
        msg->Get(this->m_move.m_flightSpeed);

        if (status.m_facing != 0.0f) {
            msg->Get(status.m_facing);
        }

        msg->Get(this->m_move.m_runSpeed);

        if (status.m_pitch != 0.0f) {
            msg->Get(status.m_pitch);
        }

        msg->Get(this->m_move.m_flightBackSpeed);
    }

    if (this->m_hasVehicle) {
        msg->Get(this->m_vehicleOrientation);
        msg->Get(this->m_vehicleId);
    }

    if (this->m_hasGOTransport) {
        GetGuidByte(msg, goTransportGuid[0]);
        GetGuidByte(msg, goTransportGuid[5]);

        if (this->m_goTransport.m_hasVehicleId) {
            msg->Get(this->m_goTransport.m_vehicleId);
        }

        GetGuidByte(msg, goTransportGuid[3]);
        msg->Get(this->m_goTransport.m_position.x);
        GetGuidByte(msg, goTransportGuid[4]);
        GetGuidByte(msg, goTransportGuid[6]);
        GetGuidByte(msg, goTransportGuid[1]);
        msg->Get(this->m_goTransport.m_time);
        msg->Get(this->m_goTransport.m_position.y);
        GetGuidByte(msg, goTransportGuid[2]);
        GetGuidByte(msg, goTransportGuid[7]);
        msg->Get(this->m_goTransport.m_position.z);
        msg->Get(this->m_goTransport.m_seat);
        msg->Get(this->m_goTransport.m_facing);

        if (this->m_goTransport.m_hasTime2) {
            msg->Get(this->m_goTransport.m_time2);
        }
    }

    if (this->m_hasRotation) {
        msg->Get(this->m_packedRotation);
    }

    if (this->m_hasAreaTrigger) {
        msg->Get(this->m_areaTrigger[13]);
        msg->Get(this->m_areaTrigger[4]);
        msg->Get(this->m_areaTrigger[7]);
        msg->Get(this->m_areaTrigger[15]);
        msg->Get(this->unk_020C);
        msg->Get(this->m_areaTrigger[10]);
        msg->Get(this->m_areaTrigger[11]);
        msg->Get(this->m_areaTrigger[3]);
        msg->Get(this->m_areaTrigger[5]);
        msg->Get(this->m_areaTrigger[14]);
        msg->Get(this->m_areaTrigger[6]);
        msg->Get(this->m_areaTrigger[1]);
        msg->Get(this->m_areaTrigger[8]);
        msg->Get(this->m_areaTrigger[12]);
        msg->Get(this->m_areaTrigger[0]);
        msg->Get(this->m_areaTrigger[2]);
        msg->Get(this->m_areaTrigger[9]);
    }

    if (this->m_hasStationaryPosition) {
        msg->Get(this->m_stationaryFacing);
        msg->Get(this->m_stationaryPosition.x);
        msg->Get(this->m_stationaryPosition.y);
        msg->Get(this->m_stationaryPosition.z);
    }

    if (this->m_hasTarget) {
        GetGuidByte(msg, targetGuid[4]);
        GetGuidByte(msg, targetGuid[0]);
        GetGuidByte(msg, targetGuid[3]);
        GetGuidByte(msg, targetGuid[5]);
        GetGuidByte(msg, targetGuid[7]);
        GetGuidByte(msg, targetGuid[6]);
        GetGuidByte(msg, targetGuid[2]);
        GetGuidByte(msg, targetGuid[1]);
    }

    if (this->m_hasAnimKits) {
        if (this->m_animKits[0]) {
            msg->Get(this->m_animKits[0]);
        }

        if (this->m_animKits[1]) {
            msg->Get(this->m_animKits[1]);
        }

        if (this->m_animKits[2]) {
            msg->Get(this->m_animKits[2]);
        }
    }

    if (this->m_hasTransportTime) {
        msg->Get(this->m_transportTime);
    }
}

// OFFSET: 0x00698200
bool CClientObjCreate::IsValid() const {
    return (!this->m_hasMovement || (!this->m_hasGOTransport && !this->m_hasStationaryPosition))
        && (!this->m_hasTarget || this->m_target.m_guid)
        && (!this->m_isActivePlayer || this->m_hasMovement)
        && (!this->m_hasGOTransport || this->m_goTransport.IsValid())
        && (!this->m_hasMovement || this->m_move.IsValid());
}

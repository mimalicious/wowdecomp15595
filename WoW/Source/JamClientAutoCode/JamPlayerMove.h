#ifndef WOW_SOURCE_JAMCLIENTAUTOCODE_JAMPLAYERMOVE_H
#define WOW_SOURCE_JAMCLIENTAUTOCODE_JAMPLAYERMOVE_H

#include <cstddef>
#include <cstdint>
#include "WoW/Source/JamClientAutoCode/JamClientMessage.h"
#include "WoW/Source/Object/CMovementStatus.h"
#include "WoW/Source/Object/WGUID.h"

struct JamMoveSpeedAck {
    /* 0000 */ CMovementStatus m_status;
    /* 0088 */ uint32_t m_ackIndex;
    /* 008C */ uint32_t unk_008C;
    /* 0090 */ float m_speed;
};

class PlayerChangeSeatsOnControlledVehicle : public JamClientMessage {
    public:
    /* 0010 */ WOWGUID m_vehicleGuid;
    /* 0018 */ uint8_t m_seat;
    /* 0020 */ CMovementStatus m_status;

    void Put(CDataStore* msg) override;

    PlayerChangeSeatsOnControlledVehicle();
    ~PlayerChangeSeatsOnControlledVehicle();
};

class PlayerDismissControlledVehicle : public JamClientMessage {
    public:
    /* 0010 */ CMovementStatus m_status;

    void Put(CDataStore* msg) override;

    PlayerDismissControlledVehicle();
    ~PlayerDismissControlledVehicle();
};

class PlayerMoveChangeTransport : public JamClientMessage {
    public:
    /* 0010 */ CMovementStatus m_status;

    void Put(CDataStore* msg) override;

    PlayerMoveChangeTransport();
    ~PlayerMoveChangeTransport();
};

class PlayerMoveFallLand : public JamClientMessage {
    public:
    /* 0010 */ CMovementStatus m_status;

    void Put(CDataStore* msg) override;

    PlayerMoveFallLand();
    ~PlayerMoveFallLand();
};

class PlayerMoveFallReset : public JamClientMessage {
    public:
    /* 0010 */ CMovementStatus m_status;

    void Put(CDataStore* msg) override;

    PlayerMoveFallReset();
    ~PlayerMoveFallReset();
};

class PlayerMoveFeatherFallAck : public JamClientMessage {
    public:
    /* 0010 */ CMovementStatus m_status;
    /* 0098 */ uint32_t m_ackIndex;

    void Put(CDataStore* msg) override;

    PlayerMoveFeatherFallAck();
    ~PlayerMoveFeatherFallAck();
};

class PlayerMoveFlightBackSpeedChangeAck : public JamClientMessage {
    public:
    /* 0010 */ JamMoveSpeedAck m_data;

    void Put(CDataStore* msg) override;

    PlayerMoveFlightBackSpeedChangeAck();
    ~PlayerMoveFlightBackSpeedChangeAck();
};

class PlayerMoveFlightSpeedChangeAck : public JamClientMessage {
    public:
    /* 0010 */ JamMoveSpeedAck m_data;

    void Put(CDataStore* msg) override;

    PlayerMoveFlightSpeedChangeAck();
    ~PlayerMoveFlightSpeedChangeAck();
};

class PlayerMoveGravityDisableAck : public JamClientMessage {
    public:
    /* 0010 */ CMovementStatus m_status;
    /* 0098 */ uint32_t m_ackIndex;

    void Put(CDataStore* msg) override;

    PlayerMoveGravityDisableAck();
    ~PlayerMoveGravityDisableAck();
};

class PlayerMoveGravityEnableAck : public JamClientMessage {
    public:
    /* 0010 */ CMovementStatus m_status;
    /* 0098 */ uint32_t m_ackIndex;

    void Put(CDataStore* msg) override;

    PlayerMoveGravityEnableAck();
    ~PlayerMoveGravityEnableAck();
};

class PlayerMoveHeartbeat : public JamClientMessage {
    public:
    /* 0010 */ CMovementStatus m_status;

    void Put(CDataStore* msg) override;

    PlayerMoveHeartbeat();
    ~PlayerMoveHeartbeat();
};

class PlayerMoveHoverAck : public JamClientMessage {
    public:
    /* 0010 */ CMovementStatus m_status;
    /* 0098 */ uint32_t m_ackIndex;

    void Put(CDataStore* msg) override;

    PlayerMoveHoverAck();
    ~PlayerMoveHoverAck();
};

class PlayerMoveJump : public JamClientMessage {
    public:
    /* 0010 */ CMovementStatus m_status;

    void Put(CDataStore* msg) override;

    PlayerMoveJump();
    ~PlayerMoveJump();
};

class PlayerMoveKnockBackAck : public JamClientMessage {
    public:
    /* 0010 */ CMovementStatus m_status;
    /* 0098 */ uint32_t m_ackIndex;

    void Put(CDataStore* msg) override;

    PlayerMoveKnockBackAck();
    ~PlayerMoveKnockBackAck();
};

class PlayerMoveNotActiveMover : public JamClientMessage {
    public:
    /* 0010 */ CMovementStatus m_status;

    void Put(CDataStore* msg) override;

    PlayerMoveNotActiveMover();
    ~PlayerMoveNotActiveMover();
};

class PlayerMovePitchRateChangeAck : public JamClientMessage {
    public:
    /* 0010 */ JamMoveSpeedAck m_data;

    void Put(CDataStore* msg) override;

    PlayerMovePitchRateChangeAck();
    ~PlayerMovePitchRateChangeAck();
};

class PlayerMoveRootAck : public JamClientMessage {
    public:
    /* 0010 */ CMovementStatus m_status;
    /* 0098 */ uint32_t m_ackIndex;

    void Put(CDataStore* msg) override;

    PlayerMoveRootAck();
    ~PlayerMoveRootAck();
};

class PlayerMoveRunBackSpeedChangeAck : public JamClientMessage {
    public:
    /* 0010 */ JamMoveSpeedAck m_data;

    void Put(CDataStore* msg) override;

    PlayerMoveRunBackSpeedChangeAck();
    ~PlayerMoveRunBackSpeedChangeAck();
};

class PlayerMoveRunSpeedChangeAck : public JamClientMessage {
    public:
    /* 0010 */ JamMoveSpeedAck m_data;

    void Put(CDataStore* msg) override;

    PlayerMoveRunSpeedChangeAck();
    ~PlayerMoveRunSpeedChangeAck();
};

class PlayerMoveSetCanFlyAck : public JamClientMessage {
    public:
    /* 0010 */ CMovementStatus m_status;
    /* 0098 */ uint32_t m_ackIndex;

    void Put(CDataStore* msg) override;

    PlayerMoveSetCanFlyAck();
    ~PlayerMoveSetCanFlyAck();
};

class PlayerMoveSetCanTransitionBetweenSwimAndFlyAck : public JamClientMessage {
    public:
    /* 0010 */ CMovementStatus m_status;
    /* 0098 */ uint32_t m_ackIndex;

    void Put(CDataStore* msg) override;

    PlayerMoveSetCanTransitionBetweenSwimAndFlyAck();
    ~PlayerMoveSetCanTransitionBetweenSwimAndFlyAck();
};

class PlayerMoveSetCollisionHeightAck : public JamClientMessage {
    public:
    /* 0010 */ uint32_t m_reason;
    /* 0018 */ CMovementStatus m_status;
    /* 00A0 */ uint32_t m_ackIndex;
    /* 00A4 */ uint32_t unk_00A4;
    /* 00A8 */ float m_height;

    void Put(CDataStore* msg) override;

    PlayerMoveSetCollisionHeightAck();
    ~PlayerMoveSetCollisionHeightAck();
};

class PlayerMoveSetFacing : public JamClientMessage {
    public:
    /* 0010 */ CMovementStatus m_status;

    void Put(CDataStore* msg) override;

    PlayerMoveSetFacing();
    ~PlayerMoveSetFacing();
};

class PlayerMoveSetFly : public JamClientMessage {
    public:
    /* 0010 */ CMovementStatus m_status;

    void Put(CDataStore* msg) override;

    PlayerMoveSetFly();
    ~PlayerMoveSetFly();
};

class PlayerMoveSetPitch : public JamClientMessage {
    public:
    /* 0010 */ CMovementStatus m_status;

    void Put(CDataStore* msg) override;

    PlayerMoveSetPitch();
    ~PlayerMoveSetPitch();
};

class PlayerMoveSetRunMode : public JamClientMessage {
    public:
    /* 0010 */ CMovementStatus m_status;

    void Put(CDataStore* msg) override;

    PlayerMoveSetRunMode();
    ~PlayerMoveSetRunMode();
};

class PlayerMoveSetVehicleRecIdAck : public JamClientMessage {
    public:
    /* 0010 */ uint32_t m_vehicleRecId;
    /* 0018 */ CMovementStatus m_status;
    /* 00A0 */ uint32_t m_ackIndex;

    void Put(CDataStore* msg) override;

    PlayerMoveSetVehicleRecIdAck();
    ~PlayerMoveSetVehicleRecIdAck();
};

class PlayerMoveSetWalkMode : public JamClientMessage {
    public:
    /* 0010 */ CMovementStatus m_status;

    void Put(CDataStore* msg) override;

    PlayerMoveSetWalkMode();
    ~PlayerMoveSetWalkMode();
};

class PlayerMoveSplineDone : public JamClientMessage {
    public:
    /* 0010 */ CMovementStatus m_status;
    /* 0098 */ uint32_t m_splineId;

    void Put(CDataStore* msg) override;

    PlayerMoveSplineDone();
    ~PlayerMoveSplineDone();
};

class PlayerMoveStartAscend : public JamClientMessage {
    public:
    /* 0010 */ CMovementStatus m_status;

    void Put(CDataStore* msg) override;

    PlayerMoveStartAscend();
    ~PlayerMoveStartAscend();
};

class PlayerMoveStartBackward : public JamClientMessage {
    public:
    /* 0010 */ CMovementStatus m_status;

    void Put(CDataStore* msg) override;

    PlayerMoveStartBackward();
    ~PlayerMoveStartBackward();
};

class PlayerMoveStartDescend : public JamClientMessage {
    public:
    /* 0010 */ CMovementStatus m_status;

    void Put(CDataStore* msg) override;

    PlayerMoveStartDescend();
    ~PlayerMoveStartDescend();
};

class PlayerMoveStartForward : public JamClientMessage {
    public:
    /* 0010 */ CMovementStatus m_status;

    void Put(CDataStore* msg) override;

    PlayerMoveStartForward();
    ~PlayerMoveStartForward();
};

class PlayerMoveStartPitchDown : public JamClientMessage {
    public:
    /* 0010 */ CMovementStatus m_status;

    void Put(CDataStore* msg) override;

    PlayerMoveStartPitchDown();
    ~PlayerMoveStartPitchDown();
};

class PlayerMoveStartPitchUp : public JamClientMessage {
    public:
    /* 0010 */ CMovementStatus m_status;

    void Put(CDataStore* msg) override;

    PlayerMoveStartPitchUp();
    ~PlayerMoveStartPitchUp();
};

class PlayerMoveStartStrafeLeft : public JamClientMessage {
    public:
    /* 0010 */ CMovementStatus m_status;

    void Put(CDataStore* msg) override;

    PlayerMoveStartStrafeLeft();
    ~PlayerMoveStartStrafeLeft();
};

class PlayerMoveStartStrafeRight : public JamClientMessage {
    public:
    /* 0010 */ CMovementStatus m_status;

    void Put(CDataStore* msg) override;

    PlayerMoveStartStrafeRight();
    ~PlayerMoveStartStrafeRight();
};

class PlayerMoveStartSwim : public JamClientMessage {
    public:
    /* 0010 */ CMovementStatus m_status;

    void Put(CDataStore* msg) override;

    PlayerMoveStartSwim();
    ~PlayerMoveStartSwim();
};

class PlayerMoveStartTurnLeft : public JamClientMessage {
    public:
    /* 0010 */ CMovementStatus m_status;

    void Put(CDataStore* msg) override;

    PlayerMoveStartTurnLeft();
    ~PlayerMoveStartTurnLeft();
};

class PlayerMoveStartTurnRight : public JamClientMessage {
    public:
    /* 0010 */ CMovementStatus m_status;

    void Put(CDataStore* msg) override;

    PlayerMoveStartTurnRight();
    ~PlayerMoveStartTurnRight();
};

class PlayerMoveStop : public JamClientMessage {
    public:
    /* 0010 */ CMovementStatus m_status;

    void Put(CDataStore* msg) override;

    PlayerMoveStop();
    ~PlayerMoveStop();
};

class PlayerMoveStopAscend : public JamClientMessage {
    public:
    /* 0010 */ CMovementStatus m_status;

    void Put(CDataStore* msg) override;

    PlayerMoveStopAscend();
    ~PlayerMoveStopAscend();
};

class PlayerMoveStopPitch : public JamClientMessage {
    public:
    /* 0010 */ CMovementStatus m_status;

    void Put(CDataStore* msg) override;

    PlayerMoveStopPitch();
    ~PlayerMoveStopPitch();
};

class PlayerMoveStopStrafe : public JamClientMessage {
    public:
    /* 0010 */ CMovementStatus m_status;

    void Put(CDataStore* msg) override;

    PlayerMoveStopStrafe();
    ~PlayerMoveStopStrafe();
};

class PlayerMoveStopSwim : public JamClientMessage {
    public:
    /* 0010 */ CMovementStatus m_status;

    void Put(CDataStore* msg) override;

    PlayerMoveStopSwim();
    ~PlayerMoveStopSwim();
};

class PlayerMoveStopTurn : public JamClientMessage {
    public:
    /* 0010 */ CMovementStatus m_status;

    void Put(CDataStore* msg) override;

    PlayerMoveStopTurn();
    ~PlayerMoveStopTurn();
};

class PlayerMoveSwimBackSpeedChangeAck : public JamClientMessage {
    public:
    /* 0010 */ JamMoveSpeedAck m_data;

    void Put(CDataStore* msg) override;

    PlayerMoveSwimBackSpeedChangeAck();
    ~PlayerMoveSwimBackSpeedChangeAck();
};

class PlayerMoveSwimSpeedChangeAck : public JamClientMessage {
    public:
    /* 0010 */ JamMoveSpeedAck m_data;

    void Put(CDataStore* msg) override;

    PlayerMoveSwimSpeedChangeAck();
    ~PlayerMoveSwimSpeedChangeAck();
};

class PlayerMoveTeleportAck : public JamClientMessage {
    public:
    /* 0010 */ WOWGUID m_guid;
    /* 0018 */ uint32_t m_ackIndex;
    /* 001C */ uint32_t m_time;

    void Put(CDataStore* msg) override;

    PlayerMoveTeleportAck();
    ~PlayerMoveTeleportAck();
};

class PlayerMoveTimeSkipped : public JamClientMessage {
    public:
    /* 0010 */ WOWGUID m_guid;
    /* 0018 */ uint32_t m_timeSkipped;

    void Put(CDataStore* msg) override;

    PlayerMoveTimeSkipped();
    ~PlayerMoveTimeSkipped();
};

class PlayerMoveToggleCollisionCheat : public JamClientMessage {
    public:
    /* 0010 */ CMovementStatus m_status;

    void Put(CDataStore* msg) override;

    PlayerMoveToggleCollisionCheat();
    ~PlayerMoveToggleCollisionCheat();
};

class PlayerMoveTurnRateChangeAck : public JamClientMessage {
    public:
    /* 0010 */ JamMoveSpeedAck m_data;

    void Put(CDataStore* msg) override;

    PlayerMoveTurnRateChangeAck();
    ~PlayerMoveTurnRateChangeAck();
};

class PlayerMoveUnrootAck : public JamClientMessage {
    public:
    /* 0010 */ CMovementStatus m_status;
    /* 0098 */ uint32_t m_ackIndex;

    void Put(CDataStore* msg) override;

    PlayerMoveUnrootAck();
    ~PlayerMoveUnrootAck();
};

class PlayerMoveWalkSpeedChangeAck : public JamClientMessage {
    public:
    /* 0010 */ JamMoveSpeedAck m_data;

    void Put(CDataStore* msg) override;

    PlayerMoveWalkSpeedChangeAck();
    ~PlayerMoveWalkSpeedChangeAck();
};

class PlayerMoveWaterWalkAck : public JamClientMessage {
    public:
    /* 0010 */ CMovementStatus m_status;
    /* 0098 */ uint32_t m_ackIndex;

    void Put(CDataStore* msg) override;

    PlayerMoveWaterWalkAck();
    ~PlayerMoveWaterWalkAck();
};

class PlayerSetActiveMover : public JamClientMessage {
    public:
    /* 0010 */ WOWGUID m_guid;

    void Put(CDataStore* msg) override;

    PlayerSetActiveMover();
    ~PlayerSetActiveMover();
};

class PlayerMoveUpdateMissileTrajectory : public JamClientMessage {
    public:
    /* 0010 */ WOWGUID m_guid;
    /* 0018 */ bool m_hasStatus;
    /* 0020 */ CMovementStatus m_status;
    /* 00A8 */ C3Vector m_impactPos;
    /* 00B4 */ uint32_t m_spellID;
    /* 00B8 */ uint16_t m_moveMsgID;
    /* 00BC */ float m_pitch;
    /* 00C0 */ float m_speed;
    /* 00C4 */ C3Vector m_firePos;

    void Put(CDataStore* msg) override;

    PlayerMoveUpdateMissileTrajectory();
    ~PlayerMoveUpdateMissileTrajectory();
};

class PlayerMoveTeleportCheat : public JamClientMessage {
    public:
    /* 0010 */ C3Vector m_position;
    /* 001C */ float m_facing;

    void Put(CDataStore* msg) override;

    PlayerMoveTeleportCheat(const C3Vector& position, float facing);
    ~PlayerMoveTeleportCheat();
};

class PlayerMoveSetRawPosition : public JamClientMessage {
    public:
    /* 0010 */ float m_facing;
    /* 0014 */ C3Vector m_position;

    void Put(CDataStore* msg) override;

    PlayerMoveSetRawPosition(const C3Vector& position, float facing);
    ~PlayerMoveSetRawPosition();
};

class PlayerMoveCharmPortCheat : public JamClientMessage {
    public:
    /* 0010 */ float m_facing;
    /* 0014 */ C3Vector m_position;

    void Put(CDataStore* msg) override;

    PlayerMoveCharmPortCheat(const C3Vector& position, float facing);
    ~PlayerMoveCharmPortCheat();
};

#endif

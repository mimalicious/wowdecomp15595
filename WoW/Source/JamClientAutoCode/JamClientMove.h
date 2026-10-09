#ifndef WOW_SOURCE_JAMCLIENTAUTOCODE_JAMCLIENTMOVE_H
#define WOW_SOURCE_JAMCLIENTAUTOCODE_JAMCLIENTMOVE_H

#include <cstdint>
#include "Engine/Source/Storm/H/stpl.h"
#include "Engine/Source/Tempest/C2Vector.h"
#include "WoW/Source/JamClientAutoCode/JamClientMessage.h"
#include "WoW/Source/Object/CMovementStatus.h"
#include "WoW/Source/Object/WGUID.h"

class CDataStore;

class ClientAuraPointsDepleted : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientAuraPointsDepleted* msg);

    /* 0010 */ WOWGUID m_guid;
    /* 0018 */ uint8_t m_slot;
    /* 0019 */ uint8_t m_effectIndex;

    ClientAuraPointsDepleted(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientAuraPointsDepleted();
    bool Get(CDataStore* msg, bool full) override;
    bool Handle();
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientMoveCollisionDisable : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientMoveCollisionDisable* msg);

    /* 0010 */ WOWGUID m_guid;
    /* 0018 */ uint32_t m_ackIndex;

    ClientMoveCollisionDisable(WOWGUID guid, uint32_t ackIndex);
    ClientMoveCollisionDisable(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientMoveCollisionDisable();
    bool Get(CDataStore* msg, bool full) override;
    bool Handle();
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientMoveCollisionEnable : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientMoveCollisionEnable* msg);

    /* 0010 */ WOWGUID m_guid;
    /* 0018 */ uint32_t m_ackIndex;

    ClientMoveCollisionEnable(WOWGUID guid, uint32_t ackIndex);
    ClientMoveCollisionEnable(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientMoveCollisionEnable();
    bool Get(CDataStore* msg, bool full) override;
    bool Handle();
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientMoveGravityDisable : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientMoveGravityDisable* msg);

    /* 0010 */ WOWGUID m_guid;
    /* 0018 */ uint32_t m_ackIndex;

    ClientMoveGravityDisable(WOWGUID guid, uint32_t ackIndex);
    ClientMoveGravityDisable(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientMoveGravityDisable();
    bool Get(CDataStore* msg, bool full) override;
    bool Handle();
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientMoveGravityEnable : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientMoveGravityEnable* msg);

    /* 0010 */ WOWGUID m_guid;
    /* 0018 */ uint32_t m_ackIndex;

    ClientMoveGravityEnable(WOWGUID guid, uint32_t ackIndex);
    ClientMoveGravityEnable(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientMoveGravityEnable();
    bool Get(CDataStore* msg, bool full) override;
    bool Handle();
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientMoveKnockBack : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientMoveKnockBack* msg);

    /* 0010 */ WOWGUID m_guid;
    /* 0018 */ float m_horizontalSpeed;
    /* 001C */ uint32_t m_ackIndex;
    /* 0020 */ float m_verticalSpeed;
    /* 0024 */ C2Vector m_direction;

    ClientMoveKnockBack(WOWGUID guid, uint32_t ackIndex, const C2Vector& direction, float horizontalSpeed, float verticalSpeed);
    ClientMoveKnockBack(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientMoveKnockBack();
    bool Get(CDataStore* msg, bool full) override;
    bool Handle();
    bool IsValid() const;
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientMoveRoot : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientMoveRoot* msg);

    /* 0010 */ WOWGUID m_guid;
    /* 0018 */ uint32_t m_ackIndex;

    ClientMoveRoot(WOWGUID guid, uint32_t ackIndex);
    ClientMoveRoot(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientMoveRoot();
    bool Get(CDataStore* msg, bool full) override;
    bool Handle();
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientMoveSetCanFly : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientMoveSetCanFly* msg);

    /* 0010 */ WOWGUID m_guid;
    /* 0018 */ uint32_t m_ackIndex;

    ClientMoveSetCanFly(WOWGUID guid, uint32_t ackIndex);
    ClientMoveSetCanFly(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientMoveSetCanFly();
    bool Get(CDataStore* msg, bool full) override;
    bool Handle();
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientMoveSetCanTransitionBetweenSwimAndFly : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientMoveSetCanTransitionBetweenSwimAndFly* msg);

    /* 0010 */ WOWGUID m_guid;
    /* 0018 */ uint32_t m_ackIndex;

    ClientMoveSetCanTransitionBetweenSwimAndFly(WOWGUID guid, uint32_t ackIndex);
    ClientMoveSetCanTransitionBetweenSwimAndFly(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientMoveSetCanTransitionBetweenSwimAndFly();
    bool Get(CDataStore* msg, bool full) override;
    bool Handle();
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientMoveSetCollisionHeight : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientMoveSetCollisionHeight* msg);

    /* 0010 */ WOWGUID m_guid;
    /* 0018 */ float m_height;
    /* 001C */ uint32_t m_ackIndex;
    /* 0020 */ uint32_t m_reason;

    ClientMoveSetCollisionHeight(WOWGUID guid, uint32_t ackIndex, float height, uint32_t reason);
    ClientMoveSetCollisionHeight(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientMoveSetCollisionHeight();
    bool Get(CDataStore* msg, bool full) override;
    bool Handle();
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientMoveSetFeatherFall : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientMoveSetFeatherFall* msg);

    /* 0010 */ WOWGUID m_guid;
    /* 0018 */ uint32_t m_ackIndex;

    ClientMoveSetFeatherFall(WOWGUID guid, uint32_t ackIndex);
    ClientMoveSetFeatherFall(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientMoveSetFeatherFall();
    bool Get(CDataStore* msg, bool full) override;
    bool Handle();
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientMoveSetFlightBackSpeed : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientMoveSetFlightBackSpeed* msg);

    /* 0010 */ WOWGUID m_guid;
    /* 0018 */ uint32_t m_ackIndex;
    /* 001C */ float m_speed;

    ClientMoveSetFlightBackSpeed(WOWGUID guid, uint32_t ackIndex, float speed);
    ClientMoveSetFlightBackSpeed(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientMoveSetFlightBackSpeed();
    bool Get(CDataStore* msg, bool full) override;
    bool Handle();
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientMoveSetFlightSpeed : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientMoveSetFlightSpeed* msg);

    /* 0010 */ WOWGUID m_guid;
    /* 0018 */ uint32_t m_ackIndex;
    /* 001C */ float m_speed;

    ClientMoveSetFlightSpeed(WOWGUID guid, uint32_t ackIndex, float speed);
    ClientMoveSetFlightSpeed(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientMoveSetFlightSpeed();
    bool Get(CDataStore* msg, bool full) override;
    bool Handle();
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientMoveSetHover : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientMoveSetHover* msg);

    /* 0010 */ WOWGUID m_guid;
    /* 0018 */ uint32_t m_ackIndex;

    ClientMoveSetHover(WOWGUID guid, uint32_t ackIndex);
    ClientMoveSetHover(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientMoveSetHover();
    bool Get(CDataStore* msg, bool full) override;
    bool Handle();
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientMoveSetLandWalk : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientMoveSetLandWalk* msg);

    /* 0010 */ WOWGUID m_guid;
    /* 0018 */ uint32_t m_ackIndex;

    ClientMoveSetLandWalk(WOWGUID guid, uint32_t ackIndex);
    ClientMoveSetLandWalk(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientMoveSetLandWalk();
    bool Get(CDataStore* msg, bool full) override;
    bool Handle();
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientMoveSetNormalFall : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientMoveSetNormalFall* msg);

    /* 0010 */ WOWGUID m_guid;
    /* 0018 */ uint32_t m_ackIndex;

    ClientMoveSetNormalFall(WOWGUID guid, uint32_t ackIndex);
    ClientMoveSetNormalFall(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientMoveSetNormalFall();
    bool Get(CDataStore* msg, bool full) override;
    bool Handle();
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientMoveSetPitchRate : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientMoveSetPitchRate* msg);

    /* 0010 */ WOWGUID m_guid;
    /* 0018 */ float m_rate;
    /* 001C */ uint32_t m_ackIndex;

    ClientMoveSetPitchRate(WOWGUID guid, uint32_t ackIndex, float rate);
    ClientMoveSetPitchRate(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientMoveSetPitchRate();
    bool Get(CDataStore* msg, bool full) override;
    bool Handle();
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientMoveSetRunBackSpeed : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientMoveSetRunBackSpeed* msg);

    /* 0010 */ WOWGUID m_guid;
    /* 0018 */ float m_speed;
    /* 001C */ uint32_t m_ackIndex;

    ClientMoveSetRunBackSpeed(WOWGUID guid, uint32_t ackIndex, float speed);
    ClientMoveSetRunBackSpeed(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientMoveSetRunBackSpeed();
    bool Get(CDataStore* msg, bool full) override;
    bool Handle();
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientMoveSetRunSpeed : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientMoveSetRunSpeed* msg);

    /* 0010 */ WOWGUID m_guid;
    /* 0018 */ uint32_t m_ackIndex;
    /* 001C */ float m_speed;

    ClientMoveSetRunSpeed(WOWGUID guid, uint32_t ackIndex, float speed);
    ClientMoveSetRunSpeed(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientMoveSetRunSpeed();
    bool Get(CDataStore* msg, bool full) override;
    bool Handle();
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientMoveSetSwimBackSpeed : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientMoveSetSwimBackSpeed* msg);

    /* 0010 */ WOWGUID m_guid;
    /* 0018 */ float m_speed;
    /* 001C */ uint32_t m_ackIndex;

    ClientMoveSetSwimBackSpeed(WOWGUID guid, uint32_t ackIndex, float speed);
    ClientMoveSetSwimBackSpeed(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientMoveSetSwimBackSpeed();
    bool Get(CDataStore* msg, bool full) override;
    bool Handle();
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientMoveSetSwimSpeed : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientMoveSetSwimSpeed* msg);

    /* 0010 */ WOWGUID m_guid;
    /* 0018 */ float m_speed;
    /* 001C */ uint32_t m_ackIndex;

    ClientMoveSetSwimSpeed(WOWGUID guid, uint32_t ackIndex, float speed);
    ClientMoveSetSwimSpeed(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientMoveSetSwimSpeed();
    bool Get(CDataStore* msg, bool full) override;
    bool Handle();
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientMoveSetTurnRate : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientMoveSetTurnRate* msg);

    /* 0010 */ WOWGUID m_guid;
    /* 0018 */ uint32_t m_ackIndex;
    /* 001C */ float m_rate;

    ClientMoveSetTurnRate(WOWGUID guid, uint32_t ackIndex, float rate);
    ClientMoveSetTurnRate(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientMoveSetTurnRate();
    bool Get(CDataStore* msg, bool full) override;
    bool Handle();
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientMoveSetVehicleRecId : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientMoveSetVehicleRecId* msg);

    /* 0010 */ WOWGUID m_guid;
    /* 0018 */ uint32_t m_ackIndex;
    /* 001C */ uint32_t m_vehicleRecId;

    ClientMoveSetVehicleRecId(WOWGUID guid, uint32_t ackIndex, uint32_t vehicleRecId);
    ClientMoveSetVehicleRecId(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientMoveSetVehicleRecId();
    bool Get(CDataStore* msg, bool full) override;
    bool Handle();
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientMoveSetWalkSpeed : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientMoveSetWalkSpeed* msg);

    /* 0010 */ WOWGUID m_guid;
    /* 0018 */ uint32_t m_ackIndex;
    /* 001C */ float m_speed;

    ClientMoveSetWalkSpeed(WOWGUID guid, uint32_t ackIndex, float speed);
    ClientMoveSetWalkSpeed(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientMoveSetWalkSpeed();
    bool Get(CDataStore* msg, bool full) override;
    bool Handle();
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientMoveSetWaterWalk : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientMoveSetWaterWalk* msg);

    /* 0010 */ WOWGUID m_guid;
    /* 0018 */ uint32_t m_ackIndex;

    ClientMoveSetWaterWalk(WOWGUID guid, uint32_t ackIndex);
    ClientMoveSetWaterWalk(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientMoveSetWaterWalk();
    bool Get(CDataStore* msg, bool full) override;
    bool Handle();
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientMoveSkipTime : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientMoveSkipTime* msg);

    /* 0010 */ WOWGUID m_guid;
    /* 0018 */ uint32_t m_time;

    ClientMoveSkipTime(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientMoveSkipTime();
    bool Get(CDataStore* msg, bool full) override;
    bool Handle();
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientMoveTeleport : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientMoveTeleport* msg);

    /* 0010 */ C3Vector m_position;
    /* 001C */ float m_facing;
    /* 0020 */ bool m_hasTransport;
    /* 0028 */ WOWGUID m_transportGuid;
    /* 0030 */ bool m_hasVehicle;
    /* 0031 */ uint8_t m_seat;
    /* 0032 */ bool m_exitVoluntary;
    /* 0033 */ bool m_awaitTransition;
    /* 0034 */ uint32_t m_ackIndex;
    /* 0038 */ WOWGUID m_guid;

    ClientMoveTeleport(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientMoveTeleport();
    bool Get(CDataStore* msg, bool full) override;
    bool Handle();
    bool IsValid() const;
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientMoveUnroot : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientMoveUnroot* msg);

    /* 0010 */ WOWGUID m_guid;
    /* 0018 */ uint32_t m_ackIndex;

    ClientMoveUnroot(WOWGUID guid, uint32_t ackIndex);
    ClientMoveUnroot(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientMoveUnroot();
    bool Get(CDataStore* msg, bool full) override;
    bool Handle();
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientMoveUnsetCanFly : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientMoveUnsetCanFly* msg);

    /* 0010 */ WOWGUID m_guid;
    /* 0018 */ uint32_t m_ackIndex;

    ClientMoveUnsetCanFly(WOWGUID guid, uint32_t ackIndex);
    ClientMoveUnsetCanFly(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientMoveUnsetCanFly();
    bool Get(CDataStore* msg, bool full) override;
    bool Handle();
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientMoveUnsetCanTransitionBetweenSwimAndFly : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientMoveUnsetCanTransitionBetweenSwimAndFly* msg);

    /* 0010 */ WOWGUID m_guid;
    /* 0018 */ uint32_t m_ackIndex;

    ClientMoveUnsetCanTransitionBetweenSwimAndFly(WOWGUID guid, uint32_t ackIndex);
    ClientMoveUnsetCanTransitionBetweenSwimAndFly(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientMoveUnsetCanTransitionBetweenSwimAndFly();
    bool Get(CDataStore* msg, bool full) override;
    bool Handle();
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientMoveUnsetHover : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientMoveUnsetHover* msg);

    /* 0010 */ WOWGUID m_guid;
    /* 0018 */ uint32_t m_ackIndex;

    ClientMoveUnsetHover(WOWGUID guid, uint32_t ackIndex);
    ClientMoveUnsetHover(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientMoveUnsetHover();
    bool Get(CDataStore* msg, bool full) override;
    bool Handle();
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientMoveUpdate : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientMoveUpdate* msg);

    /* 0010 */ CMovementStatus m_status;

    ClientMoveUpdate(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientMoveUpdate();
    bool Get(CDataStore* msg, bool full) override;
    bool Handle();
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientMoveUpdateCollisionHeight : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientMoveUpdateCollisionHeight* msg);

    /* 0010 */ float m_height;
    /* 0018 */ CMovementStatus m_status;

    ClientMoveUpdateCollisionHeight(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientMoveUpdateCollisionHeight();
    bool Get(CDataStore* msg, bool full) override;
    bool Handle();
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientMoveUpdateFlightBackSpeed : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientMoveUpdateFlightBackSpeed* msg);

    /* 0010 */ CMovementStatus m_status;
    /* 0098 */ float m_speed;

    ClientMoveUpdateFlightBackSpeed(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientMoveUpdateFlightBackSpeed();
    bool Get(CDataStore* msg, bool full) override;
    bool Handle();
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientMoveUpdateFlightSpeed : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientMoveUpdateFlightSpeed* msg);

    /* 0010 */ CMovementStatus m_status;
    /* 0098 */ float m_speed;

    ClientMoveUpdateFlightSpeed(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientMoveUpdateFlightSpeed();
    bool Get(CDataStore* msg, bool full) override;
    bool Handle();
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientMoveUpdateKnockBack : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientMoveUpdateKnockBack* msg);

    /* 0010 */ CMovementStatus m_status;

    ClientMoveUpdateKnockBack(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientMoveUpdateKnockBack();
    bool Get(CDataStore* msg, bool full) override;
    bool Handle();
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientMoveUpdatePitchRate : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientMoveUpdatePitchRate* msg);

    /* 0010 */ float m_rate;
    /* 0018 */ CMovementStatus m_status;

    ClientMoveUpdatePitchRate(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientMoveUpdatePitchRate();
    bool Get(CDataStore* msg, bool full) override;
    bool Handle();
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientMoveUpdateRunBackSpeed : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientMoveUpdateRunBackSpeed* msg);

    /* 0010 */ float m_speed;
    /* 0018 */ CMovementStatus m_status;

    ClientMoveUpdateRunBackSpeed(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientMoveUpdateRunBackSpeed();
    bool Get(CDataStore* msg, bool full) override;
    bool Handle();
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientMoveUpdateRunSpeed : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientMoveUpdateRunSpeed* msg);

    /* 0010 */ CMovementStatus m_status;
    /* 0098 */ float m_speed;

    ClientMoveUpdateRunSpeed(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientMoveUpdateRunSpeed();
    bool Get(CDataStore* msg, bool full) override;
    bool Handle();
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientMoveUpdateSwimBackSpeed : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientMoveUpdateSwimBackSpeed* msg);

    /* 0010 */ CMovementStatus m_status;
    /* 0098 */ float m_speed;

    ClientMoveUpdateSwimBackSpeed(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientMoveUpdateSwimBackSpeed();
    bool Get(CDataStore* msg, bool full) override;
    bool Handle();
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientMoveUpdateSwimSpeed : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientMoveUpdateSwimSpeed* msg);

    /* 0010 */ CMovementStatus m_status;
    /* 0098 */ float m_speed;

    ClientMoveUpdateSwimSpeed(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientMoveUpdateSwimSpeed();
    bool Get(CDataStore* msg, bool full) override;
    bool Handle();
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientMoveUpdateTeleport : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientMoveUpdateTeleport* msg);

    /* 0010 */ CMovementStatus m_status;

    ClientMoveUpdateTeleport(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientMoveUpdateTeleport();
    bool Get(CDataStore* msg, bool full) override;
    bool Handle();
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientMoveUpdateTurnRate : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientMoveUpdateTurnRate* msg);

    /* 0010 */ float m_rate;
    /* 0018 */ CMovementStatus m_status;

    ClientMoveUpdateTurnRate(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientMoveUpdateTurnRate();
    bool Get(CDataStore* msg, bool full) override;
    bool Handle();
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientMoveUpdateWalkSpeed : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientMoveUpdateWalkSpeed* msg);

    /* 0010 */ float m_speed;
    /* 0018 */ CMovementStatus m_status;

    ClientMoveUpdateWalkSpeed(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientMoveUpdateWalkSpeed();
    bool Get(CDataStore* msg, bool full) override;
    bool Handle();
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientPlaySpellVisual : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientPlaySpellVisual* msg);

    /* 0010 */ WOWGUID m_guid;
    /* 0018 */ float m_travelSpeed;
    /* 0020 */ WOWGUID m_targetGuid;
    /* 0028 */ uint32_t m_spellVisualId;
    /* 002C */ C3Vector m_targetPosition;
    /* 0038 */ bool m_speedAsTime;
    /* 003A */ uint16_t m_missReason;
    /* 003C */ uint16_t m_reflectStatus;

    ClientPlaySpellVisual(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientPlaySpellVisual();
    bool Get(CDataStore* msg, bool full) override;
    bool Handle();
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientSetPlayHoverAnim : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientSetPlayHoverAnim* msg);

    /* 0010 */ WOWGUID m_guid;
    /* 0018 */ bool m_playHoverAnim;

    ClientSetPlayHoverAnim(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientSetPlayHoverAnim();
    bool Get(CDataStore* msg, bool full) override;
    bool Handle();
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientSplineMoveCollisionDisable : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientSplineMoveCollisionDisable* msg);

    /* 0010 */ WOWGUID m_guid;

    ClientSplineMoveCollisionDisable(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientSplineMoveCollisionDisable();
    bool Get(CDataStore* msg, bool full) override;
    bool Handle();
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientSplineMoveCollisionEnable : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientSplineMoveCollisionEnable* msg);

    /* 0010 */ WOWGUID m_guid;

    ClientSplineMoveCollisionEnable(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientSplineMoveCollisionEnable();
    bool Get(CDataStore* msg, bool full) override;
    bool Handle();
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientSplineMoveGravityDisable : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientSplineMoveGravityDisable* msg);

    /* 0010 */ WOWGUID m_guid;

    ClientSplineMoveGravityDisable(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientSplineMoveGravityDisable();
    bool Get(CDataStore* msg, bool full) override;
    bool Handle();
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientSplineMoveGravityEnable : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientSplineMoveGravityEnable* msg);

    /* 0010 */ WOWGUID m_guid;

    ClientSplineMoveGravityEnable(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientSplineMoveGravityEnable();
    bool Get(CDataStore* msg, bool full) override;
    bool Handle();
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientSplineMoveRoot : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientSplineMoveRoot* msg);

    /* 0010 */ WOWGUID m_guid;

    ClientSplineMoveRoot(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientSplineMoveRoot();
    bool Get(CDataStore* msg, bool full) override;
    bool Handle();
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientSplineMoveSetFeatherFall : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientSplineMoveSetFeatherFall* msg);

    /* 0010 */ WOWGUID m_guid;

    ClientSplineMoveSetFeatherFall(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientSplineMoveSetFeatherFall();
    bool Get(CDataStore* msg, bool full) override;
    bool Handle();
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientSplineMoveSetFlightBackSpeed : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientSplineMoveSetFlightBackSpeed* msg);

    /* 0010 */ WOWGUID m_guid;
    /* 0018 */ float m_speed;

    ClientSplineMoveSetFlightBackSpeed(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientSplineMoveSetFlightBackSpeed();
    bool Get(CDataStore* msg, bool full) override;
    bool Handle();
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientSplineMoveSetFlightSpeed : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientSplineMoveSetFlightSpeed* msg);

    /* 0010 */ WOWGUID m_guid;
    /* 0018 */ float m_speed;

    ClientSplineMoveSetFlightSpeed(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientSplineMoveSetFlightSpeed();
    bool Get(CDataStore* msg, bool full) override;
    bool Handle();
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientSplineMoveSetFlying : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientSplineMoveSetFlying* msg);

    /* 0010 */ WOWGUID m_guid;

    ClientSplineMoveSetFlying(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientSplineMoveSetFlying();
    bool Get(CDataStore* msg, bool full) override;
    bool Handle();
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientSplineMoveSetHover : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientSplineMoveSetHover* msg);

    /* 0010 */ WOWGUID m_guid;

    ClientSplineMoveSetHover(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientSplineMoveSetHover();
    bool Get(CDataStore* msg, bool full) override;
    bool Handle();
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientSplineMoveSetLandWalk : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientSplineMoveSetLandWalk* msg);

    /* 0010 */ WOWGUID m_guid;

    ClientSplineMoveSetLandWalk(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientSplineMoveSetLandWalk();
    bool Get(CDataStore* msg, bool full) override;
    bool Handle();
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientSplineMoveSetNormalFall : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientSplineMoveSetNormalFall* msg);

    /* 0010 */ WOWGUID m_guid;

    ClientSplineMoveSetNormalFall(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientSplineMoveSetNormalFall();
    bool Get(CDataStore* msg, bool full) override;
    bool Handle();
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientSplineMoveSetPitchRate : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientSplineMoveSetPitchRate* msg);

    /* 0010 */ WOWGUID m_guid;
    /* 0018 */ float m_rate;

    ClientSplineMoveSetPitchRate(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientSplineMoveSetPitchRate();
    bool Get(CDataStore* msg, bool full) override;
    bool Handle();
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientSplineMoveSetRunBackSpeed : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientSplineMoveSetRunBackSpeed* msg);

    /* 0010 */ WOWGUID m_guid;
    /* 0018 */ float m_speed;

    ClientSplineMoveSetRunBackSpeed(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientSplineMoveSetRunBackSpeed();
    bool Get(CDataStore* msg, bool full) override;
    bool Handle();
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientSplineMoveSetRunMode : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientSplineMoveSetRunMode* msg);

    /* 0010 */ WOWGUID m_guid;

    ClientSplineMoveSetRunMode(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientSplineMoveSetRunMode();
    bool Get(CDataStore* msg, bool full) override;
    bool Handle();
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientSplineMoveSetRunSpeed : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientSplineMoveSetRunSpeed* msg);

    /* 0010 */ WOWGUID m_guid;
    /* 0018 */ float m_speed;

    ClientSplineMoveSetRunSpeed(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientSplineMoveSetRunSpeed();
    bool Get(CDataStore* msg, bool full) override;
    bool Handle();
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientSplineMoveSetSwimBackSpeed : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientSplineMoveSetSwimBackSpeed* msg);

    /* 0010 */ WOWGUID m_guid;
    /* 0018 */ float m_speed;

    ClientSplineMoveSetSwimBackSpeed(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientSplineMoveSetSwimBackSpeed();
    bool Get(CDataStore* msg, bool full) override;
    bool Handle();
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientSplineMoveSetSwimSpeed : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientSplineMoveSetSwimSpeed* msg);

    /* 0010 */ WOWGUID m_guid;
    /* 0018 */ float m_speed;

    ClientSplineMoveSetSwimSpeed(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientSplineMoveSetSwimSpeed();
    bool Get(CDataStore* msg, bool full) override;
    bool Handle();
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientSplineMoveSetTurnRate : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientSplineMoveSetTurnRate* msg);

    /* 0010 */ WOWGUID m_guid;
    /* 0018 */ float m_rate;

    ClientSplineMoveSetTurnRate(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientSplineMoveSetTurnRate();
    bool Get(CDataStore* msg, bool full) override;
    bool Handle();
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientSplineMoveSetWalkMode : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientSplineMoveSetWalkMode* msg);

    /* 0010 */ WOWGUID m_guid;

    ClientSplineMoveSetWalkMode(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientSplineMoveSetWalkMode();
    bool Get(CDataStore* msg, bool full) override;
    bool Handle();
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientSplineMoveSetWalkSpeed : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientSplineMoveSetWalkSpeed* msg);

    /* 0010 */ WOWGUID m_guid;
    /* 0018 */ float m_speed;

    ClientSplineMoveSetWalkSpeed(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientSplineMoveSetWalkSpeed();
    bool Get(CDataStore* msg, bool full) override;
    bool Handle();
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientSplineMoveSetWaterWalk : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientSplineMoveSetWaterWalk* msg);

    /* 0010 */ WOWGUID m_guid;

    ClientSplineMoveSetWaterWalk(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientSplineMoveSetWaterWalk();
    bool Get(CDataStore* msg, bool full) override;
    bool Handle();
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientSplineMoveStartSwim : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientSplineMoveStartSwim* msg);

    /* 0010 */ WOWGUID m_guid;

    ClientSplineMoveStartSwim(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientSplineMoveStartSwim();
    bool Get(CDataStore* msg, bool full) override;
    bool Handle();
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientSplineMoveStopSwim : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientSplineMoveStopSwim* msg);

    /* 0010 */ WOWGUID m_guid;

    ClientSplineMoveStopSwim(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientSplineMoveStopSwim();
    bool Get(CDataStore* msg, bool full) override;
    bool Handle();
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientSplineMoveUnroot : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientSplineMoveUnroot* msg);

    /* 0010 */ WOWGUID m_guid;

    ClientSplineMoveUnroot(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientSplineMoveUnroot();
    bool Get(CDataStore* msg, bool full) override;
    bool Handle();
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientSplineMoveUnsetFlying : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientSplineMoveUnsetFlying* msg);

    /* 0010 */ WOWGUID m_guid;

    ClientSplineMoveUnsetFlying(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientSplineMoveUnsetFlying();
    bool Get(CDataStore* msg, bool full) override;
    bool Handle();
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientSplineMoveUnsetHover : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientSplineMoveUnsetHover* msg);

    /* 0010 */ WOWGUID m_guid;

    ClientSplineMoveUnsetHover(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientSplineMoveUnsetHover();
    bool Get(CDataStore* msg, bool full) override;
    bool Handle();
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

class ClientSuppressNPCGreetings : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientSuppressNPCGreetings* msg);

    /* 0010 */ WOWGUID m_guid;
    /* 0018 */ bool m_suppressed;

    ClientSuppressNPCGreetings(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientSuppressNPCGreetings();
    bool Get(CDataStore* msg, bool full) override;
    bool Handle();
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};
struct JamMoveKnockBack {
    /* 0000 */ float m_horizontalSpeed;
    /* 0004 */ C2Vector m_direction;
    /* 000C */ float m_verticalSpeed;

    bool IsValid() const;
};

struct JamMoveCollisionHeight {
    /* 0000 */ float m_height;
    /* 0004 */ uint32_t m_reason;
};

struct JamMoveStateChange {
    /* 0000 */ uint16_t m_messageId;
    /* 0004 */ uint32_t m_ackIndex;
    /* 0008 */ bool m_hasSpeed;
    /* 000C */ float m_speed;
    /* 0010 */ bool m_hasKnockBack;
    /* 0014 */ JamMoveKnockBack m_knockBack;
    /* 0024 */ bool m_hasVehicleRecId;
    /* 0028 */ uint32_t m_vehicleRecId;
    /* 002C */ bool m_hasCollisionHeight;
    /* 0030 */ JamMoveCollisionHeight m_collisionHeight;
};

class ClientMoveSetCompoundState : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientMoveSetCompoundState* msg);

    /* 0010 */ WOWGUID m_guid;
    /* 0018 */ TSGrowableArray<JamMoveStateChange> m_changes;

    ClientMoveSetCompoundState(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientMoveSetCompoundState();
    bool Get(CDataStore* msg, bool full) override;
    bool Handle();
    bool IsValid();
    bool Read(CDataStore* msg, bool full);
    void Unpack(CDataStore* msg, bool full);
};

#endif

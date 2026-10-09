#ifndef WOW_SOURCE_OBJECTMGRCLIENT_CCLIENTOBJCREATE_H
#define WOW_SOURCE_OBJECTMGRCLIENT_CCLIENTOBJCREATE_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Storm/H/stpl.h"
#include "Engine/Source/Tempest/C3Vector.h"
#include "WoW/Source/Object/CClientMoveUpdate.h"
#include "WoW/Source/Object/WGUID.h"

class CDataStore;

class CClientObjCreate {
    public:
    /* 0000 */ bool m_isActivePlayer;
    /* 0001 */ bool unk_0001;
    /* 0002 */ bool unk_0002;
    /* 0003 */ bool unk_0003;
    /* 0004 */ bool unk_0004;
    /* 0005 */ uint8_t unk_0005[3];
    /* 0008 */ bool m_hasMovement;
    /* 0010 */ CClientMoveUpdate m_move;
    /* 0138 */ bool m_hasGOTransport;
    /* 0140 */ CMovementStatus::TransportInfo m_goTransport;
    /* 0170 */ bool m_hasStationaryPosition;
    /* 0174 */ C3Vector m_stationaryPosition;
    /* 0180 */ float m_stationaryFacing;
    /* 0184 */ uint32_t unk_0184;
    /* 0188 */ bool m_hasTarget;
    /* 0190 */ WOWGUID m_target;
    /* 0198 */ bool m_hasTransportTime;
    /* 019C */ uint32_t m_transportTime;
    /* 01A0 */ bool m_hasVehicle;
    /* 01A4 */ uint32_t m_vehicleId;
    /* 01A8 */ float m_vehicleOrientation;
    /* 01AC */ bool m_hasAnimKits;
    /* 01AE */ uint16_t m_animKits[3];
    /* 01B4 */ uint32_t unk_01B4;
    /* 01B8 */ bool m_hasRotation;
    /* 01C0 */ uint64_t m_packedRotation;
    /* 01C8 */ bool m_hasAreaTrigger;
    /* 01CC */ float m_areaTrigger[16];
    /* 020C */ uint8_t unk_020C;
    /* 0210 */ TSGrowableArray<uint32_t> m_stopFrames;

    CClientObjCreate();
    ~CClientObjCreate();
    void Get(CDataStore* msg, bool a2);
    bool IsValid() const;
};

#endif

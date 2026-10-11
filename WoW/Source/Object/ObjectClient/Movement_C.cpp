#include "Movement_C.h"
#include "Engine/Source/Os/TimeManager.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/Tempest/C44Matrix.h"
#include "Engine/Source/Tempest/C4Quaternion.h"
#include "WoW/Source/Object/CMovementGlobals.h"
#include "WoW/Source/Object/CMoveSpline.h"
#include "WoW/Source/Object/Movement.h"
#include "WoW/Source/Object/ObjectClient/GameObject_C.h"
#include "WoW/Source/Object/ObjectClient/Unit_C.h"
#include "WoW/Source/Object/ObjectClient/Vehicle_C.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/Ui/WorldFrame.h"
#include "WoW/Source/UIUtil/Camera.h"
#include "WoW/Source/WorldClient/World.h"

// STATIC: 0x00CA7790
static TSExplicitList<CGGameObject_C, offsetof(CGGameObject_C, m_transportLink)> s_transports;

// OFFSET: 0x00571920/0x1401D2B80
uint32_t MovementGetIdleTime() {
    return MovementGetGlobals()->m_idleTime;
}

// OFFSET: 0x005E7000
void MovementAddTransport(CGGameObject_C* transport) {
    s_transports.LinkToTail(transport);
}

// OFFSET: 0x005E6BA0/0x140267500
int32_t MovementAddToTransport(CPassenger* passenger, WOWGUID transport, int32_t a3, int32_t reason, int32_t vehicleId) {
    CGObject_C* object = ClntObjMgrObjectPtr(transport, TYPEMASK_OBJECT, __FILE__, __LINE__);

    if (!object) {
        return 0;
    }

    uint32_t high = static_cast<uint32_t>(transport.m_guid >> 32);

    if (((high & 0xF0F00000) == 0xF0500000 || (!(high & 0xF0000000) && ((high & 0xF07FFFFF) | static_cast<uint32_t>(transport.m_guid)))) && vehicleId && !static_cast<CGUnit_C*>(object)->m_vehicle) {
        static_cast<CGUnit_C*>(object)->CreateVehicleData(NULL, vehicleId);
    }

    return object->AddPassenger(passenger, a3);
}

// PROVEN: 8e5bc963 IDENTICAL/NONE
// OFFSET: 0x005E6B90/0x1402674F0
CMovementGlobals* MovementGetGlobals() {
    return ClntObjMgrGetMovementGlobals();
}

// PROVEN: fbafa5c1 IDENTICAL/NONE
// OFFSET: 0x005E6B80/0x1402674E0
void MovementSetGlobals(CMovementGlobals* globals) {
    ClntObjMgrSetMovementGlobals(globals);
}

// OFFSET: 0x005E6E60/0x140267860
float MovementGetTransportFacing(WOWGUID transport) {
    CGObject_C* object = ClntObjMgrObjectPtr(transport, TYPEMASK_OBJECT, __FILE__, __LINE__);

    if (!object) {
        DebugLog("Failed to dereference transport! Provided GUID 0x%016I64X", transport);
        return 0.0f;
    }

    return object->GetFacing();
}

// PROVEN: c9e9b163 IDENTICAL/NONE
// OFFSET: 0x005E6CD0/0x140267680
int32_t MovementGetTransportMtxSafe(WOWGUID transport, C44Matrix* matrix) {
    CGObject_C* object = ClntObjMgrObjectPtr(transport, TYPEMASK_OBJECT, __FILE__, __LINE__);

    if (object) {
        *matrix = object->GetMatrix();
        return 1;
    }

    matrix->d3 = 1.0f;
    matrix->c2 = 1.0f;
    matrix->b1 = 1.0f;
    matrix->a0 = 1.0f;
    matrix->d2 = 0.0f;
    matrix->d1 = 0.0f;
    matrix->d0 = 0.0f;
    matrix->c3 = 0.0f;
    matrix->c1 = 0.0f;
    matrix->c0 = 0.0f;
    matrix->b3 = 0.0f;
    matrix->b2 = 0.0f;
    matrix->b0 = 0.0f;
    matrix->a3 = 0.0f;
    matrix->a2 = 0.0f;
    matrix->a1 = 0.0f;
    return 0;
}

// OFFSET: 0x005E6D80/0x140267780
int32_t MovementGetTransportMtxX(WOWGUID transport, C44Matrix* matrix, WOWGUID owner, const char* file, int32_t line) {
    if (MovementGetTransportMtxSafe(transport, matrix)) {
        return 1;
    }

    DebugLog("Failed to dereference transport! Provided GUID 0x%016I64X from %s(%d)", transport, file, line);
    return 0;
}

// OFFSET: 0x005E6DD0
void MovementGetTransportQuat(WOWGUID transport, C4Quaternion* quat) {
    CGObject_C* object = ClntObjMgrObjectPtr(transport, TYPEMASK_OBJECT, __FILE__, __LINE__);

    if (object) {
        *quat = object->GetRotation();
        return;
    }

    DebugLog("Failed to dereference transport! Provided GUID 0x%016I64X", transport);
    quat->x = 0.0f;
    quat->y = 0.0f;
    quat->z = 0.0f;
    quat->w = 1.0f;
}

// PROVEN: 3bb0d15a IDENTICAL/NONE
// OFFSET: 0x005E6EB0
int32_t MovementInsideTransport(WOWGUID transport, const C3Vector& point) {
    CGObject_C* object = ClntObjMgrObjectPtr(transport, TYPEMASK_OBJECT, __FILE__, __LINE__);

    if (!object) {
        return 0;
    }

    return object->IsPointInside(point);
}

// OFFSET: 0x005E6FB0/0x1402679D0
void MovementMoveTransports(uint32_t time, uint32_t elapsed) {
    for (CGGameObject_C* transport = s_transports.Head(); transport; transport = s_transports.Next(transport)) {
        transport->m_typeData->UpdateMovement(time, elapsed);
    }
}

// OFFSET: 0x005E6F40/0x140267950
void MovementNotifyTransportUnitRemoved(WOWGUID passenger, WOWGUID transport, uint8_t seat) {
    uint32_t high = (uint32_t)(transport.m_guid >> 32);

    if ((high & 0xF0F00000) == 0xF0500000 || (!(high & 0xF0000000) && ((high & 0xF07FFFFF) | (uint32_t)transport.m_guid))) {
        CGUnit_C* unit = (CGUnit_C*)ClntObjMgrObjectPtr(transport, TYPEMASK_UNIT, __FILE__, __LINE__);

        if (unit && unit->m_vehicle) {
            unit->m_vehicle->UpdateSeatVacancy();
        }
    }
}

// PROVEN: 06762f05 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void MovementNotifyZoneMgr(WOWGUID guid) {
}

// PROVEN: 21879ace IDENTICAL/NONE
// OFFSET: 0x005E6C90/0x140267640
int32_t MovementObjIsTransport(WOWGUID guid) {
    CGObject_C* object = ClntObjMgrObjectPtr(guid, TYPEMASK_OBJECT, __FILE__, __LINE__);

    if (!object) {
        return 0;
    }

    return object->IsTransport();
}

// OFFSET: 0x005E7020/0x140267AC0
void MovementRemoveTransport(CGGameObject_C* transport) {
    s_transports.UnlinkNode(transport);
}

// OFFSET: 0x005716B0/0x1401D29A0
int32_t MovementGetLastUpdateTime(uint32_t* time) {
    CMovementGlobals* globals = MovementGetGlobals();

    if (!globals || !(globals->m_flags & 0x1)) {
        return 0;
    }

    *time = globals->m_lastUpdateTime;

    return 1;
}

// OFFSET: 0x0057B270/0x1401E0DB0
int32_t MovementIdleMoveUnits(const void* data, void* param) {
    uint32_t now = OsGetAsyncTimeMs();
    CMovementGlobals* globals = MovementGetGlobals();
    int32_t elapsed = now - globals->m_lastUpdateTime;

    if (elapsed > 0) {
        MovementMoveTransports(now, elapsed);

        if (globals->m_movers.Head()) {
            CMovement_C::MoveUnits(now, globals->m_lastUpdateTime);
        }

        CGUnit_C* mover = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(CGUnit_C::s_activeMover, TYPEMASK_UNIT, __FILE__, __LINE__));

        if (mover) {
            mover->UpdateFloodsafeMoveEvents(now);
        }

        globals->m_idleTime = now;
        globals->m_lastUpdateTime = now;
    }

    return 1;
}

// OFFSET: 0x005E7190/0x140267C90
void MovementDelSpline(CMoveSpline* spline) {
    if (spline) {
        delete spline;
    }
}

// OFFSET: 0x005E6C30/0x1402675C0
void MovementUpdateCameraYaw(WOWGUID guid, WOWGUID transport) {
    uint32_t high = static_cast<uint32_t>(transport.m_guid >> 32);

    if ((high & 0xF0F00000) == 0xF0500000 || (!(high & 0xF0000000) && ((high & 0xF07FFFFF) | static_cast<uint32_t>(transport.m_guid)))) {
        return;
    }

    CGCamera* camera = CGWorldFrame::GetActiveCamera();

    if (camera->m_targetGUID == guid) {
        camera->MakeRelativeTo(transport);
    }
}

// OFFSET: 0x005E6EF0/0x1402678F0
void MovementUpdateWorld(WOWGUID guid, const C3Vector& position, int32_t preload) {
    CGCamera* camera = CGWorldFrame::GetActiveCamera();

    if (camera->m_targetGUID == guid) {
        if (preload) {
            World::Preload(position, 1);
        } else {
            World::UpdateAreaOfInterest(position, 0);
        }

        CGObject_C::UpdateAllWorldObjects();
    }
}

// OFFSET: 0x005E7160
CMoveSpline* MovementNewSpline() {
    return new (SMemAlloc(sizeof(CMoveSpline), __FILE__, __LINE__, 0)) CMoveSpline();
}

// OFFSET: 0x00571690/0x1401D2970
void MovementSetTransportUpdateTime(uint32_t time) {
    CMovementGlobals* globals = MovementGetGlobals();
    globals->m_transportTime2 = globals->m_transportTime;
    globals->m_transportTime = time;
}

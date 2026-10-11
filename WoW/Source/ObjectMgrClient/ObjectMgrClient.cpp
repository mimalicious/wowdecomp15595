#include <cstdint>
#include <cstring>
#include <ctime>
#include <new>
#include "ObjectMgrClient.h"
#include "Engine/Source/Base/CDataStore.h"
#include "Engine/Source/ConsoleBase/ConsoleCommand.h"
#include "Engine/Source/ConsoleBase/IConsole.h"
#include "Engine/Source/Event/Event.h"
#include "Engine/Source/Event/FrameTime.h"
#include "Engine/Source/Storm/H/storm.h"
#include "WoW/Source/Client.h"
#include "WoW/Source/JamClientAutoCode/JamPlayer.h"
#include "WoW/Source/Object/AreaTrigger.h"
#include "WoW/Source/Object/Container.h"
#include "WoW/Source/Object/Corpse.h"
#include "WoW/Source/Object/DynamicObject.h"
#include "WoW/Source/Object/GameObject.h"
#include "WoW/Source/Object/Item.h"
#include "WoW/Source/Object/ObjectClient/AreaTrigger_C.h"
#include "WoW/Source/Object/ObjectClient/Container_C.h"
#include "WoW/Source/Object/ObjectClient/Corpse_C.h"
#include "WoW/Source/Object/ObjectClient/DynamicObject_C.h"
#include "WoW/Source/Object/ObjectClient/GameObject_C.h"
#include "WoW/Source/Object/ObjectClient/Item_C.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/Object/ObjectClient/Unit_C.h"
#include "WoW/Source/Object/ObjectClient/VehiclePassenger_C.h"
#include "WoW/Source/Object/Player.h"
#include "WoW/Source/Object/Unit.h"
#include "Engine/Source/Base/ObjectAlloc.h"
#include "WoW/Source/ObjectMgrClient/CClientObjCreate.h"
#include "WoW/Source/ObjectMgrClient/Mirror.h"
#include "WoW/Source/Ui/BattlefieldInfo.h"
#include "WoW/Source/Ui/GameUI.h"
#include "WoW/Source/WowServices/RealmConnection.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"

// STATIC: 0x01014008
static __declspec(thread) ObjectMgr* s_curMgr;

// STATIC: 0x00D73658
static ObjectMgr* s_savedObjMgr;

// STATIC: 0x00D73660
static TSList<CMirrorHandler, TSGetLink<CMirrorHandler> > s_typeMirrorHandlers[NUM_CLIENT_OBJECT_TYPES][(sizeof(CGObjectData) + sizeof(CGUnitData) + sizeof(CGPlayerLocalData)) / sizeof(uint32_t)];

// STATIC: 0x00D73634
static uint32_t s_objHeapIds[NUM_CLIENT_OBJECT_TYPES];

// STATIC: 0x00D7365C
static bool s_heapsAllocated;

// STATIC: 0x00C9AC14
static const char* s_objNames[NUM_CLIENT_OBJECT_TYPES] = { "CGObject_C", "CGItem_C", "CGContainer_C", "CGUnit_C", "CGPlayer_C", "CGGameObject_C", "CGDynamicObject_C", "CGCorpse_C", "CGAreaTrigger_C" };

// STATIC: 0x00C9AC38
static const uint32_t s_objHeapSize[NUM_CLIENT_OBJECT_TYPES] = { 0, 512, 32, 64, 64, 64, 32, 32, 32 };

// STATIC: 0x00C9AC5C
static const uint32_t s_objDwordCount[NUM_CLIENT_OBJECT_TYPES] = {
    sizeof(CGObjectData) / sizeof(uint32_t),
    (sizeof(CGObjectData) + sizeof(CGItemData)) / sizeof(uint32_t),
    (sizeof(CGObjectData) + sizeof(CGItemData) + sizeof(CGContainerData)) / sizeof(uint32_t),
    (sizeof(CGObjectData) + sizeof(CGUnitData)) / sizeof(uint32_t),
    (sizeof(CGObjectData) + sizeof(CGUnitData) + sizeof(CGPlayerLocalData)) / sizeof(uint32_t),
    (sizeof(CGObjectData) + sizeof(CGGameObjectData)) / sizeof(uint32_t),
    (sizeof(CGObjectData) + sizeof(CGDynamicObjectData)) / sizeof(uint32_t),
    (sizeof(CGObjectData) + sizeof(CGCorpseData)) / sizeof(uint32_t),
    (sizeof(CGObjectData) + sizeof(CGAreaTriggerData)) / sizeof(uint32_t),
};

// STATIC: 0x00D97E40
uint32_t s_objTotalSize[NUM_CLIENT_OBJECT_TYPES] = {
    sizeof(CGObject_C) + sizeof(CGObjectData) + CGObject::TotalFieldsSaved() * sizeof(uint32_t),
    sizeof(CGItem_C) + sizeof(CGObjectData) + sizeof(CGItemData) + CGItem::TotalFieldsSaved() * sizeof(uint32_t),
    sizeof(CGContainer_C) + sizeof(CGObjectData) + sizeof(CGItemData) + sizeof(CGContainerData) + CGContainer::TotalFieldsSaved() * sizeof(uint32_t),
    sizeof(CGUnit_C) + sizeof(CGObjectData) + sizeof(CGUnitData) + CGUnit::TotalFields() * sizeof(uint32_t),
    sizeof(CGPlayer_C) + sizeof(CGObjectData) + sizeof(CGUnitData) + sizeof(CGPlayerData) + CGPlayer::TotalRemoteFields() * sizeof(uint32_t),
    sizeof(CGGameObject_C) + sizeof(CGObjectData) + sizeof(CGGameObjectData) + CGGameObject::TotalFieldsSaved() * sizeof(uint32_t),
    sizeof(CGDynamicObject_C) + sizeof(CGObjectData) + sizeof(CGDynamicObjectData) + CGDynamicObject::TotalFields() * sizeof(uint32_t),
    sizeof(CGCorpse_C) + sizeof(CGObjectData) + sizeof(CGCorpseData) + CGCorpse::TotalFieldsSaved() * sizeof(uint32_t),
    sizeof(CGAreaTrigger_C) + sizeof(CGObjectData) + sizeof(CGAreaTriggerData) + CGAreaTrigger::TotalFields() * sizeof(uint32_t),
};

// OFFSET: 0x0048F190/0x1400B37F0
static CMirrorHandler* AssignMirrorHandler(TSList<CMirrorHandler, TSGetLink<CMirrorHandler> >* list, uint32_t fieldByteOffset, uint32_t mirrorByteOffset, uint32_t fieldByteSize, MIRRORHANDLERFUNC function, void* param, HANDLER_PRIORITY priority, int32_t alwaysFire) {
    CMirrorHandler* handler = list->NewNode(2, 0, 0);
    handler->m_fieldByteOffset = fieldByteOffset;
    handler->m_mirrorByteOffset = mirrorByteOffset;
    handler->m_fieldByteSize = fieldByteSize;
    handler->m_function = function;
    handler->m_functionParam = param;
    handler->m_linkPositionSelector = priority;
    handler->m_dispatching = 0;
    handler->m_deletePending = 0;
    handler->m_alwaysFire = alwaysFire != 0;
    return handler;
}

// OFFSET: 0x0048D2D0
static uint32_t GetObjectTypeFieldByteOffset(OBJECT_TYPE_ID typeId) {
    switch (typeId) {
        case ID_ITEM:
        case ID_UNIT:
        case ID_GAMEOBJECT:
        case ID_DYNAMICOBJECT:
        case ID_CORPSE:
        case ID_AREATRIGGER: {
            return sizeof(CGObjectData);
        }

        case ID_CONTAINER: {
            return sizeof(CGObjectData) + sizeof(CGItemData);
        }

        case ID_PLAYER: {
            return sizeof(CGObjectData) + sizeof(CGUnitData);
        }

        default: {
            return 0;
        }
    }
}

// OFFSET: 0x0048DC00/0x1400B1C00
static uint32_t TypeDescriptorToMirrorOffset(OBJECT_TYPE_ID typeId, uint32_t offset, uint32_t size) {
    switch (typeId) {
        case ID_ITEM: {
            return CGItem::DescriptorToMirrorOffset(offset, size, 0, 0);
        }

        case ID_CONTAINER: {
            return CGContainer::DescriptorToMirrorOffset(offset, size, 0, 0);
        }

        case ID_UNIT: {
            return CGUnit::DescriptorToMirrorOffset(offset, size, 0, 0);
        }

        case ID_PLAYER: {
            return CGPlayer::DescriptorToMirrorOffset(offset, size, 0, 0);
        }

        case ID_GAMEOBJECT: {
            return CGGameObject::DescriptorToMirrorOffset(offset, size, 0, 0);
        }

        case ID_DYNAMICOBJECT: {
            return CGDynamicObject::DescriptorToMirrorOffset(offset, size, 0, 0);
        }

        case ID_CORPSE: {
            return CGCorpse::DescriptorToMirrorOffset(offset, size, 0, 0);
        }

        case ID_AREATRIGGER: {
            return CGAreaTrigger::DescriptorToMirrorOffset(offset, size, 0, 0);
        }

        default: {
            return (offset & 3) + 4 * CGObject::GetSavedIndexFromDataIndex(offset >> 2);
        }
    }
}

// OFFSET: 0x0048F240/0x1400B3900
static void UnassignMirrorHandler(TSList<CMirrorHandler, TSGetLink<CMirrorHandler> >* list, MIRRORHANDLERFUNC function, void* param) {
    for (CMirrorHandler* handler = list->Head(); handler; handler = list->Next(handler)) {
        if (handler->m_function == function && handler->m_functionParam == param) {
            if (handler->m_dispatching) {
                handler->m_deletePending = 1;
            } else {
                list->DeleteNode(handler);
            }

            return;
        }
    }
}

// PROVEN: f5d66880 IDENTICAL/NONE
// OFFSET: 0x0048D120/0x1400B0F80
void ClntObjMgrPush(ClntObjMgr* mgr) {
    if (s_savedObjMgr) {
        return;
    }

    ObjectMgr* current = s_curMgr;

    if (reinterpret_cast<ObjectMgr*>(mgr) != current) {
        s_savedObjMgr = current;
        s_curMgr = reinterpret_cast<ObjectMgr*>(mgr);
    }
}

// PROVEN: dc993463 IDENTICAL/NONE
// OFFSET: 0x0048D160/0x1400B0FC0
void ClntObjMgrPop() {
    if (s_savedObjMgr) {
        s_curMgr = s_savedObjMgr;
        s_savedObjMgr = NULL;
    }
}

// PROVEN: 675b198a IDENTICAL/NONE
// OFFSET: 0x0048D1B0/0x1400B1020
int32_t ClntObjMgrIsValid(int32_t a1) {
    ObjectMgr* mgr = s_curMgr;
    return mgr && SMemIsValidPointer(mgr, sizeof(ObjectMgr), a1);
}

// OFFSET: 0x0048D1F0/0x1400B1070
WOWGUID ClntObjMgrGetActivePlayer() {
    ObjectMgr* mgr = s_curMgr;

    if (!mgr) {
        return WOWGUID();
    }

    return mgr->m_playerGuid;
}

// OFFSET: 0x0048E5C0/0x1400B26D0
CGObject_C* ClntObjMgrObjectPtr(WOWGUID guid, TypeMask mask, const char* fileName, int32_t lineNumber) {
    ObjectMgr* mgr = s_curMgr;

    if (!mgr || !guid) {
        return NULL;
    }

    CGObject_C* object = mgr->m_objects.Ptr(static_cast<uint32_t>(guid.m_guid), CHashKeyGUID(guid.m_guid));

    if (object && !(object->m_obj->m_type & mask)) {
        return NULL;
    }

    return object;
}

// OFFSET: 0x0048E2C0/0x1400B2380
int32_t ClntObjMgrEnumVisibleObjects(OBJECTENUMFUNC func, void* param) {
    for (CGObject_C* object = s_curMgr->m_visibleObjects.Head(); object; object = s_curMgr->m_visibleObjects.Next(object)) {
        if (!func(object->m_key.m_guid, param)) {
            return 0;
        }
    }

    return 1;
}

// OFFSET: 0x0048E340/0x1400B2430
CGCorpse_C* ClntObjMgrFindCorpseByOwner(const WOWGUID& owner) {
    if (!s_curMgr) {
        return NULL;
    }

    for (CGObject_C* object = s_curMgr->m_visibleObjects.Head(), * next; object; object = next) {
        next = s_curMgr->m_visibleObjects.Next(object);

        if (object->m_obj->m_type & TYPEMASK_CORPSE) {
            CGCorpse_C* corpse = static_cast<CGCorpse_C*>(object);

            if (corpse->m_corpse->m_corpseFieldOwner == owner && !(corpse->m_corpse->m_corpseFieldFlags & 0x1)) {
                return corpse;
            }
        }
    }

    return NULL;
}

// PROVEN: 4ea93bf0 IDENTICAL/NONE
// OFFSET: 0x0048D190/0x1400B1000
ObjectMgr* ClntObjMgrGetCurrent() {
    return s_curMgr;
}

// OFFSET: 0x0048D240/0x1400B10D0
uint32_t ClntObjMgrGetMapID() {
    ObjectMgr* mgr = s_curMgr;

    if (!mgr) {
        return 0;
    }

    return mgr->m_mapId;
}

// PROVEN: f6be13da IDENTICAL/NONE
// OFFSET: 0x0048D270/0x1400B1100
CMovementGlobals* ClntObjMgrGetMovementGlobals() {
    ObjectMgr* mgr = s_curMgr;

    if (!mgr) {
        return NULL;
    }

    return mgr->m_movementGlobal;
}

// PROVEN: c765748d IDENTICAL/NONE
// OFFSET: 0x0048D220/0x1400B10A0
uint32_t ClntObjMgrGetPlayerType() {
    return s_curMgr->m_type;
}

// OFFSET: 0x0048E4B0
void ClntObjMgrHideObject(WOWGUID guid) {
    ObjectMgr* mgr = s_curMgr;
    CGObject_C* object = mgr->m_objects.Ptr(static_cast<uint32_t>(guid.m_guid), CHashKeyGUID(guid.m_guid));

    if (object && mgr->m_visibleObjects.IsLinked(object)) {
        mgr->m_visibleObjects.UnlinkNode(object);
    }
}

// OFFSET: 0x00490740
void ClntObjMgrLinkInNewObject(CGObject_C* object) {
    WOWGUID guid = object->m_obj->m_guid;
    s_curMgr->m_objects.Insert(object, static_cast<uint32_t>(guid.m_guid), CHashKeyGUID(guid.m_guid));
}

// OFFSET: 0x0048E570
void ClntObjMgrLockObject(WOWGUID guid) {
    CGObject_C* object = s_curMgr->m_objects.Ptr(static_cast<uint32_t>(guid.m_guid), CHashKeyGUID(guid.m_guid));

    if (object) {
        object->SetObjectLocked(1);
    }
}

// PROVEN: e8b58fac IDENTICAL/NONE
// OFFSET: 0x0048D2A0/0x1400B1130
void ClntObjMgrSetMovementGlobals(CMovementGlobals* globals) {
    ObjectMgr* mgr = s_curMgr;

    if (mgr) {
        mgr->m_movementGlobal = globals;
    }
}

// OFFSET: 0x0048F3C0/0x1400B3B40
void ClntObjMgrSetTypeMirrorHandler(OBJECT_TYPE_ID typeId, uint32_t fieldOffset, uint32_t fieldSize, MIRRORHANDLERFUNC function, void* param, HANDLER_PRIORITY priority, int32_t alwaysFire) {
    uint32_t mirrorOffset = TypeDescriptorToMirrorOffset(typeId, fieldOffset, fieldSize);
    AssignMirrorHandler(&s_typeMirrorHandlers[typeId][fieldOffset >> 2], fieldOffset + GetObjectTypeFieldByteOffset(typeId), mirrorOffset, fieldSize, function, param, priority, alwaysFire);
}

// OFFSET: 0x0048E510
void ClntObjMgrShowObject(WOWGUID guid) {
    ObjectMgr* mgr = s_curMgr;
    CGObject_C* object = mgr->m_objects.Ptr(static_cast<uint32_t>(guid.m_guid), CHashKeyGUID(guid.m_guid));

    if (object && !mgr->m_visibleObjects.IsLinked(object)) {
        mgr->m_visibleObjects.LinkToTail(object);
    }
}

// OFFSET: 0x0048F460
void ClntObjMgrUnsetTypeMirrorHandler(OBJECT_TYPE_ID typeId, uint32_t fieldOffset, MIRRORHANDLERFUNC function, void* param) {
    UnassignMirrorHandler(&s_typeMirrorHandlers[typeId][fieldOffset >> 2], function, param);
}

// OFFSET: 0x0048D320
static OBJECT_TYPE_ID IncTypeId(CGObject_C* object, OBJECT_TYPE_ID typeId) {
    switch (object->m_obj->m_type) {
        case TYPEMASK_OBJECT | TYPEMASK_ITEM:
        case TYPEMASK_OBJECT | TYPEMASK_ITEM | TYPEMASK_CONTAINER: {
            if (typeId == ID_OBJECT) {
                return ID_ITEM;
            }

            if (typeId == ID_ITEM) {
                return ID_CONTAINER;
            }

            break;
        }

        case TYPEMASK_OBJECT | TYPEMASK_UNIT:
        case TYPEMASK_OBJECT | TYPEMASK_UNIT | TYPEMASK_PLAYER: {
            if (typeId == ID_OBJECT) {
                return ID_UNIT;
            }

            if (typeId == ID_UNIT) {
                return ID_PLAYER;
            }

            break;
        }

        case TYPEMASK_OBJECT | TYPEMASK_GAMEOBJECT: {
            if (typeId == ID_OBJECT) {
                return ID_GAMEOBJECT;
            }

            break;
        }

        case TYPEMASK_OBJECT | TYPEMASK_DYNAMICOBJECT: {
            if (typeId == ID_OBJECT) {
                return ID_DYNAMICOBJECT;
            }

            break;
        }

        case TYPEMASK_OBJECT | TYPEMASK_CORPSE: {
            if (typeId == ID_OBJECT) {
                return ID_CORPSE;
            }

            break;
        }

        case TYPEMASK_OBJECT | TYPEMASK_AREATRIGGER: {
            if (typeId == ID_OBJECT) {
                return ID_AREATRIGGER;
            }

            break;
        }
    }

    return NUM_CLIENT_OBJECT_TYPES;
}

// OFFSET: 0x0048D4B0/0x1400B1360
static TSList<CMirrorHandler, TSGetLink<CMirrorHandler> >* GetObjectMirrorList(OBJECT_TYPE_ID typeId, CGObject_C* object, uint32_t index) {
    TSList<void>* list;

    switch (typeId) {
        case ID_OBJECT: {
            list = &object->m_objMirrorLists[index];
            break;
        }

        case ID_ITEM: {
            list = &static_cast<CGItem_C*>(object)->m_itemMirrorLists[index];
            break;
        }

        case ID_CONTAINER: {
            list = &static_cast<CGContainer_C*>(object)->m_containerMirrorLists[index];
            break;
        }

        case ID_UNIT: {
            list = &static_cast<CGUnit_C*>(object)->m_unitMirrorLists[index];
            break;
        }

        case ID_PLAYER: {
            CGPlayer_C* player = static_cast<CGPlayer_C*>(object);

            if (player->IsActivePlayer() && index >= sizeof(CGPlayerData) / sizeof(uint32_t)) {
                list = &player->m_playerLocalMirrorLists[index - sizeof(CGPlayerData) / sizeof(uint32_t)];
            } else {
                list = &player->m_playerMirrorLists[index];
            }

            break;
        }

        case ID_GAMEOBJECT: {
            list = &static_cast<CGGameObject_C*>(object)->m_gameObjectMirrorLists[index];
            break;
        }

        case ID_DYNAMICOBJECT: {
            list = &static_cast<CGDynamicObject_C*>(object)->m_dynamicObjectMirrorLists[index];
            break;
        }

        case ID_CORPSE: {
            list = &static_cast<CGCorpse_C*>(object)->m_corpseMirrorLists[index];
            break;
        }

        case ID_AREATRIGGER: {
            list = &static_cast<CGAreaTrigger_C*>(object)->m_areaTriggerMirrorLists[index];
            break;
        }

        default: {
            return NULL;
        }
    }

    return reinterpret_cast<TSList<CMirrorHandler, TSGetLink<CMirrorHandler> >*>(list);
}

// OFFSET: 0x0048D580
static uint32_t GetNumDwordBlocks(uint32_t type, WOWGUID guid) {
    switch (type) {
        case TYPEMASK_OBJECT: {
            return s_objDwordCount[ID_OBJECT];
        }

        case TYPEMASK_OBJECT | TYPEMASK_ITEM: {
            return s_objDwordCount[ID_ITEM];
        }

        case TYPEMASK_OBJECT | TYPEMASK_ITEM | TYPEMASK_CONTAINER: {
            return s_objDwordCount[ID_CONTAINER];
        }

        case TYPEMASK_OBJECT | TYPEMASK_UNIT: {
            return s_objDwordCount[ID_UNIT];
        }

        case TYPEMASK_OBJECT | TYPEMASK_UNIT | TYPEMASK_PLAYER: {
            return guid == ClntObjMgrGetActivePlayer() ? s_objDwordCount[ID_PLAYER] : (sizeof(CGObjectData) + sizeof(CGUnitData) + sizeof(CGPlayerData)) / sizeof(uint32_t);
        }

        case TYPEMASK_OBJECT | TYPEMASK_GAMEOBJECT: {
            return s_objDwordCount[ID_GAMEOBJECT];
        }

        case TYPEMASK_OBJECT | TYPEMASK_DYNAMICOBJECT: {
            return s_objDwordCount[ID_DYNAMICOBJECT];
        }

        case TYPEMASK_OBJECT | TYPEMASK_CORPSE: {
            return s_objDwordCount[ID_CORPSE];
        }

        case TYPEMASK_OBJECT | TYPEMASK_AREATRIGGER: {
            return s_objDwordCount[ID_AREATRIGGER];
        }

        default: {
            return 0;
        }
    }
}

// OFFSET: 0x0048D750
static int32_t ExtractDirtyMasks(CDataStore* msg, uint8_t* blockCount, uint32_t* masks) {
    uint8_t count;
    msg->Get(count);
    *blockCount = count;

    if (count > 44) {
        return 0;
    }

    int32_t i = 0;

    for (; i < count; i++) {
        msg->Get(masks[i]);
    }

    if (i < 44) {
        memset(&masks[i], 0, sizeof(uint32_t) * (44 - i));
    }

    return 1;
}

// OFFSET: 0x0048D7C0/0x1400B1520
static int32_t SkipPartialObjectUpdate(CDataStore* msg) {
    uint32_t masks[44];
    uint8_t blockCount;

    if (!ExtractDirtyMasks(msg, &blockCount, masks)) {
        return 0;
    }

    for (uint32_t i = 0; i < 32u * blockCount; i++) {
        if (masks[i >> 5] & (1 << (i & 31))) {
            uint32_t value;
            msg->Get(value);
        }
    }

    return 1;
}

// OFFSET: 0x0048D830
static void InitObject(CGObject_C* object, const CClientObjCreate& create, uint32_t time) {
    switch (object->m_typeId) {
        case ID_ITEM: {
            new (object) CGItem_C(time, create);
            break;
        }

        case ID_CONTAINER: {
            new (object) CGContainer_C(time, create);
            break;
        }

        case ID_UNIT: {
            new (object) CGUnit_C(time, create);
            object->AddWorldObject();
            break;
        }

        case ID_PLAYER: {
            new (object) CGPlayer_C(time, create);
            object->AddWorldObject();
            break;
        }

        case ID_GAMEOBJECT: {
            new (object) CGGameObject_C(time, create);
            object->AddWorldObject();
            break;
        }

        case ID_DYNAMICOBJECT: {
            new (object) CGDynamicObject_C(time, create);
            object->AddWorldObject();
            break;
        }

        case ID_CORPSE: {
            new (object) CGCorpse_C(time, create);
            object->AddWorldObject();
            break;
        }

        case ID_AREATRIGGER: {
            new (object) CGAreaTrigger_C(time, create);
            object->AddWorldObject();
            break;
        }
    }
}

// OFFSET: 0x0048FBD0/0x1400B44B0
static void DestroyRemovedObject(CGObject_C* object) {
    int32_t activePlayer = object->m_obj->m_guid == ClntObjMgrGetActivePlayer();

    switch (object->m_obj->m_type) {
        case TYPEMASK_OBJECT: {
            object->~CGObject_C();
            break;
        }

        case TYPEMASK_OBJECT | TYPEMASK_ITEM: {
            static_cast<CGItem_C*>(object)->~CGItem_C();
            break;
        }

        case TYPEMASK_OBJECT | TYPEMASK_ITEM | TYPEMASK_CONTAINER: {
            static_cast<CGContainer_C*>(object)->~CGContainer_C();
            break;
        }

        case TYPEMASK_OBJECT | TYPEMASK_UNIT: {
            static_cast<CGUnit_C*>(object)->~CGUnit_C();
            break;
        }

        case TYPEMASK_OBJECT | TYPEMASK_UNIT | TYPEMASK_PLAYER: {
            static_cast<CGPlayer_C*>(object)->~CGPlayer_C();
            break;
        }

        case TYPEMASK_OBJECT | TYPEMASK_GAMEOBJECT: {
            static_cast<CGGameObject_C*>(object)->~CGGameObject_C();
            break;
        }

        case TYPEMASK_OBJECT | TYPEMASK_DYNAMICOBJECT: {
            static_cast<CGDynamicObject_C*>(object)->~CGDynamicObject_C();
            break;
        }

        case TYPEMASK_OBJECT | TYPEMASK_CORPSE: {
            static_cast<CGCorpse_C*>(object)->~CGCorpse_C();
            break;
        }

        case TYPEMASK_OBJECT | TYPEMASK_AREATRIGGER: {
            static_cast<CGAreaTrigger_C*>(object)->~CGAreaTrigger_C();
            break;
        }
    }

    if (activePlayer) {
        SMemFree(object, __FILE__, __LINE__, 0);
    } else {
        ObjectFree(s_objHeapIds[object->m_typeId], object->m_heapIndex, object, __FILE__, __LINE__);
    }
}

// PROVEN: 5e07f643 IDENTICAL/NONE
// OFFSET: 0x0048D8E0
static void SkipSetOfObjects(CDataStore* msg) {
    uint32_t count;
    msg->Get(count);

    for (uint32_t i = 0; i < count; i++) {
        SmartGUID guid;
        *msg >> guid;
    }
}

// OFFSET: 0x0048DAE0
static uint32_t ObjDescriptorToMirrorOffset(OBJECT_TYPE_ID typeId, int32_t local, uint32_t offset, uint32_t size) {
    switch (typeId) {
        case ID_CONTAINER: {
            if (offset >= sizeof(CGObjectData) + sizeof(CGItemData)) {
                return CGContainer::DescriptorToMirrorOffset(offset, size, 0, sizeof(CGObjectData) + sizeof(CGItemData));
            }
        }

        case ID_ITEM: {
            if (offset >= sizeof(CGObjectData)) {
                return CGItem::DescriptorToMirrorOffset(offset, size, 0, sizeof(CGObjectData));
            }

            break;
        }

        case ID_PLAYER: {
            if (offset >= sizeof(CGObjectData) + sizeof(CGUnitData)) {
                return CGPlayer::DescriptorToMirrorOffset(offset, size, local, sizeof(CGObjectData) + sizeof(CGUnitData));
            }
        }

        case ID_UNIT: {
            if (offset >= sizeof(CGObjectData)) {
                return CGUnit::DescriptorToMirrorOffset(offset, size, local, sizeof(CGObjectData));
            }

            break;
        }

        case ID_GAMEOBJECT: {
            if (offset >= sizeof(CGObjectData)) {
                return CGGameObject::DescriptorToMirrorOffset(offset, size, 0, sizeof(CGObjectData));
            }

            break;
        }

        case ID_DYNAMICOBJECT: {
            if (offset >= sizeof(CGObjectData)) {
                return CGDynamicObject::DescriptorToMirrorOffset(offset, size, 0, sizeof(CGObjectData));
            }

            break;
        }

        case ID_CORPSE: {
            if (offset >= sizeof(CGObjectData)) {
                return CGCorpse::DescriptorToMirrorOffset(offset, size, 0, sizeof(CGObjectData));
            }

            break;
        }

        case ID_AREATRIGGER: {
            if (offset >= sizeof(CGObjectData)) {
                return CGAreaTrigger::DescriptorToMirrorOffset(offset, size, 0, sizeof(CGObjectData));
            }

            break;
        }
    }

    return (offset & 3) + 4 * CGObject::GetSavedIndexFromDataIndex(offset >> 2);
}

// OFFSET: 0x0048DCE0
static void SetupObjectStorage(CGObject_C* object, OBJECT_TYPE_ID typeId, WOWGUID guid) {
    uint8_t* base = reinterpret_cast<uint8_t*>(object);

    switch (typeId) {
        case ID_OBJECT: {
            uint32_t size = sizeof(CGObjectData);
            uint32_t* storage = reinterpret_cast<uint32_t*>(base + sizeof(CGObject_C));
            object->SetStorage(storage, reinterpret_cast<uint32_t*>(base + sizeof(CGObject_C) + size));
            memset(storage, 0, size);
            break;
        }

        case ID_ITEM: {
            uint32_t size = sizeof(CGObjectData) + sizeof(CGItemData);
            uint32_t* storage = reinterpret_cast<uint32_t*>(base + sizeof(CGItem_C));
            static_cast<CGItem_C*>(object)->SetStorage(storage, reinterpret_cast<uint32_t*>(base + sizeof(CGItem_C) + size));
            memset(storage, 0, size);
            break;
        }

        case ID_CONTAINER: {
            uint32_t size = sizeof(CGObjectData) + sizeof(CGItemData) + sizeof(CGContainerData);
            uint32_t* storage = reinterpret_cast<uint32_t*>(base + sizeof(CGContainer_C));
            static_cast<CGContainer_C*>(object)->SetStorage(storage, reinterpret_cast<uint32_t*>(base + sizeof(CGContainer_C) + size));
            memset(storage, 0, size);
            break;
        }

        case ID_UNIT: {
            uint32_t size = sizeof(CGObjectData) + sizeof(CGUnitData);
            uint32_t* storage = reinterpret_cast<uint32_t*>(base + sizeof(CGUnit_C));
            static_cast<CGUnit_C*>(object)->SetStorage(storage, reinterpret_cast<uint32_t*>(base + sizeof(CGUnit_C) + size));
            memset(storage, 0, size);
            break;
        }

        case ID_PLAYER: {
            CGPlayer_C* player = static_cast<CGPlayer_C*>(object);
            uint32_t size;

            if (guid == ClntObjMgrGetActivePlayer()) {
                size = sizeof(CGObjectData) + sizeof(CGUnitData) + sizeof(CGPlayerLocalData);
                player->m_playerLocalMirrorLists = reinterpret_cast<TSList<void>*>(base + sizeof(CGPlayer_C) + size + CGPlayer::TotalFields() * sizeof(uint32_t));
            } else {
                size = sizeof(CGObjectData) + sizeof(CGUnitData) + sizeof(CGPlayerData);
                player->m_playerLocalMirrorLists = NULL;
            }

            uint32_t* storage = reinterpret_cast<uint32_t*>(base + sizeof(CGPlayer_C));
            player->SetStorage(storage, reinterpret_cast<uint32_t*>(base + sizeof(CGPlayer_C) + size));
            memset(storage, 0, size);
            break;
        }

        case ID_GAMEOBJECT: {
            uint32_t size = sizeof(CGObjectData) + sizeof(CGGameObjectData);
            uint32_t* storage = reinterpret_cast<uint32_t*>(base + sizeof(CGGameObject_C));
            static_cast<CGGameObject_C*>(object)->SetStorage(storage, reinterpret_cast<uint32_t*>(base + sizeof(CGGameObject_C) + size));
            memset(storage, 0, size);
            break;
        }

        case ID_DYNAMICOBJECT: {
            uint32_t size = sizeof(CGObjectData) + sizeof(CGDynamicObjectData);
            uint32_t* storage = reinterpret_cast<uint32_t*>(base + sizeof(CGDynamicObject_C));
            static_cast<CGDynamicObject_C*>(object)->SetStorage(storage, reinterpret_cast<uint32_t*>(base + sizeof(CGDynamicObject_C) + size));
            memset(storage, 0, size);
            break;
        }

        case ID_CORPSE: {
            uint32_t size = sizeof(CGObjectData) + sizeof(CGCorpseData);
            uint32_t* storage = reinterpret_cast<uint32_t*>(base + sizeof(CGCorpse_C));
            static_cast<CGCorpse_C*>(object)->SetStorage(storage, reinterpret_cast<uint32_t*>(base + sizeof(CGCorpse_C) + size));
            memset(storage, 0, size);
            break;
        }

        case ID_AREATRIGGER: {
            uint32_t size = sizeof(CGObjectData) + sizeof(CGAreaTriggerData);
            uint32_t* storage = reinterpret_cast<uint32_t*>(base + sizeof(CGAreaTrigger_C));
            static_cast<CGAreaTrigger_C*>(object)->SetStorage(storage, reinterpret_cast<uint32_t*>(base + sizeof(CGAreaTrigger_C) + size));
            memset(storage, 0, size);
            break;
        }
    }
}

// OFFSET: 0x0048E010/0x1400B1480
static void ExpirePendingHandlers(TSExplicitList<CMirrorHandler, offsetof(CMirrorHandler, m_link2)>* pending) {
    CMirrorHandler* handler = pending->Head();

    while (handler) {
        CMirrorHandler* next = pending->Next(handler);

        if (handler->m_count-- == 1) {
            pending->Link(handler)->Unlink();
        }

        handler = next;
    }
}

// OFFSET: 0x0048FDF0
static void ExpireOldObjects(OBJECT_TYPE_ID typeId, uint32_t expireTime) {
    CGObject_C* object = s_curMgr->m_lazyCleanupFifo[typeId - 1].Head();

    if (object && !object->m_fadeoutInfo && static_cast<int32_t>(FrameTime::GetCurTimeMs() - object->m_disableTime - expireTime) >= 0) {
        s_curMgr->m_lazyCleanupObjects.Unlink(object);
        object->m_link.Unlink();
        DestroyRemovedObject(object);
    }
}

// OFFSET: 0x0048FE60
CGObject_C* ClntObjMgrAllocObject(OBJECT_TYPE_ID typeId, WOWGUID guid) {
    if (guid == ClntObjMgrGetActivePlayer()) {
        return static_cast<CGObject_C*>(SMemAlloc(sizeof(CGPlayer_C) + s_objDwordCount[ID_PLAYER] * sizeof(uint32_t) + (sizeof(CGPlayerLocalData) - sizeof(CGPlayerData)) / sizeof(uint32_t) * sizeof(TSList<void>) + CGPlayer::TotalFields() * sizeof(uint32_t), __FILE__, __LINE__, 0));
    }

    ExpireOldObjects(typeId, 10000);

    uint32_t memHandle;
    void* object;

    if (!ObjectAlloc(s_objHeapIds[typeId], &memHandle, &object, false, __FILE__, __LINE__)) {
        return NULL;
    }

    static_cast<CGObject_C*>(object)->m_heapIndex = memHandle;
    return static_cast<CGObject_C*>(object);
}

// OFFSET: 0x0048E0F0/0x1400B2120
static int32_t CCommand_ObjUsage(const char* command, const char* arguments) {
    ObjectMgr* mgr = s_curMgr;
    uint32_t visible = 0;
    uint32_t active = 0;
    uint32_t lazy = 0;
    uint32_t items = 0;
    uint32_t units = 0;
    uint32_t gameObjects = 0;

    for (CGObject_C* object = mgr->m_visibleObjects.Head(); object; object = mgr->m_visibleObjects.Next(object)) {
        visible++;
    }

    for (CGObject_C* object = mgr->m_objects.m_fulllist.Head(); object; object = mgr->m_objects.m_fulllist.Next(object)) {
        uint16_t type = object->m_obj->m_type;
        active++;

        if ((type & TYPEMASK_ITEM) || (type & TYPEMASK_CONTAINER)) {
            items++;
        } else if (type & TYPEMASK_UNIT) {
            units++;
        } else if (type & TYPEMASK_GAMEOBJECT) {
            gameObjects++;
        }
    }

    for (CGObject_C* object = mgr->m_lazyCleanupObjects.m_fulllist.Head(); object; object = mgr->m_lazyCleanupObjects.m_fulllist.Next(object)) {
        lazy++;
    }

    ConsoleWrite("Object manager list status: (use gmvision to see server onlys)", HIGHLIGHT_COLOR);
    ConsoleWriteA("    Active objects:    %u (%u visible)", HIGHLIGHT_COLOR, active, visible);
    ConsoleWriteA("    Units: %u,   GameObjs: %u    Items: %u,    Other: %u", HIGHLIGHT_COLOR, units, gameObjects, items, active - gameObjects - units - items);
    ConsoleWriteA("    Objects waiting to be freed: %u objects", HIGHLIGHT_COLOR, lazy);
    return 1;
}

// PROVEN: 7dd71eb2 IDENTICAL/NONE
// OFFSET: 0x0048E240/0x1400B22B0
void ClntObjMgrInitializeShared() {
    if (!s_heapsAllocated) {
        for (uint32_t i = ID_ITEM; i < NUM_CLIENT_OBJECT_TYPES; i++) {
            s_objHeapIds[i] = ObjectAllocAddHeap(s_objTotalSize[i], s_objHeapSize[i], s_objNames[i], true);
        }

        s_heapsAllocated = true;
    }

    MirrorInitialize();
    ConsoleCommandRegister("ObjUsage", CCommand_ObjUsage, CATEGORY_GAME, NULL, NULL);
    ConsoleCommandRegister("ShowObjUsage", CCommand_ObjUsage, CATEGORY_GAME, NULL, NULL);
}

// OFFSET: 0x0048EA80
static void CallMirrorFunctions(TSExplicitList<CMirrorHandler, offsetof(CMirrorHandler, m_link2)>* pending, WOWGUID guid, CGObject_C* object, OBJECT_TYPE_ID typeId) {
    CMirrorHandler* handler = pending->Head();

    while (handler) {
        handler->m_dispatching = 1;
        handler->m_count = 1;

        const void* mirror = static_cast<uint8_t*>(object->m_objMirror) + handler->m_mirrorByteOffset;

        if (handler->m_alwaysFire || memcmp(reinterpret_cast<uint8_t*>(object->m_obj) + handler->m_fieldByteOffset, mirror, handler->m_fieldByteSize)) {
            handler->m_function(guid, handler->m_fieldByteOffset - GetObjectTypeFieldByteOffset(typeId), handler->m_fieldByteSize, mirror, handler->m_functionParam);
        }

        CMirrorHandler* current = handler;
        handler = pending->Next(handler);

        if (current->m_deletePending) {
            delete current;
        } else {
            current->m_dispatching = 0;
        }
    }
}

// OFFSET: 0x0048DF60/0x1400B2060
static void CopyMirrorFields(TSList<CMirrorHandler, TSGetLink<CMirrorHandler> >* list, CGObject_C* object) {
    int32_t local = object->m_obj->m_guid == ClntObjMgrGetActivePlayer();

    for (CMirrorHandler* handler = list->Head(); handler; handler = handler->m_link.Next()) {
        uint32_t mirrorOffset = ObjDescriptorToMirrorOffset(object->m_typeId, local, handler->m_fieldByteOffset, handler->m_fieldByteSize);
        memcpy(static_cast<uint8_t*>(object->m_objMirror) + mirrorOffset, reinterpret_cast<uint8_t*>(object->m_obj) + handler->m_fieldByteOffset, handler->m_fieldByteSize);
    }
}

// OFFSET: 0x0048E080/0x1400B11C0
static int32_t LinkPendingNodes(TSExplicitList<CMirrorHandler, offsetof(CMirrorHandler, m_link2)>* pending, TSList<CMirrorHandler, TSGetLink<CMirrorHandler> >* list) {
    if (!list || !list->Head()) {
        return 0;
    }

    for (CMirrorHandler* handler = list->Head(); handler; handler = handler->m_link.Next()) {
        uint32_t misalign = handler->m_fieldByteOffset & 3;
        pending->LinkNode(handler, (handler->m_linkPositionSelector != 1) + 1, NULL);
        handler->m_count = (handler->m_fieldByteSize + misalign + 3) >> 2;
    }

    return 1;
}

// OFFSET: 0x0048E6B0/0x1400B27F0
static int32_t FillInPartialObjectData(CGObject_C* object, WOWGUID guid, CDataStore* msg, bool skipMirrorUpdate, bool zeroFillAbsent) {
    uint32_t masks[44];
    uint8_t blockCount;

    if (!ExtractDirtyMasks(msg, &blockCount, masks)) {
        return 0;
    }

    OBJECT_TYPE_ID typeId = ID_OBJECT;
    uint32_t typeBase = 0;
    uint32_t numBlocks = GetNumDwordBlocks(object->m_obj->m_type, guid);
    TSExplicitList<CMirrorHandler, offsetof(CMirrorHandler, m_link2)> pending;

    for (uint32_t i = 0; i < numBlocks; i++) {
        if (i >= s_objDwordCount[typeId]) {
            typeBase = s_objDwordCount[typeId];
            typeId = IncTypeId(object, typeId);
        }

        if (!skipMirrorUpdate) {
            ExpirePendingHandlers(&pending);

            TSList<CMirrorHandler, TSGetLink<CMirrorHandler> >* list = &s_typeMirrorHandlers[typeId][i - typeBase];

            if (LinkPendingNodes(&pending, list)) {
                CopyMirrorFields(list, object);
            }

            list = GetObjectMirrorList(typeId, object, i - typeBase);

            if (LinkPendingNodes(&pending, list)) {
                CopyMirrorFields(list, object);
            }
        }

        uint32_t value;

        if (masks[i >> 5] & (1 << (i & 31))) {
            msg->GetArray(reinterpret_cast<uint8_t*>(&value), sizeof(value));
        } else if (zeroFillAbsent) {
            value = 0;
        } else {
            continue;
        }

        object->SetBlock(i, value);
    }

    return 1;
}

// OFFSET: 0x0048EC40
static int32_t CallMirrorHandlers(CDataStore* msg, bool postInit, WOWGUID guid) {
    if (!postInit) {
        SmartGUID smartGuid;
        *msg >> smartGuid;
        guid = smartGuid;
    }

    CGObject_C* object = s_curMgr->m_objects.Ptr(static_cast<uint32_t>(guid.m_guid), CHashKeyGUID(guid.m_guid));

    if (!object) {
        return SkipPartialObjectUpdate(msg);
    }

    uint32_t masks[44];
    uint8_t blockCount;

    if (!ExtractDirtyMasks(msg, &blockCount, masks)) {
        return 0;
    }

    OBJECT_TYPE_ID typeId = ID_OBJECT;
    uint32_t typeBase = 0;
    uint32_t numBlocks = GetNumDwordBlocks(object->m_obj->m_type, guid);
    TSExplicitList<CMirrorHandler, offsetof(CMirrorHandler, m_link2)> pending;

    for (uint32_t i = 0; i < numBlocks; i++) {
        if (i >= s_objDwordCount[typeId]) {
            typeBase = s_objDwordCount[typeId];
            typeId = IncTypeId(object, typeId);
        }

        ExpirePendingHandlers(&pending);
        LinkPendingNodes(&pending, &s_typeMirrorHandlers[typeId][i - typeBase]);
        LinkPendingNodes(&pending, GetObjectMirrorList(typeId, object, i - typeBase));

        if (masks[i >> 5] & (1 << (i & 31))) {
            uint32_t value = 0;
            msg->Get(value);
        } else if (!postInit) {
            continue;
        }

        CallMirrorFunctions(&pending, guid, object, typeId);
    }

    return 1;
}

// OFFSET: 0x0048E8D0
static void FreeMirrorHandlers(TSList<void>* lists) {
    TSList<CMirrorHandler, TSGetLink<CMirrorHandler> >* list = reinterpret_cast<TSList<CMirrorHandler, TSGetLink<CMirrorHandler> >*>(lists);

    for (CMirrorHandler* handler = list->Head(); handler; handler = list->Head()) {
        list->DeleteNode(handler);
    }
}

// OFFSET: 0x0048EF70/0x1400B3630
static void ClearMirrorLists(CGObject_C* object) {
    switch (object->m_typeId) {
        case ID_CONTAINER: {
            for (uint32_t i = 0; i < sizeof(CGContainerData) / sizeof(uint32_t); i++) {
                FreeMirrorHandlers(&static_cast<CGContainer_C*>(object)->m_containerMirrorLists[i]);
            }
        }

        case ID_ITEM: {
            for (uint32_t i = 0; i < sizeof(CGItemData) / sizeof(uint32_t); i++) {
                FreeMirrorHandlers(&static_cast<CGItem_C*>(object)->m_itemMirrorLists[i]);
            }

            break;
        }

        case ID_PLAYER: {
            CGPlayer_C* player = static_cast<CGPlayer_C*>(object);

            for (uint32_t i = 0; i < sizeof(CGPlayerData) / sizeof(uint32_t); i++) {
                FreeMirrorHandlers(&player->m_playerMirrorLists[i]);
            }

            if (player->IsActivePlayer()) {
                for (uint32_t i = 0; i < (sizeof(CGPlayerLocalData) - sizeof(CGPlayerData)) / sizeof(uint32_t); i++) {
                    FreeMirrorHandlers(&player->m_playerLocalMirrorLists[i]);
                }
            }
        }

        case ID_UNIT: {
            for (uint32_t i = 0; i < sizeof(CGUnitData) / sizeof(uint32_t); i++) {
                FreeMirrorHandlers(&static_cast<CGUnit_C*>(object)->m_unitMirrorLists[i]);
            }

            break;
        }

        case ID_GAMEOBJECT: {
            for (uint32_t i = 0; i < sizeof(CGGameObjectData) / sizeof(uint32_t); i++) {
                FreeMirrorHandlers(&static_cast<CGGameObject_C*>(object)->m_gameObjectMirrorLists[i]);
            }

            break;
        }

        case ID_DYNAMICOBJECT: {
            for (uint32_t i = 0; i < sizeof(CGDynamicObjectData) / sizeof(uint32_t); i++) {
                FreeMirrorHandlers(&static_cast<CGDynamicObject_C*>(object)->m_dynamicObjectMirrorLists[i]);
            }

            break;
        }

        case ID_CORPSE: {
            for (uint32_t i = 0; i < sizeof(CGCorpseData) / sizeof(uint32_t); i++) {
                FreeMirrorHandlers(&static_cast<CGCorpse_C*>(object)->m_corpseMirrorLists[i]);
            }

            break;
        }

        case ID_AREATRIGGER: {
            for (uint32_t i = 0; i < sizeof(CGAreaTriggerData) / sizeof(uint32_t); i++) {
                FreeMirrorHandlers(&static_cast<CGAreaTrigger_C*>(object)->m_areaTriggerMirrorLists[i]);
            }

            break;
        }
    }

    for (uint32_t i = 0; i < sizeof(CGObjectData) / sizeof(uint32_t); i++) {
        FreeMirrorHandlers(&object->m_objMirrorLists[i]);
    }
}

// OFFSET: 0x0048FF10
static int32_t ExpireLazyObjects(const void* data, void* param) {
    for (uint32_t typeId = ID_ITEM; typeId < NUM_CLIENT_OBJECT_TYPES; typeId++) {
        ExpireOldObjects(static_cast<OBJECT_TYPE_ID>(typeId), 30000);
    }

    return 1;
}

// OFFSET: 0x004900F0/0x1400B4920
void ClntObjMgrDestroyLazyCleanupObjects() {
    CGObject_C* object;

    while ((object = s_curMgr->m_lazyCleanupObjects.m_fulllist.Head()) != NULL) {
        s_curMgr->m_lazyCleanupObjects.Unlink(object);
        object->m_link.Unlink();
        DestroyRemovedObject(object);
    }
}

// OFFSET: 0x0048FFD0
static void DestroyAllObjects() {
    CGObject_C* object;

    while ((object = s_curMgr->m_objects.m_fulllist.Head()) != NULL) {
        s_curMgr->m_objects.Unlink(object);

        if (s_curMgr->m_visibleObjects.IsLinked(object)) {
            s_curMgr->m_visibleObjects.UnlinkNode(object);
        }

        ClearMirrorLists(object);
        DestroyRemovedObject(object);
    }

    while ((object = s_curMgr->m_lazyCleanupObjects.m_fulllist.Head()) != NULL) {
        s_curMgr->m_lazyCleanupObjects.Unlink(object);
        object->m_link.Unlink();
        DestroyRemovedObject(object);
    }

    ClientServices::ClearMessageHandler(SMSG_UPDATE_OBJECT);
    ClientServices::ClearMessageHandler(SMSG_DESTROY_OBJECT);
    EventUnregister(EVENT_ID_IDLE, ExpireLazyObjects);
}

// OFFSET: 0x0048F2A0/0x1400B3980
void ClntObjMgrSetObjMirrorHandler(WOWGUID guid, OBJECT_TYPE_ID typeId, uint32_t fieldOffset, uint32_t fieldSize, MIRRORHANDLERFUNC function, void* param, HANDLER_PRIORITY priority, int32_t alwaysFire) {
    ObjectMgr* mgr = s_curMgr;
    CGObject_C* object = mgr->m_objects.Ptr(static_cast<uint32_t>(guid.m_guid), CHashKeyGUID(guid.m_guid));

    if (!object) {
        return;
    }

    uint32_t fieldByteOffset = fieldOffset + GetObjectTypeFieldByteOffset(typeId);
    uint32_t mirrorByteOffset = ObjDescriptorToMirrorOffset(object->m_typeId, guid == ClntObjMgrGetActivePlayer(), fieldByteOffset, fieldSize);
    AssignMirrorHandler(GetObjectMirrorList(typeId, object, fieldOffset >> 2), fieldByteOffset, mirrorByteOffset, fieldSize, function, param, priority, alwaysFire);
}

// OFFSET: 0x0048F360/0x1400B3AF0
void ClntObjMgrUnsetObjMirrorHandler(WOWGUID guid, OBJECT_TYPE_ID typeId, uint32_t fieldOffset, MIRRORHANDLERFUNC function, void* param) {
    CGObject_C* object = s_curMgr->m_objects.Ptr(static_cast<uint32_t>(guid.m_guid), CHashKeyGUID(guid.m_guid));

    if (object) {
        UnassignMirrorHandler(GetObjectMirrorList(typeId, object, fieldOffset >> 2), function, param);
    }
}

// OFFSET: 0x0048F490
void ClntObjMgrDestroyShared() {
    for (uint32_t typeId = 0; typeId < NUM_CLIENT_OBJECT_TYPES; typeId++) {
        for (uint32_t i = 0; i < sizeof(s_typeMirrorHandlers[0]) / sizeof(s_typeMirrorHandlers[0][0]); i++) {
            TSList<CMirrorHandler, TSGetLink<CMirrorHandler> >* list = &s_typeMirrorHandlers[typeId][i];

            for (CMirrorHandler* handler = list->Head(); handler; handler = list->Head()) {
                list->DeleteNode(handler);
            }
        }
    }

    ConsoleCommandUnregister("ObjUsage");
}

// OFFSET: 0x0048F9C0/0x1400B4190
ObjectMgr::ObjectMgr(uint32_t type) {
    this->m_type = type;
    this->m_playerGuid = WOWGUID();
    this->m_mapId = 0xFFFFFFFF;
    this->m_realmConnection = NULL;
    this->m_movementGlobal = NULL;
}

// OFFSET: 0x0048FAF0
ObjectMgr::~ObjectMgr() {
    if (this->m_realmConnection) {
        this->m_realmConnection->m_objMgr = NULL;
    }

    if (this == s_savedObjMgr) {
        s_savedObjMgr = NULL;
    }
}

// OFFSET: 0x0048EE20
static int32_t PostInitObject(CGPlayer_C** activePlayer, CDataStore* msg, uint32_t time, bool reenable, bool* transport) {
    SmartGUID guid;
    *msg >> guid;

    uint8_t typeId;
    msg->Get(typeId);

    if (!guid.m_guid) {
        return 0;
    }

    CGObject_C* object = s_curMgr->m_objects.Ptr(static_cast<uint32_t>(guid.m_guid), CHashKeyGUID(guid.m_guid));

    if (!object) {
        return 0;
    }

    if (!*activePlayer && (object->m_obj->m_type & TYPEMASK_PLAYER) && static_cast<CGPlayer_C*>(object)->IsActivePlayer()) {
        *activePlayer = static_cast<CGPlayer_C*>(object);
    }

    if (!*transport && (object->m_obj->m_type & TYPEMASK_GAMEOBJECT)) {
        *transport = static_cast<CGGameObject_C*>(object)->m_gameObject->m_typeID == 33;
    }

    CClientObjCreate create;
    create.Get(msg, true);

    if (object->m_modelFlags & 0x20000) {
        object->PostReenable(create);
    }

    int32_t result;

    if (object->m_modelFlags & 0x40000) {
        result = CallMirrorHandlers(msg, true, guid);
    } else {
        object->PostInit(time, create, reenable);
        result = SkipPartialObjectUpdate(msg);
    }

    return result;
}

// OFFSET: 0x004902F0
void ClntObjMgrDestroyStd() {
    if (!s_curMgr) {
        return;
    }

    DestroyAllObjects();

    ObjectMgr* mgr = s_curMgr;
    s_curMgr = NULL;
    delete mgr;
}

// OFFSET: 0x00490670
static CGObject_C* GetUpdateObject(WOWGUID guid, int32_t* reenabled) {
    *reenabled = 0;

    ObjectMgr* mgr = s_curMgr;
    CGObject_C* object = mgr->m_objects.Ptr(static_cast<uint32_t>(guid.m_guid), CHashKeyGUID(guid.m_guid));

    if (object) {
        object->SetDisablePending(0);
        return object;
    }

    object = mgr->m_lazyCleanupObjects.Ptr(static_cast<uint32_t>(guid.m_guid), CHashKeyGUID(guid.m_guid));

    if (!object) {
        return NULL;
    }

    mgr->m_lazyCleanupObjects.Unlink(object);
    object->m_link.Unlink();
    mgr->m_objects.Insert(object, static_cast<uint32_t>(guid.m_guid), CHashKeyGUID(guid.m_guid));

    if (!s_curMgr->m_visibleObjects.IsLinked(object) && !(object->m_modelFlags & 0x20000)) {
        *reenabled = 1;
    }

    return object;
}

// OFFSET: 0x0048EB90
static int32_t UpdateExistingObject(CDataStore* msg, CGObject_C* object, int32_t reenabled) {
    CClientObjCreate create;
    create.Get(msg, true);

    if (!create.IsValid() || !FillInPartialObjectData(object, object->m_obj->m_guid, msg, false, true)) {
        return 0;
    }

    if (reenabled) {
        object->Reenable(create);
    }

    s_curMgr->m_visibleObjects.LinkToTail(object);
    return 1;
}

// OFFSET: 0x00490780/0x1400B5420
static int32_t CreateObject(uint32_t time, CDataStore* msg) {
    SmartGUID guid;
    *msg >> guid;

    uint8_t typeId;
    msg->Get(typeId);

    if (typeId >= NUM_CLIENT_OBJECT_TYPES) {
        return 0;
    }

    int32_t reenabled;
    CGObject_C* object = GetUpdateObject(guid, &reenabled);

    if (object) {
        return UpdateExistingObject(msg, object, reenabled);
    }

    CClientObjCreate create;
    create.Get(msg, true);

    if (!create.IsValid()) {
        return 0;
    }

    if (create.m_isActivePlayer) {
        s_curMgr->m_playerGuid = guid;
    }

    object = ClntObjMgrAllocObject(static_cast<OBJECT_TYPE_ID>(typeId), guid);
    SetupObjectStorage(object, static_cast<OBJECT_TYPE_ID>(typeId), guid);
    object->SetTypeID(static_cast<OBJECT_TYPE_ID>(typeId));

    if (!FillInPartialObjectData(object, guid, msg, true, true)) {
        return 0;
    }

    InitObject(object, create, time);
    s_curMgr->m_visibleObjects.LinkToTail(object);
    return 1;
}

// OFFSET: 0x004908F0/0x1400B5610
static int32_t UpdateObjectValues(CDataStore* msg) {
    SmartGUID guid;
    *msg >> guid;

    int32_t reenabled;
    CGObject_C* object = GetUpdateObject(guid, &reenabled);

    if (!object) {
        PlayerObjectUpdateFailed failed;
        memcpy(failed.m_guid, &guid.m_guid, sizeof(failed.m_guid));
        ClientServices::Send(&failed);
        return SkipPartialObjectUpdate(msg);
    }

    if (!FillInPartialObjectData(object, object->m_obj->m_guid, msg, false, false)) {
        return 0;
    }

    if (reenabled) {
        PlayerObjectUpdateRescued rescued;
        memcpy(rescued.m_guid, &guid.m_guid, sizeof(rescued.m_guid));
        ClientServices::Send(&rescued);
    }

    return 1;
}

// OFFSET: 0x004909B0/0x1400B56E0
static void ObjDelete(CGObject_C* object) {
    ClearMirrorLists(object);

    ObjectMgr* mgr = s_curMgr;
    mgr->m_objects.Unlink(object);

    CHashKeyGUID key = object->m_key;
    uint32_t hashval = object->m_hashval;

    if (mgr->m_visibleObjects.IsLinked(object)) {
        mgr->m_visibleObjects.UnlinkNode(object);
    }

    mgr->m_lazyCleanupObjects.Insert(object, hashval, key);
    mgr->m_lazyCleanupFifo[object->m_typeId - 1].LinkToTail(object);
}

// OFFSET: 0x00490A40/0x1400B57E0
static int32_t ObjectUpdateFirstPass(uint32_t index, uint32_t count, CDataStore* msg, uint32_t time) {
    for (; index < count; index++) {
        uint8_t type;
        msg->Get(type);

        if (type == 0) {
            if (!UpdateObjectValues(msg)) {
                return 0;
            }
        } else if (type <= 2) {
            if (!CreateObject(time, msg)) {
                return 0;
            }
        } else {
            DebugLog("Unknown client update packet type (%d)!", type);
            return 0;
        }
    }

    return 1;
}

// OFFSET: 0x0048F0E0
static int32_t ObjectUpdateSecondPass(CGPlayer_C** activePlayer, CDataStore* msg, bool* transport, uint32_t time, uint32_t count) {
    *activePlayer = NULL;
    *transport = false;

    for (uint32_t i = 0; i < count; i++) {
        uint8_t type;
        msg->Get(type);

        switch (type) {
            case 0: {
                if (!CallMirrorHandlers(msg, false, WOWGUID())) {
                    return 0;
                }

                break;
            }

            case 1:
            case 2: {
                if (!PostInitObject(activePlayer, msg, time, type == 2, transport)) {
                    return 0;
                }

                break;
            }

            case 3: {
                SkipSetOfObjects(msg);
                break;
            }
        }
    }

    return 1;
}

// OFFSET: 0x00490AB0
static void UpdateOutOfRangeObjects(CDataStore* msg) {
    uint32_t count;
    msg->Get(count);

    CVehiclePassenger_C::StartAddingPendingRescueTransitions();

    uint32_t read = msg->m_read;

    for (uint32_t i = count; i; i--) {
        SmartGUID guid;
        *msg >> guid;

        if (guid == ClntObjMgrGetActivePlayer()) {
            continue;
        }

        CGObject_C* object = s_curMgr->m_objects.Ptr(static_cast<uint32_t>(guid.m_guid), CHashKeyGUID(guid.m_guid));

        if (!object) {
            continue;
        }

        if (CGBattlefieldInfo::m_instanceType == 4) {
            WOWGUID objectGuid = object->m_obj->m_guid;
            CGBattlefieldInfo::OnArenaUnitOutOfRange(objectGuid);
        }

        object->HandleOutOfRange(static_cast<OUT_OF_RANGE_TYPE>(0));

        if (object->IsObjectLocked()) {
            object->SetDisablePending(1);
        } else {
            object->SetDisablePending(0);
            object->Disable();
        }
    }

    msg->m_read = read;

    for (uint32_t i = count; i; i--) {
        SmartGUID guid;
        *msg >> guid;

        if (guid == ClntObjMgrGetActivePlayer()) {
            continue;
        }

        CGObject_C* object = s_curMgr->m_objects.Ptr(static_cast<uint32_t>(guid.m_guid), CHashKeyGUID(guid.m_guid));

        if (object && !object->IsObjectLocked()) {
            ObjDelete(object);
        }
    }

    CVehiclePassenger_C::ExecutePendingRescueTransitions();
}

// OFFSET: 0x00490C20
int32_t Packet_SMSG_UPDATE_OBJECT(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    uint32_t start = msg->m_read;

    uint16_t mapId = 0;
    msg->Get(mapId);

    uint32_t count = 0;
    msg->Get(count);

    uint32_t read = msg->m_read;
    uint8_t type = 0;
    msg->Get(type);

    uint32_t index;

    if (type == 3) {
        UpdateOutOfRangeObjects(msg);
        index = 1;
    } else {
        msg->m_read = read;
        index = 0;
    }

    int32_t result = 0;

    if (ObjectUpdateFirstPass(index, count, msg, time)) {
        msg->m_read = read;

        CGPlayer_C* activePlayer;
        bool transport;
        result = ObjectUpdateSecondPass(&activePlayer, msg, &transport, time, count);

        if (result && activePlayer) {
            activePlayer->UpdateObjectComplete(transport);
        }
    }

    if (mapId == s_curMgr->m_mapId) {
        return result;
    }

    C3Vector position(0.0f, 0.0f, 0.0f);
    float facing = 0.0f;
    CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();

    if (player) {
        position = player->GetPosition();
        facing = player->GetFacing();
    }

    ClientLoadNewWorld(mapId, &position, facing);
    msg->m_read = start;
    return Packet_SMSG_UPDATE_OBJECT(NULL, SMSG_UPDATE_OBJECT, time, msg);
}

// OFFSET: 0x00490D70
int32_t Packet_SMSG_DESTROY_OBJECT(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    WOWGUID guid;
    msg->Get(guid.m_guid);

    uint8_t death;
    msg->Get(death);

    CGObject_C* object = s_curMgr->m_objects.Ptr(static_cast<uint32_t>(guid.m_guid), CHashKeyGUID(guid.m_guid));

    if (!object) {
        return 1;
    }

    if (death && (object->m_obj->m_type & TYPEMASK_UNIT) && static_cast<CGUnit_C*>(object)->m_unit->m_unitFieldHealth > 0) {
        static_cast<CGUnit_C*>(object)->OnDeath();
    }

    object->HandleOutOfRange(static_cast<OUT_OF_RANGE_TYPE>(1));

    if (object->IsObjectLocked()) {
        object->SetDisablePending(1);
        return 1;
    }

    object->SetDisablePending(0);
    object->Disable();
    ObjDelete(object);
    return 1;
}

// OFFSET: 0x00490E40/0x1400B5D80
void ClntObjMgrInitializeStd(uint32_t mapId) {
    CGGameUI::SetLastInstanceTime(static_cast<int32_t>(time(NULL)), CGGameUI::m_iCurrentMapID, mapId);

    ObjectMgr* mgr = new (__FILE__, __LINE__) ObjectMgr(0);
    g_clientConnection->m_objMgr = reinterpret_cast<ClntObjMgr*>(mgr);
    mgr->m_realmConnection = g_clientConnection;
    s_curMgr = mgr;
    ClientServices::SetMessageHandler(SMSG_UPDATE_OBJECT, Packet_SMSG_UPDATE_OBJECT, NULL);
    ClientServices::SetMessageHandler(SMSG_DESTROY_OBJECT, Packet_SMSG_DESTROY_OBJECT, NULL);
    EventRegisterEx(EVENT_ID_IDLE, ExpireLazyObjects, NULL, 0.0f);
    s_curMgr->m_mapId = mapId;
}

// OFFSET: 0x00490F20/0x1400B5E90
void ClntObjMgrUnlockObject(WOWGUID guid, const char* fileName, int32_t lineNumber) {
    CGObject_C* object = s_curMgr->m_objects.Ptr(static_cast<uint32_t>(guid.m_guid), CHashKeyGUID(guid.m_guid));

    if (!object) {
        return;
    }

    object->SetObjectLocked(0);

    if (object->IsObjectLocked() || !(object->m_modelFlags & 0x100000)) {
        return;
    }

    object->HandleOutOfRange(static_cast<OUT_OF_RANGE_TYPE>(2));

    if (object->IsObjectLocked()) {
        object->SetDisablePending(1);
    } else {
        object->SetDisablePending(0);
        object->Disable();
        ObjDelete(object);
    }
}

// OFFSET: [0x0048E430/0x1400B24C0]
CMirrorHandler::~CMirrorHandler() {
}

// OFFSET: 0x004031D0/0x1400066C0
CGPlayer_C* ClntObjMgrGetActivePlayerObj() {
    return static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));
}

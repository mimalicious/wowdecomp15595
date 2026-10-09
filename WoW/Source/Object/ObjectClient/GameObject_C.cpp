#include "GameObject_C.h"
#include "WoW/Source/Object/ObjectClient/Item_C.h"
#include <cmath>
#include <cstring>
#include "Engine/Source/Base/CDataStore.h"
#include "Engine/Source/Base/Handle.h"
#include "Engine/Source/Base/Status.h"
#include "Engine/Source/Base/Types.h"
#include "Engine/Source/ConsoleBase/ConsoleVar.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/Gx/CGxTexFlags.h"
#include "Engine/Source/Model2/M2Data.h"
#include "Engine/Source/Model2/M2Model.h"
#include "Engine/Source/Model2/M2Scene.h"
#include "Engine/Source/Model2/M2Shared.h"
#include "Engine/Source/Os/System.h"
#include "Engine/Source/Os/TimeManager.h"
#include "Engine/Source/Services/AsyncFile.h"
#include "Engine/Source/Services/SysMessage.h"
#include "Engine/Source/Services/Texture.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/Tempest/C4Quaternion.h"
#include "Engine/Source/Tempest/CAaSphere.h"
#include "Engine/Source/Tempest/CMath.h"
#include "WoW/Source/DB/DBClient/DBCacheInstances.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/DBClient/GameObjectStats_C.h"
#include "WoW/Source/DB/Rec/DestructibleModelDataRec.h"
#include "WoW/Source/DB/Rec/GameObjectDisplayInfoRec.h"
#include "WoW/Source/DB/Rec/SpellEffectCameraShakesRec.h"
#include "WoW/Source/DB/Rec/SpellVisualEffectNameRec.h"
#include "WoW/Source/Game/GameClient/PlayerName.h"
#include "WoW/Source/JamClientAutoCode/JamClient.h"
#include "WoW/Source/WowServices/WDataStore.h"
#include "WoW/Source/LoadingScreen.h"
#include "WoW/Source/Magic/MagicClient/Spell_C.h"
#include "WoW/Source/Object/GameObjectDef.h"
#include "WoW/Source/Object/GameObjectStats.h"
#include "WoW/Source/Object/MovementShared.h"
#include "WoW/Source/Object/ObjectClient/CGGameObject_C_Type_AreaDamage.h"
#include "WoW/Source/Object/ObjectClient/CGGameObject_C_Type_AuraGenerator.h"
#include "WoW/Source/Object/ObjectClient/CGGameObject_C_Type_BarberChair.h"
#include "WoW/Source/Object/ObjectClient/CGGameObject_C_Type_Binder.h"
#include "WoW/Source/Object/ObjectClient/CGGameObject_C_Type_Button.h"
#include "WoW/Source/Object/ObjectClient/CGGameObject_C_Type_Camera.h"
#include "WoW/Source/Object/ObjectClient/CGGameObject_C_Type_CapturePoint.h"
#include "WoW/Source/Object/ObjectClient/CGGameObject_C_Type_Chair.h"
#include "WoW/Source/Object/ObjectClient/CGGameObject_C_Type_Chest.h"
#include "WoW/Source/Object/ObjectClient/CGGameObject_C_Type_Door.h"
#include "WoW/Source/Object/ObjectClient/CGGameObject_C_Type_DuelArbiter.h"
#include "WoW/Source/Object/ObjectClient/CGGameObject_C_Type_DungeonDifficulty.h"
#include "WoW/Source/Object/ObjectClient/CGGameObject_C_Type_FishingHole.h"
#include "WoW/Source/Object/ObjectClient/CGGameObject_C_Type_FlagDrop.h"
#include "WoW/Source/Object/ObjectClient/CGGameObject_C_Type_FlagStand.h"
#include "WoW/Source/Object/ObjectClient/CGGameObject_C_Type_Generic.h"
#include "WoW/Source/Object/ObjectClient/CGGameObject_C_Type_Goober.h"
#include "WoW/Source/Object/ObjectClient/CGGameObject_C_Type_Mailbox.h"
#include "WoW/Source/Object/ObjectClient/CGGameObject_C_Type_MapObj.h"
#include "WoW/Source/Object/ObjectClient/CGGameObject_C_Type_MiniGame.h"
#include "WoW/Source/Object/ObjectClient/CGGameObject_C_Type_QuestGiver.h"
#include "WoW/Source/Object/ObjectClient/CGGameObject_C_Type_SpellCaster.h"
#include "WoW/Source/Object/ObjectClient/CGGameObject_C_Type_SpellFocus.h"
#include "WoW/Source/Object/ObjectClient/CGGameObject_C_Type_Text.h"
#include "WoW/Source/Object/ObjectClient/CGGameObject_C_Type_Transport.h"
#include "WoW/Source/Object/ObjectClient/CGGameObject_C_Type_Trap.h"
#include "WoW/Source/Object/ObjectClient/CGGameObject_C_Type_TrapDoor.h"
#include "WoW/Source/Object/ObjectClient/Effect_C.h"
#include "WoW/Source/Object/ObjectClient/Movement_C.h"
#include "WoW/Source/Object/ObjectClient/ObjectEffect.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/Object/ObjectClient/Unit_C.h"
#include "WoW/Source/Object/ObjectClient/Vehicle_C.h"
#include "WoW/Source/Object/Passenger.h"
#include "WoW/Source/ObjectMgrClient/CClientObjCreate.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/SoundInterface/SI3.h"
#include "WoW/Source/Ui/GameUI.h"
#include "WoW/Source/Ui/ItemTextFrame.h"
#include "WoW/Source/Ui/MinimapFrame.h"
#include "WoW/Source/Ui/PartyFrame.h"
#include "WoW/Source/Ui/RaidInfo.h"
#include "WoW/Source/Ui/SpellBookFrame.h"
#include "WoW/Source/Ui/WorldFrame.h"
#include "WoW/Source/UIUtil/Camera.h"
#include "WoW/Source/UIUtil/Cursor.h"
#include "WoW/Source/UIUtil/Tooltip.h"
#include "WoW/Source/WorldClient/CWorldMath.h"
#include "Engine/Source/Event/FrameTime.h"
#include "WoW/Source/WorldClient/Map.h"
#include "WoW/Source/WorldClient/MapEntity.h"
#include "WoW/Source/WorldClient/World.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"

// STATIC: 0x00BB15EC
static const uint32_t s_oneShotAnims[7] = { 0xFFFFFFFF, 0, 8, 9, 10, 11, 12 };

// STATIC: #0x00ADA974
static uint32_t s_buildingDamageColor = 0xFFFF8400;

// STATIC: 0x00CA5FF8
uint32_t CGGameObject_C_TypeAnimated::s_animSequenceIds[13] = { 145, 147, 148, 149, 146, 150, 151, 152, 153, 154, 155, 156, 157 };

// STATIC: 0x00CA6180
const char* CGGameObject_C_TypeAnimated::s_animStateNames[13] = { "Spawn", "Closed", "Opening", "Open", "Closing", "Destroying", "Destroyed", "Rebuilding", "Custom0", "Custom1", "Custom2", "Custom3", "Despawn" };

// STATIC: 0x00CA61B4
const char* CGGameObject_C_TypeAnimated::s_displayStateNames[3] = { "Open", "Closed", "Destroyed" };

// STATIC: 0x00CA5FC0
uint32_t CGGameObject_C_TypeAnimated::s_oneShotAnimKitIds[7] = { 0, 1577, 1584, 1584, 1584, 1584, 1589 };

// STATIC: 0x00CA5FDC
uint32_t CGGameObject_C_TypeAnimated::s_oneShotAnimKitAnims[7] = { 0xFFFFFFFF, 145, 153, 154, 155, 156, 157 };

struct MapObjSpeedTransition {
    /* 0000 */ uint32_t m_uniqueID;
    /* 0004 */ uint32_t m_duration;
    /* 0008 */ uint32_t m_remaining;
    /* 000C */ float m_startSpeed;
    /* 0010 */ float m_targetSpeed;
    /* 0014 */ uint32_t m_applied;
};

// STATIC: 0x00DD5D40
static TSCGrowableArray<MapObjSpeedTransition, 8> s_mapObjSpeedTransitions;

// STATIC: 0x00CA61C0
static uint32_t s_mapObjSpeedTransitionTime = 4000;

// OFFSET: 0x005A35F0
static void GameObjectStatsCallback(int32_t id, const WOWGUID& guid, void* param, bool success) {
    CGGameObject_C* object = static_cast<CGGameObject_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_GAMEOBJECT, __FILE__, __LINE__));
    WOWGUID requester = WOWGUID();
    GameObjectStats_C* stats = g_gameObjectDBCache.GetRecord(id, requester, NULL, NULL, false);

    if (stats) {
        object->SetStats(stats);
    }
}

// PROVEN: 45221240 IDENTICAL/NONE
// OFFSET: 0x0059B100
void CGGameObject_C::AnimEventCallback(CM2Model* model, uint32_t boneId, uint32_t eventId, uint32_t data, const C3Vector* position, uint32_t time, uint64_t param) {
    CGGameObject_C* object = (CGGameObject_C*)ClntObjMgrObjectPtr(WOWGUID::Make(param), TYPEMASK_GAMEOBJECT, __FILE__, __LINE__);

    if (object) {
        object->m_typeData->HandleAnimEvent(eventId, data, *position, time);
    }
}

// PROVEN: 360e0662 IDENTICAL/NONE
// OFFSET: 0x0059B1A0
int32_t CGGameObject_C::OnAnimProgressChanged(WOWGUID guid, uint32_t fieldOffset, uint32_t fieldSize, const void* oldValue, void* param) {
    CGGameObject_C* object = (CGGameObject_C*)ClntObjMgrObjectPtr(guid, TYPEMASK_GAMEOBJECT, __FILE__, __LINE__);

    if (object->m_typeData) {
        object->m_typeData->OnUpdateAnimProgress();
    }

    return 1;
}

// PROVEN: cd985646 IDENTICAL/NONE
// OFFSET: 0x0059DD00
int32_t CGGameObject_C::OnFlagsChanged(WOWGUID guid, uint32_t fieldOffset, uint32_t fieldSize, const void* oldValue, void* param) {
    CGGameObject_C* object = (CGGameObject_C*)ClntObjMgrObjectPtr(guid, TYPEMASK_GAMEOBJECT, __FILE__, __LINE__);

    if (object->m_typeData) {
        object->m_typeData->OnUpdateFlags(*static_cast<const uint32_t*>(oldValue) ^ object->m_gameObject->m_flags);
    }

    return 1;
}

// PROVEN: e87f9121 IDENTICAL/NONE
// OFFSET: 0x0059CC50/0x14020B860
void CGGameObject_C_Type_DestructibleBuilding::PostInitWithStats() {
    CGGameObject_C* owner = this->m_owner;

    if (owner->m_nameDesc) {
        PlayerNameDelete(owner->m_nameDesc);
    }

    WOWGUID guid = owner->m_obj->m_guid;
    owner->m_nameDesc = ObjectNameCreate(guid);
}

// PROVEN: f408ffa5 IDENTICAL/NONE
// OFFSET: 0x005A13E0
int32_t CGGameObject_C::OnStateChanged(WOWGUID guid, uint32_t fieldOffset, uint32_t fieldSize, const void* oldValue, void* param) {
    CGGameObject_C* object = (CGGameObject_C*)ClntObjMgrObjectPtr(guid, TYPEMASK_GAMEOBJECT, __FILE__, __LINE__);
    int32_t state = object->m_gameObject->m_state;
    int32_t oldState = object->m_displayState;

    if (state != oldState || object->m_gameObject->m_typeID == 15) {
        object->m_displayState = state;

        if (object->m_typeData) {
            object->m_typeData->UpdateState(oldState, state);
        }
    }

    return 1;
}

// OFFSET: 0x0059A060
int32_t Packet_SMSG_GAMEOBJECT_PAGETEXT(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    WOWGUID guid;
    msg->Get(guid.m_guid);
    CGObject_C* object = ClntObjMgrObjectPtr(guid, TYPEMASK_GAMEOBJECT, __FILE__, __LINE__);

    if (object) {
        WOWGUID objectGuid = object->m_obj->m_guid;
        CGItemText::SetItem(objectGuid, 0);
    }

    return 1;
}

// OFFSET: 0x0059A0C0
static bool OnGameObjectActivateAnimKit(ClientGameObjectActivateAnimKit* msg) {
    CGGameObject_C* object = static_cast<CGGameObject_C*>(ClntObjMgrObjectPtr(*reinterpret_cast<const WOWGUID*>(msg->m_objectGUID), TYPEMASK_GAMEOBJECT, __FILE__, __LINE__));

    if (object) {
        uint32_t animKitID = msg->m_animKitID;
        object->m_animKitID = animKitID;
        object->m_typeData->ActivateOneShotAnimKit(animKitID);
    }

    return true;
}

// PROVEN: ca51077f IDENTICAL/NONE
// OFFSET: 0x00599920
static void MapObjSequenceFinishedCallback(CM2Model* model, uint32_t boneId, uint32_t sequenceId, int32_t interrupted, uint32_t time, uint64_t param, int32_t a7) {
    if (!interrupted) {
        model->SetBoneSequence(0xFFFFFFFF, 0, 0xFFFFFFFF, 0, 1.0f, 1, 1, 0, 0, 0);
    }
}

// OFFSET: 0x005A2CF0
static void SetMapObjSpeed(uint32_t uniqueID, float speed) {
    uint32_t index = 0;
    uint32_t count = s_mapObjSpeedTransitions.Count();
    MapObjSpeedTransition* transition = NULL;
    float current;

    for (; index < count; index++) {
        if (s_mapObjSpeedTransitions[index].m_uniqueID == uniqueID) {
            break;
        }
    }

    if (index < count && index != 0xFFFFFFFF) {
        transition = &s_mapObjSpeedTransitions[index];

        if (transition->m_duration) {
            float t = 1.0f - static_cast<float>(static_cast<double>(transition->m_remaining) / static_cast<double>(transition->m_duration));
            current = t * transition->m_targetSpeed + (1.0f - t) * transition->m_startSpeed;
        } else {
            current = transition->m_targetSpeed;
        }
    } else {
        transition = s_mapObjSpeedTransitions.New();
        current = 1.0f;
    }

    transition->m_uniqueID = uniqueID;
    transition->m_startSpeed = current;
    transition->m_duration = s_mapObjSpeedTransitionTime;
    transition->m_remaining = s_mapObjSpeedTransitionTime;
    transition->m_targetSpeed = speed;
    transition->m_applied = 0;
}

// OFFSET: 0x005A2FB0
static bool OnMapObjEvents(ClientMapObjEvents* msg) {
    uintptr_t object = World::ObjectFind(msg->m_uniqueID);

    if (!object) {
        return true;
    }

    WDataStore& data = msg->m_data;
    uint8_t count = 0;
    data.Get(count);

    for (uint32_t i = count; i; i--) {
        uint8_t type = 0;
        data.Get(type);

        if (type == 0) {
            uint32_t sequence = 0;
            data.Get(sequence);
            World::ObjectSetSequence(object, sequence, 0, 0);
            World::ObjectSetSequenceFinishedCallback(object, MapObjSequenceFinishedCallback, 0, 0);
        } else if (type == 1) {
            uint32_t speed = 0;
            data.Get(speed);
            SetMapObjSpeed(msg->m_uniqueID, static_cast<float>(static_cast<int32_t>(speed)) * 0.01f);
        }
    }

    return true;
}

// OFFSET: 0x0059B4D0
static void UpdateMapObjSpeedsInternal() {
    int32_t elapsed = static_cast<int32_t>(CGWorldFrame::s_currentWorldFrame->m_elapsedSec * 1000.0f);

    for (uint32_t i = 0; i < s_mapObjSpeedTransitions.Count(); i++) {
        MapObjSpeedTransition& transition = s_mapObjSpeedTransitions[i];
        uintptr_t object = World::ObjectFind(transition.m_uniqueID);

        if (!object) {
            transition.m_applied = 0;
            continue;
        }

        uint32_t remaining = transition.m_remaining;

        if (!remaining && transition.m_applied) {
            continue;
        }

        remaining = static_cast<int32_t>(remaining) >= elapsed ? remaining - elapsed : 0;
        uint32_t duration = transition.m_duration;
        transition.m_remaining = remaining;
        float speed;

        if (duration) {
            double t = 1.0 - static_cast<double>(remaining) / static_cast<double>(duration);
            speed = static_cast<float>(t * transition.m_targetSpeed + (1.0 - t) * transition.m_startSpeed);
        } else {
            speed = transition.m_targetSpeed;
        }

        World::ObjectSetSequenceSpeed(object, speed, 0, 0);
        transition.m_applied = 1;
    }
}

// PROVEN: d3e5025a IDENTICAL/NONE
// OFFSET: 0x0059EC70
void CGGameObject_C::UpdateMapObjSpeeds() {
    UpdateMapObjSpeedsInternal();
}

// OFFSET: 0x00599E50/0x140208380
static void GameObjectModelEventCallback(CM2Model* model, uint32_t boneId, uint32_t eventId, uint32_t data, const C3Vector& position, uint32_t time, uint64_t param) {
    switch (eventId) {
        case 'VNE$': {
            CGGameObject_C* object = static_cast<CGGameObject_C*>(ClntObjMgrObjectPtr(WOWGUID::Make(param), TYPEMASK_GAMEOBJECT, __FILE__, __LINE__));

            if (object && object->IsTransport() && object->m_effectManager) {
                object->m_effectManager->EnterState(4, 1, 0);
                object->m_effectManager->ExitState(3, 1);
                object->m_effectManager->ExitState(2, 1);
                object->m_effectManager->ExitState(5, 1);
            }

            break;
        }

        case 'TWE$': {
            CGGameObject_C* object = static_cast<CGGameObject_C*>(ClntObjMgrObjectPtr(WOWGUID::Make(param), TYPEMASK_GAMEOBJECT, __FILE__, __LINE__));

            if (object && object->IsTransport() && object->m_effectManager) {
                object->m_effectManager->EnterState(2, 1, 0);
                object->m_effectManager->ExitState(3, 1);
                object->m_effectManager->ExitState(4, 1);
                object->m_effectManager->ExitState(5, 1);
            }

            break;
        }

        case 'CAE$': {
            CGGameObject_C* object = static_cast<CGGameObject_C*>(ClntObjMgrObjectPtr(WOWGUID::Make(param), TYPEMASK_GAMEOBJECT, __FILE__, __LINE__));

            if (object && object->IsTransport() && object->m_effectManager) {
                object->m_effectManager->EnterState(3, 1, 0);
                object->m_effectManager->ExitState(2, 1);
                object->m_effectManager->ExitState(4, 1);
                object->m_effectManager->ExitState(5, 1);
            }

            break;
        }

        case 'CDE$': {
            CGGameObject_C* object = static_cast<CGGameObject_C*>(ClntObjMgrObjectPtr(WOWGUID::Make(param), TYPEMASK_GAMEOBJECT, __FILE__, __LINE__));

            if (object && object->IsTransport() && object->m_effectManager) {
                object->m_effectManager->EnterState(5, 1, 0);
                object->m_effectManager->ExitState(3, 1);
                object->m_effectManager->ExitState(4, 1);
                object->m_effectManager->ExitState(2, 1);
            }

            break;
        }

    }
}

// OFFSET: 0x005A2C60/0x140212FB0
void CGGameObject_C::CheckForPassiveHighlight() {
    CGPlayer_C* player;

    if ((this->m_gameObject->m_dynamicFlags & 0x8) || ((player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__))) != NULL && player->CanTrackObject(this))) {
        this->m_modelFlags |= 0x400000;
        this->ShowLootEffect();
    } else {
        this->m_modelFlags &= ~0x400000;

        if (this->m_lootEffect) {
            if (this->m_lootEffect->m_attachParent) {
                this->m_lootEffect->DetachFromParent();
            }

            this->m_lootEffect->Release();
            this->m_lootEffect = NULL;
        }
    }
}

// PROVEN: 0932682d IDENTICAL/NONE
// OFFSET: 0x005A37C0
static int32_t OnGameObjectDynamicFlagsChanged(WOWGUID guid, uint32_t fieldOffset, uint32_t fieldSize, const void* oldValue, void* param) {
    CGGameObject_C* object = static_cast<CGGameObject_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_GAMEOBJECT, __FILE__, __LINE__));

    if ((static_cast<uint8_t>(object->m_gameObject->m_dynamicFlags) ^ *static_cast<const uint8_t*>(oldValue)) & 0x8) {
        object->CheckForPassiveHighlight();
    }

    return 1;
}

// OFFSET: 0x005A3650
void ApplyGameObjectArtKit(CM2Model* model, int32_t artKit) {
    GameObjectArtKitRec* rec = g_gameObjectArtKitDB.GetRecord(artKit);

    for (uint32_t i = 0; i < 4; i++) {
        model->DetachAllChildrenById(i);
    }

    if (rec) {
        CStatus status;
        CGxTexFlags flags(GxTex_LinearMipNearest, 1, 1, 0, 0, 1, 0, 0, 0);

        for (uint32_t i = 0; i < 3; i++) {
            if (*rec->m_textureVariation[i]) {
                CTexture* texture = TextureCreate(rec->m_textureVariation[i], flags, &status, 0, NULL, 2);

                if (texture) {
                    model->ReplaceTexture(i + 11, texture);
                    TextureRelease(texture);
                }
            }
        }

        for (uint32_t i = 0; i < 4; i++) {
            if (*rec->m_attachModel[i]) {
                CM2Model* child = model->m_scene->CreateModel(rec->m_attachModel[i], 0, 3);

                if (child) {
                    child->AttachToParent(model, i, NULL, 0);
                    child->Release();
                }
            }
        }
    }
}

// OFFSET: 0x005A3770
static int32_t OnUpdateArtKit(WOWGUID guid, uint32_t fieldOffset, uint32_t fieldSize, const void* oldValue, void* param) {
    CGGameObject_C* object = static_cast<CGGameObject_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_GAMEOBJECT, __FILE__, __LINE__));

    if (object->IsObjectModelLoaded()) {
        ApplyGameObjectArtKit(object->m_worldModel, object->m_gameObject->m_artKit);
    }

    return 1;
}

// PROVEN: 07eba844 IDENTICAL/NONE
// OFFSET: 0x00599B10
void CGGameObject_C::UpdateDisplay(uint32_t time) {
    this->m_typeData->UpdateDisplay(time);
}

// OFFSET: 0x005A4080/0x1402149E0
void CGGameObject_C::Initialize() {
    ClientGameObjectActivateAnimKit::s_handler = OnGameObjectActivateAnimKit;
    ClientServices::SetMessageHandler(SMSG_PAGE_TEXT, Packet_SMSG_GAMEOBJECT_PAGETEXT, NULL);
    ClientServices::SetMessageHandler(SMSG_GAME_OBJECT_CUSTOM_ANIM, Packet_SMSG_GAMEOBJECT_CUSTOM_ANIM, NULL);
    ClientServices::SetMessageHandler(SMSG_GAMEOBJECT_DESPAWN_ANIM, Packet_SMSG_GAMEOBJECT_DESPAWN_ANIM, NULL);
    ClntObjMgrSetTypeMirrorHandler(ID_GAMEOBJECT, offsetof(CGGameObjectData, m_state), sizeof(int8_t), CGGameObject_C::OnStateChanged, NULL, static_cast<HANDLER_PRIORITY>(0), 1);
    ClntObjMgrSetTypeMirrorHandler(ID_GAMEOBJECT, offsetof(CGGameObjectData, m_flags), sizeof(uint32_t), CGGameObject_C::OnFlagsChanged, NULL, static_cast<HANDLER_PRIORITY>(0), 0);
    ClntObjMgrSetTypeMirrorHandler(ID_GAMEOBJECT, offsetof(CGGameObjectData, m_artKit), sizeof(int8_t), OnUpdateArtKit, NULL, static_cast<HANDLER_PRIORITY>(0), 0);
    ClntObjMgrSetTypeMirrorHandler(ID_GAMEOBJECT, offsetof(CGGameObjectData, m_pathProgress), sizeof(uint16_t), CGGameObject_C::OnAnimProgressChanged, NULL, static_cast<HANDLER_PRIORITY>(0), 0);
    ClntObjMgrSetTypeMirrorHandler(ID_GAMEOBJECT, offsetof(CGGameObjectData, m_dynamicFlags), sizeof(uint16_t), OnGameObjectDynamicFlagsChanged, NULL, static_cast<HANDLER_PRIORITY>(0), 0);
    CGGameObject_C::s_cvUseAnimKits = CVar::Register("GameObjUseAnimKits", "0=off 1=on", 0x200, "1", CGGameObject_C::UseAnimKitsChangedCallback, CATEGORY_DEFAULT, false, NULL, false);
    CGGameObject_C::s_cvForceMouseOver = CVar::Register("GameObjForceMouseOver", "0=off 1=on", 0, "0", NULL, CATEGORY_DEFAULT, false, NULL, false);
    World::SetUserModelEventCallback(GameObjectModelEventCallback);
    ClientMapObjEvents::s_handler = OnMapObjEvents;
}

// PROVEN: d4313aca IDENTICAL/NONE
// OFFSET: 0x005A41C0
void CGGameObject_C::Shutdown() {
    ClientServices::ClearMessageHandler(SMSG_PAGE_TEXT);
    ClientServices::ClearMessageHandler(SMSG_GAME_OBJECT_CUSTOM_ANIM);
    ClientServices::ClearMessageHandler(SMSG_GAMEOBJECT_DESPAWN_ANIM);
    ClntObjMgrUnsetTypeMirrorHandler(ID_GAMEOBJECT, offsetof(CGGameObjectData, m_state), CGGameObject_C::OnStateChanged, NULL);
    ClntObjMgrUnsetTypeMirrorHandler(ID_GAMEOBJECT, offsetof(CGGameObjectData, m_flags), CGGameObject_C::OnFlagsChanged, NULL);
    ClntObjMgrUnsetTypeMirrorHandler(ID_GAMEOBJECT, offsetof(CGGameObjectData, m_artKit), OnUpdateArtKit, NULL);
    ClntObjMgrUnsetTypeMirrorHandler(ID_GAMEOBJECT, offsetof(CGGameObjectData, m_pathProgress), CGGameObject_C::OnAnimProgressChanged, NULL);
    ClntObjMgrUnsetTypeMirrorHandler(ID_GAMEOBJECT, offsetof(CGGameObjectData, m_dynamicFlags), OnGameObjectDynamicFlagsChanged, NULL);
    ClientMapObjEvents::s_handler = NULL;
}

// PROVEN: 6bcabb39 IDENTICAL/NONE
// OFFSET: 0x0059DC80
void CGGameObject_C::SequenceCallback(CM2Model* model, uint32_t boneId, uint32_t sequenceId, int32_t interrupted, uint32_t time, uint64_t param, int32_t a7) {
    CGGameObject_C* object = (CGGameObject_C*)ClntObjMgrObjectPtr(WOWGUID::Make(param), TYPEMASK_GAMEOBJECT, __FILE__, __LINE__);

    if (!object) {
        return;
    }

    if (CGGameObject_C::s_cvUseAnimKits && CGGameObject_C::s_cvUseAnimKits->m_intValue) {
        object->m_animKitManager.OnModelSequenceCallback(boneId, sequenceId, interrupted, a7);
    } else if (interrupted) {
        object->m_typeData->HandleAnimInterrupted();
    } else if (!CGGameObject_C::s_cvUseAnimKits || !CGGameObject_C::s_cvUseAnimKits->m_intValue) {
        object->m_typeData->HandleAnimFinished();
    }
}

// PROVEN: dcade347 IDENTICAL/NONE
// OFFSET: 0x0059B150
void CGGameObject_C::SequenceStartCallback(CM2Model* model, uint32_t boneId, uint32_t sequenceId, uint32_t variation, uint32_t time, uint64_t param, int32_t primary) {
    if (!CGGameObject_C::s_cvUseAnimKits || !CGGameObject_C::s_cvUseAnimKits->m_intValue) {
        return;
    }

    CGGameObject_C* object = (CGGameObject_C*)ClntObjMgrObjectPtr(WOWGUID::Make(param), TYPEMASK_GAMEOBJECT, __FILE__, __LINE__);

    if (object) {
        object->m_animKitManager.OnModelSequenceStartCallback(boneId);
    }
}

// PROVEN: e1f5c99d IDENTICAL/NONE
// OFFSET: 0x00599A30/0x140207DE0
void CGGameObject_C::AddToolTipInfo(CGTooltip* tooltip) {
    if (this->m_typeData) {
        this->m_typeData->AddToolTipInfo(tooltip);
    }
}

// PROVEN: 47bfd9d0 IDENTICAL/NONE
// OFFSET: 0x005A3B40
int32_t CGGameObject_C::AddPassenger(CPassenger* passenger, int32_t a2) {
    this->m_typeData->AddPassenger(passenger, a2);
    return 1;
}

// PROVEN: a26984e2 IDENTICAL/NONE
// OFFSET: 0x00599950
int32_t CGGameObject_C::Animate(float elapsed) {
    return this->m_typeData->Animate(elapsed) != 0;
}

// PROVEN: 671ca8cb IDENTICAL/NONE
// OFFSET: 0x0059B200/0x1402099F0
bool CGGameObject_C::CanUse() const {
    if (CGGameObject_C::s_cvForceMouseOver && CGGameObject_C::s_cvForceMouseOver->m_intValue) {
        return true;
    }

    return this->m_typeData->CanUse();
}

// PROVEN: 4a02f334 IDENTICAL/NONE
// OFFSET: 0x0059B220
bool CGGameObject_C::CanUseNow() const {
    if (CGGameObject_C::s_cvForceMouseOver && CGGameObject_C::s_cvForceMouseOver->m_intValue) {
        return true;
    }

    return this->m_typeData->CanUseNow(NULL, NULL, NULL);
}

// PROVEN: d0ead98d IDENTICAL/NONE
// OFFSET: 0x00599A20
int32_t CGGameObject_C::CheckRange() const {
    return this->m_typeData->CheckRange();
}

// PROVEN: 97716f6e IDENTICAL/NONE
// OFFSET: 0x00599AB0/0x140207E70
void CGGameObject_C::CloseInteraction() {
    this->m_typeData->CloseInteraction();
}

// OFFSET: 0x0059AED0/0x1402095F0
static int32_t DungeonDifficultyEnumProc(WOWGUID guid, void* param) {
    CGObject_C* object = ClntObjMgrObjectPtr(guid, TYPEMASK_OBJECT, __FILE__, __LINE__);

    if (object && (object->m_obj->m_type & TYPEMASK_GAMEOBJECT)) {
        CGGameObject_C* gameObject = static_cast<CGGameObject_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_GAMEOBJECT, __FILE__, __LINE__));
        gameObject->m_typeData->UpdateDisabled();
    }

    return 1;
}

// PROVEN: 5f829874 IDENTICAL/NONE
// OFFSET: 0x0059AF40/0x140209660
void CGGameObject_C::UpdateDungeonDifficulty() {
    ClntObjMgrEnumVisibleObjects(&DungeonDifficultyEnumProc, NULL);
}

// PROVEN: d225bd7f IDENTICAL/NONE
// OFFSET: 0x005999C0
float CGGameObject_C::GetBaseAlpha() const {
    return this->m_typeData->GetBaseAlpha();
}

// PROVEN: 1dd067f1 IDENTICAL/NONE
// OFFSET: 0x00599A80
const char* CGGameObject_C::GetCloseText() const {
    if (this->m_stats) {
        return this->m_stats->m_closeText;
    }

    return NULL;
}

// PROVEN: 83ceab92 IDENTICAL/NONE
// OFFSET: 0x0059EBD0
void CGGameObject_C::GetCursorFile(char* buffer, uint32_t size) const {
    if (!this->m_stats) {
        return;
    }

    if ((CGGameObject_C::s_cvForceMouseOver && CGGameObject_C::s_cvForceMouseOver->m_intValue) || this->m_typeData->CanUseNow(NULL, NULL, NULL)) {
        SStrPrintf(buffer, size, "Interface\\Cursor\\%s.blp", this->m_stats->m_iconName);
    } else {
        SStrPrintf(buffer, size, "Interface\\Cursor\\Unable%s.blp", this->m_stats->m_iconName);
    }
}

// OFFSET: 0x00599AC0/0x140207E90
float CGGameObject_C::GetInteractDistanceSquared() const {
    if (!this->m_typeData) {
        return 0.0f;
    }

    CGGameObject_C_TypeBase* type = this->m_typeData;
    float distance = type->GetInteractDistance();
    return type->GetInteractDistance() * distance;
}

// PROVEN: 80a527a9 IDENTICAL/NONE
// OFFSET: 0x005A3B70
C44Matrix CGGameObject_C::GetMatrix() const {
    return this->m_matrix;
}

// OFFSET: 0x0059E110/0x14020D2C0
const char* CGGameObject_C::GetModelFileNameInternal() const {
    int32_t displayID = this->m_gameObject->m_displayID;

    if (!displayID) {
        OsOutputDebugString("Game object id %d (type %s) has a NULL display ID", this->m_obj->m_entryID, CGameObjectDef::NameFromTypeId(this->m_gameObject->m_typeID));
        return NULL;
    }

    GameObjectDisplayInfoRec* rec = g_gameObjectDisplayInfoDB.GetRecord(displayID);

    if (rec) {
        return rec->m_modelName;
    }

    SysMsgPrintf(STATUS_ERROR, 2, "Game object id %d is missing its display record %d", this->m_obj->m_entryID, displayID);
    return NULL;
}

// PROVEN: e716e6de IDENTICAL/NONE
// OFFSET: 0x0059E190
int32_t CGGameObject_C::GetModelFileName(const char** name) const {
    switch (this->m_gameObject->m_typeID) {
        case 11:
        case 14:
        case 15:
        case 33:
        case 35: {
            return 0;
        }

        default: {
            const char* file = this->GetModelFileNameInternal();
            *name = file;
            return file != NULL;
        }
    }
}

// PROVEN: 14710a12 IDENTICAL/NONE
// OFFSET: 0x0059B1E0/0x1402099D0
const char* CGGameObject_C::GetObjectName() const {
    if (this->m_stats) {
        return this->m_stats->m_name[0];
    }

    return "";
}

// PROVEN: edc928ef IDENTICAL/NONE
// OFFSET: 0x00599A60
const char* CGGameObject_C::GetOpenText() const {
    if (this->m_stats) {
        return this->m_stats->m_castBarCaption;
    }

    return NULL;
}

// PROVEN: 8f49abbd IDENTICAL/NONE
// OFFSET: 0x005999D0
uint32_t CGGameObject_C::GetPropertyValue(uint32_t index) const {
    if (this->m_stats && index < 32) {
        return this->m_stats->m_propValue[index];
    }

    return 0;
}

// OFFSET: 0x005A39E0
C3Vector CGGameObject_C::GetRawPosition() const {
    C3Vector position = this->m_typeData ? this->m_typeData->GetRawPosition() : this->m_passenger.m_position;
    return position;
}

// PROVEN: 8f231009 IDENTICAL/NONE
// OFFSET: 0x005A3AD0
ShipPath* CGGameObject_C::GetShipPath() const {
    if (this->m_typeData) {
        return this->m_typeData->GetShipPath();
    }

    return NULL;
}

// OFFSET: 0x005A3B90
float CGGameObject_C::GetSpeed() const {
    if (this->m_typeData) {
        return this->m_typeData->GetSpeed();
    }

    return -1.0f;
}

// PROVEN: 8de7bad7 IDENTICAL/NONE
// OFFSET: 0x00598B70/0x1402142E0
uint64_t CGGameObject_C::GetTransportGUID() const {
    return this->m_passenger.m_transportGuid.m_guid;
}

// PROVEN: 7ea7425c IDENTICAL/NONE
// OFFSET: 0x005A3B10
int32_t CGGameObject_C::IsHighlightSuppressed() const {
    return this->m_typeData->IsHighlightSuppressed();
}

// PROVEN: f6ebc09e IDENTICAL/NONE
// OFFSET: 0x005A3B20
int32_t CGGameObject_C::IsPointInside(const C3Vector& point) const {
    return this->m_typeData->IsPointInside(point);
}

// PROVEN: 4ce42d85 IDENTICAL/NONE
// OFFSET: 0x0059EC50
int32_t CGGameObject_C::IsQuestGiver() const {
    return this->m_gameObject->m_typeID == 2;
}

// OFFSET: 0x005A3AF0
int32_t CGGameObject_C::IsSolidSelectable() const {
    if (this->m_hasWorldBounds) {
        return 1;
    }

    if (this->CanHighlight()) {
        return 1;
    }

    return 0;
}

// PROVEN: d4f7834d IDENTICAL/NONE
// OFFSET: 0x005A3BD0
bool CGGameObject_C::IsTransport() const {
    return (this->m_gameObject->m_flags >> 3) & 1;
}

// OFFSET: 0x0059E2A0/0x14020D460
bool CGGameObject_C::IsValidOpenAction(int32_t action) const {
    if (action == 4) {
        return this->m_displayState != 2;
    }

    int32_t state = this->m_displayState;

    if (state == 2) {
        return false;
    }

    if ((action == 0 || action == 1 || action == 3) && state != 1) {
        return false;
    }

    if (action == 0) {
        return !(this->m_gameObject->m_flags & 2);
    }

    if (action == 1) {
        if (!(this->m_gameObject->m_flags & 2)) {
            return false;
        }
    } else if (action == 2 && state) {
        return false;
    }

    return true;
}

// OFFSET: 0x0059B3D0
void CGGameObject_C::PlayCustomAnim(uint32_t anim) {
    uint32_t pending = anim + 2;

    if (this->IsObjectModelLoaded()) {
        this->m_typeData->ActivateOneShotAnim(pending);
        this->m_pendingOneShotAnim = 0;
    } else {
        this->m_pendingOneShotAnim = pending;
    }
}

// PROVEN: 1896237a IDENTICAL/NONE
// OFFSET: 0x0059ECC0
void CGGameObject_C::ResetDisplayState() {
    int32_t state = this->m_gameObject->m_state;
    int32_t oldState = this->m_displayState;

    if (state != oldState || this->m_gameObject->m_typeID == 15) {
        this->m_displayState = state;

        if (this->m_typeData) {
            this->m_typeData->UpdateState(oldState, state);
        }
    }
}

// PROVEN: 01492e39 IDENTICAL/NONE
// OFFSET: 0x0059EC80
void CGGameObject_C::SetDisplayState(int32_t state) {
    int32_t oldState = this->m_displayState;

    if (state != oldState || this->m_gameObject->m_typeID == 15) {
        this->m_displayState = state;

        if (this->m_typeData) {
            this->m_typeData->UpdateState(oldState, state);
        }
    }
}

// OFFSET: 0x0059B390
void CGGameObject_C::SetPathProgress(float progress) {
    this->m_gameObject->m_pathProgress = (int32_t)(float)(progress * 65535.0f);
}

// PROVEN: 9c5ea5e7 IDENTICAL/NONE
// OFFSET: 0x00599A00
int32_t CGGameObject_C::ShouldFadeIn() const {
    return this->m_typeData->ShouldFadeIn();
}

// PROVEN: 47635946 IDENTICAL/NONE
// OFFSET: 0x00599980
void CGGameObject_C::ShouldRender(uint32_t flags, int32_t& render, int32_t& a3) {
    CGObject_C::ShouldRender(flags, render, a3);
    this->m_typeData->ShouldRender(flags, render, a3);
}

// PROVEN: a241bb95 IDENTICAL/NONE
// OFFSET: 0x00599AA0/0x140207E50
void CGGameObject_C::StartInteraction() {
    this->m_typeData->StartInteraction();
}

// PROVEN: cfd8eb7a IDENTICAL/NONE
// OFFSET: 0x0059E800
int32_t CGGameObject_C::IsSolidCollidable(uint32_t flags) const {
    if ((flags & 0x8000) && !this->m_gameObject->m_typeID) {
        return 0;
    }

    return this->m_hasWorldBounds;
}

// PROVEN: 51a47f0b IDENTICAL/NONE
// OFFSET: 0x0059B250
void CGGameObject_C::UpdateInteractIconAttach() {
    if (!this->m_interactIcon) {
        return;
    }

    CM2Model* worldModel = this->m_worldModel;

    if (!worldModel || !worldModel->IsLoaded()) {
        return;
    }

    if (this->m_interactIcon->m_attachParent) {
        this->m_interactIcon->DetachFromParent();
    }

    if (!this->m_worldModel->HasAttachment(18)) {
        return;
    }

    this->m_interactIcon->AttachToParent(this->m_worldModel, 18, NULL, 0);
    C44Matrix transform = this->m_worldModel->GetAttachmentWorldTransform(18);

    if (transform.c1 < 0.99000001f) {
        C44Matrix inverse = transform.AffineInverse();
        inverse.d0 = 0.0f;
        inverse.d1 = 0.0f;
        inverse.d2 = 0.0f;
        this->m_interactIcon->SetWorldTransform(inverse);
    }

    this->UpdateInteractIconScale();
    this->ObjectNameVisibilityChanged();
}

// INLINED
CGGameObject_C_TypeBase::CGGameObject_C_TypeBase(CGGameObject_C* owner, const CClientObjCreate& create) {
    this->m_owner = owner;
    this->m_useErrorType = 243;
    this->m_interactDistance = 5.0f;
}

// OFFSET: 0x0059C030/0x14020B100
CGGameObject_C_TypeBase::~CGGameObject_C_TypeBase() {
}

// PROVEN: 6a18318b IDENTICAL/NONE
// OFFSET: 0x00599E40/0x140824640
float CGGameObject_C_TypeBase::GetInteractDistance() const {
    return this->m_interactDistance;
}

// OFFSET: 0x0059A550/0x140208CF0
C3Vector CGGameObject_C_TypeBase::GetRawPosition() const {
    return this->m_owner->m_passenger.m_position;
}

// INLINED
CGGameObject_C_Type_Null::CGGameObject_C_Type_Null(CGGameObject_C* owner, const CClientObjCreate& create) : CGGameObject_C_TypeBase(owner, create) {
}

// PROVEN: b303883d IDENTICAL/NONE
// OFFSET: 0x0059A5B0/0x140208D60
const char* CGGameObject_C_Type_Null::DebugStatus() {
    return "Unknown Object Type";
}

// OFFSET: #0x007124B0
CGGameObject_C_TypeAnimated::CGGameObject_C_TypeAnimated(CGGameObject_C* owner, const CClientObjCreate& create) : CGGameObject_C_TypeBase(owner, create), m_animState(-1), m_pendingAnimState(-1), m_nextAnimTime(-1) {
    this->ResetAnimState();
}

// OFFSET: 0x005A2590/0x140211F40
CGGameObject_C_TypeAnimated::~CGGameObject_C_TypeAnimated() {
    this->CloseLoopingSound();
}

// OFFSET: 0x005A1B70/0x140211C50
void CGGameObject_C_TypeAnimated::ActivateOneShotAnim(uint32_t anim) {
    if (CGGameObject_C::s_cvUseAnimKits && CGGameObject_C::s_cvUseAnimKits->m_intValue) {
        if (!this->PlayOneShotAnimKit(CGGameObject_C_TypeAnimated::s_oneShotAnimKitIds[anim], CGGameObject_C_TypeAnimated::s_oneShotAnimKitAnims[anim])) {
            return;
        }
    } else {
        uint32_t state = s_oneShotAnims[anim];

        if (!this->m_owner->m_worldModel->HasSequence(CGGameObject_C_TypeAnimated::s_animSequenceIds[state])) {
            return;
        }

        this->UpdateAnimState(state);
    }

    if (anim == 1) {
        this->m_owner->DoFade(this->m_owner->GetBaseAlpha(), 0);
    }
}

// PROVEN: d9f16295 IDENTICAL/NONE
// OFFSET: 0x0059A720/0x140208F10
const char* CGGameObject_C_TypeAnimated::DebugStatus() {
    if (CGGameObject_C::s_cvUseAnimKits && CGGameObject_C::s_cvUseAnimKits->m_intValue) {
        return CGGameObject_C_TypeAnimated::s_displayStateNames[this->m_owner->m_displayState];
    }

    return CGGameObject_C_TypeAnimated::s_animStateNames[this->m_animState];
}

// PROVEN: 1995b3da IDENTICAL/NONE
// OFFSET: 0x0059A5C0/0x140208D70
void CGGameObject_C_TypeAnimated::Disable() {
    this->CloseLoopingSound();
}

// OFFSET: 0x0059BD70/0x14020A7B0
int32_t CGGameObject_C_TypeAnimated::Animate(float elapsed) {
    CGGameObject_C* owner = this->m_owner;

    if (!(owner->m_modelFlags & 0x100000)) {
        return 1;
    }

    CM2Model* model = owner->m_worldModel;

    if (!model || !model->IsLoaded() || model->m_animatePrev) {
        return 1;
    }

    if (CGGameObject_C::s_cvUseAnimKits && CGGameObject_C::s_cvUseAnimKits->m_intValue) {
        if (this->m_owner->m_animKitManager.IsFrozen() || this->IsIdle()) {
            return this->UpdateObjectLock();
        }

        return 1;
    }

    uint32_t time = CGWorldFrame::s_currentWorldFrame->GetSceneTime();

    if (model->m_lastAnimTime) {
        return this->HandleAnimInterrupted();
    }

    if (this->m_nextAnimTime != -1 && static_cast<int32_t>(time - this->m_nextAnimTime) >= 0) {
        return this->HandleAnimInterrupted();
    }

    return 1;
}

// OFFSET: 0x0059B6E0/0x140209FF0
void CGGameObject_C_TypeAnimated::OnUpdateFlags(uint32_t changed) {
    if (!(changed & 0x80)) {
        return;
    }

    CM2Model* model = this->m_owner->m_worldModel;

    if (!model || !model->IsLoaded()) {
        return;
    }

    if (CGGameObject_C::s_cvUseAnimKits && CGGameObject_C::s_cvUseAnimKits->m_intValue) {
        if (!this->m_stateKit) {
            return;
        }
    } else {
        int32_t state = this->m_animState;

        if (state != 2 && state != 4 && state != 5 && state != 7) {
            return;
        }
    }

    this->SetAnimPaused(model, (this->m_owner->m_gameObject->m_flags >> 7) & 1);
}

// OFFSET: 0x005A1AE0/0x140211BA0
void CGGameObject_C_TypeAnimated::PostInit(bool a1) {
    if (CGGameObject_C::s_cvUseAnimKits && CGGameObject_C::s_cvUseAnimKits->m_intValue) {
        this->ResetAnimState();
    }

    if (!a1) {
        return;
    }

    CGGameObject_C* owner = this->m_owner;

    if (owner->IsObjectModelLoaded()) {
        owner->m_typeData->ActivateOneShotAnim(1);
        owner->m_pendingOneShotAnim = 0;
    } else {
        owner->m_pendingOneShotAnim = 1;
    }
}

// PROVEN: 46dc8c9d IDENTICAL/NONE
// OFFSET: 0x005A1B40/0x140211C20
void CGGameObject_C_TypeAnimated::PostReenable() {
    CM2Model* model = this->m_owner->m_worldModel;

    if (model && model->IsLoaded()) {
        this->ResetAnimState();
    }
}

// OFFSET: 0x0059F870/0x14020F010
void CGGameObject_C_TypeAnimated::ResetAnimState() {
    this->m_animState = -1;
    this->m_pendingAnimState = -1;
    this->m_nextAnimTime = -1;

    if (CGGameObject_C::s_cvUseAnimKits && CGGameObject_C::s_cvUseAnimKits->m_intValue) {
        this->StopAnimKits();
        this->PlayStateAnimKits(this->m_owner->m_gameObject->m_pathProgress != 0xFFFF);

        if (this->m_owner->m_animKitID) {
            this->ActivateOneShotAnimKit(this->m_owner->m_animKitID);
        }

        return;
    }

    CGGameObject_C* owner = this->m_owner;

    switch (owner->m_displayState) {
        case 0:
            this->UpdateAnimState((owner->m_gameObject->m_pathProgress == 0xFFFF) + 2);
            break;

        case 1:
            this->UpdateAnimState(owner->m_gameObject->m_pathProgress != 0xFFFF ? 4 : 1);
            break;

        case 2:
            this->UpdateAnimState((owner->m_gameObject->m_pathProgress == 0xFFFF) + 5);
            break;

        default:
            OsOutputDebugString("FIXME: Unhandled display state");
            this->UpdateAnimState(-1);
            break;
    }
}

// PROVEN: 4b31ddf3 IDENTICAL/NONE
// OFFSET: 0x0059A5E0/0x140208DA0
bool CGGameObject_C_TypeAnimated::ShouldFadeIn() const {
    if (!CGGameObject_C::s_cvUseAnimKits || !CGGameObject_C::s_cvUseAnimKits->m_intValue) {
        return this->m_animState != 0;
    }

    return !this->m_oneShotKit || this->m_oneShotKit->GetID() != 1577;
}

// OFFSET: 0x00599140/0x1402075B0
void CGGameObject_C_TypeAnimated::UpdateDisplay(uint32_t time) {
    SE3::Set3DPosition(this->m_sound, this->GetPosition());
}

// OFFSET: 0x0059F990/0x14020F1C0
void CGGameObject_C_TypeAnimated::UpdateState(int32_t oldState, int32_t newState) {
    if (this->m_owner->m_modelFlags & 0x20000) {
        return;
    }

    this->CloseLoopingSound();

    if (CGGameObject_C::s_cvUseAnimKits && CGGameObject_C::s_cvUseAnimKits->m_intValue) {
        this->PlayStateAnimKits(1);
        this->UpdateSolidity();
        return;
    }

    switch (newState) {
        case 0:
            if (oldState == 1 || this->m_owner->m_gameObject->m_pathProgress != 0xFFFF) {
                this->UpdateAnimState(2);
            } else {
                this->UpdateAnimState(3);
            }

            break;

        case 1: {
            int32_t state = this->m_owner->m_gameObject->m_pathProgress == 0xFFFF ? oldState : 0;

            if (!state) {
                this->UpdateAnimState(4);
            } else if (state == 2) {
                this->UpdateAnimState(7);
            } else {
                this->UpdateAnimState(1);
            }

            break;
        }

        case 2:
            if (oldState == 1 || this->m_owner->m_gameObject->m_pathProgress != 0xFFFF) {
                this->UpdateAnimState(5);
            } else {
                this->UpdateAnimState(6);
            }

            break;
    }
}

// PROVEN: 30f8e5fc IDENTICAL/NONE
// OFFSET: 0x005A1C00/0x140211CE0
void CGGameObject_C_TypeAnimated::ActivateOneShotAnimKit(uint32_t animKitID) {
    if (CGGameObject_C::s_cvUseAnimKits && CGGameObject_C::s_cvUseAnimKits->m_intValue) {
        this->PlayOneShotAnimKit(animKitID, 0xFFFFFFFF);
    }
}

// PROVEN: a68a74e1 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void CGGameObject_C_TypeAnimated::UpdateSolidity() {
}

// INLINED
void CGGameObject_C_TypeAnimated::CloseLoopingSound() {
    SE3::StopSound(this->m_sound, -1, __FILE__, __LINE__, "CGGameObject_C_TypeAnimated::CloseLoopingSound");
}

// PROVEN: 166b2aec IDENTICAL/NONE
// OFFSET: 0x00599170
bool CGGameObject_C_TypeAnimated::IsIdle() const {
    return this->m_baseKit && !this->m_stateKit && !this->m_oneShotKit;
}

// PROVEN: cd691e9d IDENTICAL/NONE
// OFFSET: 0x0059A620/0x140208DF0
void CGGameObject_C_TypeAnimated::SetAnimPaused(CM2Model* model, int32_t paused) {
    if (CGGameObject_C::s_cvUseAnimKits && CGGameObject_C::s_cvUseAnimKits->m_intValue) {
        CAnimKitManager* manager = &this->m_owner->m_animKitManager;

        if (paused) {
            manager->Freeze(0xFFFFFFFF);
        } else {
            manager->Unfreeze();
        }
    } else if (paused) {
        uint32_t time = model->m_scene->m_time;
        model->m_lastAnimTime = time ? time : 1;
    } else {
        model->m_lastAnimTime = 0;
    }
}

// PROVEN: 03018c2c IDENTICAL/NONE
// OFFSET: 0x0059A680
void CGGameObject_C_TypeAnimated::StopAnimKits() {
    if (this->m_baseKit) {
        this->m_baseKit->SetStopCallback(NULL, 0);
        this->m_baseKit->Stop(1, NULL);

        if (this->m_baseKit) {
            this->m_baseKit->Release();
            this->m_baseKit = NULL;
        }
    }

    if (this->m_stateKit) {
        this->m_stateKit->SetStopCallback(NULL, 0);
        this->m_stateKit->Stop(1, NULL);

        if (this->m_stateKit) {
            this->m_stateKit->Release();
            this->m_stateKit = NULL;
        }
    }

    if (this->m_oneShotKit) {
        this->m_oneShotKit->SetStopCallback(NULL, 0);
        this->m_oneShotKit->Stop(1, NULL);

        if (this->m_oneShotKit) {
            this->m_oneShotKit->Release();
            this->m_oneShotKit = NULL;
        }
    }
}

// OFFSET: 0x0059BAF0/0x14020A4D0
int32_t CGGameObject_C_TypeAnimated::UpdateObjectLock() {
    if (CGGameObject_C::s_cvUseAnimKits && CGGameObject_C::s_cvUseAnimKits->m_intValue) {
        if ((!this->m_baseKit || this->m_stateKit || this->m_oneShotKit) && !this->m_owner->m_animKitID) {
            this->m_owner->SetObjectLocked(1);
            return 1;
        }

        CGGameObject_C* owner = this->m_owner;

        if (owner->m_animLocked) {
            owner->m_animLocked = 0;
            ClntObjMgrUnlockObject(owner->m_obj->m_guid, __FILE__, __LINE__);
        }

        return 0;
    }

    int32_t pending = this->m_pendingAnimState;

    if (pending == -1 || pending == 3 || pending == 1 || pending == 6) {
        return 1;
    }

    ClntObjMgrUnlockObject(this->m_owner->m_obj->m_guid, __FILE__, __LINE__);
    return 0;
}

// PROVEN: 08ed79d9 IDENTICAL/NONE
// OFFSET: 0x0059BBA0
void CGGameObject_C_TypeAnimated::BaseAnimKitStopped(CAnimKitInst* inst, uint64_t param) {
    CGGameObject_C_TypeAnimated* animated = reinterpret_cast<CGGameObject_C_TypeAnimated*>(static_cast<uintptr_t>(param));

    if (!animated) {
        return;
    }

    if (inst == animated->m_baseKit && animated->m_baseKit) {
        animated->m_baseKit->SetStopCallback(NULL, 0);
        animated->m_baseKit->Stop(1, NULL);

        if (animated->m_baseKit) {
            animated->m_baseKit->Release();
            animated->m_baseKit = NULL;
        }
    }

    animated->UpdateObjectLock();
    animated->UpdateSolidity();
}

// PROVEN: 0a6b4656 IDENTICAL/NONE
// OFFSET: 0x0059BC00
void CGGameObject_C_TypeAnimated::StateAnimKitStopped(CAnimKitInst* inst, uint64_t param) {
    CGGameObject_C_TypeAnimated* animated = reinterpret_cast<CGGameObject_C_TypeAnimated*>(static_cast<uintptr_t>(param));

    if (!animated) {
        return;
    }

    if (inst == animated->m_stateKit && animated->m_stateKit) {
        animated->m_stateKit->SetStopCallback(NULL, 0);
        animated->m_stateKit->Stop(1, NULL);

        if (animated->m_stateKit) {
            animated->m_stateKit->Release();
            animated->m_stateKit = NULL;
        }
    }

    animated->UpdateObjectLock();
    animated->UpdateSolidity();
}

// OFFSET: 0x0059F4A0/0x14020EC30
void CGGameObject_C_TypeAnimated::PlayStateAnimKits(int32_t transition) {
    if (!CGGameObject_C::s_cvUseAnimKits || !CGGameObject_C::s_cvUseAnimKits->m_intValue) {
        return;
    }

    if (this->m_owner->m_animKitID) {
        return;
    }

    uint32_t baseID = this->m_baseKit ? this->m_baseKit->GetID() : 0;
    uint32_t stateID = 0;
    float stateProgress = 0.0f;

    if (this->m_stateKit) {
        stateID = this->m_stateKit->GetID();
        stateProgress = this->m_stateKit->GetSegmentProgress(0);
    }

    uint32_t transitionKit = 0;
    uint32_t idleKit = 0;

    switch (this->m_owner->m_displayState) {
        case 0:
            transitionKit = 1585;
            idleKit = 1576;
            break;

        case 1:
            idleKit = 1590;
            transitionKit = 2 * (baseID == 1591) + 1592;
            break;

        case 2:
            transitionKit = 1593;
            idleKit = 1591;
            break;
    }

    if (this->m_baseKit && this->m_baseKit->GetID() == idleKit) {
        return;
    }

    if (transition && transitionKit && this->m_stateKit && this->m_stateKit->GetID() == transitionKit) {
        return;
    }

    if (this->m_baseKit) {
        this->m_baseKit->SetStopCallback(NULL, 0);
        this->m_baseKit->Stop(1, NULL);

        if (this->m_baseKit) {
            this->m_baseKit->Release();
            this->m_baseKit = NULL;
        }
    }

    if (this->m_stateKit) {
        this->m_stateKit->SetStopCallback(NULL, 0);
        this->m_stateKit->Stop(1, NULL);

        if (this->m_stateKit) {
            this->m_stateKit->Release();
            this->m_stateKit = NULL;
        }
    }

    if (this->m_oneShotKit) {
        this->m_oneShotKit->SetStopCallback(NULL, 0);
        this->m_oneShotKit->Stop(1, NULL);

        if (this->m_oneShotKit) {
            this->m_oneShotKit->Release();
            this->m_oneShotKit = NULL;
        }
    }

    if (transition && transitionKit) {
        int32_t reverse = 0;

        if (this->m_stateKit) {
            switch (transitionKit) {
                case 1585:
                    reverse = stateID == 1592;
                    break;

                case 1592:
                    reverse = stateID == 1585;
                    break;

                case 1593:
                    reverse = stateID == 1594;
                    break;

                default:
                    reverse = transitionKit == 1594 && stateID == 1593;
                    break;
            }
        }

        AnimKitExtraPlayParams params;
        params.boneSetID = -1;
        params.startTime = 0;
        params.startFraction = reverse ? 1.0f - stateProgress : 0.0f;
        params.oneShot = 0;
        params.blendTimeMode = 0;
        params.blendTime = 0;
        this->m_stateKit = this->m_owner->m_animKitManager.PlayAndMaintain(transitionKit, 0xFFFFFFFF, &params);

        if (this->m_stateKit) {
            if (this->m_stateKit->IsStoppedOrPendingDestroy()) {
                this->m_stateKit->Release();
                this->m_stateKit = NULL;
            } else {
                this->m_stateKit->SetStopCallback(&CGGameObject_C_TypeAnimated::StateAnimKitStopped, reinterpret_cast<uintptr_t>(this));
            }
        }
    }

    if (idleKit) {
        this->m_baseKit = this->m_owner->m_animKitManager.PlayAndMaintain(idleKit, 0xFFFFFFFF, NULL);

        if (this->m_baseKit) {
            this->m_baseKit->SetStopCallback(&CGGameObject_C_TypeAnimated::BaseAnimKitStopped, reinterpret_cast<uintptr_t>(this));
        }
    }

    this->UpdateObjectLock();
    this->UpdateSolidity();
}

// OFFSET: 0x0059F760/0x14020EF70
void CGGameObject_C_TypeAnimated::OneShotAnimKitStopped(CAnimKitInst* inst, uint64_t param) {
    CGGameObject_C_TypeAnimated* animated = reinterpret_cast<CGGameObject_C_TypeAnimated*>(static_cast<uintptr_t>(param));

    if (!animated) {
        return;
    }

    uint32_t id = inst->GetID();

    if (inst == animated->m_oneShotKit && animated->m_oneShotKit) {
        animated->m_oneShotKit->SetStopCallback(NULL, 0);
        animated->m_oneShotKit->Stop(1, NULL);

        if (animated->m_oneShotKit) {
            animated->m_oneShotKit->Release();
            animated->m_oneShotKit = NULL;
        }
    }

    if (inst && id == animated->m_owner->m_animKitID) {
        animated->m_owner->m_animKitID = 0;
    }

    if (id == 1589) {
        AnimKitExtraPlayParams params;
        params.boneSetID = -1;
        params.startTime = 0;
        params.startFraction = 1.0f;
        params.oneShot = 0;
        params.blendTimeMode = 0;
        params.blendTime = 0;
        animated->m_baseKit = animated->m_owner->m_animKitManager.PlayAndMaintain(1589, 157, &params);

        if (animated->m_baseKit) {
            animated->m_baseKit->SetSpeed(0, 1, 0.0f);
        }
    } else {
        animated->PlayStateAnimKits(animated->m_owner->m_gameObject->m_pathProgress != 0xFFFF);
    }

    animated->UpdateObjectLock();
    animated->UpdateSolidity();
}

// OFFSET: 0x0059FAE0/0x14020F300
bool CGGameObject_C_TypeAnimated::PlayOneShotAnimKit(uint32_t animKitID, uint32_t anim) {
    if (!CGGameObject_C::s_cvUseAnimKits || !CGGameObject_C::s_cvUseAnimKits->m_intValue) {
        return false;
    }

    uint32_t kitID = this->m_owner->m_animKitID;

    if (kitID) {
        if (this->m_oneShotKit && this->m_oneShotKit->GetID() == kitID) {
            return true;
        }
    } else {
        kitID = animKitID;
    }

    if (this->m_oneShotKit) {
        this->m_oneShotKit->Stop(1, NULL);

        if (this->m_oneShotKit) {
            this->m_oneShotKit->Release();
            this->m_oneShotKit = NULL;
        }
    }

    this->m_oneShotKit = this->m_owner->m_animKitManager.PlayAndMaintain(kitID, anim, NULL);

    if (this->m_oneShotKit) {
        if (this->m_oneShotKit->IsStoppedOrPendingDestroy()) {
            this->m_oneShotKit->Release();
            this->m_oneShotKit = NULL;
        } else {
            this->m_oneShotKit->SetStopCallback(&CGGameObject_C_TypeAnimated::OneShotAnimKitStopped, reinterpret_cast<uintptr_t>(this));
        }
    }

    if (this->m_oneShotKit) {
        if (this->m_baseKit) {
            this->m_baseKit->Stop(1, NULL);

            if (this->m_baseKit) {
                this->m_baseKit->Release();
                this->m_baseKit = NULL;
            }
        }

        if (this->m_stateKit) {
            this->m_stateKit->Stop(1, NULL);

            if (this->m_stateKit) {
                this->m_stateKit->Release();
                this->m_stateKit = NULL;
            }
        }
    } else {
        this->PlayStateAnimKits(0);
    }

    this->UpdateObjectLock();
    this->UpdateSolidity();
    return this->m_oneShotKit != NULL;
}

// PROVEN: 38c7dec2 IDENTICAL/NONE
// OFFSET: 0x005A2640
bool CGGameObject_C_Type_FishingNode::CanUse() const {
    CGObject_C* player = ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__);

    if (player && *(const WOWGUID*)&((CGUnit_C*)player)->m_unit->m_unitFieldChannelObject == this->m_owner->m_obj->m_guid) {
        return CGGameObject_C_TypeBase::CanUse();
    }

    return false;
}

// PROVEN: 85a39f49 IDENTICAL/NONE
// OFFSET: 0x00599900/0x140207C90
int32_t CGGameObject_C_Type_GuildBank::CanChangeCursor() const {
    return this->CanUseNow(NULL, NULL, NULL) ? 3 : 29;
}

// PROVEN: 6590c841 IDENTICAL/NONE
// OFFSET: 0x0059A7B0/0x140208F60
void CGGameObject_C_Type_TransportBase::Reenable() {
    this->m_isLoaded = 0;
    this->m_transportState = -1;
}

// PROVEN: 6aae4f10 IDENTICAL/NONE
// OFFSET: 0x005A2F80/0x1402134C0
C3Vector CGGameObject_C_Type_TransportBase::GetRawPosition() const {
    return this->m_position;
}

// PROVEN: f91804e5 IDENTICAL/NONE
// OFFSET: 0x005A2FA0/0x1402134E0
float CGGameObject_C_Type_TransportBase::GetSpeed() const {
    return this->m_speed;
}

// PROVEN: 797c7138 IDENTICAL/IDENTICAL
// OFFSET: 0x00B3EAA0/0x140902360
uint32_t CGGameObject_C_Type_TransportBase::GetTransportUpdateTime() const {
    return this->m_transportUpdateTime;
}

// PROVEN: 97f39fea IDENTICAL/DIFFERENT
// OFFSET: 0x005A21E0/0x1402123D0
void CGGameObject_C_Type_MapObjTransport::AddPassenger(CPassenger* passenger, int32_t a2) {
    CGGameObject_C_Type_TransportBase::AddPassenger(passenger, a2);
}

// OFFSET: 0x005A5010/0x140215ED0
CGGameObject_C_Type_MapObjTransport::~CGGameObject_C_Type_MapObjTransport() {
}

// OFFSET: 0x005A4550/0x1402150A0
CGGameObject_C_Type_MapObjTransport::CGGameObject_C_Type_MapObjTransport(CGGameObject_C* owner, const CClientObjCreate& create) : CGGameObject_C_Type_TransportBase(owner, create), m_currentLeg(0) {
}

// PROVEN: 9cff0fde IDENTICAL/NONE
// OFFSET: 0x005A45C0/0x140215130
ShipPath* CGGameObject_C_Type_MapObjTransport::GetShipPath() const {
    return (ShipPath*)&this->m_shipPath;
}

// INLINED
CGGameObject_C_Type_WorldObj::CGGameObject_C_Type_WorldObj(CGGameObject_C* owner, const CClientObjCreate& create) : CGGameObject_C_TypeBase(owner, create) {
    this->m_worldObject = NULL;
}

// PROVEN: 79887fb8 IDENTICAL/NONE
// OFFSET: 0x005991B0/0x140207600
void CGGameObject_C_Type_WorldObj::DestroyWorldObj() {
    if (this->m_worldObject) {
        World::ObjectDestroy(this->m_worldObject);
        this->m_worldObject = NULL;
    }
}

// OFFSET: 0x005991D0
void CGGameObject_C_Type_WorldObj::SequenceFinishedHandler(CM2Model* model, uint32_t boneId, uint32_t sequenceId, int32_t interrupted, uint32_t time, uint64_t param, int32_t a7) {
    if (interrupted) {
        return;
    }

    uint32_t next;

    switch (sequenceId) {
        case 146: {
            next = 147;
            break;
        }

        case 148: {
            next = 149;
            break;
        }

        case 162: {
            next = 163;
            break;
        }

        case 164: {
            next = 0;
            break;
        }

        default: {
            return;
        }
    }

    model->SetBoneSequence(0xFFFFFFFF, next, 0xFFFFFFFF, 0, 1.0f, 1, 1, 0, 0, 0);
}

// OFFSET: 0x0059AFA0/0x1402096D0
CGGameObject_C_Type_DestructibleBuilding::CGGameObject_C_Type_DestructibleBuilding(CGGameObject_C* owner, const CClientObjCreate& create) : CGGameObject_C_Type_WorldObj(owner, create) {
    memset(this->m_states, 0, sizeof(this->m_states));
    this->m_animStartTime = 0;
    this->m_animDuration = 0;
    this->m_animating = 0;
    this->m_rebuildEffect = 0;
    this->m_baseCollision = 0;
    this->m_modelData = NULL;
    this->m_sound = 0;
    this->m_loadPending = 1;
    this->m_previousState = -1;
    this->m_baseZ = 0.0f;
    this->m_sinkDepth = 0.0f;
    this->m_previousSinkDepth = 0.0f;
    this->m_boundingRadius = 0.0f;
}

// PROVEN: 3e60e922 IDENTICAL/NONE
// OFFSET: 0x00599850/0x140207BB0
void CGGameObject_C_Type_DestructibleBuilding::AmbientSequenceFinishedHandler(CM2Model* model, uint32_t boneId, uint32_t sequenceId, int32_t interrupted, uint32_t time, uint64_t param, int32_t a7) {
    if (!interrupted && sequenceId != 154) {
        model->SetBoneSequence(0xFFFFFFFF, 154, 0xFFFFFFFF, 0, 1.0f, 1, 1, 0, 0, 0);
    }
}

// PROVEN: 199d58d1 IDENTICAL/NONE
// OFFSET: 0x0059AF50/0x140209670
void CGGameObject_C_Type_DestructibleBuilding::AnimEventCallback(CM2Model* model, uint32_t boneId, uint32_t eventId, uint32_t data, const C3Vector& position, uint32_t time, uint64_t param) {
    CGGameObject_C* object = (CGGameObject_C*)ClntObjMgrObjectPtr(WOWGUID::Make(param), TYPEMASK_GAMEOBJECT, __FILE__, __LINE__);

    if (object) {
        object->m_typeData->HandleAnimEvent(eventId, data, position, time);
    }
}

// PROVEN: 0777a622 IDENTICAL/NONE
// OFFSET: 0x005996C0/0x140207A80
void CGGameObject_C_Type_DestructibleBuilding::PostReenable() {
    if (this->m_worldObject) {
        World::ObjectSetRenderEnabled(this->m_worldObject, 1);
        World::ObjectEnableCollision(this->m_worldObject, 1);
    }
}

// PROVEN: a4eb9e03 IDENTICAL/NONE
// OFFSET: 0x005996F0/0x140207AB0
void CGGameObject_C_Type_DestructibleBuilding::Disable() {
    if (this->m_worldObject) {
        World::ObjectSetRenderEnabled(this->m_worldObject, 0);
        World::ObjectEnableCollision(this->m_worldObject, 0);
    }
}

// PROVEN: 785bbc5e IDENTICAL/NONE
// OFFSET: 0x00599720
uint16_t CGGameObject_C_Type_DestructibleBuilding::GetImpactEffectDoodadSet(int32_t state) const {
    if (this->m_modelData) {
        switch (state >> 1) {
            case 0:
                return this->m_modelData->m_state0ImpactEffectDoodadSet;

            case 1:
                return this->m_modelData->m_state1ImpactEffectDoodadSet;

            case 2:
                return this->m_modelData->m_state2ImpactEffectDoodadSet;
        }
    }

    return 0;
}

// INLINED
uint16_t CGGameObject_C_Type_DestructibleBuilding::GetDestructionDoodadSet(int32_t state) const {
    if (this->m_modelData) {
        switch (state >> 1) {
            case 1:
                return this->m_modelData->m_state1DestructionDoodadSet;

            case 2:
                return this->m_modelData->m_state2DestructionDoodadSet;

            case 3:
                return this->m_modelData->m_state3InitDoodadSet;
        }
    }

    return 0;
}

// PROVEN: fcd47cb0 IDENTICAL/NONE
// OFFSET: 0x00599760
uint16_t CGGameObject_C_Type_DestructibleBuilding::GetAmbientDoodadSet(int32_t state) const {
    if (!this->m_modelData) {
        return 0;
    }

    switch (state >> 1) {
        case 0:
            return this->m_modelData->m_state0AmbientDoodadSet;

        case 1:
            return this->m_modelData->m_state1AmbientDoodadSet;

        case 2:
            return this->m_modelData->m_state2AmbientDoodadSet;

        case 3:
            return this->m_modelData->m_state3AmbientDoodadSet;

        default:
            return 0;
    }
}

// PROVEN: 41c75495 IDENTICAL/NONE
// OFFSET: 0x005997C0
void CGGameObject_C_Type_DestructibleBuilding::DestructSequenceFinishedHandler(CM2Model* model, uint32_t boneId, uint32_t sequenceId, int32_t interrupted, uint32_t time, uint64_t param, int32_t a7) {
    if (!interrupted && sequenceId) {
        model->SetBoneSequence(0xFFFFFFFF, 0, 0xFFFFFFFF, 0, 1.0f, 1, 1, 0, 0, 0);
    }
}

// PROVEN: d3ff62f8 IDENTICAL/NONE
// OFFSET: 0x00599800/0x140207B40
void CGGameObject_C_Type_DestructibleBuilding::ImpactEffectSequenceFinishedHandler(CM2Model* model, uint32_t boneId, uint32_t sequenceId, int32_t interrupted, uint32_t time, uint64_t param, int32_t a7) {
    if (interrupted) {
        return;
    }

    if (sequenceId == 153) {
        model->SetBoneSequence(0xFFFFFFFF, 154, 0xFFFFFFFF, 0, 1.0f, 1, 1, 0, 0, 0);
    } else {
        model->SetBoneSequence(0xFFFFFFFF, 0, 0xFFFFFFFF, 0, 1.0f, 1, 1, 0, 0, 0);
    }
}

// PROVEN: d9480551 IDENTICAL/NONE
// OFFSET: 0x00599890
void CGGameObject_C_Type_DestructibleBuilding::MovedDoodadSequenceHandler(CM2Model* model, uint32_t boneId, uint32_t sequenceId, int32_t interrupted, uint32_t time, uint64_t param, int32_t a7) {
    if (interrupted) {
        return;
    }

    if (sequenceId == 153 || sequenceId == 154) {
        model->SetBoneSequence(0xFFFFFFFF, 155, 0xFFFFFFFF, 0, 1.0f, 1, 1, 0, 0, 0);
    } else if (sequenceId == 155) {
        model->SetBoneSequence(0xFFFFFFFF, 0, 0xFFFFFFFF, 0, 1.0f, 1, 1, 0, 0, 0);
    }
}

// OFFSET: 0x0059B0D0/0x140209860
int32_t CGGameObject_C_Type_DestructibleBuilding::IsHighlightSuppressed() const {
    if (this->m_modelData && this->m_modelData->m_doNotHighlight) {
        return 1;
    }

    CGGameObject_C* owner = this->m_owner;

    if (!owner || owner->m_displayState == 1) {
        return 0;
    }

    return 1;
}

// OFFSET: 0x0059A7C0
float CGGameObject_C_Type_TransportBase::UpdateCurrentVelocity(uint32_t elapsed, const C3Vector& position, C3Vector* direction) {
    float dx = position.x - this->m_position.x;
    float dy = position.y - this->m_position.y;
    float dz = position.z - this->m_position.z;
    float distance = sqrt(dz * dz + dy * dy + dx * dx);
    float speed = elapsed ? distance / ((float)elapsed * 0.001f) : 0.0f;

    if (speed != this->m_speed && this->m_owner->m_effectManager) {
        this->m_owner->m_effectManager->PropertyChanged(1);
    }

    this->m_speed = speed;

    if (direction && !(fabs(distance) < 2.3841858e-7f)) {
        float scale = 1.0f / distance;
        direction->x = scale * dx;
        direction->y = scale * dy;
        direction->z = scale * dz;
    }

    return distance;
}

// OFFSET: 0x0059B660
int32_t Packet_SMSG_GAMEOBJECT_DESPAWN_ANIM(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    WOWGUID guid;
    msg->Get(guid.m_guid);

    CGGameObject_C* object = (CGGameObject_C*)ClntObjMgrObjectPtr(guid, TYPEMASK_GAMEOBJECT, __FILE__, __LINE__);

    if (object) {
        if (!object->IsObjectModelLoaded()) {
            object->m_pendingOneShotAnim = 6;
            return 1;
        }

        object->m_typeData->ActivateOneShotAnim(6);
        object->m_pendingOneShotAnim = 0;
    }

    return 1;
}

// PROVEN: 240f5180 IDENTICAL/NONE
// OFFSET: 0x00599320
void CGGameObject_C_Type_MapObjTransport::UpdateObjectEffectState(uint32_t anim) {
    CObjectEffectManager* manager = this->m_owner->m_effectManager;

    if (!manager) {
        return;
    }

    switch (anim) {
        case 0: {
            manager->EnterState(2, 1, 0);
            this->m_owner->m_effectManager->ExitState(3, 1);
            this->m_owner->m_effectManager->ExitState(4, 1);
            this->m_owner->m_effectManager->ExitState(5, 1);
            break;
        }

        case 162: {
            manager->EnterState(3, 1, 0);
            this->m_owner->m_effectManager->ExitState(2, 1);
            this->m_owner->m_effectManager->ExitState(4, 1);
            this->m_owner->m_effectManager->ExitState(5, 1);
            break;
        }

        case 163: {
            manager->EnterState(4, 1, 0);
            this->m_owner->m_effectManager->ExitState(3, 1);
            this->m_owner->m_effectManager->ExitState(2, 1);
            this->m_owner->m_effectManager->ExitState(5, 1);
            break;
        }

        case 164: {
            manager->EnterState(5, 1, 0);
            this->m_owner->m_effectManager->ExitState(3, 1);
            this->m_owner->m_effectManager->ExitState(4, 1);
            this->m_owner->m_effectManager->ExitState(2, 1);
            break;
        }
    }
}

// OFFSET: 0x0059ED20
void CGGameObject_C::ShowLootEffect() {
    if (this->m_lootEffect) {
        return;
    }

    if (!this->m_worldModel || !this->m_worldModel->IsLoaded()) {
        return;
    }

    const SpellVisualEffectNameRec* rec = GetSpecialSpellVisualEffectNameRec(4);
    this->m_lootEffect = this->m_worldModel->m_scene->CreateModel(rec->m_fileName, 0, 3);

    if (!this->m_lootEffect) {
        return;
    }

    this->m_lootEffect->AttachToParent(this->m_worldModel, 19, NULL, 1);
    GameObjectDisplayInfoRec* displayInfo = g_gameObjectDisplayInfoDB.GetRecord(this->m_gameObject->m_displayID);
    float scale;

    if (displayInfo && displayInfo->m_overrideLootEffectScale > 0.0f) {
        scale = displayInfo->m_overrideLootEffectScale;
    } else {
        scale = fclamp(this->m_height * 1.15f, 1.0f, 5.0f);
    }

    C44Matrix matrix;
    matrix.Scale(scale);
    CM2Model* effect = this->m_lootEffect;
    effect->m_fFlags |= 0x4000;
    effect->m_worldTransform = matrix;
}

// PROVEN: e6130465 IDENTICAL/NONE
// OFFSET: 0x00599B30
void CGGameObject_C::UpdateObjectEffectAnimationStates(uint32_t anim) {
    if (!this->m_effectManager) {
        return;
    }

    uint32_t state = FindObjectEffectStateFromAnimID(anim, 0);

    if (!state || state == this->m_effectAnimState) {
        return;
    }

    this->m_effectManager->ExitState(this->m_effectAnimState, 1);
    this->m_effectManager->EnterState(state, 1, 0);

    if (state == 15 || state == 81) {
        this->m_effectManager->ExitState(37, 1);
        this->m_effectManager->ExitState(38, 1);
        this->m_effectManager->ExitState(6, 1);
        this->m_effectManager->ExitState(7, 1);
        this->m_effectManager->ExitState(47, 1);
        this->m_effectManager->ExitState(48, 1);
        this->m_effectManager->ExitState(49, 1);
        this->m_effectManager->ExitState(50, 1);
        this->m_effectManager->ExitState(51, 1);
        this->m_effectManager->ExitState(52, 1);
        this->m_effectManager->ExitState(53, 1);
        this->m_effectManager->ExitState(54, 1);
        this->m_effectManager->ExitState(55, 1);
        this->m_effectManager->ExitState(56, 1);
        this->m_effectManager->ExitState(57, 1);
        this->m_effectManager->ExitState(58, 1);
        this->m_effectManager->ExitState(59, 1);
        this->m_effectManager->ExitState(60, 1);
        this->m_effectManager->ExitState(61, 1);
        this->m_effectManager->ExitState(62, 1);
        this->m_effectManager->ExitState(63, 1);
        this->m_effectManager->ExitState(64, 1);
        this->m_effectManager->ExitState(65, 1);
        this->m_effectManager->ExitState(66, 1);
    }

    this->m_effectAnimState = state;
}

// OFFSET: 0x005A25E0/0x140212870
CGGameObject_C_Type_FishingNode::CGGameObject_C_Type_FishingNode(CGGameObject_C* owner, const CClientObjCreate& create) : CGGameObject_C_TypeAnimated(owner, create) {
    this->m_interactDistance = 100.0f;
}

// OFFSET: [0x0059B020/0x140209780, 0x0059C750/0x14020B200]
CGGameObject_C_Type_DestructibleBuilding::~CGGameObject_C_Type_DestructibleBuilding() {
    for (uint32_t i = 0; i < 4; i++) {
        if (this->m_states[i].m_object) {
            World::ObjectDestroy(this->m_states[i].m_object);
            this->m_states[i].m_object = 0;
        }
    }

    if (this->m_rebuildEffect) {
        World::ObjectDestroy(this->m_rebuildEffect);
        this->m_rebuildEffect = 0;
    }

    this->m_worldObject = 0;

    if (this->m_baseCollision) {
        World::ObjectDestroy(this->m_baseCollision);
        this->m_baseCollision = 0;
    }

    SE3::StopSound(this->m_sound, -1, __FILE__, __LINE__, "CGGameObject_C_Type_DestructibleBuilding::~CGGameObject_C_Type_DestructibleBuilding");
}

// OFFSET: 0x005A2BF0/0x140212F30
CGGameObject_C_Type_GuildBank::CGGameObject_C_Type_GuildBank(CGGameObject_C* owner, const CClientObjCreate& create) : CGGameObject_C_TypeAnimated(owner, create) {
}

// OFFSET: 0x005A2A80/0x140212DE0
CGGameObject_C_Type_MeetingStone::CGGameObject_C_Type_MeetingStone(CGGameObject_C* owner, const CClientObjCreate& create) : CGGameObject_C_TypeAnimated(owner, create) {
}

// OFFSET: 0x005A2690/0x140212940
CGGameObject_C_Type_Ritual::CGGameObject_C_Type_Ritual(CGGameObject_C* owner, const CClientObjCreate& create) : CGGameObject_C_TypeAnimated(owner, create) {
}

// OFFSET: 0x005A3F60
void CGGameObject_C::Disable() {
    CGObject_C::Disable();

    if (this->m_stats) {
        this->m_typeData->Disable();
    } else {
        g_gameObjectDBCache.CancelCallback(this->m_obj->m_entryID, &GameObjectStatsCallback, this);
    }

    this->m_passenger.m_link.Unlink();

    if (this->m_lootEffect) {
        if (this->m_lootEffect->m_attachParent) {
            this->m_lootEffect->DetachFromParent();
        }

        this->m_lootEffect->Release();
        this->m_lootEffect = NULL;
    }

    if (this->m_nameDesc) {
        PlayerNameDelete(this->m_nameDesc);
    }

    this->m_nameDesc = NULL;
}

// OFFSET: 0x005A3230/0x140213840
CGGameObject_C_Type_TransportBase::CGGameObject_C_Type_TransportBase(CGGameObject_C* owner, const CClientObjCreate& create) : CGGameObject_C_Type_WorldObj(owner, create) {
    this->m_position = C3Vector(0.0f, 0.0f, 0.0f);
    this->m_isLoaded = 0;
    this->m_inLiquid = 0;
    this->m_pathTime = 0;
    this->m_transportUpdateTime = 0;
    this->m_speed = 0.0f;
    this->m_transportState = -1;
    this->m_position = owner->m_passenger.GetPosition(owner->m_passenger.m_position);
    owner->m_hasWorldBounds = 1;
}

// PROVEN: e97e6050 IDENTICAL/NONE
// OFFSET: 0x00599260
void CGGameObject_C_Type_TransportBase::PostInitWithStats() {
    this->CreateWorldObj();

    if (this->m_worldObject) {
        MovementAddTransport(this->m_owner);
    }
}

// OFFSET: 0x005A02E0/0x14020FD30
void CGGameObject_C_Type_MapObjTransport::PostReenable() {
    this->CreateWorldObj();

    if (this->m_worldObject) {
        MovementAddTransport(this->m_owner);
        World::ObjectUpdate(this->m_worldObject, this->m_owner->GetMatrix());
    }

    this->InitializeMovement();
}

// OFFSET: 0x0059A520/0x140208CC0
C3Vector CGGameObject_C_TypeBase::GetPosition() const {
    return this->m_owner->m_passenger.GetPosition(this->m_owner->m_passenger.m_position);
}

// OFFSET: 0x005A3800
CGGameObject_C::~CGGameObject_C() {
    if (!this->m_stats) {
        g_gameObjectDBCache.CancelCallback(this->m_obj->m_entryID, &GameObjectStatsCallback, this);
    }

    this->m_modelFlags &= ~0x400000;

    if (this->m_lootEffect) {
        if (this->m_lootEffect->m_attachParent) {
            this->m_lootEffect->DetachFromParent();
        }

        this->m_lootEffect->Release();
        this->m_lootEffect = NULL;
    }

    if (this->m_nameDesc) {
        PlayerNameDelete(this->m_nameDesc);
    }

    this->m_nameDesc = NULL;

    if (this->m_typeData) {
        delete this->m_typeData;
    }
}

// OFFSET: 0x005A3980
C3Vector CGGameObject_C::GetPosition() const {
    if (this->m_typeData) {
        return this->m_typeData->GetPosition();
    }

    return this->m_passenger.GetPosition(this->m_passenger.m_position);
}

// OFFSET: 0x0059B600
int32_t Packet_SMSG_GAMEOBJECT_CUSTOM_ANIM(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    WOWGUID guid;
    uint32_t anim;
    msg->Get(guid.m_guid);
    msg->Get(anim);

    CGGameObject_C* object = (CGGameObject_C*)ClntObjMgrObjectPtr(guid, TYPEMASK_GAMEOBJECT, __FILE__, __LINE__);

    if (object && anim < 4) {
        object->PlayCustomAnim(anim);
    }

    return 1;
}

// PROVEN: 5325bb50 IDENTICAL/IDENTICAL
// OFFSET: 0x00A3AA30/0x1404BDD20
bool CGGameObject_C_TypeBase::CanBeSolid() const {
    return true;
}

// PROVEN: 07efd57b IDENTICAL/IDENTICAL
// OFFSET: 0x00A3AA30/0x1404BDD20
bool CGGameObject_C_TypeBase::ShouldFadeIn() const {
    return true;
}

// PROVEN: 5c9c2a70 IDENTICAL/NONE
// OFFSET: 0x00599E00/0x1402082A0
bool CGGameObject_C_TypeBase::CanHighlight() const {
    return this->CanUse();
}

// PROVEN: d85e66b1 IDENTICAL/IDENTICAL
// OFFSET: 0x00842700/0x1407FDD80
int32_t CGGameObject_C_TypeBase::IsHighlightSuppressed() const {
    return 0;
}

// PROVEN: 8ee24f6a IDENTICAL/IDENTICAL
// OFFSET: 0x00847950/0x1407D7E80
void CGGameObject_C_TypeBase::OnUpdateFlags(uint32_t changed) {
}

// PROVEN: 296b109d IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void CGGameObject_C_TypeBase::OnUpdateAnimProgress() {
}

// PROVEN: 6b1c2c32 IDENTICAL/DIFFERENT
// OFFSET: 0x00841AE0/0x140211B90
void CGGameObject_C_TypeBase::UpdateState(int32_t oldState, int32_t newState) {
}

// PROVEN: b1339602 IDENTICAL/NONE
// OFFSET: 0x005EB640/0x14026CD90
int32_t CGGameObject_C_TypeBase::Animate(float elapsed) {
    return 1;
}

// PROVEN: 13be4357 IDENTICAL/DIFFERENT
// OFFSET: 0x00627990/0x140693B10
void CGGameObject_C_TypeBase::ShouldRender(uint32_t flags, int32_t& render, int32_t& a3) {
}

// PROVEN: ec8d926d IDENTICAL/IDENTICAL
// OFFSET: 0x00599E10/0x1402082B0
float CGGameObject_C_TypeBase::GetBaseAlpha() const {
    return 1.0f;
}

// OFFSET: #0x006B1B80
void CGGameObject_C_TypeBase::HandleAnimEvent(uint32_t eventId, uint32_t data, const C3Vector& position, uint32_t time) {
}

// PROVEN: a39997f5 IDENTICAL/DIFFERENT
// OFFSET: 0x009FCE20/0x14090FD50
int32_t CGGameObject_C_TypeBase::HandleAnimInterrupted() {
    return 1;
}

// PROVEN: d522ab25 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void CGGameObject_C_TypeBase::HandleAnimFinished() {
}

// PROVEN: 4d2d7931 IDENTICAL/IDENTICAL
// OFFSET: 0x00847950/0x1407D7E80
void CGGameObject_C_TypeBase::ActivateOneShotAnim(uint32_t anim) {
}

// PROVEN: 7880ab7b IDENTICAL/NONE
// OFFSET: 0x00847950
void CGGameObject_C_TypeBase::ActivateOneShotAnimKit(uint32_t animKitID) {
}

// PROVEN: 2e572263 IDENTICAL/NONE
// OFFSET: 0x00599130/0x1402075A0
const char* CGGameObject_C_TypeBase::DebugStatus() {
    return "";
}

// PROVEN: de7aae2e IDENTICAL/NONE
// OFFSET: 0x00599E20/0x14025E5A0
float CGGameObject_C_TypeBase::GetSpeed() const {
    return -1.0f;
}

// PROVEN: 4c00802d IDENTICAL/DIFFERENT
// OFFSET: 0x00841AE0/0x140211B90
void CGGameObject_C_TypeBase::AddPassenger(CPassenger* passenger, int32_t a2) {
}

// PROVEN: eeb2d21c IDENTICAL/DIFFERENT
// OFFSET: 0x00A9AA30/0x1402123D0
int32_t CGGameObject_C_TypeBase::IsPointInside(const C3Vector& point) const {
    return 0;
}

// PROVEN: 8b8dff6c IDENTICAL/IDENTICAL
// OFFSET: 0x00842700/0x1407FDD80
uintptr_t CGGameObject_C_TypeBase::GetMapObjectTransportID() const {
    return NULL;
}

// PROVEN: 980ed0eb IDENTICAL/IDENTICAL
// OFFSET: 0x00847950/0x1407D7E80
void CGGameObject_C_TypeBase::PostInit(bool a1) {
}

// PROVEN: 8b5d6ada IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void CGGameObject_C_TypeBase::PostInitWithStats() {
}

// PROVEN: d0d1bd54 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void CGGameObject_C_TypeBase::Reenable() {
}

// PROVEN: 97ab290a IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void CGGameObject_C_TypeBase::Disable() {
}

// PROVEN: 8e070758 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void CGGameObject_C_TypeBase::PostReenable() {
}

// PROVEN: c8a47237 IDENTICAL/IDENTICAL
// OFFSET: 0x00842700/0x1407FDD80
ShipPath* CGGameObject_C_TypeBase::GetShipPath() const {
    return NULL;
}

// PROVEN: 06edc0db IDENTICAL/DIFFERENT
// OFFSET: 0x00841AE0/0x140211B90
void CGGameObject_C_TypeBase::UpdateMovement(uint32_t time, uint32_t elapsed) {
}

// PROVEN: a4cd3ea3 IDENTICAL/IDENTICAL
// OFFSET: 0x00847950/0x1407D7E80
void CGGameObject_C_TypeBase::UpdateDisplay(uint32_t time) {
}

// PROVEN: 9289b4b5 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void CGGameObject_C_TypeBase::UpdateDisabled() {
}

// PROVEN: feb43afd IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void CGGameObject_C_TypeBase::ModelLoaded() {
}

// PROVEN: 9690f15d IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void CGGameObject_C_TypeBase::StartInteraction() {
}

// PROVEN: abeac73c IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void CGGameObject_C_TypeBase::CloseInteraction() {
}

// PROVEN: c9a9e833 IDENTICAL/IDENTICAL
// OFFSET: 0x00847950/0x1407D7E80
void CGGameObject_C_TypeBase::AddToolTipInfo(CGTooltip* tooltip) {
}

// PROVEN: 16b161da IDENTICAL/IDENTICAL
// OFFSET: 0x00599E30/0x1402082B0
uint32_t CGGameObject_C_TypeBase::GetSequenceTimeFromRealTime(uint32_t time) const {
    return time;
}

// PROVEN: faee8b95 IDENTICAL/IDENTICAL
// OFFSET: 0x00842700/0x1407FDD80
uint32_t CGGameObject_C_TypeBase::GetTransportUpdateTime() const {
    return 0;
}

// PROVEN: a6eb848b IDENTICAL/IDENTICAL
// OFFSET: 0x00B06D20/0x140317F90
bool CGGameObject_C_Type_Null::CanUse() const {
    return false;
}

// PROVEN: 2c35ebff IDENTICAL/DIFFERENT
// OFFSET: 0x005A1AD0/0x140211B90
void CGGameObject_C_TypeAnimated::ModelLoaded() {
    this->ResetAnimState();
}

// OFFSET: #0x0070CBA0
void CGGameObject_C::SetStorage(uint32_t* storage, uint32_t* saved) {
    this->CGObject_C::SetStorage(storage, saved);
    this->m_gameObject = reinterpret_cast<CGGameObjectData*>(storage + OBJECT_END);
    this->m_gameObjectMirror = saved + CGObject::TotalFieldsSaved();
}

// OFFSET: 0x005A4770/0x1402152D0
CGGameObject_C::CGGameObject_C(uint32_t time, const CClientObjCreate& create) : CGObject_C(time, create), m_passenger(create.m_hasGOTransport ? create.m_goTransport.m_guid : WOWGUID(), create.m_hasGOTransport ? create.m_goTransport.m_position : create.m_stationaryPosition, this->m_obj->m_guid), m_typeData(NULL), m_stats(NULL) {
    this->m_worldBounds.b = C3Vector(0.0f, 0.0f, 0.0f);
    this->m_worldBounds.t = C3Vector(0.0f, 0.0f, 0.0f);
    this->m_displayState = this->m_gameObject->m_state;
    this->m_pendingOneShotAnim = 0;
    this->m_hasWorldBounds = 0;
    this->m_animLocked = 0;
    this->m_animKitID = 0;
    this->m_lootEffect = NULL;
    this->m_animKitManager.SetGameObjectMode(1);
    *reinterpret_cast<uint64_t*>(&this->m_passenger.m_facing) = create.m_hasRotation ? create.m_packedRotation : 0;
    this->m_passenger.unk_002C |= 0x2;

    ClntObjMgrHideObject(this->m_obj->m_guid);

    if (create.m_hasTransportTime) {
        this->m_transportTimeOffset = create.m_transportTime - time;
    }

    switch (this->m_gameObject->m_typeID) {
        case 0:
            this->m_typeData = new (SMemAlloc(sizeof(CGGameObject_C_Type_Door), __FILE__, __LINE__, 0)) CGGameObject_C_Type_Door(this, create);
            break;
        case 1:
            this->m_typeData = new (SMemAlloc(sizeof(CGGameObject_C_Type_Button), __FILE__, __LINE__, 0)) CGGameObject_C_Type_Button(this, create);
            break;
        case 3:
            this->m_typeData = new (SMemAlloc(sizeof(CGGameObject_C_Type_Chest), __FILE__, __LINE__, 0)) CGGameObject_C_Type_Chest(this, create);
            break;
        case 6:
            this->m_typeData = new (SMemAlloc(sizeof(CGGameObject_C_Type_Trap), __FILE__, __LINE__, 0)) CGGameObject_C_Type_Trap(this, create);
            break;
        case 12:
            this->m_typeData = new (SMemAlloc(sizeof(CGGameObject_C_Type_AreaDamage), __FILE__, __LINE__, 0)) CGGameObject_C_Type_AreaDamage(this, create);
            break;
        case 2:
            this->m_typeData = new (SMemAlloc(sizeof(CGGameObject_C_Type_QuestGiver), __FILE__, __LINE__, 0)) CGGameObject_C_Type_QuestGiver(this, create);
            break;
        case 4:
            this->m_typeData = new (SMemAlloc(sizeof(CGGameObject_C_Type_Binder), __FILE__, __LINE__, 0)) CGGameObject_C_Type_Binder(this, create);
            break;
        case 5:
            this->m_typeData = new (SMemAlloc(sizeof(CGGameObject_C_Type_Generic), __FILE__, __LINE__, 0)) CGGameObject_C_Type_Generic(this, create);
            break;
        case 7:
            this->m_typeData = new (SMemAlloc(sizeof(CGGameObject_C_Type_Chair), __FILE__, __LINE__, 0)) CGGameObject_C_Type_Chair(this, create);
            break;
        case 8:
            this->m_typeData = new (SMemAlloc(sizeof(CGGameObject_C_Type_SpellFocus), __FILE__, __LINE__, 0)) CGGameObject_C_Type_SpellFocus(this, create);
            break;
        case 9:
            this->m_typeData = new (SMemAlloc(sizeof(CGGameObject_C_Type_Text), __FILE__, __LINE__, 0)) CGGameObject_C_Type_Text(this, create);
            break;
        case 10:
            this->m_typeData = new (SMemAlloc(sizeof(CGGameObject_C_Type_Goober), __FILE__, __LINE__, 0)) CGGameObject_C_Type_Goober(this, create);
            break;
        case 11:
            this->m_typeData = new (SMemAlloc(sizeof(CGGameObject_C_Type_Transport), __FILE__, __LINE__, 0)) CGGameObject_C_Type_Transport(this, create);
            break;
        case 35:
            this->m_typeData = new (SMemAlloc(sizeof(CGGameObject_C_Type_TrapDoor), __FILE__, __LINE__, 0)) CGGameObject_C_Type_TrapDoor(this, create);
            break;
        case 13:
            this->m_typeData = new (SMemAlloc(sizeof(CGGameObject_C_Type_Camera), __FILE__, __LINE__, 0)) CGGameObject_C_Type_Camera(this, create);
            break;
        case 14:
            this->m_typeData = new (SMemAlloc(sizeof(CGGameObject_C_Type_MapObj), __FILE__, __LINE__, 0)) CGGameObject_C_Type_MapObj(this, create);
            break;
        case 15:
            this->m_typeData = new (SMemAlloc(sizeof(CGGameObject_C_Type_MapObjTransport), __FILE__, __LINE__, 0)) CGGameObject_C_Type_MapObjTransport(this, create);
            break;
        case 16:
            this->m_typeData = new (SMemAlloc(sizeof(CGGameObject_C_Type_DuelArbiter), __FILE__, __LINE__, 0)) CGGameObject_C_Type_DuelArbiter(this, create);
            break;
        case 25:
            this->m_typeData = new (SMemAlloc(sizeof(CGGameObject_C_Type_FishingHole), __FILE__, __LINE__, 0)) CGGameObject_C_Type_FishingHole(this, create);
            break;
        case 17:
            this->m_typeData = new (SMemAlloc(sizeof(CGGameObject_C_Type_FishingNode), __FILE__, __LINE__, 0)) CGGameObject_C_Type_FishingNode(this, create);
            break;
        case 18:
            this->m_typeData = new (SMemAlloc(sizeof(CGGameObject_C_Type_Ritual), __FILE__, __LINE__, 0)) CGGameObject_C_Type_Ritual(this, create);
            break;
        case 19:
            this->m_typeData = new (SMemAlloc(sizeof(CGGameObject_C_Type_Mailbox), __FILE__, __LINE__, 0)) CGGameObject_C_Type_Mailbox(this, create);
            break;
        case 22:
            this->m_typeData = new (SMemAlloc(sizeof(CGGameObject_C_Type_SpellCaster), __FILE__, __LINE__, 0)) CGGameObject_C_Type_SpellCaster(this, create);
            break;
        case 23:
            this->m_typeData = new (SMemAlloc(sizeof(CGGameObject_C_Type_MeetingStone), __FILE__, __LINE__, 0)) CGGameObject_C_Type_MeetingStone(this, create);
            break;
        case 24:
            this->m_typeData = new (SMemAlloc(sizeof(CGGameObject_C_Type_FlagStand), __FILE__, __LINE__, 0)) CGGameObject_C_Type_FlagStand(this, create);
            break;
        case 26:
            this->m_typeData = new (SMemAlloc(sizeof(CGGameObject_C_Type_FlagDrop), __FILE__, __LINE__, 0)) CGGameObject_C_Type_FlagDrop(this, create);
            break;
        case 27:
            this->m_typeData = new (SMemAlloc(sizeof(CGGameObject_C_Type_MiniGame), __FILE__, __LINE__, 0)) CGGameObject_C_Type_MiniGame(this, create);
            break;
        case 29:
            this->m_typeData = new (SMemAlloc(sizeof(CGGameObject_C_Type_CapturePoint), __FILE__, __LINE__, 0)) CGGameObject_C_Type_CapturePoint(this, create);
            break;
        case 30:
            this->m_typeData = new (SMemAlloc(sizeof(CGGameObject_C_Type_AuraGenerator), __FILE__, __LINE__, 0)) CGGameObject_C_Type_AuraGenerator(this, create);
            break;
        case 31:
            this->m_typeData = new (SMemAlloc(sizeof(CGGameObject_C_Type_DungeonDifficulty), __FILE__, __LINE__, 0)) CGGameObject_C_Type_DungeonDifficulty(this, create);
            break;
        case 32:
            this->m_typeData = new (SMemAlloc(sizeof(CGGameObject_C_Type_BarberChair), __FILE__, __LINE__, 0)) CGGameObject_C_Type_BarberChair(this, create);
            break;
        case 33:
            this->m_typeData = new (SMemAlloc(sizeof(CGGameObject_C_Type_DestructibleBuilding), __FILE__, __LINE__, 0)) CGGameObject_C_Type_DestructibleBuilding(this, create);
            break;
        case 34:
            this->m_typeData = new (SMemAlloc(sizeof(CGGameObject_C_Type_GuildBank), __FILE__, __LINE__, 0)) CGGameObject_C_Type_GuildBank(this, create);
            break;
        default:
            this->m_typeData = new (SMemAlloc(sizeof(CGGameObject_C_Type_Null), __FILE__, __LINE__, 0)) CGGameObject_C_Type_Null(this, create);
            SysMsgPrintf(STATUS_FATAL, 2, "BADBASEGAMEOBJECT|%d", this->m_gameObject->m_typeID);
            break;
    }

    this->m_effectAnimState = 0;
}

// OFFSET: 0x005A3D70
void CGGameObject_C::PostInit(uint32_t time, const CClientObjCreate& create, bool reenable) {
    this->m_passenger.PostInit(create.m_stationaryPosition);
    this->CGObject_C::PostInit(time, create, reenable);
    CM2Model* model = this->m_worldModel;

    if (model) {
        if (!CGGameObject_C::s_cvUseAnimKits || !CGGameObject_C::s_cvUseAnimKits->m_intValue) {
            this->m_animKitManager.SetExpressionsBlocked(1, 1);
        }

        this->m_animKitManager.SetModel(model, 0);
        ClntObjMgrShowObject(this->m_obj->m_guid);
        model->SetEventCallback(reinterpret_cast<void*>(&CGGameObject_C::AnimEventCallback), this->m_obj->m_guid.m_guid);
        model->SetSequenceStartCallback(&CGGameObject_C::SequenceStartCallback, this->m_obj->m_guid.m_guid);
        model->SetSequenceCallback(reinterpret_cast<void*>(&CGGameObject_C::SequenceCallback), this->m_obj->m_guid.m_guid);

        if (model->IsLoaded()) {
            CM2Model* worldModel = this->m_worldModel;

            if (!(worldModel->m_shared->m_flags & 0x1)) {
                worldModel->WaitForLoad(NULL);
            }

            CAaBox bounds = worldModel->m_shared->m_data->m_collisionBounds.m_extent;
            CWorldMath::TransformAABox(this->m_matrix, bounds, this->m_worldBounds);

            if (this->m_typeData->CanBeSolid()) {
                this->m_hasWorldBounds = this->m_worldBounds.t.x > this->m_worldBounds.b.x && this->m_worldBounds.t.y > this->m_worldBounds.b.y && this->m_worldBounds.t.z > this->m_worldBounds.b.z;
            }
        }
    }

    this->m_typeData->PostInit(reenable);

    if (create.m_hasAnimKits) {
        uint32_t animKitID = create.m_animKits[0];

        if (animKitID) {
            this->m_animKitID = animKitID;
            this->m_typeData->ActivateOneShotAnimKit(animKitID);
        }
    }

    WOWGUID guid = this->m_obj->m_guid;
    GameObjectStats_C* stats = g_gameObjectDBCache.GetRecord(this->m_obj->m_entryID, guid, &GameObjectStatsCallback, this, false);

    if (stats) {
        this->SetStats(stats);
    }
}

// PROVEN: 4cd7ac8d IDENTICAL/DIFFERENT
// OFFSET: 0x006705F0/0x140313F10
bool CGGameObject_C_Type_Null::CanUseNow(GAME_ERROR_TYPE* error, float* distance) const {
    return false;
}

// OFFSET: 0x0059C060/0x14020AB40
CGGameObject_C_Type_WorldObj::~CGGameObject_C_Type_WorldObj() {
    if (this->m_worldObject) {
        World::ObjectDestroy(this->m_worldObject);
        this->m_worldObject = NULL;
    }
}

// PROVEN: 6bd1342c IDENTICAL/IDENTICAL
// OFFSET: 0x00B06D20/0x140317F90
bool CGGameObject_C_Type_WorldObj::CanBeSolid() const {
    return false;
}

// PROVEN: 86c2c3fc IDENTICAL/IDENTICAL
// OFFSET: 0x00B06D20/0x140317F90
bool CGGameObject_C_Type_WorldObj::CanHighlight() const {
    return false;
}

// PROVEN: 06ad81e6 IDENTICAL/IDENTICAL
// OFFSET: 0x00B06D20/0x140317F90
bool CGGameObject_C_Type_WorldObj::CanUse() const {
    return false;
}

// PROVEN: ae2e8844 IDENTICAL/DIFFERENT
// OFFSET: 0x00B41A40/0x14090FD00
uintptr_t CGGameObject_C_Type_WorldObj::GetMapObjectTransportID() const {
    return this->m_worldObject;
}

// PROVEN: 4450928f IDENTICAL/NONE
// OFFSET: 0x00599190/0x1402075E0
void CGGameObject_C_Type_WorldObj::PostInitWithStats() {
    this->CreateWorldObj();
}

// PROVEN: 910dae4b IDENTICAL/NONE
// OFFSET: 0x005991A0/0x1402075F0
void CGGameObject_C_Type_WorldObj::Disable() {
    this->DestroyWorldObj();
}

// PROVEN: 087412bb IDENTICAL/NONE
// OFFSET: 0x00599190/0x1402075E0
void CGGameObject_C_Type_WorldObj::PostReenable() {
    this->CreateWorldObj();
}

// OFFSET: [0x005A2EF0/0x140213400, 0x005A3300]
CGGameObject_C_Type_TransportBase::~CGGameObject_C_Type_TransportBase() {
    if (this->m_worldObject) {
        MovementRemoveTransport(this->m_owner);
    }
}

// PROVEN: d52b63c7 IDENTICAL/NONE
// OFFSET: 0x005A2F80/0x1402134C0
C3Vector CGGameObject_C_Type_TransportBase::GetPosition() const {
    return this->m_position;
}

// PROVEN: b635ed38 IDENTICAL/NONE
// OFFSET: 0x005992E0/0x140207740
int32_t CGGameObject_C_Type_TransportBase::IsPointInside(const C3Vector& point) const {
    if (!this->m_worldObject) {
        return 1;
    }

    return static_cast<uint8_t>(World::ObjectTestConvexVolume(this->m_worldObject, point, 1.6404123f));
}

// OFFSET: 0x00599290
void CGGameObject_C_Type_TransportBase::PostReenable() {
    this->CreateWorldObj();

    if (this->m_worldObject) {
        MovementAddTransport(this->m_owner);
        World::ObjectUpdate(this->m_worldObject, this->m_owner->GetMatrix());
    }
}

// OFFSET: 0x005A0650/0x1402100B0
void CGGameObject_C_Type_MapObjTransport::UpdateState(int32_t oldState, int32_t newState) {
    uint32_t pausePointsIndex = CGameObjectDef::GetPropNum(this->m_owner->m_gameObject->m_typeID, 128);
    GameObjectStats* pausePointsStats = this->m_owner->m_stats;
    uint32_t pausePoints = (pausePointsStats && pausePointsIndex < 32) ? pausePointsStats->m_propValue[pausePointsIndex] : 0;

    if (!pausePoints) {
        return;
    }

    bool closed = newState == 1;

    if (oldState == newState) {
        this->m_shipPath.WaitAtNextPausePoint(this->m_pathTime, !closed);
    }

    this->m_shipPath.WaitAtNextPausePoint(this->m_pathTime, closed);
}

// OFFSET: 0x0059C780/0x14020B250
void CGGameObject_C_Type_DestructibleBuilding::PostInit(bool a1) {
    const GameObjectDisplayInfoRec* display = g_gameObjectDisplayInfoDB.GetRecord(this->m_owner->m_gameObject->m_displayID);

    if (display) {
        float x = display->m_geoBoxMax[0] * display->m_geoBoxMax[0] <= display->m_geoBoxMin[0] * display->m_geoBoxMin[0] ? display->m_geoBoxMin[0] * display->m_geoBoxMin[0] : display->m_geoBoxMax[0] * display->m_geoBoxMax[0];
        x = sqrt(x);
        float y = display->m_geoBoxMax[1] * display->m_geoBoxMax[1] <= display->m_geoBoxMin[1] * display->m_geoBoxMin[1] ? display->m_geoBoxMin[1] * display->m_geoBoxMin[1] : display->m_geoBoxMax[1] * display->m_geoBoxMax[1];
        y = sqrt(y);
        float z = display->m_geoBoxMax[2] * display->m_geoBoxMax[2] <= display->m_geoBoxMin[2] * display->m_geoBoxMin[2] ? display->m_geoBoxMin[2] * display->m_geoBoxMin[2] : display->m_geoBoxMax[2] * display->m_geoBoxMax[2];
        z = sqrt(z);
        this->m_boundingRadius = sqrt(x * x + y * y + z * z);

        if (this->m_boundingRadius < 0.001f) {
            this->m_boundingRadius = 50.0f;
            SysMsgPrintf(STATUS_ERROR, 2, "Destructible building WMO(%s) has invalid geobox", static_cast<const char*>(display->m_modelName));
        }
    }

    if (this->m_owner) {
        this->m_owner->UpdateWorldObject(0);
    }

    this->m_modelData = g_destructibleModelDataDB.GetRecord(reinterpret_cast<const int32_t&>(this->m_owner->m_gameObject->m_parentRotation[0]));

    if (this->m_modelData) {
        this->m_states[0].m_displayId = this->m_modelData->m_state0WMO;
        this->m_states[1].m_displayId = this->m_modelData->m_state1WMO;
        this->m_states[2].m_displayId = this->m_modelData->m_state2WMO;
        this->m_states[3].m_displayId = this->m_modelData->m_state3WMO;

        for (int32_t i = 0; i < 4; i++) {
            DestructibleBuildingState& entry = this->m_states[i];

            if (entry.m_displayId) {
                const GameObjectDisplayInfoRec* stateDisplay = g_gameObjectDisplayInfoDB.GetRecord(entry.m_displayId);

                if (stateDisplay) {
                    entry.m_modelName = stateDisplay->m_modelName;
                    continue;
                }

                SysMsgPrintf(STATUS_ERROR, 2, "Destructible building - Game object id %d is missing its display record %d for state %d", this->m_owner->m_obj->m_entryID, this->m_owner->m_gameObject->m_displayID, i);
            }

            entry.m_displayId = this->m_states[i - 1].m_displayId;
            entry.m_modelName = this->m_states[i - 1].m_modelName;
        }
    }

    C3Vector position = this->m_owner->m_passenger.GetPosition(this->m_owner->m_passenger.m_position);

    for (int32_t i = 0; i < 4; i++) {
        DestructibleBuildingState& entry = this->m_states[i];

        if (entry.m_object || !entry.m_modelName) {
            continue;
        }

        uint16_t doodadSets[3];
        doodadSets[0] = this->GetDestructionDoodadSet(2 * i);
        doodadSets[1] = this->GetImpactEffectDoodadSet(2 * i);
        doodadSets[2] = this->GetAmbientDoodadSet(2 * i);
        int32_t nameSet;

        if (!this->m_modelData) {
            nameSet = 0;
        } else {
            switch (i) {
                case 1:
                    nameSet = this->m_modelData->m_state1NameSet;
                    break;

                case 2:
                    nameSet = this->m_modelData->m_state2NameSet;
                    break;

                case 3:
                    nameSet = this->m_modelData->m_state3NameSet;
                    break;

                default:
                    nameSet = this->m_modelData->m_state0NameSet;
                    break;
            }
        }

        uint64_t guid = this->m_owner->m_obj->m_guid.m_guid;
        entry.m_object = World::ObjectCreate(entry.m_modelName, position, this->m_owner->GetFacing(), 0, 0, guid, 0, 3, doodadSets, &this->m_boundingRadius, nameSet);

        if (entry.m_object) {
            World::ObjectSetRenderEnabled(entry.m_object, 0);
            World::ObjectEnableCollision(entry.m_object, 0);

            if (reinterpret_cast<const int32_t&>(this->m_owner->m_gameObject->m_parentRotation[0]) == 114) {
                World::ObjectSetMoveable(entry.m_object, 1);
            }
        }

        this->m_states[i].m_loading = 1;
    }

    this->m_worldObject = this->m_states[((this->m_owner->m_gameObject->m_flags >> 8) & 0xE) >> 1].m_object;

    if (this->m_worldObject) {
        World::ObjectSetRenderEnabled(this->m_worldObject, 1);
        World::ObjectEnableCollision(this->m_worldObject, 1);
    }

    this->m_loadPending = 1;

    if (!this->m_modelData) {
        return;
    }

    int32_t repairFx = this->m_modelData->m_repairGroundFx;

    if (repairFx) {
        const GameObjectDisplayInfoRec* fxDisplay = g_gameObjectDisplayInfoDB.GetRecord(repairFx);

        if (fxDisplay) {
            this->m_rebuildEffect = World::ObjectCreate(fxDisplay->m_modelName, position, this->m_owner->GetFacing(), 0, 0, 0, 0, 0, NULL, NULL, 0);

            if (this->m_rebuildEffect) {
                World::ObjectSetAlwaysExterior(this->m_rebuildEffect, 1);
                World::ObjectSetRenderEnabled(this->m_rebuildEffect, 0);
                World::ObjectEnableCollision(this->m_rebuildEffect, 0);
            }
        } else {
            SysMsgPrintf(STATUS_ERROR, 2, "Destructible building - Game object id %d is missing its rebuild FX display record %d", this->m_owner->m_obj->m_entryID, repairFx);
        }
    }

    if (this->m_states[0].m_modelName) {
        uint64_t guid = this->m_owner->m_obj->m_guid.m_guid;
        this->m_baseCollision = World::ObjectCreate(this->m_states[0].m_modelName, position, this->m_owner->GetFacing(), 0, 0, guid, 0, 0, NULL, NULL, 0);

        if (this->m_baseCollision) {
            World::ObjectSetRenderEnabled(this->m_baseCollision, 0);
            World::ObjectEnableCollision(this->m_baseCollision, 0);
        }
    }
}

// OFFSET: 0x0059CCA0/0x14020B8A0
bool CGGameObject_C_Type_DestructibleBuilding::CanHighlight() const {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player && this->m_owner->m_obj->m_guid.m_guid == World::QueryObjectParentParam64(player->m_worldObject)) {
        return false;
    }

    return this->m_modelData && !this->m_modelData->m_doNotHighlight;
}

// OFFSET: 0x0059CD10/0x14020B920
void CGGameObject_C_Type_DestructibleBuilding::UpdateDestructibleState() {
    int32_t current = 0;

    do {
        if (this->m_states[current].m_object == this->m_worldObject) {
            break;
        }

        current++;
    } while (current < 4);

    int32_t state = ((this->m_owner->m_gameObject->m_flags >> 8) & 0xE) >> 1;

    if (state == current) {
        return;
    }

    SE3::StopSound(this->m_sound, -1, __FILE__, __LINE__, "CGGameObject_C_Type_DestructibleBuilding::UpdateDestructibleState");
    DestructibleBuildingState& entry = this->m_states[state];
    uintptr_t object = entry.m_object;

    if (object) {
        World::ObjectSetRenderEnabled(object, 1);
        World::ObjectEnableCollision(object, 1);
    }

    if (this->m_worldObject) {
        World::ObjectSetRenderEnabled(this->m_worldObject, 0);
        World::ObjectEnableCollision(this->m_worldObject, 0);
        uint16_t destruction = this->GetDestructionDoodadSet(2 * current);

        if (destruction) {
            World::ObjectSetDoodadsEnabled(this->m_worldObject, 0, destruction);
        }
    }

    if (this->m_worldObject && state > current && state != 3 && state) {
        CGUnit_C* mover = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(CGUnit_C::s_activeMover, TYPEMASK_UNIT, __FILE__, __LINE__));

        if (mover && !mover->HasVehicleTransport() && !(mover->m_movement->m_flags & 0x1000000)) {
            CAaBox extents;
            extents.b = C3Vector(0.0f, 0.0f, 0.0f);
            extents.t = C3Vector(0.0f, 0.0f, 0.0f);
            World::ObjectGetExtents(this->m_worldObject, extents);
            C44Matrix inverse = this->m_owner->GetMatrix().AffineInverse();

            if (extents.ContainsPointExclusive(mover->GetPosition() * inverse)) {
                mover->OnFallLocal(FrameTime::GetCurTimeMs());
            }
        }
    }

    uint16_t moved[2] = { this->GetImpactEffectDoodadSet(2 * current), this->GetAmbientDoodadSet(2 * current) };
    uint16_t offset = 100 * (current + 1);

    for (uint32_t i = 0; i < 2; i++) {
        uint16_t set = moved[i];

        if (!set) {
            continue;
        }

        World::ObjectSetSequence(this->m_worldObject, 155, 0, set);
        World::ObjectSetSequenceFinishedCallback(this->m_worldObject, &CGGameObject_C_Type_DestructibleBuilding::MovedDoodadSequenceHandler, static_cast<uint64_t>(static_cast<int32_t>(set + offset)) | (static_cast<uint64_t>(reinterpret_cast<uintptr_t>(this)) << 32), set);
        World::ObjectSetEventCallback(this->m_worldObject, &CGGameObject_C_Type_DestructibleBuilding::AnimEventCallback, this->m_owner->m_obj->m_guid.m_guid, set);
        uint16_t target = set + offset;
        World::ObjectMoveDoodads(this->m_worldObject, set, object, target);
        World::ObjectSetDoodadsEnabled(object, 1, target);
    }

    if (this->m_previousState != -1) {
        uint16_t restored[2] = { this->GetImpactEffectDoodadSet(2 * this->m_previousState), this->GetAmbientDoodadSet(2 * this->m_previousState) };

        for (uint32_t i = 0; i < 2; i++) {
            uint16_t set = restored[i];

            if (!set) {
                continue;
            }

            World::ObjectMoveDoodads(this->m_worldObject, 100 * (this->m_previousState + 1) + set, this->m_states[this->m_previousState].m_object, set);
            World::ObjectSetSequenceFinishedCallback(this->m_states[this->m_previousState].m_object, NULL, 0, set);
            World::ObjectSetDoodadsEnabled(this->m_states[this->m_previousState].m_object, 0, set);
            World::ObjectSetSequence(this->m_states[this->m_previousState].m_object, 0, 0, set);
            World::ObjectSetEventCallback(this->m_worldObject, &CGGameObject_C_Type_DestructibleBuilding::AnimEventCallback, this->m_owner->m_obj->m_guid.m_guid, set);
        }
    }

    this->m_worldObject = object;
    this->m_previousState = current;
    entry.m_animate = !state || state == 3;

    if (!this->m_worldObject || entry.m_loading) {
        return;
    }

    if (entry.m_animate) {
        if (this->m_modelData) {
            float speed = 12.0f;
            float healSpeed = static_cast<float>(this->m_modelData->m_healEffectSpeed);

            if (healSpeed > 0.0f) {
                speed = healSpeed;
            }

            int32_t effect = this->m_modelData->m_healEffect;

            if (effect != 4 && speed > 0.0f) {
                CAaBox extents;
                extents.b = C3Vector(0.0f, 0.0f, 0.0f);
                extents.t = C3Vector(0.0f, 0.0f, 0.0f);
                World::ObjectGetExtents(this->m_worldObject, extents);
                this->m_sinkDepth = extents.t.z - extents.b.z;
                float z = this->m_owner->GetPosition().z;
                C3Vector position(z, z, z);
                this->m_baseZ = z;
                World::ObjectUpdate(this->m_worldObject, position, this->m_owner->GetFacing(), 0.0f, 0.0f, 0);
                this->m_animating = 1;
                this->m_baseZ = position.z;
                this->m_animStartTime = CGWorldFrame::s_currentWorldFrame->GetSceneTime();
                this->m_animDuration = static_cast<uint32_t>((extents.t.z - extents.b.z) / (speed * 0.33333334f) * 1000.0f);

                if (this->m_previousState != -1 && effect != 3) {
                    World::ObjectGetExtents(this->m_states[this->m_previousState].m_object, extents);
                    this->m_previousSinkDepth = extents.t.z - extents.b.z;
                    World::ObjectSetRenderEnabled(this->m_states[this->m_previousState].m_object, 1);
                    World::ObjectEnableCollision(this->m_states[this->m_previousState].m_object, 1);
                }

                if (this->m_baseCollision) {
                    World::ObjectEnableCollision(this->m_baseCollision, 1);
                }
            }
        }
    } else if (this->m_modelData) {
        uint16_t set;

        switch (((this->m_owner->m_gameObject->m_flags >> 8) & 0xE) >> 1) {
            case 1:
                set = this->m_modelData->m_state1DestructionDoodadSet;
                break;

            case 2:
                set = this->m_modelData->m_state2DestructionDoodadSet;
                break;

            case 3:
                set = this->m_modelData->m_state3InitDoodadSet;
                break;

            default:
                set = 0;
                break;
        }

        if (set) {
            World::ObjectSetDoodadsEnabled(this->m_worldObject, 1, set);
            World::ObjectSetSequence(this->m_worldObject, 153, 0, set);
            World::ObjectSetSequenceFinishedCallback(this->m_worldObject, &CGGameObject_C_Type_DestructibleBuilding::DestructSequenceFinishedHandler, set, set);
            World::ObjectSetEventCallback(this->m_worldObject, &CGGameObject_C_Type_DestructibleBuilding::AnimEventCallback, this->m_owner->m_obj->m_guid.m_guid, set);
        }
    }

    if (!this->m_modelData) {
        return;
    }

    uint16_t ambient;

    switch (((this->m_owner->m_gameObject->m_flags >> 8) & 0xE) >> 1) {
        case 0:
            ambient = this->m_modelData->m_state0AmbientDoodadSet;
            break;

        case 1:
            ambient = this->m_modelData->m_state1AmbientDoodadSet;
            break;

        case 2:
            ambient = this->m_modelData->m_state2AmbientDoodadSet;
            break;

        case 3:
            ambient = this->m_modelData->m_state3AmbientDoodadSet;
            break;

        default:
            return;
    }

    if (ambient) {
        World::ObjectSetDoodadsEnabled(this->m_worldObject, 1, ambient);
        World::ObjectSetSequence(this->m_worldObject, 153, 0, ambient);
        World::ObjectSetSequenceFinishedCallback(this->m_worldObject, &CGGameObject_C_Type_DestructibleBuilding::AmbientSequenceFinishedHandler, ambient, ambient);
        World::ObjectSetEventCallback(this->m_worldObject, &CGGameObject_C_Type_DestructibleBuilding::AnimEventCallback, this->m_owner->m_obj->m_guid.m_guid, ambient);
    }
}

// PROVEN: e85731ae IDENTICAL/NONE
// OFFSET: 0x0059D510/0x14020C2B0
void CGGameObject_C_Type_DestructibleBuilding::OnUpdateFlags(uint32_t changed) {
    if (!this->m_modelData || !(changed & 0xF00)) {
        return;
    }

    if (changed & 0x100) {
        uint16_t set = this->GetImpactEffectDoodadSet((this->m_owner->m_gameObject->m_flags >> 8) & 0xF);

        if (set) {
            World::ObjectSetDoodadsEnabled(this->m_worldObject, 1, set);
            World::ObjectSetSequence(this->m_worldObject, 153, 0, set);
            World::ObjectSetSequenceFinishedCallback(this->m_worldObject, &CGGameObject_C_Type_DestructibleBuilding::ImpactEffectSequenceFinishedHandler, set, set);
            World::ObjectSetEventCallback(this->m_worldObject, &CGGameObject_C_Type_DestructibleBuilding::AnimEventCallback, this->m_owner->m_obj->m_guid.m_guid, set);
        }
    }

    this->UpdateDestructibleState();
}

// OFFSET: 0x0059D5C0/0x14020C390
void CGGameObject_C_Type_DestructibleBuilding::UpdateDisplay(uint32_t time) {
    if (this->m_loadPending) {
        this->m_loadPending = 0;
        DestructibleBuildingState* entry = this->m_states;

        for (int32_t state = 0; state < 8; state += 2, entry++) {
            if (!entry->m_loading || !entry->m_object) {
                continue;
            }

            if (!World::ObjectIsLoaded(entry->m_object)) {
                this->m_loadPending = 1;
                continue;
            }

            entry->m_loading = 0;
            uint16_t sets[3];
            sets[0] = this->GetDestructionDoodadSet(state);
            sets[1] = this->GetImpactEffectDoodadSet(state);
            sets[2] = this->GetAmbientDoodadSet(state);

            for (uint32_t i = 0; i < 3; i++) {
                uint16_t set = sets[i];

                if (set) {
                    World::ObjectSetDoodadEmittersIgnoresDistance(entry->m_object, 1, set);
                    World::ObjectSetDoodadsEnabled(entry->m_object, 0, set);
                }
            }

            if (entry->m_object != this->m_worldObject) {
                continue;
            }

            uint16_t ambient = this->GetAmbientDoodadSet((this->m_owner->m_gameObject->m_flags >> 8) & 0xF);

            if (ambient) {
                World::ObjectSetDoodadsEnabled(this->m_worldObject, 1, ambient);
                World::ObjectSetSequence(this->m_worldObject, 153, 0, ambient);
                World::ObjectSetSequenceFinishedCallback(this->m_worldObject, &CGGameObject_C_Type_DestructibleBuilding::AmbientSequenceFinishedHandler, ambient, ambient);
                World::ObjectSetEventCallback(this->m_worldObject, &CGGameObject_C_Type_DestructibleBuilding::AnimEventCallback, this->m_owner->m_obj->m_guid.m_guid, ambient);
            }
        }

        return;
    }

    if (!this->m_animating) {
        return;
    }

    if (time < this->m_animStartTime + this->m_animDuration) {
        C3Vector position = this->m_owner->GetPosition();
        float progress = static_cast<float>(time - this->m_animStartTime) / static_cast<float>(this->m_animDuration);
        int32_t effect;

        if (!this->m_modelData) {
            effect = 4;
        } else {
            effect = this->m_modelData->m_healEffect;

            if (!effect) {
                float sink = 0.0f;

                if (progress > 0.5f) {
                    float t = 1.0f - ((progress - 0.5f) + (progress - 0.5f));
                    sink = 1.0f - t * (t * t);
                }

                C3Vector target = position;
                target.z = this->m_baseZ - (1.0f - sink) * this->m_sinkDepth;
                World::ObjectUpdate(this->m_worldObject, target, this->m_owner->GetFacing(), 0.0f, 0.0f, 0);

                if (this->m_previousState != -1) {
                    float scale = progress >= 0.5f ? 1.0f : progress * (progress * progress) * 8.0f;
                    C3Vector previous = this->m_owner->GetPosition();
                    previous.z = this->m_baseZ - this->m_previousSinkDepth * scale;
                    World::ObjectUpdate(this->m_states[this->m_previousState].m_object, previous, this->m_owner->GetFacing(), 0.0f, 0.0f, 0);
                }

                goto rebuild;
            }
        }

        {
            C3Vector target = position;
            float remaining = 1.0f - progress;
            target.z = this->m_baseZ - (1.0f - (1.0f - remaining * (remaining * remaining))) * this->m_sinkDepth;
            World::ObjectUpdate(this->m_worldObject, target, this->m_owner->GetFacing(), 0.0f, 0.0f, 0);

            if (this->m_previousState != -1 && effect == 2) {
                C3Vector previous = this->m_owner->GetPosition();
                previous.z = this->m_baseZ - progress * (progress * progress) * this->m_previousSinkDepth;
                World::ObjectUpdate(this->m_states[this->m_previousState].m_object, previous, this->m_owner->GetFacing(), 0.0f, 0.0f, 0);
            }
        }

    rebuild:
        if (this->m_rebuildEffect) {
            World::ObjectSetRenderEnabled(this->m_rebuildEffect, 1);
            World::ObjectEnableCollision(this->m_rebuildEffect, 1);
            World::ObjectUpdate(this->m_rebuildEffect, position, this->m_owner->GetFacing(), 0.0f, 0.0f, 0);
        }

        return;
    }

    if (this->m_rebuildEffect) {
        C3Vector position = this->m_owner->GetPosition();
        World::ObjectUpdate(this->m_rebuildEffect, position, this->m_owner->GetFacing(), 0.0f, 0.0f, 0);
        World::ObjectSetRenderEnabled(this->m_rebuildEffect, 0);
        World::ObjectEnableCollision(this->m_rebuildEffect, 0);
    }

    C3Vector position = this->m_owner->GetPosition();
    position.z = this->m_baseZ;
    World::ObjectUpdate(this->m_worldObject, position, this->m_owner->GetFacing(), 0.0f, 0.0f, 0);
    this->m_animating = 0;

    if (this->m_previousState != -1) {
        World::ObjectSetRenderEnabled(this->m_states[this->m_previousState].m_object, 0);
        World::ObjectEnableCollision(this->m_states[this->m_previousState].m_object, 0);
        World::ObjectUpdate(this->m_states[this->m_previousState].m_object, position, this->m_owner->GetFacing(), 0.0f, 0.0f, 0);
    }

    if (this->m_baseCollision) {
        World::ObjectEnableCollision(this->m_baseCollision, 0);
    }
}

// OFFSET: 0x0059DBC0/0x14020CB40
void CGGameObject_C_Type_DestructibleBuilding::HandleAnimEvent(uint32_t eventId, uint32_t data, const C3Vector& position, uint32_t time) {
    GameObject_C_ProcessAnimEvent(eventId, this->m_owner->m_gameObject->m_displayID, data, position, &this->m_sound);
}

// PROVEN: 427d6342 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void CGGameObject_C_Type_DestructibleBuilding::CreateWorldObj() {
}

// PROVEN: 3411ab45 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void CGGameObject_C_Type_DestructibleBuilding::DestroyWorldObj() {
}

// OFFSET: 0x0059DBF0
bool CGGameObject_C_Type_GuildBank::Use() {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player) {
        WOWGUID guid = this->m_owner->m_obj->m_guid;
        CGPlayer_C::TalkToGuildBanker(guid);
    }

    return true;
}

// OFFSET: 0x0059DC40
void CGGameObject_C_Type_GuildBank::StartInteraction() {
    CGGameObject_C* owner = this->m_owner;

    if (owner->IsObjectModelLoaded()) {
        owner->m_typeData->ActivateOneShotAnim(2);
        owner->m_pendingOneShotAnim = 0;
    } else {
        owner->m_pendingOneShotAnim = 2;
    }
}

// PROVEN: 128c4b18 IDENTICAL/NONE
// OFFSET: 0x008DEAB0
CAaBox CGGameObject_C::GetWorldBounds() const {
    return this->m_worldBounds;
}

// OFFSET: 0x0059DD50
void CGGameObject_C::UpdateWorldObject(int32_t a2) {
    this->m_matrix = C44Matrix();
    this->m_matrix.Translate(this->GetPosition());
    this->m_matrix.Rotate(this->GetRotation());
    this->m_matrix.Scale(this->GetScale());
    CM2Model* model = this->m_worldModel;
    C3Vector center(0.0f, 0.0f, 0.0f);
    CAaBox box(C3Vector(0.0f, 0.0f, 0.0f), C3Vector(0.0f, 0.0f, 0.0f));
    CAaSphere sphere;
    sphere.c = C3Vector(0.0f, 0.0f, 0.0f);
    sphere.r = 0.0f;
    C3Vector position(0.0f, 0.0f, 0.0f);

    if (model) {
        if (model->IsLoaded()) {
            if (!(model->m_shared->m_flags & 0x1)) {
                model->WaitForLoad(NULL);
            }

            CAaBox bounds = model->m_shared->m_data->m_collisionBounds.m_extent;
            CWorldMath::TransformAABox(this->m_matrix, bounds, this->m_worldBounds);
            model->GetCombinedBounds(box, sphere);
            center.x = (bounds.t.x + bounds.b.x) * 0.5f;
            center.y = (bounds.t.y + bounds.b.y) * 0.5f;
            center.z = (bounds.t.z + bounds.b.z) * 0.5f;
            position = center;
        }

        model->m_fFlags |= 0x4000;
        model->m_worldTransform = this->m_matrix;
    }

    if (this->m_worldObject) {
        if (!this->m_gameObject->m_typeID && static_cast<CGGameObject_C_Type_Door*>(this->m_typeData)->m_linkAll) {
            World::ObjectSetLinkAll(this->m_worldObject, 1);
        }

        World::ObjectUpdate(this->m_worldObject, this->m_matrix, box, sphere, position, a2, 0xFFFFFFFF);
    }
}

// PROVEN: 2a41aaae IDENTICAL/NONE
// OFFSET: 0x005A3A40
float CGGameObject_C::GetFacing() const {
    if (this->m_typeData) {
        return this->m_typeData->GetFacing();
    }

    return this->m_passenger.GetFacing(this->m_passenger.GetRawFacingFromRotation());
}

// PROVEN: 881618e2 IDENTICAL/NONE
// OFFSET: 0x005A3A80
float CGGameObject_C::GetRawFacing() const {
    if (this->m_typeData) {
        return this->m_typeData->GetRawFacing();
    }

    return this->m_passenger.GetRawFacingFromRotation();
}

// PROVEN: f60daa33 IDENTICAL/NONE
// OFFSET: 0x005A3AB0
C4Quaternion CGGameObject_C::GetRotation() const {
    return this->m_passenger.GetRotation();
}

// OFFSET: 0x005A3BE0
void CGGameObject_C::ModelLoaded(CM2Model* model) {
    CAaBox box = model->GetBoundingBox();
    this->m_height = (box.t.x < box.b.x || box.t.y < box.b.y || box.t.z < box.b.z) ? 0.0f : box.t.z - box.b.z;

    if ((this->m_modelFlags & 0x40000) && !(this->m_modelFlags & 0x10000)) {
        this->UpdateWorldObject(0);
    }

    if (this->m_typeData->CanBeSolid()) {
        this->m_hasWorldBounds = this->m_worldBounds.t.x > this->m_worldBounds.b.x && this->m_worldBounds.t.y > this->m_worldBounds.b.y && this->m_worldBounds.t.z > this->m_worldBounds.b.z;
    }

    ApplyGameObjectArtKit(model, this->m_gameObject->m_artKit);
    this->m_typeData->ModelLoaded();
    this->m_animKitManager.OnModelLoaded();
    this->UpdateInteractIconAttach();

    if (this->m_modelFlags & 0x400000) {
        this->m_modelFlags |= 0x400000;
        this->ShowLootEffect();
    }

    int32_t pending = this->m_pendingOneShotAnim;

    if (pending) {
        if (this->IsObjectModelLoaded()) {
            this->m_typeData->ActivateOneShotAnim(pending);
            this->m_pendingOneShotAnim = 0;
        } else {
            this->m_pendingOneShotAnim = pending;
        }
    }

    model->m_fFlags |= 0x4;
    this->LoadGuildTextures(this->GetGuildGUID().m_guid);
}

// OFFSET: 0x0059A5A0/0x140208D50
float CGGameObject_C_TypeBase::GetRawFacing() const {
    return this->m_owner->m_passenger.GetRawFacingFromRotation();
}

// PROVEN: 3c026556 IDENTICAL/NONE
// OFFSET: 0x0059A580/0x140208D20
float CGGameObject_C_TypeBase::GetFacing() const {
    CPassenger& passenger = this->m_owner->m_passenger;
    return passenger.GetFacing(passenger.GetRawFacingFromRotation());
}

// OFFSET: 0x0059BE40/0x14020A8E0
int32_t CGGameObject_C_TypeAnimated::HandleAnimInterrupted() {
    if (!this->UpdateObjectLock()) {
        return 0;
    }

    this->m_pendingAnimState = -1;
    return 1;
}

// OFFSET: 0x0059E240/0x14020D3E0
LockRec* CGGameObject_C::GetLockRec() const {
    if (!this->m_stats) {
        return NULL;
    }

    uint32_t lockId = this->GetPropertyValue(CGameObjectDef::GetPropNum(this->m_gameObject->m_typeID, 4));
    return lockId ? g_lockDB.GetRecord(lockId) : NULL;
}

// OFFSET: 0x0059E320
bool CGGameObject_C::IsValidTargetForSpell(const WOWGUID& item, int32_t spellId) const {
    LockRec* lock = this->GetLockRec();

    if (!lock) {
        return false;
    }

    uint32_t count;
    const TSGrowableArray<const SpellEffectRec*>* effects = SpellGetEffects(spellId, count);

    if (!count) {
        return false;
    }

    CGItem_C* key = static_cast<CGItem_C*>(ClntObjMgrObjectPtr(item, TYPEMASK_ITEM, __FILE__, __LINE__));

    for (uint32_t i = 0; i < count; i++) {
        const SpellEffectRec* effect = effects->m_data[i];

        if (!effect || effect->m_effect != 33) {
            continue;
        }

        for (int32_t j = 0; j < 8; j++) {
            switch (lock->m_type[j]) {
                case 1:
                    if (!key || static_cast<int32_t>(key->m_obj->m_entryID) != lock->m_index[j]) {
                        continue;
                    }

                    break;

                case 2:
                    if (effect->m_effectMiscValue != lock->m_index[j]) {
                        continue;
                    }

                    break;

                case 3:
                    if (spellId != lock->m_index[j]) {
                        continue;
                    }

                    break;

                default:
                    continue;
            }

            if (this->IsValidOpenAction(lock->m_action[j])) {
                return true;
            }
        }
    }

    return false;
}

// OFFSET: 0x0059EB00
int32_t CGGameObject_C::CanChangeCursor() const {
    if ((!CGGameObject_C::s_cvForceMouseOver || !CGGameObject_C::s_cvForceMouseOver->m_intValue) && !this->m_typeData->CanUse()) {
        return 0;
    }

    if (!this->m_stats || !this->m_stats->m_iconName || !*this->m_stats->m_iconName) {
        return this->m_typeData->CanChangeCursor();
    }

    char name[40];

    if (this->CanUseNow()) {
        SStrCopy(name, this->m_stats->m_iconName, sizeof(name));
    } else {
        SStrPrintf(name, sizeof(name), "Unable%s", this->m_stats->m_iconName);
    }

    int32_t mode = CursorGetModeFromName(name);
    return mode ? mode : 53;
}

// OFFSET: 0x005A1FC0/0x140212160
void CGGameObject_C_Type_TransportBase::AddPassenger(CPassenger* passenger, int32_t a2) {
    passenger->m_link.Unlink();
    this->m_passengers.LinkToTail(passenger);

    if (a2 && (passenger->unk_002C & 1) && (!this->m_worldObject || !World::ObjectIsLoaded(this->m_worldObject))) {
        static_cast<CMovementShared*>(passenger)->SetSplineEnabled();
    }

    if (*passenger->m_ownerGuid == CGUnit_C::s_activeMover) {
        MovementSetTransportUpdateTime(this->m_transportUpdateTime);
    }
}

// OFFSET: 0x005A0750/0x1402101E0
int32_t CGGameObject_C::GetPageTextLanguage() const {
    return this->GetPropertyValue(CGameObjectDef::GetPropNum(this->m_gameObject->m_typeID, 16));
}

// OFFSET: #0x00710300
int32_t CGGameObject_C::GetPageTextMaterial() const {
    return this->GetPropertyValue(CGameObjectDef::GetPropNum(this->m_gameObject->m_typeID, 17));
}

// OFFSET: 0x0059DFA0
void CGGameObject_C::Reenable(const CClientObjCreate& create) {
    if (create.m_hasGOTransport) {
        this->m_passenger.Reenable(create.m_goTransport.m_guid, create.m_goTransport.m_position);
    } else {
        WOWGUID none;
        this->m_passenger.Reenable(none, create.m_stationaryPosition);
    }

    *reinterpret_cast<uint64_t*>(&this->m_passenger.m_facing) = create.m_hasRotation ? create.m_packedRotation : 0;
    this->m_passenger.unk_002C |= 0x2;
    CGObject_C::Reenable(create);

    if (this->m_stats) {
        this->m_typeData->Reenable();
    } else {
        ClntObjMgrHideObject(this->m_obj->m_guid);
    }

    if (this->m_gameObject->m_typeID == 33) {
        if (this->m_nameDesc) {
            PlayerNameDelete(this->m_nameDesc);
        }

        WOWGUID guid = this->m_obj->m_guid;
        this->m_nameDesc = ObjectNameCreate(guid);
    }

    this->m_animKitID = create.m_hasAnimKits ? create.m_animKits[0] : 0;
    this->m_typeData->ActivateOneShotAnimKit(this->m_animKitID);
}

// PROVEN: 8fb6a007 IDENTICAL/NONE
// OFFSET: 0x0059E0B0/0x14020D240
int32_t CGGameObject_C::ObjectReaction(const CGUnit_C* unit) const {
    CGUnit_C* creator = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(this->m_gameObject->m_createdBy, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (creator) {
        return creator->UnitReaction(unit);
    }

    if (this->m_gameObject->m_faction) {
        return CGUnit_C::UnitReaction(this->m_gameObject->m_faction, unit);
    }

    return 3;
}

// OFFSET: #0x0070F850
bool CGGameObject_C::IsTracked() const {
    return CGMinimapFrame::s_trackingInfo && CGMinimapFrame::s_trackingInfo->m_type == 2 && CGMinimapFrame::s_trackingInfo->m_npcFlags == static_cast<uint32_t>(this->m_gameObject->m_typeID);
}

// OFFSET: 0x0059EE80
void CGGameObject_C::AddWorldDamageText(int32_t amount) {
    if (amount && g_combatDamageCVar->m_intValue && this->m_gameObject->m_typeID == 33) {
        char text[32];
        SStrPrintf(text, sizeof(text), "%d", amount);
        PlayerNameCreateText(this->m_nameDesc, 8, text, reinterpret_cast<const CImVector*>(&s_buildingDamageColor), NULL);
    }
}

// OFFSET: 0x0059EEE0
void CGGameObject_C::AddWorldHealingText(int32_t amount, const SpellRec* spell) {
    if (amount && g_combatHealingCVar->m_intValue && this->m_gameObject->m_typeID == 33) {
        char text[32];
        SStrPrintf(text, sizeof(text), "+%d", amount);
        PlayerNameCreateText(this->m_nameDesc, 9, text, NULL, NULL);
    }
}

// OFFSET: 0x005A3080/0x140213620
void CGGameObject_C::SetStats(GameObjectStats_C* stats) {
    this->m_stats = stats;
    this->m_typeData->PostInitWithStats();

    GameObjectDisplayInfoRec* displayInfo = g_gameObjectDisplayInfoDB.GetRecord(this->m_stats->m_displayID);

    if (displayInfo && displayInfo->m_objectEffectPackageID) {
        void* mem = SMemAlloc(sizeof(CObjectEffectManager), __FILE__, __LINE__, 0);
        this->m_effectManager = mem ? new (mem) CObjectEffectManager() : NULL;

        if (!this->m_effectManager->Init(displayInfo->m_objectEffectPackageID, this)) {
            delete this->m_effectManager;
            this->m_effectManager = NULL;
        }
    }

    this->CheckForPassiveHighlight();
}

// OFFSET: 0x0059BC60/0x14020A670
void CGGameObject_C_TypeAnimated::UpdateAnimState(uint32_t state) {
    if (CGGameObject_C::s_cvUseAnimKits && CGGameObject_C::s_cvUseAnimKits->m_intValue) {
        return;
    }

    if (state == this->m_animState) {
        return;
    }

    CM2Model* model = NULL;

    if (this->m_owner->IsObjectModelLoaded()) {
        model = this->m_owner->m_worldModel;

        if (model && this->m_owner->m_gameObject->m_pathProgress == 0xFFFF) {
            int32_t current = this->m_animState;

            if ((current == 2 && state == 4) || (current == 4 && state == 2) || (current == 5 && state == 7) || (current == 7 && state == 5)) {
                M2BoneSequenceInfo info;
                model->GetBoneSequenceInfo(0xFFFFFFFF, info);

                if (static_cast<int32_t>(info.m_endTime - info.m_startTime) > 0) {
                    float progress = static_cast<float>(model->m_scene->m_time - info.m_startTime) / static_cast<float>(info.m_endTime - info.m_startTime);

                    if (progress <= 1.0f) {
                        this->m_owner->SetPathProgress(1.0f - progress);
                    }
                }
            }
        }
    }

    this->m_animState = state;

    if (model) {
        this->SetSequence(model);
    }
}

// OFFSET: 0x005A2060/0x140212220
void CGGameObject_C_Type_MapObjTransport::PostInitWithStats() {
    this->CreateWorldObj();

    if (this->m_worldObject) {
        MovementAddTransport(this->m_owner);
    }

    uint32_t physicsIndex = CGameObjectDef::GetPropNum(this->m_owner->m_gameObject->m_typeID, 99);
    GameObjectStats* physicsStats = this->m_owner->m_stats;
    int32_t physicsId = (physicsStats && physicsIndex < 32) ? physicsStats->m_propValue[physicsIndex] : 0;
    TransportPhysicsRec* physics = g_transportPhysicsDB.GetRecord(physicsId);

    uint32_t accelRateIndex = CGameObjectDef::GetPropNum(this->m_owner->m_gameObject->m_typeID, 43);
    GameObjectStats* accelRateStats = this->m_owner->m_stats;
    int32_t accelRate = (accelRateStats && accelRateIndex < 32) ? accelRateStats->m_propValue[accelRateIndex] : 0;

    uint32_t moveSpeedIndex = CGameObjectDef::GetPropNum(this->m_owner->m_gameObject->m_typeID, 36);
    GameObjectStats* moveSpeedStats = this->m_owner->m_stats;
    int32_t moveSpeed = (moveSpeedStats && moveSpeedIndex < 32) ? moveSpeedStats->m_propValue[moveSpeedIndex] : 0;

    uint32_t taxiPathIndex = CGameObjectDef::GetPropNum(this->m_owner->m_gameObject->m_typeID, 35);
    GameObjectStats* taxiPathStats = this->m_owner->m_stats;
    uint32_t taxiPathId = (taxiPathStats && taxiPathIndex < 32) ? taxiPathStats->m_propValue[taxiPathIndex] : 0;

    this->m_shipPath.Init(taxiPathId, static_cast<float>(moveSpeed), static_cast<float>(accelRate), physics);
    this->m_shipPath.SetTotalPathTime(this->m_owner->m_gameObject->m_level);
    this->InitializeMovement();

    if (this->m_worldObject) {
        World::ObjectUpdate(this->m_worldObject, this->m_owner->GetMatrix());
    }
}

// OFFSET: 0x0059B760/0x14020A0A0
void CGGameObject_C_TypeAnimated::SetSequence(CM2Model* model) {
    if (CGGameObject_C::s_cvUseAnimKits && CGGameObject_C::s_cvUseAnimKits->m_intValue) {
        return;
    }

    float speed = 1.0f;
    uint32_t sequence = CGGameObject_C_TypeAnimated::s_animSequenceIds[this->m_animState];

    if (model->IsLoaded() && !model->HasSequence(sequence)) {
        int32_t fallback = 0;

        switch (sequence) {
            case 146:
                if (!model->HasSequence(148)) {
                    sequence = 147;
                }

                break;

            case 147:
                if (!model->HasSequence(146)) {
                    if (model->HasSequence(148)) {
                        sequence = 148;
                        speed = 0.0f;
                    } else {
                        sequence = 0;
                        fallback = 1;
                    }
                }

                break;

            case 148:
                if (!model->HasSequence(146)) {
                    sequence = (model->HasSequence(150) != 0) + 149;
                }

                break;

            case 149:
                if (!model->HasSequence(148)) {
                    if (model->HasSequence(146)) {
                        sequence = 146;
                        speed = 0.0f;
                    } else {
                        sequence = 151;
                    }
                }

                break;

            default:
                break;
        }

        if (!fallback && sequence == model->GetBoneSequenceId(0xFFFFFFFF)) {
            return;
        }
    }

    if (this->m_owner->m_effectManager) {
        uint32_t current = model->GetBoneSequenceId(0xFFFFFFFF);

        if (current != sequence) {
            int32_t exitState = FindObjectEffectStateFromAnimID(current, 0);

            if (exitState) {
                this->m_owner->m_effectManager->ExitState(exitState, 1);
            }

            int32_t enterState = FindObjectEffectStateFromAnimID(sequence, 0);

            if (enterState) {
                this->m_owner->m_effectManager->EnterState(enterState, 1, 0);
            }
        }
    }

    M2SequenceInfo info;
    memset(&info, 0, sizeof(info));
    model->GetSequenceInfo(sequence, 0, info);

    int32_t state = this->m_animState;
    uint32_t startTime = 0;
    int32_t paused;

    if (state == 2 || state == 4 || state == 5 || state == 7) {
        CGGameObjectData* data = this->m_owner->m_gameObject;
        paused = (data->m_flags >> 7) & 1;
        uint16_t progress = data->m_pathProgress;

        if (progress != 0xFFFF) {
            startTime = static_cast<int32_t>(static_cast<double>(progress) * static_cast<double>(info.m_duration) * 0.000015259022);
            data->m_pathProgress = static_cast<int32_t>(65535.0f);
        }
    } else {
        paused = 0;
    }

    model->SetBoneSequence(0xFFFFFFFF, sequence, 0xFFFFFFFF, startTime, speed, this->m_animState != 0, 1, 0, 0, 0);
    this->SetAnimPaused(model, paused);
    state = this->m_animState;

    if (state != 3 && state != 1 && state != 6) {
        ClntObjMgrLockObject(this->m_owner->m_obj->m_guid);
    }

    this->m_pendingAnimState = this->m_animState;

    if (speed > 0.0f) {
        uint32_t time = CGWorldFrame::s_currentWorldFrame->GetSceneTime();
        this->m_nextAnimTime = static_cast<int64_t>(static_cast<double>(info.m_duration - startTime) / speed) + time;
    } else {
        this->m_nextAnimTime = CGWorldFrame::s_currentWorldFrame->GetSceneTime();
    }
}

// OFFSET: 0x005A08A0
bool CGGameObject_C::IsCreatorOnlyUse() const {
    uint32_t creatorOnlyIndex = CGameObjectDef::GetPropNum(this->m_gameObject->m_typeID, 94);
    GameObjectStats* creatorOnlyStats = this->m_stats;
    return creatorOnlyStats && creatorOnlyIndex < 24 && creatorOnlyStats->m_propValue[creatorOnlyIndex];
}

// OFFSET: 0x005A16B0/0x1402116D0
bool CGGameObject_C_TypeBase::CanUse() const {
    CGUnit_C* player = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return false;
    }

    if (this->m_owner->m_gameObject->m_typeID == 6) {
        if (!this->m_owner->GetLockRec() || this->m_owner->ObjectReaction(player) > 1) {
            return false;
        }

        WOWGUID creatorGuid = this->m_owner->m_gameObject->m_createdBy;

        if (!(creatorGuid.m_guid & 0xF000000000000000ULL) && (creatorGuid.m_guid & 0xF07FFFFFFFFFFFFFULL)) {
            CGUnit_C* creator = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(creatorGuid, TYPEMASK_PLAYER, __FILE__, __LINE__));

            if (!creator) {
                return false;
            }

            bool ffa = (creator->m_unit->m_unitFieldBytes2[1] & 0x4) && (!CGPartyInfo::InParty() || (!CGPartyInfo::IsMember(creator->m_obj->m_guid) && !IsGuidInMyRaid(creator->m_obj->m_guid)));

            if (!(creator->m_unit->m_unitFieldBytes2[1] & 0x1) && !creator->IsDuelingWith(player) && !ffa) {
                return false;
            }
        }
    } else if (this->m_owner->ObjectReaction(player) <= 1) {
        return false;
    }

    CGGameObject_C* owner = this->m_owner;
    uint16_t dynamicFlags = owner->m_gameObject->m_dynamicFlags;

    if (dynamicFlags & 0x4) {
        return false;
    }

    uint32_t flags = owner->m_gameObject->m_flags;

    if ((flags & 0x11) || (owner->m_modelFlags & 0x100000) || ((flags & 0x4) && !(dynamicFlags & 0x1))) {
        return false;
    }

    if (owner->IsCreatorOnlyUse() && player->m_obj->m_guid != this->m_owner->m_gameObject->m_createdBy) {
        return false;
    }

    return !(this->m_owner->m_gameObject->m_dynamicFlags & 0x20);
}

// OFFSET: 0x0059BE10/0x14020A890
void CGGameObject_C_TypeAnimated::HandleAnimEvent(uint32_t eventId, uint32_t data, const C3Vector& position, uint32_t time) {
    GameObject_C_ProcessAnimEvent(eventId, this->m_owner->m_gameObject->m_displayID, data, position, &this->m_sound);
}

// OFFSET: 0x005A1440/0x1402113E0
uintptr_t CGGameObject_C::CreateWorldObject(uint64_t guid, int32_t a4, uint32_t doodadSetCount, const int16_t* doodadSets, int32_t a7) {
    const char* name = this->GetModelFileNameInternal();

    if (!name) {
        return 0;
    }

    C3Vector position = this->m_passenger.GetPosition(this->m_passenger.m_position);
    uintptr_t object = World::ObjectCreate(name, position, this->GetFacing(), a4, 0, guid, 0, doodadSetCount, reinterpret_cast<uint16_t*>(const_cast<int16_t*>(doodadSets)), NULL, a7);

    if (!object) {
        SysMsgPrintf(STATUS_ERROR, 2, "Game object (%d): failed to load: \"%s\"", this->m_obj->m_entryID, name);
    }

    return object;
}

// OFFSET: 0x005A1F00/0x140212090
void CGGameObject_C_Type_WorldObj::CreateWorldObj() {
    switch (this->m_owner->m_gameObject->m_typeID) {
        case 11:
        case 15:
        case 35:
            this->m_worldObject = this->m_owner->CreateWorldObject(this->m_owner->m_obj->m_guid.m_guid, 0, 0, NULL, 0);

            if (this->m_worldObject) {
                int8_t type = this->m_owner->m_gameObject->m_typeID;

                if (type == 11) {
                    World::ObjectSetAcceptProjTex(this->m_worldObject, 1);
                    World::ObjectSetMoveable(this->m_worldObject, 1);
                } else if (type == 15) {
                    World::ObjectSetMoveable(this->m_worldObject, 1);
                }
            }

            break;

        case 33:
            return;

        default:
            this->m_worldObject = this->m_owner->CreateWorldObject(0, 0, 0, NULL, 0);
            break;
    }
}

// OFFSET: 0x0059FF60/0x14020F930
void CGGameObject_C_Type_TransportBase::Disable() {
    CGCamera* camera = CGWorldFrame::GetActiveCamera();
    uint32_t time = OsGetAsyncTimeMs();

    for (CPassenger* passenger = this->m_passengers.Head(); passenger; ) {
        CPassenger* next = this->m_passengers.Next(passenger);

        if (camera->m_targetGUID == *passenger->m_ownerGuid) {
            camera->MakeRelativeTo(WOWGUID());
        }

        if (passenger->unk_002C & 0x1) {
            CMovement_C* movement = static_cast<CMovement_C*>(passenger);
            movement->ClearSplineEnabled();

            if (*passenger->m_ownerGuid == CGUnit_C::s_activeMover) {
                movement->OnFallLocal(time);
            } else {
                movement->TryStartFallingLocal();
            }

            movement->ForceSetTransport(WOWGUID(), 255, 1);
        } else {
            passenger->ForceSetTransport(WOWGUID());
        }

        passenger = next;
    }

    if (this->m_worldObject) {
        MovementRemoveTransport(this->m_owner);
    }

    this->DestroyWorldObj();
}

// OFFSET: #0x007100D0
void CGGameObject_C_Type_MapObjTransport::InitializeMovement() {
    uint32_t time = OsGetAsyncTimeMs();
    CGGameObject_C* owner = this->m_owner;
    uint32_t index = CGameObjectDef::GetPropNum(owner->m_gameObject->m_typeID, 128);
    GameObjectStats* stats = owner->m_stats;

    if (stats && index < 24 && stats->m_propValue[index]) {
        uint32_t offsetTime = time + this->m_owner->m_transportTimeOffset;
        this->m_shipPath.SetTimeOffset(offsetTime, this->m_owner->m_gameObject->m_pathProgress * 0.000015259022f);
        CGGameObjectData* gameObject = this->m_owner->m_gameObject;

        if (gameObject->m_dynamicFlags & 0x10) {
            this->m_shipPath.WaitAtCurrentLocation(offsetTime);
        } else {
            this->m_shipPath.WaitAtNextPausePoint(offsetTime, gameObject->m_state == 1);
        }
    }

    this->UpdateMovement(time, 0);
}

// OFFSET: 0x0059BE60/0x14020A910
void CGGameObject_C_TypeAnimated::HandleAnimFinished() {
    if (CGGameObject_C::s_cvUseAnimKits && CGGameObject_C::s_cvUseAnimKits->m_intValue) {
        return;
    }

    int32_t state = this->m_animState;
    int32_t pending = this->m_pendingAnimState;
    this->m_pendingAnimState = -1;

    switch (state) {
        case 0:
        case 8:
        case 9:
        case 10:
        case 11:
            if (!(this->m_owner->m_modelFlags & 0x100000)) {
                this->UpdateState(this->m_owner->m_displayState, this->m_owner->m_displayState);
            }

            break;

        case 1:
        case 3:
        case 6:
            this->SetSequence(this->m_owner->m_worldModel);
            break;

        case 2:
            this->UpdateAnimState(3);
            break;

        case 4:
        case 7:
            this->UpdateAnimState(1);
            break;

        case 5:
            this->UpdateAnimState(6);
            break;

        case 12:
            break;

        default:
            OsOutputDebugString("Unhandled anim state: (%d)", state);
            break;
    }

    if (pending != -1 && pending != 3 && pending != 1 && pending != 6) {
        ClntObjMgrUnlockObject(this->m_owner->m_obj->m_guid, __FILE__, __LINE__);
    }
}

// OFFSET: 0x0059A110/0x140208680
static void GameObject_C_PlayAnimatedSound(uint32_t displayId, int32_t index, const C3Vector& position, uint32_t* sound) {
    if (index == -1) {
        return;
    }

    const GameObjectDisplayInfoRec* displayInfo = g_gameObjectDisplayInfoDB.GetRecord(displayId);

    if (!displayInfo) {
        return;
    }

    int32_t soundKitId = displayInfo->m_sound[index];

    if (!SE3::IsLooping_ByDef(soundKitId)) {
        SE3SoundKitProperties properties;
        properties.m_maxAudibleBehavior = 1;
        properties.m_priority = 128;
        SE3::PlaySoundKit(soundKitId, &position, &properties, NULL, __FILE__, __LINE__, "GameObject_C_PlayAnimatedSound() - One Shot");
        return;
    }

    if (!sound || SE3::IsPlaying(*sound) || SE3::IsTooCloseToExistingSoundKit(soundKitId, position)) {
        return;
    }

    SE3SoundKitProperties properties;
    properties.m_bit0 = 1;
    properties.m_software = 1;
    properties.m_usage = 1;
    properties.m_maxAudibleBehavior = 1;
    properties.m_priority = 128;
    SE3::PlaySoundKit(soundKitId, &position, &properties, sound, __FILE__, __LINE__, "GameObject_C_PlayAnimatedSound() - Looping");
}

// OFFSET: 0x0059A230/0x140208860
void GameObject_C_ProcessAnimEvent(uint32_t eventId, uint32_t displayId, uint32_t data, const C3Vector& position, uint32_t* sound) {
    switch (eventId) {
        case '0OG$':
            GameObject_C_PlayAnimatedSound(displayId, 0, position, sound);
            break;

        case '1OG$':
            GameObject_C_PlayAnimatedSound(displayId, 1, position, sound);
            break;

        case '2OG$':
            GameObject_C_PlayAnimatedSound(displayId, 2, position, sound);
            break;

        case '3OG$':
            GameObject_C_PlayAnimatedSound(displayId, 3, position, sound);
            break;

        case '4OG$':
            GameObject_C_PlayAnimatedSound(displayId, 4, position, sound);
            break;

        case '5OG$':
            GameObject_C_PlayAnimatedSound(displayId, 5, position, sound);
            break;

        case '0CG$':
            GameObject_C_PlayAnimatedSound(displayId, 6, position, sound);
            break;

        case '1CG$':
            GameObject_C_PlayAnimatedSound(displayId, 7, position, sound);
            break;

        case '2CG$':
            GameObject_C_PlayAnimatedSound(displayId, 8, position, sound);
            break;

        case '3CG$':
            GameObject_C_PlayAnimatedSound(displayId, 9, position, sound);
            break;

        case 'OSD$': {
            SE3SoundKitProperties properties;
            SE3::PlaySoundKit(data, &position, &properties, NULL, __FILE__, __LINE__, "GameObject Anim Event $DSO");
            break;
        }

        case 'LSD$':
            if (sound && !SE3::IsPlaying(*sound) && !SE3::IsTooCloseToExistingSoundKit(data, position)) {
                SE3SoundKitProperties properties;
                properties.m_bit0 = 1;
                properties.m_software = 1;
                properties.m_usage = 1;
                properties.m_maxAudibleBehavior = 1;
                properties.m_priority = 128;
                properties.m_fadeInTime = 1000;
                properties.m_fadeOutTime = 1000;
                SE3::PlaySoundKit(data, &position, &properties, sound, __FILE__, __LINE__, "GameObject Anim Event $DSL");
            }

            break;

        case 'DNS$': {
            SE3SoundKitProperties properties;
            SE3::PlaySoundKit(data, &position, &properties, NULL, __FILE__, __LINE__, "GameObject Anim Event $SND");
            break;
        }

        case 'KHS$': {
            const SpellEffectCameraShakesRec* shakes = g_spellEffectCameraShakesDB.GetRecord(data);

            if (shakes) {
                for (int32_t i = 0; i < 3; i++) {
                    CGWorldFrame::GetActiveCamera()->AddShakeById(shakes->m_cameraShake[i], &position);
                }
            }

            break;
        }
    }
}

// OFFSET: 0x005A0330/0x14020FD80
void CGGameObject_C_Type_MapObjTransport::UpdateMovement(uint32_t time, uint32_t elapsed) {
    C3Vector position(0.0f, 0.0f, 0.0f);
    int32_t mapId = -1;
    int32_t initial = this->m_transportState == -1;
    uint32_t pathTime = this->m_owner->m_transportTimeOffset + time;
    uint32_t sequence;
    float facing;
    uint32_t leg;
    float roll;
    float pitch;
    this->m_shipPath.GetPosition(pathTime, elapsed, &mapId, &sequence, &position, &facing, &leg, &roll, &pitch, 0);

    if (mapId != static_cast<int32_t>(ClntObjMgrGetMapID())) {
        CGUnit_C* mover = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(CGUnit_C::s_activeMover, TYPEMASK_UNIT, __FILE__, __LINE__));

        if (mover && mover->GetTransportGUID() == this->m_owner->m_obj->m_guid.m_guid) {
            MovementSetTransportUpdateTime(this->m_transportUpdateTime);
        }

        return;
    }

    this->m_pathTime = pathTime;
    this->m_transportUpdateTime = this->GetSequenceTimeFromRealTime(pathTime);
    this->UpdateCurrentVelocity(elapsed, position, NULL);
    this->m_position = position;
    C44Matrix& matrix = this->m_owner->m_matrix;
    matrix.d3 = 1.0f;
    matrix.c2 = 1.0f;
    matrix.b1 = 1.0f;
    matrix.a0 = 1.0f;
    matrix.d2 = 0.0f;
    matrix.d1 = 0.0f;
    matrix.d0 = 0.0f;
    matrix.c3 = 0.0f;
    matrix.c1 = 0.0f;
    matrix.c0 = 0.0f;
    matrix.b3 = 0.0f;
    matrix.b2 = 0.0f;
    matrix.b0 = 0.0f;
    matrix.a3 = 0.0f;
    matrix.a2 = 0.0f;
    matrix.a1 = 0.0f;
    matrix.d0 = this->m_position.x;
    matrix.d1 = this->m_position.y;
    matrix.d2 = this->m_position.z;
    matrix.RotateAroundZ(facing);
    matrix.RotateAroundY(pitch);
    matrix.RotateAroundX(roll);
    CGGameObject_C* owner = this->m_owner;
    C4Quaternion rotation;
    rotation.x = 0.0f;
    rotation.y = 0.0f;
    rotation.z = 0.0f;
    rotation.w = 0.0f;
    rotation.FromRotationMatrix(owner->m_matrix);
    reinterpret_cast<C4QuaternionCompressed&>(owner->m_passenger.m_facing) = rotation;
    owner->m_passenger.unk_002C |= 0x2;

    if (this->m_worldObject) {
        World::ObjectUpdate(this->m_worldObject, matrix);

        if (!this->m_isLoaded && World::ObjectIsLoaded(this->m_worldObject)) {
            World::ObjectSetSequenceFinishedCallback(this->m_worldObject, CGGameObject_C_Type_WorldObj::SequenceFinishedHandler, 0, 0);
            this->MarkTransportLoaded();
        }

        if (this->m_isLoaded && this->m_transportState != static_cast<int32_t>(sequence)) {
            World::ObjectSetSequence(this->m_worldObject, sequence, 0, 0);
            this->m_transportState = sequence;
            this->UpdateObjectEffectState(sequence);
        }
    }

    int32_t moved;
    this->MovePassengers(&moved);
    uint32_t previousLeg = this->m_currentLeg;

    if (previousLeg == leg) {
        return;
    }

    this->m_currentLeg = leg;

    if (!moved || initial) {
        return;
    }

    CGUnit_C* target = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(CGWorldFrame::GetActiveCamera()->m_targetGUID, TYPEMASK_UNIT, __FILE__, __LINE__));
    AsyncFileReadProgressCallback(LoadingScreenAsyncCallback, NULL);
    World::SetLoadProgressCallback(LoadingScreenWorldCallback, NULL);
    LoadingScreenEnableShipLeg(this->m_owner->m_obj->m_entryID, previousLeg);
    World::Preload(target->GetPosition(), 0);
    CGObject_C::UpdateAllWorldObjects();
    LoadingScreenDisable();
    AsyncFileReadProgressCallback(NULL, NULL);
    World::SetLoadProgressCallback(NULL, NULL);
}

// PROVEN: f4264bef IDENTICAL/NONE
// OFFSET: 0x005A45D0/0x140215140
uint32_t CGGameObject_C_Type_MapObjTransport::GetSequenceTimeFromRealTime(uint32_t time) const {
    return this->m_shipPath.GetPathTime(time);
}

// OFFSET: 0x005A0050
void CGGameObject_C_Type_TransportBase::MarkTransportLoaded() {
    this->m_isLoaded = 1;

    for (CPassenger* passenger = this->m_passengers.Head(); passenger; passenger = this->m_passengers.Next(passenger)) {
        if (passenger->unk_002C & 0x1) {
            CMovementShared* movement = static_cast<CMovementShared*>(passenger);

            if (movement->m_flags2 & MOVEMENTFLAG2_INTERPOLATED_PITCHING) {
                movement->ClearInterpolatedPitching();
            }
        }
    }
}

// OFFSET: 0x005A00A0
void CGGameObject_C_Type_TransportBase::MovePassengers(int32_t* moved) {
    *moved = 0;
    C44Matrix* matrix = &this->m_owner->m_matrix;
    int32_t isWMO = 0;

    if (this->m_worldObject) {
        World::ObjectUpdate(this->m_worldObject, *matrix);
        isWMO = World::ObjectIsWMO(this->m_worldObject);
    }

    WOWGUID cameraTarget = CGWorldFrame::GetActiveCamera()->m_targetGUID;
    CGUnit_C* mover = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(CGUnit_C::s_activeMover, TYPEMASK_UNIT, __FILE__, __LINE__));

    for (CPassenger* passenger = this->m_passengers.Head(); passenger; passenger = this->m_passengers.Next(passenger)) {
        CGObject_C* object = ClntObjMgrObjectPtr(*passenger->m_ownerGuid, TYPEMASK_OBJECT, __FILE__, __LINE__);

        if (passenger->unk_002C & 0x1) {
            CGUnit_C* unit = static_cast<CGUnit_C*>(object);
            CVehicle_C* vehicle = unit->m_vehicle;

            if (vehicle && vehicle->m_vehicleRec) {
                vehicle->BuildWorldMatrix(matrix);
            }

            unit->UpdateSoundPosition();
        }

        if (*passenger->m_ownerGuid == cameraTarget) {
            *moved = 1;
        }

        object->UpdateWorldObject(object != mover && isWMO);
        object->SetFrameOfReference(matrix);
    }

    if (mover && mover->GetTransportGUID() == this->m_owner->m_obj->m_guid.m_guid) {
        MovementSetTransportUpdateTime(this->m_transportUpdateTime);
    }
}

// OFFSET: 0x0059DB40
C3Vector CGGameObject_C_Type_DestructibleBuilding::GetNamePosition() const {
    uintptr_t object = this->m_states[((this->m_owner->m_gameObject->m_flags >> 8) & 0xE) >> 1].m_object;
    CAaBox extents;
    extents.b = C3Vector(0.0f, 0.0f, 0.0f);
    extents.t = C3Vector(0.0f, 0.0f, 0.0f);
    World::ObjectGetExtents(object, extents);
    C3Vector position = this->GetPosition();
    position.z = extents.t.y - extents.b.y + position.z;
    return position;
}

// OFFSET: 0x0059E200
uint32_t CGGameObject_C::GetProperty(int32_t prop) const {
    uint32_t index = CGameObjectDef::GetPropNum(this->m_gameObject->m_typeID, prop);

    if (this->m_stats && index < 32) {
        return this->m_stats->m_propValue[index];
    }

    return 0;
}

// OFFSET: 0x0059E460/0x14020D660
bool CGGameObject_C::IsLocked(int32_t* spellId, int32_t* skillValue, int32_t* requiredSkill, CGItem_C** key, uint32_t* entry) const {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return false;
    }

    LockRec* lock = this->GetLockRec();

    if (!lock) {
        return false;
    }

    bool locked = false;

    for (uint32_t i = 0; i < 8; i++) {
        int32_t type = lock->m_type[i];

        if (!type) {
            continue;
        }

        if (type == 2) {
            locked = true;

            if (!this->IsValidOpenAction(lock->m_action[i])) {
                continue;
            }

            for (uint32_t j = 0; j < s_openLockSpells.m_count; j++) {
                const SpellRec* spell = g_spellDB.GetRecord(s_openLockSpells.m_data[j]);
                uint32_t count;
                const TSGrowableArray<const SpellEffectRec*>* effects = SpellGetEffects(spell->m_id, count);

                for (uint32_t effect = 0; effect < count; effect++) {
                    const SpellEffectRec* rec = effects->m_data[effect];

                    if (!rec || rec->m_effect != 33 || rec->m_effectMiscValue != lock->m_index[i]) {
                        continue;
                    }

                    int32_t required = lock->m_skill[i] ? lock->m_skill[i] : 5 * this->m_gameObject->m_level;
                    int32_t minPoints;
                    int32_t maxPoints;
                    Spell_C_GetMinMaxPoints(spell, effect, &minPoints, &maxPoints, 0, 0, 0, 0);

                    if (spellId) {
                        *spellId = s_openLockSpells.m_data[j];
                    }

                    if (skillValue) {
                        *skillValue = minPoints;
                    }

                    if (requiredSkill) {
                        *requiredSkill = required;
                    }

                    if (minPoints >= required) {
                        if (entry) {
                            *entry = i;
                        }

                        return false;
                    }
                }

                if (Spell_C_IsTargeting()) {
                    int32_t targetingSpell = Spell_C_GetTargetingSpell();
                    const SpellRec* targeting = g_spellDB.GetRecord(targetingSpell);
                    uint32_t targetingCount;
                    const TSGrowableArray<const SpellEffectRec*>* targetingEffects = SpellGetEffects(targeting->m_id, targetingCount);

                    for (uint32_t effect = 0; effect < targetingCount; effect++) {
                        const SpellEffectRec* rec = targetingEffects->m_data[effect];

                        if (!rec || rec->m_effect != 33 || rec->m_effectMiscValue != lock->m_index[i]) {
                            continue;
                        }

                        int32_t required = lock->m_skill[i] ? lock->m_skill[i] : 5 * this->m_gameObject->m_level;
                        int32_t minPoints;
                        int32_t maxPoints;
                        Spell_C_GetMinMaxPoints(targeting, effect, &minPoints, &maxPoints, 0, 0, 0, 0);

                        if (spellId) {
                            *spellId = targetingSpell;
                        }

                        if (skillValue) {
                            *skillValue = minPoints;
                        }

                        if (requiredSkill) {
                            *requiredSkill = required;
                        }

                        if (minPoints >= required) {
                            if (entry) {
                                *entry = i;
                            }

                            return false;
                        }
                    }
                }
            }
        } else if (type == 3) {
            locked = true;

            if (!this->IsValidOpenAction(lock->m_action[i])) {
                continue;
            }

            uint32_t count;
            const TSGrowableArray<const SpellEffectRec*>* effects = SpellGetEffects(lock->m_index[i], count);

            for (uint32_t effect = 0; effect < count; effect++) {
                if (effects->m_data[effect] && effects->m_data[effect]->m_effect == 33) {
                    if (spellId) {
                        *spellId = lock->m_index[i];
                    }

                    if (entry) {
                        *entry = i;
                    }

                    return false;
                }
            }
        } else if (type == 1) {
            locked = true;

            if (!this->IsValidOpenAction(lock->m_action[i])) {
                continue;
            }

            CGItem_C* item = player->m_bag.FindItemOfType(lock->m_index[i], 0);

            if (item) {
                if (spellId) {
                    *spellId = item->GetEnchantmentSpell(33);
                }

                if (key) {
                    *key = item;
                }

                if (entry) {
                    *entry = i;
                }

                return false;
            }
        }
    }

    return locked;
}

// OFFSET: 0x0059E830
int32_t CGGameObject_C::CanHighlight() const {
    if (!this->m_typeData->CanHighlight()) {
        return 0;
    }

    int32_t questId = static_cast<int32_t>(this->GetPropertyValue(CGameObjectDef::GetPropNum(this->m_gameObject->m_typeID, 20)));

    if (questId <= 0) {
        return 1;
    }

    CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();

    if (!player) {
        return 0;
    }

    for (uint32_t i = 0; i < 25; i++) {
        if (player->GetQuestLogEntry(i)->m_questId == static_cast<uint32_t>(questId)) {
            return 1;
        }
    }

    return 0;
}

// OFFSET: 0x0059E8F0
int32_t CGGameObject_C::FloatingTooltip() const {
    return this->GetPropertyValue(CGameObjectDef::GetPropNum(this->m_gameObject->m_typeID, 19)) != 0;
}

// OFFSET: 0x0059E940/0x14020DD50
void CGGameObject_C::UseObject() {
    GAME_ERROR_TYPE error = static_cast<GAME_ERROR_TYPE>(834);
    float distance = 0.0f;
    const char* errorText = NULL;
    GameObjectStats* stats = this->m_stats;

    if (!stats) {
        return;
    }

    if (stats->m_requiredLevel) {
        CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

        if (!player) {
            return;
        }

        if (player->m_unit->m_unitFieldLevel < this->m_stats->m_requiredLevel) {
            char buffer[512];
            SStrPrintf(buffer, sizeof(buffer), FrameScript_GetText("ITEM_MIN_LEVEL", -1, GENDER_NOT_APPLICABLE), this->m_stats->m_requiredLevel);
            CGGameUI::DisplayError(static_cast<UIErrors>(48), buffer);
            return;
        }
    }

    if (this->m_typeData->CanUseNow(&error, &distance, &errorText)) {
        this->m_typeData->Use();
        CDataStore msg;
        msg.Put(static_cast<uint32_t>(CMSG_GAME_OBJ_REPORT_USE));
        msg.Put(this->m_obj->m_guid.m_guid);
        msg.Finalize();
        ClientServices::Send2(&msg);
        return;
    }

    if (error == ERR_USE_TOO_FAR && this->m_gameObject->m_typeID != 17) {
        WOWGUID guid = this->m_obj->m_guid;

        if (ClntObjMgrGetActivePlayerObj()->CTMInteractGO(guid, distance)) {
            return;
        }
    }

    if (errorText) {
        CGGameUI::DisplayError(static_cast<UIErrors>(error), errorText);
    } else {
        CGGameUI::DisplayError(static_cast<UIErrors>(error));
    }
}

// PROVEN: 112b131a IDENTICAL/NONE
// OFFSET: 0x0059EF40
C3Vector CGGameObject_C::GetNamePosition() const {
    if (this->m_gameObject->m_typeID == 33) {
        return static_cast<CGGameObject_C_Type_DestructibleBuilding*>(this->m_typeData)->GetNamePosition();
    }

    C3Vector position = this->GetPosition();
    position.z = this->m_height * this->m_scale * 1.25f + position.z;
    return position;
}

// OFFSET: 0x0059F010/0x14020E5B0
int32_t CGGameObject_C_TypeBase::CanChangeCursor() const {
    bool canUse = this->CanUseNow(NULL, NULL, NULL);
    LockRec* lock = this->m_owner->GetLockRec();

    if (lock) {
        LockTypeRec* lockType = g_lockTypeDB.GetRecord(lock->m_index[0]);

        if (lockType && lockType->m_cursorName && *lockType->m_cursorName) {
            char cursor[32];

            if (lockType->m_id == 1 || canUse) {
                SStrCopy(cursor, lockType->m_cursorName, sizeof(cursor));
            } else {
                SStrPrintf(cursor, sizeof(cursor), "Unable%s", lockType->m_cursorName);
            }

            return CursorGetModeFromName(cursor);
        }
    }

    return canUse ? 5 : 31;
}

// OFFSET: 0x0059F100/0x14020E720
int32_t CGGameObject_C_TypeBase::CheckRange() const {
    CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();

    if (!player) {
        return 0;
    }

    float range = this->m_interactDistance;
    int32_t spellId = 0;

    if (this->m_owner->IsLocked(&spellId, NULL, NULL, NULL, NULL) || !spellId) {
        if (this->m_owner->m_displayState == 2) {
            return 0;
        }
    } else {
        float minRange;
        Spell_C::GetSpellRange(player, spellId, &minRange, &range, NULL);
    }

    CGGameObject_C* owner = this->m_owner;

    if (owner->m_gameObject->m_typeID == 8) {
        C3Vector position = owner->GetPosition();
        return range * range >= player->GetDistanceToPos(position);
    }

    CM2Model* model = owner->m_worldModel;
    CAaBox box;

    if (!owner->m_gameObject->m_typeID) {
        if (!model->IsLoaded()) {
            return 0;
        }

        M2BoneSequenceInfo info;
        model->GetBoneSequenceInfo(0xFFFFFFFF, info);
        M2SequenceInfo sequence;
        sequence.Clear();
        model->GetSequenceInfo(info.m_sequenceId, info.m_variationIndex, sequence);
        box = sequence.m_bounds;
    } else {
        GameObjectDisplayInfoRec* displayInfo = owner->m_stats ? g_gameObjectDisplayInfoDB.GetRecord(owner->m_stats->m_displayID) : NULL;

        if (displayInfo) {
            box.b = C3Vector(displayInfo->m_geoBoxMin[0], displayInfo->m_geoBoxMin[1], displayInfo->m_geoBoxMin[2]);
            box.t = C3Vector(displayInfo->m_geoBoxMax[0], displayInfo->m_geoBoxMax[1], displayInfo->m_geoBoxMax[2]);
        } else if (model->IsLoaded()) {
            box = model->GetBoundingBox();
        } else {
            return 0;
        }
    }

    float scale = this->m_owner->m_obj->m_scale;
    box *= scale;
    box.b.x = box.b.x - range;
    box.b.y = box.b.y - range;
    box.b.z = box.b.z - range;
    box.t.x = box.t.x + range;
    box.t.y = box.t.y + range;
    box.t.z = range + box.t.z;

    C44Matrix matrix = this->m_owner->GetMatrix();
    matrix.Scale(1.0f / scale);
    C44Matrix inverse = matrix.AffineInverse();
    C3Vector local = player->GetPosition() * inverse;
    return box.ContainsPointExclusive(local);
}

// PROVEN: cdb3e7bf IDENTICAL/NONE
// OFFSET: 0x0059F450/0x14020EBE0
void CGGameObject_C_TypeAnimated::OnUpdateAnimProgress() {
    if (CGGameObject_C::s_cvUseAnimKits && CGGameObject_C::s_cvUseAnimKits->m_intValue) {
        return;
    }

    if (this->m_owner->m_modelFlags & 0x20000) {
        return;
    }

    if (this->m_owner->IsObjectModelLoaded() && this->m_owner->m_worldModel) {
        this->SetSequence(this->m_owner->m_worldModel);
    }
}

// OFFSET: 0x005A0710
int32_t CGGameObject_C::GetPageTextID(void (*callback)(int32_t, const WOWGUID&, void*, bool)) const {
    return this->GetPropertyValue(CGameObjectDef::GetPropNum(this->m_gameObject->m_typeID, 15));
}

// OFFSET: 0x005A07D0
bool CGGameObject_C::IsUsableInCombat() const {
    return !this->GetPropertyValue(CGameObjectDef::GetPropNum(this->m_gameObject->m_typeID, 62));
}

// PROVEN: 59160ec3 IDENTICAL/NONE
// OFFSET: 0x005A0820
bool CGGameObject_C::IsUsableMounted() const {
    int8_t type = this->m_gameObject->m_typeID;

    if (type == 19) {
        return true;
    }

    if (type == 32) {
        return false;
    }

    return this->GetProperty(91) != 0;
}

// PROVEN: 496c437d IDENTICAL/NONE
// OFFSET: 0x005A0850
bool CGGameObject_C::IsUsableWhileImmune() const {
    int8_t type = this->m_gameObject->m_typeID;

    if (type == 3) {
        return false;
    }

    return !this->GetPropertyValue(CGameObjectDef::GetPropNum(type, 57));
}

// OFFSET: 0x005A1040
void CGGameObject_C_Type_MeetingStone::AddToolTipInfo(CGTooltip* tooltip) {
    CGGameObject_C* owner = this->m_owner;
    AreaTableRec* area = g_areaTableDB.GetRecord(owner->GetPropertyValue(CGameObjectDef::GetPropNum(owner->m_gameObject->m_typeID, 53)));

    if (area) {
        tooltip->AddLine(area->m_areaName, NULL, 0);
    }
}

// OFFSET: 0x005A1560
void CGGameObject_C::OnRightClick() {
    if (!this->m_typeData->CanUse()) {
        return;
    }

    CGGameUI::CloseLoot(true, false, false);
    CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();

    if (player) {
        player->SetAutoLoot(CGGameUI::IsAutoLooting());
    }

    this->UseObject();
}

// OFFSET: 0x0059B330
void CGGameObject_C::SetAnimLocked(int32_t locked) {
    if ((locked != 0) == (this->m_animLocked != 0)) {
        return;
    }

    if (locked) {
        this->m_animLocked = 1;
        ClntObjMgrLockObject(this->m_obj->m_guid);
    } else {
        this->m_animLocked = 0;
        ClntObjMgrUnlockObject(this->m_obj->m_guid, __FILE__, __LINE__);
    }
}

// OFFSET: 0x005A2C50
void CGGameObject_C::OnInteract() {
    this->OnRightClick();
}

// OFFSET: 0x005A15C0
void CGGameObject_C::RefreshInteractIcon() {
    CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();

    if (!player) {
        return;
    }

    if (this->ObjectReaction(player) >= 3 && this->IsQuestGiver()) {
        player->UpdateQuestStatus(this);
    } else {
        this->m_questStatus = 0;
        this->m_interactIconType = 0;
        this->RemoveInteractIcon();
    }
}

// OFFSET: 0x005A1880/0x1402118C0
bool CGGameObject_C_TypeBase::CanUseNow(GAME_ERROR_TYPE* error, float* distance, const char** errorText) const {
    CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();

    if (!player || player->IsDeadOrGhost()) {
        if (error) {
            *error = static_cast<GAME_ERROR_TYPE>(ERR_PLAYER_DEAD);
        }

        return false;
    }

    if (!CGGameUI::m_hasControl) {
        if (error) {
            *error = static_cast<GAME_ERROR_TYPE>(ERR_CLIENT_LOCKED_OUT);
        }

        return false;
    }

    if (player->m_unit->m_unitFieldFlags & 0x40000) {
        if (error) {
            int32_t mechanic;
            Spell_C_CancelsStun(player, NULL, &mechanic);

            if (mechanic && errorText) {
                *errorText = g_spellMechanicDB.GetRecord(mechanic)->m_stateName;
                *error = static_cast<GAME_ERROR_TYPE>(ERR_USE_PREVENTED_BY_MECHANIC_S);
            } else {
                *error = static_cast<GAME_ERROR_TYPE>(ERR_GENERIC_STUNNED);
            }
        }

        return false;
    }

    if (!this->m_owner->GetLockRec()) {
        if (player->IsMounted() && !this->m_owner->IsUsableMounted() && !player->CanAutoDismount()) {
            if (error) {
                *error = static_cast<GAME_ERROR_TYPE>(ERR_NOT_WHILE_MOUNTED);
            }

            return false;
        }

        if (player->IsShapeShifted()) {
            SpellShapeshiftFormRec* form = g_spellShapeshiftFormDB.GetRecord(player->GetShapeshiftForm());

            if ((!form || !(form->m_flags & 0x8)) && !player->CanAutoUnshift()) {
                if (error) {
                    *error = static_cast<GAME_ERROR_TYPE>(ERR_NOT_WHILE_SHAPESHIFTED);
                }

                return false;
            }
        }
    }

    if ((player->m_unit->m_unitFieldFlags & 0x80000) && !this->m_owner->IsUsableInCombat()) {
        if (error) {
            *error = static_cast<GAME_ERROR_TYPE>(ERR_NOT_IN_COMBAT);
        }

        return false;
    }

    if ((player->m_unit->m_unitFieldFlags & 0x80000000) && !this->m_owner->IsUsableWhileImmune()) {
        if (error) {
            *error = static_cast<GAME_ERROR_TYPE>(ERR_USE_CANT_IMMUNE);
        }

        return false;
    }

    CGGameObject_C* owner = this->m_owner;

    if ((owner->m_gameObject->m_flags & 0x2) && owner->IsLocked(NULL, NULL, NULL, NULL, NULL)) {
        if (error) {
            *error = static_cast<GAME_ERROR_TYPE>(this->m_useErrorType);
        }

        return false;
    }

    if (distance) {
        *distance = this->m_interactDistance;
    }

    bool result = this->CheckRange() != 0;

    if (!result && error) {
        *error = static_cast<GAME_ERROR_TYPE>(ERR_USE_TOO_FAR);
    }

    return result;
}

// OFFSET: 0x005A4250/0x140214C70
bool CGGameObject_C_TypeBase::Use() {
    int32_t spellId = 0;
    CGItem_C* key = NULL;
    uint32_t entry;

    if (this->m_owner->IsLocked(&spellId, NULL, NULL, &key, &entry)) {
        LockRec* lock = this->m_owner->GetLockRec();
        CGGameObject_C* owner = this->m_owner;

        if (!owner->IsValidOpenAction(lock->m_action[0])) {
            if (owner->m_gameObject->m_typeID == 3 && !owner->m_displayState) {
                CGGameUI::DisplayError(ERR_CHEST_IN_USE);
            }

            return false;
        }

        if (lock->m_type[0] != 1) {
            if (lock->m_type[0] == 2) {
                LockTypeRec* lockType = g_lockTypeDB.GetRecord(lock->m_index[0]);
                int32_t required = lock->m_skill[0] ? lock->m_skill[0] : 5 * owner->m_gameObject->m_level;

                if (spellId) {
                    CGGameUI::DisplayError(ERR_USE_LOCKED_WITH_SPELL_KNOWN_SI, lockType ? lockType->m_name : "UNKNOWN", required);
                } else {
                    CGGameUI::DisplayError(ERR_USE_LOCKED_WITH_SPELL_S, lockType ? lockType->m_name : "UNKNOWN");
                }
            } else if (lock->m_type[0] == 3) {
                const SpellRec* spell = g_spellDB.GetRecord(lock->m_index[0]);

                if (spell) {
                    CGGameUI::DisplayError(ERR_USE_LOCKED_WITH_SPELL_S, static_cast<const char*>(spell->m_name));
                }
            } else {
                CGGameUI::DisplayError(ERR_USE_CANT_OPEN);
            }

            return false;
        }

        WOWGUID guid = owner->m_obj->m_guid;
        ItemStats_C* stats = g_itemDBCache.GetRecord(lock->m_index[0], guid, NULL, NULL, false);

        if (stats) {
            CGGameUI::DisplayError(ERR_USE_LOCKED_WITH_ITEM_S, stats->GetName(0));
        }

        return false;
    }

    CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();

    if (spellId) {
        WOWGUID none = WOWGUID();

        if (!Spell_C::CastSpell(spellId, key, none, false)) {
            return false;
        }

        Spell_C_HandleSpriteClick(this->m_owner, NULL);
    } else {
        if (!player) {
            return false;
        }

        if (player->m_mountDisplayId > 0 && !(player->m_clientFlags & 0x10000000) && !this->m_owner->IsUsableMounted()) {
            player->Dismount();
        }

        if (player->IsShapeShifted()) {
            SpellShapeshiftFormRec* form = g_spellShapeshiftFormDB.GetRecord(player->GetShapeshiftForm());

            if (!form || !(form->m_flags & 0x8)) {
                player->CancelShapeshift();
            }
        }

        CDataStore msg;
        msg.Put(static_cast<uint32_t>(CMSG_GAME_OBJ_USE));
        msg.Put(this->m_owner->m_obj->m_guid.m_guid);
        msg.Finalize();
        ClientServices::Send2(&msg);
    }

    if (player && player->HasVehicleTransport()) {
        player->CheckVehicleExitOnCast();
    }

    return true;
}

// OFFSET: 0x005A26F0
bool CGGameObject_C_Type_Ritual::CanUseNow(GAME_ERROR_TYPE* error, float* distance, const char** errorText) const {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player) {
        const SpellRec* spell = g_spellDB.GetRecord(this->m_owner->GetProperty(10));

        if (!spell || !(spell->m_attributes & 0x10000000) || !player->IsInCombat() || player->HasIgnoreAuraState(spell)) {
            const WOWGUID& creator = this->m_owner->m_gameObject->m_createdBy;

            if ((creator.m_guid & 0xF000000000000000ULL) || !(creator.m_guid & 0xF07FFFFFFFFFFFFFULL)) {
                return CGGameObject_C_TypeBase::CanUseNow(error, distance, errorText);
            }

            CGUnit_C* caster = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(creator, TYPEMASK_PLAYER, __FILE__, __LINE__));

            if (caster && (player->IsInMyPartyOrRaid(caster) || player->IsInMyParty(caster))) {
                return CGGameObject_C_TypeBase::CanUseNow(error, distance, errorText);
            }
        }
    }

    if (error) {
        *error = static_cast<GAME_ERROR_TYPE>(176);
    }

    return false;
}

// OFFSET: 0x005A29F0
int32_t CGGameObject_C_Type_MeetingStone::CheckUseRange(uint32_t level) const {
    CGGameObject_C* owner = this->m_owner;

    if (level < owner->GetPropertyValue(CGameObjectDef::GetPropNum(owner->m_gameObject->m_typeID, 51))) {
        return false;
    }

    owner = this->m_owner;
    return level <= owner->GetPropertyValue(CGameObjectDef::GetPropNum(owner->m_gameObject->m_typeID, 52));
}

// OFFSET: 0x005A45E0
bool CGGameObject_C_Type_MeetingStone::Use() {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return false;
    }

    if (!CGPartyInfo::m_members[0] && !CGRaidInfo::m_numMembers) {
        CGGameUI::DisplayError(ERR_MEETING_STONE_NEED_PARTY);
        return false;
    }

    if (!CGGameUI::m_lockedTarget) {
        CGGameUI::DisplayError(ERR_GENERIC_NO_TARGET);
        return false;
    }

    if (!this->CheckUseRange(player->m_unit->m_unitFieldLevel)) {
        CGGameUI::DisplayError(ERR_MEETING_STONE_INVALID_LEVEL);
        return false;
    }

    if (!CGPartyInfo::IsMember(CGGameUI::m_lockedTarget) && !IsGuidInMyRaid(CGGameUI::m_lockedTarget)) {
        CGGameUI::DisplayError(ERR_MEETING_STONE_TARGET_NOT_IN_PARTY);
        return false;
    }

    uint32_t level;
    CGUnit_C* target = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(CGGameUI::m_lockedTarget, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (target) {
        level = target->m_unit->m_unitFieldLevel;
    } else {
        WOWGUID guid = CGGameUI::m_lockedTarget;
        CGPartyMemberState* stats = CGPartyInfo::GetRemoteStats(guid);

        if (!stats) {
            stats = CGRaidInfo::GetRemoteStats(guid);
        }

        if (!stats || !stats->m_level || !(stats->m_flags & 0x1)) {
            CGGameUI::DisplayError(ERR_MEETING_STONE_NOT_FOUND);
            return false;
        }

        level = stats->m_level;
    }

    if (this->CheckUseRange(level)) {
        return CGGameObject_C_TypeBase::Use();
    }

    CGGameUI::DisplayError(ERR_MEETING_STONE_TARGET_INVALID_LEVEL);
    return false;
}

// PROVEN: 7350f8f2 IDENTICAL/NONE
// OFFSET: 0x005A3150/0x140213710
void CGGameObject_C::UpdateInteractIcon() {
    this->CGObject_C::UpdateInteractIcon();
    this->CheckForPassiveHighlight();
}

// OFFSET: 0x005A3FF0
void CGGameObject_C::PostReenable(const CClientObjCreate& create) {
    this->m_passenger.PostInit(create.m_stationaryPosition);
    CGObject_C::PostReenable(create);

    if (this->m_stats) {
        this->m_typeData->PostReenable();
        this->CheckForPassiveHighlight();
        return;
    }

    WOWGUID guid = this->m_obj->m_guid;
    GameObjectStats_C* stats = g_gameObjectDBCache.GetRecord(this->m_obj->m_entryID, guid, &GameObjectStatsCallback, this, false);

    if (stats) {
        this->SetStats(stats);
    }
}

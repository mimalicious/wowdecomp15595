#include "WorldFrame.h"
#include "Engine/Source/Base/ConvertUTF.h"
#include <cstring>
#include <typeinfo>
#include "Engine/Source/Base/Coordinate.h"
#include "Engine/Source/Base/Status.h"
#include "Engine/Source/ConsoleBase/ConsoleVar.h"
#include "Engine/Source/Event/CKeyEvent.h"
#include "Engine/Source/Event/CMouseEvent.h"
#include "Engine/Source/Event/Event.h"
#include "Engine/Source/FFX/EffectGlow.h"
#include "Engine/Source/FFX/FFX.h"
#include "Engine/Source/Frame/CRenderBatch.h"
#include "Engine/Source/Frame/CSimpleTop.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameXML/FrameXML.h"
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "Engine/Source/Gx/CGxTexFlags.h"
#include "Engine/Source/Gx/Gx.h"
#include "Engine/Source/Gxu/Gxu.h"
#include "Engine/Source/Model2/M2Scene.h"
#include "Engine/Source/Os/Time.h"
#include "Engine/Source/Os/TimeManager.h"
#include "Engine/Source/Services/Camera.h"
#include "Engine/Source/Services/SysMessage.h"
#include "Engine/Source/Services/Texture.h"
#include "Engine/Source/ShaderEffect/ShaderEffect.h"
#include "Engine/Source/Tempest/C4Vector.h"
#include "Engine/Source/Tempest/CAaBox.h"
#include "Engine/Source/Tempest/CImVector.h"
#include "Engine/Source/lua-5.1/src/lua.h"
#include "WoW/Common/TextureCache.h"
#include "WoW/Source/Client.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/MapRec.h"
#include "WoW/Source/DB/Rec/ScreenEffectRec.h"
#include "WoW/Source/DB/Rec/SpellRec.h"
#include "WoW/Source/Game/GameClient/FFXEffects.h"
#include "WoW/Source/Game/GameClient/PassFogSeed.h"
#include "WoW/Source/Game/GameClient/PlayerName.h"
#include "WoW/Source/Game/GameClient/ProjectTex.h"
#include "WoW/Source/Game/GameClient/WorldText.h"
#include "WoW/Source/Game/GameTime.h"
#include "WoW/Source/Magic/MagicClient/SpellVisuals.h"
#include "WoW/Source/Magic/MagicClient/Spell_C.h"
#include "WoW/Source/Magic/MagicClient/SpellWorldObject.h"
#include "WoW/Source/Object/ObjectClient/AuraData.h"
#include "WoW/Source/Object/ObjectClient/Corpse_C.h"
#include "WoW/Source/Object/ObjectClient/DynamicObject_C.h"
#include "WoW/Source/Object/ObjectClient/Effect_C.h"
#include "WoW/Source/Object/ObjectClient/GameObject_C.h"
#include "WoW/Source/Object/ObjectClient/Missile_C.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/Object/ObjectClient/Unit_C.h"
#include "WoW/Source/Object/ObjectClient/VehiclePassenger_C.h"
#include "WoW/Source/Object/ObjectClient/Vehicle_C.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/SoundInterface/SI3.h"
#include "WoW/Source/UIUtil/Camera.h"
#include "WoW/Source/UIUtil/Cursor.h"
#include "WoW/Source/UIUtil/InputControl.h"
#include "WoW/Source/UIUtil/PortraitButton.h"
#include "WoW/Source/UIUtil/SmartScreenRect.h"
#include "WoW/Source/UIUtil/Tooltip.h"
#include "WoW/Source/Ui/BattlefieldInfo.h"
#include "WoW/Source/Ui/CGCooldown.h"
#include "WoW/Source/Ui/CGQuestPOIFrame.h"
#include "WoW/Source/Ui/CGWorldStateInfo.h"
#include "WoW/Source/Ui/CharacterModelBase.h"
#include "WoW/Source/Ui/ChatBubbleFrame.h"
#include "WoW/Source/Ui/CommentatorFrame.h"
#include "WoW/Source/Ui/DressUpModelFrame.h"
#include "WoW/Source/Ui/GameUI.h"
#include "WoW/Source/Ui/MailInfo.h"
#include "WoW/Source/Ui/MinimapFrame.h"
#include "WoW/Source/Ui/NamePlateFrame.h"
#include "WoW/Source/Ui/ScriptEvents.h"
#include "WoW/Source/Ui/TabardModelFrame.h"
#include "WoW/Source/Ui/UIBindings.h"
#include "WoW/Source/Ui/UIMacros.h"
#include "WoW/Source/WorldClient/Barrier.h"
#include "WoW/Source/DayNight/DayNight.h"
#include "Engine/Source/Event/FrameTime.h"
#include "WoW/Source/WorldClient/Map.h"
#include "WoW/Source/WorldClient/MapEntity.h"
#include "WoW/Source/WorldClient/MapWeather.h"
#include "WoW/Source/WorldClient/World.h"
#include <cfloat>
#include <cmath>
#include "WoW/Source/Ui/SpellBookFrame.h"
#include "WoW/Source/StaticSingleton.h"
#include "Engine/Source/Tempest/CMath.h"
#include "WoW/Source/Object/ObjectClient/Movement_C.h"
#include "WoW/Source/UIUtil/CameraManager.h"
#include "WoW/Source/Game/GameClient/EffectUnderwater.h"
#include "WoW/Source/LiquidSystem/Liquid.h"
#include "Engine/Source/SoundEngine/SE3.h"
#include "WoW/Source/WorldClient/AreaList.h"
#include "WoW/Source/DB/Rec/AreaTableRec.h"
#include "WoW/Source/DB/Rec/SpellEffectRec.h"
#include "WoW/Source/Ui/PetInfo.h"
#include "WoW/Source/Object/ObjectClient/AreaTrigger_C.h"
#include "WoW/Source/Object/ObjectClient/AnimKitManager.h"
#include "WoW/Common/ComponentCore/CharacterComponent.h"
#include "Engine/Source/Gxu/GxuScene.h"
#include "Engine/Source/Gx/ViewSettings.h"
#include "WoW/Source/Ui/BlobFrame.h"
#include "WoW/Source/Ui/ArchaeologyDigSiteFrame.h"

// STATIC: 0x00BEF4EC
static const uint32_t s_skinCursorModes[2] = { 21, 20 };

// STATIC: 0x00BEF4F4
static const uint32_t s_skinCursorModesUnable[2] = { 47, 46 };

// STATIC: 0x00D10130
static const char* s_spellShadowTextureNames[2] = { "Interface\\SpellShadow\\Spell-Shadow-Acceptable.blp", "Interface\\SpellShadow\\Spell-Shadow-Unacceptable.blp" };

// STATIC: 0x00D10138
static int32_t s_spellShadowStyle = 3;

// STATIC: 0x00ED79F0
static CTexture* s_spellShadowTexture[2];

// STATIC: 0x00ED7A24
static float s_spellShadowSize;

// STATIC: 0x00ED7A30
static C3Vector s_spellShadowPos;

// STATIC: 0x00ED79F8
FFX::Effect* CGWorldFrame::ffxEffectUnconscious;

// STATIC: 0x00ED79FC
FFX::Effect* CGWorldFrame::ffxEffectUnderwater;

// STATIC: 0x00ED7A00
FFX::Effect* CGWorldFrame::ffxEffectSwirlingFog;

// STATIC: 0x00ED7A04
FFX::Effect* CGWorldFrame::ffxEffectNetherWorld;

// STATIC: 0x00ED7A08
FFX::Effect* CGWorldFrame::ffxEffectGlow;

// STATIC: 0x00ED7A0C
FFX::Effect* CGWorldFrame::ffxEffectDeath;

// STATIC: 0x00ED7A10
CGWorldFrame* CGWorldFrame::s_currentWorldFrame;

// STATIC: 0x00ED7A14
static float s_timeOfDayBlendRemaining;

// STATIC: 0x00ED7A18
static float s_timeOfDayProgression;

// STATIC: 0x00ED7A1C
static int32_t s_timeOfDayOverrideActive;

// STATIC: 0x00ED7A20
static float s_timeOfDayOverride;

// STATIC: 0x00D1016C
static float s_timeOfDayBlendTime;

// STATIC: 0x00ED7A58
static char s_keyNameBuffer[8];

// STATIC: 0x00D101B0
static float s_maxTimeScale = 1.0f;

// STATIC: 0x00D101B4
static float s_minTimeScale = 0.0f;

// STATIC: 0x00ED7A60
static C3Vector s_listenerUp;

// STATIC: 0x00D10188
CDataAllocator CGWorldFrame::s_worldFrameHeap(sizeof(CGWorldFrame), 1);

// OFFSET: 0x008DEAF0
int32_t CGWorldFrame::CanTargetCorpseSelection(const CGCorpse_C* corpse, uint32_t flags) {
    if (Spell_C_IsTargeting()) {
        return Spell_C_CanTargetCorpse(corpse);
    }

    return 1;
}

// OFFSET: 0x008DEB20/0x14060B8A0
int32_t CGWorldFrame::SpellEffectEnumProc(void* param, uintptr_t flags, uint64_t guid, uintptr_t a4) {
    if (a4) {
        reinterpret_cast<CEffect*>(a4)->UpdateWorldObject(CGWorldFrame::s_currentWorldFrame->m_elapsedSec, static_cast<uint32_t>(flags));
    }

    return 1;
}

// OFFSET: 0x008DEB50/0x14060B8D0
CGCamera* CGWorldFrame::GetActiveCamera() {
    if (CGWorldFrame::s_currentWorldFrame) {
        return CGWorldFrame::s_currentWorldFrame->m_camera;
    }

    return NULL;
}

// OFFSET: 0x008DEB70
int32_t CGWorldFrame::HandleSpriteTrack(WOWGUID guid) {
    WOWGUID tracked = this->m_trackedGuid;

    if (guid == tracked) {
        return 0;
    }

    CObjectTrackEvent evt;
    evt.m_guid = guid;
    evt.m_prevGuid = tracked;
    CGGameUI::HandleSpriteTrack(evt);
    this->m_trackedGuid = guid;
    return 1;
}

// PROVEN: 190ea79a IDENTICAL/NONE
// OFFSET: 0x008DEBE0
int32_t CGWorldFrame::OnLayerTrackUpdate(const CMouseEvent& evt) {
    int32_t result = CSimpleFrame::OnLayerTrackUpdate(evt);

    if (!result) {
        return result;
    }

    CGInputControl::GetActive();

    return 1;
}

// OFFSET: 0x008DEC10
static C3Vector GetFacingVector(CGObject_C* object) {
    float facing = object->GetFacing();
    return C3Vector(cosf(facing), sinf(facing), 0.0f);
}

// OFFSET: 0x008DEC60
void CGWorldFrame::OnLayerCursorExit(int32_t a2, int32_t a3) {
    this->CSimpleFrame::OnLayerCursorExit(a2, a3);
    WOWGUID tracked = this->m_trackedGuid;

    if (tracked) {
        CObjectTrackEvent evt;
        evt.m_guid = WOWGUID();
        evt.m_prevGuid = tracked;
        CGGameUI::HandleSpriteTrack(evt);
        this->m_trackedGuid = WOWGUID();
    }

    CursorResetMode();
    s_spellShadowStyle = 3;
}

// OFFSET: 0x008DECD0
int32_t CGWorldFrame::OnLayerMouseWheel(const CMouseEvent& evt) {
    if (this->CSimpleFrame::OnLayerMouseWheel(evt) || !evt.m_data.m_wheelAmount) {
        return 1;
    }

    char binding[32];

    if (!CGUIBindings::MouseEventToString(evt, binding, sizeof(binding))) {
        return 0;
    }

    int32_t down = CGUIBindings::s_bindings->ExecKey(evt.m_data.m_metaKeyState, binding, 1, 1, static_cast<BINDING_MODE>(4));
    return CGUIBindings::s_bindings->ExecKey(evt.m_data.m_metaKeyState, binding, 0, 1, static_cast<BINDING_MODE>(4)) + down;
}

// PROVEN: 95e3b746 IDENTICAL/NONE
// OFFSET: 0x008DED60/0x14060BA90
void CGWorldFrame::OnMouseModeNormal() {
    this->m_worldFlags &= ~0x2;
}

// OFFSET: 0x008DED70
void CGWorldFrame::OnMouseModeRelative() {
    WOWGUID tracked = this->m_trackedGuid;
    this->m_worldFlags |= 0x2;

    if (tracked) {
        CObjectTrackEvent evt;
        evt.m_guid = WOWGUID();
        evt.m_prevGuid = tracked;
        CGGameUI::HandleSpriteTrack(evt);
        this->m_trackedGuid = WOWGUID();
    }
}

// OFFSET: 0x008DEDC0
void CGWorldFrame::SetNamePlateUpdate() {
    this->m_namePlateFlags |= 0x1;
}

// OFFSET: 0x008DEDE0
static const char* KeyCodeToString(KEY key) {
    if (static_cast<uint32_t>(key - 33) <= 0xDE) {
        sputu8(key, s_keyNameBuffer);
        return s_keyNameBuffer;
    }

    switch (key) {
        case KEY_NONE:
            return "NONE";
        case KEY_LSHIFT:
            return "LSHIFT";
        case KEY_RSHIFT:
            return "RSHIFT";
        case KEY_LCONTROL:
            return "LCTRL";
        case KEY_RCONTROL:
            return "RCTRL";
        case KEY_LALT:
            return "LALT";
        case KEY_RALT:
            return "RALT";
        case KEY_SPACE:
            return "SPACE";
        case KEY_NUMPAD0:
        case KEY_NUMPAD1:
        case KEY_NUMPAD2:
        case KEY_NUMPAD3:
        case KEY_NUMPAD4:
        case KEY_NUMPAD5:
        case KEY_NUMPAD6:
        case KEY_NUMPAD7:
        case KEY_NUMPAD8:
        case KEY_NUMPAD9:
            SStrPrintf(s_keyNameBuffer, sizeof(s_keyNameBuffer), "NUMPAD%d", key - KEY_NUMPAD0);
            return s_keyNameBuffer;
        case KEY_NUMPAD_PLUS:
            return "NUMPADPLUS";
        case KEY_NUMPAD_MINUS:
            return "NUMPADMINUS";
        case KEY_NUMPAD_MULTIPLY:
            return "NUMPADMULTIPLY";
        case KEY_NUMPAD_DIVIDE:
            return "NUMPADDIVIDE";
        case KEY_NUMPAD_DECIMAL:
            return "NUMPADDECIMAL";
        case KEY_NUMPAD_EQUALS:
            return "NUMPADEQUALS";
        case KEY_ESCAPE:
            return "ESCAPE";
        case KEY_ENTER:
            return "ENTER";
        case KEY_BACKSPACE:
            return "BACKSPACE";
        case KEY_TAB:
            return "TAB";
        case KEY_LEFT:
            return "LEFT";
        case KEY_UP:
            return "UP";
        case KEY_RIGHT:
            return "RIGHT";
        case KEY_DOWN:
            return "DOWN";
        case KEY_INSERT:
            return "INSERT";
        case KEY_DELETE:
            return "DELETE";
        case KEY_HOME:
            return "HOME";
        case KEY_END:
            return "END";
        case KEY_PAGEUP:
            return "PAGEUP";
        case KEY_PAGEDOWN:
            return "PAGEDOWN";
        case KEY_CAPSLOCK:
            return "CAPSLOCK";
        case KEY_NUMLOCK:
            return "NUMLOCK";
        case KEY_PRINTSCREEN:
            return "PRINTSCREEN";
        default:
            if (key >= KEY_F1 && key <= KEY_F19) {
                SStrPrintf(s_keyNameBuffer, sizeof(s_keyNameBuffer), "F%d", key - KEY_F1 + 1);
                return s_keyNameBuffer;
            }

            return "UNKNOWN";
    }
}

// OFFSET: 0x008DF200
static int32_t GetObjectHitTestType(CGObject_C* object) {
    if (!object) {
        return 0;
    }

    switch (object->m_obj->m_type) {
        case TYPEMASK_OBJECT | TYPEMASK_UNIT:
        case TYPEMASK_OBJECT | TYPEMASK_UNIT | TYPEMASK_PLAYER: {
            CGUnit_C* unit = static_cast<CGUnit_C*>(object);

            if (unit->m_unit->m_unitFieldHealth > 0) {
                return 3;
            }

            return unit->CanBeLooted(FrameTime::GetCurTimeMs()) ? 2 : 0;
        }

        case TYPEMASK_OBJECT | TYPEMASK_GAMEOBJECT:
            return static_cast<CGGameObject_C*>(object)->CanUse() ? 1 : 0;

        case TYPEMASK_OBJECT | TYPEMASK_CORPSE:
            return static_cast<CGCorpse_C*>(object)->CanBeLooted() ? 2 : 0;

        default:
            return 0;
    }
}

// OFFSET: 0x008DF300
static void AddModelToHitTest(CM2Model* model, uint32_t group, void* owner, uint32_t mode) {
    if (model->m_fFlags & 0x1) {
        if (!model->m_hitTestPrev) {
            CM2Model** head = &model->m_scene->m_hitTestList;
            model->m_hitTestPrev = head;
            model->m_hitTestNext = *head;
            *head = model;

            if (model->m_hitTestNext) {
                model->m_hitTestNext->m_hitTestPrev = &model->m_hitTestNext;
            }
        }

        model->m_hitTestMode = mode;
        model->m_hitTestGroup = group;
        model->m_hitTestOwner = owner;
    }

    for (CM2Model* attached = model->m_attachList; attached; attached = attached->m_attachNext) {
        if (!attached->m_hitTestPrev) {
            AddModelToHitTest(attached, group, reinterpret_cast<void*>(-1), mode);
        }
    }
}

// OFFSET: 0x008DF390
void CGWorldFrame::AddObjectToHitTestList(CModelRecord* record, uint32_t flags) {
    uint32_t group;

    if (record->m_guid == this->m_hitGuid) {
        group = -1;
    } else {
        group = GetObjectHitTestType(record->m_object);
    }

    record->m_object = NULL;
    AddModelToHitTest(record->m_model, group, record, (flags >> 25) & 0x4);
}

// OFFSET: 0x008DF3E0
int32_t CGWorldFrame::GetLineSegment(float mouseX, float mouseY, C3Vector* start, C3Vector* end) {
    float x = (mouseX - this->m_viewportDDC.minX) / (this->m_viewportDDC.maxX - this->m_viewportDDC.minX);
    float y = (mouseY - this->m_viewportDDC.minY) / (this->m_viewportDDC.maxY - this->m_viewportDDC.minY);

    if (x < 0.0f || y < 0.0f || x > 1.0f || y > 1.0f) {
        return 0;
    }

    CameraGetLineSegment(x, y, start, end);

    const C3Vector& position = StaticSingleton<CCameraManager>::Instance()->m_shot.m_position;
    *start += position;
    *end += position;

    float dx = end->x - start->x;
    float dy = end->y - start->y;
    float dz = end->z - start->z;
    return fabs(dz * dz + dx * dx + dy * dy) >= 0.0000010000001f;
}

// OFFSET: 0x008DF550/0x14060C180
static int32_t ObjectRenderCallback(WOWGUID* guid, uint32_t param, World::ObjectCollisionData* data, uint32_t flags) {
    uint32_t high = static_cast<uint32_t>(guid->m_guid >> 32);

    if ((high & 0xF0F00000) != 0xF0100000 && ((high & 0xF0000000) != 0x10000000 || (high & 0xFF00000) != 0xFC00000)) {
        return 0;
    }

    CGObject_C* object = ClntObjMgrObjectPtr(*guid, TYPEMASK_GAMEOBJECT, __FILE__, __LINE__);

    if (!object || !static_cast<CGGameObject_C*>(object)->IsSolidCollidable(flags)) {
        return 0;
    }

    data->m_model = object->GetObjectModel();
    data->m_bounds = static_cast<CGGameObject_C*>(object)->GetWorldBounds();
    data->m_scale = object->GetTrueScale();
    data->m_matrix = object->GetMatrix();

    uint64_t transport = object->GetTransportGUID();

    if (transport) {
        guid->m_guid = transport;
    }

    return 1;
}

// OFFSET: 0x008DF670/0x14060C350
void CGWorldFrame::GetCameraPosition(C3Vector* position) {
    *position = StaticSingleton<CCameraManager>::Instance()->m_shot.m_position;
}

// OFFSET: 0x008DF6C0/0x14060C3C0
void CGWorldFrame::GetCameraFacing(C3Vector* facing) {
    const C44Matrix& matrix = StaticSingleton<CCameraManager>::Instance()->m_renderMatrix;
    facing->x = matrix.a0;
    facing->y = matrix.a1;
    facing->z = matrix.a2;
}

// OFFSET: 0x008DF710/0x14060C430
static int32_t DestroyLiquidPerturbatorProc(WOWGUID guid, void* param) {
    CGObject_C* object = ClntObjMgrObjectPtr(guid, TYPEMASK_OBJECT, __FILE__, __LINE__);

    if (object && (object->m_obj->m_type & TYPEMASK_UNIT)) {
        static_cast<CGUnit_C*>(object)->DestroyLiquidPerturbator();
    }

    return 1;
}

// OFFSET: 0x008DF750/0x14060C480
void CGWorldFrame::WaterReset() {
    ClntObjMgrEnumVisibleObjects(DestroyLiquidPerturbatorProc, NULL);
    FFX::SetEffect(0, NULL);

    if (CGWorldFrame::ffxEffectUnderwater) {
        delete CGWorldFrame::ffxEffectUnderwater;
    }

    CGWorldFrame::ffxEffectUnderwater = NULL;

    if (Liquid::GetWaterDetail()) {
        CGWorldFrame::ffxEffectUnderwater = new (__FILE__, __LINE__) EffectUnderwater(Liquid::GetWaterDetail());
    }
}

// OFFSET: 0x008DF7D0/0x14060C520
uint32_t CGWorldFrame::GetSceneTime() const {
    return World::GetScene() ? World::GetScene()->m_time : 0;
}

// OFFSET: 0x008DF7F0
void CGWorldFrame::OnLayerTrackTerrain(const HITTESTRESULT& result) {
    if (Spell_C_IsTargeting() && Spell_C_CanTargetTerrain()) {
        s_spellShadowPos = result.m_point;
        CTerrainClickEvent evt;
        evt.m_point = s_spellShadowPos;
        evt.m_guid = WOWGUID();

        if ((result.m_guid.m_guid & 0xF000000000000000ull) == 0x1000000000000000ull && (result.m_guid.m_guid & 0x0FF0000000000000ull) == 0x0FC0000000000000ull) {
            CGObject_C* transport = ClntObjMgrObjectPtr(result.m_guid, TYPEMASK_OBJECT, __FILE__, __LINE__);

            if (transport) {
                evt.m_guid = result.m_guid;
                s_spellShadowPos *= transport->GetMatrix();
            }
        }

        Spell_C_SetCursorWorldObjectPosition(s_spellShadowPos);

        switch (Spell_C_HandleTerrainRay(evt, true)) {
            case 0:
                s_spellShadowStyle = 0;
                s_spellShadowSize = Spell_C_GetSpellRadius() >= 20.0f ? 20.0f : Spell_C_GetSpellRadius();
                CursorSetMode(2);
                break;

            case 1:
                s_spellShadowStyle = 1;
                s_spellShadowSize = 0.0f;
                CursorSetMode(28);
                break;

            case 2:
                s_spellShadowStyle = 3;
                s_spellShadowSize = 0.0f;
                CursorSetMode(28);
                break;
        }

        SpellWorldObject* cursor = Spell_C_GetCursorWorldObject();
        uintptr_t entity = cursor->m_entity;

        if (!entity) {
            return;
        }

        CM2Model* model = cursor->m_model;
        CAaBox box;
        box.b = C3Vector(0.0f, 0.0f, 0.0f);
        box.t = C3Vector(0.0f, 0.0f, 0.0f);
        CAaSphere sphere;
        sphere.c = C3Vector(0.0f, 0.0f, 0.0f);
        sphere.r = 0.0f;
        C3Vector center(0.0f, 0.0f, 0.0f);

        if (model->IsLoaded()) {
            box = model->GetBoundingBox();
            sphere = model->GetBoundingSphere();
            center = sphere.c;
        }

        C44Matrix matrix;
        const C3Vector& position = cursor->m_position;
        matrix.d0 = position.x;
        matrix.d1 = position.y;
        matrix.d2 = position.z;
        matrix.RotateAroundZ(cursor->m_facing);
        World::ObjectUpdate(entity, matrix, box, sphere, center, 0, 0xFFFFFFFF);
        World::ObjectSetHidden(entity, s_spellShadowStyle != 0);
        return;
    }

    if (CGPetInfo::m_petOrderMoveTo) {
        s_spellShadowPos = result.m_point;
        CTerrainClickEvent evt;
        evt.m_point = s_spellShadowPos;
        evt.m_guid = WOWGUID();

        if ((result.m_guid.m_guid & 0xF000000000000000ull) == 0x1000000000000000ull && (result.m_guid.m_guid & 0x0FF0000000000000ull) == 0x0FC0000000000000ull) {
            CGObject_C* transport = ClntObjMgrObjectPtr(result.m_guid, TYPEMASK_OBJECT, __FILE__, __LINE__);

            if (transport) {
                evt.m_guid = result.m_guid;
                s_spellShadowPos *= transport->GetMatrix();
            }
        }

        switch (CGPetInfo::HandleMoveToTerrainRay(evt)) {
            case 0:
                s_spellShadowStyle = 0;
                s_spellShadowSize = 1.0f;
                break;

            case 1:
                s_spellShadowStyle = 1;
                s_spellShadowSize = 0.0f;
                break;

            case 2:
                s_spellShadowStyle = 3;
                s_spellShadowSize = 0.0f;
                break;
        }

        return;
    }

    CursorResetMode();
    this->HandleSpriteTrack(WOWGUID());
}

// OFFSET: 0x008DFC20/0x14060CB90
static int32_t UnitUpdateProc(WOWGUID guid, void* param) {
    CGObject_C* object = ClntObjMgrObjectPtr(guid, TYPEMASK_OBJECT, __FILE__, __LINE__);
    CGWorldFrame* frame = CGWorldFrame::s_currentWorldFrame;

    if (!object) {
        return 1;
    }

    uintptr_t entity = object->m_worldObject;

    switch (object->m_obj->m_type) {
        case TYPEMASK_OBJECT | TYPEMASK_UNIT:
        case TYPEMASK_OBJECT | TYPEMASK_UNIT | TYPEMASK_PLAYER:
            static_cast<CGUnit_C*>(object)->UpdateDisplay(World::GetScene() ? World::GetScene()->m_time : 0);
            break;

        case TYPEMASK_OBJECT | TYPEMASK_GAMEOBJECT:
            static_cast<CGGameObject_C*>(object)->UpdateDisplay(frame->GetSceneTime());
            break;

        case TYPEMASK_OBJECT | TYPEMASK_DYNAMICOBJECT:
            static_cast<CGDynamicObject_C*>(object)->UpdateDisplay(frame->GetSceneTime());
            break;

        case TYPEMASK_OBJECT | TYPEMASK_AREATRIGGER:
            static_cast<CGAreaTrigger_C*>(object)->UpdateDisplay(frame->GetSceneTime());
            break;

        default:
            break;
    }

    if (!entity) {
        return 1;
    }

    int32_t render = 0;
    int32_t a3 = 0;
    object->ShouldRender(1, render, a3);
    int32_t visible = !render && !a3;

    if (object->IsReadyToDraw()) {
        object->PreAnimate(frame);
        object->Animate(frame->m_elapsedSec);
    } else {
        visible = 0;
    }

    World::ObjectSetHidden(entity, !visible);
    return 1;
}

// OFFSET: 0x008DFE70/0x14060CD50
static int32_t UnitPostAnimateProc(WOWGUID guid, void* param) {
    CGObject_C* object = ClntObjMgrObjectPtr(guid, TYPEMASK_OBJECT, __FILE__, __LINE__);

    if (object && (object->m_obj->m_type & TYPEMASK_UNIT)) {
        static_cast<CGUnit_C*>(object)->PostAnimate(static_cast<CGWorldFrame*>(param));
    }

    return 1;
}

// OFFSET: 0x008DFEC0
void CGWorldFrame::RenderPostAnimateCallback(uint32_t pass, uint64_t param64) {
    CGUnit_C::UpdateMissileTrajectoryTargeting();
    SpellVisualsTick();

    if (this->m_namePlateFlags & 0x2) {
        CGUnit_C::NamePlateSetResortAll(true);
        this->m_namePlateFlags &= ~0x2;
    }

    ClntObjMgrEnumVisibleObjects(UnitPostAnimateProc, this);
    CGUnit_C::NamePlateSetResortAll(false);
    CGUnit_C::NamePlateUpdateLevels(false);
    CGUnit_C::NamePlateUpdateColor(false);

    if (this->m_namePlateFlags & 0x1) {
        SmartScreenRectClearGrid(0);

        CGUnit_C::UnitNameplatesReposition(this);
        this->m_namePlateFlags &= ~0x1;
    }
}

// OFFSET: 0x008DFF40
void CGWorldFrame::OnFrameSizeChanged(const CRect& rect) {
    this->CSimpleFrame::OnFrameSizeChanged(rect);

    this->m_viewportDDC = this->m_rect;
    this->m_viewportDDC.minX = this->m_viewportDDC.minX <= 0.0f ? 0.0f : this->m_viewportDDC.minX;
    this->m_viewportDDC.minY = this->m_viewportDDC.minY <= 0.0f ? 0.0f : this->m_viewportDDC.minY;
    float maxX = this->m_viewportDDC.maxX;
    this->m_viewportDDC.maxX = maxX >= NDCToDDCWidth(1.0f) ? NDCToDDCWidth(1.0f) : maxX;
    float maxY = this->m_viewportDDC.maxY;
    this->m_viewportDDC.maxY = maxY >= NDCToDDCHeight(1.0f) ? NDCToDDCHeight(1.0f) : maxY;

    if (this->m_camera) {
        StaticSingleton<CCameraManager>::Instance()->SetScreenAspect(this->m_viewportDDC);
    }

    DDCToNDC(this->m_rect.minX, this->m_rect.minY, &this->m_viewportNDC.minX, &this->m_viewportNDC.minY);
    DDCToNDC(this->m_rect.maxX, this->m_rect.maxY, &this->m_viewportNDC.maxX, &this->m_viewportNDC.maxY);
    this->m_viewportNDC.minX = this->m_viewportNDC.minX <= 0.0f ? 0.0f : this->m_viewportNDC.minX;
    this->m_viewportNDC.minY = this->m_viewportNDC.minY <= 0.0f ? 0.0f : this->m_viewportNDC.minY;
    this->m_viewportNDC.maxX = this->m_viewportNDC.maxX >= 1.0f ? 1.0f : this->m_viewportNDC.maxX;
    this->m_viewportNDC.maxY = this->m_viewportNDC.maxY >= 1.0f ? 1.0f : this->m_viewportNDC.maxY;
}

// OFFSET: 0x008E0120
int32_t CGWorldFrame::OnLayerKeyDown(const CKeyEvent& evt) {
    if (this->CSimpleFrame::OnLayerKeyDown(evt)) {
        return 1;
    }

    KEY key = evt.m_data.m_key;

    if (key >= KEY_LAST) {
        return 0;
    }

    if (key <= KEY_RALT) {
        FrameScript_SignalEvent(SCRIPT_EVENT_MODIFIER_STATE_CHANGED, "%s%d", KeyCodeToString(key), 1);
        return 1;
    }

    KEYDOWNSTATE& state = this->m_keyDown[key];

    if (!CGUIBindings::KeyEventToString(evt, state.m_keyString, sizeof(state.m_keyString))) {
        return 0;
    }

    state.m_modifiers = evt.m_data.m_metaKeyState;
    return CGUIBindings::s_bindings->ExecKey(state.m_modifiers, state.m_keyString, 1, 1, static_cast<BINDING_MODE>(4));
}

// OFFSET: 0x008E01B0
int32_t CGWorldFrame::OnLayerKeyUp(const CKeyEvent& evt) {
    if (this->CSimpleFrame::OnLayerKeyUp(evt)) {
        return 1;
    }

    KEY key = evt.m_data.m_key;

    if (key >= KEY_LAST) {
        return 0;
    }

    if (key <= KEY_RALT) {
        FrameScript_SignalEvent(SCRIPT_EVENT_MODIFIER_STATE_CHANGED, "%s%d", KeyCodeToString(key), 0);
        return 1;
    }

    KEYDOWNSTATE& state = this->m_keyDown[key];
    int32_t result = 0;

    if (!state.m_keyString[0]) {
        state.m_modifiers = 0;
        CGUIBindings::KeyEventToString(evt, state.m_keyString, sizeof(state.m_keyString));
    }

    if (state.m_keyString[0]) {
        state.m_modifiers |= evt.m_data.m_metaKeyState;
        result = CGUIBindings::s_bindings->ExecKey(state.m_modifiers, state.m_keyString, 0, 1, static_cast<BINDING_MODE>(4));
        state.m_keyString[0] = 0;
    }

    return result;
}

// OFFSET: 0x008E0250
int32_t CGWorldFrame::OnLayerMouseDown(const CMouseEvent& evt, const char* btn) {
    if (this->CSimpleFrame::OnLayerMouseDown(evt, btn) || CGGameUI::HandleMouseDown(evt)) {
        return 1;
    }

    KEYDOWNSTATE& state = this->m_mouseDown[MouseButtonToIndex(evt.m_data.m_button)];

    if (!CGUIBindings::MouseEventToString(evt, state.m_keyString, sizeof(state.m_keyString))) {
        return 0;
    }

    state.m_modifiers = evt.m_data.m_metaKeyState;
    return CGUIBindings::s_bindings->ExecKey(state.m_modifiers, state.m_keyString, 1, 1, static_cast<BINDING_MODE>(4));
}

// OFFSET: 0x008E02D0
int32_t CGWorldFrame::OnLayerMouseUp(const CMouseEvent& evt, const char* btn) {
    if (this->CSimpleFrame::OnLayerMouseUp(evt, btn) || CGGameUI::HandleMouseUp(evt)) {
        return 1;
    }

    KEYDOWNSTATE& state = this->m_mouseDown[MouseButtonToIndex(evt.m_data.m_button)];
    int32_t result = 0;

    if (!state.m_keyString[0]) {
        state.m_modifiers = 0;
        CGUIBindings::MouseEventToString(evt, state.m_keyString, sizeof(state.m_keyString));
    }

    if (state.m_keyString[0]) {
        state.m_modifiers |= evt.m_data.m_metaKeyState;
        result = CGUIBindings::s_bindings->ExecKey(state.m_modifiers, state.m_keyString, 0, 1, static_cast<BINDING_MODE>(4));
        state.m_keyString[0] = 0;
    }

    return result;
}

// OFFSET: 0x008E0360/0x14060D360
bool CGWorldFrame::GetScreenCoordinates(const C3Vector& world, C3Vector* screen, uint32_t* result, bool* behind) {
    const C3Vector& camera = StaticSingleton<CCameraManager>::Instance()->m_shot.m_position;
    float x = world.x - camera.x;
    float y = world.y - camera.y;
    float z = world.z - camera.z;
    const C44Matrix& m = this->m_viewProjection;
    float px = m.a0 * x + m.b0 * y + m.c0 * z + m.d0 * 0.0f;
    float pz = m.a2 * x + m.b2 * y + m.c2 * z + m.d2 * 0.0f;
    float pw = m.a3 * x + m.b3 * y + m.c3 * z + m.d3 * 0.0f;
    float py = m.b1 * y + m.c1 * z + m.a1 * x + m.d1 * 0.0f;

    if (World::GetNearClip() > pz || pw == 0.0f) {
        if (behind) {
            *behind = true;
        }

        return false;
    }

    if (behind) {
        *behind = false;
    }

    screen->z = pz;

    float invW = 1.0f / pw;
    float ddcX;
    float ddcY;
    NDCToDDC((this->m_viewportNDC.maxX - this->m_viewportNDC.minX) * ((invW * px + 1.0f) * 0.5f), (this->m_viewportNDC.maxY - this->m_viewportNDC.minY) * ((py * invW + 1.0f) * 0.5f), &ddcX, &ddcY);

    if (this->m_rect.minX < 0.0f) {
        ddcX -= this->m_rect.minX;
    }

    if (this->m_rect.minY < 0.0f) {
        ddcY -= this->m_rect.minY;
    }

    screen->x = ddcX;
    screen->y = ddcY;

    if (result) {
        uint32_t flags = (ddcX >= 0.0f ? 0x1 : 0) | (ddcY >= 0.0f ? 0x2 : 0) | (this->m_rect.maxX - this->m_rect.minX >= ddcX ? 0x4 : 0) | (this->m_rect.maxY - this->m_rect.minY >= ddcY ? 0x8 : 0);
        *result = flags;
        return flags == 0xF;
    }

    return ddcX >= 0.0f && this->m_rect.maxX - this->m_rect.minX >= ddcX && ddcY >= 0.0f && this->m_rect.maxY - this->m_rect.minY >= ddcY;
}

// OFFSET: 0x008E06B0/0x14060D6F0
void CGWorldFrame::SetCameraTarget(CGObject_C* target, int32_t freeLook) {
    this->m_camera->SetTarget(target, freeLook);

    if (target && (target->m_obj->m_type & TYPEMASK_UNIT)) {
        static_cast<CGUnit_C*>(target)->OnVehicleCameraPossiblyNeeded();
    }
}

// OFFSET: 0x008E06F0
void CGWorldFrame::ClearTrackedObjects() {
    if (this->m_trackedEffectGuidA) {
        CGObject_C* object = ClntObjMgrObjectPtr(this->m_trackedEffectGuidA, TYPEMASK_OBJECT, __FILE__, __LINE__);

        if (object) {
            object->RenderTargetSelection();
        }

        this->m_trackedEffectGuidA = WOWGUID();
    }

    if (this->m_trackedEffectGuidB) {
        CGObject_C* object = ClntObjMgrObjectPtr(this->m_trackedEffectGuidB, TYPEMASK_OBJECT, __FILE__, __LINE__);

        if (object) {
            object->RenderPetTargetSelection();
        }

        this->m_trackedEffectGuidB = WOWGUID();
    }
}

// OFFSET: 0x008E0780
static void SetWorldCameraTarget(uintptr_t entity, WOWGUID transport) {
    C44Matrix matrix;

    if (transport) {
        MovementGetTransportMtxX(transport, &matrix, WOWGUID(), __FILE__, __LINE__);
    }

    World::SetCameraTarget(entity, transport.m_guid, matrix);
}

// OFFSET: 0x008E0860
int32_t CGWorldFrame::IsUnitLegalSelection(CGUnit_C* unit, uint32_t flags) {
    if (!(flags & 0x20)) {
        if (unit->IsActivePlayer()) {
            return 0;
        }

        WOWGUID activePlayer = ClntObjMgrGetActivePlayer();

        if ((unit->m_unit->m_unitFieldFlags & 0x1000000) && unit->m_unit->m_unitFieldCharmedby == activePlayer) {
            return 0;
        }
    }

    if (flags & 0xF0000) {
        CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

        if ((flags & 0x40000) && !player->CanAssist(unit, false)) {
            return (flags & 0x800000) && unit->m_unit->m_unitFieldCharmedby && player->CanAttack(unit, false);
        }

        if (!((flags & 0x10000) && player->IsInMyPartyOrRaid(unit)) && !((flags & 0x20000) && player->IsInMyParty(unit)) && !((flags & 0x40000) && player->CanAssist(unit, false)) && !((flags & 0x80000) && player->CanAttack(unit, false))) {
            return 0;
        }
    }

    if (flags & 0x300000) {
        if (unit->m_unit->m_unitFieldHealth > 0 ? !(flags & 0x100000) : !(flags & 0x200000)) {
            return 0;
        }
    }

    if ((flags & 0x400000) && unit->GetCreatureType() == 12) {
        return 1;
    }

    if (!Spell_C_IsTargeting()) {
        return 1;
    }

    int32_t restricted = (unit->m_unit->m_unitFieldFlags >> 8) & 1;

    if (!restricted) {
        return 1;
    }

    const SpellRec* spell = g_spellDB.GetRecord(Spell_C_GetTargetingSpell());

    if (spell && ((spell->m_attributesExC & 0x10000000) || (spell->m_attributesExE & 0x200000))) {
        return 1;
    }

    return 0;
}

// OFFSET: 0x008E0A10
int32_t CGWorldFrame::IsLegalSelection(CGObject_C* object, uint32_t flags) {
    switch (object->m_obj->m_type) {
        case TYPEMASK_OBJECT | TYPEMASK_UNIT:
            if (!(flags & 0x8)) {
                return 0;
            }

            return this->IsUnitLegalSelection(static_cast<CGUnit_C*>(object), flags);

        case TYPEMASK_OBJECT | TYPEMASK_UNIT | TYPEMASK_PLAYER:
            if (!(flags & 0x10)) {
                return 0;
            }

            return this->IsUnitLegalSelection(static_cast<CGUnit_C*>(object), flags);

        case TYPEMASK_OBJECT | TYPEMASK_GAMEOBJECT:
            if (!(flags & 0x4) || (Spell_C_IsTargeting() && !Spell_C_CanTargetObject(static_cast<CGGameObject_C*>(object)))) {
                return 0;
            }

            return 1;

        case TYPEMASK_OBJECT | TYPEMASK_CORPSE:
            if (!(flags & 0x40)) {
                return 0;
            }

            return this->CanTargetCorpseSelection(static_cast<CGCorpse_C*>(object), flags);

        default:
            return 0;
    }
}

// OFFSET: 0x008E0B30
uint32_t CGWorldFrame::GetHitTestFilterFlags(int32_t a1) {
    if (CGPetInfo::m_petOrderMoveTo) {
        return 0x3;
    }

    if (Spell_C_IsTargeting()) {
        uint32_t flags = 0;

        if (Spell_C_CanTargetTerrain()) {
            flags = 0x3;
        }

        if (Spell_C_CanTargetObjects()) {
            flags |= 0x4;
        }

        if (Spell_C_CanTargetUnits()) {
            flags |= 0x18;

            if (Spell_C_CanTargetMe()) {
                flags |= 0x20;
            }

            if (Spell_C_CanTargetParty()) {
                flags |= 0x10000;
            }

            if (Spell_C_CanTargetFriends()) {
                flags |= 0x40000;
            }

            if (Spell_C_CanTargetEnemies()) {
                flags |= 0x80000;
            }

            if (Spell_C_CanTargetRaid()) {
                flags |= 0x20000;
            }

            if (Spell_C_CanTargetDead()) {
                flags |= 0x200040;
            }

            if (Spell_C_CanTargetAlive()) {
                flags |= 0x100000;
            }

            if (Spell_C_CanTargetNonCombatPet()) {
                flags |= 0x400000;
            }

            if (Spell_C_CanTargetPossessedFriends()) {
                flags |= 0x800000;
            }
        }

        if (Spell_C_CanTargetFriendCorpses()) {
            flags |= 0x40040;
        }

        if (Spell_C_CanTargetEnemyCorpses()) {
            return flags | 0x80040;
        }

        return flags;
    }

    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));
    uint32_t flags = 0;
    CGUnit_C* mover = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(CGUnit_C::s_activeMover, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (mover && mover->CanAutoInteract() && !CGGameUI::m_cursorItemType) {
        uint32_t moveFlags = mover->m_movement->m_flags;
        flags = 1;

        if ((moveFlags & 0x4000000) && !(moveFlags & 0x100000)) {
            flags = 3;
        }
    }

    if (player) {
        return flags | 0x5C;
    }

    return flags;
}

// OFFSET: 0x008E0CB0/0x14060DCD0
static int32_t ObjectCollisionCallback(uint64_t param64, uint32_t param32, CM2Model* model, int32_t blobShadow) {
    if (!blobShadow || !model->IsDrawable() || (model->m_fFlags & 0x2000)) {
        return 1;
    }

    CGObject_C* object = ClntObjMgrObjectPtr(WOWGUID::Make(param64), TYPEMASK_OBJECT, __FILE__, __LINE__);

    if (!object) {
        return 1;
    }

    if (object->m_obj->m_type & TYPEMASK_UNIT) {
        CGUnit_C* unit = static_cast<CGUnit_C*>(object);

        if (unit->m_unit->m_unitFieldBytes1[0] != 9) {
            float alpha = model->m_alpha;
            CAaBox box = unit->GetStandBox();
            World::ShadowRender(object->GetPosition(), object->GetDisplayFacing(), object->GetTrueScale(), box, alpha);
        }
    } else if (object->m_obj->m_type & TYPEMASK_CORPSE) {
        M2SequenceInfo info;
        model->GetSequenceInfo(0, 0, info);
        float alpha = model->m_alpha;
        World::ShadowRender(object->GetPosition(), object->GetDisplayFacing(), object->GetTrueScale(), info.m_bounds, alpha);
    }

    return 1;
}

// OFFSET: 0x008E0DC0/0x14060DEC0
void CGWorldFrame::PerformDefaultAction(MOUSEBUTTON button) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player || player->m_unit->m_unitFieldCharmedby) {
        CWorldClickEvent evt;
        evt.m_segStart = C3Vector(0.0f, 0.0f, 0.0f);
        evt.m_segEnd = C3Vector(0.0f, 0.0f, 0.0f);
        evt.m_button = button;
        CGGameUI::HandleWorldClick(evt);
        return;
    }

    CGNamePlateFrame* namePlate = CGNamePlateFrame::GetNamePlateFocus();

    if (namePlate) {
        if (button == 1) {
            CGGameUI::OnSpriteLeftClick(namePlate->m_guid);
        } else {
            CGGameUI::OnSpriteRightClick(namePlate->m_guid);
        }

        return;
    }

    switch (this->m_defaultActionHitKind) {
        case 0: {
            CWorldClickEvent evt;
            evt.m_segStart = this->m_defaultActionHit.m_segStart;
            evt.m_segEnd = this->m_defaultActionHit.m_segEnd;
            evt.m_button = button;
            CGGameUI::HandleWorldClick(evt);
            break;
        }

        case 1:
        case 3: {
            CTerrainClickEvent evt;
            uint32_t high = static_cast<uint32_t>(this->m_defaultActionHit.m_guid.m_guid >> 32);

            if ((high & 0xF0000000) == 0x10000000 && (high & 0xFF00000) == 0xFC00000) {
                evt.m_guid = this->m_defaultActionHit.m_guid;
            } else {
                evt.m_guid = WOWGUID();
            }

            evt.m_point = this->m_defaultActionHit.m_point;
            evt.m_button = button;
            CGGameUI::HandleTerrainClick(evt);
            break;
        }

        case 2: {
            CSpriteClickEvent evt;
            evt.m_guid = this->m_defaultActionHit.m_guid;
            evt.m_button = button;
            CGGameUI::HandleSpriteClick(evt);
            break;
        }

        default:
            break;
    }
}

// OFFSET: 0x008E0FA0/0x14060E0A0
void __stdcall CGWorldFrame::CursorTrackUnit(CGUnit_C* unit) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return;
    }

    if (player->m_auraEffectMask[41] & 0x40) {
        CursorResetMode();
        return;
    }

    if (((player->m_combatModeFlags & 1) && player->GetTransportGUID() != player->GetFarSightGuid()) || !player->VehicleAllowsInteraction()) {
        CGUnit_C* possessed = player->GetPossessedUnit();

        if (unit->m_unit->m_unitFieldHealth > 0 && possessed && possessed->CanAttackNow(unit)) {
            CursorSetMode(4);
        } else {
            CursorResetMode();
        }

        return;
    }

    float distance = player->GetDistanceToPos(unit->GetPosition());

    if (player->CanInteractNow(unit) && unit->m_unit->m_unitNpcFlags) {
        float reach = unit->m_unit->m_unitFieldCombatreach + 4.0f;
        int32_t unable = reach * reach < distance;
        int32_t mode = unit->GetCursorOverride(unable);

        if (mode) {
            if (mode == 53) {
                char file[260];
                unit->GetCursorFile(unable, file, sizeof(file));
                CursorSetFromName(file);
            } else {
                CursorSetMode(mode);
            }

            return;
        }

        uint32_t flags = unit->m_unit->m_unitNpcFlags;

        if (flags & 0x1000) {
            CursorSetMode(unable ? 44 : 18);
        } else if (flags & 0x10000) {
            CursorSetMode(unable ? 48 : 22);
        } else if (flags & 0x2000) {
            if (unit->m_interactIconType != 3) {
                CursorSetMode(unable ? 35 : 9);
            } else {
                CursorResetMode();
            }
        } else if (flags & 0x10) {
            CursorSetMode(unable ? 36 : 10);
        } else if ((flags & 0x4000) || (flags & 0x8000)) {
            CursorSetMode(unable ? 32 : 6);
        } else if ((flags & 0x20000) || (flags & 0x800000)) {
            CursorSetMode(unable ? 29 : 3);
        } else if ((flags & 0x40000) || (flags & 0x80000) || unit->IsGuildMaster() || (flags & 0x100000)) {
            CursorSetMode(unable ? 32 : 6);
        } else if (flags & 0x200000) {
            CursorSetMode(unable ? 29 : 3);
        } else if (flags & 0x400000) {
            CursorSetMode(unable ? 32 : 6);
        } else if (flags & 0x80) {
            CursorSetMode(unable ? 34 : 8);
        } else if (flags & 0x1) {
            CursorSetMode(unable ? 32 : 6);
        } else if (flags & 0x1000000) {
            CursorSetMode(unable ? 31 : 5);
        } else if (unit->CanBeSpellClickedByLocalPlayer(player)) {
            CursorSetMode(unable ? 52 : 26);
        } else {
            CursorResetMode();
        }

        return;
    }

    if (unit->CanBeLooted(FrameTime::GetCurTimeMs())) {
        if (player->CanLoot(unit) || player->m_lootTarget == unit->m_obj->m_guid) {
            CursorSetMode(CGGameUI::IsAutoLooting() ? 16 : 8);
        } else {
            CursorSetMode(CGGameUI::IsAutoLooting() ? 42 : 34);
        }

        return;
    }

    if ((unit->m_unit->m_unitFieldFlags & 0x4000000) && CGSpellBook::GetSkinningSpell(unit) && !((unit->m_obj->m_type & TYPEMASK_PLAYER) && player->CanAssist(unit, false))) {
        float minRange;
        float maxRange;
        Spell_C_GetMinMaxRange(player, CGSpellBook::GetSkinningSpell(unit), &minRange, &maxRange, unit);

        if (!(unit->m_obj->m_type & TYPEMASK_PLAYER)) {
            int32_t type = unit->GetSkinningType();

            switch (type) {
                case 0:
                    CursorSetMode(distance <= maxRange * maxRange ? 12 : 38);
                    return;

                case 1:
                    CursorSetMode(distance <= maxRange * maxRange ? 13 : 39);
                    return;

                case 2:
                case 3:
                    CursorSetMode(distance <= maxRange * maxRange ? 11 : 37);
                    return;

                default:
                    DebugLog("Unknown skinning type %d", type);
                    CursorResetMode();
                    return;
            }
        }

        if (distance <= maxRange * maxRange) {
            CursorSetMode(s_skinCursorModes[player->GetPVPFactionIndex()]);
        } else {
            CursorSetMode(s_skinCursorModesUnable[player->GetPVPFactionIndex()]);
        }

        return;
    }

    if (!player->CanAttackNow(unit) || (player->m_unit->m_unitFieldFlags & 0x20000) || (player->m_unit->m_unitFieldFlags2 & 0x800000) || !CGGameUI::m_hasControl) {
        CursorResetMode();
        return;
    }

    float range = player->GetMeleeRange(unit);
    CursorSetMode(distance <= range * range ? 4 : 30);
}

// OFFSET: 0x008E15C0/0x14060E6C0
void __stdcall CGWorldFrame::CursorTrackObject(CGGameObject_C* object) {
    int32_t mode = object->CanChangeCursor();

    if (!mode) {
        CursorResetMode();
        return;
    }

    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player && (player->m_auraEffectMask[41] & 0x40)) {
        CursorResetMode();
        return;
    }

    if (mode == 53) {
        char file[260];
        object->GetCursorFile(file, sizeof(file));
        CursorSetFromName(file);
    } else {
        CursorSetMode(mode);
    }
}

// OFFSET: 0x008E1650/0x14060E770
void __stdcall CGWorldFrame::CursorTrackCorpse(CGCorpse_C* corpse) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return;
    }

    if (player->m_auraEffectMask[41] & 0x40) {
        CursorResetMode();
        return;
    }

    C3Vector corpsePosition = corpse->GetPosition();
    C3Vector position = player->GetPosition();
    float dy = position.y - corpsePosition.y;
    float distance = (position.z - corpsePosition.z) * (position.z - corpsePosition.z) + (position.x - corpsePosition.x) * (position.x - corpsePosition.x) + dy * dy;

    if (corpse->CanBeLooted()) {
        if (player->CanLoot(corpse) || player->m_lootTarget == corpse->m_obj->m_guid) {
            CursorSetMode(CGGameUI::IsAutoLooting() ? 16 : 8);
        } else {
            CursorSetMode(CGGameUI::IsAutoLooting() ? 42 : 34);
        }

        return;
    }

    const CGUnit_C* asUnit = reinterpret_cast<const CGUnit_C*>(corpse);

    if ((corpse->m_corpse->m_corpseFieldFlags & 0x20) && CGSpellBook::GetSkinningSpell(asUnit) && !player->CanAssist(corpse)) {
        float minRange;
        float maxRange;
        Spell_C_GetMinMaxRange(player, CGSpellBook::GetSkinningSpell(asUnit), &minRange, &maxRange, NULL);

        if (distance <= maxRange * maxRange) {
            CursorSetMode(s_skinCursorModes[player->GetPVPFactionIndex()]);
        } else {
            CursorSetMode(s_skinCursorModesUnable[player->GetPVPFactionIndex()]);
        }

        return;
    }

    CursorResetMode();
}

// OFFSET: 0x008E1810/0x14060E970
void CGWorldFrame::OnLayerTrackObject(const HITTESTRESULT& result) {
    if (this->m_worldFlags & 0x2) {
        this->HandleSpriteTrack(WOWGUID());
        return;
    }

    if (Spell_C_IsTargeting()) {
        WOWGUID guid = result.m_guid;
        CursorSetMode(Spell_C_HandleSpriteRay(guid, true) ? 2 : 28);
        this->HandleSpriteTrack(result.m_guid);
        return;
    }

    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));
    CGObject_C* object = ClntObjMgrObjectPtr(result.m_guid, TYPEMASK_OBJECT, __FILE__, __LINE__);

    if (!player || !object || !object->CanHighlight()) {
        CursorResetMode();
        this->HandleSpriteTrack(WOWGUID());
        return;
    }

    if (result.m_distance == 0.0f) {
        CursorResetMode();
        this->HandleSpriteTrack(result.m_guid);
        return;
    }

    switch (object->m_obj->m_type) {
        case TYPEMASK_OBJECT | TYPEMASK_ITEM:
            CursorSetMode(8);
            break;

        case TYPEMASK_OBJECT | TYPEMASK_UNIT:
        case TYPEMASK_OBJECT | TYPEMASK_UNIT | TYPEMASK_PLAYER:
            CGWorldFrame::CursorTrackUnit(static_cast<CGUnit_C*>(object));
            break;

        case TYPEMASK_OBJECT | TYPEMASK_GAMEOBJECT:
            CGWorldFrame::CursorTrackObject(static_cast<CGGameObject_C*>(object));
            break;

        case TYPEMASK_OBJECT | TYPEMASK_CORPSE:
            CGWorldFrame::CursorTrackCorpse(static_cast<CGCorpse_C*>(object));
            break;

        default:
            CursorResetMode();
            this->HandleSpriteTrack(result.m_guid);
            return;
    }

    this->HandleSpriteTrack(result.m_guid);
}

// OFFSET: 0x008E1AA0/0x14060EB50
static void GetTimeOfDay(int32_t* timeOfDay, float* day, float* dayProgression) {
    WOWGUID activePlayer = ClntObjMgrGetActivePlayer();
    CGPlayer_C* player = activePlayer ? static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(activePlayer, TYPEMASK_PLAYER, __FILE__, __LINE__)) : NULL;

    if (player && (player->m_unit->m_unitFieldFlags & 0x100000) && (player->m_player->m_playerFlags & 0x20000)) {
        *timeOfDay = 0;
        *dayProgression = 0.5f;
        *day = 0.0f;
    } else {
        *timeOfDay = g_clientGameTime.GetHourAndMinutes();
        *dayProgression = g_clientGameTime.GameTimeGetDayProgression();
        *day = static_cast<float>(g_clientGameTime.GetDaysSinceEpoch());
    }

    MapRec* map = g_mapDB.GetRecord(World::QueryMapId());

    if (map && map->m_timeOfDayOverride != -1) {
        *timeOfDay = 0;
        *dayProgression = map->m_timeOfDayOverride * 0.00069444446f;
        *day = 0.0f;

        if (*dayProgression < 0.0f || *dayProgression > 1.0f) {
            SysMsgPrintf(STATUS_ERROR, "Invalid time of day override in Map ID %d. TOD = %d.", World::QueryMapId(), map->m_timeOfDayOverride);
            *dayProgression = 0.5f;
        }
    }
}

// OFFSET: 0x008E1BD0/0x14060ECC0
static void SetTimeOfDayOverride(int32_t timeOfDay) {
    bool active = false;
    float progression = 0.0f;

    if (timeOfDay != -1) {
        progression = timeOfDay * 0.00069444446f;
        active = true;

        if (progression < 0.0f || progression > 1.0f) {
            SysMsgPrintf(STATUS_ERROR, "Invalid time of day override. TOD = %d.", timeOfDay);
            progression = 0.5f;
        }
    }

    if (active) {
        s_timeOfDayOverrideActive = 1;
        s_timeOfDayOverride = progression;
    } else {
        if (!s_timeOfDayOverrideActive) {
            return;
        }

        s_timeOfDayOverrideActive = 0;
        int32_t currentTimeOfDay;
        float day;
        GetTimeOfDay(&currentTimeOfDay, &day, &progression);
    }

    s_timeOfDayBlendTime = fabs(progression - s_timeOfDayProgression) * 60.0f;
    s_timeOfDayBlendRemaining = s_timeOfDayBlendTime;
}

// OFFSET: 0x008E1C90/0x14060ED90
void CGWorldFrame::SetPlayerFadeCameraValue(uint8_t value) {
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(this->m_camera->m_targetGUID, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (!unit) {
        return;
    }

    if ((unit->m_vehicle && unit->m_vehicle->m_vehicleRec) || (unit->m_vehiclePassenger && unit->m_vehiclePassenger->m_state == 3)) {
        unit = unit->m_vehiclePassenger ? unit->m_vehiclePassenger->GetStoredRootVehicleUnit() : unit->FindRootVehicle(NULL);

        if (!unit) {
            return;
        }
    }

    unit->SetMaxAlpha(value * 0.0039215689f);
}

// OFFSET: 0x008E1D30/0x14060EE30
int32_t CGWorldFrame::Update(const void* data, void* param) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player) {
        player->UpdateActiveMoverDisplay(FrameTime::GetCurTimeMs());
    }

    PlayerNameClearFrameFlags();
    g_scene.Update(FrameTime::GetCurTimeMs());
    CAnimKitManager::GarbageCollect();
    return 1;
}

// OFFSET: 0x008E1D80
void CGWorldFrame::RenderPostAnimateCallback(void* param, uint32_t pass, uint64_t param64) {
    static_cast<CGWorldFrame*>(param)->RenderPostAnimateCallback(pass, param64);
}

// OFFSET: 0x008E1DA0/0x14060EEB0
static float GetTimeScale(float elapsedSec) {
    float scale = 1.0f;
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player) {
        for (CEffect* effect = player->m_effectList; effect; effect = effect->m_next) {
            scale *= effect->GetCurveAlpha(elapsedSec);
        }
    }

    if (s_minTimeScale > scale) {
        scale = s_minTimeScale;
    }

    if (scale > s_maxTimeScale) {
        return s_maxTimeScale;
    }

    return scale;
}

// OFFSET: 0x008E1E40/0x14060EF50
void CGWorldFrame::SetScreenEffect(int32_t effect) {
    ScreenEffectRec* rec = g_screenEffectDB.GetRecord(effect);

    if (!rec) {
        if (!CGWorldFrame::ffxEffectUnconscious || !FFX::EffectIsSet(CGWorldFrame::ffxEffectUnconscious) || static_cast<EffectUnconscious*>(CGWorldFrame::ffxEffectUnconscious)->FadedOut()) {
            DayNight::ClearOverrideFog();
            FFX::SetEffect(1, CGWorldFrame::ffxEffectGlow);
            DayNight::ClearOverrideLightParamsID();
            SetTimeOfDayOverride(-1);
            SI3::SetScreenEffect(0, 0);
        }

        return;
    }

    uint32_t flags = 0;

    switch (rec->m_effect) {
        case 0:
            flags = rec->m_param[3];
            DayNight::ClearOverrideFog();
            FFX::SetEffect(1, CGWorldFrame::ffxEffectGlow);
            break;

        case 1:
            DayNight::ClearOverrideFog();
            FFX::SetEffect(1, CGWorldFrame::ffxEffectDeath);
            break;

        case 2: {
            CImVector color;

            if (FFX::s_cvarFFX->m_intValue) {
                color.b = 0xFF;
                color.g = 0xFF;
                color.r = 0xFF;
                color.a = 0xFF;
            } else {
                color.b = 0x63;
                color.g = 0x4C;
                color.r = 0x4C;
                color.a = 0xFF;
            }

            DayNight::SetOverrideFog(0.7f, 150.0f, color, 0);
            FFX::SetEffect(1, CGWorldFrame::ffxEffectNetherWorld);
            break;
        }

        case 3:
            DayNight::ClearOverrideFog();

            if (g_theGxDevicePtr->DeviceApi()) {
                PassFogSeed::DeviceRestoredCallback();
                CGWorldFrame::ffxEffectSwirlingFog->SetParameters(3, rec->m_param);
                FFX::SetEffect(1, CGWorldFrame::ffxEffectSwirlingFog);
            } else {
                FFX::SetEffect(1, CGWorldFrame::ffxEffectGlow);
            }

            break;

        case 4:
            flags = rec->m_param[3];
            DayNight::ClearOverrideFog();
            FFX::SetEffect(1, CGWorldFrame::ffxEffectUnconscious);
            CGWorldFrame::ffxEffectUnconscious->SetParameters(4, rec->m_param);
            break;

        default:
            DebugLog("Unhandled screen effect %d", rec->m_effect);
            break;
    }

    DayNight::SetOverrideLightParamsID(rec->m_lightParamsID, flags & 2);
    SetTimeOfDayOverride(rec->unk_0028);
    SI3::SetScreenEffect(rec->m_soundAmbienceID, rec->m_zoneMusicID);
}

// OFFSET: 0x008E2040/0x14060F1A0
void CGWorldFrame::UpdateScreenEffectParams() {
    FFX::s_camPos = World::GetCamPos();
    FFX::s_curTimeMs = FrameTime::GetCurTimeMs();
    FFX::s_tickTimeSec = FrameTime::GetTickTimeSec();

    if (Liquid::GetWaterDetail()) {
        float fogStart;
        float fogEnd;
        float fogDensity;
        CImVector fogColor;
        *reinterpret_cast<uint32_t*>(&fogColor) = 0;
        Liquid::GetUnderWaterFog(fogStart, fogEnd, fogDensity, fogColor);

        if (g_theGxDevicePtr->MasterEnable(GxMasterEnable_Fog)) {
            float scale = 1.0f / (fogEnd - fogStart);
            FFX::psProcWaterConsts[1] = C4Vector(-(scale * CShaderEffect::s_fogMul), scale * fogEnd, fogDensity, 0.0f);
        } else {
            FFX::psProcWaterConsts[1] = C4Vector(0.0f, 1.0f, 1.0f, 0.0f);
        }

        FFX::psProcWaterConsts[0] = C4Vector(fogColor.r * 0.0039215689f, fogColor.g * 0.0039215689f, fogColor.b * 0.0039215689f, 0.0f);

        C3Vector camPos = World::GetCamPos();
        FFX::psProcWaterConsts[5] = C4Vector(camPos.x, camPos.y, camPos.z, 0.0f);

        const C3Vector* corners = World::GetCamFrustumCorners();
        FFX::psProcWaterConsts[2] = C4Vector(corners[1].x - camPos.x, corners[1].y - camPos.y, corners[1].z - camPos.z, 0.0f);
        FFX::psProcWaterConsts[3] = C4Vector(corners[2].x - corners[1].x, corners[2].y - corners[1].y, corners[2].z - corners[1].z, 0.0f);
        FFX::psProcWaterConsts[4] = C4Vector(corners[0].x - corners[1].x, corners[0].y - corners[1].y, corners[0].z - corners[1].z, 0.0f);
        FFX::psProcWaterConsts[6] = ViewSettings::CalculateParams();

        C3Vector sun;
        C3Vector moon;
        DayNight::GetSunPos(&sun);
        DayNight::GetMoonPos(&moon);
        C4Vector color;

        if (sun.z <= moon.z) {
            FFX::psProcWaterConsts[9] = C4Vector(moon.x, moon.y, moon.z, 0.0f);
            FFX::psProcWaterConsts[10].x = DayNight::GetMoonGlareOpacity();
            float height = ((moon.z - camPos.z) - 3.0f) * 0.14285715f;
            height = height >= 0.0f ? (height >= 1.0f ? 1.0f : height) : 0.0f;
            FFX::psProcWaterConsts[10].y = height * 1.2f;
            color = C4Vector(0.26666668f, 0.57401389f, 0.71149766f, 0.0f);
        } else {
            FFX::psProcWaterConsts[9] = C4Vector(sun.x, sun.y, sun.z, 0.0f);
            FFX::psProcWaterConsts[10].x = DayNight::GetSunGlareOpacity();
            float height = ((sun.z - camPos.z) - 6.0f) * 0.25f;
            height = height >= 0.0f ? (height >= 1.0f ? 1.0f : height) : 0.0f;
            FFX::psProcWaterConsts[10].y = height * 1.4f;
            color = C4Vector(0.59754711f, 0.70368195f, 0.91177231f, 0.0f);
        }

        FFX::psProcWaterConsts[11] = color;
        FFX::psProcWaterConsts[12].x = World::GetCamDepth();
        FFX::psProcWaterConsts[12].y = World::GetCamDepth() + camPos.z;
        FFX::psProcWaterConsts[12].z = World::GetSmoothCamDepth();
        FFX::psProcWaterConsts[13] = C4Vector(4.0f, 0.0f, 120.0f, 100.0f);
    }

    uint8_t level = 0;
    int32_t underwater = 0;
    uint8_t minimum = 0;
    CGPlayer_C* player;

    if (ClntObjMgrGetCurrent() && (player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__))) != NULL) {
        float drunk = player->GetDrunknessTurnValue() * 255.0f + 512.0f;
        level = *reinterpret_cast<uint32_t*>(&drunk) >> 14;

        if (World::SceneCamLiquidStatus(NULL)) {
            minimum = 84;
            underwater = 1;
        }
    }

    if (underwater && CGWorldFrame::ffxEffectUnderwater) {
        FFX::SetEffect(0, CGWorldFrame::ffxEffectUnderwater);
    } else {
        FFX::SetEffect(0, NULL);
    }

    if (level <= minimum) {
        level = minimum;
    }

    float glow = DayNight::GetGlow() * 255.0f + 512.0f;
    uint8_t glowLevel = *reinterpret_cast<uint32_t*>(&glow) >> 14;

    if (CGWorldFrame::ffxEffectGlow) {
        int32_t params[3] = { underwater, glowLevel, level };
        CGWorldFrame::ffxEffectGlow->SetParameters(3, params);
    }

    if (CGWorldFrame::ffxEffectUnconscious && FFX::EffectIsSet(CGWorldFrame::ffxEffectUnconscious)) {
        EffectUnconscious* unconscious = static_cast<EffectUnconscious*>(CGWorldFrame::ffxEffectUnconscious);
        unconscious->SetGlowBlur(glowLevel, level);

        if (unconscious->FadingOut() && unconscious->FadedOut()) {
            CGWorldFrame::SetScreenEffect(0);
        }
    }
}

// OFFSET: 0x008E2670/0x14060F930
void CGWorldFrame::UpdateWorld() {
    int32_t cameraAtPlayer = 0;
    C3Vector weatherPosition(0.0f, 0.0f, 0.0f);
    C3Vector weatherVelocity(0.0f, 0.0f, 0.0f);
    float weatherFacing = 0.0f;
    int32_t weatherOnTaxi = 0;
    const C3Spline_CatmullRom* weatherSpline = NULL;
    float splineElapsedTime = 0.0f;
    float splineDuration = 0.0f;

    WOWGUID activePlayer = ClntObjMgrGetActivePlayer();

    if (activePlayer) {
        CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(activePlayer, TYPEMASK_PLAYER, __FILE__, __LINE__));

        if (player) {
            weatherPosition = player->GetPosition();
            CMovement_C* movement = player->m_movement;
            float speed = movement->m_currentSpeed;
            weatherVelocity = C3Vector(speed * movement->m_moveDir.x, movement->m_moveDir.y * speed, movement->m_moveDir.z * speed);
            weatherFacing = player->GetFacing();
            weatherOnTaxi = (player->m_unit->m_unitFieldFlags >> 20) & 1;
            weatherSpline = player->GetSpline();
            splineElapsedTime = player->GetSplineElapsedTime();
            splineDuration = player->GetSplineDuration();
        }
    }

    CGObject_C* target = ClntObjMgrObjectPtr(this->m_camera->m_targetGUID, TYPEMASK_OBJECT, __FILE__, __LINE__);

    if (target) {
        if (target->m_obj->m_type & TYPEMASK_PLAYER) {
            cameraAtPlayer = (static_cast<CGPlayer_C*>(target)->m_player->m_playerFlags >> 14) & 1;
        }
    } else {
        CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

        if (player) {
            if (player->m_combatModeFlags & 0x1) {
                player->ToggleFarSight(NULL);
            }

            this->m_camera->SetTarget(player, 0);
            player->OnVehicleCameraPossiblyNeeded();
            cameraAtPlayer = (player->m_player->m_playerFlags >> 14) & 1;
            target = player;
        }
    }

    CCameraManager* manager = StaticSingleton<CCameraManager>::Instance();
    const CCameraShot& shot = manager->m_shot;
    C3Vector focus = (this->m_camera->m_model || !target) ? shot.m_position : target->GetPosition();

    if (StaticSingleton<CCameraManager>::Instance()->IsCompletelyLegacy()) {
        if (cameraAtPlayer) {
            focus = shot.m_position;
        }
    } else {
        focus = shot.m_target;
    }

    float nearClip = World::GetNearClip();
    float farClip = World::GetFarClip();
    float fov = shot.m_fov;
    World::Update(shot.m_position, shot.m_target, nearClip, farClip, fov, StaticSingleton<CCameraManager>::Instance()->m_aspect, focus, weatherPosition, weatherVelocity, weatherFacing, weatherOnTaxi, weatherSpline, splineElapsedTime, splineDuration);
}

// OFFSET: 0x008E2990/0x14060FD60
void CGWorldFrame::UpdatePlayerAmbientMultiplier() {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return;
    }

    const AreaTableRec* area = AreaListGetObjectArea(player->m_worldObject);

    if (!area) {
        return;
    }

    if (!(area->m_flags & 0x2000) && area->m_parentAreaID) {
        area = g_areaTableDB.GetRecord(area->m_parentAreaID);
    }

    if (!area) {
        return;
    }

    float multiplier = area->m_ambientMultiplier + area->m_ambientMultiplier + 1.0f;
    float delta = multiplier - World::GetCharacterAmbMul();
    float tick = FrameTime::GetTickTimeSec();

    if (fabs(delta) > 0.0099999998f && tick < 1.0f) {
        multiplier = World::GetCharacterAmbMul() + tick * delta;
    }

    if (multiplier < 1.3f) {
        multiplier = 1.3f;
    }

    World::SetObjectAmbientMultiplier(multiplier);
}

// OFFSET: 0x008E2AC0/0x14060FED0
void CGWorldFrame::MoveToFreeList(TSList<CModelRecord, TSGetLink<CModelRecord> >* list) {
    for (CModelRecord* record = list->Head(); record; record = list->Next(record)) {
        if (record->m_model) {
            record->m_model->Release();
            record->m_model = NULL;
        }
    }

    this->m_freeModelList.Combine(list, 2, NULL);
}

// OFFSET: 0x008E2BD0/0x140610020
void CGWorldFrame::UpdateDayNight(float deltaSec) {
    int32_t timeOfDay;
    float day;
    float progression;
    GetTimeOfDay(&timeOfDay, &day, &progression);

    if (s_timeOfDayOverrideActive) {
        progression = s_timeOfDayOverride;
    }

    float remaining = 0.0f;

    if (s_timeOfDayBlendRemaining - deltaSec > 0.0f) {
        remaining = s_timeOfDayBlendRemaining - deltaSec;
    }

    s_timeOfDayBlendRemaining = remaining;
    float t = 1.0f;

    if (s_timeOfDayBlendTime > 0.0099999998f) {
        float s = 1.0f - remaining / s_timeOfDayBlendTime;
        t = (3.0f - s * 2.0f) * s * s;
    }

    s_timeOfDayProgression = (1.0f - t) * s_timeOfDayProgression + t * progression;
    DayNight::SetTime(timeOfDay, day, s_timeOfDayProgression);
}

// PROVEN: 17dd596a IDENTICAL/NONE
// OFFSET: 0x008E2CB0/0x1406100F0
void CGWorldFrame::UnitClear() {
    this->MoveToFreeList(&this->m_modelList);
    this->MoveToFreeList(&this->m_hitModelList);
}

// OFFSET: 0x008E2CD0/0x140610120
void CGWorldFrame::OnWorldUpdate() {
    float elapsedSec = this->m_elapsedSec;
    CGObject_C* target = ClntObjMgrObjectPtr(this->m_camera->m_targetGUID, TYPEMASK_OBJECT, __FILE__, __LINE__);
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player && player->IsCommentatorUberOrInArena()) {
        if (this->m_camera->m_targetGUID) {
            this->m_camera->SetTarget(NULL, 0);
        }

        CGCommentator::s_Commentator.OnUpdate(this->m_elapsedSec);
        target = NULL;
    } else if (!target) {
        CGPlayer_C* activePlayer = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

        if (activePlayer) {
            if (activePlayer->m_combatModeFlags & 0x1) {
                activePlayer->ToggleFarSight(NULL);
            }

            this->m_camera->SetTarget(activePlayer, 0);
            activePlayer->OnVehicleCameraPossiblyNeeded();
            target = activePlayer;
        }
    }

    SpellVisualsFreeDeleted();

    if (target && target->m_worldObject) {
        SetWorldCameraTarget(target->m_worldObject, WOWGUID::Make(target->GetTransportGUID()));
    }

    CVehiclePassenger_C::UpdateAll(FrameTime::GetCurTimeMs());
    CGCamera::UpdateCallback(NULL, this->m_camera);

    CCameraManager* manager = StaticSingleton<CCameraManager>::Instance();
    C3Vector camPos = manager->m_shot.m_position;
    C3Vector camTarget = manager->m_shot.m_target;
    manager->Update(this->m_elapsedSec);
    StaticSingleton<CCameraManager>::Instance()->ApplyWorldProjection(this->m_viewportDDC);

    if (CGUnit_C::s_activeMover) {
        CGObject_C* mover = ClntObjMgrObjectPtr(CGUnit_C::s_activeMover, TYPEMASK_UNIT, __FILE__, __LINE__);
        World::SetActivePlayerPos(mover ? mover->GetPosition() : camTarget);
    }

    this->UpdateDayNight(elapsedSec);
    CGWorldFrame::UpdatePlayerAmbientMultiplier();

    if (!this->m_camera->HasModelCamera() && SI3::sm_pCVar_Sound_ListenerAtCharacter && SI3::sm_pCVar_Sound_ListenerAtCharacter->m_intValue) {
        CGObject_C* focus = CGObject_C::GetFocusObject();

        if (focus) {
            s_listenerUp = C3Vector(0.0f, 0.0f, 1.0f);

            C3Vector facing;

            if (focus->m_obj->m_type & TYPEMASK_UNIT) {
                facing = GetFacingVector(focus);
            } else {
                const C44Matrix& matrix = StaticSingleton<CCameraManager>::Instance()->m_renderMatrix;
                facing = C3Vector(matrix.a0, matrix.a1, matrix.a2);
            }

            C3Vector offset(facing.x * 0.0f, facing.y * 0.0f, facing.z * 0.0f);
            C3Vector listener;

            if (focus->m_worldModel && (focus->m_worldModel->m_fFlags & 0x4000)) {
                C3Vector position = focus->m_worldModel->GetWorldPosition();
                listener = C3Vector(position.x - offset.x, position.y - offset.y, position.z - offset.z + 2.0f);
            } else {
                C3Vector position = focus->GetPosition();
                listener = C3Vector(position.x - offset.x, position.y - offset.y, position.z - offset.z + 2.0f);
            }

            SE3::SetListenerAttributes(listener, facing, s_listenerUp, NULL);
        } else {
            const C44Matrix& matrix = StaticSingleton<CCameraManager>::Instance()->m_renderMatrix;
            SE3::SetListenerAttributes(camPos, C3Vector(matrix.a0, matrix.a1, matrix.a2), C3Vector(matrix.c0, matrix.c1, matrix.c2), NULL);
        }
    } else {
        const C44Matrix& matrix = StaticSingleton<CCameraManager>::Instance()->m_renderMatrix;
        SE3::SetListenerAttributes(camPos, C3Vector(matrix.a0, matrix.a1, matrix.a2), C3Vector(matrix.c0, matrix.c1, matrix.c2), NULL);
    }

    CGObject_C::UpdateAllObjectEffectManagers();
    PlayerNameNextFrame();
    UpdatePendingPortraits();
    this->MoveToFreeList(&this->m_modelList);
    this->MoveToFreeList(&this->m_hitModelList);
    CGGameUI::ResetDisplayUpdateCount();
    ClntObjMgrEnumVisibleObjects(UnitUpdateProc, NULL);
    World::SetObjectRenderCallback(reinterpret_cast<World::RENDER_OBJECT_CALLBACK>(ObjectRenderCallback));
    World::SetObjectCollisionCallback(ObjectCollisionCallback);
    CEffect::CleanUpStaticLists();
    CMissile::UpdateStaticLists();
    CGObject_C::UpdateFadeoutList();
    Spell_C_CleanUpProcessedDestLocSpellCasts(FrameTime::GetCurTimeMs());
    Spell_C_UpdateInTransitList();
    CGGameObject_C::UpdateMapObjSpeeds();
    this->UpdateWorld();

    C44Matrix viewProjection;
    GxXformViewProj(viewProjection);

    if (viewProjection != this->m_viewProjection) {
        this->m_namePlateFlags |= 0x2;
        this->m_viewProjection = viewProjection;
    }

    ClientUpdateTaxiBenchmarkMode();
    SI3::UpdateTaxiSounds(elapsedSec);
    SI3::DSP_MakeDSPsExpire(elapsedSec);

    CGUnit_C* mover = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(CGUnit_C::s_activeMover, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (mover) {
        mover->UpdateMissileTrajectory();
    }

    CGUnit_C::UpdateAllSmoothFacing();
    CGWorldStateInfo::TimerUpdate();
}

// OFFSET: 0x008E3270/0x140610900
void CGWorldFrame::UpdateScreenEffect() {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        CGWorldFrame::SetScreenEffect(0);
        return;
    }

    uint32_t count = player->GetAuraCount();

    for (uint32_t i = count; i > 0; i--) {
        AuraData* aura = player->GetAura(i - 1);
        const SpellRec* spell = g_spellDB.GetRecord(aura->m_spellId);

        if (!spell) {
            continue;
        }

        uint32_t effectCount;
        const TSGrowableArray<const SpellEffectRec*>* effects = SpellGetEffects(spell->m_id, effectCount);

        for (uint32_t j = 0; j < effectCount; j++) {
            const SpellEffectRec* effect = effects->m_data[j];

            if (effect && effect->m_effectAura == 260) {
                CGWorldFrame::SetScreenEffect(effect->m_effectMiscValue);
                return;
            }
        }
    }

    if ((player->m_player->m_playerFlags & 0x10) && CGBattlefieldInfo::m_instanceType != 4) {
        CGWorldFrame::SetScreenEffect(1);
        return;
    }

    if ((player->m_player->m_playerFieldBytes2[3] & 0x40) || (count && (player->m_auraEffectMask[41] & 0x40))) {
        CGWorldFrame::SetScreenEffect(81);
        return;
    }

    CGWorldFrame::SetScreenEffect(0);
}

// OFFSET: 0x008E33E0
void CGWorldFrame::WaterResetCallback() {
    if (CGWorldFrame::s_currentWorldFrame) {
        CGWorldFrame::WaterReset();
        CGWorldFrame::UpdateScreenEffect();
    }
}

// OFFSET: 0x008E3470/0x140610B20
static void DrawSpellCursorShadow(uint32_t triFlags) {
    if (!s_spellShadowTexture[s_spellShadowStyle]) {
        return;
    }

    CGxTex* texture = TextureGetGxTex(s_spellShadowTexture[s_spellShadowStyle]);

    if (!texture) {
        return;
    }

    if (Spell_C_GetCursorWorldObject()->m_entity && !s_spellShadowStyle) {
        return;
    }

    float size = s_spellShadowSize;

    if (size == 0.0f) {
        size = 1.3888888f;
    }

    CAaBox box;
    box.b = C3Vector(s_spellShadowPos.x - size, s_spellShadowPos.y - size, s_spellShadowPos.z - 2.0f);
    box.t = C3Vector(s_spellShadowPos.x + size, s_spellShadowPos.y + size, s_spellShadowPos.z + 2.0f);

    g_theGxDevicePtr->RsPush();
    GxRsSet(GxRs_Lighting, 0);
    GxRsSet(GxRs_DepthWrite, 0);
    GxRsSet(GxRs_BlendingMode, 2);
    GxUpdateAlphaRef(GxRs_AlphaRef);
    g_theGxDevicePtr->RsSet(GxRs_Texture0, texture);
    GxRsSet(GxRs_ColorOp0, 0);
    GxRsSet(GxRs_AlphaOp0, 0);

    CImVector color(0xFF, 0xFF, 0xFF, 0xFF);
    World::ProjectTex2d(box, color, NULL, 0.5f, triFlags, 11, GxPolygonOffset_Default);
    g_theGxDevicePtr->RsPop();
}

// OFFSET: 0x008E36E0/0x140610DE0
uint64_t CGWorldFrame::FindClosestModel(const C3Vector& start, const C3Vector& end, uint32_t flags, float* distance) {
    const C3Vector& camera = StaticSingleton<CCameraManager>::Instance()->m_shot.m_position;
    C3Vector viewStart = start - camera;
    C3Vector viewEnd = end - camera;

    TSList<CModelRecord, TSGetLink<CModelRecord> > lowPriority;
    uint32_t time = FrameTime::GetCurTimeMs();
    World::GetScene()->BeginHitTest();
    int32_t allowsTerrain = Spell_C_TargetingSpellAllowsTerrain();

    for (CModelRecord* record = this->m_modelList.Head(); record;) {
        CModelRecord* next = this->m_modelList.Next(record);
        CGObject_C* object = ClntObjMgrObjectPtr(record->m_guid, TYPEMASK_OBJECT, __FILE__, __LINE__);

        if (object && this->IsLegalSelection(object, flags) && (allowsTerrain || object->CanHighlight())) {
            record->m_object = object;

            if ((object->m_obj->m_type & TYPEMASK_UNIT) && static_cast<CGUnit_C*>(object)->IsLowPrioritySelection(time)) {
                lowPriority.LinkToTail(record);
            } else {
                this->AddObjectToHitTestList(record, flags);
            }
        }

        record = next;
    }

    float t = 1.0f;
    CModelRecord* hit = static_cast<CModelRecord*>(World::GetScene()->EndHitTest(viewStart, viewEnd, t, 1));

    if (!hit) {
        World::GetScene()->BeginHitTest();

        for (CModelRecord* record = lowPriority.Head(); record; record = lowPriority.Next(record)) {
            this->AddObjectToHitTestList(record, flags);
        }

        t = 1.0f;
        hit = static_cast<CModelRecord*>(World::GetScene()->EndHitTest(viewStart, viewEnd, t, 1));
    }

    this->m_modelList.Combine(&lowPriority, 2, NULL);

    if (!hit) {
        return 0;
    }

    C3Vector delta = viewEnd - viewStart;
    float hitDistance = sqrt(delta.x * delta.x + delta.y * delta.y + delta.z * delta.z) * t;
    hit->m_distance = hitDistance;
    *distance = hitDistance;
    return hit->m_guid.m_guid;
}

// OFFSET: 0x008E3C00/0x140611620
int32_t CGWorldFrame::HitTest(const C3Vector& start, const C3Vector& end, uint32_t flags, HITTESTRESULT* result) {
    C3Vector hitPoint(0.0f, 0.0f, 0.0f);
    float t = 1.0f;
    float objectDistance = 0.0f;
    WOWGUID collision = WOWGUID();
    uint32_t worldFlags = (flags & 0x2) ? 0x1020124 : 0x1000124;
    C3Vector point = end;
    C3Vector direction = point - start;
    float length = direction.z * direction.z + direction.y * direction.y + direction.x * direction.x;

    if (fabs(length) >= 0.0000010000001f) {
        length = sqrt(length);
        float inverse = 1.0f / length;
        direction.x *= inverse;
        direction.y *= inverse;
        direction.z *= inverse;
    }

    int32_t hitWorld = World::Intersect(start, point, &hitPoint, &t, worldFlags, NULL) != 0;

    if (hitWorld) {
        if (t < -0.001f || t > 1.001f || !_finite(t)) {
            DebugLog("World::Intersect() returned a bogus value for terrDist: %f", static_cast<double>(t));
        }

        collision.m_guid = World::GetLastCollisionGUID();
        point.x = start.x + direction.x * (length * t);
        point.y = start.y + direction.y * (length * t);
        point.z = start.z + direction.z * (length * t);
        length *= t;
        t = length;
    }

    WOWGUID model = WOWGUID();

    if ((flags & 0x7C) && fabs(length) >= 0.001f) {
        model.m_guid = this->FindClosestModel(start, point, flags, &objectDistance);

        if (model) {
            if (objectDistance < -0.001f || !_finite(objectDistance)) {
                DebugLog("CGWorldFrame::FindClosestModel() returned a bogus value for objDist: %f", static_cast<double>(objectDistance));
            }

            length = objectDistance;
            point.x = start.x + direction.x * objectDistance;
            point.y = start.y + direction.y * objectDistance;
            point.z = start.z + direction.z * objectDistance;
        }
    }

    uint32_t terrain = flags & 0x3;

    if (terrain && fabs(length) >= 0.001f) {
        t = 1.0f;

        if (World::Intersect(start, point, &hitPoint, &t, 0x100151, NULL)) {
            if (t < -0.001f || t > 1.001f || !IsFinite(static_cast<double>(t))) {
                DebugLog("World::Intersect() returned a bogus value for terrDist: %f", static_cast<double>(t));
            }

            collision.m_guid = World::GetLastCollisionGUID();
            point.x = start.x + direction.x * (length * t);
            point.y = start.y + direction.y * (length * t);
            point.z = start.z + direction.z * (length * t);
            hitWorld = 1;
            t = length * t;
        }
    }

    C3Vector screen;
    screen.z = 0.0f;

    if (this->GetScreenCoordinates(point, &screen, NULL, NULL)) {
        g_theGxDevicePtr->CursorSetDepth(screen.z);
    }

    if (model) {
        result->m_point = point;
        result->m_guid = model;
        result->m_distance = objectDistance;
        return 2;
    }

    if (!hitWorld) {
        return 0;
    }

    if (collision) {
        result->m_point = point;
        CGObject_C* object = ClntObjMgrObjectPtr(collision, TYPEMASK_OBJECT, __FILE__, __LINE__);

        if (object) {
            result->m_guid = collision;

            if (object->IsTransport()) {
                C44Matrix inverse = object->GetMatrix().AffineInverse();
                result->m_point = point * inverse;
            }
        } else {
            result->m_guid = WOWGUID();
        }

        result->m_distance = t;
        return 3;
    }

    result->m_point = point;
    result->m_guid = WOWGUID();
    result->m_distance = t;
    return terrain != 0;
}

// OFFSET: 0x008E4150/0x140611BC0
int32_t CGWorldFrame::HitTestPoint(float x, float y, int32_t a3, HITTESTRESULT* result) {
    if (!(CGInputControl::GetActive()->m_mouseModeFlags & 0x1)) {
        return 0;
    }

    CGxDevice* device = g_theGxDevicePtr;
    C44Matrix projection;
    C44Matrix view;
    projection = device->m_projection;
    CGxMatrixStack& stack = device->m_xforms[GxXform_View];
    view = stack.m_mtx[stack.m_level];
    StaticSingleton<CCameraManager>::Instance()->ApplyWorldProjection(this->m_viewportDDC);

    int32_t kind = 0;
    uint32_t flags = this->GetHitTestFilterFlags(a3);

    if (flags) {
        C3Vector start(0.0f, 0.0f, 0.0f);
        C3Vector end(0.0f, 0.0f, 0.0f);

        if (this->GetLineSegment(x, y, &start, &end)) {
            result->m_segStart = start;
            result->m_segEnd = end;
            kind = this->HitTest(start, end, flags, result);

            if (kind >= 2) {
                this->m_hitGuid = result->m_guid;
            } else {
                this->MoveToFreeList(&this->m_hitModelList);
                this->m_hitGuid = WOWGUID();
            }
        }
    }

    g_theGxDevicePtr->XformSetProjection(projection);
    g_theGxDevicePtr->XformSetView(view);
    return kind;
}

// OFFSET: 0x008E43A0/0x1406120C0
void CGWorldFrame::UpdateObject(CGObject_C* object) {
    if (!(object->m_obj->m_type & (TYPEMASK_UNIT | TYPEMASK_GAMEOBJECT | TYPEMASK_CORPSE)) || !object->GetObjectModel()) {
        return;
    }

    CModelRecord* record;

    if (this->m_freeModelList.IsEmpty()) {
        record = this->m_modelList.NewNode(2, 0, 0);
    } else {
        record = this->m_freeModelList.Head();
        this->m_freeModelList.UnlinkNode(record);
        this->m_modelList.LinkToTail(record);
    }

    CM2Model* model = object->GetObjectModel();
    record->m_model = model;
    model->m_refCount++;
    record->m_guid = object->m_obj->m_guid;
}

// OFFSET: 0x008E4440/0x140612190
void CGWorldFrame::OnLayerUpdate(float elapsedSec) {
    float scaledSec = GetTimeScale(elapsedSec) * elapsedSec;
    this->CSimpleFrame::OnLayerUpdate(scaledSec);
    CGChatBubbleFrame::OnWorldLayerUpdate();

    CSimpleTop* top = this->m_simpleTop;
    HITTESTRESULT hit;
    hit.m_guid = WOWGUID();
    hit.m_point = C3Vector(0.0f, 0.0f, 0.0f);
    hit.m_distance = 0.0f;
    hit.m_segStart = C3Vector(0.0f, 0.0f, 0.0f);
    hit.m_segEnd = C3Vector(0.0f, 0.0f, 0.0f);
    int32_t kind = -1;
    CSimpleFrame* focus = top->m_mouseFocus;

    if (focus == this) {
        C2Vector ddc(0.0f, 0.0f);
        NDCToDDC(top->m_mousePosition.m_x, top->m_mousePosition.m_y, &ddc.x, &ddc.y);

        if (Spell_C_IsTargeting() && Spell_C_CanTargetTerrain() && !Spell_C_CanTargetUnits()) {
            CGUnit_C::ClearNamePlateFocus();
        } else {
            CGUnit_C::SetNamePlateFocus(ddc);
        }

        if (CGNamePlateFrame::GetNamePlateFocus()) {
            kind = 2;
            hit.m_guid = CGNamePlateFrame::GetNamePlateFocus()->m_guid;
        } else {
            kind = this->HitTestPoint(ddc.x, ddc.y, 1, &hit);
        }
    } else if (focus && (focus->m_layoutFlags & 0x100)) {
        lua_State* L = FrameScript_GetContext();
        const char* savedTaint = lua_tainted;

        if (++lua_taintexpected && !lua_taintedclosure) {
            lua_tainted = NULL;
        }

        lua_rawgeti(L, LUA_REGISTRYINDEX, FrameScript_GetErrorHandlerReference());
        lua_pushstring(L, "SecureButton_GetModifiedUnit");
        lua_rawget(L, LUA_GLOBALSINDEX);

        if (!focus->m_luaRegistered) {
            focus->RegisterScriptObject(NULL);
        }

        lua_rawgeti(L, LUA_REGISTRYINDEX, focus->m_luaObjectRef);
        lua_pushstring(L, CGUIMacros::m_macroRunning);

        if (!lua_pcall(L, 2, 1, -4)) {
            const char* token = lua_tolstring(L, -1, NULL);

            if (token && !lua_tainted && Script_GetGUIDFromToken(token, hit.m_guid, false, false)) {
                kind = 2;
            }
        }

        lua_settop(L, -3);

        if (lua_taintexpected && !lua_taintedclosure) {
            lua_tainted = savedTaint;
        }

        if (--lua_taintexpected <= 0) {
            lua_taintexpected = 0;
        }
    }

    s_spellShadowStyle = 3;

    switch (kind) {
        case 0:
            if (Spell_C_IsTargeting()) {
                CursorSetMode(28);
            } else {
                CursorResetMode();
            }

            this->HandleSpriteTrack(WOWGUID());
            break;

        case 1:
            this->OnLayerTrackTerrain(hit);
            break;

        case 2:
            this->OnLayerTrackObject(hit);
            break;

        case 3: {
            CGGameObject_C* gameObject = static_cast<CGGameObject_C*>(ClntObjMgrObjectPtr(hit.m_guid, TYPEMASK_GAMEOBJECT, __FILE__, __LINE__));

            if (!gameObject || gameObject->m_gameObject->m_typeID != 33 || Spell_C_CanTargetTerrain()) {
                this->OnLayerTrackTerrain(hit);
                break;
            }

            CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();

            if (player && gameObject->m_obj->m_guid.m_guid == World::QueryObjectParentParam64(player->m_worldObject)) {
                this->OnLayerTrackTerrain(hit);
                break;
            }

            this->OnLayerTrackObject(hit);
            break;
        }

        default:
            break;
    }

    CGInputControl::GetActive()->OnUpdate(elapsedSec);
    this->m_elapsedSec = scaledSec;
    CGGameUI::UpdateInteractTarget();
    CGGameUI::UpdateCorpseDistance();
    CGGameUI::UpdateAreaSpiritHealerDistance();
    CGMailInfo::UpdatePendingMail(elapsedSec);
}

// OFFSET: 0x008E47B0/0x140612570
World::HitTestDebug::HitTestDebug() {
    this->m_box.b.x = 0.0f;
    this->m_box.b.y = 0.0f;
    this->m_box.b.z = 0.0f;
    this->m_box.t.x = 0.0f;
    this->m_box.t.y = 0.0f;
    this->m_box.t.z = 0.0f;
    this->unk_0AA4.x = 0.0f;
    this->unk_0AA4.y = 0.0f;
    this->unk_0AA4.z = 0.0f;
    this->unk_0AB0 = 0.0f;
    this->unk_0AB4.x = 0.0f;
    this->unk_0AB4.y = 0.0f;
    this->unk_0AB4.z = 0.0f;
    this->unk_0AC0.x = 0.0f;
    this->unk_0AC0.y = 0.0f;
    this->unk_0AC0.z = 0.0f;
}

// OFFSET: 0x008E48F0/0x1406126B0
int32_t CGWorldFrame::SetupDefaultAction() {
    NDCToDDC(this->m_simpleTop->m_mousePosition.m_x, this->m_simpleTop->m_mousePosition.m_y, &this->m_defaultActionPointDDC.x, &this->m_defaultActionPointDDC.y);
    this->m_defaultActionHitKind = this->HitTestPoint(this->m_defaultActionPointDDC.x, this->m_defaultActionPointDDC.y, 0, &this->m_defaultActionHit);
    return this->m_defaultActionHitKind;
}

// OFFSET: 0x008E4950/0x140612720
void CGWorldFrame::OnWorldRender() {
    CRect window = { 0.0f, 0.0f, 0.0f, 0.0f };
    g_theGxDevicePtr->CapsWindowSize(window);

    if (window.maxX - window.minX == 0.0f || window.maxY - window.minY == 0.0f) {
        return;
    }

    if (!(this->m_viewportNDC.maxY > this->m_viewportNDC.minY && this->m_viewportNDC.maxX > this->m_viewportNDC.minX)) {
        return;
    }

    CCharacterComponent::ResetCounts();
    CAnimKitManager::UpdateAll(static_cast<int32_t>(this->m_elapsedSec * 1000.0f));
    g_theGxDevicePtr->RsPush();
    GxRsSet(GxRs_Multisample, 1);
    CGWorldFrame::UpdateScreenEffectParams();
    FFX::Start();

    CRect fullViewport = { 0.0f, 0.0f, 1.0f, 1.0f };

    if (this->m_viewportNDC != fullViewport) {
        CImVector black(0x00, 0x00, 0x00, 0xFF);
        GxSceneClear(31, black);
    }

    float minX = 0.0f;
    float minY = 0.0f;
    float minZ = 0.0f;
    float maxX = 0.0f;
    float maxY = 0.0f;
    float maxZ = 0.0f;
    GxXformViewport(minX, maxX, minY, maxY, minZ, maxZ);

    int32_t offscreen = g_theGxDevicePtr->FrameBufferGet() != g_theGxDevicePtr->BackBuffer();

    float width = maxX - minX;
    float height = maxY - minY;
    float top;
    float bottom;

    if (offscreen) {
        bottom = 1.0f - this->m_viewportNDC.maxY;
        top = 1.0f - this->m_viewportNDC.minY;
    } else {
        bottom = this->m_viewportNDC.minY;
        top = this->m_viewportNDC.maxY;
    }

    GxXformSetViewport(this->m_viewportNDC.minX * width + minX, this->m_viewportNDC.maxX * width + minX, bottom * height + minY, top * height + minY, minZ, maxZ);
    World::Render(StaticSingleton<CCameraManager>::Instance()->m_shot.m_position, this->m_elapsedSec);
    FFX::DoPass(1);
    FFX::Finish();
    PlayerNameUpdateLate();
    g_theGxDevicePtr->RsPop();
    SmartScreenRectClearGrid(1);
    CGChatBubbleFrame::OnWorldRender(this);
    GxXformSetViewport(minX, maxX, minY, maxY, minZ, maxZ);
    CVehiclePassenger_C::PostRenderUpdateAll();
    CursorReleaseItemMips();
}

// OFFSET: 0x008E4C50/0x140612A40
void CGWorldFrame::RenderTransparentCallback(uint32_t pass, uint64_t param64) {
    GxXformSet(GxXform_World, C44Matrix());

    if (s_spellShadowStyle < 2) {
        DrawSpellCursorShadow(0x30000);
    }

    SpellVisualsRender();
    CGUnit_C::RenderMissileTrajectoryTargetingTexture();
    this->ClearTrackedObjects();
}

// OFFSET: 0x008E4DA0/0x140612BE0
int32_t CGWorldFrame::ObjectEnumProc(void* param, uintptr_t flags, uint64_t guid, uintptr_t a4) {
    CGWorldFrame* frame = CGWorldFrame::s_currentWorldFrame;
    CGObject_C* object = ClntObjMgrObjectPtr(WOWGUID::Make(guid), TYPEMASK_OBJECT, __FILE__, __LINE__);

    if (!object || (object->m_modelFlags & 0x100000)) {
        return 1;
    }

    if (flags & 4) {
        object->m_modelFlags |= 0x800000;
    } else {
        object->m_modelFlags &= ~0x800000;
    }

    if (flags & 1) {
        frame->UpdateObject(object);
    }

    return 1;
}

// OFFSET: 0x008E4E10
void CGWorldFrame::RenderWorld(void* param) {
    CGWorldFrame* worldFrame = static_cast<CGWorldFrame*>(param);
    C44Matrix projection;
    C44Matrix view;
    C44Matrix world;
    projection = g_theGxDevicePtr->m_projection;
    GxXformView(view);
    GxXform(GxXform_World, world);

    worldFrame->OnWorldUpdate();
    PlayerNameUpdateWorldText();
    worldFrame->OnWorldRender();
    WorldTextRenderBatch();

    g_theGxDevicePtr->XformSetProjection(projection);
    g_theGxDevicePtr->XformSetView(view);
    GxXformSet(GxXform_World, world);
    CShaderEffect::UpdateProjMatrix();
}

// OFFSET: 0x008E5010
void CGWorldFrame::RenderOpaqueCallback(void* param, uint32_t pass, uint64_t param64) {
    if (s_spellShadowStyle < 2) {
        DrawSpellCursorShadow(0x200122);
    }

    if (!CGUnit_C::s_activeMover) {
        return;
    }

    CGUnit_C* mover = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(CGUnit_C::s_activeMover, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (mover) {
        mover->RenderAutoTrackCursor();
    }
}

// OFFSET: 0x008E5060
void CGWorldFrame::RenderTransparentCallback(void* param, uint32_t pass, uint64_t param64) {
    static_cast<CGWorldFrame*>(param)->RenderTransparentCallback(pass, param64);
}

// OFFSET: 0x008E5080/0x140613240
CGWorldFrame::CGWorldFrame(CSimpleFrame* parent) : CSimpleFrame(parent) {
    this->unk_0214 = 1;
    this->unk_0218 = 1;
    this->m_trackedGuid = WOWGUID();
    this->m_hitGuid = WOWGUID();
    this->m_defaultActionHit.m_guid = WOWGUID();
    this->m_defaultActionHit.m_point = C3Vector(0.0f, 0.0f, 0.0f);
    this->m_defaultActionHit.m_distance = 0.0f;
    this->m_defaultActionHit.m_segStart = C3Vector(0.0f, 0.0f, 0.0f);
    this->m_defaultActionHit.m_segEnd = C3Vector(0.0f, 0.0f, 0.0f);
    this->m_defaultActionPointDDC = C2Vector(0.0f, 0.0f);
    this->m_worldFlags = (this->m_worldFlags & ~0x3) | 0x1;
    this->m_viewportDDC.minY = 0.0f;
    this->m_viewportDDC.minX = 0.0f;
    this->m_viewportDDC.maxY = 0.0f;
    this->m_viewportDDC.maxX = 0.0f;
    this->m_viewportNDC.minY = 0.0f;
    this->m_viewportNDC.minX = 0.0f;
    this->m_viewportNDC.maxY = 0.0f;
    this->m_viewportNDC.maxX = 0.0f;
    this->m_namePlateFlags = 0;

    CGWorldFrame::s_currentWorldFrame = this;

    this->SetFrameStrata(FRAME_STRATA_WORLD);
    this->EnableEvent(SIMPLE_EVENT_KEY, -1);
    this->EnableEvent(SIMPLE_EVENT_MOUSE, -1);
    this->EnableEvent(SIMPLE_EVENT_MOUSEWHEEL, -1);

    memset(this->m_keyDown, 0, sizeof(this->m_keyDown));
    memset(this->m_mouseDown, 0, sizeof(this->m_mouseDown));

    this->m_camera = new (__FILE__, __LINE__) CGCamera();
    this->unk_0214 = 5;

    CStatus status;

    for (uint32_t i = 0; i < 2; i++) {
        s_spellShadowTexture[i] = TextureCreate(s_spellShadowTextureNames[i], CGxTexFlags(), &status, 1, NULL, 2);
        SysMsgAdd(&status);
    }

    EventRegisterEx(EVENT_ID_IDLE, CGWorldFrame::Update, this, -1000.0f);
    FFX::Init();

    CGWorldFrame::ffxEffectGlow = new (__FILE__, __LINE__) EffectGlow();
    CGWorldFrame::ffxEffectDeath = new (__FILE__, __LINE__) EffectDeath();
    CGWorldFrame::ffxEffectNetherWorld = new (__FILE__, __LINE__) EffectNetherWorld();
    CGWorldFrame::ffxEffectSwirlingFog = new (__FILE__, __LINE__) EffectSwirlingFog();
    CGWorldFrame::ffxEffectUnconscious = new (__FILE__, __LINE__) EffectUnconscious();
    CGWorldFrame::ffxEffectUnderwater = NULL;

    CGWorldFrame::WaterReset();
    CGWorldFrame::UpdateScreenEffect();
    World::AddRenderCallback(0, static_cast<WORLD_RENDER_CALLBACK>(&CGWorldFrame::RenderPostAnimateCallback), this, 0);
    World::AddRenderCallback(1, CGWorldFrame::RenderOpaqueCallback, this, 0);
    World::AddRenderCallback(2, static_cast<WORLD_RENDER_CALLBACK>(&CGWorldFrame::RenderTransparentCallback), this, 0);
    g_theGxDevicePtr->AddDeviceRestoredCallback(reinterpret_cast<GxDeviceCallback>(&CGWorldFrame::WaterResetCallback));
    Liquid::AddConfigChangedCallback(CGWorldFrame::WaterResetCallback);
}

// OFFSET: 0x008E54E0/0x140613720
CGWorldFrame::~CGWorldFrame() {
    EventUnregisterEx(EVENT_ID_IDLE, CGWorldFrame::Update, this, 0xFFFFFFFF);
    EventSetMouseMode(MOUSE_MODE_NORMAL, 0);
    CGWorldFrame::s_currentWorldFrame = NULL;

    for (uint32_t i = 0; i < 2; i++) {
        if (s_spellShadowTexture[i]) {
            TextureRelease(s_spellShadowTexture[i]);
        }

        s_spellShadowTexture[i] = NULL;
    }

    SmartScreenRectShutdown();
    Spell_C_ClearInTransitList();
    this->m_modelList.Clear();
    this->m_hitModelList.Clear();
    this->m_freeModelList.Clear();
    delete this->m_camera;
    this->m_camera = NULL;

    if (CGWorldFrame::ffxEffectGlow) {
        delete CGWorldFrame::ffxEffectGlow;
    }

    if (CGWorldFrame::ffxEffectDeath) {
        delete CGWorldFrame::ffxEffectDeath;
    }

    if (CGWorldFrame::ffxEffectNetherWorld) {
        delete CGWorldFrame::ffxEffectNetherWorld;
    }

    if (CGWorldFrame::ffxEffectSwirlingFog) {
        delete CGWorldFrame::ffxEffectSwirlingFog;
    }

    if (CGWorldFrame::ffxEffectUnderwater) {
        delete CGWorldFrame::ffxEffectUnderwater;
    }

    if (CGWorldFrame::ffxEffectUnconscious) {
        delete CGWorldFrame::ffxEffectUnconscious;
    }

    FFX::Destroy();
    World::RemoveRenderCallback(0, static_cast<WORLD_RENDER_CALLBACK>(&CGWorldFrame::RenderPostAnimateCallback), this);
    World::RemoveRenderCallback(1, CGWorldFrame::RenderOpaqueCallback, this);
    World::RemoveRenderCallback(2, static_cast<WORLD_RENDER_CALLBACK>(&CGWorldFrame::RenderTransparentCallback), this);
    g_theGxDevicePtr->RemoveDeviceRestoredCallback(reinterpret_cast<GxDeviceCallback>(&CGWorldFrame::WaterResetCallback));
    Liquid::RemoveConfigChangedCallback(CGWorldFrame::WaterResetCallback);
}

// PROVEN: 072ef2d8 IDENTICAL/NONE
// OFFSET: 0x008E5740
void CGWorldFrame::OnFrameRender(CRenderBatch* batch, uint32_t layer) {
    this->CSimpleFrame::OnFrameRender(batch, layer);

    if (!layer) {
        batch->QueueCallback(CGWorldFrame::RenderWorld, this);
    }
}

// OFFSET: 0x008C13F0
CSimpleFrame* CGWorldFrame::Create(CSimpleFrame* parent) {
    void* data = CGWorldFrame::s_worldFrameHeap.GetData(0, typeid(CGWorldFrame).raw_name(), SLOG_OBJECT);
    return data ? new (data) CGWorldFrame(parent) : NULL;
}

// INLINED
void CGWorldFrame::operator delete(void* ptr) {
    CGWorldFrame::s_worldFrameHeap.PutData(ptr, NULL, 0);
}

// OFFSET: 0x008CB080/0x14063B870
void CGWorldFrame::RegisterFrames() {
    FrameXML_RegisterFactory("WorldFrame", CGWorldFrame::Create, true);
    FrameXML_RegisterFactory("GameTooltip", CGTooltip::Create, false);
    FrameXML_RegisterFactory("Cooldown", CGCooldown::Create, false);
    FrameXML_RegisterFactory("Minimap", CGMinimapFrame::Create, false);
    FrameXML_RegisterFactory("PlayerModel", CGCharacterModelBase::Create, false);
    FrameXML_RegisterFactory("DressUpModel", CGDressUpModelFrame::Create, false);
    FrameXML_RegisterFactory("TabardModel", CGTabardModelFrame::Create, false);
    FrameXML_RegisterFactory("BlobFrame", CGBlobFrame::Create, false);
    FrameXML_RegisterFactory("QuestPOIFrame", CGQuestPOIFrame::Create, false);
    FrameXML_RegisterFactory("ArchaeologyDigSiteFrame", CGArchaeologyDigSiteFrame::Create, false);
}

// OFFSET: #0x004F5910
int32_t CGWorldFrame::MissileEnumProc(void* param, uint32_t flags, uint64_t guid, uintptr_t a4) {
    CMissile* missile = reinterpret_cast<CMissile*>(a4);
    return !missile || missile->m_mapEntity != NULL;
}

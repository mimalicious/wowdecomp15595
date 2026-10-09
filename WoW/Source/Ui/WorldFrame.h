#ifndef WOW_SOURCE_UI_WORLDFRAME_H
#define WOW_SOURCE_UI_WORLDFRAME_H

#include <cstddef>
#include <cstdint>
#include <limits>
#include "Engine/Source/Base/CDataAllocator.h"
#include "Engine/Source/Frame/CSimpleFrame.h"
#include "Engine/Source/Model2/M2Model.h"
#include "Engine/Source/Tempest/C2Vector.h"
#include "Engine/Source/Tempest/C3Vector.h"
#include "Engine/Source/Tempest/C44Matrix.h"
#include "Engine/Source/Storm/H/stpl.h"
#include "Engine/Source/Event/Types.h"
#include "WoW/Source/Object/WGUID.h"
#include "Engine/Source/Tempest/CRect.h"
#include "WoW/Source/WorldClient/World.h"

class CGGameObject_C;
class CGUnit_C;
class CGCorpse_C;
class CGCamera;
class CGObject_C;
class CKeyEvent;
class CMouseEvent;
class CRenderBatch;

namespace FFX {
    class Effect;
}

class CModelRecord : public TSLinkedNode<CModelRecord> {
    public:
    /* 0008 */ CM2Model* m_model;
    /* 000C */ float m_distance;
    /* 0010 */ WOWGUID m_guid;
    /* 0018 */ CGObject_C* m_object;
    /* 001C */ uint32_t unk_001C;

    CModelRecord();

    ~CModelRecord();
};

struct HITTESTRESULT {
    /* 0000 */ WOWGUID m_guid;
    /* 0008 */ C3Vector m_point;
    /* 0014 */ float m_distance;
    /* 0018 */ C3Vector m_segStart;
    /* 0024 */ C3Vector m_segEnd;
};

struct KEYDOWNSTATE {
    /* 0000 */ char m_keyString [32];
    /* 0020 */ uint32_t m_modifiers;
};

class CGWorldFrame : public CSimpleFrame {
    public:
    static CDataAllocator s_worldFrameHeap;
    static FFX::Effect* ffxEffectSwirlingFog;
    static FFX::Effect* ffxEffectNetherWorld;
    static FFX::Effect* ffxEffectGlow;
    static FFX::Effect* ffxEffectDeath;
    static FFX::Effect* ffxEffectUnderwater;
    static FFX::Effect* ffxEffectUnconscious;
    static CGWorldFrame* s_currentWorldFrame;

    static CSimpleFrame* Create(CSimpleFrame* parent);
    static CGCamera* GetActiveCamera();
    static void GetCameraFacing(C3Vector* facing);
    static void GetCameraPosition(C3Vector* position);
    static void RegisterFrames();
    static void SetScreenEffect(int32_t effect);
    static void UpdateScreenEffect();
    static void UpdateScreenEffectParams();
    static void UpdatePlayerAmbientMultiplier();
    static void WaterReset();
    static void WaterResetCallback();
    static int32_t Update(const void* data, void* param);
    static void RenderWorld(void* param);
    static void RenderPostAnimateCallback(void* param, uint32_t pass, uint64_t param64);
    static void RenderOpaqueCallback(void* param, uint32_t pass, uint64_t param64);
    static void RenderTransparentCallback(void* param, uint32_t pass, uint64_t param64);
    static int32_t ObjectEnumProc(void* param, uintptr_t flags, uint64_t guid, uintptr_t a4);
    static int32_t MissileEnumProc(void* param, uint32_t flags, uint64_t guid, uintptr_t a4);
    static int32_t SpellEffectEnumProc(void* param, uintptr_t flags, uint64_t guid, uintptr_t a4);

    /* 01F0 */ TSList<CModelRecord, TSGetLink<CModelRecord> > m_modelList;
    /* 01FC */ TSList<CModelRecord, TSGetLink<CModelRecord> > m_hitModelList;
    /* 0208 */ TSList<CModelRecord, TSGetLink<CModelRecord> > m_freeModelList;
    /* 0214 */ uint32_t unk_0214;
    /* 0218 */ uint32_t unk_0218;
    /* 021C */ uint32_t unk_021C;
    /* 0220 */ WOWGUID m_trackedGuid;
    /* 0228 */ WOWGUID m_hitGuid;
    /* 0230 */ uint32_t m_defaultActionHitKind;
    /* 0234 */ uint32_t unk_0234;
    /* 0238 */ HITTESTRESULT m_defaultActionHit;
    /* 0268 */ C2Vector m_defaultActionPointDDC;
    /* 0270 */ uint32_t unk_0270;
    /* 0274 */ uint32_t m_worldFlags;
    /* 0278 */ CRect m_viewportDDC;
    /* 0288 */ CRect m_viewportNDC;
    /* 0298 */ C44Matrix m_viewProjection;
    /* 02D8 */ WOWGUID m_trackedEffectGuidA;
    /* 02E0 */ WOWGUID m_trackedEffectGuidB;
    /* 02E8 */ World::HitTestDebug m_hitTestDebug;
    /* 0DC0 */ uint32_t m_namePlateFlags;
    /* 0DC4 */ float m_elapsedSec;
    /* 0DC8 */ KEYDOWNSTATE m_keyDown[787];
    /* 7C74 */ KEYDOWNSTATE m_mouseDown[31];
    /* 80D0 */ CGCamera* m_camera;

    CGWorldFrame(CSimpleFrame* parent);
    ~CGWorldFrame();
    void OnLayerUpdate(float elapsedSec) override;
    int32_t OnLayerTrackUpdate(const CMouseEvent& evt) override;
    void OnFrameRender(CRenderBatch* batch, uint32_t layer) override;
    void OnFrameSizeChanged(const CRect& rect) override;
    void OnLayerCursorExit(int32_t a2, int32_t a3) override;
    int32_t OnLayerKeyDown(const CKeyEvent& evt) override;
    int32_t OnLayerKeyUp(const CKeyEvent& evt) override;
    int32_t OnLayerMouseDown(const CMouseEvent& evt, const char* btn) override;
    int32_t OnLayerMouseUp(const CMouseEvent& evt, const char* btn) override;
    int32_t OnLayerMouseWheel(const CMouseEvent& evt) override;
    void operator delete(void* ptr);
    int32_t SetupDefaultAction();
    uint32_t GetSceneTime() const;
    void AddObjectToHitTestList(CModelRecord* record, uint32_t flags);
    void UpdateObject(CGObject_C* object);
    void ClearTrackedObjects();
    int32_t GetLineSegment(float mouseX, float mouseY, C3Vector* start, C3Vector* end);
    bool GetScreenCoordinates(const C3Vector& world, C3Vector* screen, uint32_t* result, bool* behind = NULL);
    void MoveToFreeList(TSList<CModelRecord, TSGetLink<CModelRecord> >* list);
    void OnMouseModeNormal();
    void OnMouseModeRelative();
    void PerformDefaultAction(MOUSEBUTTON button);
    void OnWorldRender();
    void OnWorldUpdate();
    void UpdateWorld();
    void UpdateDayNight(float deltaSec);
    void RenderPostAnimateCallback(uint32_t pass, uint64_t param64);
    void RenderTransparentCallback(uint32_t pass, uint64_t param64);
    int32_t HandleSpriteTrack(WOWGUID guid);
    int32_t HitTestPoint(float x, float y, int32_t a3, HITTESTRESULT* result);
    int32_t CanTargetCorpseSelection(const CGCorpse_C* corpse, uint32_t flags);
    static void __stdcall CursorTrackObject(CGGameObject_C* object);
    static void __stdcall CursorTrackUnit(CGUnit_C* unit);
    static void __stdcall CursorTrackCorpse(CGCorpse_C* corpse);
    int32_t IsUnitLegalSelection(CGUnit_C* unit, uint32_t flags);
    int32_t IsLegalSelection(CGObject_C* object, uint32_t flags);
    uint32_t GetHitTestFilterFlags(int32_t a1);
    uint64_t FindClosestModel(const C3Vector& start, const C3Vector& end, uint32_t flags, float* distance);
    int32_t HitTest(const C3Vector& start, const C3Vector& end, uint32_t flags, HITTESTRESULT* result);
    void OnLayerTrackObject(const HITTESTRESULT& result);
    void OnLayerTrackTerrain(const HITTESTRESULT& result);
    void SetNamePlateUpdate();
    void SetPlayerFadeCameraValue(uint8_t value);
    void UnitClear();
    void SetCameraTarget(CGObject_C* target, int32_t freeLook);
};

class CWorldClickEvent {
    public:
    /* 0000 */ C3Vector m_segStart;
    /* 000C */ C3Vector m_segEnd;
    /* 0018 */ MOUSEBUTTON m_button;
};

#include "WoW/Source/Ui/WorldFrame.inl"

#endif

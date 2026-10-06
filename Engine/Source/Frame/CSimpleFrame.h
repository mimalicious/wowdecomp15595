#ifndef ENGINE_SOURCE_FRAME_CSIMPLEFRAME_H
#define ENGINE_SOURCE_FRAME_CSIMPLEFRAME_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Base/CDataAllocator.h"
#include "Engine/Source/Frame/CScriptRegion.h"
#include "Engine/Source/Frame/CRenderBatch.h"
#include "Engine/Source/Frame/CSimpleRegion.h"
#include "Engine/Source/Frame/CSimpleAnim.h"
#include "Engine/Source/Frame/Types.h"
#include "Engine/Source/FrameXML/XMLTree.h"
#include "Engine/Source/Tempest/C2Vector.h"
#include "Engine/Source/Storm/H/stpl.h"
#include "Engine/Source/Tempest/CRect.h"

class CBackdropGenerator;
class CCharEvent;
class CImeEvent;
class CKeyEvent;
class CMouseEvent;
class CSimpleFrame;
class CStatus;
class linkObject;
class CSimpleTop;
struct SIMPLEFRAMENODE;
struct lua_State;

struct FRAMEATTR : public TSHashObject<FRAMEATTR, HASHKEY_STRI> {
    /* 0018 */ int32_t m_luaRef;
};

class CSimpleFrame : public CScriptRegion {
    public:
    static CDataAllocator s_allocator;
    static bool s_defaultPreload;
    static int32_t s_frameCount;
    static int32_t s_metatable;
    static int32_t s_objectType;
    static const char* s_objectTypeName;

    static void operator delete(void* ptr);
    static void CreateScriptMetaTable();
    static void DestroyScriptMetaTable();
    static int32_t GetObjectType();
    static void RegisterScriptMethods(lua_State* L);

    /* 00A4 */ CSimpleTop* m_simpleTop;
    /* 00A8 */ CSimpleFrame* m_hoverFrame;
    /* 00AC */ CScriptRegion* m_titleRegion;
    /* 00B0 */ CBackdropGenerator* m_backdropGenerator;
    /* 00B4 */ CRenderBatch* m_renderBatch [5];
    /* 00C8 */ int32_t m_id;
    /* 00CC */ uint32_t m_frameFlags;
    /* 00D0 */ float m_frameScale;
    /* 00D4 */ float m_depth;
    /* 00D8 */ float m_parentDepth;
    /* 00DC */ int32_t m_level;
    /* 00E0 */ uint32_t m_batchDirty;
    /* 00E4 */ uint32_t m_enabledEventsBitmask;
    /* 00E8 */ uint32_t m_dragButtons;
    /* 00EC */ uint32_t m_mouseDragButton;
    /* 00F0 */ C2Vector m_mousePosition;
    /* 00F8 */ CRect m_hitRect;
    /* 0108 */ CRect m_hitRectInsets;
    /* 0118 */ CRect m_clampRectInsets;
    /* 0128 */ FrameScript_Object::ScriptIx m_onLoad;
    /* 0129 */ FrameScript_Object::ScriptIx m_onSizeChanged;
    /* 012A */ FrameScript_Object::ScriptIx m_onUpdate;
    /* 012B */ FrameScript_Object::ScriptIx m_onShow;
    /* 012C */ FrameScript_Object::ScriptIx m_onHide;
    /* 012D */ FrameScript_Object::ScriptIx m_onCursorEnter;
    /* 012E */ FrameScript_Object::ScriptIx m_onCursorExit;
    /* 012F */ FrameScript_Object::ScriptIx m_onMouseDown;
    /* 0130 */ FrameScript_Object::ScriptIx m_onMouseUp;
    /* 0131 */ FrameScript_Object::ScriptIx m_onMouseWheel;
    /* 0132 */ FrameScript_Object::ScriptIx m_onDragStart;
    /* 0133 */ FrameScript_Object::ScriptIx m_onDragStop;
    /* 0134 */ FrameScript_Object::ScriptIx m_onReceiveDrag;
    /* 0135 */ FrameScript_Object::ScriptIx m_onChar;
    /* 0136 */ FrameScript_Object::ScriptIx m_onKeyDown;
    /* 0137 */ FrameScript_Object::ScriptIx m_onKeyUp;
    /* 0138 */ FrameScript_Object::ScriptIx m_onAttributeChanged;
    /* 0139 */ FrameScript_Object::ScriptIx m_onEnable;
    /* 013A */ FrameScript_Object::ScriptIx m_onDisable;
    /* 013C */ TSHashTable<FRAMEATTR, HASHKEY_STRI> m_attributes;
    /* 0168 */ TSExplicitList<CSimpleRegion, offsetof(CSimpleRegion, m_regionLink)> m_regions;
    /* 0174 */ TSExplicitList<CSimpleRegion, offsetof(CSimpleRegion, m_layerLink)> m_drawLayers [5];
    /* 01B0 */ TSExplicitList<CRenderBatch, offsetof(CRenderBatch, m_renderLink)> m_renderList;
    /* 01BC */ TSList<SIMPLEFRAMENODE> m_children;
    /* 01C8 */ TSLink<CSimpleFrame> m_framesLink;
    /* 01D0 */ TSLink<CSimpleFrame> m_destroyedLink;
    /* 01D8 */ TSLink<CSimpleFrame> m_strataLink;
    /* 01E0 */ TSLink<CSimpleFrame> m_loadLink;
    /* 01E8 */ uint32_t m_alpha : 8;
    /* 01E8 */ uint32_t m_parentAlpha : 8;
    /* 01E8 */ uint32_t m_drawLayerEnabled : 5;
    /* 01E8 */ uint32_t m_frameState : 2;
    /* 01E8 */ uint32_t m_isMouseDragging : 1;
    /* 01E8 */ uint32_t m_strata : 4;
    /* 01E8 */ uint32_t m_ignoreDepth : 1;
    /* 01E8 */ uint32_t m_highlightLocked : 1;
    /* 01E8 */ uint32_t m_isMouseDown : 1;
    /* 01E8 */ uint32_t m_isLoading : 1;
    /* 01EC */ uint32_t m_forceAlpha : 1;

    virtual ~CSimpleFrame();
    virtual int32_t GetScriptMetaTable();
    virtual FrameScript_Object::ScriptIx* GetScriptByName(const char* name, FrameScript_Object::ScriptData& data);
    virtual bool IsA(int32_t type);
    virtual bool IsA(const char* typeName);
    virtual const char* GetObjectTypeName();
    virtual void SetParent(CSimpleFrame* parent);
    virtual bool IsDragging();
    virtual void PreOnAnimUpdate();
    virtual void StopAnimating();
    virtual void AnimActivated(CSimpleAnimGroup* group, int32_t alpha, int32_t position);
    virtual void AnimDeactivated(CSimpleAnimGroup* group, int32_t alpha, int32_t position);
    virtual void OnLayerUpdate(float elapsedSec);
    virtual void AddAnimTranslation(CScriptRegion* region, const C2Vector& translation);
    virtual void AddAnimRotation(CScriptRegion* region, FRAMEPOINT origin, const C2Vector& offset, float radians);
    virtual void AddAnimScale(CScriptRegion* region, FRAMEPOINT origin, const C2Vector& offset, const C2Vector& scale);
    virtual void AddAnimAlpha(CScriptRegion* region, int16_t alpha);
    virtual void PreLoadXML(linkObject* node, CStatus* status);
    virtual void GetScriptTime(lua_State* L, double* time, uint32_t* count, int32_t recurse);
    virtual void PostLoadXML(linkObject* node, CStatus* status);
    virtual void UnregisterRegion(CSimpleRegion* region);
    virtual bool GetBoundsRect(CRect& rect);
    virtual void OnLayerShow();
    virtual void OnLayerHide();
    virtual int32_t OnLayerTrackUpdate(const CMouseEvent& evt);
    virtual void OnFrameRender(CRenderBatch* batch, uint32_t layer);
    virtual void OnFrameRender();
    virtual void OnScreenSizeChanged();
    virtual void OnFrameSizeChanged(float width, float height);
    virtual void OnLayerCursorEnter(int32_t a2);
    virtual void OnLayerCursorExit(int32_t a2, int32_t a3);
    virtual int32_t OnLayerIme(const CImeEvent& evt);
    virtual int32_t OnLayerKeyDownRepeat(const CKeyEvent& evt);
    virtual int32_t OnLayerChar(const CCharEvent& evt);
    virtual int32_t OnLayerKeyDown(const CKeyEvent& evt);
    virtual int32_t OnLayerKeyUp(const CKeyEvent& evt);
    virtual int32_t OnLayerMouseDown(const CMouseEvent& evt, const char* btn);
    virtual int32_t OnLayerMouseUp(const CMouseEvent& evt, const char* btn);
    virtual int32_t OnLayerMouseWheel(const CMouseEvent& evt);
    virtual void OnDragStart(const CMouseEvent& evt);
    virtual void OnDragStop(const CMouseEvent& evt);
    virtual void OnReceiveDrag(const CMouseEvent& evt);
    virtual void LockHighlight(int32_t lock);
    virtual bool UpdateScale(bool force);
    virtual void UpdateAlpha();
    virtual bool UpdateDepth(bool force);
    virtual void ParentFrame(CSimpleFrame* child);
    virtual void UnparentFrame(CSimpleFrame* child);
    virtual void LoadXML(linkObject* node, CStatus* status);
    virtual void PropagateProtectFlagToParent(uint32_t flag);
    virtual int32_t AreChildrenProtected(int32_t& deferred);
    virtual void GetClampRectInsets(float& left, float& right, float& top, float& bottom);
    virtual int32_t IsObjectLoaded();
    virtual void DumpInfo(int32_t depth, void (*print)(char*, void*), void* param);
    virtual void HideThis();
    virtual void ShowThis(bool loadPending);
    virtual void OnFrameSizeChanged(const CRect& rect);

    CSimpleFrame(CSimpleFrame* parent);
    void AddFrameRegion(CSimpleRegion* region, uint32_t drawLayer);
    void AllowAttributeChanges(int32_t allow);
    int32_t AttributeChangesAllowed();
    void DisableDrawLayer(uint32_t drawLayer);
    void DisableEvent(CSimpleEventType eventType);
    int32_t DragBy(float offsetX, float offsetY, FRAMEPOINT point);
    void EnableDrawLayer(uint32_t drawLayer);
    void EnableEvent(CSimpleEventType eventType, int32_t priority);
    bool GetAttribute(const char* name, int32_t& luaRef);
    uint8_t GetEffectiveAlpha();
    int32_t GetHitRect(CRect& rect);
    void GetHitRectInsets(float& left, float& right, float& top, float& bottom);
    void Hide();
    void LoadXML_Attributes(linkObject* node, CStatus* status);
    void LoadXML_Layers(linkObject* node, CStatus* status);
    void LoadXML_Scripts(linkObject* node, CStatus* status);
    void MarkDirty();
    int32_t Lower();
    void NotifyDelayedLoad();
    void NotifyDrawLayerChanged(uint32_t drawLayer);
    void NotifyScrollParent();
    void Raise();
    void RegisterForEvents(int32_t a2);
    void RegisterRegion(CSimpleRegion* region);
    void RemoveFrameRegion(CSimpleRegion* region, uint32_t drawLayer);
    void RunOnAttributeChangedScript(const char* name, int32_t luaRef);
    void RunOnCharScript(const char* text);
    void RunOnEnableScript(int32_t disabled);
    void RunOnEnterScript(int32_t motion);
    void RunOnKeyDownScript(const char* key);
    void RunOnKeyUpScript(const char* key);
    void RunOnLeaveScript(int32_t motion);
    void RunOnMouseWheelScript(int32_t delta);
    void RunOnSizeChangedScript(float width, float height);
    void SetAttribute(const char* name, int32_t luaRef);
    void SetBackdrop(CBackdropGenerator* backdrop);
    void SetBeingScrolled(int32_t scrolled, int32_t a3);
    void SetClampRectInsets(float left, float right, float top, float bottom);
    void SetDontReturnDepth(int32_t dontReturn);
    void SetForceAlpha(bool force);
    void SetFrameAlpha(uint8_t alpha);
    bool SetFrameDepth(float depth, bool force);
    void SetFrameFlag(uint32_t flag, int32_t set);
    void SetFrameLevel(int32_t level, int32_t shiftChildren);
    bool SetFrameScale(float scale, bool force);
    void SetFrameStrata(FRAME_STRATA strata);
    void SetHitRectInsets(float left, float right, float top, float bottom);
    void SetTitleRegion(CScriptRegion* region);
    void SetUseParentLevel(bool use);
    void Show();
    void SignalLoaded();
    int32_t TestHitRect(const C2Vector& pt);
    void UnregisterForEvents(int32_t a2);
};

#include "Engine/Source/Frame/CSimpleFrame.inl"

#endif

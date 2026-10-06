#ifndef ENGINE_SOURCE_FRAME_CSIMPLEBUTTON_H
#define ENGINE_SOURCE_FRAME_CSIMPLEBUTTON_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Base/CDataAllocator.h"
#include "Engine/Source/Frame/CSimpleFrame.h"
#include "Engine/Source/Frame/Types.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"
#include "Engine/Source/Gx/Types.h"
#include "Engine/Source/Tempest/C2Vector.h"

class CImVector;
class CMouseEvent;
class CSimpleFont;
class CSimpleFontString;
class CSimpleRegion;
class CSimpleTexture;
class CStatus;
class linkObject;
struct lua_State;

class CSimpleButton : public CSimpleFrame {
    public:
    static CDataAllocator s_allocator;
    static int32_t s_metatable;
    static int32_t s_objectType;
    static const char* s_objectTypeName;

    static void operator delete(void* ptr);
    static void CreateScriptMetaTable();
    static void DestroyScriptMetaTable();
    static int32_t GetObjectType();
    static void RegisterScriptMethods(lua_State* L);

    /* 01F0 */ uint32_t m_lastClickTime;
    /* 01F8 */ uint64_t m_clickAction;
    /* 0200 */ CSimpleButtonState m_state : 4;
    /* 0200 */ uint32_t m_inClick : 1;
    /* 0200 */ uint32_t m_stateLocked : 1;
    /* 0200 */ int32_t m_motionScriptsWhileDisabled : 1;
    /* 0204 */ CSimpleFontString* m_fontString;
    /* 0208 */ CSimpleFont* m_normalFont;
    /* 020C */ CSimpleFont* m_highlightFont;
    /* 0210 */ CSimpleFont* m_disabledFont;
    /* 0214 */ C2Vector m_pushedTextOffset;
    /* 021C */ CSimpleTexture* m_stateTextures[3];
    /* 0228 */ CSimpleTexture* m_activeTexture;
    /* 022C */ CSimpleTexture* m_highlightTexture;
    /* 0230 */ FrameScript_Object::ScriptIx m_onPreClick;
    /* 0231 */ FrameScript_Object::ScriptIx m_onClick;
    /* 0232 */ FrameScript_Object::ScriptIx m_onPostClick;
    /* 0233 */ FrameScript_Object::ScriptIx m_onDoubleClick;

    virtual ~CSimpleButton();
    virtual int32_t GetScriptMetaTable();
    virtual FrameScript_Object::ScriptIx* GetScriptByName(const char* name, FrameScript_Object::ScriptData& data);
    virtual bool IsA(int32_t type);
    virtual bool IsA(const char* typeName);
    virtual const char* GetObjectTypeName();
    virtual void GetScriptTime(lua_State* L, double* time, uint32_t* count, int32_t recurse);
    virtual void UnregisterRegion(CSimpleRegion* region);
    virtual void OnLayerHide();
    virtual void OnLayerCursorEnter(int32_t a2);
    virtual void OnLayerCursorExit(int32_t a2, int32_t a3);
    virtual int32_t OnLayerMouseDown(const CMouseEvent& evt, const char* btn);
    virtual int32_t OnLayerMouseUp(const CMouseEvent& evt, const char* btn);
    virtual void OnDragStart(const CMouseEvent& evt);
    virtual void LockHighlight(int32_t lock);
    virtual void LoadXML(linkObject* node, CStatus* status);
    virtual void Enable(int32_t enabled);
    virtual void OnClick(const char* btn, int32_t down);
    virtual void OnDoubleClick(const char* btn);
    virtual void SetButtonState(CSimpleButtonState state, int32_t stateLocked);
    virtual void SetMotionScriptsWhileDisabled(int32_t enabled);
    virtual int32_t GetMotionScriptsWhileDisabled();

    CSimpleButton(CSimpleFrame* parent);
    void RunOnClickScript(const char* btn, int32_t down);
    void RunOnEnableScript(int32_t disabled);
    void RunOnDoubleClickScript(const char* btn);
    void SetClickAction(uint64_t action);
    void SetDisabledFontColor(const CImVector& color);
    void SetFontString(CSimpleFontString* fontString);
    int32_t SetHighlight(const char* fileName, EGxBlend blend);
    void SetHighlight(CSimpleTexture* texture, EGxBlend blend);
    void SetHighlightFontColor(const CImVector& color);
    void SetNormalFontColor(const CImVector& color);
    int32_t SetStateTexture(CSimpleButtonState state, const char* fileName);
    void SetStateTexture(CSimpleButtonState state, CSimpleTexture* texture);
    void SetText(const char* text);
    void UpdateTextState(CSimpleButtonState state);
};

const char* GetMouseButtonName(uint32_t button);
uint64_t MouseButtonStringToBitmask(const char* string);

#include "Engine/Source/Frame/CSimpleButton.inl"

#endif

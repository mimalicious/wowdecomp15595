#ifndef ENGINE_SOURCE_FRAME_CSIMPLEHYPERLINKEDFRAME_H
#define ENGINE_SOURCE_FRAME_CSIMPLEHYPERLINKEDFRAME_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Base/CDataAllocator.h"
#include "Engine/Source/Frame/CSimpleButton.h"
#include "Engine/Source/Frame/CSimpleFrame.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"
#include "Engine/Source/Gxu/Types.h"
#include "Engine/Source/Storm/H/stpl.h"

class CSimpleFontString;
class CSimpleHyperlinkedFrame;
struct lua_State;

class CSimpleHyperlinkButton : public CSimpleButton {
    public:
    static CDataAllocator s_allocator;
    static int32_t s_objectType;
    static const char* s_objectTypeName;

    static void operator delete(void* ptr);
    static int32_t GetObjectType();

    /* 0238 */ char* m_linkData;
    /* 023C */ char* m_linkText;
    /* 0240 */ TSLink<CSimpleHyperlinkButton> m_hyperlinkLink;

    virtual ~CSimpleHyperlinkButton();
    virtual bool IsA(int32_t type);
    virtual bool IsA(const char* typeName);
    virtual const char* GetObjectTypeName();
    virtual void OnLayerCursorEnter(int32_t a2);
    virtual void OnLayerCursorExit(int32_t a2, int32_t a3);
    virtual void OnClick(const char* btn, int32_t down);

    CSimpleHyperlinkButton(CSimpleHyperlinkedFrame* parent);
    void SetHyperlink(CSimpleFontString* fontString, const GXUFONTHYPERLINKINFO* info);
};

class CSimpleHyperlinkedFrame : public CSimpleFrame {
    public:
    /* 01F0 */ uint8_t m_inHyperlinkScript : 1;
    /* 01F0 */ uint8_t m_hyperlinksEnabled : 1;
    /* 01F0 */ uint8_t m_hyperlinksDirty : 1;
    /* 01F1 */ FrameScript_Object::ScriptIx m_onHyperlinkEnter;
    /* 01F2 */ FrameScript_Object::ScriptIx m_onHyperlinkLeave;
    /* 01F3 */ FrameScript_Object::ScriptIx m_onHyperlinkClick;
    /* 01F4 */ TSExplicitList<CSimpleHyperlinkButton, offsetof(CSimpleHyperlinkButton, m_hyperlinkLink)> m_hyperlinks;

    virtual ~CSimpleHyperlinkedFrame();
    virtual FrameScript_Object::ScriptIx* GetScriptByName(const char* name, FrameScript_Object::ScriptData& data);
    virtual void GetScriptTime(lua_State* L, double* time, uint32_t* count, int32_t recurse);
    virtual void OnHyperlinkEnter(const char* link, const char* text);
    virtual void OnHyperlinkLeave(const char* link, const char* text);
    virtual void OnHyperlinkClick(const char* link, const char* text, const char* btn);
    virtual void RefreshHyperlinks() = 0;

    static void RegisterScriptMethods(lua_State* L);

    CSimpleHyperlinkedFrame(CSimpleFrame* parent);
    void AddHyperlinks(CSimpleFontString* string);
    void ClearHyperlinks();
    void RunOnHyperlinkClickScript(const char* link, const char* text, const char* btn);
    void RunOnHyperlinkEnterScript(const char* link, const char* text);
    void RunOnHyperlinkLeaveScript(const char* link, const char* text);
};

#include "Engine/Source/Frame/CSimpleHyperlinkedFrame.inl"

#endif

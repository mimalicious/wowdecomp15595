#ifndef ENGINE_SOURCE_FRAME_CSIMPLESLIDER_H
#define ENGINE_SOURCE_FRAME_CSIMPLESLIDER_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Base/CDataAllocator.h"
#include "Engine/Source/Frame/CSimpleFrame.h"
#include "Engine/Source/Frame/Types.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"

class CMouseEvent;
class CRect;
class CSimpleRegion;
class CSimpleTexture;
class CStatus;
class linkObject;
struct lua_State;

class CSimpleSlider : public CSimpleFrame {
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

    /* 01F0 */ uint32_t m_changed : 1;
    /* 01F0 */ uint32_t m_rangeSet : 1;
    /* 01F0 */ uint32_t m_valueSet : 1;
    /* 01F0 */ uint32_t m_buttonDown : 1;
    /* 01F4 */ float m_baseValue;
    /* 01F8 */ float m_range;
    /* 01FC */ float m_value;
    /* 0200 */ float m_valueStep;
    /* 0204 */ CSimpleTexture* m_thumbTexture;
    /* 0208 */ ORIENTATION m_orientation;
    /* 020C */ FrameScript_Object::ScriptIx m_onValueChanged;
    /* 020D */ FrameScript_Object::ScriptIx m_onMinMaxChanged;

    virtual ~CSimpleSlider();
    virtual int32_t GetScriptMetaTable();
    virtual FrameScript_Object::ScriptIx* GetScriptByName(const char* name, FrameScript_Object::ScriptData& data);
    virtual bool IsA(int32_t type);
    virtual bool IsA(const char* typeName);
    virtual const char* GetObjectTypeName();
    virtual void GetScriptTime(lua_State* L, double* time, uint32_t* count, int32_t recurse);
    virtual void UnregisterRegion(CSimpleRegion* region);
    virtual void OnLayerHide();
    virtual void OnLayerUpdate(float elapsedSec);
    virtual int32_t OnLayerTrackUpdate(const CMouseEvent& evt);
    virtual int32_t OnLayerMouseDown(const CMouseEvent& evt, const char* btn);
    virtual int32_t OnLayerMouseUp(const CMouseEvent& evt, const char* btn);
    virtual void LoadXML(linkObject* node, CStatus* status);
    virtual void OnFrameSizeChanged(const CRect& rect);

    CSimpleSlider(CSimpleFrame* parent);
    void RunOnMinMaxChangedScript();
    void RunOnValueChangedScript(bool userInput);
    void SetMinMaxValues(float min, float max);
    void SetOrientation(ORIENTATION orientation);
    int32_t SetThumbTexture(const char* fileName);
    void SetThumbTexture(CSimpleTexture* texture, uint32_t drawLayer);
    void SetValue(float value, bool userInput);
    void SetValueStep(float step);
    float SnapValueToStep(float value);
};

#include "Engine/Source/Frame/CSimpleSlider.inl"

#endif

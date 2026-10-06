#ifndef ENGINE_SOURCE_FRAME_CSIMPLESTATUSBAR_H
#define ENGINE_SOURCE_FRAME_CSIMPLESTATUSBAR_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Base/CDataAllocator.h"
#include "Engine/Source/Frame/CSimpleFrame.h"
#include "Engine/Source/Frame/Types.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"

class CImVector;
class CSimpleRegion;
class CSimpleTexture;
class CStatus;
class linkObject;
struct lua_State;

class CSimpleStatusBar : public CSimpleFrame {
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
    /* 01F0 */ uint32_t m_rotatesTexture : 1;
    /* 01F4 */ float m_minValue;
    /* 01F8 */ float m_maxValue;
    /* 01FC */ float m_value;
    /* 0200 */ CSimpleTexture* m_barTexture;
    /* 0204 */ ORIENTATION m_orientation;
    /* 0208 */ FrameScript_Object::ScriptIx m_onValueChanged;
    /* 0209 */ FrameScript_Object::ScriptIx m_onMinMaxChanged;

    virtual ~CSimpleStatusBar();
    virtual int32_t GetScriptMetaTable();
    virtual FrameScript_Object::ScriptIx* GetScriptByName(const char* name, FrameScript_Object::ScriptData& data);
    virtual bool IsA(int32_t type);
    virtual bool IsA(const char* typeName);
    virtual const char* GetObjectTypeName();
    virtual void GetScriptTime(lua_State* L, double* time, uint32_t* count, int32_t recurse);
    virtual void UnregisterRegion(CSimpleRegion* region);
    virtual void OnLayerUpdate(float elapsedSec);
    virtual void LoadXML(linkObject* node, CStatus* status);
    virtual void SetWidth(float width);
    virtual void SetValue(float value);
    virtual void SetStatusBarColor(const CImVector& color);
    virtual float GetAnimValue() const;

    CSimpleStatusBar(CSimpleFrame* parent);
    void GetStatusBarColor(CImVector& color);
    void RunOnMinMaxChangedScript();
    void RunOnValueChangedScript();
    int32_t SetBarTexture(const char* fileName, uint32_t drawLayer, int8_t subLevel);
    void SetBarTexture(CSimpleTexture* texture, uint32_t drawLayer, int8_t subLevel);
    void SetMinMaxValues(float minValue, float maxValue);
    void SetOrientation(ORIENTATION orientation);
    void SetRotatesTexture(int32_t rotates);
};

#include "Engine/Source/Frame/CSimpleStatusBar.inl"

#endif

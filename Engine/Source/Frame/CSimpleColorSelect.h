#ifndef ENGINE_SOURCE_FRAME_CSIMPLECOLORSELECT_H
#define ENGINE_SOURCE_FRAME_CSIMPLECOLORSELECT_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Base/CDataAllocator.h"
#include "Engine/Source/Frame/CSimpleFrame.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"
#include "Engine/Source/Tempest/C3Vector.h"
#include "Engine/Source/Tempest/CImVector.h"

class CMouseEvent;
class CSimpleRegion;
class CSimpleTexture;
class CStatus;
class linkObject;
struct lua_State;

class CSimpleColorSelect : public CSimpleFrame {
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

    /* 01F0 */ bool m_wheelTracking;
    /* 01F1 */ bool m_valueTracking;
    /* 01F4 */ CSimpleTexture* m_wheelTexture;
    /* 01F8 */ CSimpleTexture* m_wheelThumbTexture;
    /* 01FC */ CSimpleTexture* m_valueTexture;
    /* 0200 */ CSimpleTexture* m_valueThumbTexture;
    /* 0204 */ C3Vector m_hsv;
    /* 0210 */ CImVector m_valueColor;
    /* 0214 */ FrameScript_Object::ScriptIx m_onColorSelect;

    virtual ~CSimpleColorSelect();
    virtual int32_t GetScriptMetaTable();
    virtual FrameScript_Object::ScriptIx* GetScriptByName(const char* name, FrameScript_Object::ScriptData& data);
    virtual bool IsA(int32_t type);
    virtual bool IsA(const char* typeName);
    virtual const char* GetObjectTypeName();
    virtual void GetScriptTime(lua_State* L, double* time, uint32_t* count, int32_t recurse);
    virtual void UnregisterRegion(CSimpleRegion* region);
    virtual void OnLayerHide();
    virtual int32_t OnLayerTrackUpdate(const CMouseEvent& evt);
    virtual int32_t OnLayerMouseDown(const CMouseEvent& evt, const char* btn);
    virtual int32_t OnLayerMouseUp(const CMouseEvent& evt, const char* btn);
    virtual void LoadXML(linkObject* node, CStatus* status);

    CSimpleColorSelect(CSimpleFrame* parent);
    void RepositionValueThumb();
    void RepositionWheelThumb();
    void RunOnColorSelectScript();
    int32_t SetColorValueThumbTexture(const char* fileName);
    void SetColorValueThumbTexture(CSimpleTexture* texture);
    void SetColorValueTexture(CSimpleTexture* texture);
    int32_t SetColorWheelThumbTexture(const char* fileName);
    void SetColorWheelThumbTexture(CSimpleTexture* texture);
    void SetColorWheelTexture(CSimpleTexture* texture);
    void UpdateColor();
    void UpdateValueTexture();
};

#include "Engine/Source/Frame/CSimpleColorSelect.inl"

#endif

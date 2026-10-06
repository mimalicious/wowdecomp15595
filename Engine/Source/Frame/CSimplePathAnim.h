#ifndef ENGINE_SOURCE_FRAME_CSIMPLEPATHANIM_H
#define ENGINE_SOURCE_FRAME_CSIMPLEPATHANIM_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Frame/CSimpleAnim.h"
#include "Engine/Source/Frame/CSimpleControlPoint.h"
#include "Engine/Source/Storm/H/stpl.h"

class CSimpleAnimGroup;
class CStatus;
class linkObject;

class CSimplePathAnim : public CSimpleAnim {
    public:
    static int32_t s_metatable;
    static int32_t s_objectType;
    static const char* s_objectTypeName;

    static int32_t GetObjectType();
    static void CreateScriptMetaTable();
    static void DestroyScriptMetaTable();
    static void RegisterScriptMethods(lua_State* L);

    /* 00B8 */ TSExplicitList<CSimpleControlPoint, offsetof(CSimpleControlPoint, m_link)> m_controlPoints;
    /* 00C4 */ TSGrowableArray<CSimpleControlPoint*> m_sortedPoints;
    /* 00D4 */ int8_t m_curve;

    virtual ~CSimplePathAnim();
    virtual int32_t GetScriptMetaTable();
    virtual bool IsA(int32_t type);
    virtual bool IsA(const char* typeName);
    virtual const char* GetObjectTypeName();
    virtual void LoadXML(linkObject* node, CStatus* status);
    virtual void PostLoadXML(linkObject* node, CStatus* status);
    virtual void Apply(float progress);

    CSimplePathAnim(CSimpleAnimGroup* group);
    void BuildSpline();
    int32_t FindControlPoint(int32_t order);
    CSimpleControlPoint* GetControlPoint(uint32_t index);
    int32_t GetMaxControlPointOrder();
    void LoadXML_ControlPoint(linkObject* node, CStatus* status);
    void NotifyControlPointAdded(CSimpleControlPoint* point);
    void NotifyControlPointRemoved(CSimpleControlPoint* point);
    void NotifyOrderChanged(CSimpleControlPoint* point);
    int32_t RegisterSplineControlPoint(CSimpleControlPoint* point);
    void UpdateControlPointPositions();
};

#include "Engine/Source/Frame/CSimplePathAnim.inl"

#endif

#ifndef WOW_SOURCE_WORLDCLIENT_MAPOBJ_H
#define WOW_SOURCE_WORLDCLIENT_MAPOBJ_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Storm/H/stpl.h"
#include "Engine/Source/Tempest/C3Vector.h"
#include "Engine/Source/Tempest/C44Matrix.h"
#include "Engine/Source/Tempest/C4Plane.h"
#include "Engine/Source/Tempest/CAaBox.h"
#include "Engine/Source/Tempest/CImVector.h"
#include "WoW/Source/WorldClient/MapBaseObj.h"

class CMapObjGroup;

struct CMapObjGroupRender {
    /* 0000 */ CMapObjGroup* m_group;
    /* 0004 */ const C44Matrix* m_matrix;
};

struct SMOPoly {
    /* 0000 */ uint8_t m_flags;
    /* 0001 */ uint8_t m_materialId;
};

class CAsyncObject;

struct SMOPortalInfo {
    /* 0000 */ uint16_t m_startVertex;
    /* 0002 */ uint16_t m_count;
    /* 0004 */ C4Plane m_plane;
};

struct SMOPortalRef {
    /* 0000 */ uint16_t m_portalIndex;
    /* 0002 */ uint16_t m_groupIndex;
    /* 0004 */ int16_t m_side;
    /* 0006 */ uint16_t m_filler;
};

struct SMOMaterial {
    /* 0000 */ uint32_t m_flags;
    /* 0004 */ uint32_t m_shader;
    /* 0008 */ uint32_t m_blendMode;
    /* 000C */ uint32_t m_texture1;
    /* 0010 */ uint32_t m_sidnColor;
    /* 0014 */ uint32_t m_frameSidnColor;
    /* 0018 */ uint32_t m_texture2;
    /* 001C */ uint32_t m_color2;
    /* 0020 */ uint32_t m_groundType;
    /* 0024 */ uint32_t m_texture3;
    /* 0028 */ uint32_t m_color3;
    /* 002C */ uint32_t m_flags3;
    /* 0030 */ uint32_t m_runtimeData[4];
};

struct SMOFog {
    /* 0000 */ uint32_t m_flags;
    /* 0004 */ C3Vector m_pos;
    /* 0010 */ float m_start;
    /* 0014 */ float m_end;
    /* 0018 */ float m_fogEnd;
    /* 001C */ float m_fogStartScalar;
    /* 0020 */ uint32_t m_fogColor;
    /* 0024 */ float m_uwFogEnd;
    /* 0028 */ float m_uwFogStartScalar;
    /* 002C */ uint32_t m_uwFogColor;
};

class CMapObjOccluder {
    public:
    /* 0000 */ uint32_t unk_0000;
    /* 0004 */ C3Vector m_start;
    /* 0010 */ C3Vector m_end;
    /* 001C */ TSLink<CMapObjOccluder> m_link;
};

class CMapObj : public CMapBaseObj, public TSHashObject<CMapObj, HASHKEY_NONE> {
    public:
    static int32_t bPoolsDirty;
    static HASHKEY_NONE s_mapObjKey;
    static TSHashTable<CMapObj, HASHKEY_NONE> s_mapObjHash;

    static void ClearCache(int32_t a1);
    static CMapObj* Create(const char* name);
    static void Update();
    static void Destroy();
    static void DestroyPools();
    static void Initialize();

    static float PortalViewDepth(const SMOPortalInfo* portal, const C4Plane& plane);
    static void PortalViewPos(const SMOPortalInfo* portal, const C44Matrix& matrix, C3Vector& pos);
    static void UpdatePools();
    static void RenderMapObjGroupsColor(const CMapObjGroupRender* groups, uint32_t count, const C44Matrix& world, const CImVector& color);

    /* 0028 */ float unk_0028;
    /* 002C */ CAsyncObject* m_asyncObject;
    /* 0030 */ uint32_t unk_0030;
    /* 0034 */ CMapObjGroup* m_groups[512];
    /* 0834 */ uint32_t unk_0834;
    /* 0838 */ uint32_t unk_0838[6];
    /* 0850 */ char m_name[260];
    /* 0954 */ uint8_t* m_data;
    /* 0958 */ uint32_t m_dataSize;
    /* 095C */ uint32_t unk_095C[3];
    /* 0968 */ const char* m_skyboxName;
    /* 096C */ uint32_t unk_096C;
    /* 0970 */ C3Vector* m_portalVertices;
    /* 0974 */ const SMOPortalInfo* m_portalInfos;
    /* 0978 */ const SMOPortalRef* m_portalRefs;
    /* 097C */ uint32_t unk_097C[12];
    /* 09AC */ uint32_t m_groupCount;
    /* 09B0 */ uint32_t unk_09B0;
    /* 09B4 */ uint32_t m_portalCount;
    /* 09B8 */ uint32_t unk_09B8[10];
    /* 09E0 */ uint32_t unk_09E0;
    /* 09E4 */ uint16_t unk_09E4;

    static void AsyncCallback(CAsyncObject* object);
    static void AsyncFailedCallback(CAsyncObject* object);

    virtual ~CMapObj();

    CMapObj();
    void ClearPointers();
    void GetBounds(CAaBox& box);
    CMapObjGroup* GetGroup(uint32_t groupNum, int32_t loaded);
    uint32_t GetGroupFlags(uint32_t groupNum);
    SMOMaterial* GetGroupMaterial(uint32_t groupNum);
    const char* GetGroupName(uint32_t groupNum);
    void Init();
    int32_t IsAllLoaded();
    int32_t IsGroupLoaded(uint32_t groupNum);
    int32_t Load(const char* name);
    void ReadGroup(uint32_t groupNum);
    void WaitLoad();
    void WaitLoadGroup(uint32_t groupNum);
    int32_t TestConvexVolume(const C3Vector& pos);
};

#endif

#ifndef WOW_SOURCE_WORLDCLIENT_MAPOBJDEF_H
#define WOW_SOURCE_WORLDCLIENT_MAPOBJDEF_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Storm/H/stpl.h"
#include "Engine/Source/Tempest/C3Vector.h"
#include "Engine/Source/Tempest/C44Matrix.h"
#include "Engine/Source/Tempest/CAaBox.h"
#include "Engine/Source/Tempest/CImVector.h"
#include "WoW/Source/WorldClient/MapBaseObj.h"
#include "WoW/Source/WorldClient/World.h"

class CMapDoodadDef;
class CMapObj;
class CMapObjDef;
class CMapLight;
class CSceneMaterial;
namespace Liquid { class CInstance; struct DisplMapPerturbationMesh; }

class CMapObjDefGroup : public CMapBaseObj {
    public:
    /* 0010 */ CMapObjDef* m_mapObjDef;
    /* 0014 */ CAaBox m_bbox;
    /* 002C */ CAaBox m_portalBox;
    /* 0044 */ int32_t m_groupNum;
    /* 0048 */ uint32_t m_frameStamp;
    /* 004C */ uint32_t m_cullStamp;
    /* 0050 */ const char* m_areaName;
    /* 0054 */ Liquid::CInstance* m_liquidInstance;
    /* 0058 */ TSExplicitList<CMapBaseObjLink, offsetof(CMapBaseObjLink, m_refLink)> m_doodadLinks;
    /* 0064 */ TSExplicitList<CMapBaseObjLink, offsetof(CMapBaseObjLink, m_refLink)> m_entityLinks;
    /* 0070 */ TSLink<CMapObjDefGroup> m_moveableLink;
    /* 0078 */ TSLink<CMapObjDefGroup> m_mapObjDefLink;
    /* 0080 */ TSGrowableArray<CSceneMaterial*> m_batchMaterials;
    /* 0090 */ uint32_t m_materialState;

    virtual ~CMapObjDefGroup();

    CMapObjDefGroup();
    void ClearRefs();
    void CreateBatchMaterials();
    void ExtendBounds(const CAaBox& box);
    void CreateDoodadRefs();
    void WaterDestroy();
    void UpdateBounds(const C44Matrix& matrix);
    void UpdateMatrix(const C44Matrix& matrix);
    void SetSequence(uint32_t sequence, int32_t a2, uint16_t doodadSet);
    void SetSequenceSpeed(float speed, int32_t a2, uint16_t doodadSet);
    void SetSequenceFinishedCallback(WORLD_SEQUENCE_CALLBACK callback, uint64_t param, uint16_t doodadSet);
    void SetEventCallback(WORLD_MODEL_EVENT_CALLBACK callback, uint64_t param, uint16_t doodadSet);
    bool IsAllDrawable();
};

class CMapObjDef : public CMapBaseObj, public TSHashObject<CMapObjDef, HASHKEY_NONE> {
    public:
    /* 0028 */ CWorldMap* m_worldMap;
    /* 002C */ CMapObj* m_mapObj;
    /* 0030 */ TSLink<CMapObjDef> m_link30;
    /* 0038 */ TSLink<CMapObjDef> m_link38;
    /* 0040 */ TSExplicitList<CMapBaseObjLink, offsetof(CMapBaseObjLink, m_ownerLink)> m_parentLinkList;
    /* 004C */ uint32_t unk_004C;
    /* 0050 */ uint32_t m_gatherStamp;
    /* 0054 */ C3Vector m_position;
    /* 0060 */ CAaBox m_bbox;
    /* 0078 */ C44Matrix m_mat;
    /* 00B8 */ C44Matrix m_invMat;
    /* 00F8 */ uint32_t m_doodadSet;
    /* 00FC */ uint32_t m_nameSet;
    /* 0100 */ uint32_t unk_0100;
    /* 0104 */ TSExplicitList<CMapObjDefGroup, offsetof(CMapObjDefGroup, m_mapObjDefLink)> m_defGroupList;
    /* 0110 */ TSCGrowableArray<CMapObjDefGroup*, 8> m_defGroups;
    /* 0134 */ TSGrowableArray<CMapLight*> m_lights;
    /* 0144 */ CImVector m_ambColor;
    /* 0148 */ uint64_t m_param64;
    /* 0150 */ Liquid::DisplMapPerturbationMesh* m_perturbatorMesh;
    /* 0154 */ uint16_t m_extraDoodadSets[3];
    /* 015A */ int16_t m_skipPortal;

    virtual ~CMapObjDef();

    CMapObjDef();
    bool TestAABox(const C3Vector& start, const C3Vector& end);
    void CreateGroups();
    int32_t IsAllDrawable();
    int32_t IsAllLoaded();
    int32_t MoveDoodadDefs(uint16_t doodadSet, CMapObjDef* target, uint16_t targetDoodadSet);
    void Purge(int32_t unlinkWorld);
    uint32_t GetGroundType(uint32_t groupNum, uint16_t polyIndex);
    CMapObjDefGroup* GetDefGroup(uint32_t index);
    void UpdateMoved();
    void SetDoodadEmittersIgnoresDistance(int32_t enable, uint16_t doodadSet);
    void SetDoodadsEnabled(int32_t enable, uint16_t doodadSet);
    void SetEventCallback(WORLD_MODEL_EVENT_CALLBACK callback, uint64_t param, uint16_t doodadSet);
    void SetSequence(uint32_t sequence, int32_t a2, uint16_t doodadSet);
    void SetSequenceFinishedCallback(WORLD_SEQUENCE_CALLBACK callback, uint64_t param, uint16_t doodadSet);
    void SetSequenceSpeed(float speed, int32_t a2, uint16_t doodadSet);
    void UpdateLoaded();
    void UpdatePos(const C3Vector& pos, float facing, float pitch, float roll);
    void UpdateMatrix(const C44Matrix& matrix);
    int32_t CanLinkTo();
    void UpdateBounds();
    bool GetFacets(const CAaBox& box, const CAaBox& box2, World::FacetData& facets, uint32_t flags);
    bool GetFacets(const CFrustum& frustum, World::FacetData& facets, uint32_t flags);
    void LinkDoodadDef(CMapDoodadDef* doodadDef, int32_t testBounds);
};

#endif

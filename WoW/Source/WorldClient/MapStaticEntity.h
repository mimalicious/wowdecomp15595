#ifndef WOW_SOURCE_WORLDCLIENT_MAPSTATICENTITY_H
#define WOW_SOURCE_WORLDCLIENT_MAPSTATICENTITY_H

#include <cstdint>
#include "Engine/Source/Tempest/C3Vector.h"
#include "Engine/Source/Tempest/CAaBox.h"
#include "Engine/Source/Tempest/CAaSphere.h"
#include "Engine/Source/Tempest/CImVector.h"
#include "WoW/Source/WorldClient/MapBaseObj.h"

class CM2Lighting;
class CM2Model;

class CMapObj;
class CMapObjDef;
class CMapObjDefGroup;
class CMapObjGroup;

class CMapStaticEntity : public CMapBaseObj {
    public:
    /* 0010 */ CWorldMap* m_worldMap;
    /* 0014 */ TSExplicitList<CMapBaseObjLink, offsetof(CMapBaseObjLink, m_ownerLink)> m_parentLinkList;
    /* 0020 */ uint32_t unk_0020;
    /* 0024 */ uint8_t m_fadeLevel;
    /* 0025 */ uint8_t m_culled;
    /* 0026 */ uint8_t unk_0026;
    /* 0027 */ uint8_t unk_0027;
    /* 0028 */ uint8_t m_sizeCategory;
    /* 002C */ uint32_t m_staticFlags;
    /* 0030 */ int32_t m_exteriorLightingValid;
    /* 0034 */ uint32_t m_entityFlags;
    /* 0038 */ CM2Model* m_model;
    /* 003C */ CAaSphere m_sphere;
    /* 004C */ CAaBox m_bbox;
    /* 0064 */ C3Vector m_vec2;
    /* 0070 */ C3Vector m_position;
    /* 007C */ float m_liquidHeight;
    /* 0080 */ CImVector m_interiorAmbient;
    /* 0084 */ CImVector m_interiorDirect;
    /* 0088 */ CImVector m_exteriorVertexLight;
    /* 008C */ uint32_t unk_008C;

    static C3Vector s_interiorLightDir;

    static void AdjustLighting(const CImVector& color, CImVector& direct, uint8_t maxDirect, CImVector& ambient, uint8_t maxAmbient);
    static void ModelEventCallback(CM2Model* model, uint32_t a2, uint32_t a3, uint32_t a4, const C3Vector& a5, uint32_t a6, uint64_t param);
    static void ModelLightingCallback(CM2Model* model, CM2Lighting* lighting, void* arg);

    virtual ~CMapStaticEntity();
    virtual void SelectUnderwater(CM2Lighting* lighting);
    virtual void AdjustLighting(CMapObjDef* mapObjDef, CMapObj* mapObj, const uint16_t* groupNum, const C3Vector* pos);
    virtual void QueryExteriorLighting();

    CMapStaticEntity();
    int32_t GetMapObjAndGroup(CMapObjDef*& mapObjDef, CMapObj*& mapObj, CMapObjDefGroup*& defGroup, CMapObjGroup*& group, int32_t skipMoving);
    void SetSequenceSpeed(float speed);
};

#endif

#ifndef WOW_SOURCE_WORLDCLIENT_WORLDQUERY_H
#define WOW_SOURCE_WORLDCLIENT_WORLDQUERY_H

#include <cstdint>
#include "Engine/Source/Model2/M2Model.h"
#include "WoW/Source/WorldClient/MapObj.h"

class C44Matrix;
class CMapObjGroup;

class CWorldQuery {
    public:
    /* 0000 */ CWorldQueryElement* m_mapObjGroups;
    /* 0004 */ uint32_t m_mapObjGroupCount;
    /* 0008 */ uint32_t m_mapObjGroupMax;
    /* 000C */ CM2Model** m_models;
    /* 0010 */ uint32_t m_modelCount;
    /* 0014 */ uint32_t m_modelMax;
    /* 0018 */ CModelBatchList m_untexturedBatches;
    /* 0028 */ CModelBatchList m_texturedBatches;

    CWorldQuery();
    ~CWorldQuery();
    void AddMapObjGroup(CMapObjGroup* group, const C44Matrix* matrix);
    void AllocMapObjGroupList(uint32_t count);
    void AllocModelBatchList(uint32_t count);
    void FreeMapObjGroupList();
    void FreeModelBatchList();
    void FreeModelList();
};

#endif

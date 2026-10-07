#include "WoW/Source/WorldClient/MapObj.h"
#include <cfloat>
#include "Engine/Source/Storm/H/storm.h"
#include "WoW/Source/WorldClient/Map.h"
#include "WoW/Source/WorldClient/MapObjGroup.h"

// STATIC: 0x00E75684
HASHKEY_NONE CMapObj::s_mapObjKey;

// STATIC: 0x00E75688
TSHashTable<CMapObj, HASHKEY_NONE> CMapObj::s_mapObjHash;

// OFFSET: 0x00745A30
void CMapObj::ClearPointers() {
    this->unk_095C[0] = 0;
    this->unk_095C[1] = 0;
    this->unk_095C[2] = 0;
    this->unk_096C = 0;
    this->m_portalVertices = NULL;
    this->m_portalInfos = NULL;
    this->m_portalRefs = NULL;
    this->unk_097C[2] = 0;
    this->unk_097C[3] = 0;
    this->unk_097C[4] = 0;
    this->unk_097C[5] = 0;
    this->unk_097C[6] = 0;
    this->unk_097C[7] = 0;
    this->unk_097C[10] = 0;
    this->unk_097C[11] = 0;
    this->m_groupCount = 0;
    this->unk_09B0 = 0;
    this->m_portalCount = 0;
    this->unk_09B8[0] = 0;
    this->unk_09B8[3] = 0;
    this->unk_09B8[4] = 0;
    this->unk_09B8[5] = 0;
    this->unk_09B8[6] = 0;
    this->unk_09B8[7] = 0;
    this->unk_09B8[8] = 0;
}

// OFFSET: 0x00745C50
void CMapObj::Init() {
    this->unk_09E4 = 0xFFFF;
    this->unk_0838[0] = 0;
    this->unk_0838[1] = 0;
    this->unk_0838[2] = 0;
    this->unk_0838[3] = 0;
    this->unk_0838[4] = 0;
    this->m_data = NULL;
    this->m_dataSize = 0;
    this->m_asyncObject = NULL;
    this->unk_097C[8] = 0;
    this->unk_097C[9] = 0;
    this->unk_09B8[9] = 0;
    this->unk_0838[5] = 0;
    this->unk_0030 = 0;
    this->unk_0028 = FLT_MAX;
    this->ClearPointers();
}

// OFFSET: 0x00745DD0
int32_t CMapObj::IsGroupLoaded(uint32_t groupNum) {
    if (!(this->m_flags & 0x40)) {
        return 0;
    }

    return this->m_groups[groupNum]->m_flags & 0x40;
}

// OFFSET: 0x00747000
CMapObj::CMapObj() {
    this->unk_0030 = 512;
    this->unk_0838[0] = 0;
    this->unk_0838[1] = 0;
    this->unk_0838[2] = 0;
    this->unk_0838[3] = 0;
    this->unk_0838[4] = 0;
    this->unk_0838[5] = 0;
    this->unk_09E0 = 0;
    this->Init();
}

// OFFSET: 0x00748360
CMapObj* CMapObj::Create(const char* name) {
    uint32_t hashval = SStrHash(name, 0, 0);
    CMapObj* mapObj = CMapObj::s_mapObjHash.Ptr(hashval, CMapObj::s_mapObjKey);

    if (!mapObj) {
        mapObj = CMap::AllocMapObj();
        mapObj->Load(name);
        CMapObj::s_mapObjHash.Insert(mapObj, hashval, CMapObj::s_mapObjKey);
    }

    mapObj->m_refCount++;
    return mapObj;
}

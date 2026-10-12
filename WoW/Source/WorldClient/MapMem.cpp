#include "WoW/Source/WorldClient/Map.h"
#include <new>
#include "Engine/Source/Storm/H/storm.h"
#include "WoW/Source/WorldClient/MapDoodadDef.h"
#include "WoW/Source/WorldClient/MapEntity.h"
#include "WoW/Source/WorldClient/MapLight.h"
#include "WoW/Source/WorldClient/MapObj.h"
#include "WoW/Source/WorldClient/MapObjDef.h"
#include "WoW/Source/WorldClient/MapObjGroup.h"
#include "WoW/Source/WorldClient/MapRenderChunk.h"

// STATIC: 0x00E893E0/0x140D75110
TSObjectAllocPool<CMapLight>* CMap::s_lightHeap;

// STATIC: 0x00E893E4/0x140D75118
TSObjectAllocPool<CMapObjGroup>* CMap::s_mapObjGroupHeap;

// STATIC: 0x00E893E8/0x140D75120
TSObjectAllocPool<CMapObj>* CMap::s_mapObjHeap;

// STATIC: 0x00E893EC/0x140D75128
TSObjectAllocPool<CMapBaseObjLink>* CMap::s_baseObjLinkHeap;

// STATIC: 0x00E893F0/0x140D75130
TSObjectAllocPool<CMapArea>* CMap::s_areaHeap;

// STATIC: 0x00E893F4/0x140D75138
TSObjectAllocPool<CMapAreaMed>* CMap::s_areaMedHeap;

// STATIC: 0x00E893F8/0x140D75140
TSObjectAllocPool<CMapAreaLow>* CMap::s_areaLowHeap;

// STATIC: 0x00E893FC/0x140D75148
TSObjectAllocPool<CMapChunk>* CMap::s_chunkHeap;

// STATIC: 0x00E89400/0x140D75150
TSObjectAllocPool<CMapRenderChunk>* CMap::s_renderChunkHeap;

// STATIC: 0x00E89404/0x140D75158
TSObjectAllocPool<CMapChunkBuf>* CMap::s_chunkBufHeap;

// STATIC: 0x00E89408/0x140D75160
TSObjectAllocPool<CMapChunkInfo>* CMap::s_chunkInfoHeap;

// STATIC: 0x00E8940C/0x140D75168
TSObjectAllocPool<CMapDoodadDef>* CMap::s_doodadDefHeap;

// STATIC: 0x00E89410/0x140D75170
TSObjectAllocPool<CMapEntity>* CMap::s_entityHeap;

// STATIC: 0x00E89414/0x140D75178
TSObjectAllocPool<CMapObjDefGroup>* CMap::s_mapObjDefGroupHeap;

// STATIC: 0x00E89418/0x140D75180
TSObjectAllocPool<CMapObjDef>* CMap::s_mapObjDefHeap;

// STATIC: 0x00E8941C/0x140D75188
TSObjectAllocPool<CChunkLiquid>* CMap::s_chunkLiquidHeap;

template <class T>
static TSObjectAllocPool<T>* NewPool(const char* name, uint32_t objectsPerBlock, int32_t line) {
    void* mem = SMemAlloc(sizeof(TSObjectAllocPool<T>), __FILE__, line, 0);
    return mem ? new (mem) TSObjectAllocPool<T>(name, objectsPerBlock) : NULL;
}

// PROVEN: bc78ff82 IDENTICAL/DIFFERENT
// OFFSET: 0x00775900/0x14045FFE0
void* CMap::AllocRawAreaData(uint32_t size) {
    return SMemAlloc(size, __FILE__, __LINE__, 0);
}

// PROVEN: 9b0ed89c IDENTICAL/DIFFERENT
// OFFSET: 0x00775920/0x140460000
void CMap::FreeRawAreaData(void* data) {
    SMemFree(data, __FILE__, __LINE__, 0);
}

// PROVEN: cfae4b05 IDENTICAL/NONE
// OFFSET: 0x00775FC0
CMapObj* CMap::AllocMapObj() {
    return CMap::s_mapObjHeap->New("", 0);
}

// PROVEN: 5363ed54 IDENTICAL/DIFFERENT
// OFFSET: 0x00775FE0/0x140460610
void CMap::FreeMapObj(CMapObj* mapObj) {
    CMap::s_mapObjHeap->Delete(mapObj, "", 0);
}

// PROVEN: 84b4c79b IDENTICAL/DIFFERENT
// OFFSET: 0x00776030/0x140460660
CMapObjGroup* CMap::AllocMapObjGroup() {
    return CMap::s_mapObjGroupHeap->New("", 0);
}

// PROVEN: 6c68d4f2 IDENTICAL/DIFFERENT
// OFFSET: 0x00776050/0x140460670
void CMap::FreeMapObjGroup(CMapObjGroup* mapObjGroup) {
    CMap::s_mapObjGroupHeap->Delete(mapObjGroup, "", 0);
}

// PROVEN: 7041d1ba IDENTICAL/DIFFERENT
// OFFSET: 0x007760A0/0x1404606C0
CMapArea* CMap::AllocArea() {
    return CMap::s_areaHeap->New("", 0);
}

// PROVEN: f82bedf1 IDENTICAL/DIFFERENT
// OFFSET: 0x007760C0/0x1404606D0
void CMap::FreeArea(CMapArea* area) {
    area->m_mapLink.Unlink();
    CMap::s_areaHeap->Delete(area, "", 0);
}

// PROVEN: 71ef28a7 IDENTICAL/NONE
// OFFSET: 0x00776140/0x140460770
CMapAreaLow* CMap::AllocAreaLow() {
    return CMap::s_areaLowHeap->New("", 0);
}

// PROVEN: cdd373df IDENTICAL/DIFFERENT
// OFFSET: 0x00776160/0x140460780
void CMap::FreeAreaLow(CMapAreaLow* areaLow) {
    CMap::s_areaLowHeap->Delete(areaLow, "", 0);
}

// PROVEN: 1be8ee8f IDENTICAL/NONE
// OFFSET: 0x007761B0
CMapChunk* CMap::AllocChunk() {
    return CMap::s_chunkHeap->New("", 0);
}

// PROVEN: bebc4ccb IDENTICAL/DIFFERENT
// OFFSET: 0x007761D0/0x1404607E0
void CMap::FreeChunk(CMapChunk* chunk) {
    chunk->m_areaLink.Unlink();
    CMap::s_chunkHeap->Delete(chunk, "", 0);
}

// PROVEN: ff521b47 IDENTICAL/NONE
// OFFSET: 0x00776250/0x140460880
CMapRenderChunk* CMap::AllocRenderChunk() {
    return CMap::s_renderChunkHeap->New("", 0);
}

// PROVEN: 02bd6c9b IDENTICAL/DIFFERENT
// OFFSET: 0x00776270/0x140460890
void CMap::FreeRenderChunk(CMapRenderChunk* renderChunk) {
    renderChunk->m_link.Unlink();
    CMap::s_renderChunkHeap->Delete(renderChunk, "", 0);
}

// PROVEN: 2ad07806 IDENTICAL/DIFFERENT
// OFFSET: 0x007762F0/0x140460930
CMapChunkBuf* CMap::AllocChunkBuf() {
    return CMap::s_chunkBufHeap->New("", 0);
}

// PROVEN: b1c1e5ba IDENTICAL/DIFFERENT
// OFFSET: 0x00776310/0x140460940
void CMap::FreeChunkBuf(CMapChunkBuf* chunkBuf) {
    chunkBuf->m_link.Unlink();
    CMap::s_chunkBufHeap->Delete(chunkBuf, "", 0);
}

// PROVEN: cf25f4c5 IDENTICAL/NONE
// OFFSET: 0x00776390
CMapDoodadDef* CMap::AllocDoodadDef() {
    return CMap::s_doodadDefHeap->New("", 0);
}

// PROVEN: b4a8f3df IDENTICAL/DIFFERENT
// OFFSET: 0x007763B0/0x1404609F0
void CMap::FreeDoodadDef(CMapDoodadDef* doodadDef) {
    CMap::s_doodadDefHeap->Delete(doodadDef, "", 0);
}

// PROVEN: 172929f8 IDENTICAL/DIFFERENT
// OFFSET: 0x00776400/0x140460A40
CMapEntity* CMap::AllocEntity() {
    return CMap::s_entityHeap->New("", 0);
}

// PROVEN: d830969d IDENTICAL/DIFFERENT
// OFFSET: 0x00776420/0x140460A50
void CMap::FreeEntity(CMapEntity* entity) {
    CMap::s_entityHeap->Delete(entity, "", 0);
}

// PROVEN: c2b7eb60 IDENTICAL/DIFFERENT
// OFFSET: 0x00776470/0x140460AA0
CMapObjDefGroup* CMap::AllocMapObjDefGroup() {
    return CMap::s_mapObjDefGroupHeap->New("", 0);
}

// PROVEN: 30fcfb4b IDENTICAL/DIFFERENT
// OFFSET: 0x00776490/0x140460AB0
void CMap::FreeMapObjDefGroup(CMapObjDefGroup* mapObjDefGroup) {
    mapObjDefGroup->m_mapObjDefLink.Unlink();
    CMap::s_mapObjDefGroupHeap->Delete(mapObjDefGroup, "", 0);
}

// PROVEN: dc58b4c4 IDENTICAL/NONE
// OFFSET: 0x00776510
CMapObjDef* CMap::AllocMapObjDef() {
    return CMap::s_mapObjDefHeap->New("", 0);
}

// PROVEN: 49cc6230 IDENTICAL/DIFFERENT
// OFFSET: 0x00776530/0x140460B70
void CMap::FreeMapObjDef(CMapObjDef* mapObjDef) {
    CMap::s_mapObjDefHeap->Delete(mapObjDef, "", 0);
}

// PROVEN: 9945248f IDENTICAL/DIFFERENT
// OFFSET: 0x00776580/0x140460BC0
CChunkLiquid* CMap::AllocChunkLiquid() {
    return CMap::s_chunkLiquidHeap->New("", 0);
}

// PROVEN: e5dc5fe6 IDENTICAL/DIFFERENT
// OFFSET: 0x007765A0/0x140460BD0
void CMap::FreeChunkLiquid(CChunkLiquid* liquid) {
    CMap::s_chunkLiquidHeap->Delete(liquid, "", 0);
}

// OFFSET: 0x00776660
CMapChunkInfo* CMap::AllocChunkInfo() {
    return CMap::s_chunkInfoHeap->New("", 0);
}

// PROVEN: 12691b6e IDENTICAL/DIFFERENT
// OFFSET: 0x00776680/0x140460D20
CMapBaseObjLink* CMap::AllocBaseObjLink(CMapBaseObj* owner, CMapBaseObj* ref) {
    owner->m_refCount++;
    CMapBaseObjLink* link = CMap::s_baseObjLinkHeap->New("", 0);
    link->m_owner = owner;
    link->m_ref = ref;
    return link;
}

// PROVEN: 41149d71 IDENTICAL/DIFFERENT
// OFFSET: 0x007766B0/0x140460D60
void CMap::FreeChunkInfo(CMapChunkInfo* chunkInfo) {
    CMap::s_chunkInfoHeap->Delete(chunkInfo, "", 0);
}

// OFFSET: 0x00776D20/0x140461520
void CMap::FreeBaseObjLink(CMapBaseObjLink* link) {
    link->m_owner->m_refCount--;
    link->m_ownerLink.Unlink();
    link->m_refLink.Unlink();
    link->m_ref = NULL;
    link->m_owner = NULL;
    CMap::s_baseObjLinkHeap->Delete(link, "", 0);
}

// OFFSET: 0x007766F0/0x140460DA0
void CMap::MapMemInitialize() {
    CMap::s_lightHeap = NewPool<CMapLight>("WLIGHT", 128, __LINE__);
    CMap::s_mapObjGroupHeap = NewPool<CMapObjGroup>("WMAPOBJGROUP", 128, __LINE__);
    CMap::s_mapObjHeap = NewPool<CMapObj>("WMAPOBJ", 32, __LINE__);
    CMap::s_baseObjLinkHeap = NewPool<CMapBaseObjLink>("WBASEOBJLINK", 10000, __LINE__);
    CMap::s_areaHeap = NewPool<CMapArea>("WAREA", 16, __LINE__);
    CMap::s_areaMedHeap = NewPool<CMapAreaMed>("WAREAMED", 16, __LINE__);
    CMap::s_areaLowHeap = NewPool<CMapAreaLow>("WAREALOW", 16, __LINE__);
    CMap::s_chunkHeap = NewPool<CMapChunk>("WCHUNK", 256, __LINE__);
    CMap::s_renderChunkHeap = NewPool<CMapRenderChunk>("WRCHUNK", 256, __LINE__);
    CMap::s_chunkBufHeap = NewPool<CMapChunkBuf>("WCHUNKBUF", 256, __LINE__);
    CMap::s_chunkInfoHeap = NewPool<CMapChunkInfo>("WCHUNKINFO", 16, __LINE__);
    CMap::s_doodadDefHeap = NewPool<CMapDoodadDef>("WDOODADDEF", 5000, __LINE__);
    CMap::s_entityHeap = NewPool<CMapEntity>("WENTITY", 128, __LINE__);
    CMap::s_mapObjDefGroupHeap = NewPool<CMapObjDefGroup>("WMAPOBJDEFGROUP", 128, __LINE__);
    CMap::s_mapObjDefHeap = NewPool<CMapObjDef>("WMAPOBJDEF", 64, __LINE__);
    CMap::s_chunkLiquidHeap = NewPool<CChunkLiquid>("WCHUNKLIQUID", 64, __LINE__);
}

// OFFSET: 0x00776DD0/0x140461610
void CMap::MapMemDestroy() {
    if (CMap::s_lightHeap) {
        delete CMap::s_lightHeap;
    }

    if (CMap::s_mapObjGroupHeap) {
        delete CMap::s_mapObjGroupHeap;
    }

    if (CMap::s_mapObjHeap) {
        delete CMap::s_mapObjHeap;
    }

    if (CMap::s_baseObjLinkHeap) {
        delete CMap::s_baseObjLinkHeap;
    }

    if (CMap::s_areaHeap) {
        delete CMap::s_areaHeap;
    }

    if (CMap::s_areaMedHeap) {
        delete CMap::s_areaMedHeap;
    }

    if (CMap::s_areaLowHeap) {
        delete CMap::s_areaLowHeap;
    }

    if (CMap::s_chunkHeap) {
        delete CMap::s_chunkHeap;
    }

    if (CMap::s_renderChunkHeap) {
        delete CMap::s_renderChunkHeap;
    }

    if (CMap::s_chunkBufHeap) {
        delete CMap::s_chunkBufHeap;
    }

    if (CMap::s_chunkInfoHeap) {
        delete CMap::s_chunkInfoHeap;
    }

    if (CMap::s_doodadDefHeap) {
        delete CMap::s_doodadDefHeap;
    }

    if (CMap::s_entityHeap) {
        delete CMap::s_entityHeap;
    }

    if (CMap::s_mapObjDefGroupHeap) {
        delete CMap::s_mapObjDefGroupHeap;
    }

    if (CMap::s_mapObjDefHeap) {
        delete CMap::s_mapObjDefHeap;
    }

    if (CMap::s_chunkLiquidHeap) {
        delete CMap::s_chunkLiquidHeap;
    }
}

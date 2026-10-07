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

// STATIC: 0x00E893E0
TSObjectAllocPool<CMapLight>* CMap::s_lightHeap;

// STATIC: 0x00E893E4
TSObjectAllocPool<CMapObjGroup>* CMap::s_mapObjGroupHeap;

// STATIC: 0x00E893E8
TSObjectAllocPool<CMapObj>* CMap::s_mapObjHeap;

// STATIC: 0x00E893EC
TSObjectAllocPool<CMapBaseObjLink>* CMap::s_baseObjLinkHeap;

// STATIC: 0x00E893F0
TSObjectAllocPool<CMapArea>* CMap::s_areaHeap;

// STATIC: 0x00E893F4
TSObjectAllocPool<CMapAreaMed>* CMap::s_areaMedHeap;

// STATIC: 0x00E893F8
TSObjectAllocPool<CMapAreaLow>* CMap::s_areaLowHeap;

// STATIC: 0x00E893FC
TSObjectAllocPool<CMapChunk>* CMap::s_chunkHeap;

// STATIC: 0x00E89400
TSObjectAllocPool<CMapRenderChunk>* CMap::s_renderChunkHeap;

// STATIC: 0x00E89404
TSObjectAllocPool<CMapChunkBuf>* CMap::s_chunkBufHeap;

// STATIC: 0x00E89408
TSObjectAllocPool<CMapChunkInfo>* CMap::s_chunkInfoHeap;

// STATIC: 0x00E8940C
TSObjectAllocPool<CMapDoodadDef>* CMap::s_doodadDefHeap;

// STATIC: 0x00E89410
TSObjectAllocPool<CMapEntity>* CMap::s_entityHeap;

// STATIC: 0x00E89414
TSObjectAllocPool<CMapObjDefGroup>* CMap::s_mapObjDefGroupHeap;

// STATIC: 0x00E89418
TSObjectAllocPool<CMapObjDef>* CMap::s_mapObjDefHeap;

// STATIC: 0x00E8941C
TSObjectAllocPool<CChunkLiquid>* CMap::s_chunkLiquidHeap;

template <class T>
static TSObjectAllocPool<T>* NewPool(const char* name, uint32_t objectsPerBlock, int32_t line) {
    void* mem = SMemAlloc(sizeof(TSObjectAllocPool<T>), __FILE__, line, 0);
    return mem ? new (mem) TSObjectAllocPool<T>(name, objectsPerBlock) : NULL;
}

// PROVEN: bc78ff82 IDENTICAL/NONE
// OFFSET: 0x00775900
void* CMap::AllocRawAreaData(uint32_t size) {
    return SMemAlloc(size, __FILE__, __LINE__, 0);
}

// PROVEN: 9b0ed89c IDENTICAL/NONE
// OFFSET: 0x00775920/0x140460000
void CMap::FreeRawAreaData(void* data) {
    SMemFree(data, __FILE__, __LINE__, 0);
}

// OFFSET: 0x00775FC0
CMapObj* CMap::AllocMapObj() {
    return CMap::s_mapObjHeap->New(__FILE__, __LINE__);
}

// OFFSET: 0x00775FE0
void CMap::FreeMapObj(CMapObj* mapObj) {
    CMap::s_mapObjHeap->Delete(mapObj, __FILE__, __LINE__);
}

// OFFSET: 0x00776030/0x140460660
CMapObjGroup* CMap::AllocMapObjGroup() {
    return CMap::s_mapObjGroupHeap->New(__FILE__, __LINE__);
}

// OFFSET: 0x00776050/0x140460670
void CMap::FreeMapObjGroup(CMapObjGroup* mapObjGroup) {
    CMap::s_mapObjGroupHeap->Delete(mapObjGroup, __FILE__, __LINE__);
}

// OFFSET: 0x007760A0
CMapArea* CMap::AllocArea() {
    return CMap::s_areaHeap->New(__FILE__, __LINE__);
}

// OFFSET: 0x007760C0/0x1404606D0
void CMap::FreeArea(CMapArea* area) {
    area->m_mapLink.Unlink();
    CMap::s_areaHeap->Delete(area, __FILE__, __LINE__);
}

// OFFSET: 0x00776140
CMapAreaLow* CMap::AllocAreaLow() {
    return CMap::s_areaLowHeap->New(__FILE__, __LINE__);
}

// OFFSET: 0x00776160
void CMap::FreeAreaLow(CMapAreaLow* areaLow) {
    CMap::s_areaLowHeap->Delete(areaLow, __FILE__, __LINE__);
}

// OFFSET: 0x007761B0
CMapChunk* CMap::AllocChunk() {
    return CMap::s_chunkHeap->New(__FILE__, __LINE__);
}

// OFFSET: 0x007761D0
void CMap::FreeChunk(CMapChunk* chunk) {
    chunk->m_areaLink.Unlink();
    CMap::s_chunkHeap->Delete(chunk, __FILE__, __LINE__);
}

// OFFSET: 0x00776250
CMapRenderChunk* CMap::AllocRenderChunk() {
    return CMap::s_renderChunkHeap->New(__FILE__, __LINE__);
}

// OFFSET: 0x00776270
void CMap::FreeRenderChunk(CMapRenderChunk* renderChunk) {
    renderChunk->m_link.Unlink();
    CMap::s_renderChunkHeap->Delete(renderChunk, __FILE__, __LINE__);
}

// OFFSET: 0x007762F0/0x140460930
CMapChunkBuf* CMap::AllocChunkBuf() {
    return CMap::s_chunkBufHeap->New(__FILE__, __LINE__);
}

// OFFSET: 0x00776310
void CMap::FreeChunkBuf(CMapChunkBuf* chunkBuf) {
    chunkBuf->m_link.Unlink();
    CMap::s_chunkBufHeap->Delete(chunkBuf, __FILE__, __LINE__);
}

// OFFSET: 0x00776390
CMapDoodadDef* CMap::AllocDoodadDef() {
    return CMap::s_doodadDefHeap->New(__FILE__, __LINE__);
}

// OFFSET: 0x007763B0/0x1404609F0
void CMap::FreeDoodadDef(CMapDoodadDef* doodadDef) {
    CMap::s_doodadDefHeap->Delete(doodadDef, __FILE__, __LINE__);
}

// OFFSET: 0x00776400/0x140460A40
CMapEntity* CMap::AllocEntity() {
    return CMap::s_entityHeap->New(__FILE__, __LINE__);
}

// OFFSET: 0x00776420/0x140460A50
void CMap::FreeEntity(CMapEntity* entity) {
    CMap::s_entityHeap->Delete(entity, __FILE__, __LINE__);
}

// OFFSET: 0x00776470
CMapObjDefGroup* CMap::AllocMapObjDefGroup() {
    return CMap::s_mapObjDefGroupHeap->New(__FILE__, __LINE__);
}

// OFFSET: 0x00776490/0x140460AB0
void CMap::FreeMapObjDefGroup(CMapObjDefGroup* mapObjDefGroup) {
    mapObjDefGroup->m_mapObjDefLink.Unlink();
    CMap::s_mapObjDefGroupHeap->Delete(mapObjDefGroup, __FILE__, __LINE__);
}

// OFFSET: 0x00776510
CMapObjDef* CMap::AllocMapObjDef() {
    return CMap::s_mapObjDefHeap->New(__FILE__, __LINE__);
}

// OFFSET: 0x00776530/0x140460B70
void CMap::FreeMapObjDef(CMapObjDef* mapObjDef) {
    CMap::s_mapObjDefHeap->Delete(mapObjDef, __FILE__, __LINE__);
}

// OFFSET: 0x00776580
CChunkLiquid* CMap::AllocChunkLiquid() {
    return CMap::s_chunkLiquidHeap->New(__FILE__, __LINE__);
}

// OFFSET: 0x007765A0
void CMap::FreeChunkLiquid(CChunkLiquid* liquid) {
    CMap::s_chunkLiquidHeap->Delete(liquid, __FILE__, __LINE__);
}

// OFFSET: 0x00776660
CMapChunkInfo* CMap::AllocChunkInfo() {
    return CMap::s_chunkInfoHeap->New(__FILE__, __LINE__);
}

// OFFSET: 0x00776680/0x140460D20
CMapBaseObjLink* CMap::AllocBaseObjLink(CMapBaseObj* owner, CMapBaseObj* ref) {
    owner->m_refCount++;
    CMapBaseObjLink* link = CMap::s_baseObjLinkHeap->New(__FILE__, __LINE__);
    link->m_owner = owner;
    link->m_ref = ref;
    return link;
}

// OFFSET: 0x007766B0/0x140460D60
void CMap::FreeChunkInfo(CMapChunkInfo* chunkInfo) {
    CMap::s_chunkInfoHeap->Delete(chunkInfo, __FILE__, __LINE__);
}

// OFFSET: 0x00776D20/0x140461520
void CMap::FreeBaseObjLink(CMapBaseObjLink* link) {
    link->m_owner->m_refCount--;
    link->m_ownerLink.Unlink();
    link->m_refLink.Unlink();
    link->m_ref = NULL;
    link->m_owner = NULL;
    CMap::s_baseObjLinkHeap->Delete(link, __FILE__, __LINE__);
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

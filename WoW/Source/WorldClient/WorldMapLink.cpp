#include "WoW/Source/WorldClient/WorldMap.h"
#include "Engine/Source/Tempest/C3Segment.h"
#include "WoW/Source/WorldClient/CWorldMath.h"
#include "WoW/Source/WorldClient/Map.h"
#include "WoW/Source/WorldClient/MapChunk.h"
#include "WoW/Source/WorldClient/MapEntity.h"
#include "WoW/Source/WorldClient/MapObj.h"
#include "WoW/Source/WorldClient/MapObjDef.h"
#include "WoW/Source/WorldClient/MapObjGroup.h"
#include "WoW/Source/WorldClient/MapStaticEntity.h"

// OFFSET: 0x00785460
static void ClassifyStaticEntityLink(CMapStaticEntity* entity, CMapObjDef* mapObjDef, CMapObjDefGroup* group, uint16_t polyIndex, C3Vector pos) {
    CMapObjGroup* objGroup = mapObjDef->m_mapObj->GetGroup(group->m_groupNum, 0);

    if (objGroup) {
        if (objGroup->m_groupFlags & 0x48) {
            entity->m_flags |= 0x1;
        } else {
            entity->m_flags |= 0x2;
        }
    } else {
        entity->m_flags |= 0x1;
    }

    if (entity->m_flags & 0x2) {
        entity->AdjustLighting(mapObjDef, group->m_groupNum, &polyIndex, &pos);
    }
}

// OFFSET: 0x007854C0/0x140474090
void CWorldMap::LinkIntersectMapObjDefGroup(CMapObjDef* mapObjDef, uint32_t groupNum, const C3Vector& start, const C3Vector& end, MapObjIntersectData& front, MapObjIntersectData& back) {
    CMapObj* mapObj = mapObjDef->m_mapObj;
    CMapObjGroup* group = mapObj->GetGroup(groupNum, 0);

    if (!group) {
        return;
    }

    uint32_t groupFlags = group->m_groupFlags;
    uint32_t frontFace = 0xFFFFFFFF;
    uint32_t backFace = 0xFFFFFFFF;
    C3Segment seg;
    seg.b = start;
    seg.t = end;
    uint16_t exterior = (groupFlags & 0x8) == 0;

    if (group->GetFacesForLinking(seg, front.m_t, frontFace, back.m_t, backFace)) {
        if (frontFace != 0xFFFFFFFF) {
            front.m_mapObjDef = mapObjDef;
            front.m_group = mapObjDef->m_defGroups[groupNum];
            front.m_polyIndex = static_cast<uint16_t>(frontFace);
            front.m_exterior = exterior;
        }

        if (backFace != 0xFFFFFFFF) {
            back.m_mapObjDef = mapObjDef;
            back.m_group = mapObjDef->m_defGroups[groupNum];
            back.m_polyIndex = static_cast<uint16_t>(backFace);
            back.m_exterior = exterior;
        }
    }

    if (!exterior) {
        return;
    }

    float t = 1.05f;
    uint32_t portalGroup[2];

    if (mapObj->VectorIntersectPortals(groupNum, seg, t, portalGroup) && t - front.m_t < 0.000099999997f) {
        front.m_mapObjDef = mapObjDef;
        CMapObjDefGroup* defGroup = mapObjDef->m_defGroups[portalGroup[0]];
        front.m_group = defGroup;
        front.m_polyIndex = 0xFFFF;
        front.m_t = t;
        front.m_exterior = (mapObj->GetGroup(defGroup->m_groupNum, 0)->m_groupFlags & 0x8) == 0;
    }
}

// OFFSET: 0x00785660/0x1404742C0
bool CWorldMap::LinkIntersectMapObjDef(CMapObjDef* mapObjDef, const C3Vector& start, const C3Vector& end, const C3Vector& pos, MapObjIntersectData* front, MapObjIntersectData* back) {
    CMapObj* mapObj = mapObjDef->m_mapObj;

    if (!mapObj) {
        return false;
    }

    C3Vector localStart = start * mapObjDef->m_invMat;
    C3Vector localEnd = end * mapObjDef->m_invMat;
    C3Vector localPos = pos * mapObjDef->m_invMat;
    uint32_t interior = mapObjDef->m_flags & 0x200;

    for (uint32_t i = 0; i < mapObj->m_groupCount; i++) {
        uint32_t groupFlags = mapObj->GetGroupFlags(i);

        if (groupFlags & 0x410080) {
            continue;
        }

        if (!mapObj->TestGroupBounds(localStart, localEnd, i)) {
            continue;
        }

        if ((!interior && (groupFlags & 0x8)) || mapObj->TestGroupBounds(localPos, i)) {
            CWorldMap::LinkIntersectMapObjDefGroup(mapObjDef, i, localStart, localEnd, front[interior == 0], back[interior == 0]);
        }
    }

    return true;
}

// OFFSET: 0x00785960/0x140474630
bool CWorldMap::LinkIntersectMapObjDefs(const C3Vector& start, const C3Vector& end, const C3Vector& pos, MapObjIntersectData* front, MapObjIntersectData* back, const CMapChunk* chunk) {
    back[0].m_t = 1.05f;
    back[0].m_mapObjDef = NULL;
    back[1].m_t = 1.05f;
    back[1].m_mapObjDef = NULL;
    front[0].m_t = 1.05f;
    front[0].m_mapObjDef = NULL;
    front[1].m_t = 1.05f;
    front[1].m_mapObjDef = NULL;

    if (chunk) {
        TSExplicitList<CMapBaseObjLink, offsetof(CMapBaseObjLink, m_refLink)>& links = const_cast<CMapChunk*>(chunk)->m_mapObjDefLinkList;

        for (CMapBaseObjLink* link = links.Head(); link; link = links.Next(link)) {
            CMapObjDef* mapObjDef = static_cast<CMapObjDef*>(link->m_owner);
            uint32_t flags = mapObjDef->m_flags;

            if (!(flags & 0x10) && (flags & 0x40) && mapObjDef->TestAABox(start, end)) {
                this->LinkIntersectMapObjDef(mapObjDef, start, end, pos, front, back);
            }
        }
    }

    for (CMapObjDef* mapObjDef = this->m_mapObjDefList.Head(), * next; mapObjDef; mapObjDef = next) {
        next = this->m_mapObjDefList.Next(mapObjDef);
        uint32_t flags = mapObjDef->m_flags;

        if (!(flags & 0x10) && (flags & 0x40) && mapObjDef->TestAABox(start, end)) {
            this->LinkIntersectMapObjDef(mapObjDef, start, end, pos, front, back);
        }
    }

    if (!front[0].m_mapObjDef && back[0].m_mapObjDef) {
        front[0] = back[0];
    }

    if (!front[1].m_mapObjDef && back[1].m_mapObjDef) {
        front[1] = back[1];
    }

    if (front[0].m_mapObjDef && (front[0].m_mapObjDef->m_flags & 0x4000)) {
        front[0].m_mapObjDef = NULL;
        back[0].m_mapObjDef = NULL;
    }

    if (!front[0].m_mapObjDef) {
        if (!front[1].m_mapObjDef) {
            return false;
        }

        front[0] = front[1];
        back[0] = back[1];
        front[1].m_mapObjDef = NULL;
        back[1].m_mapObjDef = NULL;
    }

    if (!back[0].m_mapObjDef) {
        back[0] = front[0];
        back[0].m_polyIndex = 0xFFFF;
    }

    if (!back[1].m_mapObjDef) {
        back[1] = front[1];
        back[1].m_polyIndex = 0xFFFF;
    }

    return true;
}

// OFFSET: 0x00785B70/0x1404748C0
int32_t CWorldMap::LinkObjectToMapExterior(CMapBaseObj* object, const CAaBox& box, int32_t linkChunks, int32_t linkMapObjs) {
    if (this->m_isGlobalMapObj) {
        return 0;
    }

    CWorldMap::s_queryTag++;
    int32_t minCol = static_cast<int32_t>(-(box.t.y - 17066.666f) * 0.029999999f);
    int32_t minRow = static_cast<int32_t>(-(box.t.x - 17066.666f) * 0.029999999f);
    int32_t maxCol = static_cast<int32_t>(-(box.b.y - 17066.666f) * 0.029999999f);
    int32_t maxRow = static_cast<int32_t>(-(box.b.x - 17066.666f) * 0.029999999f);
    int32_t result = 1;
    CiRect rect(0, 0, 0, 0);

    for (int32_t y = minRow >> 4; y <= maxRow >> 4; y++) {
        rect.minY = 16 * y <= minRow ? minRow : 16 * y;
        rect.maxY = 16 * y + 15 >= maxRow ? maxRow : 16 * y + 15;

        for (int32_t x = minCol >> 4; x <= maxCol >> 4; x++) {
            CMap* map;

            if (this->m_isGlobalMapObj) {
                map = this->m_maps[0];
            } else {
                CMapTableEntry* entry = this->m_mapTable.Get(x, y);

                if (!entry || !(entry->m_flags & 0x1)) {
                    continue;
                }

                map = this->m_maps[entry->m_mapIndex];
            }

            if (!map) {
                continue;
            }

            rect.minX = 16 * x <= minCol ? minCol : 16 * x;
            rect.maxX = 16 * x + 15 >= maxCol ? maxCol : 16 * x + 15;
            result &= map->LinkObjectToExterior(object, box, rect, linkChunks, linkMapObjs);
        }
    }

    return result;
}

// OFFSET: 0x00785D40/0x140474B40
void CWorldMap::LinkStaticEntityMultiple2ToMapObjDefExterior(CMapStaticEntity* entity, CMapObjDef* mapObjDef, CMapObjDefGroup* group) {
    CMapObj* mapObj = mapObjDef->m_mapObj;

    if (!mapObj || !(mapObj->m_flags & 0x40)) {
        return;
    }

    CAaBox local;
    memset(&local, 0, sizeof(local));
    CWorldMath::TransformAABox(mapObjDef->m_invMat, entity->m_bbox, local);

    if (group) {
        CMap::LinkObjectToMapObjDefGroup(entity, group);
    }

    for (CMapObjDefGroup* defGroup = mapObjDef->m_defGroupList.Head(), * next; defGroup; defGroup = next) {
        next = mapObjDef->m_defGroupList.Next(defGroup);

        if (defGroup == group) {
            continue;
        }

        uint32_t groupFlags = mapObj->GetGroupFlags(defGroup->m_groupNum);

        if (!(groupFlags & 0x410080) && (groupFlags & 0x8) && mapObj->TestGroupBounds(local, defGroup->m_groupNum, 1)) {
            CMap::LinkObjectToMapObjDefGroup(entity, defGroup);
        }
    }
}

// OFFSET: 0x00785E30/0x140474C50
void CWorldMap::LinkStaticEntityMultiple2ToMapObjDefInterior(CMapStaticEntity* entity, CMapObjDef* mapObjDef, CMapObjDefGroup* group) {
    CMapObj* mapObj = mapObjDef->m_mapObj;

    if (!mapObj || !(mapObj->m_flags & 0x40)) {
        return;
    }

    CAaBox local;
    memset(&local, 0, sizeof(local));
    CWorldMath::TransformAABox(mapObjDef->m_invMat, entity->m_bbox, local);

    if (group) {
        CMap::LinkObjectToMapObjDefGroup(entity, group);
    }

    for (CMapObjDefGroup* defGroup = mapObjDef->m_defGroupList.Head(), * next; defGroup; defGroup = next) {
        next = mapObjDef->m_defGroupList.Next(defGroup);

        if (defGroup == group) {
            continue;
        }

        uint32_t groupFlags = mapObj->GetGroupFlags(defGroup->m_groupNum);

        if (!(groupFlags & 0x410088) && mapObj->TestGroupBounds(local, defGroup->m_groupNum, 1)) {
            CMap::LinkObjectToMapObjDefGroup(entity, defGroup);
        }
    }
}

// PROVEN: 98eeba89 IDENTICAL/IDENTICAL
// OFFSET: 0x00785F10/0x140474D60
int32_t CWorldMap::LinkMapObjDef(CMapObjDef* mapObjDef) {
    return this->LinkObjectToMapExterior(mapObjDef, mapObjDef->m_bbox, 1, 0);
}

// OFFSET: 0x00785F30/0x140474D90
void CWorldMap::LinkStaticEntity(CMapStaticEntity* entity, C3Vector& start, C3Vector& end, C3Vector& pos, int32_t& exterior, int32_t& found, MapObjIntersectData* front, MapObjIntersectData* back) {
    front[0].m_mapObjDef = NULL;
    back[0].m_mapObjDef = NULL;
    front[1].m_mapObjDef = NULL;
    float height = 100000.0f;
    float limit = 2.0f;
    back[1].m_mapObjDef = NULL;
    CMapChunk* chunk = NULL;
    int32_t terrain = 0;

    if (!this->m_isGlobalMapObj) {
        CMap* map = this->GetMap(start);

        if (map) {
            terrain = map->LinkStaticEntityGetChunk(start, height, chunk);
        } else {
            chunk = NULL;
            terrain = 0;
        }

        limit = (start.z - height) * 0.001f;

        if (limit < 0.0f) {
            terrain = 0;
        }
    }

    if (entity->m_flags & 0x1000) {
        terrain = 1;
    } else {
        bool hit = this->LinkIntersectMapObjDefs(start, end, pos, front, back, chunk);

        if (!terrain && !hit) {
            start = entity->m_vec2;
            end = entity->m_vec2;
            end.z = end.z + 1000.0f;
            this->LinkIntersectMapObjDefs(start, end, pos, front, back, chunk);
        }
    }

    if (entity->m_entityFlags & 0x1000) {
        front[1].m_mapObjDef = NULL;
        back[1].m_mapObjDef = NULL;
    }

    if (terrain) {
        if (front[0].m_t > limit) {
            front[0].m_mapObjDef = NULL;
            back[0].m_mapObjDef = NULL;
        }

        if (front[1].m_t > limit) {
            front[1].m_mapObjDef = NULL;
            back[1].m_mapObjDef = NULL;
        }
    }

    exterior = 0;
    found = 0;

    if (front[0].m_mapObjDef) {
        found = 1;
    }

    if (front[1].m_mapObjDef) {
        found = 1;
    }

    if (front[0].m_mapObjDef) {
        exterior = front[0].m_exterior;
    } else if (front[1].m_mapObjDef) {
        exterior = front[1].m_exterior;
    }
}

// OFFSET: 0x007860E0/0x140475020
void CWorldMap::LinkStaticEntitySingle2(CMapStaticEntity* entity) {
    C3Vector start(entity->m_vec2.x, entity->m_vec2.y, entity->m_vec2.z + 0.1f);
    C3Vector end(entity->m_vec2.x, entity->m_vec2.y, entity->m_vec2.z - 1000.0f);
    int32_t exterior = 0;
    int32_t found = 0;
    MapObjIntersectData front[2];
    MapObjIntersectData back[2];
    this->LinkStaticEntity(entity, start, end, start, exterior, found, front, back);
    uint32_t isEntity = entity->m_type & 0x40;
    uint32_t groundType = 0xFFFFFFFF;

    if (found) {
        uint32_t index = back[0].m_mapObjDef == NULL;

        if (front[0].m_mapObjDef) {
            CMap::LinkObjectToMapObjDefGroup(entity, front[0].m_group);
        }

        if (front[1].m_mapObjDef) {
            CMap::LinkObjectToMapObjDefGroup(entity, front[1].m_group);
        }

        if (isEntity) {
            groundType = back[index].m_mapObjDef->GetGroundType(back[index].m_group->m_groupNum, back[index].m_polyIndex);
            static_cast<CMapEntity*>(entity)->m_groundType = groundType;
        }

        entity->m_flags |= 0x100;
        uint32_t flags = entity->m_flags;

        if (front[0].m_mapObjDef && front[1].m_mapObjDef) {
            entity->m_entityFlags |= 0x10;
        }

        if (exterior) {
            entity->m_staticFlags |= 0x8;
            C3Vector pos(entity->m_vec2.x, entity->m_vec2.y, (end.z - start.z) * back[index].m_t + entity->m_vec2.z);
            ClassifyStaticEntityLink(entity, back[index].m_mapObjDef, back[index].m_group, back[index].m_polyIndex, pos);
            entity->m_entityFlags &= ~0x4000;
            return;
        }

        entity->m_flags = (flags & ~0x3) | 0x1;
    } else {
        this->LinkObjectToMapExterior(entity, entity->m_bbox, 1, 0);

        if (isEntity) {
            if (!this->QueryGroundTypeTerrain(entity->m_position, groundType)) {
                groundType = 0xFFFFFFFF;
            }

            static_cast<CMapEntity*>(entity)->m_groundType = groundType;
        }

        entity->m_flags = (entity->m_flags & ~0x103) | 0x1;
    }

    entity->QueryExteriorLighting();
    entity->m_entityFlags &= ~0x4000;
}

// OFFSET: 0x007862C0/0x1404752E0
void CWorldMap::LinkStaticEntityMultiple2(CMapStaticEntity* entity) {
    C3Vector start(entity->m_vec2.x, entity->m_vec2.y, entity->m_vec2.z + 4.0f);
    C3Vector end(entity->m_vec2.x, entity->m_vec2.y, entity->m_vec2.z - 1000.0f);
    C3Vector pos(entity->m_vec2.x, entity->m_vec2.y, entity->m_vec2.z + 0.15000001f);
    float top = entity->m_bbox.t.z + 0.1f;

    if (start.z > top) {
        start.z = top;
    }

    int32_t exterior = 0;
    int32_t found = 0;
    MapObjIntersectData front[2];
    MapObjIntersectData back[2];
    this->LinkStaticEntity(entity, start, end, pos, exterior, found, front, back);
    int32_t linked = 1;

    if (!(entity->m_flags & 0x1000)) {
        if (found) {
            if (exterior) {
                CWorldMap::LinkStaticEntityMultiple2ToMapObjDefInterior(entity, front[0].m_mapObjDef, front[0].m_group);
                entity->m_staticFlags |= 0x8;
                C3Vector hit(entity->m_vec2.x, entity->m_vec2.y, (end.z - start.z) * back[0].m_t + entity->m_vec2.z);
                ClassifyStaticEntityLink(entity, back[0].m_mapObjDef, back[0].m_group, back[0].m_polyIndex, hit);

                if (entity->m_entityFlags & 0x2000) {
                    CWorldMap::LinkStaticEntityMultiple2ToMapObjDefExterior(entity, front[0].m_mapObjDef, NULL);
                }
            } else {
                if (front[0].m_mapObjDef->m_flags & 0x200) {
                    CWorldMap::LinkStaticEntityMultiple2ToMapObjDefExterior(entity, front[0].m_mapObjDef, front[0].m_group);
                } else {
                    linked = this->LinkObjectToMapExterior(entity, entity->m_bbox, 1, 0) & 1;
                }

                entity->m_flags = (entity->m_flags & ~0x3) | 0x1;
                entity->QueryExteriorLighting();

                if (entity->m_entityFlags & 0x2000) {
                    CWorldMap::LinkStaticEntityMultiple2ToMapObjDefInterior(entity, front[0].m_mapObjDef, NULL);
                }
            }
        } else {
            linked = this->LinkObjectToMapExterior(entity, entity->m_bbox, 1, 0) & 1;
            entity->m_flags = (entity->m_flags & ~0x3) | 0x1;
            entity->QueryExteriorLighting();
        }
    }

    entity->m_entityFlags ^= (entity->m_entityFlags ^ ((linked == 0) << 14)) & 0x4000;
}

// OFFSET: 0x007864B0/0x1404755E0
void CWorldMap::LinkStaticEntity(CMapStaticEntity* entity) {
    for (CMapBaseObjLink* link = entity->m_parentLinkList.Head(), * next; link; link = next) {
        next = entity->m_parentLinkList.Next(link);
        CMap::FreeBaseObjLink(link);
    }

    if (entity->m_type & 0x40) {
        static_cast<CMapEntity*>(entity)->m_groundType = -1;
    }

    if (entity->m_entityFlags & 0x1000) {
        this->LinkStaticEntityMultiple2(entity);
    } else {
        this->LinkStaticEntitySingle2(entity);
    }
}

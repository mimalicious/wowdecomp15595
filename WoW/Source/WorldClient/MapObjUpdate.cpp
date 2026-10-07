#include "WoW/Source/WorldClient/MapObj.h"
#include <cfloat>
#include <cmath>
#include "Engine/Source/Event/FrameTime.h"
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "Engine/Source/Services/AsyncFile.h"
#include "Engine/Source/Services/Texture.h"
#include "WoW/Source/DayNight/DayNight.h"
#include "WoW/Source/WorldClient/Map.h"
#include "WoW/Source/WorldClient/MapObjGroup.h"
#include "WoW/Source/WorldClient/World.h"

// STATIC: 0x00D0174C
float CMapObj::s_closestDistSq = FLT_MAX;

// OFFSET: 0x0079ECD0/0x140493FB0
void CMapObj::UpdateTextures(CMapObjGroup* group) {
    if (group->m_distSq >= 1111.1111f) {
        return;
    }

    bool done = group->m_batchCount == 0;

    for (uint32_t i = 0; i < group->m_batchCount; i++) {
        CMapObjMaterial* material = &this->m_runtimeMaterials[group->m_batches[i].m_materialId];

        for (uint32_t j = 0; j < 3; j++) {
            if (material->m_textures[j] && !TextureIsLoaded(material->m_textures[j])) {
                break;
            }
        }

        done = i + 1 == group->m_batchCount;
    }

    if (done) {
        group->m_renderFlags |= 0x4;
        return;
    }

    int32_t priority = AsyncFileReadComputePriority(2, 1, 1, group->m_distSq);
    AsyncFileReadAddStreamingObject(CMapObjGroup::LinkToAsyncHead, group, priority);
}

// OFFSET: 0x0079ED90/0x1404940B0
void CMapObj::UpdateGroups(CMapObj* mapObj) {
    for (uint32_t i = 0; i < mapObj->m_mapObjGroupCount; i++) {
        CMapObjGroup* group = mapObj->m_groups[i];

        if (World::GetFrameCnt() - group->m_frameStamp >= 120) {
            group->Clear(0);
            continue;
        }

        if (group->m_flags & 0x40) {
            if (!(group->m_renderFlags & 0x4)) {
                mapObj->UpdateTextures(group);
            }

            if (g_theGxDevicePtr->DeviceApi() != GxApi_D3d11 && g_theGxDevicePtr->DeviceApi() != GxApi_GLL) {
                group->m_unusedFrames += FrameTime::GetTickTimeMs();

                if (group->m_unusedFrames >= 2000) {
                    group->FreeVB();
                    group->m_unusedFrames = 2000;
                }
            }
        } else {
            if (CMapObj::s_closestDistSq > group->m_distSq) {
                CMapObj::s_closestDistSq = group->m_distSq;
            }

            if (group->m_asyncObject) {
                int32_t priority = AsyncFileReadComputePriority(group->m_distSq < 1111.1111f ? 0 : 2, 0, 1, group->m_distSq);
                AsyncFileReadAddStreamingObject(CMapObjGroup::LinkToAsyncHead, group, priority);
            }
        }

        group->m_distSq = FLT_MAX;
    }

    DayNight::SetFogDistanceMax(sqrtf(CMapObj::s_closestDistSq));
}

// OFFSET: 0x0079EEC0/0x140494200
void CMapObj::Update() {
    CMapObj::s_closestDistSq = FLT_MAX;
    CMapObj* mapObj = CMapObj::s_mapObjHash.Head();

    while (mapObj) {
        CMapObj* next = CMapObj::s_mapObjHash.Next(mapObj);

        if (!mapObj->m_refCount) {
            CMapObj::s_mapObjHash.Unlink(mapObj);
            CMap::FreeMapObj(mapObj);
        } else if (mapObj->m_flags & 0x40) {
            mapObj->UpdateMaterials();
            CMapObj::UpdateGroups(mapObj);
            mapObj->m_flushTimer = FLT_MAX;
        } else {
            if (mapObj->m_asyncObject) {
                int32_t priority = AsyncFileReadComputePriority(mapObj->m_flushTimer < 1111.1111f ? 0 : 2, 0, 1, mapObj->m_flushTimer);
                AsyncFileReadAddStreamingObject(CMapObj::LinkToAsyncHead, mapObj, priority);
            }

            mapObj->m_flushTimer = FLT_MAX;
        }

        mapObj = next;
    }
}

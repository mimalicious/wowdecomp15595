#include "WoW/Source/Game/GameClient/Minimap.h"
#include <cfloat>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <ctime>
#include <malloc.h>
#include "Engine/Source/Base/Status.h"
#include "Engine/Source/ConsoleBase/ConsoleVar.h"
#include "Engine/Source/Event/FrameTime.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/Gx/CGxTexFlags.h"
#include "Engine/Source/Services/Texture.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/Tempest/CMath.h"
#include "WoW/Source/DB/DBClient/DBCacheInstances.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/AreaPOIRec.h"
#include "WoW/Source/DB/Rec/AreaTableRec.h"
#include "WoW/Source/DB/Rec/MapRec.h"
#include "WoW/Source/DB/Rec/VehicleRec.h"
#include "WoW/Source/DB/Rec/WMOAreaTableRec.h"
#include "WoW/Source/DayNight/DayNight.h"
#include "WoW/Source/Game/GameClient/NameCache.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/Object/ObjectClient/Unit_C.h"
#include "WoW/Source/Object/ObjectClient/VehiclePassenger_C.h"
#include "WoW/Source/Object/ObjectClient/Vehicle_C.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/Ui/BattlefieldInfo.h"
#include "WoW/Source/Ui/CGWorldStateInfo.h"
#include "WoW/Source/Ui/GameUI.h"
#include "WoW/Source/Ui/MinimapFrame.h"
#include "WoW/Source/Ui/PartyFrame.h"
#include "WoW/Source/Ui/RaidInfo.h"
#include "WoW/Source/Ui/WorldMap.h"
#include "WoW/Source/WorldClient/World.h"

// STATIC: 0x00BA5224
static const uint32_t s_outdoorZoomTiles[6] = { 14, 12, 10, 8, 6, 4 };

// STATIC: 0x00BA523C
static const float s_indoorZoomRadius[6] = { 150.0f, 120.0f, 90.0f, 60.0f, 40.0f, 25.0f };

// STATIC: 0x00BA52E4
static const int32_t s_tileOffsets[4][2] = { { 0, 0 }, { 1, 0 }, { 1, 1 }, { 0, 1 } };

// STATIC: 0x00CA2710
static int32_t s_mapId = -1;

// STATIC: 0x00CA2714
static int32_t s_outdoorZoom = 3;

// STATIC: 0x00CA2718
static int32_t s_indoorZoom = 3;

// STATIC: 0x00CA271C
static int32_t s_mapObjAreaGroup = -1;

// STATIC: 0x00CA2720
static const char* s_minimapTileFormat = "World\\Minimaps\\%s\\map%02d_%02d.blp";

// STATIC: 0x00CA2724
static const char* s_minimapNoLiquidTileFormat = "World\\Minimaps\\%s\\noLiquid_map%02d_%02d.blp";

// STATIC: 0x00CA2728
static const char* s_minimapQuadFormat = "World\\Minimaps\\%s_%03d_%02d_%02d.blp";

// STATIC: 0x00CA2730
static C2Vector s_halfTile(266.66666f, 266.66666f);

// STATIC: 0x00CA2738
static C2Vector s_mapOrigin(17066.666f, 17066.666f);

// STATIC: 0x00CA2740
static C44Matrix s_mapObjMatrix;

// STATIC: 0x00CA2780
static C44Matrix s_mapObjInvMatrix;

// STATIC: 0x00CA27FC
static float s_mapObjRotation = 90.0f;

// STATIC: 0x00CA2800
static C2Vector s_tileSize(1066.6666f, 1066.6666f);

// STATIC: 0x00DC7398
static float s_closestPOIFacing[3];

// STATIC: 0x00DC73A4
static int32_t s_closestPOIs[3];

// STATIC: 0x00DC73B0
static int32_t s_poisChanged;

// STATIC: 0x00DC73B4
static uint32_t s_minimapUpdateFlags;

// STATIC: 0x00DC73B8
static char s_dynamicPOINames[4][64];

// STATIC: 0x00DC74B8
static char s_mapObjName[260];

// STATIC: 0x00DC75BC
static uint8_t s_isIndoors;

// STATIC: 0x00DC75C0
static int32_t s_mapObjGroup;

// STATIC: 0x00DC75C4
static int32_t s_mapObjId;

// STATIC: 0x00DC75C8
static int32_t s_noLiquid;

// STATIC: 0x00DC75CC
static uint32_t s_worldMapRefreshTime;

// STATIC: 0x00DC75D0
static uint32_t s_numPOIs;

// STATIC: 0x00DC75D4
static uint32_t s_numClosestPOIs;

// STATIC: 0x00DC75D8
static int32_t s_closestPOIsChanged;

// STATIC: 0x00DC75DC
static uint8_t s_mapObjIndoorArea;

// STATIC: 0x00DC75E0
static C3Vector s_playerPosition;

// STATIC: 0x00DC75EC
static CAaBox s_indoorBounds;

// STATIC: 0x00DC7604
static int32_t s_tileMinX = -1;

// STATIC: 0x00DC7608
static int32_t s_tileMinY = -1;

// STATIC: 0x00DC760C
static int32_t s_tileMaxX = -1;

// STATIC: 0x00DC7610
static int32_t s_tileMaxY = -1;

// STATIC: 0x00DC7614
static TSFixedArray<AreaPOIRec*> s_pois;

// STATIC: 0x00DC7620
static AreaPOIRec s_dynamicPOIs[4];

// STATIC: 0x00DC7780
static TSFixedArray<int32_t> s_poiVisible;

// STATIC: 0x00DC778C
static TSFixedArray<int32_t> s_poiOnEdge;

// STATIC: 0x00DC7798
static TSGrowableArray<AreaPOIRec*> s_visiblePOIs;

// OFFSET: 0x004E8440
static MINIMAPTILE* MinimapFindTile(MINIMAPTILE* tiles, const World::MinimapQuad* quad, int32_t currentGroup, float height) {
    for (uint32_t i = 0; i < 256; i++) {
        MINIMAPTILE* tile = &tiles[i];

        if (tile->m_groupId == quad->m_groupNum && tile->m_x == quad->m_x && tile->m_y == quad->m_y) {
            if (tile->m_groupId == currentGroup) {
                tile->m_flags |= 2;
                tile->m_depth = 3.402823466e38f;
            } else {
                tile->m_flags |= 2;
                tile->m_depth = (quad->m_box.t.z + quad->m_box.b.z) * 0.5f - height;
            }

            return tile;
        }
    }

    return NULL;
}

// OFFSET: 0x004E84E0/0x14012AA60
void MinimapShutdown() {
    s_playerPosition = C3Vector(FLT_MAX, FLT_MAX, FLT_MAX);
    s_mapId = -1;
    s_tileMaxX = -1;
    s_tileMaxY = -1;
    s_tileMinX = -1;
    s_tileMinY = -1;
    s_mapObjId = 0;
    s_mapObjGroup = 0;
    s_mapObjAreaGroup = -1;
    s_minimapUpdateFlags = 0;
}

// OFFSET: 0x004E8560
void MinimapLoadZoom() {
    s_outdoorZoom = g_minimapZoomCVar->m_intValue;
    s_indoorZoom = g_minimapInsideZoomCVar->m_intValue;
    FrameScript_SignalEvent(SCRIPT_EVENT_MINIMAP_UPDATE_ZOOM, NULL);
}

// OFFSET: 0x004E8590/0x14012AB20
void MinimapSetZoom(uint32_t zoom) {
    uint8_t indoors = s_isIndoors;
    int32_t* current = indoors ? &s_indoorZoom : &s_outdoorZoom;
    int32_t previous = *current;
    *current = zoom >= 5 ? 5 : zoom;

    if (previous != *current) {
        s_minimapUpdateFlags |= 0x1;
        (indoors ? g_minimapInsideZoomCVar : g_minimapZoomCVar)->Set(*current, true, false, false, true);
    }
}

// PROVEN: 99488e7c IDENTICAL/NONE
// OFFSET: 0x004E85F0
void MinimapSetForceUpdate() {
    s_minimapUpdateFlags |= 4;
}

// OFFSET: 0x004E8600/0x14012ABA0
int32_t MinimapGetZoom() {
    return s_isIndoors ? s_indoorZoom : s_outdoorZoom;
}

// OFFSET: 0x004E8620/0x14012ABE0
double MinimapGetViewRadius() {
    if (s_isIndoors) {
        return s_indoorZoomRadius[s_indoorZoom];
    }

    return s_outdoorZoomTiles[s_outdoorZoom] * 0.5f * 33.333332f;
}

// OFFSET: 0x004E8680
TSGrowableArray<AreaPOIRec*>* MinimapGetPOI(int32_t* changed) {
    *changed = s_poisChanged;
    s_poisChanged = 0;
    return &s_visiblePOIs;
}

// OFFSET: 0x004E86A0
int32_t MinimapSetNoLiquid(int32_t noLiquid) {
    if (s_mapId || s_playerPosition.x < -8000.0f || s_playerPosition.x > -3735.0f || s_playerPosition.y < 2670.0f || s_playerPosition.x > 8000.0f) {
        noLiquid = 0;
    }

    if (noLiquid != s_noLiquid) {
        s_minimapUpdateFlags |= 4;
        s_noLiquid = noLiquid;
    }

    return noLiquid;
}

// OFFSET: 0x004E8710
AreaPOIRec* MinimapGetGossipPoi() {
    return &s_dynamicPOIs[1];
}

// OFFSET: 0x004E8720
uint8_t MinimapIsIndoors() {
    return s_isIndoors;
}

// OFFSET: 0x004E8730
int32_t MinimapGetOutdoorZoom() {
    return s_outdoorZoom;
}

// OFFSET: 0x004E8740
int32_t MinimapGetIndoorZoom() {
    return s_indoorZoom;
}

// OFFSET: 0x004E8750
float MinimapGetOutdoorZoomTiles(int32_t zoom) {
    if (zoom >= 0 && zoom < sizeof(s_outdoorZoomTiles) / sizeof(s_outdoorZoomTiles[0])) {
        return static_cast<float>(s_outdoorZoomTiles[zoom]);
    }

    return 0.0f;
}

// OFFSET: 0x004E8780
float MinimapGetIndoorZoomRadius(int32_t zoom) {
    if (zoom >= 0 && zoom < sizeof(s_indoorZoomRadius) / sizeof(s_indoorZoomRadius[0])) {
        return s_indoorZoomRadius[zoom];
    }

    return 0.0f;
}

// OFFSET: 0x004E8880
static void SetupMapObjMatrices(uintptr_t entity) {
    World::QueryMapObjMatrix(entity, &s_mapObjMatrix, &s_mapObjInvMatrix);
    float a0 = s_mapObjMatrix.a0;
    float a1 = s_mapObjMatrix.a1;
    float a2 = s_mapObjMatrix.a2;
    float lengthSq = a1 * a1 + a2 * a2 + a0 * a0;

    if (std::fabs(lengthSq - 1.0f) >= 0.00000023841858f) {
        float inv = 1.0f / std::sqrt(lengthSq);
        float scale = inv / std::sqrt(lengthSq);
        s_mapObjMatrix.a0 = a0 * scale;
        s_mapObjMatrix.a1 = a1 * scale;
        s_mapObjMatrix.a2 = scale * a2;
        float b = inv / std::sqrt(s_mapObjMatrix.b2 * s_mapObjMatrix.b2 + s_mapObjMatrix.b1 * s_mapObjMatrix.b1 + s_mapObjMatrix.b0 * s_mapObjMatrix.b0);
        s_mapObjMatrix.b0 = s_mapObjMatrix.b0 * b;
        s_mapObjMatrix.b1 = s_mapObjMatrix.b1 * b;
        s_mapObjMatrix.b2 = s_mapObjMatrix.b2 * b;
        float c = inv / std::sqrt(s_mapObjMatrix.c2 * s_mapObjMatrix.c2 + s_mapObjMatrix.c1 * s_mapObjMatrix.c1 + s_mapObjMatrix.c0 * s_mapObjMatrix.c0);
        s_mapObjMatrix.c0 = s_mapObjMatrix.c0 * c;
        s_mapObjMatrix.c1 = s_mapObjMatrix.c1 * c;
        s_mapObjMatrix.c2 = c * s_mapObjMatrix.c2;
    }

    s_mapObjMatrix.d0 = 0.0f;
    s_mapObjMatrix.d1 = 0.0f;
    s_mapObjMatrix.d2 = 0.0f;
    s_mapObjMatrix.RotateAroundZ(s_mapObjRotation * 0.017453292f);
}

// OFFSET: 0x004E89F0/0x14012AFF0
static void MinimapSetMapObjName(uintptr_t entity) {
    const char* fileName = NULL;
    World::QueryMapObjFileName(entity, fileName);

    if (fileName) {
        fileName += SStrLen("World\\");
        SStrCopy(s_mapObjName, fileName, sizeof(s_mapObjName));
    } else {
        SStrCopy(s_mapObjName, "Dont Crash!", sizeof(s_mapObjName));
    }

    char* extension = SStrChrR(s_mapObjName, '.');

    if (extension) {
        *extension = 0;
    }
}

// OFFSET: 0x004E8AA0
int32_t MinimapBuildSource(const int32_t* tile, char* path, uint32_t size) {
    *path = 0;
    int32_t mapId = World::GetActiveMapLogical(tile[0], tile[1]);

    if (mapId == -1) {
        return World::IsInfiniteOcean() != 0;
    }

    MapRec* map = g_mapDB.GetRecord(mapId);

    if (!map) {
        return 0;
    }

    SStrPrintf(path, size, s_noLiquid ? s_minimapNoLiquidTileFormat : s_minimapTileFormat, static_cast<const char*>(map->m_directory), tile[0], tile[1]);
    return 2;
}

// OFFSET: 0x004E8C30
static void SetMemberData(WOWGUID guid, MEMBERINFO* member, CGPlayer_C* player, uint32_t index, bool raid) {
    float dx = s_playerPosition.x;
    float dy = s_playerPosition.y;
    C3Vector position(0.0f, 0.0f, 0.0f);
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (unit) {
        if (unit->m_vehiclePassenger && unit->m_vehiclePassenger->m_state) {
            for (uint32_t i = 0; i < CGBattlefieldInfo::m_numVehicles; i++) {
                CGUnit_C* vehicle = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(CGBattlefieldInfo::m_vehicleGuids[i], TYPEMASK_UNIT, __FILE__, __LINE__));

                if (!vehicle) {
                    continue;
                }

                VehicleRec* vehicleRec = vehicle->m_vehicle ? vehicle->m_vehicle->m_vehicleRec : NULL;
                bool shown = vehicleRec && (vehicleRec->m_flags & 0x10000000);

                if (vehicle->IsPossessedBy(guid) && shown) {
                    member->m_offMap = 0;
                    member->m_onMap = 0;
                    return;
                }
            }
        }

        dx = dx - unit->GetPosition().x;
        dy = dy - unit->GetPosition().y;
        position = unit->GetPosition();
        member->m_class = unit->m_unit->m_unitFieldBytes0[1];
    } else {
        if (guid == player->m_obj->m_guid) {
            member->m_offMap = 0;
            member->m_onMap = 0;
            return;
        }

        PartyMemberState* state = raid ? CGRaidInfo::GetRemoteStats(guid) : (index < 4 ? CGPartyInfo::GetMemberState(index) : NULL);
        int32_t continentId = -1;

        if (!state) {
            member->m_offMap = 0;
            member->m_onMap = 0;
            return;
        }

        AreaTableRec* area = g_areaTableDB.GetRecord(state->m_zoneID);

        if (area) {
            continentId = area->m_continentID;
        }

        if (continentId != CGPlayer_C::GetNewContinentID() || !(state->m_flags & 1) || (state->m_flags & 0x20)) {
            member->m_offMap = 0;
            member->m_onMap = 0;
            return;
        }

        position.x = static_cast<float>(state->m_posX);
        position.y = static_cast<float>(state->m_posY);
        dx = dx - position.x;
        dy = dy - static_cast<float>(state->m_posY);
        position.z = static_cast<float>(state->m_posZ);

        for (uint32_t i = 0; i < CGBattlefieldInfo::m_numVehicles; i++) {
            CGUnit_C* vehicle = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(CGBattlefieldInfo::GetVehicleGuid(i), TYPEMASK_UNIT, __FILE__, __LINE__));

            if (!vehicle) {
                continue;
            }

            VehicleRec* vehicleRec = vehicle->m_vehicle ? vehicle->m_vehicle->m_vehicleRec : NULL;
            bool shown = vehicleRec && (vehicleRec->m_flags & 0x10000000);

            if (vehicle->IsPossessedBy(guid) && shown) {
                member->m_offMap = 0;
                member->m_onMap = 0;
                return;
            }
        }
    }

    member->m_guid = guid;

    if (guid == player->m_obj->m_guid) {
        SStrCopy(member->m_name, unit->GetUnitName(NULL, 1), sizeof(member->m_name));
    } else {
        WOWGUID requester = WOWGUID();
        NameCache* name = g_nameDBCache.GetRecord(guid.m_guid, requester, NULL, NULL, false);

        if (name) {
            SStrCopy(member->m_name, name->m_name, sizeof(member->m_name));
            member->m_class = name->m_class;
        } else {
            member->m_name[0] = 0;
            member->m_class = 0;
        }
    }

    double range = s_isIndoors ? s_indoorZoomRadius[s_indoorZoom] : s_outdoorZoomTiles[s_outdoorZoom] * 0.5f * 33.333332f;

    if (std::sqrt(dy * dy + dx * dx) / range <= 0.8f) {
        member->m_position.x = position.x;
        member->m_position.y = position.y;
        member->m_offMap = 0;
        member->m_onMap = 1;
        member->m_position.z = position.z;
    } else {
        member->m_onMap = 0;
        member->m_offMap = 1;
        member->m_facing = CalculateFacingTo(&s_playerPosition, &position);
    }
}

// OFFSET: 0x004E9080
void MinimapGetPartyMembers(MEMBERINFO* members) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return;
    }

    for (uint32_t i = 0; i < 4; i++) {
        WOWGUID guid = CGPartyInfo::GetMember(i);

        if (guid) {
            SetMemberData(guid, &members[i], player, i, false);
        } else {
            members[i].m_onMap = 0;
            members[i].m_offMap = 0;
        }
    }
}

// OFFSET: 0x004E90F0
void MinimapGetRaidMembers(MEMBERINFO* members) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return;
    }

    CGRaidMember* self = CGRaidInfo::FindMemberByGuid(player->m_obj->m_guid);

    for (uint32_t i = 0; i < 40; i++) {
        CGRaidMember* raidMember;

        if (i < CGRaidInfo::m_numMembers && (raidMember = CGRaidInfo::m_members[i]) != NULL && self && raidMember->m_guid && raidMember->m_subgroup != self->m_subgroup) {
            SetMemberData(raidMember->m_guid, &members[i], player, i, true);
        } else {
            members[i].m_onMap = 0;
            members[i].m_offMap = 0;
        }
    }
}

// OFFSET: 0x004E91F0/0x14012B880
void UpdatePointsOfInterest() {
    if (s_mapId == -1) {
        return;
    }

    s_visiblePOIs.SetCount(0);
    float viewRadius = static_cast<float>(MinimapGetViewRadius());

    for (uint32_t i = 0; i < s_numPOIs; i++) {
        AreaPOIRec* poi = s_pois.m_data[i];

        if (poi->m_pos[0] == 0.0f && poi->m_pos[1] == 0.0f) {
            if (!s_poiVisible.m_data[i] && !s_poiOnEdge.m_data[i]) {
                continue;
            }

            s_poisChanged = 1;
            s_poiVisible.m_data[i] = 0;
            s_poiOnEdge.m_data[i] = 0;
            continue;
        }

        if (!poi->m_worldStateID || GetWorldState(poi->m_worldStateID)) {
            C2Vector delta(s_playerPosition);
            delta.x = delta.x - s_pois.m_data[i]->m_pos[0];
            delta.y = delta.y - s_pois.m_data[i]->m_pos[1];

            if (sqrt(delta.x * delta.x + delta.y * delta.y) / viewRadius > 0.8f) {
                if (s_poiVisible.m_data[i] || !s_poiOnEdge.m_data[i]) {
                    s_poiVisible.m_data[i] = 0;
                    s_poisChanged = 1;
                    s_poiOnEdge.m_data[i] = 1;
                }

                continue;
            }

            if (s_pois.m_data[i]->m_flags & 2) {
                *s_visiblePOIs.New() = s_pois.m_data[i];

                if (!s_poiVisible.m_data[i] || s_poiOnEdge.m_data[i]) {
                    s_poisChanged = 1;
                    s_poiVisible.m_data[i] = CGMinimapFrame::IsTrackingDynamicPOI() != 0;
                    s_poiOnEdge.m_data[i] = 0;
                }

                continue;
            }
        }

        if (s_poiVisible.m_data[i] || s_poiOnEdge.m_data[i]) {
            s_poiVisible.m_data[i] = 0;
            s_poisChanged = 1;
            s_poiOnEdge.m_data[i] = 0;
        }
    }

    int32_t worst = -1;
    uint32_t count = 0;
    float distances[3] = { 694.44446f, 694.44446f, 694.44446f };
    int32_t indices[3] = { -1, -1, -1 };
    int32_t importance[3];

    for (uint32_t i = 0; i < s_numPOIs; i++) {
        if (!s_poiOnEdge.m_data[i]) {
            continue;
        }

        if (!CGMinimapFrame::IsTrackingDynamicPOI() && s_pois.m_data[i] != &s_dynamicPOIs[2] && s_pois.m_data[i] != &s_dynamicPOIs[1]) {
            continue;
        }

        AreaPOIRec* poi = s_pois.m_data[i];
        C2Vector origin(s_playerPosition);
        float dx = poi->m_pos[0] - origin.x;
        float dy = poi->m_pos[1] - origin.y;
        float distance = sqrt(dx * dx + dy * dy);

        if (distance > 694.44446f && s_pois.m_data[i] != &s_dynamicPOIs[2]) {
            continue;
        }

        if (worst == -1) {
            distances[count] = distance;
            indices[count] = i;
            importance[count] = s_pois.m_data[i]->m_importance;
            count++;
        } else if (s_pois.m_data[i]->m_importance < importance[worst] || (s_pois.m_data[i]->m_importance == importance[worst] && distance < distances[worst])) {
            importance[worst] = s_pois.m_data[i]->m_importance;
            indices[worst] = i;
            distances[worst] = distance;
        }

        if (count == 3) {
            worst = 0;

            for (int32_t j = 1; j < 3; j++) {
                if (importance[j] > importance[worst] || (importance[j] == importance[worst] && distances[j] > distances[worst])) {
                    worst = j;
                }
            }
        }
    }

    if (s_numClosestPOIs != count) {
        s_numClosestPOIs = count;
        s_closestPOIsChanged = 1;
    } else {
        for (uint32_t j = 0; j < count; j++) {
            if (indices[j] != s_closestPOIs[j]) {
                s_closestPOIsChanged = 1;
                break;
            }
        }
    }

    for (uint32_t j = 0; j < count; j++) {
        s_closestPOIs[j] = indices[j];
        C2Vector origin(s_playerPosition);
        AreaPOIRec* poi = s_pois.m_data[indices[j]];
        C3Vector position(poi->m_pos[0], poi->m_pos[1], s_playerPosition.z);
        s_closestPOIFacing[j] = CalculateFacingTo(&s_playerPosition, &position);
    }
}

// OFFSET: 0x004E95D0
void RefreshWorldStatePOIs(uint32_t state) {
    for (uint32_t i = 0; i < s_numPOIs; i++) {
        AreaPOIRec* poi = s_pois.m_data[i];

        if (0.0f != poi->m_pos[0] && 0.0f != poi->m_pos[1] && poi->m_worldStateID == static_cast<int32_t>(state)) {
            UpdatePointsOfInterest();
            s_poisChanged = 1;
            return;
        }
    }
}

// OFFSET: 0x004E9640
void MinimapSetDynamicPOI(int32_t index, int32_t flags, float x, float y, int32_t icon, int32_t importance, const char* name) {
    s_dynamicPOIs[index].m_flags = flags;
    s_dynamicPOIs[index].m_pos[0] = x;
    s_dynamicPOIs[index].m_pos[1] = y;
    s_dynamicPOIs[index].m_continentID = s_mapId;
    s_dynamicPOIs[index].m_importance = importance;
    s_dynamicPOIs[index].m_icon[0] = icon;
    SStrCopy(s_dynamicPOINames[index], name, sizeof(s_dynamicPOINames[index]));

    if (index == 1) {
        s_worldMapRefreshTime = static_cast<uint32_t>(time(NULL)) + 480;

        if (CGWorldMap::m_currentOrphanZone == -1) {
            CGWorldMap::SetMap(CGWorldMap::m_currentContinent, CGWorldMap::m_currentZone, CGWorldMap::m_currentOrphanZone);
        } else {
            CGWorldMap::SetMap(CGWorldMap::m_currentContinent, CGWorldMap::m_currentOrphanZone, CGWorldMap::m_currentDungeonMap);
        }
    }

    UpdatePointsOfInterest();
    s_closestPOIsChanged = 1;
}

// OFFSET: 0x004E9720
void DisplayQuestPoint(float x, float y, int32_t importance, const char* name) {
    MinimapSetDynamicPOI(0, 0, x, y, 6, importance, name);
}

// OFFSET: 0x004E9750
void MinimapSetCorpsePOI(float x, float y) {
    MinimapSetDynamicPOI(2, 2, x, y, 8, -1, FrameScript_GetText("CORPSE_RED", -1, GENDER_NOT_APPLICABLE));
}

// OFFSET: 0x004E97E0
int32_t MinimapBuildPOIDirectionData(TSGrowableArray<POIDIRECTIONDATA>* directions) {
    if (!s_closestPOIsChanged) {
        for (uint32_t i = 0; i < s_numClosestPOIs; i++) {
            directions->m_data[i].m_facing = s_closestPOIFacing[i];
        }

        return 0;
    }

    directions->SetCount(s_numClosestPOIs);
    s_closestPOIsChanged = 0;

    for (uint32_t i = 0; i < s_numClosestPOIs; i++) {
        POIDIRECTIONDATA* direction = &directions->m_data[i];
        AreaPOIRec* poi = s_pois.m_data[s_closestPOIs[i]];
        SStrCopy(direction->m_name, poi->m_name, sizeof(direction->m_name));
        direction->m_facing = s_closestPOIFacing[i];
        poi = s_pois.m_data[s_closestPOIs[i]];
        direction->m_alwaysShow = poi == &s_dynamicPOIs[1] || poi == &s_dynamicPOIs[2] || poi == &s_dynamicPOIs[0] || (poi->m_flags & 0x100);
        direction->m_dynamicIndex = -2;

        for (uint32_t j = 0; j < 4; j++) {
            if (s_pois.m_data[s_closestPOIs[i]] == &s_dynamicPOIs[j]) {
                direction->m_dynamicIndex = j;
                break;
            }
        }
    }

    return 1;
}

// OFFSET: 0x004E9980/0x14012C440
int32_t MinimapInitialize(int32_t mapId) {
    s_numPOIs = 0;

    for (int32_t pass = 0; pass < 2; pass++) {
        if (pass > 0) {
            s_pois.SetCount(s_numPOIs);
            s_poiVisible.SetCount(s_numPOIs);
            s_poiOnEdge.SetCount(s_numPOIs);
            memset(s_poiVisible.m_data, 0, sizeof(int32_t) * s_poiVisible.m_count);
            memset(s_poiOnEdge.m_data, 0, sizeof(int32_t) * s_poiOnEdge.m_count);
        }

        uint32_t count = 0;

        for (int32_t i = g_areaPOIDB.m_numRecords - 1; i >= 0; i--) {
            AreaPOIRec* rec = g_areaPOIDB.GetRecordByIndex(i);

            if (rec->m_continentID == mapId && (rec->m_flags & 1)) {
                if (pass > 0) {
                    s_pois.m_data[count] = rec;
                }

                count++;
            }
        }

        s_numPOIs = count + 4;
    }

    for (uint32_t i = 0; i < 4; i++) {
        s_pois.m_data[s_numPOIs - 4 + i] = &s_dynamicPOIs[i];
        memset(&s_dynamicPOIs[i], 0, sizeof(s_dynamicPOIs[i]));
        s_dynamicPOIs[i].m_name = s_dynamicPOINames[i];
    }

    s_closestPOIFacing[0] = -1.0f;
    s_closestPOIFacing[1] = -1.0f;
    s_closestPOIFacing[2] = -1.0f;
    s_closestPOIs[0] = -1;
    s_closestPOIs[1] = -1;
    s_closestPOIs[2] = -1;
    s_outdoorZoom = g_minimapZoomCVar->m_intValue;
    s_indoorZoom = g_minimapInsideZoomCVar->m_intValue;
    FrameScript_SignalEvent(SCRIPT_EVENT_MINIMAP_UPDATE_ZOOM, NULL);
    return 1;
}

// OFFSET: 0x004E9BF0/0x14012C730
static void SetupTextureHandles(const int32_t* tile, int32_t reset, MINIMAPTILE* tiles) {
    tiles[0].m_flags &= ~2u;
    tiles[1].m_flags &= ~2u;
    tiles[2].m_flags &= ~2u;
    tiles[3].m_flags &= ~2u;
    CTexture* released[4] = { NULL, NULL, NULL, NULL };

    if (!reset) {
        for (uint32_t i = 0; i < 4; i++) {
            int32_t x = tile[0] + s_tileOffsets[i][0];
            int32_t y = tile[1] + s_tileOffsets[i][1];

            for (uint32_t j = 0; j < 4; j++) {
                if (tiles[j].m_x == x && tiles[j].m_y == y) {
                    if (j != i) {
                        if (tiles[i].m_texture) {
                            released[i] = tiles[i].m_texture;
                        }

                        tiles[i].m_texture = tiles[j].m_texture;
                        tiles[i].m_x = x;
                        tiles[i].m_y = y;
                        tiles[j].m_texture = NULL;
                    }

                    tiles[i].m_flags |= 2;
                }
            }
        }
    }

    for (uint32_t i = 0; i < 4; i++) {
        if (tiles[i].m_flags & 2) {
            continue;
        }

        int32_t source[2];
        source[0] = tile[0] + s_tileOffsets[i][0];
        source[1] = tile[1] + s_tileOffsets[i][1];
        char path[260];
        int32_t kind = MinimapBuildSource(source, path, sizeof(path));

        if (!kind) {
            SysMsgPrintf(STATUS_ERROR, 2, "NOCONTINENTNAME|%d", s_mapId);
            SysMsgPrintf(STATUS_ERROR, 4, "MINIMAPCHUNKNOTFOUND|%d|%d", source[0], source[1]);
            continue;
        }

        if (tiles[i].m_texture) {
            released[i] = tiles[i].m_texture;
            tiles[i].m_texture = NULL;
        }

        CGxTexFlags texFlags(GxTex_Linear, 0, 0, 0, 0, 1);
        CStatus status;

        if (kind == 2) {
            tiles[i].m_texture = TextureCreate(path, texFlags, &status, 1, NULL, 2);
        } else if (kind == 1) {
            uint32_t now = FrameTime::GetCurTimeMs();
            C3Vector position(17066.666f - source[1] * 533.33331f, s_mapOrigin.y - source[0] * 533.33331f, 0.0f);
            CImVector color;
            DayNight::GetLiquidMinimapColor(&color, now, position, 255, 0);
            tiles[i].m_texture = TextureCreateSolid(color);
        }

        tiles[i].m_flags |= 2;
        tiles[i].m_x = source[0];
        tiles[i].m_y = source[1];
    }

    for (uint32_t i = 0; i < 4; i++) {
        if (released[i]) {
            TextureRelease(released[i]);
            released[i] = NULL;
        }
    }
}

// OFFSET: 0x004E9E80
static void SetupQuad(MINIMAPTILE* tiles, const World::MinimapQuad* quad, int32_t currentGroup, float height, const char* mapObjName) {
    uint32_t index = 0;

    while (tiles->m_flags & 2) {
        index++;
        tiles++;

        if (index >= 256) {
            return;
        }
    }

    tiles->m_flags |= 2;
    char path[260];
    SStrPrintf(path, sizeof(path), s_minimapQuadFormat, mapObjName, quad->m_groupNum, quad->m_x, quad->m_y);

    if (tiles->m_texture) {
        TextureRelease(tiles->m_texture);
    }

    if (path[0]) {
        CStatus status;
        CTexture* texture = TextureCreate(path, CGxTexFlags(GxTex_Linear, 0, 0, 0, 0, 1), &status, 1, NULL, 2);
        tiles->m_flags |= 4;
        tiles->m_texture = texture;
    } else {
        tiles->m_flags &= ~2u;
        tiles->m_texture = NULL;
    }

    tiles->m_x = quad->m_x;
    tiles->m_y = quad->m_y;
    tiles->m_bounds = quad->m_box;
    tiles->m_groupId = quad->m_groupNum;

    if (tiles->m_groupId == currentGroup) {
        tiles->m_depth = 3.402823466e38f;
    } else {
        tiles->m_depth = (tiles->m_bounds.t.z + tiles->m_bounds.b.z) * 0.5f - height;
    }
}

// INLINED
static void MinimapWorldToTileCoords(const C3Vector* position, int32_t* tile) {
    float y = (17066.666f - position->y) * 0.0018750001f;
    float x = 0.0018750001f * (17066.666f - position->x);

    if (y < 0.0f) {
        y = y - 1.0f;
    }

    if (x < 0.0f) {
        x = x - 1.0f;
    }

    tile[0] = static_cast<int32_t>(y);
    tile[1] = static_cast<int32_t>(x);
}

// OFFSET: 0x004E9FF0
static int32_t MinimapUpdatePosition(int32_t mapId, const C3Vector* position, C2Vector* center, float* scale, MINIMAPTILE* tiles) {
    int32_t mapChanged = (s_minimapUpdateFlags & 4) | (mapId != s_mapId);
    s_mapId = mapId;
    s_playerPosition = *position;
    UpdatePointsOfInterest();

    C3Vector maxPosition(position->x + s_halfTile.x, position->y + s_halfTile.y, 0.0f);
    int32_t baseTile[2];
    MinimapWorldToTileCoords(&maxPosition, baseTile);
    C3Vector minPosition(position->x - s_halfTile.x, position->y - s_halfTile.y, 0.0f);
    int32_t endTile[2];
    MinimapWorldToTileCoords(&minPosition, endTile);

    int32_t endX = endTile[0];

    if (baseTile[0] == endX) {
        if (endX + 1 >= 64) {
            baseTile[0]--;
        } else {
            endX = endX + 1;
        }
    }

    int32_t endY = endTile[1];

    if (baseTile[1] == endY) {
        if (endY + 1 >= 64) {
            baseTile[1]--;
        } else {
            endY = endY + 1;
        }
    }

    if (mapChanged || (s_minimapUpdateFlags & 1) || s_tileMinX != baseTile[0] || s_tileMinY != baseTile[1] || s_tileMaxX != endX || s_tileMaxY != endY) {
        SetupTextureHandles(baseTile, mapChanged, tiles);
        s_tileMinX = baseTile[0];
        s_tileMinY = baseTile[1];
        s_tileMaxX = endX;
        s_tileMaxY = endY;
    }

    s_minimapUpdateFlags &= ~5u;
    C3Vector origin(17066.666f - baseTile[1] * 533.33331f, s_mapOrigin.y - 533.33331f * baseTile[0], 0.0f);
    C2Vector origin2(origin);
    float inv = 1.0f / s_tileSize.y;
    uint32_t halfTiles = s_outdoorZoomTiles[s_outdoorZoom] >> 1;
    float centerX = (origin2.y - position->y) * inv;
    float centerY = (origin2.x - position->x) / s_tileSize.x;
    *scale = inv * (halfTiles * 33.333332f);
    center->x = centerX;
    center->y = centerY;
    return 1;
}

// OFFSET: 0x004EA250
int32_t MinimapUpdate(uintptr_t entity, int32_t mapId, const C3Vector* position, C2Vector* center, float* scale, MINIMAPTILE* tiles, MINIMAPINDOOR* indoor, int32_t force) {
    bool changed = (s_minimapUpdateFlags & 3) != 0;
    bool result = changed;
    uint8_t reload = s_minimapUpdateFlags & 1;

    if (s_worldMapRefreshTime && time(NULL) >= static_cast<time_t>(s_worldMapRefreshTime)) {
        s_dynamicPOIs[1].m_pos[0] = 0.0f;
        s_dynamicPOIs[1].m_pos[1] = 0.0f;
        s_worldMapRefreshTime = 0;
        result = true;
        CGWorldMap::RefreshCurrentMap();
        changed = result;
    }

    C3Vector delta(0.0f, 0.0f, 0.0f);
    int32_t inside;

    if (position->x != s_playerPosition.x || position->y != s_playerPosition.y || mapId != s_mapId || (s_minimapUpdateFlags & 4)) {
        delta.x = position->x - s_playerPosition.x;
        delta.y = position->y - s_playerPosition.y;
        delta.z = position->z - s_playerPosition.z;
        s_playerPosition = *position;
        s_mapId = mapId;
        result = true;

        const WMOAreaTableRec* groupRec = NULL;
        const WMOAreaTableRec* rootRec = NULL;
        int32_t outdoorArea = 0;
        inside = World::QueryObjectInside(entity, 0);
        uint32_t groupId;
        World::QueryMapObjAreaTable(entity, groupRec, rootRec, groupId, 0);
        uint8_t indoorArea = 0;

        if (rootRec) {
            if (rootRec->m_flags & 1) {
                outdoorArea = 1;
            }

            if (rootRec->m_flags & 0x18) {
                indoorArea = 1;
            }
        }

        s_mapObjIndoorArea = indoorArea;
        uint32_t wmoId;
        uint32_t wmoHash;

        if (groupRec && (groupRec->m_flags & 0x20)) {
            inside = 0;
            s_mapObjIndoorArea = 0;
        } else if ((outdoorArea || indoorArea || inside) && World::QueryMapObjIDs(entity, wmoId, wmoHash, groupId)) {
            indoor->m_active = 1;

            if (wmoId != s_mapObjId || wmoHash != s_mapObjGroup) {
                reload = 1;
                SetupMapObjMatrices(entity);
                MinimapSetMapObjName(entity);

                for (uint32_t i = 0; i < 256; i++) {
                    tiles[i].m_groupId = -1;
                }

                s_mapObjId = wmoId;
                s_mapObjGroup = wmoHash;
            }

            if (groupId != s_mapObjAreaGroup) {
                reload = 1;
                s_mapObjAreaGroup = groupId;
            }

            if (position->x <= s_indoorBounds.b.x || position->y <= s_indoorBounds.b.y || s_indoorBounds.t.x <= position->x || s_indoorBounds.t.y <= position->y) {
                reload = 1;
            }

            goto indoorChecked;
        }

        indoor->m_active = 0;
        s_mapObjAreaGroup = -1;
        s_mapObjGroup = -1;
        s_mapObjId = -1;

    indoorChecked:
        if (s_isIndoors != indoor->m_active) {
            s_minimapUpdateFlags |= 1;
            s_isIndoors = indoor->m_active;
            FrameScript_SignalEvent(SCRIPT_EVENT_MINIMAP_UPDATE_ZOOM, NULL);
        }
    } else {
        if (!changed) {
            goto done;
        }

        inside = World::QueryObjectInside(entity, 0);
    }

    if (indoor->m_active) {
        indoor->m_position = *position;
        SetupMapObjMatrices(entity);

        if (reload) {
            float radius = s_indoorZoomRadius[s_indoorZoom];
            float inv = 1.0f / radius;
            int32_t tileX = static_cast<int32_t>(std::floor(position->x * inv));
            int32_t tileY = static_cast<int32_t>(std::floor(position->y * inv));
            float minX = tileX * radius;
            float minY = tileY * radius;
            float half = radius * 0.5f;
            s_indoorBounds.b.x = minX;
            s_indoorBounds.b.y = minY;
            s_indoorBounds.b.z = position->z - half;
            s_indoorBounds.t.x = minX + radius;
            s_indoorBounds.t.y = minY + radius;
            s_indoorBounds.t.z = half + s_indoorBounds.b.z;

            World::MinimapQuad* buffer = static_cast<World::MinimapQuad*>(alloca(256 * sizeof(World::MinimapQuad)));
            TSStackArray<World::MinimapQuad> quads(buffer, 0, 0);
            CAaBox box = s_indoorBounds;
            box.Expand(radius);
            World::QueryMapObjMinimap(entity, box, quads, 256, inside != 0, s_mapObjIndoorArea);
            C3Vector local = *position * s_mapObjInvMatrix;

            for (uint32_t i = 0; i < 256; i++) {
                tiles[i].m_flags &= ~2u;
            }

            if (quads.m_count) {
                int32_t currentGroup = quads.m_data[0].m_groupNum;

                for (uint32_t i = 0; i < quads.m_count; i++) {
                    if (MinimapFindTile(tiles, &quads.m_data[i], currentGroup, local.z)) {
                        quads.m_data[i].m_groupNum = -1;
                    }
                }

                for (uint32_t i = 0; i < quads.m_count; i++) {
                    if (quads.m_data[i].m_groupNum != -1) {
                        SetupQuad(tiles, &quads.m_data[i], currentGroup, local.z, s_mapObjName);
                    }
                }
            }

            s_minimapUpdateFlags = (s_minimapUpdateFlags & ~7u) | 2;
        }

        if (s_minimapUpdateFlags & 2) {
            for (uint32_t i = 0; i < 256; i++) {
                uint32_t flags = tiles[i].m_flags;

                if ((flags & 2) && (flags & 4)) {
                    if (tiles[i].m_texture) {
                        if (!TextureGetGxTex(tiles[i].m_texture)) {
                            goto texturesPending;
                        }

                        tiles[i].m_flags &= ~4u;
                    } else {
                        tiles[i].m_flags = flags & ~2u;
                    }
                }
            }

            C3Vector boxCenter;
            boxCenter.x = (s_indoorBounds.t.x + s_indoorBounds.b.x) * 0.5f;
            indoor->m_center.x = boxCenter.x;
            boxCenter.y = (s_indoorBounds.t.y + s_indoorBounds.b.y) * 0.5f;
            indoor->m_center.y = boxCenter.y;
            boxCenter.z = (s_indoorBounds.t.z + s_indoorBounds.b.z) * 0.5f;
            indoor->m_center.z = boxCenter.z;
            indoor->m_matrix = s_mapObjMatrix;
            s_minimapUpdateFlags &= ~2u;
            indoor->m_invMatrix = s_mapObjInvMatrix;
            indoor->m_dirty = 1;
            indoor->m_radius = s_indoorZoomRadius[s_indoorZoom];
        }

    texturesPending:
        if (!reload && indoor->m_invMatrix != s_mapObjInvMatrix) {
            bool notPending = !(s_minimapUpdateFlags & 2);
            indoor->m_center.x = indoor->m_center.x + delta.x;
            indoor->m_center.y = delta.y + indoor->m_center.y;
            indoor->m_center.z = delta.z + indoor->m_center.z;
            indoor->m_matrix = s_mapObjMatrix;
            indoor->m_invMatrix = s_mapObjInvMatrix;

            if (notPending) {
                indoor->m_dirty = 1;
            }
        }

        UpdatePointsOfInterest();
    } else {
        result = MinimapUpdatePosition(mapId, position, center, scale, tiles) != 0;
    }

    if (s_dynamicPOIs[1].m_pos[0] != 0.0f || s_dynamicPOIs[1].m_pos[1] != 0.0f) {
        C2Vector current(*position);
        float dx = current.x - s_dynamicPOIs[1].m_pos[0];
        float dy = current.y - s_dynamicPOIs[1].m_pos[1];

        if (dx * dx + dy * dy < 100.0f) {
            s_dynamicPOIs[1].m_pos[0] = 0.0f;
            s_dynamicPOIs[1].m_pos[1] = 0.0f;
            s_worldMapRefreshTime = 0;

            if (CGWorldMap::m_currentOrphanZone == -1) {
                CGWorldMap::SetMap(CGWorldMap::m_currentContinent, CGWorldMap::m_currentZone, CGWorldMap::m_currentOrphanZone);
            } else {
                CGWorldMap::SetMap(CGWorldMap::m_currentContinent, CGWorldMap::m_currentOrphanZone, CGWorldMap::m_currentDungeonMap);
            }
        }
    }

    if (result) {
        return result;
    }

    changed = false;

done:
    if (force && !indoor->m_active) {
        MinimapUpdatePosition(mapId, position, center, scale, tiles);
    }

    return changed;
}

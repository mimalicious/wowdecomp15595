#include "WoW/Source/DayNight/DayNight.h"
#include <cfloat>
#include <cmath>
#include "Engine/Source/Tempest/NTempest.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/LightRec.h"
#include "WoW/Source/DB/Rec/ZoneLightPointRec.h"
#include "WoW/Source/DB/Rec/ZoneLightRec.h"

// STATIC: 0x00E43310
TSGrowableArray<DayNight::CZoneLight> DayNight::g_zoneLights;

static uint32_t FindZoneLight(int32_t id) {
    int32_t high = DayNight::g_zoneLights.m_count - 1;
    int32_t low = 0;
    int32_t index = 0;

    while (low <= high) {
        index = (low + high) >> 1;
        int32_t zoneId = DayNight::g_zoneLights.m_data[index].m_id;

        if (id == zoneId) {
            break;
        }

        if (id <= zoneId) {
            high = index - 1;
        } else {
            low = index + 1;
        }
    }

    return index;
}

// OFFSET: 0x00652630/0x1402F27A0
int32_t DayNight::PointInZone(const CZoneLight& zone, const C2Vector& point, float& dist) {
    int32_t inside = 0;
    float minDist = FLT_MAX;

    for (int32_t i = 0, j = zone.m_pointCount - 1; i < static_cast<int32_t>(zone.m_pointCount); j = i++) {
        const ZoneLightPointRec* a = zone.m_points.m_data[i];
        const ZoneLightPointRec* b = zone.m_points.m_data[j];
        float edgeDist = NTempest::DistanceSquaredFromEdge(*reinterpret_cast<const C2Vector*>(a->m_pos), *reinterpret_cast<const C2Vector*>(b->m_pos), point);

        if (minDist > edgeDist) {
            minDist = edgeDist;
        }

        float ay = a->m_pos[1];
        float by = b->m_pos[1];

        if ((point.y > ay && by >= point.y) || (point.y > by && ay >= point.y)) {
            if (point.x > (point.y - ay) / (by - ay) * (b->m_pos[0] - a->m_pos[0]) + a->m_pos[0]) {
                inside = !inside;
            }
        }
    }

    dist = sqrt(minDist);
    return inside;
}

// OFFSET: 0x00652770/0x1402F28D0
void DayNight::FindAndAddZoneLight() {
    C2Vector pos(DayNight::g_dnInfo.m_camPos.x, DayNight::g_dnInfo.m_camPos.y);
    int32_t mapId = DayNight::g_dnInfo.m_mapId;

    for (uint32_t i = 0; i < DayNight::g_zoneLights.m_count; i++) {
        const CZoneLight& zone = DayNight::g_zoneLights.m_data[i];
        ZoneLightRec* rec = g_zoneLightDB.GetRecord(zone.m_id);

        if (rec->m_mapID != mapId || zone.m_min.x > pos.x || zone.m_min.y > pos.y || pos.x > zone.m_max.x || pos.y > zone.m_max.y) {
            continue;
        }

        float dist = FLT_MAX;

        if (DayNight::PointInZone(zone, pos, dist)) {
            dist = -dist;
        }

        dist = dist - 50.0f;

        if (dist < 0.0f) {
            DayNight::g_dnInfo.AddZoneLight(rec->m_lightID, -dist);
        }
    }
}

// OFFSET: 0x00652C30/0x1402F2E50
void DayNight::UpdateZoneLights() {
    if (!DayNight::g_zoneLightsDirty) {
        return;
    }

    int32_t count = g_zoneLightDB.m_numRecords;
    DayNight::g_zoneLightsDirty = 0;
    DayNight::g_zoneLights.SetCount(0);

    if (!count) {
        return;
    }

    DayNight::g_zoneLights.SetCount(count);
    uint32_t index = 0;

    for (int32_t id = g_zoneLightDB.m_minID; id <= g_zoneLightDB.m_maxID; id++) {
        ZoneLightRec* rec = g_zoneLightDB.GetRecord(id);

        if (rec) {
            DayNight::g_zoneLights.m_data[index++].m_id = rec->m_id;
        }
    }

    for (int32_t id = g_zoneLightPointDB.m_minID; id <= g_zoneLightPointDB.m_maxID; id++) {
        ZoneLightPointRec* point = g_zoneLightPointDB.GetRecord(id);

        if (point) {
            DayNight::g_zoneLights.m_data[FindZoneLight(point->m_zoneLightID)].m_pointCount++;
        }
    }

    for (int32_t i = 0; i < count; i++) {
        CZoneLight& zone = DayNight::g_zoneLights.m_data[i];
        zone.m_points.SetCount(zone.m_pointCount);
    }

    for (int32_t id = g_zoneLightPointDB.m_minID; id <= g_zoneLightPointDB.m_maxID; id++) {
        ZoneLightPointRec* point = g_zoneLightPointDB.GetRecord(id);

        if (!point) {
            continue;
        }

        CZoneLight& zone = DayNight::g_zoneLights.m_data[FindZoneLight(point->m_zoneLightID)];
        zone.m_points.m_data[point->m_pointOrder - 1] = point;
        zone.m_max.x = zone.m_max.x <= point->m_pos[0] ? point->m_pos[0] : zone.m_max.x;
        zone.m_max.y = zone.m_max.y <= point->m_pos[1] ? point->m_pos[1] : zone.m_max.y;
        zone.m_min.x = point->m_pos[0] <= zone.m_min.x ? point->m_pos[0] : zone.m_min.x;
        zone.m_min.y = point->m_pos[1] <= zone.m_min.y ? point->m_pos[1] : zone.m_min.y;
    }
}

// PROVEN: b15ccd3c IDENTICAL/NONE
// OFFSET: 0x00652F00/0x1402F31E0
void DayNight::DNInfo::ClearZoneLights() {
    this->m_zoneLights[0] = NULL;
    this->m_zoneLightBlend[0] = 0.0f;
    this->m_zoneLights[1] = NULL;
    this->m_zoneLightBlend[1] = 0.0f;
    this->m_zoneLights[2] = NULL;
    this->m_zoneLightBlend[2] = 0.0f;
    this->m_zoneLights[3] = NULL;
    this->m_zoneLightBlend[3] = 0.0f;
    this->m_zoneLights[4] = NULL;
    this->m_zoneLightBlend[4] = 0.0f;
    this->m_zoneLightCount = 0;
}

// PROVEN: 7c3d27ec IDENTICAL/NONE
// OFFSET: 0x00652F60/0x1402F3220
void DayNight::DNInfo::AddZoneLight(int32_t lightId, float blend) {
    if (this->m_zoneLightCount >= 5) {
        return;
    }

    LightRec* light = g_lightDB.GetRecord(lightId);

    if (light) {
        this->m_zoneLights[this->m_zoneLightCount] = light;
        this->m_zoneLightBlend[this->m_zoneLightCount++] = blend;
    }
}

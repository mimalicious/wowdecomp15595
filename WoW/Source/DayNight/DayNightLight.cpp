#include "WoW/Source/DayNight/DayNight.h"
#include <algorithm>
#include <cmath>
#include <cstring>
#include <vector>
#include "Engine/Source/Tempest/C4Vector.h"
#include "Engine/Source/Tempest/CMath.h"
#include "Engine/Source/Tempest/NTempest.h"
#include "WoW/Source/DayNight/DNClouds.h"
#include "WoW/Source/DayNight/DNGlare.h"
#include "WoW/Source/DayNight/DNPlanet.h"
#include "WoW/Source/DayNight/DNSky.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/LightFloatBandRec.h"
#include "WoW/Source/DB/Rec/LightIntBandRec.h"
#include "WoW/Source/DB/Rec/LightParamsRec.h"
#include "WoW/Source/DB/Rec/LightRec.h"
#include "WoW/Source/DB/Rec/LightSkyboxRec.h"
#include "WoW/Source/DB/Rec/LiquidTypeRec.h"

// STATIC: 0x00CEF89C
static C2Vector s_selfIlluminatedTable[4] = {
    C2Vector(0.25f, 1.0f),
    C2Vector(0.29166666f, 0.0f),
    C2Vector(0.85416663f, 0.0f),
    C2Vector(0.89583331f, 1.0f),
};

// STATIC: 0x00CEFAD0
float DayNight::g_areaLightSortRadius = 0.33333334f;

// STATIC: 0x00E4235C
static uint32_t s_lastLiquidTime;

// STATIC: 0x00E42360
static C4Vector s_liquidDarken;

// STATIC: 0x00E42370
static C4Vector s_liquidDarkenVelocity;

// STATIC: 0x00E42944
int32_t DayNight::g_areaLightSortByDistance;

// STATIC: 0x00E42948
static float s_fogRateScale = 5.5f;

struct LightQELess {
    bool operator()(const DayNight::LightQE& a, const DayNight::LightQE& b) const {
        if (DayNight::g_areaLightSortByDistance) {
            return b.m_distSq < a.m_distSq;
        }

        const LightRec* lightA = *a.m_light;
        const LightRec* lightB = *b.m_light;
        float dx = lightA->m_gameCoords[0] - lightB->m_gameCoords[0];
        float dz = lightA->m_gameCoords[2] - lightB->m_gameCoords[2];
        float dy = lightA->m_gameCoords[1] - lightB->m_gameCoords[1];

        if (DayNight::g_areaLightSortRadius < sqrt(dx * dx + dz * dz + dy * dy)) {
            return a.m_distSq < b.m_distSq;
        }

        return lightA->m_gameFalloffStart < lightB->m_gameFalloffStart;
    }
};

// OFFSET: 0x0064AB90/0x1402E83E0
const LightParamsRec* DayNight::GetLightParams(const LightRec* light, int32_t index) {
    return g_lightParamsDB.GetRecord(light->m_lightParamsID[index]);
}

// OFFSET: 0x0064AC60/0x1402E8500
void DayNight::LightLoad(TSGrowableArray<const LightRec*>& lights, int32_t mapId) {
    uint32_t count = 1;

    for (int32_t i = 0; i < g_lightDB.m_numRecords; i++) {
        LightRec* rec = g_lightDB.GetRecordByIndex(i);

        if (rec->m_continentID == mapId && (rec->m_gameCoords[0] != 0.0f || rec->m_gameCoords[1] != 0.0f || rec->m_gameCoords[2] != 0.0f)) {
            count++;
        }
    }

    int32_t foundGlobal = 0;
    lights.SetCount(count);
    uint32_t index = 1;

    for (int32_t i = 0; i < g_lightDB.m_numRecords; i++) {
        LightRec* rec = g_lightDB.GetRecordByIndex(i);

        if (rec->m_continentID != mapId) {
            continue;
        }

        if (rec->m_gameCoords[0] == 0.0f && rec->m_gameCoords[1] == 0.0f && rec->m_gameCoords[2] == 0.0f) {
            foundGlobal = 1;
            lights.m_data[0] = rec;
        } else {
            lights.m_data[index++] = rec;
        }
    }

    if (!foundGlobal) {
        lights.m_data[0] = g_lightDB.GetRecord(1);
    }
}

// OFFSET: 0x0064AF40
CImVector* DayNight::InterpColor(CImVector* out, const CImVector* a, const CImVector* b, float t) {
    out->b = static_cast<uint8_t>(static_cast<int32_t>((static_cast<float>(b->b) - static_cast<float>(a->b)) * t + static_cast<float>(a->b)));
    out->g = static_cast<uint8_t>(static_cast<int32_t>((static_cast<float>(b->g) - static_cast<float>(a->g)) * t + static_cast<float>(a->g)));
    out->r = static_cast<uint8_t>(static_cast<int32_t>((static_cast<float>(b->r) - static_cast<float>(a->r)) * t + static_cast<float>(a->r)));
    out->a = 0xFF;
    return out;
}

// OFFSET: 0x0064AFC0/0x1402EAC90
CImVector* DayNight::DarkenColor(CImVector* out, const CImVector* color, float scale) {
    C3Vector rgb(color->r * 0.0039215689f, color->g * 0.0039215689f, color->b * 0.0039215689f);
    C3Vector hsv(0.0f, 0.0f, 0.0f);
    RGBtoHSV(rgb, hsv);
    hsv.z = hsv.z * scale;
    HSVtoRGB(hsv, rgb);
    *out = rgb;
    return out;
}

// OFFSET: 0x0064B060/0x1402EAD50
float DayNight::InterpTable(const C2Vector* table, uint32_t count, float t) {
    float value = t;

    if (t >= 0.0f) {
        if (t >= 1.0f) {
            value = 1.0f;
        }
    } else {
        value = 0.0f;
    }

    uint32_t next = 0;

    while (next < count && table[next].x < value) {
        next++;
    }

    uint32_t prev;

    if (next == count) {
        next = 0;
        prev = count - 1;
    } else if (next) {
        prev = next - 1;
    } else {
        prev = count - 1;
    }

    float start = table[prev].x;
    float range = table[next].x - start;

    if (fabs(range) < 0.001) {
        return table[prev].y;
    }

    if (range < 0.0f) {
        range = range + 1.0f;
    }

    float offset = value - start;

    if (offset < 0.0f) {
        offset = offset + 1.0f;
    }

    float base = table[prev].y;
    return base + (table[next].y - base) * (offset / range);
}

// OFFSET: 0x0064B180/0x1402EAA30
static int32_t FindBand(int32_t time, int32_t* index, int32_t count, const int32_t* times, float* t, int32_t* next) {
    *next = 0;
    *t = 0.0f;
    *index = 0;

    if (count <= 0) {
        return 0;
    }

    while (true) {
        int32_t start = times[*index];
        *next = (*index + 1) % count;
        int32_t end = times[*next];

        if (end <= start) {
            if (time <= end || time >= start) {
                end += 2880;

                if (time < start) {
                    time += 2880;
                }

                *t = static_cast<float>(time - start) / static_cast<float>(end - start);
                return end - start;
            }
        } else if (time >= start && time <= end) {
            *t = static_cast<float>(time - start) / static_cast<float>(end - start);
            return end - start;
        }

        if (++*index >= count) {
            return end;
        }
    }
}

// OFFSET: 0x0064B210/0x1402EAE50
CImVector* DayNight::CalcIndividualColorFromDBRec(CImVector* out, const LightIntBandRec* band, int32_t time) {
    if (!band->m_num) {
        *reinterpret_cast<uint32_t*>(out) = 0xFF000000;
        return out;
    }

    int32_t index;
    float t;
    int32_t next;
    FindBand(time, &index, band->m_num, band->m_time, &t, &next);

    const CImVector& a = *reinterpret_cast<const CImVector*>(&band->m_data[index]);
    const CImVector& b = *reinterpret_cast<const CImVector*>(&band->m_data[next]);
    out->b = static_cast<uint8_t>(static_cast<int32_t>((static_cast<float>(b.b) - a.b) * t + a.b));
    out->g = static_cast<uint8_t>(static_cast<int32_t>((static_cast<float>(b.g) - a.g) * t + a.g));
    out->r = static_cast<uint8_t>(static_cast<int32_t>((static_cast<float>(b.r) - a.r) * t + a.r));
    out->a = 0xFF;
    return out;
}

// OFFSET: 0x0064B330/0x1402EAB70
float DayNight::CalcFogRate(float start, float end) {
    float farClip = 700.0f;

    if (DayNight::g_dnInfo.m_farClip < 700.0f) {
        farClip = DayNight::g_dnInfo.m_farClip;
    }

    float range = farClip - 200.0f;
    float length = end - start;

    if (length <= range) {
        return (1.0f - length / range) * s_fogRateScale + 1.5f;
    }

    return 1.5f;
}

// OFFSET: 0x0064B3C0
void DayNight::BlendOutputs(CurrentLight& out, const CurrentLight& in, float t) {
    for (uint32_t i = 0; i < 18; i++) {
        CImVector color;
        color.b = static_cast<uint8_t>(static_cast<int32_t>((static_cast<float>(in.m_colors[i].b) - out.m_colors[i].b) * t + out.m_colors[i].b));
        color.g = static_cast<uint8_t>(static_cast<int32_t>((static_cast<float>(in.m_colors[i].g) - out.m_colors[i].g) * t + out.m_colors[i].g));
        color.r = static_cast<uint8_t>(static_cast<int32_t>((static_cast<float>(in.m_colors[i].r) - out.m_colors[i].r) * t + out.m_colors[i].r));
        color.a = 0xFF;
        out.m_colors[i] = color;
    }

    out.m_fogEnd = (in.m_fogEnd - out.m_fogEnd) * t + out.m_fogEnd;
    out.m_fogStartScalar = (in.m_fogStartScalar - out.m_fogStartScalar) * t + out.m_fogStartScalar;
    out.m_fogRate = (in.m_fogRate - out.m_fogRate) * t + out.m_fogRate;
    out.m_highlightSky = (in.m_highlightSky - out.m_highlightSky) * t + out.m_highlightSky;
    out.m_floatBands[1] = (in.m_floatBands[1] - out.m_floatBands[1]) * t + out.m_floatBands[1];
    out.m_glow = (in.m_glow - out.m_glow) * t + out.m_glow;
    out.m_waterShallowAlpha = (in.m_waterShallowAlpha - out.m_waterShallowAlpha) * t + out.m_waterShallowAlpha;
    out.m_waterDeepAlpha = (in.m_waterDeepAlpha - out.m_waterDeepAlpha) * t + out.m_waterDeepAlpha;
    out.m_oceanShallowAlpha = (in.m_oceanShallowAlpha - out.m_oceanShallowAlpha) * t + out.m_oceanShallowAlpha;
    out.m_oceanDeepAlpha = (in.m_oceanDeepAlpha - out.m_oceanDeepAlpha) * t + out.m_oceanDeepAlpha;
    out.m_lightParamsFlag = (in.m_lightParamsFlag - out.m_lightParamsFlag) * t + out.m_lightParamsFlag;

    int32_t skyboxId = in.m_skyboxes[0].m_skyboxId;

    if (skyboxId && t > 0.0f) {
        for (uint32_t i = 0; i < 3; i++) {
            if (out.m_skyboxes[i].m_skyboxId == skyboxId) {
                float weight = out.m_skyboxes[i].m_weight + t;
                out.m_skyboxes[i].m_weight = weight;

                if (weight > 1.0f) {
                    out.m_skyboxes[i].m_weight = 1.0f;
                }

                break;
            }

            if (!out.m_skyboxes[i].m_skyboxId) {
                out.m_skyboxes[i].m_skyboxId = skyboxId;
                out.m_skyboxes[i].m_weight = t;
                break;
            }
        }
    }

    out.m_cloudTypeId = in.m_cloudTypeId;
    out.m_cloudWeight = t;
}

// OFFSET: 0x0064B8C0/0x1402EB000
CImVector* DayNight::CalcIndividualColorFromDBRec(CImVector* out, int32_t time, const LightParamsRec* params, int32_t band) {
    const LightIntBandRec* rec = g_lightIntBandDB.GetRecord(band + 18 * params->m_id - 17);
    return DayNight::CalcIndividualColorFromDBRec(out, rec, time);
}

// OFFSET: 0x0064B910
void DayNight::BlendColors(CurrentLight& out, const CurrentLight& in, float t) {
    float blend = t;

    if (t < 0.0f) {
        blend = 0.0f;
        t = 0.0f;
    } else if (t >= 1.0f) {
        blend = 1.0f;
        t = 1.0f;
    }

    uint8_t colorBlend = static_cast<uint8_t>(static_cast<int32_t>(blend * 255.0f));

    for (uint32_t i = 0; i < 18; i++) {
        if (i == 2) {
            continue;
        }

        if (colorBlend) {
            out.m_colors[i].InterpColor(colorBlend, &in.m_colors[i]);
        }
    }

    out.m_fogEnd = (in.m_fogEnd - out.m_fogEnd) * t + out.m_fogEnd;
    out.m_fogStartScalar = (in.m_fogStartScalar - out.m_fogStartScalar) * t + out.m_fogStartScalar;
    out.m_fogRate = (in.m_fogRate - out.m_fogRate) * t + out.m_fogRate;

    if (colorBlend) {
        out.m_colors[2].InterpColor(colorBlend, &in.m_colors[2]);
    }

    out.m_floatBands[1] = (in.m_floatBands[1] - out.m_floatBands[1]) * t + out.m_floatBands[1];
    out.m_glow = (in.m_glow - out.m_glow) * t + out.m_glow;
    out.m_waterShallowAlpha = (in.m_waterShallowAlpha - out.m_waterShallowAlpha) * t + out.m_waterShallowAlpha;
    out.m_waterDeepAlpha = (in.m_waterDeepAlpha - out.m_waterDeepAlpha) * t + out.m_waterDeepAlpha;
    out.m_oceanShallowAlpha = (in.m_oceanShallowAlpha - out.m_oceanShallowAlpha) * t + out.m_oceanShallowAlpha;
    out.m_oceanDeepAlpha = (in.m_oceanDeepAlpha - out.m_oceanDeepAlpha) * t + out.m_oceanDeepAlpha;
    out.m_lightParamsFlag = (in.m_lightParamsFlag - out.m_lightParamsFlag) * t + out.m_lightParamsFlag;
}

static float CalcFloatBand(const LightFloatBandRec* band, int32_t time, float scale) {
    if (!band->m_num) {
        return 0.0f;
    }

    int32_t index;
    float t;
    int32_t next;
    FindBand(time, &index, band->m_num, band->m_time, &t, &next);

    float a = band->m_data[index] * scale;
    return (band->m_data[next] * scale - a) * t + a;
}

// OFFSET: 0x0064BCD0/0x1402EB060
void DayNight::CalcColors(int32_t time, CurrentLight& light, const LightParamsRec* params) {
    static const int32_t s_bandSlots[18] = { 1, 0, 3, 4, 5, 6, 7, 8, 2, 9, 10, 11, 12, 13, 14, 15, 16, 17 };

    for (int32_t band = 0; band < 18; band++) {
        const LightIntBandRec* rec = g_lightIntBandDB.GetRecord(18 * params->m_id - 17 + band);
        DayNight::CalcIndividualColorFromDBRec(&light.m_colors[s_bandSlots[band]], rec, time);
    }

    float fogEnd = CalcFloatBand(g_lightFloatBandDB.GetRecord(6 * params->m_id - 5), time, 0.027777778f);
    light.m_fogEnd = fogEnd;

    float fogStartScalar = CalcFloatBand(g_lightFloatBandDB.GetRecord(6 * params->m_id - 4), time, 1.0f);
    light.m_fogStartScalar = fogStartScalar;

    if (fogStartScalar >= -1.0f) {
        if (fogStartScalar > 1.0f) {
            light.m_fogStartScalar = 1.0f;
        }
    } else {
        light.m_fogStartScalar = -1.0f;
    }

    light.m_fogRate = 1.0f;
    light.m_floatBands[0] = CalcFloatBand(g_lightFloatBandDB.GetRecord(6 * params->m_id - 3), time, 1.0f);
    light.m_floatBands[1] = CalcFloatBand(g_lightFloatBandDB.GetRecord(6 * params->m_id - 2), time, 1.0f);
    light.m_floatBands[2] = CalcFloatBand(g_lightFloatBandDB.GetRecord(6 * params->m_id - 1), time, 1.0f);
    light.m_floatBands[3] = CalcFloatBand(g_lightFloatBandDB.GetRecord(6 * params->m_id), time, 1.0f);
    light.m_highlightSky = static_cast<float>(params->m_highlightSky);
    light.m_glow = params->m_glow;
    light.m_waterShallowAlpha = params->m_waterShallowAlpha;
    light.m_waterDeepAlpha = params->m_waterDeepAlpha;
    light.m_oceanShallowAlpha = params->m_oceanShallowAlpha;
    light.m_oceanDeepAlpha = params->m_oceanDeepAlpha;
    light.m_lightParamsFlag = params->unk_0024 ? 1.0f : 0.0f;
    light.m_skyboxes[0].m_skyboxId = params->m_lightSkyboxID;
    light.m_skyboxes[0].m_weight = 1.0f;
    light.m_cloudTypeId = params->m_cloudTypeID;
    light.m_cloudWeight = 1.0f;

    if (fogEnd < 10.0f) {
        light.m_fogEnd = 10.0f;
    }

    if (DayNight::g_dnInfo.m_fogLinear == 1) {
        float end = light.m_fogEnd;

        if (end >= 27.777779f) {
            light.m_fogRate = DayNight::CalcFogRate(end * light.m_fogStartScalar, end);
            light.m_fogEnd = DayNight::g_dnInfo.m_farClip;
        }

        if (light.m_fogStartScalar < 0.0f) {
            light.m_fogStartScalar = 0.0f;
        }
    }
}

// OFFSET: 0x0064C560
void DayNight::BlendAreaLightColors(CurrentLight& light, const LightRec* rec, int32_t index, int32_t stormIndex) {
    DayNight::CalcColors(static_cast<int32_t>(DayNight::g_dnInfo.m_dayFraction * 2880.0f), light, DayNight::GetLightParams(rec, index));

    if (DayNight::g_dnInfo.m_stormBlend > 0.0f) {
        CurrentLight storm;
        DayNight::CalcColors(static_cast<int32_t>(DayNight::g_dnInfo.m_dayFraction * 2880.0f), storm, DayNight::GetLightParams(rec, stormIndex));
        DayNight::BlendColors(light, storm, DayNight::g_dnInfo.m_stormBlend);
    }
}

// OFFSET: 0x0064C610
void DayNight::BlendAreaLight(CurrentLight& out, float weight, const LightRec* rec, int32_t index, int32_t stormIndex) {
    CurrentLight light;
    DayNight::BlendAreaLightColors(light, rec, index, stormIndex);

    float dz = DayNight::g_dnInfo.m_camTarget.z - rec->m_gameCoords[2];
    float dx = DayNight::g_dnInfo.m_camTarget.x - rec->m_gameCoords[0];
    float dy = DayNight::g_dnInfo.m_camTarget.y - rec->m_gameCoords[1];
    float falloff = 1.0f;
    float dist = sqrt(dz * dz + dx * dx + dy * dy);

    if (dist > rec->m_gameFalloffStart) {
        float ratio = 1.0f - (dist - rec->m_gameFalloffStart) / (rec->m_gameFalloffEnd - rec->m_gameFalloffStart);
        falloff = ratio >= 0.0f ? ratio : 0.0f;
    }

    if (weight <= falloff) {
        falloff = weight;
    }

    DayNight::BlendOutputs(out, light, falloff);
}

// OFFSET: 0x0064CC00/0x1402ED6D0
void DayNight::DoAreaLights(uint32_t time, CurrentLight& light, int32_t index, int32_t stormIndex) {
    std::vector<LightQE> queue;
    std::make_heap(queue.begin(), queue.end(), LightQELess());

    if (DayNight::g_dnInfo.m_zoneLightCount) {
        for (uint32_t i = 0; i < DayNight::g_dnInfo.m_zoneLightCount; i++) {
            const LightRec* zoneLight = DayNight::g_dnInfo.m_zoneLights[i];
            float blend = DayNight::g_dnInfo.m_zoneLightBlend[i];
            CurrentLight zone;
            DayNight::BlendAreaLightColors(zone, zoneLight, index, stormIndex);

            float t = (100.0f - blend) * 0.0099999998f;
            float weight;

            if (t <= 0.0f) {
                weight = 1.0f;
            } else if (1.0f - t < 0.0f) {
                weight = 0.0f;
            } else {
                weight = 1.0f - t;

                if (weight >= 1.0f) {
                    weight = 1.0f;
                }
            }

            DayNight::BlendOutputs(light, zone, weight);
        }

        DayNight::g_dnInfo.ClearZoneLights();
    }

    for (uint32_t i = 1; i < DayNight::g_areaLights.m_count; i++) {
        const LightRec* rec = DayNight::g_areaLights.m_data[i];

        if (rec->m_gameFalloffEnd >= 3.0f) {
            float dz = DayNight::g_dnInfo.m_camTarget.z - rec->m_gameCoords[2];
            float dy = DayNight::g_dnInfo.m_camTarget.y - rec->m_gameCoords[1];
            float dx = DayNight::g_dnInfo.m_camTarget.x - rec->m_gameCoords[0];
            float distSq = dz * dz + dy * dy + dx * dx;

            if (rec->m_gameFalloffEnd * rec->m_gameFalloffEnd > distSq) {
                LightQE entry;
                entry.m_distSq = distSq;
                entry.m_light = &DayNight::g_areaLights.m_data[i];
                queue.push_back(entry);
                std::push_heap(queue.begin(), queue.end(), LightQELess());
            }
        }
    }

    while (!queue.empty()) {
        DayNight::BlendAreaLight(light, 1.0f, *queue.front().m_light, index, stormIndex);
        std::pop_heap(queue.begin(), queue.end(), LightQELess());
        queue.pop_back();
    }

    uint32_t count = DayNight::g_areaLightOverrides.m_count;

    for (uint32_t i = 0; i < count; i++) {
        AreaLightOverride* areaLight = &DayNight::g_areaLightOverrides.m_data[i];
        int32_t elapsed = time - areaLight->m_time;
        areaLight->m_time = time;

        if (areaLight->m_state == 1) {
            float blend = static_cast<float>(elapsed) / static_cast<float>(static_cast<uint32_t>(areaLight->m_duration)) + areaLight->m_blend;
            areaLight->m_blend = blend <= 1.0f ? blend : 1.0f;
        } else if (areaLight->m_state == 2) {
            float blend = areaLight->m_blend - static_cast<float>(elapsed) / static_cast<float>(static_cast<uint32_t>(areaLight->m_duration));
            areaLight->m_blend = blend >= 0.0f ? blend : 0.0f;
        }

        const LightRec* rec = areaLight->m_rec;
        float dx = DayNight::g_dnInfo.m_camTarget.x - rec->m_gameCoords[0];
        float dy = DayNight::g_dnInfo.m_camTarget.y - rec->m_gameCoords[1];
        float dz = DayNight::g_dnInfo.m_camTarget.z - rec->m_gameCoords[2];
        float blend = areaLight->m_blend;

        if (blend > 0.0f && rec->m_gameFalloffEnd * rec->m_gameFalloffEnd > dz * dz + dy * dy + dx * dx) {
            DayNight::BlendAreaLight(light, blend, rec, index, stormIndex);
        }

        if (areaLight->m_state == 2 && areaLight->m_blend == 0.0f) {
            count--;
            DayNight::g_areaLightRecordCache.PutData(const_cast<LightRec*>(areaLight->m_rec), 0, 0);
            *areaLight = DayNight::g_areaLightOverrides.m_data[count];
            DayNight::g_areaLightOverrides.SetCount(count);
            i--;
        }
    }
}

// OFFSET: 0x0064D0B0/0x1402EDC20
void DayNight::GetLight(uint32_t time, const C3Vector& pos, float dayFraction, CurrentLight& light, int32_t index, int32_t stormIndex) {
    if (DayNight::g_areaLights.m_count) {
        C3Vector camPos = DayNight::g_dnInfo.m_camPos;
        C3Vector camTarget = DayNight::g_dnInfo.m_camTarget;
        DayNight::g_dnInfo.m_camPos = pos;
        DayNight::g_dnInfo.m_camTarget = pos;
        DayNight::CalcColors(static_cast<int32_t>(dayFraction * 2880.0f), light, DayNight::GetLightParams(DayNight::g_areaLights.m_data[0], index));
        DayNight::DoAreaLights(time, light, index, stormIndex);
        DayNight::g_dnInfo.m_camPos = camPos;
        DayNight::g_dnInfo.m_camTarget = camTarget;
        return;
    }

    memset(light.m_colors, 0xFF, sizeof(CurrentLight));
    light.m_fogEnd = 1.0e10f;
    light.m_fogStartScalar = 0.5f;
    DayNight::g_dnInfo.m_light.m_fogRate = 4.0f;
    light.m_highlightSky = 0.0f;
    light.m_floatBands[0] = 0.0f;
    light.m_floatBands[1] = 0.0f;
    light.m_floatBands[2] = 0.0f;
    light.m_floatBands[3] = 0.0f;
    light.m_oceanShallowAlpha = 0.75f;
    light.m_cloudTypeId = 0;
    *reinterpret_cast<uint32_t*>(&light.m_colors[0]) = 0xFFFFFFFF;
    light.m_glow = 0.5f;
    light.m_oceanDeepAlpha = 1.0f;
    light.m_lightParamsFlag = 0.0f;
    light.m_waterShallowAlpha = 0.5f;
    light.m_waterDeepAlpha = 1.0f;
    light.m_skyboxes[0].m_skyboxId = 0;
    light.m_skyboxes[1].m_skyboxId = 0;
    light.m_skyboxes[2].m_skyboxId = 0;
}

// OFFSET: 0x00649AA0
void DayNight::SetColors() {
    DNInfo& info = DayNight::g_dnInfo;
    float depth = info.m_camDepth;
    LiquidTypeRec* liquidType = info.m_liquidStatus ? g_liquidTypeDB.GetRecord(info.m_liquidStatus) : NULL;

    info.m_blendSkybox[0] = NULL;
    info.m_blendSkyboxWeight[0] = 0.0f;
    info.m_blendSkyboxFlags[0] = 0;
    info.m_blendSkybox[1] = NULL;
    info.m_blendSkyboxWeight[1] = 0.0f;
    info.m_blendSkyboxFlags[1] = 0;
    info.m_blendSkybox[2] = NULL;
    info.m_blendSkyboxWeight[2] = 0.0f;
    info.m_blendSkyboxFlags[2] = 0;
    info.m_skybox = NULL;
    info.m_skyboxWeight = 0.0f;

    if (DayNight::g_areaLights.m_count) {
        CurrentLight light;
        CurrentLight lightUW;
        const LightRec* global = DayNight::g_areaLights.m_data[0];
        int32_t time = static_cast<int32_t>(info.m_dayFraction * 2880.0f);

        DayNight::CalcColors(time, light, DayNight::GetLightParams(global, 0));
        DayNight::CalcColors(time, lightUW, DayNight::GetLightParams(global, 1));

        if (info.m_stormBlend > 0.0f) {
            CurrentLight storm;
            DayNight::CalcColors(time, storm, DayNight::GetLightParams(global, 2));
            DayNight::BlendColors(light, storm, info.m_stormBlend);

            CurrentLight stormUW;
            DayNight::CalcColors(time, stormUW, DayNight::GetLightParams(global, 3));
            DayNight::BlendColors(lightUW, stormUW, info.m_stormBlend);
        }

        DayNight::DoAreaLights(info.m_timeMs, light, 0, 2);
        DayNight::DoAreaLights(info.m_timeMs, lightUW, 1, 3);

        LightParamsRec* overrideParams = info.m_overrideLightParamsId ? g_lightParamsDB.GetRecord(info.m_overrideLightParamsId) : NULL;

        if (overrideParams) {
            DayNight::CalcColors(time, info.m_light, overrideParams);
            DayNight::CalcColors(time, info.m_lightUW, overrideParams);

            info.m_lightUW.m_glow = light.m_glow;
            info.m_light.m_glow = light.m_glow;
            info.m_lightUW.m_waterShallowAlpha = light.m_waterShallowAlpha;
            info.m_light.m_waterShallowAlpha = light.m_waterShallowAlpha;
            info.m_lightUW.m_waterDeepAlpha = light.m_waterDeepAlpha;
            info.m_light.m_waterDeepAlpha = light.m_waterDeepAlpha;
            info.m_lightUW.m_cloudTypeId = light.m_cloudTypeId;
            info.m_light.m_cloudTypeId = light.m_cloudTypeId;
            info.m_lightUW.m_cloudWeight = light.m_cloudWeight;
            info.m_light.m_cloudWeight = light.m_cloudWeight;
            info.m_lightUW.m_oceanShallowAlpha = light.m_oceanShallowAlpha;
            info.m_light.m_oceanShallowAlpha = light.m_oceanShallowAlpha;
            info.m_light.m_skyboxes[1].m_skyboxId = light.m_skyboxes[1].m_skyboxId;
            info.m_light.m_skyboxes[0].m_weight = light.m_skyboxes[0].m_weight;
            info.m_lightUW.m_oceanDeepAlpha = light.m_oceanDeepAlpha;
            info.m_light.m_oceanDeepAlpha = light.m_oceanDeepAlpha;
            info.m_light.m_skyboxes[2].m_weight = light.m_skyboxes[2].m_weight;
            info.m_lightUW.m_lightParamsFlag = light.m_lightParamsFlag;
            info.m_light.m_lightParamsFlag = light.m_lightParamsFlag;
            info.m_light.m_skyboxes[0].m_skyboxId = light.m_skyboxes[0].m_skyboxId;
            info.m_lightUW.m_skyboxes[0].m_skyboxId = 0;
            info.m_light.m_skyboxes[1].m_weight = light.m_skyboxes[1].m_weight;
            info.m_lightUW.m_skyboxes[1].m_skyboxId = 0;
            info.m_light.m_skyboxes[2].m_skyboxId = light.m_skyboxes[2].m_skyboxId;
            info.m_lightUW.m_skyboxes[2].m_skyboxId = 0;

            LightSkyboxRec* skybox = g_lightSkyboxDB.GetRecord(overrideParams->m_lightSkyboxID);

            if (skybox) {
                info.m_skybox = DayNight::GetOverrideSky(info.m_timeMs, skybox->m_name, skybox->m_flags);
                info.m_skyboxWeight = 1.0f;
            }
        } else {
            info.m_light = light;
            info.m_lightUW = lightUW;
        }

        if (liquidType && liquidType->m_lightID) {
            DayNight::CalcColors(time, info.m_lightUW, g_lightParamsDB.GetRecord(liquidType->m_lightID));
        }
    } else {
        memset(&info.m_light, 0xFF, sizeof(CurrentLight));
        memset(&info.m_lightUW, 0xFF, sizeof(CurrentLight));
        info.m_lightUW.m_fogEnd = 1.0e10f;
        info.m_light.m_fogEnd = 1.0e10f;
        info.m_lightUW.m_fogRate = 4.0f;
        info.m_light.m_fogRate = 4.0f;
        info.m_lightUW.m_oceanShallowAlpha = 0.75f;
        info.m_light.m_oceanShallowAlpha = 0.75f;
        info.m_lightUW.m_fogStartScalar = 0.5f;
        info.m_light.m_fogStartScalar = 0.5f;
        info.m_lightUW.m_highlightSky = 0.0f;
        info.m_light.m_highlightSky = 0.0f;
        info.m_lightUW.m_floatBands[0] = 0.0f;
        info.m_light.m_floatBands[0] = 0.0f;
        info.m_lightUW.m_floatBands[1] = 0.0f;
        info.m_light.m_floatBands[1] = 0.0f;
        info.m_lightUW.m_floatBands[2] = 0.0f;
        info.m_light.m_floatBands[2] = 0.0f;
        info.m_lightUW.m_floatBands[3] = 0.0f;
        info.m_light.m_floatBands[3] = 0.0f;
        *reinterpret_cast<uint32_t*>(&info.m_lightUW.m_colors[0]) = 0xFF404040;
        *reinterpret_cast<uint32_t*>(&info.m_light.m_colors[0]) = 0xFF404040;
        info.m_lightUW.m_glow = 0.5f;
        info.m_light.m_glow = 0.5f;
        info.m_lightUW.m_oceanDeepAlpha = 1.0f;
        info.m_light.m_oceanDeepAlpha = 1.0f;
        info.m_lightUW.m_lightParamsFlag = 0.0f;
        info.m_light.m_lightParamsFlag = 0.0f;
        info.m_light.m_skyboxes[0].m_skyboxId = 0;
        info.m_light.m_skyboxes[0].m_weight = 0.0f;
        info.m_lightUW.m_waterShallowAlpha = 0.5f;
        info.m_light.m_waterShallowAlpha = 0.5f;
        info.m_lightUW.m_waterDeepAlpha = 1.0f;
        info.m_light.m_waterDeepAlpha = 1.0f;
        info.m_lightUW.m_cloudTypeId = 0;
        info.m_light.m_cloudTypeId = 0;
        info.m_lightUW.m_skyboxes[0].m_skyboxId = 0;
        info.m_lightUW.m_skyboxes[1].m_skyboxId = 0;
        info.m_light.m_skyboxes[1].m_weight = 0.0f;
        info.m_lightUW.m_skyboxes[2].m_skyboxId = 0;
        info.m_light.m_skyboxes[2].m_weight = 0.0f;
    }

    DayNight::SetLightColors();

    float dt = static_cast<float>(static_cast<uint32_t>(info.m_timeMs - s_lastLiquidTime)) * 0.001f;
    C4Vector target;
    target.x = 0.0f;
    target.y = 0.0f;
    target.z = 0.0f;
    target.w = 0.0f;

    if (liquidType) {
        float maxDepth = liquidType->m_maxDarkenDepth;

        if (maxDepth > 0.0000099999997f) {
            float minDepth = -maxDepth;
            float value = -((1.0f - info.m_lightUW.m_lightParamsFlag) * depth);
            float clamped = value <= 0.0f ? value : 0.0f;

            if (clamped <= minDepth) {
                value = minDepth;
            } else if (value > 0.0f) {
                value = 0.0f;
            }

            target.x = 1.0f - value / minDepth;
            target.y = liquidType->m_fogDarkenIntensity;
            target.z = liquidType->m_ambDarkenIntensity;
            target.w = liquidType->m_dirDarkenIntensity;
        }
    }

    if (s_lastLiquidTime) {
        NTempest::SmoothDamp(s_liquidDarken, s_liquidDarkenVelocity, target, 1.0f, dt, 1.0f / (dt * dt * dt * 0.235f + dt * dt * 0.47999999f + dt + 1.0f));
    } else {
        s_liquidDarken = target;
        s_liquidDarkenVelocity.x = 0.0f;
        s_liquidDarkenVelocity.y = 0.0f;
        s_liquidDarkenVelocity.z = 0.0f;
        s_liquidDarkenVelocity.w = 0.0f;
    }

    s_lastLiquidTime = info.m_timeMs;

    CImVector color;
    info.m_fogUW.m_color = *DayNight::DarkenColor(&color, &info.m_fogUW.m_color, 1.0f + (s_liquidDarken.x - 1.0f) * s_liquidDarken.y);
    info.m_lightInfoUW.m_ambientColor = *DayNight::DarkenColor(&color, &info.m_lightInfoUW.m_ambientColor, (s_liquidDarken.x - 1.0f) * s_liquidDarken.z + 1.0f);
    info.m_lightInfoUW.m_directColor = *DayNight::DarkenColor(&color, &info.m_lightInfoUW.m_directColor, (s_liquidDarken.x - 1.0f) * s_liquidDarken.w + 1.0f);

    DayNight::g_sky.SetColors(info.m_light);
    DayNight::g_planets[0].m_color = info.m_light.m_colors[9];
    DayNight::g_sunGlare.m_color = info.m_light.m_colors[9];
    DayNight::g_planets[1].m_color = info.m_light.m_colors[9];
    DayNight::g_moonGlare.m_color = info.m_light.m_colors[9];

    if (static_cast<float>(info.m_eclipseAmount) != 0.0f) {
        CImVector* targets[8] = {
            &info.m_fog.m_color,
            &info.m_lightInfo.m_ambientColor,
            &info.m_lightInfo.m_directColor,
            &info.m_fogUW.m_color,
            &info.m_lightInfoUW.m_ambientColor,
            &info.m_lightInfoUW.m_directColor,
            &DayNight::g_planets[0].m_color,
            &DayNight::g_planets[1].m_color,
        };

        for (uint32_t i = 0; i < 8; i++) {
            CImVector eclipse;
            memcpy(&eclipse, info.m_eclipseLightId, sizeof(eclipse));

            if (info.m_eclipseAmount) {
                targets[i]->InterpColor(info.m_eclipseAmount, &eclipse);
            }
        }
    }

    if (info.m_stormBlend != 0.0f) {
        uint8_t alpha = static_cast<uint8_t>(static_cast<int32_t>((1.0f - info.m_stormBlend) * 255.0f));
        DayNight::g_planets[0].m_color.a = alpha;
        DayNight::g_sunGlare.m_color.a = alpha;
        DayNight::g_planets[1].m_color.a = alpha;
        DayNight::g_moonGlare.m_color.a = alpha;
        DayNight::g_planets[2].m_color.a = alpha;
    }

    uint32_t count = 0;

    for (uint32_t i = 0; i < 3 && count < 3; i++) {
        const SkyboxBlend& blend = info.m_light.m_skyboxes[i];

        if (!blend.m_skyboxId || blend.m_weight <= 0.0f) {
            continue;
        }

        LightSkyboxRec* skybox = g_lightSkyboxDB.GetRecord(blend.m_skyboxId);

        if (!skybox) {
            continue;
        }

        DNOverrideSky* sky = DayNight::GetOverrideSky(info.m_timeMs, skybox->m_name, skybox->m_flags);
        float weight = blend.m_weight;
        uint32_t flags = skybox->m_flags & 0x2;

        if (weight > 0.99000001f && count && !flags) {
            count = 0;
        }

        info.m_blendSkybox[count] = sky;
        info.m_blendSkyboxWeight[count] = weight;
        info.m_blendSkyboxFlags[count] = flags;
        count++;
    }

    if (count < 3) {
        info.m_blendSkyboxWeight[count] = 0.0f;
    }

    info.m_selfIlluminatedScalar = DayNight::InterpTable(s_selfIlluminatedTable, 4, info.m_dayFraction);
}

#include "WoW/Source/WorldClient/MapWeather.h"
#include <cfloat>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <new>
#include "Engine/Source/Event/FrameTime.h"
#include "Engine/Source/Storm/H/storm.h"
#include "WoW/Source/DayNight/DayNight.h"
#include "WoW/Source/DB/Rec/WeatherRec.h"
#include "WoW/Source/WorldClient/Rain.h"
#include "WoW/Source/WorldClient/Sand.h"
#include "WoW/Source/WorldClient/Snow.h"
#include "WoW/Source/WorldClient/World.h"
#include "WoW/Source/WorldClient/WorldMap.h"
#include "WoW/Source/WorldClient/WorldScene.h"
#include "WoW/Source/WorldClient/WorldView.h"

// STATIC: 0x00D017A8
static float s_twoPi = 6.2831855f;

// STATIC: 0x00E756C4
uint8_t Weather::s_weatherTypeForced;

// PROVEN: 25c70816 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void Weather::Initialize() {
}

// PROVEN: f8e1aa9a IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void Weather::Destroy() {
}

// OFFSET: 0x007483D0
int32_t Weather::GetType() const {
    int32_t type = 0;

    if (this->m_rain) {
        return 1;
    }

    if (this->m_snow) {
        return 2;
    }

    if (this->m_sand) {
        type = 3;
    }

    return type;
}

// PROVEN: 02c6d5e6 IDENTICAL/NONE
// OFFSET: 0x00748400
void Weather::FillInDefaultPrecipTexturePath(int32_t type) {
    if (type == 1) {
        strcpy(this->m_precipTexture, "textures\\Weather\\RainDrop01.blp");
    } else if (type == 2) {
        strcpy(this->m_precipTexture, "textures\\Weather\\SnowFlake01.blp");
    }
}

// PROVEN: 42821182 IDENTICAL/NONE
// OFFSET: 0x00748440
CHMMgr::~CHMMgr() {
    for (uint32_t y = 0; y < 5; y++) {
        for (uint32_t x = 0; x < 5; x++) {
            if (this->m_cells[y][x]) {
                SMemFree(this->m_cells[y][x], "delete", -1, 0);
            }
        }
    }
}

// PROVEN: 25f18666 IDENTICAL/NONE
// OFFSET: 0x00748480/0x1404274C0
void CHMMgr::Invalidate() {
    for (uint32_t y = 0; y < 5; y++) {
        for (uint32_t x = 0; x < 5; x++) {
            WeatherGridCell* cell = this->m_cells[y][x];

            for (uint32_t i = 0; i < 1024; i++) {
                cell->m_heights[i] = FLT_MAX;
            }
        }
    }
}

// OFFSET: 0x00748A30
void Weather::SetType(int32_t type, float intensity, const WeatherRec* rec, bool transition, float densityBlend) {
    if (Weather::s_weatherTypeForced || static_cast<uint32_t>(type) > 3) {
        return;
    }

    this->m_transition = transition;
    bool same = true;

    if (this->GetType() != type) {
        this->m_precipTexture[0] = '\0';
        same = false;
    }

    bool custom = false;

    if (rec) {
        this->m_color.Set(1.0f, rec->m_effectColor[0], rec->m_effectColor[1], rec->m_effectColor[2]);

        if (strlen(rec->m_effectTexture) && _stricmp(this->m_precipTexture, rec->m_effectTexture)) {
            same = false;
            strcpy(this->m_precipTexture, rec->m_effectTexture);
            custom = true;
        }
    }

    if (!strlen(this->m_precipTexture) && !custom) {
        this->FillInDefaultPrecipTexturePath(type);
        same = false;
    }

    if (this->GetType() != type || !same) {
        this->m_pendingType = type;
    }

    float target = intensity < 0.0f ? 0.0f : intensity >= 1.0f ? 1.0f : intensity;
    this->m_startIntensity = this->m_targetIntensity;
    this->m_targetIntensity = target;
    float blend;

    if (!this->GetType()) {
        blend = densityBlend;
        this->m_targetDensityBlend = densityBlend;
    } else if (type) {
        blend = densityBlend;
    } else {
        blend = this->m_targetDensityBlend;
    }

    this->m_startDensityBlend = this->m_targetDensityBlend;
    this->m_targetDensityBlend = blend;

    if (!transition) {
        this->m_startIntensity = target;
        this->m_intensity = target;
        this->m_startDensityBlend = blend;
        this->m_densityBlend = blend;
        this->m_densityChanged = 1;
    }

    this->m_targetFogIntensity = target < 0.0f ? 0.0f : target >= 0.25f ? 0.25f : target;
    this->m_startFogIntensity = this->m_startIntensity < 0.0f ? 0.0f : this->m_startIntensity >= 0.25f ? 0.25f : this->m_startIntensity;
    uint32_t now = FrameTime::GetCurTimeMs();
    this->m_intensityStartTime = now;
    this->m_fogStartTime = now;
}

// OFFSET: 0x00748BE0
void Weather::SetStormIntensity(float start, float target) {
    float range = target - start;
    uint32_t now = FrameTime::GetCurTimeMs();
    double t = static_cast<double>(now - this->m_intensityStartTime) * 0.001 / (fabs(range + 0.001) * 10.0);
    float blend = t < 0.0 ? 0.0f : t >= 1.0 ? 1.0f : static_cast<float>(t);
    float intensity = range * blend + start;
    this->m_intensity = intensity;
    float storm = (intensity - 0.25f) * 1.3333334f;

    if (storm < 0.0f) {
        storm = 0.0f;
    }

    float density = Weather::s_densityScale;
    Rain* rain = this->m_rain;

    if (rain) {
        float count = rain->m_useShaders ? 35000.0f : 6500.0f;
        rain->m_intensity = storm;
        rain->m_particlesPerSecond = density * count * storm;
        rain->m_maxBuildTime = count * 0.0009765625f;
        float mists = rain->m_useShaders ? 38.0f : 18.0f;
        float heavy = storm - 0.5f;

        if (heavy < 0.0f) {
            heavy = 0.0f;
        }

        rain->m_mists.m_spawnRate = heavy * 2.0f * density * mists;
    }

    Snow* snow = this->m_snow;

    if (snow) {
        float count = snow->m_useShaders ? 14000.0f : 1300.0f;
        snow->m_intensity = storm;
        snow->m_particlesPerSecond = density * count * storm;
        snow->m_maxBuildTime = count * 0.00016276042f;
        float mists = snow->m_useShaders ? 48.0f : 24.0f;
        float heavy = storm - 0.5f;

        if (heavy < 0.0f) {
            heavy = 0.0f;
        }

        snow->m_mists.m_spawnRate = heavy * 2.0f * density * mists;
    }

    Sand* sand = this->m_sand;

    if (sand) {
        float count = sand->m_useShaders ? 32000.0f : 6000.0f;
        sand->m_maxBuildTime = count * 0.00016276042f;
        sand->m_particlesPerSecond = density * count * storm;
        float mists = sand->m_useShaders ? 64.0f : 32.0f;
        sand->m_mists.m_spawnRate = density * mists * storm;
    }

    double elapsed = static_cast<double>(now - this->m_fogStartTime) * 0.001;
    double fogT = elapsed / (10.0 * fabs((this->m_targetFogIntensity - this->m_startFogIntensity) * 4.0f + 0.001));
    float fogBlend = fogT < 0.0 ? 0.0f : fogT >= 1.0 ? 1.0f : static_cast<float>(fogT);
    float fog = (this->m_targetFogIntensity - this->m_startFogIntensity) * fogBlend + this->m_startFogIntensity;
    float startBlend = this->m_startDensityBlend;
    float blendRange = this->m_targetDensityBlend - startBlend;
    this->m_fogIntensity = fog;
    double blendDuration = fabs(4.0f * blendRange + 0.001) * 5.0;
    double blendT = elapsed / blendDuration;
    float densityT = blendT < 0.0 ? 0.0f : blendT >= 1.0 ? 1.0f : static_cast<float>(blendT);
    float densityBlend = blendRange * densityT + startBlend;
    this->m_densityBlend = densityBlend;
    DayNight::SetStormPercentage(densityBlend * fog);
    this->m_densityChanged = 0;
}

// OFFSET: 0x00748F40
static int32_t GetHeightOrDefault(const C3Vector& pos, float& height) {
    CWorldMap* worldMap = CWorldMap::s_worldMaps[CWorldMap::s_activeWorldMapIndex];

    if (!worldMap) {
        height = pos.z - 200.0f;
        return 1;
    }

    if (worldMap->QueryHeight(pos, 200.0f, height)) {
        return 1;
    }

    height = pos.z - 200.0f;
    return 0;
}

// OFFSET: 0x00748FC0
float WeatherGridCell::GetHeight(const C3Vector& pos) {
    float dx = pos.x - static_cast<float>(this->m_cellX) * 33.333332f;
    float dy = pos.y - static_cast<float>(this->m_cellY) * 33.333332f;

    if (dx < 0.0f || dy < 0.0f) {
        return FLT_MAX;
    }

    int32_t x = static_cast<int32_t>(dx * 0.95999998f);
    int32_t y = static_cast<int32_t>(dy * 0.95999998f);

    if (x < 0 || y < 0 || x >= 32 || y >= 32) {
        return FLT_MAX;
    }

    float& cached = this->m_heights[32 * y + x];
    float height = cached;

    if (height != FLT_MAX) {
        return height;
    }

    if (GetHeightOrDefault(pos, height)) {
        cached = height;
    }

    return height;
}

WeatherGridCell::WeatherGridCell() {
    this->m_cellX = -9999;
    this->m_cellY = -9999;

    for (uint32_t i = 0; i < 1024; i++) {
        this->m_heights[i] = FLT_MAX;
    }
}

// OFFSET: 0x00749090/0x140428430
CHMMgr::CHMMgr() {
    this->m_originX = 0;
    this->m_originY = 0;
    this->m_minCellX = 0;
    this->m_minCellY = 0;
    this->m_minX = 0;
    this->m_minY = 0;

    for (uint32_t y = 0; y < 5; y++) {
        for (uint32_t x = 0; x < 5; x++) {
            void* mem = SMemAlloc(sizeof(WeatherGridCell), __FILE__, __LINE__, 0);
            this->m_cells[y][x] = mem ? new (mem) WeatherGridCell() : NULL;
        }
    }
}

// OFFSET: 0x00749130
float CHMMgr::Query(const C3Vector& pos) {
    int32_t x = static_cast<int32_t>(floor(pos.x * 0.029999999f)) - this->m_minCellX;
    int32_t y = static_cast<int32_t>(floor(pos.y * 0.029999999f)) - this->m_minCellY;

    if (static_cast<uint32_t>(x) > 4 || static_cast<uint32_t>(y) > 4) {
        return FLT_MAX;
    }

    return this->m_cells[y][x]->GetHeight(pos);
}

// OFFSET: 0x007491B0
float CHMMgr::GetSubCellHeight(const C2iVector& cell, float z) {
    WeatherGridCell* gridCell = this->m_cells[cell.y >> 5][cell.x >> 5];
    uint32_t index = (cell.x & 0x1F) + 32 * (cell.y & 0x1F);
    C3Vector pos;
    pos.x = static_cast<float>(this->m_minCellX) * 33.333332f + static_cast<float>(cell.x) * 1.0416666f + 0.52083331f;
    pos.y = static_cast<float>(this->m_minCellY) * 33.333332f + static_cast<float>(cell.y) * 1.0416666f + 0.52083331f;
    pos.z = z;
    float height = gridCell->m_heights[index];

    if (height != FLT_MAX) {
        return height;
    }

    if (GetHeightOrDefault(pos, height)) {
        gridCell->m_heights[index] = height;
    }

    return height;
}

// OFFSET: 0x00749290
bool CHMMgr::Query(const C3Vector& start, const C3Vector& end, C3Vector& hit) {
    hit = start;
    int32_t startX = static_cast<int32_t>(floor(start.x * 0.95999998f)) - this->m_minX;
    int32_t startY = static_cast<int32_t>(floor(start.y * 0.95999998f)) - this->m_minY;
    int32_t endX = static_cast<int32_t>(floor(end.x * 0.95999998f)) - this->m_minX;
    int32_t endY = static_cast<int32_t>(floor(end.y * 0.95999998f)) - this->m_minY;
    C3Vector delta(end.x - start.x, end.y - start.y, end.z - start.z);
    C2iVector size = { abs(endX - startX), abs(endY - startY) };
    int32_t straight;
    int32_t error;
    int32_t diagonal;
    int32_t steps;
    int32_t stepX;
    int32_t diagX;
    int32_t stepY;

    if (size.MajorAxis()) {
        straight = 2 * size.x;
        error = 2 * size.x - size.y;
        diagonal = 2 * (size.x - size.y);
        steps = size.y;
        stepX = 0;
        diagX = 1;
        stepY = 1;
    } else {
        straight = 2 * size.y;
        error = 2 * size.y - size.x;
        steps = size.x;
        diagonal = 2 * (size.y - size.x);
        stepX = 1;
        diagX = 1;
        stepY = 0;
    }

    int32_t diagY = 1;

    if (start.x > end.x) {
        stepX = -stepX;
        diagX = -1;
    }

    if (start.y > end.y) {
        stepY = -stepY;
        diagY = -1;
    }

    if (steps <= 0) {
        steps = 1;
    }

    float invSteps = 1.0f / static_cast<float>(steps);
    C2iVector cell = { startX, startY };

    for (int32_t i = 0; cell.x >= 0 && cell.y >= 0 && cell.x < 160 && cell.y < 160; i++) {
        float t = static_cast<float>(i) * invSteps;
        hit.x = start.x + delta.x * t;
        hit.y = start.y + delta.y * t;
        hit.z = delta.z * t + start.z;
        float z = hit.z;
        float height = this->GetSubCellHeight(cell, z);

        if (height > z) {
            hit.z = height;
            return true;
        }

        if (error > 0) {
            error += diagonal;
            cell.x += diagX;
            cell.y += diagY;
        } else {
            error += straight;
            cell.x += stepX;
            cell.y += stepY;
        }

        if (i == steps) {
            hit = end;
        }

        if (i + 1 > steps) {
            return false;
        }
    }

    hit = start;
    return false;
}

// OFFSET: 0x007495C0
void CHMMgr::Update(const C3Vector& pos) {
    int32_t originX = static_cast<int32_t>(floor(pos.x * 0.029999999f));
    int32_t originY = static_cast<int32_t>(floor(pos.y * 0.029999999f));

    if (originX == this->m_originX && originY == this->m_originY) {
        return;
    }

    this->m_originY = originY;
    this->m_minCellY = originY - 2;
    this->m_minCellX = originX - 2;
    this->m_originX = originX;
    this->m_minX = 32 * (originX - 2);
    this->m_minY = 32 * this->m_minCellY;
    WeatherGridCell* cells[25];

    for (uint32_t y = 0; y < 5; y++) {
        for (uint32_t x = 0; x < 5; x++) {
            cells[5 * y + x] = this->m_cells[y][x];
            this->m_cells[y][x] = NULL;
        }
    }

    for (int32_t y = 0; y < 5; y++) {
        for (int32_t x = 0; x < 5; x++) {
            int32_t cellX = x + this->m_minCellX;

            for (uint32_t i = 0; i < 25; i++) {
                WeatherGridCell* cell = cells[i];

                if (cell && cell->m_cellX == cellX && cell->m_cellY == y + this->m_minCellY) {
                    this->m_cells[y][x] = cells[i];
                    cells[i] = NULL;
                    break;
                }
            }
        }
    }

    for (int32_t y = 0; y < 5; y++) {
        for (int32_t x = 0; x < 5; x++) {
            if (this->m_cells[y][x]) {
                continue;
            }

            for (uint32_t i = 0; i < 25; i++) {
                if (!cells[i]) {
                    continue;
                }

                this->m_cells[y][x] = cells[i];
                cells[i] = NULL;
                WeatherGridCell* cell = this->m_cells[y][x];
                cell->m_cellX = x + this->m_minCellX;
                cell->m_cellY = y + this->m_minCellY;

                for (uint32_t j = 0; j < 1024; j++) {
                    cell->m_heights[j] = FLT_MAX;
                }

                break;
            }
        }
    }
}

// OFFSET: 0x007510B0/0x1404319E0
Weather::Weather() {
    this->m_densityBlend = 1.0f;
    this->m_startDensityBlend = 1.0f;
    this->m_targetDensityBlend = 1.0f;
    this->m_targetIntensity = 0.0f;
    this->m_startIntensity = 0.0f;
    this->m_intensity = 0.0f;
    this->m_targetFogIntensity = 0.0f;
    this->m_startFogIntensity = 0.0f;
    this->m_fogIntensity = 0.0f;
    this->m_intensityStartTime = 0;
    this->m_fogStartTime = 0;
    this->m_pendingType = -1;
    this->m_densityChanged = 0;
    this->m_transition = 0;
    this->m_color.b = 0xFF;
    this->m_color.g = 0xFF;
    this->m_color.r = 0xFF;
    this->m_color.a = 0xFF;
    this->m_rain = NULL;
    this->m_snow = NULL;
    this->m_sand = NULL;
    this->m_hmMgr = NULL;
    this->m_position = C3Vector(FLT_MAX, FLT_MAX, FLT_MAX);
    this->m_onTaxi = 0;
    this->m_facing = 0.0f;
    this->m_speed = 0.0f;
    this->m_updated = 0;
    void* mem = SMemAlloc(sizeof(CHMMgr), __FILE__, __LINE__, 0);
    this->m_hmMgr = mem ? new (mem) CHMMgr() : NULL;
    this->m_precipTexture[0] = '\0';
}

// OFFSET: 0x007511F0
void Weather::CalcPlayerVelocity(const C3Vector& position, const C3Vector& velocity) {
    uint32_t total = 0;

    for (PosDelta* delta = this->m_posDeltas.Head(), * next; delta; delta = next) {
        total += delta->m_time;
        next = this->m_posDeltas.Next(delta);

        if (total >= 150) {
            this->m_posDeltas.DeleteNode(this->m_posDeltas.Head());
        }
    }

    uint32_t now = FrameTime::GetCurTimeMs();
    static uint32_t s_lastTime = now;

    if (now != s_lastTime) {
        float dx = position.x - this->m_position.x;
        float dy = position.y - this->m_position.y;
        float dz = position.z - this->m_position.z;
        PosDelta* delta = this->m_posDeltas.NewNode(2, 0, 0);
        delta->m_delta.x = dx;
        delta->m_delta.y = dy;
        delta->m_time = now - s_lastTime;
        delta->m_delta.z = dz;
    }

    s_lastTime = now;
    uint32_t time = 0;
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;

    for (PosDelta* delta = this->m_posDeltas.Head(); delta; delta = this->m_posDeltas.Next(delta)) {
        x = delta->m_delta.x + x;
        time += delta->m_time;
        y = delta->m_delta.y + y;
        z = delta->m_delta.z + z;
    }

    double seconds = static_cast<double>(time + 1) * 0.001;
    this->m_velocity.x = static_cast<float>(1.0 / seconds) * x;
    this->m_velocity.y = static_cast<float>(1.0 / seconds) * y;
    this->m_velocity.z = static_cast<float>(1.0 / seconds) * z;

    if (fabs(velocity.x) < 0.00000023841858f && fabs(velocity.y) < 0.00000023841858f && fabs(velocity.z) < 0.00000023841858f) {
        this->m_velocity.x = 0.0f;
        this->m_velocity.y = 0.0f;
        this->m_velocity.z = 0.0f;
    }
}

// OFFSET: 0x00752E90
void Weather::Clear() {
    Rain* rain = this->m_rain;

    if (rain) {
        rain->~Rain();
        SMemFree(rain, "delete", -1, 0);
    }

    this->m_rain = NULL;

    Snow* snow = this->m_snow;

    if (snow) {
        snow->~Snow();
        SMemFree(snow, "delete", -1, 0);
    }

    this->m_snow = NULL;

    Sand* sand = this->m_sand;

    if (sand) {
        sand->~Sand();
        SMemFree(sand, "delete", -1, 0);
    }

    this->m_sand = NULL;
}

// OFFSET: 0x00752F10/0x140434100
void Weather::Update() {
    if (this->m_position.x == FLT_MAX) {
        this->m_position = World::GetWeatherPosition();
    }

    const C3Vector& velocity = World::GetWeatherVelocity();
    this->CalcPlayerVelocity(World::GetWeatherPosition(), velocity);
    float vx = this->m_velocity.x;
    float vy = this->m_velocity.y;
    float speedSq = vy * vy + vx * vx;

    if (speedSq < 1.0f || World::GetWeatherOnTaxi()) {
        this->m_facing = World::GetWeatherFacing();
    } else {
        float inv = 1.0f / sqrt(speedSq);
        float facing = atan2(inv * vy, vx * inv);
        this->m_facing = facing;

        if (facing < 0.0f) {
            this->m_facing = facing + s_twoPi;
        }
    }

    this->m_onTaxi = World::GetWeatherOnTaxi();
    this->m_position = World::GetWeatherPosition();
    this->m_speed = sqrt(this->m_velocity.x * this->m_velocity.x + this->m_velocity.y * this->m_velocity.y);
    int32_t fading = 0;

    if (this->m_pendingType != -1) {
        uint32_t active = 0;

        if (this->m_rain) {
            active = this->m_rain->NumActive();
        }

        if (this->m_snow) {
            active += this->m_snow->NumActive();
        }

        if (this->m_sand) {
            active += this->m_sand->NumActive();
        }

        if (static_cast<int32_t>(active) > 0 && this->m_transition) {
            float target = this->m_targetIntensity < 0.25f ? this->m_targetIntensity : 0.25f;
            this->SetStormIntensity(this->m_startIntensity, target);
            fading = 1;

            if (this->m_snow) {
                this->m_snow->m_fadeOut = 1;
                this->m_snow->m_mists.m_fadeOut = 1;
            }

            if (this->m_rain) {
                this->m_rain->m_fadeOut = 1;
                this->m_rain->m_mists.m_fadeOut = 1;
            }

            if (this->m_sand) {
                this->m_sand->m_fadeOut = 1;
                this->m_sand->m_mists.m_fadeOut = 1;
            }
        } else {
            this->Clear();

            switch (this->m_pendingType) {
                case 1: {
                    void* mem = SMemAlloc(sizeof(Rain), __FILE__, __LINE__, 0);
                    this->m_rain = mem ? new (mem) Rain(this, 130.0f, 75.0f) : NULL;
                    break;
                }

                case 2: {
                    void* mem = SMemAlloc(sizeof(Snow), __FILE__, __LINE__, 0);
                    this->m_snow = mem ? new (mem) Snow(this, 90.0f, 60.0f) : NULL;
                    break;
                }

                case 3: {
                    void* mem = SMemAlloc(sizeof(Sand), __FILE__, __LINE__, 0);
                    this->m_sand = mem ? new (mem) Sand(this, 40.0f, 25.0f) : NULL;
                    break;
                }
            }

            uint32_t now = FrameTime::GetCurTimeMs();
            this->m_startIntensity = this->m_intensity;
            this->m_intensityStartTime = now;
            this->m_pendingType = -1;
        }
    }

    if (this->m_rain || this->m_snow || this->m_sand) {
        this->m_hmMgr->Update(CWorldScene::s_defaultWorldView->m_camPos);
    }

    if (((fabs(this->m_intensity - this->m_targetIntensity) >= 0.00000023841858f || fabs(this->m_fogIntensity - this->m_targetFogIntensity) >= 0.00000023841858f || fabs(this->m_densityBlend - this->m_targetDensityBlend) >= 0.00000023841858f) && !fading) || this->m_densityChanged) {
        this->SetStormIntensity(this->m_startIntensity, this->m_targetIntensity);
    }

    this->m_updated = 1;
    this->m_transportVelocity.x = 0.0f;
    this->m_transportVelocity.y = 0.0f;
    this->m_transportVelocity.z = 0.0f;
}

// OFFSET: 0x007532D0
Weather::~Weather() {
    this->Clear();

    if (this->m_hmMgr) {
        this->m_hmMgr->~CHMMgr();
        SMemFree(this->m_hmMgr, "delete", -1, 0);
    }

    this->m_hmMgr = NULL;
    this->m_posDeltas.Clear();
}

// OFFSET: 0x007525F0
void Weather::Render() {
    if (!this->m_updated) {
        return;
    }

    if (this->m_rain) {
        this->m_rain->Render();
    }

    if (this->m_snow) {
        this->m_snow->Render();
    }

    if (this->m_sand) {
        this->m_sand->Render();
    }
}

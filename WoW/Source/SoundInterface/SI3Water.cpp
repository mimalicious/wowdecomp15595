#include <cfloat>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include "WoW/Source/SoundInterface/SI3Water.h"
#include "Engine/Source/ConsoleBase/ConsoleVar.h"
#include "Engine/Source/Event/Event.h"
#include "Engine/Source/Os/TimeManager.h"
#include "Engine/Source/SoundEngine/SE3.h"
#include "Engine/Source/Storm/H/stpl.h"
#include "Engine/Source/Storm/H/storm.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/LiquidTypeRec.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/SoundInterface/SI3.h"
#include "WoW/Source/Ui/GameUI.h"
#include "WoW/Source/WorldClient/World.h"

// STATIC: 0x00E28888
static int32_t* s_liquidTypeWaterSounds;

// STATIC: 0x00E2888C
static SI3WaterSound* s_waterSounds;

// STATIC: 0x00E28890
static int32_t s_numWaterSounds;

// STATIC: 0x00E28894
static float s_waterSoundRadius;

// OFFSET: 0x00634890/0x1402CAAB0
static void _StopAllWaterSounds() {
    for (int32_t i = 0; i < s_numWaterSounds; i++) {
        SE3::StopSound(s_waterSounds[i].m_handle, -1, __FILE__, __LINE__, "_StopAllWaterSounds");
    }
}

// OFFSET: 0x006348D0/0x1402CAB30
static int32_t _OkToQueryLiquidSounds() {
    // STATIC: 0x00E288B4
    static uint32_t s_nextQueryTime = OsGetAsyncTimeMs();

    if (static_cast<int32_t>(OsGetAsyncTimeMs() - s_nextQueryTime) < 0) {
        return 0;
    }

    s_nextQueryTime = OsGetAsyncTimeMs() + 100;

    // STATIC: 0x00E288B0
    static CVar* s_enableAllSound = CVar::Lookup("Sound_EnableAllSound");
    // STATIC: 0x00E288AC
    static CVar* s_enableAmbience = CVar::Lookup("Sound_EnableAmbience");

    if (!s_enableAllSound || !s_enableAllSound->m_intValue || !s_enableAmbience || !s_enableAmbience->m_intValue) {
        return 0;
    }

    if (CGGameUI::m_inMovie) {
        return 0;
    }

    CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();

    if (!player || !player->m_worldObject) {
        return 0;
    }

    // STATIC: 0x00E288A0
    static C3Vector s_lastPosition;

    C3Vector position = player->GetPosition();

    if (position == s_lastPosition) {
        return 0;
    }

    s_lastPosition = position;
    return 1;
}

// OFFSET: 0x00634A30/0x1402CACC0
static int32_t _HandleWaterSounds(const void* data, void* param) {
    if (SI3::IsUnderwater()) {
        _StopAllWaterSounds();
        return 1;
    }

    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player || !player->m_worldObject || !_OkToQueryLiquidSounds()) {
        return 1;
    }

    for (int32_t i = 0; i < s_numWaterSounds; i++) {
        SI3WaterSound& sound = s_waterSounds[i];
        sound.m_direction.x = 0.0f;
        sound.m_direction.y = 0.0f;
        sound.m_direction.z = 0.0f;
        sound.m_nearestDistanceSq = FLT_MAX;
        sound.m_active = 0;
    }

    C3Vector position = player->GetPosition();
    uintptr_t worldObject = player->m_worldObject;
    TSGrowableArray<World::LiquidQueryResult> results;
    World::QueryLiquidSounds(worldObject, s_waterSoundRadius, results);

    for (uint32_t i = 0; i < results.Count(); i++) {
        World::LiquidQueryResult& result = results[i];
        LiquidTypeRec* liquidType = g_liquidTypeDB.GetRecord(result.m_liquidType);

        if ((liquidType->m_flags & 0x80) && worldObject && World::QueryObjectInside(worldObject, 0)) {
            continue;
        }

        C3Vector direction = result.m_offset;
        SI3WaterSound& sound = s_waterSounds[s_liquidTypeWaterSounds[result.m_liquidType]];
        float distanceSq = direction.SquaredMag();
        sound.m_active = 1;

        if (sound.m_nearestDistanceSq > distanceSq) {
            sound.m_nearestDistanceSq = distanceSq;
        }

        float weight = (s_waterSoundRadius * s_waterSoundRadius - distanceSq) / (s_waterSoundRadius * s_waterSoundRadius);
        direction.NormalizeSafe();
        direction *= weight / sqrt(direction.SquaredMag());
        sound.m_direction += direction;
    }

    for (int32_t i = 0; i < s_numWaterSounds; i++) {
        SI3WaterSound& sound = s_waterSounds[i];

        if (!sound.m_active) {
            SE3::StopSound(sound.m_handle, 500, __FILE__, __LINE__, "_HandleWaterSounds");
            continue;
        }

        sound.m_direction.NormalizeSafe();
        sound.m_direction *= sqrt(sound.m_nearestDistanceSq) / sqrt(sound.m_direction.SquaredMag());
        C3Vector soundPosition = position + sound.m_direction;

        if (SE3::IsPlaying(sound.m_handle)) {
            SE3::Set3DPosition(sound.m_handle, soundPosition);
        } else {
            SE3SoundKitProperties properties;
            properties.m_bus = SE3BUS_AMBIENCE;
            properties.m_usage = 1;
            properties.m_fadeInTime = 1000;
            SE3::PlaySoundKit(sound.m_soundKitId, &soundPosition, &properties, &sound.m_handle, __FILE__, __LINE__, "_HandleWaterSounds");
        }
    }

    return 1;
}

// OFFSET: 0x00634EB0/0x1402CB220
void SI3::WaterInit() {
    s_numWaterSounds = g_liquidTypeDB.m_numRecords;
    s_waterSounds = static_cast<SI3WaterSound*>(SMemAlloc(sizeof(SI3WaterSound) * s_numWaterSounds, __FILE__, __LINE__, 0));
    s_liquidTypeWaterSounds = static_cast<int32_t*>(SMemAlloc(sizeof(int32_t) * (g_liquidTypeDB.m_maxID + 1), __FILE__, __LINE__, 0));
    float radius = 0.0f;
    s_waterSoundRadius = 0.0f;

    for (int32_t i = 0; i < s_numWaterSounds; i++) {
        SI3WaterSound& sound = s_waterSounds[i];
        sound.m_handle = 0;
        sound.m_soundKitId = 0;
        sound.m_direction.x = 0.0f;
        sound.m_direction.y = 0.0f;
        sound.m_direction.z = 0.0f;
        sound.m_nearestDistanceSq = FLT_MAX;
        sound.m_distanceCutoff = 0.0f;
        sound.m_active = 0;

        LiquidTypeRec* liquidType = g_liquidTypeDB.GetRecordByIndex(i);

        if (!liquidType) {
            continue;
        }

        SoundEntriesRec* soundEntry = g_soundEntriesDB.GetRecord(liquidType->m_soundID);

        if (soundEntry) {
            sound.m_distanceCutoff = soundEntry->m_distanceCutoff;

            if (sound.m_distanceCutoff > radius) {
                radius = sound.m_distanceCutoff;
            }

            sound.m_soundKitId = liquidType->m_soundID;
        }

        s_liquidTypeWaterSounds[liquidType->m_id] = i;
    }

    s_waterSoundRadius = radius;
    EventRegister(EVENT_ID_IDLE, _HandleWaterSounds);
}

// INLINED
void SI3::WaterShutdown() {
    _StopAllWaterSounds();
    EventUnregister(EVENT_ID_IDLE, _HandleWaterSounds);

    if (s_waterSounds) {
        SMemFree(s_waterSounds, NULL, 0, 0);
        s_waterSounds = NULL;
        s_numWaterSounds = 0;
    }

    if (s_liquidTypeWaterSounds) {
        SMemFree(s_liquidTypeWaterSounds, NULL, 0, 0);
        s_liquidTypeWaterSounds = NULL;
    }
}

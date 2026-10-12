#include <cstddef>
#include <cstdint>
#include "WoW/Source/SoundInterface/SI3Footsteps.h"
#include "Engine/Source/ConsoleBase/ConsoleVar.h"
#include "Engine/Source/SoundEngine/SE3.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/Object/ObjectClient/Unit_C.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/SoundInterface/SI3.h"

// STATIC: 0x00E28858
static HASHKEY_NONE s_footstepKey;

// STATIC: 0x00E2885C/0x140B69430
static TSHashTable<FOOTSTEPSNDCACHE, HASHKEY_NONE> s_footstepSounds;

// PROVEN: f504e212 IDENTICAL/DIFFERENT
// OFFSET: 0x00633C50/0x1402C9A10
void SI3::ShutDownFootstepSounds() {
    s_footstepSounds.Clear();
}

// OFFSET: 0x006338C0/0x1402C9570
static uint32_t GetFootstepSound(uint32_t terrainType, int32_t footstepId, int32_t splash) {
    TerrainTypeRec* terrain = g_terrainTypeDB.GetRecord(terrainType);

    if (!terrain) {
        return 0;
    }

    int32_t index = terrain->m_soundID;
    FOOTSTEPSNDCACHE* cache = s_footstepSounds.Ptr(footstepId, s_footstepKey);

    if (!cache) {
        return 0;
    }

    if (splash) {
        return cache->m_splashSounds[index];
    }

    return cache->m_sounds[index];
}

// OFFSET: 0x00633930/0x1402C9610
void SI3::PlayFootstepSound(int32_t footstepId, const C3Vector* position, uint32_t terrainType, int32_t splash, int32_t loud, CGObject_C* object) {
    if (!object) {
        return;
    }

    uint32_t soundKitId = GetFootstepSound(terrainType, footstepId, splash);

    if (!soundKitId) {
        soundKitId = GetFootstepSound(0, footstepId, splash);

        if (!soundKitId) {
            return;
        }
    }

    CGObjectData* data = object->m_obj;

    if ((data->m_type & TYPEMASK_PLAYER) && data->m_guid == ClntObjMgrGetActivePlayer()) {
        if (SI3::sm_pCVar_Sound_ListenerAtCharacter && SI3::sm_pCVar_Sound_ListenerAtCharacter->m_intValue) {
            SE3SoundKitProperties properties;
            properties.unk_002C = loud ? 50.0f : 20.0f;
            properties.m_bus = SE3BUS_20;
            properties.m_priority = 115;
            SE3::PlaySoundKit(soundKitId, NULL, &properties, NULL, __FILE__, __LINE__, "Player's Footstep Sound (2D)");
        } else {
            SE3SoundKitProperties properties;
            properties.m_bus = SE3BUS_20;
            properties.m_priority = 115;
            SE3::PlaySoundKit(soundKitId, position, &properties, NULL, __FILE__, __LINE__, "Player's Footstep Sound (3D)");
        }
    } else {
        SE3SoundKitProperties properties;
        properties.m_bus = SE3BUS_16;
        properties.m_priority = 128;
        SE3::PlaySoundKit(soundKitId, position, &properties, NULL, __FILE__, __LINE__, "Footstep Sound");
    }
}

// OFFSET: 0x00634400/0x1402CA4D0
void SI3::InitFootstepSounds() {
    s_footstepSounds.Clear();

    uint32_t count = g_terrainTypeSoundsDB.m_maxID + 1;

    for (int32_t i = g_footstepTerrainLookupDB.m_numRecords; i;) {
        i--;

        FootstepTerrainLookupRec* rec = g_footstepTerrainLookupDB.GetRecordByIndex(i);

        if (!rec || rec->m_terrainSoundID < 0) {
            continue;
        }

        FOOTSTEPSNDCACHE* cache = s_footstepSounds.Ptr(rec->m_creatureFootstepID, s_footstepKey);

        if (!cache) {
            cache = s_footstepSounds.New(rec->m_creatureFootstepID, s_footstepKey, 0, 0);
            cache->m_sounds.SetCount(count);
            cache->m_splashSounds.SetCount(count);

            for (uint32_t j = 0; j < count; j++) {
                cache->m_sounds[j] = 0;
                cache->m_splashSounds[j] = 0;
            }
        }

        cache->m_sounds[rec->m_terrainSoundID] = rec->m_soundID;
        cache->m_splashSounds[rec->m_terrainSoundID] = rec->m_soundIdsplash;
    }
}

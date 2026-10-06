#include <cstddef>
#include <cstdint>
#include <cstring>
#include <new>
#include "WoW/Source/SoundInterface/SoundInterface2Internal.h"
#include "Engine/Source/BlizzardCore/SFile2-Core.h"
#include "Engine/Source/Storm/H/storm.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/SoundInterface/SoundInterface2.h"
#include "WoW/Source/SoundInterface/SoundInterface2ZoneSounds.h"
#include "WoW/Source/Ui/CGWorldStateInfo.h"

// STATIC: #0x00B4AD74
static TSGrowableArray<WorldStateZoneSoundOverride*> s_worldStateZoneSoundOverrides;

// STATIC: #0x00B4AD84
static TSHashTable<ZONESOUNDOVERRIDELOOKUP, HASHKEY_STRI> s_zoneSoundOverrides;

// OFFSET: #0x004CBE70
int32_t SI2::UpdateZoneSoundsForWorldState(int32_t areaId, int32_t zoneId, int32_t wmoAreaId, int32_t wmoGroupAreaId, int32_t inside) {
    int32_t count = s_worldStateZoneSoundOverrides.Count();
    int32_t found = 0;

    if (inside) {
        for (int32_t i = 0; i < count; i++) {
            WorldStateZoneSoundOverride* entry = s_worldStateZoneSoundOverrides[i];

            if ((wmoAreaId > 0 && wmoAreaId == entry->m_wmoAreaId) || (wmoGroupAreaId > 0 && wmoGroupAreaId == entry->m_wmoAreaId)) {
                if (GetWorldState(entry->m_worldStateId) == entry->m_worldStateValue) {
                    SI2::SetZoneReverb(1, s_worldStateZoneSoundOverrides[i]->m_soundProviderPreferencesId);
                    SI2::SetZoneAmbienceID(1, s_worldStateZoneSoundOverrides[i]->m_soundAmbienceId);
                    SI2::SetZoneIntroMusicID(6, s_worldStateZoneSoundOverrides[i]->m_zoneIntroMusicId);
                    SI2::SetZoneMusicID(1, s_worldStateZoneSoundOverrides[i]->m_zoneMusicId);
                    found = 1;

                    if (wmoGroupAreaId == s_worldStateZoneSoundOverrides[i]->m_wmoAreaId) {
                        return 1;
                    }
                }
            }
        }

        if (found) {
            return 1;
        }
    } else if (wmoAreaId > 0 || wmoGroupAreaId > 0) {
        int32_t best = 4;

        for (int32_t i = 0; i < count; i++) {
            WorldStateZoneSoundOverride* entry = s_worldStateZoneSoundOverrides[i];
            int32_t rank = 4;

            if (areaId > 0 && areaId == entry->m_areaId) {
                rank = 3;
            }

            if (zoneId > 0 && zoneId == entry->m_areaId) {
                rank = 2;
            }

            if (wmoGroupAreaId > 0 && wmoGroupAreaId == entry->m_wmoAreaId) {
                rank = 0;
            }

            if (rank < best && GetWorldState(entry->m_worldStateId) == entry->m_worldStateValue) {
                SI2::SetZoneReverb(1, s_worldStateZoneSoundOverrides[i]->m_soundProviderPreferencesId);
                SI2::SetZoneAmbienceID(1, s_worldStateZoneSoundOverrides[i]->m_soundAmbienceId);
                SI2::SetZoneIntroMusicID(6, s_worldStateZoneSoundOverrides[i]->m_zoneIntroMusicId);
                SI2::SetZoneMusicID(1, s_worldStateZoneSoundOverrides[i]->m_zoneMusicId);
                found = 1;
                best = rank;

                if (!rank) {
                    return 1;
                }
            }
        }

        if (found) {
            return 1;
        }
    } else {
        for (int32_t i = 0; i < count; i++) {
            WorldStateZoneSoundOverride* entry = s_worldStateZoneSoundOverrides[i];

            if ((areaId > 0 && areaId == entry->m_areaId) || (zoneId > 0 && zoneId == entry->m_areaId)) {
                if (GetWorldState(entry->m_worldStateId) == entry->m_worldStateValue) {
                    SI2::SetZoneReverb(1, s_worldStateZoneSoundOverrides[i]->m_soundProviderPreferencesId);
                    SI2::SetZoneAmbienceID(1, s_worldStateZoneSoundOverrides[i]->m_soundAmbienceId);
                    SI2::SetZoneIntroMusicID(6, s_worldStateZoneSoundOverrides[i]->m_zoneIntroMusicId);
                    SI2::SetZoneMusicID(1, s_worldStateZoneSoundOverrides[i]->m_zoneMusicId);
                    found = 1;

                    if (zoneId == s_worldStateZoneSoundOverrides[i]->m_areaId) {
                        return 1;
                    }
                }
            }
        }

        if (found) {
            return 1;
        }
    }

    SI2::SetZoneReverb(1, -1);
    SI2::SetZoneAmbienceID(1, -1);
    SI2::SetZoneIntroMusicID(6, -1);
    SI2::SetZoneMusicID(1, -1);
    return 0;
}

// OFFSET: #0x004CC1B0
void SI2::InitWorldStateZoneSoundOverrides() {
    uint32_t count = g_worldStateZoneSoundsDB.m_numRecords;
    s_worldStateZoneSoundOverrides.SetCount(count);
    memset(s_worldStateZoneSoundOverrides.Ptr(), 0, sizeof(WorldStateZoneSoundOverride*) * count);

    for (uint32_t i = 0; i < s_worldStateZoneSoundOverrides.Count(); i++) {
        s_worldStateZoneSoundOverrides[i] = static_cast<WorldStateZoneSoundOverride*>(SMemAlloc(sizeof(WorldStateZoneSoundOverride), __FILE__, __LINE__, 0));

        WorldStateZoneSoundsRec* rec = (static_cast<int32_t>(i) < 0 || static_cast<int32_t>(i) >= g_worldStateZoneSoundsDB.m_numRecords) ? NULL : &g_worldStateZoneSoundsDB.m_records[i];

        s_worldStateZoneSoundOverrides[i]->m_areaId = rec->m_areaID;
        s_worldStateZoneSoundOverrides[i]->m_wmoAreaId = rec->m_wmoareaID;
        s_worldStateZoneSoundOverrides[i]->m_worldStateId = rec->m_worldStateID;
        s_worldStateZoneSoundOverrides[i]->m_worldStateValue = rec->m_worldStateValue;
        s_worldStateZoneSoundOverrides[i]->m_zoneIntroMusicId = rec->m_zoneIntroMusicID;
        s_worldStateZoneSoundOverrides[i]->m_zoneMusicId = rec->m_zoneMusicID;
        s_worldStateZoneSoundOverrides[i]->m_soundAmbienceId = rec->m_soundAmbienceID;
        s_worldStateZoneSoundOverrides[i]->m_soundProviderPreferencesId = rec->m_soundProviderPreferencesID;
    }
}

// OFFSET: #0x004CC2F0
void SI2::FreeWorldStateZoneSoundOverrides() {
    for (uint32_t i = 0; i < s_worldStateZoneSoundOverrides.Count(); i++) {
        if (s_worldStateZoneSoundOverrides[i]) {
            delete s_worldStateZoneSoundOverrides[i];
        }
    }

    s_worldStateZoneSoundOverrides.Clear();
}

// OFFSET: #0x004CC6E0
void SI2::FreeZoneSoundOverrides() {
    s_zoneSoundOverrides.Clear();
}

// OFFSET: #0x004CCA30
int32_t SI2::GetZoneSoundOverride(int32_t mapId, int32_t chunkX, int32_t chunkY, int32_t subchunkX, int32_t subchunkY, int32_t* zoneIntroMusicId, int32_t* zoneMusicId, int32_t* soundAmbienceId, int32_t* soundProviderPreferencesId) {
    char key[64];
    SStrPrintf(key, sizeof(key), "%010d_%010d_%010d_%010d_%010d", mapId, chunkX, chunkY, subchunkX, subchunkY);

    ZONESOUNDOVERRIDELOOKUP* lookup = s_zoneSoundOverrides.Ptr(key);

    if (!lookup) {
        return 0;
    }

    *zoneIntroMusicId = lookup->m_zoneIntroMusicId;
    *zoneMusicId = lookup->m_zoneMusicId;
    *soundAmbienceId = lookup->m_soundAmbienceId;
    *soundProviderPreferencesId = lookup->m_soundProviderPreferencesId;
    return 1;
}

// OFFSET: #0x004CCAA0
int32_t SI2::LookupSoundKitId(const char* name) {
    return SE3::GetSoundKitID(name);
}

// OFFSET: #0x004CD870
void SI2::InitZoneSoundOverrides() {
    char key[64];

    for (int32_t i = 0; i < g_worldChunkSoundsDB.m_numRecords; i++) {
        WorldChunkSoundsRec* rec = g_worldChunkSoundsDB.GetRecordByIndex(i);

        SStrPrintf(key, sizeof(key), "%010d_%010d_%010d_%010d_%010d", rec->m_id, rec->m_chunkX, rec->m_chunkY, rec->m_subchunkX, rec->m_subchunkY);

        ZONESOUNDOVERRIDELOOKUP* lookup = s_zoneSoundOverrides.Ptr(key);

        if (!lookup) {
            lookup = s_zoneSoundOverrides.New(key, 0, 0);
        }

        lookup->m_zoneIntroMusicId = rec->m_zoneIntroMusicID;
        lookup->m_zoneMusicId = rec->m_zoneMusicID;
        lookup->m_soundAmbienceId = rec->m_soundAmbienceID;
        lookup->m_soundProviderPreferencesId = rec->m_soundProviderPreferencesID;
    }
}

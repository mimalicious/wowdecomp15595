#include "WoW/Source/WorldClient/AreaList.h"
#include "Engine/Source/Base/CDataStore.h"
#include "Engine/Source/Storm/H/storm.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/AreaTableRec.h"
#include "WoW/Source/DB/Rec/WMOAreaTableRec.h"
#include "WoW/Source/Net/Types.h"
#include "WoW/Source/SoundInterface/SI3.h"
#include "WoW/Source/Ui/GameUI.h"
#include "WoW/Source/WorldClient/World.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"

// STATIC: 0x00CA0EF4
static int32_t s_lastAreaID = -1;

// STATIC: 0x00CA0EF8
static int32_t s_lastSubAreaID = -1;

// STATIC: 0x00CA0EFC
static int32_t s_lastMapID = -1;

// STATIC: 0x00CA0F00
static int32_t s_lastWMOID = -1;

// STATIC: 0x00CA0F04
static int32_t s_lastWMOHash = -1;

// STATIC: 0x00CA0F08
static int32_t s_lastWMOGroupNum = -1;

// STATIC: 0x00CA0F0C
static uint32_t s_wmoGroupID = static_cast<uint32_t>(-1);

// STATIC: 0x00DC0D34
static int32_t s_inMapObj;

// PROVEN: 9305d4f7 IDENTICAL/NONE
// OFFSET: 0x004D9980
void AreaListShutdown() {
    s_lastAreaID = -1;
    s_lastSubAreaID = -1;
    s_lastMapID = -1;
    s_inMapObj = 0;
    s_lastWMOID = -1;
    s_lastWMOHash = -1;
    s_lastWMOGroupNum = -1;
    s_wmoGroupID = static_cast<uint32_t>(-1);
}

// OFFSET: 0x004D99C0/0x14011EEE0
void AreaListUpdateSounds(const AreaTableRec* zone, const AreaTableRec* area, const WMOAreaTableRec* rootRec, const WMOAreaTableRec* groupRec, int32_t inside, int32_t mapID) {
    int32_t introSound;
    int32_t zoneMusic;
    int32_t ambience;
    int32_t reverb;
    int32_t uwIntroSound;
    int32_t uwZoneMusic;
    int32_t uwAmbience;
    int32_t uwReverb;

    if (inside) {
        introSound = groupRec && groupRec->m_introSound ? groupRec->m_introSound : (rootRec ? rootRec->m_introSound : 0);
        zoneMusic = groupRec && groupRec->m_zoneMusic ? groupRec->m_zoneMusic : (rootRec ? rootRec->m_zoneMusic : 0);
        ambience = groupRec && groupRec->m_ambienceID ? groupRec->m_ambienceID : (rootRec ? rootRec->m_ambienceID : 0);
        reverb = groupRec && groupRec->m_soundProviderPref ? groupRec->m_soundProviderPref : (rootRec ? rootRec->m_soundProviderPref : 0);
        uwIntroSound = groupRec && groupRec->m_uwIntroSound ? groupRec->m_uwIntroSound : (rootRec ? rootRec->m_uwIntroSound : 0);
        uwZoneMusic = groupRec && groupRec->m_uwZoneMusic ? groupRec->m_uwZoneMusic : (rootRec ? rootRec->m_uwZoneMusic : 0);
        uwAmbience = groupRec && groupRec->m_uwAmbienceID ? groupRec->m_uwAmbienceID : (rootRec ? rootRec->m_uwAmbienceID : 0);
        uwReverb = groupRec && groupRec->m_soundProviderPrefUnderwater ? groupRec->m_soundProviderPrefUnderwater : (rootRec ? rootRec->m_soundProviderPrefUnderwater : 0);
    } else {
        if ((groupRec && groupRec->m_introSound) || (rootRec && rootRec->m_introSound)) {
            introSound = groupRec && groupRec->m_introSound ? groupRec->m_introSound : (rootRec ? rootRec->m_introSound : 0);
        } else {
            introSound = area && area->m_introSound ? area->m_introSound : (zone ? zone->m_introSound : 0);
        }

        if ((groupRec && groupRec->m_zoneMusic) || (rootRec && rootRec->m_zoneMusic)) {
            zoneMusic = groupRec && groupRec->m_zoneMusic ? groupRec->m_zoneMusic : (rootRec ? rootRec->m_zoneMusic : 0);
        } else {
            zoneMusic = area && area->m_zoneMusic ? area->m_zoneMusic : (zone ? zone->m_zoneMusic : 0);
        }

        if ((groupRec && groupRec->m_ambienceID) || (rootRec && rootRec->m_ambienceID)) {
            ambience = groupRec && groupRec->m_ambienceID ? groupRec->m_ambienceID : (rootRec ? rootRec->m_ambienceID : 0);
        } else {
            ambience = area && area->m_ambienceID ? area->m_ambienceID : (zone ? zone->m_ambienceID : 0);
        }

        if ((groupRec && groupRec->m_soundProviderPref) || (rootRec && rootRec->m_soundProviderPref)) {
            reverb = groupRec && groupRec->m_soundProviderPref ? groupRec->m_soundProviderPref : (rootRec ? rootRec->m_soundProviderPref : 0);
        } else {
            reverb = area && area->m_soundProviderPref ? area->m_soundProviderPref : (zone ? zone->m_soundProviderPref : 0);
        }

        if ((groupRec && groupRec->m_uwIntroSound) || (rootRec && rootRec->m_uwIntroSound)) {
            uwIntroSound = groupRec && groupRec->m_uwIntroSound ? groupRec->m_uwIntroSound : (rootRec ? rootRec->m_uwIntroSound : 0);
        } else {
            uwIntroSound = area && area->m_uwIntroSound ? area->m_uwIntroSound : (zone ? zone->m_uwIntroSound : 0);
        }

        if ((groupRec && groupRec->m_uwZoneMusic) || (rootRec && rootRec->m_uwZoneMusic)) {
            uwZoneMusic = groupRec && groupRec->m_uwZoneMusic ? groupRec->m_uwZoneMusic : (rootRec ? rootRec->m_uwZoneMusic : 0);
        } else {
            uwZoneMusic = area && area->m_uwZoneMusic ? area->m_uwZoneMusic : (zone ? zone->m_uwZoneMusic : 0);
        }

        if ((groupRec && groupRec->m_uwAmbienceID) || (rootRec && rootRec->m_uwAmbienceID)) {
            uwAmbience = groupRec && groupRec->m_uwAmbienceID ? groupRec->m_uwAmbienceID : (rootRec ? rootRec->m_uwAmbienceID : 0);
        } else {
            uwAmbience = area && area->m_uwAmbienceID ? area->m_uwAmbienceID : (zone ? zone->m_uwAmbienceID : 0);
        }

        if ((groupRec && groupRec->m_soundProviderPrefUnderwater) || (rootRec && rootRec->m_soundProviderPrefUnderwater)) {
            uwReverb = groupRec && groupRec->m_soundProviderPrefUnderwater ? groupRec->m_soundProviderPrefUnderwater : (rootRec ? rootRec->m_soundProviderPrefUnderwater : 0);
        } else {
            uwReverb = area && area->m_soundProviderPrefUnderwater ? area->m_soundProviderPrefUnderwater : (zone ? zone->m_soundProviderPrefUnderwater : 0);
        }
    }

    SI3::GetWorldStateZoneSoundOverride(zone ? zone->m_id : 0, area ? area->m_id : 0, rootRec ? rootRec->m_id : 0, groupRec ? groupRec->m_id : 0, inside);
    SI3::GetPhaseShiftZoneSoundOverride(zone ? zone->m_id : 0, area ? area->m_id : 0, rootRec ? rootRec->m_id : 0, groupRec ? groupRec->m_id : 0, inside);
    SI3::SetZoneIntroMusicID(SI3ZONESOUNDTYPE_4, introSound);
    SI3::SetZoneMusicID(SI3ZONESOUNDTYPE_0, zoneMusic);
    SI3::SetZoneAmbienceID(SI3ZONESOUNDTYPE_0, ambience);
    SI3::SetZoneReverb(SI3ZONESOUNDTYPE_0, reverb);
    SI3::SetZoneIntroMusicID(SI3ZONESOUNDTYPE_11, uwIntroSound);
    SI3::SetZoneMusicID(SI3ZONESOUNDTYPE_9, uwZoneMusic);
    SI3::SetZoneAmbienceID(SI3ZONESOUNDTYPE_9, uwAmbience);
    SI3::SetZoneUnderwaterReverb(uwReverb);
    SI3::SetPlayerInside(inside != 0);
    SI3::UpdateSoundEmitters(mapID);
}

// PROVEN: 17ee88f7 IDENTICAL/NONE
// OFFSET: 0x004D9E60
static bool AreaListUpdateCurrentZone(uintptr_t object, const char** zoneName, const char** subzoneName, uint32_t wmoID, uint32_t wmoHash, uint32_t wmoGroupNum, uint32_t areaID) {
    const char* wmoSubzoneName = NULL;
    const char* wmoZoneName = NULL;
    int32_t hasZoneName = World::QueryMapObjZoneName(object, wmoZoneName);
    int32_t hasSubzoneName = World::QueryMapObjSubzoneName(object, wmoSubzoneName, areaID);

    if (areaID == s_lastAreaID && wmoID == s_lastWMOID && wmoHash == s_lastWMOHash && wmoGroupNum == s_lastWMOGroupNum) {
        return false;
    }

    s_lastAreaID = areaID;
    s_lastWMOID = wmoID;
    s_lastWMOHash = wmoHash;
    s_lastWMOGroupNum = wmoGroupNum;

    if (hasZoneName && wmoZoneName && *wmoZoneName) {
        if (!*subzoneName || SStrCmpUTF8I(wmoZoneName, *subzoneName, STORM_MAX_STR)) {
            *zoneName = wmoZoneName;
            *subzoneName = NULL;
        }
    }

    if (hasSubzoneName && wmoSubzoneName && *wmoSubzoneName) {
        *subzoneName = wmoSubzoneName;
    }

    return true;
}

// PROVEN: 64abac18 IDENTICAL/NONE
// OFFSET: 0x004D9F50
static void AreaListUpdateSoundsForObject(uintptr_t object, const AreaTableRec* zone, const AreaTableRec* area) {
    const WMOAreaTableRec* groupRec = NULL;
    const WMOAreaTableRec* rootRec = NULL;
    int32_t inside = World::QueryObjectInside(object, 1);
    World::QueryMapObjAreaTable(object, groupRec, rootRec, s_wmoGroupID, 1);
    AreaListUpdateSounds(zone, area, rootRec, groupRec, inside, World::QueryMapId());
}

// OFFSET: 0x004D9FB0/0x14011F4D0
int32_t AreaListGetName(uint32_t areaID, char* buffer, uint32_t size, int32_t includeParent) {
    if (!buffer || !size) {
        return 0;
    }

    *buffer = '\0';
    AreaTableRec* area = g_areaTableDB.GetRecord(areaID);

    if (!area) {
        return 0;
    }

    AreaTableRec* parent;

    if (includeParent && area->m_parentAreaID && (parent = g_areaTableDB.GetRecord(area->m_parentAreaID)) != NULL) {
        SStrPrintf(buffer, size, "%s, %s", area->m_areaName, parent->m_areaName);
        return 1;
    }

    SStrCopy(buffer, area->m_areaName, size);
    return 1;
}

// OFFSET: 0x004DA080
static void AreaListSendZoneUpdate(int32_t zoneID) {
    if (!zoneID) {
        return;
    }

    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_ZONEUPDATE));
    msg.Put(static_cast<uint32_t>(zoneID));
    msg.Finalize();
    ClientServices::Send2(&msg);
}

// OFFSET: 0x004DA100
static bool AreaListUpdateReverbForLocation(int32_t mapID, int32_t zoneID, int32_t areaID) {
    if (!s_inMapObj && zoneID == s_lastAreaID && areaID == s_lastSubAreaID && mapID == s_lastMapID) {
        return false;
    }

    s_lastAreaID = zoneID;
    s_lastSubAreaID = areaID;
    s_lastMapID = mapID;
    s_wmoGroupID = 0;
    AreaListSendZoneUpdate(zoneID);
    return true;
}

// OFFSET: 0x004DA150/0x14011F6D0
AreaTableRec* AreaListGetObjectArea(uintptr_t object) {
    uint32_t areaIDs[3];

    if (!World::QueryAreaId(object, areaIDs)) {
        return NULL;
    }

    AreaTableRec* area = NULL;

    if (areaIDs[0]) {
        area = g_areaTableDB.GetRecord(areaIDs[0]);

        if (area && (area->m_flags & 0x400)) {
            return area;
        }

        area = NULL;
    }

    if (areaIDs[1]) {
        area = g_areaTableDB.GetRecord(areaIDs[1]);

        if (area) {
            return area;
        }
    }

    if (areaIDs[2]) {
        return g_areaTableDB.GetRecord(areaIDs[2]);
    }

    return area;
}

// OFFSET: 0x004DA1F0/0x14011F770
void AreaListRegisterLocation(int32_t mapID, uintptr_t object) {
    if (!object) {
        return;
    }

    AreaTableRec* objectArea = AreaListGetObjectArea(object);
    int32_t areaID = objectArea ? objectArea->m_id : 0;
    AreaTableRec* area = g_areaTableDB.GetRecord(areaID);

    if (!area) {
        return;
    }

    AreaTableRec* zone;
    int32_t zoneID;

    if (area->m_flags & 0x40000000) {
        zone = g_areaTableDB.GetRecord(area->m_parentAreaID);
        zoneID = zone->m_id;
    } else {
        zone = area;
        zoneID = areaID;
        areaID = 0;
        area = NULL;
    }

    if (!zoneID) {
        return;
    }

    const char* subzoneName = area ? static_cast<const char*>(area->m_areaName) : NULL;
    const char* zoneName = zone->m_areaName;
    uint32_t wmoID = static_cast<uint32_t>(-1);
    uint32_t wmoHash = static_cast<uint32_t>(-1);
    uint32_t wmoGroupNum = static_cast<uint32_t>(-1);
    int32_t inMapObj = World::QueryMapObjIDs(object, wmoID, wmoHash, wmoGroupNum);
    bool unchanged = inMapObj == s_inMapObj;
    s_inMapObj = inMapObj;
    bool updated;

    if (inMapObj || !unchanged) {
        updated = AreaListUpdateCurrentZone(object, &zoneName, &subzoneName, wmoID, wmoHash, wmoGroupNum, areaID ? areaID : zoneID);
    } else {
        updated = AreaListUpdateReverbForLocation(mapID, zone->m_id, area ? area->m_id : 0);
    }

    AreaListUpdateSoundsForObject(object, zone, area);
    SI3::OverrideZoneSounds();

    if (!updated) {
        return;
    }

    const char* zoneText = zoneName;
    CGGameUI::SetMinimapZoneText(subzoneName ? subzoneName : zoneName);
    const char* subZoneText = subzoneName && *subzoneName ? subzoneName : NULL;
    zoneText = zoneText && *zoneText ? zoneText : NULL;
    CGGameUI::NewZoneFeedback(zone->m_id, area ? area->m_id : 0, s_wmoGroupID, zoneText, subZoneText, World::QueryObjectInside(object, 0));
}

// OFFSET: 0x004DA3C0/0x14011FB00
bool AreaListZoneHasBreathParticles(uintptr_t object) {
    AreaTableRec* area;

    if (!World::QueryObjectInside(object, 0)) {
        area = AreaListGetObjectArea(object);

        if (!area) {
            return false;
        }
    } else {
        const WMOAreaTableRec* groupRec = NULL;
        const WMOAreaTableRec* rootRec = NULL;
        uint32_t groupID = 0;

        if (!World::QueryMapObjAreaTable(object, groupRec, rootRec, groupID, 0)) {
            return false;
        }

        area = groupRec ? g_areaTableDB.GetRecord(groupRec->m_areaTableID) : NULL;

        if (!area) {
            if (!rootRec) {
                return false;
            }

            area = g_areaTableDB.GetRecord(rootRec->m_areaTableID);

            if (!area) {
                return false;
            }
        }
    }

    AreaTableRec* parent = g_areaTableDB.GetRecord(area->m_parentAreaID);
    int32_t flags = area->m_flags;

    if (!(flags & 2) && parent) {
        flags = parent->m_flags;
    }

    return flags & 1;
}

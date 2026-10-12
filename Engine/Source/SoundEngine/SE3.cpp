#include <cfloat>
#include <cmath>
#include <cstdarg>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include "Engine/Source/SoundEngine/SE3.h"
#include "Engine/Source/Base/CDataAllocator.h"
#include "Engine/Source/BlizzardCore/SFile2-Core.h"
#include "Engine/Source/Event/Event.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/Os/Path.h"
#include "Engine/Source/Os/TimeManager.h"
#include "Engine/Source/Services/AsyncFile.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/Storm/Source/SLock.h"
#include "Engine/Source/Tempest/CRandom.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/SoundEmitterPillPointsRec.h"
#include "WoW/Source/DB/Rec/SoundEmittersRec.h"
#include "WoW/Source/DB/Rec/SoundEntriesAdvancedRec.h"
#include "WoW/Source/DB/Rec/SoundEntriesFallbacksRec.h"
#include "WoW/Source/DB/Rec/SoundEntriesRec.h"

// STATIC: 0x00CFFEB4/0x140BAF420
static int32_t s_millisecondsPerDay = 86400000;

// STATIC: 0x00CFFEB8/0x140BAF424
FMOD_OUTPUTTYPE SE3::sm_eGameSystemOutputType = FMOD_OUTPUTTYPE_NOSOUND;

// STATIC: 0x00CFFEBC/0x140BAF428
float SE3::sm_fFocusScalar = 1.0f;

// STATIC: 0x00CFFEC0
uint32_t SE3::sm_uNextUniqueID = 2;

// STATIC: 0x00CFFEC4/0x140BAF430
static uint32_t s_uNextEmitterHandle = 1;

// STATIC: 0x00CFFEC8/0x140BAF434
static int32_t s_bListenerChunkDirty = 1;

// STATIC: 0x00CFFECC/0x140BAF438
static int32_t s_bEmitterManagerShutDown = 1;

// STATIC: 0x00CFFF88/0x140BAF740
static float s_speakerMixFront = 0.6f;

// STATIC: 0x00CFFF8C/0x140BAF744
static float s_speakerMixCenter = 0.001f;

// STATIC: 0x00CFFF90/0x140BAF74C
static float s_speakerMixBack = 0.2f;

// STATIC: 0x00CFFF94/0x140BAF748
static float s_speakerMixSide = 0.001f;

// STATIC: 0x00E4DE30/0x140D37110
static uint32_t* s_soundChunkList;

// STATIC: 0x00E4DE34/0x140D37118
static int32_t sm_SoundKitDefs_ClosestAsyncObjectSortVal;

// STATIC: 0x00E4DE38/0x140D37120
static char s_gameOutputDeviceName[256];

// STATIC: 0x00E4DF38/0x140D37220
static CAsyncObject* sm_SoundKitDefs_ClosestAsyncObject;

// STATIC: 0x00E4DF3C
static HASHKEY_NONE s_lookupKey;

// STATIC: 0x00E4DF40/0x140D37228
int32_t SE3::sm_bShuttingDown;

// STATIC: 0x00E4DF44/0x140D37230
FMOD::Sound* SE3::sm_pCaptureSound;

// STATIC: 0x00E4DF48/0x140D37238
FMOD::System* SE3::sm_pGameSystem;

// STATIC: 0x00E4DF4C/0x140D37240
FMOD::System* SE3::sm_pChatSystem;

// STATIC: 0x00E4DF50/0x140D3722C
static int32_t s_dayProgression;

// STATIC: 0x00E4DF54/0x140D37248
static void (*sm_fpFilenameTranslateCallback)(const char**);

// STATIC: 0x00E4DF58/0x140D37250
static void* sm_fpStatTrackerUpdate;

// STATIC: 0x00E4DF5C/0x140D37258
static int32_t (*sm_fpGetDayProgression)();

// STATIC: 0x00E4DF64/0x140D37268
static void (*s_deviceChangeCallback)();

// STATIC: 0x00E4DF68/0x140D37264
static uint32_t s_chunkSize;

// STATIC: 0x00E4DF6C/0x140D37270
static uint32_t s_chunkRadius;

// STATIC: 0x00E4DF70/0x140D37274
static uint32_t s_listenerChunkX;

// STATIC: 0x00E4DF74/0x140D37278
static uint32_t s_listenerChunkY;

// STATIC: 0x00E4DF78
static void (*_fpEmitterManagerPosToSoundChunkCoordsCallback)(const C3Vector&, uint32_t*, uint32_t*);

// STATIC: 0x00E4DF7C/0x140D37288
static C3Vector s_listenerPosition;

// STATIC: 0x00E4DF90/0x140D372A0
TSGrowableArray<SE3DriverInfo> SE3::sm_aCachedGameSoundOutputDriverInfo;

// STATIC: 0x00E4DFA0/0x140D372B8
TSGrowableArray<SE3DriverInfo> SE3::sm_aCachedChatSoundInputDriverInfo;

// STATIC: 0x00E4DFB0/0x140D372D0
TSGrowableArray<SE3DriverInfo> SE3::sm_aCachedChatSoundOutputDriverInfo;

// STATIC: 0x00E4DFC0/0x140D372E8
static CDataAllocator s_CombinableSoundKitPositionPool(sizeof(SE3CombinableSoundKitPosition), 32);

// STATIC: 0x00E4DFD4/0x140D37308
SCritSect SE3::sm_DoneLoading_MessageQueue_CritSect;

// STATIC: 0x00CFFEE0/0x140BAF450
TSList<SE3DoneLoadingMessage, TSGetLink<SE3DoneLoadingMessage> > SE3::sm_DoneLoading_MessageQueue;

// STATIC: 0x00E4DFFC/0x140D37330
static SCritSect sm_MemorySoundEnded_MessageQueue_CritSect;

// STATIC: 0x00E4E024/0x140D37358
static SCritSect sm_SoundEnded_MessageQueue_CritSect;

// STATIC: 0x00CFFEEC/0x140BAF468
static TSList<SE3MemorySoundEndedMessage, TSGetLink<SE3MemorySoundEndedMessage> > sm_MemorySoundEnded_MessageQueue;

// STATIC: 0x00CFFEF8/0x140BAF480
static TSList<SE3SoundEndedMessage, TSGetLink<SE3SoundEndedMessage> > sm_SoundEnded_MessageQueue;

// STATIC: 0x00E4E04C/0x140D37380
static CDataAllocator s_MemorySoundEnded_MessagePool(sizeof(SE3MemorySoundEndedMessage), 32);

// STATIC: 0x00E4E060/0x140D373A0
static CDataAllocator s_SoundEnded_MessagePool(sizeof(SE3SoundEndedMessage), 32);

// STATIC: 0x00E4E074/0x140D373C0
static CDataAllocator s_DoneLoading_MessagePool(sizeof(SE3DoneLoadingMessage), 32);

// STATIC: 0x00E4E088/0x140BAF4A0
static TSHashTableReuse<SE3COMBINABLESOUNDLIST, HASHKEY_NONE> sm_CombinableSoundsTable;

// STATIC: 0x00E4E0C0/0x140BAF500
TSHashTable<SE3SOUNDKITOBJECT_LOOKUP, HASHKEY_NONE> SE3::sm_SoundKitObjectTable;

// STATIC: 0x00CFFF04/0x140BAF548
static TSExplicitList<SE3SoundKitObject, offsetof(SE3SoundKitObject, m_loadingLink)> sm_SoundKitObjects_Loading;

// STATIC: 0x00CFFF10/0x140BAF560
static TSExplicitList<SE3SoundKitObject, offsetof(SE3SoundKitObject, m_goGoGoLink)> sm_SoundKitObjects_GoGoGo;

// STATIC: 0x00CFFF1C/0x140BAF578
TSList<SE3SoundKitObject, TSGetLink<SE3SoundKitObject> > SE3::sm_SoundKitObjects;

// STATIC: 0x00CFFF28/0x140BAF590
static TSExplicitList<SE3SoundKitObject, offsetof(SE3SoundKitObject, m_deleteLink)> sm_SoundKitObjects_DeleteList;

// STATIC: 0x00CFFF34/0x140BAF5A8
static TSExplicitList<SE3SoundKitObject, offsetof(SE3SoundKitObject, m_fadeLink)> sm_SoundKitObjects_FadeList;

// STATIC: 0x00CFFF40/0x140BAF5C0
static TSExplicitList<SE3SoundKitObject, offsetof(SE3SoundKitObject, m_downloadLink)> sm_SoundKitObjects_WaitingForDownloadList;

// STATIC: 0x00E4E0EC/0x140D373E0
static TSGrowableArray<SE3SoundKitDef*> sm_SoundKitDefArray;

// STATIC: 0x00E4E0FC/0x140BAF5E0
static TSHashTable<SOUNDKITLOOKUP, HASHKEY_CONSTSTRI> s_soundKitLookup;

// STATIC: 0x00E4E128
static TSHashTable<SE3SoundChunk, HASHKEY_NONE> s_soundChunkTable;

// STATIC: 0x00E4E154
static TSHashTable<SE3SoundEmitterLookup, HASHKEY_NONE> s_soundEmitterTable;

// STATIC: 0x00E4E180/0x140D373F8
static SCritSect s_emitterManagerCritSect;

// STATIC: 0x00E4E1A8/0x140D37420
static SCritSect s_createEmitterMessagesCritSect;

// STATIC: 0x00E4E1D0/0x140D37448
static SCritSect s_set3DPositionMessagesCritSect;

// STATIC: 0x00E4E1F8/0x140D37470
static SCritSect s_destroyEmitterMessagesCritSect;

// STATIC: 0x00E4E220/0x140D37498
static SCritSect s_markSoundChunkDirtyMessagesCritSect;

// STATIC: 0x00CFFF4C/0x140BAF6C8
static TSList<SE3CreateSoundEmitterMessage, TSGetLink<SE3CreateSoundEmitterMessage> > s_createEmitterMessages;

// STATIC: 0x00CFFF58/0x140BAF6E0
static TSList<SE3Set3DPositionMessage, TSGetLink<SE3Set3DPositionMessage> > s_set3DPositionMessages;

// STATIC: 0x00CFFF64/0x140BAF6F8
static TSList<SE3DestroySoundEmitterMessage, TSGetLink<SE3DestroySoundEmitterMessage> > s_destroyEmitterMessages;

// STATIC: 0x00CFFF70/0x140BAF710
static TSList<SE3MarkSoundChunkDirtyMessage, TSGetLink<SE3MarkSoundChunkDirtyMessage> > s_markSoundChunkDirtyMessages;

// STATIC: 0x00E4E248/0x140D374C0
static CDataAllocator s_createEmitterMessagePool(sizeof(SE3CreateSoundEmitterMessage), 32);

// STATIC: 0x00E4E25C/0x140D374E0
static CDataAllocator s_set3DPositionMessagePool(sizeof(SE3Set3DPositionMessage), 32);

// STATIC: 0x00E4E270/0x140D37500
static CDataAllocator s_destroyEmitterMessagePool(sizeof(SE3DestroySoundEmitterMessage), 32);

// STATIC: 0x00E4E284/0x140D37520
static CDataAllocator s_markSoundChunkDirtyMessagePool(sizeof(SE3MarkSoundChunkDirtyMessage), 32);

// STATIC: 0x00CFFF7C/0x140BAF728
static TSExplicitList<SE3SoundChunk, offsetof(SE3SoundChunk, m_dirtyLink)> s_dirtySoundChunks;

// STATIC: 0x00E4E2A0/0x140D37548
static float s_speakerMixLFE;

// STATIC: 0x00E4E2B8
SE3SoundBus SE3::sm_aSoundBus[SE3BUS_LAST];

// OFFSET: 0x00705520/0x1403D25E0
static void* F_CALLBACK FSoundAllocCallback(unsigned int size, FMOD_MEMORY_TYPE type) {
    return SMemAlloc(size, "FMod FSoundAllocCallback", 0, 0);
}

// OFFSET: 0x00705540/0x1403D2600
static void* F_CALLBACK FSoundReallocCallback(void* ptr, unsigned int size, FMOD_MEMORY_TYPE type) {
    return SMemReAlloc(ptr, size, "FMod FSoundReallocCallback", 0, 0);
}

// OFFSET: 0x00705560/0x1403D2620
static void F_CALLBACK FSoundFreeCallback(void* ptr, FMOD_MEMORY_TYPE type) {
    SMemFree(ptr, "FMod FSoundFreeCallback", 0, 0);
}

// OFFSET: 0x00705580/0x1403D2640
static float F_CALLBACK RolloffCallback(FMOD_CHANNEL* channel, float distance) {
    float minDistance;
    float maxDistance;

    reinterpret_cast<FMOD::Channel*>(channel)->get3DMinMaxDistance(&minDistance, &maxDistance);

    if (distance >= maxDistance) {
        return 0.0f;
    }

    if (distance <= minDistance) {
        return 1.0f;
    }

    float rolloff = minDistance / ((distance - minDistance) * 4.0f + minDistance);

    if (distance > maxDistance * 0.9f) {
        rolloff = (1.0f - (distance - maxDistance * 0.9f) / (maxDistance - maxDistance * 0.9f)) * rolloff;
    }

    return rolloff;
}

// OFFSET: 0x00705630/0x1403D26E0
void SE3::CleanUpFMODGameSystem() {
    FMOD::System* system = SE3::sm_pGameSystem;

    if (system) {
        SE3::sm_pGameSystem = NULL;
        system->close();
        system->release();
    }
}

// OFFSET: 0x00705660/0x1403D2720
void SE3::SetListenerAttributes(const C3Vector& position, const C3Vector& forward, const C3Vector& up, const C3Vector* velocity) {
    if (SE3::sm_pGameSystem) {
        SE3::sm_pGameSystem->set3DListenerAttributes(0, reinterpret_cast<const FMOD_VECTOR*>(&position), reinterpret_cast<const FMOD_VECTOR*>(velocity), reinterpret_cast<const FMOD_VECTOR*>(&forward), reinterpret_cast<const FMOD_VECTOR*>(&up));
    }
}

// OFFSET: 0x00705690/0x1403D2750
FMOD_RESULT SE3::GetListenerPosition(C3Vector* position) {
    return SE3::sm_pGameSystem->get3DListenerAttributes(0, reinterpret_cast<FMOD_VECTOR*>(position), NULL, NULL, NULL);
}

// OFFSET: 0x007056B0/0x1403D2780
void SE3SoundKitDef::BuildPath(char* path, int32_t size, int32_t index) {
    const char* filename = this->m_pSoundKitRec->m_file[index];

    if (*this->m_pSoundKitRec->m_directoryBase) {
        SStrPrintf(path, size, "%s\\%s", static_cast<const char*>(this->m_pSoundKitRec->m_directoryBase), filename);
    } else {
        SStrCopy(path, filename, size);
    }
}

// OFFSET: 0x00705730/0x1403D2830
static int32_t MyFileIsLocal(char* filename) {
    if (OsPathCheckExtension(filename, "wav")) {
        if (SFile::FileHasHigherPriority(filename, "ogg", 260)) {
            return 1;
        }

        OsPathReplaceExtension(filename, 260, filename, "ogg");
    }

    int32_t isLocal = SFile::FileIsLocal(filename);
    return isLocal && isLocal != 2;
}

// OFFSET: 0x00705790/0x1403D28A0
int32_t SE3SoundKitDef::IsLocal(int32_t index) {
    if (!SFile::IsStreamingMode()) {
        return 1;
    }

    char path[260];
    this->BuildPath(path, sizeof(path), index);
    return MyFileIsLocal(path);
}

// OFFSET: 0x007057E0/0x1403D29E0
static void SoundKitDefDownloadCancelled(CAsyncObject* object) {
    SE3SoundKitDef* def = static_cast<SE3SoundKitDef*>(object->m_userArg);
    def->m_pAsyncObj = NULL;
    def->m_downloaded = 1;
    AsyncFileReadDestroyObject(object);
}

// OFFSET: 0x00705810
static void SoundKitDefDownloadFailed(CAsyncObject* object) {
    SE3SoundKitDef* def = static_cast<SE3SoundKitDef*>(object->m_userArg);
    def->m_pAsyncObj = NULL;
    def->m_downloaded = 0;
    AsyncFileReadDestroyObject(object);
}

// OFFSET: 0x00705840/0x1403D28A0
int32_t SE3SoundKitDef::SelectLocalFile(int32_t* index) {
    int32_t i = 0;

    if (this->m_nNumFiles <= 0) {
        return 0;
    }

    while (SFile::IsStreamingMode()) {
        char path[260];
        this->BuildPath(path, sizeof(path), i);

        if (OsPathCheckExtension(path, "wav")) {
            if (SFile::FileHasHigherPriority(path, "ogg", 260)) {
                break;
            }

            OsPathReplaceExtension(path, 260, path, "ogg");
        }

        int32_t isLocal = SFile::FileIsLocal(path);

        if (isLocal && isLocal != 2) {
            break;
        }

        if (++i >= this->m_nNumFiles) {
            return 0;
        }
    }

    if (index) {
        *index = i;
    }

    return 1;
}

// OFFSET: 0x00705920
static int32_t GetDownloadPriority(SE3BUS bus) {
    if (bus <= SE3BUS_SFX) {
        return 5;
    }

    if (bus > SE3BUS_AMBIENCE) {
        return 5;
    }

    return 6;
}

// OFFSET: 0x00705940/0x1403D2C40
void SE3::SetAttachCustomDSPChainCallback(SE3SoundKitObject* sko, void (*callback)(void*, char*), const char* name) {
    if (sko && callback && name) {
        sko->m_fpCustomDSPChainCallback = callback;
        sko->m_sCustomDSPChainName = static_cast<char*>(SMemAlloc(128, __FILE__, __LINE__, SMEM_FLAG_ZEROMEMORY));
        SStrCopy(sko->m_sCustomDSPChainName, name, 128);
    }
}

// OFFSET: 0x007059B0/0x1403D2CD0
void SE3::SetReverbPropertiesAndSpeakerMix(SE3SoundKitObject* sko, FMOD::Channel* channel) {
    if (!sko || !channel) {
        return;
    }

    FMOD_MODE mode;

    if (channel->getMode(&mode) != FMOD_OK) {
        return;
    }

    FMOD_REVERB_CHANNELPROPERTIES properties = { 0, 0, 0, 0, 0, 0.0f, 0, 0.25f, 1.5f, 1.0f, 0, 1.0f, 0, 0.0f, 0.0f, 0.0f, 1.0f, FMOD_REVERB_CHANNELFLAGS_DEFAULT };
    channel->setReverbProperties(&properties);

    if (mode & FMOD_2D) {
        if (!(mode & FMOD_SOFTWARE) || channel->setSpeakerMix(s_speakerMixFront, s_speakerMixFront, s_speakerMixCenter, s_speakerMixLFE, s_speakerMixBack, s_speakerMixBack, s_speakerMixSide, s_speakerMixSide) == FMOD_OK) {
            if (!sko->m_reverb) {
                FMOD_REVERB_CHANNELPROPERTIES current;
                channel->getReverbProperties(&current);
                current.Room = -10000;
                channel->setReverbProperties(&current);
            }
        }
    } else if (mode & FMOD_SOFTWARE) {
        channel->setSpeakerMix(s_speakerMixFront, s_speakerMixFront, s_speakerMixCenter, s_speakerMixLFE, s_speakerMixBack, s_speakerMixBack, s_speakerMixSide, s_speakerMixSide);
    }
}

// OFFSET: 0x00706D90/0x1403D2ED0
void SE3SoundKitObject::Set3DPosition(const C3Vector& position) {
    this->m_position = position;

    C3Vector offsetPosition;
    offsetPosition.x = this->m_positionOffset.x + position.x;
    offsetPosition.y = this->m_positionOffset.y + position.y;
    offsetPosition.z = this->m_positionOffset.z + position.z;

    for (int32_t i = 0; i < 2; i++) {
        if (this->m_channels[i].m_pFMODChannel) {
            this->m_channels[i].m_pFMODChannel->set3DAttributes(reinterpret_cast<const FMOD_VECTOR*>(&offsetPosition), NULL);
        }
    }
}

// OFFSET: 0x00705B40/0x140835F40
int32_t SE3SoundKitObject::IsPlaying() {
    bool playing = false;

    for (int32_t i = 0; i < 2; i++) {
        if (this->m_channels[i].m_pFMODChannel) {
            this->m_channels[i].m_pFMODChannel->isPlaying(&playing);

            if (playing) {
                return 1;
            }
        }
    }

    return 0;
}

// OFFSET: 0x00705B90/0x1403D2F60
int32_t SE3SoundKitObject::IsVirtual() {
    int32_t i;

    for (i = 0; !this->m_channels[i].m_pFMODChannel; i++) {
        if (i + 1 >= 2) {
            return 1;
        }
    }

    bool isVirtual;
    this->m_channels[i].m_pFMODChannel->isVirtual(&isVirtual);

    float audibility = 1.0f;
    this->m_channels[i].m_pFMODChannel->getAudibility(&audibility);

    if (audibility == 0.0f) {
        return 1;
    }

    return isVirtual;
}

// OFFSET: 0x00705C10/0x1403D2FF0
uint32_t SE3::GetModeFlags(const C3Vector* position, const SE3SoundKitDef* def, SE3SoundKitProperties* properties) {
    int32_t is3D = position != NULL;
    uint32_t channelMode = 0;

    if (properties->m_software || !is3D) {
        channelMode = FMOD_SOFTWARE;
    } else if (position) {
        channelMode = FMOD_HARDWARE;
    }

    uint32_t mode = (8 * (is3D != 0) + 8) | (FMOD_NONBLOCKING | FMOD_LOWMEM | FMOD_IGNORETAGS) | channelMode;

    switch (properties->m_usage) {
        case SE3USAGE_LOOPING:
            if (def && (def->m_pSoundKitRec->m_flags & 0x200)) {
                mode |= FMOD_LOOP_NORMAL;
            } else {
                mode |= FMOD_LOOP_OFF;
            }
            break;

        case SE3USAGE_PERIODIC:
            mode |= FMOD_LOOP_NORMAL;
            break;

        default:
            mode |= FMOD_LOOP_OFF;
            break;
    }

    if (def && def->m_pAdvancedSoundKitRec) {
        mode |= FMOD_SOFTWARE;
    }

    return mode;
}

// OFFSET: 0x00705CA0/0x1403D32F0
int32_t SE3::GetNumOutputDrivers_Cached(FMOD::System* system) {
    int32_t numDrivers = 0;

    if (system) {
        FMOD_OUTPUTTYPE output;
        system->getOutput(&output);
        system->getNumDrivers(&numDrivers);

        if (numDrivers) {
            return numDrivers + 1;
        }
    }

    return numDrivers;
}

// OFFSET: 0x00705CE0/0x1403D30D0
void SE3::InitializeDefaultSystemName() {
    if (!SE3::sm_sSystemDefaultName[0]) {
        SStrCopy(SE3::sm_sSystemDefaultName, FrameScript_GetText("SYSTEM_DEFAULT", -1, GENDER_NOT_APPLICABLE), sizeof(SE3::sm_sSystemDefaultName));
    }
}

// OFFSET: 0x00705D30/0x1403D3130
int32_t SE3::GetOutputDriverName(int32_t index, char* name, int32_t nameLength, FMOD::System* system) {
    if (index < 0) {
        index = 0;
    }

    if (!system) {
        return 0;
    }

    if (index) {
        system->getDriverName(index - 1, name, nameLength);
    } else {
        SE3::InitializeDefaultSystemName();
        SStrCopy(name, SE3::sm_sSystemDefaultName, nameLength);

        if (!strlen(name)) {
            SStrCopy(name, "System Default", nameLength);
        }
    }

    return 1;
}

// OFFSET: 0x00705E00/0x1403D3240
int32_t SE3::GetInputDriverName(int32_t index, char* name, int32_t nameLength, FMOD::System* system) {
    if (index < 0) {
        index = 0;
    }

    if (!system) {
        return 0;
    }

    if (index) {
        system->getRecordDriverName(index - 1, name, nameLength);
    } else {
        SE3::InitializeDefaultSystemName();
        SStrCopy(name, SE3::sm_sSystemDefaultName, nameLength);
    }

    return 1;
}

// OFFSET: 0x00705E80/0x1403D3340
int32_t SE3::GetOutputDriverName_Cached(int32_t index, char* name, int32_t nameLength, FMOD::System* system) {
    return SE3::GetOutputDriverName(index, name, nameLength, system);
}

// OFFSET: 0x00705E90/0x1403D3080
int32_t SE3::GetNumInputDrivers(FMOD::System* system) {
    int32_t numDrivers = 0;

    if (!system) {
        return 0;
    }

    FMOD_OUTPUTTYPE output;
    system->getOutput(&output);
    system->getRecordNumDrivers(&numDrivers);

    return numDrivers + 1;
}

// OFFSET: 0x00705EC0/0x1403D3350
int32_t SE3::GetInputDriverName_Cached(int32_t index, char* name, int32_t nameLength, FMOD::System* system) {
    return SE3::GetInputDriverName(index, name, nameLength, system);
}

// OFFSET: 0x00705ED0/0x1403D3360
int32_t SE3::IsChatSoundInitialized() {
    return SE3::sm_bCaptureInitialized;
}

// OFFSET: 0x00705EE0/0x1403D3370
float SE3::SoundBusGetSuppressedVolumeScalar(SE3BUS bus, uint32_t uid) {
    float scalar = 1.0f;

    if (SE3::SuppressorIsPresent()) {
        if (SE3::IsBusEqualOrChild(bus, SE3BUS_DIALOG)) {
            scalar = SE3::GetSuppressionScalar(SE3BUS_DIALOG, uid);
        } else if (SE3::IsBusEqualOrChild(bus, SE3BUS_SFX)) {
            scalar = SE3::GetSuppressionScalar(SE3BUS_SFX, uid);
        } else if (SE3::IsBusEqualOrChild(bus, SE3BUS_MUSIC)) {
            scalar = SE3::GetSuppressionScalar(SE3BUS_MUSIC, uid);
        } else if (SE3::IsBusEqualOrChild(bus, SE3BUS_AMBIENCE)) {
            scalar = SE3::GetSuppressionScalar(SE3BUS_AMBIENCE, uid);
        }
    }

    return SE3::SoundBusGetVolume(bus) * scalar;
}

// OFFSET: 0x00705F80/0x1403D3410
float SE3ChatSound::GetScaledVolume() {
    return SE3::SoundBusGetSuppressedVolumeScalar(this->m_bus, this->m_uUID) * this->m_volume;
}

// OFFSET: 0x00705FA0/0x1403D3430
char* SE3::GetCurrentGameOutputDeviceName() {
    return s_gameOutputDeviceName;
}

// OFFSET: 0x00705FB0/0x1403D3440
uint32_t SE3::EmitterManagerGetSoundChunkHandle(const C3Vector& position) {
    uint32_t x = 0;
    uint32_t y = 0;
    _fpEmitterManagerPosToSoundChunkCoordsCallback(position, &x, &y);

    return y | (x << 16);
}

// OFFSET: 0x00705FE0/0x1403D3470
static int ComparePillPoints(const void* a, const void* b) {
    return static_cast<const SoundEmitterPillPointsRec*>(a)->m_soundEmittersID - static_cast<const SoundEmitterPillPointsRec*>(b)->m_soundEmittersID;
}

// OFFSET: 0x00706000/0x1403D3480
void SE3::EmitterManagerBuildSoundChunkList(uint32_t x, uint32_t y) {
    uint32_t radius = s_chunkRadius;
    uint32_t diameter = 2 * s_chunkRadius + 1;

    if (diameter * diameter != 0xFFFFFFFF) {
        for (uint32_t i = 0; i < diameter * diameter + 1; i++) {
            s_soundChunkList[i] = 0x7D007D00;
        }
    }

    uint32_t maxCoord = 1024 / s_chunkSize;
    uint32_t maxX = radius + x;
    uint32_t minX = x - radius;

    if (maxCoord < maxX) {
        maxX = maxCoord;
    }

    uint32_t minY = y - radius;
    uint32_t maxY = y + radius;

    if (maxCoord < maxY) {
        maxY = maxCoord;
    }

    int32_t count = 0;

    if (minX > maxX) {
        s_soundChunkList[0] = 0x75307530;
        return;
    }

    for (uint32_t chunkX = minX; chunkX <= maxX; chunkX++) {
        for (uint32_t chunkY = minY; chunkY <= maxY; chunkY++) {
            s_soundChunkList[count++] = chunkY | (chunkX << 16);
        }
    }

    s_soundChunkList[count] = 0x75307530;
}

// OFFSET: 0x007060C0/0x1403D3550
static void StreamingCallback(void* param) {
    AsyncFileReadLinkToPriority(static_cast<CAsyncObject*>(param), 1);
}

// OFFSET: 0x007060E0/0x1403D3560
void SE3::StreamingUpdate() {
    if (sm_SoundKitDefs_ClosestAsyncObject) {
        AsyncFileReadAddStreamingObject(StreamingCallback, sm_SoundKitDefs_ClosestAsyncObject, sm_SoundKitDefs_ClosestAsyncObjectSortVal);
    }
}

// OFFSET: 0x00706250
static int32_t DriverNameMatches(FMOD::System* system, int32_t index, const char* name) {
    char driverName[512];
    driverName[0] = '\0';
    SE3::GetOutputDriverName(index, driverName, sizeof(driverName), system);

    return _stricmp(driverName, name) == 0;
}

// OFFSET: 0x00706290/0x1403D3770
static int32_t FindDriverIndex(FMOD::System* system, const char* name, int32_t index) {
    int32_t numDrivers = 0;
    int32_t count = 0;

    if (system) {
        FMOD_OUTPUTTYPE output;
        system->getOutput(&output);
        system->getNumDrivers(&count);
        numDrivers = count;

        if (count) {
            numDrivers = count + 1;
        }
    }

    if (index >= 0 && index < numDrivers && DriverNameMatches(system, index, name)) {
        return index;
    }

    for (int32_t i = 0; i < numDrivers; i++) {
        if (i == index) {
            continue;
        }

        char driverName[512];
        driverName[0] = '\0';
        SE3::GetOutputDriverName(i, driverName, sizeof(driverName), system);

        if (!_stricmp(driverName, name)) {
            return i;
        }
    }

    return 0;
}

// OFFSET: 0x00706340/0x1403D3860
FMOD_RESULT SE3::SetDrivers(SE3EngineInitParams* params, FMOD::System* system) {
    int32_t index = FindDriverIndex(system, params->m_outputDriverName, params->m_outputDriverIndex);
    int32_t driver = index ? index - 1 : 0;

    FMOD_RESULT result = SE3::sm_pGameSystem->getDriverName(driver, s_gameOutputDeviceName, sizeof(s_gameOutputDeviceName));

    if (result != FMOD_OK) {
        SE3::Log_Write(__LINE__, __FILE__, result, "Failed to get driver name for driver index %d", driver);

        if (driver) {
            SE3::Log_Write(__LINE__, __FILE__, result, "Falling back to default sound driver");
            driver = 0;
            result = SE3::sm_pGameSystem->getDriverName(0, s_gameOutputDeviceName, sizeof(s_gameOutputDeviceName));
        }

        if (result != FMOD_OK) {
            return result;
        }
    }

    if (index) {
        SE3::Log_Write(__LINE__, __FILE__, FMOD_OK, "** Using User Specified Driver [%d] %s", index, s_gameOutputDeviceName);
    } else {
        SE3::Log_Write(__LINE__, __FILE__, FMOD_OK, "** Using Driver [%d] System Default (%s)", 0, s_gameOutputDeviceName);
    }

    result = system->setDriver(driver);

    if (result != FMOD_OK && driver) {
        SE3::Log_Write(__LINE__, __FILE__, result, "Error setting output driver. Falling back to default sound driver");
        return SE3::sm_pGameSystem->setDriver(0);
    }

    return result;
}

// OFFSET: 0x00706460/0x1403D39B0
int32_t SE3::GetFallbackSoundKitID(int32_t soundKitID) {
    int32_t high = g_soundEntriesFallbacksDB.m_numRecords;
    SoundEntriesFallbacksRec* records = g_soundEntriesFallbacksDB.GetRecordByIndex(0);
    int32_t low = 0;

    if (high <= 0) {
        return 0;
    }

    do {
        int32_t mid = (high + low) / 2;

        if (records[mid].m_soundEntriesID >= soundKitID) {
            high = mid;
        } else {
            low = mid + 1;
        }
    } while (low < high);

    if (low < g_soundEntriesFallbacksDB.m_numRecords && records[low].m_soundEntriesID == soundKitID) {
        return records[low].m_fallbackSoundEntriesID;
    }

    return 0;
}

// OFFSET: 0x007064D0/0x1403D3A30
void SE3::InitGameSystemForSilence() {
    SE3::sm_eGameSystemOutputType = FMOD_OUTPUTTYPE_NOSOUND;
    FMOD::System_Create(&SE3::sm_pGameSystem);
    SE3::sm_pGameSystem->setOutput(FMOD_OUTPUTTYPE_NOSOUND);
    SE3::Log_Write(__LINE__, __FILE__, FMOD_OK, " - ##############################");
    SE3::Log_Write(__LINE__, __FILE__, FMOD_OK, " - ### Sound System: SILENT MODE");
    SE3::Log_Write(__LINE__, __FILE__, FMOD_OK, " - ##############################");
}

// OFFSET: 0x00706540/0x1403D3AB0
void SE3SoundKitProperties::ResetToDefaults() {
    this->m_nIndexOverride = -1;
    this->unk_0004 = -1.0f;
    this->unk_002C = -1.0f;
    this->m_conditionArg = NULL;
    this->m_fpCondition = NULL;
    this->m_bus = SE3BUS_SFX;
    this->m_fadeInTime = 0;
    this->m_fadeOutTime = 0;
    this->m_usage = SE3USAGE_LOOPING;
    this->m_uniqueness = 0;
    this->unk_0028 = 0;
    this->m_muteFadeSpeed = 0.5f;
    this->m_bit0 = 0;
    this->m_reverb = 1;
    this->m_noRandomStart = 0;
    this->m_bit3 = 0;
    this->m_software = 0;
    this->m_volume = 1.0f;
    this->m_selectMode = 2;
    this->m_maxAudibleBehavior = 2;
    this->m_coneOrientation.x = 0.0f;
    this->m_coneOrientation.y = 0.0f;
    this->m_coneOrientation.z = 0.0f;
    this->m_pSKOToReuse = NULL;
    this->m_sFilename = NULL;
    this->m_fpCustomDSPChainCallback = NULL;
    this->m_stopPrevious = 1;
    this->m_attachCustomDSPChain = 0;
    this->m_noStreaming = 0;
    this->m_checkRange = 1;
    this->m_combinable = 0;
    this->m_bit10 = 0;
    this->m_highPriorityDownload = 0;
    this->m_priority = 128;
}

// OFFSET: 0x00706600/0x1403D3B60
static int32_t RandomBetween(int32_t min, int32_t max) {
    if (max > min) {
        return min + CRandom::dice_(max - min, g_rndSeed);
    }

    return max;
}

// OFFSET: 0x00706630/0x1403D3BB0
void SE3SoundKitDef::SelectNextFile(const SE3SoundKitProperties* properties, int32_t* index) {
    *index = -1;
    uint32_t selectMode = properties->m_selectMode;

    if (selectMode > 2) {
        SE3::SoundKitDefLog("The kit %s has invalidate play style:%d!", static_cast<const char*>(this->m_pSoundKitRec->m_name), selectMode);
        return;
    }

    int32_t numFiles = this->m_nNumFiles;

    if (numFiles <= 0) {
        SE3::SoundKitDefLog("The kit %s has no sound variant files to select from!", static_cast<const char*>(this->m_pSoundKitRec->m_name));
        return;
    }

    if (properties->m_nIndexOverride > -1) {
        int32_t selected = properties->m_nIndexOverride;

        if (selected >= numFiles - 1) {
            selected = numFiles - 1;
        }

        *index = selected;
        this->m_lastIndex = selected;
        return;
    }

    if (numFiles == 1) {
        *index = 0;
        this->m_lastIndex = 0;
        return;
    }

    uint32_t total;

    for (int32_t pass = 0; ; pass++) {
        total = 0;

        if (pass == 2) {
            this->m_lastIndex = -1;
        } else if (pass >= 3) {
            SE3::SoundKitDefLog("The kit %s has multiple files of zero weight!", static_cast<const char*>(this->m_pSoundKitRec->m_name));
            return;
        }

        for (int32_t i = 0; i < this->m_nNumFiles; i++) {
            if (i != this->m_lastIndex) {
                total += this->m_weights[i];
            }
        }

        if (total) {
            break;
        }

        switch (selectMode) {
            case 0:
                memset(this->m_weights, 1, sizeof(this->m_weights));
                break;

            case 1:
            case 2:
                for (int32_t i = 0; i < this->m_nNumFiles; i++) {
                    this->m_weights[i] = static_cast<uint8_t>(this->m_pSoundKitRec->m_freq[i]);
                }
                break;
        }
    }

    int32_t random = 0;

    if (selectMode) {
        random = CRandom::dice_(total, g_rndSeed);
    }

    int32_t i;

    for (i = 0; i < this->m_nNumFiles; i++) {
        if (i != this->m_lastIndex) {
            random -= this->m_weights[i];
        }

        if (random < 0) {
            this->m_weights[i] = 0;
            *index = i;
            this->m_lastIndex = i;

            if (selectMode == 1) {
                memset(this->m_weights, 0, sizeof(this->m_weights));
            }

            return;
        }
    }

    SE3::SoundKitDefLog("The kit %s fail to select a variant!", static_cast<const char*>(this->m_pSoundKitRec->m_name));
}

// OFFSET: 0x00706850/0x1403D3EF0
SE3SoundKitDef* SE3::GetSoundKitDef(int32_t soundKitID) {
    if (soundKitID >= static_cast<int32_t>(sm_SoundKitDefArray.Count()) || soundKitID <= 0) {
        return NULL;
    }

    return sm_SoundKitDefArray[soundKitID];
}

// OFFSET: 0x00706880/0x1403D3F10
int32_t SE3::GetNumSoundsInSoundKit(int32_t soundKitID) {
    SE3SoundKitDef* def;

    if (soundKitID < static_cast<int32_t>(sm_SoundKitDefArray.Count()) && soundKitID > 0 && (def = sm_SoundKitDefArray[soundKitID]) != NULL) {
        return def->m_nNumFiles;
    }

    return 0;
}

// OFFSET: 0x007068B0/0x1403D3F40
int32_t SE3::IsLooping_ByDef(int32_t soundKitID) {
    SE3SoundKitDef* def;

    if (soundKitID < static_cast<int32_t>(sm_SoundKitDefArray.Count()) && soundKitID > 0 && (def = sm_SoundKitDefArray[soundKitID]) != NULL && def->m_pSoundKitRec) {
        return def->m_pSoundKitRec->m_flags & 0x200;
    }

    return 0;
}

// OFFSET: 0x00706940/0x1403D3FD0
int32_t SE3::PreloadSoundKit(int32_t soundKitID, int32_t wait) {
    SE3SoundKitDef* def = SE3::GetSoundKitDef(soundKitID);

    if (!def) {
        return SE3_ERR_INVALID_KIT;
    }

    for (int32_t i = 0; i < def->m_nNumFiles; i++) {
        char path[260];
        def->BuildPath(path, sizeof(path), i);

        if (OsPathCheckExtension(path, "wav")) {
            OsPathReplaceExtension(path, sizeof(path), path, "ogg");
        }

        CAsyncObject* object = NULL;

        if (!AsyncFileReadCreateObject(path, 5, 1, 0, &object, NULL, NULL, DefaultAsyncObjectCleanupCallback, DefaultAsyncObjectCleanupCallback, 0, 0, 0)) {
            return SE3_ERR_FILE;
        }

        if (object && wait) {
            AsyncFileReadWait(object);
        }
    }

    return SE3_OK;
}

// OFFSET: 0x007068F0/0x1403D3F80
int32_t SE3::IsLoopingOrPeriodic_ByDef(int32_t soundKitID) {
    if (soundKitID >= static_cast<int32_t>(sm_SoundKitDefArray.Count())) {
        return 0;
    }

    if (soundKitID <= 0) {
        return 0;
    }

    SE3SoundKitDef* def = sm_SoundKitDefArray[soundKitID];

    if (!def) {
        return 0;
    }

    if (!def->m_pSoundKitRec) {
        return 0;
    }

    if (def->m_pAdvancedSoundKitRec) {
        return static_cast<uint32_t>(def->m_pAdvancedSoundKitRec->m_usage) <= SE3USAGE_PERIODIC;
    }

    return def->m_pSoundKitRec->m_flags & 0x200;
}

// OFFSET: 0x00706A40/0x1403D2A20
static int32_t IsBeyondListenerRange(const C3Vector* position, float range, float* distanceSquared) {
    C3Vector listener;
    SE3::sm_pGameSystem->get3DListenerAttributes(0, reinterpret_cast<FMOD_VECTOR*>(&listener), NULL, NULL, NULL);

    float distSq = (listener.x - position->x) * (listener.x - position->x) + (listener.y - position->y) * (listener.y - position->y) + (listener.z - position->z) * (listener.z - position->z);
    *distanceSquared = distSq;

    return distSq > range * range;
}

// OFFSET: 0x00706C30/0x1403D41C0
static void PeriodicSoundFinishedCallback(SE3SoundKitObject* sko) {
    if (sko) {
        const SoundEntriesAdvancedRec* advancedRec = sko->m_pSoundKitDef->m_pAdvancedSoundKitRec;

        if (advancedRec) {
            sko->m_periodicTimer = RandomBetween(advancedRec->m_timeIntervalMin, advancedRec->m_timeIntervalMax);
        }
    }
}

// OFFSET: 0x00706C90
float SE3SoundKitObject::GetChannelVolume(int32_t index) {
    float busVolume = SE3::SoundBusGetSuppressedVolumeScalar(this->m_properties.m_bus, this->m_uUID);
    float recVolume = 1.0f;

    if (this->m_pSoundKitDef && this->m_pSoundKitDef->m_pSoundKitRec) {
        recVolume = this->m_pSoundKitDef->m_pSoundKitRec->m_volumeFloat;
    }

    return this->m_channels[index].m_fadeVolume * this->m_fTimeOfDayScalar * this->m_properties.m_volume * this->m_randomVolume * this->m_fadeVolume * recVolume * busVolume;
}

// OFFSET: 0x00706D00/0x1403D4230
void SE3SoundKitObject::UpdateVolume() {
    for (int32_t i = 0; i < 2; i++) {
        SE3SoundChannel* channel = &this->m_channels[i];

        if (channel->m_pFMODChannel) {
            float busVolume = SE3::SoundBusGetSuppressedVolumeScalar(this->m_properties.m_bus, this->m_uUID);
            float recVolume = 1.0f;

            if (this->m_pSoundKitDef && this->m_pSoundKitDef->m_pSoundKitRec) {
                recVolume = this->m_pSoundKitDef->m_pSoundKitRec->m_volumeFloat;
            }

            channel->m_pFMODChannel->setVolume(channel->m_fadeVolume * this->m_fTimeOfDayScalar * this->m_properties.m_volume * this->m_randomVolume * this->m_fadeVolume * recVolume * busVolume);
        }
    }
}

// OFFSET: 0x00706E10/0x1403D42E0
void SE3::SetNewSoundKitObjectProperties(SE3SoundKitObject* sko, const C3Vector* position, SE3SoundKitProperties* properties, uint32_t modeFlags) {
    sko->m_properties.m_fadeInTime = properties->m_fadeInTime;
    sko->m_properties.m_fadeOutTime = properties->m_fadeOutTime;

    if (properties->m_fadeInTime > 0) {
        sko->m_fadeVolume = 0.0f;
    }

    if (position) {
        sko->m_position = *position;
    } else {
        sko->m_position.x = 0.0f;
        sko->m_position.y = 0.0f;
        sko->m_position.z = 0.0f;
    }

    sko->m_properties.m_coneOrientation = properties->m_coneOrientation;
    sko->m_modeFlags = modeFlags;

    if (properties->m_usage) {
        sko->m_usage = properties->m_usage != SE3USAGE_PERIODIC ? SE3USAGE_ONESHOT : SE3USAGE_LOOPING;
    } else if (sko->m_pSoundKitDef->m_pAdvancedSoundKitRec) {
        sko->m_usage = static_cast<SE3USAGE>(sko->m_pSoundKitDef->m_pAdvancedSoundKitRec->m_usage);
    } else {
        sko->m_usage = (sko->m_pSoundKitDef->m_pSoundKitRec->m_flags & 0x200) ? SE3USAGE_LOOPING : SE3USAGE_ONESHOT;
    }

    if (properties->m_attachCustomDSPChain) {
        SE3::SetAttachCustomDSPChainCallback(sko, properties->m_fpCustomDSPChainCallback, properties->m_sCustomDSPChainName);
    }

    sko->m_reverb = properties->m_reverb;

    if (!sko->m_usage) {
        SE3SoundKitDef* def = sko->m_pSoundKitDef;

        if (def && def->m_nNumFiles > 1) {
            const SoundEntriesAdvancedRec* advancedRec = def->m_pAdvancedSoundKitRec;

            if (advancedRec && advancedRec->m_timeIntervalMax > 0) {
                sko->m_periodicTimer = RandomBetween(advancedRec->m_timeIntervalMin, advancedRec->m_timeIntervalMax);
            }
        }
    }
}

// OFFSET: 0x00706F50/0x1403D4440
int32_t SE3::GetSoundKitUsage(int32_t soundKitID) {
    if (soundKitID >= static_cast<int32_t>(sm_SoundKitDefArray.Count())) {
        return SE3USAGE_ONESHOT;
    }

    if (soundKitID <= 0) {
        return SE3USAGE_ONESHOT;
    }

    SE3SoundKitDef* def = sm_SoundKitDefArray[soundKitID];

    if (!def) {
        return SE3USAGE_ONESHOT;
    }

    if (def->m_pAdvancedSoundKitRec) {
        return def->m_pAdvancedSoundKitRec->m_usage;
    }

    if (def->m_pSoundKitRec) {
        return (def->m_pSoundKitRec->m_flags & 0x200) ? SE3USAGE_LOOPING : SE3USAGE_ONESHOT;
    }

    return SE3USAGE_ONESHOT;
}

// OFFSET: 0x00706FA0/0x1403D4490
int32_t SE3::ResetSoundKitOrder(int32_t soundKitID) {
    if (soundKitID >= static_cast<int32_t>(sm_SoundKitDefArray.Count())) {
        return SE3_ERR_INVALID_KIT;
    }

    if (soundKitID <= 0) {
        return SE3_ERR_INVALID_KIT;
    }

    SE3SoundKitDef* def = sm_SoundKitDefArray[soundKitID];

    if (!def) {
        return SE3_ERR_INVALID_KIT;
    }

    if (!def->m_nNumFiles) {
        return SE3_ERR_KIT_EMPTY;
    }

    memset(def->m_weights, 0, sizeof(def->m_weights));
    def->m_lastIndex = -1;

    return SE3_OK;
}

// OFFSET: 0x007071B0/0x1403D4840
void SE3::TrimCache() {
    while (SE3::sm_SoundCache_Stats_TotalSizeInBytes > SE3::sm_nMaxCacheSizeInBytes) {
        SE3WaveDataNode* node = SE3::sm_SoundCacheUnreferenced.Tail();

        if (!node) {
            break;
        }

        SE3::sm_SoundCacheUnreferenced.UnlinkNode(node);
        SE3::WaveData_PutOnDeleteList(node, 1);
    }
}

// OFFSET: 0x00707270
void SE3::ReleaseUnusedStreams() {
    SE3WaveDataNode* node = SE3::sm_Streams.Head();

    while (node) {
        SE3WaveDataNode* next = SE3::sm_Streams.Next(node);

        if (!node->m_nRefCount) {
            SE3::WaveData_PutOnDeleteList(node, 1);
        }

        node = next;
    }
}

// OFFSET: 0x007072B0/0x1403D4CE0
void SE3::InitSoundKitDefArray(uint32_t maxID) {
    sm_SoundKitDefArray.SetCount(maxID + 1);
    memset(sm_SoundKitDefArray.Ptr(), 0, (maxID + 1) * sizeof(SE3SoundKitDef*));

    for (uint32_t i = 0; i < maxID + 1; i++) {
        sm_SoundKitDefArray[i] = NULL;
    }
}

// OFFSET: 0x00707350/0x1403D4E50
static void OutWithTheOldInWithTheNew(SE3SoundKitObject* sko, uint32_t waveDataID) {
    int32_t stopped = 0;

    if (sko->m_channels[1].m_pFMODChannel) {
        stopped = 1;
        SE3::SoundKitObjectLog(sko, __FILE__, __LINE__, "Fading Out Sound Object [1] Still exists, stopping early\n");
        sko->m_channels[1].m_pFMODChannel->stop();
    }

    if (sko->m_channels[0].m_pFMODChannel) {
        sko->m_channels[1].m_pFMODChannel = sko->m_channels[0].m_pFMODChannel;
        sko->m_channels[1].m_baseFrequency = sko->m_channels[0].m_baseFrequency;
        sko->m_channels[1].m_fadeState = sko->m_channels[0].m_fadeState;
        sko->m_channels[1].m_fadeVolume = sko->m_channels[0].m_fadeVolume;
        sko->m_channels[1].m_waveDataID = sko->m_channels[0].m_waveDataID;
        stopped = 1;

        if (sko->m_usage) {
            sko->m_channels[0].m_pFMODChannel = NULL;
            sko->m_channels[0].m_waveDataID = waveDataID;
            return;
        }

        sko->m_channels[1].m_fadeState = SE3FADE_OUT;
        SE3::SoundKitObjectLog(sko, __FILE__, __LINE__, "Re-used kit fading out existing sound");
    }

    if (sko->m_usage) {
        sko->m_channels[0].m_pFMODChannel = NULL;
        sko->m_channels[0].m_waveDataID = waveDataID;
        return;
    }

    sko->m_channels[0].m_fadeState = SE3FADE_IN;
    sko->m_channels[0].m_pFMODChannel = NULL;

    if (stopped) {
        sko->m_channels[0].m_fadeVolume = 0.0f;
    }

    sko->m_channels[0].m_waveDataID = waveDataID;
}

// OFFSET: 0x00706AE0/0x1403D2AE0
int32_t SE3::ShouldSoundPlay(SE3SoundKitDef* def, SE3SoundKitProperties* properties, const C3Vector* position) {
    if (def && (def->m_pSoundKitRec->m_flags & 0x2000) && properties->m_conditionArg && properties->m_fpCondition && !properties->m_fpCondition(properties->m_conditionArg)) {
        return SE3_ERR_CONDITION;
    }

    if (position && (position->x != 0.0f || position->y != 0.0f || position->z != 0.0f) && properties->m_checkRange) {
        float distanceSquared = 0.0f;
        int32_t usage = properties->m_usage;

        if (usage != SE3USAGE_ONESHOT && (usage || (def->m_pSoundKitRec->m_flags & 0x200))) {
            if (IsBeyondListenerRange(position, def->m_pSoundKitRec->m_distanceCutoff + 25.0f, &distanceSquared)) {
                return SE3_ERR_OUT_OF_RANGE;
            }
        } else if (IsBeyondListenerRange(position, def->m_pSoundKitRec->m_distanceCutoff, &distanceSquared)) {
            return SE3_ERR_OUT_OF_RANGE;
        }
    }

    if (def) {
        int32_t uniqueness = properties->m_uniqueness;

        if ((uniqueness == 2 || (!uniqueness && (def->m_pSoundKitRec->m_flags & 0x20))) && def->m_nPlayingCount > 0) {
            return SE3_ERR_ALREADY_PLAYING;
        }
    }

    if (!SE3::SoundBusIsEnabled(properties->m_bus)) {
        return SE3_ERR_BUS_DISABLED;
    }

    return SE3::SoundBusIsFull(properties->m_bus) ? SE3_ERR_BUS_FULL : SE3_OK;
}

// OFFSET: 0x00707420/0x1403D5A20
int32_t SE3::IsTooCloseToExistingSoundKit(int32_t soundKitID, const C3Vector& position) {
    for (SE3SoundKitObject* sko = sm_SoundKitObjects_Loading.Head(); sko; sko = sm_SoundKitObjects_Loading.Next(sko)) {
        if (sko->m_pSoundKitDef && sko->m_pSoundKitDef->m_pSoundKitRec && sko->m_pSoundKitDef->m_pSoundKitRec->m_id == soundKitID) {
            float dz = position.z - sko->m_position.z;
            float dy = position.y - sko->m_position.y;

            if (dz * dz + (position.x - sko->m_position.x) * (position.x - sko->m_position.x) + dy * dy < 6.0f) {
                return 1;
            }
        }
    }

    for (SE3SoundKitObject* sko = SE3::sm_SoundKitObjects.Head(); sko; sko = SE3::sm_SoundKitObjects.Next(sko)) {
        if (sko->m_pSoundKitDef && sko->m_pSoundKitDef->m_pSoundKitRec && sko->m_pSoundKitDef->m_pSoundKitRec->m_id == soundKitID) {
            float dz = position.z - sko->m_position.z;
            float dy = position.y - sko->m_position.y;

            if (dz * dz + dy * dy + (position.x - sko->m_position.x) * (position.x - sko->m_position.x) < 6.0f) {
                return 1;
            }
        }
    }

    for (SE3SoundKitObject* sko = sm_SoundKitObjects_GoGoGo.Head(); sko; sko = sm_SoundKitObjects_GoGoGo.Next(sko)) {
        if (sko->m_pSoundKitDef && sko->m_pSoundKitDef->m_pSoundKitRec && sko->m_pSoundKitDef->m_pSoundKitRec->m_id == soundKitID) {
            float dz = position.z - sko->m_position.z;
            float dy = position.y - sko->m_position.y;

            if ((position.x - sko->m_position.x) * (position.x - sko->m_position.x) + dz * dz + dy * dy < 6.0f) {
                return 1;
            }
        }
    }

    return 0;
}

// OFFSET: 0x007075C0/0x1403D5BC0
static FMOD_RESULT F_CALLBACK DeviceChangedCallback(FMOD_SYSTEM* system, FMOD_SYSTEM_CALLBACKTYPE type, void* commanddata1, void* commanddata2) {
    if (type != FMOD_SYSTEM_CALLBACKTYPE_DEVICELISTCHANGED || !s_deviceChangeCallback) {
        return FMOD_OK;
    }

    int32_t numDrivers = SE3::GetNumOutputDrivers_Cached(SE3::sm_pGameSystem);
    SE3::sm_aCachedGameSoundOutputDriverInfo.SetCount(numDrivers);

    for (int32_t i = 0; i < numDrivers; i++) {
        SE3::GetOutputDriverName(i, SE3::sm_aCachedGameSoundOutputDriverInfo[i].m_name, sizeof(SE3DriverInfo), SE3::sm_pGameSystem);
    }

    numDrivers = SE3::GetNumOutputDrivers_Cached(SE3::sm_pChatSystem);
    SE3::sm_aCachedChatSoundOutputDriverInfo.SetCount(numDrivers);

    for (int32_t i = 0; i < numDrivers; i++) {
        SE3::GetOutputDriverName(i, SE3::sm_aCachedChatSoundOutputDriverInfo[i].m_name, sizeof(SE3DriverInfo), SE3::sm_pChatSystem);
    }

    numDrivers = SE3::GetNumInputDrivers(SE3::sm_pChatSystem);
    SE3::sm_aCachedChatSoundInputDriverInfo.SetCount(numDrivers);

    for (int32_t i = 0; i < numDrivers; i++) {
        SE3::GetInputDriverName(i, SE3::sm_aCachedChatSoundInputDriverInfo[i].m_name, sizeof(SE3DriverInfo), SE3::sm_pChatSystem);
    }

    s_deviceChangeCallback();

    return FMOD_OK;
}

// OFFSET: 0x00707800/0x1403D5F10
void SE3::SetDeviceChangeCallback(void (*callback)()) {
    s_deviceChangeCallback = callback;
    FMOD_RESULT result = SE3::sm_pGameSystem->setCallback(FMOD_SYSTEM_CALLBACKTYPE_DEVICELISTCHANGED, DeviceChangedCallback);
    SE3::Log_Write(__LINE__, __FILE__, result, " - Setting device callback");
}

// OFFSET: 0x00707B00/0x1403D4930
void SE3::ProcessFadeList(uint32_t elapsed) {
    SE3SoundKitObject* sko = sm_SoundKitObjects_FadeList.Head();

    while (sko) {
        SE3SoundKitObject* next = sm_SoundKitObjects_FadeList.Next(sko);

        if (sko->m_fadeState == SE3FADE_IN) {
            if (sko->m_properties.m_fadeInTime <= 0) {
                sko->m_fadeVolume = 1.0f;
            } else {
                sko->m_fadeVolume = static_cast<float>(elapsed) / static_cast<float>(sko->m_properties.m_fadeInTime) + sko->m_fadeVolume;
            }

            if (sko->m_fadeVolume >= 1.0f) {
                sko->m_fadeVolume = 1.0f;
                sko->m_fadeState = SE3FADE_NONE;
                SE3::SoundKitObjectLog(sko, __FILE__, __LINE__, "Done Fading In, Unlinking from sm_SoundKitObjects_FadeList");
                sm_SoundKitObjects_FadeList.UnlinkNode(sko);
            }
        } else if (sko->m_fadeState == SE3FADE_OUT) {
            if (sko->m_properties.m_fadeOutTime <= 0) {
                sko->m_fadeVolume = 0.0f;
            } else {
                sko->m_fadeVolume = sko->m_fadeVolume - static_cast<float>(elapsed) / static_cast<float>(sko->m_properties.m_fadeOutTime);
            }

            if (sko->m_fadeVolume <= 0.0f) {
                SE3::SoundKitObjectLog(sko, __FILE__, __LINE__, "Done Fading Out, Unlinking from sm_SoundKitObjects_FadeList");
                sko->m_fadeVolume = 0.0f;
                sm_SoundKitObjects_FadeList.UnlinkNode(sko);
                SE3::SoundKitObjectLog(sko, __FILE__, __LINE__, "Stop all playing channels...");

                int32_t stopped = 0;

                for (int32_t i = 0; i < 2; i++) {
                    if (sko->m_channels[i].m_pFMODChannel) {
                        stopped++;
                        SE3::SoundKitObjectLog(sko, __FILE__, __LINE__, "Object [%d] Stop Channel", i);
                        sko->m_channels[i].m_pFMODChannel->stop();
                    }
                }

                SE3::SoundKitObjectLog(sko, __FILE__, __LINE__, "Done Stopping [%d] Playing Channels", stopped);

                if (!stopped) {
                    SE3::SoundKitObjectLog(sko, __FILE__, __LINE__, "Sound finished fading out but didn't have any channels playing...");
                    SE3::SoundKitObjectLog(sko, __FILE__, __LINE__, "Done Fading Out, Unlinking from SE3::sm_SoundKitObjects");
                    SE3::sm_SoundKitObjects.UnlinkNode(sko);
                    SE3::SoundKitObjectLog(sko, __FILE__, __LINE__, "Done Fading Out, Linking to sm_SoundKitObjects_DeleteList");
                    sm_SoundKitObjects_DeleteList.LinkToTail(sko);
                }
            }
        }

        sko->UpdateVolume();
        sko = next;
    }
}

// OFFSET: 0x00707D90/0x1403D6130
void SE3::UpdateVolumes() {
    for (SE3SoundKitObject* sko = SE3::sm_SoundKitObjects.Head(); sko; sko = SE3::sm_SoundKitObjects.Next(sko)) {
        sko->UpdateVolume();
    }

    for (SE3MemorySoundObject* sound = SE3::sm_MemorySoundObjectTable.Head(); sound; sound = SE3::sm_MemorySoundObjectTable.Next(sound)) {
        if (sound->m_pFMODChannel) {
            sound->m_pFMODChannel->setVolume(SE3::SoundBusGetVolume(SE3BUS_MOVIE));
        }
    }
}

// OFFSET: 0x00707E10
SE3SoundKitDef::SE3SoundKitDef() {
    this->m_nNumFiles = 0;
    memset(this->m_weights, 0, sizeof(this->m_weights));
    this->m_lastIndex = -1;
    this->m_ownsRecords = 0;
    this->m_hasTimeOfDay = 0;
    this->m_notLocal = 0;
    this->m_pSoundGroup = NULL;
    this->m_pSoundKitRec = NULL;
    this->m_pAdvancedSoundKitRec = NULL;
    this->m_timeOfDay[0] = 0;
    this->m_timeOfDay[1] = 0;
    this->m_timeOfDay[2] = 0;
    this->m_timeOfDay[3] = 0;
    this->m_pAsyncObj = NULL;
    this->m_downloaded = 0;
    this->m_nPlayingCount = 0;
}

// OFFSET: 0x00707E60/0x1403D61C0
SE3SoundKitDef::~SE3SoundKitDef() {
    if (this->m_ownsRecords) {
        SoundEntriesRec* rec = const_cast<SoundEntriesRec*>(this->m_pSoundKitRec);

        if (rec) {
            if (rec->m_name) {
                SMemFree(const_cast<char*>(static_cast<const char*>(rec->m_name)), __FILE__, __LINE__, 0);
            }

            if (rec->m_directoryBase) {
                SMemFree(const_cast<char*>(static_cast<const char*>(rec->m_directoryBase)), __FILE__, __LINE__, 0);
            }

            for (int32_t i = 0; i < 10; i++) {
                if (rec->m_file[i]) {
                    SMemFree(const_cast<char*>(static_cast<const char*>(rec->m_file[i])), __FILE__, __LINE__, 0);
                }
            }

            SMemFree(rec, __FILE__, __LINE__, 0);
        }

        SoundEntriesAdvancedRec* advancedRec = const_cast<SoundEntriesAdvancedRec*>(this->m_pAdvancedSoundKitRec);

        if (advancedRec) {
            SMemFree(advancedRec, __FILE__, __LINE__, 0);
        }
    }

    if (this->m_pSoundGroup) {
        this->m_pSoundGroup->release();
        this->m_pSoundGroup = NULL;
    }

    this->m_pAsyncObj = NULL;
}

// OFFSET: 0x00707F80/0x1403D4DA0
static void FadeInNow(SE3SoundKitObject* sko, const char* file, int32_t line, const char* function) {
    sko->m_fadeState = SE3FADE_IN;
    SE3::SoundKitObjectLog(sko, __FILE__, __LINE__, "Fade in NOW (fade time %d ms) - Link to sm_SoundKitObjects_FadeList\ncalled from\n%s(%d):\t%s", sko->m_properties.m_fadeInTime, file, line, function);
    sm_SoundKitObjects_FadeList.LinkToTail(sko);
}

// INLINED
static void FadeOutNow(SE3SoundKitObject* sko, const char* file, int32_t line, const char* function) {
    sko->m_fadeState = SE3FADE_OUT;
    SE3::SoundKitObjectLog(sko, __FILE__, __LINE__, "Fade out NOW (fade time %d ms) - Link to sm_SoundKitObjects_FadeList\ncalled from\n%s(%d):\t%s", sko->m_properties.m_fadeOutTime, file, line, function);
    sm_SoundKitObjects_FadeList.LinkToTail(sko);
}

// OFFSET: 0x00708010/0x1403D4F30
void SE3::StopOrFadeOut(SE3SoundKitObject* sko, int32_t fadeOutTime, const char* file, int32_t line, const char* function) {
    if (fadeOutTime >= 0) {
        sko->m_properties.m_fadeOutTime = fadeOutTime;
    }

    if (sko->m_usage == SE3USAGE_PERIODIC) {
        SE3::SoundKitObjectLog(sko, __FILE__, __LINE__, "Forcing PERIODIC sound to ONE SHOT so it can die peacefully");
        sko->m_usage = SE3USAGE_ONESHOT;
    }

    if (sko->m_properties.m_fadeOutTime > 0) {
        SE3::SoundKitObjectLog(sko, __FILE__, __LINE__, "StopOrFadeOut(): Fade Out...called from:\n%s(%d):\t%s", file, line, function);
        FadeOutNow(sko, __FILE__, __LINE__, "SE3::StopOrFadeOut");
        return;
    }

    int32_t stopped = 0;

    for (int32_t i = 0; i < 2; i++) {
        if (sko->m_channels[i].m_pFMODChannel) {
            stopped++;
            SE3::SoundKitObjectLog(sko, __FILE__, __LINE__, "StopSound: Stop Immediately (object %d)...called from:\n%s\n%s(%d)", i, function, file, line);
            sko->m_channels[i].m_pFMODChannel->stop();
        }
    }

    if (!stopped) {
        SE3::SoundKitObjectLog(sko, __FILE__, __LINE__, "StopSound: Periodic sound between tweets: Unlink from SE3::sm_SoundKitObjects, Link to sm_SoundKitObjects_DeleteList");
        SE3::sm_SoundKitObjects.UnlinkNode(sko);
        sm_SoundKitObjects_DeleteList.LinkToTail(sko);
    }
}

// OFFSET: 0x00708250/0x1403D51E0
void SE3::ManageOutOfRangeSounds(int32_t elapsed) {
    static int32_t s_timer;

    s_timer -= elapsed;

    if (s_timer > 0) {
        return;
    }

    s_timer = 2000;

    SE3SoundKitObject* sko = SE3::sm_SoundKitObjects.Head();

    while (sko) {
        SE3SoundKitObject* next = SE3::sm_SoundKitObjects.Next(sko);

        if (sko->m_properties.m_checkRange && (sko->m_position.x != 0.0f || sko->m_position.y != 0.0f || sko->m_position.z != 0.0f)) {
            float range = sko->m_pSoundKitDef->m_pSoundKitRec->m_distanceCutoff + 50.0f;

            C3Vector listener;
            SE3::sm_pGameSystem->get3DListenerAttributes(0, reinterpret_cast<FMOD_VECTOR*>(&listener), NULL, NULL, NULL);

            if ((listener.x - sko->m_position.x) * (listener.x - sko->m_position.x) + (listener.y - sko->m_position.y) * (listener.y - sko->m_position.y) + (listener.z - sko->m_position.z) * (listener.z - sko->m_position.z) > range * range) {
                SE3::StopOrFadeOut(sko, 500, __FILE__, __LINE__, "Dumped out of range sound to free up channels");
            }
        }

        sko = next;
    }
}

// OFFSET: 0x00708420/0x1403D52E0
void SE3::SoundKitObjects_Loading_Delete(uint32_t waveDataID) {
    SE3SoundKitObject* sko = sm_SoundKitObjects_Loading.Head();

    while (sko) {
        SE3SoundKitObject* next = sm_SoundKitObjects_Loading.Next(sko);
        int32_t valid = 0;

        for (int32_t i = 0; i < 2; i++) {
            if (sko->m_channels[i].m_waveDataID == waveDataID) {
                sko->m_channels[i].m_pFMODChannel = NULL;
                sko->m_channels[i].m_waveDataID = 0;
            } else if (sko->m_channels[i].m_pFMODChannel) {
                valid++;
            }
        }

        if (!valid) {
            SE3::SoundKitObjectLog(sko, __FILE__, __LINE__, "No More Valid Channels, linking to SoundKitObjects_DeleteList");
            sm_SoundKitObjects_Loading.UnlinkNode(sko);
            SE3::SoundKitObjectLog(sko, __FILE__, __LINE__, "Unlink from sm_SoundKitObjects_Loading, Link to sm_SoundKitObjects_DeleteList");
            sm_SoundKitObjects_DeleteList.LinkToTail(sko);
        }

        sko = next;
    }
}

// OFFSET: 0x00708570/0x1403D5470
void SE3::SoundKitObjects_HandleWaveDataDelete(uint32_t waveDataID) {
    SE3SoundKitObject* sko = sm_SoundKitObjects_Loading.Head();

    while (sko) {
        SE3SoundKitObject* next = sm_SoundKitObjects_Loading.Next(sko);
        int32_t valid = 0;

        for (int32_t i = 0; i < 2; i++) {
            if (sko->m_channels[i].m_waveDataID == waveDataID) {
                sko->m_channels[i].m_pFMODChannel = NULL;
                sko->m_channels[i].m_waveDataID = 0;
            } else if (sko->m_channels[i].m_pFMODChannel) {
                valid++;
            }
        }

        if (!valid) {
            SE3::SoundKitObjectLog(sko, __FILE__, __LINE__, "No More Valid Channels, linking to SoundKitObjects_DeleteList");
            sm_SoundKitObjects_Loading.UnlinkNode(sko);
            SE3::SoundKitObjectLog(sko, __FILE__, __LINE__, "Unlink from sm_SoundKitObjects_Loading, Link to sm_SoundKitObjects_DeleteList");
            sm_SoundKitObjects_DeleteList.LinkToTail(sko);
        }

        sko = next;
    }

    sko = SE3::sm_SoundKitObjects.Head();

    while (sko) {
        SE3SoundKitObject* next = SE3::sm_SoundKitObjects.Next(sko);
        int32_t valid = 0;

        for (int32_t i = 0; i < 2; i++) {
            if (sko->m_channels[i].m_waveDataID == waveDataID) {
                sko->m_channels[i].m_pFMODChannel = NULL;
                sko->m_channels[i].m_waveDataID = 0;
            } else if (sko->m_channels[i].m_pFMODChannel) {
                valid++;
            }
        }

        if (!valid) {
            SE3::SoundKitObjectLog(sko, __FILE__, __LINE__, "No More Valid Channels, linking to SoundKitObjects_DeleteList");
            SE3::sm_SoundKitObjects.UnlinkNode(sko);
            SE3::SoundKitObjectLog(sko, __FILE__, __LINE__, "Unlink from SE3::sm_SoundKitObjects, Link to sm_SoundKitObjects_DeleteList");
            sm_SoundKitObjects_DeleteList.LinkToTail(sko);
        }

        sko = next;
    }

    sko = sm_SoundKitObjects_GoGoGo.Head();

    while (sko) {
        SE3SoundKitObject* next = sm_SoundKitObjects_GoGoGo.Next(sko);
        int32_t valid = 0;

        for (int32_t i = 0; i < 2; i++) {
            if (sko->m_channels[i].m_waveDataID == waveDataID) {
                sko->m_channels[i].m_pFMODChannel = NULL;
                sko->m_channels[i].m_waveDataID = 0;
            } else if (sko->m_channels[i].m_pFMODChannel) {
                valid++;
            }
        }

        if (!valid) {
            SE3::SoundKitObjectLog(sko, __FILE__, __LINE__, "No More Valid Channels, linking to SoundKitObjects_DeleteList");
            sm_SoundKitObjects_GoGoGo.UnlinkNode(sko);
            SE3::SoundKitObjectLog(sko, __FILE__, __LINE__, "Unlink from sm_SoundKitObjects_GoGoGo, Link to sm_SoundKitObjects_DeleteList");
            sm_SoundKitObjects_DeleteList.LinkToTail(sko);
        }

        sko = next;
    }
}

// OFFSET: 0x007088E0/0x1403D5890
void SE3::SoundKitObjects_Loading_Ready(uint32_t waveDataID) {
    if (SE3::sm_bShuttingDown) {
        return;
    }

    SE3SoundKitObject* sko = sm_SoundKitObjects_Loading.Head();

    while (sko) {
        SE3SoundKitObject* next = sm_SoundKitObjects_Loading.Next(sko);

        for (int32_t i = 0; i < 2; i++) {
            if (sko->m_channels[i].m_waveDataID == waveDataID) {
                sm_SoundKitObjects_Loading.UnlinkNode(sko);
                SE3::SoundKitObjectLog(sko, __FILE__, __LINE__, "Unlink from sm_SoundKitObjects_Loading, Link to sm_SoundKitObjects_GoGoGo");
                sm_SoundKitObjects_GoGoGo.LinkToTail(sko);
                break;
            }
        }

        sko = next;
    }
}

// OFFSET: 0x007089F0/0x1403D59E0
void SE3::CacheEventCallback(SE3CACHEEVENT event, uint32_t waveDataID) {
    switch (event) {
        case SE3CACHEEVENT_LOADING_FAILED:
            SE3::SoundKitObjects_Loading_Delete(waveDataID);
            break;

        case SE3CACHEEVENT_DELETED:
            SE3::SoundKitObjects_HandleWaveDataDelete(waveDataID);
            break;

        case SE3CACHEEVENT_LOADED:
            SE3::SoundKitObjects_Loading_Ready(waveDataID);
            break;
    }
}

// OFFSET: 0x00709130/0x1403D6F10
SE3SoundKitObject::SE3SoundKitObject() {
    this->m_uUID = ++SE3::sm_uNextUniqueID;
    this->m_pMySKOL = NULL;
    this->m_randomVolume = 1.0f;
    this->m_position.x = 0.0f;
    this->m_position.y = 0.0f;
    this->m_position.z = 0.0f;
    this->m_positionOffset.x = 0.0f;
    this->m_positionOffset.y = 0.0f;
    this->m_positionOffset.z = 0.0f;
    this->m_notCacheable = 0;
    this->m_reverb = 1;
    this->m_bit2 = 0;
    this->m_hasPosition = 0;
    this->unk_006C = 0;
    this->unk_0070 = 0;
    this->m_modeFlags = 0;
    this->m_fadeState = SE3FADE_NONE;
    this->m_fadeVolume = 1.0f;
    this->m_fpPlayFinishedCallback = NULL;
    this->m_periodicTimer = 0;
    this->m_pSoundKitDef = NULL;
    this->unk_0098 = 0;
    this->unk_009C = 0;
    this->m_fpCustomDSPChainCallback = NULL;
    this->m_sCustomDSPChainName = NULL;
    this->m_priority = 128;
    this->m_frequencyScale = 0.0f;
    this->unk_0180 = 0;
    this->m_fTimeOfDayScalar = 1.0f;
    this->m_timeOfDayOffset = 0;
    this->m_virtualTime = 0;
    this->m_properties.ResetToDefaults();
    SE3::SoundKitObjectLog(this, __FILE__, __LINE__, "Created() AS Handle [%d]", this->m_uUID);
}

// OFFSET: 0x007092E0/0x1403D7130
SE3WaveDataNode::~SE3WaveDataNode() {
}

// OFFSET: 0x00709530/0x1403D7E20
SE3SoundKitObject::~SE3SoundKitObject() {
    SE3::UnregisterSuppressor(this->m_uUID);

    if (this->m_sCustomDSPChainName) {
        SMemFree(this->m_sCustomDSPChainName, "delete[]", -1, 0);
        this->m_sCustomDSPChainName = NULL;
    }
}

// OFFSET: 0x00709730/0x1403D8120
void SE3::FreeSoundKitDefList() {
    for (uint32_t i = 0; i < sm_SoundKitDefArray.Count(); i++) {
        if (sm_SoundKitDefArray[i]) {
            if (sm_SoundKitDefArray[i]->m_pAsyncObj) {
                AsyncFileReadCancel(sm_SoundKitDefArray[i]->m_pAsyncObj, DefaultAsyncObjectCleanupCallback);
                sm_SoundKitDefArray[i]->m_pAsyncObj = NULL;
            }

            delete sm_SoundKitDefArray[i];
        }
    }

    sm_SoundKitDefArray.Clear();
    s_soundKitLookup.Clear();
}

// OFFSET: 0x00709830/0x1403D6370
void SE3::StopAllSounds(SE3BUS bus, int32_t fadeOutTime) {
    SE3SoundKitObject* sko = SE3::sm_SoundKitObjects.Head();

    while (sko) {
        SE3SoundKitObject* next = SE3::sm_SoundKitObjects.Next(sko);

        if (SE3::IsBusEqualOrChild(sko->m_properties.m_bus, bus)) {
            if (fadeOutTime <= 0) {
                for (int32_t i = 0; i < 2; i++) {
                    if (sko->m_channels[i].m_pFMODChannel) {
                        sko->m_channels[i].m_pFMODChannel->stop();
                    }
                }
            } else {
                sko->m_properties.m_fadeOutTime = fadeOutTime;
                FadeOutNow(sko, __FILE__, __LINE__, "SE3::StopAllSounds");
            }
        }

        sko = next;
    }

    sko = sm_SoundKitObjects_GoGoGo.Head();

    while (sko) {
        SE3SoundKitObject* next = sm_SoundKitObjects_GoGoGo.Next(sko);

        if (SE3::IsBusEqualOrChild(sko->m_properties.m_bus, bus)) {
            SE3::SoundKitObjectLog(sko, __FILE__, __LINE__, "Unlink from sm_SoundKitObjects_GoGoGo, Link to sm_SoundKitObjects_DeleteList");
            sm_SoundKitObjects_GoGoGo.UnlinkNode(sko);
            sm_SoundKitObjects_DeleteList.LinkToTail(sko);
        }

        sko = next;
    }

    sko = sm_SoundKitObjects_Loading.Head();

    while (sko) {
        SE3SoundKitObject* next = sm_SoundKitObjects_Loading.Next(sko);

        if (SE3::IsBusEqualOrChild(sko->m_properties.m_bus, bus)) {
            SE3::SoundKitObjectLog(sko, __FILE__, __LINE__, "Unlink from sm_SoundKitObjects_Loading, Link to sm_SoundKitObjects_DeleteList");
            sm_SoundKitObjects_Loading.UnlinkNode(sko);
            sm_SoundKitObjects_DeleteList.LinkToTail(sko);
        }

        sko = next;
    }

    sko = SE3::sm_SoundKitObjects.Head();

    while (sko) {
        SE3SoundKitObject* next = SE3::sm_SoundKitObjects.Next(sko);

        if (sko->m_usage == SE3USAGE_PERIODIC) {
            SE3::SoundKitObjectLog(sko, __FILE__, __LINE__, "(StopAllSounds() Deleting Periodic Sound");
            SE3::sm_SoundKitObjects.UnlinkNode(sko);
            sm_SoundKitObjects_DeleteList.LinkToTail(sko);
        }

        sko = next;
    }
}

// OFFSET: 0x0070A270/0x1403D9910
void SE3::ProcessSoundKitObjectDeleteList() {
    SE3SoundKitObject* sko = sm_SoundKitObjects_DeleteList.Head();

    while (sko) {
        SE3SoundKitObject* next = sm_SoundKitObjects_DeleteList.Next(sko);
        SE3::SoundKitObjectLog(sko, __FILE__, __LINE__, "ProcessSoundKitObjectDeleteList: Delete!");

        SE3SOUNDKITOBJECT_LOOKUP* lookup = sko->m_pMySKOL;
        SE3::sm_SoundKitObjectTable.Delete(lookup);
        sko->m_pMySKOL = NULL;

        delete sko;
        sko = next;
    }
}

// OFFSET: 0x0070A300/0x1403D9A60
static SE3StreamInfo* CreateStreamInfo(const uint8_t* key, SFileImpl* file) {
    SE3StreamInfo* info = new (__FILE__, __LINE__) SE3StreamInfo();
    info->m_file = file;
    info->m_pendingReads = 0;
    info->m_closeRequested = 0;
    info->m_nextBlock = 0;
    info->m_headerPosition = 0;
    info->m_headerState = 0;

    if (key) {
        memcpy(info->m_key, key, sizeof(info->m_key));
        info->m_encrypted = 1;
    } else {
        info->m_encrypted = 0;
    }

    return info;
}

// OFFSET: 0x0070A380/0x1403D8280
void SE3::AddMessageToQueue_MemorySoundEnded(uint32_t uid) {
    if (SE3::sm_bShuttingDown) {
        return;
    }

    sm_MemorySoundEnded_MessageQueue_CritSect.Enter();

    SE3MemorySoundEndedMessage* message = s_MemorySoundEnded_MessagePool.New<SE3MemorySoundEndedMessage>(0);
    message->m_uUID = uid;
    sm_MemorySoundEnded_MessageQueue.LinkToTail(message);

    sm_MemorySoundEnded_MessageQueue_CritSect.Leave();
}

// OFFSET: 0x0070A3F0/0x1403D8350
void SE3::AddMessageToQueue_SoundEnded(FMOD::Channel* channel) {
    if (SE3::sm_bShuttingDown) {
        return;
    }

    void* userdata = NULL;
    channel->getUserData(&userdata);

    sm_SoundEnded_MessageQueue_CritSect.Enter();

    SE3SoundEndedMessage* message = s_SoundEnded_MessagePool.New<SE3SoundEndedMessage>(0);
    message->m_pFMODChannel = channel;
    message->m_uSoundKitObjectUID = reinterpret_cast<uint32_t>(userdata);
    sm_SoundEnded_MessageQueue.LinkToTail(message);

    sm_SoundEnded_MessageQueue_CritSect.Leave();
}

// OFFSET: 0x0070A470/0x1403D8450
FMOD_RESULT SE3::AddMessageToQueue_DoneLoading(FMOD_SOUND* sound, FMOD_RESULT result) {
    if (!SE3::sm_pGameSystem || SE3::sm_bShuttingDown) {
        return FMOD_OK;
    }

    SE3::sm_DoneLoading_MessageQueue_CritSect.Enter();

    SE3DoneLoadingMessage* message = s_DoneLoading_MessagePool.New<SE3DoneLoadingMessage>(0);
    message->m_pFMODSound = reinterpret_cast<FMOD::Sound*>(sound);
    message->m_result = result;
    SE3::sm_DoneLoading_MessageQueue.LinkToTail(message);

    SE3::sm_DoneLoading_MessageQueue_CritSect.Leave();

    return FMOD_OK;
}

// OFFSET: 0x0070A4F0/0x1403D8650
void SE3::EmitterManagerSendMessage_MarkSoundChunkDirty(SE3SoundChunk* chunk) {
    s_emitterManagerCritSect.Enter();

    if (s_bEmitterManagerShutDown) {
        s_emitterManagerCritSect.Leave();
        return;
    }

    s_emitterManagerCritSect.Leave();

    s_markSoundChunkDirtyMessagesCritSect.Enter();

    SE3MarkSoundChunkDirtyMessage* message = s_markSoundChunkDirtyMessagePool.New<SE3MarkSoundChunkDirtyMessage>(0);
    message->m_pSoundChunk = chunk;
    s_markSoundChunkDirtyMessages.LinkToTail(message);

    s_markSoundChunkDirtyMessagesCritSect.Leave();
}

// OFFSET: 0x0070A570/0x1403D8740
void SE3::EmitterManagerSendMessage_CreateEmitter(uint32_t* handle, SE3SoundChunk* chunk, int32_t soundKitID, const C3Vector& position, SE3SoundKitProperties* properties, const TSGrowableArray<C3Vector>* positions, int32_t userData) {
    if (!SE3::sm_pGameSystem || SE3::sm_eGameSystemOutputType == FMOD_OUTPUTTYPE_NOSOUND) {
        return;
    }

    s_emitterManagerCritSect.Enter();

    if (s_bEmitterManagerShutDown) {
        s_emitterManagerCritSect.Leave();
        return;
    }

    s_emitterManagerCritSect.Leave();

    *handle = s_uNextEmitterHandle++;

    s_createEmitterMessagesCritSect.Enter();

    SE3CreateSoundEmitterMessage* message = s_createEmitterMessagePool.New<SE3CreateSoundEmitterMessage>(0);
    message->m_pParentSoundChunk = chunk;
    message->m_hSoundEmitter = *handle;
    message->m_nSoundKitID = soundKitID;
    message->m_position = position;
    memcpy(&message->m_properties, properties, sizeof(SE3SoundKitProperties));

    if (positions) {
        uint32_t count = positions->m_count;
        message->m_positions.SetCount(count);

        for (uint32_t i = 0; i < count; i++) {
            message->m_positions.m_data[i] = positions->m_data[i];
        }
    }

    message->m_userData = userData;
    s_createEmitterMessages.LinkToTail(message);

    s_createEmitterMessagesCritSect.Leave();
}

// OFFSET: 0x0070A680/0x1403D8920
void SE3::EmitterMangerSendMessage_DestroyEmitter(uint32_t handle) {
    s_emitterManagerCritSect.Enter();

    if (s_bEmitterManagerShutDown) {
        s_emitterManagerCritSect.Leave();
        return;
    }

    s_emitterManagerCritSect.Leave();

    s_destroyEmitterMessagesCritSect.Enter();

    SE3DestroySoundEmitterMessage* message = s_destroyEmitterMessagePool.New<SE3DestroySoundEmitterMessage>(0);
    message->m_hSoundEmitter = handle;
    s_destroyEmitterMessages.LinkToTail(message);

    s_destroyEmitterMessagesCritSect.Leave();
}

// OFFSET: 0x0070A700/0x1403D8A00
void SE3::EmitterManagerSendMessage_Set3DPosition(uint32_t handle, const C3Vector& position) {
    s_emitterManagerCritSect.Enter();

    if (s_bEmitterManagerShutDown) {
        s_emitterManagerCritSect.Leave();
        return;
    }

    s_emitterManagerCritSect.Leave();

    s_set3DPositionMessagesCritSect.Enter();

    SE3Set3DPositionMessage* message = s_set3DPositionMessagePool.New<SE3Set3DPositionMessage>(0);
    message->m_hSoundEmitter = handle;
    message->m_position = position;
    s_set3DPositionMessages.LinkToTail(message);

    s_set3DPositionMessagesCritSect.Leave();
}

// OFFSET: 0x0070B0C0/0x1403D9090
void SE3::ProcessMemorySoundEndedMessageQueue() {
    TSList<SE3MemorySoundEndedMessage, TSGetLink<SE3MemorySoundEndedMessage> > messages;

    sm_MemorySoundEnded_MessageQueue_CritSect.Enter();

    for (SE3MemorySoundEndedMessage* message = sm_MemorySoundEnded_MessageQueue.Head(); message; message = sm_MemorySoundEnded_MessageQueue.Head()) {
        sm_MemorySoundEnded_MessageQueue.UnlinkNode(message);
        messages.LinkToTail(message);
    }

    sm_MemorySoundEnded_MessageQueue_CritSect.Leave();

    SE3MemorySoundEndedMessage* message = messages.Head();

    while (message) {
        SE3MemorySoundEndedMessage* next = messages.Next(message);
        SE3MemorySoundObject* sound = SE3::sm_MemorySoundObjectTable.Ptr(message->m_uUID, s_lookupKey);

        if (sound) {
            if (sound->m_fpPlayFinishedCallback) {
                sound->m_fpPlayFinishedCallback(sound, sound->m_playFinishedParam);
            }

            sound->m_pFMODSound->release();
            SE3::sm_MemorySoundObjectTable.Delete(sound);
        }

        messages.UnlinkNode(message);
        s_MemorySoundEnded_MessagePool.PutData(message, NULL, 0);
        message = next;
    }
}

// OFFSET: 0x0070B2B0/0x1403D9420
void SE3::ProcessSoundEndedMessageQueue() {
    TSList<SE3SoundEndedMessage, TSGetLink<SE3SoundEndedMessage> > messages;

    sm_SoundEnded_MessageQueue_CritSect.Enter();

    for (SE3SoundEndedMessage* message = sm_SoundEnded_MessageQueue.Head(); message; message = sm_SoundEnded_MessageQueue.Head()) {
        sm_SoundEnded_MessageQueue.UnlinkNode(message);
        messages.LinkToTail(message);
    }

    sm_SoundEnded_MessageQueue_CritSect.Leave();

    SE3SoundEndedMessage* message = messages.Head();

    while (message) {
        SE3SoundEndedMessage* next = messages.Next(message);
        FMOD::Channel* channel = message->m_pFMODChannel;
        uint32_t uid = message->m_uSoundKitObjectUID;
        uint32_t waveDataID = 0;
        int32_t numValid = 0;

        SE3SoundKitObject* sko = SE3::sm_SoundKitObjects.Head();

        while (sko) {
            SE3SoundKitObject* skoNext = SE3::sm_SoundKitObjects.Next(sko);

            if (sko->m_uUID == uid || sko->m_uUID == 1) {
                int32_t found = 0;

                for (int32_t i = 0; i < 2; i++) {
                    if (sko->m_channels[i].m_pFMODChannel == channel) {
                        SE3::SoundKitObjectLog(sko, __FILE__, __LINE__, "Sound object %d owns the ended sound", i);
                        waveDataID = sko->m_channels[i].m_waveDataID;
                        sko->m_channels[i].m_pFMODChannel = NULL;
                        sko->m_channels[i].m_waveDataID = 0;
                        found = 1;

                        if (sko->m_pSoundKitDef) {
                            sko->m_pSoundKitDef->m_nPlayingCount--;
                        }
                    }

                    if (sko->m_channels[i].m_pFMODChannel) {
                        numValid++;
                    }
                }

                if (found) {
                    SE3::SoundKitObjectLog(sko, __FILE__, __LINE__, "SoundEndedCallback()");
                    SE3::SoundKitObjectLog(sko, __FILE__, __LINE__, "Num Valid Channels == %d", numValid);
                    SE3::SoundKitObjectLog(sko, __FILE__, __LINE__, "Usage == %s", sko->m_usage == SE3USAGE_PERIODIC ? "PERIODIC" : (sko->m_usage == SE3USAGE_ONESHOT ? "ONESHOT" : "LOOPING"));

                    if (!numValid && sko->m_usage != SE3USAGE_PERIODIC) {
                        SE3::SoundKitObjectLog(sko, __FILE__, __LINE__, "No More Valid Channels, linking to SoundKitObjects_DeleteList");
                        SE3::sm_SoundKitObjects.UnlinkNode(sko);
                        SE3::SoundKitObjectLog(sko, __FILE__, __LINE__, "Unlink from SE3::sm_SoundKitObjects, Link to sm_SoundKitObjects_DeleteList");
                        sm_SoundKitObjects_DeleteList.LinkToTail(sko);
                    }

                    SE3::SoundBusDecrementChannelCount(sko->m_properties.m_bus);

                    if (sko->m_fpPlayFinishedCallback) {
                        sko->m_fpPlayFinishedCallback(sko);
                    }

                    break;
                }
            }

            sko = skoNext;
            numValid = 0;
        }

        SE3::DecrementWaveDataNodeRefCount(waveDataID);
        messages.UnlinkNode(message);
        s_SoundEnded_MessagePool.PutData(message, NULL, 0);
        message = next;
    }
}

// OFFSET: 0x00707F60/0x1403D6340
int32_t SE3::GetSoundKitID(const char* name) {
    SOUNDKITLOOKUP* lookup = s_soundKitLookup.Ptr(name);

    if (lookup) {
        return lookup->m_nSoundKitID;
    }

    return 0;
}

// OFFSET: 0x0070B640/0x1403D9B20
static FMOD_RESULT F_CALLBACK SoundEndedCallback(FMOD_CHANNEL* channel, FMOD_CHANNEL_CALLBACKTYPE type, int command, unsigned int commanddata1, unsigned int commanddata2) {
    if (type == FMOD_CHANNEL_CALLBACKTYPE_END) {
        SE3::sm_SE3ShuttingDown_CritSec.Enter();
        SE3::AddMessageToQueue_SoundEnded(reinterpret_cast<FMOD::Channel*>(channel));
        SE3::sm_SE3ShuttingDown_CritSec.Leave();
    }

    return FMOD_OK;
}

// OFFSET: 0x0070B670/0x1403D9B60
void SE3::DoneLoadingMessageQueuePut(SE3DoneLoadingMessage* message) {
    SE3::sm_DoneLoading_MessageQueue_CritSect.Enter();
    SE3::sm_DoneLoading_MessageQueue.UnlinkNode(message);
    s_DoneLoading_MessagePool.PutData(message, NULL, 0);
    SE3::sm_DoneLoading_MessageQueue_CritSect.Leave();
}

// OFFSET: 0x0070C470/0x1403DAB00
SE3COMBINABLESOUNDLIST::SE3COMBINABLESOUNDLIST() {
}

// OFFSET: 0x0070B6E0/0x1403D9BE0
SE3COMBINABLESOUNDLIST::~SE3COMBINABLESOUNDLIST() {
    SE3CombinableSoundKitPosition* position = this->m_positions.Head();

    while (position) {
        SE3CombinableSoundKitPosition* next = this->m_positions.Next(position);
        this->m_positions.UnlinkNode(position);
        s_CombinableSoundKitPositionPool.PutData(position, NULL, 0);
        position = next;
    }
}

// OFFSET: 0x0070B7B0/0x1403D9D00
void SE3SoundEmitter::UpdatePillPosition(C3Vector listener) {
    int32_t segments = this->m_positions.m_count - 1;
    float closest = -1.0f;

    for (int32_t i = 0; i < segments; i++) {
        C3Vector start = this->m_positions.m_data[i];
        C3Vector end = this->m_positions.m_data[i + 1];
        float dx = end.x - start.x;
        float dy = end.y - start.y;
        float dz = end.z - start.z;
        float lengthSquared = dz * dz + dy * dy + dx * dx;
        float scale = 1.0f / sqrt(lengthSquared);
        float nx = dx * scale;
        float ny = scale * dy;
        float nz = scale * dz;
        float t = (listener.z - start.z) * nz + (listener.y - start.y) * ny + (listener.x - start.x) * nx;
        C3Vector point;

        if (t > 0.0f) {
            if (t < sqrt(lengthSquared)) {
                float s = t / sqrt(nz * nz + ny * ny + nx * nx);
                point.x = s * nx + start.x;
                point.y = ny * s + start.y;
                point.z = nz * s + start.z;
            } else {
                point = end;
            }
        } else {
            point = start;
        }

        if (!i) {
            this->m_position = point;
            closest = (listener.z - this->m_position.z) * (listener.z - this->m_position.z) + (listener.y - this->m_position.y) * (listener.y - this->m_position.y) + (listener.x - this->m_position.x) * (listener.x - this->m_position.x);
            continue;
        }

        float distance = (listener.z - point.z) * (listener.z - point.z) + (listener.y - point.y) * (listener.y - point.y) + (listener.x - point.x) * (listener.x - point.x);

        if (closest > distance) {
            this->m_position = point;
            closest = distance;
        }
    }

    SE3::EmitterManagerSendMessage_Set3DPosition(this->m_hSoundEmitter, this->m_position);
}

// OFFSET: 0x0070BE10/0x1403DA7D0
void SE3::EmitterManagerDestroyAllEmitters() {
    for (SE3SoundChunk* chunk = s_soundChunkTable.Head(); chunk; chunk = s_soundChunkTable.Next(chunk)) {
        SE3SoundEmitter* emitter = chunk->m_emitters.Head();

        while (emitter) {
            SE3SoundEmitter* next = chunk->m_emitters.Next(emitter);
            SE3::EmitterMangerSendMessage_DestroyEmitter(emitter->m_hSoundEmitter);
            emitter = next;
        }
    }
}

// OFFSET: 0x0070BA50/0x1403DA120
void SE3::EmitterManagerInit(void (*callback)(const C3Vector&, uint32_t*, uint32_t*), uint32_t chunkSize, uint32_t chunkRadius) {
    _fpEmitterManagerPosToSoundChunkCoordsCallback = callback;
    s_chunkRadius = chunkRadius;
    s_chunkSize = chunkSize;
    s_soundChunkList = static_cast<uint32_t*>(SMemAlloc((2 * chunkRadius + 1) * (8 * chunkRadius + 4) + 4, __FILE__, __LINE__, 0));
    s_bEmitterManagerShutDown = 0;

    s_soundEmitterTable.InternalClear(0);
    s_soundChunkTable.InternalClear(0);
    s_createEmitterMessages.Clear();
    s_destroyEmitterMessages.Clear();
    s_markSoundChunkDirtyMessages.Clear();

    SE3::sm_pGameSystem->get3DListenerAttributes(0, reinterpret_cast<FMOD_VECTOR*>(&s_listenerPosition), NULL, NULL, NULL);
    _fpEmitterManagerPosToSoundChunkCoordsCallback(s_listenerPosition, &s_listenerChunkX, &s_listenerChunkY);
    SE3::EmitterManagerBuildSoundChunkList(s_listenerChunkX, s_listenerChunkY);
}

// OFFSET: 0x0070BB20/0x1403DA2F0
void SE3::EmitterManagerShutDown() {
    s_emitterManagerCritSect.Enter();
    s_bEmitterManagerShutDown = 1;
    s_emitterManagerCritSect.Leave();

    s_createEmitterMessagesCritSect.Enter();

    for (SE3CreateSoundEmitterMessage* message = s_createEmitterMessages.Head(); message; message = s_createEmitterMessages.Head()) {
        s_createEmitterMessages.UnlinkNode(message);
        message->~SE3CreateSoundEmitterMessage();
        s_createEmitterMessagePool.PutData(message, NULL, 0);
    }

    s_createEmitterMessagesCritSect.Leave();

    s_set3DPositionMessagesCritSect.Enter();

    for (SE3Set3DPositionMessage* message = s_set3DPositionMessages.Head(); message; message = s_set3DPositionMessages.Head()) {
        s_set3DPositionMessages.UnlinkNode(message);
        s_set3DPositionMessagePool.PutData(message, NULL, 0);
    }

    s_set3DPositionMessagesCritSect.Leave();

    s_destroyEmitterMessagesCritSect.Enter();

    for (SE3DestroySoundEmitterMessage* message = s_destroyEmitterMessages.Head(); message; message = s_destroyEmitterMessages.Head()) {
        s_destroyEmitterMessages.UnlinkNode(message);
        s_destroyEmitterMessagePool.PutData(message, NULL, 0);
    }

    s_destroyEmitterMessagesCritSect.Leave();

    s_markSoundChunkDirtyMessagesCritSect.Enter();

    for (SE3MarkSoundChunkDirtyMessage* message = s_markSoundChunkDirtyMessages.Head(); message; message = s_markSoundChunkDirtyMessages.Head()) {
        s_markSoundChunkDirtyMessages.UnlinkNode(message);
        s_markSoundChunkDirtyMessagePool.PutData(message, NULL, 0);
    }

    s_markSoundChunkDirtyMessagesCritSect.Leave();

    s_soundEmitterTable.InternalClear(0);
    s_soundChunkTable.InternalClear(0);
    s_createEmitterMessages.Clear();
    s_set3DPositionMessages.Clear();
    s_destroyEmitterMessages.Clear();
    s_markSoundChunkDirtyMessages.Clear();

    SMemFree(s_soundChunkList, __FILE__, __LINE__, 0);
}

// OFFSET: 0x00708B90/0x1403D6E30
int32_t SE3::EmitterManagerEmitterExists(uint32_t handle) {
    return handle != 0x7D007D00 && s_soundEmitterTable.Ptr(handle, s_lookupKey) != NULL;
}

// OFFSET: 0x00707AB0/0x1403D60B0
C3Vector SE3::GetPosition(uint32_t handle) {
    C3Vector position;
    position.x = 0.0f;
    position.y = 0.0f;
    position.z = 0.0f;

    SE3SOUNDKITOBJECT_LOOKUP* lookup = SE3::sm_SoundKitObjectTable.Ptr(handle, s_lookupKey);

    if (lookup) {
        position = lookup->m_pSKO->m_position;
    }

    return position;
}

// OFFSET: 0x00709BB0/0x1403D6840
void SE3::FadeInNow(uint32_t handle) {
    if (!handle) {
        return;
    }

    SE3SOUNDKITOBJECT_LOOKUP* lookup = SE3::sm_SoundKitObjectTable.Ptr(handle, s_lookupKey);

    if (!lookup) {
        return;
    }

    SE3SoundKitObject* sko = lookup->m_pSKO;

    if (sko->m_fadeVolume < 1.0f && sko->m_fadeState != SE3FADE_IN) {
        SE3::SoundKitObjectLog(sko, __FILE__, __LINE__, "Sound Handle [%d]: Fade In!", sko->m_uUID);
        ::FadeInNow(sko, __FILE__, __LINE__, "SE3::FadeInNow");
    }
}

// OFFSET: 0x00707FD0/0x1403D6900
int32_t SE3::GetSoundKitID(uint32_t handle) {
    SE3SOUNDKITOBJECT_LOOKUP* lookup = SE3::sm_SoundKitObjectTable.Ptr(handle, s_lookupKey);
    SE3SoundKitObject* sko;
    SE3SoundKitDef* def;

    if (lookup && (sko = lookup->m_pSKO) != NULL && (def = sko->m_pSoundKitDef) != NULL && def->m_pSoundKitRec) {
        return def->m_pSoundKitRec->m_id;
    }

    return 0;
}

// OFFSET: 0x00708170/0x1403D6970
int32_t SE3::StopSound(uint32_t handle, int32_t fadeOutTime, const char* file, int32_t line, const char* function) {
    if (!handle) {
        return 1;
    }

    SE3SOUNDKITOBJECT_LOOKUP* lookup = SE3::sm_SoundKitObjectTable.Ptr(handle, s_lookupKey);

    if (!lookup) {
        return 1;
    }

    SE3SoundKitObject* sko = lookup->m_pSKO;

    if (!sko) {
        return 1;
    }

    if (sko->m_link.m_next) {
        SE3::StopOrFadeOut(sko, fadeOutTime, file, line, function);
        return 1;
    }

    if (sko->m_loadingLink.m_next || sko->m_downloadLink.m_next) {
        sm_SoundKitObjects_Loading.UnlinkNode(sko);
        sm_SoundKitObjects_WaitingForDownloadList.UnlinkNode(sko);
        SE3::SoundKitObjectLog(sko, __FILE__, __LINE__, "Unlink from sm_SoundKitObjects_Loading, Link to sm_SoundKitObjects_DeleteList");
    } else {
        if (!sko->m_goGoGoLink.m_next) {
            return 0;
        }

        sm_SoundKitObjects_GoGoGo.UnlinkNode(sko);
        SE3::SoundKitObjectLog(sko, __FILE__, __LINE__, "Unlink from sm_SoundKitObjects_GoGoGo, Link to sm_SoundKitObjects_DeleteList");
    }

    sm_SoundKitObjects_DeleteList.LinkToTail(sko);

    return 1;
}

// OFFSET: 0x007083A0/0x1403D6B30
void SE3::SetPlayFinishedCallback(uint32_t handle, void (*callback)(SE3SoundKitObject*), void* param) {
    SE3SOUNDKITOBJECT_LOOKUP* lookup = SE3::sm_SoundKitObjectTable.Ptr(handle, s_lookupKey);

    if (lookup && lookup->m_pSKO) {
        lookup->m_pSKO->m_fpPlayFinishedCallback = callback;
        lookup->m_pSKO->m_playFinishedParam = param;
    }
}

// OFFSET: 0x007083E0/0x1403D6BA0
int32_t SE3::IsLoopingOrPeriodic(uint32_t handle) {
    SE3SOUNDKITOBJECT_LOOKUP* lookup = SE3::sm_SoundKitObjectTable.Ptr(handle, s_lookupKey);

    if (lookup && lookup->m_pSKO && static_cast<uint32_t>(lookup->m_pSKO->m_usage) <= SE3USAGE_PERIODIC) {
        return 1;
    }

    return 0;
}

// OFFSET: 0x00708A30/0x1403D6C10
void SE3::Set3DPosition(uint32_t handle, const C3Vector& position) {
    if (!handle) {
        return;
    }

    SE3SOUNDKITOBJECT_LOOKUP* lookup = SE3::sm_SoundKitObjectTable.Ptr(handle, s_lookupKey);

    if (lookup && lookup->m_pSKO) {
        lookup->m_pSKO->Set3DPosition(position);
    }
}

// OFFSET: 0x00708BC0/0x1403D6E90
int32_t SE3::IsStoppable(uint32_t handle) {
    if (!handle) {
        return 0;
    }

    SE3SOUNDKITOBJECT_LOOKUP* lookup = SE3::sm_SoundKitObjectTable.Ptr(handle, s_lookupKey);
    SE3SoundKitObject* sko;
    SE3SoundKitDef* def;
    const SoundEntriesRec* rec;

    if (!lookup || (sko = lookup->m_pSKO) == NULL || (def = sko->m_pSoundKitDef) == NULL || (rec = def->m_pSoundKitRec) == NULL || !(rec->m_flags & 0x1000)) {
        return 1;
    }

    return 0;
}

// OFFSET: 0x00708A70/0x1403D6C80
void SE3::SetFrequencyScale(uint32_t handle, float scale) {
    if (!handle) {
        return;
    }

    SE3SOUNDKITOBJECT_LOOKUP* lookup = SE3::sm_SoundKitObjectTable.Ptr(handle, s_lookupKey);

    if (!lookup || !lookup->m_pSKO) {
        return;
    }

    lookup->m_pSKO->m_frequencyScale = scale;

    for (int32_t i = 0; i < 2; i++) {
        SE3SoundChannel* channel = &lookup->m_pSKO->m_channels[i];

        if (channel->m_pFMODChannel) {
            if (channel->m_baseFrequency == 0.0f) {
                channel->m_pFMODChannel->getFrequency(&channel->m_baseFrequency);
            }

            channel->m_pFMODChannel->setFrequency(scale * channel->m_baseFrequency);
        }
    }
}

// OFFSET: 0x00709490/0x1403D7D20
void SE3::KillVirtualSounds(int32_t elapsed) {
    SE3SoundKitObject* sko = SE3::sm_SoundKitObjects.Head();

    while (sko) {
        SE3SoundKitObject* next = SE3::sm_SoundKitObjects.Next(sko);

        if (!sko->m_properties.m_bit0) {
            SE3USAGE usage = sko->m_usage;

            if ((usage == SE3USAGE_ONESHOT || usage == SE3USAGE_PERIODIC) && sko->IsPlaying() && sko->IsVirtual()) {
                sko->m_virtualTime += elapsed;

                if (sko->m_virtualTime > 400) {
                    SE3::StopSound(sko->m_uUID, 0, __FILE__, __LINE__, "SE3::KillVirtualSounds");
                }
            }
        }

        sko = next;
    }
}

// OFFSET: 0x00707840/0x1403D5F50
SE3SoundEmitter::SE3SoundEmitter() {
    this->m_userData = 0;
    this->m_hSoundEmitter = 0;
    this->m_uSoundKitObjectUID = 0;
    this->m_pParentSoundChunk = NULL;
    this->m_nSoundKitID = 0;
    this->m_position.x = 0.0f;
    this->m_position.y = 0.0f;
    this->m_position.z = 0.0f;
    this->m_positions.Clear();
    this->m_properties.ResetToDefaults();
}

// OFFSET: 0x00709360/0x1403D71E0
SE3CreateSoundEmitterMessage::SE3CreateSoundEmitterMessage() {
    this->m_userData = 0;
    this->m_hSoundEmitter = 0;
    this->m_pParentSoundChunk = NULL;
    this->m_nSoundKitID = 0;
    this->m_position.x = 0.0f;
    this->m_position.y = 0.0f;
    this->m_position.z = 0.0f;
    this->m_positions.Clear();
    this->m_properties.ResetToDefaults();
}

// OFFSET: 0x00709400/0x1403D72A0
SE3CreateSoundEmitterMessage::~SE3CreateSoundEmitterMessage() {
}

// INLINED
// OFFSET: 0x00708B10/0x1403D6D90
SE3SoundEmitter::~SE3SoundEmitter() {
    SE3::StopSound(this->m_uSoundKitObjectUID, 500, __FILE__, __LINE__, "Sound Emitter Destructor, Stop Sound");
}

// OFFSET: 0x00709C20/0x1403D8540
SE3SoundChunk::~SE3SoundChunk() {
    this->m_emitters.Clear();
}

// OFFSET: 0x0070E650/0x1403DEB30
void SE3::CreateSoundEmitter(uint32_t* handle, const SoundEmittersRec* rec, SE3SoundKitProperties* properties) {
    TSGrowableArray<C3Vector> positions;
    uint32_t count = 0;

    if (rec->m_emitterType == 1) {
        SoundEmitterPillPointsRec key;
        key.m_soundEmittersID = rec->m_id;
        key.m_position.x = 0.0f;
        key.m_position.y = 0.0f;
        key.m_position.z = 0.0f;

        SoundEmitterPillPointsRec* records = g_soundEmitterPillPointsDB.GetRecordByIndex(0);
        int32_t numRecords = g_soundEmitterPillPointsDB.m_numRecords;
        SoundEmitterPillPointsRec* point = static_cast<SoundEmitterPillPointsRec*>(bsearch(&key, records, numRecords, sizeof(SoundEmitterPillPointsRec), ComparePillPoints));

        if (!point) {
            return;
        }

        while (point >= records && point->m_soundEmittersID == rec->m_id) {
            point--;
        }

        for (point++; point; point++) {
            if (point->m_soundEmittersID != rec->m_id || point >= records + numRecords) {
                break;
            }

            positions.GrowToFit(count, 0);
            positions.m_data[count++] = point->m_position;
        }
    }

    uint32_t x = 0;
    uint32_t y = 0;
    _fpEmitterManagerPosToSoundChunkCoordsCallback(rec->m_position, &x, &y);
    SE3SoundChunk* chunk = SE3::EmitterManagerGetOrCreateSoundChunk(y | (x << 16));
    SE3::EmitterManagerSendMessage_CreateEmitter(handle, chunk, rec->m_soundEntriesID, rec->m_position, properties, &positions, rec->m_id);
}

// OFFSET: 0x0070E7A0/0x1403DEF20
void SE3::InsertSoundKitDef(const SoundEntriesRec* rec, const SoundEntriesAdvancedRec* advancedRec, int32_t ownsRecords) {
    if (!rec) {
        return;
    }

    if (static_cast<uint32_t>(rec->m_id) >= sm_SoundKitDefArray.Count()) {
        sm_SoundKitDefArray.GrowToFit(rec->m_id, 1);
    }

    SE3SoundKitDef* def = new (__FILE__, __LINE__) SE3SoundKitDef();
    def->m_ownsRecords = ownsRecords;
    def->m_pSoundKitRec = rec;
    def->m_pAdvancedSoundKitRec = advancedRec;

    if (advancedRec) {
        def->m_timeOfDay[0] = advancedRec->m_timeA;
        def->m_timeOfDay[1] = advancedRec->m_timeB;
        def->m_timeOfDay[2] = advancedRec->m_timeC;
        def->m_timeOfDay[3] = advancedRec->m_timeD;

        if (def->m_timeOfDay[0] || def->m_timeOfDay[1] || def->m_timeOfDay[2] || def->m_timeOfDay[3]) {
            def->m_hasTimeOfDay = 1;
        }

        if (def->m_timeOfDay[1] < def->m_timeOfDay[0]) {
            def->m_timeOfDay[1] += s_millisecondsPerDay;
        }

        if (def->m_timeOfDay[2] < def->m_timeOfDay[1]) {
            def->m_timeOfDay[2] += s_millisecondsPerDay;
        }

        if (def->m_timeOfDay[3] < def->m_timeOfDay[2]) {
            def->m_timeOfDay[3] += s_millisecondsPerDay;
        }
    }

    sm_SoundKitDefArray[rec->m_id] = def;
    def->m_nNumFiles = 0;

    for (int32_t i = 0; i < 10; i++) {
        if (!rec->m_file[i] || !*rec->m_file[i]) {
            break;
        }

        def->m_nNumFiles = def->m_nNumFiles + 1;
    }

    SOUNDKITLOOKUP* lookup = s_soundKitLookup.Ptr(rec->m_name);

    if (!lookup) {
        lookup = s_soundKitLookup.New(rec->m_name, 0, 0);
    }

    lookup->m_nSoundKitID = rec->m_id;

    if (rec->m_flags & 0x200) {
        SE3::sm_pGameSystem->createSoundGroup(rec->m_name, &def->m_pSoundGroup);
        def->m_pSoundGroup->setMaxAudible(3);
        def->m_pSoundGroup->setMaxAudibleBehavior(FMOD_SOUNDGROUP_BEHAVIOR_MUTE);
        def->m_pSoundGroup->setMuteFadeSpeed(0.5f);
    } else {
        SE3::sm_pGameSystem->createSoundGroup(rec->m_name, &def->m_pSoundGroup);
        def->m_pSoundGroup->setMaxAudible(3);
        def->m_pSoundGroup->setMaxAudibleBehavior(FMOD_SOUNDGROUP_BEHAVIOR_STEALLOWEST);
    }
}

// OFFSET: 0x0070E9C0/0x1403DD4B0
SE3SoundKitObject* SE3::CreateNewSoundKitObject(uint32_t* handle, SE3SoundKitProperties* properties) {
    uint32_t uid = SE3::sm_uNextUniqueID + 1;
    SE3SOUNDKITOBJECT_LOOKUP* lookup = SE3::sm_SoundKitObjectTable.New(uid, s_lookupKey, 0, 0);

    lookup->m_pSKO = new (__FILE__, __LINE__) SE3SoundKitObject();
    memcpy(&lookup->m_pSKO->m_properties, properties, sizeof(SE3SoundKitProperties));

    if (handle) {
        *handle = lookup->m_pSKO->m_uUID;
    }

    lookup->m_pSKO->m_pMySKOL = lookup;

    return lookup->m_pSKO;
}

// OFFSET: 0x0070D940/0x1403DD640
SE3SoundChunk* SE3::EmitterManagerGetOrCreateSoundChunk(uint32_t handle) {
    SE3SoundChunk* chunk = s_soundChunkTable.Ptr(handle, s_lookupKey);

    if (!chunk) {
        chunk = s_soundChunkTable.New(handle, s_lookupKey, 0, 0);
        chunk->m_handle = handle;
    }

    return chunk;
}

// OFFSET: 0x0070D9D0/0x1403DD830
void SE3::EmitterManagerProcessEmitterMessages() {
    TSList<SE3CreateSoundEmitterMessage, TSGetLink<SE3CreateSoundEmitterMessage> > createMessages;

    s_createEmitterMessagesCritSect.Enter();

    for (SE3CreateSoundEmitterMessage* message = s_createEmitterMessages.Head(); message; message = s_createEmitterMessages.Head()) {
        s_createEmitterMessages.UnlinkNode(message);
        createMessages.LinkToTail(message);
    }

    s_createEmitterMessagesCritSect.Leave();

    SE3CreateSoundEmitterMessage* createMessage = createMessages.Head();

    while (createMessage) {
        SE3CreateSoundEmitterMessage* next = createMessages.Next(createMessage);
        int32_t soundKitID = createMessage->m_nSoundKitID;
        SE3SoundKitDef* def;

        s_bListenerChunkDirty = 1;

        if (soundKitID < static_cast<int32_t>(sm_SoundKitDefArray.Count()) && soundKitID > 0 && (def = sm_SoundKitDefArray[soundKitID]) != NULL) {
            SE3::EmitterManagerSendMessage_MarkSoundChunkDirty(createMessage->m_pParentSoundChunk);

            if (def->m_pSoundKitRec->m_distanceCutoff > static_cast<double>(s_chunkRadius * s_chunkSize) * 33.333332) {
                createMessage->m_pParentSoundChunk = SE3::EmitterManagerGetOrCreateSoundChunk(0x75307530);
                SE3::EmitterManagerSendMessage_MarkSoundChunkDirty(createMessage->m_pParentSoundChunk);
            }

            SE3SoundEmitter* emitter = new (__FILE__, __LINE__) SE3SoundEmitter();
            emitter->m_hSoundEmitter = createMessage->m_hSoundEmitter;
            emitter->m_pParentSoundChunk = createMessage->m_pParentSoundChunk;
            emitter->m_nSoundKitID = createMessage->m_nSoundKitID;
            emitter->m_position = createMessage->m_position;
            memcpy(&emitter->m_properties, &createMessage->m_properties, sizeof(SE3SoundKitProperties));
            uint32_t count = createMessage->m_positions.m_count;
            emitter->m_positions.SetCount(count);

            for (uint32_t i = 0; i < count; i++) {
                emitter->m_positions.m_data[i] = createMessage->m_positions.m_data[i];
            }

            emitter->m_userData = createMessage->m_userData;

            if (emitter->m_positions.m_count < 2) {
                emitter->m_pParentSoundChunk->m_emitters.LinkToTail(emitter);
            } else {
                emitter->m_pParentSoundChunk->m_emitters.LinkToHead(emitter);
            }

            uint32_t emitterHandle = createMessage->m_hSoundEmitter;
            SE3SoundEmitterLookup* lookup = s_soundEmitterTable.New(emitterHandle, s_lookupKey, 0, 0);
            lookup->m_pSoundEmitter = emitter;
        }

        createMessage->~SE3CreateSoundEmitterMessage();
        s_createEmitterMessagePool.PutData(createMessage, NULL, 0);
        createMessage = next;
    }

    TSList<SE3Set3DPositionMessage, TSGetLink<SE3Set3DPositionMessage> > positionMessages;

    s_set3DPositionMessagesCritSect.Enter();

    for (SE3Set3DPositionMessage* message = s_set3DPositionMessages.Head(); message; message = s_set3DPositionMessages.Head()) {
        s_set3DPositionMessages.UnlinkNode(message);
        positionMessages.LinkToTail(message);
    }

    s_set3DPositionMessagesCritSect.Leave();

    SE3Set3DPositionMessage* positionMessage = positionMessages.Head();

    while (positionMessage) {
        SE3Set3DPositionMessage* next = positionMessages.Next(positionMessage);
        SE3SoundEmitterLookup* lookup = s_soundEmitterTable.Ptr(positionMessage->m_hSoundEmitter, s_lookupKey);

        if (lookup) {
            SE3SoundChunk* chunk = lookup->m_pSoundEmitter->m_pParentSoundChunk;
            uint32_t oldHandle = chunk->m_handle;
            uint32_t newHandle = SE3::EmitterManagerGetSoundChunkHandle(positionMessage->m_position);

            SE3::EmitterManagerSendMessage_MarkSoundChunkDirty(chunk);

            if (oldHandle != newHandle) {
                chunk->m_emitters.UnlinkNode(lookup->m_pSoundEmitter);

                SE3SoundChunk* newChunk = SE3::EmitterManagerGetOrCreateSoundChunk(newHandle);

                if (lookup->m_pSoundEmitter->m_positions.m_count < 2) {
                    newChunk->m_emitters.LinkToTail(lookup->m_pSoundEmitter);
                } else {
                    newChunk->m_emitters.LinkToHead(lookup->m_pSoundEmitter);
                }

                lookup->m_pSoundEmitter->m_pParentSoundChunk = newChunk;
                SE3::EmitterManagerSendMessage_MarkSoundChunkDirty(newChunk);
            }

            lookup->m_pSoundEmitter->m_position = positionMessage->m_position;
            uint32_t uid = lookup->m_pSoundEmitter->m_uSoundKitObjectUID;
            s_bListenerChunkDirty = 1;

            if (uid) {
                SE3SOUNDKITOBJECT_LOOKUP* skoLookup = SE3::sm_SoundKitObjectTable.Ptr(uid, s_lookupKey);

                if (skoLookup && skoLookup->m_pSKO) {
                    skoLookup->m_pSKO->Set3DPosition(positionMessage->m_position);
                }
            }
        }

        positionMessages.UnlinkNode(positionMessage);
        s_set3DPositionMessagePool.PutData(positionMessage, NULL, 0);
        positionMessage = next;
    }

    TSList<SE3DestroySoundEmitterMessage, TSGetLink<SE3DestroySoundEmitterMessage> > destroyMessages;

    s_destroyEmitterMessagesCritSect.Enter();

    for (SE3DestroySoundEmitterMessage* message = s_destroyEmitterMessages.Head(); message; message = s_destroyEmitterMessages.Head()) {
        s_destroyEmitterMessages.UnlinkNode(message);
        destroyMessages.LinkToTail(message);
    }

    s_destroyEmitterMessagesCritSect.Leave();

    SE3DestroySoundEmitterMessage* destroyMessage = destroyMessages.Head();

    while (destroyMessage) {
        SE3DestroySoundEmitterMessage* next = destroyMessages.Next(destroyMessage);
        s_bListenerChunkDirty = 1;

        SE3SoundEmitterLookup* lookup = s_soundEmitterTable.Ptr(destroyMessage->m_hSoundEmitter, s_lookupKey);

        if (lookup) {
            SE3::EmitterManagerSendMessage_MarkSoundChunkDirty(lookup->m_pSoundEmitter->m_pParentSoundChunk);

            if (lookup->m_pSoundEmitter) {
                delete lookup->m_pSoundEmitter;
            }

            delete lookup;
        }

        destroyMessages.UnlinkNode(destroyMessage);
        s_destroyEmitterMessagePool.PutData(destroyMessage, NULL, 0);
        destroyMessage = next;
    }

    TSList<SE3MarkSoundChunkDirtyMessage, TSGetLink<SE3MarkSoundChunkDirtyMessage> > dirtyMessages;

    s_markSoundChunkDirtyMessagesCritSect.Enter();

    for (SE3MarkSoundChunkDirtyMessage* message = s_markSoundChunkDirtyMessages.Head(); message; message = s_markSoundChunkDirtyMessages.Head()) {
        s_markSoundChunkDirtyMessages.UnlinkNode(message);
        dirtyMessages.LinkToTail(message);
    }

    s_markSoundChunkDirtyMessagesCritSect.Leave();

    SE3MarkSoundChunkDirtyMessage* dirtyMessage = dirtyMessages.Head();

    while (dirtyMessage) {
        SE3MarkSoundChunkDirtyMessage* next = dirtyMessages.Next(dirtyMessage);
        SE3SoundChunk* chunk = dirtyMessage->m_pSoundChunk;

        if (chunk) {
            s_dirtySoundChunks.UnlinkNode(chunk);
            s_dirtySoundChunks.LinkToTail(chunk);
        } else {
            s_dirtySoundChunks.LinkToTail(chunk);
        }

        dirtyMessages.UnlinkNode(dirtyMessage);
        s_markSoundChunkDirtyMessagePool.PutData(dirtyMessage, NULL, 0);
        dirtyMessage = next;
    }

    SE3SoundChunk* chunk = s_dirtySoundChunks.Head();

    while (chunk) {
        SE3SoundChunk* next = s_dirtySoundChunks.Next(chunk);
        SE3SoundEmitter* emitter = chunk->m_emitters.Head();

        if (!emitter) {
            delete chunk;
            chunk = next;
            continue;
        }

        int32_t first = 1;

        for (; emitter; emitter = chunk->m_emitters.Next(emitter)) {
            int32_t soundKitID = emitter->m_nSoundKitID;
            SE3SoundKitDef* def;

            if (soundKitID >= static_cast<int32_t>(sm_SoundKitDefArray.Count()) || soundKitID <= 0 || (def = sm_SoundKitDefArray[soundKitID]) == NULL) {
                continue;
            }

            float cutoff = def->m_pSoundKitRec->m_distanceCutoff;
            float minX = emitter->m_position.x - cutoff;

            if (first) {
                chunk->m_minX = minX;
                chunk->m_maxX = cutoff + emitter->m_position.x;
                chunk->m_minY = emitter->m_position.y - cutoff;
                chunk->m_maxY = cutoff + emitter->m_position.y;
                first = 0;
                continue;
            }

            if (minX > chunk->m_minX) {
                minX = chunk->m_minX;
            }

            chunk->m_minX = minX;

            float maxX = emitter->m_position.x + cutoff;

            if (chunk->m_maxX > maxX) {
                maxX = chunk->m_maxX;
            }

            chunk->m_maxX = maxX;

            float minY = emitter->m_position.y - cutoff;

            if (minY > chunk->m_minY) {
                minY = chunk->m_minY;
            }

            chunk->m_minY = minY;

            float maxY = emitter->m_position.y + cutoff;

            if (chunk->m_maxY > maxY) {
                maxY = chunk->m_maxY;
            }

            chunk->m_maxY = maxY;
        }

        s_dirtySoundChunks.UnlinkNode(chunk);
        chunk = next;
    }
}

// OFFSET: 0x0070EA90/0x1403E1900
void SE3::InitSoundKitDefList() {
    SE3::InitSoundKitDefArray(g_soundEntriesDB.m_maxID);

    for (int32_t i = 0; i < g_soundEntriesDB.m_numRecords; i++) {
        SoundEntriesRec* rec = g_soundEntriesDB.GetRecordByIndex(i);

        if (rec) {
            SE3::InsertSoundKitDef(rec, g_soundEntriesAdvancedDB.GetRecord(rec->m_soundEntriesAdvancedID), 0);
        }
    }
}

// OFFSET: 0x0070EB20/0x1403DED00
int32_t SE3SoundKitDef::BeginDownload(const char* filename, int32_t index, uint32_t* handle, SE3SoundKitProperties* properties, const C3Vector* position) {
    SE3SoundKitObject* sko = SE3::CreateNewSoundKitObject(handle, properties);
    sko->m_pSoundKitDef = this;

    if (this->m_pAdvancedSoundKitRec) {
        int32_t range = this->m_pAdvancedSoundKitRec->m_randomOffsetRange;
        sko->m_timeOfDayOffset = range > -range ? CRandom::dice_(2 * range, g_rndSeed) - range : range;
    }

    sko->m_properties.m_nIndexOverride = index;

    if (position) {
        sko->m_hasPosition = 1;
        sko->m_position = *position;
    } else {
        sko->m_hasPosition = 0;
        sko->m_position.x = 0.0f;
        sko->m_position.y = 0.0f;
        sko->m_position.z = 0.0f;
    }

    sko->m_modeFlags = SE3::GetModeFlags(position, this, properties);
    SE3::SoundKitObjectLog(sko, __FILE__, __LINE__, "Created for download and play ASAP. Linking to sm_SoundKitObjects_WaitingForDownloadList");
    sm_SoundKitObjects_WaitingForDownloadList.LinkToTail(sko);

    if (this->m_pAsyncObj) {
        return SE3_OK;
    }

    uint8_t priority = (!properties->m_highPriorityDownload) | 2;
    this->m_downloadStartTime = OsGetAsyncTimeMs();

    if (AsyncFileReadCreateObject(filename, priority, 1, 0, &this->m_pAsyncObj, NULL, sko->m_pSoundKitDef, SoundKitDefDownloadCancelled, SoundKitDefDownloadFailed, 0, 0, 0)) {
        return SE3_OK;
    }

    *handle = 0;
    delete sko;
    return SE3_ERR_FILE;
}

// OFFSET: 0x0070EC80/0x1403DF430
int32_t SE3::PlaySoundKit(int32_t soundKitID, const C3Vector* position, SE3SoundKitProperties* properties, uint32_t* handle, const char* file, int32_t line, const char* function, int32_t streamed, const uint8_t* key) {
    if (soundKitID <= 0 && soundKitID != -128) {
        return SE3_ERR_INVALID_KIT;
    }

    if (!SE3::sm_pGameSystem || SE3::sm_bShuttingDown) {
        return SE3_ERR_NOT_INITIALIZED;
    }

    if (SE3::sm_eGameSystemOutputType == FMOD_OUTPUTTYPE_NOSOUND) {
        return SE3_ERR_SILENT;
    }

    if (properties && properties->m_combinable) {
        SE3COMBINABLESOUNDLIST* list = sm_CombinableSoundsTable.Ptr(soundKitID, s_lookupKey);

        if (!list) {
            list = sm_CombinableSoundsTable.New(soundKitID, s_lookupKey, 0, 0);
        }

        list->m_nSoundKitID = soundKitID;
        memcpy(&list->m_properties, properties, sizeof(SE3SoundKitProperties));

        SE3CombinableSoundKitPosition* entry = s_CombinableSoundKitPositionPool.New<SE3CombinableSoundKitPosition>(0);

        if (position) {
            entry->m_hasPosition = 1;
            entry->m_position = *position;
            list->m_positions.LinkToTail(entry);
        } else {
            list->m_positions.LinkToHead(entry);
        }

        return SE3_OK;
    }

    SE3SoundKitProperties defaultProperties;

    if (!properties) {
        properties = &defaultProperties;
    }

    SE3SoundKitDef* def = NULL;

    if (soundKitID != -128) {
        def = SE3::GetSoundKitDef(soundKitID);

        if (!def) {
            return SE3_ERR_INVALID_KIT;
        }

        if (!def->m_nNumFiles) {
            return SE3_ERR_KIT_EMPTY;
        }
    }

    int32_t result = SE3::ShouldSoundPlay(def, properties, position);

    if (result) {
        return result;
    }

    int32_t behavior = 0;

    if (def && def->m_pSoundKitRec) {
        behavior = def->m_pSoundKitRec->unk_0088;
    }

    if (handle && *handle) {
        if (behavior) {
            properties->m_bus = SE3BUS_DIALOG;

            if (behavior != 1 && SE3::IsPlaying(*handle)) {
                return SE3_ERR_STILL_PLAYING;
            }

            SE3::StopSound(*handle, 500, __FILE__, __LINE__, "SE3::PlaySoundKit");
        }

        uint32_t previous = *handle;

        if (previous && properties->m_stopPrevious && SE3::IsLoopingOrPeriodic(previous)) {
            SE3::StopSound(previous, 500, __FILE__, __LINE__, "SE3::PlaySoundKit");
        }
    }

    int32_t index = 0;
    const char* filename = NULL;
    char path[260];

    if (def && soundKitID != -128) {
        def->SelectNextFile(properties, &index);
        def->BuildPath(path, sizeof(path), index);

        if (!streamed || !OsPathCheckExtension(path, "mp3") || SFile::IsPatchFile(path)) {
            streamed = 0;

            if (!def->IsLocal(index)) {
                if (!def->m_pAsyncObj) {
                    if (OsPathCheckExtension(path, "wav")) {
                        OsPathReplaceExtension(path, sizeof(path), path, "ogg");
                    }

                    def->BeginDownload(path, index, handle, properties, position);
                }

                def->m_notLocal = def->SelectLocalFile(&index) == 0;

                if (def->m_notLocal) {
                    return SE3_ERR_NOT_LOCAL;
                }
            }
        }

        def->BuildPath(path, sizeof(path), index);
        filename = path;
    } else {
        if (!properties->m_sFilename) {
            return SE3_ERR_FILE;
        }

        if (!OsPathCheckExtension(properties->m_sFilename, "mp3")) {
            streamed = 0;
        }

        char localPath[260];
        SStrCopy(localPath, properties->m_sFilename, sizeof(localPath));

        if (!streamed && !MyFileIsLocal(localPath)) {
            AsyncFileReadCreateObject(localPath, GetDownloadPriority(properties->m_bus), 0, 0, NULL, NULL, NULL, DefaultAsyncObjectCleanupCallback, DefaultAsyncObjectCleanupCallback, 0, 0, 0);
            return SE3_ERR_NOT_LOCAL;
        }

        SStrCopy(path, properties->m_sFilename, sizeof(path));
        filename = path;
    }

    if (!sm_fpFilenameTranslateCallback && !SFile::FileExists(path) && OsPathCheckExtension(path, "wav")) {
        OsPathReplaceExtension(path, sizeof(path), path, "ogg");
    }

    if (def) {
        def->m_pSoundGroup->setMaxAudibleBehavior(static_cast<FMOD_SOUNDGROUP_BEHAVIOR>(properties->m_maxAudibleBehavior));
        def->m_pSoundGroup->setMuteFadeSpeed(0.5f);
    }

    uint32_t modeFlags = SE3::GetModeFlags(position, def, properties);

    char cacheName[512];
    SStrPrintf(cacheName, sizeof(cacheName), "%s%s%s", filename, (modeFlags & FMOD_2D) ? "_2D_" : "_3D_", (modeFlags & FMOD_LOOP_NORMAL) ? "_LOOP_" : "_NOLOOP_");
    uint32_t hash = SStrHash(cacheName, 0, 0);
    uint32_t waveDataID = 0;

    SE3::sm_SoundCache_Loading_CritSec.Enter();

    int32_t inCache = SE3::FindAndReferenceWaveDataNodeInCache(hash, &waveDataID);

    if (inCache || SE3::FindAndReferenceWaveDataNodeLoadingInCache(hash, &waveDataID)) {
        SE3SoundKitObject* sko;

        if (properties->m_pSKOToReuse) {
            sko = properties->m_pSKOToReuse;
            SE3::SoundKitObjectLog(sko, file, line, "Re-Using Handle[%d] & SKO", sko->m_uUID);
            properties = &sko->m_properties;
        } else {
            sko = SE3::CreateNewSoundKitObject(handle, properties);
        }

        SE3::SoundKitObjectLog(sko, file, line, "PlaySoundKit(%d) - %s", soundKitID, function);
        sko->m_pSoundKitDef = def;
        sko->m_priority = properties->m_priority;
        SE3::SetNewSoundKitObjectProperties(sko, position, properties, modeFlags);
        OutWithTheOldInWithTheNew(sko, waveDataID);

        if (inCache) {
            SE3::SoundKitObjectLog(sko, __FILE__, __LINE__, "Data found in cache node %d, linking to SoundKitObjects_GoGoGo list", waveDataID);
            SE3::SoundKitObjectLog(sko, __FILE__, __LINE__, "Link to sm_SoundKitObjects_GoGoGo");
            sm_SoundKitObjects_GoGoGo.LinkToTail(sko);
        } else {
            SE3::SoundKitObjectLog(sko, __FILE__, __LINE__, "Data cache node %d is already loading, linking to SoundKitObjects_Loading list", waveDataID);
            SE3::SoundKitObjectLog(sko, __FILE__, __LINE__, "Link to sm_SoundKitObjects_Loading");
            sm_SoundKitObjects_Loading.LinkToTail(sko);
        }

        SE3::sm_SoundCache_Loading_CritSec.Leave();
        return SE3_OK;
    }

    SE3::sm_SoundCache_Loading_CritSec.Leave();

    if (sm_fpFilenameTranslateCallback) {
        sm_fpFilenameTranslateCallback(&filename);
    }

    SFileImpl* fileHandle = NULL;

    if (!SFile::OpenEx(NULL, filename, 1, &fileHandle)) {
        return SE3_ERR_FILE;
    }

    int32_t cacheable = 1;
    uint32_t size = SFileGetFileSize(fileHandle, NULL);

    if (OsPathCheckExtension(filename, "mp3")) {
        cacheable = 0;
    } else {
        if (OsPathCheckExtension(filename, "ogg")) {
            SFile::ConvertOggToWavInBackground(filename);
            size *= 12;
        }

        if (size > SE3::sm_uMaxCacheableSize) {
            cacheable = 0;
        }
    }

    SE3SoundKitObject* sko;

    if (properties->m_pSKOToReuse) {
        sko = properties->m_pSKOToReuse;
        SE3::SoundKitObjectLog(sko, file, line, "Re-Using Handle[%d] & SKO", sko->m_uUID);
        properties = &sko->m_properties;
    } else {
        sko = SE3::CreateNewSoundKitObject(handle, properties);
    }

    SE3::SoundKitObjectLog(sko, file, line, "PlaySoundKit(%d) - %s", soundKitID, function);
    sko->m_pSoundKitDef = def;

    if (def && def->m_pAdvancedSoundKitRec) {
        sko->m_timeOfDayOffset = RandomBetween(-def->m_pAdvancedSoundKitRec->m_randomOffsetRange, def->m_pAdvancedSoundKitRec->m_randomOffsetRange);
    }

    sko->m_notCacheable = cacheable == 0;
    sko->m_priority = properties->m_priority;
    SE3::SetNewSoundKitObjectProperties(sko, position, properties, modeFlags);

    if (cacheable) {
        SE3::SoundKitObjectLog(sko, __FILE__, __LINE__, "Not in cache, linking to SoundKitObjects_Loading list");
    } else {
        SE3::SoundKitObjectLog(sko, __FILE__, __LINE__, "Too big for cache, linking to SoundKitObjects_Loading list");

        if (properties->m_noStreaming) {
            SFileCloseFile(fileHandle);

            if (sko->m_pMySKOL) {
                delete sko->m_pMySKOL;
            }

            sko->m_pMySKOL = NULL;
            delete sko;

            return SE3_ERR_CONDITION;
        }
    }

    sm_SoundKitObjects_Loading.LinkToTail(sko);
    waveDataID = 0;

    SE3StreamInfo* streamInfo = NULL;

    if (streamed) {
        streamInfo = CreateStreamInfo(key, fileHandle);
    }

    int32_t error = SE3::CreateWaveData(hash, def ? def->m_pSoundGroup : NULL, modeFlags, sko->m_notCacheable, size, filename, fileHandle, &waveDataID, streamInfo);

    if (error) {
        sm_SoundKitObjects_DeleteList.LinkToTail(sko);
        return error != SE3_OK ? SE3_ERR_FMOD : SE3_OK;
    }

    SE3::SoundKitObjectLog(sko, __FILE__, __LINE__, "Created Wave Data Node ID %d", waveDataID);

    if (sko->m_notCacheable) {
        SE3::SoundKitObjectLog(sko, __FILE__, __LINE__, "**This is a streamed sound**");
    }

    OutWithTheOldInWithTheNew(sko, waveDataID);

    return error != SE3_OK ? SE3_ERR_FMOD : SE3_OK;
}

// OFFSET: 0x0070F630/0x1403E0390
void SE3::UpdateAdvancedProperties(SE3SoundKitObject* sko, uint32_t elapsed, int32_t retrigger) {
    SE3SoundKitDef* def = sko->m_pSoundKitDef;

    if (!def || !def->m_pAdvancedSoundKitRec) {
        return;
    }

    const SoundEntriesAdvancedRec* advancedRec = def->m_pAdvancedSoundKitRec;
    float innerRadius = advancedRec->m_innerRadius2d;
    float outerRadius = advancedRec->m_outerRadius2d;
    float outerRadiusOfInfluence = advancedRec->m_innerRadiusOfInfluence;

    C3Vector listener;
    listener.x = 0.0f;
    listener.y = 0.0f;
    listener.z = 0.0f;
    SE3::sm_pGameSystem->get3DListenerAttributes(0, reinterpret_cast<FMOD_VECTOR*>(&listener), NULL, NULL, NULL);

    advancedRec = sko->m_pSoundKitDef->m_pAdvancedSoundKitRec;
    float distanceSquared = (listener.z - sko->m_position.z) * (listener.z - sko->m_position.z) + (listener.y - sko->m_position.y) * (listener.y - sko->m_position.y) + (listener.x - sko->m_position.x) * (listener.x - sko->m_position.x);

    if ((advancedRec->m_duckToSfx < 1.0f || advancedRec->m_duckToMusic < 1.0f || advancedRec->m_duckToAmbience < 1.0f || advancedRec->m_duckToDialog < 1.0f) && ((sko->m_position.x == 0.0f && sko->m_position.y == 0.0f && sko->m_position.z == 0.0f) || outerRadiusOfInfluence * outerRadiusOfInfluence >= distanceSquared)) {
        SE3::RegisterSuppressor(sko);
    }

    if (outerRadius > 0.0f) {
        float panLevel;

        if (innerRadius * innerRadius < distanceSquared) {
            if (outerRadius * outerRadius < distanceSquared) {
                panLevel = 1.0f;
            } else {
                panLevel = (sqrt(distanceSquared) - innerRadius) / (outerRadius - innerRadius);
            }
        } else {
            panLevel = 0.0f;
        }

        for (int32_t i = 0; i < 2; i++) {
            if (sko->m_channels[i].m_pFMODChannel) {
                sko->m_channels[i].m_pFMODChannel->set3DPanLevel(panLevel);
            }
        }
    }

    if (sko->m_properties.m_coneOrientation.x != 0.0f || sko->m_properties.m_coneOrientation.y != 0.0f || sko->m_properties.m_coneOrientation.z != 0.0f) {
        advancedRec = sko->m_pSoundKitDef->m_pAdvancedSoundKitRec;

        if (advancedRec->m_insideAngle != 180.0f || advancedRec->m_outsideAngle != 270.0f || advancedRec->m_outsideVolume != 0.0f) {
            C3Vector& cone = sko->m_properties.m_coneOrientation;

            if (sqrt(cone.z * cone.z + cone.y * cone.y + cone.x * cone.x) >= 0.000099999997) {
                float x = cone.x;
                float y = cone.y;
                float z = cone.z;
                float invLength = static_cast<float>(1.0 / sqrt(x * x + y * y + z * z));
                cone.x = x * invLength;
                cone.y = y * invLength;
                cone.z = z * invLength;
            } else {
                cone.x = 0.0f;
                cone.y = 0.0f;
                cone.z = 1.0f;
            }

            C3Vector orientation;
            orientation.x = cone.x;
            orientation.y = cone.y;
            orientation.z = -cone.z;

            for (int32_t i = 0; i < 2; i++) {
                if (sko->m_channels[i].m_pFMODChannel) {
                    advancedRec = sko->m_pSoundKitDef->m_pAdvancedSoundKitRec;
                    sko->m_channels[i].m_pFMODChannel->set3DConeSettings(advancedRec->m_insideAngle, advancedRec->m_outsideAngle, advancedRec->m_outsideVolume * 0.0099999998f);
                    sko->m_channels[i].m_pFMODChannel->set3DConeOrientation(reinterpret_cast<FMOD_VECTOR*>(&orientation));
                }
            }
        }
    }

    def = sko->m_pSoundKitDef;

    if (def->m_pAdvancedSoundKitRec->m_timeIntervalMax && !sko->m_fadeState && retrigger) {
        sko->m_periodicTimer -= elapsed;

        if (sko->m_periodicTimer <= 0) {
            sko->m_periodicTimer = 0;

            if (static_cast<uint32_t>(sko->m_usage) <= SE3USAGE_PERIODIC) {
                sko->m_properties.m_retriggered = 1;
                sko->m_properties.m_pSKOToReuse = sko;
                sko->m_properties.m_fadeInTime = 0;

                int32_t error = SE3::PlaySoundKit(def->m_pSoundKitRec->m_id, &sko->m_position, &sko->m_properties, NULL, __FILE__, __LINE__, sko->m_usage == SE3USAGE_PERIODIC ? "Re-Trigger Periodic Sound" : "Re-Trigger Looping Sound");

                if (error) {
                    SE3::SoundKitObjectLog(sko, __FILE__, __LINE__, "Failed to re-trigger periodic/looping sound with SE3 Error %d", error);
                }
            }
        }
    }

    for (int32_t i = 0; i < 2; i++) {
        SE3SoundChannel* channel = &sko->m_channels[i];

        if (channel->m_pFMODChannel && channel->m_fadeState == SE3FADE_IN) {
            channel->m_fadeVolume = static_cast<float>(static_cast<double>(elapsed) * 0.001 + channel->m_fadeVolume);

            if (channel->m_fadeVolume > 1.0f) {
                SE3::SoundKitObjectLog(sko, __FILE__, __LINE__, "Sound Object [%d] Done Fading In\n", i);
                channel->m_fadeVolume = 1.0f;
                channel->m_fadeState = SE3FADE_NONE;
            }
        }

        if (channel->m_fadeState == SE3FADE_OUT) {
            channel->m_fadeVolume = static_cast<float>(channel->m_fadeVolume - static_cast<double>(elapsed) * 0.001);

            if (channel->m_fadeVolume < 0.0f) {
                SE3::SoundKitObjectLog(sko, __FILE__, __LINE__, "Sound Object [%d] Done Fading Out. Stopping.\n", i);
                channel->m_fadeState = SE3FADE_NONE;
                channel->m_fadeVolume = 0.0f;
                channel->m_pFMODChannel->stop();
            }
        }
    }

    if (!sm_fpGetDayProgression || !sko->m_pSoundKitDef->m_hasTimeOfDay) {
        return;
    }

    SE3SoundKitDef* timeDef = sko->m_pSoundKitDef;
    int32_t offset = sko->m_timeOfDayOffset;
    int32_t timeA = offset + timeDef->m_timeOfDay[0];
    int32_t timeD = offset + timeDef->m_timeOfDay[3];
    int32_t now = s_dayProgression;

    if (timeD >= s_millisecondsPerDay && now < timeA) {
        now += s_millisecondsPerDay;
    }

    if (timeA > now || timeD < now) {
        sko->m_fTimeOfDayScalar = 0.0f;
        return;
    }

    int32_t timeB = offset + timeDef->m_timeOfDay[1];
    int32_t timeC = offset + timeDef->m_timeOfDay[2];
    now = s_dayProgression;

    if (timeD >= s_millisecondsPerDay && now < timeA) {
        now += s_millisecondsPerDay;
    }

    if (now < timeB) {
        sko->m_fTimeOfDayScalar = static_cast<float>(now - timeA) / static_cast<float>(timeB - timeA);
    } else if (now < timeC) {
        sko->m_fTimeOfDayScalar = 1.0f;
    } else {
        sko->m_fTimeOfDayScalar = 1.0f - static_cast<float>(now - timeC) / static_cast<float>(timeD - timeC);
    }
}

// OFFSET: 0x0070FBE0/0x1403E09C0
void SE3::ProcessGoGoGoList() {
    SE3SoundKitObject* sko = sm_SoundKitObjects_GoGoGo.Head();

    while (sko) {
        SE3SoundKitObject* next = sm_SoundKitObjects_GoGoGo.Next(sko);

        if (sko->m_pSoundKitDef) {
            const SoundEntriesAdvancedRec* advancedRec = sko->m_pSoundKitDef->m_pAdvancedSoundKitRec;

            if (advancedRec && advancedRec->m_timeIntervalMax) {
                sko->m_periodicTimer = RandomBetween(advancedRec->m_timeIntervalMin, advancedRec->m_timeIntervalMax);
            }
        }

        if (sko->m_usage == SE3USAGE_PERIODIC && !sko->m_properties.m_retriggered) {
            sm_SoundKitObjects_GoGoGo.UnlinkNode(sko);
            SE3::SoundKitObjectLog(sko, __FILE__, __LINE__, "Periodic sound created (won't play until retrigger)");
            SE3::SoundKitObjectLog(sko, __FILE__, __LINE__, "Unlink from sm_SoundKitObjects_GoGoGo, Link to sm_SoundKitObjects");
            SE3::sm_SoundKitObjects.LinkToTail(sko);
            sko = next;
            continue;
        }

        int32_t i = 0;
        SE3WaveDataNode* node;

        while (!SE3::FindWaveDataNode(sko->m_channels[i].m_waveDataID, &node)) {
            if (++i >= 2) {
                break;
            }
        }

        if (i >= 2) {
            sko = next;
            continue;
        }

        SE3SoundChannel* channel = &sko->m_channels[i];

        if (node) {
            FMOD_RESULT result = SE3::sm_pGameSystem->playSound(FMOD_CHANNEL_FREE, node->m_pFMODSound, true, &channel->m_pFMODChannel);

            if (result != FMOD_OK) {
                SE3::SoundKitObjectLog(sko, __FILE__, __LINE__, "Failed to play (FMOD_RESULT: %d), unlinking from sm_SoundKitObjects_GoGoGo", result);
                sm_SoundKitObjects_GoGoGo.UnlinkNode(sko);
                SE3::SoundKitObjectLog(sko, __FILE__, __LINE__, "Unlink from sm_SoundKitObjects_GoGoGo, Link to sm_SoundKitObjects_DeleteList");
                sm_SoundKitObjects_DeleteList.LinkToTail(sko);
                sko = next;
                continue;
            }

            channel->m_pFMODChannel->setCallback(FMOD_CHANNEL_CALLBACKTYPE_END, SoundEndedCallback, 0);
        }

        FMOD::Channel* fmodChannel = channel->m_pFMODChannel;
        fmodChannel->setUserData(reinterpret_cast<void*>(sko->m_uUID));

        if (sko->m_modeFlags & FMOD_3D) {
            const SoundEntriesRec* rec = sko->m_pSoundKitDef->m_pSoundKitRec;
            float minDistance = rec->m_minDistance;
            float maxDistance = rec->m_distanceCutoff;

            if (minDistance < 0.0f) {
                minDistance = 0.0f;
            }

            if (minDistance > maxDistance) {
                maxDistance = minDistance;
            }

            channel->m_pFMODChannel->set3DMinMaxDistance(minDistance, maxDistance);

            if (sko->m_usage == SE3USAGE_PERIODIC) {
                sko->m_positionOffset.x = (static_cast<double>(CRandom::dice_(1001, g_rndSeed)) - 500.0) * 0.0020000001f;
                sko->m_positionOffset.y = (static_cast<double>(CRandom::dice_(1001, g_rndSeed)) - 500.0) * 0.0020000001f;
                sko->m_positionOffset.z = (static_cast<double>(CRandom::dice_(1001, g_rndSeed)) - 500.0) * 0.0020000001f;
                float x = sko->m_positionOffset.x;
                float y = sko->m_positionOffset.y;
                float z = sko->m_positionOffset.z;
                float lengthSquared = x * x + y * y + z * z;

                if (lengthSquared > 0.00000023841858f) {
                    float scale = 1.0f / sqrt(lengthSquared);
                    sko->m_positionOffset.x = x * scale;
                    sko->m_positionOffset.y = y * scale;
                    sko->m_positionOffset.z = scale * z;
                }

                const SoundEntriesAdvancedRec* advancedRec = sko->m_pSoundKitDef->m_pAdvancedSoundKitRec;
                int32_t range = 1000 * (advancedRec->m_randomOffsetMax - advancedRec->m_randomOffsetMin);
                int32_t minOffset = advancedRec->m_randomOffsetMin;
                float extra;

                if (range <= 0) {
                    extra = 0.0f;
                } else {
                    extra = CRandom::dice_(range, g_rndSeed) * 0.001;
                }

                x = sko->m_positionOffset.x;
                y = sko->m_positionOffset.y;
                z = sko->m_positionOffset.z;
                float scale = (extra + minOffset) / sqrt(x * x + y * y + z * z);
                sko->m_positionOffset.x = scale * x;
                sko->m_positionOffset.y = scale * y;
                sko->m_positionOffset.z = scale * z;
            }

            C3Vector position;
            position.x = sko->m_positionOffset.x + sko->m_position.x;
            position.y = sko->m_position.y + sko->m_positionOffset.y;
            position.z = sko->m_position.z + sko->m_positionOffset.z;
            fmodChannel->set3DAttributes(reinterpret_cast<FMOD_VECTOR*>(&position), NULL);
        }

        if (!SE3::IsBusEqualOrChild(sko->m_properties.m_bus, SE3BUS_MUSIC) && !sko->m_properties.m_noRandomStart && !sko->m_usage) {
            unsigned int length = 0;
            node->m_pFMODSound->getLength(&length, FMOD_TIMEUNIT_PCMBYTES);
            uint32_t random = CRandom::uint32_(g_rndSeed);
            channel->m_pFMODChannel->setPosition(static_cast<uint32_t>((static_cast<uint64_t>(length >> 1) * random) >> 32) & 0xFFFFFFF8, FMOD_TIMEUNIT_PCMBYTES);
        }

        float randomVolume = 1.0f;

        if (sko->m_pSoundKitDef) {
            const SoundEntriesRec* rec = sko->m_pSoundKitDef->m_pSoundKitRec;
            float range = static_cast<float>(fabs(rec->m_volumeVariationPlus) + fabs(rec->m_volumeVariationMinus));

            if (range > 0.0) {
                randomVolume = static_cast<float>(CRandom::dice_(static_cast<uint32_t>(range * 1000.0f), g_rndSeed) * 0.001 - fabs(sko->m_pSoundKitDef->m_pSoundKitRec->m_volumeVariationMinus) + 1.0);
            }
        }

        sko->m_randomVolume = randomVolume;
        fmodChannel->setVolume(sko->GetChannelVolume(i));

        if (sko->m_pSoundKitDef) {
            const SoundEntriesRec* rec = sko->m_pSoundKitDef->m_pSoundKitRec;
            float range = static_cast<float>(fabs(rec->m_pitchVariationPlus) + fabs(rec->m_pitchVariationMinus));

            if (range > 0.0) {
                float scale = static_cast<float>(CRandom::dice_(static_cast<uint32_t>(range * 1000.0f), g_rndSeed) * 0.001 - fabs(sko->m_pSoundKitDef->m_pSoundKitRec->m_pitchVariationMinus) + 1.0);
                SE3::SetFrequencyScale(sko->m_uUID, scale);
            }
        }

        if (sko->m_properties.m_fadeInTime > 0) {
            SE3::SoundKitObjectLog(sko, __FILE__, __LINE__, "Sound Handle [%d]: Fade In!", sko->m_uUID);
            ::FadeInNow(sko, __FILE__, __LINE__, "SE3::ProcessGoGoGoList");
        }

        SE3::UpdateAdvancedProperties(sko, 0, 0);

        if (sko->m_usage == SE3USAGE_PERIODIC && sko->m_pSoundKitDef && sko->m_pSoundKitDef->m_pAdvancedSoundKitRec) {
            sko->m_fpPlayFinishedCallback = PeriodicSoundFinishedCallback;
        }

        uint32_t priority = sko->m_priority;

        if (priority != 128 && priority <= 256 && fmodChannel && fmodChannel->setPriority(sko->m_priority) != FMOD_OK) {
            SE3::SoundKitObjectLog(sko, __FILE__, __LINE__, "Failed to set priority (WTF], unlinking from sm_SoundKitObjects_GoGoGo");
            sm_SoundKitObjects_GoGoGo.UnlinkNode(sko);
            SE3::SoundKitObjectLog(sko, __FILE__, __LINE__, "Unlink from sm_SoundKitObjects_GoGoGo, Link to sm_SoundKitObjects_DeleteList");
            sm_SoundKitObjects_DeleteList.LinkToTail(sko);
            return;
        }

        if (sko->m_fpCustomDSPChainCallback) {
            sko->m_fpCustomDSPChainCallback(fmodChannel, sko->m_sCustomDSPChainName);
        }

        SE3::SetReverbPropertiesAndSpeakerMix(sko, fmodChannel);

        if (fmodChannel->setPaused(false) != FMOD_OK) {
            SE3::SoundKitObjectLog(sko, __FILE__, __LINE__, "Failed to unPause (maybe out of channels?), unlinking from sm_SoundKitObjects_GoGoGo");
            sm_SoundKitObjects_GoGoGo.UnlinkNode(sko);
            SE3::SoundKitObjectLog(sko, __FILE__, __LINE__, "Unlink from sm_SoundKitObjects_GoGoGo, Link to sm_SoundKitObjects_DeleteList");
            sm_SoundKitObjects_DeleteList.LinkToTail(sko);
            return;
        }

        if (sko->m_pSoundKitDef) {
            sko->m_pSoundKitDef->m_nPlayingCount++;
        }

        SE3::SoundBusIncrementChannelCount(sko->m_properties.m_bus);
        sm_SoundKitObjects_GoGoGo.UnlinkNode(sko);
        SE3::SoundKitObjectLog(sko, __FILE__, __LINE__, "Unlink from sm_SoundKitObjects_GoGoGo, Link to sm_SoundKitObjects");
        SE3::sm_SoundKitObjects.LinkToTail(sko);
        sko = next;
    }
}

// OFFSET: 0x00710410/0x1403E14F0
void SE3::EmitterManagerHeartbeat(int32_t elapsed) {
    static int32_t s_timer;

    if (s_bEmitterManagerShutDown) {
        return;
    }

    s_timer -= elapsed;

    if (s_timer > 0) {
        return;
    }

    s_timer = 10;
    SE3::EmitterManagerProcessEmitterMessages();

    C3Vector listener;
    listener.x = 0.0f;
    listener.y = 0.0f;
    listener.z = 0.0f;
    SE3::sm_pGameSystem->get3DListenerAttributes(0, reinterpret_cast<FMOD_VECTOR*>(&listener), NULL, NULL, NULL);

    int32_t moved = listener.x != s_listenerPosition.x || listener.y != s_listenerPosition.y || listener.z != s_listenerPosition.z;

    if (!moved && !s_bListenerChunkDirty) {
        return;
    }

    uint32_t x = 0;
    uint32_t y = 0;
    _fpEmitterManagerPosToSoundChunkCoordsCallback(listener, &x, &y);
    SE3::EmitterManagerBuildSoundChunkList(x, y);

    s_listenerPosition = listener;
    s_listenerChunkX = x;
    s_listenerChunkY = y;
    s_bListenerChunkDirty = 0;

    uint32_t count = (2 * s_chunkRadius + 1) * (2 * s_chunkRadius + 1);

    if (count == 0xFFFFFFFF) {
        return;
    }

    for (uint32_t i = 0; i < count + 1; i++) {
        uint32_t handle = s_soundChunkList[i];

        if (handle == 0x7D007D00) {
            break;
        }

        SE3SoundChunk* chunk = s_soundChunkTable.Ptr(handle, s_lookupKey);

        if (!chunk) {
            continue;
        }

        if (moved) {
            for (SE3SoundEmitter* emitter = chunk->m_emitters.Head(); emitter; emitter = chunk->m_emitters.Next(emitter)) {
                if (emitter->m_positions.m_count < 2) {
                    break;
                }

                emitter->UpdatePillPosition(listener);
            }
        }

        if (s_soundChunkList[i] != 0x75307530 && (chunk->m_minX > listener.x || listener.x > chunk->m_maxX || chunk->m_minY > listener.y || listener.y > chunk->m_maxY)) {
            continue;
        }

        for (SE3SoundEmitter* emitter = chunk->m_emitters.Head(); emitter; emitter = chunk->m_emitters.Next(emitter)) {
            int32_t soundKitID = emitter->m_nSoundKitID;
            SE3SoundKitDef* def;

            if (soundKitID >= static_cast<int32_t>(sm_SoundKitDefArray.Count()) || soundKitID <= 0 || (def = sm_SoundKitDefArray[soundKitID]) == NULL) {
                continue;
            }

            float distanceSquared = 0.0f;

            if (!IsBeyondListenerRange(&emitter->m_position, def->m_pSoundKitRec->m_distanceCutoff + 25.0f, &distanceSquared) && !SE3::IsPlaying(emitter->m_uSoundKitObjectUID)) {
                SE3::PlaySoundKit(emitter->m_nSoundKitID, &emitter->m_position, &emitter->m_properties, &emitter->m_uSoundKitObjectUID, __FILE__, __LINE__, "SE3::EmitterManagerHeartbeat");
            }
        }
    }
}

// OFFSET: 0x00710740/0x1403E1990
static void UpdateSoundKitObjects(uint32_t elapsed) {
    if (sm_fpGetDayProgression) {
        s_dayProgression = sm_fpGetDayProgression();
    }

    SE3::UpdateSuppressors(elapsed);

    SE3SoundKitObject* sko = SE3::sm_SoundKitObjects.Head();

    while (sko) {
        SE3SoundKitObject* next = SE3::sm_SoundKitObjects.Next(sko);
        SE3::UpdateAdvancedProperties(sko, elapsed, 1);
        sko = next;
    }
}

// OFFSET: 0x00710790/0x1403E1A00
void SE3::ProcessDownloadList() {
    C3Vector listener;
    listener.x = 0.0f;
    listener.y = 0.0f;
    listener.z = 0.0f;
    SE3::sm_pGameSystem->get3DListenerAttributes(0, reinterpret_cast<FMOD_VECTOR*>(&listener), NULL, NULL, NULL);

    uint32_t now = OsGetAsyncTimeMs();
    int32_t closest = 0x7FFFFFFF;
    CAsyncObject* closestObject = NULL;

    SE3SoundKitObject* sko = sm_SoundKitObjects_WaitingForDownloadList.Head();

    while (sko) {
        SE3SoundKitDef* def = sko->m_pSoundKitDef;
        SE3SoundKitObject* next = sm_SoundKitObjects_WaitingForDownloadList.Next(sko);

        if (def->m_pAsyncObj) {
            float distance = 0.0f;

            if (sko->m_position.x != 0.0f || sko->m_position.y != 0.0f || sko->m_position.z != 0.0f) {
                distance = (sko->m_position.z - listener.z) * (sko->m_position.z - listener.z) + (sko->m_position.y - listener.y) * (sko->m_position.y - listener.y) + (sko->m_position.x - listener.x) * (sko->m_position.x - listener.x);
            } else if (SE3::IsBusEqualOrChild(sko->m_properties.m_bus, SE3BUS_MUSIC) || SE3::IsBusEqualOrChild(sko->m_properties.m_bus, SE3BUS_AMBIENCE)) {
                distance = 10000.0f;
            }

            int32_t priority = AsyncFileReadComputeSoundPriority(sko->m_properties.m_highPriorityDownload, distance);

            if (priority < closest) {
                closestObject = def->m_pAsyncObj;
                closest = priority;
            }
        } else if (def->m_downloaded && def->m_notLocal && (sko->m_properties.m_highPriorityDownload || now - def->m_downloadStartTime < 500)) {
            sko->m_properties.m_pSKOToReuse = sko;
            SE3::SoundKitObjectLog(sko, __FILE__, __LINE__, "Done downloading!");
            SE3::SoundKitObjectLog(sko, __FILE__, __LINE__, "Unlink from download list");
            sm_SoundKitObjects_WaitingForDownloadList.UnlinkNode(sko);
            SE3::PlaySoundKit(def->m_pSoundKitRec->m_id, sko->m_hasPosition ? &sko->m_position : NULL, &sko->m_properties, NULL, __FILE__, __LINE__, "SE3::ProcessDownloadList");
        } else {
            if (sko->m_pMySKOL) {
                delete sko->m_pMySKOL;
            }

            sko->m_pMySKOL = NULL;
            delete sko;
        }

        sko = next;
    }

    sm_SoundKitDefs_ClosestAsyncObject = closestObject;
    sm_SoundKitDefs_ClosestAsyncObjectSortVal = closest;
}

// OFFSET: 0x00710A60/0x1403E1D30
void SE3::ProcessCombinableSounds() {
    SE3COMBINABLESOUNDLIST* list = sm_CombinableSoundsTable.Head();

    if (!list) {
        return;
    }

    C3Vector listener;
    listener.x = 0.0f;
    listener.y = 0.0f;
    listener.z = 0.0f;
    SE3::sm_pGameSystem->get3DListenerAttributes(0, reinterpret_cast<FMOD_VECTOR*>(&listener), NULL, NULL, NULL);

    C3Vector position;
    position.x = 0.0f;
    position.y = 0.0f;
    position.z = 0.0f;

    do {
        SE3SoundKitDef* def;

        if (static_cast<uint32_t>(list->m_nSoundKitID) < sm_SoundKitDefArray.Count() && (def = sm_SoundKitDefArray[list->m_nSoundKitID]) != NULL) {
            float closest = FLT_MAX;
            int32_t allPositioned = 1;
            int32_t count = 0;

            for (SE3CombinableSoundKitPosition* entry = list->m_positions.Head(); entry; entry = list->m_positions.Next(entry)) {
                position = entry->m_position;
                count++;

                if (!entry->m_hasPosition) {
                    allPositioned = 0;
                }

                float distanceSquared = (listener.z - entry->m_position.z) * (listener.z - entry->m_position.z) + (listener.y - entry->m_position.y) * (listener.y - entry->m_position.y) + (listener.x - entry->m_position.x) * (listener.x - entry->m_position.x);

                if (closest > distanceSquared) {
                    closest = distanceSquared;
                }
            }

            list->m_properties.m_combinable = 0;

            if (count == 1) {
                SE3::PlaySoundKit(list->m_nSoundKitID, allPositioned ? &position : NULL, &list->m_properties, NULL, __FILE__, __LINE__, "SE3::ProcessCombinableSounds");
            } else {
                float volume = 1.0f;

                if (allPositioned) {
                    const SoundEntriesRec* rec = def->m_pSoundKitRec;
                    float maxDistance = rec->m_distanceCutoff;
                    float minDistance = rec->m_minDistance;
                    float distance = sqrt(closest);

                    if (distance < maxDistance) {
                        if (distance <= minDistance) {
                            volume = 1.0f;
                        } else {
                            volume = minDistance / ((distance - minDistance) * 4.0f + minDistance);

                            if (distance > maxDistance * 0.9f) {
                                volume = (1.0f - (distance - maxDistance * 0.9f) / (maxDistance - maxDistance * 0.9f)) * volume;
                            }
                        }
                    } else {
                        volume = 0.0f;
                    }
                }

                list->m_properties.m_volume = volume;
                SE3::PlaySoundKit(list->m_nSoundKitID, NULL, &list->m_properties, NULL, __FILE__, __LINE__, "SE3::ProcessCombinableSounds");
            }
        }

        sm_CombinableSoundsTable.Delete(list);
        list = sm_CombinableSoundsTable.Head();
    } while (list);
}

// OFFSET: 0x00710CB0/0x1403E2200
int32_t SE3::PlaySoundFile(const char* filename, uint32_t* handle, SE3BUS bus, SE3USAGE usage) {
    if (!filename) {
        return SE3_ERR_FILE;
    }

    SE3SoundKitProperties properties;
    properties.m_usage = usage;
    properties.m_sFilename = filename;
    properties.m_bus = bus;
    properties.m_bit10 = bus == SE3BUS_7;

    if (SE3::IsBusEqualOrChild(bus, SE3BUS_MUSIC)) {
        properties.m_reverb = 0;
    }

    return SE3::PlaySoundKit(-128, NULL, &properties, handle, __FILE__, __LINE__, "SE3::PlaySoundFile");
}

// OFFSET: 0x00710D70/0x1403E2390
int32_t SE3::PlaySoundKit(const char* name, const C3Vector* position, SE3SoundKitProperties* properties, uint32_t* handle, const char* file, int32_t line, const char* function) {
    SOUNDKITLOOKUP* lookup = s_soundKitLookup.Ptr(name);
    int32_t soundKitID;

    if (lookup && (soundKitID = lookup->m_nSoundKitID) != 0) {
        return SE3::PlaySoundKit(soundKitID, position, properties, handle, file, line, function);
    }

    return SE3_ERR_KIT_NAME_NOT_FOUND;
}

// OFFSET: 0x00710DC0/0x1403E2430
int32_t SE3::HeartBeat(const void* data, void* param) {
    static uint32_t s_lastTime = OsGetAsyncTimeMs();
    uint32_t now = OsGetAsyncTimeMs();
    int32_t elapsed = now - s_lastTime;
    s_lastTime = now;

    if (elapsed < 0) {
        elapsed = 0;
    }

    if (SE3::sm_pGameSystem) {
        SE3::sm_pGameSystem->update();
    }

    if (SE3::sm_pChatSystem) {
        SE3::sm_pChatSystem->update();
    }

    SE3::TrimCache();
    SE3::ReleaseUnusedStreams();
    SE3::ProcessCombinableSounds();
    SE3::ManageOutOfRangeSounds(elapsed);
    SE3::EmitterManagerHeartbeat(elapsed);
    SE3::ProcessDoneLoadingMessageQueue();
    SE3::ProcessDownloadList();
    SE3::ProcessGoGoGoList();
    SE3::ProcessFadeList(elapsed);
    SE3::ProcessMemorySoundEndedMessageQueue();
    SE3::ProcessSoundEndedMessageQueue();
    SE3::ProcessSoundKitObjectDeleteList();

    static int32_t s_lastDeleteTime = OsGetAsyncTimeMs();
    int32_t deleteTime = OsGetAsyncTimeMs();
    SE3::ProcessWaveDataNodeDeleteList(deleteTime - s_lastDeleteTime, 0);
    s_lastDeleteTime = deleteTime;

    UpdateSoundKitObjects(elapsed);
    SE3::UpdateVolumes();
    SE3::KillVirtualSounds(elapsed);

    return 1;
}

// OFFSET: 0x00710EB0/0x1403E2570
int32_t SE3::Init(SE3EngineInitParams* params) {
    SE3::sm_fFocusScalar = 1.0f;
    SE3::sm_bShuttingDown = 0;
    SE3::sm_SoundCache_Stats_Count = 0;
    SE3::sm_SoundCache_Stats_TotalSizeInBytes = 0;
    SStrCopy(SE3::sm_sSystemDefaultName, params->m_systemDefaultName, sizeof(SE3::sm_sSystemDefaultName));
    SE3::sm_eGameSystemOutputType = FMOD_OUTPUTTYPE_NOSOUND;
    SE3::sm_pGameSystem = NULL;
    sm_fpFilenameTranslateCallback = params->m_fpFilenameTranslateCallback;
    sm_fpStatTrackerUpdate = params->unk_0064;
    sm_fpGetDayProgression = params->m_fpGetDayProgression;
    s_millisecondsPerDay = params->m_millisecondsPerDay;

    FMOD_RESULT result = FMOD::Memory_Initialize(NULL, 0, FSoundAllocCallback, FSoundReallocCallback, FSoundFreeCallback);
    SE3::Log_Write(__LINE__, __FILE__, result, " - FMOD Memory Init");

    if (result != FMOD_OK && result != FMOD_ERR_INITIALIZED) {
        return SE3_ERR_FMOD;
    }

    result = FMOD::System_Create(&SE3::sm_pGameSystem);
    SE3::Log_Write(__LINE__, __FILE__, result, " - FMOD System Create");

    if (result != FMOD_OK) {
        return SE3_ERR_FMOD;
    }

    result = SE3::sm_pGameSystem->setOutput(FMOD_OUTPUTTYPE_AUTODETECT);
    SE3::Log_Write(__LINE__, __FILE__, result, " - Setting Output Type");

    if (result != FMOD_OK) {
        SE3::Log_Write(__LINE__, __FILE__, result, " - Failed to set output type.");
        return SE3_ERR_FMOD;
    }

    int32_t numDrivers = SE3::GetNumOutputDrivers_Cached(SE3::sm_pGameSystem);
    SE3::Log_Write(__LINE__, __FILE__, FMOD_OK, " - %d Output drivers detected", numDrivers);

    if (!numDrivers) {
        SE3::Log_Write(__LINE__, __FILE__, FMOD_OK, " - SOUND INIT ERROR: NO SOUND OUTPUT DEVICES DETECTED", 0);
        return SE3_ERR_NO_OUTPUT_DEVICES;
    }

    for (int32_t i = 0; i < numDrivers; i++) {
        char driverName[256];
        SE3::GetOutputDriverName(i, driverName, sizeof(driverName), SE3::sm_pGameSystem);
        SE3::Log_Write(__LINE__, __FILE__, FMOD_OK, " --- [%d] %s", i, driverName);
    }

    result = SE3::SetDrivers(params, SE3::sm_pGameSystem);

    if (result != FMOD_OK) {
        SE3::Log_Write(__LINE__, __FILE__, result, " - Failed to set output driver.");
        return SE3_ERR_FMOD;
    }

    const char* speakerModeNames[] = { "RAW", "MONO", "STEREO", "QUAD", "SURROUND", "5.1", "7.1", "PROLOGIC" };

    FMOD_CAPS caps = 0;
    FMOD_SPEAKERMODE speakerMode;
    int32_t outputDriverIndex = params->m_outputDriverIndex;

    if (SE3::sm_pGameSystem->getDriverCaps(outputDriverIndex ? outputDriverIndex - 1 : 0, &caps, NULL, NULL, &speakerMode) != FMOD_OK) {
        speakerMode = FMOD_SPEAKERMODE_STEREO;
    }

    if (params->m_forceStereo) {
        speakerMode = FMOD_SPEAKERMODE_STEREO;
    }

    int32_t dspBufferSize = params->m_dspBufferSize;

    if (dspBufferSize) {
        SE3::Log_Write(__LINE__, __FILE__, FMOD_OK, " - DSPBufferSize = %d [Valid values are 0 = AUTO DETECT (default), 4 through 10]", params->m_dspBufferSize);
    } else {
        SE3::Log_Write(__LINE__, __FILE__, FMOD_OK, " - DSPBufferSize = AUTO DETECT");
    }

    if (caps & FMOD_CAPS_HARDWARE_EMULATED) {
        SE3::Log_Write(__LINE__, __FILE__, FMOD_OK, " - ###########################################################################################");
        SE3::Log_Write(__LINE__, __FILE__, FMOD_OK, " - # NOTICE:   Your Sound Driver is running in EMULATED mode.");
        SE3::Log_Write(__LINE__, __FILE__, FMOD_OK, " - #           This may lead to poor sound performance.");

        if (!dspBufferSize) {
            SE3::Log_Write(__LINE__, __FILE__, FMOD_OK, " - #           Using Max DSP Buffer size (%d) to avoid stuttering.", 10);
            dspBufferSize = 10;
        }

        SE3::Log_Write(__LINE__, __FILE__, FMOD_OK, " - #           Also in EMULATED mode some speaker configurations like 5.1 may not be available.");
        SE3::Log_Write(__LINE__, __FILE__, FMOD_OK, " - ###########################################################################################");
    }

    if (!dspBufferSize) {
        dspBufferSize = 4;
    } else {
        SE3::Log_Write(__LINE__, __FILE__, FMOD_OK, " - ###########################################################################################");
        SE3::Log_Write(__LINE__, __FILE__, FMOD_OK, " - # NOTICE:   You are using a custom DSP Buffer size");
        SE3::Log_Write(__LINE__, __FILE__, FMOD_OK, " - #           Higher settings may reduce sound stuttering and crackling,");
        SE3::Log_Write(__LINE__, __FILE__, FMOD_OK, " - #           but may affect sound timing.");
        SE3::Log_Write(__LINE__, __FILE__, FMOD_OK, " - #           Do not change this setting from default unless you really need to.");
        SE3::Log_Write(__LINE__, __FILE__, FMOD_OK, " - ###########################################################################################");

        if (dspBufferSize < 4 || dspBufferSize > 10) {
            SE3::Log_Write(__LINE__, __FILE__, FMOD_OK, " - ###########################################################################################");
            SE3::Log_Write(__LINE__, __FILE__, FMOD_OK, " - # NOTICE:    DSPBufferSize out of range!! Valid range is %d to %d, or %d for AUTO DETECT", 4, 10, 0);
            SE3::Log_Write(__LINE__, __FILE__, FMOD_OK, " - #            Using default DSPBufferSize");
            SE3::Log_Write(__LINE__, __FILE__, FMOD_OK, " - ###########################################################################################");
            dspBufferSize = 4;
        }
    }

    result = SE3::sm_pGameSystem->setDSPBufferSize(1024, dspBufferSize);
    SE3::Log_Write(__LINE__, __FILE__, result, " - Setting DSP Buffer Size");

    if (result != FMOD_OK) {
        SE3::Log_Write(__LINE__, __FILE__, FMOD_OK, " - ###########################################################################################");
        SE3::Log_Write(__LINE__, __FILE__, FMOD_OK, " - # Driver failed to accept buffer size setting");
        SE3::Log_Write(__LINE__, __FILE__, FMOD_OK, " - ###########################################################################################");
        return SE3_ERR_FMOD;
    }

    result = SE3::sm_pGameSystem->setSpeakerMode(speakerMode);
    SE3::Log_Write(__LINE__, __FILE__, result, " - Using Control Panel Speaker Mode %s", speakerModeNames[speakerMode]);

    if (result != FMOD_OK) {
        SE3::Log_Write(" - Failed to set speaker mode");
        return SE3_ERR_FMOD;
    }

    int32_t resampleMethod = params->m_resampleMethod != 0;
    FMOD_SOUND_FORMAT format = FMOD_SOUND_FORMAT_PCM16;

    if (params->m_mixMode) {
        SE3::Log_Write(__LINE__, __FILE__, FMOD_OK, " -###########################################################################################");
        SE3::Log_Write(__LINE__, __FILE__, FMOD_OK, " -# NOTICE:   Using Mix Mode 2.");

        switch (params->m_mixMode) {
            case SE3MIXMODE2_PCMFLOAT:
                SE3::Log_Write(__LINE__, __FILE__, FMOD_OK, " -# %d PCMFLOAT", 1);
                format = FMOD_SOUND_FORMAT_PCMFLOAT;
                break;

            case SE3MIXMODE2_PCM24:
                SE3::Log_Write(__LINE__, __FILE__, FMOD_OK, " -# %d PCM24", 2);
                format = FMOD_SOUND_FORMAT_PCM24;
                break;

            case SE3MIXMODE2_PCM32:
                SE3::Log_Write(__LINE__, __FILE__, FMOD_OK, " -# %d PCM32", 3);
                format = FMOD_SOUND_FORMAT_PCM32;
                break;

            default:
                SE3::Log_Write(__LINE__, __FILE__, FMOD_OK, " -# %d PCM16", 0);
                break;
        }

        SE3::Log_Write(__LINE__, __FILE__, FMOD_OK, " -###########################################################################################");
    }

    result = SE3::sm_pGameSystem->setSoftwareFormat(params->m_sampleRate, format, 0, 0, static_cast<FMOD_DSP_RESAMPLER>(resampleMethod));
    SE3::Log_Write(__LINE__, __FILE__, FMOD_OK, " - Setting Software Format (%d, %d, %d).", params->m_sampleRate, format, resampleMethod);

    if (result != FMOD_OK) {
        SE3::Log_Write(__LINE__, __FILE__, FMOD_OK, " - ###########################################################################################");
        SE3::Log_Write(__LINE__, __FILE__, FMOD_OK, " - # Failed to set software format, driver doesn't support selected settings.");
        SE3::Log_Write(__LINE__, __FILE__, FMOD_OK, " - ###########################################################################################");
        return SE3_ERR_FMOD;
    }

    FMOD_INITFLAGS initFlags = 0;

    if (params->m_softwareHRTF) {
        initFlags = FMOD_INIT_SOFTWARE_HRTF;
    }

    int32_t numChannels = params->m_numChannels;

    if (numChannels > 128) {
        numChannels = 128;
    } else if (numChannels < 20) {
        numChannels = 20;
    }

    SE3::Log_Write(__LINE__, __FILE__, FMOD_OK, " - Requested %d channels.", numChannels);

    int32_t useHardware = 1;

    if (params->m_hardware) {
        SE3::Log_Write(__LINE__, __FILE__, FMOD_OK, " - Initializing Hardware Channels");

        int num2D = 0;
        int num3D = 0;
        int total = 0;

        if (SE3::sm_pGameSystem->getHardwareChannels(&num2D, &num3D, &total) != FMOD_OK) {
            SE3::Log_Write(__LINE__, __FILE__, FMOD_OK, " - ###########################################################################################");
            SE3::Log_Write(__LINE__, __FILE__, FMOD_OK, " - # Sound driver failed to report hardware information");
            SE3::Log_Write(__LINE__, __FILE__, FMOD_OK, " - ###########################################################################################");
            useHardware = 0;
        }

        SE3::Log_Write(__LINE__, __FILE__, FMOD_OK, " - Sound Driver Reports: %d Hardware Channels Available.", num3D);

        if (useHardware) {
            if (!num3D) {
                SE3::Log_Write(__LINE__, __FILE__, FMOD_OK, " - ###########################################################################################");
                SE3::Log_Write(__LINE__, __FILE__, FMOD_OK, " - # NOTICE:   No Hardware Channels Available.");
                SE3::Log_Write(__LINE__, __FILE__, FMOD_OK, " - #           Microsoft dropped support for hardware channels in Windows Vista and Windows 7.");
                SE3::Log_Write(__LINE__, __FILE__, FMOD_OK, " - #           If you are not using Windows Vista or Windows 7 and you are still seeing this, ");
                SE3::Log_Write(__LINE__, __FILE__, FMOD_OK, " - #           it means your audio hardware/driver doesn't support hardware channels.");
                SE3::Log_Write(__LINE__, __FILE__, FMOD_OK, " - ###########################################################################################");
                useHardware = 0;
            } else if (num3D < 20) {
                SE3::Log_Write(__LINE__, __FILE__, FMOD_OK, " - ###########################################################################################");
                SE3::Log_Write(__LINE__, __FILE__, FMOD_OK, " - # NOTICE:   Insufficient Hardware Channels Available.");
                SE3::Log_Write(__LINE__, __FILE__, FMOD_OK, " - #           World of Warcraft Requires At Least %d Hardware Channels to Function in Hardware Mode.", 20);
                SE3::Log_Write(__LINE__, __FILE__, FMOD_OK, " - ###########################################################################################");
                useHardware = 0;
            } else {
                if (num3D < numChannels) {
                    SE3::Log_Write(__LINE__, __FILE__, FMOD_OK, " - ###########################################################################################");
                    SE3::Log_Write(__LINE__, __FILE__, FMOD_OK, " - # NOTICE:");
                    SE3::Log_Write(__LINE__, __FILE__, FMOD_OK, " - # Requested %d hardware channels, but only %d are available.", numChannels, num3D);
                    SE3::Log_Write(__LINE__, __FILE__, FMOD_OK, " - # Using the maximum available hardware channels (%d).", num3D);
                    SE3::Log_Write(__LINE__, __FILE__, FMOD_OK, " - ###########################################################################################");
                    numChannels = num3D;
                }

                if (SE3::sm_pGameSystem->setHardwareChannels(0, 0, numChannels, numChannels) != FMOD_OK) {
                    SE3::Log_Write(__LINE__, __FILE__, FMOD_OK, " - ###########################################################################################");
                    SE3::Log_Write(__LINE__, __FILE__, FMOD_OK, " - # Failed to set hardware channels");
                    SE3::Log_Write(__LINE__, __FILE__, FMOD_OK, " - ###########################################################################################");
                    useHardware = 0;
                } else if (SE3::sm_pGameSystem->setSoftwareChannels(16) != FMOD_OK) {
                    SE3::Log_Write(__LINE__, __FILE__, FMOD_OK, " - ###########################################################################################");
                    SE3::Log_Write(__LINE__, __FILE__, FMOD_OK, " - # Failed to set software channels");
                    SE3::Log_Write(__LINE__, __FILE__, FMOD_OK, " - ###########################################################################################");
                    useHardware = 0;
                }
            }
        }
    }

    if (!params->m_hardware || !useHardware) {
        SE3::Log_Write(__LINE__, __FILE__, FMOD_OK, " - Initializing Software Channels");
        int32_t success = 1;

        if (numChannels > 64) {
            SE3::Log_Write(__LINE__, __FILE__, FMOD_OK, " -###########################################################################################");
            SE3::Log_Write(__LINE__, __FILE__, FMOD_OK, " -# NOTICE:   You are using a very high number of channels.");
            SE3::Log_Write(__LINE__, __FILE__, FMOD_OK, " -#           This may lead to poor sound performance.");
            SE3::Log_Write(__LINE__, __FILE__, FMOD_OK, " -###########################################################################################");
        }

        if (SE3::sm_pGameSystem->setHardwareChannels(0, 0, 0, 0) != FMOD_OK) {
            SE3::Log_Write(__LINE__, __FILE__, FMOD_OK, " - ###########################################################################################");
            SE3::Log_Write(__LINE__, __FILE__, FMOD_OK, " - # Failed to set number of channels");
            SE3::Log_Write(__LINE__, __FILE__, FMOD_OK, " - ###########################################################################################");
            success = 0;
        }

        if (SE3::sm_pGameSystem->setSoftwareChannels(numChannels) != FMOD_OK) {
            SE3::Log_Write(__LINE__, __FILE__, FMOD_OK, " - ###########################################################################################");
            SE3::Log_Write(__LINE__, __FILE__, FMOD_OK, " - # Failed to set software channels");
            SE3::Log_Write(__LINE__, __FILE__, FMOD_OK, " - ###########################################################################################");
            return SE3_ERR_FMOD;
        }

        if (!success) {
            return SE3_ERR_FMOD;
        }
    }

    result = SE3::sm_pGameSystem->init(params->m_maxSoftwareChannels, initFlags | FMOD_INIT_3D_RIGHTHANDED, NULL);
    SE3::Log_Write(__LINE__, __FILE__, result, " - Init FMOD Game System");

    if (result == FMOD_ERR_OUTPUT_CREATEBUFFER) {
        SE3::Log_Write(__LINE__, __FILE__, FMOD_OK, " - ###########################################################################################");
        SE3::Log_Write(__LINE__, __FILE__, FMOD_OK, " - # NOTICE:   FMOD System Init failed!");
        SE3::Log_Write(__LINE__, __FILE__, FMOD_OK, " - #           Control Panel Speaker Mode %s FAILED to initialize!", speakerModeNames[speakerMode]);
        SE3::Log_Write(__LINE__, __FILE__, FMOD_OK, " - #           Reverting to STEREO");
        SE3::Log_Write(__LINE__, __FILE__, FMOD_OK, " - #           This most commonly happens when SURROUND is selected.");
        SE3::Log_Write(__LINE__, __FILE__, FMOD_OK, " - #           SURROUND is actually 5.0 (no subwoofer).");
        SE3::Log_Write(__LINE__, __FILE__, FMOD_OK, " - #           Many sound drivers do not support SURROUND even though you can select it in the control panel");
        SE3::Log_Write(__LINE__, __FILE__, FMOD_OK, " - #           You probably wanted 5.1, not SURROUND.");
        SE3::Log_Write(__LINE__, __FILE__, FMOD_OK, " - #           Some speaker modes will fail to initialize if the sound driver is running in EMULATED mode.");
        SE3::Log_Write(__LINE__, __FILE__, FMOD_OK, " - ###########################################################################################");
        SE3::Log_Write(" - Failed to init FMOD Game System.");
        return SE3_ERR_FMOD;
    }

    if (result != FMOD_OK) {
        SE3::Log_Write(" - Failed to init FMOD Game System.");
        return SE3_ERR_FMOD;
    }

    SE3::Log_Write(" - Ok");
    SE3::sm_pGameSystem->getOutput(&SE3::sm_eGameSystemOutputType);

    result = SE3::sm_pGameSystem->set3DSettings(1.0f, 1.0f, 4.0f);
    SE3::Log_Write(__LINE__, __FILE__, result, " - Setting Rolloff Scale");

    result = SE3::sm_pGameSystem->set3DRolloffCallback(RolloffCallback);
    SE3::Log_Write(__LINE__, __FILE__, result, " - Setting Rolloff Callback");

    SE3::Log_Write(__LINE__, __FILE__, FMOD_OK, " - Sound Bus Init");
    SE3::SoundBusInitBuses();
    SE3::SoundBusSetParameters(SE3BUS_SFX, SE3BUS_MASTER, 0x7FFFFFFF, params->m_sfxVolume, 0, params->m_sfxEnabled);
    SE3::SoundBusSetParameters(SE3BUS_MUSIC, SE3BUS_MASTER, 0x7FFFFFFF, params->m_musicVolume, 0, params->m_musicEnabled);
    SE3::SoundBusSetParameters(SE3BUS_AMBIENCE, SE3BUS_MASTER, 0x7FFFFFFF, params->m_ambienceVolume, 0, params->m_ambienceEnabled);
    SE3::SoundBusSetParameters(SE3BUS_MOVIE, SE3BUS_NONE, 0x7FFFFFFF, 1.0f, 0, 1);
    SE3::SoundBusSetParameters(SE3BUS_5, SE3BUS_MASTER, 8, 1.0f, 0, 1);
    SE3::SoundBusSetParameters(SE3BUS_6, SE3BUS_SFX, 8, 1.0f, 0, 1);
    SE3::SoundBusSetParameters(SE3BUS_7, SE3BUS_MUSIC, 8, 1.0f, 0, 1);
    SE3::SoundBusSetParameters(SE3BUS_8, SE3BUS_AMBIENCE, 8, 1.0f, 0, 1);
    SE3::SoundBusSetParameters(SE3BUS_9, SE3BUS_MASTER, 1, 1.0f, 0, 1);
    SE3::SoundBusSetParameters(SE3BUS_10, SE3BUS_SFX, 1, 1.0f, 0, 1);
    SE3::SoundBusSetParameters(SE3BUS_11, SE3BUS_SFX, 2, 1.0f, 0, 1);
    SE3::SoundBusSetParameters(SE3BUS_12, SE3BUS_SFX, 1, 1.0f, 0, 1);
    SE3::SoundBusSetParameters(SE3BUS_13, SE3BUS_SFX, 2, 1.0f, 0, 1);
    SE3::SoundBusSetParameters(SE3BUS_14, SE3BUS_SFX, 2, 1.0f, 0, 1);
    SE3::SoundBusSetParameters(SE3BUS_15, SE3BUS_SFX, 1, 1.0f, 0, 1);
    SE3::SoundBusSetParameters(SE3BUS_16, SE3BUS_SFX, 0x7FFFFFFF, 1.0f, 0, 1);
    SE3::SoundBusSetParameters(SE3BUS_17, SE3BUS_SFX, 4, 1.0f, 0, 1);
    SE3::SoundBusSetParameters(SE3BUS_18, SE3BUS_SFX, 1, 1.0f, 0, 1);
    SE3::SoundBusSetParameters(SE3BUS_19, SE3BUS_SFX, 2, 1.0f, 0, 1);
    SE3::SoundBusSetParameters(SE3BUS_20, SE3BUS_SFX, 4, 1.0f, 0, 1);
    SE3::SoundBusSetParameters(SE3BUS_MASTER, SE3BUS_NONE, 0x7FFFFFFF, params->m_masterVolume, 0, params->m_masterEnabled);
    SE3::SoundBusSetParameters(SE3BUS_VOICECHAT, SE3BUS_MASTER, 8, params->m_voiceChatVolume, 0, 1);
    SE3::SoundBusSetParameters(SE3BUS_DIALOG, SE3BUS_SFX, 0x7FFFFFFF, 1.0f, 0, 1);

    EventRegister(EVENT_ID_POLL, SE3::HeartBeat);

    SE3::Log_Write(" - Init Suppressors");
    SE3::InitSuppressorSounds();
    SE3::RegisterCacheEventCallback(SE3::CacheEventCallback);
    SE3::RegisterWaveDataDoneLoadingCallback(SE3::AddMessageToQueue_DoneLoading);
    SE3::InitSoundKitDefList();
    FMOD::Debug_SetLevel(FMOD_DEBUG_LEVEL_NONE);

    return SE3_OK;
}

// OFFSET: 0x00711E70/0x1403E3900
void SE3::ShutDown() {
    SE3::Log_Write(" - Shutting Down SE3");
    EventUnregister(EVENT_ID_POLL, SE3::HeartBeat);

    SE3::sm_SE3ShuttingDown_CritSec.Enter();
    SE3::sm_bShuttingDown = 1;
    SE3::sm_SE3ShuttingDown_CritSec.Leave();

    SE3::StopAllSounds(SE3BUS_ALL, 0);
    SE3::ProcessSoundKitObjectDeleteList();
    SE3::ProcessWaveDataNodeDeleteList(0, 1);
    SE3::ProcessMemorySoundEndedMessageQueue();
    SE3::FreeSoundKitDefList();
    SE3::CleanUpFMODGameSystem();

    SE3::sm_SoundCache_Loading_CritSec.Enter();
    SE3::sm_SoundCache_Loading.Clear();
    SE3::sm_SoundCache_Loading_CritSec.Leave();

    SE3::sm_StreamedSound_Loading_CritSec.Enter();
    SE3::sm_Streams_Loading.Clear();
    SE3::sm_StreamedSound_Loading_CritSec.Leave();

    SE3::sm_SoundCache.Clear();
    SE3::sm_Streams.Clear();
    SE3::sm_WaveDataNode_DeleteList.Clear();

    for (SE3SOUNDKITOBJECT_LOOKUP* lookup = SE3::sm_SoundKitObjectTable.Head(); lookup; lookup = SE3::sm_SoundKitObjectTable.Head()) {
        lookup->m_pSKO->m_pSoundKitDef = NULL;
        lookup->m_pSKO->m_pMySKOL = NULL;

        if (lookup->m_pSKO) {
            delete lookup->m_pSKO;
        }

        delete lookup;
    }

    for (SE3MemorySoundEndedMessage* message = sm_MemorySoundEnded_MessageQueue.Head(); message; ) {
        SE3MemorySoundEndedMessage* next = sm_MemorySoundEnded_MessageQueue.Next(message);
        sm_MemorySoundEnded_MessageQueue.UnlinkNode(message);
        s_MemorySoundEnded_MessagePool.PutData(message, NULL, 0);
        message = next;
    }

    s_MemorySoundEnded_MessagePool.Clear(typeid(SE3MemorySoundEndedMessage).raw_name(), -2, 1);
    sm_MemorySoundEnded_MessageQueue.Clear();

    for (SE3SoundEndedMessage* message = sm_SoundEnded_MessageQueue.Head(); message; ) {
        SE3SoundEndedMessage* next = sm_SoundEnded_MessageQueue.Next(message);
        sm_SoundEnded_MessageQueue.UnlinkNode(message);
        s_SoundEnded_MessagePool.PutData(message, NULL, 0);
        message = next;
    }

    s_SoundEnded_MessagePool.Clear(typeid(SE3SoundEndedMessage).raw_name(), -2, 1);
    sm_SoundEnded_MessageQueue.Clear();

    SE3::sm_DoneLoading_MessageQueue_CritSect.Enter();

    for (SE3DoneLoadingMessage* message = SE3::sm_DoneLoading_MessageQueue.Head(); message; ) {
        SE3DoneLoadingMessage* next = SE3::sm_DoneLoading_MessageQueue.Next(message);
        SE3::sm_DoneLoading_MessageQueue.UnlinkNode(message);
        s_DoneLoading_MessagePool.PutData(message, NULL, 0);
        message = next;
    }

    s_DoneLoading_MessagePool.Clear(typeid(SE3DoneLoadingMessage).raw_name(), -2, 1);
    SE3::sm_DoneLoading_MessageQueue.Clear();

    SE3::sm_DoneLoading_MessageQueue_CritSect.Leave();

    sm_CombinableSoundsTable.Clear();
    s_CombinableSoundKitPositionPool.Clear(typeid(SE3CombinableSoundKitPosition).raw_name(), -2, 1);

    SE3::Log_Write(" - Shutting Down SE3 COMPLETE.");
}

// OFFSET: 0x007121A0/0x1403E4010
void SE3::SetFocusScalar(float scalar) {
    SE3::sm_fFocusScalar = scalar;
}

// OFFSET: 0x007121C0/0x1403E4020
void SE3::SoundBusInitBuses() {
    for (int32_t i = 0; i < SE3BUS_LAST; i++) {
        SE3::sm_aSoundBus[i].m_parent = SE3BUS_NONE;
        SE3::sm_aSoundBus[i].m_volume = 1.0f;
        SE3::sm_aSoundBus[i].m_muted = 0;
        SE3::sm_aSoundBus[i].m_enabled = 1;
        SE3::sm_aSoundBus[i].m_channelCount = 0;
        SE3::sm_aSoundBus[i].m_maxChannels = 10000;
    }
}

// OFFSET: 0x00712210/0x1403E4060
void SE3::SoundBusSetParameters(SE3BUS bus, SE3BUS parent, int32_t maxChannels, float volume, int32_t muted, int32_t enabled) {
    SE3::sm_aSoundBus[bus].m_parent = parent;
    SE3::sm_aSoundBus[bus].m_maxChannels = maxChannels;

    if (volume < 0.0f) {
        volume = 0.0f;
    } else if (volume > 1.0f) {
        volume = 1.0f;
    }

    SE3::sm_aSoundBus[bus].m_volume = volume;
    SE3::sm_aSoundBus[bus].m_muted = muted;
    SE3::sm_aSoundBus[bus].m_enabled = enabled;
}

// OFFSET: 0x00712270/0x1403E40B0
void SE3::SoundBusSetVolume(SE3BUS bus, float volume) {
    if (volume < 0.0f) {
        volume = 0.0f;
    } else if (volume > 1.0f) {
        volume = 1.0f;
    }

    SE3::sm_aSoundBus[bus].m_volume = volume;
}

// OFFSET: 0x007122B0/0x1403E40E0
void SE3::SoundBusMute(SE3BUS bus, int32_t mute) {
    SE3::sm_aSoundBus[bus].m_muted = mute;
}

// OFFSET: 0x007122D0/0x1403E4100
void SE3::SoundBusEnable(SE3BUS bus, int32_t enable) {
    SE3::sm_aSoundBus[bus].m_enabled = enable;
}

// OFFSET: 0x007122F0/0x1403E4120
float SE3::SoundBusGetVolume(SE3BUS bus) {
    float volume = SE3::sm_fFocusScalar;

    do {
        float busVolume;

        if (SE3::sm_aSoundBus[bus].m_muted || !SE3::sm_aSoundBus[bus].m_enabled) {
            busVolume = 0.0f;
        } else {
            busVolume = SE3::sm_aSoundBus[bus].m_volume;
        }

        bus = SE3::sm_aSoundBus[bus].m_parent;
        volume = busVolume * volume;
    } while (bus != SE3BUS_NONE);

    return volume;
}

// OFFSET: 0x00712350
int32_t SE3::SoundBusIsFull(SE3BUS bus) {
    while (SE3::sm_aSoundBus[bus].m_channelCount < SE3::sm_aSoundBus[bus].m_maxChannels) {
        bus = SE3::sm_aSoundBus[bus].m_parent;

        if (bus == SE3BUS_NONE) {
            return 0;
        }
    }

    return 1;
}

// OFFSET: 0x00712380
int32_t SE3::SoundBusIsEnabled(SE3BUS bus) {
    while (SE3::sm_aSoundBus[bus].m_enabled) {
        bus = SE3::sm_aSoundBus[bus].m_parent;

        if (bus == SE3BUS_NONE) {
            return 1;
        }
    }

    return 0;
}

// OFFSET: 0x007123B0/0x1403E41E0
int32_t SE3::SoundBusIsMuted(SE3BUS bus) {
    while (!SE3::sm_aSoundBus[bus].m_muted) {
        bus = SE3::sm_aSoundBus[bus].m_parent;

        if (bus == SE3BUS_NONE) {
            return 0;
        }
    }

    return 1;
}

// OFFSET: 0x007123E0
void SE3::SoundBusIncrementChannelCount(SE3BUS bus) {
    SE3::sm_aSoundBus[bus].m_channelCount++;
}

// OFFSET: 0x00712400/0x1403E4230
void SE3::SoundBusDecrementChannelCount(SE3BUS bus) {
    SE3::sm_aSoundBus[bus].m_channelCount--;
}

// OFFSET: 0x00712420/0x1403E4250
int32_t SE3::IsBusEqualOrChild(SE3BUS bus, SE3BUS parent) {
    if (parent != SE3BUS_ALL) {
        while (bus != parent) {
            bus = SE3::sm_aSoundBus[bus].m_parent;

            if (bus == SE3BUS_NONE) {
                return 0;
            }
        }
    }

    return 1;
}

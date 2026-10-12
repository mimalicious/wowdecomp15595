#include <cstddef>
#include <cstdint>
#include <cstring>
#include "Engine/Source/SoundEngine/SE3.h"
#include "Engine/Source/Storm/Source/SLock.h"

// STATIC: 0x00E4E824
static HASHKEY_NONE s_memoryLookupKey;

// STATIC: 0x00E4E828/0x140BAFDD0
TSHashTable<SE3MemorySoundObject, HASHKEY_NONE> SE3::sm_MemorySoundObjectTable;

// OFFSET: 0x007147F0/0x1403E6F30
static FMOD_RESULT F_CALLBACK MemorySoundEndedCallback(FMOD_CHANNEL* channel, FMOD_CHANNEL_CALLBACKTYPE type, int command, unsigned int commanddata1, unsigned int commanddata2) {
    if (type == FMOD_CHANNEL_CALLBACKTYPE_END) {
        void* userdata = NULL;
        reinterpret_cast<FMOD::Channel*>(channel)->getUserData(&userdata);

        SE3::sm_SE3ShuttingDown_CritSec.Enter();
        SE3::AddMessageToQueue_MemorySoundEnded(static_cast<uint32_t>(reinterpret_cast<uintptr_t>(userdata)));
        SE3::sm_SE3ShuttingDown_CritSec.Leave();
    }

    return FMOD_OK;
}

// OFFSET: 0x00714830/0x1403E6F80
void SE3MemorySoundProperties::ResetToDefaults() {
    this->m_bus = SE3BUS_SFX;
    this->m_volume = 1.0f;
    this->m_data = NULL;
    this->m_length = 0;
    this->m_handle = NULL;
    this->m_fpPlayFinishedCallback = NULL;
    this->m_playFinishedParam = NULL;
    this->m_position = 0;
    this->m_attachMonitorDSP = 0;
}

// OFFSET: 0x00714860/0x1403E6FB0
int32_t SE3::SetMemorySoundFinishedCallback(uint32_t handle, void (*callback)(SE3MemorySoundObject*, void*)) {
    SE3MemorySoundObject* sound = SE3::sm_MemorySoundObjectTable.Ptr(handle, s_memoryLookupKey);

    if (!sound) {
        return SE3_ERR_NOT_FOUND;
    }

    sound->m_fpPlayFinishedCallback = callback;

    return SE3_OK;
}

// OFFSET: 0x00714890/0x1403E70D0
int32_t SE3::StopMemorySound(uint32_t handle) {
    if (!handle) {
        return SE3_ERR_NOT_FOUND;
    }

    FMOD::Channel* channel;
    SE3SOUNDKITOBJECT_LOOKUP* lookup = SE3::sm_SoundKitObjectTable.Ptr(handle, s_memoryLookupKey);

    if (lookup && lookup->m_pSKO) {
        channel = lookup->m_pSKO->m_channels[0].m_pFMODChannel;
    } else {
        SE3MemorySoundObject* sound = SE3::sm_MemorySoundObjectTable.Ptr(handle, s_memoryLookupKey);

        if (!sound) {
            return SE3_ERR_NOT_FOUND;
        }

        channel = sound->m_pFMODChannel;
    }

    if (!channel) {
        return SE3_ERR_NOT_FOUND;
    }

    return channel->stop() != FMOD_OK ? SE3_ERR_FMOD : SE3_OK;
}

// OFFSET: 0x007148F0
uint32_t SE3::GetPositionInMS(uint32_t handle) {
    if (!handle) {
        return SE3_ERR_NOT_FOUND;
    }

    FMOD::Channel* channel;
    SE3SOUNDKITOBJECT_LOOKUP* lookup = SE3::sm_SoundKitObjectTable.Ptr(handle, s_memoryLookupKey);

    if (lookup && lookup->m_pSKO) {
        channel = lookup->m_pSKO->m_channels[0].m_pFMODChannel;
    } else {
        SE3MemorySoundObject* sound = SE3::sm_MemorySoundObjectTable.Ptr(handle, s_memoryLookupKey);

        if (!sound) {
            return SE3_ERR_NOT_FOUND;
        }

        channel = sound->m_pFMODChannel;
    }

    if (!channel) {
        return SE3_ERR_NOT_FOUND;
    }

    unsigned int position = 0;
    channel->getPosition(&position, FMOD_TIMEUNIT_MS);
    return position;
}

// INLINED
SE3MemorySoundObject::SE3MemorySoundObject() {
    this->m_uUID = ++SE3::sm_uNextUniqueID;
}

// OFFSET: 0x00714960/0x1403E7140
int32_t SE3::IsPlaying(uint32_t handle) {
    if (!handle) {
        return 0;
    }

    SE3SOUNDKITOBJECT_LOOKUP* lookup = SE3::sm_SoundKitObjectTable.Ptr(handle, s_memoryLookupKey);
    SE3SoundKitObject* sko;

    if (lookup && (sko = lookup->m_pSKO) != NULL) {
        if (sko->m_link.m_next || sko->m_loadingLink.m_next || sko->m_goGoGoLink.m_next || sko->m_downloadLink.m_next) {
            return 1;
        }
    } else {
        SE3MemorySoundObject* sound = SE3::sm_MemorySoundObjectTable.Ptr(handle, s_memoryLookupKey);

        if (sound) {
            bool playing = false;
            sound->m_pFMODChannel->isPlaying(&playing);
            return playing;
        }
    }

    return 0;
}

// OFFSET: 0x00715200/0x1403E7D80
int32_t SE3::PlaySoundFromMemory(SE3MemorySoundProperties* properties, int32_t chatSystem) {
    FMOD::System* system = chatSystem ? SE3::sm_pChatSystem : SE3::sm_pGameSystem;

    if (!properties->m_handle) {
        return SE3_ERR_INVALID_PARAM;
    }

    SE3MemorySoundObject* sound = SE3::sm_MemorySoundObjectTable.New(SE3::sm_uNextUniqueID + 1, s_memoryLookupKey, 0, 0);
    *properties->m_handle = sound->m_uUID;

    FMOD_CREATESOUNDEXINFO exinfo;
    exinfo.cbsize = sizeof(exinfo);
    exinfo.length = properties->m_length;
    memset(&exinfo.fileoffset, 0, sizeof(exinfo) - offsetof(FMOD_CREATESOUNDEXINFO, fileoffset));

    if (system->createStream(properties->m_data, FMOD_LOOP_OFF | FMOD_2D | FMOD_SOFTWARE | FMOD_OPENMEMORY, &exinfo, &sound->m_pFMODSound) != FMOD_OK) {
        SE3::sm_MemorySoundObjectTable.DeleteNode(sound);
        return SE3_ERR_FMOD;
    }

    system->playSound(FMOD_CHANNEL_FREE, sound->m_pFMODSound, true, &sound->m_pFMODChannel);
    sound->m_pFMODChannel->setUserData(reinterpret_cast<void*>(static_cast<uintptr_t>(sound->m_uUID)));
    sound->m_pFMODChannel->setCallback(FMOD_CHANNEL_CALLBACKTYPE_END, MemorySoundEndedCallback, 0);
    sound->m_fpPlayFinishedCallback = properties->m_fpPlayFinishedCallback;
    sound->m_playFinishedParam = properties->m_playFinishedParam;
    sound->m_pFMODChannel->setPosition(properties->m_position, FMOD_TIMEUNIT_MS);

    if (properties->m_attachMonitorDSP) {
        SE3::AttachMonitorDSP(sound->m_pFMODChannel);
    }

    sound->m_pFMODChannel->setVolume(SE3::SoundBusGetVolume(properties->m_bus));
    sound->m_pFMODChannel->setPaused(false);

    return SE3_OK;
}

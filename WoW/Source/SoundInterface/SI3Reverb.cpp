#include <cstddef>
#include <cstdint>
#include <cstring>
#include "Engine/Source/SoundEngine/SE3.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/SoundInterface/SI3.h"
#include "WoW/Source/SoundInterface/SI3ZoneSounds.h"

// STATIC: 0x00E283B4
int32_t SI3::sm_bReverbEnabled;

// OFFSET: 0x006345C0
static void GetReverbProperties(SoundProviderPreferencesRec* rec, FMOD_REVERB_PROPERTIES* properties) {
    if (!properties) {
        return;
    }

    FMOD_REVERB_PROPERTIES off = FMOD_PRESET_OFF;

    if (!rec) {
        memcpy(properties, &off, sizeof(FMOD_REVERB_PROPERTIES));
        return;
    }

    properties->Instance = 0;
    properties->Environment = rec->m_eaxenvironmentSelection;
    properties->EnvSize = rec->m_eax2EnvironmentSize;
    properties->EnvDiffusion = rec->m_eax2EnvironmentDiffusion;
    properties->Room = rec->m_eax2Room;
    properties->RoomHF = rec->m_eax2RoomHf;
    properties->RoomLF = rec->m_eax3RoomLf;
    properties->DecayTime = rec->m_eaxdecayTime;
    properties->DecayHFRatio = rec->m_eax2DecayHfratio;
    properties->DecayLFRatio = rec->m_eax3DecayLfratio;
    properties->Reflections = rec->m_eax2Reflections;
    properties->ReflectionsDelay = rec->m_eax2ReflectionsDelay;
    properties->Reverb = rec->m_eax2Reverb;
    properties->ReverbDelay = rec->m_eax2ReverbDelay;
    properties->EchoTime = rec->m_eax3EchoTime;
    properties->EchoDepth = rec->m_eax3EchoDepth;
    properties->ModulationTime = rec->m_eax3ModulationTime;
    properties->ModulationDepth = rec->m_eax3ModulationDepth;
    properties->AirAbsorptionHF = rec->m_eax2AirAbsorption;
    properties->HFReference = rec->m_eax3Hfreference;
    properties->LFReference = rec->m_eax3Lfreference;
    properties->Flags = 0x4FF;
    properties->RoomRolloffFactor = rec->m_eax2RoomRolloff;
    properties->Diffusion = 100.0f;
    properties->Density = 100.0f;
    properties->ReflectionsPan[0] = 0.0f;
    properties->ReflectionsPan[1] = 0.0f;
    properties->ReflectionsPan[2] = 0.0f;
    properties->ReverbPan[0] = 0.0f;
    properties->ReverbPan[1] = 0.0f;
    properties->ReverbPan[2] = 0.0f;
}

// OFFSET: 0x00634790
int32_t SI3::GetZoneReverb() {
    int32_t type = SI3ZONESOUNDTYPE_LAST - 1;

    while (SI3::sm_aZoneSoundInfo[type].m_reverb < 0 || (SI3::IsUnderwaterZoneSoundType(type) && !SI3::sm_bCameraIsUnderwater) || (type == SI3ZONESOUNDTYPE_8 && SI3::sm_bPlayerIsIndoors)) {
        if (--type < 0) {
            return 0;
        }
    }

    return SI3::sm_aZoneSoundInfo[type].m_reverb;
}

// PROVEN: f6c49d19 IDENTICAL/NONE
// OFFSET: 0x006347F0
void SI3::EnableReverb(int32_t enable) {
    SI3::sm_bReverbEnabled = enable;
}

// OFFSET: 0x00634800
void SI3::UpdateReverb() {
    FMOD_REVERB_PROPERTIES properties;

    if (SI3::sm_bReverbEnabled) {
        SoundProviderPreferencesRec* rec = g_soundProviderPreferencesDB.GetRecord(SI3::GetZoneReverb());

        if (rec) {
            GetReverbProperties(rec, &properties);
            SE3::SetReverb(&properties);
            return;
        }
    }

    GetReverbProperties(NULL, &properties);
    SE3::SetReverb(&properties);
}

// PROVEN: 30f52b84 IDENTICAL/NONE
// OFFSET: 0x00634880
void SI3::ReverbUnderwaterChanged() {
    SI3::UpdateReverb();
}

#include <cstddef>
#include <cstdint>
#include <cstring>
#include "Engine/Source/ConsoleBase/ConsoleVar.h"
#include "Engine/Source/Os/System.h"
#include "Engine/Source/SoundEngine/SE3.h"
#include "Engine/Source/Storm/H/storm.h"
#include "WoW/Source/Game/GameClient/ComSatClient.h"
#include "WoW/Source/SoundInterface/SI3.h"

// STATIC: 0x00E28758/0x140D10308
CVar* SI3::sm_pCVar_VoiceChat_EnableVoiceChat;

// STATIC: 0x00E2875C
CVar* SI3::sm_pCVar_Sound_OutputQuality;

// STATIC: 0x00E28760
CVar* SI3::sm_pCVar_Sound_NumChannels;

// STATIC: 0x00E28764/0x140D10320
CVar* SI3::sm_pCVar_Sound_EnableReverb;

// STATIC: 0x00E28768/0x140D10328
CVar* SI3::sm_pCVar_Sound_EnableSoftwareHRTF;

// STATIC: 0x00E2876C/0x140D10330
CVar* SI3::sm_pCVar_Sound_OutputDriverIndex;

// STATIC: 0x00E28770/0x140D10338
CVar* SI3::sm_pCVar_Sound_OutputDriverName;

// STATIC: 0x00E28774/0x140D10340
CVar* SI3::sm_pCVar_Sound_VoiceChatInputDriverIndex;

// STATIC: 0x00E28778/0x140D10348
CVar* SI3::sm_pCVar_Sound_VoiceChatInputDriverName;

// STATIC: 0x00E2877C/0x140D10350
CVar* SI3::sm_pCVar_Sound_VoiceChatOutputDriverIndex;

// STATIC: 0x00E28780/0x140D10358
CVar* SI3::sm_pCVar_Sound_VoiceChatOutputDriverName;

// STATIC: 0x00E28784/0x140D10360
CVar* SI3::sm_pCVar_Sound_DSPBufferSize;

// STATIC: 0x00E28788/0x140D10368
CVar* SI3::sm_pCVar_Sound_EnableHardware;

// STATIC: 0x00E2878C/0x140D10370
CVar* SI3::sm_pCVar_Sound_EnableMode2;

// STATIC: 0x00E28790/0x140D10378
CVar* SI3::sm_pCVar_Sound_EnableMixMode2;

// STATIC: 0x00E28794/0x140D10380
CVar* SI3::sm_pCVar_Sound_OutputSampleRate;

// STATIC: 0x00E28798/0x140D10388
CVar* SI3::sm_pCVar_Sound_EnableSFX;

// STATIC: 0x00E2879C/0x140D10390
CVar* SI3::sm_pCVar_Sound_EnableAmbience;

// STATIC: 0x00E287A0/0x140D10398
CVar* SI3::sm_pCVar_Sound_EnableErrorSpeech;

// STATIC: 0x00E287A4/0x140D103A0
CVar* SI3::sm_pCVar_Sound_EnableMusic;

// STATIC: 0x00E287A8/0x140D103A8
CVar* SI3::sm_pCVar_Sound_EnableAllSound;

// STATIC: 0x00E287AC/0x140D103B0
CVar* SI3::sm_pCVar_Sound_MasterVolume;

// STATIC: 0x00E287B0/0x140D103B8
CVar* SI3::sm_pCVar_Sound_SFXVolume;

// STATIC: 0x00E287B4/0x140D103C0
CVar* SI3::sm_pCVar_Sound_MusicVolume;

// STATIC: 0x00E287B8
CVar* SI3::sm_pCVar_Sound_AmbienceVolume;

// STATIC: 0x00E287BC/0x140D103D0
CVar* SI3::sm_pCVar_Sound_ListenerAtCharacter;

// STATIC: 0x00E287C0
CVar* SI3::sm_pCVar_Sound_EnableEmoteSounds;

// STATIC: 0x00E287C4/0x140D103E0
CVar* SI3::sm_pCVar_Sound_ZoneMusicNoDelay;

// STATIC: 0x00E287C8/0x140D103E8
CVar* SI3::sm_pCVar_Sound_EnableArmorFoleySoundForSelf;

// STATIC: 0x00E287CC/0x140D103F0
CVar* SI3::sm_pCVar_Sound_EnableArmorFoleySoundForOthers;

// STATIC: 0x00E287D0/0x140D103F8
CVar* SI3::sm_pCVar_Sound_EnableDSPEffects;

// STATIC: 0x00E287D4/0x140D10400
CVar* SI3::sm_pCVar_Sound_EnablePetSounds;

// STATIC: 0x00E287D8/0x140D10408
CVar* SI3::sm_pCVar_Sound_EnableSoundWhenGameIsInBG;

// STATIC: 0x00E287DC/0x140D10410
CVar* SI3::sm_pCVar_Sound_MaxCacheSizeInBytes;

// STATIC: 0x00E287E0
CVar* SI3::sm_pCVar_Sound_MaxCacheableSizeInBytes;

// STATIC: 0x00E287E4/0x140D10420
CVar* SI3::sm_pCVar_Sound_FootstepSounds;

// PROVEN: 2e4acfbb IDENTICAL/IDENTICAL
// OFFSET: 0x00630AA0/0x1402C5840
static bool MasterVolumeHandler(CVar* var, const char* oldValue, const char* value, void* arg) {
    SE3::SoundBusSetVolume(SE3BUS_MASTER, SStrToFloat(value));
    return true;
}

// PROVEN: bf1d4362 IDENTICAL/IDENTICAL
// OFFSET: 0x00630AC0/0x1402C5860
static bool SFXVolumeHandler(CVar* var, const char* oldValue, const char* value, void* arg) {
    SE3::SoundBusSetVolume(SE3BUS_SFX, SStrToFloat(value));
    return true;
}

// PROVEN: 53cf0d30 IDENTICAL/IDENTICAL
// OFFSET: 0x00630AE0/0x1402C5880
static bool MusicVolumeHandler(CVar* var, const char* oldValue, const char* value, void* arg) {
    SE3::SoundBusSetVolume(SE3BUS_MUSIC, SStrToFloat(value));
    return true;
}

// PROVEN: d5ec65ef IDENTICAL/IDENTICAL
// OFFSET: 0x00630B00/0x1402C58A0
static bool MaxCacheSizeInBytesHandler(CVar* var, const char* oldValue, const char* value, void* arg) {
    SE3::SetMaxCacheSizeInBytes(SStrToInt(value));
    return true;
}

// PROVEN: 67d4f5cc IDENTICAL/IDENTICAL
// OFFSET: 0x00630B20/0x1402C58C0
static bool MaxCacheableSizeInBytesHandler(CVar* var, const char* oldValue, const char* value, void* arg) {
    SE3::SetMaxCacheableSizeInBytes(SStrToInt(value));
    return true;
}

// PROVEN: 3d6a6528 IDENTICAL/IDENTICAL
// OFFSET: 0x00630B40/0x1402C58E0
static bool AmbienceVolumeHandler(CVar* var, const char* oldValue, const char* value, void* arg) {
    SE3::SoundBusSetVolume(SE3BUS_AMBIENCE, SStrToFloat(value));
    return true;
}

// PROVEN: fe97c09a IDENTICAL/UNPROVEN
// OFFSET: 0x00630B60/0x1402C5900
static bool EnableSFXHandler(CVar* var, const char* oldValue, const char* value, void* arg) {
    int32_t enable = SStrToInt(value);
    SE3::SoundBusEnable(SE3BUS_SFX, enable);
    SE3::SoundBusMute(SE3BUS_SFX, enable == 0);
    return true;
}

// PROVEN: 951028de IDENTICAL/UNPROVEN
// OFFSET: 0x00630B90/0x1402C5930
static bool EnableMusicHandler(CVar* var, const char* oldValue, const char* value, void* arg) {
    int32_t enable = SStrToInt(value);
    SE3::SoundBusEnable(SE3BUS_MUSIC, enable);
    SE3::SoundBusMute(SE3BUS_MUSIC, enable == 0);

    if (!enable) {
        SI3::StopAllMusic();
    }

    return true;
}

// PROVEN: 1c0d00dc IDENTICAL/UNPROVEN
// OFFSET: 0x00630BD0/0x1402C5970
static bool EnableAmbienceHandler(CVar* var, const char* oldValue, const char* value, void* arg) {
    int32_t enable = SStrToInt(value);
    SE3::SoundBusEnable(SE3BUS_AMBIENCE, enable);
    SE3::SoundBusMute(SE3BUS_AMBIENCE, enable == 0);
    return true;
}

// PROVEN: 4b3f3d36 IDENTICAL/UNPROVEN
// OFFSET: 0x00630C00/0x1402C59B0
static bool EnableAllSoundHandler(CVar* var, const char* oldValue, const char* value, void* arg) {
    int32_t enable = SStrToInt(value);
    SE3::SoundBusEnable(SE3BUS_MASTER, enable);
    SE3::SoundBusMute(SE3BUS_MASTER, enable == 0);
    return true;
}

// PROVEN: 0e96590b IDENTICAL/UNPROVEN
// OFFSET: 0x00630C30/0x1402C59F0
static bool EnableErrorSpeechHandler(CVar* var, const char* oldValue, const char* value, void* arg) {
    int32_t enable = SStrToInt(value);
    SE3::SoundBusEnable(SE3BUS_10, enable);
    SE3::SoundBusMute(SE3BUS_10, enable == 0);
    return true;
}

// PROVEN: 99e70bec IDENTICAL/UNPROVEN
// OFFSET: 0x00630C60/0x1402C5A30
static bool EnableReverbHandler(CVar* var, const char* oldValue, const char* value, void* arg) {
    if (SStrToInt(value)) {
        SI3::EnableReverb(1);
        SI3::UpdateReverb();
    } else {
        SI3::EnableReverb(0);
        FMOD_REVERB_PROPERTIES properties = FMOD_PRESET_OFF;
        SE3::SetReverb(&properties);
    }

    return true;
}

// OFFSET: 0x00630D90/0x1402C5BA0
static bool OutputDriverIndexHandler(CVar* var, const char* oldValue, const char* value, void* arg) {
    int32_t index = SStrToInt(value);
    char name[256];
    memset(name, 0, sizeof(name));

    if (SE3::GetOutputDriverName(index, name, sizeof(name), SE3::sm_pGameSystem)) {
        SI3::sm_pCVar_Sound_OutputDriverName->Set(name, true, false, false, true);
    }

    return true;
}

// OFFSET: 0x00630E00/0x1402C5C20
void SI3::RegisterCVars() {
    CVar::Register("ChatMusicVolume", "music volume (0.0 to 1.0)", 0, "0.3", NULL, 7, false, NULL, false);
    CVar::Register("ChatSoundVolume", "sound volume (0.0 to 1.0)", 0, "0.4", NULL, 7, false, NULL, false);
    CVar::Register("ChatAmbienceVolume", "Ambience Volume (0.0 to 1.0)", 0, "0.3", NULL, 7, false, NULL, false);
    CVar::Register("StartTalkingDelay", "", 0, "0.0", NULL, 5, false, NULL, false);
    CVar::Register("StartTalkingTime", "", 0, "1.0", NULL, 5, false, NULL, false);
    CVar::Register("StopTalkingDelay", "", 0, "0.0", NULL, 5, false, NULL, false);
    CVar::Register("StopTalkingTime", "", 0, "2.0", NULL, 5, false, NULL, false);
    CVar::Register("OutboundChatVolume", "The software amplification factor (0.0 - 2.0)", 0, "1.0", OutboundChatVolumeHandler, 5, false, NULL, false);
    CVar::Register("InboundChatVolume", "The volume of all other chat you hear (0.0 - 1.0)", 0, "1.0", InboundChatVolumeHandler, 5, false, NULL, false);
    CVar::Register("VoiceChatMode", "Push to talk(0) or voice activation(1)", 0, "0", VoiceChatModeHandler, 5, false, NULL, false);
    CVar::Register("VoiceActivationSensitivity", "Sensitivity of the microphone (0.0 - 1.0)", 0, "0.4", VoiceActivationSensitivityHandler, 5, false, NULL, false);
    CVar::Register("EnableMicrophone", "Enables the microphone so you can speak.", 0, "1", EnableMicrophoneHandler, 5, false, NULL, false);
    SI3::sm_pCVar_VoiceChat_EnableVoiceChat = CVar::Register("EnableVoiceChat", "Enables the voice chat feature.", 0x200, "0", EnableVoiceChatHandler, 5, false, NULL, false);
    CVar::Register("VoiceChatSelfMute", "Turn off your ability to talk.", 0, "0", SelfMuteHandler, 5, false, NULL, false);
    CVar::Register("PushToTalkButton", "String representation of the Push-To-Talk button.", 0, "`", PushToTalkButtonHandler, 5, false, NULL, false);
    SI3::sm_pCVar_Sound_OutputQuality = CVar::Register("Sound_OutputQuality", "sound quality, default 1 (medium)", 0, "1", NULL, 7, false, NULL, false);
    SI3::sm_pCVar_Sound_NumChannels = CVar::Register("Sound_NumChannels", "number of sound channels", 0, "24", NULL, 7, false, NULL, false);
    SI3::sm_pCVar_Sound_EnableReverb = CVar::Register("Sound_EnableReverb", "", 0, "0", EnableReverbHandler, 7, false, NULL, false);
    SI3::sm_pCVar_Sound_EnableSoftwareHRTF = CVar::Register("Sound_EnableSoftwareHRTF", "", 0, "0", NULL, 7, false, NULL, false);
    SI3::sm_pCVar_Sound_OutputDriverIndex = CVar::Register("Sound_OutputDriverIndex", "", 0, "0", OutputDriverIndexHandler, 7, false, NULL, false);
    SI3::sm_pCVar_Sound_OutputDriverName = CVar::Register("Sound_OutputDriverName", "", 0, "Primary Sound Driver", NULL, 7, false, NULL, false);
    SI3::sm_pCVar_Sound_VoiceChatInputDriverIndex = CVar::Register("Sound_VoiceChatInputDriverIndex", "", 0, "0", NULL, 7, false, NULL, false);
    SI3::sm_pCVar_Sound_VoiceChatInputDriverName = CVar::Register("Sound_VoiceChatInputDriverName", "", 0, "Primary Sound Capture Driver", NULL, 7, false, NULL, false);
    SI3::sm_pCVar_Sound_VoiceChatOutputDriverIndex = CVar::Register("Sound_VoiceChatOutputDriverIndex", "", 0, "0", NULL, 7, false, NULL, false);
    SI3::sm_pCVar_Sound_VoiceChatOutputDriverName = CVar::Register("Sound_VoiceChatOutputDriverName", "", 0, "Primary Sound Driver", NULL, 7, false, NULL, false);
    SI3::sm_pCVar_Sound_DSPBufferSize = CVar::Register("Sound_DSPBufferSize", "sound buffer size, default 0", 0, "0", NULL, 7, false, NULL, false);
    SI3::sm_pCVar_Sound_OutputSampleRate = CVar::Register("Sound_OutputSampleRate", "output sample rate", 0, "44100", NULL, 7, false, NULL, false);
    SI3::sm_pCVar_Sound_EnableHardware = CVar::Register("Sound_EnableHardware", "Enables Hardware", 0, "0", NULL, 7, false, NULL, false);
    SI3::sm_pCVar_Sound_EnableMode2 = CVar::Register("Sound_EnableMode2", "test", 0, "0", NULL, 7, false, NULL, false);
    SI3::sm_pCVar_Sound_EnableMixMode2 = CVar::Register("Sound_EnableMixMode2", "test", 0, "0", NULL, 7, false, NULL, false);
    SI3::sm_pCVar_Sound_EnableSFX = CVar::Register("Sound_EnableSFX", "", 0, "1", EnableSFXHandler, 7, false, NULL, false);
    SI3::sm_pCVar_Sound_EnableAmbience = CVar::Register("Sound_EnableAmbience", "Enable Ambience", 0, "1", EnableAmbienceHandler, 7, false, NULL, false);
    SI3::sm_pCVar_Sound_EnableErrorSpeech = CVar::Register("Sound_EnableErrorSpeech", "error speech", 0, "1", EnableErrorSpeechHandler, 7, false, NULL, false);
    SI3::sm_pCVar_Sound_EnableMusic = CVar::Register("Sound_EnableMusic", "Enables music", 0, "1", EnableMusicHandler, 7, false, NULL, false);
    SI3::sm_pCVar_Sound_EnableAllSound = CVar::Register("Sound_EnableAllSound", "", 0, "1", EnableAllSoundHandler, 7, false, NULL, false);
    SI3::sm_pCVar_Sound_MasterVolume = CVar::Register("Sound_MasterVolume", "master volume (0.0 to 1.0)", 0, "1.0", MasterVolumeHandler, 7, false, NULL, false);
    SI3::sm_pCVar_Sound_SFXVolume = CVar::Register("Sound_SFXVolume", "sound volume (0.0 to 1.0)", 0, "1.0", SFXVolumeHandler, 7, false, NULL, false);
    SI3::sm_pCVar_Sound_MusicVolume = CVar::Register("Sound_MusicVolume", "music volume (0.0 to 1.0)", 0, "0.4", MusicVolumeHandler, 7, false, NULL, false);
    SI3::sm_pCVar_Sound_AmbienceVolume = CVar::Register("Sound_AmbienceVolume", "Ambience Volume (0.0 to 1.0)", 0, "0.6", AmbienceVolumeHandler, 7, false, NULL, false);
    SI3::sm_pCVar_Sound_ListenerAtCharacter = CVar::Register("Sound_ListenerAtCharacter", "lock listener at character", 0, "1", NULL, 7, false, NULL, false);
    SI3::sm_pCVar_Sound_EnableEmoteSounds = CVar::Register("Sound_EnableEmoteSounds", "", 0, "1", NULL, 7, false, NULL, false);
    SI3::sm_pCVar_Sound_ZoneMusicNoDelay = CVar::Register("Sound_ZoneMusicNoDelay", "", 0, "0", NULL, 7, false, NULL, false);
    SI3::sm_pCVar_Sound_EnableArmorFoleySoundForSelf = CVar::Register("Sound_EnableArmorFoleySoundForSelf", "", 0, "1", NULL, 7, false, NULL, false);
    SI3::sm_pCVar_Sound_EnableArmorFoleySoundForOthers = CVar::Register("Sound_EnableArmorFoleySoundForOthers", "", 0, "1", NULL, 7, false, NULL, false);
    SI3::sm_pCVar_Sound_EnableDSPEffects = CVar::Register("Sound_EnableDSPEffects", "", 0, OsGetNumberOfProcessors() <= 1 ? "0" : "1", NULL, 7, false, NULL, false);
    SI3::sm_pCVar_Sound_EnablePetSounds = CVar::Register("Sound_EnablePetSounds", "Enables pet sounds", 0, "1", NULL, 7, false, NULL, false);
    SI3::sm_pCVar_Sound_MaxCacheSizeInBytes = CVar::Register("Sound_MaxCacheSizeInBytes", "Max cache size in bytes", 0, "16777216", MaxCacheSizeInBytesHandler, 7, false, NULL, false);
    SI3::sm_pCVar_Sound_MaxCacheableSizeInBytes = CVar::Register("Sound_MaxCacheableSizeInBytes", "Max sound size that will be cached, larger files will be streamed instead", 0, "1048576", MaxCacheableSizeInBytesHandler, 7, false, NULL, false);
    SI3::sm_pCVar_Sound_EnableEmoteSounds = CVar::Register("Sound_EnableEmoteSounds", "play footstep sounds", 0, "1", NULL, 7, false, NULL, false);
    SI3::sm_pCVar_Sound_FootstepSounds = CVar::Register("FootstepSounds", "play footstep sounds", 0, "1", NULL, 7, false, NULL, false);
    SI3::sm_pCVar_Sound_EnableSoundWhenGameIsInBG = CVar::Register("Sound_EnableSoundWhenGameIsInBG", "Enable Sound When Game Is In Background", 0, "0", NULL, 7, false, NULL, false);

    SE3::Log_Write(" ");
    SE3::Log_Write("=> User Settings Report:");
    SE3::Log_Write(" - ========= PLAYBACK =========");
    SE3::Log_Write(" - Enable All Sound         [%d]", SI3::sm_pCVar_Sound_EnableAllSound->m_intValue);
    SE3::Log_Write(" - Enable SFX               [%d]", SI3::sm_pCVar_Sound_EnableSFX->m_intValue);
    SE3::Log_Write(" --- Enable Error Speech    [%d]", SI3::sm_pCVar_Sound_EnableErrorSpeech->m_intValue);
    SE3::Log_Write(" --- Enable Emote Sounds    [%d]", SI3::sm_pCVar_Sound_EnableEmoteSounds->m_intValue);
    SE3::Log_Write(" - Enable Music             [%d]", SI3::sm_pCVar_Sound_EnableMusic->m_intValue);
    SE3::Log_Write(" --- Loop Music             [%d]", SI3::sm_pCVar_Sound_ZoneMusicNoDelay->m_intValue);
    SE3::Log_Write(" - Enable Ambient Sounds    [%d]", SI3::sm_pCVar_Sound_EnableAmbience->m_intValue);
    SE3::Log_Write(" - Sound at Character       [%d]", SI3::sm_pCVar_Sound_ListenerAtCharacter->m_intValue);
    SE3::Log_Write(" - Sound in Background      [%d]", SI3::sm_pCVar_Sound_EnableSoundWhenGameIsInBG->m_intValue);
    SE3::Log_Write(" - Enable Reverb            [%d]", SI3::sm_pCVar_Sound_EnableReverb->m_intValue);
    SE3::Log_Write(" - Enable Software HRTF     [%d]", SI3::sm_pCVar_Sound_EnableSoftwareHRTF->m_intValue);
    SE3::Log_Write(" - Sound Quality            [%d]", SI3::sm_pCVar_Sound_OutputQuality->m_intValue);
    SE3::Log_Write(" -");
    SE3::Log_Write(" - ========== VOLUME ==========");
    SE3::Log_Write(" - Master Volume         [%.2f]", SI3::sm_pCVar_Sound_MasterVolume->m_floatValue);
    SE3::Log_Write(" - SFX Volume            [%.2f]", SI3::sm_pCVar_Sound_SFXVolume->m_floatValue);
    SE3::Log_Write(" - Music Volume          [%.2f]", SI3::sm_pCVar_Sound_MusicVolume->m_floatValue);
    SE3::Log_Write(" - Ambience Volume       [%.2f]", SI3::sm_pCVar_Sound_AmbienceVolume->m_floatValue);
    SE3::Log_Write(" -");
    SE3::Log_Write(" - =========== MISC ===========");
    SE3::Log_Write(" - Sound Channels          [%d]", SI3::sm_pCVar_Sound_NumChannels->m_intValue);
    SE3::Log_Write(" - Use Hardware             [%d]", SI3::sm_pCVar_Sound_EnableHardware->m_intValue);
    SE3::Log_Write(" - Mix Mode 2               [%d]", SI3::sm_pCVar_Sound_EnableMixMode2->m_intValue);

    if (SI3::sm_pCVar_Sound_DSPBufferSize->m_intValue) {
        SE3::Log_Write(" - DSP Buffer Size          [%d]", SI3::sm_pCVar_Sound_DSPBufferSize->m_intValue);
    } else {
        SE3::Log_Write(" - DSP Buffer Size       [AUTO]");
    }

    SE3::Log_Write(" - Armor Foley SFX (Self)   [%d]", SI3::sm_pCVar_Sound_EnableArmorFoleySoundForSelf->m_intValue);
    SE3::Log_Write(" - Armor Foley SFX (Others) [%d]", SI3::sm_pCVar_Sound_EnableArmorFoleySoundForOthers->m_intValue);
    SE3::Log_Write("=> End of User Settings Report");
    SE3::Log_Write(" ");
}

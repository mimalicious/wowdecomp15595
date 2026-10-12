#include "ComSatClient.h"
#include "Engine/Source/Base/CDataStore.h"
#include "Engine/Source/ConsoleBase/ConsoleVar.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/Storm/H/storm.h"
#include "WoW/Source/Game/GameClient/ComSatThread.h"
#include "WoW/Source/Net/Types.h"
#include "WoW/Source/SoundInterface/SI3.h"
#include "WoW/Source/Ui/ChatFrame.h"
#include "WoW/Source/Ui/GameUI.h"
#include "WoW/Source/Ui/ScriptEvents.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"

// STATIC: 0x00DC6FA8/0x140CACB70
CVar* s_cvarEnableVoiceChat;

// STATIC: 0x00DC6FAC/0x140CACB78
CVar* s_cvarEnableMicrophone;

// OFFSET: 0x004DC120/0x14011A590
bool EnableMicrophoneHandler(CVar* var, const char* oldValue, const char* value, void* arg) {
    if (!s_cvarEnableVoiceChat) {
        return true;
    }

    int32_t voiceChatEnabled = s_cvarEnableVoiceChat->m_intValue;
    int32_t microphoneEnabled = SStrToInt(value);

    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_VOICE_SESSION_ENABLE));
    msg.Put(static_cast<uint8_t>(voiceChatEnabled != 0));
    msg.Put(static_cast<uint8_t>(microphoneEnabled != 0));
    msg.Finalize();
    ClientServices::Send2(&msg);

    if (voiceChatEnabled && microphoneEnabled && CGChat::IsVoiceChatAllowed()) {
        SI3::VoiceChat_StartCapture();
    } else {
        SI3::VoiceChat_StopCapture();
    }

    return true;
}

// OFFSET: 0x004DC8A0/0x14011AF30
bool InboundChatVolumeHandler(CVar* var, const char* oldValue, const char* value, void* arg) {
    float volume = SStrToFloat(value);

    if (ComSatThread::s_comsat) {
        for (COMSATDATA* data = s_comsatData; data < s_comsatData + sizeof(s_comsatData) / sizeof(s_comsatData[0]); data++) {
            if (data->m_sessionID) {
                AddComSatEvent(1, volume, data->m_sessionID);
            }
        }
    }

    return true;
}

// PROVEN: fe6fc487 IDENTICAL/UNPROVEN
// OFFSET: 0x004DC900/0x14011AFA0
bool VoiceChatModeHandler(CVar* var, const char* oldValue, const char* value, void* arg) {
    if (SStrToInt(value) == 1) {
        s_voiceActivation = 1;
        Script_SendUniqueSignal(SCRIPT_EVENT_VOICE_SELF_MUTE);

        CVar* selfMute = CVar::Lookup("VoiceChatSelfMute");

        if (selfMute && selfMute->m_intValue) {
            AddComSatEvent(2, 0u, 0);
        } else {
            AddComSatEvent(2, 1u, 0);
        }

        CVar* sensitivity = CVar::Lookup("VoiceActivationSensitivity");

        if (sensitivity) {
            float level = sensitivity->m_floatValue;

            level = level < 0.0f ? 0.0f : (level < 0.99f ? level : 0.99f);

            AddComSatEvent(3, level, 0);
        }
    } else {
        s_voiceActivation = 0;
        AddComSatEvent(2, 0u, 0);
        AddComSatEvent(3, 0.0f, 0);
    }

    return true;
}

// PROVEN: e115f5fb DIFFERENT/IDENTICAL
// OFFSET: 0x004DC9D0/0x14011B060
bool VoiceActivationSensitivityHandler(CVar* var, const char* oldValue, const char* value, void* arg) {
    CVar* voiceChatMode = CVar::Lookup("VoiceChatMode");

    if (voiceChatMode && voiceChatMode->m_intValue == 1) {
        float level = SStrToFloat(value);

        if (level < 0.0f) {
            level = 0.0f;
        } else if (level >= 0.99f) {
            level = 0.99f;
        }

        AddComSatEvent(3, level, 0);
    }

    return true;
}

// PROVEN: bc07ee84 IDENTICAL/IDENTICAL
// OFFSET: 0x004DCA40/0x14011B0C0
bool PushToTalkButtonHandler(CVar* var, const char* oldValue, const char* value, void* arg) {
    AddComSatEvent(10, 0u, 0);

    return true;
}

// OFFSET: 0x004DCA60/0x14011B0E0
bool SelfMuteHandler(CVar* var, const char* oldValue, const char* value, void* arg) {
    if (SStrToInt(value)) {
        AddComSatEvent(2, 0u, 0);
    } else {
        AddComSatEvent(2, 1u, 0);
    }

    Script_SendUniqueSignal(SCRIPT_EVENT_VOICE_SELF_MUTE);

    return true;
}

// OFFSET: 0x004DF970/0x14011EC40
bool EnableVoiceChatHandler(CVar* var, const char* oldValue, const char* value, void* arg) {
    bool wasEnabled = oldValue && SStrToInt(oldValue);
    bool enabled = SStrToInt(value) != 0;
    bool microphoneEnabled = s_cvarEnableMicrophone && s_cvarEnableMicrophone->m_intValue;

    // STATIC: 0x00DC7090/0x140CACC38
    static CVar* s_voiceChatMode = CVar::Lookup("VoiceChatMode");

    if (!wasEnabled && enabled) {
        ComSatClient_Init();

        if (s_voiceChatMode) {
            DebugLog("!!!!!!!!!! HACK\n");
            s_voiceChatMode->Set(s_voiceChatMode->m_intValue ? "0" : "1", true, false, false, true);
            s_voiceChatMode->Set(s_voiceChatMode->m_intValue ? "0" : "1", true, false, false, true);
        }
    }

    if (s_cvarEnableMicrophone) {
        CDataStore msg;
        msg.Put(static_cast<uint32_t>(CMSG_VOICE_SESSION_ENABLE));
        msg.Put(static_cast<uint8_t>(enabled));
        msg.Put(static_cast<uint8_t>(microphoneEnabled));
        msg.Finalize();
        ClientServices::Send2(&msg);
    }

    if (enabled) {
        s_voiceChatWasEnabled = 1;
    }

    if (CGGameUI::m_initialized) {
        FrameScript_SignalEvent(SCRIPT_EVENT_VOICE_CHAT_ENABLED_UPDATE, NULL);
    }

    if (enabled && microphoneEnabled && CGChat::IsVoiceChatAllowed()) {
        SI3::VoiceChat_StartCapture();
    } else {
        SI3::VoiceChat_StopCapture();
    }

    if (wasEnabled && !enabled) {
        ComSatClient_Shutdown();
    }

    return true;
}

bool OutboundChatVolumeHandler(CVar* var, const char* oldValue, const char* value, void* arg) {
    return true;
}

#include "CKBSystem.h"
#include "Engine/Source/Base/CDataStore.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "WoW/Source/Ui/ChatFrame.h"
#include "WoW/Source/Ui/Types.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"

// STATIC: 0x00F76B64/0x140E6CC88
CDynamicString s_motd;

// STATIC: 0x00F76B70/0x140E6CC98
CDynamicString s_serverStatus;

// STATIC: 0x00F76B7C/0x140E6CCA8
CDynamicString s_serverNotice;

// PROVEN: c11e0599 IDENTICAL/UNPROVEN
// OFFSET: 0x009FBBB0/0x1407991F0
static int32_t Packet_SMSG_MOTD(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    if (msgId != SMSG_MOTD) {
        return 0;
    }

    if (!msg) {
        return 0;
    }

    uint32_t lineCount;
    msg->Get(lineCount);

    s_motd.Free();

    for (int32_t i = 0; i < static_cast<int32_t>(lineCount); i++) {
        if (i > 0) {
            s_motd += '\n';
        }

        char line[100];
        msg->GetString(line, sizeof(line));
        ChatMessageParams params;
        params.m_flags = "MOTD";
        CGChat::AddChatMessage(line, static_cast<SLASH_COMMAND_ID>(0), &params);
        s_motd += line;
    }

    FrameScript_SignalEvent(SCRIPT_EVENT_KNOWLEDGE_BASE_SYSTEM_MOTD_UPDATED, NULL);

    return 1;
}

// OFFSET: 0x009FC980/0x14079A590
void CKBSystem::Initialize() {
    ClientServices::SetMessageHandler(SMSG_MOTD, Packet_SMSG_MOTD, NULL);
}

// PROVEN: 72b9e0cc IDENTICAL/UNPROVEN
// OFFSET: 0x009FC9A0/0x14079A5C0
void CKBSystem::Shutdown() {
    s_motd.Free();
    s_serverStatus.Free();
    s_serverNotice.Free();
    ClientServices::ClearMessageHandler(SMSG_MOTD);
}

// OFFSET: 0x009FBB70
void CKBSystem::CacheServerMessage(const char* text, int32_t id) {
    if (id == 3) {
        s_serverNotice = text;
    } else {
        s_serverStatus = text;
    }

    FrameScript_SignalEvent(SCRIPT_EVENT_KNOWLEDGE_BASE_SERVER_MESSAGE, NULL);
}

// PROVEN: 687699c0 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void CKBSystem::LeaveWorld() {
}

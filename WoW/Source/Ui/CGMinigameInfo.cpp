#include "CGMinigameInfo.h"
#include "Engine/Source/Base/CDataStore.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "WoW/Source/Minigame/Minigame_c.h"
#include "WoW/Source/Ui/Types.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"

// STATIC: 0x00F771A8
CMinigame_C* CGMinigameInfo::m_minigame;

// OFFSET: #0x005C5720
void CGMinigameInfo::InitializeGame() {
    ClientServices::SetMessageHandler(SMSG_MINIGAME_SETUP, Packet_SMSG_MINIGAME_SETUP, NULL);
    ClientServices::SetMessageHandler(SMSG_MINIGAME_STATE, Packet_SMSG_MINIGAME_STATE, NULL);
}

// OFFSET: #0x005C54B0
void CGMinigameInfo::ShutdownGame() {
    ClientServices::ClearMessageHandler(SMSG_MINIGAME_SETUP);
    ClientServices::ClearMessageHandler(SMSG_MINIGAME_STATE);
}

// OFFSET: #0x005C54D0
int32_t Packet_SMSG_MINIGAME_STATE(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    if (CGMinigameInfo::m_minigame) {
        CGMinigameInfo::m_minigame->UpdateState(msg);
    }

    FrameScript_SignalEvent(SCRIPT_EVENT_MINIGAME_UPDATE, NULL);
    return 1;
}

// OFFSET: #0x005C5660
void CGMinigameInfo::LeaveWorld() {
    if (CGMinigameInfo::m_minigame) {
        delete CGMinigameInfo::m_minigame;
        CGMinigameInfo::m_minigame = NULL;
    }
}

// OFFSET: #0x005C5690
int32_t Packet_SMSG_MINIGAME_SETUP(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    uint64_t guid;
    uint8_t type;
    msg->Get(guid);
    msg->Get(type);

    if (CGMinigameInfo::m_minigame) {
        delete CGMinigameInfo::m_minigame;
        CGMinigameInfo::m_minigame = NULL;
    }

    CGMinigameInfo::m_minigame = CMinigame_C::CreateMinigame(static_cast<MINIGAME_TYPE>(type), guid);
    CGMinigameInfo::m_minigame->UpdateSetup(msg);
    FrameScript_SignalEvent(SCRIPT_EVENT_START_MINIGAME, NULL);
    FrameScript_SignalEvent(SCRIPT_EVENT_MINIGAME_UPDATE, NULL);
    return 1;
}

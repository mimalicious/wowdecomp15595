#include "DuelInfo.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "WoW/Source/Ui/Types.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"
#include <cstdint>
#include "Engine/Source/Base/CDataStore.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/Event/Event.h"
#include "WoW/Source/Client.h"
#include "WoW/Source/Ui/GameUI.h"
#include "WoW/Source/Ui/ChatFrame.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/FriendList.h"

// STATIC: 0x00F76880/0x140E6C918
static WOWGUID s_duelArbiter;

// STATIC: 0x00F76888/0x140E6C920
WOWGUID CGDuelInfo::m_dueler;

// STATIC: 0x00F76890/0x140E6C928
static uint32_t s_countdown;

// STATIC: 0x00F76894/0x140E6C92C
static uint32_t s_countdownTimer;

// OFFSET: 0x009F0760/0x14078A5F0
int32_t Packet_SMSG_DUEL_COUNTDOWN(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    uint32_t countdown;
    msg->Get(countdown);
    s_countdown = countdown / 1000;
    CGDuelInfo::ProcessCountdown(NULL, NULL);
    return 1;
}

// OFFSET: 0x009F07A0/0x14078A630
int32_t Packet_SMSG_DUEL_COMPLETE(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    uint8_t completed;
    msg->Get(completed);

    if (s_duelArbiter) {
        if (!completed) {
            CGGameUI::DisplayError(ERR_DUEL_CANCELLED);
        }

        s_duelArbiter.m_guid = 0;
        FrameScript_SignalEvent(SCRIPT_EVENT_DUEL_FINISHED, NULL);
    }

    if (s_countdownTimer) {
        ClientKillTimer(s_countdownTimer, CGDuelInfo::ProcessCountdown, "ProcessCountdown");
        s_countdownTimer = 0;
    }

    CGDuelInfo::m_dueler.m_guid = 0;
    return 1;
}

// OFFSET: 0x009F0830/0x14078A6B0
int32_t Packet_SMSG_DUEL_WINNER(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    uint8_t retreat;
    char winner[48];
    char loser[48];
    msg->Get(retreat);
    msg->GetString(winner, sizeof(winner));
    msg->GetString(loser, sizeof(loser));
    const char* format;

    if (retreat) {
        format = FrameScript_GetText("DUEL_WINNER_RETREAT", -1, GENDER_NOT_APPLICABLE);
    } else {
        format = FrameScript_GetText("DUEL_WINNER_KNOCKOUT", -1, GENDER_NOT_APPLICABLE);
    }

    char buffer[3000];
    SStrPrintf(buffer, sizeof(buffer), format, winner, loser);
    CGChat::AddChatMessage(buffer, static_cast<SLASH_COMMAND_ID>(0), NULL);
    return 1;
}

// OFFSET: 0x009F08C0/0x14078A760
void CGDuelInfo::AcceptDuel() {
    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_DUEL_ACCEPTED));
    msg.Put(s_duelArbiter.m_guid);
    msg.Finalize();
    ClientServices::Send2(&msg);
}

// OFFSET: 0x009F0940/0x14078A7F0
void CGDuelInfo::CancelDuel() {
    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_DUEL_CANCELLED));
    msg.Put(s_duelArbiter.m_guid);
    msg.Finalize();
    ClientServices::Send2(&msg);
}

// OFFSET: 0x009F09C0/0x14078A880
int32_t Packet_SMSG_DUEL_REQUESTED(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    msg->Get(s_duelArbiter.m_guid);
    WOWGUID requester;
    msg->Get(requester.m_guid);

    if (requester == ClntObjMgrGetActivePlayer()) {
        CGGameUI::DisplayError(ERR_DUEL_REQUESTED);
        CGDuelInfo::AcceptDuel();
        return 1;
    }

    if (FriendList::s_friendList->IsIgnored(requester, true)) {
        CGDuelInfo::CancelDuel();
        s_duelArbiter.m_guid = 0;
        return 1;
    }

    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(requester, TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player) {
        FrameScript_SignalEvent(SCRIPT_EVENT_DUEL_REQUESTED, "%s", player->GetUnitName(NULL, 1));
    }

    CGDuelInfo::m_dueler = requester;
    return 1;
}

// OFFSET: 0x009F0AC0/0x14078A980
void CGDuelInfo::InitializeGame() {
    ClientServices::SetMessageHandler(SMSG_DUEL_REQUESTED, Packet_SMSG_DUEL_REQUESTED, NULL);
    ClientServices::SetMessageHandler(SMSG_DUEL_OUT_OF_BOUNDS, Packet_SMSG_DUEL_OUTOFBOUNDS, NULL);
    ClientServices::SetMessageHandler(SMSG_DUEL_IN_BOUNDS, Packet_SMSG_DUEL_INBOUNDS, NULL);
    ClientServices::SetMessageHandler(SMSG_DUEL_COUNTDOWN, Packet_SMSG_DUEL_COUNTDOWN, NULL);
    ClientServices::SetMessageHandler(SMSG_DUEL_COMPLETE, Packet_SMSG_DUEL_COMPLETE, NULL);
    ClientServices::SetMessageHandler(SMSG_DUEL_WINNER, Packet_SMSG_DUEL_WINNER, NULL);
}

// OFFSET: 0x009F0500/0x14078A310
int32_t CGDuelInfo::ProcessCountdown(const void* data, void* param) {
    char buffer[3000];
    SStrPrintf(buffer, sizeof(buffer), FrameScript_GetText("DUEL_COUNTDOWN", -1, GENDER_NOT_APPLICABLE), s_countdown);
    CGChat::AddChatMessage(buffer, static_cast<SLASH_COMMAND_ID>(0), NULL);

    if (--s_countdown) {
        s_countdownTimer = ClientSetTimer(1000, CGDuelInfo::ProcessCountdown, NULL);
    } else {
        s_countdownTimer = 0;
    }

    return 1;
}

// OFFSET: 0x009F0590/0x14078A3A0
int32_t Packet_SMSG_DUEL_OUTOFBOUNDS(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    FrameScript_SignalEvent(SCRIPT_EVENT_DUEL_OUTOFBOUNDS, NULL);
    return 1;
}

// OFFSET: 0x009F05B0/0x14078A3C0
int32_t Packet_SMSG_DUEL_INBOUNDS(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    FrameScript_SignalEvent(SCRIPT_EVENT_DUEL_INBOUNDS, NULL);
    return 1;
}

// OFFSET: 0x009F06A0/0x14078A510
void CGDuelInfo::ShutdownGame() {
    ClientServices::ClearMessageHandler(SMSG_DUEL_REQUESTED);
    ClientServices::ClearMessageHandler(SMSG_DUEL_OUT_OF_BOUNDS);
    ClientServices::ClearMessageHandler(SMSG_DUEL_IN_BOUNDS);
    ClientServices::ClearMessageHandler(SMSG_DUEL_COUNTDOWN);
    ClientServices::ClearMessageHandler(SMSG_DUEL_COMPLETE);
    ClientServices::ClearMessageHandler(SMSG_DUEL_WINNER);
}

// OFFSET: 0x009F0710/0x14078A5A0
void CGDuelInfo::LeaveWorld() {
    if (!CGGameUI::m_currentlyReloadingUI) {
        s_duelArbiter.m_guid = 0;
    }

    if (s_countdownTimer) {
        ClientKillTimer(s_countdownTimer, CGDuelInfo::ProcessCountdown, "ProcessCountdown");
        s_countdownTimer = 0;
    }
}

#include "CGBankInfo.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/Ui/Types.h"
#include "WoW/Source/Ui/GameUI.h"
#include "WoW/Source/Ui/GossipInfo.h"

// STATIC: 0x00F76860
WOWGUID CGBankInfo::m_banker;

// OFFSET: 0x009EE710/0x140787D10
static int32_t BankUpdateHandler(WOWGUID guid, uint32_t fieldOffset, uint32_t fieldSize, const void* oldValue, void* param) {
    FrameScript_SignalEvent(SCRIPT_EVENT_PLAYERBANKBAGSLOTS_CHANGED, NULL);
    return 1;
}

// OFFSET: 0x009EE8C0/0x140787EF0
void CGBankInfo::EnterWorld() {
    ClntObjMgrSetObjMirrorHandler(ClntObjMgrGetActivePlayer(), ID_PLAYER, 0x1E, 1, BankUpdateHandler, NULL, static_cast<HANDLER_PRIORITY>(0), 0);
}

// OFFSET: 0x009EE730/0x140787D30
void CGBankInfo::SetBanker(const WOWGUID& banker) {
    if (!banker) {
        FrameScript_SignalEvent(SCRIPT_EVENT_BANKFRAME_CLOSED, NULL);
        CGGameUI::ClearInteractTarget(CGBankInfo::m_banker, 0);
        CGBankInfo::m_banker.m_guid = 0;
        return;
    }

    CGGossipInfo::ClearGossip();
    CGGameUI::SetInteractTarget(banker);
    CGBankInfo::m_banker = banker;
    FrameScript_SignalEvent(SCRIPT_EVENT_BANKFRAME_OPENED, NULL);
}

// OFFSET: 0x009EE8F0/0x140787F30
void CGBankInfo::LeaveWorld() {
    ClntObjMgrUnsetObjMirrorHandler(ClntObjMgrGetActivePlayer(), ID_PLAYER, 0x1E, BankUpdateHandler, NULL);
}

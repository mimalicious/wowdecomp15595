#include "CGMailFrame.h"
#include <cstring>
#include "Engine/Source/FrameScript/FrameScript.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/Ui/GameUI.h"
#include "WoW/Source/Ui/GossipInfo.h"
#include "WoW/Source/Ui/MailInfo.h"
#include "WoW/Source/Ui/Types.h"

// OFFSET: 0x0096D7A0/0x1406E4720
void CGMailInfo::ClearSendMail(bool signal) {
    for (uint32_t i = 0; i < 16; i++) {
        if (CGMailInfo::m_sendItems[i].m_guid) {
            CGGameUI::UnlockItem(CGMailInfo::m_sendItems[i].m_guid);
        }
    }

    memset(CGMailInfo::m_sendItems, 0, sizeof(((CGMailInfo*)0)->m_sendItems));
    CGMailInfo::m_sendMoney = 0;
    CGMailInfo::m_lockSendItems = 0;
    CGMailInfo::m_sendCOD = 0;
    CGMailInfo::m_sendPending = 0;
    CGMailInfo::m_selectedStationery = 0;
    CGMailInfo::m_selectedPackage = 0;

    if (signal) {
        FrameScript_SignalEvent(SCRIPT_EVENT_SEND_MAIL_MONEY_CHANGED, NULL);
        FrameScript_SignalEvent(SCRIPT_EVENT_SEND_MAIL_COD_CHANGED, NULL);
        FrameScript_SignalEvent(SCRIPT_EVENT_MAIL_SEND_SUCCESS, NULL);
    }
}

// OFFSET: 0x0096E030/0x1406E5110
void CGPlayer_C::TalkToMailbox(WOWGUID guid) {
    CGGossipInfo::ClearGossip();
    CGGameUI::SetInteractTarget(guid);
    CGMailInfo::m_object = guid;
    CGMailInfo::ClearSendMail(false);
    FrameScript_SignalEvent(SCRIPT_EVENT_MAIL_SHOW, NULL);
}

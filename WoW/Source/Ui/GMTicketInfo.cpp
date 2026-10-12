#include "GMTicketInfo.h"
#include "Engine/Source/Base/CDataStore.h"
#include "WoW/Source/Net/Types.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"
#include <cstring>
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/Storm/H/storm.h"
#include "WoW/Source/Ui/GameUI.h"
#include "WoW/Source/Ui/Types.h"
#include <ctime>
#include <malloc.h>
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/Ui/ChatFrame.h"

// STATIC: 0x00F76304/0x140E6C300
int32_t CGGMTicketInfo::m_ticketID;

// STATIC: 0x00F76308/0x140E6C304
int32_t CGGMTicketInfo::m_responseID;

// STATIC: 0x00D169F0/0x140BD0F70
uint8_t CGGMTicketInfo::s_quickTicketSystemStatus = 1;

// STATIC: 0x00D169F1/0x140BD0F71
uint8_t CGGMTicketInfo::s_itemRestorationButtonStatus = 1;

// STATIC: 0x00D16A98/0x140BD10C0
ThrottleObject CGGMTicketInfo::s_quickTicketThrottle(0, 1);

// PROVEN: 99963cea IDENTICAL/IDENTICAL
// OFFSET: 0x009E5700/0x14077DA60
void CGGMTicketInfo::SetQuickTicketSystemStatus(bool enabled) {
    if (static_cast<uint8_t>(enabled) != CGGMTicketInfo::s_quickTicketSystemStatus) {
        CGGMTicketInfo::s_quickTicketSystemStatus = enabled;
        FrameScript_SignalEvent(SCRIPT_EVENT_QUICK_TICKET_SYSTEM_STATUS, NULL);
    }
}

// PROVEN: 39e350b0 IDENTICAL/DIFFERENT
// OFFSET: 0x009E5730/0x14077DA90
void CGGMTicketInfo::SetQuickTicketThrottle(const ThrottleObject* throttle) {
    ThrottleObject saved = *throttle;
    CGGMTicketInfo::s_quickTicketThrottle.Restore(saved);
    FrameScript_SignalEvent(SCRIPT_EVENT_QUICK_TICKET_THROTTLE_CHANGED, NULL);
}

// PROVEN: 5062c212 IDENTICAL/IDENTICAL
// OFFSET: 0x009E5780/0x14077DAE0
void CGGMTicketInfo::SetItemRestorationButtonStatus(bool enabled) {
    if (static_cast<uint8_t>(enabled) != CGGMTicketInfo::s_itemRestorationButtonStatus) {
        CGGMTicketInfo::s_itemRestorationButtonStatus = enabled;
        FrameScript_SignalEvent(SCRIPT_EVENT_ITEM_RESTORATION_BUTTON_STATUS, NULL);
    }
}

// PROVEN: 24dd1cb4 IDENTICAL/IDENTICAL
// OFFSET: 0x009E5670/0x14077D9C0
void CGGMTicketInfo::EnterWorld() {
    CGGMTicketInfo::m_ticketID = 0;
    CGGMTicketInfo::m_responseID = 0;
}

// OFFSET: 0x009E7000/0x14077F890
void CGGMTicketInfo::InitializeGame() {
    ClientServices::SetMessageHandler(SMSG_GM_TICKET_CREATE, Packet_Group_5, NULL);
    ClientServices::SetMessageHandler(SMSG_GM_TICKET_UPDATE_TEXT, Packet_Group_5, NULL);
    ClientServices::SetMessageHandler(SMSG_GM_TICKET_GET_TICKET, Packet_Group_5, NULL);
    ClientServices::SetMessageHandler(SMSG_GM_TICKET_DELETE_TICKET, Packet_Group_5, NULL);
    ClientServices::SetMessageHandler(SMSG_GMRESPONSE_DB_ERROR, Packet_Group_5, NULL);
    ClientServices::SetMessageHandler(SMSG_GMRESPONSE_RECEIVED, Packet_Group_5, NULL);
    ClientServices::SetMessageHandler(SMSG_GMRESPONSE_STATUS_UPDATE, Packet_Group_5, NULL);
    ClientServices::SetMessageHandler(SMSG_GM_TICKET_STATUS_UPDATE, Packet_Group_5, NULL);
    CGGMTicketInfo::s_quickTicketSystemStatus = 1;
    CGGMTicketInfo::s_itemRestorationButtonStatus = 1;
}

// OFFSET: 0x009E5680/0x14077D9D0
static int32_t IsTicketTextValid(const char* text) {
    if (SStrLenUTF8(text) > 500) {
        CGGameUI::DisplayError(ERR_TICKET_TEXT_TOO_LONG);
        return 0;
    }

    char token[2000];
    const char* string = text;
    SStrTokenize(&string, token, sizeof(token), " \t\r\n", NULL);

    if (!token[0]) {
        CGGameUI::DisplayError(ERR_TICKET_NO_TEXT);
        return 0;
    }

    return 1;
}

// OFFSET: 0x009E5FC0/0x14077E520
void CGGMTicketInfo::CreateTicket(const char* text, int32_t needResponse, uint32_t moreHelp) {
    if (moreHelp) {
        if (!CGGMTicketInfo::m_responseID) {
            return;
        }
    } else if (CGGMTicketInfo::m_ticketID) {
        return;
    }

    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player || !IsTicketTextValid(text)) {
        return;
    }

    C3Vector position = player->GetPosition();
    int32_t continent = CGPlayer_C::GetNewContinentID();
    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_GM_TICKET_CREATE));
    msg.Put(static_cast<uint32_t>(continent));
    msg.Put(position.x);
    msg.Put(position.y);
    msg.Put(position.z);
    msg.PutString(text);
    msg.Put(static_cast<uint32_t>(needResponse ? 17 : 1));
    msg.Put(static_cast<uint8_t>(moreHelp != 0));
    uint32_t lines = 0;
    char* chat = static_cast<char*>(SMemAlloc(180000, __FILE__, __LINE__, 0));
    *chat = '\0';

    for (int32_t i = 0; i < 60; i++) {
        ChatRecord* record = GetChatRecord(i);

        if (record && record->m_description[0]) {
            SStrPack(chat, record->m_description, 180000);
            SStrPack(chat, "\n", 180000);
            lines++;
        }
    }

    msg.Put(lines);
    uint32_t now = static_cast<uint32_t>(::time(NULL));

    for (int32_t i = 0; i < 60; i++) {
        ChatRecord* record = GetChatRecord(i);

        if (record && record->m_description[0]) {
            msg.Put(now - record->m_time);
        }
    }

    if (strlen(chat)) {
        uint32_t length = static_cast<uint32_t>(strlen(chat)) + 1;
        uint32_t size = length / 1000 + length + 13;
        uint8_t* compressed = static_cast<uint8_t*>(alloca(size));
        zlib_compress(compressed, &size, reinterpret_cast<uint8_t*>(chat), length, -1);
        msg.Put(length);
        msg.PutData(compressed, size);
    } else {
        msg.Put(static_cast<uint32_t>(0));
    }

    SMemFree(chat, "delete[]", -1, 0);
    msg.Finalize();
    ClientServices::Send2(&msg);
}

// OFFSET: 0x009E62A0/0x14077E880
void SendGMTicketUpdateText(const char* text) {
    if (!CGGMTicketInfo::m_ticketID || CGGMTicketInfo::m_responseID || !IsTicketTextValid(text)) {
        return;
    }

    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_GM_TICKET_UPDATE_TEXT));
    msg.PutString(text);
    msg.Finalize();
    ClientServices::Send2(&msg);
}

// OFFSET: 0x009E63B0/0x14077E9D0
void SendGMTicketDeleteTicket() {
    if (!CGGMTicketInfo::m_ticketID || CGGMTicketInfo::m_responseID) {
        return;
    }

    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_GM_TICKET_DELETE_TICKET));
    msg.Finalize();
    ClientServices::Send2(&msg);
}

// OFFSET: 0x009E6430/0x14077EA70
void SendGMResponseResolve() {
    if (!CGGMTicketInfo::m_responseID) {
        return;
    }

    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_GM_TICKET_RESPONSE_RESOLVE));
    msg.Finalize();
    ClientServices::Send2(&msg);
}

// OFFSET: 0x009E6670/0x14077ED30
void CGGMReportLag::Submit(int32_t type) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return;
    }

    C3Vector position = player->GetPosition();
    int32_t continent = CGPlayer_C::GetNewContinentID();
    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_GM_LAG_REPORT));
    msg.Put(static_cast<uint32_t>(type - 1));
    msg.Put(static_cast<uint32_t>(continent));
    msg.Put(position.x);
    msg.Put(position.y);
    msg.Put(position.z);
    msg.Finalize();
    ClientServices::Send2(&msg);
}

// OFFSET: 0x009E6340/0x14077E950
void SendGetTicketOpcode() {
    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_GM_TICKET_GET_TICKET));
    msg.Finalize();
    ClientServices::Send2(&msg);
}

// OFFSET: 0x009E6C10/0x14077F3C0
int32_t Packet_Group_5(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    switch (msgId) {
        case SMSG_GM_TICKET_CREATE:
        case SMSG_GM_TICKET_UPDATE_TEXT: {
            uint32_t result;
            msg->Get(result);

            switch (result) {
                case 1:
                    CGGameUI::DisplayError(ERR_TICKET_ALREADY_EXISTS);
                    break;

                case 2:
                case 4:
                    SendGetTicketOpcode();
                    break;

                case 3:
                    CGGameUI::DisplayError(ERR_TICKET_CREATE_ERROR);
                    break;

                case 5:
                    CGGameUI::DisplayError(ERR_TICKET_UPDATE_ERROR);
                    break;
            }

            break;
        }

        case SMSG_GM_TICKET_GET_TICKET: {
            uint32_t status;
            msg->Get(status);

            if (status != 6) {
                if (!status) {
                    CGGameUI::DisplayError(ERR_TICKET_DB_ERROR);
                }

                FrameScript_SignalEvent(SCRIPT_EVENT_UPDATE_TICKET, NULL);
                break;
            }

            CGGMTicketInfo::m_responseID = 0;
            msg->Get(reinterpret_cast<uint32_t&>(CGGMTicketInfo::m_ticketID));
            char text[2000];
            msg->GetString(text, sizeof(text));
            uint8_t category;
            msg->Get(category);
            uint32_t ticketAge;
            msg->Get(ticketAge);
            uint32_t oldestTicketAge;
            msg->Get(oldestTicketAge);
            uint32_t lastUpdate;
            msg->Get(lastUpdate);
            uint8_t assigned;
            msg->Get(assigned);
            uint8_t opened;
            msg->Get(opened);
            char waitTimeOverrideMessage[1024] = { 0 };
            msg->GetString(waitTimeOverrideMessage, 0x7FFFFFFF);
            uint32_t waitTimeOverrideMinutes;
            msg->Get(waitTimeOverrideMinutes);
            FrameScript_SignalEvent(SCRIPT_EVENT_UPDATE_TICKET, "%d%s%d%d%d%d%d%s%d", category, text, ticketAge, oldestTicketAge, lastUpdate, assigned, opened, waitTimeOverrideMessage, waitTimeOverrideMinutes);
            break;
        }

        case SMSG_GM_TICKET_DELETE_TICKET: {
            uint32_t result;
            msg->Get(result);

            if (result == 9) {
                CGGMTicketInfo::m_ticketID = 0;
                CGGMTicketInfo::m_responseID = 0;
                FrameScript_SignalEvent(SCRIPT_EVENT_UPDATE_TICKET, NULL);
            } else {
                CGGameUI::DisplayError(ERR_TICKET_DB_ERROR);
            }

            break;
        }

        case SMSG_GM_TICKET_STATUS_UPDATE: {
            uint32_t status;
            msg->Get(status);

            switch (status) {
                case 1:
                case 4:
                    SendGetTicketOpcode();
                    break;

                case 2:
                    CGGMTicketInfo::m_ticketID = 0;
                    CGGMTicketInfo::m_responseID = 0;
                    FrameScript_SignalEvent(SCRIPT_EVENT_UPDATE_TICKET, NULL);
                    break;

                case 3:
                    CGGMTicketInfo::m_ticketID = 0;
                    CGGMTicketInfo::m_responseID = 0;
                    FrameScript_SignalEvent(SCRIPT_EVENT_GMSURVEY_DISPLAY, NULL);
                    break;
            }

            break;
        }

        case SMSG_GMRESPONSE_DB_ERROR:
            CGGameUI::DisplayError(ERR_GMRESPONSE_DB_ERROR);
            break;

        case SMSG_GMRESPONSE_RECEIVED: {
            msg->Get(reinterpret_cast<uint32_t&>(CGGMTicketInfo::m_responseID));
            msg->Get(reinterpret_cast<uint32_t&>(CGGMTicketInfo::m_ticketID));
            char text[2000];
            msg->GetString(text, sizeof(text));
            char response[16000] = { 0 };

            for (int32_t i = 0; i < 4; i++) {
                char line[4000];
                msg->GetString(line, sizeof(line));
                SStrPack(response, line, sizeof(response));
            }

            FrameScript_SignalEvent(SCRIPT_EVENT_GMRESPONSE_RECEIVED, "%s%s", text, response);
            break;
        }

        case SMSG_GMRESPONSE_STATUS_UPDATE: {
            CGGMTicketInfo::m_ticketID = 0;
            CGGMTicketInfo::m_responseID = 0;
            uint32_t showSurvey;
            msg->GetInt8(showSurvey);

            if (showSurvey) {
                FrameScript_SignalEvent(SCRIPT_EVENT_GMSURVEY_DISPLAY, NULL);
            } else {
                FrameScript_SignalEvent(SCRIPT_EVENT_UPDATE_TICKET, NULL);
            }

            break;
        }
    }

    return 1;
}

// PROVEN: ae89e004 IDENTICAL/IDENTICAL
// OFFSET: 0x009E5B50/0x14077DFA0
void CGGMTicketInfo::ShutdownGame() {
    ClientServices::ClearMessageHandler(SMSG_GM_TICKET_CREATE);
    ClientServices::ClearMessageHandler(SMSG_GM_TICKET_UPDATE_TEXT);
    ClientServices::ClearMessageHandler(SMSG_GM_TICKET_GET_TICKET);
    ClientServices::ClearMessageHandler(SMSG_GM_TICKET_DELETE_TICKET);
    ClientServices::ClearMessageHandler(SMSG_GMRESPONSE_DB_ERROR);
    ClientServices::ClearMessageHandler(SMSG_GMRESPONSE_RECEIVED);
    ClientServices::ClearMessageHandler(SMSG_GMRESPONSE_STATUS_UPDATE);
    ClientServices::ClearMessageHandler(SMSG_GM_TICKET_STATUS_UPDATE);
}

// PROVEN: db6a0166 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void CGGMTicketInfo::LeaveWorld() {
}

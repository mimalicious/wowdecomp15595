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

// STATIC: #0x00C1E8D0
int32_t CGGMTicketInfo::m_ticketID;

// STATIC: #0x00C1E8D4
int32_t CGGMTicketInfo::m_responseID;

// PROVEN: 24dd1cb4 IDENTICAL/NONE
// OFFSET: 0x009E5670/0x14077D9C0
void CGGMTicketInfo::EnterWorld() {
    CGGMTicketInfo::m_ticketID = 0;
    CGGMTicketInfo::m_responseID = 0;
}

// OFFSET: #0x005AD600
void CGGMTicketInfo::InitializeGame() {
    ClientServices::SetMessageHandler(SMSG_GM_TICKET_CREATE, Packet_Group_5, NULL);
    ClientServices::SetMessageHandler(SMSG_GM_TICKET_UPDATE_TEXT, Packet_Group_5, NULL);
    ClientServices::SetMessageHandler(SMSG_GM_TICKET_GET_TICKET, Packet_Group_5, NULL);
    ClientServices::SetMessageHandler(SMSG_GM_TICKET_DELETE_TICKET, Packet_Group_5, NULL);
    ClientServices::SetMessageHandler(SMSG_GMRESPONSE_DB_ERROR, Packet_Group_5, NULL);
    ClientServices::SetMessageHandler(SMSG_GMRESPONSE_RECEIVED, Packet_Group_5, NULL);
    ClientServices::SetMessageHandler(SMSG_GMRESPONSE_STATUS_UPDATE, Packet_Group_5, NULL);
    ClientServices::SetMessageHandler(SMSG_GM_TICKET_STATUS_UPDATE, Packet_Group_5, NULL);
}

// OFFSET: #0x005AC170
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

// OFFSET: #0x005AC8F0
void CGGMTicketInfo::CreateTicket(const char* text, int32_t needResponse, uint32_t moreHelp) {
    if (moreHelp) {
        if (!CGGMTicketInfo::m_responseID) {
            return;
        }
    } else if (CGGMTicketInfo::m_ticketID) {
        return;
    }

    CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();

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

// OFFSET: #0x005ACB50
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

// OFFSET: #0x005ACC60
void SendGMTicketDeleteTicket() {
    if (!CGGMTicketInfo::m_ticketID || CGGMTicketInfo::m_responseID) {
        return;
    }

    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_GM_TICKET_DELETE_TICKET));
    msg.Finalize();
    ClientServices::Send2(&msg);
}

// OFFSET: #0x005ACCE0
void SendGMResponseResolve() {
    if (!CGGMTicketInfo::m_responseID) {
        return;
    }

    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_GM_TICKET_RESPONSE_RESOLVE));
    msg.Finalize();
    ClientServices::Send2(&msg);
}

// OFFSET: #0x005ACF30
void CGGMReportLag::Submit(int32_t type) {
    CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();

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

// OFFSET: #0x005ACBF0
void SendGetTicketOpcode() {
    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_GM_TICKET_GET_TICKET));
    msg.Finalize();
    ClientServices::Send2(&msg);
}

// OFFSET: #0x005AD240
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
            float ticketAge;
            msg->Get(ticketAge);

            if (ticketAge < 0.0f) {
                ticketAge = 0.0f;
            }

            float oldestTicketAge;
            msg->Get(oldestTicketAge);
            float lastUpdate;
            msg->Get(lastUpdate);
            uint8_t assigned;
            msg->Get(assigned);
            uint8_t opened;
            msg->Get(opened);
            FrameScript_SignalEvent(SCRIPT_EVENT_UPDATE_TICKET, "%d%s%f%f%f%d%d", category, text, ticketAge, oldestTicketAge, lastUpdate, assigned, opened);
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
                SStrPack(response, line, 0x7FFFFFFF);
            }

            FrameScript_SignalEvent(SCRIPT_EVENT_GMRESPONSE_RECEIVED, "%s%s", text, response);
            break;
        }

        case SMSG_GMRESPONSE_STATUS_UPDATE: {
            CGGMTicketInfo::m_ticketID = 0;
            CGGMTicketInfo::m_responseID = 0;
            uint8_t showSurvey = 0;
            msg->Get(showSurvey);

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

// OFFSET: #0x005AC100
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

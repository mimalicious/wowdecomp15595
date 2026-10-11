#include "MailInfo.h"
#include <cmath>
#include <cstdlib>
#include <cstring>
#include "Engine/Source/Storm/H/storm.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/StationeryRec.h"
#include "WoW/Source/DB/Rec/ItemRecSparse.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "WoW/Source/Ui/Types.h"
#include "Engine/Source/Base/CDataStore.h"
#include "WoW/Source/DB/DBClient/DBCacheInstances.h"
#include "WoW/Source/Object/ItemStats.h"
#include "WoW/Source/Object/ObjectClient/Item_C.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "Engine/Source/Tempest/CMath.h"
#include "WoW/Source/Ui/GameUI.h"
#include "WoW/Source/Ui/GossipInfo.h"
#include "WoW/Source/Ui/ChatFrame.h"
#include "WoW/Source/Ui/ScriptEvents.h"
#include "WoW/Source/Object/ObjectClient/Bag_C.h"
#include "WoW/Source/DB/Rec/MailTemplateRec.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/FriendList.h"
#include "WoW/Source/DB/Rec/AuctionHouseRec.h"
#include "WoW/Source/Object/CreatureStats.h"
#include "WoW/Source/Object/GameObjectStats.h"
#include "WoW/Source/Game/GameClient/NameCache.h"
#include "WoW/Source/SoundInterface/SI3.h"
#include "WoW/Source/UIUtil/Tooltip.h"

// STATIC: 0x00F48E78
SendMailItem CGMailInfo::m_sendItems[16];

// STATIC: 0x00F48FF8
uint8_t CGMailInfo::m_lockSendItems;

// STATIC: 0x00F49010
uint32_t CGMailInfo::m_sendPending;

// STATIC: 0x00F48E28
MailSenderInfo s_recentSenders[3];

// STATIC: 0x00F49000
uint64_t CGMailInfo::m_sendMoney;

// STATIC: 0x00F49008
uint64_t CGMailInfo::m_sendCOD;

// STATIC: 0x00D13340
float CGMailInfo::m_nextMailTime = -1.0f;

// STATIC: 0x00F49024
uint32_t CGMailInfo::m_numInboxItems;

// STATIC: 0x00F49028
uint32_t CGMailInfo::m_totalInboxItems;

// STATIC: 0x00F49018
int32_t CGMailInfo::m_selectedStationery;

// STATIC: 0x00F4901C
int32_t CGMailInfo::m_selectedPackage;

// STATIC: 0x00F48E70
WOWGUID CGMailInfo::m_object;

// STATIC: 0x00F4902C
uint32_t CGMailInfo::m_nextUpdateTime;

// STATIC: 0x00F49030
int32_t CGMailInfo::m_commandPending;

// STATIC: 0x00F48FF9
uint8_t CGMailInfo::m_sendMailShowing;

// STATIC: 0x00F49014
uint32_t CGMailInfo::m_numStationeries;

// STATIC: 0x00F49020
uint32_t CGMailInfo::m_pendingStationeryCallbacks;

// STATIC: 0x00F49034
int32_t CGMailInfo::m_doPendingQuery;

// STATIC: 0x00F49038
MailboxMessageInfo* CGMailInfo::m_autoLooting;

// STATIC: 0x00F4903C
TSFixedArray<StationeryInfo*> CGMailInfo::m_stationeries;

// STATIC: 0x00F49048
TSGrowableArray<MailboxMessageInfo*> CGMailInfo::m_inbox;

// OFFSET: 0x00464790
MailAttachmentInfo* CGMailInfo::GetFirstAttachment(MailboxMessageInfo* mail) {
    if (!mail) {
        return NULL;
    }

    for (uint32_t i = 0; i < 16; i++) {
        if (mail->m_attachments[i].m_itemID) {
            return &mail->m_attachments[i];
        }
    }

    return NULL;
}

// OFFSET: 0x00466140
MailboxMessageInfo* CGMailInfo::GetInboxItem(uint32_t index) {
    if (index >= CGMailInfo::m_numInboxItems) {
        return NULL;
    }

    return CGMailInfo::m_inbox.m_data[index];
}

// OFFSET: 0x00466160/0x140079830
MailAttachmentInfo* GetAttachment(MailboxMessageInfo* message, uint32_t index) {
    if (!message) {
        return NULL;
    }

    if (index == static_cast<uint32_t>(-1)) {
        return CGMailInfo::GetFirstAttachment(message);
    }

    if (index >= 16) {
        return NULL;
    }

    return &message->m_attachments[index];
}

// OFFSET: 0x0096DED0/0x1406E4F20
MailAttachmentInfo* GetInboxAttachment(uint32_t index, uint32_t attachment) {
    return GetAttachment(CGMailInfo::GetInboxItem(index), attachment);
}

// OFFSET: 0x0096D590/0x1406E4610
uint32_t GetNumAttachments(const MailboxMessageInfo* message) {
    uint32_t count = 0;

    for (uint32_t i = 0; i < 16; i++) {
        if (message->m_attachments[i].m_itemID) {
            count++;
        }
    }

    return count;
}

// OFFSET: 0x0096D730/0x1406E46B0
static void SenderNameQueryCallback(int32_t id, const WOWGUID& guid, void* param, bool success) {
    if (success) {
        Script_SendUniqueSignal(SCRIPT_EVENT_MAIL_INBOX_UPDATE);
        FrameScript_SignalEvent(SCRIPT_EVENT_UPDATE_PENDING_MAIL, NULL);
    }
}

// OFFSET: 0x0096D760/0x1406E46E0
void SendItemQueryCallback(int32_t id, const WOWGUID& guid, void* param, bool success) {
    if (success) {
        Script_SendUniqueSignal(SCRIPT_EVENT_MAIL_SEND_INFO_UPDATE);
    }
}

// OFFSET: 0x0096D780/0x1406E4700
void InboxItemQueryCallback(int32_t id, const WOWGUID& guid, void* param, bool success) {
    if (success) {
        Script_SendUniqueSignal(SCRIPT_EVENT_MAIL_INBOX_UPDATE);
    }
}

// OFFSET: 0x0096DE70
void CGMailInfo::ClearSendItem(uint32_t index) {
    if (index < 12) {
        CGMailInfo::m_sendItems[index].m_guid = WOWGUID();
        CGMailInfo::m_sendItems[index].m_container = WOWGUID();
        CGMailInfo::m_sendItems[index].m_slot = 0;
    }
}

// OFFSET: 0x0096DEB0
StationeryInfo* GetStationery(uint32_t index) {
    if (index >= CGMailInfo::m_numStationeries) {
        return NULL;
    }

    return CGMailInfo::m_stationeries.m_data[index];
}

// OFFSET: 0x009705C0
int32_t Packet_SMSG_SEND_MAIL_RESULT(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    uint32_t itemGuid = 0;
    uint32_t inventoryError = 40;
    uint32_t itemCount = 0;
    uint32_t messageID;
    uint32_t action;
    uint32_t result;
    msg->Get(messageID);
    msg->Get(action);
    msg->Get(result);

    if (result == 1) {
        msg->Get(inventoryError);
    } else if (action == 2 && (!result || result == 21)) {
        msg->Get(itemGuid);
        msg->Get(itemCount);
    }

    if (action == 2 && result) {
        CGMailInfo::m_autoLooting = NULL;
    }

    switch (result) {
        case 0: {
            switch (action) {
                case 0: {
                    CGGameUI::DisplayError(ERR_MAIL_SENT);
                    CGMailInfo::ClearSendMail(true);
                    break;
                }

                case 1: {
                    CGMailInfo::RemoveMoney(messageID);
                    break;
                }

                case 2: {
                    CGMailInfo::RemoveItem(messageID, itemGuid, itemCount);
                    break;
                }

                case 3:
                case 4: {
                    CGMailInfo::RemoveMail(messageID);
                    break;
                }

                case 5: {
                    CGMailInfo::SetMailCopied(messageID);
                    break;
                }
            }

            break;
        }

        case 1: {
            CGGameUI::DisplayError(static_cast<UIErrors>(GetDisplayErrorByInventoryError(inventoryError)));
            break;
        }

        case 2: {
            CGGameUI::DisplayError(ERR_MAIL_TO_SELF);
            break;
        }

        case 3: {
            CGGameUI::DisplayError(ERR_NOT_ENOUGH_MONEY);
            break;
        }

        case 4: {
            CGGameUI::DisplayError(ERR_MAIL_TARGET_NOT_FOUND);
            break;
        }

        case 5: {
            CGGameUI::DisplayError(ERR_PLAYER_WRONG_FACTION);
            break;
        }

        case 10: {
            if (action == 4) {
                CGGameUI::DisplayError(ERR_MAIL_DELETE_ITEM_ERROR);
            } else {
                CGGameUI::DisplayError(ERR_MAIL_DATABASE_ERROR);
            }

            break;
        }

        case 14: {
            CGGameUI::DisplayError(ERR_RESTRICTED_ACCOUNT);
            break;
        }

        case 15: {
            CGGameUI::DisplayError(ERR_MAIL_REACHED_CAP);
            break;
        }

        case 16: {
            CGGameUI::DisplayError(ERR_MAIL_WRAPPED_COD);
            break;
        }

        case 17: {
            CGGameUI::DisplayError(ERR_USER_SQUELCHED);
            break;
        }

        case 18: {
            CGGameUI::DisplayError(ERR_MAIL_TOO_MANY_ATTACHMENTS);
            break;
        }

        case 19: {
            CGGameUI::DisplayError(ERR_MAIL_INVALID_ATTACHMENT);
            break;
        }

        case 20: {
            break;
        }

        case 21: {
            CGGameUI::DisplayError(ERR_MAIL_ATTACHMENT_EXPIRED);
            CGMailInfo::RemoveItem(messageID, itemGuid, itemCount);
            break;
        }

        default: {
            CGGameUI::DisplayError(ERR_MAIL_DATABASE_ERROR);
            break;
        }
    }

    if (!action) {
        CGMailInfo::m_commandPending = 0;
        CGMailInfo::m_sendPending = 0;
    }

    if (!result && action == 2 && CGMailInfo::m_autoLooting) {
        MailboxMessageInfo* message = CGMailInfo::m_autoLooting;
        CGMailInfo::m_autoLooting = NULL;
        CGMailInfo::ProcessAutoLootAttachments(message);
    } else {
        CGMailInfo::m_commandPending = 0;
    }

    FrameScript_SignalEvent(result ? SCRIPT_EVENT_MAIL_FAILED : SCRIPT_EVENT_MAIL_SUCCESS, NULL);
    return 1;
}

// OFFSET: 0x0096F610
int32_t Packet_SMSG_RECEIVED_MAIL(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    float delay;
    msg->Get(delay);
    CGMailInfo::ReceivedMail(delay);
    return 1;
}

// OFFSET: 0x00970E00
int32_t Packet_SMSG_MAIL_LIST_RESULT(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    uint32_t total = 0;
    msg->Get(total);
    uint8_t numMessages = 0;
    msg->Get(numMessages);
    uint32_t count = numMessages;

    if (count > CGMailInfo::m_inbox.m_count) {
        uint32_t i = CGMailInfo::m_inbox.m_count;
        CGMailInfo::m_inbox.SetCount(count);

        for (; i < count; i++) {
            CGMailInfo::m_inbox.m_data[i] = static_cast<MailboxMessageInfo*>(SMemAlloc(sizeof(MailboxMessageInfo), __FILE__, __LINE__, 0));
        }
    }

    for (uint32_t i = 0; i < count; i++) {
        MailboxMessageInfo* message = CGMailInfo::m_inbox.m_data[i];
        memset(message, 0, sizeof(*message));
        uint16_t size;
        msg->Get(size);
        msg->Get(reinterpret_cast<uint32_t&>(message->m_messageID));
        uint8_t type = 0;
        msg->Get(type);
        message->m_messageType = type;

        if (!type) {
            msg->Get(message->m_sender.m_guid);
            message->m_senderEntry = 0;
        } else if (type <= 5) {
            msg->Get(reinterpret_cast<uint32_t&>(message->m_senderEntry));
            message->m_sender = WOWGUID();
        }

        msg->Get(message->m_cod);
        msg->Get(reinterpret_cast<uint32_t&>(message->m_package));
        msg->Get(reinterpret_cast<uint32_t&>(message->m_stationery));
        msg->Get(message->m_money);
        msg->Get(message->m_flags);
        msg->Get(message->m_expireTime);
        msg->Get(reinterpret_cast<uint32_t&>(message->m_mailTemplateID));
        msg->GetString(message->m_subject, sizeof(message->m_subject));
        msg->GetString(message->m_body, sizeof(message->m_body));
        uint8_t numItems;
        msg->Get(numItems);
        memset(message->m_attachments, 0, sizeof(message->m_attachments));

        TSBitField<TSGrowableArray<uint32_t> > used;
        used.m_count = 16;
        used.m_bits.ReallocData(1);

        if (used.m_count) {
            used.m_bits.m_data[0] = 0;
        }

        for (int32_t j = 0; j < numItems; j++) {
            uint8_t index;
            msg->Get(index);

            if (index >= 16) {
                index = 0;
            }

            if (used.m_bits.m_data[index >> 5] & (1 << (index & 0x1F))) {
                for (index = 0; used.m_bits.m_data[index >> 5] & (1 << (index & 0x1F)); ) {
                    if (++index >= 16) {
                        break;
                    }
                }
            }

            MailAttachmentInfo unused;
            MailAttachmentInfo* attachment;

            if (index >= 16) {
                attachment = &unused;
            } else {
                attachment = &message->m_attachments[index];
                used.m_bits.m_data[index >> 5] |= 1 << (index & 0x1F);
            }

            msg->Get(attachment->m_itemGuid);
            msg->Get(reinterpret_cast<uint32_t&>(attachment->m_itemID));

            for (uint32_t k = 0; k < 10; k++) {
                msg->Get(reinterpret_cast<uint32_t&>(attachment->m_enchantIDs[k]));
                msg->Get(attachment->m_enchantDurations[k]);
                msg->Get(reinterpret_cast<uint32_t&>(attachment->m_enchantCharges[k]));
            }

            msg->Get(reinterpret_cast<uint32_t&>(attachment->m_randomPropertiesID));
            msg->Get(attachment->m_suffixFactor);
            msg->Get(attachment->m_count);
            msg->Get(reinterpret_cast<uint32_t&>(attachment->m_spellCharges));
            msg->Get(attachment->m_maxDurability);
            msg->Get(attachment->m_durability);
            uint8_t unknown;
            msg->Get(unknown);
            attachment->unk_009C = unknown;
        }

        message->m_invoiceItemID = 0;
        message->m_invoiceRandomPropertiesID = 0;
        message->m_invoiceType = -1;

        if (!type && FriendList::s_friendList->IsIgnored(message->m_sender, true) && !(message->m_flags & 0x20A)) {
            CDataStore reply;

            if (GetNumAttachments(message) || message->m_money) {
                reply.Put(static_cast<uint32_t>(CMSG_MAIL_RETURN_TO_SENDER));
                reply.Put(CGMailInfo::m_object.m_guid);
                reply.Put(static_cast<uint32_t>(message->m_messageID));
                reply.Put(message->m_sender.m_guid);
            } else {
                reply.Put(static_cast<uint32_t>(CMSG_MAIL_DELETE));
                reply.Put(CGMailInfo::m_object.m_guid);
                reply.Put(static_cast<uint32_t>(message->m_messageID));
                reply.Put(static_cast<uint32_t>(1));
            }

            reply.Finalize();
            ClientServices::Send2(&reply);
            i--;
            count--;
            total--;
        } else {
            if (message->m_stationery <= 0) {
                message->m_stationery = GetStationery(0)->m_id;
            }

            if (type == 2) {
                SStrCopy(message->m_invoiceSubject, message->m_subject, sizeof(message->m_invoiceSubject));
                CGMailInfo::ParseInboxSubject(message);
            } else if (type == 5) {
                message->m_sender.m_guid = SStrToInt64(message->m_subject);
                SStrCopy(message->m_invoiceSubject, SStrChr(message->m_subject, ':') + 1, sizeof(message->m_invoiceSubject));
                ProfanityFilter(message->m_invoiceSubject, false, false);
                CGMailInfo::ParseInboxSubject(message);
            }

            ProfanityFilter(message->m_subject, false, false);
        }
    }

    CGMailInfo::m_numInboxItems = count;
    CGMailInfo::m_totalInboxItems = total;
    CGMailInfo::m_commandPending = 0;

    if (!count && !fnotequal(CGMailInfo::m_nextMailTime, 0.0f)) {
        CGMailInfo::QueryNextMailTime();
    }

    FrameScript_SignalEvent(SCRIPT_EVENT_MAIL_INBOX_UPDATE, NULL);
    return 1;
}

// OFFSET: 0x00972720/0x1406EA560
void CGMailInfo::InitializeGame() {
    uint32_t numStationeries = g_stationeryDB.m_numRecords;
    CGMailInfo::m_stationeries.SetCount(numStationeries);

    for (int32_t i = 0; i < static_cast<int32_t>(numStationeries); i++) {
        StationeryRec* rec = g_stationeryDB.GetRecordByIndex(i);

        CGMailInfo::m_stationeries.m_data[i] = static_cast<StationeryInfo*>(SMemAlloc(sizeof(StationeryInfo), __FILE__, __LINE__, 0));
        CGMailInfo::m_stationeries.m_data[i]->m_id = rec->m_id;
        CGMailInfo::m_stationeries.m_data[i]->m_itemID = rec->m_itemID;
        CGMailInfo::m_stationeries.m_data[i]->m_isDefault = rec->m_flags & 1;
        CGMailInfo::m_stationeries.m_data[i]->m_itemGUID = WOWGUID::Make(0);
    }

    CGMailInfo::m_inbox.SetCount(8);

    for (uint32_t i = 0; i < 8; i++) {
        CGMailInfo::m_inbox.m_data[i] = static_cast<MailboxMessageInfo*>(SMemAlloc(sizeof(MailboxMessageInfo), __FILE__, __LINE__, 0));
    }

    memset(s_recentSenders, 0, sizeof(s_recentSenders));

    ClientServices::SetMessageHandler(SMSG_MAIL_COMMAND_RESULT, Packet_SMSG_SEND_MAIL_RESULT, NULL);
    ClientServices::SetMessageHandler(SMSG_MAIL_LIST_RESULT, Packet_SMSG_MAIL_LIST_RESULT, NULL);
    ClientServices::SetMessageHandler(MSG_QUERY_NEXT_MAIL_TIME, Packet_MSG_QUERY_NEXT_MAIL_TIME, NULL);
    ClientServices::SetMessageHandler(SMSG_RECEIVED_MAIL, Packet_SMSG_RECEIVED_MAIL, NULL);
    ClientServices::SetMessageHandler(SMSG_SHOW_MAILBOX, Packet_SMSG_SHOW_MAILBOX, NULL);
}

// OFFSET: 0x0096E4C0
void CGMailInfo::UpdateRecentSenders() {
    bool changed = false;

    for (uint32_t i = 0; i < 3; i++) {
        MailSenderInfo& sender = s_recentSenders[i];
        WOWGUID guid = sender.m_guid;

        if (!guid && !sender.m_entry) {
            continue;
        }

        bool found = false;

        for (uint32_t j = 0; j < CGMailInfo::m_numInboxItems; j++) {
            MailboxMessageInfo* message = CGMailInfo::m_inbox.m_data[j];

            if (message->m_flags & 0x1) {
                continue;
            }

            if (guid && guid == message->m_sender) {
                found = true;
                break;
            }

            if (sender.m_entry && sender.m_entry == message->m_senderEntry) {
                found = true;
                break;
            }
        }

        if (!found) {
            sender.m_guid = WOWGUID();
            sender.m_entry = 0;
            sender.m_messageType = 0;
            sender.m_stationery = 0;
            sender.unk_0014 = 0.0f;
            changed = true;
        }
    }

    if (changed) {
        FrameScript_SignalEvent(SCRIPT_EVENT_UPDATE_PENDING_MAIL, NULL);
    }
}

// OFFSET: 0x0096D9F0/0x1406E49D0
void CGMailInfo::UpdatePendingMail(float elapsed) {
    for (int32_t i = 0; i < 3; i++) {
        MailSenderInfo& sender = s_recentSenders[i];

        if ((sender.m_guid || sender.m_entry) && sender.unk_0014 > 0.0f) {
            float remaining = sender.unk_0014 - elapsed;
            sender.unk_0014 = remaining > 0.0f ? remaining : 0.0f;

            if (fabs(sender.unk_0014) < 0.00000023841858f) {
                FrameScript_SignalEvent(SCRIPT_EVENT_UPDATE_PENDING_MAIL, NULL);
            }
        }
    }

    if (CGMailInfo::m_nextMailTime > 0.0f) {
        float remaining = CGMailInfo::m_nextMailTime - elapsed;
        CGMailInfo::m_nextMailTime = remaining > 0.0f ? remaining : 0.0f;

        if (fabs(CGMailInfo::m_nextMailTime) < 0.00000023841858f) {
            FrameScript_SignalEvent(SCRIPT_EVENT_UPDATE_PENDING_MAIL, NULL);
        }
    }
}

// OFFSET: 0x0096E0B0
int32_t Packet_SMSG_SHOW_MAILBOX(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    WOWGUID mailbox;
    msg->Get(mailbox.m_guid);
    WOWGUID target = mailbox;
    CGGossipInfo::ClearGossip();
    CGGameUI::SetInteractTarget(target);
    CGMailInfo::m_object = target;
    CGMailInfo::ClearSendMail(false);
    FrameScript_SignalEvent(SCRIPT_EVENT_MAIL_SHOW, NULL);
    return 1;
}

// OFFSET: 0x0096E3F0
void CGMailInfo::SetMailCopied(uint32_t messageID) {
    for (uint32_t i = 0; i < CGMailInfo::m_numInboxItems; i++) {
        MailboxMessageInfo* message = CGMailInfo::m_inbox.m_data[i];

        if (message->m_messageID == static_cast<int32_t>(messageID)) {
            message->m_flags |= 0x4;
            FrameScript_SignalEvent(SCRIPT_EVENT_MAIL_INBOX_UPDATE, NULL);
            return;
        }
    }
}

// OFFSET: 0x0096E440
void CGMailInfo::RemoveMail(uint32_t messageID) {
    for (uint32_t i = 0; i < CGMailInfo::m_numInboxItems; i++) {
        if (CGMailInfo::m_inbox.m_data[i]->m_messageID == static_cast<int32_t>(messageID)) {
            CGMailInfo::m_totalInboxItems--;

            for (CGMailInfo::m_numInboxItems--; i < CGMailInfo::m_numInboxItems; i++) {
                memcpy(CGMailInfo::m_inbox.m_data[i], CGMailInfo::m_inbox.m_data[i + 1], sizeof(MailboxMessageInfo));
            }

            FrameScript_SignalEvent(SCRIPT_EVENT_MAIL_INBOX_UPDATE, NULL);
            return;
        }
    }
}

// OFFSET: 0x00971400/0x1406E8ED0
static int32_t QSortStationery(const void* a, const void* b) {
    const StationeryInfo* infoA = *static_cast<StationeryInfo* const*>(a);
    const StationeryInfo* infoB = *static_cast<StationeryInfo* const*>(b);

    if (infoA->m_isDefault != infoB->m_isDefault) {
        return 2 * (infoA->m_isDefault == 0) - 1;
    }

    if (infoA->m_itemGUID && !infoB->m_itemGUID) {
        return -1;
    }

    if (infoB->m_itemGUID && !infoA->m_itemGUID) {
        return 1;
    }

    ItemRecSparse* statsA = g_itemSparseDB.GetRecord(infoA->m_itemID, WOWGUID::Make(0), NULL, NULL, false);
    ItemRecSparse* statsB = g_itemSparseDB.GetRecord(infoB->m_itemID, WOWGUID::Make(0), NULL, NULL, false);

    if (!statsA) {
        if (!statsB) {
            return 0;
        }

        return 2 * (statsA == NULL) - 1;
    }

    if (!statsB) {
        return 2 * (statsA == NULL) - 1;
    }

    int32_t priceA = statsA->GetBuyPrice(NULL);
    int32_t priceB = statsB->GetBuyPrice(NULL);

    if (priceA != priceB) {
        return 2 * (priceA >= priceB) - 1;
    }

    return SStrSortUTF8I(statsA->GetName(0), statsB->GetName(0), STORM_MAX_STR);
}

// OFFSET: 0x0096F550/0x1406E6960
static void InboxAuctionItemQueryCallback(int32_t id, const WOWGUID& guid, void* param, bool success) {
    uint32_t* messageID = static_cast<uint32_t*>(param);

    if (success) {
        CGMailInfo::ParseInboxSubject(*messageID);
        Script_SendUniqueSignal(SCRIPT_EVENT_MAIL_INBOX_UPDATE);
    }

    delete messageID;
}

// OFFSET: 0x00970B20/0x1406E83A0
void CGMailInfo::ParseInboxSubject(MailboxMessageInfo* message) {
    if (message->m_messageType == 2) {
        int32_t itemID = SStrToInt(message->m_invoiceSubject);
        const char* field = SStrChr(message->m_invoiceSubject, ':');
        uint32_t type = 0;
        int32_t randomPropertiesID = 0;
        uint32_t value = 0;
        int32_t stackCount = 1;

        if (field && *field) {
            randomPropertiesID = SStrToInt(field + 1);
            const char* next = SStrChr(field + 1, ':');

            if (next && *next) {
                type = SStrToInt(next + 1);
            } else {
                type = 0;
            }

            if (next && *next) {
                next = SStrChr(next + 1, ':');

                if (next && *next) {
                    value = SStrToInt(next + 1);
                } else {
                    value = 0;
                }
            }

            if (next && *next) {
                next = SStrChr(next + 1, ':');

                if (next && *next) {
                    stackCount = SStrToInt(next + 1);
                } else {
                    stackCount = 0;
                }
            }
        }

        WOWGUID none = WOWGUID();

        if (!g_itemSparseDB.GetRecord(itemID, none, NULL, NULL, false)) {
            uint32_t* messageID = static_cast<uint32_t*>(SMemAlloc(sizeof(uint32_t), __FILE__, __LINE__, 0));
            *messageID = message->m_messageID;
            WOWGUID requester = WOWGUID();
            g_itemSparseDB.GetRecord(itemID, requester, &InboxAuctionItemQueryCallback, messageID, false);
            message->m_subject[0] = '\0';
            return;
        }

        char itemName[256];
        CGItem_C::BuildItemName(itemName, sizeof(itemName), itemID, randomPropertiesID);
        const char* format;

        switch (type) {
            case 0: {
                format = FrameScript_GetText("AUCTION_OUTBID_MAIL_SUBJECT", -1, GENDER_NOT_APPLICABLE);
                break;
            }

            case 1: {
                format = FrameScript_GetText("AUCTION_WON_MAIL_SUBJECT", -1, GENDER_NOT_APPLICABLE);
                break;
            }

            case 2: {
                format = FrameScript_GetText("AUCTION_SOLD_MAIL_SUBJECT", -1, GENDER_NOT_APPLICABLE);
                break;
            }

            case 3: {
                format = FrameScript_GetText("AUCTION_EXPIRED_MAIL_SUBJECT", -1, GENDER_NOT_APPLICABLE);
                break;
            }

            case 4:
            case 5: {
                format = FrameScript_GetText("AUCTION_REMOVED_MAIL_SUBJECT", -1, GENDER_NOT_APPLICABLE);
                break;
            }

            case 6: {
                format = FrameScript_GetText("AUCTION_INVOICE_MAIL_SUBJECT", -1, GENDER_NOT_APPLICABLE);
                break;
            }

            default: {
                format = NULL;
                break;
            }
        }

        if (format && *format) {
            if (stackCount > 1) {
                SStrPrintf(itemName, sizeof(itemName), FrameScript_GetText("AUCTION_MAIL_ITEM_STACK", -1, GENDER_NOT_APPLICABLE), itemName, stackCount);
            }

            SStrPrintf(message->m_subject, sizeof(message->m_subject), format, itemName);
        } else {
            message->m_subject[0] = '\0';
        }

        if (type == 1 || type == 2 || type == 6) {
            message->m_invoiceItemID = itemID;
            message->m_invoiceRandomPropertiesID = randomPropertiesID;
            message->m_invoiceType = type;
            message->m_invoiceValue = value;
            message->m_invoiceStackCount = stackCount;
        }

        return;
    }

    if (message->m_messageType == 5) {
        const char* format;

        if (message->m_flags & 0x100) {
            format = FrameScript_GetText("CALENDAR_INVITE_REMOVED_MAIL_SUBJECT", -1, GENDER_NOT_APPLICABLE);
        } else {
            format = FrameScript_GetText("CALENDAR_EVENT_REMOVED_MAIL_SUBJECT", -1, GENDER_NOT_APPLICABLE);
        }

        if (format && *format) {
            SStrPrintf(message->m_subject, sizeof(message->m_subject), format, message->m_invoiceSubject);
        } else {
            message->m_subject[0] = '\0';
        }
    }
}

// OFFSET: 0x0096D930
int32_t CanDeleteEmptyMail(MailboxMessageInfo* message) {
    bool autoDelete = (message->m_flags >> 3) & 1;
    return (message->m_messageType == 2 || autoDelete || (!SStrLen(message->m_body) && !message->m_mailTemplateID)) && !message->m_deletePending && !message->m_money && !GetNumAttachments(message);
}

// OFFSET: 0x0096E110
void CGMailInfo::RemoveMoney(uint32_t messageID) {
    uint32_t i = 0;

    if (!CGMailInfo::m_numInboxItems) {
        return;
    }

    while (CGMailInfo::m_inbox.m_data[i]->m_messageID != static_cast<int32_t>(messageID)) {
        if (++i >= CGMailInfo::m_numInboxItems) {
            return;
        }
    }

    CGMailInfo::m_inbox.m_data[i]->m_money = 0;

    if (CanDeleteEmptyMail(CGMailInfo::m_inbox.m_data[i])) {
        CDataStore msg;
        msg.Put(static_cast<uint32_t>(CMSG_MAIL_DELETE));
        msg.Put(CGMailInfo::m_object.m_guid);
        msg.Put(messageID);
        msg.Put(static_cast<uint32_t>(2));
        msg.Finalize();
        ClientServices::Send2(&msg);
        CGMailInfo::m_inbox.m_data[i]->m_deletePending = 1;
        FrameScript_SignalEvent(SCRIPT_EVENT_CLOSE_INBOX_ITEM, "%d", i + 1);
    }

    FrameScript_SignalEvent(SCRIPT_EVENT_MAIL_INBOX_UPDATE, NULL);
}

// OFFSET: 0x0096E230
void CGMailInfo::RemoveItem(uint32_t messageID, uint32_t itemGuid, uint32_t itemCount) {
    uint32_t i = 0;

    if (!CGMailInfo::m_numInboxItems) {
        return;
    }

    while (CGMailInfo::m_inbox.m_data[i]->m_messageID != static_cast<int32_t>(messageID)) {
        if (++i >= CGMailInfo::m_numInboxItems) {
            return;
        }
    }

    MailboxMessageInfo* message = CGMailInfo::m_inbox.m_data[i];

    if (!message) {
        return;
    }

    uint32_t index = 0;

    while (!message->m_attachments[index].m_itemID || message->m_attachments[index].m_itemGuid != itemGuid) {
        if (++index >= 16) {
            return;
        }
    }

    MailAttachmentInfo* attachment = &message->m_attachments[index];

    if (!attachment) {
        return;
    }

    memset(attachment, 0, sizeof(*attachment));
    message->m_cod = 0;

    if ((message->m_messageType == 2 || (message->m_flags & 0x8) || (!SStrLen(message->m_body) && !message->m_mailTemplateID)) && !message->m_deletePending && !message->m_money && !GetNumAttachments(message)) {
        CDataStore msg;
        msg.Put(static_cast<uint32_t>(CMSG_MAIL_DELETE));
        msg.Put(CGMailInfo::m_object.m_guid);
        msg.Put(messageID);
        msg.Put(static_cast<uint32_t>(3));
        msg.Finalize();
        ClientServices::Send2(&msg);
        message->m_deletePending = 1;
        FrameScript_SignalEvent(SCRIPT_EVENT_CLOSE_INBOX_ITEM, "%d", i + 1);
    }

    FrameScript_SignalEvent(SCRIPT_EVENT_MAIL_INBOX_UPDATE, NULL);
}

// OFFSET: 0x0096F640/0x1406E6B00
bool CGMailInfo::CanComplain(uint32_t index) {
    CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();

    if (!player || index >= CGMailInfo::m_numInboxItems) {
        return false;
    }

    MailboxMessageInfo* message = CGMailInfo::m_inbox.m_data[index];
    return message && !message->m_messageType && !(message->m_flags & 0x2) && message->m_stationery != 61 && message->m_sender != player->m_obj->m_guid && !FriendList::s_friendList->IsFriend(message->m_sender);
}

// OFFSET: 0x0096F6C0/0x1406E6B90
void CGMailInfo::Complain(uint32_t index) {
    if (CGMailInfo::m_commandPending || !CGChat::m_complaintsSystemStatus) {
        return;
    }

    WOWGUID mailbox = CGMailInfo::m_object;

    if (!mailbox || !CGMailInfo::CanComplain(index)) {
        return;
    }

    MailboxMessageInfo* message = CGMailInfo::GetInboxItem(index);

    if (!message) {
        return;
    }

    if (FriendList::s_friendList->IsComplained(message->m_sender)) {
        CGChat::AddChatMessage(FrameScript_GetText("COMPLAINT_ADDED", -1, GENDER_NOT_APPLICABLE), static_cast<SLASH_COMMAND_ID>(0), NULL);
        return;
    }

    FriendList::s_friendList->AddComplained(message->m_sender);
    CDataStore complaint;
    complaint.Put(static_cast<uint32_t>(CMSG_COMPLAINT));
    complaint.Put(static_cast<uint8_t>(0));
    complaint.Put(message->m_sender.m_guid);
    complaint.Put(static_cast<uint32_t>(0));
    complaint.Put(static_cast<uint32_t>(message->m_messageID));
    complaint.Put(static_cast<uint32_t>(0));
    complaint.Finalize();
    ClientServices::Send2(&complaint);
    FrameScript_SignalEvent(SCRIPT_EVENT_CLOSE_INBOX_ITEM, "%d", index + 1);
    CDataStore query;
    query.Put(static_cast<uint32_t>(CMSG_MAIL_GET_LIST));
    query.Put(mailbox.m_guid);
    query.Finalize();
    ClientServices::Send2(&query);
}

// OFFSET: 0x00971520/0x1406E9040
void CGMailInfo::UpdateStationeryList() {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return;
    }

    CGMailInfo::m_numStationeries = 0;

    uint32_t count = CGMailInfo::m_stationeries.m_count;

    for (uint32_t i = 0; i < count; i++) {
        StationeryRec* rec = g_stationeryDB.GetRecord(CGMailInfo::m_stationeries.m_data[i]->m_id);
        CGItem_C* item = player->m_bag.FindItemOfType(rec->m_itemID, 0);
        WOWGUID guid = item ? item->m_obj->m_guid : WOWGUID::Make(0);
        CGMailInfo::m_stationeries.m_data[i]->m_itemGUID = guid;

        if (CGMailInfo::m_stationeries.m_data[i]->m_isDefault || CGMailInfo::m_stationeries.m_data[i]->m_itemGUID) {
            if (g_itemSparseDB.GetRecord(rec->m_itemID, WOWGUID::Make(0), NULL, NULL, false)) {
                CGMailInfo::m_numStationeries++;
            }
        }
    }

    qsort(CGMailInfo::m_stationeries.m_data, count, sizeof(StationeryInfo*), QSortStationery);
}

// OFFSET: 0x0096F830
int32_t CGMailInfo::SetSendMoney(uint64_t money) {
    CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();

    if (player && player->GetCoinage() >= money) {
        if (!(money & 0x8000000000000000ull) && money < 10000000000ull) {
            CGMailInfo::m_sendMoney = money;
            FrameScript_SignalEvent(SCRIPT_EVENT_SEND_MAIL_MONEY_CHANGED, NULL);
            return 1;
        }
    } else {
        CGGameUI::DisplayError(ERR_NOT_ENOUGH_MONEY);
    }

    return 0;
}

// OFFSET: 0x0096D9A0
void CGMailInfo::SetSendCOD(uint64_t amount) {
    if (CGMailInfo::GetNumSendItems() && !(amount & 0x8000000000000000ull) && amount < 10000000000ull) {
        CGMailInfo::m_sendCOD = amount;
        FrameScript_SignalEvent(SCRIPT_EVENT_SEND_MAIL_COD_CHANGED, NULL);
    }
}

// OFFSET: 0x0096F8F0
void CGMailInfo::MarkAsRead(uint32_t index) {
    if (index >= CGMailInfo::m_numInboxItems || (CGMailInfo::m_inbox.m_data[index]->m_flags & 0x1)) {
        return;
    }

    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_MAIL_MARK_AS_READ));
    msg.Put(CGMailInfo::m_object.m_guid);
    msg.Put(static_cast<uint32_t>(CGMailInfo::m_inbox.m_data[index]->m_messageID));
    msg.Finalize();
    ClientServices::Send2(&msg);
    CGMailInfo::m_inbox.m_data[index]->m_flags |= 0x1;
    CGMailInfo::UpdateRecentSenders();

    if (fabs(CGMailInfo::m_nextMailTime) < 0.00000023841858f) {
        uint32_t i;

        for (i = 0; i < CGMailInfo::m_numInboxItems; i++) {
            if (!(CGMailInfo::m_inbox.m_data[i]->m_flags & 0x1)) {
                break;
            }
        }

        if (i >= CGMailInfo::m_numInboxItems) {
            CGMailInfo::m_nextMailTime = -1.0f;
            FrameScript_SignalEvent(SCRIPT_EVENT_UPDATE_PENDING_MAIL, NULL);
        }
    }

    FrameScript_SignalEvent(SCRIPT_EVENT_MAIL_INBOX_UPDATE, NULL);
    CGMailInfo::m_doPendingQuery = 1;
}

// OFFSET: 0x0096E5A0/0x1406E5780
void CGMailInfo::QueryNextMailTime() {
    CDataStore msg;
    msg.Put(static_cast<uint32_t>(MSG_QUERY_NEXT_MAIL_TIME));
    msg.Finalize();
    ClientServices::Send2(&msg);
    CGMailInfo::m_nextMailTime = -1.0f;
    CGMailInfo::m_doPendingQuery = 0;
}

// OFFSET: 0x00971630/0x1406E91B0
bool CGMailInfo::AddItemToOutgoingMail(uint32_t index, WOWGUID item, WOWGUID container, uint8_t slot) {
    CGItem_C* object = static_cast<CGItem_C*>(ClntObjMgrObjectPtr(item, TYPEMASK_ITEM, __FILE__, __LINE__));

    if (!object) {
        return false;
    }

    if (CGMailInfo::m_lockSendItems) {
        CGGameUI::DisplayError(ERR_CLIENT_LOCKED_OUT);
        return false;
    }

    WOWGUID none = WOWGUID();
    ItemRecSparse* stats = g_itemSparseDB.GetRecord(object->m_obj->m_entryID, none, NULL, NULL, false);

    if (!stats) {
        CGGameUI::DisplayError(ERR_ITEM_NOT_FOUND);
        return false;
    }

    if (object->IsBound() && !object->IsBoundToAccount()) {
        if (stats->m_bonding == 4) {
            CGGameUI::DisplayError(ERR_MAIL_QUEST_ITEM);
        } else {
            CGGameUI::DisplayError(ERR_MAIL_BOUND_ITEM);
        }

        return false;
    }

    if (object->GetItemStaticFlags(0) & 0x2) {
        CGGameUI::DisplayError(ERR_MAIL_CONJURED_ITEM);
        return false;
    }

    if (((object->GetItemStaticFlags(0) & 0x10000) && stats->m_duration > 0) || object->m_item->m_itemFieldDuration || stats->m_holidayID) {
        CGGameUI::DisplayError(ERR_MAIL_LIMITED_DURATION_ITEM);
        return false;
    }

    if (object->IsZoneBound() || object->IsInstanceBound()) {
        CGGameUI::DisplayError(ERR_MAIL_CONJURED_ITEM);
        return false;
    }

    if (object->m_obj->m_type & TYPEMASK_CONTAINER) {
        CGBag_C* bag = object->GetBag();

        if (bag && bag->GetNumItems()) {
            CGGameUI::DisplayError(ERR_MAIL_BAG);
            return false;
        }
    }

    uint32_t sendIndex = index;

    if (index == static_cast<uint32_t>(-1)) {
        for (sendIndex = 0; CGMailInfo::GetSendItemGuid(sendIndex); sendIndex++) {
            if (sendIndex + 1 >= 12) {
                CGGameUI::DisplayError(ERR_MAIL_INVALID_ATTACHMENT_SLOT);
                return false;
            }
        }
    }

    if (sendIndex > 11) {
        CGGameUI::DisplayError(ERR_MAIL_INVALID_ATTACHMENT_SLOT);
        return false;
    }

    CGMailInfo::SetSendItem(sendIndex, item, container, slot);
    CGGameUI::LockItem(item);
    SI3::PlayItemSound(1, object);
    CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();

    if (player) {
        CGItem_C::ItemPurchaseData* purchase = object->m_itemPurchaseData;

        if (purchase && static_cast<int32_t>(purchase->m_refundTimeLeft - player->GetPlayedTime() + 7200) > 0) {
            CGMailInfo::m_lockSendItems = 1;
            FrameScript_SignalEvent(SCRIPT_EVENT_MAIL_LOCK_SEND_ITEMS, "%d%s", sendIndex + 1, CGTooltip::GetItemHyperlinkString(object));
        }
    }

    FrameScript_SignalEvent(SCRIPT_EVENT_MAIL_SEND_INFO_UPDATE, NULL);
    return true;
}

// OFFSET: 0x0096FA30/0x1406E6FF0
void CGMailInfo::ProcessAutoLootAttachments(MailboxMessageInfo* message) {
    CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();

    if (!player || player->GetCoinage() < message->m_cod) {
        CGGameUI::DisplayError(ERR_NOT_ENOUGH_MONEY);
        return;
    }

    uint32_t index = 0;
    MailAttachmentInfo* attachment;

    while (true) {
        attachment = GetAttachment(message, index);

        if (attachment && attachment->m_itemID) {
            break;
        }

        if (++index >= 16) {
            return;
        }
    }

    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_MAIL_TAKE_ITEM));
    msg.Put(CGMailInfo::m_object.m_guid);
    msg.Put(static_cast<uint32_t>(message->m_messageID));
    msg.Put(attachment->m_itemGuid);
    msg.Finalize();
    ClientServices::Send2(&msg);
    CGMailInfo::m_commandPending = 1;

    for (index++; index < 16; index++) {
        MailAttachmentInfo* next = GetAttachment(message, index);

        if (next && next->m_itemID) {
            CGMailInfo::m_autoLooting = message;
            break;
        }
    }
}

// OFFSET: 0x0096FBE0/0x1406E7210
void CGMailInfo::AutoLoot(uint32_t index) {
    if (CGMailInfo::m_autoLooting || index >= CGMailInfo::m_numInboxItems) {
        return;
    }

    MailboxMessageInfo* message = CGMailInfo::m_inbox.m_data[index];

    if (!message) {
        return;
    }

    if (message->m_money) {
        CDataStore msg;
        msg.Put(static_cast<uint32_t>(CMSG_MAIL_TAKE_MONEY));
        msg.Put(CGMailInfo::m_object.m_guid);
        msg.Put(static_cast<uint32_t>(message->m_messageID));
        msg.Put(message->m_money);
        msg.Finalize();
        ClientServices::Send2(&msg);
    }

    CGMailInfo::ProcessAutoLootAttachments(message);
}

// OFFSET: 0x0096E620
void CGMailInfo::ReceivedMail(float delay) {
    if (CGMailInfo::m_object) {
        CGMailInfo::m_doPendingQuery = 1;
        return;
    }

    CGMailInfo::QueryNextMailTime();

    if (fequal(delay, 0.0f)) {
        CGMailInfo::m_nextMailTime = 0.0f;
        FrameScript_SignalEvent(SCRIPT_EVENT_UPDATE_PENDING_MAIL, NULL);
    } else if (CGMailInfo::m_nextMailTime < 0.0f || CGMailInfo::m_nextMailTime > delay) {
        CGMailInfo::m_nextMailTime = delay;
    }
}

// PROVEN: 8585ab24 IDENTICAL/NONE
// OFFSET: 0x00972710/0x1406EA540
void StationeryItemCallback(int32_t id, const WOWGUID& guid, void* param, bool success) {
    if (!--CGMailInfo::m_pendingStationeryCallbacks) {
        CGMailInfo::UpdateStationeryList();
    }
}

// OFFSET: 0x00972920/0x1406EA7F0
void CGMailInfo::EnterWorld() {
    CGMailInfo::m_object = WOWGUID::Make(0);
    memset(CGMailInfo::m_sendItems, 0, sizeof(((CGMailInfo*)0)->m_sendItems));
    uint32_t count = CGMailInfo::m_stationeries.m_count;
    CGMailInfo::m_sendMoney = 0;
    CGMailInfo::m_lockSendItems = 0;
    CGMailInfo::m_sendPending = 0;
    CGMailInfo::m_numStationeries = 0;
    CGMailInfo::m_selectedStationery = 0;
    CGMailInfo::m_selectedPackage = 0;
    CGMailInfo::m_pendingStationeryCallbacks = 0;
    CGMailInfo::m_numInboxItems = 0;
    CGMailInfo::m_totalInboxItems = 0;
    CGMailInfo::m_nextUpdateTime = 0;
    CGMailInfo::m_commandPending = 0;
    CGMailInfo::m_nextMailTime = -1.0f;
    CGMailInfo::m_sendMailShowing = 0;

    for (uint32_t i = 0; i < count; i++) {
        if (!g_itemSparseDB.GetRecord(CGMailInfo::m_stationeries.m_data[i]->m_itemID, WOWGUID::Make(0), StationeryItemCallback, NULL, false)) {
            CGMailInfo::m_pendingStationeryCallbacks++;
        }
    }

    if (!CGMailInfo::m_pendingStationeryCallbacks) {
        CGMailInfo::UpdateStationeryList();
    }

    memset(s_recentSenders, 0, sizeof(s_recentSenders));
    CGMailInfo::QueryNextMailTime();
    CGMailInfo::m_autoLooting = 0;
}

// PROVEN: 3e722487 IDENTICAL/NONE
// OFFSET: 0x0096D5D0/0x1406E4640
void CGMailInfo::SetSendItem(uint32_t index, WOWGUID item, WOWGUID container, uint8_t slot) {
    if (index >= 12) {
        return;
    }

    if (item) {
        for (uint32_t i = 0; i < 12; i++) {
            if (i != index && CGMailInfo::m_sendItems[i].m_guid == item) {
                memset(&CGMailInfo::m_sendItems[i], 0, sizeof(SendMailItem));
            }
        }
    }

    CGMailInfo::m_sendItems[index].m_guid = item;
    CGMailInfo::m_sendItems[index].m_container = container;
    CGMailInfo::m_sendItems[index].m_slot = slot;
}

// OFFSET: 0x0096D660
void CGMailInfo::GetSendItem(uint32_t index, WOWGUID& item, WOWGUID& container, uint8_t& slot) {
    if (index < 16) {
        item = CGMailInfo::m_sendItems[index].m_guid;
        container = CGMailInfo::m_sendItems[index].m_container;
        slot = CGMailInfo::m_sendItems[index].m_slot;
    } else {
        item = WOWGUID::Make(0);
        container = WOWGUID::Make(0);
        slot = 0;
    }
}

// OFFSET: 0x00464760
WOWGUID CGMailInfo::GetSendItemGuid(uint32_t index) {
    if (index < 16) {
        return CGMailInfo::m_sendItems[index].m_guid;
    }

    return WOWGUID::Make(0);
}

// PROVEN: 0c95f03f IDENTICAL/NONE
// OFFSET: 0x0096D6D0
uint32_t CGMailInfo::GetNumSendItems() {
    uint32_t count = 0;

    for (uint32_t i = 0; i < 16; i++) {
        if (CGMailInfo::m_sendItems[i].m_guid) {
            count++;
        }
    }

    return count;
}

// OFFSET: 0x005F2000
int32_t CGMailInfo::IsSendItem(WOWGUID item) {
    if (!item) {
        return 0;
    }

    for (uint32_t i = 0; i < 16; i++) {
        if (CGMailInfo::m_sendItems[i].m_guid == item) {
            return 1;
        }
    }

    return 0;
}

// OFFSET: 0x0096DF20/0x1406E4FA0
const char* GetSenderName(int32_t entry, int32_t type, WOWGUID guid, int32_t stationery) {
    switch (type) {
        case 0:
        case 5: {
            if (stationery == 61) {
                return FrameScript_GetText("GM_EMAIL_NAME", -1, GENDER_NOT_APPLICABLE);
            }

            NameCache* name = g_nameDBCache.GetRecord(guid.m_guid, guid, &SenderNameQueryCallback, NULL, false);
            return name ? name->m_name : NULL;
        }

        case 1: {
            if (stationery == 61) {
                return FrameScript_GetText("GM_EMAIL_NAME", -1, GENDER_NOT_APPLICABLE);
            }

            break;
        }

        case 2: {
            AuctionHouseRec* auctionHouse = g_auctionHouseDB.GetRecord(entry);

            if (auctionHouse) {
                return auctionHouse->m_name;
            }

            break;
        }

        case 3: {
            WOWGUID none = WOWGUID();
            CreatureStats_C* stats = g_creatureDBCache.GetRecord(entry, none, &SenderNameQueryCallback, NULL, false);

            if (stats) {
                return stats->GetName(0);
            }

            break;
        }

        case 4: {
            WOWGUID none = WOWGUID();
            GameObjectStats_C* stats = g_gameObjectDBCache.GetRecord(entry, none, &SenderNameQueryCallback, NULL, false);

            if (stats) {
                return stats->GetName(0);
            }

            break;
        }
    }

    return NULL;
}

// OFFSET: 0x0096E070
void CGMailInfo::ParseInboxSubject(uint32_t messageID) {
    for (uint32_t i = 0; i < CGMailInfo::m_numInboxItems; i++) {
        MailboxMessageInfo* message = CGMailInfo::m_inbox.m_data[i];

        if (message->m_messageID == static_cast<int32_t>(messageID)) {
            CGMailInfo::ParseInboxSubject(message);
            return;
        }
    }
}

// OFFSET: 0x0096D850
int32_t Packet_MSG_QUERY_NEXT_MAIL_TIME(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    msg->Get(CGMailInfo::m_nextMailTime);
    uint32_t count;
    msg->Get(count);

    for (uint32_t i = 0; i < count; i++) {
        if (i >= 3) {
            break;
        }

        MailSenderInfo& sender = s_recentSenders[i];
        msg->Get(sender.m_guid.m_guid);
        msg->Get(reinterpret_cast<uint32_t&>(sender.m_entry));
        msg->Get(reinterpret_cast<uint32_t&>(sender.m_messageType));
        msg->Get(reinterpret_cast<uint32_t&>(sender.m_stationery));
        msg->Get(sender.unk_0014);
    }

    for (uint32_t i = count; i < 3; i++) {
        MailSenderInfo& sender = s_recentSenders[i];
        sender.m_guid = WOWGUID();
        sender.m_entry = 0;
        sender.m_messageType = 0;
        sender.m_stationery = 0;
        sender.unk_0014 = 0.0f;
    }

    FrameScript_SignalEvent(SCRIPT_EVENT_UPDATE_PENDING_MAIL, NULL);
    return 1;
}

// OFFSET: 0x009704A0
void CGMailInfo::ShutdownGame() {
    uint32_t count = CGMailInfo::m_stationeries.m_count;

    for (uint32_t i = 0; i < count; i++) {
        delete CGMailInfo::m_stationeries.m_data[i];
    }

    CGMailInfo::m_stationeries.Clear();
    count = CGMailInfo::m_inbox.m_count;

    for (uint32_t i = 0; i < count; i++) {
        delete CGMailInfo::m_inbox.m_data[i];
    }

    CGMailInfo::m_inbox.Clear();
    ClientServices::ClearMessageHandler(SMSG_MAIL_COMMAND_RESULT);
    ClientServices::ClearMessageHandler(SMSG_MAIL_LIST_RESULT);
    ClientServices::ClearMessageHandler(MSG_QUERY_NEXT_MAIL_TIME);
    ClientServices::ClearMessageHandler(SMSG_RECEIVED_MAIL);
    ClientServices::ClearMessageHandler(SMSG_SHOW_MAILBOX);
}

// OFFSET: 0x0096F590/0x1406E69E0
void CGMailInfo::CloseMail(bool leavingWorld) {
    CGMailInfo::ClearSendMail(false);

    if (leavingWorld) {
        CGMailInfo::m_object = WOWGUID::Make(0);
        CGMailInfo::m_numInboxItems = 0;
        CGMailInfo::m_totalInboxItems = 0;
        CGMailInfo::m_autoLooting = NULL;
    } else if (CGMailInfo::m_object.m_guid) {
        CGGameUI::ClearInteractTarget(CGMailInfo::m_object, 0);
        CGMailInfo::m_object = WOWGUID::Make(0);
        CGMailInfo::m_autoLooting = NULL;
        FrameScript_SignalEvent(SCRIPT_EVENT_MAIL_CLOSED, NULL);

        if (CGMailInfo::m_doPendingQuery) {
            CGMailInfo::QueryNextMailTime();
        }
    }
}

// OFFSET: 0x00972A20
void CGMailInfo::LeaveWorld() {
    CGMailInfo::CloseMail(true);
    uint32_t count = CGMailInfo::m_stationeries.Count();

    for (uint32_t i = 0; i < count; i++) {
        g_itemSparseDB.CancelCallback(CGMailInfo::m_stationeries.m_data[i]->m_itemID, StationeryItemCallback, NULL);
    }
}

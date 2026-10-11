#include "MailInfo.h"
#include "WoW/Source/DB/Rec/ItemRec.h"
#include "WoW/Source/DB/Rec/ItemRecSparse.h"
#include "Engine/Source/Event/FrameTime.h"
#include <cstdint>
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"
#include "Engine/Source/lua-5.1/src/lua.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/StationeryRec.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"
#include "WoW/Source/DB/Rec/PackageRec.h"
#include "Engine/Source/Base/CDataStore.h"
#include "Engine/Source/Os/Time.h"
#include "WoW/Source/Net/Types.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"
#include "WoW/Source/JamClientAutoCode/JamUserClient.h"
#include "WoW/Source/Ui/Types.h"
#include "Engine/Source/Storm/H/storm.h"
#include "WoW/Source/DB/DBClient/DBCacheInstances.h"
#include "WoW/Source/Object/ObjectClient/Item_C.h"
#include <cstdio>
#include "WoW/Common/WowTime.h"
#include "WoW/Source/DB/Rec/MailTemplateRec.h"
#include "WoW/Source/Game/GameClient/NameCache.h"
#include "WoW/Source/Object/Item.h"
#include "WoW/Source/Object/ObjectClient/Bag_C.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/Ui/Calendar.h"
#include "WoW/Source/Ui/ChatFrame.h"
#include "WoW/Source/Ui/GameUI.h"
#include "WoW/Source/UIUtil/Tooltip.h"
#include "WoW/Source/Ui/ScriptEvents.h"
#include "Engine/Source/FrameXML/LoadXML.h"

// STATIC: 0x00F49160
static char s_stationeryTexture[260];

// STATIC: #0x00BEB318
static char s_packageTexture[260];

// STATIC: 0x00F49268
static char s_headerTexture[260];

// OFFSET: 0x0096DB10/0x1406E4B30
static int32_t Script_GetSendMailMoney(lua_State* L) {
    lua_pushnumber(L, CGMailInfo::m_sendMoney);
    return 1;
}

// OFFSET: 0x0096DBD0/0x1406E4BD0
static int32_t Script_GetSendMailCOD(lua_State* L) {
    lua_pushnumber(L, CGMailInfo::m_sendCOD);
    return 1;
}

// OFFSET: 0x0096DC90/0x1406E4C40
static int32_t Script_HasNewMail(lua_State* L) {
    if (fabsf(CGMailInfo::m_nextMailTime) < 2.3841858e-7f) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: 0x0096DC30/0x1406E4C00
static int32_t Script_GetInboxNumItems(lua_State* L) {
    lua_pushnumber(L, CGMailInfo::m_numInboxItems);
    lua_pushnumber(L, CGMailInfo::m_totalInboxItems);
    return 2;
}

// OFFSET: 0x0096E730/0x1406E5920
static int32_t Script_GetSelectedStationeryTexture(lua_State* L) {
    const StationeryRec* rec;

    if (CGMailInfo::m_selectedStationery && (rec = g_stationeryDB.GetRecord(CGMailInfo::m_selectedStationery))) {
        lua_pushstring(L, rec->m_texture);
        return 1;
    }

    lua_pushnil(L);
    return 1;
}

// OFFSET: 0x0096E920/0x1406E5B60
static int32_t Script_SelectPackage(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: SelectPackage(index)");
        return 0;
    }

    int32_t index = static_cast<int32_t>(lua_tonumber(L, 1)) - 1;
    const PackageRec* rec;

    if (index >= 0 && index < g_packageDB.m_numRecords && (rec = g_packageDB.GetRecordByIndex(index))) {
        CGMailInfo::m_selectedPackage = rec->m_id;
        return 0;
    }

    CGMailInfo::m_selectedPackage = 0;
    return 0;
}

// OFFSET: 0x0096EA50/0x1406E5C70
static int32_t Script_CheckInbox(lua_State* L) {
    if (CGMailInfo::m_commandPending) {
        return 0;
    }

    WOWGUID object = CGMailInfo::m_object;

    if (!object) {
        return 0;
    }

    uint32_t now = FrameTime::GetCurTimeMs();

    if (CGMailInfo::m_nextUpdateTime && static_cast<int32_t>(now - CGMailInfo::m_nextUpdateTime) < 0) {
        FrameScript_SignalEvent(SCRIPT_EVENT_MAIL_INBOX_UPDATE, NULL);
        return 0;
    }

    CGMailInfo::m_nextUpdateTime = now + 60000;
    CGMailInfo::m_commandPending = 1;
    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_MAIL_GET_LIST));
    msg.Put(object.m_guid);
    msg.Finalize();
    ClientServices::Send2(&msg);
    return 0;
}

// OFFSET: 0x0096EE90/0x1406E6140
static int32_t Script_TakeInboxMoney(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: TakeInboxMoney(index)");
        return 0;
    }

    if (!CGMailInfo::m_commandPending) {
        WOWGUID object = CGMailInfo::m_object;

        if (object) {
            uint32_t index = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1;

            if (index < CGMailInfo::m_numInboxItems) {
                MailboxMessageInfo* mail = CGMailInfo::m_inbox.m_data[index];

                if (mail && mail->m_money) {
                    CDataStore msg;
                    msg.Put(static_cast<uint32_t>(CMSG_MAIL_TAKE_MONEY));
                    msg.Put(object.m_guid);
                    msg.Put(static_cast<uint32_t>(mail->m_messageID));
                    msg.Put(mail->m_money);
                    msg.Finalize();
                    ClientServices::Send2(&msg);
                    CGMailInfo::m_commandPending = 1;
                }
            }
        }
    }

    return 0;
}

// OFFSET: 0x0096EFA0/0x1406E62A0
static int32_t Script_TakeInboxTextItem(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: TakeInboxTextItem(index)");
        return 0;
    }

    if (!CGMailInfo::m_commandPending) {
        WOWGUID object = CGMailInfo::m_object;

        if (object) {
            uint32_t index = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1;

            if (index < CGMailInfo::m_numInboxItems) {
                MailboxMessageInfo* mail = CGMailInfo::m_inbox.m_data[index];

                if (mail && (SStrLen(mail->m_body) || mail->m_mailTemplateID) && !(mail->m_flags & 0x4)) {
                    CDataStore msg;
                    msg.Put(static_cast<uint32_t>(CMSG_MAIL_CREATE_TEXT_ITEM));
                    msg.Put(object.m_guid);
                    msg.Put(static_cast<uint32_t>(mail->m_messageID));
                    msg.Finalize();
                    ClientServices::Send2(&msg);
                    CGMailInfo::m_commandPending = 1;
                }
            }
        }
    }

    return 0;
}

// OFFSET: 0x0096E6A0/0x1406E58A0
static int32_t Script_SelectStationery(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: SelectStationery(index)");
        return 0;
    }

    uint32_t index = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1;

    if (index < CGMailInfo::m_numStationeries) {
        CGMailInfo::m_selectedStationery = CGMailInfo::m_stationeries.m_data[index]->m_id;
    } else {
        CGMailInfo::m_selectedStationery = 0;
    }

    return 0;
}

// OFFSET: 0x00971A10/0x1406E95F0
static int32_t Script_GetStationeryInfo(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetStationeryInfo(index)");
        return 0;
    }

    uint32_t index = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1;
    StationeryInfo* stationery;
    ItemRecSparse* stats;
    ItemRec* item;
    WOWGUID none = { 0 };

    if (index < CGMailInfo::m_numStationeries && (stationery = CGMailInfo::m_stationeries.m_data[index]) && (stats = g_itemSparseDB.GetRecord(stationery->m_itemID, none, NULL, NULL, false)) && (item = stats->GetNonSparse())) {
        const char* path = ClientDBStringLookup(3);
        SStrPrintf(s_stationeryTexture, sizeof(s_stationeryTexture), "%s%s%s", path, *path ? "\\" : "", CGItem_C::GetInventoryArt(item->m_displayInfoID));
        lua_pushstring(L, stats->GetName(0));
        lua_pushstring(L, s_stationeryTexture);

        if (stationery->m_itemGUID) {
            lua_pushnil(L);
        } else {
            lua_pushnumber(L, stats->GetBuyPrice(NULL));
        }

        return 3;
    }

    lua_pushnil(L);
    lua_pushnil(L);
    lua_pushnil(L);
    return 3;
}

// OFFSET: 0x0096DAB0/0x1406E4AC0
static int32_t Script_SetSendMailShowing(lua_State* L) {
    if (lua_type(L, 1) != LUA_TBOOLEAN) {
        luaL_error(L, "Usage: SetSendMailShowing(bool)");
        return 0;
    }

    CGMailInfo::m_sendMailShowing = lua_toboolean(L, 1) != 0;
    return 0;
}

// OFFSET: 0x0096E7D0/0x1406E59C0
static int32_t Script_GetPackageInfo(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetPackageInfo(index)");
        return 0;
    }

    int32_t index = static_cast<int32_t>(lua_tonumber(L, 1)) - 1;
    PackageRec* rec;

    if (index >= 0 && index < g_packageDB.m_numRecords && (rec = g_packageDB.GetRecordByIndex(index))) {
        const char* path = ClientDBStringLookup(3);
        SStrPrintf(s_packageTexture, sizeof(s_packageTexture), "%s%s", path, *path ? "\\" : "");
        SStrPack(s_packageTexture, rec->m_icon, sizeof(s_packageTexture));
        lua_pushstring(L, rec->m_name);
        lua_pushstring(L, s_packageTexture);
        lua_pushnumber(L, rec->m_cost);
        return 3;
    }

    lua_pushnil(L);
    lua_pushnil(L);
    lua_pushnumber(L, 0.0);
    return 3;
}

// OFFSET: 0x0096E7A0/0x1406E59A0
static int32_t Script_GetNumPackages(lua_State* L) {
    lua_pushnumber(L, g_packageDB.m_numRecords);
    return 1;
}

// OFFSET: 0x00972AE0/0x1406EAB10
static int32_t Script_GetSendMailPrice(lua_State* L) {
    uint64_t price = 30;
    uint64_t itemPrice = 30 * CGMailInfo::GetNumSendItems();
    price = itemPrice < price ? price : itemPrice;

    if (CGMailInfo::m_selectedStationery) {
        for (uint32_t i = 0; i < CGMailInfo::m_numStationeries; i++) {
            StationeryInfo* stationery = CGMailInfo::m_stationeries.m_data[i];

            if (stationery->m_id == CGMailInfo::m_selectedStationery) {
                if (!stationery->m_itemGUID) {
                    ItemRecSparse* stats = g_itemSparseDB.GetRecord(stationery->m_itemID, WOWGUID::Make(0), NULL, NULL, false);

                    if (stats) {
                        price += stats->GetBuyPrice(NULL);
                    }
                }

                break;
            }
        }
    }

    if (CGMailInfo::m_selectedPackage) {
        PackageRec* rec = g_packageDB.GetRecord(CGMailInfo::m_selectedPackage);

        if (rec) {
            price += rec->m_cost;
        }
    }

    lua_pushnumber(L, price + CGMailInfo::m_sendMoney);
    return 1;
}

// OFFSET: 0x0096DB00/0x1406E4B10
static int32_t Script_ClearSendMail(lua_State* L) {
    CGMailInfo::ClearSendMail(true);
    return 0;
}

// OFFSET: 0x0096DB70/0x1406E4B60
static int32_t Script_SetSendMailCOD(lua_State* L) {
    uint64_t amount;

    if (!lua_isnumber(L, 1) || !Script_GetMoney(L, 1, &amount)) {
        luaL_error(L, "Usage: SetSendMailCOD(amount)");
        return 0;
    }

    CGMailInfo::SetSendCOD(amount);
    return 0;
}

// OFFSET: 0x0096E9B0/0x1406E5BE0
static int32_t Script_GetSendMailItemLink(lua_State* L) {
    uint32_t index;

    if (lua_isnumber(L, 1)) {
        index = static_cast<int32_t>(lua_tonumber(L, 1) - 1.0);

        if (index >= 16) {
            return 0;
        }
    } else {
        index = 0;
    }

    WOWGUID guid = CGMailInfo::m_sendItems[index].m_guid;

    if (!guid) {
        return 0;
    }

    CGItem_C* item = static_cast<CGItem_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_ITEM, __FILE__, __LINE__));

    if (!item) {
        return 0;
    }

    lua_pushstring(L, CGTooltip::GetItemHyperlinkString(item));
    return 1;
}

// OFFSET: 0x00971D60/0x1406E9970
static int32_t Script_GetInboxHeaderInfo(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetInboxHeaderInfo(index)");
        return 0;
    }

    lua_checkstack(L, 13);
    uint32_t index = static_cast<int32_t>(lua_tonumber(L, 1)) - 1;
    MailboxMessageInfo* message;

    if (index >= CGMailInfo::m_numInboxItems || !(message = CGMailInfo::m_inbox.m_data[index])) {
        lua_pushnil(L);
        lua_pushnil(L);
        lua_pushnil(L);
        lua_pushnil(L);
        lua_pushnumber(L, 0.0);
        lua_pushnumber(L, 0.0);
        lua_pushnumber(L, 0.0);
        lua_pushnil(L);
        lua_pushnil(L);
        lua_pushnil(L);
        lua_pushnil(L);
        lua_pushnil(L);
        lua_pushnil(L);
        lua_pushnil(L);
        return 14;
    }

    uint32_t numAttachments = GetNumAttachments(message);
    MailAttachmentInfo* attachment = numAttachments == 1 ? CGMailInfo::GetFirstAttachment(message) : NULL;
    const char* iconPath = ClientDBStringLookup(3);
    SStrPrintf(s_headerTexture, sizeof(s_headerTexture), "%s%s", iconPath, *iconPath ? "\\" : "");
    PackageRec* package = message->m_package ? g_packageDB.GetRecord(message->m_package) : NULL;

    if (package) {
        SStrPack(s_headerTexture, package->m_icon, sizeof(s_headerTexture));
        lua_pushstring(L, s_headerTexture);
    } else if (numAttachments > 1) {
        SStrPack(s_headerTexture, CGItem_C::GetInventoryArt(7913), sizeof(s_headerTexture));
        lua_pushstring(L, s_headerTexture);
    } else {
        ItemRecSparse* stats = NULL;

        if (attachment && attachment->m_itemID) {
            WOWGUID none = WOWGUID();
            stats = g_itemSparseDB.GetRecord(attachment->m_itemID, none, &InboxItemQueryCallback, NULL, false);
        }

        if (stats) {
            SStrPack(s_headerTexture, CGItem_C::GetInventoryArt(stats->GetNonSparse()->m_displayInfoID), sizeof(s_headerTexture));
            lua_pushstring(L, s_headerTexture);
        } else {
            lua_pushnil(L);
        }
    }

    SStrPrintf(s_headerTexture, sizeof(s_headerTexture), "%s%s", iconPath, *iconPath ? "\\" : "");
    StationeryRec* stationery = g_stationeryDB.GetRecord(message->m_stationery);
    ItemRecSparse* stationeryStats = NULL;

    if (stationery) {
        WOWGUID none = WOWGUID();
        stationeryStats = g_itemSparseDB.GetRecord(stationery->m_itemID, none, &InboxItemQueryCallback, NULL, false);
    }

    if (stationeryStats) {
        SStrPack(s_headerTexture, CGItem_C::GetInventoryArt(stationeryStats->GetNonSparse()->m_displayInfoID), sizeof(s_headerTexture));
        lua_pushstring(L, s_headerTexture);
    } else {
        lua_pushnil(L);
    }

    bool isGM = message->m_stationery == 61;
    lua_pushstring(L, GetSenderName(message->m_senderEntry, message->m_messageType, message->m_sender, message->m_stationery));

    if (message->m_flags & 0x8) {
        char subject[512];
        SStrPrintf(subject, sizeof(subject), FrameScript_GetText("COD_PAYMENT", -1, GENDER_NOT_APPLICABLE), static_cast<const char*>(message->m_subject));
        lua_pushstring(L, subject);
    } else {
        lua_pushstring(L, message->m_subject);
    }

    lua_pushnumber(L, message->m_money);
    lua_pushnumber(L, message->m_cod);
    lua_pushnumber(L, message->m_expireTime);

    if (GetNumAttachments(message)) {
        lua_pushnumber(L, GetNumAttachments(message));
    } else {
        lua_pushnil(L);
    }

    if (message->m_flags & 0x1) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    if ((message->m_flags & 0x2) || (message->m_flags & 0x200)) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    if (message->m_flags & 0x4) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    if (message->m_messageType || isGM) {
        lua_pushnil(L);

        if (isGM) {
            lua_pushnumber(L, 1.0);
        } else {
            lua_pushnil(L);
        }
    } else {
        lua_pushnumber(L, 1.0);
        lua_pushnil(L);
    }

    if (numAttachments == 1 && attachment) {
        lua_pushnumber(L, static_cast<int32_t>(attachment->m_count));
    } else {
        lua_pushnil(L);
    }

    return 14;
}

// OFFSET: 0x0096EB30/0x1406E5D70
static int32_t Script_GetInboxInvoiceInfo(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetInboxInvoiceInfo(index)");
        return 0;
    }

    uint32_t index = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1;
    MailboxMessageInfo* message;

    if (index >= CGMailInfo::m_numInboxItems || !(message = CGMailInfo::m_inbox.m_data[index]) || !SStrLen(message->m_body) || !message->m_invoiceItemID) {
        lua_pushnil(L);
        lua_pushnil(L);
        lua_pushnil(L);
        lua_pushnumber(L, 0.0);
        lua_pushnumber(L, 0.0);
        lua_pushnumber(L, 0.0);
        lua_pushnumber(L, 0.0);
        return 7;
    }

    char itemName[256];
    CGItem_C::BuildItemName(itemName, sizeof(itemName), message->m_invoiceItemID, message->m_invoiceRandomPropertiesID);
    WOWGUID player = WOWGUID();
    uint64_t bid = 0;
    uint64_t buyout = 0;
    int32_t deposit = 0;
    uint64_t consignment = 0;
    uint32_t moneyDelay = 0;
    uint32_t etaTime = 0;

    switch (message->m_invoiceType) {
        case 2:
            sscanf(message->m_body, "%16llX:%llu:%llu:%d:%llu", &player.m_guid, &bid, &buyout, &deposit, &consignment);
            break;

        case 6:
            sscanf(message->m_body, "%16llX:%llu:%llu:%d:%llu:%d:%d", &player.m_guid, &bid, &buyout, &deposit, &consignment, &moneyDelay, &etaTime);
            break;

        case 1:
            sscanf(message->m_body, "%16llX:%llu:%llu", &player.m_guid, &bid, &buyout);
            break;
    }

    WowTime eta;
    WowTime::WowDecodeTime(etaTime, &eta);
    eta.m_tzHours = 0;

    if (message->m_invoiceType == 2) {
        lua_pushstring(L, "seller");
    } else if (message->m_invoiceType == 6) {
        lua_pushstring(L, "seller_temp_invoice");
    } else {
        lua_pushstring(L, "buyer");
    }

    lua_pushstring(L, itemName);
    NameCache* name = g_nameDBCache.GetRecord(player.m_guid, player, &InboxItemQueryCallback, NULL, false);
    lua_pushstring(L, name ? name->m_name : "");
    lua_pushnumber(L, bid);
    lua_pushnumber(L, buyout);
    lua_pushnumber(L, deposit);
    lua_pushnumber(L, consignment);
    lua_pushnumber(L, moneyDelay);
    lua_pushnumber(L, eta.m_hour);
    lua_pushnumber(L, eta.m_minute);
    lua_pushnumber(L, message->m_invoiceStackCount);
    return 11;
}

// OFFSET: 0x009725F0/0x1406EA430
static int32_t Script_GetInboxItemLink(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetInboxItemLink(messageIndex, attachIndex)");
        return 0;
    }

    uint32_t index = static_cast<int32_t>(lua_tonumber(L, 1)) - 1;
    uint32_t attachIndex = lua_isnumber(L, 2) ? static_cast<int32_t>(lua_tonumber(L, 2) - 1.0) : -1;

    MailAttachmentInfo* attachment = GetInboxAttachment(index, attachIndex);

    if (!attachment || !attachment->m_itemID) {
        return 0;
    }

    WOWGUID none = WOWGUID();
    ItemRecSparse* stats = g_itemSparseDB.GetRecord(attachment->m_itemID, none, &InboxItemQueryCallback, NULL, false);

    if (!stats) {
        return 0;
    }

    lua_pushstring(L, CGTooltip::GetItemHyperlinkString(attachment->m_itemID, stats->m_quality, attachment->m_enchantIDs[0], &attachment->m_enchantIDs[2], attachment->m_randomPropertiesID, attachment->m_suffixFactor, attachment->m_enchantIDs[8]));
    return 1;
}

// OFFSET: 0x0096F0C0/0x1406E6410
static int32_t Script_ReturnInboxItem(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: ReturnInboxItem(index)");
        return 0;
    }

    if (CGMailInfo::m_commandPending) {
        return 0;
    }

    WOWGUID mailbox = CGMailInfo::m_object;

    if (!mailbox) {
        return 0;
    }

    uint32_t index = static_cast<int32_t>(lua_tonumber(L, 1)) - 1;

    if (index >= CGMailInfo::m_numInboxItems) {
        return 0;
    }

    MailboxMessageInfo* message = CGMailInfo::m_inbox.m_data[index];

    if (!message || message->m_deletePending || message->m_messageType || (!(GetNumAttachments(message) > 0) && !message->m_money) || (message->m_flags & 0x20A)) {
        return 0;
    }

    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_MAIL_RETURN_TO_SENDER));
    msg.Put(mailbox.m_guid);
    msg.Put(static_cast<uint32_t>(message->m_messageID));
    msg.Put(message->m_sender.m_guid);
    msg.Finalize();
    ClientServices::Send2(&msg);
    message->m_deletePending = 1;
    CGMailInfo::m_commandPending = 1;
    return 0;
}

// OFFSET: 0x0096F210/0x1406E65A0
static int32_t Script_DeleteInboxItem(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: DeleteInboxItem(index)");
        return 0;
    }

    if (CGMailInfo::m_commandPending) {
        return 0;
    }

    WOWGUID mailbox = CGMailInfo::m_object;

    if (!mailbox) {
        return 0;
    }

    uint32_t index = static_cast<int32_t>(lua_tonumber(L, 1)) - 1;

    if (index >= CGMailInfo::m_numInboxItems) {
        return 0;
    }

    MailboxMessageInfo* message = CGMailInfo::m_inbox.m_data[index];

    if (!message || message->m_deletePending) {
        return 0;
    }

    if (!(message->m_flags & 0x20A) && !message->m_messageType && (GetNumAttachments(message) || message->m_money)) {
        return 0;
    }

    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_MAIL_DELETE));
    msg.Put(mailbox.m_guid);
    msg.Put(static_cast<uint32_t>(message->m_messageID));
    msg.Put(static_cast<uint32_t>(0));
    msg.Finalize();
    ClientServices::Send2(&msg);
    message->m_deletePending = 1;
    CGMailInfo::m_commandPending = 1;
    return 0;
}

// OFFSET: 0x0096F350/0x1406E6730
static int32_t Script_InboxItemCanDelete(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: InboxItemCanDelete(index)");
        return 0;
    }

    uint32_t index = static_cast<int32_t>(lua_tonumber(L, 1)) - 1;
    MailboxMessageInfo* message;

    if (index < CGMailInfo::m_numInboxItems && (message = CGMailInfo::m_inbox.m_data[index]) && ((message->m_flags & 0x20A) || message->m_messageType || (!GetNumAttachments(message) && !message->m_money))) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: 0x0096F420/0x1406E6800
static int32_t Script_GetLatestThreeSenders(lua_State* L) {
    int32_t count = 0;

    for (uint32_t i = 0; i < 3; i++) {
        MailSenderInfo& sender = s_recentSenders[i];

        if ((sender.m_guid || sender.m_entry) && sender.unk_0014 <= 0.0f) {
            lua_pushstring(L, GetSenderName(sender.m_entry, sender.m_messageType, sender.m_guid, sender.m_stationery));
            count++;
        }
    }

    return count;
}

// OFFSET: 0x0096F480/0x1406E6890
static int32_t Script_RespondMailLockSendItem(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: RespondMailLockSendItem(slot, keepItem)");
        return 0;
    }

    uint32_t slot = lua_tointeger(L, 1) - 1;
    int32_t keepItem = StringToBOOL(L, 2, 0);

    if (!CGMailInfo::m_lockSendItems) {
        return 0;
    }

    CGMailInfo::m_lockSendItems = 0;

    if (keepItem) {
        FrameScript_SignalEvent(SCRIPT_EVENT_MAIL_UNLOCK_SEND_ITEMS, NULL);
        return 0;
    }

    WOWGUID item;
    WOWGUID container;
    uint8_t itemSlot;
    CGMailInfo::GetSendItem(slot, item, container, itemSlot);
    CGMailInfo::ClearSendItem(slot);

    if (item) {
        CGGameUI::UnlockItem(item);
    }

    FrameScript_SignalEvent(SCRIPT_EVENT_MAIL_SEND_INFO_UPDATE, NULL);
    FrameScript_SignalEvent(SCRIPT_EVENT_MAIL_UNLOCK_SEND_ITEMS, NULL);
    return 0;
}

// OFFSET: 0x009718A0/0x1406E9470
static int32_t Script_ClickSendMailItemButton(lua_State* L) {
    uint32_t index = lua_isnumber(L, 1) ? static_cast<int32_t>(lua_tonumber(L, 1) - 1.0) : -1;

    if (!lua_toboolean(L, 2)) {
        WOWGUID cursorItem;
        WOWGUID cursorContainer;
        uint32_t cursorSlot;
        CGGameUI::GetCursorItem(cursorItem, cursorContainer, cursorSlot);
        WOWGUID item;
        WOWGUID container;
        uint8_t slot;
        CGMailInfo::GetSendItem(index, item, container, slot);

        if (item == cursorItem) {
            CGGameUI::ClearCursor(1, 1);
            return 0;
        }

        if (cursorItem) {
            if (!CGMailInfo::AddItemToOutgoingMail(index, cursorItem, cursorContainer, static_cast<uint8_t>(cursorSlot))) {
                return 0;
            }
        } else {
            CGMailInfo::ClearSendItem(index);
        }

        CGGameUI::SetCursorItem(item, container, slot, 0, 0, 1);
        FrameScript_SignalEvent(SCRIPT_EVENT_MAIL_SEND_INFO_UPDATE, NULL);
        return 0;
    }

    if (index >= 16) {
        return 0;
    }

    WOWGUID item = CGMailInfo::m_sendItems[index].m_guid;

    if (!item) {
        return 0;
    }

    CGMailInfo::ClearSendItem(index);
    CGGameUI::UnlockItem(item);
    FrameScript_SignalEvent(SCRIPT_EVENT_MAIL_SEND_INFO_UPDATE, NULL);
    return 0;
}

// OFFSET: 0x0096FD40/0x1406E7340
static int32_t Script_SetSendMailMoney(lua_State* L) {
    uint64_t amount;

    if (!lua_isnumber(L, 1) || !Script_GetMoney(L, 1, &amount)) {
        luaL_error(L, "Usage: SetSendMailMoney(amount)");
        return 0;
    }

    if (CGMailInfo::SetSendMoney(amount)) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: 0x00971B90/0x1406E9780
static int32_t Script_GetSendMailItem(lua_State* L) {
    uint32_t index;

    if (lua_isnumber(L, 1)) {
        index = static_cast<int32_t>(lua_tonumber(L, 1) - 1.0);

        if (index >= 16) {
            lua_pushnil(L);
            lua_pushnil(L);
            lua_pushnumber(L, 0.0);
            lua_pushnumber(L, 0.0);
            return 4;
        }
    } else {
        index = 0;
    }

    WOWGUID guid = CGMailInfo::m_sendItems[index].m_guid;
    CGItem_C* item = guid ? static_cast<CGItem_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_ITEM, __FILE__, __LINE__)) : NULL;
    ItemRecSparse* stats = NULL;
    ItemRec* itemRec = NULL;

    if (item) {
        WOWGUID none = WOWGUID();
        stats = g_itemSparseDB.GetRecord(item->m_obj->m_entryID, none, &SendItemQueryCallback, NULL, false);
    }

    if (stats) {
        itemRec = stats->GetNonSparse();
    }

    if (!itemRec) {
        lua_pushnil(L);
        lua_pushnil(L);
        lua_pushnumber(L, 0.0);
        lua_pushnumber(L, 0.0);
        return 4;
    }

    char name[256];
    item->GetFullItemName(name, sizeof(name));
    lua_pushstring(L, name);
    const char* iconPath = ClientDBStringLookup(3);
    char texture[260];
    SStrPrintf(texture, sizeof(texture), "%s%s%s", iconPath, *iconPath ? "\\" : "", CGItem_C::GetInventoryArt(itemRec->m_displayInfoID));
    lua_pushstring(L, texture);
    lua_pushnumber(L, item->m_item->m_itemFieldStackCount);
    lua_pushnumber(L, stats->m_inventoryType ? stats->m_quality : -1);
    return 4;
}

// OFFSET: 0x00970830/0x1406E7FC0
static int32_t Script_SendMail(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: SendMail(target, subject, body)");
        return 0;
    }

    if (CGMailInfo::m_commandPending) {
        return 0;
    }

    WOWGUID mailbox = CGMailInfo::m_object;

    if (!mailbox || CGMailInfo::m_sendPending) {
        return 0;
    }

    const char* target = lua_tolstring(L, 1, NULL);

    if (!*target || !lua_isstring(L, 2)) {
        return 0;
    }

    const char* subject = lua_tolstring(L, 2, NULL);

    if (!*subject) {
        return 0;
    }

    const char* body = lua_isstring(L, 3) ? lua_tolstring(L, 3, NULL) : "";
    int32_t stationery = CGMailInfo::m_selectedStationery;

    if (!stationery) {
        return 0;
    }

    uint32_t count = 0;

    for (uint32_t i = 0; i < 16; i++) {
        WOWGUID guid = CGMailInfo::GetSendItemGuid(i);

        if (!guid) {
            continue;
        }

        count++;
        CGItem_C* item = static_cast<CGItem_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_ITEM, __FILE__, __LINE__));

        if (!item || item->m_item->m_itemFieldOwner != ClntObjMgrGetActivePlayer()) {
            CGGameUI::DisplayError(static_cast<UIErrors>(GetDisplayErrorByInventoryError(23)));
            CGMailInfo::ClearSendItem(i);
            FrameScript_SignalEvent(SCRIPT_EVENT_MAIL_SEND_INFO_UPDATE, NULL);
            return 0;
        }
    }

    int32_t package = count ? CGMailInfo::m_selectedPackage : 0;
    uint64_t money = CGMailInfo::m_sendMoney;
    uint64_t cod = CGMailInfo::m_sendCOD;

    if (money ? cod != 0 : (cod && !count)) {
        return 0;
    }

    UserClientSendMail message;
    memcpy(message.m_mailbox, &mailbox, sizeof(message.m_mailbox));
    SStrCopy(message.m_target, target, sizeof(message.m_target));
    SStrCopy(message.m_subject, subject, sizeof(message.m_subject));
    SStrCopy(message.m_body, body, sizeof(message.m_body));
    message.m_stationeryID = stationery;
    message.m_packageID = package;
    message.m_attachments.SetCount(count);
    uint32_t index = 0;

    for (uint32_t i = 0; i < 16; i++) {
        WOWGUID guid = CGMailInfo::GetSendItemGuid(i);

        if (guid) {
            message.m_attachments[index].m_attachPosition = static_cast<uint8_t>(i);
            memcpy(message.m_attachments[index].m_itemGUID, &guid, sizeof(guid));
            index++;
        }
    }

    message.m_sendMoney = money;
    message.m_cod = cod;
    ClientServices::Send(&message);
    CGMailInfo::m_sendPending = 1;
    CGMailInfo::m_commandPending = 1;
    return 0;
}

// OFFSET: 0x0096FDD0/0x1406E73E0
static int32_t Script_GetInboxText(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetInboxText(index)");
        return 0;
    }

    uint32_t index = static_cast<int32_t>(lua_tonumber(L, 1)) - 1;
    MailboxMessageInfo* message;

    if (index >= CGMailInfo::m_numInboxItems || !(message = CGMailInfo::m_inbox.m_data[index])) {
        lua_pushnil(L);
        lua_pushnil(L);
        lua_pushnil(L);
        lua_pushnil(L);
        return 4;
    }

    CGMailInfo::MarkAsRead(index);
    char buffer[8000];
    char text[8000];

    if (!SStrLen(message->m_body)) {
        MailTemplateRec* mailTemplate = message->m_mailTemplateID ? g_mailTemplateDB.GetRecord(message->m_mailTemplateID) : NULL;

        if (mailTemplate) {
            WOWGUID player = ClntObjMgrGetActivePlayer();
            QuestParserParseText(mailTemplate->m_body, buffer, sizeof(buffer), player, NULL, 0);
            lua_pushstring(L, buffer);
        } else {
            lua_pushnil(L);
        }
    } else {
        switch (message->m_messageType) {
            case 2:
                lua_pushnil(L);
                break;

            case 0:
                SStrCopy(text, message->m_body, sizeof(text));
                ProfanityFilter(text, false, false);
                lua_pushstring(L, text);
                break;

            case 5: {
                CGCalendar::GetDateTimeText(buffer, sizeof(buffer), SStrToUnsigned(message->m_body));
                const char* format = FrameScript_GetText((message->m_flags & 0x100) ? "CALENDAR_INVITE_REMOVED_MAIL_BODY" : "CALENDAR_EVENT_REMOVED_MAIL_BODY", -1, GENDER_NOT_APPLICABLE);

                if (!format || !*format) {
                    text[0] = '\0';
                } else {
                    SStrPrintf(text, sizeof(text), format, GetSenderName(message->m_senderEntry, message->m_messageType, message->m_sender, message->m_stationery), message->m_invoiceSubject, buffer);
                }

                lua_pushstring(L, text);
                break;
            }

            default: {
                WOWGUID player = ClntObjMgrGetActivePlayer();
                QuestParserParseText(message->m_body, text, sizeof(text), player, NULL, 0);
                lua_pushstring(L, text);
                break;
            }
        }
    }

    StationeryRec* stationery = g_stationeryDB.GetRecord(message->m_stationery);

    if (stationery) {
        lua_pushstring(L, stationery->m_texture);
    } else {
        lua_pushnil(L);
    }

    if ((!SStrLen(message->m_body) && !message->m_mailTemplateID) || (message->m_flags & 0x4) || message->m_stationery == 61 || message->m_messageType == 5 || (message->m_messageType == 2 && !message->m_invoiceItemID)) {
        lua_pushnil(L);
    } else {
        lua_pushnumber(L, 1.0);
    }

    if (message->m_invoiceItemID) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 4;
}

// OFFSET: 0x00972360/0x1406EA110
static int32_t Script_GetInboxItem(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetInboxItem(messageIndex, attachIndex)");
        return 0;
    }

    uint32_t index = static_cast<int32_t>(lua_tonumber(L, 1)) - 1;
    uint32_t attachIndex = lua_isnumber(L, 2) ? static_cast<int32_t>(lua_tonumber(L, 2) - 1.0) : -1;
    MailAttachmentInfo* attachment = GetInboxAttachment(index, attachIndex);

    if (!attachment || !attachment->m_itemID) {
        lua_pushnil(L);
        lua_pushnil(L);
        lua_pushnumber(L, 0.0);
        lua_pushnumber(L, 0.0);
        lua_pushnil(L);
        return 5;
    }

    WOWGUID requester = WOWGUID::Make(0);
    ItemRecSparse* stats = g_itemSparseDB.GetRecord(attachment->m_itemID, requester, &InboxItemQueryCallback, NULL, false);
    ItemRec* item;
    char texture[260];

    if (!stats || !(item = stats->GetNonSparse())) {
        lua_pushstring(L, FrameScript_GetText("UNKNOWNOBJECT", -1, GENDER_NOT_APPLICABLE));
        const char* iconPath = ClientDBStringLookup(3);
        SStrPrintf(texture, sizeof(texture), "%s%s%s", iconPath, *iconPath ? "\\" : "", "INV_Misc_QuestionMark");
        lua_pushstring(L, texture);
        lua_pushnumber(L, 0.0);
        lua_pushnumber(L, 0.0);
        lua_pushnil(L);
        return 5;
    }

    char name[256];
    CGItem_C::BuildItemName(name, sizeof(name), attachment->m_itemID, attachment->m_randomPropertiesID);
    lua_pushstring(L, name);
    const char* iconPath = ClientDBStringLookup(3);
    SStrPrintf(texture, sizeof(texture), "%s%s%s", iconPath, *iconPath ? "\\" : "", CGItem_C::GetInventoryArt(item->m_displayInfoID));
    lua_pushstring(L, texture);
    lua_pushnumber(L, static_cast<int32_t>(attachment->m_count));
    lua_pushnumber(L, stats->m_inventoryType ? stats->m_quality : -1);
    CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();
    int32_t error;

    if (!player || player->CanUseItem(stats, error)) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 5;
}

// OFFSET: 0x00970140/0x1406E77E0
static int32_t Script_TakeInboxItem(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: TakeInboxItem(messageIndex, attachIndex)");
        return 0;
    }

    if (CGMailInfo::m_commandPending) {
        return 0;
    }

    WOWGUID mailbox = CGMailInfo::m_object;

    if (!mailbox) {
        return 0;
    }

    uint32_t index = static_cast<int32_t>(lua_tonumber(L, 1)) - 1;

    if (index >= CGMailInfo::m_numInboxItems) {
        return 0;
    }

    MailboxMessageInfo* message = CGMailInfo::m_inbox.m_data[index];

    if (!message) {
        return 0;
    }

    uint32_t attachIndex = lua_isnumber(L, 2) ? static_cast<int32_t>(lua_tonumber(L, 2)) - 1 : -1;
    MailAttachmentInfo* attachment = GetAttachment(message, attachIndex);

    if (!attachment || !attachment->m_itemID) {
        return 0;
    }

    CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();

    if (!player || player->GetCoinage() < message->m_cod) {
        CGGameUI::DisplayError(ERR_NOT_ENOUGH_MONEY);
        return 0;
    }

    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_MAIL_TAKE_ITEM));
    msg.Put(mailbox.m_guid);
    msg.Put(static_cast<uint32_t>(message->m_messageID));
    msg.Put(attachment->m_itemGuid);
    msg.Finalize();
    ClientServices::Send2(&msg);
    CGMailInfo::m_commandPending = 1;
    return 0;
}

// OFFSET: 0x009702D0/0x1406E79C0
static int32_t Script_ComplainInboxItem(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: ComplainInboxItem(index)");
        return 0;
    }

    CGMailInfo::Complain(static_cast<int32_t>(lua_tonumber(L, 1)) - 1);
    return 0;
}

// OFFSET: 0x00970340/0x1406E7A10
static int32_t Script_CanComplainInboxItem(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: InboxItemCanComplain(index)");
        return 0;
    }

    if (CGMailInfo::CanComplain(static_cast<int32_t>(lua_tonumber(L, 1)) - 1)) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: 0x009703D0/0x1406E7A90
static int32_t Script_AutoLootMailItem(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: AutoLootMailItem(index)");
        return 0;
    }

    if (!CGMailInfo::m_commandPending && CGMailInfo::m_object) {
        uint32_t index = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1;
        CGMailInfo::AutoLoot(index);
    }

    return 0;
}

// OFFSET: 0x0096FCD0
static int32_t Script_CloseMail(lua_State* L) {
    CGMailInfo::ClearSendMail(false);

    if (CGMailInfo::m_object) {
        CGGameUI::ClearInteractTarget(CGMailInfo::m_object, 0);
        CGMailInfo::m_object = WOWGUID::Make(0);
        CGMailInfo::m_autoLooting = NULL;
        FrameScript_SignalEvent(SCRIPT_EVENT_MAIL_CLOSED, NULL);

        if (CGMailInfo::m_doPendingQuery) {
            CGMailInfo::QueryNextMailTime();
        }
    }

    return 0;
}

// OFFSET: 0x00972A90/0x1406EAAD0
static int32_t Script_GetNumStationeries(lua_State* L) {
    if (!CGMailInfo::m_pendingStationeryCallbacks) {
        CGMailInfo::UpdateStationeryList();
    }

    lua_pushnumber(L, CGMailInfo::m_numStationeries);
    return 1;
}

// STATIC: 0x00D13348
static FrameScript_Method s_ScriptFunctions[] = {
    { "CloseMail", &Script_CloseMail },
    { "ClearSendMail", &Script_ClearSendMail },
    { "ClickSendMailItemButton", &Script_ClickSendMailItemButton },
    { "SetSendMailMoney", &Script_SetSendMailMoney },
    { "GetSendMailMoney", &Script_GetSendMailMoney },
    { "SetSendMailCOD", &Script_SetSendMailCOD },
    { "GetSendMailCOD", &Script_GetSendMailCOD },
    { "GetNumStationeries", &Script_GetNumStationeries },
    { "GetStationeryInfo", &Script_GetStationeryInfo },
    { "SelectStationery", &Script_SelectStationery },
    { "GetSelectedStationeryTexture", &Script_GetSelectedStationeryTexture },
    { "GetNumPackages", &Script_GetNumPackages },
    { "GetPackageInfo", &Script_GetPackageInfo },
    { "SelectPackage", &Script_SelectPackage },
    { "GetSendMailItem", &Script_GetSendMailItem },
    { "GetSendMailItemLink", &Script_GetSendMailItemLink },
    { "GetSendMailPrice", &Script_GetSendMailPrice },
    { "SendMail", &Script_SendMail },
    { "CheckInbox", &Script_CheckInbox },
    { "GetInboxNumItems", &Script_GetInboxNumItems },
    { "GetInboxHeaderInfo", &Script_GetInboxHeaderInfo },
    { "GetInboxText", &Script_GetInboxText },
    { "GetInboxInvoiceInfo", &Script_GetInboxInvoiceInfo },
    { "GetInboxItem", &Script_GetInboxItem },
    { "GetInboxItemLink", &Script_GetInboxItemLink },
    { "TakeInboxMoney", &Script_TakeInboxMoney },
    { "TakeInboxItem", &Script_TakeInboxItem },
    { "TakeInboxTextItem", &Script_TakeInboxTextItem },
    { "ReturnInboxItem", &Script_ReturnInboxItem },
    { "DeleteInboxItem", &Script_DeleteInboxItem },
    { "InboxItemCanDelete", &Script_InboxItemCanDelete },
    { "HasNewMail", &Script_HasNewMail },
    { "ComplainInboxItem", &Script_ComplainInboxItem },
    { "CanComplainInboxItem", &Script_CanComplainInboxItem },
    { "GetLatestThreeSenders", &Script_GetLatestThreeSenders },
    { "SetSendMailShowing", &Script_SetSendMailShowing },
    { "AutoLootMailItem", &Script_AutoLootMailItem },
    { "RespondMailLockSendItem", &Script_RespondMailLockSendItem },
};

// OFFSET: 0x0096DCE0
void MailInfoRegisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_RegisterFunction(s_ScriptFunctions[i].m_name, s_ScriptFunctions[i].m_method);
    }
}

// OFFSET: 0x0096DD10
void MailInfoUnregisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_UnregisterFunction(s_ScriptFunctions[i].m_name);
    }
}

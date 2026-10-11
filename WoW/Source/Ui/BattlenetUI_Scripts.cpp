#include "BattlenetUI.h"
#include <cstdint>
#include <cstdio>
#include <cstring>
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"
#include "Engine/Source/FrameXML/LoadXML.h"
#include "Engine/Source/Gxu/GxuFont.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"
#include "Engine/Source/lua-5.1/src/lua.h"
#include "Engine/Source/Os/Time.h"
#include "Engine/Source/Storm/H/storm.h"
#include "WoW/Source/Game/ValidateName.h"
#include "WoW/Source/JamClientAutoCode/JamUserClient.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/Ui/ChatFrame.h"
#include "WoW/Source/Ui/GameUI.h"
#include "WoW/Source/WowServices/BattlenetLogin.h"
#include "WoW/Source/WowServices/Login.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"

// OFFSET: 0x00902320/0x14065A930
static int32_t Script_BNConnected(lua_State* L) {
    bool connected = false;
    Login* login = ClientServices::LoginConnection();

    if (login && login->GetLoginServerType() == 1) {
        int32_t state = static_cast<BattlenetLogin*>(login)->m_serviceState;

        if (state == 7 || state == 6 || state == 3) {
            connected = true;
        }
    }

    lua_pushboolean(L, connected);
    return 1;
}

// OFFSET: 0x00902380/0x14065A9A0
static int32_t Script_IsBNLogin(lua_State* L) {
    Login* login = ClientServices::LoginConnection();
    bool isBattlenet = login && login->GetLoginServerType() == 1;
    lua_pushboolean(L, isBattlenet);
    return 1;
}

// OFFSET: 0x009023C0/0x14065A9E0
static int32_t Script_BNInviteFriend(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: BNInviteFriend(presenceID, [tank], [heal], [dps])");
        return 0;
    }

    uint32_t roles = 0;
    uint32_t presenceId = static_cast<uint32_t>(lua_tonumber(L, 1));

    if (StringToBOOL(L, 2, 0)) {
        roles = 2;
    }

    if (StringToBOOL(L, 3, 0)) {
        roles |= 4;
    }

    if (StringToBOOL(L, 4, 0)) {
        roles |= 8;
    }

    CGPlayer_C::InviteToon(presenceId, roles);
    return 0;
}

// OFFSET: 0x009055E0/0x14065E310
static int32_t Script_BNGetInfo(lua_State* L) {
    Login* login = ClientServices::LoginConnection();

    if (!login || login->GetLoginServerType() != 1 || login->IsTrialAccount() || !BattlenetAPI::IsRIDEnabled()) {
        return 0;
    }

    uint32_t accountId = BattlenetAPI::GetPresenceIDForCurrentAccount();
    lua_pushnumber(L, accountId);
    DebugLog("BNUI: GetInfo account ID is %u", accountId);

    uint32_t toonId = BattlenetAPI::GetPresenceIDForCurrentToon();
    lua_pushnumber(L, toonId);
    DebugLog("BNUI: GetInfo toon ID is %u", toonId);

    Battlenet::Presence::FieldVal broadcast = BattlenetAPI::GetPresenceValue(toonId, BattlenetUI::s_presenceCallbacks[BN_PRESENCE_BROADCAST].m_field);

    if (broadcast.m_tag == 13) {
        lua_pushstring(L, broadcast.Value<BSN::String<127> >().m_data);
        DebugLog("BNUI: GetInfo custom message is %s", broadcast.Value<BSN::String<127> >().m_data);
    } else {
        lua_pushnil(L);
        DebugLog("BNUI: GetInfo custom message is <none>");
    }

    {
        Battlenet::Presence::FieldVal value = BattlenetAPI::GetPresenceValue(toonId, BattlenetUI::s_presenceCallbacks[BN_PRESENCE_AFK].m_field);
        uint8_t isAFK = value.m_tag == 11 ? value.Value<bool>() : 0;
        lua_pushboolean(L, isAFK);
        DebugLog("BNUI: GetInfo AFK is %d", isAFK);
    }

    {
        Battlenet::Presence::FieldVal value = BattlenetAPI::GetPresenceValue(toonId, BattlenetUI::s_presenceCallbacks[BN_PRESENCE_DND].m_field);
        uint8_t isDND = value.m_tag == 11 ? value.Value<bool>() : 0;
        lua_pushboolean(L, isDND);
        DebugLog("BNUI: GetInfo DND is %d", isDND);
    }

    return 5;
}

// OFFSET: 0x00905780/0x14065E4D0
static int32_t Script_BNGetNumFriends(lua_State* L) {
    Login* login = ClientServices::LoginConnection();

    if (login && login->GetLoginServerType() == 1 && !login->IsTrialAccount() && BattlenetAPI::IsRIDEnabled() && BattlenetUI::m_friendListLoaded) {
        if (BattlenetUI::FeaturesEnabledAndConnected()) {
            lua_pushnumber(L, static_cast<int32_t>(BattlenetUI::m_friends.Count()));

            uint32_t online = 0;

            for (uint32_t i = 0; i < BattlenetUI::m_friends.Count(); i++) {
                Battlenet::Presence::FieldVal value = BattlenetAPI::GetPresenceValue(BattlenetUI::m_friends[i].m_presenceID, BattlenetUI::s_presenceCallbacks[BN_PRESENCE_ONLINE].m_field);

                if (value.m_tag == 1 && value.Value<bool>()) {
                    online++;
                }
            }

            lua_pushnumber(L, online);
            return 2;
        }

        lua_pushnumber(L, 0.0);
        lua_pushnumber(L, 0.0);
        DebugLog("BNUI: Num Friends is 0, friends online is 0. NOT CONNECTED");
        return 2;
    }

    lua_pushnumber(L, 0.0);
    lua_pushnumber(L, 0.0);
    return 2;
}

// OFFSET: 0x009058D0/0x14065E650
static int32_t Script_BNSendSoR(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: BNSendSoR(presence[, \"comment\"])");
        return 0;
    }

    uint32_t presenceId = lua_tointeger(L, 1);
    const char* comment = "";

    if (lua_isstring(L, 2)) {
        comment = lua_tolstring(L, 2, NULL);
    }

    if (CGGameUI::s_canSendSoR) {
        Battlenet::Presence::FieldVal value = BattlenetAPI::GetPresenceValue(presenceId, BattlenetUI::s_presenceCallbacks[BN_PRESENCE_ACCOUNT_ID].m_field);
        uint32_t accountId;

        if (value.m_tag == 5 && (accountId = value.Value<uint32_t>()) != 0) {
            UserClientSendSorRequestViaBnetAccountId msg;
            msg.unk_040C = accountId;

            if (ValidateStringIsUTF8(comment) && ScrubString(comment, msg.unk_0010, 255, sizeof(msg.unk_0010), 6)) {
                const char* source = msg.unk_0010;
                char* dest = msg.unk_0010;

                do {
                    if (source[0] == '|' && source[1] == '|') {
                        *dest++ = '|';
                        source += 2;
                    } else {
                        *dest++ = *source++;
                    }
                } while (*source);

                ClientServices::Send(&msg);
                lua_pushboolean(L, 1);
                return 1;
            }

            lua_pushboolean(L, 0);
            return 1;
        }
    }

    lua_pushboolean(L, 0);
    return 1;
}

// OFFSET: 0x00905A30/0x14065E7D0
static int32_t Script_BNGetNumFriendToons(lua_State* L) {
    Login* login = ClientServices::LoginConnection();

    if (!login || login->GetLoginServerType() != 1 || login->IsTrialAccount() || !BattlenetAPI::IsRIDEnabled()) {
        lua_pushnumber(L, 0.0);
        return 1;
    }

    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: BNGetNumFriendToons(index)");
        return 0;
    }

    uint32_t index;

    if (!BattlenetUI::m_friendListLoaded || (index = static_cast<uint32_t>(lua_tonumber(L, 1)), index - 1 >= BattlenetUI::m_friends.Count())) {
        lua_pushnumber(L, 0.0);
        return 1;
    }

    uint32_t presenceId = BattlenetUI::m_friends[index - 1].m_presenceID;
    uint32_t numToons = BattlenetAPI::GetNumToons(presenceId);
    uint32_t online = 0;

    for (uint32_t i = 0; i < numToons; i++) {
        Battlenet::Presence::FieldVal value = BattlenetAPI::GetPresenceValue(BattlenetAPI::GetToon(presenceId, i), BattlenetUI::s_presenceCallbacks[BN_PRESENCE_ONLINE].m_field);

        if (value.m_tag == 1 && value.Value<bool>()) {
            online++;
        }
    }

    lua_pushnumber(L, online);
    DebugLog("BNUI: GetNumFriendToons is %d", online);
    return 1;
}

// OFFSET: 0x009060B0/0x14065EEB0
static int32_t Script_BNRemoveFriend(lua_State* L) {
    Login* login = ClientServices::LoginConnection();

    if (login && login->GetLoginServerType() == 1 && !login->IsTrialAccount() && BattlenetAPI::IsRIDEnabled()) {
        if (!lua_isnumber(L, 1)) {
            luaL_error(L, "Usage: BNRemoveFriend(ID)");
            return 0;
        }

        if (BattlenetUI::m_friendListLoaded) {
            BattlenetAPI::RemoveFriend(static_cast<uint32_t>(lua_tonumber(L, 1)));
        }
    }

    return 0;
}

// OFFSET: 0x00906160/0x14065EF50
static int32_t Script_BNSetFriendNote(lua_State* L) {
    Login* login = ClientServices::LoginConnection();

    if (login && login->GetLoginServerType() == 1 && !login->IsTrialAccount() && BattlenetAPI::IsRIDEnabled()) {
        if (!lua_isnumber(L, 1)) {
            luaL_error(L, "Usage: BNSetFriendNote(ID, noteText)");
            return 0;
        }

        if (BattlenetUI::m_friendListLoaded) {
            if (!lua_isstring(L, 2)) {
                luaL_error(L, "Usage: BNSetFriendNote(ID, noteText)");
                return 0;
            }

            uint32_t presenceId = static_cast<uint32_t>(lua_tonumber(L, 1));
            const char* note = lua_tolstring(L, 2, NULL);

            if (SStrLenUTF8(note) > 255) {
                luaL_error(L, "Friend note text too long, maximum length %d chars", 255);
                return 0;
            }

            BattlenetAPI::SetFriendNote(presenceId, note);
        }
    }

    return 0;
}

// OFFSET: 0x00906260/0x14065F070
static int32_t Script_BNGetNumFriendInvites(lua_State* L) {
    Login* login = ClientServices::LoginConnection();

    if (login && login->GetLoginServerType() == 1 && !login->IsTrialAccount() && BattlenetAPI::IsRIDEnabled()) {
        if (BattlenetUI::FeaturesEnabledAndConnected()) {
            uint32_t count = BattlenetUI::m_friendInvites.Count();
            lua_pushnumber(L, count);
            DebugLog("BNUI: Num friend invites: %d", count);
        } else {
            lua_pushnumber(L, 0.0);
            DebugLog("BNUI: Num friend invites: 0, NOT CONNNECTED");
        }

        return 1;
    }

    lua_pushnumber(L, 0.0);
    return 1;
}

// OFFSET: 0x00906330/0x14065F140
static int32_t Script_BNSendFriendInvite(lua_State* L) {
    Login* login = ClientServices::LoginConnection();

    if (login && login->GetLoginServerType() == 1 && !login->IsTrialAccount() && BattlenetAPI::IsRIDEnabled()) {
        if (!lua_isstring(L, 1)) {
            luaL_error(L, "Usage: BNSendFriendInvite(text, noteText)");
            return 0;
        }

        const char* text = lua_tolstring(L, 1, NULL);
        char note[1024];

        if (lua_isstring(L, 2)) {
            const char* noteText = lua_tolstring(L, 2, NULL);

            if (SStrLenUTF8(noteText) > 255) {
                luaL_error(L, "Invite note text too long, maximum length %d chars", 255);
                return 0;
            }

            if (noteText) {
                FilterChatLinks(noteText, note, 1021);
            } else {
                note[0] = '\0';
            }
        } else {
            note[0] = '\0';
        }

        BattlenetAPI::SendRIDFriendInviteByEmail(text, note, Battlenet::InviteSource::MISC, &BattlenetUI::s_friendInviteSentCB);
    }

    return 0;
}

// OFFSET: 0x00906450/0x14065F290
static int32_t Script_BNSendFriendInviteByID(lua_State* L) {
    Login* login = ClientServices::LoginConnection();

    if (login && login->GetLoginServerType() == 1 && !login->IsTrialAccount() && BattlenetAPI::IsRIDEnabled()) {
        if (!lua_isstring(L, 1)) {
            luaL_error(L, "Usage: BNSendFriendInviteByID(ID, noteText)");
            return 0;
        }

        uint32_t presenceId = static_cast<uint32_t>(lua_tonumber(L, 1));
        char note[1024];

        if (lua_isstring(L, 2)) {
            const char* noteText = lua_tolstring(L, 2, NULL);

            if (SStrLenUTF8(noteText) > 255) {
                luaL_error(L, "Invite note text too long, maximum length %d chars", 255);
                return 0;
            }

            if (noteText) {
                FilterChatLinks(noteText, note, 1021);
            } else {
                note[0] = '\0';
            }
        } else {
            note[0] = '\0';
        }

        BattlenetAPI::SendRIDFriendInviteByPresenceId(presenceId, note, Battlenet::InviteSource::MISC, &BattlenetUI::s_friendInviteSentCB);
    }

    return 0;
}

// OFFSET: 0x00906580/0x14065F3E0
static int32_t Script_BNAcceptFriendInvite(lua_State* L) {
    Login* login = ClientServices::LoginConnection();

    if (login && login->GetLoginServerType() == 1 && !login->IsTrialAccount() && BattlenetAPI::IsRIDEnabled()) {
        if (!lua_isnumber(L, 1)) {
            luaL_error(L, "Usage: BNAcceptFriendInvite(ID)");
            return 0;
        }

        BattlenetAPI::AcceptFriendInvite(static_cast<uint32_t>(lua_tonumber(L, 1)));
    }

    return 0;
}

// OFFSET: 0x00906620/0x14065F480
static int32_t Script_BNDeclineFriendInvite(lua_State* L) {
    Login* login = ClientServices::LoginConnection();

    if (login && login->GetLoginServerType() == 1 && !login->IsTrialAccount() && BattlenetAPI::IsRIDEnabled()) {
        if (!lua_isnumber(L, 1)) {
            luaL_error(L, "Usage: BNDeclineFriendInvite(ID)");
            return 0;
        }

        BattlenetAPI::DeclineFriendInvite(static_cast<uint32_t>(lua_tonumber(L, 1)));
    }

    return 0;
}

// OFFSET: 0x009066C0/0x14065F520
static int32_t Script_BNReportFriendInvite(lua_State* L) {
    Login* login = ClientServices::LoginConnection();

    if (login && login->GetLoginServerType() == 1 && !login->IsTrialAccount() && BattlenetAPI::IsRIDEnabled()) {
        if (!lua_isnumber(L, 1)) {
            luaL_error(L, "Usage: BNReportFriendInvite(ID)");
            return 0;
        }

        BattlenetAPI::ReportFriendInvite(static_cast<uint32_t>(lua_tonumber(L, 1)));
    }

    return 0;
}

// OFFSET: 0x00906760/0x14065F5C0
static int32_t Script_BNSetAFK(lua_State* L) {
    Login* login = ClientServices::LoginConnection();

    if (login && login->GetLoginServerType() == 1 && !login->IsTrialAccount() && BattlenetAPI::IsRIDEnabled()) {
        if (lua_type(L, 1) != LUA_TBOOLEAN) {
            luaL_error(L, "Usage: BNSetAFK(bool)");
            return 0;
        }

        bool afk = lua_toboolean(L, 1) != 0;
        Battlenet::Presence::FieldVal value;
        value.m_tag = 11;
        value.Value<bool>() = afk;
        BattlenetAPI::SetPresenceValue(BattlenetUI::s_presenceCallbacks[BN_PRESENCE_AFK].m_field, &value);
    }

    return 0;
}

// OFFSET: 0x00906820/0x14065F6A0
static int32_t Script_BNSetDND(lua_State* L) {
    Login* login = ClientServices::LoginConnection();

    if (login && login->GetLoginServerType() == 1 && !login->IsTrialAccount() && BattlenetAPI::IsRIDEnabled()) {
        if (lua_type(L, 1) != LUA_TBOOLEAN) {
            luaL_error(L, "Usage: BNSetDND(bool)");
            return 0;
        }

        bool dnd = lua_toboolean(L, 1) != 0;
        Battlenet::Presence::FieldVal value;
        value.m_tag = 11;
        value.Value<bool>() = dnd;
        BattlenetAPI::SetPresenceValue(BattlenetUI::s_presenceCallbacks[BN_PRESENCE_DND].m_field, &value);
    }

    return 0;
}

// OFFSET: 0x009068E0/0x14065F780
static int32_t Script_BNSetCustomMessage(lua_State* L) {
    Login* login = ClientServices::LoginConnection();

    if (login && login->GetLoginServerType() == 1 && !login->IsTrialAccount() && BattlenetAPI::IsRIDEnabled()) {
        if (!lua_isstring(L, 1)) {
            luaL_error(L, "Usage: BNSetCustomMessage(text)");
            return 0;
        }

        if (++BattlenetUI::s_broadcastCount >= 2) {
            float now = OsGetAsyncTimeSec();

            if (now - BattlenetUI::s_broadcastTime < 3.0) {
                if (BattlenetUI::s_broadcastCount >= 2) {
                    CGGameUI::DisplayError(ERR_BN_BROADCAST_THROTTLE);
                    return 0;
                }
            } else {
                BattlenetUI::s_broadcastTime = now;
                BattlenetUI::s_broadcastCount = 0;
            }
        }

        const char* text = lua_tolstring(L, 1, NULL);

        if (SStrLenUTF8(text) > 127) {
            luaL_error(L, "Message text too long, maximum length %d chars", 127);
            return 0;
        }

        char message[512];
        FilterChatLinks(text, message, 509);

        Battlenet::Presence::FieldVal value;
        *value.Set<13, BSN::String<127> >() = message;
        BattlenetAPI::SetPresenceValue(BattlenetUI::s_presenceCallbacks[BN_PRESENCE_BROADCAST].m_field, &value);
    }

    return 0;
}

// OFFSET: 0x00906A60/0x14065F950
static int32_t Script_BNGetCustomMessageTable(lua_State* L) {
    Login* login = ClientServices::LoginConnection();
    uint32_t i = 0;

    if (!login || login->GetLoginServerType() != 1 || login->IsTrialAccount() || !BattlenetAPI::IsRIDEnabled()) {
        return 0;
    }

    if (lua_type(L, 1) != LUA_TTABLE) {
        luaL_error(L, "Usage: BNGetCustomMessageTable(table)");
        return 0;
    }

    if (BattlenetUI::m_friendListLoaded) {
        uint32_t online = 0;
        uint32_t offline = 0;

        while (i < BattlenetUI::m_friends.Count()) {
            Battlenet::Presence::FieldVal broadcast = BattlenetAPI::GetPresenceValue(BattlenetUI::m_friends[i].m_presenceID, BattlenetUI::s_presenceCallbacks[BN_PRESENCE_BROADCAST].m_field);
            uint8_t hasMessage = 0;

            if (broadcast.m_tag == 13 && SStrLenUTF8(broadcast.Value<BSN::String<127> >().m_data)) {
                hasMessage = 1;

                Battlenet::Presence::FieldVal value = BattlenetAPI::GetPresenceValue(BattlenetUI::m_friends[i].m_presenceID, BattlenetUI::s_presenceCallbacks[BN_PRESENCE_ONLINE].m_field);

                if (value.m_tag == 1 && value.Value<bool>()) {
                    online++;
                } else {
                    offline++;
                }
            }

            lua_checkstack(L, 2);
            lua_pushnumber(L, ++i);
            lua_pushboolean(L, hasMessage);
            lua_settable(L, -3);
        }

        lua_pushnumber(L, online);
        lua_pushnumber(L, offline);
    } else {
        lua_pushnil(L);
        lua_pushnumber(L, 0.0);
        lua_pushnumber(L, 0.0);
    }

    return 3;
}

// OFFSET: 0x00906C00/0x14065FB40
static int32_t Script_BNSetFocus(lua_State* L) {
    Login* login = ClientServices::LoginConnection();

    if (login && login->GetLoginServerType() == 1 && !login->IsTrialAccount() && BattlenetAPI::IsRIDEnabled()) {
        Battlenet::Presence::FieldVal value;
        value.m_tag = 0;
        BattlenetAPI::SetPresenceValue(BattlenetUI::s_presenceCallbacks[BN_PRESENCE_FOCUS].m_field, &value);
    }

    return 0;
}

// OFFSET: 0x00906C80/0x14065FBC0
static int32_t Script_BNCreateConversation(lua_State* L) {
    Login* login = ClientServices::LoginConnection();

    if (!login || login->GetLoginServerType() != 1 || login->IsTrialAccount() || !BattlenetAPI::IsRIDEnabled()) {
        return 0;
    }

    Battlenet::Toon::FullName first;
    Battlenet::Toon::FullName second;

    if (!BattlenetUI::FeaturesEnabledAndConnected()) {
        return 0;
    }

    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: BNCreateConversation(id,id)");
        return 0;
    }

    {
        Battlenet::Presence::FieldVal toon = BattlenetAPI::GetPresenceValue(static_cast<uint32_t>(lua_tonumber(L, 1)), BattlenetUI::s_presenceCallbacks[BN_PRESENCE_TOON_NAME].m_field);

        if (toon.m_tag != 17) {
            return 0;
        }

        first = toon.Value<Battlenet::Toon::FullName>();
    }

    if (!lua_isnumber(L, 2)) {
        luaL_error(L, "Usage: BNCreateConversation(id,id)");
        return 0;
    }

    {
        Battlenet::Presence::FieldVal toon = BattlenetAPI::GetPresenceValue(static_cast<uint32_t>(lua_tonumber(L, 2)), BattlenetUI::s_presenceCallbacks[BN_PRESENCE_TOON_NAME].m_field);

        if (toon.m_tag != 17) {
            return 0;
        }

        second = toon.Value<Battlenet::Toon::FullName>();
    }

    BattlenetAPI::CreateChatConversation(&first, &second, BattlenetUI::s_createConversationCB);

    lua_pushboolean(L, 1);
    return 1;
}

// OFFSET: 0x00906E40/0x14065FD80
static int32_t Script_BNInviteToConversation(lua_State* L) {
    Login* login = ClientServices::LoginConnection();

    if (!login || login->GetLoginServerType() != 1 || login->IsTrialAccount() || !BattlenetAPI::IsRIDEnabled()) {
        return 0;
    }

    Battlenet::Toon::FullName toon;

    if (!BattlenetUI::FeaturesEnabledAndConnected()) {
        return 0;
    }

    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: BNInviteToConversation(channel,id)");
        return 0;
    }

    uint8_t channel = static_cast<uint8_t>(lua_tonumber(L, 1) - 1.0);

    if (channel >= 6) {
        luaL_error(L, "Invalid Channel");
        return 0;
    }

    if (!lua_isnumber(L, 2)) {
        luaL_error(L, "Usage: BNInviteToConversation(channel,id)");
        return 0;
    }

    Battlenet::Presence::FieldVal value = BattlenetAPI::GetPresenceValue(static_cast<uint32_t>(lua_tonumber(L, 2)), BattlenetUI::s_presenceCallbacks[BN_PRESENCE_TOON_NAME].m_field);

    if (value.m_tag != 17) {
        return 0;
    }

    toon = value.Value<Battlenet::Toon::FullName>();
    BattlenetAPI::SendChatConversationInvite(channel, &toon, BattlenetUI::s_inviteToConversationCB);

    return 0;
}

// OFFSET: 0x00906FB0/0x14065FEE0
static int32_t Script_BNLeaveConversation(lua_State* L) {
    Login* login = ClientServices::LoginConnection();

    if (!login || login->GetLoginServerType() != 1 || login->IsTrialAccount() || !BattlenetAPI::IsRIDEnabled()) {
        return 0;
    }

    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: BNLeaveConversation(channel)");
        return 0;
    }

    if (!BattlenetUI::FeaturesEnabledAndConnected()) {
        return 0;
    }

    uint8_t channel = static_cast<uint8_t>(lua_tonumber(L, 1) - 1.0);

    if (channel >= 6) {
        luaL_error(L, "Invalid Channel");
        return 0;
    }

    BattlenetAPI::LeaveChatConversation(channel);
    return 0;
}

// OFFSET: 0x00907090/0x14065FFA0
static int32_t Script_BNSendConversationMessage(lua_State* L) {
    Login* login = ClientServices::LoginConnection();

    if (login && login->GetLoginServerType() == 1 && !login->IsTrialAccount() && BattlenetAPI::IsRIDEnabled()) {
        if (!lua_isnumber(L, 1) || !lua_isstring(L, 2)) {
            luaL_error(L, "Usage: BNSendConversationMessage(channel,text)");
            return 0;
        }

        if (BattlenetUI::FeaturesEnabledAndConnected()) {
            uint8_t channel = static_cast<uint8_t>(lua_tonumber(L, 1) - 1.0);
            const char* text = lua_tolstring(L, 2, NULL);
            char message[1024];
            FilterChatLinks(text, message, 1021);

            if (channel >= 6) {
                luaL_error(L, "Invalid Channel");
                return 0;
            }

            if (ClntObjMgrGetActivePlayerObj()) {
                CGPlayer_C::ClearAFK(0);
            }

            BattlenetAPI::SendChatMessage(channel, message);
        }
    }

    return 0;
}

// OFFSET: 0x009071C0/0x140660100
static int32_t Script_BNGetNumConversationMembers(lua_State* L) {
    Login* login = ClientServices::LoginConnection();

    if (login && login->GetLoginServerType() == 1 && !login->IsTrialAccount() && BattlenetAPI::IsRIDEnabled()) {
        if (!lua_isnumber(L, 1)) {
            luaL_error(L, "Usage: BNGetNumConversationMembers(channel)");
            return 0;
        }

        if (BattlenetUI::FeaturesEnabledAndConnected()) {
            uint8_t channel = static_cast<uint8_t>(lua_tonumber(L, 1) - 1.0);

            if (channel >= 6) {
                luaL_error(L, "Invalid Channel");
                return 0;
            }

            lua_pushnumber(L, BattlenetAPI::GetNumChatMembers(channel));
        } else {
            lua_pushnumber(L, 0.0);
        }

        return 1;
    }

    lua_pushnumber(L, 0.0);
    return 1;
}

// OFFSET: 0x009072D0/0x140660210
static int32_t Script_BNGetConversationInfo(lua_State* L) {
    Login* login = ClientServices::LoginConnection();

    if (!login || login->GetLoginServerType() != 1 || login->IsTrialAccount() || !BattlenetAPI::IsRIDEnabled()) {
        return 0;
    }

    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: BNGetConversationInfo(channel)");
        return 0;
    }

    uint8_t channel = static_cast<uint8_t>(lua_tonumber(L, 1) - 1.0);

    if (BattlenetAPI::GetChatChannelType(channel) == 2) {
        lua_pushstring(L, "conversation");
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: 0x009073A0/0x1406602E0
static int32_t Script_BNGetNumBlocked(lua_State* L) {
    Login* login = ClientServices::LoginConnection();

    if (login && login->GetLoginServerType() == 1 && !login->IsTrialAccount() && BattlenetAPI::IsRIDEnabled()) {
        if (BattlenetUI::FeaturesEnabledAndConnected()) {
            Battlenet::Presence::BlockList blockList = BattlenetAPI::GetBlockList();
            uint32_t count = blockList.m_ridBlocks.m_count;
            lua_pushnumber(L, count);
            DebugLog("BNUI: Num Blocked is %d", count);
        } else {
            lua_pushnumber(L, 0.0);
            DebugLog("BNUI: Num Blocked is 0. NOT CONNECTED");
        }

        return 1;
    }

    lua_pushnumber(L, 0.0);
    return 1;
}

// OFFSET: 0x00907480/0x1406603A0
static int32_t Script_BNIsBlocked(lua_State* L) {
    Login* login = ClientServices::LoginConnection();

    if (!login || login->GetLoginServerType() != 1 || login->IsTrialAccount() || !BattlenetAPI::IsRIDEnabled()) {
        return 0;
    }

    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: BNIsBlocked(ID)");
        return 0;
    }

    uint32_t presenceId = static_cast<uint32_t>(lua_tonumber(L, 1));
    bool blocked = BattlenetAPI::IsRIDBlock(presenceId);
    lua_pushboolean(L, blocked);

    const char* text = "true";

    if (!blocked) {
        text = "false";
    }

    DebugLog("BNUI: Is Blocked for ID %u %s", presenceId, text);

    return 1;
}

// OFFSET: 0x00907550/0x140660480
static int32_t Script_BNSetBlocked(lua_State* L) {
    Login* login = ClientServices::LoginConnection();

    if (!login || login->GetLoginServerType() != 1 || login->IsTrialAccount() || !BattlenetAPI::IsRIDEnabled()) {
        return 0;
    }

    if (!lua_isnumber(L, 1) || lua_type(L, 2) != LUA_TBOOLEAN) {
        luaL_error(L, "Usage: BNSetBlocked(ID, true/false)");
        return 0;
    }

    uint32_t presenceId = static_cast<uint32_t>(lua_tonumber(L, 1));

    if (!lua_toboolean(L, 2)) {
        BattlenetAPI::RemoveRIDBlock(presenceId);
        DebugLog("BNUI: Removing block for ID %u", presenceId);
        return 0;
    }

    Battlenet::Presence::BlockList blockList = BattlenetAPI::GetBlockList();

    if (blockList.m_ridBlocks.m_count < 99) {
        BattlenetAPI::AddRIDBlock(presenceId);
        DebugLog("BNUI: Adding block for ID %u", presenceId);
    } else {
        BattlenetUI::SendScriptEvent(SCRIPT_EVENT_BN_BLOCK_FAILED_TOO_MANY, "%s", "RID");
        DebugLog("BNUI: Failed to add block for ID %u (too many)", presenceId);
    }

    return 0;
}

// OFFSET: 0x00907690/0x1406605B0
static int32_t Script_BNSetSelectedBlock(lua_State* L) {
    Login* login = ClientServices::LoginConnection();

    if (login && login->GetLoginServerType() == 1 && !login->IsTrialAccount() && BattlenetAPI::IsRIDEnabled()) {
        if (!lua_isnumber(L, 1)) {
            luaL_error(L, "Usage: BNSetSelectedBlock(index)");
            return 0;
        }

        uint32_t index = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1;
        Battlenet::Presence::BlockList blockList = BattlenetAPI::GetBlockList();

        if (index >= blockList.m_ridBlocks.m_count) {
            luaL_error(L, "Block index %d too large, only %d blocked.", index + 1, blockList.m_ridBlocks.m_count);
            return 0;
        }

        BattlenetUI::s_selectedBlock = blockList.m_ridBlocks.m_data[index];
    }

    return 0;
}

// OFFSET: 0x00907780/0x1406606B0
static int32_t Script_BNGetSelectedBlock(lua_State* L) {
    Login* login = ClientServices::LoginConnection();

    if (login && login->GetLoginServerType() == 1 && !login->IsTrialAccount() && BattlenetAPI::IsRIDEnabled()) {
        if (BattlenetUI::s_selectedBlock) {
            Battlenet::Presence::BlockList blockList = BattlenetAPI::GetBlockList();

            for (uint32_t i = 0; i < blockList.m_ridBlocks.m_count; i++) {
                if (blockList.m_ridBlocks.m_data[i] == BattlenetUI::s_selectedBlock) {
                    lua_pushnumber(L, i + 1);
                    return 1;
                }
            }
        }

        lua_pushnumber(L, 0.0);
        return 1;
    }

    lua_pushnumber(L, 0.0);
    return 1;
}

// OFFSET: 0x00907870/0x140660770
static int32_t Script_BNGetNumBlockedToons(lua_State* L) {
    Login* login = ClientServices::LoginConnection();

    if (login && login->GetLoginServerType() == 1 && !login->IsTrialAccount() && BattlenetAPI::IsRIDEnabled()) {
        if (BattlenetUI::FeaturesEnabledAndConnected()) {
            Battlenet::Presence::BlockList blockList = BattlenetAPI::GetBlockList();
            uint32_t count = blockList.m_cidBlocks.m_count;
            lua_pushnumber(L, count);
            DebugLog("BNUI: Num Blocked toons is %d", count);
        } else {
            lua_pushnumber(L, 0.0);
            DebugLog("BNUI: Num Blocked toons is 0. NOT CONNECTED");
        }

        return 1;
    }

    lua_pushnumber(L, 0.0);
    return 1;
}

// OFFSET: 0x00907950/0x140660840
static int32_t Script_BNGetBlockedToonInfo(lua_State* L) {
    Login* login = ClientServices::LoginConnection();

    if (!login || login->GetLoginServerType() != 1 || login->IsTrialAccount()) {
        return 0;
    }

    if (!BattlenetAPI::IsRIDEnabled()) {
        return 0;
    }

    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: BNGetBlockedToonInfo(index)");
        return 0;
    }

    Battlenet::Presence::BlockList blockList = BattlenetAPI::GetBlockList();
    uint32_t index = static_cast<uint32_t>(lua_tonumber(L, 1) - 1.0);

    if (index >= blockList.m_cidBlocks.m_count) {
        luaL_error(L, "Invalid Index");
        return 0;
    }

    uint32_t presenceId = blockList.m_cidBlocks.m_data[index];
    lua_pushnumber(L, presenceId);

    const char* name = BattlenetAPI::GetExactNameForPresenceId(presenceId, false);
    lua_pushstring(L, name);
    DebugLog("BNUI: Blocked Toon info. index %d, ID %u, name: %s", index + 1, presenceId, name);

    return 2;
}

// OFFSET: 0x00907A70/0x140660980
static int32_t Script_BNIsToonBlocked(lua_State* L) {
    Login* login = ClientServices::LoginConnection();

    if (!login || login->GetLoginServerType() != 1 || login->IsTrialAccount() || !BattlenetAPI::IsRIDEnabled()) {
        return 0;
    }

    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: BNIsToonBlocked(ID)");
        return 0;
    }

    uint32_t presenceId = static_cast<uint32_t>(lua_tonumber(L, 1));
    bool blocked = BattlenetAPI::IsCIDBlock(presenceId);
    lua_pushboolean(L, blocked);

    const char* text = "true";

    if (!blocked) {
        text = "false";
    }

    DebugLog("BNUI: Is Toon Blocked for ID %u %s", presenceId, text);

    return 1;
}

// OFFSET: 0x00907B40/0x140660A60
static int32_t Script_BNSetToonBlocked(lua_State* L) {
    Login* login = ClientServices::LoginConnection();

    if (!login || login->GetLoginServerType() != 1 || login->IsTrialAccount() || !BattlenetAPI::IsRIDEnabled()) {
        return 0;
    }

    if (!lua_isnumber(L, 1) || lua_type(L, 2) != LUA_TBOOLEAN) {
        luaL_error(L, "Usage: BNSetToonBlocked(ID, true/false)");
        return 0;
    }

    uint32_t presenceId = static_cast<uint32_t>(lua_tonumber(L, 1));

    if (!lua_toboolean(L, 2)) {
        BattlenetAPI::RemoveCIDBlock(presenceId);
        DebugLog("BNUI: Removing block for toon ID %u", presenceId);
        return 0;
    }

    BattlenetAPI::AddCIDBlock(presenceId);
    Battlenet::Presence::BlockList blockList = BattlenetAPI::GetBlockList();

    if (blockList.m_cidBlocks.m_count < 100) {
        BattlenetAPI::AddCIDBlock(presenceId);
        DebugLog("BNUI: Adding block for toon ID %u", presenceId);
    } else {
        BattlenetUI::SendScriptEvent(SCRIPT_EVENT_BN_BLOCK_FAILED_TOO_MANY, "%s", "CID");
        DebugLog("BNUI: Failed to add block for toon ID %u (too many)", presenceId);
    }

    return 0;
}

// OFFSET: 0x00907C80/0x140660B90
static int32_t Script_BNSetSelectedToonBlock(lua_State* L) {
    Login* login = ClientServices::LoginConnection();

    if (login && login->GetLoginServerType() == 1 && !login->IsTrialAccount() && BattlenetAPI::IsRIDEnabled()) {
        if (!lua_isnumber(L, 1)) {
            luaL_error(L, "Usage: BNSetSelectedToonBlock(index)");
            return 0;
        }

        uint32_t index = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1;
        Battlenet::Presence::BlockList blockList = BattlenetAPI::GetBlockList();

        if (index >= blockList.m_cidBlocks.m_count) {
            luaL_error(L, "Toon Block index %d too large, only %d blocked.", index + 1, blockList.m_cidBlocks.m_count);
            return 0;
        }

        BattlenetUI::s_selectedToonBlock = blockList.m_cidBlocks.m_data[index];
    }

    return 0;
}

// OFFSET: 0x00907D70/0x140660C90
static int32_t Script_BNGetSelectedToonBlock(lua_State* L) {
    Login* login = ClientServices::LoginConnection();

    if (login && login->GetLoginServerType() == 1 && !login->IsTrialAccount() && BattlenetAPI::IsRIDEnabled()) {
        if (BattlenetUI::s_selectedToonBlock) {
            Battlenet::Presence::BlockList blockList = BattlenetAPI::GetBlockList();

            for (uint32_t i = 0; i < blockList.m_cidBlocks.m_count; i++) {
                if (blockList.m_cidBlocks.m_data[i] == BattlenetUI::s_selectedToonBlock) {
                    lua_pushnumber(L, i + 1);
                    return 1;
                }
            }
        }

        lua_pushnumber(L, 0.0);
        return 1;
    }

    lua_pushnumber(L, 0.0);
    return 1;
}

// OFFSET: 0x00907E60/0x140660D60
static int32_t Script_BNReportPlayer(lua_State* L) {
    Login* login = ClientServices::LoginConnection();

    if (login && login->GetLoginServerType() == 1 && !login->IsTrialAccount() && BattlenetAPI::IsRIDEnabled()) {
        if (!lua_isnumber(L, 1) || !lua_isstring(L, 2)) {
            luaL_error(L, "Usage: BNReportPlayer(ID,typeText,note)");
            return 0;
        }

        const char* type = lua_tolstring(L, 2, NULL);
        const char* note = NULL;

        if (lua_isstring(L, 3)) {
            note = lua_tolstring(L, 3, NULL);

            if (SStrLenUTF8(note, 509) > 127) {
                luaL_error(L, "Report note is too long.");
                return 0;
            }
        }

        int32_t reason;

        if (!_stricmp(type, "SPAM")) {
            reason = 0;
        } else if (!_stricmp(type, "ABUSE")) {
            reason = 1;
        } else if (!_stricmp(type, "THREAT")) {
            reason = 2;
        } else {
            if (_stricmp(type, "NAME")) {
                luaL_error(L, "Unknown Reason");
                return 0;
            }

            reason = 3;
        }

        if (!BattlenetAPI::ReportPlayer(static_cast<uint32_t>(lua_tonumber(L, 1)), reason, note)) {
            CGChat::AddChatMessage(FrameScript_GetText("BNET_REPORT_SENT", -1, GENDER_NOT_APPLICABLE), static_cast<SLASH_COMMAND_ID>(0), NULL);
        }
    }

    return 0;
}

// OFFSET: 0x00908010
static int32_t Script_BNFeaturesEnabledAndConnected(lua_State* L) {
    Login* login = ClientServices::LoginConnection();
    bool enabled = login && login->GetLoginServerType() == 1 && !login->IsTrialAccount() && BattlenetAPI::IsRIDEnabled() && static_cast<BattlenetLogin*>(login)->IsConnected();
    lua_pushboolean(L, enabled);
    return 1;
}

// OFFSET: 0x00908070/0x140660F50
static int32_t Script_BNFeaturesEnabled(lua_State* L) {
    Login* login = ClientServices::LoginConnection();
    bool enabled = login && login->GetLoginServerType() == 1 && !login->IsTrialAccount() && BattlenetAPI::IsRIDEnabled();
    lua_pushboolean(L, enabled);
    return 1;
}

// OFFSET: 0x009080D0/0x140660FC0
static int32_t Script_BNGetNumFOF(lua_State* L) {
    Login* login = ClientServices::LoginConnection();

    if (!login || login->GetLoginServerType() != 1 || login->IsTrialAccount() || !BattlenetAPI::IsRIDEnabled()) {
        return 0;
    }

    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: BNGetNumFOF(ID,mutual,non)");
        return 0;
    }

    uint32_t mutual = 0;
    uint32_t nonMutual = 0;

    for (uint32_t i = 0; i < BattlenetUI::s_friendsOfFriend.Count(); i++) {
        if (!BattlenetAPI::IsSelf(BattlenetUI::s_friendsOfFriend[i])) {
            if (BattlenetAPI::IsRIDFriend(BattlenetUI::s_friendsOfFriend[i])) {
                mutual++;
            } else {
                nonMutual++;
            }
        }
    }

    lua_pushnumber(L, mutual);
    lua_pushnumber(L, nonMutual);
    DebugLog("BNUI: BNGetNumFOF is %d mutual, %d non-mutual", mutual, nonMutual);

    return 2;
}

// OFFSET: 0x009081D0/0x1406610F0
static int32_t Script_BNSetSelectedFriend(lua_State* L) {
    Login* login = ClientServices::LoginConnection();

    if (login && login->GetLoginServerType() == 1 && !login->IsTrialAccount() && BattlenetAPI::IsRIDEnabled()) {
        if (!lua_isnumber(L, 1)) {
            luaL_error(L, "Usage: BNSetSelectedFriend(index)");
            return 0;
        }

        if (BattlenetUI::m_friendListLoaded) {
            uint32_t index = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1;

            if (index >= BattlenetUI::m_friends.Count()) {
                luaL_error(L, "Friend index %d too large, only %d friends.", index + 1, BattlenetUI::m_friends.Count());
                return 0;
            }

            BattlenetUI::s_selectedFriend = BattlenetUI::m_friends[index].m_presenceID;
        }
    }

    return 0;
}

// OFFSET: 0x009082C0/0x1406611E0
static int32_t Script_BNGetSelectedFriend(lua_State* L) {
    Login* login = ClientServices::LoginConnection();

    if (login && login->GetLoginServerType() == 1 && !login->IsTrialAccount() && BattlenetAPI::IsRIDEnabled() && BattlenetUI::m_friendListLoaded && BattlenetUI::s_selectedFriend) {
        int32_t index = BattlenetUI::FindFriendIndex(BattlenetUI::s_selectedFriend);

        if (index < 0) {
            lua_pushnumber(L, 0.0);
        } else {
            lua_pushnumber(L, index + 1);
        }

        return 1;
    }

    lua_pushnumber(L, 0.0);
    return 1;
}

// OFFSET: 0x00908380/0x140661290
static int32_t Script_BNGetFriendIndex(lua_State* L) {
    Login* login = ClientServices::LoginConnection();

    if (login && login->GetLoginServerType() == 1 && !login->IsTrialAccount() && BattlenetAPI::IsRIDEnabled()) {
        if (!lua_isnumber(L, 1)) {
            luaL_error(L, "Usage: BNGetFriendIndex(presenceID)");
            return 0;
        }

        if (BattlenetUI::m_friendListLoaded) {
            int32_t index = BattlenetUI::FindFriendIndex(static_cast<uint32_t>(lua_tonumber(L, 1))) + 1;
            lua_pushnumber(L, index);
            return 1;
        }

        return 0;
    }

    lua_pushnumber(L, 0.0);
    return 1;
}

// OFFSET: 0x00908460/0x140661380
static int32_t Script_BNSetMatureLanguageFilter(lua_State* L) {
    Login* login = ClientServices::LoginConnection();

    if (login && login->GetLoginServerType() == 1 && !login->IsTrialAccount() && BattlenetAPI::IsRIDEnabled()) {
        if (lua_type(L, 1) != LUA_TBOOLEAN) {
            luaL_error(L, "Usage: BNSetMatureLanguageFilter(true/false)");
            return 0;
        }

        if (!BattlenetUI::FeaturesEnabledAndConnected()) {
            luaL_error(L, "not connected to Battle.net");
            return 0;
        }

        int32_t enabled = lua_toboolean(L, 1);
        GeneralFunction::Arg value;
        value = static_cast<int64_t>(enabled);
        BattlenetAPI::SetSetting("Chat.ProfanityFilterEnabled", value, true);

        const char* text = "on";

        if (!enabled) {
            text = "off";
        }

        DebugLog("BNUI: Set mature language filter %s", text);
    }

    return 0;
}

// OFFSET: 0x00908560/0x1406614A0
static int32_t Script_BNGetMatureLanguageFilter(lua_State* L) {
    Login* login = ClientServices::LoginConnection();

    if (!login || login->GetLoginServerType() != 1 || login->IsTrialAccount() || !BattlenetAPI::IsRIDEnabled()) {
        return 0;
    }

    if (!BattlenetUI::FeaturesEnabledAndConnected()) {
        luaL_error(L, "not connected to Battle.net");
        return 0;
    }

    GeneralFunction::Arg setting = BattlenetAPI::GetSetting("Chat.ProfanityFilterEnabled");
    bool enabled = setting.ToBool();

    const char* text = "on";

    if (!enabled) {
        text = "off";
    }

    DebugLog("BNUI: mature language filter is %s", text);
    lua_pushboolean(L, enabled);

    return 1;
}

// OFFSET: 0x00908640/0x140661600
static int32_t Script_BNIsSelf(lua_State* L) {
    Login* login = ClientServices::LoginConnection();

    if (!login || login->GetLoginServerType() != 1 || login->IsTrialAccount() || !BattlenetAPI::IsRIDEnabled()) {
        return 0;
    }

    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: BNIsSelf(presenceID)");
        return 0;
    }

    lua_pushboolean(L, BattlenetAPI::IsSelf(static_cast<uint32_t>(lua_tonumber(L, 1))));
    return 1;
}

// OFFSET: 0x009086F0/0x1406616A0
static int32_t Script_BNIsFriend(lua_State* L) {
    Login* login = ClientServices::LoginConnection();

    if (!login || login->GetLoginServerType() != 1 || login->IsTrialAccount() || !BattlenetAPI::IsRIDEnabled()) {
        return 0;
    }

    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: BNIsFriend(presenceID)");
        return 0;
    }

    lua_pushboolean(L, BattlenetAPI::HasRIDFriendship(static_cast<uint32_t>(lua_tonumber(L, 1))));
    return 1;
}

// OFFSET: 0x009087A0/0x140661740
static int32_t Script_BNGetMaxPlayersInConversation(lua_State* L) {
    Login* login = ClientServices::LoginConnection();

    if (!login || login->GetLoginServerType() != 1 || login->IsTrialAccount() || !BattlenetAPI::IsRIDEnabled()) {
        return 0;
    }

    lua_pushnumber(L, 12.0);
    return 1;
}

// OFFSET: 0x00909410/0x1406627A0
static int32_t Script_BNGetFriendToonInfo(lua_State* L) {
    Login* login = ClientServices::LoginConnection();

    if (!login || login->GetLoginServerType() != 1 || login->IsTrialAccount() || !BattlenetAPI::IsRIDEnabled()) {
        return 0;
    }

    if (!lua_isnumber(L, 1) || !lua_isnumber(L, 2)) {
        luaL_error(L, "Usage: BNGetToonInfo(friendIndex,toonIndex)");
        return 0;
    }

    if (!BattlenetUI::m_friendListLoaded) {
        return 0;
    }

    uint32_t friendIndex = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1;
    uint32_t toonIndex = static_cast<uint32_t>(lua_tonumber(L, 2)) - 1;

    if (friendIndex >= BattlenetUI::m_friends.Count()) {
        luaL_error(L, "Friend index %d too large, only %d friends.", friendIndex + 1, BattlenetUI::m_friends.Count());
        return 0;
    }

    uint32_t presenceId = BattlenetUI::m_friends[friendIndex].m_presenceID;
    uint32_t numToons = BattlenetAPI::GetNumToons(presenceId);

    if (toonIndex >= numToons) {
        luaL_error(L, "Toon index %d too large, only %d toons.", toonIndex + 1, numToons);
        return 0;
    }

    uint32_t i = 0;
    uint32_t online = 0;

    for (; i < numToons; i++) {
        Battlenet::Presence::FieldVal value = BattlenetAPI::GetPresenceValue(BattlenetAPI::GetToon(presenceId, i), BattlenetUI::s_presenceCallbacks[BN_PRESENCE_ONLINE].m_field);

        if (value.m_tag == 1 && value.Value<bool>()) {
            if (online == toonIndex) {
                break;
            }

            online++;
        }
    }

    if (i >= numToons) {
        luaL_error(L, "Couldn't find a toon at friend index %d, online toon index %d", friendIndex + 1, i + 1, toonIndex);
        return 0;
    }

    uint32_t toonId = BattlenetAPI::GetToon(presenceId, i);

    if (!toonId) {
        luaL_error(L, "Couldn't find a toon at friend index %d, toon index %d, online toon index %d", friendIndex + 1, i + 1, toonIndex);
        return 0;
    }

    return BattlenetUI::BNGetToonInfoHelper(L, toonId);
}

// OFFSET: 0x00909620/0x140662A10
static int32_t Script_BNGetToonInfo(lua_State* L) {
    Login* login = ClientServices::LoginConnection();

    if (!login || login->GetLoginServerType() != 1 || login->IsTrialAccount() || !BattlenetAPI::IsRIDEnabled()) {
        return 0;
    }

    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: BNGetToonInfo(ID)");
        return 0;
    }

    return BattlenetUI::BNGetToonInfoHelper(L, static_cast<uint32_t>(lua_tonumber(L, 1)));
}

// OFFSET: 0x0090A860/0x1406643A0
static int32_t Script_BNTokenCombineGivenAndSurname(lua_State* L) {
    Login* login = ClientServices::LoginConnection();
    const char* rest = NULL;

    if (!login || login->GetLoginServerType() != 1 || login->IsTrialAccount() || !BattlenetAPI::IsRIDEnabled()) {
        return 0;
    }

    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: BNTokenCombineGivenAndSurname(tokenstring)");
        return 0;
    }

    const char* text = lua_tolstring(L, 1, NULL);
    uint32_t presenceId = 0;

    if (text[0] != '|' || text[1] != 'K') {
        return 0;
    }

    char kind = text[2];

    if (kind != 'g' && kind != 's' && kind != 'f') {
        return 0;
    }

    text += 3;
    sscanf(text, "%d", &presenceId);

    if (!presenceId || !*text) {
        return 0;
    }

    do {
        int32_t advance;
        uint32_t wide;
        QUOTEDCODE code = GxuDetermineQuotedCode(text, advance, NULL, 0, wide, 0);
        text += advance;

        if (code == CODE_TOKENSTOP) {
            rest = text;
        }
    } while (*text);

    if (!rest) {
        return 0;
    }

    const char* name = BattlenetUI::GetKStringText('f', presenceId);
    BattlenetUI::TokenizeName(presenceId, name);
    lua_pushstring(L, name);
    lua_pushstring(L, rest);

    return 2;
}

// OFFSET: 0x0090C940/0x140667610
static int32_t Script_BNGetFriendInviteInfo(lua_State* L) {
    Login* login = ClientServices::LoginConnection();

    if (!login || login->GetLoginServerType() != 1 || login->IsTrialAccount() || !BattlenetAPI::IsRIDEnabled()) {
        return 0;
    }

    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: BNGetFriendInviteInfo(index)");
        return 0;
    }

    uint32_t index = static_cast<uint32_t>(lua_tonumber(L, 1));

    if (index - 1 >= BattlenetUI::m_friendInvites.Count()) {
        return 0;
    }

    uint32_t presenceId = BattlenetUI::m_friendInvites[index - 1];

    if (!presenceId) {
        luaL_error(L, "No Friend at index");
        return 0;
    }

    Battlenet::Presence::FieldVal name = BattlenetAPI::GetPresenceValue(presenceId, BattlenetUI::s_presenceCallbacks[BN_PRESENCE_ACCOUNT_NAME].m_field);
    lua_pushnumber(L, presenceId);
    DebugLog("BNUI: Invite Info ID: %u", presenceId);

    if (name.m_tag == 18) {
        const char* givenName = name.Value<Battlenet::Account::FullName>().m_givenName.m_data;
        const char* surname = name.Value<Battlenet::Account::FullName>().m_surname.m_data;
        BattlenetUI::TokenizeGivenAndSurname(presenceId, givenName, surname);
        lua_pushstring(L, givenName);
        lua_pushstring(L, surname);
        DebugLog("BNUI: Invite Info Account name: %s %s", givenName, surname);
    } else {
        lua_pushnil(L);
        lua_pushnil(L);
    }

    Battlenet::Friends::FriendInviteContainer invite = BattlenetAPI::GetFriendInvite(presenceId);

    if (invite.m_msg.m_exists) {
        lua_pushstring(L, invite.m_msg.m_value.m_data);
        DebugLog("BNUI: Invite Info message: %s", invite.m_msg.m_value.m_data);
    } else {
        lua_pushnil(L);
    }

    int32_t time = invite.m_createdTime;
    lua_pushnumber(L, time);
    DebugLog("BNUI: Invite Info time: %d", time);

    return 5;
}

// OFFSET: 0x0090CB10/0x140667820
static int32_t Script_BNSendWhisper(lua_State* L) {
    Login* login = ClientServices::LoginConnection();

    if (!login || login->GetLoginServerType() != 1 || login->IsTrialAccount() || !BattlenetAPI::IsRIDEnabled()) {
        return 0;
    }

    if (!lua_isnumber(L, 1) || !lua_isstring(L, 2)) {
        luaL_error(L, "Usage: BNSendWhisper(id,text)");
        return 0;
    }

    if (!BattlenetUI::FeaturesEnabledAndConnected()) {
        return 0;
    }

    uint32_t presenceId = static_cast<uint32_t>(lua_tonumber(L, 1));
    const char* text = lua_tolstring(L, 2, NULL);
    char message[1024];
    FilterChatLinks(text, message, 1021);

    Battlenet::Presence::FieldVal toon = BattlenetAPI::GetPresenceValue(presenceId, BattlenetUI::s_presenceCallbacks[BN_PRESENCE_TOON_NAME].m_field);

    if (toon.m_tag != 17) {
        BattlenetUI::DisplayError(318, presenceId);
        return 0;
    }

    if (ClntObjMgrGetActivePlayerObj()) {
        CGPlayer_C::ClearAFK(0);
    }

    BattlenetAPI::SendChatWhisper(presenceId, message);
    return 0;
}

// OFFSET: 0x0090CC50/0x140667990
static int32_t Script_BNGetConversationMemberInfo(lua_State* L) {
    Login* login = ClientServices::LoginConnection();

    if (!login || login->GetLoginServerType() != 1 || login->IsTrialAccount() || !BattlenetAPI::IsRIDEnabled()) {
        return 0;
    }

    if (!lua_isnumber(L, 1) || !lua_isnumber(L, 2)) {
        luaL_error(L, "Usage: BNGetConversationMemberInfo(channel,index)");
        return 0;
    }

    if (!BattlenetUI::FeaturesEnabledAndConnected()) {
        return 0;
    }

    uint8_t channel = static_cast<uint8_t>(lua_tonumber(L, 1) - 1.0);
    uint32_t index = static_cast<uint32_t>(lua_tonumber(L, 2)) - 1;

    if (channel >= 6) {
        luaL_error(L, "Invalid Channel");
        return 0;
    }

    BattlenetUI::MemberListCB members;
    BattlenetAPI::GetChatMemberList(channel, &members);

    if (index >= members.m_members->Count()) {
        luaL_error(L, "Invalid Index");
        return 0;
    }

    lua_pushnumber(L, BattlenetAPI::GetAccountPresenceId((*members.m_members)[index]));
    lua_pushnumber(L, (*members.m_members)[index]);

    const char* name = BattlenetUI::GetTokenizedName((*members.m_members)[index]);
    lua_pushstring(L, name);
    DebugLog("BNUI: ConversationMemberInfo name: %s", name);

    return 3;
}

// OFFSET: 0x0090CE10/0x140667B50
static int32_t Script_BNListConversation(lua_State* L) {
    Login* login = ClientServices::LoginConnection();

    if (login && login->GetLoginServerType() == 1 && !login->IsTrialAccount() && BattlenetAPI::IsRIDEnabled()) {
        if (!lua_isnumber(L, 1)) {
            luaL_error(L, "Usage: BNListConversation(channel)");
            return 0;
        }

        uint8_t channel = static_cast<uint8_t>(lua_tonumber(L, 1) - 1.0);

        if (BattlenetAPI::GetChatChannelType(channel) == 2) {
            BattlenetUI::MemberListCB members;
            BattlenetAPI::GetChatMemberList(channel, &members);

            ChatConversation conversation;
            conversation.m_index = channel;
            conversation.m_presenceID = BattlenetAPI::GetPresenceIDForCurrentAccount();
            conversation.m_inactive = 0;

            const char* delimiter = FrameScript_GetText("PLAYER_LIST_DELIMITER", -1, GENDER_NOT_APPLICABLE);
            uint32_t delimiterLength = SStrLen(delimiter);
            char text[256];
            bool started = false;

            for (uint32_t i = 0; i < members.m_members->Count(); i++) {
                if (started) {
                    const char* name = BattlenetUI::GetTokenizedName((*members.m_members)[i]);

                    if (delimiterLength + SStrLen(text) + SStrLen(name) < sizeof(text)) {
                        SStrPrintf(text, sizeof(text), "%s%s%s", text, delimiter, name);
                    } else {
                        ChatMessageParams params;
                        params.m_conversation = &conversation;
                        CGChat::AddChatMessage(text, CHAT_MSG_BN_CONVERSATION_LIST, &params);
                        text[0] = '\0';
                        SStrCopyUTF8(text, name, sizeof(text), sizeof(text));
                    }
                } else {
                    SStrCopyUTF8(text, BattlenetUI::GetTokenizedName((*members.m_members)[i]), sizeof(text), sizeof(text));
                    started = true;
                }
            }

            if (started) {
                ChatMessageParams params;
                params.m_conversation = &conversation;
                CGChat::AddChatMessage(text, CHAT_MSG_BN_CONVERSATION_LIST, &params);
            }
        }
    }

    return 0;
}

// OFFSET: 0x0090D0B0/0x140667F80
static int32_t Script_BNGetBlockedInfo(lua_State* L) {
    Login* login = ClientServices::LoginConnection();

    if (!login || login->GetLoginServerType() != 1 || login->IsTrialAccount()) {
        return 0;
    }

    if (!BattlenetAPI::IsRIDEnabled()) {
        return 0;
    }

    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: BNGetBlockedInfo(index)");
        return 0;
    }

    Battlenet::Presence::BlockList blockList = BattlenetAPI::GetBlockList();
    uint32_t index = static_cast<uint32_t>(lua_tonumber(L, 1) - 1.0);

    if (index >= blockList.m_ridBlocks.m_count) {
        luaL_error(L, "Invalid Index");
        return 0;
    }

    uint32_t presenceId = blockList.m_ridBlocks.m_data[index];
    lua_pushnumber(L, presenceId);

    const char* name = BattlenetUI::GetTokenizedName(presenceId);
    lua_pushstring(L, name);
    DebugLog("BNUI: Blocked info. index %d, ID %u, name: %s", index + 1, presenceId, name);

    return 2;
}

// OFFSET: 0x0090D1D0/0x1406680B0
static int32_t Script_BNRequestFOFInfo(lua_State* L) {
    Login* login = ClientServices::LoginConnection();

    if (!login || login->GetLoginServerType() != 1 || login->IsTrialAccount() || !BattlenetAPI::IsRIDEnabled()) {
        return 0;
    }

    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: BNRequestFOF(ID)");
        return 0;
    }

    uint32_t presenceId = static_cast<uint32_t>(lua_tonumber(L, 1));
    BattlenetUI::s_friendsOfFriend.Clear();

    uint16_t error = BattlenetAPI::GetFriendsOfFriend(presenceId, BattlenetUI::s_friendsOfFriendCB);
    lua_pushboolean(L, error == 0);
    BattlenetUI::DisplayError(error, presenceId);

    const char* text = "succeeded";

    if (error) {
        text = "failed";
    }

    DebugLog("BNUI: Sending Request for FOF for ID %d %s", presenceId, text);

    return 1;
}

// OFFSET: 0x0090D2F0/0x1406681F0
static int32_t Script_BNGetFOFInfo(lua_State* L) {
    Login* login = ClientServices::LoginConnection();

    if (!login || login->GetLoginServerType() != 1 || login->IsTrialAccount() || !BattlenetAPI::IsRIDEnabled()) {
        return 0;
    }

    if (!lua_isnumber(L, 1) || !lua_isnumber(L, 4)) {
        luaL_error(L, "Usage: BNGetFOFInfo(ID, mutual, non, index)");
        return 0;
    }

    uint32_t presenceId = static_cast<uint32_t>(lua_tonumber(L, 1));
    bool mutual = lua_toboolean(L, 2) != 0;
    bool nonMutual = lua_toboolean(L, 3) != 0;
    uint32_t index = static_cast<uint32_t>(lua_tonumber(L, 4)) - 1;

    if (!mutual && !nonMutual) {
        luaL_error(L, "Must select mutual and/or non.");
        return 0;
    }

    uint32_t current = 0;
    uint8_t isMutual = 0;
    uint32_t i;

    for (i = 0; i < BattlenetUI::s_friendsOfFriend.Count(); i++) {
        if (BattlenetAPI::IsSelf(BattlenetUI::s_friendsOfFriend[i])) {
            continue;
        }

        if (BattlenetAPI::IsRIDFriend(BattlenetUI::s_friendsOfFriend[i])) {
            if (mutual) {
                if (current == index) {
                    isMutual = 1;
                    break;
                }

                current++;
            }
        } else if (nonMutual) {
            if (current == index) {
                break;
            }

            current++;
        }
    }

    if (i >= BattlenetUI::s_friendsOfFriend.Count()) {
        luaL_error(L, "No friend at that index.");
        return 0;
    }

    uint32_t friendId = BattlenetUI::s_friendsOfFriend[i];
    lua_pushnumber(L, friendId);

    const char* givenName = "none";
    const char* surname = "none";
    Battlenet::Presence::FieldVal name = BattlenetAPI::GetPresenceValue(friendId, BattlenetUI::s_presenceCallbacks[BN_PRESENCE_ACCOUNT_NAME].m_field);

    if (name.m_tag == 18) {
        givenName = name.Value<Battlenet::Account::FullName>().m_givenName.m_data;
        surname = name.Value<Battlenet::Account::FullName>().m_surname.m_data;
        BattlenetUI::TokenizeGivenAndSurname(friendId, givenName, surname);
        lua_pushstring(L, givenName);
        lua_pushstring(L, surname);
    } else {
        lua_pushnil(L);
        lua_pushnil(L);
    }

    lua_pushboolean(L, isMutual);
    DebugLog("BNUI: BNGetFOFInfo for ID %u index %d is ID %u, Acct name %s %s % mutual friend.", presenceId, index + 1, friendId, givenName, surname);

    return 4;
}

// OFFSET: 0x0090DFF0/0x1406692D0
static int32_t Script_BNGetFriendInfo(lua_State* L) {
    Login* login = ClientServices::LoginConnection();

    if (!login || login->GetLoginServerType() != 1 || login->IsTrialAccount() || !BattlenetAPI::IsRIDEnabled()) {
        return 0;
    }

    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: BNGetFriendInfo(index)");
        return 0;
    }

    if (!BattlenetUI::m_friendListLoaded) {
        return 0;
    }

    uint32_t index = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1;

    if (index >= BattlenetUI::m_friends.Count()) {
        luaL_error(L, "Friend index %d too large, only %d friends.", index + 1, BattlenetUI::m_friends.Count());
        return 0;
    }

    return BattlenetUI::BNGetFriendInfoHelper(L, index);
}

// OFFSET: 0x0090E0C0/0x1406693A0
static int32_t Script_BNGetFriendInfoByID(lua_State* L) {
    Login* login = ClientServices::LoginConnection();

    if (!login || login->GetLoginServerType() != 1 || login->IsTrialAccount() || !BattlenetAPI::IsRIDEnabled()) {
        return 0;
    }

    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: BNGetFriendInfoByID(ID)");
        return 0;
    }

    int32_t index;

    if (BattlenetUI::m_friendListLoaded && (index = BattlenetUI::FindFriendIndex(static_cast<uint32_t>(lua_tonumber(L, 1)))) >= 0) {
        return BattlenetUI::BNGetFriendInfoHelper(L, index);
    }

    return 0;
}

// OFFSET: [0x009546F0/0x14065A910]
static int32_t Script_BNGetMaxNumConversations(lua_State* L) {
    lua_pushnumber(L, 6.0);
    return 1;
}

// STATIC: 0x00D10C30
static FrameScript_Method s_ScriptFunctions[] = {
    { "BNGetInfo", &Script_BNGetInfo },
    { "BNGetNumFriends", &Script_BNGetNumFriends },
    { "BNGetFriendInfo", &Script_BNGetFriendInfo },
    { "BNGetFriendInfoByID", &Script_BNGetFriendInfoByID },
    { "BNSendSoR", &Script_BNSendSoR },
    { "BNGetNumFriendToons", &Script_BNGetNumFriendToons },
    { "BNGetFriendToonInfo", &Script_BNGetFriendToonInfo },
    { "BNGetToonInfo", &Script_BNGetToonInfo },
    { "BNRemoveFriend", &Script_BNRemoveFriend },
    { "BNSetFriendNote", &Script_BNSetFriendNote },
    { "BNSetSelectedFriend", &Script_BNSetSelectedFriend },
    { "BNGetSelectedFriend", &Script_BNGetSelectedFriend },
    { "BNGetFriendIndex", &Script_BNGetFriendIndex },
    { "BNGetNumFriendInvites", &Script_BNGetNumFriendInvites },
    { "BNGetFriendInviteInfo", &Script_BNGetFriendInviteInfo },
    { "BNSendFriendInvite", &Script_BNSendFriendInvite },
    { "BNSendFriendInviteByID", &Script_BNSendFriendInviteByID },
    { "BNAcceptFriendInvite", &Script_BNAcceptFriendInvite },
    { "BNDeclineFriendInvite", &Script_BNDeclineFriendInvite },
    { "BNReportFriendInvite", &Script_BNReportFriendInvite },
    { "BNSetAFK", &Script_BNSetAFK },
    { "BNSetDND", &Script_BNSetDND },
    { "BNSetCustomMessage", &Script_BNSetCustomMessage },
    { "BNGetCustomMessageTable", &Script_BNGetCustomMessageTable },
    { "BNSetFocus", &Script_BNSetFocus },
    { "BNSendWhisper", &Script_BNSendWhisper },
    { "BNCreateConversation", &Script_BNCreateConversation },
    { "BNInviteToConversation", &Script_BNInviteToConversation },
    { "BNLeaveConversation", &Script_BNLeaveConversation },
    { "BNSendConversationMessage", &Script_BNSendConversationMessage },
    { "BNGetNumConversationMembers", &Script_BNGetNumConversationMembers },
    { "BNGetConversationMemberInfo", &Script_BNGetConversationMemberInfo },
    { "BNGetConversationInfo", &Script_BNGetConversationInfo },
    { "BNListConversation", &Script_BNListConversation },
    { "BNGetMaxNumConversations", &Script_BNGetMaxNumConversations },
    { "BNGetNumBlocked", &Script_BNGetNumBlocked },
    { "BNGetBlockedInfo", &Script_BNGetBlockedInfo },
    { "BNIsBlocked", &Script_BNIsBlocked },
    { "BNSetBlocked", &Script_BNSetBlocked },
    { "BNSetSelectedBlock", &Script_BNSetSelectedBlock },
    { "BNGetSelectedBlock", &Script_BNGetSelectedBlock },
    { "BNGetNumBlockedToons", &Script_BNGetNumBlockedToons },
    { "BNGetBlockedToonInfo", &Script_BNGetBlockedToonInfo },
    { "BNIsToonBlocked", &Script_BNIsToonBlocked },
    { "BNSetToonBlocked", &Script_BNSetToonBlocked },
    { "BNSetSelectedToonBlock", &Script_BNSetSelectedToonBlock },
    { "BNGetSelectedToonBlock", &Script_BNGetSelectedToonBlock },
    { "BNReportPlayer", &Script_BNReportPlayer },
    { "BNConnected", &Script_BNConnected },
    { "BNFeaturesEnabledAndConnected", &Script_BNFeaturesEnabledAndConnected },
    { "IsBNLogin", &Script_IsBNLogin },
    { "BNFeaturesEnabled", &Script_BNFeaturesEnabled },
    { "BNRequestFOFInfo", &Script_BNRequestFOFInfo },
    { "BNGetNumFOF", &Script_BNGetNumFOF },
    { "BNGetFOFInfo", &Script_BNGetFOFInfo },
    { "BNSetMatureLanguageFilter", &Script_BNSetMatureLanguageFilter },
    { "BNGetMatureLanguageFilter", &Script_BNGetMatureLanguageFilter },
    { "BNIsSelf", &Script_BNIsSelf },
    { "BNIsFriend", &Script_BNIsFriend },
    { "BNGetMaxPlayersInConversation", &Script_BNGetMaxPlayersInConversation },
    { "BNTokenCombineGivenAndSurname", &Script_BNTokenCombineGivenAndSurname },
    { "BNInviteFriend", &Script_BNInviteFriend },
};

// OFFSET: 0x00902470
void BattlenetUI_RegisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_RegisterFunction(s_ScriptFunctions[i].m_name, s_ScriptFunctions[i].m_method);
    }
}

// OFFSET: 0x009024A0
void BattlenetUI_UnregisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_UnregisterFunction(s_ScriptFunctions[i].m_name);
    }
}

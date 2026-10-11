#include "BattlenetUI.h"
#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "Engine/Source/ConsoleBase/ConsoleVar.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/Gxu/GxuFont.h"
#include "Engine/Source/Storm/H/storm.h"
#include "WoW/Source/Battlenet/Tumor/TumorManager.h"
#include "WoW/Source/Ui/CGAutoCompleteName.h"
#include "WoW/Source/Ui/ChatFrame.h"
#include "WoW/Source/Ui/GameUI.h"
#include "WoW/Source/WowServices/BattlenetLogin.h"
#include "WoW/Source/WowServices/Login.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"

// STATIC: 0x00D109E0
BattlenetUI::EventCallback BattlenetUI::s_eventCallbacks[26] = {
    { "BattlenetEvent_NewPresence", BattlenetUI::NewPresenceCB, 0 },
    { "BattlenetEvent_ToonOnline", BattlenetUI::SelfToonOnlineCB, 0 },
    { "BattlenetEvent_ToonOffine", BattlenetUI::SelfToonOfflineCB, 0 },
    { "BattlenetEvent_FriendListInitialized", BattlenetUI::FriendListInitializedCB, 0 },
    { "BattlenetEvent_FriendAdded", BattlenetUI::FriendAddedCB, 0 },
    { "BattlenetEvent_FriendRemoved", BattlenetUI::FriendRemovedCB, 0 },
    { "BattlenetEvent_InviteListInitialized", BattlenetUI::FriendInviteListInitializedCB, 0 },
    { "BattlenetEvent_InviteAdded", BattlenetUI::FriendInviteAddedCB, 0 },
    { "BattlenetEvent_InviteRemoved", BattlenetUI::FriendInviteRemovedCB, 0 },
    { "BattlenetEvent_ChatWhisperSent", BattlenetUI::ChatWhisperSentCB, 0 },
    { "BattlenetEvent_ChatWhisperReceived", BattlenetUI::ChatWhisperReceivedCB, 0 },
    { "BattlenetEvent_ChatWhisperUndeliverable", BattlenetUI::ChatWhisperUndeliverableCB, 0 },
    { "BattlenetEvent_ChannelJoined", BattlenetUI::ChatChannelJoinedCB, 0 },
    { "BattlenetEvent_ChannelLeft", BattlenetUI::ChatChannelLeftCB, 0 },
    { "BattlenetEvent_ChannelClosed", BattlenetUI::ChatChannelClosedCB, 0 },
    { "BattlenetEvent_ChatMessage", BattlenetUI::ChatChannelMessageReceivedCB, 0 },
    { "BattlenetEvent_ChatMessageUndeliverable", BattlenetUI::ChatChannelMessageUndeliverableCB, 0 },
    { "BattlenetEvent_ChatMessageBlocked", BattlenetUI::ChatChannelMessageBlockedCB, 0 },
    { "BattlenetEvent_ChannelMemberJoined", BattlenetUI::ChatChannelMemberJoinedCB, 0 },
    { "BattlenetEvent_ChannelMemberLeft", BattlenetUI::ChatChannelMemberLeftCB, 0 },
    { "BattlenetEvent_ChannelMemberUpdated", BattlenetUI::ChatChannelMemberUpdatedCB, 0 },
    { "BattlenetEvent_BlockListInitialized", BattlenetUI::BlockListInitializedCB, 0 },
    { "BattlenetEvent_BlockAdded", BattlenetUI::BlockAddedCB, 0 },
    { "BattlenetEvent_BlockRemoved", BattlenetUI::BlockRemovedCB, 0 },
    { "BattlenetEvent_SystemMessage", BattlenetUI::SystemMessageCB, 0 },
    { "BattlenetEvent_SettingChanged", BattlenetUI::SettingChangedCB, 0 },
};

// STATIC: 0x00D10B18
BattlenetUI::PresenceCallback BattlenetUI::s_presenceCallbacks[BattlenetUI::NUM_PRESENCE_FIELDS] = {
    { 0x10002, BattlenetUI::OnlinePresenceChangedCB, 0 },
    { 0x10003, BattlenetUI::OnlineTimeChangedCB, 0 },
    { 0x10007, BattlenetUI::LastOnlineChangedCB, 0 },
    { 0x10010, BattlenetUI::PresenceUpdateCB, 0 },
    { 0x10011, BattlenetUI::PresenceUpdateCB, 0 },
    { 0x1000C, BattlenetUI::CustomMessageChangedCB, 0 },
    { 0x1000D, BattlenetUI::NoActionCB, 0 },
    { 0x1000E, BattlenetUI::ToonNameUpdateCB, 0 },
    { 0x10005, BattlenetUI::PresenceUpdateCB, 0 },
    { 0x10004, BattlenetUI::PresenceUpdateCB, 0 },
    { 0x10009, BattlenetUI::PresenceUpdateCB, 0 },
    { 0x10008, BattlenetUI::PresenceUpdateCB, 0 },
    { 0x30001, BattlenetUI::PresenceUpdateCB, 0 },
    { 0x30002, BattlenetUI::PresenceUpdateCB, 0 },
    { 0x30003, BattlenetUI::PresenceUpdateCB, 0 },
    { 0x30004, BattlenetUI::PresenceUpdateCB, 0 },
    { 0x30005, BattlenetUI::PresenceUpdateCB, 0 },
    { 0x30006, BattlenetUI::PresenceUpdateCB, 0 },
    { 0x1000A, BattlenetUI::PresenceUpdateCB, 0 },
    { 0x1000B, BattlenetUI::NoActionCB, 0 },
    { 0x1001D, BattlenetUI::PresenceUpdateCB, 0 },
    { 0x40001, BattlenetUI::FactionUpdateCB, 0 },
    { 0x40002, BattlenetUI::PresenceUpdateCB, 0 },
};

// STATIC: 0x00D10E2C
BattlenetUI::FriendInviteSentCB BattlenetUI::s_friendInviteSentCB;

// STATIC: 0x00D10EA8
int32_t BattlenetUI::s_broadcastCount = 2;

// STATIC: 0x00F3387C
uint32_t BattlenetUI::s_selfRealm;

// STATIC: 0x00F33880
bool BattlenetUI::s_initialized;

// STATIC: 0x00F33881
bool BattlenetUI::m_friendListLoaded;

// STATIC: 0x00F33884
GeneralFunction::Callback* BattlenetUI::s_createConversationCB;

// STATIC: 0x00F33888
GeneralFunction::Callback* BattlenetUI::s_inviteToConversationCB;

// STATIC: 0x00F3388C
GeneralFunction::Callback* BattlenetUI::s_friendsOfFriendCB;

// STATIC: 0x00F33890
uint32_t BattlenetUI::s_selectedFriend;

// STATIC: 0x00F33894
uint32_t BattlenetUI::s_selectedBlock;

// STATIC: 0x00F33898
uint32_t BattlenetUI::s_selectedToonBlock;

// STATIC: 0x00F3389C
uint32_t BattlenetUI::s_accountIdCacheLowest;

// STATIC: 0x00F338A0
uint32_t BattlenetUI::s_accountIdCacheCount;

// STATIC: 0x00F338A4
TSGrowableArray<uint32_t> BattlenetUI::m_friendInvites;

// STATIC: 0x00F338B4
TSGrowableArray<uint32_t> BattlenetUI::s_friendsOfFriend;

// STATIC: 0x00F338C4
TSGrowableArray<BattlenetUI::RIDFriendData> BattlenetUI::m_friends;

// STATIC: 0x00F338D4
TSHashTable<ACCOUNTIDCACHE, HASHKEY_DWORD> BattlenetUI::s_accountIdCache;

// STATIC: 0x00F33900
char BattlenetUI::s_programString[16];

// STATIC: 0x00F33910
float BattlenetUI::s_broadcastTime;

// STATIC: 0x00F33918
char BattlenetUI::s_displayName[72];

// STATIC: 0x00F33960
char BattlenetUI::s_tokenName[296];

// STATIC: 0x00F33A88
char BattlenetUI::s_tokenSurname[168];

// STATIC: 0x00F33B30
char BattlenetUI::s_tokenGivenName[168];

// OFFSET: 0x00902130
char* FilterChatLinks(const char* source, char* dest, uint32_t size) {
    char* limit = &dest[size - 1];
    char* out = dest;
    const char* end = &source[SStrLen(source)];

    if (source < end) {
        do {
            if (out >= limit) {
                break;
            }

            if (*source == '|') {
                switch (source[1]) {
                    case 'T':
                    case 'r':
                    case 't':
                        source += 2;
                        break;

                    case 'c':
                        source += 10;
                        break;

                    default:
                        break;
                }
            }

            if (source >= end) {
                break;
            }

            *out++ = *source++;
        } while (source < end);
    }

    *out = '\0';
    return dest;
}

// OFFSET: 0x009021E0
static int32_t CountFormatStrings(const char* format) {
    if (!format) {
        return 0;
    }

    int32_t length = SStrLen(format);
    bool inFormat = false;
    int32_t count = 0;

    for (int32_t i = 0; i < length; i++) {
        switch (format[i]) {
            case ' ':
            case '#':
            case '$':
            case '+':
            case '-':
            case '.':
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
                break;

            case '%':
                inFormat = !inFormat;
                break;

            case 'S':
            case 's':
                if (inFormat) {
                    count++;
                    inFormat = false;
                }

                break;

            default:
                if (inFormat) {
                    return -1;
                }

                break;
        }
    }

    return count;
}

// OFFSET: 0x009022B0
const char* BattlenetUI::GetProgramString(BSN::FourCC program) {
    program.ToString(BattlenetUI::s_programString, sizeof(BattlenetUI::s_programString));
    return BattlenetUI::s_programString;
}

// OFFSET: 0x00903C60
void BattlenetUI::SendScriptEvent(uint32_t event, const char* format, ...) {
    va_list args;
    va_start(args, format);

    if (CGGameUI::m_initialized) {
        vFrameScript_SignalEvent(event, format, args);
    }
}

// OFFSET: 0x00903C90
bool BattlenetUI::IsToonFriend(uint32_t accountId) {
    for (uint32_t i = 0; i < BattlenetUI::m_friends.Count(); i++) {
        if (accountId == BattlenetUI::m_friends[i].m_accountId) {
            return true;
        }
    }

    return false;
}

// OFFSET: 0x00903CD0
int32_t BattlenetUI::FindFriendIndex(uint32_t presenceId) {
    for (uint32_t i = 0; i < BattlenetUI::m_friends.Count(); i++) {
        if (BattlenetUI::m_friends[i].m_presenceID == presenceId) {
            return i;
        }
    }

    return -1;
}

// OFFSET: 0x00903D10
BattlenetUI::RIDFriendData* BattlenetUI::FindFriend(uint32_t presenceId) {
    for (uint32_t i = 0; i < BattlenetUI::m_friends.Count(); i++) {
        if (BattlenetUI::m_friends[i].m_presenceID == presenceId) {
            return &BattlenetUI::m_friends[i];
        }
    }

    return NULL;
}

// OFFSET: 0x00903D60
int32_t BattlenetUI::QSortDisambiguateFriendListCB(const void* a, const void* b) {
    const RIDFriendData* friend1 = static_cast<const RIDFriendData*>(a);
    const RIDFriendData* friend2 = static_cast<const RIDFriendData*>(b);

    int32_t result = SStrCmpUTF8I(friend1->m_givenName.m_data, friend2->m_givenName.m_data, 40);

    if (!result) {
        result = SStrCmpUTF8I(friend1->m_sortSurname.m_data, friend2->m_sortSurname.m_data, 40);

        if (!result) {
            return friend1->m_accountId - friend2->m_accountId;
        }
    }

    return result;
}

// OFFSET: 0x00903DB0
void BattlenetUI::SetBattlenetConnectionRecovered() {
    BattlenetUI::SendScriptEvent(SCRIPT_EVENT_BN_CONNECTED, NULL);
}

// OFFSET: 0x00905200
bool BattlenetUI::RemoveFromFriendList(uint32_t presenceId) {
    int32_t index = BattlenetUI::FindFriendIndex(presenceId);

    if (index >= 0) {
        if (index != BattlenetUI::m_friends.Count() - 1) {
            BattlenetUI::m_friends[index] = BattlenetUI::m_friends[BattlenetUI::m_friends.Count() - 1];
        }

        BattlenetUI::m_friends.m_count--;
        return true;
    }

    return false;
}

// OFFSET: 0x00905260
uint32_t BattlenetUI::GetSelfRealmID() {
    if (!BattlenetUI::s_selfRealm) {
        uint32_t toonId = BattlenetAPI::GetPresenceIDForCurrentToon();
        Battlenet::Presence::FieldVal toon = BattlenetAPI::GetPresenceValue(toonId, BattlenetUI::s_presenceCallbacks[BN_PRESENCE_TOON_NAME].m_field);

        if (toon.m_tag == 17) {
            BattlenetUI::s_selfRealm = toon.Value<Battlenet::Toon::FullName>().m_realm;
        }
    }

    return BattlenetUI::s_selfRealm;
}

// OFFSET: 0x009052B0
int32_t BattlenetUI::QSortFriendListCB(const void* a, const void* b) {
    const RIDFriendData* friend1 = static_cast<const RIDFriendData*>(a);
    const RIDFriendData* friend2 = static_cast<const RIDFriendData*>(b);

    Battlenet::Presence::FieldVal online1 = BattlenetAPI::GetPresenceValue(friend1->m_presenceID, BattlenetUI::s_presenceCallbacks[BN_PRESENCE_ONLINE].m_field);
    Battlenet::Presence::FieldVal online2 = BattlenetAPI::GetPresenceValue(friend2->m_presenceID, BattlenetUI::s_presenceCallbacks[BN_PRESENCE_ONLINE].m_field);

    bool isOnline1 = false;
    bool isOnline2 = false;

    if (online1.m_tag == 1) {
        isOnline1 = online1.Value<bool>() != 0;
    }

    if (online2.m_tag == 1) {
        isOnline2 = online2.Value<bool>() != 0;
    }

    if (isOnline1) {
        if (!isOnline2) {
            return -1;
        }
    } else if (isOnline2) {
        return 1;
    }

    return SStrCmpUTF8I(friend1->m_name.m_data, friend2->m_name.m_data, 72);
}

// OFFSET: 0x00905350
void BattlenetUI::DisambiguateFriendList() {
    if (BattlenetUI::m_friends.Count() > 1) {
        qsort(BattlenetUI::m_friends.Ptr(), BattlenetUI::m_friends.Count(), sizeof(RIDFriendData), BattlenetUI::QSortDisambiguateFriendListCB);
    }

    uint32_t duplicates = 0;

    for (uint32_t i = 0; i < BattlenetUI::m_friends.Count(); i++) {
        if (i != BattlenetUI::m_friends.Count() - 1
            && !SStrCmpUTF8I(BattlenetUI::m_friends[i].m_givenName.m_data, BattlenetUI::m_friends[i + 1].m_givenName.m_data, 40)
            && !SStrCmpUTF8I(BattlenetUI::m_friends[i].m_sortSurname.m_data, BattlenetUI::m_friends[i + 1].m_sortSurname.m_data, 40)) {
            if (++duplicates == 1) {
                BattlenetUI::m_friends[i].m_surname = BattlenetUI::m_friends[i].m_sortSurname;
                BattlenetUI::m_friends[i].m_surname += " 1";
            }

            char suffix[16];
            SStrPrintf(suffix, sizeof(suffix), " %d", duplicates + 1);
            BattlenetUI::m_friends[i + 1].m_surname = BattlenetUI::m_friends[i + 1].m_sortSurname;
            BattlenetUI::m_friends[i + 1].m_surname += suffix;
        } else {
            if (!duplicates) {
                BattlenetUI::m_friends[i].m_surname = BattlenetUI::m_friends[i].m_sortSurname;
            }

            duplicates = 0;
        }

        RIDFriendData& entry = BattlenetUI::m_friends[i];
        SStrPrintf(entry.m_name.m_data, sizeof(entry.m_name.m_data), FrameScript_GetText("BATTLENET_NAME_FORMAT", -1, GENDER_NOT_APPLICABLE), entry.m_givenName.m_data, entry.m_surname.m_data);
    }
}

// OFFSET: 0x00905530
int32_t BattlenetUI::QSortFriendInviteListByTimeCB(const void* a, const void* b) {
    Battlenet::Friends::FriendInviteContainer invite1 = BattlenetAPI::GetFriendInvite(*static_cast<const uint32_t*>(a));
    Battlenet::Friends::FriendInviteContainer invite2 = BattlenetAPI::GetFriendInvite(*static_cast<const uint32_t*>(b));

    if (invite1.m_createdTime > invite2.m_createdTime) {
        return -1;
    }

    return invite1.m_createdTime < invite2.m_createdTime;
}

// OFFSET: 0x00905590
bool BattlenetUI::FeaturesEnabledAndConnected() {
    Login* login = ClientServices::LoginConnection();

    if (login && login->GetLoginServerType() == 1 && !login->IsTrialAccount() && BattlenetAPI::IsRIDEnabled()) {
        return static_cast<BattlenetLogin*>(login)->IsConnected();
    }

    return false;
}

// OFFSET: 0x00905BA0
bool BattlenetUI::GetToonInviteInfo(uint32_t toonId, uint32_t& realmId, uint8_t* guid, char* name, uint32_t nameSize, char* realm, uint32_t realmSize) {
    realmId = 0;
    *reinterpret_cast<uint64_t*>(guid) = 0;

    {
        Battlenet::Presence::FieldVal handle = BattlenetAPI::GetPresenceValue(toonId, 0x10019);

        if (handle.m_tag == 24) {
            *reinterpret_cast<uint64_t*>(guid) = handle.Value<Battlenet::Toon::Handle>().m_id;
        } else {
            *reinterpret_cast<uint64_t*>(guid) = 0;
        }
    }

    {
        Battlenet::Presence::FieldVal realmValue = BattlenetAPI::GetPresenceValue(toonId, 0x40002);
        realmId = realmValue.m_tag != 5 ? 0 : realmValue.Value<uint32_t>();
    }

    name[0] = '\0';

    {
        Battlenet::Presence::FieldVal toon = BattlenetAPI::GetPresenceValue(toonId, BattlenetUI::s_presenceCallbacks[BN_PRESENCE_TOON_NAME].m_field);

        if (toon.m_tag == 17) {
            const char* toonName = BattlenetAPI::GetToonNameForPresenceId(toonId, false);

            if (toonName) {
                SStrCopy(name, toonName, nameSize);
            }
        }
    }

    realm[0] = '\0';

    {
        Battlenet::Presence::FieldVal realmName = BattlenetAPI::GetPresenceValue(toonId, BattlenetUI::s_presenceCallbacks[BN_PRESENCE_REALM_NAME].m_field);

        if (realmName.m_tag == 13) {
            SStrCopy(realm, realmName.Value<BSN::String<127> >().m_data, realmSize);
        }
    }

    return (*reinterpret_cast<uint64_t*>(guid) || name[0]) && (realmId || realm[0]);
}

// OFFSET: 0x00905D30
int32_t BattlenetUI::BNGetToonInfoHelper(lua_State* L, uint32_t presenceId) {
    lua_pushboolean(L, BattlenetAPI::GetFocusedToon(BattlenetAPI::GetAccountPresenceId(presenceId)) == presenceId);

    Battlenet::Presence::FieldVal toon = BattlenetAPI::GetPresenceValue(presenceId, BattlenetUI::s_presenceCallbacks[BN_PRESENCE_TOON_NAME].m_field);

    if (toon.m_tag == 17) {
        const char* toonName = BattlenetAPI::GetToonNameForPresenceId(presenceId, false);

        if (!toonName) {
            toonName = toon.Value<Battlenet::Toon::FullName>().m_name.m_data;
        }

        lua_pushstring(L, toonName);
        lua_pushstring(L, BattlenetUI::GetProgramString(toon.Value<Battlenet::Toon::FullName>().m_programId));
    } else {
        lua_pushstring(L, "");
        lua_pushstring(L, "");
    }

    Battlenet::Presence::FieldVal realm = BattlenetAPI::GetPresenceValue(presenceId, BattlenetUI::s_presenceCallbacks[BN_PRESENCE_REALM_NAME].m_field);
    if (realm.m_tag == 13) {
        lua_pushstring(L, realm.Value<BSN::String<127> >().m_data);
    } else {
        lua_pushstring(L, "");
    }

    {
        Battlenet::Presence::FieldVal value = BattlenetAPI::GetPresenceValue(presenceId, BattlenetUI::s_presenceCallbacks[PRESENCE_FIELD_REALM_ID].m_field);

        if (value.m_tag == 5) {
            lua_pushinteger(L, value.Value<uint32_t>());
        } else {
            lua_pushinteger(L, 0);
        }
    }

    {
        Battlenet::Presence::FieldVal value = BattlenetAPI::GetPresenceValue(presenceId, BattlenetUI::s_presenceCallbacks[PRESENCE_FIELD_FACTION].m_field);

        if (value.m_tag == 1) {
            lua_pushnumber(L, value.Value<uint8_t>());
        } else {
            lua_pushnumber(L, -1.0);
        }
    }

    Battlenet::Presence::FieldVal race = BattlenetAPI::GetPresenceValue(presenceId, BattlenetUI::s_presenceCallbacks[BN_PRESENCE_RACE].m_field);
    if (race.m_tag == 13) {
        lua_pushstring(L, race.Value<BSN::String<127> >().m_data);
    } else {
        lua_pushstring(L, "");
    }

    Battlenet::Presence::FieldVal classValue = BattlenetAPI::GetPresenceValue(presenceId, BattlenetUI::s_presenceCallbacks[BN_PRESENCE_CLASS].m_field);
    if (classValue.m_tag == 13) {
        lua_pushstring(L, classValue.Value<BSN::String<127> >().m_data);
    } else {
        lua_pushstring(L, "");
    }

    Battlenet::Presence::FieldVal guild = BattlenetAPI::GetPresenceValue(presenceId, BattlenetUI::s_presenceCallbacks[BN_PRESENCE_GUILD].m_field);
    if (guild.m_tag == 13) {
        lua_pushstring(L, guild.Value<BSN::String<127> >().m_data);
    } else {
        lua_pushstring(L, "");
    }

    Battlenet::Presence::FieldVal area = BattlenetAPI::GetPresenceValue(presenceId, BattlenetUI::s_presenceCallbacks[BN_PRESENCE_AREA].m_field);
    if (area.m_tag == 13) {
        lua_pushstring(L, area.Value<BSN::String<127> >().m_data);
    } else {
        lua_pushstring(L, "");
    }

    {
        Battlenet::Presence::FieldVal value = BattlenetAPI::GetPresenceValue(presenceId, BattlenetUI::s_presenceCallbacks[BN_PRESENCE_LEVEL].m_field);

        if (value.m_tag == 1) {
            char level[8];
            SStrPrintf(level, sizeof(level), "%u", value.Value<uint8_t>());
            lua_pushstring(L, level);
        } else {
            lua_pushstring(L, "");
        }
    }

    Battlenet::Presence::FieldVal gameText = BattlenetAPI::GetPresenceValue(presenceId, BattlenetUI::s_presenceCallbacks[BN_PRESENCE_GAME_TEXT].m_field);
    if (gameText.m_tag == 13) {
        lua_pushstring(L, gameText.Value<BSN::String<127> >().m_data);
    } else {
        lua_pushstring(L, "");
    }

    Battlenet::Presence::FieldVal broadcast = BattlenetAPI::GetPresenceValue(presenceId, BattlenetUI::s_presenceCallbacks[BN_PRESENCE_BROADCAST].m_field);
    if (broadcast.m_tag == 13) {
        lua_pushstring(L, broadcast.Value<BSN::String<127> >().m_data);
    } else {
        lua_pushstring(L, "");
    }

    {
        Battlenet::Presence::FieldVal value = BattlenetAPI::GetPresenceValue(presenceId, BattlenetUI::s_presenceCallbacks[BN_PRESENCE_BROADCAST_TIME].m_field);

        if (value.m_tag == 6) {
            lua_pushnumber(L, value.Value<int32_t>());
        } else {
            lua_pushnumber(L, 0.0);
        }
    }

    {
        Battlenet::Presence::FieldVal value = BattlenetAPI::GetPresenceValue(presenceId, BattlenetUI::s_presenceCallbacks[BN_PRESENCE_ONLINE].m_field);
        bool isOnline = false;

        if (value.m_tag == 1) {
            isOnline = value.Value<bool>() != 0;
        }

        lua_pushboolean(L, isOnline);
    }

    lua_pushnumber(L, presenceId);

    return 16;
}

// OFFSET: 0x00908950
BattlenetUI::MemberListCB::MemberListCB() {
    this->m_members = new (__FILE__, __LINE__) TSGrowableArray<uint32_t>();
}

// OFFSET: 0x009089A0
void BattlenetUI::SelfToonOnlineCB(void* param, int32_t argc, const GeneralFunction::Arg* args) {
    Login* login = ClientServices::LoginConnection();

    if (!login || login->GetLoginServerType() != 1 || login->IsTrialAccount() || !BattlenetAPI::IsRIDEnabled()) {
        return;
    }

    uint32_t presenceId = static_cast<uint32_t>(args[0].ToUInt64());
    BattlenetUI::SendScriptEvent(SCRIPT_EVENT_BN_SELF_ONLINE, "%u", presenceId);
    DebugLog("BNUI Self Toon Online, ID %u", presenceId);
}

// OFFSET: 0x00908A10
void BattlenetUI::SelfToonOfflineCB(void* param, int32_t argc, const GeneralFunction::Arg* args) {
    Login* login = ClientServices::LoginConnection();

    if (!login || login->GetLoginServerType() != 1 || login->IsTrialAccount() || !BattlenetAPI::IsRIDEnabled()) {
        return;
    }

    BattlenetUI::SendScriptEvent(SCRIPT_EVENT_BN_SELF_OFFLINE, NULL);
    DebugLog("BNUI: Self Toon Offline");
}

// OFFSET: 0x00908A70
bool BattlenetUI::AddToFriendList(uint32_t presenceId) {
    if (BattlenetUI::FindFriendIndex(presenceId) >= 0) {
        return true;
    }

    RIDFriendData* entry = BattlenetUI::m_friends.New();
    entry->m_presenceID = presenceId;

    Battlenet::Presence::FieldVal accountName = BattlenetAPI::GetPresenceValue(presenceId, BattlenetUI::s_presenceCallbacks[BN_PRESENCE_ACCOUNT_NAME].m_field);

    if (accountName.m_tag == 18) {
        SStrPrintf(entry->m_name.m_data, sizeof(entry->m_name.m_data), FrameScript_GetText("BATTLENET_NAME_FORMAT", -1, GENDER_NOT_APPLICABLE), entry->m_givenName.m_data, entry->m_surname.m_data);
        entry->m_givenName = accountName.Value<Battlenet::Account::FullName>().m_givenName.m_data;
        entry->m_surname = accountName.Value<Battlenet::Account::FullName>().m_surname.m_data;
        entry->m_sortSurname = accountName.Value<Battlenet::Account::FullName>().m_surname.m_data;
    } else {
        entry->m_name = "";
        entry->m_givenName = "";
        entry->m_surname = "";
        entry->m_sortSurname = "";
    }

    Battlenet::Presence::FieldVal accountId = BattlenetAPI::GetPresenceValue(entry->m_presenceID, BattlenetUI::s_presenceCallbacks[BN_PRESENCE_PROGRAM].m_field);
    entry->m_accountId = accountId.m_tag == 5 ? accountId.Value<uint32_t>() : 0;

    return true;
}

// OFFSET: 0x00908C10
const char* BattlenetUI::GetNameForPresenceIdHelper(uint32_t presenceId) {
    uint32_t accountId = presenceId;

    if (BattlenetAPI::HasRIDFriendship(presenceId)) {
        if (!BattlenetAPI::IsRIDFriend(presenceId)) {
            accountId = BattlenetAPI::GetAccountPresenceId(presenceId);
        }

        RIDFriendData* entry = BattlenetUI::FindFriend(accountId);

        if (entry) {
            return entry->m_name.m_data;
        }

        return BattlenetAPI::GetNameForPresenceId(accountId, false);
    }

    if (BattlenetAPI::IsSelf(presenceId)) {
        return BattlenetAPI::GetNameForPresenceId(presenceId, false);
    }

    uint32_t realm = BattlenetUI::GetSelfRealmID();

    if (!realm) {
        return BattlenetAPI::GetNameForPresenceId(presenceId, false);
    }

    Battlenet::Presence::FieldVal toon = BattlenetAPI::GetPresenceValue(presenceId, BattlenetUI::s_presenceCallbacks[BN_PRESENCE_TOON_NAME].m_field);

    if (toon.m_tag != 17 || (realm == toon.Value<Battlenet::Toon::FullName>().m_realm && toon.Value<Battlenet::Toon::FullName>().m_programId.m_value == Battlenet::Program::WOW.m_value)) {
        return BattlenetAPI::GetNameForPresenceId(presenceId, false);
    }

    SStrPrintf(BattlenetUI::s_displayName, sizeof(BattlenetUI::s_displayName), "%s*", BattlenetAPI::GetNameForPresenceId(presenceId, false));
    return BattlenetUI::s_displayName;
}

// OFFSET: 0x00908CF0
void BattlenetUI::FriendInviteSentCB::Call(int32_t argc, const GeneralFunction::Arg* args) const {
    Login* login = ClientServices::LoginConnection();

    if (!login || login->GetLoginServerType() != 1 || login->IsTrialAccount() || !BattlenetAPI::IsRIDEnabled()) {
        return;
    }

    UIErrors error = ERR_BN_FRIEND_REQUEST_SENT;
    uint16_t result = static_cast<uint16_t>(args[0].ToUInt64());

    if (result == 604) {
        error = ERR_BN_FRIEND_ALREADY;
    } else if (result == 617) {
        error = ERR_BN_FRIEND_SELF;
    } else if (result == 623) {
        error = ERR_BN_FRIEND_BLOCKED;
    }

    CGGameUI::DisplayError(error);

    const char* text = "success";

    if (result) {
        text = "failure";
    }

    DebugLog("BNUI: Friend Invite Send Result: %s", text);
}

// PROVEN: 0926afe1 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void BattlenetUI::FriendInviteSentCB::Destroy() const {
}

// OFFSET: 0x00401000/0x1407D7E80
void BattlenetUI::NoActionCB(void* param, int32_t argc, const GeneralFunction::Arg* args) {
}

// OFFSET: 0x00908DA0
void BattlenetUI::LastOnlineChangedCB(void* param, int32_t argc, const GeneralFunction::Arg* args) {
    Login* login = ClientServices::LoginConnection();

    if (!login || login->GetLoginServerType() != 1 || login->IsTrialAccount() || !BattlenetAPI::IsRIDEnabled()) {
        return;
    }

    BSN::BSNEncodedObject<Battlenet::Presence::FieldUpdate, Protocol> update(args[0].ToObject());

    if (update.m_data.m_value.m_tag == 6) {
        DebugLog("BNUI: Last Online for ID %u is now %d", update.m_data.m_id, update.m_data.m_value.Value<int32_t>());
    } else {
        DebugLog("BNUI: Last Online for ID %u changed", update.m_data.m_id);
    }
}

// OFFSET: 0x00908E40
void BattlenetUI::MemberListCB::Call(int32_t argc, const GeneralFunction::Arg* args) const {
    if (argc == 2) {
        BSN::BSNEncodedObject<Battlenet::Chat::MemberList, Protocol> members(args[0].ToObject());

        for (uint32_t i = 0; i < members.m_data.m_list.m_count; i++) {
            this->m_members->New(members.m_data.m_list.m_data[i]);
        }
    } else {
        DebugLog("MemberListCB called with %d parameters instead of 2", argc);
    }
}

// OFFSET: 0x00401000
void BattlenetUI::MemberListCB::Destroy() const {
}

// OFFSET: 0x00908F00
void BattlenetUI::ChatChannelMessageBlockedCB(void* param, int32_t argc, const GeneralFunction::Arg* args) {
    Login* login = ClientServices::LoginConnection();

    if (!login || login->GetLoginServerType() != 1 || login->IsTrialAccount() || !BattlenetAPI::IsRIDEnabled()) {
        return;
    }

    uint8_t channel = static_cast<uint8_t>(args[0].ToUInt64());
    uint32_t presenceId = static_cast<uint32_t>(args[1].ToUInt64());

    BattlenetUI::SendScriptEvent(SCRIPT_EVENT_BN_CHAT_CHANNEL_MESSAGE_BLOCKED, "%d%u", channel + 1, presenceId);
    DebugLog("BNUI: Chat Channel Message Blocked. channel %d, id %u", channel + 1, presenceId);
}

// OFFSET: 0x00908F80
void BattlenetUI::ChatChannelMemberUpdatedCB(void* param, int32_t argc, const GeneralFunction::Arg* args) {
    Login* login = ClientServices::LoginConnection();

    if (!login || login->GetLoginServerType() != 1 || login->IsTrialAccount() || !BattlenetAPI::IsRIDEnabled()) {
        return;
    }

    uint8_t channel = static_cast<uint8_t>(args[0].ToUInt64());
    uint32_t presenceId = static_cast<uint32_t>(args[1].ToUInt64());

    BattlenetUI::SendScriptEvent(SCRIPT_EVENT_BN_CHAT_CHANNEL_MEMBER_UPDATED, "%d%u", channel + 1, presenceId);
    DebugLog("BNUI: Chat Channel Member Updated. channel %d, id %u", channel + 1, presenceId);
}

// OFFSET: 0x00909000
void BattlenetUI::BlockListInitializedCB(void* param, int32_t argc, const GeneralFunction::Arg* args) {
    Login* login = ClientServices::LoginConnection();

    if (!login || login->GetLoginServerType() != 1 || login->IsTrialAccount() || !BattlenetAPI::IsRIDEnabled()) {
        return;
    }

    BattlenetUI::SendScriptEvent(SCRIPT_EVENT_BN_BLOCK_LIST_UPDATED, NULL);
    DebugLog("BNUI: Block List Initialized");
}

// OFFSET: 0x00909060
void BattlenetUI::BlockAddedCB(void* param, int32_t argc, const GeneralFunction::Arg* args) {
    Login* login = ClientServices::LoginConnection();

    if (!login || login->GetLoginServerType() != 1 || login->IsTrialAccount() || !BattlenetAPI::IsRIDEnabled()) {
        return;
    }

    uint32_t presenceId = static_cast<uint32_t>(args[0].ToUInt64());
    BattlenetUI::SendScriptEvent(SCRIPT_EVENT_BN_BLOCK_LIST_UPDATED, NULL);
    DebugLog("BNUI: Block Added for ID %u", presenceId);
}

// OFFSET: 0x009090C0
void BattlenetUI::BlockRemovedCB(void* param, int32_t argc, const GeneralFunction::Arg* args) {
    Login* login = ClientServices::LoginConnection();

    if (!login || login->GetLoginServerType() != 1 || login->IsTrialAccount() || !BattlenetAPI::IsRIDEnabled()) {
        return;
    }

    uint32_t presenceId = static_cast<uint32_t>(args[0].ToUInt64());
    BattlenetUI::SendScriptEvent(SCRIPT_EVENT_BN_BLOCK_LIST_UPDATED, NULL);
    DebugLog("BNUI: Block Removed for ID %u", presenceId);
}

// OFFSET: 0x00909120
void BattlenetUI::SystemMessageCB(void* param, int32_t argc, const GeneralFunction::Arg* args) {
    Login* login = ClientServices::LoginConnection();

    if (!login || login->GetLoginServerType() != 1 || login->IsTrialAccount() || !BattlenetAPI::IsRIDEnabled()) {
        return;
    }

    const char* type = "unknown";
    uint32_t value = 0;

    switch (static_cast<uint16_t>(args[0].ToUInt64())) {
        case 3800:
            type = "shutdown minutes";
            value = static_cast<uint32_t>(args[1].ToUInt64());
            break;

        case 3801:
            type = "shutdown seconds";
            value = static_cast<uint32_t>(args[1].ToUInt64());
            break;

        case 3802:
            type = "shutdown now";
            break;
    }

    BattlenetUI::SendScriptEvent(SCRIPT_EVENT_BN_SYSTEM_MESSAGE, "%s%u", type, value);
    DebugLog("BNUI: SYSTEM MESSAGE RECEIVED: %s %u", type, value);
}

// OFFSET: 0x009091D0
void BattlenetUI::SettingChangedCB(void* param, int32_t argc, const GeneralFunction::Arg* args) {
    Login* login = ClientServices::LoginConnection();

    if (!login || login->GetLoginServerType() != 1 || login->IsTrialAccount() || !BattlenetAPI::IsRIDEnabled()) {
        return;
    }

    if (!_stricmp(args[1].ToString(), "Chat.ProfanityFilterEnabled")) {
        bool enabled = args[2].ToBool();
        BattlenetUI::SendScriptEvent(SCRIPT_EVENT_BN_MATURE_LANGUAGE_FILTER, "%b", enabled != 0);
    }
}

// OFFSET: 0x00909280
void BattlenetUI::LeaveRealm() {
    BattlenetUI::m_friendListLoaded = false;
    BattlenetUI::m_friendInvites.Clear();
    BattlenetUI::m_friends.Clear();
    BattlenetUI::s_friendsOfFriend.Clear();
    BattlenetUI::s_selfRealm = 0;
}

// OFFSET: 0x00909330
void BattlenetUI_LeaveRealm() {
    BattlenetUI::LeaveRealm();
}

// OFFSET: 0x00909340
void BattlenetUI::SetDisconnectedFromBattlenet(bool disconnected) {
    BattlenetUI::SendScriptEvent(SCRIPT_EVENT_BN_DISCONNECTED, "%b", disconnected);

    CGAutoCompleteName::RemoveAllBattlenet();
    BattlenetUI::LeaveRealm();
}

// OFFSET: 0x009096C0
BattlenetUI::MemberListCB::~MemberListCB() {
    if (this->m_members) {
        delete this->m_members;
    }
}

// OFFSET: 0x00909780
void BattlenetUI::FriendInviteListInitializedCB(void* param, int32_t argc, const GeneralFunction::Arg* args) {
    Login* login = ClientServices::LoginConnection();

    if (!login || login->GetLoginServerType() != 1 || login->IsTrialAccount() || !BattlenetAPI::IsRIDEnabled()) {
        return;
    }

    BSN::BSNEncodedObject<Battlenet::Friends::FriendList, Protocol> invites;
    invites.WalkAndGet(args[0].ToObject());

    uint32_t count = BattlenetUI::m_friendInvites.Count();
    BattlenetUI::m_friendInvites.SetCount(count + invites.m_data.m_friends.m_count);

    for (uint32_t i = 0; i < invites.m_data.m_friends.m_count; i++) {
        BattlenetUI::m_friendInvites[count + i] = invites.m_data.m_friends.m_data[i];
    }

    if (!args[1].ToBool()) {
        return;
    }

    if (BattlenetUI::m_friendInvites.Count() > 1) {
        qsort(BattlenetUI::m_friendInvites.Ptr(), BattlenetUI::m_friendInvites.Count(), sizeof(uint32_t), BattlenetUI::QSortFriendInviteListByTimeCB);
    }

    BattlenetUI::SendScriptEvent(SCRIPT_EVENT_BN_FRIEND_INVITE_LIST_INITIALIZED, "%d", BattlenetUI::m_friendInvites.Count());

    if (s_cvShowToastFriendRequest && s_cvShowToastFriendRequest->m_intValue) {
        CGChat::AddChatMessage("FRIEND_PENDING", CHAT_MSG_BN_INLINE_TOAST_ALERT, NULL);
    }

    DebugLog("BNUI: Friend Invite List Initialized");
}

// OFFSET: 0x00909940
void BattlenetUI::FriendInviteAddedCB(void* param, int32_t argc, const GeneralFunction::Arg* args) {
    Login* login = ClientServices::LoginConnection();

    if (!login || login->GetLoginServerType() != 1 || login->IsTrialAccount() || !BattlenetAPI::IsRIDEnabled()) {
        return;
    }

    uint32_t presenceId = static_cast<uint32_t>(args[0].ToUInt64());
    Battlenet::Friends::FriendInviteContainer invite = BattlenetAPI::GetFriendInvite(presenceId);

    BattlenetUI::m_friendInvites.New(presenceId);

    if (BattlenetUI::m_friendInvites.Count() > 1) {
        qsort(BattlenetUI::m_friendInvites.Ptr(), BattlenetUI::m_friendInvites.Count(), sizeof(uint32_t), BattlenetUI::QSortFriendInviteListByTimeCB);
    }

    uint32_t toonId = BattlenetAPI::GetPresenceIDForCurrentToon();
    Battlenet::Presence::FieldVal onlineTime = BattlenetAPI::GetPresenceValue(toonId, BattlenetUI::s_presenceCallbacks[BN_PRESENCE_ONLINE_TIME].m_field);

    if (onlineTime.m_tag != 6 || invite.m_createdTime < onlineTime.Value<int32_t>()) {
        return;
    }

    BattlenetUI::SendScriptEvent(SCRIPT_EVENT_BN_FRIEND_INVITE_ADDED, "%u", presenceId);

    if (s_cvShowToastFriendRequest && s_cvShowToastFriendRequest->m_intValue) {
        CGChat::AddChatMessage("FRIEND_REQUEST", CHAT_MSG_BN_INLINE_TOAST_ALERT, NULL);
    }

    DebugLog("BNUI: Friend Invite Added. ID %u", presenceId);
}

// OFFSET: 0x00909A60
void BattlenetUI::FriendInviteRemovedCB(void* param, int32_t argc, const GeneralFunction::Arg* args) {
    Login* login = ClientServices::LoginConnection();

    if (!login || login->GetLoginServerType() != 1 || login->IsTrialAccount() || !BattlenetAPI::IsRIDEnabled()) {
        return;
    }

    uint32_t presenceId = static_cast<uint32_t>(args[0].ToUInt64());
    int32_t index = -1;

    for (uint32_t i = 0; i < BattlenetUI::m_friendInvites.Count(); i++) {
        if (BattlenetUI::m_friendInvites[i] == presenceId) {
            index = i;
            break;
        }
    }

    if (index >= 0) {
        if (index != BattlenetUI::m_friendInvites.Count() - 1) {
            BattlenetUI::m_friendInvites[index] = BattlenetUI::m_friendInvites[BattlenetUI::m_friendInvites.Count() - 1];
        }

        BattlenetUI::m_friendInvites.m_count--;
        BattlenetUI::SendScriptEvent(SCRIPT_EVENT_BN_FRIEND_INVITE_REMOVED, "%d%u", index, presenceId);

        if (BattlenetUI::m_friendInvites.Count() > 1) {
            qsort(BattlenetUI::m_friendInvites.Ptr(), BattlenetUI::m_friendInvites.Count(), sizeof(uint32_t), BattlenetUI::QSortFriendInviteListByTimeCB);
        }
    }

    DebugLog("BNUI: Friend Invite Removed. index %d, ID %u", index, presenceId);
}

// OFFSET: 0x00909CD0
void BattlenetUI::Uninit() {
    BattlenetUI::s_initialized = false;

    for (uint32_t i = 0; i < 26; i++) {
        BattlenetAPI::UnregisterEvent(BattlenetUI::s_eventCallbacks[i].m_handle);
        BattlenetUI::s_eventCallbacks[i].m_handle = 0;
    }

    for (uint32_t i = 0; i < NUM_PRESENCE_FIELDS; i++) {
        BattlenetAPI::UnregisterPresenceUpdateCallback(BattlenetUI::s_presenceCallbacks[i].m_handle);
        BattlenetUI::s_presenceCallbacks[i].m_handle = 0;
    }

    if (BattlenetUI::s_inviteToConversationCB) {
        BattlenetUI::s_inviteToConversationCB->DecRef();
        BattlenetUI::s_inviteToConversationCB = NULL;
    }

    if (BattlenetUI::s_createConversationCB) {
        BattlenetUI::s_createConversationCB->DecRef();
        BattlenetUI::s_createConversationCB = NULL;
    }

    if (BattlenetUI::s_friendsOfFriendCB) {
        BattlenetUI::s_friendsOfFriendCB->DecRef();
        BattlenetUI::s_friendsOfFriendCB = NULL;
    }

    BattlenetUI::s_accountIdCache.Clear();
    BattlenetUI::s_accountIdCacheCount = 0;
    DebugLog("BNUI: Uninit");
}

// OFFSET: 0x0090A470
ACCOUNTIDCACHE* BattlenetUI::GetAccountIDCache(uint32_t presenceId) {
    HASHKEY_DWORD key;
    key.m_key = presenceId;

    ACCOUNTIDCACHE* cache = BattlenetUI::s_accountIdCache.Ptr(presenceId, key);

    if (cache) {
        return cache;
    }

    if (BattlenetUI::s_accountIdCacheCount >= 500) {
        ACCOUNTIDCACHE* oldest = NULL;
        key.m_key = BattlenetUI::s_accountIdCacheLowest;
        cache = BattlenetUI::s_accountIdCache.Ptr(BattlenetUI::s_accountIdCacheLowest, key);

        if (cache) {
            oldest = cache;
        } else {
            uint32_t lowest = 0xFFFFFFFF;

            ACCOUNTIDCACHE* node = BattlenetUI::s_accountIdCache.Head();

            while (node) {
                ACCOUNTIDCACHE* next = BattlenetUI::s_accountIdCache.Next(node);

                if (node->m_hashval <= lowest) {
                    lowest = node->m_hashval;
                    oldest = node;
                }

                node = next;
            }

            BattlenetUI::s_accountIdCacheLowest = lowest;
        }

        BattlenetUI::s_accountIdCache.Delete(oldest);
        BattlenetUI::s_accountIdCacheLowest++;
        BattlenetUI::s_accountIdCacheCount--;
    }

    if (BattlenetUI::s_accountIdCacheLowest > presenceId) {
        BattlenetUI::s_accountIdCacheLowest = presenceId;
    }

    BattlenetUI::s_accountIdCacheCount++;
    key.m_key = presenceId;

    return BattlenetUI::s_accountIdCache.New(presenceId, key, 0, 0);
}

// OFFSET: 0x0090A5C0
const char* BattlenetUI::GetKStringText(uint32_t kind, uint32_t presenceId) {
    for (int32_t tries = 0; tries < 2; tries++) {
        int32_t index = BattlenetUI::FindFriendIndex(presenceId);

        if (index >= 0) {
            switch (kind) {
                case 'f':
                    return BattlenetUI::m_friends[index].m_name.m_data;

                case 'g':
                    return BattlenetUI::m_friends[index].m_givenName.m_data;

                case 's':
                    return BattlenetUI::m_friends[index].m_surname.m_data;
            }
        }

        if (kind == 'f') {
            const char* name = BattlenetUI::GetNameForPresenceIdHelper(presenceId);

            if (name && *name) {
                return name;
            }
        }

        Battlenet::Presence::FieldVal accountName = BattlenetAPI::GetPresenceValue(presenceId, BattlenetUI::s_presenceCallbacks[BN_PRESENCE_ACCOUNT_NAME].m_field);

        if (accountName.m_tag == 18) {
            if (kind == 'g') {
                return accountName.Value<Battlenet::Account::FullName>().m_givenName.m_data;
            }

            if (kind == 's') {
                return accountName.Value<Battlenet::Account::FullName>().m_surname.m_data;
            }
        }

        if (tries == 1) {
            break;
        }

        ACCOUNTIDCACHE* cache = BattlenetUI::GetAccountIDCache(presenceId);

        if (cache) {
            presenceId = BattlenetAPI::GetPresenceIDForAccount(cache->m_accountId);
        }
    }

    return FrameScript_GetText("UNKNOWNOBJECT", -1, GENDER_NOT_APPLICABLE);
}

// OFFSET: 0x0090A710
void BattlenetUI::SavePrescenceID(uint32_t presenceId) {
    ACCOUNTIDCACHE* cache = BattlenetUI::GetAccountIDCache(presenceId);

    if (cache && !cache->m_accountId) {
        Battlenet::Presence::FieldVal accountId = BattlenetAPI::GetPresenceValue(presenceId, BattlenetUI::s_presenceCallbacks[BN_PRESENCE_PROGRAM].m_field);

        if (accountId.m_tag == 5) {
            cache->m_accountId = accountId.Value<uint32_t>();
        } else {
            DebugLog("Jaaacob, ACCOUNT_ID_FIELD returned something other than U32VAL: %d\n", accountId.m_tag);
        }
    }
}

// OFFSET: 0x0090A780
bool BattlenetUI::TokenizeName(uint32_t presenceId, const char*& name) {
    const char* text = name;
    BattlenetUI::SavePrescenceID(presenceId);
    sprintf(BattlenetUI::s_tokenName, "|K%c%d|k%0*d|k", 'f', presenceId, strlen(text), 0);
    name = BattlenetUI::s_tokenName;
    return true;
}

// OFFSET: 0x0090A7D0
bool BattlenetUI::TokenizeGivenAndSurname(uint32_t presenceId, const char*& givenName, const char*& surname) {
    const char* givenText = givenName;
    BattlenetUI::SavePrescenceID(presenceId);
    sprintf(BattlenetUI::s_tokenGivenName, "|K%c%d|k%0*d|k", 'g', presenceId, strlen(givenText), 0);
    givenName = BattlenetUI::s_tokenGivenName;

    const char* surnameText = surname;
    BattlenetUI::SavePrescenceID(presenceId);
    sprintf(BattlenetUI::s_tokenSurname, "|K%c%d|k%0*d|k", 's', presenceId, strlen(surnameText), 0);
    surname = BattlenetUI::s_tokenSurname;
    return true;
}

// OFFSET: 0x0090A990
const char* BattlenetUI::GetTokenizedName(uint32_t presenceId) {
    const char* name = BattlenetUI::GetNameForPresenceIdHelper(presenceId);
    BattlenetUI::TokenizeName(presenceId, name);
    return name;
}

// OFFSET: 0x0090A9E0
int32_t BattlenetUI::UpdateAutoComplete(uint32_t& presenceId, uint32_t flags, int32_t add, const char* name) {
    if (BattlenetAPI::IsSelf(presenceId)) {
        return CGAutoCompleteName::RemoveFlags(presenceId, 0x7FFFFFFF);
    }

    if (BattlenetAPI::HasRIDFriendship(presenceId) && !BattlenetAPI::IsRIDFriend(presenceId)) {
        presenceId = BattlenetAPI::GetAccountPresenceId(presenceId);
        RIDFriendData* entry = BattlenetUI::FindFriend(presenceId);

        if (entry) {
            name = entry->m_name.m_data;
        }
    }

    int32_t result = CGAutoCompleteName::AddFlags(presenceId, flags, add, name);

    if (name && *name) {
        BattlenetUI::TokenizeName(presenceId, name);
        return CGAutoCompleteName::SetName(presenceId, name, true);
    }

    return result;
}

// OFFSET: 0x0090AA90
void BattlenetUI::PresenceUpdateCB(void* param, int32_t argc, const GeneralFunction::Arg* args) {
    Login* login = ClientServices::LoginConnection();

    if (!login || login->GetLoginServerType() != 1 || login->IsTrialAccount() || !BattlenetAPI::IsRIDEnabled()) {
        return;
    }

    BSN::BSNEncodedObject<Battlenet::Presence::FieldUpdate, Protocol> update(args[0].ToObject());
    const char* name = BattlenetUI::GetTokenizedName(update.m_data.m_id);

    if (update.m_data.m_handle == BattlenetUI::s_presenceCallbacks[BN_PRESENCE_ACCOUNT_NAME].m_field) {
        if (!BattlenetAPI::HasRIDFriendship(update.m_data.m_id) || BattlenetAPI::IsRIDFriend(update.m_data.m_id)) {
            CGAutoCompleteName::SetName(update.m_data.m_id, name, true);
        } else {
            CGAutoCompleteName::RemoveFlags(update.m_data.m_id, 0x7FFFFFFF);
        }
    }

    int32_t index = BattlenetUI::FindFriendIndex(update.m_data.m_id);

    if (index >= 0) {
        BattlenetUI::SendScriptEvent(SCRIPT_EVENT_BN_FRIEND_INFO_CHANGED, "%d", index + 1);
    }
}

// OFFSET: 0x0090ABA0
void BattlenetUI::FactionUpdateCB(void* param, int32_t argc, const GeneralFunction::Arg* args) {
    Login* login = ClientServices::LoginConnection();

    if (!login || login->GetLoginServerType() != 1 || login->IsTrialAccount() || !BattlenetAPI::IsRIDEnabled()) {
        return;
    }

    BSN::BSNEncodedObject<Battlenet::Presence::FieldUpdate, Protocol> update(args[0].ToObject());

    if (!BattlenetAPI::IsSelf(update.m_data.m_id)) {
        BattlenetUI::UpdateAutoComplete(update.m_data.m_id, 0x20, 0, NULL);
    }
}

// OFFSET: 0x0090AC30
void BattlenetUI::NewPresenceCB(void* param, int32_t argc, const GeneralFunction::Arg* args) {
    Login* login = ClientServices::LoginConnection();

    if (!login || login->GetLoginServerType() != 1 || login->IsTrialAccount() || !BattlenetAPI::IsRIDEnabled()) {
        return;
    }

    uint32_t presenceId = static_cast<uint32_t>(args[0].ToUInt64());

    if (!BattlenetUI::m_friendListLoaded) {
        DebugLog("BNUI: New Presence ID %u before Friend List initialized.", presenceId);
        return;
    }

    Battlenet::Presence::FieldVal toon = BattlenetAPI::GetPresenceValue(presenceId, BattlenetUI::s_presenceCallbacks[BN_PRESENCE_TOON_NAME].m_field);

    if (toon.m_tag == 17) {
        if (BattlenetAPI::HasRIDFriendship(presenceId)) {
            BattlenetUI::SendScriptEvent(SCRIPT_EVENT_BN_FRIEND_TOON_ONLINE, "%u", presenceId);
            DebugLog("BNUI: >> TOON %s is now ONLINE. ID %u (new presence)", BattlenetUI::GetTokenizedName(presenceId), presenceId);
        } else {
            BattlenetUI::SendScriptEvent(SCRIPT_EVENT_BN_NEW_PRESENCE, "%u%s", presenceId, toon.Value<Battlenet::Toon::FullName>().m_name.m_data);
            DebugLog("BNUI: New Presence ID %u, name %s", presenceId, toon.Value<Battlenet::Toon::FullName>().m_name.m_data);
        }
    } else {
        const char* name = BattlenetUI::GetTokenizedName(presenceId);
        BattlenetUI::SendScriptEvent(SCRIPT_EVENT_BN_NEW_PRESENCE, "%u%s", presenceId, name);
        DebugLog("BNUI: New Non-Toon Presence ID %u, name %s", presenceId, name);
    }
}

// OFFSET: 0x0090AD70
void BattlenetUI::ToonNameUpdateCB(void* param, int32_t argc, const GeneralFunction::Arg* args) {
    Login* login = ClientServices::LoginConnection();

    if (!login || login->GetLoginServerType() != 1 || login->IsTrialAccount() || !BattlenetAPI::IsRIDEnabled()) {
        return;
    }

    BSN::BSNEncodedObject<Battlenet::Presence::FieldUpdate, Protocol> update(args[0].ToObject());
    bool friendship = BattlenetAPI::HasRIDFriendship(update.m_data.m_id);

    if (update.m_data.m_value.m_tag == 17) {
        BattlenetUI::SendScriptEvent(SCRIPT_EVENT_BN_TOON_NAME_UPDATED, "%u%s%b", update.m_data.m_id, update.m_data.m_value.Value<Battlenet::Toon::FullName>().m_name.m_data, friendship);
        DebugLog("BNUI: Toon Name Updated, %s friend. ID %u, name %s", friendship ? "" : "non", update.m_data.m_id, update.m_data.m_value.Value<Battlenet::Toon::FullName>().m_name.m_data);
    }

    if (!BattlenetAPI::IsSelf(update.m_data.m_id)) {
        BattlenetUI::UpdateAutoComplete(update.m_data.m_id, 0x20, 0, NULL);
    }
}

// OFFSET: 0x0090AE80
void BattlenetUI::OnlinePresenceChangedCB(void* param, int32_t argc, const GeneralFunction::Arg* args) {
    Login* login = ClientServices::LoginConnection();

    if (!login || login->GetLoginServerType() != 1 || login->IsTrialAccount() || !BattlenetAPI::IsRIDEnabled()) {
        return;
    }

    BSN::BSNEncodedObject<Battlenet::Presence::FieldUpdate, Protocol> update(args[0].ToObject());
    bool online = false;

    if (update.m_data.m_value.m_tag == 1) {
        online = update.m_data.m_value.Value<bool>() != 0;
    }

    bool ridFriend = BattlenetAPI::IsRIDFriend(update.m_data.m_id);
    bool cidFriend = BattlenetAPI::IsCIDFriend(update.m_data.m_id);

    BattlenetUI::DisambiguateFriendList();

    if (BattlenetUI::m_friends.Count() > 1) {
        qsort(BattlenetUI::m_friends.Ptr(), BattlenetUI::m_friends.Count(), sizeof(RIDFriendData), BattlenetUI::QSortFriendListCB);
    }

    if (online) {
        BattlenetUI::UpdateAutoComplete(update.m_data.m_id, 0x20, 0, NULL);
        BattlenetUI::SendScriptEvent(SCRIPT_EVENT_BN_FRIEND_INFO_CHANGED, NULL);
        return;
    }

    CGAutoCompleteName::RemoveFlags(update.m_data.m_id, 0x20);

    if (ridFriend) {
        BattlenetUI::SendScriptEvent(SCRIPT_EVENT_BN_FRIEND_ACCOUNT_OFFLINE, "%u", update.m_data.m_id);

        if (BattlenetUI::m_friendListLoaded && s_cvShowToastOffline && s_cvShowToastOffline->m_intValue) {
            ChatConversation conversation;
            conversation.m_presenceID = update.m_data.m_id;
            conversation.m_inactive = 1;
            ChatMessageParams params;
            params.m_senderName = BattlenetUI::GetTokenizedName(update.m_data.m_id);
            params.m_conversation = &conversation;
            CGChat::AddChatMessage("FRIEND_OFFLINE", CHAT_MSG_BN_INLINE_TOAST_ALERT, &params);
        }

        DebugLog("BNUI: >> ACCOUNT %s is now OFFLINE. ID %u", BattlenetUI::GetTokenizedName(update.m_data.m_id), update.m_data.m_id);
    } else if (cidFriend) {
        BattlenetUI::SendScriptEvent(SCRIPT_EVENT_BN_FRIEND_TOON_OFFLINE, "%u", update.m_data.m_id);
        DebugLog("BNUI: >> TOON %s is now OFFLINE. ID %u", BattlenetUI::GetTokenizedName(update.m_data.m_id), update.m_data.m_id);
    }

    BattlenetUI::SendScriptEvent(SCRIPT_EVENT_BN_FRIEND_INFO_CHANGED, NULL);
}

// OFFSET: 0x0090B070
void BattlenetUI::OnlineTimeChangedCB(void* param, int32_t argc, const GeneralFunction::Arg* args) {
    Login* login = ClientServices::LoginConnection();

    if (!login || login->GetLoginServerType() != 1 || login->IsTrialAccount() || !BattlenetAPI::IsRIDEnabled()) {
        return;
    }

    BSN::BSNEncodedObject<Battlenet::Presence::FieldUpdate, Protocol> update(args[0].ToObject());

    if (BattlenetAPI::IsSelf(update.m_data.m_id) || update.m_data.m_value.m_tag != 6) {
        return;
    }

    int32_t onlineTime = update.m_data.m_value.Value<int32_t>();
    bool ridFriend = BattlenetAPI::IsRIDFriend(update.m_data.m_id);
    bool cidFriend = BattlenetAPI::IsCIDFriend(update.m_data.m_id);

    if (!ridFriend && !cidFriend) {
        return;
    }

    uint32_t toonId = BattlenetAPI::GetPresenceIDForCurrentToon();
    Battlenet::Presence::FieldVal selfTime = BattlenetAPI::GetPresenceValue(toonId, BattlenetUI::s_presenceCallbacks[BN_PRESENCE_ONLINE_TIME].m_field);

    if (selfTime.m_tag != 6) {
        return;
    }

    int32_t selfOnlineTime = selfTime.Value<int32_t>();

    if (selfOnlineTime && onlineTime && onlineTime > selfOnlineTime) {
        if (ridFriend) {
            BattlenetUI::SendScriptEvent(SCRIPT_EVENT_BN_FRIEND_ACCOUNT_ONLINE, "%u", update.m_data.m_id);

            if (s_cvShowToastOnline && s_cvShowToastOnline->m_intValue) {
                ChatConversation conversation;
                conversation.m_presenceID = update.m_data.m_id;
                conversation.m_inactive = 1;
                ChatMessageParams params;
                params.m_senderName = BattlenetUI::GetTokenizedName(update.m_data.m_id);
                params.m_conversation = &conversation;
                CGChat::AddChatMessage("FRIEND_ONLINE", CHAT_MSG_BN_INLINE_TOAST_ALERT, &params);
            }

            DebugLog("BNUI: >> ACCOUNT %s Online Time changed. ID %u", BattlenetUI::GetTokenizedName(update.m_data.m_id), update.m_data.m_id);
        } else if (cidFriend) {
            BattlenetUI::SendScriptEvent(SCRIPT_EVENT_BN_FRIEND_TOON_ONLINE, "%u", update.m_data.m_id);
            DebugLog("BNUI: >> TOON %s is now ONLINE. ID %u", BattlenetUI::GetTokenizedName(update.m_data.m_id), update.m_data.m_id);
        }
    }

    BattlenetUI::SendScriptEvent(SCRIPT_EVENT_BN_FRIEND_INFO_CHANGED, NULL);
}

// OFFSET: 0x0090B250
void BattlenetUI::CustomMessageChangedCB(void* param, int32_t argc, const GeneralFunction::Arg* args) {
    Login* login = ClientServices::LoginConnection();

    if (!login || login->GetLoginServerType() != 1 || login->IsTrialAccount() || !BattlenetAPI::IsRIDEnabled()) {
        return;
    }

    BSN::BSNEncodedObject<Battlenet::Presence::FieldUpdate, Protocol> update(args[0].ToObject());
    uint32_t toonId = BattlenetAPI::GetPresenceIDForCurrentToon();
    int32_t onlineTime = 0;

    {
        Battlenet::Presence::FieldVal value = BattlenetAPI::GetPresenceValue(toonId, BattlenetUI::s_presenceCallbacks[BN_PRESENCE_ONLINE_TIME].m_field);

        if (value.m_tag == 6) {
            onlineTime = value.Value<int32_t>();
        }
    }

    const char* message = update.m_data.m_value.m_tag == 13 ? update.m_data.m_value.Value<BSN::String<127> >().m_data : "";
    int32_t broadcastTime = 0;

    {
        Battlenet::Presence::FieldVal value = BattlenetAPI::GetPresenceValue(update.m_data.m_id, BattlenetUI::s_presenceCallbacks[BN_PRESENCE_BROADCAST_TIME].m_field);

        if (value.m_tag == 6) {
            broadcastTime = value.Value<int32_t>();
        }
    }

    char text[512];

    if (toonId == update.m_data.m_id) {
        if (onlineTime && broadcastTime >= onlineTime) {
            BattlenetUI::SendScriptEvent(SCRIPT_EVENT_BN_CUSTOM_MESSAGE_CHANGED, NULL);
            FilterChatLinks(message, text, 509);
            CGChat::AddChatMessage(text, CHAT_MSG_BN_INLINE_TOAST_BROADCAST_INFORM, NULL);
        } else {
            BattlenetUI::SendScriptEvent(SCRIPT_EVENT_BN_CUSTOM_MESSAGE_LOADED, NULL);
        }

        return;
    }

    if (BattlenetAPI::IsRIDFriend(update.m_data.m_id)) {
        if (!onlineTime || broadcastTime < onlineTime) {
            return;
        }

        {
            Battlenet::Presence::FieldVal value = BattlenetAPI::GetPresenceValue(update.m_data.m_id, BattlenetUI::s_presenceCallbacks[BN_PRESENCE_ONLINE_TIME].m_field);

            if (value.m_tag == 6 && broadcastTime < value.Value<int32_t>()) {
                return;
            }
        }

        BattlenetUI::SendScriptEvent(SCRIPT_EVENT_BN_CUSTOM_MESSAGE_CHANGED, "%u", update.m_data.m_id);

        if (s_cvShowToastBroadcast && s_cvShowToastBroadcast->m_intValue) {
            FilterChatLinks(message, text, 509);

            ChatConversation conversation;
            conversation.m_presenceID = update.m_data.m_id;
            conversation.m_inactive = 1;
            ChatMessageParams params;
            params.m_senderName = BattlenetUI::GetTokenizedName(update.m_data.m_id);
            params.m_conversation = &conversation;
            CGChat::AddChatMessage(text, CHAT_MSG_BN_INLINE_TOAST_BROADCAST, &params);
        }
    }

    DebugLog("BNUI: Custom Message Changed");
}

// OFFSET: 0x0090B480
void BattlenetUI::ChatWhisperSentCB(void* param, int32_t argc, const GeneralFunction::Arg* args) {
    Login* login = ClientServices::LoginConnection();

    if (!login || login->GetLoginServerType() != 1 || login->IsTrialAccount() || !BattlenetAPI::IsRIDEnabled()) {
        return;
    }

    uint32_t presenceId = static_cast<uint32_t>(args[0].ToUInt64());
    const char* message = args[1].ToString();

    if (!message) {
        message = "";
    }

    BattlenetUI::UpdateAutoComplete(presenceId, 0x30, 1, BattlenetUI::GetTokenizedName(presenceId));

    const char* toonName = "unknown";
    Battlenet::Presence::FieldVal toon = BattlenetAPI::GetPresenceValue(presenceId, BattlenetUI::s_presenceCallbacks[BN_PRESENCE_TOON_NAME].m_field);

    if (toon.m_tag == 17) {
        toonName = toon.Value<Battlenet::Toon::FullName>().m_name.m_data;
    }

    {
        Battlenet::Presence::FieldVal afk = BattlenetAPI::GetPresenceValue(presenceId, BattlenetUI::s_presenceCallbacks[BN_PRESENCE_AFK].m_field);
    }

    {
        Battlenet::Presence::FieldVal dnd = BattlenetAPI::GetPresenceValue(presenceId, BattlenetUI::s_presenceCallbacks[BN_PRESENCE_DND].m_field);
    }

    char text[1024];
    FilterChatLinks(message, text, 1021);

    ChatConversation conversation;
    conversation.m_presenceID = presenceId;
    conversation.m_inactive = 1;
    ChatMessageParams params;
    params.m_senderName = BattlenetUI::GetTokenizedName(presenceId);
    params.m_conversation = &conversation;
    CGChat::AddChatMessage(text, CHAT_MSG_BN_WHISPER_INFORM, &params);

    DebugLog("BNUI: Whisper Sent to %s, ID %u. %s", toonName, presenceId, message);
}

// OFFSET: 0x0090B610
void BattlenetUI::ChatWhisperReceivedCB(void* param, int32_t argc, const GeneralFunction::Arg* args) {
    Login* login = ClientServices::LoginConnection();

    if (!login || login->GetLoginServerType() != 1 || login->IsTrialAccount() || !BattlenetAPI::IsRIDEnabled()) {
        return;
    }

    uint32_t presenceId = static_cast<uint32_t>(args[0].ToUInt64());
    const char* message = args[1].ToString();

    if (!message) {
        message = "";
    }

    BattlenetUI::UpdateAutoComplete(presenceId, 0x30, 1, BattlenetUI::GetTokenizedName(presenceId));

    const char* toonName = "unknown";
    Battlenet::Presence::FieldVal toon = BattlenetAPI::GetPresenceValue(presenceId, BattlenetUI::s_presenceCallbacks[BN_PRESENCE_TOON_NAME].m_field);

    if (toon.m_tag == 17) {
        toonName = toon.Value<Battlenet::Toon::FullName>().m_name.m_data;
    }

    {
        Battlenet::Presence::FieldVal afk = BattlenetAPI::GetPresenceValue(presenceId, BattlenetUI::s_presenceCallbacks[BN_PRESENCE_AFK].m_field);
    }

    {
        Battlenet::Presence::FieldVal dnd = BattlenetAPI::GetPresenceValue(presenceId, BattlenetUI::s_presenceCallbacks[BN_PRESENCE_DND].m_field);
    }

    char text[1024];
    FilterChatLinks(message, text, 1021);

    ChatConversation conversation;
    conversation.m_presenceID = presenceId;
    conversation.m_inactive = 1;
    ChatMessageParams params;
    params.m_senderName = BattlenetUI::GetTokenizedName(presenceId);
    params.m_conversation = &conversation;
    CGChat::AddChatMessage(text, CHAT_MSG_BN_WHISPER, &params);

    DebugLog("BNUI: Whisper Received From %s, ID %u. %s", toonName, presenceId, message);
}

// OFFSET: 0x0090B7A0
void BattlenetUI::ChatChannelJoinedCB(void* param, int32_t argc, const GeneralFunction::Arg* args) {
    Login* login = ClientServices::LoginConnection();

    if (!login || login->GetLoginServerType() != 1 || login->IsTrialAccount() || !BattlenetAPI::IsRIDEnabled()) {
        return;
    }

    uint8_t channel = static_cast<uint8_t>(args[0].ToUInt64());
    int32_t type = static_cast<int32_t>(args[1].ToInt64());

    MemberListCB members;
    BattlenetAPI::GetChatMemberList(channel, &members);

    for (uint32_t i = 0; i < members.m_members->Count(); i++) {
        uint32_t presenceId = (*members.m_members)[i];
        BattlenetUI::UpdateAutoComplete(presenceId, 0x30, 1, BattlenetUI::GetTokenizedName(presenceId));
    }

    if (type == 2) {
        ChatConversation conversation;
        conversation.m_presenceID = BattlenetAPI::GetPresenceIDForCurrentAccount();
        conversation.m_index = channel;
        conversation.m_inactive = 0;
        ChatMessageParams params;
        params.m_senderName = BattlenetUI::GetTokenizedName(BattlenetAPI::GetPresenceIDForCurrentAccount());
        params.m_conversation = &conversation;
        CGChat::AddChatMessage("YOU_JOINED_CONVERSATION", CHAT_MSG_BN_CONVERSATION_NOTICE, &params);
        BattlenetUI::SendScriptEvent(SCRIPT_EVENT_BN_CHAT_CHANNEL_JOINED, "%d", channel + 1);

        if (s_cvShowToastConversation && s_cvShowToastConversation->m_intValue) {
            uint32_t accountId = BattlenetAPI::GetPresenceIDForCurrentAccount();
            uint32_t toonId = BattlenetAPI::GetPresenceIDForCurrentToon();
            const char* delimiter = FrameScript_GetText("PLAYER_LIST_DELIMITER", -1, GENDER_NOT_APPLICABLE);
            char text[3000];
            bool first = true;

            for (uint32_t i = 0; i < members.m_members->Count(); i++) {
                uint32_t presenceId = (*members.m_members)[i];

                if (presenceId != accountId && presenceId != toonId) {
                    if (first) {
                        SStrCopyUTF8(text, BattlenetUI::GetTokenizedName(presenceId), sizeof(text), sizeof(text));
                        first = false;
                    } else {
                        SStrPrintf(text, sizeof(text), "%s%s%s", text, delimiter, BattlenetUI::GetTokenizedName((*members.m_members)[i]));
                    }
                }
            }

            ChatMessageParams params2;
            params2.m_senderName = BattlenetUI::GetTokenizedName(BattlenetAPI::GetPresenceIDForCurrentAccount());
            params2.m_conversation = &conversation;
            CGChat::AddChatMessage(text, CHAT_MSG_BN_INLINE_TOAST_CONVERSATION, &params2);
        }
    }

    DebugLog("BNUI: Chat Channel Joined. channel %d", channel + 1);
}

// OFFSET: 0x0090BBB0
void BattlenetUI::ChatChannelLeftCB(void* param, int32_t argc, const GeneralFunction::Arg* args) {
    Login* login = ClientServices::LoginConnection();

    if (!login || login->GetLoginServerType() != 1 || login->IsTrialAccount() || !BattlenetAPI::IsRIDEnabled()) {
        return;
    }

    uint8_t channel = static_cast<uint8_t>(args[0].ToUInt64());

    ChatConversation conversation;
    conversation.m_index = channel;
    conversation.m_presenceID = BattlenetAPI::GetPresenceIDForCurrentAccount();
    conversation.m_inactive = 0;
    ChatMessageParams params;
    params.m_senderName = BattlenetUI::GetTokenizedName(BattlenetAPI::GetPresenceIDForCurrentAccount());
    params.m_conversation = &conversation;
    CGChat::AddChatMessage("YOU_LEFT_CONVERSATION", CHAT_MSG_BN_CONVERSATION_NOTICE, &params);
    BattlenetUI::SendScriptEvent(SCRIPT_EVENT_BN_CHAT_CHANNEL_LEFT, "%d", channel + 1);
    DebugLog("BNUI: Chat Channel Left. Channel %d", channel + 1);
}

// OFFSET: 0x0090BCB0
void BattlenetUI::ChatChannelClosedCB(void* param, int32_t argc, const GeneralFunction::Arg* args) {
    Login* login = ClientServices::LoginConnection();

    if (!login || login->GetLoginServerType() != 1 || login->IsTrialAccount() || !BattlenetAPI::IsRIDEnabled()) {
        return;
    }

    uint8_t channel = static_cast<uint8_t>(args[0].ToUInt64());
    uint32_t presenceId = static_cast<uint32_t>(args[1].ToUInt64());

    ChatConversation conversation;
    conversation.m_index = channel;
    conversation.m_presenceID = presenceId;
    conversation.m_inactive = 0;
    ChatMessageParams params;
    params.m_senderName = BattlenetUI::GetTokenizedName(presenceId);
    params.m_conversation = &conversation;
    CGChat::AddChatMessage("CONVERSATION_CONVERTED_TO_WHISPER", CHAT_MSG_BN_CONVERSATION_NOTICE, &params);
    BattlenetUI::SendScriptEvent(SCRIPT_EVENT_BN_CHAT_CHANNEL_CLOSED, "%d%s", channel + 1, BattlenetUI::GetTokenizedName(presenceId));
    DebugLog("BNUI: Chat Channel Closed channel, index %d, remaining ID %u", channel + 1, presenceId);
}

// OFFSET: 0x0090BDC0
void BattlenetUI::ChatChannelMessageReceivedCB(void* param, int32_t argc, const GeneralFunction::Arg* args) {
    Login* login = ClientServices::LoginConnection();

    if (!login || login->GetLoginServerType() != 1 || login->IsTrialAccount() || !BattlenetAPI::IsRIDEnabled()) {
        return;
    }

    uint8_t channel = static_cast<uint8_t>(args[0].ToUInt64());
    uint32_t presenceId = static_cast<uint32_t>(args[1].ToUInt64());
    const char* message = args[2].ToString();

    if (!message) {
        message = "";
    }

    char text[1024];
    FilterChatLinks(message, text, 1021);
    BattlenetUI::UpdateAutoComplete(presenceId, 0x30, 1, BattlenetUI::GetTokenizedName(presenceId));

    ChatConversation conversation;
    conversation.m_index = channel;
    conversation.m_presenceID = presenceId;
    conversation.m_inactive = 0;
    ChatMessageParams params;
    params.m_senderName = BattlenetUI::GetTokenizedName(presenceId);
    params.m_conversation = &conversation;
    CGChat::AddChatMessage(text, CHAT_MSG_BN_CONVERSATION, &params);
    DebugLog("BNUI: Chat Channel Message Recieved. channel %d, ID %u, msg: %s", channel + 1, presenceId, message);
}

// OFFSET: 0x0090BF00
void BattlenetUI::ChatChannelMemberJoinedCB(void* param, int32_t argc, const GeneralFunction::Arg* args) {
    Login* login = ClientServices::LoginConnection();

    if (!login || login->GetLoginServerType() != 1 || login->IsTrialAccount() || !BattlenetAPI::IsRIDEnabled()) {
        return;
    }

    uint8_t channel = static_cast<uint8_t>(args[0].ToUInt64());
    uint32_t presenceId = static_cast<uint32_t>(args[1].ToUInt64());

    BattlenetUI::UpdateAutoComplete(presenceId, 0x30, 1, BattlenetUI::GetTokenizedName(presenceId));

    ChatConversation conversation;
    conversation.m_index = channel;
    conversation.m_presenceID = presenceId;
    conversation.m_inactive = 0;
    ChatMessageParams params;
    params.m_senderName = BattlenetUI::GetTokenizedName(presenceId);
    params.m_conversation = &conversation;
    CGChat::AddChatMessage("MEMBER_JOINED", CHAT_MSG_BN_CONVERSATION_NOTICE, &params);
    BattlenetUI::SendScriptEvent(SCRIPT_EVENT_BN_CHAT_CHANNEL_MEMBER_JOINED, "%d%u", channel + 1, presenceId);
    DebugLog("BNUI: Chat Channel Member Joined. channel %d, id %u", channel + 1, presenceId);
}

// OFFSET: 0x0090C020
void BattlenetUI::ChatChannelMemberLeftCB(void* param, int32_t argc, const GeneralFunction::Arg* args) {
    Login* login = ClientServices::LoginConnection();

    if (!login || login->GetLoginServerType() != 1 || login->IsTrialAccount() || !BattlenetAPI::IsRIDEnabled()) {
        return;
    }

    uint8_t channel = static_cast<uint8_t>(args[0].ToUInt64());
    uint32_t presenceId = static_cast<uint32_t>(args[1].ToUInt64());

    ChatConversation conversation;
    conversation.m_index = channel;
    conversation.m_presenceID = presenceId;
    conversation.m_inactive = 0;
    ChatMessageParams params;
    params.m_senderName = BattlenetUI::GetTokenizedName(presenceId);
    params.m_conversation = &conversation;
    CGChat::AddChatMessage("MEMBER_LEFT", CHAT_MSG_BN_CONVERSATION_NOTICE, &params);
    BattlenetUI::SendScriptEvent(SCRIPT_EVENT_BN_CHAT_CHANNEL_MEMBER_LEFT, "%d%u", channel + 1, presenceId);
    DebugLog("BNUI: Chat Channel Member Left. channel %d, id %u", channel + 1, presenceId);
}

// OFFSET: 0x0090C120
void BattlenetUI::DisplayError(uint16_t error, uint32_t presenceId) {
    if (!error) {
        return;
    }

    const char* format = BattlenetAPI::GetErrorString(error);

    if (!format) {
        return;
    }

    char text[1024];
    int32_t count = CountFormatStrings(format);

    if (count == 0) {
        SStrCopyUTF8(text, format, sizeof(text), sizeof(text));
    } else if (count == 1) {
        const char* name = BattlenetUI::GetTokenizedName(presenceId);

        if (!name) {
            name = FrameScript_GetText("UNKNOWNOBJECT", -1, GENDER_NOT_APPLICABLE);
        }

        SStrPrintf(text, sizeof(text), format, name);
    } else {
        SStrPrintf(text, sizeof(text), FrameScript_GetText("INTERNAL_STRING_ERROR", -1, GENDER_NOT_APPLICABLE), error);
    }

    if (error != 318) {
        CGChat::AddChatMessage(text, static_cast<SLASH_COMMAND_ID>(0), NULL);
        CGGameUI::AddErrorMessage(text, 0);
    } else {
        ChatConversation conversation;
        conversation.m_presenceID = presenceId;
        conversation.m_inactive = 1;
        ChatMessageParams params;
        const char* name = BattlenetUI::GetTokenizedName(presenceId);
        params.m_senderName = name;
        params.m_conversation = &conversation;
        CGChat::AddChatMessage(text, CHAT_MSG_BN_WHISPER_PLAYER_OFFLINE, &params);
        CGAutoCompleteName::RemoveFlagsByName(name, 0x20, false, true);
    }
}

// OFFSET: 0x0090C250
void BattlenetUI::RequestFofInfoResultCB(void* param, int32_t argc, const GeneralFunction::Arg* args) {
    Login* login = ClientServices::LoginConnection();

    if (!login || login->GetLoginServerType() != 1 || login->IsTrialAccount() || !BattlenetAPI::IsRIDEnabled()) {
        return;
    }

    uint16_t error = static_cast<uint16_t>(args[0].ToUInt64());

    if (!error) {
        BSN::BSNEncodedObject<Battlenet::Friends::FriendsOfFriendList, Protocol> friends;
        friends.WalkAndGet(args[1].ToObject());

        uint32_t count = BattlenetUI::s_friendsOfFriend.Count();
        BattlenetUI::s_friendsOfFriend.SetCount(count + friends.m_data.m_friends.m_count);

        for (uint32_t i = 0; i < friends.m_data.m_friends.m_count; i++) {
            BattlenetUI::s_friendsOfFriend[count + i] = friends.m_data.m_friends.m_data[i];
        }

        if (args[2].ToBool()) {
            BattlenetUI::SendScriptEvent(SCRIPT_EVENT_BN_REQUEST_FOF_SUCCEEDED, NULL);
        }
    } else {
        BattlenetUI::DisplayError(error, 0);
        BattlenetUI::SendScriptEvent(SCRIPT_EVENT_BN_REQUEST_FOF_FAILED, NULL);
        DebugLog("BNUI: FOF info request failed.");
    }
}

// OFFSET: 0x0090C390
void BattlenetUI::PopulateAutoComplete() {
    uint32_t count = BattlenetUI::m_friends.Count();

    for (uint32_t i = 0; i < count; i++) {
        uint32_t presenceId = BattlenetUI::m_friends[i].m_presenceID;
        const char* name = BattlenetUI::GetTokenizedName(presenceId);
        bool online = false;
        Battlenet::Presence::FieldVal value = BattlenetAPI::GetPresenceValue(presenceId, BattlenetUI::s_presenceCallbacks[BN_PRESENCE_ONLINE].m_field);

        if (value.m_tag == 1) {
            online = value.Value<bool>() != 0;
        }

        BattlenetUI::UpdateAutoComplete(presenceId, (online ? 0x20 : 0) | 0x4, 0, name);
    }

    MemberListCB members;

    for (uint8_t channel = 0; channel < 6; channel++) {
        members.m_members->Clear();
        BattlenetAPI::GetChatMemberList(channel, &members);

        for (uint32_t i = 0; i < members.m_members->Count(); i++) {
            uint32_t presenceId = (*members.m_members)[i];
            BattlenetUI::UpdateAutoComplete(presenceId, 0x30, 1, BattlenetUI::GetTokenizedName(presenceId));
        }
    }
}

// OFFSET: 0x0090C620
int32_t BattlenetUI::BNGetFriendInfoHelper(lua_State* L, uint32_t index) {
    if (index >= BattlenetUI::m_friends.Count()) {
        return 0;
    }

    uint32_t presenceId = BattlenetUI::m_friends[index].m_presenceID;
    lua_pushnumber(L, presenceId);

    Battlenet::Presence::FieldVal toon = BattlenetAPI::GetPresenceValue(presenceId, BattlenetUI::s_presenceCallbacks[BN_PRESENCE_TOON_NAME].m_field);
    Battlenet::Presence::FieldVal online = BattlenetAPI::GetPresenceValue(presenceId, BattlenetUI::s_presenceCallbacks[BN_PRESENCE_ONLINE].m_field);
    Battlenet::Presence::FieldVal lastOnline = BattlenetAPI::GetPresenceValue(presenceId, BattlenetUI::s_presenceCallbacks[BN_PRESENCE_LAST_ONLINE].m_field);
    Battlenet::Presence::FieldVal afk = BattlenetAPI::GetPresenceValue(presenceId, BattlenetUI::s_presenceCallbacks[BN_PRESENCE_AFK].m_field);
    Battlenet::Presence::FieldVal dnd = BattlenetAPI::GetPresenceValue(presenceId, BattlenetUI::s_presenceCallbacks[BN_PRESENCE_DND].m_field);
    Battlenet::Presence::FieldVal broadcast = BattlenetAPI::GetPresenceValue(presenceId, BattlenetUI::s_presenceCallbacks[BN_PRESENCE_BROADCAST].m_field);
    Battlenet::Presence::FieldVal note = BattlenetAPI::GetPresenceValue(presenceId, BattlenetUI::s_presenceCallbacks[BN_PRESENCE_NOTE].m_field);

    const char* givenName = BattlenetUI::m_friends[index].m_givenName.m_data;
    const char* surname = BattlenetUI::m_friends[index].m_surname.m_data;
    BattlenetUI::TokenizeGivenAndSurname(presenceId, givenName, surname);
    lua_pushstring(L, givenName);
    lua_pushstring(L, surname);

    if (toon.m_tag == 17) {
        uint32_t toonId = BattlenetAPI::GetPresenceIDForToon(&toon.Value<Battlenet::Toon::FullName>());
        const char* toonName = BattlenetAPI::GetToonNameForPresenceId(presenceId, false);

        if (!toonName) {
            toonName = toon.Value<Battlenet::Toon::FullName>().m_name.m_data;
        }

        lua_pushstring(L, toonName);
        lua_pushnumber(L, toonId);
        lua_pushstring(L, BattlenetUI::GetProgramString(toon.Value<Battlenet::Toon::FullName>().m_programId));
    } else {
        lua_pushnil(L);
        lua_pushnil(L);
        lua_pushnil(L);
    }

    bool isOnline = false;

    if (online.m_tag == 1) {
        isOnline = online.Value<bool>() != 0;
    }

    lua_pushboolean(L, isOnline);

    uint32_t lastOnlineTime = 0;

    if (lastOnline.m_tag == 6) {
        lastOnlineTime = lastOnline.Value<int32_t>();
    }

    lua_pushnumber(L, lastOnlineTime);

    uint8_t isAFK = 0;

    if (afk.m_tag == 11) {
        isAFK = afk.Value<bool>();
    }

    lua_pushboolean(L, isAFK);

    uint8_t isDND = 0;

    if (dnd.m_tag == 11) {
        isDND = dnd.Value<bool>();
    }

    lua_pushboolean(L, isDND);

    if (broadcast.m_tag == 13) {
        lua_pushstring(L, broadcast.Value<BSN::String<127> >().m_data);
    } else {
        lua_pushnil(L);
    }

    if (note.m_tag == 13) {
        lua_pushstring(L, note.Value<BSN::String<127> >().m_data);
    } else {
        lua_pushnil(L);
    }

    lua_pushboolean(L, BattlenetAPI::IsRIDFriend(presenceId));

    {
        Battlenet::Presence::FieldVal broadcastTime = BattlenetAPI::GetPresenceValue(presenceId, BattlenetUI::s_presenceCallbacks[BN_PRESENCE_BROADCAST_TIME].m_field);

        if (broadcastTime.m_tag == 6) {
            lua_pushnumber(L, broadcastTime.Value<int32_t>());
        } else {
            lua_pushnumber(L, 0.0);
        }
    }

    {
        Battlenet::Presence::FieldVal value = BattlenetAPI::GetPresenceValue(presenceId, BattlenetUI::s_presenceCallbacks[PRESENCE_FIELD_FLAGS].m_field);
        uint64_t flags = value.m_tag == 7 ? value.Value<uint64_t>() : 0;
        lua_pushboolean(L, CGGameUI::s_canSendSoR && (flags & 1));
    }

    return 15;
}

// OFFSET: 0x0090D590
void BattlenetUI::FriendListInitializedCB(void* param, int32_t argc, const GeneralFunction::Arg* args) {
    Login* login = ClientServices::LoginConnection();

    if (!login || login->GetLoginServerType() != 1 || login->IsTrialAccount() || !BattlenetAPI::IsRIDEnabled()) {
        return;
    }

    if (args[1].ToBool()) {
        BSN::BSNEncodedObject<Battlenet::Friends::FriendList, Protocol> friendList;
        friendList.WalkAndGet(args[0].ToObject());

        for (uint32_t i = 0; i < friendList.m_data.m_friends.m_count; i++) {
            BattlenetUI::AddToFriendList(friendList.m_data.m_friends.m_data[i]);
        }
    }

    if (args[2].ToBool() || !args[1].ToBool()) {
        BattlenetUI::DisambiguateFriendList();

        if (BattlenetUI::m_friends.Count() > 1) {
            qsort(BattlenetUI::m_friends.Ptr(), BattlenetUI::m_friends.Count(), sizeof(RIDFriendData), BattlenetUI::QSortFriendListCB);
        }

        BattlenetUI::m_friendListLoaded = true;

        for (uint32_t i = 0; i < BattlenetUI::m_friends.Count(); i++) {
            BattlenetUI::SendScriptEvent(SCRIPT_EVENT_BN_FRIEND_TOON_ONLINE, "%u", BattlenetUI::m_friends[i].m_presenceID);
            DebugLog("BNUI: >> TOON %s is now ONLINE. ID %u (friend list initialized)", BattlenetUI::GetTokenizedName(BattlenetUI::m_friends[i].m_presenceID), BattlenetUI::m_friends[i].m_presenceID);
        }

        BattlenetUI::PopulateAutoComplete();
        BattlenetUI::SendScriptEvent(SCRIPT_EVENT_BN_FRIEND_LIST_SIZE_CHANGED, NULL);
    }

    DebugLog("BNUI: Friend List Initialized. Num friends = %d", BattlenetUI::m_friends.Count());
}

// OFFSET: 0x0090D7E0
void BattlenetUI::FriendAddedCB(void* param, int32_t argc, const GeneralFunction::Arg* args) {
    Login* login = ClientServices::LoginConnection();

    if (!login || login->GetLoginServerType() != 1 || login->IsTrialAccount() || !BattlenetAPI::IsRIDEnabled()) {
        return;
    }

    uint32_t presenceId = static_cast<uint32_t>(args[0].ToUInt64());

    if (BattlenetAPI::IsRIDFriend(presenceId)) {
        BattlenetUI::AddToFriendList(presenceId);
        BattlenetUI::DisambiguateFriendList();

        if (BattlenetUI::m_friends.Count() > 1) {
            qsort(BattlenetUI::m_friends.Ptr(), BattlenetUI::m_friends.Count(), sizeof(RIDFriendData), BattlenetUI::QSortFriendListCB);
        }

        BattlenetUI::PopulateAutoComplete();
        DebugLog("BNUI: Friend Added. ID = %u", presenceId);
    } else {
        DebugLog("BNUI: Friend Add, %u not friend", presenceId);
    }

    BattlenetUI::SendScriptEvent(SCRIPT_EVENT_BN_FRIEND_LIST_SIZE_CHANGED, "%d", presenceId);

    ChatConversation conversation;
    conversation.m_presenceID = presenceId;
    conversation.m_inactive = 1;
    ChatMessageParams params;
    params.m_senderName = BattlenetUI::GetTokenizedName(presenceId);
    params.m_conversation = &conversation;
    CGChat::AddChatMessage("FRIEND_ADDED", CHAT_MSG_BN_INLINE_TOAST_ALERT, &params);
}

// OFFSET: 0x0090D910
void BattlenetUI::FriendRemovedCB(void* param, int32_t argc, const GeneralFunction::Arg* args) {
    Login* login = ClientServices::LoginConnection();

    if (!login || login->GetLoginServerType() != 1 || login->IsTrialAccount() || !BattlenetAPI::IsRIDEnabled()) {
        return;
    }

    uint32_t presenceId = static_cast<uint32_t>(args[0].ToUInt64());

    if (!BattlenetUI::RemoveFromFriendList(presenceId)) {
        return;
    }

    BattlenetUI::DisambiguateFriendList();

    if (BattlenetUI::m_friends.Count() > 1) {
        qsort(BattlenetUI::m_friends.Ptr(), BattlenetUI::m_friends.Count(), sizeof(RIDFriendData), BattlenetUI::QSortFriendListCB);
    }

    BattlenetUI::PopulateAutoComplete();
    CGAutoCompleteName::RemoveFlags(presenceId, 0x4);
    BattlenetUI::SendScriptEvent(SCRIPT_EVENT_BN_FRIEND_LIST_SIZE_CHANGED, "%d", presenceId);
}

// OFFSET: 0x0090D9C0
void BattlenetUI::ChatWhisperUndeliverableCB(void* param, int32_t argc, const GeneralFunction::Arg* args) {
    Login* login = ClientServices::LoginConnection();

    if (!login || login->GetLoginServerType() != 1 || login->IsTrialAccount() || !BattlenetAPI::IsRIDEnabled()) {
        return;
    }

    uint32_t presenceId = static_cast<uint32_t>(args[0].ToUInt64());
    uint16_t error = static_cast<uint16_t>(args[1].ToUInt64());
    BattlenetUI::DisplayError(error, presenceId);
    BattlenetUI::SendScriptEvent(SCRIPT_EVENT_BN_CHAT_WHISPER_UNDELIVERABLE, "%u", presenceId);
}

// OFFSET: 0x0090DA30
void BattlenetUI::ChatChannelCreationResultCB(void* param, int32_t argc, const GeneralFunction::Arg* args) {
    Login* login = ClientServices::LoginConnection();

    if (!login || login->GetLoginServerType() != 1 || login->IsTrialAccount() || !BattlenetAPI::IsRIDEnabled()) {
        return;
    }

    uint16_t error = static_cast<uint16_t>(args[0].ToUInt64());

    if (!error) {
        uint8_t channel = static_cast<uint8_t>(args[1].ToUInt64());

        BattlenetUI::SendScriptEvent(SCRIPT_EVENT_BN_CHAT_CHANNEL_CREATE_SUCCEEDED, "%d", channel + 1);
        DebugLog("BNUI: Chat Channel Create Succeeded. channel = %d", channel + 1);
        return;
    }

    uint32_t presenceId = 0;

    if (argc >= 2) {
        if (args[1].m_type == GeneralFunction::Arg::TYPE_OBJECT) {
            BSN::BSNEncodedObject<Battlenet::Toon::FullName, Protocol> toon;
            toon.WalkAndGet(args[1].ToObject());
            presenceId = BattlenetAPI::GetPresenceIDForToon(&toon.m_data);
        } else if (args[1].m_type == GeneralFunction::Arg::TYPE_INT || args[1].m_type == GeneralFunction::Arg::TYPE_UINT) {
            presenceId = static_cast<uint32_t>(args[1].ToUInt64());
        }
    }

    BattlenetUI::DisplayError(error, presenceId);
    BattlenetUI::SendScriptEvent(SCRIPT_EVENT_BN_CHAT_CHANNEL_CREATE_FAILED, "%u", presenceId);
}

// OFFSET: 0x0090DB60
void BattlenetUI::ChatChannelInviteResultCB(void* param, int32_t argc, const GeneralFunction::Arg* args) {
    Login* login = ClientServices::LoginConnection();

    if (!login || login->GetLoginServerType() != 1 || login->IsTrialAccount() || !BattlenetAPI::IsRIDEnabled()) {
        return;
    }

    uint16_t error = static_cast<uint16_t>(args[0].ToUInt64());
    BSN::BSNEncodedObject<Battlenet::Toon::FullName, Protocol> toon;
    toon.WalkAndGet(args[1].ToObject());
    uint32_t presenceId = BattlenetAPI::GetPresenceIDForToon(&toon.m_data);
    uint8_t channel = static_cast<uint8_t>(args[2].ToUInt64());

    if (!error) {
        BattlenetUI::SendScriptEvent(SCRIPT_EVENT_BN_CHAT_CHANNEL_INVITE_SUCCEEDED, "%d%u", channel + 1, presenceId);
        DebugLog("BNUI: Chat Channel Invite Succeeded. channel %d, id %u", channel + 1, presenceId);
    } else {
        BattlenetUI::DisplayError(error, presenceId);
        BattlenetUI::SendScriptEvent(SCRIPT_EVENT_BN_CHAT_CHANNEL_INVITE_FAILED, "%d%u", channel + 1, presenceId);
        DebugLog("BNUI: Chat Channel Invite Failed. channel %d, id %d", channel + 1, presenceId);
    }
}

// OFFSET: 0x0090DCB0
void BattlenetUI::ChatChannelMessageUndeliverableCB(void* param, int32_t argc, const GeneralFunction::Arg* args) {
    Login* login = ClientServices::LoginConnection();

    if (!login || login->GetLoginServerType() != 1 || login->IsTrialAccount() || !BattlenetAPI::IsRIDEnabled()) {
        return;
    }

    uint8_t channel = static_cast<uint8_t>(args[0].ToUInt64());
    uint16_t error = static_cast<uint16_t>(args[1].ToUInt64());

    BattlenetUI::DisplayError(error, 0);
    BattlenetUI::SendScriptEvent(SCRIPT_EVENT_BN_CHAT_CHANNEL_MESSAGE_UNDELIVERABLE, "%d", channel + 1);
    DebugLog("BNUI: Chat Channel Message Undeliverable. channel %d", channel + 1);
}

// OFFSET: 0x0090DD40
void BattlenetUI::Init() {
    if (!g_tumorManager) {
        return;
    }

    BattlenetUI::m_friends.Clear();
    BattlenetUI::m_friendInvites.Clear();
    BattlenetUI::s_friendsOfFriend.Clear();

    for (uint32_t i = 0; i < 26; i++) {
        GeneralFunction::SimpleCallback* callback = new (__FILE__, __LINE__) GeneralFunction::SimpleCallback(BattlenetUI::s_eventCallbacks[i].m_func, NULL);
        BattlenetUI::s_eventCallbacks[i].m_handle = BattlenetAPI::RegisterEvent(BattlenetUI::s_eventCallbacks[i].m_event, callback);
    }

    for (uint32_t i = 0; i < NUM_PRESENCE_FIELDS; i++) {
        GeneralFunction::SimpleCallback* callback = new (__FILE__, __LINE__) GeneralFunction::SimpleCallback(BattlenetUI::s_presenceCallbacks[i].m_func, NULL);
        BattlenetUI::s_presenceCallbacks[i].m_handle = BattlenetAPI::RegisterPresenceUpdateCallback(0, BattlenetUI::s_presenceCallbacks[i].m_field, callback);
    }

    BattlenetUI::s_createConversationCB = new (__FILE__, __LINE__) GeneralFunction::SimpleCallback(BattlenetUI::ChatChannelCreationResultCB, NULL);
    BattlenetUI::s_createConversationCB->IncRef();

    BattlenetUI::s_inviteToConversationCB = new (__FILE__, __LINE__) GeneralFunction::SimpleCallback(BattlenetUI::ChatChannelInviteResultCB, NULL);
    BattlenetUI::s_inviteToConversationCB->IncRef();

    BattlenetUI::s_friendsOfFriendCB = new (__FILE__, __LINE__) GeneralFunction::SimpleCallback(BattlenetUI::RequestFofInfoResultCB, NULL);
    BattlenetUI::s_friendsOfFriendCB->IncRef();

    GxuFontSetTokenCallback(BattlenetUI::GetKStringText);

    BattlenetUI::s_initialized = true;
    DebugLog("BNUI: Init");
}

// OFFSET: 0x0090DF70
void BattlenetUI::EnterRealm() {
    Login* login = ClientServices::LoginConnection();

    if (!login || login->GetLoginServerType() != 1 || login->IsTrialAccount() || !BattlenetAPI::IsRIDEnabled() || !static_cast<BattlenetLogin*>(login)->IsConnected()) {
        return;
    }

    BattlenetUI::PopulateAutoComplete();

    const char* nameFormat = FrameScript_GetText("BATTLENET_NAME_FORMAT", -1, GENDER_NOT_APPLICABLE);

    if (*nameFormat) {
        BattlenetAPI::SetAccountNameFormatString(nameFormat);
    }
}

// OFFSET: 0x0090DFE0
void BattlenetUI_EnterRealm() {
    BattlenetUI::EnterRealm();
}

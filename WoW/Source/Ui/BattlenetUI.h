#ifndef WOW_SOURCE_UI_BATTLENETUI_H
#define WOW_SOURCE_UI_BATTLENETUI_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Storm/H/stpl.h"
#include "WoW/Source/Battlenet/BattlenetProtocol.h"
#include "WoW/Source/Battlenet/GeneralFunction.h"
#include "WoW/Source/Ui/BattlenetAPI.h"
#include "WoW/Source/Ui/Types.h"

struct lua_State;

namespace Battlenet {
namespace InviteSource {
extern BSN::FourCC MISC;
}
}

namespace BattlenetAPI {
void GetChatMemberList(uint8_t channel, GeneralFunction::Callback* callback);
uint32_t GetPresenceIDForAccount(uint32_t accountId);
uint16_t SendRIDFriendInviteByEmail(const char* email, const char* note, BSN::FourCC source, GeneralFunction::Callback* callback);
uint16_t SendRIDFriendInviteByPresenceId(uint32_t presenceId, const char* note, BSN::FourCC source, GeneralFunction::Callback* callback);
}

struct ACCOUNTIDCACHE : public TSHashObject<ACCOUNTIDCACHE, HASHKEY_DWORD> {
    /* 0018 */ uint32_t m_accountId;
};

class BattlenetUI {
    public:
    enum {
        PRESENCE_FIELD_FLAGS = 20,
        PRESENCE_FIELD_FACTION = 21,
        PRESENCE_FIELD_REALM_ID = 22,
        NUM_PRESENCE_FIELDS = 23,
    };

    struct RIDFriendData {
        /* 0000 */ uint32_t m_presenceID;
        /* 0004 */ uint32_t m_accountId;
        /* 0008 */ BSN::String<72> m_name;
        /* 0134 */ BSN::String<40> m_givenName;
        /* 01E0 */ BSN::String<40> m_surname;
        /* 028C */ BSN::String<40> m_sortSurname;
    };

    struct EventCallback {
        /* 0000 */ const char* m_event;
        /* 0004 */ GeneralFunction::SimpleCallbackFunc m_func;
        /* 0008 */ uint32_t m_handle;
    };

    struct PresenceCallback {
        /* 0000 */ uint32_t m_field;
        /* 0004 */ GeneralFunction::SimpleCallbackFunc m_func;
        /* 0008 */ uint32_t m_handle;
    };

    class FriendInviteSentCB : public GeneralFunction::Callback {
        public:
        void Call(int32_t argc, const GeneralFunction::Arg* args) const override;
        void Destroy() const override;
    };

    class MemberListCB : public GeneralFunction::Callback {
        public:
        /* 0008 */ TSGrowableArray<uint32_t>* m_members;

        ~MemberListCB();
        void Call(int32_t argc, const GeneralFunction::Arg* args) const override;
        void Destroy() const override;

        MemberListCB();
    };

    static EventCallback s_eventCallbacks[26];
    static PresenceCallback s_presenceCallbacks[NUM_PRESENCE_FIELDS];
    static FriendInviteSentCB s_friendInviteSentCB;
    static int32_t s_broadcastCount;
    static uint32_t s_selfRealm;
    static bool s_initialized;
    static bool m_friendListLoaded;
    static GeneralFunction::Callback* s_createConversationCB;
    static GeneralFunction::Callback* s_inviteToConversationCB;
    static GeneralFunction::Callback* s_friendsOfFriendCB;
    static uint32_t s_selectedFriend;
    static uint32_t s_selectedBlock;
    static uint32_t s_selectedToonBlock;
    static uint32_t s_accountIdCacheLowest;
    static uint32_t s_accountIdCacheCount;
    static TSGrowableArray<uint32_t> m_friendInvites;
    static TSGrowableArray<uint32_t> s_friendsOfFriend;
    static TSGrowableArray<RIDFriendData> m_friends;
    static TSHashTable<ACCOUNTIDCACHE, HASHKEY_DWORD> s_accountIdCache;
    static char s_programString[16];
    static float s_broadcastTime;
    static char s_displayName[72];
    static char s_tokenName[296];
    static char s_tokenSurname[168];
    static char s_tokenGivenName[168];

    static bool AddToFriendList(uint32_t presenceId);
    static int32_t BNGetFriendInfoHelper(lua_State* L, uint32_t index);
    static int32_t BNGetToonInfoHelper(lua_State* L, uint32_t presenceId);
    static void BlockAddedCB(void* param, int32_t argc, const GeneralFunction::Arg* args);
    static void BlockListInitializedCB(void* param, int32_t argc, const GeneralFunction::Arg* args);
    static void BlockRemovedCB(void* param, int32_t argc, const GeneralFunction::Arg* args);
    static void ChatChannelClosedCB(void* param, int32_t argc, const GeneralFunction::Arg* args);
    static void ChatChannelCreationResultCB(void* param, int32_t argc, const GeneralFunction::Arg* args);
    static void ChatChannelInviteResultCB(void* param, int32_t argc, const GeneralFunction::Arg* args);
    static void ChatChannelJoinedCB(void* param, int32_t argc, const GeneralFunction::Arg* args);
    static void ChatChannelLeftCB(void* param, int32_t argc, const GeneralFunction::Arg* args);
    static void ChatChannelMemberJoinedCB(void* param, int32_t argc, const GeneralFunction::Arg* args);
    static void ChatChannelMemberLeftCB(void* param, int32_t argc, const GeneralFunction::Arg* args);
    static void ChatChannelMemberUpdatedCB(void* param, int32_t argc, const GeneralFunction::Arg* args);
    static void ChatChannelMessageBlockedCB(void* param, int32_t argc, const GeneralFunction::Arg* args);
    static void ChatChannelMessageReceivedCB(void* param, int32_t argc, const GeneralFunction::Arg* args);
    static void ChatChannelMessageUndeliverableCB(void* param, int32_t argc, const GeneralFunction::Arg* args);
    static void ChatWhisperReceivedCB(void* param, int32_t argc, const GeneralFunction::Arg* args);
    static void ChatWhisperSentCB(void* param, int32_t argc, const GeneralFunction::Arg* args);
    static void ChatWhisperUndeliverableCB(void* param, int32_t argc, const GeneralFunction::Arg* args);
    static void CustomMessageChangedCB(void* param, int32_t argc, const GeneralFunction::Arg* args);
    static void DisambiguateFriendList();
    static void DisplayError(uint16_t error, uint32_t presenceId);
    static void EnterRealm();
    static void FactionUpdateCB(void* param, int32_t argc, const GeneralFunction::Arg* args);
    static bool FeaturesEnabledAndConnected();
    static RIDFriendData* FindFriend(uint32_t presenceId);
    static int32_t FindFriendIndex(uint32_t presenceId);
    static void FriendAddedCB(void* param, int32_t argc, const GeneralFunction::Arg* args);
    static void FriendInviteAddedCB(void* param, int32_t argc, const GeneralFunction::Arg* args);
    static void FriendInviteListInitializedCB(void* param, int32_t argc, const GeneralFunction::Arg* args);
    static void FriendInviteRemovedCB(void* param, int32_t argc, const GeneralFunction::Arg* args);
    static void FriendListInitializedCB(void* param, int32_t argc, const GeneralFunction::Arg* args);
    static void FriendRemovedCB(void* param, int32_t argc, const GeneralFunction::Arg* args);
    static ACCOUNTIDCACHE* GetAccountIDCache(uint32_t presenceId);
    static const char* GetKStringText(uint32_t kind, uint32_t presenceId);
    static const char* GetNameForPresenceIdHelper(uint32_t presenceId);
    static const char* GetProgramString(BSN::FourCC program);
    static uint32_t GetSelfRealmID();
    static const char* GetTokenizedName(uint32_t presenceId);
    static bool GetToonInviteInfo(uint32_t toonId, uint32_t& realmId, uint8_t* guid, char* name, uint32_t nameSize, char* realm, uint32_t realmSize);
    static void Init();
    static bool IsToonFriend(uint32_t accountId);
    static void LastOnlineChangedCB(void* param, int32_t argc, const GeneralFunction::Arg* args);
    static void LeaveRealm();
    static void NewPresenceCB(void* param, int32_t argc, const GeneralFunction::Arg* args);
    static void NoActionCB(void* param, int32_t argc, const GeneralFunction::Arg* args);
    static void OnlinePresenceChangedCB(void* param, int32_t argc, const GeneralFunction::Arg* args);
    static void OnlineTimeChangedCB(void* param, int32_t argc, const GeneralFunction::Arg* args);
    static void PopulateAutoComplete();
    static void PresenceUpdateCB(void* param, int32_t argc, const GeneralFunction::Arg* args);
    static int32_t QSortDisambiguateFriendListCB(const void* a, const void* b);
    static int32_t QSortFriendInviteListByTimeCB(const void* a, const void* b);
    static int32_t QSortFriendListCB(const void* a, const void* b);
    static bool RemoveFromFriendList(uint32_t presenceId);
    static void RequestFofInfoResultCB(void* param, int32_t argc, const GeneralFunction::Arg* args);
    static void SavePrescenceID(uint32_t presenceId);
    static void SelfToonOfflineCB(void* param, int32_t argc, const GeneralFunction::Arg* args);
    static void SelfToonOnlineCB(void* param, int32_t argc, const GeneralFunction::Arg* args);
    static void SendScriptEvent(uint32_t event, const char* format, ...);
    static void SetBattlenetConnectionRecovered();
    static void SetDisconnectedFromBattlenet(bool disconnected);
    static void SettingChangedCB(void* param, int32_t argc, const GeneralFunction::Arg* args);
    static void SystemMessageCB(void* param, int32_t argc, const GeneralFunction::Arg* args);
    static bool TokenizeGivenAndSurname(uint32_t presenceId, const char*& givenName, const char*& surname);
    static bool TokenizeName(uint32_t presenceId, const char*& name);
    static void ToonNameUpdateCB(void* param, int32_t argc, const GeneralFunction::Arg* args);
    static void Uninit();
    static int32_t UpdateAutoComplete(uint32_t& presenceId, uint32_t flags, int32_t add, const char* name);
};

char* FilterChatLinks(const char* source, char* dest, uint32_t size);
void BattlenetUI_EnterRealm();
void BattlenetUI_LeaveRealm();
void BattlenetUI_RegisterScriptFunctions();
void BattlenetUI_UnregisterScriptFunctions();

#endif

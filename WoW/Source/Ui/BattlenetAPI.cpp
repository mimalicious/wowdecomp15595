#include "BattlenetAPI.h"
#include "WoW/Source/Battlenet/Tumor/TumorManager.h"
#include "WoW/Source/WowServices/BattlenetLogin.h"

// OFFSET: 0x009026D0/0x14065AD00
uint16_t BattlenetAPI::SendChatWhisper(uint32_t presenceId, const char* message) {
    GeneralFunction::Arg args[2];
    args[0] = static_cast<uint64_t>(presenceId);
    args[1] = message;
    return static_cast<uint16_t>(g_tumorManager->CallFunction("BattlenetAPI_SendChatWhisper", 2, args).ToUInt64());
}

// OFFSET: 0x00902790/0x14065ADE0
uint16_t BattlenetAPI::LeaveChatConversation(uint8_t channel) {
    GeneralFunction::Arg arg;
    arg = static_cast<uint64_t>(channel);
    return static_cast<uint16_t>(g_tumorManager->CallFunction("BattlenetAPI_LeaveChatConversation", 1, &arg).ToUInt64());
}

// OFFSET: 0x00902820/0x14065AE90
uint16_t BattlenetAPI::SendChatMessage(uint8_t channel, const char* message) {
    GeneralFunction::Arg args[2];
    args[0] = static_cast<uint64_t>(channel);
    args[1] = message;
    return static_cast<uint16_t>(g_tumorManager->CallFunction("BattlenetAPI_SendChatMessage", 2, args).ToUInt64());
}

// OFFSET: 0x009028E0/0x14065AF70
uint32_t BattlenetAPI::GetNumChatMembers(uint8_t channel) {
    GeneralFunction::Arg arg;
    arg = static_cast<uint64_t>(channel);
    return static_cast<uint32_t>(g_tumorManager->CallFunction("BattlenetAPI_GetNumChatMembers", 1, &arg).ToUInt64());
}

// OFFSET: 0x00902970/0x14065B020
void BattlenetAPI::GetChatMemberList(uint8_t channel, GeneralFunction::Callback* callback) {
    GeneralFunction::Arg args[2];
    args[0] = static_cast<uint64_t>(channel);
    args[1] = callback;
    g_tumorManager->CallFunction("BattlenetAPI_GetChatMemberList", 2, args);
}

// OFFSET: 0x00902A20/0x14065B0F0
uint16_t BattlenetAPI::SetFriendNote(uint32_t presenceId, const char* note) {
    GeneralFunction::Arg args[2];
    args[0] = static_cast<uint64_t>(presenceId);
    args[1] = note;
    return static_cast<uint16_t>(g_tumorManager->CallFunction("BattlenetAPI_SetFriendNote", 2, args).ToUInt64());
}

// OFFSET: 0x00902AE0/0x14065B1D0
uint16_t BattlenetAPI::SendRIDFriendInviteByEmail(const char* email, const char* note, BSN::FourCC source, GeneralFunction::Callback* callback) {
    GeneralFunction::Arg args[4];
    args[0] = email;
    args[1] = note;
    args[2] = source;
    args[3] = callback;
    return static_cast<uint16_t>(g_tumorManager->CallFunction("BattlenetAPI_SendRIDFriendInviteByEmail", 4, args).ToUInt64());
}

// OFFSET: 0x00902BE0/0x14065B340
uint16_t BattlenetAPI::SendRIDFriendInviteByPresenceId(uint32_t presenceId, const char* note, BSN::FourCC source, GeneralFunction::Callback* callback) {
    GeneralFunction::Arg args[4];
    args[0] = static_cast<uint64_t>(presenceId);
    args[1] = note;
    args[2] = source;
    args[3] = callback;
    return static_cast<uint16_t>(g_tumorManager->CallFunction("BattlenetAPI_SendRIDFriendInviteByPresenceId", 4, args).ToUInt64());
}

// OFFSET: 0x00902CD0/0x14065B4B0
uint16_t BattlenetAPI::RemoveFriend(uint32_t presenceId) {
    GeneralFunction::Arg arg;
    arg = static_cast<uint64_t>(presenceId);
    return static_cast<uint16_t>(g_tumorManager->CallFunction("BattlenetAPI_RemoveFriend", 1, &arg).ToUInt64());
}

// OFFSET: 0x00902D60/0x14065B560
uint16_t BattlenetAPI::AcceptFriendInvite(uint32_t presenceId) {
    GeneralFunction::Arg arg;
    arg = static_cast<uint64_t>(presenceId);
    return static_cast<uint16_t>(g_tumorManager->CallFunction("BattlenetAPI_AcceptFriendInvite", 1, &arg).ToUInt64());
}

// OFFSET: 0x00902DF0/0x14065B610
uint16_t BattlenetAPI::DeclineFriendInvite(uint32_t presenceId) {
    GeneralFunction::Arg arg;
    arg = static_cast<uint64_t>(presenceId);
    return static_cast<uint16_t>(g_tumorManager->CallFunction("BattlenetAPI_DeclineFriendInvite", 1, &arg).ToUInt64());
}

// OFFSET: 0x00902E80/0x14065B6C0
uint16_t BattlenetAPI::ReportFriendInvite(uint32_t presenceId) {
    GeneralFunction::Arg arg;
    arg = static_cast<uint64_t>(presenceId);
    return static_cast<uint16_t>(g_tumorManager->CallFunction("BattlenetAPI_ReportFriendInvite", 1, &arg).ToUInt64());
}

// OFFSET: 0x00902F10/0x14065B770
uint16_t BattlenetAPI::GetFriendsOfFriend(uint32_t presenceId, GeneralFunction::Callback* callback) {
    GeneralFunction::Arg args[2];
    args[0] = static_cast<uint64_t>(presenceId);
    args[1] = callback;
    return static_cast<uint16_t>(g_tumorManager->CallFunction("BattlenetAPI_GetFriendsOfFriend", 2, args).ToUInt64());
}

// OFFSET: 0x00902FD0/0x14065B860
uint32_t BattlenetAPI::RegisterPresenceUpdateCallback(uint32_t presenceId, uint32_t field, GeneralFunction::Callback* callback) {
    GeneralFunction::Arg args[3];
    args[0] = static_cast<uint64_t>(presenceId);
    args[1] = static_cast<uint64_t>(field);
    args[2] = callback;
    return static_cast<uint32_t>(g_tumorManager->CallFunction("BattlenetAPI_RegisterPresenceUpdateCallback", 3, args).ToUInt64());
}

// OFFSET: 0x009030A0/0x14065B960
uint32_t BattlenetAPI::GetPresenceIDForAccount(uint32_t accountId) {
    GeneralFunction::Arg arg;
    arg = static_cast<uint64_t>(accountId);
    return static_cast<uint32_t>(g_tumorManager->CallFunction("BattlenetAPI_GetPresenceIDForAccount", 1, &arg).ToUInt64());
}

// PROVEN: 658b7fdc IDENTICAL/IDENTICAL
// OFFSET: 0x00903130/0x14065BA10
uint32_t BattlenetAPI::GetPresenceIDForCurrentAccount() {
    return static_cast<uint32_t>(g_tumorManager->CallFunction("BattlenetAPI_GetPresenceIDForCurrentAccount", 0, NULL).ToUInt64());
}

// PROVEN: 6a9eaf9e IDENTICAL/IDENTICAL
// OFFSET: 0x00903190/0x14065BA80
uint32_t BattlenetAPI::GetPresenceIDForCurrentToon() {
    return static_cast<uint32_t>(g_tumorManager->CallFunction("BattlenetAPI_GetPresenceIDForCurrentToon", 0, NULL).ToUInt64());
}

// OFFSET: 0x009031F0/0x14065BAF0
uint32_t BattlenetAPI::GetAccountPresenceId(uint32_t presenceId) {
    GeneralFunction::Arg arg;
    arg = static_cast<uint64_t>(presenceId);
    return static_cast<uint32_t>(g_tumorManager->CallFunction("BattlenetAPI_GetAccountPresenceId", 1, &arg).ToUInt64());
}

// OFFSET: 0x00903280/0x14065BBA0
const char* BattlenetAPI::GetNameForPresenceId(uint32_t presenceId, bool full) {
    GeneralFunction::Arg args[2];
    args[0] = static_cast<uint64_t>(presenceId);
    args[1] = full;
    return g_tumorManager->CallFunction("BattlenetAPI_GetNameForPresenceId", 2, args).ToString();
}

// OFFSET: 0x00903340/0x14065BC90
const char* BattlenetAPI::GetExactNameForPresenceId(uint32_t presenceId, bool full) {
    GeneralFunction::Arg args[2];
    args[0] = static_cast<uint64_t>(presenceId);
    args[1] = full;
    return g_tumorManager->CallFunction("BattlenetAPI_GetExactNameForPresenceId", 2, args).ToString();
}

// OFFSET: 0x00903400/0x14065BD80
const char* BattlenetAPI::GetToonNameForPresenceId(uint32_t presenceId, bool full) {
    GeneralFunction::Arg args[2];
    args[0] = static_cast<uint64_t>(presenceId);
    args[1] = full;
    return g_tumorManager->CallFunction("BattlenetAPI_GetToonNameForPresenceId", 2, args).ToString();
}

// OFFSET: 0x009034C0/0x14065BE70
uint32_t BattlenetAPI::GetNumToons(uint32_t presenceId) {
    GeneralFunction::Arg arg;
    arg = static_cast<uint64_t>(presenceId);
    return static_cast<uint32_t>(g_tumorManager->CallFunction("BattlenetAPI_GetNumToons", 1, &arg).ToUInt64());
}

// OFFSET: 0x00903550/0x14065BF20
uint32_t BattlenetAPI::GetToon(uint32_t presenceId, uint32_t index) {
    GeneralFunction::Arg args[2];
    args[0] = static_cast<uint64_t>(presenceId);
    args[1] = static_cast<uint64_t>(index);
    return static_cast<uint32_t>(g_tumorManager->CallFunction("BattlenetAPI_GetToon", 2, args).ToUInt64());
}

// OFFSET: 0x00903600/0x14065C000
uint32_t BattlenetAPI::GetFocusedToon(uint32_t presenceId) {
    GeneralFunction::Arg arg;
    arg = static_cast<uint64_t>(presenceId);
    return static_cast<uint32_t>(g_tumorManager->CallFunction("BattlenetAPI_GetFocusedToon", 1, &arg).ToUInt64());
}

// OFFSET: 0x00903690/0x14065C0B0
uint16_t BattlenetAPI::AddRIDBlock(uint32_t presenceId) {
    GeneralFunction::Arg arg;
    arg = static_cast<uint64_t>(presenceId);
    return static_cast<uint16_t>(g_tumorManager->CallFunction("BattlenetAPI_AddRIDBlock", 1, &arg).ToUInt64());
}

// OFFSET: 0x00903720/0x14065C160
uint16_t BattlenetAPI::AddCIDBlock(uint32_t presenceId) {
    GeneralFunction::Arg arg;
    arg = static_cast<uint64_t>(presenceId);
    return static_cast<uint16_t>(g_tumorManager->CallFunction("BattlenetAPI_AddCIDBlock", 1, &arg).ToUInt64());
}

// OFFSET: 0x009037B0/0x14065C210
uint16_t BattlenetAPI::RemoveRIDBlock(uint32_t presenceId) {
    GeneralFunction::Arg arg;
    arg = static_cast<uint64_t>(presenceId);
    return static_cast<uint16_t>(g_tumorManager->CallFunction("BattlenetAPI_RemoveRIDBlock", 1, &arg).ToUInt64());
}

// OFFSET: 0x00903840/0x14065C2C0
uint16_t BattlenetAPI::RemoveCIDBlock(uint32_t presenceId) {
    GeneralFunction::Arg arg;
    arg = static_cast<uint64_t>(presenceId);
    return static_cast<uint16_t>(g_tumorManager->CallFunction("BattlenetAPI_RemoveCIDBlock", 1, &arg).ToUInt64());
}

// OFFSET: 0x009038D0/0x14065C370
uint16_t BattlenetAPI::ReportPlayer(uint32_t presenceId, int32_t reason, const char* note) {
    GeneralFunction::Arg args[3];
    args[0] = static_cast<uint64_t>(presenceId);
    args[1] = static_cast<int64_t>(reason);
    args[2] = note;
    return static_cast<uint16_t>(g_tumorManager->CallFunction("BattlenetAPI_ReportPlayer", 3, args).ToUInt64());
}

// PROVEN: e80009bc IDENTICAL/IDENTICAL
// OFFSET: 0x00903990/0x14065C460
void BattlenetAPI::SetSetting(const char* name, GeneralFunction::Arg value, bool notify) {
    GeneralFunction::Arg args[3];
    args[0] = name;
    args[1] = value;
    args[2] = notify;
    g_tumorManager->CallFunction("BattlenetAPI_SetSetting", 3, args);
}

// OFFSET: 0x00903A90/0x14065C590
const char* BattlenetAPI::GetErrorString(uint16_t error) {
    GeneralFunction::Arg arg;
    arg = static_cast<uint64_t>(error);
    return g_tumorManager->CallFunction("BattlenetAPI_GetErrorString", 1, &arg).ToString();
}

// OFFSET: 0x00903B30/0x14065C650
uint32_t BattlenetAPI::RegisterEvent(const char* name, GeneralFunction::Callback* callback) {
    GeneralFunction::Arg args[2];
    args[0] = name;
    args[1] = callback;
    return static_cast<uint32_t>(g_tumorManager->CallFunction("BattlenetAPI_RegisterEvent", 2, args).ToUInt64());
}

// PROVEN: 0e81944a IDENTICAL/IDENTICAL
// OFFSET: 0x00903BE0/0x14065C740
void BattlenetAPI::SetAccountNameFormatString(const char* format) {
    GeneralFunction::Arg arg;
    arg = format;
    g_tumorManager->CallFunction("BattlenetAPI_SetAccountNameFormatString", 1, &arg);
}

// OFFSET: 0x009043A0/0x14065CBA0
uint16_t BattlenetAPI::CreateChatConversation(const Battlenet::Toon::FullName* first, const Battlenet::Toon::FullName* second, GeneralFunction::Callback* callback) {
    GeneralFunction::Arg args[3];

    BSN::BSNEncodedObjectPtr<Battlenet::Toon::FullName, Protocol, 1> firstObject(first);
    firstObject.IncRef();
    args[0] = &firstObject;

    BSN::BSNEncodedObjectPtr<Battlenet::Toon::FullName, Protocol, 1> secondObject(second);
    secondObject.IncRef();
    args[1] = &secondObject;

    args[2] = callback;
    return static_cast<uint16_t>(g_tumorManager->CallFunction("BattlenetAPI_CreateChatConversation", 3, args).ToUInt64());
}

// OFFSET: 0x00904560/0x14065CE30
uint16_t BattlenetAPI::SendChatConversationInvite(uint8_t channel, const Battlenet::Toon::FullName* toon, GeneralFunction::Callback* callback) {
    GeneralFunction::Arg args[3];
    args[0] = static_cast<uint64_t>(channel);

    BSN::BSNEncodedObjectPtr<Battlenet::Toon::FullName, Protocol, 1> toonObject(toon);
    toonObject.IncRef();
    args[1] = &toonObject;

    args[2] = callback;
    return static_cast<uint16_t>(g_tumorManager->CallFunction("BattlenetAPI_SendChatConversationInvite", 3, args).ToUInt64());
}

// OFFSET: 0x009046C0/0x14065D010
int32_t BattlenetAPI::GetChatChannelType(uint8_t channel) {
    GeneralFunction::Arg arg;
    arg = static_cast<uint64_t>(channel);
    return static_cast<int32_t>(g_tumorManager->CallFunction("BattlenetAPI_GetChatChannelType", 1, &arg).ToInt64());
}

// OFFSET: 0x00904750/0x14065D0C0
bool BattlenetAPI::IsRIDFriend(uint32_t presenceId) {
    GeneralFunction::Arg arg;
    arg = static_cast<uint64_t>(presenceId);
    return g_tumorManager->CallFunction("BattlenetAPI_IsRIDFriend", 1, &arg).ToBool();
}

// OFFSET: 0x009047F0/0x14065D170
bool BattlenetAPI::IsCIDFriend(uint32_t presenceId) {
    GeneralFunction::Arg arg;
    arg = static_cast<uint64_t>(presenceId);
    return g_tumorManager->CallFunction("BattlenetAPI_IsCIDFriend", 1, &arg).ToBool();
}

// OFFSET: 0x00904890/0x14065D220
bool BattlenetAPI::HasRIDFriendship(uint32_t presenceId) {
    GeneralFunction::Arg arg;
    arg = static_cast<uint64_t>(presenceId);
    return g_tumorManager->CallFunction("BattlenetAPI_HasRIDFriendship", 1, &arg).ToBool();
}

// OFFSET: 0x00904930/0x14065D2D0
Battlenet::Friends::FriendInviteContainer BattlenetAPI::GetFriendInvite(uint32_t presenceId) {
    GeneralFunction::Arg arg;
    arg = static_cast<uint64_t>(presenceId);
    BSN::BSNEncodedObject<Battlenet::Friends::FriendInviteContainer, Protocol> invite(g_tumorManager->CallFunction("BattlenetAPI_GetFriendInvite", 1, &arg).ToObject());
    return invite.m_data;
}

// OFFSET: 0x00904A30/0x14065D480
bool BattlenetAPI::UnregisterPresenceUpdateCallback(uint32_t handle) {
    GeneralFunction::Arg arg;
    arg = static_cast<uint64_t>(handle);
    return g_tumorManager->CallFunction("BattlenetAPI_UnregisterPresenceUpdateCallback", 1, &arg).ToBool();
}

// OFFSET: 0x00904AD0/0x14065D530
bool BattlenetAPI::SetPresenceValue(uint32_t field, const Battlenet::Presence::FieldVal* value) {
    GeneralFunction::Arg args[2];
    args[0] = static_cast<uint64_t>(field);

    BSN::BSNEncodedObjectPtr<Battlenet::Presence::FieldVal, Protocol, 1> valueObject(value);
    valueObject.IncRef();
    args[1] = &valueObject;

    return g_tumorManager->CallFunction("BattlenetAPI_SetPresenceValue", 2, args).ToBool();
}

// OFFSET: 0x00904C00/0x14065D6C0
Battlenet::Presence::FieldVal BattlenetAPI::GetPresenceValue(uint32_t presenceId, uint32_t field) {
    GeneralFunction::Arg args[2];
    args[0] = static_cast<uint64_t>(presenceId);
    args[1] = static_cast<uint64_t>(field);
    BSN::BSNEncodedObject<Battlenet::Presence::FieldVal, Protocol> value(g_tumorManager->CallFunction("BattlenetAPI_GetPresenceValue", 2, args).ToObject());
    return value.m_data;
}

// OFFSET: 0x00904D40/0x14065D8E0
uint32_t BattlenetAPI::GetPresenceIDForToon(const Battlenet::Toon::FullName* toon) {
    GeneralFunction::Arg arg;

    BSN::BSNEncodedObjectPtr<Battlenet::Toon::FullName, Protocol, 1> toonObject(toon);
    toonObject.IncRef();
    arg = &toonObject;

    return static_cast<uint32_t>(g_tumorManager->CallFunction("BattlenetAPI_GetPresenceIDForToon", 1, &arg).ToUInt64());
}

// OFFSET: 0x00904E40/0x14065DA30
bool BattlenetAPI::IsSelf(uint32_t presenceId) {
    GeneralFunction::Arg arg;
    arg = static_cast<uint64_t>(presenceId);
    return g_tumorManager->CallFunction("BattlenetAPI_IsSelf", 1, &arg).ToBool();
}

// OFFSET: 0x00904EE0/0x14065DAE0
Battlenet::Presence::BlockList BattlenetAPI::GetBlockList() {
    BSN::BSNEncodedObject<Battlenet::Presence::BlockList, Protocol> blockList(g_tumorManager->CallFunction("BattlenetAPI_GetBlockList", 0, NULL).ToObject());
    return blockList.m_data;
}

// OFFSET: 0x00904F60/0x14065DBF0
bool BattlenetAPI::IsRIDBlock(uint32_t presenceId) {
    GeneralFunction::Arg arg;
    arg = static_cast<uint64_t>(presenceId);
    return g_tumorManager->CallFunction("BattlenetAPI_IsRIDBlock", 1, &arg).ToBool();
}

// OFFSET: 0x00905000/0x14065DCA0
bool BattlenetAPI::IsCIDBlock(uint32_t presenceId) {
    GeneralFunction::Arg arg;
    arg = static_cast<uint64_t>(presenceId);
    return g_tumorManager->CallFunction("BattlenetAPI_IsCIDBlock", 1, &arg).ToBool();
}

// PROVEN: ec7d80e3 IDENTICAL/NONE
// OFFSET: 0x009050A0
GeneralFunction::Arg BattlenetAPI::GetSetting(const char* name) {
    GeneralFunction::Arg arg;
    arg = name;
    return g_tumorManager->CallFunction("BattlenetAPI_GetSetting", 1, &arg);
}

// PROVEN: 7c2007ec IDENTICAL/IDENTICAL
// OFFSET: 0x00905100/0x14065DD50
bool BattlenetAPI::IsRIDEnabled() {
    return g_tumorManager->CallFunction("BattlenetAPI_IsRIDEnabled", 0, NULL).ToBool();
}

// OFFSET: 0x00905160/0x14065DDD0
bool BattlenetAPI::UnregisterEvent(uint32_t handle) {
    GeneralFunction::Arg arg;
    arg = static_cast<uint64_t>(handle);
    return g_tumorManager->CallFunction("BattlenetAPI_UnregisterEvent", 1, &arg).ToBool();
}

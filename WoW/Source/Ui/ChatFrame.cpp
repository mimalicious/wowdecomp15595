#include "ChatFrame.h"
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <malloc.h>
#include "Engine/Source/Base/CDataStore.h"
#include "Engine/Source/Base/ConvertUTF.h"
#include "Engine/Source/ConsoleBase/ConsoleVar.h"
#include "Engine/Source/Frame/CSimpleRender.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameXML/LoadXML.h"
#include "Engine/Source/Os/OsFile.h"
#include "Engine/Source/Os/TimeManager.h"
#include "Engine/Source/Services/SysMessage.h"
#include "WoW/Source/JamClientAutoCode/JamUserClient.h"
#include "WoW/Source/Object/ObjectClient/GuildStats_C.h"
#include "WoW/Source/Ui/ReputationInfo.h"
#include "WoW/Source/JamClientAutoCode/JamClient.h"
#include "WoW/Source/DB/Rec/ItemRecSparse.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/Tempest/CRandom.h"
#include "Engine/Source/pcre/pcreposix.h"
#include "WoW/Source/Client.h"
#include "WoW/Source/DB/DBClient/DBCacheInstances.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/ChatChannelsRec.h"
#include "WoW/Source/Game/GameClient/ComSatClient.h"
#include "WoW/Source/Game/ValidateName.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/Object/ObjectClient/UnitCombatLog_C.h"
#include "WoW/Source/Object/ObjectClient/Unit_C.h"
#include "WoW/Source/Object/Types.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/UIUtil/Tooltip.h"
#include "WoW/Source/Ui/CGAutoCompleteName.h"
#include "WoW/Source/Ui/CKBSystem.h"
#include "WoW/Source/Ui/GameUI.h"
#include "WoW/Source/Ui/RaidInfo.h"
#include "WoW/Source/Ui/Tutorial.h"
#include "WoW/Source/Ui/ScriptEvents.h"
#include "WoW/Source/Game/GameClient/NameCache.h"
#include "WoW/Source/Ui/UILCD/LCD.h"
#include "WoW/Source/WowServices/WDataStore.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/FriendList.h"
#include "Engine/Source/Tempest/CMath.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/AccountData.h"
#include "WoW/Source/Ui/PartyFrame.h"
#include "WoW/Source/DB/Rec/MapRec.h"
#include "WoW/Source/DB/Rec/MapDifficultyRec.h"
#include "WoW/Source/DB/Rec/CharTitlesRec.h"
#include "WoW/Source/DB/Rec/FactionTemplateRec.h"
#include "WoW/Source/DB/Rec/SpellRec.h"
#include "WoW/Source/DB/DBClient/GameObjectStats_C.h"
#include "WoW/Source/Object/ObjectClient/Effect_C.h"
#include "Engine/Source/Event/FrameTime.h"
#include "WoW/Source/SoundInterface/SI3.h"

// STATIC: 0x00F326FC/0x140E241BC
int32_t CGChat::m_afk;

// STATIC: 0x00F32700/0x140E241C0
int32_t CGChat::m_dnd;

// STATIC: 0x00F32718/0x140E241D8
int32_t CGChat::m_voiceChatSystemStatus;

// STATIC: 0x00BF0278/0x1409D97A0
const ChatColorInfo s_chatTypeInfo[CHAT_MSG_MAX] = {
    { "SYSTEM", { 0xFF, 0xFF, 0x00, 0x00 }, 0, 0 },
    { "SAY", { 0xFF, 0xFF, 0xFF, 0x00 }, 0, 0 },
    { "PARTY", { 0xAA, 0xAA, 0xFF, 0x00 }, 0, 0 },
    { "RAID", { 0xFF, 0x7F, 0x00, 0x00 }, 0, 0 },
    { "GUILD", { 0x40, 0xFF, 0x40, 0x00 }, 0, 0 },
    { "OFFICER", { 0x40, 0xC0, 0x40, 0x00 }, 0, 0 },
    { "YELL", { 0xFF, 0x40, 0x40, 0x00 }, 0, 0 },
    { "WHISPER", { 0xFF, 0x80, 0xFF, 0x00 }, 0, 0 },
    { "WHISPER_FOREIGN", { 0xFF, 0x80, 0xFF, 0x00 }, 0, 0 },
    { "WHISPER_INFORM", { 0xFF, 0x80, 0xFF, 0x00 }, 0, 0 },
    { "EMOTE", { 0xFF, 0x80, 0x40, 0x00 }, 0, 0 },
    { "TEXT_EMOTE", { 0xFF, 0x80, 0x40, 0x00 }, 0, 0 },
    { "MONSTER_SAY", { 0xFF, 0xFF, 0x9F, 0x00 }, 0, 0 },
    { "MONSTER_PARTY", { 0xAA, 0xAA, 0xFF, 0x00 }, 3, 0 },
    { "MONSTER_YELL", { 0xFF, 0x40, 0x40, 0x00 }, 0, 0 },
    { "MONSTER_WHISPER", { 0xFF, 0xB5, 0xEB, 0x00 }, 3, 0 },
    { "MONSTER_EMOTE", { 0xFF, 0x80, 0x40, 0x00 }, 0, 0 },
    { "CHANNEL", { 0xFF, 0xC0, 0xC0, 0x00 }, 0, 0 },
    { "CHANNEL_JOIN", { 0xC0, 0x80, 0x80, 0x00 }, 0, 0 },
    { "CHANNEL_LEAVE", { 0xC0, 0x80, 0x80, 0x00 }, 0, 0 },
    { "CHANNEL_LIST", { 0xC0, 0x80, 0x80, 0x00 }, 0, 0 },
    { "CHANNEL_NOTICE", { 0xC0, 0xC0, 0xC0, 0x00 }, 0, 0 },
    { "CHANNEL_NOTICE_USER", { 0xC0, 0xC0, 0xC0, 0x00 }, 0, 0 },
    { "AFK", { 0xFF, 0x80, 0xFF, 0x00 }, 0, 0 },
    { "DND", { 0xFF, 0x80, 0xFF, 0x00 }, 0, 0 },
    { "IGNORED", { 0xFF, 0x00, 0x00, 0x00 }, 0, 0 },
    { "SKILL", { 0x55, 0x55, 0xFF, 0x00 }, 0, 0 },
    { "LOOT", { 0x00, 0xAA, 0x00, 0x00 }, 0, 0 },
    { "MONEY", { 0xFF, 0xFF, 0x00, 0x00 }, 0, 0 },
    { "OPENING", { 0x80, 0x80, 0xFF, 0x00 }, 0, 0 },
    { "TRADESKILLS", { 0xFF, 0xFF, 0xFF, 0x00 }, 0, 0 },
    { "PET_INFO", { 0x80, 0x80, 0xFF, 0x00 }, 0, 0 },
    { "COMBAT_MISC_INFO", { 0x80, 0x80, 0xFF, 0x00 }, 0, 0 },
    { "COMBAT_XP_GAIN", { 0x6F, 0x6F, 0xFF, 0x00 }, 0, 0 },
    { "COMBAT_HONOR_GAIN", { 0xE0, 0xCA, 0x0A, 0x00 }, 0, 0 },
    { "COMBAT_FACTION_CHANGE", { 0x80, 0x80, 0xFF, 0x00 }, 0, 0 },
    { "BG_SYSTEM_NEUTRAL", { 0xFF, 0x78, 0x0A, 0x00 }, 0, 0 },
    { "BG_SYSTEM_ALLIANCE", { 0x00, 0xAE, 0xEF, 0x00 }, 0, 0 },
    { "BG_SYSTEM_HORDE", { 0xFF, 0x00, 0x00, 0x00 }, 0, 0 },
    { "RAID_LEADER", { 0xFF, 0x48, 0x09, 0x00 }, 0, 0 },
    { "RAID_WARNING", { 0xFF, 0x48, 0x00, 0x00 }, 0, 0 },
    { "RAID_BOSS_EMOTE", { 0xFF, 0xDD, 0x00, 0x00 }, 0, 0 },
    { "RAID_BOSS_WHISPER", { 0xFF, 0xDD, 0x00, 0x00 }, 0, 0 },
    { "FILTERED", { 0xFF, 0x00, 0x00, 0x00 }, 0, 0 },
    { "BATTLEGROUND", { 0xFF, 0x7F, 0x00, 0x00 }, 0, 0 },
    { "BATTLEGROUND_LEADER", { 0xFF, 0xDB, 0xB7, 0x00 }, 0, 0 },
    { "RESTRICTED", { 0xFF, 0x00, 0x00, 0x00 }, 0, 0 },
    { "BATTLENET", { 0xFF, 0xFF, 0xFF, 0x00 }, 0, 0 },
    { "ACHIEVEMENT", { 0xFF, 0xFF, 0x00, 0x00 }, 9, 0 },
    { "GUILD_ACHIEVEMENT", { 0x40, 0xFF, 0x40, 0x00 }, 0, 0 },
    { "ARENA_POINTS", { 0xFF, 0xFF, 0xFF, 0x00 }, 0, 0 },
    { "PARTY_LEADER", { 0x76, 0xC8, 0xFF, 0x00 }, 10, 0 },
    { "TARGETICONS", { 0xFF, 0xFF, 0x00, 0x00 }, 11, 0 },
    { "BN_WHISPER", { 0x00, 0xFF, 0xF6, 0x00 }, 12, 0 },
    { "BN_WHISPER_INFORM", { 0x00, 0xFF, 0xF6, 0x00 }, 12, 0 },
    { "BN_CONVERSATION", { 0x00, 0xB1, 0xF0, 0x00 }, 12, 0 },
    { "BN_CONVERSATION_NOTICE", { 0x00, 0xB1, 0xF0, 0x00 }, 12, 0 },
    { "BN_CONVERSATION_LIST", { 0x00, 0xB1, 0xF0, 0x00 }, 12, 0 },
    { "BN_INLINE_TOAST_ALERT", { 0x82, 0xC5, 0xFF, 0x00 }, 13, 0 },
    { "BN_INLINE_TOAST_BROADCAST", { 0x82, 0xC5, 0xFF, 0x00 }, 13, 0 },
    { "BN_INLINE_TOAST_BROADCAST_INFORM", { 0x82, 0xC5, 0xFF, 0x00 }, 13, 0 },
    { "BN_INLINE_TOAST_CONVERSATION", { 0x82, 0xC5, 0xFF, 0x00 }, 13, 0 },
    { "COMBAT_GUILD_XP_GAIN", { 0x6F, 0x6F, 0xFF, 0x00 }, 14, 0 },
    { "CURRENCY", { 0x00, 0xAA, 0x00, 0x00 }, 15, 0 },
    { "BN_WHISPER_PLAYER_OFFLINE", { 0xFF, 0xFF, 0x00, 0x00 }, 16, 0 },
};

// STATIC: 0x00BF1C70/0x1409DB2C0
static const uint32_t s_chatEvents[CHAT_MSG_MAX] = {
    SCRIPT_EVENT_CHAT_MSG_SYSTEM,
    SCRIPT_EVENT_CHAT_MSG_SAY,
    SCRIPT_EVENT_CHAT_MSG_PARTY,
    SCRIPT_EVENT_CHAT_MSG_RAID,
    SCRIPT_EVENT_CHAT_MSG_GUILD,
    SCRIPT_EVENT_CHAT_MSG_OFFICER,
    SCRIPT_EVENT_CHAT_MSG_YELL,
    SCRIPT_EVENT_CHAT_MSG_WHISPER,
    SCRIPT_EVENT_CHAT_MSG_WHISPER,
    SCRIPT_EVENT_CHAT_MSG_WHISPER_INFORM,
    SCRIPT_EVENT_CHAT_MSG_EMOTE,
    SCRIPT_EVENT_CHAT_MSG_TEXT_EMOTE,
    SCRIPT_EVENT_CHAT_MSG_MONSTER_SAY,
    SCRIPT_EVENT_CHAT_MSG_MONSTER_PARTY,
    SCRIPT_EVENT_CHAT_MSG_MONSTER_YELL,
    SCRIPT_EVENT_CHAT_MSG_MONSTER_WHISPER,
    SCRIPT_EVENT_CHAT_MSG_MONSTER_EMOTE,
    SCRIPT_EVENT_CHAT_MSG_CHANNEL,
    SCRIPT_EVENT_CHAT_MSG_CHANNEL_JOIN,
    SCRIPT_EVENT_CHAT_MSG_CHANNEL_LEAVE,
    SCRIPT_EVENT_CHAT_MSG_CHANNEL_LIST,
    SCRIPT_EVENT_CHAT_MSG_CHANNEL_NOTICE,
    SCRIPT_EVENT_CHAT_MSG_CHANNEL_NOTICE_USER,
    SCRIPT_EVENT_CHAT_MSG_AFK,
    SCRIPT_EVENT_CHAT_MSG_DND,
    SCRIPT_EVENT_CHAT_MSG_IGNORED,
    SCRIPT_EVENT_CHAT_MSG_SKILL,
    SCRIPT_EVENT_CHAT_MSG_LOOT,
    SCRIPT_EVENT_CHAT_MSG_MONEY,
    SCRIPT_EVENT_CHAT_MSG_OPENING,
    SCRIPT_EVENT_CHAT_MSG_TRADESKILLS,
    SCRIPT_EVENT_CHAT_MSG_PET_INFO,
    SCRIPT_EVENT_CHAT_MSG_COMBAT_MISC_INFO,
    SCRIPT_EVENT_CHAT_MSG_COMBAT_XP_GAIN,
    SCRIPT_EVENT_CHAT_MSG_COMBAT_HONOR_GAIN,
    SCRIPT_EVENT_CHAT_MSG_COMBAT_FACTION_CHANGE,
    SCRIPT_EVENT_CHAT_MSG_BG_SYSTEM_NEUTRAL,
    SCRIPT_EVENT_CHAT_MSG_BG_SYSTEM_ALLIANCE,
    SCRIPT_EVENT_CHAT_MSG_BG_SYSTEM_HORDE,
    SCRIPT_EVENT_CHAT_MSG_RAID_LEADER,
    SCRIPT_EVENT_CHAT_MSG_RAID_WARNING,
    SCRIPT_EVENT_CHAT_MSG_RAID_BOSS_EMOTE,
    SCRIPT_EVENT_CHAT_MSG_RAID_BOSS_WHISPER,
    SCRIPT_EVENT_CHAT_MSG_FILTERED,
    SCRIPT_EVENT_CHAT_MSG_BATTLEGROUND,
    SCRIPT_EVENT_CHAT_MSG_BATTLEGROUND_LEADER,
    SCRIPT_EVENT_CHAT_MSG_RESTRICTED,
    236,
    SCRIPT_EVENT_CHAT_MSG_ACHIEVEMENT,
    SCRIPT_EVENT_CHAT_MSG_GUILD_ACHIEVEMENT,
    666,
    SCRIPT_EVENT_CHAT_MSG_PARTY_LEADER,
    SCRIPT_EVENT_CHAT_MSG_TARGETICONS,
    SCRIPT_EVENT_CHAT_MSG_BN_WHISPER,
    SCRIPT_EVENT_CHAT_MSG_BN_WHISPER_INFORM,
    SCRIPT_EVENT_CHAT_MSG_BN_CONVERSATION,
    SCRIPT_EVENT_CHAT_MSG_BN_CONVERSATION_NOTICE,
    SCRIPT_EVENT_CHAT_MSG_BN_CONVERSATION_LIST,
    SCRIPT_EVENT_CHAT_MSG_BN_INLINE_TOAST_ALERT,
    SCRIPT_EVENT_CHAT_MSG_BN_INLINE_TOAST_BROADCAST,
    SCRIPT_EVENT_CHAT_MSG_BN_INLINE_TOAST_BROADCAST_INFORM,
    SCRIPT_EVENT_CHAT_MSG_BN_INLINE_TOAST_CONVERSATION,
    SCRIPT_EVENT_CHAT_MSG_BN_WHISPER_PLAYER_OFFLINE,
    SCRIPT_EVENT_CHAT_MSG_COMBAT_GUILD_XP_GAIN,
    SCRIPT_EVENT_CHAT_MSG_CURRENCY,
};

// STATIC: 0x00BF0230
static const int32_t s_profanityFilterTypes[18] = {
    CHAT_MSG_SAY,
    CHAT_MSG_PARTY,
    CHAT_MSG_PARTY_LEADER,
    CHAT_MSG_RAID,
    CHAT_MSG_RAID_LEADER,
    CHAT_MSG_GUILD,
    CHAT_MSG_OFFICER,
    CHAT_MSG_YELL,
    CHAT_MSG_WHISPER,
    CHAT_MSG_WHISPER_INFORM,
    CHAT_MSG_EMOTE,
    CHAT_MSG_CHANNEL,
    CHAT_MSG_AFK,
    CHAT_MSG_DND,
    CHAT_MSG_RAID_WARNING,
    CHAT_MSG_BATTLEGROUND,
    CHAT_MSG_BATTLEGROUND_LEADER,
    CHAT_MSG_SYSTEM,
};

// STATIC: 0x00BF23F8
static const char s_profanityReplacement[] = "!@#$%^&*";

// STATIC: 0x00D101D8/0x140BBD750
static int32_t s_queueChat = 1;

// STATIC: 0x00D104D8/0x140BBDD98
TSExplicitList<THROTTLEDCHAT, 0> CGChat::s_throttledChat;

// STATIC: 0x00D104E4/0x140BBDDF8
TSExplicitList<PENDINGCHAT, 0> CGChat::s_pendingChat;

// STATIC: 0x00ED9160/0x140DCAB98
int32_t s_chatLogging[2];

// STATIC: 0x00ED9168/0x140DCAC20
HSLOG s_chatLog[2];

// STATIC: 0x00ED9170/0x140DCAC30
ChatRecord CGChat::m_chatHistory[60];

// STATIC: 0x00F32270/0x140E23D30
WOWGUID CGChat::m_whisperDstHistory[128];

// STATIC: 0x00F32670/0x140E24130
WOWGUID CGChat::m_whisperSrcHistory[16];

// STATIC: 0x00F32708/0x140E241C8
uint32_t CGChat::m_currentChatIndex;

// STATIC: 0x00F3270C
static int32_t s_chatHistoryInitialized;

// STATIC: 0x00F32714
static uint32_t s_chatLineID;

// STATIC: 0x00F32780/0x140E24230
static uint32_t s_whisperThrottleIndex;

// STATIC: 0x00F32784/0x140E24234
static uint32_t s_whisperThrottleTimes[10];

// STATIC: 0x00F3273C/0x140BBDD50
static TSHashTable<CHATTHROTTLE, HASHKEY_STR> s_channelThrottle;

// STATIC: 0x00F327F8/0x140E24298
TSGrowableArray<ChatChannel> CGChat::s_channels;

// STATIC: 0x00F3329C/0x140BBDE80
static TSHashTable<PROFANITYCHECK, HASHKEY_STR> s_profanityCache;

// STATIC: 0x00F332C8/0x140BBDED0
static TSHashTable<SPAMCHECK, HASHKEY_STR> s_spamCache;

// STATIC: 0x00F33310/0x140E24E34
static uint32_t s_profanityReplacementIndex;

// STATIC: 0x00BF15C8/0x1409DAAF0
const ChatMessageGroup s_chatMessageGroups[49] = {
    { "SYSTEM", true, 0 },
    { "SYSTEM_NOMENU", true, 5 },
    { "SAY", true, 0 },
    { "EMOTE", true, 7 },
    { "YELL", true, 0 },
    { "WHISPER", true, 0 },
    { "PARTY", true, 0 },
    { "PARTY_LEADER", true, 10 },
    { "RAID", true, 7 },
    { "RAID_LEADER", true, 7 },
    { "RAID_WARNING", true, 7 },
    { "BATTLEGROUND", true, 7 },
    { "BATTLEGROUND_LEADER", true, 7 },
    { "GUILD", true, 0 },
    { "OFFICER", true, 13 },
    { "MONSTER_SAY", true, 7 },
    { "MONSTER_YELL", true, 7 },
    { "MONSTER_EMOTE", true, 7 },
    { "MONSTER_WHISPER", true, 7 },
    { "MONSTER_BOSS_EMOTE", true, 7 },
    { "MONSTER_BOSS_WHISPER", true, 7 },
    { "ERRORS", true, 7 },
    { "AFK", true, 7 },
    { "DND", true, 7 },
    { "IGNORED", true, 7 },
    { "BG_HORDE", true, 7 },
    { "BG_ALLIANCE", true, 7 },
    { "BG_NEUTRAL", true, 7 },
    { "COMBAT_FACTION_CHANGE", true, 1 },
    { "SKILL", true, 0 },
    { "LOOT", true, 0 },
    { "MONEY", true, 2 },
    { "CHANNEL", true, 0 },
    { "ACHIEVEMENT", true, 8 },
    { "GUILD_ACHIEVEMENT", true, 8 },
    { "TARGETICONS", false, 11 },
    { "BN_WHISPER", true, 12 },
    { "BN_WHISPER_INFORM", true, 12 },
    { "BN_CONVERSATION", true, 12 },
    { "BN_INLINE_TOAST_ALERT", true, 13 },
    { "CURRENCY", true, 15 },
    { "BN_WHISPER_PLAYER_OFFLINE", true, 16 },
    { "OPENING", true, 6 },
    { "TRADESKILLS", true, 6 },
    { "PET_INFO", true, 6 },
    { "COMBAT_XP_GAIN", true, 0 },
    { "COMBAT_HONOR_GAIN", true, 0 },
    { "COMBAT_MISC_INFO", true, 0 },
    { "COMBAT_GUILD_XP_GAIN", true, 14 },
};

// STATIC: 0x00ED7A74/0x140DC9524
uint32_t s_activeVoiceChannel;

// STATIC: 0x00ED7A78/0x140DC9528
uint8_t s_collapsedHeaders[3];

// STATIC: 0x00ED7A7B/0x140DC952B
static bool s_pendingUserListActive;

// STATIC: 0x00ED7A7C/0x140DC952C
uint32_t s_activeDisplayChannelFlags;

// STATIC: 0x00ED7A80/0x140DC9530
int32_t s_displayChannelMemberCount;

// STATIC: 0x00ED7A84/0x140DC9534
int32_t s_activeDisplayChannelType;

// STATIC: 0x00ED7A88/0x140DC9540
char CGChat::s_activeDisplayChannelName[128];

// STATIC: 0x00ED7B08/0x140DC9538
uint32_t s_numDisplayChannels;

// STATIC: 0x00ED7B0C/0x140DCAB90
static AreaTableRec* s_cityArea;

// STATIC: 0x00ED7B10/0x140DC95C0
DisplayChannel CGChat::s_displayChannels[20];

// STATIC: 0x00ED7D90/0x140DC9840
ChatColorInfo s_chatColors[CHAT_MSG_MAX];

// STATIC: 0x00ED90E0
static char s_inviteChannelName[128];

// STATIC: 0x00F326F4/0x140E241B4
int32_t CGChat::m_loaded;

// STATIC: 0x00F326F8/0x140E241B8
int32_t CGChat::m_registered;

// STATIC: 0x00F32710/0x140E241D0
int32_t CGChat::m_complaintsSystemStatus;

// STATIC: 0x00F32738/0x140E241F8
uint32_t s_joinedZoneChannels;

// STATIC: 0x00F32808/0x140E242B0
TSGrowableArray<ChatColorInfo> s_channelColors;

// STATIC: 0x00F32818/0x140E242D0
ChatWindowInfo s_chatWindows[10];

// STATIC: 0x00F33270/0x140BBDE30
static TSHashTable<WORDLIST, HASHKEY_LANGUAGE> s_wordLists;

// STATIC: 0x00F332F4/0x140E24E08
TSGrowableArray<PENDINGUSERLIST> s_pendingUsers;

// STATIC: 0x00D104F0/0x140BBDE10
TSExplicitList<PENDINGTEXTEMOTE, 0> CGChat::s_pendingTextEmotes;

// STATIC: 0x00F32704/0x140E241C4
int32_t CGChat::m_areaID;

// STATIC: 0x00BF1814
static const ZoneChannelDefault s_zoneChannelDefaults[1] = { { 26, 4 } };

// STATIC: 0x00F326F0/0x140E241B0
static int32_t s_chatSettingsDirty;

// STATIC: 0x00F3271C/0x140E241DC
static int32_t s_chatServerDown;

// STATIC: 0x00F32720/0x140E241E0
static int32_t s_queueAddonPrefixes;

// STATIC: 0x00F32724/0x140E241E4
static uint32_t s_numAddonPrefixes;

// STATIC: 0x00F32728/0x140E241E8
WOWGUID s_pendingReportTarget;

// STATIC: 0x00F327AC/0x140E24260
static UserClientChatRegisterAddonPrefixes s_addonPrefixMessage;

// STATIC: 0x00F327CC/0x140BBDDB0
TSHashTable<ADDONPREFIX, HASHKEY_STR> s_addonPrefixes;

// STATIC: 0x00F33240/0x140E24DC0
static TSGrowableArray<char*> s_pendingChannelNames;

// STATIC: 0x00F33250/0x140E24DD8
static TSGrowableArray<bool> s_pendingChannelFlags;

// STATIC: 0x00F33260/0x140E24DF0
static TSGrowableArray<uint32_t> s_pendingChannelNumbers;

// STATIC: 0x00D1079C/0x140BBE288
static const char* s_exhaustionTokens[7] = { NULL, "COMBATLOG_XPGAIN_EXHAUSTION1", "COMBATLOG_XPGAIN_EXHAUSTION2", "NULL", "COMBATLOG_XPGAIN_EXHAUSTION4", "COMBATLOG_XPGAIN_EXHAUSTION5", "COMBATLOG_XPGAIN_EXHAUSTION1" };

// OFFSET: 0x008E5CD0
int32_t CGChat::OnChatDown(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    msg->m_read = msg->m_size;
    s_chatServerDown = 1;
    FrameScript_SignalEvent(SCRIPT_EVENT_CHAT_SERVER_DISCONNECTED, "%b", 1);
    return 1;
}

// OFFSET: [0x008E5CD0]
int32_t CGChat::OnChatIsDown(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    msg->m_read = msg->m_size;
    s_chatServerDown = 1;
    FrameScript_SignalEvent(SCRIPT_EVENT_CHAT_SERVER_DISCONNECTED, "%b", 1);
    return 1;
}

// OFFSET: 0x008E62E0/0x140614570
int32_t CGChat::IsVoiceChatAllowed() {
    return CGChat::m_voiceChatSystemStatus && !ComSatClient_IsComsatDisabledByClient();
}

// OFFSET: 0x008E57E0/0x140613B30
static int QSortDisplayListMembers(const void* a, const void* b) {
    if (!a) {
        return -1;
    }

    if (!b) {
        return 1;
    }

    WOWGUID none = WOWGUID();
    NameCache* left = g_nameDBCache.GetRecord(static_cast<const PENDINGUSERLIST*>(a)->m_guid.m_guid, none, NULL, NULL, false);
    WOWGUID none2 = WOWGUID();
    NameCache* right = g_nameDBCache.GetRecord(static_cast<const PENDINGUSERLIST*>(b)->m_guid.m_guid, none2, NULL, NULL, false);

    if (!left) {
        return -1;
    }

    if (!right) {
        return 1;
    }

    if (!left->m_name[0]) {
        return -1;
    }

    return SStrSortUTF8I(left->m_name, right->m_name, STORM_MAX_STR);
}

// OFFSET: 0x008E5950
int32_t CGChat::IsRecordedType(int32_t type) {
    switch (type) {
        case CHAT_MSG_SAY:
        case CHAT_MSG_PARTY:
        case CHAT_MSG_RAID:
        case CHAT_MSG_GUILD:
        case CHAT_MSG_OFFICER:
        case CHAT_MSG_YELL:
        case CHAT_MSG_WHISPER:
        case CHAT_MSG_WHISPER_INFORM:
        case CHAT_MSG_EMOTE:
        case CHAT_MSG_TEXT_EMOTE:
        case CHAT_MSG_CHANNEL:
        case CHAT_MSG_AFK:
        case CHAT_MSG_DND:
        case CHAT_MSG_RAID_LEADER:
        case CHAT_MSG_RAID_WARNING:
        case CHAT_MSG_BATTLEGROUND:
        case CHAT_MSG_BATTLEGROUND_LEADER:
        case CHAT_MSG_BATTLENET:
        case CHAT_MSG_PARTY_LEADER:
        case CHAT_MSG_BN_WHISPER:
        case CHAT_MSG_BN_WHISPER_INFORM:
        case CHAT_MSG_BN_CONVERSATION:
            return 1;
        default:
            return 0;
    }
}

// OFFSET: 0x008E5ED0
int32_t CGChat::IsInWhisperSourceHistory(const char* name) {
    for (uint32_t i = 0; i < sizeof(((CGChat*)0)->m_whisperSrcHistory) / sizeof(CGChat::m_whisperSrcHistory[0]); i++) {
        WOWGUID guid = CGChat::m_whisperSrcHistory[i];

        if (guid) {
            WOWGUID requester = WOWGUID();
            NameCache* record = g_nameDBCache.GetRecord(guid.m_guid, requester, NULL, NULL, false);

            if (record && !SStrCmpUTF8I(name, record->m_name, STORM_MAX_STR)) {
                return 1;
            }
        }
    }

    return 0;
}

// OFFSET: 0x008E5F50/0x1406142C0
void CGChat::RecordWhisperSource(WOWGUID guid) {
    if (!guid) {
        return;
    }

    uint32_t count = sizeof(((CGChat*)0)->m_whisperSrcHistory) / sizeof(CGChat::m_whisperSrcHistory[0]);
    uint32_t index;

    for (index = 0; index < count; index++) {
        if (CGChat::m_whisperSrcHistory[index] == guid) {
            break;
        }
    }

    if (index >= count - 1) {
        index = count - 1;
    }

    memmove(&CGChat::m_whisperSrcHistory[1], &CGChat::m_whisperSrcHistory[0], index * sizeof(CGChat::m_whisperSrcHistory[0]));
    CGChat::m_whisperSrcHistory[0] = guid;
}

// OFFSET: 0x008E6010/0x140614320
int32_t CGChat::IsInWhisperHistory(WOWGUID guid) {
    if (!guid) {
        return 0;
    }

    for (uint32_t i = 0; i < sizeof(((CGChat*)0)->m_whisperDstHistory) / sizeof(CGChat::m_whisperDstHistory[0]); i++) {
        if (CGChat::m_whisperDstHistory[i] == guid) {
            return 1;
        }
    }

    return 0;
}

// OFFSET: 0x008E6050/0x140614350
void CGChat::RecordWhisperDest(WOWGUID guid) {
    if (!guid) {
        return;
    }

    uint32_t count = sizeof(((CGChat*)0)->m_whisperDstHistory) / sizeof(CGChat::m_whisperDstHistory[0]);
    uint32_t index;

    for (index = 0; index < count; index++) {
        if (CGChat::m_whisperDstHistory[index] == guid) {
            break;
        }
    }

    if (index >= count - 1) {
        index = count - 1;
    }

    memmove(&CGChat::m_whisperDstHistory[1], &CGChat::m_whisperDstHistory[0], index * sizeof(CGChat::m_whisperDstHistory[0]));
    CGChat::m_whisperDstHistory[0] = guid;
}

// OFFSET: 0x008E6120/0x1406143B0
int32_t CGChat::IsWhisperThrottled(uint32_t time) {
    uint32_t last = s_whisperThrottleTimes[s_whisperThrottleIndex];
    return last && static_cast<int32_t>(time - last - 10000) < 0;
}

// OFFSET: 0x008E6150
void CGChat::AddWhisperThrottled(uint32_t time) {
    s_whisperThrottleTimes[s_whisperThrottleIndex] = time;
    s_whisperThrottleIndex = (s_whisperThrottleIndex + 1) % (sizeof(s_whisperThrottleTimes) / sizeof(s_whisperThrottleTimes[0]));
}

// OFFSET: 0x008E72B0/0x140615660
static void UpdateVoiceMemberFlags(PENDINGUSERLIST* user, uint8_t flags, WOWGUID guid, int32_t type, const char* name) {
    if (!user) {
        return;
    }

    user->m_guid = guid;
    user->m_flags = flags;

    if (type && !ComSatClient_IsTalkerSquelchedInChannel(guid, type, name)) {
        user->m_flags |= 0x10;
    }

    if (ComSatClient_IsMemberInVoiceSession(user->m_guid, type, name)) {
        user->m_flags |= 0x40;
    }

    if (ComSatClient_GetChannelMemberVoiceStatus(user->m_guid, type, name)) {
        user->m_flags |= 0x80;
    }

    if (ComSatClient_IsPlayerMuted(user->m_guid) || ComSatClient_IsTalkerSquelchedInChannel(user->m_guid, type, name)) {
        user->m_flags |= 0x20;
    }
}

// OFFSET: 0x008E7780
static bool IsInColorCode(const char* text, int32_t length) {
    bool inColor = false;

    while (length) {
        char c = *text;
        length--;
        text++;

        if (c == '|') {
            if (!length--) {
                return inColor;
            }

            c = *text;

            if (c == 'c' || c == 'C') {
                inColor = true;
            } else if ((c == 'r' || c == 'R') && inColor) {
                inColor = false;
            }

            text++;
        }
    }

    return inColor;
}

// OFFSET: 0x008E58A0/0x140613C10
static bool IsStaffChatFlag(const char* flags) {
    if (!flags) {
        return false;
    }

    if (!SStrCmp(flags, "DEV", STORM_MAX_STR) || !SStrCmp(flags, "GM", STORM_MAX_STR)) {
        return true;
    }

    return SStrCmp(flags, "COM", STORM_MAX_STR) == 0;
}

// OFFSET: 0x008E82E0/0x1406168E0
static void SendChatEvent(ChatChannel* channel, uint32_t language, const char* text, int32_t type, const char* senderName, const char* channelString, const char* targetName, const char* flags, uint32_t lineID, WOWGUID guid, const ChatConversation* conversation, bool a12) {
    if (type >= CHAT_MSG_MAX) {
        return;
    }

    const char* languageName = "";
    LanguagesRec* languageRec = g_languagesDB.GetRecord(language);

    if (languageRec) {
        languageName = languageRec->m_name;
    }

    if (type == CHAT_MSG_WHISPER) {
        CGTutorial::TriggerTutorial(16);
    }

    char guidString[20] = {};

    if (!(guid.m_guid & 0xF000000000000000ULL) && (guid.m_guid & 0xF07FFFFFFFFFFFFFULL) && (!flags || !IsStaffChatFlag(flags))) {
        GUIDToString(guid, guidString);
    }

    uint32_t presenceID = conversation ? conversation->m_presenceID : 0;
    int32_t channelIndex;
    uint32_t zoneChannelID;
    const char* channelBaseName;
    uint32_t unused;

    if (channel) {
        channelIndex = channel->m_channelNumber;
        unused = channel->m_unk98;
        zoneChannelID = channel->m_zoneChannelID;
        channelBaseName = channel->m_name;
    } else {
        unused = 0;
        channelBaseName = "";
        channelIndex = conversation && !conversation->m_inactive ? conversation->m_index + 1 : 0;
        zoneChannelID = 0;
    }

    FrameScript_SignalEvent(s_chatEvents[type], "%s%s%s%s%s%s%d%d%s%d%d%s%u%b", text ? text : "", senderName ? senderName : "", languageName, channelString ? channelString : "", targetName ? targetName : "", flags ? flags : "", zoneChannelID, channelIndex, channelBaseName, unused, lineID, guidString, presenceID, a12);
}

// OFFSET: 0x008E8470/0x140616B50
void CGChat::RecordChat(const char* text, int32_t type, const char* senderName, WOWGUID senderGuid, uint32_t senderUnk, uint32_t a6, const char* channelName, uint32_t lineID, const CGPlayer_C* player) {
    if (!s_chatHistoryInitialized) {
        for (uint32_t i = 0; i < sizeof(((CGChat*)0)->m_chatHistory) / sizeof(CGChat::m_chatHistory[0]); i++) {
            ChatRecord* record = &CGChat::m_chatHistory[i];
            record->m_senderGuid.m_guid = 0;
            record->m_senderUnk = 0;
            record->m_senderName[0] = 0;
            record->m_description[0] = 0;
            record->m_text[0] = 0;
            record->m_type = 0;
            record->m_unk17B0 = 0;
            record->m_lineID = 0;
            record->m_time = 0;
        }

        s_chatHistoryInitialized = 1;
    }

    if (!text || !CGChat::IsRecordedType(type)) {
        return;
    }

    ChatRecord* record = &CGChat::m_chatHistory[CGChat::m_currentChatIndex];
    WOWGUID playerGuid = player ? player->m_obj->m_guid : WOWGUID();

    SStrPrintf(record->m_description, sizeof(record->m_description), "Type: [%d], Channel: [%s], Player Name: [%s], Sender GUID: [%016llX], Active player: [%016llX], Text: [%s]", type, channelName ? channelName : "", senderName ? senderName : "", senderGuid.m_guid, playerGuid.m_guid, text);
    SStrCopy(record->m_text, text, sizeof(record->m_text));
    record->m_time = static_cast<uint32_t>(time(NULL));
    record->m_type = type;
    record->m_senderGuid = senderGuid;
    record->m_senderUnk = senderUnk;
    record->m_unk17B0 = a6;

    if (senderName) {
        SStrCopy(record->m_senderName, senderName, sizeof(record->m_senderName));
    }

    record->m_lineID = lineID;

    if (++CGChat::m_currentChatIndex >= sizeof(((CGChat*)0)->m_chatHistory) / sizeof(CGChat::m_chatHistory[0])) {
        CGChat::m_currentChatIndex = 0;
    }
}

// OFFSET: 0x008E8730/0x140616EE0
ChatChannel* GetChannelByName(const char* name) {
    for (uint32_t i = 0; i < CGChat::s_channels.Count(); i++) {
        if (!SStrCmpUTF8I(CGChat::s_channels.m_data[i].m_name, name, STORM_MAX_STR)) {
            return &CGChat::s_channels.m_data[i];
        }
    }

    return NULL;
}

// OFFSET: 0x008E8670
const char* CGChat::GetChannelName(int32_t number, bool includeInactive) {
    if (number < 1 || number > static_cast<int32_t>(CGChat::s_channels.m_count)) {
        return NULL;
    }

    ChatChannel* channel = &CGChat::s_channels.m_data[number - 1];

    if (channel->m_channelNumber != number || (!includeInactive && channel->m_unk9C)) {
        return NULL;
    }

    return channel->m_name;
}

// OFFSET: 0x008E87F0
ChatChannel* CGChat::GetChannelByNumber(int32_t number) {
    if (number < 1 || number > static_cast<int32_t>(CGChat::s_channels.m_count)) {
        return NULL;
    }

    ChatChannel* channel = &CGChat::s_channels.m_data[number - 1];

    if (channel->m_channelNumber != number) {
        return NULL;
    }

    return channel->m_unk9C ? NULL : channel;
}

// OFFSET: 0x008E8840/0x140616FB0
void CGChat::SendChat(int32_t language, const char* text, int32_t type, const char* target, const char* prefix) {
    if (s_chatServerDown && language != -1) {
        FrameScript_SignalEvent(SCRIPT_EVENT_CHAT_SERVER_DISCONNECTED, NULL);
    }

    if (language == -1) {
        if (!prefix) {
            prefix = "";
        }

        switch (type) {
            case CHAT_MSG_PARTY:
            case CHAT_MSG_PARTY_LEADER: {
                UserClientMessageChatAddonParty msg(prefix, text);
                ClientServices::Send(&msg);
                break;
            }

            case CHAT_MSG_RAID:
            case CHAT_MSG_RAID_LEADER: {
                UserClientMessageChatAddonRaid msg(prefix, text);
                ClientServices::Send(&msg);
                break;
            }

            case CHAT_MSG_GUILD: {
                UserClientMessageChatAddonGuild msg(prefix, text);
                ClientServices::Send(&msg);
                break;
            }

            case CHAT_MSG_OFFICER: {
                UserClientMessageChatAddonOfficer msg(prefix, text);
                ClientServices::Send(&msg);
                break;
            }

            case CHAT_MSG_WHISPER: {
                UserClientMessageChatAddonWhisper msg(target, prefix, text);
                ClientServices::Send(&msg);
                break;
            }

            case CHAT_MSG_BATTLEGROUND:
            case CHAT_MSG_BATTLEGROUND_LEADER: {
                UserClientMessageChatAddonBattleground msg(prefix, text);
                ClientServices::Send(&msg);
                break;
            }

            default:
                SysMsgPrintf(STATUS_ERROR, "SendChatMessage: No JAM message for addon chat type: %d.", type);
                break;
        }

        return;
    }

    switch (type) {
        case CHAT_MSG_SAY: {
            UserClientMessageChatSay msg(language, text);
            ClientServices::Send(&msg);
            break;
        }

        case CHAT_MSG_PARTY:
        case CHAT_MSG_PARTY_LEADER: {
            UserClientMessageChatParty msg(language, text);
            ClientServices::Send(&msg);
            break;
        }

        case CHAT_MSG_RAID:
        case CHAT_MSG_RAID_LEADER: {
            UserClientMessageChatRaid msg(language, text);
            ClientServices::Send(&msg);
            break;
        }

        case CHAT_MSG_GUILD: {
            UserClientMessageChatGuild msg(language, text);
            ClientServices::Send(&msg);
            break;
        }

        case CHAT_MSG_OFFICER: {
            UserClientMessageChatOfficer msg(language, text);
            ClientServices::Send(&msg);
            break;
        }

        case CHAT_MSG_YELL: {
            UserClientMessageChatYell msg(language, text);
            ClientServices::Send(&msg);
            break;
        }

        case CHAT_MSG_WHISPER: {
            UserClientMessageChatWhisper msg(language, target, text);
            ClientServices::Send(&msg);
            break;
        }

        case CHAT_MSG_EMOTE: {
            UserClientMessageChatEmote msg(text);
            ClientServices::Send(&msg);
            break;
        }

        case CHAT_MSG_CHANNEL: {
            UserClientMessageChatChannel msg(language, target, text);
            ClientServices::Send(&msg);
            break;
        }

        case CHAT_MSG_AFK: {
            UserClientMessageChatAfk msg(text);
            ClientServices::Send(&msg);
            break;
        }

        case CHAT_MSG_DND: {
            UserClientMessageChatDnd msg(text);
            ClientServices::Send(&msg);
            break;
        }

        case CHAT_MSG_RAID_WARNING: {
            UserClientMessageChatRaidWarning msg(language, text);
            ClientServices::Send(&msg);
            break;
        }

        case CHAT_MSG_BATTLEGROUND:
        case CHAT_MSG_BATTLEGROUND_LEADER: {
            UserClientMessageChatBattleground msg(language, text);
            ClientServices::Send(&msg);
            break;
        }

        default:
            SysMsgPrintf(STATUS_ERROR, "SendChatMessage: No JAM message for chat type: %d.", type);
            break;
    }
}

// OFFSET: 0x008E8E60/0x140617630
void CGChat::RejoinChannels() {
    for (uint32_t i = 0; i < CGChat::s_channels.m_count; i++) {
        ChatChannel* channel = &CGChat::s_channels.m_data[i];

        if (!channel->m_unk9C || !channel->m_unkA0) {
            channel->m_joinPending = 1;
            UserClientChatJoinChannel msg(channel->m_zoneChannelID, channel->m_unk1AC, channel->m_unkA4, channel->m_name, channel->m_password);
            ClientServices::Send(&msg);
        }
    }
}

// OFFSET: 0x008E8F50
int32_t StringToLanguage(const char* string, uint32_t* language) {
    for (int32_t i = 0; i < g_languagesDB.m_numRecords; i++) {
        LanguagesRec* rec = g_languagesDB.GetRecordByIndex(i);

        if (rec && !SStrCmpUTF8I(string, rec->m_name, STORM_MAX_STR)) {
            *language = rec->m_id;
            return 1;
        }
    }

    return 0;
}

// OFFSET: 0x008E5DF0/0x140614140
void CGChat::SendIgnoredResponse(WOWGUID guid, uint8_t a2) {
    UserClientChatReportIgnored msg;
    *reinterpret_cast<uint64_t*>(msg.m_ignoredGUID) = guid.m_guid;
    msg.m_reason = a2;
    ClientServices::Send(&msg);
}

// OFFSET: 0x008ECD60/0x14061C2D0
THROTTLEDCHAT::~THROTTLEDCHAT() {
    SMemFree(this->m_target, __FILE__, __LINE__, 0);
    SMemFree(this->m_text, __FILE__, __LINE__, 0);
}

// OFFSET: 0x008EAFE0/0x140619F30
int32_t CGChat::CanSendChannelMessage(const char* channelName) {
    ChatChannel* channel = GetChannelByName(channelName);

    if (!channel) {
        return 0;
    }

    if (channel->m_flags & 0x8) {
        time_t cutoff = time(NULL) - 30;
        int32_t recent = 0;

        for (uint32_t i = 0; i < sizeof(channel->m_messageTimes) / sizeof(channel->m_messageTimes[0]); i++) {
            if (channel->m_messageTimes[i] > cutoff) {
                recent++;
            }
        }

        if (recent >= 2) {
            return 0;
        }

        channel->m_messageTimes[channel->m_messageTimeIndex++] = time(NULL);

        if (channel->m_messageTimeIndex >= sizeof(channel->m_messageTimes) / sizeof(channel->m_messageTimes[0])) {
            channel->m_messageTimeIndex = 0;
        }
    }

    return 1;
}

// OFFSET: 0x008EEF90/0x14061EE90
void CGChat::QueueChatText(const char* text, int32_t queued, int32_t a3, SLASH_COMMAND_ID type, const ChatMessageParams* params) {
    PENDINGCHAT* chat = CGChat::s_pendingChat.NewNode(2, 0, 0);

    chat->m_type = type;
    chat->m_senderGuid = params->m_senderGuid;
    chat->m_senderUnk = params->m_senderUnk;
    chat->m_text = SStrDupA(text, __FILE__, __LINE__);
    chat->m_senderName = params->m_senderName ? SStrDupA(params->m_senderName, __FILE__, __LINE__) : NULL;
    chat->m_targetName = params->m_targetName ? SStrDupA(params->m_targetName, __FILE__, __LINE__) : NULL;
    chat->m_language = params->m_language;
    chat->m_queued = queued;
    chat->unk_0030 = a3;
    chat->m_achievementID = params->m_achievementID;
    chat->m_isGM = params->m_isGM;
    chat->m_conversation = params->m_conversation ? new (SMemAlloc(sizeof(ChatConversation), __FILE__, __LINE__, 0)) ChatConversation(*params->m_conversation) : NULL;
    chat->m_mobile = params->m_mobile;

    if (params->m_channelName) {
        SStrCopy(chat->m_channelName, params->m_channelName, sizeof(chat->m_channelName));
    } else {
        chat->m_channelName[0] = 0;
    }

    chat->m_targetGuid = params->m_targetGuid;

    if (params->m_flags) {
        SStrCopy(chat->m_flags, params->m_flags, sizeof(chat->m_flags));
    } else {
        chat->m_flags[0] = 0;
    }
}

// OFFSET: 0x008ED830/0x14061D050
int32_t CGChat::IsChannelThrottled(const char* channel, uint32_t time) {
    CHATTHROTTLE* throttle = s_channelThrottle.Ptr(channel);

    if (throttle) {
        uint32_t last = throttle->m_times[throttle->m_index];

        if (last && static_cast<int32_t>(time - last - 10000) < 0) {
            return 1;
        }
    }

    return 0;
}

// OFFSET: 0x008EF250/0x14061F230
void CGChat::AddThrottledChat(int32_t type, uint32_t language, const char* target, const char* text) {
    THROTTLEDCHAT* chat = CGChat::s_throttledChat.NewNode(2, 0, 0);
    chat->m_type = type;
    chat->m_language = language;
    chat->m_target = SStrDupA(target, __FILE__, __LINE__);
    chat->m_text = SStrDupA(text, __FILE__, __LINE__);
}

// OFFSET: 0x008F5E70/0x140627EC0
void CGChat::AddChannelThrottled(const char* channel, uint32_t time) {
    CHATTHROTTLE* throttle = s_channelThrottle.Ptr(channel);

    if (!throttle) {
        throttle = s_channelThrottle.New(channel, 0, 0);
    }

    throttle->m_times[throttle->m_index] = time;
    throttle->m_index = (throttle->m_index + 1) % (sizeof(throttle->m_times) / sizeof(throttle->m_times[0]));
}

// OFFSET: 0x008F5F50/0x140628080
void CGChat::ProcessThrottledChat() {
    if (!CGChat::s_throttledChat.Head()) {
        return;
    }

    uint32_t time = FrameTime::GetCurTimeMs();
    THROTTLEDCHAT* next;

    for (THROTTLEDCHAT* chat = CGChat::s_throttledChat.Head(); !CGChat::s_throttledChat.IsEnd(chat); chat = next) {
        next = CGChat::s_throttledChat.RawNext(chat);

        if (chat->m_type == CHAT_MSG_WHISPER || chat->m_type == CHAT_MSG_BATTLENET) {
            if (!CGChat::IsInWhisperSourceHistory(chat->m_target)) {
                if (CGChat::IsWhisperThrottled(time)) {
                    continue;
                }

                CGChat::AddWhisperThrottled(time);
            }
        } else {
            if (CGChat::IsChannelThrottled(chat->m_target, time)) {
                continue;
            }

            CGChat::AddChannelThrottled(chat->m_target, time);
        }

        CGChat::SendChat(chat->m_language, chat->m_text, chat->m_type, chat->m_target, NULL);
        CGChat::s_throttledChat.DeleteNode(chat);
    }
}

// OFFSET: 0x008F6270/0x140628520
bool ProfanityFilter(char* text, bool cache, bool force) {
    if (!force && !g_profanityFilterCVar->m_intValue) {
        return false;
    }

    if (!*text) {
        return false;
    }

    PROFANITYCHECK* check = s_profanityCache.Ptr(text);

    if (check) {
        if (check->m_filtered) {
            SStrCopy(text, check->m_filteredText, STORM_MAX_STR);
        }

        return check->m_filtered;
    }

    if (cache) {
        check = s_profanityCache.New(text, 0, 0);
    }

    bool filtered = false;
    uint32_t count = g_chatProfanityTokens[s_locale].Count();

    for (uint32_t i = 0; i < count; i++) {
        char* current = text;
        regmatch_t match;

        while (!regexec(&g_chatProfanityTokens[s_locale].m_data[i], current, 1, &match, 0)) {
            if (match.rm_so < 0 || IsInColorCode(text, match.rm_so)) {
                break;
            }

            int32_t length = match.rm_eo - match.rm_so;
            current += match.rm_so;
            filtered = true;

            for (; length > 0; length--) {
                *current++ = s_profanityReplacement[s_profanityReplacementIndex++];

                if (s_profanityReplacementIndex >= sizeof(s_profanityReplacement) - 1) {
                    s_profanityReplacementIndex = 0;
                }
            }
        }
    }

    if (check) {
        check->m_filtered = filtered;

        if (filtered) {
            check->m_filteredText = SStrDupA(text, __FILE__, __LINE__);
        }
    }

    return filtered;
}

// OFFSET: 0x008F63F0/0x140628930
bool SpamFilter(char* text, bool cache) {
    if (!*text) {
        return false;
    }

    SPAMCHECK* check = s_spamCache.Ptr(text);

    if (check) {
        if (check->m_filtered) {
            SStrCopy(text, check->m_filteredText, STORM_MAX_STR);
        }

        return check->m_filtered;
    }

    if (cache) {
        check = s_spamCache.New(text, 0, 0);
    }

    bool spam = false;

    for (uint32_t i = 0; i < g_chatSpamTokens.Count(); i++) {
        const char* current = text;
        regmatch_t match;

        while (!regexec(&g_chatSpamTokens.m_data[i], current, 1, &match, 0)) {
            if (match.rm_so < 0) {
                break;
            }

            current += match.rm_eo;
            spam = true;
        }
    }

    for (uint32_t i = 0; i < g_expectedSpamTokens.Count(); i++) {
        const char* current = text;
        regmatch_t match;

        while (!regexec(&g_expectedSpamTokens.m_data[i], current, 1, &match, 0)) {
            if (match.rm_so < 0) {
                break;
            }

            current += match.rm_eo;
            spam = true;
        }
    }

    if (check) {
        check->m_filtered = spam;

        if (spam) {
            check->m_filteredText = SStrDupA(text, __FILE__, __LINE__);
        }
    }

    return spam;
}

// OFFSET: 0x008F6560/0x140628CD0
int32_t CGChat::AddChatMessage(const char* text, SLASH_COMMAND_ID type, ChatMessageParams* params) {
    if (!text) {
        SErrSetLastError(STORM_ERROR_INVALID_PARAMETER);
        return 0;
    }

    ChatMessageParams defaults;

    if (!params) {
        params = &defaults;
    }

    char senderFullName[305];
    FRAMESCRIPT_GENDER gender = GENDER_NOT_APPLICABLE;

    if (!params->m_senderName) {
        if (!(params->m_senderGuid.m_guid & 0xF000000000000000ULL) && (params->m_senderGuid.m_guid & 0xF07FFFFFFFFFFFFFULL)) {
            NameCache* record = g_nameDBCache.GetRecord(params->m_senderGuid.m_guid, params->m_senderGuid, CGChat::NameQueryCallback, NULL, false);

            if (record) {
                params->m_senderName = record->FullName(senderFullName, sizeof(senderFullName));

                if (record->m_sex == 1) {
                    gender = GENDER_FEMALE;
                }
            }
        } else if ((params->m_senderGuid.m_guid & 0xF0F0000000000000ULL) == 0xF040000000000000ULL) {
            PetNameCache* record = g_petNameCache.GetRecord(static_cast<int32_t>((params->m_senderGuid.m_guid >> 24) & 0xFFFFFFF), params->m_senderGuid, CGChat::NameQueryCallback, NULL, false);

            if (record) {
                params->m_senderName = record->m_name;
            }
        }
    }

    if (params->m_language != 0xFFFFFFFF && (type == CHAT_MSG_WHISPER || type == CHAT_MSG_WHISPER_INFORM)) {
        CGAutoCompleteName::AddFlags(params->m_senderGuid, 0x30, 1, params->m_senderName);
    }

    if (!params->m_targetName) {
        if (!(params->m_targetGuid.m_guid & 0xF000000000000000ULL) && (params->m_targetGuid.m_guid & 0xF07FFFFFFFFFFFFFULL)) {
            NameCache* record = g_nameDBCache.GetRecord(params->m_targetGuid.m_guid, params->m_targetGuid, CGChat::NameQueryCallback, NULL, false);

            if (record) {
                params->m_targetName = record->m_name;
            }
        } else if ((params->m_targetGuid.m_guid & 0xF0F0000000000000ULL) == 0xF040000000000000ULL) {
            PetNameCache* record = g_petNameCache.GetRecord(static_cast<int32_t>((params->m_targetGuid.m_guid >> 24) & 0xFFFFFFF), params->m_targetGuid, CGChat::NameQueryCallback, NULL, false);

            if (record) {
                params->m_targetName = record->m_name;
            }
        }
    }

    if (s_queueChat
        || (params->m_senderGuid && ((!(params->m_senderGuid.m_guid & 0xF000000000000000ULL) && (params->m_senderGuid.m_guid & 0xF07FFFFFFFFFFFFFULL)) || (params->m_senderGuid.m_guid & 0xF0F0000000000000ULL) == 0xF040000000000000ULL) && !params->m_senderName)
        || (params->m_targetGuid && ((!(params->m_targetGuid.m_guid & 0xF000000000000000ULL) && (params->m_targetGuid.m_guid & 0xF07FFFFFFFFFFFFFULL)) || (params->m_targetGuid.m_guid & 0xF0F0000000000000ULL) == 0xF040000000000000ULL) && !params->m_targetName)) {
        CGChat::QueueChatText(text, s_queueChat, 0, type, params);
        return 0;
    }

    if (params->m_language == 0xFFFFFFFF) {
        const char* prefix = params->m_addonPrefix;

        if (prefix && *prefix) {
            switch (type) {
                case CHAT_MSG_PARTY:
                case CHAT_MSG_PARTY_LEADER:
                    params->m_channelName = "PARTY";
                    break;
                case CHAT_MSG_RAID:
                case CHAT_MSG_RAID_LEADER:
                    params->m_channelName = "RAID";
                    break;
                case CHAT_MSG_GUILD:
                    params->m_channelName = "GUILD";
                    break;
                case CHAT_MSG_OFFICER:
                    params->m_channelName = "OFFICER";
                    break;
                case CHAT_MSG_WHISPER:
                case CHAT_MSG_WHISPER_FOREIGN:
                case CHAT_MSG_BATTLENET:
                    params->m_channelName = "WHISPER";
                    break;
                case CHAT_MSG_BATTLEGROUND:
                case CHAT_MSG_BATTLEGROUND_LEADER:
                    params->m_channelName = "BATTLEGROUND";
                    break;
                default:
                    params->m_channelName = "UNKNOWN";
                    break;
            }

            FrameScript_SignalEvent(SCRIPT_EVENT_CHAT_MSG_ADDON, "%s%s%s%s", prefix, text, params->m_channelName, params->m_senderName ? params->m_senderName : "");
        }

        return 0;
    }

    if (!ValidateStringIsUTF8(text)) {
        return 0;
    }

    char questText[3000];

    if ((type == CHAT_MSG_MONSTER_SAY || type == CHAT_MSG_MONSTER_YELL || type == CHAT_MSG_RAID_BOSS_EMOTE || type == CHAT_MSG_RAID_BOSS_WHISPER || type == CHAT_MSG_MONSTER_EMOTE || type == CHAT_MSG_MONSTER_WHISPER || type == CHAT_MSG_MONSTER_PARTY || type == CHAT_MSG_BG_SYSTEM_NEUTRAL || type == CHAT_MSG_BG_SYSTEM_ALLIANCE || type == CHAT_MSG_BG_SYSTEM_HORDE || type == CHAT_MSG_ACHIEVEMENT || type == CHAT_MSG_GUILD_ACHIEVEMENT) && QuestParserParseText(text, questText, sizeof(questText), params->m_targetGuid, params->m_targetName, params->m_achievementID)) {
        text = questText;
    }

    if (type == CHAT_MSG_SAY || type == CHAT_MSG_YELL || type == CHAT_MSG_PARTY || type == CHAT_MSG_PARTY_LEADER) {
        CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(params->m_senderGuid, TYPEMASK_UNIT, __FILE__, __LINE__));

        if (unit && !(unit->m_unit->m_unitFieldFlags & 0x20000000)) {
            char token[32];

            for (int32_t i = 1;;) {
                SStrPrintf(token, sizeof(token), "LAUGH_WORD%d", i++);
                const char* word = FrameScript_GetText(token, -1, GENDER_NOT_APPLICABLE);

                if (!word || !*word) {
                    break;
                }

                if (!SStrCmpUTF8I(word, text, STORM_MAX_STR)) {
                    unit->RequestTalkEmote(4);
                    unit->PlayTextEmoteSound(60, 0xFFFFFFFF);
                    goto emoteDone;
                }
            }

            if (type == CHAT_MSG_SAY || type == CHAT_MSG_YELL) {
                int32_t animation = 0;
                uint32_t length = SStrLen(text);

                if (type == CHAT_MSG_YELL) {
                    animation = 3;
                } else if (length) {
                    if (text[length - 1] == '?') {
                        animation = 1;
                    } else if (text[length - 1] == '!') {
                        animation = 2;
                    }
                }

                unit->RequestTalkEmote(animation);
            }
        }
    }

emoteDone:
    if (type == CHAT_MSG_EMOTE || type == CHAT_MSG_MONSTER_EMOTE || type == CHAT_MSG_SYSTEM || type == CHAT_MSG_RAID_BOSS_EMOTE) {
        params->m_language = 0;
    }

    if (!*text) {
        switch (type) {
            case CHAT_MSG_SAY:
                text = FrameScript_GetText("CHAT_SAY_UNKNOWN", -1, gender);
                type = CHAT_MSG_EMOTE;
                break;
            case CHAT_MSG_YELL:
                text = FrameScript_GetText("CHAT_YELL_UNKNOWN", -1, gender);
                type = CHAT_MSG_EMOTE;
                break;
            case CHAT_MSG_EMOTE:
                text = FrameScript_GetText("CHAT_EMOTE_UNKNOWN", -1, gender);
                break;
        }
    }

    if (!params->m_conversation) {
        for (char* quest = SStrStr(text, "|Hquest:"); quest; quest = SStrStr(quest + 8, "|Hquest:")) {
            if (quest[-1] == '|') {
                continue;
            }

            char* separator = SStrChr(quest, ':');

            if (!separator) {
                continue;
            }

            char* level = SStrChr(separator + 1, ':');

            if (!level) {
                continue;
            }

            int32_t questLevel = SStrToInt(level + 1);
            CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();

            if (player && questLevel == -1) {
                questLevel = player->m_unit->m_unitFieldLevel;
            }

            const char* color = CGTooltip::GetQuestDifficultyColorString(questLevel);

            for (char* dest = quest - 8; *color; color++) {
                *dest++ = *color;
            }
        }
    }

    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player && ((player->m_player->m_playerFlags & 0x8) || (player->m_player->m_playerFlags & 0x80000))) {
        params->m_language = 0;
    }

    char translated[3000];

    if (params->m_language && player) {
        uint32_t skill;
        player->GetLanguageSkill(params->m_language, skill);
        CGChat::TranslateMessage(params->m_language, skill, text, translated, sizeof(translated), 0, 0);
    } else {
        SStrCopy(translated, text, sizeof(translated));
    }

    char channelString[138] = {};
    ChatChannel* channel = NULL;
    uint32_t zoneChannelID = 0;

    if (params->m_channelName && *params->m_channelName) {
        channel = GetChannelByName(params->m_channelName);

        if (channel) {
            zoneChannelID = channel->m_zoneChannelID;
            SStrPrintf(channelString, sizeof(channelString), "%d. %s", channel->m_channelNumber, params->m_channelName);
        } else {
            SStrCopy(channelString, params->m_channelName, sizeof(channelString));
        }

        ProfanityFilter(channelString, false, false);
    }

    if (params->m_conversation && !params->m_conversation->m_inactive) {
        SStrPrintf(channelString, sizeof(channelString), FrameScript_GetText("CONVERSATION_NAME", -1, GENDER_NOT_APPLICABLE), params->m_conversation->m_index + 1);
    }

    bool filterProfanity = false;

    if (g_profanityFilterCVar->m_intValue) {
        for (uint32_t i = 0; i < sizeof(s_profanityFilterTypes) / sizeof(s_profanityFilterTypes[0]); i++) {
            if (type == s_profanityFilterTypes[i]) {
                filterProfanity = true;
                break;
            }
        }
    }

    int32_t spamFilter = g_spamFilterCVar->m_intValue;

    if (type != CHAT_MSG_FILTERED && !IsStaffChatFlag(params->m_flags) && (!player || !(player->m_player->m_playerFlags & 0x8)) && spamFilter && type && SpamFilter(translated, false)) {
        if (type == CHAT_MSG_WHISPER && params->m_senderGuid) {
            UserClientChatReportFiltered msg;
            *reinterpret_cast<uint64_t*>(msg.m_filteredGUID) = params->m_senderGuid.m_guid;
            ClientServices::Send(&msg);
        }

        return 0;
    }

    uint32_t lineID = ++s_chatLineID;
    const char* senderName = params->m_senderName;
    CGChat::RecordChat(translated, type, params->m_senderName, params->m_senderGuid, params->m_senderUnk, zoneChannelID, params->m_channelName, s_chatLineID, player);

    if (filterProfanity) {
        ProfanityFilter(translated, false, false);
    }

    if (!params->m_hideChatLog) {
        SendChatEvent(channel, params->m_language, translated, type, senderName, channelString, params->m_targetName, params->m_flags, lineID, params->m_senderGuid, params->m_conversation, params->m_mobile);
    }

    switch (type) {
        case CHAT_MSG_WHISPER:
            CGLCD::OnWhisper(params->m_senderName);
            break;
        case CHAT_MSG_BN_WHISPER:
            CGLCD::OnWhisper(CGAutoCompleteName::GetDisplayName(params->m_senderName));
            break;
        case CHAT_MSG_RAID_BOSS_WHISPER:
            FrameScript_SignalEvent(SCRIPT_EVENT_RAID_BOSS_WHISPER, "%s%s%f%b", translated, senderName ? senderName : "", params->m_displayTime, params->m_playSound);
            break;
        case CHAT_MSG_RAID_BOSS_EMOTE:
            FrameScript_SignalEvent(SCRIPT_EVENT_RAID_BOSS_EMOTE, "%s%s%f%b", translated, senderName ? senderName : "", params->m_displayTime, params->m_playSound);
            break;
    }

    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(params->m_senderGuid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (unit) {
        char bubbleText[3000];
        const char* bubble = translated;

        if (type == CHAT_MSG_MONSTER_SAY || type == CHAT_MSG_MONSTER_YELL) {
            int32_t source = 0;
            int32_t dest = 0;

            do {
                char c = translated[source];

                if (!c) {
                    break;
                }

                bubbleText[dest] = c;

                if (c == '%' && translated[source + 1] == '%') {
                    source++;
                }

                source++;
                dest++;
            } while (source < static_cast<int32_t>(sizeof(translated)));

            bubbleText[dest] = 0;
            bubble = bubbleText;
        }

        unit->ChatBubbleShow(type, bubble);
    }

    if (!s_chatLogging[0]) {
        return 0;
    }

    char channelPrefix[256];
    char token[64];

    switch (type) {
        case CHAT_MSG_CHANNEL_LIST: {
            if (channelString[0]) {
                SStrPrintf(channelPrefix, sizeof(channelPrefix), FrameScript_GetText("CHAT_CHANNEL_LIST_GET", -1, GENDER_NOT_APPLICABLE), zoneChannelID, params->m_channelName);
            } else {
                channelPrefix[0] = 0;
            }

            SLogWrite(s_chatLog[0], "%s%s", channelPrefix, translated);
            return 0;
        }

        case CHAT_MSG_CHANNEL_NOTICE:
        case CHAT_MSG_CHANNEL_NOTICE_USER: {
            SStrPrintf(token, sizeof(token), "CHAT_%s_NOTICE", translated);
            SLogWrite(s_chatLog[0], FrameScript_GetText(token, -1, GENDER_NOT_APPLICABLE), channelString, senderName, params->m_targetName);
            return 0;
        }

        case CHAT_MSG_IGNORED: {
            SLogWrite(s_chatLog[0], FrameScript_GetText("CHAT_IGNORED", -1, GENDER_NOT_APPLICABLE), senderName);
            return 0;
        }

        case CHAT_MSG_FILTERED: {
            SLogWrite(s_chatLog[0], FrameScript_GetText("CHAT_FILTERED", -1, GENDER_NOT_APPLICABLE), senderName);
            return 0;
        }

        case CHAT_MSG_RESTRICTED: {
            SLogWrite(s_chatLog[0], FrameScript_GetText("CHAT_RESTRICTED", -1, GENDER_NOT_APPLICABLE));
            return 0;
        }

        case CHAT_MSG_BN_CONVERSATION_NOTICE: {
            char noticeToken[128];
            SStrPrintf(noticeToken, sizeof(noticeToken), "CHAT_CONVERSATION_%s_NOTICE", translated);
            SLogWrite(s_chatLog[0], FrameScript_GetText(noticeToken, -1, GENDER_NOT_APPLICABLE), channelString, senderName);
            return 0;
        }

        default: {
            if (channelString[0]) {
                SStrPrintf(channelPrefix, sizeof(channelPrefix), "[%s] ", channelString);
                char* dash = SStrStr(channelPrefix, " - ");

                if (dash) {
                    SStrCopy(dash, "] ", STORM_MAX_STR);
                }
            } else {
                channelPrefix[0] = 0;
            }

            SStrPrintf(token, sizeof(token), "CHAT_%s_GET", static_cast<const char*>(s_chatTypeInfo[type].m_name));

            char header[304];
            SStrPrintf(header, sizeof(header), FrameScript_GetText(token, -1, GENDER_NOT_APPLICABLE), senderName ? senderName : "");

            if (params->m_language && player && params->m_language != player->GetDefaultLanguage()) {
                char languageName[256];
                SStrPrintf(languageName, sizeof(languageName), "[%s] ", static_cast<const char*>(g_languagesDB.GetRecord(params->m_language)->m_name));
                SStrPack(header, languageName, sizeof(header));
            }

            const char* source = LanguageProcess(translated);
            char line[6560];
            SStrPrintf(line, sizeof(line), "%s%s", channelPrefix, header);
            char* dest = &line[SStrLen(line)];

            while (source) {
                char c = *source;

                if (!c || dest >= &line[sizeof(line) - 1]) {
                    break;
                }

                if (c == '|') {
                    if (source[1] == '|') {
                        *dest++ = '|';
                        source += 2;
                        continue;
                    }

                    source = SStrChr(source, '[');

                    if (!source) {
                        break;
                    }

                    while (*source) {
                        c = *source;

                        if (c != '[') {
                            if (c == ']') {
                                source++;

                                if (!SStrCmp(source, "|h", 2)) {
                                    source += 2;
                                }

                                if (!SStrCmp(source, "|r", 2)) {
                                    source += 2;
                                }

                                break;
                            }

                            *dest++ = c;
                        }

                        source++;
                    }
                } else {
                    if (c == '%' && type != CHAT_MSG_MONSTER_SAY && type != CHAT_MSG_MONSTER_YELL && type != CHAT_MSG_RAID_BOSS_EMOTE && type != CHAT_MSG_RAID_BOSS_WHISPER && type != CHAT_MSG_MONSTER_EMOTE && type != CHAT_MSG_MONSTER_WHISPER && type != CHAT_MSG_MONSTER_PARTY) {
                        *dest++ = '%';
                        *dest = '%';
                    } else {
                        *dest = c;
                    }

                    dest++;
                    source++;
                }
            }

            *dest = 0;
            SLogWrite(s_chatLog[0], line, senderName ? senderName : "");
            return 0;
        }
    }
}

// OFFSET: 0x008F8770/0x14062B9C0
int32_t CGChat::ChatMessageReceived(CDataStore* msg, bool isGM) {
    char channelName[128];
    memset(channelName, 0, sizeof(channelName));

    const char* senderName = NULL;
    const char* targetName = NULL;
    char addonPrefix[17] = {};
    uint8_t chatFlags = 0;
    const char* flags = "";
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));
    int32_t achievementID = 0;
    bool mobile = false;
    uint8_t hideChatLog = 0;
    float displayTime = 0.0f;
    WOWGUID senderGuid = WOWGUID::Make(0);
    uint32_t senderUnk = 0;
    WOWGUID targetGuid = WOWGUID::Make(0);
    uint8_t type;
    uint32_t language;
    uint32_t length;
    uint32_t textLength;
    void* data;
    const char* text;

    msg->Get(type);
    msg->Get(language);
    msg->Get(senderGuid.m_guid);
    msg->Get(senderUnk);

    if (type == CHAT_MSG_MONSTER_SAY || type == CHAT_MSG_MONSTER_YELL || type == CHAT_MSG_RAID_BOSS_EMOTE || type == CHAT_MSG_RAID_BOSS_WHISPER || type == CHAT_MSG_MONSTER_EMOTE || type == CHAT_MSG_MONSTER_WHISPER || type == CHAT_MSG_MONSTER_PARTY || type == CHAT_MSG_BATTLENET) {
        msg->Get(length);
        msg->GetDataInSitu(data, length);
        senderName = static_cast<const char*>(data);
        msg->Get(targetGuid.m_guid);

        if (targetGuid && ((targetGuid.m_guid & 0xF000000000000000ULL) || !(targetGuid.m_guid & 0xF07FFFFFFFFFFFFFULL)) && (targetGuid.m_guid & 0xF0F0000000000000ULL) != 0xF040000000000000ULL) {
            msg->Get(length);
            msg->GetDataInSitu(data, length);
            targetName = static_cast<const char*>(data);
        }

        if (language == 0xFFFFFFFF) {
            msg->GetString(addonPrefix, sizeof(addonPrefix));
        }

        msg->Get(textLength);
        msg->GetDataInSitu(data, textLength);
        text = static_cast<const char*>(data);
        msg->Get(chatFlags);

        if (type == CHAT_MSG_RAID_BOSS_EMOTE || type == CHAT_MSG_RAID_BOSS_WHISPER) {
            msg->Get(displayTime);
            msg->Get(hideChatLog);
        }

        if (FriendList::s_friendList && FriendList::s_friendList->IsIgnored(targetGuid, true)) {
            return 1;
        }
    } else if (type == CHAT_MSG_WHISPER_FOREIGN) {
        msg->Get(length);
        msg->GetDataInSitu(data, length);
        senderName = static_cast<const char*>(data);
        msg->Get(targetGuid.m_guid);

        if (language == 0xFFFFFFFF) {
            msg->GetString(addonPrefix, sizeof(addonPrefix));
        }

        msg->Get(textLength);
        msg->GetDataInSitu(data, textLength);
        text = static_cast<const char*>(data);
        msg->Get(chatFlags);

        if (FriendList::s_friendList && FriendList::s_friendList->IsIgnored(senderGuid, true)) {
            return 1;
        }

        type = CHAT_MSG_WHISPER;
    } else if (type == CHAT_MSG_BG_SYSTEM_NEUTRAL || type == CHAT_MSG_BG_SYSTEM_ALLIANCE || type == CHAT_MSG_BG_SYSTEM_HORDE) {
        msg->Get(targetGuid.m_guid);

        if (targetGuid && ((targetGuid.m_guid & 0xF000000000000000ULL) || !(targetGuid.m_guid & 0xF07FFFFFFFFFFFFFULL))) {
            msg->Get(length);
            msg->GetDataInSitu(data, length);
            targetName = static_cast<const char*>(data);
        }

        if (language == 0xFFFFFFFF) {
            msg->GetString(addonPrefix, sizeof(addonPrefix));
        }

        msg->Get(textLength);
        msg->GetDataInSitu(data, textLength);
        text = static_cast<const char*>(data);
        msg->Get(chatFlags);
    } else if (type == CHAT_MSG_ACHIEVEMENT || type == CHAT_MSG_GUILD_ACHIEVEMENT) {
        msg->Get(targetGuid.m_guid);

        if (language == 0xFFFFFFFF) {
            msg->GetString(addonPrefix, sizeof(addonPrefix));
        }

        msg->Get(textLength);
        msg->GetDataInSitu(data, textLength);
        text = static_cast<const char*>(data);
        msg->Get(chatFlags);
        msg->Get(reinterpret_cast<uint32_t&>(achievementID));
    } else {
        if (isGM) {
            msg->Get(length);
            msg->GetDataInSitu(data, length);
            senderName = static_cast<const char*>(data);
        }

        if (type == CHAT_MSG_CHANNEL) {
            msg->GetString(channelName, sizeof(channelName));
        }

        msg->Get(targetGuid.m_guid);

        if (language == 0xFFFFFFFF) {
            msg->GetString(addonPrefix, sizeof(addonPrefix));
        }

        msg->Get(textLength);
        msg->GetDataInSitu(data, textLength);
        text = static_cast<const char*>(data);
        msg->Get(chatFlags);

        if (chatFlags & 0x10) {
            flags = "DEV";
        } else if (chatFlags & 0x4) {
            flags = "GM";
        } else if (chatFlags & 0x80) {
            flags = "COM";
        } else if (chatFlags & 0x2) {
            flags = "DND";
        } else if (chatFlags & 0x1) {
            flags = "AFK";
        }

        if (chatFlags & 0x40) {
            mobile = true;
        }

        if (type == CHAT_MSG_WHISPER || type == CHAT_MSG_BATTLENET) {
            CGChat::RecordWhisperSource(senderGuid);
        } else if (type == CHAT_MSG_WHISPER_INFORM) {
            CGChat::RecordWhisperDest(senderGuid);
        }

        if ((type == CHAT_MSG_WHISPER || type == CHAT_MSG_BATTLENET) && (chatFlags & 0x8) && !(chatFlags & 0x4) && language != 0xFFFFFFFF && (!player || senderGuid.m_guid != player->m_obj->m_guid.m_guid) && (!FriendList::s_friendList || !FriendList::s_friendList->IsFriend(senderGuid)) && !CGChat::IsInWhisperHistory(senderGuid)) {
            CGChat::SendIgnoredResponse(senderGuid, 1);
            return 1;
        }

        if (FriendList::s_friendList && FriendList::s_friendList->IsIgnored(senderGuid, true) && !(chatFlags & 0x4)) {
            bool ignore = true;

            if (type == CHAT_MSG_WHISPER) {
                if (language != 0xFFFFFFFF) {
                    CGChat::SendIgnoredResponse(senderGuid, 0);
                }
            } else if (type == CHAT_MSG_WHISPER_INFORM || type == CHAT_MSG_IGNORED || type == CHAT_MSG_RESTRICTED) {
                ignore = false;
            }

            if (ignore) {
                char buffer[256];
                SStrPrintf(buffer, sizeof(buffer), "Ignoring msg [%s] from %016llX", text, senderGuid.m_guid);
                SysMsgPrintf(STATUS_WARNING, buffer);
                return 1;
            }
        }
    }

    if (!msg->IsRead()) {
        msg->Reset();
        return 1;
    }

    ChatMessageParams params;
    params.m_senderName = senderName;
    params.m_language = language;
    params.m_channelName = channelName;
    params.m_addonPrefix = addonPrefix;
    params.m_targetName = targetName;
    params.m_flags = flags;
    params.m_senderGuid = senderGuid;
    params.m_senderUnk = senderUnk;
    params.m_targetGuid = targetGuid;
    params.m_achievementID = achievementID;
    params.m_isGM = isGM;
    params.m_playSound = (chatFlags & 0x20) != 0;
    params.m_hideChatLog = hideChatLog != 0;
    params.m_mobile = mobile;
    params.m_conversation = NULL;
    params.m_displayTime = displayTime;
    CGChat::AddChatMessage(text, static_cast<SLASH_COMMAND_ID>(type), &params);
    return 1;
}

// OFFSET: 0x008FBC30
int32_t CGChat::OnMessageChat(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    return CGChat::ChatMessageReceived(msg, false);
}

// OFFSET: 0x008EC380/0x14061B660
static void FlushAddonPrefixes() {
    if (s_addonPrefixMessage.m_prefixes.m_count > 0) {
        ClientServices::Send(&s_addonPrefixMessage);
        s_addonPrefixMessage.m_prefixes.Clear();
    }
}

// OFFSET: 0x008ED9B0/0x14061D230
static void SendAddonPrefixes() {
    UserClientChatUnregisterAllAddonPrefixes clear;
    ClientServices::Send(&clear);

    for (ADDONPREFIX* prefix = s_addonPrefixes.Head(); prefix; prefix = s_addonPrefixes.Next(prefix)) {
        if (s_addonPrefixMessage.m_prefixes.m_count == 64) {
            ClientServices::Send(&s_addonPrefixMessage);
            s_addonPrefixMessage.m_prefixes.Clear();
        }

        JamCliStructAddonPrefix* entry = s_addonPrefixMessage.m_prefixes.New();
        SStrCopy(entry->m_prefix, prefix->m_key.m_str, sizeof(entry->m_prefix));
    }

    if (s_addonPrefixMessage.m_prefixes.m_count > 0) {
        ClientServices::Send(&s_addonPrefixMessage);
        s_addonPrefixMessage.m_prefixes.Clear();
    }
}

// OFFSET: 0x008EEAB0/0x14061E690
void CGChat::Shutdown() {
    s_queueAddonPrefixes = 1;
    s_addonPrefixMessage.m_prefixes.Clear();
    s_addonPrefixes.Clear();
    s_numAddonPrefixes = 0;
    s_pendingReportTarget.m_guid = 0;
}

// OFFSET: 0x008EEB00/0x14061E790
int32_t CGChat::OnChatReconnect(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    msg->m_read = msg->m_size;
    s_chatServerDown = 0;
    CGChat::RejoinChannels();
    SendAddonPrefixes();
    FrameScript_SignalEvent(SCRIPT_EVENT_CHAT_SERVER_RECONNECTED, NULL);
    return 1;
}

// OFFSET: 0x008F6080/0x140628200
bool RegisterAddonMessagePrefix(const char* prefix) {
    if (s_addonPrefixes.Ptr(prefix)) {
        return true;
    }

    if (s_numAddonPrefixes == 512) {
        return false;
    }

    s_addonPrefixes.New(prefix, 0, 0);
    s_numAddonPrefixes++;

    if (s_queueAddonPrefixes) {
        if (s_addonPrefixMessage.m_prefixes.m_count == 64) {
            FlushAddonPrefixes();
        }

        JamCliStructAddonPrefix* entry = s_addonPrefixMessage.m_prefixes.New();
        SStrCopy(entry->m_prefix, prefix, sizeof(entry->m_prefix));

        if (s_addonPrefixMessage.m_prefixes.m_count == 64) {
            FlushAddonPrefixes();
        }
    } else {
        UserClientChatRegisterAddonPrefixes msg;
        msg.m_prefixes.Clear();
        JamCliStructAddonPrefix* entry = msg.m_prefixes.New();
        SStrCopy(entry->m_prefix, prefix, sizeof(entry->m_prefix));
        ClientServices::Send(&msg);
    }

    return true;
}

// OFFSET: 0x008FBC50
int32_t CGChat::OnGMMessageChat(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    return CGChat::ChatMessageReceived(msg, true);
}

// OFFSET: 0x008E6310/0x1406145B0
int32_t StringToChatType(const char* string, int32_t* type) {
    struct {
        int32_t type;
        const char* name;
    } types[] = {
        { CHAT_MSG_SAY, "SAY" },
        { CHAT_MSG_PARTY, "PARTY" },
        { CHAT_MSG_RAID, "RAID" },
        { CHAT_MSG_GUILD, "GUILD" },
        { CHAT_MSG_OFFICER, "OFFICER" },
        { CHAT_MSG_YELL, "YELL" },
        { CHAT_MSG_WHISPER, "WHISPER" },
        { CHAT_MSG_EMOTE, "EMOTE" },
        { CHAT_MSG_CHANNEL, "CHANNEL" },
        { CHAT_MSG_AFK, "AFK" },
        { CHAT_MSG_DND, "DND" },
        { CHAT_MSG_RAID_WARNING, "RAID_WARNING" },
        { CHAT_MSG_BATTLEGROUND, "BATTLEGROUND" },
        { CHAT_MSG_BATTLENET, "BN" },
        { CHAT_MSG_BN_WHISPER, "BN_WHISPER" },
        { CHAT_MSG_BN_WHISPER_INFORM, "BN_WHISPER_INFORM" },
        { CHAT_MSG_BN_CONVERSATION, "BN_CONVERSATION" },
    };

    for (uint32_t i = 0; i < sizeof(types) / sizeof(types[0]); i++) {
        if (!SStrCmpI(types[i].name, string, STORM_MAX_STR)) {
            *type = types[i].type;
            return 1;
        }
    }

    return 0;
}

// OFFSET: 0x008E5BC0/0x140613EE0
static void TranslateMessageCase(const char* word, char* dest, uint32_t size, const char* source) {
    size--;

    while (*dest && size) {
        dest++;
        size--;
    }

    if (s_locale == 1 || (s_locale > 3 && s_locale <= 5)) {
        for (char c = *word; c && *source && size; c = *++word) {
            char s = *source++;

            if (static_cast<uint8_t>(s - 'A') <= 25) {
                *dest = static_cast<char>(ToUpper(c));
            } else {
                *dest = c;
            }

            dest++;
            size--;
        }
    } else {
        while (*word && *source && size) {
            int32_t chars;
            uint32_t c = sgetu8(reinterpret_cast<const uint8_t*>(source), &chars);

            if (c == 0xFFFFFFFF) {
                break;
            }

            *dest = static_cast<char>(IsUpper(static_cast<uint16_t>(c)) ? ToUpper(*word) : ToLower(*word));
            source += chars;
            word++;
            dest++;
            size--;
        }
    }

    *dest = 0;
}

// OFFSET: 0x008E8600/0x140616DA0
int32_t CGChat::GetChannelID(const char* name) {
    for (uint32_t i = 0; i < CGChat::s_channels.Count(); i++) {
        if (!SStrCmpUTF8I(CGChat::s_channels.m_data[i].m_name, name, STORM_MAX_STR) && !CGChat::s_channels.m_data[i].m_unk9C) {
            return CGChat::s_channels.m_data[i].m_channelNumber;
        }
    }

    return 0;
}

// OFFSET: 0x008E87A0/0x140616F60
ChatChannel* CGChat::GetChannelByChatChannelID(uint32_t id) {
    for (uint32_t i = 0; i < CGChat::s_channels.Count(); i++) {
        if (CGChat::s_channels.m_data[i].m_zoneChannelID == id) {
            return &CGChat::s_channels.m_data[i];
        }
    }

    return NULL;
}

// OFFSET: 0x008E5D10/0x140614030
int32_t CGChat::OnChannelMemberCount(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    char channel[128];
    uint8_t flags;
    uint32_t count;
    msg->GetString(channel, sizeof(channel));
    msg->Get(flags);
    msg->Get(count);
    return 1;
}

// OFFSET: 0x008E5D90
void CGChat::InviteNameQueryCallback(int32_t id, const WOWGUID& guid, void* param, bool success) {
    WOWGUID requester = WOWGUID();
    NameCache* record = g_nameDBCache.GetRecord(guid.m_guid, requester, NULL, NULL, false);

    if (record) {
        FrameScript_SignalEvent(SCRIPT_EVENT_CHANNEL_INVITE_REQUEST, "%s%s", s_inviteChannelName, static_cast<const char*>(record->m_name));
        s_inviteChannelName[0] = 0;
    }
}

// OFFSET: 0x008E5E30/0x140614190
int32_t CGChat::OnComsatReconnectAttempt(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    CGGameUI::DisplayError(ERR_COMSAT_RECONNECT_ATTEMPT);
    return 1;
}

// OFFSET: 0x008E5E50/0x1406141B0
int32_t CGChat::OnComsatDisconnect(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    CGGameUI::DisplayError(ERR_COMSAT_DISCONNECT);
    return 1;
}

// OFFSET: 0x008E5E70/0x1406141D0
int32_t CGChat::OnComsatConnectFail(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    CGGameUI::DisplayError(ERR_COMSAT_CONNECT_FAIL);
    return 1;
}

// OFFSET: 0x008E5E90/0x1406141F0
int32_t CGChat::OnVoicesessionFull(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    CGGameUI::DisplayError(ERR_VOICESESSION_FULL);
    return 1;
}

// OFFSET: 0x008E61C0/0x140614420
int32_t CGChat::OnUpdateInstanceOwnership(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    int32_t owns;
    msg->Get(reinterpret_cast<uint32_t&>(owns));
    UpdateInstanceOwnershipPacket(owns);
    return 1;
}

// OFFSET: 0x008E8CF0/0x1406174A0
int32_t CGChat::OnUpdateInstance(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    int32_t mapId;
    msg->Get(reinterpret_cast<uint32_t&>(mapId));

    if (g_mapDB.GetRecord(mapId)) {
        uint32_t now = static_cast<uint32_t>(::time(NULL));
        CGGameUI::SetLastInstanceTime(now, mapId, ClntObjMgrGetMapID());
    }

    return 1;
}

// OFFSET: 0x008ECEB0/0x14061C470
void ChatWindowInfo::Init() {
    CImVector color(0, 0, 0, 0x28);

    this->m_name[0] = 0;
    this->m_unkD0 = 0;
    this->m_unkD8 = 1;
    this->m_docked = 0;
    this->m_shown = 0;
    this->m_unkE4 = 0;
    this->m_color = color;
    this->m_channels.SetCount(10);
    this->m_channelIDs.SetCount(10);

    for (uint32_t i = 0; i < 10; i++) {
        this->m_channels.m_data[i] = NULL;
        this->m_channelIDs.m_data[i] = 0;
    }

    this->m_unkE8 = 6;
    this->m_unkEC = 0.0f;
    this->m_unkF0 = 0.0f;
    this->m_unkF4 = 0;
    this->m_width = 430.0f;
    this->m_height = 120.0f;
}

// OFFSET: 0x008ED870
int32_t CGChat::OnCheckReloadSpamFilter(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    uint32_t count;
    msg->Get(count);
    g_expectedSpamTokens.SetCount(count);

    uint32_t valid = 0;

    for (int32_t i = 0; i < static_cast<int32_t>(count); i++) {
        char expression[512];
        msg->GetString(expression, sizeof(expression));

        if (!regcomp(&g_expectedSpamTokens.m_data[valid], expression, REG_ICASE)) {
            valid++;
        }
    }

    g_expectedSpamTokens.SetCount(valid);
    return 1;
}

// OFFSET: 0x008ED2A0/0x14061C900
void CGChat::TranslateMessage(uint32_t language, uint32_t skill, const char* text, char* buffer, uint32_t size, int32_t a6, int32_t a7) {
    if (!language) {
        SStrCopy(buffer, text, size);
        return;
    }

    if (!ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__) || skill >= 300) {
        SStrCopy(buffer, text, size);
        return;
    }

    char stripped[3000];
    StripHyperlinks(text, stripped, sizeof(stripped));

    int32_t syllables = s_locale == 4 || s_locale == 5 ? 2 : 0;
    *buffer = 0;

    const char* current = stripped;
    char word[1024];
    int32_t chars;

    while (true) {
        bool spaced = false;

        for (uint32_t c = sgetu8(reinterpret_cast<const uint8_t*>(current), &chars); c != 0xFFFFFFFF; c = sgetu8(reinterpret_cast<const uint8_t*>(current), &chars)) {
            if (IsAlphaLatin1(static_cast<uint16_t>(c)) || IsNumeric(static_cast<uint16_t>(c)) || c == '\'' || c > 0xFF) {
                break;
            }

            if (c == '<' && a6) {
                for (uint32_t t = sgetu8(reinterpret_cast<const uint8_t*>(current), &chars); t != 0xFFFFFFFF; t = sgetu8(reinterpret_cast<const uint8_t*>(current), &chars)) {
                    int32_t i;

                    for (i = 0; i < chars; i++) {
                        word[i] = current[i];
                    }

                    word[i] = 0;
                    SStrPack(buffer, word, size);
                    current += chars;

                    if (t == '>') {
                        break;
                    }
                }
            } else if (a7) {
                int32_t i;

                for (i = 0; i < chars; i++) {
                    word[i] = current[i];
                }

                word[i] = 0;
                SStrPack(buffer, word, size);
                current += chars;
            } else {
                if (!spaced) {
                    SStrPack(buffer, " ", size);
                    spaced = true;
                }

                current += chars;
            }
        }

        if (sgetu8(reinterpret_cast<const uint8_t*>(current), &chars) == 0xFFFFFFFF) {
            return;
        }

        const char* end = current;
        const char* next = current;
        int32_t count = 0;

        for (uint32_t c = sgetu8(reinterpret_cast<const uint8_t*>(end), &chars); c != 0xFFFFFFFF; c = sgetu8(reinterpret_cast<const uint8_t*>(end), &chars)) {
            if (!IsAlphaLatin1(static_cast<uint16_t>(c)) && !IsNumeric(static_cast<uint16_t>(c)) && c != '\'' && c <= 0xFF) {
                break;
            }

            end += chars;

            if (syllables > 0 && ++count >= syllables) {
                SStrPack(buffer, " ", size);

                if (++syllables > 5) {
                    syllables = 2;
                }

                break;
            }
        }

        next = end;
        uint32_t length = end - current;

        if (length >= sizeof(word)) {
            length = sizeof(word);
        }

        SStrCopy(word, current, length + 1);
        uint32_t hash = SStrHash(word, 0, 0);

        if (hash % 300 < skill) {
            SStrPack(buffer, word, size);
        } else {
            if (length >= 18) {
                length = 18;
            }

            HASHKEY_LANGUAGE key;
            key.m_length = length;
            key.m_unk04 = 0;
            WORDLIST* list = s_wordLists.Ptr(language, key);

            while (!list && length != 1) {
                key.m_length = --length;
                key.m_unk04 = 0;
                list = s_wordLists.Ptr(language, key);
            }

            if (list) {
                TranslateMessageCase(list->m_words.m_data[hash % list->m_words.Count()]->m_word, buffer, size, word);
            }
        }

        current = next;
    }
}

// OFFSET: 0x008F40E0/0x140625E60
void CGChat::JoinedChannel(uint32_t id, const char* name, uint32_t a3, uint8_t flags) {
    ChatChannel* channel = id ? CGChat::GetChannelByChatChannelID(id) : GetChannelByName(name);

    if (!channel || !channel->m_channelNumber) {
        return;
    }

    uint32_t zoneChannelID = channel->m_zoneChannelID;
    channel->m_unk9C = 0;

    if (zoneChannelID) {
        s_joinedZoneChannels |= 1 << (zoneChannelID - 1);
        SStrCopy(channel->m_name, name, sizeof(channel->m_name));
    }

    channel->m_unk98 = a3;
    channel->m_flags = flags;
    CGChat::OnChannelsUpdated();
}

// OFFSET: 0x008F4170/0x140625F50
void CGChat::LeftChannel(uint32_t id, const char* name) {
    ChatChannel* channel = id ? CGChat::GetChannelByChatChannelID(id) : GetChannelByName(name);

    if (!channel) {
        return;
    }

    channel->m_unk9C = 1;

    if (s_activeVoiceChannel) {
        int32_t type;
        const char* voiceChannel = CGChat::GetChannelStringForDisplay(s_activeVoiceChannel, &type);

        if (!type && voiceChannel && !SStrCmpUTF8I(channel->m_name, voiceChannel, STORM_MAX_STR)) {
            if (CGChat::s_displayChannels[s_activeVoiceChannel].m_voiceEnabled) {
                FrameScript_SignalEvent(SCRIPT_EVENT_CHANNEL_VOICE_UPDATE, "%d%d", s_activeVoiceChannel + 1, CGChat::s_displayChannels[s_activeVoiceChannel].m_voiceEnabled);
            } else {
                FrameScript_SignalEvent(SCRIPT_EVENT_CHANNEL_VOICE_UPDATE, "%d", s_activeVoiceChannel + 1);
            }

            s_activeVoiceChannel = 0;
        }
    }

    if (!s_activeDisplayChannelType && !SStrCmpUTF8I(channel->m_name, CGChat::s_activeDisplayChannelName, STORM_MAX_STR)) {
        memset(CGChat::s_activeDisplayChannelName, 0, sizeof(CGChat::s_activeDisplayChannelName));
        s_activeDisplayChannelType = 4;
        s_pendingUsers.Clear();
        s_activeDisplayChannelFlags = 0;
    }

    CGChat::OnChannelsUpdated();
}

// OFFSET: 0x008F42A0
void CGChat::ResetChannel(uint32_t id, const char* name) {
    ChatChannel* channel = id ? CGChat::GetChannelByChatChannelID(id) : GetChannelByName(name);

    if (channel) {
        channel->m_channelNumber = 0;
        channel->m_name[0] = 0;
        channel->m_zoneChannelID = 0;
        channel->m_unk9C = 1;
        channel->m_unkA0 = 1;
    }

    CGChat::OnChannelsUpdated();
}

// OFFSET: 0x008F78B0/0x14062A780
int32_t CGChat::OnServerFirstAchievement(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    char name[96];
    WOWGUID guid;
    uint32_t achievementID;
    uint32_t linked;
    char text[2048];

    msg->GetString(name, sizeof(name));
    msg->Get(guid.m_guid);
    msg->Get(achievementID);
    msg->Get(linked);

    if (linked) {
        SStrPrintf(text, sizeof(text), FrameScript_GetText("PLAYER_SERVER_FIRST_ACHIEVEMENT", -1, GENDER_NOT_APPLICABLE), name, name);
    } else {
        SStrPrintf(text, sizeof(text), FrameScript_GetText("SERVER_FIRST_ACHIEVEMENT", -1, GENDER_NOT_APPLICABLE), name);
    }

    ChatMessageParams params;
    params.m_senderName = name;
    params.m_senderGuid = guid;
    params.m_targetGuid = guid;
    params.m_achievementID = achievementID;
    CGChat::AddChatMessage(text, CHAT_MSG_ACHIEVEMENT, &params);
    return 1;
}

// OFFSET: 0x008F8E20
int32_t CGChat::OnZoneUnderAttack(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    int32_t areaId;
    msg->Get(reinterpret_cast<uint32_t&>(areaId));

    AreaTableRec* area = g_areaTableDB.GetRecord(areaId);

    if (!area) {
        return 1;
    }

    char text[3000];
    SStrPrintf(text, sizeof(text), FrameScript_GetText("ZONE_UNDER_ATTACK", -1, GENDER_NOT_APPLICABLE), static_cast<const char*>(area->m_areaName));

    if (area->m_flags & 0x40000000) {
        areaId = area->m_parentAreaID;
    }

    for (uint32_t i = 0; i < CGChat::s_channels.Count(); i++) {
        ChatChannel* channel = &CGChat::s_channels.m_data[i];

        if (!channel->m_channelNumber || channel->m_unk9C) {
            continue;
        }

        ChatChannelsRec* rec = g_chatChannelsDB.GetRecord(channel->m_zoneChannelID);

        if (rec && (rec->m_flags & 0x10000) && (!(rec->m_flags & 0x2) || areaId == CGGameUI::m_areaID)) {
            ChatMessageParams params;
            params.m_channelName = channel->m_name;
            CGChat::AddChatMessage(text, CHAT_MSG_CHANNEL, &params);
        }
    }

    return 1;
}

// OFFSET: 0x008F92D0/0x14062C7E0
int32_t CGChat::OnDurabilityDamageDeath(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    uint32_t percent = 0;
    msg->Get(percent);
    char text[3000];
    SStrPrintf(text, sizeof(text), FrameScript_GetText("DURABILITYDAMAGE_DEATH", -1, GENDER_NOT_APPLICABLE), percent);
    CGChat::AddChatMessage(text, CHAT_MSG_COMBAT_MISC_INFO, NULL);
    return 1;
}

// OFFSET: 0x008F9330/0x14062C850
int32_t CGChat::OnDefenseMessage(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    int32_t areaId;
    uint32_t length;
    void* data;

    msg->Get(reinterpret_cast<uint32_t&>(areaId));
    msg->Get(length);
    msg->GetDataInSitu(data, length);
    const char* text = static_cast<const char*>(data);

    AreaTableRec* area = g_areaTableDB.GetRecord(areaId);

    if (area && (area->m_flags & 0x40000000)) {
        areaId = area->m_parentAreaID;
    }

    for (uint32_t i = 0; i < CGChat::s_channels.Count(); i++) {
        ChatChannel* channel = &CGChat::s_channels.m_data[i];

        if (!channel->m_channelNumber || channel->m_unk9C) {
            continue;
        }

        ChatChannelsRec* rec = g_chatChannelsDB.GetRecord(channel->m_zoneChannelID);

        if (rec && (rec->m_flags & 0x10000) && (!(rec->m_flags & 0x2) || areaId == CGGameUI::m_areaID)) {
            ChatMessageParams params;
            params.m_channelName = channel->m_name;
            CGChat::AddChatMessage(text, CHAT_MSG_CHANNEL, &params);
        }
    }

    return 1;
}

// OFFSET: 0x008F94A0
int32_t CGChat::OnServerMessage(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    int32_t id;
    char argument[1024];
    char text[1024];

    msg->Get(reinterpret_cast<uint32_t&>(id));
    msg->GetString(argument, sizeof(argument));

    ServerMessagesRec* rec = g_serverMessagesDB.GetRecord(id);

    if (rec) {
        if (argument[0]) {
            SStrPrintf(text, sizeof(text), static_cast<const char*>(rec->m_text), argument);
        } else {
            SStrCopy(text, rec->m_text, sizeof(text));
        }
    } else {
        SStrPrintf(text, sizeof(text), "[%d]: %s", id, argument);
    }

    CGChat::AddChatMessage(text, CHAT_MSG_SYSTEM, NULL);
    CKBSystem::CacheServerMessage(text, id);
    return 1;
}

// OFFSET: 0x008F95A0/0x14062CBA0
int32_t CGChat::OnRaidInstanceMessage(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    int32_t type;
    int32_t mapId;
    int32_t difficulty;
    int32_t seconds;
    char mapName[256];
    char text[3000];

    msg->Get(reinterpret_cast<uint32_t&>(type));
    msg->Get(reinterpret_cast<uint32_t&>(mapId));
    msg->Get(reinterpret_cast<uint32_t&>(difficulty));
    msg->Get(reinterpret_cast<uint32_t&>(seconds));
    CGRaidInfo::GetMapNameWithDifficulty(mapName, sizeof(mapName), mapId, difficulty, 0);

    switch (type) {
        case 1: {
            SStrPrintf(text, sizeof(text), FrameScript_GetText("RAID_INSTANCE_WARNING_HOURS", -1, GENDER_NOT_APPLICABLE), mapName, seconds / 3600);
            CGChat::AddChatMessage(text, CHAT_MSG_SYSTEM, NULL);
            break;
        }

        case 2: {
            SStrPrintf(text, sizeof(text), FrameScript_GetText("RAID_INSTANCE_WARNING_MIN", -1, GENDER_NOT_APPLICABLE), mapName, seconds / 60);
            CGChat::AddChatMessage(text, CHAT_MSG_SYSTEM, NULL);
            break;
        }

        case 3: {
            SStrPrintf(text, sizeof(text), FrameScript_GetText("RAID_INSTANCE_WARNING_MIN_SOON", -1, GENDER_NOT_APPLICABLE), mapName, seconds / 60);
            CGChat::AddChatMessage(text, CHAT_MSG_SYSTEM, NULL);
            break;
        }

        case 4: {
            uint32_t locked;
            uint32_t extended;
            msg->GetInt8(locked);
            msg->GetInt8(extended);
            FrameScript_SignalEvent(SCRIPT_EVENT_RAID_INSTANCE_WELCOME, "%s%d%d%d", mapName, seconds, locked != 0, extended != 0);
            break;
        }

        case 5: {
            SStrPrintf(text, sizeof(text), FrameScript_GetText("RAID_INSTANCE_EXPIRED", -1, GENDER_NOT_APPLICABLE), mapName);
            CGChat::AddChatMessage(text, CHAT_MSG_SYSTEM, NULL);
            break;
        }
    }

    return 1;
}

// OFFSET: 0x008F97A0
int32_t CGChat::OnInstanceReset(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    int32_t mapId;
    char mapName[256];
    char text[3000];

    CGGameUI::ClearInstanceResetMap();
    msg->Get(reinterpret_cast<uint32_t&>(mapId));

    MapRec* map = g_mapDB.GetRecord(mapId);

    if (map) {
        SStrCopy(mapName, map->m_mapName, sizeof(mapName));
    } else {
        SStrPrintf(mapName, sizeof(mapName), "%d", mapId);
    }

    SStrPrintf(text, sizeof(text), FrameScript_GetText("INSTANCE_RESET_SUCCESS", -1, GENDER_NOT_APPLICABLE), mapName);
    CGChat::AddChatMessage(text, CHAT_MSG_SYSTEM, NULL);
    return 1;
}

// OFFSET: 0x008F9870
int32_t CGChat::OnInstanceResetFailed(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    int32_t reason;
    int32_t mapId;
    char mapName[256];
    char text[3000];

    msg->Get(reinterpret_cast<uint32_t&>(reason));
    msg->Get(reinterpret_cast<uint32_t&>(mapId));

    MapRec* map = g_mapDB.GetRecord(mapId);

    if (map) {
        SStrCopy(mapName, map->m_mapName, sizeof(mapName));
    } else {
        SStrPrintf(mapName, sizeof(mapName), "%d", mapId);
    }

    switch (reason) {
        case 0:
            SStrPrintf(text, sizeof(text), FrameScript_GetText("INSTANCE_RESET_FAILED", -1, GENDER_NOT_APPLICABLE), mapName);
            break;
        case 1:
            SStrPrintf(text, sizeof(text), FrameScript_GetText("INSTANCE_RESET_FAILED_OFFLINE", -1, GENDER_NOT_APPLICABLE), mapName);
            break;
        case 2:
            SStrPrintf(text, sizeof(text), FrameScript_GetText("INSTANCE_RESET_FAILED_ZONING", -1, GENDER_NOT_APPLICABLE), mapName);
            break;
    }

    CGChat::AddChatMessage(text, CHAT_MSG_SYSTEM, NULL);
    return 1;
}

// OFFSET: 0x008F99B0/0x14062CFB0
int32_t CGChat::OnResetFailedNotify(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    CGChat::AddChatMessage(FrameScript_GetText("RESET_FAILED_NOTIFY", -1, GENDER_NOT_APPLICABLE), CHAT_MSG_SYSTEM, NULL);
    return 1;
}

// OFFSET: 0x008FACB0
int32_t CGChat::OnChannelNotify(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    uint8_t notify;
    char channelName[128];
    char displayName[128];
    char text[3000];
    char extra[516];
    uint8_t flags;
    uint32_t zoneChannelID = 0;
    uint32_t channelFlags;
    uint8_t oldFlags;
    uint8_t newFlags;
    uint8_t suspended;

    msg->Get(notify);
    msg->GetString(channelName, sizeof(channelName));
    SStrCopy(displayName, channelName, sizeof(displayName));

    int32_t type = 0;
    WOWGUID guid = WOWGUID();
    WOWGUID targetGuid = WOWGUID();
    const char* senderName = NULL;
    text[0] = 0;
    int32_t reset = 0;
    int32_t rebuild = 0;

    switch (notify) {
        case 0: {
            type = CHAT_MSG_CHANNEL_JOIN;
            msg->Get(guid.m_guid);

            if (!CGChat::GetChannelID(channelName)) {
                return 1;
            }

            break;
        }

        case 1: {
            type = CHAT_MSG_CHANNEL_LEAVE;
            msg->Get(guid.m_guid);

            if (!CGChat::GetChannelID(channelName)) {
                return 1;
            }

            break;
        }

        case 2: {
            msg->Get(flags);
            msg->Get(zoneChannelID);
            msg->Get(channelFlags);

            if (msg->IsRead()) {
                extra[0] = 0;
            } else {
                msg->GetString(extra, sizeof(extra));
            }

            type = CHAT_MSG_CHANNEL_NOTICE;
            ChatChannel* channel = zoneChannelID ? CGChat::GetChannelByChatChannelID(zoneChannelID) : GetChannelByName(channelName);
            bool silent = false;

            if (channel && channel->m_joinPending) {
                channel->m_joinPending = 0;
                silent = true;
            } else if (channel && !channel->m_unk9C) {
                SStrCopy(text, "YOU_CHANGED", sizeof(text));

                if (!s_activeDisplayChannelType && !SStrCmpUTF8I(channel->m_name, CGChat::s_activeDisplayChannelName, STORM_MAX_STR)) {
                    SetCurrentActiveDisplayChannel(NULL, 0);
                }
            } else {
                SStrCopy(text, "YOU_JOINED", sizeof(text));
            }

            CGChat::JoinedChannel(zoneChannelID, channelName, channelFlags, flags);

            if (silent) {
                msg->m_read = msg->m_size;
                return 1;
            }

            break;
        }

        case 3: {
            msg->Get(zoneChannelID);
            msg->Get(suspended);
            type = CHAT_MSG_CHANNEL_NOTICE;
            ChatChannel* channel = zoneChannelID ? CGChat::GetChannelByChatChannelID(zoneChannelID) : GetChannelByName(channelName);

            if (channel) {
                if (channel->m_unkA0 || !suspended) {
                    SStrCopy(text, "YOU_LEFT", sizeof(text));
                    reset = 1;
                } else {
                    SStrCopy(text, "SUSPENDED", sizeof(text));
                }
            }

            CGChat::LeftChannel(zoneChannelID, channelName);
            break;
        }

        case 4: {
            type = CHAT_MSG_CHANNEL_NOTICE;
            SStrCopy(text, "WRONG_PASSWORD", sizeof(text));
            reset = 1;
            FrameScript_SignalEvent(SCRIPT_EVENT_CHANNEL_PASSWORD_REQUEST, "%s", displayName);
            break;
        }

        case 5: {
            type = CHAT_MSG_CHANNEL_NOTICE;
            SStrCopy(text, "NOT_MEMBER", sizeof(text));
            break;
        }

        case 6: {
            type = CHAT_MSG_CHANNEL_NOTICE;
            SStrCopy(text, "NOT_MODERATOR", sizeof(text));
            break;
        }

        case 7: {
            msg->Get(guid.m_guid);
            type = CHAT_MSG_CHANNEL_NOTICE_USER;
            SStrCopy(text, "PASSWORD_CHANGED", sizeof(text));
            break;
        }

        case 8: {
            msg->Get(guid.m_guid);
            type = CHAT_MSG_CHANNEL_NOTICE_USER;
            SStrCopy(text, "OWNER_CHANGED", sizeof(text));
            break;
        }

        case 9: {
            msg->GetString(extra, sizeof(extra));
            senderName = extra;
            type = CHAT_MSG_CHANNEL_NOTICE_USER;
            SStrCopy(text, "PLAYER_NOT_FOUND", sizeof(text));
            break;
        }

        case 10: {
            type = CHAT_MSG_CHANNEL_NOTICE;
            SStrCopy(text, "NOT_OWNER", sizeof(text));
            break;
        }

        case 11: {
            msg->GetString(extra, sizeof(extra));
            senderName = extra;
            type = CHAT_MSG_CHANNEL_NOTICE_USER;
            SStrCopy(text, "CHANNEL_OWNER", sizeof(text));
            break;
        }

        case 12: {
            msg->Get(guid.m_guid);
            msg->Get(oldFlags);
            msg->Get(newFlags);
            CGChat::HandleFlagsChanged(guid, oldFlags, newFlags, channelName);
            return 1;
        }

        case 13: {
            msg->Get(guid.m_guid);
            type = CHAT_MSG_CHANNEL_NOTICE_USER;
            SStrCopy(text, "ANNOUNCEMENTS_ON", sizeof(text));
            break;
        }

        case 14: {
            msg->Get(guid.m_guid);
            type = CHAT_MSG_CHANNEL_NOTICE_USER;
            SStrCopy(text, "ANNOUNCEMENTS_OFF", sizeof(text));
            break;
        }

        case 15: {
            msg->Get(guid.m_guid);
            type = CHAT_MSG_CHANNEL_NOTICE_USER;
            SStrCopy(text, "MODERATION_ON", sizeof(text));
            break;
        }

        case 16: {
            msg->Get(guid.m_guid);
            type = CHAT_MSG_CHANNEL_NOTICE_USER;
            SStrCopy(text, "MODERATION_OFF", sizeof(text));
            break;
        }

        case 17: {
            type = CHAT_MSG_CHANNEL_NOTICE;
            SStrCopy(text, "MUTED", sizeof(text));
            break;
        }

        case 18: {
            msg->Get(guid.m_guid);
            msg->Get(targetGuid.m_guid);
            type = CHAT_MSG_CHANNEL_NOTICE_USER;
            SStrCopy(text, "PLAYER_KICKED", sizeof(text));
            break;
        }

        case 19: {
            type = CHAT_MSG_CHANNEL_NOTICE;
            SStrCopy(text, "BANNED", sizeof(text));
            reset = 1;
            break;
        }

        case 20: {
            msg->Get(guid.m_guid);
            msg->Get(targetGuid.m_guid);
            type = CHAT_MSG_CHANNEL_NOTICE_USER;
            SStrCopy(text, "PLAYER_BANNED", sizeof(text));
            break;
        }

        case 21: {
            msg->Get(guid.m_guid);
            msg->Get(targetGuid.m_guid);
            type = CHAT_MSG_CHANNEL_NOTICE_USER;
            SStrCopy(text, "PLAYER_UNBANNED", sizeof(text));
            break;
        }

        case 22: {
            msg->GetString(extra, sizeof(extra));
            senderName = extra;
            type = CHAT_MSG_CHANNEL_NOTICE_USER;
            SStrCopy(text, "PLAYER_NOT_BANNED", sizeof(text));
            break;
        }

        case 23: {
            msg->Get(guid.m_guid);
            type = CHAT_MSG_CHANNEL_NOTICE_USER;
            SStrCopy(text, "PLAYER_ALREADY_MEMBER", sizeof(text));
            break;
        }

        case 24: {
            msg->Get(guid.m_guid);

            if ((FriendList::s_friendList && FriendList::s_friendList->IsIgnored(guid, true)) || (guid.m_guid & 0xF000000000000000ULL) || !(guid.m_guid & 0xF07FFFFFFFFFFFFFULL) || s_inviteChannelName[0]) {
                return 1;
            }

            type = CHAT_MSG_CHANNEL_NOTICE_USER;
            SStrCopy(text, "INVITE", sizeof(text));
            NameCache* record = g_nameDBCache.GetRecord(guid.m_guid, guid, CGChat::InviteNameQueryCallback, NULL, false);

            if (record) {
                FrameScript_SignalEvent(SCRIPT_EVENT_CHANNEL_INVITE_REQUEST, "%s%s", displayName, static_cast<const char*>(record->m_name));
            } else {
                SStrCopy(s_inviteChannelName, displayName, sizeof(s_inviteChannelName));
            }

            break;
        }

        case 25: {
            type = CHAT_MSG_CHANNEL_NOTICE_USER;
            SStrCopy(text, "INVITE_WRONG_FACTION", sizeof(text));
            break;
        }

        case 26: {
            type = CHAT_MSG_CHANNEL_NOTICE_USER;
            SStrCopy(text, "WRONG_FACTION", sizeof(text));
            reset = 1;
            break;
        }

        case 27: {
            type = CHAT_MSG_CHANNEL_NOTICE_USER;
            SStrCopy(text, "INVALID_NAME", sizeof(text));
            reset = 1;
            break;
        }

        case 28: {
            type = CHAT_MSG_CHANNEL_NOTICE_USER;
            SStrCopy(text, "NOT_MODERATED", sizeof(text));
            break;
        }

        case 29: {
            msg->GetString(extra, sizeof(extra));
            senderName = extra;
            type = CHAT_MSG_CHANNEL_NOTICE_USER;
            SStrCopy(text, "PLAYER_INVITED", sizeof(text));
            break;
        }

        case 30: {
            msg->GetString(extra, sizeof(extra));
            senderName = extra;
            type = CHAT_MSG_CHANNEL_NOTICE_USER;
            SStrCopy(text, "PLAYER_INVITE_BANNED", sizeof(text));
            break;
        }

        case 31: {
            type = CHAT_MSG_CHANNEL_NOTICE;
            SStrCopy(text, "THROTTLED", sizeof(text));
            break;
        }

        case 32: {
            type = CHAT_MSG_CHANNEL_NOTICE;
            SStrCopy(text, "NOT_IN_AREA", sizeof(text));
            reset = 1;
            break;
        }

        case 33: {
            type = CHAT_MSG_CHANNEL_NOTICE;
            SStrCopy(text, "NOT_IN_LFG", sizeof(text));
            reset = 1;
            break;
        }

        case 34: {
            msg->Get(guid.m_guid);
            type = CHAT_MSG_CHANNEL_NOTICE_USER;
            SStrCopy(text, "VOICE_ON", sizeof(text));
            ChatChannel* channel = zoneChannelID ? CGChat::GetChannelByChatChannelID(zoneChannelID) : GetChannelByName(channelName);

            if (channel) {
                channel->m_flags |= 0x80;
                rebuild = 1;
            }

            break;
        }

        case 35: {
            msg->Get(guid.m_guid);
            type = CHAT_MSG_CHANNEL_NOTICE_USER;
            SStrCopy(text, "VOICE_OFF", sizeof(text));
            ChatChannel* channel = zoneChannelID ? CGChat::GetChannelByChatChannelID(zoneChannelID) : GetChannelByName(channelName);

            if (channel) {
                channel->m_flags &= ~0x80;
                rebuild = 1;
            }

            break;
        }

        case 36: {
            msg->Get(guid.m_guid);
            type = CHAT_MSG_CHANNEL_NOTICE_USER;
            ChatChannel* channel = zoneChannelID ? CGChat::GetChannelByChatChannelID(zoneChannelID) : GetChannelByName(channelName);

            if (channel) {
                channel->m_flags |= 0x80;
                RebuildDisplayChannelList(1);
            }

            msg->m_read = msg->m_size;
            return 1;
        }

        case 37: {
            type = CHAT_MSG_CHANNEL_NOTICE;
            SStrCopy(text, "TRIAL_RESTRICTED", sizeof(text));
            break;
        }
    }

    ChatMessageParams params;
    params.m_senderName = senderName;
    params.m_channelName = displayName;
    params.m_senderGuid = guid;
    params.m_targetGuid = targetGuid;
    CGChat::AddChatMessage(text, static_cast<SLASH_COMMAND_ID>(type), &params);

    if (notify == 2 && extra[0]) {
        ChatMessageParams params2;
        params2.m_channelName = channelName;
        CGChat::AddChatMessage(extra, static_cast<SLASH_COMMAND_ID>(type), &params2);
    }

    if (reset) {
        CGChat::ResetChannel(zoneChannelID, channelName);
    }

    if (rebuild) {
        RebuildDisplayChannelList(1);
    }

    msg->m_read = msg->m_size;
    return 1;
}

// OFFSET: 0x008E5EB0/0x140614210
static bool OnClearBossEmotes(ClientClearBossEmotes* msg) {
    FrameScript_SignalEvent(SCRIPT_EVENT_CLEAR_BOSS_EMOTES, NULL);
    return true;
}

// OFFSET: 0x008FBD40/0x14062FE70
void CGChat::InitializeGame() {
    for (int32_t i = 0; i < g_languageWordsDB.m_numRecords; i++) {
        LanguageWordsRec* rec = g_languageWordsDB.GetRecordByIndex(i);

        if (!rec || !rec->m_word || !*rec->m_word) {
            continue;
        }

        HASHKEY_LANGUAGE key;
        key.m_length = SStrLen(rec->m_word);
        key.m_unk04 = 0;
        WORDLIST* list = s_wordLists.Ptr(rec->m_languageID, key);

        if (!list) {
            list = s_wordLists.New(rec->m_languageID, key, 0, 0);
        }

        *list->m_words.New() = rec;
    }

    for (uint32_t i = 0; i < CHAT_MSG_MAX; i++) {
        SStrCopy(s_chatColors[i].m_name, s_chatTypeInfo[i].m_name, sizeof(s_chatColors[i].m_name));
        s_chatColors[i].m_color[0] = s_chatTypeInfo[i].m_color[0];
        s_chatColors[i].m_color[1] = s_chatTypeInfo[i].m_color[1];
        s_chatColors[i].m_color[2] = s_chatTypeInfo[i].m_color[2];
        s_chatColors[i].m_unk48 = s_chatTypeInfo[i].m_unk48;
    }

    s_channelColors.SetCount(10);

    for (uint32_t i = 0; i < 10; i++) {
        SStrPrintf(s_channelColors.m_data[i].m_name, sizeof(s_channelColors.m_data[i].m_name), "CHANNEL%d", i + 1);
        s_channelColors.m_data[i].m_color[0] = s_chatColors[CHAT_MSG_CHANNEL].m_color[0];
        s_channelColors.m_data[i].m_color[1] = s_chatColors[CHAT_MSG_CHANNEL].m_color[1];
        s_channelColors.m_data[i].m_color[2] = s_chatColors[CHAT_MSG_CHANNEL].m_color[2];
        s_channelColors.m_data[i].m_unk48 = s_chatColors[CHAT_MSG_CHANNEL].m_unk48;
    }

    for (uint32_t i = 0; i < sizeof(s_chatWindows) / sizeof(s_chatWindows[0]); i++) {
        s_chatWindows[i].Init();

        if (i == 0) {
            s_chatWindows[i].m_shown = 1;
            s_chatWindows[i].m_docked = 1;
            memset(s_chatWindows[i].m_messageGroups, 1, 42);
        } else if (i == 1) {
            s_chatWindows[i].m_shown = 1;
            s_chatWindows[i].m_docked = 2;

            for (uint32_t j = 42; j < 49; j++) {
                s_chatWindows[i].m_messageGroups[j] = s_chatMessageGroups[j].m_default != 0;
            }
        } else {
            memset(s_chatWindows[i].m_messageGroups, 0, sizeof(s_chatWindows[i].m_messageGroups));
        }
    }

    for (int32_t i = g_areaTableDB.m_numRecords; i;) {
        i--;
        AreaTableRec* area = g_areaTableDB.GetRecordByIndex(i);

        if (area->m_flags & 0x200) {
            s_cityArea = area;
            break;
        }
    }

    CGChat::m_loaded = 0;
    CGChat::m_registered = 0;
    CGChat::m_complaintsSystemStatus = 0;
    s_queueAddonPrefixes = 1;
    s_addonPrefixMessage.m_prefixes.Clear();
    s_addonPrefixes.Clear();
    s_numAddonPrefixes = 0;
    s_pendingReportTarget.m_guid = 0;
    s_joinedZoneChannels = 0;
    memset(CGChat::m_whisperSrcHistory, 0, sizeof(((CGChat*)0)->m_whisperSrcHistory));
    memset(CGChat::m_whisperDstHistory, 0, sizeof(((CGChat*)0)->m_whisperDstHistory));
    s_whisperThrottleIndex = 0;

    for (uint32_t i = 0; i < sizeof(s_whisperThrottleTimes) / sizeof(s_whisperThrottleTimes[0]); i++) {
        s_whisperThrottleTimes[i] = 0;
    }

    CGChat::m_voiceChatSystemStatus = 0;
    s_chatServerDown = 0;
    memset(CGChat::s_displayChannels, 0, sizeof(CGChat::s_displayChannels));
    s_numDisplayChannels = 0;
    memset(CGChat::s_activeDisplayChannelName, 0, sizeof(CGChat::s_activeDisplayChannelName));
    s_activeDisplayChannelType = 4;
    s_pendingUserListActive = false;
    s_displayChannelMemberCount = 0;
    s_pendingUsers.Clear();
    s_activeDisplayChannelFlags = 0;
    memset(s_collapsedHeaders, 0, sizeof(s_collapsedHeaders));
    s_activeVoiceChannel = 0;
    s_inviteChannelName[0] = 0;

    ClientServices::SetMessageHandler(SMSG_CHANNEL_NOTIFY, CGChat::OnChannelNotify, NULL);
    ClientServices::SetMessageHandler(SMSG_CHAT_DOWN, CGChat::OnChatDown, NULL);
    ClientServices::SetMessageHandler(SMSG_CHAT_IS_DOWN, CGChat::OnChatIsDown, NULL);
    ClientServices::SetMessageHandler(SMSG_CHAT_RECONNECT, CGChat::OnChatReconnect, NULL);
    ClientServices::SetMessageHandler(SMSG_CHANNEL_LIST, CGChat::OnChannelList, NULL);
    ClientServices::SetMessageHandler(SMSG_CHAT, CGChat::OnMessageChat, NULL);
    ClientServices::SetMessageHandler(SMSG_TEXT_EMOTE, CGChat::OnTextEmote, NULL);
    ClientServices::SetMessageHandler(SMSG_ZONE_UNDER_ATTACK, CGChat::OnZoneUnderAttack, NULL);
    ClientServices::SetMessageHandler(SMSG_DEFENSE_MESSAGE, CGChat::OnDefenseMessage, NULL);
    ClientServices::SetMessageHandler(SMSG_CHAT_SERVER_MESSAGE, CGChat::OnServerMessage, NULL);
    ClientServices::SetMessageHandler(SMSG_RAID_INSTANCE_MESSAGE, CGChat::OnRaidInstanceMessage, NULL);
    ClientServices::SetMessageHandler(SMSG_INSTANCE_RESET, CGChat::OnInstanceReset, NULL);
    ClientServices::SetMessageHandler(SMSG_INSTANCE_RESET_FAILED, CGChat::OnInstanceResetFailed, NULL);
    ClientServices::SetMessageHandler(SMSG_UPDATE_LAST_INSTANCE, CGChat::OnUpdateInstance, NULL);
    ClientServices::SetMessageHandler(SMSG_UPDATE_INSTANCE_OWNERSHIP, CGChat::OnUpdateInstanceOwnership, NULL);
    ClientServices::SetMessageHandler(SMSG_EXPECTED_SPAM_RECORDS, CGChat::OnCheckReloadSpamFilter, NULL);
    ClientServices::SetMessageHandler(SMSG_TITLE_EARNED, CGChat::OnTitleEarned, NULL);
    ClientServices::SetMessageHandler(SMSG_RESET_FAILED_NOTIFY, CGChat::OnResetFailedNotify, NULL);
    ClientServices::SetMessageHandler(SMSG_GM_MESSAGECHAT, CGChat::OnGMMessageChat, NULL);
    ClientServices::SetMessageHandler(SMSG_LOG_XP_GAIN, Packet_SMSG_LOG_XPGAIN, NULL);
    ClientServices::SetMessageHandler(SMSG_DURABILITY_DAMAGE_DEATH, CGChat::OnDurabilityDamageDeath, NULL);
    ClientServices::SetMessageHandler(SMSG_CHANNEL_MEMBER_COUNT, CGChat::OnChannelMemberCount, NULL);
    ClientServices::SetMessageHandler(SMSG_COMSAT_RECONNECT_TRY, CGChat::OnComsatReconnectAttempt, NULL);
    ClientServices::SetMessageHandler(SMSG_COMSAT_DISCONNECT, CGChat::OnComsatDisconnect, NULL);
    ClientServices::SetMessageHandler(SMSG_COMSAT_CONNECT_FAIL, CGChat::OnComsatConnectFail, NULL);
    ClientServices::SetMessageHandler(SMSG_VOICE_CHAT_STATUS, CGChat::OnVoiceStatusChanged, NULL);
    ClientServices::SetMessageHandler(SMSG_USERLIST_ADD, CGChat::OnUserListAdded, NULL);
    ClientServices::SetMessageHandler(SMSG_USERLIST_REMOVE, CGChat::OnUserListRemove, NULL);
    ClientServices::SetMessageHandler(SMSG_USERLIST_UPDATE, CGChat::OnUserListUpdate, NULL);
    ClientServices::SetMessageHandler(SMSG_VOICESESSION_FULL, CGChat::OnVoicesessionFull, NULL);
    ClientServices::SetMessageHandler(SMSG_SERVER_FIRST_ACHIEVEMENT, CGChat::OnServerFirstAchievement, NULL);
    ClientClearBossEmotes::s_handler = JamHandlerEncode(OnClearBossEmotes, g_jamHandlerKey_CFF1D8);
}

// OFFSET: 0x008F7F10
void CGChat::OnLogXPGain(int32_t amount) {
    char text[3000];
    SStrPrintf(text, sizeof(text), FrameScript_GetText("COMBATLOG_XPLOSS_FIRSTPERSON_UNNAMED", -1, GENDER_NOT_APPLICABLE), -amount);
    CGChat::AddChatMessage(text, static_cast<SLASH_COMMAND_ID>(33), NULL);
}

// OFFSET: 0x008F79C0/0x14062A8E0
void CGChat::InformTargetIcon(WOWGUID setter, WOWGUID target, int32_t index) {
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(target, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (!unit) {
        return;
    }

    WOWGUID none = WOWGUID();
    NameCache* name = g_nameDBCache.GetRecord(setter.m_guid, none, NULL, NULL, false);
    const char* unitName = unit->GetVisibleUnitName(NULL, 1);

    if (!unitName) {
        unitName = FrameScript_GetText("UNKNOWNOBJECT", -1, GENDER_NOT_APPLICABLE);
    }

    if (name) {
        char buffer[3000];
        SStrPrintf(buffer, sizeof(buffer), FrameScript_GetText("TARGET_ICON_SET", -1, GENDER_NOT_APPLICABLE), static_cast<const char*>(name->m_name), static_cast<const char*>(name->m_name), index + 1, unitName);
        CGChat::AddChatMessage(buffer, static_cast<SLASH_COMMAND_ID>(52), NULL);
    }
}

// OFFSET: 0x008F7A90/0x14062A9F0
static void UnitCombatLogShowXPGained(CGUnit_C* unit, int32_t amount, float groupMult) {
    char text[3000];
    int32_t base = static_cast<int32_t>(amount / groupMult);

    if (unit && (static_cast<uint8_t>(static_cast<uint32_t>(static_cast<int16_t>(unit->m_obj->m_type)) >> 3) & 1)) {
        if (base == amount) {
            SStrPrintf(text, sizeof(text), FrameScript_GetText("COMBATLOG_XPGAIN_FIRSTPERSON", -1, GENDER_NOT_APPLICABLE), unit->GetUnitName(NULL, 1), amount);
        } else {
            const char* token = base < amount ? "COMBATLOG_XPGAIN_FIRSTPERSON_GROUP" : "COMBATLOG_XPGAIN_FIRSTPERSON_RAID";
            SStrPrintf(text, sizeof(text), FrameScript_GetText(token, -1, GENDER_NOT_APPLICABLE), unit->GetUnitName(NULL, 1), amount, abs(amount - base));
        }
    } else if (base == amount) {
        SStrPrintf(text, sizeof(text), FrameScript_GetText("COMBATLOG_XPGAIN_FIRSTPERSON_UNNAMED", -1, GENDER_NOT_APPLICABLE), amount);
    } else {
        const char* token = base < amount ? "COMBATLOG_XPGAIN_FIRSTPERSON_UNNAMED_GROUP" : "COMBATLOG_XPGAIN_FIRSTPERSON_UNNAMED_RAID";
        SStrPrintf(text, sizeof(text), FrameScript_GetText(token, -1, GENDER_NOT_APPLICABLE), amount, abs(amount - base));
    }

    CGChat::AddChatMessage(text, static_cast<SLASH_COMMAND_ID>(33), NULL);
}

// OFFSET: 0x008F7BD0/0x14062AB80
void CGChat::LogXPGain(CGUnit_C* unit, int32_t amount, int32_t type, int32_t bonus, float groupMult, bool referAFriend) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player) {
        player->AddWorldXPGainText(amount);
    }

    if (unit && !type) {
        if (!player) {
            return;
        }

        uint32_t restState = player->m_player->m_playerBytes2[3];

        if (referAFriend) {
            restState = 6;
        }

        ExhaustionRec* exhaustion = g_exhaustionDB.GetRecord(restState);

        if (exhaustion && !fequal(exhaustion->m_factor, 0.0f) && !fequal(exhaustion->m_factor, 1.0f)) {
            char token[256];
            int32_t base = static_cast<int32_t>(bonus / groupMult);

            if (base == bonus) {
                SStrCopy(token, s_exhaustionTokens[restState], sizeof(token));
            } else if (base < bonus) {
                SStrPrintf(token, sizeof(token), "%s_GROUP", s_exhaustionTokens[restState]);
            } else {
                SStrPrintf(token, sizeof(token), "%s_RAID", s_exhaustionTokens[restState]);
            }

            char bonusText[32];
            SStrPrintf(bonusText, sizeof(bonusText), "%s%d", amount - bonus >= 0 ? "+" : "", amount - bonus);
            char text[3000];

            if (base == bonus) {
                SStrPrintf(text, sizeof(text), FrameScript_GetText(token, -1, GENDER_NOT_APPLICABLE), unit->GetUnitName(NULL, 1), amount, bonusText, static_cast<const char*>(exhaustion->m_name));
            } else {
                SStrPrintf(text, sizeof(text), FrameScript_GetText(token, -1, GENDER_NOT_APPLICABLE), unit->GetUnitName(NULL, 1), amount, bonusText, static_cast<const char*>(exhaustion->m_name), abs(bonus - base));
            }

            CGChat::AddChatMessage(text, static_cast<SLASH_COMMAND_ID>(33), NULL);
            return;
        }

        UnitCombatLogShowXPGained(unit, amount, groupMult);
        return;
    }

    if (!referAFriend || (type != 1 && type != 6)) {
        UnitCombatLogShowXPGained(unit, amount, groupMult);
        return;
    }

    int32_t rafBonus = CMath::fint_(amount * 0.6666667f);
    char bonusText[32];
    SStrPrintf(bonusText, sizeof(bonusText), "%s%d", rafBonus >= 0 ? "+" : "", rafBonus);
    ExhaustionRec* exhaustion = g_exhaustionDB.GetRecord(6);
    char text[3000];
    SStrPrintf(text, sizeof(text), FrameScript_GetText("COMBATLOG_XPGAIN_QUEST", -1, GENDER_NOT_APPLICABLE), amount, bonusText, static_cast<const char*>(exhaustion->m_name), rafBonus);
    CGChat::AddChatMessage(text, static_cast<SLASH_COMMAND_ID>(33), NULL);
}

// OFFSET: 0x008F91E0/0x14062C6A0
int32_t Packet_SMSG_LOG_XPGAIN(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    WOWGUID guid = WOWGUID::Make(0);
    msg->Get(guid.m_guid);
    uint32_t amount = 0;
    msg->Get(amount);
    uint8_t type = 0;
    msg->Get(type);
    float groupMult = 1.0f;
    uint32_t bonus = 0;

    if (!type) {
        msg->Get(bonus);
        msg->Get(groupMult);
    }

    uint8_t referAFriend = 0;
    msg->Get(referAFriend);
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (guid == ClntObjMgrGetActivePlayer()) {
        CGChat::OnLogXPGain(amount);
    } else {
        CGChat::LogXPGain(unit, amount, type, bonus, groupMult, referAFriend != 0);
    }

    return 1;
}

// OFFSET: 0x00994480/0x140713D20
bool InebriatedParseText(const char* text, char* buffer, uint32_t size, float drunkness) {
    if (!text) {
        return false;
    }

    char* dest = buffer;
    uint32_t remaining = size;
    bool wordStart = true;

    while (*text && remaining > 1) {
        char c = *text;
        *dest++ = c;
        remaining--;
        text++;

        switch (c) {
            case ' ': {
                wordStart = true;
                continue;
            }

            case 'S':
            case 's': {
                if (wordStart) {
                    if (remaining > 1 && *text != ' ' && *text != 'S' && *text != 's' && CRandom::ufloat_(g_rndSeed) < drunkness) {
                        char word[260];
                        word[0] = 's';
                        word[1] = 'h';
                        uint32_t length = 2;

                        for (const char* source = text; *source && *source != ' '; source++) {
                            word[length++] = *source;
                        }

                        word[length] = 0;

                        if (!ProfanityFilter(word, false, true)) {
                            *dest++ = 'h';
                            remaining--;
                        }
                    }
                } else if (remaining > 1 && *text == ' ' && CRandom::ufloat_(g_rndSeed) < drunkness) {
                    *dest++ = 'h';
                    remaining--;
                }

                break;
            }

            case '|': {
                if (*text == 'c') {
                    const char* end = SStrStr(text, "|r");

                    if (end) {
                        uint32_t length = end - text + 2;

                        if (length >= remaining) {
                            length = remaining - 1;
                        }

                        SStrCopy(dest, text, length + 1);
                        dest += length;
                        remaining -= length;
                        text = end + 2;
                    }
                }

                break;
            }
        }

        wordStart = false;
    }

    *dest = 0;

    if (drunkness * 0.25f > CRandom::ufloat_(g_rndSeed)) {
        char* copy = static_cast<char*>(alloca(size));
        SStrCopy(copy, buffer, STORM_MAX_STR);
        SStrPrintf(buffer, size, FrameScript_GetText("SLURRED_SPEECH", -1, GENDER_NOT_APPLICABLE), copy);
    }

    return true;
}

// OFFSET: 0x008EFA20/0x14061FC70
void CGChat::AddTextEmoteMessage(const WOWGUID& guid, uint32_t emote, const char* target, uint32_t emoteNum) {
    if (!target) {
        SErrSetLastError(STORM_ERROR_INVALID_PARAMETER);
        return;
    }

    if (s_queueChat) {
        CGChat::QueueTextEmote(guid, emote, target, 1, emoteNum);
        return;
    }

    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return;
    }

    WOWGUID requester = WOWGUID();
    NameCache* sender = g_nameDBCache.GetRecord(guid.m_guid, requester, NULL, NULL, false);

    if (!sender) {
        return;
    }

    EmotesTextRec* emoteText = g_emotesTextDB.GetRecord(emote);

    if (!emoteText) {
        return;
    }

    uint32_t index = 0;
    int32_t hasSender = 1;
    int32_t hasTarget = 1;

    if (guid.m_guid == ClntObjMgrGetActivePlayer().m_guid) {
        index = 2;
        hasSender = 0;
    } else if (!SStrCmp(target, player->GetUnitName(NULL, 1), STORM_MAX_STR)) {
        index = 1;
        hasTarget = 0;
    }

    if (!*target) {
        index |= 4;
    }

    uint32_t gender = sender->m_sex != 1 ? 0 : 8;
    EmotesTextDataRec* text = g_emotesTextDataDB.GetRecord(emoteText->m_emoteText[index | gender]);

    if (!text || !*text->m_text) {
        text = g_emotesTextDataDB.GetRecord(emoteText->m_emoteText[index]);
    }

    if (!text || !*text->m_text) {
        index = (index & ~5) | 4;
        text = g_emotesTextDataDB.GetRecord(emoteText->m_emoteText[index | gender]);
    }

    if (!text || !*text->m_text) {
        text = g_emotesTextDataDB.GetRecord(emoteText->m_emoteText[index]);
    }

    if (text && *text->m_text) {
        char buffer[256];

        if (hasSender) {
            if (hasTarget) {
                SStrPrintf(buffer, sizeof(buffer), static_cast<const char*>(text->m_text), static_cast<const char*>(sender->m_name), target);
            } else {
                SStrPrintf(buffer, sizeof(buffer), static_cast<const char*>(text->m_text), static_cast<const char*>(sender->m_name));
            }
        } else if (hasTarget) {
            SStrPrintf(buffer, sizeof(buffer), static_cast<const char*>(text->m_text), target);
        } else {
            SStrPrintf(buffer, sizeof(buffer), static_cast<const char*>(text->m_text));
        }

        const char* languageName = "";
        LanguagesRec* language = g_languagesDB.GetRecord(0);

        if (language) {
            languageName = language->m_name;
        }

        char guidString[20] = {};

        if (!(guid.m_guid & 0xF000000000000000ULL) && (guid.m_guid & 0xF07FFFFFFFFFFFFFULL)) {
            GUIDToString(guid, guidString);
        }

        FrameScript_SignalEvent(SCRIPT_EVENT_CHAT_MSG_TEXT_EMOTE, "%s%s%s%s%s%s%d%d%s%d%d%s%u%b", buffer, static_cast<const char*>(sender->m_name), languageName, "", "", "", 0, 0, "", 0, 0, guidString, 0, 0);

        if (s_chatLogging[0]) {
            SLogWrite(s_chatLog[0], "%s", buffer);
        }
    }

    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (unit) {
        unit->PlayTextEmoteSound(emote, emoteNum);
    }
}

// OFFSET: 0x008F04B0/0x140620B40
int32_t CGChat::OnTextEmote(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    WOWGUID guid;
    uint32_t emote;
    uint32_t emoteNum;
    uint32_t length;
    void* data;

    msg->Get(guid.m_guid);
    msg->Get(emote);
    msg->Get(emoteNum);
    msg->Get(length);
    msg->GetDataInSitu(data, length);
    const char* target = static_cast<const char*>(data);

    if (FriendList::s_friendList && FriendList::s_friendList->IsIgnored(guid, true)) {
        return 1;
    }

    if (!g_nameDBCache.GetRecord(guid.m_guid, guid, CGChat::TextEmoteNameQueryCallback, NULL, false)) {
        CGChat::QueueTextEmote(guid, emote, target, 0, emoteNum);
        return 1;
    }

    CGChat::AddTextEmoteMessage(guid, emote, target, emoteNum);
    return 1;
}

// OFFSET: 0x008F1E80/0x140622CF0
int32_t CGChat::OnChannelList(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    uint8_t display = 0;
    msg->Get(display);

    if (display) {
        return CGChat::OnDisplayChannelList(msg);
    }

    char name[128];
    msg->GetString(name, sizeof(name));
    ChatChannel* channel = GetChannelByName(name);

    if (!channel) {
        msg->m_read = msg->m_size;
        return 1;
    }

    msg->Get(channel->m_flags);

    uint32_t count;
    msg->Get(count);

    int32_t pending = 0;
    s_pendingUserListActive = false;

    for (int32_t i = 0; i < static_cast<int32_t>(count); i++) {
        PENDINGUSERLIST* user = channel->m_pendingUsers.NewNode(2, 0, 0);
        msg->Get(user->m_guid.m_guid);
        msg->Get(user->m_flags);

        if (!g_nameDBCache.GetRecord(user->m_guid.m_guid, user->m_guid, CGChat::NameQueryCallback, NULL, false)) {
            pending++;
            s_pendingUserListActive = true;
        }
    }

    if (!pending) {
        CGChat::DisplayPendingUserList(channel);
    }

    return 1;
}

// OFFSET: 0x008E5CC0/0x140613FE0
void CGChat::UpdateLanguages() {
    FrameScript_SignalEvent(SCRIPT_EVENT_LANGUAGE_LIST_CHANGED, NULL);
}

// OFFSET: 0x008E8DC0/0x140617590
const char* CGChat::GetChannelStringForDisplay(int32_t session, int32_t* type) {
    *type = 4;

    if (static_cast<uint32_t>(session) >= s_numDisplayChannels) {
        return NULL;
    }

    int32_t displayType = CGChat::s_displayChannels[session].m_type;

    if (displayType) {
        if (displayType == 2 || displayType == 3 || displayType == 1) {
            *type = displayType;
            return "";
        }

        return NULL;
    }

    int32_t index = CGChat::s_displayChannels[session].m_channelIndex;

    if (index < 0 || index >= static_cast<int32_t>(CGChat::s_channels.m_count)) {
        return NULL;
    }

    const char* name = CGChat::GetChannelName(CGChat::s_channels.m_data[index].m_channelNumber, true);

    if (name) {
        *type = 0;
    }

    return name;
}

// OFFSET: 0x008ECDC0/0x14061C350
PENDINGCHAT::~PENDINGCHAT() {
    if (this->m_text) {
        SMemFree(this->m_text, __FILE__, __LINE__, 0);
    }

    if (this->m_senderName) {
        SMemFree(this->m_senderName, __FILE__, __LINE__, 0);
    }

    if (this->m_conversation) {
        SMemFree(this->m_conversation, "delete", -1, 0);
    }
}

// OFFSET: 0x008ECE50
PENDINGTEXTEMOTE::~PENDINGTEXTEMOTE() {
    if (this->m_target) {
        SMemFree(this->m_target, __FILE__, __LINE__, 0);
    }
}

// OFFSET: 0x008EF180/0x14061F110
void CGChat::QueueTextEmote(const WOWGUID& guid, uint32_t emote, const char* target, int32_t queued, uint32_t emoteNum) {
    PENDINGTEXTEMOTE* pending = CGChat::s_pendingTextEmotes.NewNode(2, 0, 0);
    pending->m_guid = guid;
    pending->m_emote = emote;
    pending->m_target = SStrDupA(target, __FILE__, __LINE__);
    pending->m_queued = queued;
    pending->m_emoteNum = emoteNum;
}

// OFFSET: 0x008F0060/0x1406204F0
void CGChat::NameQueryCallback(int32_t id, const WOWGUID& guid, void* param, bool success) {
    PENDINGCHAT* next;

    for (PENDINGCHAT* chat = CGChat::s_pendingChat.Head(); !CGChat::s_pendingChat.IsEnd(chat); chat = next) {
        next = CGChat::s_pendingChat.RawNext(chat);

        if (chat->m_queued) {
            continue;
        }

        if (chat->m_senderGuid != guid && chat->m_targetGuid != guid) {
            continue;
        }

        const char* senderName = NULL;
        const char* targetName = NULL;
        WOWGUID senderGuid = chat->m_senderGuid;

        if (!(senderGuid.m_guid & 0xF000000000000000ULL) && (senderGuid.m_guid & 0xF07FFFFFFFFFFFFFULL)) {
            WOWGUID requester = WOWGUID();
            NameCache* record = g_nameDBCache.GetRecord(senderGuid.m_guid, requester, NULL, NULL, false);

            if (record) {
                senderName = record->m_name;
            }
        } else if ((senderGuid.m_guid & 0xF0F0000000000000ULL) == 0xF040000000000000ULL) {
            PetNameCache* record = g_petNameCache.GetRecord(static_cast<int32_t>((senderGuid.m_guid >> 24) & 0xFFFFFFF), chat->m_senderGuid, NULL, NULL, false);

            if (record) {
                senderName = record->m_name;
            }
        }

        WOWGUID targetGuid = chat->m_targetGuid;

        if (!(targetGuid.m_guid & 0xF000000000000000ULL) && (targetGuid.m_guid & 0xF07FFFFFFFFFFFFFULL)) {
            WOWGUID requester = WOWGUID();
            NameCache* record = g_nameDBCache.GetRecord(targetGuid.m_guid, requester, NULL, NULL, false);

            if (record) {
                targetName = record->m_name;
            }
        } else if ((targetGuid.m_guid & 0xF0F0000000000000ULL) == 0xF040000000000000ULL) {
            PetNameCache* record = g_petNameCache.GetRecord(static_cast<int32_t>((targetGuid.m_guid >> 24) & 0xFFFFFFF), chat->m_targetGuid, NULL, NULL, false);

            if (record) {
                targetName = record->m_name;
            }
        }

        if ((!senderGuid || senderName || chat->m_senderName) && (!chat->m_targetGuid || targetName || chat->m_targetName)) {
            if (s_queueChat) {
                chat->m_queued = 1;
                continue;
            }

            ChatMessageParams params;
            params.m_senderName = chat->m_senderName ? chat->m_senderName : senderName;
            params.m_language = chat->m_language;
            params.m_channelName = chat->m_channelName;
            params.m_targetName = chat->m_targetName ? chat->m_targetName : targetName;
            params.m_flags = chat->m_flags;
            params.m_senderGuid = senderGuid;
            params.m_senderUnk = chat->m_senderUnk;
            params.m_targetGuid = chat->m_targetGuid;
            params.m_achievementID = chat->m_achievementID;
            params.m_isGM = chat->m_isGM;
            params.m_conversation = chat->m_conversation;
            params.m_mobile = chat->m_mobile;
            CGChat::AddChatMessage(chat->m_text, static_cast<SLASH_COMMAND_ID>(chat->m_type), &params);
            CGChat::s_pendingChat.DeleteNode(chat);
        } else if ((senderGuid == guid && !senderName) || (chat->m_targetGuid == guid && !targetName)) {
            CGChat::s_pendingChat.DeleteNode(chat);
        }
    }

    if (!s_pendingUserListActive) {
        return;
    }

    for (uint32_t i = 0; i < CGChat::s_channels.Count(); i++) {
        ChatChannel* channel = &CGChat::s_channels.m_data[i];
        PENDINGUSERLIST* user = channel->m_pendingUsers.Head();

        if (!user) {
            continue;
        }

        while (!channel->m_pendingUsers.IsEnd(user)) {
            if (user->m_guid == guid && !success) {
                user = channel->m_pendingUsers.DeleteNode(user);
            } else {
                user = channel->m_pendingUsers.RawNext(user);
            }
        }

        CGChat::DisplayPendingUserList(channel);
    }
}

// OFFSET: 0x008F03D0
void CGChat::TextEmoteNameQueryCallback(int32_t id, const WOWGUID& guid, void* param, bool success) {
    PENDINGTEXTEMOTE* next;

    for (PENDINGTEXTEMOTE* pending = CGChat::s_pendingTextEmotes.Head(); !CGChat::s_pendingTextEmotes.IsEnd(pending); pending = next) {
        next = CGChat::s_pendingTextEmotes.RawNext(pending);

        if (pending->m_queued || pending->m_guid != guid) {
            continue;
        }

        WOWGUID requester = WOWGUID();

        if (g_nameDBCache.GetRecord(pending->m_guid.m_guid, requester, NULL, NULL, false)) {
            if (s_queueChat) {
                pending->m_queued = 1;
                continue;
            }

            CGChat::AddTextEmoteMessage(pending->m_guid, pending->m_emote, pending->m_target, pending->m_emoteNum);
        }

        CGChat::s_pendingTextEmotes.DeleteNode(pending);
    }
}

// OFFSET: 0x008F08E0/0x140621070
void SetCurrentActiveDisplayChannel(const char* name, int32_t type) {
    if (!name) {
        memset(CGChat::s_activeDisplayChannelName, 0, sizeof(CGChat::s_activeDisplayChannelName));
        s_activeDisplayChannelType = 4;
        s_pendingUsers.Clear();
        s_activeDisplayChannelFlags = 0;
        return;
    }

    if (type == s_activeDisplayChannelType && (type || !SStrCmpUTF8I(name, CGChat::s_activeDisplayChannelName, STORM_MAX_STR))) {
        return;
    }

    SStrCopy(CGChat::s_activeDisplayChannelName, name, sizeof(CGChat::s_activeDisplayChannelName));
    s_activeDisplayChannelType = type;
    s_pendingUsers.Clear();
    s_activeDisplayChannelFlags = 0;

    if (!type) {
        CDataStore msg;
        msg.Put(static_cast<uint32_t>(CMSG_SET_CHANNEL_WATCH));
        msg.PutString(CGChat::s_activeDisplayChannelName);
        msg.Finalize();
        ClientServices::Send2(&msg);
    }
}

// OFFSET: 0x008F0A20/0x140621200
uint32_t BuildPartyDisplayList() {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return 0;
    }

    s_pendingUsers.Clear();

    if (!CGPartyInfo::m_members[0]) {
        return 0;
    }

    s_pendingUsers.SetCount(CGPartyInfo::NumMembers() + 1);
    PENDINGUSERLIST* self = s_pendingUsers.m_data;
    WOWGUID guid = player->m_obj->m_guid;
    UpdateVoiceMemberFlags(self, guid == CGPartyInfo::m_leader ? 5 : 4, guid, 2, "");
    s_activeDisplayChannelFlags = self->m_flags;
    uint32_t count = 1;

    for (uint32_t i = 0; i < 5; i++) {
        if (!CGPartyInfo::GetMember(i)) {
            continue;
        }

        uint8_t flags = CGPartyInfo::GetMember(i) == CGPartyInfo::m_leader ? 5 : 4;
        UpdateVoiceMemberFlags(&s_pendingUsers.m_data[count], flags, CGPartyInfo::GetMember(i), 2, "");
        count++;
    }

    qsort(s_pendingUsers.m_data, count, sizeof(PENDINGUSERLIST), QSortDisplayListMembers);
    return count;
}

// OFFSET: 0x008F0B80/0x140621490
uint32_t BuildRaidDisplayList(int32_t type) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return 0;
    }

    s_pendingUsers.Clear();

    if (!CGRaidInfo::m_numMembers) {
        return 0;
    }

    s_pendingUsers.SetCount(CGRaidInfo::m_numMembers);

    for (uint32_t i = 0; i < CGRaidInfo::m_numMembers; i++) {
        CGRaidMember* member = CGRaidInfo::m_members[i];

        if (!member) {
            continue;
        }

        PENDINGUSERLIST* user = &s_pendingUsers.m_data[i];
        uint8_t flags = 4;

        if (member->m_rank == 2) {
            flags = 7;
        } else if (member->m_rank == 1) {
            flags = 6;
        }

        UpdateVoiceMemberFlags(user, flags, member->m_guid, type, "");

        if (user->m_guid == player->m_obj->m_guid) {
            s_activeDisplayChannelFlags = user->m_flags;
        }
    }

    qsort(s_pendingUsers.m_data, CGRaidInfo::m_numMembers, sizeof(PENDINGUSERLIST), QSortDisplayListMembers);
    return CGRaidInfo::m_numMembers;
}

// OFFSET: 0x008F0C90/0x140621620
void RebuildDisplayChannelList(int32_t signal) {
    memset(CGChat::s_displayChannels, 0, sizeof(CGChat::s_displayChannels));

    for (uint32_t i = 0; i < sizeof(CGChat::s_displayChannels) / sizeof(CGChat::s_displayChannels[0]); i++) {
        CGChat::s_displayChannels[i].m_headerIndex = -1;
    }

    int32_t battleground;
    int32_t party;
    int32_t raid;

    if (CGRaidInfo::m_numMembers && CGRaidInfo::IsPVPRaid()) {
        battleground = 1;
        party = ComSatClient_HasSession(2, "") || CGPartyInfo::m_members[0];
        raid = ComSatClient_HasSession(3, "");
    } else {
        battleground = 0;
        party = CGPartyInfo::m_members[0].m_guid != 0;
        raid = CGRaidInfo::m_numMembers != 0;
    }

    uint32_t count = 1;
    DisplayChannel* group = &CGChat::s_displayChannels[0];
    group->m_channelIndex = 0;
    group->m_headerIndex = 0;
    group->m_collapsed = s_collapsedHeaders[0];
    group->m_flags = 0;
    group->m_type = 4;
    group->m_voiceEnabled = 0;
    group->m_count = party;

    if (raid) {
        group->m_count++;
    }

    if (battleground) {
        group->m_count++;
    }

    if (!s_collapsedHeaders[0]) {
        if (party && !raid) {
            DisplayChannel* entry = &CGChat::s_displayChannels[1];
            entry->m_channelIndex = -1;
            entry->m_headerIndex = -1;
            entry->m_collapsed = 0;
            entry->m_flags = 0;
            entry->m_enabled = 1;
            entry->m_type = 2;
            entry->m_voiceEnabled = ComSatClient_HasSession(2, "");
            entry->m_count = CGPartyInfo::NumMembers() + 1;

            if (!s_collapsedHeaders[0]) {
                group->m_collapsed = 0;
            }

            count = 2;
        } else if (raid) {
            DisplayChannel* entry = &CGChat::s_displayChannels[1];
            entry->m_channelIndex = -2;
            entry->m_headerIndex = -1;
            entry->m_collapsed = 0;
            entry->m_flags = 0;
            entry->m_enabled = 1;
            entry->m_type = 3;
            entry->m_voiceEnabled = ComSatClient_HasSession(3, "");
            entry->m_count = CGRaidInfo::m_numMembers;

            if (!s_collapsedHeaders[0]) {
                group->m_collapsed = 0;
            }

            count = 2;
        }

        if (battleground) {
            DisplayChannel* entry = &CGChat::s_displayChannels[count];
            entry->m_channelIndex = -3;
            entry->m_headerIndex = -1;
            entry->m_collapsed = 0;
            entry->m_flags = 0;
            entry->m_enabled = 1;
            entry->m_type = 1;
            entry->m_voiceEnabled = ComSatClient_HasSession(1, "");
            entry->m_count = CGRaidInfo::m_numMembers;

            if (!s_collapsedHeaders[0]) {
                group->m_collapsed = 0;
            }

            count++;
        }
    }

    int32_t zoneHeader = -1;
    int32_t customHeader = -1;

    for (uint32_t header = 1; header < 3; header++) {
        DisplayChannel* entry = &CGChat::s_displayChannels[count];
        entry->m_channelIndex = 0;
        entry->m_headerIndex = header;
        entry->m_collapsed = s_collapsedHeaders[header];
        entry->m_flags = 0;
        entry->m_enabled = 0;
        entry->m_type = 4;
        entry->m_voiceEnabled = 0;
        entry->m_count = 0;

        if (header == 1) {
            zoneHeader = count;
        } else if (header == 2) {
            customHeader = count;
        }

        count++;

        for (uint32_t i = 0; i < CGChat::s_channels.Count(); i++) {
            ChatChannel* channel = &CGChat::s_channels.m_data[i];

            if (!channel->m_channelNumber) {
                continue;
            }

            int32_t parent;

            if (channel->m_zoneChannelID && header == 1) {
                if (!s_collapsedHeaders[1]) {
                    entry = &CGChat::s_displayChannels[count++];
                    entry->m_flags |= 1;
                    entry->m_collapsed = 0;
                    entry->m_channelIndex = i;
                    entry->m_headerIndex = -1;
                    entry->m_enabled = channel->m_unk9C == 0;
                    entry->m_type = 0;
                    entry->m_voiceEnabled = 0;
                    entry->m_count = 0;

                    if (zoneHeader < 0) {
                        continue;
                    }

                    CGChat::s_displayChannels[zoneHeader].m_collapsed = 0;
                }

                parent = zoneHeader;
            } else {
                if (channel->m_zoneChannelID || header != 2) {
                    continue;
                }

                if (!s_collapsedHeaders[2]) {
                    if (channel->m_unkA0 == 1) {
                        continue;
                    }

                    entry = &CGChat::s_displayChannels[count++];
                    entry->m_channelIndex = i;
                    entry->m_collapsed = 0;
                    entry->m_headerIndex = -1;
                    entry->m_flags = 0;
                    entry->m_enabled = channel->m_unkA0 == 0;
                    entry->m_type = 0;
                    entry->m_voiceEnabled = ComSatClient_HasSession(0, channel->m_name);
                    entry->m_count = 0;

                    if (customHeader < 0) {
                        continue;
                    }

                    CGChat::s_displayChannels[customHeader].m_collapsed = 0;
                }

                parent = customHeader;
            }

            if (parent >= 0) {
                CGChat::s_displayChannels[parent].m_count++;
            }
        }
    }

    s_numDisplayChannels = count;
    s_activeVoiceChannel = 0;

    int32_t sessionType = ComSatClient_GetCurrentSessionType();
    const char* sessionName = sessionType ? "" : ComSatClient_GetCurrentSessionName();

    for (uint32_t i = 0; i < s_numDisplayChannels; i++) {
        DisplayChannel* entry = &CGChat::s_displayChannels[i];
        const char* name = entry->m_type ? "" : CGChat::s_channels.m_data[entry->m_channelIndex].m_name;

        if (sessionType == entry->m_type && (sessionType || (sessionName && name && !SStrCmpUTF8I(sessionName, name, STORM_MAX_STR)))) {
            s_activeVoiceChannel = i;
            break;
        }
    }

    switch (s_activeDisplayChannelType) {
        case 2: {
            BuildPartyDisplayList();
            break;
        }

        case 1:
        case 3: {
            BuildRaidDisplayList(s_activeDisplayChannelType);
            break;
        }

        case 0: {
            UserClientChatChannelDisplayList msg(CGChat::s_activeDisplayChannelName);
            ClientServices::Send(&msg);
            break;
        }
    }

    if (signal) {
        FrameScript_SignalEvent(SCRIPT_EVENT_CHANNEL_UI_UPDATE, NULL);
    }
}

// OFFSET: 0x008F1E00/0x140622C50
void CGChat::OnChannelsUpdated() {
    for (uint32_t i = 0; i < CGChat::s_channels.Count(); i++) {
        ChatChannel* channel = &CGChat::s_channels.m_data[i];

        if (!channel->m_channelNumber || channel->m_unk9C) {
            continue;
        }

        ChatChannelsRec* rec = g_chatChannelsDB.GetRecord(channel->m_zoneChannelID);

        if (rec && (rec->m_flags & 0x10000) && (rec->m_flags & 0x2)) {
            break;
        }
    }

    RebuildDisplayChannelList(1);
}

// OFFSET: 0x008F9A60
static void NotifyFlagChanged(WOWGUID guid, const char* name, uint8_t oldFlags, uint8_t newFlags, const char* channel, uint8_t mask, const char* setText, const char* unsetText) {
    if (!(mask & oldFlags) && (newFlags & mask)) {
        ChatMessageParams params;
        params.m_senderName = name;
        params.m_channelName = channel;
        CGChat::AddChatMessage(setText, CHAT_MSG_CHANNEL_NOTICE_USER, &params);
    } else if ((mask & oldFlags) && !(newFlags & mask)) {
        ChatMessageParams params;
        params.m_senderName = name;
        params.m_channelName = channel;
        CGChat::AddChatMessage(unsetText, CHAT_MSG_CHANNEL_NOTICE_USER, &params);
    }
}

// OFFSET: 0x008F9B20/0x14062D050
void CGChat::HandleFlagsChanged(WOWGUID guid, uint8_t oldFlags, uint8_t newFlags, const char* channel) {
    NameCache* record = g_nameDBCache.GetRecord(guid.m_guid, guid, CGChat::NameQueryCallback, NULL, false);

    if (!record) {
        return;
    }

    if (guid == ClntObjMgrGetActivePlayer() && !s_activeDisplayChannelType && channel && !SStrCmpUTF8I(channel, CGChat::s_activeDisplayChannelName, STORM_MAX_STR)) {
        s_activeDisplayChannelFlags = newFlags;
    }

    NotifyFlagChanged(guid, record->m_name, oldFlags, newFlags, channel, 0x2, "SET_MODERATOR", "UNSET_MODERATOR");
    NotifyFlagChanged(guid, record->m_name, oldFlags, newFlags, channel, 0x4, "SET_VOICE", "UNSET_VOICE");
    NotifyFlagChanged(guid, record->m_name, oldFlags, newFlags, channel, 0x10, "SET_SPEAK", "UNSET_SPEAK");
}

// OFFSET: 0x008F7670/0x14062A380
void CGChat::ProcessPendingChatQueue() {
    if (s_queueChat) {
        return;
    }

    PENDINGCHAT* next;

    for (PENDINGCHAT* chat = CGChat::s_pendingChat.Head(); !CGChat::s_pendingChat.IsEnd(chat); chat = next) {
        next = CGChat::s_pendingChat.RawNext(chat);

        if (!chat->m_queued) {
            continue;
        }

        chat->m_queued = 0;
        const char* senderName = NULL;
        const char* targetName = NULL;
        WOWGUID senderGuid = chat->m_senderGuid;

        if (!(senderGuid.m_guid & 0xF000000000000000ULL) && (senderGuid.m_guid & 0xF07FFFFFFFFFFFFFULL)) {
            NameCache* record = g_nameDBCache.GetRecord(senderGuid.m_guid, chat->m_senderGuid, CGChat::NameQueryCallback, NULL, false);

            if (!record) {
                continue;
            }

            senderName = record->m_name;
        } else if ((senderGuid.m_guid & 0xF0F0000000000000ULL) == 0xF040000000000000ULL) {
            PetNameCache* record = g_petNameCache.GetRecord(static_cast<int32_t>((senderGuid.m_guid >> 24) & 0xFFFFFFF), chat->m_senderGuid, CGChat::NameQueryCallback, NULL, false);

            if (!record) {
                continue;
            }

            senderName = record->m_name;
        }

        WOWGUID targetGuid = chat->m_targetGuid;

        if (!(targetGuid.m_guid & 0xF000000000000000ULL) && (targetGuid.m_guid & 0xF07FFFFFFFFFFFFFULL)) {
            NameCache* record = g_nameDBCache.GetRecord(targetGuid.m_guid, chat->m_targetGuid, CGChat::NameQueryCallback, NULL, false);

            if (!record) {
                continue;
            }

            targetName = record->m_name;
        } else if ((targetGuid.m_guid & 0xF0F0000000000000ULL) == 0xF040000000000000ULL) {
            PetNameCache* record = g_petNameCache.GetRecord(static_cast<int32_t>((targetGuid.m_guid >> 24) & 0xFFFFFFF), chat->m_targetGuid, CGChat::NameQueryCallback, NULL, false);

            if (!record) {
                continue;
            }

            targetName = record->m_name;
        }

        ChatMessageParams params;
        params.m_senderName = !senderName || chat->m_isGM ? chat->m_senderName : senderName;
        params.m_language = chat->m_language;
        params.m_channelName = chat->m_channelName;
        params.m_targetName = targetName ? targetName : chat->m_targetName;
        params.m_flags = chat->m_flags;
        params.m_senderGuid = chat->m_senderGuid;
        params.m_senderUnk = chat->m_senderUnk;
        params.m_targetGuid = chat->m_targetGuid;
        params.m_achievementID = chat->m_achievementID;
        params.m_isGM = chat->m_isGM;
        params.m_conversation = chat->m_conversation;
        CGChat::AddChatMessage(chat->m_text, static_cast<SLASH_COMMAND_ID>(chat->m_type), &params);
        CGChat::s_pendingChat.DeleteNode(chat);
    }
}

// OFFSET: 0x008FAC30/0x14062E750
void CGChat::EnterWorld() {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));
    CGChat::m_afk = player && (player->m_player->m_playerFlags & 0x2) ? 1 : 0;
    CGChat::m_dnd = player && (player->m_player->m_playerFlags & 0x4) ? 1 : 0;
    s_queueChat = 0;
    CGChat::ProcessPendingChatQueue();
    SendAddonPrefixes();
    s_queueAddonPrefixes = 0;
}

// OFFSET: 0x008F8280/0x14062B3E0
static void LogGuildReactionCallback(int32_t id, const WOWGUID& guid, void* param, bool success) {
    if (!success) {
        return;
    }

    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player || !player->GetGuildGUID()) {
        return;
    }

    char token[64];
    SStrPrintf(token, sizeof(token), "FACTION_STANDING_LABEL%d", CGReputationInfo::GetFactionStandingReaction(1168) + 1);
    const char* label = player->GetFrameScriptText(token, -1);
    const char* format = FrameScript_GetText("FACTION_STANDING_CHANGED_GUILDNAME", -1, GENDER_NOT_APPLICABLE);
    WOWGUID requester = WOWGUID();
    GuildStats_C* stats = g_guildInfoCache.GetRecord(player->GetGuildGUID().m_guid, requester, NULL, NULL, false);

    if (stats) {
        char text[3000];
        SStrPrintf(text, sizeof(text), format, label, stats->m_name);
        CGChat::AddChatMessage(text, CHAT_MSG_SYSTEM, NULL);
    }
}

// OFFSET: 0x008F8380/0x14062B530
void CGChat::LogReactionChanged(int32_t faction, int32_t reaction) {
    FactionRec* rec = g_factionDB.GetRecord(faction);

    if (!rec) {
        return;
    }

    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return;
    }

    char token[64];
    SStrPrintf(token, sizeof(token), "FACTION_STANDING_LABEL%d", reaction + 1);
    const char* label = player->GetFrameScriptText(token, -1);
    char text[3000];

    if (faction != 1168) {
        SStrPrintf(text, sizeof(text), FrameScript_GetText("FACTION_STANDING_CHANGED", -1, GENDER_NOT_APPLICABLE), label, static_cast<const char*>(rec->m_name));
    } else if (!player->GetGuildGUID()) {
        SStrPrintf(text, sizeof(text), FrameScript_GetText("FACTION_STANDING_CHANGED_GUILD", -1, GENDER_NOT_APPLICABLE), label);
    } else {
        WOWGUID requester = WOWGUID();
        GuildStats_C* stats = g_guildInfoCache.GetRecord(player->GetGuildGUID().m_guid, requester, LogGuildReactionCallback, NULL, false);

        if (!stats) {
            return;
        }

        SStrPrintf(text, sizeof(text), FrameScript_GetText("FACTION_STANDING_CHANGED_GUILDNAME", -1, GENDER_NOT_APPLICABLE), label, stats->m_name);
    }

    CGChat::AddChatMessage(text, CHAT_MSG_SYSTEM, NULL);
}

// OFFSET: 0x008F8160/0x14062B250
void CGChat::LogFactionChanged(int32_t faction, int32_t delta, int32_t generic, float bonus) {
    FactionRec* rec = g_factionDB.GetRecord(faction);

    if (!rec || !delta) {
        return;
    }

    char text[3000];

    if (generic) {
        SStrPrintf(text, sizeof(text), FrameScript_GetText(delta > 0 ? "FACTION_STANDING_INCREASED_GENERIC" : "FACTION_STANDING_DECREASED_GENERIC", -1, GENDER_NOT_APPLICABLE), static_cast<const char*>(rec->m_name));
        CGChat::AddChatMessage(text, CHAT_MSG_COMBAT_FACTION_CHANGE, NULL);
        return;
    }

    const char* token;

    if (delta <= 0) {
        token = "FACTION_STANDING_DECREASED";
    } else if (bonus > 0.0f) {
        token = "FACTION_STANDING_INCREASED_BONUS";
    } else {
        token = "FACTION_STANDING_INCREASED";
    }

    SStrPrintf(text, sizeof(text), FrameScript_GetText(token, -1, GENDER_NOT_APPLICABLE), static_cast<const char*>(rec->m_name), abs(delta), bonus);
    CGChat::AddChatMessage(text, CHAT_MSG_COMBAT_FACTION_CHANGE, NULL);
    SendToCombatText(36, rec->m_name, delta);
}

// OFFSET: 0x008E5BB0/0x140613ED0
int32_t CGChat::IsChatLogging(int32_t type) {
    return s_chatLogging[type];
}

// OFFSET: 0x008E59C0/0x140613C90
ChatRecord* GetChatRecord(int32_t index) {
    return &CGChat::m_chatHistory[static_cast<int32_t>(CGChat::m_currentChatIndex + index) % 60];
}

// OFFSET: 0x008E5B70/0x140613E90
void CGChat::AddCombatMessage(const char* text) {
    if (!text) {
        SErrSetLastError(STORM_ERROR_INVALID_PARAMETER);
        return;
    }

    if (s_chatLogging[1]) {
        SLogWrite(s_chatLog[1], "%s", text);
    }
}

// OFFSET: 0x008F85F0/0x14062B810
void CGChat::LogDismissPet(const WOWGUID& guid, const WOWGUID& pet) {
    char text[3000];

    if (guid == ClntObjMgrGetActivePlayer()) {
        const char* petName = GetCombatLogObjectName(pet);
        SStrPrintf(text, sizeof(text), FrameScript_GetText("SPELLDISMISSPETSELF", -1, GENDER_NOT_APPLICABLE), petName);
    } else {
        SStrPrintf(text, sizeof(text), FrameScript_GetText("SPELLDISMISSPETOTHER", -1, GENDER_NOT_APPLICABLE), GetCombatLogObjectName(guid), GetCombatLogObjectName(pet));
    }

    CGChat::AddChatMessage(text, CHAT_MSG_PET_INFO, NULL);
}

// OFFSET: 0x008E79B0/0x140615DF0
void ChatWindowInfo::RemoveChannel(const char* name) {
    for (uint32_t i = 0; i < this->m_channels.m_count; i++) {
        if (this->m_channels.m_data[i] && !SStrCmpUTF8I(this->m_channels.m_data[i], name, STORM_MAX_STR)) {
            if (this->m_channels.m_data[i]) {
                SMemFree(this->m_channels.m_data[i], __FILE__, __LINE__, 0);
            }

            this->m_channels.m_data[i] = NULL;
            this->m_channelIDs.m_data[i] = 0;
            return;
        }
    }
}

// OFFSET: 0x008E8D50/0x140617520
const char* CGChat::GetChannelString(const char* name) {
    if (!name || !*name) {
        return NULL;
    }

    int32_t number = SStrToInt(name);

    if (!number) {
        return name;
    }

    if (number >= 1 && number <= static_cast<int32_t>(CGChat::s_channels.m_count)) {
        ChatChannel* channel = &CGChat::s_channels.m_data[number - 1];

        if (channel->m_channelNumber == number && !channel->m_unk9C) {
            return channel->m_name;
        }
    }

    return NULL;
}

// OFFSET: 0x008ED090/0x14061C6B0
void ChatWindowInfo::AddChannel(const char* name, int32_t id) {
    for (uint32_t i = 0; i < this->m_channels.m_count; i++) {
        if (this->m_channels.m_data[i] && !SStrCmpUTF8I(this->m_channels.m_data[i], name, STORM_MAX_STR)) {
            return;
        }
    }

    uint32_t index;

    for (index = 0; index < this->m_channels.m_count; index++) {
        if (!this->m_channels.m_data[index]) {
            this->m_channels.m_data[index] = SStrDupA(name, __FILE__, __LINE__);
            this->m_channelIDs.m_data[index] = id;
            return;
        }
    }

    this->m_channels.SetCount(this->m_channels.m_count * 2);
    this->m_channelIDs.SetCount(this->m_channelIDs.m_count * 2);
    this->m_channels.m_data[index] = SStrDupA(name, __FILE__, __LINE__);
    this->m_channelIDs.m_data[index] = id;

    for (uint32_t i = index + 1; i < this->m_channels.m_count; i++) {
        this->m_channels.m_data[i] = NULL;
        this->m_channelIDs.m_data[i] = 0;
    }
}

// OFFSET: 0x008F3E60/0x140625B40
int32_t CGChat::AddChatChannel(const char* name, uint32_t id, bool silent, bool a4, bool a5, uint32_t number, const char* password) {
    if (GetChannelByName(name)) {
        return 0;
    }

    if (id && CGChat::GetChannelByChatChannelID(id)) {
        return 0;
    }

    uint32_t slot;

    if (number && number <= 10) {
        if (number > CGChat::s_channels.m_count) {
            CGChat::s_channels.SetCount(number);
            slot = number - 1;
        } else if (!CGChat::s_channels.m_data[number - 1].m_channelNumber) {
            slot = number - 1;
        } else {
            slot = CGChat::s_channels.m_count;
        }
    } else {
        slot = CGChat::s_channels.m_count;
    }

    if (slot == CGChat::s_channels.m_count) {
        for (slot = 0; slot < CGChat::s_channels.m_count; slot++) {
            if (!CGChat::s_channels.m_data[slot].m_channelNumber) {
                break;
            }
        }

        if (slot == CGChat::s_channels.m_count) {
            if (CGChat::s_channels.m_count == 10) {
                if (!silent) {
                    CGGameUI::DisplayError(ERR_TOO_MANY_CHAT_CHANNELS);
                }

                return 0;
            }

            CGChat::s_channels.SetCount(CGChat::s_channels.m_count + 1);
        }
    }

    ChatChannel* channel = &CGChat::s_channels.m_data[slot];
    channel->m_channelNumber = slot + 1;
    SStrCopy(channel->m_name, name, sizeof(channel->m_name));
    channel->m_zoneChannelID = id;
    channel->m_unk9C = 1;
    channel->m_unkA0 = 0;
    channel->m_unkA4 = a4;
    channel->m_unkA5 = a5;
    SStrCopy(channel->m_password, password, sizeof(channel->m_password));
    channel->m_unk1AC = silent;
    channel->m_joinPending = 0;
    return 1;
}

// OFFSET: 0x008F42F0/0x1406260C0
bool InternalJoinChannel(const char* channelName, const char* password, uint32_t window, const char** zoneChannelName, int32_t* zoneChannelID, bool silent, bool a7, bool a8, uint32_t number) {
    *zoneChannelName = NULL;
    *zoneChannelID = 0;

    char name[256];
    SStrCopy(name, channelName, sizeof(name));
    const char* zoneText = CGGameUI::m_zoneText;
    AreaTableRec* area = g_areaTableDB.GetRecord(CGGameUI::m_areaID);

    for (int32_t i = 0; i < g_chatChannelsDB.m_numRecords; i++) {
        ChatChannelsRec* rec = g_chatChannelsDB.GetRecordByIndex(i);

        if (SStrCmpUTF8I(rec->m_shortcut, channelName, STORM_MAX_STR)) {
            continue;
        }

        const char* text = area && (rec->m_flags & 0x20) && (area->m_flags & 0x100) && s_cityArea ? s_cityArea->m_areaName : zoneText;

        if (!text || !*text) {
            return false;
        }

        *zoneChannelName = rec->m_shortcut;
        *zoneChannelID = rec->m_id;
        SStrPrintf(name, sizeof(name), static_cast<const char*>(rec->m_name), text);
        password = "";
        break;
    }

    ChatChannel* channel = *zoneChannelID ? CGChat::GetChannelByChatChannelID(*zoneChannelID) : GetChannelByName(name);

    if (!channel) {
        if (!CGChat::AddChatChannel(name, *zoneChannelID, silent, a7, a8, number, password)) {
            return false;
        }

        UserClientChatJoinChannel msg(*zoneChannelID, silent, a7, name, password);
        ClientServices::Send(&msg);
    }

    if (window < 10) {
        s_chatWindows[window].AddChannel(*zoneChannelName ? *zoneChannelName : name, *zoneChannelID);
        s_chatSettingsDirty = 1;
    }

    return true;
}

// OFFSET: 0x008F4AE0/0x140626880
void CGChat::ZoneUpdate() {
    if (!CGChat::m_loaded) {
        return;
    }

    if (CGGameUI::m_zoneText && *CGGameUI::m_zoneText) {
        CGChat::m_areaID = CGGameUI::m_areaID;

        if (!CGChat::m_registered) {
            if (!g_areaTableDB.GetRecord(CGGameUI::m_areaID)) {
                return;
            }

            const char* zoneChannelName;
            int32_t zoneChannelID;

            for (int32_t i = 0; i < g_chatChannelsDB.m_numRecords; i++) {
                ChatChannelsRec* rec = g_chatChannelsDB.GetRecordByIndex(i);

                if ((1 << (rec->m_id - 1)) & s_joinedZoneChannels) {
                    InternalJoinChannel(rec->m_shortcut, "", 10, &zoneChannelName, &zoneChannelID, true, false, true, 0);
                }
            }

            if (s_pendingChannelNames.m_count) {
                for (uint32_t i = 0; i < s_pendingChannelNames.m_count; i++) {
                    InternalJoinChannel(s_pendingChannelNames.m_data[i], "", 10, &zoneChannelName, &zoneChannelID, true, s_pendingChannelFlags.m_data[i], true, s_pendingChannelNumbers.m_data[i]);

                    if (s_pendingChannelNames.m_data[i]) {
                        SMemFree(s_pendingChannelNames.m_data[i], __FILE__, __LINE__, 0);
                    }
                }

                s_pendingChannelNames.Clear();
                s_pendingChannelFlags.Clear();
                s_pendingChannelNumbers.Clear();
            }

            CGChat::m_registered = 1;
        }
    }

}

// OFFSET: 0x008F4D80/0x140626B50
void CGChat::LoadChatSettingsCallback(HOSFILE localFile, HOSFILE file, void* param) {
    int32_t version = 0;
    int32_t addedVersion = 0;
    uint32_t allChannels = 0;

    for (int32_t i = 0; i < g_chatChannelsDB.m_numRecords; i++) {
        ChatChannelsRec* rec = g_chatChannelsDB.GetRecordByIndex(i);
        allChannels |= 1 << (rec->m_id - 1);
    }

    char* buffer = NULL;
    uint32_t size = 0;

    if (file != OS_INVALID_FILE) {
        size = static_cast<uint32_t>(OsGetFileSize(file));

        if (size) {
            buffer = static_cast<char*>(alloca(size + 1));
            uint32_t bytesRead;

            if (!OsReadFile(file, buffer, size, &bytesRead)) {
                buffer = NULL;
            }
        }
    }

    if (buffer) {
        buffer[size] = '\0';

        for (uint32_t i = 0; i < s_pendingChannelNames.m_count; i++) {
            if (s_pendingChannelNames.m_data[i]) {
                SMemFree(s_pendingChannelNames.m_data[i], __FILE__, __LINE__, 0);
            }
        }

        s_pendingChannelNames.Clear();
        s_pendingChannelFlags.Clear();
        s_pendingChannelNumbers.Clear();

        int32_t state = 0;
        uint32_t window = 0;
        const char* cursor = buffer;
        char line[128];
        char token[256];
        char word[256];
        char value[256];
        SStrTokenize(&cursor, line, sizeof(line), "\r\n", NULL);

        if (!SStrCmpI(line, "VERSION", SStrLen("VERSION"))) {
            const char* rest = &line[SStrLen("VERSION")];
            SStrTokenize(&rest, value, sizeof(value), " \r\n", NULL);
            version = SStrToInt(value);

            if (version >= 2) {
                SStrTokenize(&cursor, line, sizeof(line), "\r\n", NULL);

                if (!SStrCmpI(line, "ADDEDVERSION", SStrLen("ADDEDVERSION"))) {
                    const char* added = &line[SStrLen("ADDEDVERSION")];
                    SStrTokenize(&added, value, sizeof(value), " \r\n", NULL);
                    addedVersion = SStrToInt(value);
                }
            }
        } else {
            cursor = buffer;
        }

        for (uint32_t i = 0; i < 10; i++) {
            memset(s_chatWindows[i].m_messageGroups, 0, sizeof(s_chatWindows[i].m_messageGroups));
        }

        do {
            SStrTokenize(&cursor, line, sizeof(line), "\r\n", NULL);

            if (!line[0]) {
                continue;
            }

            if (!state) {
                if (!SStrCmpI(line, "CHANNELS", STORM_MAX_STR)) {
                    state = 1;
                    continue;
                }

                if (!SStrCmpI(line, "COLORS", STORM_MAX_STR)) {
                    state = 2;
                    continue;
                }

                if (!SStrCmpI(line, "WINDOW", SStrLen("WINDOW"))) {
                    if (SStrLen(line) < SStrLen("WINDOW") + 2) {
                        continue;
                    }

                    uint32_t index = SStrToInt(&line[SStrLen("WINDOW") + 1]) - 1;

                    if (index > 10) {
                        continue;
                    }

                    window = index;
                    state = 3;
                }

                if (!SStrCmpI(line, "ZONECHANNELS", SStrLen("ZONECHANNELS"))) {
                    const char* rest = &line[SStrLen("ZONECHANNELS")];
                    SStrTokenize(&rest, value, sizeof(value), " \r\n", NULL);
                    s_joinedZoneChannels = SStrToInt(value) & allChannels;
                    continue;
                }

                continue;
            }

            if (state == 3) {
                if (!SStrCmpI(line, "CHANNELS", STORM_MAX_STR)) {
                    state = 4;
                    continue;
                }

                if (!SStrCmpI(line, "MESSAGES", STORM_MAX_STR)) {
                    state = 5;
                    continue;
                }
            }

            if (!SStrCmpI(line, "END", STORM_MAX_STR)) {
                state = state == 4 || state == 5 ? 3 : 0;
                continue;
            }

            const char* rest = line;
            SStrTokenize(&rest, word, sizeof(word), " \r\n", NULL);

            if (!word[0]) {
                continue;
            }

            switch (state) {
                case 1: {
                    if (version < 3) {
                        break;
                    }

                    token[0] = '\0';
                    SStrTokenize(&rest, token, sizeof(token), " \r\n", NULL);
                    value[0] = '\0';
                    SStrTokenize(&rest, value, sizeof(value), " \r\n", NULL);
                    s_pendingChannelNames.SetCount(s_pendingChannelNames.m_count + 1);
                    s_pendingChannelNames.m_data[s_pendingChannelNames.m_count - 1] = SStrDupA(word, __FILE__, __LINE__);
                    s_pendingChannelFlags.SetCount(s_pendingChannelFlags.m_count + 1);
                    s_pendingChannelFlags.m_data[s_pendingChannelFlags.m_count - 1] = atoi(token) != 0;
                    s_pendingChannelNumbers.SetCount(s_pendingChannelNumbers.m_count + 1);
                    s_pendingChannelNumbers.m_data[s_pendingChannelNumbers.m_count - 1] = atoi(value);
                    break;
                }

                case 2: {
                    ChatColorInfo* color = NULL;

                    for (uint32_t i = 0; i < CHAT_MSG_MAX; i++) {
                        if (!SStrCmpI(word, s_chatColors[i].m_name, STORM_MAX_STR)) {
                            color = &s_chatColors[i];
                            break;
                        }
                    }

                    if (!color) {
                        if (SStrCmpI(word, "CHANNEL", SStrLen("CHANNEL"))) {
                            break;
                        }

                        uint32_t index = SStrToInt(&word[SStrLen("CHANNEL")]);

                        if (!index || index > s_channelColors.m_count) {
                            break;
                        }

                        color = &s_channelColors.m_data[index - 1];

                        if (!color) {
                            break;
                        }
                    }

                    SStrTokenize(&rest, word, sizeof(word), " \r\n", NULL);
                    color->m_color[0] = SStrToInt(word);
                    SStrTokenize(&rest, word, sizeof(word), " \r\n", NULL);
                    color->m_color[1] = SStrToInt(word);
                    SStrTokenize(&rest, word, sizeof(word), " \r\n", NULL);
                    color->m_color[2] = SStrToInt(word);
                    SStrTokenize(&rest, word, sizeof(word), " \r\n", NULL);

                    if (word[0] == 'Y' || word[0] == 'N') {
                        color->m_unk48 = word[0] == 'Y';
                    }

                    break;
                }

                case 3: {
                    ChatWindowInfo* info = &s_chatWindows[window];

                    if (!SStrCmpI(word, "NAME", STORM_MAX_STR)) {
                        SStrTokenize(&rest, info->m_name, sizeof(info->m_name), "\r\n", NULL);
                        break;
                    }

                    if (!SStrCmpI(word, "SIZE", STORM_MAX_STR)) {
                        SStrTokenize(&rest, word, sizeof(word), " \r\n", NULL);
                        info->m_unkD0 = SStrToInt(word);
                        break;
                    }

                    if (!SStrCmpI(word, "COLOR", STORM_MAX_STR)) {
                        SStrTokenize(&rest, word, sizeof(word), " \r\n", NULL);
                        info->m_color.r = SStrToInt(word);
                        SStrTokenize(&rest, word, sizeof(word), " \r\n", NULL);
                        info->m_color.g = SStrToInt(word);
                        SStrTokenize(&rest, word, sizeof(word), " \r\n", NULL);
                        info->m_color.b = SStrToInt(word);
                        SStrTokenize(&rest, word, sizeof(word), " \r\n", NULL);
                        info->m_color.a = SStrToInt(word);
                    }

                    if (!SStrCmpI(word, "LOCKED", STORM_MAX_STR)) {
                        SStrTokenize(&rest, word, sizeof(word), " \r\n", NULL);
                        info->m_unkD8 = SStrToInt(word);
                    }

                    if (!SStrCmpI(word, "UNINTERACTABLE", STORM_MAX_STR)) {
                        SStrTokenize(&rest, word, sizeof(word), "\r\n", NULL);
                        info->m_unkE4 = SStrToInt(word);
                    }

                    if (!SStrCmpI(word, "DOCKED", STORM_MAX_STR)) {
                        SStrTokenize(&rest, word, sizeof(word), " \r\n", NULL);
                        info->m_docked = SStrToInt(word);
                    }

                    if (!SStrCmpI(word, "SHOWN", STORM_MAX_STR)) {
                        SStrTokenize(&rest, word, sizeof(word), " \r\n", NULL);
                        info->m_shown = SStrToInt(word);
                    }

                    if (!SStrCmpI(word, "POSITION", STORM_MAX_STR)) {
                        SStrTokenize(&rest, word, sizeof(word), " \r\n", NULL);
                        StringToFramePoint(word, reinterpret_cast<FRAMEPOINT&>(info->m_unkE8));
                        SStrTokenize(&rest, word, sizeof(word), " \r\n", NULL);
                        float x = SStrToFloat(word);
                        SStrTokenize(&rest, word, sizeof(word), " \r\n", NULL);
                        float y = SStrToFloat(word);
                        info->m_unkEC = x;
                        info->m_unkF0 = y;
                        info->m_unkF4 = 1;
                    }

                    if (!SStrCmpI(word, "DIMENSIONS", STORM_MAX_STR)) {
                        SStrTokenize(&rest, word, sizeof(word), " \r\n", NULL);
                        float width = SStrToFloat(word);
                        SStrTokenize(&rest, word, sizeof(word), " \r\n", NULL);
                        float height = SStrToFloat(word);
                        info->m_width = width;
                        info->m_height = height;
                        info->m_unkF4 = 1;
                    }

                    if (!SStrCmpI(word, "ZONECHANNELS", STORM_MAX_STR)) {
                        SStrTokenize(&rest, word, sizeof(word), "\r\n", NULL);
                        uint32_t channels = SStrToInt(word) & allChannels;

                        for (int32_t i = 0; i < g_chatChannelsDB.m_numRecords; i++) {
                            ChatChannelsRec* rec = g_chatChannelsDB.GetRecordByIndex(i);

                            if (channels & (1 << (rec->m_id - 1))) {
                                info->AddChannel(rec->m_shortcut, rec->m_id);
                            }
                        }
                    }

                    break;
                }

                case 4: {
                    s_chatWindows[window].AddChannel(word, 0);
                    break;
                }

                case 5: {
                    for (uint32_t i = 0; i < sizeof(s_chatMessageGroups) / sizeof(s_chatMessageGroups[0]); i++) {
                        if (!SStrCmpI(word, s_chatMessageGroups[i].m_name, STORM_MAX_STR)) {
                            s_chatWindows[window].m_messageGroups[i] = 1;
                            break;
                        }
                    }

                    break;
                }

                default: {
                    break;
                }
            }
        } while (cursor && *cursor);

        if (addedVersion < 16) {
            for (uint32_t i = 0; i < sizeof(s_zoneChannelDefaults) / sizeof(s_zoneChannelDefaults[0]); i++) {
                if (s_zoneChannelDefaults[i].m_addedVersion <= addedVersion) {
                    continue;
                }

                ChatChannelsRec* rec = g_chatChannelsDB.GetRecord(s_zoneChannelDefaults[i].m_channelID);

                if (!rec || !(rec->m_flags & 1)) {
                    continue;
                }

                uint32_t bit = 1 << (rec->m_id - 1);

                if (!(bit & s_joinedZoneChannels)) {
                    s_joinedZoneChannels |= bit;
                    s_chatWindows[0].AddChannel(rec->m_shortcut, rec->m_id);
                }
            }

            for (uint32_t i = 0; i < sizeof(s_chatMessageGroups) / sizeof(s_chatMessageGroups[0]); i++) {
                if (s_chatMessageGroups[i].m_default && s_chatMessageGroups[i].m_type > addedVersion) {
                    s_chatWindows[i < 42 ? 0 : 1].m_messageGroups[i] = 1;
                }
            }

            for (uint32_t i = 0; i < CHAT_MSG_MAX; i++) {
                if (s_chatTypeInfo[i].m_unk44 > addedVersion) {
                    s_chatColors[i].m_color[0] = s_chatTypeInfo[i].m_color[0];
                    s_chatColors[i].m_color[1] = s_chatTypeInfo[i].m_color[1];
                    s_chatColors[i].m_color[2] = s_chatTypeInfo[i].m_color[2];
                    s_chatColors[i].m_unk48 = s_chatTypeInfo[i].m_unk48;
                }
            }
        }

        FrameScript_SignalEvent(SCRIPT_EVENT_UPDATE_CHAT_WINDOWS, NULL);

        for (uint32_t i = 0; i < CHAT_MSG_MAX; i++) {
            FrameScript_SignalEvent(SCRIPT_EVENT_UPDATE_CHAT_COLOR, "%s%f%f%f", static_cast<const char*>(s_chatColors[i].m_name), s_chatColors[i].m_color[0] * (1.0f / 255.0f), s_chatColors[i].m_color[1] * (1.0f / 255.0f), s_chatColors[i].m_color[2] * (1.0f / 255.0f));
            FrameScript_SignalEvent(SCRIPT_EVENT_UPDATE_CHAT_COLOR_NAME_BY_CLASS, "%s%b", static_cast<const char*>(s_chatColors[i].m_name), s_chatColors[i].m_unk48);
        }

        for (uint32_t i = 0; i < s_channelColors.m_count; i++) {
            ChatColorInfo* color = &s_channelColors.m_data[i];
            FrameScript_SignalEvent(SCRIPT_EVENT_UPDATE_CHAT_COLOR, "%s%f%f%f", static_cast<const char*>(color->m_name), color->m_color[0] * (1.0f / 255.0f), color->m_color[1] * (1.0f / 255.0f), color->m_color[2] * (1.0f / 255.0f));
            FrameScript_SignalEvent(SCRIPT_EVENT_UPDATE_CHAT_COLOR_NAME_BY_CLASS, "%s%b", static_cast<const char*>(color->m_name), color->m_unk48);
        }
    } else {
        for (int32_t i = 0; i < g_chatChannelsDB.m_numRecords; i++) {
            ChatChannelsRec* rec = g_chatChannelsDB.GetRecordByIndex(i);

            if (!(rec->m_flags & 1)) {
                continue;
            }

            s_joinedZoneChannels |= 1 << (rec->m_id - 1);
            s_chatWindows[0].AddChannel(rec->m_shortcut, rec->m_id);
        }

        FrameScript_SignalEvent(SCRIPT_EVENT_UPDATE_CHAT_WINDOWS, NULL);

        for (uint32_t i = 0; i < CHAT_MSG_MAX; i++) {
            FrameScript_SignalEvent(SCRIPT_EVENT_UPDATE_CHAT_COLOR, "%s%f%f%f", static_cast<const char*>(s_chatColors[i].m_name), s_chatColors[i].m_color[0] * (1.0f / 255.0f), s_chatColors[i].m_color[1] * (1.0f / 255.0f), s_chatColors[i].m_color[2] * (1.0f / 255.0f));
            FrameScript_SignalEvent(SCRIPT_EVENT_UPDATE_CHAT_COLOR_NAME_BY_CLASS, "%s%b", static_cast<const char*>(s_chatColors[i].m_name), s_chatColors[i].m_unk48);
        }

        for (uint32_t i = 0; i < s_channelColors.m_count; i++) {
            ChatColorInfo* color = &s_channelColors.m_data[i];
            FrameScript_SignalEvent(SCRIPT_EVENT_UPDATE_CHAT_COLOR, "%s%f%f%f", static_cast<const char*>(color->m_name), color->m_color[0] * (1.0f / 255.0f), color->m_color[1] * (1.0f / 255.0f), color->m_color[2] * (1.0f / 255.0f));
            FrameScript_SignalEvent(SCRIPT_EVENT_UPDATE_CHAT_COLOR_NAME_BY_CLASS, "%s%b", static_cast<const char*>(color->m_name), color->m_unk48);
        }
    }

    CGChat::m_loaded = 1;
    CGChat::ZoneUpdate();
}

// OFFSET: 0x008F4530/0x140626430
int32_t InternalLeaveChannel(const char* channelName, bool force) {
    const char* channelString = force ? channelName : CGChat::GetChannelString(channelName);

    if (!channelString) {
        return 0;
    }

    const char* zoneChannelName = NULL;
    char name[256];
    SStrCopy(name, channelString, sizeof(name));
    const char* zoneText = CGGameUI::m_zoneText;
    int32_t zoneChannelID = 0;
    AreaTableRec* area = g_areaTableDB.GetRecord(CGGameUI::m_areaID);

    for (int32_t i = 0; i < g_chatChannelsDB.m_numRecords; i++) {
        ChatChannelsRec* rec = g_chatChannelsDB.GetRecordByIndex(i);

        if (SStrCmpUTF8I(rec->m_shortcut, channelName, STORM_MAX_STR)) {
            continue;
        }

        const char* text = area && (rec->m_flags & 0x20) && (area->m_flags & 0x100) && s_cityArea ? s_cityArea->m_areaName : zoneText;

        if (!text || !*text) {
            return 0;
        }

        zoneChannelName = rec->m_shortcut;
        zoneChannelID = rec->m_id;
        SStrPrintf(name, sizeof(name), static_cast<const char*>(rec->m_name), text);
        break;
    }

    for (uint32_t i = 0; i < 10; i++) {
        s_chatWindows[i].RemoveChannel(zoneChannelName ? zoneChannelName : name);
    }

    ChatChannel* channel = zoneChannelID ? CGChat::GetChannelByChatChannelID(zoneChannelID) : GetChannelByName(name);

    if (channel && (!channel->m_unk9C || force)) {
        UserClientChatLeaveChannel msg(channel->m_zoneChannelID, channel->m_name);
        ClientServices::Send(&msg);

        uint32_t id = channel->m_zoneChannelID;
        channel->m_unkA0 = 1;

        if (id) {
            s_joinedZoneChannels &= ~(1 << (id - 1));
        }

        if (channel->m_unk9C == 1) {
            ChatChannel* entry = id ? CGChat::GetChannelByChatChannelID(id) : GetChannelByName(channel->m_name);

            if (entry) {
                entry->m_channelNumber = 0;
                entry->m_name[0] = '\0';
                entry->m_zoneChannelID = 0;
                entry->m_unk9C = 1;
                entry->m_unkA0 = 1;
            }

            CGChat::OnChannelsUpdated();
        }
    }

    s_chatSettingsDirty = 1;
    return 0;
}

// OFFSET: 0x008F5DF0/0x140627EA0
void CGChat::LoadChatSettings() {
    LoadAccountData(7, CGChat::LoadChatSettingsCallback, NULL);
}

// OFFSET: 0x008E86C0/0x140616E50
const char* CGChat::GetChannelShortName(int32_t number, bool includeInactive) {
    if (number < 1 || number > static_cast<int32_t>(CGChat::s_channels.m_count)) {
        return NULL;
    }

    ChatChannel* channel = &CGChat::s_channels.m_data[number - 1];

    if (channel->m_channelNumber != number) {
        return NULL;
    }

    if (!includeInactive && channel->m_unk9C) {
        return NULL;
    }

    if (channel->m_zoneChannelID) {
        ChatChannelsRec* rec = g_chatChannelsDB.GetRecord(channel->m_zoneChannelID);

        if (rec) {
            return rec->m_shortcut;
        }
    }

    return channel->m_name;
}

// OFFSET: 0x008EA7E0/0x140619610
uint32_t CGChat::FindDisplayChannelIndex(const char* name, int32_t type) {
    for (uint32_t i = 0; i < s_numDisplayChannels; i++) {
        if (static_cast<uint32_t>(CGChat::s_displayChannels[i].m_channelIndex) > CGChat::s_channels.m_count || CGChat::s_displayChannels[i].m_headerIndex >= 0) {
            continue;
        }

        int32_t displayType;
        const char* channel = CGChat::GetChannelStringForDisplay(i, &displayType);

        if (!channel || displayType == 4 || displayType != type) {
            continue;
        }

        if (displayType) {
            return i + 1;
        }

        if (name && !SStrCmpUTF8I(channel, name, STORM_MAX_STR)) {
            return i + 1;
        }
    }

    return 0;
}

// OFFSET: 0x008E5D50/0x140614080
bool CGChat::ChatTokenToGUID(const char* token, WOWGUID& guid) {
    NameCache* rec = g_nameDBCache.GetRecordByName(token);

    if (!rec) {
        return Script_GetGUIDFromToken(token, guid, false, false);
    }

    guid = rec->m_guid;
    return true;
}

// OFFSET: 0x008E8F00/0x140617700
CImVector CGChat::GetChatColor(int32_t type) {
    CImVector color;

    if (type < CHAT_MSG_MAX) {
        color.b = s_chatColors[type].m_color[2];
        color.g = s_chatColors[type].m_color[1];
        color.a = 0xFF;
        color.r = s_chatColors[type].m_color[0];
    } else {
        color.b = 0xFF;
        color.g = 0xFF;
        color.a = 0xFF;
        color.r = 0xFF;
    }

    return color;
}

// OFFSET: 0x008E6200/0x140614460
int32_t FormatTime(char* buffer, uint32_t size, int32_t seconds) {
    int32_t days = seconds / 86400;
    int32_t hours = seconds % 86400 / 3600;
    int32_t minutes = (seconds - 3600 * (hours + 24 * days)) / 60;
    return SStrPrintf(buffer, size, "%dd %dh %dm %ds", days, hours, minutes, seconds % 60);
}

// OFFSET: 0x008EC240/0x14061B440
void ChatWindowInfo::Reset() {
    for (uint32_t i = 0; i < this->m_channels.m_count; i++) {
        if (this->m_channels.m_data[i]) {
            SMemFree(this->m_channels.m_data[i], __FILE__, __LINE__, 0);
        }

        this->m_channels.m_data[i] = NULL;
    }

    this->m_channels.Clear();
    this->m_channelIDs.Clear();
}

// OFFSET: 0x008F2010/0x140622F60
void CGChat::JoiningParty(bool raid, int32_t pvp) {
    bool voiceEnabled = s_cvarEnableVoiceChat && s_cvarEnableVoiceChat->m_intValue;
    int32_t disabled = ComSatClient_IsComsatDisabledByClient();

    if (!CGChat::m_voiceChatSystemStatus || !voiceEnabled || disabled) {
        return;
    }

    if (pvp) {
        if (g_autojoinBGVoiceCVar->m_intValue && ComSatClient_GetCurrentSessionType() == 4) {
            CGChat::SetActiveVoiceSession("", 1, -1);
            RebuildDisplayChannelList(0);
        }
    } else if (g_autojoinPartyVoiceCVar->m_intValue && ComSatClient_GetCurrentSessionType() == 4) {
        CGChat::SetActiveVoiceSession("", raid ? 3 : 2, -1);
        RebuildDisplayChannelList(0);
    }
}

// OFFSET: 0x008F38A0/0x1406253C0
void CGChat::ShutdownGame() {
    for (uint32_t i = 0; i < 2; i++) {
        if (s_chatLog[i]) {
            SLogClose(s_chatLog[i]);
        }

        s_chatLog[i] = NULL;
        s_chatLogging[i] = 0;
    }

    s_wordLists.Clear();
    s_profanityCache.Clear();
    s_spamCache.Clear();
    CGChat::s_pendingChat.Clear();
    CGChat::s_channels.SetCount(0);
    s_channelColors.Clear();
    s_queueAddonPrefixes = 0;
    s_addonPrefixMessage.m_prefixes.Clear();
    s_addonPrefixes.Clear();
    s_numAddonPrefixes = 0;
    s_pendingReportTarget.m_guid = 0;

    for (uint32_t i = 0; i < 10; i++) {
        s_chatWindows[i].Reset();
    }

    for (uint32_t i = 0; i < s_pendingChannelNames.m_count; i++) {
        if (s_pendingChannelNames.m_data[i]) {
            SMemFree(s_pendingChannelNames.m_data[i], __FILE__, __LINE__, 0);
        }
    }

    s_pendingChannelNames.Clear();
    s_pendingChannelFlags.Clear();
    s_pendingChannelNumbers.Clear();
    s_channelThrottle.Clear();
    CGChat::s_throttledChat.Clear();
    ClientServices::ClearMessageHandler(SMSG_CHANNEL_NOTIFY);
    ClientServices::ClearMessageHandler(SMSG_CHAT_DOWN);
    ClientServices::ClearMessageHandler(SMSG_CHAT_IS_DOWN);
    ClientServices::ClearMessageHandler(SMSG_CHAT_RECONNECT);
    ClientServices::ClearMessageHandler(SMSG_CHANNEL_LIST);
    ClientServices::ClearMessageHandler(SMSG_CHAT);
    ClientServices::ClearMessageHandler(SMSG_TEXT_EMOTE);
    ClientServices::ClearMessageHandler(SMSG_ZONE_UNDER_ATTACK);
    ClientServices::ClearMessageHandler(SMSG_DEFENSE_MESSAGE);
    ClientServices::ClearMessageHandler(SMSG_CHAT_SERVER_MESSAGE);
    ClientServices::ClearMessageHandler(SMSG_RAID_INSTANCE_MESSAGE);
    ClientServices::ClearMessageHandler(SMSG_INSTANCE_RESET);
    ClientServices::ClearMessageHandler(SMSG_INSTANCE_RESET_FAILED);
    ClientServices::ClearMessageHandler(SMSG_UPDATE_LAST_INSTANCE);
    ClientServices::ClearMessageHandler(SMSG_UPDATE_INSTANCE_OWNERSHIP);
    ClientServices::ClearMessageHandler(SMSG_EXPECTED_SPAM_RECORDS);
    ClientServices::ClearMessageHandler(SMSG_TITLE_EARNED);
    ClientServices::ClearMessageHandler(SMSG_RESET_FAILED_NOTIFY);
    ClientServices::ClearMessageHandler(SMSG_GM_MESSAGECHAT);
    ClientServices::ClearMessageHandler(SMSG_LOG_XP_GAIN);
    ClientServices::ClearMessageHandler(SMSG_DURABILITY_DAMAGE_DEATH);
    ClientServices::ClearMessageHandler(SMSG_CHANNEL_MEMBER_COUNT);
    ClientServices::ClearMessageHandler(SMSG_COMSAT_RECONNECT_TRY);
    ClientServices::ClearMessageHandler(SMSG_COMSAT_DISCONNECT);
    ClientServices::ClearMessageHandler(SMSG_COMSAT_CONNECT_FAIL);
    ClientServices::ClearMessageHandler(SMSG_VOICE_CHAT_STATUS);
    ClientServices::ClearMessageHandler(SMSG_USERLIST_ADD);
    ClientServices::ClearMessageHandler(SMSG_USERLIST_REMOVE);
    ClientServices::ClearMessageHandler(SMSG_USERLIST_UPDATE);
    ClientServices::ClearMessageHandler(SMSG_VOICESESSION_FULL);
    ClientServices::ClearMessageHandler(SMSG_SERVER_FIRST_ACHIEVEMENT);
    ClientClearBossEmotes::s_handler = NULL;
}

// OFFSET: 0x008EB670/0x14061A6E0
int32_t CGChat::OnUserListUpdate(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    WOWGUID guid;
    uint8_t memberFlags;
    uint8_t channelFlags;
    uint32_t count;
    char channelName[128];
    msg->Get(guid.m_guid);
    msg->Get(memberFlags);
    msg->Get(channelFlags);
    msg->Get(count);
    msg->GetString(channelName, sizeof(channelName));

    if (s_activeDisplayChannelType || SStrCmpUTF8I(channelName, CGChat::s_activeDisplayChannelName, STORM_MAX_STR)) {
        msg->m_read = msg->m_size;
        return 1;
    }

    uint32_t displayIndex = CGChat::FindDisplayChannelIndex(channelName, 0);

    if (displayIndex) {
        ChatChannel* channel = &CGChat::s_channels.m_data[CGChat::s_displayChannels[displayIndex - 1].m_channelIndex];

        if (channel->m_flags != channelFlags) {
            channel->m_flags = channelFlags;
            FrameScript_SignalEvent(SCRIPT_EVENT_CHANNEL_FLAGS_UPDATED, "%d", displayIndex);
        }
    }

    for (uint32_t i = 0; i < s_pendingUsers.m_count; i++) {
        PENDINGUSERLIST* user = &s_pendingUsers.m_data[i];

        if (user->m_guid != guid) {
            continue;
        }

        user->m_flags = memberFlags;

        if (ComSatClient_IsMemberInVoiceSession(user->m_guid, 0, channelName)) {
            user->m_flags |= 0x40;
        }

        if (ComSatClient_GetChannelMemberVoiceStatus(user->m_guid, 0, channelName)) {
            user->m_flags |= 0x80;
        }

        if (ComSatClient_IsPlayerMuted(user->m_guid) || ComSatClient_IsTalkerSquelchedInChannel(user->m_guid, 0, channelName)) {
            user->m_flags |= 0x20;
        }

        if (displayIndex) {
            CGChat::s_displayChannels[displayIndex - 1].m_count = s_pendingUsers.m_count;
            FrameScript_SignalEvent(SCRIPT_EVENT_CHANNEL_COUNT_UPDATE, "%d%d", displayIndex, s_pendingUsers.m_count);
            FrameScript_SignalEvent(SCRIPT_EVENT_CHANNEL_ROSTER_UPDATE, "%d%d", displayIndex, s_pendingUsers.m_count);
        }

        break;
    }

    return 1;
}

// OFFSET: 0x008E7A50/0x140615EB0
void CGChat::SaveChatSettingsCallback(HOSFILE localFile, HOSFILE file, void* param) {
    if (file == OS_INVALID_FILE) {
        return;
    }

    uint32_t written = 0;
    char line[256];
    SStrPrintf(line, sizeof(line), "VERSION %d\n\n", 5);
    OsWriteFile(file, line, SStrLen(line), &written);
    SStrPrintf(line, sizeof(line), "ADDEDVERSION %d\n\n", 16);
    OsWriteFile(file, line, SStrLen(line), &written);
    OsWriteFile(file, "CHANNELS\n", SStrLen("CHANNELS\n"), &written);

    for (uint32_t i = 0; i < CGChat::s_channels.m_count; i++) {
        ChatChannel* channel = &CGChat::s_channels.m_data[i];

        if (channel->m_channelNumber && (!channel->m_unk9C || !channel->m_unkA0) && !channel->m_zoneChannelID && channel->m_unkA5) {
            SStrPrintf(line, sizeof(line), "%s %d %d\n", static_cast<const char*>(channel->m_name), channel->m_unkA4 != 0, channel->m_channelNumber);
            OsWriteFile(file, line, SStrLen(line), &written);
        }
    }

    OsWriteFile(file, "END\n\n", SStrLen("END\n\n"), &written);
    SStrPrintf(line, sizeof(line), "ZONECHANNELS %d\n\n", s_joinedZoneChannels);
    OsWriteFile(file, line, SStrLen(line), &written);
    OsWriteFile(file, "COLORS\n", SStrLen("COLORS\n"), &written);

    for (uint32_t i = 0; i < CHAT_MSG_MAX; i++) {
        ChatColorInfo* color = &s_chatColors[i];

        if (color->m_name[0]) {
            SStrPrintf(line, sizeof(line), "%s %d %d %d %c\n", static_cast<const char*>(color->m_name), color->m_color[0], color->m_color[1], color->m_color[2], color->m_unk48 ? 'Y' : 'N');
            OsWriteFile(file, line, SStrLen(line), &written);
        }
    }

    for (uint32_t i = 0; i < s_channelColors.m_count; i++) {
        ChatColorInfo* color = &s_channelColors.m_data[i];

        if (color->m_name[0]) {
            SStrPrintf(line, sizeof(line), "%s %d %d %d %c\n", static_cast<const char*>(color->m_name), color->m_color[0], color->m_color[1], color->m_color[2], color->m_unk48 ? 'Y' : 'N');
            OsWriteFile(file, line, SStrLen(line), &written);
        }
    }

    OsWriteFile(file, "END\n\n", SStrLen("END\n\n"), &written);

    for (uint32_t w = 0; w < 10; w++) {
        ChatWindowInfo* info = &s_chatWindows[w];
        SStrPrintf(line, sizeof(line), "WINDOW %d\n", w + 1);
        OsWriteFile(file, line, SStrLen(line), &written);

        if (info->m_name[0]) {
            SStrPrintf(line, sizeof(line), "NAME %s\n", static_cast<const char*>(info->m_name));
            OsWriteFile(file, line, SStrLen(line), &written);
        }

        SStrPrintf(line, sizeof(line), "SIZE %d\n", info->m_unkD0);
        OsWriteFile(file, line, SStrLen(line), &written);
        SStrPrintf(line, sizeof(line), "COLOR %d %d %d %d\n", info->m_color.r, info->m_color.g, info->m_color.b, info->m_color.a);
        OsWriteFile(file, line, SStrLen(line), &written);
        SStrPrintf(line, sizeof(line), "LOCKED %d\n", info->m_unkD8 != 0);
        OsWriteFile(file, line, SStrLen(line), &written);
        SStrPrintf(line, sizeof(line), "UNINTERACTABLE %d\n", info->m_unkE4 != 0);
        OsWriteFile(file, line, SStrLen(line), &written);
        SStrPrintf(line, sizeof(line), "DOCKED %d\n", info->m_docked);
        OsWriteFile(file, line, SStrLen(line), &written);
        SStrPrintf(line, sizeof(line), "SHOWN %d\n", info->m_shown != 0);
        OsWriteFile(file, line, SStrLen(line), &written);

        if (info->m_unkF4) {
            SStrPrintf(line, sizeof(line), "POSITION %s %f %f\n", FramePointToString(static_cast<FRAMEPOINT>(info->m_unkE8)), info->m_unkEC, info->m_unkF0);
            OsWriteFile(file, line, SStrLen(line), &written);
            SStrPrintf(line, sizeof(line), "DIMENSIONS %f %f\n\n", info->m_width, info->m_height);
            OsWriteFile(file, line, SStrLen(line), &written);
        }

        OsWriteFile(file, "MESSAGES\n", SStrLen("MESSAGES\n"), &written);

        for (uint32_t i = 0; i < sizeof(info->m_messageGroups); i++) {
            if (info->m_messageGroups[i]) {
                SStrPrintf(line, sizeof(line), "%s\n", static_cast<const char*>(s_chatMessageGroups[i].m_name));
                OsWriteFile(file, line, SStrLen(line), &written);
            }
        }

        OsWriteFile(file, "END\n\n", SStrLen("END\n\n"), &written);
        OsWriteFile(file, "CHANNELS\n", SStrLen("CHANNELS\n"), &written);
        uint32_t zoneChannels = 0;

        for (uint32_t i = 0; i < info->m_channels.m_count; i++) {
            if (!info->m_channels.m_data[i]) {
                continue;
            }

            int32_t channelId = info->m_channelIDs.m_data[i];

            if (channelId) {
                zoneChannels |= 1 << (channelId - 1);
            }

            for (uint32_t j = 0; j < CGChat::s_channels.m_count; j++) {
                if (!SStrCmpUTF8I(CGChat::s_channels.m_data[j].m_name, info->m_channels.m_data[i], STORM_MAX_STR) && !CGChat::s_channels.m_data[j].m_unkA0) {
                    SStrPrintf(line, sizeof(line), "%s\n", info->m_channels.m_data[i]);
                    OsWriteFile(file, line, SStrLen(line), &written);
                    break;
                }
            }
        }

        OsWriteFile(file, "END\n\n", SStrLen("END\n\n"), &written);
        SStrPrintf(line, sizeof(line), "ZONECHANNELS %d\n\n", zoneChannels & s_joinedZoneChannels);
        OsWriteFile(file, line, SStrLen(line), &written);
        OsWriteFile(file, "END\n\n", SStrLen("END\n\n"), &written);
    }
}

// OFFSET: 0x008EAFC0/0x140619F10
void CGChat::SaveChatSettings() {
    s_chatSettingsDirty = 0;
    SaveAccountData(7, CGChat::SaveChatSettingsCallback, NULL);
}

// OFFSET: 0x008E6300/0x1406145A0
int32_t CGChat::GetActiveVoiceSession() {
    return s_activeVoiceChannel;
}

// OFFSET: 0x008EB3F0/0x14061A430
int32_t CGChat::SetActiveVoiceSession(const char* name, int32_t type, int32_t index) {
    if (type == 4) {
        if (CGChat::s_displayChannels[s_activeVoiceChannel].m_voiceEnabled) {
            FrameScript_SignalEvent(SCRIPT_EVENT_CHANNEL_VOICE_UPDATE, "%d%d", s_activeVoiceChannel + 1, CGChat::s_displayChannels[s_activeVoiceChannel].m_voiceEnabled);
        } else {
            FrameScript_SignalEvent(SCRIPT_EVENT_CHANNEL_VOICE_UPDATE, "%d", s_activeVoiceChannel + 1);
        }

        s_activeVoiceChannel = 0;
        ComSatClient_DisableCurrentSession();
        return 1;
    }

    if (index < 0) {
        uint32_t display = CGChat::FindDisplayChannelIndex(name, type);
        index = display ? display - 1 : 0;
    }

    int32_t result = ComSatClient_SetCurrentSession(type, name);

    if (!result) {
        return result;
    }

    if (s_activeVoiceChannel == index) {
        return 1;
    }

    if (s_activeVoiceChannel) {
        int32_t enabled = CGChat::s_displayChannels[s_activeVoiceChannel].m_voiceEnabled;

        if (enabled) {
            FrameScript_SignalEvent(SCRIPT_EVENT_CHANNEL_VOICE_UPDATE, "%d%d", s_activeVoiceChannel + 1, enabled);
        } else {
            FrameScript_SignalEvent(SCRIPT_EVENT_CHANNEL_VOICE_UPDATE, "%d", s_activeVoiceChannel + 1);
        }
    }

    if (index) {
        int32_t enabled = CGChat::s_displayChannels[index].m_voiceEnabled;

        if (enabled) {
            FrameScript_SignalEvent(SCRIPT_EVENT_CHANNEL_VOICE_UPDATE, "%d%d%d", index + 1, enabled, 1);
            s_activeVoiceChannel = index;
            return 1;
        }

        FrameScript_SignalEvent(SCRIPT_EVENT_CHANNEL_VOICE_UPDATE, "%d", index + 1);
    }

    s_activeVoiceChannel = index;
    return 1;
}

// OFFSET: 0x008E7270
bool ChannelNamesEqual(const char* activeName, const char* name, int32_t activeType, int32_t type) {
    return type == activeType && (type || (name && activeName && !SStrCmpUTF8I(name, activeName, STORM_MAX_STR)));
}

// OFFSET: 0x008EB1F0/0x14061A170
void CGChat::NameDisplayQueryCallback(int32_t id, const WOWGUID& guid, void* param, bool success) {
    if (!s_displayChannelMemberCount) {
        return;
    }

    for (uint32_t i = 0; i < s_pendingUsers.Count(); i++) {
        if (s_pendingUsers.m_data[i].m_guid == guid) {
            if (!--s_displayChannelMemberCount) {
                qsort(s_pendingUsers.m_data, s_pendingUsers.Count(), sizeof(PENDINGUSERLIST), QSortDisplayListMembers);
                uint32_t index = CGChat::FindDisplayChannelIndex(CGChat::s_activeDisplayChannelName, s_activeDisplayChannelType);

                if (index) {
                    CGChat::s_displayChannels[index - 1].m_count = s_pendingUsers.Count();
                    FrameScript_SignalEvent(SCRIPT_EVENT_CHANNEL_COUNT_UPDATE, "%d%d", index, s_pendingUsers.Count());
                    FrameScript_SignalEvent(SCRIPT_EVENT_CHANNEL_ROSTER_UPDATE, "%d%d", index, s_pendingUsers.Count());
                }
            }

            return;
        }
    }
}

// OFFSET: 0x008EFD70/0x140620170
int32_t CGChat::OnDisplayChannelList(CDataStore* msg) {
    char name[128];
    msg->GetString(name, sizeof(name));

    if (s_activeDisplayChannelType || SStrCmpUTF8I(name, CGChat::s_activeDisplayChannelName, STORM_MAX_STR)) {
        msg->m_read = msg->m_size;
        return 1;
    }

    uint8_t flags;
    uint32_t count;
    msg->Get(flags);
    msg->Get(count);
    s_displayChannelMemberCount = 0;
    s_pendingUsers.Clear();
    s_activeDisplayChannelFlags = 0;
    s_pendingUsers.SetCount(count);

    uint32_t index = CGChat::FindDisplayChannelIndex(name, 0);

    if (index) {
        ChatChannel* channel = &CGChat::s_channels.m_data[CGChat::s_displayChannels[index - 1].m_channelIndex];

        if (channel->m_flags != flags) {
            channel->m_flags = flags;
            FrameScript_SignalEvent(SCRIPT_EVENT_CHANNEL_FLAGS_UPDATED, "%d", index);
        }
    }

    for (uint32_t i = 0; i < count; i++) {
        PENDINGUSERLIST* user = &s_pendingUsers.m_data[i];
        WOWGUID guid;
        uint8_t userFlags;
        msg->Get(guid.m_guid);
        msg->Get(userFlags);

        if (user) {
            user->m_guid = guid;
            user->m_flags = userFlags;

            if (ComSatClient_IsMemberInVoiceSession(user->m_guid, 0, name)) {
                user->m_flags |= 0x40;
            }

            if (ComSatClient_GetChannelMemberVoiceStatus(user->m_guid, 0, name)) {
                user->m_flags |= 0x80;
            }

            if (ComSatClient_IsPlayerMuted(user->m_guid) || ComSatClient_IsTalkerSquelchedInChannel(user->m_guid, 0, name)) {
                user->m_flags |= 0x20;
            }
        }

        if (ClntObjMgrGetActivePlayer() && static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__))->m_obj->m_guid) {
            if (user->m_guid == static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__))->m_obj->m_guid) {
                s_activeDisplayChannelFlags = user->m_flags;

                if (index) {
                    FrameScript_SignalEvent(SCRIPT_EVENT_CHANNEL_FLAGS_UPDATED, "%d", index);
                }
            }
        }

        if (!g_nameDBCache.GetRecord(user->m_guid.m_guid, user->m_guid, CGChat::NameDisplayQueryCallback, NULL, false)) {
            s_displayChannelMemberCount++;
        }
    }

    if (!s_displayChannelMemberCount) {
        qsort(s_pendingUsers.m_data, count, sizeof(PENDINGUSERLIST), QSortDisplayListMembers);

        if (index) {
            CGChat::s_displayChannels[index - 1].m_count = count;
            FrameScript_SignalEvent(SCRIPT_EVENT_CHANNEL_COUNT_UPDATE, "%d%d", index, count);
            FrameScript_SignalEvent(SCRIPT_EVENT_CHANNEL_ROSTER_UPDATE, "%d%d", index, count);
        }
    }

    return 1;
}

// OFFSET: 0x008EEA20/0x14061E5E0
ChatChannel::ChatChannel() {
    this->m_channelNumber = 0;
    this->m_name[0] = 0;
    this->m_pendingUsers.Clear();
    this->m_flags = 0;
    this->m_zoneChannelID = 0;
    this->m_unk98 = 0;
    this->m_unk9C = 1;
    this->m_unkA0 = 1;
    this->m_unkA4 = false;
    this->m_unkA5 = 0;
    memset(this->m_messageTimes, 0, sizeof(this->m_messageTimes));
    this->m_messageTimeIndex = 0;
    this->m_password[0] = 0;
    this->m_unk1AC = false;
    this->m_joinPending = 0;
}

// OFFSET: 0x008E5890/0x140613C00
void CGChat::LeaveWorld() {
    s_queueChat = 1;
}

// OFFSET: 0x008E59F0
static uint32_t GetComplaintPriority(int32_t type, uint32_t channel) {
    switch (type) {
        case CHAT_MSG_WHISPER:
        case CHAT_MSG_BATTLENET:
            return 0;

        case CHAT_MSG_CHANNEL:
            return channel ? 1 : 2;

        case CHAT_MSG_BATTLEGROUND_LEADER:
            return 3;

        case CHAT_MSG_BATTLEGROUND:
            return 4;

        case CHAT_MSG_RAID_LEADER:
            return 5;

        case CHAT_MSG_RAID_WARNING:
            return 6;

        case CHAT_MSG_RAID:
            return 7;

        case CHAT_MSG_PARTY_LEADER:
            return 8;

        case CHAT_MSG_PARTY:
            return 9;

        case CHAT_MSG_YELL:
            return 10;

        case CHAT_MSG_SAY:
            return 11;

        case CHAT_MSG_EMOTE:
            return 12;

        case CHAT_MSG_AFK:
            return 13;
    }

    return type != CHAT_MSG_DND ? 0xFFFFFFFF : 14;
}

// OFFSET: 0x008E5AB0/0x140613CD0
ChatRecord* GetChatRecordEx(const char* name, const char* text, uint32_t lineID) {
    ChatRecord* best = NULL;
    uint32_t bestPriority = 0xFFFFFFFF;

    for (uint32_t i = 0; i < 60; i++) {
        ChatRecord* record = &CGChat::m_chatHistory[static_cast<int32_t>(i + CGChat::m_currentChatIndex) % 60];

        if (!record->m_description[0]) {
            continue;
        }

        uint32_t priority = GetComplaintPriority(record->m_type, record->m_unk17B0);

        if (priority < bestPriority && (!name || !SStrCmpI(name, record->m_senderName, STORM_MAX_STR)) && (!text || !SStrCmpI(text, record->m_text, STORM_MAX_STR)) && (!lineID || lineID == record->m_lineID)) {
            best = record;
            bestPriority = priority;
        }
    }

    return best;
}

// OFFSET: 0x008E6180/0x1406143E0
static void LogFeedPetCallback(int32_t id, const WOWGUID& guid, void* param, bool success) {
    if (success) {
        CGChat::LogFeedPet(guid, id);
    }
}

// OFFSET: 0x008E61A0/0x140614400
static void LogTradeSkillCreateCallback(int32_t id, const WOWGUID& guid, void* param, bool success) {
    if (success) {
        CGChat::LogTradeSkillCreate(guid, id);
    }
}

// OFFSET: 0x008E61F0/0x140614450
int32_t CGChat::IsProfanityFilterEnabled() {
    return g_profanityFilterCVar->m_intValue;
}

// OFFSET: 0x008E62A0/0x140614520
int32_t CGChat::IsVoiceChatEnabled() {
    int32_t enabled = s_cvarEnableVoiceChat && s_cvarEnableVoiceChat->m_intValue;
    int32_t disabled = ComSatClient_IsComsatDisabledByClient();
    return CGChat::m_voiceChatSystemStatus && enabled && !disabled;
}

// OFFSET: 0x008E7350
static void RebuildDisplayListMember(PENDINGUSERLIST* user, int32_t type, const char* name) {
    if (!user) {
        return;
    }

    user->m_flags &= 0xF;

    if (!ComSatClient_IsTalkerSquelchedInChannel(user->m_guid, type, name)) {
        user->m_flags |= 0x10;
    }

    if (ComSatClient_IsMemberInVoiceSession(user->m_guid, type, name)) {
        user->m_flags |= 0x40;
    }

    if (ComSatClient_GetChannelMemberVoiceStatus(user->m_guid, type, name)) {
        user->m_flags |= 0x80;
    }

    if (ComSatClient_IsPlayerMuted(user->m_guid) || ComSatClient_IsTalkerSquelchedInChannel(user->m_guid, type, name)) {
        user->m_flags |= 0x20;
    }
}

// OFFSET: 0x008EB080/0x140619FC0
bool CGChat::CanComplain(WOWGUID guid) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return false;
    }

    if (guid == player->m_obj->m_guid) {
        return false;
    }

    if (FriendList::s_friendList && FriendList::s_friendList->IsFriend(guid)) {
        return false;
    }

    return true;
}

// OFFSET: 0x008EB2C0/0x14061A260
void CGChat::VoiceSessionUpdate(const char* name, int32_t type) {
    if (type != s_activeDisplayChannelType || (!type && (!name || SStrCmpUTF8I(name, CGChat::s_activeDisplayChannelName, STORM_MAX_STR)))) {
        return;
    }

    for (uint32_t i = 0; i < s_pendingUsers.Count(); i++) {
        RebuildDisplayListMember(&s_pendingUsers.m_data[i], type, name);
    }

    uint32_t index = CGChat::FindDisplayChannelIndex(CGChat::s_activeDisplayChannelName, s_activeDisplayChannelType);

    if (index) {
        FrameScript_SignalEvent(SCRIPT_EVENT_CHANNEL_ROSTER_UPDATE, "%d%d", index, s_pendingUsers.Count());
    }
}

// OFFSET: 0x008EB380/0x14061A3C0
void CGChat::NewVoiceSession(const char* name, int32_t type) {
    uint32_t index = CGChat::FindDisplayChannelIndex(name, type);

    if (!(index > 0)) {
        return;
    }

    index--;
    CGChat::s_displayChannels[index].m_voiceEnabled = 1;
    int32_t active = s_activeVoiceChannel == static_cast<int32_t>(index);
    index++;

    if (active) {
        FrameScript_SignalEvent(SCRIPT_EVENT_CHANNEL_VOICE_UPDATE, "%d%d%d", index, 1, active);
    } else {
        FrameScript_SignalEvent(SCRIPT_EVENT_CHANNEL_VOICE_UPDATE, "%d%d", index, 1);
    }
}

// OFFSET: 0x008EB530/0x14061A590
int32_t CGChat::OnVoiceStatusChanged(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    uint8_t status;
    msg->Get(status);

    int32_t enabled = status != 0;

    if (CGChat::m_voiceChatSystemStatus == enabled) {
        return 1;
    }

    if (enabled) {
        CGChat::m_voiceChatSystemStatus = 1;

        if (CGGameUI::m_initialized) {
            FrameScript_SignalEvent(SCRIPT_EVENT_VOICE_CHAT_ENABLED_UPDATE, NULL);
        }

        static CVar* s_enableMicrophone = CVar::Lookup("EnableMicrophone");
        static CVar* s_enableVoiceChat = CVar::Lookup("EnableVoiceChat");

        if (s_enableVoiceChat && s_enableVoiceChat->m_intValue && s_enableMicrophone && s_enableMicrophone->m_intValue && !ComSatClient_IsComsatDisabledByClient()) {
            SI3::VoiceChat_StartCapture();
        }

        return 1;
    }

    CGChat::m_voiceChatSystemStatus = 0;

    if (CGGameUI::m_initialized) {
        FrameScript_SignalEvent(SCRIPT_EVENT_VOICE_CHAT_ENABLED_UPDATE, NULL);
    }

    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return 0;
    }

    ComSatClient_LeaveAllSessions(player->m_obj->m_guid);
    SI3::VoiceChat_StopCapture();
    return 1;
}

// OFFSET: 0x008EBFF0/0x14061B140
void CGChat::SilencePartyMember(bool silence, const char* name) {
    if (!CGRaidInfo::m_numMembers && !CGPartyInfo::m_members[0]) {
        return;
    }

    WOWGUID guid = CGGameUI::ClosestObjectMatch(name, TYPEMASK_PLAYER, CGRaidInfo::m_numMembers ? 6 : 5, 0, 3.402823466e38f, 0);

    if (!guid) {
        CGGameUI::DisplayError(ERR_TARGET_NOT_IN_GROUP_S, name);
        return;
    }

    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return;
    }

    if (CGRaidInfo::m_numMembers) {
        WOWGUID self = player->m_obj->m_guid;

        if (!CGRaidInfo::IsPlayerRaidOfficer(self)) {
            return;
        }
    } else if (CGPartyInfo::m_leader != player->m_obj->m_guid) {
        return;
    }

    bool pvp = CGRaidInfo::IsPVPRaid() != 0;

    if (silence) {
        CGPlayer_C::SilencePlayer(guid, pvp);
    } else {
        CGPlayer_C::UnSilencePlayer(guid, pvp);
    }
}

// OFFSET: 0x008EC2F0/0x14061B550
void CGChat::UpdateRosterMember(WOWGUID guid) {
    for (uint32_t i = 0; i < s_pendingUsers.Count(); i++) {
        if (s_pendingUsers.m_data[i].m_guid != guid) {
            continue;
        }

        RebuildDisplayListMember(&s_pendingUsers.m_data[i], s_activeDisplayChannelType, CGChat::s_activeDisplayChannelName);
        uint32_t index = CGChat::FindDisplayChannelIndex(CGChat::s_activeDisplayChannelName, s_activeDisplayChannelType);

        if (index) {
            FrameScript_SignalEvent(SCRIPT_EVENT_CHANNEL_ROSTER_UPDATE, "%d%d", index, s_pendingUsers.Count());
        }

        return;
    }
}

// OFFSET: 0x008EEB40/0x14061E7D0
void CGChat::DisplayPendingUserList(ChatChannel* channel) {
    for (PENDINGUSERLIST* user = channel->m_pendingUsers.Head(); user; user = channel->m_pendingUsers.Next(user)) {
        if (!g_nameDBCache.GetRecord(user->m_guid.m_guid, WOWGUID(), NULL, NULL, false)) {
            return;
        }
    }

    s_pendingUserListActive = false;
    char text[256];
    text[0] = 0;
    int32_t count = 0;
    char channelName[128];
    SStrCopy(channelName, channel->m_name, sizeof(channelName));
    TSExplicitList<PENDINGUSERLIST, 0> users;
    users.Combine(&channel->m_pendingUsers, 2, NULL);

    for (PENDINGUSERLIST* user = users.Head(); user; user = users.Next(user)) {
        NameCache* name = g_nameDBCache.GetRecord(user->m_guid.m_guid, WOWGUID(), NULL, NULL, false);

        if (!name) {
            continue;
        }

        const char* prefix = "";

        if (user->m_flags & 0x1) {
            prefix = "*";
        } else if (user->m_flags & 0x2) {
            prefix = "@";
        } else if (!(user->m_flags & 0x4)) {
            prefix = "#";
        }

        const char* separator = text[0] ? ", " : "";
        char fullName[305];

        if (name->m_realmName[0]) {
            SStrPrintf(fullName, sizeof(fullName), "%s-%s", static_cast<const char*>(name->m_name), name->m_realmName);
        } else {
            SStrCopy(fullName, name->m_name, sizeof(fullName));
        }

        char entry[51];
        SStrPrintf(entry, sizeof(entry), "%s%s%s", separator, prefix, fullName);

        if (SStrLen(entry) + SStrLen(text) < sizeof(text)) {
            SStrPack(text, entry, STORM_MAX_STR);
            count++;
        } else {
            ChatMessageParams params;
            params.m_channelName = channelName;
            CGChat::AddChatMessage(text, CHAT_MSG_CHANNEL_LIST, &params);
            text[0] = 0;

            if (entry[0] == ',') {
                memmove(entry, &entry[2], sizeof(entry));
            }

            SStrPack(text, entry, STORM_MAX_STR);
            count = 1;
        }
    }

    if (count) {
        ChatMessageParams params2;
        params2.m_channelName = channelName;
        CGChat::AddChatMessage(text, CHAT_MSG_CHANNEL_LIST, &params2);
    }

    users.Clear();
}

// OFFSET: 0x008F0570/0x140620C20
int32_t CGChat::OnUserListAdded(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    WOWGUID guid;
    uint8_t flags;
    uint8_t channelFlags;
    uint32_t count;
    char name[128];
    msg->Get(guid.m_guid);
    msg->Get(flags);
    msg->Get(channelFlags);
    msg->Get(count);
    msg->GetString(name, sizeof(name));

    if (s_activeDisplayChannelType || SStrCmpUTF8I(name, CGChat::s_activeDisplayChannelName, STORM_MAX_STR)) {
        msg->m_read = msg->m_size;
        return 1;
    }

    PENDINGUSERLIST user;
    UpdateVoiceMemberFlags(&user, flags, guid, 0, name);
    s_pendingUsers.New(user);
    uint32_t index = CGChat::FindDisplayChannelIndex(name, 0);

    if (index) {
        ChatChannel* channel = &CGChat::s_channels.m_data[CGChat::s_displayChannels[index - 1].m_channelIndex];

        if (channel->m_flags != channelFlags) {
            channel->m_flags = channelFlags;
            FrameScript_SignalEvent(SCRIPT_EVENT_CHANNEL_FLAGS_UPDATED, "%d", index);
        }
    }

    if (g_nameDBCache.GetRecord(guid.m_guid, guid, CGChat::NameDisplayQueryCallback, NULL, false)) {
        qsort(s_pendingUsers.m_data, s_pendingUsers.Count(), sizeof(PENDINGUSERLIST), QSortDisplayListMembers);

        if (index) {
            CGChat::s_displayChannels[index - 1].m_count = s_pendingUsers.Count();
            FrameScript_SignalEvent(SCRIPT_EVENT_CHANNEL_COUNT_UPDATE, "%d%d", index, s_pendingUsers.Count());
            FrameScript_SignalEvent(SCRIPT_EVENT_CHANNEL_ROSTER_UPDATE, "%d%d", index, s_pendingUsers.Count());
        }
    } else {
        s_displayChannelMemberCount++;
    }

    return 1;
}

// OFFSET: 0x008F0740/0x140620EA0
int32_t CGChat::OnUserListRemove(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    WOWGUID guid;
    uint8_t channelFlags;
    uint32_t count;
    char name[128];
    msg->Get(guid.m_guid);
    msg->Get(channelFlags);
    msg->Get(count);
    msg->GetString(name, sizeof(name));

    if (s_activeDisplayChannelType || SStrCmpUTF8I(name, CGChat::s_activeDisplayChannelName, STORM_MAX_STR)) {
        msg->m_read = msg->m_size;
        return 1;
    }

    uint32_t total = s_pendingUsers.Count();
    uint32_t index = CGChat::FindDisplayChannelIndex(name, 0);

    if (index) {
        ChatChannel* channel = &CGChat::s_channels.m_data[CGChat::s_displayChannels[index - 1].m_channelIndex];

        if (channel->m_flags != channelFlags) {
            channel->m_flags = channelFlags;
            FrameScript_SignalEvent(SCRIPT_EVENT_CHANNEL_FLAGS_UPDATED, "%d", index);
        }
    }

    for (uint32_t i = 0; i < s_pendingUsers.Count(); i++) {
        if (s_pendingUsers.m_data[i].m_guid != guid) {
            continue;
        }

        if (i < total - 1) {
            memmove(&s_pendingUsers.m_data[i], &s_pendingUsers.m_data[i + 1], sizeof(PENDINGUSERLIST) * (total - i - 1));
        }

        s_pendingUsers.SetCount(s_pendingUsers.Count() - 1);

        if (index) {
            CGChat::s_displayChannels[index - 1].m_count = s_pendingUsers.Count();
            FrameScript_SignalEvent(SCRIPT_EVENT_CHANNEL_COUNT_UPDATE, "%d%d", index, s_pendingUsers.Count());
            FrameScript_SignalEvent(SCRIPT_EVENT_CHANNEL_ROSTER_UPDATE, "%d%d", index, s_pendingUsers.Count());
        }

        return 1;
    }

    return 1;
}

// OFFSET: 0x008F7F60/0x14062AFA0
void CGChat::LogHonorGain(CGUnit_C* unit, const WOWGUID& victim, int32_t rank, int32_t honor) {
    char text[3000];
    float amount = honor * 0.01f;

    if (!victim) {
        const char* format = FrameScript_GetText("COMBATLOG_HONORAWARD", -1, GENDER_NOT_APPLICABLE);

        if (format && *format) {
            SStrPrintf(text, sizeof(text), format, amount);
            CGChat::AddChatMessage(text, CHAT_MSG_COMBAT_HONOR_GAIN, NULL);
            SendToCombatText(43, amount);
        }

        return;
    }

    CGUnit_C* target = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(victim, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (!target || amount <= 0.0f) {
        return;
    }

    const char* format = rank < 0 ? FrameScript_GetText("COMBATLOG_HONORGAIN_NO_RANK", -1, GENDER_NOT_APPLICABLE) : FrameScript_GetText("COMBATLOG_HONORGAIN", -1, GENDER_NOT_APPLICABLE);

    if (!format || !*format) {
        return;
    }

    char token[64];
    FactionTemplateRec* faction = g_factionTemplateDB.GetRecord(target->m_unit->m_unitFieldFactiontemplate);

    if (faction) {
        int32_t side = (faction->m_factionGroup & 0x4) ? 0 : ((faction->m_factionGroup & 0x2) ? 1 : -1);

        if (side >= 0) {
            SStrPrintf(token, sizeof(token), "PVP_RANK_%d_%d", rank, side);
        }
    }

    const char* rankName;

    if (rank < 0) {
        rankName = NULL;
        SStrPrintf(text, sizeof(text), format, target->GetUnitName(NULL, 1), amount);
    } else {
        rankName = unit->GetFrameScriptText(token, -1);
        SStrPrintf(text, sizeof(text), format, target->GetUnitName(NULL, 1), rankName, amount);
    }

    CGChat::AddChatMessage(text, CHAT_MSG_COMBAT_HONOR_GAIN, NULL);
    SendToCombatText(43, amount);

    if (rank >= 0) {
        unit->AddWorldHonorGainText(amount, rank, rankName);
    }
}

// OFFSET: 0x008F8500/0x14062B6F0
void CGChat::LogFeedPet(const WOWGUID& guid, int32_t itemId) {
    ItemRecSparse* stats = g_itemSparseDB.GetRecord(itemId, guid, LogFeedPetCallback, NULL, false);

    if (!stats) {
        return;
    }

    char text[3000];

    if (guid == ClntObjMgrGetActivePlayer()) {
        SStrPrintf(text, sizeof(text), FrameScript_GetText("FEEDPET_LOG_FIRSTPERSON", -1, GENDER_NOT_APPLICABLE), stats->GetName(0));
    } else {
        NameCache* name = g_nameDBCache.GetRecord(guid.m_guid, WOWGUID(), NULL, NULL, false);

        if (!name) {
            return;
        }

        SStrPrintf(text, sizeof(text), FrameScript_GetText("FEEDPET_LOG_THIRDPERSON", -1, GENDER_NOT_APPLICABLE), static_cast<const char*>(name->m_name), stats->GetName(0));
    }

    CGChat::AddChatMessage(text, CHAT_MSG_PET_INFO, NULL);
}

// OFFSET: 0x008F86A0/0x14062B8D0
void CGChat::LogTradeSkillCreate(const WOWGUID& guid, int32_t itemId) {
    ItemRecSparse* stats = g_itemSparseDB.GetRecord(itemId, guid, LogTradeSkillCreateCallback, NULL, false);

    if (!stats) {
        return;
    }

    char text[3000];

    if (guid == ClntObjMgrGetActivePlayer()) {
        SStrPrintf(text, sizeof(text), FrameScript_GetText("TRADESKILL_LOG_FIRSTPERSON", -1, GENDER_NOT_APPLICABLE), stats->GetName(0));
    } else {
        SStrPrintf(text, sizeof(text), FrameScript_GetText("TRADESKILL_LOG_THIRDPERSON", -1, GENDER_NOT_APPLICABLE), GetCombatLogObjectName(guid), stats->GetName(0));
    }

    CGChat::AddChatMessage(text, CHAT_MSG_TRADESKILLS, NULL);
}

// OFFSET: 0x008F8F80/0x14062C3E0
int32_t CGChat::OnTitleEarned(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    uint32_t titleId;
    uint32_t earned;
    msg->Get(titleId);
    msg->Get(earned);
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));
    CharTitlesRec* title = g_charTitlesDB.GetRecord(CGUnit_C::GetRecordForTitle(titleId));

    if (!player || !title) {
        return 1;
    }

    const char* guildName = NULL;

    if ((title->unk_0014 & 0x1) && !(guildName = player->GetPlayerGuildNameText())) {
        return 1;
    }

    char name[256];

    if (player->m_player->m_playerBytes3[0] == 1) {
        if (strcmp(title->m_name1, "")) {
            SStrPrintf(name, sizeof(name), static_cast<const char*>(title->m_name1), guildName ? guildName : player->GetUnitName(NULL, 1));
        } else {
            SStrPrintf(name, sizeof(name), static_cast<const char*>(title->m_name), guildName ? guildName : player->GetUnitName(NULL, 1));
        }
    } else if (strcmp(title->m_name, "")) {
        SStrPrintf(name, sizeof(name), static_cast<const char*>(title->m_name), guildName ? guildName : player->GetUnitName(NULL, 1));
    } else {
        SStrPrintf(name, sizeof(name), static_cast<const char*>(title->m_name1), guildName ? guildName : player->GetUnitName(NULL, 1));
    }

    char text[3000];

    if (earned) {
        CEffect* effect = CEffect::Create();
        WOWGUID guid = player->m_obj->m_guid;
        effect->AddSpecialEffect(9, guid, CGObject_C::EffectOnceHandler);
        effect->m_flags |= 0x20;
        effect->Release();
        SStrPrintf(text, sizeof(text), FrameScript_GetText("NEW_TITLE_EARNED", -1, GENDER_NOT_APPLICABLE), name);
    } else {
        SStrPrintf(text, sizeof(text), FrameScript_GetText("OLD_TITLE_LOST", -1, GENDER_NOT_APPLICABLE), name);
    }

    CGChat::AddChatMessage(text, CHAT_MSG_SYSTEM, NULL);
    return 1;
}

// OFFSET: 0x008F99E0/0x14062CFE0
void CGChat::DisplayParentControlsNotice(UIErrors error) {
    char text[3000];

    if (error == ERR_VOICE_CHAT_PARENTAL_DISABLE_ALL) {
        SStrPrintf(text, sizeof(text), FrameScript_GetText("ERR_VOICE_CHAT_PARENTAL_DISABLE_ALL", -1, GENDER_NOT_APPLICABLE));
    } else if (error == ERR_VOICE_CHAT_PARENTAL_DISABLE_MIC) {
        SStrPrintf(text, sizeof(text), FrameScript_GetText("ERR_VOICE_CHAT_PARENTAL_DISABLE_MIC", -1, GENDER_NOT_APPLICABLE));
    } else {
        return;
    }

    CGGameUI::DisplayError(error);
    CGChat::AddChatMessage(text, CHAT_MSG_SYSTEM, NULL);
}

// OFFSET: 0x008FAAC0/0x14062E530
void CGChat::LogOpenLock(const WOWGUID& guid, const SpellRec* spell, ItemRecSparse* item) {
    if (!item) {
        return;
    }

    char text[3000];

    if (guid == ClntObjMgrGetActivePlayer()) {
        SStrPrintf(text, sizeof(text), FrameScript_GetText("OPEN_LOCK_SELF", -1, GENDER_NOT_APPLICABLE), static_cast<const char*>(spell->m_name), item->GetName(0));
    } else {
        SStrPrintf(text, sizeof(text), FrameScript_GetText("OPEN_LOCK_OTHER", -1, GENDER_NOT_APPLICABLE), GetCombatLogObjectName(guid), static_cast<const char*>(spell->m_name), item->GetName(0));
    }

    CGChat::AddChatMessage(text, CHAT_MSG_OPENING, NULL);
}

// OFFSET: 0x008FAB80/0x14062E650
void CGChat::LogOpenLock(const WOWGUID& guid, const SpellRec* spell, const GameObjectStats_C* stats) {
    if (!stats) {
        return;
    }

    char text[3000];

    if (guid == ClntObjMgrGetActivePlayer()) {
        SStrPrintf(text, sizeof(text), FrameScript_GetText("OPEN_LOCK_SELF", -1, GENDER_NOT_APPLICABLE), static_cast<const char*>(spell->m_name), stats->m_name[0]);
    } else {
        SStrPrintf(text, sizeof(text), FrameScript_GetText("OPEN_LOCK_OTHER", -1, GENDER_NOT_APPLICABLE), GetCombatLogObjectName(guid), static_cast<const char*>(spell->m_name), stats->m_name[0]);
    }

    CGChat::AddChatMessage(text, CHAT_MSG_OPENING, NULL);
}

// OFFSET: 0x008EB0E0/0x14061A020
void ComplainChat(const char* name, const char* text, uint32_t lineID) {
    ChatRecord* record = GetChatRecordEx(name, text, lineID);

    if (!record || !CGChat::CanComplain(record->m_senderGuid) || !CGChat::m_complaintsSystemStatus || !FriendList::s_friendList || FriendList::s_friendList->IsComplained(record->m_senderGuid)) {
        return;
    }

    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_COMPLAINT));
    msg.Put(static_cast<uint8_t>(1));
    msg.Put(record->m_senderGuid.m_guid);
    msg.Put(record->m_senderUnk);
    msg.Put(static_cast<uint32_t>(record->m_type));
    msg.Put(record->m_unk17B0);
    msg.Put(static_cast<uint32_t>(::time(NULL) - record->m_time));
    msg.PutString(record->m_description);
    msg.Finalize();
    ClientServices::Send2(&msg);
}

// OFFSET: 0x008FBC70/0x14062FD70
void CGChat::LogOpenLock(const WOWGUID& caster, const WOWGUID& target, const SpellRec* spell) {
    if (!spell || (spell->m_attributes & 0x180) || !*spell->m_name) {
        return;
    }

    CGObject_C* object = ClntObjMgrObjectPtr(target, TYPEMASK_OBJECT, __FILE__, __LINE__);

    if (!object) {
        return;
    }

    if (object->m_obj->m_type & TYPEMASK_ITEM) {
        ItemRecSparse* item = g_itemSparseDB.GetRecord(object->m_obj->m_entryID, WOWGUID(), NULL, NULL, false);
        CGChat::LogOpenLock(caster, spell, item);
    } else if (object->m_obj->m_type & TYPEMASK_GAMEOBJECT) {
        GameObjectStats_C* stats = g_gameObjectDBCache.GetRecord(object->m_obj->m_entryID, WOWGUID(), NULL, NULL, false);
        CGChat::LogOpenLock(caster, spell, stats);
    }
}

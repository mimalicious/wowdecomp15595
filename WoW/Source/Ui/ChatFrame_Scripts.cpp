#include "ChatFrame.h"
#include <cstdint>
#include "Engine/Source/lua-5.1/src/lua.h"
#include "Engine/Source/Base/CDataStore.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"
#include "Engine/Source/Os/TimeManager.h"
#include "Engine/Source/Event/FrameTime.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"
#include "WoW/Source/Game/ValidateName.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/Object/Types.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/Ui/QuestTextParser.h"
#include "WoW/Source/Ui/GameUI.h"
#include "WoW/Source/Ui/PartyFrame.h"
#include "WoW/Source/Ui/RaidInfo.h"
#include "WoW/Source/Ui/Tutorial.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"
#include "WoW/Source/Net/Types.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/LanguagesRec.h"
#include "WoW/Source/WowServices/WDataStore.h"
#include "Engine/Source/FrameXML/LoadXML.h"
#include "WoW/Source/DB/Rec/ChatChannelsRec.h"
#include "WoW/Source/Game/GameClient/ComSatClient.h"
#include "Engine/Source/ConsoleBase/ConsoleVar.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/FriendList.h"
#include "WoW/Source/DB/DBClient/DBCacheInstances.h"
#include "WoW/Source/Game/GameClient/NameCache.h"
#include "WoW/Source/DB/Rec/MapRec.h"
#include "WoW/Source/DB/Rec/MapDifficultyRec.h"
#include "WoW/Source/DB/Rec/AreaTableRec.h"
#include "WoW/Source/DB/Rec/EmotesTextRec.h"
#include "WoW/Source/Ui/DanceStudio.h"
#include "Engine/Source/Os/OsFile.h"
#include "WoW/Source/Ui/ScriptEvents.h"
#include "WoW/Source/Ui/Types.h"
#include "WoW/Common/ThrottleObject.h"
#include "WoW/Source/Ui/GMTicketInfo.h"
#include "WoW/Source/Object/ObjectClient/UnitCombatLog_C.h"
#include "WoW/Source/JamClientAutoCode/JamUserClient.h"

// STATIC: 0x00D101E4
static const char* s_channelCategories[3] = { "CHANNEL_CATEGORY_GROUP", "CHANNEL_CATEGORY_WORLD", "CHANNEL_CATEGORY_CUSTOM" };

// STATIC: 0x00D101DC
static const char* s_chatLogNames[2] = { "Logs\\WoWChatLog.txt", "Logs\\WoWCombatLog.txt" };

// OFFSET: 0x008E6450/0x140614780
static int32_t Script_IsChatAFK(lua_State* L) {
    lua_pushboolean(L, CGChat::m_afk);
    return 1;
}

// OFFSET: 0x008E6470/0x1406147A0
static int32_t Script_IsChatDND(lua_State* L) {
    lua_pushboolean(L, CGChat::m_dnd);
    return 1;
}

// OFFSET: 0x008E71E0/0x1406155D0
static int32_t Script_SetPendingReportTarget(lua_State* L) {
    WOWGUID guid;

    if (lua_isstring(L, 1) && Script_GetGUIDFromToken(lua_tolstring(L, 1, NULL), guid, false, false)) {
        s_pendingReportTarget = guid;
        lua_pushboolean(L, 1);
        return 1;
    }

    s_pendingReportTarget.m_guid = 0;
    lua_pushboolean(L, 0);
    return 1;
}

// OFFSET: 0x008EBDC0/0x14061AF20
static int32_t Script_CancelEmote(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player) {
        player->ClearEmote();
    }

    return 0;
}

// OFFSET: 0x008EDB70/0x14061D390
int32_t FillAddonMessagePrefixes(lua_State* L) {
    int32_t count = 1;

    for (ADDONPREFIX* prefix = s_addonPrefixes.Head(); prefix; prefix = s_addonPrefixes.Next(prefix)) {
        lua_checkstack(L, 2);
        lua_pushnumber(L, count);
        lua_pushstring(L, prefix->m_key.m_str);
        lua_settable(L, -3);
        count++;
    }

    return count;
}

// OFFSET: 0x008EE0D0/0x14061DA40
static int32_t Script_IsAddonMessagePrefixRegistered(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: IsAddonMessagePrefixRegistered (prefix)");
        return 0;
    }

    bool registered = s_addonPrefixes.Ptr(lua_tolstring(L, 1, NULL)) != NULL;
    lua_pushboolean(L, registered);
    return 1;
}

// OFFSET: 0x008EE130/0x14061DAB0
static int32_t Script_GetRegisteredAddonMessagePrefixes(lua_State* L) {
    if (lua_type(L, 1) != LUA_TTABLE) {
        lua_createtable(L, 0, 0);
    }

    FillAddonMessagePrefixes(L);
    return 1;
}

// OFFSET: 0x008F6220/0x1406284C0
static int32_t Script_RegisterAddonMessagePrefix(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: RegisterAddonMessagePrefix (prefix)");
        return 0;
    }

    lua_pushboolean(L, RegisterAddonMessagePrefix(lua_tolstring(L, 1, NULL)));
    return 1;
}

// OFFSET: 0x008F9C10/0x14062D3D0
static int32_t Script_SendChatMessage(lua_State* L) {
    if (!CGGameUI::CanPerformAction(10)) {
        return 0;
    }

    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return 0;
    }

    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: SendChatMessage(text [,type] [,language] [,targetPlayer])");
        return 0;
    }

    const char* text = lua_tolstring(L, 1, NULL);
    int32_t type = CHAT_MSG_SAY;

    if (lua_isstring(L, 2)) {
        if (!StringToChatType(lua_tolstring(L, 2, NULL), &type)) {
            luaL_error(L, "SendChatMessage(): Unknown chat type");
            return 0;
        }
    }

    if ((!text || !*text) && type != CHAT_MSG_DND && type != CHAT_MSG_AFK) {
        return 0;
    }

    if (!ValidateStringIsUTF8(text)) {
        luaL_error(L, "SendChatMessage(): Chat message must be UTF-8 text");
        return 0;
    }

    const char* target = NULL;
    uint32_t language = player->GetDefaultLanguage();

    if (lua_isstring(L, 3)) {
        if (!StringToLanguage(lua_tolstring(L, 3, NULL), &language)) {
            luaL_error(L, "SendChatMessage(): Unknown language");
            return 0;
        }

        uint32_t skill;

        if (!player->GetLanguageSkill(language, skill)) {
            CGGameUI::DisplayError(ERR_CANT_SPEAK_LANGAGE);
            return 0;
        }
    }

    if (lua_isstring(L, 4)) {
        target = lua_tolstring(L, 4, NULL);
    }

    if (type == CHAT_MSG_WHISPER) {
        if (!target || !*target) {
            luaL_error(L, "SendChatMessage(): Whisper message missing target player!");
            return 0;
        }
    } else if (type == CHAT_MSG_PARTY) {
        if (!CGPartyInfo::InParty() && (!CGRaidInfo::m_numMembers || !CGRaidInfo::IsPVPRaid())) {
            CGGameUI::DisplayError(ERR_NOT_IN_GROUP);
            return 0;
        }

        if (CGPartyInfo::m_leader.m_guid == player->m_obj->m_guid.m_guid) {
            type = CHAT_MSG_PARTY_LEADER;
        }
    } else if (type == CHAT_MSG_RAID_WARNING) {
        if (!CGRaidInfo::m_numMembers) {
            if (!CGPartyInfo::InParty()) {
                CGGameUI::DisplayError(ERR_NOT_IN_GROUP);
                return 0;
            }
        } else {
            WOWGUID guid = player->m_obj->m_guid;

            if (!CGRaidInfo::IsPlayerRaidOfficer(guid)) {
                CGGameUI::DisplayError(ERR_NOT_LEADER);
                return 0;
            }
        }
    } else if (type == CHAT_MSG_RAID || type == CHAT_MSG_BATTLEGROUND) {
        if (!CGRaidInfo::m_numMembers) {
            CGGameUI::DisplayError(type != CHAT_MSG_BATTLEGROUND ? ERR_NOT_IN_RAID : ERR_NOT_IN_BATTLEGROUND);
            return 0;
        }

        if (type == CHAT_MSG_RAID) {
            if (CGPartyInfo::m_leader.m_guid == player->m_obj->m_guid.m_guid) {
                type = CHAT_MSG_RAID_LEADER;
            }
        } else if (CGPartyInfo::m_leader.m_guid == player->m_obj->m_guid.m_guid) {
            type = CHAT_MSG_BATTLEGROUND_LEADER;
        }
    } else if (type == CHAT_MSG_CHANNEL) {
        if (!target || !*target) {
            luaL_error(L, "SendChatMessage(): Channel send missing channel number");
            return 0;
        }

        target = CGChat::GetChannelName(SStrToInt(target), false);

        if (!target) {
            return 0;
        }

        if (!(static_cast<uint8_t>(player->m_player->m_playerFlags >> 3) & 1) && !(static_cast<uint8_t>(player->m_player->m_playerFlags >> 19) & 1) && !CGChat::CanSendChannelMessage(target)) {
            ChatMessageParams params;
            params.m_channelName = target;
            CGChat::AddChatMessage("THROTTLED", CHAT_MSG_CHANNEL_NOTICE, &params);
            return 0;
        }
    } else if (type == CHAT_MSG_AFK) {
        if (!CGChat::m_afk && !*text) {
            text = FrameScript_GetText("DEFAULT_AFK_MESSAGE", -1, GENDER_NOT_APPLICABLE);
        }

        if (*text) {
            player->SetAFK(text);
        } else {
            CGPlayer_C::ClearAFK(1);
        }

        return 0;
    }

    if (CGChat::m_afk) {
        CGPlayer_C::ClearAFK(0);
    }

    if (type == CHAT_MSG_DND) {
        if (!(player->m_player->m_playerFlags & 0x4) && !*text) {
            text = FrameScript_GetText("DEFAULT_DND_MESSAGE", -1, GENDER_NOT_APPLICABLE);
        }

        if (*text) {
            char buffer[3000];
            SStrPrintf(buffer, sizeof(buffer), FrameScript_GetText("MARKED_DND", -1, GENDER_NOT_APPLICABLE), text);
            CGChat::AddChatMessage(buffer, CHAT_MSG_SYSTEM, NULL);
            CGChat::m_dnd = 1;
        } else {
            CGChat::AddChatMessage(FrameScript_GetText("CLEARED_DND", -1, GENDER_NOT_APPLICABLE), CHAT_MSG_SYSTEM, NULL);
            CGChat::m_dnd = 0;
        }
    }

    if (player->IsDeadOrGhost() && type != CHAT_MSG_WHISPER && type != CHAT_MSG_PARTY && type != CHAT_MSG_PARTY_LEADER && type != CHAT_MSG_RAID && type != CHAT_MSG_RAID_LEADER && type != CHAT_MSG_RAID_WARNING && type != CHAT_MSG_GUILD && type != CHAT_MSG_OFFICER && type != CHAT_MSG_CHANNEL && type != CHAT_MSG_DND && type != CHAT_MSG_BATTLEGROUND && type != CHAT_MSG_BATTLEGROUND_LEADER) {
        CGGameUI::DisplayError(ERR_CHAT_WHILE_DEAD);
        return 0;
    }

    CGTutorial::FlagTutorial(22);

    char parsed[256];
    ChatParserParseText(text, parsed, sizeof(parsed));
    char* message = parsed;
    char drunk[256];

    if (type != CHAT_MSG_EMOTE && type != CHAT_MSG_DND && player->IsDrunk()) {
        InebriatedParseText(parsed, drunk, sizeof(drunk), player->GetDrunknessTurnValue());
        message = drunk;
    }

    TruncateNewLines(message);

    for (char* escape = SStrChr(message, '|'); escape;) {
        char* next;

        if (escape[1] == '|') {
            next = escape + 2;
        } else {
            if (escape[1] != 'c') {
                luaL_error(L, "SendChatMessage(): Invalid escape code in chat message");
                return 0;
            }

            char* end = SStrStr(escape, "|r");

            if (!end) {
                *escape = 0;
                break;
            }

            next = end + 2;
        }

        escape = SStrChr(next, '|');
    }

    if ((type == CHAT_MSG_WHISPER || type == CHAT_MSG_CHANNEL || type == CHAT_MSG_BATTLENET) && language != 0xFFFFFFFF && !(static_cast<uint8_t>(player->m_player->m_playerFlags >> 3) & 1) && !(static_cast<uint8_t>(player->m_player->m_playerFlags >> 19) & 1)) {
        CGChat::ProcessThrottledChat();

        if (type == CHAT_MSG_WHISPER || type == CHAT_MSG_BATTLENET) {
            if (!CGChat::IsInWhisperSourceHistory(target)) {
                uint32_t time = FrameTime::GetCurTimeMs();

                if (CGChat::IsWhisperThrottled(time)) {
                    CGChat::AddThrottledChat(type, language, target, message);
                    return 0;
                }

                CGChat::AddWhisperThrottled(time);
            }
        } else {
            uint32_t time = FrameTime::GetCurTimeMs();

            if (CGChat::IsChannelThrottled(target, time)) {
                CGChat::AddThrottledChat(type, language, target, message);
                return 0;
            }

            CGChat::AddChannelThrottled(target, time);
        }
    }

    CGChat::SendChat(language, message, type, target, NULL);
    return 0;
}

// OFFSET: 0x008E74F0/0x140615810
static int32_t Script_IsVoiceChatAllowedByServer(lua_State* L) {
    if (CGChat::m_voiceChatSystemStatus) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: 0x008E73F0/0x140615710
static int32_t Script_GetNumDisplayChannels(lua_State* L) {
    lua_pushnumber(L, s_numDisplayChannels);
    return 1;
}

// PROVEN: e591e05f IDENTICAL/NONE
// OFFSET: 0x008E6570/0x1406148C0
static int32_t Script_GetActiveVoiceChannel(lua_State* L) {
    if (s_activeVoiceChannel) {
        lua_pushnumber(L, s_activeVoiceChannel + 1);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: 0x008FA340/0x14062DC10
static int32_t Script_SendSystemMessage(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: SendSystemMessage(\"message\")");
        return 0;
    }

    CGChat::AddChatMessage(lua_tolstring(L, 1, NULL), CHAT_MSG_SYSTEM, NULL);
    return 0;
}

// OFFSET: 0x008EAF30/0x140619E70
static int32_t Script_ClearChannelWatch(lua_State* L) {
    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_CLEAR_CHANNEL_WATCH));
    msg.Finalize();
    ClientServices::Send2(&msg);
    return 0;
}

// OFFSET: 0x008EBBB0/0x14061ACC0
static int32_t Script_GetDefaultLanguage(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player) {
        const LanguagesRec* rec = g_languagesDB.GetRecord(player->GetDefaultLanguage());

        if (!rec) {
            return 0;
        }

        lua_pushstring(L, rec->m_name);
        return 1;
    }

    return 0;
}

// OFFSET: 0x008E9080/0x140617860
static void ChannelDisplayCommand(lua_State* L, uint32_t msgId, const char* name) {
    if (!lua_isnumber(L, 1)) {
        char usage[512];
        SStrPrintf(usage, sizeof(usage), "Usage: %s(\"displayIndex\")", name);
        luaL_error(L, usage);
        return;
    }

    int32_t type;
    const char* channel = CGChat::GetChannelStringForDisplay(static_cast<uint32_t>(lua_tonumber(L, 1)) - 1, &type);

    if (!channel || type) {
        return;
    }

    switch (msgId) {
        case CMSG_CHAT_CHANNEL_VOICE_ON: {
            UserClientChatChannelVoiceOn msg(channel);
            ClientServices::Send(&msg);
            break;
        }

        case CMSG_CHAT_CHANNEL_DISPLAY_LIST: {
            UserClientChatChannelDisplayList msg(channel);
            ClientServices::Send(&msg);
            break;
        }

        case CMSG_CHAT_CHANNEL_VOICE_OFF: {
            UserClientChatChannelVoiceOff msg(channel);
            ClientServices::Send(&msg);
            break;
        }
    }
}

// OFFSET: 0x008F1290/0x140621CE0
static int32_t Script_SetSelectedDisplayChannel(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        return 0;
    }

    int32_t type;
    const char* channel = CGChat::GetChannelStringForDisplay(static_cast<int32_t>(lua_tonumber(L, 1)) - 1, &type);

    if (!channel || type == 4) {
        return 0;
    }

    if (type == s_activeDisplayChannelType && (type || !SStrCmpUTF8I(channel, CGChat::s_activeDisplayChannelName, STORM_MAX_STR))) {
        return 0;
    }

    SetCurrentActiveDisplayChannel(channel, type);

    if (!type) {
        ChannelDisplayCommand(L, CMSG_CHAT_CHANNEL_DISPLAY_LIST, "GetChannelRosterInfo");
    } else if (type == 2) {
        BuildPartyDisplayList();
    } else if (type == 3 || type == 1) {
        BuildRaidDisplayList(type);
    }

    return 0;
}

// OFFSET: 0x008F1710/0x140622250
static int32_t Script_CollapseChannelHeader(lua_State* L) {
    if (lua_isnumber(L, 1)) {
        uint32_t index = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1;

        if (index < s_numDisplayChannels) {
            uint32_t header = CGChat::s_displayChannels[index].m_headerIndex;

            if (header <= 2) {
                s_collapsedHeaders[header] = 1;
                RebuildDisplayChannelList(1);
            }
        }
    }

    return 0;
}

// OFFSET: 0x008F1790/0x1406222C0
static int32_t Script_ExpandChannelHeader(lua_State* L) {
    if (lua_isnumber(L, 1)) {
        uint32_t index = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1;

        if (index < s_numDisplayChannels) {
            uint32_t header = CGChat::s_displayChannels[index].m_headerIndex;

            if (header <= 2) {
                s_collapsedHeaders[header] = 0;
                RebuildDisplayChannelList(1);
            }
        }
    }

    return 0;
}

// OFFSET: 0x008E6DF0/0x140615180
static int32_t Script_SetChatWindowLocked(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: SetChatWindowLocked(index, locked)");
        return 0;
    }

    uint32_t index = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1;

    if (index < 10) {
        s_chatWindows[index].m_unkD8 = StringToBOOL(L, 2, 0);
    }

    return 0;
}

// OFFSET: 0x008E6E70/0x140615200
static int32_t Script_SetChatWindowUninteractable(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: SetChatWindowUninteractable(index, uninteractable)");
        return 0;
    }

    uint32_t index = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1;

    if (index < 10) {
        s_chatWindows[index].m_unkE4 = StringToBOOL(L, 2, 0);
    }

    return 0;
}

// OFFSET: 0x008E6FA0/0x140615320
static int32_t Script_SetChatWindowShown(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: SetChatWindowShown(index, shown)");
        return 0;
    }

    uint32_t index = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1;

    if (index < 10) {
        s_chatWindows[index].m_shown = StringToBOOL(L, 2, 1);
    }

    return 0;
}

// OFFSET: 0x008E6850/0x140614BB0
static int32_t Script_GetChatWindowSavedDimensions(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetChatWindowSavedDimensions(index)");
        return 0;
    }

    uint32_t index = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1;

    if (index >= 10) {
        return 0;
    }

    lua_pushnumber(L, s_chatWindows[index].m_width);
    lua_pushnumber(L, s_chatWindows[index].m_height);
    return 2;
}

// OFFSET: 0x008EA660/0x140619420
static int32_t Script_SetChatWindowName(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: SetChatWindowName(index, \"name\")");
        return 0;
    }

    uint32_t index = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1;

    if (index < 10) {
        const char* name = "";

        if (lua_isstring(L, 2)) {
            name = lua_tolstring(L, 2, NULL);
        }

        SStrCopy(s_chatWindows[index].m_name, name, sizeof(s_chatWindows[index].m_name));
    }

    return 0;
}

// OFFSET: 0x008E6B60/0x140614F20
static int32_t Script_SetChatWindowSize(lua_State* L) {
    if (!lua_isnumber(L, 1) || !lua_isnumber(L, 2)) {
        luaL_error(L, "Usage: SetChatWindowSize(index, size)");
        return 0;
    }

    uint32_t index = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1;

    if (index < 10) {
        int32_t size = static_cast<int32_t>(lua_tonumber(L, 2));

        if (size > 0) {
            s_chatWindows[index].m_unkD0 = size;
        }
    }

    return 0;
}

// OFFSET: 0x008E6D40/0x1406150E0
static int32_t Script_SetChatWindowAlpha(lua_State* L) {
    if (!lua_isnumber(L, 1) || !lua_isnumber(L, 2)) {
        luaL_error(L, "Usage: SetChatWindowAlpha(index, alpha)");
        return 0;
    }

    uint32_t index = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1;

    if (index < 10) {
        s_chatWindows[index].m_color.a = static_cast<uint8_t>(lua_tonumber(L, 2) * 255.0);
    }

    return 0;
}

// OFFSET: 0x008E6EF0/0x140615280
static int32_t Script_SetChatWindowDocked(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: SetChatWindowDocked(index, docked)");
        return 0;
    }

    uint32_t index = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1;

    if (index < 10) {
        uint32_t docked = 0;

        if (lua_isnumber(L, 2)) {
            docked = static_cast<uint32_t>(lua_tonumber(L, 2));
        }

        s_chatWindows[index].m_docked = docked;
    }

    return 0;
}

// OFFSET: 0x008E7130/0x1406154F0
static int32_t Script_SetChatWindowSavedDimensions(lua_State* L) {
    if (!lua_isnumber(L, 1) || !lua_isnumber(L, 2) || !lua_isnumber(L, 3)) {
        luaL_error(L, "Usage: SetChatWindowSavedDimensions(index, width, height)");
        return 0;
    }

    uint32_t index = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1;

    if (index >= 10) {
        return 0;
    }

    ChatWindowInfo* window = &s_chatWindows[index];
    float width = static_cast<float>(lua_tonumber(L, 2));
    float height = static_cast<float>(lua_tonumber(L, 3));
    window->m_width = width;
    window->m_height = height;
    return 0;
}

// OFFSET: 0x008E6790/0x140614AD0
static int32_t Script_GetChatWindowSavedPosition(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetChatWindowSavedPosition(index)");
        return 0;
    }

    uint32_t index = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1;

    if (index >= 10 || !s_chatWindows[index].m_unkF4) {
        return 0;
    }

    lua_pushstring(L, FramePointToString(static_cast<FRAMEPOINT>(s_chatWindows[index].m_unkE8)));
    lua_pushnumber(L, s_chatWindows[index].m_unkEC);
    lua_pushnumber(L, s_chatWindows[index].m_unkF0);
    return 3;
}

// OFFSET: 0x008EBA10/0x14061AAB0
static int32_t Script_GetNumLanguages(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return 0;
    }

    uint32_t count = 0;
    int32_t numRecords = g_languagesDB.m_numRecords;

    for (int32_t i = 0; i < numRecords; i++) {
        const LanguagesRec* rec = g_languagesDB.GetRecordByIndex(i);
        uint32_t skill;

        if (rec && player->GetLanguageSkill(rec->m_id, skill)) {
            count++;
        }
    }

    lua_pushnumber(L, count);
    return 1;
}

// OFFSET: 0x008EBAB0/0x14061AB80
static int32_t Script_GetLanguageByIndex(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetLanguageByIndex(index)");
        return 0;
    }

    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return 0;
    }

    int32_t wanted = static_cast<uint32_t>(lua_tonumber(L, 1));
    int32_t numRecords = g_languagesDB.m_numRecords;
    int32_t found = 0;

    for (int32_t i = 0; i < numRecords; i++) {
        const LanguagesRec* rec = g_languagesDB.GetRecordByIndex(i);
        uint32_t skill;

        if (rec && player->GetLanguageSkill(rec->m_id, skill) && ++found == wanted) {
            lua_pushstring(L, rec->m_name);
            return 1;
        }
    }

    return 0;
}

// OFFSET: 0x008E7020/0x1406153A0
static int32_t Script_SetChatWindowSavedPosition(lua_State* L) {
    if (!lua_isnumber(L, 1) || !lua_isstring(L, 2) || !lua_isnumber(L, 3) || !lua_isnumber(L, 4)) {
        luaL_error(L, "Usage: SetChatWindowSavedPosition(index, \"point\", xOffsetRatio, yOffsetRatio)");
        return 0;
    }

    uint32_t index = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1;

    if (index >= 10) {
        return 0;
    }

    FRAMEPOINT point;

    if (!StringToFramePoint(lua_tolstring(L, 2, NULL), point)) {
        luaL_error(L, "Unknown Region Point");
        return 0;
    }

    ChatWindowInfo* window = &s_chatWindows[index];
    window->m_unkE8 = point;
    float x = static_cast<float>(lua_tonumber(L, 3));
    float y = static_cast<float>(lua_tonumber(L, 4));
    window->m_unkEC = x;
    window->m_unkF0 = y;
    window->m_unkF4 = 1;
    return 0;
}

// OFFSET: 0x008FA580/0x14062DEA0
static int32_t Script_ListChannels(lua_State* L) {
    char text[3000];
    text[0] = 0;

    for (uint32_t i = 0; i < CGChat::s_channels.Count(); i++) {
        ChatChannel* channel = &CGChat::s_channels.m_data[i];

        if (channel->m_channelNumber && !channel->m_unk9C) {
            char entry[138];
            SStrPrintf(entry, sizeof(entry), "[%d. %s] ", channel->m_channelNumber, static_cast<const char*>(channel->m_name));

            if (SStrLen(&text[SStrLen(entry)]) >= sizeof(text)) {
                CGChat::AddChatMessage(text, CHAT_MSG_CHANNEL_LIST, NULL);
                text[0] = 0;
            }

            SStrPack(text, entry, sizeof(text));
        }
    }

    CGChat::AddChatMessage(text, CHAT_MSG_CHANNEL_LIST, NULL);
    return 0;
}

// OFFSET: 0x008E6C00/0x140614FC0
static int32_t Script_SetChatWindowColor(lua_State* L) {
    if (!lua_isnumber(L, 1) || !lua_isnumber(L, 2) || !lua_isnumber(L, 3) || !lua_isnumber(L, 4)) {
        luaL_error(L, "Usage: SetChatWindowColor(index, r, g, b)");
        return 0;
    }

    uint32_t index = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1;

    if (index >= 10) {
        return 0;
    }

    ChatWindowInfo* window = &s_chatWindows[index];
    window->m_color.r = static_cast<uint8_t>(lua_tonumber(L, 2) * 255.0);
    window->m_color.g = static_cast<uint8_t>(lua_tonumber(L, 3) * 255.0);
    window->m_color.b = static_cast<uint8_t>(lua_tonumber(L, 4) * 255.0);
    return 0;
}

// OFFSET: 0x008E65C0/0x140614900
static int32_t Script_GetChatWindowInfo(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetChatWindowInfo(index)");
        return 0;
    }

    uint32_t index = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1;

    if (index >= 10) {
        return 0;
    }

    lua_pushstring(L, s_chatWindows[index].m_name);
    lua_pushnumber(L, s_chatWindows[index].m_unkD0);
    lua_pushnumber(L, s_chatWindows[index].m_color.r * (1.0f / 255.0f));
    lua_pushnumber(L, s_chatWindows[index].m_color.g * (1.0f / 255.0f));
    lua_pushnumber(L, s_chatWindows[index].m_color.b * (1.0f / 255.0f));
    lua_pushnumber(L, s_chatWindows[index].m_color.a * (1.0f / 255.0f));

    if (s_chatWindows[index].m_shown) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    if (s_chatWindows[index].m_unkD8) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    uint32_t docked = s_chatWindows[index].m_docked;

    if (docked) {
        lua_pushnumber(L, docked);
    } else {
        lua_pushnil(L);
    }

    int32_t uninteractable = s_chatWindows[index].m_unkE4;

    if (uninteractable) {
        lua_pushnumber(L, uninteractable);
    } else {
        lua_pushnil(L);
    }

    return 10;
}

// OFFSET: 0x008E8FB0/0x140617750
static void ChannelCommand(lua_State* L, uint32_t msgId, const char* name) {
    if (!lua_isstring(L, 1)) {
        char usage[512];
        SStrPrintf(usage, sizeof(usage), "Usage: %s(\"channel\")", name);
        luaL_error(L, usage);
        return;
    }

    const char* channel = CGChat::GetChannelString(lua_tolstring(L, 1, NULL));

    if (!channel) {
        return;
    }

    CDataStore msg;
    msg.Put(msgId);
    msg.PutString(channel);
    msg.Finalize();
    ClientServices::Send2(&msg);
}

// OFFSET: 0x008EAFA0/0x140619EF0
static int32_t Script_DeclineInvite(lua_State* L) {
    ChannelCommand(L, CMSG_CHAT_REPORT_IGNORED, "DeclineInvite");
    return 0;
}

// OFFSET: 0x008EB850/0x14061A8E0
static int32_t Script_SendAddonMessage(lua_State* L) {
    if (!ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__)) {
        return 0;
    }

    const char* prefix = lua_tolstring(L, 1, NULL);
    const char* text = lua_tolstring(L, 2, NULL);

    if (!prefix) {
        prefix = "";
    }

    if (!text) {
        text = "";
    }

    if (!*prefix && !*text) {
        luaL_error(L, "Usage: SendAddonMessage(\"prefix\", \"message\" [,\"type\"] [,\"target\"])");
        return 0;
    }

    int32_t type = CHAT_MSG_PARTY;

    if (lua_isstring(L, 3)) {
        if (!StringToChatType(lua_tolstring(L, 3, NULL), &type) || (type != CHAT_MSG_PARTY && type != CHAT_MSG_RAID && type != CHAT_MSG_GUILD && type != CHAT_MSG_OFFICER && type != CHAT_MSG_BATTLEGROUND && type != CHAT_MSG_WHISPER)) {
            luaL_error(L, "Unknown addon chat type");
            return 0;
        }
    }

    uint32_t length = SStrLen(prefix);

    if (SStrLen(prefix) >= 17) {
        luaL_error(L, "Prefix is too long");
        return 0;
    }

    if (!length) {
        luaL_error(L, "Prefix cannot be an empty string");
        return 0;
    }

    if (type == CHAT_MSG_RAID) {
        if (!CGRaidInfo::m_numMembers) {
            type = CHAT_MSG_PARTY;
        }
    }

    if (type == CHAT_MSG_PARTY && !CGPartyInfo::InParty()) {
        return 0;
    }

    const char* target = lua_tolstring(L, 4, NULL);

    if (type == CHAT_MSG_WHISPER && (!target || !*target)) {
        luaL_error(L, "SendAddonMessage(): Whisper message missing target player!");
        return 0;
    }

    CGChat::SendChat(0xFFFFFFFF, text, type, target, prefix);
    return 0;
}

// OFFSET: 0x008E9F50/0x140618BC0
static int32_t Script_ChangeChatColor(lua_State* L) {
    if (!lua_isstring(L, 1) || !lua_isnumber(L, 2) || !lua_isnumber(L, 3) || !lua_isnumber(L, 4)) {
        luaL_error(L, "Usage: ChangeChatColor(chatType, r, g, b)");
        return 0;
    }

    const char* type = lua_tolstring(L, 1, NULL);
    uint8_t r = static_cast<uint8_t>(lua_tonumber(L, 2) * 255.0);
    uint8_t g = static_cast<uint8_t>(lua_tonumber(L, 3) * 255.0);
    uint8_t b = static_cast<uint8_t>(lua_tonumber(L, 4) * 255.0);
    int32_t found = 0;

    for (uint32_t i = 0; i < CHAT_MSG_MAX; i++) {
        if (!SStrCmpI(type, s_chatColors[i].m_name, STORM_MAX_STR)) {
            s_chatColors[i].m_color[0] = r;
            s_chatColors[i].m_color[1] = g;
            s_chatColors[i].m_color[2] = b;
            found = 1;
            break;
        }
    }

    for (uint32_t i = 0; i < s_channelColors.Count(); i++) {
        if (!SStrCmpI(type, s_channelColors.m_data[i].m_name, STORM_MAX_STR)) {
            s_channelColors.m_data[i].m_color[0] = r;
            s_channelColors.m_data[i].m_color[1] = g;
            s_channelColors.m_data[i].m_color[2] = b;
            found = 1;
            break;
        }
    }

    if (found) {
        FrameScript_SignalEvent(SCRIPT_EVENT_UPDATE_CHAT_COLOR, "%s%f%f%f", type, r * (1.0f / 255.0f), g * (1.0f / 255.0f), b * (1.0f / 255.0f));
    }

    return 0;
}

// OFFSET: 0x008EA240/0x140618F00
static int32_t Script_SetChatColorNameByClass(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: SetChatColorNameByClass(chatType, colorNameByClass)");
        return 0;
    }

    const char* type = lua_tolstring(L, 1, NULL);
    bool colorNameByClass = StringToBOOL(L, 2, 0) != 0;
    int32_t found = 0;

    for (uint32_t i = 0; i < CHAT_MSG_MAX; i++) {
        if (!SStrCmpI(type, s_chatColors[i].m_name, STORM_MAX_STR)) {
            s_chatColors[i].m_unk48 = colorNameByClass;
            found = 1;
            break;
        }
    }

    for (uint32_t i = 0; i < s_channelColors.Count(); i++) {
        if (!SStrCmpI(type, s_channelColors.m_data[i].m_name, STORM_MAX_STR)) {
            s_channelColors.m_data[i].m_unk48 = colorNameByClass;
            found = 1;
            break;
        }
    }

    if (found) {
        FrameScript_SignalEvent(SCRIPT_EVENT_UPDATE_CHAT_COLOR_NAME_BY_CLASS, "%s%b", type, colorNameByClass);
    }

    return 0;
}

// OFFSET: 0x008EA350/0x140619040
static int32_t Script_GetChatTypeIndex(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: GetChatTypeIndex(type)");
        return 0;
    }

    const char* type = lua_tolstring(L, 1, NULL);

    for (uint32_t i = 0; i < CHAT_MSG_MAX; i++) {
        if (!SStrCmpI(type, s_chatColors[i].m_name, STORM_MAX_STR)) {
            lua_pushnumber(L, i + 1);
            return 1;
        }
    }

    for (uint32_t i = 0; i < s_channelColors.Count(); i++) {
        if (!SStrCmpI(type, s_channelColors.m_data[i].m_name, STORM_MAX_STR)) {
            lua_pushnumber(L, i + CHAT_MSG_MAX + 1);
            return 1;
        }
    }

    lua_pushnumber(L, 0.0);
    return 1;
}

// OFFSET: 0x008E9240/0x140617AB0
static int32_t Script_GetChannelList(lua_State* L) {
    int32_t count = 0;

    for (uint32_t i = 0; i < CGChat::s_channels.Count(); i++) {
        ChatChannel* channel = &CGChat::s_channels.m_data[i];

        if (channel->m_channelNumber && !channel->m_unk9C) {
            lua_pushnumber(L, channel->m_channelNumber);
            ChatChannelsRec* rec = channel->m_zoneChannelID ? g_chatChannelsDB.GetRecord(channel->m_zoneChannelID) : NULL;

            if (rec) {
                lua_pushstring(L, rec->m_shortcut);
            } else {
                lua_pushstring(L, channel->m_name);
            }

            count++;
        }
    }

    return 2 * count;
}

// OFFSET: 0x008E9530/0x140617E70
static int32_t Script_GetChannelName(lua_State* L) {
    ChatChannel* channel;

    if (lua_isnumber(L, 1)) {
        int32_t number = static_cast<int32_t>(lua_tonumber(L, 1));

        if (number < 1 || number > static_cast<int32_t>(CGChat::s_channels.Count())) {
            channel = NULL;
        } else {
            channel = &CGChat::s_channels.m_data[number - 1];

            if (channel->m_channelNumber != number || channel->m_unk9C) {
                channel = NULL;
            }
        }
    } else {
        if (!lua_isstring(L, 1)) {
            luaL_error(L, "Usage: GetChannelName([channelIndex] or [channelName])");
            return 0;
        }

        channel = GetChannelByName(lua_tolstring(L, 1, NULL));
    }

    lua_pushnumber(L, channel ? channel->m_channelNumber : 0);
    lua_pushstring(L, channel ? channel->m_name : NULL);
    lua_pushnumber(L, channel ? channel->m_unk98 : 0);
    return 3;
}

// PROVEN: b0b603a5 IDENTICAL/NONE
// OFFSET: 0x008E74A0/0x1406157C0
static int32_t Script_IsVoiceChatAllowed(lua_State* L) {
    if (!CGChat::m_voiceChatSystemStatus || ComSatClient_IsComsatDisabledByClient()) {
        lua_pushnil(L);
        return 1;
    }

    lua_pushnumber(L, 1.0);
    return 1;
}

// PROVEN: 9c610585 IDENTICAL/NONE
// OFFSET: 0x008E7430/0x140615740
static int32_t Script_IsVoiceChatEnabled(lua_State* L) {
    int32_t enabled = s_cvarEnableVoiceChat && s_cvarEnableVoiceChat->m_intValue;
    int32_t disabledByClient = ComSatClient_IsComsatDisabledByClient();

    if (CGChat::m_voiceChatSystemStatus && enabled && !disabledByClient) {
        lua_pushnumber(L, 1.0);
        return 1;
    }

    lua_pushnil(L);
    return 1;
}

// OFFSET: 0x008E68F0/0x140614C50
static int32_t Script_GetChatWindowMessages(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetChatWindowMessages(index)");
        return 0;
    }

    uint32_t index = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1;

    if (index >= 10) {
        return 0;
    }

    int32_t count = 0;

    for (uint32_t i = 0; i < 49; i++) {
        if (s_chatWindows[index].m_messageGroups[i]) {
            lua_checkstack(L, 1);
            lua_pushstring(L, s_chatMessageGroups[i].m_name);
            count++;
        }
    }

    return count;
}

// OFFSET: 0x008E69A0/0x140614D20
static int32_t Script_AddChatWindowMessages(lua_State* L) {
    if (!lua_isnumber(L, 1) || !lua_isstring(L, 2)) {
        luaL_error(L, "Usage: AddChatWindowMessages(index, \"messageGroup\")");
        return 0;
    }

    uint32_t index = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1;

    if (index >= 10) {
        return 0;
    }

    const char* group = lua_tolstring(L, 2, NULL);

    for (uint32_t i = 0; i < 49; i++) {
        if (!SStrCmpI(group, s_chatMessageGroups[i].m_name, STORM_MAX_STR)) {
            s_chatWindows[index].m_messageGroups[i] = 1;
            return 0;
        }
    }

    return 0;
}

// OFFSET: 0x008E6A80/0x140614E20
static int32_t Script_RemoveChatWindowMessages(lua_State* L) {
    if (!lua_isnumber(L, 1) || !lua_isstring(L, 2)) {
        luaL_error(L, "Usage: RemoveChatWindowMessages(index, \"messageGroup\")");
        return 0;
    }

    uint32_t index = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1;

    if (index >= 10) {
        return 0;
    }

    const char* group = lua_tolstring(L, 2, NULL);

    for (uint32_t i = 0; i < 49; i++) {
        if (!SStrCmpI(group, s_chatMessageGroups[i].m_name, STORM_MAX_STR)) {
            s_chatWindows[index].m_messageGroups[i] = 0;
            return 0;
        }
    }

    return 0;
}

// OFFSET: 0x008EA870/0x140619770
static int32_t Script_GetChannelDisplayInfo(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetChannelDisplayInfo(index)");
        return 0;
    }

    uint32_t index = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1;

    if (index >= s_numDisplayChannels) {
        return 0;
    }

    DisplayChannel* display = &CGChat::s_displayChannels[index];

    if (display->m_headerIndex >= 0) {
        lua_pushstring(L, FrameScript_GetText(s_channelCategories[display->m_headerIndex], -1, GENDER_NOT_APPLICABLE));
        lua_pushnumber(L, 1.0);

        if (display->m_collapsed) {
            lua_pushnumber(L, 1.0);
        } else {
            lua_pushnil(L);
        }

        lua_pushnil(L);

        if (display->m_count) {
            lua_pushnumber(L, display->m_count);
        } else {
            lua_pushnil(L);
        }

        lua_pushnil(L);
        lua_pushnil(L);
        lua_pushnil(L);
        lua_pushnil(L);
        return 9;
    }

    int32_t channelIndex = display->m_channelIndex;

    if (channelIndex < 0) {
        const char* token;

        switch (channelIndex) {
            case -1:
                token = "CHAT_MSG_PARTY";
                break;

            case -2:
                token = "CHAT_MSG_RAID";
                break;

            case -3:
                token = "CHAT_MSG_BATTLEGROUND";
                break;

            default:
                for (int32_t i = 0; i < 9; i++) {
                    lua_pushnil(L);
                }

                return 9;
        }

        lua_pushstring(L, FrameScript_GetText(token, -1, GENDER_NOT_APPLICABLE));
        lua_pushnil(L);
        lua_pushnil(L);
        lua_pushnil(L);

        if (display->m_count) {
            lua_pushnumber(L, display->m_count);
        } else {
            lua_pushnil(L);
        }

        if (display->m_enabled) {
            lua_pushnumber(L, 1.0);
        } else {
            lua_pushnil(L);
        }

        lua_pushstring(L, s_channelCategories[0]);
    } else {
        const char* name = CGChat::GetChannelShortName(channelIndex + 1, true);
        lua_pushstring(L, name ? name : "");
        lua_pushnil(L);
        lua_pushnil(L);
        lua_pushnumber(L, CGChat::s_channels.m_data[display->m_channelIndex].m_channelNumber);

        if (display->m_count) {
            lua_pushnumber(L, display->m_count);
        } else {
            lua_pushnil(L);
        }

        if (display->m_enabled) {
            lua_pushnumber(L, 1.0);
        } else {
            lua_pushnil(L);
        }

        lua_pushstring(L, display->m_flags & 1 ? s_channelCategories[1] : s_channelCategories[2]);
    }

    if (display->m_voiceEnabled) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    if (static_cast<uint32_t>(s_activeVoiceChannel) == index) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 9;
}

// PROVEN: 88b61d4c IDENTICAL/NONE
// OFFSET: 0x008EACF0/0x140619C30
static int32_t Script_GetSelectedDisplayChannel(lua_State* L) {
    int32_t index = CGChat::FindDisplayChannelIndex(CGChat::s_activeDisplayChannelName, s_activeDisplayChannelType);

    if (!index) {
        lua_pushnil(L);
        return 1;
    }

    lua_pushnumber(L, index);
    return 1;
}

// OFFSET: 0x008EA450/0x140619150
static int32_t Script_GetChatWindowChannels(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetChatWindowChannels(index)");
        return 0;
    }

    uint32_t index = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1;

    if (index >= 10) {
        return 0;
    }

    ChatWindowInfo& window = s_chatWindows[index];
    int32_t count = 0;

    for (uint32_t i = 0; i < window.m_channels.Count(); i++) {
        if (window.m_channels.m_data[i]) {
            lua_checkstack(L, 2);
            lua_pushstring(L, window.m_channels.m_data[i]);
            lua_pushnumber(L, window.m_channelIDs.m_data[i]);
            count++;
        }
    }

    return 2 * count;
}

// OFFSET: 0x008E7630/0x140615960
static int32_t Script_GetMuteStatus(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        return 0;
    }

    WOWGUID guid;

    if (FriendList::s_friendList && (guid = WOWGUID(), CGChat::ChatTokenToGUID(lua_tolstring(L, 1, NULL), guid), guid) && (FriendList::s_friendList->IsIgnoredOrMuted(guid) || (lua_isstring(L, 2) ? ComSatClient_IsTalkerSquelchedInChannel(guid, ComSatClient_DetermineSessionTypeFromName(lua_tolstring(L, 2, NULL)), lua_tolstring(L, 2, NULL)) : ComSatClient_IsTalkerSquelchedInCurrent(guid)))) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: 0x008F1360/0x140621E60
static int32_t Script_GetChannelRosterInfo(lua_State* L) {
    if (!lua_isnumber(L, 1) || !lua_isnumber(L, 2)) {
        return 0;
    }

    int32_t type;
    const char* name = CGChat::GetChannelStringForDisplay(static_cast<int32_t>(lua_tonumber(L, 1)) - 1, &type);

    if (!name || type == 4) {
        return 0;
    }

    if (!ChannelNamesEqual(CGChat::s_activeDisplayChannelName, name, s_activeDisplayChannelType, type)) {
        SetCurrentActiveDisplayChannel(name, type);

        switch (type) {
            case 0:
                ChannelDisplayCommand(L, CMSG_CHAT_CHANNEL_DISPLAY_LIST, "GetChannelRosterInfo");
                return 0;

            case 2:
                BuildPartyDisplayList();
                return 0;

            case 1:
            case 3:
                BuildRaidDisplayList(type);
                break;
        }

        return 0;
    }

    if (s_displayChannelMemberCount) {
        return 0;
    }

    uint32_t index = static_cast<uint32_t>(lua_tonumber(L, 2)) - 1;

    if (index >= s_pendingUsers.Count()) {
        return 0;
    }

    PENDINGUSERLIST* user = &s_pendingUsers.m_data[index];

    if (!user) {
        return 0;
    }

    WOWGUID none = WOWGUID();
    NameCache* record = g_nameDBCache.GetRecord(user->m_guid.m_guid, none, NULL, NULL, false);

    if (record && record->m_name[0]) {
        lua_pushstring(L, record->m_name);
    } else {
        lua_pushstring(L, FrameScript_GetText("UNKNOWN", -1, GENDER_NOT_APPLICABLE));
    }

    if (user->m_flags & 0x1) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    if (user->m_flags & 0x2) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    if (user->m_flags & 0x20) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    if (user->m_flags & 0x40) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    if (user->m_flags & 0x80) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 6;
}

// OFFSET: 0x008E6490/0x1406147C0
static int32_t EnableLogging(lua_State* L, int32_t index) {
    if (lua_gettop(L) > 0) {
        s_chatLogging[index] = StringToBOOL(L, 1, 1);

        if (s_chatLogging[index] && !s_chatLog[index]) {
            OsCreateDirectory("Logs", 0);

            if (!SLogCreate(s_chatLogNames[index], 4, &s_chatLog[index])) {
                s_chatLogging[index] = 0;
            }
        }
    }

    if (s_chatLogging[index]) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// PROVEN: 128a3e1e IDENTICAL/NONE
// OFFSET: 0x008E6530/0x1406148A0
static int32_t Script_LoggingChat(lua_State* L) {
    return EnableLogging(L, 0);
}

// PROVEN: faba59c7 IDENTICAL/NONE
// OFFSET: 0x008E6550/0x1406148B0
static int32_t Script_LoggingCombat(lua_State* L) {
    return EnableLogging(L, 1);
}

// OFFSET: 0x008E7530/0x140615850
static int32_t Script_UnitIsSilenced(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        return 0;
    }

    WOWGUID guid = WOWGUID::Make(0);
    const char* name = lua_tolstring(L, 1, NULL);
    NameCache* record = g_nameDBCache.GetRecordByName(name);

    if (record) {
        guid = record->m_guid;
    } else {
        Script_GetGUIDFromToken(name, guid, false, false);
    }

    if (guid) {
        int32_t silenced;

        if (lua_type(L, 2)) {
            int32_t type = ComSatClient_DetermineSessionTypeFromName(lua_tolstring(L, 2, NULL));
            silenced = ComSatClient_IsTalkerSquelchedInChannel(guid, type, lua_tolstring(L, 2, NULL));
        } else {
            silenced = ComSatClient_IsTalkerSquelchedInCurrent(guid);
        }

        if (silenced) {
            lua_pushnumber(L, 1.0);
            return 1;
        }
    }

    lua_pushnil(L);
    return 1;
}

// OFFSET: 0x008E91A0/0x140617A10
static int32_t Script_ListChannelByName(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        char usage[512];
        SStrPrintf(usage, sizeof(usage), "Usage: %s(\"channel\")", "ListChannelByName");
        luaL_error(L, usage);
        return 0;
    }

    const char* channel = CGChat::GetChannelString(lua_tolstring(L, 1, NULL));

    if (channel) {
        UserClientChatChannelList msg(channel);
        ClientServices::Send(&msg);
    }

    return 0;
}

// OFFSET: 0x008E9310/0x140617BE0
static int32_t Script_SetChannelPassword(lua_State* L) {
    if (!lua_isstring(L, 1) || !lua_isstring(L, 2)) {
        luaL_error(L, "Usage: SetChannelPassword(\"name\", \"password\")");
        return 0;
    }

    const char* channel = CGChat::GetChannelString(lua_tolstring(L, 1, NULL));

    if (channel) {
        UserClientChatChannelPassword msg(channel, lua_tolstring(L, 2, NULL));
        ClientServices::Send(&msg);
    }

    return 0;
}

// OFFSET: 0x008E93B0/0x140617CA0
static int32_t Script_SetChannelOwner(lua_State* L) {
    if (!lua_isstring(L, 1) || !lua_isstring(L, 2)) {
        char usage[512];
        SStrPrintf(usage, sizeof(usage), "Usage: %s(\"channel\", \"name\")", "SetChannelOwner");
        luaL_error(L, usage);
        return 0;
    }

    const char* player = lua_tolstring(L, 2, NULL);

    if (player && SStrLen(player) > 48) {
        luaL_error(L, "Name too long");
        return 0;
    }

    const char* channel = CGChat::GetChannelString(lua_tolstring(L, 1, NULL));

    if (channel) {
        UserClientChatChannelSetOwner msg(channel, player);
        ClientServices::Send(&msg);
    }

    return 0;
}

// OFFSET: 0x008E9490/0x140617DD0
static int32_t Script_DisplayChannelOwner(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        char usage[512];
        SStrPrintf(usage, sizeof(usage), "Usage: %s(\"channel\")", "DisplayChannelOwner");
        luaL_error(L, usage);
        return 0;
    }

    const char* channel = CGChat::GetChannelString(lua_tolstring(L, 1, NULL));

    if (channel) {
        UserClientChatChannelOwner msg(channel);
        ClientServices::Send(&msg);
    }

    return 0;
}

// OFFSET: 0x008E9610/0x140617F80
static int32_t Script_ChannelModerator(lua_State* L) {
    if (!lua_isstring(L, 1) || !lua_isstring(L, 2)) {
        char usage[512];
        SStrPrintf(usage, sizeof(usage), "Usage: %s(\"channel\", \"name\")", "ChannelModerator");
        luaL_error(L, usage);
        return 0;
    }

    const char* player = lua_tolstring(L, 2, NULL);

    if (player && SStrLen(player) > 48) {
        luaL_error(L, "Name too long");
        return 0;
    }

    const char* channel = CGChat::GetChannelString(lua_tolstring(L, 1, NULL));

    if (channel) {
        UserClientChatChannelModerator msg(channel, player);
        ClientServices::Send(&msg);
    }

    return 0;
}

// OFFSET: 0x008E96F0/0x1406180B0
static int32_t Script_ChannelUnmoderator(lua_State* L) {
    if (!lua_isstring(L, 1) || !lua_isstring(L, 2)) {
        char usage[512];
        SStrPrintf(usage, sizeof(usage), "Usage: %s(\"channel\", \"name\")", "ChannelUnmoderator");
        luaL_error(L, usage);
        return 0;
    }

    const char* player = lua_tolstring(L, 2, NULL);

    if (player && SStrLen(player) > 48) {
        luaL_error(L, "Name too long");
        return 0;
    }

    const char* channel = CGChat::GetChannelString(lua_tolstring(L, 1, NULL));

    if (channel) {
        UserClientChatChannelUnmoderator msg(channel, player);
        ClientServices::Send(&msg);
    }

    return 0;
}

// OFFSET: 0x008E97D0/0x1406181E0
static int32_t Script_ChannelMute(lua_State* L) {
    if (!lua_isstring(L, 1) || !lua_isstring(L, 2)) {
        char usage[512];
        SStrPrintf(usage, sizeof(usage), "Usage: %s(\"channel\", \"name\")", "ChannelMute");
        luaL_error(L, usage);
        return 0;
    }

    const char* player = lua_tolstring(L, 2, NULL);

    if (player && SStrLen(player) > 48) {
        luaL_error(L, "Name too long");
        return 0;
    }

    const char* channel = CGChat::GetChannelString(lua_tolstring(L, 1, NULL));

    if (channel) {
        UserClientChatChannelMute msg(channel, player);
        ClientServices::Send(&msg);
    }

    return 0;
}

// OFFSET: 0x008E98B0/0x140618310
static int32_t Script_ChannelUnmute(lua_State* L) {
    if (!lua_isstring(L, 1) || !lua_isstring(L, 2)) {
        char usage[512];
        SStrPrintf(usage, sizeof(usage), "Usage: %s(\"channel\", \"name\")", "ChannelUnmute");
        luaL_error(L, usage);
        return 0;
    }

    const char* player = lua_tolstring(L, 2, NULL);

    if (player && SStrLen(player) > 48) {
        luaL_error(L, "Name too long");
        return 0;
    }

    const char* channel = CGChat::GetChannelString(lua_tolstring(L, 1, NULL));

    if (channel) {
        UserClientChatChannelUnmute msg(channel, player);
        ClientServices::Send(&msg);
    }

    return 0;
}

// OFFSET: 0x008E9990/0x140618440
static int32_t Script_ChannelInvite(lua_State* L) {
    if (!lua_isstring(L, 1) || !lua_isstring(L, 2)) {
        char usage[512];
        SStrPrintf(usage, sizeof(usage), "Usage: %s(\"channel\", \"name\")", "ChannelInvite");
        luaL_error(L, usage);
        return 0;
    }

    const char* player = lua_tolstring(L, 2, NULL);

    if (player && SStrLen(player) > 48) {
        luaL_error(L, "Name too long");
        return 0;
    }

    const char* channel = CGChat::GetChannelString(lua_tolstring(L, 1, NULL));

    if (channel) {
        UserClientChatChannelInvite msg(channel, player);
        ClientServices::Send(&msg);
    }

    return 0;
}

// OFFSET: 0x008E9A70/0x140618570
static int32_t Script_ChannelKick(lua_State* L) {
    if (!lua_isstring(L, 1) || !lua_isstring(L, 2)) {
        char usage[512];
        SStrPrintf(usage, sizeof(usage), "Usage: %s(\"channel\", \"name\")", "ChannelKick");
        luaL_error(L, usage);
        return 0;
    }

    const char* player = lua_tolstring(L, 2, NULL);

    if (player && SStrLen(player) > 48) {
        luaL_error(L, "Name too long");
        return 0;
    }

    const char* channel = CGChat::GetChannelString(lua_tolstring(L, 1, NULL));

    if (channel) {
        UserClientChatChannelKick msg(channel, player);
        ClientServices::Send(&msg);
    }

    return 0;
}

// OFFSET: 0x008E9B50/0x1406186A0
static int32_t Script_ChannelBan(lua_State* L) {
    if (!lua_isstring(L, 1) || !lua_isstring(L, 2)) {
        char usage[512];
        SStrPrintf(usage, sizeof(usage), "Usage: %s(\"channel\", \"name\")", "ChannelBan");
        luaL_error(L, usage);
        return 0;
    }

    const char* player = lua_tolstring(L, 2, NULL);

    if (player && SStrLen(player) > 48) {
        luaL_error(L, "Name too long");
        return 0;
    }

    const char* channel = CGChat::GetChannelString(lua_tolstring(L, 1, NULL));

    if (channel) {
        UserClientChatChannelBan msg(channel, player);
        ClientServices::Send(&msg);
    }

    return 0;
}

// OFFSET: 0x008E9C30/0x1406187D0
static int32_t Script_ChannelUnban(lua_State* L) {
    if (!lua_isstring(L, 1) || !lua_isstring(L, 2)) {
        char usage[512];
        SStrPrintf(usage, sizeof(usage), "Usage: %s(\"channel\", \"name\")", "ChannelUnban");
        luaL_error(L, usage);
        return 0;
    }

    const char* player = lua_tolstring(L, 2, NULL);

    if (player && SStrLen(player) > 48) {
        luaL_error(L, "Name too long");
        return 0;
    }

    const char* channel = CGChat::GetChannelString(lua_tolstring(L, 1, NULL));

    if (channel) {
        UserClientChatChannelUnban msg(channel, player);
        ClientServices::Send(&msg);
    }

    return 0;
}

// OFFSET: 0x008E9D10/0x140618900
static int32_t Script_ChannelToggleAnnouncements(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        char usage[512];
        SStrPrintf(usage, sizeof(usage), "Usage: %s(\"channel\")", "ChannelToggleAnnouncements");
        luaL_error(L, usage);
        return 0;
    }

    const char* channel = CGChat::GetChannelString(lua_tolstring(L, 1, NULL));

    if (channel) {
        UserClientChatChannelAnnouncements msg(channel);
        ClientServices::Send(&msg);
    }

    return 0;
}

// OFFSET: 0x008E9DB0/0x1406189A0
static int32_t Script_ChannelVoiceOn(lua_State* L) {
    if (!lua_isstring(L, 1) || !lua_isstring(L, 2)) {
        char usage[512];
        SStrPrintf(usage, sizeof(usage), "Usage: %s(\"channel\", \"name\")", "ChannelVoiceOn");
        luaL_error(L, usage);
        return 0;
    }

    const char* player = lua_tolstring(L, 2, NULL);

    if (player && SStrLen(player) > 48) {
        luaL_error(L, "Name too long");
        return 0;
    }

    CGChat::GetChannelString(lua_tolstring(L, 1, NULL));
    return 0;
}

// OFFSET: 0x008E9E60/0x140618A90
static int32_t Script_ChannelVoiceOff(lua_State* L) {
    if (!lua_isstring(L, 1) || !lua_isstring(L, 2)) {
        char usage[512];
        SStrPrintf(usage, sizeof(usage), "Usage: %s(\"channel\", \"name\")", "ChannelVoiceOff");
        luaL_error(L, usage);
        return 0;
    }

    const char* player = lua_tolstring(L, 2, NULL);

    if (player && SStrLen(player) > 48) {
        luaL_error(L, "Name too long");
        return 0;
    }

    CGChat::GetChannelString(lua_tolstring(L, 1, NULL));
    return 0;
}

// OFFSET: 0x008E9F10/0x140618B80
static int32_t Script_DisplayChannelVoiceOn(lua_State* L) {
    ChannelDisplayCommand(L, CMSG_CHAT_CHANNEL_VOICE_ON, "DisplayChannelVoiceOn");
    return 0;
}

// OFFSET: 0x008E9F30/0x140618BA0
static int32_t Script_DisplayChannelVoiceOff(lua_State* L) {
    ChannelDisplayCommand(L, CMSG_CHAT_CHANNEL_VOICE_OFF, "DisplayChannelVoiceOff");
    return 0;
}

// OFFSET: 0x008EA170/0x140618E00
static int32_t Script_ResetChatColors(lua_State* L) {
    for (uint32_t i = 0; i < CHAT_MSG_MAX; i++) {
        SStrCopy(s_chatColors[i].m_name, s_chatTypeInfo[i].m_name, sizeof(s_chatColors[i].m_name));
        s_chatColors[i].m_color[0] = s_chatTypeInfo[i].m_color[0];
        s_chatColors[i].m_color[1] = s_chatTypeInfo[i].m_color[1];
        s_chatColors[i].m_color[2] = s_chatTypeInfo[i].m_color[2];
    }

    for (uint32_t i = 0; i < s_channelColors.Count(); i++) {
        s_channelColors.m_data[i].m_color[0] = s_chatColors[CHAT_MSG_CHANNEL].m_color[0];
        s_channelColors.m_data[i].m_color[1] = s_chatColors[CHAT_MSG_CHANNEL].m_color[1];
        s_channelColors.m_data[i].m_color[2] = s_chatColors[CHAT_MSG_CHANNEL].m_color[2];
    }

    return 0;
}

// OFFSET: 0x008EA530/0x140619280
static int32_t Script_RemoveChatWindowChannel(lua_State* L) {
    if (!lua_isnumber(L, 1) || !lua_isstring(L, 2)) {
        luaL_error(L, "Usage: RemoveChatWindowChannel(index, \"channel\")");
        return 0;
    }

    uint32_t index = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1;

    if (index >= 10) {
        return 0;
    }

    const char* channel = CGChat::GetChannelString(lua_tolstring(L, 2, NULL));

    if (!channel) {
        return 0;
    }

    const char* zoneText = CGGameUI::m_zoneText;

    for (int32_t i = 0; i < g_chatChannelsDB.m_numRecords; i++) {
        ChatChannelsRec* rec = g_chatChannelsDB.GetRecordByIndex(i);

        if (SStrCmpUTF8I(rec->m_shortcut, channel, STORM_MAX_STR)) {
            continue;
        }

        if (!zoneText || !*zoneText) {
            return 0;
        }

        if (rec->m_shortcut) {
            s_chatWindows[index].RemoveChannel(rec->m_shortcut);
            return 0;
        }

        break;
    }

    s_chatWindows[index].RemoveChannel(channel);
    return 0;
}

// OFFSET: 0x008EA720/0x140619500
static int32_t Script_EnumerateServerChannels(lua_State* L) {
    uint32_t count = g_chatChannelsDB.m_numRecords;
    lua_checkstack(L, count);
    AreaTableRec* area = g_areaTableDB.GetRecord(CGGameUI::m_areaID);

    if (!area) {
        return 0;
    }

    int32_t pushed = 0;

    for (uint32_t i = 0; i < count; i++) {
        ChatChannelsRec* rec = g_chatChannelsDB.GetRecordByIndex(i);

        if (!(rec->m_flags & 0x10) || (area->m_flags & 0x8)) {
            lua_pushstring(L, *rec->m_shortcut ? rec->m_shortcut : rec->m_name);
            pushed++;
        }
    }

    return pushed;
}

// OFFSET: 0x008EAD50/0x140619C80
static int32_t Script_IsDisplayChannelOwner(lua_State* L) {
    uint32_t index = CGChat::FindDisplayChannelIndex(CGChat::s_activeDisplayChannelName, s_activeDisplayChannelType);

    if (index && CGChat::s_displayChannels[index - 1].m_headerIndex < 0 && (s_activeDisplayChannelFlags & 0x1)) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: 0x008EADC0/0x140619CF0
static int32_t Script_IsDisplayChannelModerator(lua_State* L) {
    uint32_t index = CGChat::FindDisplayChannelIndex(CGChat::s_activeDisplayChannelName, s_activeDisplayChannelType);

    if (index && CGChat::s_displayChannels[index - 1].m_headerIndex < 0 && (s_activeDisplayChannelFlags & 0x3)) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: 0x008EAE30/0x140619D60
static int32_t Script_IsSilenced(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        return 0;
    }

    WOWGUID guid = WOWGUID::Make(0);
    const char* name = lua_tolstring(L, 1, NULL);
    NameCache* record = g_nameDBCache.GetRecordByName(name);

    if (record) {
        guid = record->m_guid;
    } else {
        Script_GetGUIDFromToken(name, guid, false, false);
    }

    if (guid) {
        for (uint32_t i = 0; i < s_pendingUsers.Count(); i++) {
            if (s_pendingUsers.m_data[i].m_guid != guid) {
                continue;
            }

            if (!(s_pendingUsers.m_data[i].m_flags & 0x10)) {
                lua_pushnumber(L, 1.0);
                return 1;
            }

            break;
        }
    }

    lua_pushnil(L);
    return 1;
}

// OFFSET: 0x008EAF10/0x140619E50
static int32_t Script_SetChannelWatch(lua_State* L) {
    ChannelCommand(L, CMSG_SET_CHANNEL_WATCH, "SetChannelWatch");
    return 0;
}

// OFFSET: 0x008EBC20/0x14061AD50
static int32_t Script_DoEmote(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!lua_isstring(L, 1)) {
        if (lua_type(L, 1) == LUA_TNIL && player) {
            player->ClearEmote();
        }

        return 0;
    }

    const char* emote = lua_tolstring(L, 1, NULL);
    const char* dance;

    if (!SStrCmpI(emote, "DANCE", STORM_MAX_STR) && (dance = lua_tolstring(L, 2, NULL)) && CGDanceStudio::HasDance(dance)) {
        PlayDance(dance);
        return 0;
    }

    int32_t i = g_emotesTextDB.m_numRecords;

    if (!i) {
        return 0;
    }

    EmotesTextRec* rec;

    while (true) {
        i--;
        rec = g_emotesTextDB.GetRecordByIndex(i);

        if (!SStrCmpI(emote, rec->m_name, STORM_MAX_STR)) {
            break;
        }

        if (!i) {
            return 0;
        }
    }

    WOWGUID target = CGGameUI::m_lockedTarget;
    const char* token = lua_tolstring(L, 2, NULL);

    if (token && *token && *token != '%') {
        target = CGGameUI::ClosestObjectMatch(token, TYPEMASK_PLAYER, 0, 0, 3.402823466e38f, 0);
    }

    if (!player) {
        return 0;
    }

    int32_t silent = 0;

    if (lua_type(L, 3) == LUA_TBOOLEAN) {
        silent = lua_toboolean(L, 3);
    }

    player->SendTextEmote(rec, target, silent);
    return 0;
}

// OFFSET: 0x008EBDF0/0x14061AF60
static int32_t Script_SetActiveVoiceChannel(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        return 0;
    }

    if (static_cast<int32_t>(lua_tonumber(L, 1))) {
        int32_t type;
        int32_t index = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1;
        const char* channel = CGChat::GetChannelStringForDisplay(index, &type);

        if (channel && CGChat::s_displayChannels[index].m_voiceEnabled) {
            CGChat::SetActiveVoiceSession(channel, type, index);
        }

        return 0;
    }

    if (CGChat::s_displayChannels[s_activeVoiceChannel].m_voiceEnabled) {
        FrameScript_SignalEvent(SCRIPT_EVENT_CHANNEL_VOICE_UPDATE, "%d%d", s_activeVoiceChannel + 1, CGChat::s_displayChannels[s_activeVoiceChannel].m_voiceEnabled);
    } else {
        FrameScript_SignalEvent(SCRIPT_EVENT_CHANNEL_VOICE_UPDATE, "%d", s_activeVoiceChannel + 1);
    }

    s_activeVoiceChannel = 0;
    ComSatClient_DisableCurrentSession();
    return 0;
}

// OFFSET: 0x008EBEF0/0x14061B050
static int32_t Script_CanComplainChat(lua_State* L) {
    const char* text = NULL;
    ChatRecord* record;

    if (lua_isnumber(L, 1)) {
        record = GetChatRecordEx(NULL, NULL, static_cast<uint32_t>(lua_tonumber(L, 1)));
    } else {
        if (!lua_isstring(L, 1)) {
            luaL_error(L, "Usage: CanComplainChat(lineID) or CanComplainChat(name) or CanComplainChat(name, text)");
            return 0;
        }

        const char* name = lua_tolstring(L, 1, NULL);

        if (lua_isstring(L, 2)) {
            text = lua_tolstring(L, 2, NULL);
        }

        record = GetChatRecordEx(name, text, 0);
    }

    if (record && CGChat::CanComplain(record->m_senderGuid)) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: 0x008EC3C0/0x14061B710
static int32_t Script_ChannelSilenceVoice(lua_State* L) {
    const char* channel = CGChat::GetChannelString(lua_tolstring(L, 1, NULL));

    if (!channel) {
        return 0;
    }

    if (SStrCmpI(channel, s_sessionTypeNames[1], STORM_MAX_STR) && SStrCmpI(channel, s_sessionTypeNames[2], STORM_MAX_STR) && SStrCmpI(channel, s_sessionTypeNames[0], STORM_MAX_STR) && SStrCmpI(channel, FrameScript_GetText("PARTY", -1, GENDER_NOT_APPLICABLE), STORM_MAX_STR) && SStrCmpI(channel, FrameScript_GetText("RAID", -1, GENDER_NOT_APPLICABLE), STORM_MAX_STR) && SStrCmpI(channel, FrameScript_GetText("BATTLEFIELDS", -1, GENDER_NOT_APPLICABLE), STORM_MAX_STR)) {
        if (!lua_isstring(L, 1) || !lua_isstring(L, 2)) {
            char usage[512];
            SStrPrintf(usage, sizeof(usage), "Usage: %s(\"channel\", \"name\")", "ChannelSilenceVoice");
            luaL_error(L, usage);
            return 0;
        }

        const char* player = lua_tolstring(L, 2, NULL);

        if (player && SStrLen(player) > 48) {
            luaL_error(L, "Name too long");
            return 0;
        }

        const char* channel = CGChat::GetChannelString(lua_tolstring(L, 1, NULL));

        if (channel) {
            UserClientChatChannelSilenceVoice msg(channel, player);
            ClientServices::Send(&msg);
        }

        return 0;
    }

    CGChat::SilencePartyMember(true, lua_tolstring(L, 2, NULL));
    return 0;
}

// OFFSET: 0x008EC580/0x14061B950
static int32_t Script_ChannelSilenceAll(lua_State* L) {
    const char* channel = CGChat::GetChannelString(lua_tolstring(L, 1, NULL));

    if (!channel) {
        return 0;
    }

    if (SStrCmpI(channel, s_sessionTypeNames[1], STORM_MAX_STR) && SStrCmpI(channel, s_sessionTypeNames[2], STORM_MAX_STR) && SStrCmpI(channel, s_sessionTypeNames[0], STORM_MAX_STR)) {
        if (!lua_isstring(L, 1) || !lua_isstring(L, 2)) {
            char usage[512];
            SStrPrintf(usage, sizeof(usage), "Usage: %s(\"channel\", \"name\")", "ChannelSilenceAll");
            luaL_error(L, usage);
            return 0;
        }

        const char* player = lua_tolstring(L, 2, NULL);

        if (player && SStrLen(player) > 48) {
            luaL_error(L, "Name too long");
            return 0;
        }

        const char* channel = CGChat::GetChannelString(lua_tolstring(L, 1, NULL));

        if (channel) {
            UserClientChatChannelSilenceAll msg(channel, player);
            ClientServices::Send(&msg);
        }

        return 0;
    }

    CGChat::SilencePartyMember(true, lua_tolstring(L, 2, NULL));
    return 0;
}

// OFFSET: 0x008EC6E0/0x14061BB30
static int32_t Script_ChannelUnSilenceVoice(lua_State* L) {
    const char* channel = CGChat::GetChannelString(lua_tolstring(L, 1, NULL));

    if (!channel) {
        return 0;
    }

    if (SStrCmpI(channel, s_sessionTypeNames[1], STORM_MAX_STR) && SStrCmpI(channel, s_sessionTypeNames[2], STORM_MAX_STR) && SStrCmpI(channel, s_sessionTypeNames[0], STORM_MAX_STR) && SStrCmpI(channel, FrameScript_GetText("PARTY", -1, GENDER_NOT_APPLICABLE), STORM_MAX_STR) && SStrCmpI(channel, FrameScript_GetText("RAID", -1, GENDER_NOT_APPLICABLE), STORM_MAX_STR) && SStrCmpI(channel, FrameScript_GetText("BATTLEFIELDS", -1, GENDER_NOT_APPLICABLE), STORM_MAX_STR)) {
        if (!lua_isstring(L, 1) || !lua_isstring(L, 2)) {
            char usage[512];
            SStrPrintf(usage, sizeof(usage), "Usage: %s(\"channel\", \"name\")", "ChannelUnSilenceVoice");
            luaL_error(L, usage);
            return 0;
        }

        const char* player = lua_tolstring(L, 2, NULL);

        if (player && SStrLen(player) > 48) {
            luaL_error(L, "Name too long");
            return 0;
        }

        const char* channel = CGChat::GetChannelString(lua_tolstring(L, 1, NULL));

        if (channel) {
            UserClientChatChannelUnsilenceVoice msg(channel, player);
            ClientServices::Send(&msg);
        }

        return 0;
    }

    CGChat::SilencePartyMember(false, lua_tolstring(L, 2, NULL));
    return 0;
}

// OFFSET: 0x008EC8A0/0x14061BD70
static int32_t Script_ChannelUnSilenceAll(lua_State* L) {
    const char* channel = CGChat::GetChannelString(lua_tolstring(L, 1, NULL));

    if (!channel) {
        return 0;
    }

    if (SStrCmpI(channel, s_sessionTypeNames[1], STORM_MAX_STR) && SStrCmpI(channel, s_sessionTypeNames[2], STORM_MAX_STR) && SStrCmpI(channel, s_sessionTypeNames[0], STORM_MAX_STR)) {
        if (!lua_isstring(L, 1) || !lua_isstring(L, 2)) {
            char usage[512];
            SStrPrintf(usage, sizeof(usage), "Usage: %s(\"channel\", \"name\")", "ChannelUnSilenceAll");
            luaL_error(L, usage);
            return 0;
        }

        const char* player = lua_tolstring(L, 2, NULL);

        if (player && SStrLen(player) > 48) {
            luaL_error(L, "Name too long");
            return 0;
        }

        const char* channel = CGChat::GetChannelString(lua_tolstring(L, 1, NULL));

        if (channel) {
            UserClientChatChannelUnsilenceAll msg(channel, player);
            ClientServices::Send(&msg);
        }

        return 0;
    }

    CGChat::SilencePartyMember(false, lua_tolstring(L, 2, NULL));
    return 0;
}

// OFFSET: 0x008EDC00/0x14061D420
static int32_t Script_AddChatWindowChannel(lua_State* L) {
    if (!lua_isnumber(L, 1) || !lua_isstring(L, 2)) {
        luaL_error(L, "Usage: AddChatWindowChannel(index, \"channel\")");
        return 0;
    }

    uint32_t index = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1;

    if (index >= 10) {
        return 0;
    }

    const char* channel = lua_tolstring(L, 2, NULL);
    const char* zoneChannel = NULL;
    int32_t channelId = 0;
    const char* zoneText = CGGameUI::m_zoneText;

    for (int32_t i = 0; i < g_chatChannelsDB.m_numRecords; i++) {
        ChatChannelsRec* rec = g_chatChannelsDB.GetRecordByIndex(i);

        if (SStrCmpUTF8I(rec->m_shortcut, channel, STORM_MAX_STR)) {
            continue;
        }

        if (!zoneText || !*zoneText) {
            return 0;
        }

        zoneChannel = rec->m_shortcut;
        channelId = rec->m_id;
        break;
    }

    lua_pushnumber(L, channelId);
    s_chatWindows[index].AddChannel(zoneChannel ? zoneChannel : channel, channelId);
    return 1;
}

// OFFSET: 0x008EDD50/0x14061D5E0
static int32_t Script_ResetChatWindows(lua_State* L) {
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

    for (uint32_t i = 0; i < 10; i++) {
        memset(s_chatWindows[i].m_messageGroups, 0, sizeof(s_chatWindows[i].m_messageGroups));
        s_chatWindows[i].Reset();
        s_chatWindows[i].Init();

        if (i == 0) {
            s_chatWindows[0].m_shown = 1;

            for (uint32_t j = 0; j < 42; j++) {
                s_chatWindows[0].m_messageGroups[j] = s_chatMessageGroups[j].m_default != 0;
            }

            s_chatWindows[0].m_docked = 1;
        } else if (i == 1) {
            s_chatWindows[1].m_shown = 1;
            s_chatWindows[1].m_docked = 2;
            s_chatWindows[1].m_messageGroups[42] = 1;
            s_chatWindows[1].m_messageGroups[43] = 1;
            s_chatWindows[1].m_messageGroups[44] = 1;
            s_chatWindows[1].m_messageGroups[45] = 1;
            s_chatWindows[1].m_messageGroups[46] = 1;
            s_chatWindows[1].m_messageGroups[47] = 1;
            s_chatWindows[1].m_messageGroups[48] = 1;
        }
    }

    int32_t count = g_chatChannelsDB.m_numRecords;

    for (int32_t i = 0; i < count; i++) {
        ChatChannelsRec* rec = g_chatChannelsDB.GetRecordByIndex(i);

        if (!(rec->m_flags & 0x1)) {
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

    for (uint32_t i = 0; i < s_channelColors.Count(); i++) {
        ChatColorInfo* color = &s_channelColors.m_data[i];
        FrameScript_SignalEvent(SCRIPT_EVENT_UPDATE_CHAT_COLOR, "%s%f%f%f", static_cast<const char*>(color->m_name), color->m_color[0] * (1.0f / 255.0f), color->m_color[1] * (1.0f / 255.0f), color->m_color[2] * (1.0f / 255.0f));
        FrameScript_SignalEvent(SCRIPT_EVENT_UPDATE_CHAT_COLOR_NAME_BY_CLASS, "%s%b", static_cast<const char*>(color->m_name), color->m_unk48);
    }

    return 0;
}

// OFFSET: 0x008F15C0/0x1406220D0
static int32_t Script_GetNumChannelMembers(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        lua_pushnil(L);
        return 1;
    }

    int32_t type;
    const char* channel = CGChat::GetChannelStringForDisplay(static_cast<uint32_t>(lua_tonumber(L, 1)) - 1, &type);

    if (!channel || type == 4) {
        lua_pushnil(L);
        return 1;
    }

    uint32_t count;

    if (!ChannelNamesEqual(CGChat::s_activeDisplayChannelName, channel, s_activeDisplayChannelType, type)) {
        SetCurrentActiveDisplayChannel(channel, type);

        switch (type) {
            case 0:
                ChannelDisplayCommand(L, CMSG_CHAT_CHANNEL_DISPLAY_LIST, "GetNumChannelMembers");
                lua_pushnil(L);
                return 1;

            case 2:
                count = BuildPartyDisplayList();
                break;

            case 1:
            case 3:
                count = BuildRaidDisplayList(type);
                break;

            default:
                lua_pushnil(L);
                return 1;
        }
    } else {
        if (s_displayChannelMemberCount) {
            lua_pushnil(L);
            return 1;
        }

        count = s_pendingUsers.Count();
    }

    if (!count) {
        lua_pushnil(L);
        return 1;
    }

    lua_pushnumber(L, count);
    return 1;
}

// OFFSET: 0x008F47C0/0x140626830
static int32_t Script_LeaveChannelByName(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: LeaveChannelByName(\"name\")");
        return 0;
    }

    InternalLeaveChannel(lua_tolstring(L, 1, NULL), false);
    return 0;
}

// OFFSET: 0x008FA390/0x14062DC60
static int32_t JoinChannelFromScript(lua_State* L, bool permanent) {
    if (!lua_isstring(L, 1)) {
        char function[1024];
        luaL_error(L, "Usage: %s(\"name\" [,\"password\"] [,index] [,hasVoice])", FrameScript_GetCurrentFunction(L, function, sizeof(function)));
        return 0;
    }

    const char* name = lua_tolstring(L, 1, NULL);
    const char* password = lua_isstring(L, 2) ? lua_tolstring(L, 2, NULL) : "";

    if (SStrChr(name, ' ') || SStrLen(name) >= 128 || SStrLen(password) >= 128) {
        return 0;
    }

    char filtered[128];
    strcpy(filtered, name);

    if (ProfanityFilter(filtered, false, true)) {
        CGGameUI::DisplayError(ERR_PROFANE_CHAT_NAME);
        return 0;
    }

    uint32_t window = 0xFFFFFFFF;

    if (lua_isnumber(L, 3)) {
        window = static_cast<uint32_t>(lua_tonumber(L, 3)) - 1;
    }

    bool voice = StringToBOOL(L, 4, 0) != 0;
    const char* zoneChannelName = NULL;
    int32_t zoneChannelID = 0;

    if (!InternalJoinChannel(name, password, window, &zoneChannelName, &zoneChannelID, false, voice, permanent, 0)) {
        return 0;
    }

    lua_pushnumber(L, zoneChannelID);

    if (zoneChannelName) {
        lua_pushstring(L, zoneChannelName);
    } else {
        lua_pushnil(L);
    }

    return 2;
}

// OFFSET: 0x008FA540/0x14062DE80
static int32_t Script_JoinChannelByName(lua_State* L) {
    return JoinChannelFromScript(L, false);
}

// OFFSET: 0x008FA540/0x14062DE80
static int32_t Script_JoinTemporaryChannel(lua_State* L) {
    return JoinChannelFromScript(L, false);
}

// OFFSET: 0x008FA560/0x14062DE90
static int32_t Script_JoinPermanentChannel(lua_State* L) {
    return JoinChannelFromScript(L, true);
}

// OFFSET: 0x008FA6B0/0x14062E010
static int32_t Script_ReportPlayer(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: ReportPlayer(\"complaintType\", \"pending\" or chatLineID[, comment])");
        return 0;
    }

    if (!CGGameUI::CanPerformAction(26)) {
        return 0;
    }

    const char* typeName = lua_tolstring(L, 1, NULL);
    uint32_t complaintType;

    if (!SStrCmpI("spam", typeName, STORM_MAX_STR)) {
        complaintType = 0;
    } else if (!SStrCmpI("language", typeName, STORM_MAX_STR)) {
        complaintType = 10;
    } else if (!SStrCmpI("badplayername", typeName, STORM_MAX_STR)) {
        complaintType = 3;
    } else if (!SStrCmpI("badguildname", typeName, STORM_MAX_STR)) {
        complaintType = 6;
    } else if (!SStrCmpI("badarenaname", typeName, STORM_MAX_STR)) {
        complaintType = 9;
    } else if (!SStrCmpI("cheater", typeName, STORM_MAX_STR)) {
        complaintType = 1;
    } else {
        return 0;
    }

    uint32_t lineID = 0;
    bool hasLine = false;
    WOWGUID target;

    if (lua_isnumber(L, 2)) {
        hasLine = true;
        lineID = static_cast<uint32_t>(lua_tonumber(L, 2));
        ChatRecord* record = GetChatRecordEx(NULL, NULL, lineID);

        if (!record || !record->m_description[0]) {
            return 0;
        }

        target = record->m_senderGuid;
    } else {
        if (!lua_isstring(L, 2)) {
            return 0;
        }

        target = s_pendingReportTarget;
    }

    if (!target.m_guid) {
        return 0;
    }

    bool submitted = false;

    if (CGGMTicketInfo::s_quickTicketSystemStatus) {
        CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();

        if (!player) {
            return 0;
        }

        if (CGGMTicketInfo::s_quickTicketThrottle.IncrementAndCheck()) {
            UserClientSupportTicketSubmitComplaint msg;
            msg.m_header.m_mapID = CGGameUI::m_iCurrentMapID;
            msg.m_header.m_position = player->GetPosition();
            msg.m_header.m_facing = player->GetFacing();
            uint32_t lineIndex = 0;

            for (int32_t i = 0; i < 60; i++) {
                ChatRecord* record = GetChatRecord(i);

                if (record && record->m_description[0]) {
                    JamCliSupportTicketChatLine* line = msg.m_chatLog.m_lines.New();
                    line->m_timestamp = record->m_time;
                    SStrCopy(line->m_text, record->m_description, sizeof(line->m_text));

                    if (hasLine && record->m_lineID == lineID) {
                        msg.m_chatLog.m_reportLineIndex = lineIndex;
                        msg.m_chatLog.m_hasReportLineIndex = true;
                    }

                    lineIndex++;
                }
            }

            *reinterpret_cast<uint64_t*>(msg.m_targetCharacterGUID) = target.m_guid;
            msg.m_complaintType = complaintType;

            if (lua_isstring(L, 3)) {
                SStrCopy(msg.m_note, lua_tolstring(L, 3, NULL), sizeof(msg.m_note));
            } else {
                msg.m_note[0] = 0;
            }

            ClientServices::Send(&msg);
            submitted = true;
        }
    }

    if (hasLine) {
        if (!complaintType) {
            ComplainChat(NULL, NULL, lineID);
            submitted = true;
        }
    }

    if (!complaintType || complaintType == 10) {
        if (FriendList::s_friendList) {
            FriendList::s_friendList->AddComplained(target);
        }

        char guidString[20];
        FrameScript_SignalEvent(SCRIPT_EVENT_PLAYER_REPORT_SUBMITTED, "%s", GUIDToString(target, guidString));
    }

    if (submitted) {
        CGGameUI::DisplayError(ERR_REPORT_SUBMITTED_SUCCESSFULLY);
        CGChat::AddChatMessage(FrameScript_GetText("COMPLAINT_ADDED", -1, GENDER_NOT_APPLICABLE), CHAT_MSG_SYSTEM, NULL);
        FrameScript_SignalEvent(SCRIPT_EVENT_QUICK_TICKET_THROTTLE_CHANGED, NULL);
        return 0;
    }

    CGGameUI::DisplayError(ERR_REPORT_SUBMISSION_FAILED);
    CGChat::AddChatMessage(FrameScript_GetText("ERR_REPORT_SUBMISSION_FAILED", -1, GENDER_NOT_APPLICABLE), CHAT_MSG_SYSTEM, NULL);
    return 0;
}

// STATIC: 0x00D101F8
static FrameScript_Method s_ScriptFunctions[] = {
    { "SendChatMessage", &Script_SendChatMessage },
    { "IsChatAFK", &Script_IsChatAFK },
    { "IsChatDND", &Script_IsChatDND },
    { "RegisterAddonMessagePrefix", &Script_RegisterAddonMessagePrefix },
    { "IsAddonMessagePrefixRegistered", &Script_IsAddonMessagePrefixRegistered },
    { "GetRegisteredAddonMessagePrefixes", &Script_GetRegisteredAddonMessagePrefixes },
    { "SendAddonMessage", &Script_SendAddonMessage },
    { "SendSystemMessage", &Script_SendSystemMessage },
    { "GetNumLanguages", &Script_GetNumLanguages },
    { "GetLanguageByIndex", &Script_GetLanguageByIndex },
    { "GetDefaultLanguage", &Script_GetDefaultLanguage },
    { "DoEmote", &Script_DoEmote },
    { "CancelEmote", &Script_CancelEmote },
    { "LoggingChat", &Script_LoggingChat },
    { "LoggingCombat", &Script_LoggingCombat },
    { "JoinChannelByName", &Script_JoinChannelByName },
    { "JoinTemporaryChannel", &Script_JoinTemporaryChannel },
    { "JoinPermanentChannel", &Script_JoinPermanentChannel },
    { "LeaveChannelByName", &Script_LeaveChannelByName },
    { "ListChannelByName", &Script_ListChannelByName },
    { "ListChannels", &Script_ListChannels },
    { "GetChannelList", &Script_GetChannelList },
    { "SetChannelPassword", &Script_SetChannelPassword },
    { "SetChannelOwner", &Script_SetChannelOwner },
    { "DisplayChannelOwner", &Script_DisplayChannelOwner },
    { "GetChannelName", &Script_GetChannelName },
    { "ChannelModerator", &Script_ChannelModerator },
    { "ChannelUnmoderator", &Script_ChannelUnmoderator },
    { "ChannelMute", &Script_ChannelMute },
    { "ChannelUnmute", &Script_ChannelUnmute },
    { "ChannelInvite", &Script_ChannelInvite },
    { "ChannelKick", &Script_ChannelKick },
    { "ChannelBan", &Script_ChannelBan },
    { "ChannelUnban", &Script_ChannelUnban },
    { "ChannelToggleAnnouncements", &Script_ChannelToggleAnnouncements },
    { "ChannelSilenceVoice", &Script_ChannelSilenceVoice },
    { "ChannelSilenceAll", &Script_ChannelSilenceAll },
    { "ChannelUnSilenceVoice", &Script_ChannelUnSilenceVoice },
    { "ChannelUnSilenceAll", &Script_ChannelUnSilenceAll },
    { "ChangeChatColor", &Script_ChangeChatColor },
    { "ResetChatColors", &Script_ResetChatColors },
    { "GetChatTypeIndex", &Script_GetChatTypeIndex },
    { "GetChatWindowInfo", &Script_GetChatWindowInfo },
    { "GetChatWindowSavedPosition", &Script_GetChatWindowSavedPosition },
    { "GetChatWindowSavedDimensions", &Script_GetChatWindowSavedDimensions },
    { "GetChatWindowMessages", &Script_GetChatWindowMessages },
    { "GetChatWindowChannels", &Script_GetChatWindowChannels },
    { "AddChatWindowMessages", &Script_AddChatWindowMessages },
    { "RemoveChatWindowMessages", &Script_RemoveChatWindowMessages },
    { "AddChatWindowChannel", &Script_AddChatWindowChannel },
    { "RemoveChatWindowChannel", &Script_RemoveChatWindowChannel },
    { "SetChatWindowName", &Script_SetChatWindowName },
    { "SetChatWindowSize", &Script_SetChatWindowSize },
    { "SetChatWindowColor", &Script_SetChatWindowColor },
    { "SetChatWindowAlpha", &Script_SetChatWindowAlpha },
    { "SetChatWindowLocked", &Script_SetChatWindowLocked },
    { "SetChatWindowDocked", &Script_SetChatWindowDocked },
    { "SetChatWindowUninteractable", &Script_SetChatWindowUninteractable },
    { "SetChatWindowShown", &Script_SetChatWindowShown },
    { "SetChatWindowSavedPosition", &Script_SetChatWindowSavedPosition },
    { "SetChatWindowSavedDimensions", &Script_SetChatWindowSavedDimensions },
    { "EnumerateServerChannels", &Script_EnumerateServerChannels },
    { "ResetChatWindows", &Script_ResetChatWindows },
    { "CanComplainChat", &Script_CanComplainChat },
    { "ReportPlayer", &Script_ReportPlayer },
    { "SetPendingReportTarget", &Script_SetPendingReportTarget },
    { "GetNumDisplayChannels", &Script_GetNumDisplayChannels },
    { "GetChannelDisplayInfo", &Script_GetChannelDisplayInfo },
    { "GetSelectedDisplayChannel", &Script_GetSelectedDisplayChannel },
    { "SetSelectedDisplayChannel", &Script_SetSelectedDisplayChannel },
    { "GetChannelRosterInfo", &Script_GetChannelRosterInfo },
    { "GetNumChannelMembers", &Script_GetNumChannelMembers },
    { "SetActiveVoiceChannel", &Script_SetActiveVoiceChannel },
    { "GetActiveVoiceChannel", &Script_GetActiveVoiceChannel },
    { "CollapseChannelHeader", &Script_CollapseChannelHeader },
    { "ExpandChannelHeader", &Script_ExpandChannelHeader },
    { "ChannelVoiceOn", &Script_ChannelVoiceOn },
    { "ChannelVoiceOff", &Script_ChannelVoiceOff },
    { "DisplayChannelVoiceOn", &Script_DisplayChannelVoiceOn },
    { "DisplayChannelVoiceOff", &Script_DisplayChannelVoiceOff },
    { "IsDisplayChannelOwner", &Script_IsDisplayChannelOwner },
    { "IsDisplayChannelModerator", &Script_IsDisplayChannelModerator },
    { "IsVoiceChatEnabled", &Script_IsVoiceChatEnabled },
    { "IsVoiceChatAllowed", &Script_IsVoiceChatAllowed },
    { "IsVoiceChatAllowedByServer", &Script_IsVoiceChatAllowedByServer },
    { "IsSilenced", &Script_IsSilenced },
    { "GetMuteStatus", &Script_GetMuteStatus },
    { "UnitIsSilenced", &Script_UnitIsSilenced },
    { "SetChannelWatch", &Script_SetChannelWatch },
    { "ClearChannelWatch", &Script_ClearChannelWatch },
    { "DeclineInvite", &Script_DeclineInvite },
    { "SetChatColorNameByClass", &Script_SetChatColorNameByClass },
};

// OFFSET: 0x008E7730/0x140615AA0
void ChatRegisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_RegisterFunction(s_ScriptFunctions[i].m_name, s_ScriptFunctions[i].m_method);
    }
}

// OFFSET: 0x008E7760/0x140615AF0
void ChatUnregisterScriptFunctions() {
    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_UnregisterFunction(s_ScriptFunctions[i].m_name);
    }
}

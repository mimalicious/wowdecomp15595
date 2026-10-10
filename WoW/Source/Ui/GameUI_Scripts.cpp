#include "GameUI.h"
#include "WoW/Source/Ui/BlobFrame.h"
#include "WoW/Source/Ui/ArchaeologyDigSiteFrame.h"
#include "WoW/Source/JamClientAutoCode/JamUserClient.h"
#include <cmath>
#include <cstdarg>
#include <cstring>
#include "Engine/Source/Base/CDataStore.h"
#include "Engine/Source/Base/Coordinate.h"
#include "Engine/Source/BlizzardCore/SFile2-Core.h"
#include "Engine/Source/BlizzardCore/Source/Packages/Streaming/Streaming.h"
#include "Engine/Source/ConsoleBase/ConsoleVar.h"
#include "Engine/Source/ConsoleBase/IConsole.h"
#include "Engine/Source/Event/CMouseEvent.h"
#include "Engine/Source/Event/CSizeEvent.h"
#include "Engine/Source/Event/Event.h"
#include "Engine/Source/Frame/CScriptObject.h"
#include "Engine/Source/Frame/CSimpleButton.h"
#include "Engine/Source/Frame/CSimpleFrame.h"
#include "Engine/Source/Frame/CSimpleTexture.h"
#include "Engine/Source/Frame/CSimpleTop.h"
#include "Engine/Source/Frame/SimpleFrameScript.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameScript/FrameScript_Object.h"
#include "Engine/Source/FrameXML/FrameXML.h"
#include "Engine/Source/FrameXML/LoadXML.h"
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"
#include "Engine/Source/lua-5.1/src/lua.h"
#include "Engine/Source/Os/OsFile.h"
#include "Engine/Source/Os/Time.h"
#include "Engine/Source/Os/TimeManager.h"
#include "Engine/Source/Scrn/Scrn.h"
#include "Engine/Source/ShaderEffect/ShadowCache.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/Storm/Source/MD5.h"
#include "Engine/Source/Tempest/CImVector.h"
#include "Engine/Source/Tempest/CRect.h"
#include "WoW/Common/ComponentCore/CharacterComponent.h"
#include "WoW/Source/Client.h"
#include "WoW/Source/Console/ConsoleClient.h"
#include "WoW/Source/ConsoleDevice/ConsoleDevice.h"
#include "WoW/Source/DB/DBClient/DBCacheInstances.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/AreaTableRec.h"
#include "WoW/Source/DB/Rec/CharTitlesRec.h"
#include "WoW/Source/DB/Rec/ChrClassesRec.h"
#include "WoW/Source/DB/Rec/ExhaustionRec.h"
#include "WoW/Source/DB/Rec/ItemClassRec.h"
#include "WoW/Source/DB/Rec/ItemSubClassRec.h"
#include "WoW/Source/DB/Rec/MapDifficultyRec.h"
#include "WoW/Source/DB/Rec/MapRec.h"
#include "WoW/Source/DB/Rec/SpellRec.h"
#include "WoW/Source/Game/GameClient/ComSatClient.h"
#include "WoW/Source/Game/GameClient/PlayerName.h"
#include "WoW/Source/Glue/CGlueMgr.h"
#include "WoW/Source/Glue/CWOWClientStatus.h"
#include "WoW/Source/LoadingScreen.h"
#include "WoW/Source/Magic/MagicClient/Spell_C.h"
#include "WoW/Source/Net/NetClient/NetClient.h"
#include "WoW/Source/Net/Types.h"
#include "WoW/Source/Object/ItemStats.h"
#include "WoW/Source/Object/Movement.h"
#include "WoW/Source/Object/ObjectClient/Bag_C.h"
#include "WoW/Source/Object/ObjectClient/Item_C.h"
#include "WoW/Source/Object/ObjectClient/Object_C.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/Object/ObjectClient/SimpleScript.h"
#include "WoW/Source/Object/ObjectClient/Trade_C.h"
#include "WoW/Source/Object/ObjectClient/Unit_C.h"
#include "WoW/Source/Object/ObjectClient/UnitCombatLog_C.h"
#include "WoW/Source/Object/ObjectClient/Vehicle_C.h"
#include "WoW/Source/Object/ObjectClient/VehiclePassenger_C.h"
#include "WoW/Source/Object/Player.h"
#include "WoW/Source/Object/Types.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/SoundInterface/SI3.h"
#include "WoW/Source/Ui/AchievementInfo.h"
#include "WoW/Source/Ui/ActionBarFrame.h"
#include "WoW/Source/Ui/AddOns.h"
#include "WoW/Source/Ui/ArenaTeamInfo.h"
#include "WoW/Source/Ui/AuctionHouse.h"
#include "WoW/Source/Ui/BarberShop.h"
#include "WoW/Source/Ui/BattlefieldInfo.h"
#include "WoW/Source/Ui/BattlenetUI.h"
#include "WoW/Source/Ui/Calendar.h"
#include "WoW/Source/Ui/CGAutoCompleteName.h"
#include "WoW/Source/Ui/CGBankInfo.h"
#include "WoW/Source/Ui/CGCooldown.h"
#include "WoW/Source/Ui/CGMinigameInfo.h"
#include "WoW/Source/Ui/CGQuestPOIFrame.h"
#include "WoW/Source/Ui/CGTabardCreationFrame.h"
#include "WoW/Source/Ui/CGWorldStateInfo.h"
#include "WoW/Source/Ui/CharacterModelBase.h"
#include "WoW/Source/Ui/ChatBubbleFrame.h"
#include "WoW/Source/Ui/ChatFrame.h"
#include "WoW/Source/Ui/ClassTrainerFrame.h"
#include "WoW/Source/Ui/CommentatorFrame.h"
#include "WoW/Source/Ui/ContainerFrame.h"
#include "WoW/Source/Ui/CurrencyTypes.h"
#include "WoW/Source/Ui/DanceStudio.h"
#include "WoW/Source/Ui/DressUpModelFrame.h"
#include "WoW/Source/Ui/DuelInfo.h"
#include "WoW/Source/Ui/EquipmentManager.h"
#include "WoW/Source/Ui/GameUI.h"
#include "WoW/Source/Ui/Flyout.h"
#include "WoW/Source/Ui/GlyphInfo.h"
#include "WoW/Source/Ui/GMSurveyInfo.h"
#include "WoW/Source/Ui/GMTicketInfo.h"
#include "WoW/Source/Ui/GossipInfo.h"
#include "WoW/Source/Ui/GuildBankFrame.h"
#include "WoW/Source/Ui/GuildInfo.h"
#include "WoW/Source/Ui/GuildRegistrar.h"
#include "WoW/Source/Ui/InstanceEncounter_C.h"
#include "WoW/Source/Ui/ItemSocketInfo.h"
#include "WoW/Source/Ui/ItemTextFrame.h"
#include "WoW/Source/Ui/KnowledgeBase.h"
#include "WoW/Source/Ui/LFGInfo.h"
#include "WoW/Source/Ui/LootFrame.h"
#include "WoW/Source/Ui/MailInfo.h"
#include "WoW/Source/Ui/MerchantFrame.h"
#include "WoW/Source/Ui/MinimapFrame.h"
#include "WoW/Source/Ui/PaperDollInfoFrame.h"
#include "WoW/Source/Ui/PartyFrame.h"
#include "WoW/Source/Ui/PetInfo.h"
#include "WoW/Source/Ui/PetitionInfo.h"
#include "WoW/Source/Ui/PetitionVendor.h"
#include "WoW/Source/Ui/QuestFrame.h"
#include "WoW/Source/Ui/QuestLog.h"
#include "WoW/Source/Ui/RaidInfo.h"
#include "WoW/Source/Ui/ReputationInfo.h"
#include "WoW/Source/Ui/RuneInfo.h"
#include "WoW/Source/Ui/Screen.h"
#include "WoW/Source/Ui/ScriptEvents.h"
#include "WoW/Source/Ui/SkillInfo.h"
#include "WoW/Source/Ui/SpellBookFrame.h"
#include "WoW/Source/Ui/StableInfo.h"
#include "WoW/Source/Ui/TabardModelFrame.h"
#include "WoW/Source/Ui/TalentInfo.h"
#include "WoW/Source/Ui/TaxiMapFrame.h"
#include "WoW/Source/Ui/TotemInfo.h"
#include "WoW/Source/Ui/TradeFrame.h"
#include "WoW/Source/Ui/TradeSkillFrame.h"
#include "WoW/Source/Ui/Tutorial.h"
#include "WoW/Source/Ui/Types.h"
#include "WoW/Source/Ui/UIBindings.h"
#include "WoW/Source/Ui/UILCD/LCD.h"
#include "WoW/Source/Ui/UIMacros.h"
#include "WoW/Source/Ui/VideoOptions.h"
#include "WoW/Source/Ui/WorldFrame.h"
#include "WoW/Source/Ui/WorldMap.h"
#include "WoW/Source/UIUtil/Camera.h"
#include "WoW/Source/UIUtil/Cursor.h"
#include "WoW/Source/UIUtil/InputControl.h"
#include "WoW/Source/UIUtil/PortraitButton.h"
#include "WoW/Source/UIUtil/Tooltip.h"
#include "WoW/Source/WorldClient/World.h"
#include "WoW/Source/WowServices/RealmConnection.h"
#include "WoW/Source/WowServices/WDataStore.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/AccountData.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/FriendList.h"
#include <cctype>
#include "WoW/Source/DB/Rec/GMTicketCategoryRec.h"
#include "WoW/Source/DB/Rec/ItemLimitCategoryRec.h"
#include "WoW/Source/DB/Rec/ItemRec.h"
#include "WoW/Source/DB/Rec/SpellItemEnchantmentRec.h"
#include "WoW/Source/Game/ValidateName.h"
#include <cfloat>
#include "Engine/Source/Tempest/CMath.h"
#include "WoW/Source/DB/Rec/PaperDollItemFrameRec.h"
#include "WoW/Source/Object/WGUID.h"

// STATIC: #0x00BD0CC4
static CWOWClientStatus* s_loadAddOnStatus;

// STATIC: #0x00BD0CC8
static int32_t s_loadAddOnDepth;

// STATIC: #0x00AC8074
static const char* s_instanceTypeTokens[5] = { "none", "party", "raid", "pvp", "arena" };

// STATIC: #0x00ACC600
static int32_t s_setTitleCount = 10;

// STATIC: #0x00BD0CB8
static float s_setTitleTime;

// OFFSET: 0x008C25A0/0x140631DB0
static int32_t Script_BeginTrade(lua_State* L) {
    Trade_C_BeginTrade();
    return 0;
}

// OFFSET: 0x008C25B0/0x140631DC0
static int32_t Script_CancelTrade(lua_State* L) {
    Trade_C_CancelTrade();
    return 0;
}

// OFFSET: 0x008C25D0/0x140631DF0
static int32_t Script_ForceQuit(lua_State* L) {
    ClientPostClose(0);
    return 0;
}

// OFFSET: 0x00481710/0x1400A0300
static int32_t Script_GetTimeToWellRested(lua_State* L) {
    lua_pushnil(L);
    return 1;
}

// OFFSET: #0x00510DC0
int32_t Script_RestoreVideoResolutionDefaults(lua_State* L) {
    ConsoleDeviceDefaults(OPTION_PAGE_0);
    return 0;
}

// OFFSET: #0x00510DD0
int32_t Script_RestoreVideoEffectsDefaults(lua_State* L) {
    ConsoleDeviceDefaults(OPTION_PAGE_1);
    return 0;
}

// OFFSET: 0x008C30B0/0x140632B20
static int32_t Script_ResetCPUUsage(lua_State* L) {
    FrameScript_ResetCPUUsage();
    return 0;
}

// OFFSET: 0x008C6FF0/0x140636F60
static int32_t Script_TargetDirectionFinished(lua_State* L) {
    CGGameUI::m_directionalTarget = WOWGUID();
    return 0;
}

// OFFSET: 0x008C9090/0x140639290
static int32_t Script_DeclineLevelGrant(lua_State* L) {
    CGGameUI::m_pendingLevelGranter = WOWGUID();
    return 0;
}

// OFFSET: 0x008D4560/0x14063F2B0
static int32_t Script_NotWhileDeadError(lua_State* L) {
    CGGameUI::DisplayError(ERR_PLAYER_DEAD);
    return 0;
}

// PROVEN: dc2798fd IDENTICAL/NONE
// OFFSET: 0x00807310/0x140631100
static int32_t Script_IsDebugBuild(lua_State* L) {
    lua_pushboolean(L, 0);
    return 1;
}

// OFFSET: 0x008C27A0/0x140632190
static int32_t Script_SetEuropeanNumbers(lua_State* L) {
    SetDecimalConversion(StringToBOOL(L, 1, 1));
    return 0;
}

// OFFSET: 0x008C27C0/0x140632220
static int32_t Script_GetRealmName(lua_State* L) {
    lua_pushstring(L, ClientServices::GetSelectedRealmName());
    return 1;
}

// OFFSET: 0x008C2DA0/0x140632830
static int32_t Script_UpdateAddOnMemoryUsage(lua_State* L) {
    lua_gcupdatesizes(L);
    return 0;
}

// OFFSET: 0x008C2EC0/0x140632940
static int32_t Script_UpdateAddOnCPUUsage(lua_State* L) {
    lua_gcupdatetimes(L);
    return 0;
}

// OFFSET: 0x008C32C0/0x140632D50
static int32_t Script_ResetDisabledAddOns(lua_State* L) {
    LoadAddOnEnableState(ClientServices::GetCharacterName());
    return 0;
}

// OFFSET: 0x008CC380/0x14063D2A0
static int32_t Script_ClearCursor(lua_State* L) {
    CGGameUI::ClearCursor(1, 1);
    return 1;
}

// OFFSET: 0x008C2E90/0x140632910
static int32_t Script_GetScriptCPUUsage(lua_State* L) {
    lua_pushnumber(L, FrameScript_GetScriptCPUUsage());
    return 1;
}

// OFFSET: 0x008C37D0/0x1406332E0
static int32_t Script_IsDesaturateSupported(lua_State* L) {
    if (!CSimpleTexture::GetImageModePixelShader(ImageMode_Desaturate)) {
        return 0;
    }

    lua_pushnumber(L, 1.0);
    return 1;
}

// OFFSET: 0x008C7000/0x140631C40
static int32_t Script_GetZoneText(lua_State* L) {
    lua_pushstring(L, CGGameUI::m_zoneText ? CGGameUI::m_zoneText : "");
    return 1;
}

// OFFSET: 0x008C7030/0x140636F70
static int32_t Script_GetRealZoneText(lua_State* L) {
    lua_pushstring(L, CGGameUI::m_realZoneText ? CGGameUI::m_realZoneText : "");
    return 1;
}

// OFFSET: 0x008CC980/0x14063DEC0
static int32_t Script_RepopMe(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player) {
        player->HandleRepopRequest(false);
    }

    return 0;
}

// OFFSET: 0x008CC9B0/0x14063DF00
static int32_t Script_AcceptResurrect(lua_State* L) {
    if (ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__)) {
        CGPlayer_C::AcceptResurrectRequest(1);
    }

    return 0;
}

// OFFSET: 0x008CC9E0/0x14063DF40
static int32_t Script_DeclineResurrect(lua_State* L) {
    if (ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__)) {
        CGPlayer_C::AcceptResurrectRequest(0);
    }

    return 0;
}

// OFFSET: 0x008CCA10/0x14063DF80
static int32_t Script_AcceptGuild(lua_State* L) {
    if (ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__)) {
        CGPlayer_C::AcceptGuild();
    }

    return 0;
}

// OFFSET: 0x008CCA40/0x14063DFC0
static int32_t Script_DeclineGuild(lua_State* L) {
    if (ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__)) {
        CGPlayer_C::DeclineGuild();
    }

    return 0;
}

// OFFSET: 0x008CCA70/0x14063E000
static int32_t Script_AcceptArenaTeam(lua_State* L) {
    if (ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__)) {
        CGPlayer_C::AcceptArenaTeam();
    }

    return 0;
}

// OFFSET: 0x008CCAA0/0x14063E040
static int32_t Script_DeclineArenaTeam(lua_State* L) {
    if (ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__)) {
        CGPlayer_C::DeclineArenaTeam();
    }

    return 0;
}

// OFFSET: 0x008D5E00/0x14064A040
static int32_t Script_DropCursorMoney(lua_State* L) {
    if (CGGameUI::m_cursorMoney && CGGameUI::m_hasControl && CGGameUI::m_cursorItemType == 2) {
        CGGameUI::ClearCursor(1, 1);
    }

    return 0;
}

// OFFSET: 0x00807AB0/0x14051D330
int32_t Script_GetBillingTimeRested(lua_State* L) {
    lua_pushnumber(L, ClientServices::Connection()->m_billingTimeRested);
    return 1;
}

// OFFSET: 0x008C6ED0/0x140636F30
static int32_t Script_Screenshot(lua_State* L) {
    ScrnScreenshot(&CGGameUI::HandleScreenshot, reinterpret_cast<uint8_t*>(ClientServices::GetClientStamp()), 88, NULL, NULL);
    return 0;
}

// OFFSET: 0x008C6EF0/0x140631B40
static int32_t Script_CursorHasItem(lua_State* L) {
    if (CGGameUI::m_cursorItemType == 1 || CGGameUI::m_cursorItemType == 9) {
        lua_pushnumber(L, 1.0);
        return 1;
    }

    lua_pushnil(L);
    return 1;
}

// OFFSET: 0x008C6F30/0x140631B80
static int32_t Script_CursorHasSpell(lua_State* L) {
    if (CGGameUI::m_cursorItemType == 3) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: 0x008C6F70/0x140631BC0
static int32_t Script_CursorHasMacro(lua_State* L) {
    if (CGGameUI::m_cursorItemType == 8) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: 0x008C6FB0/0x140631C00
static int32_t Script_CursorHasMoney(lua_State* L) {
    if (CGGameUI::m_cursorItemType == 2) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: 0x008C7250/0x140637150
static int32_t Script_GetInstanceDifficulty(lua_State* L) {
    lua_pushnumber(L, CGGameUI::m_instanceDifficulty + 1);
    return 1;
}

// OFFSET: 0x008C8200/0x1406320A0
static int32_t Script_InCinematic(lua_State* L) {
    if (CGGameUI::m_inCinematic) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: 0x008C90A0/0x1406333D0
static int32_t Script_IsThreatWarningEnabled(lua_State* L) {
    if (CGGameUI::m_threatWarningEnabled) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: 0x008C38B0/0x140633410
static int32_t Script_ConsoleAddMessage(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: ConsoleAddMessage(string)");
    }

    const char* text = lua_tolstring(L, 1, NULL);

    if (text && *text) {
        ConsolePrintf("%s", text);
    }

    return 0;
}

// OFFSET: 0x008C16B0/0x140630BF0
static int32_t Script_FrameXML_Debug(lua_State* L) {
    int32_t level = FrameXML_GetDebugLevel();

    if (lua_isnumber(L, 1)) {
        level = static_cast<int32_t>(lua_tonumber(L, 1));
        FrameXML_SetDebugLevel(level);
    }

    lua_pushnumber(L, level);
    return 1;
}

// OFFSET: 0x008C66A0/0x140636700
static int32_t Script_IsLeftShiftKeyDown(lua_State* L) {
    int32_t down;

    if (CGUIBindings::s_bindings->m_keyStateOverride) {
        down = CGUIBindings::s_bindings->m_heldModifiers & 1;
    } else {
        down = EventIsKeyDown(KEY_LSHIFT);
    }

    if (down) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: 0x008C6700/0x140636760
static int32_t Script_IsRightShiftKeyDown(lua_State* L) {
    int32_t down;

    if (CGUIBindings::s_bindings->m_keyStateOverride) {
        down = (CGUIBindings::s_bindings->m_heldModifiers >> 1) & 1;
    } else {
        down = EventIsKeyDown(KEY_RSHIFT);
    }

    if (down) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: 0x008C67F0/0x140636850
static int32_t Script_IsLeftControlKeyDown(lua_State* L) {
    int32_t down;

    if (CGUIBindings::s_bindings->m_keyStateOverride) {
        down = (CGUIBindings::s_bindings->m_heldModifiers >> 2) & 1;
    } else {
        down = EventIsKeyDown(KEY_LCONTROL);
    }

    if (down) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: 0x008C6850/0x1406368B0
static int32_t Script_IsRightControlKeyDown(lua_State* L) {
    int32_t down;

    if (CGUIBindings::s_bindings->m_keyStateOverride) {
        down = (CGUIBindings::s_bindings->m_heldModifiers >> 3) & 1;
    } else {
        down = EventIsKeyDown(KEY_RCONTROL);
    }

    if (down) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: 0x008C6940/0x1406369A0
static int32_t Script_IsLeftAltKeyDown(lua_State* L) {
    int32_t down;

    if (CGUIBindings::s_bindings->m_keyStateOverride) {
        down = (CGUIBindings::s_bindings->m_heldModifiers >> 4) & 1;
    } else {
        down = EventIsKeyDown(KEY_LALT);
    }

    if (down) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: 0x008C69A0/0x140636A00
static int32_t Script_IsRightAltKeyDown(lua_State* L) {
    int32_t down;

    if (CGUIBindings::s_bindings->m_keyStateOverride) {
        down = (CGUIBindings::s_bindings->m_heldModifiers >> 5) & 1;
    } else {
        down = EventIsKeyDown(KEY_RALT);
    }

    if (down) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: 0x008C7C50/0x140637C30
static int32_t Script_GetCorpseRecoveryDelay(lua_State* L) {
    int32_t remaining;

    if (!CGGameUI::m_corpseReclaimDelay || (remaining = CGGameUI::m_corpseReclaimDelay - OsGetAsyncTimeMs()) <= 0) {
        remaining = 0;
    }

    lua_pushnumber(L, static_cast<uint32_t>(remaining) / 1000);
    return 1;
}

// OFFSET: 0x008C7CB0/0x140637C80
static int32_t Script_GetInstanceBootTimeRemaining(lua_State* L) {
    int32_t remaining;

    if (!CGGameUI::m_instanceBootTime || (remaining = CGGameUI::m_instanceBootTime - OsGetAsyncTimeMs()) <= 0) {
        remaining = 0;
    }

    lua_pushnumber(L, static_cast<uint32_t>(remaining) / 1000);
    return 1;
}

// OFFSET: 0x008C7E50/0x140637E20
static int32_t Script_GetSummonConfirmTimeLeft(lua_State* L) {
    int32_t remaining;

    if (!CGGameUI::m_summonConfirmTime || (remaining = CGGameUI::m_summonConfirmTime - OsGetAsyncTimeMs()) <= 0) {
        remaining = 0;
    }

    lua_pushnumber(L, static_cast<uint32_t>(remaining) / 1000);
    return 1;
}

// OFFSET: 0x008C7F20/0x140637E70
static int32_t Script_GetSummonConfirmAreaName(lua_State* L) {
    const AreaTableRec* area = g_areaTableDB.GetRecord(CGGameUI::m_summonConfirmArea);

    if (area) {
        lua_pushstring(L, area->m_areaName);
        return 1;
    }

    lua_pushstring(L, "");
    return 1;
}

// OFFSET: 0x008C8440/0x140638510
static int32_t Script_IsSubZonePVPPOI(lua_State* L) {
    const AreaTableRec* area = g_areaTableDB.GetRecord(CGGameUI::m_subzoneID);

    if (area && (area->m_flags & 0x8000)) {
        lua_pushnumber(L, 1.0);
        return 1;
    }

    lua_pushnil(L);
    return 1;
}

// OFFSET: 0x008C87E0/0x140638970
static int32_t Script_GetAreaSpiritHealerTime(lua_State* L) {
    int32_t remaining = 0;

    if (CGGameUI::m_areaSpiritHealerTime) {
        remaining = CGGameUI::m_areaSpiritHealerTime - OsGetAsyncTimeMs();

        if (remaining < 0) {
            remaining = 0;
        }
    }

    lua_pushnumber(L, remaining / 1000);
    return 1;
}

// OFFSET: 0x008CD450/0x14063F0A0
static int32_t Script_CheckSpiritHealerDist(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player && player->CheckSpiritHealerDistance()) {
        lua_pushnumber(L, 1.0);
        return 1;
    }

    lua_pushnil(L);
    return 1;
}

// OFFSET: 0x008CD4B0/0x14063F110
static int32_t Script_CheckTalentMasterDist(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player && player->CheckTalentMasterDistance()) {
        lua_pushnumber(L, 1.0);
        return 1;
    }

    lua_pushnil(L);
    return 1;
}

// OFFSET: 0x008CD510/0x14063F180
static int32_t Script_CheckBinderDist(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player && player->CheckBinderDistance()) {
        lua_pushnumber(L, 1.0);
        return 1;
    }

    lua_pushnil(L);
    return 1;
}

// OFFSET: 0x008CDA30/0x14063F7B0
static int32_t Script_GetPVPTimer(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));
    int32_t timer = player ? player->GetPVPToggleTimer() + 1000 : 0;
    lua_pushnumber(L, timer);
    return 1;
}

// OFFSET: #0x0054C810
int32_t Script_CanHearthAndResurrectFromArea(lua_State* L) {
    const AreaTableRec* area = g_areaTableDB.GetRecord(CGGameUI::m_areaID);

    if (area && (area->m_flags & 0x8000000)) {
        lua_pushnumber(L, 1.0);
        return 1;
    }

    lua_pushnil(L);
    return 1;
}

// OFFSET: 0x008C2610/0x140631F70
static int32_t Script_GetNetStats(lua_State* L) {
    float bytesIn;
    float bytesOut;
    ClientServices::Connection()->GetNetBandwidth(bytesIn, bytesOut);
    lua_pushnumber(L, bytesIn);
    lua_pushnumber(L, bytesOut);

    for (int32_t i = 0; i < 2; i++) {
        uint32_t latency;
        ClientServices::Connection()->GetNetLatency(static_cast<CONNECTION_ID>(i), latency);
        lua_pushnumber(L, static_cast<double>(latency));
    }

    return 4;
}

// OFFSET: 0x008C3040/0x140632AB0
static int32_t Script_GetEventCPUUsage(lua_State* L) {
    double time = 0.0;
    uint32_t count = 0;
    FrameScript_GetEventCPUUsage(lua_tolstring(L, 1, NULL), &time, &count);
    lua_pushnumber(L, time);
    lua_pushnumber(L, count);
    return 2;
}

// OFFSET: 0x008C6D80/0x140636DA0
static int32_t Script_GetMouseButtonName(lua_State* L) {
    if (lua_isnumber(L, 1)) {
        lua_pushstring(L, GetMouseButtonName(IndexToMouseButton(static_cast<uint32_t>(lua_tonumber(L, 1)))));
        return 1;
    }

    if (lua_isstring(L, 1)) {
        lua_settop(L, 1);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: 0x008C7130/0x140637030
static int32_t Script_ResetInstances(lua_State* L) {
    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_RESET_INSTANCES));
    msg.Finalize();
    ClientServices::Send2(&msg);
    return 0;
}

// OFFSET: 0x008C7350/0x1406371E0
static int32_t Script_GetRaidDifficulty(lua_State* L) {
    uint32_t difficulty = CGPartyInfo::m_raidDifficulty;

    if (!CGRaidInfo::m_numMembers) {
        difficulty = CGGameUI::m_raidDifficulty;
    }

    lua_pushnumber(L, difficulty + 1);
    lua_pushnumber(L, CGGameUI::m_raidDifficulty + 1);
    return 2;
}

// OFFSET: 0x008D43C0/0x14063E870
static int32_t Script_GuildLeave(lua_State* L) {
    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_GUILD_LEAVE));
    msg.Finalize();
    ClientServices::Send2(&msg);
    return 0;
}

// OFFSET: 0x008C25E0/0x140631E40
static int32_t Script_GuildDisband(lua_State* L) {
    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_GUILD_DISBAND));
    msg.Finalize();
    ClientServices::Send2(&msg);
    return 0;
}

// OFFSET: 0x008C7BE0/0x140637BC0
static int32_t Script_GetReleaseTimeRemaining(lua_State* L) {
    int32_t remaining;

    if (CGGameUI::m_deadNoRepopTimer) {
        remaining = -1000;
    } else if (!CGGameUI::m_repopTime || (remaining = CGGameUI::m_repopTime - OsGetAsyncTimeMs()) <= 0) {
        remaining = 0;
    }

    lua_pushnumber(L, remaining / 1000);
    return 1;
}

// OFFSET: 0x008C8190/0x140638190
static int32_t Script_OpeningCinematic(lua_State* L) {
    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_OPENING_CINEMATIC));
    msg.Finalize();
    ClientServices::Send2(&msg);
    return 0;
}

// OFFSET: 0x008C84A0/0x140638570
static int32_t Script_TogglePVP(lua_State* L) {
    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_TOGGLE_PVP));
    msg.Finalize();
    ClientServices::Send2(&msg);
    return 0;
}

// OFFSET: 0x008C8770/0x1406388E0
static int32_t Script_GetBindLocation(lua_State* L) {
    const AreaTableRec* area = g_areaTableDB.GetRecord(CGPlayer_C::GetBindAreaID());

    if (area) {
        lua_pushstring(L, area->m_areaName);
        return 1;
    }

    lua_pushstring(L, FrameScript_GetText("HOME_INN", -1, GENDER_NOT_APPLICABLE));
    return 1;
}

// OFFSET: 0x008CDA90/0x14063F810
static int32_t Script_IsPVPTimerRunning(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player && (player->m_player->m_playerFlags & 0x40000)) {
        lua_pushnumber(L, 1.0);
        return 1;
    }

    lua_pushnil(L);
    return 1;
}

// OFFSET: 0x008CDCF0/0x14063FA90
static int32_t Script_ShowingHelm(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player && !(player->m_player->m_playerFlags & 0x400)) {
        lua_pushnumber(L, 1.0);
        return 1;
    }

    lua_pushnil(L);
    return 1;
}

// OFFSET: 0x008CDD60/0x14063FB00
static int32_t Script_ShowingCloak(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player && !(player->m_player->m_playerFlags & 0x800)) {
        lua_pushnumber(L, 1.0);
        return 1;
    }

    lua_pushnil(L);
    return 1;
}

// OFFSET: 0x008CE830/0x140640790
static int32_t Script_GetTaxiBenchmarkMode(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player && (player->m_player->m_playerFlags & 0x20000)) {
        lua_pushnumber(L, 1.0);
        return 1;
    }

    lua_pushnil(L);
    return 1;
}

// OFFSET: 0x008CEC80/0x140640F80
static int32_t Script_GetAllowLowLevelRaid(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player && (player->m_player->m_playerFlags & 0x10000)) {
        lua_pushnumber(L, 1.0);
        return 1;
    }

    lua_pushnil(L);
    return 1;
}

// OFFSET: 0x008CE240/0x140640080
static int32_t Script_PartialPlayTime(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));
    int32_t partial;

    if (player && (partial = (player->m_player->m_playerFlags & 0x1000) != 0)) {
        lua_pushnumber(L, partial);
        return 1;
    }

    lua_pushnil(L);
    return 1;
}

// OFFSET: 0x008CE2B0/0x1406400F0
static int32_t Script_NoPlayTime(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));
    int32_t none;

    if (player && (none = (player->m_player->m_playerFlags & 0x2000) != 0)) {
        lua_pushnumber(L, none);
        return 1;
    }

    lua_pushnil(L);
    return 1;
}

// OFFSET: 0x008CD9D0/0x14063F750
static int32_t Script_GetPVPDesired(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));
    uint32_t desired = player ? (player->m_player->m_playerFlags >> 9) & 1 : 0;
    lua_pushnumber(L, desired);
    return 1;
}

// OFFSET: 0x008C72D0/0x140637180
static int32_t Script_GetDungeonDifficulty(lua_State* L) {
    uint32_t difficulty;

    if (!CGPartyInfo::m_members[0] || (difficulty = CGPartyInfo::m_dungeonDifficulty, CGRaidInfo::m_numMembers)) {
        difficulty = CGGameUI::m_dungeonDifficulty;
    }

    lua_pushnumber(L, difficulty + 1);
    lua_pushnumber(L, CGGameUI::m_dungeonDifficulty + 1);
    return 2;
}

// OFFSET: 0x008C7450/0x1406372B0
static int32_t Script_RequestTimePlayed(lua_State* L) {
    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_REQUEST_PLAYED_TIME));
    msg.Put(static_cast<uint8_t>(1));
    msg.Finalize();
    ClientServices::Send2(&msg);
    return 0;
}

// OFFSET: 0x008C8EC0/0x1406390D0
static int32_t Script_SetTaxiBenchmarkMode(lua_State* L) {
    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_SET_TAXI_BENCHMARK_MODE));
    msg.Put(static_cast<uint8_t>(StringToBOOL(L, 1, 1) != 0));
    msg.Finalize();
    ClientServices::Send2(&msg);
    return 0;
}

// OFFSET: 0x008C90E0/0x1406392A0
static int32_t Script_CanMapChangeDifficulty(lua_State* L) {
    int32_t mapId;

    if (lua_isnumber(L, 1)) {
        mapId = static_cast<int32_t>(lua_tonumber(L, 1));
    } else {
        mapId = CGGameUI::m_iCurrentMapID;
    }

    const MapRec* map = g_mapDB.GetRecord(mapId);

    if (map && (map->m_flags & 0x100)) {
        lua_pushnumber(L, 1.0);
        return 1;
    }

    lua_pushnil(L);
    return 1;
}

// OFFSET: 0x008D7B30/0x14064A210
static int32_t Script_TargetTotem(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: TargetTotem(slot)");
        return 0;
    }

    uint32_t slot = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1;

    if (slot > 3) {
        luaL_error(L, "Totem slot must be in range of 1 to %d", 4);
        return 0;
    }

    TotemInfo* totem = &CGGameUI::m_totems[slot];

    if (!totem || !totem->m_guid) {
        return 0;
    }

    CGGameUI::Target(totem->m_guid);
    return 0;
}

// OFFSET: 0x008C2FA0/0x140632A00
static int32_t Script_GetFunctionCPUUsage(lua_State* L) {
    if (lua_type(L, 1) != LUA_TFUNCTION) {
        luaL_error(L, "Usage: GetFunctionCPUUsage(function[, includeSubroutines])");
        return 0;
    }

    int32_t includeSubroutines = StringToBOOL(L, 2, 1);
    lua_settop(L, 1);
    double time;
    double totalTime;
    int32_t calls;
    lua_gcgetfunctiontime(L, &time, &totalTime, &calls);
    lua_pushnumber(L, includeSubroutines ? totalTime : time);
    lua_pushnumber(L, static_cast<uint32_t>(calls));
    return 2;
}

// OFFSET: 0x008CD570/0x14063F1F0
static int32_t Script_RetrieveCorpse(lua_State* L) {
    if (ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__)) {
        CGPlayer_C::AcceptResurrectRequest(0);
    }

    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_RECLAIM_CORPSE));
    msg.Put(CGGameUI::m_corpseGUID.m_guid);
    msg.Finalize();
    ClientServices::Send2(&msg);
    return 0;
}

// OFFSET: 0x008C8510/0x1406385F0
static int32_t Script_SetPVP(lua_State* L) {
    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_TOGGLE_PVP));
    uint8_t enable;

    if (lua_isnumber(L, 1)) {
        enable = static_cast<uint8_t>(lua_tonumber(L, 1));
    } else {
        enable = 0;
    }

    msg.Put(enable);
    msg.Finalize();
    ClientServices::Send2(&msg);
    return 0;
}

// OFFSET: 0x008C7F80/0x140637EE0
static int32_t Script_CancelSummon(lua_State* L) {
    WDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_SUMMON_RESPONSE));
    msg.Put(CGGameUI::m_summonConfirmGUID.m_guid);
    msg.Put(static_cast<uint8_t>(0));
    msg.Finalize();
    ClientServices::Send2(&msg);
    return 0;
}

// OFFSET: 0x008D3F20/0x140647610
static int32_t Script_GuildInvite(lua_State* L) {
    const char* target = lua_tolstring(L, 1, NULL);

    if (!target) {
        CGGameUI::DisplayError(ERR_COMMAND_NEEDS_TARGET);
        return 0;
    }

    if (SStrLen(target) > 48) {
        luaL_error(L, "Name too long");
        return 0;
    }

    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_GUILD_INVITE));
    msg.PutString(target);
    msg.Finalize();
    ClientServices::Send2(&msg);
    return 0;
}

// OFFSET: 0x008D4070/0x14063E3E0
static int32_t Script_GuildUninvite(lua_State* L) {
    const char* target = lua_tolstring(L, 1, NULL);

    if (!target) {
        CGGameUI::DisplayError(ERR_COMMAND_NEEDS_TARGET);
        return 0;
    }

    if (SStrLen(target) > 48) {
        luaL_error(L, "Name too long");
        return 0;
    }

    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_GUILD_OFFICER_REMOVE_MEMBER));
    msg.PutString(target);
    msg.Finalize();
    ClientServices::Send2(&msg);
    return 0;
}

// OFFSET: 0x008D4160/0x14063E4E0
static int32_t Script_GuildPromote(lua_State* L) {
    const char* target = lua_tolstring(L, 1, NULL);

    if (!target) {
        CGGameUI::DisplayError(ERR_COMMAND_NEEDS_TARGET);
        return 0;
    }

    if (SStrLen(target) > 48) {
        luaL_error(L, "Name too long");
        return 0;
    }

    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_GUILD_PROMOTE_MEMBER));
    msg.PutString(target);
    msg.Finalize();
    ClientServices::Send2(&msg);
    return 0;
}

// OFFSET: 0x008D4200/0x14063E590
static int32_t Script_GuildDemote(lua_State* L) {
    const char* target = lua_tolstring(L, 1, NULL);

    if (!target) {
        CGGameUI::DisplayError(ERR_COMMAND_NEEDS_TARGET);
        return 0;
    }

    if (SStrLen(target) > 48) {
        luaL_error(L, "Name too long");
        return 0;
    }

    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_GUILD_DEMOTE_MEMBER));
    msg.PutString(target);
    msg.Finalize();
    ClientServices::Send2(&msg);
    return 0;
}

// OFFSET: 0x008D42F0/0x14063E690
static int32_t Script_GuildSetLeader(lua_State* L) {
    const char* target = lua_tolstring(L, 1, NULL);

    if (!target) {
        CGGameUI::DisplayError(ERR_COMMAND_NEEDS_TARGET);
        return 0;
    }

    if (SStrLen(target) > 48) {
        luaL_error(L, "Name too long");
        return 0;
    }

    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_GUILD_LEADER));
    msg.PutString(target);
    msg.Finalize();
    ClientServices::Send2(&msg);
    return 0;
}

// OFFSET: 0x008C80B0/0x140638040
static int32_t Script_RandomRoll(lua_State* L) {
    if (!lua_isstring(L, 1) || !lua_isstring(L, 2)) {
        luaL_error(L, "Usage: RandomRoll(\"max\") or RandomRoll(\"min\", \"max\")");
        return 0;
    }

    int32_t min = SStrToInt(lua_tolstring(L, 1, NULL));
    int32_t max = SStrToInt(lua_tolstring(L, 2, NULL));

    if ((min <= 0 && max <= 0) || max < min || max > 1000000) {
        return 0;
    }

    CDataStore msg;
    msg.Put(static_cast<uint32_t>(MSG_RANDOM_ROLL));
    msg.Put(static_cast<uint32_t>(min));
    msg.Put(static_cast<uint32_t>(max));
    msg.Finalize();
    ClientServices::Send2(&msg);
    return 0;
}

// OFFSET: 0x008C1D60/0x140631210
static int32_t Script_GetCVarInfo(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: GetCVarInfo(\"cvar\")");
        return 0;
    }

    CVar* var = CVar::LookupRegistered(lua_tolstring(L, 1, NULL));

    if (!var || (var->m_flags & 0x40)) {
        lua_pushnil(L);
        lua_pushnil(L);
        lua_pushnil(L);
        lua_pushnil(L);
        return 4;
    }

    lua_pushstring(L, var->m_stringValue.m_str);
    lua_pushstring(L, var->m_defaultValue.m_str);

    if (var->m_flags & 0x10) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    if (var->m_flags & 0x20) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 4;
}

// OFFSET: 0x008CECE0/0x140640FF0
static int32_t Script_SetAllowLowLevelRaid(lua_State* L) {
    if (ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__)) {
        uint8_t allow = StringToBOOL(L, 1, 1) != 0;
        CDataStore msg1;
        msg1.Put(static_cast<uint32_t>(CMSG_LOW_LEVEL_RAID1));
        msg1.Put(allow);
        msg1.Finalize();
        ClientServices::Send2(&msg1);
        CDataStore msg2;
        msg2.Put(static_cast<uint32_t>(CMSG_LOW_LEVEL_RAID2));
        msg2.Put(allow);
        msg2.Finalize();
        ClientServices::Send2(&msg2);
    }

    return 0;
}

// OFFSET: 0x008C6A90/0x140636AF0
static int32_t Script_IsMouseButtonDown(lua_State* L) {
    int32_t down;

    if (lua_isnumber(L, 1)) {
        down = EventIsButtonDown(IndexToMouseButton(static_cast<uint32_t>(lua_tonumber(L, 1))));
    } else if (lua_isstring(L, 1)) {
        down = EventIsButtonDown(StringToMouseButton(lua_tolstring(L, 1, NULL)));
    } else {
        down = EventIsButtonDown(static_cast<MOUSEBUTTON>(0x1)) || EventIsButtonDown(static_cast<MOUSEBUTTON>(0x2)) || EventIsButtonDown(static_cast<MOUSEBUTTON>(0x4)) || EventIsButtonDown(static_cast<MOUSEBUTTON>(0x8)) || EventIsButtonDown(static_cast<MOUSEBUTTON>(0x10)) || EventIsButtonDown(static_cast<MOUSEBUTTON>(0x20)) || EventIsButtonDown(static_cast<MOUSEBUTTON>(0x40)) || EventIsButtonDown(static_cast<MOUSEBUTTON>(0x80)) || EventIsButtonDown(static_cast<MOUSEBUTTON>(0x100)) || EventIsButtonDown(static_cast<MOUSEBUTTON>(0x200)) || EventIsButtonDown(static_cast<MOUSEBUTTON>(0x400)) || EventIsButtonDown(static_cast<MOUSEBUTTON>(0x800)) || EventIsButtonDown(static_cast<MOUSEBUTTON>(0x1000)) || EventIsButtonDown(static_cast<MOUSEBUTTON>(0x2000)) || EventIsButtonDown(static_cast<MOUSEBUTTON>(0x4000)) || EventIsButtonDown(static_cast<MOUSEBUTTON>(0x8000)) || EventIsButtonDown(static_cast<MOUSEBUTTON>(0x10000)) || EventIsButtonDown(static_cast<MOUSEBUTTON>(0x20000)) || EventIsButtonDown(static_cast<MOUSEBUTTON>(0x40000)) || EventIsButtonDown(static_cast<MOUSEBUTTON>(0x80000)) || EventIsButtonDown(static_cast<MOUSEBUTTON>(0x100000)) || EventIsButtonDown(static_cast<MOUSEBUTTON>(0x200000)) || EventIsButtonDown(static_cast<MOUSEBUTTON>(0x400000)) || EventIsButtonDown(static_cast<MOUSEBUTTON>(0x800000)) || EventIsButtonDown(static_cast<MOUSEBUTTON>(0x1000000)) || EventIsButtonDown(static_cast<MOUSEBUTTON>(0x2000000)) || EventIsButtonDown(static_cast<MOUSEBUTTON>(0x4000000)) || EventIsButtonDown(static_cast<MOUSEBUTTON>(0x8000000)) || EventIsButtonDown(static_cast<MOUSEBUTTON>(0x10000000)) || EventIsButtonDown(static_cast<MOUSEBUTTON>(0x20000000)) || EventIsButtonDown(static_cast<MOUSEBUTTON>(0x40000000));
    }

    if (down) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: 0x008C27E0/0x140632250
static int32_t Script_GetItemQualityColor(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetItemQualityColor(index)");
        return 0;
    }

    uint32_t quality = static_cast<int32_t>(lua_tonumber(L, 1));
    const CImVector* color = CGTooltip::GetItemQualityColor(quality);
    lua_pushnumber(L, color->r * 0.00392156862745098);
    lua_pushnumber(L, color->g * 0.00392156862745098);
    lua_pushnumber(L, color->b * 0.00392156862745098);
    lua_pushstring(L, CGTooltip::GetItemQualityColorString(quality));
    return 4;
}

// OFFSET: 0x008C33B0/0x140632E30
static int32_t Script_IsAddOnLoaded(lua_State* L) {
    const char* name;

    if (lua_isnumber(L, 1)) {
        name = GetAddOnByIndex(static_cast<int32_t>(lua_tonumber(L, 1)) - 1);

        if (!name) {
            luaL_error(L, "AddOn index must be in the range of 1 to %d", GetNumAddOns());
            return 0;
        }
    } else {
        if (!lua_isstring(L, 1)) {
            luaL_error(L, "Usage: IsAddOnLoaded(index or \"name\")");
            return 0;
        }

        name = lua_tolstring(L, 1, NULL);
    }

    if (IsAddOnLoading(name)) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    if (IsAddOnLoaded(name)) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 2;
}

// OFFSET: 0x008C7620/0x140631E10
static int32_t Script_GetCursorMoney(lua_State* L) {
    lua_pushnumber(L, CGGameUI::m_cursorMoney);
    return 1;
}

// OFFSET: 0x008C1E50/0x140631310
static int32_t Script_GetCVar(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: GetCVar(\"cvar\")");
        return 0;
    }

    CVar* var = CVar::LookupRegistered(lua_tolstring(L, 1, NULL));

    if (!var || (var->m_flags & 0x40)) {
        lua_pushnil(L);
        return 1;
    }

    lua_pushstring(L, var->m_stringValue.m_str);
    return 1;
}

// OFFSET: 0x008C1ED0/0x140631390
static int32_t Script_GetCVarBool(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: GetCVarBool(\"cvar\")");
        return 0;
    }

    CVar* var = CVar::LookupRegistered(lua_tolstring(L, 1, NULL));

    if (var && !(var->m_flags & 0x40) && StringToBOOL(var->m_stringValue.m_str)) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: 0x008C1F60/0x140631420
static int32_t Script_GetCVarDefault(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: GetCVarDefault(\"cvar\")");
        return 0;
    }

    CVar* var = CVar::LookupRegistered(lua_tolstring(L, 1, NULL));

    if (!var || (var->m_flags & 0x40)) {
        luaL_error(L, "Couldn't find CVar named '%s'", lua_tolstring(L, 1, NULL));
        return 0;
    }

    lua_pushstring(L, var->m_defaultValue.m_str);
    return 1;
}

// OFFSET: 0x008067B0/0x140631ED0
static int32_t Script_GetCursorPosition(lua_State* L) {
    float x = 0.0f;
    float y = 0.0f;

    if (!CSimpleTop::s_instance) {
        SErrDisplayAppFatalMessage("", "", 0);
    }

    NDCToDDC(CSimpleTop::s_instance->m_mousePosition.m_x, CSimpleTop::s_instance->m_mousePosition.m_y, &x, &y);
    lua_pushnumber(L, DDCToNDCWidth(CoordinateGetAspectCompensation() * 1024.0f * x));
    lua_pushnumber(L, DDCToNDCWidth(CoordinateGetAspectCompensation() * 1024.0f * y));
    return 2;
}

// OFFSET: 0x008C6620/0x140636680
static int32_t Script_IsModifierKeyDown(lua_State* L) {
    for (int32_t key = KEY_LSHIFT; key <= KEY_RALT; key++) {
        if (CGGameUI::IsKeyDown(static_cast<KEY>(key))) {
            lua_pushnumber(L, 1.0);
            return 1;
        }
    }

    lua_pushnil(L);
    return 1;
}

// OFFSET: 0x008C6760/0x1406367C0
static int32_t Script_IsShiftKeyDown(lua_State* L) {
    if (CGGameUI::IsKeyDown(KEY_LSHIFT) || CGGameUI::IsKeyDown(KEY_RSHIFT)) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: 0x008C68B0/0x140636910
static int32_t Script_IsControlKeyDown(lua_State* L) {
    if (CGGameUI::IsKeyDown(KEY_LCONTROL) || CGGameUI::IsKeyDown(KEY_RCONTROL)) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: 0x008C6A00/0x140636A60
static int32_t Script_IsAltKeyDown(lua_State* L) {
    if (CGGameUI::IsKeyDown(KEY_LALT) || CGGameUI::IsKeyDown(KEY_RALT)) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: 0x008CC0E0/0x14063CF30
static int32_t Script_SetCVar(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: SetCVar(\"cvar\", value [, \"scriptCvar\")");
        return 0;
    }

    const char* name = lua_tolstring(L, 1, NULL);
    CVar* var = CVar::LookupRegistered(name);

    if (!var || (var->m_flags & 0x40)) {
        luaL_error(L, "Couldn't find CVar named '%s'", name);
        return 0;
    }

    if ((var->m_flags & 0x4) || (var->m_flags & 0x100)) {
        luaL_error(L, "\"%s\" is read-only", name);
        return 0;
    }

    if ((var->m_flags & 0x8) && !CSimpleTop::GetInstance()->m_allowProtectedActions) {
        CGGameUI::ShowBlockedActionFeedback(NULL, 2);
        return 0;
    }

    const char* value = lua_tolstring(L, 2, NULL);

    if (!value) {
        value = "0";
    }

    var->Set(value, true, false, false, true);

    if (lua_isstring(L, 3)) {
        FrameScript_SignalEvent(SCRIPT_EVENT_CVAR_UPDATE, "%s%s", lua_tolstring(L, 3, NULL), value);
    }

    return 0;
}

// OFFSET: 0x008CCD10/0x140637AC0
static int32_t Script_GetScreenWidth(lua_State* L) {
    float width = NDCToDDCWidth(1.0f);
    float scale = CGGameUI::m_UISimpleParent ? CGGameUI::m_UISimpleParent->m_layoutScale : 1.0f;
    lua_pushnumber(L, DDCToNDCWidth(CoordinateGetAspectCompensation() * 1024.0f * (width / scale)));
    return 1;
}

// OFFSET: 0x008CCD80/0x140637B40
static int32_t Script_GetScreenHeight(lua_State* L) {
    float height = NDCToDDCHeight(1.0f);
    float scale = CGGameUI::m_UISimpleParent ? CGGameUI::m_UISimpleParent->m_layoutScale : 1.0f;
    lua_pushnumber(L, DDCToNDCWidth(CoordinateGetAspectCompensation() * 1024.0f * (height / scale)));
    return 1;
}

// OFFSET: 0x008DAD50/0x14064EE00
static int32_t Script_LoadAddOn(lua_State* L) {
    const char* name;

    if (lua_isnumber(L, 1)) {
        name = GetAddOnByIndex(static_cast<int32_t>(lua_tonumber(L, 1)) - 1);

        if (!name) {
            luaL_error(L, "AddOn index must be in the range of 1 to %d", GetNumAddOns());
            return 0;
        }
    } else {
        if (!lua_isstring(L, 1)) {
            luaL_error(L, "Usage: LoadAddOn(index or \"name\")");
            return 0;
        }

        name = lua_tolstring(L, 1, NULL);
    }

    if (!s_loadAddOnDepth) {
        s_loadAddOnStatus = new (__FILE__, __LINE__) CWOWClientStatus("Logs\\FrameXML.log", 1);
    }

    s_loadAddOnDepth++;
    bool loaded = LoadAddOn(name, true, s_loadAddOnStatus);

    if (--s_loadAddOnDepth == 0) {
        if (s_loadAddOnStatus) {
            delete s_loadAddOnStatus;
        }

        s_loadAddOnStatus = NULL;
    }

    if (loaded) {
        lua_pushnumber(L, 1.0);
        lua_pushnil(L);
        return 2;
    }

    lua_pushnil(L);

    UIADDON_LOADABLE_REASON reason;
    UIADDON_LOADABLE_REASON dependencyReason;

    if (!IsAddOnLoadable(name, true, reason, dependencyReason, ClientServices::GetCharacterName()) && (reason || dependencyReason)) {
        char buffer[128];
        lua_pushstring(L, AddOnUnloadableToken(reason, dependencyReason, buffer, sizeof(buffer)));
        return 2;
    }

    lua_pushstring(L, "UNKNOWN_ERROR");
    return 2;
}

// OFFSET: 0x008C70D0/0x140631C70
static int32_t Script_GetSubZoneText(lua_State* L) {
    lua_pushstring(L, CGGameUI::m_subZoneText ? CGGameUI::m_subZoneText : "");
    return 1;
}

// OFFSET: 0x008C7100/0x140631CA0
static int32_t Script_GetMinimapZoneText(lua_State* L) {
    lua_pushstring(L, CGGameUI::m_minimapZoneText ? CGGameUI::m_minimapZoneText : "");
    return 1;
}

// OFFSET: 0x008C71A0/0x1406370B0
static int32_t Script_IsInInstance(lua_State* L) {
    MapRec* map = g_mapDB.GetRecord(CGGameUI::m_iCurrentMapID);

    if (!map) {
        lua_pushnil(L);
        lua_pushstring(L, s_instanceTypeTokens[0]);
        return 2;
    }

    if (map->m_instanceType) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    uint32_t type = map->m_instanceType;
    lua_pushstring(L, type < sizeof(s_instanceTypeTokens) / sizeof(s_instanceTypeTokens[0]) ? s_instanceTypeTokens[type] : s_instanceTypeTokens[0]);
    return 2;
}

// OFFSET: 0x008C8840/0x1406321B0
static int32_t Script_GetMouseFocus(lua_State* L) {
    if (!CSimpleTop::s_instance) {
        SErrDisplayAppFatalMessage("", "", 0);
    }

    CSimpleFrame* focus = CSimpleTop::s_instance->m_mouseFocus;

    if (!focus) {
        lua_pushnil(L);
        return 1;
    }

    if (!focus->m_luaRegistered) {
        focus->RegisterScriptObject(NULL);
    }

    lua_rawgeti(L, LUA_REGISTRYINDEX, focus->m_luaObjectRef);
    return 1;
}

// OFFSET: 0x008CD7C0/0x14063F4C0
static int32_t Script_GetZonePVPInfo(lua_State* L) {
    int32_t flags = -1;
    int32_t subzone = 0;
    AreaTableRec* subzoneRec = g_areaTableDB.GetRecord(CGGameUI::m_subzoneID);

    if (subzoneRec) {
        flags = subzoneRec->m_flags;
        subzone = 1;
    }

    AreaTableRec* area = g_areaTableDB.GetRecord(CGGameUI::m_areaID);

    if (flags == -1) {
        subzone = 0;
        flags = area ? area->m_flags : 0;
    }

    const char* type = NULL;

    if (flags & 0x800) {
        type = "sanctuary";
    } else if (flags & 0x80) {
        type = "arena";
    } else if (flags & 0x1000000) {
        type = "combat";
    }

    if (type) {
        lua_pushstring(L, type);

        if (subzone) {
            lua_pushnumber(L, 1.0);
        } else {
            lua_pushnil(L);
        }

        return 2;
    }

    int32_t contested = (flags & 0x40000) != 0;
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!area || !player || (!s_isPvPRealm && !(area->m_flags & 0x10))) {
        return 0;
    }

    if (!contested) {
        FactionTemplateRec* faction = g_factionTemplateDB.GetRecord(player->m_unit->m_unitFieldFactiontemplate);

        if (!faction || (area->m_factionGroupMask & faction->m_friendGroup)) {
            type = "friendly";
        } else if (area->m_factionGroupMask & faction->m_enemyGroup) {
            type = "hostile";
        }
    }

    lua_pushstring(L, type ? type : "contested");
    const char* factionName = "";

    for (int32_t i = 0; i < g_factionGroupDB.m_numRecords; i++) {
        FactionGroupRec* group = g_factionGroupDB.GetRecordByIndex(i);

        if (((1 << group->m_maskID) & area->m_factionGroupMask) && *group->m_name) {
            factionName = group->m_name;
            break;
        }
    }

    lua_pushnil(L);
    lua_pushstring(L, factionName);
    return 3;
}

// OFFSET: 0x008C17C0/0x140630D10
static int32_t Script_GetMouseButtonClicked(lua_State* L) {
    if (!CSimpleTop::s_instance) {
        SErrDisplayAppFatalMessage("", "", 0);
    }

    lua_pushstring(L, CSimpleTop::s_instance->m_mouseButtonName);
    return 1;
}

// OFFSET: #0x00517BE0
static int32_t ToggleInteractModelIcon(WOWGUID guid, void* param) {
    CGObject_C* object = ClntObjMgrObjectPtr(guid, TYPEMASK_OBJECT, __FILE__, __LINE__);

    if (object) {
        object->SetInteractIconVisible(CGGameUI::m_visible);
    }

    return 1;
}

// OFFSET: 0x008C8F90/0x140639180
static int32_t Script_SetUIVisibility(lua_State* L) {
    int32_t visible = StringToBOOL(L, 1, 1);

    if (CGGameUI::m_visible != visible) {
        CGGameUI::m_visible = visible;
        CGGameUI::UpdateUnitHighlights(0);
        ClntObjMgrEnumVisibleObjects(ToggleInteractModelIcon, NULL);
    }

    return 0;
}

// OFFSET: 0x008D3DF0/0x1406475C0
static int32_t Script_CancelLogout(lua_State* L) {
    if (lua_tainted && CGGameUI::m_simpleTop && !CGGameUI::m_simpleTop->unk_1260) {
        CGGameUI::ShowBlockedActionFeedback(NULL, 1);
        return 0;
    }

    ClientServices::Connection()->CharacterAbortLogout();
    return 0;
}

// OFFSET: 0x008D5DA0/0x140649C60
static int32_t Script_FocusUnit(lua_State* L) {
    WOWGUID guid;

    if (Script_GetGUIDFromToken(lua_tolstring(L, 1, NULL), guid, true)) {
        CGGameUI::SetFocusTarget(guid);
    }

    return 0;
}

// OFFSET: 0x008C1770/0x140630CB0
static int32_t Script_SetLayoutMode(lua_State* L) {
    int32_t enabled = 1;

    if (lua_isnumber(L, 1)) {
        enabled = static_cast<int32_t>(lua_tonumber(L, 1));
    }

    if (!CSimpleTop::s_instance) {
        SErrDisplayAppFatalMessage("", "", 0);
    }

    CSimpleTop::s_instance->m_moveResizeEnabled = enabled;
    return 0;
}

// OFFSET: 0x008C34B0/0x140632F30
static int32_t Script_InCombatLockdown(lua_State* L) {
    if (!CSimpleTop::s_instance) {
        SErrDisplayAppFatalMessage("", "", 0);
    }

    if (CSimpleTop::s_instance->m_allowProtectedActions) {
        lua_pushnil(L);
    } else {
        lua_pushnumber(L, 1.0);
    }

    return 1;
}

// OFFSET: 0x008CEBF0/0x140640EF0
static int32_t Script_DestroyTotem(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: DestroyTotem(slot)");
        return 0;
    }

    int32_t slot = static_cast<int32_t>(lua_tonumber(L, 1));

    if (static_cast<uint32_t>(slot - 1) > 3) {
        luaL_error(L, "Totem slot must be in range of 1 to %d", 4);
        return 0;
    }

    CGGameUI::DestroyTotem(slot - 1);
    return 0;
}

// OFFSET: 0x008C7680/0x140637480
static int32_t Script_ArenaTeamInviteByName(lua_State* L) {
    if (!lua_isnumber(L, 1) || !lua_isstring(L, 2)) {
        luaL_error(L, "Usage: ArenaTeamInviteByName(team, name)");
        return 0;
    }

    uint32_t index = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1;
    const char* playerName = lua_tolstring(L, 2, NULL);
    uint32_t teamId = index >= 3 ? 0 : CGArenaTeamInfo::m_arenaTeams[index].m_teamId;

    if (!playerName || !*playerName) {
        return 0;
    }

    if (SStrLen(playerName) > 48) {
        luaL_error(L, "Name too long");
        return 0;
    }

    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_ARENA_TEAM_INVITE));
    msg.Put(teamId);
    msg.PutString(playerName);
    msg.Finalize();
    ClientServices::Send2(&msg);
    return 0;
}

// OFFSET: 0x008C77A0/0x140637600
static int32_t Script_ArenaTeamLeave(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: ArenaTeamLeave(team)");
        return 0;
    }

    uint32_t index = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1;
    uint32_t teamId = index >= 3 ? 0 : CGArenaTeamInfo::m_arenaTeams[index].m_teamId;
    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_ARENA_TEAM_LEAVE));
    msg.Put(teamId);
    msg.Finalize();
    ClientServices::Send2(&msg);
    return 0;
}

// OFFSET: 0x008C7880/0x1406376F0
static int32_t Script_ArenaTeamUninviteByName(lua_State* L) {
    if (!lua_isnumber(L, 1) || !lua_isstring(L, 2)) {
        luaL_error(L, "Usage: ArenaTeamUninviteByName(team, name)");
        return 0;
    }

    uint32_t index = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1;
    const char* playerName = lua_tolstring(L, 2, NULL);

    if (playerName && SStrLen(playerName) > 48) {
        luaL_error(L, "Name too long");
        return 0;
    }

    uint32_t teamId = index >= 3 ? 0 : CGArenaTeamInfo::m_arenaTeams[index].m_teamId;
    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_ARENA_TEAM_REMOVE));
    msg.Put(teamId);
    msg.PutString(playerName);
    msg.Finalize();
    ClientServices::Send2(&msg);
    return 0;
}

// OFFSET: 0x008C79C0/0x140637860
static int32_t Script_ArenaTeamSetLeaderByName(lua_State* L) {
    if (!lua_isnumber(L, 1) || !lua_isstring(L, 2)) {
        luaL_error(L, "Usage: ArenaTeamSetLeaderByName(team, name)");
        return 0;
    }

    uint32_t index = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1;
    const char* playerName = lua_tolstring(L, 2, NULL);

    if (playerName && SStrLen(playerName) > 48) {
        luaL_error(L, "Name too long");
        return 0;
    }

    uint32_t teamId = index >= 3 ? 0 : CGArenaTeamInfo::m_arenaTeams[index].m_teamId;
    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_ARENA_TEAM_LEADER));
    msg.Put(teamId);
    msg.PutString(playerName);
    msg.Finalize();
    ClientServices::Send2(&msg);
    return 0;
}

// OFFSET: 0x008C7B00/0x1406379D0
static int32_t Script_ArenaTeamDisband(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: ArenaTeamDisband(team)");
        return 0;
    }

    uint32_t index = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1;
    uint32_t teamId = index >= 3 ? 0 : CGArenaTeamInfo::m_arenaTeams[index].m_teamId;
    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_ARENA_TEAM_DISBAND));
    msg.Put(teamId);
    msg.Finalize();
    ClientServices::Send2(&msg);
    return 0;
}

// OFFSET: 0x008CE8E0/0x140640850
static int32_t Script_IsReferAFriendLinked(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));
    WOWGUID guid = GetGuidByKeyword(lua_tolstring(L, 1, NULL));

    if (player && player->IsRAFPlayer(guid)) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: 0x008C3500/0x140632FC0
static int32_t Script_GetSummonFriendCooldown(lua_State* L) {
    uint32_t start = 0;
    uint32_t duration = 0;
    uint32_t enable = 0;
    Spell_C_GetSpellCooldown(CGSpellBook::m_summonFriendSpell, 0, &duration, &start, &enable);
    lua_pushnumber(L, start * 0.001);
    lua_pushnumber(L, duration * 0.001);
    lua_pushnumber(L, enable);
    return 3;
}

// OFFSET: 0x008CEA60/0x140640D00
static int32_t Script_GetTotemInfo(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetTotemInfo(slot)");
        return 0;
    }

    uint32_t slot = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1;

    if (slot > 3) {
        luaL_error(L, "Totem slot must be in range of 1 to %d", 4);
        return 0;
    }

    TotemInfo* totem = slot < sizeof(((CGGameUI*)0)->m_totems) / sizeof(CGGameUI::m_totems[0]) ? &CGGameUI::m_totems[slot] : NULL;

    if (totem && totem->m_guid) {
        lua_pushboolean(L, 1);
        lua_pushstring(L, totem->m_name);
        lua_pushnumber(L, totem->m_startTime / 1000);
        lua_pushnumber(L, totem->m_duration / 1000);
        lua_pushstring(L, totem->m_icon);
        return 5;
    }

    bool found = false;
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player) {
        int32_t category = 0;

        switch (slot) {
            case 0:
                category = 4;
                break;

            case 1:
                category = 2;
                break;

            case 2:
                category = 5;
                break;

            case 3:
                category = 3;
                break;
        }

        found = player->m_bag.FindTotemCategory(category, 8);
    }

    lua_pushboolean(L, found);
    lua_pushstring(L, "");
    lua_pushnumber(L, 0.0);
    lua_pushnumber(L, 0.0);
    lua_pushstring(L, "");
    return 5;
}

// OFFSET: 0x008CC760/0x14063DC70
static int32_t Script_GetInstanceInfo(lua_State* L) {
    int32_t mapId = CGGameUI::m_iCurrentMapID;
    MapRec* map = g_mapDB.GetRecord(mapId);

    if (!map) {
        lua_pushnil(L);
        lua_pushstring(L, s_instanceTypeTokens[0]);
        lua_pushnumber(L, 0.0);
        lua_pushstring(L, "");
        lua_pushnumber(L, 0.0);
        lua_pushnumber(L, 0.0);
        lua_pushnil(L);
        return 7;
    }

    lua_pushstring(L, map->m_mapName);
    uint32_t type = map->m_instanceType;
    lua_pushstring(L, type < 5 ? s_instanceTypeTokens[type] : s_instanceTypeTokens[0]);
    uint32_t difficulty = CGGameUI::m_instanceDifficulty;
    lua_pushnumber(L, difficulty + 1);
    MapDifficultyRec* rec = GetMapDifficultyRec(mapId, difficulty, NULL);

    if (rec) {
        lua_pushstring(L, FrameScript_GetText(rec->m_difficultystring, -1, GENDER_NOT_APPLICABLE));
        lua_pushnumber(L, rec->m_maxPlayers);
    } else {
        lua_pushstring(L, "");
        lua_pushnumber(L, 0.0);
    }

    lua_pushnumber(L, CGPartyInfo::m_dynamicDifficulty);
    map = g_mapDB.GetRecord(CGGameUI::m_iCurrentMapID);
    lua_pushboolean(L, map && (map->m_flags & 0x100));
    return 7;
}

// OFFSET: 0x008CD620/0x14063F2D0
static int32_t Script_GetRestState(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));
    ExhaustionRec* rec = player ? g_exhaustionDB.GetRecord(player->m_player->m_playerBytes2[3]) : NULL;

    if (!rec) {
        lua_pushnil(L);
        lua_pushnil(L);
        lua_pushnil(L);
        return 3;
    }

    lua_pushnumber(L, rec->m_id);
    lua_pushstring(L, rec->m_name);
    lua_pushnumber(L, rec->m_factor);
    return 3;
}

// OFFSET: 0x008CD6D0/0x14063F3B0
static int32_t Script_GetXPExhaustion(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));
    ExhaustionRec* rested = g_exhaustionDB.GetRecord(1);

    if (!player || player->m_player->m_playerBytes2[3] != 1) {
        lua_pushnil(L);
        return 1;
    }

    uint32_t experience = player->m_obj->m_guid == ClntObjMgrGetActivePlayer() ? player->m_player->m_playerRestStateExperience : 0;
    lua_pushnumber(L, experience * rested->m_factor);
    return 1;
}

// OFFSET: 0x008D7810
static int32_t Script_ClearTarget(lua_State* L) {
    int32_t hadTarget = CGGameUI::m_lockedTarget != WOWGUID();
    CGGameUI::unk_BD08CC = 0;

    if (CGGameUI::m_lockedTarget) {
        CGGameUI::m_lastTarget = CGGameUI::m_lockedTarget;
        CGGameUI::ClearTarget(CGGameUI::m_lockedTarget, 1);

        if (Spell_C_GetAutoRepeatingSpell()) {
            Spell_C_CancelAutoRepeat(true);
        }
    }

    if (hadTarget) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: 0x008C2020/0x140631510
static int32_t Script_Quit(lua_State* L) {
    ClientServices::Connection()->CharacterLogout(true, false);
    return 0;
}

// OFFSET: 0x008DE300/0x140653040
static int32_t Script_RegisterForSave(lua_State* L) {
    if (lua_tainted) {
        luaL_error(L, "RegisterForSave() is only available to Blizzard scripts");
        return 0;
    }

    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: RegisterForSave(\"variable\")");
        return 0;
    }

    CGGameUI::RegisterSavedVariable(lua_tolstring(L, 1, NULL), 0);
    return 0;
}

// OFFSET: 0x008DE370/0x1406530B0
static int32_t Script_RegisterForSavePerCharacter(lua_State* L) {
    if (lua_tainted) {
        luaL_error(L, "RegisterForSavePerCharacter() is only available to Blizzard scripts");
        return 0;
    }

    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: RegisterForSavePerCharacter(\"variable\")");
        return 0;
    }

    CGGameUI::RegisterSavedVariable(lua_tolstring(L, 1, NULL), 1);
    return 0;
}

// OFFSET: 0x008CE320/0x140640160
static int32_t Script_GetMirrorTimerInfo(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetMirrorTimerInfo(\"timer\")");
        return 0;
    }

    uint32_t index = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1;

    if (index > 2) {
        luaL_error(L, "ERROR: Invalid timer (Ex: \"BREATH\"");
        return 0;
    }

    MirrorTimerInfo& timer = CGGameUI::m_mirrorTimerInfo[index];
    lua_pushstring(L, MirrorTimerTypeToken(timer.m_type));
    lua_pushnumber(L, timer.m_value);
    lua_pushnumber(L, timer.m_maxValue);
    lua_pushnumber(L, timer.m_scale);
    lua_pushnumber(L, timer.m_paused);
    lua_pushstring(L, GetMirrorTimerLabel(timer.m_type, timer.m_spellId));
    return 6;
}

// OFFSET: 0x008C2040/0x140631530
static int32_t Script_GetFramerate(lua_State* L) {
    lua_pushnumber(L, World::GetFramerate());
    return 1;
}

// OFFSET: 0x008C8040/0x140637FD0
static int32_t Script_GetNumTitles(lua_State* L) {
    lua_pushnumber(L, g_charTitlesDB.m_maxID + 1);
    return 1;
}

// OFFSET: 0x008CD0C0/0x14063EC60
static int32_t Script_GetCurrentTitle(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player) {
        lua_pushnumber(L, static_cast<int32_t>(player->m_player->m_playerChosenTitle));
    } else {
        lua_pushnumber(L, 0.0);
    }

    return 1;
}

// OFFSET: 0x008C2900/0x140632380
static int32_t Script_GetItemCooldown(lua_State* L) {
    int32_t itemId = 0;

    if (lua_isnumber(L, 1)) {
        itemId = lua_tointeger(L, 1);
    } else if (lua_isstring(L, 1)) {
        itemId = GetItemIDByName(lua_tolstring(L, 1, NULL));
    }

    uint32_t start = 0;
    uint32_t duration = 0;
    uint32_t enable = 0;

    if (itemId) {
        CGItem_C* item = CGActionBar::FindUseEnchantItemPtr(itemId);

        if (item) {
            Spell_C_GetItemCooldown(item, &duration, &start, &enable);
        } else {
            Spell_C_GetItemCooldown(itemId, &duration, &start, &enable);
        }
    }

    lua_pushnumber(L, start * 0.001);
    lua_pushnumber(L, duration * 0.001);
    lua_pushnumber(L, enable);
    return 3;
}

// OFFSET: 0x008D9B80/0x14064D5E0
static int32_t Script_GetItemInfo(lua_State* L) {
    TooltipExtendedItemInfo linkInfo;
    linkInfo.Clear();
    int32_t itemId;

    if (lua_isnumber(L, 1)) {
        itemId = static_cast<int32_t>(lua_tonumber(L, 1));
    } else {
        if (!lua_isstring(L, 1)) {
            luaL_error(L, "Usage: GetItemInfo(itemID|\"name\"|\"itemlink\")");
            return 0;
        }

        const char* text = lua_tolstring(L, 1, NULL);

        if (SStrStr(text, "item:")) {
            itemId = linkInfo.GetItemIdFromLink(text);
        } else {
            itemId = GetItemIDByName(lua_tolstring(L, 1, NULL));
        }
    }

    WOWGUID none = WOWGUID();
    ItemStats_C* stats = g_itemDBCache.GetRecord(itemId, none, NULL, NULL, false);

    if (!stats) {
        return 0;
    }

    char name[1024];
    CGItem_C::BuildItemName(name, sizeof(name), itemId, linkInfo.m_randomPropertiesId);
    lua_pushstring(L, name);
    lua_pushstring(L, CGTooltip::GetItemHyperlinkString(itemId, stats->m_quality, linkInfo.m_enchants[0], reinterpret_cast<const int32_t*>(&linkInfo.m_enchants[2]), linkInfo.m_randomPropertiesId, linkInfo.m_propertySeed, 0));
    lua_pushnumber(L, static_cast<int32_t>(stats->m_quality));
    lua_pushnumber(L, static_cast<int32_t>(stats->m_itemLevel));
    lua_pushnumber(L, static_cast<int32_t>(stats->m_requiredLevel));
    ItemClassRec* itemClass = g_itemClassDB.GetRecord(stats->m_class);
    lua_pushstring(L, itemClass ? itemClass->m_className : "");
    const ItemSubClassRec* subClass = NULL;

    for (int32_t i = 0; i < g_itemSubClassDB.m_numRecords; i++) {
        const ItemSubClassRec* rec = g_itemSubClassDB.GetRecordByIndex(i);

        if (rec && rec->m_classID == static_cast<int32_t>(stats->m_class) && rec->m_subClassID == static_cast<int32_t>(stats->m_subClass)) {
            subClass = rec;
            break;
        }
    }

    if (subClass) {
        lua_pushstring(L, *subClass->m_verboseName ? subClass->m_verboseName : subClass->m_displayName);
    } else {
        lua_pushstring(L, "");
    }

    lua_pushnumber(L, stats->m_stackable);
    lua_pushstring(L, s_inventoryTypeTokens[stats->m_inventoryType]);
    const char* path = ClientDBStringLookup(3);
    const char* separator = *path ? "\\" : "";
    char texture[260];
    SStrPrintf(texture, sizeof(texture), "%s%s%s", path, separator, CGItem_C::GetInventoryArt(stats->m_displayId));
    lua_pushstring(L, texture);
    lua_pushnumber(L, static_cast<int32_t>(stats->m_sellPrice));
    return 11;
}

// OFFSET: 0x008C88A0/0x1406389D0
static int32_t Script_GetItemSpell(lua_State* L) {
    const SpellRec* spell = NULL;
    int32_t itemId = 0;

    if (lua_isnumber(L, 1)) {
        itemId = lua_tointeger(L, 1);
    } else if (lua_isstring(L, 1)) {
        itemId = GetItemIDByName(lua_tolstring(L, 1, NULL));
    }

    CGItem_C* item = CGActionBar::FindUseEnchantItemPtr(itemId);
    int32_t spellId = item ? item->GetEnchantmentSpell(0) : CGItem_C::GetUseSpell(itemId);

    if ((spell = g_spellDB.GetRecord(spellId))) {
        lua_pushstring(L, spell->m_name);
        lua_pushstring(L, spell->m_nameSubtext);
    } else {
        lua_pushnil(L);
        lua_pushnil(L);
    }

    return 2;
}

// OFFSET: 0x008C8FD0/0x1406391C0
static int32_t Script_GetTotemTimeLeft(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetTotemTimeLeft(slot)");
        return 0;
    }

    uint32_t slot = static_cast<uint32_t>(lua_tonumber(L, 1)) - 1;

    if (slot > 3) {
        luaL_error(L, "Totem slot must be in range of 1 to %d", 4);
        return 0;
    }

    TotemInfo* totem = &CGGameUI::m_totems[slot];

    if (totem && totem->m_guid.m_guid) {
        lua_pushnumber(L, totem->GetSpawnTimeLeft() / 1000);
    } else {
        lua_pushnumber(L, 0.0);
    }

    return 1;
}

// OFFSET: 0x008D5F00/0x14064A1C0
static int32_t Script_StopCinematic(lua_State* L) {
    SE3::StopSound(CGGameUI::m_cinematic.m_cameraMusic, 0, __FILE__, __LINE__, "CGGameUI::StopCinematic");
    EnableFadingScreen(0.25f, &CGGameUI::StopCinematicInternal, NULL);
    return 0;
}

// OFFSET: 0x008D7320/0x140649670
static int32_t Script_TargetNearest(lua_State* L) {
    if (CGUIMacros::IsActionAllowed(5)) {
        TargetNearestEnemy(StringToBOOL(L, 1, 0), 0);
        CGUIMacros::m_state &= ~0x20u;
    }

    return 0;
}

// OFFSET: 0x008D7360/0x1406496B0
static int32_t Script_TargetNearestEnemy(lua_State* L) {
    if (CGUIMacros::IsActionAllowed(5)) {
        TargetNearestEnemy(StringToBOOL(L, 1, 0), 1);
        CGUIMacros::m_state &= ~0x20u;
    }

    return 0;
}

// OFFSET: 0x008D73A0/0x1406496F0
static int32_t Script_TargetNearestEnemyPlayer(lua_State* L) {
    if (CGUIMacros::IsActionAllowed(5)) {
        TargetNearestEnemy(StringToBOOL(L, 1, 0), 2);
        CGUIMacros::m_state &= ~0x20u;
    }

    return 0;
}

// OFFSET: 0x008D73E0/0x140649730
static int32_t Script_TargetNearestFriend(lua_State* L) {
    if (CGUIMacros::IsActionAllowed(5)) {
        TargetNearestEnemy(StringToBOOL(L, 1, 0), 3);
        CGUIMacros::m_state &= ~0x20u;
    }

    return 0;
}

// OFFSET: 0x008D7420/0x140649770
static int32_t Script_TargetNearestFriendPlayer(lua_State* L) {
    if (CGUIMacros::IsActionAllowed(5)) {
        TargetNearestEnemy(StringToBOOL(L, 1, 0), 4);
        CGUIMacros::m_state &= ~0x20u;
    }

    return 0;
}

// OFFSET: 0x008D7460/0x1406497B0
static int32_t Script_TargetNearestPartyMember(lua_State* L) {
    TargetNearestEnemy(StringToBOOL(L, 1, 0), 5);
    return 0;
}

// OFFSET: 0x008D7490/0x1406497E0
static int32_t Script_TargetNearestRaidMember(lua_State* L) {
    TargetNearestEnemy(StringToBOOL(L, 1, 0), 6);
    return 0;
}

// OFFSET: 0x008C3800/0x140633320
static int32_t Script_GetThreatStatusColor(lua_State* L) {
    CImVector color = *CGTooltip::GetThreatStateColor(static_cast<uint32_t>(static_cast<int64_t>(lua_tonumber(L, 1) + 1.0)));
    lua_pushnumber(L, color.r * 0.0039215689);
    lua_pushnumber(L, color.g * 0.0039215689);
    lua_pushnumber(L, color.b * 0.0039215689);
    return 3;
}

// OFFSET: 0x008C24F0/0x140631B30
static int32_t Script_ResetCursor(lua_State* L) {
    CursorResetMode();
    return 0;
}

// OFFSET: 0x008D96D0/0x14064D0B0
static int32_t Script_GetCursorInfo(lua_State* L) {
    switch (CGGameUI::m_cursorItemType) {
        case 1: {
            CGItem_C* item = static_cast<CGItem_C*>(ClntObjMgrObjectPtr(CGGameUI::GetCursorItem(), TYPEMASK_ITEM, __FILE__, __LINE__));

            if (!item) {
                return 0;
            }

            lua_pushstring(L, "item");
            lua_pushnumber(L, item->m_obj->m_entryID);
            lua_pushstring(L, CGTooltip::GetItemHyperlinkString(item));
            return 3;
        }

        case 2:
            lua_pushstring(L, "money");
            lua_pushnumber(L, static_cast<double>(CGGameUI::m_cursorMoney));
            return 2;

        case 3: {
            int32_t spellId = CGGameUI::m_cursorSpell;
            const SpellRec* spell = g_spellDB.GetRecord(spellId);

            if (spell && SpellIsCallCompanion(spell)) {
                int32_t type = CGSpellBook::GetCompanionType(spell);
                lua_pushstring(L, "companion");
                lua_pushnumber(L, CGSpellBook::FindCompanionSlotByID(spellId, type) + 1);

                if (type == 0) {
                    lua_pushstring(L, "CRITTER");
                } else if (type == 1) {
                    lua_pushstring(L, "MOUNT");
                } else {
                    lua_pushstring(L, "UNKNOWN");
                }

                return 3;
            }

            int32_t petSpell = 0;
            int32_t slot = CGSpellBook::FindSlotBySpellID(spellId, 0);

            if (slot < 0) {
                slot = CGSpellBook::FindSlotBySpellID(spellId, 1);

                if (slot >= 0) {
                    petSpell = 1;
                }
            }

            lua_pushstring(L, "spell");
            lua_pushnumber(L, slot + 1);
            lua_pushstring(L, petSpell ? "pet" : "spell");
            lua_pushnumber(L, spellId);
            return 4;
        }

        case 4:
            lua_pushstring(L, "petaction");
            return 1;

        case 5:
            lua_pushstring(L, "merchant");
            lua_pushnumber(L, CGGameUI::m_cursorVirtualSlot + 1);
            return 2;

        case 6:
        case 10: {
            int32_t itemId = CGGameUI::m_cursorVirtualID;
            ItemRecSparse* rec = g_itemSparseDB.GetRecord(itemId, WOWGUID(), NULL, NULL, false);

            if (!rec) {
                return 0;
            }

            lua_pushstring(L, "item");
            lua_pushnumber(L, itemId);
            lua_pushstring(L, CGTooltip::GetItemHyperlinkString(itemId, rec->m_quality, 0, 0, 0, 0));
            return 3;
        }

        case 7:
            lua_pushstring(L, "macro");
            lua_pushnumber(L, CGUIMacros::GetMacroIndexByID(CGGameUI::m_cursorMacro) + 1);
            return 2;

        case 9:
            lua_pushstring(L, "pet");
            lua_pushnumber(L, static_cast<uint32_t>(CGGameUI::m_cursorPet + 1));
            return 2;

        case 11:
            lua_pushstring(L, "guildbankmoney");
            lua_pushnumber(L, static_cast<double>(CGGameUI::m_cursorMoney));
            return 2;

        case 12: {
            const EQUIPMENT_SET* set = CGEquipmentManager::GetSetByID(CGGameUI::m_cursorEquipmentSet);

            if (!set) {
                return 0;
            }

            lua_pushstring(L, "equipmentset");
            lua_pushstring(L, set->m_name);
            return 2;
        }

        case 13: {
            CurrencyTypesRec* currency = g_currencyTypesDB.GetRecord(CGGameUI::m_cursorCurrency);

            if (!currency) {
                return 0;
            }

            lua_pushstring(L, "currency");
            lua_pushnumber(L, currency->m_id);
            lua_pushstring(L, CGCurrencyTypes::GetCurrencyIcon(currency));
            return 3;
        }

        case 14: {
            SpellFlyoutRec* flyout = g_spellFlyoutDB.GetRecord(CGGameUI::m_cursorFlyout);

            if (!flyout) {
                return 0;
            }

            SpellIconRec* icon = g_spellIconDB.GetRecord(flyout->m_spellIconID);
            const char* texture = NULL;

            if (icon) {
                texture = icon->m_textureFilename;
            }

            lua_pushstring(L, "flyout");
            lua_pushnumber(L, flyout->m_id);
            lua_pushstring(L, texture);
            return 3;
        }

        default:
            return 0;
    }
}

// OFFSET: 0x008CC660/0x14063D730
static int32_t Script_AutoEquipCursorItem(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player) {
        player->AutoEquipCursorItem(0);
    }

    return 0;
}

// OFFSET: 0x008CCBC0/0x14063E360
static int32_t Script_HasKey(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player && player->m_bag.FindItemOfBagFamily(0x100, 79)) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: 0x008CD120/0x14063ECD0
static int32_t Script_IsTitleKnown(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: IsTitleKnown(titleMaskID)");
        return 0;
    }

    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));
    uint32_t mask = static_cast<uint32_t>(lua_tonumber(L, 1));

    if (player && mask && (player->m_player->m_playerFieldKnownTitles[mask >> 6] & (1ull << (mask & 63)))) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnumber(L, 0.0);
    }

    return 1;
}

// OFFSET: 0x008CD1F0/0x14063ED90
static int32_t Script_GetTitleName(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetTitleName(titleMaskID)");
        return 0;
    }

    int32_t maskId = static_cast<int32_t>(lua_tonumber(L, 1));
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));
    int32_t gender = player ? player->m_player->m_playerBytes3[0] : 0;
    char title[256];
    title[0] = 0;
    CharTitlesRec* rec = NULL;

    for (int32_t i = 0; i < g_charTitlesDB.m_numRecords; i++) {
        CharTitlesRec* candidate = g_charTitlesDB.GetRecordByIndex(i);

        if (candidate && candidate->m_maskID == maskId) {
            rec = candidate;
            break;
        }
    }

    if (!rec) {
        return 0;
    }

    if (gender == 1) {
        strcpy(title, strcmp(rec->m_name1, "") ? rec->m_name1 : rec->m_name);
    } else {
        strcpy(title, strcmp(rec->m_name, "") ? rec->m_name : rec->m_name1);
    }

    char stripped[256];
    int32_t length = 0;
    bool skipping = false;

    for (uint32_t i = 0; i < SStrLen(title); i++) {
        char c = title[i];

        if (!c) {
            break;
        }

        if (c == '%') {
            skipping = true;
        } else if (!skipping) {
            stripped[length++] = c;
        } else if (c == ' ') {
            skipping = false;
            stripped[length++] = c;
        }
    }

    stripped[length] = 0;
    lua_pushstring(L, stripped);
    return 1;
}

// OFFSET: 0x008CCEB0/0x14063EA30
static int32_t Script_SitStandOrDescendStart(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return 0;
    }

    DescendStart();

    if (player->m_movement->m_flags & 0x2000000) {
        return 0;
    }

    if (!player->m_unit->m_unitFieldBytes1[0]) {
        player->ChangeStandState(1);
    } else {
        player->ChangeStandState(0);
    }

    return 0;
}

// OFFSET: 0x008C2070/0x140631560
static int32_t Script_SetCursor(lua_State* L) {
    struct CursorName {
        uint32_t m_mode;
        const char* m_name;
    };

    CursorName cursors[52] = { { 1, "POINT_CURSOR" }, { 2, "CAST_CURSOR" }, { 3, "BUY_CURSOR" }, { 4, "ATTACK_CURSOR" }, { 5, "INTERACT_CURSOR" }, { 6, "SPEAK_CURSOR" }, { 7, "INSPECT_CURSOR" }, { 8, "PICKUP_CURSOR" }, { 9, "TAXI_CURSOR" }, { 10, "TRAINER_CURSOR" }, { 11, "MINE_CURSOR" }, { 12, "SKIN_CURSOR" }, { 13, "GATHER_CURSOR" }, { 14, "LOCK_CURSOR" }, { 15, "MAIL_CURSOR" }, { 16, "LOOT_ALL_CURSOR" }, { 17, "REPAIR_CURSOR" }, { 18, "REPAIRNPC_CURSOR" }, { 19, "ITEM_CURSOR" }, { 20, "SKIN_HORDE_CURSOR" }, { 21, "SKIN_ALLIANCE_CURSOR" }, { 22, "INNKEEPER_CURSOR" }, { 23, "QUEST_CURSOR" }, { 24, "QUEST_REPEATABLE_CURSOR" }, { 25, "QUEST_TURNIN_CURSOR" }, { 26, "VEHICLE_CURSOR" }, { 27, "POINT_ERROR_CURSOR" }, { 28, "CAST_ERROR_CURSOR" }, { 29, "BUY_ERROR_CURSOR" }, { 30, "ATTACK_ERROR_CURSOR" }, { 31, "INTERACT_ERROR_CURSOR" }, { 32, "SPEAK_ERROR_CURSOR" }, { 33, "INSPECT_ERROR_CURSOR" }, { 34, "PICKUP_ERROR_CURSOR" }, { 35, "TAXI_ERROR_CURSOR" }, { 36, "TRAINER_ERROR_CURSOR" }, { 37, "MINE_ERROR_CURSOR" }, { 38, "SKIN_ERROR_CURSOR" }, { 39, "GATHER_ERROR_CURSOR" }, { 40, "LOCK_ERROR_CURSOR" }, { 41, "MAIL_ERROR_CURSOR" }, { 42, "LOOT_ALL_ERROR_CURSOR" }, { 43, "REPAIR_ERROR_CURSOR" }, { 44, "REPAIRNPC_ERROR_CURSOR" }, { 45, "ITEM_ERROR_CURSOR" }, { 46, "SKIN_HORDE_ERROR_CURSOR" }, { 47, "SKIN_ALLIANCE_ERROR_CURSOR" }, { 48, "INNKEEPER_ERROR_CURSOR" }, { 49, "QUEST_ERROR_CURSOR" }, { 50, "QUEST_REPEATABLE_ERROR_CURSOR" }, { 51, "QUEST_TURNIN_ERROR_CURSOR" }, { 52, "VEHICLE_ERROR_CURSOR" } };

    if (lua_type(L, 1) == LUA_TNIL) {
        CursorResetMode();
        return 0;
    }

    const char* name = lua_tolstring(L, 1, NULL);

    if (!name) {
        luaL_error(L, "Usage: SetCursor(\"cursor\" or nil)");
        return 0;
    }

    for (uint32_t i = 0; i < 52; i++) {
        if (!SStrCmpI(cursors[i].m_name, name, STORM_MAX_STR)) {
            CursorSetMode(cursors[i].m_mode);
            lua_pushnumber(L, 1.0);
            return 1;
        }
    }

    if (CursorSetFromName(name)) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: 0x008C85D0/0x1406386B0
static int32_t Script_SetPortraitToTexture(lua_State* L) {
    CSimpleTexture* texture;

    if (lua_isstring(L, 1)) {
        texture = static_cast<CSimpleTexture*>(CScriptObject::GetScriptObjectByName(lua_tolstring(L, 1, NULL), CSimpleTexture::GetObjectType()));
    } else {
        if (lua_type(L, 1) != LUA_TTABLE) {
            luaL_error(L, "SetPortraitToTexture(): Couldn't find texture named '%s'", lua_tolstring(L, 1, NULL));
        }

        lua_rawgeti(L, 1, 0);
        texture = static_cast<CSimpleTexture*>(lua_touserdata(L, -1));
        lua_settop(L, -2);

        if (!texture) {
            luaL_error(L, "SetPortraitToTexture(): Couldn't find 'this' in texture object");
        }

        if (!texture->IsA(CSimpleTexture::GetObjectType())) {
            luaL_error(L, "SetPortraitToTexture(): Wrong object type, expected texture");
        }
    }

    if (!texture) {
        luaL_error(L, "SetPortraitToTexture(): Couldn't find texture named '%s'", lua_tolstring(L, 1, NULL));
    }

    if (lua_isstring(L, 2)) {
        SetPortraitTexture(texture, lua_tolstring(L, 2, NULL));
    }

    return 0;
}

// OFFSET: 0x008C2000/0x1406314F0
static int32_t Script_Logout(lua_State* L) {
    ClientServices::Connection()->CharacterLogout(false, false);
    return 0;
}

// OFFSET: 0x008C1710/0x140630C50
static int32_t Script_GetBuildInfo(lua_State* L) {
    lua_pushstring(L, "3.3.5");
    lua_pushstring(L, "12340");
    lua_pushstring(L, "Jun 24 2010");
    lua_pushnumber(L, GetCodeInterfaceVersion());
    return 4;
}

// OFFSET: 0x008C17F0/0x140630D40
static int32_t Script_SetConsoleKey(lua_State* L) {
    KEY key = KEY_LAST;

    if (!lua_isstring(L, 1)) {
        SetConsoleKey(key);
        return 0;
    }

    const char* name = lua_tolstring(L, 1, NULL);

    if (SStrLen(name) == 1) {
        char c = *name;

        if ((c >= '0' && c <= '9') || (c >= 'A' && c <= 'Z') || c < 'a' || c > 'z') {
            SetConsoleKey(static_cast<KEY>(c));
        } else {
            SetConsoleKey(static_cast<KEY>(toupper(c)));
        }

        return 0;
    }

    if (toupper(*name) == 'F' && isdigit(name[1])) {
        SetConsoleKey(static_cast<KEY>(SStrToInt(name + 1) + KEY_F1 - 1));
        return 0;
    }

    if (!SStrCmpI(name, "NUMPAD", SStrLen("NUMPAD"))) {
        const char* rest = name + SStrLen("NUMPAD");

        while (*rest == ' ') {
            rest++;
        }

        if (isdigit(*rest)) {
            SetConsoleKey(static_cast<KEY>(*rest + KEY_NUMPAD0 - '0'));
            return 0;
        }

        if (!SStrCmpI(rest, "PLUS", STORM_MAX_STR) || *rest == '+') {
            SetConsoleKey(KEY_NUMPAD_PLUS);
            return 0;
        }

        if (!SStrCmpI(rest, "MINUS", STORM_MAX_STR) || *rest == '-') {
            SetConsoleKey(KEY_NUMPAD_MINUS);
            return 0;
        }

        if (!SStrCmpI(rest, "MULTIPLY", STORM_MAX_STR) || *rest == '*') {
            SetConsoleKey(KEY_NUMPAD_MULTIPLY);
            return 0;
        }

        if (!SStrCmpI(rest, "DIVIDE", STORM_MAX_STR) || *rest == '/') {
            SetConsoleKey(KEY_NUMPAD_DIVIDE);
            return 0;
        }

        if (!SStrCmpI(rest, "DECIMAL", STORM_MAX_STR) || *rest == '.') {
            SetConsoleKey(KEY_NUMPAD_DECIMAL);
            return 0;
        }

        if (!SStrCmpI(rest, "EQUALS", STORM_MAX_STR) || *rest == '=') {
            SetConsoleKey(KEY_NUMPAD_EQUALS);
            return 0;
        }
    } else {
        if (!SStrCmpI(name, "ESCAPE", STORM_MAX_STR)) {
            SetConsoleKey(KEY_ESCAPE);
            return 0;
        }

        if (!SStrCmpI(name, "ENTER", STORM_MAX_STR)) {
            SetConsoleKey(KEY_ENTER);
            return 0;
        }

        if (!SStrCmpI(name, "BACKSPACE", STORM_MAX_STR)) {
            SetConsoleKey(KEY_BACKSPACE);
            return 0;
        }

        if (!SStrCmpI(name, "TAB", STORM_MAX_STR)) {
            SetConsoleKey(KEY_TAB);
            return 0;
        }

        if (!SStrCmpI(name, "LEFT", STORM_MAX_STR)) {
            SetConsoleKey(KEY_LEFT);
            return 0;
        }

        if (!SStrCmpI(name, "UP", STORM_MAX_STR)) {
            SetConsoleKey(KEY_UP);
            return 0;
        }

        if (!SStrCmpI(name, "RIGHT", STORM_MAX_STR)) {
            SetConsoleKey(KEY_RIGHT);
            return 0;
        }

        if (!SStrCmpI(name, "DOWN", STORM_MAX_STR)) {
            SetConsoleKey(KEY_DOWN);
            return 0;
        }

        if (!SStrCmpI(name, "INSERT", STORM_MAX_STR)) {
            SetConsoleKey(KEY_INSERT);
            return 0;
        }

        if (!SStrCmpI(name, "DELETE", STORM_MAX_STR)) {
            SetConsoleKey(KEY_DELETE);
            return 0;
        }

        if (!SStrCmpI(name, "HOME", STORM_MAX_STR)) {
            SetConsoleKey(KEY_HOME);
            return 0;
        }

        if (!SStrCmpI(name, "END", STORM_MAX_STR)) {
            SetConsoleKey(KEY_END);
            return 0;
        }

        if (!SStrCmpI(name, "PAGEUP", STORM_MAX_STR)) {
            SetConsoleKey(KEY_PAGEUP);
            return 0;
        }

        if (!SStrCmpI(name, "PAGEDOWN", STORM_MAX_STR)) {
            SetConsoleKey(KEY_PAGEDOWN);
            return 0;
        }

        if (!SStrCmpI(name, "NUMLOCK", STORM_MAX_STR)) {
            SetConsoleKey(KEY_NUMLOCK);
            return 0;
        }

        if (!SStrCmpI(name, "PRINTSCREEN", STORM_MAX_STR)) {
            key = KEY_PRINTSCREEN;
        }
    }

    SetConsoleKey(key);
    return 0;
}

// OFFSET: #0x0050FEA0
static int32_t RegisterCVar(lua_State* L, uint32_t flags) {
    if (!lua_isstring(L, 1)) {
        char function[1024];
        luaL_error(L, "Usage: %s(\"cvar\" [, default])", FrameScript_GetCurrentFunction(L, function, sizeof(function)));
    }

    const char* name = lua_tolstring(L, 1, NULL);
    const char* value = lua_tolstring(L, 2, NULL);

    if (!value) {
        value = "0";
    }

    CVar::Register(name, NULL, flags, value, NULL, 4, true, NULL, true);
    return 0;
}

// OFFSET: 0x008C1D40/0x140631200
static int32_t Script_RegisterCVar(lua_State* L) {
    return RegisterCVar(L, 0);
}

// OFFSET: 0x008C1FE0/0x1406314C0
static int32_t Script_Stuck(lua_State* L) {
    Spell_C_CastSpell(CGSpellBook::m_stuckSpell, NULL, WOWGUID(), false);
    return 0;
}

// OFFSET: 0x008C2500/0x140631D10
static int32_t Script_ReportBug(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: ReportBug(\"description\")");
        return 0;
    }

    ClientServices::Report(0, lua_tolstring(L, 1, NULL));
    return 0;
}

// OFFSET: 0x008C2550/0x140631D60
static int32_t Script_ReportSuggestion(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: ReportSuggestion(\"description\")");
        return 0;
    }

    ClientServices::Report(1, lua_tolstring(L, 1, NULL));
    return 0;
}

// OFFSET: 0x008C25C0/0x140631DD0
static int32_t Script_ForceLogout(lua_State* L) {
    ClientServices::Connection()->CharacterForceLogout();
    return 0;
}

// OFFSET: 0x008C26F0/0x140632050
static int32_t Script_RunScript(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        return 0;
    }

    const char* script = lua_tolstring(L, 1, NULL);

    if (script && *script) {
        FrameScript_Execute(script, script, g_macroTaint);
    }

    return 0;
}

// OFFSET: 0x008C2740/0x140632120
static int32_t Script_ReplaceTradeEnchant(lua_State* L) {
    Spell_C_TargetTradeItem();
    return 0;
}

// OFFSET: 0x0044BD50/0x140632130
static int32_t Script_GMRequestPlayerInfo(lua_State* L) {
    luaL_error(L, "Access Denied");
    return 0;
}

// OFFSET: 0x008C8280/0x140638210
static int32_t Script_GetCoinIcon(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetCoinIcon(amount)");
        return 0;
    }

    char icon[260];
    GetCurrencyIcon(static_cast<uint32_t>(lua_tonumber(L, 1)), icon, sizeof(icon));
    lua_pushstring(L, icon);
    return 1;
}

// OFFSET: 0x008C8300/0x1406382F0
static int32_t Script_GetCoinText(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetCoinText(amount [,separator])");
        return 0;
    }

    const char* separator = luaL_optlstring(L, 2, ", ", NULL);
    char text[1024];
    GetCurrencyText(static_cast<uint32_t>(lua_tonumber(L, 1)), text, sizeof(text), separator);
    lua_pushstring(L, text);
    return 1;
}

// OFFSET: 0x008C8390/0x140638400
static int32_t Script_GetCoinTextureString(lua_State* L) {
    int32_t height = 14;

    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetCoinText(amount, fontHeight)");
        return 0;
    }

    if (lua_isnumber(L, 2)) {
        height = static_cast<int32_t>(lua_tonumber(L, 2));
    }

    char text[1024];
    GetCurrencyTextureString(static_cast<uint32_t>(lua_tonumber(L, 1)), text, sizeof(text), height);
    lua_pushstring(L, text);
    return 1;
}

// OFFSET: 0x008C2750/0x140632150
static int32_t Script_ConfirmBindOnUse(lua_State* L) {
    CGItem_C::ConfirmBindOnUse();
    return 0;
}

// OFFSET: 0x008DA1F0/0x14064E000
static int32_t Script_GetItemFamily(lua_State* L) {
    WOWGUID none = WOWGUID();
    int32_t itemId = 0;

    if (lua_isnumber(L, 1)) {
        itemId = lua_tointeger(L, 1);
    } else if (lua_isstring(L, 1)) {
        itemId = GetItemIDByName(lua_tolstring(L, 1, NULL));
    }

    ItemStats_C* stats = g_itemDBCache.GetRecord(itemId, none, NULL, NULL, false);

    if (!stats) {
        return 0;
    }

    lua_pushnumber(L, stats->m_bagFamily);
    return 1;
}

// OFFSET: 0x008C2A10/0x140632480
static int32_t Script_IsCurrentItem(lua_State* L) {
    int32_t itemId;

    if (lua_isnumber(L, 1)) {
        itemId = lua_tointeger(L, 1);
    } else {
        if (!lua_isstring(L, 1)) {
            lua_pushnil(L);
            return 1;
        }

        itemId = GetItemIDByName(lua_tolstring(L, 1, NULL));
    }

    if (itemId && Spell_C_IsCurrentItem(itemId)) {
        lua_pushnumber(L, 1.0);
        return 1;
    }

    lua_pushnil(L);
    return 1;
}

// OFFSET: 0x008DA860/0x14064E7D0
static int32_t Script_IsEquippableItem(lua_State* L) {
    int32_t itemId = 0;

    if (lua_isnumber(L, 1)) {
        itemId = lua_tointeger(L, 1);
    } else if (lua_isstring(L, 1)) {
        itemId = GetItemIDByName(lua_tolstring(L, 1, NULL));
    }

    WOWGUID none = WOWGUID();
    ItemStats_C* stats = g_itemDBCache.GetRecord(itemId, none, NULL, NULL, false);

    if (stats && stats->m_inventoryType) {
        lua_pushnumber(L, 1.0);
        return 1;
    }

    lua_pushnil(L);
    return 1;
}

// OFFSET: 0x008C2AD0/0x140632540
static int32_t Script_GetAddOnInfo(lua_State* L) {
    const char* name;

    if (lua_isnumber(L, 1)) {
        name = GetAddOnByIndex(static_cast<uint32_t>(lua_tonumber(L, 1)) - 1);

        if (!name) {
            luaL_error(L, "AddOn index must be in the range of 1 to %d", GetNumAddOns());
            return 0;
        }
    } else {
        if (!lua_isstring(L, 1)) {
            luaL_error(L, "Usage: GetAddOnInfo(index or \"name\")");
            return 0;
        }

        name = lua_tolstring(L, 1, NULL);
    }

    lua_pushstring(L, name);
    const char* title = GetAddOnTitle(name);

    if (title) {
        lua_pushstring(L, title);
    } else {
        lua_pushnil(L);
    }

    const char* notes = GetAddOnNotes(name);

    if (notes) {
        lua_pushstring(L, notes);
    } else {
        lua_pushnil(L);
    }

    if (GetAddOnEnableState(name, ClientServices::GetCharacterName(), true)) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    if (IsAddOnLoaded(name)) {
        lua_pushnumber(L, 1.0);
        lua_pushnil(L);
    } else {
        UIADDON_LOADABLE_REASON reason = UIADDON_LOADABLE;
        UIADDON_LOADABLE_REASON dependencyReason = UIADDON_LOADABLE;

        if (IsAddOnLoadable(name, true, reason, dependencyReason, ClientServices::GetCharacterName())) {
            lua_pushnumber(L, 1.0);
        } else {
            lua_pushnil(L);
        }

        if (reason || dependencyReason) {
            char token[128];
            lua_pushstring(L, AddOnUnloadableToken(reason, dependencyReason, token, sizeof(token)));
        } else {
            lua_pushnil(L);
        }
    }

    lua_pushstring(L, AddOnSecurityToken(GetAddOnSecurity(name)));
    return 7;
}

// OFFSET: 0x008C2CC0/0x140632740
static int32_t Script_GetAddOnMetadata(lua_State* L) {
    const char* name;

    if (lua_isnumber(L, 1)) {
        name = GetAddOnByIndex(static_cast<uint32_t>(lua_tonumber(L, 1)) - 1);

        if (!name) {
            luaL_error(L, "AddOn index must be in the range of 1 to %d", GetNumAddOns());
        }
    } else if (!lua_isstring(L, 1) || !(name = lua_tolstring(L, 1, NULL))) {
        luaL_error(L, "Usage: GetAddOnMetadata(index or \"name\", \"variable\")");
    }

    if (!lua_isstring(L, 2)) {
        luaL_error(L, "Usage: GetAddOnMetadata(index or \"name\", \"variable\")");
    }

    lua_pushstring(L, GetAddOnMetadata(name, lua_tolstring(L, 2, NULL)));
    return 1;
}

// OFFSET: 0x008C2DC0/0x140632840
static int32_t Script_GetAddOnMemoryUsage(lua_State* L) {
    const char* name;

    if (lua_isnumber(L, 1)) {
        name = GetAddOnByIndex(static_cast<uint32_t>(lua_tonumber(L, 1)) - 1);

        if (!name) {
            luaL_error(L, "AddOn index must be in the range of 1 to %d", GetNumAddOns());
            return 0;
        }
    } else {
        if (!lua_isstring(L, 1)) {
            luaL_error(L, "Usage: GetAddOnMemoryUsage(index or \"name\")");
            return 0;
        }

        name = lua_tolstring(L, 1, NULL);
    }

    lua_pushnumber(L, lua_gcgetsize(L, GetAddOnName(name)) * 0.0009765625);
    return 1;
}

// OFFSET: 0x008C2EE0/0x140632950
static int32_t Script_GetAddOnCPUUsage(lua_State* L) {
    const char* name;

    if (lua_isnumber(L, 1)) {
        name = GetAddOnByIndex(static_cast<uint32_t>(lua_tonumber(L, 1)) - 1);

        if (!name) {
            luaL_error(L, "AddOn index must be in the range of 1 to %d", GetNumAddOns());
            return 0;
        }
    } else {
        if (!lua_isstring(L, 1)) {
            luaL_error(L, "Usage: GetAddOnCPUUsage(index or \"name\")");
            return 0;
        }

        name = lua_tolstring(L, 1, NULL);
    }

    lua_pushnumber(L, lua_gcgettime(L, GetAddOnName(name)));
    return 1;
}

// OFFSET: 0x008C30C0/0x140632B30
static int32_t Script_EnableAddOn(lua_State* L) {
    const char* name;

    if (lua_isnumber(L, 1)) {
        name = GetAddOnByIndex(static_cast<uint32_t>(lua_tonumber(L, 1)) - 1);

        if (!name) {
            luaL_error(L, "AddOn index must be in the range of 1 to %d", GetNumAddOns());
            return 0;
        }
    } else {
        if (!lua_isstring(L, 1)) {
            luaL_error(L, "Usage: EnableAddOn(index or \"name\")");
            return 0;
        }

        name = lua_tolstring(L, 1, NULL);
    }

    SetAddOnEnableState(name, ClientServices::GetCharacterName(), 2);
    return 0;
}

// OFFSET: 0x008C3180
static int32_t Script_EnableAllAddOns(lua_State* L) {
    for (uint32_t i = 0; i < GetNumAddOns(); i++) {
        SetAddOnEnableState(GetAddOnByIndex(i), ClientServices::GetCharacterName(), 2);
    }

    return 0;
}

// OFFSET: 0x008C31C0/0x140632C40
static int32_t Script_DisableAddOn(lua_State* L) {
    const char* name;

    if (lua_isnumber(L, 1)) {
        name = GetAddOnByIndex(static_cast<uint32_t>(lua_tonumber(L, 1)) - 1);

        if (!name) {
            luaL_error(L, "AddOn index must be in the range of 1 to %d", GetNumAddOns());
            return 0;
        }
    } else {
        if (!lua_isstring(L, 1)) {
            luaL_error(L, "Usage: DisableAddOn(index or \"name\")");
            return 0;
        }

        name = lua_tolstring(L, 1, NULL);
    }

    SetAddOnEnableState(name, ClientServices::GetCharacterName(), 0);
    return 0;
}

// OFFSET: 0x008C3280/0x140632D00
static int32_t Script_DisableAllAddOns(lua_State* L) {
    for (uint32_t i = 0; i < GetNumAddOns(); i++) {
        SetAddOnEnableState(GetAddOnByIndex(i), ClientServices::GetCharacterName(), 0);
    }

    return 0;
}

// OFFSET: 0x008C32E0/0x140632D70
static int32_t Script_IsAddOnLoadOnDemand(lua_State* L) {
    const char* name;

    if (lua_isnumber(L, 1)) {
        name = GetAddOnByIndex(static_cast<uint32_t>(lua_tonumber(L, 1)) - 1);

        if (!name) {
            luaL_error(L, "AddOn index must be in the range of 1 to %d", GetNumAddOns());
            return 0;
        }
    } else {
        if (!lua_isstring(L, 1)) {
            luaL_error(L, "Usage: IsAddOnLoadOnDemand(index or \"name\")");
            return 0;
        }

        name = lua_tolstring(L, 1, NULL);
    }

    if (IsAddOnLoadOnDemand(name)) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// PROVEN: 67f3440e IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
static void VoicePushToTalkNoop() {
}

// OFFSET: 0x008C34F0/0x140632F70
static int32_t Script_VoicePushToTalkStart(lua_State* L) {
    VoicePushToTalkNoop();
    return 0;
}

// OFFSET: 0x008C34F0/0x140632F70
static int32_t Script_VoicePushToTalkStop(lua_State* L) {
    VoicePushToTalkNoop();
    return 0;
}

// OFFSET: 0x008C35A0/0x140633060
static int32_t Script_GetNumDeclensionSets(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: GetNumDeclensionSets(\"name\", gender)");
        return 0;
    }

    int32_t sex = 2;
    const char* name = lua_tolstring(L, 1, NULL);

    if (lua_isnumber(L, 2)) {
        int32_t value = lua_tointeger(L, 2);

        for (int32_t i = 0; i < 3; i++) {
            if (s_unitSexValues[i] == value) {
                sex = i;
                break;
            }
        }
    }

    lua_pushnumber(L, GetNumNameDeclensionSets(s_locale, name, sex));
    return 1;
}

// OFFSET: 0x008C3650/0x140633120
static int32_t Script_DeclineName(lua_State* L) {
    if (!lua_isstring(L, 1) || !lua_isnumber(L, 3)) {
        luaL_error(L, "Usage: DeclineName(\"name\", gender, declensionSet)");
        return 0;
    }

    int32_t sex = 2;
    const char* name = lua_tolstring(L, 1, NULL);

    if (lua_isnumber(L, 2)) {
        int32_t value = lua_tointeger(L, 2);

        for (int32_t i = 0; i < 3; i++) {
            if (s_unitSexValues[i] == value) {
                sex = i;
                break;
            }
        }
    }

    char declensions[5][1024];

    if (GetNameDeclensions(s_locale, declensions[0], name, sex, static_cast<int32_t>(lua_tonumber(L, 3)) - 1)) {
        for (uint32_t i = 0; i < 5; i++) {
            lua_pushstring(L, declensions[i]);
        }

        return 5;
    }

    for (uint32_t i = 0; i < 5; i++) {
        lua_pushnil(L);
    }

    return 5;
}

// OFFSET: 0x008C3760/0x140633260
static int32_t Script_UploadSettings(lua_State* L) {
    UploadAccountData();
    return 0;
}

// OFFSET: 0x008C3770/0x140633270
static int32_t Script_DownloadSettings(lua_State* L) {
    DownloadAccountData();
    return 0;
}

// OFFSET: #0x00515900
static void ResurrectNameQueryCallback(int32_t id, const WOWGUID& guid, void* param, bool success) {
    WOWGUID offerer = CGPlayer_C::s_resurrectOfferer;
    NameCache* name = g_nameDBCache.GetRecord(CGPlayer_C::s_resurrectOfferer.m_guid, offerer, NULL, NULL, false);

    if (name) {
        FrameScript_SignalEvent(SCRIPT_EVENT_RESURRECT_REQUEST, "%s", name->m_name);
    }
}

// OFFSET: 0x008C7520/0x140637390
static int32_t Script_ResurrectGetOfferer(lua_State* L) {
    WOWGUID offerer = CGPlayer_C::s_resurrectOfferer;
    NameCache* name;

    if (CGPlayer_C::s_resurrectOfferer && (name = g_nameDBCache.GetRecord(CGPlayer_C::s_resurrectOfferer.m_guid, offerer, &ResurrectNameQueryCallback, NULL, false))) {
        lua_pushstring(L, name->m_name);
        return 1;
    }

    lua_pushnil(L);
    return 1;
}

// OFFSET: 0x008C7590/0x140637400
static int32_t Script_ResurrectHasSickness(lua_State* L) {
    if (CGPlayer_C::s_resurrectOfferSickness) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: 0x008C75D0/0x140637440
static int32_t Script_ResurrectHasTimer(lua_State* L) {
    if (CGBattlefieldInfo::m_instanceType == 4 || !CGPlayer_C::s_resurrectOfferUseTimer) {
        lua_pushnil(L);
        return 1;
    }

    lua_pushnumber(L, 1.0);
    return 1;
}

// OFFSET: 0x008CCC30/0x14063E780
static int32_t Script_GuildSetMOTD(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: GuildSetMOTD(\"message\")");
        return 0;
    }

    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_GUILD_MOTD));
    const char* text = lua_tolstring(L, 1, NULL);
    char motd[516];
    motd[0] = 0;

    if (text && *text) {
        ScrubString(text, motd, 129, sizeof(motd), 1);
    }

    msg.PutString(motd);
    msg.Finalize();
    ClientServices::Send2(&msg);
    return 0;
}

// OFFSET: 0x008C7D10/0x140637CD0
static int32_t Script_GetInstanceLockTimeRemaining(lua_State* L) {
    int32_t remaining;
    uint32_t completedMask;
    uint32_t extending;

    if (CGGameUI::m_instanceLockTime && (remaining = CGGameUI::m_instanceLockTime - OsGetAsyncTimeMs()) > 0) {
        completedMask = CGGameUI::m_instanceLockCompletedMask;
        extending = CGGameUI::m_instanceLockExtending;
    } else {
        remaining = 0;
        completedMask = 0;
        extending = 0;
    }

    lua_pushnumber(L, static_cast<uint32_t>(remaining) / 1000);
    lua_pushboolean(L, extending);
    uint32_t total;
    uint32_t completed;
    DungeonEncounterCountMatchingRows(CGGameUI::m_iCurrentMapID, CGGameUI::m_instanceDifficulty, completedMask, &total, &completed);
    lua_pushnumber(L, total);
    lua_pushnumber(L, completed);
    return 4;
}

// OFFSET: 0x008C7DD0/0x140637D90
static int32_t Script_GetInstanceLockTimeRemainingEncounter(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: GetInstanceLockTimeRemainingEncounter(encounterIndex)");
        return 0;
    }

    double index = lua_tonumber(L, 1);
    uint32_t remaining;
    uint32_t completedMask;
    uint32_t extending;
    CGGameUI::GetInstanceLockInfo(&remaining, &completedMask, &extending);

    if (remaining) {
        return PushDungeonEncounterInfo(L, CGGameUI::m_iCurrentMapID, CGGameUI::m_instanceDifficulty, static_cast<int32_t>(index - 1.0), completedMask);
    }

    return PushDungeonEncounterInfo(L, 0, 0, 0, 0);
}

// PROVEN: 10a9e962 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
static void SummonConfirmNameCallback(int32_t id, const WOWGUID& guid, void* param, bool success) {
}

// OFFSET: 0x008C7EB0/0x140631E70
static int32_t Script_GetSummonConfirmSummoner(lua_State* L) {
    WOWGUID summoner = CGGameUI::m_summonConfirmGUID;
    NameCache* name = g_nameDBCache.GetRecord(CGGameUI::m_summonConfirmGUID.m_guid, summoner, &SummonConfirmNameCallback, NULL, false);

    if (name) {
        lua_pushstring(L, name->m_name);
    } else {
        lua_pushstring(L, "");
    }

    return 1;
}

// OFFSET: 0x008C8240/0x1406320E0
static int32_t Script_ReplaceEnchant(lua_State* L) {
    CGItem_C* item = static_cast<CGItem_C*>(ClntObjMgrObjectPtr(CGGameUI::m_lastItemSpellTarget, TYPEMASK_ITEM, __FILE__, __LINE__));

    if (item) {
        Spell_C_HandleSpriteClick(item, NULL);
    }

    return 0;
}

// OFFSET: 0x008C86F0/0x140638830
static int32_t Script_GetGMTicketCategories(lua_State* L) {
    int32_t count = g_gmTicketCategoryDB.m_numRecords;
    int32_t results = 2 * count;
    lua_checkstack(L, results);

    for (int32_t i = 0; i < count; i++) {
        GMTicketCategoryRec* category = g_gmTicketCategoryDB.GetRecordByIndex(i);
        lua_pushnumber(L, category->m_id);
        lua_pushstring(L, category->m_category);
    }

    return results;
}

// OFFSET: 0x008D9FC0/0x14064DCE0
static int32_t Script_GetItemUniqueness(lua_State* L) {
    TooltipExtendedItemInfo linkInfo;
    linkInfo.Clear();
    int32_t itemId;

    if (lua_isnumber(L, 1)) {
        itemId = static_cast<int32_t>(lua_tonumber(L, 1));
    } else {
        if (!lua_isstring(L, 1)) {
            luaL_error(L, "Usage: GetItemUniqueness(itemID|\"name\"|\"itemlink\")");
            return 0;
        }

        const char* text = lua_tolstring(L, 1, NULL);

        if (SStrStr(text, "item:")) {
            itemId = linkInfo.GetItemIdFromLink(text);
        } else {
            itemId = GetItemIDByName(lua_tolstring(L, 1, NULL));
        }
    }

    WOWGUID none = WOWGUID::Make(0);
    ItemRecSparse* stats = g_itemSparseDB.GetRecord(itemId, none, NULL, NULL, false);

    if (!stats) {
        return 0;
    }

    if (stats->m_flags[0] & 0x80000) {
        lua_pushnumber(L, -1.0);
        lua_pushnumber(L, 1.0);
        return 2;
    }

    uint32_t limitCategory = stats->m_itemLimitCategory;

    if (!limitCategory) {
        return 0;
    }

    ItemLimitCategoryRec* category = g_itemLimitCategoryDB.GetRecord(limitCategory);

    if (!category) {
        return 0;
    }

    lua_pushnumber(L, limitCategory);
    lua_pushnumber(L, category->m_quantity);
    return 2;
}

// OFFSET: 0x008DA110/0x14064DF00
static int32_t Script_GetItemIcon(lua_State* L) {
    int32_t itemId = 0;

    if (lua_isnumber(L, 1)) {
        itemId = lua_tointeger(L, 1);
    } else if (lua_isstring(L, 1)) {
        itemId = GetItemIDByName(lua_tolstring(L, 1, NULL));
    }

    ItemRec* item = g_itemDB.GetRecord(itemId, WOWGUID(), NULL, NULL, false);

    if (!item) {
        return 0;
    }

    const char* path = ClientDBStringLookup(3);
    const char* separator = *path ? "\\" : "";
    char icon[260];
    SStrPrintf(icon, sizeof(icon), "%s%s%s", path, separator, CGItem_C::GetInventoryArt(item->m_displayInfoID));
    lua_pushstring(L, icon);
    return 1;
}

// OFFSET: 0x008C8970/0x140638AE0
static int32_t Script_IsHelpfulItem(lua_State* L) {
    int32_t itemId = 0;

    if (lua_isnumber(L, 1)) {
        itemId = lua_tointeger(L, 1);
    } else if (lua_isstring(L, 1)) {
        itemId = GetItemIDByName(lua_tolstring(L, 1, NULL));
    }

    CGItem_C* item = CGActionBar::FindUseEnchantItemPtr(itemId);
    int32_t spellId = item ? item->GetEnchantmentSpell(0) : CGItem_C::GetUseSpell(itemId);
    const SpellRec* spell = NULL;

    if ((spell = g_spellDB.GetRecord(spellId)) && SpellFaction(spell) == 1) {
        lua_pushnumber(L, 1.0);
        return 1;
    }

    lua_pushnil(L);
    return 1;
}

// OFFSET: 0x008C8A40/0x140638BC0
static int32_t Script_IsHarmfulItem(lua_State* L) {
    int32_t itemId = 0;

    if (lua_isnumber(L, 1)) {
        itemId = lua_tointeger(L, 1);
    } else if (lua_isstring(L, 1)) {
        itemId = GetItemIDByName(lua_tolstring(L, 1, NULL));
    }

    CGItem_C* item = CGActionBar::FindUseEnchantItemPtr(itemId);
    int32_t spellId = item ? item->GetEnchantmentSpell(0) : CGItem_C::GetUseSpell(itemId);
    const SpellRec* spell = NULL;

    if ((spell = g_spellDB.GetRecord(spellId)) && SpellFaction(spell) == 2) {
        lua_pushnumber(L, 1.0);
        return 1;
    }

    lua_pushnil(L);
    return 1;
}

// OFFSET: 0x008DA710/0x14064E640
static int32_t Script_IsConsumableItem(lua_State* L) {
    int32_t itemId = 0;

    if (lua_isnumber(L, 1)) {
        itemId = lua_tointeger(L, 1);
    } else if (lua_isstring(L, 1)) {
        itemId = GetItemIDByName(lua_tolstring(L, 1, NULL));
    }

    ItemRecSparse* rec = g_itemSparseDB.GetRecord(itemId, WOWGUID(), NULL, NULL, false);

    if (!rec) {
        lua_pushnil(L);
        return 1;
    }

    if (rec->m_inventoryType != 25) {
        uint32_t i;

        for (i = 0; i < 5; i++) {
            if (!rec->m_spellID[i] || rec->m_spellTrigger[i]) {
                continue;
            }

            const SpellRec* spell = g_spellDB.GetRecord(rec->m_spellID[i]);

            if (!spell) {
                continue;
            }

            int32_t reagents = 0;

            if (spell->m_spellReagentsID) {
                const SpellReagentsRec* reagent = g_spellReagentsDB.GetRecord(spell->m_spellReagentsID);

                if (reagent) {
                    for (uint32_t j = 0; j < 8; j++) {
                        if (reagent->m_reagent[j] && reagent->m_reagentCount[j] > 0) {
                            reagents = 1;
                            break;
                        }
                    }
                }
            }

            if (rec->m_spellCharges[i] < 0 || reagents) {
                break;
            }
        }

        if (i >= 5) {
            lua_pushnil(L);
            return 1;
        }
    }

    lua_pushnumber(L, 1.0);
    return 1;
}

// OFFSET: #0x00510EF0
static int32_t GetItemSpell(int32_t itemId) {
    CGItem_C* item = CGActionBar::FindUseEnchantItemPtr(itemId);

    if (item) {
        return item->GetEnchantmentSpell(0);
    }

    return CGItem_C::GetUseSpell(itemId);
}

// OFFSET: 0x008DA910/0x14064E890
static int32_t Script_IsDressableItem(lua_State* L) {
    int32_t itemId = 0;

    if (lua_isnumber(L, 1)) {
        itemId = lua_tointeger(L, 1);
    } else if (lua_isstring(L, 1)) {
        itemId = GetItemIDByName(lua_tolstring(L, 1, NULL));
    }

    ItemRecSparse* rec = g_itemSparseDB.GetRecord(itemId, WOWGUID(), NULL, NULL, false);

    if (!rec) {
        lua_pushnil(L);
        return 1;
    }

    if (rec->m_inventoryType) {
        lua_pushnumber(L, 1.0);
        return 1;
    }

    for (uint32_t i = 0; i < 5; i++) {
        if (!rec->m_spellID[i]) {
            continue;
        }

        const SpellEffectRec* effect = SpellGetEffect(rec->m_spellID[i], 0);

        if (!effect || effect->m_effect != 36) {
            continue;
        }

        int32_t learnedId = effect->m_effectTriggerSpell;

        if (!learnedId) {
            for (uint32_t j = 0; j < 5; j++) {
                if (rec->m_spellTrigger[j] == 6) {
                    learnedId = rec->m_spellID[j];
                    break;
                }
            }
        }

        const SpellEffectRec* learned = SpellGetEffect(learnedId, 0);

        if (!learned || learned->m_effect != 24) {
            continue;
        }

        ItemRecSparse* created = g_itemSparseDB.GetRecord(learned->m_effectItemType, WOWGUID(), NULL, NULL, false);

        if (created && created->m_inventoryType) {
            lua_pushnumber(L, 1.0);
            return 1;
        }
    }

    lua_pushnil(L);
    return 1;
}

// OFFSET: 0x008C8B10/0x140638CA0
static int32_t Script_GetFrameCPUUsage(lua_State* L) {
    if (lua_type(L, 1) != LUA_TTABLE) {
        luaL_error(L, "Usage: GetFrameCPUUsage(frame[, includeChildren])");
        return 0;
    }

    lua_rawgeti(L, 1, 0);
    CSimpleFrame* frame = static_cast<CSimpleFrame*>(lua_touserdata(L, -1));
    lua_settop(L, -2);

    if (!frame) {
        luaL_error(L, "GetFrameCPUUsage(): Couldn't find 'this' in frame object");
        return 0;
    }

    if (!frame->IsA(CSimpleFrame::GetObjectType())) {
        luaL_error(L, "GetFrameCPUUsage(): Wrong object type, expected frame");
        return 0;
    }

    double time = 0.0;
    uint32_t count = 0;
    frame->GetScriptTime(L, &time, &count, StringToBOOL(L, 2, 1));
    lua_pushnumber(L, time);
    lua_pushnumber(L, count);
    return 2;
}

// OFFSET: 0x008C8C30/0x140638DF0
static int32_t Script_GetAddOnDependencies(lua_State* L) {
    const char* name;

    if (lua_isnumber(L, 1)) {
        name = GetAddOnByIndex(static_cast<uint32_t>(lua_tonumber(L, 1)) - 1);

        if (!name) {
            luaL_error(L, "AddOn index must be in the range of 1 to %d", GetNumAddOns());
            return 0;
        }
    } else {
        if (!lua_isstring(L, 1)) {
            luaL_error(L, "Usage: GetAddOnDependencies(index or \"name\")");
            return 0;
        }

        name = lua_tolstring(L, 1, NULL);
    }

    TSGrowableArray<char*>* dependencies = GetAddonDeps(name);

    if (!dependencies) {
        return 0;
    }

    lua_checkstack(L, dependencies->m_count);

    for (uint32_t i = 0; i < dependencies->m_count; i++) {
        lua_pushstring(L, dependencies->m_data[i]);
    }

    return dependencies->m_count;
}

// OFFSET: 0x008C8E10/0x140639010
static int32_t Script_GetMirrorTimerProgress(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: GetMirrorTimerInfo(\"timer\")");
        return 0;
    }

    uint32_t index = CGGameUI::MirrorTimerToIndex(lua_tolstring(L, 1, NULL));

    if (index > 2) {
        luaL_error(L, "ERROR: Invalid timer (Ex: \"BREATH\"");
        return 0;
    }

    MirrorTimerInfo* timer = &CGGameUI::m_mirrorTimerInfo[index];

    if (!timer) {
        SErrDisplayAppFatalMessage("", "", 0);
    }

    lua_pushnumber(L, timer->m_value + timer->m_scale * static_cast<int32_t>(OsGetAsyncTimeMs() - timer->unk_0018));
    return 1;
}

// OFFSET: 0x008CC3A0/0x14063D2C0
static int32_t Script_EquipCursorItem(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: EquipCursorItem(slot)");
        return 0;
    }

    CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();

    if (!player) {
        return 0;
    }

    WOWGUID item = CGGameUI::m_cursorItem;
    WOWGUID container = CGGameUI::m_cursorItemContainer;
    uint32_t sourceSlot = CGGameUI::m_cursorItemSlot;

    if (!item) {
        return 0;
    }

    int32_t slot = static_cast<int32_t>(lua_tonumber(L, 1));
    uint32_t index = slot - 1;

    if (slot != 256 && slot) {
        WOWGUID current;
        current.m_guid = player->m_bag.GetItemGuid(index);

        if (item == current) {
            CGGameUI::ClearCursor(1, 1);
            return 0;
        }

        if (player->ValidateSlot(index, item)) {
            player->SwapItems(item, container, sourceSlot, player->m_obj->m_guid, index, 0);
            CGGameUI::LockItem(current);
            return 0;
        }

        if (container == player->m_obj->m_guid && sourceSlot < 19) {
            CGGameUI::ClearCursor(1, 1);
            return 0;
        }
    }

    player->AutoEquipCursorItem(1);
    return 0;
}

// OFFSET: 0x008CC500/0x14063D5C0
static int32_t Script_EquipPendingItem(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: EquipPendingItem(index)");
        return 0;
    }

    CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();

    if (player) {
        player->ClearPendingEquip(static_cast<uint32_t>(lua_tonumber(L, 1)), 1);
    }

    return 0;
}

// OFFSET: 0x008CC590/0x14063D650
static int32_t Script_CancelPendingEquip(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: CancelPendingEquip(index)");
        return 0;
    }

    CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();

    if (player) {
        player->ClearPendingEquip(static_cast<uint32_t>(lua_tonumber(L, 1)), 0);
    }

    return 0;
}

// OFFSET: 0x008CC620/0x14063D6E0
static int32_t Script_AttackTarget(lua_State* L) {
    CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();

    if (player) {
        player->OnAttackIconPressed(WOWGUID(), 0);
    }

    return 0;
}

// OFFSET: 0x008CC690/0x14063D770
static int32_t Script_ToggleSheath(lua_State* L) {
    CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();

    if (player) {
        player->ToggleSheathe();
    }

    return 0;
}

// OFFSET: 0x008CC720/0x14063DC30
static int32_t Script_CanShowResetInstances(lua_State* L) {
    if (CGGameUI::CanResetInstance()) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: 0x008CCAD0/0x14063E080
static int32_t Script_HasSoulstone(lua_State* L) {
    CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();

    if (!player || player->m_unit->m_unitFieldHealth > 0) {
        lua_pushnil(L);
        return 1;
    }

    if (!player->m_player->m_playerSelfResSpell) {
        CGItem_C* soulstone = player->GetSoulstone();

        if (soulstone) {
            char name[1024];
            soulstone->GetFullItemName(name, sizeof(name));
            lua_pushstring(L, name);
            return 1;
        }

        lua_pushnil(L);
        return 1;
    }

    const SpellRec* spell = NULL;
    lua_pushstring(L, (spell = g_spellDB.GetRecord(player->m_player->m_playerSelfResSpell)) ? spell->m_name : "UNKNOWN");
    return 1;
}

// OFFSET: 0x008D3E30/0x14063E1C0
static int32_t Script_UseSoulstone(lua_State* L) {
    CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();

    if (!player) {
        return 0;
    }

    if (player->m_player->m_playerSelfResSpell) {
        if (player->CheckSelfResurrect()) {
            WDataStore msg;
            msg.Put(static_cast<uint32_t>(CMSG_SELF_RES));
            msg.Finalize();
            ClientServices::Send2(&msg);
        }

        return 0;
    }

    CGItem_C* soulstone = player->GetSoulstone();

    if (soulstone) {
        Spell_C_CancelPlayerSpells();
        WOWGUID target = WOWGUID();
        soulstone->UseItem(&target, 0);
    }

    return 0;
}

// OFFSET: 0x008CCDF0/0x14063E8D0
static int32_t Script_ConfirmSummon(lua_State* L) {
    CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();

    if (player->IsDeadOrGhost()) {
        return 0;
    }

    uint32_t flags = player->m_unit->m_unitFieldFlags;

    if ((flags & 0x40000) || (flags & 0x800000) || (flags & 0x400000) || (flags & 0x80000)) {
        return 0;
    }

    WDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_SUMMON_RESPONSE));
    msg.Put(CGGameUI::m_summonConfirmGUID.m_guid);
    msg.Put(static_cast<uint8_t>(1));
    msg.Finalize();
    ClientServices::Send2(&msg);
    return 0;
}

// OFFSET: 0x008CCF20/0x14063EAC0
static int32_t Script_CheckInteractDistance(lua_State* L) {
    if (!lua_isstring(L, 1) || !lua_isnumber(L, 2)) {
        luaL_error(L, "Usage: CheckInteractDistance(\"unit\", distIndex)");
        return 0;
    }

    static float s_interactDistances[4] = { s_inspectDistSq, s_playerInteractDistSq, s_duelDistSq, s_followDistSq };
    CGUnit_C* unit = GetUnitFromName(lua_tolstring(L, 1, NULL));
    CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();
    uint32_t index = static_cast<uint32_t>(lua_tonumber(L, 2)) - 1;

    if (player && unit && index < 4) {
        C3Vector unitPosition = unit->GetPosition();
        C3Vector delta = player->GetPosition() - unitPosition;

        if (delta.y * delta.y + delta.x * delta.x + delta.z * delta.z < s_interactDistances[index]) {
            lua_pushnumber(L, 1.0);
            return 1;
        }
    }

    lua_pushnil(L);
    return 1;
}

// OFFSET: 0x008CD410/0x14063F050
static int32_t Script_AcceptXPLoss(lua_State* L) {
    CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();

    if (player) {
        player->TalkToSpiritHealer(WOWGUID());
    }

    return 0;
}

// OFFSET: 0x008CDB00/0x14063F880
static int32_t Script_DropItemOnUnit(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: DropItemOnUnit(\"unit\")");
        return 0;
    }

    WOWGUID cursor = CGGameUI::m_cursorItemType == 1 ? CGGameUI::m_cursorItem : WOWGUID();
    CGItem_C* item = static_cast<CGItem_C*>(ClntObjMgrObjectPtr(cursor, TYPEMASK_ITEM, __FILE__, __LINE__));

    if (!item) {
        return 0;
    }

    CGUnit_C* unit = GetUnitFromName(lua_tolstring(L, 1, NULL));

    if (!unit || !(unit->m_obj->m_type & TYPEMASK_UNIT)) {
        return 0;
    }

    if (unit->m_obj->m_type & TYPEMASK_PLAYER) {
        CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();

        if (unit->m_obj->m_guid == ClntObjMgrGetActivePlayer()) {
            if (!player) {
                SErrDisplayAppFatalMessage("", "", 0);
            }

            player->AutoEquipCursorItem(0);
        } else if (player && player->CanCooperate(unit)) {
            if (unit->m_obj->m_guid == CGTradeInfo::m_tradingPlayer) {
                CGTradeInfo::AddCursorItem(item->IsBound());
            } else if (!CGTradeInfo::m_tradingPlayer) {
                CGTradeInfo::InitiateTrade(unit->m_obj->m_guid, 1);
            }
        }
    }

    if (unit->m_obj->m_guid == CGPetInfo::GetPet(0) && Spell_C_HandleItemDrop(item, unit)) {
        CGGameUI::ClearCursor(1, 1);
    }

    return 0;
}

// OFFSET: 0x008CDC70/0x14063F9F0
static int32_t Script_ConfirmTalentWipe(lua_State* L) {
    CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();

    if (player) {
        player->TalkToTalentMaster(WOWGUID(), 0);
    }

    return 0;
}

// OFFSET: 0x008CDCB0/0x14063FA40
static int32_t Script_ConfirmBinder(lua_State* L) {
    CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();

    if (player) {
        player->TalkToBinder(WOWGUID());
    }

    return 0;
}

// OFFSET: 0x008CDDD0/0x14063FB70
static int32_t Script_ShowHelm(lua_State* L) {
    CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();

    if (!player) {
        return 0;
    }

    if (StringToBOOL(L, 1, 1)) {
        player->ShowHelm();
        return 0;
    }

    player->HideHelm();
    return 0;
}

// OFFSET: 0x008CDE70/0x14063FC30
static int32_t Script_ShowCloak(lua_State* L) {
    CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();

    if (!player) {
        return 0;
    }

    if (StringToBOOL(L, 1, 1)) {
        player->ShowCloak();
        return 0;
    }

    player->HideCloak();
    return 0;
}

// OFFSET: 0x008D9E30/0x14064DA20
static int32_t Script_GetItemGem(lua_State* L) {
    int32_t enchantId = 0;

    if (!lua_isstring(L, 1) || !lua_isnumber(L, 2)) {
        luaL_error(L, "Usage: GetItemGem(\"name\"|\"itemlink\", index)");
        return 0;
    }

    uint32_t index = static_cast<int32_t>(lua_tonumber(L, 2)) - 1;

    if (index > 2) {
        return 0;
    }

    const char* text = lua_tolstring(L, 1, NULL);

    if (SStrStr(text, "item:")) {
        TooltipExtendedItemInfo info;
        info.Clear();
        info.GetItemIdFromLink(text);
        enchantId = info.m_enchants[index + 2];
    } else {
        CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();

        if (!player) {
            return 0;
        }

        WOWGUID container;
        uint32_t slot;
        CGItem_C* item = player->m_bag.GetItemIdByName(lua_tolstring(L, 1, NULL), container, slot, 0);

        if (item) {
            enchantId = item->GetEnchantmentId(index + 2);
        }
    }

    int32_t gemId = 0;
    SpellItemEnchantmentRec* enchant = g_spellItemEnchantmentDB.GetRecord(enchantId);

    if (enchant) {
        gemId = enchant->m_srcItemID;
    }

    WOWGUID none = WOWGUID::Make(0);
    ItemRecSparse* stats = g_itemSparseDB.GetRecord(gemId, none, NULL, NULL, false);

    if (!stats) {
        return 0;
    }

    char name[1024];
    CGItem_C::BuildItemName(name, sizeof(name), gemId, 0);
    lua_pushstring(L, name);
    lua_pushstring(L, CGTooltip::GetItemHyperlinkString(gemId, stats->m_quality, 0, 0, 0, 0));
    return 2;
}

// OFFSET: 0x008DA290/0x14064E0B0
static int32_t Script_GetItemCount(lua_State* L) {
    int32_t includeBank = StringToBOOL(L, 2, 0);
    int32_t includeCharges = StringToBOOL(L, 3, 0);
    int32_t itemId = 0;

    if (lua_isnumber(L, 1)) {
        itemId = lua_tointeger(L, 1);
    } else if (lua_isstring(L, 1)) {
        itemId = GetItemIDByName(lua_tolstring(L, 1, NULL));
    }

    CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();
    double count;

    if (player) {
        uint32_t flags = 0;

        if (includeBank) {
            flags = 8;
        }

        if (includeCharges) {
            flags |= 0x80;
        }

        int32_t total = player->m_bag.GetItemTypeCount(itemId, flags);

        if (total && includeCharges) {
            WOWGUID none = WOWGUID();
            ItemStats_C* stats = g_itemDBCache.GetRecord(itemId, none, NULL, NULL, false);

            if (stats && stats->m_stackable <= 1) {
                CGItem_C* item = CGActionBar::FindUseEnchantItemPtr(itemId);
                int32_t charges = item ? item->GetUsableSpellCharges() : CGItem_C::NumBaseCharges(itemId);

                if (charges < 0) {
                    total = player->GetBag()->GetItemTypeCharges(itemId, 0);
                }

                int32_t spellId = item ? item->GetEnchantmentSpell(0) : CGItem_C::GetUseSpell(itemId);
                int32_t uses = Spell_C_GetSpellReagentUses(spellId);

                if (uses) {
                    total = uses;
                }
            }
        }

        count = total;
    } else {
        count = 0.0;
    }

    lua_pushnumber(L, count);
    return 1;
}

// OFFSET: 0x008DA4E0/0x14064E350
static int32_t Script_IsUsableItem(lua_State* L) {
    int32_t itemId = 0;

    if (lua_isnumber(L, 1)) {
        itemId = lua_tointeger(L, 1);
    } else if (lua_isstring(L, 1)) {
        itemId = GetItemIDByName(lua_tolstring(L, 1, NULL));
    }

    bool usable = false;
    bool noMana = false;
    CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();

    if (player && itemId && player->m_bag.GetItemTypeCount(itemId, 0) && !Spell_C_NeedsCooldownEvent(itemId)) {
        ItemRecSparse* rec = g_itemSparseDB.GetRecord(itemId, WOWGUID(), NULL, NULL, false);
        bool check = true;

        if (rec) {
            bool arena = false;

            if (CGGameUI::IsInArenaOrRatedBattleground(&arena) && arena && !((rec->m_flags[0] & 0x200000) && (rec->m_flags[1] & 0x8000))) {
                CGItem_C* item = CGActionBar::FindUseEnchantItemPtr(itemId);
                uint32_t cooldown;
                uint32_t categoryCooldown;
                uint32_t category;
                int32_t charges;

                if (item) {
                    charges = item->GetUsableSpellCharges();
                    item->GetSpellCooldownInfo(cooldown, categoryCooldown, category);
                } else {
                    charges = CGItem_C::NumBaseCharges(itemId);
                    CGItem_C::GetCooldowns(itemId, cooldown, categoryCooldown, category);
                }

                Spell_C_ModifyCategoryRecovery(reinterpret_cast<int32_t&>(categoryCooldown), category, 1);

                if (charges < 0 || static_cast<int32_t>(cooldown) > 900000 || static_cast<int32_t>(categoryCooldown) > 900000) {
                    check = false;
                }
            }
        }

        if (check) {
            const SpellRec* spell = g_spellDB.GetRecord(GetItemSpell(itemId));

            if (!spell) {
                usable = true;
            } else if (Spell_C_IsUsableAction(player, spell, WOWGUID(), rec, false)) {
                int32_t powerType;
                int32_t requirement;
                usable = Spell_C_HaveSpellPower(player, spell, powerType, requirement);
                noMana = !usable;
            }
        }
    }

    if (usable) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    if (noMana) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 2;
}

// OFFSET: #0x005176F0
int32_t CanEquipInSlotCallback(const CGItem_C* item, void* param) {
    WOWGUID none = WOWGUID();
    ItemStats_C* stats = g_itemDBCache.GetRecord(item->m_obj->m_entryID, none, NULL, NULL, false);
    return stats && static_cast<int32_t>(stats->m_inventoryType) == *static_cast<int32_t*>(param);
}

// OFFSET: #0x00517740
int32_t FindItemClassTypeCallback(const CGItem_C* item, void* param) {
    const char* type = static_cast<const char*>(param);
    WOWGUID none = WOWGUID();
    ItemStats_C* stats = g_itemDBCache.GetRecord(item->m_obj->m_entryID, none, NULL, NULL, false);

    if (!stats) {
        return 0;
    }

    ItemClassRec* itemClass = g_itemClassDB.GetRecord(stats->m_class);

    if (itemClass && !SStrCmpUTF8I(itemClass->m_className, type, STORM_MAX_STR)) {
        return 1;
    }

    int32_t i = g_itemSubClassDB.m_numRecords;

    while (i) {
        i--;
        const ItemSubClassRec* rec = g_itemSubClassDB.GetRecordByIndex(i);

        if (rec && rec->m_classID == static_cast<int32_t>(stats->m_class) && rec->m_subClassID == static_cast<int32_t>(stats->m_subClass)) {
            if (!SStrCmpUTF8I(rec->m_verboseName, type, STORM_MAX_STR) || !SStrCmpUTF8I(rec->m_displayName, type, STORM_MAX_STR)) {
                return 1;
            }

            break;
        }
    }

    i = g_itemSubClassMaskDB.m_numRecords;

    while (i) {
        i--;
        const ItemSubClassMaskRec* rec = g_itemSubClassMaskDB.GetRecordByIndex(i);

        if (rec->m_classID == static_cast<int32_t>(stats->m_class) && ((1 << stats->m_subClass) & rec->m_mask) && !SStrCmpUTF8I(rec->m_name, type, STORM_MAX_STR)) {
            return 1;
        }
    }

    return 0;
}

// OFFSET: 0x008CDF10/0x14063FCF0
static int32_t Script_IsEquippedItem(lua_State* L) {
    CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();

    if (player) {
        CGItem_C* item;

        if (lua_isnumber(L, 1)) {
            item = player->m_bag.FindItemOfType(lua_tointeger(L, 1), 1);
        } else {
            item = player->m_bag.FindItemByFullName(lua_tolstring(L, 1, NULL), 1);
        }

        if (item) {
            lua_pushnumber(L, 1.0);
            return 1;
        }
    }

    lua_pushnil(L);
    return 1;
}

// OFFSET: 0x008DAC20/0x14064EC60
static int32_t Script_IsEquippedItemType(lua_State* L) {
    const char* type = lua_tolstring(L, 1, NULL);

    if (!type || !*type) {
        luaL_error(L, "Usage: IsEquippedItemType(\"type\")");
        return 0;
    }

    CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();

    if (player) {
        int32_t inventoryType = -1;

        for (int32_t i = 0; i < 29; i++) {
            if (!SStrCmpI(type, s_inventoryTypeTokens[i], STORM_MAX_STR) || !SStrCmpUTF8I(type, FrameScript_GetText(s_inventoryTypeTokens[i], -1, GENDER_NOT_APPLICABLE), STORM_MAX_STR)) {
                inventoryType = i;
                break;
            }
        }

        WOWGUID container;
        uint32_t slot;
        CGItem_C* item;

        if (inventoryType > 0) {
            item = player->m_bag.FindItem(&CanEquipInSlotCallback, &inventoryType, container, slot, 1);
        } else {
            item = player->m_bag.FindItem(&FindItemClassTypeCallback, const_cast<char*>(type), container, slot, 1);
        }

        if (item) {
            lua_pushnumber(L, 1.0);
            return 1;
        }
    }

    lua_pushnil(L);
    return 1;
}

// OFFSET: 0x008CDFC0/0x14063FDC0
static int32_t Script_ItemHasRange(lua_State* L) {
    int32_t itemId = 0;

    if (lua_isnumber(L, 1)) {
        itemId = lua_tointeger(L, 1);
    } else if (lua_isstring(L, 1)) {
        itemId = GetItemIDByName(lua_tolstring(L, 1, NULL));
    }

    const SpellRec* spell = g_spellDB.GetRecord(GetItemSpell(itemId));

    if (spell) {
        CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();

        if (player) {
            float minRange;
            float maxRange;
            Spell_C_GetMinMaxRange(player, spell, &minRange, &maxRange, NULL);

            if (fnotequal(minRange, 0.0f) || fnotequal(maxRange, 0.0f)) {
                lua_pushnumber(L, 1.0);
                return 1;
            }
        }
    }

    lua_pushnil(L);
    return 1;
}

// OFFSET: 0x008CE0F0/0x14063FF10
static int32_t Script_IsItemInRange(lua_State* L) {
    int32_t itemId = 0;

    if (lua_isnumber(L, 1)) {
        itemId = lua_tointeger(L, 1);
    } else if (lua_isstring(L, 1)) {
        itemId = GetItemIDByName(lua_tolstring(L, 1, NULL));
    }

    const SpellRec* spell = g_spellDB.GetRecord(GetItemSpell(itemId));

    if (spell) {
        CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();
        WOWGUID target;
        uint8_t inRange;

        if (player && Script_GetGUIDFromToken(lua_tolstring(L, 2, NULL), target, true) && Spell_C_RangeCheckSelected(player, spell, target, &inRange, false, NULL)) {
            if (inRange) {
                lua_pushnumber(L, 1.0);
            } else {
                lua_pushnumber(L, 0.0);
            }

            return 1;
        }
    }

    lua_pushnil(L);
    return 1;
}

// OFFSET: 0x008CE400/0x140640270
static int32_t Script_UseItemByName(lua_State* L) {
    CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();

    if (player) {
        CGItem_C* item;

        if (lua_isnumber(L, 1)) {
            item = player->m_bag.FindItemOfType(lua_tointeger(L, 1), 0);
        } else {
            item = player->m_bag.FindItemByFullName(lua_tolstring(L, 1, NULL), 0);
        }

        if (item) {
            WOWGUID target;

            if (Script_GetGUIDFromToken(lua_tolstring(L, 2, NULL), target, true)) {
                item->UseItem(&target, 0);
            }
        }
    }

    return 0;
}

// OFFSET: 0x008CE4E0/0x140640380
static int32_t Script_EquipItemByName(lua_State* L) {
    CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();

    if (!player) {
        return 0;
    }

    WOWGUID container;
    uint32_t slot;
    CGItem_C* item;

    if (!lua_isnumber(L, 2) && !lua_isstring(L, 2)) {
        if (lua_isnumber(L, 1)) {
            item = player->m_bag.FindItemOfType(lua_tointeger(L, 1), container, slot, 6);

            if (!container) {
                item = player->m_bag.FindItemOfType(lua_tointeger(L, 1), container, slot, 0);
            }
        } else {
            item = player->m_bag.GetItemIdByName(lua_tolstring(L, 1, NULL), container, slot, 6);

            if (!container) {
                item = player->m_bag.GetItemIdByName(lua_tolstring(L, 1, NULL), container, slot, 0);
            }
        }

        if (!container) {
            return 0;
        }

        if (item && !(item->m_itemFlags & 0x1) && (player->m_bag.m_guid != container || slot > 18)) {
            player->AutoEquipItem(container, slot, 0);
        }

        return 0;
    }

    uint32_t dstSlot = 0;

    if (lua_isnumber(L, 2)) {
        dstSlot = static_cast<int32_t>(lua_tonumber(L, 2)) - 1;
    } else {
        int32_t found = 0;

        if (lua_isstring(L, 2)) {
            const char* name = lua_tolstring(L, 2, NULL);

            for (int32_t i = 0; i < g_paperDollItemFrameDB.m_numRecords; i++) {
                const PaperDollItemFrameRec* rec = g_paperDollItemFrameDB.GetRecordByIndex(i);

                if (!SStrCmpI(rec->m_itemButtonName, name, STORM_MAX_STR)) {
                    dstSlot = rec->m_slotNumber;
                    found = 1;
                    break;
                }
            }
        }

        if (!found) {
            luaL_error(L, "EquipItemByName(): Invalid inventory dstSlot");
        }
    }

    if (dstSlot > 18) {
        luaL_error(L, "EquipItemByName(): Invalid inventory dstSlot");
    }

    if (lua_isnumber(L, 1)) {
        item = player->m_bag.FindItemOfType(lua_tointeger(L, 1), container, slot, 6);

        if (!item) {
            item = player->m_bag.FindItemOfType(lua_tointeger(L, 1), container, slot, 0);
        }
    } else {
        item = player->m_bag.GetItemIdByName(lua_tolstring(L, 1, NULL), container, slot, 6);

        if (!item) {
            item = player->m_bag.GetItemIdByName(lua_tolstring(L, 1, NULL), container, slot, 0);
        }
    }

    if (!item) {
        return 0;
    }

    if ((item->m_itemFlags & 0x1) || (player->m_bag.m_guid == container && slot == dstSlot)) {
        return 0;
    }

    player->AutoEquipItemToSlot(item, dstSlot, 0);
    return 0;
}

// OFFSET: 0x008CE7E0/0x140640730
static int32_t Script_StopAttack(lua_State* L) {
    CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();

    if (player && player->IsInMelee() && CGGameUI::m_hasControl) {
        player->SetCombatMode(true);
    }

    return 0;
}

// OFFSET: 0x008CE8A0/0x140640800
static int32_t Script_Dismount(lua_State* L) {
    CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();

    if (player && !player->IsDeadOrGhost()) {
        player->Dismount();
    }

    return 0;
}

// OFFSET: 0x008CE960/0x1406408F0
static int32_t Script_CanGrantLevel(lua_State* L) {
    CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();
    WOWGUID guid = GetGuidByKeyword(lua_tolstring(L, 1, NULL));

    if (!player || player->CanGrantLevel(guid)) {
        lua_pushnil(L);
        return 1;
    }

    lua_pushnumber(L, 1.0);
    return 1;
}

// OFFSET: 0x008CE9E0/0x140640AF0
static int32_t Script_CanSummonFriend(lua_State* L) {
    CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();
    WOWGUID guid = GetGuidByKeyword(lua_tolstring(L, 1, NULL));

    if (!player || CGPlayer_C::CanSummonFriend(guid)) {
        lua_pushnil(L);
        return 1;
    }

    lua_pushnumber(L, 1.0);
    return 1;
}

// OFFSET: 0x008CEC70/0x140640F70
static int32_t Script_GameMovieFinished(lua_State* L) {
    CGGameUI::StopMovie();
    return 0;
}

// OFFSET: 0x008D5D60/0x140649570
static int32_t Script_ReloadUI(lua_State* L) {
    if (lua_tainted && CGGameUI::m_simpleTop && !CGGameUI::m_simpleTop->unk_1260) {
        CGGameUI::ShowBlockedActionFeedback(NULL, 1);
        return 0;
    }

    CGGameUI::m_reloadUIRequested = 1;
    return 0;
}

// OFFSET: 0x008D37E0/0x14063D420
static int32_t Script_DeleteCursorItem(lua_State* L) {
    CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();

    if (!player) {
        return 0;
    }

    WOWGUID item = CGGameUI::m_cursorItem;
    uint32_t slot = CGGameUI::m_cursorItemSlot;

    if (!item) {
        return 0;
    }

    uint8_t bag = player->FindSlotIndex(CGGameUI::m_cursorItemContainer);
    CGItem_C* object = static_cast<CGItem_C*>(ClntObjMgrObjectPtr(item, TYPEMASK_ITEM, __FILE__, __LINE__));

    if (object && (object->GetItemStaticFlags(0) & 0x20)) {
        CGGameUI::DisplayError(ERR_ITEM_CANT_BE_DESTROYED);
        return 0;
    }

    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_DESTROY_ITEM));
    msg.Put(bag);
    msg.Put(static_cast<uint8_t>(slot));
    msg.Put(static_cast<uint32_t>(CGGameUI::m_stackSplit));
    msg.Finalize();
    ClientServices::Send2(&msg);
    CGGameUI::ClearCursor(1, 1);

    object = static_cast<CGItem_C*>(ClntObjMgrObjectPtr(item, TYPEMASK_ITEM, __FILE__, __LINE__));

    if (object) {
        object->Lock();
    }

    CGActionBar::UpdateUsable();
    return 0;
}

// OFFSET: 0x008D3940/0x140647470
static int32_t Script_FollowUnit(lua_State* L) {
    const char* name = lua_tolstring(L, 1, NULL);
    WOWGUID guid = CGGameUI::ClosestObjectMatch(name, TYPEMASK_PLAYER, 3, StringToBOOL(L, 2, 0), FLT_MAX, 0);

    if (!guid) {
        guid = CGGameUI::ClosestObjectMatch(name, TYPEMASK_UNIT, 0, StringToBOOL(L, 2, 0), FLT_MAX, 0);
    }

    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (unit) {
        CGUnit_C* mover = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(CGUnit_C::s_activeMover, TYPEMASK_UNIT, __FILE__, __LINE__));

        if (mover) {
            WOWGUID target = unit->m_obj->m_guid;
            mover->SetTrackingTarget(target, 3);
        }

        return 0;
    }

    if (name && *name && SStrCmpI(name, "target", STORM_MAX_STR)) {
        CGGameUI::DisplayError(ERR_UNIT_NOT_FOUND);
        return 0;
    }

    CGGameUI::DisplayError(ERR_GENERIC_NO_TARGET);
    return 0;
}

// OFFSET: 0x008D5DE0/0x140649D60
static int32_t Script_ClearFocus(lua_State* L) {
    WOWGUID guid = WOWGUID();
    CGGameUI::SetFocusTarget(guid);
    return 0;
}

// OFFSET: 0x008D3A60/0x14063D810
static int32_t Script_InitiateTrade(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: InitiateTrade(\"unit\")");
        return 0;
    }

    WOWGUID guid = GetGuidByKeyword(lua_tolstring(L, 1, NULL));

    if (!guid) {
        return 0;
    }

    CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();
    CGPlayer_C* target = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player || !target) {
        return 0;
    }

    if (player->IsDeadOrGhost()) {
        CGGameUI::DisplayError(ERR_PLAYER_DEAD);
        return 0;
    }

    if (ClntObjMgrGetActivePlayer() == guid) {
        CGGameUI::DisplayError(ERR_TRADE_SELF);
        return 0;
    }

    if (player->CanCooperate(target)) {
        CGTradeInfo::InitiateTrade(guid, 0);
    } else {
        CGGameUI::DisplayError(ERR_PLAYER_WRONG_FACTION);
    }

    return 0;
}

// OFFSET: 0x008D3B70/0x14063D960
static int32_t Script_CanInspect(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: CanInspect(unit [, showError])");
        return 0;
    }

    const char* token = lua_tolstring(L, 1, NULL);
    CGUnit_C* unit = GetUnitFromName(token);

    if (StringToBOOL(L, 2, 0)) {
        if (unit) {
            CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();

            if (player && (unit->m_obj->m_type & TYPEMASK_PLAYER) && !player->CanAttack(unit)) {
                C3Vector unitPosition = unit->GetPosition();
                C3Vector delta = player->GetPosition() - unitPosition;

                if (delta.y * delta.y + delta.z * delta.z + delta.x * delta.x > s_inspectDistSq) {
                    CGGameUI::DisplayError(ERR_OUT_OF_RANGE);
                }
            } else {
                CGGameUI::DisplayError(ERR_INVALID_INSPECT_TARGET);
            }
        } else if (token && *token && SStrCmpI(token, "target", STORM_MAX_STR)) {
            CGGameUI::DisplayError(ERR_UNIT_NOT_FOUND);
        } else {
            CGGameUI::DisplayError(ERR_GENERIC_NO_TARGET);
        }
    }

    if (CGGameUI::CanInspect(unit)) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: 0x008D3D10/0x14063DB20
static int32_t Script_NotifyInspect(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: NotifyInspect(unit)");
        return 0;
    }

    CGUnit_C* unit = GetUnitFromName(lua_tolstring(L, 1, NULL));

    if (CGGameUI::CanInspect(unit)) {
        CDataStore msg;
        msg.Put(static_cast<uint32_t>(CMSG_INSPECT));
        msg.Put(unit->m_obj->m_guid.m_guid);
        msg.Finalize();
        ClientServices::Send2(&msg);
        WOWGUID guid = unit->m_obj->m_guid;
        CGCharacterInfo::SetInspectTarget(guid);
    }

    return 0;
}

// OFFSET: 0x008D5E30/0x14064A080
static int32_t Script_PickupPlayerMoney(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: PickupPlayerMoney(amount)");
        return 0;
    }

    CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();

    if (player) {
        uint32_t amount = static_cast<uint32_t>(lua_tonumber(L, 1));

        if (amount && amount <= player->GetCoinage()) {
            CGGameUI::SetCursorMoney(amount);
        }
    }

    return 0;
}

// OFFSET: 0x008D4410/0x140647770
static int32_t Script_SetCurrentTitle(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: SetCurrentTitle(titleMaskID)");
        return 0;
    }

    CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();
    int32_t title = static_cast<int32_t>(lua_tonumber(L, 1));

    if (!player) {
        return 0;
    }

    if (lua_tainted && CGGameUI::m_simpleTop && !CGGameUI::m_simpleTop->unk_1260) {
        CGGameUI::ShowBlockedActionFeedback(NULL, 1);
        return 0;
    }

    if (++s_setTitleCount > 10) {
        float now = OsGetAsyncTimeSec();

        if (now - s_setTitleTime >= 60.0f) {
            s_setTitleTime = now;
            s_setTitleCount = 0;
        } else if (s_setTitleCount > 10) {
            CGGameUI::DisplayError(ERR_CLIENT_LOCKED_OUT);
            return 0;
        }
    }

    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_SET_TITLE));
    msg.Put(static_cast<uint32_t>(title));
    msg.Finalize();
    ClientServices::Send2(&msg);
    return 0;
}

// OFFSET: 0x008DCE60/0x140651860
static int32_t Script_BindEnchant(lua_State* L) {
    CGGameUI::CastItemSpell(CGGameUI::m_lastItemSpellTarget, 1);
    return 0;
}

// OFFSET: 0x008D4570
static int32_t Script_CancelAreaSpiritHeal(lua_State* L) {
    CGGameUI::CastSpiritHealSpell();
    return 0;
}

// OFFSET: 0x008DA400/0x14064E250
static int32_t Script_PickupItem(lua_State* L) {
    int32_t itemId = 0;

    if (lua_isnumber(L, 1)) {
        itemId = lua_tointeger(L, 1);
    } else if (lua_isstring(L, 1)) {
        itemId = GetItemIDByName(lua_tolstring(L, 1, NULL));
    }

    ItemRec* rec = g_itemDB.GetRecord(itemId, WOWGUID(), NULL, NULL, false);
    int32_t displayId = rec ? rec->m_displayInfoID : 0;
    CGGameUI::SetCursorVirtualItem(itemId, displayId, 0, 7, 0);
    return 0;
}

// OFFSET: 0x008D4580/0x140647980
static int32_t Script_StartAttack(lua_State* L) {
    CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();
    int32_t hasTarget = 0;

    if (!player) {
        return 0;
    }

    WOWGUID guid = WOWGUID();
    const char* token = lua_tolstring(L, 1, NULL);

    if (token && *token) {
        if (!Script_GetGUIDFromToken(token, guid, false)) {
            guid = CGGameUI::ClosestObjectMatch(token, TYPEMASK_UNIT, 1, StringToBOOL(L, 2, 0), FLT_MAX, 1);
        }

        hasTarget = 1;
    }

    if (!player->IsInMelee()) {
        player->OnAttackIconPressed(guid, hasTarget);
    }

    return 0;
}

// OFFSET: 0x008D4650/0x140640990
static int32_t Script_GrantLevel(lua_State* L) {
    CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();
    WOWGUID guid = GetGuidByKeyword(lua_tolstring(L, 1, NULL));
    int32_t error = player->CanGrantLevel(guid);

    if (!player || error) {
        BroadcastUIError(error, NULL);
        return 0;
    }

    WDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_GRANT_LEVEL));
    SmartGUID target;
    target.m_guid = guid.m_guid;
    msg << target;
    msg.Finalize();
    ClientServices::Send2(&msg);
    return 0;
}

// OFFSET: #0x00523210
static void SummonFriendErrorCallback(int32_t id, const WOWGUID& guid, void* param, bool success) {
    if (!success) {
        return;
    }

    WOWGUID none = WOWGUID();
    NameCache* name = g_nameDBCache.GetRecord(guid.m_guid, none, NULL, NULL, false);

    if (name) {
        BroadcastUIError(static_cast<int32_t>(reinterpret_cast<intptr_t>(param)), name->m_name);
    }
}

// OFFSET: 0x008D4750/0x140640BE0
static int32_t Script_SummonFriend(lua_State* L) {
    CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();
    WOWGUID guid = GetGuidByKeyword(lua_tolstring(L, 1, NULL));
    int32_t error = CGPlayer_C::CanSummonFriend(guid);

    if (!player || error) {
        if (error != 9 && error != 13) {
            BroadcastUIError(error, NULL);
            return 0;
        }

        WOWGUID none = WOWGUID();
        NameCache* name = g_nameDBCache.GetRecord(guid.m_guid, none, &SummonFriendErrorCallback, reinterpret_cast<void*>(static_cast<intptr_t>(error)), false);

        if (name) {
            BroadcastUIError(error, name->m_name);
        }

        return 0;
    }

    int32_t retarget = 0;

    if (CGGameUI::m_lockedTarget != guid) {
        retarget = 1;
        CGGameUI::SendTarget(guid);
    }

    Spell_C_CastSpell(CGSpellBook::m_summonFriendSpell, NULL, guid, false);

    if (retarget) {
        CGGameUI::SendTarget(CGGameUI::m_lockedTarget);
    }

    return 0;
}

// OFFSET: 0x008CEC60/0x140640F60
static int32_t Script_AcceptLevelGrant(lua_State* L) {
    CGGameUI::AcceptLevelGrant();
    return 0;
}

// OFFSET: 0x008DCE80/0x140651880
static int32_t Script_EndRefund(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        return 0;
    }

    int32_t type = static_cast<int32_t>(lua_tonumber(L, 1));

    if (type == 1) {
        CGGameUI::CastItemSpell(CGGameUI::m_lastItemSpellTarget, 1);
        return 0;
    }

    if (type == 2) {
        CGItemSocketInfo::Accept();
    }

    return 0;
}

// OFFSET: 0x008DCEE0/0x1406518E0
static int32_t Script_EndBoundTradeable(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: EndBoundTradeable(\"type\")");
        return 0;
    }

    const char* type = lua_tolstring(L, 1, NULL);

    if (!SStrCmpI(type, "itemenchant", STORM_MAX_STR)) {
        CGGameUI::CastItemSpell(CGGameUI::m_lastItemSpellTarget, 1);
        return 0;
    }

    if (!SStrCmpI(type, "gem", STORM_MAX_STR)) {
        CGItemSocketInfo::Accept();
        return 0;
    }

    if (!SStrCmpI(type, "spellenchant", STORM_MAX_STR) && CGGameUI::m_lastEnchantSpellID) {
        Spell_C_CastSpell(CGGameUI::m_lastEnchantSpellID, NULL, CGGameUI::m_lastItemSpellTarget, true);
        CGGameUI::m_lastEnchantSpellID = 0;
    }

    return 0;
}

// OFFSET: #0x005259E0
static void TargetIfValid(const WOWGUID& guid) {
    if (ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__) || CGPartyInfo::IsMemberOrPet(guid) || CGRaidInfo::IsPlayerOrPetInRaid(guid)) {
        CGGameUI::Target(guid);
    }
}

// OFFSET: 0x008D72C0
static int32_t Script_TargetUnit(lua_State* L) {
    const char* name = lua_tolstring(L, 1, NULL);
    int32_t exact = StringToBOOL(L, 2, 0);
    WOWGUID guid = CGGameUI::ClosestObjectMatch(name, TYPEMASK_UNIT, 0, exact, FLT_MAX, 1);

    if (guid) {
        TargetIfValid(guid);
    }

    return 0;
}

// OFFSET: 0x008D74C0/0x140649810
static int32_t Script_TargetDirectionEnemy(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: TargetDirectionEnemy(facing)");
        return 0;
    }

    if (CGUIMacros::IsActionAllowed(5)) {
        float facing = static_cast<float>(lua_tonumber(L, 1));
        float angle = static_cast<float>(lua_isnumber(L, 2) ? lua_tonumber(L, 2) : 0.0);
        CGGameUI::TargetDirection(facing, angle, 1);
        CGUIMacros::m_state &= ~0x20u;
    }

    return 0;
}

// OFFSET: 0x008D7560/0x1406498C0
static int32_t Script_TargetDirectionFriend(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: TargetDirectionFriend(facing)");
        return 0;
    }

    if (CGUIMacros::IsActionAllowed(5)) {
        float facing = static_cast<float>(lua_tonumber(L, 1));
        float angle = static_cast<float>(lua_isnumber(L, 2) ? lua_tonumber(L, 2) : 0.0);
        CGGameUI::TargetDirection(facing, angle, 3);
        CGUIMacros::m_state &= ~0x20u;
    }

    return 0;
}

// OFFSET: 0x008D7600
static int32_t Script_TargetLastTarget(lua_State* L) {
    if (CGGameUI::m_lastTarget) {
        TargetIfValid(CGGameUI::m_lastTarget);
        return 0;
    }

    WOWGUID none = WOWGUID();
    CGGameUI::Target(none);
    return 0;
}

// OFFSET: 0x008D7640/0x140649A40
static int32_t Script_TargetLastEnemy(lua_State* L) {
    TargetIfValid(CGGameUI::m_lastEnemyTarget);
    return 0;
}

// OFFSET: 0x008D76A0/0x140649AA0
static int32_t Script_TargetLastFriend(lua_State* L) {
    TargetIfValid(CGGameUI::m_lastFriendTarget);
    return 0;
}

// OFFSET: 0x008D7700/0x140649B00
static int32_t Script_AssistUnit(lua_State* L) {
    const char* name = lua_tolstring(L, 1, NULL);
    int32_t exact = StringToBOOL(L, 2, 0);
    WOWGUID guid = CGGameUI::ClosestObjectMatch(name, TYPEMASK_UNIT, 0, exact, FLT_MAX, 0);
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (unit) {
        WOWGUID target = unit->m_unit->m_unitFieldTarget;

        if (target) {
            TargetIfValid(target);

            if (g_assistAttackCVar->m_intValue) {
                CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();

                if (player) {
                    player->CombatModeEnter(target, 0, 0, 0, 0);
                }
            }
        }

        return 0;
    }

    if (name && *name && SStrCmpI(name, "target", STORM_MAX_STR)) {
        CGGameUI::DisplayError(ERR_UNIT_NOT_FOUND);
        return 0;
    }

    CGGameUI::DisplayError(ERR_GENERIC_NO_TARGET);
    return 0;
}

// OFFSET: 0x008D7870/0x140649D80
static int32_t Script_SetDungeonDifficulty(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: SetDungeonDifficulty(difficulty)");
        return 0;
    }

    uint32_t difficulty = static_cast<uint32_t>(lua_tonumber(L, 1) - 1.0);

    if (difficulty > 1) {
        return 0;
    }

    if ((CGPartyInfo::m_members[0] || CGRaidInfo::m_numMembers) && CGPartyInfo::m_leader != ClntObjMgrGetActivePlayer()) {
        CGGameUI::DisplayError(ERR_NOT_LEADER);
        return 0;
    }

    uint32_t current = CGGameUI::GetCurrentDungeonDifficulty();
    CGGameUI::m_dungeonDifficulty = difficulty;
    CGPartyInfo::m_dungeonDifficulty = difficulty;

    if (current != difficulty) {
        CGGameUI::ShowDungeonDifficultyChanged();
    }

    CDataStore msg;
    msg.Put(static_cast<uint32_t>(MSG_SET_DUNGEON_DIFFICULTY));
    msg.Put(difficulty);
    msg.Finalize();
    ClientServices::Send2(&msg);
    return 0;
}

// OFFSET: 0x008D79F0/0x140649F00
static int32_t Script_SetRaidDifficulty(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: SetRaidDifficulty(difficulty)");
        return 0;
    }

    uint32_t difficulty = static_cast<uint32_t>(lua_tonumber(L, 1) - 1.0);

    if (difficulty > 3) {
        return 0;
    }

    if ((CGPartyInfo::m_members[0] || CGRaidInfo::m_numMembers) && CGPartyInfo::m_leader != ClntObjMgrGetActivePlayer()) {
        CGGameUI::DisplayError(ERR_NOT_LEADER);
        return 0;
    }

    CGGameUI::SetRaidDifficulty(difficulty, 1, 1);

    CDataStore msg;
    msg.Put(static_cast<uint32_t>(MSG_SET_RAID_DIFFICULTY));
    msg.Put(difficulty);
    msg.Finalize();
    ClientServices::Send2(&msg);
    return 0;
}

// OFFSET: 0x008D7B20/0x14064AFB0
static int32_t Script_AcceptAreaSpiritHeal(lua_State* L) {
    CGGameUI::AcceptAreaSpiritHeal();
    return 0;
}

// OFFSET: 0x008D8D40/0x14064AEB0
static int32_t Script_InteractUnit(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: InteractUnit(unitToken[, exactMatch])");
        return 0;
    }

    const char* token = lua_tolstring(L, 1, NULL);
    WOWGUID guid;

    if (SStrCmpI(token, "mouseover", STORM_MAX_STR)) {
        guid = CGGameUI::ClosestObjectMatch(token, TYPEMASK_OBJECT, 0, StringToBOOL(L, 2, 0), FLT_MAX, 0);
    } else {
        guid = CGGameUI::m_currentObjectTrack;
    }

    if (lua_tainted) {
        CGGameUI::ShowBlockedActionFeedback(NULL, 0);
        return 0;
    }

    if (CGGameUI::OnSpriteRightClick(guid)) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// PROVEN: f51454e9 IDENTICAL/IDENTICAL
// OFFSET: 0x00842700/0x1407FDD80
static int32_t Script_GetDebugStats(lua_State* L) {
    return 0;
}

// PROVEN: bb028e9b IDENTICAL/IDENTICAL
// OFFSET: 0x00842700/0x1407FDD80
static int32_t Script_GetExtendedItemInfo(lua_State* L) {
    return 0;
}

// PROVEN: 5e9564d9 IDENTICAL/IDENTICAL
// OFFSET: 0x00842700/0x1407FDD80
static int32_t Script_ResetPerformanceValues(lua_State* L) {
    return 0;
}

// PROVEN: d8cb255d IDENTICAL/IDENTICAL
// OFFSET: 0x00842700/0x1407FDD80
static int32_t Script_ToggleCollision(lua_State* L) {
    return 0;
}

// PROVEN: e567a066 IDENTICAL/IDENTICAL
// OFFSET: 0x00842700/0x1407FDD80
static int32_t Script_ToggleCollisionDisplay(lua_State* L) {
    return 0;
}

// PROVEN: c6ad3ca4 IDENTICAL/IDENTICAL
// OFFSET: 0x00842700/0x1407FDD80
static int32_t Script_TogglePerformanceDisplay(lua_State* L) {
    return 0;
}

// PROVEN: 76ca55f6 IDENTICAL/IDENTICAL
// OFFSET: 0x00842700/0x1407FDD80
static int32_t Script_TogglePerformancePause(lua_State* L) {
    return 0;
}

// PROVEN: 19aff168 IDENTICAL/IDENTICAL
// OFFSET: 0x00842700/0x1407FDD80
static int32_t Script_TogglePerformanceValues(lua_State* L) {
    return 0;
}

// PROVEN: ad89f463 IDENTICAL/IDENTICAL
// OFFSET: 0x00842700/0x1407FDD80
static int32_t Script_TogglePlayerBounds(lua_State* L) {
    return 0;
}

// PROVEN: 16dc2e2a IDENTICAL/IDENTICAL
// OFFSET: 0x00842700/0x1407FDD80
static int32_t Script_TogglePortals(lua_State* L) {
    return 0;
}

// PROVEN: 7750e8f7 IDENTICAL/IDENTICAL
// OFFSET: 0x00842700/0x1407FDD80
static int32_t Script_ToggleTris(lua_State* L) {
    return 0;
}

// OFFSET: 0x008C2760/0x140632160
static int32_t Script_ConfirmOnUse(lua_State* L) {
    CGItem_C::ConfirmOnUse();
    return 0;
}

// OFFSET: 0x008C26A0/0x140632000
static int32_t Script_GetNetIpTypes(lua_State* L) {
    for (uint32_t i = 0; i < 2; i++) {
        lua_pushnumber(L, static_cast<double>(ClientServices::Connection()->GetLocalAddress(i)));
    }

    return 2;
}

// OFFSET: 0x008C3900/0x1406334A0
static int32_t Script_GetBackgroundLoadingStatus(lua_State* L) {
    lua_pushnumber(L, static_cast<double>(static_cast<uint32_t>(SFile::IsBackgroundStreamingInProgress())));
    return 1;
}

// OFFSET: 0x008C3940/0x1406334D0
static int32_t Script_GetAvailableBandwidth(lua_State* L) {
    lua_pushnumber(L, AsyncFileReadGetStreamingBandwidth());
    return 1;
}

// OFFSET: 0x008C3970/0x140633500
static int32_t Script_GetDownloadedPercentage(lua_State* L) {
    lua_pushnumber(L, SFile::GetBGStreamingDownloadedPercentage());
    return 1;
}

// OFFSET: 0x008C39A0/0x140633530
static int32_t Script_GetMaxSpellStartRecoveryOffset(lua_State* L) {
    lua_pushnumber(L, static_cast<double>(Spell_C_GetMaxStartRecoveryOffset()));
    return 1;
}

// OFFSET: 0x008C39D0/0x140633560
static int32_t Script_ValidateTimer(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: Script_ValidateTimer(TimerType)");
    }

    if (!lua_tointeger(L, 1)) {
        UserClientQueryBattlefieldState msg;
        ClientServices::Send(&msg);
    }

    return 0;
}

// OFFSET: 0x008C3A30/0x1406335D0
static int32_t Script_GetRestrictedAccountData(lua_State* L) {
    lua_pushnumber(L, static_cast<double>(CGGameUI::s_restrictedLevel));
    lua_pushnumber(L, static_cast<double>(CGGameUI::s_restrictedMoney));
    lua_pushnumber(L, 100.0);
    return 3;
}

// OFFSET: 0x008C6DF0/0x140636E20
static int32_t Script_GetCVarBitfield(lua_State* L) {
    if (!lua_isstring(L, 1) || !lua_isnumber(L, 2)) {
        luaL_error(L, "Usage: GetCVarBitfield(\"cvar\", index)");
    }

    int32_t index = lua_tointeger(L, 2);

    if (index < 1) {
        luaL_error(L, "Index out of range");
    }

    CVar* var = CVar::LookupRegistered(lua_tolstring(L, 1, NULL));

    if (!var || (var->m_flags & 0x40)) {
        lua_pushnil(L);
        return 1;
    }

    CVarArrayWrapper wrapper(var, false);
    lua_pushboolean(L, wrapper.GetValue(index - 1));
    return 1;
}

// OFFSET: 0x008C7290/0x140631CD0
static int32_t Script_IsOnTournamentRealm(lua_State* L) {
    if (CGGameUI::s_tournamentRealm) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: 0x008C73C0/0x140637240
static int32_t Script_GetDefaultRaidDifficulty(lua_State* L) {
    lua_pushnumber(L, static_cast<double>(CGGameUI::m_raidDifficulty + 1));
    return 1;
}

// OFFSET: 0x008C7400/0x140637270
static int32_t Script_GetCurrentRaidDifficulty(lua_State* L) {
    lua_pushnumber(L, static_cast<double>(CGGameUI::GetCurrentRaidDifficulty() + 1));
    return 1;
}

// OFFSET: 0x008C8D20/0x140638F00
static int32_t Script_GetAddOnOptionalDependencies(lua_State* L) {
    const char* name;

    if (lua_isnumber(L, 1)) {
        name = GetAddOnByIndex(static_cast<uint32_t>(static_cast<int64_t>(lua_tonumber(L, 1))) - 1);

        if (!name) {
            luaL_error(L, "AddOn index must be in the range of 1 to %d", GetNumAddOns());
        }
    } else {
        if (!lua_isstring(L, 1)) {
            luaL_error(L, "Usage: GetAddOnOptionalDependencies(index or \"name\")");
        }

        name = lua_tolstring(L, 1, NULL);
    }

    TSGrowableArray<char*>* dependencies = GetAddonOptionalDeps(name);

    if (!dependencies) {
        return 0;
    }

    lua_checkstack(L, dependencies->m_count);

    for (uint32_t i = 0; i < dependencies->m_count; i++) {
        lua_pushstring(L, dependencies->m_data[i]);
    }

    return dependencies->m_count;
}

// OFFSET: 0x008C9170/0x140633470
static int32_t Script_GetCemeteryPreference(lua_State* L) {
    lua_pushnumber(L, static_cast<double>(CGGameUI::s_cemeteryPreference));
    return 1;
}

// OFFSET: 0x008C91B0/0x140639330
static int32_t Script_SetCemeteryPreference(lua_State* L) {
    if (!lua_isnumber(L, 1)) {
        luaL_error(L, "Usage: Script_SetCemeteryPreference(cemeteryID)");
    }

    CGGameUI::SetCemeteryPreference(static_cast<uint32_t>(static_cast<int64_t>(lua_tonumber(L, 1))));
    return 0;
}

// OFFSET: 0x008C9220/0x140639380
static int32_t Script_IsCemeterySelectionAvailable(lua_State* L) {
    if (CGGameUI::s_numCemeteries) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: 0x008C9260/0x1406393C0
static int32_t Script_GetFileStreamingStatus(lua_State* L) {
    lua_pushnumber(L, static_cast<double>(static_cast<uint32_t>(CGGameUI::s_streamingIcon)));
    return 1;
}

// OFFSET: 0x008C92A0/0x1406393F0
static int32_t Script_MouseOverrideCinematicDisable(lua_State* L) {
    int32_t disable = StringToBOOL(L, 1, 0);
    CGInputControl* input = CGInputControl::GetActive();

    if (disable) {
        input->m_mouseModeFlags |= 0x80;
    } else {
        input->m_mouseModeFlags &= ~0x80u;
    }

    input->UpdateMouseMode(0);
    return 0;
}

// OFFSET: 0x008C92F0/0x140633620
static int32_t Script_HasTravelPass(lua_State* L) {
    lua_pushboolean(L, CGGameUI::m_hasTravelPass);
    return 1;
}

// OFFSET: 0x008C9310/0x140633640
static int32_t Script_CanSendSoRByText(lua_State* L) {
    lua_pushboolean(L, CGGameUI::s_canSendSoR);
    return 1;
}

// OFFSET: 0x008C9330/0x140633660
static int32_t Script_GetNumSoRRemaining(lua_State* L) {
    lua_pushinteger(L, CGGameUI::s_numSoRRemaining);
    return 1;
}

// OFFSET: 0x008CC210/0x14063D0A0
static int32_t Script_SetCVarBitfield(lua_State* L) {
    if (!lua_isstring(L, 1)) {
        luaL_error(L, "Usage: SetCVarBitfield(\"cvar\", index, value, [, \"scriptCvar\")");
    }

    const char* name = lua_tolstring(L, 1, NULL);
    CVar* var = CVar::LookupRegistered(name);

    if (!var || (var->m_flags & 0x40)) {
        luaL_error(L, "Couldn't find CVar named '%s'", name);
    }

    if ((var->m_flags & 0x4) || (var->m_flags & 0x100)) {
        luaL_error(L, "\"%s\" is read-only", name);
    }

    if ((var->m_flags & 0x8) && !CSimpleTop::s_instance->m_allowProtectedActions) {
        CGGameUI::ShowBlockedActionFeedback(NULL, 2);
        return 0;
    }

    int32_t index = lua_tointeger(L, 2);

    if (index < 1) {
        luaL_error(L, "Index out of range");
    }

    int32_t value = lua_toboolean(L, 3);
    CVarArrayWrapper wrapper(var, false);

    if (!wrapper.SetValue(index - 1, value != 0)) {
        luaL_error(L, "Index out of range");
    }

    if (lua_isstring(L, 4)) {
        FrameScript_SignalEvent(SCRIPT_EVENT_CVAR_UPDATE, "%s%s", lua_tolstring(L, 4, NULL), lua_tolstring(L, 3, NULL));
    }

    return 0;
}

// OFFSET: 0x008CC6C0/0x14063D7B0
static int32_t Script_GetSheathState(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return 0;
    }

    lua_pushnumber(L, static_cast<double>(player->m_unit->m_unitFieldBytes2[0] + 1));
    return 1;
}

// OFFSET: 0x008CEDF0/0x140641140
static int32_t Script_GetAutoDeclineGuildInvites(lua_State* L) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player && (player->m_player->m_playerFlags & 0x8000000)) {
        lua_pushnumber(L, 1.0);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

// OFFSET: 0x008CEE60/0x1406411B0
static int32_t Script_SetAutoDeclineGuildInvites(lua_State* L) {
    if (ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__)) {
        int32_t decline = StringToBOOL(L, 1, 1);
        CDataStore msg;
        msg.Put(static_cast<uint32_t>(CMSG_AUTO_DECLINE_GUILD_INVITES));
        msg.Put(static_cast<uint8_t>(decline != 0));
        msg.Finalize();
        ClientServices::Send2(&msg);
    }

    return 0;
}

// OFFSET: 0x008D3760/0x1406473F0
static int32_t Script_PortGraveyard(lua_State* L) {
    if (lua_tainted) {
        CGGameUI::ShowBlockedActionFeedback(NULL, 0);
        return 0;
    }

    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player && (player->m_player->m_playerFlags & 0x10)) {
        UserClientReturnToGraveyard msg;
        ClientServices::Send(&msg);
    }

    return 0;
}

// OFFSET: 0x00842700/0x1407FDD80
static int32_t Script_GetDebugAnimationStats(lua_State* L) {
    return 0;
}

// OFFSET: 0x00842700/0x1407FDD80
static int32_t Script_GetDebugSpellEffects(lua_State* L) {
    return 0;
}

// OFFSET: 0x00842700/0x1407FDD80
static int32_t Script_ToggleAnimKitDisplay(lua_State* L) {
    return 0;
}

// STATIC: 0x00D0B2B8
static FrameScript_Method s_ScriptFunctions[] = {
    { "FrameXML_Debug", &Script_FrameXML_Debug },
    { "GetBuildInfo", &Script_GetBuildInfo },
    { "ReloadUI", &Script_ReloadUI },
    { "RegisterForSave", &Script_RegisterForSave },
    { "RegisterForSavePerCharacter", &Script_RegisterForSavePerCharacter },
    { "SetLayoutMode", &Script_SetLayoutMode },
    { "IsModifierKeyDown", &Script_IsModifierKeyDown },
    { "IsLeftShiftKeyDown", &Script_IsLeftShiftKeyDown },
    { "IsRightShiftKeyDown", &Script_IsRightShiftKeyDown },
    { "IsShiftKeyDown", &Script_IsShiftKeyDown },
    { "IsLeftControlKeyDown", &Script_IsLeftControlKeyDown },
    { "IsRightControlKeyDown", &Script_IsRightControlKeyDown },
    { "IsControlKeyDown", &Script_IsControlKeyDown },
    { "IsLeftAltKeyDown", &Script_IsLeftAltKeyDown },
    { "IsRightAltKeyDown", &Script_IsRightAltKeyDown },
    { "IsAltKeyDown", &Script_IsAltKeyDown },
    { "IsMouseButtonDown", &Script_IsMouseButtonDown },
    { "GetMouseButtonName", &Script_GetMouseButtonName },
    { "GetMouseButtonClicked", &Script_GetMouseButtonClicked },
    { "SetConsoleKey", &Script_SetConsoleKey },
    { "Screenshot", &Script_Screenshot },
    { "GetFramerate", &Script_GetFramerate },
    { "TogglePerformanceDisplay", &Script_TogglePerformanceDisplay },
    { "TogglePerformancePause", &Script_TogglePerformancePause },
    { "TogglePerformanceValues", &Script_TogglePerformanceValues },
    { "ResetPerformanceValues", &Script_ResetPerformanceValues },
    { "GetDebugStats", &Script_GetDebugStats },
    { "GetDebugAnimationStats", &Script_GetDebugAnimationStats },
    { "GetDebugSpellEffects", &Script_GetDebugSpellEffects },
    { "IsDebugBuild", &Script_IsDebugBuild },
    { "RegisterCVar", &Script_RegisterCVar },
    { "GetCVarInfo", &Script_GetCVarInfo },
    { "SetCVar", &Script_SetCVar },
    { "GetCVar", &Script_GetCVar },
    { "SetCVarBitfield", &Script_SetCVarBitfield },
    { "GetCVarBitfield", &Script_GetCVarBitfield },
    { "GetCVarBool", &Script_GetCVarBool },
    { "GetCVarDefault", &Script_GetCVarDefault },
    { "ToggleTris", &Script_ToggleTris },
    { "TogglePortals", &Script_TogglePortals },
    { "ToggleCollision", &Script_ToggleCollision },
    { "ToggleCollisionDisplay", &Script_ToggleCollisionDisplay },
    { "TogglePlayerBounds", &Script_TogglePlayerBounds },
    { "Stuck", &Script_Stuck },
    { "PortGraveyard", &Script_PortGraveyard },
    { "Logout", &Script_Logout },
    { "Quit", &Script_Quit },
    { "SetCursor", &Script_SetCursor },
    { "ResetCursor", &Script_ResetCursor },
    { "ClearCursor", &Script_ClearCursor },
    { "CursorHasItem", &Script_CursorHasItem },
    { "CursorHasSpell", &Script_CursorHasSpell },
    { "CursorHasMacro", &Script_CursorHasMacro },
    { "CursorHasMoney", &Script_CursorHasMoney },
    { "GetCursorInfo", &Script_GetCursorInfo },
    { "EquipCursorItem", &Script_EquipCursorItem },
    { "DeleteCursorItem", &Script_DeleteCursorItem },
    { "EquipPendingItem", &Script_EquipPendingItem },
    { "CancelPendingEquip", &Script_CancelPendingEquip },
    { "TargetUnit", &Script_TargetUnit },
    { "TargetNearest", &Script_TargetNearest },
    { "TargetNearestEnemy", &Script_TargetNearestEnemy },
    { "TargetNearestEnemyPlayer", &Script_TargetNearestEnemyPlayer },
    { "TargetNearestFriend", &Script_TargetNearestFriend },
    { "TargetNearestFriendPlayer", &Script_TargetNearestFriendPlayer },
    { "TargetNearestPartyMember", &Script_TargetNearestPartyMember },
    { "TargetNearestRaidMember", &Script_TargetNearestRaidMember },
    { "TargetDirectionEnemy", &Script_TargetDirectionEnemy },
    { "TargetDirectionFriend", &Script_TargetDirectionFriend },
    { "TargetDirectionFinished", &Script_TargetDirectionFinished },
    { "TargetLastTarget", &Script_TargetLastTarget },
    { "TargetLastEnemy", &Script_TargetLastEnemy },
    { "TargetLastFriend", &Script_TargetLastFriend },
    { "AttackTarget", &Script_AttackTarget },
    { "AssistUnit", &Script_AssistUnit },
    { "FocusUnit", &Script_FocusUnit },
    { "FollowUnit", &Script_FollowUnit },
    { "InteractUnit", &Script_InteractUnit },
    { "ClearTarget", &Script_ClearTarget },
    { "ClearFocus", &Script_ClearFocus },
    { "AutoEquipCursorItem", &Script_AutoEquipCursorItem },
    { "ToggleSheath", &Script_ToggleSheath },
    { "GetSheathState", &Script_GetSheathState },
    { "GetZoneText", &Script_GetZoneText },
    { "GetRealZoneText", &Script_GetRealZoneText },
    { "GetSubZoneText", &Script_GetSubZoneText },
    { "GetMinimapZoneText", &Script_GetMinimapZoneText },
    { "InitiateTrade", &Script_InitiateTrade },
    { "CanInspect", &Script_CanInspect },
    { "NotifyInspect", &Script_NotifyInspect },
    { "RequestTimePlayed", &Script_RequestTimePlayed },
    { "RepopMe", &Script_RepopMe },
    { "AcceptResurrect", &Script_AcceptResurrect },
    { "DeclineResurrect", &Script_DeclineResurrect },
    { "ResurrectGetOfferer", &Script_ResurrectGetOfferer },
    { "ResurrectHasSickness", &Script_ResurrectHasSickness },
    { "ResurrectHasTimer", &Script_ResurrectHasTimer },
    { "BeginTrade", &Script_BeginTrade },
    { "CancelTrade", &Script_CancelTrade },
    { "AcceptGuild", &Script_AcceptGuild },
    { "DeclineGuild", &Script_DeclineGuild },
    { "AcceptArenaTeam", &Script_AcceptArenaTeam },
    { "DeclineArenaTeam", &Script_DeclineArenaTeam },
    { "CancelLogout", &Script_CancelLogout },
    { "ForceLogout", &Script_ForceLogout },
    { "ForceQuit", &Script_ForceQuit },
    { "GetCursorMoney", &Script_GetCursorMoney },
    { "DropCursorMoney", &Script_DropCursorMoney },
    { "PickupPlayerMoney", &Script_PickupPlayerMoney },
    { "HasSoulstone", &Script_HasSoulstone },
    { "UseSoulstone", &Script_UseSoulstone },
    { "HasKey", &Script_HasKey },
    { "GuildInvite", &Script_GuildInvite },
    { "GuildUninvite", &Script_GuildUninvite },
    { "GuildPromote", &Script_GuildPromote },
    { "GuildDemote", &Script_GuildDemote },
    { "GuildSetLeader", &Script_GuildSetLeader },
    { "GuildSetMOTD", &Script_GuildSetMOTD },
    { "GuildLeave", &Script_GuildLeave },
    { "GuildDisband", &Script_GuildDisband },
    { "ArenaTeamInviteByName", &Script_ArenaTeamInviteByName },
    { "ArenaTeamLeave", &Script_ArenaTeamLeave },
    { "ArenaTeamUninviteByName", &Script_ArenaTeamUninviteByName },
    { "ArenaTeamSetLeaderByName", &Script_ArenaTeamSetLeaderByName },
    { "ArenaTeamDisband", &Script_ArenaTeamDisband },
    { "GetScreenWidth", &Script_GetScreenWidth },
    { "GetScreenHeight", &Script_GetScreenHeight },
    { "GetReleaseTimeRemaining", &Script_GetReleaseTimeRemaining },
    { "GetCorpseRecoveryDelay", &Script_GetCorpseRecoveryDelay },
    { "GetInstanceBootTimeRemaining", &Script_GetInstanceBootTimeRemaining },
    { "GetInstanceLockTimeRemaining", &Script_GetInstanceLockTimeRemaining },
    { "GetInstanceLockTimeRemainingEncounter", &Script_GetInstanceLockTimeRemainingEncounter },
    { "GetSummonConfirmTimeLeft", &Script_GetSummonConfirmTimeLeft },
    { "GetSummonConfirmSummoner", &Script_GetSummonConfirmSummoner },
    { "GetSummonConfirmAreaName", &Script_GetSummonConfirmAreaName },
    { "ConfirmSummon", &Script_ConfirmSummon },
    { "CancelSummon", &Script_CancelSummon },
    { "GetCursorPosition", &Script_GetCursorPosition },
    { "GetNetStats", &Script_GetNetStats },
    { "GetNetIpTypes", &Script_GetNetIpTypes },
    { "SitStandOrDescendStart", &Script_SitStandOrDescendStart },
    { "StopCinematic", &Script_StopCinematic },
    { "RunScript", &Script_RunScript },
    { "CheckInteractDistance", &Script_CheckInteractDistance },
    { "RandomRoll", &Script_RandomRoll },
    { "OpeningCinematic", &Script_OpeningCinematic },
    { "InCinematic", &Script_InCinematic },
    { "AcceptXPLoss", &Script_AcceptXPLoss },
    { "CheckSpiritHealerDist", &Script_CheckSpiritHealerDist },
    { "CheckTalentMasterDist", &Script_CheckTalentMasterDist },
    { "CheckBinderDist", &Script_CheckBinderDist },
    { "RetrieveCorpse", &Script_RetrieveCorpse },
    { "BindEnchant", &Script_BindEnchant },
    { "ReplaceEnchant", &Script_ReplaceEnchant },
    { "ReplaceTradeEnchant", &Script_ReplaceTradeEnchant },
    { "NotWhileDeadError", &Script_NotWhileDeadError },
    { "GetRestState", &Script_GetRestState },
    { "GetXPExhaustion", &Script_GetXPExhaustion },
    { "GetTimeToWellRested", &Script_GetTimeToWellRested },
    { "GMRequestPlayerInfo", &Script_GMRequestPlayerInfo },
    { "GetCoinIcon", &Script_GetCoinIcon },
    { "GetCoinText", &Script_GetCoinText },
    { "GetCoinTextureString", &Script_GetCoinTextureString },
    { "IsSubZonePVPPOI", &Script_IsSubZonePVPPOI },
    { "GetZonePVPInfo", &Script_GetZonePVPInfo },
    { "TogglePVP", &Script_TogglePVP },
    { "SetPVP", &Script_SetPVP },
    { "GetPVPDesired", &Script_GetPVPDesired },
    { "GetPVPTimer", &Script_GetPVPTimer },
    { "IsPVPTimerRunning", &Script_IsPVPTimerRunning },
    { "ConfirmBindOnUse", &Script_ConfirmBindOnUse },
    { "ConfirmOnUse", &Script_ConfirmOnUse },
    { "SetPortraitToTexture", &Script_SetPortraitToTexture },
    { "GetLocale", &Script_GetLocale },
    { "GetGMTicketCategories", &Script_GetGMTicketCategories },
    { "DropItemOnUnit", &Script_DropItemOnUnit },
    { "RestartGx", &Script_RestartGx },
    { "GetBindLocation", &Script_GetBindLocation },
    { "ConfirmTalentWipe", &Script_ConfirmTalentWipe },
    { "ConfirmBinder", &Script_ConfirmBinder },
    { "ShowingHelm", &Script_ShowingHelm },
    { "ShowingCloak", &Script_ShowingCloak },
    { "ShowHelm", &Script_ShowHelm },
    { "ShowCloak", &Script_ShowCloak },
    { "SetEuropeanNumbers", &Script_SetEuropeanNumbers },
    { "GetAreaSpiritHealerTime", &Script_GetAreaSpiritHealerTime },
    { "AcceptAreaSpiritHeal", &Script_AcceptAreaSpiritHeal },
    { "CancelAreaSpiritHeal", &Script_CancelAreaSpiritHeal },
    { "GetMouseFocus", &Script_GetMouseFocus },
    { "GetRealmName", &Script_GetRealmName },
    { "GetItemQualityColor", &Script_GetItemQualityColor },
    { "GetItemInfo", &Script_GetItemInfo },
    { "GetItemGem", &Script_GetItemGem },
    { "GetExtendedItemInfo", &Script_GetExtendedItemInfo },
    { "GetItemIcon", &Script_GetItemIcon },
    { "GetItemFamily", &Script_GetItemFamily },
    { "GetItemCount", &Script_GetItemCount },
    { "GetItemSpell", &Script_GetItemSpell },
    { "GetItemCooldown", &Script_GetItemCooldown },
    { "PickupItem", &Script_PickupItem },
    { "IsCurrentItem", &Script_IsCurrentItem },
    { "IsUsableItem", &Script_IsUsableItem },
    { "IsHelpfulItem", &Script_IsHelpfulItem },
    { "IsHarmfulItem", &Script_IsHarmfulItem },
    { "IsConsumableItem", &Script_IsConsumableItem },
    { "IsEquippableItem", &Script_IsEquippableItem },
    { "IsEquippedItem", &Script_IsEquippedItem },
    { "IsEquippedItemType", &Script_IsEquippedItemType },
    { "IsDressableItem", &Script_IsDressableItem },
    { "ItemHasRange", &Script_ItemHasRange },
    { "IsItemInRange", &Script_IsItemInRange },
    { "GetNumAddOns", &Script_GetNumAddOns },
    { "GetAddOnInfo", &Script_GetAddOnInfo },
    { "GetAddOnMetadata", &Script_GetAddOnMetadata },
    { "UpdateAddOnMemoryUsage", &Script_UpdateAddOnMemoryUsage },
    { "GetAddOnMemoryUsage", &Script_GetAddOnMemoryUsage },
    { "GetScriptCPUUsage", &Script_GetScriptCPUUsage },
    { "UpdateAddOnCPUUsage", &Script_UpdateAddOnCPUUsage },
    { "GetAddOnCPUUsage", &Script_GetAddOnCPUUsage },
    { "GetFunctionCPUUsage", &Script_GetFunctionCPUUsage },
    { "GetFrameCPUUsage", &Script_GetFrameCPUUsage },
    { "GetEventCPUUsage", &Script_GetEventCPUUsage },
    { "ResetCPUUsage", &Script_ResetCPUUsage },
    { "GetCurrentEventID", &Script_GetCurrentEventID },
    { "GetEventTime", &Script_GetEventTime },
    { "GetAddOnDependencies", &Script_GetAddOnDependencies },
    { "GetAddOnOptionalDependencies", &Script_GetAddOnOptionalDependencies },
    { "EnableAddOn", &Script_EnableAddOn },
    { "EnableAllAddOns", &Script_EnableAllAddOns },
    { "DisableAddOn", &Script_DisableAddOn },
    { "DisableAllAddOns", &Script_DisableAllAddOns },
    { "ResetDisabledAddOns", &Script_ResetDisabledAddOns },
    { "IsAddOnLoadOnDemand", &Script_IsAddOnLoadOnDemand },
    { "IsAddOnLoaded", &Script_IsAddOnLoaded },
    { "LoadAddOn", &Script_LoadAddOn },
    { "PartialPlayTime", &Script_PartialPlayTime },
    { "NoPlayTime", &Script_NoPlayTime },
    { "GetBillingTimeRested", &Script_GetBillingTimeRested },
    { "CanShowResetInstances", &Script_CanShowResetInstances },
    { "ResetInstances", &Script_ResetInstances },
    { "IsInInstance", &Script_IsInInstance },
    { "GetInstanceDifficulty", &Script_GetInstanceDifficulty },
    { "GetInstanceInfo", &Script_GetInstanceInfo },
    { "IsOnTournamentRealm", &Script_IsOnTournamentRealm },
    { "GetDungeonDifficulty", &Script_GetDungeonDifficulty },
    { "SetDungeonDifficulty", &Script_SetDungeonDifficulty },
    { "GetRaidDifficulty", &Script_GetRaidDifficulty },
    { "SetRaidDifficulty", &Script_SetRaidDifficulty },
    { "ReportBug", &Script_ReportBug },
    { "ReportSuggestion", &Script_ReportSuggestion },
    { "GetMirrorTimerInfo", &Script_GetMirrorTimerInfo },
    { "GetMirrorTimerProgress", &Script_GetMirrorTimerProgress },
    { "GetNumTitles", &Script_GetNumTitles },
    { "GetCurrentTitle", &Script_GetCurrentTitle },
    { "SetCurrentTitle", &Script_SetCurrentTitle },
    { "IsTitleKnown", &Script_IsTitleKnown },
    { "GetTitleName", &Script_GetTitleName },
    { "UseItemByName", &Script_UseItemByName },
    { "EquipItemByName", &Script_EquipItemByName },
    { "InCombatLockdown", &Script_InCombatLockdown },
    { "StartAttack", &Script_StartAttack },
    { "StopAttack", &Script_StopAttack },
    { "SetTaxiBenchmarkMode", &Script_SetTaxiBenchmarkMode },
    { "GetTaxiBenchmarkMode", &Script_GetTaxiBenchmarkMode },
    { "Dismount", &Script_Dismount },
    { "VoicePushToTalkStart", &Script_VoicePushToTalkStart },
    { "VoicePushToTalkStop", &Script_VoicePushToTalkStop },
    { "SetUIVisibility", &Script_SetUIVisibility },
    { "IsReferAFriendLinked", &Script_IsReferAFriendLinked },
    { "CanGrantLevel", &Script_CanGrantLevel },
    { "GrantLevel", &Script_GrantLevel },
    { "CanSummonFriend", &Script_CanSummonFriend },
    { "SummonFriend", &Script_SummonFriend },
    { "GetSummonFriendCooldown", &Script_GetSummonFriendCooldown },
    { "GetTotemInfo", &Script_GetTotemInfo },
    { "GetTotemTimeLeft", &Script_GetTotemTimeLeft },
    { "TargetTotem", &Script_TargetTotem },
    { "DestroyTotem", &Script_DestroyTotem },
    { "GetNumDeclensionSets", &Script_GetNumDeclensionSets },
    { "DeclineName", &Script_DeclineName },
    { "AcceptLevelGrant", &Script_AcceptLevelGrant },
    { "DeclineLevelGrant", &Script_DeclineLevelGrant },
    { "UploadSettings", &Script_UploadSettings },
    { "DownloadSettings", &Script_DownloadSettings },
    { "GetMovieResolution", &Script_GetMovieResolution },
    { "GameMovieFinished", &Script_GameMovieFinished },
    { "IsDesaturateSupported", &Script_IsDesaturateSupported },
    { "GetThreatStatusColor", &Script_GetThreatStatusColor },
    { "IsThreatWarningEnabled", &Script_IsThreatWarningEnabled },
    { "ConsoleAddMessage", &Script_ConsoleAddMessage },
    { "GetItemUniqueness", &Script_GetItemUniqueness },
    { "EndRefund", &Script_EndRefund },
    { "EndBoundTradeable", &Script_EndBoundTradeable },
    { "CanMapChangeDifficulty", &Script_CanMapChangeDifficulty },
    { "ToggleAnimKitDisplay", &Script_ToggleAnimKitDisplay },
    { "GetDefaultRaidDifficulty", &Script_GetDefaultRaidDifficulty },
    { "GetCurrentRaidDifficulty", &Script_GetCurrentRaidDifficulty },
    { "GetAllowLowLevelRaid", &Script_GetAllowLowLevelRaid },
    { "SetAllowLowLevelRaid", &Script_SetAllowLowLevelRaid },
    { "GetAutoDeclineGuildInvites", &Script_GetAutoDeclineGuildInvites },
    { "SetAutoDeclineGuildInvites", &Script_SetAutoDeclineGuildInvites },
    { "GetCemeteryPreference", &Script_GetCemeteryPreference },
    { "SetCemeteryPreference", &Script_SetCemeteryPreference },
    { "IsCemeterySelectionAvailable", &Script_IsCemeterySelectionAvailable },
    { "GetFileStreamingStatus", &Script_GetFileStreamingStatus },
    { "GetBackgroundLoadingStatus", &Script_GetBackgroundLoadingStatus },
    { "GetAvailableBandwidth", &Script_GetAvailableBandwidth },
    { "GetDownloadedPercentage", &Script_GetDownloadedPercentage },
    { "MouseOverrideCinematicDisable", &Script_MouseOverrideCinematicDisable },
    { "GetMaxSpellStartRecoveryOffset", &Script_GetMaxSpellStartRecoveryOffset },
    { "ValidateTimer", &Script_ValidateTimer },
    { "GetRestrictedAccountData", &Script_GetRestrictedAccountData },
    { "HasTravelPass", &Script_HasTravelPass },
    { "SendSoRByText", &Script_SendSoRByText },
    { "CanSendSoRByText", &Script_CanSendSoRByText },
    { "GetNumSoRRemaining", &Script_GetNumSoRRemaining },
};

// OFFSET: 0x008C3AA0
void LoadScriptFunctions() {
    RegisterSimpleFrameScriptMethods();
    CGTooltip::CreateScriptMetaTable();
    CGCooldown::CreateScriptMetaTable();
    CGMinimapFrame::CreateScriptMetaTable();
    CGCharacterModelBase::CreateScriptMetaTable();
    CGDressUpModelFrame::CreateScriptMetaTable();
    CGTabardModelFrame::CreateScriptMetaTable();
    CGBlobFrame::CreateScriptMetaTable();
    CGQuestPOIFrame::CreateScriptMetaTable();
    CGArchaeologyDigSiteFrame::CreateScriptMetaTable();

    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_RegisterFunction(s_ScriptFunctions[i].m_name, s_ScriptFunctions[i].m_method);
    }

    UIBindingsRegisterScriptFunctions();
    UIMacrosRegisterScriptFunctions();
    InputControlRegisterScriptFunctions();
    CameraRegisterScriptFunctions();
    SI3::RegisterScriptFunctions();
    SpellRegisterScriptFunctions();
    ScriptEventsRegisterFunctions();
    MinimapRegisterScriptFunctions();
    UnitCombatLogRegisterScriptFunctions();
    ActionBarRegisterScriptFunctions();
    SpellActivationOverlayRegisterScriptFunctions();
    CFlyout::RegisterScriptFunctions();
    CGPartyInfo::RegisterScriptFunctions();
    ChatRegisterScriptFunctions();
    AutoCompleteRegisterScriptFunctions();
    SpellBookRegisterScriptFunctions();
    CharacterInfoRegisterScriptFunctions();
    LootInfoRegisterScriptFunctions();
    ItemTextRegisterScriptFunctions();
    GossipInfoRegisterScriptFunctions();
    QuestInfoRegisterScriptFunctions();
    QuestLogRegisterScriptFunctions();
    CGTaxiMap::RegisterScriptFunctions();
    ClassTrainerRegisterScriptFunctions();
    TradeSkillRegisterScriptFunctions();
    MerchantRegisterScriptFunctions();
    TradeInfoRegisterScriptFunctions();
    ContainerRegisterScriptFunctions();
    BankRegisterScriptFunctions();
    FriendList::RegisterScriptFunctions();
    PetInfoRegisterScriptFunctions();
    WorldMapRegisterScriptFunctions();
    ReputationInfoRegisterScriptFunctions();
    TabardCreationRegisterScriptFunctions();
    GuildRegistrarRegisterScriptFunctions();
    DuelInfoRegisterScriptFunctions();
    TutorialRegisterScriptFunctions();
    PetitionInfoRegisterScriptFunctions();
    SkillInfoRegisterScriptFunctions();
    GuildInfoRegisterScriptFunctions();
    LFGuildRegisterScriptFunctions();
    MailInfoRegisterScriptFunctions();
    BattlefieldInfoRegisterScriptFunctions();
    TalentInfoRegisterScriptFunctions();
    AuctionHouseRegisterScriptFunctions();
    StableInfoRegisterScriptFunctions();
    RaidInfoRegisterScriptFunctions();
    WorldStateInfoRegisterScriptFunctions();
    MinigameInfoRegisterScriptFunctions();
    KnowledgeBase_RegisterScriptFunctions();
    ItemSocketInfoRegisterScriptFunctions();
    PetitionVendorRegisterScriptFunctions();
    ArenaTeamInfoRegisterScriptFunctions();
    LookingForGroupRegisterScriptFunctions();
    VoiceRegisterScriptFunctions();
    CalendarRegisterScriptFunctions();
    CGCommentator::RegisterScriptFunctions();
    CGBarberShop::RegisterScriptFunctions();
    CGDanceStudio::RegisterScriptFunctions();
    GuildBankRegisterScriptFunctions();
    GlyphInfoRegisterScriptFunctions();
    AchievementInfoRegisterScriptFunctions();
    CurrencyTypesRegisterScriptFunctions();
    CGVideoOptions::RegisterScriptFunctions();
    EquipmentManagerRegisterScriptFunctions();
    GMTicketInfoRegisterScriptFunctions();
    BattlenetUI_RegisterScriptFunctions();
    ReforgeRegisterScriptFunctions();
    ResearchRegisterScriptFunctions();
    NamePlateRegisterScriptFunctions();
    EncounterJournalRegisterScriptFunctions();
    CompactUnitFrameProfilesRegisterScriptFunctions();
    IncomingResurrectionRegisterScriptFunctions();
    TransmogrifyRegisterScriptFunctions();
}

// OFFSET: 0x008C3C80
void UnloadScriptFunctions() {
    UnregisterSimpleFrameScriptMethods();
    CGTooltip::DestroyScriptMetaTable();
    CGCooldown::DestroyScriptMetaTable();
    CGMinimapFrame::DestroyScriptMetaTable();
    CGCharacterModelBase::DestroyScriptMetaTable();
    CGDressUpModelFrame::DestroyScriptMetaTable();
    CGTabardModelFrame::DestroyScriptMetaTable();
    CGBlobFrame::DestroyScriptMetaTable();
    CGQuestPOIFrame::DestroyScriptMetaTable();
    CGArchaeologyDigSiteFrame::DestroyScriptMetaTable();

    for (uint32_t i = 0; i < sizeof(s_ScriptFunctions) / sizeof(s_ScriptFunctions[0]); i++) {
        FrameScript_UnregisterFunction(s_ScriptFunctions[i].m_name);
    }

    UIBindingsUnegisterScriptFunctions();
    UIMacrosUnegisterScriptFunctions();
    InputControlUnregisterScriptFunctions();
    CameraUnregisterScriptFunctions();
    SI3::UnregisterScriptFunctions();
    SpellUnregisterScriptFunctions();
    MinimapUnregisterScriptFunctions();
    ScriptEventsUnregisterFunctions();
    UnitCombatLogUnregisterScriptFunctions();
    ActionBarUnregisterScriptFunctions();
    SpellActivationOverlayUnregisterScriptFunctions();
    CFlyout::UnregisterScriptFunctions();
    CGPartyInfo::UnregisterScriptFunctions();
    ChatUnregisterScriptFunctions();
    AutoCompleteUnregisterScriptFunctions();
    SpellBookUnregisterScriptFunctions();
    CharacterInfoUnregisterScriptFunctions();
    LootInfoUnregisterScriptFunctions();
    ItemTextUnregisterScriptFunctions();
    GossipInfoUnregisterScriptFunctions();
    QuestInfoUnregisterScriptFunctions();
    QuestLogUnregisterScriptFunctions();
    CGTaxiMap::UnregisterScriptFunctions();
    ClassTrainerUnregisterScriptFunctions();
    TradeSkillUnregisterScriptFunctions();
    MerchantUnregisterScriptFunctions();
    TradeInfoUnregisterScriptFunctions();
    ContainerUnregisterScriptFunctions();
    BankUnregisterScriptFunctions();
    FriendList::UnregisterScriptFunctions();
    PetInfoUnregisterScriptFunctions();
    WorldMapUnregisterScriptFunctions();
    ReputationInfoUnregisterScriptFunctions();
    TabardCreationUnregisterScriptFunctions();
    GuildRegistrarUnregisterScriptFunctions();
    DuelInfoUnregisterScriptFunctions();
    TutorialUnregisterScriptFunctions();
    PetitionInfoUnregisterScriptFunctions();
    SkillInfoUnregisterScriptFunctions();
    GuildInfoUnregisterScriptFunctions();
    MailInfoUnregisterScriptFunctions();
    BattlefieldInfoUnregisterScriptFunctions();
    TalentInfoUnregisterScriptFunctions();
    AuctionHouseUnregisterScriptFunctions();
    StableInfoUnregisterScriptFunctions();
    RaidInfoUnregisterScriptFunctions();
    WorldStateInfoUnregisterScriptFunctions();
    MinigameInfoUnregisterScriptFunctions();
    KnowledgeBase_UnregisterScriptFunctions();
    ItemSocketInfoUnregisterScriptFunctions();
    PetitionVendorUnregisterScriptFunctions();
    ArenaTeamInfoUnregisterScriptFunctions();
    LookingForGroupUnregisterScriptFunctions();
    VoiceUnregisterScriptFunctions();
    CalendarUnregisterScriptFunctions();
    CGCommentator::UnregisterScriptFunctions();
    CGBarberShop::UnregisterScriptFunctions();
    CGDanceStudio::UnregisterScriptFunctions();
    GuildBankUnregisterScriptFunctions();
    GlyphInfoUnregisterScriptFunctions();
    AchievementInfoUnregisterScriptFunctions();
    CurrencyTypesUnregisterScriptFunctions();
    CGVideoOptions::UnregisterScriptFunctions();
    EquipmentManagerUnregisterScriptFunctions();
    GMTicketInfoUnregisterScriptFunctions();
    BattlenetUI_UnregisterScriptFunctions();
    ReforgeUnregisterScriptFunctions();
    ResearchUnregisterScriptFunctions();
    NamePlateUnregisterScriptFunctions();
    EncounterJournalUnregisterScriptFunctions();
    CompactUnitFrameProfilesUnregisterScriptFunctions();
    IncomingResurrectionUnregisterScriptFunctions();
    TransmogrifyUnregisterScriptFunctions();
}

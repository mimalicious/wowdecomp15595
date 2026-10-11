#include "GameUI.h"
#include "WoW/Source/Object/ObjectClient/VoidStorage_C.h"
#include "WoW/Source/DB/Rec/ItemRec.h"
#include "WoW/Source/StaticSingleton.h"
#include "WoW/Source/UIUtil/CameraManager.h"
#include "Engine/Source/Event/FrameTime.h"
#include "Engine/Source/BlizzardCore/Source/Packages/Streaming/WoWReport.h"
#include "WoW/Source/Ui/CGQuestObjectiveCache.h"
#include "WoW/Source/Ui/SpellActivationOverlay.h"
#include "WoW/Source/Ui/GlyphInfo.h"
#include "WoW/Source/Ui/Reforge.h"
#include "WoW/Source/Ui/Transmogrify.h"
#include "WoW/Source/Ui/IncomingResurrection.h"
#include "WoW/Source/Ui/VoidStorageInfo.h"
#include "WoW/Source/JamClientAutoCode/JamUserClient.h"
#include "WoW/Source/Ui/TalentInfo.h"
#include "WoW/Source/Ui/Flyout.h"
#include "WoW/Source/Ui/LFGuildInfo.h"
#include "WoW/Source/Ui/EncounterJournal.h"
#include "WoW/Source/Ui/CUFProfiles.h"
#include "WoW/Source/Ui/TradeFrame.h"
#include "WoW/Source/Ui/ResearchFrame.h"
#include "WoW/Source/Glue/CharacterSelection.h"
#include "Engine/Source/Services/PreloadList.h"
#include "Engine/Source/Frame/Movie.h"
#include "WoW/Source/Game/GameClient/PhaseShift_C.h"
#include "WoW/Source/JamClientAutoCode/JamClient.h"
#include "WoW/Source/DB/Rec/FileDataRec.h"
#include "WoW/Source/DB/Rec/MovieFileDataRec.h"
#include "WoW/Source/DB/Rec/MovieRec.h"
#include "WoW/Source/DB/Rec/MovieVariationRec.h"
#include "WoW/Source/Ui/CKBSystem.h"
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
#include "Engine/Source/Frame/CSimpleFrame.h"
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
#include "Engine/Source/ShaderEffect/ShadowCache.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/Storm/Source/MD5.h"
#include "Engine/Source/Tempest/CImVector.h"
#include "Engine/Source/Tempest/CRect.h"
#include "WoW/Common/ComponentCore/CharacterComponent.h"
#include "WoW/Source/Client.h"
#include "WoW/Source/DB/DBClient/DBCacheInstances.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/AreaTableRec.h"
#include "WoW/Source/DB/Rec/ItemSubClassRec.h"
#include "WoW/Source/DB/Rec/MapRec.h"
#include "WoW/Source/DB/Rec/SpellItemEnchantmentRec.h"
#include "WoW/Source/DB/Rec/VehicleSeatRec.h"
#include "WoW/Source/Game/GameClient/ComSatClient.h"
#include "WoW/Source/Game/GameClient/PlayerName.h"
#include "WoW/Source/Glue/CGlueMgr.h"
#include "WoW/Source/Glue/CWOWClientStatus.h"
#include "WoW/Source/LoadingScreen.h"
#include "WoW/Source/Magic/MagicClient/Spell_C.h"
#include "WoW/Source/Net/Types.h"
#include "WoW/Source/Object/ObjectClient/Bag_C.h"
#include "WoW/Source/Object/ObjectClient/GameObject_C.h"
#include "WoW/Source/Object/ObjectClient/Item_C.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/Object/ObjectClient/SimpleScript.h"
#include "WoW/Source/Object/ObjectClient/Unit_C.h"
#include "WoW/Source/Object/ObjectClient/UnitCombatLog_C.h"
#include "WoW/Source/Object/ObjectClient/Vehicle_C.h"
#include "WoW/Source/Object/ObjectClient/VehiclePassenger_C.h"
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
#include "WoW/Source/WowSvcs/WowSvcsClient/AccountData.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/FriendList.h"
#include <malloc.h>
#include "Engine/Source/Tempest/CMath.h"
#include "WoW/Source/DB/Rec/WeatherRec.h"
#include "WoW/Source/Object/CMoveSpline.h"
#include "WoW/Source/Object/ObjectClient/Corpse_C.h"
#include "WoW/Source/DayNight/DayNight.h"
#include "WoW/Source/WorldClient/MapWeather.h"
#include <ctime>
#include "Engine/Source/Event/CFocusEvent.h"
#include "Engine/Source/Frame/CLayoutFrame.h"
#include "Engine/Source/Model2/M2Scene.h"
#include "Engine/Source/Scrn/Scrn.h"
#include "Engine/Source/Services/AsyncFile.h"
#include "Engine/Source/Services/Camera.h"
#include "WoW/Source/DB/Rec/CinematicCameraRec.h"
#include "WoW/Source/DB/Rec/CinematicSequencesRec.h"
#include "WoW/Source/Game/GameClient/NameCache.h"
#include "WoW/Source/Object/ObjectClient/Movement_C.h"
#include "WoW/Source/WowServices/WDataStore.h"
#include <cfloat>
#include "Engine/Source/Tempest/C2Vector.h"
#include "Engine/Source/Tempest/C33Matrix.h"
#include "WoW/Source/DB/Rec/CreatureFamilyRec.h"
#include "WoW/Source/DB/Rec/CreatureTypeRec.h"

// STATIC: 0x00BE4DD0
static const uint8_t s_parentalControlsWarningTimes[] = { 1, 2, 3, 4, 5, 10, 15, 0 };

// STATIC: 0x00D0FF9C
static int32_t s_cemeteryListRequestDelay = 1000;

// STATIC: 0x00ED758C
uint32_t CGGameUI::s_numCemeteries;

// STATIC: 0x00ED7590
uint32_t CGGameUI::s_cemeteries[16];

// STATIC: 0x00ED757C
uint32_t CGGameUI::s_cemeteryPreference;

// STATIC: 0x00D0B2AC
int32_t CGGameUI::s_parentalControlsWarning;

// STATIC: 0x00ED7610
int32_t CGGameUI::s_streamingWait;

// STATIC: 0x00ED7604
int32_t CGGameUI::s_streamingIcon;

// STATIC: 0x00ED7608
WOWGUID CGGameUI::m_lastEnchantItem;

// STATIC: 0x00ED75D4
int32_t CGGameUI::s_weeklyResetTime;

// STATIC: 0x00ED75DA
bool CGGameUI::s_sessionAlertEnabled;

// STATIC: 0x00ED75DC
int32_t CGGameUI::s_sessionAlertInterval;

// STATIC: 0x00ED75E0
int32_t CGGameUI::s_sessionAlertIntervalAlt;

// STATIC: 0x00ED75E4
int32_t CGGameUI::s_sessionAlertTime;

// STATIC: 0x00ED7588
uint32_t CGGameUI::s_cemeteryListRequestTime;

// STATIC: 0x00ED748C
int32_t CGGameUI::m_cursorCurrency;

// STATIC: 0x00ED7490
int32_t CGGameUI::m_cursorFlyout;

// STATIC: 0x00ED7498
WOWGUID CGGameUI::m_cursorVoidStorageItem;

// STATIC: #0x00ACC3F8
CImVector s_deadSelectionColor = CImVector(0x7F, 0x7F, 0x7F, 0xFF);

// STATIC: 0x00D0B254
const char* g_macroTaint = "";

// STATIC: #0x00AC8088
static const char* s_readOnlyCVars[6] = { "realmList", "realmName", "decorateAccountName", "realmlistbn", "portal", "serverAlert" };

// STATIC: 0x00D0BD30
static UIErrorInfo s_errorInfo[ERR_MAX_VALUE] = {
    { "ERR_INV_FULL", 2, "NONE", 0, 0 },
    { "ERR_BANK_FULL", 2, "NONE", 68, 0 },
    { "ERR_CANT_EQUIP_LEVEL_I", 2, "NONE", 2, 0 },
    { "ERR_CANT_EQUIP_SKILL", 2, "NONE", 41, 0 },
    { "ERR_CANT_EQUIP_EVER", 2, "NONE", 3, 0 },
    { "ERR_CANT_EQUIP_RANK", 2, "NONE", 68, 0 },
    { "ERR_CANT_EQUIP_RATING", 2, "NONE", 68, 0 },
    { "ERR_CANT_EQUIP_REPUTATION", 2, "NONE", 68, 0 },
    { "ERR_PROFICIENCY_NEEDED", 2, "NONE", 48, 0 },
    { "ERR_WRONG_SLOT", 2, "NONE", 27, 0 },
    { "ERR_CANT_EQUIP_NEED_TALENT", 2, "NONE", 41, 0 },
    { "ERR_BAG_FULL", 2, "NONE", 29, 0 },
    { "ERR_INTERNAL_BAG_ERROR", 2, "NONE", 68, 0 },
    { "ERR_DESTROY_NONEMPTY_BAG", 2, "NONE", 68, 0 },
    { "ERR_BAG_IN_BAG", 2, "NONE", 26, 0 },
    { "ERR_TOO_MANY_SPECIAL_BAGS", 2, "NONE", 68, 0 },
    { "ERR_TRADE_EQUIPPED_BAG", 2, "NONE", 68, 0 },
    { "ERR_AMMO_ONLY", 2, "NONE", 28, 0 },
    { "ERR_NO_SLOT_AVAILABLE", 2, "NONE", 68, 0 },
    { "ERR_WRONG_BAG_TYPE", 2, "NONE", 68, 0 },
    { "ERR_ITEM_MAX_COUNT", 2, "NONE", 30, 0 },
    { "ERR_NOT_EQUIPPABLE", 2, "NONE", 44, 0 },
    { "ERR_CANT_STACK", 2, "NONE", 68, 0 },
    { "ERR_CANT_SWAP", 2, "NONE", 68, 0 },
    { "ERR_SLOT_EMPTY", 2, "NONE", 68, 0 },
    { "ERR_ITEM_NOT_FOUND", 2, "NONE", 68, 0 },
    { "ERR_TOO_FEW_TO_SPLIT", 2, "NONE", 68, 0 },
    { "ERR_SPLIT_FAILED", 2, "NONE", 68, 0 },
    { "ERR_NOT_A_BAG", 2, "NONE", 25, 0 },
    { "ERR_NOT_OWNER", 2, "NONE", 68, 0 },
    { "ERR_ONLY_ONE_QUIVER", 2, "NONE", 68, 0 },
    { "ERR_NO_BANK_SLOT", 2, "NONE", 68, 0 },
    { "ERR_NO_BANK_HERE", 2, "NONE", 68, 0 },
    { "ERR_ITEM_LOCKED", 2, "NONE", 61, 0 },
    { "ERR_2HANDED_EQUIPPED", 2, "NONE", 42, 0 },
    { "ERR_VENDOR_NOT_INTERESTED", 2, "NONE", 68, 0 },
    { "ERR_VENDOR_HATES_YOU", 2, "NONE", 68, 0 },
    { "ERR_VENDOR_SOLD_OUT", 2, "NONE", 68, 0 },
    { "ERR_VENDOR_TOO_FAR", 2, "NONE", 68, 0 },
    { "ERR_VENDOR_DOESNT_BUY", 2, "NONE", 68, 0 },
    { "ERR_NOT_ENOUGH_MONEY", 2, "NONE", 40, 0 },
    { "ERR_RECEIVE_ITEM_S", 0, "ITEMGENERICSOUND", 68, 0 },
    { "ERR_DROP_BOUND_ITEM", 2, "NONE", 4, 0 },
    { "ERR_TRADE_BOUND_ITEM", 2, "NONE", 59, 0 },
    { "ERR_TRADE_QUEST_ITEM", 2, "NONE", 59, 0 },
    { "ERR_TRADE_TEMP_ENCHANT_BOUND", 2, "NONE", 59, 0 },
    { "ERR_TRADE_GROUND_ITEM", 2, "NONE", 68, 0 },
    { "ERR_TRADE_BAG", 2, "NONE", 68, 0 },
    { "ERR_SPELL_FAILED_S", 2, "NONE", 68, 0 },
    { "ERR_ITEM_COOLDOWN", 2, "NONE", 5, 0 },
    { "ERR_POTION_COOLDOWN", 2, "NONE", 47, 0 },
    { "ERR_FOOD_COOLDOWN", 2, "NONE", 7, 0 },
    { "ERR_SPELL_COOLDOWN", 2, "NONE", 12, 0 },
    { "ERR_ABILITY_COOLDOWN", 2, "NONE", 50, 0 },
    { "ERR_SPELL_ALREADY_KNOWN_S", 2, "NONE", 13, 0 },
    { "ERR_PET_SPELL_ALREADY_KNOWN_S", 2, "NONE", 68, 0 },
    { "ERR_PROFICIENCY_GAINED_S", 0, "NONE", 68, 26 },
    { "ERR_SKILL_GAINED_S", 0, "NONE", 68, 26 },
    { "ERR_SKILL_UP_SI", 0, "NONE", 68, 26 },
    { "ERR_LEARN_SPELL_S", 0, "NONE", 68, 0 },
    { "ERR_LEARN_ABILITY_S", 0, "NONE", 68, 0 },
    { "ERR_LEARN_RECIPE_S", 0, "NONE", 68, 0 },
    { "ERR_LEARN_COMPANION_S", 0, "NONE", 68, 0 },
    { "ERR_INVITE_PLAYER_S", 0, "NONE", 68, 0 },
    { "ERR_INVITE_SELF", 2, "NONE", 68, 0 },
    { "ERR_INVITED_TO_GROUP_SS", 0, "NONE", 68, 0 },
    { "ERR_INVITED_ALREADY_IN_GROUP_SS", 0, "NONE", 20, 0 },
    { "ERR_ALREADY_IN_GROUP_S", 0, "NONE", 20, 0 },
    { "ERR_PLAYER_BUSY_S", 0, "NONE", 68, 0 },
    { "ERR_NEW_LEADER_S", 0, "NONE", 68, 0 },
    { "ERR_NEW_LEADER_YOU", 0, "NONE", 68, 0 },
    { "ERR_NEW_GUIDE_S", 0, "NONE", 68, 0 },
    { "ERR_NEW_GUIDE_YOU", 0, "NONE", 68, 0 },
    { "ERR_LEFT_GROUP_S", 0, "NONE", 68, 0 },
    { "ERR_LEFT_GROUP_YOU", 0, "NONE", 68, 0 },
    { "ERR_GROUP_DISBANDED", 0, "NONE", 68, 0 },
    { "ERR_DECLINE_GROUP_S", 0, "igPlayerInviteDecline", 68, 0 },
    { "ERR_JOINED_GROUP_S", 0, "NONE", 68, 0 },
    { "ERR_UNINVITE_YOU", 0, "NONE", 68, 0 },
    { "ERR_BAD_PLAYER_NAME_S", 0, "NONE", 68, 0 },
    { "ERR_NOT_IN_GROUP", 0, "NONE", 68, 0 },
    { "ERR_TARGET_NOT_IN_GROUP_S", 0, "NONE", 68, 0 },
    { "ERR_TARGET_NOT_IN_INSTANCE_S", 0, "NONE", 68, 0 },
    { "ERR_GROUP_FULL", 0, "NONE", 8, 0 },
    { "ERR_NOT_LEADER", 0, "NONE", 68, 0 },
    { "ERR_PLAYER_DIED_S", 0, "NONE", 68, 0 },
    { "ERR_GUILD_CREATE_S", 0, "LEVELUP", 68, 0 },
    { "ERR_GUILD_INVITE_S", 0, "NONE", 68, 0 },
    { "ERR_INVITED_TO_GUILD_SSS", 0, "LEVELUP", 68, 0 },
    { "ERR_ALREADY_IN_GUILD_S", 0, "NONE", 68, 0 },
    { "ERR_ALREADY_INVITED_TO_GUILD_S", 0, "NONE", 68, 0 },
    { "ERR_INVITED_TO_GUILD", 0, "NONE", 68, 0 },
    { "ERR_ALREADY_IN_GUILD", 0, "NONE", 68, 0 },
    { "ERR_GUILD_ACCEPT", 0, "NONE", 68, 0 },
    { "ERR_GUILD_DECLINE_S", 0, "NONE", 68, 0 },
    { "ERR_GUILD_PERMISSIONS", 0, "NONE", 62, 0 },
    { "ERR_GUILD_JOIN_S", 0, "NONE", 68, 0 },
    { "ERR_GUILD_FOUNDER_S", 0, "NONE", 68, 0 },
    { "ERR_GUILD_PROMOTE_SSS", 0, "NONE", 68, 0 },
    { "ERR_GUILD_DEMOTE_SSS", 0, "NONE", 68, 0 },
    { "ERR_GUILD_QUIT_S", 0, "NONE", 68, 0 },
    { "ERR_GUILD_LEAVE_S", 0, "NONE", 68, 0 },
    { "ERR_GUILD_REMOVE_SS", 0, "NONE", 68, 0 },
    { "ERR_GUILD_REMOVE_SELF", 0, "NONE", 68, 0 },
    { "ERR_GUILD_DISBAND_S", 0, "NONE", 68, 0 },
    { "ERR_GUILD_DISBAND_SELF", 0, "NONE", 68, 0 },
    { "ERR_GUILD_LEADER_S", 0, "NONE", 68, 0 },
    { "ERR_GUILD_LEADER_SELF", 0, "NONE", 68, 0 },
    { "ERR_GUILD_PLAYER_NOT_FOUND_S", 0, "NONE", 68, 0 },
    { "ERR_GUILD_PLAYER_NOT_IN_GUILD_S", 0, "NONE", 68, 0 },
    { "ERR_GUILD_PLAYER_NOT_IN_GUILD", 0, "NONE", 68, 0 },
    { "ERR_GUILD_CANT_PROMOTE_S", 0, "NONE", 68, 0 },
    { "ERR_GUILD_CANT_DEMOTE_S", 0, "NONE", 68, 0 },
    { "ERR_GUILD_NOT_IN_A_GUILD", 0, "NONE", 68, 0 },
    { "ERR_GUILD_INTERNAL", 0, "NONE", 68, 0 },
    { "ERR_GUILD_LEADER_IS_S", 0, "NONE", 68, 0 },
    { "ERR_GUILD_LEADER_CHANGED_SS", 0, "NONE", 68, 0 },
    { "ERR_GUILD_DISBANDED", 0, "NONE", 68, 0 },
    { "ERR_GUILD_NOT_ALLIED", 0, "NONE", 68, 0 },
    { "ERR_GUILD_LEADER_LEAVE", 0, "NONE", 68, 0 },
    { "ERR_GUILD_RANKS_LOCKED", 0, "NONE", 68, 0 },
    { "ERR_GUILD_RANK_IN_USE", 0, "NONE", 68, 0 },
    { "ERR_GUILD_RANK_TOO_HIGH_S", 0, "NONE", 68, 0 },
    { "ERR_GUILD_RANK_TOO_LOW_S", 0, "NONE", 68, 0 },
    { "ERR_GUILD_NAME_EXISTS_S", 2, "NONE", 68, 0 },
    { "ERR_GUILD_WITHDRAW_LIMIT", 2, "NONE", 68, 0 },
    { "ERR_GUILD_NOT_ENOUGH_MONEY", 2, "NONE", 68, 0 },
    { "ERR_GUILD_BANK_CONJURED_ITEM", 2, "NONE", 68, 0 },
    { "ERR_GUILD_BANK_EQUIPPED_ITEM", 2, "NONE", 68, 0 },
    { "ERR_GUILD_BANK_BOUND_ITEM", 2, "NONE", 68, 0 },
    { "ERR_GUILD_BANK_QUEST_ITEM", 2, "NONE", 68, 0 },
    { "ERR_GUILD_BANK_WRAPPED_ITEM", 2, "NONE", 68, 0 },
    { "ERR_GUILD_BANK_FULL", 2, "NONE", 68, 0 },
    { "ERR_NO_GUILD_CHARTER", 2, "NONE", 68, 0 },
    { "ERR_OUT_OF_RANGE", 2, "NONE", 10, 0 },
    { "ERR_PLAYER_DEAD", 2, "NONE", 68, 0 },
    { "ERR_CLIENT_LOCKED_OUT", 2, "NONE", 68, 0 },
    { "ERR_KILLED_BY_S", 0, "NONE", 68, 0 },
    { "ERR_LOOT_LOCKED", 2, "NONE", 33, 0 },
    { "ERR_LOOT_TOO_FAR", 2, "NONE", 35, 0 },
    { "ERR_LOOT_DIDNT_KILL", 2, "NONE", 31, 0 },
    { "ERR_LOOT_BAD_FACING", 2, "NONE", 32, 0 },
    { "ERR_LOOT_NOTSTANDING", 2, "NONE", 68, 0 },
    { "ERR_LOOT_STUNNED", 2, "NONE", 68, 0 },
    { "ERR_LOOT_NO_UI", 2, "NONE", 68, 0 },
    { "ERR_LOOT_WHILE_INVULNERABLE", 2, "NONE", 68, 0 },
    { "ERR_QUEST_ACCEPTED_S", 0, "QUESTADDED", 68, 0 },
    { "ERR_QUEST_COMPLETE_S", 0, "NONE", 68, 0 },
    { "ERR_QUEST_FAILED_S", 0, "igQuestFailed", 68, 0 },
    { "ERR_QUEST_FAILED_BAG_FULL_S", 0, "igQuestFailed", 68, 0 },
    { "ERR_QUEST_FAILED_MAX_COUNT_S", 0, "igQuestFailed", 68, 0 },
    { "ERR_QUEST_FAILED_LOW_LEVEL", 0, "igQuestFailed", 68, 0 },
    { "ERR_QUEST_FAILED_MISSING_ITEMS", 0, "igQuestFailed", 68, 0 },
    { "ERR_QUEST_FAILED_WRONG_RACE", 0, "igQuestFailed", 68, 0 },
    { "ERR_QUEST_FAILED_NOT_ENOUGH_MONEY", 0, "igQuestFailed", 68, 0 },
    { "ERR_QUEST_FAILED_EXPANSION", 0, "igQuestFailed", 68, 0 },
    { "ERR_QUEST_ONLY_ONE_TIMED", 0, "igQuestFailed", 68, 0 },
    { "ERR_QUEST_NEED_PREREQS", 0, "igQuestFailed", 68, 0 },
    { "ERR_QUEST_ALREADY_ON", 0, "igQuestFailed", 68, 0 },
    { "ERR_QUEST_ALREADY_DONE", 0, "igQuestFailed", 68, 0 },
    { "ERR_QUEST_ALREADY_DONE_DAILY", 0, "igQuestFailed", 68, 0 },
    { "ERR_QUEST_REWARD_EXP_I", 0, "NONE", 68, 0 },
    { "ERR_QUEST_REWARD_ITEM_S", 0, "NONE", 68, 0 },
    { "ERR_QUEST_REWARD_ITEM_MULT_IS", 0, "NONE", 68, 0 },
    { "ERR_QUEST_REWARD_MONEY_S", 0, "NONE", 68, 0 },
    { "ERR_QUEST_MUST_CHOOSE", 2, "igQuestFailed", 68, 0 },
    { "ERR_QUEST_LOG_FULL", 2, "igQuestFailed", 68, 0 },
    { "ERR_COMBAT_DAMAGE_SSI", 0, "NONE", 68, 0 },
    { "ERR_INSPECT_S", 0, "NONE", 68, 0 },
    { "ERR_CANT_USE_ITEM", 2, "NONE", 51, 0 },
    { "ERR_CANT_USE_ITEM_IN_ARENA", 2, "NONE", 51, 0 },
    { "ERR_MUST_EQUIP_ITEM", 2, "NONE", 49, 0 },
    { "ERR_PASSIVE_ABILITY", 2, "NONE", 68, 0 },
    { "ERR_2HSKILLNOTFOUND", 2, "NONE", 43, 0 },
    { "ERR_NO_ATTACK_TARGET", 2, "NONE", 38, 0 },
    { "ERR_INVALID_ATTACK_TARGET", 2, "NONE", 11, 0 },
    { "ERR_ATTACK_STUNNED", 2, "NONE", 68, 0 },
    { "ERR_ATTACK_PACIFIED", 2, "NONE", 68, 0 },
    { "ERR_ATTACK_MOUNTED", 2, "NONE", 68, 0 },
    { "ERR_ATTACK_FLEEING", 2, "NONE", 68, 0 },
    { "ERR_ATTACK_CONFUSED", 2, "NONE", 68, 0 },
    { "ERR_ATTACK_CHARMED", 2, "NONE", 68, 0 },
    { "ERR_ATTACK_DEAD", 2, "NONE", 68, 0 },
    { "ERR_ATTACK_PREVENTED_BY_MECHANIC_S", 2, "NONE", 68, 0 },
    { "ERR_ATTACK_CHANNEL", 2, "NONE", 68, 0 },
    { "ERR_TAXISAMENODE", 2, "NONE", 68, 0 },
    { "ERR_TAXINOSUCHPATH", 1, "NONE", 68, 0 },
    { "ERR_TAXIUNSPECIFIEDSERVERERROR", 2, "NONE", 68, 0 },
    { "ERR_TAXINOTENOUGHMONEY", 1, "NONE", 54, 0 },
    { "ERR_TAXITOOFARAWAY", 1, "NONE", 68, 0 },
    { "ERR_TAXINOVENDORNEARBY", 1, "NONE", 68, 0 },
    { "ERR_TAXINOTVISITED", 1, "NONE", 68, 0 },
    { "ERR_TAXIPLAYERBUSY", 2, "NONE", 68, 0 },
    { "ERR_TAXIPLAYERALREADYMOUNTED", 2, "NONE", 68, 0 },
    { "ERR_TAXIPLAYERSHAPESHIFTED", 2, "NONE", 68, 0 },
    { "ERR_TAXIPLAYERMOVING", 1, "NONE", 68, 0 },
    { "ERR_TAXINOPATHS", 2, "NONE", 68, 0 },
    { "ERR_TAXINOTSTANDING", 1, "NONE", 68, 0 },
    { "ERR_NO_REPLY_TARGET", 0, "NONE", 68, 0 },
    { "ERR_GENERIC_NO_TARGET", 2, "NONE", 45, 0 },
    { "ERR_INITIATE_TRADE_S", 0, "NONE", 68, 0 },
    { "ERR_TRADE_REQUEST_S", 0, "LEVELUP", 68, 0 },
    { "ERR_TRADE_BLOCKED_S", 0, "NONE", 68, 0 },
    { "ERR_TRADE_TARGET_DEAD", 2, "NONE", 68, 0 },
    { "ERR_TRADE_TOO_FAR", 2, "NONE", 68, 0 },
    { "ERR_TRADE_CANCELLED", 1, "NONE", 68, 0 },
    { "ERR_TRADE_COMPLETE", 1, "NONE", 68, 0 },
    { "ERR_TRADE_BAG_FULL", 2, "NONE", 68, 0 },
    { "ERR_TRADE_TARGET_BAG_FULL", 2, "NONE", 68, 0 },
    { "ERR_TRADE_MAX_COUNT_EXCEEDED", 2, "NONE", 68, 0 },
    { "ERR_TRADE_TARGET_MAX_COUNT_EXCEEDED", 2, "NONE", 68, 0 },
    { "ERR_ALREADY_TRADING", 2, "NONE", 68, 0 },
    { "ERR_MOUNT_INVALIDMOUNTEE", 2, "NONE", 68, 0 },
    { "ERR_MOUNT_TOOFARAWAY", 2, "NONE", 68, 0 },
    { "ERR_MOUNT_ALREADYMOUNTED", 2, "NONE", 68, 0 },
    { "ERR_MOUNT_NOTMOUNTABLE", 2, "NONE", 68, 0 },
    { "ERR_MOUNT_NOTYOURPET", 2, "NONE", 68, 0 },
    { "ERR_MOUNT_OTHER", 2, "NONE", 68, 0 },
    { "ERR_MOUNT_LOOTING", 2, "NONE", 68, 0 },
    { "ERR_MOUNT_RACECANTMOUNT", 2, "NONE", 68, 0 },
    { "ERR_MOUNT_SHAPESHIFTED", 2, "NONE", 68, 0 },
    { "ERR_DISMOUNT_NOPET", 2, "NONE", 68, 0 },
    { "ERR_DISMOUNT_NOTMOUNTED", 2, "NONE", 68, 0 },
    { "ERR_DISMOUNT_NOTYOURPET", 2, "NONE", 68, 0 },
    { "ERR_SPELL_FAILED_TOTEMS", 2, "NONE", 68, 0 },
    { "ERR_SPELL_FAILED_REAGENTS", 2, "NONE", 68, 0 },
    { "ERR_SPELL_FAILED_REAGENTS_GENERIC", 2, "NONE", 68, 0 },
    { "ERR_SPELL_FAILED_EQUIPPED_ITEM", 2, "NONE", 68, 0 },
    { "ERR_SPELL_FAILED_EQUIPPED_ITEM_CLASS_S", 2, "NONE", 68, 0 },
    { "ERR_SPELL_FAILED_SHAPESHIFT_FORM_S", 2, "NONE", 68, 0 },
    { "ERR_BADATTACKFACING", 2, "NONE", 68, 0 },
    { "ERR_BADATTACKPOS", 2, "NONE", 68, 0 },
    { "ERR_CHEST_IN_USE", 2, "NONE", 52, 0 },
    { "ERR_USE_CANT_OPEN", 2, "NONE", 68, 0 },
    { "ERR_USE_LOCKED", 2, "NONE", 61, 0 },
    { "ERR_DOOR_LOCKED", 2, "NONE", 61, 0 },
    { "ERR_BUTTON_LOCKED", 2, "NONE", 61, 0 },
    { "ERR_USE_LOCKED_WITH_ITEM_S", 2, "NONE", 68, 0 },
    { "ERR_USE_LOCKED_WITH_SPELL_S", 2, "NONE", 68, 0 },
    { "ERR_USE_LOCKED_WITH_SPELL_KNOWN_SI", 2, "NONE", 68, 0 },
    { "ERR_USE_TOO_FAR", 2, "NONE", 57, 0 },
    { "ERR_USE_BAD_ANGLE", 2, "NONE", 68, 0 },
    { "ERR_USE_OBJECT_MOVING", 2, "NONE", 68, 0 },
    { "ERR_USE_SPELL_FOCUS", 1, "NONE", 68, 0 },
    { "ERR_USE_DESTROYED", 2, "NONE", 68, 0 },
    { "ERR_SET_LOOT_FREEFORALL", 0, "NONE", 68, 0 },
    { "ERR_SET_LOOT_ROUNDROBIN", 0, "NONE", 68, 0 },
    { "ERR_SET_LOOT_MASTER", 0, "NONE", 68, 0 },
    { "ERR_SET_LOOT_GROUP", 0, "NONE", 68, 0 },
    { "ERR_SET_LOOT_NBG", 0, "NONE", 68, 0 },
    { "ERR_SET_LOOT_THRESHOLD_S", 0, "NONE", 68, 0 },
    { "ERR_NEW_LOOT_MASTER_S", 0, "NONE", 68, 0 },
    { "ERR_SPECIFY_MASTER_LOOTER", 2, "NONE", 68, 0 },
    { "ERR_TAME_FAILED", 2, "NONE", 68, 0 },
    { "ERR_CHAT_WHILE_DEAD", 2, "NONE", 68, 0 },
    { "ERR_CHAT_WRONG_FACTION", 0, "NONE", 68, 0 },
    { "ERR_CHAT_PLAYER_NOT_FOUND_S", 0, "NONE", 68, 0 },
    { "ERR_NEWTAXIPATH", 1, "TaxiNodeDiscovered", 68, 0 },
    { "ERR_NO_PET", 2, "NONE", 68, 0 },
    { "ERR_NOTYOURPET", 2, "NONE", 68, 0 },
    { "ERR_PET_NOT_RENAMEABLE", 2, "NONE", 68, 0 },
    { "ERR_QUEST_OBJECTIVE_COMPLETE_S", 1, "NONE", 68, 0 },
    { "ERR_QUEST_UNKNOWN_COMPLETE", 1, "NONE", 68, 0 },
    { "ERR_QUEST_ADD_KILL_SII", 1, "NONE", 68, 0 },
    { "ERR_QUEST_ADD_FOUND_SII", 1, "NONE", 68, 0 },
    { "ERR_QUEST_ADD_ITEM_SII", 1, "NONE", 68, 0 },
    { "ERR_QUEST_ADD_PLAYER_KILL_SII", 1, "NONE", 68, 0 },
    { "ERR_CANNOTCREATEDIRECTORY", 2, "NONE", 68, 0 },
    { "ERR_CANNOTCREATEFILE", 2, "NONE", 68, 0 },
    { "ERR_PLAYER_WRONG_FACTION", 2, "NONE", 68, 0 },
    { "ERR_BANKSLOT_FAILED_TOO_MANY", 2, "NONE", 68, 0 },
    { "ERR_BANKSLOT_INSUFFICIENT_FUNDS", 2, "NONE", 22, 0 },
    { "ERR_BANKSLOT_NOTBANKER", 2, "NONE", 68, 0 },
    { "ERR_FRIEND_DB_ERROR", 0, "NONE", 68, 0 },
    { "ERR_FRIEND_LIST_FULL", 0, "NONE", 68, 0 },
    { "ERR_FRIEND_ADDED_S", 0, "NONE", 68, 0 },
    { "ERR_FRIEND_ONLINE_SS", 0, "FRIENDJOINGAME", 68, 0 },
    { "ERR_FRIEND_OFFLINE_S", 0, "NONE", 68, 0 },
    { "ERR_FRIEND_NOT_FOUND", 0, "NONE", 68, 0 },
    { "ERR_FRIEND_WRONG_FACTION", 0, "NONE", 68, 0 },
    { "ERR_FRIEND_REMOVED_S", 0, "NONE", 68, 0 },
    { "ERR_FRIEND_ERROR", 0, "NONE", 68, 0 },
    { "ERR_FRIEND_ALREADY_S", 0, "NONE", 68, 0 },
    { "ERR_FRIEND_SELF", 0, "NONE", 68, 0 },
    { "ERR_FRIEND_DELETED", 0, "NONE", 68, 0 },
    { "ERR_IGNORE_FULL", 0, "NONE", 68, 0 },
    { "ERR_IGNORE_SELF", 0, "NONE", 68, 0 },
    { "ERR_IGNORE_NOT_FOUND", 0, "NONE", 68, 0 },
    { "ERR_IGNORE_ALREADY_S", 0, "NONE", 68, 0 },
    { "ERR_IGNORE_ADDED_S", 0, "NONE", 68, 0 },
    { "ERR_IGNORE_REMOVED_S", 0, "NONE", 68, 0 },
    { "ERR_IGNORE_AMBIGUOUS", 0, "NONE", 68, 0 },
    { "ERR_IGNORE_DELETED", 0, "NONE", 68, 0 },
    { "ERR_ONLY_ONE_BOLT", 2, "NONE", 68, 0 },
    { "ERR_ONLY_ONE_AMMO", 2, "NONE", 68, 0 },
    { "ERR_SPELL_FAILED_EQUIPPED_SPECIFIC_ITEM", 2, "NONE", 68, 0 },
    { "ERR_WRONG_BAG_TYPE_SUBCLASS", 2, "NONE", 28, 0 },
    { "ERR_CANT_WRAP_STACKABLE", 2, "NONE", 68, 0 },
    { "ERR_CANT_WRAP_EQUIPPED", 2, "NONE", 68, 0 },
    { "ERR_CANT_WRAP_WRAPPED", 2, "NONE", 68, 0 },
    { "ERR_CANT_WRAP_BOUND", 2, "NONE", 68, 0 },
    { "ERR_CANT_WRAP_UNIQUE", 2, "NONE", 68, 0 },
    { "ERR_CANT_WRAP_BAGS", 2, "NONE", 68, 0 },
    { "ERR_OUT_OF_MANA", 2, "NONE", 15, 0 },
    { "ERR_OUT_OF_RAGE", 2, "NONE", 63, 0 },
    { "ERR_OUT_OF_FOCUS", 2, "NONE", 68, 0 },
    { "ERR_OUT_OF_ENERGY", 2, "NONE", 64, 0 },
    { "ERR_OUT_OF_HEALTH", 2, "NONE", 68, 0 },
    { "ERR_OUT_OF_RUNES", 2, "NONE", 68, 0 },
    { "ERR_OUT_OF_RUNIC_POWER", 2, "NONE", 68, 0 },
    { "ERR_OUT_OF_POWER_DISPLAY", 2, "NONE", 68, 0 },
    { "ERR_LOOT_GONE", 2, "NONE", 68, 0 },
    { "ERR_MOUNT_FORCEDDISMOUNT", 2, "NONE", 68, 0 },
    { "ERR_AUTOFOLLOW_TOO_FAR", 2, "NONE", 68, 0 },
    { "ERR_UNIT_NOT_FOUND", 2, "NONE", 68, 0 },
    { "ERR_INVALID_FOLLOW_TARGET", 2, "NONE", 68, 0 },
    { "ERR_INVALID_INSPECT_TARGET", 2, "NONE", 68, 0 },
    { "ERR_GUILDEMBLEM_SUCCESS", 2, "NONE", 68, 0 },
    { "ERR_GUILDEMBLEM_INVALID_TABARD_COLORS", 2, "NONE", 68, 0 },
    { "ERR_GUILDEMBLEM_NOGUILD", 2, "NONE", 68, 0 },
    { "ERR_GUILDEMBLEM_NOTGUILDMASTER", 2, "NONE", 68, 0 },
    { "ERR_GUILDEMBLEM_NOTENOUGHMONEY", 2, "NONE", 40, 0 },
    { "ERR_GUILDEMBLEM_INVALIDVENDOR", 2, "NONE", 68, 0 },
    { "ERR_EMBLEMERROR_NOTABARDGEOSET", 2, "NONE", 68, 0 },
    { "ERR_SPELL_OUT_OF_RANGE", 2, "NONE", 46, 0 },
    { "ERR_COMMAND_NEEDS_TARGET", 0, "NONE", 68, 0 },
    { "ERR_NOAMMO_S", 2, "NONE", 1, 0 },
    { "ERR_TOOBUSYTOFOLLOW", 2, "NONE", 68, 0 },
    { "ERR_DUEL_REQUESTED", 0, "LEVELUP", 68, 0 },
    { "ERR_DUEL_CANCELLED", 1, "NONE", 68, 0 },
    { "ERR_DEATHBINDALREADYBOUND", 2, "NONE", 68, 0 },
    { "ERR_DEATHBIND_SUCCESS_S", 0, "NONE", 68, 0 },
    { "ERR_NOEMOTEWHILERUNNING", 2, "NONE", 68, 0 },
    { "ERR_ZONE_EXPLORED", 1, "NONE", 68, 0 },
    { "ERR_ZONE_EXPLORED_XP", 0, "NONE", 68, 0 },
    { "ERR_INVALID_ITEM_TARGET", 2, "NONE", 51, 0 },
    { "ERR_IGNORING_YOU_S", 0, "NONE", 68, 0 },
    { "ERR_FISH_NOT_HOOKED", 1, "NONE", 68, 0 },
    { "ERR_FISH_ESCAPED", 1, "NONE", 68, 0 },
    { "ERR_SPELL_FAILED_NOTUNSHEATHED", 1, "NONE", 68, 0 },
    { "ERR_PETITION_OFFERED_S", 0, "NONE", 68, 0 },
    { "ERR_PETITION_SIGNED", 0, "igPlayerInviteAccept", 68, 0 },
    { "ERR_PETITION_SIGNED_S", 0, "igPlayerInviteAccept", 68, 0 },
    { "ERR_PETITION_DECLINED_S", 0, "igPlayerInviteDecline", 68, 0 },
    { "ERR_PETITION_ALREADY_SIGNED", 2, "igPlayerInviteDecline", 68, 0 },
    { "ERR_PETITION_RESTRICTED_ACCOUNT", 2, "igPlayerInviteDecline", 68, 0 },
    { "ERR_PETITION_ALREADY_SIGNED_OTHER", 2, "igPlayerInviteDecline", 68, 0 },
    { "ERR_PETITION_IN_GUILD", 2, "igPlayerInviteDecline", 68, 0 },
    { "ERR_PETITION_CREATOR", 2, "igPlayerInviteDecline", 68, 0 },
    { "ERR_PETITION_NOT_ENOUGH_SIGNATURES", 2, "igPlayerInviteDecline", 68, 0 },
    { "ERR_PETITION_NOT_SAME_SERVER", 2, "igPlayerInviteDecline", 68, 0 },
    { "ERR_PETITION_FULL", 2, "igPlayerInviteDecline", 68, 0 },
    { "ERR_SPELL_UNLEARNED_S", 0, "NONE", 68, 0 },
    { "ERR_PET_SPELL_ROOTED", 2, "NONE", 68, 0 },
    { "ERR_PET_SPELL_AFFECTING_COMBAT", 2, "NONE", 68, 0 },
    { "ERR_PET_SPELL_OUT_OF_RANGE", 2, "NONE", 68, 0 },
    { "ERR_PET_SPELL_NOT_BEHIND", 2, "NONE", 68, 0 },
    { "ERR_PET_SPELL_TARGETS_DEAD", 2, "NONE", 68, 0 },
    { "ERR_PET_SPELL_DEAD", 2, "NONE", 68, 0 },
    { "ERR_PET_SPELL_NOPATH", 2, "NONE", 68, 0 },
    { "ERR_ITEM_CANT_BE_DESTROYED", 2, "NONE", 68, 0 },
    { "ERR_TICKET_ALREADY_EXISTS", 2, "NONE", 68, 0 },
    { "ERR_TICKET_CREATE_ERROR", 2, "NONE", 68, 0 },
    { "ERR_TICKET_UPDATE_ERROR", 2, "NONE", 68, 0 },
    { "ERR_TICKET_DB_ERROR", 2, "NONE", 68, 0 },
    { "ERR_TICKET_NO_TEXT", 2, "NONE", 68, 0 },
    { "ERR_TICKET_TEXT_TOO_LONG", 2, "NONE", 68, 0 },
    { "ERR_OBJECT_IS_BUSY", 2, "NONE", 68, 0 },
    { "ERR_EXHAUSTION_WELLRESTED", 0, "NONE", 68, 0 },
    { "ERR_EXHAUSTION_RESTED", 0, "NONE", 68, 0 },
    { "ERR_EXHAUSTION_NORMAL", 0, "NONE", 68, 0 },
    { "ERR_EXHAUSTION_TIRED", 0, "NONE", 68, 0 },
    { "ERR_EXHAUSTION_EXHAUSTED", 0, "NONE", 68, 0 },
    { "ERR_NO_ITEMS_WHILE_SHAPESHIFTED", 2, "NONE", 68, 0 },
    { "ERR_CANT_INTERACT_SHAPESHIFTED", 2, "NONE", 68, 0 },
    { "ERR_MAIL_QUEST_ITEM", 2, "NONE", 59, 0 },
    { "ERR_MAIL_BOUND_ITEM", 2, "NONE", 59, 0 },
    { "ERR_MAIL_CONJURED_ITEM", 2, "NONE", 68, 0 },
    { "ERR_MAIL_BAG", 2, "NONE", 68, 0 },
    { "ERR_MAIL_TO_SELF", 2, "NONE", 68, 0 },
    { "ERR_MAIL_TARGET_NOT_FOUND", 2, "NONE", 68, 0 },
    { "ERR_MAIL_DATABASE_ERROR", 2, "NONE", 68, 0 },
    { "ERR_MAIL_WRAPPED_COD", 2, "NONE", 68, 0 },
    { "ERR_MAIL_SENT", 1, "NONE", 68, 0 },
    { "ERR_NOT_HAPPY_ENOUGH", 2, "NONE", 68, 0 },
    { "ERR_USE_CANT_IMMUNE", 2, "NONE", 68, 0 },
    { "ERR_CANT_BE_DISENCHANTED", 2, "NONE", 68, 0 },
    { "ERR_CANT_USE_DISARMED", 2, "NONE", 68, 0 },
    { "ERR_AUCTION_QUEST_ITEM", 2, "NONE", 59, 0 },
    { "ERR_AUCTION_BOUND_ITEM", 2, "NONE", 59, 0 },
    { "ERR_AUCTION_CONJURED_ITEM", 2, "NONE", 68, 0 },
    { "ERR_AUCTION_LIMITED_DURATION_ITEM", 2, "NONE", 68, 0 },
    { "ERR_AUCTION_WRAPPED_ITEM", 2, "NONE", 68, 0 },
    { "ERR_AUCTION_LOOT_ITEM", 2, "NONE", 68, 0 },
    { "ERR_AUCTION_BAG", 2, "NONE", 68, 0 },
    { "ERR_AUCTION_DATABASE_ERROR", 2, "NONE", 68, 0 },
    { "ERR_AUCTION_BID_OWN", 2, "NONE", 68, 0 },
    { "ERR_AUCTION_BID_INCREMENT", 2, "NONE", 68, 0 },
    { "ERR_AUCTION_HIGHER_BID", 2, "NONE", 68, 0 },
    { "ERR_AUCTION_MIN_BID", 2, "NONE", 68, 0 },
    { "ERR_AUCTION_REPAIR_ITEM", 2, "NONE", 68, 0 },
    { "ERR_AUCTION_USED_CHARGES", 2, "NONE", 68, 0 },
    { "ERR_AUCTION_STARTED", 0, "NONE", 68, 0 },
    { "ERR_AUCTION_REMOVED", 0, "NONE", 68, 0 },
    { "ERR_AUCTION_OUTBID_S", 0, "NONE", 68, 0 },
    { "ERR_AUCTION_WON_S", 0, "NONE", 68, 0 },
    { "ERR_AUCTION_SOLD_S", 0, "NONE", 68, 0 },
    { "ERR_AUCTION_EXPIRED_S", 0, "NONE", 68, 0 },
    { "ERR_AUCTION_REMOVED_S", 0, "NONE", 68, 0 },
    { "ERR_AUCTION_BID_PLACED", 0, "NONE", 68, 0 },
    { "ERR_LOGOUT_FAILED", 2, "NONE", 68, 0 },
    { "ERR_QUEST_PUSH_SUCCESS_S", 0, "NONE", 68, 0 },
    { "ERR_QUEST_PUSH_INVALID_S", 0, "NONE", 68, 0 },
    { "ERR_QUEST_PUSH_ACCEPTED_S", 0, "NONE", 68, 0 },
    { "ERR_QUEST_PUSH_DECLINED_S", 0, "NONE", 68, 0 },
    { "ERR_QUEST_PUSH_BUSY_S", 0, "NONE", 68, 0 },
    { "ERR_QUEST_PUSH_LOG_FULL_S", 0, "NONE", 68, 0 },
    { "ERR_QUEST_PUSH_ONQUEST_S", 0, "NONE", 68, 0 },
    { "ERR_QUEST_PUSH_ALREADY_DONE_S", 0, "NONE", 68, 0 },
    { "ERR_QUEST_PUSH_NOT_DAILY_S", 0, "NONE", 68, 0 },
    { "ERR_QUEST_PUSH_TIMER_EXPIRED_S", 0, "NONE", 68, 0 },
    { "ERR_QUEST_PUSH_NOT_IN_PARTY_S", 0, "NONE", 68, 0 },
    { "ERR_QUEST_PUSH_DIFFERENT_SERVER_DAILY_S", 0, "NONE", 68, 0 },
    { "ERR_RAID_GROUP_LOWLEVEL", 2, "NONE", 68, 0 },
    { "ERR_RAID_GROUP_ONLY", 2, "NONE", 68, 0 },
    { "ERR_RAID_GROUP_FULL", 2, "NONE", 68, 0 },
    { "ERR_RAID_GROUP_REQUIREMENTS_UNMATCH", 2, "NONE", 68, 0 },
    { "ERR_CORPSE_IS_NOT_IN_INSTANCE", 2, "NONE", 68, 0 },
    { "ERR_PVP_KILL_HONORABLE", 1, "NONE", 68, 0 },
    { "ERR_PVP_KILL_DISHONORABLE", 1, "NONE", 68, 0 },
    { "ERR_SPELL_FAILED_ALREADY_AT_FULL_HEALTH", 2, "NONE", 68, 0 },
    { "ERR_SPELL_FAILED_ALREADY_AT_FULL_MANA", 2, "NONE", 68, 0 },
    { "ERR_SPELL_FAILED_ALREADY_AT_FULL_POWER_S", 2, "NONE", 68, 0 },
    { "ERR_AUTOLOOT_MONEY_S", 0, "NONE", 68, 0 },
    { "ERR_GENERIC_STUNNED", 2, "NONE", 68, 0 },
    { "ERR_TARGET_STUNNED", 2, "NONE", 68, 0 },
    { "ERR_MUST_REPAIR_DURABILITY", 2, "NONE", 68, 0 },
    { "ERR_RAID_YOU_JOINED", 0, "NONE", 68, 0 },
    { "ERR_RAID_YOU_LEFT", 0, "NONE", 68, 0 },
    { "ERR_RAID_MEMBER_ADDED_S", 0, "NONE", 68, 0 },
    { "ERR_RAID_MEMBER_REMOVED_S", 0, "NONE", 68, 0 },
    { "ERR_CLICK_ON_ITEM_TO_FEED", 2, "NONE", 68, 0 },
    { "ERR_TOO_MANY_CHAT_CHANNELS", 0, "NONE", 68, 0 },
    { "ERR_LOOT_ROLL_PENDING", 2, "NONE", 68, 0 },
    { "ERR_LOOT_PLAYER_NOT_FOUND", 2, "NONE", 68, 0 },
    { "ERR_NOT_IN_RAID", 0, "NONE", 68, 0 },
    { "ERR_LOGGING_OUT", 2, "NONE", 68, 0 },
    { "ERR_TARGET_LOGGING_OUT", 2, "NONE", 68, 0 },
    { "ERR_NOT_WHILE_MOUNTED", 2, "NONE", 68, 0 },
    { "ERR_NOT_WHILE_SHAPESHIFTED", 2, "NONE", 68, 0 },
    { "ERR_NOT_IN_COMBAT", 2, "NONE", 68, 0 },
    { "ERR_NOT_WHILE_DISARMED", 2, "NONE", 68, 0 },
    { "ERR_PET_BROKEN", 2, "NONE", 68, 0 },
    { "ERR_TALENT_WIPE_ERROR", 2, "NONE", 68, 0 },
    { "ERR_FEIGN_DEATH_RESISTED", 2, "NONE", 68, 0 },
    { "ERR_MEETING_STONE_IN_QUEUE_S", 0, "PVPENTERQUEUE", 68, 0 },
    { "ERR_MEETING_STONE_LEFT_QUEUE_S", 0, "NONE", 68, 0 },
    { "ERR_MEETING_STONE_OTHER_MEMBER_LEFT", 0, "NONE", 68, 0 },
    { "ERR_MEETING_STONE_PARTY_KICKED_FROM_QUEUE", 0, "NONE", 68, 0 },
    { "ERR_MEETING_STONE_MEMBER_STILL_IN_QUEUE", 0, "NONE", 68, 0 },
    { "ERR_MEETING_STONE_SUCCESS", 0, "NONE", 68, 0 },
    { "ERR_MEETING_STONE_IN_PROGRESS", 0, "NONE", 68, 0 },
    { "ERR_MEETING_STONE_MEMBER_ADDED_S", 0, "NONE", 68, 0 },
    { "ERR_MEETING_STONE_GROUP_FULL", 2, "NONE", 68, 0 },
    { "ERR_MEETING_STONE_NOT_LEADER", 2, "NONE", 68, 0 },
    { "ERR_MEETING_STONE_INVALID_LEVEL", 2, "NONE", 68, 0 },
    { "ERR_MEETING_STONE_TARGET_NOT_IN_PARTY", 2, "NONE", 68, 0 },
    { "ERR_MEETING_STONE_TARGET_INVALID_LEVEL", 2, "NONE", 68, 0 },
    { "ERR_MEETING_STONE_MUST_BE_LEADER", 2, "NONE", 68, 0 },
    { "ERR_MEETING_STONE_NO_RAID_GROUP", 2, "NONE", 68, 0 },
    { "ERR_MEETING_STONE_NEED_PARTY", 2, "NONE", 68, 0 },
    { "ERR_MEETING_STONE_NOT_FOUND", 2, "NONE", 68, 0 },
    { "ERR_GUILDEMBLEM_SAME", 2, "NONE", 68, 0 },
    { "ERR_EQUIP_TRADE_ITEM", 2, "NONE", 68, 0 },
    { "ERR_PVP_TOGGLE_ON", 1, "NONE", 68, 0 },
    { "ERR_PVP_TOGGLE_OFF", 1, "NONE", 68, 0 },
    { "ERR_GROUP_JOIN_BATTLEGROUND_DESERTERS", 1, "NONE", 68, 0 },
    { "ERR_GROUP_JOIN_BATTLEGROUND_S", 1, "NONE", 68, 0 },
    { "ERR_GROUP_JOIN_BATTLEGROUND_FAIL", 2, "NONE", 68, 0 },
    { "ERR_GROUP_JOIN_BATTLEGROUND_TOO_MANY", 2, "NONE", 68, 0 },
    { "ERR_BATTLEGROUND_TOO_MANY_QUEUES", 2, "NONE", 68, 0 },
    { "ERR_BATTLEGROUND_CANNOT_QUEUE_FOR_RATED", 2, "NONE", 68, 0 },
    { "ERR_BATTLEDGROUND_QUEUED_FOR_RATED", 2, "NONE", 68, 0 },
    { "ERR_BATTLEGROUND_TEAM_LEFT_QUEUE", 2, "NONE", 68, 0 },
    { "ERR_BATTLEGROUND_NOT_IN_BATTLEGROUND", 2, "NONE", 68, 0 },
    { "ERR_INVALID_PROMOTION_CODE", 2, "NONE", 68, 0 },
    { "ERR_BG_PLAYER_JOINED_SS", 0, "NONE", 68, 0 },
    { "ERR_BG_PLAYER_LEFT_S", 0, "NONE", 68, 0 },
    { "ERR_RESTRICTED_ACCOUNT", 2, "NONE", 68, 0 },
    { "ERR_PLAY_TIME_EXCEEDED", 2, "NONE", 68, 0 },
    { "ERR_APPROACHING_PARTIAL_PLAY_TIME", 0, "NONE", 68, 0 },
    { "ERR_APPROACHING_PARTIAL_PLAY_TIME_2", 0, "NONE", 68, 0 },
    { "ERR_APPROACHING_NO_PLAY_TIME", 0, "NONE", 68, 0 },
    { "ERR_APPROACHING_NO_PLAY_TIME_2", 0, "NONE", 68, 0 },
    { "ERR_UNHEALTHY_TIME", 0, "NONE", 68, 0 },
    { "ERR_CHAT_RESTRICTED", 0, "NONE", 68, 0 },
    { "ERR_CHAT_THROTTLED", 0, "NONE", 68, 0 },
    { "ERR_MAIL_REACHED_CAP", 2, "NONE", 68, 0 },
    { "ERR_INVALID_RAID_TARGET", 2, "NONE", 68, 0 },
    { "ERR_RAID_LEADER_READY_CHECK_START_S", 0, "NONE", 68, 0 },
    { "ERR_READY_CHECK_IN_PROGRESS", 2, "NONE", 68, 0 },
    { "ERR_READY_CHECK_THROTTLED", 2, "NONE", 68, 0 },
    { "ERR_DUNGEON_DIFFICULTY_FAILED", 0, "NONE", 68, 0 },
    { "ERR_DUNGEON_DIFFICULTY_CHANGED_S", 0, "NONE", 68, 0 },
    { "ERR_TRADE_WRONG_REALM", 2, "NONE", 68, 0 },
    { "ERR_TRADE_NOT_ON_TAPLIST", 2, "NONE", 68, 0 },
    { "ERR_CHAT_PLAYER_AMBIGUOUS_S", 0, "NONE", 68, 0 },
    { "ERR_LOOT_CANT_LOOT_THAT_NOW", 2, "NONE", 68, 0 },
    { "ERR_LOOT_MASTER_INV_FULL", 2, "NONE", 68, 0 },
    { "ERR_LOOT_MASTER_UNIQUE_ITEM", 2, "NONE", 68, 0 },
    { "ERR_LOOT_MASTER_OTHER", 2, "NONE", 68, 0 },
    { "ERR_FILTERING_YOU_S", 0, "NONE", 68, 0 },
    { "ERR_USE_PREVENTED_BY_MECHANIC_S", 2, "NONE", 68, 0 },
    { "ERR_ITEM_UNIQUE_EQUIPPABLE", 2, "NONE", 68, 0 },
    { "ERR_LFG_LEADER_IS_LFM_S", 2, "NONE", 68, 0 },
    { "ERR_LFG_PENDING", 0, "NONE", 68, 0 },
    { "ERR_ARENA_NO_TEAM_II", 2, "NONE", 68, 0 },
    { "ERR_ARENA_TEAM_CREATE_S", 0, "LEVELUP", 68, 0 },
    { "ERR_ARENA_TEAM_INVITE_SS", 0, "NONE", 68, 0 },
    { "ERR_ARENA_TEAM_QUIT_S", 0, "NONE", 68, 0 },
    { "ERR_ARENA_TEAM_FOUNDER_S", 0, "NONE", 68, 0 },
    { "ERR_ARENA_TEAM_INTERNAL", 0, "NONE", 68, 0 },
    { "ERR_ALREADY_IN_ARENA_TEAM", 0, "NONE", 68, 0 },
    { "ERR_ALREADY_IN_ARENA_TEAM_S", 0, "NONE", 68, 0 },
    { "ERR_INVITED_TO_ARENA_TEAM", 0, "LEVELUP", 68, 0 },
    { "ERR_ALREADY_INVITED_TO_ARENA_TEAM_S", 0, "NONE", 68, 0 },
    { "ERR_ARENA_TEAM_NAME_INVALID", 0, "NONE", 68, 0 },
    { "ERR_ARENA_TEAM_NAME_EXISTS_S", 0, "NONE", 68, 0 },
    { "ERR_ARENA_TEAM_LEADER_LEAVE_S", 0, "NONE", 68, 0 },
    { "ERR_ARENA_TEAM_PERMISSIONS", 0, "NONE", 68, 0 },
    { "ERR_ARENA_TEAM_PLAYER_NOT_IN_TEAM", 0, "NONE", 68, 0 },
    { "ERR_ARENA_TEAM_PLAYER_NOT_IN_TEAM_SS", 0, "NONE", 68, 0 },
    { "ERR_ARENA_TEAM_PLAYER_NOT_FOUND_S", 0, "NONE", 68, 0 },
    { "ERR_ARENA_TEAM_NOT_ALLIED", 0, "NONE", 68, 0 },
    { "ERR_ARENA_TEAM_JOIN_SS", 0, "NONE", 68, 0 },
    { "ERR_ARENA_TEAM_YOU_JOIN_S", 0, "NONE", 68, 0 },
    { "ERR_ARENA_TEAM_LEAVE_SS", 0, "NONE", 68, 0 },
    { "ERR_ARENA_TEAM_LEADER_IS_SS", 0, "NONE", 68, 0 },
    { "ERR_ARENA_TEAM_LEADER_CHANGED_SSS", 0, "NONE", 68, 0 },
    { "ERR_ARENA_TEAM_REMOVE_SSS", 0, "NONE", 68, 0 },
    { "ERR_ARENA_TEAM_DISBANDED_S", 0, "NONE", 68, 0 },
    { "ERR_ARENA_TEAM_TARGET_TOO_LOW_S", 0, "NONE", 68, 0 },
    { "ERR_ARENA_TEAM_TARGET_TOO_HIGH_S", 0, "NONE", 68, 0 },
    { "ERR_ARENA_TEAM_TOO_MANY_MEMBERS_S", 0, "NONE", 68, 0 },
    { "ERR_ARENA_TEAM_LEVEL_TOO_LOW_I", 0, "NONE", 68, 0 },
    { "ERR_ARENA_TEAM_NOT_FOUND", 0, "NONE", 68, 0 },
    { "ERR_ARENA_TEAMS_LOCKED", 0, "NONE", 68, 0 },
    { "ERR_NO_ARENA_CHARTER", 2, "NONE", 68, 0 },
    { "ERR_CANT_SPEAK_LANGAGE", 0, "NONE", 68, 0 },
    { "ERR_VENDOR_MISSING_TURNINS", 2, "NONE", 68, 0 },
    { "ERR_BATTLEGROUND_NOT_IN_TEAM", 2, "NONE", 68, 0 },
    { "ERR_NOT_IN_BATTLEGROUND", 0, "NONE", 68, 0 },
    { "ERR_NOT_ENOUGH_HONOR_POINTS", 2, "NONE", 68, 0 },
    { "ERR_NOT_ENOUGH_ARENA_POINTS", 2, "NONE", 68, 0 },
    { "ERR_SOCKETING_REQUIRES_META_GEM", 2, "NONE", 68, 0 },
    { "ERR_SOCKETING_META_GEM_ONLY_IN_METASLOT", 2, "NONE", 68, 0 },
    { "ERR_ITEM_MAX_COUNT_SOCKETED", 2, "NONE", 68, 0 },
    { "ERR_SYSTEM_DISABLED", 2, "NONE", 68, 0 },
    { "ERR_QUEST_FAILED_TOO_MANY_DAILY_QUESTS_I", 0, "igQuestFailed", 68, 0 },
    { "ERR_ITEM_MAX_COUNT_EQUIPPED_SOCKETED", 2, "NONE", 68, 0 },
    { "ERR_ITEM_UNIQUE_EQUIPPABLE_SOCKETED", 2, "NONE", 68, 0 },
    { "ERR_USER_SQUELCHED", 0, "NONE", 68, 0 },
    { "ERR_TOO_MUCH_GOLD", 2, "NONE", 68, 0 },
    { "ERR_NOT_BARBER_SITTING", 2, "NONE", 68, 0 },
    { "ERR_QUEST_FAILED_CAIS", 0, "igQuestFailed", 68, 0 },
    { "ERR_INVITE_RESTRICTED", 0, "NONE", 68, 0 },
    { "ERR_VOICE_IGNORE_FULL", 0, "NONE", 68, 0 },
    { "ERR_VOICE_IGNORE_SELF", 0, "NONE", 68, 0 },
    { "ERR_VOICE_IGNORE_NOT_FOUND", 0, "NONE", 68, 0 },
    { "ERR_VOICE_IGNORE_ALREADY_S", 0, "NONE", 68, 0 },
    { "ERR_VOICE_IGNORE_ADDED_S", 0, "NONE", 68, 0 },
    { "ERR_VOICE_IGNORE_REMOVED_S", 0, "NONE", 68, 0 },
    { "ERR_VOICE_IGNORE_AMBIGUOUS", 0, "NONE", 68, 0 },
    { "ERR_VOICE_IGNORE_DELETED", 0, "NONE", 68, 0 },
    { "ERR_UNKNOWN_MACRO_OPTION_S", 0, "NONE", 68, 0 },
    { "ERR_NOT_DURING_ARENA_MATCH", 2, "NONE", 68, 0 },
    { "ERR_PLAYER_SILENCED", 0, "NONE", 68, 0 },
    { "ERR_PLAYER_UNSILENCED", 0, "NONE", 68, 0 },
    { "ERR_COMSAT_DISCONNECT", 0, "NONE", 68, 0 },
    { "ERR_COMSAT_RECONNECT_ATTEMPT", 0, "NONE", 68, 0 },
    { "ERR_COMSAT_CONNECT_FAIL", 0, "NONE", 68, 0 },
    { "ERR_DANCE_CREATE_DUPLICATE", 0, "NONE", 68, 0 },
    { "ERR_DANCE_SAVE_FAILED", 0, "NONE", 68, 0 },
    { "ERR_DANCE_DELETE_FAILED", 0, "NONE", 68, 0 },
    { "ERR_MAIL_INVALID_ATTACHMENT_SLOT", 2, "NONE", 68, 0 },
    { "ERR_MAIL_TOO_MANY_ATTACHMENTS", 2, "NONE", 68, 0 },
    { "ERR_MAIL_INVALID_ATTACHMENT", 2, "NONE", 68, 0 },
    { "ERR_MAIL_ATTACHMENT_EXPIRED", 2, "NONE", 68, 0 },
    { "ERR_VOICE_CHAT_PARENTAL_DISABLE_ALL", 2, "NONE", 68, 0 },
    { "ERR_VOICE_CHAT_PARENTAL_DISABLE_MIC", 2, "NONE", 68, 0 },
    { "ERR_PROFANE_CHAT_NAME", 2, "NONE", 68, 0 },
    { "ERR_PLAYER_SILENCED_ECHO", 0, "NONE", 68, 0 },
    { "ERR_PLAYER_UNSILENCED_ECHO", 0, "NONE", 68, 0 },
    { "ERR_VOICESESSION_FULL", 0, "NONE", 68, 0 },
    { "ERR_LOOT_CANT_LOOT_THAT", 2, "NONE", 68, 0 },
    { "ERR_ARENA_EXPIRED_CAIS", 2, "NONE", 68, 0 },
    { "ERR_GROUP_ACTION_THROTTLED", 0, "NONE", 68, 0 },
    { "ERR_ALREADY_PICKPOCKETED", 2, "NONE", 68, 0 },
    { "ERR_NAME_INVALID", 2, "NONE", 68, 0 },
    { "ERR_NAME_NO_NAME", 2, "NONE", 68, 0 },
    { "ERR_NAME_TOO_SHORT", 2, "NONE", 68, 0 },
    { "ERR_NAME_TOO_LONG", 2, "NONE", 68, 0 },
    { "ERR_NAME_MIXED_LANGUAGES", 2, "NONE", 68, 0 },
    { "ERR_NAME_PROFANE", 2, "NONE", 68, 0 },
    { "ERR_NAME_RESERVED", 2, "NONE", 68, 0 },
    { "ERR_NAME_THREE_CONSECUTIVE", 2, "NONE", 68, 0 },
    { "ERR_NAME_INVALID_SPACE", 2, "NONE", 68, 0 },
    { "ERR_NAME_CONSECUTIVE_SPACES", 2, "NONE", 68, 0 },
    { "ERR_NAME_RUSSIAN_CONSECUTIVE_SILENT_CHARACTERS", 2, "NONE", 68, 0 },
    { "ERR_NAME_RUSSIAN_SILENT_CHARACTER_AT_BEGINNING_OR_END", 2, "NONE", 68, 0 },
    { "ERR_NAME_DECLENSION_DOESNT_MATCH_BASE_NAME", 2, "NONE", 68, 0 },
    { "ERR_REFER_A_FRIEND_NOT_REFERRED_BY", 2, "NONE", 68, 0 },
    { "ERR_REFER_A_FRIEND_TARGET_TOO_HIGH", 2, "NONE", 68, 0 },
    { "ERR_REFER_A_FRIEND_INSUFFICIENT_GRANTABLE_LEVELS", 2, "NONE", 68, 0 },
    { "ERR_REFER_A_FRIEND_TOO_FAR", 2, "NONE", 68, 0 },
    { "ERR_REFER_A_FRIEND_DIFFERENT_FACTION", 2, "NONE", 68, 0 },
    { "ERR_REFER_A_FRIEND_NOT_NOW", 2, "NONE", 68, 0 },
    { "ERR_REFER_A_FRIEND_GRANT_LEVEL_MAX_I", 2, "NONE", 68, 0 },
    { "ERR_REFER_A_FRIEND_SUMMON_LEVEL_MAX_I", 2, "NONE", 68, 0 },
    { "ERR_REFER_A_FRIEND_SUMMON_COOLDOWN", 2, "NONE", 68, 0 },
    { "ERR_REFER_A_FRIEND_SUMMON_OFFLINE_S", 2, "NONE", 68, 0 },
    { "ERR_REFER_A_FRIEND_INSUF_EXPAN_LVL", 2, "NONE", 68, 0 },
    { "ERR_NOT_SAME_ACCOUNT", 2, "NONE", 59, 0 },
    { "ERR_BAD_ON_USE_ENCHANT", 2, "NONE", 68, 0 },
    { "ERR_TRADE_SELF", 2, "NONE", 68, 0 },
    { "ERR_TOO_MANY_SOCKETS", 2, "NONE", 68, 0 },
    { "ERR_ITEM_MAX_LIMIT_CATEGORY_COUNT_EXCEEDED_IS", 2, "NONE", 68, 0 },
    { "ERR_TRADE_TARGET_MAX_LIMIT_CATEGORY_COUNT_EXCEEDED_IS", 2, "NONE", 68, 0 },
    { "ERR_ITEM_MAX_LIMIT_CATEGORY_SOCKETED_EXCEEDED_IS", 2, "NONE", 68, 0 },
    { "ERR_ITEM_MAX_LIMIT_CATEGORY_EQUIPPED_EXCEEDED_IS", 2, "NONE", 68, 0 },
    { "ERR_SHAPESHIFT_FORM_CANNOT_EQUIP", 2, "NONE", 68, 0 },
    { "ERR_ITEM_INVENTORY_FULL_SATCHEL", 2, "NONE", 68, 0 },
    { "ERR_SCALING_STAT_ITEM_LEVEL_EXCEEDED", 2, "NONE", 68, 0 },
    { "ERR_PURCHASE_LEVEL_TOO_LOW", 2, "NONE", 68, 0 },
    { "ERR_GROUP_SWAP_FAILED", 2, "NONE", 68, 0 },
    { "ERR_INVITE_IN_COMBAT", 2, "NONE", 68, 0 },
    { "ERR_INVALID_GLYPH_SLOT", 2, "NONE", 68, 0 },
    { "ERR_GENERIC_NO_VALID_TARGETS", 2, "NONE", 45, 0 },
    { "ERR_CALENDAR_EVENT_ALERT_S", 0, "NONE", 68, 0 },
    { "ERR_PET_LEARN_SPELL_S", 0, "NONE", 68, 0 },
    { "ERR_PET_LEARN_ABILITY_S", 0, "NONE", 68, 0 },
    { "ERR_PET_SPELL_UNLEARNED_S", 0, "NONE", 68, 0 },
    { "ERR_INVITE_UNKNOWN_REALM", 2, "NONE", 68, 0 },
    { "ERR_INVITE_NO_PARTY_SERVER", 2, "NONE", 68, 0 },
    { "ERR_INVITE_PARTY_BUSY", 2, "NONE", 68, 0 },
    { "ERR_PARTY_TARGET_AMBIGUOUS", 2, "NONE", 68, 0 },
    { "ERR_PARTY_LFG_INVITE_RAID_LOCKED", 0, "NONE", 68, 0 },
    { "ERR_PARTY_LFG_BOOT_LIMIT", 2, "NONE", 68, 0 },
    { "ERR_PARTY_LFG_BOOT_COOLDOWN_S", 2, "NONE", 68, 0 },
    { "ERR_PARTY_LFG_BOOT_NOT_ELIGIBLE_S", 2, "NONE", 68, 0 },
    { "ERR_PARTY_LFG_BOOT_IN_PROGRESS", 2, "NONE", 68, 0 },
    { "ERR_PARTY_LFG_BOOT_TOO_FEW_PLAYERS", 2, "NONE", 68, 0 },
    { "ERR_PARTY_LFG_BOOT_VOTE_SUCCEEDED", 0, "NONE", 68, 0 },
    { "ERR_PARTY_LFG_BOOT_VOTE_FAILED", 0, "NONE", 68, 0 },
    { "ERR_PARTY_LFG_BOOT_IN_COMBAT", 2, "NONE", 68, 0 },
    { "ERR_PARTY_LFG_BOOT_DUNGEON_COMPLETE", 2, "NONE", 68, 0 },
    { "ERR_PARTY_LFG_BOOT_LOOT_ROLLS", 2, "NONE", 68, 0 },
    { "ERR_PARTY_LFG_TELEPORT_IN_COMBAT", 2, "NONE", 68, 0 },
    { "ERR_RAID_DISALLOWED_BY_LEVEL", 2, "NONE", 68, 0 },
    { "ERR_BATTLEGROUND_INFO_THROTTLED", 0, "NONE", 68, 0 },
    { "ERR_BATTLEGROUND_ALREADY_IN", 2, "NONE", 68, 0 },
    { "ERR_ARENA_TEAM_CHANGE_FAILED_QUEUED", 2, "NONE", 68, 0 },
    { "ERR_NOT_WHILE_FALLING", 2, "NONE", 68, 0 },
    { "ERR_NOT_WHILE_FATIGUED", 2, "NONE", 68, 0 },
    { "ERR_MAX_SOCKETS", 2, "NONE", 68, 0 },
    { "ERR_MULTI_CAST_ACTION_TOTEM_S", 2, "NONE", 68, 0 },
    { "ERR_REMOVE_FROM_PVP_QUEUE_GRANT_LEVEL", 2, "NONE", 68, 0 },
    { "ERR_REMOVE_FROM_PVP_QUEUE_XP_GAIN", 2, "NONE", 68, 0 },
    { "ERR_BATTLEGROUND_JOIN_XP_GAIN", 2, "NONE", 68, 0 },
    { "ERR_RAID_DIFFICULTY_FAILED", 0, "NONE", 68, 0 },
    { "ERR_RAID_DIFFICULTY_CHANGED_S", 0, "NONE", 68, 0 },
    { "ERR_RAID_LOCKOUT_CHANGED_S", 0, "NONE", 68, 0 },
    { "ERR_PLAYER_DIFFICULTY_CHANGED_S", 0, "NONE", 68, 0 },
    { "ERR_GMRESPONSE_DB_ERROR", 2, "NONE", 68, 0 },
    { "ERR_BATTLEGROUND_JOIN_RANGE_INDEX", 2, "NONE", 68, 0 },
    { "ERR_REMOVE_FROM_PVP_QUEUE_FACTION_CHANGE", 2, "NONE", 68, 0 },
    { "ERR_BATTLEGROUND_JOIN_FAILED", 2, "NONE", 68, 0 },
    { "ERR_BATTLEGROUND_JOIN_TIMED_OUT", 2, "NONE", 68, 0 },
    { "ERR_MAIL_LIMITED_DURATION_ITEM", 2, "NONE", 68, 0 },
    { "ERR_YELL_RESTRICTED", 0, "NONE", 68, 0 },
    { "ERR_LFG_ROLE_CHECK_FAILED", 0, "NONE", 68, 0 },
    { "ERR_LFG_ROLE_CHECK_FAILED_TIMEOUT", 0, "NONE", 68, 0 },
    { "ERR_LFG_ROLE_CHECK_FAILED_NOT_VIABLE", 0, "NONE", 68, 0 },
    { "ERR_LFG_GROUP_FULL", 0, "NONE", 68, 0 },
    { "ERR_LFG_NO_LFG_OBJECT", 0, "NONE", 68, 0 },
    { "ERR_LFG_NO_SLOTS_PLAYER", 0, "NONE", 68, 0 },
    { "ERR_LFG_NO_SLOTS_PARTY", 0, "NONE", 68, 0 },
    { "ERR_LFG_MISMATCHED_SLOTS", 0, "NONE", 68, 0 },
    { "ERR_LFG_PARTY_PLAYERS_FROM_DIFFERENT_REALMS", 0, "NONE", 68, 0 },
    { "ERR_LFG_MEMBERS_NOT_PRESENT", 0, "NONE", 68, 0 },
    { "ERR_LFG_GET_INFO_TIMEOUT", 0, "NONE", 68, 0 },
    { "ERR_LFG_INVALID_SLOT", 0, "NONE", 68, 0 },
    { "ERR_LFG_DESERTER_PLAYER", 0, "NONE", 68, 0 },
    { "ERR_LFG_DESERTER_PARTY", 0, "NONE", 68, 0 },
    { "ERR_LFG_RANDOM_COOLDOWN_PLAYER", 0, "NONE", 68, 0 },
    { "ERR_LFG_RANDOM_COOLDOWN_PARTY", 0, "NONE", 68, 0 },
    { "ERR_LFG_TOO_MANY_MEMBERS", 0, "NONE", 68, 0 },
    { "ERR_LFG_PROPOSAL_FAILED", 0, "NONE", 68, 0 },
    { "ERR_LFG_PROPOSAL_DECLINED_SELF", 0, "NONE", 68, 0 },
    { "ERR_LFG_PROPOSAL_DECLINED_PARTY", 0, "NONE", 68, 0 },
    { "ERR_LFG_NO_SLOTS_SELECTED", 2, "NONE", 68, 0 },
    { "ERR_LFG_NO_ROLES_SELECTED", 2, "NONE", 68, 0 },
    { "ERR_LFG_ROLE_CHECK_INITIATED", 0, "NONE", 68, 0 },
    { "ERR_LFG_PLAYER_DECLINED_ROLE_CHECK", 0, "NONE", 68, 0 },
    { "ERR_LFG_JOINED_QUEUE", 0, "NONE", 68, 0 },
    { "ERR_LFG_JOINED_LIST", 0, "NONE", 68, 0 },
    { "ERR_LFG_LEFT_QUEUE", 0, "NONE", 68, 0 },
    { "ERR_LFG_LEFT_LIST", 0, "NONE", 68, 0 },
    { "ERR_LFG_ROLE_CHECK_ABORTED", 0, "NONE", 68, 0 },
    { "ERR_LFG_CANT_USE_BATTLEGROUND", 2, "NONE", 68, 0 },
    { "ERR_LFG_CANT_USE_DUNGEONS", 2, "NONE", 68, 0 },
    { "ERR_INVALID_TELEPORT_LOCATION", 2, "NONE", 68, 0 },
    { "ERR_CURRENCY_FULL", 2, "NONE", 0, 0 },
    { "ERR_TOO_FAR_TO_INTERACT", 2, "NONE", 68, 0 },
    { "ERR_DIFFICULTY_CHANGE_COOLDOWN_S", 2, "NONE", 68, 0 },
    { "ERR_DIFFICULTY_CHANGE_WORLDSTATE", 2, "NONE", 68, 0 },
    { "ERR_DIFFICULTY_CHANGE_ENCOUNTER", 2, "NONE", 68, 0 },
    { "ERR_DIFFICULTY_CHANGE_COMBAT", 2, "NONE", 68, 0 },
    { "ERR_DIFFICULTY_CHANGE_PLAYER_BUSY", 2, "NONE", 68, 0 },
    { "ERR_DIFFICULTY_CHANGE_ALREADY_STARTED", 2, "NONE", 68, 0 },
    { "ERR_ARENA_TEAM_PARTY_SIZE", 2, "NONE", 68, 0 },
    { "ERR_QUEST_FORCE_REMOVED_S", 0, "NONE", 68, 0 },
    { "ERR_IN_RANDOM_BG", 2, "NONE", 68, 0 },
    { "ERR_IN_NON_RANDOM_BG", 2, "NONE", 68, 0 },
    { "ERR_AUCTION_ENOUGH_ITEMS", 2, "NONE", 68, 0 },
    { "ERR_BN_FRIEND_SELF", 2, "NONE", 68, 0 },
    { "ERR_BN_FRIEND_ALREADY", 2, "NONE", 68, 0 },
    { "ERR_BN_FRIEND_BLOCKED", 2, "NONE", 68, 0 },
    { "ERR_BN_FRIEND_REQUEST_SENT", 1, "NONE", 68, 0 },
    { "ERR_BN_BROADCAST_THROTTLE", 1, "NONE", 68, 0 },
};

// STATIC: 0x00D0B2A8
int32_t CGGameUI::m_cursorEquipmentSet = -1;

// STATIC: #0x00AC80A4
int32_t CGGameUI::m_phaseMask;

// STATIC: 0x00D0B2B4
int32_t CGGameUI::m_visible = 1;

// STATIC: 0x00D0B2B0
int32_t CGGameUI::m_areaMountFlags = -1;

// STATIC: 0x00ED7524
int32_t CGGameUI::m_billingActive;

// STATIC: 0x00ED74F2
uint8_t CGGameUI::m_hasTravelPass;

// STATIC: 0x00ED74F3
uint8_t CGGameUI::s_canSendSoR;

// STATIC: 0x00ED7564
int32_t CGGameUI::s_numSoRRemaining;

// STATIC: 0x00ED7560
int32_t CGGameUI::s_maxSoRPerDay;

// STATIC: 0x00ED7568
uint32_t CGGameUI::s_cfgRealmID;

// STATIC: 0x00ED756C
uint32_t CGGameUI::s_cfgRealmRecID;

// STATIC: 0x00ED753C
int32_t CGGameUI::s_tournamentRealm;

// STATIC: 0x00ED7558
uint32_t CGGameUI::s_restrictedLevel;

// STATIC: 0x00ED755C
uint32_t CGGameUI::s_restrictedMoney;

// STATIC: 0x00D0BD10
CCapturePointUIManager g_capturePointUIManager;

// STATIC: 0x00D0BCAC
static float s_maxMatchDistance = 10000.0f;

// STATIC: 0x00D0BCB0
static float s_corpseRangeSq = 1600.0f;

// STATIC: 0x00D0BCD0
static C44Matrix s_corpseMapRotation;

// STATIC: 0x00D0BD00
static C3Vector s_corpseMapPosition;

// STATIC: 0x00ED74D8
static int32_t s_corpseQueryTime;

// STATIC: 0x00ED6808
static uint32_t s_accountDataPending;

// STATIC: 0x00ED680C
CinematicInfo CGGameUI::m_cinematic;

// STATIC: 0x00ED6824
int32_t CGGameUI::m_hasControl;

// STATIC: 0x00ED6828
char CGGameUI::m_lastErrorMessage[3000];

// STATIC: 0x00ED73E0
int32_t CGGameUI::m_cursorItemType;

// STATIC: 0x00ED73E4
int32_t CGGameUI::m_cursorHasAction;

// STATIC: 0x00ED73E8
int32_t CGGameUI::m_cursorVirtualSlot;

// STATIC: 0x00ED73EC
int32_t CGGameUI::m_cursorVirtualDisplay;

// STATIC: 0x00ED73F0
int32_t CGGameUI::m_cursorVirtualID;

// STATIC: 0x00ED73F4
uint32_t CGGameUI::m_cursorItemSlot;

// STATIC: 0x00ED73F8
WOWGUID CGGameUI::m_cursorItemContainer;

// STATIC: 0x00ED7400
WOWGUID CGGameUI::m_cursorItem;

// STATIC: 0x00ED740C
CGTooltip* CGGameUI::m_gameTooltip;

// STATIC: 0x00ED77A4
static TAINTEVENT s_lastTaintEvent;

// STATIC: 0x00ED7934
static TSHashTable<TAINTEVENT, HASHKEY_STR> s_taintEvents;

// STATIC: 0x00ED7410
CSimpleFrame* CGGameUI::m_UISimpleParent;

// STATIC: 0x00ED7420
CSimpleTop* CGGameUI::m_simpleTop;

// STATIC: 0x00ED7424
uint8_t CGGameUI::m_currentlyReloadingUI;

// STATIC: 0x00ED7425
uint8_t CGGameUI::m_reloadUIRequested;

// STATIC: 0x00ED7427
uint8_t CGGameUI::m_loggingIn;

// STATIC: 0x00ED7478
uint64_t CGGameUI::m_cursorMoney;

// STATIC: 0x00ED7438
WOWGUID CGGameUI::m_currentObjectTrack;

// STATIC: 0x00ED7440
WOWGUID CGGameUI::m_interactTarget;

// STATIC: 0x00ED7430
float CGGameUI::m_interactMaxDistSq;

// STATIC: 0x00ED74E0
WOWGUID CGGameUI::m_areaSpiritHealerGUID;

// STATIC: #0x00AC8A64
float s_duelDistSq = 100.0f;

// STATIC: 0x00D0BCA0
float s_playerInteractDistSq = 123.45678f;

// STATIC: 0x00D0BCA4
float s_inspectDistSq = 900.0f;

// STATIC: #0x00AC8A70
float s_followDistSq = 900.0f;

// STATIC: 0x00ED7448
WOWGUID CGGameUI::m_lockedTarget;

// STATIC: 0x00ED7450
WOWGUID CGGameUI::m_lastTarget;

// STATIC: 0x00ED7458
WOWGUID CGGameUI::m_lastEnemyTarget;

// STATIC: 0x00ED7460
WOWGUID CGGameUI::m_lastFriendTarget;

// STATIC: 0x00ED7468
WOWGUID CGGameUI::m_focusTarget;

// STATIC: 0x00ED7434
int32_t CGGameUI::m_cursorSpell;

// STATIC: 0x00ED7480
PetAction CGGameUI::m_cursorPetAction;

// STATIC: 0x00ED7484
int32_t CGGameUI::m_cursorMacro;

// STATIC: 0x00ED7488
int32_t CGGameUI::m_cursorPet;

// STATIC: 0x00ED7494
int32_t CGGameUI::m_screenWidth;

// STATIC: 0x00ED74A0
int32_t CGGameUI::m_screenHeight;

// STATIC: 0x00ED74A4
float CGGameUI::m_aspect;

// STATIC: 0x00ED74A8
int32_t CGGameUI::m_inCinematic;

// STATIC: 0x00ED74AC
int32_t CGGameUI::m_inMovie;

// STATIC: #0x00BD0804
int32_t CGGameUI::m_movieFileDataID;

// STATIC: #0x00BD0808
int32_t CGGameUI::m_movieVolume;

// STATIC: 0x00ED74C8
WOWGUID CGGameUI::m_corpseGUID;

// STATIC: 0x00ED74EC
uint32_t CGGameUI::m_repopTime;

// STATIC: 0x00ED74F0
uint8_t CGGameUI::m_deadNoRepopTimer;

// STATIC: 0x00ED74F1
uint8_t CGGameUI::m_comboPoints;

// STATIC: 0x00ED74F4
uint32_t CGGameUI::m_corpseReclaimDelay;

// STATIC: 0x00ED74F8
uint32_t CGGameUI::m_instanceBootTime;

// STATIC: 0x00ED74FC
uint32_t CGGameUI::m_instanceLockTime;

// STATIC: 0x00ED7500
uint32_t CGGameUI::m_instanceLockCompletedMask;

// STATIC: 0x00ED7504
uint32_t CGGameUI::m_instanceLockExtending;

// STATIC: 0x00ED7508
uint32_t CGGameUI::m_summonConfirmTime;

// STATIC: 0x00ED7518
uint32_t CGGameUI::m_billingExpireTime;

// STATIC: 0x00ED751C
int32_t CGGameUI::m_billingNagCheckTime;

// STATIC: 0x00ED7520
int32_t CGGameUI::m_billingNagShown;

// STATIC: 0x00ED7528
int32_t CGGameUI::m_initialized;

// STATIC: 0x00ED752C
uint32_t CGGameUI::m_uiLastInstanceTime;

// STATIC: 0x00ED7530
int32_t CGGameUI::m_iMapID;

// STATIC: 0x00ED7534
int32_t CGGameUI::m_iCurrentMapID;

// STATIC: 0x00ED7540
uint32_t CGGameUI::m_instanceDifficulty;

// STATIC: 0x00ED7544
uint32_t CGGameUI::m_dungeonDifficulty;

// STATIC: 0x00ED754C
uint32_t CGGameUI::m_lastRegenTime;

// STATIC: 0x00ED7550
WOWGUID CGGameUI::m_comboTarget;

// STATIC: 0x00ED75D0
int32_t CGGameUI::m_threatWarningEnabled;

// STATIC: 0x00ED75D8
uint16_t CGGameUI::m_accountDataGeneration;

// STATIC: #0x00BD08CC
int32_t CGGameUI::unk_BD08CC;

// STATIC: 0x00ED7830
TotemInfo CGGameUI::m_totems[5];

// STATIC: 0x00ED78D0
MirrorTimerInfo CGGameUI::m_mirrorTimerInfo[3];

// STATIC: 0x00ED74B0
int32_t CGGameUI::m_areaID;

// STATIC: 0x00ED7538
int32_t CGGameUI::m_bOwnsInstance;

// STATIC: 0x00ED7408
int32_t CGGameUI::m_stackSplit;

// STATIC: 0x00ED7426
uint8_t CGGameUI::s_inWorld;

// STATIC: 0x00ED7578
uint32_t CGGameUI::m_autoStoreItemID;

// STATIC: 0x00ED7570
WOWGUID CGGameUI::m_autoStoreItem;

// STATIC: 0x00ED74BC
int32_t CGGameUI::m_corpseMapID;

// STATIC: 0x00ED74C0
int32_t CGGameUI::m_corpseActualMapID;

// STATIC: 0x00ED74C4
int32_t CGGameUI::m_corpseInRange;

// STATIC: 0x00ED74D0
WOWGUID CGGameUI::m_corpseTransportGUID;

// STATIC: 0x00ED74E8
int32_t CGGameUI::m_deathReleaseMapID;

// STATIC: 0x00ED778C
C3Vector CGGameUI::m_corpsePos;

// STATIC: 0x00ED7798
C3Vector CGGameUI::m_deathReleaseLoc;

// STATIC: 0x00ED7414
const char* CGGameUI::m_minimapZoneText;

// STATIC: 0x00ED7418
const char* CGGameUI::m_subZoneText;

// STATIC: 0x00ED74B4
int32_t CGGameUI::m_subzoneID;

// STATIC: 0x00ED74B8
int32_t CGGameUI::m_wmoGroupID;

// STATIC: 0x00ED7470
WOWGUID CGGameUI::m_directionalTarget;

// STATIC: 0x00ED7580
WOWGUID CGGameUI::m_pendingLevelGranter;

// STATIC: 0x00ED7548
uint32_t CGGameUI::m_raidDifficulty;

// STATIC: #0x00BD0780
const char* CGGameUI::m_realZoneText;

// STATIC: 0x00ED741C
const char* CGGameUI::m_zoneText;

// STATIC: 0x00ED750C
uint32_t CGGameUI::m_summonConfirmArea;

// STATIC: 0x00ED7510
WOWGUID CGGameUI::m_summonConfirmGUID;

// STATIC: 0x00ED74DC
int32_t CGGameUI::m_areaSpiritHealerTime;

// STATIC: 0x00ED77D8
TSHashTable<SAVEDVARIABLE, HASHKEY_STR> CGGameUI::s_savedVariables[2];

struct ItemPushData {
    /* 0000 */ WOWGUID m_guid;
    /* 0008 */ uint32_t m_bag;
    /* 000C */ uint32_t m_slot;
    /* 0010 */ uint32_t m_received;
    /* 0014 */ uint32_t m_created;
    /* 0018 */ uint32_t m_showInChat;
    /* 001C */ uint32_t m_suffixFactor;
    /* 0020 */ uint32_t m_randomPropertyId;
    /* 0024 */ uint32_t m_count;
};

// STATIC: 0x00ED75F8
WOWGUID CGGameUI::m_lastItemSpellTarget;

// STATIC: 0x00ED7600
int32_t CGGameUI::m_lastEnchantSpellID;

// STATIC: 0x00ED75E8
static uint32_t s_nearestIndex;

// STATIC: 0x00ED75EC
static uint32_t s_nearestSortTime;

// STATIC: 0x00ED75F0
static uint32_t s_nearestTargetTime;

// STATIC: 0x00ED75F4
static int32_t s_nearestType;

struct NearestUnitData {
    /* 0000 */ WOWGUID m_guid;
    /* 0008 */ float m_distanceSq;
    /* 000C */ int32_t m_inFront;
};

// STATIC: 0x00ED7924
static TSGrowableArray<NearestUnitData> s_nearestUnits;

// STATIC: #0x00AC8050
static const char* s_victimStateTokens[9] = { "MISS", "WOUND", "DODGE", "PARRY", "INTERRUPT", "BLOCK", "EVADE", "IMMUNE", "DEFLECT" };

// STATIC: 0x00ED74AC
int32_t CGGameUI::s_loadingScreen;

// OFFSET: 0x008C40D0/0x140633BD0
void CGGameUI::OpenGuildInvite(const char* inviter, const char* guild, int32_t guildLevel, const char* oldGuild) {
    FrameScript_SignalEvent(SCRIPT_EVENT_GUILD_INVITE_REQUEST, "%s%s%d%s", inviter, guild, guildLevel, oldGuild);
}

// OFFSET: 0x008C4100
void CGGameUI::CancelGuildInvite() {
    FrameScript_SignalEvent(SCRIPT_EVENT_GUILD_INVITE_CANCEL, NULL);
}

// OFFSET: 0x008C4210/0x140633D50
void CGGameUI::SetAspect() {
    CGGameUI::m_aspect = g_theGxDevicePtr->CapsAspectRatio(0);
    CoordinateSetAspectRatio(CGGameUI::m_aspect);
}

// OFFSET: 0x008C4620/0x1406341B0
WOWGUID CGGameUI::GetPartyMember(uint32_t index) {
    return CGPartyInfo::GetMember(index);
}

// OFFSET: 0x008C4630/0x1406341C0
WOWGUID CGGameUI::GetPartyMemberPet(uint32_t index) {
    return CGPartyInfo::GetMemberPet(index);
}

// OFFSET: 0x008C4640/0x1406341D0
WOWGUID GetRaidMemberGUID(uint32_t index) {
    if (index >= CGRaidInfo::m_numMembers || !CGRaidInfo::m_members[index]) {
        return WOWGUID();
    }

    return CGRaidInfo::m_members[index]->m_guid;
}

// OFFSET: 0x008C4790
void CGGameUI::SetActiveCorpse(WOWGUID guid) {
    CGGameUI::m_corpseGUID = guid;
}

// OFFSET: 0x008CA170/0x14063A4C0
WOWGUID CGGameUI::GetRaidMemberPet(uint32_t index) {
    if (index >= CGRaidInfo::m_numMembers) {
        return WOWGUID();
    }

    CGRaidMember* member = CGRaidInfo::m_members[index];

    if (!member) {
        return WOWGUID();
    }

    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(member->m_guid, TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!unit) {
        return member->m_stats.m_pet.m_guid;
    }

    const WOWGUID& pet = unit->m_unit->m_unitFieldCharm ? unit->m_unit->m_unitFieldCharm : unit->m_unit->m_unitFieldSummon;
    return pet;
}

// OFFSET: 0x008CAB80/0x14063B170
void CGGameUI::ShowBlockedActionFeedback(FrameScript_Object* object, uint32_t type) {
    // STATIC: 0x00ED7964
    static int32_t showing;

    if (showing) {
        return;
    }

    showing = 1;

    lua_State* L = FrameScript_GetContext();
    char function[1024];
    FrameScript_GetCurrentFunction(L, function, sizeof(function));
    SStrPack(function, "()", sizeof(function));

    if (type) {
        if (lua_tainted == g_macroTaint) {
            FrameScript_SignalEvent(SCRIPT_EVENT_MACRO_ACTION_BLOCKED, "%s", function);
        } else {
            FrameScript_SignalEvent(SCRIPT_EVENT_ADDON_ACTION_BLOCKED, "%s%s", lua_tainted, function);
        }
    } else if (lua_tainted == g_macroTaint) {
        FrameScript_SignalEvent(SCRIPT_EVENT_MACRO_ACTION_FORBIDDEN, "%s", function);
    } else {
        FrameScript_SignalEvent(SCRIPT_EVENT_ADDON_ACTION_FORBIDDEN, "%s%s", lua_tainted, function);
    }

    if (lua_tainteventcallback) {
        if (type == 2) {
            lua_tainteventcallback(L, 4, NULL, lua_tainted);
        } else {
            lua_tainteventcallback(L, 3, NULL, lua_tainted);
        }
    }

    showing = 0;
}

// OFFSET: 0x008C57E0/0x1406354A0
uint32_t CGGameUI::GetCurrentDungeonDifficulty() {
    if (!CGPartyInfo::m_members[0] || CGRaidInfo::m_numMembers) {
        return CGGameUI::m_dungeonDifficulty;
    }

    return CGPartyInfo::m_dungeonDifficulty;
}

// OFFSET: 0x008CB140/0x14063B960
int32_t CGGameUI::CanResetInstance() {
    if (!CGGameUI::m_bOwnsInstance) {
        return 0;
    }

    MapRec* map = g_mapDB.GetRecord(CGGameUI::m_iCurrentMapID);

    if (map && (map->m_instanceType == 1 || map->m_instanceType == 2)) {
        uint32_t difficulty = CGGameUI::GetCurrentDungeonDifficulty();

        if (difficulty > 3 || !GetMapRaidDuration(map->m_id, difficulty)) {
            return 0;
        }
    }

    if (!g_mapDB.GetRecord(CGGameUI::m_iMapID) || (map->m_instanceType != 1 && map->m_instanceType != 2)) {
        return 0;
    }

    uint32_t difficulty = CGGameUI::GetCurrentDungeonDifficulty();

    if ((difficulty > 3 || !GetMapRaidDuration(map->m_id, difficulty)) && _time32(NULL) - CGGameUI::m_uiLastInstanceTime <= 90000) {
        return 1;
    }

    return 0;
}

// OFFSET: 0x008CB220/0x14063BAD0
void CGGameUI::SetLastInstanceTime(uint32_t time, int32_t mapId, int32_t currentMapId) {
    if (mapId == currentMapId) {
        return;
    }

    MapRec* map = g_mapDB.GetRecord(mapId);

    if (map && (map->m_instanceType == 1 || map->m_instanceType == 2)) {
        uint32_t difficulty = CGGameUI::GetCurrentDungeonDifficulty();

        if (difficulty > 3 || !GetMapRaidDuration(map->m_id, difficulty)) {
            CGGameUI::m_uiLastInstanceTime = time;
            CGGameUI::m_iMapID = mapId;
        }
    }

    CGGameUI::m_iCurrentMapID = currentMapId;
}

struct TargetDirectionData {
    /* 0000 */ CGWorldFrame* m_worldFrame;
    /* 0004 */ CGUnit_C* m_unit;
    /* 0008 */ CGUnit_C* m_origin;
    /* 000C */ C2Vector m_originPosition;
    /* 0014 */ C2Vector m_facing;
    /* 001C */ C3Vector m_originScreen;
    /* 0028 */ C2Vector m_screenDirection;
    /* 0030 */ int32_t m_type;
    /* 0038 */ WOWGUID m_bestGuid;
    /* 0040 */ float m_cosHalfAngle;
    /* 0044 */ int32_t m_bestInCone;
    /* 0048 */ float m_bestDistance;
};

// OFFSET: 0x008CA890/0x14063AC70
static int32_t TargetDirectionProc(WOWGUID guid, void* param) {
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (!unit) {
        return 1;
    }

    TargetDirectionData* data = static_cast<TargetDirectionData*>(param);

    if (!data) {
        SErrDisplayAppFatalMessage("", "", 0);
    }

    if (unit == data->m_unit || unit == data->m_origin || !UnitMatchesTargetType(data->m_unit, unit, data->m_type)) {
        return 1;
    }

    C3Vector unitPosition = data->m_unit->GetPosition();
    C3Vector delta = unit->GetPosition() - unitPosition;

    if (delta.y * delta.y + delta.z * delta.z + delta.x * delta.x > 1681.0f) {
        return 1;
    }

    CreatureTypeRec* creatureType = g_creatureTypeDB.GetRecord(unit->GetCreatureType());

    if (creatureType && (creatureType->m_flags & 1)) {
        return 1;
    }

    if (!World::QueryObjectVisible(unit->m_worldObject) || ((unit->m_obj->m_type & TYPEMASK_PLAYER) && static_cast<CGPlayer_C*>(unit)->IsCommentatorUberOrInArena())) {
        return 1;
    }

    C3Vector position = unit->GetPosition();
    C3Vector screen(0.0f, 0.0f, 0.0f);

    if (!data->m_worldFrame->GetScreenCoordinates(position, &screen, NULL)) {
        return 1;
    }

    int32_t inCone = 1;
    C2Vector direction = C2Vector(position);
    direction.x = direction.x - data->m_originPosition.x;
    direction.y = direction.y - data->m_originPosition.y;
    direction.NormalizeSafe();
    float dot = data->m_facing.y * direction.y + data->m_facing.x * direction.x;
    C2Vector screenDirection = C2Vector(screen - data->m_originScreen);
    screenDirection.NormalizeSafe();

    if (data->m_screenDirection.y * screenDirection.y + data->m_screenDirection.x * screenDirection.x < 0.0f) {
        return 1;
    }

    if (data->m_cosHalfAngle <= dot) {
        if (!data->m_bestInCone) {
            data->m_bestDistance = FLT_MAX;
        }
    } else {
        if (data->m_bestInCone) {
            return 1;
        }

        inCone = 0;
    }

    C3Vector screenDelta = screen - data->m_originScreen;
    float distance = screenDelta.z * screenDelta.z + screenDelta.y * screenDelta.y + screenDelta.x * screenDelta.x;

    if (!inCone) {
        distance = distance * ((1.0f - dot) * (1.0f - dot));
    }

    if (distance > data->m_bestDistance) {
        return 1;
    }

    data->m_bestDistance = distance;
    data->m_bestGuid = guid;
    data->m_bestInCone = inCone;
    return 1;
}

// OFFSET: 0x008D6A50/0x140648C50
void CGGameUI::TargetDirection(float facing, float angle, int32_t type) {
    if (lua_tainted) {
        CGGameUI::ShowBlockedActionFeedback(NULL, 0);
        return;
    }

    if (CGGameUI::m_directionalTarget) {
        return;
    }

    CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();

    if (!player) {
        return;
    }

    CGUnit_C* unit = player->GetPossessedUnit();

    if (!unit) {
        unit = player;
    }

    CGWorldFrame* worldFrame = CGWorldFrame::s_currentWorldFrame;

    if (!worldFrame) {
        return;
    }

    CGUnit_C* origin = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(CGGameUI::m_lockedTarget, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (!origin) {
        origin = unit;
    }

    C3Vector position = origin->GetPosition();
    C3Vector cameraFacing(0.0f, 0.0f, 0.0f);
    C3Vector screen(0.0f, 0.0f, 0.0f);

    if (!worldFrame->GetScreenCoordinates(position, &screen, NULL)) {
        origin = unit;
        position = unit->GetPosition();
        worldFrame->GetScreenCoordinates(position, &screen, NULL);
    }

    CGWorldFrame::GetCameraFacing(&cameraFacing);
    cameraFacing *= C33Matrix::Rotation(6.2831855f - facing);

    TargetDirectionData data;
    data.m_worldFrame = worldFrame;
    data.m_unit = unit;
    data.m_origin = origin;
    data.m_originPosition = C2Vector(position);
    data.m_facing = C2Vector(cameraFacing);
    data.m_originScreen = screen;
    data.m_screenDirection.x = sinf(facing);
    data.m_screenDirection.y = cosf(facing);
    data.m_type = type;
    data.m_bestGuid = WOWGUID();

    float coneAngle = 0.39269909f;

    if (angle > coneAngle) {
        coneAngle = angle;
    }

    data.m_cosHalfAngle = cosf(coneAngle * 0.5f);
    data.m_bestInCone = 0;
    data.m_bestDistance = FLT_MAX;

    ClntObjMgrEnumVisibleObjects(TargetDirectionProc, &data);

    if (data.m_bestGuid) {
        CGGameUI::Target(data.m_bestGuid);
        CGGameUI::m_directionalTarget = data.m_bestGuid;
    }
}

// OFFSET: 0x008D20E0/0x140646180
int32_t CGGameUI::CanPerformAction(uint32_t action) {
    if (lua_tainted) {
        switch (action) {
            case 0:
            case 1:
            case 2:
            case 3:
            case 4:
            case 5:
            case 16:
            case 17:
                CGGameUI::ShowBlockedActionFeedback(NULL, 0);
                return 0;

            case 6:
            case 7:
            case 8:
            case 9:
            case 15:
            case 18:
            case 19:
            case 20:
            case 21:
            case 23:
            case 25:
            case 26:
                if (!CGGameUI::m_simpleTop) {
                    return 1;
                }

                if (!CGGameUI::m_simpleTop->unk_1260) {
                    CGGameUI::ShowBlockedActionFeedback(NULL, 1);
                    return 0;
                }

                break;

            case 11:
            case 12:
            case 13:
            case 14:
            case 22:
            case 24:
                if (!CGGameUI::m_simpleTop) {
                    return 1;
                }

                if (!CGGameUI::m_simpleTop->m_allowProtectedActions) {
                    CGGameUI::ShowBlockedActionFeedback(NULL, 2);
                    return 0;
                }

                break;

            default:
                break;
        }
    }

    if (CGGameUI::m_simpleTop && (!action || action == 8 || action == 25)) {
        CGGameUI::m_simpleTop->unk_1260 = 0;
    }

    return 1;
}

// OFFSET: 0x008C3FC0/0x140633A90
int32_t CGGameUI::StartCinematicSound() {
    CinematicCameraRec* record = CGGameUI::m_cinematic.m_camera;

    if (!record) {
        return 0;
    }

    SI3::StopAllMusic();
    SE3SoundKitProperties properties;
    properties.m_bus = SE3BUS_9;
    properties.m_bit0 = 1;
    properties.m_reverb = 0;
    properties.m_bit10 = 1;
    SE3::PlaySoundKit(record->m_soundID, NULL, &properties, &CGGameUI::m_cinematic.m_cameraMusic, __FILE__, __LINE__, "CGGameUI::StartCinematicSound", 1, NULL);
    return 1;
}

// OFFSET: 0x008C4450
uint32_t CGGameUI::GetCemetery(uint32_t index) {
    if (index >= CGGameUI::s_numCemeteries) {
        return 0;
    }

    return CGGameUI::s_cemeteries[index];
}

// OFFSET: 0x008C4870/0x140634460
void CGGameUI::SetCemeteryPreference(uint32_t cemeteryID) {
    bool found = false;

    for (uint32_t i = 0; i < CGGameUI::s_numCemeteries; i++) {
        if (cemeteryID == CGGameUI::s_cemeteries[i]) {
            found = true;
        }
    }

    if (!found && cemeteryID) {
        CGGameUI::s_cemeteryPreference = 0;
        return;
    }

    if (cemeteryID != CGGameUI::s_cemeteryPreference) {
        CGGameUI::s_cemeteryPreference = cemeteryID;
        UserClientSetPreferredCemetery msg;
        msg.m_cemeteryID = cemeteryID;
        ClientServices::Send(&msg);
    }
}

// OFFSET: 0x008C5380/0x140635010
void CGGameUI::UpdateBillingTime() {
    if (!CGGameUI::m_billingActive) {
        return;
    }

    uint32_t now = FrameTime::GetCurTimeMs();
    uint32_t remaining = CGGameUI::m_billingExpireTime - now;
    uint32_t minutes = static_cast<uint32_t>(static_cast<int64_t>(ceil(static_cast<double>(remaining) * (1.0 / 60000.0))));
    int32_t igr = (ClientServices::Connection()->m_billingFlags >> 3) & 1;

    if (!CGGameUI::m_billingNagCheckTime) {
        if (igr) {
            if (minutes <= 600) {
                FrameScript_SignalEvent(SCRIPT_EVENT_IGR_BILLING_NAG_DIALOG, NULL);
                CGGameUI::m_billingNagShown = 1;
            }
        } else if (minutes <= 30) {
            FrameScript_SignalEvent(SCRIPT_EVENT_BILLING_NAG_DIALOG, "%d", minutes);
            CGGameUI::m_billingNagShown = 1;
        }

        CGGameUI::m_billingNagCheckTime = now;
    } else if (static_cast<int32_t>(now - CGGameUI::m_billingNagCheckTime) >= 0 && !igr) {
        if (minutes <= 30) {
            char text[3000];
            SStrPrintf(text, sizeof(text), FrameScript_GetText("BILLING_NAG_WARNING", -1, GENDER_NOT_APPLICABLE), minutes);
            CGChat::AddChatMessage(text, static_cast<SLASH_COMMAND_ID>(0), NULL);
        }

        if (static_cast<int32_t>(now - CGGameUI::m_billingExpireTime) < 0) {
            if (minutes <= 5) {
                CGGameUI::m_billingNagCheckTime = now + remaining - 60000 * (remaining / 60000);
            } else {
                CGGameUI::m_billingNagCheckTime = now + remaining - 300000 * (remaining / 300000);
            }
        } else {
            CGGameUI::m_billingActive = 0;
        }

        if (minutes <= 30 && !CGGameUI::m_billingNagShown) {
            FrameScript_SignalEvent(SCRIPT_EVENT_BILLING_NAG_DIALOG, "%d", minutes);
            CGGameUI::m_billingNagShown = 1;
        }

        return;
    }

    if (igr) {
        if (minutes <= 600 && !CGGameUI::m_billingNagShown) {
            FrameScript_SignalEvent(SCRIPT_EVENT_IGR_BILLING_NAG_DIALOG, NULL);
            CGGameUI::m_billingNagShown = 1;
        }
    } else if (minutes <= 30 && !CGGameUI::m_billingNagShown) {
        FrameScript_SignalEvent(SCRIPT_EVENT_BILLING_NAG_DIALOG, "%d", minutes);
        CGGameUI::m_billingNagShown = 1;
    }
}

// OFFSET: 0x008C5560/0x140635210
void CGGameUI::UpdateParentalControls() {
    if (!ClientServices::Connection()) {
        return;
    }

    uint32_t remaining = ClientServices::Connection()->GetTimeRemaining();

    if (remaining > 900) {
        return;
    }

    int32_t minutes;

    if (CGGameUI::s_parentalControlsWarning < 0) {
        if (static_cast<int32_t>(remaining) < 60) {
            CGChat::AddChatMessage(FrameScript_GetText("ERR_PARENTAL_CONTROLS_DISCONNECT_WARNING_SOON", -1, GENDER_NOT_APPLICABLE), static_cast<SLASH_COMMAND_ID>(0), NULL);
            CGGameUI::s_parentalControlsWarning = 0;
            return;
        }

        minutes = remaining / 60;
    } else {
        uint32_t index = 0;

        if (static_cast<int32_t>(remaining) > 60) {
            while (remaining > 60u * s_parentalControlsWarningTimes[++index]) {
            }
        }

        minutes = s_parentalControlsWarningTimes[index];

        if (CGGameUI::s_parentalControlsWarning <= minutes) {
            return;
        }
    }

    CGGameUI::s_parentalControlsWarning = minutes;
    char text[3000];
    SStrPrintf(text, sizeof(text), FrameScript_GetText("ERR_PARENTAL_CONTROLS_DISCONNECT_WARNING", -1, GENDER_NOT_APPLICABLE), minutes);
    CGChat::AddChatMessage(text, static_cast<SLASH_COMMAND_ID>(0), NULL);
}

// OFFSET: 0x008C5660/0x140635330
void CGGameUI::UpdatePlayerAFK(uint32_t now, CGPlayer_C* player) {
    int32_t idleTime = now - CSimpleTop::s_eventTime;

    if (idleTime - 300000 < 0) {
        return;
    }

    if (idleTime - 1800000 < 0) {
        if (!player) {
            return;
        }

        if (!player->m_unit->m_unitFieldBytes1[0] && !(player->m_unit->m_unitFieldFlags & 0x80000) && !player->IsMounted() && !player->IsAutoTracking()) {
            player->ChangeStandState(1);
        }

        if (!(player->m_unit->m_unitFieldFlags & 0x100000) && !CGChat::m_afk) {
            player->SetAFK(NULL);
        }
    } else if (!ClientServices::Connection()->CharacterLoggingOut()) {
        CGChat::AddChatMessage(FrameScript_GetText("IDLE_MESSAGE", -1, GENDER_NOT_APPLICABLE), static_cast<SLASH_COMMAND_ID>(0), NULL);
        CGLCD::IdleDisconnectWarning();
        ClientServices::Connection()->CharacterLogout(false, false);
    }
}

// OFFSET: 0x008C56D0
void CGGameUI::ClearStreamingWait() {
    CGGameUI::s_streamingWait = 0;
}

// OFFSET: 0x008C56E0
int32_t CGGameUI::GetStreamingWait() {
    return CGGameUI::s_streamingWait;
}

// OFFSET: 0x008C56F0
void CGGameUI::UpdateStreamingIcon() {
    int32_t previous = CGGameUI::s_streamingIcon;

    switch (AsyncFileReadGetStreamingQueue()) {
        case 0:
            CGGameUI::s_streamingIcon = 3;
            break;

        case 1:
            CGGameUI::s_streamingIcon = 2;
            break;

        case 2:
            CGGameUI::s_streamingIcon = 1;
            break;

        case 3:
            CGGameUI::s_streamingIcon = 0;
            break;

        default:
            break;
    }

    if (previous != CGGameUI::s_streamingIcon) {
        FrameScript_SignalEvent(SCRIPT_EVENT_STREAMING_ICON, "%d", CGGameUI::s_streamingIcon);
    }
}

// OFFSET: 0x008C57C0
void CGGameUI::SetInstanceDifficulty(uint32_t difficulty) {
    CGGameUI::m_instanceDifficulty = difficulty;
}

// OFFSET: 0x008C5FA0
int32_t CGGameUI::GetWeeklyTimeRemaining() {
    int32_t remaining = CGGameUI::s_weeklyResetTime - _time32(NULL) + 604800;
    return remaining >= 0 ? remaining : 0;
}

// OFFSET: 0x008C5FE0/0x140636120
bool Script_GetMoney(lua_State* L, int32_t index, uint64_t* money) {
    double value = lua_tonumber(L, index);

    if (value < 0.0 || value > static_cast<double>(0x7FFFFFFFFFFFFFFFLL)) {
        return false;
    }

    *money = static_cast<uint64_t>(value);
    return true;
}

// OFFSET: 0x008C6050
void CGGameUI::SetSessionTimeAlert(bool enabled, int32_t interval, int32_t intervalAlt, int32_t alertTime) {
    CGGameUI::s_sessionAlertEnabled = enabled;
    CGGameUI::s_sessionAlertInterval = 1000 * interval;
    CGGameUI::s_sessionAlertIntervalAlt = 1000 * intervalAlt;
    CGGameUI::s_sessionAlertTime = 1000 * alertTime;
}

// OFFSET: 0x008C6090
void CGGameUI::UpdateSessionTimeAlert() {
    if (!CGGameUI::s_sessionAlertEnabled) {
        return;
    }

    int32_t interval = SessionTime_IsAlternate() ? CGGameUI::s_sessionAlertIntervalAlt : CGGameUI::s_sessionAlertInterval;

    if (SessionTime_GetElapsed() > interval) {
        SessionTime_ResetElapsed();
        SessionTime_ResetAlternate();
        FrameScript_SignalEvent(SCRIPT_EVENT_SESSION_TIME_ALERT, "%d", CGGameUI::s_sessionAlertTime);
    }
}

// OFFSET: 0x008C60E0/0x1407AEEA0
uint32_t MouseButtonNumberToMask(int32_t button) {
    switch (button) {
        case 1:
            return 0x1;

        case 2:
            return 0x4;

        case 3:
            return 0x2;

        default:
            break;
    }

    if (button >= 4 && button <= 31) {
        return 1u << (button - 1);
    }

    return 0;
}

// OFFSET: 0x008CAB60
float CGGameUI::GetUIScale() {
    if (CGGameUI::m_UISimpleParent) {
        return CGGameUI::m_UISimpleParent->m_layoutScale;
    }

    return 1.0f;
}

// OFFSET: 0x008CBA40
int32_t CGGameUI::GetCurrentAreaMountFlags() {
    AreaTableRec* area = g_areaTableDB.GetRecord(CGGameUI::m_subzoneID);

    if (area) {
        return area->m_mountFlags;
    }

    area = g_areaTableDB.GetRecord(CGGameUI::m_areaID);
    return area ? area->m_mountFlags : -1;
}

// OFFSET: 0x008CC050/0x14063CE80
bool CGGameUI::IsInArenaOrRatedBattleground(bool* arena) {
    if (arena) {
        *arena = false;
    }

    MapRec* map = g_mapDB.GetRecord(CGGameUI::m_iCurrentMapID);

    if (!map) {
        return false;
    }

    if (map->m_instanceType == 4) {
        if (arena) {
            *arena = true;
        }

        return true;
    }

    return map->m_instanceType == 3 && CGBattlefieldInfo::IsRatedBattleground();
}

// OFFSET: 0x008D2F40
void CGGameUI::UpdateMountFlags() {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));
    CGGameUI::m_areaMountFlags = player ? player->GetMountFlags() : CGGameUI::GetCurrentAreaMountFlags();
}

// OFFSET: 0x008D30C0
void CGGameUI::CheckThreatTutorial(const WOWGUID& guid) {
    if (CGGameUI::m_threatWarningEnabled && guid == ClntObjMgrGetActivePlayer()) {
        CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

        if (player->m_unit->m_unitFieldLevel >= 14) {
            CGTutorial::TriggerTutorial(53);
        }
    }
}

// OFFSET: 0x008D5850
void CGGameUI::SetCursorMoney(uint64_t amount) {
    CGGameUI::SetCursorMoney(amount, 2, SCRIPT_EVENT_PLAYER_MONEY);
}

// OFFSET: 0x008D8B30/0x14064C280
void CCapturePointUIManager::Clear() {
    this->m_nodes.Clear();
}

// OFFSET: 0x008DBEA0/0x140650280
void CGGameUI::SetCursorVoidStorageItem(WOWGUID guid) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return;
    }

    CGVoidStorage_C* storage = player->GetVoidStorage();

    if (!storage) {
        return;
    }

    VoidItem_C* item = storage->GetItem(guid);

    if (!item) {
        return;
    }

    CGGameUI::ClearCursor(1, 1);
    CGGameUI::m_cursorVoidStorageItem = guid;
    CGGameUI::m_cursorItemType = 15;
    WOWGUID none = WOWGUID();
    ItemRec* rec = g_itemDB.GetRecord(item->m_entryID, none, NULL, NULL, false);
    CursorGrabVirtualItem(rec ? rec->m_displayInfoID : 0);

    if (rec) {
        SI3::PlayItemSound(0, rec->m_displayInfoID);
        CGGameUI::m_cursorVirtualDisplay = rec->m_displayInfoID;
    }

    CGGameUI::m_cursorHasAction = 0;
}

// OFFSET: 0x008DDB50/0x1406526A0
void CGGameUI::Initialize() {
    g_taintLogCVar = CVar::Register("taintLog", "Whether taint logging is enabled", 0, "0", SetTaintLogCallback, 5, false, NULL, false);
    g_scriptProfileCVar = CVar::Register("scriptProfile", "Whether or not script profiling is enabled", 0, "0", NULL, 5, false, NULL, false);

    CGGameUI::m_loggingIn = 1;
    CGGameUI::m_reloadUIRequested = 0;
    CGGameUI::m_repopTime = 0;
    CGGameUI::m_deadNoRepopTimer = 0;
    CGGameUI::m_corpseReclaimDelay = 0;
    CGGameUI::m_instanceBootTime = 0;
    CGGameUI::m_instanceLockTime = 0;
    CGGameUI::m_instanceLockCompletedMask = 0;
    CGGameUI::m_instanceLockExtending = 0;
    CGGameUI::m_summonConfirmTime = 0;

    AreaTableRec* area = g_areaTableDB.GetRecord(CGGameUI::m_subzoneID);

    if (!area) {
        area = g_areaTableDB.GetRecord(CGGameUI::m_areaID);
    }

    CGGameUI::m_areaMountFlags = area ? area->m_mountFlags : -1;

    CRect windowRect = { 0.0f, 0.0f, 0.0f, 0.0f };
    g_theGxDevicePtr->CapsWindowSizeInScreenCoords(windowRect);
    CGGameUI::m_screenWidth = 0;
    CGGameUI::m_screenHeight = 0;
    CGGameUI::m_aspect = g_theGxDevicePtr->CapsAspectRatio(0);
    CoordinateSetAspectRatio(CGGameUI::m_aspect);

    CGGameUI::m_simpleTop = new (__FILE__, __LINE__) CSimpleTop();
    CGGameUI::m_simpleTop->m_mouseButtonCallback = CGGameUI::FilterMouseDown;
    CGGameUI::m_simpleTop->m_mouseMoveCallback = CGGameUI::FilterMouseMotion;
    CGGameUI::m_simpleTop->m_displaySizeCallback = CGGameUI::HandleDisplaySizeChanged;
    CGGameUI::m_simpleTop->m_focusChangedCallback = CGGameUI::HandleFocusChanged;
    CGGameUI::m_simpleTop->m_allowProtectedActions = 1;
    CGGameUI::m_simpleTop->m_actionBlockedCallback = CGGameUI::ShowBlockedFrameFeedback;

    CursorInitialize();

    CGInputControl* input = CGInputControl::GetActive();
    input->m_mouseModeFlags = 3;
    input->m_forceCursorOn = 0;
    input->UpdateMouseMode(1);

    FrameScript_Destroy();
    FrameScript_Initialize(g_scriptProfileCVar->m_intValue);
    LoadScriptFunctions();
    FrameScript_CreateEvents(g_scriptEvents, NUM_SCRIPT_EVENTS);
    CGGameUI::RegisterGameCVars();
    CGUIBindings::Initialize();
    CGWorldFrame::RegisterFrames();
    OsCreateDirectory("Logs", 0);

    CWOWClientStatus status("Logs\\FrameXML.log");

    LoadAddOnEnableState(ClientServices::GetCharacterName());

    MD5_CTX md5;
    MD5Init(&md5);

    uint8_t signature[16];

    switch (FrameXML_CheckSignature("Interface\\FrameXML\\FrameXML.toc", "Interface\\FrameXML\\Bindings.xml", InterfaceKey, signature)) {
        case 0:
            status.Add(STATUS_WARNING, "FrameXML missing signature");
            ClientPostClose(10);
            break;

        case 1:
            status.Add(STATUS_WARNING, "FrameXML has corrupt signature");
            ClientPostClose(10);
            break;

        case 2:
            status.Add(STATUS_WARNING, "FrameXML is modified or corrupt");
            ClientPostClose(10);
            break;

        case 3:
            break;

        default:
            ClientPostClose(10);
            break;
    }

    uint32_t numFiles = 0;
    void* toc;

    if (SFile::ReadFile(NULL, "Interface\\FrameXML\\FrameXML.toc", &toc, NULL, 1, 1)) {
        numFiles = FrameXML_GuessNumFiles(static_cast<const char*>(toc));
        SFile::Unload(toc);
    }

    FrameXML_RegisterLoadProgressCallback(LoadingScreenXMLCallback, NULL, LoadAddOnFileCount() + numFiles);
    FrameXML_FreeHashNodes();
    FrameXML_CreateFrames("Interface\\FrameXML\\FrameXML.toc", NULL, &md5, &status);

    if (SFile::FileExistsEx("Interface\\FrameXML\\Bindings.xml", 1)) {
        CGUIBindings::s_bindings->Load("Interface\\FrameXML\\Bindings.xml", &md5, &status);
    }

    uint8_t digest[16];
    MD5Final(digest, &md5);

#ifndef DISABLE_SIGNATURE_CHECKS
    if (memcmp(signature, digest, sizeof(digest))) {
        ClientPostClose(10);
    }
#endif

    CGGameUI::m_UISimpleParent = static_cast<CSimpleFrame*>(CScriptObject::GetScriptObjectByName("UIParent", CSimpleFrame::GetObjectType()));
    CGGameUI::m_gameTooltip = reinterpret_cast<CGTooltip*>(CScriptObject::GetScriptObjectByName("GameTooltip", CGTooltip::GetObjectType()));

    LoadAddOns(&status);
    FrameXML_RegisterLoadProgressCallback(NULL, NULL, 0);
    g_uiScaleCVar = CVar::Register("uiScale", "The current UI scale", 0, "1.0", CVarHandler_uiScale, 4, false, NULL, false);
    g_useUiScaleCVar = CVar::Register("useUiScale", "Whether or not the UI scale should be used", 0, "0", CVarHandler_useUiScale, 4, false, NULL, false);
    LoadSavedVariables(&status);
    LoadAccountData(6, LoadPlacedFrames, NULL);
    LoadAccountVariables();
    CGUIBindings::LoadBindings();
    CGUIMacros::Initialize();
    CGUIMacros::LoadMacros();
    CGChat::LoadChatSettings();
    CSimpleScriptManager::Create();

    CSizeEvent sizeEvent;
    sizeEvent.m_data.m_w = static_cast<int32_t>(windowRect.maxX - windowRect.minX);
    sizeEvent.m_data.m_h = static_cast<int32_t>(windowRect.maxY - windowRect.minY);
    CGGameUI::HandleDisplaySizeChanged(sizeEvent);

    CGGameUI::m_initialized = 1;
    EventRegister(EVENT_ID_IDLE, CGGameUI::Idle);
    g_theGxDevicePtr->AddDeviceRestoredCallback(UpdateAllPortraits);
    g_theGxDevicePtr->AddTextureRecreationCallback(UpdateAllPortraits);
    CSimpleTop::SetColorblindShader();

    if (ClntObjMgrGetActivePlayer()) {
        CGGameUI::EnterWorld();
    }

    CGGameUI::m_currentlyReloadingUI = 0;
}

// OFFSET: 0x008C3F60
void CGGameUI::SetCanSendSoRByText(bool enabled) {
    CGGameUI::s_canSendSoR = enabled;
    FrameScript_SignalEvent(SCRIPT_EVENT_SOR_BY_TEXT_UPDATED, NULL);
}

// OFFSET: 0x008C3F80
void CGGameUI::SetSoRCounts(int32_t maxPerDay, int32_t remaining) {
    CGGameUI::s_maxSoRPerDay = maxPerDay;
    CGGameUI::s_numSoRRemaining = remaining;
    FrameScript_SignalEvent(SCRIPT_EVENT_SOR_COUNTS_UPDATED, NULL);
}

// OFFSET: 0x008CB2A0
bool CGGameUI::OnWorldServerInfo(ClientWorldServerInfo* msg) {
    CGGameUI::m_instanceDifficulty = msg->m_difficultyID;
    CGGameUI::s_tournamentRealm = msg->m_isTournamentRealm;
    CGGameUI::s_weeklyResetTime = msg->m_weeklyReset;
    CGGameUI::s_restrictedLevel = 0;
    CGGameUI::s_restrictedMoney = 0;

    if (msg->m_hasRestrictedLevel) {
        CGGameUI::s_restrictedLevel = msg->m_restrictedLevel;
    }

    if (msg->m_hasRestrictedMoney) {
        CGGameUI::s_restrictedMoney = msg->m_restrictedMoney;
    }

    if (msg->m_hasIneligibleForLootMask) {
        FrameScript_SignalEvent(SCRIPT_EVENT_INELIGIBLE_FOR_LOOT, "%u", msg->m_ineligibleForLootMask);
    }

    return true;
}

// OFFSET: 0x008DE3E0/0x140653120
void CGGameUI::InitializeGame() {
    ClientServices::SetMessageHandler(SMSG_PVP_CREDIT, CGGameUI::MessageHandler, NULL);
    ClientServices::SetMessageHandler(MSG_CORPSE_QUERY, CGGameUI::MessageHandler, NULL);
    ClientServices::SetMessageHandler(SMSG_CORPSE_RECLAIM_DELAY, CGGameUI::MessageHandler, NULL);
    ClientServices::SetMessageHandler(CMSG_GM_REQUEST_PLAYER_INFO, CGGameUI::MessageHandler, NULL);
    ClientServices::SetMessageHandler(SMSG_GM_PLAYER_INFO, CGGameUI::MessageHandler, NULL);
    ClientServices::SetMessageHandler(SMSG_PLAY_MUSIC, CGGameUI::MessageHandler, NULL);
    ClientServices::SetMessageHandler(SMSG_PLAY_SOUND, CGGameUI::MessageHandler, NULL);
    ClientServices::SetMessageHandler(SMSG_PLAY_OBJECT_SOUND, CGGameUI::MessageHandler, NULL);
    ClientServices::SetMessageHandler(SMSG_AREA_TRIGGER_MESSAGE, CGGameUI::MessageHandler, NULL);
    ClientServices::SetMessageHandler(SMSG_AREA_TRIGGER_NO_CORPSE, CGGameUI::MessageHandler, NULL);
    ClientServices::SetMessageHandler(SMSG_INIT_WORLD_STATES, CGGameUI::MessageHandler, NULL);
    ClientServices::SetMessageHandler(SMSG_UPDATE_WORLD_STATE, CGGameUI::MessageHandler, NULL);
    ClientServices::SetMessageHandler(SMSG_AREA_SPIRIT_HEALER_TIME, CGGameUI::MessageHandler, NULL);
    ClientServices::SetMessageHandler(SMSG_INVALID_PROMOTION_CODE, CGGameUI::MessageHandler, NULL);
    ClientServices::SetMessageHandler(SMSG_WEATHER, CGGameUI::MessageHandler, NULL);
    ClientServices::SetMessageHandler(SMSG_OVERRIDE_LIGHT, CGGameUI::MessageHandler, NULL);
    ClientServices::SetMessageHandler(MSG_SET_DUNGEON_DIFFICULTY, CGGameUI::MessageHandler, NULL);
    ClientServices::SetMessageHandler(MSG_SET_RAID_DIFFICULTY, CGGameUI::MessageHandler, NULL);
    ClientServices::SetMessageHandler(SMSG_UPDATE_COMBO_POINTS, CGGameUI::MessageHandler, NULL);
    ClientServices::SetMessageHandler(SMSG_TOTEM_CREATED, CGGameUI::MessageHandler, NULL);
    ClientServices::SetMessageHandler(SMSG_START_MIRROR_TIMER, CGGameUI::OnMirrorTimerEvent, NULL);
    ClientServices::SetMessageHandler(SMSG_PAUSE_MIRROR_TIMER, CGGameUI::OnMirrorTimerEvent, NULL);
    ClientServices::SetMessageHandler(SMSG_STOP_MIRROR_TIMER, CGGameUI::OnMirrorTimerEvent, NULL);
    ClientServices::SetMessageHandler(SMSG_PROPOSE_LEVEL_GRANT, CGGameUI::MessageHandler, NULL);
    ClientServices::SetMessageHandler(SMSG_REFER_A_FRIEND_FAILURE, CGGameUI::MessageHandler, NULL);
    ClientServices::SetMessageHandler(SMSG_REFER_A_FRIEND_EXPIRED, CGGameUI::MessageHandler, NULL);
    ClientServices::SetMessageHandler(SMSG_BREAK_TARGET, CGGameUI::MessageHandler, NULL);
    ClientServices::SetMessageHandler(SMSG_CORPSE_MAP_POSITION_QUERY_RESPONSE, CGGameUI::MessageHandler, NULL);
    ClientServices::SetMessageHandler(SMSG_UI_TIME, CGGameUI::MessageHandler, NULL);
    ClientServices::SetMessageHandler(SMSG_TALENTS_INVOLUNTARILY_RESET, CGGameUI::MessageHandler, NULL);
    ClientDifferentInstanceFromParty::s_handler = CGGameUI::OnDifferentInstanceFromParty;
    ClientMoneyNotify::s_handler = CGGameUI::OnMoneyNotify;
    ClientWorldServerInfo::s_handler = CGGameUI::OnWorldServerInfo;
    ClientDisplayGameError::s_handler = CGGameUI::OnDisplayGameError;
    ClientFailedPlayerCondition::s_handler = CGGameUI::OnFailedPlayerCondition;

    do {
        CGGameUI::m_accountDataGeneration++;
    } while (!CGGameUI::m_accountDataGeneration);

    for (uint32_t i = 0; i < sizeof(s_readOnlyCVars) / sizeof(s_readOnlyCVars[0]); i++) {
        CVar* var = CVar::Lookup(s_readOnlyCVars[i]);

        if (var) {
            var->SetReadOnly(true);
        }
    }

    DBCache_ClearTemporaryCaches();
    PortraitInitialize();
    CGMinimapFrame::InitializeGame();
    CGChat::InitializeGame();
    CGPartyInfo::InitializeGame();
    CGGlyphInfo::InitializeGame();
    CGSpellBook::InitializeGame();
    CGActionBar::InitializeGame();
    CFlyout::InitializeGame();
    CGLootInfo::InitializeGame();
    CGItemText::InitializeGame();
    CGGossipInfo::InitializeGame();
    CGQuestLog::InitializeGame();
    CGTaxiMap::InitializeGame();
    CGPetInfo::InitializeGame();
    CGWorldMap::InitializeGame();
    CGReputationInfo::InitializeGame();
    CGDuelInfo::InitializeGame();
    CGGuildInfo::InitializeGame();
    CGLFGuildInfo::InitializeGame();
    CGMailInfo::InitializeGame();
    CGBattlefieldInfo::InitializeGame();
    CGAuctionHouse::InitializeGame();
    CGStableInfo::InitializeGame();
    CGRaidInfo::InitializeGame();
    CGCharacterInfo::InitializeGame();
    CGWorldStateInfo::InitializeGame();
    CGMinigameInfo::InitializeGame();
    CGArenaTeamInfo::InitializeGame();
    CGLookingForGroup::InitializeGame();
    CGPetitionInfo::InitializeGame();
    CGCommentator::InitializeGame();
    CGCalendar::InitializeGame();
    CGDanceStudio::InitializeGame();
    CGBarberShop::InitializeGame();
    CGGuildBankInfo::InitializeGame();
    CGAchievementInfo::InitializeGame();
    CGRuneInfo::InitializeGame();
    CGCurrencyTypes::InitializeGame();
    CGEquipmentManager::InitializeGame();
    CGGMTicketInfo::InitializeGame();
    CGTradeInfo::InitializeGame();
    CGResearchFrame::InitializeGame();
    PhaseShift_C_InitializeGame();
    MovieStreaming::InitializeGame();
    CGEncounterJournal::InitializeGame();
    CGCUFProfiles::InitializeGame();
    CGIncomingResurrection::Reset();
    CGTransmogrify::InitializeGame();

    CGGameUI::m_cinematic.m_music = 0;
    CGGameUI::m_cinematic.m_cameraMusic = 0;
    CGGameUI::m_hasControl = 1;
    CGGameUI::m_inCinematic = 0;
    CGGameUI::m_billingActive = 0;

    uint32_t billingMinutes = ClientServices::Connection()->m_billingTimeRemaining;

    if (billingMinutes <= 4320 && !(ClientServices::Connection()->m_billingFlags & 0x2) && billingMinutes) {
        CGGameUI::m_billingExpireTime = 60000 * billingMinutes + FrameTime::GetCurTimeMs();
        CGGameUI::m_billingNagCheckTime = 0;
        CGGameUI::m_billingNagShown = 0;
        CGGameUI::m_billingActive = 1;
    }

    CGGameUI::s_parentalControlsWarning = -1;
    CGGameUI::m_instanceDifficulty = 0;
    CGGameUI::m_dungeonDifficulty = 0;
    CGGameUI::m_lastRegenTime = 0;
    CGGameUI::m_comboTarget = WOWGUID();
    CGGameUI::m_comboPoints = 0;
    CGGameUI::m_hasTravelPass = 0;

    for (int32_t i = 0; i < 5; i++) {
        TotemInfo& totem = CGGameUI::m_totems[i];
        totem.m_slot = i;
        totem.m_guid = WOWGUID();
        totem.m_name = NULL;
        totem.m_duration = 0;
        totem.m_startTime = 0;
        totem.m_icon = NULL;
    }

    for (int32_t i = 0; i < 3; i++) {
        MirrorTimerInfo& timer = CGGameUI::m_mirrorTimerInfo[i];
        timer.m_type = 3;
        timer.unk_0018 = 0;
        timer.m_spellId = 0;
        timer.m_paused = 0;
        timer.m_scale = 0;
        timer.m_maxValue = 0;
        timer.m_value = 0;
    }

    if ((CGlueMgr::m_showingIntroScene && !CharSelectIsStartRaceComplete()) || (PreloadList::StopDownload(CCharacterSelection::m_startRacePreloadList), CCharacterSelection::m_startRacePreloadList = NULL, s_startWorldCommonList && !s_startWorldCommonList->IsComplete())) {
        WoWReport::SetStage("dwnw");
        PreloadList::WaitAll();
        PreloadList::StopDownload(s_startWorldCommonList);
        s_startWorldCommonList = NULL;
    }

    CGGameUI::Initialize();

    if (CGInputControl::GetActive()->WowMouseEnabled() && !CGInputControl::GetActive()->WowMouseConnected()) {
        FrameScript_SignalEvent(SCRIPT_EVENT_WOW_MOUSE_NOT_FOUND, NULL);
    }
}

// OFFSET: 0x008DE140/0x140652E30
void CGGameUI::DoUiReload() {
    CGGameUI::m_currentlyReloadingUI = 1;
    CoordinateSetAspectRatio(g_theGxDevicePtr->CapsAspectRatio(0));
    LoadingScreenEnable(CGGameUI::m_iCurrentMapID, false);
    ClientServices::Connection()->CharacterAbortLogout();
    CGGameUI::Shutdown();
    CGGameUI::Initialize();
    CVehiclePassenger_C::OnReloadUI();
    LoadingScreenDisable();
}

// OFFSET: 0x008DE190/0x140652E80
int32_t CGGameUI::Idle(const void* data, void* param) {
    if (g_theGxDevicePtr->StereoEnabled()) {
        float convergence = g_theGxDevicePtr->StereoGetConvergence();

        if (CGGameUI::m_UISimpleParent) {
            CGGameUI::m_UISimpleParent->SetFrameDepth(convergence - CGGameUI::m_UISimpleParent->m_parentDepth, false);
        }
    }

    lua_State* L = FrameScript_GetContext();
    lua_taintexpected = 0;
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player && !(player->m_unit->m_unitFieldFlags & 0x80000)) {
        lua_gc(L, LUA_GCSTEP, 1);
    }

    if (CGGameUI::m_reloadUIRequested && !CGGameUI::m_inCinematic) {
        CGGameUI::DoUiReload();
        return 1;
    }

    CGGameUI::UpdateBillingTime();
    CGGameUI::UpdateParentalControls();
    uint32_t now = FrameTime::GetCurTimeMs();

    if (CGGameUI::s_cemeteryListRequestTime && static_cast<int32_t>(now - CGGameUI::s_cemeteryListRequestTime) >= 0) {
        CGGameUI::s_cemeteryListRequestTime = 0;
        UserClientRequestCemeteryList msg;
        ClientServices::Send(&msg);
    }

    CGGameUI::DoPowerRegen();
    CGGameUI::AutoMouseOver();
    CGChat::ProcessThrottledChat();
    CGQuestLog::UpdateWatchTimers();
    CGUIMacros::UpdateDynamicMacros();
    CKBPage::UpdateLoadingQueue();
    g_capturePointUIManager.Update();
    DBCache_Idle();
    CGGameUI::UpdateStreamingIcon();
    Script_SendUniqueSignals();
    DBClient_Idle();
    CGGameUI::UpdatePlayerAFK(now, player);
    CGGameUI::UpdateSessionTimeAlert();
    return 1;
}

// OFFSET: 0x008C4840/0x140634430
void CGGameUI::GetDeathReleaseLocation(int32_t& mapID, C3Vector& position) {
    mapID = CGGameUI::m_deathReleaseMapID;
    position = CGGameUI::m_deathReleaseLoc;
}

// OFFSET: 0x008C4900
int32_t CGGameUI::FilterMouseMotion(CMouseEvent* evt) {
    if (evt->m_id != 0x400500CB) {
        return 0;
    }

    CGInputControl::GetActive()->OnMouseMoveRel(*evt);
    return 1;
}

// OFFSET: 0x008D2B60/0x14063B7F0
void CGGameUI::UpdateActivePlayer() {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        SErrDisplayAppFatalMessage("", "", 0);
    }

    if (player->m_unit->m_unitFieldHealth > 0) {
        FrameScript_SignalEvent(SCRIPT_EVENT_PLAYER_ALIVE, NULL);
    } else {
        FrameScript_SignalEvent(SCRIPT_EVENT_PLAYER_DEAD, NULL);
        CGGameUI::ClearCursor(1, 1);
    }
}

// OFFSET: 0x008C4950
int32_t CGGameUI::IsKeyDown(KEY key) {
    if (key <= KEY_RALT && CGUIBindings::s_bindings->m_keyStateOverride) {
        return ((1 << key) & CGUIBindings::s_bindings->m_heldModifiers) != 0;
    }

    return EventIsKeyDown(key);
}

// OFFSET: 0x008C5BB0/0x140635900
PartyMemberState* CGGameUI::GetPartyMemberState(const WOWGUID& guid) {
    if (guid == WOWGUID()) {
        return NULL;
    }

    PartyMemberState* state = CGPartyInfo::GetRemoteStats(guid);

    if (!state) {
        state = CGRaidInfo::GetRemoteStats(guid);

        if (!state) {
            return CGBattlefieldInfo::GetRemoteStats(guid);
        }
    }

    return state;
}

// OFFSET: 0x008C5C30/0x140635980
PartyMemberPetState* CGGameUI::GetPartyMemberPetState(const WOWGUID& guid) {
    if (guid == WOWGUID()) {
        return NULL;
    }

    PartyMemberPetState* state = CGPartyInfo::GetRemotePetStats(guid);

    if (!state) {
        state = CGRaidInfo::GetRemotePetStats(guid);

        if (!state) {
            return CGBattlefieldInfo::GetRemotePetStats(guid);
        }
    }

    return state;
}

// OFFSET: 0x008D5000
void CGGameUI::ScaleToScreenSize(int32_t width, int32_t height, int32_t force) {
    if (!force && CGGameUI::m_screenWidth > 0 && CGGameUI::m_screenWidth == width && CGGameUI::m_screenHeight > 0 && CGGameUI::m_screenHeight == height) {
        return;
    }

    CGGameUI::m_screenWidth = width;
    CGGameUI::m_screenHeight = height;

    float aspect = g_theGxDevicePtr->CapsAspectRatio(0);
    float scale = height <= 768 ? 1.0f : 768.0f / height;

    if (aspect * 3.0f < 4.0f && aspect * 0.75f <= scale) {
        scale = aspect * 0.75f;
    }

    if (scale <= 0.63999999f) {
        scale = 0.63999999f;
    }

    if (CGGameUI::m_UISimpleParent) {
        CGGameUI::m_UISimpleParent->SetFrameScale(scale, true);
        CLayoutFrame::ResizePending();
        FrameScript_SignalEvent(SCRIPT_EVENT_UPDATE_FLOATING_CHAT_WINDOWS, NULL);
        FrameScript_SignalEvent(SCRIPT_EVENT_UI_SCALE_CHANGED, NULL);
    }

    if (fabs(CGGameUI::m_aspect - aspect) >= 0.001f) {
        if (lua_tainted && CGGameUI::m_simpleTop && !CGGameUI::m_simpleTop->unk_1260) {
            CGGameUI::ShowBlockedActionFeedback(NULL, 1);
        } else {
            CGGameUI::m_reloadUIRequested = 1;
        }

        CGGameUI::m_aspect = aspect;
    }
}

// OFFSET: 0x008D6160
int32_t CGGameUI::HandleDisplaySizeChanged(const CSizeEvent& evt) {
    if (g_useUiScaleCVar->m_intValue) {
        float scale = g_uiScaleCVar->m_floatValue;

        if (3.0f * CGGameUI::m_aspect < 4.0f && CGGameUI::m_aspect * 0.75f <= scale) {
            scale = CGGameUI::m_aspect * 0.75f;
        }

        if (scale >= 0.63999999f) {
            CGGameUI::SetUIScale(scale);
            float aspect = g_theGxDevicePtr->CapsAspectRatio(0);

            if (fabs(CGGameUI::m_aspect - aspect) >= 0.001f) {
                CGGameUI::Reload();
                CGGameUI::m_aspect = aspect;
            }

            return 1;
        }
    }

    CGGameUI::ScaleToScreenSize(evt.m_data.m_w, evt.m_data.m_h, 0);
    FrameScript_SignalEvent(SCRIPT_EVENT_DISPLAY_SIZE_CHANGED, NULL);
    return 1;
}

// OFFSET: 0x008C4990
void CGGameUI::OnMouseModeRelative() {
    CGWorldFrame* worldFrame = CGWorldFrame::s_currentWorldFrame;

    if (!worldFrame) {
        SErrDisplayAppFatalMessage("", "", 0);
    }

    worldFrame->OnMouseModeRelative();

    if (!CSimpleTop::s_instance) {
        SErrDisplayAppFatalMessage("", "", 0);
    }

    CSimpleTop::s_instance->SetMouseFocus(worldFrame);
    g_theGxDevicePtr->CursorSetVisible(0);
    EventSetMouseMode(MOUSE_MODE_RELATIVE, 0);
}

// OFFSET: 0x008C49D0
void CGGameUI::OnMouseModeNormal() {
    if (!CGWorldFrame::s_currentWorldFrame) {
        SErrDisplayAppFatalMessage("", "", 0);
    }

    CGWorldFrame::s_currentWorldFrame->OnMouseModeNormal();

    if (!CSimpleTop::s_instance) {
        SErrDisplayAppFatalMessage("", "", 0);
    }

    CSimpleTop::s_instance->m_checkFocus = 1;

    if (CGInputControl::GetActive()->m_mouseModeFlags & 0x1) {
        g_theGxDevicePtr->CursorSetVisible(1);
    }

    EventSetMouseMode(MOUSE_MODE_NORMAL, 0);
}

// OFFSET: 0x008D18D0
void CGGameUI::HandleSpriteTrack(const CObjectTrackEvent& evt) {
    CGGameUI::HandleObjectTrackChange(evt.m_guid, evt.m_prevGuid);
}

// OFFSET: 0x008D89D0/0x14064A5C0
int32_t CGGameUI::OnSpriteRightClick(WOWGUID guid) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player || player->HasAuraEffect(334)) {
        return 1;
    }

    CGObject_C* object = ClntObjMgrObjectPtr(guid, TYPEMASK_OBJECT, __FILE__, __LINE__);

    if (!object) {
        return 0;
    }

    CGUnit_C* unit = static_cast<CGUnit_C*>(object);

    if (object->m_obj->m_type != (TYPEMASK_OBJECT | TYPEMASK_UNIT) || !unit->m_creatureCacheEntry || !(unit->m_creatureCacheEntry->m_typeFlags[0] & 0x20000000)) {
        CGGameUI::Target(guid);
    }

    object->OnRightClick();
    return 1;
}

// OFFSET: 0x008DD4A0
int32_t CGGameUI::HandleTerrainClick(const CTerrainClickEvent& evt) {
    if (evt.m_button == MOUSE_BUTTON_RIGHT || CGGameUI::m_cursorVirtualID) {
        CGGameUI::ClearCursor(1, 1);
    }

    CGGameUI::OnTerrainClick(evt);
    return 1;
}

// OFFSET: 0x008DD4E0
int32_t CGGameUI::HandleSpriteClick(const CSpriteClickEvent& evt) {
    if (CGGameUI::m_cursorVirtualID) {
        CGGameUI::ClearCursor(1, 1);
    }

    if (evt.m_button == MOUSE_BUTTON_LEFT) {
        CGGameUI::OnSpriteLeftClick(evt.m_guid);
    } else {
        CGGameUI::OnSpriteRightClick(evt.m_guid);
    }

    return 1;
}

// OFFSET: 0x008DD530
int32_t CGGameUI::HandleWorldClick(const CWorldClickEvent& evt) {
    int32_t noCursorItem = CGGameUI::m_cursorItemType == 0;
    CGUnit_C* mover = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(CGUnit_C::s_activeMover, TYPEMASK_PLAYER, __FILE__, __LINE__));
    C3Vector direction(evt.m_segEnd.x - evt.m_segStart.x, evt.m_segEnd.y - evt.m_segStart.y, evt.m_segEnd.z - evt.m_segStart.z);

    if (noCursorItem && direction.x * direction.x + direction.z * direction.z + direction.y * direction.y != 0.0f && mover && mover->CanAutoInteract() && evt.m_button == MOUSE_BUTTON_RIGHT) {
        mover->SetTrackingDirection(direction);
        return 1;
    }

    if (evt.m_button == MOUSE_BUTTON_RIGHT || !CGGameUI::m_cursorItem || !CGGameUI::m_cursorItemContainer) {
        CGGameUI::ClearCursor(1, 1);
    }

    if (evt.m_button == MOUSE_BUTTON_LEFT) {
        if (CGGameUI::m_cursorItem && CGGameUI::m_cursorItemContainer) {
            CGGameUI::DeleteCursorItem();
            return 1;
        }

        if (noCursorItem && g_deselectOnClickCVar->m_intValue) {
            CGGameUI::Target(WOWGUID());
        }
    }

    return 1;
}

// OFFSET: 0x008C5190/0x140634E20
void CGGameUI::GetCursorItem(WOWGUID& item, WOWGUID& container, uint32_t& slot) {
    item = CGGameUI::m_cursorItem;
    container = CGGameUI::m_cursorItemContainer;
    slot = CGGameUI::m_cursorItemSlot;
}

// OFFSET: 0x008C5270/0x140634F00
void CGGameUI::UnlockItem(WOWGUID guid) {
    CGItem_C* item = static_cast<CGItem_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_ITEM, __FILE__, __LINE__));

    if (item) {
        item->Unlock();
    }

    CGActionBar::UpdateUsable();
}

// OFFSET: 0x008C9D60/0x14063A080
void CGGameUI::ResetCamera(WOWGUID guid) {
    WOWGUID target = guid;

    if (!guid) {
        target = ClntObjMgrGetActivePlayer();
    }

    CGObject_C* object = ClntObjMgrObjectPtr(target, TYPEMASK_OBJECT, __FILE__, __LINE__);

    if (!object || !CGWorldFrame::s_currentWorldFrame) {
        SErrDisplayAppFatalMessage("", "", 0);
    }

    CGWorldFrame::s_currentWorldFrame->SetCameraTarget(object, 0);
}

// OFFSET: 0x008CA700/0x14063ABD0
void CGGameUI::SendTarget(WOWGUID guid) {
    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_SET_SELECTION));
    msg.Put(guid.m_guid);
    msg.Finalize();
    ClientServices::Send2(&msg);
}

// OFFSET: 0x008CACE0/0x14063B2F0
void CGGameUI::ClearCursor(int32_t unlock, int32_t signal) {
    if (CGPlayer_C::IsGiftWrapping()) {
        CGPlayer_C::CancelGiftWrap();
    }

    CursorDropItem();
    SE3SoundKitProperties properties;

    switch (CGGameUI::m_cursorItemType) {
        case 1: {
            if (unlock && CGGameUI::m_cursorItem) {
                CGGameUI::UnlockItem(CGGameUI::m_cursorItem);
            }

            CGItem_C* item = static_cast<CGItem_C*>(ClntObjMgrObjectPtr(CGGameUI::m_cursorItem, TYPEMASK_ITEM, __FILE__, __LINE__));

            if (item) {
                SI3::PlayItemSound(1, item);
            }

            CGGameUI::m_cursorItem = WOWGUID();
            CGGameUI::m_cursorItemContainer = WOWGUID();
            CGGameUI::m_cursorItemSlot = 0;
            break;
        }

        case 2:
            CGGameUI::m_cursorMoney = 0;
            SE3::PlaySoundKit("LOOTWINDOWCOINSOUND", NULL, &properties, NULL, __FILE__, __LINE__, "CGGameUI::ClearCursor");

            if (signal) {
                FrameScript_SignalEvent(SCRIPT_EVENT_PLAYER_MONEY, NULL);
            }

            break;

        case 3:
            CGGameUI::m_cursorSpell = 0;
            SE3::PlaySoundKit("INTERFACESOUND_CURSORDROPOBJECT", NULL, &properties, NULL, __FILE__, __LINE__, "CGGameUI::ClearCursor");
            break;

        case 4:
            CGGameUI::m_cursorPetAction.m_data = 0;
            SE3::PlaySoundKit("INTERFACESOUND_CURSORDROPOBJECT", NULL, &properties, NULL, __FILE__, __LINE__, "CGGameUI::ClearCursor");
            break;

        case 5:
        case 6:
            SI3::PlayItemSound(1, CGGameUI::m_cursorVirtualDisplay);
            CGGameUI::m_cursorVirtualID = 0;
            CGGameUI::m_cursorVirtualDisplay = 0;
            CGGameUI::m_cursorVirtualSlot = 0;
            SE3::PlaySoundKit("INTERFACESOUND_CURSORDROPOBJECT", NULL, &properties, NULL, __FILE__, __LINE__, "CGGameUI::ClearCursor");
            break;

        case 7:
            CGGameUI::m_cursorMacro = 0;
            SE3::PlaySoundKit("INTERFACESOUND_CURSORDROPOBJECT", NULL, &properties, NULL, __FILE__, __LINE__, "CGGameUI::ClearCursor");
            break;

        case 9:
            CGGameUI::m_cursorPet = 0;
            SE3::PlaySoundKit("INTERFACESOUND_CURSORDROPOBJECT", NULL, NULL, NULL, __FILE__, __LINE__, "CGGameUI::ClearCursor");
            break;

        case 10:
            SI3::PlayItemSound(1, CGGameUI::m_cursorVirtualDisplay);

            if (unlock && CGGameUI::m_cursorVirtualID) {
                CGGuildBankInfo::Unlock(CGGameUI::m_cursorVirtualID, CGGameUI::m_cursorVirtualSlot);
                FrameScript_SignalEvent(SCRIPT_EVENT_GUILDBANK_ITEM_LOCK_CHANGED, NULL);
            }

            CGGameUI::m_cursorVirtualID = 0;
            CGGameUI::m_cursorVirtualDisplay = 0;
            CGGameUI::m_cursorVirtualSlot = 0;
            break;

        case 11:
            CGGameUI::m_cursorMoney = 0;
            SE3::PlaySoundKit("LOOTWINDOWCOINSOUND", NULL, &properties, NULL, __FILE__, __LINE__, "CGGameUI::ClearCursor");

            if (signal) {
                FrameScript_SignalEvent(SCRIPT_EVENT_GUILDBANK_UPDATE_MONEY, NULL);
            }

            break;

        case 12:
            CGGameUI::m_cursorEquipmentSet = -1;
            SE3::PlaySoundKit("INTERFACESOUND_CURSORDROPOBJECT", NULL, &properties, NULL, __FILE__, __LINE__, "CGGameUI::ClearCursor");
            break;

        case 14:
            CGGameUI::m_cursorFlyout = 0;
            SE3::PlaySoundKit("INTERFACESOUND_CURSORDROPOBJECT", NULL, NULL, NULL, __FILE__, __LINE__, "CGGameUI::ClearCursor");
            break;

        case 15:
            SI3::PlayItemSound(1, CGGameUI::m_cursorVirtualDisplay);
            CGGameUI::m_cursorVirtualDisplay = 0;
            CGGameUI::m_cursorVoidStorageItem = WOWGUID();
            FrameScript_SignalEvent(SCRIPT_EVENT_VOID_STORAGE_CONTENTS_UPDATE, NULL);
            break;

        default:
            break;
    }

    if (CGGameUI::m_cursorHasAction) {
        if (CGGameUI::m_cursorItemType == 4) {
            CGPetInfo::HideGrid();
        } else {
            CGActionBar::HideGrid();
        }

        CGGameUI::m_cursorHasAction = 0;
    }

    CGGameUI::m_cursorItemType = 0;
    FrameScript_SignalEvent(SCRIPT_EVENT_CURSOR_UPDATE, NULL);
}

// OFFSET: 0x008D1420/0x140645180
void CGGameUI::HandleObjectTrackChange(WOWGUID guid, WOWGUID prevGuid) {
    CGUnit_C* player = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return;
    }

    WOWGUID previous = prevGuid;

    if (previous || (previous = CGGameUI::m_currentObjectTrack)) {
        CGGameUI::m_currentObjectTrack = WOWGUID();
        CGObject_C* object = ClntObjMgrObjectPtr(previous, TYPEMASK_OBJECT, __FILE__, __LINE__);
        CGGameUI::m_gameTooltip->FadeOut();

        if (object) {
            object->HideHighlightType(1, 0);
        }
    }

    CGGameUI::m_currentObjectTrack = guid;
    CGObject_C* object = ClntObjMgrObjectPtr(guid, TYPEMASK_OBJECT, __FILE__, __LINE__);

    if (!object) {
        return;
    }

    if (CGGameUI::m_visible || (g_unitHighlightsCVar && g_unitHighlightsCVar->m_intValue)) {
        object->ShowHighlightType(1, 0);
    }

    switch (static_cast<uint32_t>(object->m_obj->m_type)) {
        case TYPEMASK_OBJECT | TYPEMASK_UNIT:
        case TYPEMASK_OBJECT | TYPEMASK_UNIT | TYPEMASK_PLAYER: {
            CGUnit_C* unit = static_cast<CGUnit_C*>(object);

            if (CSimpleTop::GetInstance()->m_mouseFocus == CGWorldFrame::s_currentWorldFrame) {
                CGGameUI::m_gameTooltip->SetDefaultAnchor();
                int32_t showHealth = !unit->m_creatureCacheEntry || !(unit->m_creatureCacheEntry->m_typeFlags[1] & 4);
                WOWGUID none = WOWGUID();
                CGGameUI::m_gameTooltip->SetUnit(guid, showHealth, none, 0);
            }

            FrameScript_SignalEvent(SCRIPT_EVENT_UPDATE_MOUSEOVER_UNIT, NULL);
            SE3SoundKitProperties properties;

            if (unit->IsHatedTowards(player)) {
                SE3::PlaySoundKit("GAMEHIGHLIGHTHOSTILEUNIT", NULL, &properties, NULL, __FILE__, __LINE__, "CGGameUI::HandleObjectTrackChange");
            } else if (unit->IsFriendly(player)) {
                SE3::PlaySoundKit("GAMEHIGHLIGHTFRIENDLYUNIT", NULL, &properties, NULL, __FILE__, __LINE__, "CGGameUI::HandleObjectTrackChange");
            } else {
                SE3::PlaySoundKit("GAMEHIGHLIGHTNEUTRALUNIT", NULL, &properties, NULL, __FILE__, __LINE__, "CGGameUI::HandleObjectTrackChange");
            }

            break;
        }

        case TYPEMASK_OBJECT | TYPEMASK_GAMEOBJECT: {
            if (CSimpleTop::GetInstance()->m_mouseFocus == CGWorldFrame::s_currentWorldFrame) {
                if (object->FloatingTooltip()) {
                    CGGameUI::m_gameTooltip->SetOwner(CGGameUI::m_UISimpleParent, ANCHOR_CURSOR, 0.0f, 0.0f);
                } else {
                    CGGameUI::m_gameTooltip->SetDefaultAnchor();
                }

                CGGameUI::m_gameTooltip->SetObject(guid);
            }

            SE3SoundKitProperties properties;
            SE3::PlaySoundKit("GAMEHIGHLIGHTNEUTRALUNIT", NULL, &properties, NULL, __FILE__, __LINE__, "CGGameUI::HandleObjectTrackChange");
            break;
        }

        case TYPEMASK_OBJECT | TYPEMASK_CORPSE: {
            if (CSimpleTop::GetInstance()->m_mouseFocus == CGWorldFrame::s_currentWorldFrame) {
                CGGameUI::m_gameTooltip->SetDefaultAnchor();
                CGGameUI::m_gameTooltip->SetCorpse(guid);
            }

            SE3SoundKitProperties properties;
            SE3::PlaySoundKit("GAMEHIGHLIGHTNEUTRALUNIT", NULL, &properties, NULL, __FILE__, __LINE__, "CGGameUI::HandleObjectTrackChange");
            break;
        }

        default:
            break;
    }
}

// OFFSET: 0x008D2070/0x14063B0F0
void CGGameUI::UpdateCombatMode() {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return;
    }

    CGUIBindings* bindings = CGUIBindings::s_bindings;

    if (!bindings) {
        SErrDisplayAppFatalMessage("", "", 0);
    }

    if ((player->m_unit->m_unitFieldFlags & 0x80000) || (player->m_combatModeFlags & 0x2) || player->CGUnit_C::IsInMelee()) {
        bindings->EnableBindingMode(BINDING_MODE_1, 1);
    } else {
        bindings->EnableBindingMode(BINDING_MODE_1, 0);
    }
}

// OFFSET: 0x008D3720
bool CanTriggerTutorial(uint32_t tutorial) {
    return CGTutorial::m_tutorialShown.m_count && !(CGTutorial::m_tutorialShown.m_bits.m_data[tutorial >> 5] & (1 << (tutorial & 0x1F)));
}

// OFFSET: 0x005640E0
bool IsTutorialFlagged(uint32_t tutorial) {
    return CGTutorial::m_tutorialFlags.m_count && (CGTutorial::m_tutorialFlags.m_bits.m_data[tutorial >> 5] & (1 << (tutorial & 0x1F)));
}

// OFFSET: 0x008D5160/0x140645A00
void CGGameUI::ClearTarget(WOWGUID guid, int32_t a2) {
    int32_t noMgr = ClntObjMgrGetCurrent() == NULL;

    if (!CGGameUI::m_lockedTarget || (guid && guid != CGGameUI::m_lockedTarget)) {
        return;
    }

    if (CGLootInfo::m_lootObj == CGGameUI::m_lockedTarget) {
        CGGameUI::CloseLoot(true, false, false);
    }

    if (!noMgr) {
        CGObject_C* object = ClntObjMgrObjectPtr(CGGameUI::m_lockedTarget, TYPEMASK_OBJECT, __FILE__, __LINE__);

        if (object) {
            object->HideHighlightType(0, 0);

            if (object->m_obj->m_type & TYPEMASK_UNIT) {
                CGUnit_C* unit = static_cast<CGUnit_C*>(object);
                PlayerNameTriggerColorUpdate(unit->m_nameDesc);
                unit->UnregisterScript(0x1000);
                unit->StopNamePlateSpellbar();
            }
        }
    }

    CGGameUI::m_lockedTarget = WOWGUID();

    if (noMgr) {
        return;
    }

    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player) {
        player->SetCombatMode(true);

        if (player->GetPossessedUnit()) {
            CGPetInfo::PetClearTarget();
        }
    }

    if (a2) {
        CDataStore msg;
        msg.Put(static_cast<uint32_t>(CMSG_SET_SELECTION));
        msg.Put(CGGameUI::m_lockedTarget.m_guid);
        msg.Finalize();
        ClientServices::Send2(&msg);
        CGSpellBook::UpdateUsable();
        FrameScript_SignalEvent(SCRIPT_EVENT_PLAYER_TARGET_CHANGED, NULL);
    }
}

// OFFSET: 0x008DBF70/0x140650360
void CGGameUI::DeleteCursorItem() {
    CGItem_C* item = static_cast<CGItem_C*>(ClntObjMgrObjectPtr(CGGameUI::m_cursorItem, TYPEMASK_ITEM, __FILE__, __LINE__));

    if (!item || item->m_item->m_itemFieldOwner != ClntObjMgrGetActivePlayer()) {
        return;
    }

    if (item->GetItemStaticFlags(0) & 0x20) {
        CGGameUI::DisplayError(ERR_ITEM_CANT_BE_DESTROYED);
        return;
    }

    WOWGUID none = WOWGUID();
    ItemRecSparse* rec = g_itemSparseDB.GetRecord(item->m_obj->m_entryID, none, NULL, NULL, false);

    if (rec) {
        FrameScript_SignalEvent(SCRIPT_EVENT_DELETE_ITEM_CONFIRM, "%s%d", rec->GetName(0), rec->m_quality);
    }
}

// OFFSET: 0x008D60D0/0x14064A510
void CGGameUI::AcceptAreaSpiritHeal() {
    CGGameUI::UpdateAreaSpiritHealerDistance();

    if (!CGGameUI::m_areaSpiritHealerGUID) {
        return;
    }

    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_AREA_SPIRIT_HEALER_QUEUE));
    msg.Put(CGGameUI::m_areaSpiritHealerGUID.m_guid);
    msg.Finalize();
    ClientServices::Send2(&msg);
}

// OFFSET: 0x008D6240/0x140648190
void CGGameUI::Target(WOWGUID guid) {
    bool noPlayer;

    if (!ClntObjMgrGetCurrent() || CGGameUI::m_currentlyReloadingUI) {
        noPlayer = true;
    } else {
        noPlayer = false;

        if (lua_tainted) {
            CGGameUI::ShowBlockedActionFeedback(NULL, 0);
            return;
        }
    }

    if (!guid) {
        s_nearestSortTime = 0;

        if (CGGameUI::m_lockedTarget) {
            CGGameUI::m_lastTarget = CGGameUI::m_lockedTarget;
            CGGameUI::ClearTarget(CGGameUI::m_lockedTarget, 1);

            if (Spell_C_GetAutoRepeatingSpell()) {
                Spell_C_CancelAutoRepeat(true);
            }
        }

        return;
    }

    CGPlayer_C* player = NULL;

    if (!noPlayer) {
        player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

        if (player && (player->m_unit->m_unitFieldCharmedby || (player->HasAuraEffect(334) && guid != player->m_obj->m_guid))) {
            return;
        }
    }

    CGObject_C* object = ClntObjMgrObjectPtr(guid, TYPEMASK_OBJECT, __FILE__, __LINE__);

    if (!object) {
        if (guid == CGGameUI::m_lockedTarget) {
            return;
        }

        SE3SoundKitProperties properties;
        SE3::PlaySoundKit("igCharacterSelect", NULL, &properties, NULL, __FILE__, __LINE__, "CGGameUI::Target");
    } else {
        if (Spell_C_IsTargeting()) {
            Spell_C_HandleSpriteClick(object, NULL);
            return;
        }

        if (!object->CanBeTargetted() || guid == CGGameUI::m_lockedTarget) {
            return;
        }

        if (CGGameUI::m_visible || (g_unitHighlightsCVar && g_unitHighlightsCVar->m_intValue)) {
            object->ShowHighlightType(0, 0);
        }

        if (object->m_obj->m_type & TYPEMASK_UNIT) {
            CGUnit_C* unit = static_cast<CGUnit_C*>(object);

            if (unit->m_unit->m_unitFieldFlags2 & 0x4000000) {
                return;
            }

            PlayerNameTriggerColorUpdate(unit->m_nameDesc);
            unit->RegisterScript(0x1000);
            const SpellRec* spell = g_spellDB.GetRecord(unit->m_castingSpellId);

            if (spell || (spell = g_spellDB.GetRecord(unit->m_channelSpellId)) != NULL) {
                unit->ProcessCastNotInterruptible(spell);
            }

            unit->UpdateSpellCastBars();

            if (player) {
                if (player->CanAttackNow(unit)) {
                    CGGameUI::m_lastEnemyTarget = guid;
                } else if (player->CanAssist(unit, false)) {
                    CGGameUI::m_lastFriendTarget = guid;
                }

                if ((unit->m_obj->m_type & TYPEMASK_PLAYER) && unit->m_obj->m_guid != ClntObjMgrGetActivePlayer() && CanTriggerTutorial(18) && static_cast<int32_t>(player->m_unit->m_unitFieldLevel) >= 5) {
                    CGTutorial::TriggerTutorial(18);
                } else if ((unit->m_obj->m_type & TYPEMASK_PLAYER) && player->CanCooperate(unit) && static_cast<int32_t>(player->m_unit->m_unitFieldLevel) >= 5) {
                    CGTutorial::TriggerTutorial(17);
                } else if (player->CanAttack(unit)) {
                    CGTutorial::TriggerTutorial(4);
                } else if (unit->m_unit->m_unitNpcFlags & 0x2000) {
                    CGTutorial::TriggerTutorial(34);
                } else if (unit->m_questStatus == 1) {
                    CGTutorial::TriggerTutorial(42);
                }
            }
        }
    }

    bool inMelee = player && player->IsInMelee();
    CGGameUI::m_lastTarget = CGGameUI::m_lockedTarget;
    CGGameUI::ClearTarget(CGGameUI::m_lockedTarget, 0);
    CGGameUI::m_lockedTarget = guid;
    CGGameUI::SendTarget(guid);
    CGSpellBook::UpdateUsable();
    CGSpellActivationOverlay::UpdateUsable();
    CGSpellActivationOverlay::UpdateActiveMoverOverlays();
    FrameScript_SignalEvent(SCRIPT_EVENT_PLAYER_TARGET_CHANGED, NULL);

    if (g_stopAutoAttackOnTargetChangeCVar->m_intValue) {
        if (Spell_C_GetAutoRepeatingSpell()) {
            Spell_C_CancelAutoRepeat(true);
        }
    } else if (inMelee) {
        ClntObjMgrGetActivePlayerObj()->CombatModeEnter(CGGameUI::m_lockedTarget, 0, 0, 0, 0);
    }
}

// OFFSET: 0x008DCFD0
int32_t CGGameUI::OnTerrainClick(const CTerrainClickEvent& evt) {
    if (evt.m_button == MOUSE_BUTTON_LEFT) {
        if (Spell_C_HandleTerrainClick(evt)) {
            return 1;
        }

        if (evt.m_button == MOUSE_BUTTON_LEFT && CGPetInfo::HandleTerrainClick(evt)) {
            return 1;
        }

        if (evt.m_button == MOUSE_BUTTON_LEFT) {
            if (CGGameUI::m_cursorItem && CGGameUI::m_cursorItemContainer) {
                CGGameUI::DeleteCursorItem();
                return 1;
            }

            if (g_deselectOnClickCVar->m_intValue) {
                CGGameUI::Target(WOWGUID());
            }

            return 1;
        }
    }

    CGPlayer_C* mover = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(CGUnit_C::s_activeMover, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (!mover) {
        return 1;
    }

    C3Vector point = evt.m_point;

    if (evt.m_guid) {
        CGObject_C* object = ClntObjMgrObjectPtr(evt.m_guid, TYPEMASK_OBJECT, __FILE__, __LINE__);

        if (!object) {
            return 1;
        }

        point = evt.m_point * object->GetMatrix();
    }

    mover->CTMClickTerrain(mover->m_movement->GetRawPosition(point));
    return 1;
}

// OFFSET: 0x008DD1B0
int32_t CGGameUI::OnSpriteLeftClick(WOWGUID guid) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return 0;
    }

    CGObject_C* object = ClntObjMgrObjectPtr(guid, TYPEMASK_OBJECT, __FILE__, __LINE__);

    if (!object) {
        return 0;
    }

    if ((object->m_obj->m_type & TYPEMASK_PLAYER) && ((CGGameUI::m_cursorItem && CGGameUI::m_cursorItemContainer) || CGGameUI::m_cursorMoney) && !Spell_C_IsTargeting()) {
        int32_t bound = 0;

        if (CGGameUI::m_cursorItem) {
            CGItem_C* item = static_cast<CGItem_C*>(ClntObjMgrObjectPtr(CGGameUI::m_cursorItem, TYPEMASK_ITEM, __FILE__, __LINE__));

            if (item) {
                if (item->IsBound()) {
                    bound = 1;
                }

                if (item->m_obj->m_type & TYPEMASK_CONTAINER) {
                    CGBag_C* bag = item->GetBag();

                    if (bag) {
                        if (bag->GetNumItems()) {
                            CGGameUI::DisplayError(ERR_TRADE_BAG);
                            goto npc;
                        }

                        if (static_cast<uint32_t>(player->GetBag()->FindItemSlot(item) - 19) <= 3) {
                            CGGameUI::DisplayError(ERR_TRADE_EQUIPPED_BAG);
                            goto npc;
                        }
                    }
                }
            }
        } else if (!CGGameUI::m_cursorMoney) {
            goto npc;
        }

        if (object->m_obj->m_type & TYPEMASK_PLAYER) {
            if (player->IsDeadOrGhost()) {
                CGGameUI::DisplayError(ERR_PLAYER_DEAD);
            } else if (!player->CanCooperate(static_cast<CGUnit_C*>(object))) {
                CGGameUI::DisplayError(ERR_PLAYER_WRONG_FACTION);
            } else if (!CGTradeInfo::m_tradingPlayer) {
                CGTradeInfo::InitiateTrade(guid, 1);
            } else if (CGTradeInfo::m_tradingPlayer == guid) {
                CGTradeInfo::AddCursorItem(bound);
            } else {
                CGGameUI::DisplayError(ERR_ALREADY_TRADING);
            }
        }
    }

npc:
    if (object->m_obj->m_type & TYPEMASK_UNIT) {
        if (guid == CGPetInfo::GetPet(0) && CGGameUI::m_cursorItem) {
            CGItem_C* item = static_cast<CGItem_C*>(ClntObjMgrObjectPtr(CGGameUI::m_cursorItem, TYPEMASK_ITEM, __FILE__, __LINE__));

            if (item && Spell_C_HandleItemDrop(item, static_cast<CGUnit_C*>(object))) {
                CGGameUI::ClearCursor(1, 1);
            }
        }

        static_cast<CGUnit_C*>(object)->HandleNPCFeedback();
    }

    if ((object->m_obj->m_type & TYPEMASK_GAMEOBJECT) && !object->CanBeTargetted()) {
        WOWGUID item;
        WOWGUID container;
        uint32_t slot;
        CGGameUI::GetCursorItem(item, container, slot);

        if (item && container) {
            CGGameUI::DeleteCursorItem();
        }
    }

    CGGameUI::Target(guid);

    if (g_interactOnLeftClickCVar->m_intValue >= 1) {
        object->OnInteract();
    }

    return 1;
}

// OFFSET: 0x008D2C10/0x14063BF50
void CGGameUI::DisplayError(UIErrors error, ...) {
    if (error >= ERR_MAX_VALUE) {
        return;
    }

    if (error == ERR_BAG_FULL) {
        CGTutorial::TriggerTutorial(57);
    }

    const UIErrorInfo& info = s_errorInfo[error];

    if (info.m_vocalError == 68) {
        if (_stricmp(info.m_sound, "NONE")) {
            SE3SoundKitProperties properties;
            SE3::PlaySoundKit(s_errorInfo[error].m_sound, NULL, &properties, NULL, __FILE__, __LINE__, "CGGameUI::DisplayError");
        }
    } else {
        SI3::PlayVocalErrorSound(info.m_vocalError);
    }

    const char* token = s_errorInfo[error].m_token;

    if (!token || !*token) {
        return;
    }

    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));
    const char* text = player ? player->GetFrameScriptText(token, -1) : FrameScript_GetText(token, -1, GENDER_NOT_APPLICABLE);

    char buffer[3000];
    va_list args;
    va_start(args, error);
    SStrVPrintf(buffer, sizeof(buffer), text, args);
    va_end(args);
    SStrCopy(CGGameUI::m_lastErrorMessage, buffer, sizeof(((CGGameUI*)0)->m_lastErrorMessage));

    switch (s_errorInfo[error].m_type) {
        case 0:
            CGChat::AddChatMessage(buffer, static_cast<SLASH_COMMAND_ID>(s_errorInfo[error].m_chatType), NULL);
            break;

        case 1:
            if (buffer[0]) {
                FrameScript_SignalEvent(SCRIPT_EVENT_UI_INFO_MESSAGE, "%s", buffer);
            }

            break;

        case 2:
            if (buffer[0]) {
                FrameScript_SignalEvent(SCRIPT_EVENT_UI_ERROR_MESSAGE, "%s", buffer);
            }

            break;

        case 3:
            ConsoleWriteA(buffer, static_cast<COLOR_T>(3));
            break;

        default:
            break;
    }
}

// OFFSET: 0x008C5830/0x140635630
uint8_t CGGameUI::GetComboPoints(WOWGUID target, bool anyTarget) {
    if (!target) {
        target = CGGameUI::m_lockedTarget;
    }

    if (anyTarget) {
        if (!ClntObjMgrObjectPtr(CGGameUI::m_comboTarget, TYPEMASK_UNIT, __FILE__, __LINE__)) {
            return 0;
        }
    } else if (target != CGGameUI::m_comboTarget) {
        return 0;
    }

    return CGGameUI::m_comboPoints;
}

// OFFSET: 0x008C45C0/0x140634150
int32_t CGGameUI::IsPartyMember(const WOWGUID& guid) {
    if (CGPartyInfo::IsMember(guid)) {
        return 1;
    }

    if (IsGuidInMyRaid(guid)) {
        return 1;
    }

    return 0;
}

// OFFSET: 0x008C57A0
void CGGameUI::ClearInstanceResetMap() {
    CGGameUI::m_uiLastInstanceTime = 0;
    CGGameUI::m_iMapID = 0;
}

// OFFSET: 0x008C57B0
void UpdateInstanceOwnershipPacket(int32_t owns) {
    CGGameUI::m_bOwnsInstance = owns;
}

// OFFSET: 0x008C5170/0x140634E00
WOWGUID CGGameUI::GetCursorItem() {
    if (CGGameUI::m_cursorItemType == 1) {
        return CGGameUI::m_cursorItem;
    }

    return WOWGUID();
}

// OFFSET: 0x008C51D0/0x140634E40
int32_t CGGameUI::GetCursorSpell() {
    return CGGameUI::m_cursorSpell;
}

// OFFSET: 0x008C51E0/0x140634E50
int32_t CGGameUI::GetCursorVirtualItem() {
    return CGGameUI::m_cursorVirtualID;
}

// OFFSET: 0x008D22C0/0x1406463A0
void CGGameUI::SetCursorMoney(uint64_t amount, int32_t type, int32_t event) {
    if (amount >= 10000000000ULL || !CGGameUI::m_hasControl || (!amount && CGGameUI::m_cursorItemType != type)) {
        return;
    }

    CGGameUI::ClearCursor(1, 1);

    if (!amount) {
        return;
    }

    CGGameUI::m_cursorMoney = amount;
    CGGameUI::m_cursorItemType = type;
    SE3SoundKitProperties properties;
    SE3::PlaySoundKit("LOOTWINDOWCOINSOUND", NULL, &properties, NULL, __FILE__, __LINE__, "CGGameUI::SetCursorMoney");
    CursorGrabMoney(amount);

    if (event) {
        FrameScript_SignalEvent(event, NULL);
    }
}

// OFFSET: 0x008D2390/0x1406464B0
void CGGameUI::SetCursorSpell(int32_t spellId, int32_t displaySpellId) {
    if (!g_spellDB.GetRecord(spellId)) {
        return;
    }

    const char* texture = CGSpellBook::GetSpellTexture(displaySpellId ? displaySpellId : spellId);

    if (!texture || !*texture) {
        return;
    }

    CGGameUI::ClearCursor(1, 1);
    CGGameUI::m_cursorSpell = spellId;
    CGGameUI::m_cursorItemType = 3;
    SE3SoundKitProperties properties;
    SE3::PlaySoundKit("INTERFACESOUND_CURSORGRABOBJECT", NULL, &properties, NULL, __FILE__, __LINE__, "CGGameUI::SetCursorSpell");
    CursorGrabSpell(texture);
    CGActionBar::ShowGrid();
    CGGameUI::m_cursorHasAction = 1;
}

// OFFSET: 0x008D2480
void CGGameUI::DropCursorSpell() {
    if (CGGameUI::m_cursorItemType == 3) {
        CGGameUI::ClearCursor(1, 1);
    }
}

// OFFSET: 0x008D24A0/0x1406465F0
void CGGameUI::SetCursorPetAction(const PetAction* action) {
    if (!CGGameUI::m_hasControl) {
        return;
    }

    const char* texture = CGPetInfo::GetPetActionTexture(*action);

    if (!texture) {
        return;
    }

    CGGameUI::ClearCursor(1, 1);
    CGGameUI::m_cursorPetAction = *action;
    CGGameUI::m_cursorItemType = 4;
    SE3SoundKitProperties properties;
    SE3::PlaySoundKit("INTERFACESOUND_CURSORGRABOBJECT", NULL, &properties, NULL, __FILE__, __LINE__, "CGGameUI::SetCursorPetAction");
    CursorGrabSpell(texture);
    CGPetInfo::ShowGrid();
    CGGameUI::m_cursorHasAction = 1;
}

// OFFSET: 0x008D2560/0x1406466D0
void CGGameUI::SetCursorMacro(int32_t macroId) {
    char texture[260];
    CGUIMacros::GetFullIconPath(macroId, texture, sizeof(texture));

    if (texture[0]) {
        CGGameUI::ClearCursor(1, 1);
        CGGameUI::m_cursorMacro = macroId;
        CGGameUI::m_cursorItemType = 7;
        SE3SoundKitProperties properties;
        SE3::PlaySoundKit("INTERFACESOUND_CURSORGRABOBJECT", NULL, &properties, NULL, __FILE__, __LINE__, "CGGameUI::SetCursorMacro");
        CursorGrabSpell(texture);
        CGActionBar::ShowGrid();
        CGGameUI::m_cursorHasAction = 1;
    }
}

// OFFSET: 0x008D2620/0x1406467B0
void CGGameUI::SetCursorPet(int32_t index) {
    CGGameUI::ClearCursor(1, 1);

    if (static_cast<uint32_t>(index) >= 25) {
        return;
    }

    StablePetInfo* pet = &CGStableInfo::m_pets[index];
    WOWGUID none = WOWGUID();
    CreatureStats_C* stats = g_creatureDBCache.GetRecord(pet->m_entry, none, NULL, NULL, false);

    if (!stats) {
        return;
    }

    CreatureFamilyRec* family = g_creatureFamilyDB.GetRecord(stats->m_family);

    if (family && *family->m_iconFile) {
        CGGameUI::m_cursorPet = index;
        CGGameUI::m_cursorItemType = 9;
        SE3SoundKitProperties properties;
        SE3::PlaySoundKit("INTERFACESOUND_CURSORGRABOBJECT", NULL, &properties, NULL, __FILE__, __LINE__, "CGGameUI::SetCursorPet");
        CursorGrabSpell(family->m_iconFile);
    }
}

// OFFSET: 0x008D2730/0x140646920
void CGGameUI::SetCursorVirtualItem(int32_t id, int32_t displayId, int32_t slot, int32_t type, int32_t stackSplit) {
    if (!CGGameUI::m_hasControl && type != 6) {
        return;
    }

    CGGameUI::ClearCursor(1, 1);

    if (!id) {
        return;
    }

    CGGameUI::m_cursorVirtualID = id;
    CGGameUI::m_cursorVirtualDisplay = displayId;
    CGGameUI::m_cursorVirtualSlot = slot;
    CGGameUI::m_cursorItemType = type;
    CGGameUI::m_stackSplit = stackSplit;

    if (type == 6) {
        CGActionBar::ShowGrid();
        CGGameUI::m_cursorHasAction = 1;
    }

    CursorGrabVirtualItem(displayId);
    SI3::PlayItemSound(0, displayId);
    FrameScript_SignalEvent(SCRIPT_EVENT_CURSOR_UPDATE, NULL);
}

// OFFSET: 0x008D27C0/0x1406469D0
void CGGameUI::SetCursorEquipmentSet(int32_t setId) {
    if (!CGEquipmentManager::GetSetByID(setId)) {
        return;
    }

    char texture[260];
    CGEquipmentManager::GetFullIconPath(setId, texture, sizeof(texture));
    CGGameUI::ClearCursor(1, 1);
    CGGameUI::m_cursorEquipmentSet = setId;
    CGGameUI::m_cursorItemType = 12;
    SE3SoundKitProperties properties;
    SE3::PlaySoundKit("INTERFACESOUND_CURSORGRABOBJECT", NULL, &properties, NULL, __FILE__, __LINE__, "CGGameUI::SetCursorEquipmentSet");
    CursorGrabSpell(texture);
    CGActionBar::ShowGrid();
    CGGameUI::m_cursorHasAction = 1;
}

// OFFSET: 0x008D2880/0x140646AB0
void CGGameUI::SetCursorCurrency(int32_t currencyId) {
    CurrencyTypesRec* currency = g_currencyTypesDB.GetRecord(currencyId);

    if (!currency) {
        return;
    }

    CGGameUI::ClearCursor(1, 1);
    CGGameUI::m_cursorCurrency = currencyId;
    CGGameUI::m_cursorItemType = 13;
    SE3SoundKitProperties properties;
    SE3::PlaySoundKit("INTERFACESOUND_CURSORGRABOBJECT", NULL, &properties, NULL, __FILE__, __LINE__, "CGGameUI::SetCursorCurrency");
    CursorGrabSpell(CGCurrencyTypes::GetCurrencyIcon(currency));
    CGActionBar::ShowGrid();
    CGGameUI::m_cursorHasAction = 1;
}

// OFFSET: 0x008D2950/0x140646BB0
void CGGameUI::SetCursorFlyout(int32_t flyoutId) {
    SpellFlyoutRec* flyout = g_spellFlyoutDB.GetRecord(flyoutId);

    if (!flyout) {
        return;
    }

    SpellIconRec* icon = g_spellIconDB.GetRecord(flyout->m_spellIconID);
    const char* texture = NULL;

    if (icon) {
        texture = icon->m_textureFilename;
    }

    CGGameUI::ClearCursor(1, 1);
    CGGameUI::m_cursorFlyout = flyoutId;
    CGGameUI::m_cursorItemType = 14;
    SE3::PlaySoundKit("INTERFACESOUND_CURSORGRABOBJECT", NULL, NULL, NULL, __FILE__, __LINE__, "CGGameUI::SetCursorFlyout");
    CursorGrabSpell(texture);
    CGActionBar::ShowGrid();
    CGGameUI::m_cursorHasAction = 1;
}

// OFFSET: 0x008D2A10/0x140646CB0
void CGGameUI::DropCursorVoidStorageItem() {
    if (CGGameUI::m_cursorItemType == 15) {
        CGGameUI::ClearCursor(1, 1);
    }
}

// OFFSET: 0x009548E0
int32_t CGGameUI::GetCursorVirtualItemOfType(int32_t type) {
    if (CGGameUI::m_cursorItemType == type) {
        return CGGameUI::GetCursorVirtualItem();
    }

    return 0;
}

// OFFSET: 0x008C5770
void CGGameUI::InitClientControlState(const WOWGUID& guid) {
    CGGameUI::m_hasControl = 1;
    CGUnit_C::InitActiveMover(guid);
}

// OFFSET: 0x008D2BB0
void CGGameUI::OnClientControlChanged(int32_t control) {
    if (control == CGGameUI::m_hasControl) {
        return;
    }

    CGGameUI::m_hasControl = control;

    if (control) {
        FrameScript_SignalEvent(SCRIPT_EVENT_PLAYER_CONTROL_GAINED, NULL);
    } else {
        CGGameUI::ClearCursor(1, 1);
        Spell_C_CancelPlayerSpells();
        FrameScript_SignalEvent(SCRIPT_EVENT_PLAYER_CONTROL_LOST, NULL);
    }

    CGSpellBook::UpdateUsable();
}

// OFFSET: 0x008CBBC0/0x14063C8B0
void CGGameUI::UpdateThreatWarningEnabled(int32_t value) {
    if (value == 0) {
        CGGameUI::m_threatWarningEnabled = 0;
    } else if (value == 1) {
        MapRec* map = g_mapDB.GetRecord(CGGameUI::m_iCurrentMapID);
        CGGameUI::m_threatWarningEnabled = map && (map->m_instanceType == 1 || map->m_instanceType == 2);
    } else if (value == 2 && !CGPartyInfo::m_members[0] && !CGRaidInfo::m_numMembers) {
        CGGameUI::m_threatWarningEnabled = 0;
    } else {
        CGGameUI::m_threatWarningEnabled = 1;
    }

    FrameScript_SignalEvent(SCRIPT_EVENT_UNIT_THREAT_SITUATION_UPDATE, NULL);
    FrameScript_SignalEvent(SCRIPT_EVENT_UNIT_THREAT_LIST_UPDATE, NULL);
}

// OFFSET: 0x008C4810/0x140634400
void CGGameUI::SetDeathReleaseLocation(int32_t mapId, const C3Vector& position) {
    CGGameUI::m_deathReleaseMapID = mapId;
    CGGameUI::m_deathReleaseLoc = position;
}

// OFFSET: 0x008C46D0/0x140634260
static void ItemPushItemStatsCallback(int32_t id, const WOWGUID& guid, void* param, bool success) {
    ItemPushData* data = static_cast<ItemPushData*>(param);

    if (!data) {
        SErrDisplayAppFatalMessage("", "", 0);
    }

    if (success) {
        CGGameUI::OnItemPush(data->m_guid, data->m_bag, data->m_slot, id, data->m_suffixFactor, data->m_randomPropertyId, data->m_received, data->m_created, data->m_showInChat, data->m_count);
    }

    SMemFree(data, "delete", SLOG_FUNCTION, 0);
}

// OFFSET: 0x008DBA40/0x14064FD20
void CGGameUI::OnItemPush(WOWGUID guid, uint8_t bag, uint32_t slot, uint32_t itemId, uint32_t suffixFactor, uint32_t randomPropertyId, uint32_t received, uint32_t created, uint32_t showInChat, uint32_t count) {
    WOWGUID none = WOWGUID();
    ItemRecSparse* sparse = g_itemSparseDB.GetRecord(itemId, none, NULL, NULL, false);
    ItemRec* item;

    if (!sparse || !(item = sparse->GetNonSparse())) {
        ItemPushData* data = static_cast<ItemPushData*>(SMemAlloc(sizeof(ItemPushData), __FILE__, __LINE__, 0));
        data->m_guid = guid;
        data->m_bag = bag;
        data->m_slot = slot;
        data->m_received = received;
        data->m_created = created;
        data->m_showInChat = showInChat;
        data->m_suffixFactor = suffixFactor;
        data->m_randomPropertyId = randomPropertyId;
        data->m_count = count;
        g_itemSparseDB.GetRecord(itemId, guid, ItemPushItemStatsCallback, data, false);
        return;
    }

    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return;
    }

    char text[3000];

    if (guid == ClntObjMgrGetActivePlayer()) {
        const char* path = ClientDBStringLookup(3);
        const char* separator = *path ? "\\" : "";
        SStrPrintf(text, sizeof(text), "%s%s%s", path, separator, CGItem_C::GetInventoryArt(item->m_displayInfoID));
        int32_t slotIndex = bag == 255 ? -1 : bag;
        FrameScript_SignalEvent(SCRIPT_EVENT_ITEM_PUSH, "%d%s", slotIndex + 1, text);

        if (showInChat) {
            if (static_cast<int32_t>(count) > 1) {
                const char* token = created ? "LOOT_ITEM_CREATED_SELF_MULTIPLE" : received ? "LOOT_ITEM_PUSHED_SELF_MULTIPLE" : "LOOT_ITEM_SELF_MULTIPLE";
                const char* format = FrameScript_GetText(token, -1, GENDER_NOT_APPLICABLE);
                SStrPrintf(text, sizeof(text), format, CGTooltip::GetItemHyperlinkString(itemId, sparse->m_quality, 0, randomPropertyId, suffixFactor, 0), count);
            } else {
                const char* token = created ? "LOOT_ITEM_CREATED_SELF" : received ? "LOOT_ITEM_PUSHED_SELF" : "LOOT_ITEM_SELF";
                const char* format = FrameScript_GetText(token, -1, GENDER_NOT_APPLICABLE);
                SStrPrintf(text, sizeof(text), format, CGTooltip::GetItemHyperlinkString(itemId, sparse->m_quality, 0, randomPropertyId, suffixFactor, 0));
            }

            CGChat::AddChatMessage(text, static_cast<SLASH_COMMAND_ID>(27), NULL);
        }

        int32_t level = player->m_unit->m_unitFieldLevel;

        if (level >= 4) {
            CGTutorial::TriggerTutorial(7);
        }

        for (uint32_t i = 0; i < 5; i++) {
            if (level >= 4 && sparse->m_spellID[i] && !sparse->m_spellTrigger[i] && (item->m_classID || (item->m_subclassID != 4 && item->m_subclassID != 5))) {
                CGTutorial::TriggerTutorial(8);
                break;
            }
        }

        uint32_t slots = s_inventoryTypeSlots[sparse->m_inventoryType];

        for (uint32_t i = 0; i < 23; i++) {
            if (((1 << i) & slots) && IsTutorialFlagged(55)) {
                CGTutorial::TriggerTutorial(23);
                break;
            }
        }

        if (itemId == 6948) {
            CGTutorial::TriggerTutorial(30);
        }

        SI3::PlayItemSound(1, item->m_displayInfoID);
    } else if ((!received || created) && showInChat) {
        if (static_cast<int32_t>(count) > 1) {
            const char* format = FrameScript_GetText(created ? "CREATED_ITEM_MULTIPLE" : "LOOT_ITEM_MULTIPLE", -1, GENDER_NOT_APPLICABLE);
            const char* link = CGTooltip::GetItemHyperlinkString(itemId, sparse->m_quality, 0, randomPropertyId, suffixFactor, 0);
            SStrPrintf(text, sizeof(text), format, player->GetUnitName(NULL, 1), link, count);
        } else {
            const char* token = created ? "CREATED_ITEM" : CGLootInfo::IsPlayerIneligible(itemId) ? "LOOT_ITEM_WHILE_PLAYER_INELIGIBLE" : "LOOT_ITEM";
            const char* format = FrameScript_GetText(token, -1, GENDER_NOT_APPLICABLE);
            const char* link = CGTooltip::GetItemHyperlinkString(itemId, sparse->m_quality, 0, randomPropertyId, suffixFactor, 0);
            SStrPrintf(text, sizeof(text), format, player->GetUnitName(NULL, 1), link);
        }

        CGChat::AddChatMessage(text, static_cast<SLASH_COMMAND_ID>(27), NULL);
    }

    if (guid == ClntObjMgrGetActivePlayer() && (sparse->m_inventoryType == 15 || sparse->m_inventoryType == 26) && player->m_unit->m_unitFieldBytes0[1] != 3) {
        CGTutorial::TriggerTutorial(43);
        return;
    }

    if (item->m_classID == 2 && item->m_subclassID == 19) {
        CGTutorial::TriggerTutorial(43);
    }
}

// OFFSET: 0x008D11C0
void CGGameUI::SetCorpseLocation(int32_t mapId, const C3Vector& position, int32_t actualMapId, int32_t transport) {
    CGGameUI::m_corpseMapID = mapId;
    CGGameUI::m_corpsePos = position;
    CGGameUI::m_corpseActualMapID = actualMapId;

    if (transport) {
        CGGameUI::m_corpseTransportGUID.m_guid = static_cast<int64_t>(transport) | 0x1FC0000000000000ULL;
    } else {
        CGGameUI::m_corpseTransportGUID = WOWGUID();
    }

    C3Vector corpse(0.0f, 0.0f, 0.0f);
    CGGameUI::GetCorpsePosition(corpse);
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player && mapId == player->GetNewContinentID()) {
        MinimapSetCorpsePOI(corpse.x, corpse.y);
    } else {
        MinimapSetCorpsePOI(0.0f, 0.0f);
    }
}

// OFFSET: 0x008D59D0
void CGGameUI::CheckInstanceDifficultyMessages(uint32_t dungeonDifficulty, uint32_t raidDifficulty) {
    if (raidDifficulty != CGGameUI::GetCurrentRaidDifficulty()) {
        CGGameUI::SendRaidDifficultyMessage();
    }

    if (dungeonDifficulty != CGGameUI::GetCurrentDungeonDifficulty()) {
        CGGameUI::ShowDungeonDifficultyChanged();
    }
}

// OFFSET: 0x008D5F40
void CGGameUI::NextCinematic(CM2Model* model, uint32_t boneId, uint32_t sequenceId, int32_t interrupted, uint32_t time, uint64_t param) {
    if (interrupted) {
        return;
    }

    M2BoneSequenceInfo info;
    model->GetBoneSequenceInfo(boneId, info);
    model->SetBoneSequenceTime(boneId, info.m_endTime - info.m_startTime - 1);
    uint32_t sceneTime = model->m_scene->m_time;

    if (!sceneTime) {
        sceneTime = 1;
    }

    model->m_lastAnimTime = sceneTime;
    EnableFadingScreen(0.25f, &CGGameUI::NextCinematicInternal, NULL);
}

// OFFSET: 0x008D4CE0
void CGGameUI::UpdateCorpseLocation() {
    C3Vector position(0.0f, 0.0f, 0.0f);
    CGGameUI::SetCorpseLocation(-1, position, -1, 0);

    if (CGGameUI::m_corpseInRange) {
        CGGameUI::m_corpseInRange = 0;

        if (CGBattlefieldInfo::m_instanceType != 4) {
            FrameScript_SignalEvent(SCRIPT_EVENT_CORPSE_OUT_OF_RANGE, NULL);
        }
    }

    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player && (player->m_player->m_playerFlags & 0x10) && CGBattlefieldInfo::m_instanceType != 4) {
        CDataStore msg;
        msg.Put(static_cast<uint32_t>(MSG_CORPSE_QUERY));
        msg.Finalize();
        ClientServices::Send2(&msg);
    }
}

// OFFSET: 0x008D8E10/0x14064C540
void CGGameUI::BeginCinematicInternal(void* param) {
    int32_t started = CGGameUI::StartCinematicSound();
    LoadingScreenDisable();

    if (started && CGGameUI::StartCinematicCamera()) {
        FrameScript_SignalEvent(SCRIPT_EVENT_CINEMATIC_START, "%b", 1);
    } else {
        CGGameUI::StopCinematicInternal(param);
    }
}

// OFFSET: 0x008D8E50/0x14064C590
void CGGameUI::EnterWorld() {
    if (CGGameUI::s_inWorld) {
        return;
    }

    CGGameUI::s_inWorld = 1;
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));
    player->GetObjectModel()->WaitUntilLoadedRecursive();
    CCharacterComponent* component = player->m_characterComponent;

    if (component) {
        component->RenderPrep(1);

        if (component->m_flags & 0x40) {
            player->TurnOffGuildTabardPurchase();
        }
    }

    if (player->m_obj->m_guid == ClntObjMgrGetActivePlayer() && player->m_player->m_playerFarsight && (player->m_combatModeFlags & 0x1)) {
        WOWGUID farSight = { player->GetFarSightGuid() };
        CGGameUI::ResetCamera(farSight);
    } else {
        CGWorldFrame::s_currentWorldFrame->SetCameraTarget(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_OBJECT, __FILE__, __LINE__), 0);
    }

    CGInputControl::GetActive()->Reset();
    FrameScript_MemoryCleanup();

    if (!CGGameUI::m_currentlyReloadingUI) {
        for (uint32_t i = 0; i < 5; i++) {
            CGGameUI::m_totems[i].m_guid = WOWGUID();
            CGGameUI::m_totems[i].m_name = NULL;
            CGGameUI::m_totems[i].m_duration = 0;
            CGGameUI::m_totems[i].m_startTime = 0;
            CGGameUI::m_totems[i].m_icon = NULL;
        }
    }

    CGGameUI::m_autoStoreItem = WOWGUID();
    CGGameUI::m_autoStoreItemID = 0;

    if (CGGameUI::m_loggingIn) {
        CGGameUI::m_loggingIn = 0;
        FrameScript_SignalEvent(SCRIPT_EVENT_PLAYER_LOGIN, NULL);
        CGLCD::Login();
    }

    CGRuneInfo::EnterWorld();
    FrameScript_SignalEvent(SCRIPT_EVENT_PLAYER_ENTERING_WORLD, NULL);
    CGMinimapFrame::EnterWorld();
    CGChat::EnterWorld();
    CGActionBar::EnterWorld();
    CGCharacterInfo::EnterWorld();
    CGContainerInfo::EnterWorld();
    CGQuestInfo::EnterWorld();
    CGQuestLog::EnterWorld();
    CGClassTrainer::EnterWorld();
    CGMerchantInfo::EnterWorld();
    CGTradeInfo::EnterWorld();
    CGBankInfo::EnterWorld();
    CGPetInfo::EnterWorld();
    CGWorldMap::EnterWorld();
    CGReputationInfo::EnterWorld();
    CGTabardCreationFrame::EnterWorld();
    CGGuildRegistrar::EnterWorld();
    CGPetitionInfo::EnterWorld();
    CGTaxiMap::EnterWorld();
    CGMailInfo::EnterWorld();
    CGBattlefieldInfo::EnterWorld();
    CGTalentInfo::EnterWorld();
    CGTalentInfo_Inspect::EnterWorld();
    CGAuctionHouse::EnterWorld();
    CGStableInfo::EnterWorld();
    CGRaidInfo::EnterWorld();
    CGItemSocketInfo::EnterWorld();
    CGLCD::EnterWorld();
    CGLookingForGroup::EnterWorld();
    CGArenaTeamInfo::EnterWorld();
    g_capturePointUIManager.m_nextWarningTime = FrameTime::GetCurTimeMs();
    CGGuildBankInfo::EnterWorld();
    CGGuildInfo::EnterWorld();
    CGCalendar::EnterWorld();
    CGBarberShop::EnterWorld();
    CGCurrencyTypes::EnterWorld();
    CVehicle_C::EnterWorld();
    CGEquipmentManager::EnterWorld();
    CGGMTicketInfo::EnterWorld();
    CGAchievementInfo::EnterWorld();
    CGReforge::EnterWorld();
    CGResearchFrame::EnterWorld();
    CGQuestObjectiveCache::s_dirty = 1;
    CGSpellActivationOverlay::EnterWorld();
    CGCUFProfiles::EnterWorld(CGGameUI::m_currentlyReloadingUI);
    CGVoidStorageInfo::EnterWorld();

    WOWGUID lockedTarget = CGGameUI::m_lockedTarget;

    if (lockedTarget) {
        CDataStore msg;
        msg.Put(static_cast<uint32_t>(CMSG_SET_SELECTION));
        msg.Put(lockedTarget.m_guid);
        msg.Finalize();
        ClientServices::Send2(&msg);
    }

    if (player->m_unit->m_unitFieldFlags & 0x80000) {
        FrameScript_SignalEvent(SCRIPT_EVENT_PLAYER_REGEN_DISABLED, NULL);
        CGGameUI::m_simpleTop->m_allowProtectedActions = 0;
        CGGameUI::UpdateCombatMode();
    }

    CGGameUI::UpdateCorpseLocation();
    C3Vector releaseLoc(0.0f, 0.0f, 0.0f);
    CGGameUI::SetDeathReleaseLocation(-1, releaseLoc);
    CGTutorial::TriggerTutorial(0);
    CGGameUI::UpdateThreatWarningEnabled(g_threatWarningCVar->m_intValue);

    if (CGGameUI::m_inCinematic) {
        EnableFadingScreen(0.25f, CGGameUI::BeginCinematicInternal, NULL);
    }

    MapRec* map = g_mapDB.GetRecord(CGGameUI::m_iCurrentMapID);

    if (map && (map->m_instanceType == 3 || map->m_instanceType == 4)) {
        FrameScript_SignalEvent(SCRIPT_EVENT_PLAYER_ENTERING_BATTLEGROUND, NULL);
    }

    int32_t voiceChat = s_cvarEnableVoiceChat ? s_cvarEnableVoiceChat->m_intValue : 0;
    int32_t microphone = s_cvarEnableMicrophone ? s_cvarEnableMicrophone->m_intValue : 0;

    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_VOICE_SESSION_ENABLE));
    msg.Put(static_cast<uint8_t>(voiceChat != 0));
    msg.Put(static_cast<uint8_t>(microphone != 0));
    msg.Finalize();
    ClientServices::Send2(&msg);
    PhaseShift_C_EnterWorld(CGGameUI::m_currentlyReloadingUI);
}

// OFFSET: 0x008C46C0/0x140634250
void CGGameUI::UnitModelUpdate(const WOWGUID& guid, uint32_t flags) {
    UpdateModelForUI(guid, flags);
}

// OFFSET: 0x008D17A0
int32_t CGGameUI::FilterMouseDown(CMouseEvent* evt) {
    if (evt->m_id == 0x400500C8 && evt->m_data.m_button == 4) {
        if (CGGameUI::m_cursorItem || CGGameUI::m_cursorVirtualID || CGGameUI::m_cursorMoney || CGGameUI::m_cursorSpell || CGGameUI::m_cursorPetAction.m_data || CGGameUI::m_cursorFlyout || CGGameUI::m_cursorMacro || CGGameUI::m_cursorPet || CGGameUI::m_cursorVoidStorageItem || CGPlayer_C::IsGiftWrapping()) {
            CGGameUI::ClearCursor(1, 1);
            return 1;
        }
    }

    if (evt->m_data.m_mode != 1) {
        return 0;
    }

    if (evt->m_id == 0x400500C8) {
        CGWorldFrame::s_currentWorldFrame->OnLayerMouseDown(*evt, NULL);
    } else {
        CGWorldFrame::s_currentWorldFrame->OnLayerMouseUp(*evt, NULL);
    }

    return 1;
}

// OFFSET: 0x008D1870
int32_t CGGameUI::HandleMouseDown(const CMouseEvent& evt) {
    if (evt.m_data.m_button != 4) {
        return 0;
    }

    if (Spell_C_IsTargeting()) {
        Spell_C_StopTargeting();
    }

    if (CGPetInfo::m_petOrderMoveTo) {
        CGPetInfo::ClearPetOrderMoveTo();
    }

    if (CGPlayer_C::IsGiftWrapping()) {
        CGGameUI::ClearCursor(1, 1);
    }

    if (CursorGetResetMode() == 17) {
        CursorSetResetMode(1);
        CursorSetMode(1);
    }

    return 0;
}

// PROVEN: 79259ec7 IDENTICAL/IDENTICAL
// OFFSET: 0x00842700/0x1407FDD80
int32_t CGGameUI::HandleMouseUp(const CMouseEvent& evt) {
    return 0;
}

// OFFSET: 0x008C4070
int32_t CGGameUI::SequenceMusicIsPlaying() {
    return CGGameUI::m_inCinematic;
}

// OFFSET: 0x008C40C0
void CGGameUI::CancelPartyInvite() {
    FrameScript_SignalEvent(SCRIPT_EVENT_PARTY_INVITE_CANCEL, NULL);
}

// OFFSET: 0x008C45B0/0x140634140
int32_t CGGameUI::IsInParty(const WOWGUID& guid) {
    return CGPartyInfo::IsMember(guid);
}

// OFFSET: 0x008C40A0
void CGGameUI::OpenPlayerSkinned(bool a1) {
    FrameScript_SignalEvent(SCRIPT_EVENT_PLAYER_SKINNED, "%d", a1 != 0);
}

// OFFSET: #0x00512540
void SignalEvent_EVENT_PARTY_INVITE_REQUEST(const char* name) {
    FrameScript_SignalEvent(SCRIPT_EVENT_PARTY_INVITE_REQUEST, "%s", name);
}

// OFFSET: 0x008CACC0
void CGGameUI::ShowBlockedFrameFeedback(CScriptRegion* region) {
    CGGameUI::ShowBlockedActionFeedback(region, 2);
}

// OFFSET: 0x008C5810/0x1406354D0
uint32_t CGGameUI::GetCurrentRaidDifficulty() {
    if (!CGRaidInfo::m_numMembers) {
        return CGGameUI::m_raidDifficulty;
    }

    return CGPartyInfo::m_raidDifficulty;
}

// OFFSET: 0x008C5AD0
void CGGameUI::GetInstanceLockInfo(uint32_t* remaining, uint32_t* completedMask, uint32_t* extending) {
    *remaining = 0;

    if (CGGameUI::m_instanceLockTime) {
        int32_t left = CGGameUI::m_instanceLockTime - FrameTime::GetCurTimeMs();

        if (left > 0) {
            *remaining = left;
        }
    }

    *completedMask = CGGameUI::m_instanceLockCompletedMask;
    *extending = CGGameUI::m_instanceLockExtending;
}

// OFFSET: 0x008C5B70
void CGGameUI::SetSummonRequest(WOWGUID summoner, uint32_t area, uint32_t timeout) {
    CGGameUI::m_summonConfirmGUID = summoner;
    CGGameUI::m_summonConfirmArea = area;
    CGGameUI::m_summonConfirmTime = timeout + FrameTime::GetCurTimeMs();
    FrameScript_SignalEvent(SCRIPT_EVENT_CONFIRM_SUMMON, NULL);
}

// OFFSET: 0x008C4E90
void CGGameUI::SetUIScale(float scale) {
    if (scale <= 0.64f) {
        scale = 0.64f;
    }

    if (CGGameUI::m_UISimpleParent) {
        CGGameUI::m_UISimpleParent->SetFrameScale(scale, true);
        CLayoutFrame::ResizePending();
        FrameScript_SignalEvent(SCRIPT_EVENT_UPDATE_FLOATING_CHAT_WINDOWS, NULL);
        FrameScript_SignalEvent(SCRIPT_EVENT_UI_SCALE_CHANGED, NULL);
    }
}

// OFFSET: 0x008C5C70/0x1406359C0
void CGGameUI::UpdateUnitHighlights(int32_t force) {
    bool show = CGGameUI::m_visible || (g_unitHighlightsCVar && g_unitHighlightsCVar->m_intValue);

    if (CGGameUI::m_currentObjectTrack) {
        CGObject_C* object = ClntObjMgrObjectPtr(CGGameUI::m_currentObjectTrack, TYPEMASK_OBJECT, __FILE__, __LINE__);

        if (object) {
            if (show) {
                object->ShowHighlightType(1, force);
            } else {
                object->HideHighlightType(1, force);
            }
        }
    }

    if (CGGameUI::m_lockedTarget) {
        CGObject_C* object = ClntObjMgrObjectPtr(CGGameUI::m_lockedTarget, TYPEMASK_OBJECT, __FILE__, __LINE__);

        if (object) {
            if (show) {
                object->ShowHighlightType(0, force);
            } else {
                object->HideHighlightType(0, force);
            }
        }
    }
}

// OFFSET: 0x008D5310/0x140648620
void CGGameUI::SetFocusTarget(const WOWGUID& guid) {
    if (guid == CGGameUI::m_focusTarget) {
        return;
    }

    if (lua_tainted) {
        CGGameUI::ShowBlockedActionFeedback(NULL, 0);
        return;
    }

    if (CGGameUI::m_focusTarget) {
        CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(CGGameUI::m_focusTarget, TYPEMASK_UNIT, __FILE__, __LINE__));

        if (unit) {
            unit->UnregisterScript(0x2000);
        }
    }

    CGGameUI::m_focusTarget = guid;

    if (CGGameUI::m_focusTarget) {
        CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(CGGameUI::m_focusTarget, TYPEMASK_UNIT, __FILE__, __LINE__));

        if (unit) {
            unit->RegisterScript(0x2000);
        }
    }

    FrameScript_SignalEvent(SCRIPT_EVENT_PLAYER_FOCUS_CHANGED, NULL);
}

// OFFSET: 0x008CB530
void CGGameUI::DestroyTotem(int32_t slot, WOWGUID guid) {
    if (static_cast<uint32_t>(slot) > 4) {
        return;
    }

    TotemInfo* totem = &CGGameUI::m_totems[slot];

    if (!totem || !totem->m_guid) {
        return;
    }

    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_TOTEM_DESTROYED));
    msg.Put(static_cast<uint8_t>(slot));
    msg.Put(guid ? guid.m_guid : totem->m_guid.m_guid);
    msg.Finalize();
    ClientServices::Send2(&msg);
    totem->m_guid = WOWGUID();
    totem->m_name = NULL;
    totem->m_duration = 0;
    totem->m_startTime = 0;
    totem->m_icon = NULL;
    FrameScript_SignalEvent(SCRIPT_EVENT_PLAYER_TOTEM_UPDATE, "%d", slot + 1);
}

// OFFSET: 0x008C44B0/0x140634040
static int32_t GetSavedVariablesPath(char* path, uint32_t size, int32_t perCharacter) {
    if (perCharacter) {
        SStrPrintf(path, size, "WTF\\Account\\%s\\%s\\%s", ClientServices::GetAccountName(), ClientServices::GetSelectedRealmName(), ClientServices::GetCharacterName());
    } else {
        SStrPrintf(path, size, "WTF\\Account\\%s", ClientServices::GetAccountName());
    }

    if (!OsDirectoryExists(path)) {
        OsCreateDirectory(path, 1);
    }

    SStrPack(path, "\\SavedVariables.lua", size);
    return 1;
}

// OFFSET: 0x008C4560
void LoadSavedVariables(CStatus* status) {
    char path[260];

    for (int32_t i = 0; i < 2; i++) {
        if (GetSavedVariablesPath(path, sizeof(path), i)) {
            FrameScript_LoadVariables(path, status);
        }
    }
}

// OFFSET: 0x008C5210/0x140634E80
int32_t CGGameUI::IsAutoLooting() {
    if (!CGUIBindings::s_bindings) {
        SErrDisplayAppFatalMessage("", "", 0);
    }

    int32_t autoLoot = g_autoLootDefaultCVar->m_intValue;

    if (CGUIBindings::s_bindings->IsModifiedClick("AUTOLOOTTOGGLE")) {
        return autoLoot == 0;
    }

    return autoLoot;
}

// OFFSET: 0x008D8A70
void CCapturePointUIManager::AddCapturePoint(CGGameObject_C* object, uint32_t worldState, uint32_t radius) {
    if (!object) {
        SErrDisplayAppFatalMessage("", "", 0);
    }

    WOWGUID guid = object->m_obj->m_guid;
    CapturePointUIManagerNode* node = this->m_nodes.NewNode(2, 0, 0);
    node->m_guid = guid;
    node->m_radiusSquared = radius * radius;
    node->m_worldState = worldState;
}

// OFFSET: 0x008C50F0
void CGGameUI::SetMinimapZoneText(const char* text) {
    if (text && *text) {
        if (CGGameUI::m_minimapZoneText) {
            if (!SStrCmpUTF8I(CGGameUI::m_minimapZoneText, text, STORM_MAX_STR)) {
                return;
            }

            if (CGGameUI::m_minimapZoneText) {
                SMemFree(const_cast<char*>(CGGameUI::m_minimapZoneText), __FILE__, __LINE__, 0);
            }
        }

        CGGameUI::m_minimapZoneText = SStrDupA(text, __FILE__, __LINE__);
    } else if (CGGameUI::m_minimapZoneText && *CGGameUI::m_minimapZoneText) {
        *const_cast<char*>(CGGameUI::m_minimapZoneText) = 0;
    }
}

// OFFSET: 0x008D1FA0/0x14063B010
bool CGGameUI::CanInspect(CGUnit_C* unit) {
    CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();

    if (!player || !unit) {
        return false;
    }

    if (!(unit->m_obj->m_type & TYPEMASK_PLAYER) || player->CanAttack(unit)) {
        return false;
    }

    C3Vector unitPosition = unit->GetPosition();
    C3Vector playerPosition = player->GetPosition();
    float dy = playerPosition.y - unitPosition.y;
    float dz = playerPosition.z - unitPosition.z;
    float dx = playerPosition.x - unitPosition.x;
    return dx * dx + dz * dz + dy * dy <= s_inspectDistSq;
}

// OFFSET: 0x008D55D0/0x140649030
void CGGameUI::NewZoneFeedback(int32_t areaId, int32_t subzoneId, int32_t wmoGroupId, const char* zoneText, const char* subZoneText, int32_t inside) {
    int32_t hadArea = CGGameUI::m_areaID != 0;
    int32_t newArea = CGGameUI::m_areaID != areaId;
    CGGameUI::m_areaID = areaId;
    CGGameUI::m_subzoneID = subzoneId;
    CGGameUI::m_wmoGroupID = wmoGroupId;

    if (!hadArea) {
        CGWorldMap::SetMapToCurrentZone(0);
    }

    int32_t textChanged = 0;

    if (zoneText && *zoneText) {
        if (!CGGameUI::m_zoneText || SStrCmpUTF8I(CGGameUI::m_zoneText, zoneText, STORM_MAX_STR)) {
            if (CGGameUI::m_zoneText) {
                SMemFree(const_cast<char*>(CGGameUI::m_zoneText), __FILE__, __LINE__, 0);
            }

            CGGameUI::m_zoneText = SStrDupA(zoneText, __FILE__, __LINE__);
            textChanged = 1;
            CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

            if (player && !(player->m_unit->m_unitFieldFlags & 0x100000) && !CGGameUI::m_inCinematic && !CGPlayer_C::GetPendingMovie() && !CGGameUI::m_inMovie && !CGPlayer_C::GetPendingCinematic()) {
                CGChat::ZoneUpdate();
            }
        }
    } else if (CGGameUI::m_zoneText && *CGGameUI::m_zoneText) {
        *const_cast<char*>(CGGameUI::m_zoneText) = 0;
        textChanged = 1;
    }

    if (subZoneText && *subZoneText) {
        if (!CGGameUI::m_subZoneText || SStrCmpUTF8I(CGGameUI::m_subZoneText, subZoneText, STORM_MAX_STR)) {
            if (CGGameUI::m_subZoneText) {
                SMemFree(const_cast<char*>(CGGameUI::m_subZoneText), __FILE__, __LINE__, 0);
            }

            CGGameUI::m_subZoneText = SStrDupA(subZoneText, __FILE__, __LINE__);
            textChanged = 1;
        }
    } else if (CGGameUI::m_subZoneText && *CGGameUI::m_subZoneText) {
        *const_cast<char*>(CGGameUI::m_subZoneText) = 0;
        textChanged = 1;
    }

    if (newArea) {
        FrameScript_SignalEvent(SCRIPT_EVENT_ZONE_CHANGED_NEW_AREA, NULL);

        if (!CGGameUI::s_cemeteryListRequestTime) {
            CGGameUI::s_cemeteryListRequestTime = s_cemeteryListRequestDelay + FrameTime::GetCurTimeMs();
        }
    } else if (textChanged) {
        FrameScript_SignalEvent(inside ? SCRIPT_EVENT_ZONE_CHANGED_INDOORS : SCRIPT_EVENT_ZONE_CHANGED, NULL);
    }

    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));
    CGGameUI::m_areaMountFlags = player ? player->GetMountFlags() : CGGameUI::GetCurrentAreaMountFlags();
    CGSpellBook::UpdateUsable();
}

// OFFSET: 0x008DE0C0/0x140652D90
void CGGameUI::RegisterSavedVariable(const char* name, int32_t perCharacter) {
    if (!CGGameUI::s_savedVariables[perCharacter].Ptr(name)) {
        CGGameUI::s_savedVariables[perCharacter].New(name, 0, 0);
    }
}

// OFFSET: 0x008C5D80
const char* MirrorTimerTypeToken(int32_t type) {
    switch (type) {
        case 0:
            return "EXHAUSTION";

        case 1:
            return "BREATH";

        case 2:
            return "FEIGNDEATH";
    }

    return "UNKNOWN";
}

// OFFSET: 0x008CB620
void CGGameUI::TargetTradeItem() {
    if (!CGTradeInfo::m_targetTrade.m_items[6].m_itemID) {
        return;
    }

    const SpellRec* spell = g_spellDB.GetRecord(Spell_C_GetTargetingSpell());

    if (!spell) {
        return;
    }

    uint32_t count;
    const TSGrowableArray<const SpellEffectRec*>* effects = SpellGetEffects(spell->m_id, count);

    for (uint32_t i = 0; i < count; i++) {
        const SpellEffectRec* effect = effects->m_data[i];

        if (!effect || effect->m_effect != 53) {
            continue;
        }

        int32_t current = CGTradeInfo::GetTargetEnchant(6);

        if (!current) {
            continue;
        }

        SpellItemEnchantmentRec* existing = g_spellItemEnchantmentDB.GetRecord(current);
        SpellItemEnchantmentRec* replacement = g_spellItemEnchantmentDB.GetRecord(effect->m_effectMiscValue);

        if (existing && replacement) {
            FrameScript_SignalEvent(SCRIPT_EVENT_TRADE_REPLACE_ENCHANT, "%s%s", static_cast<const char*>(existing->m_name), static_cast<const char*>(replacement->m_name));
            return;
        }
    }

    Spell_C_TargetTradeItem();
}

// OFFSET: 0x008CB730/0x14063C290
const char* GetMirrorTimerLabel(int32_t type, int32_t spellId) {
    const SpellRec* spell = g_spellDB.GetRecord(spellId);

    if (spell) {
        return spell->m_name;
    }

    char label[128];
    SStrPrintf(label, sizeof(label), "%s_LABEL", MirrorTimerTypeToken(type));
    return FrameScript_GetText(label, -1, GENDER_NOT_APPLICABLE);
}

// OFFSET: 0x008C4A20/0x140634630
void CGGameUI::HandleScreenshot(int32_t success) {
    if (!CGGameUI::m_initialized) {
        return;
    }

    if (success) {
        FrameScript_SignalEvent(SCRIPT_EVENT_SCREENSHOT_SUCCEEDED, NULL);
    } else {
        FrameScript_SignalEvent(SCRIPT_EVENT_SCREENSHOT_FAILED, NULL);
    }
}

// OFFSET: 0x008C4A60/0x140634870
void CGGameUI::CloseInteraction() {
    WOWGUID target = CGGameUI::m_interactTarget;

    if (!target) {
        return;
    }

    CGGameUI::m_interactTarget = WOWGUID();
    WOWGUID none = WOWGUID();

    if (CGTradeInfo::m_tradingPlayer == target) {
        CGTradeInfo::SetTradePartner(none, 0, 0);
    } else if (CGItemText::m_itemGUID == target) {
        CGItemText::SetItem(none, 0);
    } else if (CGGossipInfo::m_gossip == target) {
        CGGossipInfo::SetGossip(none);
    } else if (CGQuestInfo::m_npc == target) {
        CGQuestInfo::QuestGiverFinished(0, 0, 1, 0);
    } else if (CGMerchantInfo::m_merchant == target) {
        CGMerchantInfo::CloseMerchant();
    } else if (CGTaxiMap::m_unit == target) {
        CGTaxiMap::CloseMap();
    } else if (CGClassTrainer::m_trainer == target) {
        CGClassTrainer::SetTrainer(none, 0, 0);
    } else if (CGBankInfo::m_banker == target) {
        CGBankInfo::SetBanker(none);
    } else if (CGGuildBankInfo::m_banker == target) {
        CGGuildBankInfo::SetGuildBanker(none);
    } else if (CGGuildRegistrar::m_registrar == target) {
        CGGuildRegistrar::CloseRegistrar();
    } else if (CGTabardCreationFrame::m_vendor == target) {
        CGTabardCreationFrame::Close();
    } else if (CGMailInfo::m_object == target) {
        CGMailInfo::CloseMail(false);
    } else if (CGAuctionHouse::m_auctioneer == target) {
        CGAuctionHouse::CloseAuctionHouse();
    } else if (CGStableInfo::m_stableMaster == target) {
        CGStableInfo::CloseStables(false);
    } else if (CGBattlefieldInfo::m_battlemaster == target) {
        CGBattlefieldInfo::SetBattlemaster(none);
    } else if (CGReforge::m_forgeMaster == target) {
        CGReforge::SetForgeMaster(none);
    } else if (CGTransmogrify::m_transmogrifier == target) {
        CGTransmogrify::SetTransmogrifier(none);
    } else if (CGVoidStorageInfo::m_npc == target) {
        CGVoidStorageInfo::SetNPC(none);
    }

    CGGameObject_C* object = static_cast<CGGameObject_C*>(ClntObjMgrObjectPtr(target, TYPEMASK_GAMEOBJECT, __FILE__, __LINE__));

    if (object) {
        object->CloseInteraction();
    }
}

// OFFSET: 0x008CA680/0x14063AB40
void CGGameUI::ClearInteractTarget(const WOWGUID& guid, int32_t a2) {
    if (!CGGameUI::m_interactTarget || CGGameUI::m_interactTarget != guid) {
        return;
    }

    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(CGGameUI::m_interactTarget, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (unit && !(unit->m_obj->m_type & TYPEMASK_PLAYER) && !a2) {
        unit->PlayNPCSound(false);
    }

    CGGameUI::CloseInteraction();

    if (unit && !(unit->m_obj->m_type & TYPEMASK_PLAYER)) {
        unit->UpdateEmoteAnimKit();
    }
}

// OFFSET: 0x008CB7C0
int32_t CGGameUI::OnMirrorTimerEvent(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    if (msgId == SMSG_START_MIRROR_TIMER) {
        uint32_t type;
        uint32_t value;
        uint32_t maxValue;
        uint32_t scale;
        uint8_t paused;
        uint32_t spellId;
        msg->Get(type);
        msg->Get(value);
        msg->Get(maxValue);
        msg->Get(scale);
        msg->Get(paused);
        msg->Get(spellId);

        FrameScript_SignalEvent(SCRIPT_EVENT_MIRROR_TIMER_START, "%s%d%d%d%d%s", MirrorTimerTypeToken(type), value, maxValue, scale, paused, GetMirrorTimerLabel(type, spellId));

        if (type == 0) {
            CGTutorial::TriggerTutorial(26);
        } else if (type == 1) {
            CGTutorial::TriggerTutorial(28);
        }

        if (type <= 2) {
            MirrorTimerInfo* info = &CGGameUI::m_mirrorTimerInfo[type];
            info->m_type = type;
            info->m_value = value;
            info->m_maxValue = maxValue;
            info->m_scale = scale;
            info->m_paused = paused;
            info->m_spellId = spellId;
            CGGameUI::m_mirrorTimerInfo[type].unk_0018 = OsGetAsyncTimeMs();
        }

        return 1;
    }

    if (msgId == SMSG_PAUSE_MIRROR_TIMER) {
        uint32_t type;
        uint8_t paused;
        msg->Get(type);
        msg->Get(paused);
        FrameScript_SignalEvent(SCRIPT_EVENT_MIRROR_TIMER_PAUSE, "%s%d", MirrorTimerTypeToken(type), paused);
        return 1;
    }

    if (msgId == SMSG_STOP_MIRROR_TIMER) {
        uint32_t type;
        msg->Get(type);
        FrameScript_SignalEvent(SCRIPT_EVENT_MIRROR_TIMER_STOP, "%s", MirrorTimerTypeToken(type));

        if (type <= 2) {
            MirrorTimerInfo* info = &CGGameUI::m_mirrorTimerInfo[type];
            info->m_type = 3;
            info->m_value = 0;
            info->m_maxValue = 0;
            info->m_scale = 0;
            info->m_paused = 0;
            info->m_spellId = 0;
            info->unk_0018 = 0;
        }
    }

    return 1;
}

// OFFSET: 0x008CB9F0/0x14063C5E0
AreaTableRec* CGGameUI::GetCurrentSubzoneRec() {
    AreaTableRec* area = g_areaTableDB.GetRecord(CGGameUI::m_subzoneID);

    if (area) {
        return area;
    }

    return g_areaTableDB.GetRecord(CGGameUI::m_areaID);
}

// OFFSET: 0x008CBA90/0x14063C630
bool CGGameUI::IsInArea(int32_t areaID) {
    int32_t id = CGGameUI::m_subzoneID;

    while (id) {
        if (id == areaID) {
            return true;
        }

        AreaTableRec* area = g_areaTableDB.GetRecord(id);

        if (!area) {
            return false;
        }

        id = area->m_parentAreaID;
    }

    return false;
}

// OFFSET: 0x008D18F0/0x140645710
void CGGameUI::SetInteractTarget(const WOWGUID& guid) {
    bool same = guid == CGGameUI::m_interactTarget;

    if (CGGameUI::m_interactTarget && !same) {
        CGGameUI::ClearInteractTarget(CGGameUI::m_interactTarget, static_cast<bool>(guid));
    }

    CGGameUI::m_interactTarget = guid;

    if (!CGGameUI::m_interactTarget) {
        return;
    }

    CGObject_C* object = ClntObjMgrObjectPtr(CGGameUI::m_interactTarget, TYPEMASK_OBJECT, __FILE__, __LINE__);

    if (!object) {
        return;
    }

    if (object->m_obj->m_type & TYPEMASK_UNIT) {
        if (object->m_obj->m_type & TYPEMASK_PLAYER) {
            CGGameUI::m_interactMaxDistSq = s_playerInteractDistSq;
            return;
        }

        CGUnit_C* unit = static_cast<CGUnit_C*>(object);
        float reach = unit->m_unit->m_unitFieldCombatreach + 4.0f;
        CGGameUI::m_interactMaxDistSq = reach * reach;

        if (!same) {
            unit->PlayNPCSound(true);
            unit->UpdateEmoteAnimKit();
        }
    } else if (object->m_obj->m_type & TYPEMASK_GAMEOBJECT) {
        CGGameObject_C* gameObject = static_cast<CGGameObject_C*>(object);
        gameObject->StartInteraction();
        CGGameUI::m_interactMaxDistSq = gameObject->GetInteractDistanceSquared();
    }
}

// OFFSET: 0x008D12A0/0x140644FA0
void CGGameUI::GetCorpseLocation(int32_t& mapID, C3Vector& position, int32_t& actualMapID) {
    mapID = CGGameUI::m_corpseMapID;
    CGGameUI::GetCorpsePosition(position);
    actualMapID = CGGameUI::m_corpseActualMapID;
}

// OFFSET: 0x008D4DD0/0x140647DF0
void CGGameUI::SetAreaSpiritHealer(WOWGUID guid, bool cancel) {
    if (CGGameUI::m_areaSpiritHealerGUID == guid) {
        return;
    }

    CGGameUI::m_areaSpiritHealerGUID = guid;

    if (cancel) {
        CGGameUI::CastSpiritHealSpell();
    }

    CGGameUI::m_areaSpiritHealerTime = 0;

    if (!CGGameUI::m_areaSpiritHealerGUID) {
        return;
    }

    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_AREA_SPIRIT_HEALER_QUERY));
    msg.Put(CGGameUI::m_areaSpiritHealerGUID.m_guid);
    msg.Finalize();
    ClientServices::Send2(&msg);
}

// OFFSET: 0x008C5D70
const char* CGGameUI::LastErrorMessage() {
    return CGGameUI::m_lastErrorMessage;
}

// OFFSET: 0x008C5900
void CGGameUI::SetLastEnchantSpellID(int32_t spellId) {
    CGGameUI::m_lastEnchantSpellID = spellId;
}

// OFFSET: 0x008C5910
void CGGameUI::SetLastEnchantItem(WOWGUID guid) {
    CGGameUI::m_lastEnchantItem = guid;
}

// OFFSET: 0x008C5930
void CGGameUI::SetLastItemSpellTarget(WOWGUID guid) {
    CGGameUI::m_lastItemSpellTarget = guid;
}

// OFFSET: 0x008C5040
void CGGameUI::ShowSpellMissFeedback(const WOWGUID& guid, int32_t reason) {
    if (static_cast<uint32_t>(reason) > 11) {
        return;
    }

    uint32_t schoolMask = GetPhysicalDamageClassMask();
    const char* missString = g_spellMissStrings[reason];
    int32_t count;
    char** tokens = Script_GetTokenIDsFromGUID(guid, &count);

    for (int32_t i = 0; i < count; i++) {
        FrameScript_SignalEvent(SCRIPT_EVENT_UNIT_COMBAT, "%s%s%s%d%d", tokens[i], missString, "", 0, schoolMask);
    }
}

// OFFSET: 0x008C50B0/0x140634D10
void CGGameUI::ShowHealingFeedback(const WOWGUID& guid, int32_t amount, bool crit) {
    if (amount) {
        CGGameUI::SignalUnitCombat(guid, "HEAL", amount, GetPhysicalDamageClassMask(), crit ? 0x200 : 0);
    }
}

void CGGameUI::ShowEnergizeFeedback(const WOWGUID& guid, int32_t powerType, int32_t amount) {
}

// OFFSET: 0x008C4F20/0x140634B00
void CGGameUI::SignalUnitCombat(const WOWGUID& guid, const char* action, int32_t amount, uint32_t schoolMask, uint32_t flags) {
    const char* modifier = "";

    if (amount <= 0) {
        if (flags & 0x20) {
            modifier = "ABSORB";
        } else if (flags & 0x2000) {
            modifier = "BLOCK";
        } else if (flags & 0x80) {
            modifier = "RESIST";
        }
    } else if (flags & 0x200) {
        modifier = "CRITICAL";
    } else if (flags & 0x10000) {
        modifier = "GLANCING";
    } else if (flags & 0x20000) {
        modifier = "CRUSHING";
    }

    int32_t count;
    char** tokens = Script_GetTokenIDsFromGUID(guid, &count);

    for (int32_t i = 0; i < count; i++) {
        FrameScript_SignalEvent(SCRIPT_EVENT_UNIT_COMBAT, "%s%s%s%d%d", tokens[i], action, modifier, amount, schoolMask);
    }
}

// OFFSET: 0x008C5890
TotemInfo* CGGameUI::GetTotemInfo(uint32_t slot) {
    if (slot > 4) {
        return NULL;
    }

    return &CGGameUI::m_totems[slot];
}

// OFFSET: 0x008C58B0/0x1406356B0
TotemInfo* CGGameUI::GetTotemInfo(WOWGUID guid) {
    if (!guid) {
        return NULL;
    }

    for (uint32_t i = 0; i < 5; i++) {
        if (CGGameUI::m_totems[i].m_guid == guid) {
            return &CGGameUI::m_totems[i];
        }
    }

    return NULL;
}

// OFFSET: 0x008C5DC0/0x140635B30
int32_t CGGameUI::MirrorTimerToIndex(const char* timer) {
    if (!_stricmp(timer, "EXHAUSTION")) {
        return 0;
    }

    if (!_stricmp(timer, "BREATH")) {
        return 1;
    }

    return (_stricmp(timer, "FEIGNDEATH") != 0) + 2;
}

// OFFSET: 0x008C5E90
MirrorTimerInfo* CGGameUI::MirrorTimerGetInfo(uint32_t index) {
    if (index > 2) {
        return NULL;
    }

    return &CGGameUI::m_mirrorTimerInfo[index];
}

// OFFSET: 0x008C5EC0
uint32_t TotemInfo::GetSpawnTimeLeft() {
    uint32_t now = FrameTime::GetCurTimeMs();
    uint32_t end = this->m_duration + this->m_startTime;

    if (end && static_cast<int32_t>(now - end) < 0) {
        return end - now;
    }

    return 0;
}

// OFFSET: 0x008D13C0/0x140645110
void CGGameUI::CastSpiritHealSpell() {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));
    const SpellRec* spell = g_spellDB.GetRecord(2584);

    if (spell && player) {
        Spell_C_CancelAura(spell, player, -1);
    }
}

// OFFSET: 0x008C4FD0
void CGGameUI::ShowCombatFeedback(const WOWGUID& guid, int32_t amount, uint32_t schoolMask, uint32_t flags) {
    CGGameUI::SignalUnitCombat(guid, "WOUND", amount, schoolMask, flags);
}

// OFFSET: 0x008DC040/0x140650440
void CGGameUI::CastItemSpell(WOWGUID guid, int32_t confirmed) {
    CGItem_C* item = static_cast<CGItem_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_ITEM, __FILE__, __LINE__));

    if (!item) {
        return;
    }

    PendingSpellCast* cast = Spell_C_GetTargetingSpellCast();

    if (!cast) {
        return;
    }

    const SpellRec* spell = g_spellDB.GetRecord(cast->m_data.m_spellId);

    if (!spell) {
        return;
    }

    uint32_t count;
    const TSGrowableArray<const SpellEffectRec*>* effects = SpellGetEffects(spell->m_id, count);

    for (uint32_t i = 0; i < count; i++) {
        const SpellEffectRec* effectRec = effects->m_data[i];

        if (!effectRec) {
            continue;
        }

        int32_t effect = effectRec->m_effect;

        if (effect == 53 || effect == 54 || effect == 156) {
            if (item->GetItemStaticFlags(0) & 0x2000) {
                return;
            }

            const SpellEffectRec* rec = effects->m_data[i];
            bool matchesSubclass = false;

            if (rec->m_effectMiscValueB) {
                const ItemSubClassRec* subclass = SDBItemSubclassGetSubClassRec(7, rec->m_effectMiscValueB);

                if (subclass && subclass->m_subClassID == static_cast<int32_t>(item->GetSubtypeID()) && subclass->m_classID == static_cast<int32_t>(item->GetClassID())) {
                    matchesSubclass = true;
                }
            }

            const SpellEquippedItemsRec* equipped;

            if (!spell->m_spellEquippedItemsID || !(equipped = g_spellEquippedItemsDB.GetRecord(spell->m_spellEquippedItemsID)) || (equipped->m_equippedItemSubclass && (equipped->m_equippedItemClass != static_cast<int32_t>(item->GetClassID()) || !((1 << item->GetSubtypeID()) & equipped->m_equippedItemSubclass)) && !matchesSubclass) || (equipped->m_equippedItemInvTypes && !((1 << item->GetInventoryType()) & equipped->m_equippedItemInvTypes) && !matchesSubclass)) {
                Spell_C_SpellFailed(cast, spell, 13, -1, -1, 0);
                return;
            }

            SpellItemEnchantmentRec* enchant = g_spellItemEnchantmentDB.GetRecord(effects->m_data[i]->m_effectMiscValue);

            if (enchant && (enchant->m_flags & 1) && !item->IsBound() && !confirmed && item->GetInventoryType()) {
                CGGameUI::m_lastItemSpellTarget = item->m_obj->m_guid;
                FrameScript_SignalEvent(SCRIPT_EVENT_BIND_ENCHANT, NULL);
                return;
            }

            if (item->IsBound() && !item->IsBoundForTrade() && !confirmed) {
                SpellItemEnchantmentRec* boundEnchant;

                if (effect != 54 || ((boundEnchant = g_spellItemEnchantmentDB.GetRecord(effects->m_data[i]->m_effectMiscValue)) && (boundEnchant->m_flags & 1))) {
                    CGGameUI::m_lastItemSpellTarget = item->m_obj->m_guid;
                    FrameScript_SignalEvent(SCRIPT_EVENT_END_BOUND_TRADEABLE, "%s", "itemenchant");
                    return;
                }
            }

            if (enchant && item->GetEnchantmentSpell(0) && !item->HasOnUseEnchant()) {
                for (uint32_t j = 0; j < 3; j++) {
                    if (enchant->m_effect[j] == 7 && enchant->m_effectArg[j]) {
                        Spell_C_SpellFailed(cast, spell, 173, -1, -1, 0);
                        return;
                    }
                }
            }

            int32_t existingId = item->GetEnchantmentId(Spell_C_GetEnchantmentSlot(effect != 53, enchant));

            if (existingId) {
                SpellItemEnchantmentRec* existing = g_spellItemEnchantmentDB.GetRecord(existingId);

                if (existing && enchant) {
                    CGGameUI::m_lastItemSpellTarget = item->m_obj->m_guid;
                    FrameScript_SignalEvent(SCRIPT_EVENT_REPLACE_ENCHANT, "%s%s", static_cast<const char*>(existing->m_name), static_cast<const char*>(enchant->m_name));
                    return;
                }

                continue;
            }
        }

        if (effect == 127) {
            if (!(item->GetItemStaticFlags(0) & 0x40000)) {
                Spell_C_SpellFailed(cast, spell, 18, -1, -1, 0);
                return;
            }
        } else if (effect == 158) {
            if (!(item->GetItemStaticFlags(0) & 0x20000000)) {
                Spell_C_SpellFailed(cast, spell, 17, -1, -1, 0);
                return;
            }
        } else {
            continue;
        }

        WOWGUID none = WOWGUID();
        ItemRecSparse* stats = g_itemSparseDB.GetRecord(item->m_obj->m_entryID, none, NULL, NULL, false);

        if (stats) {
            CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();
            int32_t skillIndex = player->GetSkillIndex(stats->m_requiredSkill);

            if (skillIndex < 0 || static_cast<int32_t>(player->GetSkillValue(skillIndex)) < stats->m_requiredSkillRank) {
                Spell_C_SpellFailed(cast, spell, 153, stats->m_requiredSkill, stats->m_requiredSkillRank, 0);
                return;
            }
        }

        int32_t minPoints;
        int32_t maxPoints;
        Spell_C_GetMinMaxPoints(spell, i, &minPoints, &maxPoints, 0, 0, 0, 0);

        if (item->m_item->m_itemFieldStackCount < minPoints) {
            Spell_C_SpellFailed(cast, spell, 57, item->m_obj->m_entryID, minPoints, 0);
            return;
        }
    }

    Spell_C_HandleSpriteClick(item, NULL);
}

struct ClosestObjectMatchData {
    /* 0000 */ uint32_t m_typeMask;
    /* 0004 */ int32_t m_targetType;
    /* 0008 */ const char* m_name;
    /* 000C */ int32_t m_nameLength;
    /* 0010 */ CGUnit_C* m_unit;
    /* 0018 */ WOWGUID m_bestGuid;
    /* 0020 */ float m_bestDistance;
    /* 0024 */ int32_t m_ignoreHostileTotems;
};

// OFFSET: 0x008C4D30/0x1406349A0
static int32_t FilterClosestMatchCandidate(int32_t targetType, const WOWGUID& guid, CGUnit_C* unit) {
    switch (targetType) {
        case 1:
        case 2:
            return 0;

        case 4: {
            uint32_t high = static_cast<uint32_t>(guid.m_guid >> 32);

            if ((high & 0xF0000000) || !((high & 0xF07FFFFF) | static_cast<uint32_t>(guid.m_guid))) {
                return 0;
            }
        }

        case 3:
            return guid != unit->m_obj->m_guid && CGGameUI::IsRaidMemberOrPet(guid) ? 1 : 0;

        case 5:
            if (guid == unit->m_obj->m_guid) {
                return 0;
            }

            return CGPartyInfo::IsMember(guid) ? 1 : 0;

        case 6:
            return guid != unit->m_obj->m_guid && CGGameUI::IsPartyMember(guid) ? 1 : 0;

        default:
            return 1;
    }
}

// OFFSET: 0x008C4E00/0x1400E9C30
static void UpdateClosestMatch(ClosestObjectMatchData* data, const WOWGUID& guid, const char* name, float distance) {
    if (data->m_bestDistance > distance && name && !SStrCmpUTF8I(name, data->m_name, data->m_nameLength)) {
        data->m_bestGuid = guid;
        data->m_bestDistance = distance;
    }
}

// OFFSET: 0x008D1B40/0x140645BD0
static int32_t ClosestObjectMatchProc(WOWGUID guid, void* param) {
    ClosestObjectMatchData* data = static_cast<ClosestObjectMatchData*>(param);
    CGObject_C* object = ClntObjMgrObjectPtr(guid, TYPEMASK_OBJECT, __FILE__, __LINE__);

    if (!object) {
        return 1;
    }

    if (!(object->m_obj->m_type & data->m_typeMask)) {
        return 1;
    }

    if ((object->m_obj->m_type & TYPEMASK_UNIT) && !UnitMatchesTargetType(data->m_unit, static_cast<CGUnit_C*>(object), data->m_targetType)) {
        return 1;
    }

    float distance = data->m_unit->GetDistanceToPos(object->GetPosition());

    if (guid == data->m_unit->m_obj->m_guid) {
        distance = 2500000000.0f;
    } else if (object->m_obj->m_type & TYPEMASK_UNIT) {
        CGUnit_C* unit = static_cast<CGUnit_C*>(object);
        CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();

        if (data->m_ignoreHostileTotems && unit->GetCreatureType() == 11 && !player->IsFriendly(unit)) {
            return 1;
        }

        if (unit->IsDeadOrGhost() || (unit->m_unit->m_unitDynamicFlags & 0x20) || unit->GetClientStandState() == 7) {
            distance = s_maxMatchDistance + distance;
        }
    }

    char fullName[305];
    const char* name;

    if (object->m_obj->m_type & TYPEMASK_UNIT) {
        const char* realm = NULL;
        name = static_cast<CGUnit_C*>(object)->GetUnitName(&realm, 1);

        if (realm) {
            SStrPrintf(fullName, sizeof(fullName), "%s-%s", name, realm);
            name = fullName;
        }
    } else {
        name = object->GetObjectName();
    }

    UpdateClosestMatch(data, object->m_obj->m_guid, name, distance);
    return 1;
}

// OFFSET: 0x008D1D00/0x140645E00
WOWGUID CGGameUI::ClosestObjectMatch(const char* name, uint32_t typeMask, int32_t targetType, int32_t exact, float maxDistance, int32_t ignoreHostileTotems) {
    CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();

    if (!player) {
        return WOWGUID();
    }

    CGUnit_C* unit = player->GetPossessedUnit();

    if (!unit) {
        unit = player;
    }

    WOWGUID guid = WOWGUID();

    if (Script_GetGUIDFromToken(name, guid, true, false) && guid.m_guid != 0xFFFFFFFFFFFFFFFEULL) {
        if (!guid) {
            return WOWGUID();
        }

        CGObject_C* object = ClntObjMgrObjectPtr(guid, TYPEMASK_OBJECT, __FILE__, __LINE__);

        if (object && !(typeMask & object->m_obj->m_type)) {
            return WOWGUID();
        }

        if (typeMask != TYPEMASK_UNIT && typeMask != TYPEMASK_PLAYER) {
            return guid;
        }

        if (object) {
            return UnitMatchesTargetType(unit, static_cast<CGUnit_C*>(object), targetType) ? guid : WOWGUID();
        }

        return FilterClosestMatchCandidate(targetType, guid, unit) ? guid : WOWGUID();
    }

    ClosestObjectMatchData data;
    data.m_typeMask = typeMask;
    data.m_targetType = targetType;
    data.m_name = name;
    data.m_nameLength = exact ? STORM_MAX_STR : SStrLenUTF8(name);
    data.m_unit = unit;
    data.m_bestGuid = WOWGUID();
    data.m_bestDistance = maxDistance;
    data.m_ignoreHostileTotems = ignoreHostileTotems;

    if (targetType != 1 && targetType != 2) {
        for (uint32_t i = 0; i < 4; i++) {
            WOWGUID member = CGPartyInfo::GetMember(i);

            if (member && !ClntObjMgrObjectPtr(member, TYPEMASK_UNIT, __FILE__, __LINE__)) {
                WOWGUID none = WOWGUID();
                NameCache* record = g_nameDBCache.GetRecord(member.m_guid, none, NULL, NULL, false);

                if (record) {
                    UpdateClosestMatch(&data, member, record->m_name, 2500000000.0f);
                }
            }
        }

        if (targetType != 5) {
            for (uint32_t i = 0; i < CGRaidInfo::m_numMembers; i++) {
                WOWGUID member = CGRaidInfo::m_members[i]->m_guid;

                if (member && !ClntObjMgrObjectPtr(member, TYPEMASK_UNIT, __FILE__, __LINE__)) {
                    WOWGUID none = WOWGUID();
                    NameCache* record = g_nameDBCache.GetRecord(member.m_guid, none, NULL, NULL, false);

                    if (record) {
                        UpdateClosestMatch(&data, member, record->m_name, 2500000000.0f);
                    }
                }
            }
        }
    }

    ClntObjMgrEnumVisibleObjects(ClosestObjectMatchProc, &data);
    return data.m_bestGuid;
}

// OFFSET: 0x008C4E40/0x140634A70
static int QSortCompareNearestUnit(const void* a, const void* b) {
    const NearestUnitData* left = static_cast<const NearestUnitData*>(a);
    const NearestUnitData* right = static_cast<const NearestUnitData*>(b);

    if (!left || !right) {
        SErrDisplayAppFatalMessage("", "", 0);
    }

    if (left->m_inFront != right->m_inFront) {
        return 2 * (left->m_inFront == 0) - 1;
    }

    if (right->m_distanceSq == left->m_distanceSq) {
        return 0;
    }

    return right->m_distanceSq >= left->m_distanceSq ? -1 : 1;
}

// OFFSET: 0x008CA780/0x140634860
int32_t UnitMatchesTargetType(CGUnit_C* unit, CGUnit_C* target, int32_t type) {
    switch (type) {
        case 1: {
            return unit->CanAttack(target) && !target->IsDeadOrGhost() && !(target->m_unit->m_unitDynamicFlags & 0x20) && target->GetClientStandState() != 7;
        }

        case 2: {
            if (!(target->m_obj->m_type & TYPEMASK_PLAYER)) {
                return 0;
            }

            return unit->CanAttack(target) && !target->IsDeadOrGhost() && !(target->m_unit->m_unitDynamicFlags & 0x20) && target->GetClientStandState() != 7;
        }

        case 3: {
            return unit->CanAssist(target, false) && target->m_unit->m_unitFieldHealth > 0;
        }

        case 4: {
            if (!(target->m_obj->m_type & TYPEMASK_PLAYER)) {
                return 0;
            }

            return unit->CanAssist(target, false) && target->m_unit->m_unitFieldHealth > 0;
        }

        case 5: {
            return CGPartyInfo::IsMember(target->m_obj->m_guid) ? 1 : 0;
        }

        case 6: {
            return CGGameUI::IsPartyMember(target->m_obj->m_guid) != 0;
        }

        default: {
            return 1;
        }
    }
}

// OFFSET: 0x008D53D0/0x1406486D0
static int32_t TargetUpdateProc(WOWGUID guid, void* param) {
    CGUnit_C* searcher = static_cast<CGUnit_C*>(param);
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (!unit || unit == searcher || !UnitMatchesTargetType(searcher, unit, s_nearestType)) {
        return 1;
    }

    C3Vector searcherPos = searcher->GetPosition();
    C3Vector unitPos = unit->GetPosition();
    float dx = unitPos.x - searcherPos.x;
    float dy = unitPos.y - searcherPos.y;
    float dz = unitPos.z - searcherPos.z;
    float distanceSq = dx * dx + dy * dy + dz * dz;
    C3Vector targetPos = unit->GetPosition();
    C3Vector fromPos = searcher->GetPosition();
    float facingTo = CalculateFacingTo(&fromPos, &targetPos);
    float delta = fmod(facingTo - searcher->GetFacing() + 12.566371f, 6.2831855f);
    static float s_coneAngle = 0.52359879f;
    int32_t inFront = 0;
    float maxDistanceSq;

    if (s_coneAngle > delta || 6.2831855f - s_coneAngle < delta) {
        maxDistanceSq = 1681.0f;
        inFront = 1;
    } else {
        maxDistanceSq = 100.0f;
    }

    if (maxDistanceSq >= distanceSq) {
        CreatureTypeRec* creatureType = g_creatureTypeDB.GetRecord(unit->GetCreatureType());

        if ((!creatureType || !(creatureType->m_flags & 1)) && World::QueryObjectVisible(unit->m_worldObject) && (!(unit->m_obj->m_type & TYPEMASK_PLAYER) || !static_cast<CGPlayer_C*>(unit)->IsCommentatorUberOrInArena())) {
            NearestUnitData* data = s_nearestUnits.New();
            data->m_distanceSq = distanceSq;
            data->m_guid = guid;
            data->m_inFront = inFront;
        }
    }

    return 1;
}

// OFFSET: 0x008D6750/0x140648920
void TargetNearestEnemy(int32_t reverse, int32_t type) {
    while (true) {
        if (lua_tainted) {
            CGGameUI::ShowBlockedActionFeedback(NULL, 0);
            return;
        }

        CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();

        if (!player) {
            return;
        }

        CGUnit_C* searcher = player->GetPossessedUnit();

        if (!searcher) {
            searcher = player;
        }

        uint32_t count;

        if (!s_nearestSortTime || !s_nearestTargetTime || s_nearestTargetTime + 3000 <= FrameTime::GetCurTimeMs() || !(count = s_nearestUnits.m_count) || s_nearestType != type) {
            s_nearestIndex = 0;
            s_nearestSortTime = 0;
            s_nearestTargetTime = 0;
            s_nearestType = type;
            s_nearestUnits.SetCount(0);
            ClntObjMgrEnumVisibleObjects(TargetUpdateProc, searcher);

            if (!s_nearestUnits.m_count) {
                if (player->GetTransportGUID() == searcher->m_obj->m_guid.m_guid) {
                    VehicleSeatRec* seat = player->m_vehiclePassenger ? player->m_vehiclePassenger->m_seatRec : NULL;

                    if (seat && (seat->m_flags & 0x4000)) {
                        searcher = player;
                        ClntObjMgrEnumVisibleObjects(TargetUpdateProc, player);
                    }
                }

                if (!s_nearestUnits.m_count) {
                    return;
                }
            }

            qsort(s_nearestUnits.m_data, s_nearestUnits.m_count, sizeof(NearestUnitData), QSortCompareNearestUnit);
            s_nearestSortTime = FrameTime::GetCurTimeMs();

            if (!s_nearestSortTime) {
                s_nearestSortTime = 1;
            }

            count = s_nearestUnits.m_count;
        } else if (CGGameUI::m_lockedTarget == s_nearestUnits.m_data[s_nearestIndex].m_guid) {
            if (reverse) {
                if (s_nearestIndex) {
                    s_nearestIndex--;
                } else {
                    s_nearestIndex = s_nearestUnits.m_count - 1;
                }
            } else if (++s_nearestIndex >= s_nearestUnits.m_count) {
                s_nearestIndex = 0;

                if (s_nearestSortTime + 1000 <= FrameTime::GetCurTimeMs()) {
                    s_nearestSortTime = 0;
                    continue;
                }

                count = s_nearestUnits.m_count;
            }
        }

        if (!count) {
            return;
        }

        s_nearestTargetTime = FrameTime::GetCurTimeMs();
        uint32_t i = s_nearestIndex;
        CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(s_nearestUnits.m_data[i].m_guid, TYPEMASK_UNIT, __FILE__, __LINE__));
        bool restart = false;

        while (true) {
            if (unit && UnitMatchesTargetType(searcher, unit, type)) {
                s_nearestIndex = i;
                CGGameUI::Target(s_nearestUnits.m_data[i].m_guid);
                return;
            }

            if (++i >= s_nearestUnits.m_count) {
                i = 0;

                if (s_nearestSortTime + 1000 <= FrameTime::GetCurTimeMs()) {
                    s_nearestSortTime = 0;
                    restart = true;
                    break;
                }
            }

            if (i == s_nearestIndex) {
                break;
            }

            unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(s_nearestUnits.m_data[i].m_guid, TYPEMASK_UNIT, __FILE__, __LINE__));
        }

        if (restart) {
            continue;
        }

        s_nearestUnits.SetCount(0);
        return;
    }
}

// OFFSET: 0x008C5000
void CGGameUI::ShowCombatFeedbackAttackRoundInfo(const ATTACKROUNDINFO* info) {
    if (!info || info->m_victimState >= 9) {
        SErrDisplayAppFatalMessage("", "", 0);
    }

    CGGameUI::SignalUnitCombat(info->m_target, s_victimStateTokens[info->m_victimState], info->m_damage, info->m_schoolMask[0], info->m_hitInfo);
}

// OFFSET: 0x008D6660
void CGGameUI::BreakTarget(WOWGUID guid) {
    if (guid == CGGameUI::m_lockedTarget) {
        Spell_C_CancelAutoRepeat(true);
    }

    CGGameUI::ClearTarget(guid, 1);

    if (guid == CGGameUI::m_lastTarget) {
        CGGameUI::m_lastTarget = WOWGUID();
    }

    if (guid == CGGameUI::m_lastEnemyTarget) {
        CGGameUI::m_lastEnemyTarget = WOWGUID();
    }

    if (guid == CGGameUI::m_lastFriendTarget) {
        CGGameUI::m_lastFriendTarget = WOWGUID();
    }

    if (guid == CGGameUI::m_currentObjectTrack && CGWorldFrame::s_currentWorldFrame) {
        CGWorldFrame::s_currentWorldFrame->HandleSpriteTrack(WOWGUID());
    }

    if (guid == CGGameUI::m_focusTarget) {
        CGGameUI::SetFocusTarget(WOWGUID());
    }

    if (guid == CGGameUI::m_directionalTarget) {
        CGGameUI::m_directionalTarget = WOWGUID();
    }
}

// OFFSET: 0x008C4670
void CGGameUI::UnitTooltipUpdate(const WOWGUID& guid) {
    if (CGGameUI::m_gameTooltip && CGGameUI::m_gameTooltip->m_unitGuid == guid && !CGGameUI::m_gameTooltip->m_fading) {
        WOWGUID none = WOWGUID();
        CGGameUI::m_gameTooltip->SetUnit(guid, 1, none, 0);
    }
}

// OFFSET: 0x008CB370/0x14063BC80
int32_t CGGameUI::IsDynamicDifficultyMap() {
    MapRec* map = g_mapDB.GetRecord(CGGameUI::m_iCurrentMapID);

    if (map && (map->m_flags & 0x100)) {
        return 1;
    }

    return 0;
}

// OFFSET: #0x00519680
uint32_t CGGameUI::GetEffectiveRaidDifficulty() {
    MapRec* map = g_mapDB.GetRecord(CGGameUI::m_iCurrentMapID);

    if (map && (map->m_flags & 0x100)) {
        uint32_t difficulty = CGRaidInfo::m_numMembers ? CGPartyInfo::m_raidDifficulty : CGGameUI::m_raidDifficulty;
        return (difficulty & 1) + 2 * CGPartyInfo::m_dynamicDifficulty;
    }

    return CGRaidInfo::m_numMembers ? CGPartyInfo::m_raidDifficulty : CGGameUI::m_raidDifficulty;
}

// OFFSET: 0x008CB3B0
void CGGameUI::DoPowerRegen() {
    uint32_t now = FrameTime::GetCurTimeMs();

    if (!CGGameUI::m_lastRegenTime) {
        CGGameUI::m_lastRegenTime = now;
        return;
    }

    uint32_t elapsed = now - CGGameUI::m_lastRegenTime;

    if (elapsed < 100) {
        return;
    }

    CGGameUI::m_lastRegenTime = now;

    if (!g_predictedPowerCVar->m_intValue) {
        return;
    }

    CGUnit_C* player = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_UNIT, __FILE__, __LINE__));

    if (player) {
        player->DoPowerRegen(elapsed);
    }

    CGUnit_C* pet = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(CGPetInfo::GetPet(0), TYPEMASK_UNIT, __FILE__, __LINE__));

    if (pet) {
        pet->DoPowerRegen(elapsed);
    }

    CGUnit_C* target = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(CGGameUI::m_lockedTarget, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (target && target->m_creatureCacheEntry && (target->m_creatureCacheEntry->m_typeFlags[1] & 1)) {
        target->DoPowerRegen(elapsed);
    }

    if (CGGameUI::m_focusTarget && CGGameUI::m_lockedTarget != CGGameUI::m_focusTarget) {
        CGUnit_C* focus = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(CGGameUI::m_focusTarget, TYPEMASK_UNIT, __FILE__, __LINE__));

        if (focus && focus->m_creatureCacheEntry && (focus->m_creatureCacheEntry->m_typeFlags[1] & 1)) {
            focus->DoPowerRegen(elapsed);
        }
    }
}

// OFFSET: 0x008C45F0/0x140634180
int32_t CGGameUI::IsRaidMemberOrPet(const WOWGUID& guid) {
    if (CGPartyInfo::IsMemberOrPet(guid)) {
        return 1;
    }

    if (CGRaidInfo::IsPlayerOrPetInRaid(guid)) {
        return 1;
    }

    return 0;
}

// OFFSET: 0x008D7BB0/0x14064AFC0
int32_t CGGameUI::StartCinematicCamera() {
    CGCamera* camera = CGWorldFrame::GetActiveCamera();
    CinematicCameraRec* record = CGGameUI::m_cinematic.m_camera;

    if (!record) {
        return 0;
    }

    C3Vector origin;
    origin.x = record->m_origin[0];
    origin.y = record->m_origin[1];
    origin.z = record->m_origin[2];

    if (!camera->SetModelCamera(record->m_model, &origin, record->m_originFacing, &CGGameUI::NextCinematic, 0)) {
        return 0;
    }

    C3Vector position(0.0f, 0.0f, 0.0f);
    CameraGetCoord(static_cast<CCamera*>(camera->m_model->GetCameraByIndex(0)), 7, &position);
    const C3Vector& shot = StaticSingleton<CCameraManager>::Instance()->m_shot.m_position;

    if ((shot.z - position.z) * (shot.z - position.z) + (shot.x - position.x) * (shot.x - position.x) + (shot.y - position.y) * (shot.y - position.y) > 900.0f) {
        World::Preload(position, 0);
    }

    CGObject_C::UpdateAllWorldObjects();
    ScrnLayerPaint();

    if (!SFile::IsStreamingMode()) {
        AsyncFileReadWaitAll();
    }

    camera->ResetModelCamera();
    WDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_NEXT_CINEMATIC_CAMERA));
    msg.Finalize();
    ClientServices::Send2(&msg);
    DisableFadingScreen(0.25f, NULL, NULL);
    return 1;
}

// OFFSET: 0x008D4AA0
void CGGameUI::NextCinematicInternal(void* param) {
    CGGameUI::m_cinematic.m_camera = NULL;
    SE3::StopSound(CGGameUI::m_cinematic.m_cameraMusic, 0, __FILE__, __LINE__, "CGGameUI::NextCinematicInternal");

    if (CGGameUI::m_cinematic.m_sequence && ++CGGameUI::m_cinematic.m_cameraIndex < 8) {
        int32_t camera = CGGameUI::m_cinematic.m_sequence->m_camera[CGGameUI::m_cinematic.m_cameraIndex];

        if (camera) {
            CGGameUI::m_cinematic.m_camera = g_cinematicCameraDB.GetRecord(camera);
        }
    }

    if (!CGGameUI::StartCinematicSound() || !CGGameUI::StartCinematicCamera()) {
        CGGameUI::StopCinematicInternal(NULL);
    }
}

// OFFSET: 0x008D4B30/0x140641770
void CGGameUI::CloseLoot(bool a1, bool a2, bool a3) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));
    WOWGUID guid = CGLootInfo::m_lootObj;

    if (!guid) {
        return;
    }

    if ((CGLootInfo::GetLootType() == 4 || CGLootInfo::GetLootType() == 7 || CGLootInfo::GetLootType() == 8) && CGLootInfo::HasLoot() && !a2) {
        CGLootInfo::DoAutoLoot(1);
    }

    if (a3) {
        CGGameUI::DisplayError(ERR_LOOT_NO_UI);
    }

    CGObject_C* object = ClntObjMgrObjectPtr(guid, TYPEMASK_OBJECT, __FILE__, __LINE__);

    if (object && (object->m_obj->m_type & TYPEMASK_ITEM)) {
        if (a2) {
            return;
        }

        CGGameUI::UnlockItem(guid);
    }

    if (player) {
        player->ClearLootingObject();
    }

    if (a1) {
        CDataStore msg;
        msg.Put(static_cast<uint32_t>(CMSG_LOOT_RELEASE));
        msg.Put(guid.m_guid);
        msg.Finalize();
        ClientServices::Send2(&msg);
    }

    CGLootInfo::SetObject(NULL, NULL, 0);

    if (object && (object->m_obj->m_type & TYPEMASK_UNIT) && static_cast<int32_t>(static_cast<CGUnit_C*>(object)->m_unit->m_unitFieldHealth) <= 0) {
        CGGameUI::ClearTarget(guid, 1);
    }

    if (CGGameUI::m_cursorItemType == 6) {
        CGGameUI::ClearCursor(1, 1);
    }
}

// OFFSET: 0x008D5800/0x140646120
void CGGameUI::SetRegenEnabled(int32_t enabled) {
    if (enabled) {
        CGGameUI::m_simpleTop->m_allowProtectedActions = 1;
        FrameScript_SignalEvent(SCRIPT_EVENT_PLAYER_REGEN_ENABLED, NULL);
    } else {
        FrameScript_SignalEvent(SCRIPT_EVENT_PLAYER_REGEN_DISABLED, NULL);
        CGGameUI::m_simpleTop->m_allowProtectedActions = 0;
    }

    CGGameUI::UpdateCombatMode();
}

// OFFSET: 0x008C5240/0x140634EC0
void CGGameUI::LockItem(WOWGUID guid) {
    CGItem_C* item = static_cast<CGItem_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_ITEM, __FILE__, __LINE__));

    if (item) {
        item->Lock();
    }

    CGActionBar::UpdateUsable();
}

// OFFSET: 0x008D2A30
void CGGameUI::UnlockAllItems() {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player || !player->GetBag()) {
        return;
    }

    CGBag_C* bag = player->GetBag();
    uint32_t numSlots = bag->m_numSlots;

    for (uint32_t i = 0; i < numSlots; i++) {
        WOWGUID guid = i < static_cast<uint32_t>(bag->m_numSlots) ? bag->m_slots[i] : WOWGUID();
        CGItem_C* item = static_cast<CGItem_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_ITEM, __FILE__, __LINE__));

        if (!item) {
            continue;
        }

        item->Unlock();

        if (!(item->m_obj->m_type & TYPEMASK_CONTAINER) || !item->GetBag()) {
            continue;
        }

        for (uint32_t j = 0; j < static_cast<uint32_t>(item->GetBag()->m_numSlots); j++) {
            CGBag_C* container = item->GetBag();
            WOWGUID contained = i < static_cast<uint32_t>(container->m_numSlots) ? container->m_slots[i] : WOWGUID();
            CGItem_C* containedItem = static_cast<CGItem_C*>(ClntObjMgrObjectPtr(contained, TYPEMASK_ITEM, __FILE__, __LINE__));

            if (containedItem) {
                containedItem->Unlock();
            }
        }
    }
}

// OFFSET: 0x009BB490
void CGGameUI::SetPendingItemSound(WOWGUID guid, int32_t itemId) {
    CGGameUI::m_autoStoreItem = guid;
    CGGameUI::m_autoStoreItemID = itemId;
}

// OFFSET: 0x008C5BF0/0x140635940
PartyMemberState* CGGameUI::GetPartyMemberStateFromPet(const WOWGUID& petGuid) {
    PartyMemberState* state = NULL;

    if (petGuid) {
        state = CGPartyInfo::GetRemoteStatsFromPet(petGuid);

        if (!state) {
            state = CGRaidInfo::GetRemoteStatsFromPet(petGuid);

            if (!state) {
                return CGBattlefieldInfo::GetRemoteStatsFromPet(petGuid);
            }
        }
    }

    return state;
}

// OFFSET: 0x008C51F0/0x140634E60
void CGGameUI::GetCursorVirtualItem(int32_t& id, int32_t& slot) {
    id = CGGameUI::m_cursorVirtualID;
    slot = CGGameUI::m_cursorVirtualSlot;
}

// OFFSET: 0x008C52A0/0x140634F40
int32_t CGGameUI::VirtualItemIsLocked(int32_t id, uint32_t virtualSlot, int32_t a3) {
    if (!id || a3) {
        return 0;
    }

    return CGGuildBankInfo::IsLocked(id, virtualSlot);
}

// OFFSET: 0x008C52D0/0x140634F60
void CGGameUI::LockVirtualItem(int32_t id, uint32_t virtualSlot, int32_t a3) {
    if (!id || a3) {
        return;
    }

    CGGuildBankInfo::Lock(id, virtualSlot);
    FrameScript_SignalEvent(SCRIPT_EVENT_GUILDBANK_ITEM_LOCK_CHANGED, NULL);
}

// OFFSET: 0x008C5300
void CGGameUI::UnlockVirtualItem(int32_t id, uint32_t virtualSlot, int32_t a3) {
    if (id && !a3) {
        CGGuildBankInfo::Unlock(id, virtualSlot);
        FrameScript_SignalEvent(SCRIPT_EVENT_GUILDBANK_ITEM_LOCK_CHANGED, NULL);
    }
}

// OFFSET: 0x008C5330
int32_t CGGameUI::GetVirtualItemCount(int32_t id, uint32_t virtualSlot, int32_t a3) {
    if (!id || a3) {
        return 0;
    }

    uint32_t tab = 0;
    uint32_t slot = 0;
    CGGuildBankInfo::GetTabAndSlotFromVirtual(virtualSlot, tab, slot);
    return CGGuildBankInfo::GetGuildBankItem(tab, slot)->m_count;
}

// OFFSET: 0x008D21B0/0x140646270
void CGGameUI::SetCursorItem(WOWGUID item, WOWGUID container, uint32_t slot, int32_t a4, int32_t stackSplit, int32_t playSound) {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player || (!CGGameUI::m_hasControl && !(player->m_unit->m_unitFieldFlags & 0x100000))) {
        return;
    }

    CGGameUI::ClearCursor(a4, 1);

    if (!item) {
        return;
    }

    CGGameUI::m_cursorItemContainer = container;
    CGGameUI::m_cursorItem = item;
    CGGameUI::m_cursorItemSlot = slot;
    CGGameUI::m_stackSplit = stackSplit;
    CGGameUI::m_cursorItemType = 1;
    CursorSetHeldItem(item);
    CGItem_C* itemObject = static_cast<CGItem_C*>(ClntObjMgrObjectPtr(item, TYPEMASK_ITEM, __FILE__, __LINE__));

    if (itemObject) {
        if (itemObject->CanBeUsed() || itemObject->GetInventoryType()) {
            CGActionBar::ShowGrid();
            CGGameUI::m_cursorHasAction = 1;
        }

        if (playSound) {
            SI3::PlayItemSound(0, itemObject);
        }
    }

    FrameScript_SignalEvent(SCRIPT_EVENT_CURSOR_UPDATE, NULL);
}

// OFFSET: 0x008D2E20/0x14063C1B0
void CGGameUI::DisplayNameError(uint32_t error) {
    switch (error) {
        case 2:
            CGGameUI::DisplayError(ERR_NAME_NO_NAME);
            break;

        case 3:
            CGGameUI::DisplayError(ERR_NAME_TOO_SHORT);
            break;

        case 4:
            CGGameUI::DisplayError(ERR_NAME_TOO_LONG);
            break;

        case 6:
            CGGameUI::DisplayError(ERR_NAME_MIXED_LANGUAGES);
            break;

        case 7:
            CGGameUI::DisplayError(ERR_NAME_PROFANE);
            break;

        case 8:
            CGGameUI::DisplayError(ERR_NAME_RESERVED);
            break;

        case 11:
            CGGameUI::DisplayError(ERR_NAME_THREE_CONSECUTIVE);
            break;

        case 12:
            CGGameUI::DisplayError(ERR_NAME_INVALID_SPACE);
            break;

        case 13:
            CGGameUI::DisplayError(ERR_NAME_CONSECUTIVE_SPACES);
            break;

        case 14:
            CGGameUI::DisplayError(ERR_NAME_RUSSIAN_CONSECUTIVE_SILENT_CHARACTERS);
            break;

        case 15:
            CGGameUI::DisplayError(ERR_NAME_RUSSIAN_SILENT_CHARACTER_AT_BEGINNING_OR_END);
            break;

        case 16:
            CGGameUI::DisplayError(ERR_NAME_DECLENSION_DOESNT_MATCH_BASE_NAME);
            break;

        default:
            CGGameUI::DisplayError(ERR_NAME_INVALID);
            break;
    }
}

// OFFSET: 0x008CBAE0/0x14063C7B0
void CGGameUI::AcceptLevelGrant() {
    if (!CGGameUI::m_pendingLevelGranter) {
        return;
    }

    WDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_ACCEPT_LEVEL_GRANT));
    SmartGUID granter;
    granter.m_guid = CGGameUI::m_pendingLevelGranter.m_guid;
    msg << granter;
    msg.Finalize();
    ClientServices::Send2(&msg);
    CGGameUI::m_pendingLevelGranter = WOWGUID();
}

// OFFSET: 0x008D30B0
void CGGameUI::UpdateThreatWarningFromCVar() {
    CGGameUI::UpdateThreatWarningEnabled(g_threatWarningCVar->m_intValue);
}

// OFFSET: 0x008D3120/0x14063C9B0
void CGGameUI::GetSelectionHighlightColor(const WOWGUID& guid, CImVector* color, int32_t a3) {
    // STATIC: #0x00BD0C94
    static CImVector s_reactionColors[8] = { CImVector(0x00, 0x00, 0xFF, 0xFF), CImVector(0x00, 0x00, 0xFF, 0xFF), CImVector(0x00, 0x80, 0xFF, 0xFF), CImVector(0x00, 0xFF, 0xFF, 0xFF), CImVector(0x00, 0xFF, 0x00, 0xFF), CImVector(0x00, 0xFF, 0x00, 0xFF), CImVector(0x00, 0xFF, 0x00, 0xFF), CImVector(0x00, 0xFF, 0x00, 0xFF) };

    // STATIC: #0x00BD0C8C
    static CImVector s_playerColors[2] = { CImVector(0xFF, 0x00, 0x00, 0xFF), CImVector(0xFF, 0x60, 0x60, 0xFF) };

    // STATIC: 0x00ED79AC
    static CImVector s_partyColor = CImVector(0xFF, 0xAA, 0xAA, 0xFF);

    // STATIC: 0x00ED79A8
    static CImVector s_partyPvpColor = CImVector(0xAA, 0xFF, 0xAA, 0xFF);

    // STATIC: 0x00ED79A4
    static CImVector s_friendColor = CImVector(0xFF, 0xC8, 0x53, 0xFF);

    if (!color) {
        SErrSetLastError(STORM_ERROR_INVALID_PARAMETER);
        return;
    }

    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (!unit) {
        bool pvp = false;

        if (guid) {
            PartyMemberState* state = CGPartyInfo::GetRemoteStats(guid);

            if (state || (state = CGRaidInfo::GetRemoteStats(guid)) || (state = CGBattlefieldInfo::GetRemoteStats(guid))) {
                pvp = (reinterpret_cast<const uint8_t*>(state)[24] & 2) != 0;
            }
        }

        if (a3) {
            if (CGPartyInfo::IsMemberOrPet(guid) && CGPartyInfo::m_realNumMembers) {
                *color = pvp ? s_partyPvpColor : s_partyColor;
                return;
            }

            if (FriendList::s_friendList->GetFriend(guid)) {
                *color = s_friendColor;
                return;
            }
        }

        *color = pvp ? s_reactionColors[4] : s_playerColors[a3];
        return;
    }

    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));
    CGUnit_C* viewer = NULL;

    if (player) {
        viewer = player->GetPossessedUnit();

        if (!viewer) {
            viewer = player;
        }

        VehicleSeatRec* seat = viewer->m_vehiclePassenger ? viewer->m_vehiclePassenger->m_seatRec : NULL;

        if (seat && (seat->m_flags & 0x100000)) {
            CGUnit_C* vehicle = viewer->GetVehicleUnit();

            if (vehicle) {
                viewer = vehicle;
            }
        }
    }

    if (!viewer) {
        SErrDisplayAppFatalMessage("", "", 0);
    }

    if (unit->m_unit->m_unitFieldFlags & 0x8) {
        if (unit->CanAttack(viewer)) {
            *color = viewer->CanAttack(unit) ? s_reactionColors[1] : s_playerColors[a3];
            return;
        }

        if (viewer->CanAttack(unit)) {
            *color = s_reactionColors[3];
            return;
        }

        uint8_t pvpFlags = unit->m_unit->m_unitFieldBytes2[1];
        bool pvp = (pvpFlags & 1) && !(pvpFlags & 8) && !(viewer->m_unit->m_unitFieldBytes2[1] & 8) && CGBattlefieldInfo::m_instanceType != 4;

        if (a3) {
            if (CGPartyInfo::IsMemberOrPet(guid) && CGPartyInfo::m_realNumMembers) {
                *color = pvp ? s_partyPvpColor : s_partyColor;
                return;
            }

            if (FriendList::s_friendList->GetFriend(guid)) {
                *color = s_friendColor;
                return;
            }
        }

        *color = pvp ? s_reactionColors[unit->UnitReaction(viewer)] : s_playerColors[a3];
    } else if (a3 && (unit->m_unit->m_unitFieldHealth <= 0 || (unit->m_unit->m_unitDynamicFlags & 0x20))) {
        *color = s_deadSelectionColor;
    } else {
        *color = s_reactionColors[unit->UnitReaction(viewer)];
    }
}

// OFFSET: 0x008CA1E0/0x14063A530
void CGGameUI::UnitNameUpdate(const WOWGUID& guid) {
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (unit) {
        CGGameUI::UnitTooltipUpdate(guid);
        unit->UpdateUnitNameText();
    }

    Script_SendUnitSignal(guid, SCRIPT_EVENT_UNIT_NAME_UPDATE);
}

// OFFSET: 0x008C4060
int32_t CGGameUI::CameraMusicIsPlaying() {
    return SE3::IsPlaying(CGGameUI::m_cinematic.m_cameraMusic);
}

// OFFSET: 0x008C98A0/0x140639B00
static void PlaceFrame(CSimpleFrame* frame, int32_t level, FRAMEPOINT point, int32_t x, int32_t y, int32_t width, int32_t height) {
    if (!frame) {
        return;
    }

    if (level >= 0) {
        frame->SetFrameLevel(level, 1);
    }

    if (frame->m_frameFlags & 0x100) {
        frame->MarkFramePointsUnused();
        frame->SetPoint(point, frame->m_simpleTop, point, NDCToDDCWidth(x / (CoordinateGetAspectCompensation() * 1024.0f)), NDCToDDCWidth(y / (CoordinateGetAspectCompensation() * 1024.0f)), 1);
        frame->SetFrameFlag(0x1000, 1);
    }

    if (frame->m_frameFlags & 0x200) {
        frame->SetWidth(NDCToDDCWidth(width / (CoordinateGetAspectCompensation() * 1024.0f)));
        frame->SetHeight(NDCToDDCWidth(height / (CoordinateGetAspectCompensation() * 1024.0f)));
        frame->SetFrameFlag(0x1000, 1);
    }

    frame->Resize(1);
}

// OFFSET: 0x008C99C0/0x140639CC0
void LoadPlacedFrames(HOSFILE localFile, HOSFILE cacheFile, void* param) {
    if (localFile == reinterpret_cast<HOSFILE>(-1)) {
        return;
    }

    uint32_t size = static_cast<uint32_t>(OsGetFileSize(localFile));
    char* buffer = static_cast<char*>(alloca(size + 1));
    uint32_t bytesRead;
    OsReadFile(localFile, buffer, size, &bytesRead);

    if (bytesRead != size) {
        SErrDisplayAppFatalMessage("", "", 0);
    }

    buffer[size] = '\0';

    const char* cursor = buffer;
    int32_t y = 0;
    int32_t x = 0;
    CSimpleFrame* frame = NULL;
    int32_t level = -1;
    FRAMEPOINT point = FRAMEPOINT_TOPLEFT;
    int32_t width = 0;
    int32_t height = 0;
    char line[128];

    do {
        SStrTokenize(&cursor, line, sizeof(line), "\r\n", NULL);

        if (!SStrCmpI(line, "Version: ", SStrLen("Version: "))) {
            SStrToInt(&line[SStrLen("Version: ")]);
        } else if (!SStrCmpI(line, "Frame: ", SStrLen("Frame: "))) {
            PlaceFrame(frame, level, point, x, y, width, height);
            width = 0;
            x = 0;
            frame = static_cast<CSimpleFrame*>(CScriptObject::GetScriptObjectByName(&line[SStrLen("Frame: ")], CSimpleFrame::GetObjectType()));
            level = -1;
            point = FRAMEPOINT_TOPLEFT;
            y = 0;
        } else if (!SStrCmpI(line, "FrameLevel: ", SStrLen("FrameLevel: "))) {
            level = SStrToInt(&line[SStrLen("FrameLevel: ")]);
        } else if (!SStrCmpI(line, "Anchor: ", SStrLen("Anchor: "))) {
            StringToFramePoint(&line[SStrLen("Anchor: ")], point);
        } else if (!SStrCmpI(line, "X: ", SStrLen("X: "))) {
            x = SStrToInt(&line[SStrLen("X: ")]);
        } else if (!SStrCmpI(line, "Y: ", SStrLen("Y: "))) {
            y = SStrToInt(&line[SStrLen("Y: ")]);
        } else if (!SStrCmpI(line, "W: ", SStrLen("W: "))) {
            width = SStrToInt(&line[SStrLen("W: ")]);
        } else if (!SStrCmpI(line, "H: ", SStrLen("H: "))) {
            height = SStrToInt(&line[SStrLen("H: ")]);
        }
    } while (line[0] && *cursor);

    PlaceFrame(frame, level, point, x, y, width, height);
}

// OFFSET: 0x008C9DA0/0x14063A0D0
void CGGameUI::StopMovie() {
    if (!CGGameUI::m_inMovie) {
        return;
    }

    CGGameUI::m_inMovie = 0;

    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_COMPLETE_MOVIE));
    msg.Finalize();
    ClientServices::Send2(&msg);

    CGInputControl* input = CGInputControl::GetActive();
    input->m_mouseModeFlags &= ~0x40u;
    input->UpdateMouseMode(0);
    CGInputControl::GetActive()->EnterWorld();
}

// OFFSET: 0x008CA0E0
static void AccountVariablesLoadedCallback(HOSFILE localFile, HOSFILE cacheFile, void* param) {
    uint32_t value = static_cast<uint32_t>(reinterpret_cast<uintptr_t>(param));

    if ((value >> 16) != CGGameUI::m_accountDataGeneration) {
        return;
    }

    if (cacheFile != reinterpret_cast<HOSFILE>(-1)) {
        ConsoleLoadClientCVar(cacheFile);
    }

    s_accountDataPending &= ~static_cast<uint16_t>(value);

    CGCamera* camera = CGWorldFrame::GetActiveCamera();

    if (!camera) {
        return;
    }

    if (static_cast<uint16_t>(value) == 0x20) {
        camera->LoadCameraSettings();
    }

    if (!s_accountDataPending) {
        camera->InitializeView();
        CGMinimapFrame::LoadTrackingState();
        CGAchievementInfo::LoadTrackedAchievements();
        CGReputationInfo::LoadFactions();
        CGTutorial::LoadFlaggedTutorials();
        CGQuestLog::LoadTrackedQuests(true);
        FrameScript_SignalEvent(SCRIPT_EVENT_VARIABLES_LOADED, NULL);
    }
}

// OFFSET: #0x00518C80
void LoadAccountVariables() {
    s_accountDataPending = 0x30;

    if (!CGGameUI::m_accountDataGeneration) {
        SErrDisplayAppFatalMessage("", "", 0);
    }

    LoadAccountData(0, AccountVariablesLoadedCallback, reinterpret_cast<void*>(static_cast<uintptr_t>((CGGameUI::m_accountDataGeneration << 16) | 0x10)));
    LoadAccountData(1, AccountVariablesLoadedCallback, reinterpret_cast<void*>(static_cast<uintptr_t>((CGGameUI::m_accountDataGeneration << 16) | 0x20)));
}

// OFFSET: 0x008D5C50/0x140649430
void CCapturePointUIManager::Update() {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));
    uint32_t time = FrameTime::GetCurTimeMs();

    if (!player || static_cast<int32_t>(time - this->m_nextWarningTime) < 0) {
        return;
    }

    if ((player->m_unit->m_unitFieldBytes2[1] & 0x1) || (player->m_unit->m_unitFieldFlags & 0x100000)) {
        return;
    }

    for (CapturePointUIManagerNode* node = this->m_nodes.Head(); node; node = this->m_nodes.Next(node)) {
        CGObject_C* object = ClntObjMgrObjectPtr(node->m_guid, TYPEMASK_GAMEOBJECT, __FILE__, __LINE__);

        if (object && static_cast<float>(node->m_radiusSquared) > player->GetDistanceToPos(object->GetPosition())) {
            CGChat::AddChatMessage(FrameScript_GetText("PVP_REQUIRED_FOR_CAPTURE", -1, GENDER_NOT_APPLICABLE), static_cast<SLASH_COMMAND_ID>(0), NULL);
            this->m_nextWarningTime = time + 300000;
            return;
        }
    }
}

struct AutoMouseOverData {
    /* 0000 */ CGPlayer_C* m_player;
    /* 0004 */ CGUnit_C* m_mover;
    /* 0008 */ C3Vector m_facing;
    /* 0014 */ uint32_t m_time;
    /* 0018 */ WOWGUID m_bestGuid;
    /* 0020 */ float m_bestDot;
    /* 0024 */ float m_bestDistSq;
};

// OFFSET: 0x008CBC50
static int32_t AutoMouseOverProc(WOWGUID guid, void* param) {
    CGObject_C* object = ClntObjMgrObjectPtr(guid, TYPEMASK_OBJECT, __FILE__, __LINE__);

    if (!object) {
        return 1;
    }

    if (!param) {
        SErrDisplayAppFatalMessage("", "", 0);
    }

    AutoMouseOverData* data = static_cast<AutoMouseOverData*>(param);
    CGPlayer_C* player = data->m_player;

    if (object == data->m_mover) {
        return 1;
    }

    C3Vector moverPos = data->m_mover->GetPosition();
    C3Vector objectPos = object->GetPosition();
    C3Vector dir = objectPos - moverPos;
    float distSq = dir.x * dir.x + dir.y * dir.y + dir.z * dir.z;
    dir.NormalizeSafe();
    float dot = data->m_facing.z * dir.z + data->m_facing.y * dir.y + data->m_facing.x * dir.x;

    if (dot < data->m_bestDot) {
        return 1;
    }

    if (fequal(dot, data->m_bestDot) && data->m_bestDistSq < distSq) {
        return 1;
    }

    uint32_t type = object->m_obj->m_type;

    if (type & TYPEMASK_UNIT) {
        CGUnit_C* unit = static_cast<CGUnit_C*>(object);
        float rangeSq;

        if (player->CanInteractNow(unit) && unit->m_unit->m_unitNpcFlags) {
            rangeSq = (unit->m_unit->m_unitFieldCombatreach + 4.0f) * (unit->m_unit->m_unitFieldCombatreach + 4.0f);
        } else if (unit->CanBeLooted(data->m_time)) {
            if (!player->CanLoot(unit) && player->m_lootTarget != unit->m_obj->m_guid) {
                return 1;
            }

            rangeSq = distSq;
        } else if ((unit->m_unit->m_unitFieldFlags & 0x4000000) && CGSpellBook::GetSkinningSpell(unit) && (!(unit->m_obj->m_type & TYPEMASK_PLAYER) || !player->CanAssist(unit, false))) {
            float minRange;
            float maxRange;
            Spell_C_GetMinMaxRange(player, CGSpellBook::GetSkinningSpell(unit), &minRange, &maxRange, unit);
            rangeSq = maxRange * maxRange;
        } else {
            if (!player->CanAttackNow(unit) || (player->m_unit->m_unitFieldFlags & 0x20000) || !CGGameUI::m_hasControl) {
                return 1;
            }

            rangeSq = 109.2025f;
        }

        if (rangeSq < distSq) {
            return 1;
        }
    } else if (type & TYPEMASK_GAMEOBJECT) {
        CGGameObject_C* gameObject = static_cast<CGGameObject_C*>(object);

        if (player != data->m_mover || !gameObject->CanUse() || !gameObject->CanUseNow()) {
            return 1;
        }
    } else if (type & TYPEMASK_CORPSE) {
        if (player != data->m_mover) {
            return 1;
        }

        CGCorpse_C* corpse = static_cast<CGCorpse_C*>(object);

        if (corpse->CanBeLooted()) {
            if (!player->CanLoot(corpse) && player->m_lootTarget != corpse->m_obj->m_guid) {
                return 1;
            }
        } else {
            if (!(corpse->m_corpse->m_corpseFieldFlags & 0x20) || !CGSpellBook::GetSkinningSpell(reinterpret_cast<CGUnit_C*>(corpse)) || player->CanAssist(corpse)) {
                return 1;
            }

            float minRange;
            float maxRange;
            Spell_C_GetMinMaxRange(player, CGSpellBook::GetSkinningSpell(reinterpret_cast<CGUnit_C*>(corpse)), &minRange, &maxRange, NULL);

            if (maxRange * maxRange < distSq) {
                return 1;
            }
        }
    } else {
        return 1;
    }

    data->m_bestDot = dot;
    data->m_bestGuid = guid;
    data->m_bestDistSq = distSq;

    return 1;
}

// OFFSET: 0x008D35A0
void CGGameUI::AutoMouseOver() {
    if (CGInputControl::GetActive()->m_mouseModeFlags & 0x1) {
        return;
    }

    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return;
    }

    CGUnit_C* mover = player->GetPossessedUnit();

    if (!mover) {
        mover = player;
    }

    AutoMouseOverData data;
    data.m_facing.x = 0.0f;
    data.m_facing.y = 0.0f;
    data.m_facing.z = 0.0f;
    data.m_mover = mover;
    data.m_player = player;
    float facing = mover->GetFacing();
    data.m_facing.x = cosf(facing);
    data.m_facing.y = sinf(facing);
    data.m_time = FrameTime::GetCurTimeMs();
    data.m_bestDot = 0.0f;
    data.m_bestDistSq = s_maxMatchDistance;
    data.m_bestGuid = WOWGUID();

    ClntObjMgrEnumVisibleObjects(AutoMouseOverProc, &data);
    CGGameUI::HandleObjectTrackChange(data.m_bestGuid, WOWGUID());
}

// OFFSET: #0x00513960
void CGGameUI::SetPhaseShift(uint32_t phase) {
    if (static_cast<int32_t>(phase) != CGGameUI::m_phaseMask) {
        CGGameUI::m_phaseMask = phase;
        CGWorldStateInfo::ResetWorldStateUI();
        FrameScript_SignalEvent(SCRIPT_EVENT_UPDATE_WORLD_STATES, NULL);
    }
}

// OFFSET: 0x008D6080
void CGGameUI::UpdateAreaSpiritHealerTime(WOWGUID guid, int32_t time) {
    CGGameUI::UpdateAreaSpiritHealerDistance();

    if (CGGameUI::m_areaSpiritHealerGUID == guid && time > 0) {
        CGGameUI::m_areaSpiritHealerTime = time + FrameTime::GetCurTimeMs();

        if (!CGGameUI::m_areaSpiritHealerTime) {
            CGGameUI::m_areaSpiritHealerTime = 1;
        }

        FrameScript_SignalEvent(SCRIPT_EVENT_AREA_SPIRIT_HEALER_IN_RANGE, NULL);
    }
}

// OFFSET: 0x008D7D10
int32_t CGGameUI::MessageHandler(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    switch (msgId) {
        case SMSG_UPDATE_COMBO_POINTS: {
            SmartGUID guid;
            *msg >> guid;
            uint8_t points;
            msg->Get(points);
            WOWGUID target = guid;
            CGGameUI::UpdateComboPoints(target, points);
            return 1;
        }

        case SMSG_INIT_WORLD_STATES: {
            uint32_t mapId;
            uint32_t zoneId;
            uint32_t areaId;
            msg->Get(mapId);
            msg->Get(zoneId);
            msg->Get(areaId);
            bool changed = CGWorldStateInfo::InitWorldStates(mapId, zoneId, areaId);
            uint16_t count;
            msg->Get(count);

            for (uint16_t i = 0; i < count; i++) {
                uint32_t state;
                uint32_t value;
                msg->Get(state);
                msg->Get(value);

                if (CGWorldStateInfo::SetWorldState(state, value, false) || changed) {
                    changed = true;
                }
            }

            if (changed) {
                CGWorldMap::RefreshCurrentMap();
                FrameScript_SignalEvent(SCRIPT_EVENT_UPDATE_WORLD_STATES, NULL);
            }

            return 1;
        }

        case SMSG_PVP_CREDIT: {
            uint32_t honor;
            WOWGUID victim;
            uint32_t rank;
            msg->Get(honor);
            msg->Get(victim.m_guid);
            msg->Get(rank);
            CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();

            if (player) {
                player->ShowPlayerPVPHonorGained(victim, rank, honor);
            }

            return 1;
        }

        case SMSG_PROPOSE_LEVEL_GRANT: {
            SmartGUID guid;
            *msg >> guid;
            CGGameUI::ProposeLevelGrant(guid);
            return 1;
        }

        case CMSG_GM_REQUEST_PLAYER_INFO:
        case SMSG_GM_PLAYER_INFO:
            return 1;

        case SMSG_INVALID_PROMOTION_CODE:
            CGGameUI::DisplayError(ERR_INVALID_PROMOTION_CODE);
            return 1;

        case SMSG_PLAY_MUSIC: {
            uint32_t soundKitId;
            WOWGUID source;
            msg->Get(soundKitId);
            msg->Get(source.m_guid);
            SI3::PlayAIMusic(soundKitId);
            return 1;
        }

        case SMSG_UI_TIME: {
            uint32_t uiTime = 0;
            msg->Get(uiTime);
            CGWorldStateInfo::OnUITime(uiTime);
            return 1;
        }

        case SMSG_REFER_A_FRIEND_EXPIRED: {
            WOWGUID guid;
            msg->Get(guid.m_guid);
            CGPartyInfo::ClearRecruitAFriend(guid);
            CGRaidInfo::ClearRecruitAFriend(guid);

            if (FriendList::s_friendList) {
                FriendList::s_friendList->ClearRecruitAFriend(guid);
            }

            FrameScript_SignalEvent(SCRIPT_EVENT_FRIENDLIST_UPDATE, NULL);
            return 1;
        }

        case SMSG_AREA_TRIGGER_MESSAGE: {
            uint32_t length;
            msg->Get(length);
            void* text;
            msg->GetDataInSitu(text, length);

            if (text && *static_cast<const char*>(text)) {
                FrameScript_SignalEvent(SCRIPT_EVENT_UI_INFO_MESSAGE, "%s", static_cast<const char*>(text));
            }

            return 1;
        }

        case SMSG_UPDATE_WORLD_STATE: {
            uint32_t state = 0;
            uint32_t value = 0;
            uint8_t hidden = 0;
            msg->Get(state);
            msg->Get(value);
            msg->Get(hidden);
            CGWorldStateInfo::SetWorldState(state, value, hidden != 0);
            CGWorldMap::RefreshCurrentMap();
            FrameScript_SignalEvent(SCRIPT_EVENT_UPDATE_WORLD_STATES, NULL);
            SI3::ForceUpdateZoneSounds();
            return 1;
        }

        case MSG_SET_DUNGEON_DIFFICULTY: {
            uint32_t difficulty;
            uint32_t unk;
            uint32_t inGroup;
            msg->Get(difficulty);
            msg->Get(unk);
            msg->Get(inGroup);
            CGGameUI::SetDungeonDifficulty(difficulty, unk, inGroup);
            return 1;
        }

        case MSG_CORPSE_QUERY: {
            CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));
            uint8_t found;
            msg->Get(found);
            C3Vector position(0.0f, 0.0f, 0.0f);

            if (found) {
                uint32_t mapId;
                uint32_t actualMapId;
                uint32_t transport;
                msg->Get(mapId);
                *msg >> position;
                msg->Get(actualMapId);
                msg->Get(transport);

                if (player && (player->m_player->m_playerFlags & 0x10)) {
                    CGGameUI::SetCorpseLocation(mapId, position, actualMapId, transport);
                }
            } else {
                CGGameUI::SetCorpseLocation(-1, position, -1, 0);

                if (CGGameUI::m_corpseInRange) {
                    CGGameUI::m_corpseInRange = 0;

                    if (CGBattlefieldInfo::m_instanceType != 4) {
                        FrameScript_SignalEvent(SCRIPT_EVENT_CORPSE_OUT_OF_RANGE, NULL);
                    }
                }
            }

            return 1;
        }

        case SMSG_TALENTS_INVOLUNTARILY_RESET: {
            uint32_t reset;
            msg->GetInt8(reset);
            FrameScript_SignalEvent(SCRIPT_EVENT_TALENTS_INVOLUNTARILY_RESET, "%b", reset);
            return 1;
        }

        case SMSG_OVERRIDE_LIGHT: {
            uint32_t lightId;
            uint32_t overrideId;
            uint32_t transition;
            msg->Get(lightId);
            msg->Get(overrideId);
            msg->Get(transition);

            if (overrideId) {
                DayNight::ClearOverrideAreaLight(FrameTime::GetCurTimeMs(), lightId, transition);
                DayNight::SetOverrideAreaLight(FrameTime::GetCurTimeMs(), lightId, overrideId, transition);
            } else {
                DayNight::ClearOverrideAreaLight(FrameTime::GetCurTimeMs(), lightId, transition);
            }

            return 1;
        }

        case SMSG_AREA_TRIGGER_NO_CORPSE:
            CGGameUI::DisplayError(ERR_CORPSE_IS_NOT_IN_INSTANCE);
            return 1;

        case SMSG_TOTEM_CREATED: {
            uint8_t slot;
            WOWGUID guid;
            uint32_t duration;
            uint32_t spellId;
            msg->Get(slot);
            msg->Get(guid.m_guid);
            msg->Get(duration);
            msg->Get(spellId);
            TotemInfo* totem = CGGameUI::GetTotemInfo(static_cast<uint32_t>(slot));

            if (!totem || !guid) {
                return 1;
            }

            totem->m_guid = guid;
            totem->m_duration = duration;
            totem->m_startTime = FrameTime::GetCurTimeMs();
            const SpellRec* spell = g_spellDB.GetRecord(spellId);

            if (spell) {
                SpellIconRec* icon = g_spellIconDB.GetRecord(spell->m_spellIconID);

                if (icon) {
                    totem->m_icon = icon->m_textureFilename;
                }
            }

            return 1;
        }

        case SMSG_PLAY_OBJECT_SOUND: {
            uint32_t soundKitId;
            WOWGUID guid;
            WOWGUID source;
            msg->Get(soundKitId);
            msg->Get(guid.m_guid);
            msg->Get(source.m_guid);
            CGObject_C* object = ClntObjMgrObjectPtr(guid, TYPEMASK_OBJECT, __FILE__, __LINE__);

            if (!object) {
                return 1;
            }

            if (object->m_obj->m_type & TYPEMASK_UNIT) {
                static_cast<CGUnit_C*>(object)->PlayAISound(soundKitId, source);
            } else {
                C3Vector position = object->GetPosition();
                SE3SoundKitProperties properties;
                SE3::PlaySoundKit(soundKitId, &position, &properties, NULL, __FILE__, __LINE__, "OnGameUIEvent", 0, NULL);
            }

            return 1;
        }

        case SMSG_WEATHER: {
            float transition = 1.0f;
            uint32_t type = 0;
            int32_t soundKitId = 0;
            uint32_t weatherId;
            float intensity;
            uint8_t abrupt;
            msg->Get(weatherId);
            msg->Get(intensity);
            msg->Get(abrupt);
            WeatherRec* rec = g_weatherDB.GetRecord(weatherId);

            if (rec) {
                type = rec->m_effectType;
                soundKitId = rec->m_ambienceID;
                transition = rec->m_transitionSkyBox;
            }

            World::SetWeather(type, intensity, rec, abrupt == 0, transition);

            if (soundKitId <= 0) {
                soundKitId = -1;
            }

            SI3::SetWeatherKitID(soundKitId);
            ConsolePrintf("Weather changed to %d, intensity %f\n", type, intensity);
            return 1;
        }

        case SMSG_PLAY_SOUND: {
            uint32_t soundKitId;
            WOWGUID source;
            msg->Get(soundKitId);
            msg->Get(source.m_guid);
            uint32_t* handle = NULL;
            CGObject_C* object = ClntObjMgrObjectPtr(source, TYPEMASK_OBJECT, __FILE__, __LINE__);

            if (object && (object->m_obj->m_type & TYPEMASK_UNIT)) {
                handle = &static_cast<CGUnit_C*>(object)->m_npcInteractSound;
            }

            SE3SoundKitProperties properties;
            properties.m_usage = 2;
            SE3::PlaySoundKit(soundKitId, NULL, &properties, handle, __FILE__, __LINE__, "SMSG_PLAY_SOUND", 0, NULL);
            return 1;
        }

        case SMSG_CORPSE_RECLAIM_DELAY: {
            uint32_t delay;
            msg->Get(delay);
            CGGameUI::SetCorpseReclaimDelay(delay);
            return 1;
        }

        case SMSG_BREAK_TARGET: {
            SmartGUID guid;
            *msg >> guid;
            CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));
            WOWGUID target = guid;

            if (!CGGameUI::IsRaidMemberOrPet(target) || (unit && unit->IsDuelingWith(ClntObjMgrGetActivePlayerObj()))) {
                CGGameUI::BreakTarget(guid);
            }

            return 1;
        }

        case MSG_SET_RAID_DIFFICULTY: {
            uint32_t difficulty;
            uint32_t unk;
            uint32_t inGroup;
            msg->Get(difficulty);
            msg->Get(unk);
            msg->Get(inGroup);
            CGGameUI::SetRaidDifficulty(difficulty, unk, inGroup, 1);
            return 1;
        }

        case SMSG_AREA_SPIRIT_HEALER_TIME: {
            WOWGUID guid;
            uint32_t healTime;
            msg->Get(guid.m_guid);
            msg->Get(healTime);
            CGGameUI::UpdateAreaSpiritHealerTime(guid, healTime);
            return 1;
        }

        case SMSG_CORPSE_MAP_POSITION_QUERY_RESPONSE: {
            C3Vector position(0.0f, 0.0f, 0.0f);
            float facing;
            msg->GetArray(reinterpret_cast<uint32_t*>(&position), 3);
            msg->Get(facing);
            CGGameUI::SetCorpseMapPosition(position, facing);
            return 1;
        }

        case SMSG_REFER_A_FRIEND_FAILURE: {
            uint32_t error = 0;
            msg->Get(error);
            char name[48];
            name[0] = '\0';

            if (error == 9) {
                msg->GetString(name, sizeof(name));
            }

            BroadcastUIError(error, name);
            return 1;
        }

        default:
            return 1;
    }
}

// OFFSET: 0x008C4740
void CGGameUI::SetCorpseMapPosition(const C3Vector& position, const float& facing) {
    s_corpseMapRotation = C44Matrix::RotationAroundZ(facing);
    s_corpseMapPosition = position;
}

// OFFSET: 0x008CEFA0/0x140641530
void CGGameUI::StopCinematicInternal(void* param) {
    CGCamera* camera = CGWorldFrame::GetActiveCamera();
    FrameScript_SignalEvent(SCRIPT_EVENT_CINEMATIC_STOP, NULL);
    CGObject_C* target = static_cast<CGObject_C*>(ClntObjMgrObjectPtr(camera->m_targetGUID, TYPEMASK_OBJECT, __FILE__, __LINE__));

    if (target) {
        const C3Vector& shot = StaticSingleton<CCameraManager>::Instance()->m_shot.m_position;
        C3Vector position = target->GetPosition();
        float dz = shot.z - position.z;
        float dy = shot.y - position.y;
        float dx = shot.x - position.x;

        if (dz * dz + dy * dy + dx * dx > 2025.0f) {
            World::Preload(target->GetPosition(), 1);
        }
    }

    camera->ClearModelCamera();
    CGObject_C::UpdateAllWorldObjects();
    ScrnLayerPaint();

    if (!SFile::IsStreamingMode()) {
        AsyncFileReadWaitAll();
    }

    SE3::StopSound(CGGameUI::m_cinematic.m_music, -1, __FILE__, __LINE__, "CGGameUI::StopCinematicInternal");
    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_COMPLETE_CINEMATIC));
    msg.Finalize();
    ClientServices::Send2(&msg);
    CGGameUI::m_inCinematic = 0;
    DisableFadingScreen(0.25f, NULL, NULL);
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player && !(player->m_unit->m_unitFieldFlags & 0x100000)) {
        CGChat::ZoneUpdate();
    }

    CGInputControl* input = CGInputControl::GetActive();
    input->m_mouseModeFlags &= ~0x40;
    input->UpdateMouseMode(0);
    CGInputControl::GetActive()->EnterWorld();
}

// OFFSET: 0x008CA290/0x14063A620
void CGGameUI::GetCorpsePosition(C3Vector& position) {
    if (!CGGameUI::m_corpseTransportGUID) {
        position = CGGameUI::m_corpsePos;
        return;
    }

    if (ClntObjMgrObjectPtr(CGGameUI::m_corpseTransportGUID, TYPEMASK_OBJECT, __FILE__, __LINE__)) {
        C44Matrix transform;

        if (!MovementGetTransportMtxX(CGGameUI::m_corpseTransportGUID, &transform, ClntObjMgrGetActivePlayer(), __FILE__, __LINE__)) {
            position = CGGameUI::m_corpsePos;
            return;
        }

        position = CGGameUI::m_corpsePos * transform;
        s_corpseQueryTime = 0;
        return;
    }

    if (!s_corpseQueryTime || static_cast<int32_t>(_time32(NULL) - s_corpseQueryTime) >= 0) {
        WDataStore msg;
        msg.Put(static_cast<uint32_t>(CMSG_CORPSE_MAP_POSITION_QUERY));
        msg.Put(CGGameUI::m_corpseTransportGUID.m_guid);
        msg.Finalize();
        ClientServices::Send2(&msg);
        s_corpseQueryTime = static_cast<int32_t>(_time32(NULL)) + 30;
    }

    position = CGGameUI::m_corpsePos * s_corpseMapRotation + s_corpseMapPosition;
}

// OFFSET: 0x008CA590
void CGGameUI::OnWorldStateChanged(int32_t worldState) {
    int32_t low = 0;
    int32_t high = g_areaPOISortedWorldStateDB.m_numRecords;

    while (low < high) {
        int32_t mid = (low + high) >> 1;
        const AreaPOISortedWorldStateRec* rec = g_areaPOISortedWorldStateDB.GetRecordByIndex(mid);

        if (!rec) {
            return;
        }

        if (worldState == rec->m_worldStateID) {
            if (!CGGameUI::s_cemeteryListRequestTime) {
                CGGameUI::s_cemeteryListRequestTime = s_cemeteryListRequestDelay + FrameTime::GetCurTimeMs();
            }

            return;
        }

        if (worldState >= rec->m_worldStateID) {
            low = mid + 1;
        } else {
            high = mid - 1;
        }

        if (low > high) {
            return;
        }
    }
}

// OFFSET: 0x008CA610
void CGGameUI::SetCemeteryList(uint32_t count, const uint32_t* cemeteries) {
    CGGameUI::s_numCemeteries = 0;

    for (uint32_t i = 0; i < count && CGGameUI::s_numCemeteries < 16; i++) {
        uint32_t cemetery = cemeteries[i];

        for (int32_t j = 0; j < g_areaPOISortedWorldStateDB.m_numRecords; j++) {
            const AreaPOISortedWorldStateRec* rec = g_areaPOISortedWorldStateDB.GetRecordByIndex(j);

            if (rec && rec->m_cemeteryID == static_cast<int32_t>(cemetery)) {
                CGGameUI::s_cemeteries[CGGameUI::s_numCemeteries++] = cemetery;
                break;
            }
        }
    }
}

// OFFSET: 0x008C47B0
static void UpdateCorpseRange(int32_t inRange) {
    if (inRange == CGGameUI::m_corpseInRange) {
        return;
    }

    CGGameUI::m_corpseInRange = inRange;

    if (CGBattlefieldInfo::m_instanceType == 4) {
        return;
    }

    if (inRange) {
        FrameScript_SignalEvent(SCRIPT_EVENT_CORPSE_IN_RANGE + (CGGameUI::m_corpseMapID != CGGameUI::m_corpseActualMapID), NULL);
    } else {
        FrameScript_SignalEvent(SCRIPT_EVENT_CORPSE_OUT_OF_RANGE, NULL);
    }
}

// OFFSET: 0x008D12D0
void CGGameUI::UpdateCorpseDistance() {
    int32_t inRange = 0;

    if (CGGameUI::m_corpseMapID >= 0) {
        CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

        if (player) {
            C3Vector corpse(0.0f, 0.0f, 0.0f);
            CGGameUI::GetCorpsePosition(corpse);

            if (CGGameUI::m_corpseMapID == CGPlayer_C::GetNewContinentID()) {
                MinimapSetCorpsePOI(corpse.x, corpse.y);
            }

            C3Vector position = player->GetPosition();
            float dx = position.x - corpse.x;
            float dy = position.y - corpse.y;
            float dz = position.z - corpse.z;

            if (dz * dz + dy * dy + dx * dx <= s_corpseRangeSq) {
                inRange = 1;
            }
        }
    }

    UpdateCorpseRange(inRange);
}

// OFFSET: 0x008D1A00
void CGGameUI::UpdateInteractTarget() {
    if (!CGGameUI::m_interactTarget || CGGameUI::m_interactMaxDistSq <= 0.0f) {
        return;
    }

    bool clear = false;
    CGObject_C* object = ClntObjMgrObjectPtr(CGGameUI::m_interactTarget, TYPEMASK_OBJECT, __FILE__, __LINE__);

    if (!object || (object->m_modelFlags & 0x10000)) {
        CGGameUI::ClearInteractTarget(CGGameUI::m_interactTarget, 0);
        return;
    }

    if (object->m_obj->m_type & TYPEMASK_ITEM) {
        return;
    }

    CGPlayer_C* player = ClntObjMgrGetActivePlayerObj();

    if (!player) {
        CGGameUI::ClearInteractTarget(CGGameUI::m_interactTarget, 0);
        return;
    }

    uint32_t type = object->m_obj->m_type;

    if (type & TYPEMASK_GAMEOBJECT) {
        CGGameObject_C* gameObject = static_cast<CGGameObject_C*>(object);

        if (!player->CGUnit_C::CanInteract(gameObject) || !gameObject->CheckRange()) {
            CGGameUI::ClearInteractTarget(CGGameUI::m_interactTarget, 0);
        }

        return;
    }

    if (type & TYPEMASK_UNIT) {
        CGUnit_C* unit = static_cast<CGUnit_C*>(object);

        if (((type & TYPEMASK_PLAYER) && player->m_obj->m_guid != object->m_obj->m_guid && !player->CanCooperate(unit)) || (!(object->m_obj->m_type & TYPEMASK_PLAYER) && !player->CanInteract(unit))) {
            clear = true;
        }
    }

    if (player->GetDistanceToPos(object->GetPosition()) > CGGameUI::m_interactMaxDistSq || clear) {
        CGGameUI::ClearInteractTarget(CGGameUI::m_interactTarget, 0);
    }
}

// OFFSET: 0x008D4EA0/0x140647EC0
static int32_t AreaSpiritHealerCheck(WOWGUID guid, void* param) {
    CGUnit_C* player = static_cast<CGUnit_C*>(param);
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));

    if (unit && (unit->m_unit->m_unitNpcFlags & 0x8000) && unit->CanAssist(player, false) && unit->GetDistanceToPos(player->GetPosition()) <= 400.0f) {
        CGGameUI::SetAreaSpiritHealer(unit->m_obj->m_guid);
    }

    return 1;
}

// OFFSET: 0x008D4F30/0x140647F60
void CGGameUI::UpdateAreaSpiritHealerDistance() {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player || !(player->m_player->m_playerFlags & 0x10)) {
        CGGameUI::SetAreaSpiritHealer(WOWGUID());
        return;
    }

    if (CGGameUI::m_areaSpiritHealerGUID) {
        CGUnit_C* healer = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(CGGameUI::m_areaSpiritHealerGUID, TYPEMASK_UNIT, __FILE__, __LINE__));

        if (!healer || player->GetDistanceToPos(healer->GetPosition()) > 484.00003f) {
            CGGameUI::SetAreaSpiritHealer(WOWGUID());
        }

        if (CGGameUI::m_areaSpiritHealerGUID) {
            return;
        }
    }

    ClntObjMgrEnumVisibleObjects(AreaSpiritHealerCheck, player);
}

// OFFSET: 0x008D5870/0x140646D30
void CGGameUI::ShowDungeonDifficultyChanged() {
    char token[64];
    SStrPrintf(token, sizeof(token), "DUNGEON_DIFFICULTY%d", CGGameUI::GetCurrentDungeonDifficulty() + 1);
    CGGameUI::DisplayError(ERR_DUNGEON_DIFFICULTY_CHANGED_S, FrameScript_GetText(token, -1, GENDER_NOT_APPLICABLE));
}

// OFFSET: 0x008D6D60
void CGGameUI::SetDungeonDifficulty(uint32_t difficulty, uint32_t unk, uint32_t inGroup) {
    uint32_t previous = CGGameUI::GetCurrentDungeonDifficulty();

    if (unk) {
        CGGameUI::m_dungeonDifficulty = difficulty;
        CGGameObject_C::UpdateDungeonDifficulty();
    }

    if (inGroup) {
        CGPartyInfo::SetDungeonDifficulty(difficulty);
    }

    if (previous != CGGameUI::GetCurrentDungeonDifficulty()) {
        CGGameUI::ShowDungeonDifficultyChanged();
    }
}

// OFFSET: 0x008D58D0/0x140646D90
void CGGameUI::SendRaidDifficultyMessage() {
    MapRec* map = g_mapDB.GetRecord(CGGameUI::m_iCurrentMapID);

    if (map && map->m_instanceType == 2) {
        MapDifficultyRec* difficulty = GetMapDifficultyRec(CGGameUI::m_iCurrentMapID, CGGameUI::GetCurrentRaidDifficulty(), NULL);

        if (difficulty) {
            CGGameUI::DisplayError(ERR_RAID_DIFFICULTY_CHANGED_S, FrameScript_GetText(difficulty->m_difficultystring, -1, GENDER_NOT_APPLICABLE));
            return;
        }
    }

    char token[64];
    SStrPrintf(token, sizeof(token), "RAID_DIFFICULTY%d", CGGameUI::GetCurrentRaidDifficulty() + 1);
    CGGameUI::DisplayError(ERR_RAID_DIFFICULTY_CHANGED_S, FrameScript_GetText(token, -1, GENDER_NOT_APPLICABLE));
}

// OFFSET: 0x008D6DE0/0x140649300
bool CGGameUI::SetRaidDifficulty(uint32_t difficulty, uint32_t unk, uint32_t inGroup, int32_t force) {
    uint32_t previous = CGGameUI::GetCurrentRaidDifficulty();

    if (unk) {
        CGGameUI::m_raidDifficulty = difficulty;
        CGGameObject_C::UpdateDungeonDifficulty();
    }

    if (inGroup) {
        MapRec* map = g_mapDB.GetRecord(CGGameUI::m_iCurrentMapID);

        if (map && (map->m_flags & 0x100) && !force) {
            if (difficulty <= 3 && (previous + 2 == difficulty || previous - 2 == difficulty)) {
                CGPartyInfo::ChangeRaidDifficulty(difficulty);
            }

            return false;
        }

        CGPartyInfo::SetRaidDifficulty(difficulty);
    }

    uint32_t dungeonDifficulty = CGGameUI::GetCurrentDungeonDifficulty();

    if (previous != CGGameUI::GetCurrentRaidDifficulty()) {
        CGGameUI::SendRaidDifficultyMessage();
    }

    if (dungeonDifficulty != CGGameUI::GetCurrentDungeonDifficulty()) {
        CGGameUI::ShowDungeonDifficultyChanged();
    }

    return true;
}

// OFFSET: 0x008DB580
void CGGameUI::ShutdownGame() {
    CGGameUI::Shutdown();
    PortraitShutdown();
    CGMinimapFrame::ShutdownGame();
    CGChat::ShutdownGame();
    CGPartyInfo::ShutdownGame();
    CGGlyphInfo::ShutdownGame();
    CGSpellBook::ShutdownGame();
    CGSpellActivationOverlay::ShutdownGame();
    CGActionBar::ShutdownGame();
    CFlyout::ShutdownGame();
    CGLootInfo::ShutdownGame();
    CGItemText::ShutdownGame();
    CGGossipInfo::ShutdownGame();
    CGQuestLog::ShutdownGame();
    CGTaxiMap::ShutdownGame();
    CGClassTrainer::ShutdownGame();
    CGTradeSkillInfo::ShutdownGame();
    CGPetInfo::ShutdownGame();
    CGWorldMap::ShutdownGame();
    CGReputationInfo::ShutdownGame();
    CGDuelInfo::ShutdownGame();
    CGGuildInfo::ShutdownGame();
    CGLFGuildInfo::ShutdownGame();
    CGMailInfo::ShutdownGame();
    CGBattlefieldInfo::ShutdownGame();
    CGTalentInfo::ShutdownGame();
    CGTalentInfo_Inspect::ShutdownGame();
    CGAuctionHouse::ShutdownGame();
    CGStableInfo::ShutdownGame();
    CGRaidInfo::ShutdownGame();
    CGChatBubbleFrame::ShutdownGame();
    CGCharacterInfo::ShutdownGame();
    CGWorldStateInfo::ShutdownGame();
    CGMinigameInfo::ShutdownGame();
    CGArenaTeamInfo::ShutdownGame();
    CGLookingForGroup::ShutdownGame();
    CGPetitionInfo::ShutdownGame();
    CGCommentator::ShutdownGame();
    CGCalendar::ShutdownGame();
    CGDanceStudio::ShutdownGame();
    CGBarberShop::ShutdownGame();
    CGGuildBankInfo::ShutdownGame();
    CGAchievementInfo::ShutdownGame();
    CGCurrencyTypes::ShutdownGame();
    CGEquipmentManager::ShutdownGame();
    CGAutoCompleteName::m_list.Clear();
    CGGMTicketInfo::ShutdownGame();
    CGGMSurveyInfo::ShutdownGame();
    CGReforge::ShutdownGame();
    CGTradeInfo::ShutdownGame();
    CGResearchFrame::ShutdownGame();
    PhaseShift_C_ShutdownGame();
    MovieStreaming::ShutdownGame();
    CGEncounterJournal::ShutdownGame();
    CGCUFProfiles::ShutdownGame();
    CGIncomingResurrection::Reset();
    CGTransmogrify::ShutdownGame();
    CGVoidStorageInfo::ShutdownGame();
    FrameScript_Flush();

    for (uint32_t i = 0; i < sizeof(s_readOnlyCVars) / sizeof(s_readOnlyCVars[0]); i++) {
        CVar* var = CVar::Lookup(s_readOnlyCVars[i]);

        if (var) {
            var->SetReadOnly(false);
        }
    }

    CGGameUI::m_areaID = 0;
    CGGameUI::m_subzoneID = 0;
    CGGameUI::m_wmoGroupID = 0;

    if (CGGameUI::m_zoneText) {
        SMemFree(const_cast<char*>(CGGameUI::m_zoneText), __FILE__, __LINE__, 0);
    }

    CGGameUI::m_zoneText = NULL;

    if (CGGameUI::m_subZoneText) {
        SMemFree(const_cast<char*>(CGGameUI::m_subZoneText), __FILE__, __LINE__, 0);
    }

    CGGameUI::m_subZoneText = NULL;

    if (CGGameUI::m_minimapZoneText) {
        SMemFree(const_cast<char*>(CGGameUI::m_minimapZoneText), __FILE__, __LINE__, 0);
    }

    CGGameUI::m_minimapZoneText = NULL;
    CGGameUI::m_areaMountFlags = -1;

    for (int32_t i = 0; i < 3; i++) {
        MirrorTimerInfo& timer = CGGameUI::m_mirrorTimerInfo[i];
        timer.m_type = 3;
        timer.unk_0018 = 0;
        timer.m_spellId = 0;
        timer.m_paused = 0;
        timer.m_scale = 0;
        timer.m_maxValue = 0;
        timer.m_value = 0;
    }

    ClientServices::ClearMessageHandler(SMSG_PVP_CREDIT);
    ClientServices::ClearMessageHandler(MSG_CORPSE_QUERY);
    ClientServices::ClearMessageHandler(SMSG_CORPSE_RECLAIM_DELAY);
    ClientServices::ClearMessageHandler(CMSG_GM_REQUEST_PLAYER_INFO);
    ClientServices::ClearMessageHandler(SMSG_GM_PLAYER_INFO);
    ClientServices::ClearMessageHandler(SMSG_PLAY_MUSIC);
    ClientServices::ClearMessageHandler(SMSG_PLAY_SOUND);
    ClientServices::ClearMessageHandler(SMSG_PLAY_OBJECT_SOUND);
    ClientServices::ClearMessageHandler(SMSG_AREA_TRIGGER_MESSAGE);
    ClientServices::ClearMessageHandler(SMSG_AREA_TRIGGER_NO_CORPSE);
    ClientServices::ClearMessageHandler(SMSG_INIT_WORLD_STATES);
    ClientServices::ClearMessageHandler(SMSG_UPDATE_WORLD_STATE);
    ClientServices::ClearMessageHandler(SMSG_AREA_SPIRIT_HEALER_TIME);
    ClientServices::ClearMessageHandler(SMSG_INVALID_PROMOTION_CODE);
    ClientServices::ClearMessageHandler(SMSG_WEATHER);
    ClientServices::ClearMessageHandler(SMSG_OVERRIDE_LIGHT);
    ClientServices::ClearMessageHandler(MSG_SET_DUNGEON_DIFFICULTY);
    ClientServices::ClearMessageHandler(MSG_SET_RAID_DIFFICULTY);
    ClientServices::ClearMessageHandler(SMSG_UPDATE_COMBO_POINTS);
    ClientServices::ClearMessageHandler(SMSG_TOTEM_CREATED);
    ClientServices::ClearMessageHandler(SMSG_START_MIRROR_TIMER);
    ClientServices::ClearMessageHandler(SMSG_PAUSE_MIRROR_TIMER);
    ClientServices::ClearMessageHandler(SMSG_STOP_MIRROR_TIMER);
    ClientServices::ClearMessageHandler(SMSG_PROPOSE_LEVEL_GRANT);
    ClientServices::ClearMessageHandler(SMSG_REFER_A_FRIEND_FAILURE);
    ClientServices::ClearMessageHandler(SMSG_REFER_A_FRIEND_EXPIRED);
    ClientServices::ClearMessageHandler(SMSG_BREAK_TARGET);
    ClientServices::ClearMessageHandler(SMSG_CORPSE_MAP_POSITION_QUERY_RESPONSE);
    ClientServices::ClearMessageHandler(SMSG_UI_TIME);
    ClientServices::ClearMessageHandler(SMSG_TALENTS_INVOLUNTARILY_RESET);
    ClientDifferentInstanceFromParty::s_handler = NULL;
    ClientMoneyNotify::s_handler = NULL;
    ClientDisplayGameError::s_handler = NULL;
    ClientWorldServerInfo::s_handler = NULL;
    ClientFailedPlayerCondition::s_handler = NULL;
}

// OFFSET: 0x008DAF20/0x14064F060
void CGGameUI::StartCinematic(int32_t cinematic) {
    memset(&CGGameUI::m_cinematic, 0, sizeof(CGGameUI::m_cinematic));
    CGGameUI::m_cinematic.m_sequence = g_cinematicSequencesDB.GetRecord(cinematic);
    CVar* music = CVar::Lookup("Sound_EnableMusic");

    if (CGGameUI::m_cinematic.m_sequence) {
        if (!music || music->m_intValue) {
            SI3::StopAllMusic();
            SE3SoundKitProperties properties;
            properties.m_bit10 = 1;
            properties.m_bus = SE3BUS_MOVIE;
            properties.m_usage = 1;
            SE3::PlaySoundKit(CGGameUI::m_cinematic.m_sequence->m_soundID, NULL, &properties, &CGGameUI::m_cinematic.m_music, __FILE__, __LINE__, "CGGameUI::StartCinematic", 0, NULL);
        }

        if (CGGameUI::m_cinematic.m_sequence) {
            CGGameUI::m_cinematic.m_camera = g_cinematicCameraDB.GetRecord(CGGameUI::m_cinematic.m_sequence->m_camera[0]);
        }
    }

    CGGameUI::m_inCinematic = 1;
    CGInputControl* input = CGInputControl::GetActive();
    input->m_mouseModeFlags |= 0x40;
    input->UpdateMouseMode(0);

    if (ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__)) {
        EnableFadingScreen(0.25f, &CGGameUI::BeginCinematicInternal, NULL);
    }
}

// OFFSET: 0x008C4110/0x140633C10
static void GetTaintContext(TaintStackFrame* frame, char* buffer, uint32_t size) {
    char name[1024];

    if (frame->m_objectName) {
        if (frame->m_debug.name) {
            SStrPrintf(name, sizeof(name), "%s:%s", frame->m_objectName, frame->m_debug.name);
        } else if (*frame->m_debug.source == '*') {
            SStrPrintf(name, sizeof(name), "%s:%s", frame->m_objectName, frame->m_debug.source + 2);
            frame->m_debug.source = "";
        } else {
            name[0] = '\0';
        }

        frame->m_debug.name = name;
    }

    if (*frame->m_debug.source == '@') {
        frame->m_debug.source++;
    }

    const char* source = frame->m_debug.source;

    if (*source && *source != '=') {
        if (frame->m_debug.name && *frame->m_debug.name) {
            SStrPrintf(buffer, size, "%s:%d %s()", source, frame->m_debug.currentline, frame->m_debug.name);
        } else {
            SStrPrintf(buffer, size, "%s:%d", source, frame->m_debug.currentline);
        }
    } else if (frame->m_debug.name && *frame->m_debug.name) {
        SStrPrintf(buffer, size, "%s()", frame->m_debug.name);
    } else {
        *buffer = '\0';
    }
}

// OFFSET: 0x008C9E40/0x14063A190
static void PrintTaintEvent(TAINTEVENT* event) {
    char context[1024];
    char text[4096];

    if (event->m_stack.m_count) {
        GetTaintContext(&event->m_stack.m_data[0], context, sizeof(context));
    } else {
        SStrCopy(context, "UNKNOWN", sizeof(context));
    }

    switch (event->m_type) {
        case 1:
            SStrPrintf(text, sizeof(text), "Global variable %s tainted by %s - %s", static_cast<const char*>(event->m_name), event->m_taint, context);
            DebugLog("%s\n", text);
            SLogWrite(s_taintLog, "%s", text);
            break;

        case 2:
            SStrPrintf(text, sizeof(text), "Execution tainted by %s while reading %s - %s", event->m_taint, static_cast<const char*>(event->m_name), context);
            DebugLog("%s\n", text);
            SLogWrite(s_taintLog, "%s", text);
            break;

        case 3:
            SStrPrintf(text, sizeof(text), "An action was blocked because of taint from %s - %s", event->m_taint, context);
            DebugLog("%s\n", text);
            SLogWrite(s_taintLog, "%s", text);
            break;

        case 4:
            SStrPrintf(text, sizeof(text), "An action was blocked in combat because of taint from %s - %s", event->m_taint, context);
            DebugLog("%s\n", text);
            SLogWrite(s_taintLog, "%s", text);
            break;
    }

    for (uint32_t i = 1; i < event->m_stack.m_count; i++) {
        GetTaintContext(&event->m_stack.m_data[i], context, sizeof(context));

        if (context[0]) {
            SStrPrintf(text, sizeof(text), "    %s", context);
            DebugLog("%s\n", text);
            SLogWrite(s_taintLog, "%s", text);
        }
    }
}

// OFFSET: 0x008DD800/0x1406521D0
void TaintEventCallback(lua_State* L, int32_t event, const char* name, const char* taint) {
    if (taint == g_macroTaint) {
        taint = "MACRO_TAINT";
    }

    if (g_taintLogCVar->m_intValue == 1 && (event == 3 || event == 4) && s_lastTaintEvent.m_type == 2 && s_lastTaintEvent.m_taint == taint) {
        TAINTEVENT* global = s_taintEvents.Ptr(s_lastTaintEvent.m_name);

        if (global) {
            PrintTaintEvent(global);
        }

        PrintTaintEvent(&s_lastTaintEvent);
    }

    TAINTEVENT* record;

    if (event == 1) {
        record = s_taintEvents.Ptr(name);

        if (!record) {
            record = s_taintEvents.New(name, 0, 0);
        }
    } else {
        record = &s_lastTaintEvent;
    }

    record->m_type = event;
    record->m_name = name;
    record->m_taint = taint;
    record->m_stack.m_count = 0;

    lua_Debug ar;

    if (lua_getstack(L, 0, &ar)) {
        int32_t level = 1;

        do {
            const char* objectName = NULL;
            lua_getinfo(L, "Sln", &ar);

            if ((*ar.source == '*' || !SStrCmp(ar.namewhat, "method", STORM_MAX_STR)) && lua_getlocal(L, &ar, 1)) {
                if (lua_type(L, -1) == LUA_TTABLE) {
                    lua_rawgeti(L, -1, 0);
                    FrameScript_Object* object = static_cast<FrameScript_Object*>(lua_touserdata(L, -1));
                    lua_settop(L, -2);

                    if (object) {
                        const char* objectNameValue = object->GetName();
                        objectName = objectNameValue ? objectNameValue : "<unnamed>";
                    }
                }

                lua_settop(L, -2);
            }

            record->m_stack.SetCount(level);
            memcpy(&record->m_stack.m_data[level - 1].m_debug, &ar, sizeof(ar));
            record->m_stack.m_data[level - 1].m_objectName = objectName;
        } while (lua_getstack(L, level++, &ar));
    }

    if (g_taintLogCVar->m_intValue != 1 || (event != 1 && event != 2)) {
        PrintTaintEvent(record);
    }
}

// OFFSET: 0x008DB370/0x14064F4E0
void CGGameUI::Shutdown() {
    CGGameUI::m_accountDataGeneration++;
    g_theGxDevicePtr->RemoveTextureRecreationCallback(UpdateAllPortraits);
    g_theGxDevicePtr->RemoveDeviceRestoredCallback(UpdateAllPortraits);
    CGGameUI::Target(WOWGUID());
    CGGameUI::SetFocusTarget(WOWGUID());
    CGGameUI::ClearCursor(1, 1);
    CGTradeInfo::CancelTrade();
    CGTradeSkillInfo::Close();
    DBClient_ClearDB2Caches();

    if (!SFile::IsStreamingMode()) {
        AsyncFileReadWaitAll();
    }

    if (ClntObjMgrGetActivePlayer()) {
        CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

        if (player) {
            player->ClearTrackingTarget(false, true);

            if (player->m_unit->m_unitFieldFlags & 0x80000) {
                CGGameUI::m_simpleTop->m_allowProtectedActions = 1;
                FrameScript_SignalEvent(SCRIPT_EVENT_PLAYER_REGEN_ENABLED, NULL);
                CGGameUI::UpdateCombatMode();
            }
        }

        CGGameUI::LeaveWorld();
    }

    CGLCD::Logout();
    FrameScript_SignalEvent(SCRIPT_EVENT_PLAYER_LOGOUT, NULL);
    ClearUniqueSignals();
    CGGameUI::m_initialized = 0;
    DBCache_ClearPending();
    CSimpleScriptManager::Destroy();
    CGUnit_C::NamePlateClearAll();
    CGChatBubbleFrame::ClearAll();
    CGWorldMap::ClearArrowFrames();
    CGMinimapFrame::SaveShapeshiftTracking();
    CGChat::SaveChatSettings();
    CGChat::Shutdown();
    CGUIMacros::SaveMacros();
    CGUIMacros::Shutdown();
    CGUIBindings::SaveBindings();
    CGUIBindings::Shutdown();
    CGQuestLog::SaveTrackedQuests();
    CGAchievementInfo::SaveTrackedAchievements();
    CGTutorial::SaveFlaggedTutorials();
    CGGameUI::SaveSavedVariables();
    SaveAccountData(6, SavePlacedFrames, CGGameUI::m_simpleTop);
    SaveAccountData(0, SaveGameCVarsCallback, reinterpret_cast<void*>(16));
    SaveAccountData(1, SaveGameCVarsCallback, reinterpret_cast<void*>(32));
    ConsoleSaveAllClientCVar();
    CVar::DeleteCharacterCVars();
    AccountDataFlush();
    UnloadAddOns();
    SaveAddOnEnableState();

    if (CGGameUI::m_simpleTop) {
        delete CGGameUI::m_simpleTop;
        CGGameUI::m_simpleTop = NULL;
        CGGameUI::m_UISimpleParent = NULL;
        CGGameUI::m_gameTooltip = NULL;
    }

    CursorDestroy();
    UnloadScriptFunctions();
    EventUnregister(EVENT_ID_IDLE, CGGameUI::Idle);
    DisableFadingScreen(0.0f, NULL, NULL);

    if (s_taintLog) {
        SLogClose(s_taintLog);
        s_taintLog = NULL;
    }

    s_lastTaintEvent.m_type = 0;
}

// OFFSET: 0x008CC000
int32_t IsInFlyableArea() {
    AreaTableRec* area = g_areaTableDB.GetRecord(CGGameUI::m_subzoneID);

    if (!area) {
        area = g_areaTableDB.GetRecord(CGGameUI::m_areaID);
    }

    return area && (area->m_mountFlags & 2);
}

// OFFSET: 0x008D87E0
void CGGameUI::SaveSavedVariables() {
    int32_t created = 0;

    for (int32_t i = 0; i < 2; i++) {
        char path[260];

        if (!GetSavedVariablesPath(path, sizeof(path), i)) {
            continue;
        }

        if (OsFileExists(path)) {
            char backup[260];
            SStrCopy(backup, path, sizeof(backup));
            SStrPack(backup, ".bak", sizeof(backup));
            OsMoveFile(path, backup);
        } else {
            created = 1;
        }

        HOSFILE file = OsCreateFile(path, OS_GENERIC_WRITE, 0, OS_CREATE_ALWAYS, OS_FILE_ATTRIBUTE_NORMAL, OS_FILE_TYPE_ANY);

        if (file == OS_INVALID_FILE) {
            return;
        }

        for (SAVEDVARIABLE* variable = CGGameUI::s_savedVariables[i].m_fulllist.Head(); variable; variable = CGGameUI::s_savedVariables[i].Next(variable)) {
            HASHKEY_STR name;
            name = variable->m_key.m_str;
            FrameScript_PrintVariable(file, name.m_str);
        }

        uint32_t written;
        OsWriteFile(file, "\r\n", 2, &written);
        OsCloseFile(file);
        CGGameUI::s_savedVariables[i].Clear();
    }

    if (created) {
        SFile::RebuildHash();
    }
}

// OFFSET: 0x008C4930
void CGGameUI::HandleFocusChanged(const CFocusEvent& event) {
    CGInputControl::GetActive()->OnFocusChange(event);
}

// OFFSET: 0x008C9350/0x140639440
static void PlacedFrameCallback(HOSFILE file, CSimpleFrame* frame) {
    const char* name = frame->GetName();

    if (!name || !*name) {
        return;
    }

    uint32_t flags = frame->m_frameFlags;

    if (!(flags & 0x1000) || (flags & 0x80000) || !(flags & 0x300)) {
        return;
    }

    CRect rect;
    rect.minY = 0.0f;
    rect.minX = 0.0f;
    rect.maxY = 0.0f;
    rect.maxX = 0.0f;
    CRect parentRect;
    parentRect.minY = 0.0f;
    parentRect.minX = 0.0f;
    parentRect.maxY = 0.0f;
    parentRect.maxX = 0.0f;

    if (!frame->GetRect(&rect)) {
        frame->Resize(1);

        if (!frame->GetRect(&rect)) {
            return;
        }
    }

    if (!frame->m_simpleTop->GetRect(&parentRect)) {
        return;
    }

    char line[128];
    uint32_t written;
    SStrPrintf(line, sizeof(line), "Frame: %s\n", name);
    OsWriteFile(file, line, SStrLen(line), &written);
    SStrPrintf(line, sizeof(line), "FrameLevel: %d\n", frame->m_level);
    OsWriteFile(file, line, SStrLen(line), &written);
    float dx = 0.0f;
    float dy = 0.0f;
    int32_t anchor = 0;
    C2Vector parentPoints[9];
    C2Vector points[9];
    GetRectFramePoints(parentRect, parentPoints);
    GetRectFramePoints(rect, points);
    float best = 3.402823466e38f;

    for (int32_t i = 0; i < 9; i++) {
        float ox = parentPoints[i].x - points[i].x;
        float oy = parentPoints[i].y - points[i].y;
        float distance = ox * ox + oy * oy;

        if (distance < best) {
            best = distance;
            anchor = i;
            dx = points[i].x - parentPoints[i].x;
            dy = points[i].y - parentPoints[i].y;
        }
    }

    float scale = frame->m_layoutScale;
    SStrPrintf(line, sizeof(line), "Anchor: %s\n", FramePointToString(static_cast<FRAMEPOINT>(anchor)));
    OsWriteFile(file, line, SStrLen(line), &written);
    SStrPrintf(line, sizeof(line), "X: %d\n", CMath::fint_(DDCToNDCWidth(CoordinateGetAspectCompensation() * 1024.0f * (dx / scale))));
    OsWriteFile(file, line, SStrLen(line), &written);
    SStrPrintf(line, sizeof(line), "Y: %d\n", CMath::fint_(DDCToNDCWidth(CoordinateGetAspectCompensation() * 1024.0f * (dy / scale))));
    OsWriteFile(file, line, SStrLen(line), &written);
    SStrPrintf(line, sizeof(line), "W: %d\n", CMath::fint_(DDCToNDCWidth(CoordinateGetAspectCompensation() * 1024.0f / frame->m_layoutScale * (rect.maxX - rect.minX))));
    OsWriteFile(file, line, SStrLen(line), &written);
    SStrPrintf(line, sizeof(line), "H: %d\n", CMath::fint_(DDCToNDCWidth(CoordinateGetAspectCompensation() * 1024.0f / frame->m_layoutScale * (rect.maxY - rect.minY))));
    OsWriteFile(file, line, SStrLen(line), &written);
}

// OFFSET: 0x008D4A10/0x140647A90
void SavePlacedFrames(HOSFILE localFile, HOSFILE file, void* param) {
    CSimpleTop* top = static_cast<CSimpleTop*>(param);

    if (!top || localFile == OS_INVALID_FILE) {
        return;
    }

    char line[128];
    uint32_t written;
    SStrPrintf(line, sizeof(line), "Version: %d\n", 1);
    OsWriteFile(localFile, line, SStrLen(line), &written);

    for (CSimpleFrame* frame = top->m_frames.Head(); frame; frame = top->m_frames.Next(frame)) {
        PlacedFrameCallback(localFile, frame);
    }
}

// OFFSET: 0x008C4470
void SaveGameCVarsCallback(HOSFILE localFile, HOSFILE file, void* param) {
    if (file == OS_INVALID_FILE) {
        return;
    }

    uint32_t category = static_cast<uint32_t>(reinterpret_cast<uintptr_t>(param));

    if (category == 32) {
        CGWorldFrame::GetActiveCamera()->SaveCameraSettings();
    }

    CVar::SaveAll(file, category, 0);
}

// OFFSET: 0x008C4EF0
void CGGameUI::AddErrorMessage(const char* text, int32_t error) {
    if (text && *text) {
        FrameScript_SignalEvent(error ? SCRIPT_EVENT_UI_ERROR_MESSAGE : SCRIPT_EVENT_UI_INFO_MESSAGE, "%s", text);
    }
}

// OFFSET: 0x008C5950
void CGGameUI::SetRepopTimeLeft(uint32_t time) {
    CGGameUI::m_deadNoRepopTimer = 0;
    CGGameUI::m_repopTime = time + FrameTime::GetCurTimeMs();

    if (!CGGameUI::m_repopTime) {
        CGGameUI::m_repopTime = 1;
    }
}

// OFFSET: 0x008C5980
void CGGameUI::ClearRepopTime() {
    CGGameUI::m_repopTime = 0;
    CGGameUI::m_deadNoRepopTimer = 1;
}

// OFFSET: 0x008C59A0
void CGGameUI::SetCorpseReclaimDelay(uint32_t delay) {
    CGGameUI::m_corpseReclaimDelay = delay + FrameTime::GetCurTimeMs();

    if (!CGGameUI::m_corpseReclaimDelay) {
        CGGameUI::m_corpseReclaimDelay = 1;
    }

    if (CGGameUI::m_corpseInRange) {
        FrameScript_SignalEvent(CGGameUI::m_corpseMapID != CGGameUI::m_corpseActualMapID ? SCRIPT_EVENT_CORPSE_IN_INSTANCE : SCRIPT_EVENT_CORPSE_IN_RANGE, NULL);
    }
}

// OFFSET: 0x008C5D50
const char* CGGameUI::GetErrorToken(int32_t error) {
    if (error >= ERR_MAX_VALUE) {
        return "";
    }

    return s_errorInfo[error].m_token;
}

// OFFSET: 0x008D4CB0
void CGGameUI::Reload() {
    if (lua_tainted && CGGameUI::m_simpleTop && !CGGameUI::m_simpleTop->unk_1260) {
        CGGameUI::ShowBlockedActionFeedback(NULL, 1);
        return;
    }

    CGGameUI::m_reloadUIRequested = 1;
}

// OFFSET: 0x008C57D0/0x140635490
uint32_t CGGameUI::GetInstanceDifficulty() {
    return CGGameUI::m_instanceDifficulty;
}

// OFFSET: 0x008C5E10/0x140635B90
void CGGameUI::CancelMirrorTimers() {
    for (int32_t i = 0; i < 3; i++) {
        FrameScript_SignalEvent(SCRIPT_EVENT_MIRROR_TIMER_STOP, "%s", MirrorTimerTypeToken(i));
        MirrorTimerInfo* info = &CGGameUI::m_mirrorTimerInfo[i];
        info->m_type = 3;
        info->unk_0018 = 0;
        info->m_spellId = 0;
        info->m_paused = 0;
        info->m_scale = 0;
        info->m_maxValue = 0;
        info->m_value = 0;
    }
}

// OFFSET: 0x008C4080
void CGGameUI::OpenResurrectRequest(const char* name) {
    FrameScript_SignalEvent(SCRIPT_EVENT_RESURRECT_REQUEST, "%s", name);
}

// OFFSET: 0x008C59F0
void CGGameUI::SetInstanceBootTime(uint32_t time) {
    if (static_cast<int32_t>(time) <= 0) {
        CGGameUI::m_instanceBootTime = 0;
        FrameScript_SignalEvent(SCRIPT_EVENT_INSTANCE_BOOT_STOP, NULL);
    } else {
        CGGameUI::m_instanceBootTime = time + FrameTime::GetCurTimeMs();
        FrameScript_SignalEvent(SCRIPT_EVENT_INSTANCE_BOOT_START, NULL);
    }
}

// OFFSET: 0x008C5A40
void CGGameUI::SetInstanceLockWarning(uint32_t completedMask) {
    CGGameUI::m_instanceLockCompletedMask = completedMask;
    FrameScript_SignalEvent(SCRIPT_EVENT_INSTANCE_LOCK_WARNING, NULL);
}

// OFFSET: 0x008C5A60
void CGGameUI::SetInstanceLockTime(uint32_t time, uint32_t a2, uint32_t a3) {
    if (static_cast<int32_t>(time) <= 0) {
        CGGameUI::m_instanceLockTime = 0;
        CGGameUI::m_instanceLockCompletedMask = 0;
        CGGameUI::m_instanceLockExtending = 0;
        FrameScript_SignalEvent(SCRIPT_EVENT_INSTANCE_LOCK_STOP, NULL);
    } else {
        CGGameUI::m_instanceLockTime = time + FrameTime::GetCurTimeMs();
        CGGameUI::m_instanceLockCompletedMask = a2;
        CGGameUI::m_instanceLockExtending = a3;
        FrameScript_SignalEvent(SCRIPT_EVENT_INSTANCE_LOCK_START, NULL);
    }
}

// OFFSET: 0x008C5D20
void CGGameUI::InvalidatePlayer(const WOWGUID& guid) {
    CGGuildInfo::InvalidatePlayer(guid.m_guid);
    CGCalendar::InvalidatePlayer(guid);
    CGAutoCompleteName::RefreshName(guid);
}

// OFFSET: 0x008C5EE0
static void LevelGrantNameQueryCallback(int32_t id, const WOWGUID& guid, void* param, bool success) {
    if (!success) {
        return;
    }

    NameCache* name = g_nameDBCache.GetRecord(guid.m_guid, WOWGUID(), NULL, NULL, false);

    if (name) {
        FrameScript_SignalEvent(SCRIPT_EVENT_LEVEL_GRANT_PROPOSED, "%s", name);
    }
}

// OFFSET: 0x008C5F40
void CGGameUI::ProposeLevelGrant(WOWGUID guid) {
    if (CGGameUI::m_pendingLevelGranter) {
        return;
    }

    NameCache* name = g_nameDBCache.GetRecord(guid.m_guid, guid, LevelGrantNameQueryCallback, NULL, false);

    if (name) {
        FrameScript_SignalEvent(SCRIPT_EVENT_LEVEL_GRANT_PROPOSED, "%s", name);
    }

    CGGameUI::m_pendingLevelGranter = guid;
}

// OFFSET: #0x00518700
static int32_t FindMovieFileDataID(int32_t movieId) {
    if (!g_movieDB.GetRecord(movieId)) {
        return -1;
    }

    int32_t best = -1;
    int32_t bestResolution = 0;
    CVar* var = CVar::Lookup("gxResolution");
    const char* resolution;

    if (!var || (resolution = var->m_stringValue.m_str) == NULL) {
        SErrDisplayAppFatalMessage("", "", 0);
    }

    int32_t width;
    int32_t height;
    sscanf(resolution, "%dx%d", &width, &height);

    for (int32_t i = 0; i < g_movieVariationDB.m_numRecords; i++) {
        MovieVariationRec* variation = g_movieVariationDB.GetRecordByIndex(i);

        if (!variation || variation->m_movieID != movieId) {
            continue;
        }

        MovieFileDataRec* fileData = g_movieFileDataDB.GetRecord(variation->m_fileDataID);
        FileDataRec* file = g_fileDataDB.GetRecord(variation->m_fileDataID);

        if (!fileData || !file) {
            continue;
        }

        int32_t current = fileData->m_resolution;

        if (current == width) {
            return variation->m_fileDataID;
        }

        bool better;

        if (best < 0) {
            better = true;
        } else if (bestResolution >= width) {
            better = current < bestResolution;
        } else {
            if (current <= bestResolution) {
                continue;
            }

            better = current < width;
        }

        if (better) {
            best = variation->m_fileDataID;
            bestResolution = fileData->m_resolution;
        }
    }

    return best;
}

// OFFSET: 0x008CB060
void CGGameUI::ClearClientControls() {
    CGGameUI::ClearCursor(1, 1);
    Spell_C_CancelPlayerSpells();
}

// OFFSET: 0x008CB4E0
void CGGameUI::UpdateComboPoints(const WOWGUID& guid, uint32_t points) {
    CGGameUI::m_comboTarget = guid;
    CGGameUI::m_comboPoints = static_cast<uint8_t>(points);
    CGSpellBook::UpdateUsable();
    CGSpellActivationOverlay::UpdateUsable();
    Script_SendUnitSignal(ClntObjMgrGetActivePlayer(), SCRIPT_EVENT_UNIT_COMBO_POINTS);
}

// OFFSET: #0x0051D710
static void PlayMovie() {
    FileDataRec* file = g_fileDataDB.GetRecord(CGGameUI::m_movieFileDataID);

    if (!file) {
        CGGameUI::StopMovie();
    }

    char path[1024];
    SStrPrintf(path, sizeof(path), "%s%s", static_cast<const char*>(file->m_filepath), static_cast<const char*>(file->m_filename));
    char* extension = SStrChrR(path, '.');

    if (extension) {
        *extension = '\0';
    }

    FrameScript_SignalEvent(SCRIPT_EVENT_PLAY_MOVIE, "%s%d", path, CGGameUI::m_movieVolume);
    CGInputControl::GetActive()->EnterWorld();
}

// OFFSET: 0x008D2F80/0x14063C680
void BroadcastUIError(int32_t error, const char* name) {
    switch (error) {
        case 1:
            CGGameUI::DisplayError(ERR_REFER_A_FRIEND_NOT_REFERRED_BY);
            break;
        case 2:
            CGGameUI::DisplayError(ERR_REFER_A_FRIEND_TARGET_TOO_HIGH);
            break;
        case 3:
            CGGameUI::DisplayError(ERR_REFER_A_FRIEND_INSUFFICIENT_GRANTABLE_LEVELS);
            break;
        case 4:
            CGGameUI::DisplayError(ERR_REFER_A_FRIEND_TOO_FAR);
            break;
        case 5:
            CGGameUI::DisplayError(ERR_REFER_A_FRIEND_DIFFERENT_FACTION);
            break;
        case 6:
            CGGameUI::DisplayError(ERR_REFER_A_FRIEND_NOT_NOW);
            break;
        case 7:
            CGGameUI::DisplayError(ERR_REFER_A_FRIEND_GRANT_LEVEL_MAX_I, 79);
            break;
        case 8:
            CGGameUI::DisplayError(ERR_GENERIC_NO_TARGET);
            break;
        case 9:
            CGGameUI::DisplayError(ERR_TARGET_NOT_IN_GROUP_S, name);
            break;
        case 10:
            CGGameUI::DisplayError(ERR_REFER_A_FRIEND_SUMMON_LEVEL_MAX_I, 79);
            break;
        case 11:
            CGGameUI::DisplayError(ERR_REFER_A_FRIEND_SUMMON_COOLDOWN);
            break;
        case 12:
            CGGameUI::DisplayError(ERR_REFER_A_FRIEND_INSUF_EXPAN_LVL);
            break;
        case 13:
            CGGameUI::DisplayError(ERR_REFER_A_FRIEND_SUMMON_OFFLINE_S, name);
            break;
        case 14:
            CGGameUI::DisplayError(ERR_REFER_A_FRIEND_NO_XREALM);
            break;
    }
}

// OFFSET: #0x00522180
void CGGameUI::DisplayPvPQueueRemovedError(int32_t reason) {
    if (!reason) {
        CGGameUI::DisplayError(ERR_REMOVE_FROM_PVP_QUEUE_GRANT_LEVEL);
    } else if (reason == 1) {
        CGGameUI::DisplayError(ERR_REMOVE_FROM_PVP_QUEUE_XP_GAIN);
    } else if (reason == 2) {
        CGGameUI::DisplayError(ERR_REMOVE_FROM_PVP_QUEUE_FACTION_CHANGE);
    }
}

// OFFSET: 0x008CEF10
void CGGameUI::StartMovie(uint32_t movieId) {
    if (!g_movieDB.GetRecord(movieId)) {
        return;
    }

    CGGameUI::m_inMovie = 1;
    CGInputControl* input = CGInputControl::GetActive();
    input->m_mouseModeFlags |= 0x40;
    input->UpdateMouseMode(0);

    if (ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__)) {
        FrameScript_SignalEvent(SCRIPT_EVENT_PLAY_MOVIE, "%d", movieId);
        CGInputControl::GetActive()->EnterWorld();
    }
}

// OFFSET: 0x008D93A0
void CCapturePointUIManager::DeleteNode(CapturePointUIManagerNode* node) {
    uint32_t worldState = node->m_worldState;
    this->m_nodes.DeleteNode(node);

    if (!GetWorldState(worldState)) {
        return;
    }

    for (CapturePointUIManagerNode* other = this->m_nodes.Head(); other; other = this->m_nodes.Next(other)) {
        if (other->m_worldState == worldState) {
            return;
        }
    }

    CGWorldStateInfo::SetWorldState(worldState, 0);

    if (CGWorldMap::m_currentOrphanZone == -1) {
        CGWorldMap::SetMap(CGWorldMap::m_currentContinent, CGWorldMap::m_currentZone, CGWorldMap::m_currentOrphanZone);
    } else {
        CGWorldMap::SetMap(CGWorldMap::m_currentContinent, CGWorldMap::m_currentOrphanZone, CGWorldMap::m_currentDungeonMap);
    }

    FrameScript_SignalEvent(SCRIPT_EVENT_UPDATE_WORLD_STATES, NULL);
}

// OFFSET: 0x008DB0A0/0x14064F220
void CGGameUI::LeaveWorld() {
    if (!CGGameUI::s_inWorld) {
        return;
    }

    CGGameUI::s_inWorld = 0;

    if (CGGameUI::m_inCinematic) {
        FrameScript_SignalEvent(SCRIPT_EVENT_CINEMATIC_STOP, NULL);
        CGWorldFrame::GetActiveCamera()->ClearModelCamera();
        SE3::StopSound(CGGameUI::m_cinematic.m_cameraMusic, 0, __FILE__, __LINE__, "CGGameUI::LeaveWorld");
        SE3::StopSound(CGGameUI::m_cinematic.m_music, 0, __FILE__, __LINE__, "CGGameUI::LeaveWorld");
        CGGameUI::m_inCinematic = 0;
        CGInputControl* input = CGInputControl::GetActive();
        input->m_mouseModeFlags &= ~0x40;
        input->UpdateMouseMode(0);
    }

    CGGameUI::CancelMirrorTimers();
    CGMinimapFrame::LeaveWorld();
    CGChat::LeaveWorld();
    CGActionBar::LeaveWorld(CGGameUI::m_currentlyReloadingUI);
    CGSpellBook::LeaveWorld(CGGameUI::m_currentlyReloadingUI);
    CGCharacterInfo::LeaveWorld();
    CGLootInfo::LeaveWorld();
    CGContainerInfo::LeaveWorld();
    CGItemText::LeaveWorld();
    CGGossipInfo::LeaveWorld();
    CGQuestInfo::LeaveWorld();
    CGQuestLog::LeaveWorld();
    CGTaxiMap::LeaveWorld();
    CGClassTrainer::LeaveWorld();
    CGMerchantInfo::LeaveWorld();
    CGTradeInfo::LeaveWorld();
    CGBankInfo::LeaveWorld();
    CGTradeSkillInfo::ClearItemCallbacks();
    CGPetInfo::LeaveWorld();
    CGWorldMap::LeaveWorld();
    CGReputationInfo::LeaveWorld();
    CGDuelInfo::LeaveWorld();
    CGTabardCreationFrame::LeaveWorld();
    CGGuildRegistrar::CloseRegistrar();
    CGPartyInfo::LeaveWorld(CGGameUI::m_currentlyReloadingUI);
    CGPetitionInfo::LeaveWorld();
    CGMailInfo::LeaveWorld();
    CGBattlefieldInfo::LeaveWorld();
    CGTalentInfo::LeaveWorld();
    CGTalentInfo_Inspect::EnterWorld();
    CGAuctionHouse::CloseAuctionHouse();
    CGStableInfo::LeaveWorld();
    CGMinigameInfo::LeaveWorld();
    CGItemSocketInfo::LeaveWorld();
    CGLCD::LeaveWorld();
    CGLookingForGroup::LeaveLFGSearch();
    g_capturePointUIManager.Clear();
    CGBarberShop::LeaveWorld();
    CGGMSurveyInfo::ShutdownGame();
    CGAchievementInfo::LeaveWorld();
    CGReforge::LeaveWorld();
    CGResearchFrame::LeaveWorld();
    PhaseShift_C_LeaveWorld(CGGameUI::m_currentlyReloadingUI);
    CGWorldStateInfo::LeaveWorld(CGGameUI::m_currentlyReloadingUI);
    CGVoidStorageInfo::LeaveWorld();
    CGWorldFrame::s_currentWorldFrame->SetCameraTarget(NULL, 0);
    CGGameUI::CloseInteraction();

    if (CGGameUI::m_lockedTarget && !CGPartyInfo::IsMemberOrPet(CGGameUI::m_lockedTarget) && !CGRaidInfo::IsPlayerOrPetInRaid(CGGameUI::m_lockedTarget)) {
        CGGameUI::Target(WOWGUID());
    }

    if (CGGameUI::m_focusTarget && !CGPartyInfo::IsMemberOrPet(CGGameUI::m_focusTarget) && !CGRaidInfo::IsPlayerOrPetInRaid(CGGameUI::m_focusTarget)) {
        CGGameUI::SetFocusTarget(WOWGUID());
    }

    CGGameUI::m_lastFriendTarget = WOWGUID();
    CGGameUI::m_lastEnemyTarget = WOWGUID();
    CGGameUI::m_lastTarget = WOWGUID();
    CGGameUI::m_directionalTarget = WOWGUID();
    CGGameUI::m_pendingLevelGranter = WOWGUID();
    CGGameUI::m_instanceLockTime = 0;
    CGGameUI::m_instanceLockCompletedMask = 0;
    CGGameUI::m_instanceLockExtending = 0;
    FrameScript_SignalEvent(SCRIPT_EVENT_INSTANCE_LOCK_STOP, NULL);
    FrameScript_SignalEvent(SCRIPT_EVENT_PLAYER_LEAVING_WORLD, NULL);
}

// OFFSET: 0x008DC580
void CCapturePointUIManager::RemoveCapturePoint(CGGameObject_C* object) {
    if (!object) {
        SErrDisplayAppFatalMessage("", "", 0);
    }

    WOWGUID guid = object->m_obj->m_guid;

    for (CapturePointUIManagerNode* node = this->m_nodes.Head(); node; node = this->m_nodes.Next(node)) {
        if (node->m_guid == guid) {
            this->DeleteNode(node);
            return;
        }
    }
}

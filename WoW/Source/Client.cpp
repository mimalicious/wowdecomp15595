#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "Client.h"
#include "Engine/Source/Base/Base.h"
#include "Engine/Source/Base/CmdLine.h"
#include "Engine/Source/BlizzardCore/OggDecompress.h"
#include "Engine/Source/BlizzardCore/SFile2-Core.h"
#include "Engine/Source/BlizzardCore/Source/Packages/Streaming/Manifests.h"
#include "Engine/Source/BlizzardCore/Source/Packages/Streaming/Streaming.h"
#include "Engine/Source/BlizzardCore/Source/Packages/Streaming/Tracker.h"
#include "Engine/Source/BlizzardCore/Source/Packages/Streaming/WoWReport.h"
#include "Engine/Source/BlizzardCore/Source/Packages/Util/SHA1.h"
#include "Engine/Source/BlizzardCore/Source/System/Time/Time.h"
#include "Engine/Source/ConsoleBase/ConsoleCommand.h"
#include "Engine/Source/ConsoleBase/ConsoleVar.h"
#include "Engine/Source/ConsoleBase/IConsole.h"
#include "Engine/Source/ConsoleBase/Types.h"
#include "Engine/Source/Event/Event.h"
#include "Engine/Source/Frame/CSimpleRender.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameXML/FrameXML.h"
#include "Engine/Source/FrameXML/XMLTree.h"
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "Engine/Source/Gx/Gx.h"
#include "Engine/Source/Gxu/GxuLight.h"
#include "Engine/Source/lua-5.1/src/lua.h"
#include "Engine/Source/Model2/M2Cache.h"
#include "Engine/Source/Os/Gui.h"
#include "Engine/Source/Os/Input.h"
#include "Engine/Source/Os/Joystick.h"
#include "Engine/Source/Os/OsFile.h"
#include "Engine/Source/Os/OsIME.h"
#include "Engine/Source/Os/Path.h"
#include "Engine/Source/Os/System.h"
#include "Engine/Source/Os/Time.h"
#include "Engine/Source/Os/TimeManager.h"
#include "Engine/Source/Scrn/Scrn.h"
#include "Engine/Source/Services/AsyncFile.h"
#include "Engine/Source/Services/ModelBlob.h"
#include "Engine/Source/Services/Profile.h"
#include "Engine/Source/Services/SysMessage.h"
#include "Engine/Source/Services/Texture.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/Storm/Source/MD5.h"
#include "Engine/Source/Tempest/CRandom.h"
#include "WoW/Common/ComponentCore/CharacterComponent.h"
#include "WoW/Source/Console/ConsoleClient.h"
#include "WoW/Source/ConsoleDevice/ConsoleDevice.h"
#include "WoW/Source/DB/DBClient/DBCacheInstances.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/Cfg_ConfigsRec.h"
#include "WoW/Source/DB/Rec/MapRec.h"
#include "WoW/Source/Game/DeclinedWords.h"
#include "WoW/Source/Game/GameClient/ComSatClient.h"
#include "WoW/Source/Game/GameClient/PlayerName.h"
#include "WoW/Source/Game/GameClient/WardenClient.h"
#include "WoW/Source/Game/GameClient/WTOBJECT.h"
#include "WoW/Source/Game/GameTime.h"
#include "WoW/Source/Game/ValidateName.h"
#include "WoW/Source/GameClientCommands.h"
#include "WoW/Source/Glue/CGlueMgr.h"
#include "WoW/Source/Glue/GlueMgr.h"
#include "WoW/Source/Hardware.h"
#include "WoW/Source/JamClientAutoCode/JamClient.h"
#include "WoW/Source/JamClientAutoCode/JamClientConnection.h"
#include "WoW/Source/JamClientAutoCode/JamUserClient.h"
#include "WoW/Source/LoadingScreen.h"
#include "WoW/Source/Magic/MagicClient/Spell_C.h"
#include "WoW/Source/Magic/MagicClient/SpellVisuals.h"
#include "WoW/Source/Net/NetClient/NetClient.h"
#include "WoW/Source/Object/ObjectClient/AnimKitManager.h"
#include "WoW/Source/Object/ObjectClient/AnimReplacementSet.h"
#include "WoW/Source/Object/ObjectClient/Container_C.h"
#include "WoW/Source/Object/ObjectClient/Corpse_C.h"
#include "WoW/Source/Object/ObjectClient/DynamicObject_C.h"
#include "WoW/Source/Object/ObjectClient/Effect_C.h"
#include "WoW/Source/Object/ObjectClient/GameObject_C.h"
#include "WoW/Source/Object/ObjectClient/Item_C.h"
#include "WoW/Source/Object/ObjectClient/LootRoll.h"
#include "WoW/Source/Object/ObjectClient/Missile_C.h"
#include "WoW/Source/Object/ObjectClient/Movement_C.h"
#include "WoW/Source/Object/ObjectClient/ObjectEffect.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/Object/ObjectClient/Unit_C.h"
#include "WoW/Source/Object/ObjectClient/VoidStorage_C.h"
#include "WoW/Source/ObjectAlloc/ObjectAlloc.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/SoundInterface/SI3.h"
#include "WoW/Source/Ui/AddOns.h"
#include "WoW/Source/Ui/BarberShop.h"
#include "WoW/Source/Ui/BattlenetUI.h"
#include "WoW/Source/Ui/ChatFrame.h"
#include "WoW/Source/Ui/GameUI.h"
#include "WoW/Source/Ui/LootFrame.h"
#include "WoW/Source/Ui/PartyFrame.h"
#include "WoW/Source/Ui/ScriptEvents.h"
#include "WoW/Source/Ui/Tutorial.h"
#include "WoW/Source/Ui/Types.h"
#include "WoW/Source/Ui/WorldFrame.h"
#include "WoW/Source/UIUtil/BannedAddOns.h"
#include "WoW/Source/UIUtil/Camera.h"
#include "WoW/Source/UIUtil/CameraManager.h"
#include "WoW/Source/UIUtil/InputControl.h"
#include "WoW/Source/UIUtil/SmartScreenRect.h"
#include "WoW/Source/WorldClient/AreaList.h"
#include "Engine/Source/Event/FrameTime.h"
#include "WoW/Source/WorldClient/Map.h"
#include "WoW/Source/WorldClient/MapWeather.h"
#include "WoW/Source/WorldClient/shadow.h"
#include "WoW/Source/WorldClient/World.h"
#include "WoW/Source/WorldClient/WorldParam.h"
#include "WoW/Source/WowServices/PatchFiles.h"
#include "WoW/Source/WowServices/RealmConnection.h"
#include "WoW/Source/WowServices/WDataStore.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/AccountData.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/FriendList.h"
#include "WoW/Source/WowSysMessageOutput.h"
#include "Engine/Source/Base/ConvertUTF.h"
#include "Engine/Source/BlizzardCore/BackgroundCache.h"
#include "Engine/Source/BlizzardCore/BGLoading-Core.h"
#include "Engine/Source/BlizzardCore/Source/System/File/Filestack_Streaming.h"
#include "Engine/Source/Os/Error.h"
#include "Engine/Source/Os/Locale.h"
#include "Engine/Source/Services/PreloadList.h"
#include "WoW/Source/WowServices/InstallLocation.h"
#include "Engine/Source/FFX/FFX.h"
#include "Engine/Source/Os/Processor.h"
#include "Engine/Source/ShaderEffect/DefaultShaders.h"
#include "WoW/Source/Magic/MagicClient/SpellTable.h"
#include "WoW/Source/Game/GameClient/PhaseShift_C.h"
#include "WoW/Source/DB/WowClientDB.h"
#include "Engine/Source/Frame/CSimpleTexture.h"
#include "Engine/Source/Os/SockAddr.h"

namespace {

struct ArchivePreload {
    /* 0000 */ TSLink<ArchivePreload> m_link;
    /* 0008 */ HOSFILE m_lockFile;
    /* 000C */ HOSFILE m_tempFile;
    /* 0010 */ char m_path[260];
    /* 0114 */ char m_tempPath[260];
    /* 0218 */ char m_lockPath[260];
    /* 0320 */ int64_t m_remaining;
    /* 0328 */ uint32_t m_expansion;
};

struct ArchiveEnumParams {
    /* 0000 */ uint32_t m_index;
    /* 0004 */ int32_t m_priority;
    /* 0008 */ const char* m_locale;
};

struct SignaturePrefix {
    /* 0000 */ const char* m_prefix;
    /* 0004 */ uint32_t m_length;
};

class ForEveryRunOnceWTF {
    public:
    /* 0000 */ TSGrowableArray<char*> m_files;

    static int32_t AppendFileName(OS_FILE_DATA& data, void* param);
    static int FileNameCompare(const void* a, const void* b);

    void Execute(int32_t (*callback)(const char*));
};

}

// STATIC: 0x00B88080
static const _ARGLIST s_wowArgList[] = {
    { 0, CMD_RES_800x600, "800x600", NULL },
    { 0, CMD_RES_1024x768, "1024x768", NULL },
    { 0, CMD_RES_1280x960, "1280x960", NULL },
    { 0, CMD_RES_1280x1024, "1280x1024", NULL },
    { 0, CMD_RES_1600x1200, "1600x1200", NULL },
    { 0, CMD_16_BIT, "16bit", NULL },
    { 0, CMD_UP_TO_DATE, "uptodate", NULL },
    { 0, CMD_NO_SOUND, "nosound", NULL },
    { 0, CMD_SOUND_CHAOS, "soundchaos", NULL },
    { 0, CMD_NO_FIX_LAG, "nofixlag", NULL },
    { 0, CMD_DEPTH_16, "d16", NULL },
    { 0, CMD_DEPTH_24, "d24", NULL },
    { 0, CMD_DEPTH_32, "d32", NULL },
    { 0, CMD_WINDOWED, "windowed", NULL },
    { 0, CMD_HW_DETECT, "hwdetect", NULL },
    { 0, CMD_CONSOLE, "console", NULL },
    { 0x20000, CMD_GX_OVERRIDE, "gxoverride", NULL },
    { 0x20000, CMD_CONFIG, "config", NULL },
    { 0, CMD_LAUNCH, "launch", NULL },
};

// STATIC: 0x00C97004/0x140B2E204
static int32_t s_allArchivesComplete = 1;

// STATIC: 0x00C97238/0x140B2E260
static ARCHIVEINFO s_archives[19] = {
    { "art.MPQ", NULL, 0, 0x0, 0 },
    { "world.MPQ", NULL, 0, 0x0, 0 },
    { "sound.MPQ", NULL, 0, 0x0, 0 },
    { "****\\locale-****.MPQ", NULL, 0, 0x0, 0 },
    { "****\\speech-****.MPQ", NULL, 0, 0x0, 0 },
    { "expansion1.MPQ", NULL, 0, 0x0, 1 },
    { "****\\expansion1-locale-****.MPQ", NULL, 0, 0x0, 1 },
    { "****\\expansion1-speech-****.MPQ", NULL, 0, 0x0, 1 },
    { "expansion2.MPQ", NULL, 0, 0x0, 2 },
    { "****\\expansion2-locale-****.MPQ", NULL, 0, 0x0, 2 },
    { "****\\expansion2-speech-****.MPQ", NULL, 0, 0x0, 2 },
    { "expansion3.MPQ", NULL, 0, 0x0, 3 },
    { "****\\expansion3-locale-****.MPQ", NULL, 0, 0x0, 3 },
    { "****\\expansion3-speech-****.MPQ", NULL, 0, 0x0, 3 },
    { "development.MPQ", NULL, 0, 0x5, 0 },
    { "development-locale-****.MPQ", NULL, 0, 0x5, 0 },
    { "signaturefile.MPQ", NULL, 0, 0x1, 0 },
    { "alternate.MPQ", "alternate", 0, 0x3, 0 },
    { "world2.MPQ", NULL, 0, 0x0, 0 },
};
// STATIC: 0x00B87400
static const uint8_t s_signatureExponent[4] = { 0x01, 0x00, 0x01, 0x00 };

// STATIC: 0x00C97008/0x140B2E108
static const uint8_t s_signatureModulus[256] = {
    0x4F, 0x6F, 0xD4, 0x65, 0x56, 0x85, 0x88, 0xD9, 0xAA, 0xE6, 0xF7, 0x05, 0x72, 0xBB, 0x2A, 0x20,
    0xE0, 0x24, 0x38, 0x2B, 0x77, 0xB3, 0x3D, 0xC2, 0xD3, 0x2D, 0x52, 0x5C, 0xB7, 0xA3, 0x0F, 0xE6,
    0x0F, 0x80, 0x93, 0x8D, 0x5C, 0xC4, 0x81, 0x5E, 0x9D, 0x58, 0x8E, 0xCA, 0xA6, 0x6F, 0x27, 0x36,
    0x52, 0x58, 0x0D, 0xCE, 0x65, 0xE2, 0xB5, 0x7A, 0xD2, 0x39, 0x74, 0x8A, 0xAC, 0xAA, 0xC1, 0x0E,
    0xE5, 0x67, 0x2B, 0x79, 0xC1, 0xFE, 0x08, 0x96, 0x54, 0x18, 0x2A, 0x34, 0x31, 0xA4, 0x06, 0x20,
    0x4B, 0x12, 0xD2, 0x47, 0x63, 0x53, 0x36, 0x38, 0x19, 0x88, 0x8E, 0x50, 0x2F, 0x2F, 0xD1, 0x68,
    0xC8, 0x5C, 0x9D, 0x3B, 0xDE, 0xFE, 0x56, 0x21, 0xD1, 0x59, 0x4D, 0x1C, 0x32, 0x1B, 0x70, 0x4B,
    0x40, 0xE2, 0x5E, 0x0C, 0x0E, 0x4B, 0x84, 0xD5, 0xD7, 0x52, 0x00, 0x50, 0x65, 0x81, 0xCE, 0x17,
    0x53, 0x25, 0x49, 0x9E, 0x40, 0xE9, 0x04, 0x94, 0x2C, 0xA7, 0x93, 0x5C, 0x5D, 0x54, 0xC8, 0xF9,
    0x6E, 0xD0, 0x69, 0xFC, 0xE5, 0x8F, 0xC0, 0xB6, 0x76, 0xF2, 0xEB, 0x34, 0x67, 0x9F, 0xF8, 0xD0,
    0x34, 0xB9, 0xEA, 0x9E, 0x80, 0xD0, 0x29, 0x8D, 0xB4, 0xB2, 0xEB, 0x3E, 0xF4, 0x4F, 0xD0, 0x84,
    0x43, 0x15, 0xF4, 0x19, 0xE3, 0xC6, 0x52, 0xA2, 0x18, 0x40, 0x08, 0x07, 0x21, 0xF4, 0x20, 0xEC,
    0xE6, 0x5A, 0x9C, 0x2B, 0x08, 0xF6, 0x17, 0x97, 0x72, 0xA6, 0xD7, 0xAB, 0x64, 0x0B, 0x04, 0x95,
    0x93, 0x0D, 0xCC, 0x8F, 0x02, 0x30, 0x91, 0xA2, 0xEE, 0x30, 0xE6, 0x5F, 0x2F, 0xD4, 0x13, 0xB5,
    0x5D, 0xCA, 0x5D, 0xCF, 0x7D, 0xC3, 0x6F, 0x6B, 0x9D, 0xBC, 0x6D, 0x86, 0x4C, 0x91, 0xC5, 0x51,
    0xE8, 0xFD, 0xA6, 0x33, 0x10, 0xD1, 0x12, 0x8F, 0x05, 0x80, 0xDA, 0x42, 0x57, 0x0C, 0xEB, 0xC0,
};
// STATIC: 0x00D1F1F8
const char* s_raceFileNames[24] = { NULL, "Human", "Orc", "Dwarf", "NightElf", "Scourge", "Tauren", "Gnome", "Troll", "Goblin", "BloodElf", "Draenei", "FelOrc", "Naga_", "Broken", "Skeleton", "Vrykul", "Tuskarr", "ForestTroll", "Taunka", "NorthrendSkeleton", "IceTroll", "Worgen", "Gilnean" };

// STATIC: 0x00D1F258
const char* s_classFileNames[12] = { NULL, "WARRIOR", "PALADIN", "HUNTER", "ROGUE", "PRIEST", "DEATHKNIGHT", "SHAMAN", "MAGE", "WARLOCK", NULL, "DRUID" };

// STATIC: 0x00C97230/0x140B2E208
static const char* s_dataDirectory = "Data\\";

// STATIC: 0x00C97234/0x140B2E258
static uint32_t s_archiveOpenFlags = 0x9E48;

// STATIC: 0x00C9753C
static const SignaturePrefix s_signaturePrefixes[1] = { { "alternate\\", 10 } };

// STATIC: 0x00C97530
static TSExplicitList<ArchivePreload, offsetof(ArchivePreload, m_link)> s_archivePreloads;

// STATIC: 0x00C97570
static char s_installLocationRegKeyName[128] = "World of Warcraft";

// STATIC: 0x00D36358
int32_t s_region;

// STATIC: 0x00D3635C
static int32_t s_showArchiveProgress;

// STATIC: 0x00D36360
static int64_t s_placeholderArchiveSize;

// STATIC: 0x00D36368
static char s_placeholderArchivePath[260];

// STATIC: 0x00D3646C
static int32_t s_engineInitialized;

// STATIC: 0x00D364AC
static uint8_t* s_preloadBuffer;

// STATIC: 0x00D364B0
static int32_t s_startBGLoadThread;

// STATIC: 0x00D364CC
static uint32_t s_numUpdateArchives;

// STATIC: 0x00ED742C
PreloadList* s_startWorldCommonList;

// STATIC: #0x00AB6350
WowClientDB<Startup_StringsRec> g_startup_StringsDB;

// STATIC: #0x00AD2FE0
const char* s_iniLocaleNames[9] = { "enUS", "koKR", "frFR", "deDE", "zhCN", "zhTW", "esES", "esMX", "ruRU" };

// STATIC: 0x00C9A9CC
uint32_t s_localeIds[15] = { 'deDE', 'enGB', 'enUS', 'esES', 'frFR', 'koKR', 'zhCN', 'zhTW', 'enCN', 'enTW', 'esMX', 'ruRU', 'enUS', 'ptBR', 'ptPT' };

// STATIC: #0x00B2F9E1
uint8_t s_expansionLevel;

// STATIC: #0x00B2FA18
static uint32_t s_firstBaseArchive;

// STATIC: 0x00D364E8
CRndSeed g_rndSeed(0);

// STATIC: 0x00D364D4
static TSGrowableArray<SArchive*> s_archiveHandles;

// STATIC: #0x00B2FE48
static char s_iniLocale[5];
// STATIC: #0x00AB6128
static const EGxTexFilter s_textureFilterModes[] = { GxTex_LinearMipNearest, GxTex_LinearMipLinear, GxTex_Anisotropic, GxTex_Anisotropic, GxTex_Anisotropic, GxTex_Anisotropic };

// STATIC: #0x00AB6140
static const uint32_t s_textureMaxAnisotropy[] = { 1, 1, 2, 4, 8, 16 };

// STATIC: #0x00B2F050
static uint32_t s_streamingDiag;

// STATIC: #0x00B2F059
static char s_streamingError[2051];

// STATIC: 0x00D36334
HEVENTCONTEXT g_clientEventContext;

// STATIC: #0x00B2F9A4
static int32_t s_closeErrorStringId;

// STATIC: 0x00D364A8
static uint32_t s_timeTestError;

// STATIC: 0x00D364C8/0x140BEF9EC
static uint32_t s_unk_B2FA10;

// STATIC: 0x00D36354/0x140BEF888
static char* s_mfilDownloadErrors;

// STATIC: #0x00B2FA48
static char s_installLocation[1024];

// STATIC: 0x00D732EC
bool g_hasIsoLocale[15];

// STATIC: 0x00D732FC/0x140DABA80
int32_t s_locale;

// STATIC: #0x00B2F040
static const char* s_directory;

// STATIC: #0x00B2F044
static float s_spawnRotation;

// STATIC: #0x00B2F048
static uint32_t s_zoneID;

// STATIC: 0x00D36350
int32_t s_isPvPRealm;

// STATIC: 0x00D36470
static int32_t s_gameInitialized;

// STATIC: #0x00B2F9A8
static int32_t s_clientTaxiBenchIsOnTaxi;

// STATIC: #0x00B2F9AC
static int32_t s_clientTaxiBenchEnabled;

// STATIC: #0x00B2F9B0
static double s_clientTaxiBenchMinFPS;

// STATIC: #0x00B2F9B8
static double s_clientTaxiBenchMaxFPS;

// STATIC: #0x00B2F9C0
static double s_clientTaxiBenchTotalTime;

// STATIC: #0x00B2F9C8
static uint32_t s_clientTaxiBenchTotalFrames;

// STATIC: #0x00B2F9CC
static uint32_t s_clientTaxiBenchDelayTime;

// STATIC: #0x00B2F9D0
static uint64_t s_clientTaxiTimestamp;

// STATIC: #0x00B2F9E0
uint8_t s_botCheckEnabled;

// STATIC: #0x00B2F9E4
static uint32_t s_botCheckStartTime;

// STATIC: #0x00B2F9E8
int32_t s_botCheckDelay;

// STATIC: #0x00B2FA0C
volatile long s_additionalCheckArmed;

// STATIC: #0x00B2FA1C
static C3Vector s_spawnPosition;

// STATIC: 0x00D36518
template <>
CommandManager* StaticSingleton<CommandManager>::m_instance = NULL;

// OFFSET: 0x00402580/0x140002080
static int32_t ListArchiveFilter(const char* fileName, void* param) {
    ARCHIVELISTPARAMS* params = static_cast<ARCHIVELISTPARAMS*>(param);

    if (SStrCmpI(fileName, params->m_prefix, params->m_prefixLength)) {
        return 1;
    }

    return params->m_callback(fileName, params->m_param);
}

// OFFSET: 0x00401FF0
static bool GetArchivePath(char* buffer, uint32_t size, const char* name, const char* locale) {
    SStrCopy(buffer, s_dataDirectory, size);
    SStrPack(buffer, name, size);

    for (char* token = strstr(buffer, "****"); token; token = strstr(token, "****")) {
        *reinterpret_cast<uint32_t*>(token) = *reinterpret_cast<const uint32_t*>(locale);
    }

    return true;
}

// OFFSET: 0x00402110
static void CheckAvailableLocales() {
    for (int32_t i = 0; i < 15; i++) {
        char path[1024];
        g_hasIsoLocale[i] = false;
        GetArchivePath(path, sizeof(path), s_archives[3].m_name, g_localeNames[i]);

        if (OsFileExists(path)) {
            g_hasIsoLocale[i] = true;
        }
    }
}

// OFFSET: 0x00402170/0x140001A90
static bool FormatArchiveError(char* buffer, uint32_t size, const char* initErrors, const char* errors, const char* stormMessage) {
    bool allowRetry = true;
    int32_t error = SErrGetLastError();
    const char* text;

    if (error > 2) {
        if (error == 5) {
            text = "Cannot make local cache for streaming. The folder may be read only. Please check and try again.";
            allowRetry = false;
        } else if (error == 112) {
            text = "Disk is full. User is already prompted.";
        } else {
            text = "Required archive failure.";
        }
    } else if (error == 2) {
        if (SFile::IsStreamingDisabled()) {
            text = SFile::IsStreamingDataReady() ? "The MPQ on the streaming server may be missing, invalid or corrupt.  Please try another build." : "Failed to initialize streaming.  Please check WoW.mfil.";
        } else if (SFile::IsStreamingDataReady()) {
            text = "The MPQ on the streaming server may be missing, invalid or corrupt.  Please try another build.";
        } else {
            text = "Failed to initialize streaming.  Please check your network connection.";
        }
    } else if (error == static_cast<int32_t>(0x8510006C)) {
        text = "The MPQ on the streaming server may be missing, invalid or corrupt.  Please try another build.";
    } else if (error == static_cast<int32_t>(0x85100094)) {
        text = "Cannot download data due to connectivity issue. User is already prompted.";
    } else if (error == static_cast<int32_t>(0x85100095)) {
        if (SFile::IsStreamingDisabled()) {
            text = "Failed to open a required archive because streaming is disabled.";
        } else if (!SFile::IsStreamingMfilReady()) {
            text = "Failed to open a required archive because WoW.mfil failed to download.";
        } else if (SFile::IsStreamingDataReady()) {
            text = "Cannot stream archive due to connectivity issue. User is already prompted.";
        } else {
            text = "Failed to open a required archive because streaming failed to initialize.";
        }
    } else {
        text = "Required archive failure.";
    }

    const char* mfilErrors = s_mfilDownloadErrors ? s_mfilDownloadErrors : "None";
    const char* dataStatus = SFile::IsStreamingDataReady() ? "Data Rdy" : "Data Not Rdy";
    const char* mfilStatus = SFile::IsStreamingMfilReady() ? "Mfil Rdy" : "Mfil Not Rdy";
    const char* streaming = SFile::IsStreamingDisabled() ? "Disabled" : "Enabled";
    SStrPrintf(buffer, size, "%s\n\nDetails: (Streaming Status: %s / %s / %s, Build: %d)\nLast error code: %d\nCurrent error stack: \n%sStreaming init error stack: \n%sWow.mfil download error: %s\nStorm Error Msg:%s", text, streaming, mfilStatus, dataStatus, 15595, error, errors, initErrors, mfilErrors, stormMessage);
    return allowRetry;
}

// OFFSET: 0x004022D0
static int64_t GetStreamedArchiveSize() {
    int64_t total = 0;

    for (uint32_t i = 0; i < sizeof(s_archives) / sizeof(s_archives[0]); i++) {
        char path[260];
        GetArchivePath(path, sizeof(path), s_archives[i].m_name, s_localeCode);
        Blizzard::Streaming::GetStreamedFileSize(path, &s_archives[i].m_size);
        total += s_archives[i].m_size;
    }

    return total;
}

// OFFSET: 0x00402340/0x140001DC0
static void ShowArchiveError(uint32_t error) {
    wchar_t message[260];

    if ((error != 112 && (error <= 0x85100093 || error > 0x85100095)) || (!SErrGetErrorStrW(error, message, 260, OsGetUserDefaultLangID()) && !SErrGetErrorStrW(error, message, 260, OsGetLangIdFromLocale(s_localeIds[GetCurrentLocaleIndex()])))) {
        SErrDisplayAppFatal("Unknown error code : %u\n", error);
    }

    char text[260];
    ConvertUTF16toUTF8(text, sizeof(text), reinterpret_cast<const uint16_t*>(message), 260, NULL, NULL);
    OsGuiMessageBox(NULL, 1, text, NULL);
}

// OFFSET: 0x00402400/0x140001E90
static void ReportArchiveFailure(const char* name) {
    uint32_t error = SErrGetLastError();
    const char* errors = SFile::GetErrorDetails();
    char stormMessage[256];
    SErrGetErrorStr(SErrGetLastError(), stormMessage, sizeof(stormMessage));

    if (SFile::IsStreamingDisabled() || !SFile::IsStreamingMfilReady() || !SFile::IsStreamingDataReady()) {
        error = 0x85100095;
    }

    if (!_stricmp(ClientServices::GetPatchListURL(), "streaming.corp.blizzard.net/patchlist/ldap.php")) {
        char report[2048];

        if (FormatArchiveError(report, sizeof(report), SFile::GetStreamingStatusText(), errors, stormMessage)) {
            SFile::SendErrorReport(report, true);
        }

        if (error == 112) {
            ShowArchiveError(error);
            return;
        }

        const char* mfilErrors = s_mfilDownloadErrors ? s_mfilDownloadErrors : "None";
        const char* dataStatus = SFile::IsStreamingDataReady() ? "Data Rdy" : "Data Not Rdy";
        const char* mfilStatus = SFile::IsStreamingMfilReady() ? "Mfil Rdy" : "Mfil Not Rdy";
        const char* streaming = SFile::IsStreamingDisabled() ? "Disabled" : "Enabled";
        SErrDisplayAppFatal("Failed to open required archive %s with error: %d\n\n(Streaming Status: %s / %s / %s, Build: %d)\n\nWow.mfil download error: %s\n\nDebug Details:\n\n%s\nStorm Error Msg:%s", name, error, streaming, mfilStatus, dataStatus, 15595, mfilErrors, errors, stormMessage);
    }

    if (error == 112 || (error > 0x85100093 && error <= 0x85100095)) {
        ShowArchiveError(error);
        return;
    }

    SErrDisplayAppFatal("Failed to open required archive %s with error : %d \n\nStreaming Status: Data Rdy\n\nWow.mfil download error: %s\n\nDebug Details:\n\n%sStorm Error Msg:%s", name, error, s_mfilDownloadErrors ? s_mfilDownloadErrors : "None", errors, stormMessage);
}

// OFFSET: 0x00402D90/0x1400022E0
static void LoadInstallLocation() {
    uint32_t size = 0;
    char* data = GetComponentVersionData(&size);

    if (!data) {
        return;
    }

    XMLTree* tree = XMLTree_Load(data, size);

    if (tree) {
        if (XMLTree_GetRoot(tree)->m_child) {
            XMLNode* product = XMLTree_GetRoot(tree)->m_child->GetChildByName("product");

            if (product) {
                SetInstallLocationByName(product->m_body);
            }
        }

        XMLTree_Free(tree);
    }

    XMLTree_Cleanup();
    delete[] data;
}

// OFFSET: 0x00402E10/0x140005FF0
void StartupReport() {
    if (SFile::IsTrial()) {
        uint32_t speedTest;

        if (!SRegLoadValue("World of Warcraft Trial", "SpeedTest", 1, &speedTest)) {
            speedTest = 0;
        }

        Tracker::SetLocale(s_localeCode);
        Tracker::SetBuildnum(15595);
    }

    char version[16];
    SStrPrintf(version, sizeof(version), "%d", 15595);
    s_cvarEnterWorld = CVar::Register("enterWorld", "Trial Client entered world", 0, "0", NULL, CATEGORY_GAME, false, NULL, false);
    bool trial = SFile::IsTrial() == 1;
    WoWReport::Initialize(version, s_localeCode, s_cvarEnterWorld->m_intValue != 0, trial);
}

// OFFSET: 0x00402ED0/0x140002380
static void AddMfilDownloadError(const char* format, ...) {
    char text[1024];
    va_list args;
    va_start(args, format);
    SStrVPrintf(text, sizeof(text), format, args);
    va_end(args);

    if (!s_mfilDownloadErrors) {
        s_mfilDownloadErrors = static_cast<char*>(SMemAlloc(16192, __FILE__, __LINE__, 0));
        *s_mfilDownloadErrors = '\0';
    }

    SStrPack(s_mfilDownloadErrors, text, 16192);
    SStrPack(s_mfilDownloadErrors, "\r\n", 16192);
}

// OFFSET: 0x00405320/0x140005CC0
const char* GetInstallLocationRegKeyName() {
    if (GetInstallLocation() == INSTALL_LOCATION_TEST) {
        SStrPack(s_installLocationRegKeyName, "\\", sizeof(s_installLocationRegKeyName));
        SStrPack(s_installLocationRegKeyName, "PTR", sizeof(s_installLocationRegKeyName));
    } else if (GetInstallLocation() == INSTALL_LOCATION_BETA) {
        SStrPack(s_installLocationRegKeyName, "\\", sizeof(s_installLocationRegKeyName));
        SStrPack(s_installLocationRegKeyName, "Beta", sizeof(s_installLocationRegKeyName));
    }

    return s_installLocationRegKeyName;
}

// OFFSET: 0x00405450
void UpdateInstallLocation() {
    const char* key = GetInstallLocationRegKeyName();
    char path[1560];
    GetExecutablePathUTF8(path, sizeof(path));

    if (!path[0]) {
        return;
    }

    char* slash = strrchr(path, '\\');

    if (slash) {
        slash[1] = '\0';
        SRegSaveString(key, "InstallPath", 4, path);
    }
}

// OFFSET: 0x004054B0/0x140005E50
static bool GetMfilBuild(int32_t* build) {
    HOSFILE file = OsCreateFile("WoW.mfil", OS_GENERIC_READ, 0, OS_OPEN_EXISTING, OS_FILE_ATTRIBUTE_NORMAL, OS_FILE_TYPE_ANY);

    if (file == OS_INVALID_FILE) {
        return false;
    }

    bool found = false;
    uint64_t size = OsGetFileSize(file);

    if (size - 1 <= 0x3F3E) {
        char* data = static_cast<char*>(SMemAlloc(static_cast<uint32_t>(size) + 1, __FILE__, __LINE__, 0));
        uint32_t bytes;

        if (OsReadFile(file, data, static_cast<uint32_t>(size), &bytes) && bytes == size) {
            data[size] = '\0';
            char key[256];
            SStrPrintf(key, sizeof(key), "manifest_partial=%s-", GetInstallLocationProgram());
            char* match = SStrStrI(data, key);

            if (match || (SStrPrintf(key, sizeof(key), "manifest_partial=WoW-"), (match = SStrStrI(data, key)) != NULL)) {
                found = true;
                size_t length = strlen(key);
                match[length + 5] = '\0';
                *build = atoi(match + length);
            }
        }

        SMemFree(data, __FILE__, __LINE__, 0);
    }

    OsCloseFile(file);
    return found;
}

// OFFSET: 0x00405940/0x1400060D0
static bool ParsePatchRecord(const char* data, const char* program, const char* component, char* url, uint32_t urlSize, char* mfilName, uint32_t mfilSize) {
    XMLTree* tree = XMLTree_Load(data, strlen(data));

    if (tree) {
        XMLNode* root = XMLTree_GetRoot(tree);
        XMLNode* record;

        if (!_stricmp(root->m_name.m_str, "patch") && (record = root->GetChildByName("record")) != NULL) {
            for (; record; record = record->m_next) {
                if (_stricmp(record->GetAttributeByName("program"), program) || _stricmp(record->GetAttributeByName("component"), component)) {
                    continue;
                }

                const char* body = record->m_body;
                char md5[260];
                char type[260];
                char build[10];
                SStrTokenize(&body, url, urlSize, ";\n", NULL);
                SStrTokenize(&body, md5, sizeof(md5), ";\n", NULL);
                SStrTokenize(&body, type, sizeof(type), ";\n", NULL);
                SStrTokenize(&body, build, sizeof(build), ";\n", NULL);

                char name[260];

                if (!_stricmp(type, "internal")) {
                    SStrCopy(name, "WoW", sizeof(name));
                } else {
                    SStrCopy(name, program, sizeof(name));
                    _strlwr(name);
                }

                SStrPrintf(mfilName, mfilSize, "%s-%s-%s.mfil", name, build, type);
                XMLTree_Free(tree);
                XMLTree_Cleanup();
                return true;
            }
        }

        XMLTree_Free(tree);
    }

    XMLTree_Cleanup();
    return false;
}

// OFFSET: 0x00405B20/0x140006350
static bool ParseServerConfig(const char* product, const char* data, char* servers, uint32_t size) {
    XMLTree* tree = XMLTree_Load(data, strlen(data));

    if (tree) {
        XMLNode* root = XMLTree_GetRoot(tree);

        if (!_stricmp(root->m_name.m_str, "config")) {
            XMLNode* versionInfo = root->GetChildByName("versioninfo");

            if (versionInfo) {
                for (XMLNode* version = versionInfo->m_child; version && !_stricmp(version->m_name.m_str, "version"); version = version->m_next) {
                    if (_stricmp(version->GetAttributeByName("product"), product)) {
                        continue;
                    }

                    bool found = false;
                    XMLNode* serverList = version->GetChildByName("servers");

                    if (serverList) {
                        XMLNode* server = serverList->m_child;

                        while (server) {
                            if (!_stricmp(server->m_name.m_str, "server")) {
                                const char* id = server->GetAttributeByName("id");
                                const char* url = server->GetAttributeByName("url");

                                if (id && url) {
                                    found = true;
                                    char line[260];
                                    SStrPrintf(line, sizeof(line), "server=%s\n\tlocation=%s\n", id, url);
                                    SStrPack(servers, line, size);
                                }

                                server = server->m_next;
                            }
                        }
                    }

                    XMLTree_Free(tree);
                    XMLTree_Cleanup();
                    return found;
                }
            }
        }

        XMLTree_Free(tree);
    }

    XMLTree_Cleanup();
    return false;
}

// OFFSET: 0x00405CC0/0x140006550
static bool IsMfilCurrent() {
    HOSFILE file = OsCreateFile("WoW.mfil", OS_GENERIC_READ, 0, OS_OPEN_EXISTING, OS_FILE_ATTRIBUTE_NORMAL, OS_FILE_TYPE_ANY);

    if (file == OS_INVALID_FILE) {
        return false;
    }

    char* data = NULL;
    bool current = false;
    uint64_t size = OsGetFileSize(file);

    if (size - 1 <= 0x3F3E) {
        data = static_cast<char*>(SMemAlloc(static_cast<uint32_t>(size) + 1, __FILE__, __LINE__, 0));
        uint32_t bytes;

        if (OsReadFile(file, data, static_cast<uint32_t>(size), &bytes) && bytes == size) {
            data[size] = '\0';
            char key[256];
            SStrPrintf(key, sizeof(key), "manifest_partial=%s-%d", GetInstallLocationProgram(), 15595);

            if (SStrStrI(data, key) || (SStrPrintf(key, sizeof(key), "manifest_partial=WoW-%d-internal", 15595), SStrStrI(data, key))) {
                current = true;
            }
        }
    }

    SMemFree(data, __FILE__, __LINE__, 0);
    OsCloseFile(file);
    return current;
}

// OFFSET: 0x00406260/0x1400077D0
static bool DownloadMfil(const char* patchListURL, const char* locale, uint32_t timeout) {
    char url[260];
    SStrCopy(url, "http://", sizeof(url));
    SStrPack(url, patchListURL, sizeof(url));

    const char* status = NULL;
    const char* program = GetInstallLocationProgram();
    char request[512];
    SStrPrintf(request, sizeof(request), "<version program=\"WoW\"><record program=\"Bnet\" component=\"Win\" version=\"1\" /><record program=\"%s\" component=\"%s\" version=\"%d\" build=\"%d\" /></version>", program, locale, 2 - (GetInstallLocation() != INSTALL_LOCATION_TEST), 15595);

    char servers[16192];
    char response[16192];
    servers[0] = '\0';

    if (!SFile::DownloadURLToBuffer(url, request, timeout, response, sizeof(response), &status)) {
        AddMfilDownloadError("Error downloading patch data from server '%s' , error code : %s", url, status);
        return false;
    }

    char mfilName[260];

    if (!ParsePatchRecord(response, program, locale, url, sizeof(url), mfilName, sizeof(mfilName))) {
        AddMfilDownloadError("Failed to parse patch data from server '%s'\r\nInstallID: '%s'", url, program);
        return false;
    }

    if (!SFile::DownloadURLToBuffer(url, NULL, timeout, response, sizeof(response), &status)) {
        AddMfilDownloadError("Failed to download manifest from server: '%s'\r\nInstallID: '%s', error code : %s", url, program, status);
        return false;
    }

    if (!ParseServerConfig(program, response, servers, sizeof(servers))) {
        AddMfilDownloadError("Failed to parse manifest from server '%s'\r\nInstallID: '%s'", url, program);
        return false;
    }

    HOSFILE file = OsCreateFile("WoW.mfil", OS_GENERIC_WRITE, 0, OS_CREATE_ALWAYS, OS_FILE_ATTRIBUTE_NORMAL, OS_FILE_TYPE_ANY);

    for (int32_t retry = 0; file == OS_INVALID_FILE; retry++) {
        if (retry >= 10) {
            AddMfilDownloadError("Failed to create file '%s'", "WoW.mfil");
            return false;
        }

        OsSleep(100);
        file = OsCreateFile("WoW.mfil", OS_GENERIC_WRITE, 0, OS_CREATE_ALWAYS, OS_FILE_ATTRIBUTE_NORMAL, OS_FILE_TYPE_ANY);
    }

    SStrPrintf(response, sizeof(response), "version=%d\n%smanifest_partial=%s", 2, servers, mfilName);
    uint32_t written = 0;
    bool result = OsWriteFile(file, response, strlen(response), &written) != 0;
    OsCloseFile(file);
    return result;
}

// OFFSET: 0x00406D30/0x140008610
static bool InitializeStreamingManifest() {
    bool mfilReady = false;
    bool dataReady = false;
    const char* errors = NULL;
    const char* patchListURL = ClientServices::GetPatchListURL();

    if (patchListURL) {
        char mfilPath[260];
        SStrCopy(mfilPath, "WoW.mfil", sizeof(mfilPath));

        if (DownloadMfil(patchListURL, s_localeCode, 3000)) {
            mfilReady = true;
        } else if (IsMfilCurrent()) {
            AddMfilDownloadError("Manifest is valid.");
            mfilReady = true;
        } else if (DownloadMfil(patchListURL, s_localeCode, 60000)) {
            AddMfilDownloadError("Downloaded with longer timeout.");
            mfilReady = true;
        }

        HOSFILE file = OS_INVALID_FILE;

        if (mfilReady) {
            int32_t build;

            if (GetMfilBuild(&build) && build != 15595 && SCreateProcess("Launcher.exe", NULL, 0, NULL, NULL, NULL)) {
                return false;
            }

            file = OsCreateFile(mfilPath, OS_GENERIC_READ, OS_FILE_SHARE_READ, OS_OPEN_EXISTING, OS_FILE_ATTRIBUTE_NORMAL, OS_FILE_TYPE_ANY);

            for (int32_t retry = 0; file == OS_INVALID_FILE && retry < 10; retry++) {
                OsSleep(100);
                file = OsCreateFile(mfilPath, OS_GENERIC_READ, OS_FILE_SHARE_READ, OS_OPEN_EXISTING, OS_FILE_ATTRIBUTE_NORMAL, OS_FILE_TYPE_ANY);
            }
        }

        if (!_stricmp(ClientServices::GetPatchListURL(), "streaming.corp.blizzard.net/patchlist/ldap.php")) {
            SFile::EnableBuildReports();
        }

        char currentDirectory[260];

        if (mfilReady && OsGetCurrentDirectory(sizeof(currentDirectory), currentDirectory) && SFile::InitializeStreaming(mfilPath, currentDirectory)) {
            dataReady = true;
        } else {
            errors = SFile::GetErrorDetails();
        }

        if (file != OS_INVALID_FILE) {
            OsCloseFile(file);
        }
    }

    SFile::SetStreamingStatus(mfilReady, dataReady, false, 15595, errors);
    return true;
}

// OFFSET: 0x004072A0
static bool FindArchivesToPreload() {
    for (uint32_t i = 0; i < sizeof(s_archives) / sizeof(s_archives[0]); i++) {
        char path[260];
        GetArchivePath(path, sizeof(path), s_archives[i].m_name, s_localeCode);

        char lockPath[260];
        SStrCopy(lockPath, path, sizeof(lockPath));
        SStrPack(lockPath, ".init", sizeof(lockPath));

        char tempPath[260];
        SStrCopy(tempPath, path, sizeof(tempPath));
        SStrPack(tempPath, ".temp", sizeof(tempPath));

        if (s_archives[i].m_flags & 0x1) {
            continue;
        }

        if (OsFileExists(path) && !OsFileExists(lockPath) && !OsFileExists(tempPath)) {
            continue;
        }

        if (!s_archives[i].m_expansion) {
            s_showArchiveProgress = 1;
        }

        ArchivePreload* preload = s_archivePreloads.NewNode(2, 0, 0);
        SStrCopy(preload->m_path, path, sizeof(preload->m_path));
        SStrCopy(preload->m_tempPath, tempPath, sizeof(preload->m_tempPath));
        SStrCopy(preload->m_lockPath, lockPath, sizeof(preload->m_lockPath));
        preload->m_lockFile = OS_INVALID_FILE;
        preload->m_tempFile = OS_INVALID_FILE;
        preload->m_expansion = s_archives[i].m_expansion;
        preload->m_remaining = 0;
    }

    OsDeleteFile(s_placeholderArchivePath);
    return true;
}

// OFFSET: 0x00407830/0x140009370
static void DestroyArchivePreloads() {
    ArchivePreload* preload = s_archivePreloads.Head();

    while (preload) {
        ArchivePreload* next = s_archivePreloads.Next(preload);

        if (preload->m_tempFile != OS_INVALID_FILE) {
            OsCloseFile(preload->m_tempFile);
            preload->m_tempFile = OS_INVALID_FILE;
            OsDeleteFile(preload->m_tempPath);
        }

        if (preload->m_lockFile != OS_INVALID_FILE) {
            OsCloseFile(preload->m_lockFile);
            preload->m_tempFile = OS_INVALID_FILE;
            OsDeleteFile(preload->m_lockPath);
        }

        s_archivePreloads.DeleteNode(preload);
        preload = next;
    }

    if (s_preloadBuffer) {
        SMemFree(s_preloadBuffer, __FILE__, __LINE__, 0);
        s_preloadBuffer = NULL;
    }
}

// OFFSET: 0x00406F30
uint32_t ClientBeginInitializingArchives(uint8_t expansion) {
    int64_t total = 0;

    for (ArchivePreload* preload = s_archivePreloads.Head(); preload; preload = s_archivePreloads.Next(preload)) {
        if (preload->m_expansion > expansion) {
            continue;
        }

        int64_t size = 0;

        if (!Blizzard::Streaming::GetStreamedFileSize(preload->m_path, &size) || size <= 0) {
            continue;
        }

        preload->m_remaining = size - 8;
        total += size - 8;

        char directory[260];
        SStrCopy(directory, preload->m_tempPath, sizeof(directory));
        OsPathStripFilename(directory);
        OsCreateDirectory(directory, 1);
        preload->m_lockFile = OsCreateFile(preload->m_lockPath, OS_GENERIC_WRITE, 0, OS_CREATE_ALWAYS, OS_FILE_ATTRIBUTE_NORMAL, OS_FILE_TYPE_ANY);
    }

    if (total > 0) {
        s_preloadBuffer = static_cast<uint8_t*>(SMemAlloc(4000000, __FILE__, __LINE__, SMEM_FLAG_ZEROMEMORY));
        s_preloadBuffer[3999999] = '0';
    }

    return static_cast<uint32_t>(total / 4000000);
}

// OFFSET: 0x004078D0/0x140009490
int32_t ClientContinueInitializingArchives(int32_t* stalled) {
    if (stalled) {
        *stalled = 0;
    }

    ArchivePreload* preload;

    for (preload = s_archivePreloads.Head(); preload; preload = s_archivePreloads.Next(preload)) {
        if (preload->m_lockFile != OS_INVALID_FILE) {
            if (preload->m_tempFile == OS_INVALID_FILE) {
                preload->m_tempFile = OsCreateFile(preload->m_tempPath, OS_GENERIC_WRITE, OS_FILE_SHARE_READ, OS_CREATE_ALWAYS, OS_FILE_ATTRIBUTE_NORMAL, OS_FILE_TYPE_ANY);

                if (OsFileSetSparse(preload->m_tempFile, 1)) {
                    OsSetFilePointer(preload->m_tempFile, preload->m_remaining, 0);
                    OsSetEndOfFile(preload->m_tempFile);
                    preload->m_remaining = 0;
                }

                if (preload->m_tempFile == OS_INVALID_FILE) {
                    const char* error = OsGetLastErrorStr();
                    const char* details = SFile::GetErrorDetails();
                    OsCloseFile(preload->m_lockFile);
                    preload->m_lockFile = OS_INVALID_FILE;
                    SErrDisplayAppFatal("Unable to create archive file %s: %s\nError Details:%s", preload->m_tempPath, error, details);
                }
            }

            if (preload->m_remaining <= 8000000) {
                OsCloseFile(preload->m_tempFile);
                preload->m_tempFile = OS_INVALID_FILE;
                OsMoveFile(preload->m_tempPath, preload->m_path);
                OsDeleteFile(preload->m_tempPath);
                OsCloseFile(preload->m_lockFile);
                preload->m_lockFile = OS_INVALID_FILE;
                OsDeleteFile(preload->m_lockPath);
                s_archivePreloads.DeleteNode(preload);
            } else {
                preload->m_remaining -= 4000000;
                uint32_t written;

                if (!OsWriteFile(preload->m_tempFile, s_preloadBuffer, 4000000, &written) || written != 4000000) {
                    const char* error = OsGetLastErrorStr();
                    const char* details = SFile::GetErrorDetails();
                    SErrDisplayAppFatal("Unable to write archive file %s: %s\nError Details:%s", preload->m_tempPath, error, details);
                }
            }

            return 0;
        }

        if (preload->m_remaining > 0) {
            break;
        }
    }

    if (!preload) {
        SMemFree(s_preloadBuffer, __FILE__, __LINE__, 0);
        s_preloadBuffer = NULL;
        return 1;
    }

    if (stalled) {
        *stalled = 1;
    }

    preload->m_lockFile = OsCreateFile(preload->m_lockPath, OS_GENERIC_WRITE, OS_FILE_SHARE_READ, OS_CREATE_ALWAYS, OS_FILE_ATTRIBUTE_NORMAL, OS_FILE_TYPE_ANY);

    if (System_File::ExistsUnstreamed(preload->m_path)) {
        if (preload->m_lockFile != OS_INVALID_FILE) {
            OsCloseFile(preload->m_lockFile);
            preload->m_lockFile = OS_INVALID_FILE;
            OsDeleteFile(preload->m_tempPath);
            OsDeleteFile(preload->m_lockPath);
            s_archivePreloads.DeleteNode(preload);
            return 0;
        }
    } else if (preload->m_lockFile != OS_INVALID_FILE) {
        return 0;
    }

    OsSleep(10);
    return 0;
}

// OFFSET: 0x00407D60
static bool PreloadArchives() {
    uint32_t chunks = ClientBeginInitializingArchives(0);

    if (!chunks) {
        return true;
    }

    ArchiveProgressSetStage(0);
    uint32_t done = 0;

    while (ArchiveProgressPump()) {
        int32_t stalled = 0;

        if (ClientContinueInitializingArchives(&stalled)) {
            return true;
        }

        if (!stalled && done + 1 < chunks) {
            done++;
            ArchiveProgressSetPosition(static_cast<float>(done) / static_cast<float>(chunks));
        }

        if (done >= chunks) {
            return true;
        }
    }

    return false;
}

// OFFSET: 0x004042E0
static bool OpenRequiredArchive(const char* name, const char* locale, int32_t priority, uint32_t index, bool optional, bool queue) {
    char path[260];
    GetArchivePath(path, sizeof(path), name, locale);

    if (!SFile::OpenArchive(path, priority, s_archiveOpenFlags, &s_archiveHandles[index])) {
        s_archiveHandles[index] = NULL;
        return false;
    }

    int32_t complete = SFile::IsArchiveComplete(s_archiveHandles[index]);

    if (!complete && !optional) {
        s_allArchivesComplete = 0;
    }

    if (!SFile::IsStreamingDataReady() && !complete) {
        SFile::CloseArchive(s_archiveHandles[index]);
        SErrSetLastError(0x85100095);
        s_archiveHandles[index] = NULL;
        return false;
    }

    if (queue) {
        BGLoading::QueueArchive(path);
        s_startBGLoadThread |= complete == 0;
    }

    return true;
}

// OFFSET: 0x004043B0/0x1400047B0
static bool LoadMopaqFileSigningData(const char* locale) {
    SFileClearBlockTableMD5s();

    char* data;
    uint32_t size;

    if (!SFile::LoadFile("signaturefile", reinterpret_cast<void**>(&data), &size, 0)) {
        SErrPrepareAppFatal(__FILE__, __LINE__);
        SErrDisplayAppFatalCustom(2, "file: %s", "signaturefile");
    }

    SFileSetSignatureCallbacks(1);

    if (!SSignatureVerify(reinterpret_cast<const uint8_t*>(data), size, s_signatureModulus, sizeof(s_signatureModulus), s_signatureExponent, sizeof(s_signatureExponent))) {
        SErrPrepareAppFatal(__FILE__, __LINE__);
        SErrDisplayAppFatalCustom(0x4DC, "Could not authenticate the %s file.", "signaturefile");
    }

    char* line = strstr(data, "\r\n");
    *line = '\0';
    int32_t version = SStrToInt(data);

    char* signature = line + 2;
    line = strstr(signature, "\r\n");
    *line = '\0';
    int32_t signatureVersion = atoi(signature);

    if (signatureVersion != 4) {
        SErrPrepareAppFatal(__FILE__, __LINE__);
        SErrDisplayAppFatalCustom(0x85100083, "Signature string (%d) in %s file does not match game's signature version (%d).", signatureVersion, "signaturefile", 4);
    }

    char* scope = line + 2;
    line = strstr(scope, "\r\n");
    *line = '\0';

    if (strncmp(scope, "RELEASE", 7)) {
        SErrPrepareAppFatal(__FILE__, __LINE__);
        SErrDisplayAppFatalCustom(0x85100083, "Illegal scope string %s in %s: must be \"RELEASE\".", scope, "signaturefile");
    }

    char* record = line + 2;

    if (version != 2) {
        SErrPrepareAppFatal(__FILE__, __LINE__);
        SErrDisplayAppFatalCustom(0x85100083, "Format version in %s file is not supported.", "signaturefile");
    }

    uint32_t localeTag = *reinterpret_cast<const uint32_t*>(locale);

    while (record < data + size && strncmp(record, "NGIS", 4)) {
        record[4] = '\0';
        record[6] = '\0';
        record[39] = '\0';
        char* hash = record + 7;
        char* name = record + 40;
        char* end = strstr(name, "\r\n");
        *end = '\0';

        for (uint32_t i = 0; i < sizeof(s_signaturePrefixes) / sizeof(s_signaturePrefixes[0]); i++) {
            if (!strncmp(record + 40, s_signaturePrefixes[i].m_prefix, s_signaturePrefixes[i].m_length)) {
                name += s_signaturePrefixes[i].m_length;
                break;
            }
        }

        uint32_t tag = *reinterpret_cast<const uint32_t*>(record);

        if ((tag == 'esab' || tag == localeTag) && (record[5] == 'c' || record[5] == 'e')) {
            MD5 md5(hash);

            if (!SFileSetFileBlockEntryMD5(name, md5)) {
                if (SFile::IsStreamingDataReady()) {
                    const char* details = SFile::GetErrorDetails();
                    SErrPrepareAppFatal(__FILE__, __LINE__);
                    const char* mfilErrors = s_mfilDownloadErrors ? s_mfilDownloadErrors : "None";
                    const char* dataStatus = SFile::IsStreamingDataReady() ? "Data Rdy" : "Data Not Rdy";
                    const char* mfilStatus = SFile::IsStreamingMfilReady() ? "Mfil Rdy" : "Mfil Not Rdy";
                    SErrDisplayAppFatalCustom(0x85100083, "Fail to set signature (%s) for file (%s).\n%sStreaming Status: %s / %s\nWoW.mfil download error: %s\n", hash, name, details, mfilStatus, dataStatus, mfilErrors);
                }

                ReportArchiveFailure(name);
                SFile::Unload(data);
                return false;
            }
        }

        record = end + 2;

        if (record >= data + size) {
            break;
        }
    }

    SFile::Unload(data);
    return true;
}

// OFFSET: 0x004046C0
static bool OpenCacheArchive(ArchiveEnumParams* params, const char* path, int32_t writable) {
    int32_t priority = params->m_priority;

    if (writable) {
        priority += 100;
    }

    int32_t opened;

    if (!OsFileExists(path)) {
        opened = SFile::CreateArchive(path, priority, &s_archiveHandles[params->m_index]);
    } else if (SFile::OpenArchive(path, priority, 0x14818, &s_archiveHandles[params->m_index])) {
        opened = 1;
    } else if (SErrGetLastError() == 0x8510006C) {
        if (!OsDeleteFile(path)) {
            return false;
        }

        opened = SFile::CreateArchive(path, priority, &s_archiveHandles[params->m_index]);
    } else {
        opened = SFile::OpenArchive(path, priority, 0x18808, &s_archiveHandles[params->m_index]);
    }

    if (!opened) {
        return false;
    }

    params->m_index++;
    params->m_priority++;
    return true;
}

// OFFSET: 0x00404760/0x140004C90
static int32_t OpenLocaleUpdateArchive(ArchiveEnumParams* params, const char* path, int32_t alternate) {
    SArchive* archive = NULL;

    if (!SFile::OpenArchive(path, params->m_priority, s_archiveOpenFlags, &archive)) {
        ReportArchiveFailure(path);
        return 0;
    }

    int32_t complete = SFile::IsArchiveComplete(archive);

    if (!complete) {
        s_allArchivesComplete = 0;
    }

    if (!SFile::IsStreamingDataReady() && !complete) {
        SFile::CloseArchive(archive);
        ReportArchiveFailure(path);
        return 0;
    }

    s_archiveHandles[params->m_index] = archive;
    params->m_priority++;
    params->m_index++;
    BGLoading::QueueArchive(path);
    s_startBGLoadThread |= complete == 0;

    if (!alternate) {
        return 1;
    }

    if (!params->m_locale || (strncmp(params->m_locale, "zhCN", 4) && strncmp(params->m_locale, "enCN", 4))) {
        return 1;
    }

    if (SFile::OpenPathAsArchive(archive, "alternate", params->m_priority + 100, 0, &s_archiveHandles[params->m_index])) {
        params->m_priority++;
        params->m_index++;
    }

    return 1;
}

// OFFSET: 0x00404870/0x140004DE0
static int32_t OpenBaseUpdateArchive(ArchiveEnumParams* params, const char* path) {
    bool opened = false;
    SArchive* archive = NULL;

    if (SFile::OpenArchive(path, 0, s_archiveOpenFlags, &archive) && archive) {
        int32_t complete = SFile::IsArchiveComplete(archive);

        if (!complete) {
            s_allArchivesComplete = 0;
        }

        if (SFile::IsStreamingDataReady() || complete) {
            BGLoading::QueueArchive(path);
            s_startBGLoadThread |= complete == 0;

            if (SFile::OpenPathAsArchive(archive, "base", params->m_priority, 0, &s_archiveHandles[params->m_index])) {
                params->m_priority++;
                params->m_index++;
            }

            if (params->m_locale) {
                if (SFile::OpenPathAsArchive(archive, params->m_locale, params->m_priority, 0, &s_archiveHandles[params->m_index])) {
                    params->m_priority++;
                    params->m_index++;
                }

                if (!strncmp(params->m_locale, "zhCN", 4) || !strncmp(params->m_locale, "enCN", 4)) {
                    char alternatePath[32];
                    SStrPrintf(alternatePath, 30, "%s-alternate\\alternate", params->m_locale);

                    if (SFile::OpenPathAsArchive(archive, alternatePath, params->m_priority + 100, 0, &s_archiveHandles[params->m_index])) {
                        params->m_priority++;
                        params->m_index++;
                    }
                }
            }

            opened = true;
        }

        SFile::CloseArchive(archive);

        if (opened) {
            return 1;
        }
    }

    ReportArchiveFailure(path);
    return 0;
}

// OFFSET: 0x004049E0/0x140004FA0
static bool IsUpdateArchiveInManifest(const char* path) {
    char name[260];
    SStrCopy(name, path, sizeof(name));

    char* extension = strrchr(name, '.');

    if (!extension) {
        return false;
    }

    *extension = '\0';
    char* dash = strrchr(name, '-');

    if (!dash) {
        return false;
    }

    const char* build = dash + 1;
    *dash = '\0';
    dash = strrchr(name, '-');

    if (!dash) {
        return false;
    }

    char* type = dash + 1;
    char manifestPath[260];

    if (!_stricmp(type, "base") || !_stricmp(type, "patch")) {
        SStrPrintf(manifestPath, sizeof(manifestPath), "Data/wow-update-base-%s.MPQ", build);
    } else {
        if (!_stricmp(type, "alternate")) {
            SStrCopy(type, "zhCN", 5);
        }

        SStrPrintf(manifestPath, sizeof(manifestPath), "Data/%s/wow-update-%s-%s.MPQ", type, type, build);
    }

    if (Blizzard::Streaming::IsInManifest(manifestPath)) {
        return true;
    }

    SStrPrintf(manifestPath, sizeof(manifestPath), "Data/wow-update-%s.MPQ", build);
    return Blizzard::Streaming::IsInManifest(manifestPath);
}

// OFFSET: 0x00404B60
static int32_t AcceptArchive(const char* path, void* param, uint8_t flags) {
    ArchiveEnumParams* params = static_cast<ArchiveEnumParams*>(param);

    if (SFile::IsStreamingMode() && (((flags & 0x7) && !Blizzard::Streaming::IsInManifest(path)) || ((flags & 0x18) && !IsUpdateArchiveInManifest(path)))) {
        SFile::DeleteArchive(path);
        return 1;
    }

    if (flags & 0x6) {
        return OpenLocaleUpdateArchive(params, path, flags & 0x4);
    }

    if (flags & 0x1) {
        return OpenBaseUpdateArchive(params, path);
    }

    OpenCacheArchive(params, path, flags & 0x10);
    return 1;
}

// OFFSET: 0x00404C00/0x140005320
static void ListArchives(const char* prefix, int32_t index, int32_t (*callback)(const char* fileName, void* param), void* param) {
    ARCHIVELISTPARAMS params;
    params.m_prefix = prefix;
    params.m_prefixLength = strlen(prefix);
    params.m_callback = callback;
    params.m_param = param;

    for (int32_t i = s_numUpdateArchives - 1; i >= 0; i--) {
        SFile::List(s_archiveHandles[i], ListArchiveFilter, &params);
    }

    if (s_archives[index].m_opened) {
        SFile::List(s_archiveHandles[s_archives[index].m_slot], ListArchiveFilter, &params);
    }
}

// OFFSET: 0x00404C90/0x1400053C0
void ListInterfaceArchive(const char* prefix, int32_t (*callback)(const char* fileName, void* param), void* param) {
    ListArchives(prefix, 3, callback, param);
}

// OFFSET: 0x00404CB0
static void CloseAllArchives() {
    for (uint32_t i = s_archiveHandles.Count(); i; i--) {
        if (s_archiveHandles[i - 1]) {
            SFile::CloseArchive(s_archiveHandles[i - 1]);
        }
    }
}

// OFFSET: 0x00404CE0
static void SetLocaleCode(const char* locale) {
    int32_t index = LocaleNameToIndex(locale);
    SStrCopy(s_localeCode, g_localeNames[index], sizeof(s_localeCode));
    s_locale = LocaleIndexToDB2Locale(index);
    SFile::s_locale = (s_localeCode[0] << 24) | (s_localeCode[1] << 16) | (s_localeCode[2] << 8) | s_localeCode[3];

    switch (s_locale) {
        case 1:
        case 2:
        case 4:
        case 5:
            FrameScript_SetPluralRule(PLURAL_RULE_1);
            break;

        case 8:
            FrameScript_SetPluralRule(PLURAL_RULE_2);
            break;

        default:
            FrameScript_SetPluralRule(PLURAL_RULE_0);
            break;
    }
}

// OFFSET: 0x00405DE0
bool OpenExpansionArchives(uint8_t accountExpansion, uint8_t activeExpansion) {
    s_startBGLoadThread = 0;

    for (int32_t i = sizeof(s_archives) / sizeof(s_archives[0]) - 1; i >= 0; i--) {
        ARCHIVEINFO* info = &s_archives[i];

        if (!static_cast<uint8_t>(info->m_expansion) || (info->m_flags & 0x4)) {
            continue;
        }

        SArchive* archive = s_archiveHandles[info->m_slot];
        char path[260];
        GetArchivePath(path, sizeof(path), info->m_name, s_localeCode);

        if (info->m_opened && archive) {
            int32_t complete = SFile::IsArchiveComplete(archive);
            BGLoading::QueueArchive(path);
            s_startBGLoadThread |= complete == 0;
        } else {
            if (!(info->m_flags & 0x1) && static_cast<uint8_t>(info->m_expansion) <= accountExpansion) {
                ReportArchiveFailure(path);
                return false;
            }

            if (static_cast<uint8_t>(info->m_expansion) <= activeExpansion) {
                BGLoading::QueueFile(path);

                if (!SFile::IsFileCompletelyLocal(path)) {
                    s_startBGLoadThread |= 1;
                }
            }
        }
    }

    if (s_startBGLoadThread && !BGLoading::BGLoadThreadStarted()) {
        BGLoading::StartBGLoadThread();
        s_startBGLoadThread = 0;
    }

    return true;
}

// OFFSET: 0x00406570
static bool OpenArchives() {
    for (uint32_t i = 0; i < sizeof(s_archives) / sizeof(s_archives[0]); i++) {
        s_archives[i].m_opened = false;
    }

    s_archiveHandles.SetCount(PatchOnDemand::Cache::LoadArchives("Data", s_localeCode) + 100);

    ArchiveEnumParams params;
    params.m_index = 0;
    params.m_priority = 1;
    params.m_locale = s_localeCode;

    ArchiveProgressSetStage(1);
    int64_t loaded = 0;
    int64_t total = GetStreamedArchiveSize();
    params.m_priority = 100;

    for (int32_t i = sizeof(s_archives) / sizeof(s_archives[0]) - 1; i >= 0; i--) {
        ARCHIVEINFO* info = &s_archives[i];

        if (!(info->m_flags & 0x2) || s_locale != 4) {
            continue;
        }

        SArchive* archive = NULL;
        char path[260];
        GetArchivePath(path, sizeof(path), info->m_name, s_localeCode);

        if (!SFile::OpenArchive(path, 0, s_archiveOpenFlags, &archive)) {
            continue;
        }

        int32_t complete = SFile::IsArchiveComplete(archive);

        if (!complete) {
            s_allArchivesComplete = 0;
        }

        if (SFile::IsStreamingDataReady() || complete) {
            BGLoading::QueueArchive(path);
            s_startBGLoadThread |= complete == 0;

            if (SFile::OpenPathAsArchive(archive, info->m_subdir, params.m_priority + 100, 0, &s_archiveHandles[params.m_index])) {
                info->m_slot = params.m_index;
                info->m_priority = params.m_priority;
                info->m_opened = true;
                params.m_index++;
                params.m_priority++;
            }
        }

        SFile::CloseArchive(archive);
    }

    SFile::EnableHash(false);
    int32_t enumerated = PatchOnDemand::Cache::EnumerateArchives(AcceptArchive, &params);
    SFile::EnableHash(true);

    if (!enumerated) {
        s_archiveHandles.SetCount(params.m_index);
        return false;
    }

    s_numUpdateArchives = params.m_index;
    params.m_priority = 99;

    for (int32_t i = sizeof(s_archives) / sizeof(s_archives[0]) - 1; i >= 0; i--) {
        ARCHIVEINFO* info = &s_archives[i];

        if (info->m_flags & 0x6) {
            continue;
        }

        uint8_t expansion = static_cast<uint8_t>(info->m_expansion);

        if (expansion > 3) {
            continue;
        }

        bool optional = (info->m_flags & 0x1) || (s_locale != 4 && expansion);

        if (OpenRequiredArchive(info->m_name, s_localeCode, params.m_priority, params.m_index, optional, expansion == 0)) {
            info->m_slot = params.m_index;
            info->m_priority = params.m_priority;
            info->m_opened = true;
            params.m_index++;
        } else if (!optional) {
            char path[260];
            GetArchivePath(path, sizeof(path), info->m_name, s_localeCode);
            ReportArchiveFailure(path);
            s_archiveHandles.SetCount(params.m_index);
            return false;
        }

        params.m_priority--;
        loaded += info->m_size;

        if (s_showArchiveProgress) {
            ArchiveProgressSetPosition(static_cast<float>(static_cast<double>(loaded) / static_cast<double>(total > 0 ? total : 1)));

            if (!ArchiveProgressPump()) {
                s_archiveHandles.SetCount(params.m_index);
                return false;
            }
        }
    }

    s_archiveHandles.SetCount(params.m_index);

    void* profile = ProfileCreate();

    if (ProfileReadFile(profile, "Wow.ini")) {
        ProfileGetValue(profile, "WoW Config", "Region", &s_region, 0);
    }

    ProfileClose(profile);

    return LoadMopaqFileSigningData(s_localeCode) && (CmdLineGetBool(static_cast<CMDOPT>(CMD_LAUNCH)) || s_locale != 4 || s_allArchivesComplete || !SCreateProcess("Launcher.exe", NULL, 0, NULL, NULL, NULL));
}

// OFFSET: #0x00401220
CommandManager::~CommandManager() {
    for (uint32_t i = 0; i < this->m_count; i++) {
        this->m_commands[i]->Uninstall();
        delete this->m_commands[i];
    }
}

// OFFSET: 0x004011E0/0x1400010B0
static void ClientProcessNotification(const char* text) {
    const char* message = text;
    bool show = true;
    bool chat = false;

    if (!strncmp(message, "DEBUGACTION: ", 13)) {
        message += 13;
    }

    if (*message != '~') {
        CGGameUI::AddErrorMessage(message, 1);
        ConsoleWriteA("%s", ERROR_COLOR, message);
        return;
    }

    message++;

    if (*message == '>') {
        show = false;
        message++;
    }

    if (*message == '#') {
        show = false;
        chat = true;
        message++;
    }

    int32_t type = 0;
    int32_t color = ERROR_COLOR;

    if (*message == '^') {
        type = message[1] - '0';
        color = type;
        message += 2;
    }

    if (show) {
        CGGameUI::AddErrorMessage(message, color == ERROR_COLOR);
    }

    ConsoleWriteA("%s", static_cast<COLOR_T>(color), message);

    if (chat) {
        CGChat::AddChatMessage(message, static_cast<SLASH_COMMAND_ID>(type), NULL);
    }
}

// OFFSET: 0x004012A0
int32_t Packet_SMSG_NOTIFICATION(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    char text[4096];
    msg->GetString(text, sizeof(text));
    ClientProcessNotification(text);
    return 1;
}

// OFFSET: 0x004012E0
static bool OnNotification(ClientNotification* msg) {
    ClientProcessNotification(msg->m_text);
    return true;
}

// OFFSET: 0x004013F0
static bool OnNewWorldAbort(ClientNewWorldAbort* msg) {
    MovementResumeActiveMover();
    return true;
}

// OFFSET: 0x00403580/0x140002DA0
static void MovementInit() {
    char logFile[260];

    if (!SRegLoadString("World of Warcraft\\Client", "MoveLogFile", 0, logFile, sizeof(logFile)) || !logFile[0]) {
        SRegSaveString("World of Warcraft\\Client", "MoveLogFile", 0, "ClientMovement.txt");
        SStrCopy(logFile, "ClientMovement.txt", sizeof(logFile));
    }

    uint32_t processId = SGetCurrentProcessId();
    char* extension = SStrChrR(logFile, '.');
    SStrPrintf(extension, logFile + sizeof(logFile) - extension, "%04d.txt", processId);
    MovementInitialize(logFile);
    EventRegisterEx(EVENT_ID_IDLE, MovementIdleMoveUnits, NULL, 2.0f);
}

// OFFSET: 0x00401C70/0x1400017E0
static void SetDefaults(OPTION_PAGE page) {
    const char* name;

    if (page == OPTION_PAGE_0) {
        name = "windowResizeLock";
    } else if (page == OPTION_PAGE_1) {
        if (s_desktopGammaCvar) {
            s_desktopGammaCvar->Set("0", true, false, false, true);
            s_desktopGammaCvar->Update();
        }

        if (s_gammaCvar) {
            s_gammaCvar->Set("1.0", true, false, false, true);
            s_gammaCvar->Update();
        }

        CVar* var = CVar::Lookup("gxStereoEnabled");

        if (var) {
            var->Default();
        }

        var = CVar::Lookup("gxStereoConvergence");

        if (var) {
            var->Default();
        }

        var = CVar::Lookup("gxStereoSeparation");

        if (var) {
            var->Default();
        }

        var = CVar::Lookup("maxFPS");

        if (var) {
            var->Default();
        }

        name = "maxFPSBk";
    } else if (page == OPTION_PAGE_2) {
        CVar* var = CVar::Lookup("uiScale");

        if (var) {
            var->Default();
        }

        name = "useUiScale";
    } else {
        return;
    }

    CVar* var = CVar::Lookup(name);

    if (var) {
        var->Default();
    }
}

void ZoneDebugDestroy() {
}

// OFFSET: 0x00407150
static void WowClientDestroy() {
    CGPlayer_C::PlayerShutdownClientHandlers();
    ValidateNameDestroy();
    DeclinedWordsDestroy();
    SpellTableDestroy();
    SpellVisualsShutdown();
    InputControlDestroy();
    CGlueMgr::Shutdown();
    ShutdownAddOns();
    DestroyBannedAddOns();
    DBCache_ClearHandlers();
    DBCache_Destroy();
    ZoneDebugDestroy();
    CEffect::Destroy();
    CCharacterComponent::Destroy();
    FrameXML_FreeHashNodes();
    FrameXML_ClearFactories();
    CSimpleTexture::s_fileNameHash.Clear();
    SI3::VoiceChat_StopPlayingLoopbackSound();
    ComSatClient_Shutdown();
    ObjectEffectSystem_ShutDown();
    SI3::ShutDown(0);
    FrameScript_Destroy();
    LoadingScreenShutdown();
    WowSysMessageOutput::Shutdown();
    ConsoleCommandUnregister("reloadUI");
    ConsoleCommandUnregister("perf");
    PhaseShift_C_CleanupDB();
    ClientDBShutdown();
    StaticDBUnloadAll();
    SysMsgShutdown();
}

// OFFSET: #0x004029B0
static bool IsPTRBuild() {
    bool isPTR = false;
    uint32_t size = 0;
    char* data = GetComponentVersionData(&size);

    if (data) {
        XMLTree* tree = XMLTree_Load(data, size);

        if (tree) {
            XMLNode* root = XMLTree_GetRoot(tree)->m_child;

            if (root) {
                XMLNode* product = root->GetChildByName("product");

                if (product && !SStrCmp(product->m_body, "WoWT", STORM_MAX_STR)) {
                    isPTR = true;
                }
            }

            XMLTree_Free(tree);
        }

        delete[] data;
    }

    return isPTR;
}

// OFFSET: #0x00402A40
uint8_t GetClientExpansionLevel() {
    return s_expansionLevel;
}

// OFFSET: 0x004020A0
void SetPaths() {
    SFile::DisableSFileCheckDisk();
    SFile::EnableDirectAccess(0);

    const char* path = CmdLineGetString(CMD_DATA_DIR);
    char buffer[260];

    if (!*path) {
        OsGetExePath(buffer, sizeof(buffer));
        path = buffer;
    }

    OsSetCurrentDirectory(path);
    SFile::SetBasePath(path);
    s_unk_B2FA10 = 0;
    SFile::SetDataPath(s_dataDirectory);
}

// OFFSET: #0x00402A50
const char* GetInstallLocationString() {
    if (!SFile::IsTrial()) {
        return NULL;
    }

    if (!s_installLocation[0]) {
        if (IsPTRBuild()) {
            SRegLoadString("World of Warcraft\\PTR", "RetailInstallPath", 1, s_installLocation, sizeof(s_installLocation));
            return s_installLocation;
        }

        SRegLoadString("World of Warcraft", "RetailInstallPath", 1, s_installLocation, sizeof(s_installLocation));
    }

    return s_installLocation;
}

// OFFSET: 0x004026C0/0x1400020D0
int ForEveryRunOnceWTF::FileNameCompare(const void* a, const void* b) {
    return SStrSortUTF8I(*static_cast<char* const*>(a), *static_cast<char* const*>(b), STORM_MAX_STR);
}

// OFFSET: 0x00402930/0x1400021F0
static int32_t ClientIdle(const void* data, void* param) {
    ClientGameTimeTickHandler(data, param);
    Player_C_ZoneUpdateHandler(data, NULL);

    return 1;
}

// OFFSET: 0x004029D0
static bool OnMessageBox(ClientMessageBox* msg) {
    char text[512];
    msg->m_text.GetString(text, sizeof(text));
    OsGuiMessageBox(OsGuiGetWindow(2), 1, text, "Server Message");
    return true;
}

// OFFSET: 0x00402A20
static bool OnSuspendToken(ClientSuspendToken* msg) {
    UserRouterClientSuspendTokenResponse response;
    response.m_present |= 1;
    response.m_token = msg->m_sequence;
    ClientServices::Connection()->Send(&response, CONNECTION_INSTANCE);

    if (msg->m_reason) {
        MovementSuspendActiveMover();
    }

    return true;
}

// OFFSET: 0x004031F0
static bool OnTransferPending(ClientTransferPending* msg) {
    int32_t shipID = 0;
    int32_t shipOriginMapID = -1;
    int32_t transferSpellID = 0;

    if (msg->m_hasTransferSpellID) {
        transferSpellID = msg->m_transferSpellID;
    }

    if (msg->m_hasShip) {
        shipID = msg->m_shipID;
        shipOriginMapID = msg->m_shipOriginMapID;
    }

    CGGameUI::ClearClientControls();
    ConsolePrintf("World transfer pending...");

    if (LoadingScreenIsPreparedForSpell(transferSpellID)) {
        LoadingScreenEnablePrepared(msg->m_mapID);
        return true;
    }

    LoadingScreenClearPrepared();

    if (!shipID || shipOriginMapID == -1) {
        LoadingScreenEnable(msg->m_mapID, true);
        return true;
    }

    LoadingScreenEnableShip(shipID, shipOriginMapID, msg->m_mapID);
    return true;
}

// OFFSET: 0x00403660/0x140002EA0
static int32_t ClientLoadNewWorld(int32_t mapID, C3Vector* position, float facing) {
    if (!g_mapDB.GetRecord(mapID)) {
        ConsolePrintf("Bad ClientLoadNewWorld mapID %d\n", mapID);
        return 0;
    }

    ClientServices::Connection()->CharacterSetInGame(0);
    MovementDestroy();
    EventUnregister(EVENT_ID_IDLE, MovementIdleMoveUnits);
    CMissile::RemoveMissiles();
    Spell_C_StopTargeting();
    CGBarberShop::DisableBarberShop();
    ClntObjMgrDestroyStd();
    SpellVisualsClear();
    CEffect::Destroy();
    CGWorldFrame::SetScreenEffect(0);

    if (CGWorldFrame::s_currentWorldFrame) {
        CGWorldFrame::s_currentWorldFrame->UnitClear();
    }

    CGUnit_C::StopMissileTrajectoryTargeting();
    World::UnloadMap();
    SI3::SetWeatherKitID(-1);
    Spell_C_ClearCooldowns();
    ClntObjMgrInitializeStd(mapID);
    MovementInit();
    CloseAllLootRolls();
    CGPartyInfo::CheckClearLFGSlot();
    AsyncFileReadIngameProgressCallback(NULL, NULL);
    AsyncFileReadProgressCallback(LoadingScreenAsyncCallback, NULL);
    World::SetLoadProgressCallback(LoadingScreenWorldCallback, NULL);
    World::LoadMap(mapID, *position);
    AsyncFileReadProgressCallback(NULL, NULL);
    World::SetLoadProgressCallback(NULL, NULL);

    CGCamera* camera = CGWorldFrame::GetActiveCamera();
    camera->m_position = *position;
    camera->SetFacing(facing, 0.0f, 0.0f);
    camera->SetTarget(NULL, 0);

    return 1;
}

// OFFSET: 0x004037B0
static bool OnNewWorld(ClientNewWorld* msg) {
    if (!ClientLoadNewWorld(msg->m_mapID, &msg->m_position, msg->m_facing)) {
        return false;
    }

    UserClientWorldPortResponse response;
    ClientServices::Send(&response);
    MovementResumeActiveMover();
    return true;
}

// OFFSET: 0x00403810
static int32_t Packet_SMSG_LOGIN_VERIFY_WORLD(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    C3Vector position;
    position.x = 0.0f;
    position.y = 0.0f;
    position.z = 0.0f;
    uint32_t mapID;
    float facing;

    msg->Get(mapID);
    msg->Get(position.x);
    msg->Get(position.y);
    msg->Get(position.z);
    msg->Get(facing);

    if (mapID == ClntObjMgrGetMapID()) {
        return 1;
    }

    return ClientLoadNewWorld(mapID, &position, facing);
}

// OFFSET: #0x004033C0
void ClientPostClose(int32_t reason) {
    s_closeErrorStringId = reason;
    EventPostClose();
}

// OFFSET: 0x00402A70/0x1400058F0
static void LogRealmInfo(char* buffer, uint32_t size) {
    REALM_INFO* realm = ClientServices::GetSelectedRealm();

    if (realm && realm->m_realmName[0] && realm->m_addresses[0].m_type) {
        uint32_t length = strlen(buffer);

        if (length < size) {
            SStrPrintf(buffer + length, size - length, "Realm: %s (%s)\r\n", realm->m_realmName, SockAddrString(&realm->m_addresses[0], realm->m_ports[0]).m_str);
        }
    } else {
        SStrPack(buffer, "Realm: ???\r\n", size);
    }
}

// OFFSET: #0x00403460
static void LogObjectInfo(CGObject_C* object, const char* label, char* buffer, uint32_t size) {
    if (!SMemIsValidPointer(object, sizeof(CGObject_C), 0)) {
        return;
    }

    C3Vector position = object->GetPosition();
    WOWGUID guid = object->m_obj->m_guid;

    char line[260];
    SStrPrintf(line, sizeof(line), "%s: %s, %016I64X, (%g,%g,%g)\r\n", label, object->GetObjectName(), guid, position.x, position.y, position.z);
    SStrPack(buffer, line, size);
}

// OFFSET: 0x00402B30/0x1400059F0
static void LogZoneInfo(CGObject_C* object, char* buffer, uint32_t size) {
    object->GetPosition();
    SStrPack(buffer, "Local Zone: ", size);

    AreaTableRec* area = AreaListGetObjectArea(object->m_worldObject);
    char name[260];
    AreaListGetName(area ? area->m_id : 0, name, sizeof(name), 1);
    SStrPack(buffer, name, size);
    SStrPack(buffer, "\r\n", size);
}

// OFFSET: #0x00404B80
int32_t STORMAPI WowLogHeader(char* buffer, uint32_t bufferchars) {
    SStrPrintf(buffer, bufferchars, "WoWBuild: %d\r\n", 12340);

    if (ClntObjMgrIsValid(0)) {
        CGObject_C* player = ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__);

        if (player && SMemIsValidPointer(player, sizeof(CGPlayer_C), 0)) {
            LogRealmInfo(buffer, bufferchars);
            LogZoneInfo(player, buffer, bufferchars);
            LogObjectInfo(player, "Local Player", buffer, bufferchars);

            CGObject_C* object = ClntObjMgrObjectPtr(static_cast<CGPlayer_C*>(player)->m_combat.m_attacking, TYPEMASK_OBJECT, __FILE__, __LINE__);

            if (object) {
                LogObjectInfo(object, "Attacking Target", buffer, bufferchars);
            }

            object = ClntObjMgrObjectPtr(CGGameUI::m_currentObjectTrack, TYPEMASK_OBJECT, __FILE__, __LINE__);

            if (object) {
                LogObjectInfo(object, "Current Object Track", buffer, bufferchars);
            }

            object = ClntObjMgrObjectPtr(CGGameUI::m_interactTarget, TYPEMASK_OBJECT, __FILE__, __LINE__);

            if (object) {
                LogObjectInfo(object, "Interact Target", buffer, bufferchars);
            }

            object = ClntObjMgrObjectPtr(CGGameUI::m_lockedTarget, TYPEMASK_OBJECT, __FILE__, __LINE__);

            if (object) {
                LogObjectInfo(object, "Locked Target", buffer, bufferchars);
            }

            object = ClntObjMgrObjectPtr(CGGameUI::m_lastEnemyTarget, TYPEMASK_OBJECT, __FILE__, __LINE__);

            if (object) {
                LogObjectInfo(object, "Last Enemy Target", buffer, bufferchars);
            }

            object = ClntObjMgrObjectPtr(CGGameUI::GetCursorItem(), TYPEMASK_ITEM, __FILE__, __LINE__);

            if (object) {
                LogObjectInfo(object, "Cursor Item", buffer, bufferchars);
            }

            char line[1024];
            line[0] = '\0';

            SStrPrintf(line, sizeof(line), "Total lua memory: %dKB\n", FrameScript_GetTotalMemoryUsed());
            SStrPack(buffer, line, bufferchars);

            SStrPrintf(line, sizeof(line), "Current Addon: %s\n", lua_tainted);
            SStrPack(buffer, line, bufferchars);

            char function[1024];
            FrameScript_GetCurrentFunction(FrameScript_GetContext(), function, sizeof(function));
            SStrPrintf(line, sizeof(line), "Current Addon function: %s\n", function);
            SStrPack(buffer, line, bufferchars);

            SStrPrintf(line, sizeof(line), "Current Addon object: %s\n", FrameScript_GetCurrentObject(FrameScript_GetContext(), 2));
            SStrPack(buffer, line, bufferchars);

            if (GetNumAddOns()) {
                SStrPack(buffer, "Add Ons: ", bufferchars);

                for (uint32_t i = 0; i < GetNumAddOns(); i++) {
                    const char* name = GetAddOnByIndex(i);

                    if (IsAddOnLoading(name)) {
                        SStrPack(buffer, name, bufferchars);
                        SStrPack(buffer, " ", bufferchars);
                    }
                }

                SStrPack(buffer, "\r\n", bufferchars);
            }
        }
    }

    SStrPack(buffer, "Settings: \r\n", bufferchars);
    CVar::LogAll(buffer, bufferchars, 0, 64);
    GxLogCrashInfo(buffer, bufferchars);

    return 1;
}

// OFFSET: 0x00406A70/0x140008340
int32_t ForEveryRunOnceWTF::AppendFileName(OS_FILE_DATA& data, void* param) {
    static_cast<TSGrowableArray<char*>*>(param)->New(SStrDupA(data.fileName, __FILE__, __LINE__));
    return 0;
}

// OFFSET: 0x00407090/0x140008C40
void ForEveryRunOnceWTF::Execute(int32_t (*callback)(const char*)) {
    char basePath[260];
    SFile::GetBasePath(basePath, sizeof(basePath));

    char directory[260];
    SStrCopy(directory, "WTF\\", sizeof(directory));
    OsFileList(directory, "RunOnce*.wtf", ForEveryRunOnceWTF::AppendFileName, &this->m_files, 1);

    uint32_t count = this->m_files.Count();
    qsort(this->m_files.Ptr(), count, sizeof(char*), ForEveryRunOnceWTF::FileNameCompare);

    for (uint32_t i = 0; i < count; i++) {
        callback(this->m_files[i]);
        SMemFree(this->m_files[i], __FILE__, __LINE__, 0);
    }
}

// OFFSET: 0x00407BA0
static void AbortInitializeGlobal() {
    CloseAllArchives();
    DestroyArchivePreloads();
    WoWReport::Shutdown();
    PatchOnDemand::Cache::Shutdown();
    BackgroundCache::ShutdownManager();
    SFile::ShutdownStreaming();
    ArchiveProgressDestroy();

    {
        ForEveryRunOnceWTF runOnce;
        runOnce.Execute(CVar::Delete);
    }

    ConsoleDestroyClientCVar();
    ConsoleDestroyClientCommand();
    CStringManager::DestroyManager();
}

// OFFSET: 0x008C1630
static const char* GetDefaultLocale() {
    int32_t current = GetCurrentLocaleIndex();

    if (g_hasIsoLocale[current]) {
        return g_localeNames[current];
    }

    int32_t index;

    for (index = 0; index < 15 && !g_hasIsoLocale[index]; index++) {
    }

    return g_localeNames[index == 15 ? current : index];
}

// OFFSET: 0x00407E10
bool InitializeGlobal() {
    SCmdRegisterArgList(s_wowArgList, sizeof(s_wowArgList) / sizeof(s_wowArgList[0]));
    CmdLineProcess();
    ClientServices::LoadCDKey();
    ConsoleInitializeClientCommand();

    const char* config = CmdLineGetString(static_cast<CMDOPT>(CMD_CONFIG));

    if (config && *config) {
        ConsoleInitializeClientCVar(config);
    } else {
        ConsoleInitializeClientCVar("Config.wtf");
    }

    CVar::ArchiveCodeRegisteredOnly();

    {
        ForEveryRunOnceWTF runOnce;
        runOnce.Execute(ConsoleLoadClientCVarAndSave);
    }

    CheckAvailableLocales();
    CVar* locale = CVar::Register("locale", "Set the game locale", 0x800, GetDefaultLocale(), LocaleCallback, CATEGORY_DEFAULT, false, NULL, false);
    CVar::Register("useEnglishAudio", "override the locale and use English audio", 0, "0", NULL, CATEGORY_DEFAULT, false, NULL, false);
    SetLocaleCode(locale->m_stringValue.m_str);
    ClientServices::InitLoginServerCVars();
    BackgroundCache::SetLocale(locale->m_stringValue.m_str);
    LoadInstallLocation();

    if (s_unk_B2FA10 != 2) {
        UpdateInstallLocation();
    }

    SStrCopy(s_placeholderArchivePath, s_dataDirectory, sizeof(s_placeholderArchivePath));
    SStrPack(s_placeholderArchivePath, "placeholderArchive.temp", sizeof(s_placeholderArchivePath));
    FindArchivesToPreload();

    if (!InitializeStreamingManifest()) {
        DestroyArchivePreloads();

        {
            ForEveryRunOnceWTF runOnce;
            runOnce.Execute(CVar::Delete);
        }

        ConsoleDestroyClientCVar();
        ConsoleDestroyClientCommand();
        CStringManager::DestroyManager();
        return false;
    }

    PatchOnDemandInitialize();
    CVar* imitateTrial = CVar::Lookup("imitateTrial");

    if (!imitateTrial || !imitateTrial->m_intValue) {
        CVar* accountType = CVar::Lookup("accounttype");
        SFile::SetTrial(accountType && !_stricmp(accountType->m_stringValue.m_str, "RT"));
    }

    StartupReport();

    char alternatePath[260];
    SStrCopy(alternatePath, s_dataDirectory, sizeof(alternatePath));
    SStrPack(alternatePath, locale->m_stringValue.m_str, sizeof(alternatePath));
    SFile::SetDataPathAlternate(alternatePath);
    SFile::RebuildHash();
    SFile::InitializeRepair(s_localeCode);
    ArchiveProgressInitialize(OsFileSupportsSparse());

    if (!PreloadArchives()) {
        ArchiveProgressDestroy();
        PatchOnDemand::Cache::Shutdown();
        BackgroundCache::ShutdownManager();
        SFile::ShutdownStreaming();
        DestroyArchivePreloads();
        ConsoleDestroyClientCVar();
        ConsoleDestroyClientCommand();
        CStringManager::DestroyManager();
        return false;
    }

    if (!OpenArchives()) {
        AbortInitializeGlobal();
        return false;
    }

    BGLoadingThrottleCVarInitialize();

    if (s_startBGLoadThread) {
        BGLoading::StartBGLoadThread();
        s_startBGLoadThread = 0;
    }

    g_startup_StringsDB.Load(&Startup_StringsRec::s_meta);

    if (!(OsGetProcessorFeatures() & 0x4)) {
        Startup_StringsRec* titleRec = g_startup_StringsDB.GetRecord(1);
        const char* title = titleRec ? titleRec->m_message : "World of Warcraft";
        Startup_StringsRec* textRec = g_startup_StringsDB.GetRecord(13);
        const char* text = textRec ? textRec->m_message : "World of Warcraft requires SSE (Streaming SIMD Extensions) to run.  Please run on a computer that supports SSE.";
        OsGuiMessageBox(NULL, 1, text, title);
        BGLoading::StopBGLoadThread();
        PatchOnDemand::Cache::Shutdown();
        BackgroundCache::ShutdownManager();
        ConsoleDestroyClientCVar();
        ConsoleDestroyClientCommand();
        BaseDestroyGlobal();
        CloseAllArchives();
        g_startup_StringsDB.Unload(&Startup_StringsRec::s_meta);
        return false;
    }

    CVar::Register("simd", "Enable SIMD features (e.g. SSE)", 0, "255", SimdCallback, CATEGORY_DEFAULT, false, NULL, false);
    CVar* affinity = CVar::Register("processAffinityMask", "Sets which core(s) WoW may execute on - changes require restart to take effect", 2, "0", ProcessAffinityMaskCallback, CATEGORY_DEBUG, false, NULL, false);
    CVar* videoOptionsVersion = CVar::Lookup("videoOptionsVersion");

    if (!videoOptionsVersion || videoOptionsVersion->m_intValue < 3) {
        char value[8];
        SStrPrintf(value, sizeof(value), "%u", 0);
        affinity->Set(value, true, false, false, true);
        affinity->Update();
    }

    uint32_t mask = affinity->m_intValue;

    if (mask) {
        SSetCurrentProcessAffinityMask(mask);
    }

    BaseInitializeGlobal();

    CVar* timingTestError = CVar::Register("timingTestError", "Error reported by the timing validation system", 6, "0", NULL, CATEGORY_DEFAULT, false, NULL, false);
    CVar* timingMethod = CVar::Register("timingMethod", "Desired method for game timing", 2, "0", TimingMethodCallback, CATEGORY_DEFAULT, false, timingTestError, false);
    OsTimeManagerCreate(static_cast<OsTimeManager::TimingMethod>(timingMethod->m_intValue));
    ConsoleCommandRegister("timingInfo", CCommand_TimingInfo, CATEGORY_DEBUG, NULL, NULL);

    s_timeTestError = OsTimeGetTestError();

    if (s_timeTestError != static_cast<uint32_t>(timingTestError->m_intValue)) {
        char buffer[16];
        sprintf(buffer, "%d", s_timeTestError);
        timingTestError->SetReadOnly(false);
        timingTestError->Set(buffer, true, false, false, true);
        timingTestError->Update();
        timingTestError->SetReadOnly(true);
        ConsolePrintf("Timing test error: %d", timingTestError);
    }

    EventInitialize(1, 0);
    g_rndSeed.SetSeed(OsGetAsyncTimeMs());
    g_clientEventContext = EventCreateContextEx(1, InitializeEngineCallback, DestroyEngineCallback, 0, 0);

    return true;
}

// OFFSET: 0x00401790/0x140003F00
static int32_t PollNet(const void* data, void* param) {
    RealmConnection::PollNet(NULL, NULL);
    BackgroundCache::Pump();
    BGLoading::UpdateLauncherNotification();

    if (AsyncFileReadHasStreamingErrorOccurred() | World::GetLoadFailed()) {
        if (ClientServices::Connection() && ClientServices::Connection()->unk_2F3C) {
            ClientServices::Connection()->Disconnect(static_cast<DISCONNECT_REASON>(16));
        }

        AsyncFileReadResetStreamingError();
        World::ClearLoadFailed();
    }

    return 1;
}

// OFFSET: 0x00404180/0x140004010
static void InitializeClient() {
    SysMsgInitialize();
    WowSysMessageOutput::Initialize();
    ConsoleCommandRegister("reloadUI", CCommand_ReloadUI, CATEGORY_GRAPHICS, NULL, NULL);
    ConsoleCommandRegister("perf", CCommand_Perf, CATEGORY_DEBUG, NULL, NULL);
    ClientRegisterConsoleCommands();
    ClientDBInitialize();
    LoadingScreenInitialize();
    FrameScript_Initialize(0);
    SI3::Init(0);
    FrameXML_RegisterDefault();
    GlueScriptEventsInitialize();
    ScriptEventsInitialize();
    ClientServices::Initialize();
    ClientServices::SetMessageHandler(SMSG_TUTORIAL_FLAGS, CGTutorial::OnTutorialFlags, NULL);

    CVar* voiceChat = CVar::Lookup("EnableVoiceChat");

    if (voiceChat && voiceChat->m_intValue) {
        ComSatClient_Init();
    }

    DBCache_RegisterHandlers();
    DBCache_Initialize();
    FrameTime::Initialize(0);
    FFX::InitCVars();
    World::Initialize();
    CCharacterComponent::Initialize();
    GxuLightInitialize();
    GxuLightSetGridSize(16.665001f);
    OsInputSetJoystickPollFcn(OsPollJoysticks);
    InputControlInitialize();
    CGlueMgr::Initialize();

    if (GetConsoleMessage()) {
        CGlueMgr::AddChangedOptionWarning(GetConsoleMessage());
        SetConsoleMessage(NULL);
    }

    int32_t intro = g_playIntroMovie->m_intValue;

    if (intro > 3) {
        CGlueMgr::SetScreen(0);
    } else {
        g_playIntroMovie->Set(intro + 1, true, false, false, true);
        CGlueMgr::SetScreen(5);
    }

    CGlueMgr::m_pendingTimerAlert = s_timeTestError;
    EventRegister(EVENT_ID_POLL, PollNet);
    CGPlayer_C::PlayerInitializeClientHandlers();
}

// OFFSET: 0x00407600/0x1400098A0
int32_t InitializeEngineCallback(const void* data, void* param) {
    BaseInitializeContext();
    AsyncFileReadRegisterCVars();
    ArchiveProgressSetStage(2);

    if (!PreloadList::DownloadAndWait("TrialLists\\StartClient.lst", 1, 0)) {
        EventUnregister(EVENT_ID_DESTROY, DestroyEngineCallback);
        AsyncFileReadDestroy();
        ClientPostClose(0);
        ObjectAllocDestroy();
        BaseDestroyContext();
        return 0;
    }

    ArchiveProgressDestroy();

    Startup_StringsRec* titleRec = g_startup_StringsDB.GetRecord(1);
    char title[260];
    SStrCopy(title, titleRec ? titleRec->m_message : "World of Warcraft", sizeof(title));

    ConsoleAccessSetEnabled(CmdLineGetBool(static_cast<CMDOPT>(CMD_CONSOLE)) == 1);

    if (!ConsoleDeviceInitialize(title, 1, 1)) {
        EventUnregister(EVENT_ID_DESTROY, DestroyEngineCallback);
        AsyncFileReadDestroy();
        ClientPostClose(0);
        ObjectAllocDestroy();
        BaseDestroyContext();
        return 0;
    }

    DefaultShaders::Initialize();
    OsIMEInitialize();
    s_engineInitialized = 1;
    s_startWorldCommonList = PreloadList::StartDownload("TrialLists\\StartWorldCommon.lst", 1);
    TextureInitialize();
    ModelBlobLoad("world\\model.blob");

    if (SFile::IsStreamingMode()) {
        TextureLoadBlob("world\\liquid.tex");
    }

    ScrnInitialize(0);
    ConsoleScreenInitialize(NULL);
    s_cvarUIFaster = CVar::Register("UIFaster", "UI acceleration option", 0, "3", UIFasterCallback, CATEGORY_GRAPHICS, false, NULL, false);
    s_cvTextureCacheSize = CVar::Register("textureCacheSize", "Texture cache size in bytes", 1, "32", TextureCacheSizeCallback, CATEGORY_GRAPHICS, false, NULL, false);
    TextureSetCacheSize(s_cvTextureCacheSize->m_intValue << 20);

    uint32_t m2Flags = GetM2CvarFlags();
    AddConsoleDeviceDefaultCallback(SetDefaults);

    if (ConsoleDeviceHardwareChanged()) {
        for (uint32_t page = OPTION_PAGE_0; page < NUM_OPTION_PAGES; page++) {
            SetDefaults(static_cast<OPTION_PAGE>(page));
        }
    }

    M2Initialize(m2Flags, 0);
    InitializeClient();

    return 1;
}

// OFFSET: #0x00406B70
void BeginCloseGame() {
    WoWReport::Shutdown();
    SMemSetDebugFlags(0, 8);
    OsIMEDestroy();
    OsTimeShutdown();
    EventDestroy();
    ConsoleDeviceDestroy();

    {
        ForEveryRunOnceWTF runOnce;
        runOnce.Execute(CVar::Delete);
    }

    ConsoleDestroyClientCVar();
    ConsoleDestroyClientCommand();
    BaseDestroyGlobal();
    CloseAllArchives();
    ShutdownStreamingIfTrial();
    Blizzard::Streaming::ShutdownStreaming();

    if (s_closeErrorStringId) {
        Startup_StringsRec* title = g_startup_StringsDB.GetRecord(1);
        Startup_StringsRec* message = g_startup_StringsDB.GetRecord(s_closeErrorStringId);
        OsGuiMessageBox(NULL, 0, message ? message->m_message : "Unknown Error", title ? title->m_message : "World of Warcraft");
    }
}

// OFFSET: 0x00408340/0x14000A610
int32_t CommonMain() {
    OsGuiSetGxDevWindowFcn(GxDevWindow);
    StormInitialize();
    SErrCatchUnhandledExceptions();
    CreateNamedEventA("Blizzard Entertainment World of Warcraft", 0);

    int32_t option = 1;
    StormSetOption(10, &option, sizeof(option));
    option = 1;
    StormSetOption(11, &option, sizeof(option));

    OsSystemEnableCpuLog();
    SetPaths();

    uint32_t sendErrorLogs = 1;

    if (!SRegLoadValue("World of Warcraft\\Client", "SendErrorLogs", 0, &sendErrorLogs)) {
        sendErrorLogs = 1;
        SRegSaveValue("World of Warcraft\\Client", "SendErrorLogs", 0, 1);
    }

    SErrSetLogTitleString("World of WarCraft: Retail Build (build 15595)");
    SErrSetLogCallback(WowLogHeader);

    if (sendErrorLogs) {
        SErrRegisterHandler(SendErrorLog);
    }

    if (InitializeGlobal()) {
        EventDoMessageLoop();
        BeginCloseGame();
    }

    SMemFree(s_mfilDownloadErrors, __FILE__, __LINE__, 0);
    s_mfilDownloadErrors = NULL;
    StormDestroy();

    return 0;
}

// OFFSET: #0x00406EF0
void GameClientCommands::Uninstall() {
    ConsoleCommandUnregister("port");
    ConsoleCommandUnregister("charmport");
    ConsoleCommandUnregister("worldport");
    ConsoleCommandUnregister("setrawpos");
    ConsoleCommandUnregister("showplayer");
    ConsoleCommandUnregister("togglehelm");
    ConsoleCommandUnregister("togglecloak");
}

// OFFSET: #0x00407870
void GameClientCommands::Install() {
    ConsoleCommandRegister("port", CCommand_Teleport, CATEGORY_DEBUG, NULL, NULL);
    ConsoleCommandRegister("charmport", CCommand_CharmTeleport, CATEGORY_DEBUG, NULL, NULL);
    ConsoleCommandRegister("worldport", CCommand_WorldTeleport, CATEGORY_DEBUG, NULL, NULL);
    ConsoleCommandRegister("setrawpos", CCommand_SetRawPos, CATEGORY_DEBUG, NULL, NULL);
    ConsoleCommandRegister("showplayer", CCommand_TogglePlayer, CATEGORY_GRAPHICS, NULL, NULL);
    ConsoleCommandRegister("togglehelm", CCommand_ToggleHelm, CATEGORY_GAME, NULL, NULL);
    ConsoleCommandRegister("togglecloak", CCommand_ToggleCloak, CATEGORY_GAME, NULL, NULL);
}

// OFFSET: 0x00403290
int32_t Packet_SMSG_TRANSFER_ABORTED(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    uint32_t mapID;
    uint8_t reason;
    uint8_t arg;
    msg->Get(mapID);
    msg->Get(reason);
    msg->Get(arg);
    MapRec* map = g_mapDB.GetRecord(mapID);

    if (map) {
        const char* text = NULL;
        char token[64];
        char buffer[1024];

        switch (reason) {
            case 1:
                text = FrameScript_GetText("TRANSFER_ABORT_ERROR", -1, GENDER_NOT_APPLICABLE);
                break;
            case 2:
                text = FrameScript_GetText("TRANSFER_ABORT_MAX_PLAYERS", -1, GENDER_NOT_APPLICABLE);
                break;
            case 3:
            case 12:
            case 13:
            case 14:
                text = FrameScript_GetText("TRANSFER_ABORT_NOT_FOUND", -1, GENDER_NOT_APPLICABLE);
                break;
            case 4:
                text = FrameScript_GetText("TRANSFER_ABORT_TOO_MANY_INSTANCES", -1, GENDER_NOT_APPLICABLE);
                break;
            case 6:
                text = FrameScript_GetText("TRANSFER_ABORT_ZONE_IN_COMBAT", -1, GENDER_NOT_APPLICABLE);
                break;
            case 7:
                SStrPrintf(token, sizeof(token), "TRANSFER_ABORT_INSUF_EXPAN_LVL%d", arg);
                text = FrameScript_GetText(token, -1, GENDER_NOT_APPLICABLE);
                break;
            case 8: {
                MapDifficultyRec* entry = GetMapDifficultyRec(map->m_id, arg, NULL);

                if (!entry || !(text = static_cast<const char*>(entry->m_message)) || !*text) {
                    SStrPrintf(token, sizeof(token), "TRANSFER_ABORT_DIFFICULTY%d", arg + 1);
                    text = FrameScript_GetText(token, -1, GENDER_NOT_APPLICABLE);
                }

                break;
            }
            case 9:
                SStrPrintf(token, sizeof(token), "TRANSFER_ABORT_UNIQUE_MESSAGE%d", arg);
                text = FrameScript_GetText(token, -1, GENDER_NOT_APPLICABLE);
                break;
            case 10:
                text = FrameScript_GetText("TRANSFER_ABORT_TOO_MANY_REALM_INSTANCES", -1, GENDER_NOT_APPLICABLE);
                break;
            case 11:
                text = FrameScript_GetText("TRANSFER_ABORT_NEED_GROUP", -1, GENDER_NOT_APPLICABLE);
                break;
            case 15:
                text = FrameScript_GetText("TRANSFER_ABORT_REALM_ONLY", -1, GENDER_NOT_APPLICABLE);
                break;
            case 16:
                text = FrameScript_GetText("TRANSFER_ABORT_MAP_NOT_ALLOWED", -1, GENDER_NOT_APPLICABLE);
                break;
            case 18: {
                MapDifficultyRec* entry = GetMapDifficultyRec(mapID, arg, NULL);
                const char* difficulty;

                if (entry && (difficulty = static_cast<const char*>(entry->m_difficultystring)) && *difficulty) {
                    const char* mapName = static_cast<const char*>(map->m_mapName);
                    const char* difficultyText = FrameScript_GetText(difficulty, -1, GENDER_NOT_APPLICABLE);
                    char name[64];
                    SStrPrintf(name, sizeof(name), FrameScript_GetText("DUNGEON_NAME_WITH_DIFFICULTY", -1, GENDER_NOT_APPLICABLE), mapName, difficultyText);
                    SStrPrintf(buffer, sizeof(buffer), FrameScript_GetText("TRANSFER_ABORT_LOCKED_TO_DIFFERENT_INSTANCE", -1, GENDER_NOT_APPLICABLE), name);
                    CGChat::AddChatMessage(buffer, static_cast<SLASH_COMMAND_ID>(0), NULL);
                } else {
                    text = FrameScript_GetText("TRANSFER_ABORT_LOCKED_TO_DIFFERENT_INSTANCE", -1, GENDER_NOT_APPLICABLE);
                }

                break;
            }
            case 19:
                text = FrameScript_GetText("TRANSFER_ABORT_ALREADY_COMPLETED_ENCOUNTER", -1, GENDER_NOT_APPLICABLE);
                break;
            default:
                break;
        }

        if (text && *text) {
            SStrPrintf(buffer, sizeof(buffer), text, static_cast<const char*>(map->m_mapName));
            CGChat::AddChatMessage(buffer, static_cast<SLASH_COMMAND_ID>(0), NULL);
        }
    }

    ConsolePrintf("World transfer aborted...");
    LoadingScreenDisable();
    return 1;
}

// OFFSET: #0x004034F0
int32_t STORMAPI SendErrorLog(uint32_t errorcode, const char* description, const char* filename, int32_t linenumber, const char* errorstring) {
    char path[260];

    if (SErrGetLogLastPath(path, sizeof(path))) {
        char commandLine[520];
        SStrPrintf(commandLine, sizeof(commandLine), "%s %s", "WowError.exe", path);
        SCreateProcess("WowError.exe", commandLine, NULL, NULL, NULL, NULL);
    }

    return 1;
}

// OFFSET: #0x00404EE0
static void ClientBeginTaxiBenchmarkMode() {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player || !(player->m_player->m_playerFlags & 0x20000)) {
        return;
    }

    s_clientTaxiBenchEnabled = 1;
    s_clientTaxiBenchMinFPS = 99999.0;
    s_clientTaxiBenchIsOnTaxi = 1;
    s_clientTaxiBenchTotalFrames = 0;
    s_clientTaxiBenchMaxFPS = 0.0;
    s_clientTaxiBenchTotalTime = 0.0;
    s_clientTaxiBenchDelayTime = OsGetAsyncTimeMs() + 5000;
}

// OFFSET: #0x00404F60
static void ClientEndTaxiBenchmarkMode(int32_t silent) {
    if (!s_clientTaxiBenchEnabled) {
        return;
    }

    s_clientTaxiBenchEnabled = 0;
    s_clientTaxiBenchIsOnTaxi = 0;

    if (silent || !s_clientTaxiBenchTotalFrames) {
        return;
    }

    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player || player->IsDeadOrGhost()) {
        return;
    }

    char text[3000];
    CGChat::AddChatMessage(FrameScript_GetText("BENCHMARK_TAXI_RESULTS", -1, GENDER_NOT_APPLICABLE), CHAT_MSG_SYSTEM, NULL, 0, NULL, NULL, NULL, WOWGUID(), 0, WOWGUID(), 0, false, NULL);
    SStrPrintf(text, sizeof(text), FrameScript_GetText("BENCHMARK_TAXI_MIN_FPS", -1, GENDER_NOT_APPLICABLE), s_clientTaxiBenchMinFPS);
    CGChat::AddChatMessage(text, CHAT_MSG_SYSTEM, NULL, 0, NULL, NULL, NULL, WOWGUID(), 0, WOWGUID(), 0, false, NULL);
    SStrPrintf(text, sizeof(text), FrameScript_GetText("BENCHMARK_TAXI_MAX_FPS", -1, GENDER_NOT_APPLICABLE), s_clientTaxiBenchMaxFPS);
    CGChat::AddChatMessage(text, CHAT_MSG_SYSTEM, NULL, 0, NULL, NULL, NULL, WOWGUID(), 0, WOWGUID(), 0, false, NULL);
    SStrPrintf(text, sizeof(text), FrameScript_GetText("BENCHMARK_TAXI_AVERAGE_FPS", -1, GENDER_NOT_APPLICABLE), s_clientTaxiBenchTotalFrames / s_clientTaxiBenchTotalTime);
    CGChat::AddChatMessage(text, CHAT_MSG_SYSTEM, NULL, 0, NULL, NULL, NULL, WOWGUID(), 0, WOWGUID(), 0, false, NULL);
}

// OFFSET: #0x00405130
void ClientUpdateTaxiBenchmarkMode() {
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player) {
        return;
    }

    if (s_clientTaxiBenchIsOnTaxi) {
        if (!(player->m_unit->m_unitFieldFlags & 0x100000) || !(player->m_player->m_playerFlags & 0x20000)) {
            ClientEndTaxiBenchmarkMode(0);
        }
    } else if ((player->m_unit->m_unitFieldFlags & 0x100000) && (player->m_player->m_playerFlags & 0x20000)) {
        ClientBeginTaxiBenchmarkMode();
    }

    if (!s_clientTaxiBenchEnabled) {
        return;
    }

    uint64_t now = OsGetAsyncTimeClocks();
    double elapsed = static_cast<double>(static_cast<int64_t>(now - s_clientTaxiTimestamp)) / static_cast<double>(OsGetAsyncClocksPerSecond());

    if (elapsed <= 0.0) {
        return;
    }

    s_clientTaxiTimestamp = now;

    if (static_cast<int32_t>(OsGetAsyncTimeMs() - s_clientTaxiBenchDelayTime) < 0) {
        return;
    }

    s_clientTaxiBenchTotalFrames++;
    double fps = 1.0 / elapsed;
    s_clientTaxiBenchTotalTime += elapsed;

    if (fps <= s_clientTaxiBenchMinFPS) {
        s_clientTaxiBenchMinFPS = fps;
    }

    if (fps >= s_clientTaxiBenchMaxFPS) {
        s_clientTaxiBenchMaxFPS = fps;
    }
}

// OFFSET: #0x00403FC0
void CheckAndSendBotDetected() {
    if (!s_botCheckEnabled) {
        return;
    }

    ClientConnection* connection = ClientServices::Connection();

    if (!s_gameInitialized || !connection) {
        return;
    }

    uint32_t now = OsGetAsyncTimeMs();

    if (now - s_botCheckStartTime < 1000) {
        return;
    }

    s_botCheckStartTime = now;

    if (--s_botCheckDelay > 0) {
        return;
    }

    s_botCheckEnabled = 0;

    uint8_t check1;
    uint8_t check2;
    uint8_t check3;

    if (!GatherAdditionalChecks(&check1, &check2, &check3)) {
        return;
    }

    SHA1 sha;
    sha.Prepare();
    sha.Process(connection->GetSessionKey(), sizeof(((LoginData*)0)->m_sessionKey));
    sha.Process(&check1, sizeof(check1));
    sha.Process(&check2, sizeof(check2));
    sha.Process(&check3, sizeof(check3));
    sha.Process(connection->GetSessionKey(), sizeof(((LoginData*)0)->m_sessionKey));
    SHA1::Digest digest;
    sha.Finish(digest);

    WDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_BOT_DETECTED));
    msg.Put(check1);
    msg.Put(check2);
    msg.Put(check3);
    msg.PutArray(digest.m_data, sizeof(digest.m_data));
    msg.Finalize();
    ClientServices::Send2(&msg);
}

// OFFSET: 0x00401300
int32_t Packet_SMSG_PLAYED_TIME(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    int32_t total;
    int32_t level;
    uint8_t show = 0;
    msg->Get(reinterpret_cast<uint32_t&>(total));
    msg->Get(reinterpret_cast<uint32_t&>(level));
    msg->Get(show);

    WOWGUID guid = ClntObjMgrGetActivePlayer();

    if (guid) {
        CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_PLAYER, __FILE__, __LINE__));

        if (player) {
            player->SetPlayedTime(total);
        }
    }

    if (show) {
        char text[256];
        ConsolePrintf("Time played:");
        FormatTime(text, sizeof(text), total);
        ConsolePrintf("Total: %s", text);
        FormatTime(text, sizeof(text), level);
        ConsolePrintf("Level: %s", text);
        FrameScript_SignalEvent(SCRIPT_EVENT_TIME_PLAYED_MSG, "%d%d", total, level);
    }

    return 1;
}

// OFFSET: #0x004033B0
void ClientKillTimer(uint32_t id, EVENTHANDLERFUNC handler, const char* name) {
    EventKillTimer(id, handler, name);
}

// OFFSET: #0x00402720
void* Client_MemAlloc(uint32_t bytes) {
    return SMemAlloc(bytes, __FILE__, __LINE__, 0);
}

// OFFSET: #0x00402740
void Client_MemFree(void* ptr) {
    SMemFree(ptr, __FILE__, __LINE__, 0);
}

// OFFSET: #0x00404610
void SendBotDetected2(uint32_t msgId, uint32_t code) {
    WDataStore msg;
    msg.Put(msgId);
    msg.Put(code);
    msg.Finalize();
    ClientServices::Send2(&msg);
}

// OFFSET: #0x00403370
uint32_t ClientSetTimer(uint32_t delay, EVENTHANDLERFUNC handler, void* param) {
    return EventSetTimer(delay, handler, param);
}

// OFFSET: #0x00403380
uint32_t ClientSetTimer(uint32_t delay, EVENTHANDLERGUIDFUNC handler, uint64_t guid, void* param) {
    return EventSetTimer(delay, handler, guid, param);
}

// OFFSET: 0x00405F00/0x1400068F0
void ClientInitializeGame(uint32_t mapId, C3Vector position) {
    g_theGxDevicePtr->MasterEnableSet(GxMasterEnable_Fog, 1);
    SI3::SetWeatherKitID(-1);

    REALM_INFO* realm = ClientServices::GetSelectedRealm();

    for (uint32_t i = 0; i < static_cast<uint32_t>(g_cfg_ConfigsDB.m_numRecords); i++) {
        Cfg_ConfigsRec* config = i < static_cast<uint32_t>(g_cfg_ConfigsDB.m_numRecords) ? g_cfg_ConfigsDB.GetRecordByIndex(i) : NULL;

        if (config->m_realmType == realm->m_type) {
            s_isPvPRealm = config->m_playerKillingAllowed != 0;
            break;
        }
    }

    AccountDataInitialize(false);
    ClntObjMgrInitializeShared();
    ClntObjMgrInitializeStd(mapId);
    CGUnit_C::ClientInitialize();
    SI3::InitZoneSoundsHandler();
    SI3::InitZoneIntros();
    SI3::InitAmbienceFlavor();
    LootInitialize();
    CGGameUI::InitializeGame();
    CGWorldFrame::GetActiveCamera()->m_position = position;
    WorldTextInitialize();
    PlayerNameInitialize();
    WeaponTrailsInitialize();
    CGObject_C::Initialize();
    CGUnit_C::Initialize();
    CGItem_C::Initialize();
    CGGameObject_C::Initialize();
    PlayerClientInitialize();
    CGPlayer_C::Initialize();
    CGItem_C::Initialize();
    CGContainer_C::Initialize();
    CGCorpse_C::Initialize();
    CGDynamicObject_C::Initialize();
    FriendList::Initialize();
    VoidStorage_C_Initialize();
    SmartScreenRectClearAllGrids();
    MovementInit();
    EventRegister(EVENT_ID_IDLE, ClientIdle);
    ClientInitializeGameTime();

    CommandManager* manager = StaticSingleton<CommandManager>::Instance();
    manager->m_connection = ClientServices::Connection();
    GameClientCommands* commands = new (__FILE__, __LINE__) GameClientCommands;
    manager = StaticSingleton<CommandManager>::Instance();
    manager->m_commands[manager->m_count++] = commands;
    commands->Install();

    ClientServices::SetMessageHandler(SMSG_NOTIFICATION, Packet_SMSG_NOTIFICATION, NULL);
    ClientServices::SetMessageHandler(SMSG_PLAYED_TIME, Packet_SMSG_PLAYED_TIME, NULL);
    ClientTransferPending::s_handler = OnTransferPending;
    ClientServices::SetMessageHandler(SMSG_TRANSFER_ABORTED, Packet_SMSG_TRANSFER_ABORTED, NULL);
    ClientServices::SetMessageHandler(SMSG_LOGIN_VERIFY_WORLD, Packet_SMSG_LOGIN_VERIFY_WORLD, NULL);
    ClientServices::SetMessageHandler(SMSG_KICK_REASON, CGlueMgr::OnKickReasonMsg, NULL);
    ClientNewWorldAbort::s_handler = OnNewWorldAbort;
    ClientNotification::s_handler = OnNotification;
    ClientNewWorld::s_handler = OnNewWorld;
    ClientMessageBox::s_handler = OnMessageBox;
    ClientSuspendToken::s_handler = OnSuspendToken;

    if (!g_mapDB.GetRecord(mapId)) {
        DebugLog("Bad zone ID %i", mapId);
    }

    AsyncFileReadIngameProgressCallback(NULL, NULL);
    AsyncFileReadProgressCallback(LoadingScreenAsyncCallback, NULL);
    World::SetLoadProgressCallback(LoadingScreenWorldCallback, NULL);
    World::LoadMap(mapId, position);
    AsyncFileReadProgressCallback(NULL, NULL);
    World::SetLoadProgressCallback(NULL, NULL);

    WOWGUID player = ClntObjMgrGetActivePlayer();
    StaticSingleton<CCameraManager>::Instance()->SetGameCamera(4, player);
    CAnimKitManager::InitSystem();
    CAnimReplacementSetDef::InitSystem();
    BattlenetUI_EnterRealm();
    WoWReport::SetStage("wrld");
    s_cvarEnterWorld->Set("1", true, false, false, true);
    ConsoleSaveAllClientCVar();
    s_gameInitialized = 1;
}

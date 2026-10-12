#include <cctype>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include "Client.h"
#include "Engine/Source/Base/SIMD.h"
#include "Engine/Source/BlizzardCore/Source/Packages/Streaming/Streaming.h"
#include "Engine/Source/BlizzardCore/SFile2-Core.h"
#include "Engine/Source/ConsoleBase/ConsoleCommand.h"
#include "Engine/Source/ConsoleBase/ConsoleVar.h"
#include "Engine/Source/ConsoleBase/IConsole.h"
#include "Engine/Source/Event/Event.h"
#include "Engine/Source/Frame/CSimpleRender.h"
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "Engine/Source/Model2/M2Cache.h"
#include "Engine/Source/Os/Input.h"
#include "Engine/Source/Os/Processor.h"
#include "Engine/Source/Os/System.h"
#include "Engine/Source/Os/TimeManager.h"
#include "Engine/Source/Scrn/Scrn.h"
#include "Engine/Source/Services/AsyncFile.h"
#include "Engine/Source/Services/SysMessage.h"
#include "Engine/Source/Services/Texture.h"
#include "Engine/Source/Storm/H/storm.h"
#include "WoW/Source/Console/ConsoleClient.h"
#include "WoW/Source/ConsoleDevice/ConsoleDevice.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/Hardware.h"
#include "WoW/Source/WowSysMessageOutput.h"

static const char* s_filterNames[] = { "general", "world", "ui", "animation", "models", "objects", "sound", "movement", "network" };

// STATIC: 0x00D361E8
static CVar* s_M2ForceAdditiveParticleSortVar;

// STATIC: 0x00D36220
static CVar* s_M2BatchParticlesVar;

// STATIC: 0x00D36214
static CVar* s_M2BatchDoodadsVar;

// STATIC: 0x00D361EC
static CVar* s_M2UseThreadsVar;

// STATIC: 0x00D36340/0x140BEF680
static CVar* s_M2UseClipPlanesVar;

// STATIC: 0x00D36208/0x140BEF688
static CVar* s_M2UseZFillVar;

// STATIC: 0x00D3620C/0x140BEF690
static CVar* s_M2FasterDebugVar;

// STATIC: 0x00D361CC/0x140BEF698
static CVar* s_M2FasterVar;

// STATIC: 0x00D361E4/0x140BEF6A0
CVar* s_desktopGammaCvar;

// STATIC: 0x00D361D4/0x140BEF6A8
CVar* s_gammaCvar;

// STATIC: 0x00D36200/0x140BEF6B0
static CVar* s_cvErrorFilter;

// STATIC: 0x00D361F8/0x140BEF6B8
static CVar* s_cvErrorLevelMax;

// STATIC: 0x00D361E0/0x140BEF6C0
static CVar* s_cvErrorLevelMin;

// STATIC: 0x00D361FC/0x140BEF6C8
static CVar* s_cvShowErrors;

// STATIC: 0x00D36228/0x140BEF6D0
static CVar* s_cvErrors;

// STATIC: 0x00D3622C/0x140BEF6D8
static CVar* s_cvMouseSpeed;

// STATIC: 0x00D361C0/0x140BEF7F8
static CVar* s_cvScreenshotQuality;

// STATIC: 0x00D36338/0x140BEF800
static CVar* s_cvScreenshotFormat;

// STATIC: 0x00D361DC/0x140BEF808
CVar* g_readContestVar;

// STATIC: 0x00D36348/0x140BEF810
CVar* g_readScanningVar;

// STATIC: 0x00D36330/0x140BEF818
CVar* g_readTerminationWithoutNoticeVar;

// STATIC: 0x00D361D0/0x140BEF820
CVar* g_readEULAVar;

// STATIC: 0x00D36204/0x140BEF828
CVar* g_readTOSVar;

// STATIC: 0x00D361C8/0x140BEF7E0
CVar* g_installLocaleVar;

// STATIC: 0x00D3621C/0x140BEF830
CVar* g_lastCharacterIndexVar;

// STATIC: 0x00D361D8/0x140BEF838
CVar* g_checkAddonVersionVar;

// STATIC: 0x00D36224/0x140BEF840
static CVar* s_cvMovieSubtitle;

// STATIC: 0x00D3633C/0x140BEF848
CVar* g_playIntroMovie;

// STATIC: 0x00D361C4/0x140BEF870
CVar* g_blizzconVar;

// STATIC: 0x00D36344/0x140BEF850
CVar* g_accountUsesTokenVar;

// STATIC: 0x00D36210/0x140BEF858
CVar* g_accountListVar;

// STATIC: 0x00D361F4/0x140BEF860
CVar* g_accountNameVar;

CVar* s_cvarEnterWorld;

CVar* s_cvarUIFaster;

CVar* s_cvTextureCacheSize;

CVar* s_cvTextureFilteringMode;

// STATIC: 0x00D364C0/0x140BEFA08
static CVar* s_cvAsyncThreadSleep;

// STATIC: 0x00D364C4
static CVar* s_cvAsyncHandlerTimeout;

// STATIC: 0x00D732E4/0x140C2E630
char s_localeCode[5];

// STATIC: 0x00D361F0/0x140BEF7E8
CVar* g_disableServerNagleVar;

// STATIC: 0x00D36218/0x140BEF7F0
CVar* g_useIPv6Var;

// OFFSET: 0x00401400/0x1400011F0
static bool MouseSpeedCallback(CVar* var, const char* oldValue, const char* value, void* arg) {
    OsInputSetMouseSpeed(SStrToFloat(value));
    return true;
}

// OFFSET: 0x00401420/0x140001210
static bool ErrorDisplayCallback(CVar* var, const char* oldValue, const char* value, void* arg) {
    int32_t enable = SStrToInt(value);

    if (enable) {
        WowSysMessageOutput::Enable();
        ConsoleWrite("Error display enabled", DEFAULT_COLOR);
    } else {
        WowSysMessageOutput::Disable();
        ConsoleWrite("Error display disabled", DEFAULT_COLOR);
    }

    SysMsgEnable(enable);

    return true;
}

// OFFSET: 0x00401470/0x140001260
static bool ErrorDisplayShowCallback(CVar* var, const char* oldValue, const char* value, void* arg) {
    int32_t show = SStrToInt(value);

    if (show) {
        WowSysMessageOutput::Show();
        ConsoleWrite("Error display shown", DEFAULT_COLOR);
    } else {
        WowSysMessageOutput::Hide();
        ConsoleWrite("Error display hidden", DEFAULT_COLOR);
    }

    SysMsgEnable(show);

    return true;
}

// OFFSET: 0x00403890/0x140003100
static void DisplayErrorLevelStatus() {
    uint32_t minLevel = SysMsgGetMinDisplayLevel();
    uint32_t maxLevel = SysMsgGetMaxDisplayLevel();

    if (!minLevel && maxLevel == 3) {
        ConsoleWrite("Displaying all system messages", DEFAULT_COLOR);
        return;
    }

    char status[80];
    SStrCopy(status, "Displaying ", sizeof(status));

    if (minLevel == maxLevel) {
        SStrPack(status, "only ", sizeof(status));
    }

    switch (minLevel) {
        case 0:
            SStrPack(status, "informational messages", sizeof(status));
            break;

        case 1:
            SStrPack(status, "warnings", sizeof(status));
            break;

        case 2:
            SStrPack(status, "errors", sizeof(status));
            break;

        case 3:
            SStrPack(status, "fatal errors", sizeof(status));
            break;

        default:
            break;
    }

    if (minLevel != maxLevel) {
        switch (maxLevel) {
            case 1:
                SStrPack(status, " through warnings", sizeof(status));
                break;

            case 2:
                SStrPack(status, " through errors", sizeof(status));
                break;

            case 3:
                SStrPack(status, " through fatal errors", sizeof(status));
                break;
        }
    }

    ConsoleWrite(status, DEFAULT_COLOR);
}

// OFFSET: 0x00403B20/0x140003470
static bool ErrorDisplayMinLevelCallback(CVar* var, const char* oldValue, const char* value, void* arg) {
    uint32_t level = SStrToInt(value);

    if (level > 3) {
        ConsoleWriteA("%i is not valid, valid values are 0 - %i", DEFAULT_COLOR, level, 3);
        return false;
    }

    SysMsgSetMinDisplayLevel(level);
    DisplayErrorLevelStatus();

    return true;
}

// OFFSET: 0x00403B60/0x1400034C0
static bool ErrorDisplayMaxLevelCallback(CVar* var, const char* oldValue, const char* value, void* arg) {
    uint32_t level = SStrToInt(value);

    if (level > 3) {
        ConsoleWriteA("%i is not valid, valid values are 0 - %i", DEFAULT_COLOR, level, 3);
        return false;
    }

    SysMsgSetMaxDisplayLevel(level);
    DisplayErrorLevelStatus();

    return true;
}

// OFFSET: 0x00403BA0/0x140003510
static void PrintFilterMask() {
    uint32_t filter = SysMsgGetFilter();

    if (filter == 0xFFFFFFFF) {
        ConsoleWrite("Now filtering: all messages", DEFAULT_COLOR);
        return;
    }

    char names[80] = {};

    for (uint32_t i = 0; i < sizeof(s_filterNames) / sizeof(s_filterNames[0]); i++) {
        if ((1 << i) & filter) {
            SStrPack(names, s_filterNames[i], sizeof(names));
            SStrPack(names, " ", sizeof(names));
        }
    }

    ConsolePrintf("Now filtering: %s", names);
}

// OFFSET: 0x004014C0/0x1400012B0
static int32_t ErrorDisplaySetFilter(const char* filter) {
    int32_t except = 0;
    uint32_t mask = 0;
    char separators[] = "\t\r\n\" ";
    char token[64];

    SStrTokenize(&filter, token, sizeof(token), separators, NULL);

    while (token[0]) {
        uint32_t i;

        for (i = 0; i < sizeof(s_filterNames) / sizeof(s_filterNames[0]); i++) {
            if (!SStrCmpI(token, s_filterNames[i], STORM_MAX_STR)) {
                uint32_t bit = 1 << i;

                if (except) {
                    mask &= ~bit;
                } else {
                    mask |= bit;
                }

                break;
            }
        }

        if (i == sizeof(s_filterNames) / sizeof(s_filterNames[0])) {
            if (!SStrCmpI(token, "all", STORM_MAX_STR)) {
                mask = (except != 0) - 1;
            } else {
                if (!SStrCmpI(token, "except", STORM_MAX_STR)) {
                    ConsolePrintf("Unknown filter %s", token);
                    ConsoleWrite("Filters: general world ui animation models objects sound movement all", DEFAULT_COLOR);
                    ConsoleWrite("         use \"except\" to invert mask", DEFAULT_COLOR);
                    ConsoleWrite("         i.e.: all except objects", DEFAULT_COLOR);
                    return 0;
                }

                except = 1;
            }
        }

        SStrTokenize(&filter, token, sizeof(token), separators, NULL);
    }

    SysMsgSetFilter(mask);

    return 1;
}

// OFFSET: 0x00403C90/0x140003660
static bool ErrorDisplayFilterCallback(CVar* var, const char* oldValue, const char* value, void* arg) {
    if (!ErrorDisplaySetFilter(value)) {
        return false;
    }

    PrintFilterMask();

    return true;
}

// OFFSET: 0x004015F0/0x140003690
static bool GammaCallback(CVar* var, const char* oldValue, const char* value, void* arg) {
    float gamma = SStrToFloat(value);

    if (gamma < 0.001f) {
        gamma = 1.0f;
    }

    if (!s_desktopGammaCvar || s_desktopGammaCvar->m_intValue) {
        return true;
    }

    g_theGxDevicePtr->DeviceSetGamma(gamma);

    return true;
}

// OFFSET: 0x00401640/0x1400036E0
static bool DesktopGammaCallback(CVar* var, const char* oldValue, const char* value, void* arg) {
    if (SStrToInt(value)) {
        CGxGammaRamp ramp;
        g_theGxDevicePtr->DeviceGamma(ramp);
        g_theGxDevicePtr->DeviceSetGamma(ramp);
        return true;
    }

    float gamma = s_gammaCvar ? s_gammaCvar->m_floatValue : 1.0f;

    if (gamma < 0.001f) {
        gamma = 1.0f;
    }

    g_theGxDevicePtr->DeviceSetGamma(gamma);
    return true;
}

// OFFSET: 0x00401750/0x1400014A0
static bool ScreenshotFormatCallback(CVar* var, const char* oldValue, const char* value, void* arg) {
    ScrnSetScreenshotFormat(value);
    return true;
}

// OFFSET: 0x00401770/0x1400014C0
static bool ScreenshotQualityCallback(CVar* var, const char* oldValue, const char* value, void* arg) {
    ScrnSetScreenshotQuality(SStrToInt(value));
    return true;
}

// OFFSET: 0x00401890/0x1400014E0
static uint32_t GetM2FasterModifierMask(int32_t faster, int32_t debug) {
    uint32_t mask = 0;

    switch (faster) {
        case 0:
            if (!debug) {
                return 0;
            }

            switch (debug % 10) {
                case 1:
                    mask = 0x2000;
                    break;

                case 2:
                    mask = 0x6000;
                    break;

                case 3:
                    mask = 0xE000;
                    break;
            }

            if (debug / 100 % 10 && EventIsKeyDown(KEY_LCONTROL)) {
                return 0;
            }

            return mask;

        case 1:
            return 0xE000;

        case 2:
        case 3:
            return 0x2000;

        default:
            return mask;
    }
}

// OFFSET: 0x00401950/0x1400041B0
static bool M2FasterChanged(CVar* var, const char* oldValue, const char* value, void* arg) {
    int32_t faster = SStrToInt(value);

    if (s_M2FasterDebugVar) {
        M2SetGlobalOptFlags(GetM2FasterModifierMask(faster, s_M2FasterDebugVar->m_intValue));
    } else {
        M2SetGlobalOptFlags(GetM2FasterModifierMask(faster, 0));
    }

    return true;
}

// OFFSET: 0x004019A0/0x1400041F0
static bool M2DebugFasterChanged(CVar* var, const char* oldValue, const char* value, void* arg) {
    int32_t faster = s_M2FasterVar ? s_M2FasterVar->m_intValue : 0;
    M2SetGlobalOptFlags(GetM2FasterModifierMask(faster, SStrToInt(value)));
    return true;
}

// OFFSET: 0x004019E0/0x140004230
bool UIFasterCallback(CVar* var, const char* oldValue, const char* value, void* arg) {
    uint32_t faster = SStrToInt(value);

    if (faster > 3) {
        return false;
    }

    if (faster & 1) {
        Hardware* hardware = ConsoleGetHardware();

        if (hardware) {
            if (!hardware->m_videoHw && hardware->m_videoDevice.m_vendorID == 0x8086) {
                faster &= ~1u;
                ConsoleWrite("Texture atlas disabled.", DEFAULT_COLOR);
            }

            VideoHardwareRec* video = hardware->m_videoHw;

            if (video && video->m_atlasdisable) {
                faster &= ~1u;
                ConsoleWrite("Texture atlas disabled.", DEFAULT_COLOR);
            }
        }
    }

    TextureEnableAtlas(faster & 1);
    CSimpleRender::s_mergeEnable = (faster >> 1) & 1;

    return true;
}

// OFFSET: 0x00401A70/0x1400015C0
bool ProcessAffinityMaskCallback(CVar* var, const char* oldValue, const char* value, void* arg) {
    uint32_t maxMask = 0xFFFFFFFF >> (32 - OsGetNumberOfProcessors());
    uint32_t mask = atol(value);

    if (mask > maxMask) {
        ConsolePrintf("Specified mask %08x is greater than the maximum allowable value of %08x", mask, maxMask);
        return false;
    }

    return true;
}

// OFFSET: 0x004016D0/0x140001420
bool SimdCallback(CVar* var, const char* oldValue, const char* value, void* arg) {
    int32_t requested = SStrToInt(value);

    if (!requested) {
        requested = 255;
    }

    uint32_t processor = OsGetProcessorFeatures();
    int32_t features = 1;

    if (processor & 4) {
        features = 3;
    }

    if (processor & 0x10) {
        features |= 4;
    }

    if (processor & 0x80) {
        features |= 0x10;
    }

    if (processor & 0x100) {
        features |= 0x20;
    }

    if (processor & 0x200) {
        features |= 0x40;
    }

    SIMD::BindAll(requested & features);
    return true;
}

// OFFSET: 0x00401AB0/0x140001620
bool TextureCacheSizeCallback(CVar* var, const char* oldValue, const char* value, void* arg) {
    int32_t size = SStrToInt(value) << 20;
    int32_t maxSize = TextureCacheGetMaxSize();
    char message[256];

    if (size < 0 || size > maxSize) {
        SStrPrintf(message, sizeof(message), "Texture cache size (%d meg) greater than maximum allowed for your system (%d meg).", size >> 20, maxSize >> 20);
        ConsoleWrite(message, DEFAULT_COLOR);
        return false;
    }

    SStrPrintf(message, sizeof(message), "Texture cache size set to %d meg.", size >> 20);
    ConsoleWrite(message, DEFAULT_COLOR);
    TextureSetCacheSize(size);

    return true;
}

// OFFSET: 0x00401B50/0x1400016C0
static bool BatchDoodadsCallback(CVar* var, const char* oldValue, const char* value, void* arg) {
    uint32_t enable = SStrToInt(value);
    uint32_t flags = M2GetCacheFlags();

    if (enable) {
        M2SetCacheFlags(flags | 0x20);
        ConsoleWrite("Doodad batching enabled.", DEFAULT_COLOR);
    } else {
        M2SetCacheFlags(flags & ~0x20);
        ConsoleWrite("Doodad batching disabled.", DEFAULT_COLOR);
    }

    return true;
}

// OFFSET: 0x00401C10/0x140001780
static bool BatchParticlesCallback(CVar* var, const char* oldValue, const char* value, void* arg) {
    uint32_t enable = SStrToInt(value);
    uint32_t flags = M2GetCacheFlags();

    if (enable) {
        M2SetCacheFlags(flags | 0x80);
        ConsoleWrite("Particle batching enabled.", DEFAULT_COLOR);
    } else {
        M2SetCacheFlags(flags & ~0x80);
        ConsoleWrite("Particle batching disabled.", DEFAULT_COLOR);
    }

    return true;
}

// OFFSET: 0x00401BB0/0x140001720
static bool ForceAdditiveParticleSortCallback(CVar* var, const char* oldValue, const char* value, void* arg) {
    uint32_t enable = SStrToInt(value);
    uint32_t flags = M2GetCacheFlags();

    if (enable) {
        M2SetCacheFlags(flags | 0x100);
        ConsoleWrite("Sorting all particles as though they were additive.", DEFAULT_COLOR);
    } else {
        M2SetCacheFlags(flags & ~0x100);
        ConsoleWrite("Sorting particles normally.", DEFAULT_COLOR);
    }

    return true;
}

// OFFSET: 0x00401D90/0x140001920
static bool AsyncFileReadThreadSleepCallback(CVar* var, const char* oldValue, const char* value, void* arg) {
    return static_cast<uint32_t>(SStrToInt(value)) <= 100;
}

// OFFSET: 0x00401DB0/0x140001940
static bool AsyncFileReadHandlerTimeoutCallback(CVar* var, const char* oldValue, const char* value, void* arg) {
    return static_cast<uint32_t>(SStrToInt(value) - 20) <= 230;
}

// OFFSET: 0x004025C0/0x140005430
bool TimingMethodCallback(CVar* var, const char* oldValue, const char* value, void* arg) {
    uint32_t method = atol(value);

    if (method > 2) {
        ConsolePrintf("'%s' is not a valid timing method. Valid methods are:", value);

        for (int32_t i = 0; i <= 2; i++) {
            ConsolePrintf("  %d - %s", i, GetTimingMethodName(static_cast<OsTimeManager::TimingMethod>(i)));
        }

        return false;
    }

    if (oldValue && method != static_cast<uint32_t>(atol(oldValue))) {
        CVar* testError = static_cast<CVar*>(arg);

        if (testError->m_intValue) {
            testError->SetReadOnly(false);
            testError->Set("0", true, false, false, true);
            testError->SetReadOnly(true);
        }
    }

    return true;
}

// OFFSET: 0x00402730/0x1400020F0
bool LocaleCallback(CVar* var, const char* oldValue, const char* value, void* arg) {
    if (strlen(value) != 4) {
        return false;
    }

    char locale[5];
    locale[0] = static_cast<char>(tolower(value[0]));
    locale[1] = static_cast<char>(tolower(value[1]));
    locale[2] = static_cast<char>(toupper(value[2]));
    locale[3] = static_cast<char>(toupper(value[3]));
    locale[4] = 0;

    int32_t index = LocaleNameToIndex(locale);

    if (!strcmp(g_localeNames[index], value)) {
        return true;
    }

    if (!_stricmp(g_localeNames[index], value)) {
        var->Set(g_localeNames[index], true, false, false, true);
    }

    return false;
}

// OFFSET: 0x00403CC0/0x140003770
void ClientRegisterConsoleCommands() {
    g_blizzconVar = CVar::Register("blizzcon", "Blizzcon special behavior", 0x100, "0", NULL, CATEGORY_DEFAULT, false, NULL, false);

    if (g_blizzconVar->m_intValue) {
        OsInputInstallKeyboardHook();
        ConsoleAccessSetEnabled(0);
    }

    g_accountNameVar = CVar::Register("accountName", "Saved account name", 0x40, "", NULL, CATEGORY_GAME, false, NULL, false);
    g_accountListVar = CVar::Register("accountList", "List of wow accounts for saved Blizzard account", 0, "", NULL, CATEGORY_GAME, false, NULL, false);
    g_accountUsesTokenVar = CVar::Register("g_accountUsesToken", "Saved whether uses authenticator", 0, "0", NULL, CATEGORY_GAME, false, NULL, false);
    g_playIntroMovie = CVar::Register("playIntroMovie", "Starting expansion movie to play on startup", 0, "3", NULL, CATEGORY_GAME, false, NULL, false);
    s_cvMovieSubtitle = CVar::Register("movieSubtitle", "Show movie subtitles", 0, "0", NULL, CATEGORY_GAME, false, NULL, false);
    g_checkAddonVersionVar = CVar::Register("checkAddonVersion", "Check interface addon version number", 0, "1", NULL, CATEGORY_GAME, false, NULL, false);

    char mouseSpeed[32];
    SStrPrintf(mouseSpeed, sizeof(mouseSpeed), "%1.1f", OsInputGetMouseSpeed());
    s_cvMouseSpeed = CVar::Register("mouseSpeed", NULL, 0, mouseSpeed, MouseSpeedCallback, CATEGORY_GAME, false, NULL, false);

    s_cvErrors = CVar::Register("Errors", NULL, 0x400, "0", ErrorDisplayCallback, CATEGORY_DEBUG, false, NULL, false);
    s_cvShowErrors = CVar::Register("showErrors", NULL, 0, "1", ErrorDisplayShowCallback, CATEGORY_DEBUG, false, NULL, false);
    s_cvErrorLevelMin = CVar::Register("ErrorLevelMin", NULL, 0, "2", ErrorDisplayMinLevelCallback, CATEGORY_DEBUG, false, NULL, false);
    s_cvErrorLevelMax = CVar::Register("ErrorLevelMax", NULL, 0, "3", ErrorDisplayMaxLevelCallback, CATEGORY_DEBUG, false, NULL, false);
    s_cvErrorFilter = CVar::Register("ErrorFilter", NULL, 0, "all", ErrorDisplayFilterCallback, CATEGORY_DEBUG, false, NULL, false);
    s_desktopGammaCvar = CVar::Register("DesktopGamma", NULL, 0, "0", DesktopGammaCallback, CATEGORY_GRAPHICS, false, NULL, false);
    s_gammaCvar = CVar::Register("Gamma", NULL, 0, "1.0", GammaCallback, CATEGORY_GRAPHICS, false, NULL, false);
    g_lastCharacterIndexVar = CVar::Register("lastCharacterIndex", "Last character selected", 0, "0", NULL, CATEGORY_GAME, false, NULL, false);
    g_readTOSVar = CVar::Register("readTOS", "Status of the TOS", 0, "0", NULL, CATEGORY_GAME, false, NULL, false);
    g_readEULAVar = CVar::Register("readEULA", "Status of the EULA", 0, "0", NULL, CATEGORY_GAME, false, NULL, false);
    g_readTerminationWithoutNoticeVar = CVar::Register("readTerminationWithoutNotice", "Status of the Termination without Notice notice", 0, "0", NULL, CATEGORY_GAME, false, NULL, false);
    g_readScanningVar = CVar::Register("readScanning", "Status of the Scanning notice", 0, "0", NULL, CATEGORY_GAME, false, NULL, false);
    g_readContestVar = CVar::Register("readContest", "Status of the Contest notice", 0, "0", NULL, CATEGORY_GAME, false, NULL, false);
    s_cvScreenshotFormat = CVar::Register("screenshotFormat", "Set the format of screenshots", 1, "jpeg", ScreenshotFormatCallback, CATEGORY_GRAPHICS, false, NULL, false);
    s_cvScreenshotQuality = CVar::Register("screenshotQuality", "Set the quality of screenshots (1 - 10)", 1, "3", ScreenshotQualityCallback, CATEGORY_GRAPHICS, false, NULL, false);

    CVar* showToolsUI = CVar::Register("showToolsUI", "Display the launcher when starting the game", 0, "-1", NULL, CATEGORY_GAME, false, NULL, false);

    if (static_cast<uint32_t>(showToolsUI->m_intValue) >= 2) {
        showToolsUI->Set("1", true, false, false, true);
    }

    CVar::Register("accounttype", "Account Type", 0, "", NULL, CATEGORY_GAME, false, NULL, false);
    CVar::Register("expandUpgradePanel", "Controls whether the upgrade panel is expanded or collapsed.", 0, "1", NULL, CATEGORY_GAME, false, NULL, false);
    g_useIPv6Var = CVar::Register("useIPv6", "Enable the usage of IPv6 sockets", 1, "0", NULL, CATEGORY_NET, false, NULL, false);
    g_disableServerNagleVar = CVar::Register("disableServerNagle", "Disable server-side nagle algorithm", 1, "1", NULL, CATEGORY_NET, false, NULL, false);
    g_installLocaleVar = CVar::Register("installLocale", "The locale the original install was from", 1, "", NULL, CATEGORY_GAME, false, NULL, false);
}

// OFFSET: 0x00401DD0/0x1400042D0
void AsyncFileReadRegisterCVars() {
    s_cvAsyncThreadSleep = CVar::Register("asyncThreadSleep", "Engine option: Async read thread sleep", 1, "0", AsyncFileReadThreadSleepCallback, CATEGORY_DEBUG, false, NULL, false);
    s_cvAsyncHandlerTimeout = CVar::Register("asyncHandlerTimeout", "Engine option: Async read main thread timeout", 1, "100", AsyncFileReadHandlerTimeoutCallback, CATEGORY_DEBUG, false, NULL, false);
    AsyncFileReadInitialize(s_cvAsyncThreadSleep->m_intValue, s_cvAsyncHandlerTimeout->m_intValue);
}

// OFFSET: 0x00401E40/0x140004380
uint32_t GetM2CvarFlags() {
    s_M2UseZFillVar = CVar::Register("M2UseZFill", "z-fill transparent objects", 0, "1", NULL, CATEGORY_GRAPHICS, false, NULL, false);
    s_M2UseClipPlanesVar = CVar::Register("M2UseClipPlanes", "use clip planes for sorting transparent objects", 0, "1", NULL, CATEGORY_GRAPHICS, false, NULL, false);
    s_M2UseThreadsVar = CVar::Register("M2UseThreads", "multithread model animations", 0, "1", NULL, CATEGORY_GRAPHICS, false, NULL, false);
    s_M2BatchDoodadsVar = CVar::Register("M2BatchDoodads", "combine doodads to reduce batch count", 0, "1", BatchDoodadsCallback, CATEGORY_GRAPHICS, false, NULL, false);
    s_M2BatchParticlesVar = CVar::Register("M2BatchParticles", "combine particle emitters to reduce batch count", 0, "1", BatchParticlesCallback, CATEGORY_GRAPHICS, false, NULL, false);
    s_M2ForceAdditiveParticleSortVar = CVar::Register("M2ForceAdditiveParticleSort", "force all particles to sort as though they were additive", 0, "0", ForceAdditiveParticleSortCallback, CATEGORY_GRAPHICS, false, NULL, false);
    s_M2FasterVar = CVar::Register("M2Faster", "end user control of scene optimization mode - (0-3)", 0, "1", M2FasterChanged, CATEGORY_GRAPHICS, false, NULL, false);
    s_M2FasterDebugVar = CVar::Register("M2FasterDebug", "programmer control of scene optimization mode", 0, "0", M2DebugFasterChanged, CATEGORY_GRAPHICS, false, NULL, false);

    uint32_t flags = s_M2UseZFillVar->m_intValue != 0;

    if (s_M2UseClipPlanesVar->m_intValue) {
        flags |= 0x2;
    }

    if (s_M2UseThreadsVar->m_intValue) {
        flags |= 0x4;
    }

    if (s_M2BatchDoodadsVar->m_intValue) {
        flags |= 0x20;
    }

    if (s_M2BatchParticlesVar->m_intValue) {
        flags |= 0x80;
    }

    if (s_M2ForceAdditiveParticleSortVar->m_intValue) {
        flags |= 0x100;
    }

    return flags | 0x8;
}

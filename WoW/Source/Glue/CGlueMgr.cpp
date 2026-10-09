#include "CGlueMgr.h"
#include <cmath>
#include <cstdarg>
#include <cstring>
#include <new>
#include <typeinfo>
#include "Engine/Source/Base/CDataAllocator.h"
#include "Engine/Source/Base/CDataStore.h"
#include "Engine/Source/Base/Coordinate.h"
#include "Engine/Source/BlizzardCore/SFile2-Core.h"
#include "Engine/Source/BlizzardCore/Source/Packages/Streaming/WoWReport.h"
#include "Engine/Source/ConsoleBase/ConsoleCommand.h"
#include "Engine/Source/ConsoleBase/ConsoleVar.h"
#include "Engine/Source/ConsoleBase/IConsole.h"
#include "Engine/Source/Event/CSizeEvent.h"
#include "Engine/Source/Event/Event.h"
#include "Engine/Source/Event/FrameTime.h"
#include "WoW/Source/Game/GameClient/FFXEffects.h"
#include "Engine/Source/FFX/EffectGlow.h"
#include "Engine/Source/FFX/FFX.h"
#include "Engine/Source/Frame/CLayoutFrame.h"
#include "Engine/Source/Frame/CScriptObject.h"
#include "Engine/Source/Frame/CSimpleFrame.h"
#include "Engine/Source/Frame/CSimpleModel.h"
#include "Engine/Source/Frame/CSimpleTop.h"
#include "Engine/Source/Frame/SimpleFrameScript.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/FrameXML/FrameXML.h"
#include "Engine/Source/FrameXML/LoadXML.h"
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "Engine/Source/Gx/Gx.h"
#include "Engine/Source/Gxu/GxuScene.h"
#include "Engine/Source/Model2/M2Cache.h"
#include "Engine/Source/Model2/M2Model.h"
#include "Engine/Source/Model2/M2Scene.h"
#include "Engine/Source/Os/OsFile.h"
#include "Engine/Source/Os/OsURLDownload.h"
#include "Engine/Source/Os/Time.h"
#include "Engine/Source/Os/TimeManager.h"
#include "Engine/Source/Services/AsyncFile.h"
#include "Engine/Source/Services/Camera.h"
#include "Engine/Source/Services/Texture.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/Storm/Source/MD5.h"
#include "Engine/Source/Storm/Source/SLock.h"
#include "Engine/Source/Tempest/C3Vector.h"
#include "Engine/Source/Tempest/C44Matrix.h"
#include "Engine/Source/Tempest/CRect.h"
#include "WoW/Common/ComponentCore/CharacterComponent.h"
#include "WoW/Source/Client.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/ChrClassesRec.h"
#include "WoW/Source/DB/Rec/ChrRacesRec.h"
#include "WoW/Source/DB/Rec/GameTipsRec.h"
#include "WoW/Source/DB/Rec/GlueScreenEmoteRec.h"
#include "WoW/Source/DB/WowClientDB.h"
#include "WoW/Source/Game/GameClient/CDrawBowString.h"
#include "WoW/Source/Game/GameClient/PhaseShift_C.h"
#include "WoW/Source/Game/ValidateName.h"
#include "WoW/Source/Glue/CharacterCreation.h"
#include "WoW/Source/Glue/CharacterSelection.h"
#include "WoW/Source/Glue/CharacterSelectionDisplay.h"
#include "WoW/Source/Glue/CSimpleModelFFX.h"
#include "WoW/Source/Glue/CWOWClientStatus.h"
#include "WoW/Source/Glue/GlueMgr.h"
#include "WoW/Source/Glue/RealmList.h"
#include "WoW/Source/JamClientAutoCode/JamClient.h"
#include "WoW/Source/JamClientAutoCode/JamUserClient.h"
#include "WoW/Source/LoadingScreen.h"
#include "WoW/Source/Magic/MagicClient/SpellTable.h"
#include "WoW/Source/Magic/MagicClient/SpellVisuals.h"
#include "WoW/Source/Net/Types.h"
#include "WoW/Source/Object/ObjectClient/AnimKitManager.h"
#include "WoW/Source/Object/ObjectClient/Object_C.h"
#include "WoW/Source/Object/ObjectClient/ObjectEffect.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/SoundInterface/SI3.h"
#include "WoW/Source/Ui/CAccountMsgHeaders.h"
#include "WoW/Source/Ui/CKBSystem.h"
#include "WoW/Source/Ui/KnowledgeBase.h"
#include "WoW/Source/Ui/ScriptEvents.h"
#include "WoW/Source/Ui/SharedScripts.h"
#include "WoW/Source/Ui/UILCD/LCD.h"
#include "WoW/Source/Ui/VideoOptions.h"
#include "WoW/Source/UIUtil/AddOns.h"
#include "WoW/Source/UIUtil/InputControl.h"
#include "WoW/Source/WorldClient/World.h"
#include "WoW/Source/WowServices/Login.h"
#include "WoW/Source/WowServices/LoginResponse.h"
#include "WoW/Source/WowServices/RealmConnection.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/AccountData.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ChecksumExecutables.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/FileDownload.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/Types.h"

// STATIC: 0x00B89A78
const uint8_t InterfaceKey[256] = {
    0xC3, 0x5B, 0x50, 0x84, 0xB9, 0x3E, 0x32, 0x42, 0x8C, 0xD0, 0xC7, 0x48, 0xFA, 0x0E, 0x5D, 0x54,
    0x5A, 0xA3, 0x0E, 0x14, 0xBA, 0x9E, 0x0D, 0xB9, 0x5D, 0x8B, 0xEE, 0xB6, 0x84, 0x93, 0x45, 0x75,
    0xFF, 0x31, 0xFE, 0x2F, 0x64, 0x3F, 0x3D, 0x6D, 0x07, 0xD9, 0x44, 0x9B, 0x40, 0x85, 0x59, 0x34,
    0x4E, 0x10, 0xE1, 0xE7, 0x43, 0x69, 0xEF, 0x7C, 0x16, 0xFC, 0xB4, 0xED, 0x1B, 0x95, 0x28, 0xA8,
    0x23, 0x76, 0x51, 0x31, 0x57, 0x30, 0x2B, 0x79, 0x08, 0x50, 0x10, 0x1C, 0x4A, 0x1A, 0x2C, 0xC8,
    0x8B, 0x8F, 0x05, 0x2D, 0x22, 0x3D, 0xDB, 0x5A, 0x24, 0x7A, 0x0F, 0x13, 0x50, 0x37, 0x8F, 0x5A,
    0xCC, 0x9E, 0x04, 0x44, 0x0E, 0x87, 0x01, 0xD4, 0xA3, 0x15, 0x94, 0x16, 0x34, 0xC6, 0xC2, 0xC3,
    0xFB, 0x49, 0xFE, 0xE1, 0xF9, 0xDA, 0x8C, 0x50, 0x3C, 0xBE, 0x2C, 0xBB, 0x57, 0xED, 0x46, 0xB9,
    0xAD, 0x8B, 0xC6, 0xDF, 0x0E, 0xD6, 0x0F, 0xBE, 0x80, 0xB3, 0x8B, 0x1E, 0x77, 0xCF, 0xAD, 0x22,
    0xCF, 0xB7, 0x4B, 0xCF, 0xFB, 0xF0, 0x6B, 0x11, 0x45, 0x2D, 0x7A, 0x81, 0x18, 0xF2, 0x92, 0x7E,
    0x98, 0x56, 0x5D, 0x5E, 0x69, 0x72, 0x0A, 0x0D, 0x03, 0x0A, 0x85, 0xA2, 0x85, 0x9C, 0xCB, 0xFB,
    0x56, 0x6E, 0x8F, 0x44, 0xBB, 0x8F, 0x02, 0x22, 0x68, 0x63, 0x97, 0xBC, 0x85, 0xBA, 0xA8, 0xF7,
    0xB5, 0x40, 0x68, 0x3C, 0x77, 0x86, 0x6F, 0x4B, 0xD7, 0x88, 0xCA, 0x8A, 0xD7, 0xCE, 0x36, 0xF0,
    0x45, 0x6E, 0xD5, 0x64, 0x79, 0x0F, 0x17, 0xFC, 0x64, 0xDD, 0x10, 0x6F, 0xF3, 0xF5, 0xE0, 0xA6,
    0xC3, 0xFB, 0x1B, 0x8C, 0x29, 0xEF, 0x8E, 0xE5, 0x34, 0xCB, 0xD1, 0x2A, 0xCE, 0x79, 0xC3, 0x9A,
    0x0D, 0x36, 0xEA, 0x01, 0xE0, 0xAA, 0x91, 0x20, 0x54, 0xF0, 0x72, 0xD8, 0x1E, 0xC7, 0x89, 0xD2,
};

// STATIC: 0x00BCF230
static const LoginErrorURL s_loginErrorURLs[5] = {
    { AUTH_BANNED, "AUTH_BANNED_URL" },
    { AUTH_DB_BUSY, "AUTH_DB_BUSY_URL" },
    { AUTH_NO_TIME, "AUTH_NO_TIME_URL" },
    { AUTH_SUSPENDED, "AUTH_SUSPENDED_URL" },
    { AUTH_PARENTAL_CONTROL, "AUTH_PARENTAL_CONTROL_URL" },
};

// STATIC: 0x00D07538
int32_t CGlueMgr::m_loginResult = -1;

// STATIC: 0x00D0753C
int32_t CGlueMgr::m_loginState = -1;

// STATIC: 0x00D07540
int32_t CGlueMgr::m_lastLoginResult = -1;

// STATIC: 0x00D07544
int32_t CGlueMgr::m_lastLoginState = -1;

// STATIC: 0x00D07548
int32_t g_useAnimKitsOnGlueScreen = 1;

// STATIC: 0x00D0754C
const char* s_screenNames[NUM_GLUE_SCREENS] = { "login", "patchdownload", "charselect", "charcreate", "trialconvert", "movie", "realmlist", "credits", "realmwizard" };

// STATIC: 0x00D07578
static const char* s_serverAlertTag = "SERVERALERT:";

// STATIC: 0x00D0757C
static const char* s_serverAlertTagBOM = "\xEF\xBB\xBFSERVERALERT:";

// STATIC: 0x00D07580
static TSExplicitList<ConsoleString, offsetof(ConsoleString, m_link)> s_connectionStatus;

// STATIC: 0x00EBB128
uint8_t CGlueMgr::m_ffxActive;

// STATIC: 0x00EBB12C
FFX::Effect* CGlueMgr::m_deathEffect;

// STATIC: 0x00EBB130
FFX::Effect* CGlueMgr::m_glowEffect;

// STATIC: 0x00EBB134
static HSLOG s_connectionLog;

// STATIC: 0x00EBB138
char CGlueMgr::m_serverAlert[2048];

// STATIC: 0x00EBB944
uint32_t CGlueMgr::m_queueTime[8];

// STATIC: 0x00EBB964
int32_t CGlueMgr::m_queuePosition[8];

// STATIC: 0x00EBB984
int32_t CGlueMgr::m_currentScreen;

// STATIC: 0x00EBB988
MipBits* CGlueMgr::m_cursorMipBits;

// STATIC: 0x00EBB98C
CSimpleTop* CGlueMgr::m_simpleTop;

// STATIC: 0x00EBB990
int32_t CGlueMgr::m_reload;

// STATIC: 0x00EBB994
int32_t CGlueMgr::m_initialized;

// STATIC: 0x00EBB9A0
int32_t CGlueMgr::m_suspended;

// STATIC: 0x00EBB9A4
int32_t CGlueMgr::m_disconnectPending;

// STATIC: 0x00EBB9A8
int32_t CGlueMgr::m_reconnect;

// STATIC: 0x00EBB9AC
GLUE_IDLE_STATE CGlueMgr::m_idleState;

// STATIC: 0x00EBB9B0
GLUE_IDLE_STATE CGlueMgr::m_lastIdleState;

// STATIC: 0x00EBB9B4
uint32_t CGlueMgr::m_matrixChallengeCount;

// STATIC: 0x00EBB9B8
char CGlueMgr::m_accountName[1280];

// STATIC: 0x00EBBEB8
uint32_t CGlueMgr::m_matrixRemaining;

// STATIC: 0x00EBBEBC
int32_t CGlueMgr::m_estimatedWaitTime;

// STATIC: 0x00EBBEC0
int32_t CGlueMgr::m_displayingQueueDialog;

// STATIC: 0x00EBBEC4
CHARACTER_INFO* CGlueMgr::m_characterInfo;

// STATIC: 0x00EBBEC8
int32_t CGlueMgr::m_rereadTOS;

// STATIC: 0x00EBBECC
int32_t CGlueMgr::m_acceptedTOS;

// STATIC: 0x00EBBED0
int32_t CGlueMgr::m_rereadEULA;

// STATIC: 0x00EBBED4
int32_t CGlueMgr::m_acceptedEULA;

// STATIC: 0x00EBBED8
int32_t CGlueMgr::m_rereadTerminationWithoutNotice;

// STATIC: 0x00EBBEDC
int32_t CGlueMgr::m_acceptedTerminationWithoutNotice;

// STATIC: 0x00EBBEE0
int32_t CGlueMgr::m_rereadScanning;

// STATIC: 0x00EBBEE4
int32_t CGlueMgr::m_acceptedScanning;

// STATIC: 0x00EBBEE8
int32_t CGlueMgr::m_rereadContest;

// STATIC: 0x00EBBEEC
int32_t CGlueMgr::m_acceptedContest;

// STATIC: 0x00EBBEF0
uint8_t CGlueMgr::m_authenticated;

// STATIC: 0x00EBBEF1
bool CGlueMgr::s_scriptFunctionsLoaded;

// STATIC: 0x00EBBEF4
int32_t CGlueMgr::m_showingIntroScene;

// STATIC: 0x00EBBEF8
int32_t CGlueMgr::m_screenWidth;

// STATIC: 0x00EBBEFC
int32_t CGlueMgr::m_screenHeight;

// STATIC: 0x00EBBF00
float CGlueMgr::m_aspect;

// STATIC: 0x00EBBF04
int32_t CGlueMgr::m_pendingServerAlert;

// STATIC: 0x00EBBF08
int32_t CGlueMgr::m_processServerAlert;

// STATIC: 0x00EBBF0C
uint32_t CGlueMgr::m_pendingTimerAlert;

// STATIC: 0x00EBBF10
int32_t CGlueMgr::m_accountMsgAvailable;

// STATIC: 0x00EBBF14
uint8_t CGlueMgr::m_clientKickReason;

// STATIC: 0x00EBBF18
int32_t CGlueMgr::m_shownTrialPopUp;

// STATIC: 0x00EBBF1C
uint32_t CGlueMgr::m_numChangedOptionWarnings;

// STATIC: 0x00EBBF20
int32_t CGlueMgr::m_showedDisconnect;

// STATIC: 0x00EBBF24
uint32_t CGlueMgr::m_elapsedMs;

// STATIC: 0x00EBBF28
uint32_t CGlueMgr::m_timeMs;

// STATIC: 0x00EBBF2C
uint32_t CGlueMgr::m_expansionArchivesTotalChunks;

// STATIC: 0x00EBBF30
uint32_t CGlueMgr::m_expansionArchivesInitChunks;

// STATIC: 0x00EBBF34
static CVar* s_curGameTip;

// STATIC: 0x00EBBF38
static CVar* s_showGameTips;

// STATIC: 0x00EBBF40
CGlueMgr::Agreements CGlueMgr::m_latestAgreements;

// STATIC: 0x00EBBF54
TSGrowableArray<const char*> CGlueMgr::m_changedOptionWarnings;

// STATIC: 0x00EBBF64
static SCritSect s_connectionStatusLock;

// STATIC: 0x00EBBF8C
static CDataAllocator s_emoteKitFinishedAllocator(sizeof(CharacterEmoteKitFinishedData), 4);

// STATIC: 0x00EBBFA0
static CDataAllocator s_emoteSpellStartAllocator(sizeof(CharacterEmoteSpellStartData), 4);

// STATIC: 0x00DD2010
static __declspec(thread) WOWGUID s_activeCharacterGuid;

// PROVEN: d7d34f06 IDENTICAL/NONE
// OFFSET: 0x007FFF10/0x140513D50
bool CGlueMgr::AreScriptFunctionsLoaded() {
    return CGlueMgr::s_scriptFunctionsLoaded;
}

// PROVEN: d31889d0 IDENTICAL/NONE
// OFFSET: 0x007FFF30/0x1405191E0
static void WriteConnectionLog(const char* text) {
    if (!s_connectionLog) {
        OsCreateDirectory("Logs", 0);
        SLogCreate("Logs\\connection.log", 0, &s_connectionLog);
    }

    SLogWrite(s_connectionLog, text);
    SLogFlush(s_connectionLog);
}

// PROVEN: 7194ccec IDENTICAL/NONE
// OFFSET: 0x007FFF80/0x140513D70
void CGlueMgr::InitializeFFX() {
    if (CGlueMgr::m_ffxActive) {
        return;
    }

    CGlueMgr::m_ffxActive = 1;
    FFX::Init();
    CGlueMgr::m_glowEffect = new (SMemAlloc(sizeof(EffectGlow), __FILE__, __LINE__, 0)) EffectGlow();
    CGlueMgr::m_deathEffect = new (SMemAlloc(sizeof(EffectDeath), __FILE__, __LINE__, 0)) EffectDeath();
    CSimpleTop::SetColorblindShader();
}

// OFFSET: 0x00800000/0x140513E20
void CGlueMgr::SetIdleState(GLUE_IDLE_STATE state) {
    if (state) {
        int32_t leaving = (CGlueMgr::m_lastIdleState & IDLE_CHARACTER_STATES) && !(state & IDLE_CHARACTER_STATES);
        int32_t entering = !(CGlueMgr::m_lastIdleState & IDLE_CHARACTER_STATES) && (state & IDLE_CHARACTER_STATES);

        if (leaving) {
            CCharacterSelection::Shutdown();
            CCharacterCreation::Shutdown();
            CAnimKitManager::UninitSystem();
        } else if (entering) {
            CAnimKitManager::InitSystem();
            CCharacterSelection::Initialize();
            CCharacterCreation::Initialize();
        }

        CGlueMgr::m_lastIdleState = state;
    }

    CGlueMgr::m_idleState = state;
    CGlueMgr::m_showedDisconnect = 0;
}

// OFFSET: 0x00800080/0x140513EE0
void CGlueMgr::UpdateWaitQueue(int32_t position) {
    if (position != CGlueMgr::m_queuePosition[0] || !CGlueMgr::m_queueTime[7]) {
        for (uint32_t i = 7; i; i--) {
            CGlueMgr::m_queuePosition[i] = CGlueMgr::m_queuePosition[i - 1];
            CGlueMgr::m_queueTime[i] = CGlueMgr::m_queueTime[i - 1];
        }

        CGlueMgr::m_queuePosition[0] = position;
    }

    uint32_t now = FrameTime::GetCurTimeMs();
    CGlueMgr::m_queueTime[0] = now;

    if (CGlueMgr::m_queueTime[7] && CGlueMgr::m_queuePosition[0] - CGlueMgr::m_queuePosition[7] < 0) {
        CGlueMgr::m_estimatedWaitTime = -(position * (static_cast<int32_t>(now - CGlueMgr::m_queueTime[7]) / (CGlueMgr::m_queuePosition[0] - CGlueMgr::m_queuePosition[7])));
    }
}

// PROVEN: dea94251 IDENTICAL/NONE
// OFFSET: 0x00800120/0x140513F90
void CGlueMgr::AcceptTOS() {
    int32_t value = g_readTOSVar->m_intValue;
    CGlueMgr::m_acceptedTOS = 1;
    CGlueMgr::m_rereadTOS = 0;

    if (value == -2) {
        g_readTOSVar->Set("-1", true, false, false, true);
    } else {
        g_readTOSVar->Set("1", true, false, false, true);
    }
}

// PROVEN: e5426c22 IDENTICAL/NONE
// OFFSET: 0x00800160/0x140513FE0
void CGlueMgr::AcceptEULA() {
    int32_t value = g_readEULAVar->m_intValue;
    CGlueMgr::m_acceptedEULA = 1;
    CGlueMgr::m_rereadEULA = 0;

    if (value == -2) {
        g_readEULAVar->Set("-1", true, false, false, true);
    } else {
        g_readEULAVar->Set("1", true, false, false, true);
    }
}

// PROVEN: 844d2439 IDENTICAL/NONE
// OFFSET: 0x008001A0/0x140514030
void CGlueMgr::AcceptTerminationWithoutNotice() {
    int32_t value = g_readTerminationWithoutNoticeVar->m_intValue;
    CGlueMgr::m_acceptedTerminationWithoutNotice = 1;
    CGlueMgr::m_rereadTerminationWithoutNotice = 0;

    if (value == -2) {
        g_readTerminationWithoutNoticeVar->Set("-1", true, false, false, true);
    } else {
        g_readTerminationWithoutNoticeVar->Set("1", true, false, false, true);
    }
}

// PROVEN: a6ea8825 IDENTICAL/NONE
// OFFSET: 0x008001E0/0x140514080
void CGlueMgr::AcceptScanning() {
    CGlueMgr::m_acceptedScanning = 1;
    CGlueMgr::m_rereadScanning = 0;
    g_readScanningVar->Set("1", true, false, false, true);
}

// PROVEN: ef2b6f11 IDENTICAL/NONE
// OFFSET: 0x00800210/0x1405140C0
void CGlueMgr::AcceptContest() {
    CGlueMgr::m_acceptedContest = 1;
    CGlueMgr::m_rereadContest = 0;
    g_readContestVar->Set("1", true, false, false, true);
}

// OFFSET: 0x00800240/0x140514100
void CGlueMgr::LoginServerLogin(const char* username, char* password) {
    if (!CGlueMgr::m_scandllOkayToLogIn || !CGlueMgr::m_acceptedTOS || !CGlueMgr::m_acceptedEULA || CGlueMgr::m_idleState) {
        return;
    }

    if (!username || !*username) {
        FrameScript_SignalEvent(GLUE_EVENT_OPEN_STATUS_DIALOG, "%s%s", "OKAY", FrameScript_GetText("LOGIN_ENTER_NAME", -1, GENDER_NOT_APPLICABLE));
        return;
    }

    if (!password || !*password) {
        FrameScript_SignalEvent(GLUE_EVENT_OPEN_STATUS_DIALOG, "%s%s", "OKAY", FrameScript_GetText("LOGIN_ENTER_PASSWORD", -1, GENDER_NOT_APPLICABLE));
        return;
    }

    CGlueMgr::m_loginResult = -1;
    CGlueMgr::m_loginState = -1;
    CGlueMgr::m_lastLoginResult = -1;
    CGlueMgr::m_lastLoginState = -1;
    CGlueMgr::m_authenticated = 0;
    CGlueMgr::m_matrixRemaining = 0;
    CGlueMgr::SetIdleState(IDLE_LOGIN_SERVER_LOGIN);

    const char* src = username;
    char* dst = CGlueMgr::m_accountName;
    char c = *src;

    if (c) {
        do {
            if (dst >= &CGlueMgr::m_accountName[sizeof(CGlueMgr::m_accountName) - 1]) {
                break;
            }

            if (c == '|' && src[1] == '|') {
                src++;
            }

            *dst = *src++;
            c = *src;
            dst++;
        } while (*src);
    }

    *dst = '\0';
    _strupr(CGlueMgr::m_accountName);

    FrameScript_SignalEvent(GLUE_EVENT_OPEN_STATUS_DIALOG, "%s", "CANCEL");
    ClientServices::Logon(CGlueMgr::m_accountName, password);
    memset(password, 0, strlen(password));
}

// PROVEN: 80fafbc4 IDENTICAL/NONE
// OFFSET: 0x008003D0
void CGlueMgr::LoginEnterPIN(int32_t count, uint8_t* digits) {
    FrameScript_SignalEvent(GLUE_EVENT_OPEN_STATUS_DIALOG, "%s", "CANCEL");
    ClientServices::EnterPIN(count, digits);
}

// PROVEN: c31be062 IDENTICAL/NONE
// OFFSET: 0x008003F0/0x140514300
void CGlueMgr::LoginEnterMatrix(uint8_t value) {
    if (CGlueMgr::m_loginState == LOGIN_STATE_MATRIX_WAIT && CGlueMgr::m_matrixRemaining) {
        ClientServices::EnterMatrix(value);
    }
}

// OFFSET: 0x00800410/0x140514320
void CGlueMgr::LoginRevertMatrix() {
    if (CGlueMgr::m_loginState == LOGIN_STATE_MATRIX_WAIT && CGlueMgr::m_matrixRemaining) {
        ClientServices::RevertMatrix();
    }
}

// PROVEN: fad3d260 IDENTICAL/NONE
// OFFSET: 0x00800430/0x140514340
void CGlueMgr::LoginCommitMatrix() {
    if (CGlueMgr::m_loginState == LOGIN_STATE_MATRIX_WAIT && CGlueMgr::m_matrixRemaining) {
        ClientServices::CommitMatrix();

        if (!--CGlueMgr::m_matrixRemaining) {
            ClientServices::FinalizeMatrix();
            FrameScript_SignalEvent(GLUE_EVENT_OPEN_STATUS_DIALOG, "%s", "CANCEL");
        }
    }
}

// PROVEN: c8248a9e IDENTICAL/NONE
// OFFSET: 0x00800470/0x140514390
bool CGlueMgr::LoginGetMatrixCoordinates(uint32_t* x, uint32_t* y) {
    if (CGlueMgr::m_loginState != LOGIN_STATE_MATRIX_WAIT || !CGlueMgr::m_matrixRemaining) {
        return false;
    }

    return ClientServices::LoginConnection()->GetMatrixCoordinates(CGlueMgr::m_matrixChallengeCount - CGlueMgr::m_matrixRemaining, x, y);
}

// PROVEN: ce75f188 IDENTICAL/NONE
// OFFSET: 0x008004C0/0x1405143F0
void CGlueMgr::LoginEnterToken(const char* token) {
    FrameScript_SignalEvent(GLUE_EVENT_OPEN_STATUS_DIALOG, "%s", "CANCEL");
    ClientServices::EnterToken(token);
}

// OFFSET: 0x008004E0
WOWGUID CGlueMgr::GetActive() {
    return s_activeCharacterGuid;
}

// OFFSET: 0x00800500/0x140514440
void CGlueMgr::DisplayLoginStatus() {
    if (CGlueMgr::m_lastLoginState == CGlueMgr::m_loginState && CGlueMgr::m_lastLoginResult == CGlueMgr::m_loginResult) {
        return;
    }

    CGlueMgr::m_lastLoginState = CGlueMgr::m_loginState;
    CGlueMgr::m_lastLoginResult = CGlueMgr::m_loginResult;

    if (CGlueMgr::m_loginState == LOGIN_STATE_FAILED) {
        if (CGlueMgr::m_loginResult < 0 || CGlueMgr::m_loginResult >= LOGIN_RESULT_MAX) {
            return;
        }

        if (CGlueMgr::m_loginResult == LOGIN_PARENTALCONTROL) {
            FrameScript_SignalEvent(GLUE_EVENT_OPEN_STATUS_DIALOG, "%s%s%s", "PARENTAL_CONTROL", FrameScript_GetText(g_LoginResultStringNames[LOGIN_PARENTALCONTROL], -1, GENDER_NOT_APPLICABLE), "AUTH_PARENTAL_CONTROL_URL");
            return;
        }

        const char* text = FrameScript_GetText(g_LoginResultStringNames[CGlueMgr::m_loginResult], -1, GENDER_NOT_APPLICABLE);
        const char* button;

        if (SStrStrUTF8I(text, "<HTML>")) {
            button = "OKAY_HTML";

            if (CGlueMgr::m_loginResult == LOGIN_FAILED) {
                FrameScript_SignalEvent(GLUE_EVENT_OPEN_STATUS_DIALOG, "%s%s", "CONNECTION_HELP_HTML", text);
                return;
            }
        } else {
            button = "CONNECTION_HELP";

            if (CGlueMgr::m_loginResult != LOGIN_FAILED) {
                button = "OKAY";
            }
        }

        FrameScript_SignalEvent(GLUE_EVENT_OPEN_STATUS_DIALOG, "%s%s", button, text);
        return;
    }

    int32_t state = CGlueMgr::m_loginState;
    bool show = state >= 0 && state < LOGIN_STATE_MAX;

    if (state == LOGIN_STATE_DISCONNECTED || state == LOGIN_STATE_SURVEY) {
        return;
    }

    if (state == LOGIN_STATE_MATRIX || state == LOGIN_STATE_MATRIX_WAIT) {
        show = false;
    }

    if (state == LOGIN_STATE_TOKEN || state == LOGIN_STATE_TOKEN_WAIT) {
        show = false;
    }

    if (state != 18 && state != 19 && show) {
        FrameScript_SignalEvent(GLUE_EVENT_OPEN_STATUS_DIALOG, "%s%s", "CANCEL", FrameScript_GetText(g_LoginStateStringNames[state], -1, GENDER_NOT_APPLICABLE));
    }
}

// OFFSET: 0x00800650
static void StartAccountLogin() {
    if (CGlueMgr::m_idleState) {
        return;
    }

    CGlueMgr::SetIdleState(IDLE_ACCOUNT_LOGIN);
    ClientServices::Connection()->Connect();
}

// OFFSET: 0x008006A0
void CGlueMgr::CreateCharacter(const CHARACTER_CREATE_INFO* info) {
    if (!info) {
        return;
    }

    CGlueMgr::SetIdleState(IDLE_CREATE_CHARACTER);
    FrameScript_SignalEvent(GLUE_EVENT_OPEN_STATUS_DIALOG, "%s%s", "CANCEL", FrameScript_GetText(ClientServices::GetErrorToken(CHAR_CREATE_IN_PROGRESS), -1, GENDER_NOT_APPLICABLE));
    ClientServices::Connection()->RequestCharacterCreate(*info);
}

// OFFSET: 0x00800720
void CGlueMgr::DeleteCharacter(WOWGUID guid) {
    if (!guid) {
        return;
    }

    CGlueMgr::SetIdleState(IDLE_DELETE_CHARACTER);
    FrameScript_SignalEvent(GLUE_EVENT_OPEN_STATUS_DIALOG, "%s%s", "CANCEL", FrameScript_GetText(ClientServices::GetErrorToken(CHAR_DELETE_IN_PROGRESS), -1, GENDER_NOT_APPLICABLE));
    ClientServices::Connection()->RequestCharacterDelete(guid);
}

// PROVEN: 43952506 IDENTICAL/NONE
// OFFSET: 0x008007A0/0x140514710
void CGlueMgr::RequestRandomCharacterName(uint8_t raceID, uint8_t sexID) {
    UserClientGenerateRandomCharacterName msg(raceID, sexID);
    ClientServices::Send(&msg);
}

// OFFSET: 0x008007D0
int32_t CGlueMgr::OnRealmSplitMsg(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    uint32_t realmID = 0;
    uint32_t state = 0;
    char date[64];
    date[0] = '\0';

    if (!msg->IsRead()) {
        msg->Get(realmID);
        msg->Get(state);
        msg->GetString(date, sizeof(date));
    }

    FrameScript_SignalEvent(GLUE_EVENT_SERVER_SPLIT_NOTICE, "%d%d%s", realmID, state, date);
    return 1;
}

// OFFSET: 0x00800840
int32_t CGlueMgr::OnKickReasonMsg(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    uint8_t reason = 0;

    if (!msg->IsRead()) {
        msg->Get(reason);
    }

    CGlueMgr::m_clientKickReason = reason;
    return 1;
}

// PROVEN: d81996ba IDENTICAL/NONE
// OFFSET: 0x00800880/0x140514830
void CGlueMgr::CancelRealmListQuery() {
    if (CGlueMgr::m_idleState == IDLE_REALM_LIST) {
        ClientServices::Connection()->Cancel(2);
        CGlueMgr::m_idleState = IDLE_NONE;
        CGlueMgr::m_showedDisconnect = 0;
    }
}

// PROVEN: 1892a624 IDENTICAL/NONE
// OFFSET: 0x008008B0/0x140514870
void CGlueMgr::RealmListDialogCancelled() {
    if (CGlueMgr::m_currentScreen == GLUE_SCREEN_LOGIN) {
        ClientServices::LoginConnection()->Logoff();
    }
}

// PROVEN: 736ebf51 IDENTICAL/NONE
// OFFSET: 0x008008D0/0x1405148A0
void CGlueMgr::SetScreen(int32_t screen) {
    FrameScript_SignalEvent(GLUE_EVENT_SET_GLUE_SCREEN, "%s", s_screenNames[screen]);
}

// PROVEN: 8b1efe07 IDENTICAL/NONE
// OFFSET: 0x008008F0/0x1405148C0
void CGlueMgr::UpdateCurrentScreen(const char* screen) {
    switch (CGlueMgr::m_currentScreen) {
        case GLUE_SCREEN_CHARSELECT:
            CCharacterSelection::OnHide();
            break;

        case GLUE_SCREEN_CHARCREATE:
            CCharacterCreation::OnHide();
            break;
    }

    CGlueMgr::m_currentScreen = NUM_GLUE_SCREENS;

    for (uint32_t i = 0; i < NUM_GLUE_SCREENS; i++) {
        if (!_stricmp(screen, s_screenNames[i])) {
            CGlueMgr::m_currentScreen = i;
            break;
        }
    }

    switch (CGlueMgr::m_currentScreen) {
        case GLUE_SCREEN_CHARSELECT:
            CCharacterSelection::OnShow();
            break;

        case GLUE_SCREEN_CHARCREATE:
            CCharacterCreation::OnShow();
            break;
    }

    switch (CGlueMgr::m_currentScreen) {
        case GLUE_SCREEN_LOGIN:
            WoWReport::SetStage("lgin");
            break;

        case GLUE_SCREEN_CHARSELECT:
            WoWReport::SetStage("chsl");
            break;

        case GLUE_SCREEN_CHARCREATE:
            WoWReport::SetStage("chct");
            break;
    }
}

// PROVEN: 1dc21140 IDENTICAL/NONE
// OFFSET: 0x008009B0/0x140514990
void CGlueMgr::QuitGame() {
    ClientPostClose(0);
}

// PROVEN: 8d651c66 IDENTICAL/NONE
// OFFSET: 0x008009C0/0x1405149A0
void CGlueMgr::QuitGameAndRunLauncher() {
    ClientPostClose(0);
    SCreateProcess("Launcher.exe", NULL, NULL, NULL, NULL, NULL);
}

// OFFSET: 0x00800A60/0x140514A70
void CGlueMgr::GetRealmList(int32_t showDialog) {
    CGlueMgr::SetIdleState(IDLE_REALM_LIST);

    if (showDialog) {
        FrameScript_SignalEvent(GLUE_EVENT_OPEN_STATUS_DIALOG, "%s%s", "CANCEL", FrameScript_GetText("REALM_LIST_IN_PROGRESS", -1, GENDER_NOT_APPLICABLE));
    }

    ClientServices::GetRealmList();
}

// OFFSET: 0x008009E0
void CGlueMgr::GetCharacterList() {
    if (CGlueMgr::m_idleState == IDLE_WORLD_LOGIN || CGlueMgr::m_idleState == IDLE_ENTER_WORLD) {
        return;
    }

    CGlueMgr::SetIdleState(IDLE_CHARACTER_LIST);
    FrameScript_SignalEvent(GLUE_EVENT_OPEN_STATUS_DIALOG, "%s%s", "CANCEL", FrameScript_GetText("CHAR_LIST_RETRIEVING", -1, GENDER_NOT_APPLICABLE));
    ClientServices::Connection()->GetCharacterList();
}

// PROVEN: 0b373dfb IDENTICAL/NONE
// OFFSET: 0x004CE7B0
void CGlueMgr::ResetQueueState() {
    memset(CGlueMgr::m_queuePosition, 0, sizeof(CGlueMgr::m_queuePosition));
    memset(CGlueMgr::m_queueTime, 0, sizeof(CGlueMgr::m_queueTime));
    CGlueMgr::m_estimatedWaitTime = 0;
}

// PROVEN: 80a87352 IDENTICAL/NONE
// OFFSET: 0x00800AE0/0x140514B00
void CGlueMgr::DownloadFeed(const uint8_t* data, uint32_t size) {
    if (CGlueMgr::m_patchDownload) {
        CGlueMgr::PatchDownloadFeed(data, size);
    } else if (CGlueMgr::m_surveyDownload) {
        CGlueMgr::SurveyDownloadFeed(data, size);
    }
}

// OFFSET: 0x00800B10
int32_t StoreHandItemType(int32_t slot, int32_t inventoryType, bool hunter, CGlueMgr::WeaponLoadOut* types) {
    int32_t* type;

    switch (slot) {
        case 15:
            if (hunter) {
                return 0;
            }

            type = &types->m_mainHand;
            break;

        case 16:
            if (hunter) {
                return 0;
            }

            type = &types->m_offHand;
            break;

        case 17:
            if (!hunter) {
                return 0;
            }

            type = inventoryType == 15 ? &types->m_offHand : &types->m_mainHand;
            break;

        default:
            return 0;
    }

    if (type) {
        *type = inventoryType;
    }

    return 1;
}

// PROVEN: 2ffc122e IDENTICAL/NONE
// OFFSET: 0x00800B70
void CGlueMgr::AttachmentSequenceCallback(CM2Model* model) {
    model->DetachFromParent();
}

// PROVEN: b5ce397f IDENTICAL/NONE
// OFFSET: 0x00800B80
uint32_t CGlueMgr::GetTotemDisplayId(int32_t classID, int32_t raceID) {
    if (classID != 7) {
        return 0;
    }

    switch (raceID) {
        case 2:
            return 30758;
        case 3:
            return 30754;
        case 6:
            return 4589;
        case 8:
            return 30762;
        case 9:
            return 30783;
        case 11:
            return 19074;
        default:
            return 0;
    }
}

// PROVEN: e86a3e0f IDENTICAL/NONE
// OFFSET: 0x00800C00/0x140514BC0
int32_t CGlueMgr::HandleBattlenetDisconnect() {
    if (CGlueMgr::m_loginState != LOGIN_STATE_FAILED || CGlueMgr::m_loginResult != 21 || CGlueMgr::m_showedDisconnect) {
        return 0;
    }

    if (!ClientServices::LoginConnection() || ClientServices::LoginConnection()->GetLoginServerType() != 1) {
        return 0;
    }

    CGlueMgr::m_disconnectPending = 1;
    ClientServices::Connection()->Disconnect(static_cast<DISCONNECT_REASON>(8));
    ClientServices::LoginConnection()->Logoff();
    FrameScript_SignalEvent(GLUE_EVENT_DISCONNECTED_FROM_SERVER, "%d", 0);
    CGlueMgr::m_showedDisconnect = 1;
    return 1;
}

// OFFSET: 0x00800C90
void CGlueMgr::PollUserSurvey() {
    if (CGlueMgr::m_surveyDownload && CGlueMgr::m_surveyDownload->GetStatus() == 2) {
        if (CGlueMgr::m_executedSurvey) {
            bool elapsed = FrameTime::GetCurTimeMs() - CGlueMgr::m_surveyTimer > 1500;

            if (!elapsed) {
                return;
            }
        } else {
            CGlueMgr::m_executedSurvey = 1;

            if (CGlueMgr::SurveyExecute()) {
                FrameScript_SignalEvent(GLUE_EVENT_OPEN_STATUS_DIALOG, "%s%s", "CANCEL", FrameScript_GetText("LOGIN_STATE_SURVEY", -1, GENDER_NOT_APPLICABLE));
                CGlueMgr::SurveySendResults();
                CGlueMgr::m_surveyTimer = FrameTime::GetCurTimeMs();
                return;
            }
        }
    }

    CGlueMgr::SurveyDownloadEnd();
}

// PROVEN: 54d2aefa IDENTICAL/NONE
// OFFSET: 0x00800D10/0x140514D00
void CGlueMgr::PollCharacterList(const char* text, int32_t complete, int32_t result, int32_t errorCode) {
    FrameScript_SignalEvent(GLUE_EVENT_UPDATE_STATUS_DIALOG, "%s", text);

    if (CGlueMgr::HandleBattlenetDisconnect()) {
        CGlueMgr::m_idleState = IDLE_NONE;
        CGlueMgr::m_showedDisconnect = 0;
    }

    if (!complete) {
        return;
    }

    if (!result) {
        if (errorCode == RESPONSE_CANCELLED) {
            CCharacterSelection::ClearCharacterList();
            CGlueMgr::GetRealmList(1);
            return;
        }

        FrameScript_SignalEvent(GLUE_EVENT_OPEN_STATUS_DIALOG, "%s%s", "OKAY", text);
        CGlueMgr::m_idleState = IDLE_NONE;
        CGlueMgr::m_showedDisconnect = 0;
        return;
    }

    CGlueMgr::m_idleState = IDLE_NONE;
    CGlueMgr::m_showedDisconnect = 0;
    FrameScript_SignalEvent(GLUE_EVENT_CLOSE_STATUS_DIALOG, NULL);

    if (CGlueMgr::m_accountMsgAvailable) {
        FrameScript_SignalEvent(GLUE_EVENT_ACCOUNT_MESSAGES_AVAILABLE, NULL);
        CGlueMgr::m_accountMsgAvailable = 0;
    }
}

// PROVEN: 75a9d693 IDENTICAL/NONE
// OFFSET: 0x00800DC0
void CGlueMgr::PollRealmList(const char* text, int32_t complete, int32_t result) {
    FrameScript_SignalEvent(GLUE_EVENT_UPDATE_STATUS_DIALOG, "%s", text);

    if (CGlueMgr::HandleBattlenetDisconnect()) {
        CGlueMgr::m_idleState = IDLE_NONE;
        CGlueMgr::m_showedDisconnect = 0;
    }

    if (!complete) {
        return;
    }

    if (!result) {
        FrameScript_SignalEvent(GLUE_EVENT_OPEN_STATUS_DIALOG, "%s%s", "OKAY", text);
        CGlueMgr::m_idleState = IDLE_NONE;
        CGlueMgr::m_showedDisconnect = 0;
        return;
    }

    CGlueMgr::m_idleState = IDLE_NONE;
    CGlueMgr::m_showedDisconnect = 0;
    FrameScript_SignalEvent(GLUE_EVENT_CLOSE_STATUS_DIALOG, NULL);
    CRealmList::UpdateList();

    if (CGlueMgr::m_accountMsgAvailable) {
        FrameScript_SignalEvent(GLUE_EVENT_ACCOUNT_MESSAGES_AVAILABLE, NULL);
        CGlueMgr::m_accountMsgAvailable = 0;
    }
}

// PROVEN: 895f119a IDENTICAL/NONE
// OFFSET: 0x00800E60
void CGlueMgr::PollCreateCharacter(const char* text, int32_t complete, int32_t result) {
    FrameScript_SignalEvent(GLUE_EVENT_UPDATE_STATUS_DIALOG, "%s", text);

    if (CGlueMgr::HandleBattlenetDisconnect()) {
        CGlueMgr::m_idleState = IDLE_NONE;
        CGlueMgr::m_showedDisconnect = 0;
    }

    if (!complete) {
        return;
    }

    if (!result) {
        FrameScript_SignalEvent(GLUE_EVENT_OPEN_STATUS_DIALOG, "%s%s", "OKAY", text);
        CGlueMgr::m_idleState = IDLE_NONE;
        CGlueMgr::m_showedDisconnect = 0;
        return;
    }

    CGlueMgr::m_idleState = IDLE_NONE;
    CGlueMgr::m_showedDisconnect = 0;
    FrameScript_SignalEvent(GLUE_EVENT_CLOSE_STATUS_DIALOG, NULL);
    FrameScript_SignalEvent(GLUE_EVENT_SELECT_LAST_CHARACTER, NULL);
    CGlueMgr::SetScreen(GLUE_SCREEN_CHARSELECT);
}

// PROVEN: 86442b2a IDENTICAL/NONE
// OFFSET: 0x00800F00
void CGlueMgr::PollDeleteCharacter(const char* text, int32_t complete, int32_t result) {
    FrameScript_SignalEvent(GLUE_EVENT_UPDATE_STATUS_DIALOG, "%s", text);

    if (CGlueMgr::HandleBattlenetDisconnect()) {
        CGlueMgr::m_idleState = IDLE_NONE;
        CGlueMgr::m_showedDisconnect = 0;
    }

    if (!complete) {
        return;
    }

    if (!result) {
        FrameScript_SignalEvent(GLUE_EVENT_OPEN_STATUS_DIALOG, "%s%s", "OKAY", text);
        CGlueMgr::m_idleState = IDLE_NONE;
        CGlueMgr::m_showedDisconnect = 0;
        return;
    }

    FrameScript_SignalEvent(GLUE_EVENT_SELECT_FIRST_CHARACTER, NULL);
    CGlueMgr::GetCharacterList();
}

// OFFSET: 0x00800F80/0x140515010
int32_t CGlueMgr::Paint(const void* data, void* param) {
    uint32_t now = OsGetAsyncTimeMsPrecise();
    uint32_t elapsed = now - CGlueMgr::m_timeMs;
    CGlueMgr::m_timeMs = now;
    CGlueMgr::m_elapsedMs = elapsed;

    CAnimKitManager::UpdateAll(elapsed);
    g_scene.Update(CGlueMgr::m_timeMs);
    g_scene.Render();
    CAnimKitManager::GarbageCollect();

    C3Vector viewPos(0.0f, 0.0f, 0.0f);
    GxuSortDrawList(GxuCat_0, GxuSort_VtxStream, viewPos);
    GxuFlushDrawList(GxuCat_0, viewPos);

    return 1;
}

// PROVEN: cf9b85bf IDENTICAL/NONE
// OFFSET: 0x00801000/0x1405150A0
void CGlueMgr::HandleScreenshot(int32_t success) {
    if (success) {
        FrameScript_SignalEvent(GLUE_EVENT_GLUE_SCREENSHOT_SUCCEEDED, NULL);
    } else {
        FrameScript_SignalEvent(GLUE_EVENT_GLUE_SCREENSHOT_FAILED, NULL);
    }
}

// PROVEN: 14619fab IDENTICAL/NONE
// OFFSET: 0x00801030/0x1405150C0
int32_t CGlueMgr::ShowChangedOptionWarnings() {
    return CGlueMgr::m_numChangedOptionWarnings > 0;
}

// OFFSET: 0x00801040/0x1405150D0
int32_t CGlueMgr::CCommand_ExportInterfaceFiles(const char* command, const char* arguments) {
    if (!*arguments) {
        return 1;
    }

    if (!_stricmp("art", arguments)) {
        ExportInterfaceFiles(0);
        return 1;
    }

    if (!_stricmp("code", arguments)) {
        ExportInterfaceFiles(1);
    }

    return 1;
}

// OFFSET: 0x008010D0/0x140515160
void CGlueMgr::PreloadCharacterModels() {
    uint32_t count = g_chrClassesDB.m_numRecords;

    for (uint32_t i = 0; i < count; i++) {
        char path[260];
        CGPlayer_C::GetSpellTextureBlobName(g_chrClassesDB.GetRecordByIndex(i), path);
        AsyncFileReadCreateObject(path, 5, 0, 0, NULL, NULL, NULL, AsyncFileReadDestroyObject, AsyncFileReadDestroyObject, 0, 0, 0);
    }
}

// OFFSET: 0x00801150/0x140515220
bool CGlueMgr::ServerAlertURLCallback(void* param, const char* data, uint32_t size, bool done, HTTPError::ErrorCode error) {
    if (size) {
        char buffer[2048];
        SStrCopy(buffer, data, size >= sizeof(buffer) ? sizeof(buffer) : size);
        SStrPack(CGlueMgr::m_serverAlert, buffer, sizeof(CGlueMgr::m_serverAlert));
    }

    if (done) {
        if (CGlueMgr::m_serverAlert[0] && (!_strnicmp(CGlueMgr::m_serverAlert, s_serverAlertTag, strlen(s_serverAlertTag)) || !_strnicmp(CGlueMgr::m_serverAlert, s_serverAlertTagBOM, strlen(s_serverAlertTagBOM)))) {
            CGlueMgr::m_processServerAlert = 1;
        }

        CGlueMgr::m_pendingServerAlert = 0;
    }

    return true;
}

// OFFSET: 0x00801260/0x140515360
void ProcessAgreement(SFileImpl* file, const char* name, CVar* cvar) {
    char path[260];
    SStrCopy(path, "data", sizeof(path));
    SStrPack(path, "\\", sizeof(path));
    SStrPack(path, s_localeCode, sizeof(path));
    SStrPack(path, "\\", sizeof(path));
    SStrPack(path, name, sizeof(path));

    void* existing = NULL;
    uint32_t existingSize = 0;

    if (OsFileExists(path)) {
        HOSFILE input = OsCreateFile(path, OS_GENERIC_READ, 3, OS_OPEN_EXISTING, OS_FILE_ATTRIBUTE_NORMAL, OS_FILE_TYPE_ANY);

        if (input != OS_INVALID_FILE) {
            uint64_t size = OsGetFileSize(input);
            existing = SMemAlloc(static_cast<uint32_t>(size), __FILE__, __LINE__, 0);

            if (existing) {
                OsReadFile(input, existing, static_cast<uint32_t>(size), &existingSize);
            }

            OsCloseFile(input);
        }
    }

    uint32_t size = SFileGetFileSize(file, NULL);
    void* data = SMemAlloc(size, __FILE__, __LINE__, 0);

    if (!data) {
        if (existing) {
            SMemFree(existing, __FILE__, __LINE__, 0);
        }

        return;
    }

    uint32_t read = 0;

    if (!SFile::Read(file, data, size, &read)) {
        SMemFree(data, __FILE__, __LINE__, 0);

        if (existing) {
            SMemFree(existing, __FILE__, __LINE__, 0);
        }

        return;
    }

    if (existingSize == read && !memcmp(data, existing, existingSize)) {
        SMemFree(data, __FILE__, __LINE__, 0);

        if (existing) {
            SMemFree(existing, __FILE__, __LINE__, 0);
        }

        return;
    }

    if (existing) {
        SMemFree(existing, __FILE__, __LINE__, 0);
    }

    if (OsFileExists(path) && !OsDeleteFile(path)) {
        SMemFree(data, __FILE__, __LINE__, 0);
        return;
    }

    HOSFILE output = OsCreateFile(path, OS_GENERIC_WRITE, 0, OS_CREATE_ALWAYS, OS_FILE_ATTRIBUTE_NORMAL, OS_FILE_TYPE_ANY);

    if (output == OS_INVALID_FILE) {
        SMemFree(data, __FILE__, __LINE__, 0);
        return;
    }

    uint32_t written = 0;

    if (!OsWriteFile(output, data, read, &written) || read != written) {
        SMemFree(data, __FILE__, __LINE__, 0);
        OsCloseFile(output);
        return;
    }

    SMemFree(data, __FILE__, __LINE__, 0);
    OsCloseFile(output);

    if (cvar->m_intValue >= 1) {
        cvar->Set("-1", true, false, false, true);
    } else {
        cvar->Set("-2", true, false, false, true);
    }
}

// OFFSET: 0x00801650/0x140515830
void CGlueMgr::ProcessAgreements(CGlueMgr::Agreements* agreements) {
    if (!agreements) {
        return;
    }

    char path[260];
    SStrCopy(path, "data", sizeof(path));
    SStrPack(path, "\\", sizeof(path));
    SStrPack(path, s_localeCode, sizeof(path));
    SStrPack(path, "\\", sizeof(path));
    SStrPack(path, "agreements.mpq", sizeof(path));

    HOSFILE file = OsCreateFile(path, OS_GENERIC_WRITE, 0, OS_CREATE_ALWAYS, OS_FILE_ATTRIBUTE_NORMAL, OS_FILE_TYPE_ANY);

    if (file == OS_INVALID_FILE) {
        return;
    }

    uint32_t written = 0;

    if (!OsWriteFile(file, agreements->m_data.m_data, agreements->m_data.m_count, &written) || agreements->m_data.m_count != written) {
        OsCloseFile(file);
        return;
    }

    OsCloseFile(file);

    SArchive* archive;

    if (!SFile::OpenArchive(path, 0, 0x800, &archive)) {
        OsDeleteFile(path);
        return;
    }

    SFileImpl* agreement;

    if (SFile::OpenEx(archive, "tos.html", 0, &agreement)) {
        ProcessAgreement(agreement, "tos.html", g_readTOSVar);
        SFileCloseFile(agreement);
    }

    if (SFile::OpenEx(archive, "eula.html", 0, &agreement)) {
        ProcessAgreement(agreement, "eula.html", g_readEULAVar);
        SFileCloseFile(agreement);
    }

    if (SFile::OpenEx(archive, "termination.html", 0, &agreement)) {
        ProcessAgreement(agreement, "termination.html", g_readTerminationWithoutNoticeVar);
        SFileCloseFile(agreement);
    }

    SFile::CloseArchive(archive);
    OsDeleteFile(path);
}

// PROVEN: 8e97ce4c IDENTICAL/NONE
// OFFSET: 0x00801900/0x140515BA0
int32_t CGlueMgr::HandleDisplaySizeChanged(const CSizeEvent& event) {
    if (CGlueMgr::m_screenWidth > 0 && CGlueMgr::m_screenWidth == event.m_data.m_w && CGlueMgr::m_screenHeight > 0 && CGlueMgr::m_screenHeight == event.m_data.m_h) {
        return 1;
    }

    CGlueMgr::m_screenWidth = event.m_data.m_w;
    CGlueMgr::m_screenHeight = event.m_data.m_h;

    float aspect = g_theGxDevicePtr->CapsAspectRatio(0);
    CSimpleFrame* glueParent = static_cast<CSimpleFrame*>(CScriptObject::GetScriptObjectByName("GlueParent", CSimpleFrame::GetObjectType()));

    if (glueParent) {
        glueParent->SetFrameScale(1.0f, true);
        CLayoutFrame::ResizePending();
    }

    if (!(0.0099999998f > fabs(CGlueMgr::m_aspect - aspect))) {
        CGlueMgr::m_reload = 1;
        CGlueMgr::m_aspect = aspect;
    }

    return 1;
}

// OFFSET: 0x008019D0/0x140515C80
void CGlueMgr::Suspend() {
    CGlueMgr::m_suspended = 1;
    EventUnregister(EVENT_ID_PAINT, CGlueMgr::Paint);

    if (CGlueMgr::m_ffxActive) {
        CGlueMgr::m_ffxActive = 0;

        if (CGlueMgr::m_glowEffect) {
            delete CGlueMgr::m_glowEffect;
        }

        if (CGlueMgr::m_deathEffect) {
            delete CGlueMgr::m_deathEffect;
        }

        FFX::Destroy();
    }

    if (CGlueMgr::m_simpleTop) {
        delete CGlueMgr::m_simpleTop;
        CGlueMgr::m_simpleTop = NULL;
    }

    CCharacterCreation::SetCharCustomizeFrame(NULL);
    CCharacterSelection::SetModelFrame(NULL);

    if (CGlueMgr::m_cursorMipBits) {
        TextureFreeMippedImg(CGlueMgr::m_cursorMipBits, PIXEL_ARGB8888, 32, 32);
        CGlueMgr::m_cursorMipBits = NULL;
    }

    SystemUnregisterFunctions();
    UnregisterSimpleFrameScriptMethods();
    UnregisterSharedScriptFunctions();
    GlueScriptEventsUnregisterFunctions();
    CharSelectUnregisterScriptFunctions();
    CharacterCreateUnregisterScriptFunctions();
    RealmListUnregisterScriptFunctions();
    SI3::UnregisterScriptFunctions();
    AccountMsg_UnregisterScriptFunctions();
    CGVideoOptions::UnregisterScriptFunctions();
    CGlueMgr::s_scriptFunctionsLoaded = 0;
    ConsoleCommandUnregister("ExportInterfaceFiles");
    FrameXML_FreeHashNodes();
    ClientServices::ClearMessageHandler(SMSG_CHARACTER_RENAME_RESULT);
    ClientServices::ClearMessageHandler(SMSG_SET_PLAYER_DECLINED_NAMES_RESULT);
    ClientServices::ClearMessageHandler(SMSG_CHAR_CUSTOMIZE);
    ClientServices::ClearMessageHandler(SMSG_REALM_SPLIT);
    ClientServices::ClearMessageHandler(SMSG_KICK_REASON);
    ClientServices::ClearMessageHandler(SMSG_CHAR_FACTION_CHANGE_RESULT);
    ClientGenerateRandomCharacterNameResult::s_handler = NULL;
}

// PROVEN: ddf8f35f IDENTICAL/NONE
// OFFSET: 0x00801B20
void CGlueMgr::SetCurrentAccount(const char* accountName) {
    SStrCopy(CGlueMgr::m_accountName, accountName, sizeof(CGlueMgr::m_accountName));
    _strupr(CGlueMgr::m_accountName);
}

// OFFSET: 0x00801B70/0x140515E60
void CGlueMgr::ChangeRealm(REALM_INFO* realm) {
    if (!realm) {
        return;
    }

    ClientServices::SelectRealm(realm->m_realmName);

    if (ClientServices::Connection()->unk_2F3C) {
        CGlueMgr::m_disconnectPending = 1;
        CGlueMgr::m_reconnect = 1;
        ClientServices::Connection()->Disconnect(DISCONNECT_REASON_RECONNECT);
        return;
    }

    CGlueMgr::SetIdleState(IDLE_ACCOUNT_LOGIN);
    FrameScript_SignalEvent(GLUE_EVENT_OPEN_STATUS_DIALOG, "%s%s", "CANCEL", FrameScript_GetText("GAME_SERVER_LOGIN", -1, GENDER_NOT_APPLICABLE));
    ClientServices::Connection()->Connect();
}

// OFFSET: 0x00801C20/0x140515F30
int32_t CGlueMgr::RenameCharacter(WOWGUID guid, const char* name) {
    if (!guid || !name || !*name) {
        return 1;
    }

    int32_t result = ClientServices::CharacterValidateName(name);

    if (result != CHAR_NAME_SUCCESS) {
        FrameScript_SignalEvent(GLUE_EVENT_FORCE_RENAME_CHARACTER, "%s", ClientServices::GetErrorToken(result));
        return 0;
    }

    CGlueMgr::SetIdleState(IDLE_RENAME_CHARACTER);
    FrameScript_SignalEvent(GLUE_EVENT_OPEN_STATUS_DIALOG, "%s%s", "CANCEL", FrameScript_GetText("CHAR_RENAME_IN_PROGRESS", -1, GENDER_NOT_APPLICABLE));

    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_CHARACTER_RENAME_REQUEST));
    msg.Put(guid.m_guid);
    msg.PutString(name);
    msg.Finalize();
    ClientServices::Send2(&msg);

    return 1;
}

// OFFSET: 0x00801D30/0x1405160A0
void CGlueMgr::CustomizeCharacter(WOWGUID guid, const CHARACTER_CREATE_INFO* info) {
    if (!guid || !info || !info->m_name[0]) {
        return;
    }

    CGlueMgr::SetIdleState(IDLE_CUSTOMIZE_CHARACTER);
    FrameScript_SignalEvent(GLUE_EVENT_OPEN_STATUS_DIALOG, "%s%s", "CANCEL", FrameScript_GetText("CHAR_CUSTOMIZE_IN_PROGRESS", -1, GENDER_NOT_APPLICABLE));

    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_CHAR_CUSTOMIZE));
    msg.Put(guid.m_guid);
    msg.PutString(info->m_name);
    msg.Put(info->m_sexID);
    msg.Put(info->m_skinID);
    msg.Put(info->m_hairColorID);
    msg.Put(info->m_hairStyleID);
    msg.Put(info->m_facialHairStyleID);
    msg.Put(info->m_faceID);
    msg.Finalize();
    ClientServices::Send2(&msg);
}

// OFFSET: 0x00801E60/0x140516230
void CGlueMgr::FactionChange(WOWGUID guid, const CHARACTER_CREATE_INFO* info) {
    if (!guid || !info || !info->m_name[0]) {
        return;
    }

    CGlueMgr::SetIdleState(IDLE_CUSTOMIZE_CHARACTER);
    FrameScript_SignalEvent(GLUE_EVENT_OPEN_STATUS_DIALOG, "%s%s", "CANCEL", FrameScript_GetText("FACTION_CHANGE_IN_PROGRESS", -1, GENDER_NOT_APPLICABLE));

    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_CHAR_FACTION_CHANGE));
    msg.Put(guid.m_guid);
    msg.PutString(info->m_name);
    msg.Put(info->m_sexID);
    msg.Put(info->m_skinID);
    msg.Put(info->m_hairColorID);
    msg.Put(info->m_hairStyleID);
    msg.Put(info->m_facialHairStyleID);
    msg.Put(info->m_faceID);
    msg.Put(info->m_raceID);
    msg.Finalize();
    ClientServices::Send2(&msg);
}

// OFFSET: 0x00801FA0/0x1405163D0
void CGlueMgr::RaceChange(WOWGUID guid, const CHARACTER_CREATE_INFO* info) {
    if (!guid || !info || !info->m_name[0]) {
        return;
    }

    CGlueMgr::SetIdleState(IDLE_CUSTOMIZE_CHARACTER);
    FrameScript_SignalEvent(GLUE_EVENT_OPEN_STATUS_DIALOG, "%s%s", "CANCEL", FrameScript_GetText("RACE_CHANGE_IN_PROGRESS", -1, GENDER_NOT_APPLICABLE));

    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_CHAR_RACE_CHANGE));
    msg.Put(guid.m_guid);
    msg.PutString(info->m_name);
    msg.Put(info->m_sexID);
    msg.Put(info->m_skinID);
    msg.Put(info->m_hairColorID);
    msg.Put(info->m_hairStyleID);
    msg.Put(info->m_facialHairStyleID);
    msg.Put(info->m_faceID);
    msg.Put(info->m_raceID);
    msg.Finalize();
    ClientServices::Send2(&msg);
}

// OFFSET: 0x00802110
bool CGlueMgr::OnRandomCharacterNameResult(ClientGenerateRandomCharacterNameResult* msg) {
    FrameScript_SignalEvent(GLUE_EVENT_RANDOM_CHARACTER_NAME_RESULT, "%d%s", msg->m_success, msg->m_success ? msg->m_name : "");
    return true;
}

// PROVEN: 548886ee IDENTICAL/NONE
// OFFSET: 0x008020E0
int32_t CGlueMgr::GetSelectedCharInfo(CHARACTER_INFO* info) {
    CGlueMgr::m_characterInfo = info;

    if (!info) {
        return 0;
    }

    CCharacterSelection::m_enterWorldIndex = CCharacterSelection::m_selectionIndex;
    return 1;
}

// OFFSET: 0x00802150
int32_t CGlueMgr::OnCharCustomizeResult(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    WOWGUID guid = { 0 };
    char name[48];
    uint8_t sexID = 0;
    uint8_t skinID = 0;
    uint8_t hairColorID = 0;
    uint8_t hairStyleID = 0;
    uint8_t facialHairStyleID = 0;
    uint8_t faceID = 0;
    uint8_t result;

    CGlueMgr::m_idleState = IDLE_NONE;
    CGlueMgr::m_showedDisconnect = 0;
    msg->Get(result);
    msg->Get(guid.m_guid);

    if (!msg->IsRead()) {
        msg->GetString(name, sizeof(name));
        msg->Get(sexID);
        msg->Get(skinID);
        msg->Get(hairColorID);
        msg->Get(hairStyleID);
        msg->Get(facialHairStyleID);
        msg->Get(faceID);
    }

    if (result) {
        if (result == 50) {
            FrameScript_SignalEvent(GLUE_EVENT_OPEN_STATUS_DIALOG, "%s%s", "OKAY", FrameScript_GetText(ClientServices::GetErrorToken(50), -1, GENDER_NOT_APPLICABLE));
        } else {
            FrameScript_SignalEvent(GLUE_EVENT_OPEN_STATUS_DIALOG, "%s%s", "OKAY", FrameScript_GetText("CHAR_CUSTOMIZE_FAILED", -1, GENDER_NOT_APPLICABLE));
        }

        return 1;
    }

    CCharacterSelection::UpdateCharacterInfo(guid, name, sexID, skinID, hairColorID, hairStyleID, facialHairStyleID, faceID, 0);
    FrameScript_SignalEvent(GLUE_EVENT_CLOSE_STATUS_DIALOG, NULL);
    FrameScript_SignalEvent(GLUE_EVENT_SET_GLUE_SCREEN, "%s", s_screenNames[GLUE_SCREEN_CHARSELECT]);
    return 1;
}

// OFFSET: 0x00802290
int32_t CGlueMgr::OnCharFactionChangeResult(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    WOWGUID guid = { 0 };
    char name[48];
    uint8_t sexID = 0;
    uint8_t skinID = 0;
    uint8_t hairColorID = 0;
    uint8_t hairStyleID = 0;
    uint8_t facialHairStyleID = 0;
    uint8_t faceID = 0;
    uint8_t raceID = 0;
    uint8_t result;

    CGlueMgr::m_idleState = IDLE_NONE;
    CGlueMgr::m_showedDisconnect = 0;
    msg->Get(result);
    msg->Get(guid.m_guid);

    if (!msg->IsRead()) {
        msg->GetString(name, sizeof(name));
        msg->Get(sexID);
        msg->Get(skinID);
        msg->Get(hairColorID);
        msg->Get(hairStyleID);
        msg->Get(facialHairStyleID);
        msg->Get(faceID);
        msg->Get(raceID);
    }

    switch (result) {
        case 0:
            CCharacterSelection::UpdateCharacterInfo(guid, name, sexID, skinID, hairColorID, hairStyleID, facialHairStyleID, faceID, raceID);
            FrameScript_SignalEvent(GLUE_EVENT_CLOSE_STATUS_DIALOG, NULL);
            CGlueMgr::SetScreen(GLUE_SCREEN_CHARSELECT);
            return 1;

        case 50:
            FrameScript_SignalEvent(GLUE_EVENT_OPEN_STATUS_DIALOG, "%s%s", "OKAY", FrameScript_GetText(ClientServices::GetErrorToken(50), -1, GENDER_NOT_APPLICABLE));
            return 1;

        case 57:
            FrameScript_SignalEvent(GLUE_EVENT_OPEN_STATUS_DIALOG, "%s%s", "OKAY", FrameScript_GetText(ClientServices::GetErrorToken(57), -1, GENDER_NOT_APPLICABLE));
            return 1;

        case 61:
            FrameScript_SignalEvent(GLUE_EVENT_OPEN_STATUS_DIALOG, "%s%s", "OKAY", FrameScript_GetText("CHAR_FACTION_CHANGE_STILL_IN_GUILD", -1, GENDER_NOT_APPLICABLE));
            return 1;

        case 62:
            FrameScript_SignalEvent(GLUE_EVENT_OPEN_STATUS_DIALOG, "%s%s", "OKAY", FrameScript_GetText("CHAR_FACTION_CHANGE_RACECLASS_RESTRICTED", -1, GENDER_NOT_APPLICABLE));
            return 1;

        case 63:
            FrameScript_SignalEvent(GLUE_EVENT_OPEN_STATUS_DIALOG, "%s%s", "OKAY", FrameScript_GetText("CHAR_FACTION_CHANGE_CHOOSE_RACE", -1, GENDER_NOT_APPLICABLE));
            return 1;

        case 64:
            FrameScript_SignalEvent(GLUE_EVENT_OPEN_STATUS_DIALOG, "%s%s", "OKAY", FrameScript_GetText("CHAR_FACTION_CHANGE_ARENA_LEADER", -1, GENDER_NOT_APPLICABLE));
            return 1;

        case 65:
            FrameScript_SignalEvent(GLUE_EVENT_OPEN_STATUS_DIALOG, "%s%s", "OKAY", FrameScript_GetText("CHAR_FACTION_CHANGE_DELETE_MAIL", -1, GENDER_NOT_APPLICABLE));
            return 1;

        case 66:
            FrameScript_SignalEvent(GLUE_EVENT_OPEN_STATUS_DIALOG, "%s%s", "OKAY", FrameScript_GetText("CHAR_FACTION_CHANGE_SWAP_FACTION", -1, GENDER_NOT_APPLICABLE));
            return 1;

        case 67:
            FrameScript_SignalEvent(GLUE_EVENT_OPEN_STATUS_DIALOG, "%s%s", "OKAY", FrameScript_GetText("CHAR_FACTION_CHANGE_RACE_ONLY", -1, GENDER_NOT_APPLICABLE));
            return 1;

        case 68:
            FrameScript_SignalEvent(GLUE_EVENT_OPEN_STATUS_DIALOG, "%s%s", "OKAY", FrameScript_GetText("CHAR_FACTION_CHANGE_GOLD_LIMIT", -1, GENDER_NOT_APPLICABLE));
            return 1;

        case 69:
            FrameScript_SignalEvent(GLUE_EVENT_OPEN_STATUS_DIALOG, "%s%s", "OKAY", FrameScript_GetText("CHAR_FACTION_CHANGE_FORCE_LOGIN", -1, GENDER_NOT_APPLICABLE));
            return 1;

        default:
            FrameScript_SignalEvent(GLUE_EVENT_OPEN_STATUS_DIALOG, "%s%s", "OKAY", FrameScript_GetText("CHAR_FACTION_CHANGE_FAILED", -1, GENDER_NOT_APPLICABLE));
            return 1;
    }
}

// OFFSET: 0x008024D0/0x1405149E0
void CGlueMgr::StatusDialogClick() {
    if (CGlueMgr::m_currentScreen == GLUE_SCREEN_PATCHDOWNLOAD) {
        FrameScript_SignalEvent(GLUE_EVENT_SET_GLUE_SCREEN, "%s", s_screenNames[GLUE_SCREEN_LOGIN]);
    }

    switch (CGlueMgr::m_idleState) {
        case IDLE_NONE:
            ClientServices::Connection()->Cleanup();
            break;

        case IDLE_LOGIN_SERVER_LOGIN:
            ClientServices::LoginConnection()->Logoff();
            CGlueMgr::m_showedDisconnect = 0;
            CGlueMgr::m_idleState = IDLE_NONE;
            break;

        case IDLE_ACCOUNT_LOGIN:
        case IDLE_CHARACTER_LIST:
            ClientServices::Connection()->Cancel(2);
            CCharacterSelection::ClearCharacterList();
            break;

        case IDLE_REALM_LIST:
        case IDLE_CREATE_CHARACTER:
        case IDLE_DELETE_CHARACTER:
        case IDLE_ENTER_WORLD:
            ClientServices::Connection()->Cancel(2);
            CGlueMgr::m_showedDisconnect = 0;
            CGlueMgr::m_idleState = IDLE_NONE;
            break;

        case IDLE_RENAME_CHARACTER:
        case IDLE_DECLINE_CHARACTER:
        case IDLE_CUSTOMIZE_CHARACTER:
            CGlueMgr::m_showedDisconnect = 0;
            CGlueMgr::m_idleState = IDLE_NONE;
            break;

        case IDLE_WORLD_LOGIN:
            CGlueMgr::m_idleState = IDLE_NONE;
            CGlueMgr::m_showedDisconnect = 0;
            CGlueMgr::GetCharacterList();
            break;

        case IDLE_PATCH_DOWNLOAD:
        case IDLE_SURVEY_DOWNLOAD:
            if (CGlueMgr::m_surveyDownload) {
                CGlueMgr::SurveyDownloadCancel();
                CGlueMgr::m_showedDisconnect = 0;
                CGlueMgr::m_idleState = IDLE_NONE;
            }

            break;

        default:
            break;
    }
}

// PROVEN: 6ad9eaea IDENTICAL/NONE
// OFFSET: 0x00802670/0x140516B70
void CGlueMgr::CancelLogin() {
    if (CGlueMgr::m_idleState == IDLE_LOGIN_SERVER_LOGIN) {
        CGlueMgr::StatusDialogClick();
    }
}

// OFFSET: 0x00802680
static bool CreateIntroScene(uint8_t classID, uint8_t raceID, C3Vector& position) {
    int32_t sequenceID;
    ChrClassesRec* classRec = g_chrClassesDB.GetRecord(classID);

    if (!classRec || !(sequenceID = classRec->m_cinematicSequenceID)) {
        ChrRacesRec* raceRec = g_chrRacesDB.GetRecord(raceID);

        if (!raceRec) {
            return false;
        }

        sequenceID = raceRec->m_cinematicSequenceID;

        if (!sequenceID) {
            return false;
        }
    }

    CinematicSequencesRec* sequenceRec = g_cinematicSequencesDB.GetRecord(sequenceID);

    if (!sequenceRec) {
        return false;
    }

    CinematicCameraRec* cameraRec = g_cinematicCameraDB.GetRecord(sequenceRec->m_camera[0]);

    if (!cameraRec) {
        return false;
    }

    CM2Scene* scene = M2CreateScene();
    CM2Model* model = scene->CreateModel(cameraRec->m_model, 0, 3);

    if (!model) {
        scene->Release();
        return false;
    }

    model->WaitUntilLoaded();

    C44Matrix transform;
    transform.d0 = cameraRec->m_origin[0];
    transform.d1 = cameraRec->m_origin[1];
    transform.d2 = cameraRec->m_origin[2];
    transform.RotateAroundZ(cameraRec->m_originFacing);
    model->SetWorldTransform(transform);

    scene->SetTime(0);
    model->SetBoneSequence(0xFFFFFFFF, 0, 0xFFFFFFFF, 0, 1.0f, 0, 1, 0, 0, 0);
    model->SetAnimating(1);

    C3Vector cameraPos;
    scene->Animate(cameraPos);

    void* camera = model->GetCameraByIndex(0);

    if (!camera) {
        model->Release();
        scene->Release();
        return false;
    }

    CameraGetCoord(static_cast<CCamera*>(camera), 7, &position);
    model->Release();
    scene->Release();
    return true;
}

// PROVEN: 8265971e IDENTICAL/NONE
// OFFSET: 0x00802890/0x140516FA0
void CGlueMgr::SetUIDepth(float depth) {
    CSimpleFrame* frame = static_cast<CSimpleFrame*>(CScriptObject::GetScriptObjectByName("GlueParent", CSimpleFrame::GetObjectType()));

    if (frame) {
        frame->SetFrameDepth(depth - frame->m_parentDepth, true);
    }

    frame = static_cast<CSimpleFrame*>(CScriptObject::GetScriptObjectByName("OptionsSelectFrame", CSimpleFrame::GetObjectType()));

    if (frame) {
        frame->SetFrameDepth(depth - frame->m_parentDepth, true);
    }
}

// OFFSET: 0x00802930
static int32_t FindGlueScreenEmote(int32_t classID, int32_t raceID, int32_t screen, const CGlueMgr::WeaponLoadOut& handItems, uint32_t* animKitID, uint32_t* spellVisualKitID) {
    *animKitID = 0;
    *spellVisualKitID = 0;

    if (!g_useAnimKitsOnGlueScreen) {
        return 0;
    }

    uint32_t count = g_glueScreenEmoteDB.m_numRecords;

    for (uint32_t i = 0; i < count; i++) {
        GlueScreenEmoteRec* rec = g_glueScreenEmoteDB.GetRecordByIndex(i);

        if (!rec) {
            continue;
        }

        if ((classID != rec->m_classID && rec->m_classID) || (raceID != rec->m_raceID && rec->m_raceID)) {
            continue;
        }

        if (screen != rec->m_screen && rec->m_screen != 3) {
            continue;
        }

        int32_t offHand = handItems.m_offHand;

        if (rec->m_offHandItemType != offHand && rec->m_offHandItemType && (rec->m_offHandItemType != 13 || (offHand != 17 && offHand != 21 && offHand != 22))) {
            continue;
        }

        int32_t mainHand = handItems.m_mainHand;

        if (rec->m_mainHandItemType != mainHand && rec->m_mainHandItemType && (rec->m_mainHandItemType != 13 || (mainHand != 17 && mainHand != 21 && mainHand != 22))) {
            continue;
        }

        *animKitID = rec->m_animKitID;
        *spellVisualKitID = rec->m_spellVisualKitID;
        return *animKitID || *spellVisualKitID;
    }

    return 0;
}

// OFFSET: 0x00802A20/0x140517180
void CGlueMgr::StopCharacterEmoteKits(CAnimKitManager& manager) {
    CAnimKitInst* inst = manager.GetExpressingAnimKitInst(0xFFFFFFFF, 1);
    CharacterEmoteSpellStartData* data = reinterpret_cast<CharacterEmoteSpellStartData*>(static_cast<uintptr_t>(manager.m_expressionCallbackParam));

    if (data) {
        s_emoteSpellStartAllocator.PutData(data, NULL, 0);
    }

    manager.SetExpressionCallback(NULL, 0);

    if (inst && inst->GetID() != 540 && inst->GetID() != 541) {
        inst->Stop(1, 0);
    }
}

// OFFSET: 0x00802A90
void CGlueMgr::CharacterEmoteKitFinished(CAnimKitInst* inst, uint64_t param) {
    CAnimKitManager* manager = inst->m_manager;
    CM2Model* model = manager ? manager->m_model : NULL;

    if (model) {
        CCharacterComponent::RemoveVisuals(model);
    }

    if (param) {
        CharacterEmoteKitFinishedData* data = reinterpret_cast<CharacterEmoteKitFinishedData*>(static_cast<uintptr_t>(param));
        CM2Model* rightHand = data->m_rightHand;
        CM2Model* leftHand = data->m_leftHand;
        s_emoteKitFinishedAllocator.PutData(data, NULL, 0);

        if (rightHand) {
            rightHand->AttachToParent(model, 2, NULL, 0);
            rightHand->Release();
        }

        if (leftHand) {
            leftHand->AttachToParent(model, 1, NULL, 0);
            leftHand->Release();
        }
    }

    inst->Release();
}

// OFFSET: 0x00802B10
CGlueMgr::WeaponLoadOut CGlueMgr::AddItemsFromCharInfo(const CHARACTER_INFO* info, CCharacterComponent* component, void (*guildCallback)(const WOWGUID& guid, const WOWGUID& guildGUID, void* param, bool success), void (*loadedCallback)(CM2Model* model, void* arg)) {
    CGlueMgr::WeaponLoadOut types;
    types.m_offHand = 0;
    types.m_mainHand = 0;

    if (!component) {
        return types;
    }

    for (int32_t slot = 0; slot < 23; slot++) {
        int32_t displayID = info->m_items[slot].m_displayID;
        int32_t itemVisual = info->m_items[slot].m_visualID;

        if (!displayID) {
            continue;
        }

        bool hunter = info->m_classID == 3;
        uint32_t flags = info->m_flags;

        if ((flags & CHARACTER_FLAG_HIDE_HELM) && !slot) {
            continue;
        }

        if ((flags & CHARACTER_FLAG_HIDE_CLOAK) && slot == 14) {
            continue;
        }

        ItemDisplayInfoRec displayRec;
        bool hasDisplayRec;

        if (g_itemDisplayInfoDB.GetRecord(displayID, &displayRec)) {
            hasDisplayRec = true;

            if (g_itemVisualsDB.GetRecord(displayRec.m_itemVisual)) {
                itemVisual = 0;
            }
        } else {
            hasDisplayRec = false;
        }

        int32_t inventoryType = info->m_items[slot].m_inventoryType;

        if (slot == 15 || slot == 16 || slot == 17) {
            if (StoreHandItemType(slot, inventoryType, hunter, &types)) {
                bool ranged = inventoryType == 26 || inventoryType == 25;
                GEOCOMPONENTLINKS link = component->AddHandItem(&displayRec, static_cast<INVENTORY_SLOTS>(slot), SHEATHE_NONE, false, inventoryType == 14, ranged, itemVisual);

                if (slot == 17) {
                    if (inventoryType == 15 && loadedCallback) {
                        for (CM2Model* attached = component->m_data.m_model->m_attachList; attached; attached = attached->m_attachNext) {
                            if (attached->m_attachmentId == static_cast<uint32_t>(link)) {
                                attached->SetLoadedCallback(loadedCallback, NULL);
                                break;
                            }
                        }
                    }

                    continue;
                }
            }
        } else if (slot < 19) {
            component->AddItemBySlot(static_cast<INVENTORY_SLOTS>(slot), displayID, itemVisual);
        }

        if ((slot == 18 || slot == 14 || slot == 3) && hasDisplayRec && (displayRec.m_flags & 0x19)) {
            int32_t emblemStyle;
            int32_t emblemColor;
            int32_t borderStyle;
            int32_t borderColor;
            int32_t background;

            if (GuildGetGuildTabard(info->m_guid, info->m_guildGUID, guildCallback, NULL, emblemStyle, emblemColor, borderStyle, borderColor, background)) {
                if (slot == 18) {
                    component->ApplyGuildColor(displayRec.m_flags, emblemStyle, emblemColor, borderStyle, borderColor, background);
                } else if (slot == 14) {
                    CGObject_C::ReplaceGuildTextures(component->m_data.m_model, info->m_guildGUID.m_guid, CGPlayer_C::GetPVPFactionIndex(info->m_raceID));
                } else {
                    component->ApplyShirtGuildColor(displayRec.m_flags, emblemStyle, emblemColor, borderStyle, borderColor, background);
                }
            }
        }
    }

    return types;
}

// OFFSET: 0x00802DC0
void CGlueMgr::ProcessAttachment(CSimpleModel* modelFrame, CM2Model* model, int32_t effectID, uint32_t attachID, int32_t sequenceCallback) {
    if (!model || !model->HasAttachment(attachID)) {
        return;
    }

    SpellVisualEffectNameRec* effectRec = g_spellVisualEffectNameDB.GetRecord(effectID);

    if (!effectRec) {
        return;
    }

    const char* fileName = effectRec->m_fileName;
    CM2Model* attachment = modelFrame->GetScene()->CreateModel(fileName, 0, 3);

    if (!attachment) {
        return;
    }

    C44Matrix transform;
    transform.Scale(effectRec->m_scale);
    attachment->SetWorldTransform(transform);
    attachment->AttachToParent(model, attachID, NULL, 0);

    if (sequenceCallback) {
        attachment->SetSequenceCallback(reinterpret_cast<void*>(CGlueMgr::AttachmentSequenceCallback), 0);
    }

    attachment->Release();
}

// OFFSET: 0x00802EF0/0x1405179F0
void CGlueMgr::PollLoginServerLogin() {
    if (CGlueMgr::m_loginState != LOGIN_STATE_PIN_WAIT) {
        CGlueMgr::DisplayLoginStatus();
    }

    if (CGlueMgr::m_authenticated) {
        CGlueMgr::m_idleState = IDLE_NONE;
        CGlueMgr::m_showedDisconnect = 0;
        StartAccountLogin();
        CGlueMgr::m_authenticated = 0;
        return;
    }

    switch (CGlueMgr::m_loginState) {
        case LOGIN_STATE_PIN: {
            FrameScript_SignalEvent(GLUE_EVENT_CLOSE_STATUS_DIALOG, NULL);
            const uint8_t* keyMap = ClientServices::LoginConnection()->GetKeyMap();
            FrameScript_SignalEvent(GLUE_EVENT_PLAYER_ENTER_PIN, "%d%d%d%d%d%d%d%d%d%d", keyMap[0], keyMap[1], keyMap[2], keyMap[3], keyMap[4], keyMap[5], keyMap[6], keyMap[7], keyMap[8], keyMap[9]);
            CGlueMgr::m_loginState = LOGIN_STATE_PIN_WAIT;
            break;
        }

        case LOGIN_STATE_MATRIX: {
            uint32_t width;
            uint32_t height;
            uint32_t minDigits;
            uint32_t maxDigits;
            bool flipCoords;
            uint32_t challengeCount;
            ClientServices::LoginConnection()->GetMatrixInfo(&width, &height, &minDigits, &maxDigits, &flipCoords, &challengeCount);

            CGlueMgr::m_matrixChallengeCount = challengeCount;
            CGlueMgr::m_matrixRemaining = challengeCount;
            CGlueMgr::m_loginState = LOGIN_STATE_MATRIX_WAIT;

            FrameScript_SignalEvent(GLUE_EVENT_CLOSE_STATUS_DIALOG, NULL);
            FrameScript_SignalEvent(GLUE_EVENT_PLAYER_ENTER_MATRIX, "%d%d%d%d%b%d", width, height, minDigits, maxDigits, flipCoords, challengeCount);
            break;
        }

        case LOGIN_STATE_TOKEN: {
            CGlueMgr::m_loginState = LOGIN_STATE_TOKEN_WAIT;
            FrameScript_SignalEvent(GLUE_EVENT_CLOSE_STATUS_DIALOG, NULL);
            FrameScript_SignalEvent(GLUE_EVENT_PLAYER_ENTER_TOKEN, NULL);
            break;
        }

        case LOGIN_STATE_CHECKINGVERSIONS: {
            uint8_t versionChecksum[20];
            ChecksumExecutables(ClientServices::LoginConnection()->GetVersionChallenge(), 16, versionChecksum);
            ClientServices::LoginConnection()->ProveVersion(versionChecksum);
            break;
        }

        case LOGIN_STATE_DOWNLOADFILE: {
            const char* fileName = ClientServices::LoginConnection()->m_fileName;

            if (!_stricmp(fileName, "Patch")) {
                CGlueMgr::PatchDownloadStart();
            } else if (!_stricmp(fileName, "Survey")) {
                CGlueMgr::SurveyDownloadStart();
            }

            break;
        }

        case LOGIN_STATE_FAILED: {
            ClientServices::LoginConnection()->Logoff();
            CGlueMgr::m_idleState = IDLE_NONE;
            CGlueMgr::m_showedDisconnect = 0;
            break;
        }

        case 18: {
            const char* format;
            const char* title;
            const char* text;

            if (ClientServices::LoginConnection()->GetOutOfBandChallengeType()) {
                text = ClientServices::LoginConnection()->GetOutOfBandChallengeText();
                title = ClientServices::LoginConnection()->GetOutOfBandChallengeTitle();
                format = FrameScript_GetText("PHONESECURE_ENH_TEXT", -1, GENDER_NOT_APPLICABLE);
            } else {
                text = ClientServices::LoginConnection()->GetOutOfBandChallengeText();
                title = ClientServices::LoginConnection()->GetOutOfBandChallengeTitle();
                format = FrameScript_GetText("PHONESECURE_TEXT", -1, GENDER_NOT_APPLICABLE);
            }

            char message[1024];
            SStrPrintf(message, sizeof(message), format, title, text);
            FrameScript_SignalEvent(GLUE_EVENT_CLOSE_STATUS_DIALOG, NULL);
            FrameScript_SignalEvent(GLUE_EVENT_OPEN_STATUS_DIALOG, "%s%s", "CANCEL", message);
            CGlueMgr::m_loginState = 19;
            break;
        }

        case 19: {
            const char* format;
            const char* title;
            const char* text;

            if (ClientServices::LoginConnection()->GetOutOfBandChallengeType()) {
                text = ClientServices::LoginConnection()->GetOutOfBandChallengeText();
                title = ClientServices::LoginConnection()->GetOutOfBandChallengeTitle();
                format = FrameScript_GetText("PHONESECURE_ENH_TEXT", -1, GENDER_NOT_APPLICABLE);
            } else {
                text = ClientServices::LoginConnection()->GetOutOfBandChallengeText();
                title = ClientServices::LoginConnection()->GetOutOfBandChallengeTitle();
                format = FrameScript_GetText("PHONESECURE_TEXT", -1, GENDER_NOT_APPLICABLE);
            }

            char message[1024];
            SStrPrintf(message, sizeof(message), format, title, text);

            char waiting[256];
            SStrPrintf(waiting, sizeof(waiting), FrameScript_GetText("PHONESECURE_WAITING", -1, GENDER_NOT_APPLICABLE), ClientServices::LoginConnection()->GetOutOfBandChallengeTimeLeft());
            SStrPack(message, "\n", sizeof(message));
            SStrPack(message, waiting, sizeof(message));
            FrameScript_SignalEvent(GLUE_EVENT_OPEN_STATUS_DIALOG, "%s%s", "CANCEL", message);
            break;
        }

        default:
            break;
    }
}

// OFFSET: 0x00803370
void CGlueMgr::PollEnterWorld() {
    if (!LoadingScreenDrawing()) {
        return;
    }

    if (!CGlueMgr::m_suspended) {
        CHARACTER_INFO* characterInfo = CGlueMgr::m_characterInfo;
        uint32_t mapID = characterInfo->m_mapID;
        s_activeCharacterGuid = characterInfo->m_guid;
        C3Vector position = characterInfo->m_position;

        if (!g_clientDBWorldTablesLoaded) {
            for (ClientDBTable* table = g_staticDBWorldTables; table->m_db; table++) {
                table->m_db->Load(table->m_meta);
            }

            ClientDBInitializeInGame();
            SpellVisualsInitialize();
            PhaseShift_C_InitDB();
            LoadingScreenInitializeInGame();
            SpellTableInitialize();
            ObjectEffectSystem_Initialize();
            SI3::InitSoundInGame();
            World::InitializeMaps();
            characterInfo = CGlueMgr::m_characterInfo;
            g_clientDBWorldTablesLoaded = 1;
        }

        if (characterInfo->m_firstLogin) {
            CreateIntroScene(characterInfo->m_classID, characterInfo->m_raceID, position);
            CGlueMgr::m_showingIntroScene = 1;
        } else {
            CGlueMgr::m_showingIntroScene = 0;
        }

        CCharacterSelection::TransferComponentToWorld();
        CCharacterSelection::Shutdown();
        CCharacterCreation::Shutdown();
        CGlueMgr::Suspend();
        ClientInitializeGame(mapID, position);
        return;
    }

    CGlueMgr::m_idleState = IDLE_NONE;
    CGlueMgr::m_showedDisconnect = 0;
    SI3::StopGlueMusic(3000);
    SI3::StopGlueAmbience(1000);
    ClientServices::Connection()->CharacterLogin(s_activeCharacterGuid, 0);
}

// PROVEN: 0bd5903a IDENTICAL/NONE
// OFFSET: 0x00803500
CharacterSelectionDisplay* GetCharacterInfo(int32_t index) {
    if (index < 0 || index >= static_cast<int32_t>(CCharacterSelection::m_characterList.Count())) {
        return NULL;
    }

    return &CCharacterSelection::m_characterList[index];
}

// OFFSET: 0x00803530
void CGlueMgr::ApplySpellVisualKit(CSimpleModelFFX* modelFrame, CM2Model* model, int32_t spellVisualKitID, int32_t sequenceCallback) {
    SpellVisualKitRec* kitRec = g_spellVisualKitDB.GetRecord(spellVisualKitID);

    if (!kitRec || !model) {
        return;
    }

    CGlueMgr::ProcessAttachment(modelFrame, model, kitRec->m_headEffect, 20, sequenceCallback);
    CGlueMgr::ProcessAttachment(modelFrame, model, kitRec->m_chestEffect, 34, sequenceCallback);
    CGlueMgr::ProcessAttachment(modelFrame, model, kitRec->m_baseEffect, 19, sequenceCallback);
    CGlueMgr::ProcessAttachment(modelFrame, model, kitRec->m_leftHandEffect, 21, sequenceCallback);
    CGlueMgr::ProcessAttachment(modelFrame, model, kitRec->m_rightHandEffect, 22, sequenceCallback);
    CGlueMgr::ProcessAttachment(modelFrame, model, kitRec->m_breathEffect, 17, sequenceCallback);
    CGlueMgr::ProcessAttachment(modelFrame, model, kitRec->m_specialEffect[0], 23, sequenceCallback);
    CGlueMgr::ProcessAttachment(modelFrame, model, kitRec->m_specialEffect[1], 24, sequenceCallback);
    CGlueMgr::ProcessAttachment(modelFrame, model, kitRec->m_specialEffect[2], 25, sequenceCallback);

    for (int32_t i = 0; i < 4; i++) {
        switch (kitRec->m_charProc[i]) {
            case 1: {
                uint32_t color = static_cast<uint32_t>(static_cast<int64_t>(kitRec->m_charParamZero[i])) | 0xFF000000;
                model->m_diffuseTintR = static_cast<float>(static_cast<uint8_t>(color >> 16)) * 0.0039215689f;
                model->m_diffuseTintG = static_cast<float>(static_cast<uint8_t>(color >> 8)) * 0.0039215689f;
                model->m_diffuseTintB = static_cast<float>(static_cast<uint8_t>(color)) * 0.0039215689f;
                break;
            }

            case 14: {
                float alpha = kitRec->m_charParamZero[i];

                if (0.0f > alpha) {
                    alpha = 0.0f;
                } else if (alpha > 1.0f) {
                    alpha = 1.0f;
                }

                model->m_fadeAlpha = alpha;
                break;
            }
        }
    }
}

// OFFSET: 0x00803790/0x1405184E0
int32_t CGlueMgr::DeclineCharacter(WOWGUID guid, const char** names) {
    if (CCharacterSelection::m_selectionIndex < 0 || CCharacterSelection::m_selectionIndex >= static_cast<int32_t>(CCharacterSelection::m_characterList.Count())) {
        return 1;
    }

    CharacterSelectionDisplay* display = &CCharacterSelection::m_characterList[CCharacterSelection::m_selectionIndex];

    if (!display || !display->m_characterInfo.m_guid.m_guid == guid.m_guid) {
        return 1;
    }

    for (int32_t i = 0; i < 5; i++) {
        if (!names[i] || !*names[i]) {
            return 1;
        }

        int32_t result = ClientServices::CharacterValidateDeclension(display->m_characterInfo.m_name, names[i]);

        if (result != CHAR_NAME_SUCCESS) {
            FrameScript_SignalEvent(GLUE_EVENT_FORCE_DECLINE_CHARACTER, "%s", ClientServices::GetErrorToken(result));
            return 0;
        }
    }

    CGlueMgr::SetIdleState(IDLE_DECLINE_CHARACTER);
    FrameScript_SignalEvent(GLUE_EVENT_OPEN_STATUS_DIALOG, "%s%s", "CANCEL", FrameScript_GetText("CHAR_DECLINE_IN_PROGRESS", -1, GENDER_NOT_APPLICABLE));

    CDataStore msg;
    msg.Put(static_cast<uint32_t>(CMSG_SET_PLAYER_DECLINED_NAMES));
    msg.Put(guid.m_guid);
    msg.PutString(display->m_characterInfo.m_name);

    for (int32_t i = 0; i < 5; i++) {
        msg.PutString(names[i]);
    }

    msg.Finalize();
    ClientServices::Send2(&msg);

    return 1;
}

// OFFSET: 0x00803940/0x1405186D0
void CGlueMgr::EnterWorld() {
    if (!ClientServices::GetSelectedRealm()) {
        return;
    }

    CharacterSelectionDisplay* display = CCharacterSelection::m_selectionIndex < 0 || CCharacterSelection::m_selectionIndex >= static_cast<int32_t>(CCharacterSelection::m_characterList.Count()) ? NULL : &CCharacterSelection::m_characterList[CCharacterSelection::m_selectionIndex];
    CGlueMgr::m_characterInfo = &display->m_characterInfo;

    if (!display) {
        return;
    }

    CCharacterSelection::m_enterWorldIndex = CCharacterSelection::m_selectionIndex;

    if (!ClientServices::Connection()->unk_2F3C) {
        return;
    }

    uint32_t flags = CGlueMgr::m_characterInfo->m_flags;

    if (flags & CHARACTER_LOCKED_FOR_TRANSFER) {
        FrameScript_SignalEvent(GLUE_EVENT_OPEN_STATUS_DIALOG, "%s%s", "OKAY", FrameScript_GetText(ClientServices::GetErrorToken(CHAR_LOGIN_LOCKED_FOR_TRANSFER), -1, GENDER_NOT_APPLICABLE));
        return;
    }

    if (flags & CHARACTER_FLAG_LOCKED_BY_BILLING) {
        FrameScript_SignalEvent(GLUE_EVENT_OPEN_STATUS_DIALOG, "%s%s", "OKAY", FrameScript_GetText(ClientServices::GetErrorToken(CHAR_LOGIN_LOCKED_BY_BILLING), -1, GENDER_NOT_APPLICABLE));
        return;
    }

    if (flags & CHARACTER_FLAG_RENAME) {
        FrameScript_SignalEvent(GLUE_EVENT_FORCE_RENAME_CHARACTER, "%s", "CHAR_RENAME_DESCRIPTION");
        return;
    }

    if (!(flags & CHARACTER_FLAG_DECLINED) && NameNeedsDeclensions(s_locale, CGlueMgr::m_characterInfo->m_name)) {
        FrameScript_SignalEvent(GLUE_EVENT_FORCE_DECLINE_CHARACTER, NULL);
        return;
    }

    ChrRacesRec* raceRec = g_chrRacesDB.GetRecord(CGlueMgr::m_characterInfo->m_raceID);
    ChrClassesRec* classRec;

    if (!raceRec || raceRec->m_requiredExpansion > ClientServices::Connection()->m_accountExpansion || !(classRec = g_chrClassesDB.GetRecord(CGlueMgr::m_characterInfo->m_classID)) || classRec->m_requiredExpansion > ClientServices::Connection()->m_accountExpansion) {
        FrameScript_SignalEvent(GLUE_EVENT_OPEN_STATUS_DIALOG, "%s%s", "OKAY", FrameScript_GetText(ClientServices::GetErrorToken(CHAR_LOGIN_DISABLED), -1, GENDER_NOT_APPLICABLE));
        return;
    }

    ClientServices::SetAccountName(CGlueMgr::m_accountName);
    ClientServices::SetCharacterInfo(CGlueMgr::m_characterInfo);

    if (!s_curGameTip) {
        s_curGameTip = CVar::Register("gameTip", NULL, 0, "0", NULL, CATEGORY_DEFAULT, false, NULL, false);
    }

    if (!s_showGameTips) {
        s_showGameTips = CVar::Register("showGameTips", NULL, 0, "1", NULL, CATEGORY_DEFAULT, false, NULL, false);
    }

    if (!CGlueMgr::m_characterInfo->m_firstLogin && s_showGameTips->m_intValue) {
        int32_t tip = s_curGameTip->m_intValue;
        int32_t count = g_gameTipsDB.m_numRecords;

        if (tip < 0 || tip >= count) {
            tip = 0;
        }

        if (count > 0) {
            GameTipsRec* tipRec = g_gameTipsDB.GetRecordByIndex(tip);
            int32_t start = tip;

            do {
                uint32_t level = CGlueMgr::m_characterInfo->m_experienceLevel;

                if ((!tipRec->m_minLevel || level >= static_cast<uint32_t>(tipRec->m_minLevel)) && (!tipRec->m_maxLevel || level <= static_cast<uint32_t>(tipRec->m_maxLevel))) {
                    break;
                }

                tip = (tip + 1) % count;
                tipRec = g_gameTipsDB.GetRecordByIndex(tip);
            } while (tip != start);

            LoadingScreenSetTip(tipRec->m_text);

            char tipString[40];
            SStrPrintf(tipString, sizeof(tipString), "%d", tip + 1);
            s_curGameTip->Set(tipString, true, false, false, true);
        }
    }

    if (SFile::IsStreamingMode() && !CCharacterSelection::IsStreamingDataReady()) {
        LoadingScreenSetScreenTipForRaceData();
    }

    LoadingScreenEnable(CGlueMgr::m_characterInfo->m_mapID, true);
    ConsoleSaveAllClientCVar();

    if (!ClientServices::LoginConnection()->GetLoginServerType()) {
        ClientServices::LoginConnection()->Logoff();
    }

    CGlueMgr::SetIdleState(IDLE_ENTER_WORLD);
}

// OFFSET: 0x00803CC0
void CGlueMgr::CharacterEmoteSpellStart(CAnimKitManager* manager, uint64_t param) {
    CharacterEmoteSpellStartData* data = reinterpret_cast<CharacterEmoteSpellStartData*>(static_cast<uintptr_t>(param));
    CM2Model* model = manager->m_model;

    if (!model->IsLoaded()) {
        return;
    }

    M2BoneSequenceInfo info;

    if (!model->GetBoneSequenceInfo(0xFFFFFFFF, info)) {
        return;
    }

    AnimationDataRec* animRec = g_animationDataDB.GetRecord(info.m_sequenceId);

    if (!(animRec && (animRec->m_flags & 0x8)) && !(animRec->m_flags & 0x800)) {
        return;
    }

    CAnimKitInst* inst = manager->GetExpressingAnimKitInst(0xFFFFFFFF, 1);
    CharacterEmoteKitFinishedData* finished = NULL;

    if (inst) {
        CM2Model* rightHand = NULL;

        for (CM2Model* attached = model->m_attachList; attached; attached = attached->m_attachNext) {
            if (attached->m_attachmentId == 2) {
                attached->m_refCount++;
                rightHand = attached;
                attached->DetachFromParent();
                break;
            }
        }

        CM2Model* leftHand = NULL;

        for (CM2Model* attached = model->m_attachList; attached; attached = attached->m_attachNext) {
            if (attached->m_attachmentId == 1) {
                attached->m_refCount++;
                leftHand = attached;
                attached->DetachFromParent();
                break;
            }
        }

        finished = s_emoteKitFinishedAllocator.New<CharacterEmoteKitFinishedData>(0);
        finished->m_rightHand = rightHand;
        finished->m_leftHand = leftHand;
    }

    int32_t sequenceCallback = info.m_sequenceId == 54 || info.m_sequenceId == 124 || info.m_sequenceId == 55;
    CGlueMgr::ApplySpellVisualKit(data->m_modelFrame, model, data->m_spellVisualKitID, sequenceCallback);
    manager->SetExpressionCallback(NULL, 0);
    s_emoteSpellStartAllocator.PutData(data, NULL, 0);

    if (inst) {
        inst->SetStopCallback(CGlueMgr::CharacterEmoteKitFinished, reinterpret_cast<uintptr_t>(finished));
    }
}

// OFFSET: 0x00803E30
void CGlueMgr::AddChangedOptionWarning(const char* warning) {
    if (!warning) {
        return;
    }

    const char* text = FrameScript_GetText(warning, -1, GENDER_NOT_APPLICABLE);

    if (!text) {
        return;
    }

    if (CGlueMgr::m_changedOptionWarnings.Count() <= CGlueMgr::m_numChangedOptionWarnings) {
        CGlueMgr::m_changedOptionWarnings.SetCount(2 * CGlueMgr::m_changedOptionWarnings.Count());
    }

    CGlueMgr::m_changedOptionWarnings[CGlueMgr::m_numChangedOptionWarnings++] = text;
}

// OFFSET: 0x00803ED0/0x140518DF0
bool CGlueMgr::AgreementsURLCallback(void* param, const char* data, uint32_t size, bool done, HTTPError::ErrorCode error) {
    CGlueMgr::Agreements* agreements = static_cast<CGlueMgr::Agreements*>(param);

    if (!agreements) {
        return true;
    }

    if (size) {
        agreements->m_data.Add(size, data);
    }

    if (done) {
        agreements->m_complete = 1;

        if (agreements->m_data.Count()) {
            CGlueMgr::ProcessAgreements(agreements);
            agreements->m_data.Clear();
        }
    }

    return true;
}

// OFFSET: 0x00803F20
int32_t CGlueMgr::OnCharRenameResult(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    uint8_t result;
    WOWGUID guid = { 0 };
    char name[48];

    msg->Get(result);

    if (!msg->IsRead()) {
        msg->Get(guid.m_guid);
        msg->GetString(name, sizeof(name));
    }

    FrameScript_SignalEvent(GLUE_EVENT_CLOSE_STATUS_DIALOG, NULL);

    if (result) {
        if (result == 50) {
            FrameScript_SignalEvent(GLUE_EVENT_FORCE_RENAME_CHARACTER, "%s", ClientServices::GetErrorToken(50));
        } else {
            FrameScript_SignalEvent(GLUE_EVENT_FORCE_RENAME_CHARACTER, "%s", FrameScript_GetText("CHAR_RENAME_FAILED", -1, GENDER_NOT_APPLICABLE));
        }

        return 1;
    }

    CCharacterSelection::UpdateCharacterName(guid, name);

    if (CCharacterSelection::m_selectionIndex >= 0 && CCharacterSelection::m_selectionIndex < static_cast<int32_t>(CCharacterSelection::m_characterList.Count())) {
        CharacterSelectionDisplay* display = &CCharacterSelection::m_characterList[CCharacterSelection::m_selectionIndex];

        if (display && display->m_characterInfo.m_guid == guid) {
            CGlueMgr::EnterWorld();
        }
    }

    return 1;
}

// OFFSET: 0x00804020
int32_t CGlueMgr::OnCharDeclineResult(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg) {
    WOWGUID guid = { 0 };
    uint32_t result;

    FrameScript_SignalEvent(GLUE_EVENT_CLOSE_STATUS_DIALOG, NULL);
    msg->Get(result);

    if (result) {
        FrameScript_SignalEvent(GLUE_EVENT_FORCE_DECLINE_CHARACTER, "%s", "CHAR_DECLINE_FAILED");
        return 1;
    }

    msg->Get(guid.m_guid);
    CCharacterSelection::UpdateCharacterNameDeclension(guid);

    if (CCharacterSelection::m_selectionIndex >= 0 && CCharacterSelection::m_selectionIndex < static_cast<int32_t>(CCharacterSelection::m_characterList.Count())) {
        CharacterSelectionDisplay* display = &CCharacterSelection::m_characterList[CCharacterSelection::m_selectionIndex];

        if (display && display->m_characterInfo.m_guid == guid) {
            CGlueMgr::EnterWorld();
        }
    }

    return 1;
}

// OFFSET: 0x008040D0/0x140519020
void CGlueMgr::PlayCharacterEmoteKits(int32_t classID, int32_t raceID, int32_t screen, const CGlueMgr::WeaponLoadOut& handItems, CAnimKitManager& manager, CSimpleModelFFX* modelFrame) {
    uint32_t animKitID = 0;
    uint32_t spellVisualKitID = 0;

    if (!FindGlueScreenEmote(classID, raceID, screen, handItems, &animKitID, &spellVisualKitID) || !animKitID) {
        return;
    }

    CGlueMgr::StopCharacterEmoteKits(manager);
    CAnimKitInst* inst = manager.PlayAndMaintain(animKitID, -1, 0);

    if (!inst) {
        return;
    }

    if (spellVisualKitID) {
        CharacterEmoteSpellStartData* data = s_emoteSpellStartAllocator.New<CharacterEmoteSpellStartData>(0);
        data->m_modelFrame = modelFrame;
        data->m_spellVisualKitID = spellVisualKitID;
        manager.SetExpressionCallback(CGlueMgr::CharacterEmoteSpellStart, reinterpret_cast<uintptr_t>(data));
    }

    inst->SetStopCallback(CGlueMgr::CharacterEmoteKitFinished, 0);
}

// PROVEN: f6dbeaae IDENTICAL/NONE
// OFFSET: 0x00804170/0x140519100
void CGlueMgr::AcceptChangedOptionWarnings() {
    CGlueMgr::m_changedOptionWarnings.Clear();
    CGlueMgr::m_numChangedOptionWarnings = 0;
}

// PROVEN: ab6bb506 IDENTICAL/NONE
// OFFSET: 0x008041B0/0x140519150
static void RequestLatestAgreements() {
    CGlueMgr::m_latestAgreements.m_data.Clear();
    CGlueMgr::m_latestAgreements.m_complete = 0;

    if (!OsURLDownload(FrameScript_GetText("LATEST_AGREEMENTS_URL", -1, GENDER_NOT_APPLICABLE), CGlueMgr::AgreementsURLCallback, &CGlueMgr::m_latestAgreements)) {
        CGlueMgr::m_latestAgreements.m_complete = 1;
    }
}

// OFFSET: 0x00804220
void CGlueMgr::LogConnectionStatus(const char* format, ...) {
    if (!CGlueMgr::m_initialized || !format || !*format) {
        return;
    }

    s_connectionStatusLock.Enter();

    ConsoleString* string = s_connectionStatus.NewNode(2, 0, 8);

    va_list args;
    va_start(args, format);
    SStrVPrintf(string->m_text, sizeof(string->m_text), format, args);
    va_end(args);

    WriteConnectionLog(string->m_text);

    s_connectionStatusLock.Leave();
}

// PROVEN: d3e56ff2 IDENTICAL/NONE
// OFFSET: 0x00804300/0x140519350
void CGlueMgr::SetLoginStateAndResult(LOGIN_STATE state, LOGIN_RESULT result, const char* addressString, const char* stateString, const char* resultString, uint16_t flags) {
    CGlueMgr::LogConnectionStatus("GRUNT: state: %s result: %s %s", stateString, resultString, addressString ? addressString : "");

    if (flags & 0x1) {
        CGlueMgr::m_accountMsgAvailable = 1;
    }

    CGlueMgr::m_loginState = state;

    if (state == LOGIN_STATE_AUTHENTICATED) {
        CGlueMgr::m_authenticated = 1;
    }

    CGlueMgr::m_loginResult = result;
}

// OFFSET: 0x00804440/0x140519470
void CGlueMgr::Resume() {
    CGlueMgr::m_disconnectPending = 0;
    CGlueMgr::m_reconnect = 0;
    CGlueMgr::m_idleState = IDLE_NONE;
    CGlueMgr::m_showedDisconnect = 0;
    CGlueMgr::m_characterInfo = NULL;
    CGlueMgr::m_suspended = 0;
    CGlueMgr::m_reload = 0;
    CGlueMgr::m_elapsedMs = 0;
    CGlueMgr::m_timeMs = 0;

    CRect rect = { 0.0f, 0.0f, 0.0f, 0.0f };
    g_theGxDevicePtr->CapsWindowSizeInScreenCoords(rect);
    CGlueMgr::m_screenWidth = static_cast<int32_t>(rect.maxX - rect.minX);
    CGlueMgr::m_screenHeight = static_cast<int32_t>(rect.maxY - rect.minY);
    CGlueMgr::m_aspect = g_theGxDevicePtr->CapsAspectRatio(0);
    CoordinateSetAspectRatio(CGlueMgr::m_aspect);

    CGlueMgr::m_simpleTop = new (SMemAlloc(sizeof(CSimpleTop), __FILE__, __LINE__, 0)) CSimpleTop();
    CGlueMgr::m_simpleTop->m_displaySizeCallback = CGlueMgr::HandleDisplaySizeChanged;

    uint32_t width = 0;
    uint32_t height = 0;
    PIXEL_FORMAT format = PIXEL_ARGB8888;
    CGlueMgr::m_cursorMipBits = TextureLoadImage("Interface\\Cursor\\Point.blp", &width, &height, &format, NULL, NULL, NULL, 0);
    CGlueMgr::m_simpleTop->SetCursor(CGlueMgr::m_cursorMipBits);
    CGlueMgr::m_simpleTop->SetCursor(CGlueMgr::m_cursorMipBits);

    CGInputControl* control = CGInputControl::GetActive();
    control->m_mouseModeFlags = 3;
    control->m_forceCursorOn = 0;
    control->UpdateMouseMode(1);

    FrameScript_Flush();
    SystemRegisterFunctions();
    RegisterSimpleFrameScriptMethods();
    RegisterSharedScriptFunctions();
    GlueScriptEventsRegisterFunctions();
    CharSelectRegisterScriptFunctions();
    CharacterCreateRegisterScriptFunctions();
    RealmListRegisterScriptFunctions();
    SI3::RegisterScriptFunctions();
    AccountMsg_RegisterScriptFunctions();
    CGVideoOptions::RegisterScriptFunctions();
    CGlueMgr::s_scriptFunctionsLoaded = 1;
    FrameScript_CreateEvents(g_glueScriptEvents, NUM_GLUE_SCRIPT_EVENTS);

    OsCreateDirectory("Logs", 0);
    CWOWClientStatus status("Logs\\GlueXML.log");

    DeleteInterfaceFiles();

    MD5_CTX md5;
    MD5Init(&md5);

    uint8_t expectedDigest[16];

    switch (FrameXML_CheckSignature("Interface\\GlueXML\\GlueXML.toc", NULL, InterfaceKey, expectedDigest)) {
        case 0:
            status.Add(STATUS_WARNING, "GlueXML missing signature");
            break;

        case 1:
            status.Add(STATUS_WARNING, "GlueXML has corrupt signature");
            break;

        case 2:
            status.Add(STATUS_WARNING, "GlueXML is modified or corrupt");
            break;

        case 3: {
            FrameXML_FreeHashNodes();
            ConsoleCommandRegister("ExportInterfaceFiles", CGlueMgr::CCommand_ExportInterfaceFiles, CATEGORY_DEBUG, NULL, NULL);
            FrameXML_CreateFrames("Interface\\GlueXML\\GlueXML.toc", NULL, &md5, &status);

            uint8_t digest[16];
            MD5Final(digest, &md5);

#ifndef DISABLE_SIGNATURE_CHECKS
            if (memcmp(expectedDigest, digest, sizeof(digest))) {
                break;
            }
#endif

            SetVisitableURL(0, FrameScript_GetText("VISITABLE_URL_GENERIC", -1, GENDER_NOT_APPLICABLE));

            for (int32_t i = 1; i < 4; i++) {
                char name[256];
                SStrPrintf(name, sizeof(name), "%s%d", "VISITABLE_URL", i);
                const char* url = FrameScript_GetText(name, -1, GENDER_NOT_APPLICABLE);

                if (!url || !*url) {
                    url = FrameScript_GetText("VISITABLE_URL_GENERIC", -1, GENDER_NOT_APPLICABLE);
                }

                SetVisitableURL(i, url);
            }

            FrameScript_SignalEvent(GLUE_EVENT_FRAMES_LOADED, NULL);
            CGlueMgr::InitializeFFX();

            ClientServices::SetMessageHandler(SMSG_CHARACTER_RENAME_RESULT, CGlueMgr::OnCharRenameResult, NULL);
            ClientServices::SetMessageHandler(SMSG_SET_PLAYER_DECLINED_NAMES_RESULT, CGlueMgr::OnCharDeclineResult, NULL);
            ClientServices::SetMessageHandler(SMSG_CHAR_CUSTOMIZE, CGlueMgr::OnCharCustomizeResult, NULL);
            ClientServices::SetMessageHandler(SMSG_REALM_SPLIT, CGlueMgr::OnRealmSplitMsg, NULL);
            ClientServices::SetMessageHandler(SMSG_KICK_REASON, CGlueMgr::OnKickReasonMsg, NULL);
            ClientServices::SetMessageHandler(SMSG_CHAR_FACTION_CHANGE_RESULT, CGlueMgr::OnCharFactionChangeResult, NULL);
            ClientGenerateRandomCharacterNameResult::s_handler = CGlueMgr::OnRandomCharacterNameResult;

            CGlueMgr::m_pendingServerAlert = 1;
            CGlueMgr::m_processServerAlert = 0;
            CGlueMgr::m_serverAlert[0] = '\0';

            if (!OsURLDownload(FrameScript_GetText(ClientServices::GetServerAlertURL(), -1, GENDER_NOT_APPLICABLE), CGlueMgr::ServerAlertURLCallback, NULL)) {
                CGlueMgr::m_pendingServerAlert = 0;
            }

            EventRegisterEx(EVENT_ID_PAINT, CGlueMgr::Paint, NULL, 0.0f);
            return;
        }

        default:
            break;
    }

    ClientPostClose(9);
}

// OFFSET: 0x008048A0
void CGlueMgr::PollAccountLogin(int32_t errorCode, const char* text, int32_t complete, int32_t result, WOWCS_OPS op) {
    if (ClientServices::LoginConnection()->GetLoginServerType() == 1 && CGlueMgr::m_loginState == LOGIN_STATE_FAILED) {
        CGlueMgr::DisplayLoginStatus();
        ClientServices::Connection()->Cancel(2);
        ClientServices::LoginConnection()->Logoff();
        CGlueMgr::m_idleState = IDLE_NONE;
        CGlueMgr::m_showedDisconnect = 0;
        return;
    }

    if (errorCode == AUTH_WAIT_QUEUE) {
        const char* realmName = ClientServices::GetSelectedRealmName();
        bool hasRealmName = realmName && *realmName;

        if (ClientServices::Connection()->m_freeCharacterMigration) {
            CGlueMgr::m_displayingQueueDialog = 1;
            FrameScript_SignalEvent(GLUE_EVENT_OPEN_STATUS_DIALOG, "%s", "QUEUED_WITH_FCM");
        }

        if (CGlueMgr::m_estimatedWaitTime > 0) {
            char timeLeft[256];
            int32_t remaining;

            if (CGlueMgr::m_estimatedWaitTime + CGlueMgr::m_queueTime[0] - FrameTime::GetCurTimeMs() <= 0 || (remaining = CGlueMgr::m_estimatedWaitTime + CGlueMgr::m_queueTime[0] - FrameTime::GetCurTimeMs()) < 60000) {
                if (hasRealmName) {
                    SStrPrintf(timeLeft, sizeof(timeLeft), FrameScript_GetText("QUEUE_NAME_TIME_LEFT_SECONDS", -1, GENDER_NOT_APPLICABLE), realmName, CGlueMgr::m_queuePosition[0]);
                } else {
                    SStrPrintf(timeLeft, sizeof(timeLeft), FrameScript_GetText("QUEUE_TIME_LEFT_SECONDS", -1, GENDER_NOT_APPLICABLE), CGlueMgr::m_queuePosition[0]);
                }
            } else {
                int32_t minutes = remaining / 60000;

                if (hasRealmName) {
                    SStrPrintf(timeLeft, sizeof(timeLeft), FrameScript_GetText("QUEUE_NAME_TIME_LEFT", -1, GENDER_NOT_APPLICABLE), realmName, CGlueMgr::m_queuePosition[0], minutes);
                } else {
                    SStrPrintf(timeLeft, sizeof(timeLeft), FrameScript_GetText("QUEUE_TIME_LEFT", -1, GENDER_NOT_APPLICABLE), CGlueMgr::m_queuePosition[0], minutes);
                }
            }

            char queueText[512];
            SStrPrintf(queueText, sizeof(queueText), "%s\n%s", text, timeLeft);

            if (ClientServices::Connection()->m_freeCharacterMigration) {
                SStrPack(queueText, "\n\n", sizeof(queueText));
                SStrPack(queueText, FrameScript_GetText("QUEUE_FCM", -1, GENDER_NOT_APPLICABLE), sizeof(queueText));
            }

            FrameScript_SignalEvent(GLUE_EVENT_UPDATE_STATUS_DIALOG, "%s%s", queueText, FrameScript_GetText("CHANGE_REALM", -1, GENDER_NOT_APPLICABLE));
        } else {
            char queueText[512];

            if (hasRealmName) {
                SStrPrintf(queueText, sizeof(queueText), FrameScript_GetText("QUEUE_NAME_TIME_LEFT_UNKNOWN", -1, GENDER_NOT_APPLICABLE), realmName, CGlueMgr::m_queuePosition[0]);
            } else {
                SStrPrintf(queueText, sizeof(queueText), FrameScript_GetText("QUEUE_TIME_LEFT_UNKNOWN", -1, GENDER_NOT_APPLICABLE), CGlueMgr::m_queuePosition[0]);
            }

            if (ClientServices::Connection()->m_freeCharacterMigration) {
                SStrPack(queueText, "\n\n", sizeof(queueText));
                SStrPack(queueText, FrameScript_GetText("QUEUE_FCM", -1, GENDER_NOT_APPLICABLE), sizeof(queueText));
            }

            FrameScript_SignalEvent(GLUE_EVENT_UPDATE_STATUS_DIALOG, "%s%s", queueText, FrameScript_GetText("CHANGE_REALM", -1, GENDER_NOT_APPLICABLE));
        }
    } else {
        if (errorCode == RESPONSE_FAILED_TO_CONNECT && ClientServices::HasNextRealmAddress()) {
            ClientServices::AdvanceRealmAddress();
            ClientServices::ConnectToSelectedServer();
            return;
        }

        if (CGlueMgr::m_displayingQueueDialog) {
            FrameScript_SignalEvent(GLUE_EVENT_OPEN_STATUS_DIALOG, "%s", "CANCEL");
            CGlueMgr::m_displayingQueueDialog = 0;
        }

        FrameScript_SignalEvent(GLUE_EVENT_UPDATE_STATUS_DIALOG, "%s", text);
    }

    if (!complete) {
        return;
    }

    if (!result) {
        if (errorCode != RESPONSE_CANCELLED) {
            uint32_t i;

            for (i = 0; i < sizeof(s_loginErrorURLs) / sizeof(s_loginErrorURLs[0]); i++) {
                if (errorCode == s_loginErrorURLs[i].m_errorCode) {
                    break;
                }
            }

            if (i < sizeof(s_loginErrorURLs) / sizeof(s_loginErrorURLs[0])) {
                FrameScript_SignalEvent(GLUE_EVENT_OPEN_STATUS_DIALOG, "%s%s%s", "OKAY_WITH_URL", text, s_loginErrorURLs[i].m_url);
            } else {
                FrameScript_SignalEvent(GLUE_EVENT_OPEN_STATUS_DIALOG, "%s%s", "OKAY", text);
            }
        }

        CGlueMgr::m_idleState = IDLE_NONE;
        CGlueMgr::m_showedDisconnect = 0;

        if (errorCode == RESPONSE_CANCELLED) {
            CGlueMgr::m_disconnectPending = 1;
            ClientServices::Connection()->Disconnect(static_cast<DISCONNECT_REASON>(14));

            if (ClientServices::LoginConnection()->GetLoginServerType() == 1) {
                ClientServices::LoginConnection()->Logoff();
            }
        } else if (errorCode == AUTH_FAILED) {
            if (CGlueMgr::m_currentScreen == GLUE_SCREEN_CHARSELECT) {
                FrameScript_SignalEvent(GLUE_EVENT_SET_GLUE_SCREEN, "%s", s_screenNames[GLUE_SCREEN_LOGIN]);
            }

            return;
        }

        CCharacterSelection::ClearCharacterList();

        if (!ClientServices::CreateLoginResponse()->m_realmList.m_count) {
            CGlueMgr::GetRealmList(1);
            return;
        }

        FrameScript_SignalEvent(GLUE_EVENT_CLOSE_STATUS_DIALOG, NULL);
        CRealmList::UpdateList();
        return;
    }

    if (op == COP_CONNECT) {
        ClientServices::Connection()->AccountLogin(CGlueMgr::m_accountName, "", s_region, s_locale);
        return;
    }

    if (CGlueMgr::m_expansionArchivesTotalChunks != CGlueMgr::m_expansionArchivesInitChunks) {
        int32_t stalled = 0;

        if (ClientContinueInitializingArchives(&stalled)) {
            CGlueMgr::m_expansionArchivesInitChunks = CGlueMgr::m_expansionArchivesTotalChunks;
            return;
        }

        if (!stalled && CGlueMgr::m_expansionArchivesInitChunks + 1 < CGlueMgr::m_expansionArchivesTotalChunks) {
            CGlueMgr::m_expansionArchivesInitChunks++;
        }

        char progress[256];
        SStrPrintf(progress, FrameScript_GetText("LOGIN_INIT_DATA", -1, GENDER_NOT_APPLICABLE), 100 * CGlueMgr::m_expansionArchivesInitChunks / CGlueMgr::m_expansionArchivesTotalChunks);
        FrameScript_SignalEvent(GLUE_EVENT_OPEN_STATUS_DIALOG, "%s%s", "CANCEL", progress);
        return;
    }

    CGlueMgr::m_expansionArchivesTotalChunks = ClientBeginInitializingArchives(ClientServices::Connection()->m_activeExpansion);

    if (CGlueMgr::m_expansionArchivesTotalChunks) {
        return;
    }

    CGlueMgr::m_expansionArchivesTotalChunks = 0;
    CGlueMgr::m_expansionArchivesInitChunks = 0;

    if (!OpenExpansionArchives(ClientServices::Connection()->m_accountExpansion, ClientServices::Connection()->m_activeExpansion)) {
        CGlueMgr::m_disconnectPending = 1;

        if (ClientServices::LoginConnection()->GetLoginServerType() == 1) {
            ClientServices::Connection()->Disconnect(static_cast<DISCONNECT_REASON>(16));
            ClientServices::LoginConnection()->Logoff();
        } else {
            ClientServices::Connection()->Disconnect(static_cast<DISCONNECT_REASON>(16));
        }

        FrameScript_SignalEvent(GLUE_EVENT_DISCONNECTED_FROM_SERVER, "%d", 0);
        CGlueMgr::m_idleState = IDLE_NONE;
        CGlueMgr::m_showedDisconnect = 0;
        return;
    }

    WoWReport::SetInitialDownloadData();
    CGlueMgr::m_idleState = IDLE_NONE;
    CGlueMgr::m_showedDisconnect = 0;
    CCharacterSelection::ClearCharacterList();
    CGlueMgr::m_queuePosition[0] = 0;
    CGLCD::ClearWaitQueueScreen();

    CVar* accountType = CVar::Lookup("accounttype");
    const char* type = accountType->m_stringValue.m_str;

    if (!_stricmp(type, "RT") && !ClientServices::LoginConnection()->IsTrialAccount()) {
        char version[16];
        SStrPrintf(version, sizeof(version), "%d", 15595);
        WoWReport::SetConverted(version, s_localeCode);
    }

    if ((!type || !*type || !_stricmp(type, "ST") || !_stricmp(type, "RT")) && ClientServices::LoginConnection()->IsTrialAccount()) {
        accountType->Set("RT", true, false, false, true);
    } else {
        switch (ClientServices::Connection()->m_activeExpansion) {
            case 0:
                accountType->Set("CL", true, false, false, true);
                break;

            case 1:
                accountType->Set("BC", true, false, false, true);
                break;

            case 2:
                accountType->Set("LK", true, false, false, true);
                break;

            case 3:
                accountType->Set("CT", true, false, false, true);
                break;

            default:
                break;
        }
    }

    Cfg_CategoriesRec* category = g_cfg_CategoriesDB.GetRecord(ClientServices::GetSelectedRealm()->m_category);

    if (category) {
        ValidateNameInitialize(category->m_localeMask, category->m_createCharsetMask, category->m_existingCharsetMask);
    } else {
        ValidateNameInitialize(1 << s_locale, 0, 0);
    }

    if (!g_clientDBGlueTablesLoaded) {
        for (ClientDBTable* table = g_staticDBGlueTables; table->m_db; table++) {
            table->m_db->Load(table->m_meta);
        }

        ClientDBInitializeGlue();
        g_clientDBGlueTablesLoaded = 1;
    }

    CGlueMgr::SetScreen(GLUE_SCREEN_CHARSELECT);
}

// OFFSET: 0x00805130/0x14051A590
int32_t CGlueMgr::NetDisconnectHandler(const void* data, void* param) {
    bool displayScript = CGlueMgr::m_idleState != IDLE_ACCOUNT_LOGIN;
    CGlueMgr::m_idleState = IDLE_NONE;
    CGlueMgr::m_showedDisconnect = 0;

    if (CGlueMgr::m_disconnectPending) {
        ConsolePrintf("CGlueMgr::NetDisconnectHandler: Disconnect pending");
        CGlueMgr::m_disconnectPending = 0;

        if (CGlueMgr::m_reconnect) {
            CGlueMgr::m_reconnect = 0;
            CGlueMgr::SetIdleState(IDLE_ACCOUNT_LOGIN);
            FrameScript_SignalEvent(GLUE_EVENT_OPEN_STATUS_DIALOG, "%s%s", "CANCEL", FrameScript_GetText("GAME_SERVER_LOGIN", -1, GENDER_NOT_APPLICABLE));
            ClientServices::Connection()->Connect();
        }

        return 1;
    }

    if (!ClientServices::ValidDisconnect(data)) {
        ConsolePrintf("CGlueMgr::NetDisconnectHandler: Invalid disconnect");
        return 1;
    }

    ClientDestroyGame(0, 1, 0);
    EventSetMouseMode(MOUSE_MODE_NORMAL, 0);

    if (CGlueMgr::m_suspended) {
        CGlueMgr::Resume();
    }

    if (displayScript) {
        ConsolePrintf("CGlueMgr::NetDisconnectHandler: Displaying script");
    } else {
        ConsolePrintf("CGlueMgr::NetDisconnectHandler: NOT displaying script");

        WOWCS_OPS op;
        const char* text = NULL;
        int32_t result;
        int32_t errorCode;

        if (ClientServices::Connection()->PollStatus(op, &text, result, errorCode) && !result) {
            FrameScript_SignalEvent(GLUE_EVENT_OPEN_STATUS_DIALOG, "%s%s", "OKAY", text);
            ClientServices::LoginConnection()->Logoff();
            return 1;
        }

        ClientServices::SelectRealm("");
    }

    FrameScript_SignalEvent(GLUE_EVENT_DISCONNECTED_FROM_SERVER, "%d", CGlueMgr::m_clientKickReason);
    ClientServices::LoginConnection()->Logoff();
    return 1;
}

// PROVEN: 4d713df3 IDENTICAL/NONE
// OFFSET: 0x008052D0/0x14051A770
static void ReloadGlue() {
    CCharacterCreation::Shutdown();
    CCharacterSelection::Shutdown();
    CGlueMgr::Suspend();
    CGlueMgr::Resume();
    RequestLatestAgreements();
    FrameScript_SignalEvent(GLUE_EVENT_SET_GLUE_SCREEN, "%s", s_screenNames[CGlueMgr::m_currentScreen]);
}

// OFFSET: 0x00805310/0x14051A7C0
int32_t CGlueMgr::Idle(const void* data, void* param) {
    if (g_theGxDevicePtr->StereoEnabled()) {
        CGlueMgr::SetUIDepth(g_theGxDevicePtr->StereoGetConvergence());
    }

    if (ClientServices::LoginConnection()) {
        ClientServices::LoginConnection()->Update();
    }

    if (CGlueMgr::m_processServerAlert) {
        FrameScript_SignalEvent(GLUE_EVENT_SHOW_SERVER_ALERT, "%s", &CGlueMgr::m_serverAlert[(CGlueMgr::m_serverAlert[0] != '\xEF' ? 0 : 3) + strlen(s_serverAlertTag)]);
        CGlueMgr::m_processServerAlert = 0;
    }

    if (CGlueMgr::m_pendingTimerAlert) {
        FrameScript_SignalEvent(GLUE_EVENT_TIMER_ALERT, "%d", CGlueMgr::m_pendingTimerAlert);
        CGlueMgr::m_pendingTimerAlert = 0;
    }

    AsyncFileReadGetStreamingQueue();
    CKBPage::UpdateLoadingQueue();

    if (!CGlueMgr::m_idleState) {
        if (CGlueMgr::m_reload) {
            if (!CGlueMgr::m_suspended) {
                ReloadGlue();
            }

            CGlueMgr::m_reload = 0;
        }

        if (CGlueMgr::m_accountMsgAvailable) {
            FrameScript_SignalEvent(GLUE_EVENT_ACCOUNT_MESSAGES_AVAILABLE, NULL);
            CGlueMgr::m_accountMsgAvailable = 0;
        }

        CGlueMgr::HandleBattlenetDisconnect();
        return 1;
    }

    s_connectionStatusLock.Enter();

    if (s_connectionStatus.Head()) {
        for (ConsoleString* string = s_connectionStatus.Head(); string; string = s_connectionStatus.Next(string)) {
            ConsoleWrite(string->m_text, DEFAULT_COLOR);
        }

        s_connectionStatus.Clear();
    }

    s_connectionStatusLock.Leave();

    const char* text = NULL;
    WOWCS_OPS op;
    int32_t result;
    int32_t errorCode;
    int32_t complete = ClientServices::Connection()->PollStatus(op, &text, result, errorCode);

    switch (CGlueMgr::m_idleState) {
        case IDLE_LOGIN_SERVER_LOGIN:
            CGlueMgr::PollLoginServerLogin();
            break;

        case IDLE_ACCOUNT_LOGIN:
            CGlueMgr::PollAccountLogin(errorCode, text, complete, result, op);
            break;

        case IDLE_CHARACTER_LIST:
            CGlueMgr::PollCharacterList(text, complete, result, errorCode);
            break;

        case IDLE_REALM_LIST:
            CGlueMgr::PollRealmList(text, complete, result);
            break;

        case IDLE_CREATE_CHARACTER:
            CGlueMgr::PollCreateCharacter(text, complete, result);
            break;

        case IDLE_DELETE_CHARACTER:
            CGlueMgr::PollDeleteCharacter(text, complete, result);
            break;

        case IDLE_ENTER_WORLD:
            CGlueMgr::PollEnterWorld();
            break;

        case IDLE_WORLD_LOGIN:
            FrameScript_SignalEvent(GLUE_EVENT_OPEN_STATUS_DIALOG, "%s%s", "OKAY", text);
            break;

        case IDLE_PATCH_DOWNLOAD:
            if (CGlueMgr::m_patchDownload) {
                CGlueMgr::PatchDownloadIdle();
                break;
            }

            if (CGlueMgr::m_surveyDownload && CGlueMgr::SurveyDownloadIdle()) {
                CGlueMgr::PollUserSurvey();
            }

            break;

        case IDLE_SURVEY_DOWNLOAD:
            CGlueMgr::PollUserSurvey();
            break;

        case IDLE_SCANDLL:
            if (CGlueMgr::m_scandllStatus == 3 || CGlueMgr::m_scandllStatus == 2) {
                CGlueMgr::ScanDLLFinished();
            }

            break;

        default:
            break;
    }

    return 1;
}

// OFFSET: 0x00805640/0x14051AAD0
void CGlueMgr::Initialize() {
    CGlueMgr::m_initialized = 1;

    int32_t readTOS = g_readTOSVar->m_intValue;
    CGlueMgr::m_acceptedTOS = readTOS == 1;

    if (readTOS < 0) {
        CGlueMgr::m_rereadTOS = 1;
    }

    int32_t readEULA = g_readEULAVar->m_intValue;
    CGlueMgr::m_acceptedEULA = readEULA == 1;

    if (readEULA < 0) {
        CGlueMgr::m_rereadEULA = 1;
    }

    int32_t readTerminationWithoutNotice = g_readTerminationWithoutNoticeVar->m_intValue;
    CGlueMgr::m_acceptedTerminationWithoutNotice = readTerminationWithoutNotice == 1;

    if (readTerminationWithoutNotice < 0) {
        CGlueMgr::m_rereadTerminationWithoutNotice = 1;
    }

    int32_t readScanning = g_readScanningVar->m_intValue;
    CGlueMgr::m_acceptedScanning = readScanning == 1;

    if (readScanning < 0) {
        CGlueMgr::m_rereadScanning = 1;
    }

    int32_t readContest = g_readContestVar->m_intValue;
    CGlueMgr::m_acceptedContest = readContest == 1;

    if (readContest < 0) {
        CGlueMgr::m_rereadContest = 1;
    }

    CRect rect = { 0.0f, 0.0f, 0.0f, 0.0f };
    g_theGxDevicePtr->CapsWindowSize(rect);

    CSizeEvent event;
    event.m_data.m_w = static_cast<int32_t>(rect.maxX - rect.minX);
    event.m_data.m_h = static_cast<int32_t>(rect.maxY - rect.minY);
    CGlueMgr::HandleDisplaySizeChanged(event);

    FrameXML_RegisterFactory("ModelFFX", CSimpleModelFFX::Create, false);
    CGlueMgr::Resume();
    CRealmList::Initialize();
    EventRegisterEx(EVENT_ID_IDLE, CGlueMgr::Idle, NULL, 0.0f);
    CGLCD::Initialize();
    CKBPage::Initialize();
    CKBSystem::Initialize();

    CGlueMgr::m_changedOptionWarnings.Clear();
    CGlueMgr::m_numChangedOptionWarnings = 0;
    CGlueMgr::m_changedOptionWarnings.SetCount(16);

    RequestLatestAgreements();
    AccountDataInitializeBasicSystem();
    CGlueMgr::PreloadCharacterModels();
}

// OFFSET: 0x00805840
void CGlueMgr::Shutdown() {
    while (CGlueMgr::m_pendingServerAlert || CGlueMgr::m_latestAgreements.m_data.Count()) {
        if (CGlueMgr::m_latestAgreements.m_data.Count()) {
            CGlueMgr::m_latestAgreements.m_data.Clear();
        }

        OsSleep(1);
    }

    AccountDataShutdownBasicSystem();
    CKBPage::Shutdown();
    CKBSystem::Shutdown();
    CRealmList::Shutdown();
    CCharacterSelection::m_enterWorldIndex = -1;
    CCharacterSelection::Shutdown();
    CCharacterCreation::Shutdown();
    CAnimKitManager::UninitSystem();
    CGlueMgr::Suspend();
    CGlueMgr::m_initialized = 0;
    EventUnregister(EVENT_ID_IDLE, CGlueMgr::Idle);
    EventUnregister(EVENT_ID_PAINT, CGlueMgr::Paint);
    s_connectionStatus.Clear();
    CGLCD::Shutdown();

    if (s_connectionLog) {
        SLogClose(s_connectionLog);
    }

    ClientServices::ClearMessageHandler(CMSG_GM_CHARACTER_RESTORE);
}

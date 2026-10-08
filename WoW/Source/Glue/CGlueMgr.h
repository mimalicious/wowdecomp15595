#ifndef WOW_SOURCE_GLUE_CGLUEMGR_H
#define WOW_SOURCE_GLUE_CGLUEMGR_H

#include <cstddef>
#include <cstdint>
#include "WoW/Source/Object/WGUID.h"
#include "Engine/Source/Os/OsURLDownload.h"
#include "Engine/Source/Storm/H/stpl.h"
#include "WoW/Source/Glue/Types.h"
#include "WoW/Source/Net/Types.h"
#include "WoW/Source/WowServices/Types.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/Types.h"

struct CVar;
class SFileImpl;
struct SArchive;
class CDataStore;
struct CHARACTER_CREATE_INFO;
class CHARACTER_INFO;
class CM2Model;
class CharacterSelectionDisplay;
class CSimpleModel;
class CSimpleModelFFX;
class CSimpleTop;
class SThread;
class CAnimKitInst;
class CAnimKitManager;
class CCharacterComponent;
class ClientGenerateRandomCharacterNameResult;

namespace FFX {
    class Effect;
}
struct MipBits;
class CSizeEvent;
class FileDownload;
class NetClient;

struct ConsoleString {
    /* 0000 */ TSLink<ConsoleString> m_link;
    /* 0008 */ char m_text[256];
};

struct LoginErrorURL {
    /* 0000 */ int32_t m_errorCode;
    /* 0004 */ char m_url[32];
};

struct CharacterEmoteSpellStartData {
    /* 0000 */ CSimpleModelFFX* m_modelFrame;
    /* 0004 */ uint32_t m_spellVisualKitID;
};

struct CharacterEmoteKitFinishedData {
    /* 0000 */ CM2Model* m_rightHand;
    /* 0004 */ CM2Model* m_leftHand;
};

struct CharacterHandItemTypes {
    /* 0000 */ int32_t m_offHand;
    /* 0004 */ int32_t m_mainHand;
};

class CGlueMgr {
    public:
    struct Agreements {
        /* 0000 */ TSGrowableArray<char> m_data;
        /* 0010 */ int32_t m_complete;
    };

    static uint8_t m_ffxActive;
    static FFX::Effect* m_deathEffect;
    static FFX::Effect* m_glowEffect;
    static char m_serverAlert[2048];
    static uint32_t m_queueTime[8];
    static int32_t m_queuePosition[8];
    static int32_t m_currentScreen;
    static MipBits* m_cursorMipBits;
    static CSimpleTop* m_simpleTop;
    static int32_t m_reload;
    static int32_t m_initialized;
    static int32_t m_suspended;
    static int32_t m_disconnectPending;
    static int32_t m_reconnect;
    static GLUE_IDLE_STATE m_idleState;
    static GLUE_IDLE_STATE m_lastIdleState;
    static uint32_t m_matrixChallengeCount;
    static char m_accountName[1280];
    static uint32_t m_matrixRemaining;
    static int32_t m_estimatedWaitTime;
    static int32_t m_displayingQueueDialog;
    static CHARACTER_INFO* m_characterInfo;
    static int32_t m_rereadTOS;
    static int32_t m_acceptedTOS;
    static int32_t m_rereadEULA;
    static int32_t m_acceptedEULA;
    static int32_t m_rereadTerminationWithoutNotice;
    static int32_t m_acceptedTerminationWithoutNotice;
    static int32_t m_rereadScanning;
    static int32_t m_acceptedScanning;
    static int32_t m_rereadContest;
    static int32_t m_acceptedContest;
    static uint8_t m_authenticated;
    static bool s_scriptFunctionsLoaded;
    static int32_t m_showingIntroScene;
    static int32_t m_screenWidth;
    static int32_t m_screenHeight;
    static float m_aspect;
    static int32_t m_pendingServerAlert;
    static int32_t m_processServerAlert;
    static uint32_t m_pendingTimerAlert;
    static int32_t m_accountMsgAvailable;
    static uint8_t m_clientKickReason;
    static int32_t m_shownTrialPopUp;
    static uint32_t m_numChangedOptionWarnings;
    static int32_t m_showedDisconnect;
    static uint32_t m_elapsedMs;
    static uint32_t m_timeMs;
    static uint32_t m_expansionArchivesTotalChunks;
    static uint32_t m_expansionArchivesInitChunks;
    static Agreements m_latestAgreements;
    static TSGrowableArray<const char*> m_changedOptionWarnings;
    static int32_t m_loginResult;
    static int32_t m_loginState;
    static int32_t m_lastLoginResult;
    static int32_t m_lastLoginState;
    static uint32_t m_surveyTimer;
    static uint8_t m_executedSurvey;
    static FileDownload* m_surveyDownload;
    static FileDownload* m_patchDownload;
    static float m_patchDownloadProgress;
    static uint32_t m_patchLastUpdate;
    static uint32_t m_patchStartTime;
    static uint8_t* m_surveyData;
    static uint16_t m_surveyDataSize;
    static bool m_deleteLocalPatch;
    static char m_scandllHackName[512];
    static uint8_t m_scandllOkayToLogIn;
    static const char* m_scandllResult;
    static uint8_t m_scandllShouldBlockPlaying;
    static volatile long m_scandllStatus;
    static SThread m_scandllThread;

    static bool AreScriptFunctionsLoaded();
    static void InitializeFFX();
    static void SetIdleState(GLUE_IDLE_STATE state);
    static void UpdateWaitQueue(int32_t position);
    static void AcceptTOS();
    static void AcceptEULA();
    static void AcceptTerminationWithoutNotice();
    static void AcceptScanning();
    static void AcceptContest();
    static void LoginServerLogin(const char* username, char* password);
    static void LoginEnterPIN(int32_t count, uint8_t* digits);
    static void LoginEnterMatrix(uint8_t value);
    static void LoginRevertMatrix();
    static void LoginCommitMatrix();
    static bool LoginGetMatrixCoordinates(uint32_t* x, uint32_t* y);
    static void LoginEnterToken(const char* token);
    static WOWGUID GetActive();
    static void DisplayLoginStatus();
    static void CreateCharacter(const CHARACTER_CREATE_INFO* info);
    static void DeleteCharacter(WOWGUID guid);
    static void RequestRandomCharacterName(uint8_t raceID, uint8_t sexID);
    static int32_t OnKickReasonMsg(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
    static int32_t OnRealmSplitMsg(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
    static void CancelRealmListQuery();
    static void RealmListDialogCancelled();
    static void SetScreen(int32_t screen);
    static void UpdateCurrentScreen(const char* screen);
    static void QuitGame();
    static void QuitGameAndRunLauncher();
    static void GetRealmList(int32_t showDialog);
    static void GetCharacterList();
    static void ResetQueueState();
    static void DownloadFeed(const uint8_t* data, uint32_t size);
    static void AttachmentSequenceCallback(CM2Model* model);
    static uint32_t GetTotemDisplayId(int32_t classID, int32_t raceID);
    static int32_t HandleBattlenetDisconnect();
    static void PollUserSurvey();
    static void PollCharacterList(const char* text, int32_t complete, int32_t result, int32_t errorCode);
    static void PollRealmList(const char* text, int32_t complete, int32_t result);
    static void PollCreateCharacter(const char* text, int32_t complete, int32_t result);
    static void PollDeleteCharacter(const char* text, int32_t complete, int32_t result);
    static int32_t Paint(const void* data, void* param);
    static void HandleScreenshot(int32_t success);
    static int32_t ShowChangedOptionWarnings();
    static int32_t CCommand_ExportInterfaceFiles(const char* command, const char* arguments);
    static void PreloadCharacterModels();
    static bool ServerAlertURLCallback(void* param, const char* data, uint32_t size, bool done, HTTPError::ErrorCode error);
    static void ProcessAgreements(CGlueMgr::Agreements* agreements);
    static int32_t HandleDisplaySizeChanged(const CSizeEvent& event);
    static void Suspend();
    static void SetCurrentAccount(const char* accountName);
    static void ChangeRealm(REALM_INFO* realm);
    static int32_t RenameCharacter(WOWGUID guid, const char* name);
    static void CustomizeCharacter(WOWGUID guid, const CHARACTER_CREATE_INFO* info);
    static void FactionChange(WOWGUID guid, const CHARACTER_CREATE_INFO* info);
    static void RaceChange(WOWGUID guid, const CHARACTER_CREATE_INFO* info);
    static bool OnRandomCharacterNameResult(ClientGenerateRandomCharacterNameResult* msg);
    static int32_t GetSelectedCharInfo(CHARACTER_INFO* info);
    static int32_t OnCharCustomizeResult(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
    static int32_t OnCharFactionChangeResult(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
    static void StatusDialogClick();
    static void CancelLogin();
    static void SetUIDepth(float depth);
    static void StopCharacterEmoteKits(CAnimKitManager* manager);
    static void CharacterEmoteKitFinished(CAnimKitInst* inst, uint64_t param);
    static CharacterHandItemTypes AddItemsFromCharInfo(const CHARACTER_INFO* info, CCharacterComponent* component, void (*guildCallback)(const WOWGUID& guid, const WOWGUID& guildGUID, void* param, bool success), void (*loadedCallback)(CM2Model* model, void* arg));
    static void ProcessAttachment(CSimpleModel* modelFrame, CM2Model* model, int32_t effectID, uint32_t attachID, int32_t sequenceCallback);
    static void PollLoginServerLogin();
    static void PollEnterWorld();
    static void ApplySpellVisualKit(CSimpleModelFFX* modelFrame, CM2Model* model, int32_t spellVisualKitID, int32_t sequenceCallback);
    static int32_t DeclineCharacter(WOWGUID guid, const char** names);
    static void EnterWorld();
    static void CharacterEmoteSpellStart(CAnimKitManager* manager, uint64_t param);
    static void AddChangedOptionWarning(const char* warning);
    static bool AgreementsURLCallback(void* param, const char* data, uint32_t size, bool done, HTTPError::ErrorCode error);
    static int32_t OnCharRenameResult(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
    static int32_t OnCharDeclineResult(void* param, NETMESSAGE msgId, uint32_t time, CDataStore* msg);
    static void PlayCharacterEmoteKits(int32_t classID, int32_t raceID, int32_t screen, const CharacterHandItemTypes* handItems, CAnimKitManager* manager, CSimpleModelFFX* modelFrame);
    static void AcceptChangedOptionWarnings();
    static void LogConnectionStatus(const char* format, ...);
    static void SetLoginStateAndResult(LOGIN_STATE state, LOGIN_RESULT result, const char* addressString, const char* stateString, const char* resultString, uint16_t flags);
    static void Resume();
    static void PollAccountLogin(int32_t errorCode, const char* text, int32_t complete, int32_t result, WOWCS_OPS op);
    static int32_t NetDisconnectHandler(const void* data, void* param);
    static int32_t Idle(const void* data, void* param);
    static void Initialize();
    static void Shutdown();

    static size_t GetPatchFolder(char* buffer, uint32_t size);
    static bool IsScanDLLFinished();
    static void PatchDownloadApply();
    static void PatchDownloadCancel();
    static bool PatchDownloadDeleteCommand(const char* path);
    static bool PatchDownloadExecuteCommand(const char* path, const char* arguments);
    static bool PatchDownloadExecutePrepatch(SArchive* archive);
    static bool PatchDownloadExtractCommand(SArchive* archive, const char* name);
    static void PatchDownloadFeed(const uint8_t* data, uint32_t size);
    static void PatchDownloadIdle();
    static void PatchDownloadStart();
    static void PatchFailed(int32_t reason, uint32_t data);
    static void ScanDLLContinueAnyway();
    static void ScanDLLFinished();
    static void ScanDLLStart(const char* versionURL, const char* dllURL);
    static void SurveyDownloadCancel();
    static void SurveyDownloadEnd();
    static void SurveyDownloadFeed(const uint8_t* data, uint32_t size);
    static bool SurveyDownloadIdle();
    static void SurveyDownloadStart();
    static bool SurveyExecute();
    static void SurveySendResults();
};

extern const uint8_t InterfaceKey[256];
extern int32_t g_useAnimKitsOnGlueScreen;
extern const char* s_screenNames[NUM_GLUE_SCREENS];

CharacterSelectionDisplay* GetCharacterInfo(int32_t index);
void ProcessAgreement(SFileImpl* file, const char* name, CVar* cvar);

#endif

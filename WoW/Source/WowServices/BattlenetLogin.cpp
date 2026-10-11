#include "BattlenetLogin.h"
#include <cctype>
#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "Engine/Source/Base/CDataStore.h"
#include "Engine/Source/BlizzardCore/SFile2-Core.h"
#include "Engine/Source/BlizzardCore/Source/System/Version/Version.h"
#include "Engine/Source/ConsoleBase/IConsole.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "Engine/Source/FrameXML/XMLTree.h"
#include "Engine/Source/Os/OsFile.h"
#include "Engine/Source/Os/OsSecureRandom.h"
#include "Engine/Source/Os/OsTcp.h"
#include "Engine/Source/Os/SockAddr.h"
#include "Engine/Source/Os/TimeManager.h"
#include "Engine/Source/Storm/H/storm.h"
#include "WoW/Source/Battlenet/Tumor/TumorManager.h"
#include "WoW/Source/Client.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/Glue/CGlueMgr.h"
#include "WoW/Source/Ui/BattlenetUI.h"
#include "WoW/Source/Ui/GameUI.h"
#include "WoW/Source/WowServices/InstallLocation.h"
#include "WoW/Source/WowServices/LoginResponse.h"

// STATIC: 0x00CC8C4C
BattlenetAllocator s_battlenetAllocator;

// STATIC: 0x00DD9FF8
Battlenet::TumorManager* g_tumorManager;

// STATIC: 0x00DD9FFC
bool BattlenetLogin::s_monolithicComponent;

// STATIC: 0x00DD9FFD
bool BattlenetLogin::s_toolPatch;

// STATIC: 0x00DDA018
char BattlenetLogin::s_monolithicPlatform[5];

// PROVEN: 0fdc9e0a IDENTICAL/NONE
// OFFSET: 0x006273B0/0x1402B9400
void* BattlenetAllocator::Alloc(uint32_t size) {
    return SMemAlloc(size, __FILE__, __LINE__, 0);
}

// OFFSET: 0x006273D0/0x1402B9420
void* BattlenetAllocator::Alloc(uint32_t size, const char* name) {
    return SMemAlloc(size, name && *name ? name : __FILE__, __LINE__, 0);
}

// PROVEN: 71fe10eb IDENTICAL/NONE
// OFFSET: 0x00627400/0x1402B9450
void BattlenetAllocator::Free(void* memory) {
    SMemFree(memory, __FILE__, __LINE__, 0);
}

// OFFSET: #0x008C5180
bool BattlenetApplication::Send(uint32_t connection, const void* data, uint32_t size) {
    if (connection != this->m_connection) {
        SErrDisplayAppFatalMessage("", "", 0);
    }

    return this->m_wowConnection->SendRaw(static_cast<uint8_t*>(const_cast<void*>(data)), size);
}

// OFFSET: #0x008C51C0
uint64_t BattlenetApplication::GetCurrentTick() const {
    return OsGetAsyncTimeMs();
}

// OFFSET: 0x00627450
BattlenetApplication::~BattlenetApplication() {
    if (this->m_wowConnection) {
        this->m_wowConnection->Disconnect();
        this->m_wowConnection->SetResponse(NULL, false);
        this->m_wowConnection->Release();
        this->m_wowConnection = NULL;
    }
}

// PROVEN: 3dcc8d67 IDENTICAL/NONE
// OFFSET: 0x00627510
uint8_t BattlenetLogin::GetCharacterCount(uint32_t region, uint32_t battlegroup, uint32_t id) {
    RealmCharacterCount* counts = this->m_realmCharacterCounts;

    if (!counts) {
        return 0;
    }

    for (uint32_t i = 0; i < this->m_realmCharacterCountCount; i++, counts++) {
        if (counts->m_region == region && counts->m_battlegroup == battlegroup && counts->m_id == id) {
            return static_cast<uint8_t>(counts->m_count);
        }
    }

    return 0;
}

// OFFSET: 0x00627580/0x1402B95A0
bool BattlenetLogin::Connected(const NETADDR& address) {
    char text[256];
    char addressString[32];

    this->m_loggedOn = true;

    SStrPrintf(text, sizeof(text), "Connected to %s", OsNetAddrToStr(&address, addressString, sizeof(addressString)));
    ConsoleWrite(text, DEFAULT_COLOR);

    if (!g_tumorManager || !this->m_application) {
        SErrDisplayAppFatalMessage("", "", 0);
    }

    Battlenet::SystemClient& client = g_tumorManager->m_tumor;
    client.Connected(this->m_application->m_connection, this->m_application->m_localAddress);

    return true;
}

// OFFSET: 0x00627600/0x1402B9630
bool BattlenetLogin::Disconnected() {
    if (!g_tumorManager || !this->m_application) {
        SErrDisplayAppFatalMessage("", "", 0);
    }

    Battlenet::SystemClient& client = g_tumorManager->m_tumor;
    client.Disconnected(this->m_application->m_connection);

    return false;
}

// OFFSET: #0x008C53B0
void BattlenetLogin::LogonResultBN(int32_t result) {
    this->m_logonResult = result;

    switch (result) {
        case 0:
            this->m_loginResponse->UpdateLoginStatus(LOGIN_STATE_AUTHENTICATED, LOGIN_OK, NULL, 0);
            break;

        case 103:
            this->m_loginResponse->UpdateLoginStatus(LOGIN_STATE_FAILED, LOGIN_BAD_SERVER_PROOF, NULL, 0);
            break;

        case 104:
        case 108:
        case 116:
        case 201:
            this->m_loginResponse->UpdateLoginStatus(LOGIN_STATE_FAILED, LOGIN_UNKNOWN_ACCOUNT, NULL, 0);
            break;

        case 105:
        case 106:
            this->m_loginResponse->UpdateLoginStatus(LOGIN_STATE_FAILED, LOGIN_DISCONNECTED, NULL, 0);
            break;

        case 107:
            this->m_loginResponse->UpdateLoginStatus(LOGIN_STATE_FAILED, LOGIN_NO_GAME_ACCOUNT, NULL, 0);
            break;

        case 109:
        case 110:
        case 111:
        case 112:
            this->m_loginResponse->UpdateLoginStatus(LOGIN_STATE_FAILED, LOGIN_BADVERSION, NULL, 0);
            break;

        case 113:
        case 200:
            this->m_loginResponse->UpdateLoginStatus(LOGIN_STATE_FAILED, LOGIN_AUTH_OUTAGE, NULL, 0);
            break;

        case 115:
        case 205:
            this->m_loginResponse->UpdateLoginStatus(LOGIN_STATE_FAILED, LOGIN_ALREADYONLINE, NULL, 0);
            break;

        case 123:
            this->m_loginResponse->UpdateLoginStatus(LOGIN_STATE_FAILED, LOGIN_TOO_FAST, NULL, 0);
            break;

        case 124:
            this->m_loginResponse->UpdateLoginStatus(LOGIN_STATE_FAILED, LOGIN_USE_GRUNT, NULL, 0);
            break;

        case 140:
            this->m_loginResponse->UpdateLoginStatus(LOGIN_STATE_FAILED, LOGIN_NO_GAME_ACCOUNTS_IN_REGION, NULL, 0);
            break;

        case 141:
            this->m_loginResponse->UpdateLoginStatus(LOGIN_STATE_FAILED, LOGIN_ACCOUNT_LOCKED, NULL, 0);
            break;

        case 202:
            this->m_loginResponse->UpdateLoginStatus(LOGIN_STATE_FAILED, LOGIN_BANNED, NULL, 0);
            break;

        case 203:
            this->m_loginResponse->UpdateLoginStatus(LOGIN_STATE_FAILED, LOGIN_SUSPENDED, NULL, 0);
            break;

        case 204:
        case 212:
            this->m_loginResponse->UpdateLoginStatus(LOGIN_STATE_FAILED, LOGIN_GAME_ACCOUNT_LOCKED, NULL, 0);
            break;

        case 206:
            this->m_loginResponse->UpdateLoginStatus(LOGIN_STATE_FAILED, LOGIN_NOTIME, NULL, 0);
            break;

        case 207:
        case 208:
            this->m_loginResponse->UpdateLoginStatus(LOGIN_STATE_FAILED, LOGIN_EXPIRED, NULL, 0);
            break;

        case 209:
            this->m_loginResponse->UpdateLoginStatus(LOGIN_STATE_FAILED, LOGIN_PARENTALCONTROL, NULL, 0);
            break;

        case 210:
            this->m_loginResponse->UpdateLoginStatus(LOGIN_STATE_FAILED, LOGIN_TRIAL_EXPIRED, NULL, 0);
            break;

        case 211:
            this->m_loginResponse->UpdateLoginStatus(LOGIN_STATE_FAILED, LOGIN_ANTI_INDULGENCE, NULL, 0);
            break;

        default:
            this->m_loginResponse->UpdateLoginStatus(LOGIN_STATE_FAILED, LOGIN_FAILED, NULL, 0);
            break;
    }

    this->m_surveyPending = false;
}

// OFFSET: 0x006276D0/0x1402B96E0
static void GetComponentVersion(const char* name, uint32_t& version, const char* data, uint32_t size) {
    XMLTree* tree = XMLTree_Load(data, size);

    for (XMLNode* node = XMLTree_GetRoot(tree)->m_child; node; node = node->m_next) {
        if (!SStrCmpI(node->m_name.m_str, "component", STORM_MAX_STR)) {
            const char* componentName = node->GetAttributeByName("name");
            const char* componentVersion = node->GetAttributeByName("version");

            if (componentName && componentVersion && !SStrCmpI(componentName, name, STORM_MAX_STR)) {
                version = SStrToInt(componentVersion);
            }
        }
    }

    XMLTree_Free(tree);
}

// OFFSET: 0x00627780/0x1402B97B0
static uint32_t GetWowDataVersion() {
    uint32_t version = 15595;
    void* data;
    uint32_t size;

    if (!SFileLoadFileEx(NULL, "component.wow-data.txt", &data, &size, 0, 0, NULL) || !data) {
        return 15595;
    }

    GetComponentVersion("wow-data", version, static_cast<const char*>(data), size);
    SMemFree(data, __FILE__, __LINE__, 0);

    return version;
}

// OFFSET: 0x006277F0
static uint32_t GetWowLangVersion(const char* locale) {
    char fileName[32];
    char componentName[32];
    uint32_t version = 15595;

    if (strcmp(locale, s_localeCode)) {
        return 15595;
    }

    sprintf(fileName, "component.wow-%s.txt", locale);

    void* data;
    uint32_t size;

    if (!SFile::ReadFile(NULL, fileName, &data, &size, 0, 0) || !data) {
        return 15595;
    }

    sprintf(componentName, "wow-%s", locale);
    GetComponentVersion(componentName, version, static_cast<const char*>(data), size);
    SMemFree(data, __FILE__, __LINE__, 0);

    return version;
}

// OFFSET: 0x006283E0/0x1402BA470
void BattlenetLogin::SetAuthModuleMatrixInfo(const char* info, const char* challenges) {
    this->m_matrixWidth = SStrToUnsigned(info);

    const char* cursor = SStrChr(info, ',');

    if (!cursor) {
        SErrDisplayAppFatalMessage("", "", 0);
    }

    cursor++;
    this->m_matrixHeight = SStrToUnsigned(cursor);

    cursor = SStrChr(cursor, ',');

    if (!cursor) {
        SErrDisplayAppFatalMessage("", "", 0);
    }

    cursor++;
    this->m_matrixMinDigitCount = SStrToUnsigned(cursor);

    cursor = SStrChr(cursor, ',');

    if (!cursor) {
        SErrDisplayAppFatalMessage("", "", 0);
    }

    cursor++;
    this->m_matrixMaxDigitCount = SStrToUnsigned(cursor);

    cursor = SStrChr(cursor, ',');

    if (!cursor) {
        SErrDisplayAppFatalMessage("", "", 0);
    }

    cursor++;
    this->m_matrixFlipCoords = SStrToUnsigned(cursor) != 0;

    cursor = SStrChr(cursor, ',');

    if (!cursor) {
        SErrDisplayAppFatalMessage("", "", 0);
    }

    cursor++;
    this->m_matrixChallengeCount = SStrToUnsigned(cursor);

    if (SStrChr(cursor, ',')) {
        SErrDisplayAppFatalMessage("", "", 0);
    }

    if (!this->m_matrixWidth || !this->m_matrixHeight || !this->m_matrixMinDigitCount || !this->m_matrixMaxDigitCount || !this->m_matrixChallengeCount) {
        SErrDisplayAppFatalMessage("", "", 0);
    }

    if (this->m_matrixData) {
        delete[] this->m_matrixData;
    }

    this->m_matrixData = new (__FILE__, __LINE__) uint32_t[this->m_matrixChallengeCount];

    for (uint32_t i = 0; i < this->m_matrixChallengeCount; i++) {
        if (!challenges) {
            SErrDisplayAppFatalMessage("", "", 0);
        }

        if (i) {
            challenges++;
        }

        this->m_matrixData[i] = SStrToUnsigned(challenges);
        challenges = SStrChr(challenges, ',');
    }

    if (challenges) {
        SErrDisplayAppFatalMessage("", "", 0);
    }

    this->m_matrixEnabled = true;
    this->m_loginResponse->UpdateLoginStatus(LOGIN_STATE_MATRIX, LOGIN_OK, NULL, 0);
}

// OFFSET: #0x008C61E0
void BattlenetLogin::SetAuthModulePinInfo(const char* keyMap) {
    if (!keyMap || SStrLen(keyMap) != 10) {
        SErrDisplayAppFatalMessage("", "", 0);
    }

    for (uint32_t i = 0; i < 10; i++) {
        if (!isdigit(static_cast<uint8_t>(keyMap[i]))) {
            SErrDisplayAppFatalMessage("", "", 0);
        }

        this->unk_105D[1 + i] = keyMap[i] - '0';
    }

    this->unk_105D[0] = 1;
    this->m_loginResponse->UpdateLoginStatus(LOGIN_STATE_PIN, LOGIN_OK, NULL, 0);
}

// OFFSET: 0x00628670/0x1402BA790
void BattlenetLogin::SetTokenInfo(bool enabled, uint8_t required) {
    if (enabled) {
        this->m_tokenRequired = true;
        this->unk_10D5 = required;
        this->m_loginResponse->UpdateLoginStatus(LOGIN_STATE_TOKEN, LOGIN_OK, NULL, 0);
    }
}

// PROVEN: 16bc9056 IDENTICAL/NONE
// OFFSET: 0x00627690/0x1402B96A0
int32_t BattlenetLogin::GetOutOfBandChallengeTimeLeft() {
    int32_t left = this->m_outOfBandTimeout - static_cast<int32_t>(OsGetAsyncTimeMs() - this->m_outOfBandStart) / 1000;

    return left <= 0 ? 0 : left;
}

// PROVEN: 6d03b359 IDENTICAL/NONE
// OFFSET: 0x006279B0
void BattlenetLogin::SetAuthModuleOutOfBandInfo(uint32_t type, const char* title, const char* text, int32_t timeout) {
    this->m_outOfBandType = type;
    this->m_outOfBandTitle = static_cast<char*>(SMemAlloc(strlen(title) + 1, __FILE__, __LINE__, 0));
    strcpy(this->m_outOfBandTitle, title);
    this->m_outOfBandText = static_cast<char*>(SMemAlloc(strlen(text) + 1, __FILE__, __LINE__, 0));
    strcpy(this->m_outOfBandText, text);
    this->m_outOfBandTimeout = timeout;
}

// PROVEN: ade29f11 IDENTICAL/NONE
// OFFSET: 0x00627A50/0x1402B9B70
void BattlenetLogin::ClearAuthModuleOutOfBandInfo() {
    SMemFree(this->m_outOfBandTitle, __FILE__, __LINE__, 0);
    this->m_outOfBandTitle = NULL;
    SMemFree(this->m_outOfBandText, __FILE__, __LINE__, 0);
    this->m_outOfBandText = NULL;
}

// PROVEN: 4dfddb44 IDENTICAL/NONE
// OFFSET: 0x00627AA0/0x1402B9BD0
const char* BattlenetLogin::GetAccountName() {
    return this->m_accountName;
}

// PROVEN: b5f242c6 IDENTICAL/NONE
// OFFSET: 0x0062AEA0/0x1402BDFE0
uint32_t BattlenetLogin::GetOutOfBandChallengeType() {
    return this->m_outOfBandType;
}

// PROVEN: f60428ac IDENTICAL/NONE
// OFFSET: 0x0062AEB0/0x1402BDFF0
const char* BattlenetLogin::GetOutOfBandChallengeTitle() {
    return this->m_outOfBandTitle;
}

// PROVEN: d0c3a397 IDENTICAL/NONE
// OFFSET: 0x0062AEC0/0x1402BE000
const char* BattlenetLogin::GetOutOfBandChallengeText() {
    return this->m_outOfBandText;
}

// OFFSET: 0x00627AB0/0x1402B9BE0
static bool IsInOrEnteringWorld() {
    return CGGameUI::s_inWorld || CGlueMgr::m_idleState == IDLE_ENTER_WORLD;
}

// PROVEN: 07b81ff1 IDENTICAL/NONE
// OFFSET: 0x00627AD0/0x1402B9C00
void BattlenetLogin::LogConnectionStatus(const char* format, ...) {
    char buffer[2048];

    if (format && *format) {
        va_list args;
        va_start(args, format);
        SStrVPrintf(buffer, sizeof(buffer), format, args);
        CGlueMgr::LogConnectionStatus(buffer);
    }
}

// OFFSET: #0x008C6980
void BattlenetApplication::Disconnect(uint32_t connection) {
    if (connection != this->m_connection) {
        SErrDisplayAppFatalMessage("", "", 0);
    }

    if (this->m_wowConnection) {
        this->m_wowConnection->Disconnect();
        BattlenetLogin::LogConnectionStatus("Disconnected from %x", this->m_wowConnection->m_connectAddress.m_ipv4);
    }
}

// OFFSET: 0x00628860
BattlenetApplication::BattlenetApplication() {
    this->m_eventId = Battlenet::Event::APP;
    this->m_login = NULL;
    this->m_wowConnection = NULL;
    this->m_connection = 0;
}

// OFFSET: 0x00628970/0x1402BAA80
bool BattlenetLogin::CantConnect() {
    ConsoleWrite("BattlenetLogin was unable to connect to Sunken", DEFAULT_COLOR);
    this->m_loginResponse->UpdateLoginStatus(LOGIN_STATE_FAILED, LOGIN_SERVER_DOWN, NULL, 0);
    this->m_logonStatus = 1;
    this->LogonResultBN(2);
    this->m_loggedOn = false;
    this->m_online = false;

    return false;
}

// OFFSET: 0x006286E0/0x1402BA810
const char* BattlenetLogin::GetPatchInstruction(uint32_t index) {
    if (index < this->m_patchInstructions.m_count) {
        return this->m_patchInstructions.m_data[index].m_patch;
    }

    return NULL;
}

// OFFSET: 0x006288A0/0x1402BA970
uint32_t BattlenetApplication::Connect(const Battlenet::IP4::AddressPort& address, const uint32_t& tendrilId) {
    char buffer[64];

    this->m_connection = OsGetAsyncTimeMs();
    this->m_remoteAddress = address;

    if (!this->m_wowConnection) {
        this->m_wowConnection = new (__FILE__, __LINE__) WowConnection(this, NULL);
        this->m_wowConnection->SetType(WOWC_TYPE_STREAM);
    }

    sprintf(buffer, "%d.%d.%d.%d:%d", address.m_address.m_data[0], address.m_address.m_data[1], address.m_address.m_data[2], address.m_address.m_data[3], address.m_port.m_data[1] + (address.m_port.m_data[0] << 8));
    BattlenetLogin::LogConnectionStatus("Connecting to %s", buffer);
    this->m_wowConnection->Connect(buffer, 5000);

    return this->m_connection;
}

// PROVEN: c1435f50 IDENTICAL/NONE
// OFFSET: 0x00904350
uint32_t BattlenetApplication::OpenDatagram(int32_t port, const uint32_t& tendrilId) {
    SErrDisplayAppFatalMessage("", "", 0);
}

// PROVEN: 213ab6a2 IDENTICAL/NONE
// OFFSET: 0x00904350
void BattlenetApplication::CloseDatagram(uint32_t connection) {
    SErrDisplayAppFatalMessage("", "", 0);
}

// PROVEN: 24de9826 IDENTICAL/NONE
// OFFSET: 0x00904350
bool BattlenetApplication::SendDatagram(uint32_t connection, const void* data, uint32_t size, const Battlenet::IP4::AddressPort& address) {
    SErrDisplayAppFatalMessage("", "", 0);
}

// PROVEN: 011ff2c9 IDENTICAL/NONE
// OFFSET: 0x00904350
uint64_t BattlenetApplication::GetCurrentTime() const {
    SErrDisplayAppFatalMessage("", "", 0);
}

// PROVEN: 2aca7dfc IDENTICAL/DIFFERENT
// OFFSET: 0x00841AE0/0x140211B90
void BattlenetApplication::LogLevel(const char* message, int32_t level) {
}

// PROVEN: 8684c62b IDENTICAL/DIFFERENT
// OFFSET: 0x00A9AA30/0x1402123D0
Battlenet::ApplicationFile* BattlenetApplication::OpenApplicationFile(const char* name) {
    return NULL;
}

// PROVEN: cada2529 IDENTICAL/DIFFERENT
// OFFSET: 0x007AAB20/0x1402B9E40
void BattlenetApplication::CloseApplicationFile(Battlenet::ApplicationFile* file) {
}

// PROVEN: 5b409c1d IDENTICAL/DIFFERENT
// OFFSET: 0x00627990/0x140693B10
void BattlenetApplication::WCMessageReady(WowConnection* connection, uint32_t timeStamp, CDataStore* msg) {
}

// OFFSET: 0x00628C40/0x1402BAE90
void BattlenetLogin::HandleUpdateInvoke(const Battlenet::Event::Update::Invoke* invoke) {
    char program[5];
    char component[5];

    this->m_patchInstructionLock.Enter();

    strcpy(this->m_patchInstructions.New()->m_patch, invoke->m_patch.m_instructions.m_data);

    invoke->m_patch.m_programId.ToString(program, sizeof(program));
    invoke->m_patch.m_component.ToString(component, sizeof(component));

    if (invoke->m_patch.m_programId.m_value == BSN::FourCC("Tool").m_value) {
        BattlenetLogin::s_toolPatch = true;
        this->m_patchInstructions.Clear();
    }

    if (invoke->m_patch.m_programId.m_value == BSN::FourCC("Tool").m_value || !BattlenetLogin::s_toolPatch) {
        PatchInstruction* instruction = this->m_patchInstructions.New();
        SStrCopy(instruction->m_patch, invoke->m_patch.m_instructions.m_data, sizeof(instruction->m_patch));
        BattlenetLogin::LogConnectionStatus("program=%s component=%s patch=%s", program, component, instruction->m_patch);
    }

    this->m_patchInstructionLock.Leave();

    if (!invoke->m_more) {
        SStrCopy(this->m_fileName, "Patch", sizeof(this->m_fileName));
        this->m_fileSize = 0;
        this->m_loginResponse->UpdateLoginStatus(LOGIN_STATE_DOWNLOADFILE, LOGIN_OK, NULL, 0);
    }
}

// OFFSET: 0x006274B0/0x1402B9510
void BattlenetLogin::BuildEmptyRealmListResult() {
    CDataStore* msg = new (__FILE__, __LINE__) CDataStore();
    this->RealmListResult(msg);
    delete msg;
}

// OFFSET: 0x00628E60/0x1402BB0F0
void BattlenetLogin::BuildRealmListResult() {
    CDataStore* msg = new (__FILE__, __LINE__) CDataStore();
    uint16_t count = 0;

    msg->Put(count);

    this->m_realmLock.Enter();

    for (BN_REALM* realm = this->m_realms.Head(); realm; realm = this->m_realms.Next(realm)) {
        REALM_INFO& info = realm->m_info;

        msg->Put(info.m_type);
        msg->Put(info.m_otherFlags);
        msg->Put(info.m_flags);
        msg->PutString(info.m_realmName);

        char address[64];
        SStrPrintf(address, "%s", SockAddrString(info.m_addresses[0], info.m_ports[0]).m_str);
        msg->PutString(address);

        msg->Put(info.m_population);
        msg->Put(info.m_numCharacters);
        msg->Put(info.m_category);
        msg->Put(info.m_regionId);
        msg->Put(info.m_battlegroupId);
        msg->Put(info.m_realmId);
        msg->Put(info.m_build);

        if (info.m_flags & 0x4) {
            msg->Put(info.m_versionMajor);
            msg->Put(info.m_versionMinor);
            msg->Put(info.m_versionRevision);
            msg->Put(info.m_versionBuild);
        }

        count++;
    }

    this->m_realmLock.Leave();

    msg->Put(static_cast<uint16_t>(0));
    msg->Set(0, count);
    msg->Finalize();

    this->RealmListResult(msg);

    delete msg;
}

// OFFSET: 0x00629050/0x1402BB330
void BattlenetLogin::JoinRealm(uint32_t region, uint32_t battlegroup, uint32_t id) {
    this->m_joinRealmId = id;

    Battlenet::Event::Creep::Invoke* invoke = new (__FILE__, __LINE__) Battlenet::Event::Creep::Invoke();
    Battlenet::Event::Creep::Invoke::WoW* wow = invoke->Set<6, Battlenet::Event::Creep::Invoke::WoW>();
    Battlenet::Event::Creep::Invoke::WoW::JoinRealm* join = wow->Set<1, Battlenet::Event::Creep::Invoke::WoW::JoinRealm>();

    if (region || battlegroup || id) {
        join->m_realmId.m_exists = true;
        join->m_realmId.m_value.m_region = region;
        join->m_realmId.m_value.m_site = battlegroup;
        join->m_realmId.m_value.m_realm = id;
    } else {
        join->m_realmId.m_exists = false;
    }

    if (g_tumorManager) {
        g_tumorManager->SendEvent<Battlenet::Event::Creep::Invoke, Protocol>(Battlenet::Event::CREEP, *invoke);
    }

    delete invoke;
}

// OFFSET: 0x00629720/0x1402BBD40
void BattlenetLogin::GetLogonMethod() {
    if (!this->m_password) {
        return;
    }

    Battlenet::Event::Creep::Invoke* invoke = new (__FILE__, __LINE__) Battlenet::Event::Creep::Invoke();
    Battlenet::Event::Creep::Invoke::Authentication* authentication = invoke->Set<0, Battlenet::Event::Creep::Invoke::Authentication>();
    Battlenet::Event::Creep::Invoke::Authentication::Module* module = authentication->Set<2, Battlenet::Event::Creep::Invoke::Authentication::Module>();
    module->m_token = this->m_moduleId;
    module->m_function = Battlenet::Event::AuthModule::RequestPassword;

    Battlenet::Event::AuthModule::Parameter* parameter = module->m_parameters.New();
    *parameter->Set<0, BSN::String<256> >() = this->m_password;

    memset(this->m_password, 0, SStrLen(this->m_password));
    SMemFree(this->m_password, __FILE__, __LINE__, 0);
    this->m_password = NULL;

    if (g_tumorManager) {
        g_tumorManager->SendEvent<Battlenet::Event::Creep::Invoke, Protocol>(Battlenet::Event::CREEP, *invoke);
    }

    delete invoke;
}

// OFFSET: 0x00629130/0x1402BB4A0
void BattlenetLogin::Logoff() {
    Battlenet::Event::Creep::Invoke* invoke = new (__FILE__, __LINE__) Battlenet::Event::Creep::Invoke();
    Battlenet::Event::Creep::Invoke::Service* service = invoke->Set<9, Battlenet::Event::Creep::Invoke::Service>();
    service->Set<0, Battlenet::Event::Creep::Invoke::Service::Disconnect>();

    if (g_tumorManager) {
        g_tumorManager->SendEvent<Battlenet::Event::Creep::Invoke, Protocol>(Battlenet::Event::CREEP, *invoke);
    }

    BattlenetApplication* application = this->m_application;

    if (application->m_wowConnection) {
        application->m_wowConnection->Disconnect();

        SockAddrString address(application->m_wowConnection->m_connectAddress);
        BattlenetLogin::LogConnectionStatus("Client initiated Disconnect from %s", address.m_str);
    }

    delete invoke;
}

// OFFSET: 0x00629200/0x1402BB5F0
Battlenet::TumorManager* BattlenetLogin::CreateTumorManager() {
    if (g_tumorManager) {
        BattlenetUI::Uninit();
    }

    const char* program = "WoW";
    Battlenet::Authentication::Settings settings;

    if (settings.m_seed.m_length < 32) {
        memset(&settings.m_seed.m_data[settings.m_seed.m_length], 0, 32 - settings.m_seed.m_length);
    }

    settings.m_seed.m_length = 32;
    OsSecureRandom(settings.m_seed.m_data, 32);

    settings.m_program = BSN::FourCC("WoW");
    settings.m_platform = BSN::FourCC("Win");
    settings.m_productLocale.m_value = SFile::s_locale;
    settings.m_textLocale.m_value = SFile::s_locale;
    settings.m_cacheOverridePath.m_exists = false;

    BattlenetLogin::LogConnectionStatus("Login program=%s platform=%s locale=%s", "WoW", "Win", s_localeCode);

    if (GetInstallLocation() <= INSTALL_LOCATION_BETA) {
        program = "WoWT";
    }

    if (BattlenetLogin::s_monolithicComponent) {
        BattlenetLogin::s_monolithicComponent = false;
        SStrCopy(BattlenetLogin::s_monolithicPlatform, "Win", sizeof(BattlenetLogin::s_monolithicPlatform));
        SStrCopy(&BattlenetLogin::s_monolithicPlatform[1], &s_localeCode[1], 4);

        Battlenet::Version::Record* record = settings.m_versions.New();
        record->m_programId = BSN::FourCC(program);
        record->m_component = BSN::FourCC(BattlenetLogin::s_monolithicPlatform);
        record->m_version = 15595;
    } else {
        Battlenet::Version::Record* record = settings.m_versions.New();
        record->m_programId = BSN::FourCC(program);
        record->m_component = BSN::FourCC("Win");
        record->m_version = 15595;

        record = settings.m_versions.New();
        record->m_programId = BSN::FourCC("WoW");
        record->m_component = BSN::FourCC("base");
        record->m_version = GetWowDataVersion();

        uint32_t localeCount = 0;

        for (int32_t i = 0; i < 15; i++) {
            if (g_hasIsoLocale[i]) {
                record = settings.m_versions.New();
                record->m_programId = BSN::FourCC("WoW");

                const char* locale = g_localeNames[i];
                record->m_component = BSN::FourCC(locale);
                localeCount++;
                record->m_version = GetWowLangVersion(locale);
            }
        }

        if (!localeCount) {
            record = settings.m_versions.New();
            record->m_programId = BSN::FourCC("WoW");
            record->m_component = BSN::FourCC(s_localeCode);
            record->m_version = GetWowLangVersion(s_localeCode);
        }

        record = settings.m_versions.New();
        record->m_programId = BSN::FourCC("Tool");
        record->m_component = BSN::FourCC("Win");

        uint32_t launcherVersion = 0;
        Blizzard::Version::FileVersion version;

        if (Blizzard::Version::GetFileVersion("Launcher.exe", version)) {
            launcherVersion = version.m_revision;
        }

        record->m_version = launcherVersion;
    }

    this->m_patchInstructionLock.Enter();
    this->m_patchInstructions.Clear();
    this->m_patchInstructionLock.Leave();

    for (uint32_t i = 0; i < settings.m_versions.m_count; i++) {
        char recordProgram[5];
        char recordComponent[5];

        Battlenet::Version::Record& record = settings.m_versions[i];
        record.m_programId.ToString(recordProgram, sizeof(recordProgram));
        record.m_component.ToString(recordComponent, sizeof(recordComponent));
        BattlenetLogin::LogConnectionStatus("Component %s.%s.%d", recordProgram, recordComponent, record.m_version);
    }

    return Battlenet::TumorManager::Create(*this->m_application, settings);
}

// OFFSET: 0x00629A40/0x1402BC1B0
void BattlenetLogin::EnterPIN(int32_t count, uint8_t* digits) {
    if (!this->unk_105D[0]) {
        return;
    }

    this->unk_105D[0] = 0;

    char pin[12];

    for (int32_t i = 0; i < count; i++) {
        pin[i] = digits[i] + '0';
    }

    pin[count] = '\0';

    Battlenet::Event::Creep::Invoke* invoke = new (__FILE__, __LINE__) Battlenet::Event::Creep::Invoke();
    Battlenet::Event::Creep::Invoke::Authentication::Module* module = invoke->Set<0, Battlenet::Event::Creep::Invoke::Authentication>()->Set<2, Battlenet::Event::Creep::Invoke::Authentication::Module>();
    module->m_token = this->m_moduleId;
    module->m_function = Battlenet::Event::AuthModule::RequestVirtualKeypadPIN;
    *module->m_parameters.New()->Set<0, BSN::String<256> >() = pin;

    if (g_tumorManager) {
        g_tumorManager->SendEvent<Battlenet::Event::Creep::Invoke, Protocol>(Battlenet::Event::CREEP, *invoke);
    }

    delete invoke;
}

// OFFSET: 0x00629BB0/0x1402BC450
void BattlenetLogin::EnterMatrix(uint8_t value) {
    Battlenet::Event::Creep::Invoke* invoke = new (__FILE__, __LINE__) Battlenet::Event::Creep::Invoke();
    Battlenet::Event::Creep::Invoke::Authentication::Module* module = invoke->Set<0, Battlenet::Event::Creep::Invoke::Authentication>()->Set<2, Battlenet::Event::Creep::Invoke::Authentication::Module>();
    module->m_token = this->m_moduleId;
    module->m_function = Battlenet::Event::AuthModule::EnterMatrixCard;
    *module->m_parameters.New()->Set<1, uint32_t>() = value;

    if (g_tumorManager) {
        g_tumorManager->SendEvent<Battlenet::Event::Creep::Invoke, Protocol>(Battlenet::Event::CREEP, *invoke);
    }

    delete invoke;
}

// OFFSET: 0x00629CA0/0x1402BC630
void BattlenetLogin::RevertMatrix() {
    Battlenet::Event::Creep::Invoke* invoke = new (__FILE__, __LINE__) Battlenet::Event::Creep::Invoke();
    Battlenet::Event::Creep::Invoke::Authentication::Module* module = invoke->Set<0, Battlenet::Event::Creep::Invoke::Authentication>()->Set<2, Battlenet::Event::Creep::Invoke::Authentication::Module>();
    module->m_token = this->m_moduleId;
    module->m_function = Battlenet::Event::AuthModule::RevertMatrixCard;

    if (g_tumorManager) {
        g_tumorManager->SendEvent<Battlenet::Event::Creep::Invoke, Protocol>(Battlenet::Event::CREEP, *invoke);
    }

    delete invoke;
}

// OFFSET: 0x00629D70/0x1402BC7E0
void BattlenetLogin::CommitMatrix() {
    Battlenet::Event::Creep::Invoke* invoke = new (__FILE__, __LINE__) Battlenet::Event::Creep::Invoke();
    Battlenet::Event::Creep::Invoke::Authentication::Module* module = invoke->Set<0, Battlenet::Event::Creep::Invoke::Authentication>()->Set<2, Battlenet::Event::Creep::Invoke::Authentication::Module>();
    module->m_token = this->m_moduleId;
    module->m_function = Battlenet::Event::AuthModule::CommitMatrixCard;

    if (g_tumorManager) {
        g_tumorManager->SendEvent<Battlenet::Event::Creep::Invoke, Protocol>(Battlenet::Event::CREEP, *invoke);
    }

    delete invoke;
}

// OFFSET: 0x00629E40/0x1402BC990
void BattlenetLogin::FinalizeMatrix() {
    this->m_matrixEnabled = false;

    Battlenet::Event::Creep::Invoke* invoke = new (__FILE__, __LINE__) Battlenet::Event::Creep::Invoke();
    Battlenet::Event::Creep::Invoke::Authentication::Module* module = invoke->Set<0, Battlenet::Event::Creep::Invoke::Authentication>()->Set<2, Battlenet::Event::Creep::Invoke::Authentication::Module>();
    module->m_token = this->m_moduleId;
    module->m_function = Battlenet::Event::AuthModule::FinalizeMatrixCard;

    if (g_tumorManager) {
        g_tumorManager->SendEvent<Battlenet::Event::Creep::Invoke, Protocol>(Battlenet::Event::CREEP, *invoke);
    }

    delete invoke;
}

// OFFSET: 0x00629F20/0x1402BCB40
void BattlenetLogin::EnterToken(const char* token) {
    this->m_tokenRequired = false;

    Battlenet::Event::Creep::Invoke* invoke = new (__FILE__, __LINE__) Battlenet::Event::Creep::Invoke();
    Battlenet::Event::Creep::Invoke::Authentication::Module* module = invoke->Set<0, Battlenet::Event::Creep::Invoke::Authentication>()->Set<2, Battlenet::Event::Creep::Invoke::Authentication::Module>();
    module->m_token = this->m_moduleId;
    module->m_function = Battlenet::Event::AuthModule::RequestToken;
    *module->m_parameters.New()->Set<0, BSN::String<256> >() = token;

    if (g_tumorManager) {
        g_tumorManager->SendEvent<Battlenet::Event::Creep::Invoke, Protocol>(Battlenet::Event::CREEP, *invoke);
    }

    delete invoke;
}

// OFFSET: 0x0062A050/0x1402BCD50
void BattlenetLogin::SendSurveyInfoToModule() {
    Battlenet::Event::Creep::Invoke* invoke = new (__FILE__, __LINE__) Battlenet::Event::Creep::Invoke();
    Battlenet::Event::Creep::Invoke::Authentication::Module* module = invoke->Set<0, Battlenet::Event::Creep::Invoke::Authentication>()->Set<2, Battlenet::Event::Creep::Invoke::Authentication::Module>();
    module->m_token = this->m_moduleId;
    module->m_function = Battlenet::Event::AuthModule::RequestSurveyInfo;
    *module->m_parameters.New()->Set<0, BSN::String<256> >() = "WoW";
    *module->m_parameters.New()->Set<1, uint32_t>() = SFile::s_locale;

    uint32_t desktopWidth = 0;
    uint32_t desktopHeight = 0;
    uint32_t desktopBpp = 0;
    uint32_t width = 0;
    uint32_t height = 0;
    uint32_t bpp = 0;

    if (g_theGxDevicePtr) {
        g_theGxDevicePtr->GetSurveyModeInfo(desktopWidth, desktopHeight, desktopBpp, width, height, bpp);
    }

    char info[256];
    SStrPrintf(info, sizeof(info), "%u,%u,%u,%u,%u,%u", desktopWidth, desktopHeight, desktopBpp, width, height, bpp);
    *module->m_parameters.New()->Set<0, BSN::String<256> >() = info;

    if (g_tumorManager) {
        g_tumorManager->SendEvent<Battlenet::Event::Creep::Invoke, Protocol>(Battlenet::Event::CREEP, *invoke);
    }

    delete invoke;
}

// OFFSET: 0x0062A250/0x1402BD0A0
bool BattlenetLogin::SetGameAccount(uint32_t index) {
    if (index >= this->m_gameAccounts.m_count || this->m_gameAccountSelected) {
        return false;
    }

    SStrCopy(this->m_accountName, this->m_gameAccounts.m_data[index].m_name, sizeof(this->m_accountName));

    Battlenet::Event::Creep::Invoke* invoke = new (__FILE__, __LINE__) Battlenet::Event::Creep::Invoke();
    Battlenet::Event::Creep::Invoke::Authentication::Module* module = invoke->Set<0, Battlenet::Event::Creep::Invoke::Authentication>()->Set<2, Battlenet::Event::Creep::Invoke::Authentication::Module>();
    module->m_token = this->m_moduleId;
    module->m_function = Battlenet::Event::AuthModule::RequestGameAccount;
    *module->m_parameters.New()->Set<1, uint32_t>() = this->m_gameAccounts.m_data[index].m_region;
    *module->m_parameters.New()->Set<0, BSN::String<256> >() = this->m_gameAccounts.m_data[index].m_name;

    if (g_tumorManager) {
        g_tumorManager->SendEvent<Battlenet::Event::Creep::Invoke, Protocol>(Battlenet::Event::CREEP, *invoke);
        this->m_gameAccountSelected = 1;
        delete invoke;
        return true;
    }

    delete invoke;
    return false;
}

// OFFSET: 0x00629890/0x1402BBF30
void BattlenetLogin::Logon(const char* loginServer, const char* portal) {
    if (this->m_loggedOn) {
        return;
    }

    if (!g_tumorManager) {
        g_tumorManager = this->CreateTumorManager();

        if (!g_tumorManager) {
            this->m_loginResponse->UpdateLoginStatus(LOGIN_STATE_FAILED, LOGIN_NO_BATTLENET_MANAGER, NULL, 0);
            return;
        }

        BattlenetUI::Init();
    }

    if (!SStrChr(this->m_username, '@') || static_cast<uint32_t>(SStrLen(this->m_username) - 3) > 317) {
        FrameScript_SignalEvent(GLUE_EVENT_OPEN_STATUS_DIALOG, "%s%s", "OKAY_HTML", FrameScript_GetText("LOGIN_UNKNOWN_ACCOUNT", -1, GENDER_NOT_APPLICABLE));
        return;
    }

    this->m_reconnect = false;
    this->m_serviceState = 4;
    this->m_logonStatus = -1;
    this->m_logonResult = 0;
    this->m_loginResponse->UpdateLoginStatus(LOGIN_STATE_CONNECTING, LOGIN_OK, NULL, 0);

    Battlenet::Event::Creep::Invoke* invoke = new (__FILE__, __LINE__) Battlenet::Event::Creep::Invoke();

    if (portal && *portal) {
        invoke->Set<12, Battlenet::Event::Creep::Invoke::DarkPortal>()->Set<0, Battlenet::Event::Creep::Invoke::DarkPortal::Select>()->m_name = portal;
        g_tumorManager->SendEvent<Battlenet::Event::Creep::Invoke, Protocol>(Battlenet::Event::CREEP, *invoke);
    }

    this->m_gameAccountSelected = 0;

    Battlenet::Event::Creep::Invoke::Authentication::Logon* logon = invoke->Set<0, Battlenet::Event::Creep::Invoke::Authentication>()->Set<1, Battlenet::Event::Creep::Invoke::Authentication::Logon>();
    _strlwr(this->m_username);
    logon->m_mail = this->m_username;
    g_tumorManager->SendEvent<Battlenet::Event::Creep::Invoke, Protocol>(Battlenet::Event::CREEP, *invoke);
    delete invoke;
}

// OFFSET: 0x0062A4C0/0x1402BD320
void BattlenetLogin::QueueEvent(BN_EVENT_ID id, void* data, int32_t size) {
    this->m_eventLock.Enter();

    BN_EVENT* event = this->m_events[this->m_eventIndex].NewNode(2, 0, 0);
    event->m_data = data;
    event->m_id = id;
    event->m_size = size;

    this->m_eventLock.Leave();
}

// OFFSET: 0x0062A770
void BattlenetApplication::WCConnected(WowConnection* connection, WowConnection* inbound, uint32_t timeStamp, const NETCONNADDR* addr) {
    this->m_localAddress.m_address.m_data[0] = addr->selfAddr.data[4];
    this->m_localAddress.m_address.m_data[1] = addr->selfAddr.data[5];
    this->m_localAddress.m_address.m_data[2] = addr->selfAddr.data[6];
    this->m_localAddress.m_address.m_data[3] = addr->selfAddr.data[7];
    this->m_localAddress.m_port.m_data[0] = addr->selfAddr.data[2];
    this->m_localAddress.m_port.m_data[1] = addr->selfAddr.data[3];

    if (!this->m_login) {
        SErrDisplayAppFatalMessage("", "", 0);
    }

    NETADDR* peer = static_cast<NETADDR*>(malloc(sizeof(NETADDR)));
    *peer = addr->peerAddr;
    this->m_login->QueueEvent(BN_EVENT_CONNECTED, peer, sizeof(NETADDR));
}

// OFFSET: 0x0062A7E0/0x1402BD670
void BattlenetApplication::WCDataReady(WowConnection* connection, uint32_t timeStamp, uint8_t* data, int32_t length) {
    if (!this->m_login) {
        SErrDisplayAppFatalMessage("", "", 0);
    }

    void* copy = malloc(length);
    memcpy(copy, data, length);
    this->m_login->QueueEvent(BN_EVENT_DATA, copy, length);
}

// PROVEN: 03239c46 IDENTICAL/NONE
// OFFSET: 0x0062A820/0x1402BD6E0
void BattlenetApplication::WCCantConnect(WowConnection* connection, uint32_t timeStamp, const NETCONNADDR* addr) {
    this->m_login->QueueEvent(BN_EVENT_CANT_CONNECT, NULL, 0);
}

// PROVEN: ea1ef7f7 IDENTICAL/NONE
// OFFSET: 0x0062A840/0x1402BD700
void BattlenetApplication::WCDisconnected(WowConnection* connection, uint32_t timeStamp, const NETCONNADDR* addr) {
    this->m_login->QueueEvent(BN_EVENT_DISCONNECTED, NULL, 0);
}

// OFFSET: 0x0062A860
void BattlenetLogin::UpdateRealmListData(uint32_t region, uint32_t battlegroup, uint32_t id, const Battlenet::WoWRealm::RealmInfo* info, uint8_t flags, bool remove) {
    this->m_realmLock.Enter();

    REALM_INFO* realm = NULL;

    for (BN_REALM* node = this->m_realms.Head(); node; node = this->m_realms.Next(node)) {
        if (node->m_info.m_regionId == region && node->m_info.m_battlegroupId == battlegroup && node->m_info.m_realmId == id) {
            realm = &node->m_info;

            if (remove) {
                this->m_realms.DeleteNode(node);
                this->m_realmLock.Leave();
                return;
            }

            break;
        }
    }

    if (!info) {
        this->m_realmLock.Leave();
        return;
    }

    if (!realm) {
        BN_REALM* node = this->m_realms.NewNode(2, 0, 0);

        if (!node) {
            this->m_realmLock.Leave();
            return;
        }

        realm = &node->m_info;
    }

    realm->m_type = info->m_type;
    realm->m_otherFlags = info->m_stateFlags;
    realm->m_flags = flags;
    SStrCopy(realm->m_realmName, info->m_name.m_data, sizeof(realm->m_realmName));
    realm->m_addresses[0].Clear();
    realm->m_addresses[1].Clear();
    realm->m_ports[0] = 0;
    realm->m_ports[1] = 0;
    realm->m_population = info->m_population;
    realm->m_numCharacters = this->GetCharacterCount(region, battlegroup, id);
    realm->m_category = info->m_category;
    realm->m_regionId = region;
    realm->m_battlegroupId = battlegroup;
    realm->m_realmId = id;

    if (!info->m_privilegedData.m_exists) {
        realm->m_build = 0;
        realm->m_versionMajor = 0;
        realm->m_versionMinor = 0;
        realm->m_versionRevision = 0;
        realm->m_versionBuild = 0;
        this->m_realmLock.Leave();
        return;
    }

    const Battlenet::WoWRealm::RealmInfo::__subtypeof_m_privilegedData& privileged = info->m_privilegedData.m_value;
    realm->m_build = privileged.m_configId;
    realm->m_versionMajor = SStrToInt(privileged.m_version.m_data);

    const char* cursor = strchr(privileged.m_version.m_data, '.');

    if (cursor) {
        cursor++;
        realm->m_versionMinor = SStrToInt(cursor);
        cursor = strchr(cursor, '.');

        if (cursor) {
            cursor++;
            realm->m_versionRevision = SStrToInt(cursor);
            cursor = strchr(cursor, '.');

            if (cursor) {
                realm->m_flags |= 0x4;
                realm->m_versionBuild = SStrToInt(cursor + 1);
            }
        }
    }

    realm->m_addresses[0].Clear();
    realm->m_addresses[1].Clear();
    realm->m_ports[0] = 0;
    realm->m_ports[1] = 0;
    realm->m_ports[0] = privileged.m_address.m_port.m_data[1] + (privileged.m_address.m_port.m_data[0] << 8);
    realm->m_addresses[0].m_type = SOCKADDR_IPV4;
    realm->m_addresses[0].m_ipv4 = *reinterpret_cast<const uint32_t*>(privileged.m_address.m_address.m_data);

    this->m_realmLock.Leave();
}

// OFFSET: 0x0062ABE0/0x1402BDBF0
void BattlenetLogin::GetRealmList() {
    if (this->m_realmListState == 3) {
        this->QueueEvent(BN_EVENT_REALM_LIST, NULL, 0);
    } else if (this->m_realmListState) {
        this->QueueEvent(BN_EVENT_RECONNECTED, NULL, 0);
    } else if (g_tumorManager && this->IsLoggedOn()) {
        this->m_realmListState = 1;

        Battlenet::Event::Creep::Invoke* invoke = new (__FILE__, __LINE__) Battlenet::Event::Creep::Invoke();
        invoke->Set<6, Battlenet::Event::Creep::Invoke::WoW>()->Set<0, Battlenet::Event::Creep::Invoke::WoW::RequestRealmList>()->m_on = true;
        g_tumorManager->SendEvent<Battlenet::Event::Creep::Invoke, Protocol>(Battlenet::Event::CREEP, *invoke);
        delete invoke;
    }
}

// OFFSET: 0x0062AC80/0x1402BDD00
BattlenetLogin::BattlenetLogin() {
    this->m_application = NULL;
    g_tumorManager = NULL;
    this->m_moduleId = 0;
    this->m_logonResult = 0;
    this->m_serviceState = 4;

    this->m_realmLock.Enter();
    this->m_realms.Clear();
    this->m_realmLock.Leave();

    this->m_realmListState = 0;
    this->m_joinRealmId = 0;

    this->m_eventLock.Enter();
    this->m_events[0].Clear();
    this->m_events[1].Clear();
    this->m_eventLock.Leave();

    this->m_eventIndex = 0;

    this->m_gameAccountLock.Enter();
    this->m_gameAccounts.Clear();
    memset(this->m_accountName, 0, sizeof(this->m_accountName));
    this->m_gameAccountLock.Leave();

    this->m_gameAccountSelected = 0;

    this->m_patchInstructionLock.Enter();
    this->m_patchInstructions.Clear();
    this->m_patchInstructionLock.Leave();

    this->m_accountFlags = 0;
    this->m_accountFlags2 = 0;
    this->m_surveyPending = false;
    this->m_surveyTime = 0;
    this->m_outOfBandType = 0;
    this->m_outOfBandTimeout = 0;
    this->m_outOfBandTitle = NULL;
    this->m_outOfBandText = NULL;
    this->m_outOfBandStart = 0;
}

// OFFSET: 0x0062AED0/0x1402BE010
void BattlenetLogin::DestroyTumorManager() {
    s_battlenetAllocator.Delete(g_tumorManager);

    this->m_eventLock.Enter();
    this->m_events[0].Clear();
    this->m_events[1].Clear();
    this->m_eventLock.Leave();

    this->m_realmLock.Enter();
    this->m_realms.Clear();
    this->m_realmLock.Leave();

    this->m_realmListState = 0;

    this->m_gameAccountLock.Enter();
    this->m_gameAccounts.Clear();
    this->m_gameAccountLock.Leave();

    this->m_gameAccountSelected = 0;

    this->m_patchInstructionLock.Enter();
    this->m_patchInstructions.Clear();
    this->m_patchInstructionLock.Leave();
}

// OFFSET: 0x0062B010/0x1402BE1F0
void BattlenetLogin::Init(LoginResponse* loginResponse) {
    this->m_loginResponse = loginResponse;

    if (!loginResponse) {
        return;
    }

    this->m_logonResult = 0;
    this->m_serviceState = 4;

    this->m_realmLock.Enter();
    this->m_realms.Clear();
    this->m_realmLock.Leave();

    this->m_eventLock.Enter();
    this->m_events[0].Clear();
    this->m_events[1].Clear();
    this->m_eventLock.Leave();

    this->m_gameAccountLock.Enter();
    this->m_gameAccounts.Clear();
    memset(this->m_accountName, 0, sizeof(this->m_accountName));
    this->m_gameAccountLock.Leave();

    this->m_gameAccountSelected = 0;
    this->m_realmCharacterCounts = static_cast<RealmCharacterCount*>(SMemAlloc(64 * sizeof(RealmCharacterCount), __FILE__, __LINE__, 0));
    this->m_realmCharacterCountCount = 0;
    this->m_surveyPending = false;
    this->m_surveyTime = 0;
    this->m_outOfBandType = 0;
    this->m_outOfBandTimeout = 0;
    this->m_outOfBandStart = 0;
    SMemFree(this->m_outOfBandTitle, __FILE__, __LINE__, 0);
    this->m_outOfBandTitle = NULL;
    SMemFree(this->m_outOfBandText, __FILE__, __LINE__, 0);
    this->m_outOfBandText = NULL;

    Battlenet::Initialize(s_battlenetAllocator);

    delete this->m_application;

    this->m_application = new (__FILE__, __LINE__) BattlenetApplication();

    if (this->m_application) {
        this->m_application->m_login = this;
        g_tumorManager = this->CreateTumorManager();

        if (g_tumorManager) {
            BattlenetUI::Init();
        } else {
            this->m_loginResponse->UpdateLoginStatus(LOGIN_STATE_FAILED, LOGIN_NO_BATTLENET_MANAGER, NULL, 0);
        }
    } else {
        if (g_tumorManager) {
            s_battlenetAllocator.Delete(g_tumorManager);
            g_tumorManager = NULL;
        }

        this->m_loginResponse->UpdateLoginStatus(LOGIN_STATE_FAILED, LOGIN_NO_BATTLENET_APPLICATION, NULL, 0);
    }
}

// OFFSET: 0x0062B250/0x1402BE490
void BattlenetLogin::HandleDisconnectEvents(uint32_t index) {
    TSList<BN_EVENT>& events = this->m_events[index];
    bool disconnected = false;
    bool serviceDisconnected = false;

    for (BN_EVENT* event = events.Head(); event; event = events.Next(event)) {
        if (event->m_id == BN_EVENT_SERVICE_DISCONNECTED) {
            BattlenetUI::SetDisconnectedFromBattlenet(true);
            disconnected = true;
            serviceDisconnected = true;
            break;
        }

        if (event->m_id == BN_EVENT_SERVICE_FAILED) {
            BattlenetUI::SetDisconnectedFromBattlenet(true);
            this->m_loggedOn = false;

            if (!IsInOrEnteringWorld()) {
                disconnected = true;
                break;
            }
        }
    }

    if (!disconnected) {
        return;
    }

    for (BN_EVENT* event = events.Head(); event;) {
        BN_EVENT* next = events.Next(event);
        free(event->m_data);
        events.DeleteNode(event);
        event = next;
    }

    if (CGlueMgr::m_loginState == LOGIN_STATE_DOWNLOADFILE) {
        return;
    }

    if (!this->m_online) {
        this->m_logonStatus = 1;
    }

    if (serviceDisconnected) {
        this->m_loginResponse->UpdateLoginStatus(LOGIN_STATE_DISCONNECTED, LOGIN_OK, NULL, 0);
    } else {
        int32_t result = this->m_logonResult;

        if (!result) {
            result = 105;
        }

        this->LogonResultBN(result);
    }

    this->m_loggedOn = false;
    this->m_online = false;

    Battlenet::TumorManager* manager = this->CreateTumorManager();
    this->DestroyTumorManager();
    g_tumorManager = manager;
    BattlenetUI::Init();
}

// OFFSET: 0x0062B3A0/0x1402BE660
void BattlenetLogin::HandleInvoke(const Battlenet::Event::Larva::Invoke* invoke) {
    if (invoke->m_tag == 0) {
        const Battlenet::Event::Larva::Invoke::Authentication& authentication = invoke->Value<Battlenet::Event::Larva::Invoke::Authentication>();

        if (authentication.m_tag == 0) {
            const Battlenet::Event::Larva::Invoke::Authentication::Module& module = authentication.Value<Battlenet::Event::Larva::Invoke::Authentication::Module>();

            if (!_stricmp(module.m_function.m_data, Battlenet::Event::AuthModule::RequestPassword.m_data)) {
                this->m_moduleId = module.m_token;
                this->GetLogonMethod();
            } else if (!_stricmp(module.m_function.m_data, Battlenet::Event::AuthModule::RequestToken.m_data)) {
                this->m_moduleId = module.m_token;
                this->SetTokenInfo(true, module.m_parameters[0].Value<uint32_t>());
            } else if (!_stricmp(module.m_function.m_data, Battlenet::Event::AuthModule::RequestMatrixCard.m_data)) {
                this->m_moduleId = module.m_token;
                this->SetAuthModuleMatrixInfo(module.m_parameters[0].Value<BSN::String<256> >().m_data, module.m_parameters[1].Value<BSN::String<256> >().m_data);
            } else if (!_stricmp(module.m_function.m_data, Battlenet::Event::AuthModule::RequestVirtualKeypadPIN.m_data)) {
                this->m_moduleId = module.m_token;
                this->SetAuthModulePinInfo(module.m_parameters[0].Value<BSN::String<256> >().m_data);
            } else if (!_stricmp(module.m_function.m_data, Battlenet::Event::AuthModule::ClearGameAccounts.m_data)) {
                this->m_moduleId = module.m_token;
                this->m_gameAccountLock.Enter();
                this->m_gameAccounts.Clear();
                this->m_gameAccountLock.Leave();
            } else if (!_stricmp(module.m_function.m_data, Battlenet::Event::AuthModule::AddGameAccount.m_data)) {
                this->m_moduleId = module.m_token;
                this->m_gameAccountLock.Enter();

                GameAccount* account = this->m_gameAccounts.New();
                account->m_region = module.m_parameters[0].Value<uint32_t>();
                SStrCopy(account->m_name, module.m_parameters[1].Value<BSN::String<256> >().m_data, sizeof(account->m_name));

                this->m_gameAccountLock.Leave();
            } else if (!_stricmp(module.m_function.m_data, Battlenet::Event::AuthModule::RequestGameAccount.m_data)) {
                this->m_moduleId = module.m_token;
                this->QueueEvent(BN_EVENT_GAME_ACCOUNTS_UPDATED, NULL, 0);
            } else if (!_stricmp(module.m_function.m_data, Battlenet::Event::AuthModule::RequestSurveyInfo.m_data)) {
                this->m_moduleId = module.m_token;
                this->QueueEvent(BN_EVENT_SEND_SURVEY_INFO, NULL, 0);
            } else if (!_stricmp(module.m_function.m_data, Battlenet::Event::AuthModule::ShowSurveyDialog.m_data)) {
                this->m_moduleId = module.m_token;
                this->QueueEvent(BN_EVENT_SHOW_SURVEY, NULL, 0);
            } else if (!_stricmp(module.m_function.m_data, Battlenet::Event::AuthModule::DisplayOutOfBandChallenge.m_data)) {
                this->SetAuthModuleOutOfBandInfo(module.m_parameters[0].Value<uint32_t>(), module.m_parameters[1].Value<BSN::String<256> >().m_data, module.m_parameters[2].Value<BSN::String<256> >().m_data, module.m_parameters[3].Value<uint32_t>());
                this->m_moduleId = module.m_token;
                this->QueueEvent(BN_EVENT_OUTOFBAND_CHALLENGE, NULL, 0);
            } else if (!_stricmp(module.m_function.m_data, Battlenet::Event::AuthModule::DismissOutOfBandChallenge.m_data)) {
                this->QueueEvent(BN_EVENT_OUTOFBAND_DISMISS, NULL, 0);
            }
        } else if (authentication.m_tag == 1) {
            if (this->m_serviceState == 6) {
                this->m_application->LogLevel("Ignoring Authentication Success invoke while in Service::RECOVERED state.", 4);
            } else {
                const Battlenet::Event::Larva::Invoke::Authentication::Success& success = authentication.Value<Battlenet::Event::Larva::Invoke::Authentication::Success>();
                strcpy(this->m_username, success.m_gameAccountName.m_data);
                this->m_accountFlags = static_cast<uint32_t>(success.m_gameAccountFlags);
                this->m_accountFlags2 = static_cast<uint32_t>(success.m_gameAccountFlags >> 32);
                this->m_logonResult = 0;
                this->m_loginResponse->UpdateLoginStatus(LOGIN_STATE_AUTHENTICATED, LOGIN_OK, NULL, 0);
                this->m_surveyPending = false;
            }
        } else if (authentication.m_tag == 2 && this->m_serviceState != 5) {
            char text[64];
            const Battlenet::Event::Larva::Invoke::Authentication::Failure& failure = authentication.Value<Battlenet::Event::Larva::Invoke::Authentication::Failure>();

            SStrPrintf(text, 63, "Battle.net Authentication failure code: %d", failure.m_error);
            ConsoleWrite(text, DEFAULT_COLOR);

            if (failure.m_error != 138) {
                this->LogonResultBN(failure.m_error);

                if (this->m_serviceState != 7) {
                    this->QueueEvent(BN_EVENT_LOGOFF, NULL, 0);
                }
            }
        }
    } else if (invoke->m_tag == 1) {
        const Battlenet::Event::Larva::Invoke::Service& service = invoke->Value<Battlenet::Event::Larva::Invoke::Service>();
        this->m_serviceState = service.m_tag;

        switch (service.m_tag) {
            case 2:
                this->QueueEvent(BN_EVENT_SERVICE_FAILED, NULL, 0);
                break;

            case 3:
                if (!service.Value<Battlenet::Event::Larva::Invoke::Service::FloodingError>().m_depth) {
                    this->m_serviceState = 6;
                }

                break;

            case 4:
                this->QueueEvent(BN_EVENT_SERVICE_DISCONNECTED, NULL, 0);
                break;

            case 5:
                this->QueueEvent(BN_EVENT_SERVICE_LOST, NULL, 0);
                break;

            case 6:
                this->QueueEvent(BN_EVENT_SERVICE_RECOVERED, NULL, 0);
                break;

            case 7:
                this->QueueEvent(BN_EVENT_RECONNECTED, NULL, 0);
                break;
        }
    } else if (invoke->m_tag == 9) {
        const Battlenet::Event::Larva::Invoke::WoW& wow = invoke->Value<Battlenet::Event::Larva::Invoke::WoW>();

        if (wow.m_tag == 0) {
            const Battlenet::Event::Larva::Invoke::WoW::RealmUpdate& update = wow.Value<Battlenet::Event::Larva::Invoke::WoW::RealmUpdate>();

            if (update.m_result.m_tag == 0) {
                const Battlenet::WoWRealm::RealmInfo& info = update.m_result.Value<Battlenet::Event::Larva::Invoke::WoW::RealmUpdate::Result::Update>().m_info;
                this->UpdateRealmListData(update.m_id.m_region, update.m_id.m_site, update.m_id.m_realm, &info, info.m_infoFlags, false);
            } else if (update.m_result.m_tag == 1) {
                this->UpdateRealmListData(update.m_id.m_region, update.m_id.m_site, update.m_id.m_realm, NULL, 0, true);
            }
        } else if (wow.m_tag == 1) {
            const Battlenet::Event::Larva::Invoke::WoW::RealmList::Result& result = wow.Value<Battlenet::Event::Larva::Invoke::WoW::RealmList>().m_result;

            if (result.m_tag == 0) {
                this->m_realmLock.Enter();
                this->m_realms.Clear();
                this->m_realmListState = 2;

                const BSN::Array<Battlenet::WoWRealm::ToonCountEntry, 64>& counts = result.Value<Battlenet::Event::Larva::Invoke::WoW::RealmList::Result::Start>().m_toonCounts;
                uint32_t count = counts.m_count;
                this->m_realmCharacterCountCount = count;

                if (count >= 64) {
                    count = 64;
                }

                this->m_realmCharacterCountCount = count;

                if (this->m_realmCharacterCounts) {
                    for (uint32_t i = 0; i < this->m_realmCharacterCountCount; i++) {
                        this->m_realmCharacterCounts[i].m_region = counts.m_data[i].m_id.m_region;
                        this->m_realmCharacterCounts[i].m_battlegroup = counts.m_data[i].m_id.m_site;
                        this->m_realmCharacterCounts[i].m_id = counts.m_data[i].m_id.m_realm;
                        this->m_realmCharacterCounts[i].m_count = counts.m_data[i].m_count;
                    }
                }

                this->m_realmLock.Leave();
            } else if (result.m_tag == 1) {
                this->m_realmListState = 3;
                this->QueueEvent(BN_EVENT_REALM_LIST, NULL, 0);
            } else if (result.m_tag == 2) {
                this->m_realmListState = 0;
            }
        } else if (wow.m_tag == 2) {
            const Battlenet::Event::Larva::Invoke::WoW::JoinResponse::Result& result = wow.Value<Battlenet::Event::Larva::Invoke::WoW::JoinResponse>().m_result;

            if (result.m_tag == 0) {
                const Battlenet::Event::Larva::Invoke::WoW::JoinResponse::Result::Success& success = result.Value<Battlenet::Event::Larva::Invoke::WoW::JoinResponse::Result::Success>();

                for (uint32_t i = 0; i < 40; i++) {
                    this->m_sessionKey[i] = success.m_sessionKey.m_data[i];
                }

                SockAddr ipv4Addresses[16];
                uint16_t ipv4Ports[16];

                for (uint32_t i = 0; i < success.m_addresses.m_v4.m_count; i++) {
                    const Battlenet::IP4::AddressPort& address = success.m_addresses.m_v4.m_data[i];
                    ipv4Addresses[i].m_ipv4 = *reinterpret_cast<const uint32_t*>(address.m_address.m_data);
                    ipv4Addresses[i].m_type = SOCKADDR_IPV4;
                    ipv4Ports[i] = address.m_port.m_data[1] | (address.m_port.m_data[0] << 8);
                }

                REALM_ADDRESS_LIST lists[2];
                lists[0].m_type = SOCKADDR_IPV4;
                lists[0].m_count = success.m_addresses.m_v4.m_count;
                lists[0].m_addresses = ipv4Addresses;
                lists[0].m_ports = ipv4Ports;

                SockAddr ipv6Addresses[16];
                uint16_t ipv6Ports[16];

                for (uint32_t i = 0; i < success.m_addresses.m_v6.m_count; i++) {
                    const Battlenet::IP6::AddressPort& address = success.m_addresses.m_v6.m_data[i];
                    memcpy(ipv6Addresses[i].m_ipv6, address.m_address.m_data, sizeof(ipv6Addresses[i].m_ipv6));
                    ipv6Ports[i] = address.m_port.m_data[1] | (address.m_port.m_data[0] << 8);
                    ipv6Addresses[i].m_type = SOCKADDR_IPV6;
                }

                lists[1].m_type = SOCKADDR_IPV6;
                lists[1].m_count = success.m_addresses.m_v6.m_count;
                lists[1].m_addresses = ipv6Addresses;
                lists[1].m_ports = ipv6Ports;

                this->m_loginResponse->JoinRealmResult(this->m_joinRealmId, lists, 2, 0, 0);
            } else if (result.m_tag == 1) {
                this->m_loginResponse->JoinRealmResult(0, NULL, 0, 0, result.Value<Battlenet::Event::Larva::Invoke::WoW::JoinResponse::Result::Failure>().m_reasonCode);
            }
        }
    }
}

// OFFSET: [0x0062BB90/0x1402BEEB0, 0x0062C280/0x1402BF990]
BattlenetLogin::~BattlenetLogin() {
    this->m_patchInstructionLock.Enter();
    this->m_patchInstructions.Clear();
    this->m_patchInstructionLock.Leave();

    this->m_gameAccountLock.Enter();
    this->m_gameAccounts.Clear();
    this->m_gameAccountLock.Leave();

    this->m_gameAccountSelected = 0;

    this->m_eventLock.Enter();
    this->m_events[0].Clear();
    this->m_events[1].Clear();
    this->m_eventLock.Leave();

    this->m_realmLock.Enter();
    this->m_realms.Clear();
    this->m_realmLock.Leave();

    if (g_tumorManager) {
        BattlenetUI::Uninit();
    }

    this->DestroyTumorManager();

    delete this->m_application;
    delete[] this->m_realmCharacterCounts;

    this->m_realmCharacterCounts = NULL;
    SMemFree(this->m_outOfBandTitle, __FILE__, __LINE__, 0);
    SMemFree(this->m_outOfBandText, __FILE__, __LINE__, 0);

    Battlenet::Teardown();
}

// OFFSET: 0x0062BE30/0x1402BF2C0
void BattlenetLogin::EventQueueUpdate() {
    uint32_t index = this->m_eventIndex;

    this->m_eventLock.Enter();
    this->m_eventIndex = 1 - this->m_eventIndex;
    this->m_eventLock.Leave();

    this->HandleDisconnectEvents(index);

    TSList<BN_EVENT>& events = this->m_events[index];

    for (BN_EVENT* event = events.Head(); event;) {
        BN_EVENT* next = events.Next(event);

        switch (event->m_id) {
            case BN_EVENT_CONNECTED:
                this->Connected(*static_cast<NETADDR*>(event->m_data));
                break;

            case BN_EVENT_CANT_CONNECT:
                this->CantConnect();
                break;

            case BN_EVENT_DISCONNECTED:
                this->Disconnected();
                break;

            case BN_EVENT_DATA: {
                uint32_t used = 0;

                if (g_tumorManager) {
                    Battlenet::SystemClient& client = g_tumorManager->m_tumor;
                    client.Recv(this->m_application->m_connection, event->m_data, event->m_size, used);
                }

                break;
            }

            case BN_EVENT_REALM_LIST:
                this->BuildRealmListResult();
                break;

            case BN_EVENT_REALM_LIST_EMPTY:
                this->BuildEmptyRealmListResult();
                break;

            case BN_EVENT_RECONNECTED:
                this->m_loginResponse->UpdateLoginStatus(LOGIN_STATE_RESPONSE_CONNECTED, LOGIN_OK, NULL, 0);
                BattlenetUI::SetBattlenetConnectionRecovered();
                break;

            case BN_EVENT_SERVICE_LOST:
                BattlenetUI::SetDisconnectedFromBattlenet(false);
                break;

            case BN_EVENT_SERVICE_RECOVERED:
                BattlenetUI::SetBattlenetConnectionRecovered();
                break;

            case BN_EVENT_GAME_ACCOUNTS_UPDATED:
                FrameScript_SignalEvent(GLUE_EVENT_GAME_ACCOUNTS_UPDATED, NULL);
                break;

            case BN_EVENT_LOGOFF:
                this->Logoff();
                break;

            case BN_EVENT_SHOW_SURVEY:
                this->m_surveyPending = true;
                this->m_surveyTime = OsGetAsyncTimeMs();
                FrameScript_SignalEvent(GLUE_EVENT_OPEN_STATUS_DIALOG, "%s%s", "CANCEL", FrameScript_GetText("LOGIN_STATE_SURVEY", -1, GENDER_NOT_APPLICABLE));
                break;

            case BN_EVENT_SEND_SURVEY_INFO:
                this->SendSurveyInfoToModule();
                break;

            case BN_EVENT_OUTOFBAND_CHALLENGE:
                this->m_outOfBandStart = OsGetAsyncTimeMs();
                this->m_loginResponse->UpdateLoginStatus(LOGIN_STATE_OUTOFBAND_CHALLENGE, LOGIN_OK, NULL, 0);
                break;

            case BN_EVENT_OUTOFBAND_DISMISS:
                this->m_loginResponse->UpdateLoginStatus(LOGIN_STATE_CONNECTING, LOGIN_OK, NULL, 0);
                this->ClearAuthModuleOutOfBandInfo();
                break;
        }

        free(event->m_data);
        events.DeleteNode(event);
        event = next;
    }
}

// OFFSET: 0x0062C080/0x1402BF580
void BattlenetApplication::RecvEvent(const Battlenet::Event::Header& header, const BSN::EncodedObject* data) {
    if (header.m_id == 5) {
        static BSN::BSNEncodedObject<Battlenet::Event::Larva::Invoke, Protocol> s_larvaInvoke;
        s_larvaInvoke.WalkAndGet(data);
        this->m_login->HandleInvoke(&s_larvaInvoke.m_data);
    } else if (header.m_id == 1) {
        static BSN::BSNEncodedObject<Battlenet::Event::Update::Invoke, Protocol> s_updateInvoke;
        s_updateInvoke.WalkAndGet(data);
        this->m_login->HandleUpdateInvoke(&s_updateInvoke.m_data);
    }
}

// OFFSET: 0x0062C190/0x1402BF7F0
void BattlenetLogin::Update() {
    this->EventQueueUpdate();

    if (g_tumorManager) {
        Battlenet::SystemClient& client = g_tumorManager->m_tumor;
        client.Process(OsGetAsyncTimeMs());
    }

    if (this->m_surveyPending && this->m_serviceState != 2 && this->m_serviceState != 5 && OsGetAsyncTimeMs() - this->m_surveyTime > 1000) {
        this->m_surveyPending = false;
        FrameScript_SignalEvent(GLUE_EVENT_CLOSE_STATUS_DIALOG, NULL);

        if (g_tumorManager) {
            Battlenet::Event::Creep::Invoke* invoke = new (__FILE__, __LINE__) Battlenet::Event::Creep::Invoke();
            Battlenet::Event::Creep::Invoke::Authentication::Module* module = invoke->Set<0, Battlenet::Event::Creep::Invoke::Authentication>()->Set<2, Battlenet::Event::Creep::Invoke::Authentication::Module>();
            module->m_token = this->m_moduleId;
            module->m_function = Battlenet::Event::AuthModule::SurveyDialogComplete;
            g_tumorManager->SendEvent<Battlenet::Event::Creep::Invoke, Protocol>(Battlenet::Event::CREEP, *invoke);
            delete invoke;
        }
    }
}

// PROVEN: e206d4dc IDENTICAL/NONE
// OFFSET: 0x009020E0
bool BattlenetLogin::IsConnected() {
    return this->m_serviceState == 7 || this->m_serviceState == 6 || this->m_serviceState == 3;
}

// PROVEN: 1bd8bf07 IDENTICAL/NONE
// OFFSET: 0x00627570/0x140893650
LOGIN_STATE BattlenetLogin::NextSecurityState(LOGIN_STATE state) {
    return static_cast<LOGIN_STATE>(14);
}

// PROVEN: 62643042 IDENTICAL/NONE
// OFFSET: 0x006278C0/0x1402B99E0
const uint8_t* BattlenetLogin::GetKeyMap() {
    return &this->unk_105D[1];
}

// OFFSET: 0x00627630/0x1402B9670
bool BattlenetLogin::IsTrialAccount() {
    return (this->m_accountFlags & 0x8) != 0;
}

// OFFSET: 0x00627650/0x1402B9680
bool BattlenetLogin::IsProPassAccount() {
    return (this->m_accountFlags & 0x800000) != 0;
}

// OFFSET: 0x00627670/0x1402B9690
bool BattlenetLogin::IsGMAccount() {
    return (this->m_accountFlags & 0x1) != 0;
}

// PROVEN: 8dd2d732 IDENTICAL/NONE
// OFFSET: 0x006278D0/0x1402B99F0
bool BattlenetLogin::GetMatrixInfo(uint32_t* width, uint32_t* height, uint32_t* minDigits, uint32_t* maxDigits, bool* flipCoords, uint32_t* challengeCount) {
    if (!this->m_matrixEnabled) {
        return false;
    }

    *width = this->m_matrixWidth;
    *height = this->m_matrixHeight;
    *minDigits = this->m_matrixMinDigitCount;
    *maxDigits = this->m_matrixMaxDigitCount;
    *flipCoords = this->m_matrixFlipCoords;
    *challengeCount = this->m_matrixChallengeCount;
    return true;
}

// PROVEN: ead726c1 IDENTICAL/NONE
// OFFSET: 0x00627930/0x1402B9A50
bool BattlenetLogin::GetMatrixCoordinates(uint32_t index, uint32_t* x, uint32_t* y) {
    if (!this->m_matrixEnabled || !this->m_matrixData || index >= this->m_matrixChallengeCount) {
        return false;
    }

    uint32_t coordinate = this->m_matrixData[index];
    *x = coordinate % this->m_matrixWidth;
    *y = coordinate / this->m_matrixWidth;
    return true;
}

// PROVEN: ca297bcd IDENTICAL/NONE
// OFFSET: 0x006286A0/0x1402BA7D0
uint32_t BattlenetLogin::GetNumGameAccounts() {
    return this->m_gameAccounts.m_count;
}

// OFFSET: 0x006286B0/0x1402BA7E0
const char* BattlenetLogin::GetGameAccountName(uint32_t index) {
    if (index < this->m_gameAccounts.m_count) {
        return this->m_gameAccounts.m_data[index].m_name;
    }

    return NULL;
}

// OFFSET: 0x00628710/0x1402BA830
uint32_t BattlenetLogin::GetGameAccountRegion(uint32_t index) {
    if (index < this->m_gameAccounts.m_count) {
        return this->m_gameAccounts.m_data[index].m_region;
    }

    return 0;
}

// PROVEN: 1824d6dd IDENTICAL/NONE
// OFFSET: [0x00847950]
void BattlenetLogin::GetVersionProof(const uint8_t* versionChallenge) {
}

// PROVEN: 53fa9fca IDENTICAL/NONE
// OFFSET: [0x00627990]
void BattlenetLogin::SetPinInfo(bool enabled, uint32_t seed, const uint8_t* salt) {
}

// PROVEN: 2ef3292c IDENTICAL/NONE
// OFFSET: [0x00627990]
void BattlenetLogin::GetPinInfo(bool* enabled, uint8_t** salt, uint8_t** hash) {
}

// PROVEN: aa631d02 IDENTICAL/NONE
// OFFSET: [0x006279A0]
void BattlenetLogin::SetMatrixInfo(bool enabled, uint8_t width, uint8_t height, uint8_t minDigits, uint8_t maxDigits, bool flipCoords, uint8_t challengeCount, uint64_t seed, const uint8_t* sessionKey, uint32_t sessionKeyLength) {
}

// PROVEN: 502a2077 IDENTICAL/NONE
// OFFSET: [0x00841AE0]
void BattlenetLogin::GetMatrixProof(bool* enabled, const uint8_t** proof) const {
}

// PROVEN: 678e8eb7 IDENTICAL/NONE
// OFFSET: [0x00841AE0]
void BattlenetLogin::GetTokenInfo(bool* enabled, char* token) {
}

// PROVEN: 3e419357 IDENTICAL/NONE
// OFFSET: [0x00762820]
void BattlenetLogin::LogonResult(Grunt::Result result, const uint8_t* sessionKey, uint32_t sessionKeyLength, uint16_t flags) {
}

// PROVEN: 0dc22715 IDENTICAL/NONE
// OFFSET: [0x00762820]
void BattlenetLogin::ReconnectResult(Grunt::Result result, const uint8_t* sessionKey, uint32_t sessionKeyLength, uint16_t flags) {
}

// PROVEN: 1da36603 IDENTICAL/IDENTICAL
// OFFSET: 0x00842700/0x1407FDD80
int32_t BattlenetLogin::GetServerId() {
    return 0;
}

// PROVEN: 94f46783 IDENTICAL/IDENTICAL
// OFFSET: [0x00842700/0x1407FDD80]
const uint8_t* BattlenetLogin::GetVersionChallenge() {
    return NULL;
}

// PROVEN: 6aac27e5 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void BattlenetLogin::Reconnect() {
}

// PROVEN: acfaa9f7 IDENTICAL/NONE
// OFFSET: [0x006705F0]
bool BattlenetLogin::Wait(const char* message, LoginData& loginData) {
    return false;
}

// PROVEN: 6a8f8d4b IDENTICAL/NONE
// OFFSET: [0x00847950]
void BattlenetLogin::ProveVersion(const uint8_t* versionChecksum) {
}

// PROVEN: a443bb74 IDENTICAL/NONE
// OFFSET: [0x00627990]
void BattlenetLogin::FileAccept(bool accept, uint64_t offset) {
}

// PROVEN: c749a7ef IDENTICAL/NONE
// OFFSET: [0x00847950]
void BattlenetLogin::FileComplete(bool success) {
}

// PROVEN: 462e3f66 IDENTICAL/IDENTICAL
// OFFSET: [0x00842700/0x1407FDD80]
int32_t BattlenetLogin::GetSurveyId() {
    return 0;
}

// PROVEN: be1d2a5e IDENTICAL/NONE
// OFFSET: [0x00841AE0]
void BattlenetLogin::SendSurveyData(uint16_t size, uint8_t* data) {
}

// PROVEN: 9428dbf0 IDENTICAL/IDENTICAL
// OFFSET: [0x00401000/0x1407D7E80]
void BattlenetLogin::SendSurveyFailed() {
}

// PROVEN: cb179714 IDENTICAL/DIFFERENT
// OFFSET: 0x009FCE20/0x14090FD50
int32_t BattlenetLogin::GetLoginServerType() {
    return 1;
}

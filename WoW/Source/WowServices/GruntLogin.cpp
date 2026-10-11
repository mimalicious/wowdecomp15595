#include "GruntLogin.h"
#include "Engine/Source/Os/OsSecureRandom.h"
#include "WoW/Source/Net/NetClient/NetClient.h"
#include <cstring>
#include <malloc.h>
#include "Engine/Source/BlizzardCore/SFile2-Core.h"
#include "Engine/Source/Os/OsTcp.h"
#include "Engine/Source/Os/System.h"
#include "Engine/Source/Os/Time.h"
#include "Engine/Source/Os/TimeManager.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/Storm/Source/MD5.h"
#include "WoW/Source/WowServices/InstallLocation.h"
#include "WoW/Source/WowServices/LoginResponse.h"

// INLINED
GruntLogin::MATRIXINPUT::MATRIXINPUT() {
    this->m_valid = false;
}

// INLINED
GruntLogin::MATRIXINPUT::~MATRIXINPUT() {
    memset(this->m_key.m_state, 0, sizeof(this->m_key.m_state));
    this->m_key.m_y = 0;
    this->m_key.m_x = 0;
}

// OFFSET: 0x00626370/0x1402B7FB0
bool GruntLogin::Connected(const NETADDR& address) {
    this->m_loggedOn = true;

    char addressString[32];
    const char* text = OsNetAddrToStr(&address, addressString, sizeof(addressString));
    this->m_loginResponse->UpdateLoginStatus(LOGIN_STATE_RESPONSE_CONNECTED, LOGIN_OK, text, 0);

    return true;
}

// OFFSET: 0x006263B0/0x1402B8000
bool GruntLogin::CantConnect() {
    this->m_loginResponse->UpdateLoginStatus(LOGIN_STATE_FAILED, LOGIN_SERVER_DOWN, NULL, 0);
    this->m_logonStatus = 1;
    this->LogonResult(Grunt::GRUNT_RESULT_CANT_CONNECT, NULL, 0, 0);
    this->m_loggedOn = false;
    this->m_online = false;

    return false;
}

// OFFSET: 0x006263F0/0x1402B8060
bool GruntLogin::Disconnected() {
    this->m_loginResponse->UpdateLoginStatus(LOGIN_STATE_DISCONNECTED, LOGIN_OK, NULL, 0);

    if (!this->m_online) {
        this->m_logonStatus = 1;
        this->LogonResult(Grunt::GRUNT_RESULT_DISCONNECTED, NULL, 0, 0);
    }

    this->m_loggedOn = false;
    this->m_online = false;

    return false;
}

// OFFSET: 0x00626770/0x1402B84F0
void GruntLogin::GetLogonMethod() {
    Grunt::ClientLink::Logon logon = {};

    logon.m_programId = Login::s_fourCCprogramID;

    if (!logon.m_programId) {
        InstallLocation location = GetInstallLocation();
        logon.m_programId = location == INSTALL_LOCATION_TEST || location == INSTALL_LOCATION_BETA ? 'WoWT' : 'WoW';
    }

    logon.m_processor = OsGetProcessorId();
    logon.m_os = OsGetOsId();
    logon.m_versionMajor = 4;
    logon.m_versionMinor = 3;
    logon.m_versionPatch = 4;
    logon.m_build = 15595;
    logon.m_locale = SFile::s_locale;
    logon.m_timeZone = OsGetTimeZone();

    if (this->IsReconnect()) {
        logon.m_accountName = this->m_username;
        logon.m_password = reinterpret_cast<const char*>(this->m_sessionKey);
        this->m_clientLink->LogonStoredSession(logon);
    } else if (this->m_password) {
        this->m_loginResponse->UpdateLoginStatus(LOGIN_STATE_AUTHENTICATING, LOGIN_OK, NULL, 0);

        logon.m_password = this->m_password;
        logon.m_accountName = this->m_username;
        this->m_clientLink->LogonNewSession(logon);

        memset(this->m_password, 0, SStrLen(this->m_password));
        SMemFree(this->m_password, __FILE__, __LINE__, 0);
        this->m_password = NULL;
    }
}

// OFFSET: 0x006268B0/0x1402B8680
void GruntLogin::Logon(const char* loginServer, const char* portal) {
    if (this->m_loggedOn) {
        return;
    }

    this->m_reconnect = false;
    this->m_logonStatus = -1;
    this->m_pinEnabled = false;
    this->m_matrixEnabled = false;
    this->m_tokenEnabled = false;
    this->m_token[0] = '\0';

    this->m_loginResponse->UpdateLoginStatus(LOGIN_STATE_CONNECTING, LOGIN_OK, NULL, 0);
    this->m_clientLink->Connect(loginServer ? loginServer : "us.logon.worldofwarcraft.com:3724");
}

// OFFSET: [#0x008CAB80, #0x008CAC20]
GruntLogin::~GruntLogin() {
    if (this->m_matrixCoordinates) {
        delete[] this->m_matrixCoordinates;
    }

    if (this->m_matrixInput) {
        delete this->m_matrixInput;
    }

    if (this->m_matrixCommitted) {
        delete this->m_matrixCommitted;
    }

    if (this->m_clientLink) {
        delete this->m_clientLink;
    }
}

// PROVEN: f11f5dac IDENTICAL/NONE
// OFFSET: 0x00626240/0x1402B7E40
void GruntLogin::Init(LoginResponse* loginResponse) {
    this->m_loginResponse = loginResponse;
    this->m_clientLink = new (__FILE__, __LINE__) Grunt::ClientLink(*this);
}

// OFFSET: 0x00626290/0x1402B7EA0
GruntLogin::GruntLogin() {
    this->m_clientLink = NULL;
    this->m_matrixChallengeCount = 0;
    this->m_matrixCoordinates = NULL;
    this->m_matrixInput = new (__FILE__, __LINE__) GruntLogin::MATRIXINPUT();
    this->m_matrixCommitted = new (__FILE__, __LINE__) GruntLogin::MATRIXINPUT();
}

// PROVEN: 742ce170 IDENTICAL/NONE
// OFFSET: 0x00626340/0x1402B7F80
void GruntLogin::Logoff() {
    if (this->m_loggedOn) {
        this->m_clientLink->Disconnect();
    }
}

// OFFSET: 0x00626D70/0x1402B8CF0
void GruntLogin::LogonResult(Grunt::Result result, const uint8_t* sessionKey, uint32_t sessionKeyLength, uint16_t flags) {
    if (this->IsReconnect()) {
        this->m_loginResponse->HandleRealmData(1, NULL);
        this->m_clientLink->Disconnect();
        return;
    }

    if (result == Grunt::GRUNT_RESULT_SUCCESS || result == Grunt::GRUNT_RESULT_SUCCESS_SURVEY) {
        this->m_logonStatus = 0;
        this->m_pinFailures = 0;
        memcpy(this->m_sessionKey, sessionKey, sessionKeyLength >= sizeof(this->m_sessionKey) ? sizeof(this->m_sessionKey) : sessionKeyLength);

        if (this->m_clientLink->m_surveyId) {
            this->m_loginResponse->UpdateLoginStatus(LOGIN_STATE_SURVEY, LOGIN_OK, NULL, flags);
        } else {
            this->m_loginResponse->UpdateLoginStatus(LOGIN_STATE_AUTHENTICATED, LOGIN_OK, NULL, flags);
        }

        return;
    }

    this->m_logonStatus = 1;

    switch (result) {
        case Grunt::GRUNT_RESULT_BANNED:
            this->m_loginResponse->UpdateLoginStatus(LOGIN_STATE_FAILED, LOGIN_BANNED, NULL, 0);
            break;

        case Grunt::GRUNT_RESULT_UNKNOWN_ACCOUNT:
        case Grunt::GRUNT_RESULT_INCORRECT_PASSWORD: {
            LOGIN_RESULT loginResult = LOGIN_UNKNOWN_ACCOUNT;

            if (this->m_pinEnabled) {
                loginResult = ++this->m_pinFailures < 3 ? LOGIN_UNKNOWN_ACCOUNT_PIN : LOGIN_UNKNOWN_ACCOUNT_CALL;
            }

            this->m_loginResponse->UpdateLoginStatus(LOGIN_STATE_FAILED, loginResult, NULL, 0);
            break;
        }

        case Grunt::GRUNT_RESULT_ALREADY_ONLINE:
            this->m_loginResponse->UpdateLoginStatus(LOGIN_STATE_FAILED, LOGIN_ALREADYONLINE, NULL, 0);
            break;

        case Grunt::GRUNT_RESULT_NO_TIME:
            this->m_loginResponse->UpdateLoginStatus(LOGIN_STATE_FAILED, LOGIN_NOTIME, NULL, 0);
            break;

        case Grunt::GRUNT_RESULT_DB_BUSY:
            this->m_loginResponse->UpdateLoginStatus(LOGIN_STATE_FAILED, LOGIN_DBBUSY, NULL, 0);
            break;

        case Grunt::GRUNT_RESULT_BAD_VERSION:
            this->m_loginResponse->UpdateLoginStatus(LOGIN_STATE_FAILED, LOGIN_BADVERSION, NULL, 0);
            break;

        case Grunt::GRUNT_RESULT_DOWNLOAD_FILE:
            this->m_loginResponse->UpdateLoginStatus(LOGIN_STATE_DOWNLOADFILE, LOGIN_OK, NULL, 0);
            break;

        case Grunt::GRUNT_RESULT_SUSPENDED:
            this->m_loginResponse->UpdateLoginStatus(LOGIN_STATE_FAILED, LOGIN_SUSPENDED, NULL, 0);
            break;

        case Grunt::GRUNT_RESULT_PARENTAL_CONTROL:
            this->m_loginResponse->UpdateLoginStatus(LOGIN_STATE_FAILED, LOGIN_PARENTALCONTROL, NULL, 0);
            break;

        case Grunt::GRUNT_RESULT_LOCKED_ENFORCED:
            this->m_loginResponse->UpdateLoginStatus(LOGIN_STATE_FAILED, LOGIN_LOCKED_ENFORCED, NULL, 0);
            break;

        case Grunt::GRUNT_RESULT_TRIAL_EXPIRED:
            this->m_loginResponse->UpdateLoginStatus(LOGIN_STATE_FAILED, LOGIN_TRIAL_EXPIRED, NULL, 0);
            break;

        case Grunt::GRUNT_RESULT_ACCOUNT_CONVERTED:
            this->m_loginResponse->UpdateLoginStatus(LOGIN_STATE_FAILED, LOGIN_ACCOUNT_CONVERTED, NULL, 0);
            break;

        case Grunt::GRUNT_RESULT_CHARGEBACK:
            this->m_loginResponse->UpdateLoginStatus(LOGIN_STATE_FAILED, LOGIN_CHARGEBACK, NULL, 0);
            break;

        case Grunt::GRUNT_RESULT_IGR_WITHOUT_BNET:
            this->m_loginResponse->UpdateLoginStatus(LOGIN_STATE_FAILED, LOGIN_IGR_WITHOUT_BNET, NULL, 0);
            break;

        case Grunt::GRUNT_RESULT_GAME_ACCOUNT_LOCKED:
            this->m_loginResponse->UpdateLoginStatus(LOGIN_STATE_FAILED, LOGIN_GAME_ACCOUNT_LOCKED, NULL, 0);
            break;

        case Grunt::GRUNT_RESULT_UNLOCKABLE_LOCK:
            this->m_loginResponse->UpdateLoginStatus(LOGIN_STATE_FAILED, LOGIN_UNLOCKABLE_LOCK, NULL, 0);
            break;

        case Grunt::GRUNT_RESULT_CONVERSION_REQUIRED:
            this->m_loginResponse->UpdateLoginStatus(LOGIN_STATE_FAILED, LOGIN_CONVERSION_REQUIRED, NULL, 0);
            break;

        case Grunt::GRUNT_RESULT_DISCONNECTED:
            this->m_loginResponse->UpdateLoginStatus(LOGIN_STATE_FAILED, LOGIN_DISCONNECTED, NULL, 0);
            break;

        default:
            this->m_loginResponse->UpdateLoginStatus(LOGIN_STATE_FAILED, LOGIN_FAILED, NULL, 0);
            break;
    }
}

void GruntLogin::Update() {
}

void GruntLogin::JoinRealm(uint32_t a1, uint32_t a2, uint32_t a3) {
}

int32_t GruntLogin::GetLoginServerType() {
    return 0;
}

// OFFSET: 0x00626440/0x1402B80D0
void GruntLogin::GetVersionProof(const uint8_t* versionChallenge) {
    if (this->IsReconnect()) {
        uint8_t versionChecksum[20] = {};
        this->m_clientLink->ProveVersion(versionChecksum);
        return;
    }

    memcpy(this->m_versionChallenge, versionChallenge, sizeof(this->m_versionChallenge));
    this->m_loginResponse->UpdateLoginStatus(this->NextSecurityState(LOGIN_STATE_FIRST_SECURITY), LOGIN_OK, NULL, 0);
}

// OFFSET: 0x00625E50
void GruntLogin::EncodePinDigits(uint32_t seed) {
    uint8_t digits[10];

    for (uint32_t i = 0; i < sizeof(digits); i++) {
        digits[i] = i;
    }

    uint8_t* keyMap = this->m_keyMap;

    for (uint32_t count = sizeof(digits); count; count--) {
        uint32_t index = seed % count;
        *keyMap = digits[index];
        seed /= count;
        memmove(&digits[index], &digits[index + 1], count - index - 1);
        keyMap++;
    }
}

// OFFSET: 0x00625EB0/0x1402B7A90
void GruntLogin::SetPinInfo(bool enabled, uint32_t seed, const uint8_t* salt) {
    this->m_pinEnabled = enabled;

    if (enabled) {
        this->EncodePinDigits(seed);
        memcpy(this->m_pinSalt, salt, sizeof(this->m_pinSalt));
    }
}

// PROVEN: 99ca216a IDENTICAL/NONE
// OFFSET: 0x00625F00/0x1402B7AE0
void GruntLogin::GetPinInfo(bool* enabled, uint8_t** salt, uint8_t** hash) {
    *enabled = this->m_pinEnabled;
    *salt = this->m_pinClientSalt;
    *hash = this->m_pinHash;
}

// PROVEN: 103cf58c IDENTICAL/NONE
// OFFSET: 0x00625F30/0x1402B7B00
void GruntLogin::GetMatrixProof(bool* enabled, const uint8_t** proof) const {
    *enabled = this->m_matrixEnabled;
    *proof = this->m_matrixProof;
}

// PROVEN: 36ec3fff IDENTICAL/NONE
// OFFSET: 0x00625F50/0x1402B7B20
void GruntLogin::SetTokenInfo(bool enabled, uint8_t required) {
    this->m_tokenEnabled = enabled;
    this->m_tokenRequired = required;
}

// OFFSET: 0x00625F70/0x1402B7B30
void GruntLogin::GetTokenInfo(bool* enabled, char* token) {
    *enabled = this->m_tokenEnabled;
    SStrCopy(token, this->m_token, sizeof(this->m_token));
}

// OFFSET: 0x00626730/0x1402B84B0
void GruntLogin::ProveVersion(const uint8_t* versionChecksum) {
    if (this->m_loggedOn) {
        this->m_clientLink->ProveVersion(versionChecksum);
        this->m_loginResponse->UpdateLoginStatus(LOGIN_STATE_HANDSHAKING, LOGIN_OK, NULL, 0);
    }
}

// OFFSET: 0x00625FE0/0x1402B7B90
LOGIN_STATE GruntLogin::NextSecurityState(LOGIN_STATE state) {
    switch (state) {
        case LOGIN_STATE_FIRST_SECURITY:
            if (this->m_pinEnabled) {
                return LOGIN_STATE_PIN;
            }

        case LOGIN_STATE_PIN:
            if (this->m_matrixEnabled) {
                return LOGIN_STATE_MATRIX;
            }

        case LOGIN_STATE_MATRIX:
            return this->m_tokenEnabled ? LOGIN_STATE_TOKEN : LOGIN_STATE_CHECKINGVERSIONS;
    }

    return LOGIN_STATE_CHECKINGVERSIONS;
}

// PROVEN: 21ce9a1f IDENTICAL/NONE
// OFFSET: 0x00626070/0x1402B7C20
void GruntLogin::Reconnect() {
    this->m_reconnect = true;
    this->m_clientLink->Connect(this->m_loginResponse->GetLoginServer());
}

// PROVEN: 751f2b5f IDENTICAL/NONE
// OFFSET: 0x006260A0/0x1402B7C50
void GruntLogin::GetRealmList() {
    this->m_clientLink->GetRealmList();
}

// PROVEN: c96abf01 IDENTICAL/NONE
// OFFSET: 0x006260F0/0x1402B7CC0
void GruntLogin::CommitMatrix() {
    GruntLogin::MATRIXINPUT* committed = this->m_matrixCommitted;
    GruntLogin::MATRIXINPUT* input = this->m_matrixInput;

    input->m_valid = committed->m_valid;
    memcpy(&input->m_hmac, &committed->m_hmac, sizeof(input->m_hmac));
    memcpy(&input->m_key, &committed->m_key, sizeof(input->m_key));
}

// PROVEN: b4b6acc3 IDENTICAL/NONE
// OFFSET: 0x00626360/0x1402B7FA0
const char* GruntLogin::GetAccountName() {
    return this->m_username;
}

// PROVEN: 35d7fe2b IDENTICAL/IDENTICAL
// OFFSET: [0x00842700/0x1407FDD80]
uint32_t GruntLogin::GetOutOfBandChallengeType() {
    return 0;
}

// PROVEN: 1d060ab9 IDENTICAL/IDENTICAL
// OFFSET: [0x00842700/0x1407FDD80]
const char* GruntLogin::GetOutOfBandChallengeTitle() {
    return NULL;
}

// PROVEN: c84858b1 IDENTICAL/IDENTICAL
// OFFSET: [0x00842700/0x1407FDD80]
const char* GruntLogin::GetOutOfBandChallengeText() {
    return NULL;
}

// PROVEN: 9240377d IDENTICAL/IDENTICAL
// OFFSET: [0x00842700/0x1407FDD80]
int32_t GruntLogin::GetOutOfBandChallengeTimeLeft() {
    return 0;
}

// PROVEN: a00d5c6f IDENTICAL/NONE
// OFFSET: 0x00626310/0x1402B7F50
int32_t GruntLogin::GetServerId() {
    return this->m_clientLink->m_serverId;
}

// PROVEN: 19a33ba4 IDENTICAL/NONE
// OFFSET: 0x00626320/0x1402B7F60
const uint8_t* GruntLogin::GetVersionChallenge() {
    return this->m_versionChallenge;
}

// PROVEN: 300bfca6 IDENTICAL/NONE
// OFFSET: 0x006261F0/0x1402B7DE0
bool GruntLogin::IsTrialAccount() {
    return (this->m_clientLink->m_accountFlags >> 3) & 1;
}

// PROVEN: 068fadf7 IDENTICAL/NONE
// OFFSET: 0x00626210/0x1402B7E00
bool GruntLogin::IsProPassAccount() {
    return (this->m_clientLink->m_accountFlags >> 23) & 1;
}

// PROVEN: e3a959e0 IDENTICAL/NONE
// OFFSET: 0x00626230/0x1402B7E20
bool GruntLogin::IsGMAccount() {
    return this->m_clientLink->m_accountFlags & 1;
}

// PROVEN: 04220472 IDENTICAL/NONE
// OFFSET: 0x00626330/0x1402B7F70
const uint8_t* GruntLogin::GetKeyMap() {
    return this->m_keyMap;
}

// OFFSET: 0x006264D0/0x1402B8170
void GruntLogin::SetMatrixInfo(bool enabled, uint8_t width, uint8_t height, uint8_t minDigits, uint8_t maxDigits, bool flipCoords, uint8_t challengeCount, uint64_t seed, const uint8_t* sessionKey, uint32_t sessionKeyLength) {
    this->m_matrixEnabled = enabled;

    if (!enabled) {
        return;
    }

    this->m_matrixWidth = width;
    this->m_matrixMaxDigitCount = maxDigits;
    this->m_matrixHeight = height;
    this->m_matrixMinDigitCount = minDigits;
    this->m_matrixChallengeCount = challengeCount;
    this->m_matrixFlipCoords = flipCoords;

    if (this->m_matrixCoordinates) {
        delete[] this->m_matrixCoordinates;
    }

    this->m_matrixCoordinates = new (__FILE__, __LINE__) uint32_t[challengeCount];

    uint32_t cellCount = height * width;
    uint32_t* cells = static_cast<uint32_t*>(alloca(sizeof(uint32_t) * cellCount));

    for (uint32_t i = 0; i < cellCount; i++) {
        cells[i] = i;
    }

    uint64_t value = seed;

    for (uint32_t i = 0; i < challengeCount; i++) {
        uint32_t remaining = cellCount - i;
        uint32_t index = static_cast<uint32_t>(value % remaining);
        value /= remaining;

        this->m_matrixCoordinates[i] = cells[index];
        memmove(&cells[index], &cells[index + 1], sizeof(uint32_t) * (remaining - index) - sizeof(uint32_t));
    }

    MD5_CTX md5;
    MD5Init(&md5);
    MD5Update(&md5, reinterpret_cast<uint8_t*>(&seed), sizeof(seed));
    MD5Update(&md5, sessionKey, sessionKeyLength);
    MD5Final(this->m_matrixKey, &md5);

    GruntLogin::MATRIXINPUT* input = this->m_matrixCommitted;
    memset(input->m_hmac.m_innerPad, 0x36, sizeof(input->m_hmac.m_innerPad));
    memset(input->m_hmac.m_outerPad, 0x5C, sizeof(input->m_hmac.m_outerPad));

    for (uint32_t i = 0; i < sizeof(this->m_matrixKey); i++) {
        input->m_hmac.m_innerPad[i] ^= this->m_matrixKey[i];
        input->m_hmac.m_outerPad[i] ^= this->m_matrixKey[i];
    }

    input->m_hmac.m_hash.Prepare();
    input->m_hmac.m_hash.Process(input->m_hmac.m_innerPad, sizeof(input->m_hmac.m_innerPad));
    SARC4PrepareKey(this->m_matrixKey, sizeof(this->m_matrixKey), &input->m_key);
    input->m_valid = true;

    this->CommitMatrix();

    memset(this->m_matrixProof, 0, sizeof(this->m_matrixProof));
}

// PROVEN: a17f7803 IDENTICAL/NONE
// OFFSET: 0x00626030/0x1402B7BE0
int32_t GruntLogin::GetSurveyId() {
    return this->m_clientLink->m_surveyId;
}

// PROVEN: 675c3cc7 IDENTICAL/NONE
// OFFSET: 0x006260B0/0x1402B7C60
void GruntLogin::RevertMatrix() {
    GruntLogin::MATRIXINPUT* input = this->m_matrixInput;
    GruntLogin::MATRIXINPUT* committed = this->m_matrixCommitted;
    committed->m_valid = input->m_valid;
    committed->m_hmac = input->m_hmac;
    committed->m_key = input->m_key;
}

// OFFSET: 0x00626B00/0x1402B89C0
void GruntLogin::EnterToken(const char* token) {
    SStrCopy(this->m_token, token, sizeof(this->m_token));
    LOGIN_STATE state = this->NextSecurityState(LOGIN_STATE_TOKEN);
    this->m_loginResponse->UpdateLoginStatus(state, LOGIN_OK, NULL, 0);
}

// PROVEN: 0d7cd5dd IDENTICAL/NONE
// OFFSET: 0x00626130/0x1402B7D20
bool GruntLogin::GetMatrixInfo(uint32_t* width, uint32_t* height, uint32_t* minDigits, uint32_t* maxDigits, bool* flipCoords, uint32_t* challengeCount) {
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

// PROVEN: 712fbc37 IDENTICAL/NONE
// OFFSET: 0x00626190/0x1402B7D80
bool GruntLogin::GetMatrixCoordinates(uint32_t index, uint32_t* x, uint32_t* y) {
    if (!this->m_matrixEnabled || !this->m_matrixCoordinates || index >= this->m_matrixChallengeCount) {
        return false;
    }

    uint32_t coordinate = this->m_matrixCoordinates[index];
    *x = coordinate % this->m_matrixWidth;
    *y = coordinate / this->m_matrixWidth;
    return true;
}

// PROVEN: f9ba067d IDENTICAL/NONE
// OFFSET: 0x004C5170/0x1400A8E00
void GruntLogin::MATRIXINPUT::Finish(SHA1::Digest& digest) {
    this->m_hmac.Finish(digest);
    this->m_valid = false;
}

// OFFSET: #0x00468F60
static void ProcessMatrixRC4(uint8_t* data, uint32_t length, SARC4Key* key) {
    uint8_t x = key->m_x;
    uint8_t y = key->m_y;

    for (uint32_t i = 0; i < length; i++) {
        x++;
        uint8_t a = key->m_state[x];
        y += a;
        key->m_state[x] = key->m_state[y];
        key->m_state[y] = a;
        data[i] ^= key->m_state[static_cast<uint8_t>(a + key->m_state[x])];
    }

    key->m_x = x;
    key->m_y = y;
}

// PROVEN: 9fb3d337 IDENTICAL/NONE
// OFFSET: 0x00625FC0/0x1402B7B80
void GruntLogin::FileAccept(bool accept, uint64_t offset) {
    this->m_clientLink->FileStatus(accept, offset);
}

// PROVEN: 97dba191 IDENTICAL/NONE
// OFFSET: 0x00626040/0x1402B7BF0
void GruntLogin::SendSurveyData(uint16_t size, uint8_t* data) {
    this->m_clientLink->SendSurveyData(size, data);
}

// PROVEN: dee73f5a IDENTICAL/NONE
// OFFSET: 0x00626050/0x1402B7C00
void GruntLogin::SendSurveyFailed() {
    this->m_clientLink->SendSurveyFailed();
}

// PROVEN: 9f68111c IDENTICAL/NONE
// OFFSET: 0x00626060/0x1402B7C10
void GruntLogin::FileComplete(bool success) {
    this->m_clientLink->FileComplete();
}

// OFFSET: 0x00626910/0x1402B8700
bool GruntLogin::Wait(const char* message, LoginData& loginData) {
    while (this->m_logonStatus == -1) {
        OsSleep(100);
    }

    if (this->m_logonStatus) {
        memset(&loginData, 0, sizeof(loginData));
    } else {
        loginData.m_loginServerID = 0;
        SStrCopy(loginData.m_account, this->m_username, sizeof(loginData.m_account));
        memcpy(loginData.m_sessionKey, this->m_sessionKey, sizeof(loginData.m_sessionKey));
    }

    return this->m_logonStatus == 0;
}

// OFFSET: 0x006269B0/0x1402B87E0
void GruntLogin::EnterPIN(int32_t count, uint8_t* digits) {
    char pin[12];

    for (int32_t i = 0; i < count; i++) {
        pin[i] = digits[i] + '0';
    }

    pin[count] = '\0';
    OsSecureRandom(this->m_pinClientSalt, sizeof(this->m_pinClientSalt));

    SHA1 sha;
    sha.Prepare();
    sha.Process(this->m_pinSalt, sizeof(this->m_pinSalt));
    sha.Process(pin);

    SHA1::Digest digest;
    sha.Finish(digest);

    sha.Prepare();
    sha.Process(this->m_pinClientSalt, sizeof(this->m_pinClientSalt));
    sha.Process(&digest, sizeof(digest));
    sha.Finish(*reinterpret_cast<SHA1::Digest*>(this->m_pinHash));

    this->m_loginResponse->UpdateLoginStatus(this->NextSecurityState(LOGIN_STATE_PIN), LOGIN_OK, NULL, 0);
}

// OFFSET: 0x00626A80/0x1402B8910
void GruntLogin::EnterMatrix(uint8_t value) {
    GruntLogin::MATRIXINPUT* input = this->m_matrixCommitted;
    ProcessMatrixRC4(&value, 1, &input->m_key);
    input->m_hmac.Process(&value, 1);
}

// OFFSET: 0x00626AC0/0x1402B8960
void GruntLogin::FinalizeMatrix() {
    this->m_matrixInput->Finish(*reinterpret_cast<SHA1::Digest*>(this->m_matrixProof));
    this->m_loginResponse->UpdateLoginStatus(this->NextSecurityState(LOGIN_STATE_MATRIX), LOGIN_OK, NULL, 0);
}

// OFFSET: 0x00626C50/0x1402B8B90
void GruntLogin::ReconnectResult(Grunt::Result result, const uint8_t* sessionKey, uint32_t sessionKeyLength, uint16_t flags) {
    if (!this->IsReconnect()) {
        this->m_logonStatus = 1;
        this->m_loginResponse->UpdateLoginStatus(LOGIN_STATE_FAILED, LOGIN_FAILED, NULL, 0);
        return;
    }

    switch (result) {
        case Grunt::GRUNT_RESULT_SUCCESS:
        case Grunt::GRUNT_RESULT_SUCCESS_SURVEY:
            this->m_clientLink->GetRealmList();
            this->m_loginResponse->UpdateLoginStatus(LOGIN_STATE_RESPONSE_CONNECTED, LOGIN_OK, NULL, 0);
            return;

        case Grunt::GRUNT_RESULT_BAD_VERSION:
            this->m_loginResponse->HandleRealmData(2, NULL);
            break;

        case Grunt::GRUNT_RESULT_DOWNLOAD_FILE:
            this->m_loginResponse->HandleRealmData(3, NULL);
            break;

        default:
            this->m_loginResponse->HandleRealmData(1, NULL);
            break;
    }

    this->m_clientLink->Disconnect();
}

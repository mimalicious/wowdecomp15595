#include "Login.h"
#include <cstdlib>
#include <cstring>
#include "Engine/Source/Base/CDataStore.h"
#include "Engine/Source/Storm/H/storm.h"
#include "WoW/Source/WowServices/LoginResponse.h"

// STATIC: 0x00CA06F8
const char* g_LoginResultStringNames[41] = {
    "LOGIN_OK",
    "LOGIN_INVALID_CHALLENGE_MESSAGE",
    "LOGIN_SRP_ERROR",
    "LOGIN_INVALID_PROOF_MESSAGE",
    "LOGIN_BAD_SERVER_PROOF",
    "LOGIN_INVALID_RECODE_MESSAGE",
    "LOGIN_BAD_SERVER_RECODE_PROOF",
    "LOGIN_UNKNOWN_ACCOUNT",
    "LOGIN_UNKNOWN_ACCOUNT_PIN",
    "LOGIN_UNKNOWN_ACCOUNT_CALL",
    "LOGIN_INCORRECT_PASSWORD",
    "LOGIN_FAILED",
    "LOGIN_SERVER_DOWN",
    "LOGIN_BANNED",
    "LOGIN_BADVERSION",
    "LOGIN_ALREADYONLINE",
    "LOGIN_NOTIME",
    "LOGIN_DBBUSY",
    "LOGIN_SUSPENDED",
    "LOGIN_PARENTALCONTROL",
    "LOGIN_LOCKED_ENFORCED",
    "DISCONNECTED",
    "LOGIN_ACCOUNT_CONVERTED",
    "LOGIN_ANTI_INDULGENCE",
    "LOGIN_EXPIRED",
    "LOGIN_TRIAL_EXPIRED",
    "LOGIN_NO_GAME_ACCOUNT",
    "LOGIN_AUTH_OUTAGE",
    "LOGIN_GAME_ACCOUNT_LOCKED",
    "LOGIN_NO_BATTLENET_MANAGER",
    "LOGIN_NO_BATTLENET_APPLICATION",
    "LOGIN_MALFORMED_ACCOUNT_NAME",
    "LOGIN_USE_GRUNT",
    "LOGIN_TOO_FAST",
    "LOGIN_CHARGEBACK",
    "LOGIN_IGR_WITHOUT_BNET",
    "LOGIN_UNLOCKABLE_LOCK",
    "LOGIN_CONVERSION_REQUIRED",
    "LOGIN_UNABLE_TO_DOWNLOAD_MODULE",
    "LOGIN_NO_GAME_ACCOUNTS_IN_REGION",
    "LOGIN_ACCOUNT_LOCKED",
};

// STATIC: 0x00CA07A0
const char* g_LoginStateStringNames[20] = {
    "LOGIN_STATE_INITIALIZED",
    "LOGIN_STATE_CONNECTING",
    "LOGIN_STATE_HANDSHAKING",
    "LOGIN_STATE_AUTHENTICATING",
    "LOGIN_STATE_AUTHENTICATED",
    "LOGIN_STATE_FAILED",
    "LOGIN_STATE_DOWNLOADFILE",
    "LOGIN_STATE_FIRST_SECURITY",
    "LOGIN_STATE_PIN",
    "LOGIN_STATE_PIN_WAIT",
    "LOGIN_STATE_MATRIX",
    "LOGIN_STATE_MATRIX_WAIT",
    "LOGIN_STATE_TOKEN",
    "LOGIN_STATE_TOKEN_WAIT",
    "LOGIN_STATE_CHECKINGVERSIONS",
    "RESPONSE_CONNECTED",
    "LOGIN_STATE_DISCONNECTED",
    "LOGIN_STATE_SURVEY",
    "LOGIN_STATE_OUTOFBAND_CHALLENGE",
    "LOGIN_STATE_OUTOFBAND_CHALLENGE_WAIT",
};

// STATIC: #0x00B38CD4
uint32_t Login::s_fourCCprogramID;

// OFFSET: 0x004C5090/0x1401001A0
void LoginResponse::UpdateLoginStatus(LOGIN_STATE state, LOGIN_RESULT result, const char* addressString, uint16_t flags) {
    char stateString[64];
    char resultString[64];

    this->m_loginState = state;
    this->m_loginResult = result;

    if (static_cast<uint32_t>(state) > LOGIN_STATE_OUTOFBAND_CHALLENGE_WAIT) {
        SStrPrintf(stateString, sizeof(stateString), "LOGIN_STATE_UNKNOWN_%d", state);
    } else {
        SStrCopy(stateString, g_LoginStateStringNames[state], sizeof(stateString));
    }

    if (static_cast<uint32_t>(result) > LOGIN_ACCOUNT_LOCKED) {
        SStrPrintf(resultString, sizeof(resultString), "LOGIN_UNKNOWN_%d", result);
    } else {
        SStrCopy(resultString, g_LoginResultStringNames[result], sizeof(resultString));
    }

    this->LoginServerStatus(state, result, addressString, stateString, resultString, flags);
}

// OFFSET: 0x004C51D0
void Login::GetFileStatus(const char* fileName, uint64_t size, const uint8_t* md5) {
    this->m_fileSize = size;
    SStrCopy(this->m_fileName, fileName, sizeof(this->m_fileName));
    memcpy(this->m_fileMD5, md5, sizeof(this->m_fileMD5));
    this->m_loginResponse->UpdateLoginStatus(LOGIN_STATE_DOWNLOADFILE, LOGIN_OK, NULL, 0);
}

// PROVEN: 9d6f7337 IDENTICAL/NONE
// OFFSET: 0x004C4F70/0x140100050
bool Login::OnlineIdle() {
    this->m_online = true;
    return true;
}

// PROVEN: cd701cfd IDENTICAL/NONE
// OFFSET: 0x004C5250/0x140100370
void Login::SetLogonCreds(const char* accountName, const char* password) {
    SStrCopy(this->m_username, accountName, sizeof(this->m_username));

    if (this->m_password) {
        memset(this->m_password, 0, SStrLen(this->m_password));
        SMemFree(this->m_password, __FILE__, __LINE__, 0);
    }

    this->m_password = SStrDupA(password, __FILE__, __LINE__);
}

// OFFSET: 0x004C52E0
void Login::SetRawAccountName(const char* accountName) {
    SStrCopy(this->m_rawUsername, accountName, sizeof(this->m_rawUsername));
}

// OFFSET: [#0x004656B0, #0x00465750]
Login::~Login() {
    if (this->m_password) {
        memset(this->m_password, 0, SStrLen(this->m_password));
        SMemFree(this->m_password, __FILE__, __LINE__, 0);
    }

    if (this->m_matrixData) {
        delete[] this->m_matrixData;
        this->m_matrixData = NULL;
    }

    this->m_loginResponse = NULL;
}

// OFFSET: #0x00465730
bool Login::IsReconnect() {
    return this->m_reconnect;
}

// OFFSET: #0x00465740
bool Login::IsLoggedOn() {
    return this->m_loggedOn;
}

// OFFSET: 0x004C5320/0x140100460
Login::Login() {
    this->m_loginResponse = NULL;
    this->m_password = NULL;
    this->m_loggedOn = false;
    this->m_online = false;
    this->m_logonStatus = -1;
    this->m_pinFailures = 0;
    this->m_reconnect = false;
    memset(this->m_username, 0, sizeof(this->m_username));
    memset(this->m_rawUsername, 0, sizeof(this->m_rawUsername));
    memset(this->unk_0A0D, 0, sizeof(this->unk_0A0D));
    memset(this->m_sessionKey, 0, sizeof(this->m_sessionKey));
    this->m_fileSize = 0;
    memset(this->m_fileMD5, 0, sizeof(this->m_fileMD5));
    memset(this->m_fileName, 0, sizeof(this->m_fileName));
    memset(this->unk_105D, 0, sizeof(this->unk_105D));
    memset(this->unk_1068, 0, sizeof(this->unk_1068));
    this->m_matrixEnabled = false;
    this->m_matrixWidth = 0;
    this->m_matrixHeight = 0;
    this->m_matrixMinDigitCount = 0;
    this->m_matrixMaxDigitCount = 0;
    this->m_matrixFlipCoords = false;
    this->m_matrixChallengeCount = 0;
    this->m_matrixData = NULL;
    memset(this->unk_10A8, 0, sizeof(this->unk_10A8));
    this->m_matrixSeed = 0;
    memset(this->unk_10C0, 0, sizeof(this->unk_10C0));
    this->m_tokenRequired = false;
    this->unk_10D5 = 0;
    memset(this->unk_10D6, 0, sizeof(this->unk_10D6));
    this->unk_10E8 = 0;
}

// OFFSET: #0x004DA000
LoginResponse::~LoginResponse() {
}

bool LoginResponse::GetCdKeys(Grunt::CdKey& cdKey) {
    return false;
}

void LoginResponse::FileData(const uint8_t* data, uint32_t size) {
}

// PROVEN: 93153f2a IDENTICAL/NONE
// OFFSET: 0x004C4F80/0x140100060
bool Login::GetCdKeys(Grunt::CdKey& cdKey) {
    return this->m_loginResponse->GetCdKeys(cdKey);
}

// OFFSET: 0x004C5760/0x140100950
void LoginResponse::HandleRealmData(uint32_t result, CDataStore* msg) {
    if (this->m_realmList.m_count) {
        this->m_realmList.Clear();
    }

    if (result || !msg) {
        this->RealmEnumCallback(result);
        return;
    }

    uint16_t count = 0;
    msg->Get(count);
    this->m_realmList.SetCount(count);

    for (uint32_t i = 0; i < count; i++) {
        REALM_INFO* realm = &this->m_realmList.m_data[i];
        realm->m_id = i;

        msg->Get(realm->m_type);
        msg->Get(realm->m_otherFlags);
        msg->Get(realm->m_flags);
        msg->GetString(realm->m_realmName, sizeof(realm->m_realmName));

        char address[64];
        msg->GetString(address, sizeof(address));

        realm->m_addresses[0].Clear();
        realm->m_addresses[1].Clear();
        realm->m_ports[0] = 0;
        realm->m_ports[1] = 0;

        char* port = strrchr(address, ':');

        if (port) {
            *port = '\0';
            realm->m_ports[0] = static_cast<uint16_t>(strtol(port + 1, NULL, 0));
        }

        realm->m_addresses[0].Parse(address);
        msg->Get(realm->m_population);
        msg->Get(realm->m_numCharacters);
        msg->Get(realm->m_category);

        if (this->GetLoginServerType() == 1) {
            msg->Get(realm->m_regionId);
            msg->Get(realm->m_battlegroupId);
            msg->Get(realm->m_realmId);
            msg->Get(realm->m_build);
        } else {
            uint8_t realmId;
            msg->Get(realmId);
            realm->m_realmId = realmId;
            realm->m_regionId = 0;
            realm->m_battlegroupId = 0;
            realm->m_build = 0;
        }

        if (realm->m_flags & 0x4) {
            msg->Get(realm->m_versionMajor);
            msg->Get(realm->m_versionMinor);
            msg->Get(realm->m_versionRevision);
            msg->Get(realm->m_versionBuild);
        } else {
            realm->m_versionMajor = 0;
            realm->m_versionMinor = 0;
            realm->m_versionRevision = 0;
            realm->m_versionBuild = 0;
        }

        if (msg->m_read > msg->m_size) {
            break;
        }

        if (realm->m_population == 600.0f) {
            realm->m_flags |= 0x20;
            realm->m_population = 0.0f;
        } else if (realm->m_population == 200.0f) {
            realm->m_flags |= 0x40;
            realm->m_population = 0.001f;
        } else if (realm->m_population == 400.0f) {
            realm->m_flags |= 0x80;
            realm->m_population = 8.0f;
        }

        char* name = realm->m_realmName;
        int32_t length = 0;

        while (name[length]) {
            switch (name[length]) {
                case '"':
                case '*':
                case '/':
                case ':':
                case '<':
                case '>':
                case '?':
                case '\\':
                case '|':
                    name[length] = ' ';
                    break;
            }

            length++;
        }

        while (length > 0) {
            bool trim = false;

            switch (name[length - 1]) {
                case ' ':
                case '"':
                case '*':
                case '.':
                case '/':
                case ':':
                case '<':
                case '>':
                case '?':
                case '\\':
                case '|':
                    trim = true;
                    break;
            }

            if (!trim) {
                break;
            }

            length--;
        }

        name[length] = '\0';
    }

    msg->Get(this->m_realmListTrailer);

    if (msg->m_read <= msg->m_size && msg->IsRead()) {
        this->RealmEnumCallback(result);
        return;
    }

    if (this->m_realmList.m_count) {
        this->m_realmList.Clear();
    }

    this->RealmEnumCallback(4);
}

// OFFSET: 0x004C5BF0
void Login::RealmListResult(CDataStore* msg) {
    if (!msg || msg->m_read == -1) {
        this->m_loginResponse->HandleRealmData(4, NULL);
    } else {
        this->m_loginResponse->HandleRealmData(0, msg);
    }
}

// PROVEN: 34d2314c IDENTICAL/NONE
// OFFSET: 0x004C4FA0/0x140100070
void Login::FileData(const uint8_t* data, uint32_t size) {
    this->m_loginResponse->FileData(data, size);
}

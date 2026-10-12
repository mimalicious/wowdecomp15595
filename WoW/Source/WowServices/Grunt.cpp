#include "Grunt.h"
#include <cstdio>
#include <cstring>
#include <malloc.h>
#include "Engine/Source/Os/OsSecureRandom.h"
#include "Engine/Source/Os/TimeManager.h"
#include "Engine/Source/Storm/Source/MD5.h"
#include "WoW/Source/WowServices/SRP6_Random.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/BlizzardCore/Source/Packages/Util/SHA1.h"
#include "WoW/Source/Client.h"

// STATIC: 0x00D27600/0x140BE5C20
static Grunt::Command<Grunt::ClientLink> s_authCmds[] = {
    { 0x00, {}, "ClientLink::CMD_AUTH_LOGON_CHALLENGE", &Grunt::ClientLink::CmdAuthLogonChallenge },
    { 0x01, {}, "ClientLink::CMD_AUTH_LOGON_PROOF", &Grunt::ClientLink::CmdAuthLogonProof },
    { 0x02, {}, "ClientLink::CMD_AUTH_RECONNECT_CHALLENGE", &Grunt::ClientLink::CmdAuthReconnectChallenge },
    { 0x03, {}, "ClientLink::CMD_AUTH_RECONNECT_PROOF", &Grunt::ClientLink::CmdAuthReconnectProof },
    { 0x10, {}, "ClientLink::CMD_REALM_LIST", &Grunt::ClientLink::CmdRealmList },
    { 0x30, {}, "ClientLink::CMD_XFER_INITIATE", &Grunt::ClientLink::CmdXferInitiate },
    { 0x31, {}, "ClientLink::CMD_XFER_DATA", &Grunt::ClientLink::CmdXferData },
};

static char s_accountNameUnDecorated[1280];

static const uint8_t s_serverAuthKey[16] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10 };

// STATIC: 0x00D27580/0x140BE5B90
static const char* s_linkTypeNames[] = { "Grunt", "Battlenet" };

// STATIC: 0x00D27674
static Grunt::Command<Grunt::ServerLink> s_serverCmds[] = {
    { 0x00, {}, "ServerLink::CMD_GRUNT_AUTH_CHALLENGE", &Grunt::ServerLink::CmdGruntAuthChallenge },
    { 0x02, {}, "ServerLink::CMD_GRUNT_AUTH_VERIFY", &Grunt::ServerLink::CmdGruntAuthVerify },
    { 0x10, {}, "ServerLink::CMD_GRUNT_CONN_PING", &Grunt::ServerLink::CmdGruntConnPing },
    { 0x11, {}, "ServerLink::CMD_GRUNT_CONN_PONG", &Grunt::ServerLink::CmdGruntConnPong },
    { 0x20, {}, "ServerLink::CMD_GRUNT_HELLO", &Grunt::ServerLink::CmdGruntHello },
    { 0x21, {}, "ServerLink::CMD_GRUNT_PROVESESSION", &Grunt::ServerLink::CmdGruntProveSession },
    { 0x24, {}, "ServerLink::CMD_GRUNT_KICK", &Grunt::ServerLink::CmdGruntKick },
    { 0x29, {}, "ServerLink::CMD_GRUNT_PCWARNING", &Grunt::ServerLink::CmdGruntPcWarning },
    { 0x41, {}, "ServerLink::CMD_GRUNT_STRINGS", &Grunt::ServerLink::CmdGruntStrings },
    { 0x44, {}, "ServerLink::CMD_GRUNT_SUNKENUPDATE", &Grunt::ServerLink::CmdGruntSunkenUpdate },
    { 0x46, {}, "ServerLink::CMD_GRUNT_SUNKEN_ONLINE", &Grunt::ServerLink::CmdGruntSunkenOnline },
    { 0x2C, {}, "ServerLink::CMD_GRUNT_CAISTIMEUPDATE", &Grunt::ServerLink::CmdGruntCaisTimeUpdate },
};

// STATIC: 0x00D27EC8
static Grunt::ServerLink::Feature s_serverFeatures[] = {
    { 0x00030001, 14 },
    { 0x00030002, 14 },
    { 0x00030003, 14 },
    { 0x00030004, 13 },
    { 0x00030005, 1 },
    { 0x00030006, 14 },
    { 0x00040001, 1 },
    { 0x00040002, 5 },
};

// INLINED
Grunt::Pending::Pending(uint32_t timeout) {
    this->m_timeout = timeout;
    this->unk_0008 = 0;
}

// INLINED
void Grunt::Timer::Shutdown() {
    if (!this->m_thread.Valid()) {
        return;
    }

    this->m_event.Set();
    this->m_thread.Wait(0xFFFFFFFF);
    this->m_thread.Close();

    this->m_lock.Enter();
    this->m_list.UnlinkAll();
    this->m_lock.Leave();
}

// OFFSET: 0x00B02B00
void Grunt::ClientLink::Send(CDataStore& msg) {
    this->m_lock.Enter();

    if (this->m_connection) {
        uint32_t size = msg.m_size;
        void* data;
        msg.GetDataInSitu(data, size);
        this->m_connection->SendRaw(static_cast<uint8_t*>(data), size);
    }

    this->m_lock.Leave();
}

// OFFSET: 0x00B02B50/0x1408C4AA0
void Grunt::ClientLink::Shutdown() {
    this->m_lock.Enter();

    if (this->m_connection) {
        this->m_connection->Disconnect();
        this->m_connection->SetResponse(NULL, false);
        this->m_connection->Release();
        this->m_connection = NULL;
    }

    this->m_lock.Leave();
}

// OFFSET: 0x00B02BA0/0x1408C4B10
void Grunt::ClientLink::Disconnect() {
    this->m_lock.Enter();

    if (this->m_connection) {
        this->m_connection->Disconnect();
    }

    this->m_lock.Leave();
}

// OFFSET: 0x00B02BD0/0x1408C4B50
void Grunt::ClientLink::SetState(int32_t state) {
    this->m_lock.Enter();
    this->m_state = state;
    this->m_lock.Leave();
}

// OFFSET: 0x00B02C00/0x1408C4BA0
void Grunt::ClientLink::Connect(const char* address) {
    if (this->m_state) {
        return;
    }

    this->SetState(1);

    this->m_connection = new (__FILE__, __LINE__) WowConnection(this, NULL);
    this->m_connection->SetType(WOWC_TYPE_STREAM);

    if (SStrChr(address, ':')) {
        this->m_connection->Connect(address, 5000);
    } else {
        this->m_connection->Connect(address, 3724, 5000);
    }
}

// OFFSET: 0x00B03E80/0x1408C6420
void Grunt::ClientLink::WCConnected(WowConnection* connection, WowConnection* inbound, uint32_t timeStamp, const NETCONNADDR* addr) {
    this->m_lock.Enter();
    this->SetState(2);
    bool failed = !this->m_clientResponse->Connected(addr->peerAddr);
    this->m_localAddress = OsNetAddrGetAddress(&addr->selfAddr, NULL);
    this->m_lock.Leave();

    if (failed) {
        this->Disconnect();
    }
}

// OFFSET: 0x00B03F10/0x1408C64E0
void Grunt::ClientLink::WCDisconnected(WowConnection* connection, uint32_t timeStamp, const NETCONNADDR* addr) {
    this->m_lock.Enter();
    this->Shutdown();
    this->SetState(0);
    this->m_lock.Leave();

    if (this->m_clientResponse->Disconnected()) {
        delete this;
    }
}

// OFFSET: 0x00B03F70/0x1408C6560
void Grunt::ClientLink::WCCantConnect(WowConnection* connection, uint32_t timeStamp, const NETCONNADDR* addr) {
    this->m_lock.Enter();
    this->Shutdown();
    this->SetState(0);
    this->m_lock.Leave();

    if (this->m_clientResponse->CantConnect()) {
        delete this;
    }
}

// OFFSET: 0x00B03FD0/0x1408C65E0
void Grunt::ClientLink::PackLogon(CDataStore& msg, const Grunt::ClientLink::Logon& logon) {
    uint32_t start = msg.m_size;

    msg.Put(static_cast<uint16_t>(0));
    msg.Put(logon.m_programId);
    msg.Put(logon.m_versionMajor);
    msg.Put(logon.m_versionMinor);
    msg.Put(logon.m_versionPatch);
    msg.Put(logon.m_build);
    msg.Put(logon.m_processor);
    msg.Put(logon.m_os);
    msg.Put(logon.m_locale);
    msg.Put(static_cast<uint32_t>(logon.m_timeZone));
    msg.Put(this->m_localAddress);

    uint8_t length = SStrLen(this->m_accountName);
    msg.Put(length);
    msg.PutData(this->m_accountName, length);

    msg.Set(start, static_cast<uint16_t>(msg.m_size - start - 2));
}

// OFFSET: 0x00B04A10/0x1408C7330
void Grunt::ClientLink::LogonNewSession(const Grunt::ClientLink::Logon& logon) {
    this->SetState(3);

    SStrCopy(this->m_accountName, logon.m_accountName, sizeof(this->m_accountName));
    SStrUpper(this->m_accountName);

    char* password = static_cast<char*>(alloca(SStrLen(logon.m_password) + 1));
    SStrCopy(password, logon.m_password, STORM_MAX_STR);
    SStrUpper(password);

    SStrCopy(s_accountNameUnDecorated, this->m_accountName, STORM_MAX_STR);

    char* decoration = SStrChr(s_accountNameUnDecorated, '#');

    if (decoration) {
        *decoration = '\0';
    }

    this->m_srpClient.BeginAuthentication(s_accountNameUnDecorated, password);

    CDataStoreCache<1024> msg;
    msg.Put(static_cast<uint8_t>(0));
    msg.Put(static_cast<uint8_t>(8));
    this->PackLogon(msg, logon);
    msg.Finalize();
    this->Send(msg);

    memset(password, 0, SStrLen(password));
}

// OFFSET: 0x00B04C70/0x1408C75B0
void Grunt::ClientLink::LogonStoredSession(const Grunt::ClientLink::Logon& logon) {
    this->SetState(3);

    SStrCopy(this->m_accountName, logon.m_accountName, sizeof(this->m_accountName));
    SStrUpper(this->m_accountName);
    memcpy(this->m_reconnectKey, logon.m_password, sizeof(this->m_reconnectKey));

    CDataStoreCache<1024> msg;
    msg.Put(static_cast<uint8_t>(2));
    msg.Put(static_cast<uint8_t>(8));
    this->PackLogon(msg, logon);
    msg.Finalize();
    this->Send(msg);
}

// OFFSET: 0x00B04E20/0x1408C7790
void Grunt::ClientLink::SendSurveyData(uint16_t size, uint8_t* data) {
    CDataStoreCache<1024> msg;
    msg.Put(static_cast<uint8_t>(4));
    msg.Put(this->m_surveyId);
    msg.Put(static_cast<uint8_t>(0));
    msg.Put(size);
    msg.PutData(data, size);
    msg.Finalize();
    this->Send(msg);
}

// OFFSET: 0x00B04F90/0x1408C7900
void Grunt::ClientLink::SendSurveyFailed() {
    CDataStoreCache<1024> msg;
    msg.Put(static_cast<uint8_t>(4));
    msg.Put(this->m_surveyId);
    msg.Put(static_cast<uint8_t>(1));
    msg.Put(static_cast<uint16_t>(0));
    msg.Finalize();
    this->Send(msg);
}

// OFFSET: 0x00B05480/0x1408C7E40
void Grunt::Timer::Insert(Grunt::Timer::Event& event) {
    this->m_lock.Enter();

    event.m_nextTime = event.m_interval + OsGetAsyncTimeMs();

    for (Grunt::Timer::Event* existing = this->m_list.Head(); !this->m_list.IsEnd(existing); existing = this->m_list.RawNext(existing)) {
        if (static_cast<int32_t>(event.m_nextTime - existing->m_nextTime) < 0) {
            this->m_list.LinkNode(&event, 2, existing);
            this->m_lock.Leave();
            return;
        }
    }

    this->m_list.LinkToTail(&event);
    this->m_lock.Leave();
}

// OFFSET: 0x00B05740/0x1408C81D0
Grunt::Pending::~Pending() {
}

// OFFSET: 0x00B056B0/0x1408C80F0
int32_t Grunt::Timer::Pump() {
    this->m_lock.Enter();

    Grunt::Timer::Event* event = this->m_list.Head();

    if (!event) {
        this->m_lock.Leave();
        return 100;
    }

    int32_t remaining = event->m_nextTime - OsGetAsyncTimeMs();

    if (remaining > 0) {
        this->m_lock.Leave();
        return remaining;
    }

    this->m_list.UnlinkNode(event);
    this->m_lock.Leave();

    event->Call();
    this->Insert(*event);

    return 100;
}

// OFFSET: 0x00B05E70/0x1408C8A60
Grunt::Timer::~Timer() {
    this->Shutdown();
}

// OFFSET: 0x00B05F10/0x1408C8B30
void Grunt::Pending::CheckExpired(bool all) {
    uint32_t now = OsGetAsyncTimeMs();

    TSExplicitList<Grunt::Pending::Data, offsetof(Grunt::Pending::Data, m_link)> expired;

    this->m_lock.Enter();

    Grunt::Pending::Data* data = this->m_list.Head();

    while (!this->m_list.IsEnd(data)) {
        if (!all) {
            if (!data->m_time) {
                data = this->m_list.RawNext(data);
                continue;
            }

            if (now - data->m_time < this->m_timeout) {
                break;
            }
        }

        this->m_list.UnlinkNode(data);
        expired.LinkToTail(data);
        data = this->m_list.RawNext(data);
    }

    this->m_lock.Leave();

    for (data = expired.Head(); data; data = expired.DeleteNode(data)) {
        this->Expired(*data);
    }
}

// OFFSET: 0x00B05590/0x1408C7F80
Grunt::Pending::Data* Grunt::Pending::Remove(uint32_t id) {
    this->m_lock.Enter();

    for (Grunt::Pending::Data* data = this->m_list.Head(); data; data = this->m_list.Next(data)) {
        if (data->m_id == id) {
            this->m_list.UnlinkNode(data);
            this->m_lock.Leave();

            return data;
        }
    }

    this->m_lock.Leave();

    return NULL;
}

// OFFSET: 0x00B06A10/0x1408C9AD0
void Grunt::ClientLink::Call() {
    this->CheckExpired(false);

    this->m_lock.Enter();

    if (this->m_state == 2) {
        this->m_clientResponse->GetLogonMethod();
    } else if (this->m_state == 6 && !this->m_clientResponse->OnlineIdle()) {
        this->Disconnect();
    }

    this->m_lock.Leave();
}

// OFFSET: [0x00B06A90/0x1408C9B50, 0x00B06CF0]
Grunt::ClientLink::~ClientLink() {
    this->m_timer.Shutdown();
    this->Shutdown();
}

// OFFSET: 0x00B05E40/0x1408C8A20
uint32_t STORMAPI Grunt::Timer::ThreadProc(void* param) {
    Grunt::Timer* timer = static_cast<Grunt::Timer*>(param);
    uint32_t timeout;

    do {
        timeout = timer->Pump();
    } while (timer->m_event.Wait(timeout));

    return 1;
}

// OFFSET: 0x00B06B80/0x1408C9CA0
Grunt::Timer::Timer() : m_event(0, 0) {
    SThread::Create(Grunt::Timer::ThreadProc, this, this->m_thread, const_cast<char*>("GruntTimerEvt"), 0);
}

// OFFSET: 0x00B06BE0/0x1408C9D20
Grunt::ClientLink::ClientLink(Grunt::ClientResponse& clientResponse) : Grunt::Pending(18000), m_serverId(0), m_accountFlags(0), m_surveyId(0), m_localAddress(0), m_connection(NULL), m_clientResponse(&clientResponse) {
    this->SetState(0);

    if (this->m_timer.m_thread.Valid()) {
        this->m_interval = 100;
        this->m_timer.Insert(*this);
    }
}

void Grunt::ClientLink::WCMessageReady(WowConnection* connection, uint32_t timeStamp, CDataStore* msg) {
}

void Grunt::ClientLink::Expired(Grunt::Pending::Data& data) {
}

// OFFSET: 0x00B02CD0/0x1408C4C90
bool CanReadNumBytes(CDataStore& msg, uint32_t bytes) {
    return msg.m_read <= msg.m_size && msg.m_size - msg.m_read >= bytes;
}

// OFFSET: 0x00B03420/0x1408C5660
int32_t Grunt::ClientLink::CmdRealmList(CDataStore& msg) {
    if (msg.m_read > msg.m_size || msg.m_size - msg.m_read < 2) {
        return 0;
    }

    uint16_t size;
    msg.Get(size);

    if (msg.m_read > msg.m_size || msg.m_size - msg.m_read < size) {
        return 0;
    }

    uint32_t start = msg.m_read;

    uint32_t unknown;
    msg.Get(unknown);

    uint32_t realmStart = msg.m_read;

    uint16_t count;
    msg.Get(count);

    for (uint32_t i = 0; i < count; i++) {
        uint8_t type;
        uint8_t locked;
        uint8_t flags;
        char name[256];
        char address[256];
        float population;
        uint8_t characters;
        uint8_t timeZone;
        uint8_t realmId;

        msg.Get(type);
        msg.Get(locked);
        msg.Get(flags);
        msg.GetString(name, sizeof(name));
        msg.GetString(address, sizeof(address));
        msg.Get(population);
        msg.Get(characters);
        msg.Get(timeZone);
        msg.Get(realmId);

        if (flags & 0x4) {
            uint8_t versionMajor;
            uint8_t versionMinor;
            uint8_t versionRevision;
            uint16_t build;

            msg.Get(versionMajor);
            msg.Get(versionMinor);
            msg.Get(versionRevision);
            msg.Get(build);
        }

        if (msg.m_read > msg.m_size) {
            return 1;
        }
    }

    uint16_t trailer;
    msg.Get(trailer);

    uint32_t end = msg.m_read;

    if (end > msg.m_size || end - start != size) {
        return 1;
    }

    msg.m_read = realmStart;
    this->m_clientResponse->RealmListResult(&msg);
    msg.m_read = end;

    return 2;
}

// OFFSET: 0x00B035B0/0x1408C5870
int32_t Grunt::ClientLink::CmdAuthLogonChallenge(CDataStore& msg) {
    if (msg.m_read > msg.m_size || msg.m_size - msg.m_read < 2) {
        return 0;
    }

    uint8_t protocol;
    msg.Get(protocol);

    if (protocol) {
        return 1;
    }

    uint8_t result;
    msg.Get(result);

    if (result == Grunt::GRUNT_RESULT_SUCCESS) {
        if (msg.m_read > msg.m_size || msg.m_size - msg.m_read < 33) {
            return 0;
        }

        void* serverPublicKey;
        msg.GetDataInSitu(serverPublicKey, 32);

        uint8_t generatorLength;
        msg.Get(generatorLength);

        if (!CanReadNumBytes(msg, generatorLength + 1)) {
            return 0;
        }

        void* generator;
        msg.GetDataInSitu(generator, generatorLength);

        uint8_t largeSafePrimeLength;
        msg.Get(largeSafePrimeLength);

        if (!CanReadNumBytes(msg, largeSafePrimeLength + 48)) {
            return 0;
        }

        void* largeSafePrime;
        msg.GetDataInSitu(largeSafePrime, largeSafePrimeLength);

        void* salt;
        msg.GetDataInSitu(salt, 32);

        void* versionChallenge;
        msg.GetDataInSitu(versionChallenge, 16);

        if (!CanReadNumBytes(msg, 1)) {
            return 0;
        }

        uint8_t securityFlags;
        msg.Get(securityFlags);

        uint32_t pinSeed = 0;
        void* pinSalt = NULL;

        if (securityFlags & 0x1) {
            if (!CanReadNumBytes(msg, 20)) {
                return 0;
            }

            msg.Get(pinSeed);
            msg.GetDataInSitu(pinSalt, 16);
        }

        uint8_t matrixWidth = 0;
        uint8_t matrixHeight = 0;
        uint8_t matrixDigitCount = 0;
        uint8_t matrixChallengeCount = 0;
        uint64_t matrixSeed = 0;

        if (securityFlags & 0x2) {
            if (!CanReadNumBytes(msg, 12)) {
                return 0;
            }

            msg.Get(matrixWidth);
            msg.Get(matrixHeight);
            msg.Get(matrixDigitCount);
            msg.Get(matrixChallengeCount);
            msg.Get(matrixSeed);

            if ((securityFlags & 0x2) && !matrixChallengeCount) {
                return 1;
            }
        }

        uint8_t tokenRequired = 0;

        if (securityFlags & 0x4) {
            if (!CanReadNumBytes(msg, 1)) {
                return 0;
            }

            msg.Get(tokenRequired);
        }

        if (msg.m_read > msg.m_size) {
            return 1;
        }

        memcpy(this->m_serverChallenge, serverPublicKey, sizeof(this->m_serverChallenge));

        uint8_t seed[16];
        OsSecureRandom(seed, sizeof(seed));
        SRP6_Random random(seed, sizeof(seed));

        if (this->m_srpClient.CalculateProof(static_cast<uint8_t*>(largeSafePrime), largeSafePrimeLength, static_cast<uint8_t*>(generator), generatorLength, static_cast<uint8_t*>(salt), 32, static_cast<uint8_t*>(serverPublicKey), 32, random)) {
            this->SetState(2);
            this->m_clientResponse->LogonResult(Grunt::GRUNT_RESULT_INCORRECT_PASSWORD, NULL, 0, 0);
        } else {
            this->SetState(4);
            this->m_clientResponse->SetPinInfo((securityFlags & 0x1) == 0x1, pinSeed, static_cast<uint8_t*>(pinSalt));
            this->m_clientResponse->SetMatrixInfo((securityFlags & 0x2) == 0x2, matrixWidth, matrixHeight, matrixDigitCount, matrixDigitCount, false, matrixChallengeCount, matrixSeed, this->m_srpClient.m_sessionKey, sizeof(this->m_srpClient.m_sessionKey));
            this->m_clientResponse->SetTokenInfo((securityFlags & 0x4) == 0x4, tokenRequired);
            this->m_clientResponse->GetVersionProof(static_cast<uint8_t*>(versionChallenge));
        }

        return 2;
    }

    if (msg.m_read > msg.m_size) {
        return 1;
    }

    this->SetState(2);

    if (result >= 33) {
        DebugLog(1, 10, "Unknown grunt result %d @ %d", result, 1934);
        result = Grunt::GRUNT_RESULT_DISCONNECTED;
    }

    this->m_clientResponse->LogonResult(static_cast<Grunt::Result>(result), NULL, 0, 0);

    return 2;
}

// OFFSET: 0x00B03970/0x1408C5DD0
int32_t Grunt::ClientLink::CmdAuthLogonProof(CDataStore& msg) {
    if (msg.m_read >= msg.m_size) {
        return 0;
    }

    uint8_t result;
    msg.Get(result);

    if (result == Grunt::GRUNT_RESULT_SUCCESS) {
        if (msg.m_read > msg.m_size || msg.m_size - msg.m_read < 24) {
            return 0;
        }

        void* serverProof;
        msg.GetDataInSitu(serverProof, 20);

        uint32_t accountFlags = 0;
        msg.Get(accountFlags);

        uint32_t surveyId;
        msg.Get(surveyId);

        if (msg.m_read > msg.m_size || msg.m_size - msg.m_read < 2) {
            return 0;
        }

        uint16_t loginFlags = 0;
        msg.Get(loginFlags);

        if (msg.m_read > msg.m_size) {
            return 1;
        }

        if (this->m_srpClient.VerifyServerProof(static_cast<uint8_t*>(serverProof), 20)) {
            this->SetState(2);
            this->m_clientResponse->LogonResult(Grunt::GRUNT_RESULT_UNKNOWN_11, NULL, 0, 0);
        } else {
            this->m_accountFlags = accountFlags;
            this->m_serverId = 0;
            this->m_surveyId = surveyId;
            this->SetState(6);
            this->m_clientResponse->LogonResult(Grunt::GRUNT_RESULT_SUCCESS, this->m_srpClient.m_sessionKey, sizeof(this->m_srpClient.m_sessionKey), loginFlags);
        }

        return 2;
    }

    if (result == Grunt::GRUNT_RESULT_UNKNOWN_ACCOUNT) {
        if (msg.m_read > msg.m_size || msg.m_size - msg.m_read < 2) {
            return 0;
        }

        uint16_t unknown = 0;
        msg.Get(unknown);
    }

    if (msg.m_read > msg.m_size) {
        return 1;
    }

    if (result != Grunt::GRUNT_RESULT_DOWNLOAD_FILE) {
        this->SetState(2);

        if (result >= 33) {
            DebugLog(1, 10, "Unknown grunt result %d @ %d", result, 2014);
            result = Grunt::GRUNT_RESULT_DISCONNECTED;
        }

        this->m_clientResponse->LogonResult(static_cast<Grunt::Result>(result), NULL, 0, 0);
    }

    return 2;
}

// OFFSET: 0x00B04940/0x1408C7130
void Grunt::ClientLink::WCDataReady(WowConnection* connection, uint32_t timeStamp, uint8_t* data, int32_t length) {
    this->m_datastore.PutArray(data, length);
    this->m_datastore.Finalize();

    uint32_t pos = 0;

    if (Grunt::Command<Grunt::ClientLink>::Process(this->m_datastore, s_authCmds, sizeof(s_authCmds) / sizeof(s_authCmds[0]), *this, pos)) {
        uint32_t remaining = this->m_datastore.m_size - pos;
        this->m_datastore.m_read = pos;

        void* rest;
        this->m_datastore.GetDataInSitu(rest, remaining);
        this->m_datastore.m_read = -1;
        this->m_datastore.Reset();
        this->m_datastore.PutData(rest, remaining);
    } else {
        this->m_datastore.m_read = -1;
        this->m_datastore.Reset();
        this->Disconnect();
    }
}

// OFFSET: 0x00B05370/0x1408C7CF0
void Grunt::ClientLink::GetRealmList() {
    if (this->m_state != 6) {
        return;
    }

    CDataStoreCache<1024> msg;
    msg.Put(static_cast<uint8_t>(0x10));
    msg.Put(static_cast<uint32_t>(0));
    msg.Finalize();
    this->Send(msg);
}

// OFFSET: 0x00B057C0/0x1408C8240
void Grunt::ClientLink::ProveVersion(const uint8_t* versionChecksum) {
    CDataStoreCache<1024> msg;
    Grunt::CdKey cdKeys;
    SHA1_CONTEXT context;
    uint8_t hash[20];

    if (!this->m_clientResponse->GetCdKeys(cdKeys)) {
        cdKeys.m_count = 0;
    }

    if (this->m_state == 4) {
        msg.Put(static_cast<uint8_t>(1));
        msg.PutData(this->m_srpClient.m_publicKey, sizeof(this->m_srpClient.m_publicKey));
        msg.PutData(this->m_srpClient.m_clientProof, sizeof(this->m_srpClient.m_clientProof));

        SHA1_Init(&context);
        SHA1_Update(&context, this->m_srpClient.m_publicKey, sizeof(this->m_srpClient.m_publicKey));
        SHA1_Update(&context, versionChecksum, 20);
        SHA1_Final(hash, &context);
        msg.PutData(hash, sizeof(hash));

        msg.Put(static_cast<uint8_t>(cdKeys.m_count));

        for (uint32_t i = 0; i < static_cast<uint8_t>(cdKeys.m_count); i++) {
            Grunt::CdKey::Info& key = cdKeys.m_keys.m_data[i];
            msg.Put(key.m_keyLength);
            msg.Put(key.m_productId);
            msg.PutData(key.m_publicValue, sizeof(key.m_publicValue));

            SHA1_Init(&context);
            SHA1_Update(&context, this->m_srpClient.m_publicKey, sizeof(this->m_srpClient.m_publicKey));
            SHA1_Update(&context, this->m_serverChallenge, sizeof(this->m_serverChallenge));
            SHA1_Update(&context, key.m_hash, sizeof(key.m_hash));
            SHA1_Final(hash, &context);
            msg.PutData(hash, sizeof(hash));
        }

        uint8_t securityFlags = 0;

        bool pinEnabled;
        uint8_t* pinSalt;
        uint8_t* pinHash;
        this->m_clientResponse->GetPinInfo(&pinEnabled, &pinSalt, &pinHash);

        if (pinEnabled) {
            securityFlags = 0x1;
        }

        bool matrixEnabled;
        const uint8_t* matrixProof;
        this->m_clientResponse->GetMatrixProof(&matrixEnabled, &matrixProof);

        if (matrixEnabled) {
            securityFlags |= 0x2;
        }

        char token[17] = {};
        bool tokenEnabled;
        this->m_clientResponse->GetTokenInfo(&tokenEnabled, token);
        uint8_t tokenLength = SStrLen(token);

        if (tokenEnabled) {
            securityFlags |= 0x4;
        }

        msg.Put(securityFlags);

        if (pinEnabled) {
            msg.PutData(pinSalt, 16);
            msg.PutData(pinHash, 20);
        }

        if (matrixEnabled) {
            msg.PutData(matrixProof, 20);
        }

        if (tokenEnabled) {
            msg.Put(tokenLength);
            msg.PutArray(reinterpret_cast<uint8_t*>(token), tokenLength);
        }
    } else {
        uint32_t length = SStrLen(this->m_accountName);

        uint8_t clientSalt[16];
        MD5_CTX md5;
        MD5Init(&md5);
        MD5Update(&md5, reinterpret_cast<uint8_t*>(this->m_accountName), length);
        OsSecureRandom(clientSalt, sizeof(clientSalt));
        MD5Update(&md5, clientSalt, sizeof(clientSalt));
        MD5Final(clientSalt, &md5);

        msg.Put(static_cast<uint8_t>(3));
        msg.PutData(clientSalt, sizeof(clientSalt));

        SHA1_Init(&context);
        SHA1_Update(&context, reinterpret_cast<uint8_t*>(this->m_accountName), length);
        SHA1_Update(&context, clientSalt, sizeof(clientSalt));
        SHA1_Update(&context, this->m_serverChallenge, 16);
        SHA1_Update(&context, this->m_reconnectKey, sizeof(this->m_reconnectKey));
        SHA1_Final(hash, &context);
        msg.PutData(hash, sizeof(hash));

        SHA1_Init(&context);
        SHA1_Update(&context, clientSalt, sizeof(clientSalt));
        SHA1_Update(&context, versionChecksum, 20);
        SHA1_Final(hash, &context);
        msg.PutData(hash, sizeof(hash));

        msg.Put(static_cast<uint8_t>(cdKeys.m_count));

        for (uint32_t i = 0; i < static_cast<uint8_t>(cdKeys.m_count); i++) {
            Grunt::CdKey::Info& key = cdKeys.m_keys.m_data[i];
            msg.Put(key.m_keyLength);
            msg.Put(key.m_productId);
            msg.PutData(key.m_publicValue, sizeof(key.m_publicValue));

            SHA1_Init(&context);
            SHA1_Update(&context, clientSalt, sizeof(clientSalt));
            SHA1_Update(&context, this->m_serverChallenge, 16);
            SHA1_Update(&context, key.m_hash, sizeof(key.m_hash));
            SHA1_Final(hash, &context);
            msg.PutData(hash, sizeof(hash));
        }
    }

    msg.Finalize();
    this->Send(msg);
}

// OFFSET: 0x00B03B50/0x1408C6020
int32_t Grunt::ClientLink::CmdAuthReconnectChallenge(CDataStore& msg) {
    if (msg.m_read > msg.m_size || msg.m_size - msg.m_read < 17) {
        return 0;
    }

    uint8_t result;
    msg.Get(result);

    if (result) {
        return 1;
    }

    void* challenge;
    msg.GetDataInSitu(challenge, 16);

    void* versionChallenge;
    msg.GetDataInSitu(versionChallenge, 16);

    if (msg.m_read > msg.m_size) {
        return 1;
    }

    memcpy(this->m_serverChallenge, challenge, 16);
    this->SetState(5);
    this->m_clientResponse->GetVersionProof(static_cast<const uint8_t*>(versionChallenge));
    return 2;
}

// OFFSET: 0x00B03C20/0x1408C6110
int32_t Grunt::ClientLink::CmdAuthReconnectProof(CDataStore& msg) {
    if (msg.m_read >= msg.m_size) {
        return 0;
    }

    uint8_t result;
    msg.Get(result);

    if (msg.m_read > msg.m_size) {
        return 1;
    }

    if (result) {
        this->SetState(2);

        if (result >= 33) {
            result = Grunt::GRUNT_RESULT_DISCONNECTED;
        }

        this->m_clientResponse->ReconnectResult(static_cast<Grunt::Result>(result), NULL, 0, 0);
        return 2;
    }

    if (msg.m_size - msg.m_read < 2) {
        return 0;
    }

    uint16_t flags = 0;
    msg.Get(flags);
    this->SetState(6);
    this->m_clientResponse->ReconnectResult(Grunt::GRUNT_RESULT_SUCCESS, this->m_reconnectKey, sizeof(this->m_reconnectKey), flags);
    return 2;
}

// OFFSET: 0x00B03D10/0x1408C6240
int32_t Grunt::ClientLink::CmdXferInitiate(CDataStore& msg) {
    if (msg.m_read >= msg.m_size) {
        return 0;
    }

    uint8_t length;
    msg.Get(length);

    if (msg.m_read > msg.m_size || msg.m_size - msg.m_read < static_cast<uint32_t>(length + 24)) {
        return 0;
    }

    if (length >= 0xFE) {
        length = 0xFE;
    }

    char fileName[256];
    msg.GetArray(reinterpret_cast<uint8_t*>(fileName), length);
    fileName[length] = '\0';

    uint64_t size;
    msg.Get(size);

    void* md5;
    msg.GetDataInSitu(md5, 16);

    if (msg.m_read > msg.m_size) {
        return 1;
    }

    this->m_clientResponse->GetFileStatus(fileName, size, static_cast<const uint8_t*>(md5));
    return 2;
}

// OFFSET: 0x00B03DF0/0x1408C6370
int32_t Grunt::ClientLink::CmdXferData(CDataStore& msg) {
    if (msg.m_read > msg.m_size || msg.m_size - msg.m_read < 2) {
        return 0;
    }

    uint16_t size;
    msg.Get(size);

    if (msg.m_read > msg.m_size || msg.m_size - msg.m_read < size) {
        return 0;
    }

    void* data;
    msg.GetDataInSitu(data, size);

    if (msg.m_read > msg.m_size) {
        return 1;
    }

    this->m_clientResponse->FileData(static_cast<const uint8_t*>(data), size);
    return 2;
}

// OFFSET: 0x00B050E0/0x1408C7A60
void Grunt::ClientLink::FileStatus(bool accept, uint64_t offset) {
    CDataStoreCache<1024> msg;

    if (accept) {
        if (offset) {
            msg.Put(static_cast<uint8_t>(0x33));
            msg.Put(offset);
        } else {
            msg.Put(static_cast<uint8_t>(0x32));
        }
    } else {
        msg.Put(static_cast<uint8_t>(0x34));
    }

    msg.Finalize();
    this->Send(msg);
}

// OFFSET: 0x00B05240/0x1408C7BC0
void Grunt::ClientLink::FileComplete() {
    CDataStoreCache<1024> msg;
    msg.Put(static_cast<uint8_t>(0x34));
    msg.Finalize();
    this->Send(msg);
}

// OFFSET: 0x00B02CB0
const char* Grunt::ServerLink::GetLinkTypeName() const {
    return this->m_linkType <= 1 ? s_linkTypeNames[this->m_linkType] : "None";
}

// OFFSET: 0x00B02E10/0x1408C4E40
int32_t Grunt::ServerLink::CmdGruntKick(CDataStore& msg) {
    if (!CanReadNumBytes(msg, 13)) {
        return 0;
    }

    uint32_t accountId;
    msg.Get(accountId);
    uint64_t guid;
    msg.Get(guid);
    uint8_t reason;
    msg.Get(reason);

    if (msg.m_read > msg.m_size) {
        return 1;
    }

    DebugLog(33, 10, "received kick message for account id [%d:%lld]", accountId, guid);
    this->m_response->Kick(accountId, guid, reason);

    return 2;
}

// OFFSET: 0x00B02F20/0x1408C4F70
int32_t Grunt::ServerLink::CmdGruntCaisTimeUpdate(CDataStore& msg) {
    if (!CanReadNumBytes(msg, 6)) {
        return 0;
    }

    uint32_t accountId;
    msg.Get(accountId);
    uint16_t minutes;
    msg.Get(minutes);

    if (msg.m_read > msg.m_size) {
        return 1;
    }

    this->m_response->CaisTimeUpdate(accountId, minutes);

    return 2;
}

// OFFSET: 0x00B02EC0/0x1408C4F10
int32_t Grunt::ServerLink::CmdGruntPcWarning(CDataStore& msg) {
    if (!CanReadNumBytes(msg, 16)) {
        return 0;
    }

    uint32_t accountId;
    msg.Get(accountId);
    uint64_t guid;
    msg.Get(guid);
    uint32_t warning;
    msg.Get(warning);

    return msg.m_read > msg.m_size ? 1 : 2;
}

// OFFSET: 0x00B02FA0/0x1408C5000
int32_t Grunt::ServerLink::CmdGruntAuthVerify(CDataStore& msg) {
    if (!CanReadNumBytes(msg, 1)) {
        return 0;
    }

    uint8_t nameLength;
    msg.Get(nameLength);

    if (nameLength >= 32) {
        return 1;
    }

    if (!CanReadNumBytes(msg, nameLength + 36)) {
        return 0;
    }

    DebugLog(1, 10, "%s Auth Verify. Server name=%s", this->GetLinkTypeName(), this->m_serverName);

    uint8_t name[32];
    msg.GetArray(name, nameLength);
    name[nameLength] = 0;

    uint8_t salt[16];
    msg.GetArray(salt, sizeof(salt));

    uint8_t proof[20];
    msg.GetArray(proof, sizeof(proof));

    return msg.m_read > msg.m_size ? 1 : 2;
}

// OFFSET: 0x00B03080/0x1408C5140
int32_t Grunt::ServerLink::CmdGruntConnPong(CDataStore& msg) {
    if (!CanReadNumBytes(msg, 4)) {
        return 0;
    }

    uint32_t sequence;
    msg.Get(sequence);

    if (msg.m_read > msg.m_size) {
        return 1;
    }

    this->m_lastPongTime = OsGetAsyncTimeMs();

    return 2;
}

// OFFSET: 0x00B029E0
void Grunt::ServerLink::Send(CDataStore& msg) {
    this->m_lock->Enter();

    if (this->m_connection) {
        uint32_t size = msg.m_size;
        void* data;
        msg.GetDataInSitu(data, size);
        this->m_connection->SendRaw(static_cast<uint8_t*>(data), size);
    }

    this->m_lock->Leave();
}

// OFFSET: 0x00B02A30
void Grunt::ServerLink::SendConnected(CDataStore& msg) {
    this->m_lock->Enter();

    if (this->m_connection && this->m_state == 3) {
        uint32_t size = msg.m_size;
        void* data;
        msg.GetDataInSitu(data, size);
        this->m_connection->SendRaw(static_cast<uint8_t*>(data), size);
    }

    this->m_lock->Leave();
}

// OFFSET: 0x00B030E0/0x1408C51B0
int32_t Grunt::ServerLink::CmdGruntSunkenUpdate(CDataStore& msg) {
    if (!CanReadNumBytes(msg, 9)) {
        return 0;
    }

    uint32_t accountId;
    msg.Get(accountId);
    uint32_t sunkenId;
    msg.Get(sunkenId);
    uint8_t online;
    msg.Get(online);

    this->m_response->SunkenUpdate(accountId, sunkenId, online != 0);
    DebugLog(1, 10, ">>>>BNet: Received SunkenUpdate for WoW Account ID: %d, Sunken ID: %d,  online: %s", accountId, sunkenId, online ? "true" : "false");

    return 2;
}

// OFFSET: 0x00B03180/0x1408C5280
int32_t Grunt::ServerLink::CmdGruntSunkenOnline(CDataStore& msg) {
    if (!CanReadNumBytes(msg, 5)) {
        return 0;
    }

    uint32_t sunkenId;
    msg.Get(sunkenId);
    uint8_t online;
    msg.Get(online);

    this->m_response->SunkenOnline(sunkenId, online != 0);
    DebugLog(1, 10, ">>>>BNet: received SunkenOnline: Sunken ID : %08x, online: %s", sunkenId, online ? "true" : "false");

    return 2;
}

// OFFSET: 0x00B03210/0x1408C5330
void Grunt::ServerLink::SendStrings(uint32_t id, uint32_t locale) {
    CDataStore strings;
    this->m_response->GetStrings(id, locale, &strings);
    strings.m_read = 0;

    for (int32_t offset = 0; offset < static_cast<int32_t>(strings.m_size); ) {
        uint32_t chunk = strings.m_size - offset;

        if (chunk > 8000) {
            chunk = 8000;
        }

        CDataStore msg;
        msg.Put(static_cast<uint8_t>(0x41));
        msg.Put(id);
        msg.Put(locale);
        msg.Put(static_cast<uint32_t>(offset));
        msg.Put(strings.m_size);
        msg.Put(chunk);

        void* data;
        strings.GetDataInSitu(data, chunk);
        msg.PutData(data, chunk);
        msg.m_read = 0;
        this->SendConnected(msg);

        offset += chunk;
    }

    CDataStore msg;
    msg.Put(static_cast<uint8_t>(0x41));
    msg.Put(id);
    msg.Put(locale);
    msg.Put(static_cast<uint32_t>(-1));
    msg.Put(strings.m_size);
    msg.Put(static_cast<uint32_t>(0));
    msg.m_read = 0;
    this->SendConnected(msg);
}

// OFFSET: 0x00B04500/0x1408C6A30
int32_t Grunt::ServerLink::CmdGruntAuthChallenge(CDataStore& msg) {
    if (!CanReadNumBytes(msg, 16)) {
        return 0;
    }

    uint8_t challenge[16];
    msg.GetArray(challenge, sizeof(challenge));

    if (msg.m_read > msg.m_size) {
        return 1;
    }

    uint32_t nameLength = SStrLen(this->m_serverName);
    DebugLog(1, 10, "%s Auth Challenge. Server name=%s", this->GetLinkTypeName(), this->m_serverName);

    uint8_t salt[16];
    OsSecureRandom(salt, sizeof(salt));

    SHA1_CONTEXT context;
    SHA1_Init(&context);
    SHA1_Update(&context, salt, sizeof(salt));
    SHA1_Update(&context, challenge, sizeof(challenge));
    SHA1_Update(&context, reinterpret_cast<const uint8_t*>(this->m_serverName), nameLength);
    SHA1_Update(&context, s_serverAuthKey, sizeof(s_serverAuthKey));

    uint8_t digest[20];
    SHA1_Final(digest, &context);

    CDataStoreCache<1024> reply;
    reply.Put(static_cast<uint8_t>(1));
    reply.Put(static_cast<uint8_t>(nameLength));
    reply.PutData(this->m_serverName, nameLength);
    reply.Put(static_cast<uint8_t>(0));
    reply.PutData(salt, sizeof(salt));
    reply.PutData(digest, sizeof(digest));
    reply.m_read = 0;
    this->Send(reply);

    return 2;
}

// OFFSET: 0x00B046A0/0x1408C6D50
int32_t Grunt::ServerLink::CmdGruntConnPing(CDataStore& msg) {
    if (!CanReadNumBytes(msg, 4)) {
        return 0;
    }

    uint32_t sequence;
    msg.Get(sequence);

    if (msg.m_read > msg.m_size) {
        return 1;
    }

    CDataStoreCache<1024> reply;
    reply.Put(static_cast<uint8_t>(0x11));
    reply.Put(sequence);
    reply.m_read = 0;
    this->SendConnected(reply);

    return 2;
}

// OFFSET: 0x00B04750/0x1408C6F00
int32_t Grunt::ServerLink::CmdGruntStrings(CDataStore& msg) {
    if (!CanReadNumBytes(msg, 8)) {
        return 0;
    }

    uint32_t id;
    msg.Get(id);
    uint32_t locale;
    msg.Get(locale);

    if (msg.m_read > msg.m_size) {
        return 1;
    }

    if (locale) {
        this->SendStrings(id, locale);
        return 2;
    }

    for (uint32_t i = 0; i < sizeof(s_localeIds) / sizeof(s_localeIds[0]); i++) {
        this->SendStrings(id, s_localeIds[i]);
    }

    return 2;
}

// OFFSET: 0x00B02980
static void HexString(const uint8_t* data, char* text, uint32_t length, bool spaces) {
    for (const uint8_t* end = data + length; data < end; data++) {
        uint8_t value = *data;
        *text++ = (value & 0xF0) >= 0xA0 ? (value >> 4) + 55 : (value >> 4) + 48;
        *text++ = (value & 0xF) >= 0xA ? (value & 0xF) + 55 : (value & 0xF) + 48;

        if (spaces) {
            *text++ = ' ';
        }
    }

    *text = '\0';
}

// OFFSET: 0x00B02A90/0x1408C49B0
static void LogServerPacket(CDataStore& msg, const char* name) {
    uint32_t read = msg.m_read;
    uint32_t length = msg.m_size - read;

    if (length >= 1364) {
        length = 1364;
    }

    void* data;
    msg.GetDataInSitu(data, length);

    char text[4096];
    HexString(static_cast<const uint8_t*>(data), text, length, true);
    DebugLog(1, 10, "ServerLink: Sending %s ! [%s]", name, text);
    msg.m_read = read;
}

// OFFSET: 0x00B02CF0/0x1408C4CB0
static void PutServerAddresses(CDataStore& msg, const TSFixedArray<SockAddr>& addresses, const TSFixedArray<uint16_t>& ports) {
    uint32_t count = addresses.m_count;
    uint32_t ipv4Count = 0;
    uint32_t ipv6Count = 0;

    if (count) {
        for (uint32_t i = 0; i < count; i++) {
            if (addresses.m_data[i].m_type == SOCKADDR_IPV4) {
                ipv4Count++;
            } else if (addresses.m_data[i].m_type == SOCKADDR_IPV6) {
                ipv6Count++;
            }
        }

        if (ipv4Count >= 16) {
            ipv4Count = 16;
        }

        if (ipv6Count >= 16) {
            ipv6Count = 16;
        }
    }

    msg.Put(static_cast<uint8_t>(ipv4Count));

    uint32_t remaining = ipv4Count;

    for (uint32_t i = 0; i < count; i++) {
        if (addresses.m_data[i].m_type == SOCKADDR_IPV4) {
            if (!remaining--) {
                break;
            }

            msg.PutArray(reinterpret_cast<const uint8_t*>(&addresses.m_data[i]), 4);
            msg.Put(ports.m_data[i]);
        }
    }

    msg.Put(static_cast<uint8_t>(ipv6Count));

    remaining = ipv6Count;

    for (uint32_t i = 0; i < count; i++) {
        if (addresses.m_data[i].m_type == SOCKADDR_IPV6) {
            if (!remaining--) {
                return;
            }

            msg.PutArray(reinterpret_cast<const uint8_t*>(&addresses.m_data[i]), 16);
            msg.Put(ports.m_data[i]);
        }
    }
}

// OFFSET: 0x00B047F0/0x1408C6FE0
void Grunt::ServerLink::SendFeatures() {
    CDataStoreCache<8192> msg;
    msg.Put(static_cast<uint8_t>(0x45));
    Grunt::ServerLink::PutFeatures(msg);
    msg.m_read = 0;
    LogServerPacket(msg, "CMD_GRUNT_PRESENCE_ANNOUNCE");
    this->Send(msg);
}

// OFFSET: 0x00B060B0/0x1408C8DE0
int32_t Grunt::ServerLink::CmdGruntHello(CDataStore& msg) {
    if (!CanReadNumBytes(msg, 4)) {
        return 0;
    }

    uint32_t value;
    msg.Get(value);

    if (msg.m_read > msg.m_size) {
        return 1;
    }

    this->unk_0094 = value;

    CDataStoreCache<1024> reply;
    reply.Put(static_cast<uint8_t>(0x20));

    if (this->m_linkType >= 2) {
        return 1;
    }

    const char* typeName;

    if (this->m_linkType) {
        typeName = "Battle.net";
        this->m_revision = 0x23;
    } else {
        typeName = "Grunt";
        this->m_revision = 0x20;
    }

    reply.Put(this->m_revision);
    uint32_t sizePos = reply.m_size;
    reply.Put(static_cast<uint16_t>(0));

    Grunt::ServerHelloInfo info;
    info.m_linkType = this->m_linkType;

    if (!this->m_response->GetHelloInfo(&info)) {
        DebugLog(1, 10, "Unable to get the information necessary to identify self to %s", typeName);
        return 1;
    }

    uint8_t nameLength = static_cast<uint8_t>(strlen(info.m_realmName));
    reply.Put(nameLength);
    reply.PutData(info.m_realmName, nameLength);

    if (this->m_revision < 0x23) {
        reply.Put(info.m_address);
        reply.Put(info.m_port);
    } else {
        PutServerAddresses(reply, info.m_addresses, info.m_ports);
    }

    reply.Put(info.m_realmType);
    reply.Put(info.unk_0004);
    reply.Put(info.m_versionMajor);
    reply.Put(info.m_versionMinor);
    reply.Put(info.m_versionPatch);
    reply.Put(info.m_build);
    reply.Put(info.unk_0018);
    reply.Put(info.unk_001C);
    reply.Put(info.unk_0020);
    reply.Put(info.m_realmId);
    reply.Put(info.unk_0028);
    reply.Put(info.unk_004C);
    reply.Put(this->unk_0098);
    reply.Set(sizePos, static_cast<uint16_t>(reply.m_size - sizePos - 2));

    NETADDR addr;
    char addrText[64];
    OsNetAddrMake(this->m_serverAddress, 0, &addr);
    DebugLog(1, 10, "Sending HELLO to %s login server at %s", typeName, OsNetAddrToStr(&addr, addrText, sizeof(addrText)));
    DebugLog(1, 10, "Reported IP addresses list");

    for (uint32_t i = 0; i < info.m_addresses.m_count; i++) {
        SockAddrString address(info.m_addresses.m_data[i], info.m_ports.m_data[i]);
        DebugLog(1, 10, "%u: %s", i, address.m_str);
    }

    DebugLog(1, 10, "My realm name is %s", info.m_realmName);
    DebugLog(1, 10, "My realm ID is %d", info.m_realmId);
    DebugLog(1, 10, "My version is %d.%d.%d.%d", info.m_versionMajor, info.m_versionMinor, info.m_versionPatch, info.m_build);
    DebugLog(1, 10, "My realm type is %d", info.m_realmType);
    DebugLog(1, 10, "My server revision is %d", this->m_revision);

    reply.m_read = 0;
    LogServerPacket(reply, "CMD_GRUNT_HELLO");
    this->Send(reply);

    if (this->m_linkType == 1) {
        this->SendFeatures();
    }

    DebugLog(1, 10, "ServerLink: Marking B.Net / grunt connection connected !");
    this->m_state = 3;

    if (this->m_linkType == 1) {
        this->m_response->HelloComplete();
    }

    return 2;
}

// OFFSET: 0x00B06550/0x1408C9580
int32_t Grunt::ServerLink::CmdGruntProveSession(CDataStore& msg) {
    if (!CanReadNumBytes(msg, 5)) {
        return 0;
    }

    uint32_t requestId;
    msg.Get(requestId);
    uint8_t result;
    msg.Get(result);

    Grunt::ServerSessionInfo info;
    memset(&info, 0, sizeof(info));

    if (!result) {
        msg.Get(info.unk_0000);
        msg.Get(info.m_accountId);
        msg.Get(info.unk_0008);
        msg.Get(info.unk_0010);
        msg.Get(info.unk_0018);
        msg.Get(info.unk_0078);
        msg.Get(info.unk_001C);
        msg.Get(info.m_timeOptions);

        uint64_t accountFlags;
        msg.Get(accountFlags);
        info.m_accountFlags = static_cast<uint32_t>(accountFlags);

        msg.GetArray(info.unk_0028, sizeof(info.unk_0028));
        msg.Get(info.unk_0050);
        msg.Get(info.unk_0054);
        msg.Get(info.m_os);

        uint8_t versionLength = 0;
        msg.Get(versionLength);
        char* version = static_cast<char*>(alloca(versionLength + 1));
        memset(version, 0, versionLength + 1);
        msg.GetArray(reinterpret_cast<uint8_t*>(version), versionLength);
        sscanf(version, "%d.%d.%d.%d", reinterpret_cast<int32_t*>(&info.m_versionMajor), reinterpret_cast<int32_t*>(&info.m_versionMinor), reinterpret_cast<int32_t*>(&info.m_versionPatch), reinterpret_cast<int32_t*>(&info.m_build));

        msg.Get(info.m_timePlayedMinutes);
        msg.Get(info.unk_0064);
        msg.Get(info.m_expansionLevel);
        msg.Get(reinterpret_cast<uint32_t&>(info.unk_0068));

        info.m_accountFlags &= 0x6A8E040F;

        if (info.unk_0068 > 0) {
            info.m_accountFlags |= 0x10;
        }

        msg.Get(info.unk_006C);
        msg.Get(info.m_referrerAccountId);
        msg.Get(reinterpret_cast<uint32_t&>(info.m_referralTime));

        if (this->m_linkType == 1) {
            msg.Get(info.unk_007C);
        }

        msg.Get(info.m_bnetLicenseCount);

        if (msg.m_read > msg.m_size) {
            return 0;
        }

        if (info.m_bnetLicenseCount >= 256) {
            return 1;
        }

        for (uint32_t i = 0; i < info.m_bnetLicenseCount; i++) {
            Grunt::ServerLicense& license = info.m_bnetLicenses[i];
            msg.Get(license.m_id);
            msg.Get(license.m_hasExpiration);

            if (license.m_hasExpiration) {
                msg.Get(license.m_expiration);
            } else {
                license.m_expiration = 0;
            }
        }

        msg.Get(info.m_wowLicenseCount);

        if (msg.m_read > msg.m_size) {
            return 0;
        }

        if (info.m_wowLicenseCount >= 256) {
            DebugLog(0x10021, 10, "ERROR: Too many game licenses (%d) for account (%d)", info.m_wowLicenseCount, info.m_accountId);
            return 1;
        }

        for (uint32_t i = 0; i < info.m_wowLicenseCount; i++) {
            Grunt::ServerLicense& license = info.m_wowLicenses[i];
            msg.Get(license.m_id);
            msg.Get(license.m_hasExpiration);

            if (license.m_hasExpiration) {
                msg.Get(license.m_expiration);
            } else {
                license.m_expiration = 0;
            }
        }
    }

    if (msg.m_read > msg.m_size) {
        return 0;
    }

    Grunt::ServerLink::Request* request = static_cast<Grunt::ServerLink::Request*>(this->Remove(requestId));

    if (!request) {
        return 2;
    }

    if (result) {
        if (result >= 0x21) {
            DebugLog(0x10001, 10, "Unknown result %d from %s", result, this->GetLinkTypeName());
            result = 0xFF;
        }

        this->m_response->ProveSessionResult(request->m_context, result, NULL, this->m_linkType, true);
    } else {
        if (info.m_referrerAccountId) {
            DebugLog(33, 10, "(Refer-A-Friend) Account [%d] was referred by account [%d]. Referral expires in %.02f minutes.", info.m_accountId, info.m_referrerAccountId, static_cast<double>(info.m_referralTime) * 0.016666668f);
        }

        DebugLog(33, 10, "account id [%d] has passed authentication from %s, accountflags=0x%lx, timeOptions=0x%lx, timePlayedMinutes=%d, expansionLevel=%d", info.m_accountId, this->GetLinkTypeName(), info.m_accountFlags, info.m_timeOptions, info.m_timePlayedMinutes, info.m_expansionLevel);

        if (info.m_bnetLicenseCount || info.m_wowLicenseCount) {
            DebugLog(33, 10, "Account [%d] has numBnetAccountLicenses=%d, numWowAccountLicenses=%d", info.m_accountId, info.m_bnetLicenseCount, info.m_wowLicenseCount);
        }

        this->m_response->ProveSessionResult(request->m_context, 0, &info, this->m_linkType, true);
    }

    delete request;

    return 2;
}

// OFFSET: 0x00B2F1E0/0x1408F7D30
void Grunt::ServerLink::PutFeatures(CDataStore& msg) {
    msg.Put(static_cast<uint16_t>(sizeof(s_serverFeatures) / sizeof(s_serverFeatures[0])));

    for (uint32_t i = 0; i < sizeof(s_serverFeatures) / sizeof(s_serverFeatures[0]); i++) {
        msg.Put(s_serverFeatures[i].m_id);
        msg.Put(static_cast<uint8_t>(s_serverFeatures[i].m_version));
        msg.Put(static_cast<uint8_t>(0));
        msg.Put(static_cast<uint8_t>(0));
    }

    uint32_t countPos = msg.m_size;
    msg.Put(static_cast<uint16_t>(0));
    msg.Put(static_cast<uint32_t>('Relm'));
    msg.Put(static_cast<uint32_t>('Race'));
    msg.Put(static_cast<uint32_t>('Clss'));
    msg.Put(static_cast<uint32_t>('Area'));
    msg.Set(countPos, static_cast<uint16_t>(4));
}

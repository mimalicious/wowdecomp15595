#include "WoW/Source/JamClientAutoCode/JamClientConnection.h"
#include <cstring>
#include "Engine/Source/Base/CDataStore.h"

// STATIC: 0x00E4D838
bool (*ClientConnectionAuthChallenge::s_handler)(ClientConnectionAuthChallenge* msg);

// STATIC: 0x00E4D83C
bool (*ClientConnectionSuspendComms::s_handler)(ClientConnectionSuspendComms* msg);

// STATIC: 0x00E4D840
bool (*ClientConnectionPong::s_handler)(ClientConnectionPong* msg);

// STATIC: 0x00E4D844
bool (*ClientConnectionResumeComms::s_handler)(ClientConnectionResumeComms* msg);

// STATIC: 0x00E4D848
bool (*ClientConnectionNoop::s_handler)(ClientConnectionNoop* msg);

// STATIC: 0x00E4D84C
bool (*ClientConnectionResetCompressionContext::s_handler)(ClientConnectionResetCompressionContext* msg);

// STATIC: 0x00E4D850
bool (*ClientConnectionDropNewConnection::s_handler)(ClientConnectionDropNewConnection* msg);

// STATIC: 0x00E4D854
bool (*ClientConnectionConnectTo::s_handler)(ClientConnectionConnectTo* msg);

// OFFSET: 0x006964B0
ClientConnectionAuthChallenge::ClientConnectionAuthChallenge(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// OFFSET: 0x00694900
ClientConnectionAuthChallenge::~ClientConnectionAuthChallenge() {
}

// OFFSET: 0x00694BC0
void ClientConnectionAuthChallenge::Unpack(CDataStore* msg, bool full) {
    for (uint32_t i = 0; i < 8; i++) {
        uint32_t value = 0;
        msg->Get(value);
        this->m_dosChallenge[i] = value;
    }

    uint32_t challenge = 0;
    msg->Get(challenge);
    this->m_challenge = challenge;

    uint8_t dosZeroBits = 0;
    msg->Get(dosZeroBits);
    this->m_dosZeroBits = dosZeroBits;
}

// OFFSET: 0x00694C80
bool ClientConnectionAuthChallenge::Get(CDataStore* msg, bool full) {
    for (uint32_t i = 0; i < 8; i++) {
        uint32_t value = 0;
        msg->Get(value);
        this->m_dosChallenge[i] = value;
    }

    uint32_t challenge = 0;
    msg->Get(challenge);
    this->m_challenge = challenge;

    uint8_t dosZeroBits = 0;
    msg->Get(dosZeroBits);
    this->m_dosZeroBits = dosZeroBits;

    return msg->IsRead() != 0;
}

// OFFSET: 0x00694D90
bool ClientConnectionAuthChallenge::Handle() {
    if (!ClientConnectionAuthChallenge::s_handler) {
        return false;
    }

    return ClientConnectionAuthChallenge::s_handler(this);
}

// OFFSET: 0x00694D00
ClientConnectionConnectTo::ClientConnectionConnectTo(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// OFFSET: 0x00694920
ClientConnectionConnectTo::~ClientConnectionConnectTo() {
}

// OFFSET: 0x00694AF0
void ClientConnectionConnectTo::Unpack(CDataStore* msg, bool full) {
    uint64_t key = 0;
    msg->Get(key);
    this->m_key = key;

    uint32_t serial = 0;
    msg->Get(serial);
    this->m_serial = serial;

    for (uint32_t i = 0; i < sizeof(this->m_payload); i++) {
        uint8_t value = 0;
        msg->Get(value);
        this->m_payload[i] = value;
    }

    uint8_t con = 0;
    msg->Get(con);
    this->m_con = con;
}

// OFFSET: 0x00694A50
bool ClientConnectionConnectTo::Get(CDataStore* msg, bool full) {
    uint64_t key = 0;
    msg->Get(key);
    this->m_key = key;

    uint32_t serial = 0;
    msg->Get(serial);
    this->m_serial = serial;

    for (uint32_t i = 0; i < sizeof(this->m_payload); i++) {
        uint8_t value = 0;
        msg->Get(value);
        this->m_payload[i] = value;
    }

    uint8_t con = 0;
    msg->Get(con);
    this->m_con = con;

    return msg->IsRead() != 0;
}

// OFFSET: 0x00694A00
bool ClientConnectionConnectTo::Handle() {
    if (!ClientConnectionConnectTo::s_handler) {
        return false;
    }

    return ClientConnectionConnectTo::s_handler(this);
}

// OFFSET: 0x00696520
ClientConnectionPong::ClientConnectionPong(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->m_present = 0;

    uint32_t serial = 0;
    msg->Get(serial);
    this->m_present |= 0x1;
    this->m_serial = serial;
}

// PROVEN: 4de97fa3 IDENTICAL/NONE
// OFFSET: 0x00694910
ClientConnectionPong::~ClientConnectionPong() {
}

// PROVEN: be4720ba IDENTICAL/NONE
// OFFSET: 0x00694B80
bool ClientConnectionPong::Get(CDataStore* msg, bool full) {
    uint32_t serial = 0;
    msg->Get(serial);
    this->m_present |= 0x1;
    this->m_serial = serial;

    return msg->IsRead() != 0;
}

// OFFSET: 0x00694C30
bool ClientConnectionPong::Handle() {
    if (!ClientConnectionPong::s_handler) {
        return false;
    }

    return ClientConnectionPong::s_handler(this);
}

// OFFSET: 0x00696520
ClientConnectionSuspendComms::ClientConnectionSuspendComms(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->m_present = 0;

    uint32_t serial = 0;
    msg->Get(serial);
    this->m_present |= 0x1;
    this->m_serial = serial;
}

// PROVEN: f23a8ac7 IDENTICAL/NONE
// OFFSET: 0x00694910
ClientConnectionSuspendComms::~ClientConnectionSuspendComms() {
}

// PROVEN: d0e626a0 IDENTICAL/NONE
// OFFSET: 0x00694B80
bool ClientConnectionSuspendComms::Get(CDataStore* msg, bool full) {
    uint32_t serial = 0;
    msg->Get(serial);
    this->m_present |= 0x1;
    this->m_serial = serial;

    return msg->IsRead() != 0;
}

// OFFSET: 0x00694930
bool ClientConnectionSuspendComms::Handle() {
    if (!ClientConnectionSuspendComms::s_handler) {
        return false;
    }

    return ClientConnectionSuspendComms::s_handler(this);
}

// PROVEN: 3b318ae9 IDENTICAL/NONE
// OFFSET: 0x006A2E40/0x14036ACB0
ClientConnectionResumeComms::ClientConnectionResumeComms(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
}

// PROVEN: bdba72f0 IDENTICAL/NONE
// OFFSET: 0x006A1120/0x14033F780
ClientConnectionResumeComms::~ClientConnectionResumeComms() {
}

// PROVEN: e8fe07cb IDENTICAL/NONE
// OFFSET: 0x006BAEE0
bool ClientConnectionResumeComms::Get(CDataStore* msg, bool full) {
    return msg->IsRead() != 0;
}

// OFFSET: 0x00694980
bool ClientConnectionResumeComms::Handle() {
    if (!ClientConnectionResumeComms::s_handler) {
        return false;
    }

    return ClientConnectionResumeComms::s_handler(this);
}

// PROVEN: f623d0ae IDENTICAL/NONE
// OFFSET: 0x006A2E40/0x14036ACB0
ClientConnectionResetCompressionContext::ClientConnectionResetCompressionContext(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
}

// PROVEN: 162fb9ed IDENTICAL/NONE
// OFFSET: 0x006A1120/0x14033F780
ClientConnectionResetCompressionContext::~ClientConnectionResetCompressionContext() {
}

// PROVEN: 71bff20b IDENTICAL/NONE
// OFFSET: 0x006BAEE0
bool ClientConnectionResetCompressionContext::Get(CDataStore* msg, bool full) {
    return msg->IsRead() != 0;
}

// OFFSET: 0x006949C0
bool ClientConnectionResetCompressionContext::Handle() {
    if (!ClientConnectionResetCompressionContext::s_handler) {
        return false;
    }

    return ClientConnectionResetCompressionContext::s_handler(this);
}

// PROVEN: 6f10a356 IDENTICAL/NONE
// OFFSET: 0x006A2E40/0x14036ACB0
ClientConnectionDropNewConnection::ClientConnectionDropNewConnection(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
}

// PROVEN: 265d5f07 IDENTICAL/NONE
// OFFSET: 0x006A1120/0x14033F780
ClientConnectionDropNewConnection::~ClientConnectionDropNewConnection() {
}

// PROVEN: 3d0ceb3e IDENTICAL/NONE
// OFFSET: 0x006BAEE0
bool ClientConnectionDropNewConnection::Get(CDataStore* msg, bool full) {
    return msg->IsRead() != 0;
}

// OFFSET: 0x00694DE0
bool ClientConnectionDropNewConnection::Handle() {
    if (!ClientConnectionDropNewConnection::s_handler) {
        return false;
    }

    return ClientConnectionDropNewConnection::s_handler(this);
}

// PROVEN: 7d8ac1df IDENTICAL/NONE
// OFFSET: 0x006A2E40/0x14036ACB0
ClientConnectionNoop::ClientConnectionNoop(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
}

// PROVEN: a5e67d61 IDENTICAL/NONE
// OFFSET: 0x006A1120/0x14033F780
ClientConnectionNoop::~ClientConnectionNoop() {
}

// PROVEN: 704133ec IDENTICAL/NONE
// OFFSET: 0x006BAEE0
bool ClientConnectionNoop::Get(CDataStore* msg, bool full) {
    return msg->IsRead() != 0;
}

// OFFSET: 0x00694D40
bool ClientConnectionNoop::Handle() {
    if (!ClientConnectionNoop::s_handler) {
        return false;
    }

    return ClientConnectionNoop::s_handler(this);
}

// PROVEN: 8516bb88 IDENTICAL/NONE
// OFFSET: 0x0066A970
UserRouterClientPing::UserRouterClientPing() {
    this->m_present = 0;
}

// PROVEN: 2b8c4c5a IDENTICAL/NONE
// OFFSET: 0x0066A3D0
UserRouterClientPing::~UserRouterClientPing() {
}

// PROVEN: 85a8ce9e IDENTICAL/NONE
// OFFSET: 0x0066A910
void UserRouterClientPing::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x444D));
    msg->Put(this->m_latency);
    msg->Put(this->m_serial);
}

// OFFSET: 0x0066A770
UserRouterClientLogDisconnect::UserRouterClientLogDisconnect(uint32_t reason) {
    this->m_present = 0;
    this->m_present |= 0x1;
    this->m_reason = reason;
}

// PROVEN: c87ae8cb IDENTICAL/NONE
// OFFSET: 0x0066A3E0
UserRouterClientLogDisconnect::~UserRouterClientLogDisconnect() {
}

// PROVEN: f5d00f5a IDENTICAL/NONE
// OFFSET: 0x0066A9E0
void UserRouterClientLogDisconnect::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x446D));
    msg->Put(this->m_reason);
}

// PROVEN: cf5255e8 IDENTICAL/NONE
// OFFSET: 0x0066AA10
UserRouterClientSuspendCommsAck::UserRouterClientSuspendCommsAck() {
    this->m_present = 0;
}

// PROVEN: 27b4d448 IDENTICAL/NONE
// OFFSET: 0x0066A3F0
UserRouterClientSuspendCommsAck::~UserRouterClientSuspendCommsAck() {
}

// PROVEN: 1408bd2d IDENTICAL/NONE
// OFFSET: 0x0066A990
void UserRouterClientSuspendCommsAck::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x4469));
    msg->Put(this->m_serial);
}

// PROVEN: c72426c1 IDENTICAL/NONE
// OFFSET: 0x0066A950
UserRouterClientSuspendTokenResponse::UserRouterClientSuspendTokenResponse() {
    this->m_present = 0;
}

// PROVEN: 43d88893 IDENTICAL/NONE
// OFFSET: 0x0066A400
UserRouterClientSuspendTokenResponse::~UserRouterClientSuspendTokenResponse() {
}

// PROVEN: a1d353e0 IDENTICAL/NONE
// OFFSET: 0x0066A7A0
void UserRouterClientSuspendTokenResponse::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x46D));
    msg->Put(this->m_token);
}

// PROVEN: 57cf04cd IDENTICAL/NONE
// OFFSET: 0x0066A760/0x140317590
UserRouterClientEnableNagle::UserRouterClientEnableNagle() {
}

// PROVEN: 74bf4a09 IDENTICAL/NONE
// OFFSET: 0x0066A410/0x140317570
UserRouterClientEnableNagle::~UserRouterClientEnableNagle() {
}

// PROVEN: 8c55ffb7 IDENTICAL/NONE
// OFFSET: 0x0066A9C0
void UserRouterClientEnableNagle::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x4449));
}

// PROVEN: eedd4fd9 IDENTICAL/NONE
// OFFSET: 0x0066A900
UserRouterClientAuthContinuedSession::UserRouterClientAuthContinuedSession() {
}

// PROVEN: b3c07a7c IDENTICAL/NONE
// OFFSET: 0x0066A420
UserRouterClientAuthContinuedSession::~UserRouterClientAuthContinuedSession() {
}

// PROVEN: f746efcf IDENTICAL/NONE
// OFFSET: 0x0066A7D0/0x140317620
void UserRouterClientAuthContinuedSession::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x44D));
    msg->Put(this->m_key);
    msg->Put(this->m_dosResponse);
    msg->Put(this->m_digest[5]);
    msg->Put(this->m_digest[2]);
    msg->Put(this->m_digest[6]);
    msg->Put(this->m_digest[10]);
    msg->Put(this->m_digest[8]);
    msg->Put(this->m_digest[17]);
    msg->Put(this->m_digest[11]);
    msg->Put(this->m_digest[15]);
    msg->Put(this->m_digest[7]);
    msg->Put(this->m_digest[1]);
    msg->Put(this->m_digest[4]);
    msg->Put(this->m_digest[16]);
    msg->Put(this->m_digest[0]);
    msg->Put(this->m_digest[12]);
    msg->Put(this->m_digest[14]);
    msg->Put(this->m_digest[13]);
    msg->Put(this->m_digest[18]);
    msg->Put(this->m_digest[9]);
    msg->Put(this->m_digest[19]);
    msg->Put(this->m_digest[3]);
}

// OFFSET: 0x0066AE70/0x140317910
UserRouterClientAuthSession::UserRouterClientAuthSession() {
}

// OFFSET: 0x0066AED0/0x140317970
UserRouterClientAuthSession::~UserRouterClientAuthSession() {
}

// OFFSET: 0x0066AF90/0x140317990
void UserRouterClientAuthSession::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x449));
    msg->Put(this->m_loginServerID);
    msg->Put(this->m_battlegroupID);
    msg->Put(this->m_loginServerType);
    msg->Put(this->m_digest[10]);
    msg->Put(this->m_digest[18]);
    msg->Put(this->m_digest[12]);
    msg->Put(this->m_digest[5]);
    msg->Put(this->m_dosResponse);
    msg->Put(this->m_digest[15]);
    msg->Put(this->m_digest[9]);
    msg->Put(this->m_digest[19]);
    msg->Put(this->m_digest[4]);
    msg->Put(this->m_digest[7]);
    msg->Put(this->m_digest[16]);
    msg->Put(this->m_digest[3]);
    msg->Put(this->m_build);
    msg->Put(this->m_digest[8]);
    msg->Put(this->m_realmID);
    msg->Put(this->m_unk0018);
    msg->Put(this->m_digest[17]);
    msg->Put(this->m_digest[6]);
    msg->Put(this->m_digest[0]);
    msg->Put(this->m_digest[1]);
    msg->Put(this->m_digest[11]);
    msg->Put(this->m_localChallenge);
    msg->Put(this->m_digest[2]);
    msg->Put(this->m_regionID);
    msg->Put(this->m_digest[14]);
    msg->Put(this->m_digest[13]);
    msg->Put(this->m_addonInfo.m_size);
    msg->PutData(this->m_addonInfo.m_data, this->m_addonInfo.m_size);

    JamBitWriter bits(msg);
    bits.WriteBit(this->m_useIPv6);
    bits.WriteBits<12>(strlen(this->m_account));
    bits.Flush();

    msg->PutData(this->m_account, strlen(this->m_account));
}

// PROVEN: 24b5eaf7 IDENTICAL/NONE
// OFFSET: 0x0066DE80
UserClientConnectToFailed::UserClientConnectToFailed() {
}

// PROVEN: f332452c IDENTICAL/NONE
// OFFSET: 0x0066BBA0
UserClientConnectToFailed::~UserClientConnectToFailed() {
}

// PROVEN: 65f06b2d IDENTICAL/NONE
// OFFSET: 0x0066E520
void UserClientConnectToFailed::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x2533));
    msg->Put(this->m_serial);
    msg->Put(this->m_con);
}

#include "WoW/Source/JamClientAutoCode/JamClientConnection.h"
#include <cstring>
#include "Engine/Source/Base/CDataStore.h"

// STATIC: 0x00E4D838/0x140D36A58
bool (*ClientConnectionAuthChallenge::s_handler)(ClientConnectionAuthChallenge* msg);

// STATIC: 0x00E4D83C/0x140D36A80
bool (*ClientConnectionSuspendComms::s_handler)(ClientConnectionSuspendComms* msg);

// STATIC: 0x00E4D840/0x140D36A68
bool (*ClientConnectionPong::s_handler)(ClientConnectionPong* msg);

// STATIC: 0x00E4D844/0x140D36A48
bool (*ClientConnectionResumeComms::s_handler)(ClientConnectionResumeComms* msg);

// STATIC: 0x00E4D848/0x140D36A60
bool (*ClientConnectionNoop::s_handler)(ClientConnectionNoop* msg);

// STATIC: 0x00E4D84C/0x140D36A50
bool (*ClientConnectionResetCompressionContext::s_handler)(ClientConnectionResetCompressionContext* msg);

// STATIC: 0x00E4D850/0x140D36A70
bool (*ClientConnectionDropNewConnection::s_handler)(ClientConnectionDropNewConnection* msg);

// STATIC: 0x00E4D854/0x140D36A78
bool (*ClientConnectionConnectTo::s_handler)(ClientConnectionConnectTo* msg);

// PROVEN: 2a30dabb IDENTICAL/DIFFERENT
// OFFSET: 0x006964B0/0x14033F820
ClientConnectionAuthChallenge::ClientConnectionAuthChallenge(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: f581d7f3 IDENTICAL/IDENTICAL
// OFFSET: 0x00694900/0x14033F7A0
ClientConnectionAuthChallenge::~ClientConnectionAuthChallenge() {
}

// PROVEN: 92547180 IDENTICAL/IDENTICAL
// OFFSET: 0x00694BC0/0x14033FB90
void ClientConnectionAuthChallenge::Unpack(CDataStore* msg, bool full) {
    for (uint32_t i = 0; i < 8; i++) {
        uint32_t value = 0;
        msg->Get(value);
        this->m_dosChallenge[i] = value;
    }

    {
        uint32_t challenge = 0;
        msg->Get(challenge);
        this->m_challenge = challenge;
    }

    {
        uint8_t dosZeroBits = 0;
        msg->Get(dosZeroBits);
        this->m_dosZeroBits = dosZeroBits;
    }
}

// PROVEN: 7204f30c IDENTICAL/NONE
// OFFSET: 0x006964F0
bool ClientConnectionAuthChallenge::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: 0bc30a6c DIFFERENT/IDENTICAL
// OFFSET: 0x00694C80/0x14033FC90
bool ClientConnectionAuthChallenge::Read(CDataStore* msg, bool full) {
    for (uint32_t i = 0; i < 8; i++) {
        uint32_t value = 0;
        msg->Get(value);
        this->m_dosChallenge[i] = value;
    }

    {
        uint32_t challenge = 0;
        msg->Get(challenge);
        this->m_challenge = challenge;
    }

    {
        uint8_t dosZeroBits = 0;
        msg->Get(dosZeroBits);
        this->m_dosZeroBits = dosZeroBits;
    }

    return msg->IsRead() != 0;
}

// PROVEN: 6840c443 IDENTICAL/DIFFERENT
// OFFSET: 0x00694D90/0x14033FC30
bool ClientConnectionAuthChallenge::Handle() {
    if (ClientConnectionAuthChallenge::s_handler) {
        uint32_t right = g_jamHandlerKey_CFB738.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFB738.m_left->Get();
        return reinterpret_cast<bool (*)(ClientConnectionAuthChallenge*)>(reinterpret_cast<uintptr_t>(ClientConnectionAuthChallenge::s_handler) - (right ^ ((right & 0xF) * left)))(this);
    }

    return false;
}

// PROVEN: 13fc2c7d IDENTICAL/DIFFERENT
// OFFSET: 0x00694D00/0x14036ACB0
ClientConnectionConnectTo::ClientConnectionConnectTo(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->Unpack(msg, true);
}

// PROVEN: f0b86afe IDENTICAL/UNPROVEN
// OFFSET: 0x00694920/0x14033F780
ClientConnectionConnectTo::~ClientConnectionConnectTo() {
}

// PROVEN: ed937980 IDENTICAL/IDENTICAL
// OFFSET: 0x00694AF0/0x14033FA70
void ClientConnectionConnectTo::Unpack(CDataStore* msg, bool full) {
    {
        uint64_t key = 0;
        msg->Get(key);
        this->m_key = key;
    }

    {
        uint32_t serial = 0;
        msg->Get(serial);
        this->m_serial = serial;
    }

    for (uint32_t i = 0; i < sizeof(this->m_payload); i++) {
        uint8_t value = 0;
        msg->Get(value);
        this->m_payload[i] = value;
    }

    {
        uint8_t con = 0;
        msg->Get(con);
        this->m_con = con;
    }
}

// PROVEN: f8e8594a IDENTICAL/IDENTICAL
// OFFSET: 0x00694AE0/0x140363360
bool ClientConnectionConnectTo::Get(CDataStore* msg, bool full) {
    return this->Read(msg, full);
}

// PROVEN: 3f8dc78a IDENTICAL/IDENTICAL
// OFFSET: 0x00694A50/0x14033F990
bool ClientConnectionConnectTo::Read(CDataStore* msg, bool full) {
    {
        uint64_t key = 0;
        msg->Get(key);
        this->m_key = key;
    }

    {
        uint32_t serial = 0;
        msg->Get(serial);
        this->m_serial = serial;
    }

    for (uint32_t i = 0; i < sizeof(this->m_payload); i++) {
        uint8_t value = 0;
        msg->Get(value);
        this->m_payload[i] = value;
    }

    {
        uint8_t con = 0;
        msg->Get(con);
        this->m_con = con;
    }

    return msg->IsRead() != 0;
}

// PROVEN: cde029c5 IDENTICAL/DIFFERENT
// OFFSET: 0x00694E20/0x14033FE90
void ClientConnectPayload::Get(CDataStore* msg, bool full) {
    uint8_t value = 0;
    msg->Get(value);
    this->m_piDigits[30] = value;

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_haiku[31] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_piDigits[21] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_piDigits[26] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_haiku[3] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_piDigits[78] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_haiku[16] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_haiku[55] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_haiku[54] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_piDigits[68] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_piDigits[59] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_piDigits[115] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_haiku[65] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_hmac[15] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_haiku[25] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_piDigits[32] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_piDigits[101] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_haiku[37] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_haiku[20] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_haiku[29] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_piDigits[88] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_haiku[9] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_haiku[63] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_piDigits[3] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_piDigits[22] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_haiku[14] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_piDigits[38] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_piDigits[46] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_where.m_ipv6[1] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_piDigits[94] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_piDigits[96] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_piDigits[137] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_haiku[42] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_haiku[21] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_piDigits[13] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_hmac[14] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_piDigits[63] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_piDigits[16] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_haiku[5] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_piDigits[58] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_haiku[67] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_haiku[53] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_piDigits[79] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_where.m_ipv6[14] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_where.m_ipv6[9] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_piDigits[125] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_haiku[24] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_haiku[6] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_piDigits[140] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_haiku[8] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_piDigits[112] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_piDigits[133] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_piDigits[74] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_piDigits[135] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_piDigits[50] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_piDigits[14] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_piDigits[85] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_hmac[19] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_piDigits[52] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_hmac[17] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_piDigits[136] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_piDigits[71] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_where.m_ipv6[6] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_piDigits[87] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_piDigits[28] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_piDigits[105] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_haiku[32] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_piDigits[75] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_haiku[46] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_piDigits[5] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_piDigits[104] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_haiku[17] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_piDigits[64] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_haiku[22] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_where.m_ipv6[3] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_port[0] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_haiku[23] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_piDigits[0] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_where.m_ipv6[5] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_piDigits[110] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_piDigits[109] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_piDigits[93] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_haiku[10] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_xorMagic = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_haiku[26] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_haiku[13] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_piDigits[90] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_piDigits[72] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_piDigits[6] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_piDigits[54] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_where.m_ipv6[0] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_piDigits[23] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_piDigits[100] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_haiku[39] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_piDigits[86] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_piDigits[82] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_haiku[56] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_piDigits[95] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_hmac[18] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_piDigits[113] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_haiku[38] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_hmac[8] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_piDigits[92] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_piDigits[42] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_piDigits[120] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_piDigits[55] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_piDigits[124] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_haiku[30] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_piDigits[4] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_haiku[18] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_piDigits[123] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_where.m_ipv6[8] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_piDigits[61] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_piDigits[122] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_haiku[19] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_piDigits[53] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_where.m_ipv6[2] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_hmac[11] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_piDigits[31] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_piDigits[36] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_haiku[2] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_haiku[57] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_haiku[40] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_piDigits[70] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_haiku[34] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_piDigits[132] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_piDigits[20] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_piDigits[107] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_piDigits[141] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_piDigits[97] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_hmac[2] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_haiku[60] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_piDigits[102] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_piDigits[116] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_piDigits[49] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_piDigits[37] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_piDigits[48] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_piDigits[18] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_haiku[69] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_hmac[12] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_piDigits[65] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_hmac[3] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_haiku[27] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_piDigits[118] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_piDigits[44] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_haiku[50] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_haiku[59] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_piDigits[81] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_piDigits[51] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_where.m_ipv6[4] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_piDigits[12] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_piDigits[27] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_where.m_ipv6[11] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_piDigits[40] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_piDigits[139] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_haiku[51] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_haiku[64] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_piDigits[111] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_piDigits[131] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_haiku[1] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_haiku[49] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_piDigits[41] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_haiku[28] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_piDigits[77] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_piDigits[76] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_piDigits[8] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_where.m_ipv6[12] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_haiku[62] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_piDigits[19] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_piDigits[17] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_piDigits[24] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_haiku[72] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_hmac[13] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_haiku[61] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_piDigits[29] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_piDigits[15] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_where.m_ipv6[7] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_piDigits[121] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_piDigits[69] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_where.m_ipv6[13] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_haiku[35] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_piDigits[103] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_piDigits[39] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_hmac[5] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_haiku[4] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_piDigits[34] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_piDigits[56] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_port[1] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_hmac[10] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_piDigits[80] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_piDigits[130] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_haiku[12] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_piDigits[134] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_piDigits[33] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_piDigits[25] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_piDigits[73] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_piDigits[138] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_piDigits[9] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_piDigits[66] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_piDigits[1] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_haiku[45] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_piDigits[126] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_piDigits[67] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_haiku[33] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_piDigits[10] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_hmac[4] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_hmac[9] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_haiku[44] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_piDigits[60] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_piDigits[98] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_piDigits[91] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_hmac[1] = value;
    }

    {
        uint8_t type = 0;
        msg->Get(type);
        this->m_where.m_type = type;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_piDigits[11] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_piDigits[83] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_hmac[0] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_haiku[52] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_haiku[43] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_piDigits[47] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_haiku[11] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_piDigits[129] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_haiku[0] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_piDigits[57] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_piDigits[7] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_hmac[7] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_haiku[15] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_haiku[58] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_haiku[66] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_piDigits[127] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_haiku[41] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_where.m_ipv6[10] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_haiku[71] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_piDigits[99] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_piDigits[117] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_piDigits[62] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_piDigits[89] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_haiku[70] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_hmac[6] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_piDigits[114] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_piDigits[106] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_piDigits[108] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_haiku[48] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_haiku[7] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_piDigits[2] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_piDigits[43] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_haiku[36] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_piDigits[45] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_piDigits[119] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_haiku[47] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_haiku[68] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_hmac[16] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_piDigits[128] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_where.m_ipv6[15] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_piDigits[35] = value;
    }

    {
        uint8_t value = 0;
        msg->Get(value);
        this->m_piDigits[84] = value;
    }
}

// PROVEN: feb93af3 IDENTICAL/DIFFERENT
// OFFSET: 0x00694A00/0x14033FE30
bool ClientConnectionConnectTo::Handle() {
    if (ClientConnectionConnectTo::s_handler) {
        uint32_t right = g_jamHandlerKey_CFB74C.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFB74C.m_left->Get();
        return reinterpret_cast<bool (*)(ClientConnectionConnectTo*)>(reinterpret_cast<uintptr_t>(ClientConnectionConnectTo::s_handler) - ((left << (right & 0xF)) | (left >> (32 - (right & 0xF)))))(this);
    }

    return false;
}

// OFFSET: 0x00696520/0x14033FD30
ClientConnectionPong::ClientConnectionPong(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->m_present = 0;

    uint32_t serial = 0;
    msg->Get(serial);
    this->m_present |= 0x1;
    this->m_serial = serial;
}

// PROVEN: 4de97fa3 IDENTICAL/UNPROVEN
// OFFSET: 0x00694910/0x14033F7B0
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

// PROVEN: 00e0edc4 IDENTICAL/UNPROVEN
// OFFSET: 0x00694C30/0x14033FD70
bool ClientConnectionPong::Handle() {
    if (ClientConnectionPong::s_handler) {
        uint32_t right = g_jamHandlerKey_CFB780.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFB780.m_left->Get();
        return reinterpret_cast<bool (*)(ClientConnectionPong*)>(reinterpret_cast<uintptr_t>(ClientConnectionPong::s_handler) - ((left << (right & 0xF)) | (left >> (32 - (right & 0xF)))))(this);
    }

    return false;
}

// OFFSET: 0x00696520/0x14033FD30
ClientConnectionSuspendComms::ClientConnectionSuspendComms(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
    this->m_present = 0;

    uint32_t serial = 0;
    msg->Get(serial);
    this->m_present |= 0x1;
    this->m_serial = serial;
}

// PROVEN: f23a8ac7 IDENTICAL/UNPROVEN
// OFFSET: 0x00694910/0x14033F7B0
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

// PROVEN: b3130309 IDENTICAL/DIFFERENT
// OFFSET: 0x00694930/0x14033F930
bool ClientConnectionSuspendComms::Handle() {
    if (ClientConnectionSuspendComms::s_handler) {
        uint32_t right = g_jamHandlerKey_CFB738.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFB738.m_left->Get();
        return reinterpret_cast<bool (*)(ClientConnectionSuspendComms*)>(reinterpret_cast<uintptr_t>(ClientConnectionSuspendComms::s_handler) - (right ^ ((right & 0xF) * left)))(this);
    }

    return false;
}

// PROVEN: 3b318ae9 IDENTICAL/DIFFERENT
// OFFSET: 0x006A2E40/0x14036ACB0
ClientConnectionResumeComms::ClientConnectionResumeComms(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
}

// PROVEN: bdba72f0 IDENTICAL/UNPROVEN
// OFFSET: 0x006A1120/0x14033F780
ClientConnectionResumeComms::~ClientConnectionResumeComms() {
}

// PROVEN: e8fe07cb IDENTICAL/IDENTICAL
// OFFSET: 0x006BAEE0/0x140363360
bool ClientConnectionResumeComms::Get(CDataStore* msg, bool full) {
    return msg->IsRead() != 0;
}

// PROVEN: 26cc65bc IDENTICAL/DIFFERENT
// OFFSET: 0x00694980/0x14033FDD0
bool ClientConnectionResumeComms::Handle() {
    if (ClientConnectionResumeComms::s_handler) {
        uint32_t right = g_jamHandlerKey_CFB688.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFB688.m_left->Get();
        return reinterpret_cast<bool (*)(ClientConnectionResumeComms*)>(reinterpret_cast<uintptr_t>(ClientConnectionResumeComms::s_handler) - (left - right))(this);
    }

    return false;
}

// PROVEN: f623d0ae IDENTICAL/DIFFERENT
// OFFSET: 0x006A2E40/0x14036ACB0
ClientConnectionResetCompressionContext::ClientConnectionResetCompressionContext(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
}

// PROVEN: 162fb9ed IDENTICAL/UNPROVEN
// OFFSET: 0x006A1120/0x14033F780
ClientConnectionResetCompressionContext::~ClientConnectionResetCompressionContext() {
}

// PROVEN: 71bff20b IDENTICAL/IDENTICAL
// OFFSET: 0x006BAEE0/0x140363360
bool ClientConnectionResetCompressionContext::Get(CDataStore* msg, bool full) {
    return msg->IsRead() != 0;
}

// PROVEN: 1cbe5843 IDENTICAL/DIFFERENT
// OFFSET: 0x006949C0/0x14033F7C0
bool ClientConnectionResetCompressionContext::Handle() {
    if (ClientConnectionResetCompressionContext::s_handler) {
        uint32_t right = g_jamHandlerKey_CFB688.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFB688.m_left->Get();
        return reinterpret_cast<bool (*)(ClientConnectionResetCompressionContext*)>(reinterpret_cast<uintptr_t>(ClientConnectionResetCompressionContext::s_handler) - (left - right))(this);
    }

    return false;
}

// PROVEN: 6f10a356 IDENTICAL/DIFFERENT
// OFFSET: 0x006A2E40/0x14036ACB0
ClientConnectionDropNewConnection::ClientConnectionDropNewConnection(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
}

// PROVEN: 265d5f07 IDENTICAL/UNPROVEN
// OFFSET: 0x006A1120/0x14033F780
ClientConnectionDropNewConnection::~ClientConnectionDropNewConnection() {
}

// PROVEN: 3d0ceb3e IDENTICAL/IDENTICAL
// OFFSET: 0x006BAEE0/0x140363360
bool ClientConnectionDropNewConnection::Get(CDataStore* msg, bool full) {
    return msg->IsRead() != 0;
}

// PROVEN: 95137c9d IDENTICAL/DIFFERENT
// OFFSET: 0x00694DE0/0x14033F8D0
bool ClientConnectionDropNewConnection::Handle() {
    if (ClientConnectionDropNewConnection::s_handler) {
        uint32_t right = g_jamHandlerKey_CFB718.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFB718.m_left->Get();
        return reinterpret_cast<bool (*)(ClientConnectionDropNewConnection*)>(reinterpret_cast<uintptr_t>(ClientConnectionDropNewConnection::s_handler) - (right + left))(this);
    }

    return false;
}

// PROVEN: 7d8ac1df IDENTICAL/DIFFERENT
// OFFSET: 0x006A2E40/0x14036ACB0
ClientConnectionNoop::ClientConnectionNoop(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time) : JamClientMessage(netClient, connection, time) {
}

// PROVEN: a5e67d61 IDENTICAL/UNPROVEN
// OFFSET: 0x006A1120/0x14033F780
ClientConnectionNoop::~ClientConnectionNoop() {
}

// PROVEN: 704133ec IDENTICAL/IDENTICAL
// OFFSET: 0x006BAEE0/0x140363360
bool ClientConnectionNoop::Get(CDataStore* msg, bool full) {
    return msg->IsRead() != 0;
}

// PROVEN: 37285460 IDENTICAL/DIFFERENT
// OFFSET: 0x00694D40/0x14033F870
bool ClientConnectionNoop::Handle() {
    if (ClientConnectionNoop::s_handler) {
        uint32_t right = g_jamHandlerKey_CFB708.m_right->Get();
        uint32_t left = g_jamHandlerKey_CFB708.m_left->Get();
        return reinterpret_cast<bool (*)(ClientConnectionNoop*)>(reinterpret_cast<uintptr_t>(ClientConnectionNoop::s_handler) - ((left << (right & 0xF)) | (left >> (32 - (right & 0xF)))))(this);
    }

    return false;
}

// PROVEN: 8516bb88 IDENTICAL/DIFFERENT
// OFFSET: 0x0066A970/0x1403177D0
UserRouterClientPing::UserRouterClientPing() {
    this->m_present = 0;
}

// PROVEN: 2b8c4c5a IDENTICAL/NONE
// OFFSET: 0x0066A3D0
UserRouterClientPing::~UserRouterClientPing() {
}

// PROVEN: 85a8ce9e IDENTICAL/IDENTICAL
// OFFSET: 0x0066A910/0x140317770
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

// PROVEN: f5d00f5a IDENTICAL/IDENTICAL
// OFFSET: 0x0066A9E0/0x140317840
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

// PROVEN: 1408bd2d IDENTICAL/IDENTICAL
// OFFSET: 0x0066A990/0x1403177F0
void UserRouterClientSuspendCommsAck::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x4469));
    msg->Put(this->m_serial);
}

// PROVEN: c72426c1 IDENTICAL/DIFFERENT
// OFFSET: 0x0066A950/0x1403177B0
UserRouterClientSuspendTokenResponse::UserRouterClientSuspendTokenResponse() {
    this->m_present = 0;
}

// PROVEN: 43d88893 IDENTICAL/IDENTICAL
// OFFSET: 0x0066A400/0x140317560
UserRouterClientSuspendTokenResponse::~UserRouterClientSuspendTokenResponse() {
}

// PROVEN: a1d353e0 IDENTICAL/IDENTICAL
// OFFSET: 0x0066A7A0/0x1403175E0
void UserRouterClientSuspendTokenResponse::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x46D));
    msg->Put(this->m_token);
}

// PROVEN: 57cf04cd IDENTICAL/IDENTICAL
// OFFSET: 0x0066A760/0x140317590
UserRouterClientEnableNagle::UserRouterClientEnableNagle() {
}

// PROVEN: 74bf4a09 IDENTICAL/IDENTICAL
// OFFSET: 0x0066A410/0x140317570
UserRouterClientEnableNagle::~UserRouterClientEnableNagle() {
}

// PROVEN: 8c55ffb7 IDENTICAL/IDENTICAL
// OFFSET: 0x0066A9C0/0x140317830
void UserRouterClientEnableNagle::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x4449));
}

// PROVEN: eedd4fd9 IDENTICAL/IDENTICAL
// OFFSET: 0x0066A900/0x140317750
UserRouterClientAuthContinuedSession::UserRouterClientAuthContinuedSession() {
}

// PROVEN: b3c07a7c IDENTICAL/IDENTICAL
// OFFSET: 0x0066A420/0x140317580
UserRouterClientAuthContinuedSession::~UserRouterClientAuthContinuedSession() {
}

// PROVEN: f746efcf IDENTICAL/IDENTICAL
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

// PROVEN: 21e06934 IDENTICAL/IDENTICAL
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
    bits.WriteBits<12>(strlen(this->m_account), JamBitCount<true>());
    bits.Flush();

    msg->PutData(this->m_account, strlen(this->m_account));
}

// PROVEN: 24b5eaf7 IDENTICAL/IDENTICAL
// OFFSET: 0x0066DE80/0x140315070
UserClientConnectToFailed::UserClientConnectToFailed() {
}

// PROVEN: f332452c IDENTICAL/IDENTICAL
// OFFSET: 0x0066BBA0/0x1403123A0
UserClientConnectToFailed::~UserClientConnectToFailed() {
}

// PROVEN: 65f06b2d IDENTICAL/IDENTICAL
// OFFSET: 0x0066E520/0x140315740
void UserClientConnectToFailed::Put(CDataStore* msg) {
    msg->Put(static_cast<uint32_t>(0x2533));
    msg->Put(this->m_serial);
    msg->Put(this->m_con);
}

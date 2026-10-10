#ifndef WOW_SOURCE_JAMCLIENTAUTOCODE_JAMCLIENTCONNECTION_H
#define WOW_SOURCE_JAMCLIENTAUTOCODE_JAMCLIENTCONNECTION_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Os/SockAddr.h"
#include "WoW/Source/JamClientAutoCode/JamClientMessage.h"
#include "WoW/Source/WowServices/WDataStore.h"

class ClientConnectionAuthChallenge : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientConnectionAuthChallenge* msg);

    /* 0010 */ uint32_t m_dosChallenge[8];
    /* 0030 */ uint32_t m_challenge;
    /* 0034 */ uint8_t m_dosZeroBits;

    bool Get(CDataStore* msg, bool full) override;

    ClientConnectionAuthChallenge(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientConnectionAuthChallenge();
    bool Handle();
    void Unpack(CDataStore* msg, bool full);
};

class ClientConnectPayload {
    public:
    /* 0000 */ SockAddr m_where;
    /* 0014 */ uint8_t m_xorMagic;
    /* 0015 */ uint8_t m_port[2];
    /* 0017 */ char m_haiku[73];
    /* 0060 */ uint8_t m_piDigits[142];
    /* 00EE */ uint8_t m_hmac[20];

    void Get(CDataStore* msg, bool full);
};

class ClientConnectionConnectTo : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientConnectionConnectTo* msg);

    /* 0010 */ uint64_t m_key;
    /* 0018 */ uint32_t m_serial;
    /* 001C */ uint8_t m_payload[256];
    /* 011C */ uint8_t m_con;

    bool Get(CDataStore* msg, bool full) override;

    ClientConnectionConnectTo(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientConnectionConnectTo();
    bool Handle();
    void Unpack(CDataStore* msg, bool full);
};

class ClientConnectionPong : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientConnectionPong* msg);

    /* 0010 */ uint32_t m_present;
    /* 0014 */ uint32_t m_serial;

    bool Get(CDataStore* msg, bool full) override;

    ClientConnectionPong(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientConnectionPong();
    bool Handle();
};

class ClientConnectionSuspendComms : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientConnectionSuspendComms* msg);

    /* 0010 */ uint32_t m_present;
    /* 0014 */ uint32_t m_serial;

    bool Get(CDataStore* msg, bool full) override;

    ClientConnectionSuspendComms(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientConnectionSuspendComms();
    bool Handle();
};

class ClientConnectionResumeComms : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientConnectionResumeComms* msg);

    bool Get(CDataStore* msg, bool full) override;

    ClientConnectionResumeComms(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientConnectionResumeComms();
    bool Handle();
};

class ClientConnectionResetCompressionContext : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientConnectionResetCompressionContext* msg);

    bool Get(CDataStore* msg, bool full) override;

    ClientConnectionResetCompressionContext(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientConnectionResetCompressionContext();
    bool Handle();
};

class ClientConnectionDropNewConnection : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientConnectionDropNewConnection* msg);

    bool Get(CDataStore* msg, bool full) override;

    ClientConnectionDropNewConnection(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientConnectionDropNewConnection();
    bool Handle();
};

class ClientConnectionNoop : public JamClientMessage {
    public:
    static bool (*s_handler)(ClientConnectionNoop* msg);

    bool Get(CDataStore* msg, bool full) override;

    ClientConnectionNoop(NetClient* netClient, WowConnection* connection, CDataStore* msg, uint32_t time);
    ~ClientConnectionNoop();
    bool Handle();
};

class UserRouterClientPing : public JamClientMessage {
    public:
    /* 0010 */ uint32_t m_present;
    /* 0014 */ uint32_t m_latency;
    /* 0018 */ uint32_t m_serial;

    void Put(CDataStore* msg) override;

    UserRouterClientPing();
    ~UserRouterClientPing();
};

class UserRouterClientLogDisconnect : public JamClientMessage {
    public:
    /* 0010 */ uint32_t m_present;
    /* 0014 */ uint32_t m_reason;

    void Put(CDataStore* msg) override;

    UserRouterClientLogDisconnect(uint32_t reason);
    ~UserRouterClientLogDisconnect();
};

class UserRouterClientSuspendCommsAck : public JamClientMessage {
    public:
    /* 0010 */ uint32_t m_present;
    /* 0014 */ uint32_t m_serial;

    void Put(CDataStore* msg) override;

    UserRouterClientSuspendCommsAck();
    ~UserRouterClientSuspendCommsAck();
};

class UserRouterClientSuspendTokenResponse : public JamClientMessage {
    public:
    /* 0010 */ uint32_t m_present;
    /* 0014 */ uint32_t m_token;

    void Put(CDataStore* msg) override;

    UserRouterClientSuspendTokenResponse();
    ~UserRouterClientSuspendTokenResponse();
};

class UserRouterClientEnableNagle : public JamClientMessage {
    public:
    void Put(CDataStore* msg) override;

    UserRouterClientEnableNagle();
    ~UserRouterClientEnableNagle();
};

class UserRouterClientAuthContinuedSession : public JamClientMessage {
    public:
    /* 0010 */ uint64_t m_dosResponse;
    /* 0018 */ uint64_t m_key;
    /* 0020 */ uint8_t m_digest[20];

    void Put(CDataStore* msg) override;

    UserRouterClientAuthContinuedSession();
    ~UserRouterClientAuthContinuedSession();
};

class UserRouterClientAuthSession : public JamClientMessage {
    public:
    /* 0010 */ uint32_t m_regionID;
    /* 0014 */ uint32_t m_localChallenge;
    /* 0018 */ uint8_t m_unk0018;
    /* 0019 */ uint8_t unk_0019[3];
    /* 001C */ uint32_t m_loginServerID;
    /* 0020 */ uint32_t m_realmID;
    /* 0024 */ uint32_t m_battlegroupID;
    /* 0028 */ uint8_t m_loginServerType;
    /* 0029 */ uint8_t unk_0029;
    /* 002A */ uint16_t m_build;
    /* 002C */ uint8_t m_digest[20];
    /* 0040 */ uint64_t m_dosResponse;
    /* 0048 */ bool m_useIPv6;
    /* 0049 */ char m_account[1283];
    /* 054C */ WDataStore m_addonInfo;

    void Put(CDataStore* msg) override;

    UserRouterClientAuthSession();
    ~UserRouterClientAuthSession();
};

class UserClientConnectToFailed : public JamClientMessage {
    public:
    /* 0010 */ uint32_t m_serial;
    /* 0014 */ uint8_t m_con;

    void Put(CDataStore* msg) override;

    UserClientConnectToFailed();
    ~UserClientConnectToFailed();
};

#endif

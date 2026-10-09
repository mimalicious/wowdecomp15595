#ifndef WOW_SOURCE_JAMCLIENTAUTOCODE_JAMCLIENTMESSAGE_H
#define WOW_SOURCE_JAMCLIENTAUTOCODE_JAMCLIENTMESSAGE_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Base/CDataStore.h"

class NetClient;
class WowConnection;

class JamBitReader {
    public:
    /* 0000 */ CDataStore* m_msg;
    /* 0004 */ uint8_t m_value;
    /* 0005 */ uint8_t m_bits;

    JamBitReader(CDataStore* msg);
    bool ReadBit();
    template <uint32_t count>
    uint32_t ReadBits();
};

template <bool wide>
struct JamBitCount {
};

class JamBitWriter {
    public:
    /* 0000 */ CDataStore* m_msg;
    /* 0004 */ uint8_t m_value;
    /* 0005 */ uint8_t m_bits;

    JamBitWriter(CDataStore* msg);
    void Flush();
    void WriteBit(uint32_t value);
    template <uint32_t count>
    void WriteBits(uint32_t value);
    template <uint32_t count>
    void WriteBits(uint32_t value, JamBitCount<false>);
    template <uint32_t count>
    void WriteBits(uint32_t value, JamBitCount<true>);
};

struct JAMMESSAGEINFO {
    /* 0000 */ const void* m_desc;
    /* 0004 */ uint32_t m_size;
    /* 0008 */ uint32_t m_flags;
};

class JamClientMessage {
    public:
    /* 0004 */ NetClient* m_netClient;
    /* 0008 */ WowConnection* m_connection;
    /* 000C */ uint32_t m_time;

    virtual void Put(CDataStore* msg);
    virtual bool Get(CDataStore* msg, bool full);
    virtual JAMMESSAGEINFO GetInfo();

    JamClientMessage();
    JamClientMessage(NetClient* netClient, WowConnection* connection, uint32_t time);
};

#include "WoW/Source/JamClientAutoCode/JamClientMessage.inl"

#endif

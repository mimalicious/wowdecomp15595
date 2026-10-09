#ifndef WOW_SOURCE_OBJECT_OBJECTCLIENT_GUILDSTATS_C_H
#define WOW_SOURCE_OBJECT_OBJECTCLIENT_GUILDSTATS_C_H

#include <cstddef>
#include <cstdint>

class CDataStore;

class GuildStats_C {
    public:
    static const uint32_t s_cacheVersion = 1;

    /* 0000 */ uint64_t m_guildGuid;
    /* 0008 */ char m_name [96];
    /* 0068 */ char m_rankNames [10][64];
    /* 02E8 */ int32_t m_rankIds [10];
    /* 0310 */ int32_t m_rankOrders [10];
    /* 0338 */ int32_t m_emblemStyle;
    /* 033C */ int32_t m_emblemColor;
    /* 0340 */ int32_t m_borderStyle;
    /* 0344 */ int32_t m_borderColor;
    /* 0348 */ int32_t m_background;
    /* 034C */ int32_t m_numRanks;

    void Pack(CDataStore* msg);
    void Unpack(CDataStore* msg);
};

#endif

#ifndef WOW_SOURCE_DB_REC_KEYCHAINREC_H
#define WOW_SOURCE_DB_REC_KEYCHAINREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB2.h"

class KeyChainRec {
    public:
    static const DB2Meta s_meta;

    static void FixupRecord(void* record);

    /* 0000 */ int32_t m_ID;
    /* 0004 */ uint8_t m_key[32];
};

extern WowClientDB2<KeyChainRec> g_keyChainDB;

#endif

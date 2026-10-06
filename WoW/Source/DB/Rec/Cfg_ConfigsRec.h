#ifndef WOW_SOURCE_DB_REC_CFG_CONFIGSREC_H
#define WOW_SOURCE_DB_REC_CFG_CONFIGSREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class Cfg_ConfigsRec {
    public:
    static const DBCMeta s_meta;

    /* 0000 */ int32_t m_generatedID;
    /* 0004 */ int32_t m_id;
    /* 0008 */ int32_t m_realmType;
    /* 000C */ int32_t m_playerKillingAllowed;
    /* 0010 */ int32_t m_roleplaying;

    static void FixupStrings(void* record, const char* strings);
#if defined(WOWDECOMP_ARCH_64)
    static void ConvertRow(Cfg_ConfigsRec* rec, const uint8_t* row, const char* strings);
#endif
};

#endif

#ifndef WOW_SOURCE_DB_REC_ITEMSUBCLASSREC_H
#define WOW_SOURCE_DB_REC_ITEMSUBCLASSREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class ItemSubClassRec {
    public:
    static const DBCMeta s_meta;

    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_classID;
    /* 0008 */ int32_t m_subClassID;
    /* 000C */ int32_t m_prerequisiteProficiency;
    /* 0010 */ int32_t m_postrequisiteProficiency;
    /* 0014 */ int32_t m_flags;
    /* 0018 */ int32_t m_displayFlags;
    /* 001C */ int32_t m_weaponParrySeq;
    /* 0020 */ int32_t m_weaponReadySeq;
    /* 0024 */ int32_t m_weaponAttackSeq;
    /* 0028 */ int32_t m_weaponSwingSize;
    /* 002C */ DBCString m_displayName;
    /* 0030 */ DBCString m_verboseName;

    static void FixupStrings(void* record, const char* strings);
};

#endif

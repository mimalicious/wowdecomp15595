#ifndef WOW_SOURCE_DB_REC_EMOTESREC_H
#define WOW_SOURCE_DB_REC_EMOTESREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class EmotesRec {
    public:
    static const DBCMeta s_meta;

    /* 0000 */ int32_t m_id;
    /* 0004 */ DBCString m_emoteSlashCommand;
    /* 0008 */ int32_t m_animID;
    /* 000C */ int32_t m_emoteFlags;
    /* 0010 */ int32_t m_emoteSpecProc;
    /* 0014 */ int32_t m_emoteSpecProcParam;
    /* 0018 */ int32_t m_eventSoundID;
    /* 001C */ int32_t unk_001C;

    static void FixupStrings(void* record, const char* strings);
};

#endif

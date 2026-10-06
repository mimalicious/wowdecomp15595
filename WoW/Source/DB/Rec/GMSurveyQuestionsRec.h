#ifndef WOW_SOURCE_DB_REC_GMSURVEYQUESTIONSREC_H
#define WOW_SOURCE_DB_REC_GMSURVEYQUESTIONSREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class GMSurveyQuestionsRec {
    public:
    static const DBCMeta s_meta;

    /* 0000 */ int32_t m_id;
    /* 0004 */ DBCString m_question;

    static void FixupStrings(void* record, const char* strings);
};

#endif

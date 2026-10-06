#ifndef WOW_SOURCE_DB_REC_GMSURVEYCURRENTSURVEYREC_H
#define WOW_SOURCE_DB_REC_GMSURVEYCURRENTSURVEYREC_H

#include <cstdint>
#include "WoW/Source/DB/WowClientDB.h"

class GMSurveyCurrentSurveyRec {
    public:
    static const DBCMeta s_meta;

    /* 0000 */ int32_t m_id;
    /* 0004 */ int32_t m_gmsurveyID;

    static void FixupStrings(void* record, const char* strings);
};

#endif

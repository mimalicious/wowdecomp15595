#include "WoW/Source/DB/Rec/GMSurveyCurrentSurveyRec.h"

// STATIC: %0x00B468A4
const DBCMeta GMSurveyCurrentSurveyRec::s_meta = { "DBFilesClient\\GMSurveyCurrentSurvey.dbc", 2, 8, GMSurveyCurrentSurveyRec::FixupStrings, 0 };

// PROVEN: 8635e68e IDENTICAL/NONE
// OFFSET: 0x00401000
void GMSurveyCurrentSurveyRec::FixupStrings(void* record, const char* strings) {
}

#if defined(WOWDECOMP_ARCH_64)
void GMSurveyCurrentSurveyRec::ConvertRow(GMSurveyCurrentSurveyRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    memcpy(&rec->m_gmsurveyID, &row[0x0004], sizeof(rec->m_gmsurveyID));
}
#endif

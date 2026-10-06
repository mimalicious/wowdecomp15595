#include "WoW/Source/DB/Rec/GMSurveyAnswersRec.h"

// STATIC: %0x00B468DC
const DBCMeta GMSurveyAnswersRec::s_meta = { "DBFilesClient\\GMSurveyAnswers.dbc", 4, 16, GMSurveyAnswersRec::FixupStrings, 0 };

// OFFSET: %0x0097C4E0
void GMSurveyAnswersRec::FixupStrings(void* record, const char* strings) {
    GMSurveyAnswersRec* rec = static_cast<GMSurveyAnswersRec*>(record);
    rec->m_answer = &strings[reinterpret_cast<uintptr_t>(rec->m_answer)];
}

#if defined(WOWDECOMP_ARCH_64)
void GMSurveyAnswersRec::ConvertRow(GMSurveyAnswersRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    memcpy(&rec->m_sortIndex, &row[0x0004], sizeof(rec->m_sortIndex));
    memcpy(&rec->m_gmsurveyQuestionID, &row[0x0008], sizeof(rec->m_gmsurveyQuestionID));
    rec->m_answer = &strings[*reinterpret_cast<const uint32_t*>(&row[0x000C])];
}
#endif

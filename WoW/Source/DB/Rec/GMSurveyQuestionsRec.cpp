#include "WoW/Source/DB/Rec/GMSurveyQuestionsRec.h"

// STATIC: %0x00B46868
const DBCMeta GMSurveyQuestionsRec::s_meta = { "DBFilesClient\\GMSurveyQuestions.dbc", 2, 8, GMSurveyQuestionsRec::FixupStrings, 0 };

// OFFSET: %0x0097C040
void GMSurveyQuestionsRec::FixupStrings(void* record, const char* strings) {
    GMSurveyQuestionsRec* rec = static_cast<GMSurveyQuestionsRec*>(record);
    rec->m_question = &strings[reinterpret_cast<uintptr_t>(rec->m_question)];
}

#if defined(WOWDECOMP_ARCH_64)
void GMSurveyQuestionsRec::ConvertRow(GMSurveyQuestionsRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    rec->m_question = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0004])];
}
#endif

#include "WoW/Source/DB/Rec/GMSurveyQuestionsRec.h"

// STATIC: 0x00C25138/0x140A1ABE8
const DBCMeta GMSurveyQuestionsRec::s_meta = { "DBFilesClient\\GMSurveyQuestions.dbc", 2, 8, GMSurveyQuestionsRec::FixupStrings, 0 };

// PROVEN: 40700b7a IDENTICAL/NONE
// OFFSET: 0x00A2EBA0/0x1407D8330
void GMSurveyQuestionsRec::FixupStrings(void* record, const char* strings) {
    GMSurveyQuestionsRec* rec = static_cast<GMSurveyQuestionsRec*>(record);
    DBCFixupString(rec->m_question, strings);
}

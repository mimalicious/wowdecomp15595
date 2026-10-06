#include "WoW/Source/DB/Rec/GMSurveyAnswersRec.h"

// STATIC: 0x00C2520C/0x140A1ACF0
const DBCMeta GMSurveyAnswersRec::s_meta = { "DBFilesClient\\GMSurveyAnswers.dbc", 4, 16, GMSurveyAnswersRec::FixupStrings, 0 };

// PROVEN: 02ba00c6 IDENTICAL/NONE
// OFFSET: 0x00A2ECE0/0x1407D8380
void GMSurveyAnswersRec::FixupStrings(void* record, const char* strings) {
    GMSurveyAnswersRec* rec = static_cast<GMSurveyAnswersRec*>(record);
    DBCFixupString(rec->m_answer, strings);
}

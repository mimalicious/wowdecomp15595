#include "WoW/Source/DB/Rec/GMSurveyQuestionsRec.h"

// STATIC: 0x00C25120/0x140A1ABD0
static const int32_t s_fieldOffsets[2] = { 0, 4 };

// STATIC: 0x00C25128/0x140A1ABD8
static const int32_t s_fieldSizes[2] = { 4, 4 };

// STATIC: 0x00C25130/0x140A1ABE0
static const int32_t s_fieldTypes[2] = { 0, 2 };

// STATIC: 0x00C25138/0x140A1ABE8
const DBCMeta GMSurveyQuestionsRec::s_meta = { "DBFilesClient\\GMSurveyQuestions.dbc", 2, 8, GMSurveyQuestionsRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// PROVEN: 40700b7a IDENTICAL/DIFFERENT
// OFFSET: 0x00A2EBA0/0x1407D8330
void GMSurveyQuestionsRec::FixupStrings(void* record, const char* strings) {
    GMSurveyQuestionsRec* rec = static_cast<GMSurveyQuestionsRec*>(record);
    DBCFixupString(rec->m_question, strings);
}

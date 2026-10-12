#include "WoW/Source/DB/Rec/GMSurveyAnswersRec.h"

// STATIC: 0x00C251DC/0x140A1ACC0
static const int32_t s_fieldOffsets[4] = { 0, 4, 8, 12 };

// STATIC: 0x00C251EC/0x140A1ACD0
static const int32_t s_fieldSizes[4] = { 4, 4, 4, 4 };

// STATIC: 0x00C251FC/0x140A1ACE0
static const int32_t s_fieldTypes[4] = { 0, 0, 0, 2 };

// STATIC: 0x00C2520C/0x140A1ACF0
const DBCMeta GMSurveyAnswersRec::s_meta = { "DBFilesClient\\GMSurveyAnswers.dbc", 4, 16, GMSurveyAnswersRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// OFFSET: 0x00A2ECE0/0x1407D8380
void GMSurveyAnswersRec::FixupStrings(void* record, const char* strings) {
    GMSurveyAnswersRec* rec = static_cast<GMSurveyAnswersRec*>(record);
    DBCFixupString(rec->m_answer, strings);
}

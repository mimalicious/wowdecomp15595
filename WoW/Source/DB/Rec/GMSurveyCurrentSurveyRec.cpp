#include "WoW/Source/DB/Rec/GMSurveyCurrentSurveyRec.h"

// STATIC: 0x00C25180/0x140A1AC48
static const int32_t s_fieldOffsets[2] = { 0, 4 };

// STATIC: 0x00C25188/0x140A1AC50
static const int32_t s_fieldSizes[2] = { 4, 4 };

// STATIC: 0x00C25190/0x140A1AC58
static const int32_t s_fieldTypes[2] = { 0, 0 };

// STATIC: 0x00C25198/0x140A1AC60
const DBCMeta GMSurveyCurrentSurveyRec::s_meta = { "DBFilesClient\\GMSurveyCurrentSurvey.dbc", 2, 8, GMSurveyCurrentSurveyRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// OFFSET: 0x00401000/0x1407D7E80
void GMSurveyCurrentSurveyRec::FixupStrings(void* record, const char* strings) {
}

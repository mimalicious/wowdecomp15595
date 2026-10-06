#include "WoW/Source/DB/Rec/GMSurveySurveysRec.h"

// STATIC: 0x00C250C4/0x140A1AB58
static const int32_t s_fieldOffsets[2] = { 0, 4 };

// STATIC: 0x00C250CC/0x140A1AB60
static const int32_t s_fieldSizes[2] = { 4, 60 };

// STATIC: 0x00C250D4/0x140A1AB68
static const int32_t s_fieldTypes[2] = { 0, 0 };

// STATIC: 0x00C250DC/0x140A1AB70
const DBCMeta GMSurveySurveysRec::s_meta = { "DBFilesClient\\GMSurveySurveys.dbc", 2, 64, GMSurveySurveysRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// PROVEN: 129dd48e IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void GMSurveySurveysRec::FixupStrings(void* record, const char* strings) {
}

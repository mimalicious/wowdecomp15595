#include "WoW/Source/DB/Rec/GMSurveySurveysRec.h"

// STATIC: %0x00B46830
const DBCMeta GMSurveySurveysRec::s_meta = { "DBFilesClient\\GMSurveySurveys.dbc", 2, 44, GMSurveySurveysRec::FixupStrings, 0 };

// PROVEN: 129dd48e IDENTICAL/NONE
// OFFSET: 0x00401000
void GMSurveySurveysRec::FixupStrings(void* record, const char* strings) {
}

#if defined(WOWDECOMP_ARCH_64)
void GMSurveySurveysRec::ConvertRow(GMSurveySurveysRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    memcpy(rec->unk_0004, &row[0x0004], sizeof(rec->unk_0004));
}
#endif

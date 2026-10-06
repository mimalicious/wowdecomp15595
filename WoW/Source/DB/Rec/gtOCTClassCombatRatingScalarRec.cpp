#include "WoW/Source/DB/Rec/gtOCTClassCombatRatingScalarRec.h"

// STATIC: %0x00B465DC
const DBCMeta gtOCTClassCombatRatingScalarRec::s_meta = { "DBFilesClient\\gtOCTClassCombatRatingScalar.dbc", 2, 8, gtOCTClassCombatRatingScalarRec::FixupStrings, 0 };

// PROVEN: b180743c IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void gtOCTClassCombatRatingScalarRec::FixupStrings(void* record, const char* strings) {
}

#if defined(WOWDECOMP_ARCH_64)
void gtOCTClassCombatRatingScalarRec::ConvertRow(gtOCTClassCombatRatingScalarRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    memcpy(&rec->m_data, &row[0x0004], sizeof(rec->m_data));
}
#endif

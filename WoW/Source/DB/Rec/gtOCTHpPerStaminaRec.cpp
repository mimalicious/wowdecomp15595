#include "WoW/Source/DB/Rec/gtOCTHpPerStaminaRec.h"

// STATIC: %0x00B46598
const DBCMeta gtOCTHpPerStaminaRec::s_meta = { "DBFilesClient\\gtOCTHpPerStamina.dbc", 1, 8, gtOCTHpPerStaminaRec::FixupStrings, 0 };

// PROVEN: 00304d14 IDENTICAL/NONE
// OFFSET: 0x00401000
void gtOCTHpPerStaminaRec::FixupStrings(void* record, const char* strings) {
}

#if defined(WOWDECOMP_ARCH_64)
void gtOCTHpPerStaminaRec::ConvertRow(gtOCTHpPerStaminaRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    memcpy(&rec->unk_0004, &row[0x0004], sizeof(rec->unk_0004));
}
#endif

#include "WoW/Source/DB/Rec/NumTalentsAtLevelRec.h"

// STATIC: %0x00B45B5C
const DBCMeta NumTalentsAtLevelRec::s_meta = { "DBFilesClient\\NumTalentsAtLevel.dbc", 2, 8, NumTalentsAtLevelRec::FixupStrings, 0 };

// PROVEN: cd3bd790 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void NumTalentsAtLevelRec::FixupStrings(void* record, const char* strings) {
}

#if defined(WOWDECOMP_ARCH_64)
void NumTalentsAtLevelRec::ConvertRow(NumTalentsAtLevelRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    memcpy(&rec->unk_0004, &row[0x0004], sizeof(rec->unk_0004));
}
#endif

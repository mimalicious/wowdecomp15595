#include "WoW/Source/DB/Rec/UnitBloodLevelsRec.h"

// STATIC: %0x00B44804
const DBCMeta UnitBloodLevelsRec::s_meta = { "DBFilesClient\\UnitBloodLevels.dbc", 2, 16, UnitBloodLevelsRec::FixupStrings, 0 };

// PROVEN: f2a7833a IDENTICAL
// OFFSET: 0x00401000
void UnitBloodLevelsRec::FixupStrings(void* record, const char* strings) {
}

#if defined(WOWDECOMP_ARCH_64)
void UnitBloodLevelsRec::ConvertRow(UnitBloodLevelsRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    memcpy(rec->m_violencelevel, &row[0x0004], sizeof(rec->m_violencelevel));
}
#endif

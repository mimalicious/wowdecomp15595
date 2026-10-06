#include "WoW/Source/DB/Rec/gtNPCManaCostScalerRec.h"

// STATIC: %0x00B46618
const DBCMeta gtNPCManaCostScalerRec::s_meta = { "DBFilesClient\\gtNPCManaCostScaler.dbc", 1, 8, gtNPCManaCostScalerRec::FixupStrings, 0 };

// PROVEN: 41825844 IDENTICAL/NONE
// OFFSET: 0x00401000
void gtNPCManaCostScalerRec::FixupStrings(void* record, const char* strings) {
}

#if defined(WOWDECOMP_ARCH_64)
void gtNPCManaCostScalerRec::ConvertRow(gtNPCManaCostScalerRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    memcpy(&rec->m_data, &row[0x0004], sizeof(rec->m_data));
}
#endif

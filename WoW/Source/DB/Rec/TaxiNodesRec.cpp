#include "WoW/Source/DB/Rec/TaxiNodesRec.h"

// STATIC: 0x00C201D0/0x140A14988
static const int32_t s_fieldOffsets[7] = { 0, 4, 8, 20, 24, 32, 36 };

// STATIC: 0x00C201EC/0x140A149A8
static const int32_t s_fieldSizes[7] = { 4, 4, 12, 4, 8, 4, 8 };

// STATIC: 0x00C20208/0x140A149C8
static const int32_t s_fieldTypes[7] = { 0, 0, 3, 2, 0, 0, 3 };

// STATIC: 0x00C20224/0x140A149E8
const DBCMeta TaxiNodesRec::s_meta = { "DBFilesClient\\TaxiNodes.dbc", 7, 44, TaxiNodesRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// PROVEN: ed0e107b IDENTICAL/DIFFERENT
// OFFSET: 0x00A2EBF0/0x1407D84B0
void TaxiNodesRec::FixupStrings(void* record, const char* strings) {
    TaxiNodesRec* rec = static_cast<TaxiNodesRec*>(record);
    DBCFixupString(rec->m_name, strings);
}

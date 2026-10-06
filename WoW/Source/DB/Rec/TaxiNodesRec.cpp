#include "WoW/Source/DB/Rec/TaxiNodesRec.h"

// STATIC: 0x00C20224/0x140A149E8
const DBCMeta TaxiNodesRec::s_meta = { "DBFilesClient\\TaxiNodes.dbc", 7, 44, TaxiNodesRec::FixupStrings, 0 };

// PROVEN: ed0e107b IDENTICAL/NONE
// OFFSET: 0x00A2EBF0/0x1407D84B0
void TaxiNodesRec::FixupStrings(void* record, const char* strings) {
    TaxiNodesRec* rec = static_cast<TaxiNodesRec*>(record);
    DBCFixupString(rec->m_name, strings);
}

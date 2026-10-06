#include "WoW/Source/DB/Rec/WMOAreaTableRec.h"

// STATIC: 0x00C27A48/0x140A1DFE0
const DBCMeta WMOAreaTableRec::s_meta = { "DBFilesClient\\WMOAreaTable.dbc", 15, 60, WMOAreaTableRec::FixupStrings, 0 };

// PROVEN: 8a82707e IDENTICAL/NONE
// OFFSET: 0x00A2EC40/0x1407D85D0
void WMOAreaTableRec::FixupStrings(void* record, const char* strings) {
    WMOAreaTableRec* rec = static_cast<WMOAreaTableRec*>(record);
    DBCFixupString(rec->m_areaName, strings);
}

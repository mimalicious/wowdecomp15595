#include "WoW/Source/DB/Rec/WMOAreaTableRec.h"

// STATIC: 0x00C27994/0x140A1DF20
static const int32_t s_fieldOffsets[15] = { 0, 4, 8, 12, 16, 20, 24, 28, 32, 36, 40, 44, 48, 52, 56 };

// STATIC: 0x00C279D0/0x140A1DF60
static const int32_t s_fieldSizes[15] = { 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4 };

// STATIC: 0x00C27A0C/0x140A1DFA0
static const int32_t s_fieldTypes[15] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0 };

// STATIC: 0x00C27A48/0x140A1DFE0
const DBCMeta WMOAreaTableRec::s_meta = { "DBFilesClient\\WMOAreaTable.dbc", 15, 60, WMOAreaTableRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// PROVEN: 8a82707e IDENTICAL/DIFFERENT
// OFFSET: 0x00A2EC40/0x1407D85D0
void WMOAreaTableRec::FixupStrings(void* record, const char* strings) {
    WMOAreaTableRec* rec = static_cast<WMOAreaTableRec*>(record);
    DBCFixupString(rec->m_areaName, strings);
}

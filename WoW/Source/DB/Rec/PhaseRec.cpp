#include "WoW/Source/DB/Rec/PhaseRec.h"

// STATIC: 0x00C2300C/0x140A18148
static const int32_t s_fieldOffsets[3] = { 0, 4, 8 };

// STATIC: 0x00C23018/0x140A18158
static const int32_t s_fieldSizes[3] = { 4, 4, 4 };

// STATIC: 0x00C23024/0x140A18168
static const int32_t s_fieldTypes[3] = { 0, 2, 0 };

// STATIC: 0x00C23030/0x140A18178
const DBCMeta PhaseRec::s_meta = { "DBFilesClient\\Phase.dbc", 3, 12, PhaseRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// PROVEN: f375221f IDENTICAL/NONE
// OFFSET: 0x00A2EBA0/0x1407D8330
void PhaseRec::FixupStrings(void* record, const char* strings) {
    PhaseRec* rec = static_cast<PhaseRec*>(record);
    DBCFixupString(rec->m_name, strings);
}

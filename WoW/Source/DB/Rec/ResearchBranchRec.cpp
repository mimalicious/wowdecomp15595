#include "WoW/Source/DB/Rec/ResearchBranchRec.h"

// STATIC: 0x00C22B7C/0x140A17B90
static const int32_t s_fieldOffsets[6] = { 0, 4, 8, 12, 16, 20 };

// STATIC: 0x00C22B94/0x140A17BA8
static const int32_t s_fieldSizes[6] = { 4, 4, 4, 4, 4, 4 };

// STATIC: 0x00C22BAC/0x140A17BC0
static const int32_t s_fieldTypes[6] = { 0, 2, 0, 0, 2, 0 };

// STATIC: 0x00C22BC4/0x140A17BD8
const DBCMeta ResearchBranchRec::s_meta = { "DBFilesClient\\ResearchBranch.dbc", 6, 24, ResearchBranchRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// OFFSET: 0x00A2EA60/0x1407D7ED0
void ResearchBranchRec::FixupStrings(void* record, const char* strings) {
    ResearchBranchRec* rec = static_cast<ResearchBranchRec*>(record);
    DBCFixupString(rec->m_name, strings);
    DBCFixupString(rec->m_texture, strings);
}

#include "WoW/Source/DB/Rec/ResearchProjectRec.h"

// STATIC: 0x00C22A68/0x140A17A30
static const int32_t s_fieldOffsets[9] = { 0, 4, 8, 12, 16, 20, 24, 28, 32 };

// STATIC: 0x00C22A8C/0x140A17A58
static const int32_t s_fieldSizes[9] = { 4, 4, 4, 4, 4, 4, 4, 4, 4 };

// STATIC: 0x00C22AB0/0x140A17A80
static const int32_t s_fieldTypes[9] = { 0, 2, 2, 0, 0, 0, 0, 2, 0 };

// STATIC: 0x00C22AD4/0x140A17AA8
const DBCMeta ResearchProjectRec::s_meta = { "DBFilesClient\\ResearchProject.dbc", 9, 36, ResearchProjectRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// PROVEN: 347df19a IDENTICAL/DIFFERENT
// OFFSET: 0x00A2EA40/0x1407D8050
void ResearchProjectRec::FixupStrings(void* record, const char* strings) {
    ResearchProjectRec* rec = static_cast<ResearchProjectRec*>(record);
    DBCFixupString(rec->m_name, strings);
    DBCFixupString(rec->m_description, strings);
    DBCFixupString(rec->m_texture, strings);
}

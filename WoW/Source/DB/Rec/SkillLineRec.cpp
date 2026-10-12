#include "WoW/Source/DB/Rec/SkillLineRec.h"

// STATIC: 0x00C223E4/0x140A17250
static const int32_t s_fieldOffsets[7] = { 0, 4, 8, 12, 16, 20, 24 };

// STATIC: 0x00C22400/0x140A17270
static const int32_t s_fieldSizes[7] = { 4, 4, 4, 4, 4, 4, 4 };

// STATIC: 0x00C2241C/0x140A17290
static const int32_t s_fieldTypes[7] = { 0, 0, 2, 2, 0, 2, 0 };

// STATIC: 0x00C22438/0x140A172B0
const DBCMeta SkillLineRec::s_meta = { "DBFilesClient\\SkillLine.dbc", 7, 28, SkillLineRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// PROVEN: d12a1191 IDENTICAL/DIFFERENT
// OFFSET: 0x00A2EA20/0x1407D7FF0
void SkillLineRec::FixupStrings(void* record, const char* strings) {
    SkillLineRec* rec = static_cast<SkillLineRec*>(record);
    DBCFixupString(rec->m_displayName, strings);
    DBCFixupString(rec->m_description, strings);
    DBCFixupString(rec->m_alternateVerb, strings);
}

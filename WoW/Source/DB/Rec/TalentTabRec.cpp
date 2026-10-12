#include "WoW/Source/DB/Rec/TalentTabRec.h"

// STATIC: 0x00C202DC/0x140A14AD8
static const int32_t s_fieldOffsets[10] = { 0, 4, 8, 12, 16, 20, 24, 28, 32, 36 };

// STATIC: 0x00C20304/0x140A14B00
static const int32_t s_fieldSizes[10] = { 4, 4, 4, 4, 4, 4, 4, 4, 4, 8 };

// STATIC: 0x00C2032C/0x140A14B28
static const int32_t s_fieldTypes[10] = { 0, 2, 0, 0, 0, 0, 2, 2, 0, 0 };

// STATIC: 0x00C20354/0x140A14B50
const DBCMeta TalentTabRec::s_meta = { "DBFilesClient\\TalentTab.dbc", 10, 44, TalentTabRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// PROVEN: 10a1d5fb IDENTICAL/DIFFERENT
// OFFSET: 0x00A2E970/0x1407D7DC0
void TalentTabRec::FixupStrings(void* record, const char* strings) {
    TalentTabRec* rec = static_cast<TalentTabRec*>(record);
    DBCFixupString(rec->m_name, strings);
    DBCFixupString(rec->m_backgroundFile, strings);
    DBCFixupString(rec->unk_001C, strings);
}

#include "WoW/Source/DB/Rec/LfgDungeonsRec.h"

// STATIC: 0x00C236A8/0x140A189D0
static const int32_t s_fieldOffsets[21] = { 0, 4, 8, 12, 16, 20, 24, 28, 32, 36, 40, 44, 48, 52, 56, 60, 64, 68, 72, 76, 80 };

// STATIC: 0x00C23700/0x140A18A30
static const int32_t s_fieldSizes[21] = { 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4 };

// STATIC: 0x00C23758/0x140A18A90
static const int32_t s_fieldTypes[21] = { 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 2, 0, 0, 0, 0 };

// STATIC: 0x00C237AC/0x140A18AE8
const DBCMeta LfgDungeonsRec::s_meta = { "DBFilesClient\\LfgDungeons.dbc", 21, 84, LfgDungeonsRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// PROVEN: cc2f18a1 IDENTICAL/NONE
// OFFSET: 0x00A2EAD0/0x1407D81D0
void LfgDungeonsRec::FixupStrings(void* record, const char* strings) {
    LfgDungeonsRec* rec = static_cast<LfgDungeonsRec*>(record);
    DBCFixupString(rec->m_name, strings);
    DBCFixupString(rec->m_textureFilename, strings);
    DBCFixupString(rec->m_description, strings);
}

#include "WoW/Source/DB/Rec/CharSectionsRec.h"

// STATIC: 0x00C1E79C/0x140A12A58
static const int32_t s_fieldOffsets[8] = { 0, 4, 8, 12, 16, 28, 32, 36 };

// STATIC: 0x00C1E7BC/0x140A12A78
static const int32_t s_fieldSizes[8] = { 4, 4, 4, 4, 12, 4, 4, 4 };

// STATIC: 0x00C1E7DC/0x140A12A98
static const int32_t s_fieldTypes[8] = { 0, 0, 0, 0, 2, 0, 0, 0 };

// STATIC: 0x00C1E7FC/0x140A12AB8
const DBCMeta CharSectionsRec::s_meta = { "DBFilesClient\\CharSections.dbc", 8, 40, CharSectionsRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// PROVEN: 3e0eb194 IDENTICAL/DIFFERENT
// OFFSET: 0x00A2E8E0/0x1407D7BA0
void CharSectionsRec::FixupStrings(void* record, const char* strings) {
    CharSectionsRec* rec = static_cast<CharSectionsRec*>(record);
    DBCFixupString(rec->m_textureName[0], strings);
    DBCFixupString(rec->m_textureName[1], strings);
    DBCFixupString(rec->m_textureName[2], strings);
}

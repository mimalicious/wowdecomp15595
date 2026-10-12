#include "WoW/Source/DB/Rec/NamesProfanityRec.h"

// STATIC: 0x00C1E030/0x140A12158
static const int32_t s_fieldOffsets[3] = { 0, 4, 8 };

// STATIC: 0x00C1E03C/0x140A12168
static const int32_t s_fieldSizes[3] = { 4, 4, 4 };

// STATIC: 0x00C1E048/0x140A12178
static const int32_t s_fieldTypes[3] = { 0, 2, 0 };

// STATIC: 0x00C1E054/0x140A12188
const DBCMeta NamesProfanityRec::s_meta = { "DBFilesClient\\NamesProfanity.dbc", 3, 12, NamesProfanityRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// OFFSET: 0x00A2EBA0/0x1407D8330
void NamesProfanityRec::FixupStrings(void* record, const char* strings) {
    NamesProfanityRec* rec = static_cast<NamesProfanityRec*>(record);
    DBCFixupString(rec->m_name, strings);
}

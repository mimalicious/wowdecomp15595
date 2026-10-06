#include "WoW/Source/DB/Rec/CreatureTypeRec.h"

// STATIC: 0x00C25F70/0x140A1BD68
static const int32_t s_fieldOffsets[3] = { 0, 4, 8 };

// STATIC: 0x00C25F7C/0x140A1BD78
static const int32_t s_fieldSizes[3] = { 4, 4, 4 };

// STATIC: 0x00C25F88/0x140A1BD88
static const int32_t s_fieldTypes[3] = { 0, 2, 0 };

// STATIC: 0x00C25F94/0x140A1BD98
const DBCMeta CreatureTypeRec::s_meta = { "DBFilesClient\\CreatureType.dbc", 3, 12, CreatureTypeRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// PROVEN: 1a525b3e IDENTICAL/NONE
// OFFSET: 0x00A2EBA0/0x1407D8330
void CreatureTypeRec::FixupStrings(void* record, const char* strings) {
    CreatureTypeRec* rec = static_cast<CreatureTypeRec*>(record);
    DBCFixupString(rec->m_name, strings);
}

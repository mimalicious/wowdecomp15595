#include "WoW/Source/DB/Rec/LiquidTypeRec.h"

// STATIC: 0x00C27D78/0x140A1E3F0
static const int32_t s_fieldOffsets[19] = { 0, 4, 8, 12, 16, 20, 24, 28, 32, 36, 40, 44, 48, 52, 56, 60, 84, 92, 164 };

// STATIC: 0x00C27DC8/0x140A1E440
static const int32_t s_fieldSizes[19] = { 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 24, 8, 72, 16 };

// STATIC: 0x00C27E18/0x140A1E490
static const int32_t s_fieldTypes[19] = { 0, 2, 0, 0, 0, 0, 3, 3, 3, 3, 0, 3, 0, 0, 0, 2, 0, 3, 0 };

// STATIC: 0x00C27E64/0x140A1E4E0
const DBCMeta LiquidTypeRec::s_meta = { "DBFilesClient\\LiquidType.dbc", 19, 180, LiquidTypeRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// OFFSET: 0x00A3B730/0x1407E8560
void LiquidTypeRec::FixupStrings(void* record, const char* strings) {
    LiquidTypeRec* rec = static_cast<LiquidTypeRec*>(record);
    DBCFixupString(rec->m_name, strings);
    DBCFixupString(rec->m_texture[0], strings);
    DBCFixupString(rec->m_texture[1], strings);
    DBCFixupString(rec->m_texture[2], strings);
    DBCFixupString(rec->m_texture[3], strings);
    DBCFixupString(rec->m_texture[4], strings);
    DBCFixupString(rec->m_texture[5], strings);
}

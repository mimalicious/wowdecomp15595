#include "WoW/Source/DB/Rec/LiquidTypeRec.h"

// STATIC: 0x00C27E64/0x140A1E4E0
const DBCMeta LiquidTypeRec::s_meta = { "DBFilesClient\\LiquidType.dbc", 19, 180, LiquidTypeRec::FixupStrings, 0 };

// PROVEN: f49f4a46 IDENTICAL/NONE
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

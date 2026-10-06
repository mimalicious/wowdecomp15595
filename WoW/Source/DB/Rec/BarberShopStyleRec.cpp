#include "WoW/Source/DB/Rec/BarberShopStyleRec.h"

// STATIC: 0x00C268B8/0x140A1C8A0
const DBCMeta BarberShopStyleRec::s_meta = { "DBFilesClient\\BarberShopStyle.dbc", 8, 32, BarberShopStyleRec::FixupStrings, 0 };

// PROVEN: f0c292ef IDENTICAL/NONE
// OFFSET: 0x00A2EC50/0x1407D8590
void BarberShopStyleRec::FixupStrings(void* record, const char* strings) {
    BarberShopStyleRec* rec = static_cast<BarberShopStyleRec*>(record);
    DBCFixupString(rec->m_displayName, strings);
    DBCFixupString(rec->m_description, strings);
}

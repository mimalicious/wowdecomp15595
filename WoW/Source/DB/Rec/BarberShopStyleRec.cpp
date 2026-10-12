#include "WoW/Source/DB/Rec/BarberShopStyleRec.h"

// STATIC: 0x00C26858/0x140A1C840
static const int32_t s_fieldOffsets[8] = { 0, 4, 8, 12, 16, 20, 24, 28 };

// STATIC: 0x00C26878/0x140A1C860
static const int32_t s_fieldSizes[8] = { 4, 4, 4, 4, 4, 4, 4, 4 };

// STATIC: 0x00C26898/0x140A1C880
static const int32_t s_fieldTypes[8] = { 0, 0, 2, 2, 3, 0, 0, 0 };

// STATIC: 0x00C268B8/0x140A1C8A0
const DBCMeta BarberShopStyleRec::s_meta = { "DBFilesClient\\BarberShopStyle.dbc", 8, 32, BarberShopStyleRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// PROVEN: f0c292ef IDENTICAL/DIFFERENT
// OFFSET: 0x00A2EC50/0x1407D8590
void BarberShopStyleRec::FixupStrings(void* record, const char* strings) {
    BarberShopStyleRec* rec = static_cast<BarberShopStyleRec*>(record);
    DBCFixupString(rec->m_displayName, strings);
    DBCFixupString(rec->m_description, strings);
}

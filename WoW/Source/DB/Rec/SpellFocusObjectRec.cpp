#include "WoW/Source/DB/Rec/SpellFocusObjectRec.h"

// STATIC: 0x00C21334/0x140A15E68
static const int32_t s_fieldOffsets[2] = { 0, 4 };

// STATIC: 0x00C2133C/0x140A15E70
static const int32_t s_fieldSizes[2] = { 4, 4 };

// STATIC: 0x00C21344/0x140A15E78
static const int32_t s_fieldTypes[2] = { 0, 2 };

// STATIC: 0x00C2134C/0x140A15E80
const DBCMeta SpellFocusObjectRec::s_meta = { "DBFilesClient\\SpellFocusObject.dbc", 2, 8, SpellFocusObjectRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// PROVEN: c4456c63 IDENTICAL/DIFFERENT
// OFFSET: 0x00A2EBA0/0x1407D8330
void SpellFocusObjectRec::FixupStrings(void* record, const char* strings) {
    SpellFocusObjectRec* rec = static_cast<SpellFocusObjectRec*>(record);
    DBCFixupString(rec->m_name, strings);
}

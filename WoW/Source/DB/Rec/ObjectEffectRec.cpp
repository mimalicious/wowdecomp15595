#include "WoW/Source/DB/Rec/ObjectEffectRec.h"

// STATIC: 0x00C1CFB4/0x140A10DC0
static const int32_t s_fieldOffsets[10] = { 0, 4, 8, 12, 16, 20, 24, 28, 32, 44 };

// STATIC: 0x00C1CFDC/0x140A10DE8
static const int32_t s_fieldSizes[10] = { 4, 4, 4, 4, 4, 4, 4, 4, 12, 4 };

// STATIC: 0x00C1D004/0x140A10E10
static const int32_t s_fieldTypes[10] = { 0, 2, 0, 0, 0, 0, 0, 0, 3, 0 };

// STATIC: 0x00C1D02C/0x140A10E38
const DBCMeta ObjectEffectRec::s_meta = { "DBFilesClient\\ObjectEffect.dbc", 10, 48, ObjectEffectRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// OFFSET: 0x00A2EBA0/0x1407D8330
void ObjectEffectRec::FixupStrings(void* record, const char* strings) {
    ObjectEffectRec* rec = static_cast<ObjectEffectRec*>(record);
    DBCFixupString(rec->m_name, strings);
}

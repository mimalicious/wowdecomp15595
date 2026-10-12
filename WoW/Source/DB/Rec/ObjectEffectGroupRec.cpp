#include "WoW/Source/DB/Rec/ObjectEffectGroupRec.h"

// STATIC: 0x00C1CF5C/0x140A10D50
static const int32_t s_fieldOffsets[2] = { 0, 4 };

// STATIC: 0x00C1CF64/0x140A10D58
static const int32_t s_fieldSizes[2] = { 4, 4 };

// STATIC: 0x00C1CF6C/0x140A10D60
static const int32_t s_fieldTypes[2] = { 0, 2 };

// STATIC: 0x00C1CF74/0x140A10D68
const DBCMeta ObjectEffectGroupRec::s_meta = { "DBFilesClient\\ObjectEffectGroup.dbc", 2, 8, ObjectEffectGroupRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// OFFSET: 0x00A2EBA0/0x1407D8330
void ObjectEffectGroupRec::FixupStrings(void* record, const char* strings) {
    ObjectEffectGroupRec* rec = static_cast<ObjectEffectGroupRec*>(record);
    DBCFixupString(rec->m_name, strings);
}

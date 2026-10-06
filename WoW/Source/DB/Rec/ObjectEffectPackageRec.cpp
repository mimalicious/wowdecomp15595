#include "WoW/Source/DB/Rec/ObjectEffectPackageRec.h"

// STATIC: 0x00C1CE7C/0x140A10C30
static const int32_t s_fieldOffsets[2] = { 0, 4 };

// STATIC: 0x00C1CE84/0x140A10C38
static const int32_t s_fieldSizes[2] = { 4, 4 };

// STATIC: 0x00C1CE8C/0x140A10C40
static const int32_t s_fieldTypes[2] = { 0, 2 };

// STATIC: 0x00C1CE94/0x140A10C48
const DBCMeta ObjectEffectPackageRec::s_meta = { "DBFilesClient\\ObjectEffectPackage.dbc", 2, 8, ObjectEffectPackageRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// PROVEN: ab6b77bd IDENTICAL/NONE
// OFFSET: 0x00A2EBA0/0x1407D8330
void ObjectEffectPackageRec::FixupStrings(void* record, const char* strings) {
    ObjectEffectPackageRec* rec = static_cast<ObjectEffectPackageRec*>(record);
    DBCFixupString(rec->m_name, strings);
}

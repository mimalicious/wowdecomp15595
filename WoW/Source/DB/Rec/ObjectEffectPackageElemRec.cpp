#include "WoW/Source/DB/Rec/ObjectEffectPackageElemRec.h"

// STATIC: 0x00C1CE04/0x140A10BA0
static const int32_t s_fieldOffsets[4] = { 0, 4, 8, 12 };

// STATIC: 0x00C1CE14/0x140A10BB0
static const int32_t s_fieldSizes[4] = { 4, 4, 4, 4 };

// STATIC: 0x00C1CE24/0x140A10BC0
static const int32_t s_fieldTypes[4] = { 0, 0, 0, 0 };

// STATIC: 0x00C1CE34/0x140A10BD0
const DBCMeta ObjectEffectPackageElemRec::s_meta = { "DBFilesClient\\ObjectEffectPackageElem.dbc", 4, 16, ObjectEffectPackageElemRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// OFFSET: 0x00401000/0x1407D7E80
void ObjectEffectPackageElemRec::FixupStrings(void* record, const char* strings) {
}

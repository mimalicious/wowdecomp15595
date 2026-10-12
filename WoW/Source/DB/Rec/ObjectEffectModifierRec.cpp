#include "WoW/Source/DB/Rec/ObjectEffectModifierRec.h"

// STATIC: 0x00C1CEDC/0x140A10CA8
static const int32_t s_fieldOffsets[5] = { 0, 4, 8, 12, 16 };

// STATIC: 0x00C1CEF0/0x140A10CC0
static const int32_t s_fieldSizes[5] = { 4, 4, 4, 4, 16 };

// STATIC: 0x00C1CF04/0x140A10CD8
static const int32_t s_fieldTypes[5] = { 0, 0, 0, 0, 3 };

// STATIC: 0x00C1CF18/0x140A10CF0
const DBCMeta ObjectEffectModifierRec::s_meta = { "DBFilesClient\\ObjectEffectModifier.dbc", 5, 32, ObjectEffectModifierRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// OFFSET: 0x00401000/0x1407D7E80
void ObjectEffectModifierRec::FixupStrings(void* record, const char* strings) {
}

#include "WoW/Source/DB/Rec/AnimKitBoneSetAliasRec.h"

// STATIC: 0x00C1EC5C/0x140A13030
static const int32_t s_fieldOffsets[3] = { 0, 4, 8 };

// STATIC: 0x00C1EC68/0x140A13040
static const int32_t s_fieldSizes[3] = { 4, 4, 4 };

// STATIC: 0x00C1EC74/0x140A13050
static const int32_t s_fieldTypes[3] = { 0, 0, 0 };

// STATIC: 0x00C1EC80/0x140A13060
const DBCMeta AnimKitBoneSetAliasRec::s_meta = { "DBFilesClient\\AnimKitBoneSetAlias.dbc", 3, 12, AnimKitBoneSetAliasRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// PROVEN: eaa447dc IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void AnimKitBoneSetAliasRec::FixupStrings(void* record, const char* strings) {
}

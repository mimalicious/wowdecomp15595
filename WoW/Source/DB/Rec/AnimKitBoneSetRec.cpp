#include "WoW/Source/DB/Rec/AnimKitBoneSetRec.h"

// STATIC: 0x00C1EBCC/0x140A12F88
static const int32_t s_fieldOffsets[6] = { 0, 4, 8, 12, 16, 20 };

// STATIC: 0x00C1EBE4/0x140A12FA0
static const int32_t s_fieldSizes[6] = { 4, 4, 4, 4, 4, 4 };

// STATIC: 0x00C1EBFC/0x140A12FB8
static const int32_t s_fieldTypes[6] = { 0, 2, 0, 0, 0, 0 };

// STATIC: 0x00C1EC14/0x140A12FD0
const DBCMeta AnimKitBoneSetRec::s_meta = { "DBFilesClient\\AnimKitBoneSet.dbc", 6, 24, AnimKitBoneSetRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// OFFSET: 0x00A2EBA0/0x1407D8330
void AnimKitBoneSetRec::FixupStrings(void* record, const char* strings) {
    AnimKitBoneSetRec* rec = static_cast<AnimKitBoneSetRec*>(record);
    DBCFixupString(rec->m_name, strings);
}

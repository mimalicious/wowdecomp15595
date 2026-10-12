#include "WoW/Source/DB/Rec/AnimationDataRec.h"

// STATIC: 0x00C285F4/0x140A1EDF8
static const int32_t s_fieldOffsets[6] = { 0, 4, 8, 12, 16, 20 };

// STATIC: 0x00C2860C/0x140A1EE10
static const int32_t s_fieldSizes[6] = { 4, 4, 4, 4, 4, 4 };

// STATIC: 0x00C28624/0x140A1EE28
static const int32_t s_fieldTypes[6] = { 0, 2, 0, 0, 0, 0 };

// STATIC: 0x00C2863C/0x140A1EE40
const DBCMeta AnimationDataRec::s_meta = { "DBFilesClient\\AnimationData.dbc", 6, 24, AnimationDataRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// OFFSET: 0x00A2EBA0/0x1407D8330
void AnimationDataRec::FixupStrings(void* record, const char* strings) {
    AnimationDataRec* rec = static_cast<AnimationDataRec*>(record);
    DBCFixupString(rec->m_name, strings);
}

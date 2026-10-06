#include "WoW/Source/DB/Rec/AnimationDataRec.h"

// STATIC: 0x00C2863C/0x140A1EE40
const DBCMeta AnimationDataRec::s_meta = { "DBFilesClient\\AnimationData.dbc", 6, 24, AnimationDataRec::FixupStrings, 0 };

// PROVEN: 666c8bf4 IDENTICAL/NONE
// OFFSET: 0x00A2EBA0/0x1407D8330
void AnimationDataRec::FixupStrings(void* record, const char* strings) {
    AnimationDataRec* rec = static_cast<AnimationDataRec*>(record);
    DBCFixupString(rec->m_name, strings);
}

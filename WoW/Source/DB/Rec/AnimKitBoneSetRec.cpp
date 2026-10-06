#include "WoW/Source/DB/Rec/AnimKitBoneSetRec.h"

// STATIC: 0x00C1EC14/0x140A12FD0
const DBCMeta AnimKitBoneSetRec::s_meta = { "DBFilesClient\\AnimKitBoneSet.dbc", 6, 24, AnimKitBoneSetRec::FixupStrings, 0 };

// PROVEN: 94ea4be5 IDENTICAL/NONE
// OFFSET: 0x00A2EBA0/0x1407D8330
void AnimKitBoneSetRec::FixupStrings(void* record, const char* strings) {
    AnimKitBoneSetRec* rec = static_cast<AnimKitBoneSetRec*>(record);
    DBCFixupString(rec->unk_0004, strings);
}

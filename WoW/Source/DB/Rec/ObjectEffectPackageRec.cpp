#include "WoW/Source/DB/Rec/ObjectEffectPackageRec.h"

// STATIC: 0x00C1CE94/0x140A10C48
const DBCMeta ObjectEffectPackageRec::s_meta = { "DBFilesClient\\ObjectEffectPackage.dbc", 2, 8, ObjectEffectPackageRec::FixupStrings, 0 };

// PROVEN: ab6b77bd IDENTICAL/NONE
// OFFSET: 0x00A2EBA0/0x1407D8330
void ObjectEffectPackageRec::FixupStrings(void* record, const char* strings) {
    ObjectEffectPackageRec* rec = static_cast<ObjectEffectPackageRec*>(record);
    DBCFixupString(rec->m_name, strings);
}

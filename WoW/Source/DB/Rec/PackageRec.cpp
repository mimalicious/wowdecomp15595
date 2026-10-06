#include "WoW/Source/DB/Rec/PackageRec.h"

// STATIC: 0x00C23164/0x140A18308
const DBCMeta PackageRec::s_meta = { "DBFilesClient\\Package.dbc", 4, 16, PackageRec::FixupStrings, 0 };

// PROVEN: a6caebf3 IDENTICAL/NONE
// OFFSET: 0x00A3B710/0x1407E8520
void PackageRec::FixupStrings(void* record, const char* strings) {
    PackageRec* rec = static_cast<PackageRec*>(record);
    DBCFixupString(rec->m_icon, strings);
    DBCFixupString(rec->m_name, strings);
}

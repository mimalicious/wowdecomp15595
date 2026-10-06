#include "WoW/Source/DB/Rec/PackageRec.h"

// STATIC: 0x00C23134/0x140A182D8
static const int32_t s_fieldOffsets[4] = { 0, 4, 8, 12 };

// STATIC: 0x00C23144/0x140A182E8
static const int32_t s_fieldSizes[4] = { 4, 4, 4, 4 };

// STATIC: 0x00C23154/0x140A182F8
static const int32_t s_fieldTypes[4] = { 0, 2, 0, 2 };

// STATIC: 0x00C23164/0x140A18308
const DBCMeta PackageRec::s_meta = { "DBFilesClient\\Package.dbc", 4, 16, PackageRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// PROVEN: a6caebf3 IDENTICAL/NONE
// OFFSET: 0x00A3B710/0x1407E8520
void PackageRec::FixupStrings(void* record, const char* strings) {
    PackageRec* rec = static_cast<PackageRec*>(record);
    DBCFixupString(rec->m_icon, strings);
    DBCFixupString(rec->m_name, strings);
}

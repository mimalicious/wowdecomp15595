#include "WoW/Source/DB/Rec/ItemArmorQualityRec.h"

// STATIC: 0x00C247E0/0x140A19FA8
static const int32_t s_fieldOffsets[3] = { 0, 4, 32 };

// STATIC: 0x00C247EC/0x140A19FB8
static const int32_t s_fieldSizes[3] = { 4, 28, 4 };

// STATIC: 0x00C247F8/0x140A19FC8
static const int32_t s_fieldTypes[3] = { 0, 3, 0 };

// STATIC: 0x00C24804/0x140A19FD8
const DBCMeta ItemArmorQualityRec::s_meta = { "DBFilesClient\\ItemArmorQuality.dbc", 3, 36, ItemArmorQualityRec::FixupStrings, 0, s_fieldOffsets, s_fieldSizes, s_fieldTypes };

// PROVEN: 87d92419 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void ItemArmorQualityRec::FixupStrings(void* record, const char* strings) {
}

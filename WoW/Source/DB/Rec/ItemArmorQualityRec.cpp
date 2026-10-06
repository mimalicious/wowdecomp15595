#include "WoW/Source/DB/Rec/ItemArmorQualityRec.h"

// STATIC: 0x00C24804/0x140A19FD8
const DBCMeta ItemArmorQualityRec::s_meta = { "DBFilesClient\\ItemArmorQuality.dbc", 3, 36, ItemArmorQualityRec::FixupStrings, 0 };

// PROVEN: 87d92419 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void ItemArmorQualityRec::FixupStrings(void* record, const char* strings) {
}

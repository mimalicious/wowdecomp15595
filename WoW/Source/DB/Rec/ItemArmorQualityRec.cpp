#include "WoW/Source/DB/Rec/ItemArmorQualityRec.h"

// STATIC: %0x00B463E4
const DBCMeta ItemArmorQualityRec::s_meta = { "DBFilesClient\\ItemArmorQuality.dbc", 3, 36, ItemArmorQualityRec::FixupStrings, 0 };

// PROVEN: 87d92419 IDENTICAL
// OFFSET: 0x00401000
void ItemArmorQualityRec::FixupStrings(void* record, const char* strings) {
}

#if defined(WOWDECOMP_ARCH_64)
void ItemArmorQualityRec::ConvertRow(ItemArmorQualityRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    memcpy(&rec->unk_0004, &row[0x0004], sizeof(rec->unk_0004));
    memcpy(&rec->unk_0008, &row[0x0008], sizeof(rec->unk_0008));
    memcpy(&rec->unk_000C, &row[0x000C], sizeof(rec->unk_000C));
    memcpy(&rec->unk_0010, &row[0x0010], sizeof(rec->unk_0010));
    memcpy(&rec->unk_0014, &row[0x0014], sizeof(rec->unk_0014));
    memcpy(&rec->unk_0018, &row[0x0018], sizeof(rec->unk_0018));
    memcpy(&rec->unk_001C, &row[0x001C], sizeof(rec->unk_001C));
    memcpy(&rec->unk_0020, &row[0x0020], sizeof(rec->unk_0020));
}
#endif

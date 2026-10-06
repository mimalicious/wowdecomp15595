#include "WoW/Source/DB/Rec/SpellVisualEffectNameRec.h"

// STATIC: %0x00B4355C
const DBCMeta SpellVisualEffectNameRec::s_meta = { "DBFilesClient\\SpellVisualEffectName.dbc", 7, 28, SpellVisualEffectNameRec::FixupStrings, 0 };

// OFFSET: %0x0097C1F0
void SpellVisualEffectNameRec::FixupStrings(void* record, const char* strings) {
    SpellVisualEffectNameRec* rec = static_cast<SpellVisualEffectNameRec*>(record);
    rec->m_name = &strings[reinterpret_cast<uintptr_t>(rec->m_name)];
    rec->m_fileName = &strings[reinterpret_cast<uintptr_t>(rec->m_fileName)];
}

#if defined(WOWDECOMP_ARCH_64)
void SpellVisualEffectNameRec::ConvertRow(SpellVisualEffectNameRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    rec->m_name = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0004])];
    rec->m_fileName = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0008])];
    memcpy(&rec->m_areaEffectSize, &row[0x000C], sizeof(rec->m_areaEffectSize));
    memcpy(&rec->m_scale, &row[0x0010], sizeof(rec->m_scale));
    memcpy(&rec->m_minAllowedScale, &row[0x0014], sizeof(rec->m_minAllowedScale));
    memcpy(&rec->m_maxAllowedScale, &row[0x0018], sizeof(rec->m_maxAllowedScale));
}
#endif

#include "WoW/Source/DB/Rec/SpellShapeshiftFormRec.h"

// STATIC: %0x00B44C88
const DBCMeta SpellShapeshiftFormRec::s_meta = { "DBFilesClient\\SpellShapeshiftForm.dbc", 11, 84, SpellShapeshiftFormRec::FixupStrings, 0 };

// OFFSET: %0x0097C380
void SpellShapeshiftFormRec::FixupStrings(void* record, const char* strings) {
    SpellShapeshiftFormRec* rec = static_cast<SpellShapeshiftFormRec*>(record);
    rec->m_name = &strings[reinterpret_cast<uintptr_t>(rec->m_name)];
}

#if defined(WOWDECOMP_ARCH_64)
void SpellShapeshiftFormRec::ConvertRow(SpellShapeshiftFormRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    memcpy(&rec->m_bonusActionBar, &row[0x0004], sizeof(rec->m_bonusActionBar));
    rec->m_name = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0008])];
    memcpy(&rec->m_flags, &row[0x000C], sizeof(rec->m_flags));
    memcpy(&rec->m_creatureType, &row[0x0010], sizeof(rec->m_creatureType));
    memcpy(&rec->m_attackIconID, &row[0x0014], sizeof(rec->m_attackIconID));
    memcpy(&rec->m_combatRoundTime, &row[0x0018], sizeof(rec->m_combatRoundTime));
    memcpy(rec->m_creatureDisplayID, &row[0x001C], sizeof(rec->m_creatureDisplayID));
    memcpy(rec->m_presetSpellID, &row[0x002C], sizeof(rec->m_presetSpellID));
    memcpy(&rec->unk_004C, &row[0x004C], sizeof(rec->unk_004C));
    memcpy(&rec->unk_0050, &row[0x0050], sizeof(rec->unk_0050));
}
#endif

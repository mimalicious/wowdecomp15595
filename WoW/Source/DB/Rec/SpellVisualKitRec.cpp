#include "WoW/Source/DB/Rec/SpellVisualKitRec.h"

// STATIC: %0x00B43520
const DBCMeta SpellVisualKitRec::s_meta = { "DBFilesClient\\SpellVisualKit.dbc", 22, 156, SpellVisualKitRec::FixupStrings, 0 };

// OFFSET: %0x00401000
void SpellVisualKitRec::FixupStrings(void* record, const char* strings) {
}

#if defined(WOWDECOMP_ARCH_64)
void SpellVisualKitRec::ConvertRow(SpellVisualKitRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    memcpy(&rec->m_startAnimID, &row[0x0004], sizeof(rec->m_startAnimID));
    memcpy(&rec->m_animID, &row[0x0008], sizeof(rec->m_animID));
    memcpy(&rec->unk_000C, &row[0x000C], sizeof(rec->unk_000C));
    memcpy(&rec->m_headEffect, &row[0x0010], sizeof(rec->m_headEffect));
    memcpy(&rec->m_chestEffect, &row[0x0014], sizeof(rec->m_chestEffect));
    memcpy(&rec->m_baseEffect, &row[0x0018], sizeof(rec->m_baseEffect));
    memcpy(&rec->m_leftHandEffect, &row[0x001C], sizeof(rec->m_leftHandEffect));
    memcpy(&rec->m_rightHandEffect, &row[0x0020], sizeof(rec->m_rightHandEffect));
    memcpy(&rec->m_breathEffect, &row[0x0024], sizeof(rec->m_breathEffect));
    memcpy(&rec->m_leftWeaponEffect, &row[0x0028], sizeof(rec->m_leftWeaponEffect));
    memcpy(&rec->m_rightWeaponEffect, &row[0x002C], sizeof(rec->m_rightWeaponEffect));
    memcpy(rec->m_specialEffect, &row[0x0030], sizeof(rec->m_specialEffect));
    memcpy(&rec->m_worldEffect, &row[0x003C], sizeof(rec->m_worldEffect));
    memcpy(&rec->m_soundID, &row[0x0040], sizeof(rec->m_soundID));
    memcpy(&rec->m_shakeID, &row[0x0044], sizeof(rec->m_shakeID));
    memcpy(rec->m_charProc, &row[0x0048], sizeof(rec->m_charProc));
    memcpy(rec->m_charParamZero, &row[0x0058], sizeof(rec->m_charParamZero));
    memcpy(rec->m_charParamOne, &row[0x0068], sizeof(rec->m_charParamOne));
    memcpy(rec->m_charParamTwo, &row[0x0078], sizeof(rec->m_charParamTwo));
    memcpy(rec->m_charParamThree, &row[0x0088], sizeof(rec->m_charParamThree));
    memcpy(&rec->m_flags, &row[0x0098], sizeof(rec->m_flags));
}
#endif

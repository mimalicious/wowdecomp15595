#include "WoW/Source/DB/Rec/SpellVisualKitModelAttachRec.h"

// STATIC: %0x00B44B64
const DBCMeta SpellVisualKitModelAttachRec::s_meta = { "DBFilesClient\\SpellVisualKitModelAttach.dbc", 12, 56, SpellVisualKitModelAttachRec::FixupStrings, 0 };

// OFFSET: %0x00401000
void SpellVisualKitModelAttachRec::FixupStrings(void* record, const char* strings) {
}

#if defined(WOWDECOMP_ARCH_64)
void SpellVisualKitModelAttachRec::ConvertRow(SpellVisualKitModelAttachRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    memcpy(&rec->m_parentSpellVisualKitID, &row[0x0004], sizeof(rec->m_parentSpellVisualKitID));
    memcpy(&rec->m_spellVisualEffectNameID, &row[0x0008], sizeof(rec->m_spellVisualEffectNameID));
    memcpy(&rec->m_attachmentID, &row[0x000C], sizeof(rec->m_attachmentID));
    memcpy(rec->m_offset, &row[0x0010], sizeof(rec->m_offset));
    memcpy(&rec->m_yaw, &row[0x001C], sizeof(rec->m_yaw));
    memcpy(&rec->m_pitch, &row[0x0020], sizeof(rec->m_pitch));
    memcpy(&rec->m_roll, &row[0x0024], sizeof(rec->m_roll));
    memcpy(&rec->unk_0028, &row[0x0028], sizeof(rec->unk_0028));
    memcpy(&rec->unk_002C, &row[0x002C], sizeof(rec->unk_002C));
    memcpy(&rec->unk_0030, &row[0x0030], sizeof(rec->unk_0030));
    memcpy(&rec->unk_0034, &row[0x0034], sizeof(rec->unk_0034));
}
#endif

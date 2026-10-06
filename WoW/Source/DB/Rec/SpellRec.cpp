#include "WoW/Source/DB/Rec/SpellRec.h"

// STATIC: %0x00B451E8
const DBCMeta SpellRec::s_meta = { "DBFilesClient\\Spell.dbc", 46, 188, SpellRec::FixupStrings, 0 };

// OFFSET: %0x0097C240
void SpellRec::FixupStrings(void* record, const char* strings) {
    SpellRec* rec = static_cast<SpellRec*>(record);
    rec->m_name = &strings[reinterpret_cast<uintptr_t>(rec->m_name)];
    rec->m_nameSubtext = &strings[reinterpret_cast<uintptr_t>(rec->m_nameSubtext)];
    rec->m_description = &strings[reinterpret_cast<uintptr_t>(rec->m_description)];
    rec->m_auraDescription = &strings[reinterpret_cast<uintptr_t>(rec->m_auraDescription)];
}

#if defined(WOWDECOMP_ARCH_64)
void SpellRec::ConvertRow(SpellRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    memcpy(&rec->unk_0004, &row[0x0004], sizeof(rec->unk_0004));
    memcpy(&rec->unk_0008, &row[0x0008], sizeof(rec->unk_0008));
    memcpy(&rec->unk_000C, &row[0x000C], sizeof(rec->unk_000C));
    memcpy(&rec->unk_0010, &row[0x0010], sizeof(rec->unk_0010));
    memcpy(&rec->unk_0014, &row[0x0014], sizeof(rec->unk_0014));
    memcpy(&rec->unk_0018, &row[0x0018], sizeof(rec->unk_0018));
    memcpy(&rec->unk_001C, &row[0x001C], sizeof(rec->unk_001C));
    memcpy(&rec->unk_0020, &row[0x0020], sizeof(rec->unk_0020));
    memcpy(&rec->unk_0024, &row[0x0024], sizeof(rec->unk_0024));
    memcpy(&rec->unk_0028, &row[0x0028], sizeof(rec->unk_0028));
    memcpy(&rec->unk_002C, &row[0x002C], sizeof(rec->unk_002C));
    memcpy(&rec->unk_0030, &row[0x0030], sizeof(rec->unk_0030));
    memcpy(&rec->unk_0034, &row[0x0034], sizeof(rec->unk_0034));
    memcpy(&rec->unk_0038, &row[0x0038], sizeof(rec->unk_0038));
    memcpy(&rec->unk_003C, &row[0x003C], sizeof(rec->unk_003C));
    memcpy(&rec->unk_0040, &row[0x0040], sizeof(rec->unk_0040));
    memcpy(&rec->unk_0044, &row[0x0044], sizeof(rec->unk_0044));
    memcpy(&rec->unk_0048, &row[0x0048], sizeof(rec->unk_0048));
    memcpy(&rec->unk_004C, &row[0x004C], sizeof(rec->unk_004C));
    rec->m_name = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0050])];
    rec->m_nameSubtext = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0054])];
    rec->m_description = &strings[*reinterpret_cast<const uint32_t*>(&row[0x0058])];
    rec->m_auraDescription = &strings[*reinterpret_cast<const uint32_t*>(&row[0x005C])];
    memcpy(&rec->m_schoolMask, &row[0x0060], sizeof(rec->m_schoolMask));
    memcpy(&rec->m_runeCostID, &row[0x0064], sizeof(rec->m_runeCostID));
    memcpy(&rec->m_spellMissileID, &row[0x0068], sizeof(rec->m_spellMissileID));
    memcpy(&rec->unk_006C, &row[0x006C], sizeof(rec->unk_006C));
    memcpy(&rec->m_difficulty, &row[0x0070], sizeof(rec->m_difficulty));
    memcpy(&rec->unk_0074, &row[0x0074], sizeof(rec->unk_0074));
    memcpy(&rec->unk_0078, &row[0x0078], sizeof(rec->unk_0078));
    memcpy(&rec->unk_007C, &row[0x007C], sizeof(rec->unk_007C));
    memcpy(&rec->unk_0080, &row[0x0080], sizeof(rec->unk_0080));
    memcpy(&rec->unk_0084, &row[0x0084], sizeof(rec->unk_0084));
    memcpy(&rec->unk_0088, &row[0x0088], sizeof(rec->unk_0088));
    memcpy(&rec->unk_008C, &row[0x008C], sizeof(rec->unk_008C));
    memcpy(&rec->unk_0090, &row[0x0090], sizeof(rec->unk_0090));
    memcpy(&rec->unk_0094, &row[0x0094], sizeof(rec->unk_0094));
    memcpy(&rec->unk_0098, &row[0x0098], sizeof(rec->unk_0098));
    memcpy(&rec->unk_009C, &row[0x009C], sizeof(rec->unk_009C));
    memcpy(&rec->unk_00A0, &row[0x00A0], sizeof(rec->unk_00A0));
    memcpy(&rec->unk_00A4, &row[0x00A4], sizeof(rec->unk_00A4));
    memcpy(&rec->unk_00A8, &row[0x00A8], sizeof(rec->unk_00A8));
    memcpy(&rec->unk_00AC, &row[0x00AC], sizeof(rec->unk_00AC));
    memcpy(&rec->unk_00B0, &row[0x00B0], sizeof(rec->unk_00B0));
    memcpy(&rec->unk_00B4, &row[0x00B4], sizeof(rec->unk_00B4));
    memcpy(&rec->unk_00B8, &row[0x00B8], sizeof(rec->unk_00B8));
}
#endif

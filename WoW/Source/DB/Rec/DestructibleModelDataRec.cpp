#include "WoW/Source/DB/Rec/DestructibleModelDataRec.h"

// STATIC: %0x00B46D58
const DBCMeta DestructibleModelDataRec::s_meta = { "DBFilesClient\\DestructibleModelData.dbc", 24, 96, DestructibleModelDataRec::FixupStrings, 0 };

// PROVEN: c6859269 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void DestructibleModelDataRec::FixupStrings(void* record, const char* strings) {
}

#if defined(WOWDECOMP_ARCH_64)
void DestructibleModelDataRec::ConvertRow(DestructibleModelDataRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    memcpy(&rec->unk_0004, &row[0x0004], sizeof(rec->unk_0004));
    memcpy(&rec->m_state0ImpactEffectDoodadSet, &row[0x0008], sizeof(rec->m_state0ImpactEffectDoodadSet));
    memcpy(&rec->m_state2DestructionDoodadSet, &row[0x000C], sizeof(rec->m_state2DestructionDoodadSet));
    memcpy(&rec->unk_0010, &row[0x0010], sizeof(rec->unk_0010));
    memcpy(&rec->m_state1WMO, &row[0x0014], sizeof(rec->m_state1WMO));
    memcpy(&rec->m_state1DestructionDoodadSet, &row[0x0018], sizeof(rec->m_state1DestructionDoodadSet));
    memcpy(&rec->m_state1ImpactEffectDoodadSet, &row[0x001C], sizeof(rec->m_state1ImpactEffectDoodadSet));
    memcpy(&rec->m_state1AmbientDoodadSet, &row[0x0020], sizeof(rec->m_state1AmbientDoodadSet));
    memcpy(&rec->unk_0024, &row[0x0024], sizeof(rec->unk_0024));
    memcpy(&rec->m_state2WMO, &row[0x0028], sizeof(rec->m_state2WMO));
    memcpy(&rec->unk_002C, &row[0x002C], sizeof(rec->unk_002C));
    memcpy(&rec->unk_0030, &row[0x0030], sizeof(rec->unk_0030));
    memcpy(&rec->unk_0034, &row[0x0034], sizeof(rec->unk_0034));
    memcpy(&rec->unk_0038, &row[0x0038], sizeof(rec->unk_0038));
    memcpy(&rec->unk_003C, &row[0x003C], sizeof(rec->unk_003C));
    memcpy(&rec->unk_0040, &row[0x0040], sizeof(rec->unk_0040));
    memcpy(&rec->m_state3AmbientDoodadSet, &row[0x0044], sizeof(rec->m_state3AmbientDoodadSet));
    memcpy(&rec->unk_0048, &row[0x0048], sizeof(rec->unk_0048));
    memcpy(&rec->m_ejectDirection, &row[0x004C], sizeof(rec->m_ejectDirection));
    memcpy(&rec->m_repairGroundFx, &row[0x0050], sizeof(rec->m_repairGroundFx));
    memcpy(&rec->m_doNotHighlight, &row[0x0054], sizeof(rec->m_doNotHighlight));
    memcpy(&rec->m_healEffect, &row[0x0058], sizeof(rec->m_healEffect));
    memcpy(&rec->m_healEffectSpeed, &row[0x005C], sizeof(rec->m_healEffectSpeed));
}
#endif

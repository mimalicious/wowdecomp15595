#include "WoW/Source/DB/Rec/ScalingStatDistributionRec.h"

// STATIC: %0x00B45700
const DBCMeta ScalingStatDistributionRec::s_meta = { "DBFilesClient\\ScalingStatDistribution.dbc", 5, 92, ScalingStatDistributionRec::FixupStrings, 0 };

// OFFSET: %0x00401000
void ScalingStatDistributionRec::FixupStrings(void* record, const char* strings) {
}

#if defined(WOWDECOMP_ARCH_64)
void ScalingStatDistributionRec::ConvertRow(ScalingStatDistributionRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    memcpy(rec->m_statID, &row[0x0004], sizeof(rec->m_statID));
    memcpy(rec->m_bonus, &row[0x002C], sizeof(rec->m_bonus));
    memcpy(&rec->unk_0040, &row[0x0040], sizeof(rec->unk_0040));
    memcpy(&rec->unk_0044, &row[0x0044], sizeof(rec->unk_0044));
    memcpy(&rec->unk_0048, &row[0x0048], sizeof(rec->unk_0048));
    memcpy(&rec->unk_004C, &row[0x004C], sizeof(rec->unk_004C));
    memcpy(&rec->unk_0050, &row[0x0050], sizeof(rec->unk_0050));
    memcpy(&rec->unk_0054, &row[0x0054], sizeof(rec->unk_0054));
    memcpy(&rec->m_maxlevel, &row[0x0058], sizeof(rec->m_maxlevel));
}
#endif

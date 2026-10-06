#include "WoW/Source/DB/Rec/SpellVisualRec.h"

// STATIC: %0x00B44BD8
const DBCMeta SpellVisualRec::s_meta = { "DBFilesClient\\SpellVisual.dbc", 28, 128, SpellVisualRec::FixupStrings, 0 };

// PROVEN: 9742ae4a IDENTICAL
// OFFSET: 0x00401000
void SpellVisualRec::FixupStrings(void* record, const char* strings) {
}

#if defined(WOWDECOMP_ARCH_64)
void SpellVisualRec::ConvertRow(SpellVisualRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    memcpy(&rec->m_precastKit, &row[0x0004], sizeof(rec->m_precastKit));
    memcpy(&rec->m_castKit, &row[0x0008], sizeof(rec->m_castKit));
    memcpy(&rec->m_impactKit, &row[0x000C], sizeof(rec->m_impactKit));
    memcpy(&rec->m_stateKit, &row[0x0010], sizeof(rec->m_stateKit));
    memcpy(&rec->m_stateDoneKit, &row[0x0014], sizeof(rec->m_stateDoneKit));
    memcpy(&rec->m_channelKit, &row[0x0018], sizeof(rec->m_channelKit));
    memcpy(&rec->m_hasMissile, &row[0x001C], sizeof(rec->m_hasMissile));
    memcpy(&rec->m_missileModel, &row[0x0020], sizeof(rec->m_missileModel));
    memcpy(&rec->m_missilePathType, &row[0x0024], sizeof(rec->m_missilePathType));
    memcpy(&rec->m_missileDestinationAttachment, &row[0x0028], sizeof(rec->m_missileDestinationAttachment));
    memcpy(&rec->m_missileSound, &row[0x002C], sizeof(rec->m_missileSound));
    memcpy(&rec->m_animEventSoundID, &row[0x0030], sizeof(rec->m_animEventSoundID));
    memcpy(&rec->m_flags, &row[0x0034], sizeof(rec->m_flags));
    memcpy(&rec->m_casterImpactKit, &row[0x0038], sizeof(rec->m_casterImpactKit));
    memcpy(&rec->m_targetImpactKit, &row[0x003C], sizeof(rec->m_targetImpactKit));
    memcpy(&rec->m_missileAttachment, &row[0x0040], sizeof(rec->m_missileAttachment));
    memcpy(&rec->m_missileFollowGroundHeight, &row[0x0044], sizeof(rec->m_missileFollowGroundHeight));
    memcpy(&rec->m_missileFollowGroundDropSpeed, &row[0x0048], sizeof(rec->m_missileFollowGroundDropSpeed));
    memcpy(&rec->m_missileFollowGroundApproach, &row[0x004C], sizeof(rec->m_missileFollowGroundApproach));
    memcpy(&rec->m_missileFollowGroundFlags, &row[0x0050], sizeof(rec->m_missileFollowGroundFlags));
    memcpy(&rec->m_missileMotion, &row[0x0054], sizeof(rec->m_missileMotion));
    memcpy(&rec->m_missileTargetingKit, &row[0x0058], sizeof(rec->m_missileTargetingKit));
    memcpy(&rec->m_instantAreaKit, &row[0x005C], sizeof(rec->m_instantAreaKit));
    memcpy(&rec->m_impactAreaKit, &row[0x0060], sizeof(rec->m_impactAreaKit));
    memcpy(&rec->m_persistentAreaKit, &row[0x0064], sizeof(rec->m_persistentAreaKit));
    memcpy(rec->m_missileCastOffset, &row[0x0068], sizeof(rec->m_missileCastOffset));
    memcpy(rec->m_missileImpactOffset, &row[0x0074], sizeof(rec->m_missileImpactOffset));
}
#endif

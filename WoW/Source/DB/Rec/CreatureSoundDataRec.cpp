#include "WoW/Source/DB/Rec/CreatureSoundDataRec.h"

// STATIC: %0x00B46ED4
const DBCMeta CreatureSoundDataRec::s_meta = { "DBFilesClient\\CreatureSoundData.dbc", 33, 160, CreatureSoundDataRec::FixupStrings, 0 };

// PROVEN: bbc7c4c0 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void CreatureSoundDataRec::FixupStrings(void* record, const char* strings) {
}

#if defined(WOWDECOMP_ARCH_64)
void CreatureSoundDataRec::ConvertRow(CreatureSoundDataRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    memcpy(&rec->m_soundExertionID, &row[0x0004], sizeof(rec->m_soundExertionID));
    memcpy(&rec->m_soundExertionCriticalID, &row[0x0008], sizeof(rec->m_soundExertionCriticalID));
    memcpy(&rec->m_soundInjuryID, &row[0x000C], sizeof(rec->m_soundInjuryID));
    memcpy(&rec->m_soundInjuryCriticalID, &row[0x0010], sizeof(rec->m_soundInjuryCriticalID));
    memcpy(&rec->m_soundInjuryCrushingBlowID, &row[0x0014], sizeof(rec->m_soundInjuryCrushingBlowID));
    memcpy(&rec->m_soundDeathID, &row[0x0018], sizeof(rec->m_soundDeathID));
    memcpy(&rec->m_soundStunID, &row[0x001C], sizeof(rec->m_soundStunID));
    memcpy(&rec->m_soundStandID, &row[0x0020], sizeof(rec->m_soundStandID));
    memcpy(&rec->m_soundFootstepID, &row[0x0024], sizeof(rec->m_soundFootstepID));
    memcpy(&rec->m_soundAggroID, &row[0x0028], sizeof(rec->m_soundAggroID));
    memcpy(&rec->m_soundWingFlapID, &row[0x002C], sizeof(rec->m_soundWingFlapID));
    memcpy(&rec->m_soundWingGlideID, &row[0x0030], sizeof(rec->m_soundWingGlideID));
    memcpy(&rec->m_soundAlertID, &row[0x0034], sizeof(rec->m_soundAlertID));
    memcpy(rec->m_soundFidget, &row[0x0038], sizeof(rec->m_soundFidget));
    memcpy(rec->m_customAttack, &row[0x004C], sizeof(rec->m_customAttack));
    memcpy(&rec->m_npcsoundID, &row[0x005C], sizeof(rec->m_npcsoundID));
    memcpy(&rec->m_loopSoundID, &row[0x0060], sizeof(rec->m_loopSoundID));
    memcpy(&rec->m_creatureImpactType, &row[0x0064], sizeof(rec->m_creatureImpactType));
    memcpy(&rec->m_soundJumpStartID, &row[0x0068], sizeof(rec->m_soundJumpStartID));
    memcpy(&rec->m_soundJumpEndID, &row[0x006C], sizeof(rec->m_soundJumpEndID));
    memcpy(&rec->m_soundPetAttackID, &row[0x0070], sizeof(rec->m_soundPetAttackID));
    memcpy(&rec->m_soundPetOrderID, &row[0x0074], sizeof(rec->m_soundPetOrderID));
    memcpy(&rec->m_soundPetDismissID, &row[0x0078], sizeof(rec->m_soundPetDismissID));
    memcpy(&rec->m_fidgetDelaySecondsMin, &row[0x007C], sizeof(rec->m_fidgetDelaySecondsMin));
    memcpy(&rec->m_fidgetDelaySecondsMax, &row[0x0080], sizeof(rec->m_fidgetDelaySecondsMax));
    memcpy(&rec->m_birthSoundID, &row[0x0084], sizeof(rec->m_birthSoundID));
    memcpy(&rec->m_spellCastDirectedSoundID, &row[0x0088], sizeof(rec->m_spellCastDirectedSoundID));
    memcpy(&rec->m_submergeSoundID, &row[0x008C], sizeof(rec->m_submergeSoundID));
    memcpy(&rec->unk_0090, &row[0x0090], sizeof(rec->unk_0090));
    memcpy(&rec->unk_0094, &row[0x0094], sizeof(rec->unk_0094));
    memcpy(&rec->unk_0098, &row[0x0098], sizeof(rec->unk_0098));
    memcpy(&rec->unk_009C, &row[0x009C], sizeof(rec->unk_009C));
}
#endif

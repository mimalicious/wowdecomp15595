#include "WoW/Source/DB/Rec/SpellChainEffectsRec.h"

// STATIC: %0x00B45290
const DBCMeta SpellChainEffectsRec::s_meta = { "DBFilesClient\\SpellChainEffects.dbc", 48, 180, SpellChainEffectsRec::FixupStrings, 0 };

// OFFSET: %0x0097C270
void SpellChainEffectsRec::FixupStrings(void* record, const char* strings) {
    SpellChainEffectsRec* rec = static_cast<SpellChainEffectsRec*>(record);
    rec->m_texture = &strings[reinterpret_cast<uintptr_t>(rec->m_texture)];
    rec->m_combo = &strings[reinterpret_cast<uintptr_t>(rec->m_combo)];
}

#if defined(WOWDECOMP_ARCH_64)
void SpellChainEffectsRec::ConvertRow(SpellChainEffectsRec* rec, const uint8_t* row, const char* strings) {
    memcpy(&rec->m_id, &row[0x0000], sizeof(rec->m_id));
    memcpy(&rec->m_avgSegLen, &row[0x0004], sizeof(rec->m_avgSegLen));
    memcpy(&rec->m_width, &row[0x0008], sizeof(rec->m_width));
    memcpy(&rec->m_noiseScale, &row[0x000C], sizeof(rec->m_noiseScale));
    memcpy(&rec->m_texCoordScale, &row[0x0010], sizeof(rec->m_texCoordScale));
    memcpy(&rec->m_segDuration, &row[0x0014], sizeof(rec->m_segDuration));
    memcpy(&rec->m_segDelay, &row[0x0018], sizeof(rec->m_segDelay));
    rec->m_texture = &strings[*reinterpret_cast<const uint32_t*>(&row[0x001C])];
    memcpy(&rec->m_flags, &row[0x0020], sizeof(rec->m_flags));
    memcpy(&rec->m_jointCount, &row[0x0024], sizeof(rec->m_jointCount));
    memcpy(&rec->m_jointOffsetRadius, &row[0x0028], sizeof(rec->m_jointOffsetRadius));
    memcpy(&rec->m_jointsPerMinorJoint, &row[0x002C], sizeof(rec->m_jointsPerMinorJoint));
    memcpy(&rec->m_minorJointsPerMajorJoint, &row[0x0030], sizeof(rec->m_minorJointsPerMajorJoint));
    memcpy(&rec->m_minorJointScale, &row[0x0034], sizeof(rec->m_minorJointScale));
    memcpy(&rec->m_majorJointScale, &row[0x0038], sizeof(rec->m_majorJointScale));
    memcpy(&rec->m_jointMoveSpeed, &row[0x003C], sizeof(rec->m_jointMoveSpeed));
    memcpy(&rec->m_jointSmoothness, &row[0x0040], sizeof(rec->m_jointSmoothness));
    memcpy(&rec->m_minDurationBetweenJointJumps, &row[0x0044], sizeof(rec->m_minDurationBetweenJointJumps));
    memcpy(&rec->m_maxDurationBetweenJointJumps, &row[0x0048], sizeof(rec->m_maxDurationBetweenJointJumps));
    memcpy(&rec->m_waveHeight, &row[0x004C], sizeof(rec->m_waveHeight));
    memcpy(&rec->m_waveFreq, &row[0x0050], sizeof(rec->m_waveFreq));
    memcpy(&rec->m_waveSpeed, &row[0x0054], sizeof(rec->m_waveSpeed));
    memcpy(&rec->m_minWaveAngle, &row[0x0058], sizeof(rec->m_minWaveAngle));
    memcpy(&rec->m_maxWaveAngle, &row[0x005C], sizeof(rec->m_maxWaveAngle));
    memcpy(&rec->m_minWaveSpin, &row[0x0060], sizeof(rec->m_minWaveSpin));
    memcpy(&rec->m_maxWaveSpin, &row[0x0064], sizeof(rec->m_maxWaveSpin));
    memcpy(&rec->m_arcHeight, &row[0x0068], sizeof(rec->m_arcHeight));
    memcpy(&rec->m_minArcAngle, &row[0x006C], sizeof(rec->m_minArcAngle));
    memcpy(&rec->m_maxArcAngle, &row[0x0070], sizeof(rec->m_maxArcAngle));
    memcpy(&rec->m_minArcSpin, &row[0x0074], sizeof(rec->m_minArcSpin));
    memcpy(&rec->m_maxArcSpin, &row[0x0078], sizeof(rec->m_maxArcSpin));
    memcpy(&rec->m_delayBetweenEffects, &row[0x007C], sizeof(rec->m_delayBetweenEffects));
    memcpy(&rec->m_minFlickerOnDuration, &row[0x0080], sizeof(rec->m_minFlickerOnDuration));
    memcpy(&rec->m_maxFlickerOnDuration, &row[0x0084], sizeof(rec->m_maxFlickerOnDuration));
    memcpy(&rec->m_minFlickerOffDuration, &row[0x0088], sizeof(rec->m_minFlickerOffDuration));
    memcpy(&rec->m_maxFlickerOffDuration, &row[0x008C], sizeof(rec->m_maxFlickerOffDuration));
    memcpy(&rec->m_pulseSpeed, &row[0x0090], sizeof(rec->m_pulseSpeed));
    memcpy(&rec->m_pulseOnLength, &row[0x0094], sizeof(rec->m_pulseOnLength));
    memcpy(&rec->m_pulseFadeLength, &row[0x0098], sizeof(rec->m_pulseFadeLength));
    memcpy(&rec->m_alpha, &row[0x009C], sizeof(rec->m_alpha));
    memcpy(&rec->m_red, &row[0x009D], sizeof(rec->m_red));
    memcpy(&rec->m_green, &row[0x009E], sizeof(rec->m_green));
    memcpy(&rec->m_blue, &row[0x009F], sizeof(rec->m_blue));
    memcpy(&rec->m_blendMode, &row[0x00A0], sizeof(rec->m_blendMode));
    rec->m_combo = &strings[*reinterpret_cast<const uint32_t*>(&row[0x00A4])];
    memcpy(&rec->m_renderLayer, &row[0x00A8], sizeof(rec->m_renderLayer));
    memcpy(&rec->m_textureLength, &row[0x00AC], sizeof(rec->m_textureLength));
    memcpy(&rec->m_wavePhase, &row[0x00B0], sizeof(rec->m_wavePhase));
}
#endif

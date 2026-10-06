#include <cmath>
#include <cstddef>
#include <cstdint>
#include "Engine/Source/SoundEngine/SE3.h"
#include "WoW/Source/DB/Rec/SoundEntriesAdvancedRec.h"

// STATIC: %0x00C05BA8
static TSList<SuppressorSound, TSGetLink<SuppressorSound> > s_suppressors;

// PROVEN: 1e007cf1 IDENTICAL/DIFFERENT
// OFFSET: 0x00716DA0/0x1403EA240
int32_t SE3::SuppressorIsPresent() {
    return s_suppressors.Head() != NULL;
}

// OFFSET: %0x0067D280
void SE3::UnregisterSuppressor(uint32_t uid) {
    SuppressorSound* suppressor = s_suppressors.Head();

    while (suppressor) {
        SuppressorSound* next = s_suppressors.Next(suppressor);

        if (suppressor->m_uUID == uid) {
            suppressor->m_uUID = 0;
        }

        suppressor = next;
    }
}

// OFFSET: %0x0067D2C0
float SE3::GetSuppressionScalar(SE3BUS bus, uint32_t uid) {
    float scalar = 1.0f;

    for (SuppressorSound* suppressor = s_suppressors.Head(); suppressor; suppressor = s_suppressors.Next(suppressor)) {
        if (suppressor->m_uUID == uid) {
            scalar = 1.0f;
        } else if (scalar > suppressor->m_scalar[bus]) {
            scalar = suppressor->m_scalar[bus];
        }
    }

    return scalar;
}

// OFFSET: %0x0067D330
void SE3::RegisterSuppressor(SE3SoundKitObject* sko) {
    if (!sko || !sko->m_pSoundKitDef->m_pSoundKitRec) {
        return;
    }

    const SoundEntriesAdvancedRec* advancedRec = sko->m_pSoundKitDef->m_pAdvancedSoundKitRec;

    if (!advancedRec) {
        return;
    }

    if ((advancedRec->m_duckToSfx >= 0.0f && advancedRec->m_duckToSfx < 1.0f) || (advancedRec->m_duckToMusic >= 0.0f && advancedRec->m_duckToMusic < 1.0f) || (advancedRec->m_duckToAmbience >= 0.0f && advancedRec->m_duckToAmbience < 1.0f)) {
        for (SuppressorSound* suppressor = s_suppressors.Head(); suppressor; suppressor = s_suppressors.Next(suppressor)) {
            if (suppressor->m_uUID == sko->m_uUID) {
                return;
            }
        }

        SuppressorSound* suppressor = new (__FILE__, __LINE__) SuppressorSound();
        suppressor->m_uUID = sko->m_uUID;
        suppressor->m_duckTo[0] = advancedRec->m_duckToSfx;
        suppressor->m_duckTo[1] = advancedRec->m_duckToMusic;
        suppressor->m_duckTo[2] = advancedRec->m_duckToAmbience;
        suppressor->m_timeToUnduck = advancedRec->m_timeToUnduck;
        suppressor->m_timeToDuck = advancedRec->m_timeToDuck;
        suppressor->m_innerRadius = advancedRec->m_innerRadiusOfInfluence;
        suppressor->m_outerRadius = advancedRec->m_outerRadiusOfInfluence;
        s_suppressors.LinkToTail(suppressor);
    }
}

// OFFSET: %0x0067D4E0
void SE3::InitSuppressorSounds() {
    s_suppressors.Clear();
}

// OFFSET: %0x0067D4F0
void SE3::UpdateSuppressors(int32_t elapsed) {
    C3Vector listener;
    listener.x = 0.0f;
    listener.y = 0.0f;
    listener.z = 0.0f;
    SE3::GetListenerPosition(&listener);

    int32_t present = s_suppressors.Head() != NULL;
    SuppressorSound* suppressor = s_suppressors.Head();

    while (suppressor) {
        SuppressorSound* next = s_suppressors.Next(suppressor);
        C3Vector position = SE3::GetPosition(suppressor->m_uUID);

        for (int32_t i = 0; i < 3; i++) {
            uint32_t uid = suppressor->m_uUID;

            if (uid) {
                int32_t atOrigin = position.x == 0.0f && position.y == 0.0f && position.z == 0.0f;
                float distance = sqrt((listener.x - position.x) * (listener.x - position.x) + (listener.y - position.y) * (listener.y - position.y) + (listener.z - position.z) * (listener.z - position.z));
                suppressor->m_distance = distance;

                if ((atOrigin || suppressor->m_innerRadius > distance || (suppressor->m_outerRadius > distance && suppressor->m_active)) && SE3::IsPlaying(uid)) {
                    float duckTo = suppressor->m_duckTo[i];

                    if (suppressor->m_scalar[i] != duckTo) {
                        if (suppressor->m_timeToDuck <= 0) {
                            suppressor->m_scalar[i] = duckTo;
                        } else {
                            suppressor->m_scalar[i] -= static_cast<float>(elapsed) / static_cast<float>(suppressor->m_timeToDuck);

                            if (duckTo > suppressor->m_scalar[i]) {
                                suppressor->m_scalar[i] = duckTo;
                            }
                        }
                    }

                    suppressor->m_active = 1;
                } else {
                    suppressor->m_active = 0;
                    SE3::UnregisterSuppressor(suppressor->m_uUID);
                }
            } else if (suppressor->m_scalar[i] != 1.0f) {
                if (suppressor->m_timeToUnduck <= 0) {
                    suppressor->m_scalar[i] = 1.0f;
                } else {
                    suppressor->m_scalar[i] += static_cast<float>(elapsed) / static_cast<float>(suppressor->m_timeToUnduck);

                    if (suppressor->m_scalar[i] > 1.0f) {
                        suppressor->m_scalar[i] = 1.0f;
                    }
                }
            }
        }

        if (!suppressor->m_uUID && suppressor->m_scalar[0] == 1.0f && suppressor->m_scalar[1] == 1.0f && suppressor->m_scalar[2] == 1.0f) {
            delete suppressor;
        }

        suppressor = next;
    }

    if (present) {
        SE3SoundKitObject* sko = SE3::sm_SoundKitObjects.Head();

        while (sko) {
            SE3SoundKitObject* nextSko = SE3::sm_SoundKitObjects.Next(sko);
            sko->UpdateVolume();
            sko = nextSko;
        }
    }
}

#ifndef WOW_SOURCE_MAGIC_SPELL_H
#define WOW_SOURCE_MAGIC_SPELL_H

#include <cstddef>
#include <cstdint>

class C3Vector;

float SpellUtils_ComputeMissileTrajectoryTravelTime(float pitch, float speed, const C3Vector& delta, float gravity);
int32_t SpellUtils_ComputeMissileTrajectory(float pitch, const C3Vector& delta, float gravity, float& speed, float& time, C3Vector* velocity);
int32_t GetSkinningDifficulty(int32_t level);

bool SpellIsAuraEffect(int32_t effect);
bool SpellIsApplyAuraEffect(int32_t effect);
bool SpellIsDamageEffect(int32_t effect, int32_t aura);
bool SpellIsAreaAuraEffect(int32_t effect);

#endif

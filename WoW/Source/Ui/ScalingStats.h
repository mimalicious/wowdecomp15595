#ifndef WOW_SOURCE_UI_SCALINGSTATS_H
#define WOW_SOURCE_UI_SCALINGSTATS_H

#include <cstdint>

class SpellItemEnchantmentRec;

int32_t GetRandomItemSlot(int32_t inventoryType);
int32_t GetDamageClassFromBonusStat(int32_t stat);
int32_t GetRandomPropertyPointsEx(int32_t itemLevel, int32_t quality, int32_t inventoryType);
int32_t GetEffectiveArmor(int32_t level, int32_t itemLevel, int32_t classID, int32_t subclassID, uint32_t quality, int32_t inventoryType);
float GetEffectiveWeaponDamage(int32_t level, int32_t itemLevel, int32_t classID, int32_t subclassID, uint32_t quality, float damageModifier, int32_t delay, float statScalingFactor, int32_t inventoryType, int32_t flags, bool minimum);
int32_t CalculateItemBonusFromPercent(int32_t level, int32_t itemLevel, int32_t amount, int32_t percentEditor, int32_t socketPercentage, int32_t quality, int32_t inventoryType);
int32_t GetScaledEnchantmentPoints(int32_t itemLevel, int32_t maxItemLevel, const SpellItemEnchantmentRec* enchant, int32_t points);

#endif

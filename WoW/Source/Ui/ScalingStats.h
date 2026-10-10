#ifndef WOW_SOURCE_UI_SCALINGSTATS_H
#define WOW_SOURCE_UI_SCALINGSTATS_H

#include <cstdint>

int32_t GetRandomItemSlot(int32_t inventoryType);
int32_t GetRandomPropertyPointsEx(int32_t itemLevel, int32_t quality, int32_t inventoryType);
int32_t CalculateItemBonusFromPercent(int32_t level, int32_t itemLevel, int32_t amount, int32_t percentEditor, int32_t socketPercentage, int32_t quality, int32_t inventoryType);

#endif

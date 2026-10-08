#include <cstdint>
#include "WoW/Source/Game/GameClient/Minimap.h"

// STATIC: 0x00DC73B4
static uint32_t s_minimapUpdateFlags;

// PROVEN: 99488e7c IDENTICAL/NONE
// OFFSET: 0x004E85F0
void MinimapSetForceUpdate() {
    s_minimapUpdateFlags |= 4;
}

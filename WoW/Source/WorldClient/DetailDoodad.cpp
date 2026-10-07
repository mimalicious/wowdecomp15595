#include "WoW/Source/WorldClient/DetailDoodad.h"
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"

// STATIC: 0x00E743A0
int32_t CDetailDoodad::s_poolsDirty;

// STATIC: 0x00E743C4
int32_t CDetailDoodad::s_instanced;

// PROVEN: ffc79216 IDENTICAL/NONE
// OFFSET: 0x00736270
void CDetailDoodad::DestroyPools() {
    CDetailDoodad::s_poolsDirty = 1;
}

// PROVEN: 54e42009 IDENTICAL/NONE
// OFFSET: 0x00736270
void CDetailDoodad::DirtyPools() {
    CDetailDoodad::s_poolsDirty = 1;
}

// PROVEN: c716616c IDENTICAL/NONE
// OFFSET: 0x00736280/0x140412930
int32_t CDetailDoodad::IsInstanced() {
    return CDetailDoodad::s_instanced;
}

// PROVEN: 65aa7a5f IDENTICAL/NONE
// OFFSET: 0x007362A0
void CDetailDoodad::GetDistRange(float& minDist, float& maxDist) {
    minDist = 0.0f;
    maxDist = CDetailDoodad::s_instanced ? 500.0f : 140.0f;
}

// PROVEN: 6ed0d059 IDENTICAL/NONE
// OFFSET: 0x007362E0
void CDetailDoodad::GetDensityRange(uint32_t& minDensity, uint32_t& maxDensity) {
    minDensity = 16;
    maxDensity = CDetailDoodad::s_instanced ? 256 : 64;
}

// PROVEN: 1b9103a6 IDENTICAL/NONE
// OFFSET: 0x00736A10/0x140413080
void CDetailDoodad::SetInstanced(int32_t instanced) {
    CDetailDoodad::s_instanced = g_theGxDevicePtr->m_caps.unk_0304 && instanced ? 1 : 0;
    CDetailDoodad::s_poolsDirty = 1;
}

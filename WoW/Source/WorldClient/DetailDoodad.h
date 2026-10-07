#ifndef WOW_SOURCE_WORLDCLIENT_DETAILDOODAD_H
#define WOW_SOURCE_WORLDCLIENT_DETAILDOODAD_H

#include <cstdint>

class CDetailDoodadInst {
    public:
    void Update(float dist);
};

class CDetailDoodad {
    public:
    static int32_t s_instanced;
    static int32_t s_poolsDirty;

    static void Destroy();
    static void DestroyPools();
    static void DirtyPools();
    static void FreeInst(CDetailDoodadInst* inst);
    static void GetDensityRange(uint32_t& minDensity, uint32_t& maxDensity);
    static void GetDistRange(float& minDist, float& maxDist);
    static void Initialize();
    static int32_t IsInstanced();
    static int32_t IsModelLoaded(uint32_t doodadId);
    static void LinkToAsyncHead(uint32_t doodadId);
    static void SetInstanced(int32_t instanced);
    static void UpdatePools();
};

#endif

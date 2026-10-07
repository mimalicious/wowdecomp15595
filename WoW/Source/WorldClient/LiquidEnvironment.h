#ifndef WOW_SOURCE_WORLDCLIENT_LIQUIDENVIRONMENT_H
#define WOW_SOURCE_WORLDCLIENT_LIQUIDENVIRONMENT_H

#include <cstdint>
#include "Engine/Source/Base/CDataAllocator.h"
#include "WoW/Source/LiquidSystem/Liquid.h"

class CClientLiquidEnvironment : public Liquid::IEnvironment {
    public:
    static CDataAllocator s_allocator;
    static CClientLiquidEnvironment* s_shared;

    static CClientLiquidEnvironment* Create(int32_t shared, int32_t localLight);
    static void Destroy();

    /* 0004 */ int32_t m_interiorFog;
    /* 0008 */ int32_t m_localLight;

    virtual void Release();
    virtual void SetInteriorFog(int32_t interiorFog);
    virtual void SelectLights(CM2Lighting& lighting);
};

#endif

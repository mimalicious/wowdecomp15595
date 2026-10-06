#include <cstdint>
#include "Engine/Source/Scrn/Layer.h"
#include "Engine/Source/Scrn/Scrn.h"
#include "Engine/Source/Scrn/Stock.h"

// STATIC: %0x00D879B8
static int32_t s_initParam;

// OFFSET: %0x00766960
void ScrnDestroy() {
    ILayerDestroy();
    IStockDestroy();
}

// OFFSET: %0x00766970
void ScrnInitialize(int32_t param) {
    ILayerInitialize();
    s_initParam = param;
    IStockInitialize();
}

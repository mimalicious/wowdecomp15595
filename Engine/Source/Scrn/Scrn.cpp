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

// PROVEN: d1a7e02d IDENTICAL/UNPROVEN
// OFFSET: 0x007FFDB0/0x140513BC0
void ScrnInitialize(int32_t param) {
    ILayerInitialize();
    s_initParam = param;
    IStockInitialize();
}

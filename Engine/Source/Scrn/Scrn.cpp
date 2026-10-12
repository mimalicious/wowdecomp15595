#include <cstdint>
#include "Engine/Source/Scrn/Layer.h"
#include "Engine/Source/Scrn/Scrn.h"
#include "Engine/Source/Scrn/Stock.h"

// STATIC: 0x00EBB118/0x140DABE98
static int32_t s_initParam;

// OFFSET: 0x007FFDA0/0x140513BA0
void ScrnDestroy() {
    ILayerDestroy();
    IStockDestroy();
}

// OFFSET: 0x007FFDB0/0x140513BC0
void ScrnInitialize(int32_t param) {
    ILayerInitialize();
    s_initParam = param;
    IStockInitialize();
}

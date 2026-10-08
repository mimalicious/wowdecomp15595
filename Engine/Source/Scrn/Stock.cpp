#include <cstdint>
#include "Engine/Source/Base/Coordinate.h"
#include "Engine/Source/Base/Handle.h"
#include "Engine/Source/Gxu/GxuFont.h"
#include "Engine/Source/Os/Path.h"
#include "Engine/Source/Scrn/Scrn.h"
#include "Engine/Source/Scrn/Stock.h"
#include "Engine/Source/Services/TextBlock.h"
#include "Engine/Source/Storm/H/storm.h"

// STATIC: 0x00BCEA94/0x1409C2E10
static const float s_stockObjectHeights[SCRNSTOCKOBJECTS] = { 0.01953125f, 0.01953125f };

// STATIC: 0x00EBB11C
static HTEXTFONT s_stockObjects[SCRNSTOCKOBJECTS];

// OFFSET: 0x007FFDD0
void ScrnSetStockFont(SCRNSTOCK stockID, const char* fontTexturePath) {
    if (s_stockObjects[stockID]) {
        HandleClose(reinterpret_cast<HOBJECT>(s_stockObjects[stockID]));
    }

    s_stockObjects[stockID] = TextBlockGenerateFont(fontTexturePath, 0, NDCToDDCHeight(s_stockObjectHeights[stockID]));
}

// PROVEN: de0bfc4c IDENTICAL/UNPROVEN
// OFFSET: 0x007FFE50/0x140513C30
HTEXTFONT ScrnGetStockFont(SCRNSTOCK stockID) {
    if (stockID >= SCRNSTOCKOBJECTS) {
        SErrSetLastError(STORM_ERROR_INVALID_PARAMETER);

        return NULL;
    }

    return s_stockObjects[stockID];
}

// PROVEN: f534407a IDENTICAL/IDENTICAL
// OFFSET: 0x007FFE70/0x140513C60
float ScrnGetStockFontHeight(SCRNSTOCK stockID) {
    if (stockID >= SCRNSTOCKOBJECTS) {
        SErrSetLastError(STORM_ERROR_INVALID_PARAMETER);

        return 0.0f;
    }

    return s_stockObjectHeights[stockID];
}

// OFFSET: 0x007FFE90/0x140513C90
void IStockInitialize() {
    GxuFontInitialize();

    char fontTexturePath[260];
    OsBuildFontFilePath("FRIZQT__.TTF", fontTexturePath, sizeof(fontTexturePath));

    if (fontTexturePath[0]) {
        ScrnSetStockFont(STOCK_SYSFONT, fontTexturePath);
    } else {
        SErrSetLastError(STORM_ERROR_INVALID_PARAMETER);
    }

    if (fontTexturePath[0]) {
        ScrnSetStockFont(STOCK_PERFFONT, fontTexturePath);
    } else {
        SErrSetLastError(STORM_ERROR_INVALID_PARAMETER);
    }
}

// INLINED
void IStockDestroy() {
    for (uint32_t i = 0; i < SCRNSTOCKOBJECTS; i++) {
        if (s_stockObjects[i]) {
            HandleClose(reinterpret_cast<HOBJECT>(s_stockObjects[i]));
        }

        s_stockObjects[i] = NULL;
    }

    GxuFontShutdown();
}

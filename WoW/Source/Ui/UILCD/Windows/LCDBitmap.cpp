#include "WoW/Source/Ui/UILCD/Windows/LCDBitmap.h"
#include <cmath>
#include "WoW/Source/Ui/UILCD/Windows/LCDGfx.h"

typedef BOOL (WINAPI* ALPHABLENDPROC)(HDC hdcDest, int xDest, int yDest, int widthDest, int heightDest, HDC hdcSrc, int xSrc, int ySrc, int widthSrc, int heightSrc, BLENDFUNCTION blend);

// STATIC: 0x0100AACC
static bool s_alphaBlendLoaded;

// STATIC: 0x0100AAD0
static ALPHABLENDPROC s_alphaBlend;

// OFFSET: 0x00B3FB10
CLCDBitmap::CLCDBitmap() {
    this->m_fZoom = 1.0f;
    this->m_hBitmap = NULL;
    this->m_dwROP = SRCCOPY;
    this->m_bAlpha = 1;

    if (!s_alphaBlend && !s_alphaBlendLoaded) {
        s_alphaBlendLoaded = true;
        s_alphaBlend = reinterpret_cast<ALPHABLENDPROC>(GetProcAddress(LoadLibraryA("Msimg32.dll"), "AlphaBlend"));
    }
}

// OFFSET: [#0x0095ADF0, #0x0095AE40]
CLCDBitmap::~CLCDBitmap() {
}

// PROVEN: 302a8fad IDENTICAL/NONE
// OFFSET: 0x00B3FB90/0x14090D6F0
void CLCDBitmap::SetBitmap(HBITMAP bitmap) {
    this->m_hBitmap = bitmap;
}

// PROVEN: 4d42703e IDENTICAL/NONE
// OFFSET: 0x00B3FBA0/0x14090D700
void CLCDBitmap::SetAlpha(int32_t alpha) {
    this->m_bAlpha = alpha;
}

// PROVEN: 0064655e IDENTICAL/DIFFERENT
// OFFSET: 0x007AB8C0/0x14090D710
void CLCDBitmap::SetBitmapSize(int32_t width, int32_t height) {
    this->m_nBitmapWidth = width;
    this->m_nBitmapHeight = height;
}

// OFFSET: 0x00B3FBE0
void CLCDBitmap::OnDraw(CLCDGfxBase& gfx) {
    if (!this->m_hBitmap) {
        return;
    }

    HDC compatibleDC = CreateCompatibleDC(gfx.GetHDC());
    HGDIOBJ oldBitmap = SelectObject(compatibleDC, this->m_hBitmap);

    if (gfx.GetLCDScreen()->Format == LGLCD_BMP_FORMAT_160x43x1) {
        BitBlt(gfx.GetHDC(), 0, 0, this->m_sizeLogical.cx, this->m_sizeLogical.cy, compatibleDC, 0, 0, this->m_dwROP);
    } else if (std::fabs(1.0f - this->m_fZoom) >= 0.001f) {
        if (s_alphaBlend) {
            BLENDFUNCTION blend = { AC_SRC_OVER, 0, 0xFF, AC_SRC_ALPHA };

            if (!this->m_bAlpha) {
                blend.AlphaFormat = 0;
            }

            s_alphaBlend(gfx.GetHDC(), 0, 0, static_cast<int>(this->m_sizeLogical.cx * this->m_fZoom), static_cast<int>(this->m_sizeLogical.cy * this->m_fZoom), compatibleDC, 0, 0, this->m_nBitmapWidth, this->m_nBitmapHeight, blend);
        }
    } else if (!this->m_bAlpha || !s_alphaBlend) {
        StretchBlt(gfx.GetHDC(), 0, 0, this->m_sizeLogical.cx, this->m_sizeLogical.cy, compatibleDC, 0, 0, this->m_nBitmapWidth, this->m_nBitmapHeight, this->m_dwROP);
    } else {
        BLENDFUNCTION blend = { AC_SRC_OVER, 0, 0xFF, AC_SRC_ALPHA };
        s_alphaBlend(gfx.GetHDC(), 0, 0, this->m_sizeLogical.cx, this->m_sizeLogical.cy, compatibleDC, 0, 0, this->m_nBitmapWidth, this->m_nBitmapHeight, blend);
    }

    SelectObject(compatibleDC, oldBitmap);
    DeleteDC(compatibleDC);
}

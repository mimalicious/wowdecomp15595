#include "WoW/Source/Ui/UILCD/Windows/LCDGfx.h"
#include <cstring>

// OFFSET: #0x0095BF90
CLCDGfxColor::CLCDGfxColor() {
    this->m_pLCDScreen = &this->m_lcdBitmap.hdr;
    this->m_lcdBitmap.hdr.Format = LGLCD_BMP_FORMAT_QVGAx32;
}

// OFFSET: #0x0095C030
CLCDGfxColor::~CLCDGfxColor() {
}

// PROVEN: 3559c00b IDENTICAL
// OFFSET: 0x00B40D50
HRESULT CLCDGfxColor::Initialize() {
    this->Shutdown();

    this->m_nWidth = LGLCD_QVGA_BMP_WIDTH;
    this->m_nHeight = LGLCD_QVGA_BMP_HEIGHT;

    HRESULT hr = CLCDGfxBase::Initialize();

    if (FAILED(hr)) {
        return hr;
    }

    hr = this->CreateBitmap(32);

    return FAILED(hr) ? hr : S_OK;
}

// PROVEN: f2f2ae42 IDENTICAL
// OFFSET: 0x00B40D90
lgLcdBitmapHeader* CLCDGfxColor::GetLCDScreen() {
    this->m_lcdBitmap.hdr.Format = LGLCD_BMP_FORMAT_QVGAx32;
    memcpy(this->m_lcdBitmap.pixels, this->m_pBitmapBits, 4 * this->m_nWidth * this->m_nHeight);
    return this->m_pLCDScreen;
}

// PROVEN: 9c0ded8d IDENTICAL
// OFFSET: 0x00A39FF0
uint32_t CLCDGfxColor::GetFamily() {
    return LGLCD_DEVICE_FAMILY_QVGA_BASIC;
}

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

// OFFSET: %0x00A73050
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

// OFFSET: %0x00A73090
lgLcdBitmapHeader* CLCDGfxColor::GetLCDScreen() {
    this->m_lcdBitmap.hdr.Format = LGLCD_BMP_FORMAT_QVGAx32;
    memcpy(this->m_lcdBitmap.pixels, this->m_pBitmapBits, 4 * this->m_nWidth * this->m_nHeight);
    return this->m_pLCDScreen;
}

// OFFSET: %0x0097A300
uint32_t CLCDGfxColor::GetFamily() {
    return LGLCD_DEVICE_FAMILY_QVGA_BASIC;
}

#include "WoW/Source/Ui/UILCD/Windows/LCDGfx.h"
#include <cstring>

// OFFSET: #0x0095BEA0
CLCDGfxMono::CLCDGfxMono() {
    this->m_pLCDScreen = &this->m_lcdBitmap.hdr;
    this->m_lcdBitmap.hdr.Format = LGLCD_BMP_FORMAT_160x43x1;
}

// OFFSET: #0x0095BF60
CLCDGfxMono::~CLCDGfxMono() {
}

// PROVEN: e61933a4 IDENTICAL/NONE
// OFFSET: 0x00B40C60/0x14090EBC0
HRESULT CLCDGfxMono::Initialize() {
    this->Shutdown();

    this->m_nWidth = LGLCD_BMP_WIDTH;
    this->m_nHeight = LGLCD_BMP_HEIGHT;

    HRESULT hr = CLCDGfxBase::Initialize();

    if (FAILED(hr)) {
        return hr;
    }

    hr = this->CreateBitmap(8);

    return FAILED(hr) ? hr : S_OK;
}

// PROVEN: 12559dfa IDENTICAL/NONE
// OFFSET: 0x00B40CD0/0x14090EC50
void CLCDGfxMono::ClearScreen() {
    memset(this->m_pBitmapBits, 0, this->m_nWidth * this->m_nHeight);
    CLCDGfxBase::ClearScreen();
}

// PROVEN: 20d2502b IDENTICAL/NONE
// OFFSET: 0x00B40CA0
lgLcdBitmapHeader* CLCDGfxMono::GetLCDScreen() {
    this->m_lcdBitmap.hdr.Format = LGLCD_BMP_FORMAT_160x43x1;
    memcpy(this->m_lcdBitmap.pixels, this->m_pBitmapBits, this->m_nWidth * this->m_nHeight);
    return this->m_pLCDScreen;
}

// PROVEN: 314490ba IDENTICAL/NONE
// OFFSET: 0x009FCE20/0x14090FD50
uint32_t CLCDGfxMono::GetFamily() {
    return LGLCD_DEVICE_FAMILY_BW_160x43_GAMING;
}

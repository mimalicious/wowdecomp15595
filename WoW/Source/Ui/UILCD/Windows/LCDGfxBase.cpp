#include "WoW/Source/Ui/UILCD/Windows/LCDGfx.h"
#include <cstring>
#include "Engine/Source/Storm/H/storm.h"

// OFFSET: #0x0095CA00
CLCDGfxBase::CLCDGfxBase() {
    this->m_pLCDScreen = NULL;
    this->m_nWidth = 0;
    this->m_nHeight = 0;
    this->m_pBitmapInfo = NULL;
    this->m_hDC = NULL;
    this->m_hBitmap = NULL;
    this->m_hPrevBitmap = NULL;
    this->m_pBitmapBits = NULL;
}

// OFFSET: [#0x0095CCE0, #0x0095CCF0]
CLCDGfxBase::~CLCDGfxBase() {
    this->Shutdown();
}

// PROVEN: 82a42512 IDENTICAL/NONE
// OFFSET: 0x00B41930/0x14090FBE0
HRESULT CLCDGfxBase::Initialize() {
    this->m_hDC = CreateCompatibleDC(NULL);

    if (!this->m_hDC) {
        this->Shutdown();
        return E_FAIL;
    }

    return S_OK;
}

// OFFSET: #0x0095CB60
void CLCDGfxBase::Shutdown() {
    if (this->m_hBitmap) {
        DeleteObject(this->m_hBitmap);
        this->m_hBitmap = NULL;
        this->m_pBitmapBits = NULL;
    }

    this->m_hPrevBitmap = NULL;

    if (this->m_pBitmapInfo) {
        delete[] reinterpret_cast<uint8_t*>(this->m_pBitmapInfo);
        this->m_pBitmapInfo = NULL;
    }

    if (this->m_hDC) {
        DeleteDC(this->m_hDC);
        this->m_hDC = NULL;
    }

    this->m_nHeight = 0;
    this->m_nWidth = 0;
}

// PROVEN: 38476198 IDENTICAL/NONE
// OFFSET: 0x00B41AC0/0x14090FDE0
HRESULT CLCDGfxBase::CreateBitmap(uint16_t bitCount) {
    this->m_pBitmapInfo = reinterpret_cast<BITMAPINFO*>(new (__FILE__, __LINE__) uint8_t[sizeof(BITMAPINFO) + 256 * sizeof(RGBQUAD)]);

    if (!this->m_pBitmapInfo) {
        this->Shutdown();
        return E_OUTOFMEMORY;
    }

    memset(this->m_pBitmapInfo, 0, sizeof(BITMAPINFO) + 256 * sizeof(RGBQUAD));

    this->m_pBitmapInfo->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    this->m_pBitmapInfo->bmiHeader.biWidth = this->m_nWidth;
    this->m_pBitmapInfo->bmiHeader.biHeight = -this->m_nHeight;
    this->m_pBitmapInfo->bmiHeader.biPlanes = 1;
    this->m_pBitmapInfo->bmiHeader.biBitCount = bitCount;
    this->m_pBitmapInfo->bmiHeader.biCompression = BI_RGB;
    this->m_pBitmapInfo->bmiHeader.biSizeImage = this->m_nWidth * this->m_nHeight * this->m_pBitmapInfo->bmiHeader.biBitCount / 8;
    this->m_pBitmapInfo->bmiHeader.biXPelsPerMeter = 3200;
    this->m_pBitmapInfo->bmiHeader.biYPelsPerMeter = 3200;
    this->m_pBitmapInfo->bmiHeader.biClrUsed = 256;
    this->m_pBitmapInfo->bmiHeader.biClrImportant = 256;

    for (int32_t i = 0; i < 256; i++) {
        uint8_t color = i > 128 ? 0xFF : 0x00;
        this->m_pBitmapInfo->bmiColors[i].rgbRed = color;
        this->m_pBitmapInfo->bmiColors[i].rgbGreen = color;
        this->m_pBitmapInfo->bmiColors[i].rgbBlue = color;
        this->m_pBitmapInfo->bmiColors[i].rgbReserved = 0;
    }

    this->m_hBitmap = CreateDIBSection(this->m_hDC, this->m_pBitmapInfo, DIB_RGB_COLORS, reinterpret_cast<void**>(&this->m_pBitmapBits), NULL, 0);

    if (!this->m_hBitmap) {
        this->Shutdown();
        return E_FAIL;
    }

    return S_OK;
}

// PROVEN: dae91d09 IDENTICAL/NONE
// OFFSET: 0x00B41960/0x14090FC20
void CLCDGfxBase::ClearScreen() {
    RECT rect = { 0, 0, this->m_nWidth, this->m_nHeight };
    FillRect(this->m_hDC, &rect, static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH)));
}

// PROVEN: a63538b8 IDENTICAL/NONE
// OFFSET: 0x00B419A0/0x14090FC70
void CLCDGfxBase::BeginDraw() {
    if (!this->m_hPrevBitmap) {
        this->m_hPrevBitmap = static_cast<HBITMAP>(SelectObject(this->m_hDC, this->m_hBitmap));
        SetTextColor(this->m_hDC, RGB(255, 255, 255));
        SetBkColor(this->m_hDC, RGB(0, 0, 0));
    }
}

// PROVEN: 6cde81ca IDENTICAL/NONE
// OFFSET: 0x00B419E0/0x14090FCC0
void CLCDGfxBase::EndDraw() {
    if (this->m_hPrevBitmap) {
        GdiFlush();
        SelectObject(this->m_hDC, this->m_hPrevBitmap);
        this->m_hPrevBitmap = NULL;
    }
}

// PROVEN: 5d76ee5e IDENTICAL/NONE
// OFFSET: 0x00B40BC0/0x14090EAF0
HDC CLCDGfxBase::GetHDC() {
    return this->m_hDC;
}

// PROVEN: b2aa2441 IDENTICAL/NONE
// OFFSET: 0x00B41A10/0x14090FD10
lgLcdBitmapHeader* CLCDGfxBase::GetLCDScreen() {
    if (this->m_pLCDScreen) {
        memcpy(this->m_pLCDScreen + 1, this->m_pBitmapBits, 4 * this->m_nWidth * this->m_nHeight);
    }

    return this->m_pLCDScreen;
}

// PROVEN: 90a331a4 IDENTICAL/DIFFERENT
// OFFSET: 0x00B41A40/0x14090FD00
BITMAPINFO* CLCDGfxBase::GetBitmapInfo() {
    return this->m_pBitmapInfo;
}

// PROVEN: fc78ba6e IDENTICAL/NONE
// OFFSET: 0x00B41A50/0x140208F50
HBITMAP CLCDGfxBase::GetHBITMAP() {
    return this->m_hBitmap;
}

// PROVEN: 4908a79b IDENTICAL/DIFFERENT
// OFFSET: 0x00B41A60/0x14090EBB0
int32_t CLCDGfxBase::GetWidth() {
    return this->m_nWidth;
}

// OFFSET: #0x008C32F0
int32_t CLCDGfxBase::GetHeight() {
    return this->m_nHeight;
}

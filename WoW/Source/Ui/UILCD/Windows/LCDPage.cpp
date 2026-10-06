#include "WoW/Source/Ui/UILCD/Windows/LCDPage.h"
#include "WoW/Source/Ui/UILCD/Windows/LCDGfx.h"

// OFFSET: #0x0095B4F0
CLCDPage::CLCDPage() : m_dwStartTime(0), m_dwEllapsedTime(0), m_dwExpirationTime(0) {
    DWORD now = GetTickCount();
    this->m_dwEllapsedTime = 0;
    this->m_bUseBitmapBackground = 0;
    this->m_bUseColorBackground = 0;
    this->m_dwStartTime = now;
    this->m_dwExpirationTime = INFINITE;
}

// OFFSET: [#0x0095B430, #0x0095B540]
CLCDPage::~CLCDPage() {
}

// OFFSET: %0x00A724F0
void CLCDPage::OnUpdate(uint32_t timestamp) {
    this->m_dwEllapsedTime = timestamp - this->m_dwStartTime;
    CLCDCollection::OnUpdate(timestamp);
}

// OFFSET: %0x00A72510
void CLCDPage::SetExpiration(DWORD milliseconds) {
    this->m_dwStartTime = GetTickCount();
    this->m_dwEllapsedTime = 0;
    this->m_dwExpirationTime = milliseconds;
}

// OFFSET: %0x00A72540
int32_t CLCDPage::HasExpired() {
    if (this->m_dwStartTime && this->m_dwExpirationTime && this->m_dwEllapsedTime < this->m_dwExpirationTime) {
        return 0;
    }

    return 1;
}

// OFFSET: %0x00A42EC0
void CLCDPage::OnLCDButtonDown(int32_t button) {
}

// OFFSET: %0x00A42EC0
void CLCDPage::OnLCDButtonUp(int32_t button) {
}

// OFFSET: %0x00A72560
void CLCDPage::SetPageBackground(COLORREF color) {
    this->m_crPageBackground = color;
    this->m_bUseColorBackground = 1;
    this->m_bUseBitmapBackground = 0;
}

// OFFSET: #0x0095B580
void CLCDPage::OnDraw(CLCDGfxBase& gfx) {
    if (!this->IsVisible()) {
        return;
    }

    if (this->m_bUseBitmapBackground) {
        this->m_Background.OnDraw(gfx);
    } else if (this->m_bUseColorBackground) {
        HBRUSH brush = CreateSolidBrush(this->m_crPageBackground);
        HGDIOBJ oldBrush = SelectObject(gfx.GetHDC(), brush);
        Rectangle(gfx.GetHDC(), 0, 0, this->GetWidth(), this->GetHeight());
        SelectObject(gfx.GetHDC(), oldBrush);
        DeleteObject(brush);
    }

    for (CLCDBASE_NODE* node = this->m_Objects.Head(); node; node = this->m_Objects.Next(node)) {
        CLCDBase* object = node->m_pObject;

        if (!object->IsVisible()) {
            continue;
        }

        HRGN region = CreateRectRgn(this->GetOrigin().x + object->GetOrigin().x, this->GetOrigin().y + object->GetOrigin().y, this->GetOrigin().x + object->GetOrigin().x + object->GetWidth(), this->GetOrigin().y + object->GetOrigin().y + object->GetHeight());
        SelectClipRgn(gfx.GetHDC(), region);
        DeleteObject(region);

        POINT previousOrigin = { 0, 0 };
        SetViewportOrgEx(gfx.GetHDC(), this->GetOrigin().x + object->GetOrigin().x, this->GetOrigin().y + object->GetOrigin().y, &previousOrigin);
        OffsetViewportOrgEx(gfx.GetHDC(), object->GetLogicalOrigin().x, object->GetLogicalOrigin().y, NULL);
        object->OnDraw(gfx);
        SelectClipRgn(gfx.GetHDC(), NULL);
        SetViewportOrgEx(gfx.GetHDC(), previousOrigin.x, previousOrigin.y, NULL);
        OffsetViewportOrgEx(gfx.GetHDC(), 0, 0, NULL);
    }
}

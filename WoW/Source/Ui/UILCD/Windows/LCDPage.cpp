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

// PROVEN: e14fb471 IDENTICAL/NONE
// OFFSET: 0x00B40200
void CLCDPage::OnUpdate(uint32_t timestamp) {
    this->m_dwEllapsedTime = timestamp - this->m_dwStartTime;
    CLCDCollection::OnUpdate(timestamp);
}

// PROVEN: 773af577 IDENTICAL/NONE
// OFFSET: 0x00B40220/0x14090DF60
void CLCDPage::SetExpiration(DWORD milliseconds) {
    this->m_dwStartTime = GetTickCount();
    this->m_dwEllapsedTime = 0;
    this->m_dwExpirationTime = milliseconds;
}

// PROVEN: 71f67c8b IDENTICAL/NONE
// OFFSET: 0x00B40250/0x14090DF90
int32_t CLCDPage::HasExpired() {
    if (this->m_dwStartTime && this->m_dwExpirationTime && this->m_dwEllapsedTime < this->m_dwExpirationTime) {
        return 0;
    }

    return 1;
}

// PROVEN: 4f88c1c3 IDENTICAL/IDENTICAL
// OFFSET: 0x00847950/0x1407D7E80
void CLCDPage::OnLCDButtonDown(int32_t button) {
}

// PROVEN: a2e85a41 IDENTICAL/IDENTICAL
// OFFSET: 0x00847950/0x1407D7E80
void CLCDPage::OnLCDButtonUp(int32_t button) {
}

// PROVEN: f5e39fac IDENTICAL/NONE
// OFFSET: 0x00B40270/0x14090DFB0
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

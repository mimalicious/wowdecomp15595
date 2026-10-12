#include "WoW/Source/Ui/UILCD/Windows/LCDText.h"
#include <cstring>
#include <cwchar>
#include "Engine/Source/BlizzardCore/SFile2-Core.h"
#include "Engine/Source/Storm/H/storm.h"
#include "WoW/Source/Client.h"
#include "WoW/Source/Ui/UILCD/Windows/LCDGfx.h"

typedef HANDLE (WINAPI* ADDFONTMEMRESOURCEEXPROC)(PVOID fileView, DWORD size, PVOID reserved, DWORD* numFonts);

// OFFSET: 0x00B40DF0/0x14090EDF0
CLCDText::CLCDText() {
    this->m_hFont = NULL;
    this->m_nTextLength = 0;
    this->m_nTextFormat = DT_NOPREFIX;
    this->m_bRecalcExtent = 1;
    this->m_nTextAlignment = 0;
    this->m_dtp.iTabLength = 0;
    this->m_dtp.iLeftMargin = 0;
    this->m_dtp.iRightMargin = 0;
    this->m_dtp.uiLengthDrawn = 0;
    this->m_dtp.cbSize = sizeof(DRAWTEXTPARAMS);
    this->m_sizeVExtent.cx = 0;
    this->m_sizeVExtent.cy = 0;
    this->m_sizeHExtent.cx = 0;
    this->m_sizeHExtent.cy = 0;
    CLCDBase::SetBackgroundMode(TRANSPARENT);
    memset(this->m_szText, 0, sizeof(this->m_szText));
}

// OFFSET: 0x00B41550/0x14090F6A0
CLCDText::~CLCDText() {
    if (this->m_hFont) {
        DeleteObject(this->m_hFont);
        this->m_hFont = NULL;
    }
}

// OFFSET: 0x00B40E90/0x14090EE90
int32_t CLCDText::Initialize() {
    if (this->m_hFont) {
        return S_OK;
    }

    this->SetForegroundColor(RGB(255, 255, 255));
    return this->m_hFont ? S_OK : E_OUTOFMEMORY;
}

// OFFSET: 0x00B40ED0/0x14090EED0
void CLCDText::SetFont(LOGFONTA& font) {
    if (this->m_hFont) {
        DeleteObject(this->m_hFont);
        this->m_hFont = NULL;
    }

    this->m_hFont = CreateFontIndirectA(&font);
    this->m_bRecalcExtent = 1;
}

// OFFSET: 0x00B40F10/0x14090EF30
HFONT CLCDText::GetFont() {
    return this->m_hFont;
}

// OFFSET: 0x00B415A0/0x14090F710
void CLCDText::SetFontFaceName(const char* faceName) {
    if (!faceName) {
        return;
    }

    SStrPrintf(this->m_szFontFaceName, sizeof(this->m_szFontFaceName), faceName);
    LOGFONTA font;
    memset(&font, 0, sizeof(font));
    GetObjectA(this->m_hFont, sizeof(font), &font);
    SStrCopy(font.lfFaceName, faceName, sizeof(font.lfFaceName));
    font.lfCharSet = DEFAULT_CHARSET;
    this->SetFont(font);

    if (this->m_hFont) {
        return;
    }

    char fontFile[64];

    switch (s_locale) {
        case 1:
            SStrCopy(fontFile, "Fonts\\2002.ttf", STORM_MAX_STR);
            SStrCopy(font.lfFaceName, "2002L", sizeof(font.lfFaceName));
            break;
        case 4:
            SStrCopy(fontFile, "Fonts\\ZYHei.ttf", STORM_MAX_STR);
            SStrCopy(font.lfFaceName, "ZYHei_FRIZQT", sizeof(font.lfFaceName));
            break;
        case 5:
            SStrCopy(fontFile, "Fonts\\bHEI00M.ttf", STORM_MAX_STR);
            SStrCopy(font.lfFaceName, "AR Heiti2 Medium B5", sizeof(font.lfFaceName));
            break;
        case 8:
            SStrCopy(fontFile, "Fonts\\NIM_____.ttf", STORM_MAX_STR);
            SStrCopy(font.lfFaceName, "Nimrod MT", sizeof(font.lfFaceName));
            break;
        default:
            SStrCopy(fontFile, "Fonts\\ARIALN.TTF", STORM_MAX_STR);
            SStrCopy(font.lfFaceName, "Arial Narrow", sizeof(font.lfFaceName));
            break;
    }

    void* data;
    uint32_t size;

    if (SFile::LoadFile(fontFile, &data, &size, 0)) {
        DWORD numFonts = 0;
        ADDFONTMEMRESOURCEEXPROC addFontMemResourceEx = reinterpret_cast<ADDFONTMEMRESOURCEEXPROC>(GetProcAddress(LoadLibraryA("gdi32.dll"), "AddFontMemResourceEx"));

        if (addFontMemResourceEx && addFontMemResourceEx(data, size, NULL, &numFonts)) {
            this->SetFont(font);
        }

        SFile::Unload(data);
    }

    if (!this->m_hFont) {
        SStrCopy(font.lfFaceName, "", sizeof(font.lfFaceName));
        font.lfCharSet = DEFAULT_CHARSET;
        this->SetFont(font);
    }
}

// OFFSET: 0x00B40F20/0x14090EF40
void CLCDText::SetFontPointSize(int32_t pointSize) {
    LOGFONTA font;
    memset(&font, 0, sizeof(font));
    GetObjectA(this->m_hFont, sizeof(font), &font);
    font.lfHeight = -MulDiv(pointSize, 96, 72);
    this->SetFont(font);
}

// OFFSET: 0x00B40F80/0x14090EFB0
void CLCDText::SetFontWeight(int32_t weight) {
    LOGFONTA font;
    memset(&font, 0, sizeof(font));
    GetObjectA(this->m_hFont, sizeof(font), &font);
    font.lfWeight = weight;
    this->SetFont(font);
}

// OFFSET: 0x00B40FD0/0x14090F010
void CLCDText::SetFontColor(COLORREF color) {
    this->SetForegroundColor(color);
}

// OFFSET: 0x00B40FE0/0x14090F020
void CLCDText::SetText(const WCHAR* text) {
    if (!text || !wcscmp(this->m_szText, text)) {
        return;
    }

    wcscpy(this->m_szText, text);
    this->m_nTextLength = wcslen(text);
    this->m_dtp.iLeftMargin = 0;
    this->m_dtp.iRightMargin = 0;
    this->m_bRecalcExtent = 1;
}

// OFFSET: [0x005A45C0/0x140215130]
const WCHAR* CLCDText::GetText() {
    return this->m_szText;
}

// OFFSET: 0x00B41080/0x14090F0B0
void CLCDText::SetWordWrap(int32_t enable) {
    this->m_bRecalcExtent = 1;

    if (enable) {
        this->m_nTextFormat |= DT_WORDBREAK;
    } else {
        this->m_nTextFormat &= ~DT_WORDBREAK;
    }
}

// OFFSET: 0x00B410F0/0x14090F110
SIZE& CLCDText::GetVExtent() {
    return this->m_sizeVExtent;
}

// OFFSET: 0x00B41100/0x14090F120
SIZE& CLCDText::GetHExtent() {
    return this->m_sizeHExtent;
}

// OFFSET: 0x00B41110/0x14090F130
void CLCDText::CalculateExtent(int32_t singleLine) {
    HDC dc = CreateCompatibleDC(NULL);
    int oldMapMode = SetMapMode(dc, MM_TEXT);
    int oldBkMode = SetBkMode(dc, this->GetBackgroundMode());
    HGDIOBJ oldFont = SelectObject(dc, this->m_hFont);
    RECT rect = { 0, 0, this->m_Size.cx, this->m_Size.cy };

    if (singleLine) {
        DrawTextExA(dc, reinterpret_cast<LPSTR>(this->m_szText), this->m_nTextLength, &rect, this->m_nTextFormat | DT_CALCRECT | DT_SINGLELINE, &this->m_dtp);
        this->m_sizeHExtent.cx = rect.right;
        this->m_sizeHExtent.cy = rect.bottom;
    } else {
        DrawTextExA(dc, reinterpret_cast<LPSTR>(this->m_szText), this->m_nTextLength, &rect, this->m_nTextFormat | DT_CALCRECT | DT_WORDBREAK, &this->m_dtp);
        this->m_sizeVExtent.cx = rect.right;
        this->m_sizeVExtent.cy = rect.bottom;
    }

    SetMapMode(dc, oldMapMode);
    SetBkMode(dc, oldBkMode);
    SelectObject(dc, oldFont);
    DeleteDC(dc);
}

// OFFSET: 0x00B410B0/0x14090F0D0
void CLCDText::SetLeftMargin(int32_t margin) {
    this->m_dtp.iLeftMargin = margin;
}

// OFFSET: 0x00B410D0/0x14090F0F0
int32_t CLCDText::GetLeftMargin() {
    return this->m_dtp.iLeftMargin;
}

// OFFSET: 0x00B410C0/0x14090F0E0
void CLCDText::SetRightMargin(int32_t margin) {
    this->m_dtp.iRightMargin = margin;
}

// OFFSET: 0x00B410E0/0x14090F100
int32_t CLCDText::GetRightMargin() {
    return this->m_dtp.iRightMargin;
}

// OFFSET: 0x00B41210/0x14090F2A0
void CLCDText::SetAlignment(int32_t alignment) {
    UINT format = this->m_nTextFormat & ~this->m_nTextAlignment;
    this->m_nTextAlignment = alignment;
    this->m_nTextFormat = alignment | format;
}

// OFFSET: 0x00B41240/0x14090F2C0
void CLCDText::DrawControlText(CLCDGfxBase& gfx) {
    RECT rect;
    rect.left = 0;
    rect.top = 0;
    rect.right = this->GetLogicalSize().cx;
    rect.bottom = this->GetLogicalSize().cy;
    DrawTextExW(gfx.GetHDC(), this->m_szText, this->m_nTextLength, &rect, this->m_nTextFormat, &this->m_dtp);

    if (this->m_bInverted) {
        InvertRect(gfx.GetHDC(), &rect);
    }
}

// OFFSET: 0x00B412D0/0x14090F380
void CLCDText::OnDraw(CLCDGfxBase& gfx) {
    if (this->GetBackgroundMode() == OPAQUE) {
        HBRUSH brush = CreateSolidBrush(this->m_crBackgroundColor);
        RECT full = { 0, 0, this->m_Size.cx, this->m_Size.cy };
        FillRect(gfx.GetHDC(), &full, static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH)));
        RECT logical = { 0, 0, this->m_sizeLogical.cx, this->m_sizeLogical.cy };
        FillRect(gfx.GetHDC(), &logical, brush);
        DeleteObject(brush);
    }

    if (!this->m_nTextLength) {
        return;
    }

    int oldMapMode = SetMapMode(gfx.GetHDC(), MM_TEXT);
    int oldBkMode = SetBkMode(gfx.GetHDC(), this->GetBackgroundMode());
    COLORREF oldBkColor = SetBkColor(gfx.GetHDC(), this->m_crBackgroundColor);
    HGDIOBJ oldFont = SelectObject(gfx.GetHDC(), this->m_hFont);
    COLORREF oldTextColor = SetTextColor(gfx.GetHDC(), this->m_crForegroundColor);

    if (this->m_bRecalcExtent) {
        UINT format = this->m_nTextFormat;
        RECT rect = { 0, 0, this->GetWidth(), this->GetHeight() };
        DrawTextExW(gfx.GetHDC(), this->m_szText, this->m_nTextLength, &rect, format | DT_CALCRECT | DT_WORDBREAK, &this->m_dtp);
        this->m_sizeVExtent.cx = rect.right;
        this->m_sizeVExtent.cy = rect.bottom;

        format = this->m_nTextFormat;
        rect.left = 0;
        rect.top = 0;
        rect.right = this->GetWidth();
        rect.bottom = this->GetHeight();
        DrawTextExW(gfx.GetHDC(), this->m_szText, this->m_nTextLength, &rect, format | DT_CALCRECT, &this->m_dtp);
        this->m_sizeHExtent.cx = rect.right;
        this->m_sizeHExtent.cy = rect.bottom;
        this->m_bRecalcExtent = 0;
    }

    if (this->IsVisible()) {
        this->DrawControlText(gfx);
    }

    SetMapMode(gfx.GetHDC(), oldMapMode);
    SetTextColor(gfx.GetHDC(), oldTextColor);
    SetBkMode(gfx.GetHDC(), oldBkMode);
    SetBkColor(gfx.GetHDC(), oldBkColor);
    SelectObject(gfx.GetHDC(), oldFont);
}

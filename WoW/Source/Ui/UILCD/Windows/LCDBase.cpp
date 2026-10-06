#include "WoW/Source/Ui/UILCD/Windows/LCDBase.h"

// OFFSET: #0x0095BC70
CLCDBase::CLCDBase() {
    this->m_Size.cx = 0;
    this->m_Size.cy = 0;
    this->m_Origin.x = 0;
    this->m_Origin.y = 0;
    this->m_bVisible = 1;
    this->m_bInverted = 0;
    this->m_ptLogical.x = 0;
    this->m_ptLogical.y = 0;
    this->m_sizeLogical.cx = 0;
    this->m_sizeLogical.cy = 0;
    this->m_nBkMode = TRANSPARENT;
    this->m_nObjectType = 4;
    this->m_crBackgroundColor = RGB(0, 0, 0);
    this->m_crForegroundColor = RGB(255, 255, 255);
}

// OFFSET: [#0x0095BCC0, #0x0095BE70]
CLCDBase::~CLCDBase() {
}

// OFFSET: %0x00A70D40
int32_t CLCDBase::GetWidth() {
    return this->GetSize().cx;
}

// OFFSET: %0x00A70D50
int32_t CLCDBase::GetHeight() {
    return this->GetSize().cy;
}

// OFFSET: %0x00A72E10
SIZE& CLCDBase::GetSize() {
    return this->m_Size;
}

// OFFSET: %0x00A72EE0
void CLCDBase::SetBackgroundMode(int32_t mode) {
    this->m_nBkMode = mode;
}

// OFFSET: %0x00638880
POINT& CLCDBase::GetOrigin() {
    return this->m_Origin;
}

// OFFSET: %0x00A72D70
void CLCDBase::SetOriginXY(int32_t x, int32_t y) {
    this->m_Origin.x = x;
    this->m_Origin.y = y;
}

// OFFSET: #0x0095BCF0
void CLCDBase::SetOrigin(POINT pt) {
    this->SetOriginXY(pt.x, pt.y);
}

// OFFSET: %0x00A72DB0
void CLCDBase::SetSize(SIZE& size) {
    this->m_Size = size;
    this->SetLogicalSize(this->m_Size);
}

// OFFSET: %0x00A72DE0
void CLCDBase::SetSizeWH(int32_t width, int32_t height) {
    SIZE size;
    size.cx = width;
    size.cy = height;
    this->SetSize(size);
}

// OFFSET: %0x00408910
int32_t CLCDBase::Initialize() {
    return 0;
}

// OFFSET: %0x00401000
void CLCDBase::Shutdown() {
}

// OFFSET: %0x00A72EB0
void CLCDBase::Show(int32_t show) {
    this->m_bVisible = show;
}

// OFFSET: %0x00A72EC0
int32_t CLCDBase::IsVisible() {
    return this->m_bVisible;
}

// OFFSET: %0x00A72ED0
void CLCDBase::Invert(int32_t enable) {
    this->m_bInverted = enable;
}

// OFFSET: %0x00401000
void CLCDBase::ResetUpdate() {
}

// OFFSET: %0x00A72E40
void CLCDBase::SetLogicalOriginXY(int32_t x, int32_t y) {
    this->m_ptLogical.x = x;
    this->m_ptLogical.y = y;
}

// OFFSET: %0x00A72E20
void CLCDBase::SetLogicalOrigin(POINT& pt) {
    this->m_ptLogical = pt;
}

// OFFSET: %0x00A72E60
POINT& CLCDBase::GetLogicalOrigin() {
    return this->m_ptLogical;
}

// OFFSET: %0x00A72E90
void CLCDBase::SetLogicalSizeWH(int32_t width, int32_t height) {
    this->m_sizeLogical.cx = width;
    this->m_sizeLogical.cy = height;
}

// OFFSET: %0x00A72E70
void CLCDBase::SetLogicalSize(SIZE& size) {
    this->m_sizeLogical = size;
}

// OFFSET: #0x009C2590
SIZE& CLCDBase::GetLogicalSize() {
    return this->m_sizeLogical;
}

// OFFSET: %0x00A714F0
int32_t CLCDBase::GetBackgroundMode() {
    return this->m_nBkMode;
}

// OFFSET: %0x00A72EF0
void CLCDBase::SetObjectType(int32_t type) {
    this->m_nObjectType = type;
}

// OFFSET: %0x00973490
void CLCDBase::SetForegroundColor(COLORREF color) {
    this->m_crForegroundColor = color;
}

// OFFSET: %0x00A72F00
void CLCDBase::SetBackgroundColor(COLORREF color) {
    this->m_crBackgroundColor = color;
}

// OFFSET: %0x00A42EC0
void CLCDBase::OnPrepareDraw(CLCDGfxBase& gfx) {
}

// OFFSET: %0x00A42EC0
void CLCDBase::OnDraw(CLCDGfxBase& gfx) {
}

// OFFSET: %0x00A42EC0
void CLCDBase::OnUpdate(uint32_t timestamp) {
}

// OFFSET: %0x00A70D60
void CLCDBase::SetUserData(int32_t data) {
    this->m_nUserData = data;
}

// OFFSET: %0x00A70D70
int32_t CLCDBase::GetUserData() {
    return this->m_nUserData;
}

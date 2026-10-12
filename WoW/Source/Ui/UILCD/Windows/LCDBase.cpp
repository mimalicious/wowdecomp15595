#include "WoW/Source/Ui/UILCD/Windows/LCDBase.h"

// OFFSET: 0x00B40A20/0x14090E9C0
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

// OFFSET: [0x00B40A70/0x14090EA10, 0x00B40C10/0x14090EB40]
CLCDBase::~CLCDBase() {
}

// OFFSET: 0x00B3EA70/0x14090C200
int32_t CLCDBase::GetWidth() {
    return this->GetSize().cx;
}

// OFFSET: 0x00B3EA80/0x14090C220
int32_t CLCDBase::GetHeight() {
    return this->GetSize().cy;
}

// OFFSET: 0x00B40B00/0x1408F5FA0
SIZE& CLCDBase::GetSize() {
    return this->m_Size;
}

// OFFSET: 0x00B40BE0/0x14090EB10
void CLCDBase::SetBackgroundMode(int32_t mode) {
    this->m_nBkMode = mode;
}

// OFFSET: [0x00903DD0/0x1408F59C0]
POINT& CLCDBase::GetOrigin() {
    return this->m_Origin;
}

// OFFSET: 0x005DC200/0x1407E5680
void CLCDBase::SetOriginXY(int32_t x, int32_t y) {
    this->m_Origin.x = x;
    this->m_Origin.y = y;
}

// OFFSET: 0x00B40A80/0x14090EA20
void CLCDBase::SetOrigin(POINT pt) {
    this->SetOriginXY(pt.x, pt.y);
}

// OFFSET: 0x00B40AA0/0x14090EA40
void CLCDBase::SetSize(SIZE& size) {
    this->m_Size = size;
    this->SetLogicalSize(this->m_Size);
}

// OFFSET: 0x00B40AD0/0x14090EA60
void CLCDBase::SetSizeWH(int32_t width, int32_t height) {
    SIZE size;
    size.cx = width;
    size.cy = height;
    this->SetSize(size);
}

// OFFSET: 0x00842700/0x1407FDD80
int32_t CLCDBase::Initialize() {
    return 0;
}

// OFFSET: 0x00401000/0x1407D7E80
void CLCDBase::Shutdown() {
}

// OFFSET: 0x00B40BB0/0x14090EAE0
void CLCDBase::Show(int32_t show) {
    this->m_bVisible = show;
}

// OFFSET: 0x00B40BC0/0x14090EAF0
int32_t CLCDBase::IsVisible() {
    return this->m_bVisible;
}

// OFFSET: 0x00B40BD0/0x14090EB00
void CLCDBase::Invert(int32_t enable) {
    this->m_bInverted = enable;
}

// OFFSET: 0x00401000/0x1407D7E80
void CLCDBase::ResetUpdate() {
}

// OFFSET: 0x00B40B30/0x14090EA90
void CLCDBase::SetLogicalOriginXY(int32_t x, int32_t y) {
    this->m_ptLogical.x = x;
    this->m_ptLogical.y = y;
}

// OFFSET: 0x00B40B10/0x14090EA80
void CLCDBase::SetLogicalOrigin(POINT& pt) {
    this->m_ptLogical = pt;
}

// OFFSET: 0x00B40B50/0x14090EAA0
POINT& CLCDBase::GetLogicalOrigin() {
    return this->m_ptLogical;
}

// OFFSET: 0x00B40B80/0x14090EAC0
void CLCDBase::SetLogicalSizeWH(int32_t width, int32_t height) {
    this->m_sizeLogical.cx = width;
    this->m_sizeLogical.cy = height;
}

// OFFSET: 0x00B40B60/0x14090EAB0
void CLCDBase::SetLogicalSize(SIZE& size) {
    this->m_sizeLogical = size;
}

// OFFSET: [0x00B40BA0/0x14056DDC0]
SIZE& CLCDBase::GetLogicalSize() {
    return this->m_sizeLogical;
}

// OFFSET: 0x00B3F210/0x14050FC70
int32_t CLCDBase::GetBackgroundMode() {
    return this->m_nBkMode;
}

// OFFSET: 0x00B40BF0/0x14090EB20
void CLCDBase::SetObjectType(int32_t type) {
    this->m_nObjectType = type;
}

// OFFSET: 0x00A32F60/0x14090EB30
void CLCDBase::SetForegroundColor(COLORREF color) {
    this->m_crForegroundColor = color;
}

// OFFSET: 0x00B40C00/0x1404BE2F0
void CLCDBase::SetBackgroundColor(COLORREF color) {
    this->m_crBackgroundColor = color;
}

// OFFSET: 0x00847950/0x1407D7E80
void CLCDBase::OnPrepareDraw(CLCDGfxBase& gfx) {
}

// OFFSET: 0x00847950/0x1407D7E80
void CLCDBase::OnDraw(CLCDGfxBase& gfx) {
}

// OFFSET: 0x00847950/0x1407D7E80
void CLCDBase::OnUpdate(uint32_t timestamp) {
}

// OFFSET: 0x00B3EA90/0x14090C240
void CLCDBase::SetUserData(int32_t data) {
    this->m_nUserData = data;
}

// OFFSET: 0x00B3EAA0/0x140902360
int32_t CLCDBase::GetUserData() {
    return this->m_nUserData;
}

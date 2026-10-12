#include "WoW/Source/Ui/UILCD/Windows/LCDCollection.h"
#include "WoW/Source/Ui/UILCD/Windows/LCDGfx.h"

// PROVEN: 33719c71 UNPROVEN/IDENTICAL
// OFFSET: 0x00B40820/0x14090E700
CLCDCollection::CLCDCollection() {
}

// OFFSET: [0x00B40950/0x14090E8A0, 0x00B409F0/0x14090E970]
CLCDCollection::~CLCDCollection() {
    for (CLCDBASE_NODE* node = this->m_Objects.Head(); node; node = this->m_Objects.Next(node)) {
        delete node->m_pObject;
        node->m_pObject = NULL;
    }

    this->m_Objects.Clear();
}

// PROVEN: 5a6d4992 IDENTICAL/DIFFERENT
// OFFSET: 0x00B40850/0x14090E740
bool CLCDCollection::AddObject(CLCDBase* object) {
    CLCDBASE_NODE* node = this->m_Objects.NewNode(2, 0, 0);
    node->m_pObject = object;
    return true;
}

// OFFSET: 0x00B405D0/0x14090E3F0
void CLCDCollection::OnDraw(CLCDGfxBase& gfx) {
    if (!this->IsVisible()) {
        return;
    }

    for (CLCDBASE_NODE* node = this->m_Objects.Head(); node; node = this->m_Objects.Next(node)) {
        CLCDBase* object = node->m_pObject;

        if (!object->IsVisible()) {
            continue;
        }

        object->OnPrepareDraw(gfx);
        HRGN region = CreateRectRgn(object->GetOrigin().x, object->GetOrigin().y, object->GetOrigin().x + object->GetWidth(), object->GetOrigin().y + object->GetHeight());
        SelectClipRgn(gfx.GetHDC(), region);
        DeleteObject(region);

        POINT previousOrigin = { 0, 0 };
        SetViewportOrgEx(gfx.GetHDC(), object->GetOrigin().x, object->GetOrigin().y, &previousOrigin);
        OffsetViewportOrgEx(gfx.GetHDC(), object->GetLogicalOrigin().x, object->GetLogicalOrigin().y, NULL);
        object->OnDraw(gfx);
        SelectClipRgn(gfx.GetHDC(), NULL);
        SetViewportOrgEx(gfx.GetHDC(), previousOrigin.x, previousOrigin.y, NULL);
        OffsetViewportOrgEx(gfx.GetHDC(), 0, 0, NULL);
    }
}

// OFFSET: 0x00B40770/0x14090E610
void CLCDCollection::OnUpdate(uint32_t timestamp) {
    for (CLCDBASE_NODE* node = this->m_Objects.Head(); node; node = this->m_Objects.Next(node)) {
        node->m_pObject->OnUpdate(timestamp);
    }

    CLCDBase::OnUpdate(timestamp);
}

// OFFSET: 0x00B407C0/0x14090E670
void CLCDCollection::ShowSubscreen(int32_t subscreen) {
    for (CLCDBASE_NODE* node = this->m_Objects.Head(); node; node = this->m_Objects.Next(node)) {
        CLCDBase* object = node->m_pObject;

        if (object->GetUserData() != -1) {
            object->Show(object->GetUserData() == subscreen);
        }
    }
}

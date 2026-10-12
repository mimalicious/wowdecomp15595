#include <cstddef>
#include <cstdint>
#include "Engine/Source/Frame/CSimpleFontable.h"
#include "Engine/Source/Frame/CSimpleFont.h"

// OFFSET: [0x004FB490/0x14013F520, 0x004FB4F0/0x14013F5A0]
CSimpleFontable::~CSimpleFontable() {
    this->SetFontObject(NULL);
}

// OFFSET: 0x004FB3E0/0x14013F2E0
void CSimpleFontable::SetFontObject(CSimpleFont* font) {
    if (this->m_fontObject == font) {
        return;
    }

    if (this->m_fontObject) {
        this->m_fontObject->RemoveFontable(this);
    }

    this->m_fontObject = font;

    if (font) {
        font->m_fontableList.LinkToTail(this);
        font->m_attributes.m_flags |= 0x1F;
        this->FontObjectUpdated(font->m_attributes);
        font->m_attributes.m_flags &= ~0x1Fu;
    }
}

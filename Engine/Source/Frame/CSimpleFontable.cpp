#include <cstddef>
#include <cstdint>
#include "Engine/Source/Frame/CSimpleFontable.h"
#include "Engine/Source/Frame/CSimpleFont.h"

// INLINED
CSimpleFontable::CSimpleFontable() {
    this->m_fontableFlags = 0x1F;
    this->m_fontObject = NULL;
}

// OFFSET: [%0x0050BD40, %0x0050BDA0]
CSimpleFontable::~CSimpleFontable() {
    this->SetFontObject(NULL);
}

// OFFSET: %0x0050BCF0
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

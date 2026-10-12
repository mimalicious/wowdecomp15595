#include <cstddef>
#include <cstdint>
#include <new>
#include <typeinfo>
#include "Engine/Source/Frame/CSimpleCheckbox.h"
#include "Engine/Source/Frame/CScriptObject.h"
#include "Engine/Source/Frame/CScriptRegion.h"
#include "Engine/Source/Frame/CSimpleButton.h"
#include "Engine/Source/Frame/CSimpleFrame.h"
#include "Engine/Source/Frame/CSimpleTexture.h"
#include "Engine/Source/FrameXML/LoadXML.h"
#include "Engine/Source/FrameXML/BinaryXML.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/lua-5.1/src/lauxlib.h"

// STATIC: 0x00CA3DFC/0x140B415C0
int32_t CSimpleCheckbox::s_metatable = LUA_NOREF;

// STATIC: 0x00DD39E0/0x140CB96E0
int32_t CSimpleCheckbox::s_objectType;

// STATIC: 0x00CA3348/0x140B404A0
const char* CSimpleCheckbox::s_objectTypeName = "CheckButton";

// STATIC: 0x00DD39E4/0x140CB96E8
CDataAllocator CSimpleCheckbox::s_allocator(sizeof(CSimpleCheckbox), 50);

// INLINED
void CSimpleCheckbox::operator delete(void* ptr) {
    CSimpleCheckbox::s_allocator.PutData(ptr, NULL, 0);
}

// OFFSET: 0x00515C10/0x1401602A0
CSimpleCheckbox::CSimpleCheckbox(CSimpleFrame* parent) : CSimpleButton(parent) {
    this->m_checked = 0;
    this->m_checkedTexture = NULL;
    this->m_disabledCheckedTexture = NULL;
}

// OFFSET: 0x00515C50/0x1401602F0
const char* CSimpleCheckbox::GetObjectTypeName() {
    return CSimpleCheckbox::s_objectTypeName;
}

// OFFSET: 0x00515C60/0x140160300
int32_t CSimpleCheckbox::GetScriptMetaTable() {
    return CSimpleCheckbox::s_metatable;
}

// OFFSET: 0x00515C70/0x14015EC70
void CSimpleCheckbox::UnregisterRegion(CSimpleRegion* region) {
    if (region == this->m_checkedTexture) {
        this->m_checkedTexture = NULL;
    }

    if (region == this->m_disabledCheckedTexture) {
        this->m_disabledCheckedTexture = NULL;
    }

    CSimpleButton::UnregisterRegion(region);
}

// INLINED
CSimpleCheckbox::~CSimpleCheckbox() {
}

// INLINED
bool CSimpleCheckbox::IsA(int32_t type) {
    return type == CSimpleCheckbox::s_objectType || type == CSimpleButton::s_objectType || type == CSimpleFrame::s_objectType || type == CScriptRegion::s_objectType || type == CScriptObject::s_objectType;
}

// INLINED
bool CSimpleCheckbox::IsA(const char* typeName) {
    return !SStrCmpI(typeName, CSimpleCheckbox::s_objectTypeName, STORM_MAX_STR) || !SStrCmpI(typeName, CSimpleButton::s_objectTypeName, STORM_MAX_STR) || CSimpleFrame::IsA(typeName);
}

// OFFSET: 0x00515D80/0x140160430
void CSimpleCheckbox::SetChecked(int32_t checked, int32_t force) {
    if (!force && checked == this->m_checked) {
        return;
    }

    this->m_checked = checked;

    if (this->m_checkedTexture) {
        this->m_checkedTexture->Hide();
    }

    if (this->m_disabledCheckedTexture) {
        this->m_disabledCheckedTexture->Hide();
    }

    if (!this->m_checked) {
        return;
    }

    CSimpleTexture* texture;

    if (this->m_disabledCheckedTexture && this->m_state == BUTTONSTATE_DISABLED) {
        texture = this->m_disabledCheckedTexture;
    } else {
        texture = this->m_checkedTexture;

        if (!texture) {
            return;
        }
    }

    texture->Show();
}

// OFFSET: 0x00515E30/0x1401604F0
void CSimpleCheckbox::OnClick(const char* btn, int32_t down) {
    this->SetChecked(this->m_checked == 0, 0);
    CSimpleButton::OnClick(btn, down);
}

// OFFSET: 0x00515E60/0x140160540
void CSimpleCheckbox::SetCheckedTexture(CSimpleTexture* texture) {
    if (texture == this->m_checkedTexture) {
        return;
    }

    if (this->m_checkedTexture) {
        delete this->m_checkedTexture;
    }

    if (texture) {
        texture->SetFrame(this, 3, 0, false);
    }

    this->m_checkedTexture = texture;
    this->SetChecked(this->m_checked, 1);
}

// OFFSET: 0x00515EB0/0x1401605B0
void CSimpleCheckbox::SetDisabledCheckedTexture(CSimpleTexture* texture) {
    if (texture == this->m_disabledCheckedTexture) {
        return;
    }

    if (this->m_disabledCheckedTexture) {
        delete this->m_disabledCheckedTexture;
    }

    if (texture) {
        texture->SetFrame(this, 3, 0, false);
    }

    this->m_disabledCheckedTexture = texture;
    this->SetChecked(this->m_checked, 1);
}

// OFFSET: 0x00515F00/0x140160620
void CSimpleCheckbox::Enable(int32_t enabled) {
    CSimpleButton::Enable(enabled);
    this->SetChecked(this->m_checked, 1);
}

// OFFSET: 0x00515F30/0x140160650
void CSimpleCheckbox::LoadXML(linkObject* node, CStatus* status) {
    CSimpleButton::LoadXML(node, status);

    if (!(node->m_flags & 0x2)) {
        return;
    }

    CheckButton* checkButton = static_cast<CheckButton*>(node);

    if (checkButton->m_checkFlags & 0x1) {
        this->SetChecked((checkButton->m_checkFlags >> 1) & 1, 0);
    }

    for (linkObject* child = node->m_child; child; child = child->m_next) {
        if (child->m_element == XML_CHECKEDTEXTURE) {
            this->SetCheckedTexture(LoadXML_Texture(child, this, status));
        } else if (child->m_element == XML_DISABLEDCHECKEDTEXTURE) {
            this->SetDisabledCheckedTexture(LoadXML_Texture(child, this, status));
        }
    }
}

// OFFSET: 0x00515FC0/0x140160710
int32_t CSimpleCheckbox::SetCheckedTexture(const char* fileName) {
    if (this->m_checkedTexture) {
        this->m_checkedTexture->SetTexture(fileName, 0, 0, CSimpleTexture::s_textureFilterMode, ImageMode_Normal);
        return 1;
    }

    void* data = CSimpleTexture::s_allocator.GetData(0, typeid(CSimpleTexture).raw_name(), SLOG_OBJECT);
    CSimpleTexture* texture = data ? new (data) CSimpleTexture(NULL, 2, 0, true) : NULL;

    if (!texture->SetTexture(fileName, 0, 0, CSimpleTexture::s_textureFilterMode, ImageMode_Normal)) {
        if (texture) {
            delete texture;
        }

        return 0;
    }

    texture->SetAllPoints(this, 1);
    texture->SetBlendMode(GxBlend_Add);
    this->SetCheckedTexture(texture);
    return 1;
}

// OFFSET: 0x00516080/0x140160830
int32_t CSimpleCheckbox::SetDisabledCheckedTexture(const char* fileName) {
    if (this->m_disabledCheckedTexture) {
        this->m_disabledCheckedTexture->SetTexture(fileName, 0, 0, CSimpleTexture::s_textureFilterMode, ImageMode_Normal);
        return 1;
    }

    void* data = CSimpleTexture::s_allocator.GetData(0, typeid(CSimpleTexture).raw_name(), SLOG_OBJECT);
    CSimpleTexture* texture = data ? new (data) CSimpleTexture(NULL, 2, 0, true) : NULL;

    if (!texture->SetTexture(fileName, 0, 0, CSimpleTexture::s_textureFilterMode, ImageMode_Normal)) {
        if (texture) {
            delete texture;
        }

        return 0;
    }

    texture->SetAllPoints(this, 1);
    texture->SetBlendMode(GxBlend_Add);
    this->SetDisabledCheckedTexture(texture);
    return 1;
}

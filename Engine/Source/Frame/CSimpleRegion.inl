inline CGradient*& CSimpleRegion::Gradient() {
    return *reinterpret_cast<CGradient**>(&this->m_color);
}

// PROVEN: 5f8a6138 IDENTICAL/NONE
// OFFSET: 0x00465870
inline void CSimpleRegion::Hide() {
    this->m_shown = 0;
    this->HideThis();
}

// PROVEN: 3dac64c9 IDENTICAL/NONE
// OFFSET: 0x00465890
inline void CSimpleRegion::Show() {
    this->m_shown = 1;
    this->ShowThis(false);
}

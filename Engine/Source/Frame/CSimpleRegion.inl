inline CGradient*& CSimpleRegion::Gradient() {
    return *reinterpret_cast<CGradient**>(&this->m_color);
}

// OFFSET: %0x0046ECA0
inline void CSimpleRegion::Hide() {
    this->m_shown = 0;
    this->HideThis();
}

// OFFSET: %0x0046ECC0
inline void CSimpleRegion::Show() {
    this->m_shown = 1;
    this->ShowThis(false);
}

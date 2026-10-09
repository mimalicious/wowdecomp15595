// INLINED
inline CameraCVarRef::CameraCVarRef() {
    this->m_next = CameraCVarRef::s_head;
    CameraCVarRef::s_count++;
    CameraCVarRef::s_head = this;
}

// INLINED
inline CameraCVarRef& CameraCVarRef::operator=(CVar* cvar) {
    this->m_cvar = cvar;
    return *this;
}

// INLINED
inline CVar* CameraCVarRef::operator->() const {
    return this->m_cvar;
}

// INLINED
inline CameraCVarRef::operator CVar*() const {
    return this->m_cvar;
}

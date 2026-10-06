// INLINED
inline CVarArrayWrapper::CVarArrayWrapper(CVar* cvar) {
    this->m_cvar = cvar;
    this->m_defaultValue = 0;
    uint8_t version = static_cast<uint8_t>(cvar->m_stringValue.m_str[0]);

    if (version != 2) {
        this->UpdateFromVersion(version);
    }
}

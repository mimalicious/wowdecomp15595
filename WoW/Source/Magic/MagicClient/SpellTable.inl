// INLINED
inline bool HASHKEY_SKILLLINE::operator==(const HASHKEY_SKILLLINE& key) const {
    return this->m_race == key.m_race && this->m_class == key.m_class && this->m_spellId == key.m_spellId;
}

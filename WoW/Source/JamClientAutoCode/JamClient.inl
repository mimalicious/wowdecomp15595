// INLINED
inline JamGuildNewsEvent::JamGuildNewsEvent(const JamGuildNewsEvent& other) {
    *this = other;
}

// INLINED
inline JamLFGuildSettings& JamLFGuildSettings::operator=(const JamLFGuildSettings& source) {
    this->m_availability = source.m_availability;
    this->m_classRoles = source.m_classRoles;
    this->m_level = source.m_level;
    this->m_interests = source.m_interests;
    this->unk_0010 = source.unk_0010;
    SStrCopy(this->m_comment, source.m_comment, sizeof(this->m_comment));
    return *this;
}

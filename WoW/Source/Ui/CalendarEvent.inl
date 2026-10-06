// INLINED
inline EventInviteInfo::EventInviteInfo() {
    this->m_inviteID = 0;
    this->m_guid = WOWGUID();
    this->m_responseTime = 0;
    this->m_level = 0;
    this->m_status = 0;
    this->m_moderatorStatus = 0;
    this->m_inviteType = 0;
    this->m_notes[0] = '\0';
}

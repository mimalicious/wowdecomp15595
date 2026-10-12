// INLINED
inline LFGListPlayer::LFGListPlayer() : m_index(-1), m_partyIndex(-1), m_party(NULL), m_leader(0) {
}

// INLINED
inline LFGListParty::LFGListParty() : m_index(-1) {
}

// INLINED
inline uint32_t LFGListParty::GetNumMembers() const {
    uint32_t count = this->m_members.m_count;
    return count <= 1 ? 0 : count - 1;
}

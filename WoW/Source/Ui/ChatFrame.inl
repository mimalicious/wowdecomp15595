// INLINED
inline PENDINGCHAT::PENDINGCHAT() : m_text(NULL), m_senderName(NULL), m_conversation(NULL) {
}

// INLINED
inline PENDINGTEXTEMOTE::PENDINGTEXTEMOTE() : m_target(NULL) {
}

// INLINED
inline bool HASHKEY_LANGUAGE::operator==(const HASHKEY_LANGUAGE& key) const {
    return this->m_length == key.m_length && this->m_unk04 == key.m_unk04;
}

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

// INLINED
inline ChatMessageParams::ChatMessageParams() {
    this->m_senderName = NULL;
    this->m_language = 0;
    this->m_channelName = NULL;
    this->m_addonPrefix = NULL;
    this->m_targetName = NULL;
    this->m_flags = NULL;
    this->m_senderGuid.m_guid = 0;
    this->m_senderUnk = 0;
    this->m_targetGuid.m_guid = 0;
    this->m_achievementID = 0;
    this->m_isGM = false;
    this->m_conversation = NULL;
    this->m_mobile = false;
    this->m_playSound = true;
    this->m_hideChatLog = false;
    this->m_displayTime = 0.0f;
}

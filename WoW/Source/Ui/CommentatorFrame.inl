// INLINED
inline CommentatorInstanceKey::CommentatorInstanceKey() {
    this->unk_0000 = 0;
    this->unk_0004 = 15;
    this->unk_0008 = 0;
}

// INLINED
inline void CGCommentator::SetMoveSpeed(float speed) {
    if (speed >= 40.0f) {
        this->m_moveSpeed = 40.0f;
    } else if (speed <= 0.0f) {
        this->m_moveSpeed = 0.0f;
    } else {
        this->m_moveSpeed = speed;
    }
}

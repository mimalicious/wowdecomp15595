#ifndef WOW_SOURCE_GAME_SHIPPATH_INL
#define WOW_SOURCE_GAME_SHIPPATH_INL

// INLINED
inline ShipPath::ShipPath() {
    this->m_physics = NULL;
    this->m_taxiPathId = 0xFFFFFFFF;
    this->m_moveSpeed = 0.0f;
    this->m_totalPathTime = 0;
    this->m_accelRate = 0.0f;
    this->m_waitTime = 0;
    this->m_timeOffset = 0;
    this->m_waiting = 0;
    this->m_waitReached = 0;
    this->unk_003E = 0;
}

#endif

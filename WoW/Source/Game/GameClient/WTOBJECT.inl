#ifndef WOW_SOURCE_GAME_GAMECLIENT_WTOBJECT_INL
#define WOW_SOURCE_GAME_GAMECLIENT_WTOBJECT_INL

// INLINED
inline SWING::SWING() {
    for (int32_t i = 0; i < 128; i++) {
        this->m_points[i].x = 0.0f;
        this->m_points[i].y = 0.0f;
        this->m_points[i].z = 0.0f;
    }

    this->m_tail = 0;
    this->m_head = 0;
    *reinterpret_cast<uint32_t*>(&this->m_color) = 0;
}

#endif

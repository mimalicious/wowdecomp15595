#ifndef WOW_SOURCE_WORLDCLIENT_WORLD_INL
#define WOW_SOURCE_WORLDCLIENT_WORLD_INL

// INLINED
inline World::MinimapQuad::MinimapQuad() {
    this->m_x = 0;
    this->m_y = 0;
    this->m_box.b.x = 0.0f;
    this->m_box.b.y = 0.0f;
    this->m_box.b.z = 0.0f;
    this->m_box.t.x = 0.0f;
    this->m_box.t.y = 0.0f;
    this->m_box.t.z = 0.0f;
}

#endif

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

// INLINED
inline World::HitTestDebugItem::HitTestDebugItem() {
    this->m_position.x = 0.0f;
    this->m_position.y = 0.0f;
    this->m_position.z = 0.0f;

    for (uint32_t i = 0; i < 6; i++) {
        this->unk_0014[i] = 0;
    }

    this->m_box.b.x = 0.0f;
    this->m_box.b.y = 0.0f;
    this->m_box.b.z = 0.0f;
    this->m_box.t.x = 0.0f;
    this->m_box.t.y = 0.0f;
    this->m_box.t.z = 0.0f;
    this->m_center.x = 0.0f;
    this->m_center.y = 0.0f;
    this->m_center.z = 0.0f;
    this->m_radius = 0.0f;
}

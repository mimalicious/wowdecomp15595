#include "WoW/Source/Object/CMovementStatus.h"

// OFFSET: 0x00698280/0x1403439D0
CMovementStatus::CMovementStatus() {
    this->m_position.x = 0.0f;
    this->m_position.y = 0.0f;
    this->m_position.z = 0.0f;
    this->m_hasTransport = false;
    this->m_transport.m_position.x = 0.0f;
    this->m_transport.m_position.y = 0.0f;
    this->m_transport.m_position.z = 0.0f;
    this->m_transport.m_hasTime2 = false;
    this->m_transport.m_hasTime3 = false;
    this->m_hasFall = false;
    this->m_fall.m_hasDirection = false;
    this->m_fall.m_direction.m_sinAngle = 0.0f;
    this->m_fall.m_direction.m_cosAngle = 0.0f;
    this->m_moveFlags = 0;
    this->m_moveExtraFlags = 0;
    this->m_gameTime = 0;
    this->m_facing = 0.0f;
    this->m_pitch = 0.0f;
    this->m_splineElevation = 0.0f;
    this->m_hasSpline = false;
    this->m_heightChangeFailed = false;
}

#include <cfloat>

// INLINED
inline DayNight::CZoneLight::CZoneLight() {
    this->m_id = 0;
    this->m_pointCount = 0;
    this->m_min.x = FLT_MAX;
    this->m_min.y = FLT_MAX;
    this->m_max.x = -FLT_MAX;
    this->m_max.y = -FLT_MAX;
}

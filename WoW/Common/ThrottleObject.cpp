#include "WoW/Common/ThrottleObject.h"
#include "Engine/Source/Os/TimeManager.h"

// PROVEN: fb88bf89 IDENTICAL/NONE
// OFFSET: 0x00A30350
void ThrottleObject::Restore(const ThrottleObject& saved) {
    uint32_t now = OsGetAsyncTimeMs();
    this->m_limit = saved.m_limit;
    this->m_period = saved.m_period;
    this->m_count = saved.m_count;
    this->m_startTime = now - saved.m_startTime;
}

// PROVEN: dd089d7b IDENTICAL/NONE
// OFFSET: 0x00A30380/0x1407DA290
bool ThrottleObject::IncrementAndCheck() {
    uint32_t now = OsGetAsyncTimeMs();
    this->m_count++;

    if (this->m_count > this->m_limit && (now - this->m_startTime) / 1000 >= this->m_period) {
        this->m_startTime = now;
        this->m_count = 1;
    }

    return this->m_count <= this->m_limit;
}

// OFFSET: 0x00A303C0/0x1407DA2E0
bool ThrottleObject::Check() const {
    uint32_t now = OsGetAsyncTimeMs();
    uint32_t count = this->m_count + 1;

    if (count > this->m_limit && (now - this->m_startTime) / 1000 >= this->m_period) {
        count = 1;
    }

    return count <= this->m_limit;
}

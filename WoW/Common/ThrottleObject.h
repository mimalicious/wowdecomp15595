#ifndef WOW_COMMON_THROTTLEOBJECT_H
#define WOW_COMMON_THROTTLEOBJECT_H

#include <cstdint>

class ThrottleObject {
    public:
    /* 0000 */ uint32_t m_limit;
    /* 0004 */ uint32_t m_period;
    /* 0008 */ uint32_t m_count;
    /* 000C */ uint32_t m_startTime;

    bool Check() const;
    bool IncrementAndCheck();
    void Restore(const ThrottleObject& saved);
};

#endif

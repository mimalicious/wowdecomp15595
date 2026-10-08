// INLINED
inline System_Time::TimeInfo* System_Time::GetTimeInfo() {
    if (!s_timeInfo) {
        void* memory = SMemAlloc(sizeof(TimeInfo), __FILE__, __LINE__, 0);
        s_timeInfo = memory ? new (memory) TimeInfo() : NULL;
    }

    return s_timeInfo;
}

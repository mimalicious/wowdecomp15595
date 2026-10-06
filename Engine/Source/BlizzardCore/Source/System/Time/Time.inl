// INLINED
inline System_Time::TimeInfo* System_Time::GetTimeInfo() {
    if (!s_timeInfo) {
        void* memory = malloc(sizeof(TimeInfo));
        s_timeInfo = memory ? new (memory) TimeInfo() : NULL;
    }

    return s_timeInfo;
}

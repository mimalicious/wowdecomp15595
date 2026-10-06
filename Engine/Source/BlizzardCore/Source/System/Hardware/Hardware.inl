#ifndef ENGINE_SOURCE_BLIZZARDCORE_SOURCE_SYSTEM_HARDWARE_HARDWARE_INL
#define ENGINE_SOURCE_BLIZZARDCORE_SOURCE_SYSTEM_HARDWARE_HARDWARE_INL

// INLINED
inline uint32_t Blizzard::Hardware::GetCoreCount() {
    GetProcessorFeatures();
    System_Hardware::ReadLogicalProcessorInformation();
    return System_Hardware::s_processorFeatures.m_coreCount;
}

// INLINED
inline uint32_t Blizzard::Hardware::GetVendor() {
    GetProcessorFeatures();
    return System_Hardware::s_processorFeatures.m_vendor;
}

#endif

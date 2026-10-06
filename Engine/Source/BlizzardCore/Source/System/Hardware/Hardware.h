#ifndef ENGINE_SOURCE_BLIZZARDCORE_SOURCE_SYSTEM_HARDWARE_HARDWARE_H
#define ENGINE_SOURCE_BLIZZARDCORE_SOURCE_SYSTEM_HARDWARE_HARDWARE_H

#include <cstdint>

namespace Blizzard {
namespace Hardware {

struct ProcessorFeatures {
    /* 0000 */ uint32_t m_vendor;
    /* 0004 */ uint32_t m_features;
    /* 0008 */ uint32_t m_coreCount;
    /* 000C */ uint32_t m_processorCount;
};

struct Fingerprint {
    /* 0000 */ uint8_t m_data[16];
};

bool ExtendedProcessorFeatures(ProcessorFeatures& features);
uint32_t GetCoreCount();
bool GetFingerprint(Fingerprint* fingerprint);
uint32_t GetProcessorFeatures();
uint32_t GetVendor();

}
}

namespace System_Hardware {

struct CPUIDInfo {
    /* 0000 */ char m_vendor[13];
    /* 0010 */ uint32_t m_maxLeaf;
    /* 0014 */ uint32_t m_leaf1Ebx;
    /* 0018 */ uint32_t m_leaf1Edx;
    /* 001C */ uint32_t m_leaf4Eax;
    /* 0020 */ uint32_t m_maxExtLeaf;
    /* 0024 */ uint32_t m_ext1Ecx;
    /* 0028 */ uint32_t m_ext1Edx;
    /* 002C */ uint32_t m_ext4Eax;
    /* 0030 */ uint32_t m_ext8Ecx;
};

extern Blizzard::Hardware::ProcessorFeatures s_processorFeatures;
extern Blizzard::Hardware::ProcessorFeatures s_extendedFeatures;

void CalcFingerprint(uint8_t* fingerprint);
uint32_t CountSetBits(uint32_t mask);
uint64_t GetDiskCapacity();
void GetMACAddress(uint8_t* address);
bool ReadCPUID(CPUIDInfo* info);
bool ReadLogicalProcessorInformation();

}

#include "Engine/Source/BlizzardCore/Source/System/Hardware/Hardware.inl"

#endif

#ifndef WOW_SOURCE_OBJECT_AREATRIGGER_H
#define WOW_SOURCE_OBJECT_AREATRIGGER_H

#include <cstddef>
#include <cstdint>

class CGAreaTriggerData;

class CGAreaTrigger {
    public:
    static uint32_t DescriptorToMirrorOffset(uint32_t offset, uint32_t size, int32_t local, uint32_t base);
    static int32_t TotalFields();

    /* 0000 */ CGAreaTriggerData* m_areaTrigger;
    /* 0004 */ void* m_areaTriggerMirror;
};

class CGAreaTriggerData {
    public:
    /* 0000 */ uint32_t m_areatriggerSpellid;
    /* 0004 */ uint32_t m_areatriggerSpellvisualid;
    /* 0008 */ uint32_t m_areatriggerDuration;
    /* 000C */ float m_areatriggerFinalPos [3];
};

#endif

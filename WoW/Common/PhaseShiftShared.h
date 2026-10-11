#ifndef WOW_COMMON_PHASESHIFTSHARED_H
#define WOW_COMMON_PHASESHIFTSHARED_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Storm/H/stpl.h"

class CDataStore;
class ClientPhaseShiftChange;

class CPhaseShift {
    public:
    /* 0000 */ uint32_t m_flags;
    /* 0004 */ TSCGrowableArray<uint16_t, 4> m_phases;

    CPhaseShift();
    explicit CPhaseShift(uint32_t flags);
    CPhaseShift& operator=(const CPhaseShift& source);
    int32_t operator==(const CPhaseShift& other) const;
    int32_t CalcNormalPhase() const;
    int32_t HasSharedPhase(const CPhaseShift* other) const;
    void SetPhase(uint32_t flags, uint16_t phase);
    void SetPhasesFromDataStore(CDataStore* msg);
    void SetPhasesFromMessage(ClientPhaseShiftChange* msg);
    void SetPhasesFromDB(uint32_t flags, uint32_t phaseID, uint32_t phaseGroupID);
};

int32_t PhaseShift_CompareSortedPhaseArray(uint32_t flagsA, const uint16_t* phasesA, int32_t countA, uint32_t flagsB, const uint16_t* phasesB, int32_t countB);

#endif

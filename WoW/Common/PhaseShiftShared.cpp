#include <cstring>
#include "WoW/Common/PhaseShiftShared.h"
#include "Engine/Source/Base/CDataStore.h"

// OFFSET: 0x00A32290/0x1407DC510
int32_t PhaseShift_CompareSortedPhaseArray(uint32_t flagsA, const uint16_t* phasesA, int32_t countA, uint32_t flagsB, const uint16_t* phasesB, int32_t countB) {
    if ((flagsB & flagsA & 8) || ((flagsB | flagsA) & 1) || (flagsB & flagsA & 2)) {
        return 1;
    }

    if (!((flagsB | flagsA) & 2)) {
        if (!countA || !countB) {
            return 0;
        }

        int32_t a = 0;
        int32_t b = 0;

        while (a < countA && b < countB) {
            if (phasesA[a] == phasesB[b]) {
                return 1;
            }

            if (phasesA[a] > phasesB[b]) {
                b++;
            } else if (phasesA[a] < phasesB[b]) {
                a++;
            }
        }

        return 0;
    }

    uint32_t exclusiveFlags;
    const uint16_t* exclusivePhases;
    int32_t exclusiveCount;
    uint32_t otherFlags;
    const uint16_t* otherPhases;
    int32_t otherCount;

    if (flagsB & 2) {
        exclusiveFlags = flagsB;
        exclusivePhases = phasesB;
        exclusiveCount = countB;
        otherFlags = flagsA;
        otherPhases = phasesA;
        otherCount = countA;
    } else {
        exclusiveFlags = flagsA;
        exclusivePhases = phasesA;
        exclusiveCount = countA;
        otherFlags = flagsB;
        otherPhases = phasesB;
        otherCount = countB;
    }

    if ((otherFlags & 8) && !(exclusiveFlags & 4)) {
        return 1;
    }

    for (int32_t i = 0; i < otherCount; i++) {
        int32_t j;

        for (j = 0; j < exclusiveCount; j++) {
            if (otherPhases[i] == exclusivePhases[j]) {
                break;
            }
        }

        if (j == exclusiveCount) {
            return 1;
        }
    }

    return 0;
}

// OFFSET: 0x00A32380/0x1407DC650
int32_t CPhaseShift::operator==(const CPhaseShift& other) const {
    if (this->m_flags != other.m_flags) {
        return 0;
    }

    if (this->m_phases.Count() != other.m_phases.Count()) {
        return 0;
    }

    if (!other.m_phases.Count()) {
        return 1;
    }

    return memcmp(this->m_phases.Ptr(), other.m_phases.Ptr(), sizeof(uint16_t) * this->m_phases.Count()) == 0;
}

// OFFSET: 0x00A32440/0x1407DC6D0
CPhaseShift& CPhaseShift::operator=(const CPhaseShift& source) {
    this->m_flags = source.m_flags;
    this->m_phases.SetCount(source.m_phases.Count());

    if (source.m_phases.Count()) {
        memcpy(this->m_phases.Ptr(), source.m_phases.Ptr(), sizeof(uint16_t) * source.m_phases.Count());
    }

    return *this;
}

// OFFSET: 0x00A324F0/0x1407DC790
void CPhaseShift::SetPhasesFromDataStore(CDataStore* msg) {
    msg->Get(this->m_flags);

    uint32_t count;
    msg->Get(count);
    this->m_phases.SetCount(count);

    if (count) {
        msg->GetData(this->m_phases.Ptr(), sizeof(uint16_t) * count);
    }
}

// OFFSET: 0x00A32570/0x1407DC830
CPhaseShift::CPhaseShift() {
    this->m_flags = 0;
    this->m_phases.SetCount(0);
}

// INLINED
CPhaseShift::CPhaseShift(uint32_t flags) {
    this->m_flags = flags;
}

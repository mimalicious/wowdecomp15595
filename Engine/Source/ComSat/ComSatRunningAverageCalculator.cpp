#include "Engine/Source/ComSat/ComSatRunningAverageCalculator.h"

// OFFSET: 0x00A39E10
ComSat::RunningAverageCalculator::RunningAverageCalculator(ComSat::Allocator& allocator) {
    this->m_sum = 0.0;
    this->m_allocator = &allocator;
    this->m_values = NULL;
    this->m_count = 0;
    this->m_index = 0;
}

// OFFSET: [0x00A39F60, 0x00A39FA0]
ComSat::RunningAverageCalculator::~RunningAverageCalculator() {
    if (this->m_values) {
        this->m_allocator->Free(this->m_values);
        this->m_values = NULL;
    }
    this->m_count = 0;
    this->m_sum = 0.0;
    this->m_index = 0;
}

// PROVEN: 028b1a69 IDENTICAL/DIFFERENT
// OFFSET: 0x00A39E40/0x1407E62C0
void ComSat::RunningAverageCalculator::Init(double value, uint32_t count) {
    if (this->m_values) {
        this->m_allocator->Free(this->m_values);
        this->m_values = NULL;
    }
    this->m_sum = 0.0;
    this->m_count = 0;
    this->m_index = 0;
    this->m_values = static_cast<double*>(this->m_allocator->Alloc(sizeof(double) * count));
    this->m_count = count;
    for (uint32_t i = 0; i < count; i++) {
        this->m_values[i] = value;
        this->m_sum += value;
    }
}

// PROVEN: 778150d6 IDENTICAL/DIFFERENT
// OFFSET: 0x00A39F00/0x1407E63E0
void ComSat::RunningAverageCalculator::SubmitData(double value) {
    if (this->m_values) {
        this->m_sum = this->m_sum - this->m_values[this->m_index] + value;
        this->m_values[this->m_index] = value;
        this->m_index = (this->m_index + 1) % this->m_count;
    }
}

// PROVEN: f0094b4f IDENTICAL/DIFFERENT
// OFFSET: 0x00A39F40/0x1407E6420
double ComSat::RunningAverageCalculator::GetCurrentAverage() {
    return this->m_sum / this->m_count;
}

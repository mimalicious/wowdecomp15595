#include "Engine/Source/ComSat/ComSatRunningAverageCalculator.h"

// OFFSET: %0x0097A120
ComSat::RunningAverageCalculator::RunningAverageCalculator(ComSat::Allocator& allocator) {
    this->m_sum = 0.0;
    this->m_allocator = &allocator;
    this->m_values = NULL;
    this->m_count = 0;
    this->m_index = 0;
}

// OFFSET: [#0x00886750, %0x0097A2B0]
ComSat::RunningAverageCalculator::~RunningAverageCalculator() {
    if (this->m_values) {
        this->m_allocator->Free(this->m_values);
        this->m_values = NULL;
    }
    this->m_count = 0;
    this->m_sum = 0.0;
    this->m_index = 0;
}

// OFFSET: %0x0097A150
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

// OFFSET: %0x0097A210
void ComSat::RunningAverageCalculator::SubmitData(double value) {
    if (this->m_values) {
        this->m_sum = this->m_sum - this->m_values[this->m_index] + value;
        this->m_values[this->m_index] = value;
        this->m_index = (this->m_index + 1) % this->m_count;
    }
}

// OFFSET: %0x0097A250
double ComSat::RunningAverageCalculator::GetCurrentAverage() {
    return this->m_sum / this->m_count;
}

#include <cstdint>
#include <cstring>
#include "Engine/Source/Base/CDataAllocator.h"
#include "Engine/Source/Storm/H/storm.h"

// OFFSET: %0x007D75C0
CDataAllocator::CDataAllocator(uint32_t bytesPerData, uint32_t dataPerBlock) {
    this->m_bytesPerData = bytesPerData < sizeof(Data) ? sizeof(Data) : bytesPerData;
    this->m_dataPerBlock = dataPerBlock ? dataPerBlock : 1;
    this->m_dataUsed = 0;
    this->m_blockList = NULL;
    this->m_dataList = NULL;
}

// OFFSET: #0x0095D0C0
void CDataAllocator::Clear(const char* fileName, int32_t lineNumber, int32_t reportLeaks) {
    while (this->m_blockList) {
        Block* block = this->m_blockList;
        this->m_blockList = block->m_next;
        SMemFree(block, fileName ? fileName : __FILE__, fileName ? lineNumber : __LINE__, 0);
    }

    this->m_dataList = NULL;
    this->m_dataUsed = 0;
}

// OFFSET: #0x0095D110
void* CDataAllocator::GetData(int32_t zero, const char* fileName, int32_t lineNumber) {
    if (this->m_dataPerBlock == 1) {
        fileName = NULL;
        lineNumber = 0;
    }

    if (!this->m_dataList) {
        if (!fileName) {
            fileName = __FILE__;
            lineNumber = __LINE__;
        }

        Block* block = static_cast<Block*>(SMemAlloc(this->m_dataPerBlock * this->m_bytesPerData + sizeof(Block), fileName, lineNumber, 0));
        Data* data = reinterpret_cast<Data*>(block + 1);
        this->m_dataList = data;

        for (uint32_t i = 0; i < this->m_dataPerBlock - 1; i++) {
            Data* next = reinterpret_cast<Data*>(reinterpret_cast<char*>(data) + this->m_bytesPerData);
            data->m_next = next;
            data = next;
        }

        data->m_next = NULL;
        block->m_next = this->m_blockList;
        this->m_blockList = block;
    }

    Data* data = this->m_dataList;
    this->m_dataList = data->m_next;

    if (zero) {
        memset(data, 0, this->m_bytesPerData);
    }

    this->m_dataUsed++;

    return data;
}

// OFFSET: %0x007D76F0
void CDataAllocator::PutData(void* data, const char* fileName, int32_t lineNumber) {
    Data* node = static_cast<Data*>(data);
    node->m_next = this->m_dataList;
    this->m_dataUsed--;
    this->m_dataList = node;
}

// OFFSET: #0x0095D1D0
CDataAllocator::~CDataAllocator() {
    this->Clear(NULL, 0, 1);
}

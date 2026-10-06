#include <cstdint>
#include <new>
#include "Engine/Source/BlizzardCore/Source/Packages/Streaming/Storage.h"

// OFFSET: %0x0070E7C0
Blizzard::Storage::Range::Range(int64_t offset, int32_t length, int64_t chunkOffset, int32_t chunkLength, int32_t state) {
    this->m_offset = offset;
    this->m_length = length;
    this->m_chunkOffset = chunkOffset;
    this->m_chunkLength = chunkLength;
    this->m_state = state;
}

// OFFSET: %0x0071CED0
int32_t Blizzard::Storage::GetRandomRange(StorageUnit* unit, int64_t offset, int32_t size, Range* ranges) {
    int32_t chunkSize = unit->m_chunkSize;
    int32_t remaining = size;
    int32_t count = 0;
    int64_t position = offset;
    int32_t index = static_cast<int32_t>(offset / chunkSize);

    for (int32_t phase = 1; phase <= 3; phase++) {
        if (!remaining) {
            return count;
        }

        if (phase == 2) {
            if (remaining < chunkSize) {
                continue;
            }

            do {
                uint8_t state = unit->m_chunkStates[index] & ChunkState::CHUNK_STATE_MASK;

                switch (state) {
                case ChunkState::CHUNK_NONE:
                    if (unit->m_local && unit->m_local->IsAvailable(position, chunkSize)) {
                        new (&ranges[count++]) Range(position, chunkSize, position, chunkSize, 0);
                        unit->m_chunkStates[index] = (unit->m_chunkStates[index] & ChunkState::CHUNK_CACHE_ENTRY) | ChunkState::CHUNK_LOCAL;
                        break;
                    }

                case ChunkState::CHUNK_REQUESTED:
                    new (&ranges[count++]) Range(position, chunkSize, position, chunkSize, 3 - (state != ChunkState::CHUNK_REQUESTED));
                    break;

                case ChunkState::CHUNK_DOWNLOADING:
                case ChunkState::CHUNK_CACHED:
                case ChunkState::CHUNK_LOCAL:
                    new (&ranges[count++]) Range(static_cast<int64_t>(index) * chunkSize, chunkSize, 0, 0, state == ChunkState::CHUNK_DOWNLOADING);
                    break;

                default:
                    break;
                }

                index++;
                chunkSize = unit->m_chunkSize;
                position += chunkSize;
                remaining -= chunkSize;
            } while (remaining >= chunkSize);
        } else {
            int64_t chunkStart = static_cast<int64_t>(index) * chunkSize;

            if (phase == 1 && position == chunkStart) {
                continue;
            }

            int32_t inChunk = static_cast<int32_t>(position - chunkStart);
            int32_t chunkLength = unit->m_logicalFileSize <= static_cast<int64_t>(index + 1) * chunkSize ? static_cast<int32_t>(unit->m_logicalFileSize) - index * chunkSize : chunkSize;
            int32_t length = remaining >= chunkLength - inChunk ? chunkLength - inChunk : remaining;
            uint8_t state = unit->m_chunkStates[index] & ChunkState::CHUNK_STATE_MASK;

            switch (state) {
            case ChunkState::CHUNK_NONE:
                if (unit->m_local && unit->m_local->IsAvailable(position, length)) {
                    new (&ranges[count++]) Range(position, length, position, length, 0);
                    break;
                }

            case ChunkState::CHUNK_REQUESTED:
                new (&ranges[count++]) Range(position, length, chunkStart, chunkLength, 3 - (state != ChunkState::CHUNK_REQUESTED));
                break;

            case ChunkState::CHUNK_DOWNLOADING:
            case ChunkState::CHUNK_CACHED:
            case ChunkState::CHUNK_LOCAL:
                new (&ranges[count++]) Range(inChunk + static_cast<int64_t>(chunkSize) * index, length, 0, 0, state == ChunkState::CHUNK_DOWNLOADING);
                break;

            default:
                break;
            }

            chunkSize = unit->m_chunkSize;
            remaining -= length;
            index++;
            position += chunkSize - inChunk;
        }
    }

    return count;
}

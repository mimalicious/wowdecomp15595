#ifndef ENGINE_SOURCE_BLIZZARDCORE_SOURCE_PACKAGES_UTIL_PATCH_PATCH_INL
#define ENGINE_SOURCE_BLIZZARDCORE_SOURCE_PACKAGES_UTIL_PATCH_PATCH_INL

#include <cstring>

// OFFSET: [%0x00718B60<MpqPatchStream>, %0x00718C00<Blizzard::Patch::MemoryStream>]
template <class S>
bool Blizzard::Patch::PatchStream<S>::FindChunk(uint32_t tag, uint32_t& offset, ChunkHeader& header) {
    while (this->m_stream->Read(offset, &header, sizeof(ChunkHeader))) {
        if (header.m_tag == tag) {
            return true;
        }

        if (header.m_tag == CHUNK_PTCH) {
            offset += 16;
        } else {
            if (!header.m_size) {
                return false;
            }

            offset += header.m_size;
        }
    }

    return false;
}

// OFFSET: [%0x0071FBA0<MpqPatchStream, MD5Chunk>, %0x0071FBF0<Blizzard::Patch::MemoryStream, MD5Chunk>]
template <class S>
template <class C>
bool Blizzard::Patch::PatchStream<S>::Search(uint32_t tag, uint32_t& offset, C& chunk) {
    if (!this->FindChunk(tag, offset, chunk)) {
        return false;
    }

    return this->m_stream->Read(offset + sizeof(ChunkHeader), reinterpret_cast<uint8_t*>(&chunk) + sizeof(ChunkHeader), sizeof(C) - sizeof(ChunkHeader));
}

#endif

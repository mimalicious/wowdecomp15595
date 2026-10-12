#include "WoW/Source/Battlenet/BSN/Encoder.h"
#include <cstring>

// OFFSET: 0x006621E0/0x140307EE0
BSN::Encoder::~Encoder() {
}

// OFFSET: 0x00401000/0x1407D7E80
void BSN::Encoder::Flush() {
}

// OFFSET: 0x00665080/0x14030BB20
void BSN::Encoder::SetData(void* data, uint32_t bytes) {
    this->m_data = static_cast<uint8_t*>(data);
    this->m_size = 8 * bytes;
    this->m_position = 0;
}

// OFFSET: 0x00663CA0/0x140309E40
bool BSN::IsValidAscii(uint32_t length, const char* data) {
    for (uint32_t i = 0; i < length; i++) {
        uint8_t c = data[i];

        if (!c || c > 0x7E) {
            return false;
        }
    }

    return true;
}

// OFFSET: 0x006650B0/0x14030B3F0
int32_t BSN::Encoder::PutBool(bool value) {
    return this->PutBits(1, value != false);
}

// OFFSET: 0x00665160/0x14030B4D0
int32_t BSN::Encoder::PutUInt32(uint32_t value) {
    return this->PutBits(32, value);
}

// OFFSET: 0x00665210/0x14030B5A0
int32_t BSN::Encoder::PutChecksum(uint32_t bits, uint32_t& checksum) {
    uint32_t offset = this->m_position >> 3;

    if (offset >= 4) {
        checksum += *reinterpret_cast<const uint32_t*>(&this->m_data[offset - 4]);
    }

    if (offset >= 2) {
        checksum += *reinterpret_cast<const uint16_t*>(&this->m_data[offset - 2]);
    } else if (offset) {
        checksum += this->m_data[offset - 1];
    } else {
        checksum = ~checksum;
    }

    uint32_t value = (checksum << 8) | (checksum >> 24);
    checksum = value;

    if (!bits) {
        return BSN::CODEC_VALID;
    }

    return this->PutBits(bits, value);
}

// OFFSET: 0x00665300/0x14030B6D0
int32_t BSN::Encoder::PutData(uint32_t bits, const uint8_t* data) {
    if ((this->m_position & 7) == 0 && bits >= 8) {
        do {
            uint32_t position = this->m_position;
            uint32_t size = this->m_size;

            if (position >= size) {
                this->Flush();
                position = this->m_position;
                size = this->m_size;

                if (position >= size) {
                    return BSN::CODEC_OVERFLOW;
                }

                if (position & 7) {
                    break;
                }
            }

            uint32_t chunk = bits & ~7u;

            if (chunk >= size - position) {
                chunk = size - position;
            }

            memcpy(&this->m_data[position >> 3], data, chunk >> 3);
            this->m_position += chunk;
            data += chunk >> 3;
            bits -= chunk;
        } while (bits >= 8);
    }

    while (bits >= 32) {
        uint32_t value = (data[0] << 24) | (data[1] << 16) | (data[2] << 8) | data[3];

        if (this->PutBits(32, value)) {
            return BSN::CODEC_OVERFLOW;
        }

        data += 4;
        bits -= 32;
    }

    if (!bits) {
        return BSN::CODEC_VALID;
    }

    uint32_t total = bits;
    uint32_t value = 0;

    for (; bits >= 8; bits -= 8) {
        value = (value << 8) | *data++;
    }

    if (bits) {
        value = (value << bits) | (*data & ((1 << bits) - 1));
    }

    return this->PutBits(total, value);
}

// OFFSET: 0x00665550/0x14030B980
int32_t BSN::Encoder::Align() {
    uint32_t position = this->m_position;
    uint32_t aligned = (position + 7) & ~7u;

    if (aligned <= position) {
        return BSN::CODEC_VALID;
    }

    uint32_t remaining = aligned - position;

    while (true) {
        position = this->m_position;

        if (position >= this->m_size) {
            this->Flush();
            position = this->m_position;

            if (position >= this->m_size) {
                return BSN::CODEC_OVERFLOW;
            }
        }

        uint32_t shift = position & 7;
        uint32_t count = 8 - shift;

        if (count >= remaining) {
            count = remaining;
        }

        remaining -= count;
        this->m_data[position >> 3] &= ~(((1 << count) - 1) << shift);
        this->m_position += count;

        if (!remaining) {
            return BSN::CODEC_VALID;
        }
    }
}

// OFFSET: 0x00665600/0x14030BA60
int32_t BSN::Encoder::PutAsciiData(uint32_t bytes, const char* data) {
    int32_t result = this->Align();

    if (result) {
        return result;
    }

    if (!BSN::IsValidAscii(bytes, data)) {
        return BSN::CODEC_INVALID;
    }

    return this->PutData(8 * bytes, reinterpret_cast<const uint8_t*>(data));
}

// OFFSET: 0x00665650/0x14030BAD0
int32_t BSN::Encoder::PutAlignedData(uint32_t bytes, const uint8_t* data) {
    int32_t result = this->Align();

    if (result) {
        return result;
    }

    return this->PutData(8 * bytes, data);
}

// OFFSET: 0x004E1F00/0x140122B10
BSN::Decoder::~Decoder() {
}

// OFFSET: 0x00842700/0x1407FDD80
int32_t BSN::Decoder::Fill() {
    return BSN::CODEC_VALID;
}

// OFFSET: 0x00665080/0x14030BB20
void BSN::Decoder::SetData(const void* data, uint32_t bytes) {
    this->m_data = static_cast<const uint8_t*>(data);
    this->m_size = 8 * bytes;
    this->m_position = 0;
}

// OFFSET: 0x00665680/0x14030BB40
int32_t BSN::Decoder::GetData(uint32_t bits, uint8_t* data) {
    if ((this->m_position & 7) == 0 && bits >= 8) {
        do {
            uint32_t position = this->m_position;
            uint32_t size = this->m_size;

            if (position >= size) {
                int32_t result = this->Fill();

                if (result) {
                    return result;
                }

                position = this->m_position;
                size = this->m_size;

                if (position >= size) {
                    return BSN::CODEC_OVERFLOW;
                }

                if (position & 7) {
                    break;
                }
            }

            uint32_t chunk = bits & ~7u;

            if (chunk >= size - position) {
                chunk = size - position;
            }

            memcpy(data, &this->m_data[position >> 3], chunk >> 3);
            this->m_position += chunk;
            data += chunk >> 3;
            bits -= chunk;
        } while (bits >= 8);
    }

    while (bits >= 32) {
        uint32_t remaining = 32;
        uint32_t value = 0;

        while (true) {
            uint32_t position = this->m_position;

            if (position >= this->m_size) {
                int32_t result = this->Fill();

                if (result) {
                    return result;
                }

                position = this->m_position;

                if (position >= this->m_size) {
                    return BSN::CODEC_OVERFLOW;
                }
            }

            uint32_t shift = position & 7;
            uint32_t count = 8 - shift;

            if (count >= remaining) {
                count = remaining;
            }

            uint32_t byte = this->m_data[position >> 3] >> shift;
            remaining -= count;
            this->m_position += count;
            value |= (((1 << count) - 1) & byte) << remaining;

            if (!remaining) {
                break;
            }
        }

        data[0] = static_cast<uint8_t>(value >> 24);
        data[1] = static_cast<uint8_t>(value >> 16);
        data[2] = static_cast<uint8_t>(value >> 8);
        data[3] = static_cast<uint8_t>(value);
        data += 4;
        bits -= 32;
    }

    if (!bits) {
        return BSN::CODEC_VALID;
    }

    uint32_t remaining = bits;
    uint32_t value = 0;

    while (true) {
        uint32_t position = this->m_position;

        if (position >= this->m_size) {
            int32_t result = this->Fill();

            if (result) {
                return result;
            }

            position = this->m_position;

            if (position >= this->m_size) {
                return BSN::CODEC_OVERFLOW;
            }
        }

        uint32_t shift = position & 7;
        uint32_t count = 8 - shift;

        if (count >= remaining) {
            count = remaining;
        }

        uint32_t byte = this->m_data[position >> 3] >> shift;
        remaining -= count;
        this->m_position += count;
        value |= (((1 << count) - 1) & byte) << remaining;

        if (!remaining) {
            break;
        }
    }

    if (bits >= 8) {
        uint32_t shift = bits - 8;

        for (uint32_t i = bits >> 3; i; i--) {
            *data++ = static_cast<uint8_t>(value >> shift);
            bits -= 8;
            shift -= 8;
        }
    }

    if (bits) {
        *data = value & ((1 << bits) - 1);
    }

    return BSN::CODEC_VALID;
}

// OFFSET: 0x006658D0/0x14030BE10
int32_t BSN::Decoder::GetAlignedData(uint32_t bytes, uint8_t* data) {
    this->m_position = (this->m_position + 7) & ~7u;
    return this->GetData(8 * bytes, data);
}

// INLINED
BSN::SizeEncoder::~SizeEncoder() {
}

// OFFSET: 0x00662240
BSN::Hard::SizeEncoder::~SizeEncoder() {
}

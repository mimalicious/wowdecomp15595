// INLINED
inline JamClientMessage::JamClientMessage() {
    this->m_time = 0;
}

// INLINED
inline JamClientMessage::JamClientMessage(NetClient* netClient, WowConnection* connection, uint32_t time) {
    this->m_netClient = netClient;
    this->m_connection = connection;
    this->m_time = time;
}

// INLINED
inline JamBitReader::JamBitReader(CDataStore* msg) {
    this->m_msg = msg;
    this->m_value = 0;
    this->m_bits = 8;
}

// OFFSET: 0x0066CEA0
inline uint32_t JamBitReader::ReadBit() {
    if (this->m_bits == 8) {
        uint8_t value = 0;
        this->m_msg->Get(value);
        this->m_value = value;
        this->m_bits = 0;
    }

    uint32_t bit = this->m_value >> 7;
    this->m_value <<= 1;
    this->m_bits++;

    return bit;
}

// INLINED
template <uint32_t count>
inline uint32_t JamBitReader::ReadBits() {
    return this->ReadBits<count>(JamBitCount<(count > 8)>());
}

// OFFSET: [0x00696C20/0x140342240<2>, 0x006A3020<3>, 0x0066CCC0<4>, 0x0066CD60/0x140313CA0<5>, 0x00696CC0/0x140342300<6>, 0x0066CE00/0x140313D60<7>, 0x0066CEE0<8>]
template <uint32_t count>
uint32_t JamBitReader::ReadBits(JamBitCount<false>) {
    uint8_t bits = this->m_bits;
    uint32_t result;

    if (bits <= 8 - count) {
        uint8_t value = this->m_value;
        result = value >> (8 - count);
        this->m_value = value << count;
        this->m_bits = bits + count;
    } else {
        uint8_t value = 0;

        if (bits == 8) {
            this->m_msg->Get(value);
            result = value >> (8 - count);
            this->m_value = value << count;
            this->m_bits = count;
        } else {
            uint32_t high = this->m_value >> bits;
            this->m_msg->Get(value);
            uint8_t remain = this->m_bits - (8 - count);
            result = (high << remain) | (value >> (8 - remain));
            this->m_bits = remain;
            this->m_value = value << remain;
        }
    }

    return result;
}

// INLINED
template <>
inline uint32_t JamBitReader::ReadBits<0>(JamBitCount<false>) {
    return 0;
}

// INLINED
template <>
inline uint32_t JamBitReader::ReadBits<1>(JamBitCount<false>) {
    return this->ReadBit();
}

// OFFSET: [0x0066EBD0/0x140315BA0<9>, 0x006C2BC0<11>, 0x0066EC70<12>, 0x006975E0<13>, 0x006C2C50<14>, 0x006C2CD0/0x1403A8680<17>, 0x006C2D90<18>, 0x006C2E30<19>, 0x006C2ED0<20>, 0x006C2F70<21>, 0x00697660<22>, 0x006C3010<23>, 0x00697700/0x1403BB7A0<24>, 0x006977A0/0x140342D60<25>, 0x006C30B0<26>, 0x00697860/0x140342E40<30>]
template <uint32_t count>
uint32_t JamBitReader::ReadBits(JamBitCount<true>) {
    uint32_t result = 0;

    for (int32_t i = 0; i < static_cast<int32_t>(count / 8 * 8); i += 8) {
        uint32_t byte;
        uint8_t whole;

        if (this->m_bits == 8) {
            whole = 0;
            this->m_msg->Get(whole);
            byte = whole;
        } else {
            uint32_t high = this->m_value >> this->m_bits;
            uint8_t value = 0;
            this->m_msg->Get(value);
            uint8_t bits = this->m_bits;
            byte = (value >> static_cast<uint8_t>(8 - bits)) | (high << bits);
            this->m_value = value << bits;
        }

        result |= byte << (count - 8 - i);
    }

    return result | this->ReadBits<count % 8>(JamBitCount<false>());
}

// INLINED
inline JamBitWriter::JamBitWriter(CDataStore* msg) {
    this->m_msg = msg;
    this->m_value = 0;
    this->m_bits = 0;
}

// INLINED
inline void JamBitWriter::WriteBit(uint32_t value) {
    this->m_value = (this->m_value << 1) | value;

    if (++this->m_bits == 8) {
        this->m_msg->Put(this->m_value);
        this->m_bits = 0;
        this->m_value = 0;
    }
}

// INLINED
template <uint32_t count>
inline void JamBitWriter::WriteBits(uint32_t value) {
    this->WriteBits<count>(value, JamBitCount<(count > 8)>());
}

// OFFSET: [0x0066A6C0<1>, 0x0066AA30/0x140313820<2>, 0x0066AAD0/0x1403138E0<3>, 0x0066AB70/0x1403139A0<4>, 0x0066AC00/0x140313A60<5>, 0x0066AC90/0x140313B20<6>, 0x0066AD20/0x140313BE0<7>]
template <uint32_t count>
void JamBitWriter::WriteBits(uint32_t value, JamBitCount<false>) {
    int32_t free = 8 - this->m_bits;
    int32_t over = count - free;

    if (over == 0) {
        this->m_msg->Put(static_cast<uint8_t>((this->m_value << count) | value));
        this->m_bits = 0;
        this->m_value = 0;
    } else if (over < 0) {
        this->m_value = (this->m_value << count) | value;
        this->m_bits += count;
    } else {
        this->m_msg->Put(static_cast<uint8_t>((value >> over) | (this->m_value << free)));
        this->m_bits = over;
        this->m_value = value & ((1 << over) - 1);
    }
}

// OFFSET: 0x0066A6C0
template <>
inline void JamBitWriter::WriteBits<1>(uint32_t value, JamBitCount<false>) {
    this->WriteBit(value);
}

// INLINED
template <>
inline void JamBitWriter::WriteBits<0>(uint32_t value, JamBitCount<false>) {
}

// OFFSET: 0x0066A6F0
template <>
inline void JamBitWriter::WriteBits<8>(uint32_t value, JamBitCount<false>) {
    if (!this->m_bits) {
        this->m_msg->Put(static_cast<uint8_t>(value));
        return;
    }

    this->m_msg->Put(static_cast<uint8_t>((static_cast<uint8_t>(value) >> this->m_bits) | (this->m_value << (8 - this->m_bits))));
    this->m_value = value & ((1 << this->m_bits) - 1);
}

// OFFSET: [0x0066E960<9>, 0x0066E9E0<10>, 0x0066EA50<11>, 0x0066AF20<12>, 0x0069C820<14>, 0x00675620<20>, 0x006756B0<22>, 0x0066EAC0<23>, 0x0066EB50<24>, 0x00697540/0x140342B40<25>, 0x00675740<26>, 0x006757D0/0x14031D7F0<30>]
template <uint32_t count>
void JamBitWriter::WriteBits(uint32_t value, JamBitCount<true>) {
    for (int32_t i = 0; i < static_cast<int32_t>(count / 8 * 8); i += 8) {
        uint32_t byte = value >> (count - 8 - i);

        if (!this->m_bits) {
            this->m_msg->Put(static_cast<uint8_t>(byte));
        } else {
            this->m_msg->Put(static_cast<uint8_t>((this->m_value << (8 - this->m_bits)) | (static_cast<uint8_t>(byte) >> this->m_bits)));
            this->m_value = byte & ((1 << this->m_bits) - 1);
        }
    }

    this->WriteBits<count % 8>(value & ((1 << (count % 8)) - 1), JamBitCount<false>());
}

// OFFSET: 0x0066ADB0/0x140313E20
inline void JamBitWriter::Flush() {
    switch (this->m_bits) {
        case 1:
            this->WriteBits<7>(0, JamBitCount<false>());
            break;
        case 2:
            this->WriteBits<6>(0, JamBitCount<false>());
            break;
        case 3:
            this->WriteBits<5>(0, JamBitCount<false>());
            break;
        case 4:
            this->WriteBits<4>(0, JamBitCount<false>());
            break;
        case 5:
            this->WriteBits<3>(0, JamBitCount<false>());
            break;
        case 6:
            this->WriteBits<2>(0, JamBitCount<false>());
            break;
        case 7:
            this->WriteBits<1>(0, JamBitCount<false>());
            break;
    }
}

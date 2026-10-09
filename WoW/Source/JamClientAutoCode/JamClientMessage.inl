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

// INLINED
inline bool JamBitReader::ReadBit() {
    if (this->m_bits == 8) {
        uint8_t value = 0;
        this->m_msg->Get(value);
        this->m_value = value;
        this->m_bits = 0;
    }

    uint8_t value = this->m_value;
    this->m_bits++;
    this->m_value = value << 1;

    return (value >> 7) != 0;
}

// INLINED
template <uint32_t count>
inline uint32_t JamBitReader::ReadBits() {
    uint32_t result = 0;

    for (uint32_t i = 0; i < count; i++) {
        result = (result << 1) | (this->ReadBit() ? 1 : 0);
    }

    return result;
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

// PROVEN: e73e4dd7 IDENTICAL/NONE
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

// PROVEN: a3ac65bc IDENTICAL/NONE
// OFFSET: 0x0066A6C0
template <>
inline void JamBitWriter::WriteBits<1>(uint32_t value, JamBitCount<false>) {
    this->WriteBit(value);
}

// PROVEN: 2d1fbfa8 IDENTICAL/NONE
// OFFSET: [0x0066AF20<12>, 0x006757D0<30>]
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

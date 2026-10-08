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
inline void JamBitWriter::WriteBit(bool value) {
    this->m_value = (this->m_value << 1) | (value ? 1 : 0);

    if (++this->m_bits == 8) {
        this->m_msg->Put(this->m_value);
        this->m_value = 0;
        this->m_bits = 0;
    }
}

// INLINED
template <uint32_t count>
inline void JamBitWriter::WriteBits(uint32_t value) {
    for (uint32_t i = count; i > 0; i--) {
        this->WriteBit(((value >> (i - 1)) & 1) != 0);
    }
}

// INLINED
inline void JamBitWriter::Flush() {
    while (this->m_bits) {
        this->WriteBit(false);
    }
}

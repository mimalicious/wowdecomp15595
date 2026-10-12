#include "Engine/Source/Gx/Gx.h"

// OFFSET: [0x0074B6D0<Rain::Drop, 1024>, 0x0074B730<Rain::Patter, 1024>, 0x00749760<Snow::Flake, 6144>, 0x00749760<Sand::Grain, 6144>]
template <class T, uint32_t N>
PacketBuffer<T, N>::PacketBuffer() : m_capacity(N) {
}

// OFFSET: [0x0074BBF0/0x14042B8B0<Rain::Drop, 1024, 3>, 0x0074BC70/0x14042B980<Rain::Patter, 1024, 3>, 0x0074B650/0x14042AF80<Snow::Flake, 6144, 1>, 0x0074B650/0x14042AF80<Sand::Grain, 6144, 1>]
template <class T, uint32_t N, uint32_t V>
Packet<T, N, V>::Packet() {
    this->m_pool = g_theGxDevicePtr->PoolCreate(GxPoolTarget_Vertex, static_cast<EGxPoolUsage>(1), N * V * sizeof(typename T::Vertex), static_cast<EGxPoolHintBits>(2), "WeatherPacket_vtx");
    this->m_buf = g_theGxDevicePtr->BufCreate(this->m_pool, sizeof(typename T::Vertex), N * V, 0);
}

// OFFSET: [0x0074B470<Rain::Drop, 1024, 3>, 0x0074B510<Rain::Patter, 1024, 3>, 0x0074B5B0<Snow::Flake, 6144, 1>, 0x0074B5B0<Sand::Grain, 6144, 1>]
template <class T, uint32_t N, uint32_t V>
Packet<T, N, V>::~Packet() {
    GxBufDestroy(this->m_buf);

    if (this->m_pool) {
        g_theGxDevicePtr->PoolDestroy(this->m_pool);
    }
}

// INLINED
template <class P>
PacketList<P>::PacketList() {
    this->m_activeCount = 0;
    this->m_freeCount = 0;
    this->m_current = NULL;
}

// OFFSET: [0x007517E0<Packet<Rain::Drop, 1024, 3>>, 0x00751A20<Packet<Rain::Patter, 1024, 3>>, 0x00751B60<Packet<Snow::Flake, 6144, 1>>, 0x00751CA0<Packet<Sand::Grain, 6144, 1>>]
template <class P>
PacketList<P>::~PacketList() {
    for (P* packet = this->m_active.Head(), * next; packet; packet = next) {
        next = this->m_active.Next(packet);
        this->m_activeCount--;
        this->m_freeCount++;
        this->m_free.LinkToTail(packet);
    }

    for (P* packet = this->m_free.Head(), * next; packet; packet = next) {
        next = this->m_free.Next(packet);
        packet->~P();
        SMemFree(packet, typeid(P).raw_name(), SLOG_OBJECT, 0);
    }
}

// OFFSET: [0x007516E0/0x1404320B0<Packet<Rain::Drop, 1024, 3>>, 0x00751920/0x140432460<Packet<Rain::Patter, 1024, 3>>, 0x0074BDE0/0x14042BBA0<Packet<Snow::Flake, 6144, 1>>, 0x0074BEE0/0x14042BCE0<Packet<Sand::Grain, 6144, 1>>]
template <class P>
P* PacketList<P>::Alloc() {
    this->m_current = this->m_free.Head();

    if (!this->m_current) {
        void* mem = SMemAlloc(sizeof(P), typeid(P).raw_name(), SLOG_OBJECT, 0x8);
        P* packet = mem ? new (mem) P() : NULL;
        this->m_free.LinkToTail(packet);
        this->m_freeCount++;
        this->m_current = packet;
    }

    P* packet = this->m_current;
    packet->m_count = 0;
    packet->m_baseTime = 0;
    packet->m_lastTime = 0;
    packet->m_origin.x = 0.0f;
    packet->m_origin.y = 0.0f;
    packet->m_buildTime = 0.0f;
    packet->m_origin.z = 0.0f;
    return packet;
}

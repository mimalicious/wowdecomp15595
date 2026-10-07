#ifndef WOW_SOURCE_WORLDCLIENT_WEATHERPACKET_H
#define WOW_SOURCE_WORLDCLIENT_WEATHERPACKET_H

#include <cstdint>
#include <typeinfo>
#include "Engine/Source/Gx/CGxBuf.h"
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "Engine/Source/Storm/H/stpl.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/Tempest/C3Vector.h"

template <class T, uint32_t N>
class PacketBuffer {
    public:
    /* 0000 */ uint32_t m_capacity;
    /* 0004 */ T m_particles[N];

    PacketBuffer();
};

template <class T, uint32_t N, uint32_t V>
class Packet : public TSLinkedNode<Packet<T, N, V> > {
    public:
    /* 0008 */ PacketBuffer<T, N> m_buffer;
    uint32_t m_count;
    uint32_t m_baseTime;
    uint32_t m_lastTime;
    float m_buildTime;
    C3Vector m_origin;
    CGxPool* m_pool;
    CGxBuf* m_buf;

    Packet();
    ~Packet();
};

template <class P>
class PacketList {
    public:
    /* 0000 */ TSList<P, TSGetLink<P> > m_free;
    /* 000C */ uint32_t m_activeCount;
    /* 0010 */ uint32_t m_freeCount;
    /* 0014 */ P* m_current;
    /* 0018 */ TSList<P, TSGetLink<P> > m_active;

    ~PacketList();
    P* Alloc();
};

#include "WoW/Source/WorldClient/WeatherPacket.inl"

#endif

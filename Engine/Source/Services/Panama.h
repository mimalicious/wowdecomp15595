#ifndef ENGINE_SOURCE_SERVICES_PANAMA_H
#define ENGINE_SOURCE_SERVICES_PANAMA_H

#include <cstddef>
#include <cstdint>

namespace Crypt {
    class Panama {
        public:
        /* 0004 */ uint32_t m_stateSize;
        /* 0008 */ uint32_t* m_state;
        /* 000C */ uint8_t m_stateData[0x48C];

        virtual void Reset();

        Panama();
        ~Panama();
        void Iterate(uint32_t count, const uint32_t* p, uint32_t* z);
        void SetKey(const uint8_t* key, const uint8_t* iv);
    };

    class PanamaCypher : public Panama {
        public:
        /* 0498 */ uint32_t m_bufferSize;
        /* 049C */ uint8_t* m_buffer;
        /* 04A0 */ uint8_t m_bufferData[0x40F];
        /* 08B0 */ uint32_t m_leftOver;

        static void __stdcall xorbuf(uint8_t* buf, const uint8_t* mask, uint32_t count);

        virtual void Reset();

        PanamaCypher();
        ~PanamaCypher();
        void ProcessBuffer(uint8_t* buffer, uint32_t size);
    };
}

#endif

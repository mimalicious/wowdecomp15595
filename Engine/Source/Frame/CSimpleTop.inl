#ifndef ENGINE_SOURCE_FRAME_CSIMPLETOP_INL
#define ENGINE_SOURCE_FRAME_CSIMPLETOP_INL

// INLINED
inline CSimpleTop* CSimpleTop::GetInstance() {
    return CSimpleTop::s_instance;
}

#endif

#ifndef ENGINE_SOURCE_FRAME_CSIMPLEMOVIEFRAME_INL
#define ENGINE_SOURCE_FRAME_CSIMPLEMOVIEFRAME_INL

// INLINED
inline int32_t CSimpleMovieFrame::GetObjectType() {
    if (!CSimpleMovieFrame::s_objectType) {
        CSimpleMovieFrame::s_objectType = ++FrameScript_Object::s_objectTypes;
    }

    return CSimpleMovieFrame::s_objectType;
}

#endif

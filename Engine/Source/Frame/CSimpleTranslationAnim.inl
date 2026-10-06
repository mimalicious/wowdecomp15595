#ifndef ENGINE_SOURCE_FRAME_CSIMPLETRANSLATIONANIM_INL
#define ENGINE_SOURCE_FRAME_CSIMPLETRANSLATIONANIM_INL

// INLINED
inline int32_t CSimpleTranslationAnim::GetObjectType() {
    if (!CSimpleTranslationAnim::s_objectType) {
        CSimpleTranslationAnim::s_objectType = ++FrameScript_Object::s_objectTypes;
    }

    return CSimpleTranslationAnim::s_objectType;
}

#endif

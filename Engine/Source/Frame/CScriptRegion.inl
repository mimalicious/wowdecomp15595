#ifndef ENGINE_SOURCE_FRAME_CSCRIPTREGION_INL
#define ENGINE_SOURCE_FRAME_CSCRIPTREGION_INL

// INLINED
inline int32_t CScriptRegion::GetObjectType() {
    if (!CScriptRegion::s_objectType) {
        CScriptRegion::s_objectType = ++FrameScript_Object::s_objectTypes;
    }

    return CScriptRegion::s_objectType;
}

#endif

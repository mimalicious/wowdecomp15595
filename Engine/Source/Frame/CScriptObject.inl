#ifndef ENGINE_SOURCE_FRAME_CSCRIPTOBJECT_INL
#define ENGINE_SOURCE_FRAME_CSCRIPTOBJECT_INL

// INLINED
inline int32_t CScriptObject::GetObjectType() {
    if (!CScriptObject::s_objectType) {
        CScriptObject::s_objectType = ++FrameScript_Object::s_objectTypes;
    }

    return CScriptObject::s_objectType;
}

#endif

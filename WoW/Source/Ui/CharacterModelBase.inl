#ifndef WOW_SOURCE_UI_CHARACTERMODELBASE_INL
#define WOW_SOURCE_UI_CHARACTERMODELBASE_INL

// INLINED
inline int32_t CGCharacterModelBase::GetObjectType() {
    if (!CGCharacterModelBase::s_objectType) {
        CGCharacterModelBase::s_objectType = ++FrameScript_Object::s_objectTypes;
    }

    return CGCharacterModelBase::s_objectType;
}

#endif

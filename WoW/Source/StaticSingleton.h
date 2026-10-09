#ifndef WOW_SOURCE_STATICSINGLETON_H
#define WOW_SOURCE_STATICSINGLETON_H

template <class T>
class StaticSingleton {
    public:
    static T* m_instance;

    static T* Instance();
};

#include "WoW/Source/StaticSingleton.inl"

#endif

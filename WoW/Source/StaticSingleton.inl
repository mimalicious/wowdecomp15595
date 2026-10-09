// OFFSET: [0x00402FB0/0x140002550<CCameraManager>]
template <class T>
T* StaticSingleton<T>::Instance() {
    if (!StaticSingleton<T>::m_instance) {
        StaticSingleton<T>::m_instance = new T;
    }

    return StaticSingleton<T>::m_instance;
}

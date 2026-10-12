#ifndef WOW_SOURCE_SOUNDINTERFACE_SI3ZONESOUNDS_INL
#define WOW_SOURCE_SOUNDINTERFACE_SI3ZONESOUNDS_INL

// INLINED
inline void CZoneSoundManager::Stop() {
    SE3::StopSound(this->m_handle, -1, __FILE__, __LINE__, "CZoneSoundManager::Stop");
}

#endif

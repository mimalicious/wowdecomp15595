#ifndef ENGINE_SOURCE_MODEL2_M2MODEL_INL
#define ENGINE_SOURCE_MODEL2_M2MODEL_INL

// OFFSET: 0x0043B930
inline void CM2Model::SetWorldTransform(const C44Matrix& matrix) {
    this->m_fFlags |= 0x4000;
    this->m_worldTransform = matrix;
}

#endif

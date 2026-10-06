inline CTexture** SMOMaterial::Textures() {
    return reinterpret_cast<CTexture**>(&this->m_runTimeData[4 - 2 * sizeof(CTexture*) / sizeof(uint32_t)]);
}

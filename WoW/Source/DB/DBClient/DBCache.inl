// INLINED
inline uint32_t DBCacheHashKey(int32_t key) {
    return key;
}

// INLINED
inline uint32_t DBCacheHashKey(uint64_t key) {
    return static_cast<uint32_t>(key);
}

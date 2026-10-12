// INLINED
inline C4Quaternion::C4Quaternion() {
}

// INLINED
inline C4Quaternion operator*(const C4Quaternion& l, const C4Quaternion& r) {
    C4Quaternion result;
    result.x = l.w * r.x + r.z * l.y + r.w * l.x - l.z * r.y;
    result.y = r.w * l.y + l.w * r.y + l.z * r.x - r.z * l.x;
    result.z = r.z * l.w + r.w * l.z + l.x * r.y - r.x * l.y;
    result.w = r.w * l.w - l.x * r.x - l.y * r.y - r.z * l.z;
    return result;
}

// INLINED
inline C3Vector::C3Vector() {
    this->x = 0.0f;
    this->y = 0.0f;
    this->z = 0.0f;
}

// INLINED
inline C3Vector::C3Vector(float x, float y, float z) {
    this->x = x;
    this->y = y;
    this->z = z;
}

// OFFSET: #0x004C1D20
inline C3Vector C3Vector::operator-() const {
    C3Vector result;
    result.x = -this->x;
    result.y = -this->y;
    result.z = -this->z;
    return result;
}

// OFFSET: #0x00407990
inline C3Vector operator+(const C3Vector& l, const C3Vector& r) {
    C3Vector result;
    result.x = l.x + r.x;
    result.y = l.y + r.y;
    result.z = l.z + r.z;
    return result;
}

// OFFSET: #0x00482970
inline C3Vector& C3Vector::operator+=(const C3Vector& v) {
    this->x = v.x + this->x;
    this->y = v.y + this->y;
    this->z = v.z + this->z;
    return *this;
}

// OFFSET: #0x004829A0
inline C3Vector operator-(const C3Vector& l, const C3Vector& r) {
    C3Vector result;
    result.x = l.x - r.x;
    result.y = l.y - r.y;
    result.z = l.z - r.z;
    return result;
}

// OFFSET: #0x004C35A0
inline C3Vector& C3Vector::operator*=(float s) {
    this->x = this->x * s;
    this->y = this->y * s;
    this->z = s * this->z;
    return *this;
}

// OFFSET: #0x004C35D0
inline C3Vector& C3Vector::operator/=(float s) {
    float scale = 1.0f / s;
    this->x = this->x * scale;
    this->y = scale * this->y;
    this->z = scale * this->z;
    return *this;
}

// OFFSET: #0x004BEC40
inline C3Vector operator*(const C3Vector& l, float r) {
    C3Vector result;
    result.x = l.x * r;
    result.y = l.y * r;
    result.z = r * l.z;
    return result;
}

// OFFSET: #0x00568680
inline C3Vector& C3Vector::operator-=(const C3Vector& v) {
    this->x = this->x - v.x;
    this->y = this->y - v.y;
    this->z = this->z - v.z;
    return *this;
}

// INLINED
inline float C3Vector::SquaredMag() const {
    return this->y * this->y + this->x * this->x + this->z * this->z;
}

// OFFSET: #0x00714D10
inline C3Vector C3Vector::Min(const C3Vector& a, const C3Vector& b) {
    return C3Vector(a.x < b.x ? a.x : b.x, a.y < b.y ? a.y : b.y, a.z < b.z ? a.z : b.z);
}

// OFFSET: #0x00714D70
inline C3Vector C3Vector::Max(const C3Vector& a, const C3Vector& b) {
    return C3Vector(a.x > b.x ? a.x : b.x, a.y > b.y ? a.y : b.y, a.z > b.z ? a.z : b.z);
}

// PROVEN: a0b58707 EQUIVALENT/NONE
// OFFSET: 0x00431D10
inline void C3Vector::Normalize() {
    float scale = 1.0f / sqrt(this->z * this->z + this->y * this->y + this->x * this->x);
    this->x = this->x * scale;
    this->y = this->y * scale;
    this->z = scale * this->z;
}

// OFFSET: 0x005EE500
inline C3Vector C3Vector::Normalized() const {
    float scale = 1.0f / sqrt(this->x * this->x + this->y * this->y + this->z * this->z);
    return C3Vector(this->x * scale, this->y * scale, this->z * scale);
}

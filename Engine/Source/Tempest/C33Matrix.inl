// INLINED
inline C33Matrix::C33Matrix() {
}

// INLINED
inline void C33Matrix::Scale(float x, float y, float z) {
    this->a0 = this->a0 * x;
    this->a1 = this->a1 * x;
    this->a2 = x * this->a2;
    this->b0 = this->b0 * y;
    this->b1 = this->b1 * y;
    this->b2 = y * this->b2;
    this->c0 = this->c0 * z;
    this->c1 = this->c1 * z;
    this->c2 = z * this->c2;
}

// INLINED
inline C33Matrix::C33Matrix(float a0, float a1, float a2, float b0, float b1, float b2, float c0, float c1, float c2) {
    this->a0 = a0;
    this->a1 = a1;
    this->a2 = a2;
    this->b0 = b0;
    this->b1 = b1;
    this->b2 = b2;
    this->c0 = c0;
    this->c1 = c1;
    this->c2 = c2;
}

// INLINED
inline C3Vector operator*(const C3Vector& v, const C33Matrix& m) {
    C3Vector result;
    result.x = m.b0 * v.y + m.c0 * v.z + v.x * m.a0;
    result.y = m.b1 * v.y + m.a1 * v.x + m.c1 * v.z;
    result.z = m.b2 * v.y + m.a2 * v.x + m.c2 * v.z;
    return result;
}

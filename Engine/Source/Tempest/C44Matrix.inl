// PROVEN: 50d4481f IDENTICAL/NONE
// OFFSET: 0x0043B6C0
inline C44Matrix::C44Matrix(const C34Matrix& m) {
    this->a0 = m.a0;
    this->a1 = m.a1;
    this->a2 = m.a2;
    this->a3 = 0.0f;
    this->b0 = m.b0;
    this->b1 = m.b1;
    this->b2 = m.b2;
    this->b3 = 0.0f;
    this->c0 = m.c0;
    this->c1 = m.c1;
    this->c2 = m.c2;
    this->c3 = 0.0f;
    this->d0 = m.d0;
    this->d1 = m.d1;
    this->d2 = m.d2;
    this->d3 = 1.0f;
}

// PROVEN: ae04ebbd IDENTICAL/NONE
// OFFSET: 0x00435440
inline C44Matrix C44Matrix::Inverse() const {
    return this->Inverse(this->Determinant());
}

inline void C44Matrix::AffineInvertInPlace() {
    float t = this->a1;
    this->a1 = this->b0;
    this->b0 = t;

    t = this->a2;
    this->a2 = this->c0;
    this->c0 = t;

    t = this->b2;
    this->b2 = this->c1;
    this->c1 = t;

    this->InvertTranslation();
}

// INLINED
inline CAaBox::CAaBox(const C3Vector& b, const C3Vector& t) : b(b), t(t) {
}

// INLINED
inline CAaBox::CAaBox() {
}

// INLINED
inline bool CAaBox::IsValid() const {
    return this->t.x > this->b.x && this->t.y > this->b.y && this->t.z > this->b.z;
}

// INLINED
inline bool CAaBox::ContainsPoint(const C3Vector& point) const {
    return this->b.x <= point.x && this->b.y <= point.y && this->b.z <= point.z && this->t.x >= point.x && this->t.y >= point.y && this->t.z >= point.z;
}

// INLINED
inline CAaBox& CAaBox::operator*=(float scale) {
    this->b.x = this->b.x * scale;
    this->b.y = this->b.y * scale;
    this->b.z = this->b.z * scale;
    this->t.x = this->t.x * scale;
    this->t.y = this->t.y * scale;
    this->t.z = scale * this->t.z;
    return *this;
}

// INLINED
inline void CAaBox::Expand(float amount) {
    this->b.x = this->b.x - amount;
    this->b.y = this->b.y - amount;
    this->b.z = this->b.z - amount;
    this->t.x = this->t.x + amount;
    this->t.y = this->t.y + amount;
    this->t.z = amount + this->t.z;
}

// INLINED
inline CAaBox::CAaBox(const C3Vector& point) {
    this->b = point;
    this->t = point;
}

// INLINED
inline void CAaBox::Expand(const C3Vector& amount) {
    this->b.x -= amount.x;
    this->b.y -= amount.y;
    this->b.z -= amount.z;
    this->t.x += amount.x;
    this->t.y += amount.y;
    this->t.z += amount.z;
}

// INLINED
inline C3Vector CAaBox::Center() const {
    float y = this->t.y + this->b.y;
    float z = this->t.z + this->b.z;
    C3Vector center;
    center.x = (this->t.x + this->b.x) * 0.5f;
    center.y = y * 0.5f;
    center.z = 0.5f * z;
    return center;
}

// INLINED
inline void CAaBox::operator+=(const C3Vector& v) {
    this->b.x = v.x + this->b.x;
    this->b.y = this->b.y + v.y;
    this->b.z = this->b.z + v.z;
    this->t.x = this->t.x + v.x;
    this->t.y = this->t.y + v.y;
    this->t.z = this->t.z + v.z;
}

// INLINED
inline bool CAaBox::IsInverted() const {
    return this->b.x > this->t.x && this->b.y > this->t.y && this->b.z > this->t.z;
}

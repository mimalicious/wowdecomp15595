// INLINED
inline GeneralFunction::Arg::Arg() {
    this->m_type = TYPE_NONE;
}

// INLINED
inline GeneralFunction::Arg::Arg(const Arg& other) {
    this->m_type = TYPE_NONE;
    *this = other;
}

// INLINED
inline GeneralFunction::Arg::~Arg() {
    this->FreeData();
}

// INLINED
inline void GeneralFunction::Arg::FreeData() {
    if (this->m_type == TYPE_OBJECT) {
        this->m_object->DecRef();
    }

    if (this->m_type == TYPE_CALLBACK) {
        this->m_callback->DecRef();
    }

    this->m_type = TYPE_NONE;
}

// INLINED
inline const char* GeneralFunction::Arg::ToString() const {
    if (this->m_type == TYPE_STRING) {
        return this->m_string;
    }

    if (this->m_type == TYPE_FOURCC) {
        return this->m_fourCC;
    }

    return NULL;
}

// INLINED
inline bool GeneralFunction::Arg::ToBool() const {
    if (this->m_type == TYPE_BOOL) {
        return this->m_bool;
    }

    return this->ToUInt64() != 0;
}

// INLINED
inline GeneralFunction::Arg& GeneralFunction::Arg::operator=(bool value) {
    this->FreeData();
    this->m_type = TYPE_BOOL;
    this->m_bool = value;
    return *this;
}

// INLINED
inline GeneralFunction::Arg& GeneralFunction::Arg::operator=(int64_t value) {
    this->FreeData();
    this->m_type = TYPE_INT;
    this->m_int = value;
    return *this;
}

// INLINED
inline GeneralFunction::Arg& GeneralFunction::Arg::operator=(uint64_t value) {
    this->FreeData();
    this->m_type = TYPE_UINT;
    this->m_uint = value;
    return *this;
}

// INLINED
inline GeneralFunction::Arg& GeneralFunction::Arg::operator=(const char* value) {
    this->FreeData();
    this->m_type = TYPE_STRING;
    this->m_string = value;
    return *this;
}

// INLINED
inline GeneralFunction::Arg& GeneralFunction::Arg::operator=(const BSN::EncodedObject* value) {
    this->FreeData();
    this->m_type = TYPE_OBJECT;
    this->m_object = value;
    value->IncRef();
    return *this;
}

// INLINED
inline GeneralFunction::Arg& GeneralFunction::Arg::operator=(Callback* value) {
    this->FreeData();
    this->m_type = TYPE_CALLBACK;
    this->m_callback = value;
    value->IncRef();
    return *this;
}

// INLINED
inline const BSN::EncodedObject* GeneralFunction::Arg::ToObject() const {
    if (this->m_type == TYPE_OBJECT) {
        return this->m_object;
    }

    return NULL;
}

// INLINED
inline GeneralFunction::Callback::Callback() {
    this->m_refCount = 0;
}

// INLINED
inline GeneralFunction::SimpleCallback::SimpleCallback(SimpleCallbackFunc func, void* param) {
    this->m_func = func;
    this->m_param = param;
}

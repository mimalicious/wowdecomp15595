#include "WoW/Source/Battlenet/GeneralFunction.h"
#include <cstdlib>

// INLINED
GeneralFunction::Arg::Arg() {
    this->m_type = TYPE_NONE;
}

// INLINED
GeneralFunction::Arg::Arg(const Arg& other) {
    this->m_type = TYPE_NONE;
    *this = other;
}

// INLINED
GeneralFunction::Arg::~Arg() {
    this->FreeData();
}

// INLINED
void GeneralFunction::Arg::FreeData() {
    if (this->m_type == TYPE_OBJECT) {
        this->m_object->DecRef();
    }

    if (this->m_type == TYPE_CALLBACK) {
        this->m_callback->DecRef();
    }

    this->m_type = TYPE_NONE;
}

// OFFSET: #0x005311E0
GeneralFunction::Arg& GeneralFunction::Arg::operator=(const Arg& other) {
    this->FreeData();
    this->m_type = other.m_type;

    switch (other.m_type) {
        case TYPE_BOOL:
            this->m_bool = other.m_bool;
            break;
        case TYPE_INT:
        case TYPE_UINT:
            this->m_uint = other.m_uint;
            break;
        case TYPE_REAL:
            this->m_real = other.m_real;
            break;
        case TYPE_FOURCC:
            this->m_fourCC[0] = other.m_fourCC[0];
            this->m_fourCC[1] = other.m_fourCC[1];
            this->m_fourCC[2] = other.m_fourCC[2];
            this->m_fourCC[3] = other.m_fourCC[3];
            this->m_fourCC[4] = other.m_fourCC[4];
            break;
        case TYPE_STRING:
            this->m_string = other.m_string;
            break;
        case TYPE_OBJECT:
            this->m_object = other.m_object;
            this->m_object->IncRef();
            break;
        case TYPE_CALLBACK:
            this->m_callback = other.m_callback;
            this->m_callback->IncRef();
            break;
        default:
            break;
    }

    return *this;
}

// OFFSET: #0x00530BE0
int64_t GeneralFunction::Arg::ToInt64() const {
    switch (this->m_type) {
        case TYPE_BOOL:
            return this->m_bool;
        case TYPE_INT:
        case TYPE_UINT:
            return this->m_int;
        case TYPE_REAL:
            return static_cast<int64_t>(this->m_real);
        case TYPE_FOURCC: {
            uint32_t value = 0;

            for (const char* c = this->m_fourCC; *c; c++) {
                value = (value << 8) | *c;
            }

            return value;
        }
        case TYPE_STRING:
            return _strtoi64(this->m_string, NULL, 0);
        default:
            return 0;
    }
}

// OFFSET: #0x00530C60
uint64_t GeneralFunction::Arg::ToUInt64() const {
    switch (this->m_type) {
        case TYPE_BOOL:
            return this->m_bool;
        case TYPE_INT:
        case TYPE_UINT:
            return this->m_uint;
        case TYPE_REAL:
            return static_cast<uint64_t>(this->m_real);
        case TYPE_FOURCC: {
            uint32_t value = 0;

            for (const char* c = this->m_fourCC; *c; c++) {
                value = (value << 8) | *c;
            }

            return value;
        }
        case TYPE_STRING:
            return _strtoui64(this->m_string, NULL, 0);
        default:
            return 0;
    }
}

// INLINED
const char* GeneralFunction::Arg::ToString() const {
    if (this->m_type == TYPE_STRING) {
        return this->m_string;
    }

    if (this->m_type == TYPE_FOURCC) {
        return this->m_fourCC;
    }

    return NULL;
}

// INLINED
bool GeneralFunction::Arg::ToBool() const {
    if (this->m_type == TYPE_BOOL) {
        return this->m_bool;
    }

    return this->ToUInt64() != 0;
}

// INLINED
GeneralFunction::Arg& GeneralFunction::Arg::operator=(bool value) {
    this->FreeData();
    this->m_type = TYPE_BOOL;
    this->m_bool = value;
    return *this;
}

// INLINED
GeneralFunction::Arg& GeneralFunction::Arg::operator=(int64_t value) {
    this->FreeData();
    this->m_type = TYPE_INT;
    this->m_int = value;
    return *this;
}

// INLINED
GeneralFunction::Arg& GeneralFunction::Arg::operator=(uint64_t value) {
    this->FreeData();
    this->m_type = TYPE_UINT;
    this->m_uint = value;
    return *this;
}

// INLINED
GeneralFunction::Arg& GeneralFunction::Arg::operator=(const char* value) {
    this->FreeData();
    this->m_type = TYPE_STRING;
    this->m_string = value;
    return *this;
}

// INLINED
GeneralFunction::Arg& GeneralFunction::Arg::operator=(const BSN::EncodedObject* value) {
    this->FreeData();
    this->m_type = TYPE_OBJECT;
    this->m_object = value;
    value->IncRef();
    return *this;
}

// INLINED
GeneralFunction::Arg& GeneralFunction::Arg::operator=(Callback* value) {
    this->FreeData();
    this->m_type = TYPE_CALLBACK;
    this->m_callback = value;
    value->IncRef();
    return *this;
}

// INLINED
const BSN::EncodedObject* GeneralFunction::Arg::ToObject() const {
    if (this->m_type == TYPE_OBJECT) {
        return this->m_object;
    }

    return NULL;
}

// INLINED
GeneralFunction::Callback::Callback() {
    this->m_refCount = 0;
}

// INLINED
GeneralFunction::SimpleCallback::SimpleCallback(SimpleCallbackFunc func, void* param) {
    this->m_func = func;
    this->m_param = param;
}

// OFFSET: #0x00530B90
GeneralFunction::Callback::~Callback() {
}

// PROVEN: 429efe43 IDENTICAL
// OFFSET: 0x00627270
void GeneralFunction::Callback::IncRef() const {
    const_cast<Callback*>(this)->m_refCount++;
}

// OFFSET: #0x00530B80
void GeneralFunction::Callback::DecRef() const {
    if (const_cast<Callback*>(this)->m_refCount-- == 1) {
        this->Destroy();
    }
}

// OFFSET: #0x00530BC0
void GeneralFunction::SimpleCallback::Call(int32_t argc, const Arg* args) const {
    this->m_func(this->m_param, argc, args);
}

// OFFSET: #0x00532950
void GeneralFunction::SimpleCallback::Destroy() const {
    delete this;
}

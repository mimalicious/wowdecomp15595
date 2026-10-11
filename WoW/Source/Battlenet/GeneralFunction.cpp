#include "WoW/Source/Battlenet/GeneralFunction.h"
#include <cstdlib>

// OFFSET: 0x00660940/0x140305F30
GeneralFunction::Arg& GeneralFunction::Arg::operator=(const Arg& other) {
    this->FreeData();
    this->m_type = other.m_type;

    switch (this->m_type) {
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

// OFFSET: 0x00902640
GeneralFunction::Arg& GeneralFunction::Arg::operator=(const BSN::FourCC& value) {
    this->FreeData();
    this->m_type = TYPE_FOURCC;
    value.ToString(this->m_fourCC, sizeof(this->m_fourCC));
    return *this;
}

// PROVEN: 6c736636 IDENTICAL/NONE
// OFFSET: 0x00902070/0x14065A650
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

// PROVEN: d733f862 IDENTICAL/NONE
// OFFSET: 0x00660600/0x140305C90
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

// OFFSET: 0x00902100/0x14065A6D0
GeneralFunction::Callback::~Callback() {
}

// PROVEN: 429efe43 IDENTICAL/NONE
// OFFSET: 0x00627270/0x1402B9280
void GeneralFunction::Callback::IncRef() const {
    const_cast<Callback*>(this)->m_refCount++;
}

// OFFSET: 0x00902040/0x14065A620
void GeneralFunction::Callback::DecRef() const {
    if (--const_cast<Callback*>(this)->m_refCount == 0) {
        this->Destroy();
    }
}

// PROVEN: 8ba953dc IDENTICAL/NONE
// OFFSET: 0x00902050/0x14065A640
void GeneralFunction::SimpleCallback::Call(int32_t argc, const Arg* args) const {
    this->m_func(this->m_param, argc, args);
}

// OFFSET: 0x00904350
void GeneralFunction::SimpleCallback::Destroy() const {
    delete this;
}

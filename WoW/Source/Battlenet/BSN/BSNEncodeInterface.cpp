#include "WoW/Source/Battlenet/BSN/BSNEncodeInterface.h"
#include <cstring>
#include <malloc.h>

// OFFSET: 0x006272C0
void BSN::EncodeContext::ResolveType() {
    if (!this->m_protocol || !this->m_typeId) {
        return;
    }

    this->m_header = BSN::Protocol::Meta::Base(this->m_protocol, this->m_typeId).m_header & 0x3F;

    while (this->m_header == BSN::META_USER) {
        this->m_typeId = BSN::Protocol::Meta::User(this->m_protocol, this->m_typeId).m_userTypeId;
        this->m_header = BSN::Protocol::Meta::Base(this->m_protocol, this->m_typeId).m_header & 0x3F;
    }
}

// OFFSET: 0x00627DE0/0x1402B9120
BSN::PrimitiveEncoder::~PrimitiveEncoder() {
}

// PROVEN: 4d58ca3e IDENTICAL/NONE
// OFFSET: 0x00664290/0x14030A4B0
uint32_t BSN::BSNPrimitiveEncoder::GetContextSize() const {
    return sizeof(EncodeContext);
}

// OFFSET: 0x00664660/0x14030A850
BSN::EncodeContext* BSN::BSNPrimitiveEncoder::ConstructContext(void* memory) const {
    if (!memory) {
        return NULL;
    }

    EncodeContext* context = static_cast<EncodeContext*>(memory);
    context->m_protocol = NULL;
    context->m_typeId = 0;
    context->m_data = context;
    context->ResolveType();
    return context;
}

// PROVEN: 2b1d3d87 IDENTICAL/IDENTICAL
// OFFSET: 0x00847950/0x1407D7E80
void BSN::BSNPrimitiveEncoder::DestroyContext(EncodeContext* context) const {
}

// PROVEN: 8df54369 IDENTICAL/NONE
// OFFSET: 0x006642A0/0x14030A4C0
uint64_t BSN::BSNPrimitiveEncoder::GetUInt(const EncodeContext* context) const {
    uint64_t value;

    if (context->m_header == BSN::META_INT) {
        BSN::Protocol::Meta::Int meta(context->m_protocol, context->m_typeId);
        BSN::Cpp::ReadInt(context->m_data, static_cast<uint32_t>(reinterpret_cast<uintptr_t>(meta.m_codeInfo[0])), (meta.m_bounds.m_intType & 1) == 0, value);
        return value;
    }

    if (context->m_header == BSN::META_ENUM) {
        BSN::Protocol::Meta::Enum meta(context->m_protocol, context->m_typeId);
        BSN::Cpp::ReadInt(context->m_data, static_cast<uint32_t>(reinterpret_cast<uintptr_t>(meta.m_codeInfo[0])), (meta.m_bounds.m_intType & 1) == 0, value);
        return value;
    }

    return 0;
}

// PROVEN: 52939718 IDENTICAL/NONE
// OFFSET: 0x00664340/0x14030A540
int64_t BSN::BSNPrimitiveEncoder::GetInt(const EncodeContext* context) const {
    return this->GetUInt(context);
}

// OFFSET: 0x00664350/0x14030A550
bool BSN::BSNPrimitiveEncoder::GetBool(const EncodeContext* context) const {
    if (context->m_header != BSN::META_BOOL) {
        return false;
    }

    BSN::Protocol::Meta::Bool meta(context->m_protocol, context->m_typeId);
    uint64_t value;
    BSN::Cpp::ReadInt(context->m_data, static_cast<uint32_t>(reinterpret_cast<uintptr_t>(meta.m_codeInfo[0])), false, value);
    return value != 0;
}

// PROVEN: c50f5bd8 IDENTICAL/NONE
// OFFSET: 0x006643B0/0x14030A5B0
double BSN::BSNPrimitiveEncoder::GetReal(const EncodeContext* context) const {
    if (context->m_header == BSN::META_REAL32) {
        BSN::Protocol::Meta::Real32 meta(context->m_protocol, context->m_typeId);
        return *static_cast<const float*>(context->m_data);
    }

    if (context->m_header == BSN::META_REAL64) {
        BSN::Protocol::Meta::Real64 meta(context->m_protocol, context->m_typeId);
        return *static_cast<const double*>(context->m_data);
    }

    return 0.0;
}

// OFFSET: 0x00664690/0x14030A890
uint32_t BSN::BSNPrimitiveEncoder::GetFourCC(const EncodeContext* context) const {
    if (context->m_header != BSN::META_FOURCC) {
        return 0;
    }

    BSN::Protocol::Meta::FourCC meta(context->m_protocol, context->m_typeId);
    return reinterpret_cast<AdapterGetFourCC>(meta.m_codeInfo[0])(context->m_data);
}

// OFFSET: 0x006646D0/0x14030A8D0
uint32_t BSN::BSNPrimitiveEncoder::GetStringLength(const EncodeContext* context) const {
    if (context->m_header == BSN::META_ASCII_STRING) {
        BSN::Protocol::Meta::AsciiString meta(context->m_protocol, context->m_typeId);
        return reinterpret_cast<AdapterGetLength>(meta.m_codeInfo[0])(context->m_data);
    }

    if (context->m_header == BSN::META_STRING) {
        BSN::Protocol::Meta::String meta(context->m_protocol, context->m_typeId);
        return reinterpret_cast<AdapterGetLength>(meta.m_codeInfo[0])(context->m_data);
    }

    return 0;
}

// OFFSET: 0x00664740/0x14030A950
bool BSN::BSNPrimitiveEncoder::GetString(const EncodeContext* context, char* buffer, uint32_t size) const {
    if (context->m_header == BSN::META_ASCII_STRING) {
        BSN::Protocol::Meta::AsciiString meta(context->m_protocol, context->m_typeId);
        uint32_t length = reinterpret_cast<AdapterGetLength>(meta.m_codeInfo[0])(context->m_data);

        if (length >= size) {
            length = size - 1;
        }

        memcpy(buffer, reinterpret_cast<AdapterGetElements>(meta.m_codeInfo[1])(context->m_data), length);
        buffer[length] = '\0';
        return true;
    }

    if (context->m_header == BSN::META_STRING) {
        BSN::Protocol::Meta::String meta(context->m_protocol, context->m_typeId);
        uint32_t length = reinterpret_cast<AdapterGetLength>(meta.m_codeInfo[0])(context->m_data);

        if (length >= size) {
            length = size - 1;
        }

        memcpy(buffer, reinterpret_cast<AdapterGetElements>(meta.m_codeInfo[1])(context->m_data), length);
        buffer[length] = '\0';
        return true;
    }

    *buffer = '\0';
    return false;
}

// OFFSET: 0x00664810/0x14030AA00
uint32_t BSN::BSNPrimitiveEncoder::GetBitArrayLength(const EncodeContext* context) const {
    if (context->m_header != BSN::META_BIT_ARRAY) {
        return 0;
    }

    BSN::Protocol::Meta::BitArray meta(context->m_protocol, context->m_typeId);
    return reinterpret_cast<AdapterGetLength>(meta.m_codeInfo[0])(context->m_data);
}

// OFFSET: 0x00664850/0x14030AA40
bool BSN::BSNPrimitiveEncoder::GetBitArray(const EncodeContext* context, uint8_t* buffer, uint32_t bits) const {
    if (context->m_header != BSN::META_BIT_ARRAY) {
        return false;
    }

    BSN::Protocol::Meta::BitArray meta(context->m_protocol, context->m_typeId);
    uint32_t length = reinterpret_cast<AdapterGetLength>(meta.m_codeInfo[0])(context->m_data);

    if (length > bits) {
        length = bits;
    }

    memcpy(buffer, reinterpret_cast<AdapterGetElements>(meta.m_codeInfo[1])(context->m_data), (length + 7) >> 3);
    return true;
}

// OFFSET: 0x006648C0/0x14030AAE0
uint32_t BSN::BSNPrimitiveEncoder::GetBlobLength(const EncodeContext* context) const {
    if (context->m_header != BSN::META_BLOB) {
        return 0;
    }

    BSN::Protocol::Meta::Blob meta(context->m_protocol, context->m_typeId);
    return reinterpret_cast<AdapterGetLength>(meta.m_codeInfo[0])(context->m_data);
}

// OFFSET: 0x00664900/0x14030AB20
bool BSN::BSNPrimitiveEncoder::GetBlob(const EncodeContext* context, uint8_t* buffer, uint32_t size) const {
    if (context->m_header != BSN::META_BLOB) {
        return false;
    }

    BSN::Protocol::Meta::Blob meta(context->m_protocol, context->m_typeId);
    uint32_t length = reinterpret_cast<AdapterGetLength>(meta.m_codeInfo[0])(context->m_data);

    if (length > size) {
        length = size;
    }

    memcpy(buffer, reinterpret_cast<AdapterGetElements>(meta.m_codeInfo[1])(context->m_data), length);
    return true;
}

// OFFSET: 0x00664970/0x14030ABC0
bool BSN::BSNPrimitiveEncoder::GetMemberContext(const EncodeContext* context, const char* name, EncodeContext* member) const {
    member->m_protocol = context->m_protocol;

    if (context->m_header == BSN::META_CHOICE) {
        BSN::Protocol::Meta::Choice meta(context->m_protocol, context->m_typeId);
        uint32_t count = meta.m_count;

        if (!count) {
            return false;
        }

        uint32_t field = 0;

        while (strcmp(meta.m_protocol->DumpString(meta.m_dumpInfo, field + 1), name)) {
            if (++field >= count) {
                return false;
            }
        }

        if (!meta.GetFieldTypeId(field, member->m_typeId)) {
            return false;
        }

        member->ResolveType();
        member->m_data = reinterpret_cast<AdapterGetChoice>(meta.m_codeInfo[2])(context->m_data);
        return true;
    }

    if (context->m_header == BSN::META_OPTIONAL) {
        BSN::Protocol::Meta::Optional meta(context->m_protocol, context->m_typeId);

        if (!reinterpret_cast<AdapterGetExists>(meta.m_codeInfo[0])(context->m_data)) {
            return false;
        }

        member->m_typeId = meta.m_elementTypeId;
        member->ResolveType();
        member->m_data = const_cast<void*>(reinterpret_cast<AdapterGetElements>(meta.m_codeInfo[1])(context->m_data));
        return true;
    }

    if (context->m_header == BSN::META_STRUCT) {
        BSN::Protocol::Meta::Struct meta(context->m_protocol, context->m_typeId);
        uint32_t count = meta.m_count;

        if (!count) {
            return false;
        }

        uint32_t index = 0;

        while (strcmp(meta.m_protocol->DumpString(meta.m_dumpInfo, index + 1), name)) {
            if (++index >= count) {
                return false;
            }
        }

        uint32_t offset;

        if (!meta.GetMember(index, offset, member->m_typeId)) {
            return false;
        }

        member->ResolveType();
        member->m_data = static_cast<uint8_t*>(context->m_data) + offset;
        return true;
    }

    return false;
}

// OFFSET: 0x00664B60/0x14030ADB0
bool BSN::BSNPrimitiveEncoder::GetElementContext(const EncodeContext* context, uint32_t index, EncodeContext* element) const {
    element->m_protocol = context->m_protocol;

    if (context->m_header != BSN::META_ARRAY) {
        return false;
    }

    BSN::Protocol::Meta::Array meta(context->m_protocol, context->m_typeId);

    if (index >= reinterpret_cast<AdapterGetLength>(meta.m_codeInfo[1])(context->m_data)) {
        return false;
    }

    element->m_typeId = meta.m_elementTypeId;
    element->ResolveType();

    uint32_t elementSize = static_cast<uint32_t>(reinterpret_cast<uintptr_t>(meta.m_codeInfo[0]));
    element->m_data = const_cast<uint8_t*>(static_cast<const uint8_t*>(reinterpret_cast<AdapterGetElements>(meta.m_codeInfo[2])(context->m_data))) + index * elementSize;
    return true;
}

// OFFSET: 0x00664BE0/0x14030AE60
bool BSN::BSNPrimitiveEncoder::GetChoiceName(const EncodeContext* context, char* buffer, uint32_t size) const {
    if (context->m_header == BSN::META_CHOICE) {
        BSN::Protocol::Meta::Choice meta(context->m_protocol, context->m_typeId);
        int32_t tag = reinterpret_cast<AdapterGetTag>(meta.m_codeInfo[1])(context->m_data);
        uint32_t field;

        if (meta.GetTagField(static_cast<int64_t>(tag), field)) {
            const char* name = meta.m_protocol->DumpString(meta.m_dumpInfo, field + 1);
            uint32_t length = static_cast<uint32_t>(strlen(name));

            if (length >= size) {
                length = size - 1;
            }

            memcpy(buffer, name, length);
            buffer[length] = '\0';
            return true;
        }
    }

    *buffer = '\0';
    return false;
}

// OFFSET: 0x00664C80/0x14030AF30
uint32_t BSN::BSNPrimitiveEncoder::GetArrayLength(const EncodeContext* context) const {
    if (context->m_header != BSN::META_ARRAY) {
        return 0;
    }

    BSN::Protocol::Meta::Array meta(context->m_protocol, context->m_typeId);
    return reinterpret_cast<AdapterGetLength>(meta.m_codeInfo[1])(context->m_data);
}

// OFFSET: 0x00664CC0/0x14030AF80
bool BSN::BSNPrimitiveEncoder::GetOptional(const EncodeContext* context) const {
    if (context->m_header != BSN::META_OPTIONAL) {
        return false;
    }

    BSN::Protocol::Meta::Optional meta(context->m_protocol, context->m_typeId);
    return reinterpret_cast<AdapterGetExists>(meta.m_codeInfo[0])(context->m_data);
}

// OFFSET: 0x00664410/0x14030A620
bool BSN::BSNPrimitiveEncoder::SetUInt(EncodeContext* context, uint64_t value) const {
    if (context->m_header == BSN::META_INT) {
        BSN::Protocol::Meta::Int meta(context->m_protocol, context->m_typeId);

        if (meta.m_bounds.Contains(value)) {
            BSN::Cpp::WriteInt(value, static_cast<uint32_t>(reinterpret_cast<uintptr_t>(meta.m_codeInfo[0])), context->m_data);
            return true;
        }
    }

    if (context->m_header == BSN::META_ENUM) {
        BSN::Protocol::Meta::Enum meta(context->m_protocol, context->m_typeId);
        uint32_t field;

        if (meta.GetEnumField(value, field) || (meta.m_bounds.m_variable && meta.m_bounds.Contains(value))) {
            BSN::Cpp::WriteInt(value, static_cast<uint32_t>(reinterpret_cast<uintptr_t>(meta.m_codeInfo[0])), context->m_data);
            return true;
        }
    }

    return false;
}

// PROVEN: a5b2c04d IDENTICAL/NONE
// OFFSET: 0x006644D0/0x14030A6E0
bool BSN::BSNPrimitiveEncoder::SetInt(EncodeContext* context, int64_t value) const {
    return this->SetUInt(context, value);
}

// OFFSET: 0x006644F0/0x14030A6F0
bool BSN::BSNPrimitiveEncoder::SetBool(EncodeContext* context, bool value) const {
    if (context->m_header != BSN::META_BOOL) {
        return false;
    }

    BSN::Protocol::Meta::Bool meta(context->m_protocol, context->m_typeId);
    BSN::Cpp::WriteInt(value, static_cast<uint32_t>(reinterpret_cast<uintptr_t>(meta.m_codeInfo[0])), context->m_data);
    return true;
}

// PROVEN: d5bda1d1 IDENTICAL/NONE
// OFFSET: 0x00664540/0x14030A750
bool BSN::BSNPrimitiveEncoder::SetReal(EncodeContext* context, double value) const {
    if (context->m_header == BSN::META_REAL32) {
        *static_cast<float*>(context->m_data) = static_cast<float>(value);
        return true;
    }

    if (context->m_header == BSN::META_REAL64) {
        *static_cast<double*>(context->m_data) = value;
        return true;
    }

    return false;
}

// OFFSET: 0x00664DF0/0x14030B0E0
bool BSN::BSNPrimitiveEncoder::SetFourCC(EncodeContext* context, uint32_t value) const {
    if (context->m_header != BSN::META_FOURCC) {
        return false;
    }

    BSN::Protocol::Meta::FourCC meta(context->m_protocol, context->m_typeId);
    reinterpret_cast<AdapterSetFourCC>(meta.m_codeInfo[1])(context->m_data, value);
    return true;
}

// OFFSET: 0x00664D00/0x14030AFC0
bool BSN::BSNPrimitiveEncoder::SetString(EncodeContext* context, const char* value) const {
    if (context->m_header == BSN::META_ASCII_STRING) {
        BSN::Protocol::Meta::AsciiString meta(context->m_protocol, context->m_typeId);
        uint32_t length = static_cast<uint32_t>(strlen(value));
        char* data = static_cast<char*>(reinterpret_cast<AdapterSetLength>(meta.m_codeInfo[2])(context->m_data, length));
        memcpy(data, value, length);
        data[length] = '\0';
        return true;
    }

    if (context->m_header == BSN::META_STRING) {
        BSN::Protocol::Meta::String meta(context->m_protocol, context->m_typeId);
        uint32_t length = static_cast<uint32_t>(strlen(value));
        char* data = static_cast<char*>(reinterpret_cast<AdapterSetLength>(meta.m_codeInfo[2])(context->m_data, length));
        memcpy(data, value, length);
        data[length] = '\0';

        uint32_t count;

        if (BSN::Protocol::Meta::String::IsValid(length, reinterpret_cast<uint8_t*>(data), count)) {
            reinterpret_cast<AdapterSetCount>(meta.m_codeInfo[3])(context->m_data, count);
            return true;
        }
    }

    return false;
}

// OFFSET: 0x00664E40/0x14030B130
bool BSN::BSNPrimitiveEncoder::SetBitArray(EncodeContext* context, const uint8_t* data, uint32_t bits) const {
    if (context->m_header != BSN::META_BIT_ARRAY) {
        return false;
    }

    BSN::Protocol::Meta::BitArray meta(context->m_protocol, context->m_typeId);

    if (!meta.m_bounds.Contains(bits)) {
        return false;
    }

    memcpy(reinterpret_cast<AdapterSetLength>(meta.m_codeInfo[2])(context->m_data, bits), data, (bits + 7) >> 3);
    return true;
}

// OFFSET: 0x00664EB0/0x14030B1C0
bool BSN::BSNPrimitiveEncoder::SetBlob(EncodeContext* context, const uint8_t* data, uint32_t size) const {
    if (context->m_header != BSN::META_BLOB) {
        return false;
    }

    BSN::Protocol::Meta::Blob meta(context->m_protocol, context->m_typeId);

    if (!meta.m_bounds.Contains(size)) {
        return false;
    }

    memcpy(reinterpret_cast<AdapterSetLength>(meta.m_codeInfo[2])(context->m_data, size), data, size);
    return true;
}

// OFFSET: 0x00664580/0x14030A780
bool BSN::BSNPrimitiveEncoder::SetStruct(EncodeContext* context, const char* name) const {
    return context->m_header == BSN::META_STRUCT;
}

// OFFSET: 0x00664F20/0x14030B250
bool BSN::BSNPrimitiveEncoder::SetChoice(EncodeContext* context, const char* typeName, const char* choiceName) const {
    if (context->m_header != BSN::META_CHOICE) {
        return false;
    }

    BSN::Protocol::Meta::Choice meta(context->m_protocol, context->m_typeId);
    uint32_t count = meta.m_count;

    if (!count) {
        return false;
    }

    uint32_t field = 0;

    while (strcmp(meta.m_protocol->DumpString(meta.m_dumpInfo, field + 1), choiceName)) {
        if (++field >= count) {
            return false;
        }
    }

    uint64_t tag;

    if (meta.GetFieldTag(field, tag)) {
        reinterpret_cast<AdapterSetTag>(meta.m_codeInfo[3])(context->m_data, static_cast<int32_t>(tag));
    }

    return true;
}

// OFFSET: 0x00664FD0/0x14030B320
bool BSN::BSNPrimitiveEncoder::SetArray(EncodeContext* context, uint32_t length) const {
    if (context->m_header != BSN::META_ARRAY) {
        return false;
    }

    BSN::Protocol::Meta::Array meta(context->m_protocol, context->m_typeId);

    if (!meta.m_bounds.Contains(length)) {
        return false;
    }

    reinterpret_cast<AdapterSetLength>(meta.m_codeInfo[3])(context->m_data, length);
    return true;
}

// OFFSET: 0x00665030/0x14030B390
bool BSN::BSNPrimitiveEncoder::SetOptional(EncodeContext* context, bool exists) const {
    if (context->m_header != BSN::META_OPTIONAL) {
        return false;
    }

    BSN::Protocol::Meta::Optional meta(context->m_protocol, context->m_typeId);
    reinterpret_cast<AdapterSetExists>(meta.m_codeInfo[2])(context->m_data, exists);
    return true;
}

// OFFSET: 0x006645A0/0x14030A790
bool BSN::BSNPrimitiveEncoder::WalkAndSet(const PrimitiveEncoder* encoder, EncodeContext* context, const EncodeContext* source) const {
    if (!source->m_protocol->m_dumpInfoSize) {
        return false;
    }

    BSNPrimitiveWalker::Context walkContext;
    walkContext.m_protocol = source->m_protocol;
    walkContext.m_typeId = source->m_typeId;
    walkContext.m_sourceEncoder = this;
    walkContext.m_sourceContext = source;
    walkContext.m_targetEncoder = encoder;
    walkContext.m_targetContext = context;

    BSNPrimitiveWalker walker;
    return walker.Copy(walkContext) == BSN::WALK_VALID;
}

// OFFSET: 0x00664600/0x14030A7F0
bool BSN::BSNPrimitiveEncoder::WalkAndGet(const PrimitiveEncoder* encoder, const EncodeContext* source, EncodeContext* context) const {
    if (!context->m_protocol->m_dumpInfoSize) {
        return false;
    }

    BSNPrimitiveWalker::Context walkContext;
    walkContext.m_protocol = context->m_protocol;
    walkContext.m_typeId = context->m_typeId;
    walkContext.m_sourceEncoder = encoder;
    walkContext.m_sourceContext = source;
    walkContext.m_targetEncoder = this;
    walkContext.m_targetContext = context;

    BSNPrimitiveWalker walker;
    return walker.Copy(walkContext) == BSN::WALK_VALID;
}

// OFFSET: #0x0077A200
BSN::BSNPrimitiveWalker::~BSNPrimitiveWalker() {
}

// PROVEN: 2bc4e6b0 IDENTICAL/NONE
// OFFSET: 0x00665D40/0x14030C300
int32_t BSN::BSNPrimitiveWalker::Copy(const Context& context) {
    return BSN::BSNPrimitiveWalker::Walk(*this, context);
}

// OFFSET: 0x00665D60/0x14030C310
int32_t BSN::BSNPrimitiveWalker::Array(const Protocol::Meta::Array& meta, const Context& context) {
    const PrimitiveEncoder* sourceEncoder = context.m_sourceEncoder;
    const EncodeContext* sourceContext = context.m_sourceContext;
    const PrimitiveEncoder* targetEncoder = context.m_targetEncoder;
    EncodeContext* targetContext = context.m_targetContext;
    int32_t result = BSN::WALK_VALID;

    void* sourceMemory = _alloca(sourceEncoder->GetContextSize());
    void* targetMemory = _alloca(targetEncoder->GetContextSize());

    uint32_t length = sourceEncoder->GetArrayLength(sourceContext);

    if (!targetEncoder->SetArray(targetContext, length)) {
        return BSN::WALK_INVALID;
    }

    uint32_t elementTypeId = meta.m_elementTypeId;

    for (uint32_t i = 0; i < length; i++) {
        if (result) {
            return result;
        }

        EncodeContext* sourceElement = sourceEncoder->ConstructContext(sourceMemory);
        EncodeContext* targetElement = targetEncoder->ConstructContext(targetMemory);

        if (sourceEncoder->GetElementContext(sourceContext, i, sourceElement) && targetEncoder->GetElementContext(targetContext, i, targetElement)) {
            Context element;
            element.m_protocol = context.m_protocol;
            element.m_typeId = elementTypeId;
            element.m_sourceEncoder = context.m_sourceEncoder;
            element.m_sourceContext = sourceElement;
            element.m_targetEncoder = context.m_targetEncoder;
            element.m_targetContext = targetElement;
            result = BSN::BSNPrimitiveWalker::Walk(*this, element);
        } else {
            result = BSN::WALK_INVALID;
        }

        sourceEncoder->DestroyContext(sourceElement);
        targetEncoder->DestroyContext(targetElement);
    }

    return result;
}

// OFFSET: 0x00665ED0/0x14030C590
int32_t BSN::BSNPrimitiveWalker::BitArray(const Protocol::Meta::BitArray& meta, const Context& context) {
    uint32_t bits = context.m_sourceEncoder->GetBitArrayLength(context.m_sourceContext);
    uint8_t* buffer = static_cast<uint8_t*>(_alloca((bits + 7) >> 3));
    return !context.m_sourceEncoder->GetBitArray(context.m_sourceContext, buffer, bits) || !context.m_targetEncoder->SetBitArray(context.m_targetContext, buffer, bits);
}

// OFFSET: 0x00665F40/0x14030C650
int32_t BSN::BSNPrimitiveWalker::Blob(const Protocol::Meta::Blob& meta, const Context& context) {
    uint32_t size = context.m_sourceEncoder->GetBlobLength(context.m_sourceContext);
    uint8_t* buffer = static_cast<uint8_t*>(_alloca(size));
    return !context.m_sourceEncoder->GetBlob(context.m_sourceContext, buffer, size) || !context.m_targetEncoder->SetBlob(context.m_targetContext, buffer, size);
}

// PROVEN: c1ecc9ac IDENTICAL/NONE
// OFFSET: 0x00665FB0/0x14030C700
int32_t BSN::BSNPrimitiveWalker::Bool(const Protocol::Meta::Bool& meta, const Context& context) {
    return !context.m_targetEncoder->SetBool(context.m_targetContext, context.m_sourceEncoder->GetBool(context.m_sourceContext));
}

// OFFSET: 0x00665FF0/0x14030C750
int32_t BSN::BSNPrimitiveWalker::Choice(const Protocol::Meta::Choice& meta, const Context& context) {
    char name[1024];

    if (!context.m_sourceEncoder->GetChoiceName(context.m_sourceContext, name, sizeof(name))) {
        return BSN::WALK_INVALID;
    }

    uint32_t field = 0;
    uint32_t typeId = 0;
    uint32_t count = meta.m_count;

    if (!count) {
        return BSN::WALK_INVALID;
    }

    while (strcmp(meta.m_protocol->DumpString(meta.m_dumpInfo, field + 1), name)) {
        if (++field >= count) {
            return BSN::WALK_INVALID;
        }
    }

    if (!meta.GetFieldTypeId(field, typeId)) {
        return BSN::WALK_INVALID;
    }

    const PrimitiveEncoder* sourceEncoder = context.m_sourceEncoder;
    const EncodeContext* sourceContext = context.m_sourceContext;
    EncodeContext* sourceMember = sourceEncoder->ConstructContext(_alloca(sourceEncoder->GetContextSize()));

    const PrimitiveEncoder* targetEncoder = context.m_targetEncoder;
    EncodeContext* targetContext = context.m_targetContext;
    EncodeContext* targetMember = targetEncoder->ConstructContext(_alloca(targetEncoder->GetContextSize()));

    int32_t result;

    if (targetEncoder->SetChoice(targetContext, meta.m_protocol->DumpString(meta.m_dumpInfo, 0), name) && sourceEncoder->GetMemberContext(sourceContext, name, sourceMember) && targetEncoder->GetMemberContext(targetContext, name, targetMember)) {
        Context member;
        member.m_protocol = context.m_protocol;
        member.m_typeId = typeId;
        member.m_sourceEncoder = context.m_sourceEncoder;
        member.m_sourceContext = sourceMember;
        member.m_targetEncoder = context.m_targetEncoder;
        member.m_targetContext = targetMember;
        result = BSN::BSNPrimitiveWalker::Walk(*this, member);
    } else {
        result = BSN::WALK_INVALID;
    }

    sourceEncoder->DestroyContext(sourceMember);
    targetEncoder->DestroyContext(targetMember);
    return result;
}

// PROVEN: b6f2f887 IDENTICAL/NONE
// OFFSET: 0x006661D0/0x14030C9D0
int32_t BSN::BSNPrimitiveWalker::FourCC(const Protocol::Meta::FourCC& meta, const Context& context) {
    return !context.m_targetEncoder->SetFourCC(context.m_targetContext, context.m_sourceEncoder->GetFourCC(context.m_sourceContext));
}

// PROVEN: b9a67b52 IDENTICAL/NONE
// OFFSET: 0x00666210/0x14030C980
int32_t BSN::BSNPrimitiveWalker::Int(const Protocol::Meta::Int& meta, const Context& context) {
    return !context.m_targetEncoder->SetInt(context.m_targetContext, context.m_sourceEncoder->GetInt(context.m_sourceContext));
}

// PROVEN: 6308854e IDENTICAL/NONE
// OFFSET: 0x00666210/0x14030C980
int32_t BSN::BSNPrimitiveWalker::Enum(const Protocol::Meta::Enum& meta, const Context& context) {
    return !context.m_targetEncoder->SetInt(context.m_targetContext, context.m_sourceEncoder->GetInt(context.m_sourceContext));
}

// OFFSET: 0x00666250/0x14030CA20
int32_t BSN::BSNPrimitiveWalker::Optional(const Protocol::Meta::Optional& meta, const Context& context) {
    const PrimitiveEncoder* sourceEncoder = context.m_sourceEncoder;
    const EncodeContext* sourceContext = context.m_sourceContext;
    const PrimitiveEncoder* targetEncoder = context.m_targetEncoder;
    EncodeContext* targetContext = context.m_targetContext;

    if (!sourceEncoder->GetOptional(sourceContext)) {
        return !targetEncoder->SetOptional(targetContext, false);
    }

    EncodeContext* sourceMember = sourceEncoder->ConstructContext(_alloca(sourceEncoder->GetContextSize()));
    EncodeContext* targetMember = targetEncoder->ConstructContext(_alloca(targetEncoder->GetContextSize()));

    int32_t result;

    if (targetEncoder->SetOptional(targetContext, true) && sourceEncoder->GetMemberContext(sourceContext, NULL, sourceMember) && targetEncoder->GetMemberContext(targetContext, NULL, targetMember)) {
        Context member;
        member.m_protocol = context.m_protocol;
        member.m_typeId = meta.m_elementTypeId;
        member.m_sourceEncoder = context.m_sourceEncoder;
        member.m_sourceContext = sourceMember;
        member.m_targetEncoder = context.m_targetEncoder;
        member.m_targetContext = targetMember;
        result = BSN::BSNPrimitiveWalker::Walk(*this, member);
    } else {
        result = BSN::WALK_INVALID;
    }

    sourceEncoder->DestroyContext(sourceMember);
    targetEncoder->DestroyContext(targetMember);
    return result;
}

// PROVEN: e49b5943 IDENTICAL/NONE
// OFFSET: 0x00666390/0x14030CBC0
int32_t BSN::BSNPrimitiveWalker::Real32(const Protocol::Meta::Real32& meta, const Context& context) {
    return !context.m_targetEncoder->SetReal(context.m_targetContext, context.m_sourceEncoder->GetReal(context.m_sourceContext));
}

// PROVEN: 871113e4 IDENTICAL/NONE
// OFFSET: 0x00666390/0x14030CBC0
int32_t BSN::BSNPrimitiveWalker::Real64(const Protocol::Meta::Real64& meta, const Context& context) {
    return !context.m_targetEncoder->SetReal(context.m_targetContext, context.m_sourceEncoder->GetReal(context.m_sourceContext));
}

// OFFSET: 0x006663D0/0x14030CC10
int32_t BSN::BSNPrimitiveWalker::Struct(const Protocol::Meta::Struct& meta, const Context& context) {
    const PrimitiveEncoder* sourceEncoder = context.m_sourceEncoder;
    const EncodeContext* sourceContext = context.m_sourceContext;
    int32_t result = BSN::WALK_VALID;
    void* sourceMemory = _alloca(sourceEncoder->GetContextSize());

    const PrimitiveEncoder* targetEncoder = context.m_targetEncoder;
    EncodeContext* targetContext = context.m_targetContext;
    void* targetMemory = _alloca(targetEncoder->GetContextSize());

    if (!targetEncoder->SetStruct(targetContext, meta.m_protocol->DumpString(meta.m_dumpInfo, 0))) {
        return BSN::WALK_INVALID;
    }

    uint32_t count = meta.m_count;

    for (uint32_t index = 0; index < count; index++) {
        if (result) {
            return result;
        }

        uint32_t offset;
        uint32_t typeId;

        if (!meta.GetMember(index, offset, typeId)) {
            return BSN::WALK_INVALID;
        }

        const char* name = meta.m_protocol->DumpString(meta.m_dumpInfo, index + 1);
        EncodeContext* sourceMember = sourceEncoder->ConstructContext(sourceMemory);
        EncodeContext* targetMember = targetEncoder->ConstructContext(targetMemory);

        if (sourceEncoder->GetMemberContext(sourceContext, name, sourceMember) && targetEncoder->GetMemberContext(targetContext, name, targetMember)) {
            Context member;
            member.m_protocol = context.m_protocol;
            member.m_typeId = typeId;
            member.m_sourceEncoder = context.m_sourceEncoder;
            member.m_sourceContext = sourceMember;
            member.m_targetEncoder = context.m_targetEncoder;
            member.m_targetContext = targetMember;
            result = BSN::BSNPrimitiveWalker::Walk(*this, member);
        } else {
            result = BSN::WALK_INVALID;
        }

        sourceEncoder->DestroyContext(sourceMember);
        targetEncoder->DestroyContext(targetMember);
    }

    return result;
}

// OFFSET: 0x00666570/0x14030C4F0
int32_t BSN::BSNPrimitiveWalker::AsciiString(const Protocol::Meta::AsciiString& meta, const Context& context) {
    uint32_t size = context.m_sourceEncoder->GetStringLength(context.m_sourceContext) + 1;
    char* buffer = static_cast<char*>(_alloca(size));
    return !context.m_sourceEncoder->GetString(context.m_sourceContext, buffer, size) || !context.m_targetEncoder->SetString(context.m_targetContext, buffer);
}

// OFFSET: 0x00666570/0x14030C4F0
int32_t BSN::BSNPrimitiveWalker::String(const Protocol::Meta::String& meta, const Context& context) {
    uint32_t size = context.m_sourceEncoder->GetStringLength(context.m_sourceContext) + 1;
    char* buffer = static_cast<char*>(_alloca(size));
    return !context.m_sourceEncoder->GetString(context.m_sourceContext, buffer, size) || !context.m_targetEncoder->SetString(context.m_targetContext, buffer);
}

// OFFSET: 0x006665E0/0x14030CE20
int32_t BSN::BSNPrimitiveWalker::User(const Protocol::Meta::User& meta, const Context& context) {
    Context user = context;
    user.m_typeId = meta.m_userTypeId;
    return BSN::BSNPrimitiveWalker::Walk(*this, user);
}

// PROVEN: 82717b44 IDENTICAL/NONE
// OFFSET: 0x00666630/0x14030CE70
int32_t BSN::BSNPrimitiveWalker::Walk(BSNPrimitiveWalker& walker, const Context& context) {
    switch (BSN::Protocol::Meta::PeekHeaderEnum(context.m_protocol, context.m_typeId)) {
        case BSN::META_ARRAY:
            return walker.Array(BSN::Protocol::Meta::Array(context.m_protocol, context.m_typeId), context);
        case BSN::META_ASCII_STRING:
            return walker.AsciiString(BSN::Protocol::Meta::AsciiString(context.m_protocol, context.m_typeId), context);
        case BSN::META_BIT_ARRAY:
            return walker.BitArray(BSN::Protocol::Meta::BitArray(context.m_protocol, context.m_typeId), context);
        case BSN::META_BLOB:
            return walker.Blob(BSN::Protocol::Meta::Blob(context.m_protocol, context.m_typeId), context);
        case BSN::META_BOOL:
            return walker.Bool(BSN::Protocol::Meta::Bool(context.m_protocol, context.m_typeId), context);
        case BSN::META_CHOICE:
            return walker.Choice(BSN::Protocol::Meta::Choice(context.m_protocol, context.m_typeId), context);
        case BSN::META_ENUM:
            return walker.Enum(BSN::Protocol::Meta::Enum(context.m_protocol, context.m_typeId), context);
        case BSN::META_FOURCC:
            return walker.FourCC(BSN::Protocol::Meta::FourCC(context.m_protocol, context.m_typeId), context);
        case BSN::META_INT:
            return walker.Int(BSN::Protocol::Meta::Int(context.m_protocol, context.m_typeId), context);
        case BSN::META_NULL:
            BSN::Protocol::Meta::Null(context.m_protocol, context.m_typeId);
            return BSN::WALK_VALID;
        case BSN::META_OPTIONAL:
            return walker.Optional(BSN::Protocol::Meta::Optional(context.m_protocol, context.m_typeId), context);
        case BSN::META_REAL32:
            return walker.Real32(BSN::Protocol::Meta::Real32(context.m_protocol, context.m_typeId), context);
        case BSN::META_REAL64:
            return walker.Real64(BSN::Protocol::Meta::Real64(context.m_protocol, context.m_typeId), context);
        case BSN::META_STRUCT:
            return walker.Struct(BSN::Protocol::Meta::Struct(context.m_protocol, context.m_typeId), context);
        case BSN::META_STRING:
            return walker.String(BSN::Protocol::Meta::String(context.m_protocol, context.m_typeId), context);
        case BSN::META_USER:
            return walker.User(BSN::Protocol::Meta::User(context.m_protocol, context.m_typeId), context);
        default:
            return BSN::WALK_UNKNOWN_TYPE;
    }
}

// OFFSET: 0x00627290
BSN::EncodedObject::~EncodedObject() {
}

// OFFSET: 0x006271D0/0x1402B9160
bool BSN::EncodedObject::WalkAndGet(const EncodedObject* source) {
    const PrimitiveEncoder* encoder = this->GetEncoder();
    return encoder->WalkAndGet(source->GetEncoder(), source->GetContext(), this->GetContext());
}

// OFFSET: 0x00627220/0x1402B91F0
bool BSN::EncodedObject::WalkAndSet(EncodedObject* target) const {
    const PrimitiveEncoder* encoder = this->GetEncoder();
    return encoder->WalkAndSet(target->GetEncoder(), target->GetContext(), this->GetContext());
}

// PROVEN: 998c1941 IDENTICAL/NONE
// OFFSET: 0x00627270/0x1402B9280
void BSN::EncodedObject::IncRef() const {
    const_cast<EncodedObject*>(this)->m_refCount++;
}

// OFFSET: 0x00627280/0x1402B9290
void BSN::EncodedObject::DecRef() const {
    if (const_cast<EncodedObject*>(this)->m_refCount-- == 1) {
        this->Destroy();
    }
}

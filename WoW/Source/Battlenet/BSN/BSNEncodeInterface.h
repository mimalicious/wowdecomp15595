#ifndef WOW_SOURCE_BATTLENET_BSN_BSNENCODEINTERFACE_H
#define WOW_SOURCE_BATTLENET_BSN_BSNENCODEINTERFACE_H

#include <cstddef>
#include <cstdint>
#include "WoW/Source/Battlenet/BSN/Protocol.h"
#include "WoW/Source/Battlenet/BSN/Types.h"
#include "Engine/Source/Storm/H/storm.h"

namespace BSN {
struct EncodeContext {
    /* 0000 */ const Protocol::Protocol* m_protocol;
    /* 0004 */ uint32_t m_typeId;
    /* 0008 */ uint32_t m_header;
    /* 000C */ void* m_data;

    void ResolveType();
};

class PrimitiveEncoder {
    public:
    virtual ~PrimitiveEncoder();
    virtual uint32_t GetContextSize() const = 0;
    virtual EncodeContext* ConstructContext(void* memory) const = 0;
    virtual void DestroyContext(EncodeContext* context) const = 0;
    virtual uint64_t GetUInt(const EncodeContext* context) const = 0;
    virtual int64_t GetInt(const EncodeContext* context) const = 0;
    virtual bool GetBool(const EncodeContext* context) const = 0;
    virtual double GetReal(const EncodeContext* context) const = 0;
    virtual uint32_t GetFourCC(const EncodeContext* context) const = 0;
    virtual uint32_t GetStringLength(const EncodeContext* context) const = 0;
    virtual bool GetString(const EncodeContext* context, char* buffer, uint32_t size) const = 0;
    virtual uint32_t GetBitArrayLength(const EncodeContext* context) const = 0;
    virtual bool GetBitArray(const EncodeContext* context, uint8_t* buffer, uint32_t bits) const = 0;
    virtual uint32_t GetBlobLength(const EncodeContext* context) const = 0;
    virtual bool GetBlob(const EncodeContext* context, uint8_t* buffer, uint32_t size) const = 0;
    virtual bool GetMemberContext(const EncodeContext* context, const char* name, EncodeContext* member) const = 0;
    virtual bool GetElementContext(const EncodeContext* context, uint32_t index, EncodeContext* element) const = 0;
    virtual bool GetChoiceName(const EncodeContext* context, char* buffer, uint32_t size) const = 0;
    virtual uint32_t GetArrayLength(const EncodeContext* context) const = 0;
    virtual bool GetOptional(const EncodeContext* context) const = 0;
    virtual bool SetUInt(EncodeContext* context, uint64_t value) const = 0;
    virtual bool SetInt(EncodeContext* context, int64_t value) const = 0;
    virtual bool SetBool(EncodeContext* context, bool value) const = 0;
    virtual bool SetReal(EncodeContext* context, double value) const = 0;
    virtual bool SetFourCC(EncodeContext* context, uint32_t value) const = 0;
    virtual bool SetString(EncodeContext* context, const char* value) const = 0;
    virtual bool SetBitArray(EncodeContext* context, const uint8_t* data, uint32_t bits) const = 0;
    virtual bool SetBlob(EncodeContext* context, const uint8_t* data, uint32_t size) const = 0;
    virtual bool SetStruct(EncodeContext* context, const char* name) const = 0;
    virtual bool SetChoice(EncodeContext* context, const char* typeName, const char* choiceName) const = 0;
    virtual bool SetArray(EncodeContext* context, uint32_t length) const = 0;
    virtual bool SetOptional(EncodeContext* context, bool exists) const = 0;
    virtual bool WalkAndSet(const PrimitiveEncoder* encoder, EncodeContext* context, const EncodeContext* source) const = 0;
    virtual bool WalkAndGet(const PrimitiveEncoder* encoder, const EncodeContext* source, EncodeContext* context) const = 0;
};

class BSNPrimitiveEncoder : public PrimitiveEncoder {
    public:
    uint32_t GetContextSize() const override;
    EncodeContext* ConstructContext(void* memory) const override;
    void DestroyContext(EncodeContext* context) const override;
    uint64_t GetUInt(const EncodeContext* context) const override;
    int64_t GetInt(const EncodeContext* context) const override;
    bool GetBool(const EncodeContext* context) const override;
    double GetReal(const EncodeContext* context) const override;
    uint32_t GetFourCC(const EncodeContext* context) const override;
    uint32_t GetStringLength(const EncodeContext* context) const override;
    bool GetString(const EncodeContext* context, char* buffer, uint32_t size) const override;
    uint32_t GetBitArrayLength(const EncodeContext* context) const override;
    bool GetBitArray(const EncodeContext* context, uint8_t* buffer, uint32_t bits) const override;
    uint32_t GetBlobLength(const EncodeContext* context) const override;
    bool GetBlob(const EncodeContext* context, uint8_t* buffer, uint32_t size) const override;
    bool GetMemberContext(const EncodeContext* context, const char* name, EncodeContext* member) const override;
    bool GetElementContext(const EncodeContext* context, uint32_t index, EncodeContext* element) const override;
    bool GetChoiceName(const EncodeContext* context, char* buffer, uint32_t size) const override;
    uint32_t GetArrayLength(const EncodeContext* context) const override;
    bool GetOptional(const EncodeContext* context) const override;
    bool SetUInt(EncodeContext* context, uint64_t value) const override;
    bool SetInt(EncodeContext* context, int64_t value) const override;
    bool SetBool(EncodeContext* context, bool value) const override;
    bool SetReal(EncodeContext* context, double value) const override;
    bool SetFourCC(EncodeContext* context, uint32_t value) const override;
    bool SetString(EncodeContext* context, const char* value) const override;
    bool SetBitArray(EncodeContext* context, const uint8_t* data, uint32_t bits) const override;
    bool SetBlob(EncodeContext* context, const uint8_t* data, uint32_t size) const override;
    bool SetStruct(EncodeContext* context, const char* name) const override;
    bool SetChoice(EncodeContext* context, const char* typeName, const char* choiceName) const override;
    bool SetArray(EncodeContext* context, uint32_t length) const override;
    bool SetOptional(EncodeContext* context, bool exists) const override;
    bool WalkAndSet(const PrimitiveEncoder* encoder, EncodeContext* context, const EncodeContext* source) const override;
    bool WalkAndGet(const PrimitiveEncoder* encoder, const EncodeContext* source, EncodeContext* context) const override;
};

class BSNPrimitiveWalker {
    public:
    struct Context {
        /* 0000 */ const Protocol::Protocol* m_protocol;
        /* 0004 */ uint32_t m_typeId;
        /* 0008 */ const PrimitiveEncoder* m_sourceEncoder;
        /* 000C */ const EncodeContext* m_sourceContext;
        /* 0010 */ const PrimitiveEncoder* m_targetEncoder;
        /* 0014 */ EncodeContext* m_targetContext;
    };

    static int32_t Walk(BSNPrimitiveWalker& walker, const Context& context);

    virtual ~BSNPrimitiveWalker();

    int32_t Copy(const Context& context);
    int32_t Array(const Protocol::Meta::Array& meta, const Context& context);
    int32_t BitArray(const Protocol::Meta::BitArray& meta, const Context& context);
    int32_t Blob(const Protocol::Meta::Blob& meta, const Context& context);
    int32_t Bool(const Protocol::Meta::Bool& meta, const Context& context);
    int32_t Choice(const Protocol::Meta::Choice& meta, const Context& context);
    int32_t FourCC(const Protocol::Meta::FourCC& meta, const Context& context);
    int32_t Int(const Protocol::Meta::Int& meta, const Context& context);
    int32_t Enum(const Protocol::Meta::Enum& meta, const Context& context);
    int32_t Optional(const Protocol::Meta::Optional& meta, const Context& context);
    int32_t Real32(const Protocol::Meta::Real32& meta, const Context& context);
    int32_t Real64(const Protocol::Meta::Real64& meta, const Context& context);
    int32_t Struct(const Protocol::Meta::Struct& meta, const Context& context);
    int32_t AsciiString(const Protocol::Meta::AsciiString& meta, const Context& context);
    int32_t String(const Protocol::Meta::String& meta, const Context& context);
    int32_t User(const Protocol::Meta::User& meta, const Context& context);
};

class EncodedObject {
    public:
    /* 0004 */ uint32_t m_refCount;

    virtual ~EncodedObject();
    virtual uint32_t GetEncodingType() const = 0;
    virtual const PrimitiveEncoder* GetEncoder() const = 0;
    virtual EncodeContext* GetContext() = 0;
    virtual const EncodeContext* GetContext() const = 0;
    virtual void Destroy() const = 0;
    virtual bool WalkAndGet(const EncodedObject* source);
    virtual bool WalkAndSet(EncodedObject* target) const;
    virtual void IncRef() const;
    virtual void DecRef() const;

    EncodedObject();
};

template <class T, class P>
class BSNEncodedObject : public EncodedObject {
    public:
    /* 0008 */ T m_data;
    /* 0008+T */ BSNPrimitiveEncoder m_encoder;
    /* 000C+T */ EncodeContext m_context;

    ~BSNEncodedObject();
    uint32_t GetEncodingType() const override;
    const PrimitiveEncoder* GetEncoder() const override;
    EncodeContext* GetContext() override;
    const EncodeContext* GetContext() const override;
    void Destroy() const override;

    BSNEncodedObject();
    explicit BSNEncodedObject(const EncodedObject* source);
};

template <class T, class P, int32_t ON_STACK>
class BSNEncodedObjectPtr : public EncodedObject {
    public:
    /* 0008 */ BSNPrimitiveEncoder m_encoder;
    /* 000C */ EncodeContext m_context;

    ~BSNEncodedObjectPtr();
    uint32_t GetEncodingType() const override;
    const PrimitiveEncoder* GetEncoder() const override;
    EncodeContext* GetContext() override;
    const EncodeContext* GetContext() const override;
    void Destroy() const override;

    explicit BSNEncodedObjectPtr(const T* data);
};
}

// OFFSET: [0x00904040<Battlenet::Friends::FriendList, Protocol>, 0x00628740<Battlenet::Event::Update::Invoke, Protocol>, 0x00628AF0<Battlenet::Event::Larva::Invoke, Protocol>, 0x009041B0<Battlenet::Toon::FullName, Protocol>, 0x00904300<Battlenet::Friends::FriendsOfFriendList, Protocol>]
template <class T, class P>
BSN::BSNEncodedObject<T, P>::BSNEncodedObject() {
    this->m_context.m_protocol = &P::s_protocol;
    this->m_context.m_typeId = P::template TypeIdOf<T>::s_typeId;
    this->m_context.m_data = &this->m_data;
    this->m_context.ResolveType();
}

// OFFSET: [0x00903DE0<Battlenet::Friends::FriendInvite, Protocol>, 0x00903EA0<Battlenet::Presence::FieldVal, Protocol>, 0x00903F70<Battlenet::Presence::BlockList, Protocol>, 0x009040F0<Battlenet::Presence::FieldUpdate, Protocol>, 0x00904260<Battlenet::Chat::MemberList, Protocol>]
template <class T, class P>
BSN::BSNEncodedObject<T, P>::BSNEncodedObject(const EncodedObject* source) {
    this->m_context.m_protocol = &P::s_protocol;
    this->m_context.m_typeId = P::template TypeIdOf<T>::s_typeId;
    this->m_context.m_data = &this->m_data;
    this->m_context.ResolveType();

    if (source) {
        this->EncodedObject::WalkAndGet(source);
    }
}

// INLINED
template <class T, class P>
BSN::BSNEncodedObject<T, P>::~BSNEncodedObject() {
}

// PROVEN: d72b77e6 IDENTICAL/DIFFERENT
// OFFSET: 0x009FCE20/0x14090FD50
template <class T, class P>
uint32_t BSN::BSNEncodedObject<T, P>::GetEncodingType() const {
    return 1;
}

// OFFSET: [0x00628790<Battlenet::Event::Update::Invoke, Protocol>, 0x00628B50<Battlenet::Event::Larva::Invoke, Protocol>, 0x00903E40<Battlenet::Friends::FriendInviteContainer, Protocol>, 0x00903FE0<Battlenet::Presence::BlockList, Protocol>, 0x00904090<Battlenet::Friends::FriendList, Protocol>, 0x00904160<Battlenet::Presence::FieldUpdate, Protocol>, 0x00904200<Battlenet::Toon::FullName, Protocol>, 0x009042C0<Battlenet::Chat::MemberList, Protocol>, 0x00662FE0<Battlenet::Event::Header, Protocol>]
template <class T, class P>
const BSN::PrimitiveEncoder* BSN::BSNEncodedObject<T, P>::GetEncoder() const {
    return &this->m_encoder;
}

// OFFSET: [0x00903F20<Battlenet::Presence::FieldVal, Protocol>, 0x006287A0<Battlenet::Event::Update::Invoke, Protocol>, 0x00628B60<Battlenet::Event::Larva::Invoke, Protocol>, 0x00903E50<Battlenet::Friends::FriendInviteContainer, Protocol>, 0x00903FF0<Battlenet::Presence::BlockList, Protocol>, 0x009040A0<Battlenet::Friends::FriendList, Protocol>]
template <class T, class P>
BSN::EncodeContext* BSN::BSNEncodedObject<T, P>::GetContext() {
    return &this->m_context;
}

// OFFSET: [0x00903F20<Battlenet::Presence::FieldVal, Protocol>]
template <class T, class P>
const BSN::EncodeContext* BSN::BSNEncodedObject<T, P>::GetContext() const {
    return &this->m_context;
}

// OFFSET: [0x00904350/0x14065C8C0]
template <class T, class P>
void BSN::BSNEncodedObject<T, P>::Destroy() const {
    delete this;
}

// INLINED
template <class T, class P, int32_t ON_STACK>
BSN::BSNEncodedObjectPtr<T, P, ON_STACK>::BSNEncodedObjectPtr(const T* data) {
    this->m_context.m_protocol = &P::s_protocol;
    this->m_context.m_typeId = P::template TypeIdOf<T>::s_typeId;
    this->m_context.m_data = const_cast<T*>(data);
    this->m_context.ResolveType();
}

// INLINED
template <class T, class P, int32_t ON_STACK>
BSN::BSNEncodedObjectPtr<T, P, ON_STACK>::~BSNEncodedObjectPtr() {
}

// PROVEN: 539fa59e IDENTICAL/DIFFERENT
// OFFSET: 0x009FCE20/0x14090FD50
template <class T, class P, int32_t ON_STACK>
uint32_t BSN::BSNEncodedObjectPtr<T, P, ON_STACK>::GetEncodingType() const {
    return 1;
}

// OFFSET: 0x00903DC0
template <class T, class P, int32_t ON_STACK>
const BSN::PrimitiveEncoder* BSN::BSNEncodedObjectPtr<T, P, ON_STACK>::GetEncoder() const {
    return &this->m_encoder;
}

// INLINED
template <class T, class P, int32_t ON_STACK>
BSN::EncodeContext* BSN::BSNEncodedObjectPtr<T, P, ON_STACK>::GetContext() {
    return &this->m_context;
}

// INLINED
template <class T, class P, int32_t ON_STACK>
const BSN::EncodeContext* BSN::BSNEncodedObjectPtr<T, P, ON_STACK>::GetContext() const {
    return &this->m_context;
}

// INLINED
template <class T, class P, int32_t ON_STACK>
void BSN::BSNEncodedObjectPtr<T, P, ON_STACK>::Destroy() const {
    if (ON_STACK) {
        SErrDisplayAppFatalMessage("", "", 0);
    }

    delete this;
}

#include "WoW/Source/Battlenet/BSN/BSNEncodeInterface.inl"

#endif

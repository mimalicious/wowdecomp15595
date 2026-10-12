#ifndef ENGINE_SOURCE_STORM_H_STPL_H
#define ENGINE_SOURCE_STORM_H_STPL_H

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <new>
#include <typeinfo>
#include "Engine/Source/Storm/H/storm.h"

template <class T>
class TSLink {
    public:
    /* 0000 */ TSLink<T>* m_prevLink;
    /* 0004 */ T* m_next;

    TSLink();
    ~TSLink();
    int32_t IsLinked();
    T* Next();
    TSLink<T>* NextLink(ptrdiff_t linkoffset);
    T* Prev();
    T* RawNext();
    void Unlink();
};

template <class T>
class TSGetLink {
    public:
    static TSLink<T>* Link(T* nodeptr, ptrdiff_t linkoffset);
};

template <class T>
class TSGetExplicitLink {
    public:
    static TSLink<T>* Link(T* nodeptr, ptrdiff_t linkoffset);
};

template <class T, class TGetLink = TSGetLink<T> >
class TSList {
    public:
    /* 0000 */ int32_t m_linkoffset;
    /* 0004 */ TSLink<T> m_terminator;

    static int32_t IsEnd(T* ptr);

    TSList();
    TSList(const TSList<T, TGetLink>& source);
    ~TSList();
    void ChangeLinkOffset(ptrdiff_t linkoffset);
    void Clear();
    void Combine(TSList<T, TGetLink>* list, uint32_t linktype, T* existingptr);
    T* DeleteNode(T* ptr);
    T* Head();
    void InitializeTerminator();
    int32_t IsEmpty();
    int32_t IsLinked(T* ptr);
    TSLink<T>* Link(T* ptr);
    void LinkNode(T* ptr, uint32_t linktype, T* existingptr);
    void LinkToHead(T* ptr);
    void LinkToTail(T* ptr);
    T* NewNode(uint32_t location, size_t extrabytes, uint32_t flags);
    T* Next(T* ptr);
    T* Prev(T* ptr);
    T* RawNext(T* ptr);
    void SetLinkOffset(ptrdiff_t linkoffset);
    T* Tail();
    void UnlinkAll();
    void UnlinkNode(T* ptr);
};

template <class T, int offset = static_cast<int>(0xDDDDDDDD)>
class TSExplicitList : public TSList<T, TSGetExplicitLink<T> > {
    public:
    TSExplicitList();
};

template <class T>
class TSLinkedNode {
    public:
    /* 0000 */ TSLink<T> m_link;

    T* Next();
    T* RawNext();
};

template <class T>
class TSBaseArray {
    public:
    /* 0000 */ uint32_t m_alloc;
    /* 0004 */ uint32_t m_count;
    /* 0008 */ T* m_data;

    T& operator[](uint32_t index);
    uint32_t Count();
    const char* MemFileName();
    int32_t MemLineNo();
    T* Ptr();
    T* Top();
};

template <class T>
class TSFixedArray : public TSBaseArray<T> {
    public:
    TSFixedArray();
    TSFixedArray(const TSFixedArray<T>& source);
    ~TSFixedArray();
    TSFixedArray<T>& operator=(const TSFixedArray<T>& source);
    void Clear();
    void ReallocAndClearData(uint32_t count);
    void ReallocData(uint32_t count);
    void Set(uint32_t count, const T* data);
    void SetCount(uint32_t count);
};

template <class T>
class TSBitField {
    public:
    /* 0000 */ uint32_t m_count;
    /* 0004 */ T m_bits;

    void Load(const void* data, uint32_t bytes, int32_t bits);
};

template <class T>
class TSStackArray {
    public:
    /* 0000 */ uint32_t m_count;
    /* 0004 */ T* m_data;

    TSStackArray(T* data, uint32_t count, int32_t construct);
    void Set(uint32_t count, const T* data);
    void Set(uint32_t count, uint32_t stride, const T* data);
    void SetCount(uint32_t count);
};

template <class T>
class TSGrowableArray : public TSFixedArray<T> {
    public:
    /* 000C */ uint32_t m_chunk;

    TSGrowableArray();
    uint32_t Add(uint32_t count, const T* data);
    uint32_t Add(uint32_t count, uint32_t incr, const T* data);
    uint32_t CalcChunkSize(uint32_t count);
    void GrowToFit(uint32_t index, int32_t zero);
    T* New();
    T* New(const T& source);
    void Reserve(uint32_t count, int32_t round);
    void SetCount(uint32_t count);
};

template <class T, uint32_t N>
class TSCGrowableArray {
    public:
    /* 0000 */ union {
        uint32_t m_align;
        uint8_t m_inlineStorage[sizeof(T) * N];
        uint8_t m_dynamicStorage[sizeof(TSGrowableArray<T>)];
    };
    uint32_t m_count;

    TSCGrowableArray();
    ~TSCGrowableArray();
    void Add(uint32_t count, uint32_t incr, const T* data);
    uint32_t Count() const;
    TSGrowableArray<T>& Dynamic();
    const TSGrowableArray<T>& Dynamic() const;
    T* Inline();
    const T* Inline() const;
    T& operator[](uint32_t index);
    void BecomeDynamic();
    T* New();
    T* New(const T& source);
    T* Ptr();
    const T* Ptr() const;
    void SetCount(uint32_t count);
};

template <class T, class TKey>
class TSHashObject {
    public:
    /* 0000 */ uint32_t m_hashval;
    /* 0004 */ TSLink<T> m_linktoslot;
    /* 000C */ TSLink<T> m_linktofull;
    /* 0014 */ TKey m_key;

    ~TSHashObject();
};

template <class T, class TKey>
class TSHashTable {
    public:
    /* 0004 */ TSExplicitList<T> m_fulllist;
    /* 0010 */ uint32_t m_fullnessIndicator;
    /* 0014 */ TSGrowableArray<TSExplicitList<T> > m_slotlistarray;
    /* 0024 */ uint32_t m_slotmask;
    /* 0028 */ uint32_t m_maxslots;

    virtual void InternalDelete(T* ptr);
    virtual T* InternalNew(TSExplicitList<T>* listptr, size_t extrabytes, uint32_t flags);
    virtual ~TSHashTable();
    virtual void Destroy();

    TSHashTable();
    void Clear();
    void Delete(T* ptr);
    T* DeleteNode(T* ptr);
    void GrowListArray(uint32_t newarraysize);
    T* Head();
    void Initialize();
    void Insert(T* ptr, uint32_t hashval, const TKey& key);
    void Insert(T* ptr, uint32_t hashval, const char* str);
    void Insert(T* ptr, const char* str);
    void InternalClear(int32_t warn);
    void InternalLinkNode(T* ptr, uint32_t hashval);
    T* InternalNewNode(uint32_t hashval, size_t extrabytes, uint32_t flags);
    int32_t MonitorFullness(uint32_t slot);
    T* New(uint32_t hashval, const TKey& key, size_t extrabytes, uint32_t flags);
    T* New(uint32_t hashval, const char* str, size_t extrabytes, uint32_t flags);
    T* New(const char* str, size_t extrabytes, uint32_t flags);
    T* Next(T* ptr);
    T* Ptr(uint32_t hashval, const TKey& key);
    T* Ptr(const char* str);
    void Unlink(T* ptr);
};

struct HASHKEY_CONSTSTR {
    /* 0000 */ const char* m_str;

    HASHKEY_CONSTSTR();
    bool operator==(const char* str);
};

struct HASHKEY_CONSTSTRI : public HASHKEY_CONSTSTR {
    HASHKEY_CONSTSTRI& operator=(const char* str);
    bool operator==(const char* str);
};

struct HASHKEY_DWORD {
    /* 0000 */ uint32_t m_key;

    bool operator==(const HASHKEY_DWORD& key);
};

struct HASHKEY_NONE {
    bool operator==(const HASHKEY_NONE& key);
};

struct HASHKEY_PTR {
    /* 0000 */ void* m_key;

    bool operator==(const HASHKEY_PTR& key);
};

struct HASHKEY_STR {
    /* 0000 */ char* m_str;

    HASHKEY_STR();
    ~HASHKEY_STR();
    HASHKEY_STR& operator=(const char* str);
    bool operator==(const char* str);
};

struct HASHKEY_STRI : public HASHKEY_STR {
    HASHKEY_STRI& operator=(const char* str);
    bool operator==(const char* str);
};

struct HASHKEY_UTF8I {
    /* 0000 */ char* m_str;

    HASHKEY_UTF8I& operator=(const char* str);
    bool operator==(const char* str);
};

template <class T, class TKey>
class TSHashTableReuse : public TSHashTable<T, TKey> {
    public:
    /* 002C */ TSExplicitList<T> m_reuseList;

    virtual void InternalDelete(T* ptr);
    virtual T* InternalNew(TSExplicitList<T>* listptr, size_t extrabytes, uint32_t flags);
    virtual ~TSHashTableReuse();
    virtual void Destroy();

    TSHashTableReuse();
    void Destructor();
};

template <class T, class THandle>
class TSExportTableSimpleReuse : public TSHashTableReuse<T, HASHKEY_NONE> {
    public:
    /* 0038 */ HASHKEY_NONE m_key;
    /* 003C */ uint32_t m_sequence;
    /* 0040 */ int32_t m_wrapped;

    TSExportTableSimpleReuse();
    void Delete(T* ptr);
    THandle GenerateUniqueHandle();
    T* New(THandle* handle);
    T* Ptr(THandle handle);
};

template <class T, class THandle, class TLockedHandle, class TSync>
class TSExportTableSyncReuse : public TSExportTableSimpleReuse<T, THandle> {
    public:
    /* 0044 */ TSync m_sync;

    ~TSExportTableSyncReuse();
    void Delete(THandle handle);
    void DeleteUnlock(T* ptr, TLockedHandle lockedhandle);
    int32_t IsForWriting(TLockedHandle lockedhandle);
    T* Lock(THandle handle, TLockedHandle* lockedhandle, int32_t forwriting);
    T* NewLock(THandle* handle, TLockedHandle* lockedhandle);
    void SyncEnterLock(TLockedHandle* lockedhandle, int32_t forwriting);
    void SyncLeaveLock(TLockedHandle lockedhandle);
    void Unlock(TLockedHandle lockedhandle);
};

// INLINED
template <class T>
TSLink<T>::TSLink() {
    this->m_prevLink = NULL;
    this->m_next = NULL;
}

// INLINED
template <class T>
TSLink<T>::~TSLink() {
    this->Unlink();
}

// INLINED
template <class T>
int32_t TSLink<T>::IsLinked() {
    return this->m_next != NULL;
}

// INLINED
template <class T>
T* TSLink<T>::Next() {
    return (reinterpret_cast<uintptr_t>(this->m_next) & 1) || !this->m_next ? NULL : this->m_next;
}

// OFFSET: [0x005719F0/0x1400EB480<BFSNODE>]
template <class T>
TSLink<T>* TSLink<T>::NextLink(ptrdiff_t linkoffset) {
    T* next = this->Next();

    if (!next) {
        return reinterpret_cast<TSLink<T>*>(reinterpret_cast<uintptr_t>(this->m_next) & ~static_cast<uintptr_t>(1));
    }

    if (linkoffset < 0) {
        linkoffset = reinterpret_cast<uint8_t*>(this) - reinterpret_cast<uint8_t*>(this->m_prevLink->m_next);
    }

    return reinterpret_cast<TSLink<T>*>(reinterpret_cast<uint8_t*>(this->m_next) + linkoffset);
}

// INLINED
template <class T>
T* TSLink<T>::Prev() {
    return this->m_prevLink->m_prevLink->Next();
}

// INLINED
template <class T>
T* TSLink<T>::RawNext() {
    return this->m_next;
}

// OFFSET: [0x006FEFD0/0x1400B1A40<VBBList_Block>]
template <class T>
void TSLink<T>::Unlink() {
    if (this->m_prevLink) {
        this->NextLink(-1)->m_prevLink = this->m_prevLink;
        this->m_prevLink->m_next = this->m_next;
        this->m_prevLink = NULL;
        this->m_next = NULL;
    }
}

// INLINED
template <class T>
TSLink<T>* TSGetLink<T>::Link(T* nodeptr, ptrdiff_t linkoffset) {
    return &static_cast<TSLinkedNode<T>*>(nodeptr)->m_link;
}

// INLINED
template <class T>
TSLink<T>* TSGetExplicitLink<T>::Link(T* nodeptr, ptrdiff_t linkoffset) {
    return reinterpret_cast<TSLink<T>*>(reinterpret_cast<uint8_t*>(nodeptr) + linkoffset);
}

// OFFSET: 0x00401050/0x140001020
template <class T, class TGetLink>
int32_t TSList<T, TGetLink>::IsEnd(T* ptr) {
    return (reinterpret_cast<uintptr_t>(ptr) & 1) || !ptr;
}

// INLINED
template <class T, class TGetLink>
TSList<T, TGetLink>::TSList() {
    this->SetLinkOffset(0);
}

// INLINED
template <class T, class TGetLink>
TSList<T, TGetLink>::TSList(const TSList<T, TGetLink>& source) {
    this->SetLinkOffset(source.m_linkoffset);
}

// OFFSET: [0x004468C0<_BANNEDADDONLOADNODE, TSGetLink<_BANNEDADDONLOADNODE>>, 0x009F1CF0<CGChatBubbleFrame, TSGetLink<CGChatBubbleFrame>>, 0x00510280<CSimpleAnimGroup, TSGetExplicitLink<CSimpleAnimGroup>>]
template <class T, class TGetLink>
TSList<T, TGetLink>::~TSList() {
    this->UnlinkAll();
}

// INLINED
template <class T, class TGetLink>
void TSList<T, TGetLink>::ChangeLinkOffset(ptrdiff_t linkoffset) {
    if (linkoffset != this->m_linkoffset) {
        this->UnlinkAll();
        this->SetLinkOffset(linkoffset);
    }
}

// OFFSET: [0x00508E90/0x140150AA0<CLayoutFrame::FRAMENODE, TSGetLink<CLayoutFrame::FRAMENODE>>, 0x00900DA0/0x140658DB0<STYLEREC, TSGetLink<STYLEREC>>, 0x009F1830<CGChatBubbleFrame, TSGetLink<CGChatBubbleFrame>>, 0x009A3580/0x1407289C0<EQUIPMENT_SET_NODE, TSGetLink<EQUIPMENT_SET_NODE>>, 0x0094A550/0x1406B8570<CLIENT_ACHIEVEMENT_CRITERIA, TSGetLink<CLIENT_ACHIEVEMENT_CRITERIA>>, 0x0094D9F0/0x1406BCFA0<COMPLETED_QUESTS, TSGetLink<COMPLETED_QUESTS>>, 0x0048A1C0/0x1400AC6A0<NETEVENTQUEUENODE, TSGetLink<NETEVENTQUEUENODE>>, 0x00841620<CGxBuf, TSGetLink<CGxBuf>>, 0x005F5FF0/0x1402798B0<InvalidatedName, TSGetLink<InvalidatedName>>, 0x005F6160/0x140279A20<CombatLogFilter, TSGetLink<CombatLogFilter>>, 0x005F99E0/0x14027E820<CombatLogEntry, TSGetLink<CombatLogEntry>>, 0x004B7290<CMDDEF, TSGetLink<CMDDEF>>, 0x008AE3D0/0x1405F2E80<PeriodicClientTrigger, TSGetLink<PeriodicClientTrigger>>, 0x007295A0/0x140401C30<CRenderCallback, TSGetExplicitLink<CRenderCallback>>, 0x00729690/0x140401D90<SObjectFadeout, TSGetExplicitLink<SObjectFadeout>>, 0x00781070<CWorldOccluder, TSGetExplicitLink<CWorldOccluder>>, 0x0074BCF0/0x14042BA40<Weather::PosDelta, TSGetExplicitLink<Weather::PosDelta>>, 0x0061BE00<CHARCODEDESC, TSGetExplicitLink<CHARCODEDESC>>, 0x00528140/0x1401775B0<CSimpleControlPoint, TSGetExplicitLink<CSimpleControlPoint>>, 0x00838650<CM2SequenceLoad, TSGetLink<CM2SequenceLoad>>, 0x00708C10<SE3SoundEmitter, TSGetLink<SE3SoundEmitter>>, 0x0070A7B0<SE3DoneLoadingMessage, TSGetLink<SE3DoneLoadingMessage>>, 0x0070A820<SE3MemorySoundEndedMessage, TSGetLink<SE3MemorySoundEndedMessage>>, 0x0070A890<SE3SoundEndedMessage, TSGetLink<SE3SoundEndedMessage>>, 0x0070A900<SE3WaveDataNode, TSGetLink<SE3WaveDataNode>>, 0x0070A940/0x1403D8B10<SE3CombinableSoundKitPosition, TSGetLink<SE3CombinableSoundKitPosition>>, 0x0070ABD0<SE3CreateSoundEmitterMessage, TSGetLink<SE3CreateSoundEmitterMessage>>, 0x0070AC10/0x1403D8BB0<SE3Set3DPositionMessage, TSGetLink<SE3Set3DPositionMessage>>, 0x0070AC80/0x1403D8C50<SE3DestroySoundEmitterMessage, TSGetLink<SE3DestroySoundEmitterMessage>>, 0x0070ACF0/0x1403D8CF0<SE3MarkSoundChunkDirtyMessage, TSGetLink<SE3MarkSoundChunkDirtyMessage>>, 0x00540280<CAsyncObject, TSGetExplicitLink<CAsyncObject>>, 0x005402F0<CAsyncThread, TSGetExplicitLink<CAsyncThread>>, 0x00547330<FileNotification::CallbackInfo, TSGetExplicitLink<FileNotification::CallbackInfo>>, 0x00547510<FileNotification::ManifestEntry, TSGetExplicitLink<FileNotification::ManifestEntry>>, 0x008A1F40<EvtHandler, TSGetExplicitLink<EvtHandler>>, 0x008A1FC0<EvtMessage, TSGetExplicitLink<EvtMessage>>, 0x008A2030/0x1405E3E00<EvtKeyDown, TSGetExplicitLink<EvtKeyDown>>, 0x008AE4C0/0x1405F2F20<WEEKLYSPELLUSE, TSGetLink<WEEKLYSPELLUSE>>, 0x009398F0/0x1406A3B40<CGGuildInfo::GuildProfessionHeader, TSGetLink<CGGuildInfo::GuildProfessionHeader>>, 0x007026F0/0x1403CEA80<CComponentRequest, TSGetLink<CComponentRequest>>, 0x00702770/0x1403CEB50<CComponentMipBits, TSGetLink<CComponentMipBits>>, 0x00499E50<WowClientDB2<ItemRec>::DBCACHE, TSGetExplicitLink<WowClientDB2<ItemRec>::DBCACHE>>, 0x00499E90<WowClientDB2<ItemRecSparse>::DBCACHE, TSGetExplicitLink<WowClientDB2<ItemRecSparse>::DBCACHE>>, 0x00499ED0<WowClientDB2<ItemCurrencyCostRec>::DBCACHE, TSGetExplicitLink<WowClientDB2<ItemCurrencyCostRec>::DBCACHE>>, 0x00499F10<WowClientDB2<ItemExtendedCostRec>::DBCACHE, TSGetExplicitLink<WowClientDB2<ItemExtendedCostRec>::DBCACHE>>, 0x00499F50<WowClientDB2<KeyChainRec>::DBCACHE, TSGetExplicitLink<WowClientDB2<KeyChainRec>::DBCACHE>>, 0x004D9770/0x140117B10<GAMETIMECBSTRUCT, TSGetLink<GAMETIMECBSTRUCT>>, 0x004828B0<DIRTYFACE, TSGetLink<DIRTYFACE>>, 0x0048C1E0/0x1400AFA90<WowClientDB2_Base::HOTFIX, TSGetLink<WowClientDB2_Base::HOTFIX>>, 0x00499B60/0x1400C4590<DBCALLBACK, TSGetLink<DBCALLBACK>>, 0x00569630/0x1401C8860<CATEGORYCOOLDOWN, TSGetLink<CATEGORYCOOLDOWN>>, 0x005E2030/0x140260180<CObjectEffect, TSGetLink<CObjectEffect>>, 0x0062EB30/0x1402C2DD0<AmbFlavor, TSGetLink<AmbFlavor>>, 0x00913E10<PASSIVESPELL, TSGetLink<PASSIVESPELL>>, 0x00917070/0x140676C50<SPELLBYNAME, TSGetLink<SPELLBYNAME>>, 0x0094A640/0x1406B8610<CLIENT_ACHIEVEMENTS, TSGetLink<CLIENT_ACHIEVEMENTS>>, 0x005F6070/0x140279980<PendingCombatLogData, TSGetLink<PendingCombatLogData>>, 0x004614A0/0x14006CFB0<BFSNODE, TSGetLink<BFSNODE>>, 0x009A0E90/0x140725510<PlayerCurrency, TSGetLink<PlayerCurrency>>, 0x00B316A0/0x1408FB290<POIIcon, TSGetLink<POIIcon>>, 0x00B31710/0x1408FB330<MAPDATA, TSGetLink<MAPDATA>>, 0x0050FF60/0x140159410<CSimpleAnimGroup, TSGetExplicitLink<CSimpleAnimGroup>>, 0x005281E0/0x140177760<CSimpleAnim, TSGetExplicitLink<CSimpleAnim>>, 0x00611480<CGxuLight, TSGetLink<CGxuLight>>, 0x008BEA40<LightningObject, TSGetLink<LightningObject>>, 0x0045F430/0x140072710<ADDONSTATELIST, TSGetLink<ADDONSTATELIST>>, 0x00542690/0x1401957A0<CMipBitsCache, TSGetExplicitLink<CMipBitsCache>>, 0x0060F100/0x140299210<CONSOLELINE, TSGetLink<CONSOLELINE>>, 0x00446DC0/0x140052F90<_BANNEDADDONLOADNODE, TSGetLink<_BANNEDADDONLOADNODE>>, 0x004A3A90/0x1400D2270<DBCACHECALLBACK, TSGetLink<DBCACHECALLBACK>>, 0x0052A180/0x14017A080<SIMPLEANIMNODE, TSGetLink<SIMPLEANIMNODE>>, 0x00542600/0x1401956E0<CGxTexCache, TSGetExplicitLink<CGxTexCache>>, 0x00546010/0x14019D420, 0x005695C0/0x1401C87C0<PlayerPendingItemExpiration, TSGetExplicitLink<PlayerPendingItemExpiration>>, 0x005798F0/0x1401DEF50<CPlayerMoveEvent, TSGetExplicitLink<CPlayerMoveEvent>>, 0x005E3680/0x140262130, 0x005E3A20/0x1402624E0, 0x00615990<CGxStringBatch, TSGetLink<CGxStringBatch>>, 0x0062A580/0x1402BD430, 0x0062A680/0x1402BD4A0<BattlenetLogin::BN_EVENT, TSGetLink<BattlenetLogin::BN_EVENT>>, 0x00632A10/0x1402C8140<SI3DspNode, TSGetLink<SI3DspNode>>, 0x00804360/0x1405193D0<ConsoleString, TSGetExplicitLink<ConsoleString>>, 0x00832C20/0x1405550E0<CM2SequencePlayBack, TSGetLink<CM2SequencePlayBack>>, 0x0083EE60/0x14056A6D0<EVENTLISTENERNODE, TSGetLink<EVENTLISTENERNODE>>, 0x00872900/0x1405A8E40, 0x0087DBB0/0x1405B6560, 0x008AECC0/0x1405F3E20, 0x008D9440<AUTOCOMPLETE_NAMENODE, TSGetLink<AUTOCOMPLETE_NAMENODE>>, 0x008E4D20/0x140612B20<CModelRecord, TSGetLink<CModelRecord>>, 0x008EE170/0x14061DAF0<PENDINGUSERLIST, TSGetExplicitLink<PENDINGUSERLIST>>, 0x008EF320/0x14061F350<THROTTLEDCHAT, TSGetExplicitLink<THROTTLEDCHAT>>, 0x00924610/0x140688B40<OVERRIDEKEYBINDINGENTRY, TSGetLink<OVERRIDEKEYBINDINGENTRY>>, 0x009B3F20/0x14073CFF0<CALENDARHOLIDAYNODE, TSGetLink<CALENDARHOLIDAYNODE>>, 0x009B3F90/0x14073D090<CALENDARRAIDRESETNODE, TSGetLink<CALENDARRAIDRESETNODE>>, 0x009B4000/0x14073D130<CalendarPendingInvite, TSGetLink<CalendarPendingInvite>>, 0x00B408E0/0x14090E800<CLCDBASE_NODE, TSGetLink<CLCDBASE_NODE>>]
template <class T, class TGetLink>
void TSList<T, TGetLink>::Clear() {
    T* node;

    while ((node = this->Head()) != NULL) {
        this->DeleteNode(node);
    }
}

// OFFSET: [0x004FC060<CSimpleEmbeddedTexture, TSGetExplicitLink<CSimpleEmbeddedTexture>>, 0x0094FB30<ENCOUNTER_OBJECTIVE, TSGetLink<ENCOUNTER_OBJECTIVE>>, 0x005E1570<CObjectEffectRef, TSGetLink<CObjectEffectRef>>, 0x008BEF80/0x140608610<MountTransitionObject, TSGetLink<MountTransitionObject>>, 0x00B3F7B0<CLCDConnection::CB_EVENT_NODE, TSGetLink<CLCDConnection::CB_EVENT_NODE>>, 0x005221B0/0x14016FF80<CONTENTNODE, TSGetLink<CONTENTNODE>>, 0x00729610/0x140401CE0<CRenderCallback, TSGetExplicitLink<CRenderCallback>>, 0x0074BD60/0x14042BAF0<Weather::PosDelta, TSGetExplicitLink<Weather::PosDelta>>, 0x00716240/0x1403E92D0<SE3AsyncData, TSGetLink<SE3AsyncData>>, 0x00501680<UNEEDED_TEXTURES, TSGetLink<UNEEDED_TEXTURES>>, 0x00505990/0x14014C4F0<SIMPLEFRAMENODE, TSGetLink<SIMPLEFRAMENODE>>, 0x00508F00/0x140150B40<CLayoutFrame::FRAMENODE, TSGetLink<CLayoutFrame::FRAMENODE>>, 0x008A3AA0/0x1405E6040<EvtKeyDown, TSGetExplicitLink<EvtKeyDown>>, 0x00499BD0/0x1400C4630<DBCALLBACK, TSGetLink<DBCALLBACK>>, 0x00625C80<RealmConnection::REALMCONNECTIONNODE, TSGetLink<RealmConnection::REALMCONNECTIONNODE>>, 0x008AE440<PeriodicClientTrigger, TSGetLink<PeriodicClientTrigger>>, 0x0094A5C0<CLIENT_ACHIEVEMENT_CRITERIA, TSGetLink<CLIENT_ACHIEVEMENT_CRITERIA>>, 0x0094DA60<COMPLETED_QUESTS, TSGetLink<COMPLETED_QUESTS>>, 0x009CC0C0/0x14075BE80<CGAutoCompleteName::AUTOCOMPLETE_NAMENODE, TSGetLink<CGAutoCompleteName::AUTOCOMPLETE_NAMENODE>>, 0x00407580<`anonymous namespace'::ArchivePreload, TSGetExplicitLink<`anonymous namespace'::ArchivePreload>>, 0x00439B30/0x140044500<CCameraManager::CameraShake, TSGetLink<CCameraManager::CameraShake>>, 0x0048C250/0x1400AFB30<WowClientDB2_Base::HOTFIX, TSGetLink<WowClientDB2_Base::HOTFIX>>, 0x0062A6F0/0x1402BD540<BattlenetLogin::BN_EVENT, TSGetLink<BattlenetLogin::BN_EVENT>>, 0x009130A0<SPELLRANKNODE, TSGetLink<SPELLRANKNODE>>, 0x009B4070<CalendarPendingInvite, TSGetLink<CalendarPendingInvite>>, 0x004898B0<NETCLIENTNODE, TSGetLink<NETCLIENTNODE>>, 0x00913E80/0x14066F7C0<PASSIVESPELLNODE, TSGetLink<PASSIVESPELLNODE>>, 0x005F60E0<PendingCombatLogData, TSGetLink<PendingCombatLogData>>, 0x0094A6B0<CLIENT_ACHIEVEMENTS, TSGetLink<CLIENT_ACHIEVEMENTS>>, 0x008EF400<PENDINGTEXTEMOTE, TSGetLink<PENDINGTEXTEMOTE>>, 0x004A39F0/0x1400D21C0<DBCACHECALLBACK, TSGetLink<DBCACHECALLBACK>>, 0x004B5ED0/0x1400ED6B0<HANDLER, TSGetLink<HANDLER>>, 0x004C0F30<WNDREC, TSGetLink<WNDREC>>, 0x004FC620/0x1401409A0<RENDERCALLBACKNODE, TSGetLink<RENDERCALLBACKNODE>>, 0x0057BEE0/0x1401E1C60<PendingLootRoll, TSGetExplicitLink<PendingLootRoll>>, 0x00614460<CGxFont, TSGetLink<CGxFont>>, 0x0062A600<BattlenetLogin::BN_REALM, TSGetLink<BattlenetLogin::BN_REALM>>, 0x0052A1F0<SIMPLEANIMNODE, TSGetLink<SIMPLEANIMNODE>>, 0x00792C30/0x140484950<VBBList_Block, TSGetExplicitLink<VBBList_Block>>, 0x0081FFD0/0x14053B500<CM2SequencePlayBack, TSGetLink<CM2SequencePlayBack>>, 0x00838690/0x1405622B0<CM2SequenceLoad, TSGetLink<CM2SequenceLoad>>, 0x0083E460/0x140569910<EVENTLISTENERNODE, TSGetLink<EVENTLISTENERNODE>>, 0x008A1D70/0x1405E3A50, 0x008A3A20/0x1405E5FA0<EvtHandler, TSGetExplicitLink<EvtHandler>>, 0x008D8BA0/0x14064C320<CapturePointUIManagerNode, TSGetLink<CapturePointUIManagerNode>>, 0x008EE1E0/0x14061DB90<PENDINGUSERLIST, TSGetExplicitLink<PENDINGUSERLIST>>, 0x008EF3B0<PENDINGCHAT, TSGetExplicitLink<PENDINGCHAT>>, 0x0094FAB0/0x1406BFDA0<ENCOUNTER_TIMER, TSGetLink<ENCOUNTER_TIMER>>, 0x009A35F0/0x140728A60<EQUIPMENT_SET_NODE, TSGetLink<EQUIPMENT_SET_NODE>>, 0x00B05DC0<Grunt::Pending::Data, TSGetExplicitLink<Grunt::Pending::Data>>]
template <class T, class TGetLink>
T* TSList<T, TGetLink>::DeleteNode(T* ptr) {
    T* next = this->Next(ptr);

    ptr->~T();
    SMemFree(ptr, typeid(T).raw_name(), SLOG_OBJECT, 0);

    return next;
}

// OFFSET: 0x00544F70/0x1402B0D40
template <class T, class TGetLink>
T* TSList<T, TGetLink>::Head() {
    return this->m_terminator.Next();
}

// INLINED
template <class T, class TGetLink>
void TSList<T, TGetLink>::InitializeTerminator() {
    this->m_terminator.m_prevLink = &this->m_terminator;
    this->m_terminator.m_next = reinterpret_cast<T*>(reinterpret_cast<uintptr_t>(&this->m_terminator) | 1);
}

// INLINED
template <class T, class TGetLink>
int32_t TSList<T, TGetLink>::IsEmpty() {
    return this->Head() == NULL;
}

// OFFSET: 0x005080D0
template <class T, class TGetLink>
int32_t TSList<T, TGetLink>::IsLinked(T* ptr) {
    return this->Link(ptr)->IsLinked();
}

// INLINED
template <class T, class TGetLink>
TSLink<T>* TSList<T, TGetLink>::Link(T* ptr) {
    return ptr ? TGetLink::Link(ptr, this->m_linkoffset) : &this->m_terminator;
}

// OFFSET: [0x004BA4B0/0x140298210<CONSOLELINE, TSGetLink<CONSOLELINE>>, 0x00715750/0x14019F870]
template <class T, class TGetLink>
void TSList<T, TGetLink>::LinkNode(T* ptr, uint32_t linktype, T* existingptr) {
    TSLink<T>* link = this->Link(ptr);
    link->Unlink();

    TSLink<T>* existinglink = this->Link(existingptr);

    if (linktype == 1) {
        link->m_prevLink = existinglink;
        link->m_next = existinglink->m_next;
        existinglink->NextLink(this->m_linkoffset)->m_prevLink = link;
        existinglink->m_next = ptr;
    } else {
        TSLink<T>* prevlink = existinglink->m_prevLink;
        link->m_prevLink = prevlink;
        link->m_next = prevlink->m_next;
        prevlink->m_next = ptr;
        existinglink->m_prevLink = link;
    }
}

// OFFSET: [0x007070C0/0x140298170<ProfileInternal::STRINGBLOCK, TSGetLink<ProfileInternal::STRINGBLOCK>>, 0x008A36D0/0x140570C50<CMapChunkBufBlock, TSGetExplicitLink<CMapChunkBufBlock>>]
template <class T, class TGetLink>
void TSList<T, TGetLink>::LinkToHead(T* ptr) {
    this->LinkNode(ptr, 1, NULL);
}

// OFFSET: [0x00446850/0x14071C880<DBCache<GuildStats_C, int, HASHKEY_INT>::DBCACHEHASH, TSGetExplicitLink<DBCache<GuildStats_C, int, HASHKEY_INT>::DBCACHEHASH>>, 0x00707150/0x1406FF120<WowConnection::SENDNODE, TSGetLink<WowConnection::SENDNODE>>, 0x009E8CE0<CGChatBubbleFrame, TSGetLink<CGChatBubbleFrame>>]
template <class T, class TGetLink>
void TSList<T, TGetLink>::LinkToTail(T* ptr) {
    this->LinkNode(ptr, 2, NULL);
}

// OFFSET: [0x007046B0<CACHEENTRY, TSGetExplicitLink<CACHEENTRY>>, 0x009892A0/0x140705BF0<LFGListParty, TSGetExplicitLink<LFGListParty>>, 0x00924560<OVERRIDEKEYBINDING, TSGetExplicitLink<OVERRIDEKEYBINDING>>, 0x005F5DE0<CombatLogEntry, TSGetLink<CombatLogEntry>>, 0x0077BDB0/0x140468310<CSceneMaterial, TSGetExplicitLink<CSceneMaterial>>, 0x005E3420/0x140261EC0<OBJ_EFFECT_DEF_LOOKUP, TSGetExplicitLink<OBJ_EFFECT_DEF_LOOKUP>>, 0x0061FA80<CONSOLELINE, TSGetExplicitLink<CONSOLELINE>>, 0x0061C380/0x1402AA010<CHARCODEDESC, TSGetExplicitLink<CHARCODEDESC>>, 0x0061C840/0x1402AA6D0<KERNNODE, TSGetExplicitLink<KERNNODE>>, 0x0045ABA0/0x14006B380<WowClientDB2<ItemRecSparse>::DBCACHE, TSGetLink<WowClientDB2<ItemRecSparse>::DBCACHE>>, 0x004A0BC0/0x1400CEA30<DBCALLBACK, TSGetLink<DBCALLBACK>>, 0x008E3400/0x140610A80<CModelRecord, TSGetLink<CModelRecord>>, 0x00498910/0x1400C2BC0<WowClientDB2<ItemRec>::DBCACHE, TSGetLink<WowClientDB2<ItemRec>::DBCACHE>>, 0x008F2160<LFGListParty, TSGetLink<LFGListParty>>, 0x0046E8F0/0x1400846C0<WowClientDB2<ItemExtendedCostRec>::DBCACHE, TSGetExplicitLink<WowClientDB2<ItemExtendedCostRec>::DBCACHE>>, 0x004989A0/0x1400C2CA0<WowClientDB2<KeyChainRec>::DBCACHE, TSGetExplicitLink<WowClientDB2<KeyChainRec>::DBCACHE>>, 0x004A4A70/0x1400D3EB0<DBCache<PageTextCache_C, int, HASHKEY_INT>::DBCACHEHASH, TSGetExplicitLink<DBCache<PageTextCache_C, int, HASHKEY_INT>::DBCACHEHASH>>, 0x0062A430<BattlenetLogin::BN_REALM, TSGetLink<BattlenetLogin::BN_REALM>>, 0x004C2AD0/0x1400FD7E0<RGN, TSGetExplicitLink<RGN>>, 0x00526480/0x140175110<SIMPLEFONT, TSGetExplicitLink<SIMPLEFONT>>, 0x00542120<CMipBitsCache, TSGetExplicitLink<CMipBitsCache>>, 0x0054B3C0<CTextureBlob, TSGetExplicitLink<CTextureBlob>>, 0x006112E0<CGxuLight, TSGetLink<CGxuLight>>, 0x008BEA80<LightningObject, TSGetLink<LightningObject>>, 0x004894F0/0x1400AB610<NetClient::HELDMESSAGE, TSGetLink<NetClient::HELDMESSAGE>>, 0x0045AB40/0x14006B2F0<DBCACHECALLBACK, TSGetLink<DBCACHECALLBACK>>, 0x009B2CF0]
template <class T, class TGetLink>
T* TSList<T, TGetLink>::NewNode(uint32_t location, size_t extrabytes, uint32_t flags) {
    T* node = new (SMemAlloc(sizeof(T) + extrabytes, typeid(T).raw_name(), SLOG_OBJECT, flags | SMEM_FLAG_ZEROMEMORY)) T;

    if (location) {
        this->LinkNode(node, location, NULL);
    }

    return node;
}

// INLINED
template <class T, class TGetLink>
T* TSList<T, TGetLink>::Next(T* ptr) {
    return this->Link(ptr)->Next();
}

// OFFSET: [0x00653A30<CONSOLECOMMAND, TSGetExplicitLink<CONSOLECOMMAND>>]
template <class T, class TGetLink>
T* TSList<T, TGetLink>::Prev(T* ptr) {
    return this->Link(ptr)->Prev();
}

// OFFSET: [0x00984170<ADDONSTATELIST, TSGetLink<ADDONSTATELIST>>]
template <class T, class TGetLink>
T* TSList<T, TGetLink>::RawNext(T* ptr) {
    return this->Link(ptr)->RawNext();
}

// INLINED
template <class T, class TGetLink>
void TSList<T, TGetLink>::SetLinkOffset(ptrdiff_t linkoffset) {
    this->m_linkoffset = linkoffset;
    this->InitializeTerminator();
}

// INLINED
template <class T, class TGetLink>
T* TSList<T, TGetLink>::Tail() {
    return this->m_terminator.Prev();
}

// OFFSET: [0x009F5DF0/0x140791EC0<CGResearchFrame::PROJECT, TSGetLink<CGResearchFrame::PROJECT>>, 0x009E8E20<CGChatBubbleFrame, TSGetLink<CGChatBubbleFrame>>, 0x0048A140/0x1406071B0<CSimpleRegion, TSGetExplicitLink<CSimpleRegion>>, 0x00498260/0x14077A640<Grunt::Timer::Event, TSGetExplicitLink<Grunt::Timer::Event>>, 0x008A0180]
template <class T, class TGetLink>
void TSList<T, TGetLink>::UnlinkAll() {
    T* node;

    while ((node = this->Head()) != NULL) {
        this->UnlinkNode(node);
    }
}

// INLINED
template <class T, class TGetLink>
void TSList<T, TGetLink>::Combine(TSList<T, TGetLink>* list, uint32_t linktype, T* existingptr) {
    if (!list) {
        DebugValidate("list");
        SErrSetLastError(STORM_ERROR_INVALID_PARAMETER);
        return;
    }

    if (list == this) {
        DebugValidate("list != this");
        SErrSetLastError(STORM_ERROR_INVALID_PARAMETER);
        return;
    }

    if (list->m_linkoffset != this->m_linkoffset) {
        DebugValidate("list->m_linkoffset == m_linkoffset");
        SErrSetLastError(STORM_ERROR_INVALID_PARAMETER);
        return;
    }

    if (list->m_terminator.m_prevLink == &list->m_terminator) {
        return;
    }

    TSLink<T>* link = existingptr ? this->Link(existingptr) : &this->m_terminator;

    if (linktype == 1) {
        link = link->NextLink(this->m_linkoffset);
    }

    TSLink<T>* prev = link->m_prevLink;
    T* next = prev->m_next;
    prev->m_next = list->m_terminator.m_next;
    link->m_prevLink = list->m_terminator.m_prevLink;
    list->m_terminator.NextLink(this->m_linkoffset)->m_prevLink = prev;
    list->m_terminator.m_prevLink->m_next = next;
    list->m_terminator.m_prevLink = &list->m_terminator;
    list->m_terminator.m_next = reinterpret_cast<T*>(reinterpret_cast<uintptr_t>(&list->m_terminator) | 1);
}

// OFFSET: [0x009E8D50<CGNamePlateFrame, TSGetLink<CGNamePlateFrame>>, 0x0054B280<CLayoutFrame, TSGetExplicitLink<CLayoutFrame>>, 0x008E2A70<CModelRecord, TSGetLink<CModelRecord>>]
template <class T, class TGetLink>
void TSList<T, TGetLink>::UnlinkNode(T* ptr) {
    this->Link(ptr)->Unlink();
}

// INLINED
template <class T, int offset>
TSExplicitList<T, offset>::TSExplicitList() {
    this->SetLinkOffset(offset);
}

// INLINED
template <class T>
T* TSLinkedNode<T>::Next() {
    return this->m_link.Next();
}

// INLINED
template <class T>
T* TSLinkedNode<T>::RawNext() {
    return this->m_link.RawNext();
}

// OFFSET: [0x00601810<C4Plane>]
template <class T>
void TSSwap(T& a, T& b) {
    T temp = a;
    a = b;
    b = temp;
}

// INLINED
template <class T>
T& TSBaseArray<T>::operator[](uint32_t index) {
    return this->m_data[index];
}

// INLINED
template <class T>
uint32_t TSBaseArray<T>::Count() {
    return this->m_count;
}

// INLINED
template <class T>
const char* TSBaseArray<T>::MemFileName() {
    return typeid(T).raw_name();
}

// INLINED
template <class T>
int32_t TSBaseArray<T>::MemLineNo() {
    return -2;
}

// INLINED
template <class T>
T* TSBaseArray<T>::Ptr() {
    return this->m_data;
}

// OFFSET: 0x004BBC80/0x1400F5160
template <class T>
T* TSBaseArray<T>::Top() {
    return this->m_count ? &this->m_data[this->m_count - 1] : NULL;
}

// INLINED
template <class T>
TSFixedArray<T>::TSFixedArray() {
    this->m_alloc = 0;
    this->m_count = 0;
    this->m_data = NULL;
}

// INLINED
template <class T>
TSFixedArray<T>::TSFixedArray(const TSFixedArray<T>& source) {
    this->m_alloc = 0;
    this->m_count = 0;
    this->m_data = NULL;
    this->Set(source.m_count, source.m_data);
}

// OFFSET: [0x00505860/0x14014C370<TSExplicitList<FRAMEATTR, -572662307>>, 0x0091FDA0/0x1406820E0<WorldMapContinentInfo>, 0x00848470/0x140575D20<TSExplicitList<CGxShader, -572662307>>, 0x0071D0A0/0x1403F23F0<TSExplicitList<Liquid::Object, -572662307>>, 0x0064A8D0/0x1402E8050<TSExplicitList<DayNight::DNOverrideSky, -572662307>>, 0x00652930<DayNight::CZoneLight>, 0x0072E450/0x140408090<TSExplicitList<CMapDoodadDef, -572662307>>, 0x0072E4E0/0x140408150<TSExplicitList<CMapObjDef, -572662307>>, 0x0075EB00/0x140442990<CMapFootprintTexture>, 0x00765AD0<CWaterRipple>, 0x00735EC0/0x140412460<TSExplicitList<CMapDynamicTexture>>, 0x0077BC70/0x1404681E0<TSExplicitList<CSceneMaterial, -572662307>>, 0x00747B80<TSExplicitList<CMapObj, -572662307>>, 0x007398F0/0x1404165F0<TSExplicitList<DDMaterial, -572662307>>, 0x00446E30/0x140053030<TSExplicitList<_BANNEDADDON, -572662307>>, 0x004ED0A0<TSExplicitList<FrameFactoryNode, -572662307>>, 0x00526060/0x140174F80<TSExplicitList<SIMPLEFONT, -572662307>>, 0x00542720/0x140195870<TSExplicitList<CTexture, -572662307>>, 0x00547580/0x14019F100<TSExplicitList<FileNotification::DeferredFile, -572662307>>, 0x00611370/0x14029BB20<TSExplicitList<CLightList, -572662307>>, 0x00B314A0/0x1408FAFD0<TSExplicitList<LCDHANDLE, -572662307>>, 0x00548450/0x1401A0720<TSExplicitList<CModelBlob::CHashEntry, -572662307>>, 0x005E3A90/0x140262580<TSExplicitList<OBJ_EFFECT_PACKAGE_DEF_LOOKUP, -572662307>>, 0x00434820/0x14003DAD0<TSExplicitList<CShaderEffect, -572662307>>, 0x0054BA70/0x1401A5120<TSExplicitList<CTextureBlobTexture, -572662307>>, 0x00708C50<TSExplicitList<SE3SoundChunk, -572662307>>, 0x0070A1E0/0x1403D8E50<TSExplicitList<SE3SOUNDKITOBJECT_LOOKUP, -572662307>>, 0x00709D50/0x1403D8F10<TSExplicitList<SOUNDKITLOOKUP, -572662307>>, 0x00709DE0<TSExplicitList<SE3SoundEmitterLookup, -572662307>>, 0x0070A150/0x1403D8D90<TSExplicitList<SE3COMBINABLESOUNDLIST, -572662307>>, 0x00712D20/0x1403E4C80<TSExplicitList<SE3ChatSound, -572662307>>, 0x00714B60/0x1403E7420<TSExplicitList<SE3MemorySoundObject, -572662307>>, 0x004FC260/0x140140420<TSExplicitList<FILENAMEHASH, -572662307>>, 0x00512A40<CAsyncObject*>, 0x00855260/0x140585700<TSExplicitList<CGxDeviceD3d11::InputLayoutRec, -572662307>>, 0x008552F0/0x1405857C0<TSExplicitList<CGxDeviceD3d11::RasterizerStateRec, -572662307>>, 0x00855380/0x140585880<TSExplicitList<CGxDeviceD3d11::BlendStateRec, -572662307>>, 0x00855410/0x140585940<TSExplicitList<CGxDeviceD3d11::DepthStateRec, -572662307>>, 0x008554A0/0x140585A00<TSExplicitList<CGxDeviceD3d11::SamplerStateRec, -572662307>>, 0x00612380<GxuDebugTextObject>, 0x006123D0<GxuDebugText3DObject>, 0x00547610/0x14019F1C0<TSExplicitList<FileNotification::Manifest, -572662307>>, 0x00549B90/0x1401A2820<TSExplicitList<FONTHASHOBJ, -572662307>>, 0x004ED010<TSExplicitList<HashedNode, -572662307>>, 0x006545F0/0x1402F4F20<TSExplicitList<CONSOLECOMMAND, -572662307>>, 0x00656450/0x1402F7A30<TSExplicitList<CVar, -572662307>>, 0x0083E4E0/0x1405699B0<TSExplicitList<FrameScript_EventObject, -572662307>>, 0x005A3160/0x140213730<ShipPathLeg>, 0x009F28F0/0x14078D1A0<TSExplicitList<CGSpellActivationOverlay::COUNTERNODE, -572662307>>, 0x009F2980/0x14078D260<TSExplicitList<CGSpellActivationOverlay::PETAURANODE, -572662307>>, 0x009F6160/0x140792320<TSExplicitList<CGResearchFrame::BRANCH, -572662307>>, 0x009F61F0/0x1407923E0<TSExplicitList<CGResearchFrame::BLOBPOIHASH, -572662307>>, 0x009C9C50/0x140758CA0<TSExplicitList<CGCUFProfiles::PROFILENODE, -572662307>>, 0x009C9CE0/0x140758D60<TSExplicitList<CGCUFProfiles::OPTIONENTRY, -572662307>>, 0x00439BB0/0x1400445A0<TSExplicitList<CCameraModeEntry, -572662307>>, 0x0045E270/0x140070BF0<TSExplicitList<UIADDON::METADATA, -572662307>>, 0x00483C90/0x1400A3740<TSExplicitList<PLAYERPORTRAIT, -572662307>>, 0x00483D20/0x1400A3800<TSExplicitList<UNITPORTRAIT, -572662307>>, 0x00483DB0/0x1400A38C0<TSExplicitList<ITEMPORTRAIT, -572662307>>, 0x0048C2D0/0x1400AFBD0<TSExplicitList<WowClientDB2_Base::DBTABLE, -572662307>>, 0x0048E620/0x1400B2730<TSExplicitList<CGObject_C, -572662307>>, 0x00495C50/0x1400BB8F0<TSExplicitList<SPELLACTIVATIONINFO, -572662307>>, 0x004A2500/0x1400D0630<TSExplicitList<DBCache<CreatureStats_C,int,HASHKEY_INT>::DBCACHEHASH, -572662307>>, 0x004A2590/0x1400D06F0<TSExplicitList<DBCache<CreatureStats_C,int,HASHKEY_INT>::REVERSEENTRY, -572662307>>, 0x004A2620/0x1400D07B0<TSExplicitList<DBCache<GameObjectStats_C,int,HASHKEY_INT>::DBCACHEHASH, -572662307>>, 0x004A26B0/0x1400D0870<TSExplicitList<DBCache<GameObjectStats_C,int,HASHKEY_INT>::REVERSEENTRY, -572662307>>, 0x004A2740/0x1400D0930<TSExplicitList<DBCache<NPCText,int,HASHKEY_INT>::DBCACHEHASH, -572662307>>, 0x004A27D0/0x1400D09F0<TSExplicitList<DBCache<NPCText,int,HASHKEY_INT>::REVERSEENTRY, -572662307>>, 0x004A2860/0x1400D0AB0<TSExplicitList<DBCache<NameCache,unsigned __int64,CHashKeyGUID>::DBCACHEHASH, -572662307>>, 0x004A28F0/0x1400D0B70<TSExplicitList<DBCache<NameCache,unsigned __int64,CHashKeyGUID>::REVERSEENTRY, -572662307>>, 0x004A2980/0x1400D0C30<TSExplicitList<DBCache<GuildStats_C,unsigned __int64,CHashKeyGUID>::DBCACHEHASH, -572662307>>, 0x004A2A10/0x1400D0CF0<TSExplicitList<DBCache<GuildStats_C,unsigned __int64,CHashKeyGUID>::REVERSEENTRY, -572662307>>, 0x004A2AA0/0x1400D0DB0<TSExplicitList<DBCache<QuestCache,int,HASHKEY_INT>::DBCACHEHASH, -572662307>>, 0x004A2B30/0x1400D0E70<TSExplicitList<DBCache<QuestCache,int,HASHKEY_INT>::REVERSEENTRY, -572662307>>, 0x004A2BC0/0x1400D0F30<TSExplicitList<DBCache<PageTextCache_C,int,HASHKEY_INT>::DBCACHEHASH, -572662307>>, 0x004A2C50/0x1400D0FF0<TSExplicitList<DBCache<PageTextCache_C,int,HASHKEY_INT>::REVERSEENTRY, -572662307>>, 0x004A2CE0/0x1400D10B0<TSExplicitList<DBCache<PetNameCache,int,HASHKEY_INT>::DBCACHEHASH, -572662307>>, 0x004A2D70/0x1400D1170<TSExplicitList<DBCache<PetNameCache,int,HASHKEY_INT>::REVERSEENTRY, -572662307>>, 0x004A2E00/0x1400D1230<TSExplicitList<DBCache<CGPetition,int,HASHKEY_INT>::DBCACHEHASH, -572662307>>, 0x004A2E90/0x1400D12F0<TSExplicitList<DBCache<CGPetition,int,HASHKEY_INT>::REVERSEENTRY, -572662307>>, 0x004A2F20/0x1400D13B0<TSExplicitList<DBCache<ItemTextCache_C,unsigned __int64,CHashKeyGUID>::DBCACHEHASH, -572662307>>, 0x004A2FB0/0x1400D1470<TSExplicitList<DBCache<ItemTextCache_C,unsigned __int64,CHashKeyGUID>::REVERSEENTRY, -572662307>>, 0x004A3040/0x1400D1530<TSExplicitList<DBCache<WardenCachedModule,CWardenKey,CWardenKey>::DBCACHEHASH, -572662307>>, 0x004A30D0/0x1400D15F0<TSExplicitList<DBCache<WardenCachedModule,CWardenKey,CWardenKey>::REVERSEENTRY, -572662307>>, 0x004A3160/0x1400D16B0<TSExplicitList<DBCache<ArenaTeamCache,int,HASHKEY_INT>::DBCACHEHASH, -572662307>>, 0x004A31F0/0x1400D1770<TSExplicitList<DBCache<ArenaTeamCache,int,HASHKEY_INT>::REVERSEENTRY, -572662307>>, 0x004A3280/0x1400D1830<TSExplicitList<DBCache<DanceCache,int,HASHKEY_INT>::DBCACHEHASH, -572662307>>, 0x004A3310/0x1400D18F0<TSExplicitList<DBCache<DanceCache,int,HASHKEY_INT>::REVERSEENTRY, -572662307>>, 0x004C5660/0x1401007D0<REALM_INFO>, 0x004DE130/0x14011CBA0<TSExplicitList<TALKINGPLAYER, -572662307>>, 0x004DE1C0/0x14011CC60<TSExplicitList<MUTEDPLAYER, -572662307>>, 0x004E5760/0x140127230<TSExplicitList<DECLINEDWORD, -572662307>>, 0x00545790/0x14019C610<TSExplicitList<ProfileInternal::KEYVALUE, -572662307>>, 0x005696A0/0x1401C8900<TSExplicitList<TRADESKILLLINE, -572662307>>, 0x00569730/0x1401C89C0<TSExplicitList<PROFICIENCYSPELL, -572662307>>, 0x0058F500/0x1401FA0E0<TSExplicitList<CustomFallbackPath, -572662307>>, 0x00595C80/0x140203270<TSExplicitList<VoidItem_C, -572662307>>, 0x005A6670/0x140217A10<TSExplicitList<INVENTORYART, -572662307>>, 0x005A6700/0x140217AD0<TSExplicitList<ITEMBYNAME, -572662307>>, 0x005CF4F0/0x140248630<TSExplicitList<UnitThreat, -572662307>>, 0x005E20D0/0x140260270<TSExplicitList<OBJ_EFFECT_GROUP_LOOKUP, -572662307>>, 0x005E2160/0x140260330<TSExplicitList<OBJ_EFFECT_REF_LIST_LOOKUP, -572662307>>, 0x005E21F0/0x1402603F0<TSExplicitList<OBJ_EFFECT_DEF_LOOKUP, -572662307>>, 0x005E2280/0x1402604B0<TSExplicitList<OBJ_EFFECT_GROUP_DEF_LOOKUP, -572662307>>, 0x005E2310/0x140260570<TSExplicitList<OBJ_EFFECT_PACKAGE_DEF_LOOKUP, -572662307>>, 0x005E2D80/0x140261560<TSExplicitList<OBJ_EFFECT_REF_LOOKUP, -572662307>>, 0x0061BE40/0x1402A99A0<TSExplicitList<CHARCODEDESC, -572662307>>, 0x0061C1B0/0x1402A9E00<TSExplicitList<KERNNODE, -572662307>>, 0x0062CA40/0x1402C0370<TSExplicitList<ZONESOUNDOVERRIDELOOKUP, -572662307>>, 0x0062EE00/0x1402C3230<AmbFlavorListHead>, 0x00632FA0/0x1402C87C0<TSExplicitList<DSP_CHAIN_HEAD_LOOKUP, -572662307>>, 0x00633C60/0x1402C9A90<TSExplicitList<FOOTSTEPSNDCACHE, -572662307>>, 0x006ABCD0<LFGuildApplicationData>, 0x006C8110<JamGuildRewardItem>, 0x006C82C0<LFGuildBrowseData>, 0x006C82F0<LFGuildRecruitData>, 0x007043F0<TSExplicitList<CACHEENTRY, -572662307>>, 0x00792D40/0x140484AC0<TSExplicitList<CFileName, -572662307>>, 0x00809540<CharacterSelectionDisplay>, 0x008AE5F0<TSExplicitList<ITEMCOOLDOWNHASHNODE, -572662307>>, 0x008AE680/0x1405F3090<TSExplicitList<DESTLOCSPELLCASTINDEXHASHNODE, -572662307>>, 0x008C0A80/0x14060AA90<TSExplicitList<RACECLASSINFO, -572662307>>, 0x008D8C20<TSExplicitList<SAVEDVARIABLE, -572662307>>, 0x008D8CB0/0x14064C480<TSExplicitList<TaintEvent, -572662307>>, 0x008EF5C0/0x14061F650<TSExplicitList<CHATTHROTTLE, -572662307>>, 0x008EF650/0x14061F710<TSExplicitList<RegisteredAddonPrefix, -572662307>>, 0x008EF6E0<TSExplicitList<WORDLIST, -572662307>>, 0x008EF770<TSExplicitList<PROFANITYCHECK, -572662307>>, 0x008EF800/0x14061F950<TSExplicitList<SPAMCHECK, -572662307>>, 0x00900B50<TSExplicitList<USEABLESTYLE, -572662307>>, 0x00909DD0/0x140663430<TSExplicitList<ACCOUNTIDCACHE, -572662307>>, 0x00913F00/0x140670A40<TSExplicitList<SPELLSBYNAME, -572662307>>, 0x00913FA0/0x140670B00<TSExplicitList<OVERRIDE_SPELL_NODE, -572662307>>, 0x00914030/0x140670BC0<TSExplicitList<OVERRIDE_SPELL_BY_NAME_NODE, -572662307>>, 0x00935010<GuildNewsEvent>, 0x00939A10/0x1406A3C10<TSExplicitList<CGGuildInfo::GuildRecipesKnown, -572662307>>, 0x00939AA0<TSExplicitList<CGGuildInfo::SKILLLINESPELLBITS, -572662307>>, 0x0093AE60/0x1406A3CD0<TSExplicitList<CGGuildInfo::SKILLLINESPELLS, -572662307>>, 0x0094DAE0/0x1406BD040<TSExplicitList<ACHIEVEMENT, -572662307>>, 0x0094DB70/0x1406BD100<TSExplicitList<GUILDACHIEVEMENTMEMBERLIST, -572662307>>, 0x00964900/0x1406D9EC0<TSExplicitList<CGQuestLog::QUESTCOMPLETIONNPC, -572662307>>, 0x00987C60/0x140703BD0<TSExplicitList<LFGListParty, -572662307>>, 0x00987CF0/0x140703C90<TSExplicitList<LFGListPlayer, -572662307>>, 0x009A48C0/0x14072A580<TSExplicitList<CFlyout, -572662307>>, 0x009A4950/0x14072A640<TSExplicitList<SpellToFlyoutMapItem, -572662307>>, 0x009A8510/0x14072F1C0<TSExplicitList<CGReputationInfo::FACTION, -572662307>>, 0x009B85A0/0x140742E30<TSExplicitList<KNOWNDANCES, -572662307>>, 0x009C6060/0x140754190<TSExplicitList<MACRONODE, -572662307>>, 0x009CC140/0x14075BF20<TSExplicitList<CGAutoCompleteName::AUTOCOMPLETE_CHARACTER_HASHNODE, -572662307>>, 0x009CC1D0/0x14075BFE0<TSExplicitList<CGAutoCompleteName::AUTOCOMPLETE_BATTLENET_HASHNODE, -572662307>>, 0x009CDD20/0x14075E6F0<TSExplicitList<WORLDSTATE, -572662307>>, 0x009DEBB0/0x140774E60<TSExplicitList<CGQuestObjectiveCache::REQUIREDUNITNODE, -572662307>>, 0x009DEC40/0x140774F20<TSExplicitList<CGQuestObjectiveCache::REQUIREDITEMNODE, -572662307>>, 0x009E4270/0x14077B8D0<TSExplicitList<TAXIPATHNODESCLIENT, -572662307>>, 0x009FD330/0x14079B230<TSExplicitList<CGIncomingResurrection::INCOMINGRESLIST, -572662307>>, 0x00A0EA00<TSExplicitList<MacroConditions, -572662307>>, 0x00B32970/0x1408FCC80<TSExplicitList<BITMAPBYICON, -572662307>>, 0x0045BBA0<const char*>, 0x0098EB90<const SkillLineAbilityRec*>, 0x004E52C0/0x140126C50<CPhaseShift>, 0x0069F240<JamCliStructAddonPrefix>, 0x00616E30/0x1402A3CE0<GxuDebugTextureObject>, 0x0061AD10<GxuDebugShapeObject>, 0x00A0EE50/0x1407B1550<MacroConditions>, 0x00486F90/0x1400A80A0<CObjectHeapList>, 0x006D9150/0x1403965D0<JamQuestCompletionNPC>, 0x006D91C0/0x140396670<ClientPVPLogData_Player>, 0x006DE9A0/0x14039D7C0<JamGuildNewsEvent>, 0x006ECA50/0x1403AD4D0<JamGuildBankItemInfo>, 0x006ECAC0/0x14039D860<ClientLFGBlackList>, 0x009E4090<TSGrowableArray<TAXIROUTELINE>>, 0x00495940<TSGrowableArray<SpellEffectRec const *>>, 0x004B1F20/0x1400E87C0<TSGrowableArray<int>>, 0x0098C570/0x14070A770<REFORGE_OPTION>, 0x0040C6E0<CLoadingScreenShipInfo<TaxiPathNodeRec>::PATHINFO>, 0x00963CB0/0x1406D8AD0<BLOB>, 0x0044B050/0x140058300<TSExplicitList<MOUSELOOKBINDING, -572662307>>, 0x0045D7A0/0x14006FD40<TSExplicitList<ADDONSTATE, -572662307>>, 0x0045D830/0x14006FE00<TSExplicitList<UIADDON, -572662307>>, 0x004C2930/0x1400FD5E0<TSExplicitList<RGN, -572662307>>, 0x004D9640/0x140117990<TSExplicitList<TIMESTAMPSTRUCT, -572662307>>, 0x00545470/0x14019C180<TSExplicitList<ProfileInternal::SECTION, -572662307>>, 0x006155F0/0x1402A12B0<TSExplicitList<BATCHEDRENDERFONTDESC, -572662307>>, 0x00622800/0x1402B3870<TSExplicitList<FACEDATA, -572662307>>, 0x0051C110<CSimpleMessageFrameLine>, 0x0061C240/0x1402A9EC0<TEXTURECACHEROW>, 0x008EE260/0x14061DC30<PENDINGUSERLIST>, 0x00923660/0x140687340<TSExplicitList<KEYCOMMAND, -572662307>>, 0x009236F0/0x140687400<TSExplicitList<KEYBINDING, -572662307>>, 0x00923780/0x1406874C0<TSExplicitList<OVERRIDEKEYBINDING, -572662307>>, 0x00923810/0x140687580<TSExplicitList<MODIFIEDCLICK, -572662307>>, 0x009238A0/0x140687640<TSExplicitList<CLICKFRAME, -572662307>>, 0x0099D760/0x140720800<TSExplicitList<TalentGroupItem_C, -572662307>>, 0x0099D7F0/0x1407208C0<TSExplicitList<TalentGroupTierItem_C, -572662307>>, 0x009B40F0/0x14073D1D0<TSExplicitList<EVENTDATEHASH, -572662307>>, 0x009B4180/0x14073D290<TSExplicitList<DYNAMICHOLIDAYHASH, -572662307>>, 0x00B31780/0x1408FB3D0<CHATMESSAGE>, 0x008F1810/0x140622330, 0x004EBD90/0x14012F040<CXMLAttribute>, 0x00632610/0x1402C7DA0, 0x00B05630/0x1408C8050<SockAddr>]
template <class T>
TSFixedArray<T>::~TSFixedArray() {
    for (uint32_t i = 0; i < this->m_count; i++) {
        this->m_data[i].~T();
    }

    if (this->m_data) {
        SMemFree(this->m_data, this->MemFileName(), this->MemLineNo(), 0);
    }
}

// OFFSET: [0x00620A70<CImVector>, 0x0060D8A0<DEATTHUDSOUNDINFO>, 0x0040B930<int>, 0x0046A5B0<CSimpleFontString*>, 0x00616CE0<C3Vector>, 0x006251F0<CHARACTER_INFO>, 0x0064E440<float>, 0x0064E480<C2Vector>, 0x009E33C0<TAXILINE>, 0x009E3400/0x14077A6D0<TAXIROUTELINE>, 0x00B2BBE0<LightningJoint>, 0x004824E0<uint8_t>, 0x004955C0<uint32_t>, 0x0094A510/0x1406B8530<GuildAchievementMember>, 0x00628A60/0x1402BAB90<BattlenetLogin::GameAccount>]
template <class T>
void TSFixedArray<T>::Clear() {
    this->~TSFixedArray<T>();
    this->m_alloc = 0;
    this->m_count = 0;
    this->m_data = NULL;
}

// OFFSET: [0x00486890/0x1400A78D0<CObjectHeap>, 0x004B1E90/0x1400E86F0<TSGrowableArray<int>>]
template <class T>
void TSFixedArray<T>::ReallocAndClearData(uint32_t count) {
    for (uint32_t i = 0; i < this->m_count; i++) {
        this->m_data[i].~T();
    }

    if (count != this->m_count) {
        this->m_alloc = count;

        if (this->m_data || count) {
            this->m_data = static_cast<T*>(SMemReAlloc(this->m_data, sizeof(T) * count, this->MemFileName(), this->MemLineNo(), 0));
        }
    }
}

// OFFSET: [0x00554F00<unsigned int>, 0x009CEF10/0x14075FA70<C3Vector>, 0x00486FE0/0x1400A8190<CObjectHeapList>, 0x006C1BD0<unsigned __int64>, 0x00920200/0x1406826C0<WorldMapContinentInfo>, 0x00A0EA90/0x1407B10B0<MacroCondition>, 0x00843DE0/0x140570350<CGxFormat>, 0x0084A9A0/0x140578EA0<CGxMonitorMode>, 0x00622280/0x1402B3080<VERT>, 0x0061DA90/0x1402AC270<GXUFONTHYPERLINKINFO>, 0x0061DB40/0x1402AC3A0<GXUEMBEDDEDTEXTUREINFO>, 0x00615AF0/0x1402A1A70<GRADIENTINFO>, 0x00555050/0x1401B18B0<ITEMSWAP>, 0x005852C0/0x1401ED480<SpellCastTargetPoint>, 0x005AC6C0/0x14021EB80<DelayedSpellVisualKit>, 0x005ACA20/0x14021EEB0<AuraData>, 0x008BC2E0/0x140604EB0<LightningObject::Bolt>, 0x008BC390<TSGrowableArray<const SpellVisualKitModelAttachRec*>*>, 0x008BC430/0x1406050A0<const SpellVisualKitModelAttachRec*>, 0x008C0470<SkillLineLinkInfo>, 0x0087B960/0x1405B3840<OsIMECandidate>, 0x0087A510/0x1405B0BA0<W32Joystick>, 0x00523FC0/0x140172520<MOVIECAPTION>, 0x00B13AE0/0x1408D97E0<CParticleEmitter2*>, 0x00B28900/0x1408F10F0<LightningJoint>, 0x00B289B0/0x1408F1210<CLightning*>, 0x00717630/0x1403EABB0<Liquid::DisplMapPerturbation*>, 0x007176D0/0x1403EACA0<Liquid::DisplMapPerturbationMesh*>, 0x0071C4E0/0x1403F1450<Liquid::IMaterial*>, 0x00B3C2C0/0x140909200<Liquid::CInstance*>, 0x0071CF30<TSExplicitList<Liquid::Object, -572662307>>, 0x00647670/0x1402E3F80<DayNight::AreaLightOverride>, 0x0064EF40/0x1402EDFD0<TSExplicitList<DayNight::DNOverrideSky, -572662307>>, 0x0064ABC0/0x1402E8410<const LightRec*>, 0x00652590/0x1402F26B0<const ZoneLightPointRec*>, 0x006529A0/0x1402F2B20<DayNight::CZoneLight>, 0x0072C890/0x140405A10<CWorldMap::CMapTableEntry>, 0x0072CEA0<CMapLight*>, 0x00726DC0/0x1403FE1E0<CMapAreaLow*>, 0x00726E60/0x1403FE2D0<CMapObjDef*>, 0x0072E170/0x140407CF0<TSExplicitList<CMapDoodadDef, -572662307>>, 0x0072E2E0/0x140407EC0<TSExplicitList<CMapObjDef, -572662307>>, 0x0075EE70/0x140442E70<CSceneMaterial*>, 0x0075D2D0/0x140440C70<CGxVertexPT2>, 0x0075E980/0x1404427A0<CMapFootprintTexture>, 0x00762FA0/0x1404481E0<SMapObjDefVectorIntersect>, 0x00765BB0<CWaterRipple>, 0x00755200/0x140436650<CPortalView>, 0x00755340/0x140436740<CSortEntryDoodadDef>, 0x00794210/0x140486950<C4Vector>, 0x00776FA0<CBarrier::SContact>, 0x00797DD0/0x14048B6D0<CMapLowDetail::CMapAreaLowTableEntry>, 0x0079A2D0/0x14048E740<CSortEntry>, 0x00776EF0<CGxVertexPCT0T1>, 0x00784B80/0x140473830<CMap::CMapAreaTableEntry>, 0x007865E0/0x140475760<CMapAreaTexture>, 0x0077CCC0/0x1404696F0<LiquidChunkAndDistance>, 0x0077D480<World::LiquidQueryResult>, 0x0077F630/0x14046C5A0<CGxVertexPC>, 0x0077BB00/0x140468010<TSExplicitList<CSceneMaterial, -572662307>>, 0x0077A3F0/0x1404662E0<RElement>, 0x00747A10/0x140426590<TSExplicitList<CMapObj, -572662307>>, 0x00753870/0x140434960<CMapObjDefGroup*>, 0x00753950/0x140434A50<SPortalExt>, 0x00753A00/0x140434B70<CWorldView::CVisRenderChunk>, 0x00753E10/0x140434EA0<CSortEntryChunk>, 0x00736310/0x140412990<CDetailDoodadInstAdd>, 0x00736460/0x140412B70<DDMaterial*>, 0x00739780<TSExplicitList<DDMaterial, -572662307>>, 0x0079B860/0x1404900E0<C4Plane>, 0x0079B910/0x1404901E0<CWorldOcclusionVolume>, 0x008429E0/0x14056EE60<CGxDevice::ShaderModelCall>, 0x004C22B0<TSExplicitList<RGN, -572662307>>, 0x00848D30/0x140576880<CGxShaderPermute>, 0x008315D0/0x140552C10<unsigned char *>, 0x0053ECC0/0x140199820<CStreamingObj>, 0x0045CB80/0x14006EE70<TSExplicitList<UIADDON::METADATA, -572662307>>, 0x004FC0F0/0x140140250<TSExplicitList<FILENAMEHASH, -572662307>>, 0x005056F0/0x14014C1A0<TSExplicitList<FRAMEATTR, -572662307>>, 0x00525EF0/0x140174DB0<TSExplicitList<SIMPLEFONT, -572662307>>, 0x005427B0<TSExplicitList<CTexture, -572662307>>, 0x00545190/0x14019BDE0<TSExplicitList<ProfileInternal::KEYVALUE, -572662307>>, 0x00545300/0x14019BFB0<TSExplicitList<ProfileInternal::SECTION, -572662307>>, 0x0054B730<TSExplicitList<CTextureBlobTexture, -572662307>>, 0x0061C040/0x1402A9C30<TSExplicitList<KERNNODE, -572662307>>, 0x00621260/0x1402B1930<TSExplicitList<BATCHEDRENDERFONTDESC, -572662307>>, 0x00712BB0/0x1403E4AB0<TSExplicitList<SE3ChatSound, -572662307>>, 0x007149F0/0x1403E7250<TSExplicitList<SE3MemorySoundObject, -572662307>>, 0x0087DC50/0x1405B6640<TSExplicitList<OsNet::NETSELSOCKPTR, -572662307>>, 0x00549A20<TSExplicitList<FONTHASHOBJ, -572662307>>, 0x00622690<TSExplicitList<FACEDATA, -572662307>>, 0x00708CE0<TSExplicitList<SE3SoundChunk, -572662307>>, 0x00709FE0/0x1403D75E0<TSExplicitList<SE3SOUNDKITOBJECT_LOOKUP, -572662307>>, 0x00708FC0<TSExplicitList<SOUNDKITLOOKUP, -572662307>>, 0x00708E50<TSExplicitList<SE3SoundEmitterLookup, -572662307>>, 0x00709E70/0x1403D7410<TSExplicitList<SE3COMBINABLESOUNDLIST, -572662307>>, 0x007061B0/0x1403D3680<SE3SoundKitDef*>, 0x005125D0/0x14015C580<CAsyncObject*>, 0x0086D860/0x1405A2120<TSExplicitList<CGxDeviceD3d11::InputLayoutRec, -572662307>>, 0x0086D9D0/0x1405A22F0<TSExplicitList<CGxDeviceD3d11::RasterizerStateRec, -572662307>>, 0x0086DB40/0x1405A24C0<TSExplicitList<CGxDeviceD3d11::BlendStateRec, -572662307>>, 0x0086DCB0/0x1405A2690<TSExplicitList<CGxDeviceD3d11::DepthStateRec, -572662307>>, 0x0086DE20/0x1405A2860<TSExplicitList<CGxDeviceD3d11::SamplerStateRec, -572662307>>, 0x00612420/0x14029D400<GxuDebugTextObject>, 0x00849290/0x140577080<TSExplicitList<CGxShader, -572662307>>, 0x0053EB80/0x140199640<void (*)()>, 0x0053EC20/0x140199730<int32_t (*)()>, 0x005473A0<TSExplicitList<FileNotification::DeferredFile, -572662307>>, 0x005484E0/0x1401A07E0<TSExplicitList<CModelBlob::CHashEntry, -572662307>>, 0x004ECA00<TSExplicitList<HashedNode, -572662307>>, 0x004ECB70<TSExplicitList<FrameFactoryNode, -572662307>>, 0x00611170<TSExplicitList<CLightList, -572662307>>, 0x00899E40/0x1405D95B0<FFX::PortalEffect>, 0x00654470/0x1402F4CD0<TSExplicitList<CONSOLECOMMAND, -572662307>>, 0x00655EA0<TSExplicitList<CVar, -572662307>>, 0x0083E170<TSExplicitList<FrameScript_EventObject, -572662307>>, 0x00599D10<MapObjSpeedTransition>, 0x008C08F0<TSExplicitList<RACECLASSINFO, -572662307>>, 0x00554DA0/0x1401B14E0<PartyMemberAuraStates>, 0x00554E50<_FOUNDPARAM>, 0x0058BF60/0x1401F56F0<AnimKitBoneSetInst>, 0x0058C3D0/0x1401F5D90<AnimKitDeferredCommand>, 0x00590FB0/0x1401FC890<AnimKitDesiredBoneSetInst>, 0x005AC770/0x14021ECA0<HealPredictionInfo>, 0x006C1F90<RECTF>, 0x006E70E0/0x1403A9110<JamGuildNewsEvent>, 0x009F24B0<TSExplicitList<CGSpellActivationOverlay::COUNTERNODE, -572662307>>, 0x009F2620<TSExplicitList<CGSpellActivationOverlay::PETAURANODE, -572662307>>, 0x009F20C0/0x14078C680<CGSpellActivationOverlay::WatchedSpellInfo>, 0x009F4770<const ResearchBranchRec*>, 0x009F5E60<TSExplicitList<CGResearchFrame::BRANCH, -572662307>>, 0x009F5FD0/0x140792130<TSExplicitList<CGResearchFrame::BLOBPOIHASH, -572662307>>, 0x009C82B0/0x140756B40<CUFProfileInfo*>, 0x009C9650/0x1407583E0<TSExplicitList<CGCUFProfiles::PROFILENODE, -572662307>>, 0x009C97C0/0x1407585B0<TSExplicitList<CGCUFProfiles::OPTIONENTRY, -572662307>>, 0x00672FE0/0x14031B3A0<JamCliCUFProfile>, 0x009EED20<GlyphItem*>, 0x0099D3F0/0x140720360<TSExplicitList<TalentGroupItem_C, -572662307>>, 0x0099D560/0x140720530<TSExplicitList<TalentGroupTierItem_C, -572662307>>, 0x00403000/0x1400025B0<SArchive*>, 0x00438E80/0x1400432D0<CCameraTransition>, 0x00439790<TSExplicitList<CCameraModeEntry, -572662307>>, 0x00446C30<TSExplicitList<_BANNEDADDON, -572662307>>, 0x0044AE90/0x1400580C0<TSExplicitList<MOUSELOOKBINDING, -572662307>>, 0x0045CE60<TSExplicitList<UIADDON, -572662307>>, 0x00461E70/0x140074C60<CCameraTrack::DeferredCameraAction>, 0x00482920<TSExplicitList<PLAYERPORTRAIT, -572662307>>, 0x00482A90<TSExplicitList<UNITPORTRAIT, -572662307>>, 0x00482C00<TSExplicitList<ITEMPORTRAIT, -572662307>>, 0x004879E0/0x1400A8EC0<WowConnection*>, 0x0048BE10/0x1400AF560<TSExplicitList<WowClientDB2_Base::DBTABLE, -572662307>>, 0x0048E910/0x1400B2D80<TSExplicitList<CGObject_C, -572662307>>, 0x00495AE0<TSExplicitList<SPELLACTIVATIONINFO, -572662307>>, 0x004A04B0/0x1400CE0E0<TSExplicitList<DBCache<NameCache,unsigned __int64,CHashKeyGUID>::REVERSEENTRY, -572662307>>, 0x004A0C20/0x1400CEAC0<TSExplicitList<DBCache<CreatureStats_C,int,HASHKEY_INT>::DBCACHEHASH, -572662307>>, 0x004A0D90/0x1400CEC90<TSExplicitList<DBCache<GameObjectStats_C,int,HASHKEY_INT>::DBCACHEHASH, -572662307>>, 0x004A0F00/0x1400CEE60<TSExplicitList<DBCache<NPCText,int,HASHKEY_INT>::DBCACHEHASH, -572662307>>, 0x004A1070/0x1400CF030<TSExplicitList<DBCache<NameCache,unsigned __int64,CHashKeyGUID>::DBCACHEHASH, -572662307>>, 0x004A11E0/0x1400CF200<TSExplicitList<DBCache<GuildStats_C,unsigned __int64,CHashKeyGUID>::DBCACHEHASH, -572662307>>, 0x004A1350/0x1400CF3D0<TSExplicitList<DBCache<QuestCache,int,HASHKEY_INT>::DBCACHEHASH, -572662307>>, 0x004A14C0/0x1400CF5A0<TSExplicitList<DBCache<PageTextCache_C,int,HASHKEY_INT>::DBCACHEHASH, -572662307>>, 0x004A1630/0x1400CF770<TSExplicitList<DBCache<PetNameCache,int,HASHKEY_INT>::DBCACHEHASH, -572662307>>, 0x004A17A0/0x1400CF940<TSExplicitList<DBCache<CGPetition,int,HASHKEY_INT>::DBCACHEHASH, -572662307>>, 0x004A1910/0x1400CFB10<TSExplicitList<DBCache<ItemTextCache_C,unsigned __int64,CHashKeyGUID>::DBCACHEHASH, -572662307>>, 0x004A1A80/0x1400CFCE0<TSExplicitList<DBCache<WardenCachedModule,CWardenKey,CWardenKey>::DBCACHEHASH, -572662307>>, 0x004A1BF0/0x1400CFEB0<TSExplicitList<DBCache<ArenaTeamCache,int,HASHKEY_INT>::DBCACHEHASH, -572662307>>, 0x004A1D60/0x1400D0080<TSExplicitList<DBCache<DanceCache,int,HASHKEY_INT>::DBCACHEHASH, -572662307>>, 0x004D0930/0x14010D140<Grunt::CdKey::Info>, 0x004DCB50<TSExplicitList<TALKINGPLAYER, -572662307>>, 0x004DCCC0<TSExplicitList<MUTEDPLAYER, -572662307>>, 0x004E4060/0x140125430<PhaseGroupLookup>, 0x004E4110<CPhaseMapTracker>, 0x004E55C0<TSExplicitList<DECLINEDWORD, -572662307>>, 0x005697C0<TSExplicitList<TRADESKILLLINE, -572662307>>, 0x00569930<TSExplicitList<PROFICIENCYSPELL, -572662307>>, 0x0058B9F0/0x1401F5050<CAnimReplacementSetDef*>, 0x0058C0B0/0x1401F58E0<CAnimKitInst*>, 0x0058C290/0x1401F5BB0<AnimKitSegmentBoneSetInst*>, 0x0058C330/0x1401F5CA0<AnimKitSegmentInst*>, 0x0058E9E0<TSExplicitList<CustomFallbackPath, -572662307>>, 0x00595A40/0x140203040<TSExplicitList<VoidItem_C, -572662307>>, 0x005A6390<TSExplicitList<INVENTORYART, -572662307>>, 0x005A6500<TSExplicitList<ITEMBYNAME, -572662307>>, 0x005AC830/0x14021EDA0<UnitAttackIndicator>, 0x005CF380/0x140248460<TSExplicitList<UnitThreat, -572662307>>, 0x005E15F0<TSExplicitList<OBJ_EFFECT_GROUP_DEF_REF_LIST_LOOKUP, -572662307>>, 0x005E1760/0x14025F540<TSExplicitList<OBJ_EFFECT_REF_LOOKUP, -572662307>>, 0x005E18D0<TSExplicitList<OBJ_EFFECT_GROUP_LOOKUP, -572662307>>, 0x005E1A40<TSExplicitList<OBJ_EFFECT_REF_LIST_LOOKUP, -572662307>>, 0x005E1BB0<TSExplicitList<OBJ_EFFECT_DEF_LOOKUP, -572662307>>, 0x005E1D20<TSExplicitList<OBJ_EFFECT_GROUP_DEF_LOOKUP, -572662307>>, 0x005E1E90<TSExplicitList<OBJ_EFFECT_PACKAGE_DEF_LOOKUP, -572662307>>, 0x005ED480/0x14026EE50<CVehiclePassenger_C*>, 0x005EE440<VehicleAnimDeferredSpline>, 0x0060C220/0x140295940<DEATTHUDSOUNDINFO>, 0x0061BED0<TSExplicitList<CHARCODEDESC, -572662307>>, 0x0061D9E0<VERT>, 0x0061DBF0/0x1402AC4E0<GRADIENTINFO>, 0x00624AE0/0x1402B62D0<CHARACTER_INFO>, 0x00627BA0/0x1402B9C50<BattlenetLogin::GameAccount>, 0x0062E9C0<TSExplicitList<ZONESOUNDOVERRIDELOOKUP, -572662307>>, 0x0062EBA0/0x1402C2E70<AmbFlavorListHead>, 0x00631800/0x1402C6BB0<TEXTEMOTELOOKUP>, 0x006318B0<TEXTEMOTELOOKUP*>, 0x00632A80<TSExplicitList<DSP_CHAIN_HEAD_LOOKUP, -572662307>>, 0x00633AE0<TSExplicitList<FOOTSTEPSNDCACHE, -572662307>>, 0x00637860/0x1402CE630<C2iVector>, 0x006C1E10/0x1403775F0<JamCategoryCooldown>, 0x006C25B0<JamGuildBankSocketEnchant>, 0x006C2660/0x140378120<JamGuildRankTabPermissions>, 0x00704200<TSExplicitList<CACHEENTRY, -572662307>>, 0x0076AC00/0x1404529B0<World::HitTestDebugItem::TextureInfo>, 0x00792510<TSExplicitList<CFileName, -572662307>>, 0x008ADE40<TSExplicitList<ITEMCOOLDOWNHASHNODE, -572662307>>, 0x008ADFB0<TSExplicitList<DESTLOCSPELLCASTINDEXHASHNODE, -572662307>>, 0x008D6EF0/0x14064A890<TSExplicitList<SAVEDVARIABLE, -572662307>>, 0x008D7060<TSExplicitList<TaintEvent, -572662307>>, 0x008EE2F0<TSExplicitList<WORDLIST, -572662307>>, 0x008EE460/0x14061DEA0<TSExplicitList<CHATTHROTTLE, -572662307>>, 0x008EE5D0/0x14061E070<TSExplicitList<RegisteredAddonPrefix, -572662307>>, 0x008EE740<TSExplicitList<PROFANITYCHECK, -572662307>>, 0x008EE8B0<TSExplicitList<SPAMCHECK, -572662307>>, 0x00900440/0x140658250<TSExplicitList<USEABLESTYLE, -572662307>>, 0x00909B60<TSExplicitList<ACCOUNTIDCACHE, -572662307>>, 0x0090E400/0x1406696B0<SpellBookItem*>, 0x0090E4A0/0x1406697A0<SpellBookSkillInfo*>, 0x00913120/0x14066F870<TSExplicitList<OVERRIDE_SPELL_NODE, -572662307>>, 0x00913290/0x14066FA40<TSExplicitList<SPELLSBYNAME, -572662307>>, 0x00913400<TSExplicitList<OVERRIDE_SPELL_BY_NAME_NODE, -572662307>>, 0x00922F30/0x1406869B0<TSExplicitList<KEYCOMMAND, -572662307>>, 0x009230A0/0x140686B80<TSExplicitList<KEYBINDING, -572662307>>, 0x00923210/0x140686D50<TSExplicitList<OVERRIDEKEYBINDING, -572662307>>, 0x00923380/0x140686F20<TSExplicitList<MODIFIEDCLICK, -572662307>>, 0x009234F0<TSExplicitList<CLICKFRAME, -572662307>>, 0x0092D470/0x140694430<RaidInstanceInfo>, 0x009324C0<GuildMemberInfo*>, 0x00932560/0x14069AA90<GuildProfessionMember>, 0x00938B00/0x1406A2700<TSExplicitList<CGGuildInfo::GuildRecipesKnown, -572662307>>, 0x00938C70/0x1406A28D0<TSExplicitList<CGGuildInfo::SKILLLINESPELLS, -572662307>>, 0x00938DE0/0x1406A2AA0<TSExplicitList<CGGuildInfo::SKILLLINESPELLBITS, -572662307>>, 0x0094D1A0<TSExplicitList<ACHIEVEMENT, -572662307>>, 0x0094D310/0x1406BC660<TSExplicitList<GUILDACHIEVEMENTMEMBERLIST, -572662307>>, 0x00954830<UIButtons>, 0x0095A0D0<MissingLootItem>, 0x00963D90<TSExplicitList<CGQuestLog::QUESTCOMPLETIONNPC, -572662307>>, 0x0096DD30/0x1406E4D20<StationeryInfo*>, 0x0096DDD0/0x1406E4E10<MailboxMessageInfo*>, 0x0097F930/0x1406F9BA0<CGLookingForGroup::LFGEntryInfo>, 0x00987000<TSExplicitList<LFGListParty, -572662307>>, 0x00987170<TSExplicitList<LFGListPlayer, -572662307>>, 0x0098D7C0/0x14070BD10<TradeSkillSubClassInfo*>, 0x0099F850/0x140723870<CGCurrencyTypes::CurrencyCategoryInfo>, 0x0099F900/0x140723970<CGCurrencyTypes::CurrencyListInfo>, 0x009A3D50/0x140729420<CFlyout::Slot>, 0x009A3E00/0x140729510<CFlyout*>, 0x009A45E0<TSExplicitList<CFlyout, -572662307>>, 0x009A4750<TSExplicitList<SpellToFlyoutMapItem, -572662307>>, 0x009A8380/0x14072EF60<TSExplicitList<CGReputationInfo::FACTION, -572662307>>, 0x009B2D50/0x14073B9C0<TSExplicitList<EVENTDATEHASH, -572662307>>, 0x009B2EC0<TSExplicitList<DYNAMICHOLIDAYHASH, -572662307>>, 0x009B83D0<TSExplicitList<KNOWNDANCES, -572662307>>, 0x009B91F0/0x140744080<TrainerServiceInfo*>, 0x009C0890/0x14074D160<GuildBankItem>, 0x009CB300/0x14075AD50<CGAutoCompleteName::AUTOCOMPLETE_NAMENODE*>, 0x009CBD50/0x14075BA40<TSExplicitList<CGAutoCompleteName::AUTOCOMPLETE_CHARACTER_HASHNODE, -572662307>>, 0x009CBEC0/0x14075BC10<TSExplicitList<CGAutoCompleteName::AUTOCOMPLETE_BATTLENET_HASHNODE, -572662307>>, 0x009CDB00/0x14075E380<TSExplicitList<WORLDSTATE, -572662307>>, 0x009DE8D0/0x140774AC0<TSExplicitList<CGQuestObjectiveCache::REQUIREDUNITNODE, -572662307>>, 0x009DEA40<TSExplicitList<CGQuestObjectiveCache::REQUIREDITEMNODE, -572662307>>, 0x009E2500/0x140779790<TAXILINE>, 0x009E4100<TSExplicitList<TAXIPATHNODESCLIENT, -572662307>>, 0x009E7570<CGNamePlateFrame*>, 0x009F8730<C3iVector>, 0x009FCE60<INCOMINGRESURRECTION>, 0x009FD070/0x14079AE10<TSExplicitList<CGIncomingResurrection::INCOMINGRESLIST, -572662307>>, 0x00A0D330/0x1407AEDB0<MacroSubstring>, 0x00A0E880<TSExplicitList<MacroConditions, -572662307>>, 0x00B32510/0x1408FC690<TSExplicitList<BITMAPBYICON, -572662307>>, 0x0044E250<const VehicleUIIndSeatRec*>, 0x00494F30/0x1400BA7D0<const SpellEffectRec*>, 0x0058C010/0x1401F57F0<const CAnimReplacementSetDef*>, 0x0058C150<const AnimKitSegmentRec*>, 0x0058C1F0<const AnimKitConfigBoneSetRec*>, 0x00624B90/0x1402B63C0<const JamClientCharacterListEntry*>, 0x009025A0<uint32_t>, 0x0093DD90/0x1406A97B0<WarGameType*>, 0x006C24E0/0x140377F10<JamVoidItem>, 0x006C2B10<JamAccountInfoData>, 0x006D70A0<JamServerInfoField>, 0x006D6900/0x140393FB0<JamCliGuildRankData>, 0x006D69B0/0x1403940A0<JamClientCommentatorPartyMember>, 0x006D6BA0/0x1403943E0<LFGuildApplicationData>, 0x009A92F0<ArenaTeamMemberInfo*>, 0x00993540/0x140712BF0<SpellVariables>, 0x00947940/0x1406B4F40<GuildAchievementMember>, 0x0097F7C0<LFG_ROLE_CHECK_MEMBER_INFO>, 0x0097F880/0x1406F9AA0<LFG_SLOT_INFO_LOOT>, 0x009024F0/0x14065AB30<BattlenetUI::RIDFriendData>, 0x009E2450/0x140779670<TaxiWaypoint>, 0x006C1EC0/0x1403776E0<JamGuildRewardItem>, 0x006D6A60/0x140394200<LFGuildBrowseData>, 0x006D6B00/0x1403942F0<LFGuildRecruitData>, 0x006D6C50/0x1403944D0<JamMoveStateChange>, 0x00A31490/0x1407DB390<TSExplicitList<TIMESTAMPSTRUCT, -572662307>>, 0x0091C780/0x14067DF60<WorldMapPhasedChunkInfo>, 0x009382F0/0x1406A1B00<GuildNewsEvent>, 0x00698140/0x1403438A0<JamCliSupportTicketChatLine>, 0x006C2290/0x140377C00<ClientGuildAchievementMember>, 0x006D6860/0x140393EC0<ClientGuildRosterMemberData>, 0x006D6DF0/0x140394790<JamGuildBankTabInfo>, 0x00627C50/0x1402B9D40<BattlenetLogin::PatchInstruction>, 0x00675560/0x14031D6F0<JamTransmogrifyItem>, 0x006C1D50/0x1403774F0<JamResearchHistory>, 0x006C2340/0x140377CF0<JamWeeklySpellUse>, 0x006C28C0/0x140378440<ClientLFGBlackListSlot>, 0x00842940/0x14056ED70<void (__cdecl*)(unsigned int)>, 0x008E7880/0x140615C20<bool>, 0x0097B620/0x1406F4C20<INV_ALERT_STATUS>, 0x009A5C60<ForcedReaction>, 0x0069FE80/0x14034D0C0<JamCliStructAddonPrefix>, 0x006C1C80/0x1403773C0<JamGuildKnownRecipes>, 0x006C20F0/0x1403779E0<HotfixNotify>, 0x006C21B0/0x140377AE0<JamGuildCriteriaProgress>, 0x006C2710/0x140378210<JamClientGuildEventEntry>, 0x006C2980/0x140378550<ClientLFGProposalUpdatePlayer>, 0x006C2A50/0x140378670<JamLootItem>, 0x006E7360/0x1403BB190<JamGuildBankItemInfo>, 0x006C19E0/0x1403770A0<JamEarnedAchievement>, 0x004959B0/0x1400BB590<TSGrowableArray<SpellEffectRec const *>>, 0x004B2010/0x1400E8900<TSGrowableArray<int>>, 0x004B2260/0x1400E8BF0<TSGrowableArray<TSGrowableArray<int>>>, 0x004E5160/0x140126A90<CPhaseShift>, 0x0061AFD0/0x1402A8A30<GxuDebugShapeObject>, 0x006D67B0/0x140393DA0<JamClientCharacterListEntry>, 0x006E6FC0/0x1403A8F50<JamQuestCompletionNPC>, 0x006F4060/0x1403BB320<ClientLFGBlackList>, 0x00932610/0x14069AB80<NewsHeaderDate>, 0x0066E8C0/0x140315AB0<JamCliStructMailAttachment>, 0x006C1AC0/0x1403771E0<JamCriteriaProgress>, 0x006E7220/0x1403A92A0<ClientPVPLogData_Player>, 0x006D6F50<ClientLFGSearchResultParty>, 0x006D7000<ClientLFGSearchResultPlayer>, 0x009E4300/0x14077B990<TAXIROUTELINE>, 0x00498400/0x1400C24D0<DBQuery>, 0x00624C30/0x1402B64B0<JamUserClientReorderEntry>, 0x006C2030/0x1403778E0<BattlegroundPlayerPosition>, 0x006D6E90/0x140394900<JamGuildBankLogEntry>, 0x0098C600<REFORGE_OPTION>, 0x006C23F0/0x140377DE0<JamVendorItem>, 0x006C27F0/0x140378320<ClientServerPerfStat>, 0x006D6550/0x140393AA0<ClientSetupCurrencyRecord>, 0x006D6660/0x140393BF0<JamTradeItem>, 0x006D6D20/0x140394660<ClientDumpRideTicket>, 0x004C5560/0x140100610<REALM_INFO>, 0x0040A280/0x14000CA10<int>, 0x00460890/0x14006C0E0<CAaRect>, 0x00481B70/0x1400A07F0<CTexture *>, 0x00494E90/0x1400BA6E0<ItemSubClassRec const *>, 0x004E1F30/0x140122B50<SWorldTextIcon>, 0x004F86C0/0x14013BFF0<CSimpleBatchedMesh>, 0x0062CF90/0x1402C0920<PhasedZoneSoundOverride *>, 0x00706100/0x1403D3590<SE3DriverInfo>, 0x0071BF60/0x1403F0CE0<Liquid::CMaterialSettings *>, 0x008C62E0/0x1406362C0<TaintStackFrame>, 0x00950180<PetAction>, 0x0095D740/0x1406D05D0<BLOBPOI>, 0x00964990/0x1406D9F80<BLOB>, 0x0097F680/0x1406F97C0<LFGListPlayer *>, 0x0097F720/0x1406F98B0<LFGListParty *>, 0x0098D860/0x14070BE00<SkillLineAbilityRec const *>, 0x009A5BC0<FactionRec const *>, 0x009CD500/0x14075DC20<CGWorldStateInfo::WorldStateUIData>, 0x00A01750/0x1407A0240<CGBlobFrame::BlobRenderData>, 0x00B0A8B0<HttpDownload *>, 0x00B253C0/0x1408ED380<CGxVertexPCT>, 0x0092C310/0x140693100<CGCommentator::InstanceInfo>, 0x004BBCE0/0x1400F5180<char>, 0x004CA770/0x140105D20<float>, 0x00516E30/0x1401613E0<CSimpleEditBox::EditHistory>, 0x0051BD80/0x1401680C0<CSimpleMessageFrameLine>, 0x0051D100/0x1401699D0<CSimpleMessageScrollFrameDisplayNode>, 0x0040D140/0x1400106F0<CLoadingScreenShipInfo<TaxiPathNodeRec>::PATHINFO>, 0x00434650/0x14003D7F0<TSExplicitList<CShaderEffect, -572662307>>, 0x0045CCF0/0x14006F040<TSExplicitList<ADDONSTATE, -572662307>>, 0x009C60F0/0x140754250<TSExplicitList<MACRONODE, -572662307>>, 0x00B0F660<ShipPath::Segment>, 0x00B0F710<ShipPath::TimeEvent>, 0x00B32680/0x1408FC860<TSExplicitList<LCDHANDLE, -572662307>>, 0x0043BB30/0x1400A1830, 0x004EBC70/0x14012EEB0<CXMLAttribute>, 0x00624470/0x1402B5B00<TEXTURECACHEROW>, 0x008ECB00/0x14061BFF0<PENDINGUSERLIST>, 0x008EF890/0x14061FA10<ChatChannel>, 0x009A5B20/0x14072BF40, 0x009AB950/0x140732C00<CalendarDayEvent*>, 0x009E2390/0x140779560<TSGrowableArray<TAXIROUTELINE>>, 0x00B10E50/0x1408D6570<ShipPathLeg>, 0x00B31530/0x1408FB090<CHATMESSAGE>, 0x006324B0/0x1402C7BA0<IMPACTSOUNDARRAY>, 0x00B14370/0x1408DA190<CParticle2>]
template <class T>
void TSFixedArray<T>::ReallocData(uint32_t count) {
    T* olddata = this->m_data;

    for (uint32_t i = count; i < this->m_count; i++) {
        olddata[i].~T();
    }

    this->m_alloc = count;
    this->m_data = static_cast<T*>(SMemReAlloc(olddata, sizeof(T) * count, this->MemFileName(), this->MemLineNo(), SMEM_FLAG_NOREALLOC));

    if (!this->m_data) {
        this->m_data = static_cast<T*>(SMemAlloc(sizeof(T) * count, this->MemFileName(), this->MemLineNo(), 0));

        if (olddata) {
            uint32_t copycount = count < this->m_count ? count : this->m_count;

            for (uint32_t i = 0; i < copycount; i++) {
                new (&this->m_data[i]) T(olddata[i]);
                olddata[i].~T();
            }

            SMemFree(olddata, this->MemFileName(), this->MemLineNo(), 0);
        }
    }
}

// OFFSET: [0x00486A60, 0x006528B0/0x1402F2A80<const ZoneLightPointRec*>, 0x00799CA0/0x14048DE90<CSortEntryChunk>, 0x00799D20/0x14048DF60<CMapAreaLow*>, 0x00799DA0/0x14048E000<CMapObjDef*>, 0x00587C20/0x1401F0370<unsigned __int64>, 0x0040B970/0x14000E6C0<int>, 0x00487C80/0x1400A9910<WowConnection*>, 0x0095E4A0<BLOBPOI>, 0x00A0DCA0/0x1407AFB80<MacroSubstring>, 0x00495470/0x1400BAF30<const SpellEffectRec*>, 0x0091C830/0x14067E050<WorldMapPhasedChunkInfo>, 0x00586590/0x1401EE8F0<unsigned int>, 0x004B1F90/0x1400E8860<TSGrowableArray<int>>, 0x00B10080/0x1408D54D0<ShipPath::Segment>, 0x004BBD70/0x1400F5260, 0x0092BFF0/0x140692D80<CGCommentator::InstanceInfo>]
template <class T>
void TSFixedArray<T>::Set(uint32_t count, const T* data) {
    this->ReallocAndClearData(count);

    for (uint32_t i = 0; i < count; i++) {
        new (&this->m_data[i]) T(data[i]);
    }

    this->m_count = count;
}

// OFFSET: [0x00625230/0x1402B6BE0<CHARACTER_INFO>, 0x00B2BE40/0x1408F43E0<LightningJoint>, 0x00765C90/0x14044B750<CWaterRipple>, 0x008C0B10<TSExplicitList<RACECLASSINFO, -572662307>>, 0x004E5340<CPhaseShift>, 0x0060D8E0/0x140297480<DEATTHUDSOUNDINFO>, 0x009204A0/0x140682A10<WorldMapContinentInfo>, 0x004C56B0/0x140100890<REALM_INFO>, 0x0051A520/0x140166250<CSimpleEditBox::EditHistory>, 0x00616D20/0x1402A3C30<C3Vector>, 0x006326A0/0x1402C7E60<IMPACTSOUNDARRAY>, 0x0064E4C0<C2Vector>, 0x006245E0/0x1402B5CE0<TEXTURECACHEROW>, 0x0064E3F0/0x1402E9C30<CImVector>, 0x00B31A60/0x1408FB720, 0x0099EC50/0x1407227D0]
template <class T>
void TSFixedArray<T>::SetCount(uint32_t count) {
    if (count != this->m_count) {
        if (count) {
            this->ReallocData(count);

            for (uint32_t i = this->m_count; i < count; i++) {
                new (&this->m_data[i]) T;
            }

            this->m_count = count;
        } else {
            this->Clear();
        }
    }
}

// INLINED
template <class T>
TSFixedArray<T>& TSFixedArray<T>::operator=(const TSFixedArray<T>& source) {
    if (this != &source) {
        this->Set(source.m_count, source.m_data);
    }

    return *this;
}

// INLINED
template <class T>
TSGrowableArray<T>::TSGrowableArray() {
    this->m_chunk = 0;
}

// OFFSET: [0x0061E1D0<GXUFONTHYPERLINKINFO>, 0x0061E410<GXUEMBEDDEDTEXTUREINFO>, 0x005B6370<AuraData>, 0x007634B0/0x1404487E0<SMapObjDefVectorIntersect>, 0x00777260<CGxVertexPCT0T1>, 0x00777390<CBarrier::SContact>, 0x00756E90<CMapAreaLow*>, 0x00756F10<CMapObjDef*>, 0x0075F000/0x1404430F0<CMapObjDefGroup*>, 0x0079B9C0<C4Plane>, 0x0079BC90<CWorldOcclusionVolume>, 0x004F0270/0x140134990<void*>, 0x0059B420<MapObjSpeedTransition>, 0x009F4AC0<const ResearchBranchRec*>, 0x009F0000/0x140788450<GlyphItem*>, 0x00911EC0/0x14066DED0<SpellBookItem*>, 0x00941E40/0x1406AE470<WarGameType*>, 0x009840A0<uint32_t>, 0x005FA000/0x14027EEA0<unsigned __int64>, 0x00756D70/0x140438600<CWorldView::CVisRenderChunk>, 0x009E8DA0/0x140780000<CGNamePlateFrame *>, 0x009377C0<NewsHeaderDate>, 0x0058CAE0<AnimKitBoneSetInst>, 0x0058CB80/0x1401F65B0<const CAnimReplacementSetDef*>, 0x0058CC00/0x1401F88A0<CAnimKitInst*>, 0x0058CCD0<const AnimKitSegmentRec*>, 0x0058CD50<const AnimKitConfigBoneSetRec*>, 0x0058CDD0<AnimKitSegmentBoneSetInst*>, 0x0058CE50<AnimKitSegmentInst*>, 0x0058CED0<AnimKitDeferredCommand>, 0x00954D60<UIButtons>, 0x0098C750<REFORGE_OPTION>, 0x00591530<AnimKitDesiredBoneSetInst>, 0x004E46A0<unsigned short>, 0x0046A510/0x14007EA60<FrameStackInfo>, 0x004BC050/0x1400F5710<char>, 0x0060B890<SimpleScriptFunction>, 0x00725120<CFacet>, 0x00756CE0/0x140438510<CAaRect>, 0x00758700/0x14043A9D0<CPortalView>, 0x009D2BC0<C2Vector>, 0x005B6400<SortedAuraInfo>, 0x005BCE70<int>, 0x009D3E40<C3Vector>, 0x00B0B1A0/0x1408CE450]
template <class T>
uint32_t TSGrowableArray<T>::Add(uint32_t count, const T* data) {
    uint32_t newcount = this->m_count + count;

    if (newcount > this->m_alloc) {
        uint32_t chunk = this->m_chunk ? this->m_chunk : this->CalcChunkSize(newcount);

        if (newcount % chunk) {
            newcount += chunk - newcount % chunk;
        }

        this->ReallocData(newcount);
    }

    T* element = &this->m_data[this->m_count];
    T* end = element + count;

    for (; element != end; element++) {
        new (element) T(*data++);
    }

    this->m_count += count;

    return this->m_count - count;
}

// OFFSET: [0x009F4B40<const ResearchBranchRec*>, 0x0061E0B0/0x1402AC8A0<CImVector>, 0x0061E6D0/0x1402AD130<GRADIENTINFO>, 0x0061E130<VERT>]
template <class T>
uint32_t TSGrowableArray<T>::Add(uint32_t count, uint32_t incr, const T* data) {
    uint32_t newcount = this->m_count + count;

    if (newcount > this->m_alloc) {
        uint32_t chunk = this->m_chunk ? this->m_chunk : this->CalcChunkSize(newcount);

        if (newcount % chunk) {
            newcount += chunk - newcount % chunk;
        }

        this->ReallocData(newcount);
    }

    T* element = &this->m_data[this->m_count];
    T* end = element + count;

    for (; element != end; element++) {
        new (element) T(*data);
        data = reinterpret_cast<const T*>(reinterpret_cast<const uint8_t*>(data) + incr);
    }

    this->m_count += count;

    return this->m_count - count;
}

// OFFSET: [0x009FFD20/0x140332F00<JamCliCUFProfile>, 0x0069CFC0<JamCliStructAddonPrefix>, 0x004BBCA0, 0x004E41C0<unsigned short>, 0x004E8840<FFX::Pass*>, 0x0061D9A0/0x1401C1FA0, 0x006730D0<C4Plane>, 0x006A2FE0<DayNight::AreaLightOverride>, 0x00753910<CPortalView>, 0x0076AAC0<unsigned __int64>, 0x009C8350<C3Vector>]
template <class T>
uint32_t TSGrowableArray<T>::CalcChunkSize(uint32_t count) {
    uint32_t maxchunk = 256 / sizeof(T) > 8 ? 256 / sizeof(T) : 8;

    if (count >= maxchunk) {
        this->m_chunk = maxchunk;
        return maxchunk;
    }

    uint32_t result = count;

    for (uint32_t bits = count & (count - 1); bits; bits &= bits - 1) {
        result = bits;
    }

    return result ? result : 1;
}

// OFFSET: [0x0071C5B0/0x1403F15A0<Liquid::IMaterial*>, 0x007952D0/0x140487E90<C4Vector>, 0x007078F0/0x1403D44E0<C3Vector>, 0x00707040/0x1403D4780<SE3SoundKitDef*>, 0x0071C180/0x1403F0FE0<Liquid::CMaterialSettings *>, 0x008A41F0/0x1405E6B00<EvtTimer*>]
template <class T>
void TSGrowableArray<T>::GrowToFit(uint32_t index, int32_t zero) {
    if (index >= this->m_count) {
        uint32_t newcount = index + 1;

        if (newcount > this->m_alloc) {
            uint32_t chunk = this->m_chunk ? this->m_chunk : this->CalcChunkSize(newcount);
            uint32_t alloc = newcount;

            if (newcount % chunk) {
                alloc = newcount + chunk - newcount % chunk;
            }

            this->ReallocData(alloc);
        }

        if (zero) {
            memset(&this->m_data[this->m_count], 0, sizeof(T) * (index - this->m_count + 1));
        }

        this->m_count = newcount;
    }
}

// OFFSET: [0x00631A40/0x1402C6EA0<TEXTEMOTELOOKUP>, 0x00487150/0x1400A8470<CObjectHeapList>, 0x0080A4E0<CharacterSelectionDisplay>, 0x009E3140/0x14077A340<TAXIROUTELINE>, 0x009E3370/0x14077A5B0<TAXILINE>, 0x0084FEC0<CGxFormat>, 0x004D18E0/0x14010E870<Grunt::CdKey::Info>, 0x00647730/0x1402E4330<DayNight::AreaLightOverride>, 0x00756E20<CSortEntryChunk>, 0x0079BA50<C4Plane>, 0x004C1B20/0x1400FC550<_SOURCE>, 0x00512A70/0x14015CBF0<CAsyncObject*>, 0x0084AB90/0x1405790D0<CGxMonitorMode>, 0x0053F1C0/0x140199D80<CStreamingObj>, 0x00B13BC0/0x1408D9900<CParticleEmitter2*>, 0x0087A5C0/0x1405B0C90<W32Joystick>, 0x0087BA10<OsIMECandidate>, 0x0089CD40<FFX::PortalEffect>, 0x0059EFC0<MapObjSpeedTransition>, 0x009F6A90/0x140792F00<CGResearchFrame::BLOBPOI>, 0x004392C0<CCameraTransition>, 0x00462AA0/0x140075D60<CCameraTrack::DeferredCameraAction>, 0x00628A10/0x1402BAB00<BattlenetLogin::GameAccount>, 0x00908800<BattlenetUI::RIDFriendData>, 0x00955270<UIButtons>, 0x009D2C40/0x140764280<MinimapBlobRing>, 0x008ECA00/0x14061BF50<JamCliSupportTicketChatLine>, 0x005BCC80/0x1402318D0<WOWGUID>, 0x00628AA0/0x1402BABD0<BattlenetLogin::PatchInstruction>, 0x008EC140/0x14061B280<JamCliStructAddonPrefix>, 0x00486D70/0x1400A7DE0<CObjectHeap>, 0x007251B0/0x1403FC320<CFacet>, 0x0081C200<M2ModelCallback>, 0x00406520/0x140007BD0<char *>, 0x004E91A0<AreaPOIRec *>, 0x0051C040/0x140168430<MessageData>, 0x00528220/0x1401777D0<TSList<SIMPLEANIMNODE, TSGetLink<SIMPLEANIMNODE>> *>, 0x005673E0/0x1401C1F10<int>, 0x00460B80/0x14006C460<CAaRect>, 0x00A01DA0<CGBlobFrame::BlobRenderData>, 0x0082CAF0<M2Element>, 0x008441F0/0x1405708D0<CGxPushedRenderState>, 0x008442A0/0x140570B30, 0x0089DBE0, 0x008A0F70<SThread*>, 0x008D3680/0x140647360<NearestUnitData>, 0x0099B6F0<TalentGroupTier_C*>, 0x009D2E80<OBJINFO>, 0x004E2810/0x1401236D0<SWorldTextIcon>]
template <class T>
T* TSGrowableArray<T>::New() {
    uint32_t newcount = this->m_count + 1;

    if (newcount > this->m_alloc) {
        uint32_t chunk = this->m_chunk ? this->m_chunk : this->CalcChunkSize(newcount);

        if (newcount % chunk) {
            newcount += chunk - newcount % chunk;
        }

        this->ReallocData(newcount);
    }

    T* element = &this->m_data[this->m_count++];

    return new (element) T;
}

// OFFSET: [0x009088A0/0x14065AC20<unsigned int>, 0x00B2BB80/0x1408F4040<CLightning*>, 0x00717C30/0x1403EB320<Liquid::DisplMapPerturbation*>, 0x00717C90/0x1403EB460<Liquid::DisplMapPerturbationMesh*>, 0x00B3C4F0/0x140909460<Liquid::CInstance*>, 0x0072D7B0/0x140406C70<C3Vector>, 0x0072D820/0x140406D30<unsigned short>, 0x00772670<CChunkLiquid*>, 0x00756960/0x140438060<CMapObjDefGroup*>, 0x00612C00/0x14029DF80<GxuSceneObject*>, 0x00612C60/0x14029E0C0<GxDrawListEntry>, 0x0084AA50<void*>, 0x008BDE50/0x140607100<const SpellVisualKitModelAttachRec*>, 0x009F5150/0x140790CF0<C2iVector>, 0x009F8F10<C3iVector>, 0x00563FE0<RuneConvert>, 0x0058D870<AnimKitSegmentBoneSetInst *>, 0x0058D8D0/0x1401F87A0<AnimKitSegmentInst *>, 0x0058DA90/0x1401F7BF0<CAnimKitInst *>, 0x005EF2A0/0x140271100<CVehiclePassenger_C *>, 0x00844240/0x1405709F0<void (__cdecl*)(unsigned int)>, 0x00939980<GuildNewsEvent>, 0x009FCF10<INCOMINGRESURRECTION>, 0x004594B0/0x14005C540<const VehicleUIIndSeatRec*>, 0x0053F160/0x140199CD0<void (*)()>, 0x009A3FD0/0x140729790<CFlyout::Slot>, 0x0058D930<AnimKitDeferredCommand>, 0x00622380/0x1402B31F0<GxuBufferCache::BufferEntry>, 0x004BBFF0<uint8_t>, 0x006539A0<const char*>, 0x007586A0/0x14043A920<uint32_t>, 0x00637B20<C2iVector>, 0x00482030/0x1400A0D90<CTexture *>, 0x00528180<CSimpleControlPoint *>, 0x00636620/0x1402CBFD0<void (__cdecl*)(OPTION_PAGE)>, 0x00725280/0x1403FC3C0<CFacet>, 0x008EF450<PENDINGUSERLIST>, 0x0040BD70/0x14000EC70, 0x00595850, 0x0089D960/0x1405DE0D0<FFX::Pass*>, 0x009A7160, 0x00A0C7A0/0x1407ADE50<EventInviteInfo*>]
template <class T>
T* TSGrowableArray<T>::New(const T& source) {
    uint32_t newcount = this->m_count + 1;

    if (newcount > this->m_alloc) {
        uint32_t chunk = this->m_chunk ? this->m_chunk : this->CalcChunkSize(newcount);

        if (newcount % chunk) {
            newcount += chunk - newcount % chunk;
        }

        this->ReallocData(newcount);
    }

    T* element = &this->m_data[this->m_count++];

    return new (element) T(source);
}

// INLINED
template <class T>
void TSGrowableArray<T>::Reserve(uint32_t count, int32_t round) {
    uint32_t newcount = this->m_count + count;

    if (newcount <= this->m_alloc) {
        return;
    }

    if (round) {
        uint32_t chunk = this->m_chunk ? this->m_chunk : this->CalcChunkSize(newcount);

        if (newcount % chunk) {
            newcount += chunk - newcount % chunk;
        }
    }

    this->ReallocData(newcount);
}

// OFFSET: [0x006DB760/0x14039A3C0<unsigned int>, 0x0091DD00/0x14067F960<WorldMapLandmarkInfo>, 0x00563F90<unsigned short>, 0x009E4AE0/0x14077C460<TAXIROUTELINE>, 0x00844E70/0x140571C70<CGxFormat>, 0x0084AC00/0x140579170<CGxMonitorMode>, 0x0061E260/0x1402ACB70<GXUFONTHYPERLINKINFO>, 0x0051C0A0/0x1401684D0<MessageData>, 0x0071D130<TSExplicitList<Liquid::Object, -572662307>>, 0x0064F0B0/0x1402EE1A0<TSExplicitList<DayNight::DNOverrideSky, -572662307>>, 0x00652B00/0x1402F2CD0<DayNight::CZoneLight>, 0x0072D0D0<CWorldMap::CMapTableEntry>, 0x0072E770/0x1404085B0<TSExplicitList<CMapDoodadDef, -572662307>>, 0x0072E880/0x140408750<TSExplicitList<CMapObjDef, -572662307>>, 0x0075D690/0x140440F70<CGxVertexPT2>, 0x0075EB90/0x140442A50<CMapFootprintTexture>, 0x00758790/0x14043AAD0<CPortalView>, 0x00797ED0<CMapLowDetail::CMapAreaLowTableEntry>, 0x0079A440/0x14048E930<CSortEntry>, 0x007772F0<CGxVertexPCT0T1>, 0x00777420<CBarrier::SContact>, 0x00784C30/0x140473920<CMap::CMapAreaTableEntry>, 0x00786800/0x140475A70<CMapAreaTexture>, 0x0077F6E0/0x14046C690<CGxVertexPC>, 0x0077BE90/0x140468430<TSExplicitList<CSceneMaterial, -572662307>>, 0x00747E00/0x140426B30<TSExplicitList<CMapObj, -572662307>>, 0x007514B0<Mists::Mist>, 0x00737D80/0x140414790<CDetailDoodadInstAdd>, 0x0073A680<TSExplicitList<DDMaterial, -572662307>>, 0x007569C0/0x140438110<CMapObjDefGroup*>, 0x00756A10/0x1404381A0<CSortEntryChunk>, 0x0079BAE0/0x140490490<C4Plane>, 0x0079BD10/0x1404906D0<CWorldOcclusionVolume>, 0x00446EC0<TSExplicitList<_BANNEDADDON, -572662307>>, 0x004A5BB0/0x1400D5CC0<TSExplicitList<DBCache<GuildStats_C, int, HASHKEY_INT>::DBCACHEHASH, -572662307>>, 0x004C2BA0<TSExplicitList<RGN, -572662307>>, 0x0054BC20<TSExplicitList<CTextureBlobTexture, -572662307>>, 0x006564E0<TSExplicitList<CVar, -572662307>>, 0x00B32A00/0x1408FCD40<TSExplicitList<BITMAPBYICON, -572662307>>, 0x004FC920/0x140140CA0<TSExplicitList<FILENAMEHASH, -572662307>>, 0x00526240/0x1401751D0<TSExplicitList<SIMPLEFONT, -572662307>>, 0x00848DE0/0x1405769A0<CGxShaderPermute>, 0x00713B80/0x1403E5DC0<TSExplicitList<SE3ChatSound, -572662307>>, 0x00714D00/0x1403E7640<TSExplicitList<SE3MemorySoundObject, -572662307>>, 0x00832200/0x140553C40<unsigned char *>, 0x0087E200/0x1405B6E40<TSExplicitList<OsNet::NETSELSOCKPTR, -572662307>>, 0x0045D8C0/0x14006FEC0<TSExplicitList<UIADDON::METADATA, -572662307>>, 0x00505B60/0x14014C7C0<TSExplicitList<FRAMEATTR, -572662307>>, 0x00543A00<TSExplicitList<CTexture, -572662307>>, 0x00545500/0x14019C240<TSExplicitList<ProfileInternal::KEYVALUE, -572662307>>, 0x00545610/0x14019C3E0<TSExplicitList<ProfileInternal::SECTION, -572662307>>, 0x005E27C0<TSExplicitList<OBJ_EFFECT_GROUP_LOOKUP, -572662307>>, 0x005E28D0<TSExplicitList<OBJ_EFFECT_REF_LIST_LOOKUP, -572662307>>, 0x0061C690/0x1402AA460<TSExplicitList<KERNNODE, -572662307>>, 0x00621C80/0x1402B2750<TSExplicitList<BATCHEDRENDERFONTDESC, -572662307>>, 0x0099D880/0x140720980<TSExplicitList<TalentGroupItem_C, -572662307>>, 0x0099D990/0x140720B20<TSExplicitList<TalentGroupTierItem_C, -572662307>>, 0x00A31CB0/0x1407DBBD0<TSExplicitList<TIMESTAMPSTRUCT, -572662307>>, 0x004A5770/0x1400D5640<TSExplicitList<DBCache<CreatureStats_C, int, HASHKEY_INT>::DBCACHEHASH, -572662307>>, 0x004A5990/0x1400D5980<TSExplicitList<DBCache<NPCText, int, HASHKEY_INT>::DBCACHEHASH, -572662307>>, 0x004A5AA0/0x1400D5B20<TSExplicitList<DBCache<NameCache, unsigned __int64, CHashKeyGUID>::DBCACHEHASH, -572662307>>, 0x004A5CC0/0x1400D5E60<TSExplicitList<DBCache<QuestCache, int, HASHKEY_INT>::DBCACHEHASH, -572662307>>, 0x004A5DD0/0x1400D6000<TSExplicitList<DBCache<PageTextCache_C, int, HASHKEY_INT>::DBCACHEHASH, -572662307>>, 0x004A5EE0/0x1400D61A0<TSExplicitList<DBCache<PetNameCache, int, HASHKEY_INT>::DBCACHEHASH, -572662307>>, 0x004A5FF0/0x1400D6340<TSExplicitList<DBCache<CGPetition, int, HASHKEY_INT>::DBCACHEHASH, -572662307>>, 0x004A6100/0x1400D64E0<TSExplicitList<DBCache<ItemTextCache_C, unsigned __int64, CHashKeyGUID>::DBCACHEHASH, -572662307>>, 0x004A6320/0x1400D6820<TSExplicitList<DBCache<ArenaTeamCache, int, HASHKEY_INT>::DBCACHEHASH, -572662307>>, 0x004A6430/0x1400D69C0<TSExplicitList<DBCache<DanceCache, int, HASHKEY_INT>::DBCACHEHASH, -572662307>>, 0x00612CD0/0x14029E190<C3Vector>, 0x0044B0E0/0x1400583C0<TSExplicitList<MOUSELOOKBINDING, -572662307>>, 0x00622890<TSExplicitList<FACEDATA, -572662307>>, 0x0070AD90<TSExplicitList<SE3SoundChunk, -572662307>>, 0x0070C700/0x1403DAE10<TSExplicitList<SE3SOUNDKITOBJECT_LOOKUP, -572662307>>, 0x0070AFB0<TSExplicitList<SOUNDKITLOOKUP, -572662307>>, 0x0070AEA0<TSExplicitList<SE3SoundEmitterLookup, -572662307>>, 0x0070C5F0/0x1403DAC70<TSExplicitList<SE3COMBINABLESOUNDLIST, -572662307>>, 0x00512AD0<CAsyncObject*>, 0x0086DF90/0x1405A2A30<TSExplicitList<CGxDeviceD3d11::InputLayoutRec, -572662307>>, 0x0086E0A0/0x1405A2BD0<TSExplicitList<CGxDeviceD3d11::RasterizerStateRec, -572662307>>, 0x0086E1B0/0x1405A2D70<TSExplicitList<CGxDeviceD3d11::BlendStateRec, -572662307>>, 0x0086E2C0/0x1405A2F10<TSExplicitList<CGxDeviceD3d11::DepthStateRec, -572662307>>, 0x0086E3D0/0x1405A30B0<TSExplicitList<CGxDeviceD3d11::SamplerStateRec, -572662307>>, 0x00612680/0x14029D830<GxuDebugTextObject>, 0x005477B0<TSExplicitList<FileNotification::DeferredFile, -572662307>>, 0x005488C0/0x1401A0CE0<TSExplicitList<CModelBlob::CHashEntry, -572662307>>, 0x00549C20<TSExplicitList<FONTHASHOBJ, -572662307>>, 0x0053F230/0x140199E20<CStreamingObj>, 0x00B19770/0x1408DFC30<CParticle2>, 0x004ED130<TSExplicitList<HashedNode, -572662307>>, 0x004ED240<TSExplicitList<FrameFactoryNode, -572662307>>, 0x006115E0<TSExplicitList<CLightList, -572662307>>, 0x0061C580<TSExplicitList<CHARCODEDESC, -572662307>>, 0x0089CE10/0x1405DD100<FFX::PortalEffect>, 0x00654680/0x1402F4FE0<TSExplicitList<CONSOLECOMMAND, -572662307>>, 0x0083EB50<TSExplicitList<FrameScript_EventObject, -572662307>>, 0x008BDE00<LightningObject::Bolt>, 0x0058D9B0/0x1401F7A70<AnimKitBoneSetInst>, 0x00591DA0<AnimKitDesiredBoneSetInst>, 0x006E6A80<ClientTradeItem>, 0x006F4460/0x1403BB900<JamGuildNewsEvent>, 0x006F4650/0x1403BEBC0<JamGuildBankItemInfo>, 0x009F2A10<TSExplicitList<CGSpellActivationOverlay::COUNTERNODE, -572662307>>, 0x009F2B20<TSExplicitList<CGSpellActivationOverlay::PETAURANODE, -572662307>>, 0x009F6280<TSExplicitList<CGResearchFrame::BRANCH, -572662307>>, 0x009F6390/0x140792640<TSExplicitList<CGResearchFrame::BLOBPOIHASH, -572662307>>, 0x009C9D70/0x140758E20<TSExplicitList<CGCUFProfiles::PROFILENODE, -572662307>>, 0x009C9E80/0x140758FC0<TSExplicitList<CGCUFProfiles::OPTIONENTRY, -572662307>>, 0x00687F70/0x140332D30<JamCliCUFProfile>, 0x00631AB0/0x1402C6CA0<TEXTEMOTELOOKUP*>, 0x00637B90<C2iVector>, 0x009F8F80<C3iVector>, 0x006DB550/0x14039A020<JamGuildRewardItem>, 0x006E6C60<LFGuildBrowseData>, 0x006E6CB0<LFGuildRecruitData>, 0x006E6D00<LFGuildApplicationData>, 0x00930210<RaidInstanceInfo>, 0x00937770<GuildMemberInfo*>, 0x00970450<MailboxMessageInfo*>, 0x004E4B80/0x140126280<uint16_t>, 0x00587CA0/0x1401F0410<SpellCastTargetPoint>, 0x005BCCD0/0x140231960<DelayedSpellVisualKit>, 0x006DB300<JamEarnedAchievement>, 0x006DB380/0x140399CF0<JamCriteriaProgress>, 0x006E6D50<JamMoveStateChange>, 0x00704500<TSExplicitList<CACHEENTRY, -572662307>>, 0x004BBFA0/0x1400F5540<uint32_t>, 0x008EC1F0<bool>, 0x008D95C0<TSExplicitList<TAINTEVENT, -572662307>>, 0x008F18B0<TSExplicitList<WORDLIST, -572662307>>, 0x008F1AD0/0x140622770<TSExplicitList<ADDONPREFIX, -572662307>>, 0x00916A40/0x140676340<TSExplicitList<OVERRIDE_SPELL_NODE, -572662307>>, 0x00916B50/0x1406764E0<TSExplicitList<SPELLSBYNAME, -572662307>>, 0x00923C80/0x140687CB0<TSExplicitList<KEYBINDING, -572662307>>, 0x00923D90/0x140687E50<TSExplicitList<OVERRIDEKEYBINDING, -572662307>>, 0x00923EA0/0x140687FF0<TSExplicitList<MODIFIEDCLICK, -572662307>>, 0x0093A010/0x1406A45B0<TSExplicitList<CGGuildInfo::GuildRecipesKnown, -572662307>>, 0x0093A230/0x1406A48F0<TSExplicitList<CGGuildInfo::SKILLLINESPELLS, -572662307>>, 0x0094E1D0<TSExplicitList<ACHIEVEMENT, -572662307>>, 0x0094E2E0/0x1406BDD40<TSExplicitList<GUILDACHIEVEMENTMEMBERLIST, -572662307>>, 0x00964CF0<TSExplicitList<CGQuestLog::QUESTCOMPLETIONNPC, -572662307>>, 0x009A49E0<TSExplicitList<CFlyout, -572662307>>, 0x009A4AF0<TSExplicitList<SpellToFlyoutMapItem, -572662307>>, 0x009A85A0/0x14072F280<TSExplicitList<FACTION, -572662307>>, 0x009B4C20<TSExplicitList<DYNAMICHOLIDAYHASH, -572662307>>, 0x009B8630<TSExplicitList<KNOWNDANCES, -572662307>>, 0x009CDDB0/0x14075E7B0<TSExplicitList<WORLDSTATE, -572662307>>, 0x009FD3C0/0x14079B2F0<TSExplicitList<CGIncomingResurrection::INCOMINGRESLIST, -572662307>>, 0x00439CD0<TSExplicitList<CCameraModeEntry, -572662307>>, 0x00483F50<TSExplicitList<UNITPORTRAIT, -572662307>>, 0x00484060<TSExplicitList<ITEMPORTRAIT, -572662307>>, 0x0048C430/0x1400AFDC0<TSExplicitList<WowClientDB2_Base::DBTABLE, -572662307>>, 0x00495F00<TSExplicitList<SPELLACTIVATIONOVERLAY, -572662307>>, 0x004A3B00/0x1400D2310<TSExplicitList<DBCache<NameCache, unsigned __int64, CHashKeyGUID>::REVERSEENTRY, -572662307>>, 0x004DE250<TSExplicitList<TALKINGPLAYER, -572662307>>, 0x004DE360<TSExplicitList<MUTEDPLAYER, -572662307>>, 0x004E57F0<TSExplicitList<DECLINEDWORD, -572662307>>, 0x005A6790<TSExplicitList<INVENTORYART, -572662307>>, 0x005A68A0<TSExplicitList<ITEMBYNAME, -572662307>>, 0x0058CC80<unsigned __int64>, 0x0045DAE0<TSExplicitList<UIADDON, -572662307>>, 0x005E29E0<TSExplicitList<OBJ_EFFECT_DEF_LOOKUP, -572662307>>, 0x005E2AF0<TSExplicitList<OBJ_EFFECT_GROUP_DEF_LOOKUP, -572662307>>, 0x005E2C00<TSExplicitList<OBJ_EFFECT_PACKAGE_DEF_LOOKUP, -572662307>>, 0x00495E30/0x1400BBBC0<TSGrowableArray<SpellEffectRec const *>>, 0x004B2190/0x1400E8A90<TSGrowableArray<int>>, 0x004B2360/0x1400E8D50<TSGrowableArray<TSGrowableArray<int>>>, 0x0058DA40/0x1401F7B60<CAnimReplacementSetDef const *>, 0x0058DAF0<AnimKitSegmentBoneSetInst *>, 0x0058DB40<AnimKitDeferredCommand>, 0x0061B640/0x1402A9290<GxuDebugShapeObject>, 0x00670210/0x140317330<JamCliStructMailAttachment>, 0x006DB410<JamClientRestrictedFactionChangeRule>, 0x006DB460/0x140399E70<JamGuildKnownRecipes>, 0x006DB4B0/0x140399F00<JamResearchHistory>, 0x006DB500/0x140399F90<JamCategoryCooldown>, 0x006DB5A0<ClientSummonRaidMemberValidateReason>, 0x006DB670/0x14039A210<HotfixNotify>, 0x006DB6C0<JamGuildCriteriaProgress>, 0x006DB710/0x14039A330<ClientGuildAchievementMember>, 0x006DB830/0x14039A520<JamVoidItem>, 0x006DB880/0x14039A640<JamGuildRankTabPermissions>, 0x006DB8D0/0x14039A6D0<JamClientGuildEventEntry>, 0x006DB920<ClientServerPerfStat>, 0x006DB970/0x14039A880<ClientLFGProposalUpdatePlayer>, 0x006DB9C0<JamLootItem>, 0x006DBA10<JamAccountInfoData>, 0x006E6B70<ClientGuildRosterMemberData>, 0x006E6BC0<JamCliGuildRankData>, 0x006E6C10<JamClientCommentatorPartyMember>, 0x006E6DF0<ClientDumpRideTicket>, 0x006E6E40<JamGuildBankTabInfo>, 0x006E6E90/0x1403A8CC0<JamGuildBankLogEntry>, 0x006E6F20<JamLfgSearchResultParty>, 0x006E6F70/0x1403A8EC0<JamCliServerInfoLine>, 0x006F4380<JamQuestCompletionNPC>, 0x006F4550/0x1403BBA40<ClientPVPLogData_Player>, 0x006F7400/0x1403BED20<ClientLFGBlackList>, 0x009619D0/0x1406D56D0<BLOBPOI>, 0x009D2CA0/0x140764330<MinimapBlobRing>, 0x009E3320<TaxiWaypoint>, 0x00A0ECF0/0x1407B13E0<MacroCondition>, 0x004901E0/0x1400B4AE0<TSExplicitList<CGObject_C, -572662307>>, 0x00590400<TSExplicitList<CustomFallbackPath, -572662307>>, 0x0062EE90<TSExplicitList<ZONESOUNDOVERRIDELOOKUP, -572662307>>, 0x00633030<TSExplicitList<DSP_CHAIN_HEAD_LOOKUP, -572662307>>, 0x00633CF0<TSExplicitList<FOOTSTEPSNDCACHE, -572662307>>, 0x008AED30<TSExplicitList<ITEMCOOLDOWNHASHNODE, -572662307>>, 0x008AEE40<TSExplicitList<DESTLOCSPELLCASTINDEXHASHNODE, -572662307>>, 0x00923FB0<TSExplicitList<CLICKFRAME, -572662307>>, 0x008D94B0/0x14064CC60<TSExplicitList<SAVEDVARIABLE, -572662307>>, 0x008F19C0/0x1406225D0<TSExplicitList<CHATTHROTTLE, -572662307>>, 0x00900C20/0x140658C10<TSExplicitList<USEABLESTYLE, -572662307>>, 0x00909E60<TSExplicitList<ACCOUNTIDCACHE, -572662307>>, 0x0093A120/0x1406A4750<TSExplicitList<CGGuildInfo::SKILLLINESPELLBITS, -572662307>>, 0x009884C0<TSExplicitList<LFGListParty, -572662307>>, 0x009885D0<TSExplicitList<LFGListPlayer, -572662307>>, 0x009B4B10/0x14073DEA0<TSExplicitList<CALENDAREVENTHASH, -572662307>>, 0x009CC380/0x14075C250<TSExplicitList<CGAutoCompleteName::AUTOCOMPLETE_CHARACTER_HASHNODE, -572662307>>, 0x009CC490/0x14075C3F0<TSExplicitList<CGAutoCompleteName::AUTOCOMPLETE_BATTLENET_HASHNODE, -572662307>>, 0x009DECD0/0x140774FE0<TSExplicitList<CGQuestObjectiveCache::REQUIREDUNITNODE, -572662307>>, 0x009DEDE0<TSExplicitList<CGQuestObjectiveCache::REQUIREDITEMNODE, -572662307>>, 0x009E46E0<TSExplicitList<TAXIPATHNODESCLIENT, -572662307>>, 0x00A0EBE0<TSExplicitList<MacroConditions, -572662307>>, 0x0056D9B0<TSExplicitList<TRADESKILLLINE, -572662307>>, 0x0056DAC0<TSExplicitList<PROFICIENCYSPELL, -572662307>>, 0x005E25A0<TSExplicitList<OBJ_EFFECT_GROUP_DEF_REF_LIST_LOOKUP, -572662307>>, 0x00793500<TSExplicitList<`CMapObjGroup::GetHitTestDebugTextures'::`2'::TEXTURENAME, -572662307>>, 0x0098C980<REFORGE_OPTION>, 0x0076ACB0/0x140452AA0<World::HitTestDebugItem::TextureInfo>, 0x00A0E660/0x1407B0AA0<MacroSubstring>, 0x004A5880/0x1400D57E0<TSExplicitList<DBCache<GameObjectStats_C, int, HASHKEY_INT>::DBCACHEHASH, -572662307>>, 0x00483E40<TSExplicitList<PLAYERPORTRAIT, -572662307>>, 0x004A6210/0x1400D6680<TSExplicitList<DBCache<WardenCachedModule, CWardenKey, CWardenKey>::DBCACHEHASH, -572662307>>, 0x00595D10/0x140203330<TSExplicitList<VoidItem_C, -572662307>>, 0x005D5790/0x14024FFC0<TSExplicitList<UnitThreat, -572662307>>, 0x005E26B0/0x140260B10<TSExplicitList<OBJ_EFFECT_REF_LOOKUP, -572662307>>, 0x006DB5F0/0x14039A140<BattlegroundPlayerPosition>, 0x006E6A00<ClientSetupCurrencyRecord>, 0x00849400/0x140577250<TSExplicitList<CGxShader, -572662307>>, 0x008F1BE0<TSExplicitList<PROFANITYCHECK, -572662307>>, 0x008F1CF0<TSExplicitList<SPAMCHECK, -572662307>>, 0x00916C60<TSExplicitList<OVERRIDE_SPELL_BY_NAME_NODE, -572662307>>, 0x00923B70/0x140687B10<TSExplicitList<KEYCOMMAND, -572662307>>, 0x00564040/0x1401C2030<ITEMSWAP>, 0x0062EFA0/0x1402C3490<AmbFlavorListHead>, 0x006DB7B0<JamVendorItem>, 0x006E6AF0<JamClientCharacterListEntry>, 0x00908900/0x140661950<uint32_t>, 0x00756B90/0x140438370<SPortalExt>, 0x009C4310/0x1407518F0<MacroCommand>, 0x0040B7B0<DYNAMICELEMENTVERT>, 0x004FAB70/0x14013E8E0<CSimpleBatchedMesh>, 0x00616BF0/0x1402A3AD0<C2Vector>, 0x00620AB0<VERT>, 0x0092C450<CGCommentator::InstanceInfo>, 0x0092C710<CGCommentator::MapInfo>, 0x00966C60/0x1406DCC30<BLOB>, 0x009D2EF0<OBJINFO>, 0x0040DB30/0x1400111D0<CLoadingScreenShipInfo<TaxiPathNodeRec>::PATHINFO>, 0x004348B0/0x14003DB90<TSExplicitList<CShaderEffect, -572662307>>, 0x004419B0/0x14004CE20<unsigned char>, 0x0045BBD0/0x14006D950<char const *>, 0x0045D9D0/0x140070060<TSExplicitList<ADDONSTATE, -572662307>>, 0x00482090/0x1400A0E40<CTexture *>, 0x004CB380/0x140106F70<float>, 0x004E9790<POIDIRECTIONDATA>, 0x004FFF80/0x1401452A0<FRAMEPRIORITY *>, 0x0051C370/0x1401688B0<CSimpleMessageFrameLine>, 0x0051D8E0/0x14016A380<CSimpleMessageScrollFrameLine>, 0x0051D9D0/0x14016A4F0<CSimpleMessageScrollFrameDisplayNode>, 0x004EBE60/0x14012F160<CXMLAttribute>, 0x005BCE20<AuraData>, 0x0061E750/0x1402AD200<GRADIENTINFO>, 0x00706FF0/0x1403D46F0<SE3DriverInfo>, 0x00460BF0/0x14006C500<CAaRect>, 0x008EF4D0/0x14061F500<PENDINGUSERLIST>, 0x00952F00<PetAction>, 0x009C6840/0x140754CC0<TSExplicitList<MACRONODE, -572662307>>, 0x00B32B10/0x1408FCEE0<TSExplicitList<LCDHANDLE, -572662307>>, 0x00441A00/0x14004CEC0, 0x00482470/0x1400A1220<CImVector>, 0x004B1E40/0x1400E8660<int>, 0x004CB260/0x140106DF0, 0x0081C190<CRibbonMat>, 0x0081C250<M2ModelCallback>, 0x008442F0/0x140570BC0, 0x008D36D0<NearestUnitData>, 0x008EC1A0/0x14061B320<char*>, 0x008F2510/0x140623550<ChatChannel>, 0x009753B0/0x1406ED5F0<OutbidItemInfo*>, 0x00984120/0x1406FF090, 0x0098F140/0x14070DBC0, 0x009D2DC0<POIINFO>, 0x009E0C60/0x1407779D0<PetitionSignerInfo>, 0x00B26A50/0x1408EEC30<CGxVertexPCT>, 0x00441A50<CFacet>, 0x004E28B0/0x1401237A0, 0x0080A540/0x140520820<CharacterSelectionDisplay>, 0x00B11010/0x1408D67C0<ShipPathLeg>, 0x00B1B610/0x1408E25F0<CParticle2_Model>, 0x009E31B0/0x14077A3F0<TSGrowableArray<TAXIROUTELINE>>, 0x0061E4A0/0x1402ACEB0<GXUEMBEDDEDTEXTUREINFO>]
template <class T>
void TSGrowableArray<T>::SetCount(uint32_t count) {
    if (count > this->m_count) {
        if (count > this->m_alloc) {
            uint32_t chunk = this->m_chunk ? this->m_chunk : this->CalcChunkSize(count);
            uint32_t alloc = count;

            if (count % chunk) {
                alloc = count + chunk - count % chunk;
            }

            this->ReallocData(alloc);
        }

        for (uint32_t i = this->m_count; i < count; i++) {
            new (&this->m_data[i]) T;
        }
    } else if (count < this->m_count) {
        for (uint32_t i = count; i < this->m_count; i++) {
            this->m_data[i].~T();
        }
    }

    this->m_count = count;
}

// OFFSET: [0x004DE6F0/0x1400C9600<CMapObj, HASHKEY_NONE>, 0x004FC7E0/0x1402B34A0<FONTHASHOBJ, HASHKEY_STR>]
template <class T, class TKey>
TSHashObject<T, TKey>::~TSHashObject() {
}

// OFFSET: [0x00505B30/0x14014C780<FRAMEATTR, HASHKEY_STRI>, 0x00526200/0x140175180<SIMPLEFONT, HASHKEY_STRI>, 0x00543070/0x140196770<CTexture, HASHKEY_TEXTUREFILE>, 0x005486E0/0x1401A0A00<CModelBlob::CHashEntry, HASHKEY_MODELBLOBFILE>, 0x00545900/0x14019C880<ProfileInternal::KEYVALUE, HASHKEY_CONSTSTRI>, 0x005462B0/0x14019D760<ProfileInternal::SECTION, HASHKEY_CONSTSTRI>, 0x00549DA0/0x1401A2A80<FONTHASHOBJ, HASHKEY_STR>, 0x0054BB70/0x1401A51E0<CTextureBlobTexture, HASHKEY_TEXTUREBLOBFILE>, 0x004D9950/0x140117E20<TIMESTAMPSTRUCT, HASHKEY_NONE>, 0x00704680/0x1403D11E0<CACHEENTRY, HASHKEY_NONE>, 0x008F2710/0x1406237E0<WORDLIST, HASHKEY_LANGUAGE>, 0x00989250/0x140705B90<LFGListParty, CHashKeyGUID>, 0x00924B70/0x1406892F0<CLICKFRAME, HASHKEY_STR>, 0x009C6680/0x1407549E0<MACRONODE, HASHKEY_NONE>, 0x009B5040/0x14073E650<DYNAMICHOLIDAYHASH, HASHKEY_NONE>, 0x0099DB10/0x140720D50<TalentGroupItem_C, HASHKEY_NONE>, 0x0099DD20/0x140721080<TalentGroupTierItem_C, HASHKEY_NONE>, 0x009A8790/0x14072F530<FACTION, HASHKEY_NONE>, 0x00447040/0x140053370<_BANNEDADDON, HASHKEY_NONE>, 0x0045E0D0/0x140070A70<ADDONSTATE, HASHKEY_STRI>, 0x0044B300/0x1400585F0<MOUSELOOKBINDING, HASHKEY_STRI>, 0x004843A0/0x1400A40D0<PLAYERPORTRAIT, CHashKeyGUID>, 0x00848640/0x140575F80<CGxShader, HASHKEY_STRI>, 0x00611530/0x14029BDA0<CLightList, HASHKEY_DWORD>, 0x0061C400/0x1402AA0E0<CHARCODEDESC, HASHKEY_NONE>, 0x0061C810/0x1402AA690<KERNNODE, KERNINGHASHKEY>, 0x00622A10/0x1402B3AD0<FACEDATA, HASHKEY_STRI>, 0x0056D600/0x1401CDA70<TRADESKILLLINE, HASHKEY_NONE>, 0x005E55B0/0x140264EE0<OBJ_EFFECT_GROUP_LOOKUP, HASHKEY_NONE>, 0x005A7F90/0x140219B00<INVENTORYART, HASHKEY_NONE>, 0x005D5760/0x14024FF80<UnitThreat, CHashKeyGUID>, 0x00654800/0x1402F5180<CONSOLECOMMAND, HASHKEY_CONSTSTRI>, 0x0056D720/0x1401CDC60<PROFICIENCYSPELL, HASHKEY_NONE>, 0x008C0CF0/0x14060ADF0<RACECLASSINFO, HASHKEY_NONE>, 0x004ED3C0/0x140130F20<HashedNode, HASHKEY_STRI>, 0x004ED670/0x1401312F0<FrameFactoryNode, HASHKEY_STRI>, 0x0083F010/0x14056A8B0<FrameScript_EventObject, HASHKEY_CONSTSTRI>, 0x00B31900/0x1408FB550<LCDHANDLE, HASHKEY_CONSTSTRI>, 0x00434A30/0x14003DD30<CShaderEffect, HASHKEY_STRI>, 0x0071D2B0/0x1403F2720<Liquid::Object, HASHKEY_NONE>, 0x0064AA30/0x1402E8210<DayNight::DNOverrideSky, HASHKEY_STRI>, 0x0072E5E0/0x1404082A0<CMapDoodadDef, HASHKEY_DWORD>, 0x0072E6E0/0x140408470<CMapObjDef, HASHKEY_NONE>, 0x00736110/0x140412750<CMapDynamicTexture, HASHKEY_DWORD>, 0x0077BE40/0x1404683E0<CSceneMaterial, HASHKEY_MATERIAL>, 0x00747D70/0x1404269F0<CMapObj, HASHKEY_NONE>, 0x0073AC20/0x140418250<DDMaterial, HASHKEY_NONE>, 0x005479A0/0x14019F6F0<FileNotification::DeferredFile, HASHKEY_NONE>, 0x006156F0/0x1402A1400<BATCHEDRENDERFONTDESC, HASHKEY_PTR>, 0x0070AA20/0x1403DA920<SE3SoundChunk, HASHKEY_NONE>, 0x0070BF50/0x1403DB590<SE3SOUNDKITOBJECT_LOOKUP, HASHKEY_NONE>, 0x0070C070/0x1403DB6E0<SOUNDKITLOOKUP, HASHKEY_CONSTSTRI>, 0x0070C210<SE3SoundEmitterLookup, HASHKEY_NONE>, 0x0070D7C0/0x1403DB860<SE3COMBINABLESOUNDLIST, HASHKEY_NONE>, 0x00713A80/0x1403E5D30<SE3ChatSound, HASHKEY_NONE>, 0x00714C60/0x1403E74E0<SE3MemorySoundObject, HASHKEY_NONE>, 0x004FC8F0/0x140140C60<FILENAMEHASH, HASHKEY_STR>, 0x00855600/0x140585C50<CGxDeviceD3d11::InputLayoutRec, InputLayoutKey>, 0x00855800/0x140585F60<CGxDeviceD3d11::RasterizerStateRec, HASHKEY_NONE>, 0x00855A00/0x140586270<CGxDeviceD3d11::BlendStateRec, HASHKEY_NONE>, 0x00855C00/0x140586580<CGxDeviceD3d11::DepthStateRec, HASHKEY_NONE>, 0x00855E00/0x140586890<CGxDeviceD3d11::SamplerStateRec, HASHKEY_NONE>, 0x00547CE0/0x14019FBC0<FileNotification::Manifest, HASHKEY_NONE>, 0x009F2D00/0x14078D760<CGSpellActivationOverlay::COUNTERNODE, HASHKEY_NONE>, 0x009F2F00/0x14078D9E0<CGSpellActivationOverlay::PETAURANODE, HASHKEY_NONE>, 0x009F6B70/0x140792FB0<CGResearchFrame::BRANCH, HASHKEY_NONE>, 0x009F7830/0x1407942D0<CGResearchFrame::BLOBPOIHASH, HASHKEY_NONE>, 0x009CA070/0x140759270<CGCUFProfiles::PROFILENODE, HASHKEY_CONSTUTF8I>, 0x009CA210/0x140759500<CGCUFProfiles::OPTIONENTRY, HASHKEY_CONSTSTR>, 0x0045E370/0x140070D40<UIADDON::METADATA, HASHKEY_STRI>, 0x0048C5B0/0x1400AFF60<WowClientDB2_Base::DBTABLE, HASHKEY_NONE>, 0x0048F600/0x1400B3D50<CGObject_C, CHashKeyGUID>, 0x004DE550/0x14011D170<TALKINGPLAYER, CHashKeyGUID>, 0x004DE7E0/0x14011D400<MUTEDPLAYER, CHashKeyGUID>, 0x004E59E0/0x1401275A0<DECLINEDWORD, HASHKEY_CONSTSTRI>, 0x00590BA0/0x1401FC0C0<CustomFallbackPath, HASHKEY_NONE>, 0x00595EF0/0x140203660<VoidItem_C, HASHKEY_NONE>, 0x005A8230/0x140219EC0<ITEMBYNAME, HASHKEY_UTF8I>, 0x005E2EE0/0x1402617B0<OBJ_EFFECT_REF_LOOKUP, HASHKEY_NONE>, 0x005E3200/0x140261C90<OBJ_EFFECT_REF_LIST_LOOKUP, HASHKEY_NONE>, 0x005E3C60/0x1402627F0<OBJ_EFFECT_GROUP_DEF_REF_LIST_LOOKUP, HASHKEY_NONE>, 0x0062CBB0/0x1402C0540<ZONESOUNDOVERRIDELOOKUP, HASHKEY_STRI>, 0x00633200/0x1402C8B40<DSP_CHAIN_HEAD_LOOKUP, HASHKEY_STRI>, 0x00633F80<FOOTSTEPSNDCACHE, HASHKEY_NONE>, 0x008B2890<ITEMCOOLDOWNHASHNODE, HASHKEY_NONE>, 0x008B2AA0/0x1405F8FE0<DESTLOCSPELLCASTINDEXHASHNODE, CHashKeyGUID>, 0x008DC640<SAVEDVARIABLE, HASHKEY_STR>, 0x008F2130/0x140623020<CHATTHROTTLE, HASHKEY_STR>, 0x008F28D0<PROFANITYCHECK, HASHKEY_STR>, 0x008F2B00<SPAMCHECK, HASHKEY_STR>, 0x00900EF0/0x140658FF0<USEABLESTYLE, HASHKEY_NONE>, 0x00916890/0x1406760A0<OVERRIDE_SPELL_NODE, HASHKEY_NONE>, 0x009169A0/0x1406761F0<OVERRIDE_SPELL_BY_NAME_NODE, HASHKEY_UTF8I>, 0x009171C0/0x140676EB0<SPELLSBYNAME, HASHKEY_UTF8I>, 0x0093ACC0/0x1406A5780<CGGuildInfo::GuildRecipesKnown, HASHKEY_NONE>, 0x0093AFC0<CGGuildInfo::SKILLLINESPELLS, HASHKEY_NONE>, 0x0094E4E0/0x1406BE070<ACHIEVEMENT, HASHKEY_NONE>, 0x009894B0/0x140705E90<LFGListPlayer, CHashKeyGUID>, 0x009A4C70/0x14072AA40<CFlyout, HASHKEY_NONE>, 0x009A4EF0/0x14072AD40<SpellToFlyoutMapItem, HASHKEY_NONE>, 0x009B8820/0x1407431A0<KNOWNDANCES, HASHKEY_STRI>, 0x009CDF90/0x14075EA50<WORLDSTATE, HASHKEY_NONE>, 0x009E49B0/0x14077C2E0<TAXIPATHNODESCLIENT, HASHKEY_NONE>, 0x00A0EF40/0x1407B1710<MacroConditions, HASHKEY_STR>, 0x00B32D00/0x1408FD190<BITMAPBYICON, HASHKEY_NONE>, 0x004A4070/0x1400D2C90<DBCache<CreatureStats_C, int, HASHKEY_INT>::REVERSEENTRY, HASHKEY_CONSTSTR>, 0x004A4250/0x1400D3000<DBCache<GameObjectStats_C, int, HASHKEY_INT>::REVERSEENTRY, HASHKEY_CONSTSTR>, 0x004A4430/0x1400D3380<DBCache<NPCText, int, HASHKEY_INT>::REVERSEENTRY, HASHKEY_CONSTSTR>, 0x004A4610/0x1400D3700<DBCache<NameCache, uint64_t, CHashKeyGUID>::REVERSEENTRY, HASHKEY_CONSTSTR>, 0x004A47F0/0x1400D3A70<DBCache<GuildStats_C, uint64_t, CHashKeyGUID>::REVERSEENTRY, HASHKEY_CONSTSTR>, 0x004A49D0/0x1400D3DE0<DBCache<QuestCache, int, HASHKEY_INT>::REVERSEENTRY, HASHKEY_CONSTSTR>, 0x004A4C00/0x1400D4130<DBCache<PageTextCache_C, int, HASHKEY_INT>::REVERSEENTRY, HASHKEY_CONSTSTR>, 0x004A4DE0/0x1400D44B0<DBCache<PetNameCache, int, HASHKEY_INT>::REVERSEENTRY, HASHKEY_CONSTSTR>, 0x004A4FC0/0x1400D4820<DBCache<CGPetition, int, HASHKEY_INT>::REVERSEENTRY, HASHKEY_CONSTSTR>, 0x004A51A0/0x1400D4B90<DBCache<ItemTextCache_C, uint64_t, CHashKeyGUID>::REVERSEENTRY, HASHKEY_CONSTSTR>, 0x004A5380/0x1400D4F00<DBCache<WardenCachedModule, CWardenKey, CWardenKey>::REVERSEENTRY, HASHKEY_CONSTSTR>, 0x004A5560/0x1400D5270<DBCache<ArenaTeamCache, int, HASHKEY_INT>::REVERSEENTRY, HASHKEY_CONSTSTR>, 0x004ABAE0/0x1400DF060<DBCache<CreatureStats_C, int, HASHKEY_INT>::DBCACHEHASH, HASHKEY_INT>, 0x004ABB10/0x1400DF0A0<DBCache<GameObjectStats_C, int, HASHKEY_INT>::DBCACHEHASH, HASHKEY_INT>, 0x004ABB40/0x1400DF0E0<DBCache<NPCText, int, HASHKEY_INT>::DBCACHEHASH, HASHKEY_INT>, 0x004ABB70/0x1400DF120<DBCache<NameCache, uint64_t, CHashKeyGUID>::DBCACHEHASH, CHashKeyGUID>, 0x004ABBA0/0x1400DF160<DBCache<GuildStats_C, uint64_t, CHashKeyGUID>::DBCACHEHASH, CHashKeyGUID>, 0x004ABBD0/0x1400DF1A0<DBCache<QuestCache, int, HASHKEY_INT>::DBCACHEHASH, HASHKEY_INT>, 0x004ABC00/0x1400DF1E0<DBCache<PageTextCache_C, int, HASHKEY_INT>::DBCACHEHASH, HASHKEY_INT>, 0x004ABC30/0x1400DF220<DBCache<PetNameCache, int, HASHKEY_INT>::DBCACHEHASH, HASHKEY_INT>, 0x004ABC60/0x1400DF260<DBCache<CGPetition, int, HASHKEY_INT>::DBCACHEHASH, HASHKEY_INT>, 0x004ABC90/0x1400DF2A0<DBCache<ItemTextCache_C, uint64_t, CHashKeyGUID>::DBCACHEHASH, CHashKeyGUID>, 0x004ABCC0/0x1400DF2E0<DBCache<WardenCachedModule, CWardenKey, CWardenKey>::DBCACHEHASH, CWardenKey>, 0x004ABCF0/0x1400DF320<DBCache<ArenaTeamCache, int, HASHKEY_INT>::DBCACHEHASH, HASHKEY_INT>, 0x004ABD20/0x1400DF360<DBCache<DanceCache, int, HASHKEY_INT>::DBCACHEHASH, HASHKEY_INT>, 0x00909FE0/0x140663690<ACCOUNTIDCACHE, HASHKEY_DWORD>, 0x009CC7E0/0x14075C870<CGAutoCompleteName::AUTOCOMPLETE_CHARACTER_HASHNODE, CHashKeyGUID>, 0x009CC9E0/0x14075CAF0<CGAutoCompleteName::AUTOCOMPLETE_BATTLENET_HASHNODE, HASHKEY_NONE>, 0x009DEFC0/0x140775420<CGQuestObjectiveCache::REQUIREDUNITNODE, HASHKEY_NONE>, 0x009DF1C0/0x1407756A0<CGQuestObjectiveCache::REQUIREDITEMNODE, HASHKEY_NONE>, 0x00439F20/0x140044A20<CCameraModeEntry, HASHKEY_NONE>, 0x0094E6F0/0x1406BE300<GUILDACHIEVEMENTMEMBERLIST, HASHKEY_NONE>, 0x00966E10/0x1406DCEA0<CGQuestLog::QUESTCOMPLETIONNPC, HASHKEY_NONE>, 0x009FD5C0/0x14079B5B0<CGIncomingResurrection::INCOMINGRESLIST, CHashKeyGUID>, 0x004845F0/0x1400A4350<UNITPORTRAIT, HASHKEY_NONE>, 0x00484840/0x1400A4630<ITEMPORTRAIT, HASHKEY_STR>, 0x00496240<SPELLACTIVATIONOVERLAY, HASHKEY_NONE>, 0x008DC7E0/0x140650D60<TAINTEVENT, HASHKEY_STR>, 0x008F23E0<ADDONPREFIX, HASHKEY_STR>, 0x004A5740/0x1400D5600<DBCache<DanceCache, int, HASHKEY_INT>::REVERSEENTRY, HASHKEY_CONSTSTR>, 0x0093BCA0/0x1406A6FB0<CGGuildInfo::SKILLLINESPELLBITS, HASHKEY_NONE>, 0x007938B0/0x140485940<CFileName, ?>, 0x009B4DC0/0x14073E2B0<EVENTDATEHASH, ?>, 0x005E5070/0x1402649F0<OBJ_EFFECT_GROUP_DEF_LOOKUP, HASHKEY_NONE>, 0x009248E0/0x140689050<MODIFIEDCLICK, HASHKEY_STRI>, 0x005E34C0/0x140261FA0<OBJ_EFFECT_DEF_LOOKUP, HASHKEY_NONE>, 0x005E5660/0x140265040<OBJ_EFFECT_PACKAGE_DEF_LOOKUP, HASHKEY_NONE>, 0x00656660/0x1402F7C90<CVar, HASHKEY_STRI>, 0x009241A0/0x140688480<KEYCOMMAND, HASHKEY_STRI>, 0x00924350/0x140688720<KEYBINDING, HASHKEY_STRI>]
template <class T, class TKey>
void TSHashTable<T, TKey>::InternalDelete(T* ptr) {
    ptr->~T();
    SMemFree(ptr, typeid(T).raw_name(), SLOG_OBJECT, 0);
}

// OFFSET: [0x00505AC0/0x14014C670<FRAMEATTR, HASHKEY_STRI>, 0x00526740<SIMPLEFONT, HASHKEY_STRI>, 0x00548720/0x1401A0A40<CModelBlob::CHashEntry, HASHKEY_MODELBLOBFILE>, 0x00545890/0x14019C760<ProfileInternal::KEYVALUE, HASHKEY_CONSTSTRI>, 0x005462F0/0x14019D7B0<ProfileInternal::SECTION, HASHKEY_CONSTSTRI>, 0x00549E20/0x1401A2B30<FONTHASHOBJ, HASHKEY_STR>, 0x004D97E0/0x140117BB0<TIMESTAMPSTRUCT, HASHKEY_NONE>, 0x00633230<DSP_CHAIN_HEAD_LOOKUP, HASHKEY_STRI>, 0x00704740<CACHEENTRY, HASHKEY_NONE>, 0x00917140/0x140676D90<SPELLSBYNAME, HASHKEY_UTF8I>, 0x008F21E0<CHATTHROTTLE, HASHKEY_STR>, 0x009894E0<LFGListPlayer, CHashKeyGUID>, 0x00924B00/0x1406891E0<CLICKFRAME, HASHKEY_STR>, 0x009C66B0<MACRONODE, HASHKEY_NONE>, 0x009B4FD0/0x14073E540<DYNAMICHOLIDAYHASH, HASHKEY_NONE>, 0x0099DB40/0x140720D90<TalentGroupItem_C, HASHKEY_NONE>, 0x0099DD50/0x1407210C0<TalentGroupTierItem_C, HASHKEY_NONE>, 0x009A8720/0x14072F420<FACTION, HASHKEY_NONE>, 0x0045E060/0x140070960<ADDONSTATE, HASHKEY_STRI>, 0x0044B340/0x140058640<MOUSELOOKBINDING, HASHKEY_STRI>, 0x004847C0/0x1400A4510<ITEMPORTRAIT, HASHKEY_STR>, 0x00848670/0x140575FC0<CGxShader, HASHKEY_STRI>, 0x00611560/0x14029BDE0<CLightList, HASHKEY_DWORD>, 0x0061C430/0x1402AA120<CHARCODEDESC, HASHKEY_NONE>, 0x00622A40/0x1402B3B10<FACEDATA, HASHKEY_STRI>, 0x005E34F0/0x140261FE0<OBJ_EFFECT_DEF_LOOKUP, HASHKEY_NONE>, 0x005E55F0/0x140264F30<OBJ_EFFECT_GROUP_LOOKUP, HASHKEY_NONE>, 0x005E5690<OBJ_EFFECT_PACKAGE_DEF_LOOKUP, HASHKEY_NONE>, 0x005D56F0/0x14024FE70<UnitThreat, CHashKeyGUID>, 0x00654830/0x1402F51C0<CONSOLECOMMAND, HASHKEY_CONSTSTRI>, 0x004C2B50/0x1400FD8A0<RGN, HASHKEY_NONE>, 0x008C0C90/0x14060ACF0<RACECLASSINFO, HASHKEY_NONE>, 0x004ED420<HashedNode, HASHKEY_STRI>, 0x004ED600<FrameFactoryNode, HASHKEY_STRI>, 0x00434A60/0x14003DD70<CShaderEffect, HASHKEY_STRI>, 0x0071D2E0<Liquid::Object, HASHKEY_NONE>, 0x0064A9D0/0x1402E8110<DayNight::DNOverrideSky, HASHKEY_STRI>, 0x0072E610/0x1404082E0<CMapDoodadDef, HASHKEY_DWORD>, 0x0072E710/0x1404084B0<CMapObjDef, HASHKEY_NONE>, 0x007360A0/0x140412640<CMapDynamicTexture, HASHKEY_DWORD>, 0x0077BE70/0x140468420<CSceneMaterial, HASHKEY_MATERIAL>, 0x00747DA0/0x140426A30<CMapObj, HASHKEY_NONE>, 0x0073A790/0x140417B50<DDMaterial, HASHKEY_NONE>, 0x00547930/0x14019F5E0<FileNotification::DeferredFile, HASHKEY_NONE>, 0x00615720<BATCHEDRENDERFONTDESC, HASHKEY_PTR>, 0x0070AA50<SE3SoundChunk, HASHKEY_NONE>, 0x0070BEF0/0x1403DB490<SE3SOUNDKITOBJECT_LOOKUP, HASHKEY_NONE>, 0x0070C000/0x1403DB5D0<SOUNDKITLOOKUP, HASHKEY_CONSTSTRI>, 0x0070C240<SE3SoundEmitterLookup, HASHKEY_NONE>, 0x0070C4D0/0x1403DAB70<SE3COMBINABLESOUNDLIST, HASHKEY_NONE>, 0x00713AF0<SE3ChatSound, HASHKEY_NONE>, 0x00714C90/0x1403E7520<SE3MemorySoundObject, HASHKEY_NONE>, 0x004FC880/0x140140B50<FILENAMEHASH, HASHKEY_STR>, 0x008555A0/0x140585B50<CGxDeviceD3d11::InputLayoutRec, InputLayoutKey>, 0x008557A0/0x140585E60<CGxDeviceD3d11::RasterizerStateRec, HASHKEY_NONE>, 0x008559A0/0x140586170<CGxDeviceD3d11::BlendStateRec, HASHKEY_NONE>, 0x00855BA0/0x140586480<CGxDeviceD3d11::DepthStateRec, HASHKEY_NONE>, 0x00855DA0/0x140586790<CGxDeviceD3d11::SamplerStateRec, HASHKEY_NONE>, 0x00547C80<FileNotification::Manifest, HASHKEY_NONE>, 0x009F2CA0/0x14078D660<CGSpellActivationOverlay::COUNTERNODE, HASHKEY_NONE>, 0x009F2EA0/0x14078D8E0<CGSpellActivationOverlay::PETAURANODE, HASHKEY_NONE>, 0x009F6BA0/0x140792FF0<CGResearchFrame::BRANCH, HASHKEY_NONE>, 0x009F76A0/0x1407941C0<CGResearchFrame::BLOBPOIHASH, HASHKEY_NONE>, 0x009CA000/0x140759160<CGCUFProfiles::PROFILENODE, HASHKEY_CONSTUTF8I>, 0x009CA1A0/0x1407593F0<CGCUFProfiles::OPTIONENTRY, HASHKEY_CONSTSTR>, 0x00439F70/0x140044A80<CCameraModeEntry, HASHKEY_NONE>, 0x0045E3B0/0x140070D90<UIADDON::METADATA, HASHKEY_STRI>, 0x0048C5E0/0x1400AFFA0<WowClientDB2_Base::DBTABLE, HASHKEY_NONE>, 0x00490180/0x1400B49E0<CGObject_C, CHashKeyGUID>, 0x004E5970/0x140127490<DECLINEDWORD, HASHKEY_CONSTSTRI>, 0x0054BBB0/0x1401A5220<CTextureBlobTexture, HASHKEY_TEXTUREBLOBFILE>, 0x0056D590/0x1401CD960<TRADESKILLLINE, HASHKEY_NONE>, 0x0056D6C0/0x1401CDB60<PROFICIENCYSPELL, HASHKEY_NONE>, 0x00595E90/0x140203560<VoidItem_C, HASHKEY_NONE>, 0x005A7FE0/0x140219B60<INVENTORYART, HASHKEY_NONE>, 0x005A81C0/0x140219DB0<ITEMBYNAME, HASHKEY_UTF8I>, 0x005E2E80/0x1402616B0<OBJ_EFFECT_REF_LOOKUP, HASHKEY_NONE>, 0x005E3180/0x140261B70<OBJ_EFFECT_REF_LIST_LOOKUP, HASHKEY_NONE>, 0x005E3BE0/0x1402626D0<OBJ_EFFECT_GROUP_DEF_REF_LIST_LOOKUP, HASHKEY_NONE>, 0x0062CB40/0x1402C0430<ZONESOUNDOVERRIDELOOKUP, HASHKEY_STRI>, 0x00633FB0<FOOTSTEPSNDCACHE, HASHKEY_NONE>, 0x008B2830<ITEMCOOLDOWNHASHNODE, HASHKEY_NONE>, 0x008DC5D0<SAVEDVARIABLE, HASHKEY_STR>, 0x008F2920<PROFANITYCHECK, HASHKEY_STR>, 0x008F2B50<SPAMCHECK, HASHKEY_STR>, 0x00900E10<USEABLESTYLE, HASHKEY_NONE>, 0x009169D0/0x140676230<OVERRIDE_SPELL_BY_NAME_NODE, HASHKEY_UTF8I>, 0x0093AC50/0x1406A5670<CGGuildInfo::GuildRecipesKnown, HASHKEY_NONE>, 0x0093AF60<CGGuildInfo::SKILLLINESPELLS, HASHKEY_NONE>, 0x0094E460/0x1406BDF50<ACHIEVEMENT, HASHKEY_NONE>, 0x00966DA0/0x1406DCD90<CGQuestLog::QUESTCOMPLETIONNPC, HASHKEY_NONE>, 0x009A4F20/0x14072AD80<SpellToFlyoutMapItem, HASHKEY_NONE>, 0x009B87B0/0x140743090<KNOWNDANCES, HASHKEY_STRI>, 0x009CDF30/0x14075E950<WORLDSTATE, HASHKEY_NONE>, 0x009E4950/0x14077C1E0<TAXIPATHNODESCLIENT, HASHKEY_NONE>, 0x00A0EEC0<MacroConditions, HASHKEY_STR>, 0x004A3F20/0x1400D29F0<DBCache<CreatureStats_C, int, HASHKEY_INT>::DBCACHEHASH, HASHKEY_INT>, 0x004A4000/0x1400D2B80<DBCache<CreatureStats_C, int, HASHKEY_INT>::REVERSEENTRY, HASHKEY_CONSTSTR>, 0x004A4110/0x1400D2D60<DBCache<GameObjectStats_C, int, HASHKEY_INT>::DBCACHEHASH, HASHKEY_INT>, 0x004A41E0/0x1400D2EF0<DBCache<GameObjectStats_C, int, HASHKEY_INT>::REVERSEENTRY, HASHKEY_CONSTSTR>, 0x004A43C0/0x1400D3270<DBCache<NPCText, int, HASHKEY_INT>::REVERSEENTRY, HASHKEY_CONSTSTR>, 0x004A45A0/0x1400D35F0<DBCache<NameCache, uint64_t, CHashKeyGUID>::REVERSEENTRY, HASHKEY_CONSTSTR>, 0x004A46B0<DBCache<GuildStats_C, uint64_t, CHashKeyGUID>::DBCACHEHASH, CHashKeyGUID>, 0x004A4780/0x1400D3960<DBCache<GuildStats_C, uint64_t, CHashKeyGUID>::REVERSEENTRY, HASHKEY_CONSTSTR>, 0x004A4890/0x1400D3B40<DBCache<QuestCache, int, HASHKEY_INT>::DBCACHEHASH, HASHKEY_INT>, 0x004A4960/0x1400D3CD0<DBCache<QuestCache, int, HASHKEY_INT>::REVERSEENTRY, HASHKEY_CONSTSTR>, 0x004A4B90/0x1400D4020<DBCache<PageTextCache_C, int, HASHKEY_INT>::REVERSEENTRY, HASHKEY_CONSTSTR>, 0x004A4D70/0x1400D43A0<DBCache<PetNameCache, int, HASHKEY_INT>::REVERSEENTRY, HASHKEY_CONSTSTR>, 0x004A4F50/0x1400D4710<DBCache<CGPetition, int, HASHKEY_INT>::REVERSEENTRY, HASHKEY_CONSTSTR>, 0x004A5130/0x1400D4A80<DBCache<ItemTextCache_C, uint64_t, CHashKeyGUID>::REVERSEENTRY, HASHKEY_CONSTSTR>, 0x004A5310/0x1400D4DF0<DBCache<WardenCachedModule, CWardenKey, CWardenKey>::REVERSEENTRY, HASHKEY_CONSTSTR>, 0x004A54F0/0x1400D5160<DBCache<ArenaTeamCache, int, HASHKEY_INT>::REVERSEENTRY, HASHKEY_CONSTSTR>, 0x0094E680/0x1406BE1F0<GUILDACHIEVEMENTMEMBERLIST, HASHKEY_NONE>, 0x0090A010/0x1406636D0<ACCOUNTIDCACHE, HASHKEY_DWORD>, 0x009CC980/0x14075C9F0<CGAutoCompleteName::AUTOCOMPLETE_BATTLENET_HASHNODE, HASHKEY_NONE>, 0x009DEF60/0x140775320<CGQuestObjectiveCache::REQUIREDUNITNODE, HASHKEY_NONE>, 0x009DF160/0x1407755A0<CGQuestObjectiveCache::REQUIREDITEMNODE, HASHKEY_NONE>, 0x008F2690<WORDLIST, HASHKEY_LANGUAGE>, 0x009168C0/0x1406760E0<OVERRIDE_SPELL_NODE, HASHKEY_NONE>, 0x00590BD0/0x1401FC100<CustomFallbackPath, HASHKEY_NONE>, 0x004DE4E0<TALKINGPLAYER, CHashKeyGUID>, 0x004DE770/0x14011D2F0<MUTEDPLAYER, CHashKeyGUID>, 0x008B2A30/0x1405F8ED0<DESTLOCSPELLCASTINDEXHASHNODE, CHashKeyGUID>, 0x009CC770/0x14075C760<CGAutoCompleteName::AUTOCOMPLETE_CHARACTER_HASHNODE, CHashKeyGUID>, 0x009FD540/0x14079B490<CGIncomingResurrection::INCOMINGRESLIST, CHashKeyGUID>, 0x00B32C90/0x1408FD080<BITMAPBYICON, HASHKEY_NONE>, 0x00484570<UNITPORTRAIT, HASHKEY_NONE>, 0x004961E0<SPELLACTIVATIONOVERLAY, HASHKEY_NONE>, 0x008DC830/0x140650DC0<TAINTEVENT, HASHKEY_STR>, 0x008F2370<ADDONPREFIX, HASHKEY_STR>, 0x00917C90/0x140678070<OVERRIDE_SPELL_NODE, HASHKEY_NONE>, 0x009A4CA0/0x14072AA80<CFlyout, HASHKEY_NONE>, 0x004A42F0<DBCache<NPCText, int, HASHKEY_INT>::DBCACHEHASH, HASHKEY_INT>, 0x004A44D0<DBCache<NameCache, unsigned __int64, CHashKeyGUID>::DBCACHEHASH, CHashKeyGUID>, 0x004A4B00/0x1400D3F80<DBCache<PageTextCache_C, int, HASHKEY_INT>::DBCACHEHASH, HASHKEY_INT>, 0x004A4CA0<DBCache<PetNameCache, int, HASHKEY_INT>::DBCACHEHASH, HASHKEY_INT>, 0x004A4E80/0x1400D4580<DBCache<CGPetition, int, HASHKEY_INT>::DBCACHEHASH, HASHKEY_INT>, 0x004A5060<DBCache<ItemTextCache_C, unsigned __int64, CHashKeyGUID>::DBCACHEHASH, CHashKeyGUID>, 0x004A5240/0x1400D4C60<DBCache<WardenCachedModule, CWardenKey, CWardenKey>::DBCACHEHASH, CWardenKey>, 0x004A5420<DBCache<ArenaTeamCache, int, HASHKEY_INT>::DBCACHEHASH, HASHKEY_INT>, 0x004A5600<DBCache<DanceCache, int, HASHKEY_INT>::DBCACHEHASH, HASHKEY_INT>, 0x004A56D0/0x1400D54F0<DBCache<DanceCache, int, HASHKEY_INT>::REVERSEENTRY, HASHKEY_CONSTSTR>, 0x0093BCD0/0x1406A6FF0<CGGuildInfo::SKILLLINESPELLBITS, HASHKEY_NONE>, 0x00484320<PLAYERPORTRAIT, CHashKeyGUID>, 0x00989320/0x140705CB0<LFGListParty, CHashKeyGUID>, 0x00793840/0x140485830<CFileName, ?>, 0x009B4DF0/0x14073E2F0<EVENTDATEHASH, ?>, 0x0045F3D0/0x140072610<UIADDON, HASHKEY_STRI>, 0x005449B0/0x140198D10<CTexture, HASHKEY_TEXTUREFILE>, 0x005E50A0<OBJ_EFFECT_GROUP_DEF_LOOKUP, HASHKEY_NONE>, 0x0083EED0<FrameScript_EventObject, HASHKEY_CONSTSTRI>, 0x00924390/0x140688780<KEYBINDING, HASHKEY_STRI>, 0x00447070<_BANNEDADDON, HASHKEY_NONE>, 0x006566B0/0x1402F7D00<CVar, HASHKEY_STRI>, 0x00B31880/0x1408FB480<LCDHANDLE, HASHKEY_CONSTSTRI>, 0x009241E0/0x1406884D0<KEYCOMMAND, HASHKEY_STRI>, 0x00924910<MODIFIEDCLICK, HASHKEY_STRI>]
template <class T, class TKey>
T* TSHashTable<T, TKey>::InternalNew(TSExplicitList<T>* listptr, size_t extrabytes, uint32_t flags) {
    return listptr->NewNode(1, extrabytes, flags);
}

// OFFSET: [0x0071D370/0x1403F2770<Liquid::Object, HASHKEY_NONE>, 0x0071D470/0x1403F28B0<Liquid::Object, HASHKEY_NONE>, 0x0064AA60/0x1402E8250<DayNight::DNOverrideSky, HASHKEY_STRI>, 0x0064AB60/0x1402E8390<DayNight::DNOverrideSky, HASHKEY_STRI>, 0x0072E990/0x1404088F0<CMapDoodadDef, HASHKEY_DWORD>, 0x0072EB90/0x140408B70<CMapDoodadDef, HASHKEY_DWORD>, 0x0072EA90/0x140408A30<CMapObjDef, HASHKEY_NONE>, 0x0072EBC0/0x140408BC0<CMapObjDef, HASHKEY_NONE>, 0x00736140/0x140412790<CMapDynamicTexture, HASHKEY_DWORD>, 0x0077BFA0/0x1404685D0<CSceneMaterial, HASHKEY_MATERIAL>, 0x0077C0A0/0x140468710<CSceneMaterial, HASHKEY_MATERIAL>, 0x00736240/0x1404128D0<CMapDynamicTexture, HASHKEY_DWORD>, 0x00747F10<CMapObj, HASHKEY_NONE>, 0x00748010/0x140426E10<CMapObj, HASHKEY_NONE>, 0x0073A810/0x140417C70<DDMaterial, HASHKEY_NONE>, 0x0073A910/0x140417DB0<DDMaterial, HASHKEY_NONE>, 0x004470E0/0x1400533C0<_BANNEDADDON, HASHKEY_NONE>, 0x004471E0/0x140053500<_BANNEDADDON, HASHKEY_NONE>, 0x004ED6A0<FrameFactoryNode, HASHKEY_STRI>, 0x004ED7D0<FrameFactoryNode, HASHKEY_STRI>, 0x00505C70/0x14014C960<FRAMEATTR, HASHKEY_STRI>, 0x00505D70/0x14014CAA0<FRAMEATTR, HASHKEY_STRI>, 0x00526350/0x140175370<SIMPLEFONT, HASHKEY_STRI>, 0x00526450/0x1401754B0<SIMPLEFONT, HASHKEY_STRI>, 0x005438D0/0x140197250<CTexture, HASHKEY_TEXTUREFILE>, 0x005439D0/0x140197390<CTexture, HASHKEY_TEXTUREFILE>, 0x005479D0/0x14019F730<FileNotification::DeferredFile, HASHKEY_NONE>, 0x00547E10/0x14019FD40<FileNotification::DeferredFile, HASHKEY_NONE>, 0x005E3510/0x140261FF0<OBJ_EFFECT_DEF_LOOKUP, HASHKEY_NONE>, 0x005E3F50/0x140262BF0<OBJ_EFFECT_DEF_LOOKUP, HASHKEY_NONE>, 0x005E38B0/0x1402623A0<OBJ_EFFECT_GROUP_DEF_LOOKUP, HASHKEY_NONE>, 0x005E3F80/0x140262C40<OBJ_EFFECT_GROUP_DEF_LOOKUP, HASHKEY_NONE>, 0x005E3D90/0x140262970<OBJ_EFFECT_PACKAGE_DEF_LOOKUP, HASHKEY_NONE>, 0x005E3FB0/0x140262C90<OBJ_EFFECT_PACKAGE_DEF_LOOKUP, HASHKEY_NONE>, 0x00656710/0x1402F7E00<CVar, HASHKEY_STRI>, 0x00656810/0x1402F7F40<CVar, HASHKEY_STRI>, 0x008486F0/0x1405760E0<CGxShader, HASHKEY_STRI>, 0x008487F0/0x140576220<CGxShader, HASHKEY_STRI>, 0x00B31930<LCDHANDLE, HASHKEY_CONSTSTRI>, 0x00B31A30/0x1408FB6D0<LCDHANDLE, HASHKEY_CONSTSTRI>, 0x00548790/0x1401A0B50<CModelBlob::CHashEntry, HASHKEY_MODELBLOBFILE>, 0x00548890/0x1401A0C90<CModelBlob::CHashEntry, HASHKEY_MODELBLOBFILE>, 0x0083F040/0x14056A8F0<FrameScript_EventObject, HASHKEY_CONSTSTRI>, 0x0083F140/0x14056AA30<FrameScript_EventObject, HASHKEY_CONSTSTRI>, 0x00434AC0/0x14003DE70<CShaderEffect, HASHKEY_STRI>, 0x00434BC0/0x14003DFB0<CShaderEffect, HASHKEY_STRI>, 0x0054BD30/0x1401A54D0<CTextureBlobTexture, HASHKEY_TEXTUREBLOBFILE>, 0x0054BE30/0x1401A5610<CTextureBlobTexture, HASHKEY_TEXTUREBLOBFILE>, 0x00704760<CACHEENTRY, HASHKEY_NONE>, 0x00704860<CACHEENTRY, HASHKEY_NONE>, 0x009C6710/0x140754B30<MACRONODE, HASHKEY_NONE>, 0x009C6810/0x140754C70<MACRONODE, HASHKEY_NONE>, 0x00611720/0x14029C1B0<CLightList, HASHKEY_DWORD>, 0x00611820/0x14029C2F0<CLightList, HASHKEY_DWORD>, 0x0070AAD0<SE3SoundChunk, HASHKEY_NONE>, 0x0070AD60<SE3SoundChunk, HASHKEY_NONE>, 0x0070CEE0/0x1403DB9E0<SE3SOUNDKITOBJECT_LOOKUP, HASHKEY_NONE>, 0x0070D010/0x1403DBB70<SE3SOUNDKITOBJECT_LOOKUP, HASHKEY_NONE>, 0x0070C0A0/0x1403DB720<SOUNDKITLOOKUP, HASHKEY_CONSTSTRI>, 0x0070C3A0/0x1403DBE40<SOUNDKITLOOKUP, HASHKEY_CONSTSTRI>, 0x0070C2A0<SE3SoundEmitterLookup, HASHKEY_NONE>, 0x0070C3D0<SE3SoundEmitterLookup, HASHKEY_NONE>, 0x0070CDE0/0x1403DB8A0<SE3COMBINABLESOUNDLIST, HASHKEY_NONE>, 0x0070CFE0/0x1403DBB20<SE3COMBINABLESOUNDLIST, HASHKEY_NONE>, 0x00713C90/0x1403E5F60<SE3ChatSound, HASHKEY_NONE>, 0x00713D90/0x1403E60A0<SE3ChatSound, HASHKEY_NONE>, 0x00714E10/0x1403E77E0<SE3MemorySoundObject, HASHKEY_NONE>, 0x00714F10/0x1403E7920<SE3MemorySoundObject, HASHKEY_NONE>, 0x004FCA30/0x140140E40<FILENAMEHASH, HASHKEY_STR>, 0x004FCB30/0x140140F80<FILENAMEHASH, HASHKEY_STR>, 0x00855630/0x140585C90<CGxDeviceD3d11::InputLayoutRec, InputLayoutKey>, 0x00855F30/0x140586A10<CGxDeviceD3d11::InputLayoutRec, InputLayoutKey>, 0x00855830/0x140585FA0<CGxDeviceD3d11::RasterizerStateRec, HASHKEY_NONE>, 0x00855F60/0x140586A60<CGxDeviceD3d11::RasterizerStateRec, HASHKEY_NONE>, 0x00855A30/0x1405862B0<CGxDeviceD3d11::BlendStateRec, HASHKEY_NONE>, 0x00855F90/0x140586AB0<CGxDeviceD3d11::BlendStateRec, HASHKEY_NONE>, 0x00855C30/0x1405865C0<CGxDeviceD3d11::DepthStateRec, HASHKEY_NONE>, 0x00855FC0/0x140586B00<CGxDeviceD3d11::DepthStateRec, HASHKEY_NONE>, 0x00855E30/0x1405868D0<CGxDeviceD3d11::SamplerStateRec, HASHKEY_NONE>, 0x00855FF0/0x140586B50<CGxDeviceD3d11::SamplerStateRec, HASHKEY_NONE>, 0x00547D10/0x14019FC00<FileNotification::Manifest, HASHKEY_NONE>, 0x00547E40/0x14019FD90<FileNotification::Manifest, HASHKEY_NONE>, 0x00549E90/0x1401A2C50<FONTHASHOBJ, HASHKEY_STR>, 0x00549F90/0x1401A2D90<FONTHASHOBJ, HASHKEY_STR>, 0x004ED490<HashedNode, HASHKEY_STRI>, 0x004ED7A0<HashedNode, HASHKEY_STRI>, 0x0061C450/0x1402AA130<CHARCODEDESC, HASHKEY_NONE>, 0x0061C550/0x1402AA270<CHARCODEDESC, HASHKEY_NONE>, 0x0061C8C0/0x1402AA7E0<KERNNODE, KERNINGHASHKEY>, 0x0061C9C0/0x1402AA920<KERNNODE, KERNINGHASHKEY>, 0x006548A0/0x1402F52D0<CONSOLECOMMAND, HASHKEY_CONSTSTRI>, 0x006549A0/0x1402F5410<CONSOLECOMMAND, HASHKEY_CONSTSTRI>, 0x009F2D30/0x14078D7A0<CGSpellActivationOverlay::COUNTERNODE, HASHKEY_NONE>, 0x009F2F30/0x14078DA20<CGSpellActivationOverlay::PETAURANODE, HASHKEY_NONE>, 0x009F6C20/0x140793110<CGResearchFrame::BRANCH, HASHKEY_NONE>, 0x009F6D90/0x140793250<CGResearchFrame::BLOBPOIHASH, HASHKEY_NONE>, 0x009CA0A0/0x1407592B0<CGCUFProfiles::PROFILENODE, HASHKEY_CONSTUTF8I>, 0x009CA240/0x140759540<CGCUFProfiles::OPTIONENTRY, HASHKEY_CONSTSTR>, 0x00439FD0/0x140044B80<CCameraModeEntry, HASHKEY_NONE>, 0x00484400/0x1400A4150<PLAYERPORTRAIT, CHashKeyGUID>, 0x00484650/0x1400A43D0<UNITPORTRAIT, HASHKEY_NONE>, 0x004848A0/0x1400A46B0<ITEMPORTRAIT, HASHKEY_STR>, 0x0048C650/0x1400B00B0<WowClientDB2_Base::DBTABLE, HASHKEY_NONE>, 0x00496270<SPELLACTIVATIONOVERLAY, HASHKEY_NONE>, 0x004DE580<TALKINGPLAYER, CHashKeyGUID>, 0x004DE810/0x14011D440<MUTEDPLAYER, CHashKeyGUID>, 0x004E5A10/0x1401275E0<DECLINEDWORD, HASHKEY_CONSTSTRI>, 0x00590C50/0x1401FC220<CustomFallbackPath, HASHKEY_NONE>, 0x005A8050/0x140219C70<INVENTORYART, HASHKEY_NONE>, 0x005A8260/0x140219F00<ITEMBYNAME, HASHKEY_UTF8I>, 0x0062CBE0/0x1402C0580<ZONESOUNDOVERRIDELOOKUP, HASHKEY_STRI>, 0x006332C0/0x1402C8B90<DSP_CHAIN_HEAD_LOOKUP, HASHKEY_STRI>, 0x00634010<FOOTSTEPSNDCACHE, HASHKEY_NONE>, 0x008B28C0<ITEMCOOLDOWNHASHNODE, HASHKEY_NONE>, 0x008B2AD0/0x1405F9020<DESTLOCSPELLCASTINDEXHASHNODE, CHashKeyGUID>, 0x008C0D20/0x14060AE30<RACECLASSINFO, HASHKEY_NONE>, 0x009171F0/0x140676EF0<SPELLSBYNAME, HASHKEY_UTF8I>, 0x0093ACF0/0x1406A57C0<CGGuildInfo::GuildRecipesKnown, HASHKEY_NONE>, 0x0093B0F0/0x1406A5CD0<CGGuildInfo::SKILLLINESPELLBITS, HASHKEY_NONE>, 0x00966E60/0x1406DCF00<CGQuestLog::QUESTCOMPLETIONNPC, HASHKEY_NONE>, 0x009A4D20/0x14072AB90<CFlyout, HASHKEY_NONE>, 0x009A4F90/0x14072AE90<SpellToFlyoutMapItem, HASHKEY_NONE>, 0x009A87E0/0x14072F590<FACTION, HASHKEY_NONE>, 0x009FD610/0x14079B610<CGIncomingResurrection::INCOMINGRESLIST, CHashKeyGUID>, 0x008DC8A0/0x140650EE0<TAINTEVENT, HASHKEY_STR>, 0x008F2760<WORDLIST, HASHKEY_LANGUAGE>, 0x0094E510/0x1406BE0B0<ACHIEVEMENT, HASHKEY_NONE>, 0x009B8850/0x1407431E0<KNOWNDANCES, HASHKEY_STRI>, 0x009CDFC0/0x14075EA90<WORLDSTATE, HASHKEY_NONE>, 0x00B32D30/0x1408FD1D0<BITMAPBYICON, HASHKEY_NONE>, 0x0048F890/0x1400B4000<CGObject_C, CHashKeyGUID>, 0x008F2410<ADDONPREFIX, HASHKEY_STR>, 0x0094E740/0x1406BE360<GUILDACHIEVEMENTMEMBERLIST, HASHKEY_NONE>, 0x0045E420/0x140070EA0<UIADDON::METADATA, HASHKEY_STRI>, 0x0056D750/0x1401CDCA0<TRADESKILLLINE, HASHKEY_NONE>, 0x0056D850/0x1401CDDE0<PROFICIENCYSPELL, HASHKEY_NONE>, 0x005E3010/0x140261930<OBJ_EFFECT_GROUP_LOOKUP, HASHKEY_NONE>, 0x005E3230/0x140261CD0<OBJ_EFFECT_REF_LIST_LOOKUP, HASHKEY_NONE>, 0x007938E0<`CMapObjGroup::GetHitTestDebugTextures'::`2'::TEXTURENAME, HASHKEY_CONSTSTRI>, 0x008F2200/0x140623180<CHATTHROTTLE, HASHKEY_STR>, 0x008F2990<PROFANITYCHECK, HASHKEY_STR>, 0x008F2BC0<SPAMCHECK, HASHKEY_STR>, 0x00900F20<USEABLESTYLE, HASHKEY_NONE>, 0x0090A080/0x1406637E0<ACCOUNTIDCACHE, HASHKEY_DWORD>, 0x009172F0/0x140677030<OVERRIDE_SPELL_NODE, HASHKEY_NONE>, 0x009173F0/0x140677170<OVERRIDE_SPELL_BY_NAME_NODE, HASHKEY_UTF8I>, 0x009CC810/0x14075C8B0<CGAutoCompleteName::AUTOCOMPLETE_CHARACTER_HASHNODE, CHashKeyGUID>, 0x009CCA10/0x14075CB30<CGAutoCompleteName::AUTOCOMPLETE_BATTLENET_HASHNODE, HASHKEY_NONE>, 0x009DEFF0/0x140775460<CGQuestObjectiveCache::REQUIREDUNITNODE, HASHKEY_NONE>, 0x009DF1F0/0x1407756E0<CGQuestObjectiveCache::REQUIREDITEMNODE, HASHKEY_NONE>, 0x009E49E0/0x14077C320<TAXIPATHNODESCLIENT, HASHKEY_NONE>, 0x0043A0D0/0x140044CC0<CCameraModeEntry, HASHKEY_NONE>, 0x004849A0/0x1400A47F0<PLAYERPORTRAIT, CHashKeyGUID>, 0x004849D0/0x1400A4840<UNITPORTRAIT, HASHKEY_NONE>, 0x00484A00/0x1400A4890<ITEMPORTRAIT, HASHKEY_STR>, 0x0048C750/0x1400B01F0<WowClientDB2_Base::DBTABLE, HASHKEY_NONE>, 0x00496370<SPELLACTIVATIONOVERLAY, HASHKEY_NONE>, 0x004DE910<TALKINGPLAYER, CHashKeyGUID>, 0x004DE940/0x14011D5D0<MUTEDPLAYER, CHashKeyGUID>, 0x004E5B10/0x140127720<DECLINEDWORD, HASHKEY_CONSTSTRI>, 0x00590D50/0x1401FC360<CustomFallbackPath, HASHKEY_NONE>, 0x005A8360/0x14021A040<INVENTORYART, HASHKEY_NONE>, 0x005A8390/0x14021A090<ITEMBYNAME, HASHKEY_UTF8I>, 0x0062CCE0/0x1402C06C0<ZONESOUNDOVERRIDELOOKUP, HASHKEY_STRI>, 0x006333C0/0x1402C8CD0<DSP_CHAIN_HEAD_LOOKUP, HASHKEY_STRI>, 0x00634110<FOOTSTEPSNDCACHE, HASHKEY_NONE>, 0x008B2BD0<ITEMCOOLDOWNHASHNODE, HASHKEY_NONE>, 0x008B2C00/0x1405F91B0<DESTLOCSPELLCASTINDEXHASHNODE, CHashKeyGUID>, 0x008C0E20/0x14060AF70<RACECLASSINFO, HASHKEY_NONE>, 0x008DC9D0/0x140651070<TAINTEVENT, HASHKEY_STR>, 0x008F2CC0/0x140623EE0<CHATTHROTTLE, HASHKEY_STR>, 0x008F2CF0<ADDONPREFIX, HASHKEY_STR>, 0x008F2D20<WORDLIST, HASHKEY_LANGUAGE>, 0x008F2D50<PROFANITYCHECK, HASHKEY_STR>, 0x008F2D80<SPAMCHECK, HASHKEY_STR>, 0x00901020<USEABLESTYLE, HASHKEY_NONE>, 0x0090A180/0x140663920<ACCOUNTIDCACHE, HASHKEY_DWORD>, 0x00917520/0x140677300<OVERRIDE_SPELL_NODE, HASHKEY_NONE>, 0x00917550/0x140677350<OVERRIDE_SPELL_BY_NAME_NODE, HASHKEY_UTF8I>, 0x009174F0/0x1406772B0<SPELLSBYNAME, HASHKEY_UTF8I>, 0x00917F50/0x140678560<OVERRIDE_SPELL_NODE, HASHKEY_NONE>, 0x00917F80/0x1406785B0<OVERRIDE_SPELL_BY_NAME_NODE, HASHKEY_UTF8I>, 0x0093B1F0/0x1406A5E10<CGGuildInfo::GuildRecipesKnown, HASHKEY_NONE>, 0x0093B250/0x1406A5EB0<CGGuildInfo::SKILLLINESPELLBITS, HASHKEY_NONE>, 0x0094E840/0x1406BE4A0<ACHIEVEMENT, HASHKEY_NONE>, 0x0094E870/0x1406BE4F0<GUILDACHIEVEMENTMEMBERLIST, HASHKEY_NONE>, 0x00966F60/0x1406DD040<CGQuestLog::QUESTCOMPLETIONNPC, HASHKEY_NONE>, 0x009A5090/0x14072AFD0<CFlyout, HASHKEY_NONE>, 0x009A50C0/0x14072B020<SpellToFlyoutMapItem, HASHKEY_NONE>, 0x009A88E0/0x14072F6D0<FACTION, HASHKEY_NONE>, 0x009B8950/0x140743320<KNOWNDANCES, HASHKEY_STRI>, 0x009CA340/0x140759680<CGCUFProfiles::PROFILENODE, HASHKEY_CONSTUTF8I>, 0x009CCB10/0x14075CC70<CGAutoCompleteName::AUTOCOMPLETE_CHARACTER_HASHNODE, CHashKeyGUID>, 0x009CCB40/0x14075CCC0<CGAutoCompleteName::AUTOCOMPLETE_BATTLENET_HASHNODE, HASHKEY_NONE>, 0x009CE0C0/0x14075EBD0<WORLDSTATE, HASHKEY_NONE>, 0x009DF2F0/0x140775820<CGQuestObjectiveCache::REQUIREDUNITNODE, HASHKEY_NONE>, 0x009DF320/0x140775870<CGQuestObjectiveCache::REQUIREDITEMNODE, HASHKEY_NONE>, 0x009E4BB0/0x14077C5C0<TAXIPATHNODESCLIENT, HASHKEY_NONE>, 0x009F3030/0x14078DB60<CGSpellActivationOverlay::COUNTERNODE, HASHKEY_NONE>, 0x009F3060/0x14078DBB0<CGSpellActivationOverlay::PETAURANODE, HASHKEY_NONE>, 0x009F6E90/0x140793390<CGResearchFrame::BRANCH, HASHKEY_NONE>, 0x009F6EC0/0x1407933E0<CGResearchFrame::BLOBPOIHASH, HASHKEY_NONE>, 0x009FD710/0x14079B750<CGIncomingResurrection::INCOMINGRESLIST, CHashKeyGUID>, 0x00B32E30/0x1408FD310<BITMAPBYICON, HASHKEY_NONE>, 0x004A66F0/0x1400D6DA0<DBCache<CreatureStats_C, int, HASHKEY_INT>::REVERSEENTRY, HASHKEY_CONSTSTR>, 0x004A6540/0x1400D6B60<DBCache<CreatureStats_C, int, HASHKEY_INT>::DBCACHEHASH, HASHKEY_INT>, 0x004A69A0/0x1400D7120<DBCache<GameObjectStats_C, int, HASHKEY_INT>::REVERSEENTRY, HASHKEY_CONSTSTR>, 0x004A67F0/0x1400D6EE0<DBCache<GameObjectStats_C, int, HASHKEY_INT>::DBCACHEHASH, HASHKEY_INT>, 0x004A6C50/0x1400D74E0<DBCache<NPCText, int, HASHKEY_INT>::REVERSEENTRY, HASHKEY_CONSTSTR>, 0x004A6AA0/0x1400D7260<DBCache<NPCText, int, HASHKEY_INT>::DBCACHEHASH, HASHKEY_INT>, 0x004A6F10/0x1400D7880<DBCache<NameCache, unsigned __int64, CHashKeyGUID>::REVERSEENTRY, HASHKEY_CONSTSTR>, 0x004A6D50/0x1400D7620<DBCache<NameCache, unsigned __int64, CHashKeyGUID>::DBCACHEHASH, CHashKeyGUID>, 0x004A71B0/0x1400D7C00<DBCache<GuildStats_C, unsigned __int64, CHashKeyGUID>::REVERSEENTRY, HASHKEY_CONSTSTR>, 0x004A7010/0x1400D79C0<DBCache<GuildStats_C, unsigned __int64, CHashKeyGUID>::DBCACHEHASH, CHashKeyGUID>, 0x004A7450/0x1400D7F80<DBCache<QuestCache, int, HASHKEY_INT>::REVERSEENTRY, HASHKEY_CONSTSTR>, 0x004A72B0/0x1400D7D40<DBCache<QuestCache, int, HASHKEY_INT>::DBCACHEHASH, HASHKEY_INT>, 0x004A7700/0x1400D82F0<DBCache<PageTextCache_C, int, HASHKEY_INT>::REVERSEENTRY, HASHKEY_CONSTSTR>, 0x004A7550/0x1400D80C0<DBCache<PageTextCache_C, int, HASHKEY_INT>::DBCACHEHASH, HASHKEY_INT>, 0x004A79B0/0x1400D8690<DBCache<PetNameCache, int, HASHKEY_INT>::REVERSEENTRY, HASHKEY_CONSTSTR>, 0x004A7800/0x1400D8430<DBCache<PetNameCache, int, HASHKEY_INT>::DBCACHEHASH, HASHKEY_INT>, 0x004A7C50/0x1400D8A10<DBCache<CGPetition, int, HASHKEY_INT>::REVERSEENTRY, HASHKEY_CONSTSTR>, 0x004A7AB0/0x1400D87D0<DBCache<CGPetition, int, HASHKEY_INT>::DBCACHEHASH, HASHKEY_INT>, 0x004A7EF0/0x1400D8D90<DBCache<ItemTextCache_C, unsigned __int64, CHashKeyGUID>::REVERSEENTRY, HASHKEY_CONSTSTR>, 0x004A7D50/0x1400D8B50<DBCache<ItemTextCache_C, unsigned __int64, CHashKeyGUID>::DBCACHEHASH, CHashKeyGUID>, 0x004A8190/0x1400D9110<DBCache<WardenCachedModule, CWardenKey, CWardenKey>::REVERSEENTRY, HASHKEY_CONSTSTR>, 0x004A7FF0/0x1400D8ED0<DBCache<WardenCachedModule, CWardenKey, CWardenKey>::DBCACHEHASH, CWardenKey>, 0x004A8430/0x1400D9490<DBCache<ArenaTeamCache, int, HASHKEY_INT>::REVERSEENTRY, HASHKEY_CONSTSTR>, 0x004A8290/0x1400D9250<DBCache<ArenaTeamCache, int, HASHKEY_INT>::DBCACHEHASH, HASHKEY_INT>, 0x004A86F0/0x1400D9830<DBCache<DanceCache, int, HASHKEY_INT>::REVERSEENTRY, HASHKEY_CONSTSTR>, 0x004A8530/0x1400D95D0<DBCache<DanceCache, int, HASHKEY_INT>::DBCACHEHASH, HASHKEY_INT>, 0x004A87F0/0x1400D9970<DBCache<CreatureStats_C,int,HASHKEY_INT>::DBCACHEHASH,HASHKEY_INT>, 0x004A8820/0x1400D99C0<DBCache<CreatureStats_C,int,HASHKEY_INT>::REVERSEENTRY,HASHKEY_CONSTSTR>, 0x004A8850/0x1400D9A10<DBCache<GameObjectStats_C,int,HASHKEY_INT>::DBCACHEHASH,HASHKEY_INT>, 0x004A8880/0x1400D9A60<DBCache<GameObjectStats_C,int,HASHKEY_INT>::REVERSEENTRY,HASHKEY_CONSTSTR>, 0x004A88B0/0x1400D9AB0<DBCache<NPCText,int,HASHKEY_INT>::DBCACHEHASH,HASHKEY_INT>, 0x004A88E0/0x1400D9B00<DBCache<NPCText,int,HASHKEY_INT>::REVERSEENTRY,HASHKEY_CONSTSTR>, 0x004A8910/0x1400D9B50<DBCache<NameCache,unsigned __int64,CHashKeyGUID>::DBCACHEHASH,CHashKeyGUID>, 0x004A8940/0x1400D9BA0<DBCache<NameCache,unsigned __int64,CHashKeyGUID>::REVERSEENTRY,HASHKEY_CONSTSTR>, 0x004A8970/0x1400D9BF0<DBCache<GuildStats_C,unsigned __int64,CHashKeyGUID>::DBCACHEHASH,CHashKeyGUID>, 0x004A89A0/0x1400D9C40<DBCache<GuildStats_C,unsigned __int64,CHashKeyGUID>::REVERSEENTRY,HASHKEY_CONSTSTR>, 0x004A89D0/0x1400D9C90<DBCache<QuestCache,int,HASHKEY_INT>::DBCACHEHASH,HASHKEY_INT>, 0x004A8A00/0x1400D9CE0<DBCache<QuestCache,int,HASHKEY_INT>::REVERSEENTRY,HASHKEY_CONSTSTR>, 0x004A8A30/0x1400D9D30<DBCache<PageTextCache_C,int,HASHKEY_INT>::DBCACHEHASH,HASHKEY_INT>, 0x004A8A60/0x1400D9D80<DBCache<PageTextCache_C,int,HASHKEY_INT>::REVERSEENTRY,HASHKEY_CONSTSTR>, 0x004A8A90/0x1400D9DD0<DBCache<PetNameCache,int,HASHKEY_INT>::DBCACHEHASH,HASHKEY_INT>, 0x004A8AC0/0x1400D9E20<DBCache<PetNameCache,int,HASHKEY_INT>::REVERSEENTRY,HASHKEY_CONSTSTR>, 0x004A8AF0/0x1400D9E70<DBCache<CGPetition,int,HASHKEY_INT>::DBCACHEHASH,HASHKEY_INT>, 0x004A8B20/0x1400D9EC0<DBCache<CGPetition,int,HASHKEY_INT>::REVERSEENTRY,HASHKEY_CONSTSTR>, 0x004A8B50/0x1400D9F10<DBCache<ItemTextCache_C,unsigned __int64,CHashKeyGUID>::DBCACHEHASH,CHashKeyGUID>, 0x004A8B80/0x1400D9F60<DBCache<ItemTextCache_C,unsigned __int64,CHashKeyGUID>::REVERSEENTRY,HASHKEY_CONSTSTR>, 0x004A8BB0/0x1400D9FB0<DBCache<WardenCachedModule,CWardenKey,CWardenKey>::DBCACHEHASH,CWardenKey>, 0x004A8BE0/0x1400DA000<DBCache<WardenCachedModule,CWardenKey,CWardenKey>::REVERSEENTRY,HASHKEY_CONSTSTR>, 0x004A8C10/0x1400DA050<DBCache<ArenaTeamCache,int,HASHKEY_INT>::DBCACHEHASH,HASHKEY_INT>, 0x004A8C40/0x1400DA0A0<DBCache<ArenaTeamCache,int,HASHKEY_INT>::REVERSEENTRY,HASHKEY_CONSTSTR>, 0x004A8C70/0x1400DA0F0<DBCache<DanceCache,int,HASHKEY_INT>::DBCACHEHASH,HASHKEY_INT>, 0x004A8CA0/0x1400DA140<DBCache<DanceCache,int,HASHKEY_INT>::REVERSEENTRY,HASHKEY_CONSTSTR>, 0x0045E620/0x140071120<UIADDON::METADATA, HASHKEY_STRI>, 0x0048F990/0x1400B4140<CGObject_C, CHashKeyGUID>, 0x0056D950/0x1401CDF20<TRADESKILLLINE, HASHKEY_NONE>, 0x0056D980/0x1401CDF70<PROFICIENCYSPELL, HASHKEY_NONE>, 0x005D59A0/0x1402502A0<UnitThreat, CHashKeyGUID>, 0x005E3EF0/0x140262B50<OBJ_EFFECT_GROUP_LOOKUP, HASHKEY_NONE>, 0x00545950/0x14019C8E0<ProfileInternal::KEYVALUE, HASHKEY_CONSTSTRI>, 0x00595F20/0x1402036A0<VoidItem_C, HASHKEY_NONE>, 0x005E2F10/0x1402617F0<OBJ_EFFECT_REF_LOOKUP, HASHKEY_NONE>, 0x005E3C90/0x140262830<OBJ_EFFECT_GROUP_DEF_REF_LIST_LOOKUP, HASHKEY_NONE>, 0x008DC670<SAVEDVARIABLE, HASHKEY_STR>, 0x00989340/0x140705CC0<LFGListParty, CHashKeyGUID>, 0x00989560/0x140705F90<LFGListPlayer, CHashKeyGUID>, 0x00A0EF70<MacroConditions, HASHKEY_STR>, 0x005D58A0/0x140250160<UnitThreat, CHashKeyGUID>, 0x0093AFF0/0x1406A5B90<CGGuildInfo::SKILLLINESPELLS, HASHKEY_NONE>, 0x0045E520/0x140070FE0<UIADDON, HASHKEY_STRI>, 0x00924BA0/0x140689330<CLICKFRAME, HASHKEY_STR>, 0x009B5070/0x14073E690<DYNAMICHOLIDAYHASH, HASHKEY_NONE>, 0x0045E680/0x1400711C0<UIADDON, HASHKEY_STRI>, 0x00924D60/0x1406895B0<CLICKFRAME, HASHKEY_STR>, 0x009B51A0/0x14073E820<DYNAMICHOLIDAYHASH, HASHKEY_NONE>, 0x0044B4B0/0x140058890<MOUSELOOKBINDING, HASHKEY_STRI>, 0x00924CA0/0x140689470<KEYCOMMAND, HASHKEY_STRI>, 0x00924D00/0x140689510<OVERRIDEKEYBINDING, HASHKEY_STRI>, 0x00924D30/0x140689560<MODIFIEDCLICK, HASHKEY_STRI>, 0x0044B260/0x1406883C0<SPELLBYNAME, ?>, 0x0045E100/0x140070AB0<ADDONSTATE, HASHKEY_STRI>, 0x004C2CB0/0x1400FDA90<RGN, HASHKEY_NONE>, 0x004D96D0/0x140117A50<TIMESTAMPSTRUCT, HASHKEY_NONE>, 0x00545A50/0x14019CA20<ProfileInternal::SECTION, HASHKEY_CONSTSTRI>, 0x006157A0/0x1402A1500<BATCHEDRENDERFONTDESC, HASHKEY_PTR>, 0x00622AC0/0x1402B3C30<FACEDATA, HASHKEY_STRI>, 0x009243F0/0x140688880<KEYBINDING, HASHKEY_STRI>, 0x0099DBB0/0x140720EB0<TalentGroupItem_C, HASHKEY_NONE>, 0x0099DDC0/0x1407211D0<TalentGroupTierItem_C, HASHKEY_NONE>, 0x009B4E60/0x14073E400<EVENTDATEHASH, ?>, 0x0044B3B0/0x140058750, 0x0045E650/0x140071170, 0x004D9920/0x140117DD0, 0x00545B80/0x14019CBB0, 0x006158A0/0x1402A1640, 0x00622BC0/0x1402B3D70, 0x00924250/0x1406885E0, 0x00924770/0x140688DC0, 0x00924990/0x1406890A0, 0x00924CD0/0x1406894C0, 0x0099DEC0/0x140721310, 0x0099DEF0/0x140721360, 0x009B5170/0x14073E7D0]
template <class T, class TKey>
TSHashTable<T, TKey>::~TSHashTable() {
    this->InternalClear(1);
}

// OFFSET: [0x00505A50/0x14014C5E0<FRAMEATTR, HASHKEY_STRI>, 0x00526190<SIMPLEFONT, HASHKEY_STRI>, 0x00543000/0x14024FDE0<CTexture, HASHKEY_TEXTUREFILE>, 0x00548670<CModelBlob::CHashEntry, HASHKEY_MODELBLOBFILE>, 0x00545820/0x14019C6D0<ProfileInternal::KEYVALUE, HASHKEY_CONSTSTRI>, 0x00549D30<FONTHASHOBJ, HASHKEY_STR>, 0x0054BB00<CTextureBlobTexture, HASHKEY_TEXTUREBLOBFILE>, 0x00704610<CACHEENTRY, HASHKEY_NONE>, 0x008F20C0<CHATTHROTTLE, HASHKEY_STR>, 0x008F2860<PROFANITYCHECK, HASHKEY_STR>, 0x008F2A90<SPAMCHECK, HASHKEY_STR>, 0x00900D30<USEABLESTYLE, HASHKEY_NONE>, 0x009244F0/0x1406889C0<OVERRIDEKEYBINDING, HASHKEY_STRI>, 0x00924870/0x140688F00<MODIFIEDCLICK, HASHKEY_STRI>, 0x00924A90<CLICKFRAME, HASHKEY_STR>, 0x009C6610<MACRONODE, HASHKEY_NONE>, 0x009E48E0<TAXIPATHNODESCLIENT, HASHKEY_NONE>, 0x009B4F60<DYNAMICHOLIDAYHASH, HASHKEY_NONE>, 0x00446FD0<_BANNEDADDON, HASHKEY_NONE>, 0x0045E300/0x140070CB0<UIADDON::METADATA, HASHKEY_STRI>, 0x0044B1F0/0x140058560<MOUSELOOKBINDING, HASHKEY_STRI>, 0x006114C0<CLightList, HASHKEY_DWORD>, 0x0061C2D0<CHARCODEDESC, HASHKEY_NONE>, 0x0061C7A0<KERNNODE, KERNINGHASHKEY>, 0x006229A0<FACEDATA, HASHKEY_STRI>, 0x0056D520/0x1401CD8D0<TRADESKILLLINE, HASHKEY_NONE>, 0x0056D650/0x1401CDAD0<PROFICIENCYSPELL, HASHKEY_NONE>, 0x005E2D10/0x1402614D0<OBJ_EFFECT_GROUP_LOOKUP, HASHKEY_NONE>, 0x005E3110/0x140261A70<OBJ_EFFECT_REF_LIST_LOOKUP, HASHKEY_NONE>, 0x005E3330<OBJ_EFFECT_DEF_LOOKUP, HASHKEY_NONE>, 0x005E3610<OBJ_EFFECT_GROUP_DEF_LOOKUP, HASHKEY_NONE>, 0x005E39B0<OBJ_EFFECT_PACKAGE_DEF_LOOKUP, HASHKEY_NONE>, 0x005D5680/0x14024FDE0<UnitThreat, CHashKeyGUID>, 0x006565F0<CVar, HASHKEY_STRI>, 0x00654790<CONSOLECOMMAND, HASHKEY_CONSTSTRI>, 0x00590B30<CustomFallbackPath, HASHKEY_NONE>, 0x008C0C20<RACECLASSINFO, HASHKEY_NONE>, 0x004ED350<HashedNode, HASHKEY_STRI>, 0x004ED590<FrameFactoryNode, HASHKEY_STRI>, 0x0083ED90<FrameScript_EventObject, HASHKEY_CONSTSTRI>, 0x00B31810<LCDHANDLE, HASHKEY_CONSTSTRI>, 0x004349C0<CShaderEffect, HASHKEY_STRI>, 0x0071D240<Liquid::Object, HASHKEY_NONE>, 0x0064A960<DayNight::DNOverrideSky, HASHKEY_STRI>, 0x0072E570/0x140408210<CMapDoodadDef, HASHKEY_DWORD>, 0x0072E670/0x1404083E0<CMapObjDef, HASHKEY_NONE>, 0x00736030<CMapDynamicTexture, HASHKEY_DWORD>, 0x0077BD00<CSceneMaterial, HASHKEY_MATERIAL>, 0x00747D00/0x1404083E0<CMapObj, HASHKEY_NONE>, 0x0073A5C0<DDMaterial, HASHKEY_NONE>, 0x005478C0<FileNotification::DeferredFile, HASHKEY_NONE>, 0x0070A9B0<SE3SoundChunk, HASHKEY_NONE>, 0x0070BE80<SE3SOUNDKITOBJECT_LOOKUP, HASHKEY_NONE>, 0x0070BF80<SOUNDKITLOOKUP, HASHKEY_CONSTSTRI>, 0x0070C1A0<SE3SoundEmitterLookup, HASHKEY_NONE>, 0x0070C400<SE3COMBINABLESOUNDLIST, HASHKEY_NONE>, 0x00713A10<SE3ChatSound, HASHKEY_NONE>, 0x00714BF0<SE3MemorySoundObject, HASHKEY_NONE>, 0x004FC770<FILENAMEHASH, HASHKEY_STR>, 0x00855530/0x140585AC0<CGxDeviceD3d11::InputLayoutRec, InputLayoutKey>, 0x00855730/0x140585DD0<CGxDeviceD3d11::RasterizerStateRec, HASHKEY_NONE>, 0x00855930/0x1405860E0<CGxDeviceD3d11::BlendStateRec, HASHKEY_NONE>, 0x00855B30/0x1405863F0<CGxDeviceD3d11::DepthStateRec, HASHKEY_NONE>, 0x00855D30/0x140586700<CGxDeviceD3d11::SamplerStateRec, HASHKEY_NONE>, 0x00547AD0<FileNotification::Manifest, HASHKEY_NONE>, 0x009F2C30<CGSpellActivationOverlay::COUNTERNODE, HASHKEY_NONE>, 0x009F2E30<CGSpellActivationOverlay::PETAURANODE, HASHKEY_NONE>, 0x009F6B00<CGResearchFrame::BRANCH, HASHKEY_NONE>, 0x009F6D20<CGResearchFrame::BLOBPOIHASH, HASHKEY_NONE>, 0x009C9F90<CGCUFProfiles::PROFILENODE, HASHKEY_CONSTUTF8I>, 0x00439EB0<CCameraModeEntry, HASHKEY_NONE>, 0x004842B0<PLAYERPORTRAIT, CHashKeyGUID>, 0x00484500<UNITPORTRAIT, HASHKEY_NONE>, 0x00484750<ITEMPORTRAIT, HASHKEY_STR>, 0x0048C540<WowClientDB2_Base::DBTABLE, HASHKEY_NONE>, 0x00496170<SPELLACTIVATIONOVERLAY, HASHKEY_NONE>, 0x004DE470<TALKINGPLAYER, CHashKeyGUID>, 0x004DE680<MUTEDPLAYER, CHashKeyGUID>, 0x004E5900<DECLINEDWORD, HASHKEY_CONSTSTRI>, 0x005A7F20<INVENTORYART, HASHKEY_NONE>, 0x005A8150<ITEMBYNAME, HASHKEY_UTF8I>, 0x0062CAD0<ZONESOUNDOVERRIDELOOKUP, HASHKEY_STRI>, 0x00633140<DSP_CHAIN_HEAD_LOOKUP, HASHKEY_STRI>, 0x00633E00<FOOTSTEPSNDCACHE, HASHKEY_NONE>, 0x008B27C0<ITEMCOOLDOWNHASHNODE, HASHKEY_NONE>, 0x008B29C0<DESTLOCSPELLCASTINDEXHASHNODE, CHashKeyGUID>, 0x00916820<OVERRIDE_SPELL_NODE, HASHKEY_NONE>, 0x00916930<OVERRIDE_SPELL_BY_NAME_NODE, HASHKEY_UTF8I>, 0x00917000<SPELLSBYNAME, HASHKEY_UTF8I>, 0x0093ABE0<CGGuildInfo::GuildRecipesKnown, HASHKEY_NONE>, 0x0093ADF0<CGGuildInfo::SKILLLINESPELLBITS, HASHKEY_NONE>, 0x00966D30<CGQuestLog::QUESTCOMPLETIONNPC, HASHKEY_NONE>, 0x009A4C00<CFlyout, HASHKEY_NONE>, 0x009A4E20<SpellToFlyoutMapItem, HASHKEY_NONE>, 0x009A86B0<FACTION, HASHKEY_NONE>, 0x009FD4D0<CGIncomingResurrection::INCOMINGRESLIST, CHashKeyGUID>, 0x0045E200<UIADDON, HASHKEY_STRI>, 0x008DC770<TAINTEVENT, HASHKEY_STR>, 0x008F2620<WORDLIST, HASHKEY_LANGUAGE>, 0x0094E3F0<ACHIEVEMENT, HASHKEY_NONE>, 0x009B8740<KNOWNDANCES, HASHKEY_STRI>, 0x009CDEC0<WORLDSTATE, HASHKEY_NONE>, 0x00B32C20<BITMAPBYICON, HASHKEY_NONE>, 0x0048F590/0x1400B3CC0<CGObject_C, CHashKeyGUID>, 0x008F2300<ADDONPREFIX, HASHKEY_STR>, 0x0094E610<GUILDACHIEVEMENTMEMBERLIST, HASHKEY_NONE>, 0x00595E20/0x1402034D0<VoidItem_C, HASHKEY_NONE>, 0x007937D0<`CMapObjGroup::GetHitTestDebugTextures'::`2'::TEXTURENAME, HASHKEY_CONSTSTRI>, 0x00909F70<ACCOUNTIDCACHE, HASHKEY_DWORD>, 0x009CC700<CGAutoCompleteName::AUTOCOMPLETE_CHARACTER_HASHNODE, CHashKeyGUID>, 0x009CC910<CGAutoCompleteName::AUTOCOMPLETE_BATTLENET_HASHNODE, HASHKEY_NONE>, 0x009DEEF0<CGQuestObjectiveCache::REQUIREDUNITNODE, HASHKEY_NONE>, 0x009DF0F0<CGQuestObjectiveCache::REQUIREDITEMNODE, HASHKEY_NONE>, 0x004A3EB0/0x1400D2960<DBCache<CreatureStats_C, int, HASHKEY_INT>::DBCACHEHASH, HASHKEY_INT>, 0x004A3F80/0x1400D2AF0<DBCache<CreatureStats_C, int, HASHKEY_INT>::REVERSEENTRY, HASHKEY_CONSTSTR>, 0x004A40A0/0x1400D2CD0<DBCache<GameObjectStats_C, int, HASHKEY_INT>::DBCACHEHASH, HASHKEY_INT>, 0x004A4170/0x1400D2E60<DBCache<GameObjectStats_C, int, HASHKEY_INT>::REVERSEENTRY, HASHKEY_CONSTSTR>, 0x004A4280/0x1400D3040<DBCache<NPCText, int, HASHKEY_INT>::DBCACHEHASH, HASHKEY_INT>, 0x004A4350/0x1400D31E0<DBCache<NPCText, int, HASHKEY_INT>::REVERSEENTRY, HASHKEY_CONSTSTR>, 0x004A4460/0x1400D33C0<DBCache<NameCache, unsigned __int64, CHashKeyGUID>::DBCACHEHASH, CHashKeyGUID>, 0x004A4530/0x1400D3560<DBCache<NameCache, unsigned __int64, CHashKeyGUID>::REVERSEENTRY, HASHKEY_CONSTSTR>, 0x004A4640/0x1400D3740<DBCache<GuildStats_C, unsigned __int64, CHashKeyGUID>::DBCACHEHASH, CHashKeyGUID>, 0x004A4710/0x1400D38D0<DBCache<GuildStats_C, unsigned __int64, CHashKeyGUID>::REVERSEENTRY, HASHKEY_CONSTSTR>, 0x004A4820/0x1400D3AB0<DBCache<QuestCache, int, HASHKEY_INT>::DBCACHEHASH, HASHKEY_INT>, 0x004A48F0/0x1400D3C40<DBCache<QuestCache, int, HASHKEY_INT>::REVERSEENTRY, HASHKEY_CONSTSTR>, 0x004A4A00/0x1400D3E20<DBCache<PageTextCache_C, int, HASHKEY_INT>::DBCACHEHASH, HASHKEY_INT>, 0x004A4B20/0x1400D3F90<DBCache<PageTextCache_C, int, HASHKEY_INT>::REVERSEENTRY, HASHKEY_CONSTSTR>, 0x004A4C30/0x1400D4170<DBCache<PetNameCache, int, HASHKEY_INT>::DBCACHEHASH, HASHKEY_INT>, 0x004A4D00/0x1400D4310<DBCache<PetNameCache, int, HASHKEY_INT>::REVERSEENTRY, HASHKEY_CONSTSTR>, 0x004A4E10/0x1400D44F0<DBCache<CGPetition, int, HASHKEY_INT>::DBCACHEHASH, HASHKEY_INT>, 0x004A4EE0/0x1400D4680<DBCache<CGPetition, int, HASHKEY_INT>::REVERSEENTRY, HASHKEY_CONSTSTR>, 0x004A4FF0/0x1400D4860<DBCache<ItemTextCache_C, unsigned __int64, CHashKeyGUID>::DBCACHEHASH, CHashKeyGUID>, 0x004A50C0/0x1400D49F0<DBCache<ItemTextCache_C, unsigned __int64, CHashKeyGUID>::REVERSEENTRY, HASHKEY_CONSTSTR>, 0x004A51D0/0x1400D4BD0<DBCache<WardenCachedModule, CWardenKey, CWardenKey>::DBCACHEHASH, CWardenKey>, 0x004A52A0/0x1400D4D60<DBCache<WardenCachedModule, CWardenKey, CWardenKey>::REVERSEENTRY, HASHKEY_CONSTSTR>, 0x004A53B0/0x1400D4F40<DBCache<ArenaTeamCache, int, HASHKEY_INT>::DBCACHEHASH, HASHKEY_INT>, 0x004A5480/0x1400D50D0<DBCache<ArenaTeamCache, int, HASHKEY_INT>::REVERSEENTRY, HASHKEY_CONSTSTR>, 0x004A5590/0x1400D52B0<DBCache<DanceCache, int, HASHKEY_INT>::DBCACHEHASH, HASHKEY_INT>, 0x004A5660/0x1400D5460<DBCache<DanceCache, int, HASHKEY_INT>::REVERSEENTRY, HASHKEY_CONSTSTR>, 0x0093AEF0/0x1406A59C0<CGGuildInfo::SKILLLINESPELLS, HASHKEY_NONE>, 0x004C29F0/0x1400FD6C0<RGN, HASHKEY_NONE>, 0x00545720/0x14019C580<ProfileInternal::SECTION, HASHKEY_CONSTSTRI>, 0x005E2E10/0x140261620<OBJ_EFFECT_REF_LOOKUP, HASHKEY_NONE>, 0x005E3B20<OBJ_EFFECT_GROUP_DEF_REF_LIST_LOOKUP, HASHKEY_NONE>, 0x009891E0/0x140705B00<LFGListParty, CHashKeyGUID>, 0x00989440/0x140705E00<LFGListPlayer, CHashKeyGUID>, 0x00A0EDE0<MacroConditions, HASHKEY_STR>, 0x0045DFF0/0x1400708D0<ADDONSTATE, HASHKEY_STRI>, 0x00615680/0x1402A1370, 0x00924130/0x140688330<KEYCOMMAND, HASHKEY_STRI>, 0x0099DAA0<TalentGroupItem_C, HASHKEY_NONE>, 0x0099DCB0/0x140720FF0<TalentGroupTierItem_C, HASHKEY_NONE>, 0x00A31DC0/0x1407DBD70<TIMESTAMPSTRUCT, HASHKEY_NONE>]
template <class T, class TKey>
TSHashTable<T, TKey>::TSHashTable() {
    this->m_fullnessIndicator = 0;
    this->m_fulllist.ChangeLinkOffset(offsetof(T, m_linktofull));
    this->m_slotmask = 0xFFFFFFFF;
    this->m_maxslots = 0x2000;
}

// OFFSET: [0x005D4E10/0x14024F2A0<RGN, HASHKEY_NONE>]
template <class T, class TKey>
void TSHashTable<T, TKey>::Clear() {
    this->InternalClear(0);
}

// OFFSET: [0x00505D10/0x14014CA20<FRAMEATTR, HASHKEY_STRI>, 0x005263F0/0x140175430<SIMPLEFONT, HASHKEY_STRI>, 0x00543970/0x140197310<CTexture, HASHKEY_TEXTUREFILE>, 0x00548830/0x1401A0C10<CModelBlob::CHashEntry, HASHKEY_MODELBLOBFILE>, 0x005459F0/0x14019C9A0<ProfileInternal::KEYVALUE, HASHKEY_CONSTSTRI>, 0x00549F30/0x1401A2D10<FONTHASHOBJ, HASHKEY_STR>, 0x0054BDD0/0x1401A5590<CTextureBlobTexture, HASHKEY_TEXTUREBLOBFILE>, 0x0062CC80/0x1402C0640<ZONESOUNDOVERRIDELOOKUP, HASHKEY_STRI>, 0x00633360/0x1402C8C50<DSP_CHAIN_HEAD_LOOKUP, HASHKEY_STRI>, 0x006340B0/0x1402CA070<FOOTSTEPSNDCACHE, HASHKEY_NONE>, 0x0048F930/0x1400B40C0<CGObject_C, CHashKeyGUID>, 0x00704800<CACHEENTRY, HASHKEY_NONE>, 0x008F22A0/0x140623240<CHATTHROTTLE, HASHKEY_STR>, 0x008F2800<WORDLIST, HASHKEY_LANGUAGE>, 0x008F2A30<PROFANITYCHECK, HASHKEY_STR>, 0x008F2C60/0x140623E60<SPAMCHECK, HASHKEY_STR>, 0x008DC710<SAVEDVARIABLE, HASHKEY_STR>, 0x00900FC0<USEABLESTYLE, HASHKEY_NONE>, 0x00917290/0x140676FB0<SPELLSBYNAME, HASHKEY_UTF8I>, 0x009CE060/0x14075EB50<WORLDSTATE, HASHKEY_NONE>, 0x009893E0/0x140705D80<LFGListParty, CHashKeyGUID>, 0x00989600/0x140706050<LFGListPlayer, CHashKeyGUID>, 0x00924A30/0x140689160<MODIFIEDCLICK, HASHKEY_STRI>, 0x009C67B0/0x140754BF0<MACRONODE, HASHKEY_NONE>, 0x009B88F0/0x1407432A0<KNOWNDANCES, HASHKEY_STRI>, 0x009E4A80/0x14077C3E0<TAXIPATHNODESCLIENT, HASHKEY_NONE>, 0x0094E5B0/0x1406BE170<ACHIEVEMENT, HASHKEY_NONE>, 0x0099DC50/0x140720F70<TalentGroupItem_C, HASHKEY_NONE>, 0x009A8880/0x14072F650<FACTION, HASHKEY_NONE>, 0x00A0F010<MacroConditions, HASHKEY_STR>, 0x00447180/0x140053480<_BANNEDADDON, HASHKEY_NONE>, 0x0045E4C0/0x140070F60<UIADDON::METADATA, HASHKEY_STRI>, 0x004844A0/0x1400A4210<PLAYERPORTRAIT, CHashKeyGUID>, 0x00848790/0x1405761A0<CGxShader, HASHKEY_STRI>, 0x006117C0/0x14029C270<CLightList, HASHKEY_DWORD>, 0x0061C4F0/0x1402AA1F0<CHARCODEDESC, HASHKEY_NONE>, 0x0061C960/0x1402AA8A0<KERNNODE, KERNINGHASHKEY>, 0x0056D8F0/0x1401CDEA0<PROFICIENCYSPELL, HASHKEY_NONE>, 0x005E3D30/0x1402628F0<OBJ_EFFECT_GROUP_DEF_REF_LIST_LOOKUP, HASHKEY_NONE>, 0x005E2FB0/0x1402618B0<OBJ_EFFECT_REF_LOOKUP, HASHKEY_NONE>, 0x005E30B0/0x1402619F0<OBJ_EFFECT_GROUP_LOOKUP, HASHKEY_NONE>, 0x005E32D0/0x140261D90<OBJ_EFFECT_REF_LIST_LOOKUP, HASHKEY_NONE>, 0x005E35B0/0x1402620B0<OBJ_EFFECT_DEF_LOOKUP, HASHKEY_NONE>, 0x005E3950/0x140262460<OBJ_EFFECT_GROUP_DEF_LOOKUP, HASHKEY_NONE>, 0x005E3E30/0x140262A30<OBJ_EFFECT_PACKAGE_DEF_LOOKUP, HASHKEY_NONE>, 0x005A80F0/0x140219D30<INVENTORYART, HASHKEY_NONE>, 0x005A8300/0x140219FC0<ITEMBYNAME, HASHKEY_UTF8I>, 0x005D5940/0x140250220<UnitThreat, CHashKeyGUID>, 0x006567B0/0x1402F7EC0<CVar, HASHKEY_STRI>, 0x00654940/0x1402F5390<CONSOLECOMMAND, HASHKEY_CONSTSTRI>, 0x0056D7F0/0x1401CDD60<TRADESKILLLINE, HASHKEY_NONE>, 0x004DE620/0x14011D270<TALKINGPLAYER, CHashKeyGUID>, 0x004DE8B0/0x14011D500<MUTEDPLAYER, CHashKeyGUID>, 0x004E5AB0/0x1401276A0<DECLINEDWORD, HASHKEY_CONSTSTRI>, 0x008B2B70/0x1405F90E0<DESTLOCSPELLCASTINDEXHASHNODE, CHashKeyGUID>, 0x008C0DC0/0x14060AEF0<RACECLASSINFO, HASHKEY_NONE>, 0x004ED530<HashedNode, HASHKEY_STRI>, 0x004ED740<FrameFactoryNode, HASHKEY_STRI>, 0x0083F0E0/0x14056A9B0<FrameScript_EventObject, HASHKEY_CONSTSTRI>, 0x00B319D0/0x1408FB650<LCDHANDLE, HASHKEY_CONSTSTRI>, 0x00B32DD0/0x1408FD290<BITMAPBYICON, HASHKEY_NONE>, 0x00434B60/0x14003DF30<CShaderEffect, HASHKEY_STRI>, 0x0071D410/0x1403F2830<Liquid::Object, HASHKEY_NONE>, 0x0064AB00/0x1402E8310<DayNight::DNOverrideSky, HASHKEY_STRI>, 0x0072EA30/0x1404089B0<CMapDoodadDef, HASHKEY_DWORD>, 0x0072EB30/0x140408AF0<CMapObjDef, HASHKEY_NONE>, 0x0077C040/0x140468690<CSceneMaterial, HASHKEY_MATERIAL>, 0x00747FB0/0x140426D90<CMapObj, HASHKEY_NONE>, 0x0073A8B0/0x140417D30<DDMaterial, HASHKEY_NONE>, 0x00547A70/0x14019F7F0<FileNotification::DeferredFile, HASHKEY_NONE>, 0x0070AB70<SE3SoundChunk, HASHKEY_NONE>, 0x0070CF80/0x1403DBAA0<SE3SOUNDKITOBJECT_LOOKUP, HASHKEY_NONE>, 0x0070C140/0x1403DB7E0<SOUNDKITLOOKUP, HASHKEY_CONSTSTRI>, 0x0070C340<SE3SoundEmitterLookup, HASHKEY_NONE>, 0x0070CE80/0x1403DB960<SE3COMBINABLESOUNDLIST, HASHKEY_NONE>, 0x00713D30/0x1403E6020<SE3ChatSound, HASHKEY_NONE>, 0x00714EB0/0x1403E78A0<SE3MemorySoundObject, HASHKEY_NONE>, 0x004FCAD0/0x140140F00<FILENAMEHASH, HASHKEY_STR>, 0x008556D0/0x140585D50<CGxDeviceD3d11::InputLayoutRec, InputLayoutKey>, 0x008558D0/0x140586060<CGxDeviceD3d11::RasterizerStateRec, HASHKEY_NONE>, 0x00855AD0/0x140586370<CGxDeviceD3d11::BlendStateRec, HASHKEY_NONE>, 0x00855CD0/0x140586680<CGxDeviceD3d11::DepthStateRec, HASHKEY_NONE>, 0x00855ED0/0x140586990<CGxDeviceD3d11::SamplerStateRec, HASHKEY_NONE>, 0x00547DB0/0x14019FCC0<FileNotification::Manifest, HASHKEY_NONE>, 0x009F2DD0/0x14078D860<CGSpellActivationOverlay::COUNTERNODE, HASHKEY_NONE>, 0x009F2FD0/0x14078DAE0<CGSpellActivationOverlay::PETAURANODE, HASHKEY_NONE>, 0x009F6CC0/0x1407931D0<CGResearchFrame::BRANCH, HASHKEY_NONE>, 0x009F6E30/0x140793310<CGResearchFrame::BLOBPOIHASH, HASHKEY_NONE>, 0x009CA140/0x140759370<CGCUFProfiles::PROFILENODE, HASHKEY_CONSTUTF8I>, 0x009CA2E0/0x140759600<CGCUFProfiles::OPTIONENTRY, HASHKEY_CONSTSTR>, 0x0043A070/0x140044C40<CCameraModeEntry, HASHKEY_NONE>, 0x004846F0/0x1400A4490<UNITPORTRAIT, HASHKEY_NONE>, 0x00484940/0x1400A4770<ITEMPORTRAIT, HASHKEY_STR>, 0x0048C6F0/0x1400B0170<WowClientDB2_Base::DBTABLE, HASHKEY_NONE>, 0x00496310/0x1400BC2C0<SPELLACTIVATIONOVERLAY, HASHKEY_NONE>, 0x00590CF0/0x1401FC2E0<CustomFallbackPath, HASHKEY_NONE>, 0x008B2960<ITEMCOOLDOWNHASHNODE, HASHKEY_NONE>, 0x008DC940/0x140650FA0<TAINTEVENT, HASHKEY_STR>, 0x008F24B0/0x1406234D0<ADDONPREFIX, HASHKEY_STR>, 0x0090A120/0x1406638A0<ACCOUNTIDCACHE, HASHKEY_DWORD>, 0x00917390/0x1406770F0<OVERRIDE_SPELL_NODE, HASHKEY_NONE>, 0x00917490/0x140677230<OVERRIDE_SPELL_BY_NAME_NODE, HASHKEY_UTF8I>, 0x0093AD90/0x1406A5880<CGGuildInfo::GuildRecipesKnown, HASHKEY_NONE>, 0x0093B190<CGGuildInfo::SKILLLINESPELLBITS, HASHKEY_NONE>, 0x0094E7E0/0x1406BE420<GUILDACHIEVEMENTMEMBERLIST, HASHKEY_NONE>, 0x00966F00/0x1406DCFC0<CGQuestLog::QUESTCOMPLETIONNPC, HASHKEY_NONE>, 0x009A4DC0/0x14072AC50<CFlyout, HASHKEY_NONE>, 0x009A5030/0x14072AF50<SpellToFlyoutMapItem, HASHKEY_NONE>, 0x009CC8B0/0x14075C970<CGAutoCompleteName::AUTOCOMPLETE_CHARACTER_HASHNODE, CHashKeyGUID>, 0x009CCAB0/0x14075CBF0<CGAutoCompleteName::AUTOCOMPLETE_BATTLENET_HASHNODE, HASHKEY_NONE>, 0x009DF090/0x140775520<CGQuestObjectiveCache::REQUIREDUNITNODE, HASHKEY_NONE>, 0x009DF290/0x1407757A0<CGQuestObjectiveCache::REQUIREDITEMNODE, HASHKEY_NONE>, 0x009FD6B0/0x14079B6D0<CGIncomingResurrection::INCOMINGRESLIST, CHashKeyGUID>, 0x004A65E0/0x1400D6C20<DBCache<CreatureStats_C, int, HASHKEY_INT>::DBCACHEHASH, HASHKEY_INT>, 0x004A6790/0x1400D6E60<DBCache<CreatureStats_C, int, HASHKEY_INT>::REVERSEENTRY, HASHKEY_CONSTSTR>, 0x004A6890/0x1400D6FA0<DBCache<GameObjectStats_C, int, HASHKEY_INT>::DBCACHEHASH, HASHKEY_INT>, 0x004A6A40/0x1400D71E0<DBCache<GameObjectStats_C, int, HASHKEY_INT>::REVERSEENTRY, HASHKEY_CONSTSTR>, 0x004A6B40/0x1400D7320<DBCache<NPCText, int, HASHKEY_INT>::DBCACHEHASH, HASHKEY_INT>, 0x004A6CF0/0x1400D75A0<DBCache<NPCText, int, HASHKEY_INT>::REVERSEENTRY, HASHKEY_CONSTSTR>, 0x004A6DF0/0x1400D76E0<DBCache<NameCache, unsigned __int64, CHashKeyGUID>::DBCACHEHASH, CHashKeyGUID>, 0x004A6FB0/0x1400D7940<DBCache<NameCache, unsigned __int64, CHashKeyGUID>::REVERSEENTRY, HASHKEY_CONSTSTR>, 0x004A70B0/0x1400D7A80<DBCache<GuildStats_C, unsigned __int64, CHashKeyGUID>::DBCACHEHASH, CHashKeyGUID>, 0x004A7250/0x1400D7CC0<DBCache<GuildStats_C, unsigned __int64, CHashKeyGUID>::REVERSEENTRY, HASHKEY_CONSTSTR>, 0x004A7350/0x1400D7E00<DBCache<QuestCache, int, HASHKEY_INT>::DBCACHEHASH, HASHKEY_INT>, 0x004A74F0/0x1400D8040<DBCache<QuestCache, int, HASHKEY_INT>::REVERSEENTRY, HASHKEY_CONSTSTR>, 0x004A75F0/0x1400D8180<DBCache<PageTextCache_C, int, HASHKEY_INT>::DBCACHEHASH, HASHKEY_INT>, 0x004A77A0/0x1400D83B0<DBCache<PageTextCache_C, int, HASHKEY_INT>::REVERSEENTRY, HASHKEY_CONSTSTR>, 0x004A78A0/0x1400D84F0<DBCache<PetNameCache, int, HASHKEY_INT>::DBCACHEHASH, HASHKEY_INT>, 0x004A7A50/0x1400D8750<DBCache<PetNameCache, int, HASHKEY_INT>::REVERSEENTRY, HASHKEY_CONSTSTR>, 0x004A7B50/0x1400D8890<DBCache<CGPetition, int, HASHKEY_INT>::DBCACHEHASH, HASHKEY_INT>, 0x004A7CF0/0x1400D8AD0<DBCache<CGPetition, int, HASHKEY_INT>::REVERSEENTRY, HASHKEY_CONSTSTR>, 0x004A7DF0/0x1400D8C10<DBCache<ItemTextCache_C, unsigned __int64, CHashKeyGUID>::DBCACHEHASH, CHashKeyGUID>, 0x004A7F90/0x1400D8E50<DBCache<ItemTextCache_C, unsigned __int64, CHashKeyGUID>::REVERSEENTRY, HASHKEY_CONSTSTR>, 0x004A8090/0x1400D8F90<DBCache<WardenCachedModule, CWardenKey, CWardenKey>::DBCACHEHASH, CWardenKey>, 0x004A8230/0x1400D91D0<DBCache<WardenCachedModule, CWardenKey, CWardenKey>::REVERSEENTRY, HASHKEY_CONSTSTR>, 0x004A8330/0x1400D9310<DBCache<ArenaTeamCache, int, HASHKEY_INT>::DBCACHEHASH, HASHKEY_INT>, 0x004A84D0/0x1400D9550<DBCache<ArenaTeamCache, int, HASHKEY_INT>::REVERSEENTRY, HASHKEY_CONSTSTR>, 0x004A85D0/0x1400D9690<DBCache<DanceCache, int, HASHKEY_INT>::DBCACHEHASH, HASHKEY_INT>, 0x004A8790/0x1400D98F0<DBCache<DanceCache, int, HASHKEY_INT>::REVERSEENTRY, HASHKEY_CONSTSTR>, 0x00595FC0/0x140203760<VoidItem_C, HASHKEY_NONE>, 0x004C2D50/0x1400FDB50<RGN, HASHKEY_NONE>, 0x0093B090/0x1406A5D90<CGGuildInfo::SKILLLINESPELLS, HASHKEY_NONE>, 0x00793980/0x140485A40<CFileName, ?>, 0x009B4F00/0x14073E4C0<EVENTDATEHASH, ?>, 0x0045E5C0/0x1400710A0<UIADDON, HASHKEY_STRI>, 0x00924C40/0x1406893F0<CLICKFRAME, HASHKEY_STR>, 0x009B5110/0x14073E750<DYNAMICHOLIDAYHASH, HASHKEY_NONE>, 0x0044B450/0x140058810<MOUSELOOKBINDING, HASHKEY_STRI>, 0x0045E1A0/0x140070B70<ADDONSTATE, HASHKEY_STRI>, 0x004D98C0/0x140117D50<TIMESTAMPSTRUCT, HASHKEY_NONE>, 0x00545AF0/0x14019CAE0<ProfileInternal::SECTION, HASHKEY_CONSTSTRI>, 0x00615840/0x1402A15C0<BATCHEDRENDERFONTDESC, HASHKEY_PTR>, 0x00622B60/0x1402B3CF0<FACEDATA, HASHKEY_STRI>, 0x009242F0/0x1406886A0<KEYCOMMAND, HASHKEY_STRI>, 0x00924490/0x140688940<KEYBINDING, HASHKEY_STRI>, 0x00924810/0x140688E80<OVERRIDEKEYBINDING, HASHKEY_STRI>, 0x0099DE60/0x140721290<TalentGroupTierItem_C, HASHKEY_NONE>]
template <class T, class TKey>
void TSHashTable<T, TKey>::Destroy() {
    this->InternalClear(1);
    this->m_slotmask = 0xFFFFFFFF;
    this->m_slotlistarray.Clear();
}

// INLINED
template <class T, class TKey>
void TSHashTable<T, TKey>::Delete(T* ptr) {
    this->Unlink(ptr);
    this->InternalDelete(ptr);
}

// OFFSET: [0x0071CEB0/0x1400C87F0<CVar, HASHKEY_STRI>]
template <class T, class TKey>
T* TSHashTable<T, TKey>::DeleteNode(T* ptr) {
    T* next = this->m_fulllist.Next(ptr);
    this->Unlink(ptr);
    this->InternalDelete(ptr);

    return next;
}

// OFFSET: [0x00653A00<CVar, HASHKEY_STRI>]
template <class T, class TKey>
T* TSHashTable<T, TKey>::Next(T* ptr) {
    return this->m_fulllist.Next(ptr);
}

// OFFSET: [0x005065A0/0x14014D730<FRAMEATTR, HASHKEY_STRI>, 0x00526570<SIMPLEFONT, HASHKEY_STRI>, 0x00544690<CTexture, HASHKEY_TEXTUREFILE>, 0x00548B80<CModelBlob::CHashEntry, HASHKEY_MODELBLOBFILE>, 0x00545C70<ProfileInternal::KEYVALUE, HASHKEY_CONSTSTRI>, 0x00545E40<ProfileInternal::SECTION, HASHKEY_CONSTSTRI>, 0x0054A020<FONTHASHOBJ, HASHKEY_STR>, 0x0054BEC0<CTextureBlobTexture, HASHKEY_TEXTUREBLOBFILE>, 0x0062F110<ZONESOUNDOVERRIDELOOKUP, HASHKEY_STRI>, 0x00633450<DSP_CHAIN_HEAD_LOOKUP, HASHKEY_STRI>, 0x006341A0<FOOTSTEPSNDCACHE, HASHKEY_NONE>, 0x004903A0/0x1400B4D80<CGObject_C, CHashKeyGUID>, 0x007048F0<CACHEENTRY, HASHKEY_NONE>, 0x008F2ED0<WORDLIST, HASHKEY_LANGUAGE>, 0x008F3160<CHATTHROTTLE, HASHKEY_STR>, 0x008F3500<PROFANITYCHECK, HASHKEY_STR>, 0x008F36D0<SPAMCHECK, HASHKEY_STR>, 0x008DCAC0/0x140651140<SAVEDVARIABLE, HASHKEY_STR>, 0x009010B0<USEABLESTYLE, HASHKEY_NONE>, 0x00917870<SPELLSBYNAME, HASHKEY_UTF8I>, 0x009CE150<WORLDSTATE, HASHKEY_NONE>, 0x00989780<LFGListParty, CHashKeyGUID>, 0x00989950<LFGListPlayer, CHashKeyGUID>, 0x00924F70<KEYCOMMAND, HASHKEY_STRI>, 0x00925140/0x140689B90<KEYBINDING, HASHKEY_STRI>, 0x00925310/0x140689F20<OVERRIDEKEYBINDING, HASHKEY_STRI>, 0x009254E0/0x14068A2B0<MODIFIEDCLICK, HASHKEY_STRI>, 0x009256B0<CLICKFRAME, HASHKEY_STR>, 0x009C69B0<MACRONODE, HASHKEY_NONE>, 0x009E4C40<TAXIPATHNODESCLIENT, HASHKEY_NONE>, 0x0094E960<ACHIEVEMENT, HASHKEY_NONE>, 0x009B5460<DYNAMICHOLIDAYHASH, HASHKEY_NONE>, 0x0099DFE0<TalentGroupItem_C, HASHKEY_NONE>, 0x0099E1B0<TalentGroupTierItem_C, HASHKEY_NONE>, 0x009A8970<FACTION, HASHKEY_NONE>, 0x00A0F100<MacroConditions, HASHKEY_STR>, 0x00447270<_BANNEDADDON, HASHKEY_NONE>, 0x0045E7D0/0x140071310<UIADDON::METADATA, HASHKEY_STRI>, 0x0045E9A0<ADDONSTATE, HASHKEY_STRI>, 0x0045EB70<UIADDON, HASHKEY_STRI>, 0x0044B540/0x140058960<MOUSELOOKBINDING, HASHKEY_STRI>, 0x00484AF0<PLAYERPORTRAIT, CHashKeyGUID>, 0x00849570/0x140577470<CGxShader, HASHKEY_STRI>, 0x006118B0<CLightList, HASHKEY_DWORD>, 0x0061CAB0<CHARCODEDESC, HASHKEY_NONE>, 0x0061CC80<KERNNODE, KERNINGHASHKEY>, 0x00622C50<FACEDATA, HASHKEY_STRI>, 0x0056F630<PROFICIENCYSPELL, HASHKEY_NONE>, 0x005E4280<OBJ_EFFECT_GROUP_DEF_REF_LIST_LOOKUP, HASHKEY_NONE>, 0x005E4450<OBJ_EFFECT_REF_LOOKUP, HASHKEY_NONE>, 0x005E4620<OBJ_EFFECT_GROUP_LOOKUP, HASHKEY_NONE>, 0x005E47F0<OBJ_EFFECT_REF_LIST_LOOKUP, HASHKEY_NONE>, 0x005E49C0<OBJ_EFFECT_DEF_LOOKUP, HASHKEY_NONE>, 0x005E4B90<OBJ_EFFECT_GROUP_DEF_LOOKUP, HASHKEY_NONE>, 0x005E4D60<OBJ_EFFECT_PACKAGE_DEF_LOOKUP, HASHKEY_NONE>, 0x005A8420<INVENTORYART, HASHKEY_NONE>, 0x005A8650<ITEMBYNAME, HASHKEY_UTF8I>, 0x005D7710<UnitThreat, CHashKeyGUID>, 0x006568A0<CVar, HASHKEY_STRI>, 0x00654A30<CONSOLECOMMAND, HASHKEY_CONSTSTRI>, 0x00A31E90<TIMESTAMPSTRUCT, HASHKEY_NONE>, 0x004C2F90<RGN, HASHKEY_NONE>, 0x004DEA30<TALKINGPLAYER, CHashKeyGUID>, 0x004DEC00<MUTEDPLAYER, CHashKeyGUID>, 0x004E5BA0<DECLINEDWORD, HASHKEY_CONSTSTRI>, 0x008B2CF0<ITEMCOOLDOWNHASHNODE, HASHKEY_NONE>, 0x008B2EC0<DESTLOCSPELLCASTINDEXHASHNODE, CHashKeyGUID>, 0x008C0EB0<RACECLASSINFO, HASHKEY_NONE>, 0x004ED8C0<HashedNode, HASHKEY_STRI>, 0x004EDA90<FrameFactoryNode, HASHKEY_STRI>, 0x0083F1D0<FrameScript_EventObject, HASHKEY_CONSTSTRI>, 0x00B32F20<BITMAPBYICON, HASHKEY_NONE>, 0x0087E5B0<OsNet::NETSELSOCKPTR, HASHKEY_NONE>, 0x00434C50<CShaderEffect, HASHKEY_STRI>, 0x0071D500<Liquid::Object, HASHKEY_NONE>, 0x0064F220<DayNight::DNOverrideSky, HASHKEY_STRI>, 0x0072EFB0<CMapDoodadDef, HASHKEY_DWORD>, 0x0072F180<CMapObjDef, HASHKEY_NONE>, 0x0077C130<CSceneMaterial, HASHKEY_MATERIAL>, 0x007480A0<CMapObj, HASHKEY_NONE>, 0x0073AA50<DDMaterial, HASHKEY_NONE>, 0x00621DF0<BATCHEDRENDERFONTDESC, HASHKEY_PTR>, 0x004AA350<DBCache<CreatureStats_C, int, HASHKEY_INT>::DBCACHEHASH, HASHKEY_INT>, 0x004AA6F0<DBCache<NPCText, int, HASHKEY_INT>::DBCACHEHASH, HASHKEY_INT>, 0x004AA8C0<DBCache<NameCache, unsigned __int64, CHashKeyGUID>::DBCACHEHASH, CHashKeyGUID>, 0x004AAA90<DBCache<GuildStats_C, int, HASHKEY_INT>::DBCACHEHASH, HASHKEY_INT>, 0x004AAC60<DBCache<QuestCache, int, HASHKEY_INT>::DBCACHEHASH, HASHKEY_INT>, 0x004AAE30<DBCache<PageTextCache_C, int, HASHKEY_INT>::DBCACHEHASH, HASHKEY_INT>, 0x004AB000<DBCache<PetNameCache, int, HASHKEY_INT>::DBCACHEHASH, HASHKEY_INT>, 0x004AB1D0<DBCache<CGPetition, int, HASHKEY_INT>::DBCACHEHASH, HASHKEY_INT>, 0x004AB3A0<DBCache<ItemTextCache_C, unsigned __int64, CHashKeyGUID>::DBCACHEHASH, CHashKeyGUID>, 0x004AB740<DBCache<ArenaTeamCache, int, HASHKEY_INT>::DBCACHEHASH, HASHKEY_INT>, 0x004AB910<DBCache<DanceCache, int, HASHKEY_INT>::DBCACHEHASH, HASHKEY_INT>, 0x0070C870<SE3SoundChunk, HASHKEY_NONE>, 0x0070D480<SE3SOUNDKITOBJECT_LOOKUP, HASHKEY_NONE>, 0x0070CC10<SOUNDKITLOOKUP, HASHKEY_CONSTSTRI>, 0x0070CA40<SE3SoundEmitterLookup, HASHKEY_NONE>, 0x0070D2B0<SE3COMBINABLESOUNDLIST, HASHKEY_NONE>, 0x00713E20<SE3ChatSound, HASHKEY_NONE>, 0x00714FA0<SE3MemorySoundObject, HASHKEY_NONE>, 0x004FCC90<FILENAMEHASH, HASHKEY_STR>, 0x0086E6C0/0x1405A34D0<CGxDeviceD3d11::InputLayoutRec, InputLayoutKey>, 0x0086E890<CGxDeviceD3d11::RasterizerStateRec, HASHKEY_NONE>, 0x0086EA60<CGxDeviceD3d11::BlendStateRec, HASHKEY_NONE>, 0x0086EC30<CGxDeviceD3d11::DepthStateRec, HASHKEY_NONE>, 0x0086EE00<CGxDeviceD3d11::SamplerStateRec, HASHKEY_NONE>, 0x00547ED0<FileNotification::DeferredFile, HASHKEY_NONE>, 0x009F3150<CGSpellActivationOverlay::COUNTERNODE, HASHKEY_NONE>, 0x009F3320<CGSpellActivationOverlay::PETAURANODE, HASHKEY_NONE>, 0x009F6FB0<CGResearchFrame::BRANCH, HASHKEY_NONE>, 0x009F7180<CGResearchFrame::BLOBPOIHASH, HASHKEY_NONE>, 0x009CA460<CGCUFProfiles::PROFILENODE, HASHKEY_CONSTUTF8I>, 0x009CA630/0x140759BB0<CGCUFProfiles::OPTIONENTRY, HASHKEY_CONSTSTR>, 0x008DCC90<TAINTEVENT, HASHKEY_STR>, 0x008F3330<ADDONPREFIX, HASHKEY_STR>, 0x00917640<OVERRIDE_SPELL_NODE, HASHKEY_NONE>, 0x0093B3A0<CGGuildInfo::GuildRecipesKnown, HASHKEY_NONE>, 0x0093B740<CGGuildInfo::SKILLLINESPELLS, HASHKEY_NONE>, 0x0094EB30<GUILDACHIEVEMENTMEMBERLIST, HASHKEY_NONE>, 0x00966FF0<CGQuestLog::QUESTCOMPLETIONNPC, HASHKEY_NONE>, 0x009A51B0<CFlyout, HASHKEY_NONE>, 0x009A5380<SpellToFlyoutMapItem, HASHKEY_NONE>, 0x009B89E0<KNOWNDANCES, HASHKEY_STRI>, 0x009FD7A0<CGIncomingResurrection::INCOMINGRESLIST, CHashKeyGUID>, 0x0043A160<CCameraModeEntry, HASHKEY_NONE>, 0x00484CC0<UNITPORTRAIT, HASHKEY_NONE>, 0x00484EF0<ITEMPORTRAIT, HASHKEY_STR>, 0x00496400<SPELLACTIVATIONOVERLAY, HASHKEY_NONE>, 0x004A8D30/0x1400DA210<DBCache<NameCache, unsigned __int64, CHashKeyGUID>::REVERSEENTRY, HASHKEY_NONE>, 0x0048C7E0<WowClientDB2_Base::DBTABLE, HASHKEY_NONE>, 0x00590DE0<CustomFallbackPath, HASHKEY_NONE>, 0x0090A210<ACCOUNTIDCACHE, HASHKEY_DWORD>, 0x0093B570<CGGuildInfo::SKILLLINESPELLBITS, HASHKEY_NONE>, 0x009B5290<CALENDAREVENTHASH, HASHKEY_NONE>, 0x009CCC30<CGAutoCompleteName::AUTOCOMPLETE_CHARACTER_HASHNODE, CHashKeyGUID>, 0x009CCE00<CGAutoCompleteName::AUTOCOMPLETE_BATTLENET_HASHNODE, HASHKEY_NONE>, 0x009DF410<CGQuestObjectiveCache::REQUIREDUNITNODE, HASHKEY_NONE>, 0x009DF5E0<CGQuestObjectiveCache::REQUIREDITEMNODE, HASHKEY_NONE>, 0x0056F460<TRADESKILLLINE, HASHKEY_NONE>, 0x00793A70<`CMapObjGroup::GetHitTestDebugTextures'::`2'::TEXTURENAME, HASHKEY_CONSTSTRI>, 0x004AA520<DBCache<GameObjectStats_C, int, HASHKEY_INT>::DBCACHEHASH, HASHKEY_INT>, 0x005960B0<VoidItem_C, HASHKEY_NONE>, 0x00917A40<OVERRIDE_SPELL_BY_NAME_NODE, HASHKEY_UTF8I>, 0x004AB570<DBCache<WardenCachedModule, CWardenKey, CWardenKey>::DBCACHEHASH, CWardenKey>, 0x00B330F0<LCDHANDLE, HASHKEY_CONSTSTRI>]
template <class T, class TKey>
void TSHashTable<T, TKey>::GrowListArray(uint32_t newarraysize) {
    uint32_t oldarraysize = this->m_slotmask + 1;

    TSExplicitList<T> templist;
    templist.ChangeLinkOffset(offsetof(T, m_linktoslot));

    for (uint32_t i = 0; i < oldarraysize; i++) {
        T* node;

        while ((node = this->m_slotlistarray[i].Head()) != NULL) {
            templist.LinkToTail(node);
        }
    }

    this->m_slotlistarray.SetCount(newarraysize);

    for (uint32_t i = 0; i < newarraysize; i++) {
        this->m_slotlistarray[i].ChangeLinkOffset(offsetof(T, m_linktoslot));
    }

    this->m_slotmask = newarraysize - 1;

    T* node;

    while ((node = templist.Head()) != NULL) {
        this->m_slotlistarray[node->m_hashval & this->m_slotmask].LinkToTail(node);
    }
}

// INLINED
template <class T, class TKey>
T* TSHashTable<T, TKey>::Head() {
    return this->m_fulllist.Head();
}

// OFFSET: [0x00506540/0x14014D6B0<FRAMEATTR, HASHKEY_STRI>, 0x00526510/0x140175500<SIMPLEFONT, HASHKEY_STRI>, 0x00544630/0x1402527A0<CTexture, HASHKEY_TEXTUREFILE>, 0x00548B20/0x1401A1100<CModelBlob::CHashEntry, HASHKEY_MODELBLOBFILE>, 0x00545BB0/0x14019CC00<ProfileInternal::KEYVALUE, HASHKEY_CONSTSTRI>, 0x00545C10/0x14019CC80<ProfileInternal::SECTION, HASHKEY_CONSTSTRI>, 0x00549FC0<FONTHASHOBJ, HASHKEY_STR>, 0x0054BE60<CTextureBlobTexture, HASHKEY_TEXTUREBLOBFILE>, 0x00490340/0x1400B4D00<CGObject_C, CHashKeyGUID>, 0x00704890<CACHEENTRY, HASHKEY_NONE>, 0x008F2E10/0x1406240F0<CHATTHROTTLE, HASHKEY_STR>, 0x008F30A0<PROFANITYCHECK, HASHKEY_STR>, 0x008F3100<SPAMCHECK, HASHKEY_STR>, 0x008DCA00/0x1406510C0<SAVEDVARIABLE, HASHKEY_STR>, 0x00901050/0x1406591C0<USEABLESTYLE, HASHKEY_NONE>, 0x009896C0<LFGListParty, CHashKeyGUID>, 0x00989720<LFGListPlayer, CHashKeyGUID>, 0x00924D90/0x140689600<KEYCOMMAND, HASHKEY_STRI>, 0x00924DF0/0x140689680<KEYBINDING, HASHKEY_STRI>, 0x00924E50/0x140689700<OVERRIDEKEYBINDING, HASHKEY_STRI>, 0x00924EB0/0x140689780<MODIFIEDCLICK, HASHKEY_STRI>, 0x009E4BE0<TAXIPATHNODESCLIENT, HASHKEY_NONE>, 0x0099DF20/0x1407213B0<TalentGroupItem_C, HASHKEY_NONE>, 0x0099DF80/0x140721430<TalentGroupTierItem_C, HASHKEY_NONE>, 0x00A0F0A0<MacroConditions, HASHKEY_STR>, 0x00447210<_BANNEDADDON, HASHKEY_NONE>, 0x0045E6B0/0x140071210<UIADDON::METADATA, HASHKEY_STRI>, 0x0044B4E0/0x1400588E0<MOUSELOOKBINDING, HASHKEY_STRI>, 0x00484A30<PLAYERPORTRAIT, CHashKeyGUID>, 0x00849510/0x1405773F0<CGxShader, HASHKEY_STRI>, 0x00611850<CLightList, HASHKEY_DWORD>, 0x0061C9F0<CHARCODEDESC, HASHKEY_NONE>, 0x0061CA50/0x1402AA9F0<KERNNODE, KERNINGHASHKEY>, 0x00622BF0<FACEDATA, HASHKEY_STRI>, 0x0056F400<PROFICIENCYSPELL, HASHKEY_NONE>, 0x005E3FE0<OBJ_EFFECT_GROUP_DEF_REF_LIST_LOOKUP, HASHKEY_NONE>, 0x005E4040/0x140262DF0<OBJ_EFFECT_REF_LOOKUP, HASHKEY_NONE>, 0x005E40A0<OBJ_EFFECT_GROUP_LOOKUP, HASHKEY_NONE>, 0x005E4100<OBJ_EFFECT_REF_LIST_LOOKUP, HASHKEY_NONE>, 0x005D76B0/0x1402527A0<UnitThreat, CHashKeyGUID>, 0x00656840<CVar, HASHKEY_STRI>, 0x006549D0/0x1402F5460<CONSOLECOMMAND, HASHKEY_CONSTSTRI>, 0x00A31E30/0x1407DBE00<TIMESTAMPSTRUCT, HASHKEY_NONE>, 0x004C2F30<RGN, HASHKEY_NONE>, 0x008C0E50<RACECLASSINFO, HASHKEY_NONE>, 0x004ED800<HashedNode, HASHKEY_STRI>, 0x004ED860<FrameFactoryNode, HASHKEY_STRI>, 0x0083F170<FrameScript_EventObject, HASHKEY_CONSTSTRI>, 0x00B32E60/0x1408FD360<BITMAPBYICON, HASHKEY_NONE>, 0x0087E550/0x1405B72A0<OsNet::NETSELSOCKPTR, HASHKEY_NONE>, 0x0071D4A0<Liquid::Object, HASHKEY_NONE>, 0x0064F1C0/0x1402EE340<DayNight::DNOverrideSky, HASHKEY_STRI>, 0x0072EEF0/0x140409040<CMapDoodadDef, HASHKEY_DWORD>, 0x0072EF50/0x1404090C0<CMapObjDef, HASHKEY_NONE>, 0x0077C0D0/0x140468760<CSceneMaterial, HASHKEY_MATERIAL>, 0x00748040/0x140426E60<CMapObj, HASHKEY_NONE>, 0x0073A9F0<DDMaterial, HASHKEY_NONE>, 0x004A9FF0/0x1400DBD90<DBCache<GuildStats_C, int, HASHKEY_INT>::DBCACHEHASH, HASHKEY_INT>, 0x004FCC30/0x1401410F0<FILENAMEHASH, HASHKEY_STR>, 0x00713DC0/0x1403E60F0<SE3ChatSound, HASHKEY_NONE>, 0x00714F40/0x1403E7970<SE3MemorySoundObject, HASHKEY_NONE>, 0x00621D90/0x1402B28F0<BATCHEDRENDERFONTDESC, HASHKEY_PTR>, 0x004A9F30/0x1400DBC90<DBCache<NPCText, int, HASHKEY_INT>::DBCACHEHASH, HASHKEY_INT>, 0x004A9F90/0x1400DBD10<DBCache<NameCache, unsigned __int64, CHashKeyGUID>::DBCACHEHASH, CHashKeyGUID>, 0x004AA050/0x1400DBE10<DBCache<QuestCache, int, HASHKEY_INT>::DBCACHEHASH, HASHKEY_INT>, 0x004AA0B0/0x1400DBE90<DBCache<PageTextCache_C, int, HASHKEY_INT>::DBCACHEHASH, HASHKEY_INT>, 0x004AA110/0x1400DBF10<DBCache<PetNameCache, int, HASHKEY_INT>::DBCACHEHASH, HASHKEY_INT>, 0x004AA170/0x1400DBF90<DBCache<CGPetition, int, HASHKEY_INT>::DBCACHEHASH, HASHKEY_INT>, 0x004AA1D0/0x1400DC010<DBCache<ItemTextCache_C, unsigned __int64, CHashKeyGUID>::DBCACHEHASH, CHashKeyGUID>, 0x004AA290/0x1400DC110<DBCache<ArenaTeamCache, int, HASHKEY_INT>::DBCACHEHASH, HASHKEY_INT>, 0x004AA2F0/0x1400DC190<DBCache<DanceCache, int, HASHKEY_INT>::DBCACHEHASH, HASHKEY_INT>, 0x0070C530<SE3SoundChunk, HASHKEY_NONE>, 0x0070D130/0x1403DBF60<SE3SOUNDKITOBJECT_LOOKUP, HASHKEY_NONE>, 0x0070C810<SOUNDKITLOOKUP, HASHKEY_CONSTSTRI>, 0x0070C590<SE3SoundEmitterLookup, HASHKEY_NONE>, 0x0070D0D0/0x1403DBEE0<SE3COMBINABLESOUNDLIST, HASHKEY_NONE>, 0x0086E4E0/0x1405A3250<CGxDeviceD3d11::InputLayoutRec, InputLayoutKey>, 0x0086E540/0x1405A32D0<CGxDeviceD3d11::RasterizerStateRec, HASHKEY_NONE>, 0x0086E5A0/0x1405A3350<CGxDeviceD3d11::BlendStateRec, HASHKEY_NONE>, 0x0086E600/0x1405A33D0<CGxDeviceD3d11::DepthStateRec, HASHKEY_NONE>, 0x0086E660/0x1405A3450<CGxDeviceD3d11::SamplerStateRec, HASHKEY_NONE>, 0x00547E70<FileNotification::DeferredFile, HASHKEY_NONE>, 0x009F3090<CGSpellActivationOverlay::COUNTERNODE, HASHKEY_NONE>, 0x009F30F0<CGSpellActivationOverlay::PETAURANODE, HASHKEY_NONE>, 0x009F6EF0<CGResearchFrame::BRANCH, HASHKEY_NONE>, 0x009F6F50/0x1407934B0<CGResearchFrame::BLOBPOIHASH, HASHKEY_NONE>, 0x009CA3A0/0x140759720<CGCUFProfiles::PROFILENODE, HASHKEY_CONSTUTF8I>, 0x009CA400/0x1407597A0<CGCUFProfiles::OPTIONENTRY, HASHKEY_CONSTSTR>, 0x0043A100<CCameraModeEntry, HASHKEY_NONE>, 0x00484A90/0x1400A3B20<UNITPORTRAIT, HASHKEY_NONE>, 0x00484E90<ITEMPORTRAIT, HASHKEY_STR>, 0x0048C780/0x1400B0240<WowClientDB2_Base::DBTABLE, HASHKEY_NONE>, 0x004963A0<SPELLACTIVATIONOVERLAY, HASHKEY_NONE>, 0x004DE970<TALKINGPLAYER, CHashKeyGUID>, 0x004DE9D0<MUTEDPLAYER, CHashKeyGUID>, 0x004E5B40<DECLINEDWORD, HASHKEY_CONSTSTRI>, 0x00590D80<CustomFallbackPath, HASHKEY_NONE>, 0x005A83C0<INVENTORYART, HASHKEY_NONE>, 0x005A85F0<ITEMBYNAME, HASHKEY_UTF8I>, 0x005E4160<OBJ_EFFECT_DEF_LOOKUP, HASHKEY_NONE>, 0x005E41C0<OBJ_EFFECT_GROUP_DEF_LOOKUP, HASHKEY_NONE>, 0x005E4220<OBJ_EFFECT_PACKAGE_DEF_LOOKUP, HASHKEY_NONE>, 0x0062F0B0<ZONESOUNDOVERRIDELOOKUP, HASHKEY_STRI>, 0x006333F0<DSP_CHAIN_HEAD_LOOKUP, HASHKEY_STRI>, 0x00634140<FOOTSTEPSNDCACHE, HASHKEY_NONE>, 0x008B2C30<ITEMCOOLDOWNHASHNODE, HASHKEY_NONE>, 0x008B2C90<DESTLOCSPELLCASTINDEXHASHNODE, CHashKeyGUID>, 0x00917580/0x1406773A0<OVERRIDE_SPELL_NODE, HASHKEY_NONE>, 0x009175E0/0x140677420<SPELLSBYNAME, HASHKEY_UTF8I>, 0x0093B280/0x1406A5F00<CGGuildInfo::GuildRecipesKnown, HASHKEY_NONE>, 0x0093B340/0x1406A6000<CGGuildInfo::SKILLLINESPELLS, HASHKEY_NONE>, 0x00966F90<CGQuestLog::QUESTCOMPLETIONNPC, HASHKEY_NONE>, 0x009A50F0<CFlyout, HASHKEY_NONE>, 0x009A5150<SpellToFlyoutMapItem, HASHKEY_NONE>, 0x009A8910/0x14072F720<FACTION, HASHKEY_NONE>, 0x009FD740/0x14079B7A0<CGIncomingResurrection::INCOMINGRESLIST, CHashKeyGUID>, 0x0045E770<UIADDON, HASHKEY_STRI>, 0x008DCA60<TAINTEVENT, HASHKEY_STR>, 0x008F2DB0<WORDLIST, HASHKEY_LANGUAGE>, 0x00924F10<CLICKFRAME, HASHKEY_STR>, 0x0094E8A0<ACHIEVEMENT, HASHKEY_NONE>, 0x009B5230<DYNAMICHOLIDAYHASH, HASHKEY_NONE>, 0x009B8980<KNOWNDANCES, HASHKEY_STRI>, 0x009CE0F0/0x14075EC20<WORLDSTATE, HASHKEY_NONE>, 0x008F2E70/0x140624170<ADDONPREFIX, HASHKEY_STR>, 0x0094E900/0x1406BE540<GUILDACHIEVEMENTMEMBERLIST, HASHKEY_NONE>, 0x00793A10<`CMapObjGroup::GetHitTestDebugTextures'::`2'::TEXTURENAME, HASHKEY_CONSTSTRI>, 0x0090A1B0<ACCOUNTIDCACHE, HASHKEY_DWORD>, 0x009B51D0/0x14073E870<CALENDAREVENTHASH, HASHKEY_NONE>, 0x009DF350/0x1407758C0<CGQuestObjectiveCache::REQUIREDUNITNODE, HASHKEY_NONE>, 0x009DF3B0<CGQuestObjectiveCache::REQUIREDITEMNODE, HASHKEY_NONE>, 0x004A8CD0/0x1400DA190<DBCache<NameCache, unsigned __int64, CHashKeyGUID>::REVERSEENTRY, HASHKEY_CONSTSTR>, 0x004A9E70/0x1400DBB90<DBCache<CreatureStats_C, int, HASHKEY_INT>::DBCACHEHASH, HASHKEY_INT>, 0x009CCB70/0x14075CD10<CGAutoCompleteName::AUTOCOMPLETE_CHARACTER_HASHNODE, CHashKeyGUID>, 0x009CCBD0/0x14075CD90<CGAutoCompleteName::AUTOCOMPLETE_BATTLENET_HASHNODE, HASHKEY_NONE>, 0x0093B2E0/0x1406A5F80<CGGuildInfo::SKILLLINESPELLBITS, HASHKEY_NONE>, 0x0056F3A0<TRADESKILLLINE, HASHKEY_NONE>, 0x004A9ED0/0x1400DBC10<DBCache<GameObjectStats_C, int, HASHKEY_INT>::DBCACHEHASH, HASHKEY_INT>, 0x00596050/0x140203830<VoidItem_C, HASHKEY_NONE>, 0x00917810<OVERRIDE_SPELL_BY_NAME_NODE, HASHKEY_UTF8I>, 0x004AA230/0x1400DC090<DBCache<WardenCachedModule, CWardenKey, CWardenKey>::DBCACHEHASH, CWardenKey>, 0x00434BF0/0x14003E000<CShaderEffect, HASHKEY_STRI>, 0x0045E710/0x140071290<ADDONSTATE, HASHKEY_STRI>, 0x009C6950/0x140754E60<MACRONODE, HASHKEY_NONE>, 0x00B32EC0/0x1408FD3E0<LCDHANDLE, HASHKEY_CONSTSTRI>]
template <class T, class TKey>
void TSHashTable<T, TKey>::Initialize() {
    this->m_slotmask = 3;
    this->m_slotlistarray.SetCount(4);

    for (uint32_t i = 0; i <= this->m_slotmask; i++) {
        this->m_slotlistarray[i].ChangeLinkOffset(offsetof(T, m_linktoslot));
    }
}

// OFFSET: [0x009F7860<CGResearchFrame::BRANCH, HASHKEY_NONE>]
template <class T, class TKey>
void TSHashTable<T, TKey>::Insert(T* ptr, uint32_t hashval, const TKey& key) {
    this->InternalLinkNode(ptr, hashval);
    ptr->m_key = key;
}

// OFFSET: 0x00622EB0
template <class T, class TKey>
void TSHashTable<T, TKey>::Insert(T* ptr, uint32_t hashval, const char* str) {
    this->InternalLinkNode(ptr, hashval);
    ptr->m_key = str;
}

// OFFSET: 0x00622F40/0x1402B42E0
template <class T, class TKey>
void TSHashTable<T, TKey>::Insert(T* ptr, const char* str) {
    this->Insert(ptr, SStrHashHT(str), str);
}

// OFFSET: [0x007361E0/0x140412850<CMapDynamicTexture, HASHKEY_DWORD>, 0x004EC970/0x1400CDF90<CGSpellActivationOverlay::COUNTERNODE, HASHKEY_NONE>, 0x004EC970/0x1400CDF90<CGSpellActivationOverlay::PETAURANODE, HASHKEY_NONE>, 0x004EC970/0x1400CDF90<CGResearchFrame::BRANCH, HASHKEY_NONE>, 0x004EC970/0x1400CDF90<CGResearchFrame::BLOBPOIHASH, HASHKEY_NONE>, 0x004EC970/0x1400CDF90<CGCUFProfiles::PROFILENODE, HASHKEY_CONSTUTF8I>]
template <class T, class TKey>
void TSHashTable<T, TKey>::InternalClear(int32_t warn) {
    this->m_fullnessIndicator = 0;
    this->m_fulllist.UnlinkAll();

    if (warn) {
        for (uint32_t i = 0; i < this->m_slotlistarray.Count(); i++) {
            this->m_slotlistarray[i].UnlinkAll();
        }
    } else {
        for (uint32_t i = 0; i < this->m_slotlistarray.Count(); i++) {
            T* node;

            while ((node = this->m_slotlistarray[i].Head()) != NULL) {
                this->InternalDelete(node);
            }
        }
    }
}

// OFFSET: [0x00596370/0x140203CD0<VoidItem_C, HASHKEY_NONE>, 0x00490600/0x1400B51A0<CGObject_C, CHashKeyGUID>, 0x00544AD0<CTexture, HASHKEY_TEXTUREFILE>, 0x005DB110/0x140256930<UnitThreat, CHashKeyGUID>, 0x00704B50<CACHEENTRY, HASHKEY_NONE>, 0x0072F580/0x140409B20<CMapDoodadDef, HASHKEY_DWORD>, 0x0072F5F0/0x140409BA0<CMapObjDef, HASHKEY_NONE>, 0x00748300<CMapObj, HASHKEY_NONE>]
template <class T, class TKey>
void TSHashTable<T, TKey>::InternalLinkNode(T* ptr, uint32_t hashval) {
    if (this->m_slotmask == 0xFFFFFFFF) {
        this->Initialize();
    }

    uint32_t slot = hashval & this->m_slotmask;

    if (this->MonitorFullness(slot)) {
        slot = hashval & this->m_slotmask;
    }

    this->m_slotlistarray[slot].LinkToTail(ptr);
    this->m_fulllist.LinkToTail(ptr);
    ptr->m_hashval = hashval;
}

// OFFSET: [0x008F4CC0<PROFANITYCHECK, HASHKEY_STR>, 0x00925DF0/0x14068AF70<KEYCOMMAND, HASHKEY_STRI>, 0x0044B8A0/0x140058F00<MOUSELOOKBINDING, HASHKEY_STRI>, 0x00485270<PLAYERPORTRAIT, CHashKeyGUID>, 0x004AC710/0x1400DFE50<DBCache<GuildStats_C, int, HASHKEY_INT>::DBCACHEHASH, HASHKEY_INT>, 0x008497D0/0x140577890<CGxShader, HASHKEY_STRI>, 0x0056F920<TRADESKILLLINE, HASHKEY_NONE>, 0x0056F980<PROFICIENCYSPELL, HASHKEY_NONE>, 0x00656B00<CVar, HASHKEY_STRI>, 0x0086F2A0/0x1405A4970<CGxDeviceD3d11::InputLayoutRec, InputLayoutKey>, 0x0086F300/0x1405A4A00<CGxDeviceD3d11::RasterizerStateRec, HASHKEY_NONE>, 0x0086F360/0x1405A4A90<CGxDeviceD3d11::BlendStateRec, HASHKEY_NONE>, 0x0086F3C0/0x1405A4B20<CGxDeviceD3d11::DepthStateRec, HASHKEY_NONE>, 0x0086F420/0x1405A4BB0<CGxDeviceD3d11::SamplerStateRec, HASHKEY_NONE>, 0x00596310/0x140203D50<RGN, HASHKEY_NONE>, 0x009CA920/0x140759FD0<CGCUFProfiles::OPTIONENTRY, HASHKEY_CONSTSTR>, 0x0093BD40/0x1406A7100<CGGuildInfo::SKILLLINESPELLBITS, HASHKEY_NONE>, 0x008F4D20<SPAMCHECK, HASHKEY_STR>, 0x00918160<OVERRIDE_SPELL_BY_NAME_NODE, HASHKEY_UTF8I>, 0x004AC950/0x1400E01B0<DBCache<WardenCachedModule, CWardenKey, CWardenKey>::DBCACHEHASH, CWardenKey>, 0x0045F510/0x1400728B0<UIADDON::METADATA, HASHKEY_STRI>, 0x004AC530/0x1400DFB80<DBCache<NameCache, unsigned __int64, CHashKeyGUID>::REVERSEENTRY, HASHKEY_CONSTSTR>, 0x004AC590/0x1400DFC10<DBCache<CreatureStats_C, int, HASHKEY_INT>::DBCACHEHASH, HASHKEY_INT>, 0x004AC5F0/0x1400DFCA0<DBCache<GameObjectStats_C, int, HASHKEY_INT>::DBCACHEHASH, HASHKEY_INT>, 0x004AC650/0x1400DFD30<DBCache<NPCText, int, HASHKEY_INT>::DBCACHEHASH, HASHKEY_INT>, 0x004AC6B0/0x1400DFDC0<DBCache<NameCache, unsigned __int64, CHashKeyGUID>::DBCACHEHASH, CHashKeyGUID>, 0x004AC770/0x1400DFEE0<DBCache<QuestCache, int, HASHKEY_INT>::DBCACHEHASH, HASHKEY_INT>, 0x004AC7D0/0x1400DFF70<DBCache<PageTextCache_C, int, HASHKEY_INT>::DBCACHEHASH, HASHKEY_INT>, 0x004AC830/0x1400E0000<DBCache<PetNameCache, int, HASHKEY_INT>::DBCACHEHASH, HASHKEY_INT>, 0x004AC890/0x1400E0090<DBCache<CGPetition, int, HASHKEY_INT>::DBCACHEHASH, HASHKEY_INT>, 0x004AC8F0/0x1400E0120<DBCache<ItemTextCache_C, unsigned __int64, CHashKeyGUID>::DBCACHEHASH, CHashKeyGUID>, 0x004AC9B0/0x1400E0240<DBCache<ArenaTeamCache, int, HASHKEY_INT>::DBCACHEHASH, HASHKEY_INT>, 0x004ACA10/0x1400E02D0<DBCache<DanceCache, int, HASHKEY_INT>::DBCACHEHASH, HASHKEY_INT>, 0x00506830/0x14014DBA0<FRAMEATTR, HASHKEY_STRI>, 0x00546360/0x14019D8C0<ProfileInternal::KEYVALUE, HASHKEY_CONSTSTRI>, 0x005463C0/0x14019D950<ProfileInternal::SECTION, HASHKEY_CONSTSTRI>, 0x005E54F0<OBJ_EFFECT_GROUP_LOOKUP, HASHKEY_NONE>, 0x0061D340/0x1402AB870<KERNNODE, KERNINGHASHKEY>, 0x00622050/0x1402B2D90<BATCHEDRENDERFONTDESC, HASHKEY_PTR>, 0x005E5550<OBJ_EFFECT_REF_LIST_LOOKUP, HASHKEY_NONE>, 0x0087E810/0x1405B7740<OsNet::NETSELSOCKPTR, HASHKEY_NONE>, 0x008DD7A0/0x140652140<SAVEDVARIABLE, HASHKEY_STR>, 0x00925E50/0x14068B000<KEYBINDING, HASHKEY_STRI>, 0x00925EB0/0x14068B090<OVERRIDEKEYBINDING, HASHKEY_STRI>, 0x00925F10/0x14068B120<MODIFIEDCLICK, HASHKEY_STRI>, 0x0099E5B0/0x140721E30<TalentGroupItem_C, HASHKEY_NONE>, 0x0099E740/0x140722030<TalentGroupTierItem_C, HASHKEY_NONE>, 0x00A32140/0x1407DC300<TIMESTAMPSTRUCT, HASHKEY_NONE>]
template <class T, class TKey>
T* TSHashTable<T, TKey>::InternalNewNode(uint32_t hashval, size_t extrabytes, uint32_t flags) {
    if (this->m_slotmask == 0xFFFFFFFF) {
        this->Initialize();
    }

    uint32_t slot = hashval & this->m_slotmask;

    if (this->MonitorFullness(slot)) {
        slot = hashval & this->m_slotmask;
    }

    T* node = this->InternalNew(&this->m_slotlistarray[slot], extrabytes, flags);
    this->m_fulllist.LinkToTail(node);

    return node;
}

// OFFSET: [0x005067A0/0x14014DB10<FRAMEATTR, HASHKEY_STRI>, 0x00526760<SIMPLEFONT, HASHKEY_STRI>, 0x00544A10<CTexture, HASHKEY_TEXTUREFILE>, 0x00548D50<CModelBlob::CHashEntry, HASHKEY_MODELBLOBFILE>, 0x00546130/0x14019D5C0<ProfileInternal::KEYVALUE, HASHKEY_CONSTSTRI>, 0x005461C0/0x14019D650<ProfileInternal::SECTION, HASHKEY_CONSTSTRI>, 0x0054A1F0<FONTHASHOBJ, HASHKEY_STR>, 0x0054C260<CTextureBlobTexture, HASHKEY_TEXTUREBLOBFILE>, 0x00490570/0x1400B5110<CGObject_C, CHashKeyGUID>, 0x00704AC0<CACHEENTRY, HASHKEY_NONE>, 0x008F48A0<CHATTHROTTLE, HASHKEY_STR>, 0x008F49C0<PROFANITYCHECK, HASHKEY_STR>, 0x008F4A50<SPAMCHECK, HASHKEY_STR>, 0x008DD680/0x1406520B0<SAVEDVARIABLE, HASHKEY_STR>, 0x00901280<USEABLESTYLE, HASHKEY_NONE>, 0x00989DA0<LFGListParty, CHashKeyGUID>, 0x00989E30<LFGListPlayer, CHashKeyGUID>, 0x00925AD0/0x14068ACD0<KEYCOMMAND, HASHKEY_STRI>, 0x00925B60/0x14068AD60<KEYBINDING, HASHKEY_STRI>, 0x00925BF0/0x14068ADF0<OVERRIDEKEYBINDING, HASHKEY_STRI>, 0x00925C80/0x14068AE80<MODIFIEDCLICK, HASHKEY_STRI>, 0x009E4E10<TAXIPATHNODESCLIENT, HASHKEY_NONE>, 0x0099E3F0/0x140721C10<TalentGroupItem_C, HASHKEY_NONE>, 0x0099E480/0x140721CA0<TalentGroupTierItem_C, HASHKEY_NONE>, 0x00A0F460<MacroConditions, HASHKEY_STR>, 0x0045F1F0/0x1400724B0<UIADDON::METADATA, HASHKEY_STRI>, 0x0044B710/0x140058D60<MOUSELOOKBINDING, HASHKEY_STRI>, 0x004850C0<PLAYERPORTRAIT, CHashKeyGUID>, 0x00849740/0x140577800<CGxShader, HASHKEY_STRI>, 0x00611C50<CLightList, HASHKEY_DWORD>, 0x0061CF30<CHARCODEDESC, HASHKEY_NONE>, 0x0061CFC0/0x1402AB390<KERNNODE, KERNINGHASHKEY>, 0x00622E20<FACEDATA, HASHKEY_STRI>, 0x0056F890<PROFICIENCYSPELL, HASHKEY_NONE>, 0x005E5100<OBJ_EFFECT_GROUP_DEF_REF_LIST_LOOKUP, HASHKEY_NONE>, 0x005E5190<OBJ_EFFECT_REF_LOOKUP, HASHKEY_NONE>, 0x005E52B0<OBJ_EFFECT_REF_LIST_LOOKUP, HASHKEY_NONE>, 0x005E5220<OBJ_EFFECT_GROUP_LOOKUP, HASHKEY_NONE>, 0x005D9CE0/0x140255700<UnitThreat, CHashKeyGUID>, 0x00656A70<CVar, HASHKEY_STRI>, 0x00654C00<CONSOLECOMMAND, HASHKEY_CONSTSTRI>, 0x00A320B0/0x1407DC270<TIMESTAMPSTRUCT, HASHKEY_NONE>, 0x004C31F0<RGN, HASHKEY_NONE>, 0x008C1080<RACECLASSINFO, HASHKEY_NONE>, 0x004EDC60<HashedNode, HASHKEY_STRI>, 0x004EDCF0<FrameFactoryNode, HASHKEY_STRI>, 0x0083F3C0<FrameScript_EventObject, HASHKEY_CONSTSTRI>, 0x0087E780/0x1405B76B0<OsNet::NETSELSOCKPTR, HASHKEY_NONE>, 0x0071D6D0<Liquid::Object, HASHKEY_NONE>, 0x0064F3F0<DayNight::DNOverrideSky, HASHKEY_STRI>, 0x0072F440/0x1404099D0<CMapDoodadDef, HASHKEY_DWORD>, 0x0072F4D0/0x140409A60<CMapObjDef, HASHKEY_NONE>, 0x0077C300<CSceneMaterial, HASHKEY_MATERIAL>, 0x00748270<CMapObj, HASHKEY_NONE>, 0x0073AC50<DDMaterial, HASHKEY_NONE>, 0x00621FC0/0x1402B2D00<BATCHEDRENDERFONTDESC, HASHKEY_PTR>, 0x004ABD50/0x1400DF3A0<DBCache<NameCache, unsigned __int64, CHashKeyGUID>::REVERSEENTRY, HASHKEY_NONE>, 0x004ABDE0/0x1400DF430<DBCache<CreatureStats_C, int, HASHKEY_INT>::DBCACHEHASH, HASHKEY_INT>, 0x004ABF00/0x1400DF550<DBCache<NPCText, int, HASHKEY_INT>::DBCACHEHASH, HASHKEY_INT>, 0x004ABF90/0x1400DF5E0<DBCache<NameCache, unsigned __int64, CHashKeyGUID>::DBCACHEHASH, CHashKeyGUID>, 0x004AC020/0x1400DF670<DBCache<GuildStats_C, int, HASHKEY_INT>::DBCACHEHASH, HASHKEY_INT>, 0x004AC0B0/0x1400DF700<DBCache<QuestCache, int, HASHKEY_INT>::DBCACHEHASH, HASHKEY_INT>, 0x004AC140/0x1400DF790<DBCache<PageTextCache_C, int, HASHKEY_INT>::DBCACHEHASH, HASHKEY_INT>, 0x004AC1D0/0x1400DF820<DBCache<PetNameCache, int, HASHKEY_INT>::DBCACHEHASH, HASHKEY_INT>, 0x004AC260/0x1400DF8B0<DBCache<CGPetition, int, HASHKEY_INT>::DBCACHEHASH, HASHKEY_INT>, 0x004AC2F0/0x1400DF940<DBCache<ItemTextCache_C, unsigned __int64, CHashKeyGUID>::DBCACHEHASH, CHashKeyGUID>, 0x004AC410/0x1400DFA60<DBCache<ArenaTeamCache, int, HASHKEY_INT>::DBCACHEHASH, HASHKEY_INT>, 0x004AC4A0/0x1400DFAF0<DBCache<DanceCache, int, HASHKEY_INT>::DBCACHEHASH, HASHKEY_INT>, 0x0070D190<SE3SoundChunk, HASHKEY_NONE>, 0x0070D8B0<SE3SOUNDKITOBJECT_LOOKUP, HASHKEY_NONE>, 0x0070D650<SOUNDKITLOOKUP, HASHKEY_CONSTSTRI>, 0x0070D220<SE3SoundEmitterLookup, HASHKEY_NONE>, 0x0070D820<SE3COMBINABLESOUNDLIST, HASHKEY_NONE>, 0x00713FF0<SE3ChatSound, HASHKEY_NONE>, 0x00715170<SE3MemorySoundObject, HASHKEY_NONE>, 0x004FCE90<FILENAMEHASH, HASHKEY_STR>, 0x0086EFD0/0x1405A46A0<CGxDeviceD3d11::InputLayoutRec, InputLayoutKey>, 0x0086F060/0x1405A4730<CGxDeviceD3d11::RasterizerStateRec, HASHKEY_NONE>, 0x0086F0F0/0x1405A47C0<CGxDeviceD3d11::BlendStateRec, HASHKEY_NONE>, 0x0086F180/0x1405A4850<CGxDeviceD3d11::DepthStateRec, HASHKEY_NONE>, 0x0086F210/0x1405A48E0<CGxDeviceD3d11::SamplerStateRec, HASHKEY_NONE>, 0x005480A0<FileNotification::DeferredFile, HASHKEY_NONE>, 0x009F34F0<CGSpellActivationOverlay::COUNTERNODE, HASHKEY_NONE>, 0x009F3580<CGSpellActivationOverlay::PETAURANODE, HASHKEY_NONE>, 0x009F7710<CGResearchFrame::BRANCH, HASHKEY_NONE>, 0x009F77A0<CGResearchFrame::BLOBPOIHASH, HASHKEY_NONE>, 0x009CA800<CGCUFProfiles::PROFILENODE, HASHKEY_CONSTUTF8I>, 0x009CA890/0x140759F40<CGCUFProfiles::OPTIONENTRY, HASHKEY_CONSTSTR>, 0x0043A330<CCameraModeEntry, HASHKEY_NONE>, 0x00485150<UNITPORTRAIT, HASHKEY_NONE>, 0x004851E0<ITEMPORTRAIT, HASHKEY_STR>, 0x0048C9B0<WowClientDB2_Base::DBTABLE, HASHKEY_NONE>, 0x004965D0<SPELLACTIVATIONOVERLAY, HASHKEY_NONE>, 0x004DEDD0<TALKINGPLAYER, CHashKeyGUID>, 0x004DEE60<MUTEDPLAYER, CHashKeyGUID>, 0x004E5D70<DECLINEDWORD, HASHKEY_CONSTSTRI>, 0x005914A0<CustomFallbackPath, HASHKEY_NONE>, 0x005A9020<INVENTORYART, HASHKEY_NONE>, 0x005A90B0<ITEMBYNAME, HASHKEY_UTF8I>, 0x005E5340<OBJ_EFFECT_DEF_LOOKUP, HASHKEY_NONE>, 0x005E53D0<OBJ_EFFECT_GROUP_DEF_LOOKUP, HASHKEY_NONE>, 0x005E5460<OBJ_EFFECT_PACKAGE_DEF_LOOKUP, HASHKEY_NONE>, 0x0062F7F0<ZONESOUNDOVERRIDELOOKUP, HASHKEY_STRI>, 0x00633620<DSP_CHAIN_HEAD_LOOKUP, HASHKEY_STRI>, 0x00634370<FOOTSTEPSNDCACHE, HASHKEY_NONE>, 0x008B4FE0<ITEMCOOLDOWNHASHNODE, HASHKEY_NONE>, 0x008B5070<DESTLOCSPELLCASTINDEXHASHNODE, CHashKeyGUID>, 0x00917FB0<OVERRIDE_SPELL_NODE, HASHKEY_NONE>, 0x00918040<SPELLSBYNAME, HASHKEY_UTF8I>, 0x0093BAF0<CGGuildInfo::GuildRecipesKnown, HASHKEY_NONE>, 0x0093BC10<CGGuildInfo::SKILLLINESPELLS, HASHKEY_NONE>, 0x009677B0<CGQuestLog::QUESTCOMPLETIONNPC, HASHKEY_NONE>, 0x009A5550<CFlyout, HASHKEY_NONE>, 0x009A55E0<SpellToFlyoutMapItem, HASHKEY_NONE>, 0x009A8B40<FACTION, HASHKEY_NONE>, 0x009FD970<CGIncomingResurrection::INCOMINGRESLIST, CHashKeyGUID>, 0x00447440<_BANNEDADDON, HASHKEY_NONE>, 0x0045F310<UIADDON, HASHKEY_STRI>, 0x008DD710<TAINTEVENT, HASHKEY_STR>, 0x008F4810<WORDLIST, HASHKEY_LANGUAGE>, 0x00925D10<CLICKFRAME, HASHKEY_STR>, 0x0094ED00<ACHIEVEMENT, HASHKEY_NONE>, 0x009B5780<DYNAMICHOLIDAYHASH, HASHKEY_NONE>, 0x009B8BB0<KNOWNDANCES, HASHKEY_STRI>, 0x009CE320<WORLDSTATE, HASHKEY_NONE>, 0x00B332C0<BITMAPBYICON, HASHKEY_NONE>, 0x008F4930<ADDONPREFIX, HASHKEY_STR>, 0x0094ED90<GUILDACHIEVEMENTMEMBERLIST, HASHKEY_NONE>, 0x00793CB0<`CMapObjGroup::GetHitTestDebugTextures'::`2'::TEXTURENAME, HASHKEY_CONSTSTRI>, 0x0090A3E0<ACCOUNTIDCACHE, HASHKEY_DWORD>, 0x009B56F0<CALENDAREVENTHASH, HASHKEY_NONE>, 0x009DF7B0<CGQuestObjectiveCache::REQUIREDUNITNODE, HASHKEY_NONE>, 0x009DF840<CGQuestObjectiveCache::REQUIREDITEMNODE, HASHKEY_NONE>, 0x009CCFD0<CGAutoCompleteName::AUTOCOMPLETE_CHARACTER_HASHNODE, CHashKeyGUID>, 0x009CD060<CGAutoCompleteName::AUTOCOMPLETE_BATTLENET_HASHNODE, HASHKEY_NONE>, 0x0093BB80/0x1406A6F20<CGGuildInfo::SKILLLINESPELLBITS, HASHKEY_NONE>, 0x0056F800<TRADESKILLLINE, HASHKEY_NONE>, 0x004ABE70/0x1400DF4C0<DBCache<GameObjectStats_C, int, HASHKEY_INT>::DBCACHEHASH, HASHKEY_INT>, 0x009180D0<OVERRIDE_SPELL_BY_NAME_NODE, HASHKEY_UTF8I>, 0x004AC380/0x1400DF9D0<DBCache<WardenCachedModule, CWardenKey, CWardenKey>::DBCACHEHASH, CWardenKey>, 0x00596280/0x140203C40<VoidItem_C, HASHKEY_NONE>, 0x00434E20<CShaderEffect, HASHKEY_STRI>, 0x0045F280<ADDONSTATE, HASHKEY_STRI>, 0x009C6B80<MACRONODE, HASHKEY_NONE>, 0x00B33350<LCDHANDLE, HASHKEY_CONSTSTRI>]
template <class T, class TKey>
int32_t TSHashTable<T, TKey>::MonitorFullness(uint32_t slot) {
    if (this->m_slotmask >= this->m_maxslots - 1) {
        return 0;
    }

    if (this->m_fullnessIndicator > 3) {
        this->m_fullnessIndicator -= 3;
    } else {
        this->m_fullnessIndicator = 0;
    }

    TSExplicitList<T>& list = this->m_slotlistarray[slot];
    T* node = list.Head();

    while (!TSExplicitList<T>::IsEnd(node)) {
        T* next = list.RawNext(node);

        if (++this->m_fullnessIndicator > 13) {
            this->m_fullnessIndicator = 0;
            this->GrowListArray(2 * this->m_slotmask + 2);
            return 1;
        }

        node = next;
    }

    return 0;
}

// OFFSET: [0x004852D0<PLAYERPORTRAIT, CHashKeyGUID>, 0x004ACB00<DBCache<WardenCachedModule, CWardenKey, CWardenKey>::DBCACHEHASH, CWardenKey>, 0x004ACAA0<DBCache<GuildStats_C, uint64_t, CHashKeyGUID>::DBCACHEHASH, CHashKeyGUID>, 0x004ACA70, 0x004ACAD0]
template <class T, class TKey>
T* TSHashTable<T, TKey>::New(uint32_t hashval, const TKey& key, size_t extrabytes, uint32_t flags) {
    T* node = this->InternalNewNode(hashval, extrabytes, flags);
    node->m_hashval = hashval;
    node->m_key = key;

    return node;
}

// OFFSET: [0x00656B60<CVar, HASHKEY_STRI>, 0x008F5D30<PROFANITYCHECK, HASHKEY_STR>, 0x008F5D90<SPAMCHECK, HASHKEY_STR>, 0x009181C0<OVERRIDE_SPELL_BY_NAME_NODE, HASHKEY_UTF8I>, 0x00925FB0/0x14068B200<KEYCOMMAND, HASHKEY_STRI>]
template <class T, class TKey>
T* TSHashTable<T, TKey>::New(uint32_t hashval, const char* str, size_t extrabytes, uint32_t flags) {
    T* node = this->InternalNewNode(hashval, extrabytes, flags);
    node->m_hashval = hashval;
    node->m_key = str;

    return node;
}

// OFFSET: [0x00656BC0<CVar, HASHKEY_STRI>, 0x008F5E10<PROFANITYCHECK, HASHKEY_STR>, 0x008F5E40<SPAMCHECK, HASHKEY_STR>, 0x00918220<OVERRIDE_SPELL_BY_NAME_NODE, HASHKEY_UTF8I>, 0x00926070/0x14068B300<KEYCOMMAND, HASHKEY_STRI>, 0x009260A0/0x14068B360<KEYBINDING, HASHKEY_STRI>]
template <class T, class TKey>
T* TSHashTable<T, TKey>::New(const char* str, size_t extrabytes, uint32_t flags) {
    return this->New(SStrHashHT(str), str, extrabytes, flags);
}

// OFFSET: [0x0049D640/0x1400C9550<DBCache<WardenCachedModule, CWardenKey, CWardenKey>::DBCACHEHASH, CWardenKey>, 0x0086D810/0x140752FC0<CMapDynamicTexture, HASHKEY_DWORD>, 0x0077B030/0x140467130<CSceneMaterial, HASHKEY_MATERIAL>, 0x0086D810/0x140752FC0<CGSpellActivationOverlay::COUNTERNODE, HASHKEY_NONE>, 0x0086D810/0x140752FC0<CGResearchFrame::BRANCH, HASHKEY_NONE>, 0x0086D810/0x140752FC0<CGResearchFrame::BLOBPOIHASH, HASHKEY_NONE>, 0x0048E3C0/0x1400B2550<CGObject_C, CHashKeyGUID>, 0x005C3E70<UnitThreat, CHashKeyGUID>, 0x0061BDA0<KERNNODE, KERNINGHASHKEY>, 0x007252F0<CMapDoodadDef, HASHKEY_DWORD>, 0x00621090<CLightList, HASHKEY_DWORD>, 0x00725350<CMapObj, HASHKEY_NONE>, 0x0086D7A0/0x140624070<WORDLIST, HASHKEY_LANGUAGE>, 0x009CBB50/0x14011B230<DBCache<NameCache, unsigned __int64, CHashKeyGUID>::DBCACHEHASH, CHashKeyGUID>, 0x0054B440<CTextureBlobTexture, HASHKEY_TEXTUREBLOBFILE>, 0x00542070<CTexture, HASHKEY_TEXTUREFILE>]
template <class T, class TKey>
T* TSHashTable<T, TKey>::Ptr(uint32_t hashval, const TKey& key) {
    if (this->m_slotmask == 0xFFFFFFFF) {
        return NULL;
    }

    TSExplicitList<T>& list = this->m_slotlistarray[hashval & this->m_slotmask];

    for (T* node = list.Head(); node; node = list.Next(node)) {
        if (node->m_hashval == hashval && node->m_key == key) {
            return node;
        }
    }

    return NULL;
}

// OFFSET: [0x0049D6B0/0x140757780<CGCUFProfiles::PROFILENODE, HASHKEY_CONSTUTF8I>, 0x004E5510/0x140757850<CGCUFProfiles::OPTIONENTRY, HASHKEY_CONSTSTR>, 0x00622560/0x1402B3560<FACEDATA, HASHKEY_STRI>, 0x00549980<FONTHASHOBJ, HASHKEY_STR>, 0x006327F0/0x14003D720<MODIFIEDCLICK, HASHKEY_STRI>, 0x00653A60/0x140483960<LCDHANDLE, HASHKEY_CONSTSTRI>, 0x008ECA60/0x140684D60<KNOWNDANCES, HASHKEY_STRI>, 0x00912540/0x14066E810<SPELLSBYNAME, HASHKEY_UTF8I>]
template <class T, class TKey>
T* TSHashTable<T, TKey>::Ptr(const char* str) {
    if (this->m_slotmask == 0xFFFFFFFF) {
        return NULL;
    }

    uint32_t hashval = SStrHashHT(str);
    TSExplicitList<T>& list = this->m_slotlistarray[hashval & this->m_slotmask];

    for (T* node = list.Head(); node; node = list.Next(node)) {
        if (node->m_hashval == hashval && node->m_key == str) {
            return node;
        }
    }

    return NULL;
}

// OFFSET: [0x0075F800/0x140443950<CMapObjDef, HASHKEY_NONE>, 0x00541B50<CTexture, HASHKEY_TEXTUREFILE>, 0x00703FE0<CGCUFProfiles::PROFILENODE, HASHKEY_CONSTUTF8I>, 0x0048DED0/0x1400B1F00<CGObject_C, CHashKeyGUID>, 0x005498F0/0x140231B80]
template <class T, class TKey>
void TSHashTable<T, TKey>::Unlink(T* ptr) {
    if (ptr->m_linktoslot.IsLinked()) {
        ptr->m_linktoslot.Unlink();
        ptr->m_linktofull.Unlink();
    }
}

// OFFSET: [0x004C2DE0/0x1400FDC20<RGN, HASHKEY_NONE>]
template <class T, class TKey>
TSHashTableReuse<T, TKey>::TSHashTableReuse() {
    this->m_reuseList.ChangeLinkOffset(offsetof(T, m_linktoslot));
}

// OFFSET: [0x00548A90/0x1401A1030<CModelBlob::CHashEntry, HASHKEY_MODELBLOBFILE>, 0x00548AF0/0x1401A10B0<CModelBlob::CHashEntry, HASHKEY_MODELBLOBFILE>, 0x0054C1D0/0x1401A5C50<CTextureBlobTexture, HASHKEY_TEXTUREBLOBFILE>, 0x0054C230/0x1401A5CD0<CTextureBlobTexture, HASHKEY_TEXTUREBLOBFILE>, 0x00611BC0/0x14029C920<CLightList, HASHKEY_DWORD>, 0x00611C20/0x14029C9A0<CLightList, HASHKEY_DWORD>, 0x004D9860/0x140117CD0<TIMESTAMPSTRUCT, HASHKEY_NONE>, 0x005445A0/0x140198600<CTexture, HASHKEY_TEXTUREFILE>, 0x00544600/0x140198680<CTexture, HASHKEY_TEXTUREFILE>, 0x005E3B90/0x140261B00<OBJ_EFFECT_PACKAGE_DEF_LOOKUP, HASHKEY_NONE>, 0x0070D760/0x1403DD3E0<SE3COMBINABLESOUNDLIST, HASHKEY_NONE>, 0x0070D7F0/0x1403DD460<SE3COMBINABLESOUNDLIST, HASHKEY_NONE>, 0x006331B0/0x1402C8A20<DSP_CHAIN_HEAD_LOOKUP, HASHKEY_STRI>, 0x00917D50/0x140678210<OVERRIDE_SPELL_NODE, HASHKEY_NONE>, 0x00917EF0/0x1406784E0<OVERRIDE_SPELL_BY_NAME_NODE, HASHKEY_UTF8I>, 0x00900E90/0x140658F70<STYLEREC, ?>, 0x009170E0/0x140676D10<SPELLBYNAME, ?>, 0x004C3160/0x1400FE1A0<RGN, HASHKEY_NONE>, 0x004C31C0/0x1400FE220]
template <class T, class TKey>
TSHashTableReuse<T, TKey>::~TSHashTableReuse() {
    this->Destructor();
}

// OFFSET: [0x005489D0/0x1401A0E80<CModelBlob::CHashEntry, HASHKEY_MODELBLOBFILE>, 0x0054C090/0x1401A59F0<CTextureBlobTexture, HASHKEY_TEXTUREBLOBFILE>, 0x006110D0/0x14029B880<CLightList, HASHKEY_DWORD>, 0x00544580/0x1401985E0<CTexture, HASHKEY_TEXTUREFILE>, 0x0070D040/0x1403DD1B0<SE3COMBINABLESOUNDLIST, HASHKEY_NONE>, 0x00917D30/0x1406781F0<OVERRIDE_SPELL_NODE, HASHKEY_NONE>, 0x00917ED0/0x1406784C0<OVERRIDE_SPELL_BY_NAME_NODE, HASHKEY_UTF8I>, 0x004C2290/0x1400FCDD0<RGN, HASHKEY_NONE>]
template <class T, class TKey>
void TSHashTableReuse<T, TKey>::Destroy() {
    this->Clear();
    this->Destructor();
}

// OFFSET: [0x005482D0/0x1401A0490<CModelBlob::CHashEntry, HASHKEY_MODELBLOBFILE>, 0x0054B4D0/0x1401A4840<CTextureBlobTexture, HASHKEY_TEXTUREBLOBFILE>, 0x00541BE0/0x1401946C0<CTexture, HASHKEY_TEXTUREFILE>, 0x00707A40/0x1403D6010<SE3COMBINABLESOUNDLIST, HASHKEY_NONE>, 0x009125E0/0x14066E8E0<OVERRIDE_SPELL_NODE, HASHKEY_NONE>, 0x00912650/0x14066E980<OVERRIDE_SPELL_BY_NAME_NODE, HASHKEY_UTF8I>, 0x00610CC0/0x14029B2E0<CLightList, HASHKEY_DWORD>, 0x004C21C0/0x1400FCD30<RGN, HASHKEY_NONE>]
template <class T, class TKey>
void TSHashTableReuse<T, TKey>::Destructor() {
    T* node;

    while ((node = this->m_reuseList.Head()) != NULL) {
        this->m_reuseList.UnlinkNode(node);
        SMemFree(node, typeid(T).raw_name(), SLOG_OBJECT, 0);
    }
}

// OFFSET: [0x00611A80/0x14029C6D0<CLightList, HASHKEY_DWORD>, 0x00544500/0x140198540<CTexture, HASHKEY_TEXTUREFILE>, 0x0070D6E0/0x1403DD340<SE3COMBINABLESOUNDLIST, HASHKEY_NONE>, 0x00917C10/0x140677FD0<OVERRIDE_SPELL_NODE, HASHKEY_NONE>, 0x00917DB0/0x140678290<OVERRIDE_SPELL_BY_NAME_NODE, HASHKEY_UTF8I>, 0x004C2EB0/0x1400FDD70<RGN, HASHKEY_NONE>, 0x0054C0B0/0x1401A5A10<CModelBlob::CHashEntry, HASHKEY_MODELBLOBFILE>]
template <class T, class TKey>
void TSHashTableReuse<T, TKey>::InternalDelete(T* ptr) {
    this->m_fulllist.UnlinkNode(ptr);
    ptr->~T();
    new (&ptr->m_linktoslot) TSLink<T>;
    this->m_reuseList.LinkToHead(ptr);
}

// OFFSET: [0x00544940/0x140198BA0<CTexture, HASHKEY_TEXTUREFILE>, 0x005489F0/0x1401A0EA0<CModelBlob::CHashEntry, HASHKEY_MODELBLOBFILE>, 0x00611B00/0x14029C770<CLightList, HASHKEY_DWORD>, 0x0070D060/0x1403DD1D0<SE3COMBINABLESOUNDLIST, HASHKEY_NONE>, 0x0054C130/0x1401A5AC0<CTextureBlobTexture, HASHKEY_TEXTUREBLOBFILE>, 0x00917E30/0x140678330<OVERRIDE_SPELL_BY_NAME_NODE, HASHKEY_UTF8I>, 0x004C2E40/0x1400FDC90<RGN, HASHKEY_NONE>]
template <class T, class TKey>
T* TSHashTableReuse<T, TKey>::InternalNew(TSExplicitList<T>* listptr, size_t extrabytes, uint32_t flags) {
    T* node = this->m_reuseList.Head();

    if (!node) {
        return listptr->NewNode(1, 0, 0);
    }

    this->m_reuseList.UnlinkNode(node);
    new (node) T;
    listptr->LinkToHead(node);

    return node;
}

// INLINED
template <class T, class THandle>
TSExportTableSimpleReuse<T, THandle>::TSExportTableSimpleReuse() {
    this->m_sequence = static_cast<uint32_t>(~reinterpret_cast<uintptr_t>(this) & 0xFFFFFFF);
    this->m_wrapped = 0;
}

// INLINED
template <class T, class THandle>
void TSExportTableSimpleReuse<T, THandle>::Delete(T* ptr) {
    TSHashTable<T, HASHKEY_NONE>::Delete(ptr);
}

// OFFSET: [0x004C2230<RGN, HSRGN>]
template <class T, class THandle>
THandle TSExportTableSimpleReuse<T, THandle>::GenerateUniqueHandle() {
    do {
        while (++this->m_sequence == 0) {
            this->m_wrapped = 1;
        }
    } while (this->m_wrapped && this->Ptr(reinterpret_cast<THandle>(static_cast<uintptr_t>(this->m_sequence))));

    return reinterpret_cast<THandle>(static_cast<uintptr_t>(this->m_sequence));
}

// INLINED
template <class T, class THandle>
T* TSExportTableSimpleReuse<T, THandle>::New(THandle* handle) {
    THandle uniquehandle = this->GenerateUniqueHandle();
    *handle = uniquehandle;

    return TSHashTable<T, HASHKEY_NONE>::New(static_cast<uint32_t>(reinterpret_cast<uintptr_t>(uniquehandle)), this->m_key, 0, 0);
}

// INLINED
template <class T, class THandle>
T* TSExportTableSimpleReuse<T, THandle>::Ptr(THandle handle) {
    return TSHashTable<T, HASHKEY_NONE>::Ptr(static_cast<uint32_t>(reinterpret_cast<uintptr_t>(handle)), this->m_key);
}

// OFFSET: 0x004C3280/0x1400FE270
template <class T, class THandle, class TLockedHandle, class TSync>
TSExportTableSyncReuse<T, THandle, TLockedHandle, TSync>::~TSExportTableSyncReuse() {
}

// OFFSET: 0x004C28D0/0x1400FD510
template <class T, class THandle, class TLockedHandle, class TSync>
void TSExportTableSyncReuse<T, THandle, TLockedHandle, TSync>::Delete(THandle handle) {
    TLockedHandle lockedhandle;
    T* ptr = this->Lock(handle, &lockedhandle, 1);
    this->DeleteUnlock(ptr, lockedhandle);
}

// INLINED
template <class T, class THandle, class TLockedHandle, class TSync>
void TSExportTableSyncReuse<T, THandle, TLockedHandle, TSync>::DeleteUnlock(T* ptr, TLockedHandle lockedhandle) {
    TSExportTableSimpleReuse<T, THandle>::Delete(ptr);
    this->Unlock(lockedhandle);
}

// INLINED
template <class T, class THandle, class TLockedHandle, class TSync>
int32_t TSExportTableSyncReuse<T, THandle, TLockedHandle, TSync>::IsForWriting(TLockedHandle lockedhandle) {
    return lockedhandle == reinterpret_cast<TLockedHandle>(1);
}

// INLINED
template <class T, class THandle, class TLockedHandle, class TSync>
T* TSExportTableSyncReuse<T, THandle, TLockedHandle, TSync>::Lock(THandle handle, TLockedHandle* lockedhandle, int32_t forwriting) {
    this->SyncEnterLock(lockedhandle, forwriting);
    T* ptr = this->Ptr(handle);

    if (!ptr) {
        this->SyncLeaveLock(*lockedhandle);
        *lockedhandle = NULL;
    }

    return ptr;
}

// INLINED
template <class T, class THandle, class TLockedHandle, class TSync>
T* TSExportTableSyncReuse<T, THandle, TLockedHandle, TSync>::NewLock(THandle* handle, TLockedHandle* lockedhandle) {
    this->SyncEnterLock(lockedhandle, 1);
    return this->New(handle);
}

// INLINED
template <class T, class THandle, class TLockedHandle, class TSync>
void TSExportTableSyncReuse<T, THandle, TLockedHandle, TSync>::SyncEnterLock(TLockedHandle* lockedhandle, int32_t forwriting) {
    this->m_sync.Enter(forwriting);
    *lockedhandle = reinterpret_cast<TLockedHandle>(static_cast<intptr_t>(2 * (forwriting != 0) - 1));
}

// INLINED
template <class T, class THandle, class TLockedHandle, class TSync>
void TSExportTableSyncReuse<T, THandle, TLockedHandle, TSync>::SyncLeaveLock(TLockedHandle lockedhandle) {
    if (lockedhandle) {
        this->m_sync.Leave(this->IsForWriting(lockedhandle));
    }
}

// OFFSET: 0x004C1B70
template <class T, class THandle, class TLockedHandle, class TSync>
void TSExportTableSyncReuse<T, THandle, TLockedHandle, TSync>::Unlock(TLockedHandle lockedhandle) {
    this->SyncLeaveLock(lockedhandle);
}

// INLINED
inline bool HASHKEY_CONSTSTR::operator==(const char* str) {
    return this->m_str == str || SStrCmp(this->m_str, str, STORM_MAX_STR) == 0;
}

// INLINED
inline HASHKEY_CONSTSTRI& HASHKEY_CONSTSTRI::operator=(const char* str) {
    this->m_str = str;
    return *this;
}

// INLINED
inline bool HASHKEY_CONSTSTRI::operator==(const char* str) {
    return this->m_str == str || SStrCmpI(this->m_str, str, STORM_MAX_STR) == 0;
}

// INLINED
inline bool HASHKEY_NONE::operator==(const HASHKEY_NONE& key) {
    return true;
}

// INLINED
inline bool HASHKEY_DWORD::operator==(const HASHKEY_DWORD& key) {
    return this->m_key == key.m_key;
}

// INLINED
inline bool HASHKEY_PTR::operator==(const HASHKEY_PTR& key) {
    return this->m_key == key.m_key;
}

// INLINED
inline HASHKEY_CONSTSTR::HASHKEY_CONSTSTR() {
    this->m_str = NULL;
}

// INLINED
inline HASHKEY_STR::HASHKEY_STR() {
    this->m_str = NULL;
}

// INLINED
inline HASHKEY_STR::~HASHKEY_STR() {
    if (this->m_str) {
        SMemFree(this->m_str, __FILE__, __LINE__, 0);
    }
}

// INLINED
inline HASHKEY_STR& HASHKEY_STR::operator=(const char* str) {
    if (this->m_str != str) {
        if (this->m_str) {
            SMemFree(this->m_str, __FILE__, __LINE__, 0);
        }

        this->m_str = SStrDupA(str, __FILE__, __LINE__);
    }

    return *this;
}

// INLINED
inline bool HASHKEY_STR::operator==(const char* str) {
    return SStrCmp(this->m_str, str, STORM_MAX_STR) == 0;
}

// INLINED
inline HASHKEY_STRI& HASHKEY_STRI::operator=(const char* str) {
    HASHKEY_STR::operator=(str);
    return *this;
}

// INLINED
inline bool HASHKEY_STRI::operator==(const char* str) {
    return SStrCmpI(this->m_str, str, STORM_MAX_STR) == 0;
}

// INLINED
inline HASHKEY_UTF8I& HASHKEY_UTF8I::operator=(const char* str) {
    if (this->m_str != str) {
        if (this->m_str) {
            SMemFree(this->m_str, __FILE__, __LINE__, 0);
        }

        this->m_str = SStrDupA(str, __FILE__, __LINE__);
    }

    return *this;
}

// INLINED
inline bool HASHKEY_UTF8I::operator==(const char* str) {
    return SStrCmpUTF8I(this->m_str, str, STORM_MAX_STR) == 0;
}

// INLINED
template <class T>
void TSBitField<T>::Load(const void* data, uint32_t bytes, int32_t bits) {
    uint32_t count = bits == -1 ? bytes * 8 : bits;
    this->m_count = count;
    this->m_bits.SetCount(count ? ((count - 1) >> 5) + 1 : 0);
    memcpy(this->m_bits.m_data, data, bytes);
}

// OFFSET: [0x00409800/0x14000BF30, 0x008A6200<SpellCastTargetPoint>]
template <class T>
TSStackArray<T>::TSStackArray(T* data, uint32_t count, int32_t construct) {
    this->m_data = data;

    if (construct) {
        this->m_count = count;

        for (uint32_t i = 0; i < count; i++) {
            new (&this->m_data[i]) T();
        }
    } else {
        this->m_count = 0;
    }
}

// INLINED
template <class T, uint32_t N>
TSCGrowableArray<T, N>::TSCGrowableArray() {
    this->m_count = 0;
}

// OFFSET: [0x0059B410<MapObjSpeedTransition, 8>, 0x0058CAD0<AnimKitSegmentInst *, 8>]
template <class T, uint32_t N>
uint32_t TSCGrowableArray<T, N>::Count() const {
    if (this->m_count == 0xFFFFFFFF) {
        return this->Dynamic().m_count;
    }

    return this->m_count;
}

// INLINED
template <class T, uint32_t N>
T* TSCGrowableArray<T, N>::Ptr() {
    if (this->m_count == 0xFFFFFFFF) {
        return this->Dynamic().m_data;
    }

    return this->Inline();
}

// INLINED
template <class T, uint32_t N>
const T* TSCGrowableArray<T, N>::Ptr() const {
    if (this->m_count == 0xFFFFFFFF) {
        return this->Dynamic().m_data;
    }

    return this->Inline();
}

// INLINED
template <class T, uint32_t N>
T* TSCGrowableArray<T, N>::Inline() {
    return reinterpret_cast<T*>(this->m_inlineStorage);
}

// INLINED
template <class T, uint32_t N>
const T* TSCGrowableArray<T, N>::Inline() const {
    return reinterpret_cast<const T*>(this->m_inlineStorage);
}

// INLINED
template <class T, uint32_t N>
TSGrowableArray<T>& TSCGrowableArray<T, N>::Dynamic() {
    return *reinterpret_cast<TSGrowableArray<T>*>(this->m_dynamicStorage);
}

// INLINED
template <class T, uint32_t N>
const TSGrowableArray<T>& TSCGrowableArray<T, N>::Dynamic() const {
    return *reinterpret_cast<const TSGrowableArray<T>*>(this->m_dynamicStorage);
}

// INLINED
template <class T, uint32_t N>
TSCGrowableArray<T, N>::~TSCGrowableArray() {
    if (this->m_count == 0xFFFFFFFF) {
        this->Dynamic().~TSGrowableArray<T>();
    } else {
        this->SetCount(0);
    }
}

// OFFSET: [0x005AC6A0, 0x0058CAA0/0x1401F6490<AnimKitDesiredBoneSetInst, 10>, 0x005B6340<AuraData, 16>, 0x00755320<CMapObjDefGroup *, 8>, 0x0098B5A0<REFORGE_OPTION, 16>, 0x0058CA70<AnimKitBoneSetInst, 10>]
template <class T, uint32_t N>
T& TSCGrowableArray<T, N>::operator[](uint32_t index) {
    if (this->m_count == 0xFFFFFFFF) {
        return this->Dynamic().m_data[index];
    }

    return this->Inline()[index];
}

// OFFSET: [0x00760A10/0x1404450E0<CMapObjDefGroup*, 8>, 0x005A1630<MapObjSpeedTransition, 8>, 0x009F5210<const ResearchBranchRec*, 32>, 0x0058E0A0<AnimKitSegmentBoneSetInst *, 3>, 0x0058E1B0/0x1401F84A0<CAnimReplacementSetDef const *, 3>, 0x0058E220<AnimKitSegmentInst *, 8>, 0x0058E290<AnimKitSegmentBoneSetInst *, 5>, 0x0058E300<AnimKitSegmentInst *, 4>, 0x00955370<UIButtons, 144>, 0x0058E370<AnimKitDeferredCommand, 2>, 0x0058E110<AnimKitBoneSetInst, 2>, 0x00591CD0<AnimKitDesiredBoneSetInst, 10>, 0x005EABD0<CAnimKitInst*, 4>, 0x004E4B10/0x140126170<uint16_t, 8>, 0x0098C7F0<REFORGE_OPTION, 16>, 0x005C3EE0<AuraData, 16>, 0x005C3FE0<SortedAuraInfo, 16>, 0x005C3F60]
template <class T, uint32_t N>
void TSCGrowableArray<T, N>::BecomeDynamic() {
    const uint32_t overlap = (sizeof(TSGrowableArray<T>) + sizeof(T) - 1) / sizeof(T);
    uint32_t count = this->m_count;

    if (count > overlap) {
        count = overlap;
    }

    T saved[overlap];

    if (count) {
        memcpy(saved, this->Inline(), sizeof(T) * count);
    }

    new (this->m_dynamicStorage) TSGrowableArray<T>();
    this->Dynamic().Add(count, saved);
    this->Dynamic().Add(this->m_count - count, &this->Inline()[count]);
    this->m_count = 0xFFFFFFFF;
}

// INLINED
template <class T, uint32_t N>
T* TSCGrowableArray<T, N>::New() {
    if (this->m_count == 0xFFFFFFFF) {
        return this->Dynamic().New();
    }

    if (this->m_count == N) {
        this->BecomeDynamic();
        return this->Dynamic().New();
    }

    T* element = &this->Inline()[this->m_count];
    new (element) T;
    this->m_count++;

    return element;
}

// OFFSET: [0x0058E980<AnimKitSegmentInst*, 8>, 0x0058E820<AnimKitSegmentBoneSetInst *, 5>, 0x0058E880/0x1401F79A0<AnimKitDeferredCommand, 2>, 0x0098CB50<REFORGE_OPTION, 16>]
template <class T, uint32_t N>
T* TSCGrowableArray<T, N>::New(const T& source) {
    if (this->m_count == 0xFFFFFFFF) {
        return this->Dynamic().New(source);
    }

    if (this->m_count == N) {
        this->BecomeDynamic();
        return this->Dynamic().New(source);
    }

    T* element = &this->Inline()[this->m_count];
    new (element) T(source);
    this->m_count++;

    return element;
}

// OFFSET: [0x009F5280<const ResearchBranchRec*, 32>]
template <class T, uint32_t N>
void TSCGrowableArray<T, N>::Add(uint32_t count, uint32_t incr, const T* data) {
    if (this->m_count == 0xFFFFFFFF) {
        this->Dynamic().Add(count, incr, data);
        return;
    }

    if (this->m_count + count > N) {
        this->BecomeDynamic();
        this->Dynamic().Add(count, incr, data);
        return;
    }

    T* element = &this->Inline()[this->m_count];
    T* end = element + count;

    for (; element != end; element++) {
        new (element) T(*data);
        data = reinterpret_cast<const T*>(reinterpret_cast<const uint8_t*>(data) + incr);
    }

    this->m_count += count;
}

// OFFSET: [0x0058E900/0x1401F91D0<AnimKitBoneSetInst, 10>, 0x00592130/0x1401FE230<AnimKitDesiredBoneSetInst, 10>, 0x0098CA70/0x14070AE00<REFORGE_OPTION, 16>]
template <class T, uint32_t N>
void TSCGrowableArray<T, N>::SetCount(uint32_t count) {
    if (this->m_count == 0xFFFFFFFF) {
        this->Dynamic().SetCount(count);
    } else if (count <= N) {
        T* data = this->Inline();

        for (uint32_t i = this->m_count; i < count; i++) {
            new (&data[i]) T;
        }

        for (uint32_t i = count; i < this->m_count; i++) {
            data[i].~T();
        }

        this->m_count = count;
    } else {
        this->BecomeDynamic();
        this->Dynamic().SetCount(count);
    }
}

// OFFSET: [0x00773710<CWorldMapPreloadData>, 0x008A63C0<SpellCastTargetPoint>]
template <class T>
void TSStackArray<T>::SetCount(uint32_t count) {
    if (count == this->m_count) {
        return;
    }

    for (uint32_t i = this->m_count; i < count; i++) {
        new (&this->m_data[i]) T();
    }

    this->m_count = count;
}

// OFFSET: [0x004E3FB0<uint16_t>]
template <class T>
void TSStackArray<T>::Set(uint32_t count, const T* data) {
    if (count == this->m_count) {
        for (uint32_t i = 0; i < count; i++) {
            this->m_data[i] = data[i];
        }
    } else if (count <= this->m_count) {
        for (uint32_t i = 0; i < count; i++) {
            this->m_data[i] = data[i];
        }
    } else {
        for (uint32_t i = 0; i < this->m_count; i++) {
            this->m_data[i] = data[i];
        }

        for (uint32_t i = this->m_count; i < count; i++) {
            new (&this->m_data[i]) T(data[i]);
        }
    }

    this->m_count = count;
}

// OFFSET: [0x00814F50<uint32_t>]
template <class T>
void TSStackArray<T>::Set(uint32_t count, uint32_t stride, const T* data) {
    const T* source = data;

    if (count == this->m_count) {
        for (uint32_t i = 0; i < count; i++) {
            this->m_data[i] = *source;
            source = reinterpret_cast<const T*>(reinterpret_cast<const char*>(source) + stride);
        }
    } else if (count <= this->m_count) {
        for (uint32_t i = 0; i < count; i++) {
            this->m_data[i] = *source;
            source = reinterpret_cast<const T*>(reinterpret_cast<const char*>(source) + stride);
        }
    } else {
        for (uint32_t i = 0; i < this->m_count; i++) {
            this->m_data[i] = *source;
            source = reinterpret_cast<const T*>(reinterpret_cast<const char*>(source) + stride);
        }

        for (uint32_t i = this->m_count; i < count; i++) {
            new (&this->m_data[i]) T(*source);
            source = reinterpret_cast<const T*>(reinterpret_cast<const char*>(source) + stride);
        }
    }

    this->m_count = count;
}

#endif

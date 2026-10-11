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
    /* 0000 */ ptrdiff_t m_linkoffset;
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

// OFFSET: [%0x0046A3F0<BFSNODE>]
template <class T>
TSLink<T>* TSLink<T>::NextLink(ptrdiff_t linkoffset) {
    uintptr_t next = reinterpret_cast<uintptr_t>(this->m_next);

    if ((next & 1) || !next) {
        return reinterpret_cast<TSLink<T>*>(next & ~static_cast<uintptr_t>(1));
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

// OFFSET: [%0x006C8150<VBBList_Block>, %0x0044C510]
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

// PROVEN: aaadd5d6 IDENTICAL/DIFFERENT
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

// OFFSET: [%0x00520630<CSimpleAnimGroup, TSGetExplicitLink<CSimpleAnimGroup>>, %0x00934DD0<CGChatBubbleFrame, TSGetLink<CGChatBubbleFrame>>, %0x00451080]
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

// OFFSET: [#0x0047C470, #0x0047E7C0, #0x0047E830, #0x0047E8A0, #0x00485970<RENDERCALLBACKNODE, TSGetLink<RENDERCALLBACKNODE>>, #0x00488940, 0x00508E90/0x140150AA0<CLayoutFrame::FRAMENODE, TSGetLink<CLayoutFrame::FRAMENODE>>, #0x00498AE0, #0x00498B80<CSimpleAnimGroup, TSGetExplicitLink<CSimpleAnimGroup>>, #0x0049A610<SIMPLEANIMNODE, TSGetLink<SIMPLEANIMNODE>>, #0x004B73E0<CGxTexCache, TSGetExplicitLink<CGxTexCache>>, #0x004B7460<CMipBitsCache, TSGetExplicitLink<CMipBitsCache>>, %0x005504F0, #0x004BA2A0, #0x004BCEF0, #0x004C9650, #0x004CA310<SuppressorSound, TSGetLink<SuppressorSound>>, #0x004CDA60<SI3DspNode, TSGetLink<SI3DspNode>>, #0x004DA510<ConsoleString, TSGetExplicitLink<ConsoleString>>, #0x004F1650<CComponentMipBits, TSGetExplicitLink<CComponentMipBits>>, #0x004F92A0<CModelRecord, TSGetLink<CModelRecord>>, #0x00502270<PENDINGUSERLIST, TSGetExplicitLink<PENDINGUSERLIST>>, #0x00503060<THROTTLEDCHAT, TSGetExplicitLink<THROTTLEDCHAT>>, #0x00527B00<CapturePointUIManagerNode, TSGetLink<CapturePointUIManagerNode>>, #0x00528690<AUTOCOMPLETE_NAMENODE, TSGetLink<AUTOCOMPLETE_NAMENODE>>, #0x0052F090<STYLEREC, TSGetLink<STYLEREC>>, #0x0053EEB0, #0x0055FD20<OVERRIDEKEYBINDINGENTRY, TSGetLink<OVERRIDEKEYBINDINGENTRY>>, #0x0056C9C0<CGChatBubbleFrame, TSGetLink<CGChatBubbleFrame>>, #0x005AE0C0<EQUIPMENT_SET_NODE, TSGetLink<EQUIPMENT_SET_NODE>>, #0x005B2170<CLIENT_ACHIEVEMENT_CRITERIA, TSGetLink<CLIENT_ACHIEVEMENT_CRITERIA>>, #0x005B2260<CLIENT_ACHIEVEMENTS, TSGetLink<CLIENT_ACHIEVEMENTS>>, #0x005B4CF0<COMPLETED_QUESTS, TSGetLink<COMPLETED_QUESTS>>, #0x005BE660<CALENDARHOLIDAYNODE, TSGetLink<CALENDARHOLIDAYNODE>>, #0x005BE6D0<CALENDARRAIDRESETNODE, TSGetLink<CALENDARRAIDRESETNODE>>, #0x005BE740<CalendarPendingInvite, TSGetLink<CalendarPendingInvite>>, #0x005F3DB0<_BANNEDADDONLOADNODE, TSGetLink<_BANNEDADDONLOADNODE>>, #0x00606480<CameraShake, TSGetLink<CameraShake>>, #0x00615FF0<BFSNODE, TSGetLink<BFSNODE>>, #0x00617210<PORTRAITREQUEST, TSGetLink<PORTRAITREQUEST>>, 0x0048A1C0/0x1400AC6A0<NETEVENTQUEUENODE, TSGetLink<NETEVENTQUEUENODE>>, #0x00670070<DBCACHECALLBACK, TSGetLink<DBCACHECALLBACK>>, 0x00841620<CGxBuf, TSGetLink<CGxBuf>>, #0x006BC100, #0x006BDDC0<CGxFont, TSGetLink<CGxFont>>, #0x006BDE40<CGxString, TSGetLink<CGxString>>, #0x006BF210<CGxStringBatch, TSGetLink<CGxStringBatch>>, #0x006E4B80<PlayerPendingItemExpiration, TSGetExplicitLink<PlayerPendingItemExpiration>>, #0x006EBF90<CPlayerMoveEvent, TSGetExplicitLink<CPlayerMoveEvent>>, #0x006F2B70, #0x006F2BE0, #0x006FAF40, #0x0074F4D0<InvalidatedName, TSGetLink<InvalidatedName>>, #0x0074F550<PendingCombatLogData, TSGetLink<PendingCombatLogData>>, #0x0074F640<CombatLogFilter, TSGetLink<CombatLogFilter>>, #0x00753810<CombatLogEntry, TSGetLink<CombatLogEntry>>, #0x00764420, #0x00771C20<HANDLER, TSGetLink<HANDLER>>, #0x00772EF0<CMDDEF, TSGetLink<CMDDEF>>, #0x00788B20<PosDelta, TSGetLink<PosDelta>>, #0x007CB680, #0x007FBC20, #0x00804D90<PendingSpellCast, TSGetLink<PendingSpellCast>>, #0x00804E00<PeriodicClientTrigger, TSGetLink<PeriodicClientTrigger>>, #0x00807650, #0x0081A530<EVENTLISTENERNODE, TSGetLink<EVENTLISTENERNODE>>, #0x0083D2D0<CM2SequencePlayBack, TSGetLink<CM2SequencePlayBack>>, #0x008489F0<POIIcon, TSGetLink<POIIcon>>, #0x00848A60<MAPDATA, TSGetLink<MAPDATA>>, #0x0086E850, #0x008C8550, #0x008C8640<BattlenetLogin::BN_EVENT, TSGetLink<BattlenetLogin::BN_EVENT>>, #0x0095B8E0<CLCDBASE_NODE, TSGetLink<CLCDBASE_NODE>>, #0x005F72D0, #0x00783780, #0x007982A0, #0x007CCAE0, #0x0079F2B0, #0x007C37C0, 0x007295A0<CRenderCallback, TSGetExplicitLink<CRenderCallback>>, 0x00729690<SObjectFadeout, TSGetExplicitLink<SObjectFadeout>>, 0x00781070<CWorldOccluder, TSGetExplicitLink<CWorldOccluder>>, 0x0074BCF0<Weather::PosDelta, TSGetExplicitLink<Weather::PosDelta>>, 0x0061BE00<CHARCODEDESC, TSGetExplicitLink<CHARCODEDESC>>, 0x00528140/0x1401775B0<CSimpleControlPoint, TSGetExplicitLink<CSimpleControlPoint>>, 0x00838650<CM2SequenceLoad, TSGetLink<CM2SequenceLoad>>, 0x00708C10<SE3SoundEmitter, TSGetLink<SE3SoundEmitter>>, 0x0070A7B0<SE3DoneLoadingMessage, TSGetLink<SE3DoneLoadingMessage>>, 0x0070A820<SE3MemorySoundEndedMessage, TSGetLink<SE3MemorySoundEndedMessage>>, 0x0070A890<SE3SoundEndedMessage, TSGetLink<SE3SoundEndedMessage>>, 0x0070A900<SE3WaveDataNode, TSGetLink<SE3WaveDataNode>>, 0x0070A940/0x1403D8B10<SE3CombinableSoundKitPosition, TSGetLink<SE3CombinableSoundKitPosition>>, 0x0070ABD0<SE3CreateSoundEmitterMessage, TSGetLink<SE3CreateSoundEmitterMessage>>, 0x0070AC10<SE3Set3DPositionMessage, TSGetLink<SE3Set3DPositionMessage>>, 0x0070AC80<SE3DestroySoundEmitterMessage, TSGetLink<SE3DestroySoundEmitterMessage>>, 0x0070ACF0<SE3MarkSoundChunkDirtyMessage, TSGetLink<SE3MarkSoundChunkDirtyMessage>>, 0x00540280<CAsyncObject, TSGetExplicitLink<CAsyncObject>>, 0x005402F0<CAsyncThread, TSGetExplicitLink<CAsyncThread>>, 0x00547330<FileNotification::CallbackInfo, TSGetExplicitLink<FileNotification::CallbackInfo>>, 0x00547510<FileNotification::ManifestEntry, TSGetExplicitLink<FileNotification::ManifestEntry>>, 0x008A1F40<EvtHandler, TSGetExplicitLink<EvtHandler>>, 0x008A1FC0<EvtMessage, TSGetExplicitLink<EvtMessage>>, 0x008A2030<EvtKeyDown, TSGetExplicitLink<EvtKeyDown>>, 0x008AE4C0<WEEKLYSPELLUSE, TSGetLink<WEEKLYSPELLUSE>>]
template <class T, class TGetLink>
void TSList<T, TGetLink>::Clear() {
    T* node;

    while ((node = this->Head()) != NULL) {
        this->DeleteNode(node);
    }
}

// OFFSET: [#0x004652A0<RealmConnection::REALMCONNECTIONNODE, TSGetLink<RealmConnection::REALMCONNECTIONNODE>>, #0x0047E600, #0x00480720<EvtHandler, TSGetExplicitLink<EvtHandler>>, #0x004807A0<EvtKeyDown, TSGetExplicitLink<EvtKeyDown>>, 0x004FC060<CSimpleEmbeddedTexture, TSGetExplicitLink<CSimpleEmbeddedTexture>>, #0x00489AD0, #0x00490D00, #0x0049A680<SIMPLEANIMNODE, TSGetLink<SIMPLEANIMNODE>>, #0x004D4FB0<CMirrorHandler, TSGetLink<CMirrorHandler>>, #0x005022E0<PENDINGUSERLIST, TSGetExplicitLink<PENDINGUSERLIST>>, #0x005030F0<PENDINGCHAT, TSGetExplicitLink<PENDINGCHAT>>, #0x00527B70<CapturePointUIManagerNode, TSGetLink<CapturePointUIManagerNode>>, #0x0053EF20, #0x0057AE00<AUTOCOMPLETE_NAMENODE, TSGetLink<AUTOCOMPLETE_NAMENODE>>, #0x005AE130<EQUIPMENT_SET_NODE, TSGetLink<EQUIPMENT_SET_NODE>>, #0x005B21E0<CLIENT_ACHIEVEMENT_CRITERIA, TSGetLink<CLIENT_ACHIEVEMENT_CRITERIA>>, #0x005B22D0<CLIENT_ACHIEVEMENTS, TSGetLink<CLIENT_ACHIEVEMENTS>>, #0x005B4C70<SPECIFIC_CRITERIA, TSGetLink<SPECIFIC_CRITERIA>>, #0x005B4D60<COMPLETED_QUESTS, TSGetLink<COMPLETED_QUESTS>>, #0x005BE7B0, #0x005EDA10<ENCOUNTER_TIMER, TSGetLink<ENCOUNTER_TIMER>>, #0x005EDA90<ENCOUNTER_OBJECTIVE, TSGetLink<ENCOUNTER_OBJECTIVE>>, #0x006064F0<CameraShake, TSGetLink<CameraShake>>, #0x006330F0<NetClient::HELDMESSAGE, TSGetLink<NetClient::HELDMESSAGE>>, #0x00633130, #0x0066FF60<DBCACHECALLBACK, TSGetLink<DBCACHECALLBACK>>, #0x006BDE00, #0x006F20A0<CObjectEffectRef, TSGetLink<CObjectEffectRef>>, #0x0074F5C0<PendingCombatLogData, TSGetLink<PendingCombatLogData>>, #0x0077E830<WNDREC, TSGetLink<WNDREC>>, #0x00788B90<PosDelta, TSGetLink<PosDelta>>, #0x007C3330, #0x007CB600<VBBList_Block, TSGetExplicitLink<VBBList_Block>>, #0x007D5CD0<FVBBList::Block, TSGetExplicitLink<FVBBList::Block>>, #0x007FC070<MountTransitionObject, TSGetLink<MountTransitionObject>>, #0x00804E70<PeriodicClientTrigger, TSGetLink<PeriodicClientTrigger>>, #0x0081A5A0<EVENTLISTENERNODE, TSGetLink<EVENTLISTENERNODE>>, #0x0082DB30<CM2SequenceLoad, TSGetLink<CM2SequenceLoad>>, #0x00831BB0<CM2SequencePlayBack, TSGetLink<CM2SequencePlayBack>>, #0x008C85C0, #0x008C86B0, #0x008CED10<Grunt::Pending::Data, TSGetExplicitLink<Grunt::Pending::Data>>, #0x0095AA30<CLCDConnection::CB_EVENT_NODE, TSGetLink<CLCDConnection::CB_EVENT_NODE>>, 0x005221B0/0x14016FF80<CONTENTNODE, TSGetLink<CONTENTNODE>>, #0x0079F100, 0x00729610<CRenderCallback, TSGetExplicitLink<CRenderCallback>>, 0x0074BD60<Weather::PosDelta, TSGetExplicitLink<Weather::PosDelta>>, 0x00716240/0x1403E92D0<SE3AsyncData, TSGetLink<SE3AsyncData>>, 0x00501680<UNEEDED_TEXTURES, TSGetLink<UNEEDED_TEXTURES>>, 0x00505990<SIMPLEFRAMENODE, TSGetLink<SIMPLEFRAMENODE>>, 0x00508F00/0x140150B40<CLayoutFrame::FRAMENODE, TSGetLink<CLayoutFrame::FRAMENODE>>, 0x008A3AA0<EvtKeyDown, TSGetExplicitLink<EvtKeyDown>>, 0x00499BD0<DBCALLBACK, TSGetLink<DBCALLBACK>>, 0x00625C80<RealmConnection::REALMCONNECTIONNODE, TSGetLink<RealmConnection::REALMCONNECTIONNODE>>, 0x008AE440<PeriodicClientTrigger, TSGetLink<PeriodicClientTrigger>>, 0x0094A5C0<CLIENT_ACHIEVEMENT_CRITERIA, TSGetLink<CLIENT_ACHIEVEMENT_CRITERIA>>, 0x0094DA60<COMPLETED_QUESTS, TSGetLink<COMPLETED_QUESTS>>]
template <class T, class TGetLink>
T* TSList<T, TGetLink>::DeleteNode(T* ptr) {
    T* next = this->Next(ptr);

    ptr->~T();
    SMemFree(ptr, typeid(T).raw_name(), SLOG_OBJECT, 0);

    return next;
}

// PROVEN: 89d57c33 IDENTICAL/NONE
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

// OFFSET: [#0x004F7180<CModelRecord, TSGetLink<CModelRecord>>]
template <class T, class TGetLink>
int32_t TSList<T, TGetLink>::IsEmpty() {
    return this->Head() == NULL;
}

// PROVEN: 9b79e303 IDENTICAL/NONE
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

// OFFSET: [%0x004A3690<CONSOLELINE, TSGetLink<CONSOLELINE>>, %0x006A49B0]
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
        link->m_prevLink = existinglink->m_prevLink;
        link->m_next = existinglink->m_prevLink->m_next;
        existinglink->m_prevLink->m_next = ptr;
        existinglink->m_prevLink = link;
    }
}

// OFFSET: [%0x006185F0<ProfileInternal::STRINGBLOCK, TSGetLink<ProfileInternal::STRINGBLOCK>>, %0x00698960<CMapChunkBufBlock, TSGetExplicitLink<CMapChunkBufBlock>>]
template <class T, class TGetLink>
void TSList<T, TGetLink>::LinkToHead(T* ptr) {
    this->LinkNode(ptr, 1, NULL);
}

// OFFSET: [%0x005C3420<CGChatBubbleFrame, TSGetLink<CGChatBubbleFrame>>, %0x006A31F0<DBCache<GuildStats_C, int, HASHKEY_INT>::DBCACHEHASH, TSGetExplicitLink<DBCache<GuildStats_C, int, HASHKEY_INT>::DBCACHEHASH>>, %0x008DBB60<WowConnection::SENDNODE, TSGetLink<WowConnection::SENDNODE>>, #0x008795F0]
template <class T, class TGetLink>
void TSList<T, TGetLink>::LinkToTail(T* ptr) {
    this->LinkNode(ptr, 2, NULL);
}

// OFFSET: [#0x00496C00<SIMPLEFONT, TSGetExplicitLink<SIMPLEFONT>>, #0x004B6E40, #0x004BFFC0, #0x004F3420<CACHEENTRY, TSGetExplicitLink<CACHEENTRY>>, #0x004F89E0, #0x0055A1D0<LFGListParty, TSGetExplicitLink<LFGListParty>>, 0x00924560<OVERRIDEKEYBINDING, TSGetExplicitLink<OVERRIDEKEYBINDING>>, #0x005BDF00, #0x006329D0, #0x0066CCB0<DBCACHECALLBACK, TSGetLink<DBCACHECALLBACK>>, #0x006705D0, #0x00671030, #0x006883B0<CGxShader, TSGetExplicitLink<CGxShader>>, #0x006BBDB0<CGxuLight, TSGetLink<CGxuLight>>, #0x0074F2D0<CombatLogEntry, TSGetLink<CombatLogEntry>>, #0x00776BC0<RGN, TSGetExplicitLink<RGN>>, #0x007FBC60<LightningObject, TSGetLink<LightningObject>>, #0x007FBCE0<MountTransitionObject, TSGetLink<MountTransitionObject>>, #0x00783100, 0x0077BDB0<CSceneMaterial, TSGetExplicitLink<CSceneMaterial>>, 0x005E3420/0x140261EC0<OBJ_EFFECT_DEF_LOOKUP, TSGetExplicitLink<OBJ_EFFECT_DEF_LOOKUP>>, 0x0061FA80<CONSOLELINE, TSGetExplicitLink<CONSOLELINE>>, 0x0061C380<CHARCODEDESC, TSGetExplicitLink<CHARCODEDESC>>, 0x0061C840<KERNNODE, TSGetExplicitLink<KERNNODE>>]
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

// INLINED
template <class T, class TGetLink>
T* TSList<T, TGetLink>::Prev(T* ptr) {
    return this->Link(ptr)->Prev();
}

// OFFSET: [%0x0055A730, %0x00495F40]
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

// OFFSET: [%0x00804990, #0x0056CA70, %0x00797880<CSimpleRegion, TSGetExplicitLink<CSimpleRegion>>, %0x0055BD00<Grunt::Timer::Event, TSGetExplicitLink<Grunt::Timer::Event>>, #0x00879980, 0x009F5DF0<CGResearchFrame::PROJECT, TSGetLink<CGResearchFrame::PROJECT>>]
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

// OFFSET: [%0x0059A190<CLayoutFrame, TSGetExplicitLink<CLayoutFrame>>, %0x0080F750<CModelRecord, TSGetLink<CModelRecord>>, #0x00723300]
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

// OFFSET: 0x00601810
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

// PROVEN: 0184908c IDENTICAL/NONE
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

// OFFSET: [#0x0047C630, #0x0047C6C0, 0x00505860/0x14014C370<TSExplicitList<FRAMEATTR, -572662307>>, #0x004967E0, #0x004B74F0<CTexture*>, #0x004BAFD0, #0x004BC2F0, #0x004BC380, #0x004BE2D0, #0x004C0260, #0x004C9520<TSExplicitList<TIMESTAMPSTRUCT, -572662307>>, #0x004CA720, #0x004CC6F0, #0x004CC780, #0x004CDDD0, #0x004CF070, #0x004D0A70, #0x004D3130, #0x004D33C0, #0x004D4E10<TSExplicitList<CGObject_C, -572662307>>, #0x004F31F0, #0x00502360<PENDINGUSERLIST>, #0x005032A0, #0x00503330, #0x005033C0, #0x00503450, #0x005053E0, #0x00527BF0, #0x00527C80, #0x00527E70, #0x0052F100, #0x0053F160, #0x00546FE0<WorldMapContinentInfo>, #0x00548B70, #0x00558C50<LFGListParty*>, #0x00558CE0<LFGListPlayer*>, #0x0055F560<TSExplicitList<KEYCOMMAND, -572662307>>, #0x0055F5F0<TSExplicitList<KEYBINDING, -572662307>>, #0x0055F680<TSExplicitList<OVERRIDEKEYBINDING, -572662307>>, #0x0055F710<TSExplicitList<MODIFIEDCLICK, -572662307>>, #0x0055F7A0, #0x00566700, #0x00575F60, #0x005920F0, #0x005B4DE0, #0x005B4E70, #0x005BE830, #0x005BE8C0, #0x005C7290<TSExplicitList<TalentGroupItem_C, -572662307>>, #0x005C7320<TSExplicitList<TalentGroupTierItem_C, -572662307>>, #0x005C91F0<TalentGroupTab_C>, #0x005D24E0, #0x005E6880<QuestPOIBlob>, #0x005F03B0, #0x005F3E20, #0x005F5AD0, #0x005F5B60<TSExplicitList<ADDONSTATE, -572662307>>, #0x005F5BF0, #0x005FCB80<TSExplicitList<MOUSELOOKBINDING, -572662307>>, #0x00617AA0, #0x00617B30, #0x00617BC0, #0x0066E830, #0x0066E8C0, #0x0066E950, #0x0066E9E0, #0x0066EA70, #0x0066EB00, #0x0066EB90, #0x0066EC20, #0x0066ECB0, #0x0066ED40, #0x0066EDD0, #0x0066EE60, #0x0066EEF0, #0x0066EF80, #0x0066F010, #0x0066F0A0, #0x0066F130, #0x0066F1C0, #0x0066F250, #0x0066F2E0, #0x0066F370, #0x0066F400, #0x0066F490, #0x0066F520, #0x0066F5B0, #0x0066F640, #0x0066F6D0, #0x0066F760, #0x0066F7F0, #0x0066F880, 0x00848470/0x140575D20<TSExplicitList<CGxShader, -572662307>>, #0x006BBE30, #0x006BEE90, #0x006C2650<TSExplicitList<CHARCODEDESC, -572662307>>, #0x006C2AC0<TSExplicitList<KERNNODE, -572662307>>, #0x006C2B50, #0x006C8380, #0x006E4EB0, #0x006E4F40, #0x006F2C50, #0x006F2CE0, #0x006F2D70<TSExplicitList<OBJ_EFFECT_GROUP_LOOKUP, -572662307>>, #0x006F2E00<TSExplicitList<OBJ_EFFECT_REF_LIST_LOOKUP, -572662307>>, #0x006F2E90, #0x006F2F20, #0x006F2FB0, #0x00709AA0, #0x00709B30, #0x007131B0, #0x0072FFC0<TSExplicitList<UnitThreat, -572662307>>, #0x007675F0, #0x007685D0, #0x00776AD0<TSExplicitList<RGN, -572662307>>, #0x007B04D0, #0x007B5DF0, #0x007C0B40, #0x007C0BD0<CMapObjDef*>, #0x007DD360, #0x007DD3F0, #0x007E01C0, #0x007F2A20, #0x007F4ED0, #0x00804EF0, #0x00804F80, #0x008109F0, #0x00810A80, #0x00810B10, #0x00812BC0, #0x00812C50, #0x00814910<CXMLAttribute>, #0x0081A620<FrameScript_EventObject*>, #0x00848800, #0x0084B570, #0x00876550, #0x00877760<TSExplicitList<CacheData, -572662307>>, #0x0087B0F0, #0x00965260<CSimpleEditBox::EditHistory>, #0x009681C0, #0x009694A0<CSimpleMessageScrollFrameLine>, #0x009694F0, #0x0097BB80<CParticleEmitter2*>, #0x0040A790, #0x00848AD0<CHATMESSAGE>, #0x004E3080<CharacterSelectionDisplay>, #0x00591ED0, #0x00591F20, #0x005F01E0, #0x0056BBA0<CGCommentator::MapInfo>, #0x007A0160<CMapFootprintTexture>, #0x0079D4D0, 0x0071D0A0<TSExplicitList<Liquid::Object, -572662307>>, 0x0064A8D0<TSExplicitList<DayNight::DNOverrideSky, -572662307>>, 0x00652930<DayNight::CZoneLight>, 0x0072E450<TSExplicitList<CMapDoodadDef, -572662307>>, 0x0072E4E0<TSExplicitList<CMapObjDef, -572662307>>, 0x0075EB00/0x140442990<CMapFootprintTexture>, 0x00765AD0<CWaterRipple>, 0x00735EC0/0x140412460<TSExplicitList<CMapDynamicTexture>>, 0x0077BC70<TSExplicitList<CSceneMaterial, -572662307>>, 0x00747B80<TSExplicitList<CMapObj, -572662307>>, 0x007398F0<TSExplicitList<DDMaterial, -572662307>>, 0x00446E30<TSExplicitList<_BANNEDADDON, -572662307>>, 0x004ED0A0<TSExplicitList<FrameFactoryNode, -572662307>>, 0x00526060<TSExplicitList<SIMPLEFONT, -572662307>>, 0x00542720<TSExplicitList<CTexture, -572662307>>, 0x00547580<TSExplicitList<FileNotification::DeferredFile, -572662307>>, 0x00611370/0x14029BB20<TSExplicitList<CLightList, -572662307>>, 0x00B314A0<TSExplicitList<LCDHANDLE, -572662307>>, 0x00548450/0x1401A0720<TSExplicitList<CModelBlob::CHashEntry, -572662307>>, 0x005E3A90/0x140262580<TSExplicitList<OBJ_EFFECT_PACKAGE_DEF_LOOKUP, -572662307>>, 0x00434820<TSExplicitList<CShaderEffect, -572662307>>, 0x0054BA70/0x1401A5120<TSExplicitList<CTextureBlobTexture, -572662307>>, 0x00708C50<TSExplicitList<SE3SoundChunk, -572662307>>, 0x0070A1E0<TSExplicitList<SE3SOUNDKITOBJECT_LOOKUP, -572662307>>, 0x00709D50<TSExplicitList<SOUNDKITLOOKUP, -572662307>>, 0x00709DE0<TSExplicitList<SE3SoundEmitterLookup, -572662307>>, 0x0070A150/0x1403D8D90<TSExplicitList<SE3COMBINABLESOUNDLIST, -572662307>>, 0x00712D20<TSExplicitList<SE3ChatSound, -572662307>>, 0x00714B60<TSExplicitList<SE3MemorySoundObject, -572662307>>, 0x004FC260/0x140140420<TSExplicitList<FILENAMEHASH, -572662307>>, 0x00512A40<CAsyncObject*>, 0x00855260<TSExplicitList<CGxDeviceD3d11::InputLayoutRec, -572662307>>, 0x008552F0<TSExplicitList<CGxDeviceD3d11::RasterizerStateRec, -572662307>>, 0x00855380<TSExplicitList<CGxDeviceD3d11::BlendStateRec, -572662307>>, 0x00855410<TSExplicitList<CGxDeviceD3d11::DepthStateRec, -572662307>>, 0x008554A0<TSExplicitList<CGxDeviceD3d11::SamplerStateRec, -572662307>>, 0x00612380<GxuDebugTextObject>, 0x006123D0<GxuDebugText3DObject>, 0x00547610<TSExplicitList<FileNotification::Manifest, -572662307>>, 0x00549B90<TSExplicitList<FONTHASHOBJ, -572662307>>, 0x004ED010<TSExplicitList<HashedNode, -572662307>>, 0x006545F0<TSExplicitList<CONSOLECOMMAND, -572662307>>, 0x00656450<TSExplicitList<CVar, -572662307>>, 0x0083E4E0<TSExplicitList<FrameScript_EventObject, -572662307>>, 0x005A3160<ShipPathLeg>, 0x009F28F0<TSExplicitList<CGSpellActivationOverlay::COUNTERNODE, -572662307>>, 0x009F2980<TSExplicitList<CGSpellActivationOverlay::PETAURANODE, -572662307>>, 0x009F6160<TSExplicitList<CGResearchFrame::BRANCH, -572662307>>, 0x009F61F0<TSExplicitList<CGResearchFrame::BLOBPOIHASH, -572662307>>, 0x009C9C50<TSExplicitList<CGCUFProfiles::PROFILENODE, -572662307>>, 0x009C9CE0<TSExplicitList<CGCUFProfiles::OPTIONENTRY, -572662307>>, 0x00439BB0<TSExplicitList<CCameraModeEntry, -572662307>>, 0x0045E270<TSExplicitList<UIADDON::METADATA, -572662307>>, 0x00483C90<TSExplicitList<PLAYERPORTRAIT, -572662307>>, 0x00483D20<TSExplicitList<UNITPORTRAIT, -572662307>>, 0x00483DB0<TSExplicitList<ITEMPORTRAIT, -572662307>>, 0x0048C2D0<TSExplicitList<WowClientDB2_Base::DBTABLE, -572662307>>, 0x0048E620<TSExplicitList<CGObject_C, -572662307>>, 0x00495C50<TSExplicitList<SPELLACTIVATIONINFO, -572662307>>, 0x004A2500<TSExplicitList<DBCache<CreatureStats_C,int,HASHKEY_INT>::DBCACHEHASH, -572662307>>, 0x004A2590<TSExplicitList<DBCache<CreatureStats_C,int,HASHKEY_INT>::REVERSEENTRY, -572662307>>, 0x004A2620<TSExplicitList<DBCache<GameObjectStats_C,int,HASHKEY_INT>::DBCACHEHASH, -572662307>>, 0x004A26B0<TSExplicitList<DBCache<GameObjectStats_C,int,HASHKEY_INT>::REVERSEENTRY, -572662307>>, 0x004A2740<TSExplicitList<DBCache<NPCText,int,HASHKEY_INT>::DBCACHEHASH, -572662307>>, 0x004A27D0<TSExplicitList<DBCache<NPCText,int,HASHKEY_INT>::REVERSEENTRY, -572662307>>, 0x004A2860<TSExplicitList<DBCache<NameCache,unsigned __int64,CHashKeyGUID>::DBCACHEHASH, -572662307>>, 0x004A28F0<TSExplicitList<DBCache<NameCache,unsigned __int64,CHashKeyGUID>::REVERSEENTRY, -572662307>>, 0x004A2980<TSExplicitList<DBCache<GuildStats_C,unsigned __int64,CHashKeyGUID>::DBCACHEHASH, -572662307>>, 0x004A2A10<TSExplicitList<DBCache<GuildStats_C,unsigned __int64,CHashKeyGUID>::REVERSEENTRY, -572662307>>, 0x004A2AA0<TSExplicitList<DBCache<QuestCache,int,HASHKEY_INT>::DBCACHEHASH, -572662307>>, 0x004A2B30<TSExplicitList<DBCache<QuestCache,int,HASHKEY_INT>::REVERSEENTRY, -572662307>>, 0x004A2BC0<TSExplicitList<DBCache<PageTextCache_C,int,HASHKEY_INT>::DBCACHEHASH, -572662307>>, 0x004A2C50<TSExplicitList<DBCache<PageTextCache_C,int,HASHKEY_INT>::REVERSEENTRY, -572662307>>, 0x004A2CE0<TSExplicitList<DBCache<PetNameCache,int,HASHKEY_INT>::DBCACHEHASH, -572662307>>, 0x004A2D70<TSExplicitList<DBCache<PetNameCache,int,HASHKEY_INT>::REVERSEENTRY, -572662307>>, 0x004A2E00<TSExplicitList<DBCache<CGPetition,int,HASHKEY_INT>::DBCACHEHASH, -572662307>>, 0x004A2E90<TSExplicitList<DBCache<CGPetition,int,HASHKEY_INT>::REVERSEENTRY, -572662307>>, 0x004A2F20<TSExplicitList<DBCache<ItemTextCache_C,unsigned __int64,CHashKeyGUID>::DBCACHEHASH, -572662307>>, 0x004A2FB0<TSExplicitList<DBCache<ItemTextCache_C,unsigned __int64,CHashKeyGUID>::REVERSEENTRY, -572662307>>, 0x004A3040<TSExplicitList<DBCache<WardenCachedModule,CWardenKey,CWardenKey>::DBCACHEHASH, -572662307>>, 0x004A30D0<TSExplicitList<DBCache<WardenCachedModule,CWardenKey,CWardenKey>::REVERSEENTRY, -572662307>>, 0x004A3160<TSExplicitList<DBCache<ArenaTeamCache,int,HASHKEY_INT>::DBCACHEHASH, -572662307>>, 0x004A31F0<TSExplicitList<DBCache<ArenaTeamCache,int,HASHKEY_INT>::REVERSEENTRY, -572662307>>, 0x004A3280<TSExplicitList<DBCache<DanceCache,int,HASHKEY_INT>::DBCACHEHASH, -572662307>>, 0x004A3310<TSExplicitList<DBCache<DanceCache,int,HASHKEY_INT>::REVERSEENTRY, -572662307>>, 0x004C5660<REALM_INFO>, 0x004DE130<TSExplicitList<TALKINGPLAYER, -572662307>>, 0x004DE1C0<TSExplicitList<MUTEDPLAYER, -572662307>>, 0x004E5760<TSExplicitList<DECLINEDWORD, -572662307>>, 0x00545790<TSExplicitList<ProfileInternal::KEYVALUE, -572662307>>, 0x005696A0<TSExplicitList<TRADESKILLLINE, -572662307>>, 0x00569730<TSExplicitList<PROFICIENCYSPELL, -572662307>>, 0x0058F500<TSExplicitList<CustomFallbackPath, -572662307>>, 0x00595C80<TSExplicitList<VoidItem_C, -572662307>>, 0x005A6670<TSExplicitList<INVENTORYART, -572662307>>, 0x005A6700<TSExplicitList<ITEMBYNAME, -572662307>>, 0x005CF4F0<TSExplicitList<UnitThreat, -572662307>>, 0x005E20D0<TSExplicitList<OBJ_EFFECT_GROUP_LOOKUP, -572662307>>, 0x005E2160<TSExplicitList<OBJ_EFFECT_REF_LIST_LOOKUP, -572662307>>, 0x005E21F0<TSExplicitList<OBJ_EFFECT_DEF_LOOKUP, -572662307>>, 0x005E2280<TSExplicitList<OBJ_EFFECT_GROUP_DEF_LOOKUP, -572662307>>, 0x005E2310<TSExplicitList<OBJ_EFFECT_PACKAGE_DEF_LOOKUP, -572662307>>, 0x005E2D80<TSExplicitList<OBJ_EFFECT_REF_LOOKUP, -572662307>>, 0x0061BE40<TSExplicitList<CHARCODEDESC, -572662307>>, 0x0061C1B0<TSExplicitList<KERNNODE, -572662307>>, 0x0062CA40<TSExplicitList<ZONESOUNDOVERRIDELOOKUP, -572662307>>, 0x0062EE00<AmbFlavorListHead>, 0x00632FA0<TSExplicitList<DSP_CHAIN_HEAD_LOOKUP, -572662307>>, 0x00633C60<TSExplicitList<FOOTSTEPSNDCACHE, -572662307>>, 0x006ABCD0<LFGuildApplicationData>, 0x006C8110<JamGuildRewardItem>, 0x006C82C0<LFGuildBrowseData>, 0x006C82F0<LFGuildRecruitData>, 0x007043F0<TSExplicitList<CACHEENTRY, -572662307>>, 0x00792D40<TSExplicitList<CFileName, -572662307>>, 0x00809540<CharacterSelectionDisplay>, 0x008AE5F0<TSExplicitList<ITEMCOOLDOWNHASHNODE, -572662307>>, 0x008AE680<TSExplicitList<DESTLOCSPELLCASTINDEXHASHNODE, -572662307>>, 0x008C0A80<TSExplicitList<RACECLASSINFO, -572662307>>, 0x008D8C20<TSExplicitList<SAVEDVARIABLE, -572662307>>, 0x008D8CB0<TSExplicitList<TaintEvent, -572662307>>, 0x008EF5C0<TSExplicitList<CHATTHROTTLE, -572662307>>, 0x008EF650<TSExplicitList<RegisteredAddonPrefix, -572662307>>, 0x008EF6E0<TSExplicitList<WORDLIST, -572662307>>, 0x008EF770<TSExplicitList<PROFANITYCHECK, -572662307>>, 0x008EF800<TSExplicitList<SPAMCHECK, -572662307>>, 0x00900B50<TSExplicitList<USEABLESTYLE, -572662307>>, 0x00909DD0<TSExplicitList<ACCOUNTIDCACHE, -572662307>>, 0x00913F00<TSExplicitList<SPELLSBYNAME, -572662307>>, 0x00913FA0<TSExplicitList<OVERRIDE_SPELL_NODE, -572662307>>, 0x00914030<TSExplicitList<OVERRIDE_SPELL_BY_NAME_NODE, -572662307>>, 0x00935010<GuildNewsEvent>, 0x00939A10<TSExplicitList<CGGuildInfo::GuildRecipesKnown, -572662307>>, 0x00939AA0<TSExplicitList<CGGuildInfo::SKILLLINESPELLBITS, -572662307>>, 0x0093AE60<TSExplicitList<CGGuildInfo::SKILLLINESPELLS, -572662307>>, 0x0094DAE0<TSExplicitList<ACHIEVEMENT, -572662307>>, 0x0094DB70<TSExplicitList<GUILDACHIEVEMENTMEMBERLIST, -572662307>>, 0x00964900<TSExplicitList<CGQuestLog::QUESTCOMPLETIONNPC, -572662307>>, 0x00987C60<TSExplicitList<LFGListParty, -572662307>>, 0x00987CF0<TSExplicitList<LFGListPlayer, -572662307>>, 0x009A48C0<TSExplicitList<CFlyout, -572662307>>, 0x009A4950<TSExplicitList<SpellToFlyoutMapItem, -572662307>>, 0x009A8510<TSExplicitList<CGReputationInfo::FACTION, -572662307>>, 0x009B85A0<TSExplicitList<KNOWNDANCES, -572662307>>, 0x009C6060<TSExplicitList<MACRONODE, -572662307>>, 0x009CC140<TSExplicitList<CGAutoCompleteName::AUTOCOMPLETE_CHARACTER_HASHNODE, -572662307>>, 0x009CC1D0<TSExplicitList<CGAutoCompleteName::AUTOCOMPLETE_BATTLENET_HASHNODE, -572662307>>, 0x009CDD20<TSExplicitList<WORLDSTATE, -572662307>>, 0x009DEBB0<TSExplicitList<CGQuestObjectiveCache::REQUIREDUNITNODE, -572662307>>, 0x009DEC40<TSExplicitList<CGQuestObjectiveCache::REQUIREDITEMNODE, -572662307>>, 0x009E4270<TSExplicitList<TAXIPATHNODESCLIENT, -572662307>>, 0x009FD330<TSExplicitList<CGIncomingResurrection::INCOMINGRESLIST, -572662307>>, 0x00A0EA00<TSExplicitList<MacroConditions, -572662307>>, 0x00B32970<TSExplicitList<BITMAPBYICON, -572662307>>, 0x0045BBA0<const char*>, 0x0098EB90<const SkillLineAbilityRec*>, 0x004E52C0<CPhaseShift>, 0x0069F240<JamCliStructAddonPrefix>]
template <class T>
TSFixedArray<T>::~TSFixedArray() {
    for (uint32_t i = 0; i < this->m_count; i++) {
        this->m_data[i].~T();
    }

    if (this->m_data) {
        SMemFree(this->m_data, this->MemFileName(), this->MemLineNo(), 0);
    }
}

// OFFSET: [#0x00409770<int>, #0x004D9FB0<char>, #0x005918E0, #0x00591960, #0x00879950<SEChannelGroup>, #0x008C6E30, 0x00620A70<CImVector>, 0x0060D8A0<DEATTHUDSOUNDINFO>, 0x0040B930<int>, 0x0046A5B0<CSimpleFontString*>, 0x00616CE0<C3Vector>, 0x006251F0<CHARACTER_INFO>, 0x0064E440<float>, 0x0064E480<C2Vector>, 0x009E33C0<TAXILINE>, 0x009E3400<TAXIROUTELINE>, 0x00B2BBE0<LightningJoint>, 0x004824E0<uint8_t>, 0x004955C0<uint32_t>]
template <class T>
void TSFixedArray<T>::Clear() {
    this->~TSFixedArray<T>();
    this->m_alloc = 0;
    this->m_count = 0;
    this->m_data = NULL;
}

// PROVEN: f5436f46 IDENTICAL/NONE
// OFFSET: [0x00486890<CObjectHeap>]
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

// OFFSET: [#0x00403760<char*>, #0x004037F0<SArchive*>, #0x00408340<DYNAMICELEMENTVERT>, #0x004083F0, #0x00408490, #0x00408D80<int>, #0x004644C0<CHARACTER_INFO>, #0x004657D0<REALM_INFO>, 0x00554F00<unsigned int>, #0x0047CCC0<void*>, #0x0047D9F0<SThread*>, #0x0047DA80<EvtContext*>, #0x00480EE0<EvtTimer*>, #0x004827C0<CSimpleBatchedMesh>, #0x00490A80, #0x00493A40<CFrameStrataNode*>, #0x00493AD0<FRAMEPRIORITY*>, #0x00496680, #0x00497970<CSimpleControlPoint*>, #0x00497A00<TSList<SIMPLEANIMNODE, TSGetLink<SIMPLEANIMNODE>>*>, #0x004B7580, #0x004B9990<void (__cdecl*)(void)>, #0x004B9A20<int (__cdecl*)(void)>, #0x004BB060, #0x004BBDA0, #0x004BBF00, #0x004BC060, #0x004BE170, #0x004BEBB0<CBaseManaged*>, #0x004C00B0, 0x009CEF10<C3Vector>, #0x004C4750<float>, #0x004CA380, #0x004CB930<VSOUNDKITDEF*>, #0x004CB9C0<WorldStateZoneSoundOverride*>, #0x004CC360, #0x004CC4C0, #0x004CDB20, #0x004CEA00, #0x004CEAA0, #0x004CEF00, #0x004D0910<IMPACTSOUNDARRAY>, #0x004D2D90<CObjectHeap>, #0x004D34A0<CObjectHeapList>, #0x004D4FF0, #0x004D85A0<char const*>, #0x004DE520<RealmCategory*>, #0x004E0EC0<ChrClassesRec*>, #0x004E25C0<CharacterSelectionDisplay>, #0x004E6660<DictionaryRecord>, #0x004F2FB0, #0x004FCFC0<ChatColorInfo>, #0x004FD050<RaidInstanceInfo>, #0x004FD150, #0x004FFCE0<regex_t>, #0x005014A0<PENDINGUSERLIST>, #0x005023E0, #0x00502540, #0x005026A0, #0x00502800, #0x005034E0<ChatChannel>, #0x00514100<NearestUnitData>, #0x005141A0, #0x0051A2E0<unsigned __int64>, #0x00525720, #0x00525880, #0x00527D10, #0x0052EF00, #0x0053B790<SpellBookSkillInfo*>, #0x0053B820<SpellUsableInfo>, #0x0053EFA0, #0x005436A0<WorldMapLandmarkInfo>, #0x00547C20<WorldMapContinentInfo>, #0x00548640, #0x005489C0, #0x0054EC50<C2iVector>, #0x0054ECF0<C3iVector>, #0x00553220<CGLookingForGroup::LFGEntryInfo>, #0x005532C0, #0x00553350, #0x005533E0, #0x00558710, #0x00558870, #0x0055EAD0, #0x0055EC30, #0x0055ED90, #0x0055EEF0, #0x0055F050, #0x00564CE0<MacroCommand>, #0x00566790, #0x005685D0<CGCommentator::QueueNode>, #0x0056BAD0<CGCommentator::InstanceInfo>, #0x0056BD40<CGCommentator::MapInfo>, #0x0056D780<StationeryInfo*>, #0x0056D810<MailboxMessageInfo*>, #0x00575DC0, #0x00577410, #0x0057C120<POIINFO>, #0x0057C1B0<OBJINFO>, #0x0057E360<C2Vector>, #0x0057E400<unsigned short>, #0x0058E030<QuestPOIPoint>, #0x0058F8F0<CGQuestPOIFrame::QuestPOIRenderData>, #0x005907A0, #0x00590870<TSGrowableArray<TAXIROUTELINE>>, #0x00590920, #0x005909C0<TAXILINE>, #0x00590A60, #0x00591F70, #0x00592180, #0x00592290, #0x00593FC0<TrainerServiceInfo*>, #0x00594050<TrainerSkillLineInfo*>, #0x00599030<CImVector>, #0x0059B150<AuctionItemInfo>, #0x0059B1F0<OutbidItemInfo*>, #0x005A2AF0, #0x005A46F0, #0x005B49B0, #0x005B4B10, #0x005B8C80<CalendarDayEvent*>, #0x005BDF60, #0x005BE0C0, #0x005C58D0<TalentTabInfo*>, #0x005C5960<TalentGroupTier_C*>, #0x005C6C60, #0x005C6DC0, #0x005C9080<TalentGroupTab_C>, #0x005CA3D0<GuildMemberInfo*>, #0x005CD240<SkillLineInfo*>, #0x005CD2D0<SkillTypeInfo*>, #0x005CEB50<PetitionSignerInfo>, #0x005CFFB0, #0x005D0080, #0x005D0110, #0x005D2360, #0x005D3260, #0x005DA3F0<TradeSkillInfo*>, #0x005DA480<TradeSkillSubClassInfo*>, #0x005DA510, #0x005E6E40, #0x005E7940, #0x005ED620<ENCOUNTER_UNIT>, #0x005EEB80<MacroSubstring>, #0x005F0240, #0x005F0440<MacroCondition>, #0x005F1420<EventInviteInfo*>, #0x005F39B0, #0x005F4E30, #0x005F5020, #0x005F5180, #0x005F52E0, #0x005FC9C0, #0x005FEF70<CFacet>, #0x0060A1C0<UniqueSignal>, #0x0060A260<VehicleUIIndSeatRec*>, #0x006152E0<CRect>, #0x006169A0, #0x00616C20, #0x00617280, #0x006173E0, #0x00617540, #0x0061BD20<CSimpleFontString*>, #0x0061BDB0<FrameStackInfo>, #0x00634790, #0x00634820, #0x00669EB0, #0x0066CD10, #0x0066CE70, #0x0066CFD0, #0x0066D130, #0x0066D290, #0x0066D3F0, #0x0066D550, #0x0066D6B0, #0x0066D810, #0x0066D970, #0x0066DAD0, #0x0066DC30, #0x0066DD90, #0x0066DEF0, #0x0066E050, #0x00681800<GxDrawListEntry>, 0x00843DE0/0x140570350<CGxFormat>, #0x00683760<CGxAppRenderState>, #0x00683810<CGxStateBom>, #0x006838B0<CGxPushedRenderState>, #0x00683960<enum EGxRenderState>, #0x00688040, 0x0084A9A0<CGxMonitorMode>, #0x006A54A0<CGxDeviceD3d::GxVertexDecl>, #0x006A9440<CGxDeviceD3d9Ex::GxVertexDecl>, #0x006B1EF0<Grunt::CdKey::Info>, #0x006BA590, #0x006BBBE0, #0x006C26E0, #0x006C2840, 0x00622280<VERT>, #0x006C4E20, 0x0061DA90<GXUFONTHYPERLINKINFO>, 0x0061DB40<GXUEMBEDDEDTEXTUREINFO>, 0x00615AF0<GRADIENTINFO>, #0x006C8220, #0x006C9DF0<TEXTURECACHEROW>, #0x006CF4A0<InitialSpellStruct>, #0x006CF540<GUILDUPDATETIMESTAMP>, 0x00555050<ITEMSWAP>, #0x006E4BF0, #0x006E4D50, #0x006F2120, #0x006F2280, #0x006F23E0, #0x006F2540, #0x006F26A0, #0x006F2800, #0x006F2960, 0x005852C0<SpellCastTargetPoint>, #0x00709580, #0x007096E0, 0x005AC6C0<DelayedSpellVisualKit>, 0x005ACA20<AuraData>, #0x007163B0<SortedAuraInfo>, #0x0072FE60, #0x00746400<DEATTHUDSOUNDINFO>, #0x00747C10<CVehiclePassenger_C*>, #0x00747CA0<VehiclePassengerRescueTransition>, #0x007583D0<SimpleScriptFunction>, #0x00767490, #0x00768460, #0x00769D80<void (__cdecl*)(enum OPTION_PAGE)>, #0x0076D060, #0x007760A0<_SOURCE>, #0x00776950, #0x00786B90<Mists::Mist>, #0x00790250<CMapObjDefGroup*>, #0x007902E0<CGxVertexPCT0T1>, #0x00790370, #0x007917D0<CGxVertexPC>, #0x00791870<CPortalView>, #0x0079B370, #0x0079BA30<World::LiquidQueryResult>, #0x0079D510<CWaterRipple>, #0x0079F650<CGxVertexPT0T1>, #0x007A01C0<CMapFootprintTexture>, #0x007B0370, #0x007B0D60<CDetailDoodadInstAdd>, #0x007B0DF0<CDetailDoodadData*>, #0x007B0E80<CGxBuf*>, #0x007B4F10<CMapLight*>, #0x007BA120<CMapChunkBufBlock>, #0x007BDFE0, #0x007BE140, #0x007C30B0<CMapAreaTexture>, #0x007CC020, #0x007CCBE0<C4Plane>, #0x007CCC80<CClipVolume>, #0x007CDEC0<CChunkLiquid*>, #0x007DBB10<ComSatCallback>, #0x007DBBC0<ComSatEvent>, #0x007DD0A0, #0x007DD200, #0x007E0050, #0x007E6600, #0x007E7FF0<FFX::Pass*>, #0x007EBEA0, #0x007ECFF0<DayNight::AreaLightOverride>, #0x007F1F80, #0x007F3BF0<AreaPOIRec*>, #0x007F3D70<POIDIRECTIONDATA>, #0x007F4CF0, #0x007F78F0, #0x007F79C0, #0x007F8E30<ShipPathLeg>, 0x008BC2E0<LightningObject::Bolt>, 0x008BC390<TSGrowableArray<const SpellVisualKitModelAttachRec*>*>, 0x008BC430<const SpellVisualKitModelAttachRec*>, #0x00804830, #0x00804990, 0x008C0470<SkillLineLinkInfo>, #0x00810500, #0x00810660, #0x008107C0, #0x008128D0, #0x00812A30, #0x00814800<CXMLAttribute>, #0x008196A0, #0x0081A2F0, #0x0081CDC0<M2Element>, #0x00824660<CRibbonMat>, #0x00824700<M2ModelCallback>, #0x00848890<CHATMESSAGE>, #0x0084B140, #0x0084B2A0, #0x008555E0<MemChunk*>, 0x0087B960<OsIMECandidate>, #0x0086E8E0, 0x0087A510<W32Joystick>, #0x00876370, #0x00878E90<SEDriverInfo>, #0x00878F30<SEChannelGroup>, #0x0087AEB0, #0x00887490<ComSat::Sound::OutputChannel*>, #0x008A17D0<Liquid::CMaterialSettings*>, #0x008A1860<Liquid::IMaterial*>, #0x008A18F0<Liquid::CInstance*>, #0x00959260<CEzLcdPage*>, 0x00523FC0<MOVIECAPTION>, #0x009633C0, #0x00967E50<MessageData>, #0x00967EF0<CSimpleMessageFrameLine>, #0x009690F0<CSimpleMessageScrollFrameLine>, %0x0052CE00<CSimpleMessageScrollFrameDisplayNode>, #0x00978EC0<CParticle2>, #0x0097BBE0<CParticle2_Model>, #0x0097F650, 0x00B13AE0<CParticleEmitter2*>, #0x009A80F0<GxuSceneObject*>, 0x00B28900<LightningJoint>, 0x00B289B0<CLightning*>, #0x0040A800, #0x008C64B0, #0x008C6550, 0x00717630<Liquid::DisplMapPerturbation*>, 0x007176D0<Liquid::DisplMapPerturbationMesh*>, 0x0071C4E0<Liquid::IMaterial*>, 0x00B3C2C0<Liquid::CInstance*>, 0x0071CF30<TSExplicitList<Liquid::Object, -572662307>>, 0x00647670<DayNight::AreaLightOverride>, 0x0064EF40<TSExplicitList<DayNight::DNOverrideSky, -572662307>>, 0x0064ABC0<const LightRec*>, 0x00652590/0x1402F26B0<const ZoneLightPointRec*>, 0x006529A0<DayNight::CZoneLight>, 0x0072C890<CWorldMap::CMapTableEntry>, 0x0072CEA0<CMapLight*>, 0x00726DC0<CMapAreaLow*>, 0x00726E60<CMapObjDef*>, 0x0072E170<TSExplicitList<CMapDoodadDef, -572662307>>, 0x0072E2E0<TSExplicitList<CMapObjDef, -572662307>>, 0x0075EE70<CSceneMaterial*>, 0x0075D2D0<CGxVertexPT2>, 0x0075E980<CMapFootprintTexture>, 0x00762FA0<SMapObjDefVectorIntersect>, 0x00765BB0<CWaterRipple>, 0x00755200<CPortalView>, 0x00755340<CSortEntryDoodadDef>, 0x00794210<C4Vector>, 0x00776FA0<CBarrier::SContact>, 0x00797DD0<CMapLowDetail::CMapAreaLowTableEntry>, 0x0079A2D0<CSortEntry>, 0x00776EF0<CGxVertexPCT0T1>, 0x00784B80<CMap::CMapAreaTableEntry>, 0x007865E0<CMapAreaTexture>, 0x0077CCC0<LiquidChunkAndDistance>, 0x0077D480<World::LiquidQueryResult>, 0x0077F630<CGxVertexPC>, 0x0077BB00<TSExplicitList<CSceneMaterial, -572662307>>, 0x0077A3F0/0x1404662E0<RElement>, 0x00747A10<TSExplicitList<CMapObj, -572662307>>, 0x00753870<CMapObjDefGroup*>, 0x00753950<SPortalExt>, 0x00753A00<CWorldView::CVisRenderChunk>, 0x00753E10<CSortEntryChunk>, 0x00736310<CDetailDoodadInstAdd>, 0x00736460<DDMaterial*>, 0x00739780<TSExplicitList<DDMaterial, -572662307>>, 0x0079B860<C4Plane>, 0x0079B910<CWorldOcclusionVolume>, 0x008429E0<CGxDevice::ShaderModelCall>, 0x004C22B0<TSExplicitList<RGN, -572662307>>, 0x00848D30<CGxShaderPermute>, 0x008315D0<unsigned char *>, 0x0053ECC0<CStreamingObj>, 0x0045CB80<TSExplicitList<UIADDON::METADATA, -572662307>>, 0x004FC0F0<TSExplicitList<FILENAMEHASH, -572662307>>, 0x005056F0<TSExplicitList<FRAMEATTR, -572662307>>, 0x00525EF0<TSExplicitList<SIMPLEFONT, -572662307>>, 0x005427B0<TSExplicitList<CTexture, -572662307>>, 0x00545190<TSExplicitList<ProfileInternal::KEYVALUE, -572662307>>, 0x00545300<TSExplicitList<ProfileInternal::SECTION, -572662307>>, 0x0054B730<TSExplicitList<CTextureBlobTexture, -572662307>>, 0x0061C040<TSExplicitList<KERNNODE, -572662307>>, 0x00621260<TSExplicitList<BATCHEDRENDERFONTDESC, -572662307>>, 0x00712BB0<TSExplicitList<SE3ChatSound, -572662307>>, 0x007149F0<TSExplicitList<SE3MemorySoundObject, -572662307>>, 0x0087DC50<TSExplicitList<OsNet::NETSELSOCKPTR, -572662307>>, 0x00549A20<TSExplicitList<FONTHASHOBJ, -572662307>>, 0x00622690<TSExplicitList<FACEDATA, -572662307>>, 0x00708CE0<TSExplicitList<SE3SoundChunk, -572662307>>, 0x00709FE0<TSExplicitList<SE3SOUNDKITOBJECT_LOOKUP, -572662307>>, 0x00708FC0<TSExplicitList<SOUNDKITLOOKUP, -572662307>>, 0x00708E50<TSExplicitList<SE3SoundEmitterLookup, -572662307>>, 0x00709E70<TSExplicitList<SE3COMBINABLESOUNDLIST, -572662307>>, 0x007061B0<SE3SoundKitDef*>, 0x005125D0<CAsyncObject*>, 0x0086D860<TSExplicitList<CGxDeviceD3d11::InputLayoutRec, -572662307>>, 0x0086D9D0<TSExplicitList<CGxDeviceD3d11::RasterizerStateRec, -572662307>>, 0x0086DB40<TSExplicitList<CGxDeviceD3d11::BlendStateRec, -572662307>>, 0x0086DCB0<TSExplicitList<CGxDeviceD3d11::DepthStateRec, -572662307>>, 0x0086DE20<TSExplicitList<CGxDeviceD3d11::SamplerStateRec, -572662307>>, 0x00612420<GxuDebugTextObject>, 0x00849290<TSExplicitList<CGxShader, -572662307>>, 0x0053EB80/0x140199640<void (*)()>, 0x0053EC20/0x140199730<int32_t (*)()>, 0x005473A0<TSExplicitList<FileNotification::DeferredFile, -572662307>>, 0x005484E0<TSExplicitList<CModelBlob::CHashEntry, -572662307>>, 0x004ECA00<TSExplicitList<HashedNode, -572662307>>, 0x004ECB70<TSExplicitList<FrameFactoryNode, -572662307>>, 0x00611170<TSExplicitList<CLightList, -572662307>>, 0x00899E40<FFX::PortalEffect>, 0x00654470<TSExplicitList<CONSOLECOMMAND, -572662307>>, 0x00655EA0<TSExplicitList<CVar, -572662307>>, 0x0083E170<TSExplicitList<FrameScript_EventObject, -572662307>>, 0x00599D10<MapObjSpeedTransition>, 0x008C08F0<TSExplicitList<RACECLASSINFO, -572662307>>, 0x00554DA0<PartyMemberAuraStates>, 0x00554E50<_FOUNDPARAM>, 0x0058BF60<AnimKitBoneSetInst>, 0x0058C3D0<AnimKitDeferredCommand>, 0x00590FB0<AnimKitDesiredBoneSetInst>, 0x005AC770<HealPredictionInfo>, 0x006C1F90<RECTF>, 0x006E70E0<JamGuildNewsEvent>, 0x009F24B0<TSExplicitList<CGSpellActivationOverlay::COUNTERNODE, -572662307>>, 0x009F2620<TSExplicitList<CGSpellActivationOverlay::PETAURANODE, -572662307>>, 0x009F20C0<CGSpellActivationOverlay::WatchedSpellInfo>, 0x009F4770<const ResearchBranchRec*>, 0x009F5E60<TSExplicitList<CGResearchFrame::BRANCH, -572662307>>, 0x009F5FD0<TSExplicitList<CGResearchFrame::BLOBPOIHASH, -572662307>>, 0x009C82B0<CUFProfileInfo*>, 0x009C9650<TSExplicitList<CGCUFProfiles::PROFILENODE, -572662307>>, 0x009C97C0<TSExplicitList<CGCUFProfiles::OPTIONENTRY, -572662307>>, 0x00672FE0<JamCliCUFProfile>, 0x009EED20<GlyphItem*>, 0x0099D3F0<TSExplicitList<TalentGroupItem_C, -572662307>>, 0x0099D560<TSExplicitList<TalentGroupTierItem_C, -572662307>>, 0x00403000<SArchive*>, 0x00438E80<CCameraTransition>, 0x00439790<TSExplicitList<CCameraModeEntry, -572662307>>, 0x00446C30<TSExplicitList<_BANNEDADDON, -572662307>>, 0x0044AE90<TSExplicitList<MOUSELOOKBINDING, -572662307>>, 0x0045CE60<TSExplicitList<UIADDON, -572662307>>, 0x00461E70<CCameraTrack::DeferredCameraAction>, 0x00482920<TSExplicitList<PLAYERPORTRAIT, -572662307>>, 0x00482A90<TSExplicitList<UNITPORTRAIT, -572662307>>, 0x00482C00<TSExplicitList<ITEMPORTRAIT, -572662307>>, 0x004879E0<WowConnection*>, 0x0048BE10<TSExplicitList<WowClientDB2_Base::DBTABLE, -572662307>>, 0x0048E910<TSExplicitList<CGObject_C, -572662307>>, 0x00495AE0<TSExplicitList<SPELLACTIVATIONINFO, -572662307>>, 0x004A04B0<TSExplicitList<DBCache<NameCache,unsigned __int64,CHashKeyGUID>::REVERSEENTRY, -572662307>>, 0x004A0C20<TSExplicitList<DBCache<CreatureStats_C,int,HASHKEY_INT>::DBCACHEHASH, -572662307>>, 0x004A0D90<TSExplicitList<DBCache<GameObjectStats_C,int,HASHKEY_INT>::DBCACHEHASH, -572662307>>, 0x004A0F00<TSExplicitList<DBCache<NPCText,int,HASHKEY_INT>::DBCACHEHASH, -572662307>>, 0x004A1070<TSExplicitList<DBCache<NameCache,unsigned __int64,CHashKeyGUID>::DBCACHEHASH, -572662307>>, 0x004A11E0<TSExplicitList<DBCache<GuildStats_C,unsigned __int64,CHashKeyGUID>::DBCACHEHASH, -572662307>>, 0x004A1350<TSExplicitList<DBCache<QuestCache,int,HASHKEY_INT>::DBCACHEHASH, -572662307>>, 0x004A14C0<TSExplicitList<DBCache<PageTextCache_C,int,HASHKEY_INT>::DBCACHEHASH, -572662307>>, 0x004A1630<TSExplicitList<DBCache<PetNameCache,int,HASHKEY_INT>::DBCACHEHASH, -572662307>>, 0x004A17A0<TSExplicitList<DBCache<CGPetition,int,HASHKEY_INT>::DBCACHEHASH, -572662307>>, 0x004A1910<TSExplicitList<DBCache<ItemTextCache_C,unsigned __int64,CHashKeyGUID>::DBCACHEHASH, -572662307>>, 0x004A1A80<TSExplicitList<DBCache<WardenCachedModule,CWardenKey,CWardenKey>::DBCACHEHASH, -572662307>>, 0x004A1BF0<TSExplicitList<DBCache<ArenaTeamCache,int,HASHKEY_INT>::DBCACHEHASH, -572662307>>, 0x004A1D60<TSExplicitList<DBCache<DanceCache,int,HASHKEY_INT>::DBCACHEHASH, -572662307>>, 0x004D0930<Grunt::CdKey::Info>, 0x004DCB50<TSExplicitList<TALKINGPLAYER, -572662307>>, 0x004DCCC0<TSExplicitList<MUTEDPLAYER, -572662307>>, 0x004E4060<PhaseGroupLookup>, 0x004E4110<CPhaseMapTracker>, 0x004E55C0<TSExplicitList<DECLINEDWORD, -572662307>>, 0x005697C0<TSExplicitList<TRADESKILLLINE, -572662307>>, 0x00569930<TSExplicitList<PROFICIENCYSPELL, -572662307>>, 0x0058B9F0<CAnimReplacementSetDef*>, 0x0058C0B0<CAnimKitInst*>, 0x0058C290<AnimKitSegmentBoneSetInst*>, 0x0058C330<AnimKitSegmentInst*>, 0x0058E9E0<TSExplicitList<CustomFallbackPath, -572662307>>, 0x00595A40<TSExplicitList<VoidItem_C, -572662307>>, 0x005A6390<TSExplicitList<INVENTORYART, -572662307>>, 0x005A6500<TSExplicitList<ITEMBYNAME, -572662307>>, 0x005AC830<UnitAttackIndicator>, 0x005CF380<TSExplicitList<UnitThreat, -572662307>>, 0x005E15F0<TSExplicitList<OBJ_EFFECT_GROUP_DEF_REF_LIST_LOOKUP, -572662307>>, 0x005E1760<TSExplicitList<OBJ_EFFECT_REF_LOOKUP, -572662307>>, 0x005E18D0<TSExplicitList<OBJ_EFFECT_GROUP_LOOKUP, -572662307>>, 0x005E1A40<TSExplicitList<OBJ_EFFECT_REF_LIST_LOOKUP, -572662307>>, 0x005E1BB0<TSExplicitList<OBJ_EFFECT_DEF_LOOKUP, -572662307>>, 0x005E1D20<TSExplicitList<OBJ_EFFECT_GROUP_DEF_LOOKUP, -572662307>>, 0x005E1E90<TSExplicitList<OBJ_EFFECT_PACKAGE_DEF_LOOKUP, -572662307>>, 0x005ED480<CVehiclePassenger_C*>, 0x005EE440<VehicleAnimDeferredSpline>, 0x0060C220<DEATTHUDSOUNDINFO>, 0x0061BED0<TSExplicitList<CHARCODEDESC, -572662307>>, 0x0061D9E0<VERT>, 0x0061DBF0<GRADIENTINFO>, 0x00624AE0<CHARACTER_INFO>, 0x00627BA0<BattlenetLogin::GameAccount>, 0x0062E9C0<TSExplicitList<ZONESOUNDOVERRIDELOOKUP, -572662307>>, 0x0062EBA0<AmbFlavorListHead>, 0x00631800<TEXTEMOTELOOKUP>, 0x006318B0<TEXTEMOTELOOKUP*>, 0x00632A80<TSExplicitList<DSP_CHAIN_HEAD_LOOKUP, -572662307>>, 0x00633AE0<TSExplicitList<FOOTSTEPSNDCACHE, -572662307>>, 0x00637860<C2iVector>, 0x006C1E10<JamCategoryCooldown>, 0x006C25B0<JamGuildBankSocketEnchant>, 0x006C2660<JamGuildRankTabPermissions>, 0x00704200<TSExplicitList<CACHEENTRY, -572662307>>, 0x0076AC00<World::HitTestDebugItem::TextureInfo>, 0x00792510<TSExplicitList<CFileName, -572662307>>, 0x008ADE40<TSExplicitList<ITEMCOOLDOWNHASHNODE, -572662307>>, 0x008ADFB0<TSExplicitList<DESTLOCSPELLCASTINDEXHASHNODE, -572662307>>, 0x008D6EF0<TSExplicitList<SAVEDVARIABLE, -572662307>>, 0x008D7060<TSExplicitList<TaintEvent, -572662307>>, 0x008EE2F0<TSExplicitList<WORDLIST, -572662307>>, 0x008EE460<TSExplicitList<CHATTHROTTLE, -572662307>>, 0x008EE5D0<TSExplicitList<RegisteredAddonPrefix, -572662307>>, 0x008EE740<TSExplicitList<PROFANITYCHECK, -572662307>>, 0x008EE8B0<TSExplicitList<SPAMCHECK, -572662307>>, 0x00900440<TSExplicitList<USEABLESTYLE, -572662307>>, 0x00909B60<TSExplicitList<ACCOUNTIDCACHE, -572662307>>, 0x0090E400<SpellBookItem*>, 0x0090E4A0<SpellBookSkillInfo*>, 0x00913120<TSExplicitList<OVERRIDE_SPELL_NODE, -572662307>>, 0x00913290<TSExplicitList<SPELLSBYNAME, -572662307>>, 0x00913400<TSExplicitList<OVERRIDE_SPELL_BY_NAME_NODE, -572662307>>, 0x00922F30<TSExplicitList<KEYCOMMAND, -572662307>>, 0x009230A0<TSExplicitList<KEYBINDING, -572662307>>, 0x00923210<TSExplicitList<OVERRIDEKEYBINDING, -572662307>>, 0x00923380<TSExplicitList<MODIFIEDCLICK, -572662307>>, 0x009234F0<TSExplicitList<CLICKFRAME, -572662307>>, 0x0092D470<RaidInstanceInfo>, 0x009324C0<GuildMemberInfo*>, 0x00932560<GuildProfessionMember>, 0x00938B00<TSExplicitList<CGGuildInfo::GuildRecipesKnown, -572662307>>, 0x00938C70<TSExplicitList<CGGuildInfo::SKILLLINESPELLS, -572662307>>, 0x00938DE0<TSExplicitList<CGGuildInfo::SKILLLINESPELLBITS, -572662307>>, 0x0094D1A0<TSExplicitList<ACHIEVEMENT, -572662307>>, 0x0094D310<TSExplicitList<GUILDACHIEVEMENTMEMBERLIST, -572662307>>, 0x00954830<UIButtons>, 0x0095A0D0<MissingLootItem>, 0x00963D90<TSExplicitList<CGQuestLog::QUESTCOMPLETIONNPC, -572662307>>, 0x0096DD30<StationeryInfo*>, 0x0096DDD0<MailboxMessageInfo*>, 0x0097F930<CGLookingForGroup::LFGEntryInfo>, 0x00987000<TSExplicitList<LFGListParty, -572662307>>, 0x00987170<TSExplicitList<LFGListPlayer, -572662307>>, 0x0098D7C0<TradeSkillSubClassInfo*>, 0x0099F850<CGCurrencyTypes::CurrencyCategoryInfo>, 0x0099F900<CGCurrencyTypes::CurrencyListInfo>, 0x009A3D50<CFlyout::Slot>, 0x009A3E00<CFlyout*>, 0x009A45E0<TSExplicitList<CFlyout, -572662307>>, 0x009A4750<TSExplicitList<SpellToFlyoutMapItem, -572662307>>, 0x009A8380<TSExplicitList<CGReputationInfo::FACTION, -572662307>>, 0x009B2D50<TSExplicitList<EVENTDATEHASH, -572662307>>, 0x009B2EC0<TSExplicitList<DYNAMICHOLIDAYHASH, -572662307>>, 0x009B83D0<TSExplicitList<KNOWNDANCES, -572662307>>, 0x009B91F0<TrainerServiceInfo*>, 0x009C0890<GuildBankItem>, 0x009CB300<CGAutoCompleteName::AUTOCOMPLETE_NAMENODE*>, 0x009CBD50<TSExplicitList<CGAutoCompleteName::AUTOCOMPLETE_CHARACTER_HASHNODE, -572662307>>, 0x009CBEC0<TSExplicitList<CGAutoCompleteName::AUTOCOMPLETE_BATTLENET_HASHNODE, -572662307>>, 0x009CDB00<TSExplicitList<WORLDSTATE, -572662307>>, 0x009DE8D0<TSExplicitList<CGQuestObjectiveCache::REQUIREDUNITNODE, -572662307>>, 0x009DEA40<TSExplicitList<CGQuestObjectiveCache::REQUIREDITEMNODE, -572662307>>, 0x009E2500<TAXILINE>, 0x009E4100<TSExplicitList<TAXIPATHNODESCLIENT, -572662307>>, 0x009E7570<CGNamePlateFrame*>, 0x009F8730<C3iVector>, 0x009FCE60<INCOMINGRESURRECTION>, 0x009FD070<TSExplicitList<CGIncomingResurrection::INCOMINGRESLIST, -572662307>>, 0x00A0D330<MacroSubstring>, 0x00A0E880<TSExplicitList<MacroConditions, -572662307>>, 0x00B32510<TSExplicitList<BITMAPBYICON, -572662307>>, 0x0044E250<const VehicleUIIndSeatRec*>, 0x00494F30<const SpellEffectRec*>, 0x0058C010<const CAnimReplacementSetDef*>, 0x0058C150<const AnimKitSegmentRec*>, 0x0058C1F0<const AnimKitConfigBoneSetRec*>, 0x00624B90<const JamClientCharacterListEntry*>, 0x009025A0<uint32_t>, 0x0093DD90<WarGameType*>, 0x006C24E0<JamVoidItem>, 0x006C2B10<JamAccountInfoData>, 0x006D70A0<JamServerInfoField>, 0x006D6900<JamCliGuildRankData>, 0x006D69B0<JamClientCommentatorPartyMember>, 0x006D6BA0<LFGuildApplicationData>, 0x009A92F0<ArenaTeamMemberInfo*>, 0x00993540<SpellVariables>]
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

// OFFSET: [#0x00409470, #0x004C4B10<C3Vector>, 0x00486A60, #0x0056B7C0<CGCommentator::InstanceInfo>, #0x0058EB60, #0x0058EBD0<C2Vector>, #0x00591250<TaxiPathRec*>, #0x005912C0<TAXIROUTELINE>, #0x005C5F00<TalentGroupTier_C*>, #0x005E0800, #0x005EF6A0<MacroSubstring>, #0x006EAAB0<float>, #0x006FF5C0<SpellCastTargetPoint>, #0x006FF630, #0x00700C80, #0x0077AC80, #0x007F8240, #0x0097FE80<CRibbonMat>, 0x006528B0<const ZoneLightPointRec*>, 0x00799CA0<CSortEntryChunk>, 0x00799D20<CMapAreaLow*>, 0x00799DA0<CMapObjDef*>, 0x00587C20<unsigned __int64>, 0x0040B970<int>, 0x00487C80<WowConnection*>, 0x0095E4A0<BLOBPOI>, 0x00A0DCA0<MacroSubstring>, 0x00495470<const SpellEffectRec*>]
template <class T>
void TSFixedArray<T>::Set(uint32_t count, const T* data) {
    this->ReallocAndClearData(count);

    for (uint32_t i = 0; i < count; i++) {
        new (&this->m_data[i]) T(data[i]);
    }

    this->m_count = count;
}

// OFFSET: [#0x00464B30<CHARACTER_INFO>, #0x00465AC0<REALM_INFO>, #0x004670D0, #0x004957B0<CFrameStrataNode*>, %0x00641C30, #0x004F5840<int>, #0x00547E20, #0x00592F00, #0x00592FC0, #0x00599820<CImVector>, #0x005C9690, #0x006171B0, #0x0061F9C0<CSimpleFontString*>, #0x006C0270<C3Vector>, %0x00634D80<TEXTURECACHEROW>, #0x007476F0, #0x0079E080<CWaterRipple>, #0x007F1220<float>, #0x007F1EA0<C2Vector>, #0x007F1F20<unsigned short>, %0x0052A840<CSimpleEditBox::EditHistory>, 0x00B2BE40<LightningJoint>, %0x00A647D0, 0x00765C90<CWaterRipple>, 0x008C0B10<TSExplicitList<RACECLASSINFO, -572662307>>]
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

// OFFSET: [%0x004C26E0<char>, %0x00922260<C3Vector>, #0x0057FD50<C2Vector>, %0x004EA570<unsigned short>, %0x00473F40<FrameStackInfo>, %0x00A4A1A0, #0x006C6550<GXUFONTHYPERLINKINFO>, #0x006C6760<GXUEMBEDDEDTEXTUREINFO>, #0x0071C780<AuraData>, %0x005BC5F0<SortedAuraInfo>, %0x005C3600<int>, %0x00615D80<SimpleScriptFunction>, #0x00791EB0<CMapObjDefGroup*>, %0x006D7B30<CGxVertexPCT0T1>, %0x006D7C60, #0x007940F0<CPortalView>, %0x0068B270<CFacet>, %0x006B7190<CRect>, #0x007CD140<C4Plane>, #0x007CD3F0<CClipVolume>, 0x007634B0/0x1404487E0<SMapObjDefVectorIntersect>, 0x00777260<CGxVertexPCT0T1>, 0x00777390<CBarrier::SContact>, 0x00756E90<CMapAreaLow*>, 0x00756F10<CMapObjDef*>, 0x0075F000<CMapObjDefGroup*>, 0x0079B9C0<C4Plane>, 0x0079BC90<CWorldOcclusionVolume>, 0x004F0270/0x140134990<void*>, 0x0059B420<MapObjSpeedTransition>, 0x009F4AC0<const ResearchBranchRec*>, 0x009F0000<GlyphItem*>, 0x00911EC0<SpellBookItem*>, 0x00941E40<WarGameType*>]
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

// OFFSET: [%0x0062E900<CImVector>, %0x0062E980<VERT>, %0x0062EF20<GRADIENTINFO>, 0x009F4B40<const ResearchBranchRec*>]
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

// OFFSET: [%0x0092CC40, %0x006B3F40<unsigned __int64>, %0x004EA210<unsigned short>, %0x005B4550<DayNight::AreaLightOverride>, %0x0092A8A0<C4Plane>, %0x00A5C700<FFX::Pass*>, %0x0062E1F0, %0x006B3E50<CPortalView>, %0x00535C20<C3Vector>, %0x0078E420<CGxVertexPCT0T1>, 0x009FFD20<JamCliCUFProfile>]
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

// OFFSET: [%0x00808840<EvtTimer*>, %0x006825E0<Liquid::CMaterialSettings*>, #0x008A1BB0<Liquid::IMaterial*>, 0x0071C5B0/0x1403F15A0<Liquid::IMaterial*>, 0x007952D0/0x140487E90<C4Vector>, 0x007078F0/0x1403D44E0<C3Vector>, 0x00707040<SE3SoundKitDef*>]
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

// OFFSET: [%0x00406010<char*>, #0x00409410<CLoadingScreenShipInfo<TaxiPathNodeRec>::LOADINGSCREENTAXIPATHINFO>, %0x008056C0<SThread*>, %0x005382F0<TSList<SIMPLEANIMNODE, TSGetLink<SIMPLEANIMNODE>>*>, %0x00576B50<int>, #0x004CEBE0<TEXTEMOTELOOKUP>, %0x0048FC00<CObjectHeap>, %0x0048FFF0<CObjectHeapList>, #0x004E3760<CharacterSelectionDisplay>, #0x00500F60, %0x00835500<NearestUnitData>, #0x00565420<MacroCommand>, #0x00579AD0, %0x009219F0<OBJINFO>, %0x00942530<CGQuestPOIFrame::QuestPOIRenderData>, #0x005916F0<TAXIROUTELINE>, #0x00591890<TAXILINE>, %0x008F0EB0<TalentGroupTier_C*>, %0x0046A710<CRect>, %0x007AA070<CGxPushedRenderState>, %0x007AA120, #0x006851D0<C3Vector>, #0x0068A030<CGxFormat>, #0x0068A090<CGxMonitorMode>, #0x006B2A40<Grunt::CdKey::Info>, #0x006DEC70<InitialSpellStruct>, #0x007761E0<_SOURCE>, #0x007CD1D0<C4Plane>, #0x007E6E50, #0x007F0CC0, %0x004EED70<AreaPOIRec*>, #0x00821670<M2Element>, #0x0082DB70<M2ModelCallback>, #0x0086D180<OsIMECandidate>, #0x00870550<W32Joystick>, #0x00879900<SEChannelGroup>, %0x00802390, #0x008C6DE0, #0x008C6E60, %0x0052BD40<MessageData>, #0x00980FB0<CParticleEmitter2*>, #0x00524930, #0x00782490, 0x00647730<DayNight::AreaLightOverride>, 0x00756E20<CSortEntryChunk>, 0x0079BA50<C4Plane>, 0x004C1B20<_SOURCE>, 0x00512A70<CAsyncObject*>, 0x0084AB90/0x1405790D0<CGxMonitorMode>, 0x0053F1C0/0x140199D80<CStreamingObj>, 0x00B13BC0/0x1408D9900<CParticleEmitter2*>, 0x0087A5C0/0x1405B0C90<W32Joystick>, 0x0087BA10<OsIMECandidate>, 0x0089CD40<FFX::PortalEffect>, 0x0059EFC0<MapObjSpeedTransition>, 0x009F6A90<CGResearchFrame::BLOBPOI>]
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

// OFFSET: [%0x0040B0C0, %0x006B72D0<unsigned int>, %0x00538250<CSimpleControlPoint*>, #0x004B9C00, %0x0084EFE0<PENDINGUSERLIST>, #0x0054EFE0<C2iVector>, %0x008FC020, %0x0094B360<EventInviteInfo*>, #0x00614390, %0x0048A490<CTexture*>, #0x00749730<CVehiclePassenger_C*>, %0x0061F8B0, %0x006D3940<CChunkLiquid*>, %0x00802110<FFX::Pass*>, %0x00820770, #0x008A1C30<Liquid::CInstance*>, 0x00B2BB80/0x1408F4040<CLightning*>, %0x005C31F0, %0x0068B3D0, 0x00717C30/0x1403EB320<Liquid::DisplMapPerturbation*>, 0x00717C90<Liquid::DisplMapPerturbationMesh*>, 0x00B3C4F0/0x140909460<Liquid::CInstance*>, 0x0072D7B0<C3Vector>, 0x0072D820<unsigned short>, 0x00772670<CChunkLiquid*>, 0x00756960/0x140438060<CMapObjDefGroup*>, 0x00612C00/0x14029DF80<GxuSceneObject*>, 0x00612C60<GxDrawListEntry>, 0x0084AA50<void*>, 0x008BDE50/0x140607100<const SpellVisualKitModelAttachRec*>, 0x009F5150<C2iVector>, 0x009F8F10<C3iVector>, 0x00563FE0<RuneConvert>]
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

// OFFSET: [%0x0040AC80<DYNAMICELEMENTVERT>, #0x0040AFE0, 0x006DB760<unsigned int>, #0x004845F0<CSimpleBatchedMesh>, #0x004920B0, %0x00510130<FRAMEPRIORITY*>, #0x004969C0, #0x004B8610<CTexture*>, #0x004BB540, #0x004BC4A0, #0x004BC5B0, #0x004BE360, #0x004C05B0, %0x004D0F80, %0x004D10A0, #0x004CABE0, #0x004CC810, #0x004CC920, #0x004CDE60, #0x004CEC60<TEXTEMOTELOOKUP*>, #0x004CF320, #0x004D6640, %0x004657A0<char const*>, %0x00425D00<int>, #0x004F3280, %0x0084CBF0<char*>, #0x00500F10<RaidInstanceInfo>, %0x0084F060<PENDINGUSERLIST>, #0x005054F0, #0x00505600, #0x00505710, #0x00505820, #0x00506210<ChatChannel>, %0x00835550<NearestUnitData>, #0x00528700, #0x00528810, #0x00529BC0, #0x0052F1F0, #0x0053F440, #0x005462C0<WorldMapLandmarkInfo>, #0x00548C00, %0x008DBAC0, %0x008DBB10, #0x005595E0<LFGListParty*>, #0x005596F0, #0x0055FDB0, #0x0055FEC0, #0x0055FFD0, #0x005600E0, #0x005601F0, #0x00565490<MacroCommand>, %0x009198D0, %0x00887BD0<CGCommentator::InstanceInfo>, #0x0056BE70, %0x008C9200<MailboxMessageInfo*>, #0x00575FF0, %0x00921930<POIINFO>, %0x00921A60<OBJINFO>, #0x0057FDD0<C2Vector>, 0x00563F90<unsigned short>, %0x008BB350, %0x0092D8F0<TAXIROUTELINE>, %0x00425C60, #0x005923A0, %0x008CDCE0<OutbidItemInfo*>, #0x005B5310, #0x005B5420, #0x005BF870, #0x005BF980, #0x005C8310, #0x005C8420, %0x008921E0<GuildMemberInfo*>, %0x0092B2F0<PetitionSignerInfo>, #0x005D2570, %0x008ACF60, %0x008E63A0, #0x005E72B0, #0x005EFFD0, #0x005F0570, #0x005F0680, #0x005F3ED0, #0x005F5C80, %0x004669D0, #0x005F5EA0, #0x005FCC60, #0x00603930<CFacet>, %0x0046A780<CRect>, %0x0048A850<CImVector>, %0x0048A4F0, #0x00617CE0, #0x00617DF0, #0x00617F00, #0x006700E0, #0x00671CB0, #0x00671DC0, #0x00671ED0, #0x00671FE0, #0x006720F0, #0x00672200, #0x00672310, #0x00672420, #0x00672530, #0x00672640, #0x00672750, #0x00672860, #0x00672970, #0x00672A80, #0x00672B90, #0x00682790<GxDrawListEntry>, #0x00682A40<CGxFormat>, %0x007AA170, %0x004C2690<unsigned char>, #0x00688450, 0x0084AC00/0x140579170<CGxMonitorMode>, #0x006BC250, #0x006C2E60, #0x006C2F70, %0x006312C0<VERT>, #0x006C59D0, 0x0061E260/0x1402ACB70<GXUFONTHYPERLINKINFO>, #0x006C6800<GXUEMBEDDEDTEXTUREINFO>, %0x0062EFA0<GRADIENTINFO>, #0x006C8410, #0x006DECC0<ITEMSWAP>, #0x006E6920, #0x006E6A30, #0x006F31A0, #0x006F32B0, #0x006F33C0, #0x006F34D0, #0x006F35E0, #0x006F36F0, #0x006F3800, #0x00700CF0<SpellCastTargetPoint>, #0x00709BC0, #0x00709CD0, #0x00723170<DelayedSpellVisualKit>, #0x007233C0<AuraData>, #0x00734800, #0x00767AB0, #0x00768B00, #0x0076D260, #0x00776CF0, #0x00788930<Mists::Mist>, #0x00791F30<CMapObjDefGroup*>, #0x00792010<CGxVertexPCT0T1>, #0x00792130, #0x00793FD0<CGxVertexPC>, #0x00794190<CPortalView>, #0x0079F980<CGxVertexPT0T1>, #0x007A02F0<CMapFootprintTexture>, #0x007B06F0, #0x007B25B0<CDetailDoodadInstAdd>, #0x007B2710<CGxBuf*>, #0x007BA250<CMapChunkBufBlock>, #0x007BE2A0, #0x007BE3B0<CMapObjDef*>, #0x007CD250<C4Plane>, #0x007CD470<CClipVolume>, #0x007DD480, #0x007DD590, #0x007E02E0, %0x004E88E0, #0x007F2AB0, %0x004EF360<POIDIRECTIONDATA>, #0x007F4F60, %0x00820720<LightningObject::Bolt>, #0x008076C0, #0x008077D0, #0x00810BA0, #0x00810CB0, #0x00810DC0, #0x00812D40, #0x00812E50, %0x004FEA60<CXMLAttribute>, #0x0081AB60, #0x0082DAC0<CRibbonMat>, %0x00785090<M2ModelCallback>, #0x0084B600, %0x00A65890, #0x0086EED0, %0x0043ECD0, %0x0066E6F0<SEDriverInfo>, #0x0087BB70, 0x0051C0A0<MessageData>, %0x0052C070<CSimpleMessageFrameLine>, %0x0052D450<CSimpleMessageScrollFrameLine>, %0x0052D540<CSimpleMessageScrollFrameDisplayNode>, #0x0097D600<CParticle2>, %0x00A57350<CParticle2_Model>, %0x00A5A880<CGxVertexPCT>, #0x007F8FA0<ShipPathLeg>, #0x004E37E0<CharacterSelectionDisplay>, 0x0071D130<TSExplicitList<Liquid::Object, -572662307>>, 0x0064F0B0<TSExplicitList<DayNight::DNOverrideSky, -572662307>>, 0x00652B00/0x1402F2CD0<DayNight::CZoneLight>, 0x0072D0D0<CWorldMap::CMapTableEntry>, 0x0072E770<TSExplicitList<CMapDoodadDef, -572662307>>, 0x0072E880<TSExplicitList<CMapObjDef, -572662307>>, 0x0075D690<CGxVertexPT2>, 0x0075EB90/0x140442A50<CMapFootprintTexture>, 0x00758790/0x14043AAD0<CPortalView>, 0x00797ED0<CMapLowDetail::CMapAreaLowTableEntry>, 0x0079A440/0x14048E930<CSortEntry>, 0x007772F0<CGxVertexPCT0T1>, 0x00777420<CBarrier::SContact>, 0x00784C30/0x140473920<CMap::CMapAreaTableEntry>, 0x00786800/0x140475A70<CMapAreaTexture>, 0x0077F6E0<CGxVertexPC>, 0x0077BE90<TSExplicitList<CSceneMaterial, -572662307>>, 0x00747E00<TSExplicitList<CMapObj, -572662307>>, 0x007514B0<Mists::Mist>, 0x00737D80<CDetailDoodadInstAdd>, 0x0073A680<TSExplicitList<DDMaterial, -572662307>>, 0x007569C0/0x140438110<CMapObjDefGroup*>, 0x00756A10/0x1404381A0<CSortEntryChunk>, 0x0079BAE0/0x140490490<C4Plane>, 0x0079BD10/0x1404906D0<CWorldOcclusionVolume>, 0x00446EC0<TSExplicitList<_BANNEDADDON, -572662307>>, 0x004A5BB0<TSExplicitList<DBCache<GuildStats_C, int, HASHKEY_INT>::DBCACHEHASH, -572662307>>, 0x004C2BA0<TSExplicitList<RGN, -572662307>>, 0x0054BC20<TSExplicitList<CTextureBlobTexture, -572662307>>, 0x006564E0<TSExplicitList<CVar, -572662307>>, 0x00B32A00<TSExplicitList<BITMAPBYICON, -572662307>>, 0x004FC920<TSExplicitList<FILENAMEHASH, -572662307>>, 0x00526240/0x1401751D0<TSExplicitList<SIMPLEFONT, -572662307>>, 0x00848DE0/0x1405769A0<CGxShaderPermute>, 0x00713B80/0x1403E5DC0<TSExplicitList<SE3ChatSound, -572662307>>, 0x00714D00/0x1403E7640<TSExplicitList<SE3MemorySoundObject, -572662307>>, 0x00832200<unsigned char *>, 0x0087E200<TSExplicitList<OsNet::NETSELSOCKPTR, -572662307>>, 0x0045D8C0/0x14006FEC0<TSExplicitList<UIADDON::METADATA, -572662307>>, 0x00505B60/0x14014C7C0<TSExplicitList<FRAMEATTR, -572662307>>, 0x00543A00<TSExplicitList<CTexture, -572662307>>, 0x00545500<TSExplicitList<ProfileInternal::KEYVALUE, -572662307>>, 0x00545610<TSExplicitList<ProfileInternal::SECTION, -572662307>>, 0x005E27C0<TSExplicitList<OBJ_EFFECT_GROUP_LOOKUP, -572662307>>, 0x005E28D0<TSExplicitList<OBJ_EFFECT_REF_LIST_LOOKUP, -572662307>>, 0x0061C690<TSExplicitList<KERNNODE, -572662307>>, 0x00621C80<TSExplicitList<BATCHEDRENDERFONTDESC, -572662307>>, 0x0099D880<TSExplicitList<TalentGroupItem_C, -572662307>>, 0x0099D990<TSExplicitList<TalentGroupTierItem_C, -572662307>>, 0x00A31CB0<TSExplicitList<TIMESTAMPSTRUCT, -572662307>>, 0x004A5770<TSExplicitList<DBCache<CreatureStats_C, int, HASHKEY_INT>::DBCACHEHASH, -572662307>>, 0x004A5990<TSExplicitList<DBCache<NPCText, int, HASHKEY_INT>::DBCACHEHASH, -572662307>>, 0x004A5AA0<TSExplicitList<DBCache<NameCache, unsigned __int64, CHashKeyGUID>::DBCACHEHASH, -572662307>>, 0x004A5CC0<TSExplicitList<DBCache<QuestCache, int, HASHKEY_INT>::DBCACHEHASH, -572662307>>, 0x004A5DD0<TSExplicitList<DBCache<PageTextCache_C, int, HASHKEY_INT>::DBCACHEHASH, -572662307>>, 0x004A5EE0<TSExplicitList<DBCache<PetNameCache, int, HASHKEY_INT>::DBCACHEHASH, -572662307>>, 0x004A5FF0<TSExplicitList<DBCache<CGPetition, int, HASHKEY_INT>::DBCACHEHASH, -572662307>>, 0x004A6100<TSExplicitList<DBCache<ItemTextCache_C, unsigned __int64, CHashKeyGUID>::DBCACHEHASH, -572662307>>, 0x004A6320<TSExplicitList<DBCache<ArenaTeamCache, int, HASHKEY_INT>::DBCACHEHASH, -572662307>>, 0x004A6430<TSExplicitList<DBCache<DanceCache, int, HASHKEY_INT>::DBCACHEHASH, -572662307>>, 0x00612CD0<C3Vector>, 0x0044B0E0/0x1400583C0<TSExplicitList<MOUSELOOKBINDING, -572662307>>, 0x00622890<TSExplicitList<FACEDATA, -572662307>>, 0x0070AD90<TSExplicitList<SE3SoundChunk, -572662307>>, 0x0070C700<TSExplicitList<SE3SOUNDKITOBJECT_LOOKUP, -572662307>>, 0x0070AFB0<TSExplicitList<SOUNDKITLOOKUP, -572662307>>, 0x0070AEA0<TSExplicitList<SE3SoundEmitterLookup, -572662307>>, 0x0070C5F0<TSExplicitList<SE3COMBINABLESOUNDLIST, -572662307>>, 0x00512AD0<CAsyncObject*>, 0x0086DF90/0x1405A2A30<TSExplicitList<CGxDeviceD3d11::InputLayoutRec, -572662307>>, 0x0086E0A0<TSExplicitList<CGxDeviceD3d11::RasterizerStateRec, -572662307>>, 0x0086E1B0<TSExplicitList<CGxDeviceD3d11::BlendStateRec, -572662307>>, 0x0086E2C0<TSExplicitList<CGxDeviceD3d11::DepthStateRec, -572662307>>, 0x0086E3D0<TSExplicitList<CGxDeviceD3d11::SamplerStateRec, -572662307>>, 0x00612680/0x14029D830<GxuDebugTextObject>, 0x005477B0<TSExplicitList<FileNotification::DeferredFile, -572662307>>, 0x005488C0<TSExplicitList<CModelBlob::CHashEntry, -572662307>>, 0x00549C20<TSExplicitList<FONTHASHOBJ, -572662307>>, 0x0053F230/0x140199E20<CStreamingObj>, 0x00B19770/0x1408DFC30<CParticle2>, 0x004ED130<TSExplicitList<HashedNode, -572662307>>, 0x004ED240<TSExplicitList<FrameFactoryNode, -572662307>>, 0x006115E0<TSExplicitList<CLightList, -572662307>>, 0x0061C580<TSExplicitList<CHARCODEDESC, -572662307>>, 0x0089CE10/0x1405DD100<FFX::PortalEffect>, 0x00654680/0x1402F4FE0<TSExplicitList<CONSOLECOMMAND, -572662307>>, 0x0083EB50<TSExplicitList<FrameScript_EventObject, -572662307>>, 0x008BDE00<LightningObject::Bolt>, 0x0058D9B0<AnimKitBoneSetInst>, 0x00591DA0<AnimKitDesiredBoneSetInst>, 0x006E6A80<ClientTradeItem>, 0x006F4460<JamGuildNewsEvent>, 0x006F4650<JamGuildBankItemInfo>, 0x009F2A10<TSExplicitList<CGSpellActivationOverlay::COUNTERNODE, -572662307>>, 0x009F2B20<TSExplicitList<CGSpellActivationOverlay::PETAURANODE, -572662307>>, 0x009F6280<TSExplicitList<CGResearchFrame::BRANCH, -572662307>>, 0x009F6390<TSExplicitList<CGResearchFrame::BLOBPOIHASH, -572662307>>, 0x009C9D70<TSExplicitList<CGCUFProfiles::PROFILENODE, -572662307>>, 0x009C9E80<TSExplicitList<CGCUFProfiles::OPTIONENTRY, -572662307>>, 0x00687F70<JamCliCUFProfile>, 0x00631AB0<TEXTEMOTELOOKUP*>, 0x00637B90<C2iVector>, 0x009F8F80<C3iVector>, 0x006DB550<JamGuildRewardItem>, 0x006E6C60<LFGuildBrowseData>, 0x006E6CB0<LFGuildRecruitData>, 0x006E6D00<LFGuildApplicationData>, 0x00930210<RaidInstanceInfo>, 0x00937770<GuildMemberInfo*>, 0x00970450<MailboxMessageInfo*>, 0x004E4B80<uint16_t>, 0x00587CA0<SpellCastTargetPoint>, 0x005BCCD0<DelayedSpellVisualKit>, 0x006DB300<JamEarnedAchievement>, 0x006DB380<JamCriteriaProgress>, 0x006E6D50<JamMoveStateChange>, 0x00704500<TSExplicitList<CACHEENTRY, -572662307>>, 0x004BBFA0<uint32_t>, 0x008EC1F0<bool>]
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

// OFFSET: [%0x005141D0<FONTHASHOBJ, HASHKEY_STR>, %0x004DFB10<CMapObj, HASHKEY_NONE>, %0x008708D0<OVERRIDEKEYBINDING, HASHKEY_STRI>]
template <class T, class TKey>
TSHashObject<T, TKey>::~TSHashObject() {
}

// OFFSET: [#0x0047C7B0<PrefetchNode, HASHKEY_STRI>, #0x0047CA30<UncachableNode, HASHKEY_STRI>, 0x00505B30<FRAMEATTR, HASHKEY_STRI>, 0x00526200<SIMPLEFONT, HASHKEY_STRI>, 0x00543070<CTexture, HASHKEY_TEXTUREFILE>, 0x005486E0/0x1401A0A00<CModelBlob::CHashEntry, HASHKEY_MODELBLOBFILE>, #0x004BC7D0<ProfileInternal::KEYVALUE, HASHKEY_CONSTSTRI>, #0x004BD1B0<ProfileInternal::SECTION, HASHKEY_CONSTSTRI>, 0x00549DA0<FONTHASHOBJ, HASHKEY_STR>, 0x0054BB70/0x1401A51E0<CTextureBlobTexture, HASHKEY_TEXTUREBLOBFILE>, 0x004D9950<TIMESTAMPSTRUCT, HASHKEY_NONE>, #0x004CADB0<VSOUND_LOOKUP, HASHKEY_NONE>, #0x004CCB80<SOUNDKITLOOKUP, HASHKEY_STRI>, #0x004CCD70<ZONESOUNDOVERRIDELOOKUP, HASHKEY_STRI>, #0x004CE310<DSP_CHAIN_HEAD_LOOKUP, HASHKEY_STRI>, #0x004CF500<FOOTSTEPSNDCACHE, HASHKEY_NONE>, #0x004D5DD0<CGObject_C, CHashKeyGUID>, 0x00704680<CACHEENTRY, HASHKEY_NONE>, #0x00506060<CHATTHROTTLE, HASHKEY_STR>, #0x005063F0<WORDLIST, HASHKEY_LANGUAGE>, #0x00506590<PROFANITYCHECK, HASHKEY_STR>, #0x00506790<SPAMCHECK, HASHKEY_STR>, #0x00529680<CGGameUI::THREATMAP, CHashKeyGUID>, #0x00529720<SAVEDVARIABLE, HASHKEY_STR>, #0x005299B0, #0x0052F3D0<USEABLESTYLE, HASHKEY_NONE>, #0x0053F740<SPELLSBYNAME, HASHKEY_UTF8I>, #0x00549010<WORLDSTATE, HASHKEY_NONE>, #0x0055A190<LFGListParty, CHashKeyGUID>, #0x0055A3D0<LFGListPlayer, CHashKeyGUID>, #0x00560650<KEYCOMMAND, HASHKEY_STRI>, #0x00560800<KEYBINDING, HASHKEY_STRI>, #0x00560C10<MODIFIEDCLICK, HASHKEY_STRI>, 0x00924B70<CLICKFRAME, HASHKEY_STR>, #0x00561DA0<OVERRIDEKEYBINDING, HASHKEY_STRI>, 0x009C6680<MACRONODE, HASHKEY_NONE>, #0x00576300<KNOWNDANCES, HASHKEY_STRI>, #0x00592DD0<TAXIPATHNODESCLIENT, HASHKEY_NONE>, #0x005B64C0<ACHIEVEMENT, HASHKEY_NONE>, #0x005B6660<ACHIEVEMENTCRITERIALIST, HASHKEY_NONE>, #0x005C15F0, 0x009B5040/0x14073E650<DYNAMICHOLIDAYHASH, HASHKEY_NONE>, 0x0099DB10/0x140720D50<TalentGroupItem_C, HASHKEY_NONE>, 0x0099DD20/0x140721080<TalentGroupTierItem_C, HASHKEY_NONE>, #0x005D2780<FACTION, HASHKEY_NONE>, #0x005F0820<MacroConditions, HASHKEY_STR>, 0x00447040<_BANNEDADDON, HASHKEY_NONE>, #0x005F6050<UIADDON::METADATA, HASHKEY_STRI>, 0x0045E0D0/0x140070A70<ADDONSTATE, HASHKEY_STRI>, #0x005F7240<UIADDON, HASHKEY_STRI>, 0x0044B300<MOUSELOOKBINDING, HASHKEY_STRI>, #0x00618400<PLAYERPORTRAIT, CHashKeyGUID>, #0x00618630<UNITPORTRAIT, HASHKEY_DWORD>, #0x00618850<ITEMPORTRAIT, HASHKEY_STRI>, #0x00670390, #0x00670540, #0x00670730, #0x006708E0, #0x00670A90, #0x00670C40, #0x00670DF0, #0x00670FA0, #0x006711A0, #0x00671350, #0x006715C0, #0x00671770, #0x00671920, #0x00671AD0, #0x00671C80, #0x00678F70, #0x00678FA0, #0x00678FD0, #0x00679000, #0x00679030, #0x00679060, #0x00679090, #0x006790C0, #0x006790F0, #0x00679120, #0x00679150, #0x00679180, #0x006791B0, #0x006791E0, #0x00679210, 0x00848640/0x140575F80<CGxShader, HASHKEY_STRI>, 0x00611530/0x14029BDA0<CLightList, HASHKEY_DWORD>, #0x006BEF80, 0x0061C400<CHARCODEDESC, HASHKEY_NONE>, 0x0061C810<KERNNODE, KERNINGHASHKEY>, 0x00622A10<FACEDATA, HASHKEY_STRI>, #0x006E6590<TRADESKILLLINE, HASHKEY_NONE>, #0x006E6690<PROFICIENCYSPELL, HASHKEY_NONE>, #0x006F3BE0<OBJ_EFFECT_GROUP_DEF_REF_LIST_LOOKUP, HASHKEY_NONE>, #0x006F3D70<OBJ_EFFECT_REF_LOOKUP, HASHKEY_NONE>, #0x006F4090<OBJ_EFFECT_REF_LIST_LOOKUP, HASHKEY_NONE>, #0x006F4280<OBJ_EFFECT_DEF_LOOKUP, HASHKEY_NONE>, #0x006F5970<OBJ_EFFECT_GROUP_DEF_LOOKUP, HASHKEY_NONE>, #0x006F5EC0<OBJ_EFFECT_GROUP_LOOKUP, HASHKEY_NONE>, #0x006F5F70<OBJ_EFFECT_PACKAGE_DEF_LOOKUP, HASHKEY_NONE>, #0x00709EC0<INVENTORYART, HASHKEY_NONE>, #0x0070A120<ITEMBYNAME, HASHKEY_UTF8I>, #0x00734760<UnitThreat, CHashKeyGUID>, #0x00767A00<CVar, HASHKEY_STRI>, 0x00654800<CONSOLECOMMAND, HASHKEY_CONSTSTRI>, 0x0056D720<RGN, HASHKEY_NONE>, #0x007B0660<CMapObj, HASHKEY_NONE>, #0x007B7110, #0x007C0CE0<CMapDoodadDef, HASHKEY_DWORD>, #0x007C0DD0<CMapObjDef, HASHKEY_NONE>, #0x007DD810<TALKINGPLAYER, CHashKeyGUID>, #0x007DDA00<MUTEDPLAYER, CHashKeyGUID>, #0x007E04B0<DECLINEDWORD, HASHKEY_CONSTSTRI>, #0x007F2C20<DayNight::DNOverrideSky, HASHKEY_STRI>, #0x007F53D0<MINIMAPMD5NAME, HASHKEY_STRI>, #0x0080A400, #0x0080A5F0<DESTLOCSPELLCASTINDEXHASHNODE, CHashKeyGUID>, 0x008C0CF0<RACECLASSINFO, HASHKEY_NONE>, #0x00811270<SKILLCOSTSINFO, HASHKEY_NONE>, #0x00811400<SKILLLINEREC, HASHKEY_SKILLLINE>, 0x004ED3C0<HashedNode, HASHKEY_STRI>, 0x004ED670<FrameFactoryNode, HASHKEY_STRI>, 0x0083F010<FrameScript_EventObject, HASHKEY_CONSTSTRI>, 0x00B31900<LCDHANDLE, HASHKEY_CONSTSTRI>, #0x0084B8E0<BITMAPBYICON, HASHKEY_NONE>, 0x00434A30<CShaderEffect, HASHKEY_STRI>, #0x00877850<CacheData, HASHKEY_NONE>, #0x0087C010<SOUND_INTERNAL_LOOKUP, HASHKEY_NONE>, 0x0071D2B0<Liquid::Object, HASHKEY_NONE>, 0x0064AA30<DayNight::DNOverrideSky, HASHKEY_STRI>, 0x0072E5E0<CMapDoodadDef, HASHKEY_DWORD>, 0x0072E6E0<CMapObjDef, HASHKEY_NONE>, 0x00736110/0x140412750<CMapDynamicTexture, HASHKEY_DWORD>, 0x0077BE40<CSceneMaterial, HASHKEY_MATERIAL>, 0x00747D70<CMapObj, HASHKEY_NONE>, 0x0073AC20<DDMaterial, HASHKEY_NONE>, 0x005479A0<FileNotification::DeferredFile, HASHKEY_NONE>, 0x006156F0/0x1402A1400<BATCHEDRENDERFONTDESC, HASHKEY_PTR>, 0x0070AA20<SE3SoundChunk, HASHKEY_NONE>, 0x0070BF50<SE3SOUNDKITOBJECT_LOOKUP, HASHKEY_NONE>, 0x0070C070<SOUNDKITLOOKUP, HASHKEY_CONSTSTRI>, 0x0070C210<SE3SoundEmitterLookup, HASHKEY_NONE>, 0x0070D7C0<SE3COMBINABLESOUNDLIST, HASHKEY_NONE>, 0x00713A80<SE3ChatSound, HASHKEY_NONE>, 0x00714C60<SE3MemorySoundObject, HASHKEY_NONE>, 0x004FC8F0/0x140140C60<FILENAMEHASH, HASHKEY_STR>, 0x00855600<CGxDeviceD3d11::InputLayoutRec, InputLayoutKey>, 0x00855800<CGxDeviceD3d11::RasterizerStateRec, HASHKEY_NONE>, 0x00855A00<CGxDeviceD3d11::BlendStateRec, HASHKEY_NONE>, 0x00855C00<CGxDeviceD3d11::DepthStateRec, HASHKEY_NONE>, 0x00855E00<CGxDeviceD3d11::SamplerStateRec, HASHKEY_NONE>, 0x00547CE0<FileNotification::Manifest, HASHKEY_NONE>, 0x009F2D00<CGSpellActivationOverlay::COUNTERNODE, HASHKEY_NONE>, 0x009F2F00<CGSpellActivationOverlay::PETAURANODE, HASHKEY_NONE>, 0x009F6B70<CGResearchFrame::BRANCH, HASHKEY_NONE>, 0x009F7830<CGResearchFrame::BLOBPOIHASH, HASHKEY_NONE>, 0x009CA070<CGCUFProfiles::PROFILENODE, HASHKEY_CONSTUTF8I>, 0x009CA210<CGCUFProfiles::OPTIONENTRY, HASHKEY_CONSTSTR>, 0x0045E370<UIADDON::METADATA, HASHKEY_STRI>, 0x0048C5B0<WowClientDB2_Base::DBTABLE, HASHKEY_NONE>, 0x0048F600<CGObject_C, CHashKeyGUID>, 0x004DE550<TALKINGPLAYER, CHashKeyGUID>, 0x004DE7E0<MUTEDPLAYER, CHashKeyGUID>, 0x004E59E0<DECLINEDWORD, HASHKEY_CONSTSTRI>, 0x00590BA0<CustomFallbackPath, HASHKEY_NONE>, 0x00595EF0<VoidItem_C, HASHKEY_NONE>, 0x005A8230<ITEMBYNAME, HASHKEY_UTF8I>, 0x005E2EE0<OBJ_EFFECT_REF_LOOKUP, HASHKEY_NONE>, 0x005E3200<OBJ_EFFECT_REF_LIST_LOOKUP, HASHKEY_NONE>, 0x005E3C60<OBJ_EFFECT_GROUP_DEF_REF_LIST_LOOKUP, HASHKEY_NONE>, 0x0062CBB0<ZONESOUNDOVERRIDELOOKUP, HASHKEY_STRI>, 0x00633200<DSP_CHAIN_HEAD_LOOKUP, HASHKEY_STRI>, 0x00633F80<FOOTSTEPSNDCACHE, HASHKEY_NONE>, 0x008B2890<ITEMCOOLDOWNHASHNODE, HASHKEY_NONE>, 0x008B2AA0<DESTLOCSPELLCASTINDEXHASHNODE, CHashKeyGUID>, 0x008DC640<SAVEDVARIABLE, HASHKEY_STR>, 0x008F2130<CHATTHROTTLE, HASHKEY_STR>, 0x008F28D0<PROFANITYCHECK, HASHKEY_STR>, 0x008F2B00<SPAMCHECK, HASHKEY_STR>, 0x00900EF0<USEABLESTYLE, HASHKEY_NONE>, 0x00916890<OVERRIDE_SPELL_NODE, HASHKEY_NONE>, 0x009169A0<OVERRIDE_SPELL_BY_NAME_NODE, HASHKEY_UTF8I>, 0x009171C0<SPELLSBYNAME, HASHKEY_UTF8I>, 0x0093ACC0<CGGuildInfo::GuildRecipesKnown, HASHKEY_NONE>, 0x0093AFC0<CGGuildInfo::SKILLLINESPELLS, HASHKEY_NONE>, 0x0094E4E0<ACHIEVEMENT, HASHKEY_NONE>, 0x009894B0<LFGListPlayer, CHashKeyGUID>, 0x009A4C70<CFlyout, HASHKEY_NONE>, 0x009A4EF0<SpellToFlyoutMapItem, HASHKEY_NONE>, 0x009B8820<KNOWNDANCES, HASHKEY_STRI>, 0x009CDF90<WORLDSTATE, HASHKEY_NONE>, 0x009E49B0<TAXIPATHNODESCLIENT, HASHKEY_NONE>, 0x00A0EF40<MacroConditions, HASHKEY_STR>, 0x00B32D00<BITMAPBYICON, HASHKEY_NONE>, 0x004A4070<DBCache<CreatureStats_C, int, HASHKEY_INT>::REVERSEENTRY, HASHKEY_CONSTSTR>, 0x004A4250<DBCache<GameObjectStats_C, int, HASHKEY_INT>::REVERSEENTRY, HASHKEY_CONSTSTR>, 0x004A4430<DBCache<NPCText, int, HASHKEY_INT>::REVERSEENTRY, HASHKEY_CONSTSTR>, 0x004A4610<DBCache<NameCache, uint64_t, CHashKeyGUID>::REVERSEENTRY, HASHKEY_CONSTSTR>, 0x004A47F0<DBCache<GuildStats_C, uint64_t, CHashKeyGUID>::REVERSEENTRY, HASHKEY_CONSTSTR>, 0x004A49D0<DBCache<QuestCache, int, HASHKEY_INT>::REVERSEENTRY, HASHKEY_CONSTSTR>, 0x004A4C00<DBCache<PageTextCache_C, int, HASHKEY_INT>::REVERSEENTRY, HASHKEY_CONSTSTR>, 0x004A4DE0<DBCache<PetNameCache, int, HASHKEY_INT>::REVERSEENTRY, HASHKEY_CONSTSTR>, 0x004A4FC0<DBCache<CGPetition, int, HASHKEY_INT>::REVERSEENTRY, HASHKEY_CONSTSTR>, 0x004A51A0<DBCache<ItemTextCache_C, uint64_t, CHashKeyGUID>::REVERSEENTRY, HASHKEY_CONSTSTR>, 0x004A5380<DBCache<WardenCachedModule, CWardenKey, CWardenKey>::REVERSEENTRY, HASHKEY_CONSTSTR>, 0x004A5560<DBCache<ArenaTeamCache, int, HASHKEY_INT>::REVERSEENTRY, HASHKEY_CONSTSTR>, 0x004ABAE0<DBCache<CreatureStats_C, int, HASHKEY_INT>::DBCACHEHASH, HASHKEY_INT>, 0x004ABB10<DBCache<GameObjectStats_C, int, HASHKEY_INT>::DBCACHEHASH, HASHKEY_INT>, 0x004ABB40<DBCache<NPCText, int, HASHKEY_INT>::DBCACHEHASH, HASHKEY_INT>, 0x004ABB70<DBCache<NameCache, uint64_t, CHashKeyGUID>::DBCACHEHASH, CHashKeyGUID>, 0x004ABBA0<DBCache<GuildStats_C, uint64_t, CHashKeyGUID>::DBCACHEHASH, CHashKeyGUID>, 0x004ABBD0<DBCache<QuestCache, int, HASHKEY_INT>::DBCACHEHASH, HASHKEY_INT>, 0x004ABC00<DBCache<PageTextCache_C, int, HASHKEY_INT>::DBCACHEHASH, HASHKEY_INT>, 0x004ABC30<DBCache<PetNameCache, int, HASHKEY_INT>::DBCACHEHASH, HASHKEY_INT>, 0x004ABC60<DBCache<CGPetition, int, HASHKEY_INT>::DBCACHEHASH, HASHKEY_INT>, 0x004ABC90<DBCache<ItemTextCache_C, uint64_t, CHashKeyGUID>::DBCACHEHASH, CHashKeyGUID>, 0x004ABCC0<DBCache<WardenCachedModule, CWardenKey, CWardenKey>::DBCACHEHASH, CWardenKey>, 0x004ABCF0<DBCache<ArenaTeamCache, int, HASHKEY_INT>::DBCACHEHASH, HASHKEY_INT>, 0x004ABD20<DBCache<DanceCache, int, HASHKEY_INT>::DBCACHEHASH, HASHKEY_INT>]
template <class T, class TKey>
void TSHashTable<T, TKey>::InternalDelete(T* ptr) {
    ptr->~T();
    SMemFree(ptr, typeid(T).raw_name(), SLOG_OBJECT, 0);
}

// OFFSET: [#0x0047C800<PrefetchNode, HASHKEY_STRI>, #0x0047C9D0<UncachableNode, HASHKEY_STRI>, 0x00505AC0<FRAMEATTR, HASHKEY_STRI>, 0x00526740<SIMPLEFONT, HASHKEY_STRI>, #0x004B92B0<CTexture, HASHKEY_TEXTUREFILE>, 0x00548720/0x1401A0A40<CModelBlob::CHashEntry, HASHKEY_MODELBLOBFILE>, #0x004BC760<ProfileInternal::KEYVALUE, HASHKEY_CONSTSTRI>, #0x004BD1F0<ProfileInternal::SECTION, HASHKEY_CONSTSTRI>, 0x00549E20<FONTHASHOBJ, HASHKEY_STR>, #0x004C0540<CTextureBlobTexture, HASHKEY_TEXTUREBLOBFILE>, 0x004D97E0<TIMESTAMPSTRUCT, HASHKEY_NONE>, #0x004CAD50<VSOUND_LOOKUP, HASHKEY_NONE>, #0x004CCB20<SOUNDKITLOOKUP, HASHKEY_STRI>, #0x004CCD10<ZONESOUNDOVERRIDELOOKUP, HASHKEY_STRI>, #0x004CE340<DSP_CHAIN_HEAD_LOOKUP, HASHKEY_STRI>, #0x004CF530<FOOTSTEPSNDCACHE, HASHKEY_NONE>, #0x004D65E0<CGObject_C, CHashKeyGUID>, 0x00704740<CACHEENTRY, HASHKEY_NONE>, #0x00506090<CHATTHROTTLE, HASHKEY_STR>, #0x00506380<WORDLIST, HASHKEY_LANGUAGE>, #0x005065D0<PROFANITYCHECK, HASHKEY_STR>, #0x005067D0<SPAMCHECK, HASHKEY_STR>, #0x00529610<CGGameUI::THREATMAP, CHashKeyGUID>, #0x005296C0<SAVEDVARIABLE, HASHKEY_STR>, #0x005299F0, #0x0052F360<USEABLESTYLE, HASHKEY_NONE>, #0x0053F6D0<SPELLSBYNAME, HASHKEY_UTF8I>, #0x00548FB0<WORLDSTATE, HASHKEY_NONE>, #0x0055A250<LFGListParty, CHashKeyGUID>, #0x0055A400<LFGListPlayer, CHashKeyGUID>, #0x00560690<KEYCOMMAND, HASHKEY_STRI>, #0x00560840<KEYBINDING, HASHKEY_STRI>, %0x0087F820<OVERRIDEKEYBINDING, HASHKEY_STRI>, #0x00560C40<MODIFIEDCLICK, HASHKEY_STRI>, 0x00924B00<CLICKFRAME, HASHKEY_STR>, 0x009C66B0<MACRONODE, HASHKEY_NONE>, #0x005762A0<KNOWNDANCES, HASHKEY_STRI>, #0x00592D70<TAXIPATHNODESCLIENT, HASHKEY_NONE>, #0x005B6450<ACHIEVEMENT, HASHKEY_NONE>, #0x005B65F0<ACHIEVEMENTCRITERIALIST, HASHKEY_NONE>, #0x005C1620, 0x009B4FD0/0x14073E540<DYNAMICHOLIDAYHASH, HASHKEY_NONE>, 0x0099DB40/0x140720D90<TalentGroupItem_C, HASHKEY_NONE>, 0x0099DD50/0x1407210C0<TalentGroupTierItem_C, HASHKEY_NONE>, #0x005D2710<FACTION, HASHKEY_NONE>, #0x005F07B0<MacroConditions, HASHKEY_STR>, #0x005F4070<_BANNEDADDON, HASHKEY_NONE>, #0x005F6090<UIADDON::METADATA, HASHKEY_STRI>, 0x0045E060/0x140070960<ADDONSTATE, HASHKEY_STRI>, #0x005F7270<UIADDON, HASHKEY_STRI>, 0x0044B340<MOUSELOOKBINDING, HASHKEY_STRI>, #0x00618380<PLAYERPORTRAIT, CHashKeyGUID>, #0x006185B0<UNITPORTRAIT, HASHKEY_DWORD>, #0x006187E0<ITEMPORTRAIT, HASHKEY_STRI>, #0x00670270, #0x00670330, #0x00670420, #0x006704E0, #0x00670650, #0x006706D0, #0x006707C0, #0x00670880, #0x00670970, #0x00670A30, #0x00670B20, #0x00670BE0, #0x00670CD0, #0x00670D90, #0x00670E80, #0x00670F40, #0x006710C0, #0x00671140, #0x00671230, #0x006712F0, #0x006713E0, #0x00671560, #0x00671650, #0x00671710, #0x00671800, #0x006718C0, #0x006719B0, #0x00671A70, #0x00671B60, #0x00671C20, 0x00848670/0x140575FC0<CGxShader, HASHKEY_STRI>, 0x00611560/0x14029BDE0<CLightList, HASHKEY_DWORD>, #0x006BEFB0, 0x0061C430<CHARCODEDESC, HASHKEY_NONE>, #0x006C3110<KERNNODE, KERNINGHASHKEY>, 0x00622A40<FACEDATA, HASHKEY_STRI>, #0x006E6520<TRADESKILLLINE, HASHKEY_NONE>, #0x006E6630<PROFICIENCYSPELL, HASHKEY_NONE>, #0x006F3B70<OBJ_EFFECT_GROUP_DEF_REF_LIST_LOOKUP, HASHKEY_NONE>, #0x006F3D10<OBJ_EFFECT_REF_LOOKUP, HASHKEY_NONE>, #0x006F4020<OBJ_EFFECT_REF_LIST_LOOKUP, HASHKEY_NONE>, 0x005E34F0/0x140261FE0<OBJ_EFFECT_DEF_LOOKUP, HASHKEY_NONE>, #0x006F59A0<OBJ_EFFECT_GROUP_DEF_LOOKUP, HASHKEY_NONE>, #0x006F5F00<OBJ_EFFECT_GROUP_LOOKUP, HASHKEY_NONE>, #0x006F5FA0<OBJ_EFFECT_PACKAGE_DEF_LOOKUP, HASHKEY_NONE>, #0x00709F00<INVENTORYART, HASHKEY_NONE>, #0x0070A0C0<ITEMBYNAME, HASHKEY_UTF8I>, #0x00734700<UnitThreat, CHashKeyGUID>, #0x00767A50<CVar, HASHKEY_STRI>, 0x00654830<CONSOLECOMMAND, HASHKEY_CONSTSTRI>, 0x004C2B50/0x1400FD8A0<RGN, HASHKEY_NONE>, #0x007B0800<CMapObj, HASHKEY_NONE>, #0x007B7180, #0x007C0D10<CMapDoodadDef, HASHKEY_DWORD>, #0x007C0E00<CMapObjDef, HASHKEY_NONE>, #0x007DD7B0<TALKINGPLAYER, CHashKeyGUID>, #0x007DD9A0<MUTEDPLAYER, CHashKeyGUID>, #0x007E0450<DECLINEDWORD, HASHKEY_CONSTSTRI>, #0x007F2C50<DayNight::DNOverrideSky, HASHKEY_STRI>, #0x007F5370<MINIMAPMD5NAME, HASHKEY_STRI>, #0x0080A3A0, #0x0080A590<DESTLOCSPELLCASTINDEXHASHNODE, CHashKeyGUID>, 0x008C0C90<RACECLASSINFO, HASHKEY_NONE>, #0x00811210<SKILLCOSTSINFO, HASHKEY_NONE>, #0x00811430<SKILLLINEREC, HASHKEY_SKILLLINE>, 0x004ED420<HashedNode, HASHKEY_STRI>, 0x004ED600<FrameFactoryNode, HASHKEY_STRI>, #0x0081AF40<FrameScript_EventObject, HASHKEY_CONSTSTRI>, #0x0084A100<LCDHANDLE, HASHKEY_CONSTSTRI>, #0x0084B880<BITMAPBYICON, HASHKEY_NONE>, 0x00434A60<CShaderEffect, HASHKEY_STRI>, #0x008778B0<CacheData, HASHKEY_NONE>, #0x0087C040<SOUND_INTERNAL_LOOKUP, HASHKEY_NONE>, 0x0071D2E0<Liquid::Object, HASHKEY_NONE>, 0x0064A9D0<DayNight::DNOverrideSky, HASHKEY_STRI>, 0x0072E610<CMapDoodadDef, HASHKEY_DWORD>, 0x0072E710<CMapObjDef, HASHKEY_NONE>, 0x007360A0/0x140412640<CMapDynamicTexture, HASHKEY_DWORD>, 0x0077BE70<CSceneMaterial, HASHKEY_MATERIAL>, 0x00747DA0<CMapObj, HASHKEY_NONE>, 0x0073A790<DDMaterial, HASHKEY_NONE>, 0x00547930<FileNotification::DeferredFile, HASHKEY_NONE>, 0x00615720<BATCHEDRENDERFONTDESC, HASHKEY_PTR>, 0x0070AA50<SE3SoundChunk, HASHKEY_NONE>, 0x0070BEF0<SE3SOUNDKITOBJECT_LOOKUP, HASHKEY_NONE>, 0x0070C000<SOUNDKITLOOKUP, HASHKEY_CONSTSTRI>, 0x0070C240<SE3SoundEmitterLookup, HASHKEY_NONE>, 0x0070C4D0<SE3COMBINABLESOUNDLIST, HASHKEY_NONE>, 0x00713AF0<SE3ChatSound, HASHKEY_NONE>, 0x00714C90<SE3MemorySoundObject, HASHKEY_NONE>, 0x004FC880/0x140140B50<FILENAMEHASH, HASHKEY_STR>, 0x008555A0<CGxDeviceD3d11::InputLayoutRec, InputLayoutKey>, 0x008557A0<CGxDeviceD3d11::RasterizerStateRec, HASHKEY_NONE>, 0x008559A0<CGxDeviceD3d11::BlendStateRec, HASHKEY_NONE>, 0x00855BA0<CGxDeviceD3d11::DepthStateRec, HASHKEY_NONE>, 0x00855DA0<CGxDeviceD3d11::SamplerStateRec, HASHKEY_NONE>, 0x00547C80<FileNotification::Manifest, HASHKEY_NONE>, 0x009F2CA0<CGSpellActivationOverlay::COUNTERNODE, HASHKEY_NONE>, 0x009F2EA0<CGSpellActivationOverlay::PETAURANODE, HASHKEY_NONE>, 0x009F6BA0<CGResearchFrame::BRANCH, HASHKEY_NONE>, 0x009F76A0<CGResearchFrame::BLOBPOIHASH, HASHKEY_NONE>, 0x009CA000<CGCUFProfiles::PROFILENODE, HASHKEY_CONSTUTF8I>, 0x009CA1A0<CGCUFProfiles::OPTIONENTRY, HASHKEY_CONSTSTR>, 0x00439F70<CCameraModeEntry, HASHKEY_NONE>, 0x0045E3B0<UIADDON::METADATA, HASHKEY_STRI>, 0x0048C5E0<WowClientDB2_Base::DBTABLE, HASHKEY_NONE>, 0x00490180<CGObject_C, CHashKeyGUID>, 0x004E5970<DECLINEDWORD, HASHKEY_CONSTSTRI>, 0x0054BBB0<CTextureBlobTexture, HASHKEY_TEXTUREBLOBFILE>, 0x0056D590<TRADESKILLLINE, HASHKEY_NONE>, 0x0056D6C0<PROFICIENCYSPELL, HASHKEY_NONE>, 0x00595E90<VoidItem_C, HASHKEY_NONE>, 0x005A7FE0<INVENTORYART, HASHKEY_NONE>, 0x005A81C0<ITEMBYNAME, HASHKEY_UTF8I>, 0x005E2E80<OBJ_EFFECT_REF_LOOKUP, HASHKEY_NONE>, 0x005E3180<OBJ_EFFECT_REF_LIST_LOOKUP, HASHKEY_NONE>, 0x005E3BE0<OBJ_EFFECT_GROUP_DEF_REF_LIST_LOOKUP, HASHKEY_NONE>, 0x0062CB40<ZONESOUNDOVERRIDELOOKUP, HASHKEY_STRI>, 0x00633FB0<FOOTSTEPSNDCACHE, HASHKEY_NONE>, 0x008B2830<ITEMCOOLDOWNHASHNODE, HASHKEY_NONE>, 0x008DC5D0<SAVEDVARIABLE, HASHKEY_STR>, 0x008F2920<PROFANITYCHECK, HASHKEY_STR>, 0x008F2B50<SPAMCHECK, HASHKEY_STR>, 0x00900E10<USEABLESTYLE, HASHKEY_NONE>, 0x009169D0<OVERRIDE_SPELL_BY_NAME_NODE, HASHKEY_UTF8I>, 0x0093AC50<CGGuildInfo::GuildRecipesKnown, HASHKEY_NONE>, 0x0093AF60<CGGuildInfo::SKILLLINESPELLS, HASHKEY_NONE>, 0x0094E460<ACHIEVEMENT, HASHKEY_NONE>, 0x00966DA0<CGQuestLog::QUESTCOMPLETIONNPC, HASHKEY_NONE>, 0x009A4F20<SpellToFlyoutMapItem, HASHKEY_NONE>, 0x009B87B0<KNOWNDANCES, HASHKEY_STRI>, 0x009CDF30<WORLDSTATE, HASHKEY_NONE>, 0x009E4950<TAXIPATHNODESCLIENT, HASHKEY_NONE>, 0x00A0EEC0<MacroConditions, HASHKEY_STR>, 0x004A3F20<DBCache<CreatureStats_C, int, HASHKEY_INT>::DBCACHEHASH, HASHKEY_INT>, 0x004A4000<DBCache<CreatureStats_C, int, HASHKEY_INT>::REVERSEENTRY, HASHKEY_CONSTSTR>, 0x004A4110<DBCache<GameObjectStats_C, int, HASHKEY_INT>::DBCACHEHASH, HASHKEY_INT>, 0x004A41E0<DBCache<GameObjectStats_C, int, HASHKEY_INT>::REVERSEENTRY, HASHKEY_CONSTSTR>, 0x004A43C0<DBCache<NPCText, int, HASHKEY_INT>::REVERSEENTRY, HASHKEY_CONSTSTR>, 0x004A45A0<DBCache<NameCache, uint64_t, CHashKeyGUID>::REVERSEENTRY, HASHKEY_CONSTSTR>, 0x004A46B0<DBCache<GuildStats_C, uint64_t, CHashKeyGUID>::DBCACHEHASH, CHashKeyGUID>, 0x004A4780<DBCache<GuildStats_C, uint64_t, CHashKeyGUID>::REVERSEENTRY, HASHKEY_CONSTSTR>, 0x004A4890<DBCache<QuestCache, int, HASHKEY_INT>::DBCACHEHASH, HASHKEY_INT>, 0x004A4960<DBCache<QuestCache, int, HASHKEY_INT>::REVERSEENTRY, HASHKEY_CONSTSTR>, 0x004A4B90<DBCache<PageTextCache_C, int, HASHKEY_INT>::REVERSEENTRY, HASHKEY_CONSTSTR>, 0x004A4D70<DBCache<PetNameCache, int, HASHKEY_INT>::REVERSEENTRY, HASHKEY_CONSTSTR>, 0x004A4F50<DBCache<CGPetition, int, HASHKEY_INT>::REVERSEENTRY, HASHKEY_CONSTSTR>, 0x004A5130<DBCache<ItemTextCache_C, uint64_t, CHashKeyGUID>::REVERSEENTRY, HASHKEY_CONSTSTR>, 0x004A5310<DBCache<WardenCachedModule, CWardenKey, CWardenKey>::REVERSEENTRY, HASHKEY_CONSTSTR>, 0x004A54F0<DBCache<ArenaTeamCache, int, HASHKEY_INT>::REVERSEENTRY, HASHKEY_CONSTSTR>]
template <class T, class TKey>
T* TSHashTable<T, TKey>::InternalNew(TSExplicitList<T>* listptr, size_t extrabytes, uint32_t flags) {
    return listptr->NewNode(1, extrabytes, flags);
}

// OFFSET: [#0x0047C870, #0x0047CA60, #0x0047CB60, #0x0047CB90, #0x004921C0, #0x004922C0, #0x00496AD0, #0x00496BD0, #0x004B84E0, #0x004B85E0, #0x004BB410, #0x004BB510, #0x004BC6C0, %0x00558160, #0x004BCA30, %0x00558290, #0x004BE5C0, #0x004BE6C0, #0x004C0890, #0x004C0990, %0x004DFA70, %0x004DFD40, #0x004CAE00, #0x004CAF00, #0x004CCBB0, #0x004CCDA0, #0x004CCEA0, #0x004CCED0, #0x004CE3C0, #0x004CE4C0, #0x004CF590, #0x004CF690, #0x004D6040, #0x004D6140, #0x004F34D0, #0x004F35D0, #0x00506110, #0x00506430, #0x00506630, #0x00506830, #0x00506930, #0x00506960, #0x00506990, #0x005069C0, #0x00529750, #0x00529850, #0x00529A60, #0x00529B60, #0x00529B90, #0x0052A130, #0x0052F400, #0x0052F500, #0x0053F770, #0x0053F870, #0x00549040, #0x00549140, #0x0055A270, #0x0055A480, #0x0055A580, #0x0055A5B0, %0x0087F480, %0x0087F620, %0x0087F840, %0x0087FA60, %0x0087FC70, %0x0087FD70, %0x0087FDA0, %0x0087FDD0, %0x0087FE00, %0x0087FE30, #0x00567700, #0x00567800, #0x00576330, #0x00576430, #0x00592E00, #0x00593080, #0x005B64F0, #0x005B6690, #0x005B6790, #0x005B67C0, %0x00909B40, %0x00909D50, %0x00909E50, %0x00909E80, %0x008F3380, %0x008F3590, %0x008F3690, %0x008F36C0, #0x005D27C0, #0x005D28C0, #0x005F0850, #0x005F0950, #0x005F40E0, #0x005F41E0, #0x005F5FB0, %0x00466EB0, %0x00467090, #0x005F6500, %0x004671C0, %0x004671F0, %0x00455B40, %0x00455C40, #0x00618450, #0x00618680, #0x006188A0, #0x006189A0, #0x006189D0, #0x00618A00, #0x00672CA0, #0x00672E50, #0x00672F50, #0x00673100, #0x00673200, #0x006733B0, #0x006734B0, #0x00673660, #0x00673760, #0x00673910, #0x00673A10, #0x00673BD0, #0x00673CD0, #0x00673E70, #0x00673F70, #0x00674110, #0x00674210, #0x006743C0, #0x006744C0, #0x00674670, #0x00674770, #0x00674910, #0x00674A10, #0x00674BB0, #0x00674CB0, #0x00674E50, #0x00674F50, #0x006750F0, #0x006751F0, #0x006753B0, #0x006754B0, #0x006754E0, #0x00675510, #0x00675540, #0x00675570, #0x006755A0, #0x006755D0, #0x00675600, #0x00675630, #0x00675660, #0x00675690, #0x006756C0, #0x006756F0, #0x00675720, #0x00675750, #0x00675780, #0x006757B0, #0x006757E0, #0x00675810, #0x00675840, #0x00675870, #0x006758A0, #0x006758D0, #0x00675900, #0x00675930, #0x00675960, #0x00675990, #0x006759C0, #0x006759F0, #0x00675A20, #0x00688560, #0x00688660, %0x006221F0, %0x006222F0, %0x00626270<BATCHEDRENDERFONTDESC, HASHKEY_PTR>, %0x00626370, #0x006C2D30, #0x006C2E30, #0x006C3180, #0x006C3280, %0x00633260, %0x00633360, #0x006E66C0, #0x006E67C0, #0x006E68C0, #0x006E68F0, #0x006F3AD0, #0x006F3C70, #0x006F3EC0, #0x006F40C0, #0x006F4310, #0x006F44D0, #0x006F4690, #0x006F4790, #0x006F47C0, #0x006F47F0, #0x006F4820, #0x006F4850, #0x006F4880, #0x006F48B0, #0x00709F60, #0x0070A150, #0x0070A250, #0x0070A280, #0x00734910, #0x00734A10, #0x00767BC0, #0x00767CC0, #0x00768D00, #0x00768E00, %0x004C8F80<RGN, HASHKEY_NONE>, #0x007B0860, #0x007B0960, #0x007B71F0, #0x007B72F0, #0x007C0E60, #0x007C0F60, #0x007C1060, #0x007C1090, #0x007DD840, #0x007DDA30, #0x007DDB30, #0x007DDB60, #0x007E04E0, #0x007E05E0, #0x007F2CC0, #0x007F2DC0, #0x007F5400, #0x007F5500, #0x0080A430<ITEMCOOLDOWNHASHNODE, HASHKEY_NONE>, #0x0080A620, #0x0080A720, #0x0080A750, #0x008110B0, #0x008112A0, #0x008114A0, #0x008115A0, #0x008115D0, #0x00811D10, #0x008132F0, #0x008134E0, #0x008135E0, #0x00813610, #0x0081AFD0, #0x0081B0D0, #0x0084A1B0, #0x0084A2B0, #0x0084B910, #0x0084BA10, #0x008767E0, #0x008768E0, #0x00877910, #0x00877A10, #0x0087C0A0, #0x0087C1A0, 0x0071D370<Liquid::Object, HASHKEY_NONE>, 0x0071D470<Liquid::Object, HASHKEY_NONE>, 0x0064AA60<DayNight::DNOverrideSky, HASHKEY_STRI>, 0x0064AB60<DayNight::DNOverrideSky, HASHKEY_STRI>, 0x0072E990<CMapDoodadDef, HASHKEY_DWORD>, 0x0072EB90<CMapDoodadDef, HASHKEY_DWORD>, 0x0072EA90<CMapObjDef, HASHKEY_NONE>, 0x0072EBC0<CMapObjDef, HASHKEY_NONE>, 0x00736140/0x140412790<CMapDynamicTexture, HASHKEY_DWORD>, 0x0077BFA0<CSceneMaterial, HASHKEY_MATERIAL>, 0x0077C0A0<CSceneMaterial, HASHKEY_MATERIAL>, 0x00736240/0x1404128D0<CMapDynamicTexture, HASHKEY_DWORD>, 0x00747F10<CMapObj, HASHKEY_NONE>, 0x00748010<CMapObj, HASHKEY_NONE>, 0x0073A810<DDMaterial, HASHKEY_NONE>, 0x0073A910<DDMaterial, HASHKEY_NONE>, 0x004470E0<_BANNEDADDON, HASHKEY_NONE>, 0x004471E0<_BANNEDADDON, HASHKEY_NONE>, 0x004ED6A0<FrameFactoryNode, HASHKEY_STRI>, 0x004ED7D0<FrameFactoryNode, HASHKEY_STRI>, 0x00505C70/0x14014C960<FRAMEATTR, HASHKEY_STRI>, 0x00505D70/0x14014CAA0<FRAMEATTR, HASHKEY_STRI>, 0x00526350<SIMPLEFONT, HASHKEY_STRI>, 0x00526450<SIMPLEFONT, HASHKEY_STRI>, 0x005438D0<CTexture, HASHKEY_TEXTUREFILE>, 0x005439D0<CTexture, HASHKEY_TEXTUREFILE>, 0x005479D0<FileNotification::DeferredFile, HASHKEY_NONE>, 0x00547E10<FileNotification::DeferredFile, HASHKEY_NONE>, 0x005E3510/0x140261FF0<OBJ_EFFECT_DEF_LOOKUP, HASHKEY_NONE>, 0x005E3F50/0x140262BF0<OBJ_EFFECT_DEF_LOOKUP, HASHKEY_NONE>, 0x005E38B0/0x1402623A0<OBJ_EFFECT_GROUP_DEF_LOOKUP, HASHKEY_NONE>, 0x005E3F80/0x140262C40<OBJ_EFFECT_GROUP_DEF_LOOKUP, HASHKEY_NONE>, 0x005E3D90/0x140262970<OBJ_EFFECT_PACKAGE_DEF_LOOKUP, HASHKEY_NONE>, 0x005E3FB0/0x140262C90<OBJ_EFFECT_PACKAGE_DEF_LOOKUP, HASHKEY_NONE>, 0x00656710<CVar, HASHKEY_STRI>, 0x00656810<CVar, HASHKEY_STRI>, 0x008486F0/0x1405760E0<CGxShader, HASHKEY_STRI>, 0x008487F0/0x140576220<CGxShader, HASHKEY_STRI>, 0x00B31930<LCDHANDLE, HASHKEY_CONSTSTRI>, 0x00B31A30<LCDHANDLE, HASHKEY_CONSTSTRI>, 0x00548790/0x1401A0B50<CModelBlob::CHashEntry, HASHKEY_MODELBLOBFILE>, 0x00548890/0x1401A0C90<CModelBlob::CHashEntry, HASHKEY_MODELBLOBFILE>, 0x0083F040<FrameScript_EventObject, HASHKEY_CONSTSTRI>, 0x0083F140<FrameScript_EventObject, HASHKEY_CONSTSTRI>, 0x00434AC0<CShaderEffect, HASHKEY_STRI>, 0x00434BC0<CShaderEffect, HASHKEY_STRI>, 0x0054BD30/0x1401A54D0<CTextureBlobTexture, HASHKEY_TEXTUREBLOBFILE>, 0x0054BE30/0x1401A5610<CTextureBlobTexture, HASHKEY_TEXTUREBLOBFILE>, 0x00704760<CACHEENTRY, HASHKEY_NONE>, 0x00704860<CACHEENTRY, HASHKEY_NONE>, 0x009C6710<MACRONODE, HASHKEY_NONE>, 0x009C6810<MACRONODE, HASHKEY_NONE>, 0x00611720/0x14029C1B0<CLightList, HASHKEY_DWORD>, 0x00611820/0x14029C2F0<CLightList, HASHKEY_DWORD>, 0x0070AAD0<SE3SoundChunk, HASHKEY_NONE>, 0x0070AD60<SE3SoundChunk, HASHKEY_NONE>, 0x0070CEE0<SE3SOUNDKITOBJECT_LOOKUP, HASHKEY_NONE>, 0x0070D010<SE3SOUNDKITOBJECT_LOOKUP, HASHKEY_NONE>, 0x0070C0A0<SOUNDKITLOOKUP, HASHKEY_CONSTSTRI>, 0x0070C3A0<SOUNDKITLOOKUP, HASHKEY_CONSTSTRI>, 0x0070C2A0<SE3SoundEmitterLookup, HASHKEY_NONE>, 0x0070C3D0<SE3SoundEmitterLookup, HASHKEY_NONE>, 0x0070CDE0/0x1403DB8A0<SE3COMBINABLESOUNDLIST, HASHKEY_NONE>, 0x0070CFE0<SE3COMBINABLESOUNDLIST, HASHKEY_NONE>, 0x00713C90<SE3ChatSound, HASHKEY_NONE>, 0x00713D90<SE3ChatSound, HASHKEY_NONE>, 0x00714E10<SE3MemorySoundObject, HASHKEY_NONE>, 0x00714F10<SE3MemorySoundObject, HASHKEY_NONE>, 0x004FCA30/0x140140E40<FILENAMEHASH, HASHKEY_STR>, 0x004FCB30/0x140140F80<FILENAMEHASH, HASHKEY_STR>, 0x00855630<CGxDeviceD3d11::InputLayoutRec, InputLayoutKey>, 0x00855F30<CGxDeviceD3d11::InputLayoutRec, InputLayoutKey>, 0x00855830<CGxDeviceD3d11::RasterizerStateRec, HASHKEY_NONE>, 0x00855F60<CGxDeviceD3d11::RasterizerStateRec, HASHKEY_NONE>, 0x00855A30<CGxDeviceD3d11::BlendStateRec, HASHKEY_NONE>, 0x00855F90<CGxDeviceD3d11::BlendStateRec, HASHKEY_NONE>, 0x00855C30<CGxDeviceD3d11::DepthStateRec, HASHKEY_NONE>, 0x00855FC0<CGxDeviceD3d11::DepthStateRec, HASHKEY_NONE>, 0x00855E30<CGxDeviceD3d11::SamplerStateRec, HASHKEY_NONE>, 0x00855FF0<CGxDeviceD3d11::SamplerStateRec, HASHKEY_NONE>, 0x00547D10<FileNotification::Manifest, HASHKEY_NONE>, 0x00547E40<FileNotification::Manifest, HASHKEY_NONE>, 0x00549E90<FONTHASHOBJ, HASHKEY_STR>, 0x00549F90<FONTHASHOBJ, HASHKEY_STR>, 0x004ED490<HashedNode, HASHKEY_STRI>, 0x004ED7A0<HashedNode, HASHKEY_STRI>, 0x0061C450<CHARCODEDESC, HASHKEY_NONE>, 0x0061C550<CHARCODEDESC, HASHKEY_NONE>, 0x0061C8C0<KERNNODE, KERNINGHASHKEY>, 0x0061C9C0<KERNNODE, KERNINGHASHKEY>, 0x006548A0<CONSOLECOMMAND, HASHKEY_CONSTSTRI>, 0x006549A0<CONSOLECOMMAND, HASHKEY_CONSTSTRI>, 0x009F2D30<CGSpellActivationOverlay::COUNTERNODE, HASHKEY_NONE>, 0x009F2F30<CGSpellActivationOverlay::PETAURANODE, HASHKEY_NONE>, 0x009F6C20<CGResearchFrame::BRANCH, HASHKEY_NONE>, 0x009F6D90<CGResearchFrame::BLOBPOIHASH, HASHKEY_NONE>, 0x009CA0A0<CGCUFProfiles::PROFILENODE, HASHKEY_CONSTUTF8I>, 0x009CA240<CGCUFProfiles::OPTIONENTRY, HASHKEY_CONSTSTR>, 0x00439FD0<CCameraModeEntry, HASHKEY_NONE>, 0x00484400<PLAYERPORTRAIT, CHashKeyGUID>, 0x00484650<UNITPORTRAIT, HASHKEY_NONE>, 0x004848A0<ITEMPORTRAIT, HASHKEY_STR>, 0x0048C650<WowClientDB2_Base::DBTABLE, HASHKEY_NONE>, 0x00496270<SPELLACTIVATIONOVERLAY, HASHKEY_NONE>, 0x004DE580<TALKINGPLAYER, CHashKeyGUID>, 0x004DE810<MUTEDPLAYER, CHashKeyGUID>, 0x004E5A10<DECLINEDWORD, HASHKEY_CONSTSTRI>, 0x00590C50<CustomFallbackPath, HASHKEY_NONE>, 0x005A8050<INVENTORYART, HASHKEY_NONE>, 0x005A8260<ITEMBYNAME, HASHKEY_UTF8I>, 0x0062CBE0<ZONESOUNDOVERRIDELOOKUP, HASHKEY_STRI>, 0x006332C0<DSP_CHAIN_HEAD_LOOKUP, HASHKEY_STRI>, 0x00634010<FOOTSTEPSNDCACHE, HASHKEY_NONE>, 0x008B28C0<ITEMCOOLDOWNHASHNODE, HASHKEY_NONE>, 0x008B2AD0<DESTLOCSPELLCASTINDEXHASHNODE, CHashKeyGUID>, 0x008C0D20<RACECLASSINFO, HASHKEY_NONE>, 0x009171F0<SPELLSBYNAME, HASHKEY_UTF8I>, 0x0093ACF0<CGGuildInfo::GuildRecipesKnown, HASHKEY_NONE>, 0x0093B0F0<CGGuildInfo::SKILLLINESPELLS, HASHKEY_NONE>, 0x00966E60<CGQuestLog::QUESTCOMPLETIONNPC, HASHKEY_NONE>, 0x009A4D20<CFlyout, HASHKEY_NONE>, 0x009A4F90<SpellToFlyoutMapItem, HASHKEY_NONE>, 0x009A87E0<FACTION, HASHKEY_NONE>, 0x009FD610<CGIncomingResurrection::INCOMINGRESLIST, CHashKeyGUID>, 0x008DC8A0<TAINTEVENT, HASHKEY_STR>, 0x008F2760<WORDLIST, HASHKEY_LANGUAGE>, 0x0094E510<ACHIEVEMENT, HASHKEY_NONE>, 0x009B8850<KNOWNDANCES, HASHKEY_STRI>, 0x009CDFC0<WORLDSTATE, HASHKEY_NONE>, 0x00B32D30<BITMAPBYICON, HASHKEY_NONE>, 0x0048F890<CGObject_C, CHashKeyGUID>, 0x008F2410<ADDONPREFIX, HASHKEY_STR>]
template <class T, class TKey>
TSHashTable<T, TKey>::~TSHashTable() {
    this->InternalClear(1);
}

// OFFSET: [#0x0047C750<PrefetchNode, HASHKEY_STRI>, #0x0047C970<UncachableNode, HASHKEY_STRI>, 0x00505A50/0x14014C5E0<FRAMEATTR, HASHKEY_STRI>, 0x00526190<SIMPLEFONT, HASHKEY_STRI>, 0x00543000<CTexture, HASHKEY_TEXTUREFILE>, 0x00548670<CModelBlob::CHashEntry, HASHKEY_MODELBLOBFILE>, #0x004BC870<ProfileInternal::SECTION, HASHKEY_CONSTSTRI>, #0x004BC8D0<ProfileInternal::KEYVALUE, HASHKEY_CONSTSTRI>, 0x00549D30<FONTHASHOBJ, HASHKEY_STR>, 0x0054BB00<CTextureBlobTexture, HASHKEY_TEXTUREBLOBFILE>, #0x004CACF0<VSOUND_LOOKUP, HASHKEY_NONE>, #0x004CCAC0<SOUNDKITLOOKUP, HASHKEY_STRI>, #0x004CCCB0<ZONESOUNDOVERRIDELOOKUP, HASHKEY_STRI>, #0x004CE2B0<DSP_CHAIN_HEAD_LOOKUP, HASHKEY_STRI>, #0x004CF430<FOOTSTEPSNDCACHE, HASHKEY_NONE>, #0x004D5D70<CGObject_C, CHashKeyGUID>, 0x00704610<CACHEENTRY, HASHKEY_NONE>, #0x00506000<CHATTHROTTLE, HASHKEY_STR>, #0x00506320<WORDLIST, HASHKEY_LANGUAGE>, #0x00506530<PROFANITYCHECK, HASHKEY_STR>, #0x00506730<SPAMCHECK, HASHKEY_STR>, #0x005295B0<CGGameUI::THREATMAP, CHashKeyGUID>, #0x00529950, #0x0052F300<USEABLESTYLE, HASHKEY_NONE>, #0x0053F670<SPELLSBYNAME, HASHKEY_UTF8I>, #0x00548F50<WORLDSTATE, HASHKEY_NONE>, #0x0055A130<LFGListParty, CHashKeyGUID>, #0x0055A370<LFGListPlayer, CHashKeyGUID>, #0x005605F0<KEYCOMMAND, HASHKEY_STRI>, #0x005609A0<OVERRIDEKEYBINDING, HASHKEY_STRI>, 0x00924870/0x140688F00<MODIFIEDCLICK, HASHKEY_STRI>, 0x00924A90<CLICKFRAME, HASHKEY_STR>, 0x009C6610<MACRONODE, HASHKEY_NONE>, #0x00576240<KNOWNDANCES, HASHKEY_STRI>, #0x00592D10<TAXIPATHNODESCLIENT, HASHKEY_NONE>, #0x005B63F0<ACHIEVEMENT, HASHKEY_NONE>, 0x009B4F60<DYNAMICHOLIDAYHASH, HASHKEY_NONE>, #0x005C8530<TalentGroupItem_C, HASHKEY_NONE>, #0x005C8730<TalentGroupTierItem_C, HASHKEY_NONE>, #0x005D26B0<FACTION, HASHKEY_NONE>, #0x005F0750<MacroConditions, HASHKEY_STR>, 0x00446FD0<_BANNEDADDON, HASHKEY_NONE>, #0x005F6150<ADDONSTATE, HASHKEY_STRI>, #0x005F6340, #0x005F63A0<UIADDON::METADATA, HASHKEY_STRI>, 0x0044B1F0<MOUSELOOKBINDING, HASHKEY_STRI>, #0x00618320<PLAYERPORTRAIT, CHashKeyGUID>, #0x00618550<UNITPORTRAIT, HASHKEY_DWORD>, #0x00618780<ITEMPORTRAIT, HASHKEY_STRI>, #0x00670210, #0x006702D0, #0x006703C0, #0x00670480, #0x00670570, #0x00670670, #0x00670760, #0x00670820, #0x00670910, #0x006709D0, #0x00670AC0, #0x00670B80, #0x00670C70, #0x00670D30, #0x00670E20, #0x00670EE0, #0x00670FD0, #0x006710E0, #0x006711D0, #0x00671290, #0x00671380, #0x00671440, #0x006715F0, #0x006716B0, #0x006717A0, #0x00671860, #0x00671950, #0x00671A10, #0x00671B00, #0x00671BC0, 0x006114C0<CLightList, HASHKEY_DWORD>, #0x006BEF20, 0x0061C2D0<CHARCODEDESC, HASHKEY_NONE>, 0x0061C7A0<KERNNODE, KERNINGHASHKEY>, 0x006229A0<FACEDATA, HASHKEY_STRI>, #0x006E64C0<TRADESKILLLINE, HASHKEY_NONE>, #0x006E65D0<PROFICIENCYSPELL, HASHKEY_NONE>, #0x006F3E00<OBJ_EFFECT_GROUP_LOOKUP, HASHKEY_NONE>, #0x006F3E60<OBJ_EFFECT_REF_LOOKUP, HASHKEY_NONE>, #0x006F3FC0<OBJ_EFFECT_REF_LIST_LOOKUP, HASHKEY_NONE>, 0x005E3330<OBJ_EFFECT_DEF_LOOKUP, HASHKEY_NONE>, 0x005E3610<OBJ_EFFECT_GROUP_DEF_LOOKUP, HASHKEY_NONE>, 0x005E39B0<OBJ_EFFECT_PACKAGE_DEF_LOOKUP, HASHKEY_NONE>, #0x006F4630<OBJ_EFFECT_GROUP_DEF_REF_LIST_LOOKUP, HASHKEY_NONE>, #0x00709E60<INVENTORYART, HASHKEY_NONE>, #0x0070A060<ITEMBYNAME, HASHKEY_UTF8I>, #0x007346A0<UnitThreat, CHashKeyGUID>, 0x006565F0<CVar, HASHKEY_STRI>, 0x00654790<CONSOLECOMMAND, HASHKEY_CONSTSTRI>, #0x0076D370<TIMESTAMPSTRUCT, HASHKEY_NONE>, 0x00590B30<RGN, HASHKEY_NONE>, #0x007B0600<CMapObj, HASHKEY_NONE>, #0x007B70B0, #0x007C0C80<CMapDoodadDef, HASHKEY_DWORD>, #0x007C0D70<CMapObjDef, HASHKEY_NONE>, #0x007DD750<TALKINGPLAYER, CHashKeyGUID>, #0x007DD940<MUTEDPLAYER, CHashKeyGUID>, #0x007E03F0<DECLINEDWORD, HASHKEY_CONSTSTRI>, #0x007F2BC0<DayNight::DNOverrideSky, HASHKEY_STRI>, #0x007F5310<MINIMAPMD5NAME, HASHKEY_STRI>, #0x0080A340<ITEMCOOLDOWNHASHNODE, HASHKEY_NONE>, #0x0080A530<DESTLOCSPELLCASTINDEXHASHNODE, CHashKeyGUID>, 0x008C0C20<RACECLASSINFO, HASHKEY_NONE>, #0x008111B0<SKILLCOSTSINFO, HASHKEY_NONE>, #0x008113A0<SKILLLINEREC, HASHKEY_SKILLLINE>, 0x004ED350<HashedNode, HASHKEY_STRI>, 0x004ED590<FrameFactoryNode, HASHKEY_STRI>, 0x0083ED90<FrameScript_EventObject, HASHKEY_CONSTSTRI>, 0x00B31810<LCDHANDLE, HASHKEY_CONSTSTRI>, #0x0084B820<BITMAPBYICON, HASHKEY_NONE>, 0x004349C0<CShaderEffect, HASHKEY_STRI>, #0x008777F0<CacheData, HASHKEY_NONE>, #0x0087BFB0<SOUND_INTERNAL_LOOKUP, HASHKEY_NONE>, 0x0071D240<Liquid::Object, HASHKEY_NONE>, 0x0064A960<DayNight::DNOverrideSky, HASHKEY_STRI>, 0x0072E570<CMapDoodadDef, HASHKEY_DWORD>, 0x0072E670<CMapObjDef, HASHKEY_NONE>, 0x00736030<CMapDynamicTexture, HASHKEY_DWORD>, 0x0077BD00<CSceneMaterial, HASHKEY_MATERIAL>, 0x00747D00<CMapObj, HASHKEY_NONE>, 0x0073A5C0<DDMaterial, HASHKEY_NONE>, 0x005478C0<FileNotification::DeferredFile, HASHKEY_NONE>, 0x0070A9B0<SE3SoundChunk, HASHKEY_NONE>, 0x0070BE80<SE3SOUNDKITOBJECT_LOOKUP, HASHKEY_NONE>, 0x0070BF80<SOUNDKITLOOKUP, HASHKEY_CONSTSTRI>, 0x0070C1A0<SE3SoundEmitterLookup, HASHKEY_NONE>, 0x0070C400<SE3COMBINABLESOUNDLIST, HASHKEY_NONE>, 0x00713A10<SE3ChatSound, HASHKEY_NONE>, 0x00714BF0<SE3MemorySoundObject, HASHKEY_NONE>, 0x004FC770<FILENAMEHASH, HASHKEY_STR>, 0x00855530/0x140585AC0<CGxDeviceD3d11::InputLayoutRec, InputLayoutKey>, 0x00855730/0x140585DD0<CGxDeviceD3d11::RasterizerStateRec, HASHKEY_NONE>, 0x00855930/0x1405860E0<CGxDeviceD3d11::BlendStateRec, HASHKEY_NONE>, 0x00855B30/0x1405863F0<CGxDeviceD3d11::DepthStateRec, HASHKEY_NONE>, 0x00855D30/0x140586700<CGxDeviceD3d11::SamplerStateRec, HASHKEY_NONE>, 0x00547AD0<FileNotification::Manifest, HASHKEY_NONE>, 0x009F2C30<CGSpellActivationOverlay::COUNTERNODE, HASHKEY_NONE>, 0x009F2E30<CGSpellActivationOverlay::PETAURANODE, HASHKEY_NONE>, 0x009F6B00<CGResearchFrame::BRANCH, HASHKEY_NONE>, 0x009F6D20<CGResearchFrame::BLOBPOIHASH, HASHKEY_NONE>, 0x009C9F90<CGCUFProfiles::PROFILENODE, HASHKEY_CONSTUTF8I>, 0x00439EB0<CCameraModeEntry, HASHKEY_NONE>, 0x004842B0<PLAYERPORTRAIT, CHashKeyGUID>, 0x00484500<UNITPORTRAIT, HASHKEY_NONE>, 0x00484750<ITEMPORTRAIT, HASHKEY_STR>, 0x0048C540<WowClientDB2_Base::DBTABLE, HASHKEY_NONE>, 0x00496170<SPELLACTIVATIONOVERLAY, HASHKEY_NONE>, 0x004DE470<TALKINGPLAYER, CHashKeyGUID>, 0x004DE680<MUTEDPLAYER, CHashKeyGUID>, 0x004E5900<DECLINEDWORD, HASHKEY_CONSTSTRI>, 0x005A7F20<INVENTORYART, HASHKEY_NONE>, 0x005A8150<ITEMBYNAME, HASHKEY_UTF8I>, 0x0062CAD0<ZONESOUNDOVERRIDELOOKUP, HASHKEY_STRI>, 0x00633140<DSP_CHAIN_HEAD_LOOKUP, HASHKEY_STRI>, 0x00633E00<FOOTSTEPSNDCACHE, HASHKEY_NONE>, 0x008B27C0<ITEMCOOLDOWNHASHNODE, HASHKEY_NONE>, 0x008B29C0<DESTLOCSPELLCASTINDEXHASHNODE, CHashKeyGUID>, 0x00916820<OVERRIDE_SPELL_NODE, HASHKEY_NONE>, 0x00916930<OVERRIDE_SPELL_BY_NAME_NODE, HASHKEY_UTF8I>, 0x00917000<SPELLSBYNAME, HASHKEY_UTF8I>, 0x0093ABE0<CGGuildInfo::GuildRecipesKnown, HASHKEY_NONE>, 0x0093ADF0<CGGuildInfo::SKILLLINESPELLS, HASHKEY_NONE>, 0x00966D30<CGQuestLog::QUESTCOMPLETIONNPC, HASHKEY_NONE>, 0x009A4C00<CFlyout, HASHKEY_NONE>, 0x009A4E20<SpellToFlyoutMapItem, HASHKEY_NONE>, 0x009A86B0<FACTION, HASHKEY_NONE>, 0x009FD4D0<CGIncomingResurrection::INCOMINGRESLIST, CHashKeyGUID>, 0x0045E200<UIADDON, HASHKEY_STRI>, 0x008DC770<TAINTEVENT, HASHKEY_STR>, 0x008F2620<WORDLIST, HASHKEY_LANGUAGE>, 0x0094E3F0<ACHIEVEMENT, HASHKEY_NONE>, 0x009B8740<KNOWNDANCES, HASHKEY_STRI>, 0x009CDEC0<WORLDSTATE, HASHKEY_NONE>, 0x00B32C20<BITMAPBYICON, HASHKEY_NONE>, 0x0048F590<CGObject_C, CHashKeyGUID>, 0x008F2300<ADDONPREFIX, HASHKEY_STR>]
template <class T, class TKey>
TSHashTable<T, TKey>::TSHashTable() {
    this->m_fullnessIndicator = 0;
    this->m_fulllist.ChangeLinkOffset(offsetof(T, m_linktofull));
    this->m_slotmask = 0xFFFFFFFF;
    this->m_maxslots = 0x2000;
}

// OFFSET: [0x005D4E10<RGN, HASHKEY_NONE>]
template <class T, class TKey>
void TSHashTable<T, TKey>::Clear() {
    this->InternalClear(0);
}

// OFFSET: [#0x0047C910<PrefetchNode, HASHKEY_STRI>, #0x0047CB00<UncachableNode, HASHKEY_STRI>, 0x00505D10<FRAMEATTR, HASHKEY_STRI>, 0x005263F0<SIMPLEFONT, HASHKEY_STRI>, 0x00543970<CTexture, HASHKEY_TEXTUREFILE>, 0x00548830/0x1401A0C10<CModelBlob::CHashEntry, HASHKEY_MODELBLOBFILE>, #0x004BC810<ProfileInternal::KEYVALUE, HASHKEY_CONSTSTRI>, %0x00558200<ProfileInternal::SECTION, HASHKEY_CONSTSTRI>, 0x00549F30<FONTHASHOBJ, HASHKEY_STR>, 0x0054BDD0/0x1401A5590<CTextureBlobTexture, HASHKEY_TEXTUREBLOBFILE>, %0x004DFCE0<TIMESTAMPSTRUCT, HASHKEY_NONE>, #0x004CAEA0<VSOUND_LOOKUP, HASHKEY_NONE>, #0x004CCC50<SOUNDKITLOOKUP, HASHKEY_STRI>, #0x004CCE40<ZONESOUNDOVERRIDELOOKUP, HASHKEY_STRI>, #0x004CE460<DSP_CHAIN_HEAD_LOOKUP, HASHKEY_STRI>, #0x004CF630<FOOTSTEPSNDCACHE, HASHKEY_NONE>, #0x004D60E0<CGObject_C, CHashKeyGUID>, 0x00704800<CACHEENTRY, HASHKEY_NONE>, #0x005061B0<CHATTHROTTLE, HASHKEY_STR>, #0x005064D0<WORDLIST, HASHKEY_LANGUAGE>, #0x005066D0<PROFANITYCHECK, HASHKEY_STR>, #0x005068D0<SPAMCHECK, HASHKEY_STR>, #0x005297F0<SAVEDVARIABLE, HASHKEY_STR>, #0x005298F0<CGGameUI::THREATMAP, CHashKeyGUID>, #0x00529B00, #0x0052F4A0<USEABLESTYLE, HASHKEY_NONE>, #0x0053F810<SPELLSBYNAME, HASHKEY_UTF8I>, #0x005490E0<WORLDSTATE, HASHKEY_NONE>, #0x0055A310<LFGListParty, CHashKeyGUID>, #0x0055A520<LFGListPlayer, CHashKeyGUID>, %0x0087F520<KEYCOMMAND, HASHKEY_STRI>, %0x0087F6C0<KEYBINDING, HASHKEY_STRI>, %0x0087F8E0<OVERRIDEKEYBINDING, HASHKEY_STRI>, 0x00924A30<MODIFIEDCLICK, HASHKEY_STRI>, %0x0087FD10<CLICKFRAME, HASHKEY_STR>, 0x009C67B0<MACRONODE, HASHKEY_NONE>, #0x005763D0<KNOWNDANCES, HASHKEY_STRI>, #0x00592EA0<TAXIPATHNODESCLIENT, HASHKEY_NONE>, #0x005B6590<ACHIEVEMENT, HASHKEY_NONE>, #0x005B6730<ACHIEVEMENTCRITERIALIST, HASHKEY_NONE>, #0x005C1730, %0x00909DF0<DYNAMICHOLIDAYHASH, HASHKEY_NONE>, 0x0099DC50/0x140720F70<TalentGroupItem_C, HASHKEY_NONE>, %0x008F3630<TalentGroupTierItem_C, HASHKEY_NONE>, #0x005D2860<FACTION, HASHKEY_NONE>, #0x005F08F0<MacroConditions, HASHKEY_STR>, 0x00447180<_BANNEDADDON, HASHKEY_NONE>, #0x005F60F0<UIADDON::METADATA, HASHKEY_STRI>, %0x00466F50<ADDONSTATE, HASHKEY_STRI>, %0x00467130<UIADDON, HASHKEY_STRI>, %0x00455BE0<MOUSELOOKBINDING, HASHKEY_STRI>, #0x006184F0<PLAYERPORTRAIT, CHashKeyGUID>, #0x00618720<UNITPORTRAIT, HASHKEY_DWORD>, #0x00618940<ITEMPORTRAIT, HASHKEY_STRI>, #0x00672D40, #0x00672EF0, #0x00672FF0, #0x006731A0, #0x006732A0, #0x00673450, #0x00673550, #0x00673700, #0x00673800, #0x006739B0, #0x00673AB0, #0x00673C70, #0x00673D70, #0x00673F10, #0x00674010, #0x006741B0, #0x006742B0, #0x00674460, #0x00674560, #0x00674710, #0x00674810, #0x006749B0, #0x00674AB0, #0x00674C50, #0x00674D50, #0x00674EF0, #0x00674FF0, #0x00675190, #0x00675290, #0x00675450, 0x00848790/0x1405761A0<CGxShader, HASHKEY_STRI>, 0x006117C0/0x14029C270<CLightList, HASHKEY_DWORD>, %0x00626310, 0x0061C4F0<CHARCODEDESC, HASHKEY_NONE>, 0x0061C960<KERNNODE, KERNINGHASHKEY>, %0x00633300<FACEDATA, HASHKEY_STRI>, #0x006E6760<TRADESKILLLINE, HASHKEY_NONE>, #0x006E6860<PROFICIENCYSPELL, HASHKEY_NONE>, #0x006F3C10<OBJ_EFFECT_GROUP_DEF_REF_LIST_LOOKUP, HASHKEY_NONE>, #0x006F3DA0<OBJ_EFFECT_REF_LOOKUP, HASHKEY_NONE>, #0x006F3F60<OBJ_EFFECT_GROUP_LOOKUP, HASHKEY_NONE>, #0x006F4160<OBJ_EFFECT_REF_LIST_LOOKUP, HASHKEY_NONE>, 0x005E35B0/0x1402620B0<OBJ_EFFECT_DEF_LOOKUP, HASHKEY_NONE>, 0x005E3950/0x140262460<OBJ_EFFECT_GROUP_DEF_LOOKUP, HASHKEY_NONE>, 0x005E3E30/0x140262A30<OBJ_EFFECT_PACKAGE_DEF_LOOKUP, HASHKEY_NONE>, #0x0070A000<INVENTORYART, HASHKEY_NONE>, #0x0070A1F0<ITEMBYNAME, HASHKEY_UTF8I>, #0x007349B0<UnitThreat, CHashKeyGUID>, 0x006567B0<CVar, HASHKEY_STRI>, 0x00654940<CONSOLECOMMAND, HASHKEY_CONSTSTRI>, 0x0056D7F0<RGN, HASHKEY_NONE>, #0x007B0900<CMapObj, HASHKEY_NONE>, #0x007B7290, #0x007C0F00<CMapDoodadDef, HASHKEY_DWORD>, #0x007C1000<CMapObjDef, HASHKEY_NONE>, #0x007DD8E0<TALKINGPLAYER, CHashKeyGUID>, #0x007DDAD0<MUTEDPLAYER, CHashKeyGUID>, #0x007E0580<DECLINEDWORD, HASHKEY_CONSTSTRI>, #0x007F2D60<DayNight::DNOverrideSky, HASHKEY_STRI>, #0x007F54A0<MINIMAPMD5NAME, HASHKEY_STRI>, #0x0080A4D0, #0x0080A6C0<DESTLOCSPELLCASTINDEXHASHNODE, CHashKeyGUID>, 0x008C0DC0<RACECLASSINFO, HASHKEY_NONE>, #0x00811340<SKILLCOSTSINFO, HASHKEY_NONE>, #0x00811540<SKILLLINEREC, HASHKEY_SKILLLINE>, 0x004ED530<HashedNode, HASHKEY_STRI>, 0x004ED740<FrameFactoryNode, HASHKEY_STRI>, 0x0083F0E0<FrameScript_EventObject, HASHKEY_CONSTSTRI>, 0x00B319D0<LCDHANDLE, HASHKEY_CONSTSTRI>, #0x0084B9B0<BITMAPBYICON, HASHKEY_NONE>, 0x00434B60<CShaderEffect, HASHKEY_STRI>, #0x008779B0<CacheData, HASHKEY_NONE>, #0x0087C140<SOUND_INTERNAL_LOOKUP, HASHKEY_NONE>, 0x0071D410<Liquid::Object, HASHKEY_NONE>, 0x0064AB00<DayNight::DNOverrideSky, HASHKEY_STRI>, 0x0072EA30<CMapDoodadDef, HASHKEY_DWORD>, 0x0072EB30<CMapObjDef, HASHKEY_NONE>, 0x0077C040<CSceneMaterial, HASHKEY_MATERIAL>, 0x00747FB0<CMapObj, HASHKEY_NONE>, 0x0073A8B0<DDMaterial, HASHKEY_NONE>, 0x00547A70<FileNotification::DeferredFile, HASHKEY_NONE>, 0x0070AB70<SE3SoundChunk, HASHKEY_NONE>, 0x0070CF80<SE3SOUNDKITOBJECT_LOOKUP, HASHKEY_NONE>, 0x0070C140<SOUNDKITLOOKUP, HASHKEY_CONSTSTRI>, 0x0070C340<SE3SoundEmitterLookup, HASHKEY_NONE>, 0x0070CE80<SE3COMBINABLESOUNDLIST, HASHKEY_NONE>, 0x00713D30<SE3ChatSound, HASHKEY_NONE>, 0x00714EB0<SE3MemorySoundObject, HASHKEY_NONE>, 0x004FCAD0/0x140140F00<FILENAMEHASH, HASHKEY_STR>, 0x008556D0<CGxDeviceD3d11::InputLayoutRec, InputLayoutKey>, 0x008558D0<CGxDeviceD3d11::RasterizerStateRec, HASHKEY_NONE>, 0x00855AD0<CGxDeviceD3d11::BlendStateRec, HASHKEY_NONE>, 0x00855CD0<CGxDeviceD3d11::DepthStateRec, HASHKEY_NONE>, 0x00855ED0<CGxDeviceD3d11::SamplerStateRec, HASHKEY_NONE>, 0x00547DB0<FileNotification::Manifest, HASHKEY_NONE>, 0x009F2DD0<CGSpellActivationOverlay::COUNTERNODE, HASHKEY_NONE>, 0x009F2FD0<CGSpellActivationOverlay::PETAURANODE, HASHKEY_NONE>, 0x009F6CC0<CGResearchFrame::BRANCH, HASHKEY_NONE>, 0x009F6E30<CGResearchFrame::BLOBPOIHASH, HASHKEY_NONE>, 0x009CA140<CGCUFProfiles::PROFILENODE, HASHKEY_CONSTUTF8I>, 0x009CA2E0<CGCUFProfiles::OPTIONENTRY, HASHKEY_CONSTSTR>]
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

// OFFSET: [%0x0066FE50<CVar, HASHKEY_STRI>]
template <class T, class TKey>
T* TSHashTable<T, TKey>::DeleteNode(T* ptr) {
    T* next = this->m_fulllist.Next(ptr);
    this->Unlink(ptr);
    this->InternalDelete(ptr);

    return next;
}

// PROVEN: ed33007e IDENTICAL/NONE
// OFFSET: [0x00653A00<CVar, HASHKEY_STRI>]
template <class T, class TKey>
T* TSHashTable<T, TKey>::Next(T* ptr) {
    return this->m_fulllist.Next(ptr);
}

// OFFSET: [0x005065A0/0x14014D730<FRAMEATTR, HASHKEY_STRI>, 0x00526570<SIMPLEFONT, HASHKEY_STRI>, 0x00544690<CTexture, HASHKEY_TEXTUREFILE>, 0x00548B80<CModelBlob::CHashEntry, HASHKEY_MODELBLOBFILE>, 0x00545C70<ProfileInternal::KEYVALUE, HASHKEY_CONSTSTRI>, 0x00545E40<ProfileInternal::SECTION, HASHKEY_CONSTSTRI>, 0x0054A020<FONTHASHOBJ, HASHKEY_STR>, 0x0054BEC0<CTextureBlobTexture, HASHKEY_TEXTUREBLOBFILE>, #0x004CAF90<VSOUND_LOOKUP, HASHKEY_NONE>, #0x004CCFC0<SOUNDKITLOOKUP, HASHKEY_STRI>, #0x004CD190<ZONESOUNDOVERRIDELOOKUP, HASHKEY_STRI>, #0x004CE550<DSP_CHAIN_HEAD_LOOKUP, HASHKEY_STRI>, #0x004CF720<FOOTSTEPSNDCACHE, HASHKEY_NONE>, 0x004903A0<CGObject_C, CHashKeyGUID>, 0x007048F0<CACHEENTRY, HASHKEY_NONE>, #0x00506AB0<WORDLIST, HASHKEY_LANGUAGE>, #0x00506D40<CHATTHROTTLE, HASHKEY_STR>, #0x00506F10<PROFANITYCHECK, HASHKEY_STR>, #0x005070E0<SPAMCHECK, HASHKEY_STR>, #0x00529D90<SAVEDVARIABLE, HASHKEY_STR>, #0x00529F60, #0x0052A240, #0x0052F590<USEABLESTYLE, HASHKEY_NONE>, #0x0053F900<SPELLSBYNAME, HASHKEY_UTF8I>, #0x005491D0<WORLDSTATE, HASHKEY_NONE>, #0x0055A6A0<LFGListParty, CHashKeyGUID>, #0x0055A870<LFGListPlayer, CHashKeyGUID>, #0x00561270<KEYCOMMAND, HASHKEY_STRI>, #0x00561440<KEYBINDING, HASHKEY_STRI>, #0x00561610<OVERRIDEKEYBINDING, HASHKEY_STRI>, #0x005617E0<MODIFIEDCLICK, HASHKEY_STRI>, #0x005619B0<CLICKFRAME, HASHKEY_STR>, 0x009C69B0<MACRONODE, HASHKEY_NONE>, #0x005764C0, #0x00593110<TAXIPATHNODESCLIENT, HASHKEY_NONE>, #0x005B68B0<ACHIEVEMENT, HASHKEY_NONE>, #0x005B6A80<ACHIEVEMENTCRITERIALIST, HASHKEY_NONE>, #0x005C1AA0, #0x005C1C70<DYNAMICHOLIDAYHASH, HASHKEY_NONE>, 0x0099DFE0<TalentGroupItem_C, HASHKEY_NONE>, 0x0099E1B0<TalentGroupTierItem_C, HASHKEY_NONE>, #0x005D2950<FACTION, HASHKEY_NONE>, #0x005F09E0<MacroConditions, HASHKEY_STR>, #0x005F4270<_BANNEDADDON, HASHKEY_NONE>, 0x0045E7D0/0x140071310<UIADDON::METADATA, HASHKEY_STRI>, 0x0045E9A0<ADDONSTATE, HASHKEY_STRI>, #0x005F6A50<UIADDON, HASHKEY_STRI>, #0x005FD000<MOUSELOOKBINDING, HASHKEY_STRI>, #0x00618AF0<PLAYERPORTRAIT, CHashKeyGUID>, #0x00618CC0<UNITPORTRAIT, HASHKEY_DWORD>, #0x00618EF0<ITEMPORTRAIT, HASHKEY_STRI>, #0x00675AB0, #0x006773F0, #0x006775C0, #0x00677790, #0x00677960, #0x00677B30, #0x00677D00, #0x00677ED0, #0x006780A0, #0x00678270, #0x00678440, #0x00678610, #0x006787E0, #0x006789B0, #0x00678B80, #0x00678D50, #0x006894C0<CGxShader, HASHKEY_STRI>, 0x006118B0<CLightList, HASHKEY_DWORD>, 0x0061CAB0<CHARCODEDESC, HASHKEY_NONE>, 0x0061CC80<KERNNODE, KERNINGHASHKEY>, #0x006C5B40, 0x00622C50<FACEDATA, HASHKEY_STRI>, #0x006E74A0, #0x006E7670<PROFICIENCYSPELL, HASHKEY_NONE>, #0x006F4B80<OBJ_EFFECT_GROUP_DEF_REF_LIST_LOOKUP, HASHKEY_NONE>, #0x006F4D50<OBJ_EFFECT_REF_LOOKUP, HASHKEY_NONE>, 0x005E4620<OBJ_EFFECT_GROUP_LOOKUP, HASHKEY_NONE>, 0x005E47F0<OBJ_EFFECT_REF_LIST_LOOKUP, HASHKEY_NONE>, #0x006F52C0<OBJ_EFFECT_DEF_LOOKUP, HASHKEY_NONE>, #0x006F5490<OBJ_EFFECT_GROUP_DEF_LOOKUP, HASHKEY_NONE>, #0x006F5660<OBJ_EFFECT_PACKAGE_DEF_LOOKUP, HASHKEY_NONE>, #0x0070A310<INVENTORYART, HASHKEY_NONE>, #0x0070A540<ITEMBYNAME, HASHKEY_UTF8I>, #0x007378D0<UnitThreat, CHashKeyGUID>, 0x006568A0<CVar, HASHKEY_STRI>, 0x00654A30<CONSOLECOMMAND, HASHKEY_CONSTSTRI>, 0x00A31E90<TIMESTAMPSTRUCT, HASHKEY_NONE>, #0x00777120<RGN, HASHKEY_NONE>, #0x007B09F0<CMapObj, HASHKEY_NONE>, #0x007BE580, #0x007BE760<CMapObjDef, HASHKEY_NONE>, #0x007DDC50<TALKINGPLAYER, CHashKeyGUID>, #0x007DDE20<MUTEDPLAYER, CHashKeyGUID>, #0x007E0670<DECLINEDWORD, HASHKEY_CONSTSTRI>, #0x007F2E50<DayNight::DNOverrideSky, HASHKEY_STRI>, #0x007F5590, #0x0080A840<ITEMCOOLDOWNHASHNODE, HASHKEY_NONE>, #0x0080AA10<DESTLOCSPELLCASTINDEXHASHNODE, CHashKeyGUID>, 0x008C0EB0<RACECLASSINFO, HASHKEY_NONE>, #0x008118F0<SKILLCOSTSINFO, HASHKEY_NONE>, #0x00811AC0<SKILLLINEREC, HASHKEY_SKILLLINE>, 0x004ED8C0<HashedNode, HASHKEY_STRI>, 0x004EDA90<FrameFactoryNode, HASHKEY_STRI>, 0x0083F1D0<FrameScript_EventObject, HASHKEY_CONSTSTRI>, #0x0084BB00<BITMAPBYICON, HASHKEY_NONE>, %0x00A65E70<LCDHANDLE, HASHKEY_CONSTSTRI>, 0x0087E5B0<OsNet::NETSELSOCKPTR, HASHKEY_NONE>, 0x00434C50<CShaderEffect, HASHKEY_STRI>, #0x0087C230<SOUND_INTERNAL_LOOKUP, HASHKEY_NONE>, 0x0071D500<Liquid::Object, HASHKEY_NONE>, 0x0064F220<DayNight::DNOverrideSky, HASHKEY_STRI>, 0x0072EFB0<CMapDoodadDef, HASHKEY_DWORD>, 0x0072F180<CMapObjDef, HASHKEY_NONE>, 0x0077C130<CSceneMaterial, HASHKEY_MATERIAL>, 0x007480A0<CMapObj, HASHKEY_NONE>, 0x0073AA50<DDMaterial, HASHKEY_NONE>, 0x00621DF0<BATCHEDRENDERFONTDESC, HASHKEY_PTR>, 0x004AA350<DBCache<CreatureStats_C, int, HASHKEY_INT>::DBCACHEHASH, HASHKEY_INT>, 0x004AA6F0<DBCache<NPCText, int, HASHKEY_INT>::DBCACHEHASH, HASHKEY_INT>, 0x004AA8C0<DBCache<NameCache, unsigned __int64, CHashKeyGUID>::DBCACHEHASH, CHashKeyGUID>, 0x004AAA90<DBCache<GuildStats_C, int, HASHKEY_INT>::DBCACHEHASH, HASHKEY_INT>, 0x004AAC60<DBCache<QuestCache, int, HASHKEY_INT>::DBCACHEHASH, HASHKEY_INT>, 0x004AAE30<DBCache<PageTextCache_C, int, HASHKEY_INT>::DBCACHEHASH, HASHKEY_INT>, 0x004AB000<DBCache<PetNameCache, int, HASHKEY_INT>::DBCACHEHASH, HASHKEY_INT>, 0x004AB1D0<DBCache<CGPetition, int, HASHKEY_INT>::DBCACHEHASH, HASHKEY_INT>, 0x004AB3A0<DBCache<ItemTextCache_C, unsigned __int64, CHashKeyGUID>::DBCACHEHASH, CHashKeyGUID>, 0x004AB740<DBCache<ArenaTeamCache, int, HASHKEY_INT>::DBCACHEHASH, HASHKEY_INT>, 0x004AB910<DBCache<DanceCache, int, HASHKEY_INT>::DBCACHEHASH, HASHKEY_INT>, 0x0070C870<SE3SoundChunk, HASHKEY_NONE>, 0x0070D480<SE3SOUNDKITOBJECT_LOOKUP, HASHKEY_NONE>, 0x0070CC10<SOUNDKITLOOKUP, HASHKEY_CONSTSTRI>, 0x0070CA40<SE3SoundEmitterLookup, HASHKEY_NONE>, 0x0070D2B0<SE3COMBINABLESOUNDLIST, HASHKEY_NONE>, 0x00713E20<SE3ChatSound, HASHKEY_NONE>, 0x00714FA0<SE3MemorySoundObject, HASHKEY_NONE>, 0x004FCC90<FILENAMEHASH, HASHKEY_STR>, 0x0086E6C0/0x1405A34D0<CGxDeviceD3d11::InputLayoutRec, InputLayoutKey>, 0x0086E890<CGxDeviceD3d11::RasterizerStateRec, HASHKEY_NONE>, 0x0086EA60<CGxDeviceD3d11::BlendStateRec, HASHKEY_NONE>, 0x0086EC30<CGxDeviceD3d11::DepthStateRec, HASHKEY_NONE>, 0x0086EE00<CGxDeviceD3d11::SamplerStateRec, HASHKEY_NONE>, 0x00547ED0<FileNotification::DeferredFile, HASHKEY_NONE>, 0x009F3150<CGSpellActivationOverlay::COUNTERNODE, HASHKEY_NONE>, 0x009F3320<CGSpellActivationOverlay::PETAURANODE, HASHKEY_NONE>, 0x009F6FB0<CGResearchFrame::BRANCH, HASHKEY_NONE>, 0x009F7180<CGResearchFrame::BLOBPOIHASH, HASHKEY_NONE>, 0x009CA460<CGCUFProfiles::PROFILENODE, HASHKEY_CONSTUTF8I>, 0x009CA630<CGCUFProfiles::OPTIONENTRY, HASHKEY_CONSTSTR>]
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

// OFFSET: [#0x004C9FD0<VSOUND_LOOKUP, HASHKEY_NONE>]
template <class T, class TKey>
T* TSHashTable<T, TKey>::Head() {
    return this->m_fulllist.Head();
}

// OFFSET: [0x00506540/0x14014D6B0<FRAMEATTR, HASHKEY_STRI>, 0x00526510/0x140175500<SIMPLEFONT, HASHKEY_STRI>, 0x00544630<CTexture, HASHKEY_TEXTUREFILE>, 0x00548B20<CModelBlob::CHashEntry, HASHKEY_MODELBLOBFILE>, 0x00545BB0<ProfileInternal::KEYVALUE, HASHKEY_CONSTSTRI>, 0x00545C10<ProfileInternal::SECTION, HASHKEY_CONSTSTRI>, 0x00549FC0<FONTHASHOBJ, HASHKEY_STR>, 0x0054BE60<CTextureBlobTexture, HASHKEY_TEXTUREBLOBFILE>, #0x004CAF30<VSOUND_LOOKUP, HASHKEY_NONE>, #0x004CCF00<SOUNDKITLOOKUP, HASHKEY_STRI>, #0x004CCF60<ZONESOUNDOVERRIDELOOKUP, HASHKEY_STRI>, #0x004CE4F0<DSP_CHAIN_HEAD_LOOKUP, HASHKEY_STRI>, #0x004CF6C0<FOOTSTEPSNDCACHE, HASHKEY_NONE>, #0x004D67A0<CGObject_C, CHashKeyGUID>, 0x00704890<CACHEENTRY, HASHKEY_NONE>, #0x005069F0<WORDLIST, HASHKEY_LANGUAGE>, #0x00506A50<CHATTHROTTLE, HASHKEY_STR>, #0x00506C80<PROFANITYCHECK, HASHKEY_STR>, #0x00506CE0<SPAMCHECK, HASHKEY_STR>, #0x00529CD0<SAVEDVARIABLE, HASHKEY_STR>, #0x00529D30, #0x0052A1E0, #0x0052F530<USEABLESTYLE, HASHKEY_NONE>, #0x0053F8A0<SPELLSBYNAME, HASHKEY_UTF8I>, #0x00549170<WORLDSTATE, HASHKEY_NONE>, #0x0055A5E0<LFGListParty, CHashKeyGUID>, #0x0055A640<LFGListPlayer, CHashKeyGUID>, #0x00561090<KEYCOMMAND, HASHKEY_STRI>, 0x00924DF0/0x140689680<KEYBINDING, HASHKEY_STRI>, 0x00924E50/0x140689700<OVERRIDEKEYBINDING, HASHKEY_STRI>, 0x00924EB0/0x140689780<MODIFIEDCLICK, HASHKEY_STRI>, #0x00561210<CLICKFRAME, HASHKEY_STR>, %0x00919BC0<MACRONODE, HASHKEY_NONE>, #0x00576460, #0x005930B0<TAXIPATHNODESCLIENT, HASHKEY_NONE>, #0x005B67F0<ACHIEVEMENT, HASHKEY_NONE>, #0x005B6850<ACHIEVEMENTCRITERIALIST, HASHKEY_NONE>, #0x005C19E0, #0x005C1A40<DYNAMICHOLIDAYHASH, HASHKEY_NONE>, 0x0099DF20<TalentGroupItem_C, HASHKEY_NONE>, 0x0099DF80<TalentGroupTierItem_C, HASHKEY_NONE>, #0x005D28F0<FACTION, HASHKEY_NONE>, #0x005F0980<MacroConditions, HASHKEY_STR>, 0x00447210<_BANNEDADDON, HASHKEY_NONE>, 0x0045E6B0/0x140071210<UIADDON::METADATA, HASHKEY_STRI>, %0x00467280<ADDONSTATE, HASHKEY_STRI>, #0x005F6650<UIADDON, HASHKEY_STRI>, 0x0044B4E0/0x1400588E0<MOUSELOOKBINDING, HASHKEY_STRI>, #0x00618A30<PLAYERPORTRAIT, CHashKeyGUID>, #0x00618A90<UNITPORTRAIT, HASHKEY_DWORD>, #0x00618E90<ITEMPORTRAIT, HASHKEY_STRI>, #0x00675A50, #0x00676E50, #0x00676EB0, #0x00676F10, #0x00676F70, #0x00676FD0, #0x00677030, #0x00677090, #0x006770F0, #0x00677150, #0x006771B0, #0x00677210, #0x00677270, #0x006772D0, #0x00677330, #0x00677390, #0x00689460<CGxShader, HASHKEY_STRI>, 0x00611850<CLightList, HASHKEY_DWORD>, 0x0061C9F0<CHARCODEDESC, HASHKEY_NONE>, 0x0061CA50<KERNNODE, KERNINGHASHKEY>, #0x006C5AE0, 0x00622BF0<FACEDATA, HASHKEY_STRI>, #0x006E73E0, #0x006E7440<PROFICIENCYSPELL, HASHKEY_NONE>, #0x006F48E0<OBJ_EFFECT_GROUP_DEF_REF_LIST_LOOKUP, HASHKEY_NONE>, #0x006F4940<OBJ_EFFECT_REF_LOOKUP, HASHKEY_NONE>, 0x005E40A0<OBJ_EFFECT_GROUP_LOOKUP, HASHKEY_NONE>, 0x005E4100<OBJ_EFFECT_REF_LIST_LOOKUP, HASHKEY_NONE>, #0x006F4A60<OBJ_EFFECT_DEF_LOOKUP, HASHKEY_NONE>, #0x006F4AC0<OBJ_EFFECT_GROUP_DEF_LOOKUP, HASHKEY_NONE>, #0x006F4B20<OBJ_EFFECT_PACKAGE_DEF_LOOKUP, HASHKEY_NONE>, #0x0070A2B0<INVENTORYART, HASHKEY_NONE>, #0x0070A4E0<ITEMBYNAME, HASHKEY_UTF8I>, #0x00737870<UnitThreat, CHashKeyGUID>, 0x00656840<CVar, HASHKEY_STRI>, 0x006549D0/0x1402F5460<CONSOLECOMMAND, HASHKEY_CONSTSTRI>, 0x00A31E30<TIMESTAMPSTRUCT, HASHKEY_NONE>, 0x004C2F30<RGN, HASHKEY_NONE>, #0x007B0990<CMapObj, HASHKEY_NONE>, #0x007BE4C0, #0x007BE520<CMapObjDef, HASHKEY_NONE>, #0x007DDB90<TALKINGPLAYER, CHashKeyGUID>, #0x007DDBF0<MUTEDPLAYER, CHashKeyGUID>, #0x007E0610<DECLINEDWORD, HASHKEY_CONSTSTRI>, #0x007F2DF0<DayNight::DNOverrideSky, HASHKEY_STRI>, #0x007F5530, #0x0080A780<ITEMCOOLDOWNHASHNODE, HASHKEY_NONE>, #0x0080A7E0<DESTLOCSPELLCASTINDEXHASHNODE, CHashKeyGUID>, 0x008C0E50<RACECLASSINFO, HASHKEY_NONE>, #0x00811660<SKILLCOSTSINFO, HASHKEY_NONE>, #0x008116C0<SKILLLINEREC, HASHKEY_SKILLLINE>, 0x004ED800<HashedNode, HASHKEY_STRI>, 0x004ED860<FrameFactoryNode, HASHKEY_STRI>, 0x0083F170<FrameScript_EventObject, HASHKEY_CONSTSTRI>, 0x00B32E60<BITMAPBYICON, HASHKEY_NONE>, %0x00A65C40<LCDHANDLE, HASHKEY_CONSTSTRI>, 0x0087E550<OsNet::NETSELSOCKPTR, HASHKEY_NONE>, %0x0043F010<CShaderEffect, HASHKEY_STRI>, #0x0087C1D0<SOUND_INTERNAL_LOOKUP, HASHKEY_NONE>, 0x0071D4A0<Liquid::Object, HASHKEY_NONE>, 0x0064F1C0<DayNight::DNOverrideSky, HASHKEY_STRI>, 0x0072EEF0<CMapDoodadDef, HASHKEY_DWORD>, 0x0072EF50<CMapObjDef, HASHKEY_NONE>, 0x0077C0D0<CSceneMaterial, HASHKEY_MATERIAL>, 0x00748040<CMapObj, HASHKEY_NONE>, 0x0073A9F0<DDMaterial, HASHKEY_NONE>, 0x004A9FF0<DBCache<GuildStats_C, int, HASHKEY_INT>::DBCACHEHASH, HASHKEY_INT>, 0x004FCC30<FILENAMEHASH, HASHKEY_STR>, 0x00713DC0/0x1403E60F0<SE3ChatSound, HASHKEY_NONE>, 0x00714F40/0x1403E7970<SE3MemorySoundObject, HASHKEY_NONE>, 0x00621D90<BATCHEDRENDERFONTDESC, HASHKEY_PTR>, 0x004A9F30<DBCache<NPCText, int, HASHKEY_INT>::DBCACHEHASH, HASHKEY_INT>, 0x004A9F90<DBCache<NameCache, unsigned __int64, CHashKeyGUID>::DBCACHEHASH, CHashKeyGUID>, 0x004AA050<DBCache<QuestCache, int, HASHKEY_INT>::DBCACHEHASH, HASHKEY_INT>, 0x004AA0B0<DBCache<PageTextCache_C, int, HASHKEY_INT>::DBCACHEHASH, HASHKEY_INT>, 0x004AA110<DBCache<PetNameCache, int, HASHKEY_INT>::DBCACHEHASH, HASHKEY_INT>, 0x004AA170<DBCache<CGPetition, int, HASHKEY_INT>::DBCACHEHASH, HASHKEY_INT>, 0x004AA1D0<DBCache<ItemTextCache_C, unsigned __int64, CHashKeyGUID>::DBCACHEHASH, CHashKeyGUID>, 0x004AA290<DBCache<ArenaTeamCache, int, HASHKEY_INT>::DBCACHEHASH, HASHKEY_INT>, 0x004AA2F0<DBCache<DanceCache, int, HASHKEY_INT>::DBCACHEHASH, HASHKEY_INT>, 0x0070C530<SE3SoundChunk, HASHKEY_NONE>, 0x0070D130<SE3SOUNDKITOBJECT_LOOKUP, HASHKEY_NONE>, 0x0070C810<SOUNDKITLOOKUP, HASHKEY_CONSTSTRI>, 0x0070C590<SE3SoundEmitterLookup, HASHKEY_NONE>, 0x0070D0D0<SE3COMBINABLESOUNDLIST, HASHKEY_NONE>, 0x0086E4E0/0x1405A3250<CGxDeviceD3d11::InputLayoutRec, InputLayoutKey>, 0x0086E540<CGxDeviceD3d11::RasterizerStateRec, HASHKEY_NONE>, 0x0086E5A0<CGxDeviceD3d11::BlendStateRec, HASHKEY_NONE>, 0x0086E600<CGxDeviceD3d11::DepthStateRec, HASHKEY_NONE>, 0x0086E660<CGxDeviceD3d11::SamplerStateRec, HASHKEY_NONE>, 0x00547E70<FileNotification::DeferredFile, HASHKEY_NONE>, 0x009F3090<CGSpellActivationOverlay::COUNTERNODE, HASHKEY_NONE>, 0x009F30F0<CGSpellActivationOverlay::PETAURANODE, HASHKEY_NONE>, 0x009F6EF0<CGResearchFrame::BRANCH, HASHKEY_NONE>, 0x009F6F50<CGResearchFrame::BLOBPOIHASH, HASHKEY_NONE>, 0x009CA3A0<CGCUFProfiles::PROFILENODE, HASHKEY_CONSTUTF8I>, 0x009CA400<CGCUFProfiles::OPTIONENTRY, HASHKEY_CONSTSTR>, 0x0043A100<CCameraModeEntry, HASHKEY_NONE>, 0x00484A90<UNITPORTRAIT, HASHKEY_NONE>, 0x00484E90<ITEMPORTRAIT, HASHKEY_STR>, 0x0048C780<WowClientDB2_Base::DBTABLE, HASHKEY_NONE>, 0x004963A0<SPELLACTIVATIONOVERLAY, HASHKEY_NONE>, 0x004DE970<TALKINGPLAYER, CHashKeyGUID>, 0x004DE9D0<MUTEDPLAYER, CHashKeyGUID>, 0x004E5B40<DECLINEDWORD, HASHKEY_CONSTSTRI>, 0x00590D80<CustomFallbackPath, HASHKEY_NONE>, 0x005A83C0<INVENTORYART, HASHKEY_NONE>, 0x005A85F0<ITEMBYNAME, HASHKEY_UTF8I>, 0x005E4160<OBJ_EFFECT_DEF_LOOKUP, HASHKEY_NONE>, 0x005E41C0<OBJ_EFFECT_GROUP_DEF_LOOKUP, HASHKEY_NONE>, 0x005E4220<OBJ_EFFECT_PACKAGE_DEF_LOOKUP, HASHKEY_NONE>, 0x0062F0B0<ZONESOUNDOVERRIDELOOKUP, HASHKEY_STRI>, 0x006333F0<DSP_CHAIN_HEAD_LOOKUP, HASHKEY_STRI>, 0x00634140<FOOTSTEPSNDCACHE, HASHKEY_NONE>, 0x008B2C30<ITEMCOOLDOWNHASHNODE, HASHKEY_NONE>, 0x008B2C90<DESTLOCSPELLCASTINDEXHASHNODE, CHashKeyGUID>, 0x00917580<OVERRIDE_SPELL_NODE, HASHKEY_NONE>, 0x009175E0<SPELLSBYNAME, HASHKEY_UTF8I>, 0x0093B280<CGGuildInfo::GuildRecipesKnown, HASHKEY_NONE>, 0x0093B340<CGGuildInfo::SKILLLINESPELLS, HASHKEY_NONE>, 0x00966F90<CGQuestLog::QUESTCOMPLETIONNPC, HASHKEY_NONE>, 0x009A50F0<CFlyout, HASHKEY_NONE>, 0x009A5150<SpellToFlyoutMapItem, HASHKEY_NONE>, 0x009A8910<FACTION, HASHKEY_NONE>, 0x009FD740<CGIncomingResurrection::INCOMINGRESLIST, CHashKeyGUID>, 0x0045E770<UIADDON, HASHKEY_STRI>, 0x008DCA60<TAINTEVENT, HASHKEY_STR>, 0x008F2DB0<WORDLIST, HASHKEY_LANGUAGE>, 0x00924F10<CLICKFRAME, HASHKEY_STR>, 0x0094E8A0<ACHIEVEMENT, HASHKEY_NONE>, 0x009B5230<DYNAMICHOLIDAYHASH, HASHKEY_NONE>, 0x009B8980<KNOWNDANCES, HASHKEY_STRI>, 0x009CE0F0<WORLDSTATE, HASHKEY_NONE>, 0x008F2E70<ADDONPREFIX, HASHKEY_STR>]
template <class T, class TKey>
void TSHashTable<T, TKey>::Initialize() {
    this->m_slotmask = 3;
    this->m_slotlistarray.SetCount(4);

    for (uint32_t i = 0; i <= this->m_slotmask; i++) {
        this->m_slotlistarray[i].ChangeLinkOffset(offsetof(T, m_linktoslot));
    }
}

// OFFSET: [#0x004B93B0<CTexture, HASHKEY_TEXTUREFILE>, %0x00498490<CGObject_C, CHashKeyGUID>, %0x0066C530<CACHEENTRY, HASHKEY_NONE>, %0x005E3F00<UnitThreat, CHashKeyGUID>, #0x007B0C60<CMapObj, HASHKEY_NONE>, %0x0069B030, #0x007BEAE0<CMapObjDef, HASHKEY_NONE>, 0x009F7860<CGResearchFrame::BRANCH, HASHKEY_NONE>]
template <class T, class TKey>
void TSHashTable<T, TKey>::Insert(T* ptr, uint32_t hashval, const TKey& key) {
    this->InternalLinkNode(ptr, hashval);
    ptr->m_key = key;
}

// OFFSET: #0x006C8A20
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

// OFFSET: [%0x005EDDE0<CALENDAREVENTHASH, HASHKEY_NONE>, 0x007361E0/0x140412850<CMapDynamicTexture, HASHKEY_DWORD>, 0x004EC970<CGSpellActivationOverlay::COUNTERNODE, HASHKEY_NONE>, 0x004EC970<CGSpellActivationOverlay::PETAURANODE, HASHKEY_NONE>, 0x004EC970<CGResearchFrame::BRANCH, HASHKEY_NONE>, 0x004EC970<CGResearchFrame::BLOBPOIHASH, HASHKEY_NONE>, 0x004EC970<CGCUFProfiles::PROFILENODE, HASHKEY_CONSTUTF8I>]
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

// INLINED
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

// OFFSET: [%0x00516A20<FRAMEATTR, HASHKEY_STRI>, %0x00558A90<ProfileInternal::KEYVALUE, HASHKEY_CONSTSTRI>, %0x00558AF0<ProfileInternal::SECTION, HASHKEY_CONSTSTRI>, #0x004CB520<VSOUND_LOOKUP, HASHKEY_NONE>, #0x005082C0<PROFANITYCHECK, HASHKEY_STR>, %0x0083EEE0<SAVEDVARIABLE, HASHKEY_STR>, #0x005626A0<KEYCOMMAND, HASHKEY_STRI>, %0x008815A0<KEYBINDING, HASHKEY_STRI>, %0x00881600<OVERRIDEKEYBINDING, HASHKEY_STRI>, %0x00881660<MODIFIEDCLICK, HASHKEY_STRI>, %0x008A8C70<ACHIEVEMENTCRITERIALIST, HASHKEY_NONE>, %0x008F3DA0<TalentGroupItem_C, HASHKEY_NONE>, %0x008F3F30<TalentGroupTierItem_C, HASHKEY_NONE>, %0x004680C0<UIADDON::METADATA, HASHKEY_STRI>, %0x007AF300<MOUSELOOKBINDING, HASHKEY_STRI>, #0x006192A0<PLAYERPORTRAIT, CHashKeyGUID>, %0x004B2760<DBCache<NameCache, unsigned __int64, CHashKeyGUID>::REVERSEENTRY, HASHKEY_CONSTSTR>, %0x004B3670<DBCache<CreatureStats_C, int, HASHKEY_INT>::DBCACHEHASH, HASHKEY_INT>, #0x0067AFB0<DBCache<GameObjectStats_C, int, HASHKEY_INT>::DBCACHEHASH, HASHKEY_INT>, %0x004B36D0, #0x0067B070<DBCache<ItemStats_C, int, HASHKEY_INT>::DBCACHEHASH, HASHKEY_INT>, %0x004B3730<DBCache<NPCText, int, HASHKEY_INT>::DBCACHEHASH, HASHKEY_INT>, %0x004B3790<DBCache<NameCache, unsigned __int64, CHashKeyGUID>::DBCACHEHASH, CHashKeyGUID>, 0x004AC710<DBCache<GuildStats_C, int, HASHKEY_INT>::DBCACHEHASH, HASHKEY_INT>, %0x004B3850<DBCache<QuestCache, int, HASHKEY_INT>::DBCACHEHASH, HASHKEY_INT>, %0x004B38B0<DBCache<PageTextCache_C, int, HASHKEY_INT>::DBCACHEHASH, HASHKEY_INT>, %0x004B3910<DBCache<PetNameCache, int, HASHKEY_INT>::DBCACHEHASH, HASHKEY_INT>, %0x004B3970<DBCache<CGPetition, int, HASHKEY_INT>::DBCACHEHASH, HASHKEY_INT>, %0x004B39D0<DBCache<ItemTextCache_C, unsigned __int64, CHashKeyGUID>::DBCACHEHASH, CHashKeyGUID>, %0x004B3A30, %0x004B3A90<DBCache<ArenaTeamCache, int, HASHKEY_INT>::DBCACHEHASH, HASHKEY_INT>, %0x004B3AF0<DBCache<DanceCache, int, HASHKEY_INT>::DBCACHEHASH, HASHKEY_INT>, #0x00689760<CGxShader, HASHKEY_STRI>, %0x0062DBF0<KERNNODE, KERNINGHASHKEY>, %0x00632800<BATCHEDRENDERFONTDESC, HASHKEY_PTR>, %0x0057ED90<TRADESKILLLINE, HASHKEY_NONE>, %0x0057EDF0<PROFICIENCYSPELL, HASHKEY_NONE>, %0x005F1D30<OBJ_EFFECT_REF_LIST_LOOKUP, HASHKEY_NONE>, %0x005F2030<OBJ_EFFECT_GROUP_LOOKUP, HASHKEY_NONE>, %0x009723F0<TIMESTAMPSTRUCT, HASHKEY_NONE>, %0x007E3DC0<OsNet::NETSELSOCKPTR, HASHKEY_NONE>, 0x00656B00<CVar, HASHKEY_STRI>, 0x0086F2A0/0x1405A4970<CGxDeviceD3d11::InputLayoutRec, InputLayoutKey>, 0x0086F300<CGxDeviceD3d11::RasterizerStateRec, HASHKEY_NONE>, 0x0086F360<CGxDeviceD3d11::BlendStateRec, HASHKEY_NONE>, 0x0086F3C0<CGxDeviceD3d11::DepthStateRec, HASHKEY_NONE>, 0x0086F420<CGxDeviceD3d11::SamplerStateRec, HASHKEY_NONE>, 0x00596310<RGN, HASHKEY_NONE>, 0x009CA920<CGCUFProfiles::OPTIONENTRY, HASHKEY_CONSTSTR>]
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

// OFFSET: [0x005067A0/0x14014DB10<FRAMEATTR, HASHKEY_STRI>, 0x00526760<SIMPLEFONT, HASHKEY_STRI>, 0x00544A10<CTexture, HASHKEY_TEXTUREFILE>, 0x00548D50<CModelBlob::CHashEntry, HASHKEY_MODELBLOBFILE>, 0x00546130<ProfileInternal::KEYVALUE, HASHKEY_CONSTSTRI>, 0x005461C0<ProfileInternal::SECTION, HASHKEY_CONSTSTRI>, 0x0054A1F0<FONTHASHOBJ, HASHKEY_STR>, 0x0054C260<CTextureBlobTexture, HASHKEY_TEXTUREBLOBFILE>, #0x004CB310<VSOUND_LOOKUP, HASHKEY_NONE>, #0x004CD360<SOUNDKITLOOKUP, HASHKEY_STRI>, #0x004CD400<ZONESOUNDOVERRIDELOOKUP, HASHKEY_STRI>, #0x004CE720<DSP_CHAIN_HEAD_LOOKUP, HASHKEY_STRI>, #0x004CF8F0<FOOTSTEPSNDCACHE, HASHKEY_NONE>, 0x00490570<CGObject_C, CHashKeyGUID>, 0x00704AC0<CACHEENTRY, HASHKEY_NONE>, #0x00507E10<WORDLIST, HASHKEY_LANGUAGE>, #0x00507EB0<CHATTHROTTLE, HASHKEY_STR>, #0x00507F50<PROFANITYCHECK, HASHKEY_STR>, #0x00507FF0<SPAMCHECK, HASHKEY_STR>, #0x0052A410<SAVEDVARIABLE, HASHKEY_STR>, #0x0052A4B0, #0x0052A550, #0x0052F910<USEABLESTYLE, HASHKEY_NONE>, #0x005405B0<SPELLSBYNAME, HASHKEY_UTF8I>, #0x005493A0<WORLDSTATE, HASHKEY_NONE>, #0x0055AC40<LFGListParty, CHashKeyGUID>, #0x0055ACE0<LFGListPlayer, CHashKeyGUID>, #0x00561DD0<KEYCOMMAND, HASHKEY_STRI>, 0x00925B60/0x14068AD60<KEYBINDING, HASHKEY_STRI>, 0x00925BF0/0x14068ADF0<OVERRIDEKEYBINDING, HASHKEY_STRI>, #0x00561FB0<MODIFIEDCLICK, HASHKEY_STRI>, #0x00562050<CLICKFRAME, HASHKEY_STR>, #0x00567B70<MACRONODE, HASHKEY_NONE>, #0x00576690, #0x00593830<TAXIPATHNODESCLIENT, HASHKEY_NONE>, #0x005B6C50<ACHIEVEMENT, HASHKEY_NONE>, #0x005B6CF0<ACHIEVEMENTCRITERIALIST, HASHKEY_NONE>, #0x005C1EF0, #0x005C1F90<DYNAMICHOLIDAYHASH, HASHKEY_NONE>, 0x0099E3F0<TalentGroupItem_C, HASHKEY_NONE>, 0x0099E480<TalentGroupTierItem_C, HASHKEY_NONE>, #0x005D2B20<FACTION, HASHKEY_NONE>, #0x005F0D50<MacroConditions, HASHKEY_STR>, #0x005F4440<_BANNEDADDON, HASHKEY_NONE>, 0x0045F1F0/0x1400724B0<UIADDON::METADATA, HASHKEY_STRI>, #0x005F7100<ADDONSTATE, HASHKEY_STRI>, #0x005F71A0<UIADDON, HASHKEY_STRI>, #0x005FD220<MOUSELOOKBINDING, HASHKEY_STRI>, #0x006190C0<PLAYERPORTRAIT, CHashKeyGUID>, #0x00619160<UNITPORTRAIT, HASHKEY_DWORD>, #0x00619200<ITEMPORTRAIT, HASHKEY_STRI>, #0x00679240, #0x006793D0, #0x00679470, #0x00679510, #0x006795B0, #0x00679650, #0x006796F0, #0x00679790, #0x00679830, #0x006798D0, #0x00679970, #0x00679A10, #0x00679AB0, #0x00679B50, #0x00679BF0, #0x00679C90, #0x006896C0<CGxShader, HASHKEY_STRI>, 0x00611C50<CLightList, HASHKEY_DWORD>, 0x0061CF30<CHARCODEDESC, HASHKEY_NONE>, 0x0061CFC0<KERNNODE, KERNINGHASHKEY>, #0x006C5D10, 0x00622E20<FACEDATA, HASHKEY_STRI>, #0x006E7900, #0x006E79A0<PROFICIENCYSPELL, HASHKEY_NONE>, #0x006F5A00<OBJ_EFFECT_GROUP_DEF_REF_LIST_LOOKUP, HASHKEY_NONE>, #0x006F5AA0<OBJ_EFFECT_REF_LOOKUP, HASHKEY_NONE>, 0x005E52B0<OBJ_EFFECT_REF_LIST_LOOKUP, HASHKEY_NONE>, #0x006F5BE0<OBJ_EFFECT_DEF_LOOKUP, HASHKEY_NONE>, #0x006F5C80<OBJ_EFFECT_GROUP_DEF_LOOKUP, HASHKEY_NONE>, 0x005E5220<OBJ_EFFECT_GROUP_LOOKUP, HASHKEY_NONE>, #0x006F5E20<OBJ_EFFECT_PACKAGE_DEF_LOOKUP, HASHKEY_NONE>, #0x0070A710<INVENTORYART, HASHKEY_NONE>, #0x0070A7B0<ITEMBYNAME, HASHKEY_UTF8I>, #0x0073A7F0<UnitThreat, CHashKeyGUID>, 0x00656A70<CVar, HASHKEY_STRI>, 0x00654C00<CONSOLECOMMAND, HASHKEY_CONSTSTRI>, 0x00A320B0<TIMESTAMPSTRUCT, HASHKEY_NONE>, #0x00777380<RGN, HASHKEY_NONE>, #0x007B0BC0<CMapObj, HASHKEY_NONE>, #0x007BE930, #0x007BE9D0<CMapObjDef, HASHKEY_NONE>, #0x007DF2D0<TALKINGPLAYER, CHashKeyGUID>, #0x007DF370<MUTEDPLAYER, CHashKeyGUID>, #0x007E0840<DECLINEDWORD, HASHKEY_CONSTSTRI>, #0x007F3020<DayNight::DNOverrideSky, HASHKEY_STRI>, #0x007F5B00, #0x0080C650<ITEMCOOLDOWNHASHNODE, HASHKEY_NONE>, #0x0080C6F0<DESTLOCSPELLCASTINDEXHASHNODE, CHashKeyGUID>, 0x008C1080<RACECLASSINFO, HASHKEY_NONE>, #0x00811DE0<SKILLCOSTSINFO, HASHKEY_NONE>, #0x00811E80<SKILLLINEREC, HASHKEY_SKILLLINE>, 0x004EDC60<HashedNode, HASHKEY_STRI>, 0x004EDCF0<FrameFactoryNode, HASHKEY_STRI>, 0x0083F3C0<FrameScript_EventObject, HASHKEY_CONSTSTRI>, #0x0084BEA0<BITMAPBYICON, HASHKEY_NONE>, #0x0084BF40<LCDHANDLE, HASHKEY_CONSTSTRI>, 0x0087E780<OsNet::NETSELSOCKPTR, HASHKEY_NONE>, #0x00876B40<CShaderEffect, HASHKEY_STRI>, #0x0087C670<SOUND_INTERNAL_LOOKUP, HASHKEY_NONE>, 0x0071D6D0<Liquid::Object, HASHKEY_NONE>, 0x0064F3F0<DayNight::DNOverrideSky, HASHKEY_STRI>, 0x0072F440<CMapDoodadDef, HASHKEY_DWORD>, 0x0072F4D0<CMapObjDef, HASHKEY_NONE>, 0x0077C300<CSceneMaterial, HASHKEY_MATERIAL>, 0x00748270<CMapObj, HASHKEY_NONE>, 0x0073AC50<DDMaterial, HASHKEY_NONE>, 0x00621FC0<BATCHEDRENDERFONTDESC, HASHKEY_PTR>, 0x004ABD50<DBCache<NameCache, unsigned __int64, CHashKeyGUID>::REVERSEENTRY, HASHKEY_NONE>, 0x004ABDE0<DBCache<CreatureStats_C, int, HASHKEY_INT>::DBCACHEHASH, HASHKEY_INT>, 0x004ABF00<DBCache<NPCText, int, HASHKEY_INT>::DBCACHEHASH, HASHKEY_INT>, 0x004ABF90<DBCache<NameCache, unsigned __int64, CHashKeyGUID>::DBCACHEHASH, CHashKeyGUID>, 0x004AC020<DBCache<GuildStats_C, int, HASHKEY_INT>::DBCACHEHASH, HASHKEY_INT>, 0x004AC0B0<DBCache<QuestCache, int, HASHKEY_INT>::DBCACHEHASH, HASHKEY_INT>, 0x004AC140<DBCache<PageTextCache_C, int, HASHKEY_INT>::DBCACHEHASH, HASHKEY_INT>, 0x004AC1D0<DBCache<PetNameCache, int, HASHKEY_INT>::DBCACHEHASH, HASHKEY_INT>, 0x004AC260<DBCache<CGPetition, int, HASHKEY_INT>::DBCACHEHASH, HASHKEY_INT>, 0x004AC2F0<DBCache<ItemTextCache_C, unsigned __int64, CHashKeyGUID>::DBCACHEHASH, CHashKeyGUID>, 0x004AC410<DBCache<ArenaTeamCache, int, HASHKEY_INT>::DBCACHEHASH, HASHKEY_INT>, 0x004AC4A0<DBCache<DanceCache, int, HASHKEY_INT>::DBCACHEHASH, HASHKEY_INT>, 0x0070D190<SE3SoundChunk, HASHKEY_NONE>, 0x0070D8B0<SE3SOUNDKITOBJECT_LOOKUP, HASHKEY_NONE>, 0x0070D650<SOUNDKITLOOKUP, HASHKEY_CONSTSTRI>, 0x0070D220<SE3SoundEmitterLookup, HASHKEY_NONE>, 0x0070D820<SE3COMBINABLESOUNDLIST, HASHKEY_NONE>, 0x00713FF0<SE3ChatSound, HASHKEY_NONE>, 0x00715170<SE3MemorySoundObject, HASHKEY_NONE>, 0x004FCE90<FILENAMEHASH, HASHKEY_STR>, 0x0086EFD0/0x1405A46A0<CGxDeviceD3d11::InputLayoutRec, InputLayoutKey>, 0x0086F060<CGxDeviceD3d11::RasterizerStateRec, HASHKEY_NONE>, 0x0086F0F0<CGxDeviceD3d11::BlendStateRec, HASHKEY_NONE>, 0x0086F180<CGxDeviceD3d11::DepthStateRec, HASHKEY_NONE>, 0x0086F210<CGxDeviceD3d11::SamplerStateRec, HASHKEY_NONE>, 0x005480A0<FileNotification::DeferredFile, HASHKEY_NONE>, 0x009F34F0<CGSpellActivationOverlay::COUNTERNODE, HASHKEY_NONE>, 0x009F3580<CGSpellActivationOverlay::PETAURANODE, HASHKEY_NONE>, 0x009F7710<CGResearchFrame::BRANCH, HASHKEY_NONE>, 0x009F77A0<CGResearchFrame::BLOBPOIHASH, HASHKEY_NONE>, 0x009CA800<CGCUFProfiles::PROFILENODE, HASHKEY_CONSTUTF8I>, 0x009CA890<CGCUFProfiles::OPTIONENTRY, HASHKEY_CONSTSTR>, 0x0043A330<CCameraModeEntry, HASHKEY_NONE>, 0x00485150<UNITPORTRAIT, HASHKEY_NONE>, 0x004851E0<ITEMPORTRAIT, HASHKEY_STR>, 0x0048C9B0<WowClientDB2_Base::DBTABLE, HASHKEY_NONE>, 0x004965D0<SPELLACTIVATIONOVERLAY, HASHKEY_NONE>, 0x004DEDD0<TALKINGPLAYER, CHashKeyGUID>, 0x004DEE60<MUTEDPLAYER, CHashKeyGUID>, 0x004E5D70<DECLINEDWORD, HASHKEY_CONSTSTRI>, 0x005914A0<CustomFallbackPath, HASHKEY_NONE>, 0x005A9020<INVENTORYART, HASHKEY_NONE>, 0x005A90B0<ITEMBYNAME, HASHKEY_UTF8I>, 0x005E5340<OBJ_EFFECT_DEF_LOOKUP, HASHKEY_NONE>, 0x005E53D0<OBJ_EFFECT_GROUP_DEF_LOOKUP, HASHKEY_NONE>, 0x005E5460<OBJ_EFFECT_PACKAGE_DEF_LOOKUP, HASHKEY_NONE>, 0x0062F7F0<ZONESOUNDOVERRIDELOOKUP, HASHKEY_STRI>, 0x00633620<DSP_CHAIN_HEAD_LOOKUP, HASHKEY_STRI>, 0x00634370<FOOTSTEPSNDCACHE, HASHKEY_NONE>, 0x008B4FE0<ITEMCOOLDOWNHASHNODE, HASHKEY_NONE>, 0x008B5070<DESTLOCSPELLCASTINDEXHASHNODE, CHashKeyGUID>, 0x00917FB0<OVERRIDE_SPELL_NODE, HASHKEY_NONE>, 0x00918040<SPELLSBYNAME, HASHKEY_UTF8I>, 0x0093BAF0<CGGuildInfo::GuildRecipesKnown, HASHKEY_NONE>, 0x0093BC10<CGGuildInfo::SKILLLINESPELLS, HASHKEY_NONE>, 0x009677B0<CGQuestLog::QUESTCOMPLETIONNPC, HASHKEY_NONE>, 0x009A5550<CFlyout, HASHKEY_NONE>, 0x009A55E0<SpellToFlyoutMapItem, HASHKEY_NONE>, 0x009A8B40<FACTION, HASHKEY_NONE>, 0x009FD970<CGIncomingResurrection::INCOMINGRESLIST, CHashKeyGUID>, 0x00447440<_BANNEDADDON, HASHKEY_NONE>, 0x0045F310<UIADDON, HASHKEY_STRI>, 0x008DD710<TAINTEVENT, HASHKEY_STR>, 0x008F4810<WORDLIST, HASHKEY_LANGUAGE>, 0x00925D10<CLICKFRAME, HASHKEY_STR>, 0x0094ED00<ACHIEVEMENT, HASHKEY_NONE>, 0x009B5780<DYNAMICHOLIDAYHASH, HASHKEY_NONE>, 0x009B8BB0<KNOWNDANCES, HASHKEY_STRI>, 0x009CE320<WORLDSTATE, HASHKEY_NONE>, 0x00B332C0<BITMAPBYICON, HASHKEY_NONE>, 0x008F4930<ADDONPREFIX, HASHKEY_STR>]
template <class T, class TKey>
int32_t TSHashTable<T, TKey>::MonitorFullness(uint32_t slot) {
    if (this->m_slotmask >= this->m_maxslots - 1) {
        return 0;
    }

    this->m_fullnessIndicator = this->m_fullnessIndicator > 3 ? this->m_fullnessIndicator - 3 : 0;

    for (T* node = this->m_slotlistarray[slot].Head(); node; node = this->m_slotlistarray[slot].Next(node)) {
        if (++this->m_fullnessIndicator > 13) {
            this->m_fullnessIndicator = 0;
            this->GrowListArray(2 * this->m_slotmask + 2);
            return 1;
        }
    }

    return 0;
}

// OFFSET: [%0x004B3C60, %0x004B3CC0, %0x004B3CF0, #0x00619300]
template <class T, class TKey>
T* TSHashTable<T, TKey>::New(uint32_t hashval, const TKey& key, size_t extrabytes, uint32_t flags) {
    T* node = this->InternalNewNode(hashval, extrabytes, flags);
    node->m_hashval = hashval;
    node->m_key = key;

    return node;
}

// OFFSET: [#0x00509720<PROFANITYCHECK, HASHKEY_STR>, #0x00562870<KEYCOMMAND, HASHKEY_STRI>, #0x005628D0<KEYBINDING, HASHKEY_STRI>]
template <class T, class TKey>
T* TSHashTable<T, TKey>::New(uint32_t hashval, const char* str, size_t extrabytes, uint32_t flags) {
    T* node = this->InternalNewNode(hashval, extrabytes, flags);
    node->m_hashval = hashval;
    node->m_key = str;

    return node;
}

// OFFSET: [#0x005097A0<PROFANITYCHECK, HASHKEY_STR>, %0x008817F0<KEYBINDING, HASHKEY_STRI>, #0x00562930]
template <class T, class TKey>
T* TSHashTable<T, TKey>::New(const char* str, size_t extrabytes, uint32_t flags) {
    return this->New(SStrHashHT(str), str, extrabytes, flags);
}

// OFFSET: [#0x004B6D90<CTexture, HASHKEY_TEXTUREFILE>, %0x0055C0A0<CTextureBlobTexture, HASHKEY_TEXTUREBLOBFILE>, %0x00496230<CGObject_C, CHashKeyGUID>, %0x007CCFD0<WORDLIST, HASHKEY_LANGUAGE>, %0x004B1E50<DBCache<NameCache, unsigned __int64, CHashKeyGUID>::DBCACHEHASH, CHashKeyGUID>, %0x004B1EC0<DBCache<WardenCachedModule, CWardenKey, CWardenKey>::DBCACHEHASH, CWardenKey>, %0x00621B90<CLightList, HASHKEY_DWORD>, %0x0062C9D0<KERNNODE, KERNINGHASHKEY>, %0x009358E0<DBCache<GuildStats_C, int, HASHKEY_INT>::DBCACHEHASH, HASHKEY_INT>, %0x005D73C0<UnitThreat, CHashKeyGUID>, #0x007B0690<CMapObj, HASHKEY_NONE>, %0x00699860<CMapDoodadDef, HASHKEY_DWORD>, #0x007BDDF0<CMapObjDef, HASHKEY_NONE>, #0x00810970, 0x0086D810/0x140752FC0<CMapDynamicTexture, HASHKEY_DWORD>, 0x0077B030<CSceneMaterial, HASHKEY_MATERIAL>, 0x0086D810<CGSpellActivationOverlay::COUNTERNODE, HASHKEY_NONE>, 0x0086D810<CGResearchFrame::BRANCH, HASHKEY_NONE>, 0x0086D810<CGResearchFrame::BLOBPOIHASH, HASHKEY_NONE>]
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

// OFFSET: [#0x004BC410<LCDHANDLE, HASHKEY_CONSTSTRI>, #0x004BE0E0<FONTHASHOBJ, HASHKEY_STR>, %0x005AE600<SPELLSBYNAME, HASHKEY_UTF8I>, #0x0055F4D0<MODIFIEDCLICK, HASHKEY_STRI>, #0x00617C50<KNOWNDANCES, HASHKEY_STRI>, #0x00687870<CGxShader, HASHKEY_STRI>, #0x006C8100<FACEDATA, HASHKEY_STRI>, #0x007E0250<DECLINEDWORD, HASHKEY_CONSTSTRI>, 0x0049D6B0<CGCUFProfiles::PROFILENODE, HASHKEY_CONSTUTF8I>, 0x004E5510<CGCUFProfiles::OPTIONENTRY, HASHKEY_CONSTSTR>]
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

// OFFSET: [%0x00495F60<CGObject_C, CHashKeyGUID>, %0x004A3600<CACHEENTRY, HASHKEY_NONE>, #0x00685230<CMapObj, HASHKEY_NONE>, #0x0079E6A0<CMapObjDef, HASHKEY_NONE>, %0x005C3390, #0x007BFE80<CMapDoodadDef, HASHKEY_DWORD>, 0x0075F800/0x140443950<CMapObjDef, HASHKEY_NONE>, 0x00541B50<CTexture, HASHKEY_TEXTUREFILE>, 0x00703FE0<CGCUFProfiles::PROFILENODE, HASHKEY_CONSTUTF8I>]
template <class T, class TKey>
void TSHashTable<T, TKey>::Unlink(T* ptr) {
    if (ptr->m_linktoslot.IsLinked()) {
        ptr->m_linktoslot.Unlink();
        ptr->m_linktofull.Unlink();
    }
}

// OFFSET: [%0x004C90B0<RGN, HASHKEY_NONE>]
template <class T, class TKey>
TSHashTableReuse<T, TKey>::TSHashTableReuse() {
    this->m_reuseList.ChangeLinkOffset(offsetof(T, m_linktoslot));
}

// OFFSET: [#0x004B8F10, #0x004B8F70, 0x00548A90/0x1401A1030<CModelBlob::CHashEntry, HASHKEY_MODELBLOBFILE>, 0x00548AF0/0x1401A10B0<CModelBlob::CHashEntry, HASHKEY_MODELBLOBFILE>, #0x004BB770, 0x0054C1D0/0x1401A5C50<CTextureBlobTexture, HASHKEY_TEXTUREBLOBFILE>, 0x0054C230/0x1401A5CD0<CTextureBlobTexture, HASHKEY_TEXTUREBLOBFILE>, #0x004C0D90, 0x00611BC0/0x14029C920<CLightList, HASHKEY_DWORD>, 0x00611C20/0x14029C9A0<CLightList, HASHKEY_DWORD>, %0x006226F0, %0x004C99B0<RGN, HASHKEY_NONE>, %0x004C9A10, 0x004D9860<TIMESTAMPSTRUCT, HASHKEY_NONE>, 0x005445A0<CTexture, HASHKEY_TEXTUREFILE>, 0x00544600<CTexture, HASHKEY_TEXTUREFILE>, 0x005E3B90/0x140261B00<OBJ_EFFECT_PACKAGE_DEF_LOOKUP, HASHKEY_NONE>, 0x0070D760/0x1403DD3E0<SE3COMBINABLESOUNDLIST, HASHKEY_NONE>, 0x0070D7F0/0x1403DD460<SE3COMBINABLESOUNDLIST, HASHKEY_NONE>]
template <class T, class TKey>
TSHashTableReuse<T, TKey>::~TSHashTableReuse() {
    this->Destructor();
}

// OFFSET: [#0x004B8EF0, 0x005489D0/0x1401A0E80<CModelBlob::CHashEntry, HASHKEY_MODELBLOBFILE>, 0x0054C090/0x1401A59F0<CTextureBlobTexture, HASHKEY_TEXTUREBLOBFILE>, 0x006110D0/0x14029B880<CLightList, HASHKEY_DWORD>, %0x004C8AB0<RGN, HASHKEY_NONE>, 0x00544580<CTexture, HASHKEY_TEXTUREFILE>, 0x0070D040/0x1403DD1B0<SE3COMBINABLESOUNDLIST, HASHKEY_NONE>]
template <class T, class TKey>
void TSHashTableReuse<T, TKey>::Destroy() {
    this->Clear();
    this->Destructor();
}

// OFFSET: [#0x004B6690, 0x005482D0/0x1401A0490<CModelBlob::CHashEntry, HASHKEY_MODELBLOBFILE>, 0x0054B4D0/0x1401A4840<CTextureBlobTexture, HASHKEY_TEXTUREBLOBFILE>, %0x006216A0<CLightList, HASHKEY_DWORD>, %0x004C8A40<RGN, HASHKEY_NONE>, 0x00541BE0<CTexture, HASHKEY_TEXTUREFILE>, 0x00707A40/0x1403D6010<SE3COMBINABLESOUNDLIST, HASHKEY_NONE>, 0x009125E0<OVERRIDE_SPELL_NODE, HASHKEY_NONE>, 0x00912650<OVERRIDE_SPELL_BY_NAME_NODE, HASHKEY_UTF8I>]
template <class T, class TKey>
void TSHashTableReuse<T, TKey>::Destructor() {
    T* node;

    while ((node = this->m_reuseList.Head()) != NULL) {
        this->m_reuseList.UnlinkNode(node);
        SMemFree(node, typeid(T).raw_name(), SLOG_OBJECT, 0);
    }
}

// OFFSET: [#0x004B8E70, #0x004C0C10<CModelBlob::CHashEntry, HASHKEY_MODELBLOBFILE>, 0x00611A80/0x14029C6D0<CLightList, HASHKEY_DWORD>, %0x004C9180<RGN, HASHKEY_NONE>, 0x00544500<CTexture, HASHKEY_TEXTUREFILE>, 0x0070D6E0/0x1403DD340<SE3COMBINABLESOUNDLIST, HASHKEY_NONE>]
template <class T, class TKey>
void TSHashTableReuse<T, TKey>::InternalDelete(T* ptr) {
    this->m_fulllist.UnlinkNode(ptr);
    ptr->~T();
    new (&ptr->m_linktoslot) TSLink<T>;
    this->m_reuseList.LinkToHead(ptr);
}

// OFFSET: [0x00544940<CTexture, HASHKEY_TEXTUREFILE>, 0x005489F0/0x1401A0EA0<CModelBlob::CHashEntry, HASHKEY_MODELBLOBFILE>, #0x004C0C90<CTextureBlobTexture, HASHKEY_TEXTUREBLOBFILE>, 0x00611B00/0x14029C770<CLightList, HASHKEY_DWORD>, #0x00776F90<RGN, HASHKEY_NONE>, 0x0070D060<SE3COMBINABLESOUNDLIST, HASHKEY_NONE>, 0x0054C130<CTextureBlobTexture, HASHKEY_TEXTUREBLOBFILE>, 0x00917E30<OVERRIDE_SPELL_BY_NAME_NODE, HASHKEY_UTF8I>]
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

// OFFSET: #0x00776240
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

// OFFSET: [0x00409800/0x14000BF30, %0x0080D8F0]
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

// PROVEN: aff02478 IDENTICAL/NONE
// OFFSET: [0x0059B410<MapObjSpeedTransition, 8>]
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

// OFFSET: [0x005AC6A0, #0x007917B0, 0x0058CAA0<AnimKitDesiredBoneSetInst, 10>, 0x005B6340<AuraData, 16>]
template <class T, uint32_t N>
T& TSCGrowableArray<T, N>::operator[](uint32_t index) {
    if (this->m_count == 0xFFFFFFFF) {
        return this->Dynamic().m_data[index];
    }

    return this->Inline()[index];
}

// OFFSET: [%0x005C9E90, %0x005C9F10, %0x005C9F90<unsigned int, 16>, #0x007B3B70<CMapObjDefGroup*, 4>, 0x00760A10/0x1404450E0<CMapObjDefGroup*, 8>, 0x005A1630<MapObjSpeedTransition, 8>, 0x009F5210<const ResearchBranchRec*, 32>]
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

// OFFSET: [0x0058E980<AnimKitSegmentInst*, 8>]
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

// INLINED
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

// OFFSET: [#0x00801120, 0x00773710<CWorldMapPreloadData>]
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

#endif

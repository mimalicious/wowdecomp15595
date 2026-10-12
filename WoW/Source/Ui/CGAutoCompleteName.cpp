#include "CGAutoCompleteName.h"
#include "Engine/Source/Storm/H/storm.h"
#include <cstdio>
#include <cstdlib>
#include <new>
#include <typeinfo>
#include "Engine/Source/Event/FrameTime.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "WoW/Source/WowServices/RealmConnection.h"
#include "WoW/Source/WowSvcs/WowSvcsClient/ClientServices.h"
#include "Engine/Source/lua-5.1/src/lua.h"
#include "WoW/Source/DB/DBClient/DBCacheInstances.h"
#include "WoW/Source/Ui/BattlenetUI.h"

// STATIC: 0x00D16098/0x140BCFFD0
TSList<CGAutoCompleteName::AUTOCOMPLETE_NAMENODE, TSGetLink<CGAutoCompleteName::AUTOCOMPLETE_NAMENODE>> CGAutoCompleteName::m_list;

// STATIC: 0x00F60FE0/0x140BCFFE0
TSHashTable<CGAutoCompleteName::AUTOCOMPLETE_CHARACTER_HASHNODE, CHashKeyGUID> CGAutoCompleteName::m_characterHash;

// STATIC: 0x00F6100C/0x140BD0030
TSHashTable<CGAutoCompleteName::AUTOCOMPLETE_BATTLENET_HASHNODE, HASHKEY_NONE> CGAutoCompleteName::m_battlenetHash;

// STATIC: 0x00F60FD8/0x140E56138
uint32_t CGAutoCompleteName::s_numNodes;

// STATIC: 0x00F60FD4/0x140E56134
bool CGAutoCompleteName::s_resortNamesOnRecency;

// STATIC: 0x00F60FDC/0x140E5613C
bool CGAutoCompleteName::s_batchUpdate;

// STATIC: 0x00F60FDD/0x140E5613D
bool CGAutoCompleteName::s_resortPending;

// STATIC: 0x00F60FD5/0x140E56135
bool CGAutoCompleteName::s_whenEditingFromCenter;

// STATIC: 0x00F60FD6/0x140E56136
bool CGAutoCompleteName::s_useContext;

// OFFSET: 0x009CB210/0x14075AC20
int32_t CGAutoCompleteName::CompareNodes(const void* a, const void* b) {
    const AUTOCOMPLETE_NAMENODE* nodeA = *static_cast<const AUTOCOMPLETE_NAMENODE* const*>(a);
    const AUTOCOMPLETE_NAMENODE* nodeB = *static_cast<const AUTOCOMPLETE_NAMENODE* const*>(b);

    if (CGAutoCompleteName::s_resortNamesOnRecency && nodeA->m_lastUsed != nodeB->m_lastUsed) {
        return nodeA->m_lastUsed - nodeB->m_lastUsed;
    }

    return SStrCmpUTF8I(nodeA->m_name, nodeB->m_name, STORM_MAX_STR);
}

// OFFSET: 0x009CB260/0x14075AC60
const char* CGAutoCompleteName::GetDisplayName(const char* name) {
    if (name[0] == '|' && name[1] == 'K' && name[2] == 'f') {
        uint32_t presenceId = 0;
        sscanf(name, "|Kf%d", &presenceId);

        if (presenceId) {
            return BattlenetUI::GetKStringText('f', presenceId);
        }
    }

    return name;
}

// OFFSET: 0x009CB3A0/0x14075AE40
int32_t CGAutoCompleteName::NodeMatchesParams(const AUTOCOMPLETE_NAMENODE* node, const char* text, uint32_t include, uint32_t exclude, int32_t cursor, bool allowFullMatch) {
    if (CGAutoCompleteName::s_useContext && ((node->m_flags & exclude) || !(node->m_flags & include))) {
        return 0;
    }

    if (ClientServices::GetCharacterInfo()->m_guid == node->m_guid) {
        return 0;
    }

    const char* name = CGAutoCompleteName::GetDisplayName(node->m_name);

    if (!allowFullMatch && !SStrCmpUTF8I(text, name, STORM_MAX_STR)) {
        return 0;
    }

    FrameScript_GetContext();

    if (lua_tainted && node->m_presenceId && SStrCmpUTF8I(text, name, STORM_MAX_STR) && *SStrAdvanceCharsUTF8(name, SStrLenUTF8(text)) != ' ') {
        return 0;
    }

    if (!CGAutoCompleteName::s_whenEditingFromCenter) {
        return SStrCmpUTF8I(text, name, SStrLenUTF8(text)) ? 0 : 1;
    }

    if (SStrCmpUTF8I(text, name, cursor)) {
        return 0;
    }

    int32_t nameLength = SStrLenUTF8(name);
    int32_t textLength = SStrLenUTF8(text);
    int32_t tail = textLength - cursor;

    if (nameLength < textLength) {
        return 0;
    }

    if (textLength == cursor) {
        return 1;
    }

    const char* namePosition = SStrAdvanceCharsUTF8(name, cursor);
    int32_t remaining = nameLength - tail - cursor;
    const char* textPosition = SStrAdvanceCharsUTF8(text, cursor);

    if (!remaining) {
        return 0;
    }

    while (SStrCmpUTF8I(namePosition, textPosition, tail)) {
        remaining--;
        namePosition = SStrAdvanceCharsUTF8(namePosition, 1);

        if (!remaining) {
            return 0;
        }
    }

    return 1;
}

// OFFSET: 0x009CB520/0x14075AFF0
CGAutoCompleteName::AUTOCOMPLETE_NAMENODE* CGAutoCompleteName::FindNodeByName(const char* name, bool characters, bool battlenet) {
    for (AUTOCOMPLETE_NAMENODE* node = CGAutoCompleteName::m_list.Head(); !CGAutoCompleteName::m_list.IsEnd(node); ) {
        AUTOCOMPLETE_NAMENODE* next = node->RawNext();

        if (!SStrCmpUTF8I(name, node->m_name, STORM_MAX_STR)) {
            if ((node->m_presenceId && battlenet) || (!node->m_presenceId && characters)) {
                return node;
            }
        }

        node = next;
    }

    return NULL;
}

// OFFSET: 0x009CB590/0x14075B090
void CGAutoCompleteName::PutNameInCorrectSpot(AUTOCOMPLETE_NAMENODE* node) {
    if (CGAutoCompleteName::s_batchUpdate) {
        CGAutoCompleteName::m_list.LinkToTail(node);
        CGAutoCompleteName::s_resortPending = true;
        return;
    }

    for (AUTOCOMPLETE_NAMENODE* current = CGAutoCompleteName::m_list.Head(); !CGAutoCompleteName::m_list.IsEnd(current); ) {
        AUTOCOMPLETE_NAMENODE* next = current->RawNext();
        int32_t result;

        if (CGAutoCompleteName::s_resortNamesOnRecency && current->m_lastUsed != node->m_lastUsed) {
            result = current->m_lastUsed - node->m_lastUsed;
        } else {
            result = SStrCmpUTF8I(current->m_name, node->m_name, STORM_MAX_STR);
        }

        if (result >= 0 && current != node) {
            CGAutoCompleteName::m_list.LinkNode(node, 2, current);
            return;
        }

        current = next;
    }

    CGAutoCompleteName::m_list.LinkToTail(node);
}

// OFFSET: 0x009CB6E0/0x14075B290
int32_t CGAutoCompleteName::GetMatchingResults(lua_State* L, const char* text, uint32_t include, uint32_t exclude, int32_t numReturns, int32_t cursor, bool allowFullMatch) {
    int32_t count = 0;

    for (AUTOCOMPLETE_NAMENODE* node = CGAutoCompleteName::m_list.Head(); !CGAutoCompleteName::m_list.IsEnd(node); ) {
        AUTOCOMPLETE_NAMENODE* next = node->RawNext();

        if (CGAutoCompleteName::NodeMatchesParams(node, text, include, exclude, cursor, allowFullMatch)) {
            lua_pushstring(L, node->m_name);

            if (++count == numReturns) {
                break;
            }
        }

        node = next;
    }

    return count;
}

// OFFSET: 0x009CB750/0x14075B350
int32_t CGAutoCompleteName::GetPresenceID(const char* name, uint32_t* presenceId) {
    if (name[0] == '|' && name[1] == 'K' && name[2] == 'f') {
        uint32_t id = 0;
        sscanf(name, "|Kf%d", &id);

        if (id) {
            *presenceId = id;
            return 1;
        }

        return 0;
    }

    for (AUTOCOMPLETE_NAMENODE* node = CGAutoCompleteName::m_list.Head(); !CGAutoCompleteName::m_list.IsEnd(node); ) {
        AUTOCOMPLETE_NAMENODE* next = node->RawNext();

        if (node->m_presenceId && (!SStrCmpUTF8I(name, node->m_name, STORM_MAX_STR) || !SStrCmpUTF8I(name, CGAutoCompleteName::GetDisplayName(node->m_name), STORM_MAX_STR))) {
            *presenceId = static_cast<uint32_t>(node->m_guid.m_guid);
            return 1;
        }

        node = next;
    }

    return 0;
}

// OFFSET: 0x009CB870/0x14075B490
void CGAutoCompleteName::ResortAll() {
    TSGrowableArray<AUTOCOMPLETE_NAMENODE*> nodes;
    uint32_t count = CGAutoCompleteName::s_numNodes;
    nodes.SetCount(count);
    AUTOCOMPLETE_NAMENODE** entry = nodes.m_data;

    for (AUTOCOMPLETE_NAMENODE* node = CGAutoCompleteName::m_list.Head(); !CGAutoCompleteName::m_list.IsEnd(node); ) {
        AUTOCOMPLETE_NAMENODE* next = node->RawNext();
        *entry++ = node;
        node = next;
    }

    qsort(nodes.m_data, count, sizeof(AUTOCOMPLETE_NAMENODE*), CGAutoCompleteName::CompareNodes);

    while (count) {
        AUTOCOMPLETE_NAMENODE* node = nodes.m_data[--count];
        CGAutoCompleteName::m_list.UnlinkNode(node);
        CGAutoCompleteName::m_list.LinkToHead(node);
    }

    CGAutoCompleteName::s_resortPending = false;
}

// OFFSET: 0x009CBBC0/0x14075B870
int32_t CGAutoCompleteName::SetName(const WOWGUID& guid, const char* name, bool overwrite) {
    AUTOCOMPLETE_CHARACTER_HASHNODE* hashNode = CGAutoCompleteName::m_characterHash.Ptr(static_cast<uint32_t>(guid.m_guid), CHashKeyGUID(guid.m_guid));
    AUTOCOMPLETE_NAMENODE* node;

    if (!hashNode || !(node = hashNode->m_node)) {
        return 0;
    }

    if (node->m_name[0] && !overwrite) {
        return 0;
    }

    SStrCopy(node->m_name, name, 305);
    return 1;
}

// OFFSET: 0x009CBC40/0x14075B920
int32_t CGAutoCompleteName::SetName(const uint32_t& presenceId, const char* name, bool overwrite) {
    AUTOCOMPLETE_BATTLENET_HASHNODE* hashNode = CGAutoCompleteName::m_battlenetHash.Ptr(presenceId, HASHKEY_NONE());
    AUTOCOMPLETE_NAMENODE* node;

    if (!hashNode || !(node = hashNode->m_node)) {
        return 0;
    }

    if (node->m_name[0] && !overwrite) {
        return 0;
    }

    SStrCopy(node->m_name, name, 305);
    return 1;
}

// OFFSET: 0x009CBCC0/0x14075B9C0
int32_t CGAutoCompleteName::AddFlagsToNode(AUTOCOMPLETE_NAMENODE* node, uint32_t flags, int32_t add, const char* name) {
    node->m_flags |= flags;
    bool resort = false;

    if (add) {
        node->m_lastUsed = FrameTime::GetCurTimeMs();
        resort = true;
    }

    if (name && !node->m_name[0]) {
        SStrCopy(node->m_name, name, 305);
        resort = true;
    }

    if (resort) {
        CGAutoCompleteName::PutNameInCorrectSpot(node);
    }

    return 1;
}

// OFFSET: 0x009CC030
static void AutoCompleteNameQueryCallback(int32_t id, const WOWGUID& guid, void* param, bool success) {
    NameCache* name = g_nameDBCache.GetRecord(guid.m_guid, WOWGUID(), NULL, NULL, false);

    if (name) {
        CGAutoCompleteName::SetName(guid, name->m_name, true);
    }
}

// OFFSET: 0x009CC080/0x14075BE30
void CGAutoCompleteName::RefreshName(const WOWGUID& guid) {
    NameCache* name = g_nameDBCache.GetRecord(guid.m_guid, guid, AutoCompleteNameQueryCallback, NULL, true);

    if (name) {
        CGAutoCompleteName::SetName(guid, name->m_name, true);
    }
}

// OFFSET: 0x009CC260/0x14075C0A0
CGAutoCompleteName::AUTOCOMPLETE_NAMENODE* CGAutoCompleteName::RemoveNode(AUTOCOMPLETE_NAMENODE* node) {
    CGAutoCompleteName::s_numNodes--;

    if (node->m_presenceId) {
        AUTOCOMPLETE_BATTLENET_HASHNODE* hashNode = CGAutoCompleteName::m_battlenetHash.Ptr(static_cast<uint32_t>(node->m_guid.m_guid), HASHKEY_NONE());

        if (hashNode) {
            CGAutoCompleteName::m_battlenetHash.Delete(hashNode);
        } else {
            SErrSetLastError(STORM_ERROR_INVALID_PARAMETER);
        }
    } else {
        AUTOCOMPLETE_CHARACTER_HASHNODE* hashNode = CGAutoCompleteName::m_characterHash.Ptr(static_cast<uint32_t>(node->m_guid.m_guid), CHashKeyGUID(node->m_guid.m_guid));

        if (hashNode) {
            CGAutoCompleteName::m_characterHash.Delete(hashNode);
        } else {
            SErrSetLastError(STORM_ERROR_INVALID_PARAMETER);
        }
    }

    return CGAutoCompleteName::m_list.DeleteNode(node);
}

// OFFSET: 0x009CC340/0x14075C200
void CGAutoCompleteName::RemoveAllBattlenet() {
    for (AUTOCOMPLETE_NAMENODE* node = CGAutoCompleteName::m_list.Head(); !CGAutoCompleteName::m_list.IsEnd(node); ) {
        AUTOCOMPLETE_NAMENODE* next = node->RawNext();

        if (node->m_presenceId) {
            CGAutoCompleteName::RemoveNode(node);
        }

        node = next;
    }
}

// OFFSET: 0x009CC5A0/0x14075C590
int32_t CGAutoCompleteName::RemoveFlags(const WOWGUID& guid, uint32_t flags) {
    AUTOCOMPLETE_CHARACTER_HASHNODE* hashNode = CGAutoCompleteName::m_characterHash.Ptr(static_cast<uint32_t>(guid.m_guid), CHashKeyGUID(guid.m_guid));
    AUTOCOMPLETE_NAMENODE* node;

    if (hashNode && (node = hashNode->m_node)) {
        node->m_flags &= ~flags;

        if (!(node->m_flags & ~0x28)) {
            CGAutoCompleteName::RemoveNode(node);
        }
    }

    return 1;
}

// OFFSET: 0x009CC600/0x14075C620
int32_t CGAutoCompleteName::RemoveFlags(const uint32_t& presenceId, uint32_t flags) {
    AUTOCOMPLETE_BATTLENET_HASHNODE* hashNode = CGAutoCompleteName::m_battlenetHash.Ptr(presenceId, HASHKEY_NONE());
    AUTOCOMPLETE_NAMENODE* node;

    if (hashNode && (node = hashNode->m_node)) {
        node->m_flags &= ~flags;

        if (!(node->m_flags & ~0x28)) {
            CGAutoCompleteName::RemoveNode(node);
        }
    }

    return 1;
}

// OFFSET: 0x009CC650/0x14075C6A0
int32_t CGAutoCompleteName::RemoveFlagsByName(const char* name, uint32_t flags, bool characters, bool battlenet) {
    AUTOCOMPLETE_NAMENODE* node = CGAutoCompleteName::FindNodeByName(name, characters, battlenet);

    if (node) {
        node->m_flags &= ~flags;

        if (!(node->m_flags & ~0x28) && CGAutoCompleteName::s_useContext) {
            CGAutoCompleteName::RemoveNode(node);
        }
    }

    return 1;
}

// OFFSET: 0x009CC6A0/0x14075C6F0
int32_t CGAutoCompleteName::RemoveFlagsFromAllNodes(uint32_t flags) {
    for (AUTOCOMPLETE_NAMENODE* node = CGAutoCompleteName::m_list.Head(); !CGAutoCompleteName::m_list.IsEnd(node); ) {
        AUTOCOMPLETE_NAMENODE* next = node->RawNext();
        node->m_flags &= ~flags;

        if (!(node->m_flags & ~0x28) && CGAutoCompleteName::s_useContext) {
            CGAutoCompleteName::RemoveNode(node);
        }

        node = next;
    }

    return 1;
}

// OFFSET: 0x009CD0F0/0x14075D530
CGAutoCompleteName::AUTOCOMPLETE_NAMENODE* CGAutoCompleteName::AddPlayerToList(const WOWGUID& guid, uint32_t flags, uint32_t lastUsed, const char* name) {
    AUTOCOMPLETE_NAMENODE* node = new (SMemAlloc(sizeof(AUTOCOMPLETE_NAMENODE), typeid(AUTOCOMPLETE_NAMENODE).raw_name(), -2, 0x8)) AUTOCOMPLETE_NAMENODE;
    CGAutoCompleteName::s_numNodes++;
    AUTOCOMPLETE_CHARACTER_HASHNODE* hashNode = CGAutoCompleteName::m_characterHash.New(static_cast<uint32_t>(guid.m_guid), CHashKeyGUID(guid.m_guid), 0, 0);
    hashNode->m_node = node;
    node->m_presenceId = 0;
    node->m_guid = guid;

    if (name) {
        SStrCopy(node->m_name, name, 305);
    } else {
        node->m_name[0] = '\0';
    }

    if (node->m_lastUsed != lastUsed) {
        node->m_lastUsed = lastUsed;
        CGAutoCompleteName::PutNameInCorrectSpot(node);
    }

    node->m_flags = flags;
    CGAutoCompleteName::PutNameInCorrectSpot(node);
    return node;
}

// OFFSET: 0x009CD230/0x14075D750
CGAutoCompleteName::AUTOCOMPLETE_NAMENODE* CGAutoCompleteName::AddPlayerToList(const uint32_t& presenceId, uint32_t flags, uint32_t lastUsed, const char* name) {
    AUTOCOMPLETE_NAMENODE* node = new (SMemAlloc(sizeof(AUTOCOMPLETE_NAMENODE), typeid(AUTOCOMPLETE_NAMENODE).raw_name(), -2, 0x8)) AUTOCOMPLETE_NAMENODE;
    CGAutoCompleteName::s_numNodes++;
    AUTOCOMPLETE_BATTLENET_HASHNODE* hashNode = CGAutoCompleteName::m_battlenetHash.New(presenceId, HASHKEY_NONE(), 0, 0);
    hashNode->m_node = node;
    node->m_presenceId = 1;
    *reinterpret_cast<uint32_t*>(&node->m_guid) = presenceId;

    if (name) {
        SStrCopy(node->m_name, name, 305);
    } else {
        node->m_name[0] = '\0';
    }

    if (node->m_lastUsed != lastUsed) {
        node->m_lastUsed = lastUsed;
        CGAutoCompleteName::PutNameInCorrectSpot(node);
    }

    node->m_flags = flags | 0x8;
    CGAutoCompleteName::PutNameInCorrectSpot(node);
    return node;
}

// OFFSET: 0x009CD350/0x14075D980
int32_t CGAutoCompleteName::AddFlags(const WOWGUID& guid, uint32_t flags, int32_t add, const char* name) {
    if (!guid.m_guid) {
        return 0;
    }

    AUTOCOMPLETE_CHARACTER_HASHNODE* hashNode = CGAutoCompleteName::m_characterHash.Ptr(static_cast<uint32_t>(guid.m_guid), CHashKeyGUID(guid.m_guid));
    AUTOCOMPLETE_NAMENODE* node;

    if (hashNode && (node = hashNode->m_node)) {
        return CGAutoCompleteName::AddFlagsToNode(node, flags, add, name);
    }

    if (!(flags & ~0x28)) {
        return 0;
    }

    return CGAutoCompleteName::AddPlayerToList(guid, flags, add ? FrameTime::GetCurTimeMs() : 0, name) != NULL;
}

// OFFSET: 0x009CD3F0/0x14075DA70
int32_t CGAutoCompleteName::AddFlags(const uint32_t& presenceId, uint32_t flags, int32_t add, const char* name) {
    AUTOCOMPLETE_BATTLENET_HASHNODE* hashNode = CGAutoCompleteName::m_battlenetHash.Ptr(presenceId, HASHKEY_NONE());
    AUTOCOMPLETE_NAMENODE* node;

    if (hashNode && (node = hashNode->m_node)) {
        return CGAutoCompleteName::AddFlagsToNode(node, flags, add, name);
    }

    if (!(flags & ~0x28)) {
        return 0;
    }

    return CGAutoCompleteName::AddPlayerToList(presenceId, flags, add ? FrameTime::GetCurTimeMs() : 0, name) != NULL;
}

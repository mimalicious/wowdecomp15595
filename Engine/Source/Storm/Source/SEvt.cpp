#include <cstdint>
#include "SEvt.h"
#include "Engine/Source/Storm/H/SCritSect.h"
#include "Engine/Source/Storm/H/storm.h"

static int32_t s_modified;

// STATIC: 0x00DA60AC
static _TYPEHASHENTRY** s_typehashtable;

// STATIC: 0x00DA60B0
static uint32_t s_typehashtablesize;

static uint32_t s_typehashtableused;

// STATIC: 0x00DA60B8
static CCritSect s_critsect;

// OFFSET: 0x004C17C0/0x1400FC090
static void DeleteIdHashTable(_IDHASHTABLE* idhashtable) {
    for (uint32_t i = 0; i < idhashtable->size; i++) {
        while (_IDHASHENTRY* entry = idhashtable->data[i]) {
            idhashtable->data[i] = entry->next;
            delete entry;
        }
    }

    SMemFree(idhashtable->data, __FILE__, __LINE__, 0);
    delete idhashtable;
}

// INLINED: [0x0077EB21->0x0077EB7C]
static _TYPEHASHENTRY* FindTypeHashEntry(uint32_t type, uintptr_t subtype) {
    if (!s_typehashtable || !s_typehashtablesize) {
        return NULL;
    }

    for (_TYPEHASHENTRY* entry = s_typehashtable[(s_typehashtablesize - 1) & (subtype ^ type)]; entry; entry = entry->next) {
        if (entry->type == type && entry->subtype == subtype) {
            return entry;
        }
    }

    return NULL;
}

// OFFSET: 0x004C1820/0x1400FC130
int32_t STORMAPI SEvtDestroy() {
    s_critsect.Enter();

    for (uint32_t i = 0; i < s_typehashtablesize; i++) {
        while (_TYPEHASHENTRY* entry = s_typehashtable[i]) {
            while (_IDHASHTABLE* idhashtable = entry->idhashtable) {
                entry->idhashtable = idhashtable->next;
                DeleteIdHashTable(idhashtable);
            }

            s_typehashtable[i] = entry->next;
            delete entry;
        }
    }

    delete s_typehashtable;
    s_typehashtable = NULL;
    s_typehashtablesize = 0;
    s_typehashtableused = 0;
    s_modified = 1;

    s_critsect.Leave();
    return 1;
}

// OFFSET: 0x004C18E0
int32_t STORMAPI SEvtUnregisterType(uint32_t type, uintptr_t subtype) {
    int32_t result = 0;

    s_critsect.Enter();

    _TYPEHASHENTRY* entry = FindTypeHashEntry(type, subtype);

    if (entry) {
        while (_IDHASHTABLE* idhashtable = entry->idhashtable) {
            entry->idhashtable = idhashtable->next;
            DeleteIdHashTable(idhashtable);
        }

        _TYPEHASHENTRY** link = &s_typehashtable[(s_typehashtablesize - 1) & (subtype ^ type)];

        while (*link) {
            if (*link == entry) {
                _TYPEHASHENTRY* curr = *link;
                *link = curr->next;
                delete curr;
                s_typehashtableused--;
            } else {
                link = &(*link)->next;
            }
        }

        result = 1;
        s_modified = 1;
    }

    s_critsect.Leave();
    return result;
}

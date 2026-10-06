#ifndef ENGINE_SOURCE_STORM_SOURCE_SCMD_H
#define ENGINE_SOURCE_STORM_SOURCE_SCMD_H

#include <cstdint>
#include "Engine/Source/Storm/H/stpl.h"
#include "Engine/Source/Storm/H/storm.h"

struct CMDDEF : TSLinkedNode<CMDDEF> {
    /* 0008 */ uint32_t flags;
    /* 000C */ uint32_t id;
    /* 0010 */ char name[24];
    /* 0028 */ int32_t namelength;
    /* 002C */ uint32_t setvalue;
    /* 0030 */ uint32_t setmask;
    /* 0034 */ void* variableptr;
    /* 0038 */ uint32_t variablebytes;
    /* 003C */ int32_t (STORMAPI* callback)(_CMDPARAMS*, const char*);
    /* 0040 */ int32_t found;
    union {
        /* 0044 */ uint32_t currvalue;
        /* 0044 */ char* currvaluestr;
    };
};

struct _PROCESSING {
    /* 0000 */ CMDDEF* ptr;
    /* 0004 */ char name[24];
    /* 001C */ int32_t namelength;
};

void GenerateError(void (STORMAPI* errorcallback)(_CMDERROR*), uint32_t errorcode, const char* itemstring);
int32_t ProcessFile(const char* filename, _PROCESSING* processing, CMDDEF** nextarg, int32_t (STORMAPI* extracallback)(const char*), void (STORMAPI* errorcallback)(_CMDERROR*));
int32_t ProcessString(const char** stringptr, _PROCESSING* processing, CMDDEF** nextarg, int32_t (STORMAPI* extracallback)(const char*), void (STORMAPI* errorcallback)(_CMDERROR*));

#endif

#ifndef WOW_SOURCE_GAME_NAMEGEN_H
#define WOW_SOURCE_GAME_NAMEGEN_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Storm/H/stpl.h"

struct DictionaryRecord {
    /* 0000 */ char m_str [4];
    /* 0004 */ int32_t m_count;
};

class CRndSeed;

class CNameGen {
    public:
    /* 0000 */ CRndSeed* m_seed;
    /* 0004 */ TSFixedArray<const char*> m_names;
    /* 0010 */ TSGrowableArray<DictionaryRecord> m_dictionary;
    /* 0020 */ int32_t m_dictionaryCount;

    CNameGen(CRndSeed& seed);
    void ClearDictionary();
    void Destroy();
    void CreateDictionary();
    void GenerateName(char* buffer, uint32_t size);
    void LoadNames(int32_t raceID, int32_t sexID);
};

#endif

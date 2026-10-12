#include <cstddef>
#include <cstdint>
#include <cstring>
#include "WoW/Source/SoundInterface/SI3Emotes.h"
#include "Engine/Source/SoundEngine/SE3.h"
#include "Engine/Source/Storm/H/stpl.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/SoundInterface/SI3.h"

// STATIC: 0x00E287E8/0x140D10428
static TSGrowableArray<TEXTEMOTELOOKUP> s_textEmoteLookups;

// STATIC: 0x00E287F8/0x140D10440
static TSGrowableArray<TEXTEMOTELOOKUP*> s_textEmoteLookupById;

// INLINED
TEXTEMOTELOOKUP::TEXTEMOTELOOKUP() {
    this->m_emotesTextId = 0;
    memset(this->m_soundIds, 0, sizeof(this->m_soundIds));
}

// PROVEN: eb0515c2 IDENTICAL/DIFFERENT
// OFFSET: 0x00631950/0x1402C6D90
int32_t SI3::GetTextEmoteSoundID(uint32_t emotesTextId, uint32_t race, uint32_t sex) {
    TEXTEMOTELOOKUP* lookup;

    if (emotesTextId < s_textEmoteLookupById.Count() && (lookup = s_textEmoteLookupById[emotesTextId]) != NULL && race < 24 && sex < 2) {
        return lookup->m_soundIds[race][sex];
    }

    return 0;
}

// OFFSET: 0x00631990/0x1402C6DD0
int32_t SI3::GetTextEmoteSoundIndex(uint32_t emotesTextId, uint32_t race, uint32_t sex) {
    TEXTEMOTELOOKUP* lookup;
    int32_t soundKitId;

    if (emotesTextId < s_textEmoteLookupById.Count() && (lookup = s_textEmoteLookupById[emotesTextId]) != NULL && race < 24 && sex < 2) {
        soundKitId = lookup->m_soundIds[race][sex];
    } else {
        soundKitId = 0;
    }

    SE3SoundKitDef* def = SE3::GetSoundKitDef(soundKitId);

    if (!def || !def->m_nNumFiles) {
        return -1;
    }

    SE3SoundKitProperties properties;
    properties.m_selectMode = 2;
    int32_t index = 0;
    def->SelectNextFile(&properties, &index);
    return index;
}

// PROVEN: c10ba697 IDENTICAL/DIFFERENT
// OFFSET: 0x00631B00/0x1402C6F60
void SI3::InitEmoteSounds() {
    if (!g_emotesTextDB.m_numRecords || !g_emotesTextSoundDB.m_numRecords) {
        return;
    }

    s_textEmoteLookupById.SetCount(g_emotesTextDB.m_maxID + 1);
    memset(s_textEmoteLookupById.Ptr(), 0, sizeof(TEXTEMOTELOOKUP*) * s_textEmoteLookupById.Count());

    int32_t last = -1;
    int32_t count = 0;

    for (int32_t i = 0; i < g_emotesTextSoundDB.m_numRecords; i++) {
        EmotesTextSoundRec* rec = g_emotesTextSoundDB.GetRecordByIndex(i);

        if (last == -1 || last != rec->m_emotesTextID) {
            last = rec->m_emotesTextID;
            count++;
        }
    }

    s_textEmoteLookups.m_chunk = count;

    for (int32_t i = 0; i < g_emotesTextSoundDB.m_numRecords; i++) {
        EmotesTextSoundRec* rec = g_emotesTextSoundDB.GetRecordByIndex(i);
        uint32_t id = rec->m_emotesTextID;

        if (id >= s_textEmoteLookupById.Count()) {
            continue;
        }

        TEXTEMOTELOOKUP** slot = &s_textEmoteLookupById[id];

        if (!*slot) {
            *slot = s_textEmoteLookups.New();
            (*slot)->m_emotesTextId = rec->m_emotesTextID;
        }

        if (static_cast<uint32_t>(rec->m_raceID) < 24 && static_cast<uint32_t>(rec->m_sexID) < 2) {
            (*slot)->m_soundIds[rec->m_raceID][rec->m_sexID] = rec->m_soundID;
        }
    }
}

// PROVEN: 2485bc42 IDENTICAL/UNPROVEN
// OFFSET: 0x00631C40/0x1402C7130
void SI3::ShutDownEmoteSounds() {
    s_textEmoteLookups.Clear();
    s_textEmoteLookupById.Clear();
}

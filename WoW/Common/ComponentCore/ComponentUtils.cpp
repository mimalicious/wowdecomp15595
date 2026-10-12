#include <cstddef>
#include <cstdint>
#include "WoW/Common/ComponentCore/ComponentUtils.h"
#include "Engine/Source/Storm/H/storm.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/CharSectionsRec.h"
#include "WoW/Source/DB/Rec/CharacterFacialHairStylesRec.h"

// OFFSET: 0x00704C60/0x1403D1A80
bool ComponentFlagsMatch(uint32_t flags, COMPONENT_SELECTION selection) {
    switch (selection) {
        case COMPONENT_SELECTION_0:
            return (flags & 0x1) && !(flags & 0xC);

        case COMPONENT_SELECTION_1:
            return (flags & 0x1) && (flags & 0x14) && !(flags & 0x8);

        case COMPONENT_SELECTION_2:
            return (flags & 0x3) && !(flags & 0xC);

        case COMPONENT_SELECTION_3:
            return (flags & 0x3) && (flags & 0x14) && !(flags & 0x8);

        case COMPONENT_SELECTION_4:
            return true;

        case COMPONENT_SELECTION_5:
            return !(flags & 0xC);

        case COMPONENT_SELECTION_6:
            return (flags & 0x14) && !(flags & 0x8);

        default:
            return false;
    }
}

// OFFSET: 0x00704D00/0x1403D1B10
COMPONENT_SELECTION GetSelectionFromContext(COMPONENT_CONTEXT context, int32_t classID) {
    switch (context) {
        case COMPONENT_CONTEXT_1:
            return static_cast<COMPONENT_SELECTION>((classID == 6) + COMPONENT_SELECTION_2);

        case COMPONENT_CONTEXT_2:
            return COMPONENT_SELECTION_4;

        case COMPONENT_CONTEXT_3:
            return static_cast<COMPONENT_SELECTION>((classID == 6) + COMPONENT_SELECTION_5);

        default:
            return static_cast<COMPONENT_SELECTION>(classID == 6);
    }
}

// OFFSET: 0x00704D50/0x1403D1B50
COMPONENT_CONTEXT GetContextFromSelection(COMPONENT_SELECTION selection) {
    switch (selection) {
        case COMPONENT_SELECTION_2:
        case COMPONENT_SELECTION_3:
            return COMPONENT_CONTEXT_1;

        case COMPONENT_SELECTION_4:
            return COMPONENT_CONTEXT_2;

        case COMPONENT_SELECTION_5:
        case COMPONENT_SELECTION_6:
            return COMPONENT_CONTEXT_3;

        default:
            return COMPONENT_CONTEXT_0;
    }
}

// OFFSET: 0x00704DA0/0x1403D1BA0
int32_t ComponentGetNumVariations(st_race* lookup, int32_t raceID, int32_t sexID, COMPONENT_VARIATIONS variation) {
    CharVariationList& list = lookup[raceID * 2 + sexID].m_variation[variation];

    if (!list.m_variation) {
        return 0;
    }

    return list.m_numVariations;
}

// OFFSET: 0x00704DD0/0x1403D1BD0
int32_t ComponentGetNumColors(st_race* lookup, int32_t raceID, int32_t sexID, COMPONENT_VARIATIONS variation, int32_t variationIndex) {
    CharVariationList& list = lookup[raceID * 2 + sexID].m_variation[variation];

    if (!list.m_variation || variationIndex >= list.m_numVariations || list.m_numVariations <= 0 || !list.m_variation[variationIndex].m_color) {
        return 0;
    }

    return list.m_variation[variationIndex].m_numColors;
}

// OFFSET: 0x00704E10/0x1403D1C20
bool ComponentValidateBase(st_race* lookup, int32_t raceID, int32_t sexID, COMPONENT_VARIATIONS variation, int32_t variationIndex, int32_t colorIndex) {
    if (static_cast<uint32_t>(variation) >= NUM_COMPONENT_VARIATIONS || variationIndex < 0) {
        return false;
    }

    CharVariationList& list = lookup[raceID * 2 + sexID].m_variation[variation];

    if (variationIndex >= list.m_numVariations || !list.m_numVariations || colorIndex < 0) {
        return false;
    }

    CharColorList& colors = list.m_variation[variationIndex];

    if (colorIndex >= colors.m_numColors || !colors.m_numColors) {
        return false;
    }

    return colors.m_color[colorIndex] != NULL;
}

// OFFSET: 0x00704E70/0x1403D1C90
CharSectionsRec* ComponentGetSectionsRecord(st_race* lookup, int32_t raceID, int32_t sexID, COMPONENT_VARIATIONS variation, int32_t variationIndex, int32_t colorIndex, bool* found) {
    if (ComponentValidateBase(lookup, raceID, sexID, variation, variationIndex, colorIndex)) {
        if (found) {
            *found = true;
        }

        return lookup[raceID * 2 + sexID].m_variation[variation].m_variation[variationIndex].m_color[colorIndex];
    }

    if (found) {
        *found = false;
    }

    return NULL;
}

// OFFSET: 0x00704EE0/0x1403D1D30
bool ComponentValidateSkin(st_race* lookup, int32_t raceID, int32_t sexID, int32_t classID, int32_t colorIndex, COMPONENT_CONTEXT context) {
    CharVariationList& list = lookup[raceID * 2 + sexID].m_variation[VARIATION_SKIN];

    if (list.m_numVariations <= 0 || colorIndex < 0) {
        return false;
    }

    CharColorList& colors = list.m_variation[0];

    if (colorIndex >= colors.m_numColors || !colors.m_numColors || !colors.m_color[colorIndex]) {
        return false;
    }

    return ComponentFlagsMatch(colors.m_color[colorIndex]->m_flags, GetSelectionFromContext(context, classID));
}

// OFFSET: 0x00704F50/0x1403D1DE0
bool ComponentValidateHair(st_race* lookup, int32_t raceID, int32_t sexID, int32_t classID, int32_t colorIndex, int32_t variationIndex, COMPONENT_CONTEXT context) {
    if (variationIndex < 0) {
        return false;
    }

    CharVariationList& list = lookup[raceID * 2 + sexID].m_variation[VARIATION_HAIR];

    if (variationIndex >= list.m_numVariations || !list.m_numVariations || colorIndex < 0) {
        return false;
    }

    CharColorList& colors = list.m_variation[variationIndex];

    if (colorIndex >= colors.m_numColors || !colors.m_numColors || !colors.m_color[colorIndex]) {
        return false;
    }

    return ComponentFlagsMatch(colors.m_color[colorIndex]->m_flags, GetSelectionFromContext(context, classID));
}

// OFFSET: 0x00704FD0/0x1403D1EB0
bool ComponentValidateFace(st_race* lookup, int32_t raceID, int32_t sexID, int32_t classID, int32_t colorIndex, int32_t variationIndex, COMPONENT_CONTEXT context) {
    if (variationIndex < 0) {
        return false;
    }

    CharVariationList& list = lookup[raceID * 2 + sexID].m_variation[VARIATION_FACE];

    if (variationIndex >= list.m_numVariations || !list.m_numVariations || colorIndex < 0) {
        return false;
    }

    CharColorList& colors = list.m_variation[variationIndex];

    if (colorIndex >= colors.m_numColors || !colors.m_numColors || !colors.m_color[colorIndex]) {
        return false;
    }

    return ComponentFlagsMatch(colors.m_color[colorIndex]->m_flags, GetSelectionFromContext(context, classID));
}

// OFFSET: 0x00705050/0x1403D1F80
bool ComponentValidateFacialFeature(st_race* lookup, uint32_t* facialFeatures, int32_t raceID, int32_t sexID, int32_t classID, int32_t colorIndex, int32_t variationIndex, COMPONENT_CONTEXT context) {
    if (variationIndex < 0) {
        return false;
    }

    int32_t index = raceID * 2 + sexID;

    if (static_cast<uint32_t>(variationIndex) >= facialFeatures[index]) {
        return false;
    }

    CharVariationList& list = lookup[index].m_variation[VARIATION_FACIAL_HAIR];

    if (variationIndex >= list.m_numVariations || !list.m_numVariations || colorIndex < 0) {
        return true;
    }

    CharColorList& colors = list.m_variation[variationIndex];

    if (colorIndex >= colors.m_numColors || !colors.m_numColors || !colors.m_color[colorIndex]) {
        return true;
    }

    return ComponentFlagsMatch(colors.m_color[colorIndex]->m_flags, GetSelectionFromContext(context, classID));
}

// OFFSET: 0x007050D0/0x1403D2060
int32_t BuildComponentArray(uint32_t count, st_race** lookupOut) {
    if (!lookupOut) {
        return 0;
    }

    st_race* lookup = new (__FILE__, __LINE__) st_race[count];

    int32_t variationIndex = 0;
    CharSectionsRec* first = g_charSectionsDB.GetRecordByIndex(0);
    int32_t raceID = first->m_raceID;
    int32_t sexID = first->m_sexID;
    int32_t baseSection = first->m_baseSection;

    for (int32_t i = 0; i < g_charSectionsDB.m_numRecords; i++) {
        CharSectionsRec* rec = g_charSectionsDB.GetRecordByIndex(i);

        if (rec->m_baseSection >= NUM_COMPONENT_VARIATIONS) {
            continue;
        }

        if (raceID != rec->m_raceID || sexID != rec->m_sexID || baseSection != rec->m_baseSection) {
            CharVariationList& list = lookup[raceID * 2 + sexID].m_variation[baseSection];
            list.m_numVariations = variationIndex + 1;

            if (list.m_numVariations > 0) {
                list.m_variation = new (__FILE__, __LINE__) CharColorList[list.m_numVariations];
            }

            variationIndex = 0;
        }

        if (variationIndex <= rec->m_variationIndex) {
            variationIndex = rec->m_variationIndex;
        }

        raceID = rec->m_raceID;
        sexID = rec->m_sexID;
        baseSection = rec->m_baseSection;
    }

    CharVariationList& lastList = lookup[raceID * 2 + sexID].m_variation[baseSection];
    lastList.m_numVariations = variationIndex + 1;

    if (lastList.m_numVariations > 0) {
        lastList.m_variation = new (__FILE__, __LINE__) CharColorList[lastList.m_numVariations];
    }

    sexID = 0;
    raceID = 1;
    baseSection = 0;
    variationIndex = 0;
    int32_t colorIndex = 0;

    for (int32_t i = 0; i < g_charSectionsDB.m_numRecords; i++) {
        CharSectionsRec* rec = g_charSectionsDB.GetRecordByIndex(i);

        if (rec->m_baseSection >= NUM_COMPONENT_VARIATIONS) {
            continue;
        }

        if (raceID != rec->m_raceID || sexID != rec->m_sexID || baseSection != rec->m_baseSection || variationIndex != rec->m_variationIndex) {
            CharVariationList& list = lookup[raceID * 2 + sexID].m_variation[baseSection];

            if (list.m_numVariations > 0) {
                CharColorList& colors = list.m_variation[variationIndex];
                colors.m_numColors = colorIndex + 1;

                if (colors.m_numColors > 0) {
                    colors.m_color = new (__FILE__, __LINE__) CharSectionsRec*[colorIndex + 1]();
                }
            }

            colorIndex = 0;
        }

        if (colorIndex <= rec->m_colorIndex) {
            colorIndex = rec->m_colorIndex;
        }

        raceID = rec->m_raceID;
        sexID = rec->m_sexID;
        baseSection = rec->m_baseSection;
        variationIndex = rec->m_variationIndex;
    }

    CharVariationList& lastColors = lookup[raceID * 2 + sexID].m_variation[baseSection];

    if (lastColors.m_numVariations > 0) {
        CharColorList& colors = lastColors.m_variation[variationIndex];
        colors.m_numColors = colorIndex + 1;

        if (colors.m_numColors > 0) {
            colors.m_color = new (__FILE__, __LINE__) CharSectionsRec*[colorIndex + 1]();
        }
    }

    for (int32_t i = 0; i < g_charSectionsDB.m_numRecords; i++) {
        CharSectionsRec* rec = g_charSectionsDB.GetRecordByIndex(i);

        if (rec->m_baseSection >= NUM_COMPONENT_VARIATIONS) {
            continue;
        }

        int32_t recVariation = rec->m_variationIndex;
        int32_t recColor = rec->m_colorIndex;
        CharVariationList& list = lookup[rec->m_raceID * 2 + rec->m_sexID].m_variation[rec->m_baseSection];

        if (list.m_numVariations > 0 && recVariation < list.m_numVariations) {
            CharColorList& colors = list.m_variation[recVariation];

            if (colors.m_numColors > 0 && recColor < colors.m_numColors) {
                colors.m_color[recColor] = rec;
            }
        }
    }

    *lookupOut = lookup;

    return 1;
}

// OFFSET: 0x007054A0/0x1403D2550
int32_t CountFacialFeatures(uint32_t count, uint32_t** facialFeatures) {
    uint32_t* counts = static_cast<uint32_t*>(SMemAlloc(sizeof(uint32_t) * count, __FILE__, __LINE__, SMEM_FLAG_ZEROMEMORY));

    if (!counts) {
        return 0;
    }

    for (int32_t i = 0; i < g_characterFacialHairStylesDB.m_numRecords; i++) {
        CharacterFacialHairStylesRec* rec = g_characterFacialHairStylesDB.GetRecordByIndex(i);

        if (!rec) {
            return 0;
        }

        counts[rec->m_raceID * 2 + rec->m_sexID]++;
    }

    *facialFeatures = counts;

    return 1;
}

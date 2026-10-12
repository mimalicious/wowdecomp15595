#include "WoW/Source/Ui/GlyphInfo.h"
#include <cstdlib>
#include <cstring>
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/Storm/H/storm.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/GlyphPropertiesRec.h"
#include "WoW/Source/DB/Rec/SkillLineAbilityRec.h"
#include "WoW/Source/DB/Rec/SpellEffectRec.h"
#include "WoW/Source/DB/Rec/SpellRec.h"
#include "WoW/Source/Client.h"
#include "WoW/Source/Game/ValidateName.h"
#include "WoW/Source/Magic/MagicClient/Spell_C.h"
#include "WoW/Source/Magic/MagicClient/SpellTable.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/Object/Player.h"
#include "WoW/Source/Object/Unit.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/Ui/GameUI.h"
#include "WoW/Source/Ui/ScriptEvents.h"

// STATIC: 0x00F76868/0x140E6C8F0
uint32_t CGGlyphInfo::s_filter;

// STATIC: 0x00F7686C/0x140E6C8F8
char* CGGlyphInfo::s_nameFilter;

// STATIC: 0x00F76870/0x140E6C900
TSGrowableArray<GlyphItem*> CGGlyphInfo::s_glyphs;

// STATIC: 0x00D16D38/0x140BD1510
int32_t CGGlyphInfo::s_numFiltered = -1;

// STATIC: 0x00D16D3C/0x140BD1514
int32_t CGGlyphInfo::s_clearGlyphSpell = -1;

// STATIC: 0x00D16D40/0x140BD1518
int32_t CGGlyphInfo::s_selectedGlyph = -1;

// PROVEN: 82b72423 IDENTICAL/IDENTICAL
// OFFSET: 0x009EECC0/0x1407883B0
void ClearGlyphReagentCallback(int32_t id, const WOWGUID& guid, void* param, bool success) {
    FrameScript_SignalEvent(SCRIPT_EVENT_USE_GLYPH, NULL);
}

// PROVEN: 67c804f0 IDENTICAL/DIFFERENT
// OFFSET: 0x009EEDC0/0x140788540
static int CompareGlyphs(const void* a, const void* b) {
    const GlyphItem* glyph1 = *static_cast<GlyphItem* const*>(a);
    const GlyphItem* glyph2 = *static_cast<GlyphItem* const*>(b);

    if (!glyph1 || !glyph2) {
        return 0;
    }

    if (glyph1->m_hidden != glyph2->m_hidden) {
        return glyph2->m_hidden ? -1 : 1;
    }

    if (glyph1->m_type != glyph2->m_type) {
        return glyph1->m_type < glyph2->m_type ? -1 : 1;
    }

    if (glyph1->m_spellID == -1 || glyph2->m_spellID == -1) {
        return glyph1->m_spellID == -1 ? -1 : 1;
    }

    if (glyph1->m_known != glyph2->m_known) {
        return glyph1->m_known ? -1 : 1;
    }

    SpellRec* spell1 = g_spellDB.GetRecord(glyph1->m_spellID);
    SpellRec* spell2 = g_spellDB.GetRecord(glyph2->m_spellID);

    if (!spell1 || !spell2) {
        return 0;
    }

    return SStrSortUTF8I(spell1->m_name, spell2->m_name, STORM_MAX_STR);
}

// OFFSET: 0x009EEE90/0x140788680
void CGGlyphInfo::AddKnownGlyph(int32_t spellId) {
    int32_t i;

    for (i = CGGlyphInfo::s_glyphs.Count() - 1; i >= 0; i--) {
        if (CGGlyphInfo::s_glyphs[i]->m_spellID == spellId) {
            CGGlyphInfo::s_glyphs[i]->m_known = true;
            break;
        }
    }

    if (i < 0) {
        SpellRec* spell = g_spellDB.GetRecord(spellId);

        if (spell) {
            uint32_t count;
            const TSGrowableArray<const SpellEffectRec*>* effects = SpellGetEffects(spell->m_id, count);

            for (uint32_t j = 0; j < count; j++) {
                const SpellEffectRec* effect = effects->m_data[j];

                if (effect && effect->m_effect == 74 && !effect->m_effectMiscValue) {
                    CGGlyphInfo::s_clearGlyphSpell = spellId;
                }
            }
        }
    }

    CGGlyphInfo::s_numFiltered = -1;
    FrameScript_SignalEvent(SCRIPT_EVENT_USE_GLYPH, NULL);
}

// PROVEN: 917a13c2 IDENTICAL/UNPROVEN
// OFFSET: 0x009EEF50/0x140788760
bool CGGlyphInfo::IsGlyphKnown(int32_t spellId) {
    for (int32_t i = CGGlyphInfo::s_glyphs.Count() - 1; i >= 0; i--) {
        if (CGGlyphInfo::s_glyphs[i]->m_spellID == spellId) {
            return CGGlyphInfo::s_glyphs[i]->m_known;
        }
    }

    return false;
}

// PROVEN: 72736494 IDENTICAL/NONE
// OFFSET: 0x009EEF80
void CGGlyphInfo::SelectGlyph(int32_t index) {
    CGGlyphInfo::s_selectedGlyph = -1;

    if (index >= 0 && index < static_cast<int32_t>(CGGlyphInfo::s_glyphs.Count())) {
        CGGlyphInfo::s_selectedGlyph = CGGlyphInfo::s_glyphs[index]->m_spellID;
    }

    FrameScript_SignalEvent(SCRIPT_EVENT_USE_GLYPH, NULL);
}

// PROVEN: f3a848e8 IDENTICAL/IDENTICAL
// OFFSET: 0x009EEFC0/0x1407887A0
void CGGlyphInfo::ClearSelectedGlyphBySpell(int32_t spellId) {
    if (CGGlyphInfo::s_selectedGlyph == spellId) {
        CGGlyphInfo::s_selectedGlyph = -1;
        FrameScript_SignalEvent(SCRIPT_EVENT_USE_GLYPH, NULL);
    }
}

// PROVEN: 7b35961e IDENTICAL/DIFFERENT
// OFFSET: 0x009EEFF0/0x1407887D0
int32_t CGGlyphInfo::GetGlyphSpellIDByName(const char* name) {
    for (uint32_t i = 0; i < CGGlyphInfo::s_glyphs.Count(); i++) {
        GlyphItem* glyph = CGGlyphInfo::s_glyphs[i];

        if (!glyph->m_known) {
            continue;
        }

        SpellRec* spell = g_spellDB.GetRecord(glyph->m_spellID);

        if (spell && spell->m_name && !SStrCmpUTF8I(spell->m_name, name, STORM_MAX_STR)) {
            return CGGlyphInfo::s_glyphs[i]->m_spellID;
        }
    }

    return 0;
}

// OFFSET: 0x009EF080/0x1407888C0
void CGGlyphInfo::FilterGlyphs() {
    CGGlyphInfo::s_numFiltered = 0;

    for (uint32_t i = 0; i < CGGlyphInfo::s_glyphs.Count(); i++) {
        GlyphItem* glyph = CGGlyphInfo::s_glyphs[i];

        if (glyph->m_spellID == -1) {
            CGGlyphInfo::s_numFiltered++;
            CGGlyphInfo::s_glyphs[i]->m_hidden = false;
            continue;
        }

        if (((1 << glyph->m_type) & CGGlyphInfo::s_filter) && (glyph->m_known ? (CGGlyphInfo::s_filter & 8) : (CGGlyphInfo::s_filter & 0x10))) {
            CGGlyphInfo::s_numFiltered++;
            CGGlyphInfo::s_glyphs[i]->m_hidden = false;

            if (CGGlyphInfo::s_nameFilter) {
                SpellRec* spell = g_spellDB.GetRecord(CGGlyphInfo::s_glyphs[i]->m_spellID);

                if (spell) {
                    char name[1024];
                    SStrCopy(name, spell->m_name, sizeof(name));
                    CaseAccentInsensitiveParse(name, sizeof(name), s_locale);

                    if (!strstr(name, CGGlyphInfo::s_nameFilter)) {
                        CGGlyphInfo::s_numFiltered--;
                        CGGlyphInfo::s_glyphs[i]->m_hidden = true;
                    }
                }
            }
        } else {
            glyph->m_hidden = true;
        }
    }

    qsort(CGGlyphInfo::s_glyphs.m_data, CGGlyphInfo::s_glyphs.Count(), sizeof(GlyphItem*), CompareGlyphs);
}

// PROVEN: 3a6e631c IDENTICAL/IDENTICAL
// OFFSET: 0x009EF1E0/0x140788A90
void CGGlyphInfo::SetNameFilter(const char* name) {
    CGGlyphInfo::s_numFiltered = -1;

    if (CGGlyphInfo::s_nameFilter) {
        SMemFree(CGGlyphInfo::s_nameFilter, __FILE__, __LINE__, 0);
    }

    CGGlyphInfo::s_nameFilter = NULL;

    if (name && *name) {
        char filter[1024];
        SStrCopy(filter, name, sizeof(filter));
        CaseAccentInsensitiveParse(filter, sizeof(filter), s_locale);
        CGGlyphInfo::s_nameFilter = SStrDupA(filter, __FILE__, __LINE__);
    }
}

// OFFSET: 0x009EF720/0x140789120
void CGGlyphInfo::CastGlyph(uint32_t index) {
    CGGameUI::ClearCursor(1, 1);

    GlyphItem* glyph;

    if (index >= CGGlyphInfo::s_glyphs.Count() || !(glyph = CGGlyphInfo::s_glyphs[index]) || !glyph->m_known || glyph->m_spellID == -1) {
        CGGlyphInfo::s_selectedGlyph = -1;
        FrameScript_SignalEvent(SCRIPT_EVENT_USE_GLYPH, NULL);
        return;
    }

    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));
    SpellRec* spell;

    if (!player || !(spell = g_spellDB.GetRecord(glyph->m_spellID))) {
        CGGlyphInfo::s_selectedGlyph = -1;
        FrameScript_SignalEvent(SCRIPT_EVENT_USE_GLYPH, NULL);
        return;
    }

    uint32_t count;
    const TSGrowableArray<const SpellEffectRec*>* effects = SpellGetEffects(spell->m_id, count);

    for (uint32_t i = 0; i < count; i++) {
        if (!effects->m_data[i] || effects->m_data[i]->m_effect != 74) {
            continue;
        }

        PendingSpellCast* pending = Spell_C_GetActivePlayerPendingCast();

        if (!pending || pending->m_data.m_spellId != static_cast<uint32_t>(spell->m_id)) {
            uint32_t slot;

            for (slot = 0; slot < 9; slot++) {
                if (player->m_player->m_playerFieldGlyphs1[slot] == static_cast<uint32_t>(effects->m_data[i]->m_effectMiscValue)) {
                    break;
                }
            }

            if (slot >= 9) {
                continue;
            }

            Spell_C_ClickSpell(glyph->m_spellID, NULL, WOWGUID(), 0, 0);
            PendingSpellCast* targeting = Spell_C_GetTargetingSpellCast();

            if (targeting) {
                Spell_C_SpellFailed(targeting, spell, 179, -1, -1, false);
            }
        }

        CGGlyphInfo::s_selectedGlyph = -1;
        FrameScript_SignalEvent(SCRIPT_EVENT_USE_GLYPH, NULL);
        return;
    }

    CGGlyphInfo::SelectGlyph(index);
    Spell_C_ClickSpell(glyph->m_spellID, NULL, WOWGUID(), 0, 0);
}

// PROVEN: 415b7043 IDENTICAL/DIFFERENT
// OFFSET: 0x009F0080/0x140789BE0
void CGGlyphInfo::ClearGlyphs() {
    for (int32_t i = CGGlyphInfo::s_glyphs.Count() - 1; i >= 0; i--) {
        GlyphItem* glyph = CGGlyphInfo::s_glyphs[i];

        if (glyph) {
            SMemFree(glyph, "delete", -1, 0);
        }
    }

    CGGlyphInfo::s_glyphs.Clear();

    if (CGGlyphInfo::s_nameFilter) {
        SMemFree(CGGlyphInfo::s_nameFilter, __FILE__, __LINE__, 0);
    }

    CGGlyphInfo::s_nameFilter = NULL;
    CGGlyphInfo::s_numFiltered = -1;
    CGGlyphInfo::s_clearGlyphSpell = -1;
    CGGlyphInfo::s_selectedGlyph = -1;
}

// OFFSET: 0x009F0130/0x140789CC0
void CGGlyphInfo::ListGlyphs() {
    int32_t typeOrder[3] = { 1, 2, 0 };
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (!player || CGGlyphInfo::s_glyphs.Count()) {
        return;
    }

    for (int32_t i = 0; i < 3; i++) {
        GlyphItem* header = static_cast<GlyphItem*>(SMemAlloc(sizeof(GlyphItem), __FILE__, __LINE__, 0));
        header->m_spellID = -1;
        header->m_type = typeOrder[i];
        CGGlyphInfo::s_glyphs.Add(1, &header);
    }

    int32_t race = player->m_unit->m_unitFieldBytes0[0];
    int32_t classId = player->m_unit->m_unitFieldBytes0[1];

    for (int32_t i = 0; i < g_skillLineAbilityDB.m_numRecords; i++) {
        SkillLineAbilityRec* ability = g_skillLineAbilityDB.GetRecordByIndex(i);

        if (ability->m_skillLine != 810 || !SpellTableMatchRaceClass(race, classId, ability->m_raceMask, ability->m_classMask, ability->m_excludeRace, ability->m_excludeClass)) {
            continue;
        }

        SpellRec* spell = g_spellDB.GetRecord(ability->m_spell);

        if (!spell) {
            continue;
        }

        int32_t type = -1;
        uint32_t count;
        const TSGrowableArray<const SpellEffectRec*>* effects = SpellGetEffects(spell->m_id, count);

        for (uint32_t j = 0; j < count; j++) {
            const SpellEffectRec* effect = effects->m_data[j];

            if (effect && effect->m_effect == 74) {
                int32_t glyphId = effect->m_effectMiscValue;

                if (!glyphId) {
                    break;
                }

                GlyphPropertiesRec* properties = g_glyphPropertiesDB.GetRecord(glyphId);

                if (properties) {
                    type = typeOrder[properties->m_glyphSlotFlags];
                }
            }
        }

        if (type >= 0) {
            GlyphItem* glyph = static_cast<GlyphItem*>(SMemAlloc(sizeof(GlyphItem), __FILE__, __LINE__, 0));
            glyph->m_spellID = ability->m_spell;
            glyph->m_known = false;
            glyph->m_type = type;
            CGGlyphInfo::s_glyphs.Add(1, &glyph);
        }
    }

    qsort(CGGlyphInfo::s_glyphs.m_data, CGGlyphInfo::s_glyphs.Count(), sizeof(GlyphItem*), CompareGlyphs);
    CGGlyphInfo::s_numFiltered = CGGlyphInfo::s_glyphs.Count();
    CGGlyphInfo::s_filter = 0xFFFFFFFF;
}

// PROVEN: aa9a0a0b IDENTICAL/IDENTICAL
// OFFSET: 0x009F0350/0x14078A0C0
void CGGlyphInfo::InitializeGame() {
    CGGlyphInfo::ClearGlyphs();
}

// PROVEN: 63e08c9f IDENTICAL/IDENTICAL
// OFFSET: 0x009F0350/0x14078A0C0
void CGGlyphInfo::ShutdownGame() {
    CGGlyphInfo::ClearGlyphs();
}

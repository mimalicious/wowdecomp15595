#include "QuestTextParser.h"
#include "Engine/Source/ConsoleBase/ConsoleVar.h"
#include "Engine/Source/FrameScript/FrameScript.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/Storm/H/stpl.h"
#include "Engine/Source/Tempest/CMath.h"
#include "Engine/Source/lua-5.1/src/lua.h"
#include "WoW/Source/DB/DBClient/DBCacheInstances.h"
#include "WoW/Source/DB/DBClient/DBClient.h"
#include "WoW/Source/DB/Rec/AchievementRec.h"
#include "WoW/Source/DB/Rec/AreaTableRec.h"
#include "WoW/Source/DB/Rec/ItemRec.h"
#include "WoW/Source/DB/Rec/ItemRecSparse.h"
#include "WoW/Source/DB/Rec/SpellAuraOptionsRec.h"
#include "WoW/Source/DB/Rec/SpellDescriptionVariablesRec.h"
#include "WoW/Source/DB/Rec/SpellEffectRec.h"
#include "WoW/Source/DB/Rec/SpellItemEnchantmentRec.h"
#include "WoW/Source/DB/Rec/SpellLevelsRec.h"
#include "WoW/Source/DB/Rec/SpellRadiusRec.h"
#include "WoW/Source/DB/Rec/SpellRangeRec.h"
#include "WoW/Source/DB/Rec/SpellRec.h"
#include "WoW/Source/DB/Rec/SpellScalingRec.h"
#include "WoW/Source/DB/Rec/SpellTargetRestrictionsRec.h"
#include "WoW/Source/Game/GameClient/NameCache.h"
#include "WoW/Source/Game/GameTime.h"
#include "WoW/Source/Game/ValidateName.h"
#include "WoW/Source/Magic/MagicClient/Spell_C.h"
#include "WoW/Source/Object/ObjectClient/AuraData.h"
#include "WoW/Source/Object/ObjectClient/Player_C.h"
#include "WoW/Source/Object/ObjectClient/Unit_C.h"
#include "WoW/Source/Object/ObjectClient/VisibleItem_C.h"
#include "WoW/Source/Object/Player.h"
#include "WoW/Source/Object/Types.h"
#include "WoW/Source/ObjectMgrClient/ObjectMgrClient.h"
#include "WoW/Source/UIUtil/Tooltip.h"
#include "WoW/Source/Ui/CGWorldStateInfo.h"
#include "WoW/Source/Ui/GameUI.h"
#include "WoW/Source/Ui/MathState.h"
#include "WoW/Source/Ui/RaidInfo.h"
#include "WoW/Source/Ui/ScalingStats.h"
#include <cctype>
#include <cmath>
#include <cstdlib>
#include <cstring>

// STATIC: 0x00D14908
static const MathFunction s_mathFunctions[14] = {
    { "abs", 1 },
    { "ceil", 1 },
    { "floor", 1 },
    { "min", 2 },
    { "max", 2 },
    { "gt", 2 },
    { "lt", 2 },
    { "gte", 2 },
    { "lte", 2 },
    { "eq", 2 },
    { "cond", 3 },
    { "clamp", 3 },
    { "basehp", 2 },
    { "basemp", 2 },
};

// STATIC: 0x00D14978
static const char* s_mathVariables[148] = {
    "STR", "AGI", "STA", "INT", "SPI", "str", "agi", "sta", "int", "spi",
    "m1", "m2", "m3", "M1", "M2", "M3", "a1", "a2", "a3", "A1", "A2", "A3",
    "o1", "o2", "o3", "O1", "O2", "O3",
    "d", "D", "c", "C", "p", "P", "x1", "x2", "x3", "X1", "X2", "X3",
    "t1", "t2", "t3", "T1", "T2", "T3", "h", "H", "n", "N",
    "b1", "b2", "b3", "B1", "B2", "B3", "u", "U", "v", "V",
    "e1", "e2", "e3", "E1", "E2", "E3", "i", "I", "f1", "f2", "f3", "F1", "F2", "F3",
    "q1", "q2", "q3", "Q1", "Q2", "Q3", "ap", "AP", "rap", "RAP",
    "mwb", "MWB", "owb", "OWB", "rwb", "RWB", "mw", "MW", "ow", "OW", "rw", "RW",
    "ar", "AR", "mws", "MWS", "ows", "OWS", "rws", "RWS", "pl", "PL", "hnd", "HND",
    "sp", "SP", "sph", "spfi", "spn", "spfr", "sps", "spa", "SPH", "SPFI", "SPN", "SPFR", "SPS", "SPA",
    "bh", "BH", "ph", "pfi", "pn", "pfr", "ps", "pa", "PH", "PFI", "PN", "PFR", "PS", "PA",
    "pbh", "pBH", "pbhd", "pBHD", "bc1", "bc2", "bc3", "BC1", "BC2", "BC3",
    "bap", "BAP",
};

// STATIC: 0x00D14BC8/0x140BCD9F0
static const char* s_itemStatNames[57] = {
    "Mana", "Health", "Endurance", "Agility", "Strength", "Intellect", "Spirit", "Stamina", "Energy", "Rage", "Focus",
    "Weapon Rating", "Defense Rating", "Dodge Rating", "Parry Rating", "Block Rating",
    "Melee Hit Rating", "Ranged Hit Rating", "Spell Hit Rating", "Melee Crit Rating", "Ranged Crit Rating", "Spell Crit Rating",
    "Melee Hit Taken Rating", "Ranged Hit Taken Rating", "Spell Hit Taken Rating",
    "Melee Crit Taken Rating Obsolete", "Ranged Crit Taken Rating Obsolete", "Spell Crit Taken Rating Obsolete",
    "Melee Haste Rating", "Ranged Haste Rating", "Spell Haste Rating", "Hit Rating", "Crit Rating", "Hit Taken Rating", "Crit Taken Rating",
    "Resilience Rating", "Haste Rating", "Expertise Rating", "Attack Power", "Ranged Attack Power", "Unused",
    "Spell Healing", "Spell Damage", "Mana Regeneration", "Armor Penetration Rating", "Spell Power", "Health Regen", "Spell Penetration",
    "Block Value", "Mastery",
};

// STATIC: 0x00D14CAC/0x140BCDBB8
static const char* s_mathDelimiters = "()[]^*/%+-#,> ";

// STATIC: 0x00F4B2A8
static int32_t s_journalLastNumber;

// STATIC: 0x00F4B2AC
static int32_t s_lastNumber;

// STATIC: 0x00F4B2B0/0x140E3F680
static TSGrowableArray<SpellVariables> s_spellVariables;

// STATIC: 0x00F4B2C0
static MathState s_mathState;

static int32_t SpellParserParseSubText(const SpellRec* spell, char* buffer, uint32_t size, int32_t parsePet, int32_t parseUnit, int32_t spellLevel, int32_t scale, int32_t noModifiers, const char* text, const char** cursor, const AuraData* aura);

// OFFSET: 0x00992350
static uint8_t LookupMathFunction(const char* name, int32_t* argCount) {
    for (uint32_t i = 0; i < 14; i++) {
        if (!SStrCmpI(name, s_mathFunctions[i].m_name, STORM_MAX_STR)) {
            *argCount = s_mathFunctions[i].m_argCount;
            return i + MATHOP_ABS;
        }
    }

    *argCount = 0;
    return 0;
}

// OFFSET: 0x009923A0
static uint8_t LookupMathVariable(const char* name) {
    if (!name || !*name) {
        return 0;
    }

    for (uint32_t i = 0; i < 148; i++) {
        if (!SStrCmp(name, s_mathVariables[i], STORM_MAX_STR)) {
            return i + MATHOP_STAT_STRENGTH;
        }
    }

    return 0;
}

// OFFSET: 0x00992400
uint32_t MathState::AddIntArg(int32_t value) {
    uint32_t index = this->m_numIntArgs;
    this->m_intArgs[index] = value;
    this->m_numIntArgs++;
    return index;
}

// OFFSET: 0x00992420
int32_t MathState::NextToken(char* token, int32_t* pos, int32_t* length) {
    *length = 0;
    const char* text = this->m_text + *pos;

    while (*text == ' ') {
        (*length)++;
        text++;
    }

    if (!*text) {
        *pos += *length;
        return 0;
    }

    const char* delimiter = SStrChr(s_mathDelimiters, *text);

    if (delimiter) {
        token[0] = *delimiter;
        token[1] = '\0';
        (*length)++;
        *pos += *length;
        return 1;
    }

    char* out = token;

    while (*text && !SStrChr(s_mathDelimiters, *text)) {
        *out++ = *text++;
        (*length)++;
    }

    *out = '\0';
    *pos += *length;
    return 1;
}

// OFFSET: 0x009924F0/0x140711A00
int32_t MathState::Expect(const char* expected, int32_t* pos) {
    char token[32];
    int32_t length;

    int32_t result = this->NextToken(token, pos, &length);

    if (!result || !(result = SStrCmpI(token, expected, STORM_MAX_STR) == 0)) {
        this->m_status = 1;
    }

    return result;
}

// OFFSET: 0x00992540/0x140711D00
void ParseCountDownTimerText(char* buffer, uint32_t size, int32_t time) {
    int32_t remaining = time - CGWorldStateInfo::GetUITime();

    if (remaining < 0) {
        remaining = 0;
    }

    int32_t hours = remaining / 3600;
    remaining %= 3600;
    int32_t minutes = remaining / 60;
    int32_t seconds = remaining % 60;
    char text[16];

    if (hours) {
        SStrPrintf(text, sizeof(text), "%d:%02d:%02d", hours, minutes, seconds);
    } else {
        SStrPrintf(text, sizeof(text), "%02d:%02d", minutes, seconds);
    }

    SStrPack(buffer, text, size);
}

// OFFSET: 0x00992610/0x140711E10
void GenderConditional(char* buffer, uint32_t size, int32_t gender, const CGUnit_C* unit, const NameCache* name, const char** cursor) {
    while (**cursor == ' ') {
        (*cursor)++;
    }

    if (!**cursor) {
        return;
    }

    const char* colon = SStrChr(*cursor, ':');

    if (!colon) {
        return;
    }

    const char* femaleText = colon + 1;
    const char* selector = SStrChr(colon + 1, ':');
    const char* semicolon = SStrChr(colon, ';');

    if (!semicolon) {
        return;
    }

    if (selector) {
        if (selector + 1 < semicolon) {
            switch (selector[1]) {
                case 'C':
                case 'c':
                    gender = CGUnit_C::GetDisplayClassGender(unit, name);
                    break;

                case 'R':
                case 'r':
                    gender = CGUnit_C::GetDisplayRaceGender(unit, name);
                    break;
            }
        } else {
            selector = NULL;
        }
    }

    const char* end = colon;

    if (gender) {
        *cursor = femaleText;

        while (**cursor == ' ') {
            (*cursor)++;
        }

        end = selector ? selector : semicolon;
    }

    int32_t length = end - *cursor;

    if (length) {
        uint32_t bufferLength = SStrLen(buffer);
        SStrPack(buffer, *cursor, size);

        if (bufferLength + length < size) {
            char* text = &buffer[bufferLength];
            int32_t i = length - 1;
            text[length] = '\0';

            if (text[i] == ' ') {
                do {
                    text[i] = '\0';

                    if (!i) {
                        break;
                    }

                    i--;
                } while (text[i] == ' ');
            }
        }
    }

    *cursor = semicolon;
}

// OFFSET: 0x009927D0
static bool SpellParserParseNumber(const char** cursor, int32_t* value) {
    const char* text = *cursor;
    uint32_t count = 0;

    if (!*text) {
        return false;
    }

    do {
        char c = text[count];

        if (c < '0' || c > '9') {
            break;
        }

        count++;
    } while (text[count]);

    if (count >= 8 || !count) {
        return false;
    }

    char digits[8];
    SStrCopy(digits, text, count + 1);
    *cursor = text + count;
    *value = SStrToInt(digits);
    return true;
}

// OFFSET: 0x00992870/0x1407120D0
static bool SpellParserPluralConditional(char* buffer, uint32_t size, int32_t number, const char** cursor) {
    while (**cursor == ' ') {
        (*cursor)++;
    }

    if (!**cursor) {
        return true;
    }

    const char* semicolon = SStrChr(*cursor, ';');

    if (!semicolon) {
        return true;
    }

    const char* text = *cursor;
    *cursor = semicolon + 1;
    uint32_t index = QuestTextParser::FrameScript_GetPluralIndex(number);

    for (uint32_t i = 0; i < index; i++) {
        while (*text != ':' && *text != ';') {
            text++;
        }

        if (*text == ';') {
            break;
        }

        text++;
    }

    while (*text == ' ') {
        text++;
    }

    const char* end = text;

    while (*end != ':' && *end != ';') {
        end++;
    }

    int32_t length = end - text;

    if (length) {
        uint32_t bufferLength = SStrLen(buffer);
        SStrPack(buffer, text, size);

        if (bufferLength + length < size) {
            char* out = &buffer[bufferLength];
            int32_t i = length - 1;
            out[length] = '\0';

            if (out[i] == ' ') {
                do {
                    out[i] = '\0';

                    if (!i) {
                        break;
                    }

                    i--;
                } while (out[i] == ' ');
            }
        }
    }

    return true;
}

// OFFSET: 0x009929A0/0x140712270
static bool SpellParserGetCondition(const char** cursor, const AuraData* aura) {
    char type = **cursor;
    (*cursor)++;
    int32_t id = 0;

    if (!SpellParserParseNumber(cursor, &id)) {
        return false;
    }

    switch (type) {
        case 'A':
        case 'a':
            return QuestTextParser::PlayerHasAura(QuestTextParser::Spell_C_SpellGetSpellIDForDifficulty(id));

        case 'J':
        case 'j': {
            char speed = **cursor;
            (*cursor)++;
            int32_t speedType = 0;

            switch (speed) {
                case 'f':
                    speedType = 2;
                    break;

                case 'g':
                    speedType = 1;
                    break;

                case 's':
                    speedType = 3;
                    break;
            }

            return QuestTextParser::Spell_C_GetUnitMountSpeed(aura->m_effectAmounts[id - 1], speedType) > 0;
        }

        case 'P':
        case 'p':
            return QuestTextParser::PlayerHasItemPassive(QuestTextParser::Spell_C_SpellGetSpellIDForDifficulty(id));

        case 'S':
        case 's':
            return QuestTextParser::IsPlayerSpellKnown(QuestTextParser::Spell_C_SpellGetSpellIDForDifficulty(id));
    }

    return false;
}

// OFFSET: 0x00992AD0/0x1407123C0
static void SpellParserSkipParenthesis(const char** cursor, const char* end) {
    int32_t depth = 1;

    while (**cursor && *cursor < end) {
        if (**cursor == '(') {
            depth++;
        } else if (**cursor == ')' && !--depth) {
            (*cursor)++;
            return;
        }

        (*cursor)++;
    }
}

// OFFSET: 0x00992B10/0x140712010
static bool SpellParserGetCompareValue(const char** cursor, const AuraData* aura, int32_t* value) {
    uint32_t index = 0;
    bool result = false;

    if (**cursor == '$') {
        (*cursor)++;
        const char* text = *cursor;
        bool hasIndex;

        if (static_cast<uint8_t>(text[1] - '1') <= 8) {
            index = text[1] - '1';
            hasIndex = true;
        } else {
            hasIndex = false;
        }

        if ((*text == 'W' || *text == 'w') && aura) {
            *value = aura->m_effectAmounts[index];
            result = true;
        }

        (*cursor)++;

        if (hasIndex) {
            (*cursor)++;
        }

        return result;
    }

    return SpellParserParseNumber(cursor, value);
}

// OFFSET: 0x00992B80/0x140712410
static bool SpellParserCompare(const char** cursor, const AuraData* aura) {
    int32_t op = 0;
    int32_t left = 0;

    if (!SpellParserGetCompareValue(cursor, aura, &left)) {
        return false;
    }

    switch (**cursor) {
        case '<':
            (*cursor)++;

            if (**cursor == '=') {
                op = 2;
                (*cursor)++;
            } else if (**cursor == '>') {
                op = 6;
                (*cursor)++;
            } else {
                op = 1;
            }

            break;

        case '>':
            (*cursor)++;

            if (**cursor == '=') {
                op = 4;
                (*cursor)++;
            } else {
                op = 5;
            }

            break;

        case '=':
            (*cursor)++;

            if (**cursor == '=') {
                (*cursor)++;
            }

            op = 3;
            break;

        case '!':
            (*cursor)++;

            if (**cursor == '=') {
                op = 6;
                (*cursor)++;
            }

            break;

        default:
            return false;
    }

    int32_t right = 0;

    if (!SpellParserGetCompareValue(cursor, aura, &right)) {
        return false;
    }

    switch (op) {
        case 1:
            return left < right;

        case 2:
            return left <= right;

        case 3:
            return left == right;

        case 4:
            return left >= right;

        case 5:
            return left > right;

        case 6:
            return left != right;
    }

    return false;
}

// OFFSET: 0x00992CD0/0x140712650
static bool SpellParserParseConditional(const char** cursor, const char* end, const AuraData* aura) {
    bool condition = false;

    if (!**cursor) {
        return false;
    }

    while (*cursor < end) {
        char c = **cursor;

        if (c == '$' || (c >= '0' && c <= '9')) {
            condition = SpellParserCompare(cursor, aura);
        } else {
            switch (c) {
                case '!':
                    (*cursor)++;

                    while (**cursor && *cursor < end) {
                        if (SStrChr("sSaAjJpP", **cursor)) {
                            condition = !SpellParserGetCondition(cursor, aura);
                            break;
                        }

                        if (**cursor == '(') {
                            condition = !SpellParserParseConditional(cursor, end, aura);
                            SpellParserSkipParenthesis(cursor, end);
                            break;
                        }

                        (*cursor)++;
                    }

                    break;

                case '&':
                    if (!condition) {
                        return false;
                    }

                    (*cursor)++;
                    condition = SpellParserParseConditional(cursor, end, aura);
                    break;

                case '(':
                    (*cursor)++;
                    condition = SpellParserParseConditional(cursor, end, aura);
                    SpellParserSkipParenthesis(cursor, end);
                    break;

                case ')':
                    return condition;

                case 'A':
                case 'J':
                case 'P':
                case 'S':
                case 'a':
                case 'j':
                case 'p':
                case 's':
                    condition = SpellParserGetCondition(cursor, aura);
                    break;

                case '|':
                    if (condition) {
                        return true;
                    }

                    (*cursor)++;
                    condition = SpellParserParseConditional(cursor, end, aura);
                    break;

                default:
                    (*cursor)++;
                    break;
            }
        }

        if (!**cursor) {
            return condition;
        }
    }

    return condition;
}

// OFFSET: 0x00992EB0/0x140712870
static int32_t SpellParserVerifyConditional(const char* text, const char** trueStart, const char** trueEnd, const char** elseText, const char** falseStart, const char** falseEnd) {
    *trueStart = SStrChr(text, '[');

    if (!*trueStart) {
        return 0;
    }

    *trueEnd = SStrChr(*trueStart, ']');

    if (!*trueEnd) {
        return 0;
    }

    if ((*trueEnd)[1] == '?') {
        *elseText = *trueEnd + 2;

        const char* nestedStart;
        const char* nestedEnd;
        const char* nestedElse;
        return SpellParserVerifyConditional(*elseText, &nestedStart, &nestedEnd, &nestedElse, falseStart, falseEnd);
    }

    *falseStart = SStrChr(*trueEnd, '[');

    if (!*falseStart) {
        return 0;
    }

    *falseEnd = SStrChr(*falseStart, ']');
    return *falseEnd != NULL;
}

// OFFSET: 0x009931A0
static int32_t EnchantmentParserReplaceText(char* buffer, uint32_t size, int32_t value, const char** cursor) {
    if (**cursor != 'i') {
        return 0;
    }

    char number[32];
    SStrPrintf(number, sizeof(number), "%d", value);
    SStrPack(buffer, number, size);

    if (**cursor) {
        (*cursor)++;
    }

    return 1;
}

// OFFSET: 0x00993230/0x140714020
int32_t EnchantmentParserParseText(const SpellItemEnchantmentRec* enchant, char* buffer, uint32_t size, int32_t value, int32_t itemLevel, const ItemRecSparse* item) {
    int32_t failed = 0;
    *buffer = '\0';
    const char* text = enchant->m_name;
    const char* dollar = SStrChr(text, '$');

    if (enchant->m_scalingItemLevel == 1 || itemLevel <= 0) {
        while (dollar && *dollar) {
            int32_t length = dollar - text;

            if (dollar != text) {
                uint32_t bufferLength = SStrLen(buffer);
                SStrPack(buffer, text, size);

                if (length + bufferLength < size) {
                    buffer[length + bufferLength] = '\0';
                }
            }

            const char* cursor = dollar + 1;

            if (!EnchantmentParserReplaceText(buffer, size, value, &cursor)) {
                SStrPack(buffer, "$", size);
                failed = 1;
            }

            text = cursor;
            dollar = SStrChr(cursor, '$');
        }

        SStrPack(buffer, text, size);
        return failed == 0;
    }

    bool first = true;

    for (uint32_t i = 0; i < 3; i++) {
        int32_t points;

        if (GetScaledEnchantmentPoints(itemLevel, item->m_itemLevel, enchant, enchant->m_effectPointsMin[i]) <= 1) {
            points = 1;
        } else {
            points = GetScaledEnchantmentPoints(itemLevel, item->m_itemLevel, enchant, enchant->m_effectPointsMin[i]);
        }

        if (enchant->m_effectPointsMin[i] && points != enchant->m_effectPointsMin[i] && enchant->m_effect[i] == 5) {
            if (first) {
                SStrPrintf(buffer, size, "%s%d %s", points < 0 ? "-" : "+", points, s_itemStatNames[enchant->m_effectArg[i]]);
            } else {
                SStrPrintf(buffer + SStrLen(buffer), size, ", %s%d %s", points < 0 ? "-" : "+", points, s_itemStatNames[enchant->m_effectArg[i]]);
            }

            first = false;
        }
    }

    if (!first) {
        SStrPrintf(buffer + SStrLen(buffer), size, " (was %s)", text);
        return 1;
    }

    SStrPack(buffer, text, size);
    return 1;
}

// OFFSET: 0x009935F0/0x140712CE0
static float GetSpellVariableValue(const char* name) {
    if (!name || !*name) {
        return 0.0f;
    }

    for (uint32_t i = 0; i < s_spellVariables.m_count; i++) {
        if (!SStrCmp(s_spellVariables.m_data[i].m_name, name, STORM_MAX_STR)) {
            return s_spellVariables.m_data[i].m_value;
        }
    }

    return 0.0f;
}

// OFFSET: 0x00993680/0x140712D60
int32_t MathState::CompilePrimary(int32_t pos) {
    char token[32];
    char separator[32];
    char digits[8];
    int32_t length;

    if (!this->NextToken(token, &pos, &length)) {
        return pos;
    }

    char first = token[0];

    if (!token[1]) {
        if (first == '(') {
            pos = this->CompileSum(pos);
            this->Expect(")", &pos);
            return pos;
        }

        if (first == '-') {
            int32_t result = this->CompilePrimary(pos);
            this->m_ops[this->m_numOps] = MATHOP_NEGATE;
            this->m_numOps++;
            return result;
        }
    }

    if (first == '$') {
        const char* name = &token[1];

        if (token[1] == '<') {
            this->m_variables[this->m_numVariables] = GetSpellVariableValue(&token[2]);
            this->m_numVariables++;
            this->m_ops[this->m_numOps] = MATHOP_SPELL_VARIABLE;
            this->m_numOps++;
            this->Expect(">", &pos);
            return pos;
        }

        int32_t argCount;
        uint8_t function = LookupMathFunction(name, &argCount);

        if (function) {
            this->Expect("(", &pos);
            pos = this->CompileSum(pos);

            for (int32_t i = argCount - 1; i; i--) {
                if (!this->NextToken(separator, &pos, &length) || SStrCmpI(separator, ",", STORM_MAX_STR)) {
                    this->m_status = 1;
                }

                pos = this->CompileSum(pos);
            }

            this->m_ops[this->m_numOps] = function;
            this->m_numOps++;
            this->Expect(")", &pos);
            return pos;
        }

        if (isdigit(token[1]) && token[1]) {
            int32_t count = 0;

            while (token[1 + count] >= '0' && token[1 + count] <= '9') {
                digits[count] = token[1 + count];
                count++;
            }

            if (count) {
                digits[count] = '\0';
                this->AddIntArg(SStrToInt(digits));
                this->m_ops[this->m_numOps] = MATHOP_SPELL;
                this->m_numOps++;
                name = &token[1 + count];
            }
        }

        uint8_t variable = LookupMathVariable(name);

        if (variable) {
            this->m_ops[this->m_numOps] = variable;
            this->m_numOps++;
            return pos;
        }
    }

    if (!isdigit(first) && first != '.') {
        this->m_status = 1;
        return pos;
    }

    this->m_constants[this->m_numConstants] = static_cast<float>(atof(token));
    this->m_numConstants++;
    this->m_ops[this->m_numOps] = MATHOP_CONSTANT;
    this->m_numOps++;
    return pos;
}

// OFFSET: 0x00993900
int32_t MathState::CompilePower(int32_t pos) {
    pos = this->CompilePrimary(pos);

    while (this->m_text[pos] == '^') {
        pos = this->CompilePrimary(pos + 1);
        this->m_ops[this->m_numOps] = MATHOP_POW;
        this->m_numOps++;
    }

    return pos;
}

// OFFSET: 0x00993950/0x140713110
int32_t MathState::CompileProduct(int32_t pos) {
    pos = this->CompilePower(pos);

    while (true) {
        char op = this->m_text[pos];

        if (op != '*' && op != '/' && op != '%') {
            break;
        }

        pos = this->CompilePrimary(pos + 1);

        while (this->m_text[pos] == '^') {
            pos = this->CompilePrimary(pos + 1);
            this->m_ops[this->m_numOps] = MATHOP_POW;
            this->m_numOps++;
        }

        if (op == '%') {
            this->m_ops[this->m_numOps] = MATHOP_MOD;
        } else if (op == '*') {
            this->m_ops[this->m_numOps] = MATHOP_MUL;
        } else {
            this->m_ops[this->m_numOps] = MATHOP_DIV;
        }

        this->m_numOps++;
    }

    return pos;
}

// OFFSET: 0x00993A00/0x140713220
int32_t MathState::CompileSum(int32_t pos) {
    pos = this->CompileProduct(pos);

    while (true) {
        char op = this->m_text[pos];

        if (op != '+' && op != '-') {
            break;
        }

        pos = this->CompileProduct(pos + 1);
        this->m_ops[this->m_numOps] = (op != '+') + MATHOP_ADD;
        this->m_numOps++;
    }

    return pos;
}

// OFFSET: 0x00993A50/0x140713290
void MathState::ExecuteFunction(uint32_t op, MathStack* stack) {
    float args[3];
    float result = 0.0f;

    for (int32_t i = s_mathFunctions[op - MATHOP_ABS].m_argCount; i; stack->m_top++) {
        i--;
        args[i] = stack->m_values[stack->m_top];
    }

    switch (op) {
        case MATHOP_ABS:
            result = fabsf(args[0]);
            break;

        case MATHOP_CEIL:
            result = ceilf(args[0]);
            break;

        case MATHOP_FLOOR:
            result = static_cast<float>(floor(args[0]));
            break;

        case MATHOP_MIN:
            result = args[1];

            if (args[1] > args[0]) {
                result = args[0];
            }

            break;

        case MATHOP_MAX:
            result = args[0] > args[1] ? args[0] : args[1];
            break;

        case MATHOP_GT:
            if (args[0] > args[1]) {
                result = 1.0f;
            }

            break;

        case MATHOP_LT:
            if (args[1] > args[0]) {
                result = 1.0f;
            }

            break;

        case MATHOP_GTE:
            if (args[0] >= args[1]) {
                result = 1.0f;
            }

            break;

        case MATHOP_LTE:
            if (args[1] >= args[0]) {
                result = 1.0f;
            }

            break;

        case MATHOP_EQ:
            if (fabsf(args[0] - args[1]) < 0.0001f) {
                result = 1.0f;
            }

            break;

        case MATHOP_COND:
            if (args[0] != 0.0f) {
                result = args[1];
            } else {
                result = args[2];
            }

            break;

        case MATHOP_CLAMP: {
            float upper = args[1] > args[0] ? args[1] : args[0];
            result = args[0];

            if (args[2] > upper) {
                if (args[1] > args[0]) {
                    result = args[1];
                }
            } else {
                result = args[2];
            }

            break;
        }

        case MATHOP_BASE_HP: {
            int32_t classID = static_cast<int32_t>(args[0]);
            int32_t level = static_cast<int32_t>(args[1]);

            if (static_cast<uint32_t>(classID - 1) <= 10 && static_cast<uint32_t>(level - 1) <= 99) {
                result = GameFormulaEvaluateTable(7, level - 1, classID - 1);
            }

            break;
        }

        case MATHOP_BASE_MP: {
            int32_t classID = static_cast<int32_t>(args[0]);
            int32_t level = static_cast<int32_t>(args[1]);

            if (static_cast<uint32_t>(classID - 1) <= 10 && static_cast<uint32_t>(level - 1) <= 99) {
                result = GameFormulaEvaluateTable(8, level - 1, classID - 1);
            }

            break;
        }

        default:
            this->m_status = 1;
            break;
    }

    stack->m_top--;
    stack->m_values[stack->m_top] = result;
}

// OFFSET: 0x00993CC0/0x140711B20
void MathState::ExecuteOperator(uint32_t op, MathStack* stack) {
    float result;

    switch (op) {
        case MATHOP_POW: {
            float exponent = stack->m_values[stack->m_top++];
            float base = stack->m_values[stack->m_top++];
            result = static_cast<float>(pow(base, exponent));
            break;
        }

        case MATHOP_NEGATE: {
            float operand = stack->m_values[stack->m_top];
            stack->m_top++;
            result = -operand;
            break;
        }

        case MATHOP_MUL: {
            float right = stack->m_values[stack->m_top++];
            float left = stack->m_values[stack->m_top++];
            result = left * right;
            break;
        }

        case MATHOP_DIV: {
            float right = stack->m_values[stack->m_top++];
            float left = stack->m_values[stack->m_top++];
            result = left / right;
            break;
        }

        case MATHOP_MOD: {
            float right = stack->m_values[stack->m_top++];
            float left = stack->m_values[stack->m_top++];
            float quotient = left / right;
            result = static_cast<float>(left - (quotient - floor(quotient)) * right);
            break;
        }

        case MATHOP_ADD: {
            float right = stack->m_values[stack->m_top++];
            float left = stack->m_values[stack->m_top++];
            result = left + right;
            break;
        }

        case MATHOP_SUB: {
            float right = stack->m_values[stack->m_top++];
            float left = stack->m_values[stack->m_top++];
            result = left - right;
            break;
        }

        default:
            result = 0.0f;
            this->m_status = 1;
            break;
    }

    stack->m_top--;
    stack->m_values[stack->m_top] = result;
}

// OFFSET: 0x00993E60/0x140713590
static int32_t SpellParserVariable(char* buffer, uint32_t size, const char** cursor, const char* end, int32_t* lastNumber) {
    char name[16];
    int32_t length = end - *cursor;

    if (length > 15) {
        length = 15;
    }

    SStrCopy(name, *cursor + 1, length);

    for (uint32_t i = 0; i < s_spellVariables.m_count; i++) {
        if (!SStrCmp(s_spellVariables.m_data[i].m_name, name, STORM_MAX_STR)) {
            SStrPack(buffer, s_spellVariables.m_data[i].m_text, size);
            *lastNumber = s_spellVariables.m_data[i].m_intValue;
            *cursor = end + 1;
            return 1;
        }
    }

    return 0;
}

// OFFSET: 0x00993F80/0x140713700
int32_t ChatParserParseText(const char* text, char* buffer, uint32_t size) {
    if (!text) {
        return 0;
    }

    int32_t failed = 0;
    *buffer = '\0';
    const char* percent = SStrChr(text, '%');

    while (percent && *percent) {
        int32_t length = percent - text;

        if (percent != text) {
            uint32_t bufferLength = SStrLen(buffer);
            SStrPack(buffer, text, size);

            if (bufferLength + length < size) {
                buffer[bufferLength + length] = '\0';
            }
        }

        const char* cursor = percent + 1;
        char marked[256];

        switch (*cursor) {
            case 'F':
            case 'f': {
                WOWGUID guid = CGGameUI::m_focusTarget;
                const char* name = NULL;

                if (guid) {
                    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));

                    if (unit) {
                        name = unit->GetUnitName(NULL, 1);
                    } else {
                        WOWGUID requester = { 0 };
                        NameCache* record = g_nameDBCache.GetRecord(guid.m_guid, requester, NULL, NULL, false);

                        if (record) {
                            name = record->m_name;
                        }
                    }
                }

                if (!name) {
                    SStrPack(buffer, FrameScript_GetText("FOCUS_TOKEN_NOT_FOUND", -1, GENDER_NOT_APPLICABLE), size);
                } else {
                    uint32_t index = GetRaidTargetIndexFromGuid(guid);

                    if (index >= 28) {
                        SStrPack(buffer, name, size);
                    } else {
                        SStrPrintf(marked, sizeof(marked), "{rt%d}%s", index + 1, name);
                        SStrPack(buffer, marked, size);
                    }
                }

                cursor++;
                break;
            }

            case 'N':
            case 'T':
            case 'n':
            case 't': {
                WOWGUID guid = CGGameUI::m_lockedTarget;
                const char* name = NULL;

                if (guid) {
                    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));

                    if (unit) {
                        name = unit->GetUnitName(NULL, 1);
                    } else {
                        WOWGUID requester = { 0 };
                        NameCache* record = g_nameDBCache.GetRecord(guid.m_guid, requester, NULL, NULL, false);

                        if (record) {
                            name = record->m_name;
                        }
                    }
                }

                if (!name) {
                    SStrPack(buffer, FrameScript_GetText("TARGET_TOKEN_NOT_FOUND", -1, GENDER_NOT_APPLICABLE), size);
                } else {
                    uint32_t index = GetRaidTargetIndexFromGuid(guid);

                    if (index >= 28) {
                        SStrPack(buffer, name, size);
                    } else {
                        SStrPrintf(marked, sizeof(marked), "{rt%d}%s", index + 1, name);
                        SStrPack(buffer, marked, size);
                    }
                }

                cursor++;
                break;
            }

            default:
                failed = 1;
                SStrPack(buffer, "%", size);
                break;
        }

        text = cursor;
        percent = SStrChr(cursor, '%');
    }

    SStrPack(buffer, text, size);
    return failed == 0;
}

// OFFSET: 0x00994730/0x140714460
bool QuestParserParseToken(char* buffer, uint32_t size, const CGUnit_C* unit, const NameCache* record, const char* name, int32_t achievement, const char** cursor, WOWGUID guid) {
    char number[8];
    uint32_t digits = 0;

    if (**cursor) {
        do {
            char c = (*cursor)[digits];

            if (c < '0' || c > '9') {
                break;
            }

            digits++;
        } while ((*cursor)[digits]);

        if (digits) {
            SStrCopy(number, *cursor, digits + 1);
            *cursor += digits;
        }
    }

    char token = **cursor;
    char text[64];
    char value[16];
    char label[32];
    char upper[48];

    switch (token) {
        case 'A':
        case 'a':
            if (g_achievementDB.GetRecord(achievement)) {
                SStrPack(buffer, CGTooltip::GetAchievementHyperlink(achievement, guid, 1, g_clientGameTime, -1, -1, -1, -1), size);
            }

            break;

        case 'B':
        case 'b':
            SStrPack(buffer, "\n", size);
            break;

        case 'C':
        case 'c': {
            const char* className = CGUnit_C::GetDisplayClassName(unit, record);

            if (!className) {
                return false;
            }

            if (unit && !(unit->m_obj->m_type & TYPEMASK_PLAYER)) {
                SStrPack(buffer, className, size);
                break;
            }

            SStrCopy(label, className, sizeof(label));

            if (**cursor == 'c') {
                SStrLower(label);
            }

            SStrPack(buffer, label, size);
            break;
        }

        case 'D':
        case 'd': {
            double minutes = GetWorldState(-SStrToInt(number));
            text[0] = 0;

            if (minutes >= 1440.0) {
                SStrPrintf(text, sizeof(text), FrameScript_GetText("INT_GENERAL_DURATION_DAYS", -1, GENDER_NOT_APPLICABLE), static_cast<int32_t>(static_cast<int64_t>((minutes - 1.0) * 0.0006944444444444445 + 1.0)));
            } else if (minutes >= 60.0) {
                bool roundUp = fmod(minutes, 60.0) >= 30.0;
                SStrPrintf(text, sizeof(text), FrameScript_GetText("INT_GENERAL_DURATION_HOURS", -1, GENDER_NOT_APPLICABLE), static_cast<int32_t>(static_cast<int64_t>(roundUp + minutes * 0.016666666666666666)));
            } else {
                SStrPrintf(text, sizeof(text), FrameScript_GetText("INT_GENERAL_DURATION_MIN", -1, GENDER_NOT_APPLICABLE), static_cast<int32_t>(static_cast<int64_t>(minutes)));
            }

            SStrPack(buffer, text, size);
            break;
        }

        case 'E':
        case 'e':
            SStrPrintf(value, sizeof(value), "%d", GetWorldState(-SStrToInt(number)));
            SStrPack(buffer, value, size);
            break;

        case 'G':
        case 'g': {
            if (!unit && !record) {
                return false;
            }

            if (token) {
                (*cursor)++;
            }

            int32_t gender = unit ? unit->m_unit->m_unitFieldBytes0[2] : record->m_sex;

            if (gender == 2 && unit && const_cast<CGUnit_C*>(unit)->IsActivePlayer()) {
                gender = static_cast<const CGPlayer_C*>(unit)->m_player->m_playerBytes3[0];
            }

            GenderConditional(buffer, size, gender, unit, record, cursor);
            break;
        }

        case 'K':
        case 'k':
            ParseCountDownTimerText(buffer, size, GetWorldState(SStrToInt(number)));
            break;

        case 'N':
        case 'n': {
            const char* unitName = name;

            if (unit || record) {
                if (!name) {
                    unitName = unit ? const_cast<CGUnit_C*>(unit)->GetUnitName(NULL, 1) : record->m_name;
                }
            } else if (!name) {
                return false;
            }

            SStrPack(buffer, unitName, size);
            break;
        }

        case 'P':
        case 'p': {
            CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

            if (player) {
                if (**cursor == 'P') {
                    UpperCaseString(upper, player->GetUnitName(NULL, 1), sizeof(upper));
                    SStrPack(buffer, upper, size);
                } else {
                    SStrPack(buffer, player->GetUnitName(NULL, 1), size);
                }
            }

            break;
        }

        case 'R':
        case 'r': {
            const char* raceName = CGUnit_C::GetDisplayRaceName(unit, record);

            if (!raceName) {
                return false;
            }

            if (unit && !(unit->m_obj->m_type & TYPEMASK_PLAYER)) {
                SStrPack(buffer, raceName, size);
                break;
            }

            SStrCopy(label, raceName, sizeof(label));

            if (**cursor == 'r') {
                SStrLower(label);
            }

            SStrPack(buffer, label, size);
            break;
        }

        case 'T':
        case 't': {
            if (!unit && !record) {
                return false;
            }

            int32_t lower = token == 't';
            (*cursor)++;

            while (**cursor == ' ') {
                (*cursor)++;
            }

            if (!**cursor) {
                return true;
            }

            const char* colon = SStrChr(*cursor, ':');

            if (!colon) {
                return true;
            }

            const char* semicolon = SStrChr(colon, ';');

            if (!semicolon) {
                return true;
            }

            text[0] = 0;
            int32_t gender;

            if (unit) {
                if (unit->m_obj->m_type & TYPEMASK_PLAYER) {
                    const CGPlayer_C* player = static_cast<const CGPlayer_C*>(unit);
                    uint8_t rank = player->m_player->m_playerFieldBytes[3];
                    SStrPrintf(label, sizeof(label), "PVP_RANK_%d_%d", rank, player->GetPVPFactionIndex());
                    const char* rankName = FrameScript_GetText(label, -1, GENDER_NOT_APPLICABLE);

                    if (rankName && *rankName) {
                        SStrCopy(text, rankName, sizeof(text));
                    }

                    if (text[0]) {
                        if (lower) {
                            SStrLower(text);
                        }

                        SStrPack(buffer, text, size);
                        *cursor = semicolon;
                        break;
                    }
                }

                gender = unit->m_unit->m_unitFieldBytes0[2];
            } else {
                gender = record->m_sex;
            }

            const char* end = colon;

            if (gender) {
                *cursor = colon + 1;

                while (**cursor == ' ') {
                    (*cursor)++;
                }

                end = semicolon;
            }

            uint32_t length = static_cast<uint32_t>(end - *cursor);

            if (length) {
                uint32_t start = static_cast<uint32_t>(SStrLen(buffer));
                SStrPack(buffer, *cursor, size);

                if (start + length < size) {
                    buffer[start + length] = 0;
                    char* segment = buffer + start;
                    uint32_t i = length - 1;

                    if (segment[i] == ' ') {
                        do {
                            segment[i] = 0;

                            if (!i) {
                                break;
                            }

                            i--;
                        } while (segment[i] == ' ');
                    }
                }
            }

            *cursor = semicolon;
            break;
        }

        case 'W':
        case 'w':
            SStrPrintf(value, sizeof(value), "%d", GetWorldState(SStrToInt(number)));
            SStrPack(buffer, value, size);
            break;

        default:
            return false;
    }

    if (**cursor) {
        (*cursor)++;
    }

    return true;
}

// OFFSET: 0x009951C0/0x140715180
int32_t QuestParserParseText(const char* text, char* buffer, uint32_t size, const WOWGUID& guid, const char* name, int32_t achievement) {
    bool failed = false;
    CGUnit_C* unit = static_cast<CGUnit_C*>(ClntObjMgrObjectPtr(guid, TYPEMASK_UNIT, __FILE__, __LINE__));
    NameCache* record;

    if (unit || (guid.m_guid & 0xF000000000000000ull) || !(guid.m_guid & 0xF07FFFFFFFFFFFFFull)) {
        record = NULL;
    } else {
        WOWGUID requester = { 0 };
        record = g_nameDBCache.GetRecord(guid.m_guid, requester, NULL, NULL, false);
    }

    *buffer = 0;
    const char* cursor = text;

    for (const char* token = SStrChr(text, '$'); token && *token; token = SStrChr(cursor, '$')) {
        uint32_t length = static_cast<uint32_t>(token - cursor);

        if (length) {
            uint32_t end = static_cast<uint32_t>(SStrLen(buffer));
            SStrPack(buffer, cursor, size);

            if (end + length < size) {
                buffer[end + length] = 0;
            }
        }

        cursor = token + 1;

        if (!QuestParserParseToken(buffer, size, unit, record, name, achievement, &cursor, guid)) {
            SStrPack(buffer, "$", size);
            failed = true;
        }
    }

    SStrPack(buffer, cursor, size);
    return !failed;
}

// OFFSET: 0x009953C0/0x140715410
void MathState::GetVariableValue(uint32_t op, MathStack* stack, const SpellRec* spell, int32_t spellLevel, int32_t scale, int32_t parseUnit, int32_t parsePet, int32_t noModifiers) {
    float value = 0.0f;
    uint32_t effectCount = QuestTextParser::SpellGetNumEffects(spell->m_id);
    CGPlayer_C* player = static_cast<CGPlayer_C*>(ClntObjMgrObjectPtr(ClntObjMgrGetActivePlayer(), TYPEMASK_PLAYER, __FILE__, __LINE__));

    if (player) {
        const SpellAuraOptionsRec* auraOptions = QuestTextParser::GetSpellAuraOptionsRecord(spell->m_spellAuraOptionsID);

        switch (op) {
            case MATHOP_STAT_STRENGTH:
            case MATHOP_STAT_AGILITY:
            case MATHOP_STAT_STAMINA:
            case MATHOP_STAT_INTELLECT:
            case MATHOP_STAT_SPIRIT:
                value = static_cast<float>(player->GetEffectiveStat(op - MATHOP_STAT_STRENGTH));
                break;

            case MATHOP_BASE_STAT_STRENGTH:
            case MATHOP_BASE_STAT_AGILITY:
            case MATHOP_BASE_STAT_STAMINA:
            case MATHOP_BASE_STAT_INTELLECT:
            case MATHOP_BASE_STAT_SPIRIT:
                value = static_cast<float>(player->m_unit->m_unitFieldStat[op - MATHOP_BASE_STAT_STRENGTH]) - static_cast<float>(player->m_unit->m_unitFieldPosstat[op - MATHOP_BASE_STAT_STRENGTH]) - static_cast<float>(player->m_unit->m_unitFieldNegstat[op - MATHOP_BASE_STAT_STRENGTH]);
                break;

            case MATHOP_MIN_POINTS_1:
            case MATHOP_MIN_POINTS_2:
            case MATHOP_MIN_POINTS_3: {
                if (op - MATHOP_MIN_POINTS_1 >= effectCount) {
                    break;
                }

                float minPoints;
                float maxPoints;
                QuestTextParser::Spell_C_GetMinMaxPoints(spell, op - MATHOP_MIN_POINTS_1, &minPoints, &maxPoints, spellLevel, parsePet, parseUnit, noModifiers);
                value = static_cast<float>(scale) * minPoints;
                break;
            }

            case MATHOP_MAX_POINTS_1:
            case MATHOP_MAX_POINTS_2:
            case MATHOP_MAX_POINTS_3: {
                if (op - MATHOP_MAX_POINTS_1 >= effectCount) {
                    break;
                }

                float minPoints;
                float maxPoints;
                QuestTextParser::Spell_C_GetMinMaxPoints(spell, op - MATHOP_MAX_POINTS_1, &minPoints, &maxPoints, spellLevel, parsePet, parseUnit, noModifiers);
                value = static_cast<float>(scale) * maxPoints;
                break;
            }

            case MATHOP_RADIUS_1:
            case MATHOP_RADIUS_2:
            case MATHOP_RADIUS_3:
            case MATHOP_RADIUS_1_UPPER:
            case MATHOP_RADIUS_2_UPPER:
            case MATHOP_RADIUS_3_UPPER: {
                float radius = 0.0f;
                uint32_t index = op < MATHOP_RADIUS_1_UPPER ? op - MATHOP_RADIUS_1 : op - MATHOP_RADIUS_1_UPPER;

                if (index >= effectCount) {
                    break;
                }

                const SpellEffectRec* effect = QuestTextParser::GetSpellEffectRecord(spell->m_id, index);

                if (!effect) {
                    break;
                }

                int32_t radiusIndex = effect->m_effectRadiusIndex;

                if (op >= MATHOP_RADIUS_1_UPPER && effect->m_effectRadiusMaxIndex) {
                    radiusIndex = effect->m_effectRadiusMaxIndex;
                }

                const SpellRadiusRec* rec = QuestTextParser::GetSpellRadiusRecord(radiusIndex);

                if (!rec) {
                    break;
                }

                radius = rec->m_radius;
                QuestTextParser::Spell_C_ModifySpellValue(spell, radius, 6);
                value = radius;
                break;
            }

            case MATHOP_PERIODIC_TOTAL_1:
            case MATHOP_PERIODIC_TOTAL_2:
            case MATHOP_PERIODIC_TOTAL_3:
            case MATHOP_PERIODIC_TOTAL_1_UPPER:
            case MATHOP_PERIODIC_TOTAL_2_UPPER:
            case MATHOP_PERIODIC_TOTAL_3_UPPER: {
                uint32_t index = op < MATHOP_PERIODIC_TOTAL_1_UPPER ? op - MATHOP_PERIODIC_TOTAL_1 : op - MATHOP_PERIODIC_TOTAL_1_UPPER;
                float minPoints;
                float maxPoints;
                QuestTextParser::Spell_C_GetMinMaxPoints(spell, index, &minPoints, &maxPoints, spellLevel, parsePet, parseUnit, noModifiers);
                minPoints = static_cast<float>(scale) * minPoints;
                maxPoints = static_cast<float>(scale) * maxPoints;
                int32_t period = QuestTextParser::Spell_C_GetAuraPeriod(spell, index);
                int32_t time;

                if (period <= 0 || (time = QuestTextParser::Spell_C_GetDuration(spell, parsePet, parseUnit, noModifiers, 0)) <= 0) {
                    value = (0.0f + 0.0f) * 0.5f;
                } else {
                    int32_t ticks = static_cast<int32_t>(static_cast<float>(time) / static_cast<float>(period));

                    if (spell->m_attributesExE & 0x200) {
                        ticks++;
                    }

                    value = (static_cast<float>(ticks) * minPoints + static_cast<float>(ticks) * maxPoints) * 0.5f;
                }

                break;
            }

            case MATHOP_DURATION:
            case MATHOP_DURATION_UPPER:
                value = static_cast<float>(QuestTextParser::Spell_C_GetDuration(spell, parsePet, parseUnit, noModifiers, 1)) * 0.001f;
                break;

            case MATHOP_POWER_COST:
            case MATHOP_POWER_COST_UPPER:
                value = static_cast<float>(Spell_C_GetPowerCost(spell, Spell_C_GetStatTargetUnit(parsePet, parseUnit)));
                break;

            case MATHOP_POWER_COST_PER_SECOND:
            case MATHOP_POWER_COST_PER_SECOND_UPPER:
                value = static_cast<float>(Spell_C_GetPowerCostPerSecond(spell, Spell_C_GetStatTargetUnit(parsePet, parseUnit)));
                break;

            case MATHOP_CHAIN_TARGETS_1:
            case MATHOP_CHAIN_TARGETS_2:
            case MATHOP_CHAIN_TARGETS_3:
            case MATHOP_CHAIN_TARGETS_1_UPPER:
            case MATHOP_CHAIN_TARGETS_2_UPPER:
            case MATHOP_CHAIN_TARGETS_3_UPPER: {
                int32_t targets = 0;
                uint32_t index = op < MATHOP_CHAIN_TARGETS_1_UPPER ? op - MATHOP_CHAIN_TARGETS_1 : op - MATHOP_CHAIN_TARGETS_1_UPPER;

                if (index < effectCount) {
                    const SpellEffectRec* effect = QuestTextParser::GetSpellEffectRecord(spell->m_id, index);

                    if (effect) {
                        targets = effect->m_effectChainTargets;
                    }

                    QuestTextParser::Spell_C_ModifySpellValue(spell, targets, 17);
                }

                value = static_cast<float>(targets);
                break;
            }

            case MATHOP_AURA_PERIOD_1:
            case MATHOP_AURA_PERIOD_2:
            case MATHOP_AURA_PERIOD_3:
            case MATHOP_AURA_PERIOD_1_UPPER:
            case MATHOP_AURA_PERIOD_2_UPPER:
            case MATHOP_AURA_PERIOD_3_UPPER: {
                int32_t period = 0;

                if ((auraOptions ? auraOptions->m_procTypeMask : 0) & 1) {
                    value = static_cast<float>(5200);
                } else {
                    uint32_t index = op < MATHOP_AURA_PERIOD_1_UPPER ? op - MATHOP_AURA_PERIOD_1 : op - MATHOP_AURA_PERIOD_1_UPPER;

                    if (index < effectCount) {
                        const SpellEffectRec* effect = QuestTextParser::GetSpellEffectRecord(spell->m_id, index);

                        if (effect) {
                            period = effect->m_effectAuraPeriod;
                        }

                        QuestTextParser::Spell_C_ModifySpellValue(spell, period, 19);
                    }

                    value = static_cast<float>(period);
                }

                break;
            }

            case MATHOP_PROC_CHANCE:
            case MATHOP_PROC_CHANCE_UPPER: {
                int32_t chance = auraOptions ? auraOptions->m_procChance : 101;
                QuestTextParser::Spell_C_ModifySpellValue(spell, chance, 18);
                value = static_cast<float>(chance);
                break;
            }

            case MATHOP_PROC_CHARGES:
            case MATHOP_PROC_CHARGES_UPPER: {
                int32_t charges = auraOptions ? auraOptions->m_procCharges : 0;
                QuestTextParser::Spell_C_ModifySpellValue(spell, charges, 4);
                value = static_cast<float>(charges);
                break;
            }

            case MATHOP_POINTS_PER_COMBO_1:
            case MATHOP_POINTS_PER_COMBO_2:
            case MATHOP_POINTS_PER_COMBO_3:
            case MATHOP_POINTS_PER_COMBO_1_UPPER:
            case MATHOP_POINTS_PER_COMBO_2_UPPER:
            case MATHOP_POINTS_PER_COMBO_3_UPPER: {
                uint32_t index = op < MATHOP_POINTS_PER_COMBO_1_UPPER ? op - MATHOP_POINTS_PER_COMBO_1 : op - MATHOP_POINTS_PER_COMBO_1_UPPER;

                if (index < effectCount) {
                    value = QuestTextParser::Spell_C_GetPointsPerCombo(QuestTextParser::Spell_C_GetStatTargetUnit(parsePet, parseUnit), spell, index);
                }

                break;
            }

            case MATHOP_CUMULATIVE_AURA:
            case MATHOP_CUMULATIVE_AURA_UPPER:
                if (auraOptions) {
                    value = static_cast<float>(auraOptions->m_cumulativeAura);
                }

                break;

            case MATHOP_MAX_TARGET_LEVEL:
            case MATHOP_MAX_TARGET_LEVEL_UPPER: {
                int32_t level = 0;
                const SpellTargetRestrictionsRec* restrictions = QuestTextParser::GetSpellTargetRestrictionsRecord(spell->m_spellTargetRestrictionsID);

                if (restrictions) {
                    level = restrictions->m_maxTargetLevel;
                }

                value = static_cast<float>(level);
                break;
            }

            case MATHOP_AMPLITUDE_1:
            case MATHOP_AMPLITUDE_2:
            case MATHOP_AMPLITUDE_3:
            case MATHOP_AMPLITUDE_1_UPPER:
            case MATHOP_AMPLITUDE_2_UPPER:
            case MATHOP_AMPLITUDE_3_UPPER: {
                float amplitude = 0.0f;
                uint32_t index = op < MATHOP_AMPLITUDE_1_UPPER ? op - MATHOP_AMPLITUDE_1 : op - MATHOP_AMPLITUDE_1_UPPER;

                if (index >= effectCount) {
                    break;
                }

                const SpellEffectRec* effect = QuestTextParser::GetSpellEffectRecord(spell->m_id, index);

                if (effect) {
                    amplitude = effect->m_effectValueMultiplier;
                }

                QuestTextParser::Spell_C_ModifySpellValue(spell, amplitude, 27);
                value = amplitude;
                break;
            }

            case MATHOP_MAX_TARGETS:
            case MATHOP_MAX_TARGETS_UPPER: {
                int32_t targets = 0;
                const SpellTargetRestrictionsRec* restrictions = QuestTextParser::GetSpellTargetRestrictionsRecord(spell->m_spellTargetRestrictionsID);

                if (restrictions) {
                    targets = restrictions->m_maxAffectedTargets;
                }

                value = static_cast<float>(targets);
                break;
            }

            case MATHOP_CHAIN_AMPLITUDE_1:
            case MATHOP_CHAIN_AMPLITUDE_2:
            case MATHOP_CHAIN_AMPLITUDE_3:
            case MATHOP_CHAIN_AMPLITUDE_1_UPPER:
            case MATHOP_CHAIN_AMPLITUDE_2_UPPER:
            case MATHOP_CHAIN_AMPLITUDE_3_UPPER: {
                uint32_t index = op < MATHOP_CHAIN_AMPLITUDE_1_UPPER ? op - MATHOP_CHAIN_AMPLITUDE_1 : op - MATHOP_CHAIN_AMPLITUDE_1_UPPER;

                if (index < effectCount) {
                    const SpellEffectRec* effect = QuestTextParser::GetSpellEffectRecord(spell->m_id, index);

                    if (effect) {
                        value = effect->m_effectDamageMultiplier;
                    }
                }

                break;
            }

            case MATHOP_MISC_VALUE_1:
            case MATHOP_MISC_VALUE_2:
            case MATHOP_MISC_VALUE_3:
            case MATHOP_MISC_VALUE_1_UPPER:
            case MATHOP_MISC_VALUE_2_UPPER:
            case MATHOP_MISC_VALUE_3_UPPER: {
                uint32_t index = op < MATHOP_CHAIN_AMPLITUDE_1_UPPER ? op - MATHOP_CHAIN_AMPLITUDE_1 : op - MATHOP_CHAIN_AMPLITUDE_1_UPPER;

                if (index < effectCount) {
                    const SpellEffectRec* effect = QuestTextParser::GetSpellEffectRecord(spell->m_id, index);

                    if (effect) {
                        value = static_cast<float>(effect->m_effectMiscValue);
                    }
                }

                break;
            }

            case MATHOP_ATTACK_POWER:
                value = static_cast<float>(player->m_unit->m_unitFieldAttackPower);
                value = 0.0f >= value ? 0.0f : value;
                break;

            case MATHOP_ATTACK_POWER_EFFECTIVE: {
                CGUnitData* data = player->m_unit;
                value = static_cast<float>(data->m_unitFieldAttackPower + data->m_unitFieldAttackPowerModPos + data->m_unitFieldAttackPowerModNeg) * (data->m_unitFieldAttackPowerMultiplier + 1.0f);
                value = 0.0f >= value ? 0.0f : value;
                break;
            }

            case MATHOP_RANGED_ATTACK_POWER:
                value = static_cast<float>(player->m_unit->m_unitFieldRangedAttackPower);
                value = 0.0f >= value ? 0.0f : value;
                break;

            case MATHOP_RANGED_ATTACK_POWER_EFFECTIVE: {
                CGUnitData* data = player->m_unit;
                value = static_cast<float>(data->m_unitFieldRangedAttackPower + data->m_unitFieldRangedAttackPowerModPos + data->m_unitFieldRangedAttackPowerModNeg) * (data->m_unitFieldRangedAttackPowerMultiplier + 1.0f);
                value = 0.0f >= value ? 0.0f : value;
                break;
            }

            case MATHOP_MAIN_HAND_WEAPON_MIN:
            case MATHOP_OFF_HAND_WEAPON_MIN:
            case MATHOP_RANGED_WEAPON_MIN: {
                const VisibleItem_C* item = player->GetVisibleItem(op == MATHOP_MAIN_HAND_WEAPON_MIN ? 15 : op == MATHOP_OFF_HAND_WEAPON_MIN ? 16 : 17);

                if (!item) {
                    break;
                }

                ItemRecSparse* sparse = g_itemSparseDB.GetRecord(abs(item->m_itemId), WOWGUID(), NULL, NULL, false);

                if (!sparse) {
                    break;
                }

                ItemRec* rec = sparse->GetNonSparse();

                if (!rec || sparse->m_damageType) {
                    break;
                }

                value = GetEffectiveWeaponDamage(player->m_unit->m_unitFieldMaxitemlevel, sparse->m_itemLevel, rec->m_classID, rec->m_subclassID, sparse->m_quality, sparse->m_armorDamageModifier, sparse->m_delay, sparse->m_statScalingFactor, sparse->m_inventoryType, sparse->m_flags[1], true);
                break;
            }

            case MATHOP_MAIN_HAND_WEAPON_MAX:
            case MATHOP_OFF_HAND_WEAPON_MAX:
            case MATHOP_RANGED_WEAPON_MAX: {
                const VisibleItem_C* item = player->GetVisibleItem(op == MATHOP_MAIN_HAND_WEAPON_MAX ? 15 : op == MATHOP_OFF_HAND_WEAPON_MAX ? 16 : 17);

                if (!item) {
                    break;
                }

                ItemRecSparse* sparse = g_itemSparseDB.GetRecord(abs(item->m_itemId), WOWGUID(), NULL, NULL, false);

                if (!sparse) {
                    break;
                }

                ItemRec* rec = sparse->GetNonSparse();

                if (!rec || sparse->m_damageType) {
                    break;
                }

                value = GetEffectiveWeaponDamage(player->m_unit->m_unitFieldMaxitemlevel, sparse->m_itemLevel, rec->m_classID, rec->m_subclassID, sparse->m_quality, sparse->m_armorDamageModifier, sparse->m_delay, sparse->m_statScalingFactor, sparse->m_inventoryType, sparse->m_flags[1], false);
                break;
            }

            case MATHOP_MAIN_HAND_DAMAGE_MIN:
            case MATHOP_OFF_HAND_DAMAGE_MIN:
            case MATHOP_RANGED_DAMAGE_MIN: {
                float damage = op == MATHOP_MAIN_HAND_DAMAGE_MIN ? player->m_unit->m_unitFieldMindamage : op == MATHOP_OFF_HAND_DAMAGE_MIN ? player->m_unit->m_unitFieldMinoffhanddamage : player->m_unit->m_unitFieldMinrangeddamage;
                value = static_cast<float>(floor(damage));
                value = value <= 1.0f ? 1.0f : value;
                break;
            }

            case MATHOP_MAIN_HAND_DAMAGE_MAX:
            case MATHOP_OFF_HAND_DAMAGE_MAX:
            case MATHOP_RANGED_DAMAGE_MAX: {
                float damage = op == MATHOP_MAIN_HAND_DAMAGE_MAX ? player->m_unit->m_unitFieldMaxdamage : op == MATHOP_OFF_HAND_DAMAGE_MAX ? player->m_unit->m_unitFieldMaxoffhanddamage : player->m_unit->m_unitFieldMaxrangeddamage;
                value = ceilf(damage) <= 1.0f ? 1.0f : ceilf(damage);
                break;
            }

            case MATHOP_BASE_ARMOR: {
                int32_t baseValue = 0;
                int32_t total = 0;
                int32_t effective = 0;
                int32_t positive = 0;
                int32_t negative = 0;
                player->GetResistanceAndBuffs(GetPhysicalDamageClassID(), &baseValue, &total, &effective, &positive, &negative);
                value = static_cast<float>(baseValue);
                break;
            }

            case MATHOP_ARMOR: {
                int32_t baseValue = 0;
                int32_t total = 0;
                int32_t effective = 0;
                int32_t positive = 0;
                int32_t negative = 0;
                player->GetResistanceAndBuffs(GetPhysicalDamageClassID(), &baseValue, &total, &effective, &positive, &negative);
                value = static_cast<float>(effective);
                break;
            }

            case MATHOP_MAIN_HAND_SPEED:
            case MATHOP_MAIN_HAND_SPEED_UPPER:
                value = static_cast<float>(player->m_unit->m_unitFieldBaseattacktime[0] * 0.001);
                break;

            case MATHOP_OFF_HAND_SPEED:
            case MATHOP_OFF_HAND_SPEED_UPPER:
                value = static_cast<float>(player->m_unit->m_unitFieldBaseattacktime[1] * 0.001);
                break;

            case MATHOP_RANGED_SPEED:
            case MATHOP_RANGED_SPEED_UPPER:
                value = static_cast<float>(player->m_unit->m_unitFieldRangedattacktime * 0.001);
                break;

            case MATHOP_LEVEL:
            case MATHOP_LEVEL_UPPER:
                value = static_cast<float>(static_cast<int32_t>(player->m_unit->m_unitFieldLevel));
                break;

            case MATHOP_HANDS:
            case MATHOP_HANDS_UPPER:
                value = CGUnit_C::IsTwoHanded(player->GetVirtualItem(0, false)) ? 2.0f : 1.0f;
                break;

            case MATHOP_SPELL_POWER:
            case MATHOP_SPELL_POWER_UPPER: {
                int32_t bonus = player->GetModDamageDonePos(2) + player->GetModDamageDoneNeg(2);

                for (uint32_t school = 2; school <= 6; school++) {
                    int32_t mask = 1 << school;

                    if (bonus >= player->GetModDamageDonePos(mask) + player->GetModDamageDoneNeg(mask)) {
                        bonus = player->GetModDamageDonePos(mask) + player->GetModDamageDoneNeg(mask);
                    }
                }

                value = static_cast<float>(bonus);
                break;
            }

            case MATHOP_SPELL_POWER_HOLY:
            case MATHOP_SPELL_POWER_FIRE:
            case MATHOP_SPELL_POWER_NATURE:
            case MATHOP_SPELL_POWER_FROST:
            case MATHOP_SPELL_POWER_SHADOW:
            case MATHOP_SPELL_POWER_ARCANE:
            case MATHOP_SPELL_POWER_HOLY_UPPER:
            case MATHOP_SPELL_POWER_FIRE_UPPER:
            case MATHOP_SPELL_POWER_NATURE_UPPER:
            case MATHOP_SPELL_POWER_FROST_UPPER:
            case MATHOP_SPELL_POWER_SHADOW_UPPER:
            case MATHOP_SPELL_POWER_ARCANE_UPPER: {
                int32_t mask = 1 << ((op < MATHOP_SPELL_POWER_HOLY_UPPER ? op - MATHOP_SPELL_POWER_HOLY : op - MATHOP_SPELL_POWER_HOLY_UPPER) + 1);
                float positive = static_cast<float>(player->GetModDamageDonePos(mask));
                value = static_cast<float>(player->GetModDamageDoneNeg(mask)) + positive;
                break;
            }

            case MATHOP_HEALING_POWER:
            case MATHOP_HEALING_POWER_UPPER:
                value = static_cast<float>(player->GetModHealingDonePos());
                break;

            case MATHOP_DAMAGE_PCT_HOLY:
            case MATHOP_DAMAGE_PCT_FIRE:
            case MATHOP_DAMAGE_PCT_NATURE:
            case MATHOP_DAMAGE_PCT_FROST:
            case MATHOP_DAMAGE_PCT_SHADOW:
            case MATHOP_DAMAGE_PCT_ARCANE:
            case MATHOP_DAMAGE_PCT_HOLY_UPPER:
            case MATHOP_DAMAGE_PCT_FIRE_UPPER:
            case MATHOP_DAMAGE_PCT_NATURE_UPPER:
            case MATHOP_DAMAGE_PCT_FROST_UPPER:
            case MATHOP_DAMAGE_PCT_SHADOW_UPPER:
            case MATHOP_DAMAGE_PCT_ARCANE_UPPER: {
                uint32_t index = op < MATHOP_DAMAGE_PCT_HOLY_UPPER ? op - MATHOP_DAMAGE_PCT_HOLY : op - MATHOP_DAMAGE_PCT_HOLY_UPPER;
                value = *reinterpret_cast<const float*>(&player->m_player->m_playerFieldModDamageDonePct[index + 1]);
                break;
            }

            case MATHOP_HEALING_PCT:
            case MATHOP_HEALING_PCT_UPPER:
                value = player->m_player->m_playerFieldModHealingPct;
                break;

            case MATHOP_HEALING_DONE_PCT:
            case MATHOP_HEALING_DONE_PCT_UPPER:
                value = player->m_player->m_playerFieldModHealingDonePct;
                break;

            case MATHOP_BONUS_COEFFICIENT_1:
            case MATHOP_BONUS_COEFFICIENT_2:
            case MATHOP_BONUS_COEFFICIENT_3:
            case MATHOP_BONUS_COEFFICIENT_1_UPPER:
            case MATHOP_BONUS_COEFFICIENT_2_UPPER:
            case MATHOP_BONUS_COEFFICIENT_3_UPPER: {
                float coefficient = 0.0f;
                uint32_t index = op < MATHOP_BONUS_COEFFICIENT_1_UPPER ? op - MATHOP_BONUS_COEFFICIENT_1 : op - MATHOP_BONUS_COEFFICIENT_1_UPPER;

                if (index < effectCount) {
                    const SpellEffectRec* effect = QuestTextParser::GetSpellEffectRecord(spell->m_id, index);

                    if (effect) {
                        coefficient = effect->m_effectBonusMultiplier;
                    }

                    if (spell->m_spellScalingID) {
                        SpellScalingRec* scaling = g_spellScalingDB.GetRecord(spell->m_spellScalingID);

                        if (scaling) {
                            coefficient = Spell_C_GetSpellScalingMultiplier(player, scaling, false) * coefficient;
                        }
                    }

                    QuestTextParser::Spell_C_ModifySpellValue(spell, coefficient, 24);
                }

                value = coefficient;
                break;
            }

            case MATHOP_SPELL_COEFFICIENT:
            case MATHOP_SPELL_COEFFICIENT_UPPER:
                value = spell->m_spellCoefficient;
                break;

            default:
                this->m_status = 1;
                value = 0.0f;
                break;
        }
    }

    stack->m_top--;
    stack->m_values[stack->m_top] = value;
}

// OFFSET: 0x00996050/0x1407166B0
float MathState::Evaluate(const SpellRec* spell, int32_t spellLevel, int32_t scale, int32_t parseUnit, int32_t parsePet, int32_t noModifiers) {
    MathStack stack;
    stack.m_top = 32;
    int32_t useLoaded = 0;
    SpellRec* loaded = NULL;
    const float* constant = this->m_constants;
    const float* variable = this->m_variables;
    const int32_t* intArg = this->m_intArgs;

    for (uint32_t i = 0; i < this->m_numOps; i++) {
        uint8_t op = this->m_ops[i];

        if (op == MATHOP_SPELL) {
            if (!(loaded = QuestTextParser::GetSpellRecord(QuestTextParser::Spell_C_SpellGetSpellIDForDifficulty(*intArg++)))) {
                this->m_status = 1;
                return 0.0f;
            }

            useLoaded = 1;
            op = this->m_ops[++i];
        }

        switch (op) {
            case MATHOP_CONSTANT:
                stack.m_values[--stack.m_top] = *constant++;
                break;

            case MATHOP_SPELL_VARIABLE:
                stack.m_values[--stack.m_top] = *variable++;
                break;

            case MATHOP_POW:
            case MATHOP_NEGATE:
            case MATHOP_MUL:
            case MATHOP_DIV:
            case MATHOP_MOD:
            case MATHOP_ADD:
            case MATHOP_SUB:
                this->ExecuteOperator(op, &stack);
                break;

            case MATHOP_ABS:
            case MATHOP_CEIL:
            case MATHOP_FLOOR:
            case MATHOP_MIN:
            case MATHOP_MAX:
            case MATHOP_GT:
            case MATHOP_LT:
            case MATHOP_GTE:
            case MATHOP_LTE:
            case MATHOP_EQ:
            case MATHOP_COND:
            case MATHOP_CLAMP:
            case MATHOP_BASE_HP:
            case MATHOP_BASE_MP:
                this->ExecuteFunction(op, &stack);
                break;

            default:
                this->GetVariableValue(op, &stack, useLoaded ? loaded : spell, spellLevel, scale, parseUnit, parsePet, noModifiers);
                useLoaded = 0;
                break;
        }
    }

    float result = 0.0f;

    if (this->m_status == 2) {
        result = stack.m_values[stack.m_top];
    }

    return result;
}

// OFFSET: 0x009961F0/0x140716900
static int32_t SpellParserExpression(const SpellRec* spell, char* buffer, uint32_t size, int32_t parsePet, int32_t parseUnit, int32_t spellLevel, int32_t scale, int32_t noModifiers, const char** cursor, const char* end, int32_t* intValue, float* value) {
    char expression[256];
    char text[32];
    char format[5] = "%.0f";

    SStrCopy(expression, *cursor + 1, sizeof(expression));
    expression[end - *cursor - 1] = '\0';
    *cursor = end + 1;

    if (**cursor == '.') {
        char digit = (*cursor)[1];

        if (static_cast<uint8_t>(digit - '1') <= 8) {
            SStrPrintf(format, sizeof(format), "%%.%df", digit - '0');
            *cursor += 2;
        }
    }

    s_mathState.m_numOps = 0;
    s_mathState.m_numConstants = 0;
    s_mathState.m_numIntArgs = 0;
    s_mathState.m_numVariables = 0;
    s_mathState.m_status = 0;
    s_mathState.m_name = "TomT";
    s_mathState.m_text = expression;
    s_mathState.CompileSum(0);

    if (s_mathState.m_status != 1) {
        s_mathState.m_status = 2;
        *value = s_mathState.Evaluate(spell, spellLevel, scale, parseUnit, parsePet, noModifiers);

        if (s_mathState.m_status != 2) {
            return 0;
        }

        *intValue = static_cast<int32_t>(*value);
        SStrPrintf(text, sizeof(text), format, *value);
        SStrPack(buffer, text, size);
        return 1;
    }

    return 0;
}

// OFFSET: 0x009963B0/0x140716B90
static void SpellParserVariableExpression(const SpellRec* spell, const char* name, uint32_t nameLength, int32_t parsePet, int32_t parseUnit, int32_t spellLevel, int32_t scale, int32_t noModifiers, const char** cursor, const char* end) {
    SpellVariables* variable = s_spellVariables.New();

    if (!variable) {
        return;
    }

    memset(variable, 0, sizeof(SpellVariables));

    if (SpellParserExpression(spell, variable->m_text, sizeof(variable->m_text), parsePet, parseUnit, spellLevel, scale, noModifiers, cursor, end, &variable->m_intValue, &variable->m_value)) {
        SStrCopy(variable->m_name, name, nameLength);
    }
}

// OFFSET: 0x009964A0/0x140716D30
static int32_t JournalParserReplaceText(char* buffer, uint32_t size, const char** cursor, int32_t difficulty) {
    int32_t spellLevel = 1;
    bool percent = false;

    if (**cursor == '{') {
        const char* end = SStrChr(*cursor, '}');

        if (end) {
            float value;
            return SpellParserExpression(NULL, buffer, size, 0, 0, spellLevel, 1, 1, cursor, end, &s_journalLastNumber, &value);
        }
    }

    if (**cursor == '<') {
        const char* end = SStrChr(*cursor, '>');

        if (end) {
            return SpellParserVariable(buffer, size, cursor, end, &s_journalLastNumber);
        }
    }

    float multiplier = 1.0f;

    if (**cursor == '/' || **cursor == '*') {
        const char* semicolon = SStrChr(*cursor, ';');

        if (semicolon) {
            char factorText[8];
            SStrCopy(factorText, *cursor + 1, sizeof(factorText));
            factorText[semicolon - *cursor - 1] = '\0';
            int32_t factor = SStrToInt(factorText);

            if (factor) {
                if (**cursor == '/') {
                    multiplier = 1.0f / static_cast<float>(factor);
                } else {
                    multiplier = static_cast<float>(factor);
                }
            }

            *cursor = semicolon + 1;
        }
    }

    switch (**cursor) {
        case '%':
            percent = true;
            (*cursor)++;
            break;

        case 'G':
        case 'g':
            if (**cursor) {
                (*cursor)++;
            }

            if (!QuestTextParser::GetPlayerGenderConditional(buffer, size, NULL, *cursor)) {
                return 0;
            }

            if (**cursor) {
                (*cursor)++;
            }

            return 1;

        case 'L':
        case 'l':
            if (**cursor) {
                (*cursor)++;
            }

            return SpellParserPluralConditional(buffer, size, s_journalLastNumber, cursor);

        case 'Z':
        case 'z': {
            const AreaTableRec* area = QuestTextParser::GetAreaTableRecord(QuestTextParser::GetPlayerBindAreaID());

            if (area) {
                SStrPack(buffer, area->m_areaName, size);
            } else {
                SStrPack(buffer, QuestTextParser::FrameScript_GetText("HOME_INN", -1, GENDER_NOT_APPLICABLE), size);
            }

            break;
        }

        case '[': {
            (*cursor)++;
            bool skip = true;

            while (**cursor) {
                char c = **cursor;

                if (c != ' ' && c != ',') {
                    if (c < '0' || c > '9') {
                        break;
                    }

                    if (difficulty == c - '0') {
                        skip = false;
                    }
                }

                (*cursor)++;
            }

            if (skip) {
                for (const char* dollar = SStrChr(*cursor, '$'); dollar; dollar = SStrChr(dollar + 1, '$')) {
                    if (dollar[1] == ']') {
                        *cursor = dollar + 2;
                        return 1;
                    }
                }

                return 1;
            }

            char spaces[1024];
            int32_t length = 0;
            SStrCopy(spaces, *cursor, sizeof(spaces));

            while (**cursor && **cursor <= ' ' && **cursor > 0) {
                (*cursor)++;
                length++;
            }

            spaces[length] = '\0';
            SStrPack(buffer, spaces, size);
            SStrPack(buffer, "|TInterface\\EncounterJournal\\UI-EJ-WarningTextIcon:0:0:0:0|t|CFF741A02", size);
            return 1;
        }

        case ']':
            SStrPack(buffer, "|r", size);
            break;
    }

    bool hasIndex = false;
    int32_t spellId;

    if (SpellParserParseNumber(cursor, &spellId)) {
        bool raidFinder = difficulty == 4;
        const SpellRec* spell = QuestTextParser::GetSpellRecord(QuestTextParser::SpellGetSpellIDForDifficulty(spellId, raidFinder ? 1 : difficulty));

        if (!spell) {
            return 0;
        }

        if (spell->m_attributesExB & 0x1000000) {
            QuestTextParser::GetPlayerLevel(spellLevel);
        }

        if (spell->m_spellLevelsID) {
            const SpellLevelsRec* levels = QuestTextParser::GetSpellLevelsRecord(spell->m_spellLevelsID);

            if (levels) {
                int32_t maxLevel = levels->m_maxLevel;

                if (maxLevel > 0 && spellLevel >= maxLevel) {
                    spellLevel = maxLevel;
                }
            }
        }

        uint32_t index = 0;

        if (static_cast<uint8_t>((*cursor)[1] - '1') > 8) {
            hasIndex = false;
        } else {
            index = (*cursor)[1] - '1';
            hasIndex = true;
        }

        const SpellAuraOptionsRec* auraOptions = QuestTextParser::GetSpellAuraOptionsRecord(spell->m_spellAuraOptionsID);

        if (index >= QuestTextParser::SpellGetNumEffects(spell->m_id)) {
            index = 0;
        }

        const SpellEffectRec* effect = QuestTextParser::GetSpellEffectRecord(spell->m_id, index);
        char number[32];
        char duration[64];

        switch (**cursor) {
            case 'A':
            case 'a': {
                int32_t radius = 0;
                int32_t radiusIndex = effect ? effect->m_effectRadiusIndex : 0;

                if (**cursor == 'A' && effect && effect->m_effectRadiusMaxIndex) {
                    radiusIndex = effect->m_effectRadiusMaxIndex;
                }

                const SpellRadiusRec* rec = QuestTextParser::GetSpellRadiusRecord(radiusIndex);

                if (rec) {
                    radius = static_cast<int32_t>(rec->m_radius);
                    QuestTextParser::Spell_C_ModifySpellValue(spell, radius, 6);
                }

                SStrPrintf(number, sizeof(number), "%d", radius);
                SStrPack(buffer, number, size);
                break;
            }

            case 'B':
            case 'b':
                s_journalLastNumber = static_cast<int32_t>(QuestTextParser::Spell_C_GetPointsPerCombo(QuestTextParser::Spell_C_GetStatTargetUnit(0, 0), spell, index));
                SStrPrintf(number, sizeof(number), "%d", s_journalLastNumber);
                SStrPack(buffer, number, size);
                break;

            case 'C':
            case 'c':
                SStrPrintf(number, sizeof(number), "%d", QuestTextParser::GetUnitSpellManaCost(spell, 0, 0));
                SStrPack(buffer, number, size);
                break;

            case 'D':
            case 'd': {
                int32_t time = QuestTextParser::Spell_C_GetDuration(spell, 0, 0, 1, 0);

                if (time <= 0) {
                    QuestTextParser::FrameScript_GetText("SPELL_DURATION_UNTIL_CANCELLED", duration, sizeof(duration), -1, GENDER_NOT_APPLICABLE);
                } else {
                    float seconds = static_cast<float>(time);

                    if (QuestTextParser::ToolTip_DurationIsInt(seconds, 0.01f)) {
                        QuestTextParser::ToolTip_GetDurationString(duration, sizeof(duration), time, "INT_SPELL_DURATION", NULL, 0, false);
                    } else {
                        QuestTextParser::ToolTip_GetFloatDurationString(duration, sizeof(duration), seconds, "SPELL_DURATION", NULL);
                    }
                }

                lua_convertdecimal(duration);
                SStrPack(buffer, duration, size);
                break;
            }

            case 'E':
            case 'e': {
                float amplitude = !effect ? 0.0f : effect->m_effectValueMultiplier;
                QuestTextParser::Spell_C_ModifySpellValue(spell, amplitude, 27);
                SStrPrintf(number, sizeof(number), "%.1f", amplitude * multiplier);
                lua_convertdecimal(number);
                SStrPack(buffer, number, size);
                break;
            }

            case 'F':
                SStrPrintf(number, sizeof(number), "%d", CMath::fint_n((effect ? effect->m_effectDamageMultiplier : 1.0f) * multiplier));
                SStrPack(buffer, number, size);
                break;

            case 'H':
            case 'h': {
                int32_t chance = auraOptions ? auraOptions->m_procChance : 101;
                QuestTextParser::Spell_C_ModifySpellValue(spell, chance, 18);
                s_journalLastNumber = chance;
                SStrPrintf(number, sizeof(number), "%d", chance);
                SStrPack(buffer, number, size);
                break;
            }

            case 'I':
            case 'i': {
                int32_t targets = 0;
                const SpellTargetRestrictionsRec* restrictions = QuestTextParser::GetSpellTargetRestrictionsRecord(spell->m_spellTargetRestrictionsID);

                if (restrictions) {
                    targets = restrictions->m_maxAffectedTargets;
                }

                s_journalLastNumber = targets;
                SStrPrintf(number, sizeof(number), "%d", targets);
                SStrPack(buffer, number, size);
                break;
            }

            case 'M':
            case 'O':
            case 'S':
            case 'm':
            case 'o':
            case 's': {
                float minPoints;
                float maxPoints;
                QuestTextParser::Spell_C_GetMinMaxPoints(spell, index, &minPoints, &maxPoints, spellLevel, 0, 0, 1);

                if (percent && raidFinder) {
                    float factor = 1.0f - static_cast<float>(GetWorldState(6164)) * 0.01f;
                    minPoints = factor * minPoints;
                    maxPoints = factor * maxPoints;
                }

                char type = **cursor;

                if (type == 'o' || type == 'O') {
                    int32_t period = QuestTextParser::Spell_C_GetAuraPeriod(spell, index);
                    int32_t time;

                    if (period <= 0 || (time = QuestTextParser::Spell_C_GetDuration(spell, 0, 0, 1, 0)) <= 0) {
                        maxPoints = 0.0f;
                        minPoints = 0.0f;
                    } else {
                        int32_t ticks = static_cast<int32_t>(static_cast<float>(time) / static_cast<float>(period));

                        if (spell->m_attributesExE & 0x200) {
                            ticks++;
                        }

                        minPoints = minPoints * static_cast<float>(ticks);
                        maxPoints = maxPoints * static_cast<float>(ticks);
                    }
                }

                float minValue = static_cast<float>(fabs(minPoints) * multiplier);
                int32_t minTrunc = static_cast<int32_t>(minValue);
                int32_t minInt = static_cast<int32_t>(minValue);
                float maxValue = static_cast<float>(multiplier * fabs(maxPoints));
                int32_t maxTrunc = static_cast<int32_t>(maxValue);
                int32_t maxInt = static_cast<int32_t>(maxValue);
                int32_t minIsInt;
                int32_t maxIsInt;

                if (minValue - floor(minValue) >= 0.001 && ceil(minValue) - minValue >= 0.001) {
                    minIsInt = 0;
                } else {
                    minIsInt = 1;

                    if (minInt != static_cast<int32_t>(ceil(minValue)) && ceil(minValue) - minValue < 0.001) {
                        minInt++;
                    }
                }

                if (maxValue - floor(maxValue) >= 0.001 && ceil(maxValue) - maxValue >= 0.001) {
                    maxIsInt = 0;
                } else {
                    maxIsInt = 1;

                    if (maxInt != static_cast<int32_t>(ceil(maxValue)) && ceil(maxValue) - maxValue < 0.001) {
                        maxInt++;
                    }
                }

                type = **cursor;
                int32_t isFloat = type == 'S';

                if (type == 'm') {
                    s_journalLastNumber = minIsInt ? minInt : 2;
                } else {
                    s_journalLastNumber = maxIsInt ? maxInt : 2;
                }

                if (**cursor == 'm') {
                    if (minIsInt) {
                        SStrPrintf(number, sizeof(number), "%d", minInt);
                    } else {
                        SStrPrintf(number, sizeof(number), "%.1f", minValue);
                    }
                } else if (**cursor == 'M') {
                    if (maxIsInt) {
                        SStrPrintf(number, sizeof(number), "%d", maxInt);
                    } else {
                        SStrPrintf(number, sizeof(number), "%.1f", maxValue);
                    }
                } else if (minPoints == maxPoints) {
                    if (minIsInt || !isFloat) {
                        SStrPrintf(number, sizeof(number), "%d", minInt);
                    } else {
                        SStrPrintf(number, sizeof(number), "%.1f", minValue);
                    }
                } else if (g_spellTooltip_DisplayAvgValuesCVar->m_intValue <= 0) {
                    if (minIsInt && maxIsInt) {
                        SStrPrintf(number, sizeof(number), QuestTextParser::FrameScript_GetText("INT_SPELL_POINTS_SPREAD_TEMPLATE", -1, GENDER_NOT_APPLICABLE), minInt, maxInt);
                    } else if (isFloat) {
                        SStrPrintf(number, sizeof(number), QuestTextParser::FrameScript_GetText("SPELL_POINTS_SPREAD_TEMPLATE", -1, GENDER_NOT_APPLICABLE), minValue, maxValue);
                    } else {
                        SStrPrintf(number, sizeof(number), QuestTextParser::FrameScript_GetText("INT_SPELL_POINTS_SPREAD_TEMPLATE", -1, GENDER_NOT_APPLICABLE), minTrunc, maxIsInt ? maxInt : maxTrunc + 1);
                    }
                } else if (minIsInt && maxIsInt) {
                    SStrPrintf(number, sizeof(number), "%d", static_cast<int32_t>(static_cast<double>(minInt + maxInt) * 0.5));
                } else if (!isFloat) {
                    SStrPrintf(number, sizeof(number), "%d", static_cast<int32_t>((static_cast<float>(maxIsInt ? maxInt : maxTrunc + 1) + minValue) * 0.5f));
                } else {
                    SStrPrintf(number, sizeof(number), "%.1f", (maxValue + minValue) * 0.5);
                }

                lua_convertdecimal(number);
                SStrPack(buffer, number, size);
                break;
            }

            case 'N':
            case 'n': {
                int32_t charges = auraOptions ? auraOptions->m_procCharges : 0;
                QuestTextParser::Spell_C_ModifySpellValue(spell, charges, 4);
                s_journalLastNumber = charges;
                SStrPrintf(number, sizeof(number), "%d", charges);
                SStrPack(buffer, number, size);
                break;
            }

            case 'P':
            case 'p':
                SStrPrintf(number, sizeof(number), "%d", QuestTextParser::GetUnitSpellManaCostPerSecond(spell, 0, 0));
                SStrPack(buffer, number, size);
                break;

            case 'Q':
            case 'q': {
                int32_t miscValue = effect ? effect->m_effectMiscValue : 0;
                s_journalLastNumber = miscValue;
                SStrPrintf(number, sizeof(number), "%d", miscValue);
                SStrPack(buffer, number, size);
                break;
            }

            case 'R':
            case 'r': {
                float range = 0.0f;
                const SpellRangeRec* rec = QuestTextParser::GetSpellRangeRecord(spell->m_rangeIndex <= 1 ? 1 : spell->m_rangeIndex);

                if (rec) {
                    range = rec->m_rangeMax[0];
                    QuestTextParser::Spell_C_ModifySpellValue(spell, range, 5);
                }

                s_journalLastNumber = static_cast<int32_t>(ceil(range));
                SStrPrintf(number, sizeof(number), "%d", static_cast<int32_t>(range));
                SStrPack(buffer, number, size);
                break;
            }

            case 'T':
            case 't': {
                int32_t period;

                if ((auraOptions ? auraOptions->m_procTypeMask : 0) & 1) {
                    period = 5200;
                } else {
                    period = QuestTextParser::Spell_C_GetAuraPeriod(spell, index);
                }

                float seconds = static_cast<float>(period);

                if (QuestTextParser::ToolTip_DurationIsInt(seconds, 0.01f)) {
                    SStrPrintf(number, sizeof(number), "%d", period / 1000);
                } else {
                    SStrPrintf(number, sizeof(number), "%.2f", seconds * 0.001);
                }

                lua_convertdecimal(number);
                SStrPack(buffer, number, size);
                break;
            }

            case 'U':
            case 'u': {
                int32_t stacks = auraOptions ? auraOptions->m_cumulativeAura : 0;
                s_journalLastNumber = stacks;
                SStrPrintf(number, sizeof(number), "%d", stacks);
                SStrPack(buffer, number, size);
                break;
            }

            case 'V':
            case 'v': {
                int32_t level = 0;
                const SpellTargetRestrictionsRec* restrictions = QuestTextParser::GetSpellTargetRestrictionsRecord(spell->m_spellTargetRestrictionsID);

                if (restrictions) {
                    level = restrictions->m_maxTargetLevel;
                }

                s_journalLastNumber = level;
                SStrPrintf(number, sizeof(number), "%d", level);
                SStrPack(buffer, number, size);
                break;
            }

            case 'X':
            case 'x': {
                int32_t targets = effect ? effect->m_effectChainTargets : 0;
                QuestTextParser::Spell_C_ModifySpellValue(spell, targets, 17);
                s_journalLastNumber = targets;
                SStrPrintf(number, sizeof(number), "%d", targets);
                SStrPack(buffer, number, size);
                break;
            }

            case 'Z':
            case 'z': {
                const AreaTableRec* area = QuestTextParser::GetAreaTableRecord(QuestTextParser::GetPlayerBindAreaID());

                if (area) {
                    SStrPack(buffer, area->m_areaName, size);
                } else {
                    SStrPack(buffer, QuestTextParser::FrameScript_GetText("HOME_INN", -1, GENDER_NOT_APPLICABLE), size);
                }

                break;
            }

            case 'f':
                SStrPrintf(number, sizeof(number), "%.1f", (effect ? effect->m_effectDamageMultiplier : 1.0f) * multiplier);
                lua_convertdecimal(number);
                SStrPack(buffer, number, size);
                break;

            default:
                return 0;
        }
    }

    if (**cursor) {
        (*cursor)++;

        if (hasIndex) {
            (*cursor)++;
        }
    }

    return 1;
}

// OFFSET: 0x00997A10/0x1407189D0
int32_t JournalParserParseText(const char* text, char* buffer, uint32_t size, int32_t difficulty) {
    if (!text) {
        return 0;
    }

    int32_t failed = 0;
    *buffer = '\0';

    for (const char* dollar = SStrChr(text, '$'); dollar && *dollar; dollar = SStrChr(text, '$')) {
        int32_t length = dollar - text;

        if (dollar != text) {
            uint32_t bufferLength = SStrLen(buffer);
            SStrPack(buffer, text, size);

            if (length + bufferLength < size) {
                buffer[length + bufferLength] = '\0';
            }
        }

        const char* cursor = dollar + 1;

        if (!JournalParserReplaceText(buffer, size, &cursor, difficulty)) {
            SStrPack(buffer, "$", size);
            failed = 1;
        }

        text = cursor;
    }

    SStrPack(buffer, text, size);
    return failed == 0;
}

// OFFSET: 0x00997BA0/0x140718BC0
static int32_t SpellParserConditional(const SpellRec* spell, char* buffer, uint32_t size, int32_t parsePet, int32_t parseUnit, int32_t spellLevel, int32_t scale, int32_t noModifiers, const char** cursor, bool variables, const AuraData* aura) {
    char text[1024];
    int32_t failed = 0;

    while (**cursor == ' ') {
        (*cursor)++;
    }

    if (!**cursor) {
        return 1;
    }

    const char* trueStart;
    const char* trueEnd;
    const char* elseText = NULL;
    const char* falseStart;
    const char* falseEnd;

    if (!SpellParserVerifyConditional(*cursor, &trueStart, &trueEnd, &elseText, &falseStart, &falseEnd)) {
        return 1;
    }

    const char* start;
    const char* stop;

    if (SpellParserParseConditional(cursor, trueStart, aura)) {
        start = trueStart + 1;
        stop = trueEnd;
    } else {
        if (elseText) {
            int32_t result = SpellParserConditional(spell, buffer, size, parsePet, parseUnit, spellLevel, scale, noModifiers, &elseText, variables, aura);
            *cursor = elseText;
            return result;
        }

        stop = falseEnd;
        start = falseStart + 1;
    }

    int32_t length = stop - start + 1;
    SStrCopy(text, start, length);
    text[length] = '\0';
    const char* dollar = SStrChr(text, '$');

    if (variables) {
        if (dollar) {
            const char* expression = SStrChr(dollar, '{');

            if (expression) {
                const char* expressionEnd = SStrChr(expression, '}');

                if (expressionEnd) {
                    SpellParserVariableExpression(spell, buffer, size, parsePet, parseUnit, spellLevel, scale, noModifiers, &expression, expressionEnd);
                }
            }
        }
    } else {
        const char* subCursor = dollar;

        if (!SpellParserParseSubText(spell, buffer, size, parsePet, parseUnit, spellLevel, scale, noModifiers, text, &subCursor, aura)) {
            SStrPack(buffer, "$", size);
            failed = 1;
        }
    }

    *cursor = falseEnd + 1;
    return failed == 0;
}

// OFFSET: 0x00997DB0/0x140718FD0
static void SpellParserVariables(const SpellRec* spell, int32_t parsePet, int32_t parseUnit, int32_t spellLevel, int32_t scale, int32_t noModifiers, const char** cursor, const AuraData* aura) {
    while (*cursor) {
        if (!**cursor) {
            return;
        }

        const char* name = *cursor + 1;
        *cursor = name;
        const char* equals = SStrChr(name, '=');

        if (equals) {
            int32_t length = equals - name + 1;
            uint32_t nameLength = length > 15 ? 15 : length;
            *cursor = SStrChr(*cursor, '$');

            if (!*cursor) {
                continue;
            }

            (*cursor)++;

            if (**cursor == '?') {
                (*cursor)++;
                SpellParserConditional(spell, const_cast<char*>(name), nameLength, parsePet, parseUnit, spellLevel, scale, noModifiers, cursor, true, aura);
            } else if (**cursor == '{') {
                const char* end = SStrChr(*cursor, '}');

                if (end) {
                    SpellParserVariableExpression(spell, name, nameLength, parsePet, parseUnit, spellLevel, scale, noModifiers, cursor, end);
                }
            }
        }

        if (*cursor) {
            *cursor = SStrChr(*cursor, '$');
        }
    }
}

// OFFSET: 0x00997EA0/0x140719150
static int32_t SpellParserReplaceText(const SpellRec* spell, char* buffer, uint32_t size, int32_t parsePet, int32_t parseUnit, int32_t spellLevel, int32_t scale, int32_t noModifiers, const char** cursor, const AuraData* aura) {
    if (**cursor == '{') {
        const char* end = SStrChr(*cursor, '}');

        if (end) {
            float value;
            return SpellParserExpression(spell, buffer, size, parsePet, parseUnit, spellLevel, scale, noModifiers, cursor, end, &s_lastNumber, &value);
        }
    }

    if (**cursor == '<') {
        const char* end = SStrChr(*cursor, '>');

        if (end) {
            return SpellParserVariable(buffer, size, cursor, end, &s_lastNumber);
        }
    }

    float multiplier = 1.0f;

    if (**cursor == '/' || **cursor == '*') {
        const char* semicolon = SStrChr(*cursor, ';');

        if (semicolon) {
            char factorText[8];
            SStrCopy(factorText, *cursor + 1, sizeof(factorText));
            factorText[semicolon - *cursor - 1] = '\0';
            int32_t factor = SStrToInt(factorText);

            if (factor) {
                if (**cursor == '/') {
                    multiplier = 1.0f / static_cast<float>(factor);
                } else {
                    multiplier = static_cast<float>(factor);
                }
            }

            *cursor = semicolon + 1;
        }
    }

    int32_t spellId;

    if (SpellParserParseNumber(cursor, &spellId)) {
        const SpellRec* loaded = QuestTextParser::GetSpellRecord(QuestTextParser::Spell_C_SpellGetSpellIDForDifficulty(spellId));

        if (!loaded) {
            return 0;
        }

        spell = loaded;

        if (loaded->m_attributesExB & 0x1000000) {
            QuestTextParser::GetPlayerLevel(spellLevel);
        }

        if (loaded->m_spellLevelsID) {
            const SpellLevelsRec* levels = QuestTextParser::GetSpellLevelsRecord(loaded->m_spellLevelsID);

            if (levels) {
                int32_t maxLevel = levels->m_maxLevel;

                if (maxLevel > 0 && spellLevel >= maxLevel) {
                    spellLevel = maxLevel;
                }
            }
        }
    }

    uint32_t index = 0;
    bool hasIndex;

    if (static_cast<uint8_t>((*cursor)[1] - '1') > 8) {
        hasIndex = false;
    } else {
        index = (*cursor)[1] - '1';
        hasIndex = true;
    }

    const SpellAuraOptionsRec* auraOptions = QuestTextParser::GetSpellAuraOptionsRecord(spell->m_spellAuraOptionsID);

    if (index >= QuestTextParser::SpellGetNumEffects(spell->m_id)) {
        index = 0;
    }

    const SpellEffectRec* effect = QuestTextParser::GetSpellEffectRecord(spell->m_id, index);
    const char* text = *cursor;
    char number[32];
    char duration[64];

    switch (text[0]) {
        case '?':
            if (text[0]) {
                *cursor = text + 1;
            }

            return SpellParserConditional(spell, buffer, size, parsePet, parseUnit, spellLevel, scale, noModifiers, cursor, false, aura);

        case 'A':
        case 'a': {
            int32_t radiusIndex = 0;
            int32_t radius = 0;

            if (effect) {
                radiusIndex = effect->m_effectRadiusIndex;
            }

            if (text[0] == 'A' && effect && effect->m_effectRadiusMaxIndex) {
                radiusIndex = effect->m_effectRadiusMaxIndex;
            }

            const SpellRadiusRec* rec = QuestTextParser::GetSpellRadiusRecord(radiusIndex);

            if (rec) {
                radius = static_cast<int32_t>(rec->m_radius);
                QuestTextParser::Spell_C_ModifySpellValue(spell, radius, 6);
            }

            SStrPrintf(number, sizeof(number), "%d", radius);
            SStrPack(buffer, number, size);
            break;
        }

        case 'B':
        case 'b':
            s_lastNumber = static_cast<int32_t>(QuestTextParser::Spell_C_GetPointsPerCombo(QuestTextParser::Spell_C_GetStatTargetUnit(parsePet, parseUnit), spell, index));
            SStrPrintf(number, sizeof(number), "%d", s_lastNumber);
            SStrPack(buffer, number, size);
            break;

        case 'C':
        case 'c':
            SStrPrintf(number, sizeof(number), "%d", QuestTextParser::GetUnitSpellManaCost(spell, parseUnit, parsePet));
            SStrPack(buffer, number, size);
            break;

        case 'D':
        case 'd': {
            int32_t time = QuestTextParser::Spell_C_GetDuration(spell, parsePet, parseUnit, noModifiers, 1);

            if (time <= 0) {
                QuestTextParser::FrameScript_GetText("SPELL_DURATION_UNTIL_CANCELLED", duration, sizeof(duration), -1, GENDER_NOT_APPLICABLE);
            } else {
                float seconds = static_cast<float>(time);

                if (QuestTextParser::ToolTip_DurationIsInt(seconds, 0.01f)) {
                    QuestTextParser::ToolTip_GetDurationString(duration, sizeof(duration), time, "INT_SPELL_DURATION", NULL, 0, false);
                } else {
                    QuestTextParser::ToolTip_GetFloatDurationString(duration, sizeof(duration), seconds, "SPELL_DURATION", NULL);
                }
            }

            lua_convertdecimal(duration);
            SStrPack(buffer, duration, size);
            break;
        }

        case 'E':
        case 'e': {
            float amplitude = effect ? effect->m_effectValueMultiplier : 0.0f;
            QuestTextParser::Spell_C_ModifySpellValue(spell, amplitude, 27);
            SStrPrintf(number, sizeof(number), "%.1f", amplitude * multiplier);
            lua_convertdecimal(number);
            SStrPack(buffer, number, size);
            break;
        }

        case 'F':
            SStrPrintf(number, sizeof(number), "%d", CMath::fint_n((effect ? effect->m_effectDamageMultiplier : 1.0f) * multiplier));
            SStrPack(buffer, number, size);
            break;

        case 'G':
        case 'g':
            if (text[0]) {
                *cursor = text + 1;
            }

            if (!QuestTextParser::GetPlayerGenderConditional(buffer, size, NULL, *cursor)) {
                return 0;
            }

            if (**cursor) {
                (*cursor)++;
            }

            return 1;

        case 'H':
        case 'h': {
            int32_t chance = auraOptions ? auraOptions->m_procChance : 101;
            QuestTextParser::Spell_C_ModifySpellValue(spell, chance, 18);
            s_lastNumber = chance;
            SStrPrintf(number, sizeof(number), "%d", chance);
            SStrPack(buffer, number, size);
            break;
        }

        case 'I':
        case 'i': {
            int32_t targets = 0;
            const SpellTargetRestrictionsRec* restrictions = QuestTextParser::GetSpellTargetRestrictionsRecord(spell->m_spellTargetRestrictionsID);

            if (restrictions) {
                targets = restrictions->m_maxAffectedTargets;
            }

            s_lastNumber = targets;
            SStrPrintf(number, sizeof(number), "%d", targets);
            SStrPack(buffer, number, size);
            break;
        }

        case 'J':
        case 'j': {
            if (!aura || !hasIndex) {
                break;
            }

            *cursor = text + 1;
            int32_t speedType = 0;

            switch ((*cursor)[1]) {
                case 'f':
                    speedType = 2;
                    break;

                case 'g':
                    speedType = 1;
                    break;

                case 's':
                    speedType = 3;
                    break;
            }

            SStrPrintf(number, sizeof(number), "%d", QuestTextParser::Spell_C_GetUnitMountSpeed(aura->m_effectAmounts[index], speedType));
            SStrPack(buffer, number, size);
            break;
        }

        case 'L':
        case 'l':
            if (text[0]) {
                *cursor = text + 1;
            }

            return SpellParserPluralConditional(buffer, size, s_lastNumber, cursor);

        case 'M':
        case 'O':
        case 'S':
        case 'm':
        case 'o':
        case 's': {
            float minPoints;
            float maxPoints;
            QuestTextParser::Spell_C_GetMinMaxPoints(spell, index, &minPoints, &maxPoints, spellLevel, parsePet, parseUnit, noModifiers);
            minPoints = static_cast<float>(scale) * minPoints;
            maxPoints = static_cast<float>(scale) * maxPoints;
            char type = **cursor;

            if (type == 'o' || type == 'O') {
                int32_t period = QuestTextParser::Spell_C_GetAuraPeriod(spell, index);
                int32_t time;

                if (period <= 0 || (time = QuestTextParser::Spell_C_GetDuration(spell, parsePet, parseUnit, noModifiers, 0)) <= 0) {
                    maxPoints = 0.0f;
                    minPoints = 0.0f;
                } else {
                    int32_t ticks = static_cast<int32_t>(static_cast<float>(time) / static_cast<float>(period));

                    if (spell->m_attributesExE & 0x200) {
                        ticks++;
                    }

                    maxPoints = static_cast<float>(ticks) * maxPoints;
                    minPoints = static_cast<float>(ticks) * minPoints;
                }
            }

            float minValue = static_cast<float>(fabs(minPoints) * multiplier);
            int32_t minTrunc = static_cast<int32_t>(minValue);
            int32_t minInt = static_cast<int32_t>(minValue);
            float maxValue = static_cast<float>(multiplier * fabs(maxPoints));
            int32_t maxTrunc = static_cast<int32_t>(maxValue);
            int32_t maxInt = static_cast<int32_t>(maxValue);
            int32_t minIsInt;
            int32_t maxIsInt;

            if (minValue - floor(minValue) >= 0.001 && ceil(minValue) - minValue >= 0.001) {
                minIsInt = 0;
            } else {
                minIsInt = 1;

                if (minInt != static_cast<int32_t>(ceil(minValue)) && ceil(minValue) - minValue < 0.001) {
                    minInt++;
                }
            }

            if (maxValue - floor(maxValue) >= 0.001 && ceil(maxValue) - maxValue >= 0.001) {
                maxIsInt = 0;
            } else {
                maxIsInt = 1;

                if (maxInt != static_cast<int32_t>(ceil(maxValue)) && ceil(maxValue) - maxValue < 0.001) {
                    maxInt++;
                }
            }

            type = **cursor;
            int32_t isFloat = type == 'S';

            if (type == 'm') {
                s_lastNumber = minIsInt ? minInt : 2;
            } else {
                s_lastNumber = maxIsInt ? maxInt : 2;
            }

            type = **cursor;

            if (type == 'm') {
                if (minIsInt) {
                    SStrPrintf(number, sizeof(number), "%d", minInt);
                } else {
                    SStrPrintf(number, sizeof(number), "%.1f", minValue);
                }
            } else if (type == 'M') {
                if (maxIsInt) {
                    SStrPrintf(number, sizeof(number), "%d", maxInt);
                } else {
                    SStrPrintf(number, sizeof(number), "%.1f", maxValue);
                }
            } else if (minPoints == maxPoints) {
                if (minIsInt || !isFloat) {
                    SStrPrintf(number, sizeof(number), "%d", minInt);
                } else {
                    SStrPrintf(number, sizeof(number), "%.1f", minValue);
                }
            } else if (g_spellTooltip_DisplayAvgValuesCVar->m_intValue <= 0) {
                if (minIsInt && maxIsInt) {
                    SStrPrintf(number, sizeof(number), QuestTextParser::FrameScript_GetText("INT_SPELL_POINTS_SPREAD_TEMPLATE", -1, GENDER_NOT_APPLICABLE), minInt, maxInt);
                } else if (isFloat) {
                    SStrPrintf(number, sizeof(number), QuestTextParser::FrameScript_GetText("SPELL_POINTS_SPREAD_TEMPLATE", -1, GENDER_NOT_APPLICABLE), minValue, maxValue);
                } else {
                    SStrPrintf(number, sizeof(number), QuestTextParser::FrameScript_GetText("INT_SPELL_POINTS_SPREAD_TEMPLATE", -1, GENDER_NOT_APPLICABLE), minTrunc, maxIsInt ? maxInt : maxTrunc + 1);
                }
            } else if (minIsInt && maxIsInt) {
                SStrPrintf(number, sizeof(number), "%d", static_cast<int32_t>(static_cast<double>(minInt + maxInt) * 0.5));
            } else if (!isFloat) {
                SStrPrintf(number, sizeof(number), "%d", static_cast<int32_t>((static_cast<float>(maxIsInt ? maxInt : maxTrunc + 1) + minValue) * 0.5f));
            } else {
                SStrPrintf(number, sizeof(number), "%.1f", (maxValue + minValue) * 0.5);
            }

            lua_convertdecimal(number);
            SStrPack(buffer, number, size);
            break;
        }

        case 'N':
        case 'n': {
            int32_t charges = auraOptions ? auraOptions->m_procCharges : 0;
            QuestTextParser::Spell_C_ModifySpellValue(spell, charges, 4);
            s_lastNumber = charges;
            SStrPrintf(number, sizeof(number), "%d", charges);
            SStrPack(buffer, number, size);
            break;
        }

        case 'P':
        case 'p':
            SStrPrintf(number, sizeof(number), "%d", QuestTextParser::GetUnitSpellManaCostPerSecond(spell, parseUnit, parsePet));
            SStrPack(buffer, number, size);
            break;

        case 'Q':
        case 'q': {
            int32_t miscValue = effect ? effect->m_effectMiscValue : 0;
            s_lastNumber = miscValue;
            SStrPrintf(number, sizeof(number), "%d", miscValue);
            SStrPack(buffer, number, size);
            break;
        }

        case 'R':
        case 'r': {
            float range = 0.0f;
            const SpellRangeRec* rec = QuestTextParser::GetSpellRangeRecord(spell->m_rangeIndex <= 1 ? 1 : spell->m_rangeIndex);

            if (rec) {
                range = rec->m_rangeMax[0];
                QuestTextParser::Spell_C_ModifySpellValue(spell, range, 5);
            }

            s_lastNumber = static_cast<int32_t>(ceil(range));
            SStrPrintf(number, sizeof(number), "%d", static_cast<int32_t>(range));
            SStrPack(buffer, number, size);
            break;
        }

        case 'T':
        case 't': {
            int32_t period;

            if ((auraOptions ? auraOptions->m_procTypeMask : 0) & 1) {
                period = 5200;
            } else {
                period = QuestTextParser::Spell_C_GetAuraPeriod(spell, index);
            }

            float seconds = static_cast<float>(period);

            if (QuestTextParser::ToolTip_DurationIsInt(seconds, 0.01f)) {
                SStrPrintf(number, sizeof(number), "%d", period / 1000);
            } else {
                SStrPrintf(number, sizeof(number), "%.2f", seconds * 0.001);
            }

            lua_convertdecimal(number);
            SStrPack(buffer, number, size);
            break;
        }

        case 'U':
        case 'u': {
            int32_t stacks = auraOptions ? auraOptions->m_cumulativeAura : 0;
            s_lastNumber = stacks;
            SStrPrintf(number, sizeof(number), "%d", stacks);
            SStrPack(buffer, number, size);
            break;
        }

        case 'V':
        case 'v': {
            int32_t level = 0;
            const SpellTargetRestrictionsRec* restrictions = QuestTextParser::GetSpellTargetRestrictionsRecord(spell->m_spellTargetRestrictionsID);

            if (restrictions) {
                level = restrictions->m_maxTargetLevel;
            }

            s_lastNumber = level;
            SStrPrintf(number, sizeof(number), "%d", level);
            SStrPack(buffer, number, size);
            break;
        }

        case 'W':
        case 'w': {
            if (!aura) {
                break;
            }

            int32_t amount = aura->m_effectAmounts[index];

            if (amount <= 0) {
                amount = -amount;
            }

            SStrPrintf(number, sizeof(number), "%d", amount);
            SStrPack(buffer, number, size);
            break;
        }

        case 'X':
        case 'x': {
            int32_t targets = effect ? effect->m_effectChainTargets : 0;
            QuestTextParser::Spell_C_ModifySpellValue(spell, targets, 17);
            s_lastNumber = targets;
            SStrPrintf(number, sizeof(number), "%d", targets);
            SStrPack(buffer, number, size);
            break;
        }

        case 'Z':
        case 'z': {
            const AreaTableRec* area = QuestTextParser::GetAreaTableRecord(QuestTextParser::GetPlayerBindAreaID());

            if (area) {
                SStrPack(buffer, area->m_areaName, size);
            } else {
                SStrPack(buffer, QuestTextParser::FrameScript_GetText("HOME_INN", -1, GENDER_NOT_APPLICABLE), size);
            }

            break;
        }

        case 'f':
            SStrPrintf(number, sizeof(number), "%.1f", (effect ? effect->m_effectDamageMultiplier : 1.0f) * multiplier);
            lua_convertdecimal(number);
            SStrPack(buffer, number, size);
            break;

        default:
            return 0;
    }

    if (**cursor) {
        (*cursor)++;

        if (hasIndex) {
            (*cursor)++;
        }
    }

    return 1;
}

// OFFSET: 0x00999270/0x14071AB90
static int32_t SpellParserParseSubText(const SpellRec* spell, char* buffer, uint32_t size, int32_t parsePet, int32_t parseUnit, int32_t spellLevel, int32_t scale, int32_t noModifiers, const char* text, const char** cursor, const AuraData* aura) {
    int32_t failed = 0;

    while (*cursor && **cursor) {
        int32_t length = *cursor - text;

        if (*cursor != text) {
            uint32_t bufferLength = SStrLen(buffer);
            SStrPack(buffer, text, size);

            if (bufferLength + length < size) {
                buffer[bufferLength + length] = '\0';
            }
        }

        (*cursor)++;

        if (!SpellParserReplaceText(spell, buffer, size, parsePet, parseUnit, spellLevel, scale, noModifiers, cursor, aura)) {
            SStrPack(buffer, "$", size);
            failed = 1;
        }

        text = *cursor;
        *cursor = SStrChr(*cursor, '$');
    }

    SStrPack(buffer, text, size);
    return failed == 0;
}

// OFFSET: 0x009993F0/0x14071ADE0
int32_t SpellParserParseText(const SpellRec* spell, char* buffer, uint32_t size, int32_t parsePet, int32_t parseUnit, int32_t spellLevel, int32_t useAuraDescription, int32_t scale, int32_t noModifiers, const AuraData* aura) {
    *buffer = '\0';

    if (!spellLevel) {
        spellLevel = QuestTextParser::Spell_C_GetSpellLevel(spell, parsePet, parseUnit);
    }

    const SpellDescriptionVariablesRec* variables = QuestTextParser::GetSpellDescriptionVariablesRecord(spell->m_spellDescriptionVariableID);

    const char* cursor;

    if (variables) {
        cursor = SStrChr(variables->m_variables, '$');
        SpellParserVariables(spell, parsePet, parseUnit, spellLevel, scale, noModifiers, &cursor, aura);
    }

    const char* description;

    if (!useAuraDescription || !*(description = spell->m_auraDescription)) {
        description = spell->m_description;
    }

    cursor = SStrChr(description, '$');
    int32_t result = SpellParserParseSubText(spell, buffer, size, parsePet, parseUnit, spellLevel, scale, noModifiers, description, &cursor, aura);
    s_spellVariables.m_count = 0;
    return result;
}

// OFFSET: 0x009994B0/0x14071AF60
int32_t SpellParserParseTextAsSpell(const SpellRec* spell, char* buffer, uint32_t size, const char* text, int32_t parsePet, int32_t parseUnit, int32_t spellLevel, int32_t scale, int32_t noModifiers, const AuraData* aura) {
    *buffer = '\0';

    if (!spellLevel) {
        spellLevel = QuestTextParser::Spell_C_GetSpellLevel(spell, parsePet, parseUnit);
    }

    const SpellDescriptionVariablesRec* variables = QuestTextParser::GetSpellDescriptionVariablesRecord(spell->m_spellDescriptionVariableID);

    const char* cursor;

    if (variables) {
        cursor = SStrChr(variables->m_variables, '$');
        SpellParserVariables(spell, parsePet, parseUnit, spellLevel, scale, noModifiers, &cursor, aura);
    }

    cursor = SStrChr(text, '$');
    int32_t result = SpellParserParseSubText(spell, buffer, size, parsePet, parseUnit, spellLevel, scale, noModifiers, text, &cursor, aura);
    s_spellVariables.m_count = 0;
    return result;
}

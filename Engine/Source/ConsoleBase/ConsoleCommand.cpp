#include <cstdint>
#include <cstdlib>
#include <cstring>
#include "Engine/Source/ConsoleBase/ConsoleCommand.h"
#include "Engine/Source/Base/ConvertUTF.h"
#include "Engine/Source/ConsoleBase/IConsole.h"
#include "Engine/Source/Storm/H/storm.h"

// STATIC: %0x00BBD104
int32_t s_execState = 4;

// STATIC: %0x00CD89E8
static char s_execBuffer[0x2000];

// STATIC: %0x00CDA9E8
char g_commandHistory[32][1024];

// STATIC: %0x00CE29E8
uint32_t g_commandHistoryIndex;

// STATIC: %0x00CE29EC
COMMANDHANDLER g_defaultCommand;

// STATIC: %0x00CE29F0
TSHashTable<CONSOLECOMMAND, HASHKEY_CONSTSTRI> g_consoleCommandHash;

// STATIC: %0x00CE2A20
static char s_commandName[64];

// STATIC: %0x00BBD100
static const char* s_noHelpText = "No help yet";

// STATIC: %0x00BBD028
static CATEGORYINFO s_categories[NUM_CATEGORIES] = {
    { CATEGORY_DEBUG, "debug" },
    { CATEGORY_GRAPHICS, "graphics" },
    { CATEGORY_CONSOLE, "console" },
    { CATEGORY_COMBAT, "combat" },
    { CATEGORY_GAME, "game" },
    { CATEGORY_DEFAULT, "default" },
    { CATEGORY_NET, "net" },
    { CATEGORY_SOUND, "sound" },
    { CATEGORY_GM, "gm" },
};

// PROVEN: 1fbbef9c IDENTICAL/NONE
// INLINED
CONSOLECOMMAND::CONSOLECOMMAND() {
    this->m_helpText = NULL;
}

// PROVEN: 1fbbef9c IDENTICAL/NONE
// OFFSET: 0x00610960
static int32_t ConsoleCommand_Ver(const char* command, const char* arguments) {
    ConsoleWrite("WoW [Release x86] Build 15595 (Apr 10 2012)", DEFAULT_COLOR);

    return 1;
}

// PROVEN: b68528ef IDENTICAL/NONE
// OFFSET: 0x00610980/0x14029AED0
static int32_t ConsoleCommand_Quit(const char* command, const char* arguments) {
    ConsolePostClose();

    return 1;
}

// PROVEN: 9ca48304 IDENTICAL/NONE
// OFFSET: 0x00653970/0x1402F3F40
static int32_t CompareCompletions(const void* elem1, const void* elem2) {
    const char* const* str1 = static_cast<const char* const*>(elem1);
    const char* const* str2 = static_cast<const char* const*>(elem2);

    if (!str1 || !str2 || !*str1 || !*str2) {
        return 0;
    }

    return _stricmp(*str1, *str2);
}

// OFFSET: 0x00653B00/0x1402F3FA0
static int32_t ConsoleCommand_Help(const char* command, const char* arguments) {
    if (!*arguments) {
        char buffer[512] = { 0 };
        ConsoleWrite("Console help categories: ", WARNING_COLOR);

        for (uint32_t i = 0; i < NUM_CATEGORIES; i++) {
            SStrPack(buffer, s_categories[i].m_name, sizeof(buffer));

            if (i + 1 < NUM_CATEGORIES) {
                SStrPack(buffer, ", ", sizeof(buffer));
            }
        }

        SStrPack(buffer, ", ALL", sizeof(buffer));
        ConsoleWrite(buffer, WARNING_COLOR);
        ConsoleWrite("For more information type 'help [command] or [category]'", WARNING_COLOR);

        return 1;
    }

    CATEGORY category = NUM_CATEGORIES;
    bool all = !SStrCmpI("all", arguments, STORM_MAX_STR);

    for (uint32_t i = 0; i < NUM_CATEGORIES; i++) {
        if (!SStrCmpI(s_categories[i].m_name, arguments, STORM_MAX_STR)) {
            category = s_categories[i].m_category;
            break;
        }
    }

    if (all || category != NUM_CATEGORIES) {
        char buffer[512] = { 0 };
        SStrPrintf(buffer, sizeof(buffer), "Commands registered for the category %s:", arguments);
        ConsoleWrite(buffer, WARNING_COLOR);

        uint32_t count = 0;
        buffer[0] = '\0';

        for (CONSOLECOMMAND* cmd = g_consoleCommandHash.m_fulllist.Head(); cmd; cmd = g_consoleCommandHash.m_fulllist.Next(cmd)) {
            if (!all && cmd->m_category != category) {
                continue;
            }

            SStrPack(buffer, cmd->m_key.m_str, sizeof(buffer));
            SStrPack(buffer, ", ", sizeof(buffer));

            if (++count == 8) {
                ConsoleWrite(buffer, WARNING_COLOR);
                buffer[0] = '\0';
                count = 0;
            }
        }

        if (buffer[0]) {
            char* comma = SStrChrR(buffer, ',');

            if (comma) {
                *comma = '\0';
            }

            ConsoleWrite(buffer, WARNING_COLOR);
        } else {
            ConsoleWrite("NONE", WARNING_COLOR);
        }
    }

    CONSOLECOMMAND* cmd = g_consoleCommandHash.Ptr(arguments);

    if (cmd) {
        char line[197];
        SStrPrintf(line, sizeof(line), "Help for command %s:", arguments);
        ConsoleWrite(line, WARNING_COLOR);
        SStrPrintf(line, sizeof(line), "     %s %s", arguments, cmd->m_helpText ? cmd->m_helpText : s_noHelpText);
        ConsoleWrite(line, WARNING_COLOR);
    }

    return 1;
}

// PROVEN: 7dae32a2 IDENTICAL/IDENTICAL
// OFFSET: 0x00653950/0x1402F3F20
const char* ConsoleCommandHistory(uint32_t index) {
    return g_commandHistory[(g_commandHistoryIndex - index - 1) & 31];
}

// PROVEN: ab7b4dc7 IDENTICAL/IDENTICAL
// OFFSET: 0x00A39FF0/0x14090ED00
uint32_t ConsoleCommandHistoryDepth() {
    return sizeof(g_commandHistory) / sizeof(g_commandHistory[0]);
}

// OFFSET: 0x00653EA0/0x1402F4430
CONSOLECOMMAND* ParseCommand(const char* commandLine, const char** command, char* arguments, uint32_t argsbytes) {
    const char* src = commandLine;
    char* dst = s_commandName;
    int32_t space = sizeof(s_commandName) - 1;
    int32_t chars;

    do {
        uint32_t c = sgetu8(reinterpret_cast<const uint8_t*>(src), &chars);

        if (c == 0xFFFFFFFF || c == ' ' || chars > space) {
            break;
        }

        if (chars > 0) {
            space -= chars;
        }

        while (chars-- > 0) {
            *dst++ = *src++;
        }
    } while (space > 0);

    *dst = '\0';

    if (command) {
        *command = s_commandName;
    }

    if (arguments) {
        for (uint32_t c = sgetu8(reinterpret_cast<const uint8_t*>(src), &chars); c != 0xFFFFFFFF && c == ' '; c = sgetu8(reinterpret_cast<const uint8_t*>(src), &chars)) {
            src += chars;
        }

        SStrCopy(arguments, src, argsbytes);

        for (uint32_t length = SStrLen(arguments); length > 0 && arguments[length - 1] == ' '; length--) {
            arguments[length - 1] = '\0';
        }
    }

    return g_consoleCommandHash.Ptr(s_commandName);
}

// OFFSET: 0x00653FA0/0x1402F4610
void ConsoleCommandUnregister(const char* command) {
    if (!command) {
        return;
    }

    CONSOLECOMMAND* cmd = g_consoleCommandHash.Ptr(command);

    if (cmd) {
        g_consoleCommandHash.Delete(cmd);
    }
}

// OFFSET: 0x00653FE0/0x1402F4670
int32_t ConsoleCommandComplete(const char* partial, const char** previous, int32_t* index, int32_t direction, char (&buffer)[1024]) {
    if (strchr(partial, ' ')) {
        const char* command = NULL;
        char arguments[1024];
        CONSOLECOMMAND* cmd = ParseCommand(partial, &command, arguments, sizeof(arguments));

        if (!cmd || !cmd->m_completion) {
            return 0;
        }

        TSGrowableArray<const char*> matches;
        int32_t replacePos = 0;
        cmd->m_completion(arguments, matches, replacePos);
        qsort(matches.m_data, matches.Count(), sizeof(const char*), CompareCompletions);

        uint32_t count = matches.Count();

        if (*index < 0) {
            *index = 0;
        } else {
            *index += direction ? -1 : 1;
        }

        if (*index >= 0 && static_cast<uint32_t>(*index) < count) {
            if (replacePos > 0 && static_cast<uint32_t>(replacePos) <= strlen(arguments)) {
                arguments[replacePos] = '\0';
                SStrPrintf(buffer, "%s %s", command, arguments);
                SStrPack(buffer, matches[*index], sizeof(buffer));
            } else {
                SStrPrintf(buffer, "%s %s", command, matches[*index]);
            }

            return 1;
        }

        if (*index < 0) {
            *index = 0;
        }

        if (*index >= static_cast<int32_t>(count)) {
            *index = count - 1;
        }

        return 0;
    }

    CONSOLECOMMAND* cmd;

    if (*previous) {
        cmd = g_consoleCommandHash.Ptr(*previous);

        if (!cmd) {
            return 0;
        }

        cmd = direction ? g_consoleCommandHash.m_fulllist.Prev(cmd) : g_consoleCommandHash.m_fulllist.Next(cmd);
    } else {
        cmd = g_consoleCommandHash.m_fulllist.Head();
    }

    uint32_t length = SStrLen(partial);

    for (; cmd; cmd = direction ? g_consoleCommandHash.m_fulllist.Prev(cmd) : g_consoleCommandHash.m_fulllist.Next(cmd)) {
        if (!SStrCmpI(partial, cmd->m_key.m_str, length)) {
            *previous = cmd->m_key.m_str;
            SStrCopy(buffer, cmd->m_key.m_str, sizeof(buffer));
            return 1;
        }
    }

    return 0;
}

// OFFSET: 0x00654280/0x1402F49C0
void ConsoleCommandPrintAllMatching(const char* partial, bool substring) {
    CONSOLECOMMAND* cmd = g_consoleCommandHash.m_fulllist.Head();
    uint32_t length = SStrLen(partial);
    TSGrowableArray<const char*> matches;

    if (strchr(partial, ' ')) {
        char arguments[1024];
        CONSOLECOMMAND* argCmd = ParseCommand(partial, NULL, arguments, sizeof(arguments));

        if (argCmd && argCmd->m_completion) {
            TSGrowableArray<const char*> completions;
            int32_t replacePos = 0;
            argCmd->m_completion(arguments, completions, replacePos);
            qsort(completions.m_data, completions.Count(), sizeof(const char*), CompareCompletions);

            for (uint32_t i = 0; i < completions.Count(); i++) {
                ConsolePrintf("%s", completions[i]);
            }
        }

        return;
    }

    for (; cmd; cmd = g_consoleCommandHash.m_fulllist.Next(cmd)) {
        const char* name = cmd->m_key.m_str;

        if (substring ? SStrStrI(name, partial) != NULL : !SStrCmpI(partial, name, length)) {
            matches.New(name);
        }
    }

    qsort(matches.m_data, matches.Count(), sizeof(const char*), CompareCompletions);

    for (uint32_t i = 0; i < matches.Count(); i++) {
        ConsolePrintf("%s", matches[i]);
    }
}

// PROVEN: 5b479189 IDENTICAL/IDENTICAL
// OFFSET: 0x00654450/0x1402F4CC0
void ConsoleCommandWriteHelp(const char* command) {
    ConsoleCommand_Help("help", command);
}

// OFFSET: 0x006545E0
void ConsoleCommandDestroy() {
    g_consoleCommandHash.Clear();
}

// OFFSET: 0x00654C90/0x1402F5870
int32_t ConsoleCommandRegister(const char* command, COMMANDHANDLER handler, CATEGORY category, const char* helpText, COMMANDCOMPLETION completion) {
    if (SStrLen(command) >= sizeof(s_commandName) || g_consoleCommandHash.Ptr(command)) {
        return 0;
    }

    CONSOLECOMMAND* cmd = g_consoleCommandHash.New(command, 0, 0);
    cmd->m_category = category;
    cmd->m_handler = handler;
    cmd->m_helpText = helpText;
    cmd->m_completion = completion;

    return 1;
}

// PROVEN: 75a3a4f5 IDENTICAL/NONE
// OFFSET: 0x00654D60/0x1402F5A10
void ConsoleCommandInitialize() {
    ConsoleCommandRegister("help", ConsoleCommand_Help, CATEGORY_CONSOLE, "Provides help information about a command.", NULL);
}

// OFFSET: 0x00610940
void ConsoleInitializeClientCommand() {
    ConsoleCommandInitialize();
    ConsoleInitializeCommonCommand();
    ConsoleInitializeDebugCommand();
}

// OFFSET: 0x00610950/0x14029AEA0
void ConsoleDestroyClientCommand() {
    ConsoleCommandDestroy();
}

// PROVEN: 091d27c9 IDENTICAL/NONE
// OFFSET: 0x00610990/0x14029AEF0
void ConsoleInitializeCommonCommand() {
    ConsoleCommandRegister("quit", ConsoleCommand_Quit, CATEGORY_DEFAULT, NULL, NULL);
    ConsoleCommandRegister("ver", ConsoleCommand_Ver, CATEGORY_DEFAULT, NULL, NULL);
}

// PROVEN: e79a0b38 IDENTICAL/IDENTICAL
// OFFSET: [0x00401000/0x1407D7E80]
void ConsoleInitializeDebugCommand() {
}

// OFFSET: 0x00656FD0/0x1402F8B40
int32_t AddLineToExecFile(const char* line) {
    if (!s_execState) {
        if (*line == 'n') {
            ConsoleWrite("Canceled File Creation", ECHO_COLOR);
            s_execState = 4;
            return 0;
        }

        if (*line == 'y') {
            ConsoleWrite("Begin Typing the commands", ECHO_COLOR);
            s_execState = 1;
            return 0;
        }

        ConsoleWrite("You must type 'y' to confirm overwrite. Process aborted!", ERROR_COLOR);
        s_execState = 4;
        return 0;
    }

    if (!SStrCmpI(line, "end", STORM_MAX_STR)) {
        if (s_execState != 2) {
            s_execState = 3;
        }

        return 1;
    }

    char text[260];
    SStrPrintf(text, sizeof(text), "%s\n", line);

    if (sizeof(s_execBuffer) - 1 - SStrLen(s_execBuffer) != SStrLen(text)) {
        SStrPack(s_execBuffer, text, sizeof(s_execBuffer));
    }

    return 0;
}

// PROVEN: 36217e95 IDENTICAL/DIFFERENT
// OFFSET: 0x00656F70/0x1402F8AE0
void AddToHistory(const char* command) {
    SStrCopy(g_commandHistory[g_commandHistoryIndex], command, sizeof(g_commandHistory[0]));
    g_commandHistoryIndex = (g_commandHistoryIndex + 1) & 31;
}

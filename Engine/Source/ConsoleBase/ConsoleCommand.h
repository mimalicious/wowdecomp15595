#ifndef ENGINE_SOURCE_CONSOLEBASE_CONSOLECOMMAND_H
#define ENGINE_SOURCE_CONSOLEBASE_CONSOLECOMMAND_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/ConsoleBase/Types.h"
#include "Engine/Source/Storm/H/stpl.h"

struct CONSOLECOMMAND;

typedef int32_t (*COMMANDHANDLER)(const char* command, const char* arguments);
typedef void (*COMMANDCOMPLETION)(const char* arguments, TSGrowableArray<const char*>& matches, int32_t& replacePos);

struct CATEGORYINFO {
    /* 0000 */ CATEGORY m_category;
    /* 0004 */ char m_name[20];
};

struct CONSOLECOMMAND : public TSHashObject<CONSOLECOMMAND, HASHKEY_CONSTSTRI> {
    /* 0018 */ COMMANDHANDLER m_handler;
    /* 001C */ const char* m_helpText;
    /* 0020 */ CATEGORY m_category;
    /* 0024 */ COMMANDCOMPLETION m_completion;

    CONSOLECOMMAND();
};

extern char g_commandHistory[32][1024];
extern int32_t s_execState;
extern uint32_t g_commandHistoryIndex;
extern COMMANDHANDLER g_defaultCommand;
extern TSHashTable<CONSOLECOMMAND, HASHKEY_CONSTSTRI> g_consoleCommandHash;

int32_t ConsoleCommandComplete(const char* partial, const char** previous, int32_t* index, int32_t direction, char (&buffer)[1024]);
void ConsoleCommandDestroy();
void ConsoleCommandInitialize();
void ConsoleCommandPrintAllMatching(const char* partial, bool substring);
int32_t AddLineToExecFile(const char* line);
void AddToHistory(const char* command);
const char* ConsoleCommandHistory(uint32_t index);
uint32_t ConsoleCommandHistoryDepth();
int32_t ConsoleCommandRegister(const char* command, COMMANDHANDLER handler, CATEGORY category, const char* helpText, COMMANDCOMPLETION completion);
void ConsoleCommandUnregister(const char* command);
void ConsoleCommandWriteHelp(const char* command);
void ConsoleDestroyClientCommand();
void ConsoleInitializeClientCommand();
void ConsoleInitializeCommonCommand();
void ConsoleInitializeDebugCommand();
CONSOLECOMMAND* ParseCommand(const char* commandLine, const char** command, char* arguments, uint32_t argsbytes);

#endif

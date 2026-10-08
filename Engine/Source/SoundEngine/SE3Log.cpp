#include <cstdarg>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include "Engine/Source/SoundEngine/SE3.h"
#include "Engine/Source/Os/OsFile.h"
#include "Engine/Source/SoundEngine/fmodex/src/fmod_errors.h"
#include "Engine/Source/Storm/H/storm.h"

// STATIC: 0x00E4E818/0x140D37B00
HSLOG SE3::sm_log;

// STATIC: 0x00E4E81C/0x140D37B08
static uint32_t s_logFlags;

// STATIC: 0x00E4E820
static int32_t s_errorCount;

// PROVEN: 22b93471 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void SE3::SoundKitDefLog(const char* format, ...) {
}

// PROVEN: 2d9dcdfa IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
void SE3::SoundKitObjectLog(SE3SoundKitObject* sko, const char* file, int32_t line, const char* format, ...) {
}

// PROVEN: d4085eca IDENTICAL/IDENTICAL
// OFFSET: 0x00714570/0x1403E6C50
void SE3::Log_Init() {
    OsCreateDirectory("Logs", 0);
    SLogCreate("Logs\\Sound.log", s_logFlags, &SE3::sm_log);
    s_logFlags |= SLOG_FLAG_APPEND;
    OsDeleteFile("Logs\\SESound.log");
}

// OFFSET: 0x007145D0/0x1403E6CC0
void SE3::Log_Write(uint32_t line, const char* file, FMOD_RESULT result, const char* format, ...) {
    char buffer[512];
    char text[512];

    if (s_errorCount < 200) {
        memset(buffer, 0, sizeof(buffer));

        if (strlen(format)) {
            va_list args;
            va_start(args, format);
            SStrVPrintf(buffer, sizeof(buffer), format, args);
            va_end(args);
        }

        if (result != FMOD_OK) {
            SStrPrintf(text, sizeof(text), " -######## FMOD ERROR! (err %d) %s", result, FMOD_ErrorString(result));

            if (result != FMOD_ERR_FILE_NOTFOUND) {
                SLogWrite(SE3::sm_log, text);
            }

            if (strlen(format) && result != FMOD_ERR_FILE_NOTFOUND) {
                SLogWrite(SE3::sm_log, buffer);
            }

            SStrPrintf(text, sizeof(text), "%s(%d)", file, line);

            if (result != FMOD_ERR_FILE_NOTFOUND) {
                SLogWrite(SE3::sm_log, text);
                SLogFlush(SE3::sm_log);
                s_errorCount++;
            }
        } else {
            SLogWrite(SE3::sm_log, buffer);
            SLogFlush(SE3::sm_log);
        }
    }

    if (s_errorCount == 200) {
        SStrPrintf(text, sizeof(text), " -######## TOO MANY ERRORS. NO FURTHER ERRORS WILL BE LOGGED.");
        SLogWrite(SE3::sm_log, text);
        SLogFlush(SE3::sm_log);
        s_errorCount++;
    }
}

// OFFSET: 0x00714770/0x1403E6EA0
void SE3::Log_Write(const char* format, ...) {
    char buffer[512];
    memset(buffer, 0, sizeof(buffer));

    if (strlen(format)) {
        va_list args;
        va_start(args, format);
        SStrVPrintf(buffer, sizeof(buffer), format, args);
        va_end(args);
    }

    SE3::Log_Write(__LINE__, __FILE__, FMOD_OK, buffer);
}

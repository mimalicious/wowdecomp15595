#include <cstdint>
#include "Engine/Source/Base/Handle.h"
#include "Engine/Source/ConsoleBase/IConsole.h"
#include "Engine/Source/Gxu/GxuFont.h"
#include "Engine/Source/Os/TimeManager.h"
#include "Engine/Source/Scrn/Scrn.h"
#include "Engine/Source/Services/SysMessage.h"
#include "Engine/Source/Services/TextBlock.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/Tempest/C3Vector.h"
#include "WoW/Source/WowSysMessageOutput.h"

// STATIC: 0x00D36B50
static bool s_initialized;

// STATIC: 0x00D36B51
static bool s_enabled;

// STATIC: 0x00D36B52
static bool s_hidden;

static bool s_moved;

// STATIC: 0x00D36B54
static HLAYER s_layer;

// STATIC: 0x00D36B58
static CGxStringBatch* s_batch;

static CGxFont* s_font;

static float s_fontHeight;

static uint32_t s_first;

// STATIC: 0x00D36B68
static SYSMESSAGE s_messages[10];

// PROVEN: 5b3651a7 IDENTICAL/NONE
// OFFSET: 0x0040DE00/0x140011580
void WowSysMessageOutput::Enable() {
    if (!s_enabled && s_initialized && s_layer) {
        s_enabled = true;
    }
}

// PROVEN: a282cc5f IDENTICAL/NONE
// OFFSET: 0x0040DE30/0x1400115B0
void WowSysMessageOutput::Disable() {
    if (s_enabled) {
        s_enabled = false;
    }
}

// PROVEN: fa7671f8 IDENTICAL/NONE
// OFFSET: 0x0040DE50/0x1400115C0
void WowSysMessageOutput::Show() {
    if (s_hidden) {
        s_hidden = false;
    }
}

// PROVEN: 71ccff6a IDENTICAL/NONE
// OFFSET: 0x0040DE70/0x1400115D0
void WowSysMessageOutput::Hide() {
    if (!s_hidden) {
        s_hidden = true;
    }
}

// OFFSET: 0x0040DE90/0x1400115F0
void WowSysMessageOutput::Output(const char* text, STATUS_TYPE severity) {
    if (!s_enabled) {
        return;
    }

    SYSMESSAGE& message = s_messages[s_first];
    message.m_time = OsGetAsyncTimeMs();
    SStrCopy(message.m_text, text, sizeof(message.m_text));

    float red;
    float green;
    float blue;
    SysMsgGetSeverityColor(severity, red, green, blue);

    message.m_color.a = 0xFF;
    s_moved = true;
    message.m_color.r = static_cast<uint8_t>(red * 255.0f);
    message.m_color.g = static_cast<uint8_t>(green * 255.0f);
    message.m_color.b = static_cast<uint8_t>(blue * 255.0f);

    s_first = (s_first + 1) % (sizeof(s_messages) / sizeof(s_messages[0]));

    ConsoleWrite(text, severity < STATUS_ERROR ? WARNING_COLOR : ERROR_COLOR);
}

// OFFSET: 0x0040DFA0/0x140011710
void WowSysMessageOutput::Paint(void* param, const RECTF* rect, const RECTF* visible, float elapsedSec) {
    if (s_hidden) {
        return;
    }

    uint32_t now = OsGetAsyncTimeMs();

    for (uint32_t i = 0; i < sizeof(s_messages) / sizeof(s_messages[0]); i++) {
        SYSMESSAGE& message = s_messages[(s_first + i) % (sizeof(s_messages) / sizeof(s_messages[0]))];
        float offset = (i + 12) * 0.025f;
        C3Vector position(rect->left, rect->top - offset, 1.0f);
        bool created = false;

        if (message.m_text[0]) {
            if (message.m_string) {
                GxuFontDestroyString(message.m_string);
            }

            GxuFontCreateString(s_font, message.m_text, 0.0f, position, 1.0f, s_fontHeight, 0.0f, message.m_string, GxVJ_Middle, GxHJ_Left, 0x1F44, message.m_color, -0.45f, 1.0f, 1.0f, 0.0f);
            created = true;
            message.m_text[0] = 0;
        }

        if (!message.m_string) {
            continue;
        }

        uint32_t age = now - message.m_time;

        if (age > 13000) {
            GxuFontDestroyString(message.m_string);
            message.m_string = NULL;

            continue;
        }

        if (age > 10000) {
            message.m_color.a = (13000 - age) / 12;
            GxuFontSetStringColor(message.m_string, message.m_color);
        }

        if (s_moved && !created) {
            C3Vector moved(rect->left, rect->top - offset, 0.0f);
            GxuFontSetStringPosition(message.m_string, moved);
        }

        GxuFontAddToBatch(s_batch, message.m_string);
    }

    GxuFontRenderBatch(s_batch);
    GxuFontClearBatch(s_batch);
    s_moved = false;
}

// OFFSET: 0x0040E190/0x140011A00
void WowSysMessageOutput::Initialize() {
    if (s_initialized) {
        return;
    }

    s_batch = GxuFontCreateBatch(false, false, false);

    if (!s_batch) {
        return;
    }

    s_font = TextBlockGetFontPtr(ScrnGetStockFont(STOCK_PERFFONT));

    if (!s_font) {
        return;
    }

    s_fontHeight = ScrnGetStockFontHeight(STOCK_PERFFONT);

    if (s_fontHeight == 0.0f) {
        return;
    }

    RECTF rect = { 0.0f, 0.0f, 1.0f, 1.0f };
    ScrnLayerCreate(&rect, 10.0f, 7, NULL, WowSysMessageOutput::Paint, &s_layer);

    if (!s_layer) {
        return;
    }

    SysMsgSetCallback(WowSysMessageOutput::Output);
    s_initialized = 1;
}

// OFFSET: 0x0040DD90
void WowSysMessageOutput::Shutdown() {
    if (!s_initialized) {
        return;
    }

    s_initialized = 0;
    SysMsgSetCallback(NULL);

    if (s_batch) {
        GxuFontDestroyBatch(s_batch);
        s_batch = NULL;
    }

    for (uint32_t i = 0; i < sizeof(s_messages) / sizeof(s_messages[0]); i++) {
        if (s_messages[i].m_string) {
            GxuFontDestroyString(s_messages[i].m_string);
        }
    }

    HandleClose(reinterpret_cast<HOBJECT>(s_layer));
}

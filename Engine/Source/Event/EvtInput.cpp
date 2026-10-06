#include <cmath>
#include <cstdint>
#include "Engine/Source/Base/Coordinate.h"
#include "Engine/Source/Event/Event.h"
#include "Engine/Source/Event/EvtContext.h"
#include "Engine/Source/Event/EvtInput.h"
#include "Engine/Source/Event/EvtQueue.h"
#include "Engine/Source/Os/Input.h"
#include "Engine/Source/Os/TimeManager.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/Tempest/C2Vector.h"

namespace Input {
    // STATIC: %0x00D95768
    MOUSEMODE s_mouseMode;

    // STATIC: %0x00D9576C
    uint32_t s_mouseHoldButton;

    // STATIC: %0x00D95770
    uint32_t s_metaKeyState;

    // STATIC: %0x00D95774
    uint32_t s_buttonState;

    // STATIC: %0x00D95780
    CRect s_boundingRect;
}

// STATIC: %0x00D95778
static int32_t (*s_confirmCloseCallback)(void*);

// STATIC: %0x00D9577C
static void* s_confirmCloseParam;

// PROVEN: 66f5f713 IDENTICAL/NONE
// OFFSET: 0x008A2AE0
int32_t IEvtConfirmClose() {
    if (s_confirmCloseCallback) {
        return s_confirmCloseCallback(s_confirmCloseParam);
    }

    return 1;
}

// OFFSET: %0x00807220
void PostString(EvtContext* context, const uint16_t* string, int32_t length) {
    EVENT_DATA_CHAR data;
    data.m_metaKeyState = Input::s_metaKeyState;
    data.m_repeat = 1;

    for (int32_t i = 0; i < length; i++) {
        data.m_ch = string[i];
        IEvtQueueDispatch(context, EVENT_ID_CHAR, &data);
    }
}

// OFFSET: %0x00807270
void IEvtDispatchKeyDown(EvtContext* context, KEY key, uint32_t repeat, uint32_t time) {
    if (static_cast<int32_t>(key) <= KEY_LASTMETAKEY) {
        uint32_t bit = 1 << key;

        if (Input::s_metaKeyState & bit) {
            return;
        }

        Input::s_metaKeyState |= bit;
    }

    EVENT_DATA_KEY data;
    data.m_key = key;
    data.m_metaKeyState = Input::s_metaKeyState;
    data.m_repeat = repeat;
    data.m_time = time;

    IEvtQueueDispatch(context, EVENT_ID_KEYDOWN, &data);
}

// OFFSET: %0x008072C0
void IEvtDispatchKeyUp(EvtContext* context, KEY key, uint32_t repeat, uint32_t time) {
    if (static_cast<int32_t>(key) <= KEY_LASTMETAKEY) {
        uint32_t bit = 1 << key;

        if (!(Input::s_metaKeyState & bit)) {
            return;
        }

        Input::s_metaKeyState &= ~bit;
    }

    EVENT_DATA_KEY data;
    data.m_key = key;
    data.m_metaKeyState = Input::s_metaKeyState;
    data.m_repeat = repeat;
    data.m_time = time;

    IEvtQueueDispatch(context, EVENT_ID_KEYUP, &data);
}

// OFFSET: %0x00807310
void IEvtDispatchMouseMoveRelative(EvtContext* context, int32_t x, int32_t y, uint32_t time) {
    EVENT_DATA_MOUSE data;
    data.m_mode = Input::s_mouseMode;
    data.m_button = MOUSE_BUTTON_NONE;
    data.m_buttonState = Input::s_buttonState;
    data.m_metaKeyState = Input::s_metaKeyState;
    data.m_flags = Input::s_mouseMode == MOUSE_MODE_RELATIVE ? 2 : 0;
    data.m_x = static_cast<float>(x);
    data.m_y = static_cast<float>(y);
    data.m_time = time;

    IEvtQueueDispatch(context, EVENT_ID_MOUSEMOVE_RELATIVE, &data);
}

// OFFSET: %0x00807380
void PostMouseModeChanged(EvtContext* context, MOUSEMODE mode) {
    EVENT_DATA_MOUSE data = {};
    data.m_buttonState = Input::s_buttonState;

    Input::s_mouseMode = mode;

    data.m_mode = mode;
    data.m_metaKeyState = Input::s_metaKeyState;
    data.m_flags = mode == MOUSE_MODE_RELATIVE ? 2 : 0;

    IEvtQueueDispatch(context, EVENT_ID_MOUSEMODE_CHANGED, &data);
}

// OFFSET: %0x008073F0
void IEvtDispatchJoystickAxis(EvtContext* context, int32_t id, int32_t value, uint32_t time) {
    EVENT_DATA_JOYSTICK data;
    data.unk_0000 = 0;
    data.m_id = id;
    data.m_value = value < 0 ? -(value * -(1.0f / 32768.0f)) : value * (1.0f / 32767.0f);
    data.m_time = time;

    IEvtQueueDispatch(context, EVENT_ID_18, &data);
}

// PROVEN: f19241a6 IDENTICAL/NONE
// OFFSET: 0x008A2D20
void IEvtInputDestroy() {
    OsInputDestroy();
    Input::s_buttonState = 0;
    Input::s_metaKeyState = 0;
}

// PROVEN: eadd608e IDENTICAL/IDENTICAL
// OFFSET: 0x008A2D40/0x1405E4DB0
void IEvtInputInitialize() {
    OsInputInitialize();
}

// OFFSET: %0x00807470
void IEvtInputSetMouseMode(EvtContext* context, MOUSEMODE mode, uint32_t holdButton) {
    if (!context) {
        DebugValidate("context");
        SErrSetLastError(STORM_ERROR_INVALID_PARAMETER);
        return;
    }

    if (holdButton != (Input::s_buttonState & holdButton)) {
        return;
    }

    Input::s_mouseHoldButton = holdButton;

    if (mode != Input::s_mouseMode) {
        OsInputSetMouseMode(mode == MOUSE_MODE_NORMAL ? OS_MOUSE_MODE_NORMAL : OS_MOUSE_MODE_RELATIVE);
        PostMouseModeChanged(context, mode);
    }
}

// PROVEN: ffcf6f74 IDENTICAL/IDENTICAL
// OFFSET: 0x008A2DC0/0x1405E4EA0
void IEvtInputSetConfirmCloseCallback(int32_t (*callback)(void*), void* param) {
    s_confirmCloseCallback = callback;
    s_confirmCloseParam = param;
}

// OFFSET: %0x00807500
void ConvertPosition(int32_t x, int32_t y, float* outX, float* outY) {
    if (Input::s_boundingRect.maxX - Input::s_boundingRect.minX != 0.0f && Input::s_boundingRect.maxY - Input::s_boundingRect.minY != 0.0f) {
        C2Vector point;
        point.x = static_cast<float>(x);
        point.y = static_cast<float>(y);

        if (!Input::s_boundingRect.ContainsPoint(point)) {
            float minX = Input::s_boundingRect.minX + 1.0f;
            float maxX = Input::s_boundingRect.maxX - 1.0f;
            float clampX = point.x > minX ? point.x : minX;
            clampX = clampX < maxX ? clampX : maxX;

            float minY = Input::s_boundingRect.minY + 1.0f;
            float maxY = Input::s_boundingRect.maxY - 1.0f;
            float clampY = point.y > minY ? point.y : minY;
            clampY = clampY < maxY ? clampY : maxY;

            x = static_cast<int32_t>(clampX);
            y = static_cast<int32_t>(clampY);

            OsInputSetMousePosition(x, y);
        }
    }

    tagRECT windowRect;
    OsGetDefaultWindowRect(&windowRect);

    *outX = static_cast<float>(x) / static_cast<float>(windowRect.right - windowRect.left);
    *outY = 1.0f - static_cast<float>(y) / static_cast<float>(windowRect.bottom - windowRect.top);
}

// OFFSET: %0x00807640
void IEvtDispatchFocusChanged(EvtContext* context, int32_t focus) {
    Input::s_buttonState = 0;
    Input::s_metaKeyState = 0;

    EVENT_DATA_FOCUS data;
    data.m_focus = focus;

    IEvtQueueDispatch(context, EVENT_ID_FOCUS, &data);

    if (Input::s_mouseHoldButton && Input::s_mouseHoldButton != (Input::s_buttonState & Input::s_mouseHoldButton)) {
        EventSetMouseMode(MOUSE_MODE_NORMAL, 0);
    }
}

// OFFSET: %0x00807690
void IEvtDispatchMouseDown(EvtContext* context, MOUSEBUTTON button, int32_t x, int32_t y, uint32_t time) {
    Input::s_buttonState |= button;

    EVENT_DATA_MOUSE data;
    data.m_mode = Input::s_mouseMode;
    data.m_button = button;
    data.m_buttonState = Input::s_buttonState;
    data.m_metaKeyState = Input::s_metaKeyState;
    data.m_flags = Input::s_mouseMode == MOUSE_MODE_RELATIVE ? 2 : 0;
    ConvertPosition(x, y, &data.m_x, &data.m_y);
    data.m_time = time;

    IEvtQueueDispatch(context, EVENT_ID_MOUSEDOWN, &data);
}

// OFFSET: %0x00807700
void IEvtDispatchMouseMove(EvtContext* context, int32_t x, int32_t y, uint32_t time) {
    EVENT_DATA_MOUSE data;
    data.m_mode = Input::s_mouseMode;
    data.m_button = MOUSE_BUTTON_NONE;
    data.m_buttonState = Input::s_buttonState;
    data.m_metaKeyState = Input::s_metaKeyState;
    data.m_flags = Input::s_mouseMode == MOUSE_MODE_RELATIVE ? 2 : 0;
    data.m_time = time;
    ConvertPosition(x, y, &data.m_x, &data.m_y);

    IEvtQueueDispatch(context, EVENT_ID_MOUSEMOVE, &data);
}

// OFFSET: %0x00807770
void IEvtDispatchMouseEvent(EvtContext* context, MOUSEBUTTON button, int32_t x, int32_t y, uint32_t flags, uint32_t time) {
    Input::s_buttonState &= ~button;

    EVENT_DATA_MOUSE data;
    data.m_mode = Input::s_mouseMode;
    data.m_button = button;
    data.m_buttonState = Input::s_buttonState;
    data.m_metaKeyState = Input::s_metaKeyState;
    data.m_flags = (Input::s_mouseMode == MOUSE_MODE_RELATIVE ? 2 : 0) | flags;
    ConvertPosition(x, y, &data.m_x, &data.m_y);
    data.m_time = time;

    IEvtQueueDispatch(context, EVENT_ID_MOUSEUP, &data);

    if (Input::s_mouseHoldButton && Input::s_mouseHoldButton != (Input::s_buttonState & Input::s_mouseHoldButton)) {
        EventSetMouseMode(MOUSE_MODE_NORMAL, 0);
    }
}

// OFFSET: %0x00807810
void IEvtDispatchMouseWheel(EvtContext* context, int32_t wheelDistance, int32_t x, int32_t y, uint32_t time) {
    EVENT_DATA_MOUSE data;
    data.m_mode = Input::s_mouseMode;
    data.m_button = MOUSE_BUTTON_NONE;
    data.m_buttonState = Input::s_buttonState;
    data.m_metaKeyState = Input::s_metaKeyState;
    data.m_flags = Input::s_mouseMode == MOUSE_MODE_RELATIVE ? 2 : 0;
    data.m_wheelDistance = wheelDistance;
    ConvertPosition(x, y, &data.m_x, &data.m_y);
    data.m_time = time;

    IEvtQueueDispatch(context, EVENT_ID_MOUSEWHEEL, &data);
}

// OFFSET: %0x00807880
void IEvtDispatchJoystickStick(EvtContext* context, int32_t id, int32_t x, int32_t y, uint32_t time) {
    float stickX = x < 0 ? -(x * -(1.0f / 32768.0f)) : x * (1.0f / 32767.0f);
    float stickY = y < 0 ? -(y * -(1.0f / 32768.0f)) : y * (1.0f / 32767.0f);

    EVENT_DATA_JOYSTICK data;
    data.unk_0000 = 0;
    data.m_id = id;
    data.m_value = sqrtf(stickY * stickY + stickX * stickX);
    data.m_angle = 3.1415927f - atan2f(stickX, stickY);
    data.m_x = stickX;
    data.m_y = stickY;
    data.m_time = time;

    IEvtQueueDispatch(context, EVENT_ID_17, &data);
}

// OFFSET: 0x008A3210/0x1405E5380
void IEvtInputGetMousePosition(float* x, float* y) {
    int32_t mouseX;
    int32_t mouseY;
    OsInputGetMousePosition(&mouseX, &mouseY);

    float ndcX;
    float ndcY;
    ConvertPosition(mouseX, mouseY, &ndcX, &ndcY);

    NDCToDDC(ndcX, ndcY, x, y);
}

// OFFSET: %0x00807980
void IEvtDispatchMouseEvents(EvtContext* context, int32_t x, int32_t y) {
    while (Input::s_buttonState) {
        uint32_t button = Input::s_buttonState & (Input::s_buttonState ^ (Input::s_buttonState - 1));
        IEvtDispatchMouseEvent(context, static_cast<MOUSEBUTTON>(button), x, y, 1, OsGetAsyncTimeMs());
    }
}

// OFFSET: %0x008079C0
void IEvtDispatchInput(EvtContext* context, OSINPUT id, int32_t* param, int32_t* shutdown) {
    if (!context) {
        DebugValidate("context");
        SErrSetLastError(STORM_ERROR_INVALID_PARAMETER);
        return;
    }

    switch (id) {
        case OS_INPUT_CAPTURE_CHANGED:
            IEvtDispatchMouseEvents(context, param[1], param[2]);
            break;

        case OS_INPUT_CHAR: {
            EVENT_DATA_CHAR data;
            data.m_ch = param[0];
            data.m_metaKeyState = Input::s_metaKeyState;
            data.m_repeat = param[1];

            IEvtQueueDispatch(context, EVENT_ID_CHAR, &data);
            break;
        }

        case OS_INPUT_STRING:
            PostString(context, reinterpret_cast<const uint16_t*>(static_cast<intptr_t>(param[0])), param[1]);
            break;

        case OS_INPUT_IME: {
            EVENT_DATA_IME data;
            data.m_action = param[0];
            data.unk_0004 = param[1];
            data.m_param = param[2];
            data.m_codePage = GetACP();

            IEvtQueueDispatch(context, EVENT_ID_IME, &data);
            break;
        }

        case OS_INPUT_SIZE: {
            EVENT_DATA_SIZE data;
            data.m_w = param[0];
            data.m_h = param[1];

            IEvtQueueDispatch(context, EVENT_ID_SIZE, &data);
            break;
        }

        case OS_INPUT_CLOSE:
            if (IEvtConfirmClose()) {
                EventInitiateShutdown();
                *shutdown = 1;
            }

            break;

        case OS_INPUT_FOCUS:
            IEvtDispatchFocusChanged(context, param[0]);
            break;

        case OS_INPUT_KEY_DOWN:
            IEvtDispatchKeyDown(context, static_cast<KEY>(param[0]), param[1], param[3]);
            break;

        case OS_INPUT_KEY_UP:
            IEvtDispatchKeyUp(context, static_cast<KEY>(param[0]), param[1], param[3]);
            break;

        case OS_INPUT_MOUSE_DOWN:
            IEvtDispatchMouseDown(context, static_cast<MOUSEBUTTON>(param[0]), param[1], param[2], param[3]);
            break;

        case OS_INPUT_MOUSE_MOVE:
            IEvtDispatchMouseMove(context, param[1], param[2], param[3]);
            break;

        case OS_INPUT_MOUSE_WHEEL:
            IEvtDispatchMouseWheel(context, param[0], param[1], param[2], param[3]);
            break;

        case OS_INPUT_MOUSE_MOVE_RELATIVE:
            IEvtDispatchMouseMoveRelative(context, param[1], param[2], param[3]);
            break;

        case OS_INPUT_MOUSE_UP:
            IEvtDispatchMouseEvent(context, static_cast<MOUSEBUTTON>(param[0]), param[1], param[2], 0, param[3]);
            break;

        case OS_INPUT_14:
            IEvtDispatchJoystickStick(context, param[0], param[1], param[2], param[3]);
            break;

        case OS_INPUT_15:
            IEvtDispatchJoystickAxis(context, param[0], param[1], param[3]);
            break;

        case OS_INPUT_16: {
            EVENT_DATA_JOYSTICK data;
            data.unk_0000 = 0;
            data.m_id = param[0];
            data.m_state = 1;
            data.m_time = param[3];

            IEvtQueueDispatch(context, EVENT_ID_19, &data);
            break;
        }

        case OS_INPUT_17: {
            EVENT_DATA_JOYSTICK data;
            data.unk_0000 = 0;
            data.m_id = param[0];
            data.m_state = 0;
            data.m_time = param[3];

            IEvtQueueDispatch(context, EVENT_ID_20, &data);
            break;
        }

        case OS_INPUT_18: {
            EVENT_DATA_JOYSTICK data;
            data.unk_0000 = 0;
            data.m_id = param[0];
            data.m_state = param[1];
            data.m_time = param[3];

            IEvtQueueDispatch(context, EVENT_ID_21, &data);
            break;
        }

        case OS_INPUT_SHUTDOWN:
            if (IEvtConfirmClose()) {
                *shutdown = 1;
            }

            break;
    }
}

// OFFSET: %0x00807CA0
int32_t IEvtInputProcess(EvtContext* context, int32_t* shutdown) {
    if (!context) {
        DebugValidate("context");
        SErrSetLastError(STORM_ERROR_INVALID_PARAMETER);
        return 0;
    }

    int32_t result = 0;
    OSINPUT id;
    int32_t param[4];

    while (OsInputGet(&id, &param[0], &param[1], &param[2], &param[3])) {
        result = 1;
        IEvtDispatchInput(context, id, param, shutdown);
    }

    return result;
}

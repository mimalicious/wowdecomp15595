#include <cctype>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <malloc.h>
#include <new>
#include "Engine/Source/Gx/CGxDeviceOpenGL/CGxDeviceOpenGl.h"
#include "Engine/Source/Gx/CGxDeviceOpenGL/CGxOglTables.h"
#include "Engine/Source/Gx/CGxDevice/Win32/GxWindow.h"
#include "Engine/Source/Gx/CGxDeviceOpenGL/CGxVboBroker1.h"
#include "Engine/Source/Gx/CGxDeviceOpenGL/Types.h"
#include "Engine/Source/Gx/Blit.h"
#include "Engine/Source/Gx/Buffer.h"
#include "Engine/Source/Gx/CGxBuf.h"
#include "Engine/Source/Gx/CGxPool.h"
#include "Engine/Source/Gx/CGxTex.h"
#include "Engine/Source/Gx/Gx.h"
#include "Engine/Source/Storm/H/storm.h"

// OFFSET: 0x0086AA10/0x14059E870
static int32_t SetupPixelFormat(HDC dc, const CGxFormat& format) {
    if (format.m_multisampleCount > 1) {
        return 0;
    }

    int32_t match = 0;
    uint8_t colorBits = 16;
    uint8_t blueBits = 5;
    uint8_t greenBits = 6;
    uint8_t redBits = 5;
    uint8_t alphaBits = 0;
    uint8_t depthBits = 16;
    uint8_t stencilBits = 0;

    switch (format.m_colorFormat) {
        case CGxFormat::Fmt_Rgb565:
            colorBits = 16;
            blueBits = 5;
            greenBits = 6;
            redBits = 5;
            alphaBits = 0;
            break;

        case CGxFormat::Fmt_ArgbX888:
            colorBits = 24;
            redBits = 8;
            greenBits = 8;
            blueBits = 8;
            alphaBits = 0;
            break;

        case CGxFormat::Fmt_Argb8888:
            colorBits = 24;
            redBits = 8;
            greenBits = 8;
            blueBits = 8;
            alphaBits = 8;
            break;

        case CGxFormat::Fmt_Argb2101010:
            colorBits = 30;
            redBits = 10;
            greenBits = 10;
            blueBits = 10;
            alphaBits = 2;
            break;

        default:
            break;
    }

    switch (format.m_depthFormat) {
        case CGxFormat::Fmt_Ds160:
            depthBits = 16;
            stencilBits = 0;
            break;

        case CGxFormat::Fmt_Ds24X:
            depthBits = 24;
            stencilBits = 0;
            break;

        case CGxFormat::Fmt_Ds248:
            depthBits = 24;
            stencilBits = 8;
            break;

        case CGxFormat::Fmt_Ds320:
            depthBits = 32;
            stencilBits = 0;
            break;

        default:
            break;
    }

    PIXELFORMATDESCRIPTOR pfd;
    pfd.cAlphaBits = alphaBits;
    pfd.cColorBits = colorBits;
    pfd.cDepthBits = depthBits;
    pfd.nSize = sizeof(pfd);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cRedBits = 0;
    pfd.cRedShift = 0;
    pfd.cGreenBits = 0;
    pfd.cGreenShift = 0;
    pfd.cBlueBits = 0;
    pfd.cBlueShift = 0;
    pfd.cAlphaShift = 0;
    pfd.cAccumBits = 0;
    pfd.cAccumRedBits = 0;
    pfd.cAccumGreenBits = 0;
    pfd.cAccumBlueBits = 0;
    pfd.cAccumAlphaBits = 0;
    pfd.cStencilBits = stencilBits;
    memset(&pfd.cAuxBuffers, 0, 15);

    int32_t pixelFormat = ChoosePixelFormat(dc, &pfd);

    if (!pixelFormat) {
        CGxDevice::Log("ChoosePixelFormat failed, color = %d, alpha = %d, depth = %d\n", colorBits, alphaBits, depthBits);
        return 0;
    }

    DescribePixelFormat(dc, pixelFormat, sizeof(pfd), &pfd);

    int32_t matches = 1;

    if (!format.m_window) {
        if (format.m_colorFormat != CGxFormat::Fmt_ArgbX888) {
            matches = pfd.cAlphaBits == alphaBits;
        }

        if (pfd.cRedBits == redBits && pfd.cBlueBits == blueBits && pfd.cGreenBits == greenBits) {
            match = 1;
        }

        matches &= match;
    }

    if (format.m_depthFormat != CGxFormat::Fmt_Ds24X) {
        matches &= pfd.cStencilBits == stencilBits;
    }

    if (!((pfd.cDepthBits == depthBits) & matches)) {
        CGxDevice::Log("SetupPixelFormat(): pfd does not match requested");
        return 0;
    }

    if (SetPixelFormat(dc, pixelFormat, &pfd)) {
        return 1;
    }

    CGxDevice::Log("SetPixelFormat failed, color = %d, alpha = %d, depth = %d, fmt = %d\n", colorBits, alphaBits, depthBits, pixelFormat);
    return 0;
}

// PROVEN: f8393034 IDENTICAL/IDENTICAL
// OFFSET: 0x0086AC60/0x14059EB60
BOOL WglDestroyContext(HGLRC context) {
    wglMakeCurrent(NULL, NULL);
    return wglDeleteContext(context);
}

// OFFSET: 0x0086AC80/0x14059EB90
static int32_t SetupPixelFormatWGL(HDC dc, const CGxFormat& format) {
    int32_t* attributes = static_cast<int32_t*>(alloca(512));
    attributes[0] = WGL_DRAW_TO_WINDOW_ARB;
    attributes[1] = 1;
    attributes[2] = WGL_ACCELERATION_ARB;
    attributes[3] = WGL_FULL_ACCELERATION_ARB;
    attributes[4] = WGL_SUPPORT_OPENGL_ARB;
    attributes[5] = 1;
    attributes[6] = WGL_DOUBLE_BUFFER_ARB;
    attributes[7] = 1;

    int32_t alphaBits = 0;
    int32_t redBits = 0;
    int32_t greenBits = 0;
    int32_t blueBits = 0;

    switch (format.m_colorFormat) {
        case CGxFormat::Fmt_Rgb565:
            greenBits = 6;
            alphaBits = 0;
            blueBits = 5;
            redBits = 5;
            break;

        case CGxFormat::Fmt_ArgbX888:
            greenBits = 8;
            alphaBits = 0;
            blueBits = 8;
            redBits = 8;
            break;

        case CGxFormat::Fmt_Argb8888:
            alphaBits = 8;
            blueBits = 8;
            greenBits = 8;
            redBits = 8;
            break;

        default:
            break;
    }

    attributes[8] = WGL_PIXEL_TYPE_ARB;
    attributes[9] = WGL_TYPE_RGBA_ARB;
    attributes[10] = WGL_RED_BITS_ARB;
    attributes[11] = redBits;
    attributes[12] = WGL_GREEN_BITS_ARB;
    attributes[13] = greenBits;
    attributes[14] = WGL_BLUE_BITS_ARB;
    attributes[15] = blueBits;
    attributes[16] = WGL_ALPHA_BITS_ARB;
    attributes[17] = alphaBits;

    int32_t depthBits = 0;
    int32_t stencilBits = 0;

    switch (format.m_depthFormat) {
        case CGxFormat::Fmt_Ds160:
            depthBits = 16;
            stencilBits = 0;
            break;

        case CGxFormat::Fmt_Ds24X:
            depthBits = 24;
            stencilBits = 0;
            break;

        case CGxFormat::Fmt_Ds248:
            depthBits = 24;
            stencilBits = 8;
            break;

        case CGxFormat::Fmt_Ds320:
            depthBits = 32;
            stencilBits = 0;
            break;

        default:
            break;
    }

    attributes[18] = WGL_DEPTH_BITS_ARB;
    attributes[19] = depthBits;
    attributes[20] = WGL_STENCIL_BITS_ARB;
    attributes[21] = stencilBits;

    uint32_t count = 22;

    if (glARBMultisample && format.m_multisampleCount > 1) {
        attributes[22] = WGL_SAMPLE_BUFFERS_ARB;
        attributes[23] = 1;
        attributes[24] = WGL_SAMPLES_ARB;
        count = 26;
        attributes[25] = format.m_multisampleCount;

        if (glNVMultisampleFilterHint) {
            glHint(GL_MULTISAMPLE_FILTER_HINT_NV, format.m_multisampleQuality <= 0.5f ? GL_FASTEST : GL_NICEST);
        }
    }

    attributes[count] = 0;
    attributes[count + 1] = 0;

    int32_t pixelFormat;
    UINT numFormats;

    if (!wglChoosePixelFormatARB(dc, attributes, NULL, 1, &pixelFormat, &numFormats)) {
        return 0;
    }

    int32_t matches = 1;

    if (!numFormats) {
        return 0;
    }

    int32_t attribute;
    int32_t value;

    if (!format.m_window) {
        if (format.m_colorFormat != CGxFormat::Fmt_ArgbX888) {
            attribute = WGL_ALPHA_BITS_ARB;
            matches = wglGetPixelFormatAttribivARB(dc, pixelFormat, 0, 1, &attribute, &value) && value == alphaBits;
        }

        attribute = WGL_RED_BITS_ARB;
        int32_t red = wglGetPixelFormatAttribivARB(dc, pixelFormat, 0, 1, &attribute, &value) && value == redBits;
        matches = red & matches;

        attribute = WGL_GREEN_BITS_ARB;
        int32_t green = wglGetPixelFormatAttribivARB(dc, pixelFormat, 0, 1, &attribute, &value) && value == greenBits;
        matches = green & matches;

        attribute = WGL_BLUE_BITS_ARB;
        int32_t blue = wglGetPixelFormatAttribivARB(dc, pixelFormat, 0, 1, &attribute, &value) && value == blueBits;
        matches = blue & matches;
    }

    if (format.m_depthFormat != CGxFormat::Fmt_Ds24X) {
        attribute = WGL_STENCIL_BITS_ARB;
        int32_t stencil = wglGetPixelFormatAttribivARB(dc, pixelFormat, 0, 1, &attribute, &value) && value == stencilBits;
        matches &= stencil;
    }

    attribute = WGL_DEPTH_BITS_ARB;
    int32_t depth = wglGetPixelFormatAttribivARB(dc, pixelFormat, 0, 1, &attribute, &value) && value == depthBits;
    matches = depth & matches;

    if (glARBMultisample && format.m_multisampleCount > 1) {
        attribute = WGL_SAMPLES_ARB;
        int32_t samples = wglGetPixelFormatAttribivARB(dc, pixelFormat, 0, 1, &attribute, &value) && value == static_cast<int32_t>(format.m_multisampleCount);
        matches &= samples;
    }

    if (!matches) {
        CGxDevice::Log("SetupPixelFormatWGL(): pixelFormat does not match requested");
        return 0;
    }

    PIXELFORMATDESCRIPTOR pfd;
    memset(&pfd.nVersion, 0, sizeof(pfd) - sizeof(pfd.nSize));
    pfd.nSize = sizeof(pfd);

    return SetPixelFormat(dc, pixelFormat, &pfd);
}

// OFFSET: 0x0086B050/0x14059F010
HGLRC CreateGLContext(HDC dc, const CGxFormat& format) {
    int32_t result;

    if (wglArbPixelFormat) {
        result = SetupPixelFormatWGL(dc, format);
    } else {
        result = SetupPixelFormat(dc, format);
    }

    if (!result) {
        return NULL;
    }

    return wglCreateContext(dc);
}

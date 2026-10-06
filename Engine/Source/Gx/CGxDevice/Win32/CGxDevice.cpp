#if defined(WOWDECOMP_ARCH_32)

#include <cctype>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <windows.h>
#include <d3d9.h>
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "Engine/Source/Gx/CGxDevice/Win32/GxWindow.h"
#include "Engine/Source/Gx/Gx.h"
#include "Engine/Source/Gx/CGxDeviceD3d/CGxDeviceD3d.h"

// STATIC: #0x00AD8CE8
static D3DMULTISAMPLE_TYPE s_d3dMultisampleTypes[9] = { D3DMULTISAMPLE_NONE, D3DMULTISAMPLE_2_SAMPLES, D3DMULTISAMPLE_4_SAMPLES, D3DMULTISAMPLE_6_SAMPLES, D3DMULTISAMPLE_8_SAMPLES, D3DMULTISAMPLE_10_SAMPLES, D3DMULTISAMPLE_12_SAMPLES, D3DMULTISAMPLE_14_SAMPLES, D3DMULTISAMPLE_16_SAMPLES };

// STATIC: #0x00AD8D0C
static D3DFORMAT s_d3dDepthFormats[4] = { D3DFMT_D16, D3DFMT_D24X8, D3DFMT_D24S8, D3DFMT_D32 };

// STATIC: #0x00AD8D1C
static D3DFORMAT s_d3dAdapterFormats[3] = { D3DFMT_R5G6B5, D3DFMT_X8R8G8B8, D3DFMT_A8R8G8B8 };

// STATIC: #0x00AD8D28
static D3DMULTISAMPLE_TYPE s_d3d9ExMultisampleTypes[9] = { D3DMULTISAMPLE_NONE, D3DMULTISAMPLE_2_SAMPLES, D3DMULTISAMPLE_4_SAMPLES, D3DMULTISAMPLE_6_SAMPLES, D3DMULTISAMPLE_8_SAMPLES, D3DMULTISAMPLE_10_SAMPLES, D3DMULTISAMPLE_12_SAMPLES, D3DMULTISAMPLE_14_SAMPLES, D3DMULTISAMPLE_16_SAMPLES };

// STATIC: #0x00AD8D4C
static D3DFORMAT s_d3d9ExDepthFormats[4] = { D3DFMT_D16, D3DFMT_D24X8, D3DFMT_D24S8, D3DFMT_D32 };

// STATIC: #0x00AD8D5C
static D3DFORMAT s_d3d9ExAdapterFormats[3] = { D3DFMT_R5G6B5, D3DFMT_X8R8G8B8, D3DFMT_A8R8G8B8 };

// OFFSET: %0x007A9DE0
int32_t CGxDevice::AdjustWindowRectToWorkArea(uint32_t monitor, float aspectRatio, C2iVector* adjust, tagRECT* rect) {
    MONITORINFO info;
    info.cbSize = sizeof(info);

    if (!GetMonitorInfoA(static_cast<HMONITOR>(CGxDevice::AdapterGetMonitorHandle(monitor)), &info)) {
        return 0;
    }

    int32_t height = rect->bottom - rect->top;
    int32_t width = rect->right - rect->left;

    rect->top = info.rcWork.top;
    rect->left = info.rcWork.left;
    rect->right = width + info.rcWork.left + adjust->x;
    rect->bottom = height + info.rcWork.top + adjust->y;

    if (rect->right > info.rcWork.right) {
        rect->right = info.rcWork.right;

        if (aspectRatio != 0.0f) {
            int32_t workWidth = info.rcWork.right - info.rcWork.left;
            int32_t maxWidth = workWidth - adjust->x;
            int32_t newWidth = maxWidth;
            float invAspect = 1.0f / aspectRatio;

            for (; newWidth >= width / 2; newWidth--) {
                float newHeight = newWidth * invAspect;

                if (newHeight - floor(newHeight) < 0.01f) {
                    break;
                }
            }

            double newHeight;

            if (newWidth < width / 2) {
                newHeight = maxWidth / aspectRatio + 0.5f;
            } else {
                workWidth = newWidth + adjust->x;
                newHeight = newWidth * invAspect;
            }

            rect->right = info.rcWork.left + workWidth;
            rect->bottom = info.rcWork.top + static_cast<int32_t>(adjust->y + floor(newHeight));
        }
    }

    if (rect->bottom > info.rcWork.bottom) {
        rect->bottom = info.rcWork.bottom;

        if (aspectRatio != 0.0f) {
            int32_t workHeight = info.rcWork.bottom - info.rcWork.top;
            int32_t maxHeight = workHeight - adjust->y;
            int32_t newHeight = maxHeight;

            for (; newHeight >= height / 2; newHeight--) {
                float newWidth = newHeight * aspectRatio;

                if (newWidth - floor(newWidth) < 0.01f) {
                    break;
                }
            }

            int32_t newWidth;

            if (newHeight < height / 2) {
                newWidth = static_cast<int32_t>(floor(maxHeight * aspectRatio + 0.5f));
            } else {
                workHeight = adjust->y + newHeight;
                newWidth = static_cast<int32_t>(floor(newHeight * aspectRatio));
            }

            rect->right = info.rcWork.left + adjust->x + newWidth;
            rect->bottom = info.rcWork.top + workHeight;
        }
    }

    int32_t spaceY = info.rcWork.bottom - rect->bottom;
    int32_t spaceX = info.rcWork.right - rect->right;

    rect->right += spaceX / 2;
    rect->left = spaceX / 2 + info.rcWork.left;
    rect->bottom += spaceY / 2;
    rect->top = spaceY / 2 + info.rcWork.top;
    rect->right -= adjust->x;
    rect->bottom -= adjust->y;

    return 1;
}

// OFFSET: %0x007B0FF0
static uint16_t HToI(const char* h, int32_t count) {
    uint16_t value = 0;

    while (count) {
        char c = *h;
        count--;
        value *= 16;
        h++;

        if (isxdigit(c)) {
            if (isdigit(c)) {
                value = c - '0' + value;
            } else {
                value = value + toupper(c) - 55;
            }
        }
    }

    return value;
}

// OFFSET: #0x00689C10
int32_t CGxDevice::DeviceAdapterID(uint16_t& vendorID, uint16_t& deviceID, uint32_t& driverVersionHi, uint32_t& driverVersionLo) {
    int32_t result = 0;
    vendorID = 0xFFFF;
    deviceID = 0xFFFF;
    driverVersionHi = 0;
    driverVersionLo = 0;

    DISPLAY_DEVICEA device;
    memset(&device, 0, sizeof(device));
    device.cb = sizeof(device);

    for (DWORD i = 0; EnumDisplayDevicesA(NULL, i, &device, 0); i++) {
        if (device.StateFlags & DISPLAY_DEVICE_PRIMARY_DEVICE) {
            if (strlen(device.DeviceID) >= 21) {
                uint16_t vendor = HToI(device.DeviceID + 8, 4);
                uint16_t id = HToI(device.DeviceID + 17, 4);

                if (vendor && id) {
                    vendorID = vendor;
                    deviceID = id;
                    result = 1;
                }
            }

            break;
        }
    }

    if (!result) {
        HINSTANCE d3dLib = NULL;
        LPDIRECT3D9 d3d = NULL;

        if (CGxDeviceD3d::ILoadD3dLib(d3dLib, d3d)) {
            D3DADAPTER_IDENTIFIER9 identifier;

            if (SUCCEEDED(d3d->GetAdapterIdentifier(0, 0, &identifier))) {
                vendorID = static_cast<uint16_t>(identifier.VendorId);
                deviceID = static_cast<uint16_t>(identifier.DeviceId);
                driverVersionHi = identifier.DriverVersion.HighPart;
                driverVersionLo = identifier.DriverVersion.LowPart;
                result = 1;
            }

            CGxDeviceD3d::IUnloadD3dLib(d3dLib, d3d);
        }
    }

    CGxDevice::Log("CGxDevice::DeviceAdapterID(): RET: %d, VID: %x, DID: %x, DVER: %x.%x", result, vendorID, deviceID, driverVersionHi, driverVersionLo);

    return result;
}

// OFFSET: #0x00689DB0
int32_t CGxDevice::DeviceAdapterInfer(uint16_t& deviceID) {
    int32_t result = 0;
    deviceID = 0;

    HINSTANCE d3dLib = NULL;
    LPDIRECT3D9 d3d = NULL;

    if (!CGxDeviceD3d::ILoadD3dLib(d3dLib, d3d)) {
        return 0;
    }

    D3DCAPS9 caps;

    if (SUCCEEDED(d3d->GetDeviceCaps(0, D3DDEVTYPE_HAL, &caps))) {
        if ((caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT) && caps.MaxSimultaneousTextures > 2 && static_cast<uint16_t>(caps.PixelShaderVersion) >= 0x200) {
            deviceID = 3;
        } else if ((caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT) && caps.MaxSimultaneousTextures > 2 && static_cast<uint16_t>(caps.PixelShaderVersion) >= 0x101) {
            deviceID = 2;
        } else if ((caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT) && caps.MaxSimultaneousTextures >= 2) {
            deviceID = 1;
        } else {
            deviceID = 0;
        }

        result = 1;
    }

    CGxDeviceD3d::IUnloadD3dLib(d3dLib, d3d);
    CGxDevice::Log("CGxDevice::DeviceAdapterInfer(): RET: %d, DID: %x", result, deviceID);

    return result;
}

// PROVEN: 2c8ce512 IDENTICAL/NONE
// OFFSET: 0x00849900
static int CGxMonitorModeSort(const void* a, const void* b) {
    const CGxMonitorMode* modeA = static_cast<const CGxMonitorMode*>(a);
    const CGxMonitorMode* modeB = static_cast<const CGxMonitorMode*>(b);

    if (modeA->m_size.x < modeB->m_size.x) {
        return -1;
    }

    if (modeA->m_size.x > modeB->m_size.x) {
        return 1;
    }

    if (modeA->m_size.y < modeB->m_size.y) {
        return -1;
    }

    if (modeA->m_size.y > modeB->m_size.y) {
        return 1;
    }

    if (modeA->m_bpp < modeB->m_bpp) {
        return -1;
    }

    if (modeA->m_bpp > modeB->m_bpp) {
        return 1;
    }

    uint32_t refreshB = modeB->m_refreshRate / modeB->m_refreshRateDenom;
    uint32_t refreshA = modeA->m_refreshRate / modeA->m_refreshRateDenom;

    if (refreshB > refreshA) {
        return -1;
    }

    return refreshB < refreshA;
}

// OFFSET: %0x007B5C00
int32_t CGxDevice::WinAdapterDesktopMode(uint32_t monitor, CGxMonitorMode& mode) {
    HMONITOR handle = static_cast<HMONITOR>(CGxDevice::AdapterGetMonitorHandle(monitor));

    if (!handle) {
        return 0;
    }

    MONITORINFOEXA info;
    info.cbSize = sizeof(info);

    if (!GetMonitorInfoA(handle, &info)) {
        return 0;
    }

    DEVMODEA devMode;
    memset(&devMode, 0, sizeof(devMode));
    devMode.dmSize = sizeof(devMode);

    if (!EnumDisplaySettingsA(info.szDevice, ENUM_CURRENT_SETTINGS, &devMode)) {
        return 0;
    }

    mode.m_size.x = devMode.dmPelsWidth;
    mode.m_size.y = devMode.dmPelsHeight;
    mode.m_refreshRate = devMode.dmDisplayFrequency;
    mode.m_refreshRateDenom = 1;
    mode.m_bpp = devMode.dmBitsPerPel;

    return 1;
}

struct MonitorEnumData {
    int32_t primary;
    TSGrowableArray<void*>* monitors;
};

// OFFSET: %0x007B6540
static BOOL CALLBACK MonitorEnumProc(HMONITOR monitor, HDC dc, LPRECT rect, LPARAM param) {
    MonitorEnumData* data = reinterpret_cast<MonitorEnumData*>(param);

    for (uint32_t i = 0; i < data->monitors->Count(); i++) {
        if ((*data->monitors)[i] == monitor) {
            return TRUE;
        }
    }

    if (data->primary == -1) {
        MONITORINFO info;
        info.cbSize = sizeof(info);

        if (GetMonitorInfoA(monitor, &info) && (info.dwFlags & MONITORINFOF_PRIMARY)) {
            data->primary = data->monitors->Count();
        }
    }

    void* handle = monitor;
    data->monitors->New(handle);

    return TRUE;
}

// OFFSET: %0x007B66A0
int32_t CGxDevice::D3d11AdapterMonitorModes(uint32_t monitor, TSGrowableArray<CGxMonitorMode>& modes) {
    modes.SetCount(0);

    if (!CGxDevice::D3d11EnumMonitorModes(monitor, modes)) {
        return 0;
    }

    qsort(modes.Ptr(), modes.Count(), sizeof(CGxMonitorMode), CGxMonitorModeSort);

    return modes.Count() != 0;
}

// OFFSET: %0x007B66F0
int32_t CGxDevice::WinAdapterMonitorModes(uint32_t monitor, TSGrowableArray<CGxMonitorMode>& modes) {
    modes.SetCount(0);

    HMONITOR handle = static_cast<HMONITOR>(CGxDevice::AdapterGetMonitorHandle(monitor));

    if (!handle) {
        return 0;
    }

    MONITORINFOEXA info;
    info.cbSize = sizeof(info);

    if (!GetMonitorInfoA(handle, &info)) {
        return 0;
    }

    DEVMODEA devMode;
    memset(&devMode, 0, sizeof(devMode));
    devMode.dmDriverExtra = 0;

    for (DWORD i = 0; EnumDisplaySettingsA(info.szDevice, i, &devMode); i++) {
        if (devMode.dmPelsWidth >= 640 && devMode.dmPelsHeight >= 480 && devMode.dmBitsPerPel >= 16 && (!(devMode.dmFields & DM_DISPLAYFIXEDOUTPUT) || devMode.dmDisplayFixedOutput != DMDFO_CENTER)) {
            CGxMonitorMode* mode = modes.New();
            mode->m_size.x = devMode.dmPelsWidth;
            mode->m_size.y = devMode.dmPelsHeight;
            mode->m_bpp = devMode.dmBitsPerPel;
            mode->m_refreshRate = devMode.dmDisplayFrequency;
            mode->m_refreshRateDenom = 1;
        }
    }

    qsort(modes.Ptr(), modes.Count(), sizeof(CGxMonitorMode), CGxMonitorModeSort);

    return modes.Count() != 0;
}

// OFFSET: %0x007B6800
void CGxDevice::AdapterInitMonitors() {
    CGxDevice::s_monitors.Clear();

    void* primary = NULL;
    CGxDevice::s_monitors.New(primary);

    MonitorEnumData data;
    data.primary = -1;
    data.monitors = &CGxDevice::s_monitors;
    EnumDisplayMonitors(NULL, NULL, MonitorEnumProc, reinterpret_cast<LPARAM>(&data));

    if (data.primary != -1) {
        CGxDevice::s_monitors[0] = CGxDevice::s_monitors[data.primary];
    } else if (CGxDevice::s_monitors.Count() > 1) {
        CGxDevice::s_monitors[0] = CGxDevice::s_monitors[1];
    } else {
        CGxDevice::s_monitors.Clear();
    }

    CGxDevice::s_monitorsInit = 1;
}

// PROVEN: 5af231a9 IDENTICAL/IDENTICAL
// OFFSET: 0x00842700/0x1407FDD80
int32_t CGxDevice::AdapterMonitorName(uint32_t monitor, char* name, uint32_t size) {
    return 0;
}

// PROVEN: 20389cc0 IDENTICAL/NONE
// OFFSET: 0x00849960/0x140577A80
void CGxDevice::IWindowClassDestroy(uint16_t* windowClass) {
    UnregisterClassA(reinterpret_cast<LPCSTR>(*windowClass), GetModuleHandleA(NULL));
    *windowClass = 0;
}

// PROVEN: a56170a8 IDENTICAL/NONE
// OFFSET: 0x00849990/0x140577AB0
void CGxDevice::IWindowDestroy(void* window) {
    HWND& hwnd = *static_cast<HWND*>(window);

    if (hwnd) {
        DestroyWindow(hwnd);
    }

    hwnd = NULL;
}

// OFFSET: %0x007B5A60
void CGxDevice::IWindowCursorCreate(void* window, void* icon) {
    HWND hwnd = static_cast<HWND>(window);
    HICON& cursor = *static_cast<HICON*>(icon);

    if (cursor) {
        DestroyIcon(cursor);
        cursor = NULL;
    }

    BITMAPV5HEADER header;
    memset(&header, 0, sizeof(header));
    header.bV5Size = sizeof(header);
    header.bV5Width = 32;
    header.bV5Height = -32;
    header.bV5Planes = 1;
    header.bV5BitCount = 32;
    header.bV5Compression = BI_BITFIELDS;
    header.bV5RedMask = 0x00FF0000;
    header.bV5GreenMask = 0x0000FF00;
    header.bV5BlueMask = 0x000000FF;
    header.bV5AlphaMask = 0xFF000000;

    void* bits = NULL;
    HDC dc = GetDC(hwnd);
    HBITMAP color = CreateDIBSection(dc, reinterpret_cast<BITMAPINFO*>(&header), DIB_RGB_COLORS, &bits, NULL, 0);
    ReleaseDC(hwnd, dc);

    for (uint32_t i = 0; i < 1024; i += 32) {
        for (uint32_t j = 0; j < 32; j++) {
            static_cast<uint32_t*>(bits)[i + j] = CGxDevice::s_cursor[i + j];
        }
    }

    HBITMAP mask = CreateBitmap(32, 32, 1, 1, NULL);

    ICONINFO info;
    info.fIcon = FALSE;
    info.xHotspot = CGxDevice::s_cursorHotspotX;
    info.yHotspot = CGxDevice::s_cursorHotspotY;
    info.hbmMask = mask;
    info.hbmColor = color;
    cursor = CreateIconIndirect(&info);

    SetClassLongA(hwnd, GCL_HCURSOR, reinterpret_cast<LONG>(cursor));
    SetCursor(cursor);
    DeleteObject(mask);
    DeleteObject(color);
}

// PROVEN: 4e0bebef IDENTICAL/NONE
// OFFSET: 0x00849B50/0x140577CD0
void CGxDevice::IWindowCursorDestroy(void* icon) {
    HICON& cursor = *static_cast<HICON*>(icon);

    if (cursor) {
        DestroyIcon(cursor);
        cursor = NULL;
    }
}

// OFFSET: %0x007B5CA0
LRESULT CALLBACK WindowProcGx(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam) {
    CGxDevice* device = reinterpret_cast<CGxDevice*>(GetWindowLongA(hwnd, GWL_USERDATA));

    switch (message) {
        case WM_CREATE:
            SetWindowLongA(hwnd, GWL_USERDATA, reinterpret_cast<LONG>(reinterpret_cast<CREATESTRUCTA*>(lparam)->lpCreateParams));
            return 0;

        case WM_DESTROY:
            device->DeviceWM(GxWM_Destroy, 0, 0);
            return 0;

        case WM_SIZE: {
            CRect rect;
            rect.minY = 0.0f;
            rect.minX = 0.0f;
            rect.maxY = HIWORD(lparam);
            rect.maxX = LOWORD(lparam);

            uint32_t resizeType = 0;

            if (wparam == SIZE_MINIMIZED) {
                resizeType = 1;
            } else if (wparam == SIZE_MAXHIDE) {
                resizeType = 2;
            }

            device->DeviceWM(GxWM_Size, reinterpret_cast<uintptr_t>(&rect), resizeType);
            break;
        }

        case WM_ACTIVATE:
            if (LOWORD(wparam) == WA_INACTIVE) {
                if (!device->IDevIsWindowed()) {
                    CRect rect;
                    rect.minY = 0.0f;
                    rect.minX = 0.0f;
                    rect.maxY = 0.0f;
                    rect.maxX = 0.0f;
                    device->DeviceWM(GxWM_Size, reinterpret_cast<uintptr_t>(&rect), 1);
                }
            } else if (LOWORD(wparam) == WA_ACTIVE && !device->IDevIsWindowed()) {
                CRect rect;
                rect.minY = 0.0f;
                rect.minX = 0.0f;
                rect.maxY = 0.0f;
                rect.maxX = 0.0f;
                GxCapsWindowSizeInScreenCoords(rect);
                device->DeviceWM(GxWM_Size, reinterpret_cast<uintptr_t>(&rect), 3);
            }

            break;

        case WM_SETFOCUS:
            device->DeviceWM(GxWM_SetFocus, 0, 0);
            return 0;

        case WM_KILLFOCUS:
            device->DeviceWM(GxWM_KillFocus, 0, 0);
            return 0;

        case WM_PAINT: {
            PAINTSTRUCT paint;
            BeginPaint(hwnd, &paint);
            EndPaint(hwnd, &paint);
            return 0;
        }

        case WM_ERASEBKGND:
            return 0;

        case WM_SETCURSOR:
            if (!device) {
                return DefWindowProcA(hwnd, message, wparam, lparam);
            }

            if (LOWORD(lparam) == HTCLIENT && device->ISetCursor()) {
                return 1;
            }

            break;

        case WM_DISPLAYCHANGE:
            CGxDevice::AdapterResetMonitors();

            if (device->m_format.m_monitor >= static_cast<int32_t>(CGxDevice::AdapterMonitorCount())) {
                device->m_format.m_monitor = 0;
            }

            device->NotifyOnDisplayChange(device->m_format.m_monitor);

            if (!device->IDevIsWindowed()) {
                CRect rect;
                rect.minY = 0.0f;
                rect.minX = 0.0f;
                rect.maxY = HIWORD(lparam);
                rect.maxX = LOWORD(lparam);

                MONITORINFO info;
                info.cbSize = sizeof(info);

                if (GetMonitorInfoA(static_cast<HMONITOR>(CGxDevice::AdapterGetMonitorHandle(device->m_format.m_monitor)), &info)) {
                    rect.maxX = static_cast<float>(info.rcMonitor.right) - static_cast<float>(info.rcMonitor.left);
                    rect.maxY = static_cast<float>(info.rcMonitor.bottom) - static_cast<float>(info.rcMonitor.top);
                }

                device->DeviceWM(GxWM_DisplayChange, reinterpret_cast<uintptr_t>(&rect), 0);
            }

            break;

        case WM_SYSCOMMAND:
            if (wparam == SC_SCREENSAVE || wparam == SC_MONITORPOWER) {
                return 0;
            }

            break;

        case WM_SIZING: {
            RECT* sizingRect = reinterpret_cast<RECT*>(lparam);

            CiRect rect;
            rect.minX = sizingRect->left;
            rect.minY = sizingRect->top;
            rect.maxX = sizingRect->right;
            rect.maxY = sizingRect->bottom;

            if (rect.maxX - rect.minX <= 0 || rect.maxY - rect.minY <= 0) {
                return 0;
            }

            switch (wparam) {
                case WMSZ_LEFT:
                    CGxDevice::ClampRectLeft(rect);
                    CGxDevice::ClampRectTopByAspect(rect);
                    break;

                case WMSZ_RIGHT:
                    CGxDevice::ClampRectRight(rect);
                    CGxDevice::ClampRectBottomByAspect(rect);
                    break;

                case WMSZ_TOP:
                    CGxDevice::ClampRectTop(rect);
                    CGxDevice::ClampRectLeftByAspect(rect);
                    break;

                case WMSZ_TOPLEFT:
                    CGxDevice::ClampRectTop(rect);
                    CGxDevice::ClampRectLeft(rect);
                    CGxDevice::ClampRectTopByAspect(rect);
                    CGxDevice::ClampRectLeftByAspect(rect);
                    break;

                case WMSZ_TOPRIGHT:
                    CGxDevice::ClampRectTop(rect);
                    CGxDevice::ClampRectRight(rect);
                    CGxDevice::ClampRectTopByAspect(rect);
                    CGxDevice::ClampRectRightByAspect(rect);
                    break;

                case WMSZ_BOTTOM:
                    CGxDevice::ClampRectBottom(rect);
                    CGxDevice::ClampRectRightByAspect(rect);
                    break;

                case WMSZ_BOTTOMLEFT:
                    CGxDevice::ClampRectBottom(rect);
                    CGxDevice::ClampRectLeft(rect);
                    CGxDevice::ClampRectLeftByAspect(rect);
                    CGxDevice::ClampRectBottomByAspect(rect);
                    break;

                case WMSZ_BOTTOMRIGHT:
                    CGxDevice::ClampRectBottom(rect);
                    CGxDevice::ClampRectRight(rect);
                    CGxDevice::ClampRectBottomByAspect(rect);
                    CGxDevice::ClampRectRightByAspect(rect);
                    break;

                default:
                    break;
            }

            sizingRect->left = rect.minX;
            sizingRect->top = rect.minY;
            sizingRect->right = rect.maxX;
            sizingRect->bottom = rect.maxY;
            return 1;
        }

        default:
            break;
    }

    if (device && device->m_windowProc) {
        return device->m_windowProc(hwnd, message, wparam, lparam);
    }

    return DefWindowProcA(hwnd, message, wparam, lparam);
}

// OFFSET: %0x007B61D0
uint16_t CGxDevice::IWindowClassCreate() {
    HINSTANCE instance = GetModuleHandleA(NULL);

    WNDCLASSEXA windowClass;
    memset(&windowClass, 0, sizeof(windowClass));
    windowClass.cbSize = sizeof(windowClass);
    windowClass.style = CS_OWNDC;
    windowClass.lpfnWndProc = WindowProcGx;
    windowClass.hInstance = instance;
    windowClass.lpszClassName = "GxWindowClass";
    windowClass.hIcon = static_cast<HICON>(LoadImageA(instance, "BlizzardIcon.ico", IMAGE_ICON, 0, 0, LR_DEFAULTSIZE));
    windowClass.hCursor = LoadCursorA(instance, "BlizzardCursor.cur");

    if (!windowClass.hCursor) {
        windowClass.hCursor = LoadCursorA(instance, IDC_ARROW);
    }

    return RegisterClassExA(&windowClass);
}

// OFFSET: %0x007B6260
int32_t CGxDevice::IWindowCreate(void* window, CGxFormat& format, int32_t show) {
    HWND& hwnd = *static_cast<HWND*>(window);
    HINSTANCE instance = GetModuleHandleA(NULL);
    DWORD style;

    if (!format.m_window) {
        style = WS_POPUP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_SYSMENU;
    } else if (format.m_maximize == 1) {
        style = WS_POPUP | WS_VISIBLE;
    } else if (format.m_maximize == 2) {
        style = WS_POPUP;
    } else {
        style = WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
    }

    float aspectRatio = 0.0f;

    if (format.m_aspect) {
        int32_t width = format.m_size.x;

        if (width) {
            int32_t height = format.m_size.y;

            if (height) {
                aspectRatio = static_cast<float>(width) / static_cast<float>(height);
            }
        }
    }

    HMONITOR monitor = static_cast<HMONITOR>(CGxDevice::AdapterGetMonitorHandle(format.m_monitor));

    if (monitor) {
        LONG right = format.m_size.x;
        LONG bottom = format.m_size.y;
        LONG left = 0;
        LONG top = 0;

        MONITORINFO info;
        info.cbSize = sizeof(info);

        if (GetMonitorInfoA(monitor, &info)) {
            if (format.m_window && format.m_maximize != 1) {
                left = info.rcWork.left;
                top = info.rcWork.top;
                right = info.rcWork.right;
                bottom = info.rcWork.bottom;
            } else {
                left = info.rcMonitor.left;
                top = info.rcMonitor.top;
                right = info.rcMonitor.right;
                bottom = info.rcMonitor.bottom;
            }
        }

        if (format.m_window && format.m_maximize == 1) {
            format.m_size.y = bottom - top;
            format.m_pos.x = left;
            format.m_pos.y = top;
            format.m_size.x = right - left;
        } else {
            format.m_pos.x += left;
            format.m_pos.y += top;
        }
    } else if (format.m_window && format.m_maximize == 1) {
        format.m_pos.x = 0;
        format.m_pos.y = 0;
        format.m_size.x = GetSystemMetrics(SM_CXSCREEN);
        format.m_size.y = GetSystemMetrics(SM_CYSCREEN);
    }

    if (format.m_window && format.m_maximize != 1) {
        RECT rect;
        rect.left = format.m_pos.x;
        rect.top = format.m_pos.y;
        rect.right = format.m_pos.x + format.m_size.x;
        rect.bottom = format.m_pos.y + format.m_size.y;
        AdjustWindowRectEx(&rect, style, FALSE, 0);

        CGxDevice::s_clientAdjust.x = rect.right - format.m_size.x - rect.left;
        CGxDevice::s_clientAdjust.y = rect.bottom - format.m_size.y - rect.top;

        rect.left = format.m_pos.x;
        rect.top = format.m_pos.y;
        rect.right = format.m_pos.x + format.m_size.x;
        rect.bottom = format.m_pos.y + format.m_size.y;

        if (CGxDevice::AdjustWindowRectToWorkArea(format.m_monitor, aspectRatio, &CGxDevice::s_clientAdjust, &rect)) {
            format.m_pos.x = rect.left;
            format.m_pos.y = rect.top;
            format.m_size.x = rect.right - rect.left;
            format.m_size.y = rect.bottom - rect.top;
        }
    }

    int32_t width = format.m_size.x;
    int32_t height = format.m_size.y;

    if (format.m_window && format.m_maximize != 1 && width && height) {
        width += CGxDevice::s_clientAdjust.x;
        height += CGxDevice::s_clientAdjust.y;
    } else {
        if (!width) {
            width = CW_USEDEFAULT;
        }

        if (!height) {
            height = CW_USEDEFAULT;
        }
    }

    hwnd = CreateWindowExA(WS_EX_APPWINDOW, "GxWindowClass", "World of Warcraft", style, format.m_pos.x, format.m_pos.y, width, height, NULL, NULL, instance, this);

    if (show && hwnd && format.m_maximize != 2) {
        ShowWindow(hwnd, SW_SHOWNORMAL);
    }

    return hwnd != NULL;
}

// OFFSET: #0x0068A170
int32_t CGxDevice::D3dAdapterFormats(TSGrowableArray<CGxFormat>& formats) {
    HINSTANCE d3dLib = NULL;
    LPDIRECT3D9 d3d = NULL;

    if (!CGxDeviceD3d::ILoadD3dLib(d3dLib, d3d)) {
        return 0;
    }

    for (uint32_t i = 0; i < 3; i++) {
        D3DFORMAT adapterFormat = s_d3dAdapterFormats[i];
        uint32_t modeCount = d3d->GetAdapterModeCount(0, adapterFormat);

        for (uint32_t mode = 0; mode < modeCount; mode++) {
            D3DDISPLAYMODE displayMode;

            if (d3d->EnumAdapterModes(0, adapterFormat, mode, &displayMode) != D3D_OK) {
                continue;
            }

            CGxFormat format;
            memset(&format, 0, sizeof(format));

            switch (displayMode.Format) {
                case D3DFMT_A8R8G8B8:
                    format.m_colorFormat = CGxFormat::Fmt_Argb8888;
                    break;

                case D3DFMT_X8R8G8B8:
                    format.m_colorFormat = CGxFormat::Fmt_ArgbX888;
                    break;

                case D3DFMT_R5G6B5:
                    format.m_colorFormat = CGxFormat::Fmt_Rgb565;
                    break;

                default:
                    continue;
            }

            for (uint32_t depth = 0; depth < 4; depth++) {
                if (FAILED(d3d->CheckDeviceFormat(0, D3DDEVTYPE_HAL, displayMode.Format, D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_SURFACE, s_d3dDepthFormats[depth]))) {
                    continue;
                }

                switch (s_d3dDepthFormats[depth]) {
                    case D3DFMT_D32:
                        format.m_depthFormat = CGxFormat::Fmt_Ds320;
                        break;

                    case D3DFMT_D24S8:
                        format.m_depthFormat = CGxFormat::Fmt_Ds248;
                        break;

                    case D3DFMT_D24X8:
                        format.m_depthFormat = CGxFormat::Fmt_Ds24X;
                        break;

                    case D3DFMT_D16:
                        format.m_depthFormat = CGxFormat::Fmt_Ds160;
                        break;

                    default:
                        break;
                }

                for (uint32_t multisample = 0; multisample < 9; multisample++) {
                    if (FAILED(d3d->CheckDeviceMultiSampleType(0, D3DDEVTYPE_HAL, displayMode.Format, FALSE, s_d3dMultisampleTypes[multisample], NULL))) {
                        continue;
                    }

                    if (FAILED(d3d->CheckDeviceMultiSampleType(0, D3DDEVTYPE_HAL, s_d3dDepthFormats[depth], FALSE, s_d3dMultisampleTypes[multisample], NULL))) {
                        continue;
                    }

                    switch (s_d3dMultisampleTypes[multisample]) {
                        case D3DMULTISAMPLE_NONE:
                            format.m_multisampleCount = 1;
                            break;

                        case D3DMULTISAMPLE_2_SAMPLES:
                            format.m_multisampleCount = 2;
                            break;

                        case D3DMULTISAMPLE_4_SAMPLES:
                            format.m_multisampleCount = 4;
                            break;

                        case D3DMULTISAMPLE_6_SAMPLES:
                            format.m_multisampleCount = 6;
                            break;

                        case D3DMULTISAMPLE_8_SAMPLES:
                            format.m_multisampleCount = 8;
                            break;

                        case D3DMULTISAMPLE_10_SAMPLES:
                            format.m_multisampleCount = 10;
                            break;

                        case D3DMULTISAMPLE_12_SAMPLES:
                            format.m_multisampleCount = 12;
                            break;

                        case D3DMULTISAMPLE_14_SAMPLES:
                            format.m_multisampleCount = 14;
                            break;

                        case D3DMULTISAMPLE_16_SAMPLES:
                            format.m_multisampleCount = 16;
                            break;

                        default:
                            break;
                    }

                    if (!CGxDevice::FormatExists(formats, format)) {
                        *formats.New() = format;
                    }
                }
            }
        }
    }

    CGxDeviceD3d::IUnloadD3dLib(d3dLib, d3d);

    return formats.Count() != 0;
}

#endif

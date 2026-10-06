#include <cmath>
#include <cstddef>
#include <cstdint>
#include <new>
#include <typeinfo>
#include "Engine/Source/Gxu/GxuLight.h"
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/Tempest/CMath.h"

// STATIC: %0x00BBD334
static uint8_t s_dirty = 1;

// STATIC: %0x00BBD338
static uint32_t s_maxLights = 4;

// STATIC: %0x00BBD370
static float s_gridSize = 1024.0f;

// STATIC: %0x00BBD374
static float s_gridHalfSize = 512.0f;

// STATIC: %0x00BBD378
static TSList<CGxuLight, TSGetLink<CGxuLight> > s_lights;

// STATIC: %0x00BBD384
static TSList<CGxuLight, TSGetLink<CGxuLight> > s_freeLights;

// STATIC: %0x00BBD390
static TSExplicitList<CGxuLightLink, offsetof(CGxuLightLink, m_lightLink)> s_freeLinks;

// STATIC: %0x00BBD39C
static CLightList s_globalLights;

// STATIC: %0x00CE3078
static uint32_t s_lastHashval;

// STATIC: %0x00CE3080
static uint32_t s_frame;

// STATIC: %0x00CE3090
static TSHashTableReuse<CLightList, HASHKEY_DWORD> s_lightGrid;

static const uint32_t s_lightQueueSize[2] = { 32, 32 };

// STATIC: %0x00CE30C8
static CGxuLightQueue s_lightQueue(s_lightQueueSize);

// PROVEN: ec6a25f4 IDENTICAL/NONE
// OFFSET: 0x00401000
static void GxuLightInitializeImpl() {
}

// PROVEN: 64e038cf IDENTICAL/NONE
// OFFSET: 0x006109C0
static CGxLight* GxuLightLockImpl(CGxuLight* light) {
    light->m_lockCount++;

    return &light->m_light;
}

// OFFSET: %0x006213B0
static void GxuLightSetEnabledImpl(CGxuLight* light, bool enabled) {
    CGxLight* gxLight = GxuLightLock(light);
    gxLight->m_flags ^= (enabled ^ gxLight->m_flags) & 1;
    GxuLightUnlock(light);
}

// OFFSET: %0x006213E0
static int32_t GxuLightGetEnabledImpl(CGxuLight* light) {
    int32_t enabled = GxuLightLock(light)->m_flags & 1;
    GxuLightUnlock(light);

    return enabled;
}

// PROVEN: 6761747e IDENTICAL/NONE
// OFFSET: 0x00610A30
static void GxuLightSetMaxLightsImpl(uint32_t maxLights) {
    s_maxLights = maxLights;

    if (maxLights >= 4) {
        s_maxLights = 4;
    }
}

// OFFSET: %0x00621430
static float GxuLightGetGridSizeImpl() {
    return s_gridSize;
}

// OFFSET: %0x00621440
static void GxuLightSetGridSizeImpl(float gridSize) {
    s_gridSize = gridSize;
    s_gridHalfSize = gridSize * 0.5;
    GxuLightInvalidate();
}

// OFFSET: %0x00621470
static void GxuLightInvalidateImpl() {
    s_dirty = 1;
}

// OFFSET: %0x00621480
float CGxuLight::CalcIntensity(const C3Vector& point, float linearAttenuation, float quadraticAttenuation) {
    C3Vector delta = point - this->m_light.m_dir;
    float distance = sqrt(delta.SquaredMag());
    float dirMag = sqrt(this->m_light.m_dirColor.SquaredMag());
    double intensity = dirMag / (distance * distance * quadraticAttenuation + distance * linearAttenuation + 1.0);
    float ambientMag = sqrt(this->m_light.m_ambientColor.SquaredMag());

    return ambientMag + intensity;
}

// INLINED
CGxuLightLink::~CGxuLightLink() {
}

// OFFSET: %0x00621570
void CGxuLight::ClearLinks() {
    CGxuLightLink* link = this->m_links.Head();

    while (link) {
        CGxuLightLink* next = this->m_links.Next(link);

        link->m_lightLink.Unlink();
        link->m_listLink.Unlink();

        if (this->m_light.m_flags & 2) {
            CLightList* list = link->m_list;

            if (list->m_links.Head()) {
                s_lightGrid.Delete(list);
            }
        }

        s_freeLinks.LinkToTail(link);
        link = next;
    }
}

// OFFSET: %0x006217A0
CGxuLightLink* CGxuLight::NewLink() {
    CGxuLightLink* link = s_freeLinks.Head();

    if (!link) {
        link = s_freeLinks.NewNode(2, 0, 0);
    }

    link->m_lightLink.Unlink();
    this->m_links.LinkToTail(link);

    return link;
}

// OFFSET: %0x00621890
CLightList::~CLightList() {
}

// OFFSET: %0x006218E0
static void GxuLightDestroyImpl(CGxuLight* light) {
    s_dirty = 1;
    s_lights.UnlinkNode(light);
    light->ClearLinks();
    s_freeLights.LinkToTail(light);
}

// OFFSET: %0x006219A0
CGxuLight::~CGxuLight() {
}

// INLINED
CGxuLight::CGxuLight() : m_lockCount(0) {
}

// OFFSET: %0x00621D20
static CGxuLight* GxuLightCreateImpl() {
    CGxuLight* light = s_freeLights.Head();

    if (!light) {
        return s_lights.NewNode(2, 0, 0);
    }

    s_freeLights.UnlinkNode(light);
    s_lights.LinkToTail(light);
    new (&light->m_light) CGxLight();

    return light;
}

// OFFSET: %0x00621DA0
static void GxuLightApplyImpl(float x, float y, int32_t unused, const C3Vector& viewPos, uint32_t maxLights) {
    if (!s_lights.Head()) {
        return;
    }

    if (maxLights >= s_maxLights) {
        maxLights = s_maxLights;
    }

    int32_t cellX = static_cast<int32_t>(x / s_gridSize);
    int32_t cellY = static_cast<int32_t>(y / s_gridSize);

    HASHKEY_DWORD key;
    key.m_key = (cellY << 16) | static_cast<uint16_t>(cellX);
    uint32_t hashval = key.m_key % 0x1FFF;
    CLightList* list = s_lightGrid.Ptr(hashval, key);

    s_frame++;
    uint32_t index = 0;

    for (CGxuLightLink* link = s_globalLights.m_links.Head(); link; link = s_globalLights.m_links.Next(link)) {
        g_theGxDevicePtr->LightSet(index, link->m_light->m_light, viewPos);
        g_theGxDevicePtr->LightEnable(index, 1);
        link->m_light->m_frame = s_frame;
        link->m_light->m_index = index;
        index++;
    }

    if (list) {
        for (CGxuLightLink* link = list->m_links.Head(); link && index < maxLights; link = list->m_links.Next(link)) {
            g_theGxDevicePtr->LightSet(index, link->m_light->m_light, viewPos);
            g_theGxDevicePtr->LightEnable(index, 1);
            link->m_light->m_frame = s_frame;
            link->m_light->m_index = index;
            index++;
        }
    }

    for (; index != 4; index++) {
        g_theGxDevicePtr->LightEnable(index, 0);
    }

    s_dirty = 0;
    s_lastHashval = hashval;
}

// OFFSET: %0x006221C0
static void GxuLightShutdownImpl() {
    s_lightGrid.Destroy();
    s_freeLights.Clear();
    s_freeLinks.Clear();
}

// OFFSET: %0x006227C0
static void GxuLightUnlockImpl(CGxuLight* light) {
    light->m_lockCount--;
    s_dirty = 1;
    light->ClearLinks();

    uint32_t flags = light->m_light.m_flags;

    if (!(flags & 1)) {
        return;
    }

    if (!(flags & 2)) {
        CGxuLightLink* link = light->NewLink();
        s_globalLights.m_links.LinkToTail(link);
        link->m_light = light;
        link->m_list = &s_globalLights;
        return;
    }

    float ambientMag = sqrt(light->m_light.m_ambientColor.SquaredMag());
    float dirMag = sqrt(light->m_light.m_dirColor.SquaredMag());
    float radius = (dirMag + ambientMag) / 0.05f;

    if (light->m_light.m_linearAttenuation != 0.0f) {
        radius = radius / light->m_light.m_linearAttenuation;
    }

    float minX = light->m_light.m_dir.x - radius;
    float minY = light->m_light.m_dir.y - radius;
    float minZ = light->m_light.m_dir.z - radius;
    float maxX = light->m_light.m_dir.x + radius;
    float maxY = light->m_light.m_dir.y + radius;

    int32_t minCellY = static_cast<int32_t>(minY / s_gridSize);
    int32_t minCellX = static_cast<int32_t>(minX / s_gridSize);
    int32_t maxCellY = static_cast<int32_t>(maxY / s_gridSize);
    int32_t maxCellX = static_cast<int32_t>(maxX / s_gridSize);

    for (int32_t cellY = minCellY; cellY <= maxCellY; cellY++) {
        for (int32_t cellX = minCellX; cellX <= maxCellX; cellX++) {
            HASHKEY_DWORD key;
            key.m_key = (cellY << 16) | static_cast<uint16_t>(cellX);
            uint32_t hashval = key.m_key % 0x1FFF;
            CLightList* list = s_lightGrid.Ptr(hashval, key);

            if (!list) {
                list = s_lightGrid.New(hashval, key, 0, 0);
            }

            C3Vector center(cellX * s_gridSize + s_gridHalfSize, s_gridHalfSize + s_gridSize * cellY, minZ);
            float intensity = light->CalcIntensity(center, light->m_light.m_linearAttenuation, light->m_light.m_quadraticAttenuation);

            CGxuLightLink* link = light->NewLink();
            link->m_intensity = intensity;
            link->m_light = light;
            link->m_list = list;

            CGxuLightLink* node = list->m_links.Head();

            while (node && node->m_intensity > intensity) {
                node = list->m_links.Next(node);
            }

            if (node) {
                list->m_links.LinkNode(link, 2, node);
            } else {
                list->m_links.LinkToTail(link);
            }
        }
    }
}

// STATIC: %0x00BBD33C
void (*GxuLightInitialize)() = GxuLightInitializeImpl;

// STATIC: %0x00BBD340
void (*GxuLightShutdown)() = GxuLightShutdownImpl;

// STATIC: %0x00BBD344
CGxuLight* (*GxuLightCreate)() = GxuLightCreateImpl;

// STATIC: %0x00BBD348
void (*GxuLightDestroy)(CGxuLight* light) = GxuLightDestroyImpl;

// STATIC: %0x00BBD34C
CGxLight* (*GxuLightLock)(CGxuLight* light) = GxuLightLockImpl;

// STATIC: %0x00BBD350
void (*GxuLightUnlock)(CGxuLight* light) = GxuLightUnlockImpl;

// STATIC: %0x00BBD354
void (*GxuLightApply)(float x, float y, int32_t unused, const C3Vector& viewPos, uint32_t maxLights) = GxuLightApplyImpl;

// STATIC: %0x00BBD358
int32_t (*GxuLightGetEnabled)(CGxuLight* light) = GxuLightGetEnabledImpl;

// STATIC: %0x00BBD35C
void (*GxuLightSetEnabled)(CGxuLight* light, bool enabled) = GxuLightSetEnabledImpl;

// STATIC: %0x00BBD360
void (*GxuLightSetMaxLights)(uint32_t maxLights) = GxuLightSetMaxLightsImpl;

// STATIC: %0x00BBD364
float (*GxuLightGetGridSize)() = GxuLightGetGridSizeImpl;

// STATIC: %0x00BBD368
void (*GxuLightSetGridSize)(float gridSize) = GxuLightSetGridSizeImpl;

// STATIC: %0x00BBD36C
void (*GxuLightInvalidate)() = GxuLightInvalidateImpl;

// OFFSET: %0x00621710
CGxuLightQueue::CGxuLightQueue(const uint32_t (&params)[2]) : CMemBlock(sizeof(CGxuLight*) * params[0], 0, typeid(CGxuLight*).raw_name(), SLOG_OBJECT) {
    uint32_t count = params[0];
    uint32_t chunk = params[1];

    this->m_chunk = chunk;
    this->m_capacity = count;
    this->m_count = 0;

    if (count) {
        this->m_count++;
        return;
    }

    if (!chunk) {
        return;
    }

    uint32_t grow = chunk;

    if (grow <= 1 - count) {
        grow = 1 - count;
    }

    if (this->Resize(sizeof(CGxuLight*) * (grow + count), true)) {
        this->m_capacity += grow;
        this->m_count++;
    }
}

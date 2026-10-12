#include <cmath>
#include <cstddef>
#include <cstdint>
#include <new>
#include <typeinfo>
#include "Engine/Source/Gxu/GxuLight.h"
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/Tempest/CMath.h"

// STATIC: 0x00CA83C4/0x140B482A4
static uint8_t s_dirty = 1;

// STATIC: 0x00CA83C8
static uint32_t s_maxLights = 4;

// STATIC: 0x00CA8400/0x140B48318
static float s_gridSize = 1024.0f;

// STATIC: 0x00CA8404/0x140B4831C
static float s_gridHalfSize = 512.0f;

// STATIC: 0x00CA8408/0x140B48320
static TSList<CGxuLight, TSGetLink<CGxuLight> > s_lights;

// STATIC: 0x00CA8414/0x140B48338
static TSList<CGxuLight, TSGetLink<CGxuLight> > s_freeLights;

// STATIC: 0x00CA8420/0x140B48350
static TSExplicitList<CGxuLightLink, offsetof(CGxuLightLink, m_lightLink)> s_freeLinks;

// STATIC: 0x00CA842C/0x140B48370
static CLightList s_globalLights;

static uint32_t s_lastHashval;

static uint32_t s_frame;

// STATIC: 0x00DD8AB4/0x140B483C0
static TSHashTableReuse<CLightList, HASHKEY_DWORD> s_lightGrid;

static const uint32_t s_lightQueueSize[2] = { 32, 32 };

static CGxuLightQueue s_lightQueue(s_lightQueueSize);

// PROVEN: ec6a25f4 IDENTICAL/IDENTICAL
// OFFSET: 0x00401000/0x1407D7E80
static void GxuLightInitializeImpl() {
}

// PROVEN: 64e038cf IDENTICAL/DIFFERENT
// OFFSET: 0x006109C0/0x14029AF40
static CGxLight* GxuLightLockImpl(CGxuLight* light) {
    light->m_lockCount++;

    return &light->m_light;
}

// OFFSET: 0x006109D0/0x14029AF50
static void GxuLightSetEnabledImpl(CGxuLight* light, bool enabled) {
    CGxLight* gxLight = GxuLightLock(light);
    gxLight->m_flags ^= (enabled ^ gxLight->m_flags) & 1;
    GxuLightUnlock(light);
}

// PROVEN: d937cbcd IDENTICAL/DIFFERENT
// OFFSET: 0x00610A00/0x14029AF80
static int32_t GxuLightGetEnabledImpl(CGxuLight* light) {
    int32_t enabled = GxuLightLock(light)->m_flags & 1;
    GxuLightUnlock(light);

    return enabled;
}

// PROVEN: 6761747e IDENTICAL/DIFFERENT
// OFFSET: 0x00610A30/0x14029AFB0
static void GxuLightSetMaxLightsImpl(uint32_t maxLights) {
    s_maxLights = maxLights;

    if (maxLights >= 4) {
        s_maxLights = 4;
    }
}

// PROVEN: 59b9700f IDENTICAL/IDENTICAL
// OFFSET: 0x00610A50/0x14029AFD0
static float GxuLightGetGridSizeImpl() {
    return s_gridSize;
}

// OFFSET: 0x00610A60/0x14029AFE0
static void GxuLightSetGridSizeImpl(float gridSize) {
    s_gridSize = gridSize;
    s_gridHalfSize = gridSize * 0.5;
    GxuLightInvalidate();
}

// INLINED
static void GxuLightInvalidateImpl() {
    s_dirty = 1;
}

// OFFSET: 0x00610AA0
float CGxuLight::CalcIntensity(const C3Vector& point, float linearAttenuation, float quadraticAttenuation) {
    C3Vector delta = point - this->m_light.m_dir;
    float distance = sqrt(delta.SquaredMag());
    float dirMag = sqrt(this->m_light.m_dirColor.SquaredMag());
    double intensity = dirMag / (distance * distance * quadraticAttenuation + distance * linearAttenuation + 1.0);
    float ambientMag = sqrt(this->m_light.m_ambientColor.SquaredMag());

    return ambientMag + intensity;
}

// PROVEN: 99f107c6 IDENTICAL/NONE
// OFFSET: 0x00775F40/0x140460560
CGxuLightLink::~CGxuLightLink() {
}

// OFFSET: 0x00610B90/0x14029B140
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

// OFFSET: 0x00610D30/0x14029B380
CGxuLightLink* CGxuLight::NewLink() {
    CGxuLightLink* link = s_freeLinks.Head();

    if (!link) {
        link = s_freeLinks.NewNode(2, 0, 0);
    }

    link->m_lightLink.Unlink();
    this->m_links.LinkToTail(link);

    return link;
}

// PROVEN: ac47e974 IDENTICAL/DIFFERENT
// OFFSET: 0x00610E20/0x14029B4C0
CLightList::~CLightList() {
}

// OFFSET: 0x00610E70/0x14029B530
static void GxuLightDestroyImpl(CGxuLight* light) {
    s_dirty = 1;
    s_lights.UnlinkNode(light);
    light->ClearLinks();
    s_freeLights.LinkToTail(light);
}

// PROVEN: 4e8645ef IDENTICAL/DIFFERENT
// OFFSET: 0x006110F0/0x14029B8A0
CGxuLight::~CGxuLight() {
}

// INLINED
CGxuLight::CGxuLight() : m_lockCount(0) {
}

// OFFSET: 0x00611400/0x14029BBE0
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

// OFFSET: 0x00610ED0/0x14029B600
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

// OFFSET: 0x006116F0/0x14029C0A0
static void GxuLightShutdownImpl() {
    s_lightGrid.Destroy();
    s_freeLights.Clear();
    s_freeLinks.Clear();
}

// OFFSET: 0x00611CE0/0x14029C9F0
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

void (*GxuLightInitialize)() = GxuLightInitializeImpl;

void (*GxuLightShutdown)() = GxuLightShutdownImpl;

CGxuLight* (*GxuLightCreate)() = GxuLightCreateImpl;

void (*GxuLightDestroy)(CGxuLight* light) = GxuLightDestroyImpl;

// STATIC: 0x00CA83DC/0x140B482D0
CGxLight* (*GxuLightLock)(CGxuLight* light) = GxuLightLockImpl;

// STATIC: 0x00CA83E0/0x140B482D8
void (*GxuLightUnlock)(CGxuLight* light) = GxuLightUnlockImpl;

void (*GxuLightApply)(float x, float y, int32_t unused, const C3Vector& viewPos, uint32_t maxLights) = GxuLightApplyImpl;

int32_t (*GxuLightGetEnabled)(CGxuLight* light) = GxuLightGetEnabledImpl;

void (*GxuLightSetEnabled)(CGxuLight* light, bool enabled) = GxuLightSetEnabledImpl;

void (*GxuLightSetMaxLights)(uint32_t maxLights) = GxuLightSetMaxLightsImpl;

float (*GxuLightGetGridSize)() = GxuLightGetGridSizeImpl;

void (*GxuLightSetGridSize)(float gridSize) = GxuLightSetGridSizeImpl;

void (*GxuLightInvalidate)() = GxuLightInvalidateImpl;

// INLINED
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

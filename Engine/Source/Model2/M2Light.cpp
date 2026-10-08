#include <cmath>
#include <cstdint>
#include <cstring>
#include "Engine/Source/Model2/M2Light.h"
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "Engine/Source/Gx/CGxLight.h"
#include "Engine/Source/Gx/Gx.h"
#include "Engine/Source/Model2/M2Scene.h"
#include "Engine/Source/ShaderEffect/ShaderEffect.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/Tempest/C44Matrix.h"
#include "Engine/Source/Tempest/CImVector.h"

// STATIC: 0x00D07F14
static float s_lightGridScale = 0.05f;

// PROVEN: 9a8905f2 IDENTICAL/NONE
// OFFSET: 0x00812BF0
void CM2Light::Copy(const CM2Light& light) {
    this->m_scene = NULL;
    this->m_stamp = 0;
    this->m_type = light.m_type;
    this->m_pos = light.m_pos;
    this->m_dir = light.m_dir;
    this->m_ambColor = light.m_ambColor;
    this->m_dirColor = light.m_dirColor;
    this->m_specColor = light.m_specColor;
    this->m_constantAttenuation = light.m_constantAttenuation;
    this->m_linearAttenuation = light.m_linearAttenuation;
    this->m_quadraticAttenuation = light.m_quadraticAttenuation;
    this->m_visible = light.m_visible;
    this->m_lightPrev = NULL;
    this->m_lightNext = NULL;
}

// PROVEN: d77917a8 IDENTICAL/DIFFERENT
// OFFSET: 0x00812C90/0x14052A8E0
void CM2Light::Initialize(CM2Scene* scene) {
    this->m_scene = scene;
    this->m_stamp = scene ? scene->m_frameStamp - 1 : 0;
}

// PROVEN: ee6c8208 IDENTICAL/DIFFERENT
// OFFSET: 0x00812CE0/0x14052AA80
void CM2Lighting::Initialize(CM2Scene* scene, const CAaSphere& sphere) {
    memset(this, 0, sizeof(*this));
    this->m_scene = scene;
    this->m_flags |= 0x20;
    this->m_sphere4 = sphere;
}

// PROVEN: a97406ac IDENTICAL/DIFFERENT
// OFFSET: 0x00812D20/0x14052AAE0
void CM2Lighting::SetFog(const C3Vector& color, float start, float end) {
    this->m_fogStart = start;
    this->m_fogEnd = end;
    this->m_fogScale = 1.0f / (end - start);
    this->m_fogDensity = 1.0f;
    this->m_fogColor = color;
}

// PROVEN: 38c345ef IDENTICAL/DIFFERENT
// OFFSET: 0x00812D90/0x14052AB30
void CM2Lighting::SetFog(const C3Vector& color, float start, float end, float density) {
    this->m_fogStart = start;
    this->m_fogEnd = end;
    this->m_fogScale = 1.0f / (end - start);
    this->m_fogDensity = density;
    this->m_fogColor = color;
}

// PROVEN: 20156ce1 IDENTICAL/NONE
// OFFSET: 0x00812E00
int32_t CM2Lighting::GetPointLight(uint32_t index, C3Vector& pos, C3Vector& color, C3Vector& attenuation) {
    if (index >= this->m_lightCount) {
        return 0;
    }

    CM2Light* light = this->m_lights[index];
    pos = light->m_pos;
    color = light->m_dirColor;
    attenuation.x = light->m_constantAttenuation;
    attenuation.y = light->m_linearAttenuation;
    attenuation.z = light->m_quadraticAttenuation;

    return 1;
}

// PROVEN: 4d2f8cda IDENTICAL/IDENTICAL
// OFFSET: 0x00812E60/0x14052ABF0
CM2Light::CM2Light() {
    this->m_scene = NULL;
    this->m_stamp = 0;
    this->m_type = M2LIGHT_POINT;
    this->m_constantAttenuation = 0.0f;
    this->m_linearAttenuation = 0.7f;
    this->m_quadraticAttenuation = 0.03f;
    this->m_visible = 0;
    this->m_lightPrev = NULL;
    this->m_lightNext = NULL;
}

// INLINED
CM2Light::~CM2Light() {
    this->Unlink();
}

// OFFSET: 0x00812CB0/0x14052A900
void CM2Light::Unlink() {
    if (this->m_lightPrev) {
        *this->m_lightPrev = this->m_lightNext;
    }

    if (this->m_lightNext) {
        this->m_lightNext->m_lightPrev = this->m_lightPrev;
    }

    this->m_lightNext = NULL;
    this->m_lightPrev = NULL;
}

// OFFSET: 0x00812F00/0x14052AD10
void CM2Light::SetDirection(const C3Vector& dir) {
    this->m_dir = dir;
    this->m_dir.NormalizeSafe();
}

// OFFSET: 0x00812F90/0x14052ADF0
void CM2Light::GxLightSet(uint32_t index) {
    CGxLight light;
    light.m_flags = (light.m_flags & ~1u) | (this->m_visible & 1);

    if (this->m_type == M2LIGHT_DIRECT) {
        light.m_flags &= ~2u;
        light.m_dir = this->m_dir;
        light.m_ambientColor = this->m_ambColor;
        light.m_specularColor = this->m_specColor;
    } else {
        light.m_flags |= 2;
        light.m_dir = this->m_pos;
        light.m_ambientColor = C3Vector();
        light.m_specularColor = C3Vector();
    }

    light.m_dirColor = this->m_dirColor;
    light.m_constantAttenuation = this->m_constantAttenuation;
    light.m_linearAttenuation = this->m_linearAttenuation;
    light.m_quadraticAttenuation = this->m_quadraticAttenuation;

    g_theGxDevicePtr->LightSet(index, light, C3Vector());
    g_theGxDevicePtr->LightEnable(index, 1);
}

// OFFSET: 0x008130D0/0x14052A930
void CM2Light::Link() {
    if (!this->m_visible || !this->m_scene) {
        return;
    }

    if (this->m_type == M2LIGHT_POINT) {
        if (!this->m_scene->m_lightGrid) {
            this->m_scene->m_lightGrid = static_cast<CM2Light**>(SMemAlloc(sizeof(CM2Light*) * 32 * 32, __FILE__, __LINE__, 0));
            memset(this->m_scene->m_lightGrid, 0, sizeof(CM2Light*) * 32 * 32);
        }

        int32_t y = static_cast<int32_t>(floor(this->m_pos.y * s_lightGridScale)) & 0x1F;
        int32_t x = static_cast<int32_t>(floor(this->m_pos.x * s_lightGridScale)) & 0x1F;
        CM2Light** head = &this->m_scene->m_lightGrid[y * 32 + x];

        this->m_lightPrev = head;
        this->m_lightNext = *head;
        *this->m_lightPrev = this;

        if (this->m_lightNext) {
            this->m_lightNext->m_lightPrev = &this->m_lightNext;
        }
    } else if (!(this->m_scene->m_flags & 1)) {
        CM2Light** head = &this->m_scene->m_lightList;

        this->m_lightPrev = head;
        this->m_lightNext = *head;
        *head = this;

        if (this->m_lightNext) {
            this->m_lightNext->m_lightPrev = &this->m_lightNext;
        }
    }
}

// PROVEN: 37e8d2e6 IDENTICAL/IDENTICAL
// OFFSET: 0x008131F0/0x14052AF90
void CM2Lighting::AddAmbient(const C3Vector& color) {
    this->m_sunAmbient += color;
}

// OFFSET: 0x00813230/0x14052AFC0
void CM2Lighting::AddDiffuse(const C3Vector& color, const C3Vector& dir) {
    C3Vector viewDir = dir;

    if (this->m_scene) {
        const C44Matrix& view = this->m_scene->m_view;
        viewDir.x = view.b0 * dir.y + view.c0 * dir.z + dir.x * view.a0;
        viewDir.y = view.b1 * dir.y + view.a1 * dir.x + view.c1 * dir.z;
        viewDir.z = view.b2 * dir.y + view.a2 * dir.x + view.c2 * dir.z;
    }

    this->m_vector18.x = viewDir.x * color.x + this->m_vector18.x;
    this->m_vector18.y = viewDir.y * color.x + this->m_vector18.y;
    this->m_vector18.z = color.x * viewDir.z + this->m_vector18.z;

    this->m_vector24.x = viewDir.x * color.y + this->m_vector24.x;
    this->m_vector24.y = viewDir.y * color.y + this->m_vector24.y;
    this->m_vector24.z = color.y * viewDir.z + this->m_vector24.z;

    this->m_vector30.x = color.z * viewDir.x + this->m_vector30.x;
    this->m_vector30.y = viewDir.y * color.z + this->m_vector30.y;
    this->m_vector30.z = color.z * viewDir.z + this->m_vector30.z;

    float luminance = color.y * 0.71516f + color.x * 0.212671f + color.z * 0.072169f;
    this->m_vector3C.x = viewDir.x * luminance + this->m_vector3C.x;
    this->m_vector3C.y = viewDir.y * luminance + this->m_vector3C.y;
    this->m_vector3C.z = viewDir.z * luminance + this->m_vector3C.z;

    this->m_vector48 += color;
    this->m_sunDir = dir;
    this->m_sunDiffuse = color;
}

// OFFSET: 0x008134A0/0x14052B220
void CM2Lighting::AddLight(CM2Light* light) {
    if (!light->m_visible) {
        return;
    }

    if (light->m_type != M2LIGHT_POINT) {
        this->AddAmbient(light->m_ambColor);
        this->AddDiffuse(light->m_dirColor, light->m_dir);
        this->m_sunSpecular += light->m_specColor;
        return;
    }

    float dz = light->m_pos.z - this->m_sphere4.c.z;
    float dy = light->m_pos.y - this->m_sphere4.c.y;
    float dx = light->m_pos.x - this->m_sphere4.c.x;
    float dist = dx * dx + (dy * dy + dz * dz);

    uint32_t count = this->m_lightCount;
    const uint32_t capacity = sizeof(this->m_lights) / sizeof(this->m_lights[0]);

    if (count >= capacity) {
        if (dist >= this->m_lightDist[capacity - 1]) {
            return;
        }

        count--;
    }

    for (; count && dist <= this->m_lightDist[count - 1]; count--) {
        this->m_lights[count] = this->m_lights[count - 1];
        this->m_lightDist[count] = this->m_lightDist[count - 1];
    }

    this->m_lightDist[count] = dist;
    this->m_lights[count] = light;

    if (this->m_lightCount < capacity) {
        this->m_lightCount++;
    }
}

// OFFSET: 0x00813610/0x14052B380
void CM2Lighting::CameraSpace() {
    if (this->m_flags & 1) {
        return;
    }

    if (this->m_scene) {
        for (uint32_t i = 0; i < this->m_lightCount; i++) {
            this->m_lights[i]->m_viewPos = this->m_lights[i]->m_pos * this->m_scene->m_view;
        }

        if (this->m_flags & 0x40) {
            this->m_liquidPlane.Transform(this->m_scene->m_view);
        }
    }

    this->m_flags |= 1;
}

// OFFSET: 0x00813730/0x14052B4D0
void CM2Lighting::SetupSunlight() {
    if (this->m_flags & 2) {
        return;
    }

    this->m_sunDir = this->m_vector3C;

    if (this->m_sunDir.z * this->m_sunDir.z + this->m_sunDir.y * this->m_sunDir.y + this->m_sunDir.x * this->m_sunDir.x <= 0.00001f) {
        this->m_sunDir = C3Vector(0.0f, 0.0f, -1.0f);
    } else {
        this->m_sunDir.NormalizeSafe();
    }

    float red = this->m_sunDir.z * this->m_vector18.z + this->m_sunDir.y * this->m_vector18.y + this->m_sunDir.x * this->m_vector18.x;
    float green = this->m_sunDir.z * this->m_vector24.z + this->m_sunDir.y * this->m_vector24.y + this->m_sunDir.x * this->m_vector24.x;
    float blue = this->m_vector30.z * this->m_sunDir.z + this->m_vector30.y * this->m_sunDir.y + this->m_vector30.x * this->m_sunDir.x;

    this->m_sunDiffuse = C3Vector(red * 1.25f - this->m_vector48.x * 0.25f, green * 1.25f - this->m_vector48.y * 0.25f, 1.25f * blue - this->m_vector48.z * 0.25f);
    this->m_sunAmbient += C3Vector((this->m_vector48.x - red) * 0.25f, (this->m_vector48.y - green) * 0.25f, (this->m_vector48.z - blue) * 0.25f);

    this->m_flags |= 2;
}

// OFFSET: 0x00813980
void CM2Lighting::SetupGxLights(const C3Vector* origin) {
    CGxLight light;
    light.m_flags |= 1;

    this->SetupSunlight();

    light.m_flags &= ~2u;
    light.m_dir = this->m_sunDir;
    light.m_ambientColor = this->m_sunAmbient;
    light.m_dirColor = this->m_sunDiffuse;
    light.m_specularColor = this->m_sunSpecular;

    g_theGxDevicePtr->LightSet(0, light, C3Vector());
    g_theGxDevicePtr->LightEnable(0, 1);

    light.m_flags |= 2;
    light.m_ambientColor = C3Vector();
    light.m_specularColor = C3Vector();

    uint32_t index = 1;

    for (uint32_t i = 0; i < this->m_lightCount && index < 4; i++) {
        CM2Light* point = this->m_lights[i];

        if (origin) {
            light.m_dir = point->m_pos - *origin;
        } else {
            light.m_dir = point->m_viewPos;
        }

        light.m_dirColor = point->m_dirColor;
        light.m_constantAttenuation = point->m_constantAttenuation;
        light.m_linearAttenuation = point->m_linearAttenuation;
        light.m_quadraticAttenuation = point->m_quadraticAttenuation;

        g_theGxDevicePtr->LightSet(index, light, C3Vector());
        g_theGxDevicePtr->LightEnable(index, 1);
        index++;
    }

    for (; index < 4; index++) {
        g_theGxDevicePtr->LightEnable(index, 0);
    }
}

// PROVEN: 17b6ddcc IDENTICAL/IDENTICAL
// OFFSET: 0x00813910/0x14052B720
int32_t CM2Lighting::GetSunLight(C3Vector& dir, C3Vector& ambient, C3Vector& diffuse, C3Vector& specular) {
    this->SetupSunlight();

    dir = this->m_sunDir;
    ambient = this->m_sunAmbient;
    diffuse = this->m_sunDiffuse;
    specular = this->m_sunSpecular;

    return 1;
}

// OFFSET: 0x00813BB0/0x14052AC60
void CM2Light::SetLightType(M2LIGHTTYPE type) {
    if (this->m_type == type) {
        return;
    }

    this->m_type = type;

    if (this->m_visible && this->m_scene) {
        this->Unlink();
        this->Link();
    }
}

// OFFSET: 0x00813C00/0x14052ACB0
void CM2Light::SetPosition(const C3Vector& pos) {
    this->m_pos = pos;

    if (this->m_visible && this->m_scene && this->m_type == M2LIGHT_POINT) {
        this->Unlink();
        this->Link();
    }
}

// OFFSET: 0x00813C60/0x14052AD90
void CM2Light::SetVisible(int32_t visible) {
    if (!this->m_visible == !visible) {
        return;
    }

    this->m_visible = visible;

    if (!this->m_scene) {
        return;
    }

    if (visible) {
        this->Link();
    } else {
        this->Unlink();
    }
}

// OFFSET: 0x00813CC0
void CM2Lighting::SetupGxFog() {
    int32_t fixedFog = g_theGxDevicePtr->m_caps.unk_02FC;

    if (fabs(this->m_fogScale) < 0.00000023841858f) {
        if (fixedFog) {
            GxRsSet(GxRs_Fog, 0);
        }

        return;
    }

    GxRsSet(GxRs_FogStart, this->m_fogStart);
    GxRsSet(GxRs_FogEnd, this->m_fogEnd);

    float blue = this->m_fogColor.z;
    blue = blue >= 0.0f ? (blue >= 1.0f ? 1.0f : blue) : 0.0f;
    float green = this->m_fogColor.y;
    green = green >= 0.0f ? (green >= 1.0f ? 1.0f : green) : 0.0f;
    float red = this->m_fogColor.x;
    red = red >= 0.0f ? (red >= 1.0f ? 1.0f : red) : 0.0f;

    CImVector color(static_cast<uint8_t>(static_cast<int32_t>(blue * 255.0f)), static_cast<uint8_t>(static_cast<int32_t>(green * 255.0f)), static_cast<uint8_t>(static_cast<int32_t>(red * 255.0f)), 0xFF);

    GxRsSet(GxRs_FogColor, *reinterpret_cast<int32_t*>(&color));

    if (fixedFog) {
        GxRsSet(GxRs_Fog, 1);
    }
}

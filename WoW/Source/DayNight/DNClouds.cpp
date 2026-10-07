#include "WoW/Source/DayNight/DNClouds.h"
#include <cmath>
#include <cstdlib>
#include <cstring>
#include "Engine/Source/Gx/CGxDevice/CGxDevice.h"
#include "Engine/Source/Gx/CGxDevice/CGxShader.h"
#include "Engine/Source/Gx/CGxTexFlags.h"
#include "Engine/Source/Gx/Gx.h"
#include "Engine/Source/Services/Texture.h"
#include "Engine/Source/Tempest/C44Matrix.h"
#include "Engine/Source/Tempest/CMath.h"
#include "WoW/Source/DayNight/DayNight.h"
#include "WoW/Source/DayNight/DNPlanet.h"

struct CloudOctave {
    /* 0000 */ uint16_t m_x;
    /* 0002 */ uint16_t m_y;
    /* 0004 */ uint16_t m_t;
    /* 0006 */ uint16_t m_stepX;
    /* 0008 */ uint16_t m_stepY;
    /* 000A */ uint16_t unk_000A;
    /* 000C */ uint16_t m_t0;
    /* 000E */ uint16_t m_y0;
    /* 0010 */ uint16_t m_t0Copy;
    /* 0012 */ uint16_t m_wrap;
    /* 0014 */ uint16_t m_y1;
    /* 0016 */ uint16_t m_t1;
    /* 0018 */ float m_amplitude;
    /* 001C */ int32_t m_hash[4];
    /* 002C */ float m_corner[8];
    /* 004C */ uint32_t m_x0;
    /* 0050 */ uint32_t m_lastX0;
};

// STATIC: 0x00BBAF18
static uint32_t s_lodSizes[5] = { 128, 256, 512, 1024, 2048 };

// STATIC: 0x00BBAF2C
static uint32_t s_lodShifts[5] = { 7, 8, 9, 10, 11 };

// STATIC: 0x00BBAF40
static float s_ringAngles[12] = { 0.0f, 0.025f, 0.050000001f, 0.075000003f, 0.1f, 0.125f, 0.15000001f, 0.175f, 0.205f, 0.23f, 0.245f, 0.25f };

// STATIC: 0x00BBAF70
static uint8_t s_ringAlphas[12] = { 255, 255, 255, 255, 255, 255, 255, 255, 255, 128, 0, 0 };

// STATIC: 0x00CEFAD8
static uint8_t s_perm[256] = {
    225, 155, 210, 108, 175, 199, 221, 144, 203, 116, 70, 213, 69, 158, 33, 252,
    5, 82, 173, 133, 222, 139, 174, 27, 9, 71, 90, 246, 75, 130, 91, 191,
    169, 138, 2, 151, 194, 235, 81, 7, 25, 113, 228, 159, 205, 253, 134, 142,
    248, 65, 224, 217, 22, 121, 229, 63, 89, 103, 96, 104, 156, 17, 201, 129,
    36, 8, 165, 110, 237, 117, 231, 56, 132, 211, 152, 20, 181, 111, 239, 218,
    170, 163, 51, 172, 157, 47, 80, 212, 176, 250, 87, 49, 99, 242, 136, 189,
    162, 115, 44, 43, 124, 94, 150, 16, 141, 247, 32, 10, 198, 223, 255, 72,
    53, 131, 84, 57, 220, 197, 58, 50, 208, 11, 241, 28, 3, 192, 62, 202,
    18, 215, 153, 24, 76, 41, 15, 179, 39, 46, 55, 6, 128, 167, 23, 188,
    106, 34, 187, 140, 164, 73, 112, 182, 244, 195, 227, 13, 35, 77, 196, 185,
    26, 200, 226, 119, 31, 123, 168, 125, 249, 68, 183, 230, 177, 135, 160, 180,
    12, 1, 243, 148, 102, 166, 38, 238, 251, 37, 240, 126, 64, 74, 161, 40,
    184, 149, 171, 178, 101, 66, 29, 59, 146, 61, 254, 107, 42, 86, 154, 4,
    236, 232, 120, 21, 233, 209, 45, 98, 193, 114, 78, 19, 206, 14, 118, 127,
    48, 79, 147, 85, 30, 207, 219, 54, 88, 234, 190, 122, 95, 67, 143, 109,
    137, 214, 145, 93, 92, 100, 245, 0, 216, 186, 60, 83, 105, 97, 204, 52,
};

// STATIC: 0x00CEFBD8
static C2Vector s_bumpIntensityTable[8] = {
    C2Vector(0.16666667f, 1.0f),
    C2Vector(0.19444445f, 1.0f),
    C2Vector(0.2013889f, 1.0f),
    C2Vector(0.22916667f, 1.0f),
    C2Vector(0.89583331f, 1.0f),
    C2Vector(0.9236111f, 1.0f),
    C2Vector(0.8888889f, 1.0f),
    C2Vector(0.91666669f, 1.0f),
};

// STATIC: 0x00CEFC18
static uint16_t s_octaveScales[5][5] = {
    { 16, 32, 64, 128, 256 },
    { 8, 16, 32, 64, 128 },
    { 4, 8, 16, 32, 64 },
    { 2, 4, 8, 16, 32 },
    { 1, 2, 4, 8, 4 },
};

// STATIC: 0x00E42950
static float s_smoothTable[256];

// STATIC: 0x00E42D50
static uint8_t s_sharpnessTable[256];

// STATIC: 0x00E42E50
static float s_noiseTable[256];

// STATIC: 0x00E43250
uint32_t DayNight::DNClouds::s_refCount;

// STATIC: 0x00E43254
CGxShader* DayNight::DNClouds::s_vertexShader;

// STATIC: 0x00E43258
CGxShader* DayNight::DNClouds::s_pixelShader;

// INLINED
static float FastInvSqrt(float value) {
    uint32_t bits = 0x5F3997BB - ((*reinterpret_cast<uint32_t*>(&value) >> 1) & 0x3FFFFFFF);
    return *reinterpret_cast<float*>(&bits);
}

// PROVEN: b83565b0 IDENTICAL/NONE
// OFFSET: 0x0064D260/0x1402E8790
void DayNight::DNClouds::SetFullUpdate() {
    this->m_fullUpdate = 1;
}

// PROVEN: 42668219 IDENTICAL/NONE
// OFFSET: 0x0064D270/0x1402E87A0
void DayNight::DNClouds::Callback_GxTex(EGxTexCommand command, uint32_t width, uint32_t height, uint32_t face, uint32_t level, void* userArg, uint32_t& texelStrideInBytes, const void*& texels) {
    if (command == GxTex_Latch && !level) {
        texelStrideInBytes = 4 * width;
        texels = userArg;
    }
}

// PROVEN: f280b04a IDENTICAL/NONE
// OFFSET: 0x0064D2A0
void DayNight::DNClouds::SetDensity(float density) {
    float value = this->m_densityOverride;

    if (value == 0.0f) {
        value = density;
    }

    this->m_density = static_cast<uint8_t>(static_cast<int32_t>((1.0f - value) * 255.0f));
}

// PROVEN: 04cf9141 IDENTICAL/NONE
// OFFSET: 0x0064D2E0/0x1402E88C0
void DayNight::DNClouds::Initialize() {
    if (!DNClouds::s_refCount++ && g_theGxDevicePtr->m_caps.m_shaderModel >= 3) {
        DNClouds::s_vertexShader = g_theGxDevicePtr->ShaderCreate(GxSh_Vertex, "DNClouds");
        DNClouds::s_pixelShader = g_theGxDevicePtr->ShaderCreate(GxSh_Pixel, "DNClouds");
    }
}

// PROVEN: 8fc14f98 IDENTICAL/NONE
// OFFSET: 0x0064D330
void DayNight::DNClouds::Destroy() {
    if (this->m_textures[0]) {
        TextureRelease(this->m_textures[0]);
    }

    if (this->m_textures[1]) {
        TextureRelease(this->m_textures[1]);
    }

    this->m_textures[0] = NULL;
    this->m_textures[1] = NULL;

    if (--DNClouds::s_refCount) {
        return;
    }

    if (DNClouds::s_vertexShader) {
        g_theGxDevicePtr->ShaderRelease(DNClouds::s_vertexShader);
        DNClouds::s_vertexShader = NULL;
    }

    if (DNClouds::s_pixelShader) {
        g_theGxDevicePtr->ShaderRelease(DNClouds::s_pixelShader);
        DNClouds::s_pixelShader = NULL;
    }
}

// OFFSET: 0x0064D3C0
void __stdcall DayNight::DNClouds::Collide(const C3Vector& origin, const C3Vector& dir, C3Vector& out) {
    float offset = -cos(0.78539819f);
    float r0;
    float t;
    CMath::solvequad_(dir.x * dir.x + dir.y * dir.y + dir.z * dir.z, -offset * dir.z * 2.0f, offset * offset - 1.0f, r0, t);

    out.x = DayNight::g_dnInfo.m_camPos.x + (origin.x + dir.x * t);
    out.y = DayNight::g_dnInfo.m_camPos.y + (origin.y + dir.y * t);
    out.z = (origin.z + dir.z * t) + DayNight::g_dnInfo.m_camPos.z;
}

// OFFSET: 0x0064D4D0
void DayNight::DNClouds::SetSharpness(float sharpness) {
    this->m_sharpness = sharpness;

    float step = static_cast<float>(255 - this->m_density) * 0.00390625f;
    float x = 0.0f;

    for (uint32_t i = 0; i < 256; i++) {
        s_sharpnessTable[i] = static_cast<uint8_t>(static_cast<int32_t>(255.0 - pow(this->m_sharpness, x) * 255.0));
        x = step + x;
    }
}

// OFFSET: 0x0064D570/0x1402E89E0
DayNight::DNClouds::DNClouds() {
    for (float* noise = s_noiseTable; noise < s_noiseTable + 256; noise++) {
        *noise = 1.0f - static_cast<float>(rand()) * 0.000030518509f * 2.0f;
    }

    for (uint32_t i = 0; i < 256; i++) {
        s_smoothTable[i] = (1.0 - cos(static_cast<double>(i) * 0.012271847)) * 0.5;
    }

    this->m_texIndex = 0;
    this->m_fullUpdate = 0;
    this->m_octaves = 4;
    this->m_texFormat = static_cast<EGxTexFormat>(2);
    this->m_animSpeed = 0.002f;
    this->m_rowsPerUpdate = 8;
}

// OFFSET: 0x0064D660
void DayNight::DNClouds::WorldToTexture(const C3Vector& world, C2Vector& uv) {
    float dx = world.x - DayNight::g_dnInfo.m_camPos.x;
    float dy = world.y - DayNight::g_dnInfo.m_camPos.y;
    float offset = -cos(0.78539819f);
    float dz = (world.z - DayNight::g_dnInfo.m_camPos.z) - offset;

    float angle = acos(((dy + dx) * 0.0f + dz) / (sqrt(dx * dx + dy * dy + dz * dz) * sqrt(1.0)));
    float clamped = angle <= 0.78539819f ? angle : 0.78539819f;
    float radius = clamped * 1.2732395f * 0.5f;

    float length = sqrt(dx * dx + dy * dy);
    float nx;
    float ny;

    if (length <= 0.00001f) {
        ny = 0.0f;
        nx = 0.0f;
    } else {
        ny = (1.0f / length) * dy;
        nx = (1.0f / length) * dx;
    }

    uv.x = (nx * radius + 0.5f) * static_cast<double>(this->m_size);
    uv.y = (radius * ny + 0.5f) * static_cast<double>(this->m_size);
}

// OFFSET: 0x0064D7C0
float DayNight::DNClouds::GetDensity(const C3Vector& pos, float scale) {
    if (!this->m_enabled) {
        return 0.0f;
    }

    C2Vector uv(0.0f, 0.0f);
    this->WorldToTexture(pos, uv);

    uint32_t max = this->m_size - 1;
    int32_t x = static_cast<int32_t>(static_cast<int64_t>(uv.x));
    int32_t y = static_cast<int32_t>(static_cast<int64_t>(uv.y));

    if (static_cast<uint32_t>(x) > max) {
        x = max & ~(x >> 31);
    }

    if (static_cast<uint32_t>(y) > max) {
        y = max & ~(y >> 31);
    }

    return this->m_densities.m_data[(y << this->m_shift) + x] * 0.0039215689f;
}

// OFFSET: 0x0064D860/0x1402E8D50
void DayNight::DNClouds::SetupBumpMap(C3Vector& shadeColor, C3Vector& sunColor, C3Vector& baseColor, C3Vector& sunPos, float& intensity) {
    DNInfo& info = DayNight::g_dnInfo;
    const C3Vector& planet = info.m_dayFraction < 0.2013889f || info.m_dayFraction > 0.9236111f ? DayNight::g_planets[1].m_position : DayNight::g_planets[0].m_position;

    C3Vector dir(planet.x - info.m_camPos.x, planet.y - info.m_camPos.y, planet.z - info.m_camPos.z);
    C3Vector origin(0.0f, 0.0f, 0.0f);
    C3Vector hit(0.0f, 0.0f, 0.0f);
    DNClouds::Collide(origin, dir, hit);

    C2Vector uv(0.0f, 0.0f);
    this->WorldToTexture(hit, uv);
    info.m_sunCloudUV = uv;

    float size = static_cast<float>(this->m_size);
    info.m_sunCloudUV.y = (1.0f / size) * uv.y;
    info.m_sunCloudUV.x = (1.0f / size) * uv.x;

    sunPos.x = uv.x;
    sunPos.y = uv.y;
    sunPos.z = 64.0f;

    shadeColor.x = info.m_light.m_colors[11].r * 0.0039215689f;
    shadeColor.y = info.m_light.m_colors[11].g * 0.0039215689f;
    shadeColor.z = info.m_light.m_colors[11].b * 0.0039215689f;
    sunColor.x = info.m_light.m_colors[10].r * 0.0039215689f;
    sunColor.y = info.m_light.m_colors[10].g * 0.0039215689f;
    sunColor.z = info.m_light.m_colors[10].b * 0.0039215689f;
    baseColor.x = info.m_light.m_colors[12].r * 0.0039215689f;
    baseColor.y = info.m_light.m_colors[12].g * 0.0039215689f;
    baseColor.z = info.m_light.m_colors[12].b * 0.0039215689f;

    intensity = DayNight::InterpTable(s_bumpIntensityTable, 8, info.m_dayFraction) * intensity;
    sunPos.z = info.m_stormBlend * 192.0f + sunPos.z;
    intensity = (1.0f - info.m_stormBlend * 0.75f) * intensity;
}

// OFFSET: 0x0064DAE0/0x1402E90E0
void DayNight::DNClouds::Update() {
    DNInfo& info = DayNight::g_dnInfo;

    if (!this->m_enabled || (info.m_skybox && info.m_skyboxWeight > 0.99000001f)) {
        this->m_fullUpdate = 0;
        return;
    }

    for (uint32_t i = 0; i < 3; i++) {
        if (info.m_blendSkybox[i] && info.m_blendSkyboxWeight[i] > 0.99000001f && !info.m_blendSkyboxFlags[i]) {
            this->m_fullUpdate = 0;
            return;
        }
    }

    uint32_t rowsPerUpdate = this->m_rowsPerUpdate;

    if (this->m_fullUpdate) {
        this->m_row = 0;
        this->m_rowsPerUpdate = this->m_size;
    }

    float density = info.m_light.m_floatBands[1];

    if (this->m_densityOverride != 0.0f) {
        density = this->m_densityOverride;
    }

    this->m_density = static_cast<uint8_t>(static_cast<int32_t>((1.0f - density) * 255.0f));

    uint32_t frame = this->m_animFrame >> 8;
    int32_t frameHash0 = s_perm[frame & 0xFF];
    int32_t frameHash1 = s_perm[(frame + 1) & 0xFF];

    CloudOctave octaves[4];
    const uint16_t* scales = s_octaveScales[this->m_lod];

    for (uint32_t i = 0; i < this->m_octaves; i++) {
        CloudOctave& octave = octaves[i];
        uint16_t scale = scales[i];
        octave.m_y = this->m_row * scale;
        octave.m_stepY = scale;
        octave.m_stepX = scale;
        octave.m_t0 = frame;
        octave.m_t0Copy = frame;
        octave.m_t1 = frame + 1;
        octave.m_wrap = ((scale << this->m_shift) >> 8) + frame + 2;
        octave.m_amplitude = 1.0f / static_cast<float>(1 << i);
        octave.m_t = this->m_animFrame;
    }

    C3Vector shadeColor(0.0f, 0.0f, 0.0f);
    C3Vector sunColor(0.0f, 0.0f, 0.0f);
    C3Vector baseColor(0.0f, 0.0f, 0.0f);
    C3Vector sunPos(0.0f, 0.0f, 0.0f);
    float intensity = 1.0f;
    this->SetupBumpMap(shadeColor, sunColor, baseColor, sunPos, intensity);

    float gradX = 0.0f;
    float gradY = 0.0f;

    for (uint32_t row = 0; row < this->m_rowsPerUpdate; row++) {
        uint32_t y = row + this->m_row;
        uint32_t offset = y << this->m_shift;
        uint8_t* densities = this->m_densities.m_data + offset;
        CImVector* texel = this->m_texels.m_data + offset;

        for (uint32_t i = 0; i < this->m_octaves; i++) {
            CloudOctave& octave = octaves[i];
            uint32_t y0 = octave.m_y >> 8;
            octave.m_y0 = y0;
            octave.m_lastX0 = 0xFFFFFFFF;
            octave.m_y1 = (y0 + 1) & 0xFF;
            octave.m_hash[0] = s_perm[(y0 + frameHash0) & 0xFF];
            octave.m_hash[1] = s_perm[((y0 + 1) + frameHash0) & 0xFF];
            octave.m_hash[2] = s_perm[(frameHash1 + y0) & 0xFF];
            octave.m_hash[3] = s_perm[(frameHash1 + ((y0 + 1) & 0xFF)) & 0xFF];
            octave.m_x = this->m_animFrame;
        }

        float left = 0.0f;
        float rowY = static_cast<float>(y);

        for (uint32_t x = 0; x < this->m_size; x++) {
            float value = 0.0f;

            for (uint32_t i = 0; i < this->m_octaves; i++) {
                CloudOctave& octave = octaves[i];
                uint32_t xFrac = octave.m_x & 0xFF;
                uint32_t yFrac = octave.m_y & 0xFF;
                uint32_t tFrac = octave.m_t & 0xFF;
                uint32_t x0 = octave.m_x >> 8;
                octave.m_x0 = x0;

                if (x0 != octave.m_lastX0) {
                    uint32_t h = x0 + octave.m_hash[0];
                    float c0 = s_noiseTable[s_perm[h & 0xFF]];
                    float c1 = s_noiseTable[s_perm[(h + 1) & 0xFF]];
                    octave.m_corner[0] = c0;
                    octave.m_corner[1] = c1 - c0;

                    h = x0 + octave.m_hash[1];
                    c0 = s_noiseTable[s_perm[h & 0xFF]];
                    c1 = s_noiseTable[s_perm[(h + 1) & 0xFF]];
                    octave.m_corner[2] = c0;
                    octave.m_corner[3] = c1 - c0;

                    h = x0 + octave.m_hash[2];
                    c0 = s_noiseTable[s_perm[h & 0xFF]];
                    c1 = s_noiseTable[s_perm[(h + 1) & 0xFF]];
                    octave.m_corner[4] = c0;
                    octave.m_corner[5] = c1 - c0;

                    octave.m_lastX0 = x0;
                    h = (octave.m_hash[3] + x0) & 0xFF;
                    c0 = s_noiseTable[s_perm[h]];
                    c1 = s_noiseTable[s_perm[(h + 1) & 0xFF]];
                    octave.m_corner[6] = c0;
                    octave.m_corner[7] = c1 - c0;
                }

                float sx = s_smoothTable[xFrac];
                float sy = s_smoothTable[yFrac];
                float a = octave.m_corner[1] * sx + octave.m_corner[0];
                float b = ((octave.m_corner[3] * sx + octave.m_corner[2]) - a) * sy + a;
                float c = octave.m_corner[5] * sx + octave.m_corner[4];
                float d = ((octave.m_corner[7] * sx + octave.m_corner[6]) - c) * sy + c;
                float noise = ((d - b) * s_smoothTable[tFrac] + b) * octave.m_amplitude;

                octave.m_x += octave.m_stepX;
                value = noise + value;

                if (i == 2) {
                    float scale = static_cast<float>(1 << (this->m_shift - 7));
                    gradX = (left - value) * scale;
                    gradY = (this->m_prevRow.m_data[x] - value) * scale;
                    left = value;
                    this->m_prevRow.m_data[x] = value;
                }
            }

            int32_t level = static_cast<uint8_t>(static_cast<int32_t>(value * 64.0f + 128.0f)) - this->m_density;
            uint8_t cloud = 0;

            if (level >= 0) {
                cloud = s_sharpnessTable[level];
            }

            densities[x] = cloud;

            if (cloud) {
                float shade = static_cast<float>(static_cast<uint8_t>((static_cast<uint8_t>(~cloud) >> 1) + 64)) * 0.0039215689f;
                float r = shadeColor.x * shade + baseColor.x;
                float g = shadeColor.y * shade + baseColor.y;
                float b = shadeColor.z * shade + baseColor.z;

                float lx = sunPos.x - static_cast<float>(x);
                float ly = sunPos.y - rowY;
                float lightLength = (lx * lx + sunPos.z * sunPos.z) + ly * ly;
                float normalLength = (gradY * gradY + gradX * gradX) + 1.0f;
                float lighting = ((ly * gradY + lx * gradX) + sunPos.z) * (FastInvSqrt(normalLength) * FastInvSqrt(lightLength));

                if (lighting > 0.0f) {
                    float sun = lighting * intensity;
                    r = sunColor.x * sun + r;
                    g = sunColor.y * sun + g;
                    b = sunColor.z * sun + b;
                }

                if (r > 1.0f) {
                    r = 1.0f;
                }

                if (g > 1.0f) {
                    g = 1.0f;
                }

                if (b > 1.0f) {
                    b = 1.0f;
                }

                texel[x].r = static_cast<uint8_t>(static_cast<int32_t>(r * 255.0f));
                texel[x].g = static_cast<uint8_t>(static_cast<int32_t>(g * 255.0f));
                texel[x].b = static_cast<uint8_t>(static_cast<int32_t>(b * 255.0f));
                texel[x].a = densities[x];
            } else if (x) {
                texel[x] = texel[x - 1];
                texel[x].a = 0;
            }
        }

        for (uint32_t i = 0; i < this->m_octaves; i++) {
            octaves[i].m_y += octaves[i].m_stepY;
        }
    }

    if (!this->m_textures[0]) {
        CGxTexFlags flags;
        flags.m_bit20 = 1;
        this->m_textures[0] = TextureCreate(this->m_size, this->m_size, this->m_texFormat, static_cast<EGxTexFormat>(2), flags, this->m_texels.m_data, DNClouds::Callback_GxTex, "DNClouds0", 0);
        this->m_textures[1] = TextureCreate(this->m_size, this->m_size, this->m_texFormat, static_cast<EGxTexFormat>(2), flags, this->m_texels.m_data, DNClouds::Callback_GxTex, "DNClouds1", 0);
    }

    CGxTex* texture = TextureGetGxTex(this->m_textures[(this->m_texIndex - 1) & 1]);

    if (texture) {
        GxTexUpdate(texture, 0, this->m_row, this->m_size, this->m_row + this->m_rowsPerUpdate, 1);
    }

    this->m_row += this->m_rowsPerUpdate;

    if (this->m_row >= this->m_size) {
        uint16_t animFrame = static_cast<uint16_t>(static_cast<int32_t>(static_cast<double>(info.m_timeMs) * this->m_animSpeed));

        if (animFrame == this->m_animFrame) {
            if (this->m_fullUpdate) {
                this->m_texIndex = (this->m_texIndex - 1) & 1;
            }
        } else {
            this->m_animFrame = animFrame;
            this->m_texIndex = (this->m_texIndex - 1) & 1;
        }

        this->m_row = 0;
    }

    if (this->m_fullUpdate) {
        this->m_rowsPerUpdate = rowsPerUpdate;
        this->m_fullUpdate = 0;
    }
}

// OFFSET: 0x0064E590
void DayNight::DNClouds::GenSphere(float radius) {
    float prevPhi = 0.0f;
    uint16_t prevStart = 0;
    float offset = -cos(0.78539819f);

    this->m_verts.SetCount(192);
    this->m_texCoords.SetCount(192);
    this->m_colors.SetCount(192);
    this->m_indices.SetCount(374);

    C3Vector* vert = this->m_verts.m_data;
    C2Vector* texCoord = this->m_texCoords.m_data;
    uint16_t* index = this->m_indices.m_data;
    CImVector* color = this->m_colors.m_data;
    CImVector white(0xFF, 0xFF, 0xFF, 0xFF);

    for (uint32_t ring = 0; ring < 12; ring++) {
        float phi = s_ringAngles[ring] * 3.1415927f;
        float sinPhi = sin(phi);
        float texRadius = 0.5f * (static_cast<float>(ring) * 0.090909094f);
        float z = radius * cos(phi) + offset;
        float theta = 0.0f;
        uint16_t start = static_cast<uint16_t>(vert - this->m_verts.m_data);
        bool top = CMath::fequal(phi, 0.0f);

        for (int32_t i = 0; ; ) {
            float cosTheta = cos(theta);
            float sinTheta = sin(theta);
            vert->x = sinTheta * sinPhi * radius;
            vert->y = sinPhi * cosTheta * radius;
            vert->z = z;
            vert++;

            white.a = s_ringAlphas[ring];
            *color++ = white;

            texCoord->x = sinTheta * texRadius + 0.5f;
            texCoord->y = cosTheta * texRadius + 0.5f;
            texCoord++;

            if (top || CMath::fequal(phi, 6.2831855f) || ++i >= 16) {
                break;
            }

            theta = theta + 0.39269909f;
        }

        if (ring) {
            bool prevTop = CMath::fequal(prevPhi, 0.0f);
            bool bottom = CMath::fequal(phi, 6.2831855f);

            for (int32_t i = 0; i < 17; i++) {
                *index++ = (prevTop ? 0 : i & 0xF) + prevStart;
                *index++ = (bottom ? 0 : i & 0xF) + start;
            }
        }

        prevPhi = phi;
        prevStart = start;
    }

    this->m_nVerts = static_cast<uint16_t>(vert - this->m_verts.m_data);
    this->m_nIndices = 374;
}

// OFFSET: 0x0064E880/0x1402EA300
void DayNight::DNClouds::SetLOD(uint32_t lod, uint32_t rowsPerUpdate) {
    if (this->m_textures[0] || this->m_textures[1]) {
        if (this->m_textures[0]) {
            TextureRelease(this->m_textures[0]);
        }

        if (this->m_textures[1]) {
            TextureRelease(this->m_textures[1]);
        }

        this->m_textures[0] = NULL;
        this->m_textures[1] = NULL;
        this->m_densities.Clear();
        this->m_texels.Clear();
        this->m_prevRow.Clear();
    }

    this->m_lod = static_cast<uint8_t>(lod);
    this->m_size = s_lodSizes[static_cast<uint8_t>(lod)];
    this->m_rowsPerUpdate = rowsPerUpdate ? rowsPerUpdate : 8;
    this->m_mask = this->m_size - 1;
    this->m_shift = s_lodShifts[static_cast<uint8_t>(lod)];

    this->m_texels.SetCount(this->m_size * this->m_size);
    memset(this->m_texels.m_data, 0xFF, sizeof(CImVector) * this->m_texels.m_count);
    this->m_densities.SetCount(this->m_size * this->m_size);
    this->m_prevRow.SetCount(this->m_size);
    memset(this->m_prevRow.m_data, 0, sizeof(float) * this->m_prevRow.m_count);

    this->m_row = 0;
    this->m_fullUpdate = 1;
}

// OFFSET: 0x0064EA20
void DayNight::DNClouds::Render() {
    if (!this->m_enabled) {
        return;
    }

    CTexture* texture = this->m_textures[this->m_texIndex];

    if (!texture) {
        return;
    }

    CGxTex* gxTex = TextureGetGxTex(texture);

    if (!gxTex) {
        return;
    }

    g_theGxDevicePtr->RsPush();

    C44Matrix world;
    world.Scale(6.6666665f);

    if (DNClouds::s_vertexShader) {
        g_theGxDevicePtr->RsSet(GxRs_VertexShader, &DNClouds::s_vertexShader->m_permutes[0]);
        g_theGxDevicePtr->RsSet(GxRs_PixelShader, &DNClouds::s_pixelShader->m_permutes[0]);

        C44Matrix view;
        GxXformView(view);
        C44Matrix proj;
        GxXformProjNative(proj);
        C44Matrix transform = world * view * proj;
        g_theGxDevicePtr->ShaderConstantsSet(GxSh_Vertex, 0, reinterpret_cast<const float*>(&transform), 4);
    } else {
        g_theGxDevicePtr->XformPushAndSet(GxXform_World, world);
    }

    GxRsSet(GxRs_BlendingMode, 2);
    GxUpdateAlphaRef(GxRs_AlphaRef);
    GxRsSet(GxRs_Lighting, 0);
    GxRsSet(GxRs_Fog, 0);
    GxRsSet(GxRs_DepthWrite, 0);
    GxRsSet(GxRs_Culling, 0);
    g_theGxDevicePtr->RsSet(GxRs_Texture0, gxTex);

    GxPrimVertexPtr(this->m_nVerts, this->m_verts.m_data, sizeof(C3Vector), NULL, 0, this->m_colors.m_data, sizeof(CImVector), this->m_texCoords.m_data, sizeof(C2Vector), NULL, 0);
    GxPrimIndexPtr(this->m_nIndices, this->m_indices.m_data);

    CGxBatch batch;
    batch.m_primType = GxPrim_TriangleStrip;
    batch.m_start = 0;
    batch.m_count = this->m_nIndices;
    batch.m_minIndex = 0;
    batch.m_maxIndex = this->m_nVerts - 1;
    g_theGxDevicePtr->Draw(batch);

    g_theGxDevicePtr->RsPop();

    if (!DNClouds::s_vertexShader) {
        GxXformPop(GxXform_World);
    }
}

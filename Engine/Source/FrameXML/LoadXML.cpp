#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <new>
#include <typeinfo>
#include "Engine/Source/FrameXML/LoadXML.h"
#include "Engine/Source/Base/Coordinate.h"
#include "Engine/Source/BlizzardCore/SFile2-Core.h"
#include "Engine/Source/Base/Status.h"
#include "Engine/Source/Frame/CSimpleAnimGroup.h"
#include "Engine/Source/Frame/CSimpleRender.h"
#include "Engine/Source/Frame/CSimpleTexture.h"
#include "Engine/Source/FrameXML/BinaryXML.h"
#include "Engine/Source/FrameXML/XMLTree.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/Storm/Source/MD5.h"
#include "Engine/Source/Tempest/C2Vector.h"
#include "Engine/Source/Tempest/CImVector.h"
#include "Engine/Source/lua-5.1/src/lua.h"

// STATIC: %0x00AD5670
static const XMLENUMVALUE s_frameStrata[] = {
    { FRAME_STRATA_WORLD, "WORLD" },
    { FRAME_STRATA_BACKGROUND, "BACKGROUND" },
    { FRAME_STRATA_LOW, "LOW" },
    { FRAME_STRATA_MEDIUM, "MEDIUM" },
    { FRAME_STRATA_HIGH, "HIGH" },
    { FRAME_STRATA_DIALOG, "DIALOG" },
    { FRAME_STRATA_FULLSCREEN, "FULLSCREEN" },
    { FRAME_STRATA_FULLSCREEN_DIALOG, "FULLSCREEN_DIALOG" },
    { FRAME_STRATA_TOOLTIP, "TOOLTIP" },
};

// STATIC: %0x00AD56B8
static const XMLENUMVALUE s_framePoints[] = {
    { FRAMEPOINT_BOTTOM, "BOTTOM" },
    { FRAMEPOINT_BOTTOMLEFT, "BOTTOMLEFT" },
    { FRAMEPOINT_BOTTOMRIGHT, "BOTTOMRIGHT" },
    { FRAMEPOINT_CENTER, "CENTER" },
    { FRAMEPOINT_TOP, "TOP" },
    { FRAMEPOINT_TOPRIGHT, "TOPRIGHT" },
    { FRAMEPOINT_TOPLEFT, "TOPLEFT" },
    { FRAMEPOINT_LEFT, "LEFT" },
    { FRAMEPOINT_RIGHT, "RIGHT" },
};

// STATIC: %0x00AD5700
static const XMLENUMVALUE s_drawLayers[] = {
    { 0, "BACKGROUND" },
    { 1, "BORDER" },
    { 2, "ARTWORK" },
    { 3, "OVERLAY" },
    { 4, "HIGHLIGHT" },
};

// STATIC: %0x00AD5728
static const XMLENUMVALUE s_blendModes[] = {
    { GxBlend_Opaque, "DISABLE" },
    { GxBlend_Alpha, "BLEND" },
    { GxBlend_AlphaKey, "ALPHAKEY" },
    { GxBlend_Add, "ADD" },
    { GxBlend_Mod, "MOD" },
};

// STATIC: %0x00AD5750
static const XMLENUMVALUE s_justify[] = {
    { 0x1, "LEFT" },
    { 0x2, "CENTER" },
    { 0x4, "RIGHT" },
    { 0x8, "TOP" },
    { 0x10, "MIDDLE" },
    { 0x20, "BOTTOM" },
};

// STATIC: %0x00AD5780
static const XMLENUMVALUE s_orientations[] = {
    { ORIENTATION_HORIZONTAL, "HORIZONTAL" },
    { ORIENTATION_VERTICAL, "VERTICAL" },
};

// STATIC: %0x00AD5790
static const XMLENUMVALUE s_fontFlags[] = {
    { 0x1, "OUTLINE" },
    { 0x4, "THICKOUTLINE" },
    { 0x2, "MONOCHROME" },
};

// STATIC: %0x00AD57A8
static const SMOOTHWEIGHTS s_smoothWeights[] = {
    { 0.0f, 0.0f, "NONE" },
    { 1.0f, 0.0f, "IN" },
    { 0.0f, 1.0f, "OUT" },
    { 1.0f, 1.0f, "IN_OUT" },
    { 1.0f, 1.0f, "OUT_IN" },
};

// STATIC: %0x00AD57E4
static const XMLENUMVALUE s_loopTypes[] = {
    { ANIM_LOOPTYPE_NONE, "NONE" },
    { ANIM_LOOPTYPE_REPEAT, "REPEAT" },
    { ANIM_LOOPTYPE_BOUNCE, "BOUNCE" },
};

// STATIC: %0x00AD57FC
static const XMLENUMVALUE s_loopStates[] = {
    { ANIM_LOOPSTATE_NONE, "NONE" },
    { ANIM_LOOPSTATE_FORWARD, "FORWARD" },
    { ANIM_LOOPSTATE_REVERSE, "REVERSE" },
};

// STATIC: %0x00AD5814
static const XMLENUMVALUE s_curveTypes[] = {
    { ANIM_CURVE_NONE, "NONE" },
    { ANIM_CURVE_SMOOTH, "SMOOTH" },
};

// STATIC: %0x00AD58BC
static const uint8_t s_signaturePublicExponent[4] = { 0x01, 0x00, 0x01, 0x00 };

// STATIC: %0x00CC91B0
static char s_fontFlagsString[64];

// OFFSET: 0x004EEAF0/0x140132F10
int32_t StringToFrameStrata(const char* string, FRAME_STRATA& strata) {
    for (uint32_t i = 0; i < sizeof(s_frameStrata) / sizeof(s_frameStrata[0]); i++) {
        if (!SStrCmpI(s_frameStrata[i].m_name, string, STORM_MAX_STR)) {
            strata = static_cast<FRAME_STRATA>(s_frameStrata[i].m_value);
            return 1;
        }
    }

    return 0;
}

// OFFSET: 0x004EEB40/0x140132F80
const char* FrameStrataToString(FRAME_STRATA strata) {
    for (uint32_t i = 0; i < sizeof(s_frameStrata) / sizeof(s_frameStrata[0]); i++) {
        if (strata == s_frameStrata[i].m_value) {
            return s_frameStrata[i].m_name;
        }
    }

    return "UNKNOWN";
}

// OFFSET: 0x004EEB70/0x140132FC0
int32_t StringToFramePoint(const char* string, FRAMEPOINT& point) {
    for (uint32_t i = 0; i < sizeof(s_framePoints) / sizeof(s_framePoints[0]); i++) {
        if (!SStrCmpI(s_framePoints[i].m_name, string, STORM_MAX_STR)) {
            point = static_cast<FRAMEPOINT>(s_framePoints[i].m_value);
            return 1;
        }
    }

    return 0;
}

// OFFSET: 0x004EEBC0/0x140133030
const char* FramePointToString(FRAMEPOINT point) {
    for (uint32_t i = 0; i < sizeof(s_framePoints) / sizeof(s_framePoints[0]); i++) {
        if (point == s_framePoints[i].m_value) {
            return s_framePoints[i].m_name;
        }
    }

    return "UNKNOWN";
}

// OFFSET: 0x004EEBF0/0x140133070
int32_t StringToDrawLayer(const char* string, uint32_t& layer) {
    for (uint32_t i = 0; i < sizeof(s_drawLayers) / sizeof(s_drawLayers[0]); i++) {
        if (!SStrCmpI(s_drawLayers[i].m_name, string, STORM_MAX_STR)) {
            layer = s_drawLayers[i].m_value;
            return 1;
        }
    }

    return 0;
}

// OFFSET: 0x004EEC40
const char* DrawLayerToString(uint32_t layer) {
    for (uint32_t i = 0; i < sizeof(s_drawLayers) / sizeof(s_drawLayers[0]); i++) {
        if (layer == s_drawLayers[i].m_value) {
            return s_drawLayers[i].m_name;
        }
    }

    return "UNKNOWN";
}

// OFFSET: 0x004EEC70/0x140133120
int32_t StringToBlendMode(const char* string, EGxBlend& blend) {
    for (uint32_t i = 0; i < sizeof(s_blendModes) / sizeof(s_blendModes[0]); i++) {
        if (!SStrCmpI(s_blendModes[i].m_name, string, STORM_MAX_STR)) {
            blend = static_cast<EGxBlend>(s_blendModes[i].m_value);
            return 1;
        }
    }

    return 0;
}

// OFFSET: 0x004EECC0/0x140133190
const char* BlendModeToString(EGxBlend blend) {
    for (uint32_t i = 0; i < sizeof(s_blendModes) / sizeof(s_blendModes[0]); i++) {
        if (blend == s_blendModes[i].m_value) {
            return s_blendModes[i].m_name;
        }
    }

    return "UNKNOWN";
}

// OFFSET: 0x004EECF0/0x1401331D0
int32_t StringToJustify(const char* string, uint32_t& justify) {
    for (uint32_t i = 0; i < sizeof(s_justify) / sizeof(s_justify[0]); i++) {
        if (!SStrCmpI(s_justify[i].m_name, string, STORM_MAX_STR)) {
            justify = s_justify[i].m_value;
            return 1;
        }
    }

    return 0;
}

// OFFSET: 0x004EED40/0x140133240
const char* JustifyToString(uint32_t justify) {
    for (uint32_t i = 0; i < sizeof(s_justify) / sizeof(s_justify[0]); i++) {
        if (justify & s_justify[i].m_value) {
            return s_justify[i].m_name;
        }
    }

    return "UNKNOWN";
}

// OFFSET: 0x004EED70/0x140133280
int32_t StringToOrientation(const char* string, ORIENTATION& orientation) {
    for (uint32_t i = 0; i < sizeof(s_orientations) / sizeof(s_orientations[0]); i++) {
        if (!SStrCmpI(s_orientations[i].m_name, string, STORM_MAX_STR)) {
            orientation = static_cast<ORIENTATION>(s_orientations[i].m_value);
            return 1;
        }
    }

    return 0;
}

// OFFSET: 0x004EEDC0/0x1401332F0
const char* OrientationToString(ORIENTATION orientation) {
    for (uint32_t i = 0; i < sizeof(s_orientations) / sizeof(s_orientations[0]); i++) {
        if (orientation == s_orientations[i].m_value) {
            return s_orientations[i].m_name;
        }
    }

    return "UNKNOWN";
}

// PROVEN: 8bd59284 DIFFERENT/IDENTICAL
// OFFSET: 0x004EEDF0/0x140133330
uint32_t StringToFontFlags(const char* string) {
    uint32_t flags = 0;

    for (uint32_t i = 0; i < sizeof(s_fontFlags) / sizeof(s_fontFlags[0]); i++) {
        if (SStrStrI(string, s_fontFlags[i].m_name)) {
            flags |= s_fontFlags[i].m_value;
        }
    }

    return flags;
}

// OFFSET: 0x004EF400/0x140133A20
const char* FontFlagsToString(uint32_t flags) {
    s_fontFlagsString[0] = '\0';

    for (uint32_t i = 0; i < sizeof(s_fontFlags) / sizeof(s_fontFlags[0]); i++) {
        if (flags & s_fontFlags[i].m_value) {
            if (s_fontFlagsString[0]) {
                SStrPack(s_fontFlagsString, ", ", sizeof(s_fontFlagsString));
            }

            SStrPack(s_fontFlagsString, s_fontFlags[i].m_name, sizeof(s_fontFlagsString));
        }
    }

    return s_fontFlagsString;
}

// OFFSET: 0x004EEE30
int32_t StringToSmoothWeights(const char* string, float& in, float& out) {
    for (uint32_t i = 0; i < sizeof(s_smoothWeights) / sizeof(s_smoothWeights[0]); i++) {
        if (!SStrCmpI(s_smoothWeights[i].m_name, string, STORM_MAX_STR)) {
            in = s_smoothWeights[i].m_in;
            out = s_smoothWeights[i].m_out;
            return 1;
        }
    }

    return 0;
}

// OFFSET: 0x004EEE90/0x140133490
int32_t StringToLoopType(const char* string, SimpleAnimLoopType& loop) {
    for (uint32_t i = 0; i < sizeof(s_loopTypes) / sizeof(s_loopTypes[0]); i++) {
        if (!SStrCmpI(s_loopTypes[i].m_name, string, STORM_MAX_STR)) {
            loop = static_cast<SimpleAnimLoopType>(s_loopTypes[i].m_value);
            return 1;
        }
    }

    return 0;
}

// OFFSET: 0x004EEEE0/0x140133500
const char* LoopTypeToString(SimpleAnimLoopType loop) {
    for (uint32_t i = 0; i < sizeof(s_loopTypes) / sizeof(s_loopTypes[0]); i++) {
        if (loop == s_loopTypes[i].m_value) {
            return s_loopTypes[i].m_name;
        }
    }

    return "UNKNOWN";
}

// OFFSET: 0x004EEF10/0x140133540
const char* LoopStateToString(SimpleAnimLoopState state) {
    for (uint32_t i = 0; i < sizeof(s_loopStates) / sizeof(s_loopStates[0]); i++) {
        if (state == s_loopStates[i].m_value) {
            return s_loopStates[i].m_name;
        }
    }

    return "UNKNOWN";
}

// OFFSET: 0x004EEF40/0x140133580
int32_t StringToCurveType(const char* string, SimpleAnimCurveType& curve) {
    for (uint32_t i = 0; i < sizeof(s_curveTypes) / sizeof(s_curveTypes[0]); i++) {
        if (!SStrCmpI(s_curveTypes[i].m_name, string, STORM_MAX_STR)) {
            curve = static_cast<SimpleAnimCurveType>(s_curveTypes[i].m_value);
            return 1;
        }
    }

    return 0;
}

// OFFSET: 0x004EEF90/0x1401335F0
const char* CurveTypeToString(SimpleAnimCurveType curve) {
    for (uint32_t i = 0; i < sizeof(s_curveTypes) / sizeof(s_curveTypes[0]); i++) {
        if (curve == s_curveTypes[i].m_value) {
            return s_curveTypes[i].m_name;
        }
    }

    return "UNKNOWN";
}

// OFFSET: 0x004EEFD0/0x140133630
static int32_t StringToBOOL(const char* string, int32_t defaultValue) {
    if (!string) {
        return defaultValue;
    }

    switch (*string) {
        case '0':
        case 'F':
        case 'N':
        case 'f':
        case 'n':
            return 0;

        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
        case 'T':
        case 'Y':
        case 't':
        case 'y':
            return 1;
    }

    if (!SStrCmpI(string, "off", STORM_MAX_STR) || !SStrCmpI(string, "disabled", STORM_MAX_STR)) {
        return 0;
    }

    if (!SStrCmpI(string, "on", STORM_MAX_STR) || !SStrCmpI(string, "enabled", STORM_MAX_STR)) {
        return 1;
    }

    return defaultValue;
}

// PROVEN: 9460201f DIFFERENT/IDENTICAL
// OFFSET: 0x004EF0A0/0x140133740
int32_t StringToBOOL(const char* string) {
    return StringToBOOL(string, 0);
}

// OFFSET: 0x004EF0C0/0x140133750
int32_t StringToBOOL(lua_State* L, int32_t index, int32_t defaultValue) {
    switch (lua_type(L, index)) {
        case LUA_TNIL:
            return 0;

        case LUA_TBOOLEAN:
            return lua_toboolean(L, index);

        case LUA_TNUMBER:
            return lua_tonumber(L, index) != 0.0;

        case LUA_TSTRING:
            return StringToBOOL(lua_tolstring(L, index, NULL), defaultValue);

        default:
            return defaultValue;
    }
}

// OFFSET: 0x004EF4C0/0x140133420
const char* SmoothWeightsToString(float in, float out) {
    for (uint32_t i = 0; i < sizeof(s_smoothWeights) / sizeof(s_smoothWeights[0]); i++) {
        if (fabs(in - s_smoothWeights[i].m_in) < 0.001f && fabs(out - s_smoothWeights[i].m_out) < 0.001f) {
            return s_smoothWeights[i].m_name;
        }
    }

    return "UNKNOWN";
}

// OFFSET: 0x004EE8D0
bool Dimension::GetDimensions(float& x, float& y, CStatus* status) {
    x = this->m_x;
    y = this->m_y;

    return (this->m_dimensionFlags & 3) != 0;
}

// OFFSET: 0x004EE900/0x140132D00
bool Dimension::Load(float& x, float& y, CStatus* status) {
    x = this->m_x;
    y = this->m_y;

    if (this->m_dimensionFlags & 3) {
        return true;
    }

    Dimension* child = static_cast<Dimension*>(this->m_child);

    if (!child) {
        return false;
    }

    if (child->m_element != XML_ABSDIMENSION && child->m_element != XML_RELDIMENSION) {
        status->Add(STATUS_WARNING, "Unknown child node in %s element: %s", this->m_type.m_string, child->m_type.m_string);
        return false;
    }

    return child->GetDimensions(x, y, status);
}

// OFFSET: 0x004EE980/0x140132DA0
bool Value::Load(float& value, CStatus* status) {
    value = this->m_value;

    if (this->m_valueFlags & 1) {
        return true;
    }

    Value* child = static_cast<Value*>(this->m_child);

    if (!child) {
        status->Add(STATUS_WARNING, "No \"val\" attribute in element: %s", this->m_type.m_string);
        return false;
    }

    if (child->m_element != XML_ABSVALUE && child->m_element != XML_RELVALUE) {
        status->Add(STATUS_WARNING, "Unknown child node in %s element: %s", this->m_type.m_string, child->m_type.m_string);
        return false;
    }

    value = child->m_value;

    return child->m_valueFlags & 1;
}

// OFFSET: 0x004EEA10/0x140132E40
bool Insets::Load(float& left, float& right, float& top, float& bottom, CStatus* status) {
    left = this->m_left;
    right = this->m_right;
    top = this->m_top;
    bottom = this->m_bottom;

    if (this->m_insetFlags & 1) {
        return true;
    }

    Insets* child = static_cast<Insets*>(this->m_child);

    if (!child) {
        status->Add(STATUS_WARNING, "No \"right\", \"left\", \"top\", or \"bottom\" attributes in element: %s", this->m_type.m_string);
        return false;
    }

    if (child->m_element == XML_ABSINSET) {
        left = child->m_left;
        right = child->m_right;
        top = child->m_top;
        bottom = child->m_bottom;

        return child->m_insetFlags & 1;
    }

    if (child->m_element == XML_RELINSET) {
        left = child->m_left;
        right = child->m_right;
        top = child->m_top;
        bottom = child->m_bottom;

        return child->m_insetFlags & 1;
    }

    status->Add(STATUS_WARNING, "Unknown child node in %s element: %s", this->m_type.m_string, child->m_type.m_string);

    return false;
}

// PROVEN: 6ccead45 IDENTICAL/UNPROVEN
// OFFSET: 0x004EF150/0x140133810
int32_t LoadXML_Value(linkObject* node, float& value, CStatus* status) {
    return static_cast<Value*>(node)->Load(value, status) != 0;
}

// OFFSET: 0x004EF170/0x140133830
int32_t LoadXML_Dimensions(linkObject* node, float& x, float& y, CStatus* status) {
    return static_cast<Dimension*>(node)->Load(x, y, status);
}

// PROVEN: 3fd4757d IDENTICAL/UNPROVEN
// OFFSET: 0x004EF1A0/0x140133850
int32_t LoadXML_Insets(linkObject* node, float& left, float& right, float& top, float& bottom, CStatus* status) {
    return static_cast<Insets*>(node)->Load(left, right, top, bottom, status) != 0;
}

// OFFSET: 0x004EF1D0/0x140133880
CSimpleAnimGroup* LoadXML_AnimGroup(linkObject* node, CScriptRegion* region, CStatus* status) {
    CSimpleAnimGroup* group = new CSimpleAnimGroup(region);

    group->PreLoadXML(node, status);
    group->LoadXML(node, status);
    group->PostLoadXML(node, status);

    return group;
}

// INLINED
static uint32_t PackColor(float r, float g, float b, float a) {
    return static_cast<uint8_t>(static_cast<int32_t>(b * 255.0f + 0.5f)) | ((static_cast<uint8_t>(static_cast<int32_t>(g * 255.0f + 0.5f)) | ((static_cast<uint8_t>(static_cast<int32_t>(r * 255.0f + 0.5f)) | (static_cast<uint8_t>(static_cast<int32_t>(a * 255.0f + 0.5f)) << 8)) << 8)) << 8);
}

// PROVEN: 90fad40e IDENTICAL/IDENTICAL
// OFFSET: 0x004EF230/0x140133910
bool Color::GetColor(CImVector& color, CStatus* status) {
    if (this->m_colorFlags & 1) {
        reinterpret_cast<uint32_t&>(color) = PackColor(this->m_r, this->m_g, this->m_b, this->m_a);
    }

    return this->m_colorFlags & 1;
}

// OFFSET: 0x004EF2C0
bool Gradient::Load(ORIENTATION& orientation, CImVector& minColor, CImVector& maxColor, CStatus* status) {
    orientation = static_cast<ORIENTATION>(this->m_orientation);

    for (linkObject* child = this->m_child; child; child = child->m_next) {
        Color* color = static_cast<Color*>(child);

        if (child->m_element == XML_MINCOLOR) {
            if (color->m_colorFlags & 1) {
                reinterpret_cast<uint32_t&>(minColor) = PackColor(color->m_r, color->m_g, color->m_b, color->m_a);
            }
        } else if (child->m_element == XML_MAXCOLOR && (color->m_colorFlags & 1)) {
            reinterpret_cast<uint32_t&>(maxColor) = PackColor(color->m_r, color->m_g, color->m_b, color->m_a);
        }
    }

    return true;
}

// PROVEN: db749763 IDENTICAL/IDENTICAL
// OFFSET: 0x004EF520/0x140133B30
int32_t LoadXML_Color(linkObject* node, CImVector& color, CStatus* status) {
    Color* colorNode = static_cast<Color*>(node);
    reinterpret_cast<uint32_t&>(color) = PackColor(colorNode->m_r, colorNode->m_g, colorNode->m_b, colorNode->m_a);

    return 1;
}

// PROVEN: 8bd70daf IDENTICAL/UNPROVEN
// OFFSET: 0x004EF5B0/0x140133BC0
int32_t LoadXML_Gradient(linkObject* node, ORIENTATION& orientation, CImVector& minColor, CImVector& maxColor, CStatus* status) {
    return static_cast<Gradient*>(node)->Load(orientation, minColor, maxColor, status) != 0;
}

// PROVEN: 9a30f9fb IDENTICAL/UNPROVEN
// OFFSET: 0x004EF5E0/0x140133BF0
int32_t LoadXML_AnimOrigin(linkObject* node, FRAMEPOINT& point, C2Vector& offset, CStatus* status) {
    Origin* origin = static_cast<Origin*>(node);
    point = FRAMEPOINT_CENTER;
    offset.x = 0.0f;
    offset.y = 0.0f;

    if (origin->m_originFlags & 1) {
        point = static_cast<FRAMEPOINT>(origin->m_point);

        for (linkObject* child = origin->m_child; child; child = child->m_next) {
            if (child->m_element == XML_OFFSET) {
                static_cast<Dimension*>(child)->Load(offset.x, offset.y, status);
                break;
            }
        }
    }

    return 1;
}

// OFFSET: 0x004EF650/0x140133C50
CSimpleTexture* LoadXML_Texture(linkObject* node, CSimpleFrame* frame, CStatus* status) {
    void* data = CSimpleTexture::s_allocator.GetData(0, typeid(CSimpleTexture).raw_name(), SLOG_OBJECT);
    CSimpleTexture* texture = data ? new (data) CSimpleTexture(frame, 2, 0, 1) : NULL;

    texture->PreLoadXML(node, status);
    texture->LoadXML(node, status);
    texture->PostLoadXML(node, status);

    return texture;
}

// OFFSET: 0x004EF6C0/0x140133CF0
CSimpleFontString* LoadXML_String(linkObject* node, CSimpleFrame* frame, CStatus* status) {
    void* data = CSimpleFontString::s_allocator.GetData(0, typeid(CSimpleFontString).raw_name(), SLOG_OBJECT);
    CSimpleFontString* string = data ? new (data) CSimpleFontString(frame, 2, true) : NULL;

    string->PreLoadXML(node, status);
    string->LoadXML(node, status);
    string->PostLoadXML(node, status);

    return string;
}

// PROVEN: 6d9f9abd IDENTICAL/NONE
// OFFSET: 0x004EF730
static void ProcessTocForMakeSign(MD5_CTX* md5, const char* text, const char* basePath) {
    const char* current = text;
    char line[1024];

    do {
        SStrTokenize(&current, line, sizeof(line), "\r\n", NULL);

        if (!line[0]) {
            break;
        }

        if (line[0] != '#') {
            ProcessFileForMakeSign(md5, line, basePath);
        }
    } while (line[0] && *current);
}

// OFFSET: 0x004EF7A0/0x140133D90
static void ParseManifestTag(char* tag, int32_t length, MD5_CTX* md5, const char* basePath) {
    char* end = tag + length;
    char* name = tag;

    while ((*name == '\r' || *name == '\n' || *name == ' ' || *name == '\t') && name < end) {
        name++;
    }

    if (name == end) {
        return;
    }

    char* nameEnd = name;

    while (*nameEnd != '\r' && *nameEnd != '\n' && *nameEnd != ' ' && *nameEnd != '\t' && nameEnd < end) {
        nameEnd++;
    }

    if (nameEnd == end) {
        return;
    }

    *nameEnd = '\0';

    if (SStrCmpI(name, "Script", STORM_MAX_STR) && SStrCmpI(name, "Include", STORM_MAX_STR)) {
        return;
    }

    char* attribute = name;
    char* attributeEnd = nameEnd;

    while (attribute < end) {
        attribute = attributeEnd + 1;

        while ((*attribute == '\r' || *attribute == '\n' || *attribute == ' ' || *attribute == '\t') && attribute < end) {
            attribute++;
        }

        if (attribute == end) {
            return;
        }

        attributeEnd = attribute;

        while (*attributeEnd != '\r' && *attributeEnd != '\n' && *attributeEnd != ' ' && *attributeEnd != '\t' && *attributeEnd != '=' && attributeEnd < end) {
            attributeEnd++;
        }

        if (attributeEnd == end) {
            return;
        }

        *attributeEnd = '\0';

        if (!SStrCmpI(attribute, "file", STORM_MAX_STR)) {
            break;
        }
    }

    if (attribute == end) {
        return;
    }

    char* value = attributeEnd + 1;

    while ((*value == '\r' || *value == '\n' || *value == ' ' || *value == '\t' || *value == '=' || *value == '"') && value < end) {
        value++;
    }

    if (value == end) {
        return;
    }

    char* valueEnd = value;

    while (*valueEnd != '\r' && *valueEnd != '\n' && *valueEnd != '\t' && *valueEnd != '"' && *valueEnd != '/' && valueEnd < end) {
        valueEnd++;
    }

    *valueEnd = '\0';

    ProcessFileForMakeSign(md5, value, basePath);
}

// OFFSET: 0x004EF900
static void ProcessXMLForMakeSign(char* text, uint32_t size, MD5_CTX* md5, const char* basePath) {
    char* end = text + size;
    char* current = text;

    while (*current != '<' && current < end) {
        current++;
    }

    char* tag = current + 1;
    char* cursor = tag;

    while (cursor < end) {
        if (*cursor == '>') {
            *cursor = '\0';
            ParseManifestTag(tag, cursor - tag, md5, basePath);

            char* next = cursor + 1;

            while (*next != '<' && next < end) {
                next++;
            }

            tag = next + 1;
            cursor = tag;
        } else {
            cursor++;
        }
    }
}

// OFFSET: 0x004EF9F0/0x140134020
int32_t ProcessFileForMakeSign(MD5_CTX* md5, const char* filename, const char* basePath) {
    char path[260];
    SStrPrintf(path, sizeof(path), "%s%s", basePath, filename);

    char* end = &path[SStrLen(path)];

    while (end[-1] == ' ') {
        if (end <= path) {
            break;
        }

        end--;
    }

    *end = '\0';

    if (SStrStr(path, "..")) {
        for (char* dots = SStrStr(path, ".."); dots; dots = SStrStr(path, "..")) {
            char before = dots[-1];

            if (before != '\\' && before != '/') {
                break;
            }

            char after = dots[2];

            if (after != '\\' && after != '/') {
                break;
            }

            char* dir = dots - 2;

            while (dir >= path && *dir != '\\' && *dir != '/') {
                dir--;
            }

            SStrCopy(dir + 1, dots + 3, STORM_MAX_STR);
        }
    }

    char directory[260];
    memset(directory, 0, sizeof(directory));

    char* backslash = SStrChrR(path, '\\');
    char* slash = SStrChrR(path, '/');

    if (backslash > slash) {
        slash = backslash;
    }

    if (slash) {
        uint32_t length = slash - path + 2;

        if (length < sizeof(directory)) {
            SStrCopy(directory, path, slash - path + 2);
            directory[length] = '\0';
        }
    }

    void* data;
    uint32_t size;
    int32_t result = SFile::ReadFile(NULL, path, &data, &size, 1, 1);

    if (!result) {
        return result;
    }

    MD5Update(md5, static_cast<const uint8_t*>(data), size);

    const char* extension = SStrChrR(const_cast<char*>(filename), '.');

    if (extension) {
        if (!SStrCmpI(extension, ".toc", STORM_MAX_STR)) {
            ProcessTocForMakeSign(md5, static_cast<const char*>(data), directory);
        } else if (!SStrCmpI(extension, ".xml", STORM_MAX_STR)) {
            ProcessXMLForMakeSign(static_cast<char*>(data), size, md5, directory);
        }
    }

    SFile::Unload(data);

    return 1;
}

// OFFSET: 0x004EFC30/0x1401343C0
static void CalculateSignatureMD5(const char* tocFile, const char* extraFile, uint8_t* digest) {
    MD5_CTX context;
    MD5Init(&context);
    ProcessFileForMakeSign(&context, tocFile, "");

    void* data;
    uint32_t size;

    if (extraFile && SFile::ReadFile(NULL, extraFile, &data, &size, 1, 1)) {
        MD5Update(&context, static_cast<const uint8_t*>(data), size);
        SFile::Unload(data);
    }

    MD5Final(digest, &context);
}

// OFFSET: 0x004EF960/0x140133F50
static int32_t VerifySignature(const uint8_t* data, uint32_t size, const uint8_t* modulus, const char* name) {
    if (!modulus) {
        return 0;
    }

    SSignatureData* token;
    SSignatureVerifyStream_Begin(&token, 256, 4);

    uint32_t signatureLength = SSignatureVerifyStream_GetSignatureLength(token);
    SSignatureVerifyStream_ProvideData(token, data, size - signatureLength);
    SSignatureVerifyStream_ProvideData(token, reinterpret_cast<const uint8_t*>(name), SStrLen(name));
    SSignatureVerifyStream_ProvideData(token, &data[size - signatureLength], signatureLength);

    return SSignatureVerifyStream_Finish(token, modulus, s_signaturePublicExponent);
}

// OFFSET: 0x004EFCA0/0x140134480
int32_t FrameXML_CheckSignature(const char* tocFile, const char* extraFile, const uint8_t* modulus, uint8_t* digest) {
#ifdef DISABLE_SIGNATURE_CHECKS
    CalculateSignatureMD5(tocFile, extraFile, digest);
    return 3;
#endif

    char sigFile[260];
    SStrPrintf(sigFile, sizeof(sigFile), "%s.sig", tocFile);

    void* data;
    uint32_t size;

    if (!SFile::ReadFile(NULL, sigFile, &data, &size, 0, 1)) {
        return 0;
    }

    if (size != 276) {
        SFile::Unload(data);
        return 1;
    }

    char* name = SStrChrR(sigFile, '\\');
    name = name ? name + 1 : sigFile;
    SStrUpper(name);

    if (!VerifySignature(static_cast<const uint8_t*>(data), size, modulus, name)) {
        SFile::Unload(data);
        return 1;
    }

    uint8_t signedDigest[16];
    memcpy(signedDigest, data, sizeof(signedDigest));
    SFile::Unload(data);

    CalculateSignatureMD5(tocFile, extraFile, digest);

    return memcmp(signedDigest, digest, sizeof(signedDigest)) ? 2 : 3;
}

#ifndef ENGINE_SOURCE_FRAMEXML_BINARYXML_H
#define ENGINE_SOURCE_FRAMEXML_BINARYXML_H

#include <cstddef>
#include <cstdint>
#include "Engine/Source/Frame/Types.h"
#include "Engine/Source/Storm/H/stpl.h"

class CImVector;
class CStatus;
class XMLNode;

enum XMLELEMENT {
    XML_UI = 1,
    XML_ONFINISHED = 2,
    XML_ONPLAY = 3,
    XML_ONPAUSE = 4,
    XML_ONSTOP = 5,
    XML_ONCOLORSELECT = 6,
    XML_ONTOOLTIPSETDEFAULTANCHOR = 7,
    XML_ONTOOLTIPADDMONEY = 8,
    XML_ONTOOLTIPCLEARED = 9,
    XML_ONTOOLTIPSETUNIT = 10,
    XML_ONTOOLTIPSETITEM = 11,
    XML_ONSPACEPRESSED = 12,
    XML_ONTEXTSET = 13,
    XML_ONINPUTLANGUAGECHANGED = 14,
    XML_ONMINMAXCHANGED = 15,
    XML_ONCURSORCHANGED = 16,
    XML_ONANIMFINISHED = 17,
    XML_ONMOVIESHOWSUBTITLE = 19,
    XML_ONMOVIEHIDESUBTITLE = 20,
    XML_ONMOVIEFINISHED = 21,
    XML_ONCLICK = 22,
    XML_PRECLICK = 23,
    XML_POSTCLICK = 24,
    XML_ONDOUBLECLICK = 25,
    XML_ATTRIBUTE = 26,
    XML_ATTRIBUTES = 27,
    XML_TITLEREGION = 28,
    XML_RESIZEBOUNDS = 29,
    XML_BACKDROP = 30,
    XML_HITRECTINSETS = 31,
    XML_LAYERS = 32,
    XML_LAYER = 33,
    XML_SCRIPTS = 34,
    XML_FRAMES = 37,
    XML_SCROLLCHILD = 38,
    XML_SIMPLEHTML = 39,
    XML_ONTABPRESSED = 40,
    XML_ONEDITFOCUSLOST = 41,
    XML_ONEDITFOCUSGAINED = 42,
    XML_ONTEXTCHANGED = 43,
    XML_ONUPDATEMODEL = 44,
    XML_ONLOAD = 46,
    XML_ONSIZECHANGED = 47,
    XML_ONUPDATE = 48,
    XML_ONSHOW = 49,
    XML_ONHIDE = 50,
    XML_ONENTER = 51,
    XML_ONLEAVE = 52,
    XML_ONMOUSEDOWN = 53,
    XML_ONMOUSEUP = 54,
    XML_ONDRAGSTART = 60,
    XML_ONDRAGSTOP = 61,
    XML_ONRECEIVEDRAG = 62,
    XML_ONCHAR = 63,
    XML_ONHYPERLINKCLICK = 64,
    XML_ONEVENT = 65,
    XML_ONKEYDOWN = 66,
    XML_ONKEYUP = 67,
    XML_ONATTRIBUTECHANGED = 68,
    XML_ONENABLE = 69,
    XML_ONDISABLE = 70,
    XML_INCLUDE = 71,
    XML_SCRIPT = 72,
    XML_FONT = 73,
    XML_FRAME = 74,
    XML_NORMALTEXTURE = 76,
    XML_PUSHEDTEXTURE = 77,
    XML_DISABLEDTEXTURE = 78,
    XML_HIGHLIGHTTEXTURE = 79,
    XML_BUTTONTEXT = 80,
    XML_DISABLEDFONT = 81,
    XML_NORMALFONT = 82,
    XML_HIGHLIGHTFONT = 83,
    XML_NORMALCOLOR = 84,
    XML_DISABLEDCOLOR = 85,
    XML_HIGHLIGHTCOLOR = 86,
    XML_BORDERCOLOR = 87,
    XML_BARCOLOR = 88,
    XML_PUSHEDTEXTOFFSET = 89,
    XML_CHECKEDTEXTURE = 90,
    XML_DISABLEDCHECKEDTEXTURE = 91,
    XML_FONTHEIGHT = 92,
    XML_BUTTON = 93,
    XML_CHECKBUTTON = 94,
    XML_COLORSELECT = 95,
    XML_COOLDOWN = 96,
    XML_DRESSUPMODEL = 97,
    XML_EDITBOX = 98,
    XML_FONTSTRING = 99,
    XML_GAMETOOLTIP = 100,
    XML_MESSAGEFRAME = 101,
    XML_MINIMAP = 102,
    XML_MODEL = 103,
    XML_MODELFFX = 104,
    XML_MOVIEFRAME = 106,
    XML_PLAYERMODEL = 107,
    XML_BLOBFRAME = 108,
    XML_QUESTPOIFRAME = 109,
    XML_ARCHAEOLOGYDIGSITEFRAME = 110,
    XML_SCROLLFRAME = 111,
    XML_SCROLLINGMESSAGEFRAME = 112,
    XML_SLIDER = 113,
    XML_STATUSBAR = 114,
    XML_TABARDMODEL = 115,
    XML_TEXTURE = 116,
    XML_WORLDFRAME = 117,
    XML_COLORWHEELTEXTURE = 118,
    XML_COLORWHEELTHUMBTEXTURE = 119,
    XML_COLORVALUETEXTURE = 120,
    XML_COLORVALUETHUMBTEXTURE = 121,
    XML_SIZE = 122,
    XML_ANCHORS = 123,
    XML_ANCHOR = 124,
    XML_ANIMATIONS = 125,
    XML_COLOR = 126,
    XML_GRADIENT = 127,
    XML_TEXCOORDS = 128,
    XML_REGION = 129,
    XML_MINCOLOR = 131,
    XML_MAXCOLOR = 132,
    XML_OFFSET = 133,
    XML_SHADOW = 134,
    XML_ONENTERPRESSED = 135,
    XML_ONESCAPEPRESSED = 136,
    XML_ONSCROLLRANGECHANGED = 137,
    XML_ONVERTICALSCROLL = 138,
    XML_ONMOUSEWHEEL = 139,
    XML_ONVALUECHANGED = 140,
    XML_FONTSTRINGHEADER1 = 141,
    XML_FONTSTRINGHEADER2 = 142,
    XML_FONTSTRINGHEADER3 = 143,
    XML_THUMBTEXTURE = 144,
    XML_BARTEXTURE = 145,
    XML_ANIMATIONGROUP = 146,
    XML_ANIMATION = 147,
    XML_ALPHA = 148,
    XML_TRANSLATION = 149,
    XML_EDGESIZE = 150,
    XML_TILESIZE = 151,
    XML_BACKGROUNDINSETS = 152,
    XML_FOGCOLOR = 153,
    XML_TEXTINSETS = 154,
    XML_ABSINSET = 156,
    XML_RELINSET = 157,
    XML_MINRESIZE = 158,
    XML_MAXRESIZE = 159,
    XML_ABSDIMENSION = 160,
    XML_RELDIMENSION = 161,
    XML_ABSVALUE = 163,
    XML_RELVALUE = 164,
    XML_ROTATION = 165,
    XML_PATH = 166,
    XML_SCALE = 167,
    XML_ORIGIN = 169,
    XML_CONTROLPOINT = 171,
    XML_CONTROLPOINTS = 172,
    XML_RECT = 173,
};

class linkObject;
class MemBlock;

typedef bool (*XMLChildFunc)(linkObject* node, MemBlock& block, char* name);

class MemBlock {
    public:
    /* 0000 */ char* m_block;
    /* 0004 */ char* m_cur;
    /* 0008 */ char* m_end;
    /* 000C */ char* m_strings;
    /* 0010 */ char* m_stringsEnd;
    /* 0014 */ TSGrowableArray<void*> m_allocations;

    MemBlock();
    ~MemBlock();
    bool Advance();
    bool Reserve(uint32_t bytes);
};

class XMLString {
    public:
    /* 0000 */ int32_t m_next;
    /* 0004 */ char* m_string;
};

class linkObject {
    public:
    /* 0000 */ int32_t m_strings;
    /* 0004 */ void* m_allocation;
    /* 0008 */ XMLString m_type;
    /* 0010 */ XMLString m_inherits;
    /* 0018 */ XMLString m_name;
    /* 0020 */ uint32_t m_size;
    /* 0024 */ XMLNode* m_xml;
    /* 0028 */ linkObject* m_child;
    /* 002C */ linkObject* m_next;
    /* 0030 */ uint32_t m_flags;
    /* 0034 */ uint32_t m_element;

    bool AddString(MemBlock& block, XMLString& field, const char* string);
    linkObject* Compile(uint32_t& size);
    uint32_t CalculateSize();
    bool dochildren(XMLChildFunc func, MemBlock& block);
    linkObject* GetChild(uint32_t element);
    bool parse(MemBlock& block);
    bool Relocate(char*& dest);
    void Recycle(bool force);
    bool RelocateStrings(char*& dest);
};

class Ui : public linkObject {
};

class Include : public linkObject {
    public:
    /* 0038 */ uint32_t m_includeFlags;
    /* 003C */ XMLString m_file;
};

class Script : public linkObject {
    public:
    /* 0038 */ uint32_t m_bodyFlags;
    /* 003C */ XMLString m_body;
    /* 0044 */ uint32_t m_fileFlags;
    /* 0048 */ XMLString m_file;
};

class Function : public linkObject {
    public:
    /* 0038 */ uint32_t m_bodyFlags;
    /* 003C */ XMLString m_body;
    /* 0044 */ uint32_t m_functionFlags;
    /* 0048 */ XMLString m_function;
    /* 0050 */ int32_t* m_ref;

    bool parse(MemBlock& block);
};

class Font : public linkObject {
    public:
    /* 0038 */ uint32_t m_fontFlags;
    /* 003C */ XMLString m_font;
    /* 0044 */ float m_spacing;
    /* 0048 */ XMLString m_justifyV;
    /* 0050 */ XMLString m_justifyH;
    /* 0058 */ XMLString m_outline;

    bool parseFont(MemBlock& block);
};

class Region : public Font {
    public:
    /* 0060 */ uint32_t m_layoutFlags;
    /* 0064 */ uint32_t m_regionFlags;
    /* 0068 */ XMLString m_parentKey;
    /* 0070 */ float m_alpha;
    /* 0074 */ uint32_t m_regionType;

    bool parseLayout(MemBlock& block);
    bool parse(MemBlock& block);
};

class Texture : public Region {
    public:
    /* 0078 */ uint32_t m_textureFlags;
    /* 007C */ uint32_t m_alphaMode;
    /* 0080 */ XMLString m_file;
    /* 0088 */ XMLString m_mask;

    bool parse(MemBlock& block);
};

class FontString : public Region {
    public:
    /* 0078 */ uint32_t m_stringFlags;
    /* 007C */ XMLString m_text;
    /* 0084 */ int32_t m_bytes;
    /* 0088 */ int32_t m_maxLines;
    /* 008C */ float m_scale;
    /* 0090 */ float m_rotation;

    bool parse(MemBlock& block);
};

class Frame : public Region {
    public:
    /* 0078 */ uint32_t m_frameFlags;
    /* 007C */ uint32_t m_frameFlags2;
    /* 0080 */ uint32_t m_drawLayer;
    /* 0084 */ int32_t m_id;
    /* 0088 */ XMLString m_parent;
    /* 0090 */ uint32_t m_strata;
    /* 0094 */ float m_depth;
    /* 0098 */ int32_t m_frameLevel;

    bool parse(MemBlock& block);
};

class Button : public Frame {
    public:
    /* 009C */ uint32_t m_buttonFlags;
    /* 00A0 */ XMLString m_text;
    /* 00A8 */ XMLString m_registerForClicks;

    bool parse(MemBlock& block);
};

class CheckButton : public Button {
    public:
    /* 00B0 */ uint32_t m_checkFlags;

    bool parse(MemBlock& block);
};

class StatusBar : public Frame {
    public:
    /* 009C */ uint32_t m_valueFlags;
    /* 00A0 */ uint32_t m_orientation;
    /* 00A4 */ float m_minValue;
    /* 00A8 */ float m_maxValue;
    /* 00AC */ float m_defaultValue;

    bool parseValues(MemBlock& block);
    bool parse(MemBlock& block);
};

class Slider : public StatusBar {
    public:
    /* 00B0 */ float m_valueStep;

    bool parse(MemBlock& block);
};

class Cooldown : public Frame {
    public:
    /* 009C */ uint32_t m_cooldownFlags;

    bool parse(MemBlock& block);
};

class Model : public Frame {
    public:
    /* 009C */ uint32_t m_modelFlags;
    /* 00A0 */ float m_scale;
    /* 00A4 */ float m_fogNear;
    /* 00A8 */ float m_fogFar;
    /* 00AC */ XMLString m_file;

    bool parse(MemBlock& block);
};

class ModelFFX : public Model {
    public:
    /* 00B4 */ uint32_t m_ffxFlags;
    /* 00B8 */ float m_glow;
};

class MessageFrame : public Frame {
    public:
    /* 009C */ uint32_t m_insertMode;
    /* 00A0 */ uint32_t m_messageFlags;
    /* 00A4 */ float m_displayDuration;
    /* 00A8 */ float m_fadeDuration;

    bool parse(MemBlock& block);
};

class ScrollingMessageFrame : public MessageFrame {
    public:
    /* 00AC */ uint32_t m_scrollFlags;
    /* 00B0 */ int32_t m_maxLines;

    bool parse(MemBlock& block);
};

class EditBox : public Frame {
    public:
    /* 009C */ uint32_t m_editFlags;
    /* 00A0 */ float m_blinkSpeed;
    /* 00A4 */ int32_t m_historyLines;
    /* 00A8 */ int32_t m_letters;

    bool parse(MemBlock& block);
};

class BlobFrame : public Frame {
    public:
    /* 009C */ uint32_t m_blobFlags;
    /* 00A0 */ XMLString m_fillTexture;
    /* 00A8 */ XMLString m_borderTexture;

    bool parse(MemBlock& block);
};

class Minimap : public Frame {
    public:
    /* 009C */ uint32_t m_minimapFlags;
    /* 00A0 */ XMLString m_playerTexture;
    /* 00A8 */ XMLString m_questBlobInsideTexture;
    /* 00B0 */ XMLString m_questBlobOutsideTexture;
    /* 00B8 */ XMLString m_questBlobRingTexture;
    /* 00C0 */ XMLString m_archBlobInsideTexture;
    /* 00C8 */ XMLString m_archBlobOutsideTexture;
    /* 00D0 */ XMLString m_archBlobRingTexture;

    bool parse(MemBlock& block);
};

class SimpleHTML : public Frame {
    public:
    /* 009C */ uint32_t m_htmlFlags;
    /* 00A0 */ XMLString m_hyperlinkFormat;
    /* 00A8 */ XMLString m_file;

    bool parse(MemBlock& block);
};

class Backdrop : public linkObject {
    public:
    /* 0038 */ XMLString m_bgFile;
    /* 0040 */ XMLString m_edgeFile;
    /* 0048 */ uint32_t m_alphaMode;
    /* 004C */ uint32_t m_backdropFlags;

    bool parse(MemBlock& block);
};

class Layer : public linkObject {
    public:
    /* 0038 */ uint32_t m_layerFlags;
    /* 003C */ uint32_t m_level;
    /* 0040 */ int8_t m_subLevel;

    bool parse(MemBlock& block);
};

class Attribute : public linkObject {
    public:
    /* 0038 */ XMLString m_value;
    /* 0040 */ XMLString m_attributeType;
    /* 0048 */ uint32_t m_attributeFlags;

    bool parse(MemBlock& block);
};

class Dimension : public linkObject {
    public:
    /* 0038 */ float m_x;
    /* 003C */ float m_y;
    /* 0040 */ uint32_t m_dimensionFlags;

    bool GetDimensions(float& x, float& y, CStatus* status);
    bool Load(float& x, float& y, CStatus* status);
    bool parseRelative(MemBlock& block);
    bool parseAbsolute(MemBlock& block);
};

class Anchor : public Dimension {
    public:
    /* 0044 */ uint32_t m_point;
    /* 0048 */ uint32_t m_relativePoint;
    /* 004C */ XMLString m_relativeTo;
    /* 0054 */ uint32_t m_anchorFlags;

    bool GetAnchor(FRAMEPOINT& point, FRAMEPOINT& relativePoint, float& offsetX, float& offsetY, CStatus* status);
    bool parse(MemBlock& block);
};

class Value : public linkObject {
    public:
    /* 0038 */ float m_value;
    /* 003C */ uint32_t m_valueFlags;

    bool Load(float& value, CStatus* status);
    bool parseRelative(MemBlock& block);
    bool parseAbsolute(MemBlock& block);
};

class Insets : public linkObject {
    public:
    /* 0038 */ float m_left;
    /* 003C */ float m_right;
    /* 0040 */ float m_top;
    /* 0044 */ float m_bottom;
    /* 0048 */ uint32_t m_insetFlags;

    bool Load(float& left, float& right, float& top, float& bottom, CStatus* status);
    bool parseRelative(MemBlock& block);
    bool parseAbsolute(MemBlock& block);
};

class Color : public linkObject {
    public:
    /* 0038 */ uint32_t m_colorFlags;
    /* 003C */ float m_r;
    /* 0040 */ float m_g;
    /* 0044 */ float m_b;
    /* 0048 */ float m_a;

    bool GetColor(CImVector& color, CStatus* status);
    bool parse(MemBlock& block);
};

class Gradient : public linkObject {
    public:
    /* 0038 */ uint32_t m_gradientFlags;
    /* 003C */ uint32_t m_orientation;

    bool Load(ORIENTATION& orientation, CImVector& minColor, CImVector& maxColor, CStatus* status);
    bool parse(MemBlock& block);
};

class TexCoords : public linkObject {
    public:
    /* 0038 */ uint32_t m_texCoordFlags;
    /* 003C */ float m_left;
    /* 0040 */ float m_right;
    /* 0044 */ float m_top;
    /* 0048 */ float m_bottom;
    /* 004C */ float m_radians;
    /* 0050 */ float m_degrees;

    bool parse(MemBlock& block);
};

class Rect : public linkObject {
    public:
    /* 0038 */ uint32_t m_rectFlags;
    /* 003C */ float m_ulx;
    /* 0040 */ float m_uly;
    /* 0044 */ float m_urx;
    /* 0048 */ float m_ury;
    /* 004C */ float m_llx;
    /* 0050 */ float m_lly;
    /* 0054 */ float m_lrx;
    /* 0058 */ float m_lry;

    bool parse(MemBlock& block);
};

class FontStyle : public linkObject {
    public:
    /* 0038 */ uint32_t m_styleFlags;
    /* 003C */ XMLString m_style;

    bool parse(MemBlock& block);
};

class AnimationGroup : public linkObject {
    public:
    /* 0038 */ uint32_t m_groupFlags;
    /* 003C */ float m_initialOffsetX;
    /* 0040 */ float m_initialOffsetY;
    /* 0044 */ XMLString m_looping;
    /* 004C */ XMLString m_parentKey;

    bool parse(MemBlock& block);
};

class Animation : public linkObject {
    public:
    /* 0038 */ uint32_t m_animFlags;
    /* 003C */ uint32_t m_hasSmoothing;
    /* 0040 */ uint32_t m_animFlags2;
    /* 0044 */ int32_t m_order;
    /* 0048 */ float m_startDelay;
    /* 004C */ float m_endDelay;
    /* 0050 */ float m_duration;
    /* 0054 */ float m_maxFramerate;
    /* 0058 */ XMLString m_smoothing;
    /* 0060 */ XMLString m_parentKey;
    /* 0068 */ XMLString m_target;

    bool parse(MemBlock& block);
};

class AlphaAnimation : public Animation {
    public:
    /* 0070 */ uint32_t m_alphaFlags;
    /* 0074 */ float m_change;
};

class VectorAnimation : public Animation {
    public:
    /* 0070 */ uint32_t m_vectorFlags;
    /* 0074 */ float m_x;
    /* 0078 */ float m_y;

    bool parseTranslation(MemBlock& block);
    bool parseRotation(MemBlock& block);
    bool parseScale(MemBlock& block);
};

class PathAnimation : public Animation {
    public:
    /* 0070 */ uint32_t m_pathFlags;
    /* 0074 */ XMLString m_curve;

    bool parse(MemBlock& block);
};

class ControlPoint : public linkObject {
    public:
    /* 0038 */ uint32_t m_pointFlags;
    /* 003C */ XMLString m_parentKey;
    /* 0044 */ float m_offsetX;
    /* 0048 */ float m_offsetY;

    bool parse(MemBlock& block);
};

class Origin : public linkObject {
    public:
    /* 0038 */ uint32_t m_originFlags;
    /* 003C */ uint32_t m_point;
};

linkObject* FrameXML_CompileXML_Region(XMLNode* node);
void FrameXML_FreeCompiled(linkObject* node, bool force);
linkObject* FrameXML_CompileXML_Ui(XMLNode* node, MemBlock& block);

#include "Engine/Source/FrameXML/BinaryXML.inl"

#endif

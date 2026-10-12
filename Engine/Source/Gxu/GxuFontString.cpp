#include <cmath>
#include <cstddef>
#include <cstdint>
#include "Engine/Source/Gxu/GxuFontString.h"
#include "Engine/Source/Base/Coordinate.h"
#include "Engine/Source/Gxu/GxuFontFont.h"
#include "Engine/Source/Gxu/GxuFont.h"
#include "Engine/Source/Gxu/GxuFontUtil.h"
#include "Engine/Source/Gxu/GxuFontMiscClasses.h"
#include "Engine/Source/Storm/H/storm.h"
#include "Engine/Source/Tempest/CRect.h"

// INLINED
void CGxString::RemoveShadow() {
    this->m_flags &= ~1;
}

// OFFSET: 0x0061D930/0x1402AC0B0
void CGxString::TexturePageEvicted(uint32_t texturePage) {
    if (this->m_texturePagesUsed & (1 << texturePage)) {
        this->m_textureEvicted = 1;
    }
}

// OFFSET: 0x0061D950/0x1402AC0D0
void CGxString::WriteGeometry(CGxVertexPCT* buf, int32_t texturePage, int32_t vertIndex, int32_t numVerts) const {
    TEXTLINETEXTURE* textLine = this->m_textLines[texturePage];

    if (textLine) {
        textLine->WriteGeometry(buf, this->m_fontColor, this->m_shadowOffset, this->m_shadowColor, this->m_viewTranslation, this->m_flags & 1, (this->m_flags >> 5) & 1, vertIndex, numVerts);
    }
}

// OFFSET: 0x0061DDB0
EMBEDDEDPARSEINFO::EMBEDDEDPARSEINFO() {
    this->m_state = 0;
    this->m_hyperlink.m_lineHeight = 0.0f;
    this->m_hyperlink.m_indentation = 0.0f;
    this->m_hyperlink.m_linkData = NULL;
    this->m_hyperlink.m_baseLineHeight = 0.0f;
    this->m_hyperlink.m_linkDataLength = 0;
    this->m_hyperlink.m_right = 0.0f;
    this->m_hyperlink.m_linkText = NULL;
    this->m_hyperlink.m_linkTextLength = 0;
    this->m_hyperlink.m_colorCode = NULL;
    this->m_hyperlink.m_colorCodeLength = 0;
}

// OFFSET: 0x0061DE30
void CGxString::SetGradientAlpha(const GRADIENTINFO& info, uint8_t alpha) {
    if (info.m_texturePage >= 8) {
        return;
    }

    TEXTLINETEXTURE* textLine = this->m_textLines[info.m_texturePage];

    if (textLine && info.m_vertIndex < textLine->m_colors.m_count) {
        textLine->m_colors.m_data[info.m_vertIndex].a = alpha;
    }
}

// OFFSET: 0x0061DE70
uint8_t CGxString::GetGradientAlpha(const GRADIENTINFO& info) {
    if (info.m_texturePage >= 8) {
        return 0;
    }

    TEXTLINETEXTURE* textLine = this->m_textLines[info.m_texturePage];

    if (!textLine || info.m_vertIndex >= textLine->m_colors.m_count) {
        return 0;
    }

    return textLine->m_colors.m_data[info.m_vertIndex].a;
}

// OFFSET: 0x0061DEB0/0x1402AC6B0
void CGxString::InitializeViewTranslation() {
    this->m_viewTranslation = this->m_position;

    if (!this->m_numLines || (this->m_flags & 0x80)) {
        return;
    }

    if (this->m_horzJust == GxHJ_Right) {
        this->m_viewTranslation.x = this->m_blockWidth + this->m_viewTranslation.x;
    } else if (this->m_horzJust == GxHJ_Center) {
        this->m_viewTranslation.x = this->m_blockWidth * 0.5 + this->m_viewTranslation.x;
    }

    float heightPixels = g_heightPixels;
    float spacingPixels = this->m_spacing * heightPixels;
    float spacing = static_cast<uint32_t>(spacingPixels + 0.99995) / heightPixels;
    float textHeight = spacing * (this->m_numLines - 1) + this->m_numLines * this->m_currentFontHeight;

    float extraHeight = this->m_extraLineHeight / (CoordinateGetAspectCompensation() * 1024.0);
    textHeight = DDCToNDCHeight(NDCToDDCWidth(extraHeight)) + textHeight;

    if (this->m_vertJust == GxVJ_Top) {
        this->m_viewTranslation.y = this->m_blockHeight + this->m_viewTranslation.y;
    } else if (this->m_vertJust == GxVJ_Middle) {
        this->m_viewTranslation.y = textHeight + (this->m_blockHeight - textHeight) * 0.5 + this->m_viewTranslation.y;
    } else {
        this->m_viewTranslation.y = this->m_viewTranslation.y + textHeight;
    }

    float x = g_widthPixels * this->m_viewTranslation.x;
    this->m_viewTranslation.x = floor(x);

    float y = g_heightPixels * this->m_viewTranslation.y;
    this->m_viewTranslation.y = floor(y);
}

// OFFSET: 0x0061E060/0x1402AC860
int32_t CGxString::CalculateVertsNeeded(int32_t texturePage) {
    if (this->m_flags & 1) {
        return 2 * this->m_textLines[texturePage]->m_verts.m_count;
    }

    return this->m_textLines[texturePage]->m_verts.m_count;
}

// OFFSET: 0x0061E090
void CGxString::SetStringPosition(const C3Vector& position) {
    this->m_position = position;
    this->InitializeViewTranslation();
}

// OFFSET: 0x0061E7A0
void CGxString::AddHyperlinkParseInfo(GXUFONTHYPERLINKINFO info) {
    float heightPixels = g_heightPixels;
    info.m_lineHeight = info.m_lineHeight / heightPixels;
    info.m_baseLineHeight = info.m_baseLineHeight / heightPixels;

    float widthPixels = g_widthPixels;
    info.m_indentation = info.m_indentation / widthPixels;
    info.m_right = info.m_right / widthPixels;

    this->m_hyperlinkInfo.Add(1, &info);
}

// OFFSET: 0x0061E820/0x1402AD290
void CGxString::AddEmbeddedTextureParseInfo(GXUEMBEDDEDTEXTUREINFO info) {
    float heightPixels = g_heightPixels;
    info.m_lineHeight = info.m_lineHeight / heightPixels;
    info.m_baseLineHeight = info.m_baseLineHeight / heightPixels;

    float widthPixels = g_widthPixels;
    info.m_left = info.m_left / widthPixels;
    info.m_right = info.m_right / widthPixels;
    info.m_height = info.m_height / heightPixels;
    info.m_width = info.m_width / widthPixels;
    info.m_offsetX = info.m_offsetX / widthPixels;
    info.m_offsetY = info.m_offsetY / heightPixels;

    this->m_embeddedTextureInfo.Add(1, &info);
}

// OFFSET: 0x0061E8C0/0x1402AD390
void CGxString::ClearInstanceData() {
    this->m_hyperlinkInfo.SetCount(0);
    this->m_embeddedTextureInfo.SetCount(0);
    this->m_gradientInfo.SetCount(0);
    this->m_lastGradientStart = -1;
    this->m_lastGradientLength = -1;
    this->m_numLines = 0;

    for (uint32_t i = 0; i < 8; i++) {
        if (this->m_textLines[i]) {
            TEXTLINETEXTURE::Recycle(this->m_textLines[i]);
            this->m_textLines[i] = NULL;
        }
    }
}

// OFFSET: 0x0061E930/0x1402AD410
void CGxString::Tick() {
    if (this->m_numLines && this->m_idleTicks++ > 30) {
        this->ClearInstanceData();
    }
}

// OFFSET: 0x0061E960/0x1402AD440
void CGxString::SetColor(const CImVector& color) {
    if (*reinterpret_cast<const uint32_t*>(&color) == *reinterpret_cast<const uint32_t*>(&this->m_fontColor)) {
        return;
    }

    this->m_fontColor = color;
    this->m_shadowColor.a = color.a < this->m_shadowColor.a ? color.a : this->m_shadowColor.a;

    if (!(this->m_flags & 8) || (this->m_flags & 0x20) || this->m_gradientInfo.m_count) {
        this->ClearInstanceData();
    }
}

// OFFSET: 0x0061E9A0/0x1402AD480
void CGxString::HandleScreenSizeChange() {
    if (this->m_flags & 4) {
        this->m_requestedFontHeight = GxuFontGetOneToOneHeight(this->m_currentFace);
    }

    double minHeight = 2.0 / g_heightPixels;
    this->m_currentFontHeight = this->m_requestedFontHeight > minHeight ? this->m_requestedFontHeight : minHeight;

    this->ClearInstanceData();
}

// OFFSET: 0x0061EA10/0x1402AD4E0
void CGxString::InitializeTextLine(const char* text, uint32_t numBytes, CImVector& color, const C3Vector& position, uint32_t* texturePagesUsed, EMBEDDEDPARSEINFO& parseInfo) {
    float rotation = this->m_rotation;
    bool rotated = false;
    float rotA = 1.0f;
    float rotB = 0.0f;
    float rotC = 0.0f;
    float rotD = 1.0f;

    if (fabs(rotation - 0.0f) >= 0.001f) {
        rotated = true;
        float c = cos(rotation);
        float s = sin(rotation);
        rotB = s;
        rotA = c;
        rotC = -s;
        rotD = c;
    }

    float fontHeight = this->m_currentFontHeight;
    float stringScale = this->m_stringScale;

    if (fabs(stringScale - 1.0f) >= 0.001f) {
        fontHeight = stringScale * fontHeight;
    }

    if (this->m_flags & 8) {
        for (uint32_t i = 0; i < 8; i++) {
            if (this->m_textLines[i]) {
                this->m_textLines[i]->m_colors.SetCount(0);
            }
        }
    }

    C3Vector pos = position;
    uint32_t lastCode = 0;

    float fontPixelHeight = ScreenToPixelHeight((this->m_flags >> 7) & 1, fontHeight);
    double pixelHeight = fontPixelHeight;
    float scale = pixelHeight / this->m_currentFace->GetPixelSize();
    float cellHeight = fontPixelHeight;

    if (this->m_currentFace->m_fontFlags & 8) {
        cellHeight = cellHeight + 4.0;
    } else if (this->m_currentFace->m_fontFlags & 1) {
        cellHeight = cellHeight + 2.0;
    }

    if (parseInfo.m_state == 1) {
        parseInfo.m_hyperlink.m_right = pos.x;
    }

    static CRect pixelAdjustments(1.0f / 512.0f, 1.0f / 512.0f, -1.0f / 512.0f, -1.0f / 512.0f);

    float step = 0.0f;
    float x = 0.0f;
    char* tokenStart = NULL;

    if (*text) {
        while (numBytes) {
            CImVector codeColor;
            *reinterpret_cast<uint32_t*>(&codeColor) = 0;

            int32_t advance;
            uint32_t wide;
            QUOTEDCODE code = GxuDetermineQuotedCode(text, advance, &codeColor, this->m_flags, wide, 1);

            numBytes -= advance;
            text += advance;

            if (lastCode) {
                if (this->m_flags & 0x10) {
                    step = this->m_currentFace->ComputeStepFixedWidth(lastCode, wide);
                } else {
                    step = this->m_currentFace->ComputeStep(lastCode, wide);
                }
            }

            x = step * scale;

            switch (code) {
                case CODE_COLORON: {
                    if (!(this->m_flags & 8)) {
                        codeColor.a = this->m_fontColor.a;
                        color = codeColor;
                    }

                    parseInfo.m_hyperlink.m_colorCode = text - advance;
                    parseInfo.m_hyperlink.m_colorCodeLength = advance;
                    break;
                }

                case CODE_COLORRESTORE: {
                    color = this->m_fontColor;
                    break;
                }

                case CODE_NEWLINE:
                case CODE_TEXTURESTOP:
                case CODE_NOWRAPSTART:
                case CODE_NOWRAPSTOP: {
                    break;
                }

                case CODE_HYPERLINKSTART: {
                    if (parseInfo.m_state != 0) {
                        break;
                    }

                    parseInfo.m_state = 1;
                    parseInfo.m_hyperlink.m_linkData = text - advance + g_quotedCodeLengths[CODE_HYPERLINKSTART];
                    parseInfo.m_hyperlink.m_linkDataLength = advance - g_quotedCodeLengths[CODE_HYPERLINKSTOP] - g_quotedCodeLengths[CODE_HYPERLINKSTART];
                    parseInfo.m_hyperlink.m_linkText = text - advance;

                    const char* end = SStrStr(text, "|h");

                    if (end) {
                        end += g_quotedCodeLengths[CODE_HYPERLINKSTOP];

                        if (end && !SStrCmp(end, "|r", 2) && parseInfo.m_hyperlink.m_colorCode) {
                            end += 2;
                            parseInfo.m_hyperlink.m_linkText = parseInfo.m_hyperlink.m_colorCode;
                            parseInfo.m_hyperlink.m_colorCode = NULL;
                            parseInfo.m_hyperlink.m_colorCodeLength = 0;
                        }

                        parseInfo.m_hyperlink.m_linkTextLength = end - parseInfo.m_hyperlink.m_linkText;
                    } else {
                        parseInfo.m_hyperlink.m_linkTextLength = 0;
                    }

                    float left = x + pos.x;
                    parseInfo.m_hyperlink.m_right = left;
                    parseInfo.m_hyperlink.m_indentation = left;
                    break;
                }

                case CODE_HYPERLINKSTOP: {
                    if (parseInfo.m_state != 1 && parseInfo.m_state != 2) {
                        break;
                    }

                    float right = parseInfo.m_hyperlink.m_right;
                    float current = x + pos.x;
                    parseInfo.m_hyperlink.m_right = right > current ? right : current;
                    parseInfo.m_state = 0;
                    this->AddHyperlinkParseInfo(parseInfo.m_hyperlink);
                    break;
                }

                case CODE_TEXTURESTART: {
                    if (parseInfo.m_state == 2) {
                        break;
                    }

                    GXUEMBEDDEDTEXTUREINFO& texture = parseInfo.m_texture;

                    if (!ParseEmbeddedTexture(text - advance, &texture, fontPixelHeight, this->m_scale, fontHeight)) {
                        break;
                    }

                    float savedLineHeight = texture.m_lineHeight;
                    float savedLeft = texture.m_left;
                    float savedBaseLineHeight = texture.m_baseLineHeight;
                    float savedRight = texture.m_right;

                    if (lastCode) {
                        CHARCODEDESC* desc = this->m_currentFace->NewCodeDesc(lastCode);

                        if (desc) {
                            step = this->m_currentFace->GetGlyphBearing(desc, (this->m_flags & 0x80) != 0, fontHeight) + desc->m_bitmapData.m_glyphCellWidth;
                        }

                        x = step * scale;

                        if (x < 0.0) {
                            x = 0.0;
                        }

                        if (this->m_currentFace->m_fontFlags & 8) {
                            x = 4.0 * scale + x;
                        } else if (this->m_currentFace->m_fontFlags & 1) {
                            x = scale + scale + x;
                        }
                    }

                    float left = x + pos.x;
                    texture.m_left = left;

                    if (this->m_flags & 1) {
                        texture.m_left = this->m_shadowOffset.x * scale + left;
                    }

                    float right = texture.m_width * scale + texture.m_left;
                    texture.m_right = right;

                    if (this->m_vertJust == GxVJ_Top) {
                        texture.m_baseLineHeight = texture.m_lineHeight - scale * texture.m_height;
                    } else if (this->m_vertJust == GxVJ_Bottom) {
                        texture.m_lineHeight = scale * texture.m_height + texture.m_baseLineHeight;
                    } else if (this->m_vertJust == GxVJ_Middle) {
                        float height = scale * texture.m_height;
                        float top = (height - fontPixelHeight) * 0.5 + texture.m_lineHeight;
                        texture.m_lineHeight = top;
                        texture.m_baseLineHeight = top - height;
                    }

                    float rounded = right + 0.5;
                    pos.x = floor(rounded);
                    lastCode = 0;
                    step = 0.0f;

                    if (parseInfo.m_state == 1) {
                        CRect& hyperlinkRect = *reinterpret_cast<CRect*>(&parseInfo.m_hyperlink);
                        hyperlinkRect = hyperlinkRect | *reinterpret_cast<const CRect*>(&texture);
                        parseInfo.m_state = 2;
                    }

                    this->AddEmbeddedTextureParseInfo(texture);

                    parseInfo.m_texture.m_lineHeight = savedLineHeight;
                    parseInfo.m_texture.m_left = savedLeft;
                    parseInfo.m_texture.m_baseLineHeight = savedBaseLineHeight;
                    parseInfo.m_texture.m_right = savedRight;
                    break;
                }

                case CODE_TOKENSTART: {
                    tokenStart = const_cast<char*>(text);
                    break;
                }

                case CODE_TOKENSTOP: {
                    if (tokenStart) {
                        for (char* c = tokenStart; c != text - advance; c++) {
                            *c = '0';
                        }

                        tokenStart = NULL;
                    }

                    break;
                }

                default: {
                    CHARCODEDESC* desc = this->m_currentFace->NewCodeDesc(wide);

                    if (!desc) {
                        desc = this->m_currentFace->NewCodeDesc('?');

                        if (!desc) {
                            break;
                        }
                    }

                    uint32_t texturePage = desc->m_textureNumber;
                    *texturePagesUsed |= 1 << texturePage;

                    if (!this->m_textLines[texturePage]) {
                        this->m_textLines[texturePage] = TEXTLINETEXTURE::NewTextLineTexture();
                    }

                    TEXTLINETEXTURE* textLine = this->m_textLines[texturePage];

                    if (!(this->m_flags & 8)) {
                        uint32_t colorIndex = textLine->m_colors.Add(4, 0, &color);

                        if (this->m_flags & 0x20) {
                            GRADIENTINFO gradient[4];
                            gradient[0].m_texturePage = texturePage;
                            gradient[0].m_vertIndex = colorIndex;
                            gradient[1].m_texturePage = texturePage;
                            gradient[1].m_vertIndex = colorIndex + 1;
                            gradient[2].m_texturePage = texturePage;
                            gradient[2].m_vertIndex = colorIndex + 2;
                            gradient[3].m_texturePage = texturePage;
                            gradient[3].m_vertIndex = colorIndex + 3;
                            this->m_gradientInfo.Add(4, 8, gradient);
                        }
                    }

                    float width = desc->m_bitmapData.m_glyphCellWidth * scale;
                    CRect coords = desc->m_bitmapData.m_textureCoords;
                    uint32_t flags = this->m_flags;
                    float top;

                    if (flags & 0x80) {
                        top = fontPixelHeight;
                        coords.minY = coords.minY + pixelAdjustments.minY;
                        coords.minX = coords.minX + pixelAdjustments.minX;
                        coords.maxY = coords.maxY + pixelAdjustments.maxY;
                        coords.maxX = coords.maxX + pixelAdjustments.maxX;
                    } else {
                        top = cellHeight;
                        width = floor(width);
                        x = x + 0.5;
                        x = floor(x);
                    }

                    pos.x = x + pos.x;

                    VERT vert;
                    vert.m_vc.x = pos.x;
                    vert.m_vc.y = pos.y;
                    vert.m_vc.z = pos.z;
                    vert.m_tc.x = 0.0f;
                    vert.m_tc.y = 0.0f;
                    vert.m_vc.x = this->m_currentFace->GetGlyphBearing(desc, (flags & 0x80) != 0, fontHeight) + pos.x;

                    if (this->m_currentFace->m_fontFlags & 8) {
                        vert.m_vc.y = vert.m_vc.y - 2.0;
                    } else if (this->m_currentFace->m_fontFlags & 1) {
                        vert.m_vc.y = vert.m_vc.y - 1.0;
                    }

                    vert.m_vc.y = desc->m_bitmapData.m_yOffset * scale + vert.m_vc.y;

                    uint32_t vertIndex = textLine->m_verts.Add(4, 0, &vert);
                    VERT* verts = &textLine->m_verts.m_data[vertIndex];

                    float right = verts[3].m_vc.x + width;
                    verts[3].m_vc.x = right;
                    verts[2].m_vc.x = right;

                    float bottom = verts[3].m_vc.y + top;
                    verts[3].m_vc.y = bottom;
                    verts[1].m_vc.y = bottom;

                    if (rotated) {
                        for (uint32_t i = 0; i < 4; i++) {
                            float dx = verts[i].m_vc.x - 0.0f;
                            float dy = verts[i].m_vc.y - 0.0f;
                            float rx = dx * rotA + dy * rotC;
                            float ry = dx * rotB + dy * rotD;
                            verts[i].m_vc.x = rx + 0.0f;
                            verts[i].m_vc.y = ry + 0.0f;
                        }
                    }

                    verts[2].m_tc.y = coords.maxY;
                    verts[0].m_tc.y = coords.maxY;
                    verts[3].m_tc.y = coords.minY;
                    verts[1].m_tc.y = coords.minY;
                    verts[1].m_tc.x = coords.minX;
                    verts[0].m_tc.x = coords.minX;
                    verts[3].m_tc.x = coords.maxX;
                    verts[2].m_tc.x = coords.maxX;

                    lastCode = wide;
                    break;
                }
            }

            if (!*text) {
                break;
            }
        }
    }

    InternalWipeUntilNameTokenStop(tokenStart);

    parseInfo.m_hyperlink.m_right = x + pos.x;
}

// OFFSET: 0x0061F4F0/0x1402AE350
void CGxString::AddShadow(const C2Vector& offset, const CImVector& color) {
    this->m_shadowColor = color;
    this->m_shadowColor.a = color.a < this->m_fontColor.a ? color.a : this->m_fontColor.a;
    this->m_shadowOffset = offset;
    this->m_flags |= 1;

    if (this->m_flags & 0x20) {
        this->ClearInstanceData();
    }
}

// OFFSET: 0x0061F530/0x1402AE390
int32_t CGxString::CheckGeometry() {
    if (this->m_textureEvicted) {
        if (!this->m_currentFace->CheckStringGlyphs(this->m_text)) {
            this->ClearInstanceData();
        }

        this->m_textureEvicted = 0;
    }

    this->CreateGeometry();
    this->m_idleTicks = 0;

    return this->m_numLines != 0;
}

// OFFSET: 0x0061F580/0x1402AE3F0
int32_t CGxString::Initialize(float fontHeight, const C3Vector& position, float blockWidth, float blockHeight, CGxFont* face, const char* text, EGxFontVJusts vertJust, EGxFontHJusts horzJust, float spacing, uint32_t flags, const CImVector& color, float scale, float stringScale, float rotation) {
    uint32_t textLen = SStrLen(text) + 1;

    if (textLen > this->m_textLen) {
        if (this->m_text) {
            SMemFree(this->m_text, __FILE__, __LINE__, 0);
        }

        this->m_textLen = textLen;
        this->m_text = static_cast<char*>(SMemAlloc(textLen, __FILE__, __LINE__, 0));
    }

    SStrCopy(this->m_text, text, this->m_textLen);

    this->m_blockWidth = blockWidth;
    this->m_blockHeight = blockHeight;
    this->m_spacing = spacing;
    this->m_position = position;
    this->m_horzJust = horzJust;
    this->m_vertJust = vertJust;
    this->m_flags = flags;
    this->m_fontColor = color;
    this->m_scale = scale;
    this->m_stringScale = stringScale;
    this->m_rotation = rotation;
    this->m_currentFace = face;

    face->m_strings.LinkToTail(this);

    if ((flags & 4) && !(this->m_flags & 0x80)) {
        this->m_requestedFontHeight = GxuFontGetOneToOneHeight(face);
    } else {
        this->m_requestedFontHeight = fontHeight;
    }

    double minHeight = 2.0 / g_heightPixels;
    this->m_currentFontHeight = this->m_requestedFontHeight > minHeight ? this->m_requestedFontHeight : minHeight;

    return 1;
}

// OFFSET: 0x0061F6F0/0x1402AE5D0
void CGxString::Recycle() {
    this->m_batchedStringLink.Unlink();
    g_freeStrings.LinkToTail(this);
    this->ClearInstanceData();
}

// OFFSET: 0x0061F750/0x1402AE690
CGxString::CGxString() {
    this->m_requestedFontHeight = 0.02f;
    this->m_currentFontHeight = 0.02f;
    this->m_stringScale = 1.0f;
    this->m_rotation = 0.0f;
    this->m_position.x = 0.0f;
    this->m_position.y = 0.0f;
    this->m_position.z = 0.0f;
    this->m_fontColor.b = 0xFF;
    this->m_fontColor.g = 0xFF;
    this->m_fontColor.r = 0xFF;
    this->m_fontColor.a = 0xFF;
    this->m_shadowColor.a = 0xFF;
    this->m_shadowColor.b = 0;
    this->m_shadowColor.g = 0;
    this->m_shadowColor.r = 0;
    this->m_shadowOffset.x = 0.00125f;
    this->m_shadowOffset.y = -0.00125f;
    this->m_currentFace = NULL;
    this->m_text = NULL;
    this->m_blockWidth = 1.0f;
    this->m_textLen = 0;
    this->m_blockHeight = 1.0f;
    this->m_vertJust = GxVJ_Top;
    this->m_horzJust = GxHJ_Left;
    this->m_spacing = 0.0f;
    this->m_flags = 0;
    this->m_texturePagesUsed = 0;
    this->m_textureEvicted = 0;
    this->m_lastGradientStart = -1;
    this->m_lastGradientLength = -1;
    this->m_viewTranslation.x = 0.0f;
    this->m_viewTranslation.y = 0.0f;
    this->m_viewTranslation.z = 0.0f;
    this->m_scale = 1.0f;
    this->m_numLines = 0;
    this->m_idleTicks = 0;
    this->m_gradientInfo.m_chunk = 128;

    for (uint32_t i = 0; i < 8; i++) {
        this->m_textLines[i] = NULL;
    }
}

// OFFSET: 0x0061F8B0/0x1402AE800
CGxString::~CGxString() {
    for (uint32_t i = 0; i < 8; i++) {
        if (this->m_textLines[i]) {
            TEXTLINETEXTURE::Recycle(this->m_textLines[i]);
        }
    }

    if (this->m_text) {
        SMemFree(this->m_text, __FILE__, __LINE__, 0);
    }

    this->m_hyperlinkInfo.Clear();
    this->m_embeddedTextureInfo.Clear();
}

// OFFSET: 0x0061FAE0/0x1402AEA30
int32_t CGxString::SetGradient(int32_t start, int32_t length) {
    if (start < 0 || length < 0) {
        return 2;
    }

    if (start == this->m_lastGradientStart && length == this->m_lastGradientLength) {
        return 0;
    }

    this->m_lastGradientLength = length;
    this->m_lastGradientStart = start;
    this->m_flags = (this->m_flags & ~8) | 0x20;

    if (!this->m_gradientInfo.m_count) {
        this->ClearInstanceData();
        this->CheckGeometry();
    }

    if (!this->m_numLines) {
        return -1;
    }

    uint8_t alpha = this->m_fontColor.a;
    int32_t first = 4 * start;
    int32_t count = this->m_gradientInfo.m_count;
    uint8_t fade = alpha;

    if (length) {
        fade = alpha / length;
    }

    for (int32_t i = 0; i < count; i++) {
        if (i < first) {
            this->SetGradientAlpha(this->m_gradientInfo.m_data[i], alpha);
        } else if (alpha && i < first + 4 * length) {
            this->SetGradientAlpha(this->m_gradientInfo.m_data[i], alpha);

            if (i & 1) {
                alpha = alpha > fade ? alpha - fade : 0;
            }
        } else {
            if (!this->GetGradientAlpha(this->m_gradientInfo.m_data[i])) {
                break;
            }

            this->SetGradientAlpha(this->m_gradientInfo.m_data[i], alpha);
        }
    }

    if (first < count) {
        return first + 4 * length > count;
    }

    return 2;
}

// OFFSET: 0x0061FC70/0x1402AEC50
CGxString* CGxString::GetNewString(int32_t linkOnTop) {
    CGxString* string = g_freeStrings.Head();

    if (string) {
        g_strings.LinkToTail(string);
        return string;
    }

    return g_strings.NewNode(linkOnTop ? 2 : 0, 0, 0);
}

// OFFSET: 0x0061FCC0/0x1402AED40
void CGxString::CreateGeometry() {
    if (this->m_numLines || !this->m_text || !*this->m_text) {
        return;
    }

    this->m_gradientInfo.SetCount(0);

    int32_t lastGradientStart = this->m_lastGradientStart;
    int32_t lastGradientLength = this->m_lastGradientLength;

    float top = ScreenToPixelHeight((this->m_flags >> 7) & 1, -this->m_currentFontHeight);

    C3Vector position;
    position.x = 0.0f;
    position.y = this->m_flags & 0x80 ? 0.0f : top;
    position.z = 0.0f;

    char* text = this->m_text;
    CImVector color = this->m_fontColor;

    EMBEDDEDPARSEINFO parseInfo;

    float spacing = this->m_spacing;
    uint32_t numBytes = 0;
    float width = 0.0f;

    float spacingPixels = g_heightPixels * spacing;
    spacingPixels = static_cast<uint32_t>(spacingPixels + 0.99995);

    float fontPixelHeight = ScreenToPixelHeight(0, this->m_currentFontHeight);
    float lineHeight;

    if (this->m_flags & 0x80) {
        lineHeight = this->m_currentFontHeight + spacing;
    } else {
        spacing = spacingPixels / g_heightPixels;
        lineHeight = spacingPixels + fontPixelHeight;
    }

    float blockWidth = this->m_blockWidth;
    bool wordBreak = true;
    this->m_extraLineHeight = 0.0f;
    float heightUsed = 0.0f;

    if (*text) {
        while (this->m_blockHeight > heightUsed) {
            parseInfo.m_hyperlink.m_baseLineHeight = position.y;
            parseInfo.m_texture.m_baseLineHeight = position.y;
            float lineTop = lineHeight + position.y;
            parseInfo.m_hyperlink.m_lineHeight = lineTop;
            parseInfo.m_texture.m_lineHeight = lineTop;

            if ((this->m_flags & 0x2000) && this->m_numLines == 1) {
                blockWidth = blockWidth - g_indentNormWidth;
            }

            heightUsed = heightUsed + this->m_currentFontHeight + spacing;

            if (!*text) {
                break;
            }

            int32_t advance;
            uint32_t wide;
            QUOTEDCODE code = GxuDetermineQuotedCode(text, advance, NULL, this->m_flags, wide, 1);

            width = 0.0f;

            if (code == CODE_NEWLINE) {
                text += advance;
            } else {
                const char* nextText = NULL;
                float lineHeightUsed = 0.0f;
                float shadowWidth = this->m_flags & 1 ? this->m_shadowOffset.x : 0.0f;

                CalcWrapPoint(this->m_currentFace, text, this->m_currentFontHeight, blockWidth, &numBytes, &width, &nextText, shadowWidth, this->m_flags, &wordBreak, &lineHeightUsed, this->m_scale);

                if (nextText == text || !nextText || (!numBytes && !*nextText)) {
                    break;
                }

                if (this->m_horzJust == GxHJ_Right) {
                    float indent = (this->m_flags & 0x2000) && this->m_numLines ? g_indentPixelWidth : 0.0f;
                    position.x = ScreenToPixelWidth((this->m_flags >> 7) & 1, -width) - indent;
                } else if (this->m_horzJust == GxHJ_Center) {
                    float halfWidth = -(width * 0.5);
                    position.x = ScreenToPixelWidth((this->m_flags >> 7) & 1, halfWidth);
                } else {
                    position.x = (this->m_flags & 0x2000) && this->m_numLines ? g_indentPixelWidth : 0.0f;
                }

                if (lineHeightUsed > fontPixelHeight) {
                    this->m_extraLineHeight = lineHeightUsed - fontPixelHeight + this->m_extraLineHeight;
                }

                float offset = lineHeightUsed > fontPixelHeight ? lineHeightUsed - fontPixelHeight : 0.0f;

                if (this->m_vertJust == GxVJ_Middle) {
                    float halfOffset = offset * 0.5;
                    offset = floor(halfOffset);
                }

                if (this->m_vertJust != GxVJ_Top) {
                    position.y = position.y - offset;
                    parseInfo.m_hyperlink.m_baseLineHeight = parseInfo.m_hyperlink.m_baseLineHeight - offset;
                    parseInfo.m_hyperlink.m_lineHeight = parseInfo.m_hyperlink.m_lineHeight - offset;
                    parseInfo.m_texture.m_baseLineHeight = parseInfo.m_texture.m_baseLineHeight - offset;
                    parseInfo.m_texture.m_lineHeight = parseInfo.m_texture.m_lineHeight - offset;
                }

                uint32_t texturePagesUsed = 0;

                if (parseInfo.m_state) {
                    parseInfo.m_hyperlink.m_indentation = position.x;
                }

                this->InitializeTextLine(text, numBytes, color, position, &texturePagesUsed, parseInfo);

                if (this->m_vertJust != GxVJ_Bottom) {
                    position.y = position.y - offset;
                    parseInfo.m_hyperlink.m_baseLineHeight = parseInfo.m_hyperlink.m_baseLineHeight - offset;
                    parseInfo.m_hyperlink.m_lineHeight = parseInfo.m_hyperlink.m_lineHeight - offset;
                    parseInfo.m_texture.m_baseLineHeight = parseInfo.m_texture.m_baseLineHeight - offset;
                    parseInfo.m_texture.m_lineHeight = parseInfo.m_texture.m_lineHeight - offset;
                }

                if (parseInfo.m_state == 1) {
                    this->AddHyperlinkParseInfo(parseInfo.m_hyperlink);
                }

                this->m_texturePagesUsed |= texturePagesUsed;
                text = const_cast<char*>(nextText);
            }

            this->m_numLines++;

            if (this->m_flags & 2) {
                break;
            }

            position.y = position.y - lineHeight;

            if (!*text) {
                break;
            }
        }
    }

    this->InitializeViewTranslation();

    if ((this->m_flags & 0x20) && (lastGradientStart != -1 || lastGradientLength != -1)) {
        this->SetGradient(lastGradientStart, lastGradientLength);
    }
}

// OFFSET: 0x006201C0/0x1402AF5B0
uint32_t CGxString::GetHyperLinkInfo(const GXUFONTHYPERLINKINFO*& info) {
    this->CreateGeometry();

    uint32_t count = this->m_hyperlinkInfo.m_count;

    if (!count || !g_widthPixels || !g_heightPixels) {
        return 0;
    }

    info = this->m_hyperlinkInfo.m_data;

    return count;
}

// OFFSET: 0x00620200/0x1402AF610
uint32_t CGxString::GetEmbeddedTextureInfo(const GXUEMBEDDEDTEXTUREINFO*& info) {
    this->CreateGeometry();

    uint32_t count = this->m_embeddedTextureInfo.m_count;

    if (!count || !g_widthPixels || !g_heightPixels) {
        return 0;
    }

    info = this->m_embeddedTextureInfo.m_data;

    return count;
}

// INLINED
GXUFONTHYPERLINKINFO::GXUFONTHYPERLINKINFO() {
    this->m_lineHeight = 0.0f;
    this->m_indentation = 0.0f;
    this->m_baseLineHeight = 0.0f;
    this->m_right = 0.0f;
    this->m_linkData = NULL;
    this->m_linkDataLength = 0;
    this->m_linkText = NULL;
    this->m_linkTextLength = 0;
    this->m_colorCode = NULL;
    this->m_colorCodeLength = 0;
}

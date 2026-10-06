#ifndef ENGINE_SOURCE_IMAGES_TGA_H
#define ENGINE_SOURCE_IMAGES_TGA_H

#include <cstddef>
#include <cstdint>

class SFileImpl;

struct TGA32Pixel {
    /* 0000 */ uint8_t m_b;
    /* 0001 */ uint8_t m_g;
    /* 0002 */ uint8_t m_r;
    /* 0003 */ uint8_t m_a;
};

struct TGAImageDesc {
    /* 0000 */ uint8_t m_bAlphaChannelBits : 4;
    /* 0000 */ uint8_t m_bLeftRightOrder : 1;
    /* 0000 */ uint8_t m_bTopBottomOrder : 1;
    /* 0000 */ uint8_t m_bReserved : 2;
};

#pragma pack(push, 1)

struct TGAHeader {
    /* 0000 */ uint8_t m_bIDLength;
    /* 0001 */ uint8_t m_bColorMapType;
    /* 0002 */ uint8_t m_bImageType;
    /* 0003 */ uint16_t m_wColorMapStartIndex;
    /* 0005 */ uint16_t m_wColorMapEntries;
    /* 0007 */ uint8_t m_bColorMapEntrySize;
    /* 0008 */ uint16_t m_wXOrigin;
    /* 000A */ uint16_t m_wYOrigin;
    /* 000C */ uint16_t m_wWidth;
    /* 000E */ uint16_t m_wHeight;
    /* 0010 */ uint8_t m_bPixelDepth;
    /* 0011 */ TGAImageDesc m_desc;
};

struct TGAFooter {
    /* 0000 */ uint32_t m_dwExtensionOffset;
    /* 0004 */ uint32_t m_dwDeveloperOffset;
    /* 0008 */ char m_szSigniture[18];
};

#pragma pack(pop)

class CImageFileImpl {
    public:
    virtual ~CImageFileImpl();
    virtual uint32_t Width() const = 0;
    virtual uint32_t Height() const = 0;
    virtual uint32_t Size() const = 0;
    virtual int32_t BytesPerPixel() const = 0;
    virtual uint32_t Bytes() const = 0;
    virtual uint8_t AlphaBits() const = 0;
    virtual uint32_t ComputeAlphaDepth() const = 0;
    virtual int32_t HasNonBlackZeroAlphaPixels() const = 0;
    virtual int32_t IsTopDown() const = 0;
    virtual int32_t SetTopDown(int32_t topDown) = 0;
    virtual int32_t AddAlphaChannel(const void* alpha) = 0;
    virtual const uint8_t* Image() const = 0;
    virtual uint8_t* Image() = 0;
    virtual const TGA32Pixel* ImageTGA32Pixel() const = 0;
    virtual TGA32Pixel* ImageTGA32Pixel() = 0;
    virtual int32_t LoadImageData(uint32_t flags) = 0;
    virtual int32_t Open(const char* fileName, int32_t flags) = 0;
    virtual void Close() = 0;
};

class CTgaFile : public CImageFileImpl {
    public:
    /* 0004 */ SFileImpl* m_file;
    /* 0008 */ uint8_t* m_image;
    /* 000C */ TGAHeader m_header;
    /* 0020 */ uint8_t* m_addlHeaderData;
    /* 0024 */ TGAFooter m_footer;
    /* 0040 */ uint32_t m_imageBytes;
    /* 0044 */ uint8_t* m_colorMap;

    virtual ~CTgaFile();
    virtual uint32_t Width() const;
    virtual uint32_t Height() const;
    virtual uint32_t Size() const;
    virtual int32_t BytesPerPixel() const;
    virtual uint32_t Bytes() const;
    virtual uint8_t AlphaBits() const;
    virtual uint32_t ComputeAlphaDepth() const;
    virtual int32_t HasNonBlackZeroAlphaPixels() const;
    virtual int32_t IsTopDown() const;
    virtual int32_t SetTopDown(int32_t topDown);
    virtual int32_t AddAlphaChannel(const void* alpha);
    virtual const uint8_t* Image() const;
    virtual uint8_t* Image();
    virtual const TGA32Pixel* ImageTGA32Pixel() const;
    virtual TGA32Pixel* ImageTGA32Pixel();
    virtual int32_t LoadImageData(uint32_t flags);
    virtual int32_t Open(const char* fileName, int32_t flags);
    virtual void Close();

    CTgaFile();
    void AddAlphaChannel(uint8_t* dest, uint8_t* source, const uint8_t* alpha);
    uint32_t ColorMapBytes();
    uint32_t ColorMapEntryBytes();
    int32_t Compress();
    void ConvertColorMapped(uint32_t flags);
    int32_t CountRun(const uint8_t* source, int32_t count);
    uint32_t PreImageBytes();
    int32_t ReadColorMappedImage(uint32_t flags);
    int32_t ReadRawImage(uint32_t flags);
    int32_t ReadRleImage(uint32_t flags);
    int32_t RemoveAlphaChannels();
    int32_t RleCompressLine(uint8_t** source, uint8_t** dest);
    int32_t RLEDecompressImage(uint8_t* rleData, uint8_t* data);
    int32_t SetImage(const void* bits, uint16_t width, uint16_t height, uint8_t pixelDepth, uint8_t alphaBits, int32_t topBottomOrder, int32_t leftRightOrder);
    int32_t ValidateColorDepth();
    int32_t Write(const char* fileName);
};

#endif

//
// Created by Dado on 11/02/2018.
//

#pragma once

#include <core/math/vector2f.h>
#include <ostream>

enum PixelFormat {
    PIXEL_FORMAT_UNFORCED = -1,
    PIXEL_FORMAT_RGB = 0,
    PIXEL_FORMAT_RGBA,
    PIXEL_FORMAT_LUMINANCE,
    PIXEL_FORMAT_LUMINANCE_ALPHA,
    PIXEL_FORMAT_BGRA,
    PIXEL_FORMAT_RGB565,
    PIXEL_FORMAT_RGBA_QUADRANTS,
    PIXEL_FORMAT_SRGB,
    PIXEL_FORMAT_SRGBA,
    PIXEL_FORMAT_HDR_RGB_16,
    PIXEL_FORMAT_HDR_RGBA_16,
    PIXEL_FORMAT_HDR_RGB_32,
    PIXEL_FORMAT_HDR_RGBA_32,
    PIXEL_FORMAT_DEPTH_16,
    PIXEL_FORMAT_DEPTH_24,
    PIXEL_FORMAT_DEPTH_32,
    PIXEL_FORMAT_RG,
    PIXEL_FORMAT_HDR_RG_16,
    PIXEL_FORMAT_HDR_RG_32,
    PIXEL_FORMAT_R,
    PIXEL_FORMAT_HDR_R16,
    PIXEL_FORMAT_HDR_RG32,
    PIXEL_FORMAT_HDR_R32,
    PIXEL_FORMAT_INVALID
};

static inline PixelFormat pixelFormatResolver( PixelFormat input, bool hasHDR ) {

    if ( hasHDR ) return input;

    switch ( input ) {
        case PIXEL_FORMAT_HDR_RGB_16:
            return PIXEL_FORMAT_RGB;
        case PIXEL_FORMAT_HDR_RGBA_16:
            return PIXEL_FORMAT_RGBA;
        case PIXEL_FORMAT_HDR_RGB_32:
            return PIXEL_FORMAT_RGB;
        case PIXEL_FORMAT_HDR_RGBA_32:
            return PIXEL_FORMAT_RGBA;
        case PIXEL_FORMAT_HDR_RG_16:
            return PIXEL_FORMAT_RG;
        case PIXEL_FORMAT_HDR_RG_32:
            return PIXEL_FORMAT_RG;
        case PIXEL_FORMAT_HDR_R16:
            return PIXEL_FORMAT_R;
        case PIXEL_FORMAT_HDR_RG32:
            return PIXEL_FORMAT_RG;
        case PIXEL_FORMAT_HDR_R32:
            return PIXEL_FORMAT_R;
        default:
            return input;
    };
}

static inline std::string PixelFormatToString( PixelFormat value ) {
        switch( value ) {
            case PIXEL_FORMAT_UNFORCED: return "PIXEL_FORMAT_UNFORCED";
            case PIXEL_FORMAT_RGB: return "PIXEL_FORMAT_RGB";
            case PIXEL_FORMAT_RGBA: return "PIXEL_FORMAT_RGBA";
            case PIXEL_FORMAT_LUMINANCE: return "PIXEL_FORMAT_LUMINANCE";
            case PIXEL_FORMAT_LUMINANCE_ALPHA: return "PIXEL_FORMAT_LUMINANCE_ALPHA";
            case PIXEL_FORMAT_BGRA: return "PIXEL_FORMAT_BGRA";
            case PIXEL_FORMAT_RGB565: return "PIXEL_FORMAT_RGB565";
            case PIXEL_FORMAT_RGBA_QUADRANTS: return "PIXEL_FORMAT_RGBA_QUADRANTS";
            case PIXEL_FORMAT_SRGB: return "PIXEL_FORMAT_SRGB";
            case PIXEL_FORMAT_SRGBA: return "PIXEL_FORMAT_SRGBA";
            case PIXEL_FORMAT_HDR_RGB_16: return "PIXEL_FORMAT_HDR_RGB_16";
            case PIXEL_FORMAT_HDR_RGBA_16: return "PIXEL_FORMAT_HDR_RGBA_16";
            case PIXEL_FORMAT_HDR_RGB_32: return "PIXEL_FORMAT_HDR_RGB_32";
            case PIXEL_FORMAT_HDR_RGBA_32: return "PIXEL_FORMAT_HDR_RGBA_32";
            case PIXEL_FORMAT_DEPTH_16: return "PIXEL_FORMAT_DEPTH_16";
            case PIXEL_FORMAT_DEPTH_24: return "PIXEL_FORMAT_DEPTH_24";
            case PIXEL_FORMAT_DEPTH_32: return "PIXEL_FORMAT_DEPTH_32";
            case PIXEL_FORMAT_RG: return "PIXEL_FORMAT_RG";
            case PIXEL_FORMAT_HDR_RG_16: return "PIXEL_FORMAT_HDR_RG_16";
            case PIXEL_FORMAT_HDR_RG_32: return "PIXEL_FORMAT_HDR_RG_32";
            case PIXEL_FORMAT_R: return "PIXEL_FORMAT_R";
            case PIXEL_FORMAT_HDR_R16: return "PIXEL_FORMAT_HDR_R16";
            case PIXEL_FORMAT_HDR_RG32: return "PIXEL_FORMAT_HDR_RG32";
            case PIXEL_FORMAT_HDR_R32: return "PIXEL_FORMAT_HDR_R32";
            case PIXEL_FORMAT_INVALID: return "PIXEL_FORMAT_INVALID";
        };
        return "PIXEL_FORMAT_INVALID";
}

enum Filter {
    FILTER_LINEAR,
    FILTER_NEAREST,
    FILTER_LINEAR_MIPMAP_LINEAR
};

static inline std::string FilterFormatToString( Filter value ) {
    switch ( value ) {
        case FILTER_LINEAR: return "FILTER_LINEAR";
        case FILTER_NEAREST: return "FILTER_NEAREST";
        case FILTER_LINEAR_MIPMAP_LINEAR: return "FILTER_LINEAR_MIPMAP_LINEAR";
    }
    return "FILTER_INVALID";
}

enum WrapMode {
    WRAP_MODE_CLAMP_TO_EDGE = 0,
    WRAP_MODE_REPEAT = 1,
    WRAP_MODE_CLAMP_TO_BORDER = 2,
};

static inline std::string WrapModeFormatToString( WrapMode value ) {
    switch ( value ) {
        case WRAP_MODE_CLAMP_TO_EDGE: return "WRAP_MODE_CLAMP_TO_EDGE";
        case WRAP_MODE_REPEAT: return "WRAP_MODE_REPEAT";
        case WRAP_MODE_CLAMP_TO_BORDER: return "WRAP_MODE_CLAMP_TO_BORDER";
    }
    return "WRAP_MODE_INVALID";
}

enum TextureTargetMode {
    TEXTURE_2D = 0,
    TEXTURE_3D,
    TEXTURE_CUBE_MAP,
};

static inline std::string TextureTargetModeModeFormatToString( TextureTargetMode value ) {
    switch ( value ) {
        case TEXTURE_2D : return "TEXTURE_2D";
        case TEXTURE_3D : return "TEXTURE_3D";
        case TEXTURE_CUBE_MAP : return "TEXTURE_CUBE_MAP";
    }
    return "TEXTURE_INVALID";
}

enum FrameBufferTextureTarget2d {
    FBT_TEXTURE_2D = 0,
    FBT_TEXTURE_CUBE_MAP_POS_X,
    FBT_TEXTURE_CUBE_MAP_NEG_X,
    FBT_TEXTURE_CUBE_MAP_POS_Y,
    FBT_TEXTURE_CUBE_MAP_NEG_Y,
    FBT_TEXTURE_CUBE_MAP_POS_Z,
    FBT_TEXTURE_CUBE_MAP_NEG_Z,
};

inline FrameBufferTextureTarget2d indexToFBT( const int index ) {
    switch (index) {
        case 0:
            return FBT_TEXTURE_CUBE_MAP_POS_X;
        case 1:
            return FBT_TEXTURE_CUBE_MAP_NEG_X;
        case 2:
            return FBT_TEXTURE_CUBE_MAP_POS_Y;
        case 3:
            return FBT_TEXTURE_CUBE_MAP_NEG_Y;
        case 4:
            return FBT_TEXTURE_CUBE_MAP_POS_Z;
        case 5:
            return FBT_TEXTURE_CUBE_MAP_NEG_Z;
        default:
            return FBT_TEXTURE_CUBE_MAP_POS_X;
    }
}

enum class ImageHeaderType {
    Uncompress,
    Png,
    Jpg,
    Radiance,
    Exr,
    LUT3D
};

static inline std::string imageHeaderTypeToString( ImageHeaderType value ) {
    switch ( value ) {
        case ImageHeaderType::Uncompress: return "UNCOMPRESS";
        case ImageHeaderType::Png: return "PNG";
        case ImageHeaderType::Jpg: return "JPG";
        case ImageHeaderType::Radiance: return "RADIANCE";
        case ImageHeaderType::Exr: return "EXR";
        case ImageHeaderType::LUT3D: return "LUT3D";
        default: return "UNKNOWN";
    }
}

struct ImageParams {
    int width = 0;
    int height = 0;
    int depth = 1;
    int channels = 3;
    int bpp = 8;
    PixelFormat outFormat = PIXEL_FORMAT_RGB;
    TextureTargetMode ttm = TEXTURE_2D;
    WrapMode wrapMode = WRAP_MODE_REPEAT;
    Filter filterMode = FILTER_LINEAR;
    ImageHeaderType headerType = ImageHeaderType::Uncompress;
    size_t compressLength = 0;

    [[nodiscard]] Vector2f getAspectRatioV() const {
        float r = getAspectRatio();
        return r > 1.0f ? V2f{ r, 1.0f} : V2f{ 1.0f, 1.0f/r};
    }

    [[nodiscard]] float getAspectRatio() const {
        return static_cast<float>(width) / static_cast<float>(height);
    }

    [[nodiscard]] int bppStride() const {
        return channels * (bpp/8);
    }

    [[nodiscard]] size_t memorySize() const {
        return width * height * depth * channels * (bpp/8);
    }

    void setFormatFromChannels() {
        if ( bpp == 8 ) {
            if ( channels==1 ) outFormat = PIXEL_FORMAT_LUMINANCE;//PIXEL_FORMAT_R;
            if ( channels==2 ) outFormat = PIXEL_FORMAT_LUMINANCE_ALPHA;//PIXEL_FORMAT_RG;
            if ( channels==3 ) outFormat = PIXEL_FORMAT_RGB;
            if ( channels==4 ) outFormat = PIXEL_FORMAT_RGBA;
        }
        if ( bpp == 16 ) {
            if ( channels==1 ) outFormat = PIXEL_FORMAT_HDR_R16;
            if ( channels==2 ) outFormat = PIXEL_FORMAT_HDR_RG_16;
            if ( channels==3 ) outFormat = PIXEL_FORMAT_HDR_RGB_16;
            if ( channels==4 ) outFormat = PIXEL_FORMAT_HDR_RGBA_16;
        }
        if ( bpp == 32 ) {
            if ( channels==1 ) outFormat = PIXEL_FORMAT_HDR_R32;
            if ( channels==2 ) outFormat = PIXEL_FORMAT_HDR_RG32;
            if ( channels==3 ) outFormat = PIXEL_FORMAT_HDR_RGB_32;
            if ( channels==4 ) outFormat = PIXEL_FORMAT_HDR_RGBA_32;
        }
    }

    ImageParams() = default;
    ImageParams( int width, int height, int channels ) : width( width ), height( height ), channels( channels ) {}
    ImageParams( int width, int height, int channels, int bpp ) : width( width ), height( height ),
                                                                  channels( channels ), bpp( bpp ) {}

    ImageParams& setSize( int s ) {
        width = s;
        height = s;
        return *this;
    }

    ImageParams& setBpp( int s ) {
        bpp = s;
        return *this;
    }

    ImageParams& size( int _width, int _height ) {
        width = _width;
        height = _height;
        return *this;
    }

    ImageParams& size( int _width, int _height, int _depth ) {
        width = _width;
        height = _height;
        depth = _depth;
        return *this;
    }

    ImageParams& setWidth( int _width ) {
        width = _width;
        return *this;
    }

    ImageParams& setHeight( int _height ) {
        height = _height;
        return *this;
    }

    ImageParams& setChannels( int _channels ) {
        channels = _channels;
        return *this;
    }

    ImageParams& format( PixelFormat _outFormat ) {
        outFormat = _outFormat;
        return *this;
    }

    ImageParams& target( TextureTargetMode _ttm ) {
        ttm = _ttm;
        return *this;
    }

    ImageParams& setWrapMode( WrapMode _wrapMode ) {
        wrapMode = _wrapMode;
        return *this;
    }

    ImageParams& setFilterMode( Filter _filterMode ) {
        filterMode = _filterMode;
        return *this;
    }

    friend std::ostream& operator<<( std::ostream& os, const ImageParams& params ) {
        os << "width: " << params.width << " height: " << params.height << " depth: " << params.depth << " channels: "
           << params.channels << " bpp: " << params.bpp << " outFormat: " << params.outFormat << " ttm: " << params.ttm
           << " wrapMode: " << params.wrapMode << " filterMode: ";
        return os;
    }
};


#include "../framebuffer_opengl.h"

#include <core/util_logs.hpp>
#include <graphics/texture.h>
#include "../gl_util.h"
#include "../../graphic_functions.hpp"

JMATH::Rect2f getPlatformWindowRect() {
    return Framebuffer::getCurrentViewport();
//    JMATH::Rect2f r = getScreenRect;
}

void Framebuffer::checkFrameBufferStatus() {
    GLenum fbs = glCheckFramebufferStatus( GL_FRAMEBUFFER );
    if ( fbs == GL_FRAMEBUFFER_COMPLETE ) return;

    std::string errMessage{};

    switch ( fbs ) {
        case GL_FRAMEBUFFER_UNDEFINED:
            errMessage = "Frame Buffer creation error code: GL_FRAMEBUFFER_UNDEFINED";
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
            errMessage = "Frame Buffer creation error code: GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT";
            break;

        case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
            errMessage = "Frame Buffer creation error code: GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT";
            break;

        case GL_FRAMEBUFFER_UNSUPPORTED:
            errMessage = "Frame Buffer creation error code: GL_FRAMEBUFFER_UNSUPPORTED";
            break;

        case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
            errMessage = "Frame Buffer creation error code: GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE";
            break;

        default:
            errMessage = "Frame Buffer creation error code: UNKNOWN (You should be very scared about this";
            break;
    }

    LOGRS( "Framebuffer " << mName << " size: [" << mWidth << ":" << mHeight << "] format : ["
                          << glEnumToString( pixelFormatToGlInternalFormat(mFormat) ) << "] "
                          << "Texture Slot: [" << mTextureGPUSlot << "] "
//                          << "mIsMultisampled: ["  << std::boolalpha << mMultisample << "] "
//                          << "mIsHDR         : ["  << std::boolalpha << mHDR       << "] "
//                          << "mIsCubemap     : ["  << std::boolalpha << mCubeMap   << "] "
//                          << "mUseMipMaps    : ["  << std::boolalpha << mUseMipMaps  << "] "
//                          << "Name           : ["  << mRenderToTexture->names.begin() << "] "
    )

//    ASSERT( fbs == GL_FRAMEBUFFER_COMPLETE );
}

GLenum Framebuffer::framebufferTextureTarget( bool _multisampled ) {
    return GL_TEXTURE_2D;
}

void Framebuffer::framebufferTexture2D( GLuint rth, const std::string& renderTargetIndex, [[maybe_unused]] int mipMapIndex ) {
    if ( mCubeMap ) {
        GLCALL( glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                                        nameToCubeMapSide( renderTargetIndex ),
                                        rth, mipMapIndex ));
    } else {
        GLCALL( glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                                        framebufferTextureTarget( mMultisample ),
                                        rth, mipMapIndex ));
    }
}

void Framebuffer::clearDepthBuffer( const float _clearDepthValue ) {
    GLCALL( glClearDepthf( _clearDepthValue ));
    GLCALL( glClear( GL_DEPTH_BUFFER_BIT ));
}

void Framebuffer::setSRGB( [[maybe_unused]] bool value ) {
//    if ( value ) {
//        glEnable( GL_FRAMEBUFFER_SRGB );
//    } else {
//        glDisable( GL_FRAMEBUFFER_SRGB );
//    }
}

bool Framebuffer::checkHDRSupport() {

    auto HDRenabled = emscripten_webgl_enable_extension( emscripten_webgl_get_current_context(), "EXT_color_buffer_float");

    gbIsHDRSupported = HDRenabled == EM_TRUE ? true : false;

    return gbIsHDRSupported;
}

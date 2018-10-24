#include "../window_handling_opengl.hpp"
#include "../../platform_graphics.hpp"

#include "core/util.h"
#include "../gl_util.h"
#include "imgui.h"
#include "../imgui_impl_glfw.h"
#include "../imgui_impl_opengl3.h"
#include "core/configuration/app_options.h"

namespace WindowHandling {

    void initializeWindow( uint64_t flags, TextInput& ti, MouseInput& mi, Renderer& rr ) {
        LOGR( "--- Initialising Graphics ---" );

        glfwWindowHint( GLFW_SAMPLES, 16 );
        glfwWindowHint( GLFW_SRGB_CAPABLE, GLFW_TRUE );

        if ( !glfwInit() ) {
            LOGE( "Could not start GLFW3" );
        }
        initGraphics();

        const GLFWvidmode *mode = glfwGetVideoMode( glfwGetPrimaryMonitor());
        if ( flags & InitializeWindowFlags::FullScreen ) {
            glfwWindowHint( GLFW_RED_BITS, mode->redBits );
            glfwWindowHint( GLFW_GREEN_BITS, mode->greenBits );
            glfwWindowHint( GLFW_BLUE_BITS, mode->blueBits );
            glfwWindowHint( GLFW_REFRESH_RATE, 90 );// mode->refreshRate
            glfwSwapInterval( 1 );
            window = glfwCreateWindow( mode->width, mode->height, "Sixth view", glfwGetPrimaryMonitor(), NULL );
        } else {
            float scaleFactor = 1.0f;
            if ( checkBitWiseFlag( flags, InitializeWindowFlags::HalfSize )) scaleFactor = 2.0f;
            if ( checkBitWiseFlag( flags, InitializeWindowFlags::ThreeQuarter )) scaleFactor = 1.5f;
            window = glfwCreateWindow( static_cast<int>( mode->width / scaleFactor ),
                                           static_cast<int>( mode->height / scaleFactor ), "Sixth View", NULL, NULL );
        }
        glfwSwapInterval( -1 );

        if ( flags & InitializeWindowFlags::Minimize )
            glfwIconifyWindow( window );

#ifndef OSX
        if ( flags & InitializeWindowFlags::Maximize )
            glfwMaximizeWindow( window );
#endif

        if ( !window ) {
            LOGE( "Could not open window with GLFW3" );
            glfwTerminate();
        }

        glfwMakeContextCurrent( window );
        //	glfwSetInputMode(window, GLFW_STICKY_KEYS, 1);

        initGraphicsExtensions();

        checkGlError( "GlewInit", __LINE__, __FILE__ );
        glEnable( GL_TEXTURE_CUBE_MAP_SEAMLESS );
        setTextureAligment( 1 );
        GLCALL( glClearColor( 0.0f, 0.0f, 0.0f, 1.0f ));
        GLCALL( glClear( GL_COLOR_BUFFER_BIT ));

        // get version info
        const GLubyte *renderer = glGetString( GL_RENDERER ); // get renderer string
        const GLubyte *version = glGetString( GL_VERSION ); // version as a string
        LOGR( "Renderer: %s", renderer );
        LOGR( "OpenGL version supported %s", version );

        // Get info on default framebuffer:
        gatherMainScreenInfo();

        initImGUI();

        ShaderAssetBuilder{"shaders"}.build(rr);
    }

}
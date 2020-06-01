#include "window_handling_opengl_glfw.hpp"
#include <graphics/opengl/gl_headers.hpp>
#ifdef _USE_IMGUI_
#include <graphics/imgui/imgui.h>
#include <graphics/opengl/GLFW/imgui_impl_glfw.h>
#include <graphics/opengl/imgui_impl_opengl3.h>
#endif

#include <graphics/window_handling.hpp>
#include <graphics/render_list.h>

//#define _USE_IMGUI_

namespace WindowHandling {

    GLFWwindow* window = nullptr;
    bool bUseGLFWPoll = true;

    void setDropCallback( GLFWdropfun fn ) {
        glfwSetDropCallback( window, fn );
    }

    void setResizeWindowCallback( GLFWwindowsizefun fn ) {
        glfwSetWindowSizeCallback( window, fn );
    }

    void setResizeFramebufferCallback( GLFWframebuffersizefun fn ) {
        glfwSetFramebufferSizeCallback( window, fn );
    }

    void gatherMainScreenInfo() {
        int w,h;
        glfwGetFramebufferSize( window, &w, &h );
        Vector2i sizei{ w, h };
        AppGlobals::getInstance().setScreenSizef( Vector2f{w,h});
        AppGlobals::getInstance().setScreenSizei( sizei );
    }

    bool shouldWindowBeClosed() {
        return glfwWindowShouldClose( window ) != 0;
    }

    void initImGUI() {
#ifdef _USE_IMGUI_
        // Setup Dear ImGui binding
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
        //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;   // Enable Gamepad Controls

        ImGui::GetIO().FontGlobalScale = 1.5f;
//        ImGui::GetIO().FontAllowUserScaling = true;
        ImGui_ImplGlfw_InitForOpenGL(window, true);
#ifdef __EMSCRIPTEN__
        const char* glsl_version = "#version 300 es";
#else
        const char* glsl_version = "#version 410";
#endif
        ImGui_ImplOpenGL3_Init(glsl_version);

        // Setup style
        ImGui::StyleColorsDark();
#endif
    }

    void preUpdate() {
#ifdef _USE_IMGUI_
        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
#endif
    }

    void enableInputCallbacks() {
        bUseGLFWPoll = true;
#ifdef _USE_IMGUI_
        ImGui_ImplGlfw_InstallCallbacks( window );
#endif
    }

    void disableInputCallbacks() {
        bUseGLFWPoll = false;
#ifdef _USE_IMGUI_
        ImGui_ImplGlfw_DisableCallbacks( window );
#endif
    }

    void flush() {
#ifdef _USE_IMGUI_
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
#endif
        glfwSwapBuffers( window );
#ifdef __APPLE__
        static bool macMoved = false;

        if(!macMoved) {
            int x, y;
            glfwGetWindowPos(window, &x, &y);
            glfwSetWindowPos(window, ++x, y);
            macMoved = true;
        }
#endif
    }

    void pollEvents() {
        //glfwWaitEvents();
        if ( bUseGLFWPoll ) {
            glfwPollEvents();
        }
    }

    void enableVSync( const bool val ) {
        glfwSwapInterval( val != 0 ? 1 : 0 );
    }

    bool isInputEnabled() {
        return bUseGLFWPoll;
    }

    void enableMouseCursor( bool flag ) {
        glfwSetInputMode(window, GLFW_CURSOR, flag ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
    }

    void imRenderLoopStats( const RenderStats& rs ) {
#ifdef _USE_IMGUI_
        ImGui::Begin("Renderer Console");
        ImGui::Text("Application average %.3f", 1000.0f / ImGui::GetIO().Framerate );
        ImGui::Text("Current FrameRate (%.1f FPS)", ImGui::GetIO().Framerate );
        ImGui::Text("Number drawcalls: %d", rs.getDrawCallsPerFrame() );
        ImGui::Text("Number render materials: %d", rs.getInMemoryMaterials() );
        ImGui::Text("Number vertex buffers: %d", rs.getInMemoryVertexBuffers() );
        ImGui::Text("Number textures: %d", rs.getInMemoryTextures() );
        ImGui::End();
#endif
    }

}

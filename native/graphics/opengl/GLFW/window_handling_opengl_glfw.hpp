//
//  window_handling.hpp
//
//

#pragma once

#include <graphics/text_input.hpp>
#include <graphics/mouse_input.hpp>
#include <graphics/opengl/gl_headers.hpp>

struct GLFWwindow;
typedef void (* GLFWdropfun)(GLFWwindow*,int,const char**);

namespace WindowHandling {

	void initImGUI();
	void preUpdate();
	void resizeWindow( const Vector2i& _newSize );
	void gatherMainScreenInfo();
    void setKeyCallback( GLFWkeyfun fn );
	void setDropCallback( GLFWdropfun fn );
	void setResizeWindowCallback( GLFWwindowsizefun fn );
	void setResizeFramebufferCallback( GLFWframebuffersizefun fn );

	extern GLFWwindow* window;
	extern bool bUseGLFWPoll;
};

namespace WH = WindowHandling;

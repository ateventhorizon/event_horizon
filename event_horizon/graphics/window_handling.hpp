//
//  window_handling.hpp
//
//

#pragma once

#include <cstdint>

class TextInput;
class MouseInput;
class Renderer;

namespace WindowHandling {
	void initializeWindow( uint64_t flags, TextInput& ti, MouseInput& mi, Renderer& rr );
	bool shouldWindowBeClosed();
	void flush();
	void pollEvents();
	void enableVSync( const bool val );
	void enableInputCallbacks();
	void disableInputCallbacks();
	bool isInputEnabled();
};

namespace WH = WindowHandling;

#ifdef _OPENGL
#include "opengl/window_handling_opengl.hpp"
#endif
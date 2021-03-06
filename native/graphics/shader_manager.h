#pragma once

#include <vector>
#include <string>
#include <core/serialization.hpp>

JSONDATA(ShaderLiveUpdateMap, shaders)
    std::vector<std::pair<std::string, std::string>> shaders;
    size_t count() const { return shaders.size(); }
};

#ifdef _OPENGL

#include "opengl/shader_manager_opengl.h"

#endif

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

#include <iostream>
#include "engine/graphics.h"

int main() {
    Graphics::RenderBackend renderer = Graphics::RenderBackend{};

    renderer.GraphicsInit(Graphics::DefaultWindowSizeH, Graphics::DefaultWindowSizeW);
    
    while (true) {
        if (renderer.GraphicsStep()) {
            break;
        }
    }

    renderer.Close();
    return 0;
}
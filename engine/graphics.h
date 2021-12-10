#ifndef GRAPHICS_H
#define GRAPHICS_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <string>
#include <memory>
#include <string>
#include <stdexcept>
#include <vector>
#include <iostream>

using namespace std;

namespace Graphics {
    
    const uint32_t DefaultWindowSizeW = 800;
    const uint32_t DefaultWindowSizeH = 800;

    struct RenderBackend {
        static const char* WindowTitle;
        static const char* AppName;
        static const char* EngName;

        static const uint32_t EngVersion;
        static const uint32_t AppVersion;

        GLFWwindow* Window;
        VkInstance Instance;
        VkDebugUtilsMessengerEXT DebugMessenger;

        public:
            // starts up the rendering engine
            void GraphicsInit(uint32_t, uint32_t);
            // steps the rendering engine forward one frame; returns true if window should close
            bool GraphicsStep();
            // safely closes the rendering engine
            void Close();

            // callback for displaying vulkan debug errors
            static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
                VkDebugUtilsMessageSeverityFlagBitsEXT,
                VkDebugUtilsMessageTypeFlagsEXT,
                const VkDebugUtilsMessengerCallbackDataEXT*,
                void*);

            // install our debug messenger
            VkResult InstallDebugMessenger();

            // call vulkan to destroy our debug callback
            void DestroyDebugUtilsMessengerEXT(
                const VkAllocationCallbacks*);

        private: 
            void windowStartup(uint32_t w, uint32_t h);
            void vulkanStartup();
            void createInstance();
            
            // vulkan debug pipeline creation
            bool checkValidationLayerSupport();
            void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT&);
            void installDebugMessenger();
    };

}

#endif+
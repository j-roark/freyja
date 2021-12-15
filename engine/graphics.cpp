#include "graphics.h"

using namespace Graphics;

const char *RenderBackend::WindowTitle = "DemoWindow (800x800)";
const char *RenderBackend::AppName     = "DemoApp";
const char *RenderBackend::EngName     = "DemoEngine";

const uint32_t RenderBackend::EngVersion = VK_MAKE_VERSION(0, 1, 0);
const uint32_t RenderBackend::AppVersion = VK_MAKE_VERSION(0, 1, 0);

const vector<const char*> validationLayers = {
    "VK_LAYER_KHRONOS_validation"
};

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

void RenderBackend::GraphicsInit(uint32_t w, uint32_t h) {
    windowStartup(w, h);
    vulkanStartup();
}

void RenderBackend::windowStartup(uint32_t w, uint32_t h) {
    glfwInit();

    // init glfw without opengl (target vulkan)
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    Window = glfwCreateWindow(w, h, WindowTitle, nullptr, nullptr);
}

bool RenderBackend::GraphicsStep() {
    // TODO: Add frame alpha timing here
    calcDeltaTime();
    
    if (!glfwWindowShouldClose(Window)) {
        glfwPollEvents();
        return false;
    }
    return true;
}

void RenderBackend::vulkanStartup() {
    createInstance();

    switch (InstallDebugMessenger()) {
        case     VK_ERROR_VALIDATION_FAILED_EXT | VK_SUCCESS: break;
        default: throw runtime_error("unable to install default debug messenger!");
    }
}

void RenderBackend::createInstance() {
    VkApplicationInfo                  _appInfo{};
    VkInstanceCreateInfo               _createInfo{};
    VkDebugUtilsMessengerCreateInfoEXT _debugCreateInfo{};

    uint32_t     _glfwExtensionCount = 0;
    const char** _glfwExtensions;

    // specify validations on the instance
    _createInfo.enabledLayerCount = 0;
    _createInfo.pNext = nullptr;
    if (enableValidationLayers) {
        if (!checkValidationLayerSupport()) {
            throw std::runtime_error("validation layers requested, but not available!");
        }

        _createInfo.enabledLayerCount   = static_cast<uint32_t>(validationLayers.size());
        _createInfo.ppEnabledLayerNames = validationLayers.data();

        populateDebugMessengerCreateInfo(_debugCreateInfo);
        _createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&_debugCreateInfo;
    }

    // create application information
    _appInfo.sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    _appInfo.pApplicationName   = RenderBackend::AppName;
    _appInfo.applicationVersion = RenderBackend::AppVersion;
    _appInfo.pEngineName        = RenderBackend::EngName;
    _appInfo.engineVersion      = RenderBackend::EngVersion;
    _appInfo.apiVersion         = VK_API_VERSION_1_0;

    // instance creation information
    _createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    _createInfo.pApplicationInfo = &_appInfo;

    // use glfw to wrangle the correct extensions
    _glfwExtensions = glfwGetRequiredInstanceExtensions(&_glfwExtensionCount);

    _createInfo.enabledExtensionCount   = _glfwExtensionCount;
    _createInfo.ppEnabledExtensionNames = _glfwExtensions;

    _createInfo.enabledLayerCount = 0;

    if (vkCreateInstance(&_createInfo, nullptr, &Instance) != VK_SUCCESS) {
        throw std::runtime_error("failed to create instance!");
    }
}

// make sure that all of the specified validation layers are supported by vulkan
// this runs in a global context and is therefore not a member of Graphics
bool RenderBackend::checkValidationLayerSupport() {
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    for (const char* layerName : validationLayers) {
        bool layerFound = false;

        for (const auto& layerProperties : availableLayers) {
            if (strcmp(layerName, layerProperties.layerName) == 0) {
                layerFound = true;
                break;
            }
        }

        if (!layerFound) {
            return false;
        }
    }

    return true;
}

// This is what will actually display the error message.
VKAPI_ATTR VkBool32 VKAPI_CALL RenderBackend::DebugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData) 
{
    std::cerr << "validation layer debug message: " << pCallbackData->pMessage << std::endl;
    return VK_FALSE;
}

// destroyes the debug util pipeline on our local Instance object
void RenderBackend::DestroyDebugUtilsMessengerEXT(const VkAllocationCallbacks* pAllocator) {
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(Instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr) {
        func(Instance, DebugMessenger, pAllocator);
    }
}

// install into 
VkResult RenderBackend::InstallDebugMessenger() {
    if (!enableValidationLayers) return VK_ERROR_VALIDATION_FAILED_EXT;

    VkDebugUtilsMessengerCreateInfoEXT _createInfo;
    populateDebugMessengerCreateInfo(_createInfo);

    auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(Instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr) {
        return func(Instance, &_createInfo, nullptr, &DebugMessenger);
    }
    else {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

void RenderBackend::populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& _createInfo) {
    _createInfo.sType           = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    _createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT
                                | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT
                                | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    _createInfo.messageType     = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT
                                | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT
                                | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    _createInfo.pfnUserCallback = RenderBackend::DebugCallback;

}

// calcDeltaTime() is called to update the time between frames
void RenderBackend::calcDeltaTime() {
    this->deltaCurrentFrameTime = 
        chrono::duration_cast<chrono::microseconds>
            (chrono::high_resolution_clock::now().time_since_epoch()
        ).count();

    this->DeltaFrameTimeMicros = this->deltaCurrentFrameTime - this->deltaPrevFrameTime;
    this->deltaPrevFrameTime   = this->deltaCurrentFrameTime;
}

float RenderBackend::DeltaTime(DeltaTimePrecision dtp = DeltaTimePrecision::Microseconds) {
     if (this->DeltaTime == 0) {
        return 0.0;
     }
     
    switch (dtp) {
        case DeltaTimePrecision::Microseconds:
           return (float)this->DeltaFrameTimeMicros;

        case DeltaTimePrecision::Milliseconds:
           return (float)this->DeltaFrameTimeMicros / 1000;

        case DeltaTimePrecision::Seconds:
           return (float)this->DeltaFrameTimeMicros / 1000000;
     }
}

void RenderBackend::Close() {
    if (enableValidationLayers) {
        DestroyDebugUtilsMessengerEXT(nullptr);
    }

    vkDestroyInstance(Instance, nullptr);

    glfwDestroyWindow(Window);
    glfwTerminate();
}

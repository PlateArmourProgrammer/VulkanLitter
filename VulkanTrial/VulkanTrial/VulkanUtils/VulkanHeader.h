#ifndef VULKAN_HEADER_H_
#define VULKAN_HEADER_H_

#define VK_USE_PLATFORM_WIN32_KHR
#define SDL_MAIN_HANDLED
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/hash.hpp>
#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>
#include <vulkan/vulkan.hpp>

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

// todo: figure out layers
const std::vector<const char*> _layers = {
	"VK_LAYER_LUNARG_standard_validation"
};

const std::vector<const char*> _deviceExtensions = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

#endif // !VULKAN_HEADER_H_


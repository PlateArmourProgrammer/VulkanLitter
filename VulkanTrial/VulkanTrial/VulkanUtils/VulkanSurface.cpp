#include "VulkanSurface.h"
#include "VulkanInstance.h"

namespace litter {
	VulkanSurface::VulkanSurface(VulkanInstance* instance, SDL_Window* window) {
		_instance = instance;

		SDL_SysWMinfo windowInfo;
		SDL_VERSION(&windowInfo.version);
		if (!SDL_GetWindowWMInfo(window, &windowInfo)) {
			throw std::system_error(std::error_code(), "SDK window manager info is not available.");
		}

		if (windowInfo.subsystem != SDL_SYSWM_WINDOWS) {
			throw std::system_error(std::error_code(), "Unsupported window manager is in use.");
		}
		vk::Win32SurfaceCreateInfoKHR surfaceInfo = vk::Win32SurfaceCreateInfoKHR()
			.setHinstance(GetModuleHandle(NULL))
			.setHwnd(windowInfo.info.win.window);
		_surface = instance->getObject()->createWin32SurfaceKHR(surfaceInfo);
	}

	VulkanSurface::~VulkanSurface() {
		_instance->getObject()->destroySurfaceKHR(_surface, nullptr);
	}

	vk::SurfaceKHR* VulkanSurface::getObject() {
		return &_surface;
	}
}

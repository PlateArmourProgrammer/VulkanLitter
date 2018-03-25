#ifndef VulkanSurface_h_
#define VulkanSurface_h_

#include "Base/BaseObject.h"
#include "VulkanHeader.h"

namespace litter {
	class VulkanInstance;

	class VulkanSurface : public BaseObject {
	public:
		VulkanSurface(VulkanInstance* instance, SDL_Window* window);
		~VulkanSurface();

		vk::SurfaceKHR* getObject();

	private:
		vk::SurfaceKHR _surface;
		VulkanInstance* _instance;
	};
}

#endif // !VulkanSurface_h_

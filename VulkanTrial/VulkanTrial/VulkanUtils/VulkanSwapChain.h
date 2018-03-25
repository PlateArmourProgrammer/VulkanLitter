#ifndef VulkanSwapChain_h_
#define VulkanSwapChain_h_

#include "Base/BaseObject.h"
#include "VulkanHeader.h"

namespace litter {
	class VulkanPhysicalDevice;
	class VulkanLogicalDevice;
	class VulkanSurface;

	class VulkanSwapChain : public BaseObject {
	public:
		VulkanSwapChain(VulkanLogicalDevice* logicalDevice, VulkanPhysicalDevice* physicalDevice, VulkanSurface* surface,
			uint32_t width, uint32_t height);
		~VulkanSwapChain();
		void init(uint32_t width, uint32_t height);
		void cleanup();

		vk::SwapchainKHR* getObject();
		vk::Format* getImageFormat();
		vk::Extent2D* getExtent();
		uint32_t getExtentWidth();
		uint32_t getExtentHeight();
	private:
		vk::SurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats);
		vk::PresentModeKHR chooseSwapPresentMode(const std::vector<vk::PresentModeKHR> availablePresentModes);
		vk::Extent2D chooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities, uint32_t width, uint32_t height);

	private:
		vk::SwapchainKHR _swapChain;
		vk::Format _imageFormat;
		vk::Extent2D _extent;

		VulkanLogicalDevice* _logicalDevice;
		VulkanPhysicalDevice* _physicalDevice;
		VulkanSurface* _surface;
	};
}

#endif // !VulkanSwapChain_h_

#ifndef VulkanDepthResources_h_
#define VulkanDepthResources_h_

#include "Base/BaseObject.h"
#include "VulkanHeader.h"

namespace litter {
	class VulkanPhysicalDevice;
	class VulkanLogicalDevice;
	class VulkanCommandPool;
	class VulkanSwapChain;

	class VulkanDepthResource : public BaseObject {
	public:
		VulkanDepthResource(VulkanLogicalDevice* logicalDevice, VulkanPhysicalDevice* physicalDevice,
			VulkanSwapChain* swapChain, VulkanCommandPool* commandPool);
		~VulkanDepthResource();
		void init(VulkanSwapChain* swapChain, VulkanCommandPool* commandPool);
		void cleanup();

		vk::ImageView* getImageView();
	private:
		void createImage(uint32_t width, uint32_t height, vk::Format format);

	private:
		vk::Image _depthImage;
		vk::DeviceMemory _depthImageMemory;
		vk::ImageView _depthImageView;

		VulkanLogicalDevice* _logicalDevice;
		VulkanPhysicalDevice* _physicalDevice;
	};
}

#endif // !VulkanDepthResources_h_

#ifndef VulkanImageViewPool_h_
#define VulkanImageViewPool_h_

#include "Base/BaseObject.h"
#include "VulkanHeader.h"
#include <vector>

namespace litter {
	class VulkanLogicalDevice;
	class VulkanSwapChain;

	class VulkanImageViewPool : public BaseObject {
	public:
		VulkanImageViewPool(VulkanSwapChain* swapChain, VulkanLogicalDevice* logicalDevice);
		~VulkanImageViewPool();
		void init();
		void cleanup();

		size_t getImageViewCount();
		vk::ImageView* getImageViewAt(size_t idx);
	private:
		vk::ImageView createImageView(vk::Image image, vk::Format format, vk::ImageAspectFlags aspectFlags);

	private:
		std::vector<vk::ImageView> _imageViews;
		std::vector<vk::Image> _images;

		VulkanLogicalDevice* _logicalDevice;
		VulkanSwapChain* _swapChain;
	};
}

#endif // !VulkanImageViewPool_h_

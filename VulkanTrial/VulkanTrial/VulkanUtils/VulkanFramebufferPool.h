#ifndef VulkanFramebufferPool_h_
#define VulkanFramebufferPool_h_

#include "Base/BaseObject.h"
#include "VulkanHeader.h"

namespace litter {
	class VulkanLogicalDevice;
	class VulkanImageViewPool;
	class VulkanRenderPass;
	class VulkanSwapChain;

	class VulkanFramebufferPool : public BaseObject {
	public:
		VulkanFramebufferPool(VulkanLogicalDevice* logicalDevice, VulkanImageViewPool* imageViewPool,
			vk::ImageView* depthImageView, VulkanSwapChain* swapChain, VulkanRenderPass* renderPass);
		~VulkanFramebufferPool();
		void init(VulkanImageViewPool* imageViewPool, vk::ImageView* depthImageView, VulkanSwapChain* swapChain, VulkanRenderPass* renderPass);
		void cleanup();

		size_t getFramebufferCount();
		vk::Framebuffer* getFramebufferAt(size_t idx);

	private:
		std::vector<vk::Framebuffer> _framebuffers;

		VulkanLogicalDevice* _logicalDevice;
	};
}

#endif // !VulkanFramebufferPool_h_

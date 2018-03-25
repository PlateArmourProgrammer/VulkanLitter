#ifndef VulkanCommandBuffers_h_
#define VulkanCommandBuffers_h_

#include "Base/BaseObject.h"
#include "VulkanHeader.h"

namespace litter {
	class VulkanLogicalDevice;
	class VulkanCommandPool;
	class VulkanFramebufferPool;
	class VulkanRenderPass;
	class VulkanPipeline;
	class VulkanSwapChain;
	class TextureRenderCmd;

	class VulkanCommandBuffers : public BaseObject {
	public:
		VulkanCommandBuffers(VulkanLogicalDevice* logicalDevice, VulkanCommandPool* commandPool,
			VulkanFramebufferPool* framebufferPool, VulkanRenderPass* renderPass, VulkanPipeline* pipeline,
			VulkanSwapChain* swapChain, vk::DescriptorSet* descriptorSet, TextureRenderCmd* renderCmd);
		~VulkanCommandBuffers();
		void init(VulkanFramebufferPool* framebufferPool, VulkanRenderPass* renderPass, VulkanPipeline* pipeline,
			VulkanSwapChain* swapChain, vk::DescriptorSet* descriptorSet, TextureRenderCmd* renderCmd);
		void cleanup();

		vk::CommandBuffer* getBufferAt(size_t idx);

	private:
		std::vector<vk::CommandBuffer> _commandBuffers;

		VulkanLogicalDevice* _logicalDevice;
		VulkanCommandPool* _commandPool;
	};
}

#endif // !VulkanCommandBuffers_h_

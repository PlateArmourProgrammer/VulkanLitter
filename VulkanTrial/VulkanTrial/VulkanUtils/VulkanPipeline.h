#ifndef VulkanPipeline_h_
#define VulkanPipeline_h_

#include "Base/BaseObject.h"
#include "VulkanHeader.h"

namespace litter {
	class VulkanLogicalDevice;
	class VulkanRenderPass;
	class VulkanSwapChain;
	class VulkanDescriptorSetLayout;

	class VulkanPipeline : public BaseObject {
	public:
		VulkanPipeline(VulkanLogicalDevice* logicalDevice, VulkanSwapChain* swapChain, VulkanDescriptorSetLayout* descriptorSetLayout, VulkanRenderPass* renderPass);
		~VulkanPipeline();
		void init(VulkanSwapChain* swapChain, VulkanDescriptorSetLayout* descriptorSetLayout, VulkanRenderPass* renderPass);
		void cleanup();

		vk::Pipeline* getObject();
		vk::PipelineLayout* getPiprlineLayout();
	private:
		vk::ShaderModule createShaderModule(const std::vector<char>& code);

	private:
		vk::Pipeline _pipeline;
		vk::PipelineLayout _pipelineLayout;

		VulkanLogicalDevice* _logicalDevice;
	};
}

#endif // !VulkanPipeline_h_

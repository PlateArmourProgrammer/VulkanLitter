#ifndef VulkanRenderPass_h_
#define VulkanRenderPass_h_

#include "Base/BaseObject.h"
#include "VulkanHeader.h"

namespace litter {
	class VulkanLogicalDevice;
	class VulkanPhysicalDevice;

	class VulkanRenderPass : public BaseObject {
	public:
		VulkanRenderPass(VulkanLogicalDevice* logicalDevice, vk::Format* imageFormat, VulkanPhysicalDevice* physicalDevice);
		~VulkanRenderPass();
		void init(vk::Format* imageFormat, VulkanPhysicalDevice* physicalDevice);
		void cleanup();

		vk::RenderPass* getObject();

	private:
		vk::RenderPass _renderPass;

		VulkanLogicalDevice* _logicalDevice;
	};
}

#endif // !VulkanRenderPass_h_

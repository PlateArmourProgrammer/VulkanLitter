#ifndef VulkanCommandPool_h_
#define VulkanCommandPool_h_

#include "Base/BaseObject.h"
#include "VulkanHeader.h"
#include <vector>

namespace litter {
	class VulkanPhysicalDevice;
	class VulkanLogicalDevice;
	class VulkanFramebufferPool;

	class VulkanCommandPool : public BaseObject {
	public:
		VulkanCommandPool(VulkanLogicalDevice* logicalDevice, VulkanPhysicalDevice* physicalDevice);
		~VulkanCommandPool();

		vk::CommandPool* getObject();

	private:
		vk::CommandPool _commandPool;

		VulkanLogicalDevice* _logicalDevice;
	};
}

#endif // !VulkanCommandPool_h_

#ifndef VulkanSingleTimeCommand_h_
#define VulkanSingleTimeCommand_h_

#include "Base/BaseObject.h"
#include "VulkanHeader.h"

namespace litter {
	class VulkanLogicalDevice;
	class VulkanCommandPool;

	class VulkanSingleTimeCommand : public BaseObject {
	public:
		VulkanSingleTimeCommand(VulkanLogicalDevice* logicalDevice, VulkanCommandPool* commandPool);
		~VulkanSingleTimeCommand();

		vk::CommandBuffer* getObject();

	private:
		vk::CommandBuffer _commandBuffer;

		VulkanLogicalDevice* _logicalDevice;
		VulkanCommandPool* _commandPool;
	};
}

#endif // !VulkanSingleTimeCommand_h_


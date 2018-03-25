#ifndef VulkanLogicalDevice_h_
#define VulkanLogicalDevice_h_

#include "Base/BaseObject.h"
#include "VulkanHeader.h"

namespace litter {
	class VulkanPhysicalDevice;

	class VulkanLogicalDevice : public BaseObject {
	public:
		VulkanLogicalDevice(VulkanPhysicalDevice* physicalDevice);
		~VulkanLogicalDevice();

		vk::Device* getObject();
		vk::Queue* getGraphicsQueue();
		vk::Queue* getPresentQueue();

	private:
		vk::Device _device;
		vk::Queue _graphicsQueue;
		vk::Queue _presentQueue;
	};
}

#endif // !VulkanLogicalDevice_h_

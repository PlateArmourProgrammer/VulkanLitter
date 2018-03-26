#ifndef VulkanCamera_h_
#define VulkanCamera_h_

#include "Base/BaseObject.h"
#include "VulkanHeader.h"

namespace litter {
	class VulkanPhysicalDevice;
	class VulkanLogicalDevice;

	class VulkanCamera : public BaseObject {
	public:
		VulkanCamera(VulkanLogicalDevice* logicalDevice, VulkanPhysicalDevice* physicalDevice);
		~VulkanCamera();

		void setSize(uint32_t width, uint32_t height);
		void update(float rotate);
		vk::DescriptorBufferInfo* getBufferInfo();

	private:
		vk::Buffer _uniformBuffer;
		vk::DeviceMemory _uniformBufferMemory;
		vk::DescriptorBufferInfo _bufferInfo;
		uint32_t _width;
		uint32_t _height;

		VulkanLogicalDevice* _logicalDevice;
	};
}

#endif // !VulkanCamera_h_

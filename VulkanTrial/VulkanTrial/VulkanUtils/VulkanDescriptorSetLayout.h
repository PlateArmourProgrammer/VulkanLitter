#ifndef VulkanDescriptorSetLayout_h_
#define VulkanDescriptorSetLayout_h_

#include "Base/BaseObject.h"
#include "VulkanHeader.h"

namespace litter {
	class VulkanLogicalDevice;

	class VulkanDescriptorSetLayout : public BaseObject {
	public:
		VulkanDescriptorSetLayout(VulkanLogicalDevice* logicalDevice);
		~VulkanDescriptorSetLayout();

		vk::DescriptorSetLayout* getObject();

	private:
		vk::DescriptorSetLayout _layout;

		VulkanLogicalDevice* _logicalDevice;
	};
}

#endif // !VulkanDescriptorSetLayout_h_

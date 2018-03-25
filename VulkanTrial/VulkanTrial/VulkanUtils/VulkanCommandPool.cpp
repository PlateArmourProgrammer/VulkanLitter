#include "VulkanCommandPool.h"
#include "VulkanPhysicalDevice.h"
#include "VulkanLogicalDevice.h"
#include "VulkanFramebufferPool.h"
#include "VulkanStructs.h"

namespace litter {
	VulkanCommandPool::VulkanCommandPool(VulkanLogicalDevice* logicalDevice, VulkanPhysicalDevice* physicalDevice) {
		_logicalDevice = logicalDevice;
		
		QueueFamilyIndices* queueFamilyIndices = physicalDevice->getQueueFamilyIndices();

		vk::CommandPoolCreateInfo poolInfo = vk::CommandPoolCreateInfo();
		poolInfo.queueFamilyIndex = queueFamilyIndices->graphicsFamily;

		if (_logicalDevice->getObject()->createCommandPool(&poolInfo, nullptr, &_commandPool) != vk::Result::eSuccess) {
			throw std::runtime_error("failed to create command pool!");
		}
	}

	VulkanCommandPool::~VulkanCommandPool() {
		_logicalDevice->getObject()->destroyCommandPool(_commandPool, nullptr);
	}

	vk::CommandPool* VulkanCommandPool::getObject() {
		return &_commandPool;
	}
}

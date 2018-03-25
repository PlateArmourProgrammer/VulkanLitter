#include "VulkanSingleTimeCommand.h"
#include "VulkanLogicalDevice.h"
#include "VulkanCommandPool.h"

namespace litter {
	VulkanSingleTimeCommand::VulkanSingleTimeCommand(VulkanLogicalDevice* logicalDevice, VulkanCommandPool* commandPool) {
		_logicalDevice = logicalDevice;
		_commandPool = commandPool;

		vk::CommandBufferAllocateInfo allocInfo = vk::CommandBufferAllocateInfo()
			.setLevel(vk::CommandBufferLevel::ePrimary)
			.setCommandPool(*commandPool->getObject())
			.setCommandBufferCount(1);

		logicalDevice->getObject()->allocateCommandBuffers(&allocInfo, &_commandBuffer);

		vk::CommandBufferBeginInfo beginInfo = vk::CommandBufferBeginInfo()
			.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);

		_commandBuffer.begin(&beginInfo);
	}

	VulkanSingleTimeCommand::~VulkanSingleTimeCommand() {
		_commandBuffer.end();

		vk::SubmitInfo submitInfo = vk::SubmitInfo()
			.setCommandBufferCount(1)
			.setPCommandBuffers(&_commandBuffer);

		_logicalDevice->getGraphicsQueue()->submit(1, &submitInfo, VK_NULL_HANDLE);
		_logicalDevice->getGraphicsQueue()->waitIdle();

		_logicalDevice->getObject()->freeCommandBuffers(*_commandPool->getObject(), 1, &_commandBuffer);
	}

	vk::CommandBuffer* VulkanSingleTimeCommand::getObject() {
		return &_commandBuffer;
	}
}

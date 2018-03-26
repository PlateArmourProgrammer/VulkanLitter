#include "VulkanCamera.h"
#include "VulkanPhysicalDevice.h"
#include "VulkanLogicalDevice.h"
#include <glm\glm.hpp>

namespace litter {
	struct UniformBufferObject {
		glm::mat4 model;
		glm::mat4 view;
		glm::mat4 proj;
	};

	VulkanCamera::VulkanCamera(VulkanLogicalDevice* logicalDevice, VulkanPhysicalDevice* physicalDevice) {
		_logicalDevice = logicalDevice;

		_x = 0.0f;

		vk::DeviceSize bufferSize = sizeof(UniformBufferObject);

		vk::BufferCreateInfo bufferInfo = vk::BufferCreateInfo()
			.setSize(bufferSize)
			.setUsage(vk::BufferUsageFlagBits::eUniformBuffer)
			.setSharingMode(vk::SharingMode::eExclusive);

		if (_logicalDevice->getObject()->createBuffer(&bufferInfo, nullptr, &_uniformBuffer) != vk::Result::eSuccess) {
			throw std::runtime_error("failed to create vertex buffer!");
		}

		vk::MemoryRequirements memRequirements;
		_logicalDevice->getObject()->getBufferMemoryRequirements(_uniformBuffer, &memRequirements);

		vk::MemoryPropertyFlags properties = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent;
		vk::PhysicalDeviceMemoryProperties memProperties;
		int memoryTypeIndex = -1;
		physicalDevice->getObject()->getMemoryProperties(&memProperties);
		for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
			if ((memRequirements.memoryTypeBits & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
				memoryTypeIndex = i;
				break;
			}
		}

		if (memoryTypeIndex == -1) {
			throw std::runtime_error("failed to find suitable memory type!");
		}

		vk::MemoryAllocateInfo allocInfo = vk::MemoryAllocateInfo()
			.setAllocationSize(memRequirements.size)
			.setMemoryTypeIndex(memoryTypeIndex);

		if (_logicalDevice->getObject()->allocateMemory(&allocInfo, nullptr, &_uniformBufferMemory) != vk::Result::eSuccess)
		{
			throw std::runtime_error("failed to allocate vertex buffer memory!");
		}

		_logicalDevice->getObject()->bindBufferMemory(_uniformBuffer, _uniformBufferMemory, 0);

		_bufferInfo = vk::DescriptorBufferInfo()
			.setBuffer(_uniformBuffer)
			.setOffset(0)
			.setRange(sizeof(UniformBufferObject));
	}

	VulkanCamera::~VulkanCamera() {
		vk::Device* vkDevice = _logicalDevice->getObject();

		vkDevice->destroyBuffer(_uniformBuffer, nullptr);
		vkDevice->freeMemory(_uniformBufferMemory, nullptr);
	}

	void VulkanCamera::setSize(uint32_t width, uint32_t height) {
		_width = width;
		_height = height;
	}

	void VulkanCamera::update(float offset) {
		_x += offset;

		UniformBufferObject ubo = {};
		ubo.model = glm::rotate(glm::mat4(), 0.0f, glm::vec3(0.0f, 0.0f, 1.0f));
		ubo.view = glm::lookAt(glm::vec3(_x, 0.0f, 2.0f), glm::vec3(_x, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		ubo.proj = glm::perspective(glm::radians(45.0f), _width / (float)_height, 0.1f, 10.0f);
		ubo.proj[1][1] *= -1;

		void* data;
		_logicalDevice->getObject()->mapMemory(_uniformBufferMemory, 0, sizeof(ubo), vk::MemoryMapFlagBits(), &data);
		memcpy(data, &ubo, sizeof(ubo));
		_logicalDevice->getObject()->unmapMemory(_uniformBufferMemory);
	}

	vk::DescriptorBufferInfo* VulkanCamera::getBufferInfo() {
		return &_bufferInfo;
	}
}

#include "TextureRenderCmd.h"
#include "VulkanUtils/VulkanPhysicalDevice.h"
#include "VulkanUtils/VulkanLogicalDevice.h"
#include "VulkanUtils/VulkanSingleTimeCommand.h"

namespace litter {
	TextureRenderCmd::TextureRenderCmd(VulkanPhysicalDevice* physicalDevice, VulkanLogicalDevice* logicalDevice, VulkanCommandPool* commandPool) {
		_physicalDevice = physicalDevice;
		_logicalDevice = logicalDevice;
		_commandPool = commandPool;

		createVertexBuffer();
		createIndexBuffer();
	}

	TextureRenderCmd::~TextureRenderCmd() {
		vk::Device* vkDevice = _logicalDevice->getObject();

		vkDevice->destroyBuffer(_vertexBuffer, nullptr);
		vkDevice->freeMemory(_vertexBufferMemory, nullptr);

		vkDevice->destroyBuffer(_indexBuffer, nullptr);
		vkDevice->freeMemory(_indexBufferMemory, nullptr);
	}

	vk::Buffer* TextureRenderCmd::getVertexBuffer() {
		return &_vertexBuffer;
	}

	vk::Buffer* TextureRenderCmd::getIndexBuffer() {
		return &_indexBuffer;
	}

	size_t TextureRenderCmd::getIndexSize() {
		return _indexSize;
	}

	void TextureRenderCmd::createVertexBuffer() {
		int length = 5 * 4;
		float* vertices = new float[length];

		vertices[0] = -0.5f;
		vertices[1] = -0.5f;
		vertices[2] = 0;
		vertices[3] = 1;
		vertices[4] = 0;

		vertices[5] = 0.5f;
		vertices[6] = -0.5f;
		vertices[7] = 0;
		vertices[8] = 0;
		vertices[9] = 0;

		vertices[10] = 0.5f;
		vertices[11] = 0.5f;
		vertices[12] = 0;
		vertices[13] = 0;
		vertices[14] = 1;

		vertices[15] = -0.5f;
		vertices[16] = 0.5f;
		vertices[17] = 0;
		vertices[18] = 1;
		vertices[19] = 1;

		vk::DeviceSize bufferSize = sizeof(float) * length;

		vk::Buffer stagingBuffer;
		vk::DeviceMemory stagingBufferMemory;
		createBuffer(bufferSize, vk::BufferUsageFlagBits::eTransferSrc,
			vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
			stagingBuffer, stagingBufferMemory);

		void* data;
		_logicalDevice->getObject()->mapMemory(stagingBufferMemory, 0, bufferSize, vk::MemoryMapFlagBits(), &data);
		memcpy(data, vertices, (size_t)bufferSize);
		_logicalDevice->getObject()->unmapMemory(stagingBufferMemory);

		createBuffer(bufferSize, vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer,
			vk::MemoryPropertyFlagBits::eDeviceLocal, _vertexBuffer, _vertexBufferMemory);

		copyBuffer(stagingBuffer, _vertexBuffer, bufferSize);

		_logicalDevice->getObject()->destroyBuffer(stagingBuffer, nullptr);
		_logicalDevice->getObject()->freeMemory(stagingBufferMemory, nullptr);

		delete vertices;
	}

	void TextureRenderCmd::createIndexBuffer() {
		_indexSize = 6;
		uint32_t* indices = new uint32_t[_indexSize];

		indices[0] = 0;
		indices[1] = 1;
		indices[2] = 2;
		indices[3] = 0;
		indices[4] = 2;
		indices[5] = 3;

		vk::DeviceSize bufferSize = sizeof(uint32_t) * _indexSize;

		vk::Buffer stagingBuffer;
		vk::DeviceMemory stagingBufferMemory;
		createBuffer(bufferSize, vk::BufferUsageFlagBits::eTransferSrc,
			vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
			stagingBuffer, stagingBufferMemory);

		void* data;
		_logicalDevice->getObject()->mapMemory(stagingBufferMemory, 0, bufferSize, vk::MemoryMapFlagBits(), &data);
		memcpy(data, indices, (size_t)bufferSize);
		_logicalDevice->getObject()->unmapMemory(stagingBufferMemory);

		createBuffer(bufferSize, vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eIndexBuffer,
			vk::MemoryPropertyFlagBits::eDeviceLocal, _indexBuffer, _indexBufferMemory);

		copyBuffer(stagingBuffer, _indexBuffer, bufferSize);

		_logicalDevice->getObject()->destroyBuffer(stagingBuffer, nullptr);
		_logicalDevice->getObject()->freeMemory(stagingBufferMemory, nullptr);

		delete indices;
	}

	void TextureRenderCmd::createBuffer(vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties, vk::Buffer& buffer, vk::DeviceMemory& bufferMemory) {
		vk::BufferCreateInfo bufferInfo = vk::BufferCreateInfo()
			.setSize(size)
			.setUsage(usage)
			.setSharingMode(vk::SharingMode::eExclusive);

		if (_logicalDevice->getObject()->createBuffer(&bufferInfo, nullptr, &buffer) != vk::Result::eSuccess) {
			throw std::runtime_error("failed to create vertex buffer!");
		}

		vk::MemoryRequirements memRequirements;
		_logicalDevice->getObject()->getBufferMemoryRequirements(buffer, &memRequirements);

		vk::MemoryAllocateInfo allocInfo = vk::MemoryAllocateInfo()
			.setAllocationSize(memRequirements.size)
			.setMemoryTypeIndex(findMemoryType(memRequirements.memoryTypeBits, properties));

		if (_logicalDevice->getObject()->allocateMemory(&allocInfo, nullptr, &bufferMemory) != vk::Result::eSuccess) {
			throw std::runtime_error("failed to allocate vertex buffer memory!");
		}

		_logicalDevice->getObject()->bindBufferMemory(buffer, bufferMemory, 0);
	}

	void TextureRenderCmd::copyBuffer(vk::Buffer srcBuffer, vk::Buffer dstBuffer, vk::DeviceSize size) {
		litter::VulkanSingleTimeCommand singleCmd(_logicalDevice, _commandPool);

		vk::BufferCopy copyRegion = vk::BufferCopy()
			.setSrcOffset(0)
			.setDstOffset(0)
			.setSize(size);
		copyRegion.size = size;
		singleCmd.getObject()->copyBuffer(srcBuffer, dstBuffer, 1, &copyRegion);
	}

	uint32_t TextureRenderCmd::findMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties) {
		vk::PhysicalDeviceMemoryProperties memProperties;
		_physicalDevice->getObject()->getMemoryProperties(&memProperties);

		for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
			if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
				return i;
			}
		}

		throw std::runtime_error("failed to find suitable memory type!");
	}
}

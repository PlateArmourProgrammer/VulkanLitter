#ifndef TextureRenderCmd_h_
#define TextureRenderCmd_h_

#include "Base/BaseObject.h"
#include "VulkanUtils/VulkanHeader.h"

namespace litter {
	class VulkanPhysicalDevice;
	class VulkanLogicalDevice;
	class VulkanCommandPool;

	class TextureRenderCmd {
	public:
		TextureRenderCmd(VulkanPhysicalDevice* physicalDevice, VulkanLogicalDevice* logicalDevice, VulkanCommandPool* commandPool);
		~TextureRenderCmd();

		vk::Buffer* getVertexBuffer();
		vk::Buffer* getIndexBuffer();
		size_t getIndexSize();
	private:
		void createVertexBuffer();
		void createIndexBuffer();
		void createBuffer(vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties, vk::Buffer& buffer, vk::DeviceMemory& bufferMemory);
		void copyBuffer(vk::Buffer srcBuffer, vk::Buffer dstBuffer, vk::DeviceSize size);
		uint32_t findMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties);

	private:
		size_t _indexSize;
		vk::Buffer _vertexBuffer;
		vk::Buffer _indexBuffer;
		vk::DeviceMemory _vertexBufferMemory;
		vk::DeviceMemory _indexBufferMemory;

		VulkanPhysicalDevice* _physicalDevice;
		VulkanLogicalDevice* _logicalDevice;
		VulkanCommandPool* _commandPool;
	};
}

#endif // !TextureRenderCmd_h_

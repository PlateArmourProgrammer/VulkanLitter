#include "VulkanDepthResource.h"
#include "VulkanLogicalDevice.h"
#include "VulkanPhysicalDevice.h"
#include "VulkanSingleTimeCommand.h"
#include "VulkanCommandPool.h"
#include "VulkanSwapChain.h"

namespace litter {
	VulkanDepthResource::VulkanDepthResource(VulkanLogicalDevice* logicalDevice, VulkanPhysicalDevice* physicalDevice,
		VulkanSwapChain* swapChain, VulkanCommandPool* commandPool) {
		_logicalDevice = logicalDevice;
		_physicalDevice = physicalDevice;

		init(swapChain, commandPool);
	}

	VulkanDepthResource::~VulkanDepthResource() {
		cleanup();
	}

	void VulkanDepthResource::init(VulkanSwapChain* swapChain, VulkanCommandPool* commandPool) {
		vk::Format depthFormat = *_physicalDevice->getDepthFormat();

		createImage(swapChain->getExtentWidth(), swapChain->getExtentHeight(), depthFormat);

		vk::ImageViewCreateInfo viewInfo = vk::ImageViewCreateInfo()
			.setImage(_depthImage)
			.setViewType(vk::ImageViewType::e2D)
			.setFormat(depthFormat)
			.setSubresourceRange(
				vk::ImageSubresourceRange()
				.setAspectMask(vk::ImageAspectFlagBits::eDepth)
				.setBaseMipLevel(0)
				.setLevelCount(1)
				.setBaseArrayLayer(0)
				.setLayerCount(1)
			);

		if (_logicalDevice->getObject()->createImageView(&viewInfo, nullptr, &_depthImageView) != vk::Result::eSuccess) {
			throw std::runtime_error("failed to create texture image view!");
		}

		litter::VulkanSingleTimeCommand singleCmd(_logicalDevice, commandPool);

		vk::ImageMemoryBarrier barrier = vk::ImageMemoryBarrier()
			.setOldLayout(vk::ImageLayout::eUndefined)
			.setNewLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal)
			.setSrcQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
			.setDstQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
			.setImage(_depthImage)
			.setSubresourceRange(
				vk::ImageSubresourceRange()
				.setAspectMask(vk::ImageAspectFlagBits::eColor)
				.setBaseMipLevel(0)
				.setLevelCount(1)
				.setBaseArrayLayer(0)
				.setLayerCount(1)
			);

		barrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eDepth;
		if (depthFormat == vk::Format::eD32SfloatS8Uint || depthFormat == vk::Format::eD24UnormS8Uint) {
			barrier.subresourceRange.aspectMask |= vk::ImageAspectFlagBits::eStencil;
		}
		barrier.srcAccessMask = vk::AccessFlags();
		barrier.dstAccessMask = vk::AccessFlagBits::eDepthStencilAttachmentRead | vk::AccessFlagBits::eDepthStencilAttachmentWrite;

		singleCmd.getObject()->pipelineBarrier(vk::PipelineStageFlagBits::eTopOfPipe, vk::PipelineStageFlagBits::eTopOfPipe, vk::DependencyFlags(),
			0, nullptr,
			0, nullptr,
			1, &barrier);
	}

	void VulkanDepthResource::cleanup() {
		vk::Device* vkDevice = _logicalDevice->getObject();
		vkDevice->destroyImageView(_depthImageView, nullptr);
		vkDevice->destroyImage(_depthImage, nullptr);
		vkDevice->freeMemory(_depthImageMemory, nullptr);
	}

	vk::ImageView* VulkanDepthResource::getImageView() {
		return &_depthImageView;
	}

	void VulkanDepthResource::createImage(uint32_t width, uint32_t height, vk::Format format)
	{
		vk::ImageCreateInfo imageInfo = vk::ImageCreateInfo()
			.setImageType(vk::ImageType::e2D)
			.setExtent(
				vk::Extent3D()
				.setWidth(static_cast<uint32_t>(width))
				.setHeight(static_cast<uint32_t>(height))
				.setDepth(1)
			)
			.setMipLevels(1)
			.setArrayLayers(1)
			.setFormat(format)
			.setTiling(vk::ImageTiling::eOptimal)
			.setInitialLayout(vk::ImageLayout::ePreinitialized)
			.setUsage(vk::ImageUsageFlagBits::eDepthStencilAttachment)
			.setSharingMode(vk::SharingMode::eExclusive)
			.setSamples(vk::SampleCountFlagBits::e1);

		if (_logicalDevice->getObject()->createImage(&imageInfo, nullptr, &_depthImage) != vk::Result::eSuccess)
		{
			throw std::runtime_error("failed to create image!");
		}

		vk::MemoryRequirements memRequirements;
		_logicalDevice->getObject()->getImageMemoryRequirements(_depthImage, &memRequirements);

		vk::PhysicalDeviceMemoryProperties memProperties;
		_physicalDevice->getObject()->getMemoryProperties(&memProperties);
		vk::MemoryPropertyFlags properties = vk::MemoryPropertyFlagBits::eDeviceLocal;
		int memoryTypeIndex = -1;
		for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
		{
			if ((memRequirements.memoryTypeBits & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
			{
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

		if (_logicalDevice->getObject()->allocateMemory(&allocInfo, nullptr, &_depthImageMemory) != vk::Result::eSuccess) {
			throw std::runtime_error("failed to allocate image memory!");
		}

		_logicalDevice->getObject()->bindImageMemory(_depthImage, _depthImageMemory, 0);
	}
}

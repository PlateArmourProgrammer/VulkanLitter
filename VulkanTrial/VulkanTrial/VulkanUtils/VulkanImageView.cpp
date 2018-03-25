#include "VulkanImageView.h"
#include "VulkanPhysicalDevice.h"
#include "VulkanLogicalDevice.h"
#include "VulkanCommandPool.h"
#include "VulkanSingleTimeCommand.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

const std::string TEXTURE_PATH = "resources/images/lm.jpg";

namespace litter {
	VulkanImageView::VulkanImageView(VulkanPhysicalDevice* physicalDevice, VulkanLogicalDevice* logicalDevice, VulkanCommandPool* commandPool) {
		_physicalDevice = physicalDevice;
		_logicalDevice = logicalDevice;
		_commandPool = commandPool;

		createTextureImage();

		vk::ImageViewCreateInfo viewInfo = vk::ImageViewCreateInfo()
			.setImage(_image)
			.setViewType(vk::ImageViewType::e2D)
			.setFormat(vk::Format::eR8G8B8A8Unorm)
			.setSubresourceRange(
				vk::ImageSubresourceRange()
				.setAspectMask(vk::ImageAspectFlagBits::eColor)
				.setBaseMipLevel(0)
				.setLevelCount(1)
				.setBaseArrayLayer(0)
				.setLayerCount(1)
			);
		if (_logicalDevice->getObject()->createImageView(&viewInfo, nullptr, &_imageView) != vk::Result::eSuccess) {
			throw std::runtime_error("failed to create texture image view!");
		}

		vk::SamplerCreateInfo samplerInfo = vk::SamplerCreateInfo()
			.setMagFilter(vk::Filter::eLinear)
			.setMinFilter(vk::Filter::eLinear)
			.setAddressModeU(vk::SamplerAddressMode::eRepeat)
			.setAddressModeV(vk::SamplerAddressMode::eRepeat)
			.setAddressModeW(vk::SamplerAddressMode::eRepeat)
			.setAnisotropyEnable(VK_TRUE)
			.setMaxAnisotropy(16)
			.setBorderColor(vk::BorderColor::eIntOpaqueBlack)
			.setUnnormalizedCoordinates(VK_FALSE)
			.setCompareEnable(VK_FALSE)
			.setCompareOp(vk::CompareOp::eAlways)
			.setMipmapMode(vk::SamplerMipmapMode::eLinear);

		if (_logicalDevice->getObject()->createSampler(&samplerInfo, nullptr, &_sampler) != vk::Result::eSuccess) {
			throw std::runtime_error("failed to create texture sampler!");
		}
	}

	VulkanImageView::~VulkanImageView() {
		vk::Device* vkDevice = _logicalDevice->getObject();

		vkDevice->destroySampler(_sampler, nullptr);
		vkDevice->destroyImageView(_imageView, nullptr);
		vkDevice->destroyImage(_image, nullptr);
		vkDevice->freeMemory(_imageMemory, nullptr);
	}

	vk::ImageView* VulkanImageView::getObject() {
		return &_imageView;
	}

	vk::Sampler* VulkanImageView::getSampler() {
		return &_sampler;
	}

	void VulkanImageView::createTextureImage() {
		int texChannels;
		stbi_uc* pixels = stbi_load(TEXTURE_PATH.c_str(), &_width, &_height, &texChannels, STBI_rgb_alpha);
		vk::DeviceSize imageSize = _width * _height * 4;
		if (!pixels) {
			throw std::runtime_error("failed to load texture image!");
		}

		vk::Buffer stagingBuffer;
		vk::DeviceMemory stagingBufferMemory;
		createBuffer(imageSize,
			stagingBuffer, stagingBufferMemory);

		void* data;
		_logicalDevice->getObject()->mapMemory(stagingBufferMemory, 0, imageSize, vk::MemoryMapFlagBits(), &data);
		memcpy(data, pixels, static_cast<size_t>(imageSize));
		_logicalDevice->getObject()->unmapMemory(stagingBufferMemory);
		stbi_image_free(pixels);

		createImage();

		litter::VulkanSingleTimeCommand singleCmd0(_logicalDevice, _commandPool);
		vk::ImageMemoryBarrier barrier0 = vk::ImageMemoryBarrier()
			.setOldLayout(vk::ImageLayout::ePreinitialized)
			.setNewLayout(vk::ImageLayout::eTransferDstOptimal)
			.setSrcQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
			.setDstQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
			.setImage(_image)
			.setSubresourceRange(
				vk::ImageSubresourceRange()
				.setAspectMask(vk::ImageAspectFlagBits::eColor)
				.setBaseMipLevel(0)
				.setLevelCount(1)
				.setBaseArrayLayer(0)
				.setLayerCount(1)
			);
		barrier0.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
		barrier0.srcAccessMask = vk::AccessFlagBits::eHostWrite;
		barrier0.dstAccessMask = vk::AccessFlagBits::eTransferWrite;
		singleCmd0.getObject()->pipelineBarrier(vk::PipelineStageFlagBits::eTopOfPipe, vk::PipelineStageFlagBits::eTopOfPipe, vk::DependencyFlags(),
			0, nullptr,
			0, nullptr,
			1, &barrier0);

		copyBufferToImage(stagingBuffer);

		litter::VulkanSingleTimeCommand singleCmd1(_logicalDevice, _commandPool);
		vk::ImageMemoryBarrier barrier1 = vk::ImageMemoryBarrier()
			.setOldLayout(vk::ImageLayout::eTransferDstOptimal)
			.setNewLayout(vk::ImageLayout::eShaderReadOnlyOptimal)
			.setSrcQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
			.setDstQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
			.setImage(_image)
			.setSubresourceRange(
				vk::ImageSubresourceRange()
				.setAspectMask(vk::ImageAspectFlagBits::eColor)
				.setBaseMipLevel(0)
				.setLevelCount(1)
				.setBaseArrayLayer(0)
				.setLayerCount(1)
			);
		barrier1.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
		barrier1.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
		barrier1.dstAccessMask = vk::AccessFlagBits::eShaderRead;
		singleCmd1.getObject()->pipelineBarrier(vk::PipelineStageFlagBits::eTopOfPipe, vk::PipelineStageFlagBits::eTopOfPipe, vk::DependencyFlags(),
			0, nullptr,
			0, nullptr,
			1, &barrier1);

		_logicalDevice->getObject()->destroyBuffer(stagingBuffer, nullptr);
		_logicalDevice->getObject()->freeMemory(stagingBufferMemory, nullptr);
	}

	void VulkanImageView::createBuffer(vk::DeviceSize size, vk::Buffer& buffer, vk::DeviceMemory& bufferMemory) {
		vk::BufferCreateInfo bufferInfo = vk::BufferCreateInfo()
			.setSize(size)
			.setUsage(vk::BufferUsageFlagBits::eTransferSrc)
			.setSharingMode(vk::SharingMode::eExclusive);

		if (_logicalDevice->getObject()->createBuffer(&bufferInfo, nullptr, &buffer) != vk::Result::eSuccess) {
			throw std::runtime_error("failed to create vertex buffer!");
		}

		vk::MemoryRequirements memRequirements;
		_logicalDevice->getObject()->getBufferMemoryRequirements(buffer, &memRequirements);

		vk::MemoryPropertyFlags properties = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent;
		int memoryTypeIndex = -1;

		vk::PhysicalDeviceMemoryProperties memProperties;
		_physicalDevice->getObject()->getMemoryProperties(&memProperties);
		for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
			if ((memRequirements.memoryTypeBits & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
				memoryTypeIndex = i;
			}
		}
		if (memoryTypeIndex == -1) {
			throw std::runtime_error("failed to find suitable memory type!");
		}

		vk::MemoryAllocateInfo allocInfo = vk::MemoryAllocateInfo()
			.setAllocationSize(memRequirements.size)
			.setMemoryTypeIndex(memoryTypeIndex);

		if (_logicalDevice->getObject()->allocateMemory(&allocInfo, nullptr, &bufferMemory) != vk::Result::eSuccess) {
			throw std::runtime_error("failed to allocate vertex buffer memory!");
		}

		_logicalDevice->getObject()->bindBufferMemory(buffer, bufferMemory, 0);
	}

	void VulkanImageView::createImage()
	{
		vk::ImageCreateInfo imageInfo = vk::ImageCreateInfo()
			.setImageType(vk::ImageType::e2D)
			.setExtent(
				vk::Extent3D()
				.setWidth((uint32_t)_width)
				.setHeight((uint32_t)_height)
				.setDepth(1)
			)
			.setMipLevels(1)
			.setArrayLayers(1)
			.setFormat(vk::Format::eR8G8B8A8Unorm)
			.setTiling(vk::ImageTiling::eOptimal)
			.setInitialLayout(vk::ImageLayout::ePreinitialized)
			.setUsage(vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled)
			.setSharingMode(vk::SharingMode::eExclusive)
			.setSamples(vk::SampleCountFlagBits::e1);

		if (_logicalDevice->getObject()->createImage(&imageInfo, nullptr, &_image) != vk::Result::eSuccess) {
			throw std::runtime_error("failed to create image!");
		}

		vk::MemoryRequirements memRequirements;
		_logicalDevice->getObject()->getImageMemoryRequirements(_image, &memRequirements);

		vk::MemoryPropertyFlags properties = vk::MemoryPropertyFlagBits::eDeviceLocal;
		int memoryTypeIndex = -1;

		vk::PhysicalDeviceMemoryProperties memProperties;
		_physicalDevice->getObject()->getMemoryProperties(&memProperties);
		for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
			if ((memRequirements.memoryTypeBits & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
				memoryTypeIndex = i;
			}
		}
		if (memoryTypeIndex == -1) {
			throw std::runtime_error("failed to find suitable memory type!");
		}

		vk::MemoryAllocateInfo allocInfo = vk::MemoryAllocateInfo()
			.setAllocationSize(memRequirements.size)
			.setMemoryTypeIndex(memoryTypeIndex);

		if (_logicalDevice->getObject()->allocateMemory(&allocInfo, nullptr, &_imageMemory) != vk::Result::eSuccess) {
			throw std::runtime_error("failed to allocate image memory!");
		}

		_logicalDevice->getObject()->bindImageMemory(_image, _imageMemory, 0);
	}

	void VulkanImageView::copyBufferToImage(vk::Buffer buffer) {
		litter::VulkanSingleTimeCommand singleCmd(_logicalDevice, _commandPool);

		vk::BufferImageCopy region = vk::BufferImageCopy()
			.setBufferOffset(0)
			.setBufferRowLength(0)
			.setBufferImageHeight(0)
			.setImageSubresource(
				vk::ImageSubresourceLayers()
				.setAspectMask(vk::ImageAspectFlagBits::eColor)
				.setMipLevel(0)
				.setBaseArrayLayer(0)
				.setLayerCount(1)
			)
			.setImageOffset(vk::Offset3D().setX(0).setY(0).setZ(0))
			.setImageExtent(vk::Extent3D().setWidth(_width).setHeight(_height).setDepth(1));

		singleCmd.getObject()->copyBufferToImage(buffer, _image, vk::ImageLayout::eTransferDstOptimal, 1, &region);
	}
}

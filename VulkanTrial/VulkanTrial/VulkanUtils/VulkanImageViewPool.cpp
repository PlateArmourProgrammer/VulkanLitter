#include "VulkanImageViewPool.h"
#include "VulkanLogicalDevice.h"
#include "VulkanSwapChain.h"

namespace litter {
	VulkanImageViewPool::VulkanImageViewPool(VulkanSwapChain* swapChain, VulkanLogicalDevice* logicalDevice) {
		_swapChain = swapChain;
		_logicalDevice = logicalDevice;

		init();
	}

	VulkanImageViewPool::~VulkanImageViewPool() {
		cleanup();
	}

	void VulkanImageViewPool::init() {
		vk::Device* vkDevice = _logicalDevice->getObject();

		uint32_t imageCount;
		vkDevice->getSwapchainImagesKHR(*_swapChain->getObject(), &imageCount, nullptr);
		_images.resize(imageCount);
		vkDevice->getSwapchainImagesKHR(*_swapChain->getObject(), &imageCount, _images.data());

		_imageViews.resize(_images.size());
		for (size_t i = 0; i < _images.size(); i++) {
			_imageViews[i] = createImageView(_images[i], *_swapChain->getImageFormat(), vk::ImageAspectFlagBits::eColor);
		}
	}

	void VulkanImageViewPool::cleanup() {
		for (size_t i = 0; i < _imageViews.size(); i++) {
			_logicalDevice->getObject()->destroyImageView(_imageViews[i], nullptr);
		}
	}

	size_t VulkanImageViewPool::getImageViewCount() {
		return _imageViews.size();
	}

	vk::ImageView* VulkanImageViewPool::getImageViewAt(size_t idx) {
		return &(_imageViews[idx]);
	}

	vk::ImageView VulkanImageViewPool::createImageView(vk::Image image, vk::Format format, vk::ImageAspectFlags aspectFlags) {
		vk::ImageViewCreateInfo viewInfo = vk::ImageViewCreateInfo()
			.setImage(image)
			.setViewType(vk::ImageViewType::e2D)
			.setFormat(format)
			.setSubresourceRange(
				vk::ImageSubresourceRange()
				.setAspectMask(aspectFlags)
				.setBaseMipLevel(0)
				.setLevelCount(1)
				.setBaseArrayLayer(0)
				.setLayerCount(1)
			);

		vk::ImageView imageView;
		if (_logicalDevice->getObject()->createImageView(&viewInfo, nullptr, &imageView) != vk::Result::eSuccess) {
			throw std::runtime_error("failed to create texture image view!");
		}

		return imageView;
	}
}

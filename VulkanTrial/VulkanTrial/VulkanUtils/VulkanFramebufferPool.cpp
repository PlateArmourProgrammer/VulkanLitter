#include "VulkanFramebufferPool.h"
#include "VulkanLogicalDevice.h"
#include "VulkanImageViewPool.h"
#include "VulkanRenderPass.h"
#include "VulkanSwapChain.h"

namespace litter {
	VulkanFramebufferPool::VulkanFramebufferPool(VulkanLogicalDevice* logicalDevice, VulkanImageViewPool* imageViewPool,
		vk::ImageView* depthImageView, VulkanSwapChain* swapChain, VulkanRenderPass* renderPass) {
		_logicalDevice = logicalDevice;

		init(imageViewPool, depthImageView, swapChain, renderPass);
	}

	VulkanFramebufferPool::~VulkanFramebufferPool() {
		cleanup();
	}

	void VulkanFramebufferPool::init(VulkanImageViewPool* imageViewPool, vk::ImageView* depthImageView, VulkanSwapChain* swapChain, VulkanRenderPass* renderPass) {
		_framebuffers.resize(imageViewPool->getImageViewCount());

		for (size_t i = 0; i < imageViewPool->getImageViewCount(); i++) {
			std::array<vk::ImageView, 2> attachments = {
				*imageViewPool->getImageViewAt(i),
				*depthImageView
			};

			vk::FramebufferCreateInfo framebufferInfo = vk::FramebufferCreateInfo();
			framebufferInfo.renderPass = *renderPass->getObject();
			framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
			framebufferInfo.pAttachments = attachments.data();
			framebufferInfo.width = swapChain->getExtentWidth();
			framebufferInfo.height = swapChain->getExtentHeight();
			framebufferInfo.layers = 1;

			if (_logicalDevice->getObject()->createFramebuffer(&framebufferInfo, nullptr, &_framebuffers[i]) != vk::Result::eSuccess) {
				throw std::runtime_error("failed to create framebuffer!");
			}
		}
	}

	void VulkanFramebufferPool::cleanup() {
		vk::Device* vkDevice = _logicalDevice->getObject();
		for (size_t i = 0; i < _framebuffers.size(); i++) {
			vkDevice->destroyFramebuffer(_framebuffers[i], nullptr);
		}
	}

	size_t VulkanFramebufferPool::getFramebufferCount() {
		return _framebuffers.size();
	}

	vk::Framebuffer* VulkanFramebufferPool::getFramebufferAt(size_t idx) {
		return &(_framebuffers[idx]);
	}
}

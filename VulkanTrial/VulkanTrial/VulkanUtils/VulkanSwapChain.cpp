#include "VulkanSwapChain.h"
#include "VulkanPhysicalDevice.h"
#include "VulkanLogicalDevice.h"
#include "VulkanSurface.h"
#include "VulkanStructs.h"

namespace litter {
	VulkanSwapChain::VulkanSwapChain(VulkanLogicalDevice* logicalDevice, VulkanPhysicalDevice* physicalDevice, VulkanSurface* surface,
		uint32_t width, uint32_t height) {
		_logicalDevice = logicalDevice;
		_physicalDevice = physicalDevice;
		_surface = surface;

		init(width, height);
	}

	VulkanSwapChain::~VulkanSwapChain() {
		cleanup();
	}

	void VulkanSwapChain::init(uint32_t width, uint32_t height) {
		SwapChainSupportDetails* swapChainSupport = _physicalDevice->getSwapChainSupport();

		vk::SurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport->formats);
		vk::PresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport->presentModes);
		vk::Extent2D extent = chooseSwapExtent(swapChainSupport->capabilities, width, height);

		uint32_t imageCount = swapChainSupport->capabilities.minImageCount + 1;
		if (swapChainSupport->capabilities.maxImageCount > 0 && imageCount > swapChainSupport->capabilities.maxImageCount) {
			imageCount = swapChainSupport->capabilities.maxImageCount;
		}

		vk::SwapchainCreateInfoKHR createInfo = vk::SwapchainCreateInfoKHR()
			.setSurface(*_surface->getObject())
			.setMinImageCount(imageCount)
			.setImageFormat(surfaceFormat.format)
			.setImageColorSpace(surfaceFormat.colorSpace)
			.setImageExtent(extent)
			.setImageArrayLayers(1)
			.setImageUsage(vk::ImageUsageFlagBits::eColorAttachment);

		QueueFamilyIndices* indices = _physicalDevice->getQueueFamilyIndices();
		uint32_t queueFamilyIndices[] = { (uint32_t)indices->graphicsFamily, (uint32_t)indices->presentFamily };

		if (indices->graphicsFamily != indices->presentFamily)
		{
			createInfo.setImageSharingMode(vk::SharingMode::eConcurrent);
			createInfo.setQueueFamilyIndexCount(2);
			createInfo.setPQueueFamilyIndices(queueFamilyIndices);
		}
		else {
			createInfo.setImageSharingMode(vk::SharingMode::eExclusive);
		}

		createInfo.setPreTransform(swapChainSupport->capabilities.currentTransform);
		createInfo.setCompositeAlpha(vk::CompositeAlphaFlagBitsKHR::eOpaque);
		createInfo.setPresentMode(presentMode);
		createInfo.setClipped(VK_TRUE);

		createInfo.setOldSwapchain(VK_NULL_HANDLE);


		if (_logicalDevice->getObject()->createSwapchainKHR(&createInfo, nullptr, &_swapChain) != vk::Result::eSuccess)
		{
			throw std::runtime_error("failed to create swap chain!");
		}

		_imageFormat = surfaceFormat.format;
		_extent = extent;
	}

	void VulkanSwapChain::cleanup() {
		_logicalDevice->getObject()->destroySwapchainKHR(_swapChain, nullptr);
	}

	vk::SwapchainKHR* VulkanSwapChain::getObject() {
		return &_swapChain;
	}

	vk::Format* VulkanSwapChain::getImageFormat() {
		return &_imageFormat;
	}

	vk::Extent2D* VulkanSwapChain::getExtent() {
		return &_extent;
	}

	uint32_t VulkanSwapChain::getExtentWidth() {
		return _extent.width;
	}

	uint32_t VulkanSwapChain::getExtentHeight() {
		return _extent.height;
	}

	vk::SurfaceFormatKHR VulkanSwapChain::chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats) {
		if (availableFormats.size() == 1 && availableFormats[0].format == vk::Format::eUndefined) {
			return{ vk::Format::eB8G8R8A8Unorm, vk::ColorSpaceKHR::eSrgbNonlinear };
		}

		for (const auto& availableFormat : availableFormats) {
			if (availableFormat.format == vk::Format::eB8G8R8A8Unorm && availableFormat.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear) {
				return availableFormat;
			}
		}

		return availableFormats[0];
	}

	vk::PresentModeKHR VulkanSwapChain::chooseSwapPresentMode(const std::vector<vk::PresentModeKHR> availablePresentModes) {
		vk::PresentModeKHR bestMode = vk::PresentModeKHR::eFifo;

		for (const auto& availablePresentMode : availablePresentModes) {
			if (availablePresentMode == vk::PresentModeKHR::eMailbox) {
				return availablePresentMode;
			} else if (availablePresentMode == vk::PresentModeKHR::eImmediate) {
				bestMode = availablePresentMode;
			}
		}

		return bestMode;
	}

	vk::Extent2D VulkanSwapChain::chooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities, uint32_t width, uint32_t height) {
		if (capabilities.currentExtent.width != _ULLONG_MAX) {
			return capabilities.currentExtent;
		}
		else {
			vk::Extent2D actualExtent = { width, height };

			actualExtent.width = max(capabilities.minImageExtent.width, min(capabilities.maxImageExtent.width, actualExtent.width));
			actualExtent.height = max(capabilities.minImageExtent.height, min(capabilities.maxImageExtent.height, actualExtent.height));

			return actualExtent;
		}
	}
}

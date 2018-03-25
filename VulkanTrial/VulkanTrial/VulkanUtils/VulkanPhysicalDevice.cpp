#include "VulkanPhysicalDevice.h"
#include "VulkanInstance.h"
#include "VulkanSurface.h"
#include "StdC.h"

namespace litter {
	VulkanPhysicalDevice::VulkanPhysicalDevice(VulkanInstance* instance, VulkanSurface* surface) {
		_surface = surface;

		_physicalDevice = VK_NULL_HANDLE;
		vk::Instance* vkInstance = instance->getObject();

		uint32_t deviceCount = 0;
		vkInstance->enumeratePhysicalDevices(&deviceCount, nullptr);

		if (deviceCount == 0) {
			throw std::runtime_error("failed to find GPUs with Vulkan support!");
		}

		std::vector<vk::PhysicalDevice> devices(deviceCount);
		vkInstance->enumeratePhysicalDevices(&deviceCount, devices.data());

		for (const auto& device : devices) {
			QueueFamilyIndices indices = findQueueFamilies(device, surface);

			if (!indices.isComplete()) {
				continue;
			}

			if (!checkDeviceExtensionSupport(device)) {
				continue;
			}

			SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device, surface);
			if (swapChainSupport.formats.empty() || swapChainSupport.presentModes.empty()) {
				continue;
			}

			if (isDeviceSuitable(device)) {
				_physicalDevice = device;
				_queueFamilyIndices = indices;
				_swapChainSupportDetails = swapChainSupport;
				break;
			}
		}

		if (_physicalDevice == VK_NULL_HANDLE) {
			throw std::runtime_error("failed to find a suitable GPU!");
		}

		const std::vector<vk::Format> candidates = {
			vk::Format::eD32Sfloat,
			vk::Format::eD32SfloatS8Uint,
			vk::Format::eD24UnormS8Uint
		};
		vk::FormatFeatureFlags features = vk::FormatFeatureFlagBits::eDepthStencilAttachment;

		for (vk::Format format : candidates) {
			vk::FormatProperties props;
			_physicalDevice.getFormatProperties(format, &props);

			if ((props.optimalTilingFeatures & features) == features) {
				_depthFormat = format;
				break;
			}
		}
	}

	VulkanPhysicalDevice::~VulkanPhysicalDevice() {
	}

	vk::PhysicalDevice* VulkanPhysicalDevice::getObject() {
		if (_physicalDevice == VK_NULL_HANDLE) {
			return nullptr;
		}
		return &_physicalDevice;
	}

	QueueFamilyIndices* VulkanPhysicalDevice::getQueueFamilyIndices() {
		if (_physicalDevice == VK_NULL_HANDLE) {
			return nullptr;
		}
		return &_queueFamilyIndices;
	}

	SwapChainSupportDetails* VulkanPhysicalDevice::getSwapChainSupport() {
		if (_physicalDevice == VK_NULL_HANDLE) {
			return nullptr;
		}
		// todo: do not query again
		_swapChainSupportDetails = querySwapChainSupport(_physicalDevice, _surface);
		return &_swapChainSupportDetails;
	}

	vk::Format* VulkanPhysicalDevice::getDepthFormat() {
		return &_depthFormat;
	}

	bool VulkanPhysicalDevice::isDeviceSuitable(vk::PhysicalDevice device)
	{
		vk::PhysicalDeviceFeatures supportedFeatures;
		device.getFeatures(&supportedFeatures);

		return supportedFeatures.samplerAnisotropy;
	}

	QueueFamilyIndices VulkanPhysicalDevice::findQueueFamilies(const vk::PhysicalDevice& device, VulkanSurface* surface) {
		QueueFamilyIndices indices;

		uint32_t queueFamilyCount = 0;
		device.getQueueFamilyProperties(&queueFamilyCount, nullptr);

		std::vector<vk::QueueFamilyProperties> queueFamilies(queueFamilyCount);
		device.getQueueFamilyProperties(&queueFamilyCount, queueFamilies.data());

		int i = 0;
		for (const auto& queueFamily : queueFamilies) {
			if (queueFamily.queueCount > 0 && queueFamily.queueFlags & vk::QueueFlagBits::eGraphics) {
				indices.graphicsFamily = i;
			}

			VkBool32 presentSupport = false;
			device.getSurfaceSupportKHR(i, *surface->getObject(), &presentSupport);

			if (queueFamily.queueCount > 0 && presentSupport) {
				indices.presentFamily = i;
			}

			if (indices.isComplete()) {
				break;
			}

			i++;
		}

		return indices;
	}

	bool VulkanPhysicalDevice::checkDeviceExtensionSupport(const vk::PhysicalDevice& device) {
		uint32_t extensionCount = 0;
		device.enumerateDeviceExtensionProperties(nullptr, &extensionCount, nullptr);

		std::vector<vk::ExtensionProperties> availableExtensions(extensionCount);
		device.enumerateDeviceExtensionProperties(nullptr, &extensionCount, availableExtensions.data());

		std::set<std::string> requiredExtensions(_deviceExtensions.begin(), _deviceExtensions.end());

		for (const auto& extension : availableExtensions) {
			requiredExtensions.erase(extension.extensionName);
		}

		return requiredExtensions.empty();
	}

	SwapChainSupportDetails VulkanPhysicalDevice::querySwapChainSupport(const vk::PhysicalDevice& device, VulkanSurface* surface) {
		SwapChainSupportDetails details;

		device.getSurfaceCapabilitiesKHR(*surface->getObject(), &details.capabilities);

		uint32_t formatCount;
		device.getSurfaceFormatsKHR(*surface->getObject(), &formatCount, nullptr);

		if (formatCount != 0) {
			details.formats.resize(formatCount);
			device.getSurfaceFormatsKHR(*surface->getObject(), &formatCount, details.formats.data());
		}

		uint32_t presentModeCount;
		device.getSurfacePresentModesKHR(*surface->getObject(), &presentModeCount, nullptr);

		if (presentModeCount != 0) {
			details.presentModes.resize(presentModeCount);
			device.getSurfacePresentModesKHR(*surface->getObject(), &presentModeCount, details.presentModes.data());
		}

		return details;
	}
}

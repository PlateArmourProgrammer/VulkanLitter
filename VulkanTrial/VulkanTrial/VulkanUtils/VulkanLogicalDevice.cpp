#include "VulkanLogicalDevice.h"
#include "VulkanPhysicalDevice.h"
#include "StdC.h"

namespace litter {
	VulkanLogicalDevice::VulkanLogicalDevice(VulkanPhysicalDevice* physicalDevice) {
		QueueFamilyIndices* indices = physicalDevice->getQueueFamilyIndices();

		std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;
		std::set<int> uniqueQueueFamilies = { indices->graphicsFamily, indices->presentFamily };

		float queuePriority = 1.0f;
		for (int queueFamily : uniqueQueueFamilies) {
			vk::DeviceQueueCreateInfo queueCreateInfo = vk::DeviceQueueCreateInfo()
				.setQueueFamilyIndex(queueFamily)
				.setQueueCount(1)
				.setPQueuePriorities(&queuePriority);

			queueCreateInfos.push_back(queueCreateInfo);
		}

		vk::PhysicalDeviceFeatures deviceFeatures = vk::PhysicalDeviceFeatures()
			.setSamplerAnisotropy(VK_TRUE);

		vk::DeviceCreateInfo createInfo = vk::DeviceCreateInfo()
			.setQueueCreateInfoCount(static_cast<uint32_t>(queueCreateInfos.size()))
			.setPQueueCreateInfos(queueCreateInfos.data())
			.setPEnabledFeatures(&deviceFeatures)
			.setEnabledExtensionCount(static_cast<uint32_t>(_deviceExtensions.size()))
			.setPpEnabledExtensionNames(_deviceExtensions.data());

		if (enableValidationLayers) {
			createInfo.setEnabledLayerCount(static_cast<uint32_t>(_layers.size()));
			createInfo.setPpEnabledLayerNames(_layers.data());
		} else {
			createInfo.enabledLayerCount = 0;
		}

		const auto ret = physicalDevice->getObject()->createDevice(&createInfo, nullptr, &_device);
		if (ret != vk::Result::eSuccess) {
			throw std::runtime_error("failed to create logical device!");
		}

		_device.getQueue(indices->graphicsFamily, 0, &_graphicsQueue);
		_device.getQueue(indices->presentFamily, 0, &_presentQueue);
	}

	VulkanLogicalDevice::~VulkanLogicalDevice() {
		_device.destroy();
	}

	vk::Device* VulkanLogicalDevice::getObject() {
		return &_device;
	}

	vk::Queue* VulkanLogicalDevice::getGraphicsQueue() {
		return &_graphicsQueue;
	}

	vk::Queue* VulkanLogicalDevice::getPresentQueue() {
		return &_presentQueue;
	}
}

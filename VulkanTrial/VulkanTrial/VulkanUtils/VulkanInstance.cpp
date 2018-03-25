#include "VulkanInstance.h"

namespace litter {
	VulkanInstance::VulkanInstance() {
		if (enableValidationLayers && !checkValidationLayerSupport()) {
			throw std::runtime_error("validation layers requested, but not available!");
		}

		vk::ApplicationInfo appInfo = vk::ApplicationInfo()
			.setPApplicationName("Vulkan C++ Windowed Program Template")
			.setApplicationVersion(1)
			.setPEngineName("LunarG SDK")
			.setEngineVersion(1)
			.setApiVersion(VK_API_VERSION_1_0);

		vk::InstanceCreateInfo instInfo = vk::InstanceCreateInfo()
			.setFlags(vk::InstanceCreateFlags())
			.setPApplicationInfo(&appInfo);

		auto extensions = getAvailableWSIExtensions();
		instInfo.setEnabledExtensionCount(static_cast<uint32_t>(extensions.size()));
		instInfo.setPpEnabledExtensionNames(extensions.data());

		if (enableValidationLayers) {
			instInfo.setEnabledLayerCount(static_cast<uint32_t>(_layers.size()));
			instInfo.setPpEnabledLayerNames(_layers.data());
		} else {
			instInfo.enabledLayerCount = 0;
		}

		try {
			_vkInstance = vk::createInstance(instInfo);
		} catch (const std::exception& e) {
			throw std::runtime_error("failed to create instance!");
		}
	}

	VulkanInstance::~VulkanInstance() {
		_vkInstance.destroy();
	}

	vk::Instance* VulkanInstance::getObject() {
		return &_vkInstance;
	}

	bool VulkanInstance::checkValidationLayerSupport() {
		uint32_t layerCount;
		vk::enumerateInstanceLayerProperties(&layerCount, nullptr);

		std::vector<vk::LayerProperties> availableLayers(layerCount);
		vk::enumerateInstanceLayerProperties(&layerCount, availableLayers.data());

		for (const char* layerName : _layers) {
			bool layerFound = false;

			for (const auto& layerProperties : availableLayers) {
				if (strcmp(layerName, layerProperties.layerName) == 0) {
					layerFound = true;
					break;
				}
			}

			if (!layerFound) {
				return false;
			}
		}

		return true;
	}

	std::vector<const char*> VulkanInstance::getAvailableWSIExtensions() {
		std::vector<const char*> extensions;
		extensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
		extensions.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);

		if (enableValidationLayers) {
			extensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
		}

		return extensions;
	}
}

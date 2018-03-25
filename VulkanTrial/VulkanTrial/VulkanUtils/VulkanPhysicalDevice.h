#ifndef VulkanPhysicalDevice_h_
#define VulkanPhysicalDevice_h_

#include "Base/BaseObject.h"
#include "VulkanHeader.h"
#include "VulkanStructs.h"

namespace litter {
	class VulkanInstance;
	class VulkanSurface;

	class VulkanPhysicalDevice : public BaseObject {
	public:
		VulkanPhysicalDevice(VulkanInstance* instance, VulkanSurface* surface);
		~VulkanPhysicalDevice();

		vk::PhysicalDevice* getObject();
		QueueFamilyIndices* getQueueFamilyIndices();
		SwapChainSupportDetails* getSwapChainSupport();
		vk::Format* getDepthFormat();
	private:
		bool isDeviceSuitable(vk::PhysicalDevice device);
		QueueFamilyIndices findQueueFamilies(const vk::PhysicalDevice& device, VulkanSurface* surface);
		bool checkDeviceExtensionSupport(const vk::PhysicalDevice& device);
		SwapChainSupportDetails querySwapChainSupport(const vk::PhysicalDevice& device, VulkanSurface* surface);

	private:
		vk::PhysicalDevice _physicalDevice;
		QueueFamilyIndices _queueFamilyIndices;
		SwapChainSupportDetails _swapChainSupportDetails;
		vk::Format _depthFormat;

		VulkanSurface* _surface;
	};
}

#endif // !VulkanPhysicalDevice_h_

#ifndef VulkanImageView_h_
#define VulkanImageView_h_

#include "Base/BaseObject.h"
#include "VulkanHeader.h"

namespace litter {
	class VulkanPhysicalDevice;
	class VulkanLogicalDevice;
	class VulkanCommandPool;

	class VulkanImageView : public BaseObject {
	public:
		VulkanImageView(VulkanPhysicalDevice* physicalDevice, VulkanLogicalDevice* logicalDevice, VulkanCommandPool* commandPool);
		~VulkanImageView();

		vk::ImageView* getObject();
		vk::Sampler* getSampler();
	private:
		void createTextureImage();
		void createBuffer(vk::DeviceSize size, vk::Buffer& buffer, vk::DeviceMemory& bufferMemory);
		void createImage();
		void copyBufferToImage(vk::Buffer buffer);

	private:
		vk::Image _image;
		vk::DeviceMemory _imageMemory;
		vk::ImageView _imageView;
		vk::Sampler _sampler;
		int _width;
		int _height;

		VulkanPhysicalDevice* _physicalDevice;
		VulkanLogicalDevice* _logicalDevice;
		VulkanCommandPool* _commandPool;
	};
}

#endif // !VulkanImageView_h_

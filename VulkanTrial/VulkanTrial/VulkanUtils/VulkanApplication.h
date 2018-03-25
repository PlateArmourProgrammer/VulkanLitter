#ifndef VULKAN_APPLICATION_H_
#define VULKAN_APPLICATION_H_

#include "VulkanHeader.h"
#include "VulkanStructs.h"
#include "../Renderer.h"

#include "VulkanInstance.h"
#include "VulkanPhysicalDevice.h"
#include "VulkanLogicalDevice.h"
#include "VulkanSurface.h"
#include "VulkanImageViewPool.h"
#include "VulkanPipeline.h"
#include "VulkanFramebufferPool.h"
#include "VulkanRenderPass.h"
#include "VulkanCommandPool.h"
#include "VulkanCommandBuffers.h"
#include "VulkanDepthResource.h"
#include "VulkanSingleTimeCommand.h"
#include "VulkanSwapChain.h"
#include "VulkanImageView.h"
#include "VulkanDescriptorSetLayout.h"
#include "RenderCommand/TextureRenderCmd.h"

class VulkanApplication
{
public:
	VulkanApplication();
	~VulkanApplication();

	bool init();
	void run();
	void cleanup();

private:
	bool initWindow();
	bool initVulkan();
	void setupDebugCallback();
	void createUniformBuffer();
	void createDescriptorPool();
	void createDescriptorSet();
	void createSemaphores();
	void transitionImageLayout(vk::Image image, vk::Format format, vk::ImageLayout oldLayout, vk::ImageLayout newLayout);
	bool hasStencilComponent(vk::Format format);
	void createBuffer(vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties, vk::Buffer& buffer, vk::DeviceMemory& bufferMemory);
	void copyBuffer(vk::Buffer srcBuffer, vk::Buffer dstBuffer, vk::DeviceSize size);
	uint32_t findMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties);
	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objType,
		uint64_t obj, size_t location, int32_t code, const char* layerPrefix, const char* msg, void* userData);

	void mainLoop();
	void drawFrame();
	void updateUniformBuffer();
	void recreateSwapChain();

private:
	SDL_Window* _window;

	vk::DebugReportCallbackEXT _callback;

	vk::Buffer _uniformBuffer;
	vk::DeviceMemory _uniformBufferMemory;

	vk::Semaphore _imageAvailableSemaphore;
	vk::Semaphore _renderFinishedSemaphore;

	vk::DescriptorPool _descriptorPool;
	vk::DescriptorSet _descriptorSet;

	uint32_t _windowWidth;
	uint32_t _windowHeight;

//------------------------------------------------------------------
	litter::VulkanInstance* _instance;
	litter::VulkanPhysicalDevice* _physicalDevice;
	litter::VulkanLogicalDevice* _logicalDevice;
	litter::VulkanSurface* _surface;
	litter::VulkanImageViewPool* _imageViewPool;
	litter::VulkanPipeline* _pipeline;
	litter::VulkanFramebufferPool* _framebufferPool;
	litter::VulkanRenderPass* _renderPass;
	litter::VulkanCommandPool* _commandPool;
	litter::VulkanCommandBuffers* _commandBuffers;
	litter::VulkanDepthResource* _depthResource;
	litter::VulkanSwapChain* _swapChain;
	litter::VulkanImageView* _imageView;
	litter::VulkanDescriptorSetLayout* _descriptorSetLayout;
	litter::TextureRenderCmd* _textureRenderCmd;
};

#endif // !VULKAN_APPLICATION_H_

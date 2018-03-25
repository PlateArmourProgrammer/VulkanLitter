#include "VulkanApplication.h"

#include "../StdC.h"

vk::Result CreateDebugReportCallbackEXT(vk::Instance instance, const vk::DebugReportCallbackCreateInfoEXT* pCreateInfo, const vk::AllocationCallbacks* pAllocator, vk::DebugReportCallbackEXT* pCallback)
{
	auto func = (PFN_vkCreateDebugReportCallbackEXT)instance.getProcAddr("vkCreateDebugReportCallbackEXT");
	if (func != nullptr)
	{
		return (vk::Result)func((VkInstance)instance, (VkDebugReportCallbackCreateInfoEXT*)pCreateInfo, (VkAllocationCallbacks*)pAllocator, (VkDebugReportCallbackEXT*)pCallback);
	}
	else
	{
		return vk::Result::eErrorExtensionNotPresent;
	}
}

void DestroyDebugReportCallbackEXT(vk::Instance instance, vk::DebugReportCallbackEXT callback, const vk::AllocationCallbacks* pAllocator)
{
	auto func = (PFN_vkDestroyDebugReportCallbackEXT)instance.getProcAddr("vkDestroyDebugReportCallbackEXT");
	if (func != nullptr)
	{
		func((VkInstance)instance, (VkDebugReportCallbackEXT)callback, (VkAllocationCallbacks*)pAllocator);
	}
}

VulkanApplication::VulkanApplication()
	: _windowWidth(0)
	, _windowHeight(0)
{
}

VulkanApplication::~VulkanApplication()
{
}

bool VulkanApplication::init()
{
	if (initWindow() && initVulkan())
	{
		return true;
	}
	return false;
}

void VulkanApplication::run()
{
	mainLoop();
}

void VulkanApplication::cleanup()
{
	delete _depthResource;
	delete _framebufferPool;
	delete _commandBuffers;
	delete _renderPass;
	delete _swapChain;
	delete _imageViewPool;

	vk::Device* device = _logicalDevice->getObject();

	device->destroyDescriptorPool(_descriptorPool, nullptr);
	delete _descriptorSetLayout;

	device->destroyBuffer(_uniformBuffer, nullptr);
	device->freeMemory(_uniformBufferMemory, nullptr);

	delete _textureRenderCmd;

	device->destroySemaphore(_renderFinishedSemaphore, nullptr);
	device->destroySemaphore(_imageAvailableSemaphore, nullptr);

	delete _commandPool;

	delete _logicalDevice;

	vk::Instance* vkInstance = _instance->getObject();
	DestroyDebugReportCallbackEXT(*vkInstance, _callback, nullptr);
	delete _surface;

	SDL_DestroyWindow(_window);
	SDL_Quit();

	delete _instance;
}

void VulkanApplication::mainLoop()
{
	bool stillRunning = true;
	while (stillRunning)
	{
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
			case SDL_WINDOWEVENT:
				if (event.window.event == SDL_WINDOWEVENT_RESIZED)
				{
					_windowWidth = event.window.data1;
					_windowHeight = event.window.data2;
					recreateSwapChain();
				}
				break;

			case SDL_QUIT:
				stillRunning = false;
				break;

			default:
				updateUniformBuffer();
				drawFrame();
				break;
			}
		}

		SDL_Delay(10);
	}

	_logicalDevice->getObject()->waitIdle();
}

void VulkanApplication::updateUniformBuffer()
{
	static auto startTime = std::chrono::high_resolution_clock::now();
	auto currentTime = std::chrono::high_resolution_clock::now();
	float time = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - startTime).count() / 1000.0f;

	UniformBufferObject ubo = {};
	ubo.model = glm::rotate(glm::mat4(), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	ubo.proj = glm::perspective(glm::radians(45.0f), _swapChain->getExtentWidth() / (float)_swapChain->getExtentHeight(), 0.1f, 10.0f);
	ubo.proj[1][1] *= -1;

	void* data;
	_logicalDevice->getObject()->mapMemory(_uniformBufferMemory, 0, sizeof(ubo), vk::MemoryMapFlagBits(), &data);
	memcpy(data, &ubo, sizeof(ubo));
	_logicalDevice->getObject()->unmapMemory(_uniformBufferMemory);
}

void VulkanApplication::drawFrame() {
	uint32_t imageIndex;
	vk::Result result = _logicalDevice->getObject()->acquireNextImageKHR(*_swapChain->getObject(), _ULLONG_MAX, _imageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);
	if (result == vk::Result::eErrorOutOfDateKHR)
	{
		recreateSwapChain();
		return;
	}
	else if (result != vk::Result::eSuccess && result != vk::Result::eSuboptimalKHR)
	{
		throw std::runtime_error("failed to acquire swap chain image!");
	}

	vk::SubmitInfo submitInfo = vk::SubmitInfo();

	vk::Semaphore waitSemaphores[] = { _imageAvailableSemaphore };
	vk::PipelineStageFlags waitStages[] = { vk::PipelineStageFlagBits::eColorAttachmentOutput };
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;

	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = _commandBuffers->getBufferAt(imageIndex);

	vk::Semaphore signalSemaphores[] = { _renderFinishedSemaphore };
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	if (_logicalDevice->getGraphicsQueue()->submit(1, &submitInfo, VK_NULL_HANDLE) != vk::Result::eSuccess)
	{
		throw std::runtime_error("failed to submit draw command buffer!");
	}

	vk::PresentInfoKHR presentInfo = vk::PresentInfoKHR();

	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;

	vk::SwapchainKHR swapChains[] = { *_swapChain->getObject() };
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;

	presentInfo.pImageIndices = &imageIndex;

	result = _logicalDevice->getPresentQueue()->presentKHR(&presentInfo);
	if (result == vk::Result::eErrorOutOfDateKHR || result == vk::Result::eSuboptimalKHR)
	{
		recreateSwapChain();
	}
	else if (result != vk::Result::eSuccess)
	{
		throw std::runtime_error("failed to present swap chain image!");
	}

	_logicalDevice->getPresentQueue()->waitIdle();
}

bool VulkanApplication::initWindow()
{
	_windowWidth = 1280;
	_windowHeight = 720;
	_window = SDL_CreateWindow("Vulkan Window", SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED, _windowWidth, _windowHeight,
		SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
	if (_window == NULL)
	{
		std::cout << "Could not create SDL window." << std::endl;
		return false;
	}

	return true;
}

bool VulkanApplication::initVulkan()
{
	_instance = new litter::VulkanInstance();
	setupDebugCallback();
	_surface = new litter::VulkanSurface(_instance, _window);
	_physicalDevice = new litter::VulkanPhysicalDevice(_instance, _surface);
	_logicalDevice = new litter::VulkanLogicalDevice(_physicalDevice);
	_swapChain = new litter::VulkanSwapChain(_logicalDevice, _physicalDevice, _surface, _windowWidth, _windowHeight);
	_imageViewPool = new litter::VulkanImageViewPool(_swapChain, _logicalDevice);

	_renderPass = new litter::VulkanRenderPass(_logicalDevice, _swapChain->getImageFormat(), _physicalDevice);
	
	_descriptorSetLayout = new litter::VulkanDescriptorSetLayout(_logicalDevice);
	_pipeline = new litter::VulkanPipeline(_logicalDevice, _swapChain, _descriptorSetLayout, _renderPass);
	_commandPool = new litter::VulkanCommandPool(_logicalDevice, _physicalDevice);
	_depthResource = new litter::VulkanDepthResource(_logicalDevice, _physicalDevice, _swapChain, _commandPool);
	_framebufferPool = new litter::VulkanFramebufferPool(_logicalDevice, _imageViewPool, _depthResource->getImageView(), _swapChain, _renderPass);
	_imageView = new litter::VulkanImageView(_physicalDevice, _logicalDevice, _commandPool);
	_textureRenderCmd = new litter::TextureRenderCmd(_physicalDevice, _logicalDevice, _commandPool);
	createUniformBuffer();
	createDescriptorPool();
	createDescriptorSet();
	_commandBuffers = new litter::VulkanCommandBuffers(_logicalDevice, _commandPool,
		_framebufferPool, _renderPass, _pipeline, _swapChain, &_descriptorSet, _textureRenderCmd);
	createSemaphores();

	return true;
}

void VulkanApplication::setupDebugCallback()
{
	if (!enableValidationLayers) return;

	vk::DebugReportCallbackCreateInfoEXT createInfo = vk::DebugReportCallbackCreateInfoEXT()
		.setFlags(vk::DebugReportFlagBitsEXT::eError | vk::DebugReportFlagBitsEXT::eWarning)
		.setPfnCallback(debugCallback);

	if (CreateDebugReportCallbackEXT(*_instance->getObject(), &createInfo, nullptr, &_callback) != vk::Result::eSuccess)
	{
		throw std::runtime_error("failed to set up debug callback!");
	}
}

void VulkanApplication::recreateSwapChain()
{
	_logicalDevice->getObject()->waitIdle();

	_depthResource->cleanup();
	_framebufferPool->cleanup();
	_commandBuffers->cleanup();
	_renderPass->cleanup();
	_swapChain->cleanup();
	_imageViewPool->cleanup();

	_swapChain->init(_windowWidth, _windowHeight);
	_imageViewPool->init();

	_renderPass->init(_swapChain->getImageFormat(), _physicalDevice);
	
	_pipeline->init(_swapChain, _descriptorSetLayout, _renderPass);
	_depthResource->init(_swapChain, _commandPool);
	_framebufferPool->init(_imageViewPool, _depthResource->getImageView(), _swapChain, _renderPass);
	_commandBuffers->init(_framebufferPool, _renderPass, _pipeline, _swapChain, &_descriptorSet, _textureRenderCmd);
}

void VulkanApplication::createUniformBuffer()
{
	vk::DeviceSize bufferSize = sizeof(UniformBufferObject);
	createBuffer(bufferSize, vk::BufferUsageFlagBits::eUniformBuffer,
		vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
		_uniformBuffer, _uniformBufferMemory);
}

void VulkanApplication::createDescriptorPool()
{
	std::array<vk::DescriptorPoolSize, 2> poolSizes = {
		vk::DescriptorPoolSize()
			.setType(vk::DescriptorType::eUniformBuffer)
			.setDescriptorCount(1),
		vk::DescriptorPoolSize()
			.setType(vk::DescriptorType::eCombinedImageSampler)
			.setDescriptorCount(1)
	};

	vk::DescriptorPoolCreateInfo poolInfo = vk::DescriptorPoolCreateInfo()
		.setPoolSizeCount(static_cast<uint32_t>(poolSizes.size()))
		.setPPoolSizes(poolSizes.data())
		.setMaxSets(1);

	if (_logicalDevice->getObject()->createDescriptorPool(&poolInfo, nullptr, &_descriptorPool) != vk::Result::eSuccess)
	{
		throw std::runtime_error("failed to create descriptor pool!");
	}
}

void VulkanApplication::createDescriptorSet()
{
	vk::DescriptorSetLayout layout[] = {*_descriptorSetLayout->getObject()};

	vk::DescriptorSetAllocateInfo allocInfo = vk::DescriptorSetAllocateInfo()
		.setDescriptorPool(_descriptorPool)
		.setDescriptorSetCount(1)
		.setPSetLayouts(layout);

	if (_logicalDevice->getObject()->allocateDescriptorSets(&allocInfo, &_descriptorSet) != vk::Result::eSuccess)
	{
		throw std::runtime_error("failed to allocate descriptor set!");
	}

	vk::DescriptorBufferInfo bufferInfo = vk::DescriptorBufferInfo()
		.setBuffer(_uniformBuffer)
		.setOffset(0)
		.setRange(sizeof(UniformBufferObject));

	vk::DescriptorImageInfo imageInfo = vk::DescriptorImageInfo()
		.setImageLayout(vk::ImageLayout::eShaderReadOnlyOptimal)
		.setImageView(*_imageView->getObject())
		.setSampler(*_imageView->getSampler());

	std::array<vk::WriteDescriptorSet, 2> descriptorWrites = {
		vk::WriteDescriptorSet()
			.setDstSet(_descriptorSet)
			.setDstBinding(0)
			.setDstArrayElement(0)
			.setDescriptorType(vk::DescriptorType::eUniformBuffer)
			.setDescriptorCount(1)
			.setPBufferInfo(&bufferInfo)
			.setPImageInfo(nullptr)
			.setPTexelBufferView(nullptr),
		vk::WriteDescriptorSet()
			.setDstSet(_descriptorSet)
			.setDstBinding(1)
			.setDstArrayElement(0)
			.setDescriptorType(vk::DescriptorType::eCombinedImageSampler)
			.setDescriptorCount(1)
			.setPBufferInfo(nullptr)
			.setPImageInfo(&imageInfo)
			.setPTexelBufferView(nullptr)
	};

	_logicalDevice->getObject()->updateDescriptorSets(static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
}

void VulkanApplication::createSemaphores()
{
	vk::SemaphoreCreateInfo semaphoreInfo = vk::SemaphoreCreateInfo();

	if (_logicalDevice->getObject()->createSemaphore(&semaphoreInfo, nullptr, &_imageAvailableSemaphore) != vk::Result::eSuccess ||
		_logicalDevice->getObject()->createSemaphore(&semaphoreInfo, nullptr, &_renderFinishedSemaphore) != vk::Result::eSuccess)
	{

		throw std::runtime_error("failed to create semaphores!");
	}
}

void VulkanApplication::transitionImageLayout(vk::Image image, vk::Format format, vk::ImageLayout oldLayout, vk::ImageLayout newLayout)
{
	litter::VulkanSingleTimeCommand singleCmd(_logicalDevice, _commandPool);

	vk::ImageMemoryBarrier barrier = vk::ImageMemoryBarrier()
		.setOldLayout(oldLayout)
		.setNewLayout(newLayout)
		.setSrcQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
		.setDstQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
		.setImage(image)
		.setSubresourceRange(
			vk::ImageSubresourceRange()
				.setAspectMask(vk::ImageAspectFlagBits::eColor)
				.setBaseMipLevel(0)
				.setLevelCount(1)
				.setBaseArrayLayer(0)
				.setLayerCount(1)
		);

	if (newLayout == vk::ImageLayout::eDepthStencilAttachmentOptimal)
	{
		barrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eDepth;

		if (hasStencilComponent(format))
		{
			barrier.subresourceRange.aspectMask |= vk::ImageAspectFlagBits::eStencil;
		}
	}
	else
	{
		barrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
	}

	if (oldLayout == vk::ImageLayout::ePreinitialized && newLayout == vk::ImageLayout::eTransferSrcOptimal)
	{
		barrier.srcAccessMask = vk::AccessFlagBits::eHostWrite;
		barrier.dstAccessMask = vk::AccessFlagBits::eTransferRead;
	}
	else if (oldLayout == vk::ImageLayout::ePreinitialized && newLayout == vk::ImageLayout::eTransferDstOptimal)
	{
		barrier.srcAccessMask = vk::AccessFlagBits::eHostWrite;
		barrier.dstAccessMask = vk::AccessFlagBits::eTransferWrite;
	}
	else if (oldLayout == vk::ImageLayout::eTransferDstOptimal && newLayout == vk::ImageLayout::eShaderReadOnlyOptimal)
	{
		barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
		barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;
	}
	else if (oldLayout == vk::ImageLayout::eUndefined && newLayout == vk::ImageLayout::eDepthStencilAttachmentOptimal)
	{
		barrier.srcAccessMask = vk::AccessFlags();
		barrier.dstAccessMask = vk::AccessFlagBits::eDepthStencilAttachmentRead | vk::AccessFlagBits::eDepthStencilAttachmentWrite;
	}
	else
	{
		throw std::invalid_argument("unsupported layout transition!");
	}

	singleCmd.getObject()->pipelineBarrier(vk::PipelineStageFlagBits::eTopOfPipe, vk::PipelineStageFlagBits::eTopOfPipe, vk::DependencyFlags(),
		0, nullptr,
		0, nullptr,
		1, &barrier);
}

bool VulkanApplication::hasStencilComponent(vk::Format format)
{
	return format == vk::Format::eD32SfloatS8Uint || format == vk::Format::eD24UnormS8Uint;
}

void VulkanApplication::createBuffer(vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties, vk::Buffer& buffer, vk::DeviceMemory& bufferMemory)
{
	vk::BufferCreateInfo bufferInfo = vk::BufferCreateInfo()
		.setSize(size)
		.setUsage(usage)
		.setSharingMode(vk::SharingMode::eExclusive);

	if (_logicalDevice->getObject()->createBuffer(&bufferInfo, nullptr, &buffer) != vk::Result::eSuccess)
	{
		throw std::runtime_error("failed to create vertex buffer!");
	}

	vk::MemoryRequirements memRequirements;
	_logicalDevice->getObject()->getBufferMemoryRequirements(buffer, &memRequirements);

	vk::MemoryAllocateInfo allocInfo = vk::MemoryAllocateInfo()
		.setAllocationSize(memRequirements.size)
		.setMemoryTypeIndex(findMemoryType(memRequirements.memoryTypeBits, properties));

	if (_logicalDevice->getObject()->allocateMemory(&allocInfo, nullptr, &bufferMemory) != vk::Result::eSuccess)
	{
		throw std::runtime_error("failed to allocate vertex buffer memory!");
	}

	_logicalDevice->getObject()->bindBufferMemory(buffer, bufferMemory, 0);
}

void VulkanApplication::copyBuffer(vk::Buffer srcBuffer, vk::Buffer dstBuffer, vk::DeviceSize size)
{
	litter::VulkanSingleTimeCommand singleCmd(_logicalDevice, _commandPool);

	vk::BufferCopy copyRegion = vk::BufferCopy()
		.setSrcOffset(0)
		.setDstOffset(0)
		.setSize(size);
	copyRegion.size = size;
	singleCmd.getObject()->copyBuffer(srcBuffer, dstBuffer, 1, &copyRegion);
}

uint32_t VulkanApplication::findMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties)
{
	vk::PhysicalDeviceMemoryProperties memProperties;
	_physicalDevice->getObject()->getMemoryProperties(&memProperties);

	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
	{
		if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
		{
			return i;
		}
	}

	throw std::runtime_error("failed to find suitable memory type!");
}

VKAPI_ATTR VkBool32 VKAPI_CALL VulkanApplication::debugCallback(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objType, uint64_t obj, size_t location, int32_t code, const char* layerPrefix, const char* msg, void* userData) {
	std::cerr << "validation layer: " << msg << std::endl;

	return VK_FALSE;
}

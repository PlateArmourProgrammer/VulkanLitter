#include "VulkanCommandBuffers.h"
#include "VulkanLogicalDevice.h"
#include "VulkanCommandPool.h"
#include "VulkanFramebufferPool.h"
#include "VulkanRenderPass.h"
#include "VulkanPipeline.h"
#include "VulkanSwapChain.h"
#include "RenderCommand/TextureRenderCmd.h"

namespace litter {
	VulkanCommandBuffers::VulkanCommandBuffers(VulkanLogicalDevice* logicalDevice, VulkanCommandPool* commandPool,
		VulkanFramebufferPool* framebufferPool, VulkanRenderPass* renderPass, VulkanPipeline* pipeline,
		VulkanSwapChain* swapChain, vk::DescriptorSet* descriptorSet, TextureRenderCmd* renderCmd) {
		_logicalDevice = logicalDevice;
		_commandPool = commandPool;

		init(framebufferPool,  renderPass, pipeline, swapChain, descriptorSet, renderCmd);
	}

	VulkanCommandBuffers::~VulkanCommandBuffers() {
		cleanup();
	}

	void VulkanCommandBuffers::init(VulkanFramebufferPool* framebufferPool, VulkanRenderPass* renderPass, VulkanPipeline* pipeline,
		VulkanSwapChain* swapChain, vk::DescriptorSet* descriptorSet, TextureRenderCmd* renderCmd) {
		_commandBuffers.resize(framebufferPool->getFramebufferCount());

		vk::CommandBufferAllocateInfo allocInfo = vk::CommandBufferAllocateInfo();
		allocInfo.commandPool = *_commandPool->getObject();
		allocInfo.level = vk::CommandBufferLevel::ePrimary;
		allocInfo.commandBufferCount = (uint32_t)_commandBuffers.size();

		if (_logicalDevice->getObject()->allocateCommandBuffers(&allocInfo, _commandBuffers.data()) != vk::Result::eSuccess) {
			throw std::runtime_error("failed to allocate command buffers!");
		}

		for (size_t i = 0; i < _commandBuffers.size(); i++) {
			vk::CommandBufferBeginInfo beginInfo = vk::CommandBufferBeginInfo();
			beginInfo.flags = vk::CommandBufferUsageFlagBits::eSimultaneousUse;

			_commandBuffers[i].begin(&beginInfo);

			vk::RenderPassBeginInfo renderPassInfo = vk::RenderPassBeginInfo();
			renderPassInfo.renderPass = *renderPass->getObject();
			renderPassInfo.framebuffer = *framebufferPool->getFramebufferAt(i);
			renderPassInfo.renderArea.offset = { 0, 0 };
			renderPassInfo.renderArea.extent = *swapChain->getExtent();

			std::array<float, 4> clearColorValue = { 0.0f, 0.0f, 0.0f, 1.0f };

			std::array<vk::ClearValue, 2> clearValues = {
				vk::ClearValue()
				.setColor(vk::ClearColorValue(clearColorValue)),
				vk::ClearValue()
				.setDepthStencil(vk::ClearDepthStencilValue(1.0f, 0))
			};
			renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
			renderPassInfo.pClearValues = clearValues.data();

			_commandBuffers[i].beginRenderPass(&renderPassInfo, vk::SubpassContents::eInline);

			_commandBuffers[i].bindPipeline(vk::PipelineBindPoint::eGraphics, *pipeline->getObject());

			vk::Buffer vertexBuffers[] = { *renderCmd->getVertexBuffer() };
			VkDeviceSize offsets[] = { 0 };
			_commandBuffers[i].bindVertexBuffers(0, 1, vertexBuffers, offsets);
			_commandBuffers[i].bindIndexBuffer(*renderCmd->getIndexBuffer(), 0, vk::IndexType::eUint32);

			_commandBuffers[i].bindDescriptorSets(vk::PipelineBindPoint::eGraphics, *pipeline->getPiprlineLayout(), 0, 1, descriptorSet, 0, nullptr);

			_commandBuffers[i].drawIndexed((uint32_t)renderCmd->getIndexSize(), 1, 0, 0, 0);

			_commandBuffers[i].endRenderPass();

			_commandBuffers[i].end();
		}
	}

	void VulkanCommandBuffers::cleanup() {
		_logicalDevice->getObject()->freeCommandBuffers(*_commandPool->getObject(), static_cast<uint32_t>(_commandBuffers.size()), _commandBuffers.data());
	}

	vk::CommandBuffer* VulkanCommandBuffers::getBufferAt(size_t idx) {
		return &(_commandBuffers[idx]);
	}
}
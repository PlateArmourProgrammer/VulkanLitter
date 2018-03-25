#include "VulkanRenderPass.h"
#include "VulkanLogicalDevice.h"
#include "VulkanPhysicalDevice.h"

namespace litter {
	VulkanRenderPass::VulkanRenderPass(VulkanLogicalDevice* logicalDevice, vk::Format* imageFormat, VulkanPhysicalDevice* physicalDevice) {
		_logicalDevice = logicalDevice;

		init(imageFormat, physicalDevice);
	}

	VulkanRenderPass::~VulkanRenderPass() {
		cleanup();
	}

	void VulkanRenderPass::init(vk::Format* imageFormat, VulkanPhysicalDevice* physicalDevice) {
		vk::AttachmentDescription colorAttachment = vk::AttachmentDescription()
			.setFormat(*imageFormat)
			.setSamples(vk::SampleCountFlagBits::e1)
			.setLoadOp(vk::AttachmentLoadOp::eClear)
			.setStoreOp(vk::AttachmentStoreOp::eStore)
			.setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
			.setStencilStoreOp(vk::AttachmentStoreOp::eDontCare)
			.setInitialLayout(vk::ImageLayout::eUndefined)
			.setFinalLayout(vk::ImageLayout::ePresentSrcKHR);

		vk::AttachmentDescription depthAttachment = vk::AttachmentDescription()
			.setFormat(*physicalDevice->getDepthFormat())
			.setSamples(vk::SampleCountFlagBits::e1)
			.setLoadOp(vk::AttachmentLoadOp::eClear)
			.setStoreOp(vk::AttachmentStoreOp::eDontCare)
			.setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
			.setStencilStoreOp(vk::AttachmentStoreOp::eDontCare)
			.setInitialLayout(vk::ImageLayout::eUndefined)
			.setFinalLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal);

		vk::AttachmentReference colorAttachmentRef = vk::AttachmentReference()
			.setAttachment(0)
			.setLayout(vk::ImageLayout::eColorAttachmentOptimal);

		vk::AttachmentReference depthAttachmentRef = vk::AttachmentReference()
			.setAttachment(1)
			.setLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal);

		vk::SubpassDescription subpass = vk::SubpassDescription()
			.setPipelineBindPoint(vk::PipelineBindPoint::eGraphics)
			.setColorAttachmentCount(1)
			.setPColorAttachments(&colorAttachmentRef)
			.setPDepthStencilAttachment(&depthAttachmentRef);

		vk::SubpassDependency dependency = vk::SubpassDependency()
			.setSrcSubpass(VK_SUBPASS_EXTERNAL)
			.setDstSubpass(0)
			.setSrcStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput)
			.setDstStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput)
			.setDstAccessMask(vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentWrite);

		std::array<vk::AttachmentDescription, 2> attachments = { colorAttachment, depthAttachment };

		vk::RenderPassCreateInfo renderPassInfo = vk::RenderPassCreateInfo()
			.setAttachmentCount(static_cast<uint32_t>(attachments.size()))
			.setPAttachments(attachments.data())
			.setSubpassCount(1)
			.setPSubpasses(&subpass)
			.setDependencyCount(1)
			.setPDependencies(&dependency);

		if (_logicalDevice->getObject()->createRenderPass(&renderPassInfo, nullptr, &_renderPass) != vk::Result::eSuccess) {
			throw std::runtime_error("failed to create render pass!");
		}
	}

	void VulkanRenderPass::cleanup() {
		_logicalDevice->getObject()->destroyRenderPass(_renderPass, nullptr);
	}

	vk::RenderPass* VulkanRenderPass::getObject() {
		return &_renderPass;
	}
}

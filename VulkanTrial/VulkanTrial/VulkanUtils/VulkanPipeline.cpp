#include "VulkanPipeline.h"
#include "VulkanLogicalDevice.h"
#include "VulkanRenderPass.h"
#include "VulkanSwapChain.h"
#include "VulkanDescriptorSetLayout.h"
#include "File/File.h"

namespace litter {
	VulkanPipeline::VulkanPipeline(VulkanLogicalDevice* logicalDevice,
		VulkanSwapChain* swapChain, VulkanDescriptorSetLayout* descriptorSetLayout, VulkanRenderPass* renderPass) {
		_logicalDevice = logicalDevice;

		init(swapChain, descriptorSetLayout, renderPass);
	}

	VulkanPipeline::~VulkanPipeline() {
		cleanup();
	}

	void VulkanPipeline::init(VulkanSwapChain* swapChain, VulkanDescriptorSetLayout* descriptorSetLayout, VulkanRenderPass* renderPass) {
		// todo: remove shader stuffs
		auto vertShaderCode = File::readFile("shaders/vert.spv");
		auto fragShaderCode = File::readFile("shaders/frag.spv");

		vk::ShaderModule vertShaderModule = createShaderModule(vertShaderCode);
		vk::ShaderModule fragShaderModule = createShaderModule(fragShaderCode);

		vk::PipelineShaderStageCreateInfo vertShaderStageInfo = vk::PipelineShaderStageCreateInfo()
			.setStage(vk::ShaderStageFlagBits::eVertex)
			.setModule(vertShaderModule)
			.setPName("main");

		vk::PipelineShaderStageCreateInfo fragShaderStageInfo = vk::PipelineShaderStageCreateInfo()
			.setStage(vk::ShaderStageFlagBits::eFragment)
			.setModule(fragShaderModule)
			.setPName("main");

		vk::PipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

		vk::PipelineVertexInputStateCreateInfo vertexInputInfo = vk::PipelineVertexInputStateCreateInfo()
			.setVertexBindingDescriptionCount(0)
			.setVertexAttributeDescriptionCount(0);

		vk::VertexInputBindingDescription bindingDescription = {};
		bindingDescription.binding = 0;
		bindingDescription.stride = sizeof(float) * 5;
		bindingDescription.inputRate = vk::VertexInputRate::eVertex;

		std::array<vk::VertexInputAttributeDescription, 2> attributeDescriptions = {};
		attributeDescriptions[0].binding = 0;
		attributeDescriptions[0].location = 0;
		attributeDescriptions[0].format = vk::Format::eR32G32B32Sfloat;
		attributeDescriptions[0].offset = 0;

		attributeDescriptions[1].binding = 0;
		attributeDescriptions[1].location = 1;
		attributeDescriptions[1].format = vk::Format::eR32G32B32Sfloat;
		attributeDescriptions[1].offset = sizeof(float) * 3;

		vertexInputInfo.vertexBindingDescriptionCount = 1;
		vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
		vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
		vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

		vk::PipelineInputAssemblyStateCreateInfo inputAssembly = vk::PipelineInputAssemblyStateCreateInfo()
			.setTopology(vk::PrimitiveTopology::eTriangleList)
			.setPrimitiveRestartEnable(VK_FALSE);

		vk::Viewport viewport = vk::Viewport()
			.setX(0.0f)
			.setY(0.0f)
			.setWidth((float)swapChain->getExtentWidth())
			.setHeight((float)swapChain->getExtentHeight())
			.setMinDepth(0.0f)
			.setMaxDepth(1.0f);

		vk::Rect2D scissor = vk::Rect2D()
			.setOffset(vk::Offset2D()
				.setX(0)
				.setY(0))
			.setExtent(*swapChain->getExtent());

		vk::PipelineViewportStateCreateInfo viewportState = vk::PipelineViewportStateCreateInfo()
			.setViewportCount(1)
			.setPViewports(&viewport)
			.setScissorCount(1)
			.setPScissors(&scissor);

		vk::PipelineRasterizationStateCreateInfo rasterizer = vk::PipelineRasterizationStateCreateInfo()
			.setDepthClampEnable(VK_FALSE)
			.setRasterizerDiscardEnable(VK_FALSE)
			.setPolygonMode(vk::PolygonMode::eFill)
			.setLineWidth(1.0f)
			.setCullMode(vk::CullModeFlagBits::eBack)
			.setFrontFace(vk::FrontFace::eCounterClockwise)
			.setDepthBiasEnable(VK_FALSE);

		vk::PipelineMultisampleStateCreateInfo multisampling = vk::PipelineMultisampleStateCreateInfo()
			.setSampleShadingEnable(VK_FALSE)
			.setRasterizationSamples(vk::SampleCountFlagBits::e1);

		vk::PipelineDepthStencilStateCreateInfo depthStencil = vk::PipelineDepthStencilStateCreateInfo()
			.setDepthTestEnable(VK_TRUE)
			.setDepthWriteEnable(VK_TRUE)
			.setDepthCompareOp(vk::CompareOp::eLess)
			.setDepthBoundsTestEnable(VK_FALSE)
			.setStencilTestEnable(VK_FALSE);

		vk::PipelineColorBlendAttachmentState colorBlendAttachment = vk::PipelineColorBlendAttachmentState()
			.setColorWriteMask(vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA)
			.setBlendEnable(VK_FALSE);

		vk::PipelineColorBlendStateCreateInfo colorBlending = vk::PipelineColorBlendStateCreateInfo()
			.setLogicOpEnable(VK_FALSE)
			.setLogicOp(vk::LogicOp::eCopy)
			.setAttachmentCount(1)
			.setPAttachments(&colorBlendAttachment);
		colorBlending.blendConstants[0] = 0.0f;
		colorBlending.blendConstants[1] = 0.0f;
		colorBlending.blendConstants[2] = 0.0f;
		colorBlending.blendConstants[3] = 0.0f;

		vk::PipelineLayoutCreateInfo pipelineLayoutInfo = vk::PipelineLayoutCreateInfo()
			.setSetLayoutCount(1)
			.setPSetLayouts(descriptorSetLayout->getObject());

		vk::Device* vkDevice = _logicalDevice->getObject();
		if (vkDevice->createPipelineLayout(&pipelineLayoutInfo, nullptr, &_pipelineLayout) != vk::Result::eSuccess)
		{
			throw std::runtime_error("failed to create pipeline layout!");
		}

		vk::GraphicsPipelineCreateInfo pipelineInfo = vk::GraphicsPipelineCreateInfo();
		pipelineInfo.stageCount = 2;
		pipelineInfo.pStages = shaderStages;
		pipelineInfo.pVertexInputState = &vertexInputInfo;
		pipelineInfo.pInputAssemblyState = &inputAssembly;
		pipelineInfo.pViewportState = &viewportState;
		pipelineInfo.pRasterizationState = &rasterizer;
		pipelineInfo.pMultisampleState = &multisampling;
		pipelineInfo.pColorBlendState = &colorBlending;
		pipelineInfo.layout = _pipelineLayout;
		pipelineInfo.renderPass = *renderPass->getObject();
		pipelineInfo.subpass = 0;
		pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
		pipelineInfo.pDepthStencilState = &depthStencil;

		if (vkDevice->createGraphicsPipelines(VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &_pipeline) != vk::Result::eSuccess)
		{
			throw std::runtime_error("failed to create graphics pipeline!");
		}

		vkDevice->destroyShaderModule(fragShaderModule, nullptr);
		vkDevice->destroyShaderModule(vertShaderModule, nullptr);
	}

	void VulkanPipeline::cleanup() {
		vk::Device* vkDevice = _logicalDevice->getObject();
		vkDevice->destroyPipeline(_pipeline, nullptr);
		vkDevice->destroyPipelineLayout(_pipelineLayout, nullptr);
	}

	vk::Pipeline* VulkanPipeline::getObject() {
		return &_pipeline;
	}

	vk::PipelineLayout* VulkanPipeline::getPiprlineLayout() {
		return &_pipelineLayout;
	}

	vk::ShaderModule VulkanPipeline::createShaderModule(const std::vector<char>& code)
	{
		vk::ShaderModuleCreateInfo createInfo = vk::ShaderModuleCreateInfo()
			.setCodeSize(code.size());

		std::vector<uint32_t> codeAligned(code.size() / 4 + 1);
		memcpy(codeAligned.data(), code.data(), code.size());

		createInfo.pCode = codeAligned.data();

		vk::ShaderModule shaderModule;
		if (_logicalDevice->getObject()->createShaderModule(&createInfo, nullptr, &shaderModule) != vk::Result::eSuccess)
		{
			throw std::runtime_error("failed to create shader module!");
		}

		return shaderModule;
	}
}

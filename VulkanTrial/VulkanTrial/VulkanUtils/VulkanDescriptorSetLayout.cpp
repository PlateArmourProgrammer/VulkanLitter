#include "VulkanDescriptorSetLayout.h"
#include "VulkanLogicalDevice.h"

namespace litter {
	VulkanDescriptorSetLayout::VulkanDescriptorSetLayout(VulkanLogicalDevice* logicalDevice) {
		_logicalDevice = logicalDevice;

		vk::DescriptorSetLayoutBinding uboLayoutBinding = vk::DescriptorSetLayoutBinding()
			.setBinding(0)
			.setDescriptorType(vk::DescriptorType::eUniformBuffer)
			.setDescriptorCount(1)
			.setStageFlags(vk::ShaderStageFlagBits::eVertex)
			.setPImmutableSamplers(nullptr);

		vk::DescriptorSetLayoutBinding samplerLayoutBinding = vk::DescriptorSetLayoutBinding()
			.setBinding(1)
			.setDescriptorType(vk::DescriptorType::eCombinedImageSampler)
			.setDescriptorCount(1)
			.setStageFlags(vk::ShaderStageFlagBits::eFragment)
			.setPImmutableSamplers(nullptr);

		std::array<vk::DescriptorSetLayoutBinding, 2> bindings = { uboLayoutBinding, samplerLayoutBinding };

		vk::DescriptorSetLayoutCreateInfo layoutInfo = vk::DescriptorSetLayoutCreateInfo()
			.setBindingCount(static_cast<uint32_t>(bindings.size()))
			.setPBindings(bindings.data());

		if (_logicalDevice->getObject()->createDescriptorSetLayout(&layoutInfo, nullptr, &_layout) != vk::Result::eSuccess) {
			throw std::runtime_error("failed to create descriptor set layout!");
		}
	}

	VulkanDescriptorSetLayout::~VulkanDescriptorSetLayout() {
		_logicalDevice->getObject()->destroyDescriptorSetLayout(_layout, nullptr);
	}

	vk::DescriptorSetLayout* VulkanDescriptorSetLayout::getObject() {
		return &_layout;
	}
}

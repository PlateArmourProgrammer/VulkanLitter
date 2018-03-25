#ifndef VulkanInstance_h_
#define VulkanInstance_h_

#include "Base/BaseObject.h"
#include "VulkanHeader.h"

namespace litter {
	class VulkanInstance : public BaseObject {
	public:
		VulkanInstance();
		~VulkanInstance();

		vk::Instance* getObject();
	private:
		bool checkValidationLayerSupport();
		std::vector<const char*> getAvailableWSIExtensions();

	private:
		vk::Instance _vkInstance;
	};
}

#endif // !VulkanInstance_h_


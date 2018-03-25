#include "VulkanUtils\VulkanApplication.h"
#include "StdC.h"

int main() {

	/*const std::vector<Vertex> vertices = {
		{ { 0.0f, -0.5f },{ 1.0f, 0.0f, 0.0f } },
		{ { 0.5f, 0.5f },{ 0.0f, 1.0f, 0.0f } },
		{ { -0.5f, 0.5f },{ 0.0f, 0.0f, 1.0f } }
	};*/

	VulkanApplication app;

	try {
		app.init();
		app.run();
		app.cleanup();
	}
	catch (const std::runtime_error& e) {
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
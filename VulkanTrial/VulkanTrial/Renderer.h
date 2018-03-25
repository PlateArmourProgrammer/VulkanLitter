#ifndef RENDERER_H_
#define RENDERER_H_

#include "VulkanUtils\VulkanHeader.h"
#include <glm\glm.hpp>

struct UniformBufferObject
{
	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 proj;
};

class Renderer
{
public:
	Renderer();
	~Renderer();
};

#endif

#pragma once
#define GLFW_INCLUDE_VULKAN
#include <glfw/glfw3.h>
#include <vector>
#include <string>

/*
*	This class is responsible for creating and handling
*	graphics pipeline elements.
*	Shader Modules
*	Fixed functions
*	Render passes
*/

class VulkanGraphicsPipeline
{
public:
	VulkanGraphicsPipeline(VkDevice device);
	~VulkanGraphicsPipeline();
private:
	void CreateGraphicsPipeline();
	VkShaderModule createShaderModule(const std::vector<char>& code);

	static std::vector<char> ReadFile(const std::string& fileName);
	VkDevice m_device;
	VkPipelineLayout m_pipelineLayout;
};
#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vector>

class VulkanFrameBuffer
{
public:
	VulkanFrameBuffer(std::vector<VkImageView> imageViews, VkDevice device);
	~VulkanFrameBuffer();
private:
	void CreateFrameBuffer();

	std::vector<VkFramebuffer> m_swapChainFrameBuffers;
	std::vector<VkImageView> m_imageViews;
	VkDevice m_device;
};
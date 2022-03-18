#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vector>

class VulkanFrameBuffer
{
public:
	VulkanFrameBuffer(std::vector<VkImageView> imageViews);
	~VulkanFrameBuffer();
	std::vector<VkFramebuffer> GetSwapChainFrameBuffers();
	std::vector<VkImageView> GetImageViews();
private:
	void CreateFrameBuffer();

	std::vector<VkFramebuffer> m_swapChainFrameBuffers;
	std::vector<VkImageView> m_imageViews;
	//VkDevice m_device;
};
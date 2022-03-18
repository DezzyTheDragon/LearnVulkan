#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include "VulkanStructs.h"
#include "PhysicalDevice.h"
#include "VulkanFrameBuffer.h"
#include <vector>

class VulkanCommands
{
public:
	VulkanCommands(PhysicalDevice *physicalDevice, VulkanFrameBuffer *frameBuffer);
	~VulkanCommands();
	VkCommandPool GetCommandPool();
	VkCommandBuffer GetCommandBuffer();
	void RecordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);
private:
	void CreateCommandPool();
	void CreateCommandBuffer();
	PhysicalDevice* m_physicalDevice;
	VkCommandPool m_commandPool;
	VkCommandBuffer m_commandBuffer;
	VulkanFrameBuffer* m_frameBuffer;
};
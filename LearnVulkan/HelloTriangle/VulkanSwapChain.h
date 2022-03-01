#pragma once
#define GLFW_INCLUDE_VULKAN
#include <glfw/glfw3.h>
#include "VulkanStructs.h"

/*
*	This class is responsible for searching for swap
*	chain compatibility and creating the swapchain
*/

class VulkanSwapChain
{
public:
	VulkanSwapChain(VkSurfaceKHR surface);
	~VulkanSwapChain();
	SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);
private:
	VkSurfaceKHR m_surface;
};
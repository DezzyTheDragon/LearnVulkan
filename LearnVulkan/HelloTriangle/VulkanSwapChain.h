#pragma once
#define GLFW_INCLUDE_VULKAN
#include <glfw/glfw3.h>
#include "VulkanStructs.h"
#include <vector>

/*
*	This class is responsible for searching for swap
*	chain compatibility and creating the swapchain
*/

class VulkanSwapChain
{
public:
	VulkanSwapChain();
	~VulkanSwapChain();
	void createSwapChain(VkPhysicalDevice physicalDevice);
	VkSwapchainKHR GetSwapChain();
	VkFormat GetImageFormat();
	VkExtent2D GetExtent();
	std::vector<VkImage> GetSwapChainImages();
	//void SetLogicalDevice(VkDevice device);
	SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);
	VkSurfaceFormatKHR choseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
	VkPresentModeKHR choseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
	VkExtent2D choseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
private:
	QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device);
	VkSwapchainKHR m_swapChain;
	std::vector<VkImage> m_swapChainImages;
	VkFormat m_swapChainImageFormat;
	VkExtent2D m_swapChainExtent;
	//VkDevice m_device;
};
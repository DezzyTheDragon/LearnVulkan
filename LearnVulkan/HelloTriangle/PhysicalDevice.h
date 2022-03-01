#pragma once

#define GLFW_INCLUDE_VULKAN
#include <glfw/glfw3.h>
#include <vector>
#include "VulkanStructs.h"
#include "VulkanSwapChain.h"

/*	This class is responsible for handling searching the physical device
*	and finding a suitable device to use.
*	Also responsible for finding apporpriate queue families
* 
*	Will also set up the logical device for interfacing with
*	the physical device
*/



class PhysicalDevice
{
public:
	//Class c'tor, d'tor, etc
	PhysicalDevice(VkInstance inst, bool validation, std::vector<const char*> validationLayers, VkSurfaceKHR surface);
	~PhysicalDevice();
	//Class Getters
	VkPhysicalDevice GetPhysicalDevice();
	VkDevice GetLogicalDevice();
	VkQueue GetGraphicsQueue();

private:
	//Class functions
	void PickPhysicalDevice();
	void CreateLogicalDevice();
	bool isDeviceSuitable(VkPhysicalDevice device);
	bool CheckDeviceExtensions(VkPhysicalDevice device);
	QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device);
	//member variables
	VkInstance m_instance;
	VkSurfaceKHR m_surface;
	VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;
	VulkanSwapChain* m_swapChain;
	VkDevice m_logicalDevice;
	VkQueue m_graphicsQueue;
	VkQueue m_presentQueue;
	const std::vector<const char*> m_deviceExtensions = {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};

	bool m_validationEnabled;						//hold onto validation information for old vulkan compatibility
	std::vector<const char*> m_validationLayers;	//in createLogicalDevice
};
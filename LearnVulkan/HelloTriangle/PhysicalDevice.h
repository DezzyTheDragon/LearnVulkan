#pragma once

#define GLFW_INCLUDE_VULKAN
#include <glfw/glfw3.h>
#include <vector>
#include "VulkanStructs.h"

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
	PhysicalDevice(bool validation, std::vector<const char*> validationLayers);
	~PhysicalDevice();
	//Class Getters
	VkPhysicalDevice GetPhysicalDevice();
	//VkDevice GetLogicalDevice();
	VkQueue GetGraphicsQueue();
	QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device);

private:
	//Class functions
	void PickPhysicalDevice();
	void CreateLogicalDevice();
	bool isDeviceSuitable(VkPhysicalDevice device);
	bool CheckDeviceExtensions(VkPhysicalDevice device);
	//member variables
	VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;
	//VkDevice m_logicalDevice;
	VkQueue m_graphicsQueue;
	VkQueue m_presentQueue;
	const std::vector<const char*> m_deviceExtensions = {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};

	bool m_validationEnabled;						//hold onto validation information for old vulkan compatibility
	std::vector<const char*> m_validationLayers;	//in createLogicalDevice
};
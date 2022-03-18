#pragma once
#include "PhysicalDevice.h"
#include "VulkanSurface.h"
#include "VulkanSwapChain.h"
#include "VulkanGlobal.h"
#include <stdexcept>
#include <set>

VulkanSwapChain* g_vkSwapChain;
VkDevice g_device;

PhysicalDevice::PhysicalDevice(bool validation, std::vector<const char*> validationLayers)
{
	m_validationEnabled = validation;
	m_validationLayers = validationLayers;
	g_vkSwapChain = new VulkanSwapChain();
	PickPhysicalDevice();
	CreateLogicalDevice();
	//g_vkSwapChain->SetLogicalDevice(m_logicalDevice);
	g_vkSwapChain->createSwapChain(m_physicalDevice);
}

PhysicalDevice::~PhysicalDevice()
{
	delete g_vkSwapChain;
	vkDestroyDevice(g_device, nullptr);
}

//Public getter for the found physical device
VkPhysicalDevice PhysicalDevice::GetPhysicalDevice()
{
	return m_physicalDevice;
}

//VkDevice PhysicalDevice::GetLogicalDevice()
//{
//	return m_logicalDevice;
//}

VkQueue PhysicalDevice::GetGraphicsQueue()
{
	return m_graphicsQueue;
}

//Find the desired device
void PhysicalDevice::PickPhysicalDevice()
{
	uint32_t deviceCount = 0;
	//TODO: prefer enum class over enum
	vkEnumeratePhysicalDevices(g_vkInstance, &deviceCount, nullptr);

	//Check to make sure that there are devices that we can use
	if (deviceCount == 0)
	{
		throw std::runtime_error("PhysicalDevice: Failed to find any GPUs with Vulkan support");
	}


	//Create a list containing valid devices
	std::vector<VkPhysicalDevice> devices(deviceCount);
	vkEnumeratePhysicalDevices(g_vkInstance, &deviceCount, devices.data());

	//TODO: rank all the devices and select the best one
	
	//We only care about the first valid device
	for (const auto& device : devices)
	{
		if (isDeviceSuitable(device))
		{
			m_physicalDevice = device;
			break;
		}
	}

	if (m_physicalDevice == VK_NULL_HANDLE)
	{
		throw std::runtime_error("PhysicalDevice: Failed to find suitable GPU");
	}

}

//Create the logical device so that we can use the GPU
void PhysicalDevice::CreateLogicalDevice()
{
	QueueFamilyIndices indices = FindQueueFamilies(m_physicalDevice);
	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	std::set<uint32_t> uniqueQueueFamilies = {indices.graphicsFamily.value(), indices.presentFamily.value()};
	VkDeviceQueueCreateInfo queueCreateInfo{};
	VkPhysicalDeviceFeatures deviceFeatures{};
	VkDeviceCreateInfo createInfo{};
	float queuePriority = 1.0f;
	
	for (uint32_t queueFamily : uniqueQueueFamilies)
	{
		VkDeviceQueueCreateInfo queueCreateInfo{};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = queueFamily;
		queueCreateInfo.queueCount = 1;
		queueCreateInfo.pQueuePriorities = &queuePriority;
		queueCreateInfos.push_back(queueCreateInfo);
	}

	queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	queueCreateInfo.queueFamilyIndex = indices.graphicsFamily.value();
	queueCreateInfo.queueCount = 1;
	queueCreateInfo.pQueuePriorities = &queuePriority;

	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	createInfo.pQueueCreateInfos = queueCreateInfos.data();
	createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
	createInfo.pEnabledFeatures = &deviceFeatures;
	createInfo.enabledExtensionCount = static_cast<uint32_t>(m_deviceExtensions.size());
	createInfo.ppEnabledExtensionNames = m_deviceExtensions.data();

	//Current versions of Vulkan ignore enabledLayerCount and ppEnabledLayerNames
	//This is simply for compatibility for older versions of vulkan
	if (m_validationEnabled)
	{
		createInfo.enabledLayerCount = static_cast<uint32_t>(m_validationLayers.size());
		createInfo.ppEnabledLayerNames = m_validationLayers.data();
	}
	else
	{
		createInfo.enabledLayerCount = 0;
	}

	if (vkCreateDevice(m_physicalDevice, &createInfo, nullptr, &g_device) != VK_SUCCESS)
	{
		throw std::runtime_error("PhysicalDevice: Failed to create logical device");
	}

	vkGetDeviceQueue(g_device, indices.graphicsFamily.value(), 0, &m_graphicsQueue);
	vkGetDeviceQueue(g_device, indices.presentFamily.value(), 0, &m_presentQueue);
}

//Check if the given device has what we need
bool PhysicalDevice::isDeviceSuitable(VkPhysicalDevice device)
{
	QueueFamilyIndices indices = FindQueueFamilies(device);

	bool extensionsSupported = CheckDeviceExtensions(device);
	bool swapChainAdequate = false;
	if (extensionsSupported)
	{
		SwapChainSupportDetails swapChainSupport = g_vkSwapChain->querySwapChainSupport(device);
		swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
	}


	return indices.isComplete() && extensionsSupported && swapChainAdequate;
}

//Allows us to check for swapchain support
bool PhysicalDevice::CheckDeviceExtensions(VkPhysicalDevice device)
{
	uint32_t extensionCount;
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);
	std::vector<VkExtensionProperties> availableExtentions(extensionCount);
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtentions.data());

	//go through the available extensions and when we come across one in our list check it off
	std::set<std::string> requiredExtensions(m_deviceExtensions.begin(), m_deviceExtensions.end());
	for (const auto& extension : availableExtentions)
	{
		requiredExtensions.erase(extension.extensionName);
	}

	//if out list is empty then we have all the extensions we want
	return requiredExtensions.empty();
}

QueueFamilyIndices PhysicalDevice::FindQueueFamilies(VkPhysicalDevice device)
{
	QueueFamilyIndices indices;

	uint32_t familyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &familyCount, nullptr);
	std::vector<VkQueueFamilyProperties> queueFamilies(familyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &familyCount, queueFamilies.data());

	int i = 0;
	//Find the first queue family that has VK_QUEUE_GRAPHICS_BIT
	for (const auto& queueFamily : queueFamilies)
	{
		if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
		{
			indices.graphicsFamily = 1;
		}
		VkBool32 presentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(device, i, g_vkSurface->GetSurface(), &presentSupport);
		if (presentSupport)
		{
			indices.presentFamily = i;
		}

		if (indices.isComplete())
		{
			break;
		}
		i++;
	}

	return indices;
}

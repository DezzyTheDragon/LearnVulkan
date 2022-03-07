#include "VulkanSwapChain.h"
#include "VulkanSurface.h"
#include "VulkanGlobal.h"
#include <cstdint>
#include <limits>
#include <algorithm>
#include <stdexcept>


VulkanSwapChain::VulkanSwapChain()
{
	m_swapChain = VK_NULL_HANDLE;
}

VulkanSwapChain::~VulkanSwapChain()
{
	vkDestroySwapchainKHR(m_device, m_swapChain, nullptr);
}

void VulkanSwapChain::createSwapChain(VkPhysicalDevice physicalDevice)
{
	SwapChainSupportDetails swapChainSupport = querySwapChainSupport(physicalDevice);
	VkSurfaceFormatKHR surfaceFormat = choseSwapSurfaceFormat(swapChainSupport.formats);
	VkPresentModeKHR presentMode = choseSwapPresentMode(swapChainSupport.presentModes);
	VkExtent2D extent = choseSwapExtent(swapChainSupport.capabilities);

	m_swapChainImageFormat = surfaceFormat.format;
	m_swapChainExtent = extent;

	//Vulkan requires specified image minimum swapChainSupport and to avoid 
	//wating for the driver we request one more than the minimum
	uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
	//check that we do not exceed the max
	//0 indicates that there is no max
	if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount)
	{
		imageCount = swapChainSupport.capabilities.maxImageCount;
	}

	VkSwapchainCreateInfoKHR createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = g_vkSurface->GetSurface();
	createInfo.minImageCount = imageCount;
	createInfo.imageFormat = surfaceFormat.format;
	createInfo.imageColorSpace = surfaceFormat.colorSpace;
	createInfo.imageExtent = extent;
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	QueueFamilyIndices indices = FindQueueFamilies(physicalDevice);
	uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value(), indices.presentFamily.value() };

	//if we have multiple queue families we need to specify how they are shared
	if (indices.graphicsFamily != indices.presentFamily)
	{
		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices = queueFamilyIndices;
	}
	else
	{
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	}
	createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	createInfo.presentMode = presentMode;
	createInfo.clipped = VK_TRUE;
	createInfo.oldSwapchain = VK_NULL_HANDLE;

	if (vkCreateSwapchainKHR(m_device, &createInfo, nullptr, &m_swapChain) != VK_SUCCESS)
	{
		throw std::runtime_error("VulkanSwapChain: Failed to create swap chain");
	}

	//Get the hadles for the VkImages
	vkGetSwapchainImagesKHR(m_device, m_swapChain, &imageCount, nullptr);
	m_swapChainImages.resize(imageCount);
	vkGetSwapchainImagesKHR(m_device, m_swapChain, &imageCount, m_swapChainImages.data());
}

VkSwapchainKHR VulkanSwapChain::GetSwapChain()
{
	return m_swapChain;
}

VkFormat VulkanSwapChain::GetImageFormat()
{
	return m_swapChainImageFormat;
}

VkExtent2D VulkanSwapChain::GetExtent()
{
	return m_swapChainExtent;
}

std::vector<VkImage> VulkanSwapChain::GetSwapChainImages()
{
	return m_swapChainImages;
}

void VulkanSwapChain::SetLogicalDevice(VkDevice device)
{
	m_device = device;
}

//search for swapchain support
SwapChainSupportDetails VulkanSwapChain::querySwapChainSupport(VkPhysicalDevice device)
{
	SwapChainSupportDetails details;
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, g_vkSurface->GetSurface(), &details.capabilities);
	uint32_t formatCount;
	uint32_t presentModeCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(device, g_vkSurface->GetSurface(), &formatCount, nullptr);
	vkGetPhysicalDeviceSurfacePresentModesKHR(device, g_vkSurface->GetSurface(), &presentModeCount, nullptr);
	if (formatCount != 0)
	{
		details.formats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, g_vkSurface->GetSurface(), &formatCount, details.formats.data());
	}
	if (presentModeCount != 0)
	{
		details.presentModes.resize(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, g_vkSurface->GetSurface(), &presentModeCount, details.presentModes.data());
	}
	return details;
}


//Need to find optimal settings for the surface
//surface format	| color depth
//presentation mode	| conditions for swaping images
//Swap extent		| resolution of the images in the swap chain

//Find desired surface format finding ideal format, presentation mode, and swap extent
VkSurfaceFormatKHR VulkanSwapChain::choseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
{
	//The surface format specifies the settings for the surface
	for (const auto& availableFormat : availableFormats)
	{
		//prefered surface format
		if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB
			&& availableFormat.colorSpace == VK_COLORSPACE_SRGB_NONLINEAR_KHR)
		{
			return availableFormat;
		}
	}
	//all else fails just return the first thing in the list
	return availableFormats[0];
}

//Find the desired presentation mode
VkPresentModeKHR VulkanSwapChain::choseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes)
{
	//present mode represents the conditions for showing images on the screen

	for (const auto& availablePresentMode : availablePresentModes)
	{
		if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
		{
			return availablePresentMode;
		}
	}

	//FIFO is garenteed to be implimented so return it if all else fails
	return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D VulkanSwapChain::choseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities)
{
	/*	GLFW uses pixelsand screen coordinates, but vulkan focuses in just pixels
	*	This can cause a problem if the pixel density is really high because the
	*	screen coordinates will not line up with the pixels so we need to get the
	*	actual resolution before creating the swap chain
	*/
	if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
	{
		return capabilities.currentExtent;
	}
	else
	{
		int width, height;
		glfwGetFramebufferSize(g_window, &width, &height);

		VkExtent2D actualExtent = {
			static_cast<uint32_t>(width),
			static_cast<uint32_t>(height)
		};

		actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
		actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

		return actualExtent;
	}
}

//NOTE: This is a duplicate function
//		Origonal is in PhysicalDevice
QueueFamilyIndices VulkanSwapChain::FindQueueFamilies(VkPhysicalDevice device)
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

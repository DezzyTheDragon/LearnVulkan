#include "HelloTriangle.h"
#include <stdexcept>
#include <iostream>
#include <map>
#include <set>
#include <cstdint>
#include <algorithm>
#include <fstream>

//VkSurfaceKHR m_surface;

//Run sequence for the program
void HelloTriangle::run()
{
	initWindow();
	initVulkan();
	mainLoop();
	cleanUp();
}

//Uses glfw to create a window
void HelloTriangle::initWindow()
{
	glfwInit();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	m_window = glfwCreateWindow(m_WIDTH, m_HEIGHT, "Vulkan", nullptr, nullptr);
	glfwSetWindowUserPointer(m_window, this);
	glfwSetFramebufferSizeCallback(m_window, frameBufferSizeCallback);
}

//Initialize Vulkan
void HelloTriangle::initVulkan()
{
	createInstance();
	setupDebugMessenger();
	createSurface();
	pickPhysicalDevice();
	createLogicalDevice();
	createSwapChain();
	createImageViews();
	createRenderPass();
	createGraphicsPipeline();
	createFrameBuffer();
	createCommandPool();
	createCommandBuffers();
	createSyncObjects();
}

void HelloTriangle::createSurface()
{
	//VkSurfaceKHR surface;

	if (glfwCreateWindowSurface(m_instance, m_window, nullptr, &m_surface) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create windows surface");
	}
}

//configure the types of messeges we get back from validation layers
void HelloTriangle::setupDebugMessenger()
{
	//Only setup the debug manager if the validation layers are enabled
	//Dont need a debug messenger for a release build
	if (!m_enableValidationLayers)
	{
		return;
	}


	VkDebugUtilsMessengerCreateInfoEXT createInfo;
	populateDebugMessengerCreateInfo(createInfo);

	/*createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | 
								 VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
								 VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | 
							 VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | 
							 VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	createInfo.pfnUserCallback = debugCallback;
	createInfo.pUserData = nullptr;
	*/

	//TODO: Figure out how to either scope the VkResult or change the enum class to an enum
	if (CreateDebugUtilsMessegerEXT(m_instance, &createInfo, nullptr, &m_debugMessager) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to setup debug messenger!");
	}

}

//The loop that keeps the program running until the window terminates
void HelloTriangle::mainLoop()
{
	while (!glfwWindowShouldClose(m_window))
	{
		glfwPollEvents();
		drawFrame();
	}

	//Wait for the device to finish processes before cleaning things up
	//to avoid removing things still in use
	vkDeviceWaitIdle(m_device);
}

//Removes the things initalized
void HelloTriangle::cleanUp()
{
	cleanupSwapchain();

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
	{
		vkDestroySemaphore(m_device, m_renderFinishedSemaphores[i], nullptr);
		vkDestroySemaphore(m_device, m_imageAvailableSemaphores[i], nullptr);
		vkDestroyFence(m_device, m_inFlightFences[i], nullptr);
	}
	
	vkDestroyCommandPool(m_device, m_commandPool, nullptr);
	
	vkDestroyDevice(m_device, nullptr);
	
	if (m_enableValidationLayers)
	{
		DestroyDebugUtilsMessegerEXT(m_instance, m_debugMessager, nullptr);
	}

	vkDestroySurfaceKHR(m_instance, m_surface, nullptr);
	vkDestroyInstance(m_instance, nullptr);
	glfwDestroyWindow(m_window);
	glfwTerminate();

	/*for (auto framebuffer : m_swapChainFramebuffers)
	{
		vkDestroyFramebuffer(m_device, framebuffer, nullptr);
	}*/

	/*for (auto imageView : m_swapChainImageViews)
	{
		vkDestroyImageView(m_device, imageView, nullptr);
	}*/



	//vkDestroyPipeline(m_device, m_graphicsPipeline, nullptr);
	//vkDestroyPipelineLayout(m_device, m_pipelineLayout, nullptr);
	//vkDestroyRenderPass(m_device, m_renderPass, nullptr);

	//vkDestroySwapchainKHR(m_device, m_swapChain, nullptr);


}

//Create a vulkan Instance
void HelloTriangle::createInstance()
{
	if (m_enableValidationLayers && !checkValidationLayerSupport())
	{
		throw std::runtime_error("Validation layers requested are not available");
	}

	//Set app information, optional
	VkApplicationInfo appInfo{};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "Hello Triangle";
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "No Engine";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_0;

	//Set global and validation layers, not optional
	VkInstanceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;

	//We are using glfw so we need to get the glfw extensions
	uint32_t glfwExtentionCount = 0;
	const char** glfwExtensions;

	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtentionCount);

	createInfo.enabledExtensionCount = glfwExtentionCount;
	createInfo.ppEnabledExtensionNames = glfwExtensions;
	createInfo.enabledLayerCount = 0;

	VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
	//Include validation layer names
	if (m_enableValidationLayers)
	{
		createInfo.enabledLayerCount = static_cast<uint32_t>(m_validationLayers.size());
		createInfo.ppEnabledLayerNames = m_validationLayers.data();

		populateDebugMessengerCreateInfo(debugCreateInfo);
		createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*) &debugCreateInfo;
	}
	else
	{
		createInfo.enabledLayerCount = 0;

		createInfo.pNext = nullptr;	
	}

	auto ext = getRequiredExtensions();
	createInfo.enabledExtensionCount = static_cast<uint32_t>(ext.size());
	createInfo.ppEnabledExtensionNames = ext.data();

	//check extensions
	uint32_t extensionCount = 0;
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
	std::vector<VkExtensionProperties> extensions(extensionCount);
	
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

	//TODO: why is this important? result is not used anywhere, is it needed?
	VkResult result = vkCreateInstance(&createInfo, nullptr, &m_instance);

	if (vkCreateInstance(&createInfo, nullptr, &m_instance))
	{
		throw std::runtime_error("Failed to create instance");
	}
}

//Checks to make sure that the needed validation layers are there
bool HelloTriangle::checkValidationLayerSupport()
{
	uint32_t layerCount;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

	std::vector<VkLayerProperties> availableLayers(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

	//check to make sure all the nessesary layers exist
	for (const char* layerName : m_validationLayers)
	{
		bool layerFound = false;

		for (const auto& layerProperties : availableLayers)
		{
			if (strcmp(layerName, layerProperties.layerName) == 0)
			{
				layerFound = true;
				break;
			}
		}
		if (!layerFound)
		{
			return false;
		}
	}

	return true;
}

void HelloTriangle::frameBufferSizeCallback(GLFWwindow* window, int width, int height)
{
	auto app = reinterpret_cast<HelloTriangle*>(glfwGetWindowUserPointer(window));
	app->m_framebufferResized = true;
}

//Find the first GPU that supports what is needed
void HelloTriangle::pickPhysicalDevice()
{
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(m_instance, &deviceCount, nullptr);

	if (deviceCount == 0)
	{
		throw std::runtime_error("Failed to find a GPU that supports vulkan");
	}

	std::vector<VkPhysicalDevice> devices(deviceCount);
	vkEnumeratePhysicalDevices(m_instance, &deviceCount, devices.data());

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
		throw std::runtime_error("Failed to find a Suitable GPU");
	}

}

bool HelloTriangle::isDeviceSuitable(VkPhysicalDevice device)
{
	//VkPhysicalDeviceProperties deviceProperties;
	//VkPhysicalDeviceFeatures deviceFeatures;
	//vkGetPhysicalDeviceProperties(device, &deviceProperties);
	//vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

	//Specify required features that we need	
	//return deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU && deviceFeatures.geometryShader;


	//Right now all we require is to run vulkan and have the right families
	QueueFamilyIndices indices = findQueueFamilies(device);

	bool extensionSupported = checkDeviceExtensionSupport(device);

	//check for supported swap chain
	bool swapChainAdequate = false;
	//check for support AFTER making sure there is an extention that supports us
	if (extensionSupported)
	{
		SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device);
		//For learing all we care about is having a supported format and presentation mode, not very picky on what it actually is
		swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
	}

	return indices.isComplete() && extensionSupported && swapChainAdequate;
}

bool HelloTriangle::checkDeviceExtensionSupport(VkPhysicalDevice device)
{
	uint32_t extensionCount;
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

	std::vector<VkExtensionProperties> availableExtensions(extensionCount);
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());	

	std::set<std::string> requiredExtensions(m_deviceExtensions.begin(), m_deviceExtensions.end());

	for (const auto& extension : availableExtensions)
	{
		requiredExtensions.erase(extension.extensionName);
	}


	return requiredExtensions.empty();
}

void HelloTriangle::createLogicalDevice()
{
	QueueFamilyIndices indices = findQueueFamilies(m_physicalDevice);

	/*
	VkDeviceQueueCreateInfo queueCreateInfo{};
	queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	queueCreateInfo.queueFamilyIndex = indices.graphicsFamily.has_value();
	queueCreateInfo.queueCount = 1;
	float queuePriority = 1.0f;
	queueCreateInfo.pQueuePriorities = &queuePriority;
	*/
	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	std::set<uint32_t> uniqueQueueFamilies = {indices.graphicsFamily.value(), indices.presentFamily.value()};
	float queuePriority = 1.0f;
	for (uint32_t queuefamily : uniqueQueueFamilies)
	{
		VkDeviceQueueCreateInfo queueCreateInfo{};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = queuefamily;
		queueCreateInfo.queueCount = 1;
		queueCreateInfo.pQueuePriorities = &queuePriority;
		queueCreateInfos.push_back(queueCreateInfo);
	}

	VkPhysicalDeviceFeatures deviceFeatures{};

	VkDeviceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

	createInfo.pQueueCreateInfos = queueCreateInfos.data();
	createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
	createInfo.pEnabledFeatures = &deviceFeatures;

	//createInfo.enabledExtensionCount = 0;

	createInfo.enabledExtensionCount = static_cast<uint32_t>(m_deviceExtensions.size());
	createInfo.ppEnabledExtensionNames = m_deviceExtensions.data();

	if (m_enableValidationLayers)
	{
		createInfo.enabledLayerCount = static_cast<uint32_t>(m_validationLayers.size());
		createInfo.ppEnabledLayerNames = m_validationLayers.data();
	}
	else
	{
		createInfo.enabledLayerCount = 0;
	}

	if (vkCreateDevice(m_physicalDevice, &createInfo, nullptr, &m_device) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create logical device!");
	}

	vkGetDeviceQueue(m_device, indices.graphicsFamily.value(), 0, &m_graphicsQueue);	
	vkGetDeviceQueue(m_device, indices.presentFamily.value(), 0, &m_presentQueue);	
}

QueueFamilyIndices HelloTriangle::findQueueFamilies(VkPhysicalDevice device)
{
	QueueFamilyIndices indices;
	
	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

	int i = 0;
	for (const auto& queueFamily : queueFamilies)
	{
		if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
		{
			indices.graphicsFamily = i;
		}
		//TODO: the presentation family and presentation family may be on different families. Add logic to prefer a family that supports both
		VkBool32 presentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(device, i, m_surface, &presentSupport);

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

//Create the swapchain
void HelloTriangle::createSwapChain()
{
	//SwapChain is like a framebuffer, it queues up the images to be displayed
	//create and query for the supported swap chains
	SwapChainSupportDetails swapChainSupport = querySwapChainSupport(m_physicalDevice);

	//populate the struct with the supported information
	VkSurfaceFormatKHR surfaceFormat = choseSwapChainSurfaceFormat(swapChainSupport.formats);
	VkPresentModeKHR presentMode = choseSwapPresentMode(swapChainSupport.presentModes);
	VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);

	//decide how many images we can hold in the swap chain
	//We do not want to have to wait for driver operations to finish before we can get another
	//image so it is recomended that we request one more than the minimum
	uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;

	//we need to make sure that our request does not exceed the max number of images
	if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount)
	{
		imageCount = swapChainSupport.capabilities.maxImageCount;
	}

	//fill the SwapChain struct with the required info
	VkSwapchainCreateInfoKHR createInfo {};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = m_surface;
	createInfo.minImageCount = imageCount;
	createInfo.imageFormat = surfaceFormat.format;
	createInfo.imageColorSpace = surfaceFormat.colorSpace;
	createInfo.imageExtent = extent;
	createInfo.imageArrayLayers = 1;								//the amount of layers that each image consists of (will almost always be 1)
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;	//dictates how the image is going to be used. In this case we are rendering directly to them

	//We need to specify how to handle the swapchain across multiple queuefamilies
	QueueFamilyIndices indicies = findQueueFamilies(m_physicalDevice);
	uint32_t queueFamilyIndices[] = {indicies.graphicsFamily.value(), indicies.presentFamily.value()};

	
	if (indicies.graphicsFamily != indicies.presentFamily)
	{
		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;		//Images can used across multiple queue families without explicit ownership
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices = queueFamilyIndices;
	}
	else
	{
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;		//Images have onership and need to be explicitly transfered
		createInfo.queueFamilyIndexCount = 0;
		createInfo.pQueueFamilyIndices = nullptr;	
	}

	//we can transform the images in the swap chain
	createInfo.preTransform = swapChainSupport.capabilities.currentTransform;	//we do not want any transform so just give the current transform
	//compsit apha can be used for blending with other windows
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;		//just want to igore any alpha right now
	createInfo.presentMode = presentMode;
	createInfo.clipped = VK_TRUE;			//We do not care about the color of pixles that are obscured
	//It is possible for the swapchain to become invalid over the coarse of the program running
	//such as the window being resized. The chain needs to be remade and the old one needs
	//to be referenced here. For now we will asume that there will only be one
	createInfo.oldSwapchain = VK_NULL_HANDLE;

	//put everything together into a new swapchain
	if (vkCreateSwapchainKHR(m_device, &createInfo, nullptr, &m_swapChain) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create swapchain");
	}

	//Save our format and extent for later use
	m_swapChainImageFormat = surfaceFormat.format;
	m_swapChainExtent = extent;

	//Temporary placement of retreiving images
	//get how many images there are, resize the vector, put images in the vector
	vkGetSwapchainImagesKHR(m_device, m_swapChain, &imageCount, nullptr);
	m_swapChainImages.resize(imageCount);
	vkGetSwapchainImagesKHR(m_device, m_swapChain, &imageCount, m_swapChainImages.data());


}

//Populate the SwapChainSupportDetails struct with information
SwapChainSupportDetails HelloTriangle::querySwapChainSupport(VkPhysicalDevice device)
{
	SwapChainSupportDetails details;

	//checks if the given device supports the surface that is used
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, m_surface, &details.capabilities);

	//get a list of supported formats and add that to the struct
	uint32_t formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_surface, &formatCount, nullptr);

	if (formatCount != 0)
	{
		//resize to make sure that it can hold all the formats
		details.formats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_surface, &formatCount, details.formats.data());
	}

	//get a list of supported presentation modes and add to the struct as well
	//code is similar to getting the list of formats
	uint32_t presentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_surface, &presentModeCount, nullptr);

	if (presentModeCount != 0)
	{
		//again resize to make sure it can hold everything
		details.presentModes.resize(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_surface, &presentModeCount, details.presentModes.data());	//TODO: again track down this warning and fix
	}


	return details;
}

//Swap Extent is the resolution of the images that are in the swap chain. Usually matches the resolution of the window
VkExtent2D HelloTriangle::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities)
{
	//Vulkan works with pixels but GLFW allows for screen coordinates. In certain situations they do not translate
	//very well so we need to use glfwGetFramebufferSize to get the resolution of the window in pixels before we
	//can match it aginast the min/max image extent


	//Normaly the extent matches the window resolution. However we can set our own values if 
	//currentExtent is set to the maximum value of a uint32_t
	if (capabilities.currentExtent.width != UINT32_MAX)
	{
		return capabilities.currentExtent;
	}
	else
	{
		//the current extent is set to the max of uint32_t so we need to make sure that
		//everything is set properly

		//Get the correct pixel resolution of the window
		int width;
		int height;
		glfwGetFramebufferSize(m_window, &width, &height);

		VkExtent2D actualExtent = {
			static_cast<uint32_t>(width),
			static_cast<uint32_t>(height)
		};

		//our actualExtent needs to still fit within the min/max extent so we need to clamp it. Any other values won't work
		actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
		actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);
		
		return actualExtent;
	}

	//return VkExtent2D();
}

//Find the ideal Present Mode
//The Present mode is the condition for swapping the images on screen
VkPresentModeKHR HelloTriangle::choseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes)
{
	//Go through the list of available present modes and see if VK_PRESENT_MODE_MAILBOX_KHR is available
	for (const auto& availablePresentMode : availablePresentModes)
	{
		//VK_PRESENT_MODE_MAILBOX_KHR is similar to VK_PRESENT_MODE_FIFO_KHR except that new images replace the ones in the queue ("triple buffering")
		//Little power hungry, but it is a nice trade off with avoiding tearing and relitivly low latency
		if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
		{
			return availablePresentMode;
		}
	}
	

	//VK_PRESENT_MODE_FIFO_KHR is the only mode that is garanteed
	//VK_PRESENT_MODE_FIFO_KHR swaps the image when the screen refreshes, however if the queue is full the program is forced to wait (similar to v-sync)
	return VK_PRESENT_MODE_FIFO_KHR;
}

//Find the ideal Surface format
VkSurfaceFormatKHR HelloTriangle::choseSwapChainSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
{
	//We are going to target and prioritize the SRGB color format for more acurate colors. It is also the standard for most images

	//go through the available list and see if SRGB is available
	for (const auto& availableFormat : availableFormats)
	{
		//VK_FORMAT_B8G8R8A8_SRGB = 8 bits for Blue | 8 bits for Green | 8 bits for Red | 8 bits for Alpha
		if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)	//VK_COLORSPACE_SRGB_NONLINEAR_KHR is for older versions of the vulkan specification
		{
			//return the desired format
			return availableFormat;
		}
	}

	//if we don't have our prefered format idealy we would rank and chose the "best" format but in mose cases
	//and the fact that this is for learning purposes chosing the first one should be fine
	return availableFormats[0];
}

//Get a list of the Extensions we need
std::vector<const char*> HelloTriangle::getRequiredExtensions()
{
	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions;
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

	if (m_enableValidationLayers)
	{
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	}

	return extensions;
}

//Create a Image View for viewing images
void HelloTriangle::createImageViews()
{
	//In order to view an image we need to create an image view
	//An Image view describes how to access the image and what
	//part to access
	
	//Resize the vector so it can hold all the images
	m_swapChainImageViews.resize(m_swapChainImages.size());

	for(size_t i = 0; i < m_swapChainImages.size(); i++)
	{
		//Time to create more info
		VkImageViewCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		createInfo.image = m_swapChainImages[i];

		createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;	//Specifies how to treat the image
		createInfo.format = m_swapChainImageFormat;		//Specify how the image data should be interpreted. Based of the swap chain image format

		//You can use the components to "swizzle" the color chanels around allowing you to remap color channels
		//We will just be using the defaults here
		createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

		//subresourceRange defines the images purpose and what part will be accessed
		createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		//No mipmapping levels
		createInfo.subresourceRange.baseMipLevel = 0;
		createInfo.subresourceRange.levelCount = 1;
		//No multiple layers
		createInfo.subresourceRange.baseArrayLayer = 0;
		createInfo.subresourceRange.layerCount = 1;

		//Create the Image View
		if (vkCreateImageView(m_device, &createInfo, nullptr, &m_swapChainImageViews[i]) != VK_SUCCESS)
		{
			throw std::runtime_error("Faild to create image views!");
		}

	}
}

void HelloTriangle::recreateSwapchain()
{
	vkDeviceWaitIdle(m_device);

	cleanupSwapchain();

	createSwapChain();
	createImageViews();
	createRenderPass();
	createGraphicsPipeline();
	createFrameBuffer();
	createCommandBuffers();
}

//Configures the debug messaging system
VKAPI_ATTR VkBool32 VKAPI_CALL HelloTriangle::debugCallback(
					VkDebugUtilsMessageSeverityFlagBitsEXT messageSevarity,
					VkDebugUtilsMessageTypeFlagsEXT messageType,
					const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
					void* pUserDate)
{
	std::cerr << "Validation layer: "  << pCallbackData->pMessage << std::endl;

	return VK_FALSE;
}

void HelloTriangle::cleanupSwapchain()
{
	for (size_t i = 0; i < m_swapChainFramebuffers.size(); i++)
	{
		vkDestroyFramebuffer(m_device, m_swapChainFramebuffers[i], nullptr);
	}

	vkFreeCommandBuffers(m_device, m_commandPool, static_cast<uint32_t>(m_commandBuffers.size()), m_commandBuffers.data());

	vkDestroyPipeline(m_device, m_graphicsPipeline, nullptr);
	vkDestroyPipelineLayout(m_device, m_pipelineLayout, nullptr);
	vkDestroyRenderPass(m_device, m_renderPass, nullptr);

	for (size_t i = 0; i < m_swapChainImageViews.size(); i++)
	{
		vkDestroyImageView(m_device, m_swapChainImageViews[i], nullptr);
	}

	vkDestroySwapchainKHR(m_device, m_swapChain, nullptr);
}

void HelloTriangle::createFrameBuffer()
{
	m_swapChainFramebuffers.resize(m_swapChainImageViews.size());

	for (size_t i = 0; i < m_swapChainImageViews.size(); i++)
	{
		VkImageView attachments[] = { m_swapChainImageViews[i] };

		VkFramebufferCreateInfo framebufferInfo{};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = m_renderPass;
		framebufferInfo.attachmentCount = 1;
		framebufferInfo.pAttachments = attachments;
		framebufferInfo.width = m_swapChainExtent.width;
		framebufferInfo.height = m_swapChainExtent.height;
		framebufferInfo.layers = 1;

		if (vkCreateFramebuffer(m_device, &framebufferInfo, nullptr, &m_swapChainFramebuffers[i]) != VK_SUCCESS)
		{
			throw std::runtime_error("Could not create framebuffer");
		}

	}

}



//Create a custum debug messenger
VkResult HelloTriangle::CreateDebugUtilsMessegerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger)
{
	auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
	if (func != nullptr)
	{
		return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
	}
	else
	{
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}
}

//Destroy the custum debug messenger
void HelloTriangle::DestroyDebugUtilsMessegerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator)
{
	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");

	if (func != nullptr)
	{
		func(instance, debugMessenger, pAllocator);
	}
}

void HelloTriangle::populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo)
{
	createInfo = {};
	//createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	//createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	//createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	//createInfo.pfnUserCallback = debugCallback;

	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	createInfo.messageSeverity = m_dbgmsg_msgseverity;
	createInfo.messageType = m_dbgmsg_msgtype;
	createInfo.pfnUserCallback = debugCallback;
}

void HelloTriangle::createGraphicsPipeline()
{

	auto vertShaderCode = ReadFile("..\\shaders\\vert.spv");
	auto fragShaderCode = ReadFile("..\\shaders\\frag.spv");

	VkShaderModule vertShaderModule = createShaderModule(vertShaderCode);
	VkShaderModule fragShaderModule = createShaderModule(fragShaderCode);

	//Create info for the vertex shader
	VkPipelineShaderStageCreateInfo vertShaderCreateInfo{};
	vertShaderCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	//Specify what stage in the graphics pipline this will go
	vertShaderCreateInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
	//Specify the code that will go into the structure and the entry point of the shader program
	vertShaderCreateInfo.module = vertShaderModule;
	vertShaderCreateInfo.pName = "main";

	//Create info for the fragment shader
	//Code is identical to the vertext shader structure
	VkPipelineShaderStageCreateInfo fragShaderCreateInfo{};
	fragShaderCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fragShaderCreateInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	fragShaderCreateInfo.module = fragShaderModule;
	fragShaderCreateInfo.pName = "main";

	//Array to hold the stages to use for the actual creation of the pipeline
	VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderCreateInfo, fragShaderCreateInfo };

	//Because the vertex data is hard coded into the shader we are going to say there is no vertex data for now
	VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
	vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInputInfo.vertexBindingDescriptionCount = 0;
	vertexInputInfo.pVertexBindingDescriptions = nullptr;
	vertexInputInfo.vertexAttributeDescriptionCount = 0;
	vertexInputInfo.pVertexAttributeDescriptions = nullptr;

	//Input assembly structure
	VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
	inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	inputAssembly.primitiveRestartEnable = VK_FALSE;

	//Create viewport structure
	//Viewport specifies where the output will go and the transformation of the buffer to the output
	//Here we go from 0, 0 to width, height
	VkViewport viewport{};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = (float)m_swapChainExtent.width;
	viewport.height = (float)m_swapChainExtent.height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	//Create the Scissor rectangle
	//Scissor is determins where pixels will be stored, anything outside will be ignored
	//In this case we want to render everything
	VkRect2D scissor{};
	scissor.offset = {0, 0};
	scissor.extent = m_swapChainExtent;

	//Put the viewport and the scissor together
	VkPipelineViewportStateCreateInfo viewportState{};
	viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportState.viewportCount = 1;
	viewportState.pViewports = &viewport;
	viewportState.scissorCount = 1;
	viewportState.pScissors = &scissor;

	//Create Rasterizer
	VkPipelineRasterizationStateCreateInfo rasterizer{};
	rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizer.depthClampEnable = VK_FALSE;
	rasterizer.rasterizerDiscardEnable = VK_FALSE;
	rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
	rasterizer.lineWidth = 1.0f;
	rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
	rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
	rasterizer.depthBiasEnable = VK_FALSE;
	rasterizer.depthBiasConstantFactor = 0.0f;
	rasterizer.depthBiasClamp = 0.0f;
	rasterizer.depthBiasSlopeFactor = 0.0f;

	//Create multisampling
	//For simplicity we will leave it disabled
	VkPipelineMultisampleStateCreateInfo multisampling{};
	multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampling.sampleShadingEnable = VK_FALSE;
	multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	multisampling.minSampleShading = 1.0f;
	multisampling.pSampleMask = nullptr;
	multisampling.alphaToCoverageEnable = VK_FALSE;
	multisampling.alphaToOneEnable = VK_FALSE;

	//We do not have any stencil buffers so we will just pass a nullptr

	//Create color blend
	VkPipelineColorBlendAttachmentState colorBlendAttachment{};
	colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	//Basic color blending
	colorBlendAttachment.blendEnable = VK_TRUE;
	colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
	colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
	colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
	colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
	colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
	colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

	VkPipelineColorBlendStateCreateInfo colorBlending{};
	colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlending.logicOpEnable = VK_FALSE;
	colorBlending.logicOp = VK_LOGIC_OP_COPY;
	colorBlending.attachmentCount = 1;
	colorBlending.pAttachments = &colorBlendAttachment;
	
	//We do not have any dynamic states so we will have to use nullptr

	//Create the pipeline layout
	VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = 0;
	pipelineLayoutInfo.pSetLayouts = nullptr;
	pipelineLayoutInfo.pushConstantRangeCount = 0;
	pipelineLayoutInfo.pPushConstantRanges = nullptr;

	if (vkCreatePipelineLayout(m_device, &pipelineLayoutInfo, nullptr, &m_pipelineLayout) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create graphics pipeline");
	}

	VkGraphicsPipelineCreateInfo pipelineInfo{};
	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineInfo.stageCount = 2;
	pipelineInfo.pStages = shaderStages;
	//Reference all the things we have created
	pipelineInfo.pVertexInputState = &vertexInputInfo;
	pipelineInfo.pInputAssemblyState = &inputAssembly;
	pipelineInfo.pViewportState = &viewportState;
	pipelineInfo.pRasterizationState = &rasterizer;
	pipelineInfo.pMultisampleState = &multisampling;
	pipelineInfo.pDepthStencilState = nullptr;
	pipelineInfo.pColorBlendState = &colorBlending;
	pipelineInfo.pDynamicState = nullptr;
	pipelineInfo.layout = m_pipelineLayout;
	pipelineInfo.renderPass = m_renderPass;
	pipelineInfo.subpass = 0;
	pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
	pipelineInfo.basePipelineIndex = -1;

	if (vkCreateGraphicsPipelines(m_device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_graphicsPipeline) != VK_SUCCESS)
	{
		throw std::runtime_error("Could not create graphics pipeline!");
	}



	//We don't need the modules after the pipeline is finished and sent to the gpu so we can get rid of those here
	vkDestroyShaderModule(m_device, fragShaderModule, nullptr);
	vkDestroyShaderModule(m_device, vertShaderModule, nullptr);

}

//Read in file
std::vector<char> HelloTriangle::ReadFile(const std::string& filename)
//static std::vector<char> ReadFile(const std::string& filename)
{
	//ate = start reading at the end of the file | binary = the file is binary
	//reason for start reading at the end of the file is it lets us easily figure out
	//how big the file is and allocate enough room
	std::ifstream file(filename, std::ios::ate | std::ios::binary);

	if (!file.is_open())
	{
		throw std::runtime_error("Failed to open file: " + filename);
	}

	//allocate enough room in the buffer to hold the file
	size_t fileSize = (size_t)file.tellg();
	std::vector<char> buffer(fileSize);

	//Move to the beginning of the file to start reading
	file.seekg(0);
	file.read(buffer.data(), fileSize);

	file.close();

	return buffer;	
}

void HelloTriangle::createRenderPass()
{
	//single color buffer
	VkAttachmentDescription colorAttachment{};
	colorAttachment.format = m_swapChainImageFormat;
	colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	//sub pass
	VkAttachmentReference colorAttachmentRef{};
	colorAttachmentRef.attachment = 0;
	colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpass{};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colorAttachmentRef;


	//Create render pass
	VkRenderPassCreateInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = 1;
	renderPassInfo.pAttachments = &colorAttachment;
	renderPassInfo.subpassCount = 1;
	renderPassInfo.pSubpasses = &subpass;

	VkSubpassDependency dependency{};
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependency.dstSubpass = 0;
	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.srcAccessMask = 0;
	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

	renderPassInfo.dependencyCount = 1;
	renderPassInfo.pDependencies = &dependency;

	if (vkCreateRenderPass(m_device, &renderPassInfo, nullptr, &m_renderPass) != VK_SUCCESS)
	{
		throw std::runtime_error("Could not create render pass");
	}
}

VkShaderModule HelloTriangle::createShaderModule(const std::vector<char>& code)
{
	VkShaderModuleCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = code.size();
	//The bytecode requires bytes but uses a uint32_t pointer rather than a char pointer
	//so we need to recast it to a uint32_t and becuase its in a vector we do not
	//need to worry about things not lining up
	createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

	VkShaderModule shaderModule;
	if (vkCreateShaderModule(m_device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create shader module");
	}

	return shaderModule;
}

//Creates a command pool
void HelloTriangle::createCommandPool()
{
	QueueFamilyIndices queueFamilyIndices = findQueueFamilies(m_physicalDevice);

	VkCommandPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();
	poolInfo.flags = 0;

	if (vkCreateCommandPool(m_device, &poolInfo, nullptr, &m_commandPool) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create command pool");
	}
}

void HelloTriangle::createCommandBuffers()
{
	m_commandBuffers.resize(m_swapChainFramebuffers.size());

	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = m_commandPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = (uint32_t)m_commandBuffers.size();

	if (vkAllocateCommandBuffers(m_device, &allocInfo, m_commandBuffers.data()) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to allocate command buffer");
	}

	//Start recording a command buffer
	for (size_t i = 0; i < m_commandBuffers.size(); i++)
	{
		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = 0;
		beginInfo.pInheritanceInfo = nullptr;

		if (vkBeginCommandBuffer(m_commandBuffers[i], &beginInfo) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to begin recording command buffer");
		}

		//Start render pass
		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = m_renderPass;
		renderPassInfo.framebuffer = m_swapChainFramebuffers[i];
		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = m_swapChainExtent;

		VkClearValue clearColor = { {{0.0f, 0.0f, 0.0f, 1.0f}} };
		renderPassInfo.clearValueCount = 1;
		renderPassInfo.pClearValues = &clearColor;

		vkCmdBeginRenderPass(m_commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
		vkCmdBindPipeline(m_commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, m_graphicsPipeline);
		vkCmdDraw(m_commandBuffers[i], 3, 1, 0, 0);
		vkCmdEndRenderPass(m_commandBuffers[i]);

		if (vkEndCommandBuffer(m_commandBuffers[i]) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to recored command buffer");
		}
	}
}

void HelloTriangle::drawFrame()
{

	vkWaitForFences(m_device, 1, &m_inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

	//Acuire an image from the swap chain
	uint32_t imageIndex;
	VkResult result = vkAcquireNextImageKHR(m_device, m_swapChain, UINT64_MAX, m_imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);

	//Check to make sure that the swapchain is still good and recrate it if not
	if (result == VK_ERROR_OUT_OF_DATE_KHR)
	{
		recreateSwapchain();
		return;
	}
	else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
	{
		throw std::runtime_error("Failed to aquire swap chain image");
	}

	vkResetFences(m_device, 1, &m_inFlightFences[currentFrame]);
	if (m_imagesInFlight[imageIndex] != VK_NULL_HANDLE)
	{
		vkWaitForFences(m_device, 1, &m_imagesInFlight[imageIndex], VK_TRUE, UINT64_MAX);
	}

	m_imagesInFlight[imageIndex] = m_inFlightFences[currentFrame];

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkSemaphore waitSemaphores[] = { m_imageAvailableSemaphores[currentFrame] };
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &m_commandBuffers[imageIndex];

	VkSemaphore signalSemaphores[] = { m_renderFinishedSemaphores[currentFrame] };
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	if (vkQueueSubmit(m_graphicsQueue, 1, &submitInfo, m_inFlightFences[currentFrame]) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to submit draw command buffer");
	}

	VkPresentInfoKHR presentInfo{};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;

	VkSwapchainKHR swapChains[] = {m_swapChain};
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;
	presentInfo.pImageIndices = &imageIndex;

	presentInfo.pResults = nullptr;

	result = vkQueuePresentKHR(m_presentQueue, &presentInfo);
	//Check to make sure the presentation is still good if not recreate
	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || m_framebufferResized)
	{
		m_framebufferResized = false;
		recreateSwapchain();
	}
	else if (result != VK_SUCCESS)
	{
		throw std::runtime_error("failed to present swap chain");
	}

	currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

void HelloTriangle::createSyncObjects()
{
	//We need to use semephores to syncronize function calls to ensure the finish as expected in the correct order
	
	m_imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
	m_renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
	m_inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
	m_imagesInFlight.resize(m_swapChainImages.size(), VK_NULL_HANDLE);

	VkSemaphoreCreateInfo semephoreInfo{};
	semephoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	
	VkFenceCreateInfo fenceInfo{};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
	{
		if (vkCreateSemaphore(m_device, &semephoreInfo, nullptr, &m_imageAvailableSemaphores[i]) != VK_SUCCESS || 
			vkCreateSemaphore(m_device, &semephoreInfo, nullptr, &m_renderFinishedSemaphores[i]) != VK_SUCCESS ||
			vkCreateFence(m_device, &fenceInfo, nullptr, &m_inFlightFences[i]) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create sync objects");
		}

	}
	
}
#include "InitVulkan.h"
#include <stdexcept>

InitVulkan::InitVulkan()
{
	p_instance = nullptr;
	//createInstance();
}

InitVulkan::InitVulkan(VkInstance* inst)
{
	p_instance = inst;
	createInstance();
}

VkInstance* InitVulkan::getInstance()
{
	return p_instance;
}

void InitVulkan::cleanup()
{
	vkDestroyInstance(*p_instance, nullptr);
}

void InitVulkan::createInstance()
{
	//When creating data first you must create a struct that contains the relivent information
	//Every Create requires a struct to be made first
	//Structure for the application info
	VkApplicationInfo appInfo{};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;			//Gives what type of struct this is
	appInfo.pApplicationName = "Hello Triangle";
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "No Engine";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_0;

	//GLFW info needed for the vulkan instance struct
	uint32_t glfwExtentionCount = 0;
	const char** glfwExtentions;
	glfwExtentions = glfwGetRequiredInstanceExtensions(&glfwExtentionCount);

	//Structure for the vulkan instance
	VkInstanceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;
	createInfo.enabledExtensionCount = glfwExtentionCount;
	createInfo.ppEnabledExtensionNames = glfwExtentions;
	createInfo.enabledLayerCount = 0;

	//Create the vulkan instance
	if (vkCreateInstance(&createInfo, nullptr, p_instance) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create vulkan instance");
	}


}
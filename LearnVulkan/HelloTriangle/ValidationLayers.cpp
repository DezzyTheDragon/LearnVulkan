#include "ValidationLayers.h"
#include "VulkanGlobal.h"
#include <cstring>
#include <stdexcept>
#include <iostream>

//Constructor for the class
//Checks to make sure that validation layers are available
ValidationLayers::ValidationLayers()
{
	m_debugMessenger = nullptr;
	//m_instance = nullptr;
	if (enableValidationLayers && !CheckValidationLayerSupport())
	{
		throw std::runtime_error("ValidationLayers: Validation layers requested were not available");
	}
}

//Deconstructor
ValidationLayers::~ValidationLayers()
{
	if (enableValidationLayers)
	{
		DestroyDebugUtilsMessengerEXT(g_vkInstance, m_debugMessenger, nullptr);
	}
}

//Public getter for if validation layers are enabled
bool ValidationLayers::GetEnableValidation()
{
	return enableValidationLayers;
}

/*	The vulkan instance is needed for construction and
*	destruction, but at the time of object creation the
*	instance is not ready so it needs to be set at a
*	later time
*/

//Setter for the vulkan instance
//void ValidationLayers::SetVulkanInstance(VkInstance inst)
//{
//	m_instance = inst;
//}

VkDebugUtilsMessengerEXT ValidationLayers::GetDebugInstance()
{
	return m_debugMessenger;
}

//Public getter for the list of validation layers
std::vector<const char*> ValidationLayers::GetValidationLayers()
{
	return m_validationLayers;
}

//Public getter for required extensions
std::vector<const char*> ValidationLayers::GetRequiredExtensions()
{
	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions;
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

	if (enableValidationLayers)
	{
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	}

	return extensions;
}

//Set up the Debug messenger that we want to use
void ValidationLayers::SetupDebugMessenger()
{
	//If the validation layers are enabled
	//don't set up debug messenger
	if (!enableValidationLayers)
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
	createInfo.pfnUserCallback = debugCallback;*/
	

	//TODO: prefer enum class over enum
	if (CreateDebugUtilsMessengerEXT(g_vkInstance, &createInfo, nullptr, &m_debugMessenger) != VK_SUCCESS)
	{
		throw std::runtime_error("ValidationLayers: failed to setup debug messenger");
	}
}

//This function is handles the debug messaging for the validation layers
VKAPI_ATTR VkBool32 VKAPI_CALL ValidationLayers::debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT msgSeverity, VkDebugUtilsMessageTypeFlagsEXT msgType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData)
{
	std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;
	return VK_FALSE;
}

bool ValidationLayers::CheckValidationLayerSupport()
{
	uint32_t layerCount;
	//TODO: enum class prefered over enum
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

	std::vector<VkLayerProperties> availableLayers(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

	//Iterate through all the validation layers for valid layers
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

//Private function that allows us to lookup the address of the debug messenger create function
VkResult ValidationLayers::CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger)
{
	auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
	if (func != nullptr)
	{
		return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
	}
	else
	{
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}
}

void ValidationLayers::DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator)
{
	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
	if (func != nullptr)
	{
		return func(instance, debugMessenger, pAllocator);
	}
}

//This populates the DebugMessengerCreateInfo with the needed settings
//This allows for resusing when creating other types of DebugMessengers
void ValidationLayers::populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo)
{
	//This is configured to just let everything through for the sake of example
	//Remove VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT from messageSeverity for just warnings and errors
	createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	createInfo.pfnUserCallback = debugCallback;
}

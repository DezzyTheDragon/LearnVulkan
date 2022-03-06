#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vector>

//This class is responsible for setting up the validation layers
//and has functions for interacting with the validation layers
class ValidationLayers
{
public:
	ValidationLayers();
	~ValidationLayers();
	bool GetEnableValidation();
	//void SetVulkanInstance(VkInstance inst);
	VkDebugUtilsMessengerEXT GetDebugInstance();
	std::vector<const char*> GetValidationLayers();
	std::vector<const char*> GetRequiredExtensions();
	void SetupDebugMessenger();
	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
								VkDebugUtilsMessageSeverityFlagBitsEXT msgSeverity, 
								VkDebugUtilsMessageTypeFlagsEXT msgType,
								const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
								void* pUserData);
	void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
private:
	bool CheckValidationLayerSupport();
	VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
								const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);
	void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);
	//member variables
	//List of the validation layers we want
	const std::vector<const char*> m_validationLayers = { 
		"VK_LAYER_KHRONOS_validation" 
	};

	VkDebugUtilsMessengerEXT m_debugMessenger;
	//VkInstance m_instance;

	//If we are debuging enable the validation layers
#ifdef DEBUG
	const bool enableValidationLayers = true;
#else
	const bool enableValidationLayers = false;
#endif // DEBUG

};
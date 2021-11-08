#pragma once
#define GLFW_INCLUDE_VULKAN
#define DEBUG_NEW new(__FILE__, __LINE__)
#define new DEBUG_NEW
#include <GLFW/glfw3.h>
#include <vector>


class HelloTriangle
{
public:
	void run();
private:
	void initWindow();
	void initVulkan();
	void setupDebugMessenger();
	void mainLoop();
	void cleanUp();
	void createInstance();
	bool checkValidationLayerSupport();
	
	//TODO: move messenger functions to their own class to avoid having to rewrite this long mess

	std::vector<const char*> getRequiredExtensions();
	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSevarity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserDate);
	VkResult CreateDebugUtilsMessegerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, 
											const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);
	void DestroyDebugUtilsMessegerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);
	void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);


	GLFWwindow* m_window;
	VkInstance m_instance;
	VkDebugUtilsMessengerEXT m_debugMessager;

	uint32_t m_WIDTH = 800;
	uint32_t m_HEIGHT = 600;

	const std::vector<const char*> m_validationLayers;

	//Only have validation layers enabled for debug builds
#ifdef NDEBUG
	const bool m_enableValidationLayers = false;
#else
	const bool m_enableValidationLayers = true;
#endif // NDEBUG

};


#pragma once
#define GLFW_INCLUDE_VULKAN
#define DEBUG_NEW new(__FILE__, __LINE__)
#define new DEBUG_NEW
#define VK_LAYER_VERBOSE
#include <GLFW/glfw3.h>
#include <vector>
#include <optional>		//NOTE: optional was added in C++17


struct QueueFamilyIndices
{
	//using optional so that we can know if there is or is not a valid queue family that exists. 
	//With just uint32 there is no way to know since 0 could also be a valid entry
	std::optional<uint32_t> graphicsFamily;
	std::optional<uint32_t> presentFamily;

	bool isComplete()
	{
		return graphicsFamily.has_value() && presentFamily.has_value();
	}
};

//Structure to hold onto the details of swapchain support for passing around
struct SwapChainSupportDetails
{
	VkSurfaceCapabilitiesKHR capabilities;
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> presentModes;

};

class HelloTriangle
{
public:
	void run();
private:
	void initWindow();
	void initVulkan();
	void createSurface();
	void setupDebugMessenger();
	void mainLoop();
	void cleanUp();
	void createInstance();
	bool checkValidationLayerSupport();
	
	void pickPhysicalDevice();
	bool isDeviceSuitable(VkPhysicalDevice device);
	bool checkDeviceExtensionSupport(VkPhysicalDevice device);

	void createLogicalDevice();

	QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);

	void createSwapChain();
	SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);
	VkSurfaceFormatKHR choseSwapChainSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
	VkPresentModeKHR choseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
	VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

	void createImageViews();

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

	const std::vector<const char*> m_validationLayers = {"VK_LAYER_KHRONOS_validation"};
	const std::vector<const char*> m_deviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

	VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;
	VkDevice m_device;
	VkQueue m_graphicsQueue;
	VkQueue m_presentQueue;

	VkSurfaceKHR m_surface;

	VkSwapchainKHR m_swapChain;
	std::vector<VkImage> m_swapChainImages;
	VkFormat m_swapChainImageFormat;
	VkExtent2D m_swapChainExtent;
	
	std::vector<VkImageView> m_swapChainImageViews;

	//Only have validation layers enabled for debug builds
#ifdef NDEBUG
	const bool m_enableValidationLayers = false;
#else
	const bool m_enableValidationLayers = true;
#endif // NDEBUG

//Controll how verbose I want the validation layers to make finding error and warning messages easier
#ifdef VK_LAYER_VERBOSE
	int m_dbgmsg_msgseverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	int m_dbgmsg_msgtype = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
#endif // VK_LAYER_VERBOSE
#ifdef VK_LAYER_WARNING
	int m_dbgmsg_msgseverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	int m_dbgmsg_msgtype = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
#endif // VK_LAYER_WARNING
#ifdef VK_LAYER_ERROR
	int m_dbgmsg_msgseverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	int m_dbgmsg_msgtype = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
#endif //VK_LAYER_ERROR

};


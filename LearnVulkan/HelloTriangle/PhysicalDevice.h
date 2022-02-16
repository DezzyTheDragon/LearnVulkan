#pragma once
#define GLFW_INCLUDE_VULKAN
#include <glfw/glfw3.h>
#include <optional>

/*	This class is responsible for handling searching the physical device
*	and finding a suitable device to use.
*	Also responsible for finding apporpriate queue families
* 
*	For structure simplicity it will also be responsible for
*	setting up logical devices and queues
*/

struct QueueFamilyIndices 
{ 
	std::optional<uint32_t> graphicsFamily; 
	bool isComplete() { return graphicsFamily.has_value(); }
};

class PhysicalDevice
{
public:
	//Class c'tor, d'tor, etc
	PhysicalDevice(VkInstance inst);
	~PhysicalDevice();
	//Class Getters
	VkPhysicalDevice GetPhysicalDevice();
	
private:
	//Class functions
	void PickPhysicalDevice();
	bool isDeviceSuitable(VkPhysicalDevice device);
	QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device);
	//member variables
	VkInstance m_instance;
	VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;
};
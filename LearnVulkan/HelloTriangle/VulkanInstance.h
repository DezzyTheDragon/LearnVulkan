#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include "VulkanGlobal.h"
#include "ValidationLayers.h"
#include "PhysicalDevice.h"


/*	The purpose of this class is to create the vulkan instance
*	It will be responsible for creating the instance itself and
*	initializing the needed resources to get it to work
* 
*	Create the instance
*	Setup the debug messenger
*	Create the surface
*	Pick the physical graphics device to use
*	Create the logical device
*	Create the swap chain
*	Create ImageViews
*	Create RenderPass
*	Create Graphics pipeline
*	Create Frame Buffer
*	Create Command pool
*	Create Command buffers
*	create Synced Objects
*/
class VulkanInstance
{
public:
	VulkanInstance();
	~VulkanInstance();
	//VkInstance GetInstance();
private:
	void CreateInstance();
	void CheckExtensions();

	//member variables
	//VkInstance m_instance;						//The vulkan instance
	//GLFWwindow* m_window;
	//Pointers to other objects
	ValidationLayers* m_validationLayers;
	PhysicalDevice* m_physicalDevice;
	//VulkanSurface* m_surface;
};
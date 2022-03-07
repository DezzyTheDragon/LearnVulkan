#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include "VulkanGlobal.h"
#include "ValidationLayers.h"
#include "PhysicalDevice.h"
#include "VulkanGraphicsPipeline.h"


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
private:
	void CreateInstance();
	void CreateImageViews();
	void CheckExtensions();

	//member variables
	
	//Pointers to other objects
	ValidationLayers* m_validationLayers;
	PhysicalDevice* m_physicalDevice;
	VulkanGraphicsPipeline* m_graphicsPipeline;
	std::vector<VkImageView> m_swapChainImageViews;
};
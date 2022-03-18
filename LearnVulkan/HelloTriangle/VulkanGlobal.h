#pragma once
#define GLFW_INLCUDE_VULKAN
#include <glfw/glfw3.h>

//declaration of global variables for objects used in vulkan setup
class VulkanSurface;
class VulkanSwapChain;
class VulkanGraphicsPipeline;

extern GLFWwindow* g_window;							//It is created early and needed in several places
extern VkInstance g_vkInstance;							//It is created early and needed in most classes
extern VkDevice g_device;								//Logical device interfaces with the graphics and is needed anywhere
														//Something is created or destroyed
extern VulkanSurface* g_vkSurface;						//Needed in a few places
extern VulkanSwapChain* g_vkSwapChain;					//Likely needed in other places
extern VulkanGraphicsPipeline* g_vkGraphicsPipeline;	//Pipeline elements needed in other places
extern VulkanCommands* g_vkCommands;					//make commands available

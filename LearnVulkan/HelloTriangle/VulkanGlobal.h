#pragma once
#define GLFW_INLCUDE_VULKAN
#include <glfw/glfw3.h>

//declaration of global variables for objects used in vulkan setup
class VulkanSurface;
class VulkanSwapChain;
//class ValidationLayers;
//class PhysicalDevice;

extern GLFWwindow* g_window;
extern VkInstance g_vkInstance;
extern VulkanSurface* g_vkSurface;
extern VulkanSwapChain* g_vkSwapChain;
//extern VulkanInstance* g_instance;
//extern ValidationLayers* g_validationLayers;
//extern PhysicalDevice* g_physicalDevice;

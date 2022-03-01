#pragma once
//extra defines needed for access to native Windows functions
//#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#include <glfw/glfw3.h>
//#define GLFW_EXPOSE_NATIVE_WIN32
//#include <glfw/glfw3native.h>

/*
*	This class is responsible for setting up the window
*	surface that we want to output to
*/

class VulkanSurface
{
public:
	VulkanSurface(VkInstance inst, GLFWwindow* window);
	~VulkanSurface();
	VkSurfaceKHR GetSurface();
private:
	void CreateSurface();
	//member variables
	VkSurfaceKHR m_surface;
	GLFWwindow* m_window;
	VkInstance m_instance;

};
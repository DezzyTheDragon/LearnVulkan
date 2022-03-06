#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include "VulkanInstance.h"

class HelloTriangleMain
{
public:
	HelloTriangleMain();
	void run();
private:
	void initWindow();
	void initVulkan();
	void mainLoop();
	void cleanup();

	//Class members
	GLFWwindow* m_window;	//Holds the reference to the glfw window
	VulkanInstance* ht_vkInstance;


	//Const Values
	const uint32_t WIDTH = 800;		//Const values for window size
	const uint32_t HEIGHT = 600;	//Const values for window size
};
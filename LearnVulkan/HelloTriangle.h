#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

class HelloTriangle
{
public:
	void run();
private:
	void initWindow();
	void initVulkan();
	void mainLoop();
	void cleanUp();
	void createInstance();

	GLFWwindow* m_window;
	VkInstance instance;
};


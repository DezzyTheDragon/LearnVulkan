#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include "InitVulkan.h"

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
	VkInstance m_instance;	//Holds the vulkan instance for further use

	InitVulkan ht_initVulkan;	//object reference for the init vulkan class

	//Const Values
	const uint32_t WIDTH = 800;		//Const values for window size
	const uint32_t HEIGHT = 600;	//Const values for window size
};
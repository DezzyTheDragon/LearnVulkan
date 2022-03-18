#pragma once
#include "HelloTriangleMain.h"
#include "VulkanGlobal.h"
#include "VulkanSwapChain.h"
#include "VulkanCommands.h"

GLFWwindow* g_window;

//Default constructor
HelloTriangleMain::HelloTriangleMain()
{
	ht_vkInstance = nullptr;
}

//The real start of the render application
void HelloTriangleMain::run()
{
	initWindow();
	initVulkan();
	mainLoop();
	cleanup();
}

//Initilize the GLFW window
//Due to the simplicity it will remain in this class
void HelloTriangleMain::initWindow()
{
	//Initilize the glfw library so we can actually use it
	glfwInit();
	//By default GLFW creates an OpenGL context so we need to explicitly add that we want a Vulkan context
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	//Vulkan is picky with resizing needing swapchains and presentations to be remade, so it will be disabled for now
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	//glfwCreateWindow | width, height, title, window monitor, Ignore because its openGL specific
	g_window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
}

//Initilize Vulkan
void HelloTriangleMain::initVulkan()
{
	ht_vkInstance = new VulkanInstance();
}

//The main loop of the program
//This loop will remain in this class
void HelloTriangleMain::mainLoop()
{
	//Keep the application running untill the window closes or an error occors
	while (!glfwWindowShouldClose(g_window))
	{
		glfwPollEvents();
		drawFrame();
	}
}

//Frees up all the memory that was allocated
//Call the cleanup for each class
//TODO: create a class for automatic memory management (Consider RAII) 
void HelloTriangleMain::cleanup()
{
	//Delete and destroy VulkanInstance
	delete ht_vkInstance;

	//Destroy the glfw window
	glfwDestroyWindow(g_window);
	
	//Terminate glfw, must happen after all the other glfw objects have been cleaned
	glfwTerminate();
}

void HelloTriangleMain::drawFrame()
{
	vkWaitForFences(g_device, 1, ht_vkInstance->GetInFlightFenceRef(), VK_TRUE, UINT64_MAX);
	vkResetFences(g_device, 1, ht_vkInstance->GetInFlightFenceRef());

	uint32_t imageIndex;
	vkAcquireNextImageKHR(g_device, g_vkSwapChain->GetSwapChain(), UINT32_MAX, ht_vkInstance->GetImageAvailableSemaphore(), VK_NULL_HANDLE, &imageIndex);

	vkResetCommandBuffer(g_vkCommands->GetCommandBuffer(), 0);
	g_vkCommands->RecordCommandBuffer(g_vkCommands->GetCommandBuffer(), imageIndex);

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkSemaphore waitSemaphores[] = { ht_vkInstance->GetImageAvailableSemaphore() };
	VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = g_vkCommands->GetCommandBuffer();

	VkSemaphore signalSemaphores[] = { ht_vkInstance->GetRenderFinishedSemaphore() };
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	if (vkQueueSubmit())
	{
	}
}

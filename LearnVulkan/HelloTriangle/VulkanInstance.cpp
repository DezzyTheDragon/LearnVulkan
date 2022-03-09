#pragma once
#include "VulkanInstance.h"
#include "VulkanSurface.h"
#include "VulkanSwapChain.h"
#include "VulkanGlobal.h"
#include <stdexcept>
#include <vector>
#include <iostream>

VkInstance g_vkInstance;
VulkanSurface* g_vkSurface;
VulkanGraphicsPipeline* g_vkGraphicsPipeline;

//Constructor for the class
//The Constructor will go through and create and set everything up
VulkanInstance::VulkanInstance()
{
	m_validationLayers = new ValidationLayers();
	//vulkan setup routine
	CreateInstance();
	m_validationLayers->SetupDebugMessenger();
	//surface can influince the device query and must be run first
	g_vkSurface = new VulkanSurface();
	m_physicalDevice = new PhysicalDevice(m_validationLayers->GetEnableValidation(), m_validationLayers->GetValidationLayers());
	CreateImageViews();
	g_vkGraphicsPipeline = new VulkanGraphicsPipeline(m_physicalDevice->GetLogicalDevice());
	m_frameBuffer = new VulkanFrameBuffer(m_swapChainImageViews, m_physicalDevice->GetLogicalDevice());
}

//Deconstructor
//Will clean up lose ends and is called 
//before this object gets deleted
VulkanInstance::~VulkanInstance()
{
	delete m_frameBuffer;
	delete g_vkGraphicsPipeline;
	for (auto imageView : m_swapChainImageViews)
	{
		vkDestroyImageView(m_physicalDevice->GetLogicalDevice(), imageView, nullptr);
	}
	delete m_physicalDevice;
	delete m_validationLayers;
	delete g_vkSurface;
	//Vulkan instance should be the last thing that is destroyed
	vkDestroyInstance(g_vkInstance, nullptr);
}

//Creates the vulkan instance
void VulkanInstance::CreateInstance()
{
	//Construct the validation layers object for instance structure
	//m_validationLayers = new ValidationLayers(m_instance);
	//Prep the variables we are going to need for the structures
	VkApplicationInfo appInfo{};
	VkInstanceCreateInfo createInfo{};
	VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};

	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions;

	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
	
	//Due to mysterious dangaling pointers the data needs to be copied to a local location
	std::vector<const char*> requiredExtensions;
	for (int i = 0; i < m_validationLayers->GetRequiredExtensions().size(); i++)
	{
		requiredExtensions.push_back(m_validationLayers->GetRequiredExtensions()[i]);
	}

	CheckExtensions();

	//Fill out all the structure data
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "Hello Triangle";
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "No Engine";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_0;

	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;
	createInfo.enabledExtensionCount = static_cast<uint32_t>(m_validationLayers->GetRequiredExtensions().size());
	createInfo.ppEnabledExtensionNames = requiredExtensions.data();
	std::vector<const char*> validationLayers;
	if (m_validationLayers->GetEnableValidation())
	{
		createInfo.enabledLayerCount = static_cast<uint32_t>(m_validationLayers->GetValidationLayers().size());
		
		for (int i = 0; i < m_validationLayers->GetValidationLayers().size(); i++)
		{
			validationLayers.push_back(m_validationLayers->GetValidationLayers()[i]);
		}

		createInfo.ppEnabledLayerNames = validationLayers.data();
		m_validationLayers->populateDebugMessengerCreateInfo(debugCreateInfo);
		createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
	}
	else
	{
		createInfo.enabledLayerCount = 0;
		createInfo.pNext = nullptr;
	}

	//Create the structure
	//TODO: Fix this warning: Enum type "VkResult" is unscoped. Prefer "enum class" over "enum"
	if (vkCreateInstance(&createInfo, nullptr, &g_vkInstance) != VK_SUCCESS)
	{
		throw std::runtime_error("VulkanInstance: Failed to create vulkan instance");
	}
	//m_validationLayers->SetVulkanInstance(g_vkInstance);
}

void VulkanInstance::CreateImageViews()
{
	m_swapChainImageViews.resize(g_vkSwapChain->GetSwapChainImages().size());

	for (size_t i = 0; i < g_vkSwapChain->GetSwapChainImages().size(); i++)
	{
		VkImageViewCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		createInfo.image = g_vkSwapChain->GetSwapChainImages()[i];
		createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		createInfo.format = g_vkSwapChain->GetImageFormat();
		//adjust color chanels
		createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		//define image purpose
		createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		createInfo.subresourceRange.baseMipLevel = 0;
		createInfo.subresourceRange.levelCount = 1;
		createInfo.subresourceRange.baseArrayLayer = 0;
		createInfo.subresourceRange.layerCount = 1;

		if (vkCreateImageView(m_physicalDevice->GetLogicalDevice(), &createInfo, nullptr, &m_swapChainImageViews[i]) != VK_SUCCESS)
		{
			throw std::runtime_error("VulkanInstance: Failed to create image views");
		}
	}
}

//Gets a list of the available extensions
void VulkanInstance::CheckExtensions()
{
	uint32_t extensionCount = 0;
	//TODO: enum class preference to enum warning
	//To allocate an array to hold the extensions we need to know how many there are
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
	std::vector<VkExtensionProperties> extensions(extensionCount);
	//Query for Extension details
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

#ifdef DEBUG
	//If we are in debug mode go ahead and list off the extensions
	std::cout << "Available extensions:\n";
	for (const auto& extension : extensions)
	{
		std::cout << '\t' << extension.extensionName << '\n';
	}
#endif // DEBUG

}

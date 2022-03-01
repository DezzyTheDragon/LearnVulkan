#pragma once
#include "VulkanInstance.h"
#include <stdexcept>
#include <vector>
#include <iostream>

//Constructor for the class
//The Constructor will go through and create and set everything up
VulkanInstance::VulkanInstance(GLFWwindow* window)
{
	//var setup
	m_instance = nullptr;
	m_validationLayers = new ValidationLayers();
	m_window = window;
	//vulkan setup routine
	CreateInstance();
	m_validationLayers->SetupDebugMessenger();
	//surface can influince the device query and must be run first
	m_surface = new VulkanSurface(m_instance, m_window);
	m_physicalDevice = new PhysicalDevice(m_instance, m_validationLayers->GetEnableValidation(), 
				m_validationLayers->GetValidationLayers(), m_surface->GetSurface());
}

//Deconstructor
//Will clean up lose ends and is called 
//before this object gets deleted
VulkanInstance::~VulkanInstance()
{
	delete m_physicalDevice;
	delete m_validationLayers;
	delete m_surface;
	//Vulkan instance should be the last thing that is destroyed
	vkDestroyInstance(m_instance, nullptr);
}

//Returns the instance variable for outside use
VkInstance VulkanInstance::GetInstance()
{
	return m_instance;
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
	if (vkCreateInstance(&createInfo, nullptr, &m_instance) != VK_SUCCESS)
	{
		throw std::runtime_error("VulkanInstance: Failed to create vulkan instance");
	}
	m_validationLayers->SetVulkanInstance(m_instance);
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

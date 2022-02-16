#include "PhysicalDevice.h"
#include <stdexcept>
#include <vector>

PhysicalDevice::PhysicalDevice(VkInstance inst)
{
	m_instance = inst;
	PickPhysicalDevice();
}

PhysicalDevice::~PhysicalDevice()
{
}

//Public getter for the found physical device
VkPhysicalDevice PhysicalDevice::GetPhysicalDevice()
{
	return m_physicalDevice;
}

//Find the desired device
void PhysicalDevice::PickPhysicalDevice()
{
	uint32_t deviceCount = 0;
	//TODO: prefer enum class over enum
	vkEnumeratePhysicalDevices(m_instance, &deviceCount, nullptr);

	//Check to make sure that there are devices that we can use
	if (deviceCount == 0)
	{
		throw std::runtime_error("PhysicalDevice: Failed to find any GPUs with Vulkan support");
	}


	//Create a list containing valid devices
	std::vector<VkPhysicalDevice> devices(deviceCount);
	vkEnumeratePhysicalDevices(m_instance, &deviceCount, devices.data());

	//TODO: rank all the devices and select the best one
	
	//We only care about the first valid device
	for (const auto& device : devices)
	{
		if (isDeviceSuitable(device))
		{
			m_physicalDevice = device;
			break;
		}
	}

	if (m_physicalDevice == VK_NULL_HANDLE)
	{
		throw std::runtime_error("PhysicalDevice: Failed to find suitable GPU");
	}

}

//Check if the given device has what we need
bool PhysicalDevice::isDeviceSuitable(VkPhysicalDevice device)
{
	QueueFamilyIndices indices = FindQueueFamilies(device);

	return indices.isComplete();
}

QueueFamilyIndices PhysicalDevice::FindQueueFamilies(VkPhysicalDevice device)
{
	QueueFamilyIndices indices;

	uint32_t familyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &familyCount, nullptr);
	std::vector<VkQueueFamilyProperties> queueFamilies(familyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &familyCount, queueFamilies.data());

	int i = 0;
	//Find the first queue family that has VK_QUEUE_GRAPHICS_BIT
	for (const auto& queueFamily : queueFamilies)
	{
		if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
		{
			indices.graphicsFamily = i;
		}

		if (indices.isComplete())
		{
			break;
		}
		i++;
	}

	return indices;
}

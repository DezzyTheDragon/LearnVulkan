#include "VulkanSurface.h"
#include <stdexcept>

VulkanSurface::VulkanSurface(VkInstance inst, GLFWwindow* window)
{
	m_window = window;
	m_instance = inst;
	CreateSurface();
}

VulkanSurface::~VulkanSurface()
{
	vkDestroySurfaceKHR(m_instance, m_surface, nullptr);
}

VkSurfaceKHR VulkanSurface::GetSurface()
{
	return m_surface;
}



void VulkanSurface::CreateSurface()
{
	//TODO: enum class prefered over enum
	if (glfwCreateWindowSurface(m_instance, m_window, nullptr, &m_surface) != VK_SUCCESS)
	{
		throw std::runtime_error("VulkanSurface: failed to create windows surface");
	}
}

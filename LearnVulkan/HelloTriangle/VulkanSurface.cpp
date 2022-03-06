#include "VulkanSurface.h"
#include "VulkanGlobal.h"
#include <stdexcept>

VulkanSurface::VulkanSurface()
{
	//m_window = window;
	//m_instance = inst;
	CreateSurface();
}

VulkanSurface::~VulkanSurface()
{
	vkDestroySurfaceKHR(g_vkInstance, m_surface, nullptr);
}

VkSurfaceKHR VulkanSurface::GetSurface()
{
	return m_surface;
}



void VulkanSurface::CreateSurface()
{
	//TODO: enum class prefered over enum
	if (glfwCreateWindowSurface(g_vkInstance, g_window, nullptr, &m_surface) != VK_SUCCESS)
	{
		throw std::runtime_error("VulkanSurface: failed to create windows surface");
	}
}

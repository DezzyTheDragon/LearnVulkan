#include "VulkanFrameBuffer.h"
#include "VulkanSwapChain.h"
#include "VulkanGraphicsPipeline.h"
#include "VulkanGlobal.h"
#include <stdexcept>

VulkanFrameBuffer::VulkanFrameBuffer(std::vector<VkImageView> imageViews)
{
	m_imageViews = imageViews;
	//m_device = device;
	CreateFrameBuffer();
}

VulkanFrameBuffer::~VulkanFrameBuffer()
{
	for (auto framebuffer : m_swapChainFrameBuffers)
	{
		vkDestroyFramebuffer(g_device, framebuffer, nullptr);
	}
}

std::vector<VkFramebuffer> VulkanFrameBuffer::GetSwapChainFrameBuffers()
{
	return m_swapChainFrameBuffers;
}

std::vector<VkImageView> VulkanFrameBuffer::GetImageViews()
{
	return m_imageViews;
}

void VulkanFrameBuffer::CreateFrameBuffer()
{
	m_swapChainFrameBuffers.resize(m_imageViews.size());

	for (size_t i = 0; i < m_imageViews.size(); i++)
	{
		VkImageView attachments[] = {
			m_imageViews[i]
		};

		VkFramebufferCreateInfo frameBufferInfo{};
		frameBufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		frameBufferInfo.renderPass = g_vkGraphicsPipeline->GetRenderPass();
		frameBufferInfo.attachmentCount = 1;
		frameBufferInfo.pAttachments = attachments;
		frameBufferInfo.width = g_vkSwapChain->GetExtent().width;
		frameBufferInfo.height = g_vkSwapChain->GetExtent().height;
		frameBufferInfo.layers = 1;

		if (vkCreateFramebuffer(g_device, &frameBufferInfo, nullptr, &m_swapChainFrameBuffers[i]) != VK_SUCCESS)
		{
			throw std::runtime_error("VulkanFrameBuffer: failed to create frame buffer");
		}
	}
}

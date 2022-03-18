#include "VulkanCommands.h"
#include "VulkanGlobal.h"
#include "VulkanGraphicsPipeline.h"
#include "VulkanSwapChain.h"
#include <stdexcept>

VulkanCommands::VulkanCommands(PhysicalDevice *physicalDevice, VulkanFrameBuffer *frameBuffer) : m_physicalDevice(physicalDevice), m_frameBuffer(frameBuffer)
{
	CreateCommandPool();
	CreateCommandBuffer();
}

VulkanCommands::~VulkanCommands()
{
	vkDestroyCommandPool(m_physicalDevice->GetLogicalDevice(), m_commandPool, nullptr);
}

VkCommandPool VulkanCommands::GetCommandPool()
{
	return m_commandPool;
}

VkCommandBuffer VulkanCommands::GetCommandBuffer()
{
	return m_commandBuffer;
}

void VulkanCommands::RecordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex)
{
	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = 0;
	beginInfo.pInheritanceInfo = nullptr;

	if (vkBeginCommandBuffer(m_commandBuffer, &beginInfo))
	{
		throw std::runtime_error("VulkanCommands: Failed to begin command buffer");
	}

	//Starting render pass
	VkRenderPassBeginInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass = g_vkGraphicsPipeline->GetRenderPass();
	renderPassInfo.framebuffer = m_frameBuffer->GetSwapChainFrameBuffers()[imageIndex];
	renderPassInfo.renderArea.offset = { 0, 0 };
	renderPassInfo.renderArea.extent = g_vkSwapChain->GetExtent();

	VkClearValue clearColor = { {{0.0f, 0.0f, 0.0f, 1.0f}} };
	renderPassInfo.clearValueCount = 1;
	renderPassInfo.pClearValues = &clearColor;

	vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, g_vkGraphicsPipeline->GetGraphicsPipeline());
	vkCmdDraw(commandBuffer, 3, 1, 0, 0);
	vkCmdEndRenderPass(commandBuffer);

	if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
	{
		throw std::runtime_error("VulkanCommand: Failed to record command buffer");
	}
}

void VulkanCommands::CreateCommandPool()
{
	QueueFamilyIndices queueFamilyIndices = m_physicalDevice->FindQueueFamilies(m_physicalDevice->GetPhysicalDevice());

	VkCommandPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

	if (vkCreateCommandPool(m_physicalDevice->GetLogicalDevice(), &poolInfo, nullptr, &m_commandPool))
	{
		throw std::runtime_error("VulkanCommands: Failed to create command pool");
	}
}

void VulkanCommands::CreateCommandBuffer()
{
	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = m_commandPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = 1;

	if (vkAllocateCommandBuffers(m_physicalDevice->GetLogicalDevice(), &allocInfo, &m_commandBuffer))
	{
		throw std::runtime_error("VulkanCommands: Failed to allocate command buffers");
	}
}

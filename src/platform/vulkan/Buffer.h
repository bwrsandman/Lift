#pragma once

#include "VulkanError.h"
#include "DeviceMemory.h"

namespace Vulkan
{
	class CommandPool;
	class Device;

	class Buffer final
	{
	public:

		VULKAN_NON_COPIABLE(Buffer)

		Buffer(const Device& device, size_t size, VkBufferUsageFlags usage);
		~Buffer();

		const class Device& Device() const { return device_; }

		DeviceMemory AllocateMemory(VkMemoryPropertyFlags properties);
		VkMemoryRequirements GetMemoryRequirements() const;

		void CopyFrom(CommandPool& commandPool, const Buffer& src, VkDeviceSize size);

	private:

		const class Device& device_;

		VULKAN_HANDLE(VkBuffer, buffer_)
	};

}
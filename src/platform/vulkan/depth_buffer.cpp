#include <stdexcept>
#include <core.h>
#include "DepthBuffer.h"
#include "command_pool.h"
#include "Device.h"
#include "DeviceMemory.h"
#include "Image.h"
#include "ImageView.h"

namespace Vulkan {

	namespace
	{
		VkFormat FindSupportedFormat(const Device& device, const std::vector<VkFormat>& candidates, const VkImageTiling tiling, const VkFormatFeatureFlags features)
		{
			for (auto format : candidates)
			{
				VkFormatProperties props;
				vkGetPhysicalDeviceFormatProperties(device.PhysicalDevice(), format, &props);

				if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features)
				{
					return format;
				}

				if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features)
				{
					return format;
				}
			}

			LF_ASSERT(std::runtime_error("failed to find supported format"));
		}

		VkFormat FindDepthFormat(const Device& device)
		{
			return FindSupportedFormat(
				device,
				{ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
				VK_IMAGE_TILING_OPTIMAL,
				VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
			);
		}
	}

	DepthBuffer::DepthBuffer(CommandPool& commandPool, const VkExtent2D extent) :
		format_(FindDepthFormat(commandPool.Device()))
	{
		const auto& device = commandPool.Device();

		image_.reset(new Image(device, extent, format_, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT));
		imageMemory_.reset(new DeviceMemory(image_->AllocateMemory(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)));
		imageView_.reset(new class ImageView(device, image_->Handle(), format_, VK_IMAGE_ASPECT_DEPTH_BIT));

		image_->TransitionImageLayout(commandPool, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
	}

	DepthBuffer::~DepthBuffer()
	{
		imageView_.reset();
		image_.reset();
		imageMemory_.reset(); // release memory after bound image has been destroyed
	}

}
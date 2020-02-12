
#include "image.h"
#include "buffer.h"
#include "depth_buffer.h"
#include "device.h"
#include "single_time_commands.h"
#include "vulkan/vulkan.hpp"

namespace vulkan {

Image::Image(const class Device& device, const VkExtent2D extent, const VkFormat format) :
    Image(device,
          extent,
          format,
          VK_IMAGE_TILING_OPTIMAL,
          VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT) {
}

Image::Image(const class Device& device,
             const VkExtent2D extent,
             const VkFormat format,
             const VkImageTiling tiling,
             const VkImageUsageFlags usage) :
    device_(device), extent_(extent), format_(format), image_layout_(vk::ImageLayout::eUndefined) {

    VkImageCreateInfo image_info = {};
    image_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    image_info.imageType = VK_IMAGE_TYPE_2D;
    image_info.extent.width = extent.width;
    image_info.extent.height = extent.height;
    image_info.extent.depth = 1;
    image_info.mipLevels = 1;
    image_info.arrayLayers = 1;
    image_info.format = format;
    image_info.tiling = tiling;
    image_info.initialLayout = static_cast<VkImageLayout>(image_layout_);
    image_info.usage = usage;
    image_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    image_info.samples = VK_SAMPLE_COUNT_1_BIT;
    image_info.flags = 0; // Optional

    vulkanCheck(vkCreateImage(device.handle(), &image_info, nullptr, &image_),
                "create image");
}

Image::Image(Image&& other) noexcept
    : device_(other.device_),
      extent_(other.extent_),
      format_(other.format_),
      image_layout_(other.image_layout_),
      image_(other.image_) {

    other.image_ = nullptr;
}

Image::~Image() {
    if (image_ != nullptr) {
        vkDestroyImage(device_.handle(), image_, nullptr);
        image_ = nullptr;
    }
}

DeviceMemory Image::allocateMemory(const VkMemoryPropertyFlags properties) const {
    const auto requirements = getMemoryRequirements();
    DeviceMemory memory(device_, requirements.size, requirements.memoryTypeBits, properties);

    vulkanCheck(vkBindImageMemory(device_.handle(), image_, memory.handle(), 0),
                "bind image memory");

    return memory;
}

VkMemoryRequirements Image::getMemoryRequirements() const {
    VkMemoryRequirements requirements;
    vkGetImageMemoryRequirements(device_.handle(), image_, &requirements);
    return requirements;
}

void Image::transitionImageLayout(CommandPool &command_pool, vk::ImageLayout new_layout) {
    SingleTimeCommands::submit(command_pool, [&](VkCommandBuffer command_buffer) {
        vk::ImageMemoryBarrier barrier;
        barrier.oldLayout = image_layout_;
        barrier.newLayout = new_layout;
        barrier.image = image_;
        barrier.setSubresourceRange({vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1});

        if(new_layout == vk::ImageLayout::eDepthStencilAttachmentOptimal)
            barrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eDepth;

        barrier.srcAccessMask = accessFlagsForLayout(image_layout_);
        barrier.dstAccessMask = accessFlagsForLayout(new_layout);

        vk::PipelineStageFlags src_stage_flags = pipelineStageForLayout(image_layout_);
        vk::PipelineStageFlags dst_stage_flags = pipelineStageForLayout(new_layout);
        vk::CommandBuffer vkCmdBuffer{command_buffer};


        vkCmdBuffer.pipelineBarrier(src_stage_flags,
                                    dst_stage_flags,
                                    vk::DependencyFlags(),
                                    nullptr,
                                    nullptr,
                                    barrier);
    });

    image_layout_ = new_layout;
}

//void Image::transitionImageLayout(CommandPool& command_pool, VkImageLayout new_layout) {
//    SingleTimeCommands::submit(command_pool, [&](VkCommandBuffer command_buffer) {
//       VkImageMemoryBarrier barrier = {};
//       barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
//       barrier.oldLayout = image_layout_;
//       barrier.newLayout = new_layout;
//       barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
//       barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
//       barrier.image = image_;
//       barrier.subresourceRange.baseMipLevel = 0;
//       barrier.subresourceRange.levelCount = 1;
//       barrier.subresourceRange.baseArrayLayer = 0;
//       barrier.subresourceRange.layerCount = 1;
//
//       if (new_layout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
//           barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
//
//           if (DepthBuffer::hasStencilComponent(format_)) {
//               barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
//           }
//       } else {
//           barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
//       }
//
//       VkPipelineStageFlags source_stage = 0;
//       VkPipelineStageFlags destination_stage = 0;
//
//       if (image_layout_ == VK_IMAGE_LAYOUT_UNDEFINED && new_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
//           barrier.srcAccessMask = 0;
//           barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
//
//           source_stage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
//           destination_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
//       } else if (image_layout_ == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
//           && new_layout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
//           barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
//           barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
//
//           source_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
//           destination_stage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
//       } else if (image_layout_ == VK_IMAGE_LAYOUT_UNDEFINED
//           && new_layout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
//           barrier.srcAccessMask = 0;
//           barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT |
//               VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
//
//           source_stage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
//           destination_stage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
//       } else if (image_layout_ == VK_IMAGE_LAYOUT_UNDEFINED &&
//           new_layout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL) {
//           barrier.srcAccessMask = 0;
//           barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
//
//           source_stage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
//           destination_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
//       } else {
//           barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
//           barrier.dstAccessMask = (VkAccessFlags)vk::AccessFlags();
//
//           source_stage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
//           destination_stage = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
//           LF_ERROR("unsupported layout transition");
//       }
//
//       vkCmdPipelineBarrier(command_buffer, source_stage, destination_stage, 0, 0, nullptr, 0, nullptr, 1, &barrier);
//   }
//    );
//
//    image_layout_ = new_layout;
//}

void Image::copyFromBuffer(CommandPool& command_pool, const vk::Buffer& buffer) {
    SingleTimeCommands::submit(command_pool, [&](VkCommandBuffer command_buffer) {
        vk::BufferImageCopy region;
        region.setImageSubresource({vk::ImageAspectFlagBits::eColor, 0, 0, 1});
        region.setImageOffset({0, 0, 0});
        region.setImageExtent(vk::Extent3D(extent_, 1));

        vk::CommandBuffer cmdBuf{command_buffer};
        cmdBuf.copyBufferToImage(buffer,
                                 image_,
                                 vk::ImageLayout::eTransferDstOptimal,
                                 region);
    });
}

void Image::copyToBuffer(CommandPool& command_pool, const vk::Buffer& buffer) {

    SingleTimeCommands::submit(command_pool, [&](VkCommandBuffer command_buffer) {

        vk::BufferImageCopy region;
        region.setImageSubresource({vk::ImageAspectFlagBits::eColor, 0, 0, 1});
        region.setImageExtent(vk::Extent3D(extent_, 1));

        vk::CommandBuffer cmdBuf{command_buffer};
        cmdBuf.copyImageToBuffer(image_,
                                 vk::ImageLayout::eTransferSrcOptimal,
                                 buffer,
                                 1,
                                 &region);
    });

}

vk::AccessFlags Image::accessFlagsForLayout(vk::ImageLayout layout) {
    switch (layout) {
        case vk::ImageLayout::ePreinitialized:
            return vk::AccessFlagBits::eHostWrite;
        case vk::ImageLayout::eTransferDstOptimal:
            return vk::AccessFlagBits::eTransferWrite;
        case vk::ImageLayout::eTransferSrcOptimal:
            return vk::AccessFlagBits::eTransferRead;
        case vk::ImageLayout::eColorAttachmentOptimal:
            return vk::AccessFlagBits::eColorAttachmentWrite;
        case vk::ImageLayout::eDepthStencilAttachmentOptimal:
            return vk::AccessFlagBits::eDepthStencilAttachmentWrite;
        case vk::ImageLayout::eShaderReadOnlyOptimal:
            return vk::AccessFlagBits::eShaderRead;
        default:
            return vk::AccessFlags();
    }
}

vk::PipelineStageFlags Image::pipelineStageForLayout(vk::ImageLayout layout) {
    switch (layout) {
        case vk::ImageLayout::eTransferDstOptimal:
        case vk::ImageLayout::eTransferSrcOptimal:
            return vk::PipelineStageFlagBits::eTransfer;
        case vk::ImageLayout::eColorAttachmentOptimal:
            return vk::PipelineStageFlagBits::eColorAttachmentOutput;
        case vk::ImageLayout::eDepthStencilAttachmentOptimal:
            return vk::PipelineStageFlagBits::eEarlyFragmentTests;
        case vk::ImageLayout::eShaderReadOnlyOptimal:
            return vk::PipelineStageFlagBits::eFragmentShader;
        case vk::ImageLayout::ePreinitialized:
            return vk::PipelineStageFlagBits::eHost;
        case vk::ImageLayout::eUndefined:
            return vk::PipelineStageFlagBits::eTopOfPipe;
        default:
            return vk::PipelineStageFlagBits::eBottomOfPipe;
    }

}

}

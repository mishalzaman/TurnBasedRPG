#pragma once
#include "UniformBuffers.h"

WGPU::Buffer::UniformBuffer::UniformBuffer():
	totalSize_(0), uniformBuffer_(nullptr)
{
}

WGPU::Buffer::UniformBuffer::~UniformBuffer()
{
    if (uniformBuffer_) {
        std::cout << "WGPU::Buffer::UniformBuffer::~UniformBuffer - Releasing uniform buffers..." << std::endl;
        wgpuBufferRelease(uniformBuffer_);
    }
}
void WGPU::Buffer::UniformBuffer::Write()
{
    if (!Core::Device() || !Core::Queue()) {
        throw std::runtime_error("WebGPU device or queue not initialized.");
    }

    if (bufferData_.empty()) {
        return; // No data to write
    }

    // Create the buffer only if it doesn't exist or if the size has changed
    if (!uniformBuffer_ || bufferDesc_.size != totalSize_) {
        // Create a new buffer descriptor
        bufferDesc_ = {};
        bufferDesc_.nextInChain = nullptr;
        bufferDesc_.size = totalSize_; // Match total data size
        bufferDesc_.usage = WGPUBufferUsage_Uniform | WGPUBufferUsage_CopyDst;
        bufferDesc_.mappedAtCreation = false;

        if (uniformBuffer_) {
            wgpuBufferRelease(uniformBuffer_); // Release old buffer
        }

        uniformBuffer_ = wgpuDeviceCreateBuffer(Core::Device(), &bufferDesc_);
        if (!uniformBuffer_) {
            throw std::runtime_error("Failed to create uniform buffer.");
        }
    }

    // Write updated values to the GPU buffer
    for (const auto& data : bufferData_) {
        // Serialize each updated value into a temporary buffer
        std::vector<uint8_t> tempBuffer(data.size, 0);

        std::visit([&](auto&& arg) {
            using ValueType = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<ValueType, float> || std::is_same_v<ValueType, int> || std::is_same_v<ValueType, uint32_t>) {
                std::memcpy(tempBuffer.data(), &arg, data.size);
            }
            else if constexpr (std::is_same_v<ValueType, std::array<float, 2>> ||
                std::is_same_v<ValueType, std::array<float, 3>> ||
                std::is_same_v<ValueType, std::array<float, 4>> ||
                std::is_same_v<ValueType, std::array<float, 8>> ||
                std::is_same_v<ValueType, std::array<float, 12>> ||
                std::is_same_v<ValueType, std::array<float, 16>>) {
                std::memcpy(tempBuffer.data(), arg.data(), data.size);
            }
            }, data.value);

        // Write the serialized data into the GPU buffer at the correct offset
        wgpuQueueWriteBuffer(Core::Queue(), uniformBuffer_, data.offset, tempBuffer.data(), data.size);
    }
}

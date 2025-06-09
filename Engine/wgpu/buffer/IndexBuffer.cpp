#include "IndexBuffer.h"

WGPU::Buffer::IndexBuffer::IndexBuffer(
    std::vector<uint16_t> indices,
    unsigned int indexCount,
    WGPUDevice device,
    WGPUQueue queue
) :
    indexCount_(indexCount)
{
    WGPUBufferDescriptor bufferDesc{};
    bufferDesc.nextInChain = nullptr;
    bufferDesc.size = indices.size() * sizeof(uint16_t);
    bufferDesc.usage = WGPUBufferUsage_CopyDst | WGPUBufferUsage_Index;
    indexBuffer_ = wgpuDeviceCreateBuffer(device, &bufferDesc);

    wgpuQueueWriteBuffer(queue, indexBuffer_, 0, indices.data(), bufferDesc.size);
}

WGPU::Buffer::IndexBuffer::~IndexBuffer()
{
    std::cout << "Releasing index buffer" << std::endl;
    wgpuBufferRelease(indexBuffer_);
}

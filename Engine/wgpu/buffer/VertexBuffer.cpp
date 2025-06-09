#include "VertexBuffer.h"
#include "core/Core.h"

WGPU::Buffer::VertexBuffer::VertexBuffer(
    std::vector<float> vertices,
	unsigned int vertexCount,
    WGPUDevice device,
    WGPUQueue queue
):
    vertexCount_(vertexCount)
{
    WGPUBufferDescriptor bufferDesc{};
    bufferDesc.nextInChain = nullptr;
    bufferDesc.size = vertices.size() * sizeof(float);
    bufferDesc.usage = WGPUBufferUsage_CopyDst | WGPUBufferUsage_Vertex;
    bufferDesc.mappedAtCreation = false;
    vertexBuffer_ = wgpuDeviceCreateBuffer(device, &bufferDesc);

    wgpuQueueWriteBuffer(queue, vertexBuffer_, 0, vertices.data(), bufferDesc.size);
}

WGPU::Buffer::VertexBuffer::~VertexBuffer()
{
    std::cout << "Releasing vertex buffer" << std::endl;
    wgpuBufferRelease(vertexBuffer_);
}

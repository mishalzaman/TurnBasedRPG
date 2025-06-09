#pragma once

#include <webgpu/webgpu.h>
#include <vector>
#include <iostream>

namespace WGPU::Buffer{
	class VertexBuffer {
	public:
		VertexBuffer(
			std::vector<float> vertices,
			unsigned int vertexCount,
			WGPUDevice device,
			WGPUQueue queue
		);
		~VertexBuffer();
		WGPUBuffer GetBuffer() const { return vertexBuffer_; }
	private:
		WGPUBuffer vertexBuffer_;
		int vertexCount_;
	};
}